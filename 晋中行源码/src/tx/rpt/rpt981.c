/*************************************************************
* 文 件 名:  rpt981.c
* 功能描述： 支行月报表打印
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
#include "gl_sub_c.h"
#include "com_item_c.h"
#include "com_branch_c.h"
#define PAGE_CNT 43

struct gl_sub_c	s_gl_sub;
int flag=0;								/*** 1-表内尾 ***/
int page=0;
int line=0;
int line_file=0;
char vpfm[21];
char t_str[41];
char acc_type[2];
int opncnt;
FILE *fp; 
int ret = 0;

int get_ratio_lsqd(int bli,int bll,char str[100]);
char pp[100];

rpt981( )
{
	int    tag = 0;						/*** 0-表内 1-表外 ***/
	int    ttlnum;
	char   old_acc_head[2];
	char   filename[100];

	struct com_sys_parm_c s_com_sys_parm;
	struct com_branch_c s_com_branch;

	memset(&s_com_sys_parm, 0x00, sizeof(struct com_sys_parm_c)); 
         
	ret=sql_begin(); /*打开事务*/
	if( ret )
	{
		sprintf( acErrMsg, "打开事务失败!!!" );
		WRITEMSG
		goto ErrExit;
	}

	old_acc_head[0] = 'A';
	old_acc_head[1] = '\0';

	/* 初始化全局变量 */
	pub_base_sysinit();
    
	/* 取报表打印日期 */
	ret = Com_sys_parm_Sel( g_pub_tx.reply, &s_com_sys_parm, "1=1" );
	ERR_DEAL

	g_pub_tx.tx_date = s_com_sys_parm.lst_date;

	strcpy( filename , "BMybb" );	
	strcpy( vpfm, "bbybb" );

	ret=Com_branch_Dec_Sel1( g_pub_tx.reply,"br_no!='61000'" );
	ERR_DEAL

	while(1 )
	{
		ret=Com_branch_Fet_Sel1( &s_com_branch,g_pub_tx.reply );
		if( ret==100 ) break;
		ERR_DEAL

	old_acc_head[0] = 'A';
	old_acc_head[1] = '\0';
	tag=0;
flag=0;								/*** 1-表内尾 ***/
page=0;
line=0;
line_file=0;

	sprintf( pp," and br_no='%s' ",s_com_branch.br_no );

	ret = pub_rpt_rmfile( s_com_branch.br_no , filename, 0 );

	ret = Gl_sub_Dec_Sel( g_pub_tx.reply, "cur_no='01' and br_no='%s' ORDER BY acc_hrt",s_com_branch.br_no );
	ERR_DEAL
	
	ttlnum=0;
	
	while(1)
	{
		ret = Gl_sub_Fet_Sel( &s_gl_sub,g_pub_tx.reply );
		if( ret==100 ) 
		{
			if( ttlnum )
			{
				/* 打页尾 */
				if( tag==0 )
					ret = print_tail_B( );
				else if( tag==2 )
					ret = print_tail_C( );
				ERR_DEAL
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

		/* 科目类变换时 */
		if( s_gl_sub.acc_hrt[0] != old_acc_head[0] )
		{
			if( old_acc_head[0] == 'A' )				/* 资产类 */
			{
				strcpy( acc_type , "1" );
				old_acc_head[0] = '1';
				ERR_DEAL
			}
			else 
			{
				/* 打印页尾 */
				if( old_acc_head[0] == '5' )
				{
					flag = 1;
					ret = print_tail_A( );
					ERR_DEAL
					flag = 0;
				}
				else 
				{
					if( tag==0 )
						ret = print_tail_B( );
					else if( tag==2 ) 
						ret = print_tail_C( );
					ERR_DEAL
				}

				switch( old_acc_head[0] )
				{
					case '1':							/* 负债类 */
							strcpy( acc_type , "2" );
							break;
					case '2':							/* 所有者权益类 */
							strcpy( acc_type , "3" );
							break;
					case '3':							/* 资产负债共同类 */
							strcpy( acc_type , "4" );
							break;
					case '4':							/* 损益类 */
							strcpy( acc_type , "5" );
							break;
					case '5':							/* 表外类 */
							tag = 1;					/* 表外标志 */
							strcpy( acc_type , "6" );
							break;
					case '6':							/* 表外类 */
							tag = 2;					/* 表外标志 */
							strcpy( acc_type , "7" );
							break;
				 	default: 
							break;
				}
			}

			old_acc_head[0] = s_gl_sub.acc_hrt[0] ;
			
			line_file = 1;
			
			if( !ttlnum )
			{
				page = 1;

				/* 删除原文件 */
				ret = pub_rpt_rmfile( s_gl_sub.br_no , filename , 0 );
				ERR_DEAL

				/* 打开文件 */	
				ret = pub_rpt_openfile( &fp, s_gl_sub.br_no , filename );
				ERR_DEAL
			}
			else
			{
				page += 1;
				line_file=0;
			}

			/* 打印页首 */
			if( tag == 0 )
				ret = print_head_B( );
			else if( tag==1 )
				ret = print_head_C( );
			ERR_DEAL
		}

    	/* 打印科目信息 */
		if( tag==0 )
			ret = print_body_B( );
		else 
			ret = print_body_C( );
		ERR_DEAL

		line_file++;

		if( line_file>PAGE_CNT )			
		{
			line_file=0;
			page++;

			/* 打印页尾 */
			if( tag==0 )
				ret = print_tail_B();
			else
				ret = print_tail_C();
			ERR_DEAL

			/* 打印页首 */
			if( tag==0 )
				ret = print_head();
			else
				ret = print_head_C();
			ERR_DEAL
		}
		ttlnum++;
	}

	Gl_sub_Clo_Sel( );
	fclose(fp);

	}
	Com_branch_Clo_Sel1();
	
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
	
	vtcp_log("AAAAAAAAAAA----------------------[%s]",acc_type );
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

/* 表内打印表首 */
int print_head_B( )
{
	char vrec[3];
	
	vtcp_log("BBBBBBBBBBBB----------------------[%s]",acc_type );
	strcpy( vrec,"HH" );
	ret = Make_yeb_sub(vrec);

	strcpy( vrec,"AA" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL

	strcpy( vrec,"NN" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL
		
GoodExit:
	return 0;
ErrExit:
	return 1;
}

/* 表外打印表首 */
int print_head_C( )
{
	char vrec[3];
	
	vtcp_log("AAAAAAAAAAA----------------------[%s]",acc_type );
	strcpy( vrec,"HH" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL
	
	strcpy( vrec,"DD" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL
		
GoodExit:
	return 0;
ErrExit:
	return 1;
}
/* 表内打印表尾 */
int print_tail_B( )
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

/* 表外打印表尾 */
int print_tail_C( )
{
	char vrec[3];

	strcpy( vrec,"FF" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL
GoodExit:
	return 0;
ErrExit:
	return 1;
}

/* 表内打印表体 */
int print_body_B( )
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

/* 表外打印表体 */
int print_body_C( )
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

/* 表内所有科目汇总打印表体 */
int print_tail_A( )
{
	char vrec[3];

	strcpy( vrec,"NN" );
	ret = Make_yeb_sub(vrec);

	strcpy( vrec,"CC" );
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
	struct com_item_c s_com_item;

	char vhm[101]; 
	char vstr[101]; 
	char l_kmm[41];
	char amt_tmp[41]; 
	char tmp_inf[41];
	char tmp_acc_name[41];
	char fmt[20]; 
	char br_name[71];
	double tmp_amt;
	int i=0; 

	switch( bli ) 
	{ 
		case 'A': /* 交易日期 */ 
			sprintf( t_str , "%4d年%2d月%2d日" , g_pub_tx.tx_date/10000 , g_pub_tx.tx_date%10000/100 , g_pub_tx.tx_date%100 );
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , t_str ); 
			break;
		case 'B': /* 科目名称 */
			pub_base_strpack( s_gl_sub.acc_hrt ); 
			ret = Com_item_Sel( g_pub_tx.reply, &s_com_item,"acc_hrt='%s'", \
								s_gl_sub.acc_hrt );
			ERR_DEAL
			sprintf( t_str , "%s" , s_com_item.acc_name ); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'C': /* 机构名称 */
			ret = pub_base_getbrname( s_gl_sub.br_no, br_name );
			ERR_DEAL
			pub_base_strpack( br_name ); 
			sprintf( t_str , "%s    ( %5s )" , br_name , s_gl_sub.br_no); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'D': /* 页码 */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, page); 
			break; 
		case 'E': /* 月初借余额 */
			sprintf( amt_tmp, "%.2lf" , s_gl_sub.mdr_bal ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			sprintf(fmt,"%%%ds",bll); 		
			if( strlen(amt_tmp)>bll ) 
				sprintf( amt_tmp, "%.2lf" , s_gl_sub.mdr_bal ); 
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'F': /* 月初贷余额 */
			sprintf( amt_tmp, "%.2lf" , s_gl_sub.mcr_bal ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			if( strlen(amt_tmp)>bll ) 
				sprintf( amt_tmp, "%.2lf" , s_gl_sub.mcr_bal ); 
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'G': /* 本月借发生额 */
			sprintf( amt_tmp, "%.2lf" , s_gl_sub.mdr_amt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			if( strlen(amt_tmp)>bll ) 
				sprintf( amt_tmp, "%.2lf" , s_gl_sub.mdr_amt ); 
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'H': /* 本月贷发生额 */
			sprintf( amt_tmp, "%.2lf" , s_gl_sub.mcr_amt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			if( strlen(amt_tmp)>bll ) 
				sprintf( amt_tmp, "%.2lf" , s_gl_sub.mcr_amt ); 
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'Q': /* 月末借余额 */
			sprintf( amt_tmp, "%.2lf" , s_gl_sub.dr_bal ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			sprintf(fmt,"%%%ds",bll); 		
			if( strlen(amt_tmp)>bll ) 
				sprintf( amt_tmp, "%.2lf" , s_gl_sub.dr_bal ); 
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'R': /* 月末贷余额 */
			sprintf( amt_tmp, "%.2lf" , s_gl_sub.cr_bal ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			if( strlen(amt_tmp)>bll ) 
				sprintf( amt_tmp, "%.2lf" , s_gl_sub.cr_bal ); 
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'X': /* 标题 */
			if( flag==0 )
			{
				if( acc_type[0]=='1' )
					strcpy( tmp_acc_name,"资产类合计" );
				else if( acc_type[0]=='2' )
					strcpy( tmp_acc_name,"负债类合计" );
				else if( acc_type[0]=='3' )
					strcpy( tmp_acc_name,"所有者权益类合计" );
				else if( acc_type[0]=='4' )
					strcpy( tmp_acc_name,"资产负债类合计" );
				else if( acc_type[0]=='5' )
					strcpy( tmp_acc_name,"损益类合计" );
			}
			else
				strcpy( tmp_acc_name,"表内科目类合计" );

			pub_base_strpack( tmp_acc_name ); 
			sprintf( t_str , "%s" , tmp_acc_name ); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'Y': /* 汇总 */
			strcpy( tmp_acc_name, "表内科目合计" );
			pub_base_strpack( tmp_acc_name ); 
			sprintf( t_str , "%s" , tmp_acc_name ); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'I': /* 科目号 */
			pub_base_strpack( s_gl_sub.acc_hrt ); 
			ret = Com_item_Sel( g_pub_tx.reply, &s_com_item, "acc_hrt='%s'", \
								s_gl_sub.acc_hrt );
			ERR_DEAL
			sprintf( t_str , "%s" , s_com_item.acc_no ); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'K': /* 月初借余额 */
			vtcp_log( "acc_type====================[%s]",acc_type );
			if( flag==0 )
			{
				ret = sql_sum_double( "gl_sub","mdr_bal",&tmp_amt,
	"acc_hrt[1]='%c' and acc_hrt in ( select acc_hrt from com_item where acc_lvl[1]='1' ) %s", acc_type[0],pp );
			}
			else 
			{
				ret = sql_sum_double( "gl_sub","mdr_bal",&tmp_amt,"acc_hrt[1]>='1' and acc_hrt[1]<='5' and acc_hrt in ( select acc_hrt from com_item where acc_lvl[1]='1' ) %s",pp );
			}
			ERR_DEAL

			sprintf( amt_tmp, "%.2lf" , tmp_amt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			if( strlen(amt_tmp)>bll ) 
				sprintf( amt_tmp, "%.2lf" , tmp_amt ); 
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'L': /* 月初贷余额 */
			if( flag==0 )
			{
				ret = sql_sum_double( "gl_sub","mcr_bal",&tmp_amt,"acc_hrt[1]='%c' and acc_hrt in ( select acc_hrt from com_item where acc_lvl[1]='1' ) %s", acc_type[0],pp );
			}
			else 
			{
				ret = sql_sum_double( "gl_sub","mcr_bal",&tmp_amt,"acc_hrt[1]>='1' and acc_hrt[1]<='5' and acc_hrt in ( select acc_hrt from com_item where acc_lvl[1]='1' ) %s",pp );
			}
			ERR_DEAL

			sprintf( amt_tmp, "%.2lf" , tmp_amt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			if( strlen(amt_tmp)>bll ) 
				sprintf( amt_tmp, "%.2lf" , tmp_amt ); 
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'O': /* 本月借发生额 */
			if( flag==0  )
			{
				ret = sql_sum_double( "gl_sub","mdr_amt",&tmp_amt,"acc_hrt[1]='%c'  and acc_hrt in ( select acc_hrt from com_item where acc_lvl[1]='1' ) %s", acc_type[0] ,pp);
			}
			else
			{
				ret = sql_sum_double( "gl_sub","mdr_amt",&tmp_amt,"acc_hrt[1]>='1' and acc_hrt[1]<='5' and acc_hrt in ( select acc_hrt from com_item where acc_lvl[1]='1' ) %s",pp );
			}
			ERR_DEAL

			sprintf( amt_tmp, "%.2lf" , tmp_amt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			if( strlen(amt_tmp)>bll ) 
				sprintf( amt_tmp, "%.2lf" , tmp_amt ); 
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'P': /* 本月贷发生额 */
			if( flag==0 )
			{
				ret = sql_sum_double( "gl_sub","mcr_amt",&tmp_amt,"acc_hrt[1]='%c' and acc_hrt in ( select acc_hrt from com_item where acc_lvl[1]='1' ) %s", acc_type[0],pp );
			}
			else
			{
				ret = sql_sum_double( "gl_sub","mcr_amt",&tmp_amt,"acc_hrt[1]>='1' and acc_hrt[1]<='5' and acc_hrt in ( select acc_hrt from com_item where acc_lvl[1]='1' ) %s",pp );
			}
			ERR_DEAL

			sprintf( amt_tmp, "%.2lf" , tmp_amt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			if( strlen(amt_tmp)>bll ) 
				sprintf( amt_tmp, "%.2lf" , tmp_amt ); 
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'U': /* 月末借余额 */
			if( flag==0  )
			{
				ret = sql_sum_double( "gl_sub","dr_bal",&tmp_amt,"acc_hrt[1]='%c'  and acc_hrt in ( select acc_hrt from com_item where acc_lvl[1]='1' ) %s", acc_type[0],pp );
			}
			else
			{
				ret = sql_sum_double( "gl_sub","dr_bal",&tmp_amt,"acc_hrt[1]>='1' and acc_hrt[1]<='5' and acc_hrt in ( select acc_hrt from com_item where acc_lvl[1]='1' ) %s",pp );
			}
			ERR_DEAL

			sprintf( amt_tmp, "%.2lf" , tmp_amt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			if( strlen(amt_tmp)>bll ) 
				sprintf( amt_tmp, "%.2lf" , tmp_amt ); 
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'V': /* 月末贷余额 */
			if( flag==0 )
			{
				ret = sql_sum_double( "gl_sub","cr_bal",&tmp_amt,"acc_hrt[1]='%c' and acc_hrt in ( select acc_hrt from com_item where acc_lvl[1]='1' ) %s", acc_type[0],pp );
			}
			else
			{
				ret = sql_sum_double( "gl_sub","cr_bal",&tmp_amt,"acc_hrt[1]>='1' and acc_hrt[1]<='5' and acc_hrt in ( select acc_hrt from com_item where acc_lvl[1]='1' ) %s",pp );
			}
			ERR_DEAL

			sprintf( amt_tmp, "%.2lf" , tmp_amt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			if( strlen(amt_tmp)>bll ) 
				sprintf( amt_tmp, "%.2lf" , tmp_amt ); 
			sprintf(fmt,"%%%ds",bll); 		
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
