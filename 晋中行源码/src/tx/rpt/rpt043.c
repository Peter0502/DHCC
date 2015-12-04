/*************************************************************
* 文 件 名: rpt043.c
* 功能描述：按揭贷款日终扣款客户回单(每天)
*
* 作    者: lance
* 完成日期: 2003年09月15日
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
#include "ln_mst_hst_c.h"
#include "com_branch_c.h"

struct ln_mst_hst_c s_ln_mst_hst;
struct ln_mst_hst_c old_ln_mst_hst;
struct ln_mst_c s_ln_mst;
struct ln_parm_c s_ln_parm;

int page=0;
int line=0;
int line_file=0;
char vpfm[21];
char t_str[41];
int opncnt;
long min_trace_no=0;
FILE    *fp; 
int ret = 0;
double bal_amt=0.00;
double in_amt=0.00;
double out_amt=0.00;
double cmpd_amt=0.00;
double rate_tmp=0.00;
long days=0;
long qry_date=0;
char intst_type_name[9];
		
int get_ratio_lnajkkhd(int bli,int bll,char str[100]);

rpt043()
{
	int flag=0;
	int ttlnum=0;

	char fname[100];
	char filename[100];
	char wherelist[1024];  /**查询条件**/
	char tmpstr[300];

	char tmp_str[60];
	char old_br_no[6];
	char old_intst_type[2];
	long old_ac_id=0;
	
	memset( wherelist,0,sizeof(wherelist) );
	memset( &s_ln_mst_hst , 0x00 , sizeof(struct ln_mst_hst_c));
	memset( &old_ln_mst_hst, 0x00 , sizeof( struct ln_mst_hst_c ));
		
	strcpy( old_br_no , "*****" );
	strcpy( old_intst_type,"*" );
	
	/*计算交易日期*/
	qry_date=g_pub_tx.tx_date;

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
	strcpy( filename , "BJlnajkkhd" );	
	strcpy(vpfm, "lnajkkhd" );
*/	
	strcpy( filename , "RPT043");
	strcpy(vpfm, "RPT043");
	/* 条件:tx_date为当日,tx_code='G086'and 'G085' ,增减标志为减少 */
	memset( &tmpstr,0x0,strlen(tmpstr));	
	memset( &wherelist,0x0,strlen(wherelist));
	sprintf( tmpstr," tx_code ='G085' and tx_date=%ld and add_ind='0' and " ,qry_date );
	strcat( wherelist,tmpstr );	
	
	/**组成查询**/
	strcat(wherelist," 1=1 ORDER BY opn_br_no, ac_id, ac_seqn, ln_tx_type ");
	vtcp_log( "WHERE [%s]",wherelist );
	
	ret=Ln_mst_hst_Dec_Sel( g_pub_tx.reply, wherelist );
	ERR_DEAL

		ttlnum=0;
	while(1)
	{
		ret=Ln_mst_hst_Fet_Sel( &s_ln_mst_hst, g_pub_tx.reply );
		if( ret==100 ) 
		{
			if( ttlnum )
			{
				/* 打印表体 */
				ret = print_body();
				ERR_DEAL
				/* 打页尾 */
				ret = Make_yeb_sub("CC");
				ERR_DEAL
				ret = Make_yeb_sub("DD");
				ERR_DEAL
				fclose(fp);
			}
			break;	
		}else if( ret ){
			sprintf(acErrMsg,"读取贷款明细异常出错!");
			WRITEMSG			
			goto ErrExit;			
		}
			sprintf(acErrMsg,"ac_id=%ld and old_ac_id=%ld!",s_ln_mst_hst.ac_id,old_ac_id);
			WRITEMSG
		if( old_ac_id!=s_ln_mst_hst.ac_id )	
		{	
			sprintf(acErrMsg,"ac_id=%ld and old_ac_id=%ld!",s_ln_mst_hst.ac_id,old_ac_id);
			WRITEMSG
		    	/* 机构变换时 */
			pub_base_strpack( s_ln_mst_hst.opn_br_no );
			if( strcmp( s_ln_mst_hst.opn_br_no , old_br_no ) )
			{
				TRACE
				/* 第一个机构 */
				if( strcmp( old_br_no , "*****" ) )
				{
					TRACE
					if( line_file==2 )
					{
						/* 打页尾 */
						ret = Make_yeb_sub("CC");
						ERR_DEAL
						ret = Make_yeb_sub("DD");
						ERR_DEAL
						line_file=1;/**new add by martin 070520**/
					}
					/* 打印表体 */
					ret = print_body();
					ERR_DEAL
					/* 打印页尾 */
					ret = Make_yeb_sub("CC");
					ERR_DEAL
					ret = Make_yeb_sub("DD");
					ERR_DEAL
					fclose(fp);
				}
				TRACE
				strcpy( old_br_no , s_ln_mst_hst.opn_br_no );
				
				page=1;
				line_file=1;
				
				/* 删除原文件 */
				ret = pub_rpt_rmfile( s_ln_mst_hst.opn_br_no , filename , 0 );
				ERR_DEAL
				/* 打开文件 */	
				ret = pub_rpt_openfile( &fp , s_ln_mst_hst.opn_br_no , filename );
				ERR_DEAL
				old_ac_id=0;
			} 
	    		TRACE
			/* if( line_file>3 )	 大于3个换页   del by martin 070520*/
			if( line_file>2 )	/* 大于2个换页 */
			{
				line_file=1;
				page++;
				/* 打页尾 */
				ret = Make_yeb_sub("CC");
				ERR_DEAL
				ret = Make_yeb_sub("DD");
				ERR_DEAL
				/***  fprintf(fp,"\f"); del by martin 070517 ***/
				
			}
			if( old_ac_id )
			{
				/* 打印表体 */
				ret = print_body();
				ERR_DEAL
			}			
			ttlnum++;	
			TRACE
			memset( &old_ln_mst_hst,0x00,sizeof(struct ln_mst_hst_c ));
			
			memcpy( &old_ln_mst_hst,&s_ln_mst_hst,sizeof(struct ln_mst_hst_c ));	
				
			old_ac_id=s_ln_mst_hst.ac_id;
			bal_amt=0.00;
			in_amt=0.00;
			out_amt=0.00;
			cmpd_amt=0.00;
TRACE
			if( s_ln_mst_hst.ln_tx_type[0]=='0' )
			{
				bal_amt=s_ln_mst_hst.tx_amt;
			}			
			if( s_ln_mst_hst.ln_tx_type[0]=='1' )
			{
				in_amt=s_ln_mst_hst.tx_amt;
			}
			if( s_ln_mst_hst.ln_tx_type[0]=='2' )
			{
				out_amt=s_ln_mst_hst.tx_amt;
			}
			if( s_ln_mst_hst.ln_tx_type[0]=='3' )
			{
				cmpd_amt=s_ln_mst_hst.tx_amt;
			}		
		}else{
			if( s_ln_mst_hst.ln_tx_type[0]=='0' )
			{
				bal_amt+=s_ln_mst_hst.tx_amt;
			}
			if( s_ln_mst_hst.ln_tx_type[0]=='1' )
			{
				in_amt+=s_ln_mst_hst.tx_amt;
			}
			if( s_ln_mst_hst.ln_tx_type[0]=='2' )
			{
				out_amt+=s_ln_mst_hst.tx_amt;
			}
			if( s_ln_mst_hst.ln_tx_type[0]=='3' )
			{
				cmpd_amt+=s_ln_mst_hst.tx_amt;
			}
		}			
	}		
	Ln_mst_hst_Clo_Sel();
	
OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"生成按揭贷款日终扣款客户回单成功!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--事务回滚--*/
	sprintf(acErrMsg,"生成按揭贷款日终扣款客户回单失败!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

/* 中间函数 */
int Make_yeb_sub( char vrec[3] )
{	
	ret = pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001",vrec,&opncnt,get_ratio_lnajkkhd);
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
	if( line_file>2 )	/* 大于2个换页 */
			{
				line_file=1;
				page++;
			}
	if( page != 1 && line_file==1 )
		      {
		      	fprintf ( fp , "\f" );
		      }
	strcpy( vrec,"AA" );
	/*取贷款户信息*/
	ret = Ln_mst_Sel( g_pub_tx.reply , &s_ln_mst,"ac_id=%ld and ac_seqn=%d ",old_ln_mst_hst.ac_id,old_ln_mst_hst.ac_seqn ); /*读取贷款户信息*/
	if( ret ) 
	{
		sprintf(acErrMsg,"读取贷款户信息error");
		WRITEMSG
		return ret;
	}
			
	ret = Ln_parm_Sel( g_pub_tx.reply , &s_ln_parm,"prdt_no='%s' ",s_ln_mst.prdt_no ); /*读取贷款户信息*/
	if( ret ) 
	{
		sprintf(acErrMsg,"读取贷款户信息error");
		WRITEMSG
		return ret;
	}
	
	ret = Make_yeb_sub(vrec);
	ERR_DEAL
	
		line_file++;
		
GoodExit:
	return 0;
ErrExit:
	return 1;
}


/* 赋值函数 */
int get_ratio_lnajkkhd( bli,bll,str )
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
			sprintf( t_str , "%4d年%2d月%2d日" , qry_date/10000 , qry_date%10000/100 , qry_date%100 );
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , t_str ); 
			break;
		case 'B': /* 机构编码 */
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , old_ln_mst_hst.opn_br_no ); 
			break; 
		case 'C': /* 机构名称 */
			ret = Com_branch_Sel(g_pub_tx.reply, &sCom_branch, "br_no='%s'",
		old_ln_mst_hst.opn_br_no);
			if (ret != 0)
			{
				sprintf(acErrMsg, "查询公共机构编码表错误! [%d]", ret);
				WRITEMSG
				goto ErrExit;
			}
			pub_base_strpack( sCom_branch.br_name ); 
			sprintf( t_str , "%s    ( %5s )" , sCom_branch.br_name , old_ln_mst_hst.opn_br_no); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'D': /* 页码 */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, page); 
			break; 
		case 'E': /* 流水号 */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, s_ln_mst_hst.trace_no); 
			break; 
		case 'F': /* 贷款账号 */
			ret= Mdm_ac_rel_Sel( g_pub_tx.reply ,&g_mdm_ac_rel,"ac_id=%ld and ac_seqn=%d",old_ln_mst_hst.ac_id,s_ln_mst_hst.ac_seqn);
			if( ret )
			{
				return 1;
			}
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt, g_mdm_ac_rel.ac_no);
			memset( &g_mdm_ac_rel, 0x00 , sizeof( struct mdm_ac_rel_c ));
			break;
		case 'G': /* 借据号 */
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str, fmt, s_ln_mst.pact_no);
			break;
		case 'H': /* 贷款户名 */
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt, s_ln_mst.name);
			break;
		case 'I': /* 产品名称 */			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, s_ln_parm.title ); 
			break;
		case 'T': /* 产品 */
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str, fmt, s_ln_mst.prdt_no);
			break;
			
		case 'J': /* 还息金额(大写) */
			sprintf(fmt,"%%-%ds",bll);
			sprintf(str, fmt, pub_rpt_pcask(bal_amt+out_amt+in_amt+cmpd_amt));
			break;
		case 'K': /* 还息金额(小写) */
			/* 加逗号 */
			sprintf( amt_tmp, "%.2lf" , bal_amt+out_amt+in_amt+cmpd_amt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		
		case 'L': /* 本金金额 */
			/* 加逗号 */
			sprintf( amt_tmp, "%.2lf" , bal_amt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'Y': /* 利息金额 */
			/* 加逗号 */
			sprintf( amt_tmp, "%.2lf" , in_amt+out_amt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'Z': /*罚息金额*/
			/* 加逗号 */
			sprintf( amt_tmp, "%.2lf" , cmpd_amt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'Q': /* 扣款账号 */
			ret= Mdm_ac_rel_Sel( g_pub_tx.reply ,&g_mdm_ac_rel,"ac_id=%ld",s_ln_mst.repay_ac_id);
			if( ret )
			{
				return 1;
			}
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt, g_mdm_ac_rel.ac_no);
			memset( &g_mdm_ac_rel, 0x00 , sizeof( struct mdm_ac_rel_c ));
			break;
		case 'M': /* 合同利率 */
			sprintf( str, "%8.5lf‰", s_ln_mst.rate);
			break;
		case 'X': /* 逾期利率 */
				sprintf(acErrMsg, "逾期利率! [%lf]", s_ln_mst.over_rate);
				WRITEMSG		
			memset( &str ,0x0, strlen(str));
			sprintf( str, "%8.5lf‰", s_ln_mst.over_rate);
			break;						
		case 'O': /* 本金总额 */
			/* 加逗号 */
			sprintf( amt_tmp, "%.2lf" , s_ln_mst.ttl_prvd_amt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'P': /* 尚欠本金 */
			/* 加逗号 */
			sprintf( amt_tmp, "%.2lf" , s_ln_mst.bal ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'R': /* 起始日期 */
		 	sprintf( t_str , "%4d-%2d-%2d" , s_ln_mst.opn_date/10000 , s_ln_mst.opn_date%10000/100 , 
										s_ln_mst.opn_date%100 );
		  	sprintf(fmt,"%%-%ds",bll);
			sprintf( str , fmt , t_str );
			break;
		/***应长治要求,更改为结息这一天的日期,  modify by wanglei 20060120
		case 'S': * 终止日期 *
		  	sprintf( t_str , "%4d-%2d-%2d" , s_ln_mst.mtr_date/10000 , s_ln_mst.mtr_date%10000/100 ,
									  s_ln_mst.mtr_date%100 );
			sprintf(fmt,"%%-%ds",bll);
			sprintf( str , fmt , t_str );
			break;
		***/
		case 'S': /* 终止日期 */
		  	sprintf( t_str , "%4d-%2d-%2d" , g_pub_tx.tx_date/10000 , g_pub_tx.tx_date%10000/100 ,
									  g_pub_tx.tx_date%100 );
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
