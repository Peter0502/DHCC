/*************************************************************
* 文 件 名: rpt026.c
* 功能描述：贷款到期流水清单(正常转逾期每天执行,从流水历史表trace_log_bk出)
*
* 作    者: lance
* 完成日期: 2003年09月05日
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
#include "trace_log_bk_c.h"
#include "com_branch_c.h"
#include "ln_mst_hst_c.h"
#include "ln_parm_c.h"                  /*要求将报表中“产品”更改为“科目号”，zhangpeng 20131125*/
#include "dc_acc_rel_c.h"								/*要求将报表中“产品”更改为“科目号”，zhangpeng 20131125*/
#include "com_item_c.h"									/*要求将报表中“产品”更改为“科目号”，zhangpeng 20131125*/


struct trace_log_bk_c s_trace_log_bk;
struct ln_mst_hst_c s_ln_mst_hst;
struct ln_parm_c s_ln_parm;							/*要求将报表中“产品”更改为“科目号”，zhangpeng 20131125*/
struct dc_acc_rel_c s_dc_acc_rel;				/*要求将报表中“产品”更改为“科目号”，zhangpeng 20131125*/
struct com_item_c s_com_item;						/*要求将报表中“产品”更改为“科目号”，zhangpeng 20131125*/

int page=0;
int line=0;
int line_file=0;
char vpfm[21];
char t_str[41];
int opncnt;
FILE    *fp; 
int ret = 0;

int get_ratio_lndqqd(int bli,int bll,char str[100]);

rpt026()
{
	int flag=0;
	int ttlnum;


	char fname[100];
	char filename[100];
	char wherelist[1024];  /**查询条件**/
	char tmpstr[300];

	char tmp_str[60];
	char old_br_no[6];
	long old_trace_no=0;
	
	memset( wherelist,0,sizeof(wherelist) );
	memset( &s_trace_log_bk , 0x00 , sizeof(struct trace_log_bk_c));
	
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
	strcpy( filename , "BRlndqqd" );	
	strcpy(vpfm, "lndqqd" );
*/
	strcpy( filename , "RPT026");
	strcpy(vpfm, "RPT026");
	/* 条件:交易代码为"G088" */
	strcpy( tmpstr,"tx_code='G088'and prdt_no[1]='3' and " );
	strcat( wherelist,tmpstr );	
	/**组成查询**/

	strcat(wherelist," no_show!='1' and 1=1  ORDER BY tx_br_no, prdt_no, trace_no, trace_cnt") ;

	vtcp_log( "WHERE [%s]",wherelist );
	
	ret=Trace_log_bk_Dec_Sel( g_pub_tx.reply, wherelist );
	ERR_DEAL

		ttlnum=0;
	
	while(1)
	{
		ret=Trace_log_bk_Fet_Sel( &s_trace_log_bk, g_pub_tx.reply );
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
		if( s_trace_log_bk.trace_no==old_trace_no )	/*流水号未改变*/
		{
			continue;
		}
		
		old_trace_no= s_trace_log_bk.trace_no;
		
		/*取贷款户信息*/
		ret = pub_ln_lndis_file(s_trace_log_bk.ac_no,s_trace_log_bk.ac_seqn,
			&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm ); /*读取贷款户信息*/
		if( ret ) 
		{
			sprintf(acErrMsg,"读取贷款户信息error");
			WRITEMSG
			return ret;
		}		
			
		/* 机构变换时 */
		pub_base_strpack( s_trace_log_bk.tx_br_no );
		if( strcmp( s_trace_log_bk.tx_br_no , old_br_no ) )
		{
			/* 第一个机构 */
			if( strcmp( old_br_no , "*****" ) )
			{
				/* 打印页尾 */
				ret = print_tail();
				ERR_DEAL
				fclose(fp);
			}
			
			strcpy( old_br_no , s_trace_log_bk.tx_br_no );
			
			page=1;
			line_file=1;
			
			/* 删除原文件 */
			ret = pub_rpt_rmfile( s_trace_log_bk.tx_br_no , filename , 0 );
			ERR_DEAL
			/* 打开文件 */	
			ret = pub_rpt_openfile( &fp , s_trace_log_bk.tx_br_no , filename );
			ERR_DEAL
			/* 打印页首 */
			ret = print_head();
			ERR_DEAL
		}
    		/* 打印信息 */
		ret = print_body();
		ERR_DEAL
		line_file++;
		if( line_file>50 )	/* 大于50行换页 */
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
	Trace_log_bk_Clo_Sel();
	
OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"生成贷款到期流水清单(正常转逾期)成功!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--事务回滚--*/
	sprintf(acErrMsg,"生成贷款到期流水清单(正常转逾期)失败!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

/* 中间函数 */
int Make_yeb_sub( char vrec[3] )
{	
	ret = pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001",vrec,&opncnt,get_ratio_lndqqd);
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
int get_ratio_lndqqd( bli,bll,str )
int bli,bll;
char str[100];
{ 
	char vhm[101]; 
	char vstr[101]; 
	char l_kmm[31];
	char amt_tmp[21]; 
	char tmp_inf[21];
	char mtr_date[13];
	char fmt[10]; 
	int i=0; 
	struct com_branch_c sCom_branch;

	memset(&sCom_branch, 0x0, sizeof(struct com_branch_c));
	
	switch( bli ) 
	{ 
		case 'A': /* 交易日期 */
			sprintf( t_str , "%4d年%2d月%2d日" , s_trace_log_bk.tx_date/10000 , s_trace_log_bk.tx_date%10000/100 , s_trace_log_bk.tx_date%100 );
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , t_str ); 
			break;
		case 'B': /* 机构编码 */
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , s_trace_log_bk.tx_br_no ); 
			break; 
		case 'C': /* 机构名称 */
			ret = Com_branch_Sel(g_pub_tx.reply, &sCom_branch, "br_no='%s'",
		s_trace_log_bk.tx_br_no);
			if (ret != 0)
			{
				sprintf(acErrMsg, "查询公共机构编码表错误! [%d]", ret);
				WRITEMSG
				goto ErrExit;
			}
			pub_base_strpack( sCom_branch.br_name ); 
			sprintf( t_str , "%s    ( %5s )" , sCom_branch.br_name , s_trace_log_bk.tx_br_no); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'D': /* 页码 */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, page); 
			break; 
		case 'E': /* 流水号 */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, s_trace_log_bk.trace_no); 
			break; 
		case 'G': /* 贷款账号 */
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt, s_trace_log_bk.ac_no);
			break;
		case 'H': /* 账号序号 */
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt, s_trace_log_bk.ac_seqn);
			break;
		case 'I': /* 贷款户名 */
			sprintf(fmt, "%%-%ds", bll);
			char tmp_name[51];
			memset(tmp_name,'\0',sizeof(tmp_name));
			memcpy(tmp_name,g_ln_mst.name,50);
			sprintf( str, fmt, g_ln_mst.name);
			break;
		/**产品和产品名称注释掉增加科目号。zhangpeng 20131125 start**
		case 'J': //产品
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str, fmt, g_ln_mst.prdt_no);
			break;
		case 'K': //产品名称 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, g_ln_parm.title);
			break;
		*产品和产品名称注释掉增加科目号。zhangpeng 20131125 end***/
		
		/**增加科目号。zhangpeng 20131125 start***/
		case 'J': /*科目号*/			
			ret=Ln_parm_Sel( g_pub_tx.reply ,&s_ln_parm,"prdt_no = '%s'",g_ln_mst.prdt_no);
			if( ret )
			{
				return 1;
			}
			ret=Dc_acc_rel_Sel( g_pub_tx.reply ,&s_dc_acc_rel,"dc_code = '%s'",s_ln_parm.dc_code);
			if( ret )
			{
				return 1;
			}
			ret=Com_item_Sel( g_pub_tx.reply ,&s_com_item,"acc_hrt = '%s'",s_dc_acc_rel.acc_hrt);
			if( ret )
			{
				return 1;
			}
			sprintf(fmt,"%%-%ds",bll); 		
			sprintf( str, fmt, s_com_item.acc_no ); 
			memset( &s_ln_parm, 0x00 , sizeof( struct ln_parm_c ));
			memset( &s_dc_acc_rel, 0x00 , sizeof( struct dc_acc_rel_c ));
			memset( &s_com_item, 0x00 , sizeof( struct com_item_c ));
			break;
		/**增加科目号。zhangpeng 20131125 end***/
		case 'L': /* 开户行 */
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, g_ln_mst.opn_br_no);
			break;
		case 'M': /* 到期日期 */
			if( g_ln_mst.exp_ind[0]=='0' )
			{
				sprintf( mtr_date, "%ld", g_ln_mst.mtr_date );
			}else{					
				sprintf( mtr_date, "%ld(展)", g_ln_mst.exp_mtr_date );
			}		
			sprintf(fmt,"%%-%ds",bll);				
			sprintf(str, fmt, mtr_date);
			break;	
		case 'N': /* 尚欠本金 */						
			/* 加逗号 */
			sprintf( amt_tmp, "%.2lf" , g_ln_mst.bal ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'O': /* 状态 */
			pub_base_dic_show_str(tmp_inf,"ln_ac_sts",g_ln_mst.ac_sts);
			sprintf(fmt, "%%-%ds", bll);		
			sprintf(str, fmt, tmp_inf);
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
