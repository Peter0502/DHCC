/*********************************************************************
* 版 本 号 :  
* 程序作者 :  wuhaifan
* 日    期 :  2015/11/23
* 所属模块 :  手机理财产品客户交易记录查询
* 程序名称 :  spMK08.c
* 程序作用 :
* 函数列表 :
* 使用注意 :
交易配置：
insert into tx_def (TX_CODE, TX_NAME, BIT_MAP, LOG_IND, TX_TYPE, WRK_STS)
values ('5053', '手机理财交易记录查询', '10000000000000000000110011111111100000111111111111001011100001000111111100000000000110000000100000011000000000000000000000000000', '0', '4', '0');
insert into tx_flow_def (TX_CODE, FLOW_CODE, SUB_TX_CODE, SUB_TX_RSLT)
values ('5053', 0, 'MK08', '#$');
insert into tx_sub_def (SUB_TX_CODE, SUB_TX_NAME, TX_FUNC, ACCT_IND, CHNL_IND)
values ('MK08', '手机理财交易记录查询', 'spMK08', '0', '0');

***********************************************************************/
#define EXTERN
#include "public.h"
#include "fn_parm_c.h"
#include "fn_reg_c.h"
#include "mdm_ac_rel_c.h"
#include "com_branch_c.h"
#include "string.h"

int spMK08()
{
	struct fn_parm_c sFn_parm;	/*理财产品参数表*/
	struct fn_reg_c sFn_reg;		/*理财产品登记簿*/
	struct mdm_ac_rel_c sMdm_ac_rel;	/*介质账户关系表*/
	struct com_branch_c sCom_branch;	/*公共机构信息*/
	int count=0;
	int ret = 0;
	int sum=0;
	int start=0;
	int num=1;
	double dBuying_amt=0.00;
	double amt=0.00;
	double amttmp=0.00;
    
	char sCerttype[4];	/*证件类型*/
	char sCertno[20];	/*证件号*/
	char flag[4];		/*查询条件*/
	char sTurnpagebeginpos[10];	/*查询起始记录数*/
	char sTurnpageshownum[10];		/*请求记录数*/
  
	char 		sbeginDate[8+1];	/*开始日期*/
	char		sendDate[8+1];		/*结束日期*/
	
	char	tx_date[10];
	char  sw_traceno[12];
	
	FILE 	*fp  = NULL;
	char cFilename[256];
	char	pcfilename[100];
	char 	cWherelist[256];
	
	memset(&sFn_parm,0x00,sizeof(sFn_parm));
	memset(&sFn_reg,0x00,sizeof(sFn_reg));
	memset(&sMdm_ac_rel,0x00,sizeof(sMdm_ac_rel));
	memset(&sCom_branch,0x00,sizeof(sCom_branch));
	
	memset(sbeginDate,0x00,sizeof(sbeginDate));
	memset(sendDate,0x00,sizeof(sendDate));
	memset(sCerttype,0x00,sizeof(sCerttype));
	memset(sCertno,0x00,sizeof(sCertno));
	memset(flag,0x00,sizeof(flag));
	memset(sTurnpagebeginpos,0x00,sizeof(sTurnpagebeginpos));
	memset(sTurnpageshownum,0x00,sizeof(sTurnpageshownum));
	
	memset(tx_date,0x00,sizeof(tx_date));
	memset(sw_traceno,0x00,sizeof(sw_traceno));
	memset(cFilename,0x00,sizeof(cFilename));
	memset(pcfilename,0x00,sizeof(pcfilename));
	memset(cWherelist,0x00,sizeof(cWherelist));

	if (pub_base_sysinit())
	{
		vtcp_log("初始化公用结构错误!!");
		strcpy(g_pub_tx.reply, "CU06");
		goto ErrExit;
	}
    
  get_zd_data("0670",sCerttype);	/*证件类型*/
	get_zd_data("0620",sCertno);	/*证件号码*/
	get_zd_data("0220",flag);	/*查询条件*/
	get_zd_data("0500",sTurnpagebeginpos);	/*查询起始记录号*/
	get_zd_data("0510",sTurnpageshownum);	/*请求记录数*/
	
	get_zd_data("0460",sbeginDate);	/*开始日期*/
	get_zd_data("0470",sendDate);	/*结束日期*/
	
	get_zd_data("0050",tx_date);	/*平台日期*/
	get_zd_data("0520",sw_traceno);	/*平台流水*/

	start=atoi(sTurnpagebeginpos);
	sum=atoi(sTurnpageshownum);
	if(sum == 0)
	{
		sum = 10;
	}
	
	sprintf( cFilename,"5053mobbank%s%s",tx_date,sw_traceno );
	sprintf( pcfilename,"%s/%s",getenv("FILDIR"),cFilename );
	fp=fopen(pcfilename,"w");
	if(fp == NULL)
	{
	    vtcp_log("打开文件失败[%s]",pcfilename);
	    strcpy(g_pub_tx.reply,"S066");
	    set_zd_data("0130","打开文件失败");	
	    goto ErrExit;
	}
	fprintf(fp,"~账号|交易日期|交易机构|协议编号|产品代码|产品名称|期限|预期年化收益率|风险等级|认购金额|利率浮动比例|起息日期|到期日期|流水号|状态|\n");

	ret = Mdm_ac_rel_Dec_Sel(g_pub_tx.reply,"id_type='%s' and id_no='%s' order by ac_no desc ",sCerttype,sCertno);
	if(ret)
	{
	 	 	strcpy(g_pub_tx.reply,"P084");
	  	vtcp_log("定义查询介质账号关系表的游标错误[%d]",ret);
	  	set_zd_data("0130","查询失败");	
	  	goto ErrExit;
	}
	while(1)
	{
		memset(&sMdm_ac_rel,0x00,sizeof(sMdm_ac_rel));
		ret = Mdm_ac_rel_Fet_Sel(&sMdm_ac_rel,g_pub_tx.reply);
		if(ret != 0 && ret != 100)
		{
		  	strcpy(g_pub_tx.reply,"P088");
		  	vtcp_log("定义查询介质账号关系表的游标错误[%d]",ret);
		  	set_zd_data("0130","查询错误");	
		  	Mdm_ac_rel_Clo_Sel();
		  	goto ErrExit;
		}
		else if(ret == 100 || ret == 1403)
		{
			break;
		}
		else
		{
			zip_space(flag);
			if(memcmp(flag,"0",1) == 0)
			{
				/*持有中状态*/
				sprintf(cWherelist,"ac_id =%ld and sts in('0','1','2') order by tx_date desc,trace_no desc",sMdm_ac_rel.ac_id);
			}
			else if(memcmp(flag,"1",1) == 0)
			{
				/*撤销状态*/
				sprintf(cWherelist,"ac_id =%ld and sts='*' order by tx_date desc,trace_no desc",sMdm_ac_rel.ac_id);
			}
			else if(memcmp(flag,"2",1) == 0)
			{
				/*赎回状态*/
				sprintf(cWherelist,"ac_id =%ld and sts='#' order by tx_date desc,trace_no desc",sMdm_ac_rel.ac_id);
			}	
			else
			{
				sprintf(cWherelist,"ac_id =%ld order by tx_date desc,trace_no desc",sMdm_ac_rel.ac_id);
			}
			
			vtcp_log("%s %d::cWherelist[%s]",__FILE__,__LINE__,cWherelist);
		  	
		  /*产品总金额*/
		  dBuying_amt=0.00;
			ret = sql_sum_double("fn_reg","tx_amt",&dBuying_amt,cWherelist);
 			if(ret)
			{
					vtcp_log("统计fn_reg错误!");
					set_zd_data("0130","理财总金额错");
					strcpy(g_pub_tx.reply, "D331");
					goto ErrExit;
			}
			
			amt=amt+dBuying_amt;
			vtcp_log("%s %d::amt[%.2f]dBuying_amt[%.2f]",__FILE__,__LINE__,amt,dBuying_amt);
		  	
		  ret = Fn_reg_Dec_Sel(g_pub_tx.reply,"%s",cWherelist);
		  if(ret)
		  {
		    vtcp_log("%s %d::定义理财登记簿游标错误",__FILE__,__LINE__);
		    strcpy(g_pub_tx.reply,"D101");
		    set_zd_data("0130","查询理财登记簿错");
		    Mdm_ac_rel_Clo_Sel();
		    goto ErrExit;
		  }
			while(1)
			{
				memset(&sFn_parm,0x00,sizeof(sFn_parm));
				memset(&sFn_reg,0x00,sizeof(sFn_reg));
				
				ret = Fn_reg_Fet_Sel(&sFn_reg,g_pub_tx.reply);
				if(ret != 100 && ret != 0)
				{
				 		vtcp_log("取理财登记簿游标错误");
						strcpy(g_pub_tx.reply,"D330");
						set_zd_data("0130","查询理财登记簿错");
						Mdm_ac_rel_Clo_Sel();
						Fn_reg_Clo_Sel();
				  	goto ErrExit;
				}
				else if(ret == 100 || ret == 1403)
				{
					break;
				}
				else
				{
					ret = Fn_parm_Sel(g_pub_tx.reply,&sFn_parm,"prdt_code='%s'",sFn_reg.prdt_code);
					if(ret)
					{
						vtcp_log("查询理财产品参数错误");
						strcpy(g_pub_tx.reply,"D324");
						set_zd_data("0130","查询理财产品参数错误");
						Mdm_ac_rel_Clo_Sel();
						Fn_reg_Clo_Sel();
						goto ErrExit;
					}
					if(num >= start && (num-start) < sum)
					{
						/*begin增加将机构号换成机构名 20151201 by 关建锋*/
						ret = Com_branch_Sel(g_pub_tx.reply,&sCom_branch,"br_no='%s'",sFn_reg.tx_br_no);
						if(ret)
						{
							vtcp_log("查询公共机构参数错误");
							strcpy(g_pub_tx.reply,"D324");
							set_zd_data("0130","查询公共机构参数错误");
							Mdm_ac_rel_Clo_Sel();
							Fn_reg_Clo_Sel();
							goto ErrExit;
						}
						/*end增加将机构号换成机构名 20151201 by 关建锋*/
						/*~账号|交易日期|交易机构|协议编号|产品代码|产品名称|期限|预期年化收益率|风险等级|认购金额|利率浮动比例|起息日期|到期日期|流水号|状态|*/
				  	fprintf(fp,"|%s|%ld|%s|%s|%s|%s|%ld|%9.6f|%s|%16.2f|%9.6f|%ld|%ld|%ld|%s|\n",\
				  		sFn_reg.ac_no,sFn_reg.tx_date,sCom_branch.br_name,sFn_reg.pact_no,sFn_reg.prdt_code,sFn_parm.title,\
				  		sFn_parm.term,sFn_parm.rate,sFn_parm.risk_lvl,sFn_reg.tx_amt,sFn_reg.flt_radio,\
				  		sFn_reg.ic_date,sFn_reg.end_date,sFn_reg.trace_no,sFn_reg.sts);
					}
				  else
				  {
				  	break;
				  }
				  
				  ++num;
				}
			}
			Fn_reg_Clo_Sel();	    		
		}
		
		if((num-start) >= sum)
		{
			break;
		}		
	}
	Mdm_ac_rel_Clo_Sel();

	fclose(fp);
	fp = NULL;
	
	/*理财总金额*/
	amttmp=amt*100;
	vtcp_log("%s,%d amt[%16.2f]amttmp=[%16.2f]\n",__FILE__,__LINE__,amt,amttmp);
	set_zd_double("0560",amttmp);
	
	int iRt = -1;
	char cFtpfile[1024];
	memset(cFtpfile,0x00,sizeof(cFtpfile));
	set_zd_data("0260",cFilename);
	sprintf(cFtpfile,"%s/bin/ftp_to_nsw.sh %s %s %s",getenv("WORKDIR"),get_env_info("NSW_FTP_USER"),\
	get_env_info("NSW_FTP_PWD"),cFilename);
	vtcp_log("%s,%d cFtpfile=[%s]\n",__FILE__,__LINE__,cFtpfile);
	iRt=system(cFtpfile);
	if (iRt)
	{
		vtcp_log("[%s][%d] ftp文件[%s]到平台出错[%d]", __FILE__, __LINE__,cFilename,iRt);
		strcpy(g_pub_tx.reply,"9999");
		goto ErrExit;
	}
	vtcp_log("[%s][%d] ftp文件[%s]到平台成功", __FILE__, __LINE__,cFilename); 

OkExit:
		if(fp != NULL)
		{
			fclose(fp);
			fp=NULL;	
		}
    strcpy(g_pub_tx.reply,"0000");
    set_zd_data("0120","0000");
    return 0;
ErrExit:
		if(fp != NULL)
		{
			fclose(fp);
			fp=NULL;	
		}
		set_zd_data("0120",g_pub_tx.reply);
    vtcp_log("查询理财产品客户交易记录信息失败[%s]", g_pub_tx.reply);
    return 1;  
}

