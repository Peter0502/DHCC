/*************************************************************
* 文 件 名: rpt063.c
* 功能描述：20号贷款结息贷款科目的结息传票
*
* 作    者: dadary
* 完成日期: 2006年11月21日
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
#include "com_sys_parm_c.h"
#include "dd_mst_c.h"
#include "dd_parm_c.h"
#include "ln_mst_c.h"
#include "ln_parm_c.h"
#include "mdm_ac_rel_c.h"
#include "dc_acc_rel_c.h"
#include "com_item_c.h"
#include "trace_log_c.h"
#include "dc_log_c.h"

#define TABLE_RECORD 2

struct ln_acm_reg_c 	s_ln_acm_reg;
struct ln_mst_c 	s_ln_mst;
struct ln_parm_c 	s_ln_parm;
struct com_sys_parm_c 	s_ComSysParm;
struct dd_mst_c 	s_dd_mst;
struct mdm_ac_rel_c 	s_mdm_ac_rel;
struct mdm_ac_rel_c 	w_mdm_ac_rel;
struct dd_parm_c 	s_dd_parm;
struct trace_log_c      s_trace_log;
struct dc_log_c		s_dc_log;


int page=0;
int line=0;
int line_file=0;
char vpfm[21];
char t_str[41];
int opncnt;
FILE    *fp; 
int ret = 0;
long days=0;
long qry_date=0;
double rate_money = 0.0;/*实际结息金额*/
double rtn_money = 0.0; /*偿还金额*/
double owe_money = 0.0; /*亏欠金额*/
double save_money = 0.0;/*活期帐户的存款*/

char acc_hrt_desp[60];	/***改变会计分录打印的方式***/
		
int get_ratio_lnjshd(int bli,int bll,char str[100]);

rpt063()
{
	int flag=0;
	int del_ind=0;
	int ttlnum;
	int BB_num=0;

	char fname[100];
	char filename[100];
	char wherelist[1024];  /**查询条件**/
	char tmpstr[300];

	char tmp_str[60];
	char old_br_no[6];
	char old_intst_type[2];
	long ac_id_old=0;
	char amt_tmp[21];
	
	
	int  dc_log_flag = 0;/**一个流水有多笔会计分录,但是这里我只在第一个分录的地方打印表头***/
	
	double old_trace_no = 0;
	
	
	memset( wherelist,0,sizeof(wherelist) );
	memset( &s_ln_acm_reg , 0x00 , sizeof(struct ln_acm_reg_c));
	
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

	memset( &s_ComSysParm, 0x00, sizeof( struct com_sys_parm_c ) );
	ret = Com_sys_parm_Sel( g_pub_tx.reply , &s_ComSysParm , "1=1");
	if( ret ) 
	{ 
		sprintf(acErrMsg, "SELECT com_sys_parm error![%d]", ret ); 
		WRITEMSG 
		goto ErrExit; 
	}
	
	/**------- 初始化全局变量 --------**/
	pub_base_sysinit();
	
	strcpy( filename , "RPT063");
	strcpy(vpfm, "RPT063");
	/* 本金积数结息清单 */
	/* 条件:流水号大于等于日终起始流水号 */
	memset( &tmpstr,0x0,strlen(tmpstr));	
	memset( &wherelist,0x0,strlen(wherelist));
	/*sprintf( tmpstr," cal_date=%ld and intst_type in('1','2','3','4') and  ",qry_date ); modify by martin 0719*/
	sprintf( tmpstr," cal_date=%ld and intst_type = '1' and  ",qry_date );
	strcat( wherelist,tmpstr );	
	
	/**组成查询**/
	strcat(wherelist," 1=1 ORDER BY br_no, ac_id, ac_seqn ,beg_date, intst_type");
	vtcp_log( "WHERE [%s]",wherelist );
	
	ret=Ln_acm_reg_Dec_Sel( g_pub_tx.reply, wherelist );
	ERR_DEAL

		ttlnum=0;	
	while(1)
	{
		memset(&s_ln_acm_reg,0x00,sizeof(s_ln_acm_reg));
		ret=Ln_acm_reg_Fet_Sel( &s_ln_acm_reg, g_pub_tx.reply );
		if( ret==100 ) 
		{
			if( ttlnum )
			{
				fclose(fp);
			}
			break;	
		}
		else if( ret )
		{
			sprintf(acErrMsg,"读取贷款结息登记簿异常出错!");
			WRITEMSG			
			goto ErrExit;			
		}	
		
		/*取登记表信息*/
		ret =  pub_get_all_file(s_ln_acm_reg.ac_id,s_ln_acm_reg.ac_seqn, &s_mdm_ac_rel,&s_ln_mst,&s_ln_parm,&s_dd_mst,&s_dd_parm );
		if(ret)
		{
			return ret;
		}
		/***  付息帳號查詢 0905***/
		/**** delete at 20110521
		不明白为什么从这儿再检查付息帐号信息，并且从Mdm_ac_rel表中查找，由于mdm_ac_rel表中卡的ac_seqn为9999，而对应的ac_id在活期主表中的
		ac_seqn为1，所以检查不过去 
		ret = Mdm_ac_rel_Sel( g_pub_tx.reply , &w_mdm_ac_rel, "ac_id=%ld and ac_seqn=%d ", s_dd_mst.ac_id ,s_dd_mst.ac_seqn);
		if( ret==100 )
		{
			sprintf( acErrMsg,"凭证(介质)与账户关系表无此记录[%d][%ld]",ret,s_dd_mst.ac_id);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"O001" );
			return 1;
		} 
		else if( ret )
		{
		sprintf( acErrMsg,"取凭证(介质)与账户关系表异常[%d][%ld]",ret,s_dd_mst.ac_id);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"D103" );
		return -1;
		}
		end 20110521 ****/
		
		/****如果流水变化了 就需要打印表尾巴了******/
	
		/* 机构变换时 */
		pub_base_strpack( s_ln_acm_reg.br_no );
		if( strcmp( s_ln_acm_reg.br_no , old_br_no ) )
		{
			/* 第一个机构 */
			if( strcmp( old_br_no , "*****" ) )
			{
				fclose(fp);
			}
			
			strcpy( old_br_no , s_ln_acm_reg.br_no );
			
			page=1;
			line_file=0;
			ttlnum=0;
			
			/* 删除原文件 */
			ret = pub_rpt_rmfile( s_ln_acm_reg.br_no , filename , 0 );
			ERR_DEAL
			/* 打开文件 */	
			ret = pub_rpt_openfile( &fp , s_ln_acm_reg.br_no , filename );
			ERR_DEAL
		} 
		
		
		/******************获取偿还金额*************/
		ret = Dc_log_Sel( g_pub_tx.reply , &s_dc_log , "trace_no=%ld and tx_date=%ld and acc_hrt like '2%%' ",s_ln_acm_reg.trace_no,s_ln_acm_reg.cal_date);
		if(ret==100)
		{
			vtcp_log("[%s][%d]改贷款户没有还利息!\n",__FILE__,__LINE__);
			rtn_money = 0.0;
		}
		else if (ret )
			ERR_DEAL
		if(ret==0)
		{
			
			rtn_money  = s_dc_log.amt;
			vtcp_log("[%s][%d]还利息拉!!!!!=[%lf]\n",__FILE__,__LINE__,rtn_money);
		}
		vtcp_log("[%s][%d]偿还利息===[%lf] ret=[%d] \n",__FILE__,__LINE__,rtn_money,ret);	
		owe_money   = 0.0 ;
		owe_money   = s_ln_mst.in_lo_intst + s_ln_mst.out_lo_intst + s_ln_mst.cmpd_lo_intst;/*从贷款主表中取三个和**/
		vtcp_log("[%s][%d]流水[%ld]开始打印表头haha!\n",__FILE__,__LINE__,s_ln_acm_reg.trace_no);
		if( page != 1 && line_file==0 )
		      {
		      	fprintf ( fp , "\f" );
		      }
		else if ( page == 1 && line_file==0 )
			{
				fprintf(fp,"%c%c",0x1b,0x40);
			} 
		ret = Make_yeb_sub("HH");
		if(s_ln_acm_reg.intst_type[0]!='1')
			ret = Make_yeb_sub("BB");
		else
			ret = Make_yeb_sub("AA");
		ret = Make_yeb_sub("CC");
		printf("[%s][%d]贷款户id[%ld],哈哈!\n",__FILE__,__LINE__,s_ln_acm_reg.ac_id);
		ERR_DEAL
		
		
		
		/******通过从ln_acm_reg里面获取的流水号来获取其对应的会计分录***/
		ret = Trace_log_Dec_Sel(g_pub_tx.reply,"trace_no=%ld and tx_date=%ld order by trace_cnt",s_ln_acm_reg.trace_no,s_ln_acm_reg.cal_date);
		if(ret && ret!=100)
		{
			vtcp_log("[%s][%d]查找流水错误 !ret===[%d]\n",__FILE__,__LINE__,ret);
			return ret;
		}
		while(1)
		{
			memset(&s_trace_log,0x00,sizeof(s_trace_log));
			/***注意了 如果再ln_acm_reg中存在流水,那么在trace_log中肯定也存在***/
			ret = Trace_log_Fet_Sel(&s_trace_log,g_pub_tx.reply);
			if(ret && ret!=100)
			{
				vtcp_log("[%s][%d]数据库操作异常 !ret==[%d]\n",__FILE__,__LINE__,ret);
				return ret;
			}
			else if(ret==100)
			{
				strcpy(acc_hrt_desp,"会计分录欠缺");
				ret = Make_yeb_sub("TT");
				ERR_DEAL
				break;
			}
			
			
			
			
			/***********必须进行初始化*****************/
			
			/**根据流水号以及流水分号来获取会计分录***/
			ret = Dc_log_Dec_Sel(g_pub_tx.reply,"trace_no=%ld and trace_cnt=%d and tx_date=%ld order by amt,dc_ind"
				,s_trace_log.trace_no,s_trace_log.trace_cnt,s_trace_log.tx_date);
			if(ret && ret!=100)
			{
				vtcp_log("[%s][%d]数据库异常 !ret=[%d]\n",__FILE__,__LINE__,ret);
				return ret;
			}
			rate_money = 0.0;/*实际结息金额*/
		
			save_money = 0.0;/*活期帐户的存款*/
			
			dc_log_flag = 0;
			while(1)
			{
				memset(&s_dc_log,0x00,sizeof(s_dc_log));
				ret = Dc_log_Fet_Sel(&s_dc_log,g_pub_tx.reply);
				if(ret && ret!=100)
				{
					vtcp_log("[%s][%d]数据库操作异常 !ret==[%d]\n",__FILE__,__LINE__,ret);
					return ret;
				}
				else if(ret==100)
				{
					break;
				}
				memset(acc_hrt_desp,0x00,sizeof(acc_hrt_desp));
				
				if(s_dc_log.dc_ind[0]=='1' )
				{
					vtcp_log("[%s][%d]获取到了借方科目201===[%s]\n",__FILE__,__LINE__,s_dc_log.acc_hrt);
					memset(amt_tmp,0x00,sizeof(amt_tmp));
					sprintf( acc_hrt_desp,"科目（借）:",12);
					strcat(  acc_hrt_desp,s_dc_log.acc_hrt);
					strcat(  acc_hrt_desp,"    ");
					sprintf( amt_tmp, "%.2lf" , s_dc_log.amt ); 
					pub_rpt_flttomon( amt_tmp,amt_tmp );
					strcat(  acc_hrt_desp,amt_tmp);
					vtcp_log("[%s][%d]201借科目=[%s]\n",__FILE__,__LINE__,acc_hrt_desp);
				}
				else if(s_dc_log.dc_ind[0]=='2' )
				{
					vtcp_log("[%s][%d]获取到了借方科目201===[%s]\n",__FILE__,__LINE__,s_dc_log.acc_hrt);
					memset(amt_tmp,0x00,sizeof(amt_tmp));
					sprintf( acc_hrt_desp,"科目（贷）:",12);
					strcat(  acc_hrt_desp,s_dc_log.acc_hrt);
					strcat(  acc_hrt_desp,"    ");
					sprintf( amt_tmp, "%.2lf" , s_dc_log.amt ); 
					pub_rpt_flttomon( amt_tmp,amt_tmp );
					strcat(  acc_hrt_desp,amt_tmp);
					vtcp_log("[%s][%d]201借科目=[%s]\n",__FILE__,__LINE__,acc_hrt_desp);
				}
				
				
				
				/* 打印信息 ,先把除分录外的部分打印出来,分录在下面一个循环中打印 */
				/****打印会计分录****/
				ret = Make_yeb_sub("JJ");
				ERR_DEAL
				
				dc_log_flag ++; /***分录个数+1*****/
				
				vtcp_log("[%s][%d]dc_ind==[%c],acc_hrt=[%s]\n",__FILE__,__LINE__,s_dc_log.dc_ind[0],s_dc_log.acc_hrt);
				
			}/**结束dc_log这个while***/
			
		}/**结束trace_log这个while**/
		
		/* 打印信息 */
		line_file++;
		if( line_file==TABLE_RECORD )	/* 大于换页 */
		{
			line_file=0;
			page++;
			/*** fprintf(fp,"\f"); **/
			/***  fprintf(fp,"\n"); del by martin 070517 ***/
		}
		ttlnum++;
	}		
	Ln_acm_reg_Clo_Sel();

	
OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"生成贷款结息传票成功!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--事务回滚--*/
	sprintf(acErrMsg,"生成贷款结息传票失败!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

/* 中间函数 */
int Make_yeb_sub( char vrec[3] )
{	
	ret = pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001",vrec,&opncnt,get_ratio_lnjshd);
	ERR_DEAL
	
GoodExit:
	return 0;
ErrExit:
	return 1;
}

/* 赋值函数 */
int get_ratio_lnjshd( bli,bll,str )
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
	char ln_intst_type[11];
	char acm_intst_type[21];
	int i=0; 
	int k=0;
	struct com_branch_c s_Com_branch;
	char acc_hrt_print[8];
	memset(acc_hrt_print,0x00,sizeof(acc_hrt_print));
	memcpy(acc_hrt_print,"0000000",7);
	
	memset(&s_Com_branch, 0x0, sizeof(struct com_branch_c));
	days= pub_base_CalTrueDays( s_ln_acm_reg.beg_date,s_ln_acm_reg.end_date);	
	switch( bli ) 
	{ 
		case 'C': /* 机构名称 */
			ret = Com_branch_Sel(g_pub_tx.reply, &s_Com_branch, "br_no='%s'",
				s_ln_acm_reg.br_no);
			if (ret != 0)
			{
				sprintf(acErrMsg, "查询公共机构编码表错误! [%d]", ret);
				WRITEMSG
				goto ErrExit;
			}
			pub_base_strpack( s_Com_branch.br_name ); 
			sprintf( t_str , "%s    ( %5s )" , s_Com_branch.br_name , s_ln_acm_reg.br_no); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'E':/*当前 年*/
			vtcp_log("[%s][%d]开始打印--年!\n",__FILE__,__LINE__);
			sprintf(fmt, "%%%dld", bll);
			sprintf( str, fmt, s_ComSysParm.lst_date/10000 ); 
			break; 
			break;
		case 'D':/*当前月*/
			vtcp_log("[%s][%d]开始打印--月!\n",__FILE__,__LINE__);
			sprintf(fmt, "%%%dld", bll);
			sprintf( str, fmt, s_ComSysParm.lst_date/100%100 ); 
			break; 
			break;		
		case 'F': /* 贷款账号 */
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt, s_mdm_ac_rel.ac_no);
			memset( &s_mdm_ac_rel, 0x00 , sizeof( struct mdm_ac_rel_c ));
			break;
		case 'G':/*当前天*/
			vtcp_log("[%s][%d]开始打印--日!\n",__FILE__,__LINE__);
			sprintf(fmt, "%%%dld", bll);
			sprintf( str, fmt, s_ComSysParm.lst_date%100 ); 
			break; 
			break;		
		case 'J': /* 利息类型 */
			pub_base_dic_show_str(acm_intst_type,"ln_acm_type",s_ln_acm_reg.intst_type);	/* 利息类型 */
			pub_base_strpack(acm_intst_type);
			sprintf(fmt,"%%-%ds",bll); 
			sprintf(str, fmt, acm_intst_type); 
			break;	
		case 'N': /* 积数 */						
			sprintf( amt_tmp, "%.2lf" , s_ln_acm_reg.intst_acm ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'P': /* 结息金额 */
			/* 加逗号 */
			sprintf( amt_tmp, "%.2lf" , s_ln_acm_reg.intst ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'H': /* 贷款户名 */
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt, s_ln_mst.name);
			break;
		case 'I': /* 产品 */
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str, fmt, s_ln_parm.title);
			break;
		case 'O': /* 利率 */	
			sprintf( amt_tmp, "%8.5lf" , s_ln_acm_reg.rate ); 
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			sprintf(acErrMsg,"!![%s]",amt_tmp);
			WRITEMSG	
			sprintf( str, "%8.5lf", s_ln_acm_reg.rate ); 
			break;	
		case 'K': /* 起始日期 */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, s_ln_acm_reg.beg_date); 
			break; 
		case 'L': /* 终止日期 */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, s_ln_acm_reg.end_date); 
			break; 	
		case 'M': /* 天数 */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, days); 
			break; 
		case 'R':/*偿还利息*/
			sprintf( amt_tmp, "%.2lf" , rtn_money ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			sprintf(acErrMsg,"!![%s]",amt_tmp);
			WRITEMSG
			break;
		case 'S':/*所欠利息*/
			sprintf( amt_tmp, "%.2lf" , owe_money ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			sprintf(acErrMsg,"!![%s]",amt_tmp);
			WRITEMSG
			break;
		case 'U':/*借 201类科目*/
			sprintf(fmt,"%%-%ds",bll);
			sprintf(str,fmt,acc_hrt_desp);
			break;
		case 'a': /*复利积数*/
			sprintf( amt_tmp, "%.2lf" , s_ln_acm_reg.intst_acm ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break; 
		case 'b': /*月利率*/
			sprintf( amt_tmp, "%8.5lf" , s_ln_acm_reg.rate ); 
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			sprintf( str, "%8.5lf", s_ln_acm_reg.rate ); 
			break; 
		case 'c': /**/
			sprintf( amt_tmp, "%.2lf" , s_ln_acm_reg.intst ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break; 
		case 'Q':/*付息帐号*/
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt, w_mdm_ac_rel.ac_no);
			memset( &w_mdm_ac_rel, 0x00 , sizeof( struct mdm_ac_rel_c ));
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
/*****************************************************
 *根据结息的记录获取活期表,定期表,帐号介质表中的内容 *
 *****************************************************/
 
int pub_get_all_file(int ac_id,int ac_seqn , struct mdm_ac_rel_c *mdm_ac_rel,struct ln_mst_c *ln_mst,struct ln_parm_c *ln_parm
			,struct dd_mst_c *dd_mst,struct dd_parm_c *dd_parm )
{
    int ret = 0 ;
    
    /* 根据显示账号，在凭证(介质)与账户关系mdm_ac_rel表中查询，取出账户ID */
	sprintf( acErrMsg,"!!!![%ld]",ac_id);
	WRITEMSG
	sprintf( acErrMsg,"!!!![%ld]",ac_id);
	WRITEMSG

	ret = Mdm_ac_rel_Sel( g_pub_tx.reply , mdm_ac_rel, "ac_id=%ld and ac_seqn=%d ", ac_id ,ac_seqn);
	if( ret==100 )
	{
		sprintf( acErrMsg,"凭证(介质)与账户关系表无此记录[%d][%ld]",ret,ac_id);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"O001" );
		return 1;
	} 
	else if( ret )
	{
    	sprintf( acErrMsg,"取凭证(介质)与账户关系表异常[%d][%ld]",ret,ac_id);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"D103" );
    	return -1;
        }
    	        

   	/* 取贷款主文件信息 */
	ret = Ln_mst_Sel(g_pub_tx.reply, ln_mst , "ac_id=%ld and ac_seqn=%d" ,
				        mdm_ac_rel->ac_id,ac_seqn );
	if( ret==100 )
	{
		sprintf( acErrMsg,"贷款主文件表中无此记录 erro code=[%d]",ret);
		WRITEMSG
		sprintf( acErrMsg,"mdm_ac_rel->ac_id=[%ld]ac_seqn=[%d]",mdm_ac_rel->ac_id,ac_seqn);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L002" );
		return 1;

	}
	else if( ret )
	{
   		sprintf( acErrMsg,"取贷款主文件异常 erro code=[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"D103" );
   		return -1;
   	}

   	/* 取贷款参数信息 */
	ret = Ln_parm_Sel(g_pub_tx.reply, ln_parm , " prdt_no='%s' " ,
				        ln_mst->prdt_no );
	if( ret==100 )
	{
		sprintf(acErrMsg,"贷款参数文件中无此记录 [%s][%d]",ln_mst->prdt_no,ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L001" );
		return 1;
	} 
	else if( ret )
	{
   		sprintf( acErrMsg,"取贷款参数文件异常 [%s][%d]",ln_mst->prdt_no,ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L001" );
   		return -1;
   	}
   	
   	/* 取贷款对应的活期主文件信息 */
	ret = Dd_mst_Sel(g_pub_tx.reply, dd_mst , "ac_id=%ld and ac_seqn=1" ,
				        ln_mst->repay_ac_id );
	if( ret==100 )
	{
		sprintf( acErrMsg,"活期主文件表中无此记录 erro code=[%d]",ret);
		WRITEMSG
		sprintf( acErrMsg,"mdm_ac_rel->ac_id=[%ld]ac_seqn=[%d]",ln_mst->repay_ac_id,ac_seqn);
		WRITEMSG

                return(0); /* modified by liuxuan */
         
		strcpy( g_pub_tx.reply ,"L002" );
		return 1;

	}
	else if( ret )
	{
   		sprintf( acErrMsg,"取活期主文件异常 erro code=[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"D103" );
   		return -1;
   	}

   	/* 取贷款参数信息 */
	ret = Dd_parm_Sel(g_pub_tx.reply, dd_parm , " prdt_no='%s' " ,
				        dd_mst->prdt_no );
	if( ret==100 )
	{
		sprintf(acErrMsg,"活期 参数文件中无此记录 [%s][%d]",dd_mst->prdt_no,ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L001" );
		return 1;
	} 
	else if( ret )
	{
   		sprintf( acErrMsg,"取活期参数文件异常 [%s][%d]",ln_mst->prdt_no,ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L001" );
   		return -1;
   	}
  return 0 ;
}
