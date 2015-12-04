/*************************************************************
* 文 件 名: rpt511.c
* 功能描述：小额支付类来帐清单
*
* 作    者: xyy
* 完成日期: 2006-10-16 20:10
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
#include "lv_pkgreg_c.h"
#include "com_sys_parm_c.h"
#include "com_branch_c.h"

static struct lv_pkgreg_c lv_pkgreg;
static struct com_sys_parm_c com_sys_parm;

int page=0;
int line=0;
int line_file=0;
char vpfm[21];
char t_str[41];
int opncnt;
FILE    *fp; 
int ret = 0;

int get_ratio_lvpkglz(int bli,int bll,char str[100]);
extern char *cGetLvCmnoDes(char *cmtno,char *desc);
extern char *cGetLvzfStat(char stat,char *desc);

rpt511()
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
	memset( &lv_pkgreg , 0x00 , sizeof(struct lv_pkgreg_c));
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
	
	strcpy( filename , "RPT511" );	
	strcpy(vpfm, "rpt511" );

	sprintf( tmpstr," in_date ='%ld' and lw_ind='2'  " , qry_date);
	strcat( wherelist,tmpstr );	

	strcat(wherelist," order by br_no,orderno");
	vtcp_log( "[%s][%d]WHERE = [%s]",__FILE__,__LINE__,wherelist);
	
	ret=Lv_pkgreg_Dec_Sel( g_pub_tx.reply, wherelist );

	ttlnum=0;
	
	while(1)
	{
		memset( &lv_pkgreg , 0x00 , sizeof(struct lv_pkgreg_c));
		vtcp_log("[%s][%d] line_file=[%d] ret=[%d]",__FILE__,__LINE__,line_file,ret);
		ret=Lv_pkgreg_Fet_Sel( &lv_pkgreg, g_pub_tx.reply );
		vtcp_log("[%s][%d] line_file=[%d] ret=[%d]",__FILE__,__LINE__,line_file,ret);
		if( ret==100 )
		{
			if( !ttlnum )
			{
				/* 打页尾 */
				ret = print_tail();
				ERR_DEAL
				fprintf(fp,"%c%c%c%c\n",0x1c,0x53,0x00,0x00);
				fclose(fp);
			}
			fclose(fp);
			break;	
		}else if( ret ){
			sprintf(acErrMsg,"读取小额来帐明细表异常出错!");
			WRITEMSG			
			goto ErrExit;			
		}
	  vtcp_log("[%s][%d] here line_file ",__FILE__,__LINE__,line_file);
		/* 机构变换时 */
		pub_base_strpack( lv_pkgreg.br_no );
		if( strcmp( lv_pkgreg.br_no , old_br_no ) )
		{
			/* 第一个机构 */
			if( strcmp( old_br_no , "*****" ) )
			{
				/* 打印页尾 */
				ret = print_tail();
				ERR_DEAL
				fclose(fp);
			}
			
			strcpy( old_br_no , lv_pkgreg.br_no );
			vtcp_log("[%s][%d]br_no=[%s]",__FILE__,__LINE__,lv_pkgreg.br_no);
			page=1;
			line_file=1;
			
			/* 删除原文件 */
			ret = pub_rpt_rmfile( lv_pkgreg.br_no , filename , 0 );
			ERR_DEAL
			/* 打开文件 */	
			ret = pub_rpt_openfile( &fp , lv_pkgreg.br_no , filename );
			ERR_DEAL
			 fprintf(fp,"%c%c",0x1b,0x40);
                         fprintf(fp,"%c%c%c%c%c\n",0x1b,0x40,0x1b,0x33,0x18);
                         fprintf(fp,"%c%c%c%c\n",0x1c,0x53,0x00,0x00);
			/* 打印页首 */
			ret = print_head();
			ERR_DEAL
		}
    		/* 统计信息 **/
    cnt = sql_count("lv_pkgreg"," in_date='%ld' and br_no ='%s' and lw_ind='2' ",qry_date,old_br_no);
    vtcp_log("[%s][%d]机构号为[%s]的总共[%d]笔当前打印第[%d]笔",__FILE__,__LINE__,old_br_no,cnt,line_file);
    
    		/* 打印信息 */		
		ret = print_body();
		ERR_DEAL
		vtcp_log("[%s][%d]line_file=[%d]",__FILE__,__LINE__,line_file);
		line_file++;
		vtcp_log("[%s][%d]line_file=[%d]",__FILE__,__LINE__,line_file);
		/* if( line_file>8 ) del by martin 070517 大于8行换页 */
		if( line_file>7 )	/* 大于7行换页 */
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
		vtcp_log("[%s][%d]line_file=[%d]",__FILE__,__LINE__,line_file);
		ttlnum++;
		vtcp_log("[%s][%d]line_file=[%d]",__FILE__,__LINE__,line_file);
	}
	Lv_pkgreg_Clo_Sel();
	
OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"查询小额支付交易来帐明细成功!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--事务回滚--*/
	sprintf(acErrMsg,"查询小额支付交易来帐明细失败!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

/* 中间函数 */
int Make_yeb_sub( char vrec[3] )
{	
	vtcp_log("[%s][%d]Make_yeb_sub begin",__FILE__,__LINE__);
	ret = pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001",vrec,&opncnt,get_ratio_lvpkglz);
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
	if (page != 1)
	{
		fprintf(fp, "\f");
	}
	strcpy( vrec,"TT" );
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
	strcpy( vrec,"HH" );
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
	/*fprintf(fp,"\f");*/
GoodExit:
	return 0;
ErrExit:
	return 1;
}


/* 赋值函数 */
int get_ratio_lvpkglz( bli,bll,str )
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
			sprintf( t_str , "%4d年%2d月%2d日" , lv_pkgreg.in_date/10000 , lv_pkgreg.in_date%10000/100 ,lv_pkgreg.in_date%100 );
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , t_str ); 
			vtcp_log("[%s][%d]get_ratio_lvpkglz begin in_date=[%s]",__FILE__,__LINE__,str);
			break;
		case 'B': /* 机构编码 */
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , lv_pkgreg.br_no ); 
			vtcp_log("[%s][%d]get_ratio_lvpkglz begin br_no=[%s]",__FILE__,__LINE__,str);
			break; 
		case 'C': /* 机构名称 */
			ret = Com_branch_Sel(g_pub_tx.reply, &sCom_branch, "br_no='%s'",
		  lv_pkgreg.br_no);
			if (ret != 0)
			{
				sprintf(acErrMsg, "查询公共机构编码表错误! [%d]", ret);
				WRITEMSG
			}
			pub_base_strpack( sCom_branch.br_name ); 
			sprintf( t_str , "%s    ( %5s )" , sCom_branch.br_name , lv_pkgreg.br_no); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			vtcp_log("[%s][%d]get_ratio_lvpkglz begin br_name=[%s]",__FILE__,__LINE__,str);
			break;
		case 'D': /* 页码 */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, page); 
			vtcp_log("[%s][%d]get_ratio_lvpkglz begin page=[%d]",__FILE__,__LINE__,str);
			break; 
		case 'E': /* PKGNO */
			/**
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str, fmt, lv_pkgreg.pkgno);
			vtcp_log("[%s][%d]get_ratio_lvpkglz begin pkgno=[%s]",__FILE__,__LINE__,str);
			break;
			****/
			vtcp_log("[%s][%d]开始打印--交易状态!\n",__FILE__,__LINE__);
			memset(cTx_Desc,0x00,sizeof(cTx_Desc));
			cGetLvCmnoDes(lv_pkgreg.pkgno,cTx_Desc);
			cDeal_str(cTx_Desc, bll);
			memcpy(str,cTx_Desc,bll);
			break;
		case 'F': /* 交易序号 */						
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str, fmt, lv_pkgreg.orderno); 
			vtcp_log("[%s][%d]get_ratio_lvpkglz begin orderno=[%s]",__FILE__,__LINE__,str);
			break;
		case 'G': /* 发起行行号 */
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str, fmt, lv_pkgreg.or_br_no);
			vtcp_log("[%s][%d]get_ratio_lvpkglz begin or_br_no=[%s]",__FILE__,__LINE__,str);
			break;
		case 'H': /* 付款人帐号 */
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str, fmt, lv_pkgreg.pay_ac_no);
			vtcp_log("[%s][%d]get_ratio_lvpkglz begin pay_ac_no=[%s]",__FILE__,__LINE__,str);
			break;
		case 'I': /* 付款人名称 */
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str, fmt, lv_pkgreg.pay_name);
			vtcp_log("[%s][%d]get_ratio_lvpkglz begin pay_name=[%s]",__FILE__,__LINE__,str);
			break;
		case 'J': /* 接收行行号 */
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str, fmt, lv_pkgreg.ac_br_no); 
			vtcp_log("[%s][%d]get_ratio_lvpkglz begin ac_br_no=[%s]",__FILE__,__LINE__,str);
			break;
		case 'K': /* 交易金额 */
			/* 加逗号 */
			sprintf( amt_tmp, "%.2lf" , lv_pkgreg.tx_amt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			vtcp_log("[%s][%d]get_ratio_lvpkglz begin tx_amt=[%.2lf]",__FILE__,__LINE__,str);
			break;	
		case 'L': /* 收款人帐号 */
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str, fmt, lv_pkgreg.cash_ac_no);
			vtcp_log("[%s][%d]get_ratio_lvpkglz begin cash_ac_no=[%s]",__FILE__,__LINE__,str);
			break;
		case 'M': /* 收款人名称 */
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str, fmt, lv_pkgreg.cash_name);
			vtcp_log("[%s][%d]get_ratio_lvpkglz begin cash_name=[%s]",__FILE__,__LINE__,str);
			break;
		case 'N': /* 包序号 */
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str, fmt, lv_pkgreg.packid);
			vtcp_log("[%s][%d]get_ratio_lvpkglz begin packid=[%s]",__FILE__,__LINE__,str);
			break;
		case 'O': /* 包日期 */
			sprintf(fmt,"%%-%dld",bll); 
			sprintf( str, fmt, lv_pkgreg.pack_date);
			vtcp_log("[%s][%d]get_ratio_lvpkglz begin pack_date=[%ld]",__FILE__,__LINE__,str);
			break;
		case 'P': /* 记录状态 
			sprintf(fmt,"%%-%dld",bll); 
			sprintf( str, fmt, lv_pkgreg.lv_sts);
			vtcp_log("[%s][%d]get_ratio_lvpkgjj begin pack_date=[%ld]",__FILE__,__LINE__,str);
			break;
			**/
			vtcp_log("[%s][%d]开始打印--交易状态!\n",__FILE__,__LINE__);
			memset(cTx_Desc,0x00,sizeof(cTx_Desc));
			vtcp_log("[%s][%d]lv_sts===[%c]",__FILE__,__LINE__,lv_pkgreg.lv_sts[0]);
			cGetLvzfStat(lv_pkgreg.lv_sts[0],cTx_Desc);
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
