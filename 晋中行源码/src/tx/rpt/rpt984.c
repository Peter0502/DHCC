/*************************************************************
* 文 件 名:  rpt984.c
* 功能描述： 大额资金进出报表打印
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
/*JYW#include "com_branch_c.h" **/
#include "tx_def_c.h"
#include "trace_log_c.h"
#define  AMT_LMT     1000000.00
#define  PAGE_CNT    30

struct com_branch_c	s_com_branch;
struct trace_log_c	trace_log;
struct dd_mst_c	s_dd_mst;
struct td_mst_c	s_td_mst;
int flag=0;								/*** 1-页末 ***/
int page=0;
int line=0;
int line_file=0;
char vpfm[21];
char t_str[51];
char s_name[51];
char g_ac[51];
int opncnt;
FILE *fp; 
int ret = 0;
int count=0;
double  collect_amt=0.00;
double  minus_amt=0.00;

int get_ratio_lsqd(int bli,int bll,char str[100]);
	char   old_br_no[6];
char g_dfz[6][31];
char g_brf[6][31];
	int ii=0;
	int jj=0;
	int kk=0;
	int oo=0;
	int pp=0;
	char   s_opn_br_no[6];

rpt984( )
{
	int    ttlnum;
	char   filename[100];
	int first=0;

	struct com_sys_parm_c s_com_sys_parm;
	struct trace_log_c trace_log_c;
	long lsh[6];
	int jdbz[6];

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

	strcpy( filename , "BRdezjjcmx" );	
	strcpy( vpfm, "dezjjcmx" );

	count = sql_count( "com_branch","br_type='4' or br_type='1'" );
	if( count<=0 )
	{
		sprintf(acErrMsg,"取记录出错!");
		WRITEMSG			
		goto ErrExit;			
	}

	/* 删除原文件 */
	ret = pub_rpt_rmfile( TOT_BR_NO , filename , 0 );
	ERR_DEAL

	/* 打开文件 */	
	ret = pub_rpt_openfile(&fp,TOT_BR_NO,filename );
	ERR_DEAL

	page=1;
	/* 打印页首 */
	ret = print_head( );
	ERR_DEAL

	/*** 按营业网点顺序取 ***/
	ret = Com_branch_Dec_Sel1( g_pub_tx.reply, "br_type in('4','5') ORDER BY up_br_no");
	ERR_DEAL

	ttlnum=0;
	collect_amt = 0.00;

	while(1)
	{
		ret = Com_branch_Fet_Sel1( &s_com_branch,g_pub_tx.reply );
		if( ret==100 ) 
		{
			ret = print_tail( );
			ERR_DEAL
			break;
		}
		else ERR_DEAL

		if( strncmp(old_br_no,s_com_branch.up_br_no,5) 
			&& strncmp(old_br_no,"AAAAA",5) )
		{
			flag = 2;
vtcp_log("1 PAGE[%d]",page);
			ret = print_body_hj( );
			flag = 0;
			collect_amt = 0.00;
		}

		/*
		if( !ttlnum )
		{
			page = 1;
			line_file = 1;
		}
		*/

		/*** 取活期大额记录 ***/
		vtcp_log( "SSS :lst_date=%ld and abs(ys_bal-bal)>=%lf and opn_br_no='%s' ORDER BY opn_br_no ",g_pub_tx.tx_date,AMT_LMT,s_com_branch.br_no );

		ret = Dd_mst_Dec_Sel( g_pub_tx.reply, "lst_date=%ld and abs(ys_bal-bal)>=%lf and opn_br_no='%s' ORDER BY opn_br_no ",g_pub_tx.tx_date,AMT_LMT,s_com_branch.br_no );
		ERR_DEAL
	
		while(1)
		{
			ret = Dd_mst_Fet_Sel( &s_dd_mst,g_pub_tx.reply );
			if( ret==100 ) break;
			else ERR_DEAL

			pub_base_strpack( s_dd_mst.opn_br_no );
			strcpy( s_opn_br_no,s_dd_mst.opn_br_no );

			minus_amt = s_dd_mst.bal - s_dd_mst.ys_bal ;
			collect_amt = collect_amt + ( s_dd_mst.bal - s_dd_mst.ys_bal ) ;

 		   	/* 打印表体信息 */
			strcpy( s_name,s_dd_mst.name );

vtcp_log("2 PAGE[%d]",page);
		ret=Trace_log_Dec_Sel( g_pub_tx.reply,
			"ac_id=%d and ac_seqn=%d order by amt DESC", 
			s_dd_mst.ac_id ,s_dd_mst.ac_seqn );
		ERR_DEAL
		first=0;
		jj=0;
		kk=0;
				strcpy( g_ac," " );
		while(1)
		{
			ret=Trace_log_Fet_Sel( &trace_log_c, g_pub_tx.reply );
			if( ret==100 ) break;
			ERR_DEAL
			
			if(first==0 )
			{
				strcpy( g_ac,trace_log_c.ac_no );
			}
			if( first==0 || pub_base_CompDblVal(trace_log_c.amt,AMT_LMT/2) ) 
			{
				lsh[jj]=trace_log_c.trace_no;
				jdbz[jj]=trace_log_c.add_ind[0];
				pub_base_strpack( trace_log_c.brf );
				strcpy( g_brf[jj],trace_log_c.brf );
				jj++;
				if( jj>=6 ) break;
			}
			first++;
		}
		Trace_log_Clo_Sel();
		if( first==0 )
		{
			ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,
				"ac_id=%d",s_dd_mst.ac_id);
			if( ret ) strcpy( g_mdm_ac_rel.ac_no," " );
			strcpy( g_ac,g_mdm_ac_rel.ac_no );
		}

		for( ii=0;ii<jj;ii++ )
		{
			ret=Trace_log_Sel( g_pub_tx.reply,&trace_log,
				"trace_no=%ld and ac_id!=%d and add_ind!=%c and amt>%.2lf order by amt DESC",
				lsh[ii] ,s_dd_mst.ac_id ,jdbz[ii],AMT_LMT/5);
			if( ret==100 ) 
			{	
				strcpy( trace_log.brf,"" );
				strcpy( trace_log.ac_no,"" );
			}
			else { ERR_DEAL }

			pub_base_strpack(trace_log.ac_no);
			pub_base_strpack(trace_log.brf);
			if( ii>0 )
			{
			if( !strcmp(trace_log.brf,g_brf[0]) &&
				!strcmp(trace_log.ac_no,g_dfz[0])  )

			{
				strcpy( trace_log.brf,"" );
				strcpy( trace_log.ac_no,"" );
			}
			if( !strcmp(trace_log.brf,g_brf[ii-1]) &&
				!strcmp(trace_log.ac_no,g_dfz[ii-1]) )
			{
				strcpy( trace_log.brf,"" );
				strcpy( trace_log.ac_no,"" );
			}
			}
			strcpy( g_dfz[ii],trace_log.ac_no );
		}
			for( pp=0;pp<jj;pp++ )
			{
				if( strlen(g_brf[pp]) ) break;
			}
			for( oo=pp+1;oo<jj;oo++ )
			{
				if( strlen(g_brf[oo]) && strlen(g_dfz[oo]) ) break;
			}
			ret = print_body( );
			ERR_DEAL
			line_file++;
			line_file++;

			oo++;
			for( ;oo<jj;oo++ )
			{
				if( strlen(g_brf[oo]) && strlen(g_dfz[oo]) )
				{
				ret = print_body_2( );
				ERR_DEAL
				line_file++;
				}
			}


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

		Dd_mst_Clo_Sel( );

		/*** 取定期大额记录 ***/
		ret = Td_mst_Dec_Sel( g_pub_tx.reply, "lst_date=%ld and abs(ys_bal-bal)>=%lf and opn_br_no='%s' ORDER BY opn_br_no ",g_pub_tx.tx_date,AMT_LMT,s_com_branch.br_no );
		ERR_DEAL
	
		while(1)
		{
			ret = Td_mst_Fet_Sel( &s_td_mst,g_pub_tx.reply );
			if( ret==100 ) 
			{
				flag = 0;
				break;
			}
			else ERR_DEAL

			pub_base_strpack( s_td_mst.opn_br_no );
			strcpy( s_opn_br_no,s_td_mst.opn_br_no );

			minus_amt = s_td_mst.bal - s_td_mst.ys_bal ;
			collect_amt = collect_amt + ( s_td_mst.bal - s_td_mst.ys_bal ) ;

	    	/* 打印表体信息 */
			strcpy( s_name,s_td_mst.name );
vtcp_log("3 PAGE[%d]",page);
		ret=Trace_log_Dec_Sel( g_pub_tx.reply,
			"ac_id=%d and ac_seqn=%d order by amt DESC", 
			s_td_mst.ac_id ,s_td_mst.ac_seqn );
		ERR_DEAL
		first=0;
		jj=0;
		kk=0;
				strcpy( g_ac," " );
		while(1)
		{
			ret=Trace_log_Fet_Sel( &trace_log_c, g_pub_tx.reply );
			if( ret==100 ) break;
			ERR_DEAL
			
			if(first==0 )
			{
				strcpy( g_ac,trace_log_c.ac_no );
			}
			if( first==0 || pub_base_CompDblVal(trace_log_c.amt,AMT_LMT/2) ) 
			{
				lsh[jj]=trace_log_c.trace_no;
				jdbz[jj]=trace_log_c.add_ind[0];
				pub_base_strpack( trace_log_c.brf );
				strcpy( g_brf[jj],trace_log_c.brf );
				jj++;
				if( jj>=6 ) break;
			}
			first++;
		}
		Trace_log_Clo_Sel();
		if( first==0 )
		{
			ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,
				"ac_id=%d",s_td_mst.ac_id);
			if( ret ) strcpy( g_mdm_ac_rel.ac_no," " );
			strcpy( g_ac,g_mdm_ac_rel.ac_no );
		}

		for( ii=0;ii<jj;ii++ )
		{
			ret=Trace_log_Sel( g_pub_tx.reply,&trace_log,
				"trace_no=%ld and ac_id!=%d and add_ind!=%c and amt>%.2lf order by amt DESC",
				lsh[ii] ,s_td_mst.ac_id ,jdbz[ii],AMT_LMT/5);
			if( ret==100 ) 
			{	
				strcpy( trace_log.brf,"" );
				strcpy( trace_log.ac_no,"" );
			}
			else { ERR_DEAL }

			pub_base_strpack(trace_log.ac_no);
			pub_base_strpack(trace_log.brf);
			if( ii>0 )
			{
			if( !strcmp(trace_log.brf,g_brf[0]) &&
				!strcmp(trace_log.ac_no,g_dfz[0])  )

			{
				strcpy( trace_log.brf,"" );
				strcpy( trace_log.ac_no,"" );
			}
			if( !strcmp(trace_log.brf,g_brf[ii-1]) &&
				!strcmp(trace_log.ac_no,g_dfz[ii-1]) )
			{
				strcpy( trace_log.brf,"" );
				strcpy( trace_log.ac_no,"" );
			}
			}
			strcpy( g_dfz[ii],trace_log.ac_no );
		}
			for( pp=0;pp<jj;pp++ )
			{
				if( strlen(g_brf[pp]) ) break;
			}
			for( oo=pp+1;oo<jj;oo++ )
			{
				if( strlen(g_brf[oo]) && strlen(g_dfz[oo]) ) break;
			}
			ret = print_body( );
			ERR_DEAL
			line_file++;
			line_file++;

			oo++;
			for( ;oo<jj;oo++ )
			{
				if( strlen(g_brf[oo]) && strlen(g_dfz[oo]) )
				{
				ret = print_body_2( );
				ERR_DEAL
				line_file++;
				}
			}
	
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
			ttlnum++ ;
		}

		Td_mst_Clo_Sel( );

		strcpy( old_br_no,s_com_branch.up_br_no );
	}

	Com_branch_Clo_Sel1( );
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
int print_body_hj( )
{
	char vrec[3];

	strcpy( vrec,"EE" );
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

	strcpy( vrec,"BB" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL
	
GoodExit:
	return 0;
ErrExit:
	return 1;
}
int print_body_2( )
{
	char vrec[3];

	strcpy( vrec,"DD" );
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
	struct tx_def_c s_tx_def;

	long cif_no;
	char vhm[101]; 
	char vstr[101]; 
	char l_kmm[61];
	char amt_tmp[61]; 
	char tmp_inf[61];
	char tmp_str[61];
	char fmt[20]; 
	char br_name[71];
	char prdt_name[61];
	double tmp_amt;
	int i=0; 
	char tt[2];

	tt[1]='\0';
	tt[0]='0'+oo;

	switch( bli ) 
	{ 
		case 'x':
			i=oo;
			if( jj>i )
			{
				sprintf( vhm,"%-20s",g_brf[i] );
				strcat( vhm," " );
				strcat( vhm,g_dfz[i] );
			}
			else 
				strcpy( vhm,"  " );
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , vhm );
			break;
		case 'X':
			i=pp;
			if( jj>i )
			{
				sprintf( vhm,"%-20s",g_brf[i] );
				strcat( vhm," " );
				strcat( vhm,g_dfz[i] );
			}
			else 
				strcpy( vhm,"  " );
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , vhm );
			break;
		case 'M':
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , s_opn_br_no );
			break;
		case 'A': /* 交易日期 */ 
			sprintf( t_str , "%4d年%2d月%2d日" , g_pub_tx.tx_date/10000 , g_pub_tx.tx_date%10000/100 , g_pub_tx.tx_date%100 );
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , t_str ); 
			break;
		case 'C': /* 机构名称 */
			ret = pub_base_getbrname( TOT_BR_NO, br_name );
			ERR_DEAL
			pub_base_strpack( br_name ); 
			sprintf( t_str , "%s    ( %5s )" , br_name , TOT_BR_NO ); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'D': /* 页码 */
			sprintf(fmt,"%%%dd",bll); 
vtcp_log("PAGE[%d]",page);
			sprintf(str, fmt, page); 
			break; 
		case 'l': /* 户名 */
				ret = pub_base_getbrname( old_br_no, br_name );
				ERR_DEAL
				pub_base_strpack( br_name ); 
				memset( s_name, '0', sizeof(s_name)-1 );
				strcpy( s_name, "【 " );
				strcat( s_name, br_name );
				strcat( s_name, " 轧差数 】" ); 
				sprintf( t_str , "%s" , s_name ); 
				sprintf(fmt, "%%-%ds", bll);
				sprintf(str, fmt, t_str);
			break;
		case 'O':
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, g_ac);
			break;
		case 'L': /* 户名 */
				pub_base_strpack( s_name ); 
				sprintf( t_str , "%s" , s_name ); 
				sprintf(fmt, "%%-%ds", bll);
				sprintf(str, fmt, t_str);
			break;
		case 'u': /* 交易金额 */
				tmp_amt = collect_amt;
				sprintf( amt_tmp, "%.2lf" , tmp_amt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			sprintf(fmt,"%%%ds",bll); 		
			if( strlen(amt_tmp)>bll ) 
				sprintf( amt_tmp, "%.2lf" , tmp_amt ); 
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'U': /* 交易金额 */
				tmp_amt = minus_amt;
				sprintf( amt_tmp, "%.2lf" , tmp_amt ); 

			pub_rpt_flttomon( amt_tmp,amt_tmp );
			sprintf(fmt,"%%%ds",bll); 		
			if( strlen(amt_tmp)>bll ) 
				sprintf( amt_tmp, "%.2lf" , tmp_amt ); 
			sprintf( str, fmt, amt_tmp ); 
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
