/*************************************************************
* 文 件 名: rpt065.c
* 功能描述：贷款转逾期传票，扣款传票综合版本
*
* 作    者: dadary
* 完成日期: 2006年12月17日
*
* 注：格式文件为RPT065.tbl
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
/**
#define TABLE_RECORD 3
**/
#define TABLE_RECORD 2
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
#include "com_sys_parm_c.h"
#include "ln_parm_c.h"
#include "com_item_c.h"
#include "ln_mst_c.h"
#include "dc_acc_rel_c.h"
#include "dc_log_bk_c.h"

#define   TABLENUM	3



struct trace_log_bk_c 	s_trace_log_bk;
struct ln_mst_hst_c 	s_ln_mst_hst;
struct com_sys_parm_c 	sComSysParm;
struct dc_log_bk_c	s_dc_log_bk;


int page=0;
int line=0;
int line_file=0;
char vpfm[21];
char t_str[41];
int opncnt;
FILE    *fp; 
int ret = 0;

double bal_amt=0.00;
double in_amt=0.00;
double out_amt=0.00;
double cmpd_amt=0.00;


long qry_date=0;


char acc_hrt_desp[60];	/***改变会计分录打印的方式***/

int get_ratio_mbfesd(int bli,int bll,char str[100]);

/***注意了:这里是打印一个个表单,每一页放4个表单*****/
rpt065()
{
	int flag=0;
	int ttlnum;


	char fname[100];
	char filename[100];
	char wherelist[1024];  /**查询条件**/
	char tmpstr[300];
	char amt_tmp[21];

	char tmp_str[60];
	char old_br_no[6];
	long old_trace_no=0;
	char acc_hrt_tmp[8];
	char form_acc_hrt_tmp[8];
	
	memset( wherelist,0,sizeof(wherelist) );
	memset(fname,0x00,sizeof(fname));
	memset(tmpstr,0x00,sizeof(tmpstr));
	memset(tmp_str,0x00,sizeof(tmp_str));
	memset(old_br_no,0x00,sizeof(old_br_no));
	
	
	memset( &s_trace_log_bk , 0x00 , sizeof(s_trace_log_bk));
	memset( &s_dc_log_bk 	, 0x00 , sizeof(s_dc_log_bk));
	
	
	strcpy( old_br_no , "*****" );
		

	vtcp_log("[%s][%d]开始生成贷款逾期以及扣款传票!\n",__FILE__,__LINE__);   
	ret=sql_begin(); /*打开事务*/
	if(ret != 0)
	{
		sprintf( acErrMsg, "打开事务失败!!!" );
		WRITEMSG
		goto ErrExit;
	}
	ret  = Com_sys_parm_Sel( g_pub_tx.reply , &sComSysParm , "1=1" );
	if( ret  ) 
	{ 
		sprintf(acErrMsg, "SELECT com_sys_parm error![%d]", ret  ); 
		WRITEMSG 
		goto ErrExit; 
	}
	
	
	qry_date=g_pub_tx.tx_date;
	
	
	
	/**------- 初始化全局变量 --------**/
	pub_base_sysinit();

	strcpy( filename , "RPT065");
	strcpy(vpfm, "RPT065");
	/* 条件:交易代码为"G088" */
	strcpy( tmpstr,"tx_code='G088'and prdt_no[1]='3' and " );
	strcat( wherelist,tmpstr );	
	/**组成查询**/

	strcat(wherelist," no_show!='1' and 1=1  ORDER BY tx_br_no, prdt_no, trace_no, trace_cnt") ;

	vtcp_log( "WHERE [%s]",wherelist );
	
	ret=Trace_log_bk_Dec_Sel( g_pub_tx.reply, wherelist );
	if(ret)
	{
		vtcp_log("[%s][%d]操作Trace_log_bk失败!\n",__FILE__,__LINE__);
		goto ErrExit;
		ttlnum = 0;
	}
	while(1)
	{
		ret=Trace_log_bk_Fet_Sel( &s_trace_log_bk, g_pub_tx.reply );
		if( ret==100 ) 
		{
			if( ttlnum )
			{
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
			bal_amt=0.00;
			in_amt=0.00;
			out_amt=0.00;
			cmpd_amt=0.00;
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
				/**不是第一个机构 到了另外一个机构关闭文件***/
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
			
		}
		
		
		/* 统计信息 */
		ret= sql_sum_double( "ln_mst_hst" , "tx_amt" , &bal_amt , "ac_id=%ld and tx_date=%ld and tx_code='G088' and ln_tx_type='0' and add_ind='0' " , 
					s_trace_log_bk.ac_id, s_trace_log_bk.tx_date );
		ERR_DEAL 
		ret= sql_sum_double( "ln_mst_hst" , "tx_amt" , &in_amt , "ac_id=%ld and  tx_date=%ld and tx_code='G088' and ln_tx_type='1' and add_ind='0' " , 
					s_trace_log_bk.ac_id, s_trace_log_bk.tx_date );
		ERR_DEAL
					
		ret= sql_sum_double( "ln_mst_hst" , "tx_amt" , &out_amt , "ac_id=%ld and  tx_date=%ld and tx_code='G088' and ln_tx_type='2' and add_ind='0' " , 
					s_trace_log_bk.ac_id,  s_trace_log_bk.tx_date );
		ERR_DEAL
		 
		ret= sql_sum_double( "ln_mst_hst" , "tx_amt" , &cmpd_amt , "ac_id=%ld and  tx_date=%ld and tx_code='G088' and ln_tx_type='3' and add_ind='0' " , 
					s_trace_log_bk.ac_id, s_trace_log_bk.tx_date );
		ERR_DEAL
		
		
		vtcp_log("[%s][%d]bal_amt==[%.2f],in_amt==[%.2f],out_amt==[%.2f],cmpd_amt=[%.2f]\n",__FILE__,__LINE__,
				bal_amt,in_amt,out_amt,cmpd_amt);
				
		/* 换页 */		
    if( page != 1 && line_file==1 )
		      {
		      	fprintf ( fp , "\f" );
		      }		
    /* 打印信息 */
		ret = pub_rpt_read_pfm_qd( fp,&line,vpfm,"0001","HH",&opncnt,get_ratio_mbfesd);
		ERR_DEAL
		line_file++;
		
		vtcp_log("[%s][%d]到这里说明已经打印出来了头部了 ，开始打印会计分录!\n",__FILE__,__LINE__);
		
		ret = Dc_log_bk_Dec_Sel(g_pub_tx.reply,"tx_date=%ld and trace_no=%ld  order by amt,dc_ind ",
					s_trace_log_bk.tx_date,s_trace_log_bk.trace_no);
		if(ret)
		{
			vtcp_log("[%s][%d]操作Dc_log_bk失败!\n",__FILE__,__LINE__);
			goto ErrExit;
		}
		while(1)
		{
			memset(acc_hrt_desp,0x00,sizeof(acc_hrt_desp));
			ret=Dc_log_bk_Fet_Sel( &s_dc_log_bk, g_pub_tx.reply );
			if( ret==100 ) 
			{
				ret = pub_rpt_read_pfm_qd( fp,&line,vpfm,"0001","TT",&opncnt,get_ratio_mbfesd);
				ERR_DEAL
				break;	
			}
			else if( ret )
			{
				sprintf(acErrMsg,"读取dc_log_bk表出错了!");
				WRITEMSG			
				goto ErrExit;			
			}
			
			if(s_dc_log_bk.dc_ind[0]=='1' )
			{
				vtcp_log("[%s][%d]获取到了借方科目201===[%s]\n",__FILE__,__LINE__,s_dc_log_bk.acc_hrt);
				memset(amt_tmp,0x00,sizeof(amt_tmp));
				sprintf( acc_hrt_desp,"科目（借）:",12);
				strcat(  acc_hrt_desp,s_dc_log_bk.acc_hrt);
				strcat(  acc_hrt_desp,"    ");
				sprintf( amt_tmp, "%.2lf" , s_dc_log_bk.amt ); 
				pub_rpt_flttomon( amt_tmp,amt_tmp );
				strcat(  acc_hrt_desp,amt_tmp);
				vtcp_log("[%s][%d]201借科目=[%s]\n",__FILE__,__LINE__,acc_hrt_desp);
			}
			else if(s_dc_log_bk.dc_ind[0]=='2' )
			{
				vtcp_log("[%s][%d]获取到了借方科目201===[%s]\n",__FILE__,__LINE__,s_dc_log_bk.acc_hrt);
				memset(amt_tmp,0x00,sizeof(amt_tmp));
				sprintf( acc_hrt_desp,"科目（贷）:",12);
				strcat(  acc_hrt_desp,s_dc_log_bk.acc_hrt);
				strcat(  acc_hrt_desp,"    ");
				sprintf( amt_tmp, "%.2lf" , s_dc_log_bk.amt ); 
				pub_rpt_flttomon( amt_tmp,amt_tmp );
				strcat(  acc_hrt_desp,amt_tmp);
				vtcp_log("[%s][%d]201借科目=[%s]\n",__FILE__,__LINE__,acc_hrt_desp);
			}
			
			ret = pub_rpt_read_pfm_qd( fp,&line,vpfm,"0001","KK",&opncnt,get_ratio_mbfesd);
			ERR_DEAL
		}
		if( line_file>TABLE_RECORD )	/* 大于换页 */
		{
			/**line_file=0;del by martin 070520**/
			line_file=1;
			page++;
			/*** fprintf(fp,"\f"); ***/
			/***  fprintf(fp,"\n"); del by martin 070517 ***/
			/***  fprintf(fp,"\n"); del by martin 070517 ***/
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

int get_ratio_mbfesd( bli,bll,str )
int bli,bll;
char str[100];
{
	char vstr[101]; 
	char fmt[10]; 
	char cDate[9];
	char cTx_Desc[17];
	char cTel[7];
	char amt_tmp[22];
	
	memset(vstr,	0x00,	sizeof(vstr));
	memset(fmt,	0x00,	sizeof(fmt));
	memset(cDate,	0x00,	sizeof(cDate));
	memset(cTel,	0x00,	sizeof(cTel));
	memset(cTx_Desc,0x00,	sizeof(cTx_Desc));
	memset(amt_tmp,0x00,sizeof(amt_tmp));

	struct com_branch_c sComBranch;
	memset(&sComBranch,0x00,sizeof(sComBranch));
	memset(str,	0x00,	sizeof(str));
	
	
	switch( bli ) 
	{
		case 'A': 
			vtcp_log("[%s][%d]开始打印--户名!\n",__FILE__,__LINE__);
			sprintf( fmt, "%%-%ds", bll );
			sprintf(str,fmt,g_ln_mst.name);
			break;
		case 'B': 
			vtcp_log("[%s][%d]开始打印--帐号!\n",__FILE__,__LINE__);
			sprintf( fmt, "%%-%ds", bll );
			sprintf(str,fmt,g_mdm_ac_rel.ac_no);
			break;
		case 'D':
			vtcp_log("[%s][%d]开始打印--机构号码!\n",__FILE__,__LINE__);
			memset( &sComBranch, 0x00, sizeof( struct com_branch_c ) );
			ret = Com_branch_Sel( g_pub_tx.reply, &sComBranch, "br_no='%s'", s_trace_log_bk.tx_br_no );
			if( ret )
			{
				sprintf(acErrMsg, "SELECT com_branch error![%d]", ret );
				WRITEMSG
			}
			pub_base_strpack( sComBranch.br_name );
			sprintf(vstr, "%s", sComBranch.br_name);
			pub_base_strpack( vstr );
			sprintf( fmt, "%%-%ds", bll );
			sprintf( str, fmt, vstr );
			break;
		case 'E': 
			vtcp_log("[%s][%d]开始打印--年!\n",__FILE__,__LINE__);
			sprintf(fmt, "%%%dld", bll);
			sprintf( str, fmt, sComSysParm.lst_date/10000 ); 
			break; 
		case 'F': 
			vtcp_log("[%s][%d]开始打印--月!\n",__FILE__,__LINE__);
			sprintf(fmt, "%%%dld", bll);
			sprintf( str, fmt, sComSysParm.lst_date/100%100 ); 
			break; 
		case 'Y': 
			vtcp_log("[%s][%d]开始打印--日!\n",__FILE__,__LINE__);
			sprintf(fmt, "%%%dld", bll);
			sprintf( str, fmt, sComSysParm.lst_date%100 ); 
			break; 
		case 'G': /* 偿还本金 */						
			/* 加逗号 */
			sprintf( amt_tmp, "%.2lf" , bal_amt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'H': /* 偿还表内 */
			/* 加逗号 */
			sprintf( amt_tmp, "%.2lf" , in_amt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'K': /* 偿还表外 */
			/* 加逗号 */
			sprintf( amt_tmp, "%.2lf" , out_amt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'L': /* 偿还复利 */
			/* 加逗号 */
			sprintf( amt_tmp, "%.2lf" , cmpd_amt ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'M': /* 总计 */
			/* 加逗号 */
			vtcp_log("[%s][%d]开始打印--金额!\n",__FILE__,__LINE__);
			vtcp_log("[%s][%d]开始打印--交易金额!\n",__FILE__,__LINE__);
			sprintf( fmt, "%%-%ds", bll );
			sprintf( amt_tmp, "%.2lf" , bal_amt+in_amt+out_amt+cmpd_amt ); 
			numtomoney( bal_amt+in_amt+out_amt+cmpd_amt, vstr);
			pub_rpt_flttomon( amt_tmp,amt_tmp ); 
			vtcp_log("[%s][%d]fmt=[%s]\n",__FILE__,__LINE__,fmt);
			
			vtcp_log("[%s][%d]fmt=[%s]\n",__FILE__,__LINE__,fmt);
			strcat(vstr," 小写:");
			strcat(vstr,amt_tmp);
			sprintf(str,fmt,vstr);
			vtcp_log("金额1=[%lf],vstr==[%s]str==[%s]\n",g_ln_mst.bal,vstr,str);
			/*strcpy( str, vstr ); */
			break; 
		case 'X':
			vtcp_log("[%s][%d]开始打印--转帐原因1!\n",__FILE__,__LINE__);
			memset(vstr,0x00,sizeof(vstr));
			switch(g_ln_mst.ac_sts[0])
			{
				case '1':
					strcpy(vstr,"贷款到期自动还款");
					break;
				case '2':
					strcpy(vstr,"   正常转逾期   ");
					break;
				case '3':
					strcpy(vstr,"   逾期转呆滞   ");
					break;
				case '4':
					strcpy(vstr,"   呆滞转呆帐   ");
					break;
				case '5':
					strcpy(vstr,"  逾期转90天外  ");
				default:
					strcpy(vstr,"        ");					
			}
			memcpy(str,vstr,bll);
			 break;
		case 'T':		/**分录130**/
			sprintf(fmt,"%%-%ds",bll);
			sprintf(str,fmt,acc_hrt_desp);
			
			break;
			break;
		
		default : 
			memset( str,' ',bll ); 
			
	}
	vtcp_log("[%s][%d]值[%c]==[%s]\n",__FILE__,__LINE__,bli,str);
	return 0; 
}


