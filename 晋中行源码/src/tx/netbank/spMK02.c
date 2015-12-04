/*********************************************************************
* 版 本 号 :  
* 程序作者 :  关建锋
* 日    期 :  2014/01/06
* 所属模块 :  手机银行客户所有账户查询
* 程序名称 :  spMK02.c
* 程序作用 :
* 函数列表 :
* 使用注意 :
交易配置：
insert into tx_def values ('5022', '客户所有账户查询', '11000000000000000000110011010011100000111111111110000000000001000111111100000000000110010000100000011000000000000000000000000000', '0', '4', '0',);
insert into tx_flow_def values ('5022', 0, 'MK02', '#$');
insert into tx_sub_def values ('MK02', '客户所有账户查询', 'spMK02', '0', '0');
***********************************************************************/
#define EXTERN
#include "public.h"
#include "dd_mst_c.h"
#include "cif_id_code_rel_c.h"
#include "mo_prdt_prot_c.h"
#include "com_branch_c.h"
#include "mdm_ac_rel_c.h"
#include "mo_prot_ac_rel_c.h"
#include "com_branch_c.h"
#include "mdm_attr_c.h"
#include "prdt_ac_id_c.h"
#include "cif_basic_inf_c.h"
#include "string.h"

int spMK02()
{
	int ret=-1;
	char sCustName[60+1];											/*客户姓名*/
	char sCertType[1+1];											/*证件类型*/
	char sCertNo[20+1];												/*证件号*/
	char tx_date[8+1];												/*交易日期*/
	char sw_traceno[10+1];										/*交易流水号*/
	char cFilename[128];											/*所要创建的文件名*/
	char pcfilename[256];											/*文件全路径名*/
	char cWherelist[256];
	struct dd_mst_c 					dd_mst;					/*活期存款主文件*/
	struct dd_mst_c 					g_dd_mst;				/*七天存款账户存款信息*/
	struct mdm_ac_rel_c 			mdm_ac_rel;			/*介质与账户关系*/
	struct com_branch_c 			com_branch;			/*公共机构码表*/
	struct mo_prdt_prot_c 		mo_prdt_prot;		/*划款协议登记簿*/
	struct cif_id_code_rel_c 	cif_id_code_rel;/*客户证件与客户号对照表*/
	struct mo_prot_ac_rel_c 	mo_prot_ac_rel;	/*划款协议账户对照表*/
	struct mdm_attr_c 				mdm_attr;				/*介质属性*/
	struct prdt_ac_id_c 			prdt_ac_id;			/*产品账号对照表*/
	struct cif_basic_inf_c 		cif_basic_inf;  /*客户基本信息表*/
	int count=0;															/*有效活期账户的数量*/
	FILE *fp=NULL;
	char protocolType[4];											/*7天通知存款协议状态*/
	char accountType[4];											/*账户类型*/
	char accountSts[4];												/*账户状态*/
	char channel_no[4];		/*渠道号*/
	
	memset(protocolType,0x00,sizeof(protocolType));
	memset(accountType,0x00,sizeof(accountType));
	memset(accountSts,0x00,sizeof(accountSts));
	memset(sCustName,0x00,sizeof(sCustName));
	memset(sCertType,0x00,sizeof(sCertType));
	memset(sCertNo,0x00,sizeof(sCertNo));
	memset(tx_date,0x00,sizeof(tx_date));
  memset(sw_traceno,0x00,sizeof(sw_traceno));
  memset(cFilename,0x00,sizeof(cFilename));
  memset(pcfilename,0x00,sizeof(pcfilename));
  memset(cWherelist,0x00,sizeof(cWherelist));
	memset(&dd_mst,0x00,sizeof(dd_mst));
	memset(&g_dd_mst,0x00,sizeof(g_dd_mst));
	memset(&mdm_ac_rel,0x00,sizeof(mdm_ac_rel));
	memset(&com_branch,0x00,sizeof(com_branch));
	memset(&mo_prot_ac_rel,0x00,sizeof(mo_prot_ac_rel));
	memset(&mo_prdt_prot,0x00,sizeof(mo_prdt_prot));
	memset(&cif_id_code_rel, 0x00, sizeof(cif_id_code_rel));
	memset(&mdm_attr, 0x00, sizeof(mdm_attr));
	memset(&cif_basic_inf, 0x00, sizeof(cif_basic_inf));
	memset(&prdt_ac_id, 0x00, sizeof(prdt_ac_id));
	 memset(channel_no,0x00,sizeof(channel_no));
	
	/**初始化公用结构**/
	if (pub_base_sysinit())
	{
	    vtcp_log("初始化公用结构错误!!");
	    strcpy(g_pub_tx.reply, "CU06");
	    goto ErrExit;
	}
	
	get_zd_data("0250",sCustName);
	get_zd_data("0680",sCertType);
	get_zd_data("0630",sCertNo);
	get_zd_data("0050",tx_date);
	get_zd_data("0520",sw_traceno);
	get_zd_data("0240",channel_no);
	
	sprintf( cFilename,"1010mobbank%s%s",tx_date,sw_traceno );
  sprintf( pcfilename,"%s/%s",getenv("FILDIR"),cFilename );
  fp=fopen(pcfilename,"w");
  if(fp == NULL)
  {
      vtcp_log("打开文件失败[%s]",pcfilename);
      set_zd_data("0130","文件操作失败");
      goto ErrExit;
  }
  /*客户证件与客户号对照表*/
  ret = Cif_id_code_rel_Sel(g_pub_tx.reply,&cif_id_code_rel,"id_type='%s' and id_no='%s'",sCertType,sCertNo);
	if(ret)
	{
		vtcp_log("%s %d::cif_id_code_rel [%d] error ",__FILE__,__LINE__,ret);
		strcpy(g_pub_tx.reply,"B115");
		set_zd_data("0130","客户证件与客户号不符");
		goto ErrExit;	
	}
	
	vtcp_log("%s %d::交易渠道channel_no [%s] error ",__FILE__,__LINE__,channel_no);
	if((memcmp(channel_no,"03",2) != 0) && (memcmp(channel_no,"04",2) != 0))
	{
  		/*客户基本信息表*/ 
  		ret = Cif_basic_inf_Sel(g_pub_tx.reply,&cif_basic_inf,"cif_no =%ld and name='%s'",cif_id_code_rel.cif_no,sCustName);
		if(ret)
		{
			vtcp_log("%s %d::cif_basic_inf_sel [%d] error ",__FILE__,__LINE__,ret);
			strcpy(g_pub_tx.reply,"C113");
			set_zd_data("0130","客户姓名与客户号不符");
			goto ErrExit;	
		}
	}
	/*账户数量*/
  sprintf(cWherelist,"cif_no =%ld and ac_sts='1'",cif_id_code_rel.cif_no);
	count = sql_count("dd_mst",cWherelist);
	set_zd_int("0450",count);
	vtcp_log("%s %d::产品总数count=%d",__FILE__,__LINE__,count);  
	
  fprintf(fp,"~账号|账号名称|账户类型|账户状态|介质状态|开户网点|开户日期|开户网点名称|余额|可用余额|冻结金额|通知存款金额|7天通知存款协议状态|\n");
  
  sprintf(cWherelist,"cif_no =%ld and ac_sts='1'",cif_id_code_rel.cif_no);
  ret = Dd_mst_Dec_Sel(g_pub_tx.reply,"%s",cWherelist);
	if(ret)
	{
		vtcp_log("定义活期存款主文件游标错误");
		strcpy(g_pub_tx.reply,"D101");
		set_zd_data("0130","定义活期存款主文件游标错误");
		goto ErrExit;
	}
	
	while(1)
	{
		ret = Dd_mst_Fet_Sel(&dd_mst,g_pub_tx.reply);
		if(ret != 100 && ret != 0)
		{
		  vtcp_log("取活期存款主文件游标错误");
		  strcpy(g_pub_tx.reply,"D330");
		  set_zd_data("0130","取活期存款主文件游标错误");
		  Dd_mst_Clo_Sel();
		  goto ErrExit;
		}
		else if(ret == 100 || ret == 1403)
		{
			break;
		}
		else
		{
			ret = Com_branch_Sel(g_pub_tx.reply,&com_branch,"br_no='%s'",dd_mst.opn_br_no);
			if(ret)
			{
				vtcp_log("%s %d::Com_branch_sel [%d] error ",__FILE__,__LINE__,ret);
				strcpy(g_pub_tx.reply,"S029");
				Dd_mst_Clo_Sel();
				set_zd_data("0130","查询机构信息失败");
				goto ErrExit;	
			}
			ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&mdm_ac_rel,"ac_id=%ld",dd_mst.ac_id);
			if(ret)
			{
				vtcp_log("%s %d::Mdm_ac_rel_sel [%d] error ",__FILE__,__LINE__,ret);
				strcpy(g_pub_tx.reply,"W011");
				Dd_mst_Clo_Sel();
				set_zd_data("0130","查询介质与账户关系表失败");
				goto ErrExit;	
			}

			ret = Mo_prdt_prot_Sel(g_pub_tx.reply, &mo_prdt_prot,"main_ac_id=%d and prot_type='TK7' and sts <> '*'",mdm_ac_rel.ac_id);
      if(ret == 100 || ret == 1403)
      {
  			protocolType[0] = '4';/*账户状态是未签约.*/
      }
			else	/*有7天通知存款相关信息*/
			{
		    if(mo_prdt_prot.sts[0] == '1' || mo_prdt_prot.sts[0] == '2')
		    {
		    	protocolType[0] = mo_prdt_prot.sts[0];	/*签约，临时解约*/
		    	/*取通知存款金额*/
		    	ret = Mo_prot_ac_rel_Sel(g_pub_tx.reply,&mo_prot_ac_rel,"main_ac_id=%ld",mdm_ac_rel.ac_id);
					if(ret)
					{
						vtcp_log("%s %d::Mo_prot_ac_rel_Sel [%d] error ",__FILE__,__LINE__,ret);
						strcpy(g_pub_tx.reply,"X013");
						Dd_mst_Clo_Sel();
						set_zd_data("0130","查询划款协议账户对照表失败");
						goto ErrExit;	
					}
					
		    	ret = Dd_mst_Sel1(g_pub_tx.reply,&g_dd_mst,"ac_id=%ld",mo_prot_ac_rel.app_ac_id);
					if(ret)
					{
						vtcp_log("%s %d::Dd_mst_Sel [%d] error ",__FILE__,__LINE__,ret);
						strcpy(g_pub_tx.reply,"W011");
						Dd_mst_Clo_Sel();
						set_zd_data("0130","查询活期存款主文件表失败");
						goto ErrExit;	
					}
		    }
		    else
		    {
					protocolType[0] = '4';	/*未签约*/
		    }
			}
			/*账户状态   X:其他不正常,*：销户;*/
			if(dd_mst.ac_sts[0] == '1')
			{
				accountSts[0] = dd_mst.hold_sts[0];/*0正常1完全冻结2只进不出3部分冻结*/
			}
			else if(dd_mst.ac_sts[0] == '*')
			{
				accountSts[0] = '*';
			}
			else
			{
				accountSts[0] = 'X';
			}
			
			/**********************************/
			/*判断账户类型*/
			ret = Mdm_attr_Sel(g_pub_tx.reply, &mdm_attr,"mdm_code='%s'",mdm_ac_rel.mdm_code);
	    if(ret)
		  {
		    sprintf(acErrMsg, "查询介质属性表出错[%d]", ret);
		    strcpy(g_pub_tx.reply, "N056");
		    set_zd_data("0130","查询介质属性表出错");
		    goto ErrExit;
		  }
			
			ret = Prdt_ac_id_Sel(g_pub_tx.reply, &prdt_ac_id,"ac_id=%ld and cif_no=%ld",dd_mst.ac_id,dd_mst.cif_no);
	    if(ret)
		  {
		    sprintf(acErrMsg, "查询产品账号对照表出错[%d]", ret);
		    strcpy(g_pub_tx.reply, "W013");
		    set_zd_data("0130","查询产品账号对照表出错");
		    goto ErrExit;
		  }

			if(prdt_ac_id.ac_id_type[0] == '1')
			{
				if(mdm_attr.mdm_type[0] == '1')
				{
					accountType[0]='1';
				}
				else if(mdm_attr.mdm_type[0] == '3' || mdm_attr.mdm_type[0] == '4' || mdm_attr.mdm_type[0] == '5' )
				{
					accountType[0]='4';
				}
				else
				{
					accountType[0]='X';
				}
			}
			else if(prdt_ac_id.ac_id_type[0] == '2')
			{
				accountType[0]='2';
			}
			else if(prdt_ac_id.ac_id_type[0] == '3')
			{
				accountType[0]='3';
			}
			else
			{
				accountType[0]='X';
			}
	
			fprintf(fp,"%24s|%30s|%s|%s|%s|%8s|%8ld|%30s|%16.2f|%16.2f|%16.2f|%16.2f|%s|\n",mdm_ac_rel.ac_no,dd_mst.name,accountType,accountSts,mdm_ac_rel.note_sts,dd_mst.opn_br_no,dd_mst.opn_date,com_branch.br_name,dd_mst.bal,(dd_mst.bal-dd_mst.hold_amt-dd_mst.ctl_amt),dd_mst.hold_amt,g_dd_mst.bal,protocolType);
		}
	}
	Dd_mst_Clo_Sel();

	fclose(fp);
  fp = NULL;
  int iRt = -1;
  char cFtpfile[1024];
  memset(cFtpfile,0x00,sizeof(cFtpfile));
	set_zd_data("0260",cFilename);
	sprintf(cFtpfile,"%s/bin/ftp_to_nsw.sh %s %s %s",getenv("WORKDIR"),get_env_info("NSW_FTP_USER"),get_env_info("NSW_FTP_PWD"),cFilename);
	vtcp_log("%s,%d cFtpfile=[%s]\n",__FILE__,__LINE__,cFtpfile);
	iRt=system(cFtpfile);
	if (iRt)
	{
		vtcp_log("[%s][%d] ftp文件[%s]到平台出错[%d]", __FILE__, __LINE__,cFilename,iRt);
		strcpy(g_pub_tx.reply,"9999");
		set_zd_data("0130","文件FTP到平台出错");
		goto ErrExit;
	}
	vtcp_log("[%s][%d] ftp文件[%s]到平台成功", __FILE__, __LINE__,cFilename);

OkExit:
	if(fp != NULL)
	{
		fclose(fp);
		fp=NULL;	
	}
	set_zd_data("0120","0000");
	set_zd_data("0130","客户所有账户查询成功");
  	strcpy(g_pub_tx.reply,"0000");
  	vtcp_log("客户所有账户查询成功[%s]", g_pub_tx.reply);
 	return 0;
ErrExit:
	if(fp != NULL)
	{
		fclose(fp);
		fp=NULL;	
	}
	Dd_mst_Clo_Sel();
  vtcp_log("客户所有账户查询失败[%s]", g_pub_tx.reply);
  	set_zd_data("0120",g_pub_tx.reply);
  return 0;
}