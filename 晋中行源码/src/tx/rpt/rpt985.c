/*************************************************************
* 文 件 名:  rpt985.c
* 功能描述： 承兑汇票到期清单
* 作    者:  
* 完成日期:  2003年08月15日
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#define EXTERN   
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include "public.h"
#include "com_sys_parm_c.h"
#include "mo_bank_acc_po_c.h"
#define  PAGE_CNT   43

struct mo_bank_acc_po_c	s_mo_bank_acc_po;
int flag=0;								/*** 1-页末 ***/
int page=0;
int line=0;
int line_file=0;
char vpfm[21];
char t_str[41];
int opncnt;
FILE *fp; 
int ret = 0;
double  amt=0.00;
double  all_amt=0.00;

int get_ratio_lsqd(int bli,int bll,char str[100]);

rpt985( )
{
	int    ttlnum;
	char   filename[100];
	char   old_br_no[6];
	long   tmp_date;

	struct com_sys_parm_c s_com_sys_parm;

	memset(&s_com_sys_parm, 0x00, sizeof(struct com_sys_parm_c)); 
         
	ret=sql_begin(); /*打开事务*/
	if( ret )
	{
		sprintf( acErrMsg, "打开事务失败!!!" );
		WRITEMSG
		goto ErrExit;
	}

	strcpy( old_br_no, "AAAAA" );

	/* 初始化全局变量 */
	pub_base_sysinit();
    
	/* 取报表打印日期 */
	ret = Com_sys_parm_Sel( g_pub_tx.reply, &s_com_sys_parm, "1=1" );
	ERR_DEAL

	g_pub_tx.tx_date = s_com_sys_parm.sys_date;
/*
	strcpy( filename , "BRcdhpdqqd" );	
	strcpy( vpfm, "cdhpdqqd" );
*/
	strcpy( filename , "RPT985");
	strcpy( vpfm, "RPT985");
	ret = Mo_bank_acc_po_Dec_Sel( g_pub_tx.reply, " matr_date=%ld  ORDER BY tx_br_no,req_date,payer_ac_no ", g_pub_tx.tx_date );
	ERR_DEAL
	
	ttlnum=0;
	
	while(1)
	{
		ret = Mo_bank_acc_po_Fet_Sel( &s_mo_bank_acc_po,g_pub_tx.reply );
		if( ret==100 ) 
		{
			if( ttlnum )
			{
				/* 打页尾 */
				ret = print_tail( );
				ERR_DEAL
				fclose(fp);
			}
			else 
			{
				sprintf( acErrMsg, "本日没有要打印的流水!" );
				WRITEMSG
			}
			break;
		}
		else if( ret )
		{
			sprintf(acErrMsg,"读取流水信息表异常出错!");
			WRITEMSG			
			goto ErrExit;			
		}

		pub_base_strpack( s_mo_bank_acc_po.tx_br_no );
		pub_base_strpack( old_br_no );

		/* 机构变换时 */
		if( strcmp( s_mo_bank_acc_po.tx_br_no,old_br_no ) )
		{
			if( strcmp(old_br_no,"AAAAA") )			
			{
				/* 打印页尾 */
				ret = print_tail( );
				ERR_DEAL
				fclose(fp);
			}

			strcpy( old_br_no,s_mo_bank_acc_po.tx_br_no );
			
			page = 1;
			line_file = 1;
			
			/* 删除原文件 */
			ret = pub_rpt_rmfile( s_mo_bank_acc_po.tx_br_no , filename , 0 );
			ERR_DEAL

			/* 打开文件 */	
			ret = pub_rpt_openfile( &fp, s_mo_bank_acc_po.tx_br_no , filename );
			ERR_DEAL

			/* 打印页首 */
			ret = print_head( );
			ERR_DEAL
		}

    	/* 打印表体信息 */
		ret = print_body( );
		ERR_DEAL

		line_file++;

		if( line_file>PAGE_CNT )	
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

	Mo_bank_acc_po_Clo_Sel( );
	fclose(fp);
	
OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"日报表生成处理成功!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--事务回滚--*/
	sprintf(acErrMsg,"日报表生成处理失败!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

/* 中间函数 */
int Make_yeb_sub( char vrec[3] )
{	
	ret=pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001",vrec,&opncnt,get_ratio_lsqd);
	ERR_DEAL
	
GoodExit:
	return 0;
ErrExit:
	return 1;
}

/* 表内换页打印表首 */
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

/* 页末打印表尾 */
int print_tail( )
{
	char vrec[3];

	strcpy( vrec,"CC" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL
GoodExit:
	return 0;
ErrExit:
	return 1;
}
/* 表内打印表体 */
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
/* 说明：bli 代表变量字符名称 ... bll代表变量占的长度 ... str代表变量值 */
int get_ratio_lsqd( bli,bll,str )
int bli,bll;
char str[100];
{ 
	struct mdm_ac_rel_c s_mdm_ac_rel;

	long cif_no;
	char vhm[101]; 
	char vstr[101]; 
	char l_kmm[41];
	char amt_tmp[41]; 
	char tmp_inf[41];
	char tmp_str[41];
	char fmt[20]; 
	char br_name[71];
	char prdt_name[31];
	char ac_name[51];
	double tmp_amt;
	int i=0; 

	switch( bli ) 
	{ 
		case 'A': /* 交易日期 */ 
			sprintf( t_str , "%4d年%2d月%2d日" , g_pub_tx.tx_date/10000 , g_pub_tx.tx_date%10000/100 , g_pub_tx.tx_date%100 );
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , t_str ); 
			break;
		case 'C': /* 机构名称 */
			if(strlen(s_mo_bank_acc_po.tx_br_no))	/** gyf.20060815 **/
			ret = pub_base_getbrname( s_mo_bank_acc_po.tx_br_no, br_name );
			ERR_DEAL
			pub_base_strpack( br_name ); 
			sprintf( t_str , "%s    ( %5s )" , br_name , s_mo_bank_acc_po.tx_br_no); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'D': /* 页码 */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, page); 
			break; 
		case 'L': /* 协议编号 */
			sprintf( t_str , "%s" , s_mo_bank_acc_po.pact_no ); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'U': /* 户名 */
			pub_base_strpack( s_mo_bank_acc_po.payer_name ); 
			sprintf( t_str , "%s" , s_mo_bank_acc_po.payer_name ); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'R': /* 票面金额 */
			sprintf( amt_tmp, "%.2lf" , s_mo_bank_acc_po.sign_amt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			sprintf(fmt,"%%%ds",bll); 		
			if( strlen(amt_tmp)>bll ) 
				sprintf( amt_tmp, "%.2lf" , s_mo_bank_acc_po.sign_amt ); 
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'T': /* 到期日期 */
			sprintf( t_str , "%4d%02d%02d" , s_mo_bank_acc_po.matr_date/10000 , s_mo_bank_acc_po.matr_date%10000/100 , s_mo_bank_acc_po.matr_date%100 );
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , t_str ); 
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
