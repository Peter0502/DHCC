/************************************
** 程序名：spW001.c
** 功  能：网上银行查询,验证与挂失 
** 作  者: wyw
** 日  期: 20090328
** 修改记录:
*************************************/
#include <string.h>
#define  EXTERN
#include "public.h"
#include "dc_log_rz_c.h"
#include "dc_log_c.h"
#include "com_sys_parm_c.h"
#include "cif_cop_inf_c.h"
#include "cif_addr_inf_c.h"
#include "cif_id_code_rel_c.h"
#include "chnl_self_trade_c.h"
#include "lv_pkgreg_c.h"
#include "hv_zf_c.h"
#include "lv_param_c.h"

int spW001()
{
	vtcp_log("%s,%d,进入网银查询和验证接口!",__FILE__,__LINE__);
	long dTx_num=0;      /* 交易码，用来区分不同的交易 **/
	long dCif_no=0;
	long dAc_seqn=0;
	int    ret;
	long    traceno;
	long   txdate;
	char tmp[10];
	memset(tmp,'\0',sizeof(tmp));
	get_zd_data("0070",tmp);
	vtcp_log("柜员号---------[%s][%d]------------------》》》[%s]",__FILE__,__LINE__,tmp);

	char sPc_flag[2];   /* 对公对私标志 */
	char Ac_no[25];
	char Id_type[2], Id_no[21];
	char sTmp[21];
	char filename[101];
	char ac_std[15];
	char bz[15];
	char sName[61];
	char Act_type[2];
	char tx_code[5];
	char      cStat[21];

char *iNum2Stat(char ch, char *cTmpList);  /*小额交易状态转换*/
char *iNum2Stat1(char ch, char *cTmpList); /*大额交易状态转换*/
 
	FILE *fp;
	fp = NULL;
	struct  dc_log_c        sDc_log;
	struct  dc_log_rz_c	    sDc_log_rz;
	struct  com_sys_parm_c  sCom_sys_parm;
	struct  dd_mst_c  sDd_mst;
	struct  mdm_ac_rel_c  sMdm_ac_rel;
	struct  mdm_ac_rel_c  g_mdm_ac_rel;
	struct  com_branch_c  sCom_branch;
	struct  cif_cop_inf_c   sCif_cop_inf;
	struct  cif_addr_inf_c  sCif_addr_inf;
	struct  cif_id_code_rel_c  sCif_id_code_rel;
	struct	chnl_self_trade_c   sChnl_self_trade;
	struct	lv_pkgreg_c lV_pkgreg;
	struct	hv_zf_c  Hv_zf;
	struct	lv_param_c lv_param_1;

	memset(&sDd_mst,0x00,sizeof(struct dd_mst_c));
	memset(&sMdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&sCom_branch,0x00,sizeof(struct com_branch_c));
	memset(&sDc_log,0x00,sizeof(struct dc_log_c));
	memset(&sDc_log_rz,0x00,sizeof(struct dc_log_rz_c));
	memset(&sCom_sys_parm,0x00,sizeof(struct com_sys_parm_c));
	memset(&lV_pkgreg,0x00,sizeof(struct lv_pkgreg_c));
	memset(&Hv_zf,0x00,sizeof(struct hv_zf_c));    /**add Leijh 101027**/
	memset(&lv_param_1,0x00,sizeof(struct lv_param_c));
	
  memset(cStat, 0x00, sizeof(cStat));  /**add Leijh 101027**/ 
	memset(sPc_flag,0,sizeof(sPc_flag));
	memset(Ac_no,0,sizeof(Ac_no));
	memset(Id_type,0,sizeof(Id_type));
	memset(Id_no,0,sizeof(Id_no));
	memset(sTmp,0,sizeof(sTmp));
	memset(filename,0,sizeof(filename));
	memset(ac_std,0,sizeof(ac_std));
	memset(bz,0,sizeof(bz));
	memset(sName,0,sizeof(sName));
	memset(tx_code,0,sizeof(tx_code));

	
	pub_base_sysinit();	
	{
		char buf[60];
		get_zd_data("0220", buf);
		vtcp_log("0220, %d@%s buf[%s]", __LINE__, __FILE__, buf);
	}
	get_zd_long("0500",&dTx_num);
	get_zd_data("0660",sPc_flag);
	vtcp_log("%s,%d,交易序号[%d],交易标志[%s]",__FILE__,__LINE__,dTx_num,sPc_flag);
	
	if(dTx_num==1)
	{
		get_zd_data("0300",Ac_no);	
		vtcp_log("%s,%d,储蓄账户信息查询(gw1005),Ac_no[%s]",__FILE__,__LINE__,Ac_no);
		goto OkExit1;
	}else if(dTx_num==2)
	{
		get_zd_data("0300",Ac_no);	
		vtcp_log("%s,%d,储蓄账户历史信息查询(gw1006),Ac_no[%s]",__FILE__,__LINE__,Ac_no);
		goto OkExit2;
	}else if(dTx_num==3)
	{
		get_zd_data("0300",Ac_no);
		set_zd_data("0220","11"); /*口挂*/
		get_zd_data("0630",Id_no);
		get_zd_data("0680",Id_type);
		/*不允许网银重复挂失*/
		ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &g_mdm_ac_rel,"ac_no ='%s'", Ac_no);
		if ( ret == 100 )
		{
			sprintf(acErrMsg,"介质账户对照表中不存在该记录[%d]",ret);
			WRITEMSG
			    strcpy (g_pub_tx.reply,"W010");
			goto ErrExit;;
		}
		else if ( ret )
		{
			sprintf(acErrMsg,"SELECT mdm_ac_rel error,error code=[%d]",ret);
			WRITEMSG
			    strcpy (g_pub_tx.reply,"W011");
			goto ErrExit;;
		}
		if(strcmp(g_mdm_ac_rel.note_sts,"1") == 0)
		{
			sprintf(acErrMsg,"该账号已挂失[%s]",Ac_no);
			WRITEMSG
			strcpy (g_pub_tx.reply,"P170");
			goto ErrExit;
		}
		vtcp_log("%s,%d,网银挂失(口挂)(gw1007),Ac_no[%s],id_no[%s],id_type[%s]",__FILE__,__LINE__,Ac_no,Id_no,Id_type);
		goto OkExit3;
	}else if(dTx_num==4)
	{
		get_zd_data("0300",Ac_no);
		vtcp_log("%s,%d,卡号详细信息查询(gw0044),Ac_no[%s]",__FILE__,__LINE__,Ac_no);
		goto OkExit4;
	}else if(dTx_num==5)
	{
		get_zd_data("0300",Ac_no);
		get_zd_long("0340",&dAc_seqn);
		vtcp_log("%s,%d,子帐号详细信息查询(gw0045),Ac_no[%s],ac_seqn[%d]",__FILE__,__LINE__,Ac_no,dAc_seqn);
		goto OkExit5;
	}else if(dTx_num==6)
	{
		get_zd_data("0300",Ac_no);
		vtcp_log("%s,%d,对公账户交易明细查询(gw6005),Ac_no[%s],ac_seqn[%d]",__FILE__,__LINE__,Ac_no,dAc_seqn);
		goto OkExit2;
	}else if(dTx_num==7)
	{
		get_zd_data("0300",Ac_no);
		vtcp_log("%s,%d,对公账户信息概览(gw6002),Ac_no[%s]",__FILE__,__LINE__,Ac_no);
		goto OkExit6;
	}
	/**add by lhg 20120307**/
	else if(dTx_num==15)
	{
		get_zd_data("0300",Ac_no);
		vtcp_log("%s,%d,定活互转子账号查询(gw1205),Ac_no[%s]",__FILE__,__LINE__,Ac_no);
		goto OkExit7;
	}
	/**add end**/
	else if(dTx_num==8)  /*单笔流水查证交易*/
	{
		double zfxt_fee=0.0;
		char cTmp70[2]={0}; /**LOONG 2010902**/
		/**若是大小额还需要在发送一个标志过来，查看是否发送成功!**/
		get_zd_long("0520", &traceno);
		get_zd_long("0440", &txdate);
		get_zd_data("0300", Ac_no);
		get_zd_data("0920", tx_code);
		vtcp_log("%s,%d,单笔流水查证(gw1156),traceno[%d],txday[%d],tx_code[%s]",
			__FILE__,__LINE__,traceno,txdate,tx_code);

		
		ret = Com_sys_parm_Sel(g_pub_tx.reply,&sCom_sys_parm,"1=1");
		if(ret)
		{
			sprintf(acErrMsg, "读取com_sys_parm表异常![%d]", ret);
			WRITEMSG
			strcpy(g_pub_tx.reply, "O005");
			set_zd_data("0700", "3");
			goto ErrExit;
		}

		if(sCom_sys_parm.sys_date-txdate == 0)
		{
			ret=Dc_log_Sel(g_pub_tx.reply,&sDc_log,"tx_date=%ld and trace_no=%d and sts='0'",txdate,traceno);
			/**ret=Dc_log_Sel(g_pub_tx.reply,&sDc_log,"trace_no=%d and sts='0'",traceno);**/
			if(ret==0)
			{
				ret = sql_sum_double("dc_log","amt",&zfxt_fee,"trace_no=%d and entry_code in('F00000')",traceno);
				if(ret != 0 && ret != 1403)
				{
					vtcp_log("%s,%d,统计手续费错误!",__FILE__,__LINE__);
					strcpy(g_pub_tx.reply,"GR19");
					set_zd_data("0700", "3");
					goto ErrExit;		
				}
				else if(ret == 1403)
				{
					zfxt_fee=0.0;
					vtcp_log("%s,%d, LOONG-gw1156 HERE1 ",__FILE__,__LINE__);
				}
				/* 如果是小额支付，张万松 20100423 修改 */
				if (!strncmp(tx_code, "6388", 4))
				{
					ret = Lv_pkgreg_Sel(g_pub_tx.reply, &lV_pkgreg, "jz_date=%d and input_trace_no=%d and pay_ac_no='%s'", txdate, traceno, Ac_no);
					if ((ret != 0) && (ret != 100))
					{
						vtcp_log("%s,%d,查询小额支付交易主表失败!",__FILE__,__LINE__);
						strcpy(g_pub_tx.reply,"GR19");
						set_zd_data("0700", "1");
						goto ErrExit;
					}
					if (ret == 100)
					{
						/* 无登记 */
						set_zd_data("0700", "2");
						/**2是不存在,跳到错误处理去 LOONG 20100902 **/
						vtcp_log("%s,%d, LOONG-gw1156 HERE4  ",__FILE__,__LINE__);
						strcpy(g_pub_tx.reply,"GR19");
						goto ErrExit;
					}
					if (ret == 0)
					{						
						 memset(cStat, 0x00, sizeof(cStat));
             iNum2Stat(lV_pkgreg.lv_sts[0], cStat);  /*交易状态*/
             set_zd_data("0700", cStat);  /*返回数据*/
             vtcp_log("%s,%d,开始准备小额状态值[%s]!",__FILE__,__LINE__,cStat);
						/********
						else
						{
							char tmppp[2] = {0};
							set_zd_data("0700", "3");
							get_zd_data("0700", tmppp);
							vtcp_log("%s,%d,开始准备小额状态值[%s]!",__FILE__,__LINE__,tmppp);
						}  *****/
					}
					/* 这里赋值,上面就白干了 LOONG 20100902
					set_zd_data("0700", "0");
					*/
					set_zd_double("0410",zfxt_fee);
					goto  OkExit;
				}
				/********beign Leijh for 大额支付返回**********/
				else 	if (!strncmp(tx_code, "6389", 4))
				{
					ret = Hv_zf_Sel(g_pub_tx.reply, &Hv_zf, "tx_date=%d and trace_no=%d and pay_ac_no='%s'", txdate, traceno, Ac_no);
					if ((ret != 0) && (ret != 100))
					{
						vtcp_log("%s,%d,查询大额支付交易主表失败!",__FILE__,__LINE__);
						strcpy(g_pub_tx.reply,"GR19");
						set_zd_data("0700", "1");
						goto ErrExit;
					}
					if (ret == 100)
					{
						/* 无登记 */
						set_zd_data("0700", "2");
						/**2是不存在,跳到错误处理去 LOONG 20100902 **/
						vtcp_log("%s,%d, LOONG-gw1156 HERE4  ",__FILE__,__LINE__);
						strcpy(g_pub_tx.reply,"GR19");
						goto ErrExit;
					}
					if (ret == 0)
					{ /**********
						if (Hv_zf.hv_sts[0] == '2')
						{
							set_zd_data("0700", "0");
						}
						else
						{
							char tmppp[2] = {0};
							set_zd_data("0700", "3");
							get_zd_data("0700", tmppp);
							vtcp_log("%s,%d,开始准备大额状态值[%s]!",__FILE__,__LINE__,tmppp);
						}  ****/
						 memset(cStat, 0x00, sizeof(cStat));
		         iNum2Stat1(Hv_zf.hv_sts[0], cStat);  /*大额交易状态*/
		         set_zd_data("0700", cStat);  /*返回数据*/
		         vtcp_log("%s,%d,Hv_zf开始准备大额状态值[%s]!",__FILE__,__LINE__,cStat);  /**Leijh**/
					}
					
					set_zd_double("0410",zfxt_fee);
					goto  OkExit;
				}
					/********end Leijh for 大额支付返回**********/
				else /**除了小额交易,其他交易也得处理下 LOONG 20100902**/
				{
						vtcp_log("%s,%d, LOONG-gw1156 HERE3  ",__FILE__,__LINE__);
						set_zd_data("0700", "0");
						set_zd_double("0410",zfxt_fee);
						goto  OkExit;
				}
			}
			else if(ret!=0)
			{
				strcpy(g_pub_tx.reply,"GR19");
				set_zd_data("0700", "3");
				goto  ErrExit;
			}	     
		}
		else if(sCom_sys_parm.sys_date-txdate != 0)
		{
			ret=Dc_log_rz_Sel(g_pub_tx.reply,&sDc_log_rz,"tx_date=%ld and trace_no=%d and sts='0'", txdate,traceno);
			if(ret==0)
			{
				ret = sql_sum_double("dc_log_rz","amt",&zfxt_fee,"trace_no=%d and entry_code in('F00000')",traceno);
				if(ret != 0 && ret != 1403)
				{
					vtcp_log("%s,%d,统计手续费错误!",__FILE__,__LINE__);
					strcpy(g_pub_tx.reply,"GR19");
					set_zd_data("0700", "3");
					goto ErrExit;		
				}
				else if(ret == 1403)
				{
					zfxt_fee=0.0;
				}

				if (!strncmp(tx_code, "6388", 4))
				{
					ret = Lv_pkgreg_Sel(g_pub_tx.reply, &lV_pkgreg, "jz_date=%d and input_trace_no=%d and pay_ac_no='%s'", txdate, traceno, Ac_no);
					if ((ret != 0) && (ret != 100))
					{
						vtcp_log("%s,%d,查询小额支付交易主表失败!",__FILE__,__LINE__);
						strcpy(g_pub_tx.reply,"GR19");
						set_zd_data("0700", "1");
						goto ErrExit;
					}
					if (ret == 100)
					{
						/* 无登记 */
						set_zd_data("0700", "2");
						/**2是不存在,跳到错误处理去 LOONG 20100902 **/
						vtcp_log("%s,%d, LOONG-gw1156 HERE4  ",__FILE__,__LINE__);
						strcpy(g_pub_tx.reply,"GR19");
						goto ErrExit;
					}
					if (ret == 0)
					{	
					   memset(cStat, 0x00, sizeof(cStat));
             iNum2Stat(lV_pkgreg.lv_sts[0], cStat);  /*交易状态*/
             set_zd_data("0700", cStat);  /*返回数据*/
             vtcp_log("%s,%d,开始准备小额状态值[%s]!",__FILE__,__LINE__,cStat);
					}
					/* 这里赋值,上面就白干了 LOONG 20100902
					set_zd_data("0700", "0");
					*/
					set_zd_double("0410",zfxt_fee);
					goto  OkExit;
				}
				/********beign Leijh for 大额支付返回**********/
				else 	if (!strncmp(tx_code, "6389", 4))
				{
					ret = Hv_zf_Sel(g_pub_tx.reply, &Hv_zf, "tx_date=%d and trace_no=%d and pay_ac_no='%s'", txdate, traceno, Ac_no);
					if ((ret != 0) && (ret != 100))
					{
						vtcp_log("%s,%d,Hv_zf查询大额支付交易主表失败!",__FILE__,__LINE__);
						strcpy(g_pub_tx.reply,"GR19");
						set_zd_data("0700", "1");
						goto ErrExit;
					}
				
						if (ret == 100)
					{
						/* 无登记 */
						set_zd_data("0700", "2");
						/**2是不存在,跳到错误处理去 LOONG 20100902 **/
						vtcp_log("%s,%d,Hv_zf LOONG-gw1156 HERE4  ",__FILE__,__LINE__);
						strcpy(g_pub_tx.reply,"GR19");
						goto ErrExit;
					}
					if (ret == 0)
					{
						/*************************
						if (Hv_zf.hv_sts[0] == '2')
						{
							set_zd_data("0700", "0");
						}
						else
						{
							char tmppp[2] = {0};
							set_zd_data("0700", "3");
							get_zd_data("0700", tmppp);
							vtcp_log("%s,%d,Hv_zf开始准备大额状态值[%s]!",__FILE__,__LINE__,tmppp);
						}   *************/
						 memset(cStat, 0x00, sizeof(cStat));
		         iNum2Stat1(Hv_zf.hv_sts[0], cStat);  /*大额交易状态*/
		         set_zd_data("0700", cStat);  /*返回数据*/
		         vtcp_log("%s,%d,Hv_zf开始准备大额状态值[%s]!",__FILE__,__LINE__,cStat);
	 				}
					
					set_zd_double("0410",zfxt_fee);
					goto  OkExit;
				}
					/********end Leijh for 大额支付返回**********/
				else /**除了小额交易,其他交易也得处理下 LOONG 20100902**/
				{
						vtcp_log("%s,%d, LOONG-gw1156 HERE3  ",__FILE__,__LINE__);
						set_zd_data("0700", "0");
						set_zd_double("0410",zfxt_fee);
						goto  OkExit;
				}
     	}
     	else if(ret)
     	{
				strcpy(g_pub_tx.reply,"D104");
				set_zd_data("0700", "3");
				goto  ErrExit;
     	}
  	}
  	/**test 把70域取出来看看  LOONG 20100902**/
  	get_zd_data("0700", cTmp70);
  	vtcp_log("%s,%d, LOONG-gw1156 HERE2 cTmp70[%s] ",__FILE__,__LINE__,cTmp70);
  	if(strlen(cTmp70)<1)
  	{
  		set_zd_data("0700", "3");
  		goto ErrExit;
  	}
  	vtcp_log("%s,%d, LOONG-gw1156 HERE3  ",__FILE__,__LINE__);
		goto OkExit;
	}
	else if(dTx_num==9 || dTx_num==10)
	{
		get_zd_data("0300",Ac_no);
		get_zd_data("0920",Act_type);
		get_zd_data("0680",Id_type);
		get_zd_data("0630",Id_no);
		vtcp_log("%s,%d,账户信息查询(gw6002),企业客户信息查询(gw6064),ac_no[%s]",__FILE__,__LINE__,Ac_no);
		pub_base_AllDwnFilName(filename);
		fp = fopen(filename, "w");
		if (fp == NULL) {
			sprintf(acErrMsg, "open file error [%s]", filename);
			WRITEMSG
			strcpy(g_pub_tx.reply, "S047");
			goto ErrExit;
		}
		if(dTx_num==9)
		{
			fprintf(fp, "~户名|币种|核心客户号|开户网点|账户状态|\n");
		}else if(dTx_num==10)
		{
			fprintf(fp, "~账号|证件种类|证件号码|客户中文名称|企业地址|邮政编码|联系电话|\n");
		}

		vtcp_log("%s,%d开始逐条查询帐号信息",__FILE__,__LINE__);
		ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_no='%s'",Ac_no);
		if(ret){
			sprintf(acErrMsg, "读取Mdm_ac_rel表异常![%d]", ret);
			WRITEMSG
			strcpy(g_pub_tx.reply, "P147");
			goto ErrExit;
		}

		ret=Dd_mst_Sel(g_pub_tx.reply,&sDd_mst,"ac_id='%ld'",sMdm_ac_rel.ac_id);
		if(ret){
			sprintf(acErrMsg,"读取dd_mst表异常!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"W015");
			goto ErrExit;
		}
		if(sDd_mst.ac_sts[0]=='1')
		{
			strcpy( ac_std,"正常");
		}
		else if(sDd_mst.ac_sts[0]=='3')
		{
			strcpy( ac_std,"挂失");
		}
		else if(sDd_mst.ac_sts[0]=='*')
		{
			strcpy( ac_std,"销户");	
		}
		ret=Cif_cop_inf_Sel(g_pub_tx.reply,&sCif_cop_inf,"cif_no='%ld'",sDd_mst.cif_no);
		ret=Cif_addr_inf_Sel(g_pub_tx.reply,&sCif_addr_inf,"cif_no='%ld'",sDd_mst.cif_no);
		if(ret){
			vtcp_log("%s,%d,客户号不存在!",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"D104");
			goto  ErrExit;
		}
		ret=Com_branch_Sel(g_pub_tx.reply,&sCom_branch,"br_no='%s'",sDd_mst.opn_br_no);
		if(ret){
			sprintf(acErrMsg,"读取com_branch表异常!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"D172");
			goto ErrExit;
		} 
		strcpy(bz,"CNY");
		if(dTx_num==9)
		{
			if(Act_type[0] != sDd_mst.ac_type[0] )
			{
			/***以前开户的帐户类型都为空，所以加上了后面的判断***/
				vtcp_log("%s,%d,账户类型不正确!dd_mst[%s],0920[%s]",__FILE__,__LINE__,sDd_mst.ac_type,Act_type);
				strcpy(g_pub_tx.reply,"D112");
				goto ErrExit;
			}
			fprintf(fp, "%60s|%5s|%8d|%30s|%30s|\n",sDd_mst.name,bz,sDd_mst.cif_no,sCom_branch.br_no,sDd_mst.ac_sts);
		}
		else if(dTx_num==10)
		{
			/** 比较证件类型和证件号码 */
			if(memcmp(sMdm_ac_rel.id_type,Id_type,1) || memcmp(sMdm_ac_rel.id_no,Id_no,strlen(sMdm_ac_rel.id_no)))
			{
				vtcp_log("%s,%d,证件类型或证件号码不符!",__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"O086");
				goto ErrExit;
			}
			zip_space(sCif_addr_inf.addr);
			zip_space(sCif_addr_inf.post_code);
			if (strlen(sCif_addr_inf.addr) == 0)
			{
				strcpy(sCif_addr_inf.addr, "未登记地址");
			}
			if (strlen(sCif_addr_inf.post_code) == 0)
			{
				strcpy(sCif_addr_inf.post_code, "9999");
			}
			fprintf(fp,"%21s|%2s|%20s|%60s|%60s|%6s|\n",Ac_no,sMdm_ac_rel.id_type,sMdm_ac_rel.id_no,sDd_mst.name,sCif_addr_inf.addr,sCif_addr_inf.post_code);
		}
		fclose(fp);
		set_zd_data(DC_FILE_SND, "1"); 
	}else if(dTx_num==11 || dTx_num==12 || dTx_num==13)
	{
		get_zd_data("0300",Ac_no);
		get_zd_long("0280",dCif_no);
		get_zd_data("0250",sName);
		get_zd_data("0630",Id_no);
		get_zd_data("0680",Id_type);
		if(dTx_num==13)
		{
			ret = Cif_id_code_rel_Sel(g_pub_tx.reply,&sCif_id_code_rel,"cif_no=%ld",dCif_no);
			if(ret)
			{
				vtcp_log("%s,%d,客户号不存在!",__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"C051");
				goto ErrExit;
			}else if(ret==0)
			{
				vtcp_log("%s,%d,验证成功!",__FILE__,__LINE__);
				goto OkExit;
			}
		}
		if(dTx_num==11 || dTx_num==12)
		{
			ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_no='%s'",Ac_no);
			if(ret)
			{
				vtcp_log("%s,%d,账号不存在!",__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"C115");
				goto ErrExit;
			}
			ret = Dd_mst_Sel(g_pub_tx.reply,&sDd_mst,"ac_id=%d and ac_seqn=1",sMdm_ac_rel.ac_id);
			if(ret)
			{
				vtcp_log("%s,%d,账号不存在!",__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"C115");
				goto ErrExit;
			}
			if(strcmp(sDd_mst.name,sName))
			{
				vtcp_log("%s,%d,户名不符合!",__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"B005");
				goto ErrExit;
			}
			if(strcmp(sMdm_ac_rel.id_no,Id_no))
			{
				vtcp_log("%s,%d,户名不符合!",__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"M004");
				goto ErrExit;
			}
			if(strcmp(sMdm_ac_rel.id_type,Id_type))
			{
				vtcp_log("%s,%d,证件类型不符!",__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"P186");
				goto ErrExit;
			}
			ret=Cif_id_code_rel_Sel(g_pub_tx.reply,&sCif_id_code_rel,"id_type='%s' and id_no='%s'",sMdm_ac_rel.id_type,sMdm_ac_rel.id_no);
			if(ret)
			{
				vtcp_log("%s,%d,客户号不存在!",__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"C007");
				goto ErrExit;
			}
			if(dCif_no != sCif_id_code_rel.cif_no)
			{
				vtcp_log("%s,%d,客户号不符!",__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"L019");
				goto ErrExit;
			}
		}
	}else if(dTx_num==14)
	{
		get_zd_data("0300",Ac_no);
		vtcp_log("%s,%d,账户余额查询!",__FILE__,__LINE__);
		ret =pub_base_disac_file(Ac_no,1,Act_type);
		if(ret)
		{
			vtcp_log("%s,%d,账号不存在!",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"D104");
			goto ErrExit;
		}
		if(Act_type[0]=='1')
		{
			set_zd_double("0410",g_dd_mst.bal);	
		}else if(Act_type[0]=='2')
		{
			set_zd_double("0410",g_td_mst.bal);	
		}else if(Act_type[0]=='3'){
			set_zd_double("0410",g_ln_mst.bal);	
		}else if(Act_type[0]=='9'){
			set_zd_double("0410",g_in_mst.bal);	
		}
	}
	
	/*add by dlc*/
	else if(dTx_num==16)
	{
		vtcp_log("%s,%d,网上银行企业账户详细信息查询请求(gw0303)",__FILE__,__LINE__);
			goto OkExit8;
		}
	/*add end*/
	/*add by dlc*/
	else if(dTx_num==17)
	{
		vtcp_log("%s,%d,子账户（卡）信息查询请求(gw1005)",__FILE__,__LINE__);
			goto OkExit9;
		}
	/*add end*/
	/*add by hzh*/
	else if(dTx_num==18)
	{
		vtcp_log("%s,%d,银企明细对账单详细信息查询(gw6013)",__FILE__,__LINE__);
			goto OkExit10;
		}
	/*add end*/
		/*add by dlc*/
	else if(dTx_num==19)
	{
		vtcp_log("%s,%d,代发工资单位信息查询(gw6003)",__FILE__,__LINE__);
			goto OkExit11;
		}
	/*add end*/
OkExit:
	vtcp_log("%s,%d,查询验证完毕!",__FILE__,__LINE__);
	if(fp != NULL)
	{
		fclose(fp);
	}
	strcpy(g_pub_tx.reply,"0000");
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return(0);
OkExit1:
	vtcp_log("%s,%d,查询储蓄账户信息，下个子交易spW002",__FILE__,__LINE__);
	strcpy(g_pub_tx.reply,"0001");
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return(0);
OkExit2:
	vtcp_log("%s,%d,查询储蓄账户历史信息!,下个子交易spW003",__FILE__,__LINE__);
	strcpy(g_pub_tx.reply,"0002");
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return(0);
OkExit3:
	vtcp_log("%s,%d,储蓄账户口挂!,下一个交易spD405",__FILE__,__LINE__);
	strcpy(g_pub_tx.reply,"0003");
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return(0);
OkExit4:
	vtcp_log("%s,%d,卡号信息查询!,下一个交易spW004",__FILE__,__LINE__);
	strcpy(g_pub_tx.reply,"0004");
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return(0);
OkExit5:
	vtcp_log("%s,%d,卡子账号信息查询!,下一个交易spW005",__FILE__,__LINE__);
	strcpy(g_pub_tx.reply,"0005");
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return(0);
OkExit6:
	vtcp_log("%s,%d,对公账户信息概览!,下一个交易spW006",__FILE__,__LINE__);
	strcpy(g_pub_tx.reply,"0006");
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return(0);
	/**add by lhg 20120307**/
OkExit7:
	vtcp_log("%s,%d,对公账户信息概览!,下一个交易spWNC1",__FILE__,__LINE__);
	strcpy(g_pub_tx.reply,"0007");
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return(0);
	/**add end**/
	/**add by dlc 20130603**/
OkExit8:
	vtcp_log("%s,%d,对公账户信息概览!,下一个交易spW008",__FILE__,__LINE__);
	strcpy(g_pub_tx.reply,"0008");
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return(0);
	/**add end**/
	/**add by dlc 20130618**/
OkExit9:
	vtcp_log("%s,%d,子账户（卡）信息查询!,下一个交易spW009",__FILE__,__LINE__);
	strcpy(g_pub_tx.reply,"0009");
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return(0);
	/**add by hzh 20130619**/
OkExit10:
	vtcp_log("%s,%d,银企明细对账单详细信息查询!,下一个交易spW010",__FILE__,__LINE__);
	strcpy(g_pub_tx.reply,"0010");
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return(0);
	/**add end**/
		/**add by dlc 20130808**/
OkExit11:
	vtcp_log("%s,%d,对公账户信息概览!,下一个交易spW011",__FILE__,__LINE__);
	strcpy(g_pub_tx.reply,"0011");
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return(0);
	/**add end**/
	/**add by dlc 20130808**/
ErrExit:
	if(fp != NULL)
	{
		fclose(fp);
	}
	vtcp_log("%s,%d,账户查询错误!",__FILE__,__LINE__);
	if(strlen(g_pub_tx.reply)==9 || g_pub_tx.reply[0]=='\0')
	{
		vtcp_log("%s,%d,网银查询错误!",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"Z913");
	}
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return(0);
}


/**小额交易状态转换**/
char *iNum2Stat(char ch, char *cTmpList)
{
    vtcp_log("[%s][%d]ch===[%c]\n", __FILE__, __LINE__, ch);
    
    if( ch == '1' ||ch == '3')
    {
    	strcpy(cTmpList, "3");  /**行内接收**/
    }
    else if(ch == '4' || ch == '7' ||ch == 'Q'||ch == 'R')
    {
    	strcpy(cTmpList, "3");  /**行内处理完成**/
    }
    else if (ch == 'I' ||ch == 'J')
    	{
    		strcpy(cTmpList, "3");  /**对方处理**/
    	}
    else if(ch == '6')
    {
    	strcpy(cTmpList, "0");  /**成功**/
    }
    else
    {
    	strcpy(cTmpList, "1");  /**退回 汇款失败**/
    }
    
    vtcp_log("[%s][%d]cTmpList==[%s]\n", __FILE__, __LINE__, cTmpList);
    return cTmpList;
}

/**大额交易状态**/
char *iNum2Stat1(char ch, char *cTmpList)
{
	vtcp_log("[%s][%d]ch===[%c]\n", __FILE__, __LINE__, ch);
	
	if(ch == '0' ||ch == 'C')
	{
		strcpy(cTmpList, "3");   /**行内接收**/
	}
	else if(ch == '1' || ch =='J')
	{
		strcpy(cTmpList, "3");   /**行内处理完成**/
	}
	else if(ch == '2'||ch == 'K')
	{
		strcpy(cTmpList, "0");  /**成功**/
	}
	else
	{
		strcpy(cTmpList, "1");   /**退回 汇款失败**/
	}
	vtcp_log("[%s][%d]cTmpList==[%s]\n", __FILE__, __LINE__, cTmpList);
	return cTmpList;
}

