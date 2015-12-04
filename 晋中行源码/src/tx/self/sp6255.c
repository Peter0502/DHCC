/*************************************************
* 版 本 号: V1.0
* 文 件 名: sp6255.c
* 功能描述: 活期账户信息查询(包含对开户机构，户名，客户号，客户类型，账户/介质状态，余额，明细等的查询)
* 所属模块: 新平台账务体系程序
* 程序作用: 活期账户信息查询
* 作    者: gong
* 完成日期: 2011年7月15日
* 修改记录:
* 日    期: 20110816
* 修 改 人: xueke
* 修改内容: pub_com_test.c()控制标志 本标志适用于新平台所有交易 73域接收 每个字节代表一种控制或检查
insert into tx_def  values ('6255', '活期帐户信息查询IBPS', '11000000000010010001000011100110000110111100000000000010000011000111111110010000111000001110101001000000000000000000000000000100', '1', '4', '0', '3', null);
insert into tx_flow_def  values ('6255', 0, '6255', '#=0000@1@#$');
insert into tx_flow_def  values ('6255', 1, 'F000', '#$');
insert into tx_sub_def  values ('6255', '活期账户信息查询IBPS', 'sp6255', '0', '0');
*************************************************/
#include <stdio.h>
#include "public.h"
#include "dd_mst_c.h"
#include "dd_mst_hst_c.h"
#include "card_reg_c.h"
#include "dd_parm_c.h"
#include "mdm_ac_rel_c.h"
#include "card.h"
#include "prdt_ac_id_c.h"
#include "in_mst_c.h"
#include "in_parm_c.h"
#include "cif_addr_inf_c.h"
#include "td_mst_c.h"
#include "td_parm_c.h"
#define EXTERN
int sp6255()
{
	int iFhcnt = 0;/*分户个数*/
	int iRet = 0,i=0;
	long End_line=100;
	int totnum=0;
	char cHx_tx_code[5]; /*交易代码*/
	char cAc_name[61];  /** 户名 **/
	char cAc_no[25];  /** 账号 **/
	char cMsr2[38];  /** 二磁道信息 **/
	char cMsr3[105];  /** 三磁道信息 **/
	char cPwd[17];  /** 客户密码 **/
	char cBrf[51];  /** 记账摘要 **/
	char fd95[483],brf6[128],to_str[128];
	char mx_tmp[45];
	char tmpstr[41];
	char cCert_no[21];  /**证件类型/号码**/
	char cCtrlcmd[11];  /**控制标志 **/
	char cGz_flag[2];
	char sts[51];
	char cFee_flag[2];/*是否收费*/
	char cMx_Fh_flag[2];/*返回明细或者分户账信息标志 0 明细 1 分户账*/
	double dFee=0.00;/**交易手续费**/
	double account_amt=0.0,limit_amt=0.0;/**账面金额，限制金额**/
	double ky_bal=0.0;
	long    lBeg_date=0;  /* 查询活期明细起始日期add by slzh 20130317*/
	long    lEnd_date=0;  /* 查询活期明细截止日期add by slzh 20130317*/
	char   cWherelist[1024];
       	FILE  * fp;
	char filename[256];
	char tmp_cmd[512];
	
	
	struct mdm_ac_rel_c sMdm_ac_rel;
	struct dd_mst_c sDd_mst;
	struct dd_mst_hst_c sDd_mst_hst;
	struct dd_parm_c sDd_parm;
	struct card_reg_c sCard_reg;
	struct prdt_ac_id_c sPrdt_ac_id;
	struct prdt_ac_id_c sPrdt_ac_id_fh;
	struct in_mst_c sIn_mst;
	struct in_parm_c sIn_parm;
	struct cif_addr_inf_c sCif_addr_inf;
	struct td_mst_c sTd_mst;
	struct td_parm_c sTd_parm;
	/** 初始化各变量 **/
	memset(cWherelist,0,sizeof(cWherelist));
	memset(cHx_tx_code,0,sizeof(cHx_tx_code));
	memset(cPwd, 0x00, sizeof(cPwd));
	memset(cBrf, 0x00, sizeof(cBrf));
	memset(cMsr2, 0x00, sizeof(cMsr2));
	memset(cMsr3, 0x00, sizeof(cMsr3));
	memset(cAc_no, 0x00, sizeof(cAc_no));
	memset(cGz_flag,0x00,sizeof(cGz_flag));
	memset(sts,0x00,sizeof(sts));
	memset(cAc_name, 0x00, sizeof(cAc_name));
	memset(cCert_no, 0x00, sizeof(cCert_no));
	memset(cCtrlcmd, 0x00, sizeof(cCtrlcmd));
	memset(tmpstr,0,sizeof(tmpstr));
	memset(fd95,'\0',sizeof(fd95));
	memset(mx_tmp,0,sizeof(mx_tmp));
	memset(brf6,0,sizeof(brf6));
	memset(to_str,0,sizeof(to_str));
	memset(cFee_flag,0x00,sizeof(cFee_flag));
	strcpy(g_pub_tx.cur_no,"01");
	memset(&sDd_mst,0x00,sizeof(sDd_mst));
	memset(&sDd_mst_hst,0,sizeof(sDd_mst_hst));
	memset(&sCard_reg,0,sizeof(sCard_reg));
	memset(&sDd_parm,0x00,sizeof(sDd_parm));
	memset(&sMdm_ac_rel,0x00,sizeof(sMdm_ac_rel));
	memset(&sPrdt_ac_id,0x00,sizeof(sPrdt_ac_id));
	memset(&sPrdt_ac_id_fh,0x00,sizeof(sPrdt_ac_id_fh));
	memset(&sIn_mst,0x00,sizeof(sIn_mst));
	memset(&sIn_parm,0x00,sizeof(sIn_parm));
	memset(&sCif_addr_inf,0x00,sizeof(sCif_addr_inf));
	memset(cMx_Fh_flag,0x00,sizeof(cMx_Fh_flag));
	memset(&sTd_mst,0x00,sizeof(sTd_mst));
	memset(&sTd_parm,0x00,sizeof(sTd_parm));
	/**初始化公用结构**/
	if (pub_base_sysinit())
	{
		sprintf(acErrMsg,"初始化公用结构错误!!");
		WRITEMSG
		strcpy(g_pub_tx.reply, "CU06");
		goto ErrExit;
	}
	vtcp_log("[%s][%d],tx_date=[%d]",__FILE__,__LINE__,g_pub_tx.tx_date);
	set_zd_long("0460",g_pub_tx.tx_date);
	/*** 取值 **/
	get_zd_data("0160",cHx_tx_code);
	get_zd_data("0250", cAc_name );
	get_zd_data("0300", cAc_no );
	get_zd_double("0410", &dFee);
	get_zd_long("0440",&lBeg_date);
	get_zd_long("0450",&lEnd_date);
	get_zd_data("0620", cCert_no);
	get_zd_data("0730", cCtrlcmd);
	get_zd_data("0750", cMsr2);
	get_zd_data("0760", cMsr3);
	get_zd_data("0710",cFee_flag);
	vtcp_log("[%s][%d],收费标志=[%s]",__FILE__,__LINE__,cFee_flag);
	get_zd_data("0680",cMx_Fh_flag);
	vtcp_log("[%s][%d],明细分户标志=[%s]",__FILE__,__LINE__,cMx_Fh_flag);
	get_zd_data("0810", cBrf);
	/** 新旧账号转换 **/
	pub_base_old_acno(cAc_no);
	memcpy(g_pub_tx.ac_no,cAc_no,sizeof(g_pub_tx.ac_no)-1);
	zip_space(g_pub_tx.ac_no);
	/**　开始检查各要素　**/
	iRet=pub_com_test(cCtrlcmd,cCert_no,cAc_name,cAc_no,"001","","2",cGz_flag);
	vtcp_log("%s,%d,cCtrlcmd=[%s],cCert_no=[%s],cAc_name=[%s],cAc_no=[%s],cGz_flag=[%s]",__FILE__,__LINE__,cCtrlcmd,cCert_no,cAc_name,cAc_no,cGz_flag);
	if (iRet)
	{
		sprintf(acErrMsg,"%s,%d,公共函数调用失败",__FILE__,__LINE__);
		WRITEMSG
		goto ErrExit;
	}
	if( strlen ( g_pub_tx.ac_no ) >10) /*** 此种情况下,只能是账号 ***/
	{
		iRet = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_no='%s'",g_pub_tx.ac_no);
		if (iRet )
		{
			vtcp_log("%s,%d,ac_no[%s]查找错误!",__FILE__,__LINE__,g_pub_tx.ac_no);
			strcpy(g_pub_tx.reply,"P102");
			goto ErrExit;
		}
                		if(sMdm_ac_rel.draw_pwd_chg[0]=='P'||sMdm_ac_rel.draw_pwd_chg[0]=='M'||sMdm_ac_rel.draw_pwd_chg[0]=='C')
		{
			vtcp_log("%s,%d,ac_no[%s]查找错误![%s]",__FILE__,__LINE__,g_pub_tx.ac_no,sMdm_ac_rel.draw_pwd_chg);
			strcpy(g_pub_tx.reply,"P102");
			goto ErrExit;
		}

		/*增加内部账户的查询*/
		iRet = Prdt_ac_id_Sel(g_pub_tx.reply,&sPrdt_ac_id,"ac_id = %ld",sMdm_ac_rel.ac_id);
		if(iRet)
		{
			vtcp_log("%s,%d,查询prdt_ac_id错误！！",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"P102");
			goto ErrExit;
		}
		if(sPrdt_ac_id.ac_id_type[0] == '1') /*活期*/
		{
			iRet = Dd_mst_Sel(g_pub_tx.reply,&sDd_mst,"ac_id=%ld",sMdm_ac_rel.ac_id);
			if (iRet)
			{
				vtcp_log("%s,%d,查询活期主文件错误!",__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"P102");
				goto ErrExit;
			}
			iRet = Dd_parm_Sel(g_pub_tx.reply,&sDd_parm," prdt_no='%s' and 1=1",sDd_mst.prdt_no);
			if(iRet)
			{
				sprintf(acErrMsg,"%s,%d,Dd_mst_Sel数据库操作失败[%d]",__FILE__,__LINE__,iRet);
				WRITEMSG
				strcpy(g_pub_tx.reply, "P102");
				goto ErrExit;
			}
			/**返回前台的值**/
			set_zd_data("0250",sDd_mst.name);
			set_zd_data("0020",sDd_mst.opn_br_no ); /**/
			vtcp_log("%s,%d,opn_br_no[%s]",__FILE__,__LINE__,sDd_mst.opn_br_no);
			set_zd_long("0260",sDd_mst.cif_no );    /*客户号*/

			/*返回账户类型*/
			set_zd_data("0700",sDd_mst.ac_type);
			/*平台要求返回客户类型1个人 2 对公 3内部 */
			if(sDd_parm.cif_type[0] == '1' || sDd_parm.cif_type[0] == '5') /*个人和内部员工返回1*/
			{
				set_zd_data("0670","1");
			}
			else
			{
				set_zd_data("0670","2");
			}
			/*根据账户冻结状态返回不同值*/
			vtcp_log("%s,%d,note_sts=[%c]",__FILE__,__LINE__,sMdm_ac_rel.note_sts[0]);
			if(sMdm_ac_rel.note_sts[0]=='1')/*已挂失*/
			set_zd_data("0690","5");
			if(sDd_mst.ac_sts[0] == '*')/*销户*/
			set_zd_data("0690","*");
			vtcp_log("%s,%d,ac_sts=[%c]",__FILE__,__LINE__,sDd_mst.ac_sts[0]);
			vtcp_log("%s,%d,hold_sts=[%c]",__FILE__,__LINE__,sDd_mst.hold_sts[0]);
			if(sDd_mst.hold_sts[0] != '0')
			{
				if(sDd_mst.hold_sts[0] == '1')/*已完全冻结*/
				{
					set_zd_data("0690","2");
				}
				else if(sDd_mst.hold_sts[0] == '2')/*只进不出*/
				{
					set_zd_data("0690","4");
				}
				else if(sDd_mst.hold_sts[0] == '3')/*部分冻结*/
				{
					set_zd_data("0690","3");
				}
			}
			/***取余额***/
			account_amt = sDd_mst.bal; /* 账面余额*/
			vtcp_log("%s,%d,sDd_mst_bal[%f]",__FILE__,__LINE__,sDd_mst.bal);
			if(memcmp(g_pub_tx.ac_no,CARDHEADCODE,6)==0) /*是本行的卡 查本日取现金额*/
			{
				/*核心直接返回可用金额*/
				ky_bal = sDd_mst.bal - sDd_mst.ctl_amt - sDd_mst.hold_amt;
				vtcp_log("%s,%d,sDd_mst.bal:[%lf]",__FILE__,__LINE__,sDd_mst.bal);
				vtcp_log("%s,%d,ky_bal:[%lf]",__FILE__,__LINE__,ky_bal);
			}
			else /*不是卡用户*/
			{
				ky_bal = sDd_mst.bal - sDd_mst.ctl_amt - sDd_mst.hold_amt;
			}
			vtcp_log("%s,%d,可用金额[%lf]",__FILE__,__LINE__,ky_bal);
			if (pub_base_CompDblVal(sDd_mst.bal,ky_bal)<0)
			{
				ky_bal = sDd_mst.bal;
			}
			/*如果该账号被全部冻结和只进不出，可用金额为0*/
			if(sDd_mst.hold_sts[0] == '1' || sDd_mst.hold_sts[0] == '2')
			{
				ky_bal = 0.00;
			}
			set_zd_double("0420",ky_bal);/*可用余额*/
		}
		else if (sPrdt_ac_id.ac_id_type[0] == '9') /*内部账号*/
		{
			iRet = In_mst_Sel(g_pub_tx.reply,&sIn_mst,"ac_id = %ld ",sMdm_ac_rel.ac_id);
			if(iRet)
			{
				vtcp_log("%s,%d,查询内部主文件错误！！",__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"A029");
				goto ErrExit;
			}
			iRet = In_parm_Sel(g_pub_tx.reply,&sIn_parm,"prdt_no ='%s'",sIn_mst.prdt_no);
			if(iRet)
			{
				vtcp_log("%s,%d,查询内部产品参数错误！！",__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"W023");
				goto ErrExit;
			}
			set_zd_data("0250",sIn_mst.name); /*户名*/
			set_zd_data("0020",sIn_mst.opn_br_no );/*开户机构*/
			/*
			set_zd_long("0260","");内部账号返回空 客户号*/
			set_zd_data("0270","");/*内部账号没有地址*/
			set_zd_data("0670","3");/*客户类型*/
			set_zd_data("0700",sPrdt_ac_id.ac_id_type);/*账号类型*/
			/*计算可用余额*/
			account_amt = sIn_mst.bal;/*账面金额*/
			vtcp_log("[%s][%d],内部账号账面余额=【%lf】",__FILE__,__LINE__,account_amt);
			ky_bal = sIn_mst.bal - sIn_mst.hold_amt;
			set_zd_double("0420",ky_bal);/*可用余额*/
		}
		else
		{
			vtcp_log("%s,%d,账号类型错误！",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"W003");
			goto ErrExit;
		}
	}
	else
	{
		vtcp_log("%s,%d,ac_no[%s]输入的为科目号，输入错误!",__FILE__,__LINE__,g_pub_tx.ac_no);
		strcpy(g_pub_tx.reply,"L017");
		goto ErrExit;
	}
	/* add by lw 2012-05-30 扣除手续费 */
	if(cFee_flag[0] =='1')	/*收手续费*/
	{
		char cTmp[128]    = {0};
		char cFd123[1024] = {0};
		char cFee_acno[32]= {0};
		char cDc_ind[2]   = {0};
		double dFee = 0.00;
		double dSumFee = 0.00;
		get_fd_data("1230",cFd123);
		memcpy(cTmp, cFd123+1, 2);	
		int iCnt = atoi(cTmp);
		for(int i=0; i<iCnt; i++)
		{
			memset(cFee_acno, 0, sizeof(cFee_acno));
			memcpy(cFee_acno, cFd123+3+48*i, 24);
			zip_space(cFee_acno);
			if(strcmp(cAc_no, cFee_acno) != 0) {
				continue;
			}
			memcpy(cDc_ind, cFd123+27+48*i, 1);
			memset(cTmp, 0, sizeof(cTmp));
			memcpy(cTmp, cFd123+29+48*i, 12);
			dFee = atof(cTmp) / 100.00;
			if(cDc_ind[0] == '1') {
				dFee=-dFee;
			}
			dSumFee += dFee;
		}
		vtcp_log("%s,%d,LW 可用余额[%lf] 余额[%lf] 手续费[%lf]",__FILE__,__LINE__,ky_bal,account_amt,dSumFee);
		account_amt += dSumFee;
		ky_bal += dSumFee;
		set_zd_double("0420",ky_bal);	/*可用余额*/
	}
	/* end by lw 2012-05-30 */
	vtcp_log("%s,%d,可用金额[%lf],余额[%lf] ",__FILE__,__LINE__,ky_bal,account_amt);
	memcpy(tmpstr,"1001156C",8);
	sprintf(tmpstr+8,"%012.0f",account_amt*100);
	memcpy(tmpstr+20,"1002156C",8);
	sprintf(tmpstr+28,"%012.0f",ky_bal*100);
	vtcp_log("%s,%d,查询到的余额是:tmpstr=[%s]",__FILE__,__LINE__,tmpstr);
	set_zd_data("0820",tmpstr);
	/*查询明细或者分户账信息*/
	if(cMx_Fh_flag[0] == '0')
	{
		/*查询活期明细*/
		if(sPrdt_ac_id.ac_id_type[0] == '1')
		{
		       	/* add by slzh 20130317 支持查询指定日期范围*/
			sprintf(cWherelist,"ac_id =%ld and ac_seqn=%d ",sDd_mst.ac_id,sDd_mst.ac_seqn);
	                     	if(lBeg_date)
	                     	{
	                            		sprintf(cWherelist+strlen(cWherelist)," and tx_date>=%ld ",lBeg_date);
	                     	}
			if(lEnd_date)
	                     	{
	                            		sprintf(cWherelist+strlen(cWherelist)," and tx_date<=%ld ",lEnd_date);
	                     	}
			 
			sprintf(cWherelist+strlen(cWherelist)," order by hst_cnt desc");

			if(lBeg_date || lEnd_date )
			{
				if(get_chnl_dowload_file(filename))
			    	{
			    	 	vtcp_log("%s,%d,获取下传平台文件名失败！",__FILE__,__LINE__);
				 	strcpy(g_pub_tx.reply,"CU14");
				  	goto ErrExit;
			    	}
			    	fp=fopen(filename,"w");
			    	if(fp == NULL)
	                  		{
		                  		vtcp_log("打开文件失败! [%s] [%s] [%d]",filename,__FILE__,__LINE__);
		                 		strcpy(g_pub_tx.reply,"L206");
		                  		goto ErrExit;
	                   		}
			}
			else
			{
				End_line = 1;	
			}
			   
			/* add by slzh 20130317 end*/
			iRet = Dd_mst_hst_Dec_Sel(g_pub_tx.reply,"%s",cWherelist);
			if(iRet)
			{
				vtcp_log("%s,%d,查找明细出错！",__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"CU14");
				goto ErrExit;
			}
			i=0;
			while(1)
			{
				vtcp_log("%s,%d,开始查询帐户明细记录!",__FILE__,__LINE__);
				iRet = Dd_mst_hst_Fet_Sel(&sDd_mst_hst,g_pub_tx.reply);
				if(iRet && iRet !=100)
				{
					vtcp_log("%s,%d,查找明细出错！",__FILE__,__LINE__);
					Dd_mst_hst_Clo_Sel();
					strcpy(g_pub_tx.reply,"CU14");
					goto ErrExit;
				}
				else if(iRet == 100)
				{
					vtcp_log("%s,%d,找到的记录数为[%d]",__FILE__,__LINE__,i);
					break;
				}
				/**防止给95域的值太多，限制取到的记录数为100**/
				totnum++;
				if(totnum>End_line)
				{
					/*到100行则不写，跳出*/
					vtcp_log("%s,%d,查找明细过多！totnum=[%d],End_line=[%d]",__FILE__,__LINE__,totnum,End_line);
					Dd_mst_hst_Clo_Sel();
					strcpy(g_pub_tx.reply,"W050");
					goto ErrExit;
				}
				memset(mx_tmp,0,sizeof(mx_tmp));
				memset(brf6,0,sizeof(brf6));
				memset(to_str,0,sizeof(to_str));
				strncpy(brf6,sDd_mst_hst.brf,sizeof(brf6)-1);
				vtcp_log("[%s][%d]brf=[%s]",__FILE__,__LINE__,brf6);
				Zip_halfChinese(brf6,to_str);
				vtcp_log("[%s][%d]brf=[%s]",__FILE__,__LINE__,to_str);
				/**sprintf(mx_tmp,"%08d%-12.12s%014.0lf%014.0lf%-1.1s",sDd_mst_hst.tx_date,to_str,sDd_mst_hst.tx_amt*100,sDd_mst_hst.bal*100,sDd_mst_hst.add_ind);**/
				sprintf(mx_tmp,"%08d%-12.12s%014.0lf%014.0lf%-1.1s%-24.24s%-60.60s",sDd_mst_hst.tx_date,to_str,sDd_mst_hst.tx_amt*100,sDd_mst_hst.bal*100,sDd_mst_hst.add_ind,"","");
				if(lBeg_date || lEnd_date )
				{
					fprintf(fp,"%s\n",mx_tmp);
				}
				else
			      	{
					memcpy(fd95+2+i * 48,mx_tmp,48);
				}
				i ++;
			}
			Dd_mst_hst_Clo_Sel();
			if(i==0)
			{
				memset(mx_tmp,0,sizeof(mx_tmp));
				memset(brf6,0,sizeof(brf6));
				memcpy(to_str,"无",2);
				sprintf(mx_tmp,"%08d%-12.12s%014.0lf%014.0lf",sDd_mst_hst.tx_date,to_str,sDd_mst_hst.tx_amt*100,sDd_mst_hst.bal*100);
				if(lBeg_date || lEnd_date )
				{
				 	fprintf(fp,"%s\n",mx_tmp);
				}
				else
				{
				 	memcpy(fd95+2+i * 48,mx_tmp,48);
				}
				sprintf(mx_tmp,"%02d",1);
			}
			else
			{
				sprintf(mx_tmp,"%02d",i);
			}
			memcpy(fd95,mx_tmp,2);
			set_zd_data("0950",fd95);
			vtcp_log("%s,%d,查询账户明细结束!fd95[%s]",__FILE__,__LINE__,fd95);
		}
	}
	else if(cMx_Fh_flag[0] == '1')
	{
		/*查询分户账信息*/
		iRet = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_no='%s'",g_pub_tx.ac_no);
		if (iRet )
		{
			vtcp_log("%s,%d,ac_no[%s]查找错误!",__FILE__,__LINE__,g_pub_tx.ac_no);
			strcpy(g_pub_tx.reply,"P102");
			goto ErrExit;
		}
		iRet = Prdt_ac_id_Dec_Sel(g_pub_tx.reply,"ac_id =%ld order by ac_seqn",sMdm_ac_rel.ac_id);
		if (iRet )
		{
			vtcp_log("%s,%d,Prdt_ac_id_Dec_Sel错误!",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"K106");
			goto ErrExit;
		}
		if(get_chnl_dowload_file(filename))
	    	{
	    	 	vtcp_log("%s,%d,获取下传平台文件名失败！",__FILE__,__LINE__);
		 	strcpy(g_pub_tx.reply,"CU14");
		  	goto ErrExit;
	    	}
	    	fp=fopen(filename,"w");
	    	if(fp == NULL)
          		{
                  		vtcp_log("打开文件失败! [%s] [%s] [%d]",filename,__FILE__,__LINE__);
                 		strcpy(g_pub_tx.reply,"L206");
                  		goto ErrExit;
           		}	          
		while(1)
		{
			memset(&sPrdt_ac_id_fh,0x00,sizeof(sPrdt_ac_id_fh));
			memset(&sDd_mst,0x00,sizeof(sDd_mst));
			memset(&sTd_mst,0x00,sizeof(sTd_mst));
			iRet=Prdt_ac_id_Fet_Sel(&sPrdt_ac_id_fh,g_pub_tx.reply);
			if(iRet !=0 && iRet != 100)
			{
				vtcp_log("%s,%d,Prdt_ac_id_Fet_Sel错误!",__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"K106");
				goto ErrExit;
			}
			else if(iRet == 100 && iFhcnt == 0)
			{
				vtcp_log("%s,%d,Prdt_ac_id_Fet_Sel错误!",__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"K106");
				goto ErrExit;
			}
			else if(iRet == 100)
			{
				vtcp_log("%s,%d,当前共%d子账户",__FILE__,__LINE__,iFhcnt);
				break;
			}
			if(sPrdt_ac_id_fh.ac_id_type[0] == '1') /*活期分户*/
			{
				iRet = Dd_mst_Sel(g_pub_tx.reply,&sDd_mst,"ac_id=%ld and ac_seqn = %d ",sPrdt_ac_id_fh.ac_id,sPrdt_ac_id_fh.ac_seqn);
				if(iRet)
				{
					vtcp_log("%s,%d,Dd_mst_Sel错误!",__FILE__,__LINE__);
					strcpy(g_pub_tx.reply,"K107");
					goto ErrExit;
				}
				iRet = Dd_parm_Sel(g_pub_tx.reply,&sDd_parm,"prdt_no = '%s'",sPrdt_ac_id_fh.prdt_no);
			            	if(iRet)
			            	{
			               		vtcp_log("%s,%d, 活期参数表不存在该产品!!iRet=[%d]",__FILE__,__LINE__,iRet);
			               		strcpy(g_pub_tx.reply,"W015");
			               		goto ErrExit;
			            	}
				memset(mx_tmp,0,sizeof(mx_tmp));
				memset(brf6,0,sizeof(brf6));
				strcpy(brf6,sDd_parm.title);
				vtcp_log("%s,%d, 压缩前的brf6[%s]",__FILE__,__LINE__,brf6);
				vtcp_log("%s,%d, 压缩前的to_str[%s]",__FILE__,__LINE__,to_str);
				memset(to_str,0,sizeof(to_str));
				Zip_halfChinese(brf6,to_str);
				vtcp_log("%s,%d, 压缩后的brf6[%s]",__FILE__,__LINE__,brf6);
				vtcp_log("%s,%d, 压缩后的to_str[%s]",__FILE__,__LINE__,to_str);
				/*格式：账户序号(子账号)+存款类型名称+账户余额+起息日期+到期日期*/
				sprintf(mx_tmp,"%03d %-24.24s%012.0lf%-08d--------",sDd_mst.ac_seqn,to_str,sDd_mst.bal*100,sDd_mst.ic_date);				
			}
			else if(sPrdt_ac_id_fh.ac_id_type[0] == '2')/*定期分户*/
			{
				iRet = Td_mst_Sel(g_pub_tx.reply,&sTd_mst,"ac_id=%ld and ac_seqn = %d ",sPrdt_ac_id_fh.ac_id,sPrdt_ac_id_fh.ac_seqn);
				if(iRet)
				{
					vtcp_log("%s,%d,Td_mst_Sel错误!iRet=[%d]",__FILE__,__LINE__,iRet);
					strcpy(g_pub_tx.reply,"K107");
					goto ErrExit;
				}
				iRet =Td_parm_Sel(g_pub_tx.reply,&g_td_parm,"prdt_no='%s'",sPrdt_ac_id_fh.prdt_no);
				if( iRet )
			        	{
			        		vtcp_log("%s,%d,定期参数表不存在该产品!!iRet=[%d]",__FILE__,__LINE__,iRet);
			               		strcpy( g_pub_tx.reply, "M003" );
			               		goto ErrExit;
			        	}
				memset(mx_tmp,0,sizeof(mx_tmp));
				memset(brf6,0,sizeof(brf6));
				strcpy(brf6,g_td_parm.title);
				Zip_halfChinese(brf6,to_str);
				/*格式：账户序号(子账号)+存款类型名称+账户余额+起息日期+到期日期*/
				sprintf(mx_tmp,"%03d %-24.24s%012.0lf%-08d%-08d",sTd_mst.ac_seqn,to_str,sTd_mst.bal*100,sTd_mst.ic_date,sTd_mst.mtr_date);
			}
			else
			{
				vtcp_log("%s,%d,其他账户不统计!  [%s]",__FILE__,__LINE__,sPrdt_ac_id_fh.ac_id_type);
				continue;
			}
			fprintf(fp,"%s\n",mx_tmp);
			iFhcnt ++;
		}
		Prdt_ac_id_Clo_Sel();
		memset(mx_tmp,0x00,sizeof(mx_tmp));
		sprintf(mx_tmp,"%02d",iFhcnt);
		memcpy(fd95,mx_tmp,2);
		set_zd_data("0950",fd95);
		vtcp_log("%s,%d,查询分户账账户结束!fd95[%s]",__FILE__,__LINE__,fd95);
	}
	else if(cMx_Fh_flag[0] == 'N')
	{
		vtcp_log("%s,%d,不进行明细或者分户账查询[%s]",__FILE__,__LINE__,cMx_Fh_flag);
	}
	else
	{
		vtcp_log("%s,%d,查询标志错误[%s]",__FILE__,__LINE__,cMx_Fh_flag);
		goto ErrExit;
	}
	if(cFee_flag[0] =='1')
	{
		strcpy(g_pub_tx.brf,"查询手续费");
		if (pub_ins_trace_log())
		{
			sprintf(acErrMsg, "登记交易流水错误!");
			WRITEMSG
			goto ErrExit;
		}
	}

	if((cMx_Fh_flag[0] == '0' && (lBeg_date || lEnd_date)) || cMx_Fh_flag[0] == '1' )
      	{
		fclose(fp);
               	char file_tmp[64];
		memset(file_tmp,0x00,sizeof(file_tmp));
		memcpy(file_tmp,filename+(strlen(filename)-26),26);
		sprintf(tmp_cmd,"send_back_6255.sh  %s",file_tmp);
		iRet=system(tmp_cmd);
		if(iRet)
		{
			vtcp_log("下传文件出错! [%s] [%s] [%d]",tmp_cmd,__FILE__,__LINE__);
			strcpy( g_pub_tx.reply, "S047" );
			goto ErrExit;
		}
	}
	
GoodExit:	
	sprintf(acErrMsg,"Before OK return: reply [%s]",g_pub_tx.reply);
	strcpy(g_pub_tx.reply,"0000");
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	if(fp!=NULL)
	{
	       	fclose(fp);
		fp=NULL;
	}
	if(memcmp(g_pub_tx.reply,"0000",4) == 0)
	{
		strcpy(g_pub_tx.reply, "H034");
	}
	sprintf(acErrMsg,"Before bad return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
int Zip_halfChinese(char *fstr,char *tstr)
{
	vtcp_log("%s,%d,输入数据[%s]输出数据[%s]\n",__FILE__,__LINE__,fstr,tstr);
	int len=0;
	char *strf=NULL;
	char *strt=NULL;
	len=strlen(fstr);
	if(len == 0)
	{
		return(0);
	}
	strt = tstr;
	strf = fstr;
	vtcp_log("strf[%s],fstr+len[%d]\n",strf,fstr+len);
	for(strf=fstr; strf <= fstr+len;)
	{
		if(*strf > 128)
		{
			if(*(strf+1) > 128)
			{
				*strt = *strf;
				*(strt+1) = *(strf+1);
			}
			else
			{
				*strt = 32;
				*(strt+1) = *(strf+1);
			}
			strf=strf+2;
			strt=strt+2;
		}
		else
		{
			*strt = *strf;
			strf++;
			strt++;
		}
	}
	vtcp_log("%s,%d,输出数据[%s]\n",__FILE__,__LINE__,tstr);
	return(0);
}
/* filename 缓冲不小于64byte*/
int get_chnl_dowload_file(char *filename)
{
       	 char cTx_code[4+1];
	 long lTx_date=0;
	 long lTrace_no=0;
         char tmp[256];
	 char tmpfilename[256];
		
         memset(cTx_code,0x00,sizeof(cTx_code));
	 memset(tmp,0x00,sizeof(tmp));
	 memset(filename,' ',sizeof(filename));
	 
	 get_zd_data("0160",cTx_code);
	 get_zd_long("0440",&lTx_date);
	 get_zd_long("0520",&lTrace_no);
	 if(strlen(cTx_code)==0 || lTx_date==0 || lTrace_no==0)
	 {
	        sprintf(acErrMsg, "获取下传文件名要素失败[%s][%ld][%ld]!",cTx_code,lTx_date,lTrace_no);
	        WRITEMSG
	        return -1;
	 }
	sprintf(tmp,"%s/%s_%ld_%012ld",getenv("TMPDIR"),cTx_code,lTx_date,lTrace_no);
	sprintf(tmpfilename,"%s_%ld_%012ld",cTx_code,lTx_date,lTrace_no);
	vtcp_log("%s,%d,核心传往平台文件名[%s]\n",__FILE__,__LINE__,tmpfilename);
	set_zd_data("0270",tmpfilename);/*给平台返回文件名*/
	strcpy(filename,tmp);

        return 0;
	 
}

