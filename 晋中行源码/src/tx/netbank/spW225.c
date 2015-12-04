/*********************************************************************
* 版 本 号 :  
* 程序作者 :  刘增盈
* 日    期 :  2014/12/09
* 所属模块 : 	网银查询来账信息
* 程序名称 :  spW225.c
* 程序作用 :
* 函数列表 :
* 使用注意 :
交易配置：
insert into tx_def values ('5065', '网银来账信息查询', '10000000000000000000110010010011100000111111111110000000000001000111111100000000000110000000100000011000000000000000000000000000', '0','4', '2');
insert into tx_flow_def values ('5065', 0, 'W225', '#$');
insert into tx_sub_def values ('W225', '网银查询来账信息', 'spW225', '0', '0');
***********************************************************************/
#define EXTERN
#include <string.h>
#include <stdio.h>
#include "public.h"
#include "mdm_ac_rel_c.h"
#include "dd_mst_c.h"
#include "dd_mst_hst_c.h"
#include "com_branch_c.h"
#include "chnl_tx_detl_c.h"			      /*二代支付交易日志表*/
#include "trace_log_c.h"
#include "trace_log_bk_c.h"
#include "trace_log_rz_c.h"

int spW225()
{
	struct chnl_tx_detl_c chnl_tx_detl;
	struct mdm_ac_rel_c  mdm_ac_rel;
	struct mdm_ac_rel_c  mdm_ac_rel1;
	struct dd_mst_hst_c dd_mst_hst;
	struct dd_mst_hst_c dd_mst_hst1;
	struct trace_log_c trace_log;
	struct trace_log_bk_c trace_log_bk;
	struct trace_log_rz_c trace_log_rz;
	struct dd_mst_c dd_mst;	
	struct dd_mst_c dd_mst1;	
	struct com_branch_c com_branch;
	struct com_branch_c com_branch1;
	
	char vostroType[2]={0};		      /*来账种类 1：行外 2:行内 3：渠道*/
	char cdtrAccount[32]={0};			  /*收款账号*/
	char beginDate[9]={0};			    /*开始日期*/
	char endDate[9]={0};			      /*截止日期*/
	char turnPageBeginPos[10]={0};	/*查询起始记录号*/
	char turnPageShowNum[10]={0};	  /*请求记录数*/	
	char tx_date[8+1]={0};		      /*平台交易日期*/
	char sw_traceno[10+1]={0};		  /*平台流水*/
	char filename[60]={0};
	char pcfilename[256]={0};
	char cFtpfile[256]={0};
	char cWherelist[1024]={0};
	char brftmp[61]={0};
	FILE *fp = NULL;
	int ret=0;
	int cnt=1,proSum=0;
	int count=0;			/*记录总数*/
	long l_date=0;
	
	memset(&chnl_tx_detl,0x00,sizeof(chnl_tx_detl));
	memset(&mdm_ac_rel,0x00,sizeof(mdm_ac_rel));
	memset(&dd_mst_hst,0x00,sizeof(dd_mst_hst));
	memset(&dd_mst,0x00,sizeof(dd_mst));
	memset(&com_branch,0x00,sizeof(com_branch));
	memset(&mdm_ac_rel1,0x00,sizeof(mdm_ac_rel1));
	memset(&dd_mst_hst1,0x00,sizeof(dd_mst_hst1));
	memset(&dd_mst1,0x00,sizeof(dd_mst1));
	memset(&com_branch1,0x00,sizeof(com_branch1));
	memset(&trace_log,0x00,sizeof(trace_log));
	memset(&trace_log_bk,0x00,sizeof(trace_log_bk));
	memset(&trace_log_rz,0x00,sizeof(trace_log_rz));
	
	/**初始化公用结构**/
	if (pub_base_sysinit())
	{
		vtcp_log("初始化公用结构错误!!");
		strcpy(g_pub_tx.reply, "CU06");
		goto ErrExit;
	}
	
	get_zd_data("0660",vostroType);    /*来账类型*/
	get_zd_data("0460",beginDate);     /*开始日期*/
	get_zd_data("0470",endDate);       /*截止日期*/	
	get_zd_data("0300",cdtrAccount);	 /*收款账号*/
	get_zd_data("0500",turnPageBeginPos);	 /*查询起始记录号*/
	get_zd_data("0510",turnPageShowNum);	 /*请求记录数*/
	get_zd_data("0050",tx_date);     /*平台日期*/
	get_zd_data("0520",sw_traceno);  /*平台流水*/
	
	vtcp_log("[%s][%d]::vostroType[%s],beginDate[%s],endDate[%s],cdtrAccount[%s],turnPageBeginPos[%s],turnPageShowNum[%s],tx_date[%s],\
						sw_traceno[%s]",__FILE__,__LINE__,vostroType,beginDate,endDate,cdtrAccount,turnPageBeginPos,turnPageShowNum,tx_date,sw_traceno);

	if ( !strlen(vostroType)||!strlen(beginDate)||!strlen(endDate)||!strlen(turnPageBeginPos)||!strlen(turnPageShowNum)||!strlen(tx_date)||!strlen(sw_traceno) )
	{
		strcpy(g_pub_tx.reply,"9999");
		set_zd_data("0130","关键要素不能为空值");		
		goto ErrExit;
	}

	sprintf( filename,"5065netbank%s%s",tx_date,sw_traceno );
	sprintf( pcfilename,"%s/%s",getenv("FILDIR"),filename );
	fp=fopen(pcfilename,"w");
	if(fp == NULL)
	{
		vtcp_log("打开文件失败[%s]",pcfilename);
		strcpy(g_pub_tx.reply,"S047");
		set_zd_data("0130","打开文件失败!");
		goto ErrExit;
	}
	
	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&mdm_ac_rel,"ac_no='%s'",cdtrAccount);
	if(ret)
	{
		vtcp_log("[%s][%d]::Mdm_ac_rel_Sel error ret [%d]",__FILE__,__LINE__,ret);
		strcpy(g_pub_tx.reply,"W011");
		set_zd_data("0130","查找介质账户关系表报错");	
		goto ErrExit;	
	}
	
	ret=Com_branch_Sel(g_pub_tx.reply,&com_branch,"br_no='%s' ",mdm_ac_rel.opn_br_no);
	if(ret)
	{
		vtcp_log("[%s][%d]::Com_branch_Sel error ret [%d]",__FILE__,__LINE__,ret);
		strcpy(g_pub_tx.reply,"W011");
		set_zd_data("0130","查找公共机构代码表报错");	
		goto ErrExit;	
	}
	
	ret=Dd_mst_Sel(g_pub_tx.reply,&dd_mst,"ac_id=%ld",mdm_ac_rel.ac_id);
	if(ret)
	{
		vtcp_log("[%s][%d]::Dd_mst_Sel error ret [%d]",__FILE__,__LINE__,ret);
		strcpy(g_pub_tx.reply,"W011");
		set_zd_data("0130","查找活期存款主文件表报错");	
		goto ErrExit;	
	}

	if ( vostroType[0] == '1')  /*行外来账*/
	{
		sprintf(cWherelist,"ac_id=%ld and tx_date>='%s' and tx_date <='%s' and add_ind='1' and ct_ind='2' and ( brf in \ 
   ( '跨行转入','山西同城', '普贷来帐','大额来帐' ) or brf like '%%%%柜面通%%%%') order by hst_cnt desc ",mdm_ac_rel.ac_id,beginDate,endDate);

		count=sql_count("dd_mst_hst",cWherelist);
		set_zd_int("0450",count);
		vtcp_log("[%s][%d]::行外来账总数为[%d]",__FILE__,__LINE__,count);
		if ( count == 0 )
		{
			vtcp_log("[%s][%d]::行外来账总数为0,直接返回.",__FILE__,__LINE__);
			fclose(fp);
			goto OkExit;	
		}

		ret = Dd_mst_hst_Dec_Sel(g_pub_tx.reply,"%s",cWherelist);
		if(ret)
		{
			vtcp_log("[%s][%d]::Dd_mst_hst_Dec_Sel ret [%d]",__FILE__,__LINE__,ret);
			strcpy(g_pub_tx.reply,"D101");
			set_zd_data("0130","查找活期交易明细表报错");	
			goto ErrExit;	
		}
		while(1)
		{
			ret = Dd_mst_hst_Fet_Sel(&dd_mst_hst,g_pub_tx.reply);
			if(ret != 100 && ret != 0)
			{
				vtcp_log("取活期交易明细表游标错误");
				strcpy(g_pub_tx.reply,"D330");
				set_zd_data("0130","查找活期交易明细表报错");	
				goto ErrExit;
			}
			else if(ret == 100 || ret == 1403)
			{
				break;
			}
			if(cnt < atoi(turnPageBeginPos))
			{
				cnt++;
				continue;	
			}
			if ( memcmp( dd_mst_hst.brf,"大额来帐",8) == 0 )
			{
				ret=Chnl_tx_detl_Sel(g_pub_tx.reply,&chnl_tx_detl,"tx_date=%ld and trace_no=%ld and sndrcvflag='SR01' \
				and ct_ind='2' and tx_code='6236' and ac_no='%s' ",dd_mst_hst.tx_date,dd_mst_hst.trace_no,cdtrAccount);
        
				if(ret != 100 && ret != 0)
				{
					vtcp_log("[%s][%d]::Chnl_tx_detl_Sel error ret [%d]",__FILE__,__LINE__,ret);
					strcpy(g_pub_tx.reply,"W011");
					set_zd_data("0130","查找二代支付交易明细表报错");	
					goto ErrExit;	
				}
				else if(ret == 100 || ret == 1403)
				{
					/*来账日期|来账时间|收款账户|收款名|收款方开户行|来账金额|付款账户|付款名|付款方开户行|摘要|*/
					fprintf(fp,"%ld|%06d|%s|%s|%s|%16.2f|%s|%s|%s|%s|\n",dd_mst_hst.tx_date,dd_mst_hst.tx_time,cdtrAccount,\
					dd_mst.name,com_branch.br_name,dd_mst_hst.tx_amt,"","","",dd_mst_hst.brf);
	 							
					vtcp_log("[%s][%d]开始打印》%ld|%06d|%s|%s|%s|%16.2f|%s|%s|%s|%s|\n",__FILE__,__LINE__,dd_mst_hst.tx_date,\
					dd_mst_hst.tx_time,cdtrAccount,dd_mst.name,com_branch.br_name,dd_mst_hst.tx_amt,"","","",dd_mst_hst.brf);
				}
				else
				{
					memset(brftmp,0x00,sizeof(brftmp));
					if ( strlen(chnl_tx_detl.filler1) > 0 )
					{
						memcpy(brftmp,chnl_tx_detl.filler1,sizeof(chnl_tx_detl.filler1));
					}
					else
					{
						memcpy(brftmp,dd_mst_hst.brf,sizeof(dd_mst_hst.brf));	
					}
						
					fprintf(fp,"%ld|%06d|%s|%s|%s|%16.2f|%s|%s|%s|%s|\n",dd_mst_hst.tx_date,dd_mst_hst.tx_time,cdtrAccount,dd_mst.name,\
					com_branch.br_name,dd_mst_hst.tx_amt,chnl_tx_detl.df_ac_no,chnl_tx_detl.df_name,chnl_tx_detl.or_br_no,brftmp);
		 							
					vtcp_log("[%s][%d]开始打印》%ld|%06d|%s|%s|%s|%16.2f|%s|%s|%s|%s|\n",__FILE__,__LINE__,dd_mst_hst.tx_date,dd_mst_hst.tx_time,\
					cdtrAccount,dd_mst.name,com_branch.br_name,dd_mst_hst.tx_amt,chnl_tx_detl.df_ac_no,chnl_tx_detl.df_name,chnl_tx_detl.or_br_no,brftmp);
				}
			}
			else if ( memcmp( dd_mst_hst.brf,"普贷来帐",8) == 0 )
			{
				ret=Chnl_tx_detl_Sel(g_pub_tx.reply,&chnl_tx_detl,"tx_date=%ld and trace_no=%ld and sndrcvflag='SR01' and ct_ind='2' \
				and tx_code='7022' and ac_no='%s' and tx_amt=%lf ",dd_mst_hst.tx_date,dd_mst_hst.trace_no,cdtrAccount,dd_mst_hst.tx_amt);
				
				if(ret != 100 && ret != 0)
				{
					vtcp_log("[%s][%d]::Chnl_tx_detl_Sel error ret [%d]",__FILE__,__LINE__,ret);
					strcpy(g_pub_tx.reply,"W011");
					set_zd_data("0130","查找二代支付交易明细表报错");	
					goto ErrExit;	
				}
				else if(ret == 100 || ret == 1403)
				{
					fprintf(fp,"%ld|%06d|%s|%s|%s|%16.2f|%s|%s|%s|%s|\n",dd_mst_hst.tx_date,dd_mst_hst.tx_time,cdtrAccount,\
					dd_mst.name,com_branch.br_name,dd_mst_hst.tx_amt,"","","",dd_mst_hst.brf);
	 							
					vtcp_log("[%s][%d]开始打印》%ld|%06d|%s|%s|%s|%16.2f|%s|%s|%s|%s|\n",__FILE__,__LINE__,dd_mst_hst.tx_date,\
					dd_mst_hst.tx_time,cdtrAccount,dd_mst.name,com_branch.br_name,dd_mst_hst.tx_amt,"","","",dd_mst_hst.brf);
				}
				else
				{
					memset(brftmp,0x00,sizeof(brftmp));
					if ( strlen(chnl_tx_detl.filler1) > 0 )
					{
						memcpy(brftmp,chnl_tx_detl.filler1,sizeof(chnl_tx_detl.filler1));
					}
					else
					{
						memcpy(brftmp,dd_mst_hst.brf,sizeof(dd_mst_hst.brf));	
					}
					
					fprintf(fp,"%ld|%06d|%s|%s|%s|%16.2f|%s|%s|%s|%s|\n",dd_mst_hst.tx_date,dd_mst_hst.tx_time,cdtrAccount,dd_mst.name,\
					com_branch.br_name,dd_mst_hst.tx_amt,chnl_tx_detl.df_ac_no,chnl_tx_detl.df_name,chnl_tx_detl.or_br_no,brftmp);
	 							
					vtcp_log("[%s][%d]开始打印》%ld|%06d|%s|%s|%s|%16.2f|%s|%s|%s|%s|\n",__FILE__,__LINE__,dd_mst_hst.tx_date,dd_mst_hst.tx_time,\
					cdtrAccount,dd_mst.name,com_branch.br_name,dd_mst_hst.tx_amt,chnl_tx_detl.df_ac_no,chnl_tx_detl.df_name,chnl_tx_detl.or_br_no,brftmp);
				}
			}
			else
			{
				fprintf(fp,"%ld|%06d|%s|%s|%s|%16.2f|%s|%s|%s|%s|\n",dd_mst_hst.tx_date,dd_mst_hst.tx_time,cdtrAccount,\
				dd_mst.name,com_branch.br_name,dd_mst_hst.tx_amt,"","","",dd_mst_hst.brf);
	 							
				vtcp_log("[%s][%d]开始打印》%ld|%06d|%s|%s|%s|%16.2f|%s|%s|%s|%s|\n",__FILE__,__LINE__,dd_mst_hst.tx_date,\
				dd_mst_hst.tx_time,cdtrAccount,dd_mst.name,com_branch.br_name,dd_mst_hst.tx_amt,"","","",dd_mst_hst.brf);
			}
			proSum++;
			if(proSum >= atoi(turnPageShowNum))
			{
				vtcp_log("[%s][%d]::记录数已到最大,[%d]",__FILE__,__LINE__,proSum);
				break;	
			}	
		}
	}
	else if ( vostroType[0] == '3')  /*渠道来账*/
	{
		sprintf(cWherelist,"ac_id=%ld and tx_date>='%s' and tx_date <='%s' and add_ind='1' and ct_ind='2' and  brf in \
		( '卡通单笔提现','财付通单笔提现', '财付通快捷退货','京东在线退货' )  order by hst_cnt desc ",mdm_ac_rel.ac_id,beginDate,endDate);
		
		count=sql_count("dd_mst_hst",cWherelist);
		set_zd_int("0450",count);
		vtcp_log("[%s][%d]::渠道来账总数为[%d]",__FILE__,__LINE__,count);
		if ( count == 0 )
		{
			vtcp_log("[%s][%d]::渠道来账总数为0,直接返回.",__FILE__,__LINE__);
			fclose(fp);	
			goto OkExit;	
		}
		ret = Dd_mst_hst_Dec_Sel(g_pub_tx.reply,"%s",cWherelist);
		if(ret)
		{
			vtcp_log("[%s][%d]::Dd_mst_hst_Dec_Sel ret [%d]",__FILE__,__LINE__,ret);
			strcpy(g_pub_tx.reply,"D101");
			set_zd_data("0130","查找活期交易明细表报错");	
			goto ErrExit;	
		}
		while(1)
		{
			ret = Dd_mst_hst_Fet_Sel(&dd_mst_hst,g_pub_tx.reply);
			if(ret != 100 && ret != 0)
			{
				vtcp_log("取活期交易明细表游标错误");
				strcpy(g_pub_tx.reply,"D330");
				set_zd_data("0130","查找活期交易明细表报错");	
				goto ErrExit;
			}
			else if(ret == 100 || ret == 1403)
			{
				break;
			}
			if( cnt < atoi(turnPageBeginPos) )
			{
				cnt++;
				continue;	
			}
			/*来账日期|来账时间|收款账户|收款名|收款方开户行|来账金额|付款账户|付款名|付款方开户行|摘要|*/
			fprintf(fp,"%ld|%06d|%s|%s|%s|%16.2f|%s|%s|%s|%s|\n",dd_mst_hst.tx_date,dd_mst_hst.tx_time,cdtrAccount,\
			dd_mst.name,com_branch.br_name,dd_mst_hst.tx_amt,"","","",dd_mst_hst.brf);
	 							
			vtcp_log("[%s][%d]开始打印》%ld|%06d|%s|%s|%s|%16.2f|%s|%s|%s|%s|\n",__FILE__,__LINE__,dd_mst_hst.tx_date,\
			dd_mst_hst.tx_time,cdtrAccount,dd_mst.name,com_branch.br_name,dd_mst_hst.tx_amt,"","","",dd_mst_hst.brf);

			proSum++;
			if(proSum >= atoi(turnPageShowNum))
			{
				vtcp_log("[%s][%d]::记录数已到最大,[%d]",__FILE__,__LINE__,proSum);
				break;	
			}	
		}
	}
	else if ( vostroType[0] == '2')  /*行内来账*/
	{
		sprintf( cWherelist,"ac_id=%ld and tx_date>='%s' and tx_date <='%s' and add_ind='1' and ct_ind='2' and ( brf not in \
		( '跨行转入','山西同城', '普贷来帐','大额来帐','卡通单笔提现','财付通单笔提现', '财付通快捷退货','京东在线退货' ) and \
		brf not like '%%%%柜面通%%%%' ) order by hst_cnt desc ",mdm_ac_rel.ac_id,beginDate,endDate);
		
		count=sql_count("dd_mst_hst",cWherelist);
		set_zd_int("0450",count);
		vtcp_log("[%s][%d]::行内来账总数为[%d]",__FILE__,__LINE__,count);
		if ( count == 0 )
		{
			vtcp_log("[%s][%d]::行内来账总数为0,直接返回.",__FILE__,__LINE__);
			fclose(fp);	
			goto OkExit;	
		}
		ret = Dd_mst_hst_Dec_Sel(g_pub_tx.reply,"%s",cWherelist);
		if(ret)
		{
			vtcp_log("[%s][%d]::Dd_mst_hst_Dec_Sel ret [%d]",__FILE__,__LINE__,ret);
			strcpy(g_pub_tx.reply,"D101");
			set_zd_data("0130","查找活期交易明细表报错");	
			goto ErrExit;	
		}
		while(1)
		{
			ret = Dd_mst_hst_Fet_Sel(&dd_mst_hst,g_pub_tx.reply);
			if(ret != 100 && ret != 0)
			{
				vtcp_log("取活期交易明细表游标错误");
				strcpy(g_pub_tx.reply,"D330");
				set_zd_data("0130","查找活期交易明细表报错");	
				goto ErrExit;
			}
			else if(ret == 100 || ret == 1403)
			{
				break;
			}
			if(cnt < atoi(turnPageBeginPos))
			{
				cnt++;
				continue;	
			}
			
			l_date=pub_base_daynumSUB(g_pub_tx.tx_date,1);
			
			if ( g_pub_tx.tx_date == dd_mst_hst.tx_date )
			{
				ret=Trace_log_Sel(g_pub_tx.reply,&trace_log,"tx_date=%ld and trace_no=%ld and add_ind='0' \
				and ct_ind='2' and amt=%lf ",dd_mst_hst.tx_date,dd_mst_hst.trace_no,dd_mst_hst.tx_amt);
			}
			else if ( l_date == dd_mst_hst.tx_date )
			{
				ret=Trace_log_bk_Sel(g_pub_tx.reply,&trace_log,"tx_date=%ld and trace_no=%ld and add_ind='0' \
				and ct_ind='2' and amt=%lf ",dd_mst_hst.tx_date,dd_mst_hst.trace_no,dd_mst_hst.tx_amt);
			}
			else
			{
				ret=Trace_log_rz_Sel(g_pub_tx.reply,&trace_log,"tx_date=%ld and trace_no=%ld and add_ind='0' \
				and ct_ind='2' and amt=%lf ",dd_mst_hst.tx_date,dd_mst_hst.trace_no,dd_mst_hst.tx_amt);
			}
			if(ret != 100 && ret != 0)
			{
				vtcp_log("[%s][%d]::Trace_log_rz_Sel error ret [%d]",__FILE__,__LINE__,ret);
				strcpy(g_pub_tx.reply,"W011");
				set_zd_data("0130","查找交易日志表报错");	
				goto ErrExit;	
			}
			else if(ret == 100 || ret == 1403)
			{
				/*来账日期|来账时间|收款账户|收款名|收款方开户行|来账金额|付款账户|付款名|付款方开户行|摘要|*/
				fprintf(fp,"%ld|%06d|%s|%s|%s|%16.2f|%s|%s|%s|%s|\n",dd_mst_hst.tx_date,dd_mst_hst.tx_time,cdtrAccount,\
				dd_mst.name,com_branch.br_name,dd_mst_hst.tx_amt,"","","",dd_mst_hst.brf);
	 							
				vtcp_log("[%s][%d]开始打印》%ld|%06d|%s|%s|%s|%16.2f|%s|%s|%s|%s|\n",__FILE__,__LINE__,dd_mst_hst.tx_date,\
				dd_mst_hst.tx_time,cdtrAccount,dd_mst.name,com_branch.br_name,dd_mst_hst.tx_amt,"","","",dd_mst_hst.brf);
			}
			else
			{
				if ( strlen(trace_log.ac_no) <=12 || trace_log.ac_no[0] == '9' )
				{
					fprintf(fp,"%ld|%06d|%s|%s|%s|%16.2f|%s|%s|%s|%s|\n",dd_mst_hst.tx_date,dd_mst_hst.tx_time,cdtrAccount,\
					dd_mst.name,com_branch.br_name,dd_mst_hst.tx_amt,"","","",dd_mst_hst.brf);
		 							
					vtcp_log("[%s][%d]开始打印》%ld|%06d|%s|%s|%s|%16.2f|%s|%s|%s|%s|\n",__FILE__,__LINE__,dd_mst_hst.tx_date,\
					dd_mst_hst.tx_time,cdtrAccount,dd_mst.name,com_branch.br_name,dd_mst_hst.tx_amt,"","","",dd_mst_hst.brf);
				}
				else
				{
					ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&mdm_ac_rel1,"ac_no='%s' ",trace_log.ac_no);
					if(ret)
					{
						vtcp_log("[%s][%d]::Mdm_ac_rel_Sel1 error ret [%d]",__FILE__,__LINE__,ret);
						strcpy(g_pub_tx.reply,"W011");
						set_zd_data("0130","查找介质账户关系表报错");	
						goto ErrExit;	
					}
										
					ret=Com_branch_Sel(g_pub_tx.reply,&com_branch1,"br_no='%s' ",mdm_ac_rel1.opn_br_no);
					if(ret)
					{
						vtcp_log("[%s][%d]::Com_branch_Sel1 error ret [%d]",__FILE__,__LINE__,ret);
						strcpy(g_pub_tx.reply,"W011");
						set_zd_data("0130","查找公共机构代码表报错");	
						goto ErrExit;	
					}
										
					ret=Dd_mst_Sel(g_pub_tx.reply,&dd_mst1,"ac_id=%ld",mdm_ac_rel1.ac_id);
					if(ret)
					{
						vtcp_log("[%s][%d]::Dd_mst_Sel1 error ret [%d]",__FILE__,__LINE__,ret);
						strcpy(g_pub_tx.reply,"W011");
						set_zd_data("0130","查找活期主文件表报错");	
						/*goto ErrExit;*/  /*账号可能不是活期,查不到也暂时先不管,dd_mst1.name会为空值*/
					}
										
					fprintf(fp,"%ld|%06d|%s|%s|%s|%16.2f|%s|%s|%s|%s|\n",dd_mst_hst.tx_date,dd_mst_hst.tx_time,cdtrAccount,dd_mst.name,\
					com_branch.br_name,dd_mst_hst.tx_amt,trace_log.ac_no,dd_mst1.name,com_branch1.br_no,dd_mst_hst.brf);
			 							
					vtcp_log("[%s][%d]开始打印》%ld|%06d|%s|%s|%s|%16.2f|%s|%s|%s|%s|\n",__FILE__,__LINE__,dd_mst_hst.tx_date,dd_mst_hst.tx_time,\
					cdtrAccount,dd_mst.name,com_branch.br_name,dd_mst_hst.tx_amt,trace_log.ac_no,dd_mst1.name,com_branch1.br_no,dd_mst_hst.brf);
				}
			}
			proSum++;
			if(proSum >= atoi(turnPageShowNum))
			{
				vtcp_log("[%s][%d]::记录数已到最大,[%d]",__FILE__,__LINE__,proSum);
				break;	
			}
		}
	}
	else if ( vostroType[0] == '0')  /*全部来账*/
	{
		sprintf(cWherelist,"ac_id=%ld and tx_date>='%s' and tx_date <='%s' and add_ind='1' and ct_ind='2' order by hst_cnt desc ",mdm_ac_rel.ac_id,beginDate,endDate);

		count=sql_count("dd_mst_hst",cWherelist);
		set_zd_int("0450",count);
		vtcp_log("[%s][%d]::行外来账总数为[%d]",__FILE__,__LINE__,count);
		if ( count == 0 )
		{
			vtcp_log("[%s][%d]::行外来账总数为0,直接返回.",__FILE__,__LINE__);
			fclose(fp);
			goto OkExit;	
		}

		ret = Dd_mst_hst_Dec_Sel(g_pub_tx.reply,"%s",cWherelist);
		if(ret)
		{
			vtcp_log("[%s][%d]::Dd_mst_hst_Dec_Sel ret [%d]",__FILE__,__LINE__,ret);
			strcpy(g_pub_tx.reply,"D101");
			set_zd_data("0130","查找活期交易明细表报错");	
			goto ErrExit;	
		}
		while(1)
		{
			ret = Dd_mst_hst_Fet_Sel(&dd_mst_hst,g_pub_tx.reply);
			if(ret != 100 && ret != 0)
			{
				vtcp_log("取活期交易明细表游标错误");
				strcpy(g_pub_tx.reply,"D330");
				set_zd_data("0130","查找活期交易明细表报错");	
				goto ErrExit;
			}
			else if(ret == 100 || ret == 1403)
			{
				break;
			}
			if(cnt < atoi(turnPageBeginPos))
			{
				cnt++;
				continue;	
			}
			if ( memcmp(dd_mst_hst.brf,"大额来帐",8) == 0 )
			{
				ret=Chnl_tx_detl_Sel(g_pub_tx.reply,&chnl_tx_detl,"tx_date=%ld and trace_no=%ld and sndrcvflag='SR01' \
				and ct_ind='2' and tx_code='6236' and ac_no='%s' ",dd_mst_hst.tx_date,dd_mst_hst.trace_no,cdtrAccount);
        
				if(ret != 100 && ret != 0)
				{
					vtcp_log("[%s][%d]::Chnl_tx_detl_Sel error ret [%d]",__FILE__,__LINE__,ret);
					strcpy(g_pub_tx.reply,"W011");
					set_zd_data("0130","查找二代支付交易明细表报错");	
					goto ErrExit;	
				}
				else if(ret == 100 || ret == 1403)
				{
					/*来账日期|来账时间|收款账户|收款名|收款方开户行|来账金额|付款账户|付款名|付款方开户行|摘要|*/
					fprintf(fp,"%ld|%06d|%s|%s|%s|%16.2f|%s|%s|%s|%s|\n",dd_mst_hst.tx_date,dd_mst_hst.tx_time,cdtrAccount,\
					dd_mst.name,com_branch.br_name,dd_mst_hst.tx_amt,"","","",dd_mst_hst.brf);
	 							
					vtcp_log("[%s][%d]开始打印》%ld|%06d|%s|%s|%s|%16.2f|%s|%s|%s|%s|\n",__FILE__,__LINE__,dd_mst_hst.tx_date,\
					dd_mst_hst.tx_time,cdtrAccount,dd_mst.name,com_branch.br_name,dd_mst_hst.tx_amt,"","","",dd_mst_hst.brf);
				}
				else
				{
					memset(brftmp,0x00,sizeof(brftmp));
					if ( strlen(chnl_tx_detl.filler1) > 0 )
					{
						memcpy(brftmp,chnl_tx_detl.filler1,sizeof(chnl_tx_detl.filler1));
					}
					else
					{
						memcpy(brftmp,dd_mst_hst.brf,sizeof(dd_mst_hst.brf));	
					}
					
					fprintf(fp,"%ld|%06d|%s|%s|%s|%16.2f|%s|%s|%s|%s|\n",dd_mst_hst.tx_date,dd_mst_hst.tx_time,cdtrAccount,dd_mst.name,\
					com_branch.br_name,dd_mst_hst.tx_amt,chnl_tx_detl.df_ac_no,chnl_tx_detl.df_name,chnl_tx_detl.or_br_no,brftmp);
		 							
					vtcp_log("[%s][%d]开始打印》%ld|%06d|%s|%s|%s|%16.2f|%s|%s|%s|%s|\n",__FILE__,__LINE__,dd_mst_hst.tx_date,dd_mst_hst.tx_time,\
					cdtrAccount,dd_mst.name,com_branch.br_name,dd_mst_hst.tx_amt,chnl_tx_detl.df_ac_no,chnl_tx_detl.df_name,chnl_tx_detl.or_br_no,brftmp);
				}
			}
			else if (memcmp(dd_mst_hst.brf,"普贷来帐",8) == 0 )
			{
				ret=Chnl_tx_detl_Sel(g_pub_tx.reply,&chnl_tx_detl,"tx_date=%ld and trace_no=%ld and sndrcvflag='SR01' and ct_ind='2' \
				and tx_code='7022' and ac_no='%s' and tx_amt=%lf ",dd_mst_hst.tx_date,dd_mst_hst.trace_no,cdtrAccount,dd_mst_hst.tx_amt);
				
				if(ret != 100 && ret != 0)
				{
					vtcp_log("[%s][%d]::Chnl_tx_detl_Sel error ret [%d]",__FILE__,__LINE__,ret);
					strcpy(g_pub_tx.reply,"W011");
					set_zd_data("0130","查找二代支付交易明细表报错");	
					goto ErrExit;	
				}
				else if(ret == 100 || ret == 1403)
				{
					fprintf(fp,"%ld|%06d|%s|%s|%s|%16.2f|%s|%s|%s|%s|\n",dd_mst_hst.tx_date,dd_mst_hst.tx_time,cdtrAccount,\
					dd_mst.name,com_branch.br_name,dd_mst_hst.tx_amt,"","","",dd_mst_hst.brf);
	 							
					vtcp_log("[%s][%d]开始打印》%ld|%06d|%s|%s|%s|%16.2f|%s|%s|%s|%s|\n",__FILE__,__LINE__,dd_mst_hst.tx_date,\
					dd_mst_hst.tx_time,cdtrAccount,dd_mst.name,com_branch.br_name,dd_mst_hst.tx_amt,"","","",dd_mst_hst.brf);
				}
				else
				{
					memset(brftmp,0x00,sizeof(brftmp));
					if ( strlen(chnl_tx_detl.filler1) > 0 )
					{
						memcpy(brftmp,chnl_tx_detl.filler1,sizeof(chnl_tx_detl.filler1));
					}
					else
					{
						memcpy(brftmp,dd_mst_hst.brf,sizeof(dd_mst_hst.brf));	
					}
					
					fprintf(fp,"%ld|%06d|%s|%s|%s|%16.2f|%s|%s|%s|%s|\n",dd_mst_hst.tx_date,dd_mst_hst.tx_time,cdtrAccount,dd_mst.name,\
					com_branch.br_name,dd_mst_hst.tx_amt,chnl_tx_detl.df_ac_no,chnl_tx_detl.df_name,chnl_tx_detl.or_br_no,brftmp);
	 							
					vtcp_log("[%s][%d]开始打印》%ld|%06d|%s|%s|%s|%16.2f|%s|%s|%s|%s|\n",__FILE__,__LINE__,dd_mst_hst.tx_date,dd_mst_hst.tx_time,\
					cdtrAccount,dd_mst.name,com_branch.br_name,dd_mst_hst.tx_amt,chnl_tx_detl.df_ac_no,chnl_tx_detl.df_name,chnl_tx_detl.or_br_no,brftmp);
				}
			}
			else if (memcmp(dd_mst_hst.brf,"跨行转入",8) == 0 || memcmp(dd_mst_hst.brf,"山西同城",8) == 0 || memcmp(dd_mst_hst.brf,"卡通单笔提现",12) == 0 \
				|| memcmp(dd_mst_hst.brf,"财付通单笔提现",14) == 0 || memcmp(dd_mst_hst.brf,"财付通快捷退货",14) == 0 || memcmp(dd_mst_hst.brf,"京东在线退货",12) == 0 \
				|| memcmp(dd_mst_hst.brf,"柜面通",6) == 0 || memcmp(dd_mst_hst.brf,"银联柜面通",10) == 0 )
			{
				fprintf(fp,"%ld|%06d|%s|%s|%s|%16.2f|%s|%s|%s|%s|\n",dd_mst_hst.tx_date,dd_mst_hst.tx_time,cdtrAccount,\
				dd_mst.name,com_branch.br_name,dd_mst_hst.tx_amt,"","","",dd_mst_hst.brf);
	 							
				vtcp_log("[%s][%d]开始打印》%ld|%06d|%s|%s|%s|%16.2f|%s|%s|%s|%s|\n",__FILE__,__LINE__,dd_mst_hst.tx_date,\
				dd_mst_hst.tx_time,cdtrAccount,dd_mst.name,com_branch.br_name,dd_mst_hst.tx_amt,"","","",dd_mst_hst.brf);
			}
			else
			{
				l_date=pub_base_daynumSUB(g_pub_tx.tx_date,1);
				
				if ( g_pub_tx.tx_date == dd_mst_hst.tx_date )
				{
					ret=Trace_log_Sel(g_pub_tx.reply,&trace_log,"tx_date=%ld and trace_no=%ld and add_ind='0' \
					and ct_ind='2' and amt=%lf ",dd_mst_hst.tx_date,dd_mst_hst.trace_no,dd_mst_hst.tx_amt);
				}
				else if ( l_date == dd_mst_hst.tx_date )
				{
					ret=Trace_log_bk_Sel(g_pub_tx.reply,&trace_log,"tx_date=%ld and trace_no=%ld and add_ind='0' \
					and ct_ind='2' and amt=%lf ",dd_mst_hst.tx_date,dd_mst_hst.trace_no,dd_mst_hst.tx_amt);
				}
				else
				{
					ret=Trace_log_rz_Sel(g_pub_tx.reply,&trace_log,"tx_date=%ld and trace_no=%ld and add_ind='0' \
					and ct_ind='2' and amt=%lf ",dd_mst_hst.tx_date,dd_mst_hst.trace_no,dd_mst_hst.tx_amt);
				}
				
				if(ret != 100 && ret != 0)
				{
					vtcp_log("[%s][%d]::Trace_log_rz_Sel error ret [%d]",__FILE__,__LINE__,ret);
					strcpy(g_pub_tx.reply,"W011");
					set_zd_data("0130","查找交易日志表报错");	
					goto ErrExit;	
				}
				else if(ret == 100 || ret == 1403)
				{
					/*来账日期|来账时间|收款账户|收款名|收款方开户行|来账金额|付款账户|付款名|付款方开户行|摘要|*/
					fprintf(fp,"%ld|%06d|%s|%s|%s|%16.2f|%s|%s|%s|%s|\n",dd_mst_hst.tx_date,dd_mst_hst.tx_time,cdtrAccount,\
					dd_mst.name,com_branch.br_name,dd_mst_hst.tx_amt,"","","",dd_mst_hst.brf);
		 							
					vtcp_log("[%s][%d]开始打印》%ld|%06d|%s|%s|%s|%16.2f|%s|%s|%s|%s|\n",__FILE__,__LINE__,dd_mst_hst.tx_date,\
					dd_mst_hst.tx_time,cdtrAccount,dd_mst.name,com_branch.br_name,dd_mst_hst.tx_amt,"","","",dd_mst_hst.brf);
				}
				else
				{
					if ( strlen(trace_log.ac_no) <=12 || trace_log.ac_no[0] == '9' )
					{
						fprintf(fp,"%ld|%06d|%s|%s|%s|%16.2f|%s|%s|%s|%s|\n",dd_mst_hst.tx_date,dd_mst_hst.tx_time,cdtrAccount,\
						dd_mst.name,com_branch.br_name,dd_mst_hst.tx_amt,"","","",dd_mst_hst.brf);
			 							
						vtcp_log("[%s][%d]开始打印》%ld|%06d|%s|%s|%s|%16.2f|%s|%s|%s|%s|\n",__FILE__,__LINE__,dd_mst_hst.tx_date,\
						dd_mst_hst.tx_time,cdtrAccount,dd_mst.name,com_branch.br_name,dd_mst_hst.tx_amt,"","","",dd_mst_hst.brf);
					}
					else
					{
						ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&mdm_ac_rel1,"ac_no='%s' ",trace_log.ac_no);
						if(ret)
						{
							vtcp_log("[%s][%d]::Mdm_ac_rel_Sel1 error ret [%d]",__FILE__,__LINE__,ret);
							strcpy(g_pub_tx.reply,"W011");
							set_zd_data("0130","查找介质账户关系表报错");	
							goto ErrExit;	
						}
											
						ret=Com_branch_Sel(g_pub_tx.reply,&com_branch1,"br_no='%s' ",mdm_ac_rel1.opn_br_no);
						if(ret)
						{
							vtcp_log("[%s][%d]::Com_branch_Sel1 error ret [%d]",__FILE__,__LINE__,ret);
							strcpy(g_pub_tx.reply,"W011");
							set_zd_data("0130","查找公共机构代码表报错");	
							goto ErrExit;	
						}
											
						ret=Dd_mst_Sel(g_pub_tx.reply,&dd_mst1,"ac_id=%ld",mdm_ac_rel1.ac_id);
						if(ret)
						{
							vtcp_log("[%s][%d]::Dd_mst_Sel1 error ret [%d]",__FILE__,__LINE__,ret);
							strcpy(g_pub_tx.reply,"W011");
							set_zd_data("0130","查找活期主文件表报错");	
							/*goto ErrExit;*/  /*账号可能不是活期,查不到也暂时先不管,dd_mst1.name会为空值*/
						}
											
						fprintf(fp,"%ld|%06d|%s|%s|%s|%16.2f|%s|%s|%s|%s|\n",dd_mst_hst.tx_date,dd_mst_hst.tx_time,cdtrAccount,dd_mst.name,\
						com_branch.br_name,dd_mst_hst.tx_amt,trace_log.ac_no,dd_mst1.name,com_branch1.br_no,dd_mst_hst.brf);
				 							
						vtcp_log("[%s][%d]开始打印》%ld|%06d|%s|%s|%s|%16.2f|%s|%s|%s|%s|\n",__FILE__,__LINE__,dd_mst_hst.tx_date,dd_mst_hst.tx_time,\
						cdtrAccount,dd_mst.name,com_branch.br_name,dd_mst_hst.tx_amt,trace_log.ac_no,dd_mst1.name,com_branch1.br_no,dd_mst_hst.brf);
					}
				}
			}
			proSum++;
			if(proSum >= atoi(turnPageShowNum))
			{
				vtcp_log("[%s][%d]::记录数已到最大,[%d]",__FILE__,__LINE__,proSum);
				break;	
			}	
		}
	}
	
	Dd_mst_hst_Clo_Sel();
	fclose(fp);	

	/*ftp传输文件*/
	vtcp_log("%s,%d filename=[%s]\n",__FILE__,__LINE__,filename);
	set_zd_data("0260",filename);
	
	sprintf(cFtpfile,"%s/bin/ftp_to_nsw.sh %s %s %s",getenv("WORKDIR"),get_env_info("NSW_FTP_USER"), get_env_info("NSW_FTP_PWD"),filename);
	vtcp_log("%s,%d cFtpfile=[%s]\n",__FILE__,__LINE__,cFtpfile);
	ret=system(cFtpfile);
	if (ret)
	{
		vtcp_log("[%s][%d] ftp文件[%s]到平台出错[%d]", __FILE__, __LINE__,filename,ret);
		strcpy(g_pub_tx.reply,"9999");
		set_zd_data("0130","ftp文件到平台出错");
		goto ErrExit;
	}
	
	vtcp_log("[%s][%d] ftp文件[%s]到平台成功", __FILE__, __LINE__,filename);


	OkExit:
		strcpy(g_pub_tx.reply,"0000");
		set_zd_data("0120","0000");
		set_zd_data("0130","网银查询来账信息成功!");
		vtcp_log("success !!: 5065 reply is[%s]", g_pub_tx.reply);
		return 0;		
	ErrExit:
		Dd_mst_hst_Clo_Sel();
		if(fp != NULL)
		fclose(fp);
		vtcp_log("fail !!: 5065 reply is[%s]", g_pub_tx.reply);
		return 1;
}