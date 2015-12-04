/*************************************************************
* 文 件 名: rpt081.c
* 功能描述：日终贷款扣款清单(按还款计划扣划)
*
* 作    者: lance
* 完成日期: 2003年11月21日
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
#include "ln_pln_inf_c.h"
#include "com_branch_c.h"

struct ln_pln_inf_c s_ln_pln_inf;
struct ln_mst_c s_ln_mst;
struct ln_parm_c s_ln_parm;

int page=0;
int line=0;
int line_file=0;
char vpfm[21];
char t_str[41];
int opncnt;
char pay_c_sts[7];
FILE    *fp; 
int ret = 0;
long qry_date=0;
	
int get_ratio_lnjhkkqd(int bli,int bll,char str[100]);

rpt081()
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
	memset( &s_ln_pln_inf , 0x00 , sizeof(struct ln_pln_inf_c));
	
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
	
	strcpy( filename , "BRlnjhkkqd" );	
	strcpy(vpfm, "lnjhkkqd" );
	
	/*计算交易日期*/
	pub_base_deadlineD( g_pub_tx.tx_date, -1 ,&qry_date );	
	
	/* 条件:交易代码为"G082" */
	sprintf( tmpstr," lst_date=%ld and " , qry_date );
	strcat( wherelist,tmpstr );	
	
	/**组成查询**/
	strcat(wherelist," 1=1 ORDER BY br_no,pay_ind , ac_id ,ac_seqn ");
	vtcp_log( "WHERE [%s]",wherelist );
	
	ret=Ln_pln_inf_Dec_Sel( g_pub_tx.reply, wherelist );
	ERR_DEAL

		ttlnum=0;
	
	while(1)
	{
		ret=Ln_pln_inf_Fet_Sel( &s_ln_pln_inf, g_pub_tx.reply );
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
			sprintf(acErrMsg,"读取贷款还款计划表异常出错!");
			WRITEMSG			
			goto ErrExit;			
		}	
		/*取贷款户信息*/
		ret = Ln_mst_Sel( g_pub_tx.reply , &s_ln_mst,"ac_id=%ld and ac_seqn=%d ",s_ln_pln_inf.ac_id,s_ln_pln_inf.ac_seqn ); /*读取贷款户信息*/
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
		pub_base_strpack( s_ln_pln_inf.br_no );
		if( strcmp( s_ln_pln_inf.br_no , old_br_no ) )
		{
			/* 第一个机构 */
			if( strcmp( old_br_no , "*****" ) )
			{
				/* 打印页尾 */
				ret = print_tail();
				ERR_DEAL
				fclose(fp);
			}
			
			strcpy( old_br_no , s_ln_pln_inf.br_no );
			
			page=1;
			line_file=1;
			
			/* 删除原文件 */
			ret = pub_rpt_rmfile( s_ln_pln_inf.br_no , filename , 0 );
			ERR_DEAL
			/* 打开文件 */	
			ret = pub_rpt_openfile( &fp , s_ln_pln_inf.br_no , filename );
			ERR_DEAL
			/* 打印页首 */
			ret = print_head();
			ERR_DEAL
		}
		if( s_ln_pln_inf.pay_ind[0]=='1' )
		{
			strcpy( pay_c_sts,"还  清");
		}else if(s_ln_pln_inf.pay_ind[0]=='0' ) {
			strcpy( pay_c_sts,"未还清");
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
	Ln_pln_inf_Clo_Sel();
	
OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"生成日终贷款扣款清单(按还款计划扣划)成功!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--事务回滚--*/
	sprintf(acErrMsg,"生成日终贷款扣款清单(按还款计划扣划)失败!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

/* 中间函数 */
int Make_yeb_sub( char vrec[3] )
{	
	ret = pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001",vrec,&opncnt,get_ratio_lnjhkkqd);
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
int get_ratio_lnjhkkqd( bli,bll,str )
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
			sprintf( str , fmt , s_ln_pln_inf.br_no ); 
			break; 
		case 'C': /* 机构名称 */
			ret = Com_branch_Sel(g_pub_tx.reply, &sCom_branch, "br_no='%s'",
		s_ln_pln_inf.br_no);
			if (ret != 0)
			{
				sprintf(acErrMsg, "查询公共机构编码表错误! [%d]", ret);
				WRITEMSG
				goto ErrExit;
			}
			pub_base_strpack( sCom_branch.br_name ); 
			sprintf( t_str , "%s    ( %5s )" , sCom_branch.br_name , s_ln_pln_inf.br_no); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'D': /* 页码 */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, page); 
			break; 
		case 'F': /* 贷款账号 */
			ret= Mdm_ac_rel_Sel( g_pub_tx.reply ,&g_mdm_ac_rel,"ac_id=%ld and ac_seqn=%d",s_ln_pln_inf.ac_id,s_ln_pln_inf.ac_seqn);
			if( ret )
			{
				return 1;
			}
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt, g_mdm_ac_rel.ac_no);
			memset( &g_mdm_ac_rel, 0x00 , sizeof( struct mdm_ac_rel_c ));
			break;
		case 'G': /* 应偿还本金 */						
			/* 加逗号 */
			sprintf( amt_tmp, "%.2lf" , s_ln_pln_inf.curr_amt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'H': /* 实际偿还金额 */
			/* 加逗号 */
			sprintf( amt_tmp, "%.2lf" , s_ln_pln_inf.true_amt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'I': /* 贷款户名 */
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt, s_ln_mst.name);

			break;
		case 'J': /* 产品 */
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str, fmt, s_ln_mst.prdt_no);
			break;
		case 'K': /* 还款日期 */
			sprintf( t_str , "%4d-%2d-%2d" , s_ln_pln_inf.end_date/10000 , s_ln_pln_inf.end_date%10000/100 , s_ln_pln_inf.end_date%100 );
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , t_str );
			break;
		case 'M': /* 备注 */
			sprintf(fmt, "%%-%ds", bll);		
			sprintf(str, fmt, s_ln_pln_inf.content);
			break;	
		case 'O': /* 标志 */
			sprintf(fmt, "%%-%ds", bll);		
			sprintf(str, fmt, pay_c_sts);
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
