/*************************************************************
* 文 件 名: rpt515.c
* 功能描述：小额支付类来包清单
*
* 作    者: xyy
* 完成日期: 2006-10-17 16:25
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define   EXTERN
#include "public.h"
#include "lv_lbctl_c.h"
#include "com_sys_parm_c.h"
#include "com_branch_c.h"

static struct lv_lbctl_c lv_lbctl;
static struct com_sys_parm_c com_sys_parm;

int page=0;
int line=0;
int line_file=0;
char vpfm[21];
char t_str[41];
int opncnt;
FILE    *fp; 
int ret = 0;

int get_ratio_lvlbctl(int bli,int bll,char str[100]);
extern char *cGetLvPkgDes(char *stat,char *desc);
extern char *cGetLvCmnoDes(char *cmtno,char *desc);

rpt515()
{
	int ttlnum;
	int cnt=0;
	long qry_date = 0;
	
	char filename[100];
	char wherelist[1024];  /**查询条件**/
	char tmpstr[300];

	char old_br_no[6];
	memset( vpfm     ,0,sizeof(vpfm));
	memset( t_str    ,0,sizeof(t_str));
	memset( filename ,0,sizeof(filename));
	memset( tmpstr   ,0,sizeof(tmpstr));
	memset( wherelist,0,sizeof(wherelist));
	memset( &lv_lbctl , 0x00 , sizeof(struct lv_lbctl_c));
	memset( &com_sys_parm , 0x00 , sizeof(struct com_sys_parm_c));
	
	strcpy( old_br_no , "*****" );
	/*计算交易日期*/
	
	g_reply_int = Com_sys_parm_Sel( g_pub_tx.reply , &com_sys_parm , "sys_date=%ld" , g_pub_tx.tx_date );
	if( g_reply_int ) 
	{ 
		sprintf(acErrMsg, "SELECT com_sys_parm error![%d]", g_reply_int ); 
		WRITEMSG 
		goto ErrExit; 
	}
  qry_date = com_sys_parm.lst_date;
   
	ret=sql_begin(); /*打开事务*/
	if(ret != 0)
	{
		sprintf( acErrMsg, "打开事务失败!!!" );
		WRITEMSG
		goto ErrExit;
	}

	/**------- 初始化全局变量 --------**/
	pub_base_sysinit();
	
	strcpy( filename , "RPT515" );	
	strcpy(vpfm, "rpt515" );

	sprintf( tmpstr," in_date ='%ld' ", qry_date);
	strcat( wherelist,tmpstr );	

	strcat(wherelist," order by br_no,sts,packid ");
	vtcp_log( "[%s][%d]WHERE = [%s]",__FILE__,__LINE__,wherelist);
	
	ret=Lv_lbctl_Dec_Sel( g_pub_tx.reply, wherelist );

	ttlnum=0;
	
	while(1)
	{
		memset( &lv_lbctl , 0x00 , sizeof(struct lv_lbctl_c));
		ret=Lv_lbctl_Fet_Sel( &lv_lbctl, g_pub_tx.reply );
		if( ret==100 ) 
		{
			if( ttlnum )
			{
				/* 打页尾 */
				ret = print_tail();
				ERR_DEAL
				/*fprintf(fp,"%c%c%c%c\n",0x1c,0x53,0x00,0x00);*/
				fclose(fp);
			}
			break;	
		}else if( ret ){
			sprintf(acErrMsg,"读取lv_lbctl表异常出错!");
			WRITEMSG			
			goto ErrExit;			
		}
					
		/* 机构变换时 */
		pub_base_strpack( lv_lbctl.br_no );
		if( strcmp( lv_lbctl.br_no , old_br_no ) )
		{
			/* 第一个机构 */
			if( strcmp( old_br_no , "*****" ) )
			{
				/* 打印页尾 */
				ret = print_tail();
				ERR_DEAL
				fclose(fp);
			}
			
			strcpy( old_br_no , lv_lbctl.br_no );
			vtcp_log("[%s][%d]br_no=[%s]",__FILE__,__LINE__,lv_lbctl.br_no);
			page=1;
			line_file=1;
			
			/* 删除原文件 */
			ret = pub_rpt_rmfile( lv_lbctl.br_no , filename , 0 );
			ERR_DEAL
			/* 打开文件 */	
			ret = pub_rpt_openfile( &fp , lv_lbctl.br_no , filename );
			ERR_DEAL
			fprintf(fp,"%c%c\n",0x1b,0x40);/*0607 martin*/
			/***fprintf(fp,"%c%c",0x1b,0x40);
                         fprintf(fp,"%c%c%c%c%c\n",0x1b,0x40,0x1b,0x33,0x18);
                         fprintf(fp,"%c%c%c%c\n",0x1c,0x53,0x00,0x00);***/
			/* 打印页首 */
			ret = print_head();
			ERR_DEAL
		}
    		/* 统计信息 */
    cnt = sql_count("lv_lbctl"," in_date ='%ld' and br_no='%s' ",qry_date,old_br_no);
    vtcp_log("[%s][%d]机构号为[%s]的总共[%d]笔当前打印第[%d]笔",__FILE__,__LINE__,old_br_no,cnt,line_file);
    		/* 打印信息 */		
		ret = print_body();
		ERR_DEAL
		vtcp_log("[%s][%d]line_file=[%d]",__FILE__,__LINE__,line_file);
		line_file++;
		
		/*if( line_file>50 )	 大于50行换页 del by martin 070519*/
		if( line_file>42 )	/* 大于42行换页 */
		{
			line_file=0;
			page++;
			/* 打印页尾 */
			ret = print_tail();
			ERR_DEAL
			/* 打印页首 */
			ret = print_head();
			ERR_DEAL
		}
		ttlnum++;
	}
	Lv_lbctl_Clo_Sel();
	
OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"查询小额支付交易来包成功!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--事务回滚--*/
	sprintf(acErrMsg,"查询小额支付交易来包失败!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

/* 中间函数 */
int Make_yeb_sub( char vrec[3] )
{	
	vtcp_log("[%s][%d]Make_yeb_sub begin",__FILE__,__LINE__);
	ret = pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001",vrec,&opncnt,get_ratio_lvlbctl);
	ERR_DEAL
	
GoodExit:
	return 0;
ErrExit:
	return 1;
}

/* 打印表首 */
int print_head( )
{
	char vrec[3];
	
	strcpy( vrec,"HH" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL
	
	strcpy( vrec,"AA" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL
		
GoodExit:
	return 0;
ErrExit:
	return 1;
}
int print_body( )
{
	char vrec[3];
  vtcp_log("[%s][%d]print_body begin",__FILE__,__LINE__);
	strcpy( vrec,"BB" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL
	
GoodExit:
	return 0;
ErrExit:
	return 1;
}
/* 打印表尾 */
int print_tail( )
{
	char vrec[3];

	strcpy( vrec,"CC" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL
	fprintf(fp,"\f");	
GoodExit:
	return 0;
ErrExit:
	return 1;
}


/* 赋值函数 */
int get_ratio_lvlbctl( bli,bll,str )
int bli,bll;
char str[100];
{
	char vstr[101]; 
	char amt_tmp[21]; 
	char cTx_Desc[17];
	char fmt[10]; 
	int i=0; 
	struct com_branch_c sCom_branch;

	memset(&sCom_branch, 0x0, sizeof(struct com_branch_c));
	memset(cTx_Desc,0x00,	sizeof(cTx_Desc));
	
	switch( bli ) 
	{ 
		case 'A': /* 交易日期 */
			sprintf( t_str , "%4d年%2d月%2d日" , lv_lbctl.in_date/10000 , lv_lbctl.in_date%10000/100 ,lv_lbctl.in_date%100 );
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , t_str ); 
			vtcp_log("[%s][%d]get_ratio_lvpkglb begin in_date=[%s]",__FILE__,__LINE__,str);
			break;
		case 'B': /* 机构编码 */
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , lv_lbctl.br_no ); 
			vtcp_log("[%s][%d]get_ratio_lvpkglb begin br_no=[%s]",__FILE__,__LINE__,str);
			break; 
		case 'C': /* 机构名称 */
			ret = Com_branch_Sel(g_pub_tx.reply, &sCom_branch, "br_no='%s'",
		  lv_lbctl.br_no);
			if (ret != 0)
			{
				sprintf(acErrMsg, "查询公共机构编码表错误! [%d]", ret);
				WRITEMSG
			}
			pub_base_strpack( sCom_branch.br_name ); 
			sprintf( t_str , "%s    ( %5s )" , sCom_branch.br_name , lv_lbctl.br_no); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			vtcp_log("[%s][%d]get_ratio_lvpkglb begin br_name=[%s]",__FILE__,__LINE__,str);
			break;
		case 'D': /* 页码 */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, page); 
			vtcp_log("[%s][%d]get_ratio_lvpkglb begin page=[%d]",__FILE__,__LINE__,str);
			break; 
		case 'E': /* PKGNO 
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str, fmt, lv_lbctl.pkgno);
			vtcp_log("[%s][%d]get_ratio_lvpkglb begin pkgno=[%s]",__FILE__,__LINE__,str);
			break;
			**/
			vtcp_log("[%s][%d]开始打印--交易状态!\n",__FILE__,__LINE__);
			memset(cTx_Desc,0x00,sizeof(cTx_Desc));
			cGetLvCmnoDes(lv_lbctl.pkgno,cTx_Desc);
			cDeal_str(cTx_Desc, bll);
			memcpy(str,cTx_Desc,bll);
			break;
		case 'F': /* 包序号 */						
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str, fmt, lv_lbctl.packid); 
			vtcp_log("[%s][%d]get_ratio_lvpkglb begin packid=[%s]",__FILE__,__LINE__,str);
			break;
		case 'G': /* 包日期 */
			sprintf(fmt,"%%-%dld",bll); 
			sprintf( str, fmt, lv_lbctl.pack_date);
			vtcp_log("[%s][%d]get_ratio_lvpkglb begin pack_date=[%ld]",__FILE__,__LINE__,str);
			break;	
		case 'H': /* 付款清算行 */
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str, fmt, lv_lbctl.pay_qs_no);
			vtcp_log("[%s][%d]get_ratio_lvpkglb begin pay_qs_no=[%s]",__FILE__,__LINE__,str);
			break;
		case 'I': /* 接收清算行 */
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str, fmt, lv_lbctl.cash_qs_no);
			vtcp_log("[%s][%d]get_ratio_lvpkglb begin cash_qs_no=[%s]",__FILE__,__LINE__,str);
			break;
		case 'J': /* 总金额 */
			/* 加逗号 */
			sprintf( amt_tmp, "%.2lf" , lv_lbctl.totamt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			vtcp_log("[%s][%d]get_ratio_lvpkglb begin totamt=[%.2lf]",__FILE__,__LINE__,str);
			break;
		case 'K': /* 总笔数 */
			sprintf(fmt,"%%-%dld",bll); 
			sprintf( str, fmt, lv_lbctl.dtlcnt);
			vtcp_log("[%s][%d]get_ratio_lvpkglb begin dtlcnt=[%ld]",__FILE__,__LINE__,str);
			break;		
		case 'L': /* 成功总金额 */
			/* 加逗号 */
			sprintf( amt_tmp, "%.2lf" , lv_lbctl.dtlsucamt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			vtcp_log("[%s][%d]get_ratio_lvpkglb begin dtlsucamt=[%.2lf]",__FILE__,__LINE__,str);
			break;
		case 'M': /* 成功总笔数 */
			sprintf(fmt,"%%-%dld",bll); 
			sprintf( str, fmt, lv_lbctl.dtlsuccnt);
			vtcp_log("[%s][%d]get_ratio_lvpkglb begin dtlsuccnt=[%ld]",__FILE__,__LINE__,str);
			break;	
		case 'N': /* 包处理状态 
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str, fmt, lv_lbctl.packstat);
			vtcp_log("[%s][%d]get_ratio_lvpkglb begin packstat=[%s]",__FILE__,__LINE__,str);
			break;
			**/
			vtcp_log("[%s][%d]开始打印--交易状态!\n",__FILE__,__LINE__);
			memset(cTx_Desc,0x00,sizeof(cTx_Desc));
			cGetLvPkgDes(lv_lbctl.packstat,cTx_Desc);
			cDeal_str(cTx_Desc, bll);
			memcpy(str,cTx_Desc,bll);
			break;
		case 'O': /* 本系统包状态 
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str, fmt, lv_lbctl.sts);
			vtcp_log("[%s][%d]get_ratio_lvpkglb begin sts=[%s]",__FILE__,__LINE__,str);
			break;	
			**/
			vtcp_log("[%s][%d]开始打印--交易状态!\n",__FILE__,__LINE__);
			memset(cTx_Desc,0x00,sizeof(cTx_Desc));
			cGetLvPkg2Des(lv_lbctl.sts,cTx_Desc);
			cDeal_str(cTx_Desc, bll);
			memcpy(str,cTx_Desc,bll);
			break;
		default : 
			memset( str,' ',bll ); 
			break; 
	}
	
GoodExit: 
	return 0; 
ErrExit: 
	return 1; 
}
