/*************************************************************
* 文 件 名: rpt035.c
* 功能描述：每月20号贷款结息清单
*
* 作    者: jane
* 完成日期: 2005年09月15日
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
#include "ln_acm_reg_c.h"
#include "com_branch_c.h"

struct ln_acm_reg_c s_ln_acm_reg;
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

double tol_jx_amt=0.00;
long   tol_jx_cnt=0;

char pay_c_sts[7];
char clear_flg[2];
long days=0;
long qry_date=0;
char intst_type_name[9];
		
int get_ratio_lnjxqd(int bli,int bll,char str[100]);

rpt035()
{
	int flag=0;
	int del_ind=0;
	int ttlnum;

	char fname[100];
	char filename[100];
	char wherelist[1024];  /**查询条件**/
	char tmpstr[300];

	char tmp_str[60];
	char old_br_no[6];
	char old_intst_type[2];
	long old_trace_no=0;
	
	memset( wherelist,0,sizeof(wherelist) );
	memset( &s_ln_acm_reg , 0x00 , sizeof(struct ln_acm_reg_c));
	
	strcpy( old_br_no , "*****" );
	strcpy( old_intst_type,"*" );
	
	/*计算交易日期*/
	qry_date=g_pub_tx.tx_date;
	/***
	pub_base_deadlineD( g_pub_tx.tx_date, -1 ,&qry_date );	
	*****/
	
	/*确定日终最小流水号*/
	ret= sql_min_long( "trace_log_rz", "trace_no", &min_trace_no, "tx_date=%ld and tel='9999'",qry_date);
	if(ret)  goto ErrExit;

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
	strcpy( filename , "BJlnjxqd" );	
	strcpy(vpfm, "lnjxqd" );
*/	
	strcpy( filename , "RPT035");
	strcpy(vpfm, "RPT035");
	/* 本金积数结息清单 */
	/* 条件:流水号大于等于日终起始流水号 */
	tol_jx_cnt=0;
	tol_jx_amt=0.00;
	memset( &tmpstr,0x0,strlen(tmpstr));	
	memset( &wherelist,0x0,strlen(wherelist));
	sprintf( tmpstr," trace_no>=%ld and cal_date=%ld and intst_type in('1') and " ,min_trace_no,qry_date );
	strcat( wherelist,tmpstr );	
	
	/**组成查询**/
	strcat(wherelist," 1=1 ORDER BY intst_type, br_no, ac_id, ac_seqn ");
	vtcp_log( "WHERE [%s]",wherelist );
	
	ret=Ln_acm_reg_Dec_Sel( g_pub_tx.reply, wherelist );
	ERR_DEAL

		ttlnum=0;
		strcpy( intst_type_name, "本金积数" );	
	while(1)
	{
		ret=Ln_acm_reg_Fet_Sel( &s_ln_acm_reg, g_pub_tx.reply );
		if( ret==100 ) 
		{
			if( ttlnum )
			{
				/* 打页尾 */
				ret = print_tail();
				ERR_DEAL
				/* 打总计 */
				ret = print_zj();
				ERR_DEAL				
				/* 打换页 */
				ret = print_NY();
				ERR_DEAL
				fclose(fp);
			}
			break;	
		}else if( ret ){
			sprintf(acErrMsg,"读取贷款结息登记簿异常出错!");
			WRITEMSG			
			goto ErrExit;			
		}
		del_ind++;	
		/*取贷款户信息*/
		ret = Ln_mst_Sel( g_pub_tx.reply , &s_ln_mst,"ac_id=%ld and ac_seqn=%d ",s_ln_acm_reg.ac_id,s_ln_acm_reg.ac_seqn ); /*读取贷款户信息*/
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
									
		/* 机构变换时 */
		pub_base_strpack( s_ln_acm_reg.br_no );
		if( strcmp( s_ln_acm_reg.br_no , old_br_no ) )
		{
			/* 第一个机构 */
			if( strcmp( old_br_no , "*****" ) )
			{
				/* 打印页尾 */
				ret = print_tail();
				ERR_DEAL
				/* 打总计 */
				ret = print_zj();
				ERR_DEAL				
				/* 打换页 */
				ret = print_NY();
				ERR_DEAL				
				fclose(fp);
			}
			
			strcpy( old_br_no , s_ln_acm_reg.br_no );
			
			page=1;
			line_file=0;
			
			tol_jx_cnt=0;
			tol_jx_amt=0.00;
	
			/* 删除原文件 */
			ret = pub_rpt_rmfile( s_ln_acm_reg.br_no , filename , 0 );
			ERR_DEAL
			/* 打开文件 */	
			ret = pub_rpt_openfile( &fp , s_ln_acm_reg.br_no , filename );
			ERR_DEAL
			/* 打印页首 */
			ret = print_head();
			ERR_DEAL
		} 
					  
		/* 总计信息 */
		tol_jx_cnt++;
		tol_jx_amt+=s_ln_acm_reg.intst;
		
    		/* 打印信息 */		
		ret = print_body();
		ERR_DEAL
		line_file++;
			
		if( line_file>=50 )	/* 大于50行换页 */
		{
			line_file=0;
			page++;
			/* 打印页尾 */
			ret = print_tail();
			ERR_DEAL				
			/* 打换页 */
			ret = print_NY();
			ERR_DEAL			
			/* 打印页首 */
			ret = print_head();
			ERR_DEAL
		}
		ttlnum++;
	}		
	Ln_acm_reg_Clo_Sel();

	/* 表内积数结息清单 */
	/* 条件:流水号大于等于日终起始流水号 */
	tol_jx_cnt=0;
	tol_jx_amt=0.00;
	strcpy( old_br_no , "*****" );
	strcpy( old_intst_type,"*" );
	memset( &tmpstr,0x0, strlen( tmpstr ));
	memset( &wherelist,0x0,strlen(wherelist));
	sprintf( tmpstr," trace_no>=%ld and end_date=%ld and intst_type in ('2') and " ,min_trace_no,qry_date );
	strcat( wherelist,tmpstr );	
	
	/**组成查询**/
	strcat(wherelist," 1=1 ORDER BY intst_type, br_no, ac_id, ac_seqn ");
	vtcp_log( "WHERE [%s]",wherelist );
	
	ret=Ln_acm_reg_Dec_Sel( g_pub_tx.reply, wherelist );
	ERR_DEAL

		ttlnum=0;
		strcpy( intst_type_name, "表内积数" );	
	while(1)
	{
		ret=Ln_acm_reg_Fet_Sel( &s_ln_acm_reg, g_pub_tx.reply );
		if( ret==100 ) 
		{
			if( ttlnum )
			{
				/* 打页尾 */
				ret = print_tail();
				ERR_DEAL
				/* 打总计 */
				ret = print_zj();
				ERR_DEAL				
				/* 打换页 */
				ret = print_NY();
				ERR_DEAL
				fclose(fp);
			}
			break;	
		}else if( ret ){
			sprintf(acErrMsg,"读取贷款结息登记簿异常出错!");
			WRITEMSG			
			goto ErrExit;			
		}
		del_ind++;	
		/*取贷款户信息*/
		ret = Ln_mst_Sel( g_pub_tx.reply , &s_ln_mst,"ac_id=%ld and ac_seqn=%d ",s_ln_acm_reg.ac_id,s_ln_acm_reg.ac_seqn ); /*读取贷款户信息*/
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
										
		/* 机构变换时 */
		pub_base_strpack( s_ln_acm_reg.br_no );
		if( strcmp( s_ln_acm_reg.br_no , old_br_no ) )
		{
			/* 第一个机构 */
			if( strcmp( old_br_no , "*****" ) )
			{
				/* 打印页尾 */
				ret = print_tail();
				ERR_DEAL
				/* 打总计 */
				ret = print_zj();
				ERR_DEAL				
				/* 打换页 */
				ret = print_NY();
				ERR_DEAL
				fclose(fp);
			}
			
			strcpy( old_br_no , s_ln_acm_reg.br_no );
			
			page=1;
			line_file=0;
			tol_jx_cnt=0;
			tol_jx_amt=0.00;
						
			/* 删除原文件 */
			if( del_ind==0 ) 
			{
				ret = pub_rpt_rmfile( s_ln_acm_reg.br_no , filename , 0 );
				ERR_DEAL
			}
			/* 打开文件 */	
			ret = pub_rpt_openfile( &fp , s_ln_acm_reg.br_no , filename );
			ERR_DEAL
			/* 打印页首 */
			ret = print_head();
			ERR_DEAL
		} 
					  
    		/* 打印信息 */		
		ret = print_body();
		ERR_DEAL
		line_file++;
		
		/* 总计信息 */
		tol_jx_cnt++;
		tol_jx_amt+=s_ln_acm_reg.intst;
		
		if( line_file>=50 )	/* 大于50行换页 */
		{
			line_file=0;
			page++;
			/* 打印页尾 */
			ret = print_tail();
			ERR_DEAL				
			/* 打换页 */
			ret = print_NY();
			ERR_DEAL
			/* 打印页首 */
			ret = print_head();
			ERR_DEAL
		}
		ttlnum++;
	}		
	Ln_acm_reg_Clo_Sel();
	
	/* 表外积数结息清单 */
	/* 条件:流水号大于等于日终起始流水号 */
	tol_jx_cnt=0;
	tol_jx_amt=0.00;
	strcpy( old_br_no , "*****" );
	strcpy( old_intst_type,"*" );
	memset( &tmpstr,0x0, strlen( tmpstr ));
	memset( &wherelist,0x0,strlen(wherelist));
	sprintf( tmpstr," trace_no>=%ld and end_date=%ld and intst_type in ('3') and " ,min_trace_no,qry_date );
	strcat( wherelist,tmpstr );	
	
	/**组成查询**/
	strcat(wherelist," 1=1 ORDER BY intst_type, br_no, ac_id, ac_seqn ");
	vtcp_log( "WHERE [%s]",wherelist );
	
	ret=Ln_acm_reg_Dec_Sel( g_pub_tx.reply, wherelist );
	ERR_DEAL

		ttlnum=0;
		strcpy( intst_type_name, "表外积数" );	
	while(1)
	{
		ret=Ln_acm_reg_Fet_Sel( &s_ln_acm_reg, g_pub_tx.reply );
		if( ret==100 ) 
		{
			if( ttlnum )
			{
				/* 打页尾 */
				ret = print_tail();
				ERR_DEAL
				/* 打总计 */
				ret = print_zj();
				ERR_DEAL				
				/* 打换页 */
				ret = print_NY();
				ERR_DEAL
				fclose(fp);
			}
			break;	
		}else if( ret ){
			sprintf(acErrMsg,"读取贷款结息登记簿异常出错!");
			WRITEMSG			
			goto ErrExit;			
		}
		del_ind++;	
		/*取贷款户信息*/
		ret = Ln_mst_Sel( g_pub_tx.reply , &s_ln_mst,"ac_id=%ld and ac_seqn=%d ",s_ln_acm_reg.ac_id,s_ln_acm_reg.ac_seqn ); /*读取贷款户信息*/
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
								
		/* 机构变换时 */
		pub_base_strpack( s_ln_acm_reg.br_no );
		if( strcmp( s_ln_acm_reg.br_no , old_br_no ) )
		{
			/* 第一个机构 */
			if( strcmp( old_br_no , "*****" ) )
			{
				/* 打印页尾 */
				ret = print_tail();
				ERR_DEAL
				/* 打总计 */
				ret = print_zj();
				ERR_DEAL				
				/* 打换页 */
				ret = print_NY();
				ERR_DEAL
				fclose(fp);
			}
			
			strcpy( old_br_no , s_ln_acm_reg.br_no );
			
			page=1;
			line_file=0;
			tol_jx_cnt=0;
			tol_jx_amt=0.00;
						
			/* 删除原文件 */
			if( del_ind==0 ) 
			{
				ret = pub_rpt_rmfile( s_ln_acm_reg.br_no , filename , 0 );
				ERR_DEAL
			}
			/* 打开文件 */	
			ret = pub_rpt_openfile( &fp , s_ln_acm_reg.br_no , filename );
			ERR_DEAL
			/* 打印页首 */
			ret = print_head();
			ERR_DEAL
		} 
					  
    		/* 打印信息 */		
		ret = print_body();
		ERR_DEAL
		line_file++;
		
		/* 总计信息 */
		tol_jx_cnt++;
		tol_jx_amt+=s_ln_acm_reg.intst;
		
		if( line_file>=50 )	/* 大于50行换页 */
		{
			line_file=0;
			page++;
			/* 打印页尾 */
			ret = print_tail();
			ERR_DEAL				
			/* 打换页 */
			ret = print_NY();
			ERR_DEAL
			/* 打印页首 */
			ret = print_head();
			ERR_DEAL
		}
		ttlnum++;
	}		
	Ln_acm_reg_Clo_Sel();	
	/* 复利积数结息清单 */
	/* 条件:流水号大于等于日终起始流水号 */
	tol_jx_cnt=0;
	tol_jx_amt=0.00;
	strcpy( old_br_no , "*****" );
	strcpy( old_intst_type,"*" );
	memset( &tmpstr,0x0, strlen( tmpstr ));
	memset( &wherelist,0x0,strlen(wherelist));
	sprintf( tmpstr," trace_no>=%ld and end_date=%ld and intst_type in ('4') and " ,min_trace_no,qry_date );
	strcat( wherelist,tmpstr );	
	
	/**组成查询**/
	strcat(wherelist," 1=1 ORDER BY intst_type, br_no, ac_id, ac_seqn ");
	vtcp_log( "WHERE [%s]",wherelist );
	
	ret=Ln_acm_reg_Dec_Sel( g_pub_tx.reply, wherelist );
	ERR_DEAL

		ttlnum=0;
		strcpy( intst_type_name, "复利积数" );	
	while(1)
	{
		ret=Ln_acm_reg_Fet_Sel( &s_ln_acm_reg, g_pub_tx.reply );
		if( ret==100 ) 
		{
			if( ttlnum )
			{
				/* 打页尾 */
				ret = print_tail();
				ERR_DEAL
				/* 打总计 */
				ret = print_zj();
				ERR_DEAL				
				/* 打换页 */
				ret = print_NY();
				ERR_DEAL
				fclose(fp);
			}
			break;	
		}else if( ret ){
			sprintf(acErrMsg,"读取贷款结息登记簿异常出错!");
			WRITEMSG			
			goto ErrExit;			
		}	
		del_ind++;
		/*取贷款户信息*/
		ret = Ln_mst_Sel( g_pub_tx.reply , &s_ln_mst,"ac_id=%ld and ac_seqn=%d ",s_ln_acm_reg.ac_id,s_ln_acm_reg.ac_seqn ); /*读取贷款户信息*/
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
									
		/* 机构变换时 */
		pub_base_strpack( s_ln_acm_reg.br_no );
		if( strcmp( s_ln_acm_reg.br_no , old_br_no ) )
		{
			/* 第一个机构 */
			if( strcmp( old_br_no , "*****" ) )
			{
				/* 打印页尾 */
				ret = print_tail();
				ERR_DEAL
				/* 打总计 */
				ret = print_zj();
				ERR_DEAL				
				/* 打换页 */
				ret = print_NY();
				ERR_DEAL
				fclose(fp);
			}
			
			strcpy( old_br_no , s_ln_acm_reg.br_no );
			
			page=1;
			line_file=0;
			tol_jx_cnt=0;
			tol_jx_amt=0.00;
				
			if( del_ind==0 ) 
			{			
				/* 删除原文件 */
				ret = pub_rpt_rmfile( s_ln_acm_reg.br_no , filename , 0 );
				ERR_DEAL
			}
			/* 打开文件 */	
			ret = pub_rpt_openfile( &fp , s_ln_acm_reg.br_no , filename );
			ERR_DEAL
			/* 打印页首 */
			ret = print_head();
			ERR_DEAL
		} 
					  
    		/* 打印信息 */		
		ret = print_body();
		ERR_DEAL
		line_file++;
		
		/* 总计信息 */
		tol_jx_cnt++;
		tol_jx_amt+=s_ln_acm_reg.intst;
		
		if( line_file>=50 )	/* 大于50行换页 */
		{
			line_file=0;
			page++;
			/* 打印页尾 */
			ret = print_tail();
			ERR_DEAL		
			/* 打换页 */
			ret = print_NY();
			ERR_DEAL
			/* 打印页首 */
			ret = print_head();
			ERR_DEAL
		}
		ttlnum++;
	}		
	Ln_acm_reg_Clo_Sel();	
	
OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"生成贷款结息清单成功!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--事务回滚--*/
	sprintf(acErrMsg,"生成贷款结息清单失败!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

/* 中间函数 */
int Make_yeb_sub( char vrec[3] )
{	
	ret = pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001",vrec,&opncnt,get_ratio_lnjxqd);
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

/* 打印换页 */
int print_NY( )
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

/* 打印总计 */
int print_zj( )
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
int get_ratio_lnjxqd( bli,bll,str )
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
	days= pub_base_CalTrueDays( s_ln_acm_reg.beg_date,s_ln_acm_reg.end_date);	
	switch( bli ) 
	{ 
		case 'A': /* 交易日期 */
			sprintf( t_str , "%4d年%2d月%2d日" , qry_date/10000 , qry_date%10000/100 , qry_date%100 );
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , t_str ); 
			break;
		case 'B': /* 机构编码 */
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , s_ln_acm_reg.br_no ); 
			break; 
		case 'C': /* 机构名称 */
			ret = Com_branch_Sel(g_pub_tx.reply, &sCom_branch, "br_no='%s'",
		s_ln_acm_reg.br_no);
			if (ret != 0)
			{
				sprintf(acErrMsg, "查询公共机构编码表错误! [%d]", ret);
				WRITEMSG
				goto ErrExit;
			}
			pub_base_strpack( sCom_branch.br_name ); 
			sprintf( t_str , "%s    ( %5s )" , sCom_branch.br_name , s_ln_acm_reg.br_no); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'D': /* 页码 */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, page); 
			break; 
		case 'E': /* 借据号 */
			sprintf(fmt,"%%-%ds",bll); 
			sprintf(str, fmt, s_ln_mst.pact_no); 
			break; 
		case 'F': /* 贷款账号 */
			ret= Mdm_ac_rel_Sel( g_pub_tx.reply ,&g_mdm_ac_rel,"ac_id=%ld and ac_seqn=%d",s_ln_acm_reg.ac_id,s_ln_acm_reg.ac_seqn);
			if( ret )
			{
				return 1;
			}
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt, g_mdm_ac_rel.ac_no);
			memset( &g_mdm_ac_rel, 0x00 , sizeof( struct mdm_ac_rel_c ));
			break;
		case 'G': /* 积数 */						
			sprintf( amt_tmp, "%.2lf" , s_ln_acm_reg.intst_acm ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'H': /* 结息金额 */
			/* 加逗号 */
			sprintf( amt_tmp, "%.2lf" , s_ln_acm_reg.intst ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'I': /* 贷款户名 */
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt, s_ln_mst.name);

			break;
		/* case 'J': 产品 
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str, fmt, s_ln_mst.prdt_no);
			break;
		*/
		case 'J': /* 产品名称 */			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, s_ln_parm.title ); 
			break;
		case 'K': /* 积数类型 */			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, intst_type_name ); 
			break;
		case 'L': /* 利率 */
			if(ret)	return 1;		
			sprintf( str, "%8.5lf", s_ln_acm_reg.rate ); 
			break;
		case 'O': /* 状态 */
			sprintf(fmt, "%%-%ds", bll);		
			sprintf(str, fmt, clear_flg);
			break;

		case 'X': /* 起始日期 */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, s_ln_acm_reg.beg_date); 
			break; 
		case 'Y': /* 终止日期 */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, s_ln_acm_reg.end_date); 
			break; 	
		case 'Z': /* 天数 */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, days); 
			break; 
		case 'M': /* 户数总计 */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, tol_jx_cnt); 
			break; 
		case 'N': /* 金额总计 */
			/* 加逗号 */
			sprintf( amt_tmp, "%.2lf" , tol_jx_amt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
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
