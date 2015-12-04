/*************************************************
* 文 件 名:  sp8683.c
* 功能描述： 网银USBKey凭证使用处理
* 作    者:  Han Xichao
* 完成日期： 2010年10月27日
*
* 修改记录： 
* 	日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "dd_mst_c.h"
#include "mdm_attr_c.h"
#include "com_sys_parm_c.h"
#include "netbank_cif_ukey_rel_c.h"
#include "netbank_yqdz_c.h"

int sp8683()
{
	int    ret;
	char   strtmp[17];
	char   cTel[7];
	char   tmpcc[200];
	char   doFlag[1+1];
	char   isFree[1+1];
	struct mdm_attr_c mdm_attr_c;
	struct mdm_ac_rel_c v_mdm_ac_rel;
	struct dd_mst_c dd_mst;
	struct com_sys_parm_c com_sys_parm;
	struct netbank_cif_ukey_rel_c netbank_cif_ukey_rel;
	struct netbank_yqdz_c netbank_yqdz;
	
	memset(doFlag,0x00,sizeof(doFlag));
	memset(isFree,0x00,sizeof(isFree));
	memset(tmpcc,0x00,sizeof(tmpcc));
	memset(cTel, 0x00, sizeof(cTel));
	memset(&mdm_attr_c, 0x00, sizeof(struct mdm_attr_c));
	memset(&v_mdm_ac_rel, 0x00, sizeof(struct mdm_ac_rel_c));
	memset(&dd_mst,0x0,sizeof(struct dd_mst_c));
	memset(&com_sys_parm,0x0,sizeof(struct com_sys_parm_c));
	memset(&netbank_cif_ukey_rel,0x0,sizeof(struct netbank_cif_ukey_rel_c));
	memset(&netbank_yqdz,0x0,sizeof(struct netbank_yqdz_c));
	
	pub_base_sysinit();
	
	/* 前台取值 */
	get_zd_data("0670", doFlag);
	vtcp_log("[%s],[%d],传进来的操作标志位为[%s]\n",__FILE__,__LINE__,doFlag);
	get_zd_long("0860",&netbank_cif_ukey_rel.cif_no); /* 客户号 */
	vtcp_log("[%s],[%d],传进来的客户号为[%d]\n",__FILE__,__LINE__,netbank_cif_ukey_rel.cif_no);
	get_zd_data("0070", cTel);
	vtcp_log("[%s],[%d],传进来的柜员号码为[%s]\n",__FILE__,__LINE__,cTel);
  memset(tmpcc,0x00,sizeof(tmpcc));
	get_zd_data("0580",tmpcc);
	vtcp_log("%s,%d tmpcc=[%s]\n",__FILE__,__LINE__,tmpcc);
	get_zd_data("1161", g_pub_tx.mdm_code);
	sprintf(acErrMsg, "传进来的介质和介质属性为[%s]", g_pub_tx.mdm_code);
	WRITEMSG
	get_zd_data("1162", g_pub_tx.crd_no);
	sprintf(acErrMsg, "传进来的介质和介质属性为[%s]",g_pub_tx.crd_no);
	WRITEMSG
	sprintf(acErrMsg, "传进来的介质和介质属性为[%s][%s]", g_pub_tx.mdm_code, g_pub_tx.beg_note_no);
	WRITEMSG
	get_zd_data("0580", g_pub_tx.beg_note_no);
	get_zd_data("0580", netbank_cif_ukey_rel.ukey);
	sprintf(acErrMsg, "传进来的介质和介质属性为[%s]",g_pub_tx.crd_no);
	WRITEMSG
	sprintf(acErrMsg, "传进来的介质和介质属性为[%s][%s]", g_pub_tx.mdm_code, g_pub_tx.beg_note_no);
	WRITEMSG
	sprintf(acErrMsg, "传进来的介质和介质属性为[%s]",g_pub_tx.crd_no);
	WRITEMSG
	get_zd_data("0580", g_pub_tx.end_note_no);
	sprintf(acErrMsg, "传进来的介质和介质属性为[%s][%s]", g_pub_tx.mdm_code, g_pub_tx.beg_note_no);
	WRITEMSG

	memset(strtmp, 0, sizeof(strtmp));
	sprintf(acErrMsg, "传进来的介质和介质属性为[%s][%s]", g_pub_tx.mdm_code, g_pub_tx.beg_note_no);
	WRITEMSG
	strcpy(strtmp, g_pub_tx.beg_note_no);
	sprintf(acErrMsg, "传进来的介质和介质属性为[%s][%s]", g_pub_tx.mdm_code,strtmp);
	WRITEMSG
	sprintf(g_pub_tx.beg_note_no, "%016s", strtmp);

	sprintf(acErrMsg, "传进来的介质和介质属性为[%s][%s]", g_pub_tx.mdm_code, g_pub_tx.beg_note_no);
	WRITEMSG
		pub_base_strpack(g_pub_tx.beg_note_no);
	if (!strlen(g_pub_tx.beg_note_no))
		goto OkExit;

	sprintf(acErrMsg, "传进来的介质和介质属性为[%s][%s]", g_pub_tx.mdm_code, g_pub_tx.beg_note_no);
	WRITEMSG

	ret= Com_sys_parm_Sel(g_pub_tx.reply,&com_sys_parm,"1=1");
	if(ret)
	{
	vtcp_log("[%s][%d]得到系统日期错![%d]\n",__FILE__,__LINE__,ret);
	goto ErrExit;
	}
	vtcp_log("[%s][%d]交易日期：[%d]\n",__FILE__,__LINE__,com_sys_parm.sys_date);
	/*** 读取介质属性 ***/
		ret = Mdm_attr_Sel(&g_pub_tx.reply, &mdm_attr_c, " mdm_code='%s' ", \
				   g_pub_tx.mdm_code);
	if (ret == 100) {
		sprintf(acErrMsg, "该介质属性不存在![%d][%s]",
			g_reply_int, g_pub_tx.mdm_code);
		WRITEMSG
			strcpy(g_pub_tx.reply, "P132");
		goto ErrExit;
	} else if (ret) {
		sprintf(acErrMsg, "取介质属性异常![%d]", g_reply_int);
		WRITEMSG
			strcpy(g_pub_tx.reply, "P133");
		goto ErrExit;
	}
	vtcp_log("heheheehehehehehehehePQAAA[%s]", mdm_attr_c.note_type);
	strcpy(g_pub_tx.note_type, mdm_attr_c.note_type);
	strcpy(g_pub_tx.end_note_no, g_pub_tx.beg_note_no);

	vtcp_log("PQAAA[%s]", g_pub_tx.beg_note_no);
	TRACE
		sprintf(acErrMsg, "mdm_attr_c.note_type=[%s]", mdm_attr_c.note_type);
	WRITEMSG

	/* 需要凭卡号开户，如卡类：开户时需要输入卡号 */
		if (mdm_attr_c.no_ind[0] == 'Y') {
		if (!strlen(g_pub_tx.crd_no)) {
			sprintf(acErrMsg, "需要输入卡号");
			WRITEMSG
				strcpy(g_pub_tx.reply, "D111");
			goto ErrExit;
		}
		sprintf(acErrMsg, "输入的卡号为[%s]", g_pub_tx.crd_no);
		WRITEMSG

		/**检查卡号是否已经开过**/
			ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &v_mdm_ac_rel,
					     "ac_no='%s'", g_pub_tx.crd_no);
		vtcp_log("PQAAA[%s]ret[%d]", g_pub_tx.crd_no, ret);
		TRACE
			WRITEMSG
			if (ret && ret != 100)
			goto ErrExit;
		else if (!ret) {
			sprintf(acErrMsg, "需要输入卡号[%s],ret=[%d]", g_pub_tx.crd_no, ret);
			WRITEMSG
				strcpy(g_pub_tx.reply, "D113");
			goto ErrExit;
		}
		strcpy(g_pub_tx.ac_no, g_pub_tx.crd_no);

		vtcp_log("acno[%s]", g_pub_tx.ac_no);
		TRACE
			WRITEMSG

			pub_base_strpack(g_pub_tx.beg_note_no);

		vtcp_log("crd_no[%s]beg_note_no[%s]", g_pub_tx.crd_no, g_pub_tx.beg_note_no);
		TRACE
			WRITEMSG

			vtcp_log("strlen((g_pub_tx.crd_no)=[%d],strlen(g_pub_tx.beg_note_no)=[%d]", strlen(g_pub_tx.crd_no), strlen(g_pub_tx.beg_note_no));
		TRACE
			WRITEMSG
			if (strlen(g_pub_tx.crd_no) && !strlen(g_pub_tx.beg_note_no)) {
			/*** 根据卡号取凭证号 ***/
			pub_card_CardToNote(g_pub_tx.crd_no, g_pub_tx.beg_note_no);
			strcpy(g_pub_tx.end_note_no, g_pub_tx.beg_note_no);
			vtcp_log("PQAAA[%s]", g_pub_tx.end_note_no);
		}
		vtcp_log("PAAAA[%s]", g_pub_tx.end_note_no);
	}
	/* 需要凭证销号 */
	if (mdm_attr_c.only_ind[0] == 'Y' && strcmp(doFlag,"2")!=0 ) {
		vtcp_log("PBBBBBBBBBBBBB[%s]", g_pub_tx.note_type);
		vtcp_log("[%s][%d] Beg_no =[%s]", __FILE__,__LINE__,g_pub_tx.beg_note_no);
		ret = pub_com_ChkNoteMin(g_pub_tx.note_type, g_pub_tx.beg_note_no,
					 g_pub_tx.tel);
		if (ret) {
			if(memcmp(g_pub_tx.reply,"D104",4)==0){
				strcpy(g_pub_tx.reply,"N007");
			}
			goto ErrExit;
		}
	}
	
	/* 凭证处理 */
	if(strcmp(doFlag,"2")!=0 )/**若是解约网银暂时先不对凭证做处理，只更新网银签约表**/
	{
		ret=pub_com_ChkNoteUse(mdm_attr_c.note_type);
		if(ret)
		{
			sprintf(acErrMsg,"凭证[%s]处于停用状态",mdm_attr_c.note_type);
			WRITEMSG
			strcpy(g_pub_tx.reply,"N070");
			goto ErrExit;
		}
	}
	if(strcmp(doFlag,"0") == 0)
	{
		goto OkExit;
	}
	else
	{
		ret= Netbank_cif_ukey_rel_Sel(g_pub_tx.reply,&netbank_cif_ukey_rel,"cif_no='%ld' and ukey='%s'",netbank_cif_ukey_rel.cif_no,netbank_cif_ukey_rel.ukey);
		vtcp_log("[%s][%d]Netbank_cif_ukey_rel_Sel ret[%d]\n",__FILE__,__LINE__,ret);
		if(ret==100)
		{
			if(strcmp(doFlag,"2") == 0)
			{
			vtcp_log("[%s][%d]该用户并未签约，不能解约![%d]\n",__FILE__,__LINE__,ret);
			strcpy(g_pub_tx.reply,"EB01");
			goto ErrExit;
			}
			else if(strcmp(doFlag,"1") == 0) 
			{
			/**初次签约**/
			strcpy(netbank_cif_ukey_rel.sts,"0");
			netbank_cif_ukey_rel.sign_date=com_sys_parm.sys_date;
			ret = Netbank_cif_ukey_rel_Ins(netbank_cif_ukey_rel, g_pub_tx.reply);
			if (ret != 0)
			{
			sprintf(acErrMsg,"向网银签约表插入记录错误!![%d]",ret);
			WRITEMSG
			goto ErrExit;
			}
			vtcp_log("[%s][%d]登记网银签约表成功!\n",__FILE__,__LINE__);
			}
			else
			{
			strcpy(g_pub_tx.reply,"H037");
			vtcp_log("[%s][%d]操作类型错误!\n",__FILE__,__LINE__);
			goto ErrExit;
			}
		}
		else
		{
		/*if(strcmp(netbank_cif_ukey_rel.sts,"0") == 0 && strcmp(doFlag,"1") == 0)
			{
				strcpy(g_pub_tx.reply,"EB02");
				vtcp_log("[%s][%d]已经签约用户，不能再做签约操作![%d]\n",__FILE__,__LINE__,ret);
				goto ErrExit;
			}
			else*/ if(strcmp(netbank_cif_ukey_rel.sts,"1") == 0 && strcmp(doFlag,"2") == 0)
			{
				strcpy(g_pub_tx.reply,"EB03");
				vtcp_log("[%s][%d]该用户已经解约，无需再做解约![%d]\n",__FILE__,__LINE__,ret);
				goto ErrExit;
			}
			else if(strcmp(netbank_cif_ukey_rel.sts,"0") == 0 && strcmp(doFlag,"2") == 0)
			{
			/**对已经签约用户做解约操作**/
				ret = Netbank_cif_ukey_rel_Dec_Upd(g_pub_tx.reply," cif_no=%ld and ukey='%s'", netbank_cif_ukey_rel.cif_no,netbank_cif_ukey_rel.ukey);
				if (ret != 0)
				{
						sprintf(acErrMsg,"Declare error [%d]",ret);
						WRITEMSG
						goto ErrExit;
				}
				memset(&netbank_cif_ukey_rel,0x0,sizeof(struct netbank_cif_ukey_rel_c));
				ret = Netbank_cif_ukey_rel_Fet_Upd(&netbank_cif_ukey_rel,g_pub_tx.reply);
				if (ret != 0)
				{
						sprintf(acErrMsg,"Fetch error [%d] ",ret);
						WRITEMSG
						Netbank_cif_ukey_rel_Clo_Upd();
						goto ErrExit;
				}
				
				strcpy(netbank_cif_ukey_rel.sts,"1");
				netbank_cif_ukey_rel.cancel_date=com_sys_parm.sys_date;
				ret = Netbank_cif_ukey_rel_Upd_Upd(netbank_cif_ukey_rel, g_pub_tx.reply);
				if (ret != 0)
				{
						sprintf(acErrMsg,"Update error [%d] ",ret);
						WRITEMSG
						Netbank_cif_ukey_rel_Clo_Upd();
						goto ErrExit;
				}
				Netbank_cif_ukey_rel_Clo_Upd();
				/**判断若该客户能正常使用的ukey数目为零时,自动解约网银银企对账**/
				ret=sql_count("netbank_cif_ukey_rel","cif_no=%ld and sts='0'",netbank_cif_ukey_rel.cif_no);
					if(ret<0)
					{
						sprintf(acErrMsg,"sql_count error!! [%d]",ret);
						WRITEMSG
						goto ErrExit;
					}
					else if(ret==0) 
					{
					/**该用户已经没有正常状态ukey,判断其已经解约了网银,自动解约其网银银企对账**/
						ret=Dd_mst_Dec_Sel(g_pub_tx.reply,"cif_no=%ld ",netbank_cif_ukey_rel.cif_no);
						if(ret)
						{
							sprintf( acErrMsg, "sql error" );
							WRITEMSG
							set_zd_data("0130","ERROR[SQL执行失败]");/*表示程序执行失败*/
							goto ErrExit;
						}
						while(1)
						{
							ret=Dd_mst_Fet_Sel(&dd_mst,g_pub_tx.reply);
							if(ret)
								{
								if(ret==100)
									{
									break;
									}
									else
									{
									sprintf(acErrMsg,"sql error");
									WRITEMSG
									set_zd_data("0130","ERROR[SQL执行失败]");/*表示程序执行失败*/
									Dd_mst_Clo_Sel();
									goto ErrExit;
									}
								}
								vtcp_log("[%s][%d]预检验签约银企对账的账户ID为:[%ld]!\n",__FILE__,__LINE__,dd_mst.ac_id);
							ret= Netbank_yqdz_Sel(g_pub_tx.reply,&netbank_yqdz,"ac_id='%ld' and sts='0'",dd_mst.ac_id);
							if(ret==100)
								{
									/**没有签约则直接跳出，无需解约**/
									vtcp_log("[%s][%d]账户ID为:[%ld]的账号并未签网银银企对账，无需解约\n",__FILE__,__LINE__,dd_mst.ac_id);
									break;
								}
							else
								{
									/**对已经签约用户做解约操作**/
									ret = Netbank_yqdz_Dec_Upd(g_pub_tx.reply," ac_id=%ld ", dd_mst.ac_id);
									if (ret != 0)
									{
										sprintf(acErrMsg,"Declare error [%d]",ret);
										WRITEMSG
										goto ErrExit;
									}
									memset(&netbank_yqdz,0x0,sizeof(struct netbank_yqdz_c));
									ret = Netbank_yqdz_Fet_Upd(&netbank_yqdz,g_pub_tx.reply);
									if (ret != 0)
									{
										sprintf(acErrMsg,"Fetch error [%d] ",ret);
										WRITEMSG
										Netbank_yqdz_Clo_Upd();
										goto ErrExit;
									}
									
									strcpy(netbank_yqdz.sts,"1");
									netbank_yqdz.cancel_date=com_sys_parm.sys_date;
									ret = Netbank_yqdz_Upd_Upd(netbank_yqdz, g_pub_tx.reply);
									if (ret != 0)
									{
										sprintf(acErrMsg,"Update error [%d] ",ret);
										WRITEMSG
										Netbank_yqdz_Clo_Upd();
										goto ErrExit;
									}
									Netbank_yqdz_Clo_Upd();
								}
						}
						
					}
					else
					{
					vtcp_log("[%s][%d]还有可以使用的ukey:[%d]个,不用自动解约网银银企对账!\n",__FILE__,__LINE__,ret);
					}
			}
			else if(strcmp(netbank_cif_ukey_rel.sts,"1") == 0 && strcmp(doFlag,"1") == 0)
			{
				vtcp_log("[%s][%d]对已经解约用户做重新签约操作1\n",__FILE__,__LINE__);
				/**对已经解约用户做重新签约操作**/
				ret = Netbank_cif_ukey_rel_Dec_Upd(g_pub_tx.reply," cif_no=%ld ", netbank_cif_ukey_rel.cif_no);
				if (ret != 0)
				{
						sprintf(acErrMsg,"Declare error [%d]",ret);
						WRITEMSG
						goto ErrExit;
				}
				memset(&netbank_cif_ukey_rel,0x0,sizeof(struct netbank_cif_ukey_rel_c));
				ret = Netbank_cif_ukey_rel_Fet_Upd(&netbank_cif_ukey_rel,g_pub_tx.reply);
				if (ret != 0)
				{
						sprintf(acErrMsg,"Fetch error [%d] ",ret);
						WRITEMSG
						Netbank_cif_ukey_rel_Clo_Upd();
						goto ErrExit;
				}
				strcpy(netbank_cif_ukey_rel.sts,"0");
				netbank_cif_ukey_rel.sign_date=com_sys_parm.sys_date;
				ret = Netbank_cif_ukey_rel_Upd_Upd(netbank_cif_ukey_rel, g_pub_tx.reply);
				if (ret != 0)
				{
						sprintf(acErrMsg,"Update error [%d] ",ret);
						WRITEMSG
						Netbank_cif_ukey_rel_Clo_Upd();
						goto ErrExit;
				}
				Netbank_cif_ukey_rel_Clo_Upd();
			}
			if(strcmp(doFlag,"1") != 0 && strcmp(doFlag,"2") != 0)
			{
			strcpy(g_pub_tx.reply,"H037");
			vtcp_log("[%s][%d]操作类型错误!\n",__FILE__,__LINE__);
			goto ErrExit;
			}
		}
	}
	if(strcmp(doFlag,"1") == 0)
	{
		ret=pub_com_NoteUse(0,0,mdm_attr_c.note_type,g_pub_tx.beg_note_no,g_pub_tx.beg_note_no,cTel);
		if(ret)
		{
			sprintf(acErrMsg,"柜员[%s]使用凭证[%s]错误",cTel,g_pub_tx.beg_note_no);
			WRITEMSG
			strcpy(g_pub_tx.reply,"N005");
			goto ErrExit;
		}
	}
OkExit:
	strcpy(g_pub_tx.reply, "0000");
	sprintf(acErrMsg, "Before OK return: reply is[%s]", g_pub_tx.reply);
	WRITEMSG
		set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg, "Before return: reply is[%s]", g_pub_tx.reply);
	WRITEMSG
		set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 1;
}
