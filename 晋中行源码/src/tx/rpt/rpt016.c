/*************************************************************
* 文 件 名: rpt016.c
* 功能描述：对公销户清单
*
* 作    者: lance
* 完成日期: 2003年07月13日
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
#define EXTERN
#include "public.h"
#include "com_sys_parm_c.h"
#include "com_branch_c.h"
#include "mo_opn_cls_c.h"

struct com_sys_parm_c	s_com_sys_parm;
struct mo_opn_cls_c	s_mo_opn_cls;

int page=0;
int line=0;
int line_file=0;
char vpfm[21];
char t_str[41];
int opncnt;
FILE    *fp; 
int ret = 0;


int get_ratio_dgxhqd(int bli,int bll,char str[100]);

rpt016()
{
	int flag=0;
	int ttlnum;
	
	long min_date;
	long max_date;

	char fname[100];
	char filename[100];
	char wherelist[1024];  /**查询条件**/
	char tmpstr[100];

	char tmp_str[60];
	char old_br_no[6];
	
	memset( wherelist,0,sizeof(wherelist) );
	memset( &s_com_sys_parm , 0x00 , sizeof(struct com_sys_parm_c));
	memset( &s_mo_opn_cls , 0x00 , sizeof(struct mo_opn_cls_c));
			
	strcpy( old_br_no , "*****" );
		
   
	ret=sql_begin(); /*打开事务*/
	if(ret != 0)
	{
		sprintf( acErrMsg, "打开事务失败!!!" );
		WRITEMSG
		goto ErrExit;
	}

	/**------- 初始化全局变量 --------**/
	pub_base_sysinit();
/*	
	strcpy( filename , "BMdgxhqd" );	
	strcpy(vpfm, "dgxhqd" );
*/	
	strcpy( filename , "RPT016");
	strcpy(vpfm, "RPT016");
	/* 条件: 交易日期为上一交易日期  的一个月 */
	ret = Com_sys_parm_Sel( g_pub_tx.reply , &s_com_sys_parm , "sys_date=%ld" , g_pub_tx.tx_date );
	ERR_DEAL
	min_date = s_com_sys_parm.lst_date/100*100+0;
	max_date = s_com_sys_parm.lst_date/100*100+99;
	sprintf( tmpstr," tx_date>%ld and tx_date<%ld and " , min_date , max_date );
	strcat( wherelist,tmpstr );

	/* 条件: 对公账户 */
	strcpy( tmpstr," cif_type in('2','3','4','6') and " );
	strcat( wherelist,tmpstr );

	/* 条件: 销户 */
	strcpy( tmpstr," wrk_sts='2' and " );
	strcat( wherelist,tmpstr );

	/**组成查询**/	
	strcat(wherelist," 1=1 ORDER BY opn_br_no,tx_brno,prdt_no,ac_no,ac_id");

	vtcp_log( "WHERE [%s]",wherelist );

	ret = Mo_opn_cls_Dec_Sel( g_pub_tx.reply, wherelist );
	ERR_DEAL

	ttlnum=0;
	
	while(1)
	{
		ret = Mo_opn_cls_Fet_Sel( &s_mo_opn_cls, g_pub_tx.reply );
		if( ret==100 ) 
		{
			if( ttlnum )
			{
				/* 打页尾 */
				ret = print_tail();
				ERR_DEAL
				fclose(fp);
			}
			break;
		}else if( ret ){
			sprintf(acErrMsg,"读取流水信息表异常出错!");
			WRITEMSG			
			goto ErrExit;			
		}
		
		/* 机构变换时 */
		pub_base_strpack(s_mo_opn_cls.opn_br_no );
		if( strcmp( s_mo_opn_cls.opn_br_no , old_br_no ) )
		{
			/* 第一个机构 */
			if( strcmp( old_br_no , "*****" ) )
			{
				/* 打印页尾 */
				ret = print_tail();
				ERR_DEAL
				fclose(fp);
			}
			
			strcpy( old_br_no , s_mo_opn_cls.opn_br_no );
			
			page=1;
			line_file=1;
			
			/* 删除原文件 */
			ret = pub_rpt_rmfile(s_mo_opn_cls.opn_br_no , filename , 0 );
			ERR_DEAL
			/* 打开文件 */	
			ret = pub_rpt_openfile( &fp, s_mo_opn_cls.opn_br_no , filename );
			ERR_DEAL
			/* 打印页首 */
			ret = print_head();
			ERR_DEAL
		}
    		/* 打印信息 */
		ret = print_body();
		ERR_DEAL
		line_file++;
		if( line_file>10 )	/* 大于50行换页 */
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
	Mo_opn_cls_Clo_Sel();
	
OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"生成对公销户交易清单成功!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--事务回滚--*/
	sprintf(acErrMsg,"生成对公销户流水清单失败!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

/* 中间函数 */
int Make_yeb_sub( char vrec[3] )
{	
	ret=pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001",vrec,&opncnt,get_ratio_dgxhqd);
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
	
	if( page != 1 )
		{
			fprintf ( fp , "\f" );
		}
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


/* 打印表尾 */
int print_tail( )
{
	char vrec[3];

	strcpy( vrec,"CC" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL
	/****
	fprintf(fp,"\n");
	fprintf(fp,"@HY@");
	fprintf(fp,"\n");
	**********/
GoodExit:
	return 0;
ErrExit:
	return 1;
}


/* 打印表体 */
int print_body( )
{
	char vrec[3];

	strcpy( vrec,"BB" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL
	
GoodExit:
	return 0;
ErrExit:
	return 1;
}


/* 赋值函数 */
int get_ratio_dgxhqd( bli,bll,str )
int bli,bll;
char str[100];
{ 
	char vhm[101]; 
	char vstr[101]; 
	char l_kmm[31];
	char amt_tmp[21]; 
	char tmp_inf[21];
	char fmt[10]; 
	int i=0; 
	struct com_branch_c sCom_branch;

	memset(&sCom_branch, 0x0, sizeof(struct com_branch_c));
	
	switch( bli ) 
	{ 
		case 'A': /* 交易日期 */
			sprintf( t_str , "%4d年%2d月%2d日" , s_mo_opn_cls.tx_date/10000 , s_mo_opn_cls.tx_date%10000/100 , s_mo_opn_cls.tx_date%100 );
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , t_str ); 
			break;
		case 'B': /* 机构编码 */
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , s_mo_opn_cls.opn_br_no ); 
			break; 
		case 'C': /* 机构名称 */
			ret = Com_branch_Sel(g_pub_tx.reply, &sCom_branch, "br_no='%s'",
		s_mo_opn_cls.opn_br_no);
			if (ret != 0)
			{
				sprintf(acErrMsg, "查询公共机构编码表错误! [%d]", ret);
				WRITEMSG
				goto ErrExit;
			}
			pub_base_strpack( sCom_branch.br_name ); 
			sprintf( t_str , "%s    ( %5s )" , sCom_branch.br_name , s_mo_opn_cls.opn_br_no); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'D': /* 页码 */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, page); 
			break; 
		case 'E': /* 账     号 */
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt , s_mo_opn_cls.ac_no); 
			break; 
		case 'F': /* 客户名称 */
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt , s_mo_opn_cls.name);
			break;
		case 'G': /* 销  户  金  额 */
			/* 加逗号 */
			sprintf( amt_tmp, "%.2lf" , s_mo_opn_cls.amt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
							
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'H': /* 产品 */
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt , s_mo_opn_cls.prdt_no);
			break;
		case 'I': /* 交易机构 */
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt , s_mo_opn_cls.tx_brno);
			break;
		case 'J': /* 交易柜员 */
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt , s_mo_opn_cls.tel);
			break;
		case 'K': /* 销户日期 */			
			sprintf(fmt,"%%%dld",bll);	
			sprintf( str, fmt, s_mo_opn_cls.tx_date ); 
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
