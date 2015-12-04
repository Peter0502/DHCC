/*************************************************************
* 文 件 名: rpt970.c
* 功能描述：储蓄结息清单
*
* 作    者: rob
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
#include "com_sys_parm_c.h"
#include "com_branch_c.h"
#include "intst_hst_c.h"

struct com_sys_parm_c	s_com_sys_parm;
struct intst_hst_c	s_intst_hst;

int page=0;
int line=0;
int line_file=0;
char vpfm[21];
char t_str[41];
int opncnt;
FILE    *fp; 
int ret = 0;


int get_ratio_cxxhqd(int bli,int bll,char str[100]);

rpt970()
{
	int flag=0;
	int ttlnum;
	long tmp_date;
	char fname[100];
	char filename[100];
	char wherelist[1024];  /**查询条件**/
	char tmpstr[100];
	char tmp_str[60];
	char old_br_no[6];
	
	memset( wherelist,0,sizeof(wherelist) );
	memset( &s_com_sys_parm , 0x00 , sizeof(struct com_sys_parm_c));
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
	strcpy( filename , "BJcxjxqd" );	
	strcpy(vpfm, "cxjxqd" );
*/	
	strcpy( filename , "RPT970");
	strcpy(vpfm, "RPT970");
	ret = Intst_hst_Dec_Sel(g_pub_tx.reply,"ac_ind='1' and pt_ind='1' and intst_date=%ld order by br_no ,prdt_no,ac_id",g_pub_tx.tx_date );
	ERR_DEAL
	ttlnum=0;
	while(1)
	{
		ret = Intst_hst_Fet_Sel( &s_intst_hst , g_pub_tx.reply );
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
		}
		else if( ret )
		{
			sprintf(acErrMsg,"读取结息表异常出错!");
			WRITEMSG			
			goto ErrExit;			
		}
		
		/* 机构变换时 */
		pub_base_strpack(s_intst_hst.br_no );
		if( strcmp( s_intst_hst.br_no , old_br_no ) )
		{
			/* 第一个机构 */
			if( strcmp( old_br_no , "*****" ) )
			{
				/* 打印页尾 */
				ret = print_tail();
				ERR_DEAL
				fclose(fp);
			}
			strcpy( old_br_no , s_intst_hst.br_no );
			page=1;
			line_file=1;
			/* 删除原文件 */
			ret = pub_rpt_rmfile(s_intst_hst.br_no , filename , 0 );
			ERR_DEAL
			/* 打开文件 */	
			ret = pub_rpt_openfile( &fp, s_intst_hst.br_no , filename );
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
	Intst_hst_Clo_Sel();
	
OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"生成储蓄计息交易清单成功!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--事务回滚--*/
	sprintf(acErrMsg,"生成储蓄计息清单失败!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

/* 中间函数 */
int Make_yeb_sub( char vrec[3] )
{	
	ret=pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001",vrec,&opncnt,get_ratio_cxxhqd);
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
int get_ratio_cxxhqd( bli,bll,str )
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
	long tmpdate=0;
	char tmpname[51];
	char tmpac_no[51];
	struct com_branch_c sCom_branch;
	struct mdm_ac_rel_c s_mdm_ac_rel;
	memset(&sCom_branch, 0x0, sizeof(struct com_branch_c));
	memset(&s_mdm_ac_rel, 0x0, sizeof(struct mdm_ac_rel_c));
	
	switch( bli ) 
	{ 
		case 'A': /* 交易日期 */
			pub_base_deadlineD(g_pub_tx.tx_date,-1,&tmpdate);
			sprintf( t_str , "%4d年%2d月%2d日" , tmpdate/10000 , tmpdate%10000/100 , tmpdate%100 );
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , t_str ); 
			break;
		case 'B': /* 机构编码 */
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , s_intst_hst.br_no ); 
			break; 
		case 'C': /* 机构名称 */
			ret = Com_branch_Sel(g_pub_tx.reply, &sCom_branch, "br_no='%s'",s_intst_hst.br_no);
			if (ret != 0)
			{
				sprintf(acErrMsg, "查询公共机构编码表错误! [%d]", ret);
				WRITEMSG
				goto ErrExit;
			}
			pub_base_strpack( sCom_branch.br_name ); 
			sprintf( t_str , "%s    ( %5s )" , sCom_branch.br_name , s_intst_hst.br_no); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'D': /* 页码 */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, page); 
			break; 
		case 'E': /* 账     号 */
			ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&s_mdm_ac_rel,"ac_id=%ld and ac_seqn=1 ",s_intst_hst.ac_id);
			if( ret==100 )
			{
				ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&s_mdm_ac_rel,"ac_id=%ld and ac_seqn=9999 ",s_intst_hst.ac_id);
			}
			if( ret )
			{
				sprintf(acErrMsg, "取账户错误! [%d][%d]", s_intst_hst.ac_id,ret);
				WRITEMSG
				goto ErrExit;
			}
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt , s_mdm_ac_rel.ac_no); 
			break; 
		case 'M': /*户名*/
			ret=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,"ac_id=%ld and ac_seqn=%d ",s_intst_hst.ac_id,s_intst_hst.ac_seqn);
			if( ret )
			{
				sprintf(acErrMsg, "取户名错误! [%d][%d][%d]", s_intst_hst.ac_id,s_intst_hst.ac_seqn,ret);
				WRITEMSG
				goto ErrExit;
			}
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt , g_dd_mst.name); 
			break; 
		case 'O': /* 结息前余额 */
			sprintf( amt_tmp, "%.2lf" , s_intst_hst.bic_bal ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'G': /*利息积数*/
			sprintf( amt_tmp, "%.2lf" , s_intst_hst.bic_acm ); 
			/**pub_rpt_flttomon( amt_tmp,amt_tmp );**/
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'H': /* 产品 */
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt , s_intst_hst.prdt_no);
			break;
		case 'J': /* 税前利息 */
			sprintf( amt_tmp, "%.2lf" , s_intst_hst.fct_intst ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
							
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'F': /* 利息税 */
			sprintf( amt_tmp, "%.2lf" , s_intst_hst.intst_tax ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
							
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'K': /* 税后利息 */			
			sprintf( amt_tmp, "%.2lf" , s_intst_hst.fct_intst-s_intst_hst.intst_tax ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
							
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		/***
		case 'R': * 序号 *
			sprintf(fmt,"%%%dld",bll);
			sprintf(str, fmt, s_intst_hst.ac_seqn);
			break;		
		***/
		default : 
			memset( str,' ',bll ); 
			break; 
	} 
	
GoodExit: 
	return 0; 
ErrExit: 
	return 1; 
}
