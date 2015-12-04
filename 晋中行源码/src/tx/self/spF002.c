/*************************************************
* 文 件 名:  spF002.c
* 功能描述： ATM 查询余额
*
* 作	者:  rob
* 完成日期： 2003年2月17日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "chnl_self_trade_c.h"
#include "dd_mst_hst_c.h"
#include "card_reg_c.h"
#include "com_branch_c.h"
#include "td_parm_c.h"
#include "hv_brno_orno_c.h"
#include "card.h"
#include "lv_define.h"
#define  JWQURY_FEE   2.00 /*境外查询行内收费金额*/
#define  KHJW_FEE    4.00 /*境外查询行内收费金额*/
#define BH_SQATMFEE_KM   "5111601"   /*本行收取ATM手续费科目 Mod by hxc 101218 修改银联卡查询手续费收入科目 5111203->5111601*/
#define BH_DFFEE_KM   "52704"     /*本行垫付手续费科目 add by lzy 150109 为境外ATM查询垫付增加*/

int spF002()
{

	int ret=0,i=0,j=0;
	long pass_cnt=0;
	int totnum=0;
	long Beg_line=0;
	double limit_amt =0.0, today_amt = 0.0, account_amt = 0.0, available_amt=0.0;
	double ky_amt=0.0;
	double dJw_fee=0.0;
	char s_txamt[17],prdt_name[6],flag[2],mx_tmp[45];
	char fd95[483],brf6[13],to_str[13];
	struct mdm_ac_rel_c sMdm_ac_rel;
	struct dd_mst_c sDd_mst;
	struct dd_mst_hst_c sDd_mst_hst;
	struct td_mst_c sTd_mst;
	struct td_parm_c sTd_parm;	
	struct com_branch_c zcom_branch;
	char cAcno_qs[25];
	char zAc_no[25];
	char fee_mode[2];
	char tmpstr[41];
	char str_brf[51];
	char sPrdt_no_dd[4];/****活期产品号****/

	memset(prdt_name,0,sizeof(prdt_name));
	memset(sPrdt_no_dd,0,sizeof(sPrdt_no_dd));
	memset(s_txamt,0,sizeof(s_txamt));
	memset(&sDd_mst_hst,0,sizeof(struct dd_mst_hst_c));
	memset(&sTd_mst,0,sizeof(struct td_mst_c));
	memset(&sTd_parm,0,sizeof(struct td_parm_c));
	memset(&sMdm_ac_rel,0,sizeof(struct mdm_ac_rel_c));
	memset(&zcom_branch,0,sizeof(struct com_branch_c));
	memset(flag,0,sizeof(flag));
	memset(fd95,'\0',sizeof(fd95));
	memset(mx_tmp,0,sizeof(mx_tmp));
	memset(tmpstr,0,sizeof(tmpstr));
	memset(str_brf,0,sizeof(str_brf));
	memset(brf6,0,sizeof(brf6));
	memset(to_str,0,sizeof(to_str));
	/*	
	account_amt   ==  账面余额
	available_amt ==  当日可用余额
	*/
	struct card_reg_c sCard_reg;
	memset(cAcno_qs, 0 , sizeof(cAcno_qs));
	memset(zAc_no, 0x00, sizeof(zAc_no));
	memset(fee_mode, 0 , sizeof(fee_mode));
	memset(&sCard_reg,0,sizeof(sCard_reg));

	if (pub_base_sysinit())
	{
		sprintf(acErrMsg,"初始化公用结构错误!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"CU06");
		goto ErrExit;
	}
	vtcp_log("%s,%d,Gethere1\n",__FILE__,__LINE__);
	get_zd_data("0300",g_pub_tx.crd_no);   /* 卡号 */
	get_zd_data("0790",g_pub_tx.draw_pwd); /* 支取密码  */
	get_zd_data("0690",fee_mode);	       /*费用模式*/
	get_zd_data("0810",str_brf);           /*摘要*/
	get_zd_data("0710",flag);              /*通过71域来区分查询类型:0,活期余额查询;1.活期明细查询;2.多账户查询*/
	sprintf(acErrMsg,"tx_date=[%ld],ac_no=[%s]", g_pub_tx.tx_date, g_pub_tx.crd_no);
	WRITEMSG
	vtcp_log("%s,%d,CEHSI,fee_mode[%s]",__FILE__,__LINE__,fee_mode);
	vtcp_log("%s,%d,g_pub_tx.crd_no=[%s]\n",__FILE__,__LINE__,g_pub_tx.crd_no);
	/**Add by SSH,20100427,简化对公查询处理**/
	if(g_pub_tx.crd_no[0]=='5'){
		struct hv_brno_orno_c hv_brno_orno;
		memset(&hv_brno_orno,'\0',sizeof(hv_brno_orno));
		ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_no='%s' and ac_seqn=1",	g_pub_tx.crd_no);
		if(ret){
			sprintf(acErrMsg,"未找到帐户[%s]",g_pub_tx.crd_no);
			strncpy(g_pub_tx.reply,"CU14",4);
			WRITEMSG
			vtcp_log("%s,%d 未找到帐户[%s][%d]",__FILE__,__LINE__,g_pub_tx.crd_no,g_reply_int);
			goto ErrExit;
			
		}
		/***ADD BY ZHGF 20100610 增加返回机构名称**/
		/****为保险起见，此处还是用Com_branch_Sel吧
		ret=pub_base_get_br_parm(sMdm_ac_rel.opn_br_no,&zcom_branch);
		if(ret){
			sprintf(acErrMsg,"取开户机构信息错误!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"D172");
			goto ErrExit;
		}
		****/
		ret=Com_branch_Sel(g_pub_tx.reply,&zcom_branch,"br_no = '%s' ",sMdm_ac_rel.opn_br_no);
		if(ret==100)
		{
			sprintf(acErrMsg,"公共机构表中无此机构!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"O127");
			goto ErrExit;
		}
		else if(ret)
		{
			sprintf(acErrMsg,"取开户机构信息错误!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"D172");
			goto ErrExit;
		}
		set_zd_data("0760",zcom_branch.br_name);
		/***ADD BY ZHGF 20100610 增加返回机构名称**/
		ret=Dd_mst_Sel(g_pub_tx.reply,&sDd_mst,"ac_id=%ld and ac_seqn=1",	sMdm_ac_rel.ac_id);
		if(ret){
			sprintf(acErrMsg,"未找到帐户[%s]",g_pub_tx.crd_no);
			strncpy(g_pub_tx.reply,"CU14",4);
			WRITEMSG
			vtcp_log("%s,%d 未找到帐户[%s][%d]",__FILE__,__LINE__,g_pub_tx.crd_no,g_reply_int);
			goto ErrExit;
			
		}
		/****晋中增加产品号的控制,若为柜面通取款,不允许从保证金户取款 20100703 begin****/
		memcpy(sPrdt_no_dd,sDd_mst.prdt_no,sizeof(sPrdt_no_dd));
		if(memcmp(sPrdt_no_dd,"131",3)==0||memcmp(sPrdt_no_dd,"132",3)==0||memcmp(sPrdt_no_dd,"134",3)==0||memcmp(sPrdt_no_dd,"142",3)==0||memcmp(sPrdt_no_dd,"144",3)==0){
			sprintf(acErrMsg,"此为商行保证金户,不允许用柜面通取款!产品号==[%s]",sPrdt_no_dd);
			strncpy(g_pub_tx.reply,"P471",4);
			WRITEMSG
			goto ErrExit;
		}
		/****20100703 end****/
		set_zd_data("0250",sDd_mst.name);	
		set_zd_long("0260",sDd_mst.cif_no);	
		set_zd_data("0270",sMdm_ac_rel.id_no);	
		ret=Hv_brno_orno_Sel(g_pub_tx.reply,&hv_brno_orno,"br_no='%s'",sDd_mst.opn_br_no);
		if(ret){
			set_zd_data("0320",LV_QSBRNO);
		}else{
			set_zd_data("0320",hv_brno_orno.or_br_no);
		}
		set_zd_data("0330",sDd_mst.opn_br_no);
		set_zd_data("0910",sDd_mst.opn_br_no);/**20100610**/
		/****因为融信村镇银行要从这儿取得在商行所开大小额清算户的余额 所以返回之 begin 20100713****/
		available_amt=sDd_mst.bal-sDd_mst.hold_amt-sDd_mst.ctl_amt;
		account_amt=sDd_mst.bal;
		
		/*组82域*/
		memcpy(tmpstr, "1001156C", 8);
		sprintf(tmpstr+8, "%012.0f", account_amt*100);
		memcpy(tmpstr+20, "1002156C", 8);
		sprintf(tmpstr+28, "%012.0f", available_amt*100);
		vtcp_log("%s,%d,tmpstr[%s]",__FILE__,__LINE__,tmpstr);
		set_zd_data("0820", tmpstr);
		/****因为融信村镇银行要从这儿取得在商行所开大小额清算户的余额 所以返回之 end 20100713****/
		goto OkExit;
	}
	
	/***********add by luowei at 20090815**********/
	/****增加对折的支持****/
	/****/
		ret=Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdm_ac_rel, " ac_no='%s' ", g_pub_tx.crd_no);
		if(ret)
		{
			sprintf(acErrMsg,"%s,%d,Mdm_ac_rel_Sel数据库操作失败[%d]",__FILE__,__LINE__,ret);
			WRITEMSG
			strcpy(g_pub_tx.reply, "P102");
			goto ErrExit;
		}
		/***ADD BY ZHGF 20100610 增加返回机构名称**/
		/****
		ret=pub_base_get_br_parm(sMdm_ac_rel.opn_br_no,&zcom_branch);
		if(ret){
			sprintf(acErrMsg,"取开户机构信息错误!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"D172");
			goto ErrExit;
		}
		****/
		ret=Com_branch_Sel(g_pub_tx.reply,&zcom_branch,"br_no = '%s' ",sMdm_ac_rel.opn_br_no);
		if(ret==100)
		{
			sprintf(acErrMsg,"公共机构表中无此机构!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"O127");
			goto ErrExit;
		}
		else if(ret)
		{
			sprintf(acErrMsg,"取开户机构信息错误!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"D172");
			goto ErrExit;
		}
		set_zd_data("0760",zcom_branch.br_name);
               /***ADD BY ZHGF 20100610 增加返回机构名称**/

		vtcp_log("%s,%d br_name=[%s],opn_br_no=[%s]\n",__FILE__,__LINE__,zcom_branch.br_name,sMdm_ac_rel.opn_br_no);
		set_zd_data("0760",zcom_branch.br_name);
		set_zd_data("0910",sMdm_ac_rel.opn_br_no);
		/***ADD BY ZHGF 20100610 增加返回机构名称**/
		set_zd_data("0920",sMdm_ac_rel.note_sts); /**增加返回账户凭证状态 add by hxc 100813**/
		vtcp_log("%s,%d,账户凭证状态[%s]!",__FILE__,__LINE__,sMdm_ac_rel.note_sts);
		if((memcmp(g_pub_tx.crd_no, CARDHEADCODE, 6) != 0) && (memcmp(g_pub_tx.crd_no, ICCARDHEADCODE, 6) != 0) )
		{
		ret=Dd_mst_Sel(g_pub_tx.reply, &sDd_mst, "ac_id='%d'", sMdm_ac_rel.ac_id);
		if(ret)
		{
			sprintf(acErrMsg,"%s,%d,Dd_mst_Sel数据库操作失败[%d]",__FILE__,__LINE__,ret);
			WRITEMSG
			strcpy(g_pub_tx.reply, "P102");
			goto ErrExit;
		}
		/************密码验证***20100527 非卡时校验密码 MODIFY BY ZHGF***********/
		if((memcmp(g_pub_tx.crd_no, CARDHEADCODE, 6) != 0) && (memcmp(g_pub_tx.crd_no, ICCARDHEADCODE, 6) != 0))
		{
			/***20100618 验证密码要写错误文件的 RM BY ZHGF
			ret=pub_base_DesChk(g_pub_tx.tx_date, g_pub_tx.crd_no,g_pub_tx.draw_pwd,sMdm_ac_rel.draw_pwd);
			if(ret)
			{
				sprintf(acErrMsg,"%s,%d,密码错误",__FILE__,__LINE__);
				WRITEMSG
				strcpy(g_pub_tx.reply,"AT04");
				goto ErrExit;
			}
			***********/
			/**2010/7/7 14:22 MODIFY BY ZHGF**/
			/*vtcp_log("%s,%d g_pub_tx.draw_pwd=[%s]\n",__FILE__,__LINE__,g_pub_tx.draw_pwd);*/
			if(strlen(g_pub_tx.draw_pwd)){
				strcpy(g_pub_tx.id_type,sMdm_ac_rel.id_type);
				strcpy(g_pub_tx.id_no,sMdm_ac_rel.id_no);
				strcpy(g_pub_tx.ac_no,g_pub_tx.crd_no);
				ret = pub_base_check_draw("101");
				if(ret){
					goto ErrExit;
				}
			}
			/**2010/7/7 14:22 MODIFY BY ZHGF**/
		}
		/*********************************/
		available_amt=sDd_mst.bal-sDd_mst.hold_amt-sDd_mst.ctl_amt;
		account_amt=sDd_mst.bal;
		/*组82域*/
		vtcp_log("%s,%d,户名[%s]!",__FILE__,__LINE__,sDd_mst.name);
		set_zd_data("0250",sDd_mst.name);
		memcpy(tmpstr, "1001156C", 8);
		sprintf(tmpstr+8, "%012.0f", account_amt*100);
		memcpy(tmpstr+20, "1002156C", 8);
		sprintf(tmpstr+28, "%012.0f", available_amt*100);
		vtcp_log("%s,%d,tmpstr[%s]",__FILE__,__LINE__,tmpstr);
		set_zd_data("0820", tmpstr);
		strcpy(g_pub_tx.reply, "0000");
		goto OkExit;
	}
	/***********end by luowei at 20090815**********/
	/* 检查卡的状态合法性:是否挂失、到期等 
	if ( pub_base_CheckCrdNo() )
	{
		sprintf(acErrMsg,"检查卡的合法性错误!!");
		WRITEMSG
		if(strlen(g_pub_tx.reply) ==0 || memcmp(g_pub_tx.reply,"0000",4)==0){
			strcpy(g_pub_tx.reply,"CU14");
		}
		strcpy(g_pub_tx.reply,"P019");
		goto ErrExit;
	}
*********************/
	/* 检查凭证的状态  
	if (g_mdm_ac_rel.note_sts[0] != '0') 
	{ 
		sprintf(acErrMsg, "该卡为无效卡!! "); 
		WRITEMSG 
		if (g_mdm_ac_rel.note_sts[0] == '1' || g_mdm_ac_rel.note_sts[0] == '2') 
		{ 
			strcpy(g_pub_tx.reply, "CU41"); 
		} 
		else 
		{ 
			strcpy(g_pub_tx.reply, "CU41"); 
		} 
		goto ErrExit; 
	}****/
	g_reply_int=pub_base_currentdis_file(g_pub_tx.crd_no,1,&sDd_mst);
	if(g_reply_int)
	{
	       vtcp_log("%s,%d,账号不存在!",__FILE__,__LINE__);
	       goto ErrExit;
	}
	/** 若账户状态不正常不允许查询 **/
	if(sDd_mst.hold_sts[0] != '0' && sDd_mst.hold_sts[0] !='3')
	{
	       vtcp_log("%s,%d,账户非正常状态!",__FILE__,__LINE__);
	       strcpy(g_pub_tx.reply,"CU62");
	       set_zd_data(DC_GET_MSG,"账户冻结状态不能查询!");
	       goto ErrExit;
	}
	set_zd_data("0930",sDd_mst.hold_sts); /**增加返回账户冻结状态 add by hxc 100813**/
	vtcp_log("%s,%d,冻结状态[%s]!",__FILE__,__LINE__,sDd_mst.hold_sts);
	char cTradetype[4];/* 设别类型 */
	int iSelflag=0;  /*清算组织标志 */
	int atm_qy_free_flag=0;/* atm查询免费标志 0免费 1收费 */
	int pos_qy_free_flag=0;/* pos 查询免费标志 0免费 1收费 */
	int pos_tx_free_flag=0;/* pos 交易免费标志 0免费 1收费 */
	int atm_tx_free_flag=0;/* atm 交易免费标志 0免费 1收费 */
	int iDscpt=0;/* 现转查询标志 */
	iSelflag=0;  /*清算组织标志 */						
	atm_qy_free_flag=0;/* atm查询免费标志 0免费 1收费 */  
	pos_qy_free_flag=0;/* pos 查询免费标志 0免费 1收费 */ 
	pos_tx_free_flag=0;/* pos 交易免费标志 0免费 1收费 */ 
	atm_tx_free_flag=0;/* atm 交易免费标志 0免费 1收费 */ 
	iDscpt=0;/* 现转查询标志 */						   
	memset(cTradetype, 0 , sizeof(cTradetype));
	/*modified by wyw 20080312 
		根据收费标准来判断交易来源,交易类型, 内外行标志*/
	iDscpt = CARD_TRADE_QURY;
	if(fee_mode[0] == '0')/*费率模式*/
	{
		memcpy(cTradetype ,"POS",3);
	}else if(fee_mode[0] == '6')
	{
		memcpy(cTradetype ,"GMM",3);
	}else{
		memcpy(cTradetype,"ATM",3);
	}
	if(fee_mode[0] == '1'||fee_mode[0] =='2')
	{
		iSelflag = 0; /*本行ATM,或本行POS*/
	}else{
		iSelflag = 1; /*他行ATM或他行POS*/
	}	
	g_reply_int=pub_card_reg(g_pub_tx.tx_amt1,cTradetype,'0',&atm_qy_free_flag,
		&pos_qy_free_flag,&pos_tx_free_flag,&atm_tx_free_flag,iSelflag,iDscpt);
	if(g_reply_int)
	{
		sprintf(acErrMsg,"登记卡登记簿错误");
		strncpy(g_pub_tx.reply,"AT06",4);
		WRITEMSG
		vtcp_log("%s,%d 处理卡交易登记簿错误 ret=[%d]",__FILE__,__LINE__,g_reply_int);
		goto ErrExit;
	}
	/* 查询密码输错次数控制 */
	/*ret = pub_base_GetParm_long("ATMMM",1,&pass_cnt); */
	/* 检查支取方式的合法性 
	ret = pub_base_DesChk(g_pub_tx.tx_date, g_pub_tx.crd_no,
					  g_pub_tx.draw_pwd,g_mdm_ac_rel.draw_pwd);
	*/
	strcpy(g_pub_tx.ac_no,g_pub_tx.crd_no);
     /****/  ret = pub_base_GetParm_long("KFCAR",2,&pass_cnt);
       if(ret)
       {
               pass_cnt = 3;
               vtcp_log("%s,%d,密码次数不存在!",__FILE__,__LINE__);
               vtcp_log("%s,%d,密码次数不存在,取默认次数[%d]!",__FILE__,__LINE__,pass_cnt);
       }
/*************20090729*****/
	ret = Card_reg_Sel(g_pub_tx.reply,&sCard_reg,"ac_no='%s' and l_tx_date=%ld ",g_pub_tx.crd_no,g_pub_tx.tx_date);	
	if(ret)
	{
		vtcp_log("%s,%d,卡号不存在",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"CU14");
		goto ErrExit;
	}
	vtcp_log("%s,%d,fee_ind[%s],type[%s]",__FILE__,__LINE__,fee_mode,cTradetype);
	if(memcmp(cTradetype,"ATM",3)==0)
	{
		if( g_pub_tx.tx_date == sCard_reg.l_tx_date && sCard_reg.add_pswd_cnt+1 >pass_cnt)
		{
			vtcp_log("%s,%d,超过ATM的当日可取款次数!,pass_cnt:[%d]",__FILE__,__LINE__,pass_cnt);
			strcpy(g_pub_tx.reply,"CU75");
			/*  这里return (-1)  12域还是0000.ATM界面返回的是余额为0      090602 LOONG
			return(-1);
			*/
			goto ErrExit;
		}
	}
/*****************20090728
	ret=pub_base_check_draw("101");
	if (ret != 0)
	{
		sprintf(acErrMsg,"卡的支取密码错误!!!");
		WRITEMSG
		ret = Card_reg_Dec_Upd(g_pub_tx.reply,"ac_no='%s' ",g_pub_tx.crd_no); 
		if (ret != 0) 
		{
			sprintf(acErrMsg,"Declare error!! [%d]",ret);
			WRITEMSG
			goto ErrExit;
		}
		sprintf(acErrMsg,"tx_date=[%ld],   ac_no=[%s]", g_pub_tx.tx_date,
				g_pub_tx.crd_no);
		WRITEMSG

		ret = Card_reg_Fet_Upd(&sCard_reg,g_pub_tx.reply);
		if (ret != 0) 
		{ 
			sprintf(acErrMsg,"Fetch error !! [%d]",ret);
			WRITEMSG
			goto ErrExit;
		}

		* 更新交易记录累计控制表 *
		if(sCard_reg.l_tx_date==g_pub_tx.tx_date)
		{
			sCard_reg.add_pswd_cnt = sCard_reg.add_pswd_cnt + 1;
		}else{
			sCard_reg.add_pswd_cnt=1;
		}
		sCard_reg.l_tx_date = g_pub_tx.tx_date;
		if (sCard_reg.add_pswd_cnt > pass_cnt)
		{
			* 修改介质账号对照表 
			* atm吞卡响应码 *
			ret  = sql_execute2("update mdm_ac_rel set coll_sts='2' where \
				ac_no = '%s' ", g_pub_tx.crd_no);
			if (ret != 0)
			{
				sprintf(acErrMsg, "更新mdm_ac_rel表错误!! [%d]", ret);
				WRITEMSG
				strcpy(g_pub_tx.reply, "AT08");
				goto ErrExit;
			}
			chnl_tradeadd.add_pswd_cnt = 0;*
	
			strcpy(g_pub_tx.reply,"CU75");
		}
		else
		{
			* 密码错误 *
			strcpy(g_pub_tx.reply,"CU55");
		}
		* 更新控制表 *
		vtcp_log("%s,%d,开始更新卡登记簿!",__FILE__,__LINE__);
		ret = Card_reg_Upd_Upd(sCard_reg,g_pub_tx.reply);
		if (ret != 0)
		{
			sprintf(acErrMsg,"更新交易记录累计控制表错误!! [%d]",ret);
			WRITEMSG
			goto ErrExit;
		}
		Card_reg_Clo_Upd();

		ret = sql_commit();
		if (ret != 0)
		{
			sprintf(acErrMsg,"提交事务错误!!");
			WRITEMSG
		}
		vtcp_log("%s,%d,提交更新错误密码次数!pwd_cnt[%d]",__FILE__,__LINE__,sCard_reg.add_pswd_cnt);
		ret = sql_begin();
		if (ret != 0)
		{
			sprintf(acErrMsg,"开始事务错误!!");
			WRITEMSG
		}

		goto ErrExit;
	}
**************************/
	/* 更新交易纪录累计控制表中密码连续输错次数为0 */
	if(sCard_reg.add_pswd_cnt > pass_cnt && memcmp(cTradetype,"ATM",3)==0)
	{
		vtcp_log("%s,%d,卡号不存在",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"CU38");
		goto ErrExit;
	}
	sCard_reg.add_pswd_cnt = 0;
	ret = sql_execute2("update card_reg set add_pswd_cnt=0 where \
			l_tx_date=%ld and ac_no = '%s'", 
			g_pub_tx.tx_date, g_pub_tx.crd_no);
	if (ret != 0)
	{
		sprintf(acErrMsg,"更新交易记录累计控制表错误!! [%d]",ret);
		WRITEMSG
		goto ErrExit;
	}
	strcpy(g_pub_tx.cur_no,"01");
	/*begin add by LiuHuafeng 20070203 */
	/* 检查2磁道cvn信息20090728 
	ret=pubf_card_chk_msr(3,g_pub_tx.crd_no);
	if (ret != 0)
	{
		sprintf(acErrMsg,"查询卡帐户主文件错误!!!");
		WRITEMSG
		goto ErrExit;
	}*******************/
	/* end by LiuHuafeng 20070203 */
	/*通过71域来区分是活期，明细，多账户查询*/
	/* 查询活期帐户主文件 */
	ret = pub_base_CurGetddFile(g_pub_tx.crd_no,g_pub_tx.cur_no,&g_dd_mst);
	if (ret != 0)
	{
		sprintf(acErrMsg,"查询活期帐户主文件错误!!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"AT06");
		goto ErrExit;
	}
	/** 若账户状态不正常不允许查询 **/
	if(g_dd_mst.hold_sts[0] != '0' && g_dd_mst.hold_sts[0] !='3')
	{
		vtcp_log("%s,%d,账户非正常状态!",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"CU62");
		set_zd_data(DC_GET_MSG,"账户冻结状态不能查询!");
		goto ErrExit;
	}
	vtcp_log("%s,%d,户名[%s]!",__FILE__,__LINE__,g_dd_mst.name);
	set_zd_data("0250",g_dd_mst.name);

	if(flag[0]=='1')
	{
		/*查询活期明细*/	
		get_zd_long("0540",&Beg_line); /*起始行数 */
		if(Beg_line=1)
		{
			int Count=0;
			Count=sql_count("dd_mst","ac_id=%ld and ac_seqn=%ld",g_dd_mst.ac_id,g_dd_mst.ac_seqn);
			set_zd_long("0550",Count);
		}			
		ret = Dd_mst_hst_Dec_Sel(g_pub_tx.reply,"ac_id =%ld and ac_seqn=%ld order by hst_cnt desc",g_dd_mst.ac_id,g_dd_mst.ac_seqn);    
		if(ret)
		{
			vtcp_log("%s,%d,查找明细出错！",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"CU14");
			goto ErrExit;
		}
		i=0,totnum=0;
		while(1)
		{
			vtcp_log("%s,%d,开始查询帐户明细记录!",__FILE__,__LINE__);
			if( i > 9)
			{
				vtcp_log("%s,%d,已经取完10条明细",__FILE__,__LINE__);
				break;		
			}
			ret = Dd_mst_hst_Fet_Sel(&sDd_mst_hst,g_pub_tx.reply);
			if(ret && ret !=100)
			{
				vtcp_log("%s,%d,查找明细出错！",__FILE__,__LINE__);
				Dd_mst_hst_Clo_Sel();
				strcpy(g_pub_tx.reply,"CU14");
				goto ErrExit;
			}else if(ret == 100)
			{
				vtcp_log("%s,%d,找到的记录数为[%d]",__FILE__,__LINE__,i);
				set_zd_data("0660","1"); /* 结束标志 */
				break;
			}
			totnum++;	
			if(totnum<Beg_line)
			{
				continue; /*不到行数不写文件*/
			}
			memset(mx_tmp,0,sizeof(mx_tmp));
			memset(brf6,0,sizeof(brf6));
			memcpy(brf6,sDd_mst_hst.brf,12);
			Zip_halfChinese(brf6,to_str);
								
			sprintf(mx_tmp,"%08d%-12.12s%014.0lf%014.0lf",sDd_mst_hst.tx_date,to_str,sDd_mst_hst.tx_amt*100,sDd_mst_hst.bal*100);
			memcpy(fd95+2+i * 48,mx_tmp,48);
			i ++;
		}
		Dd_mst_hst_Clo_Sel();
		if(i==0)
		{
			memset(mx_tmp,0,sizeof(mx_tmp));
			memset(brf6,0,sizeof(brf6));
			memcpy(to_str,"无",2);
			sprintf(mx_tmp,"%08d%-12.12s%014.0lf%014.0lf",sDd_mst_hst.tx_date,to_str,sDd_mst_hst.tx_amt*100,sDd_mst_hst.bal*100);
			memcpy(fd95+2+i * 48,mx_tmp,48);
			sprintf(mx_tmp,"%02d",1);
		}else{
			sprintf(mx_tmp,"%02d",i);
		}
		memcpy(fd95,mx_tmp,2);
		set_zd_data("0950",fd95);
		strcpy(g_pub_tx.reply,"0000");
		vtcp_log("%s,%d,查询账户明细结束!fd95[%s]",__FILE__,__LINE__,fd95);
		goto OkExit;
	}else if(flag[0]=='2'){
		/*查询多账户余额*/
		ret = Td_mst_Dec_Sel(g_pub_tx.reply,"ac_id=%ld",g_dd_mst.ac_id);
		if(ret)
		{
			vtcp_log("%s,%d,帐号不存在!",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"CU14");
			goto ErrExit;
		}
		memcpy(prdt_name,"活期",4);
		sprintf(fd95+2,"%03d %-5s%012.0lf",g_dd_mst.ac_seqn,prdt_name,g_dd_mst.bal*100);
		j=1;
		while(1)
		{
			if(j>9)
			{
				vtcp_log("%s,%d,做多查询10个定期户[%d]",__FILE__,__LINE__,j);
				break;
			}
			vtcp_log("%s,%d,开始查询定期帐户!",__FILE__,__LINE__);
			ret = Td_mst_Fet_Sel(&sTd_mst,g_pub_tx.reply);
			if(ret && ret !=100)
			{
				vtcp_log("%s,%d,帐号不存在!",__FILE__,__LINE__);
				Td_mst_Clo_Sel();
				strcpy(g_pub_tx.reply,"CU14");
				goto ErrExit;
			}else if(ret){
				vtcp_log("%s,%d,定期户数为[%d]",__FILE__,__LINE__,j);
				break;
			}
			ret = Td_parm_Sel(g_pub_tx.reply,&sTd_parm,"prdt_no='%s'",sTd_mst.prdt_no);
			if(ret)
			{
				vtcp_log("%s,%d,该产品不存在!",__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"CU14");
				goto ErrExit;
			}
			Get_Tdprdt_name(sTd_parm.prdt_no,prdt_name);
			memset(mx_tmp,0,sizeof(mx_tmp));
			sprintf(mx_tmp,"%03d %-5s%012.0lf",sTd_mst.ac_seqn,prdt_name,sTd_mst.bal*100);
			memcpy(fd95+2+j*21,mx_tmp,21);
			j++;
		}
		Td_mst_Clo_Sel();
		sprintf(mx_tmp,"%02d",j);
		memcpy(fd95,mx_tmp,2);
		set_zd_data("0950",fd95);
		vtcp_log("%s,%d,多账户明细[%s]",__FILE__,__LINE__,fd95);
		strcpy(g_pub_tx.reply,"0000");
		goto OkExit;
	}
	account_amt = g_dd_mst.bal; /* 账面余额*/
	
	sprintf(acErrMsg,"g_dd_mst.bal=[%lf] !!", g_dd_mst.bal);
	WRITEMSG
	
	sprintf(acErrMsg,"-------->account_amt===[%lf]", account_amt);
	WRITEMSG
	limit_amt = sCard_reg.atm_max_dr_cash_tx_amt - sCard_reg.atm_td_dr_cash_tx_amt;
	sprintf(acErrMsg,"-------->  每天的交易限额为:[%lf]", limit_amt);
	WRITEMSG
	sprintf(acErrMsg,"-------->该卡今日已取金额为:[%lf]",sCard_reg.atm_td_dr_cash_tx_amt );
	WRITEMSG

	available_amt = g_dd_mst.bal - g_dd_mst.hold_amt - g_dd_mst.ctl_amt;
	sprintf(acErrMsg,"今日可用余额为: [%lf]  ",available_amt);
	WRITEMSG
	if(fee_mode[0]!='0' && fee_mode[0] != '6') /* ATM返回当日可取现金额 */
	{
		if (pub_base_CompDblVal(available_amt, limit_amt) == 1)
		{
			  available_amt = limit_amt;   /* 可用余额 */		  
		}
	}else {  /*POS返回的是余额-预授权金额-冻结金额数*/
	}
	
	sprintf(acErrMsg,"-------->available_amt===[%lf]", available_amt);
	WRITEMSG

	/* 对卡登记簿进行处理 add by LiuHuafeng 2007-2-8 11:07*/
	/*境外查询业务,收取客户4元，2元进手续费收入,其他查询免费*/
	if(fee_mode[0]=='5')/*境外查询*/
	{
		vtcp_log("%s,%d 境外ATM查询收取客户费用",__FILE__,__LINE__);
		get_zd_double("0410",&dJw_fee);  /**取境外ATM查询手续费**/
		memcpy(g_pub_tx.opn_br_no,sMdm_ac_rel.opn_br_no,sizeof(sMdm_ac_rel.opn_br_no)-1);
    /**memcpy(g_pub_tx.tx_br_no,sMdm_ac_rel.opn_br_no,sizeof(sMdm_ac_rel.opn_br_no)-1);**/
	}
	else
	{
		/* 查询免费 **/
		vtcp_log("%s,%d 非境外查询不收费",__FILE__,__LINE__);
		/*组82域*/
		memcpy(tmpstr,"1001156C",8);
		sprintf(tmpstr+8,"%012.0f",account_amt*100);
		memcpy(tmpstr+20,"1002156C",8);
		sprintf(tmpstr+28,"%012.0f",available_amt*100);	
		vtcp_log("%s,%d,tmpstr[%s]",__FILE__,__LINE__,tmpstr);	
		set_zd_data("0820",tmpstr);
		strcpy(g_pub_tx.reply,"0000");
		goto OkExit;
	}
	memset(cAcno_qs, 0 , sizeof(cAcno_qs));
	/** 开封卡手续费收到一个其他应付款的账户上  wyw 20090504
	ret =pub_base_GetParm_Str("CUPS",1,cAcno_qs);
	if(ret)
	{
		*找不到待清算放在银联款项，启动默认帐户TESTCARD*
		memset(cAcno_qs, 0 , sizeof(cAcno_qs));
		strncpy(cAcno_qs,"3001231",sizeof(cAcno_qs)-1);
		vtcp_log("%s,%d 没有找到待清算放在银联款项，所以启用默认帐户=%s",__FILE__,__LINE__,cAcno_qs);
	}
	*****/
	/** 开封卡手续费收到一个其他应付款的账户上  wyw 20090504 
	ret =pub_base_GetParm_Str("KFCAR",22,cAcno_qs);
	if(ret)
	{
		vtcp_log("%s,%d 开封手续费放到一个其他应付款户上=%s",__FILE__,__LINE__,cAcno_qs);
		goto ErrExit;
	}**/
	ret =pub_base_GetParm_Str("CUPS",1,cAcno_qs);
	if(ret)
	{
		/*找不到待清算放在银联款项，启动默认帐户TESTCARD*/
		memset(cAcno_qs, 0 , sizeof(cAcno_qs));
		strncpy(cAcno_qs,"41404",sizeof(cAcno_qs)-1);
		vtcp_log("%s,%d 没有找到待清算放在银联款项，所以启用默认帐户=%s",__FILE__,__LINE__,cAcno_qs);
	}
	/*承德收取客户帐查询费用为4块,开封收两块  del by hxc 晋中卡收费按照神码传送过来的值上送
	ret = pub_base_GetParm_double("KFCAR",10,&dJw_fee);
	if (ret)
	{
		dJw_fee=2.00;
		vtcp_log("%s,%d 境外查询收取客户的费用=%lf",__FILE__,__LINE__,dJw_fee);
	}*/
	char sbrf[21];
	memset(sbrf,0,sizeof(sbrf));
	memcpy(sbrf,"境外查询收费",12);
	set_zd_data("1211",cAcno_qs);
	set_zd_data("1215",CR_FLAG);
	/*set_zd_double("1218",KHJW_FEE - JWQURY_FEE); 开封收取2块钱手续费 20090504*/
	set_zd_double("1218",JWQURY_FEE);   /**晋中卡交银联手续费2元**/
	set_zd_data("121A",sbrf);

	set_zd_data("1021",g_pub_tx.crd_no);	
	set_zd_data("1022","1");	/*帐户序号为1,表示活期户*/
	set_zd_data("1028",g_pub_tx.draw_pwd);	
	set_zd_double("102F",dJw_fee);	
	set_zd_data("102Y",sbrf);
	set_zd_data("102K","2"); /*转帐*/	
	set_zd_data("102J","01");
	set_zd_data("0910",g_mdm_ac_rel.opn_br_no); /*因为要检查通存通兑金额*/

	/*境外查询受理行手续费转送到应解汇款*/
	/*查找应待清算款项科目*/
	memset(cAcno_qs,0,sizeof(cAcno_qs));
	ret = pub_base_GetParm_Str("YLCSR",3,cAcno_qs); /**Mod by hxc 101218 For 银联境外查询手续费入账科目5111203->5111601**/
	if (ret)
	{
		/*若查不到配置的卡手续费收取科目则用5111601收取*/
		memset(cAcno_qs, 0 , sizeof(cAcno_qs));
		strncpy(cAcno_qs,BH_SQATMFEE_KM,sizeof(cAcno_qs)-1);
		vtcp_log("%s,%d 没有找到手续费支出科目，所以启用默认帐户=%s",__FILE__,__LINE__,cAcno_qs);
	}
	
	/** 手续费垫付账号 add by lzy 150109 For 境外ATM查询手续费垫付**/
	memset(zAc_no, 0 , sizeof(zAc_no));
	ret = pub_base_GetParm_Str("YLCDF",1,zAc_no);
	if(ret)
	{
		memset(zAc_no, 0 , sizeof(zAc_no));
		strncpy(zAc_no,BH_DFFEE_KM,sizeof(zAc_no)-1);
		vtcp_log("%s,%d 没有找到垫付科目所以启用默认科目=%s",__FILE__,__LINE__,zAc_no);
	}
	
	if ( pub_base_CompDblVal(dJw_fee,JWQURY_FEE) < 0 )
	{
		g_pub_tx.tx_amt1=JWQURY_FEE-dJw_fee;
		strcpy(g_pub_tx.add_ind,"0");/*借方标志*/
		strcpy(g_pub_tx.ac_no,zAc_no);/*支出用垫付科目*/
		memcpy(g_pub_tx.sub_tx_code,"F002",4);
	}
	else
	{
		g_pub_tx.tx_amt1=dJw_fee-JWQURY_FEE;
		strcpy(g_pub_tx.add_ind,"1");/*贷方标志*/
		strcpy(g_pub_tx.ac_no,cAcno_qs);/*收入用手续费入账科目*/
		memcpy(g_pub_tx.sub_tx_code,"F000",4);
	}
	
	memcpy(g_pub_tx.ct_ind,CR_FLAG,1); /*现转标志 此为2转账*/
	memcpy(g_pub_tx.brf,sbrf,20);
	memcmp(g_pub_tx.cur_no,"01",2); /*币种*/
	
	strcpy( g_pub_tx.ac_id_type,"9" ); /*账户类型为内部*/
	strcpy( g_pub_tx.ac_get_ind,"00" ); /*本程序未读取分户*/
	strcpy( g_pub_tx.ac_wrk_ind,"000" ); /*不核对凭证号，零金额不计流水、明细*/

	strcpy( g_pub_tx.prt_ind,"0" ); /*不登折*/
	g_pub_tx.svc_ind=9001;  /*内部帐存取*/
	strcpy(g_pub_tx.hst_ind,"1"); /*日间入明细*/
	set_zd_data("0770",sMdm_ac_rel.opn_br_no);
	vtcp_log("%s,%d,[0770]sMdm_ac_rel.opn_br_no[%s]",__FILE__,__LINE__,sMdm_ac_rel.opn_br_no);
	set_zd_data("1225","2");	 /*现转标志*/
	set_zd_double("1226",g_pub_tx.tx_amt1);
	memcpy(g_pub_tx.tx_code,"6202",4);

	ret = pub_acct_trance();
	if(ret)
	{
		vtcp_log("%s,%d,调用存款主控失败!",__FILE__,__LINE__);
		goto ErrExit;
	}
	/**为垫付科目记账专门增加,为了将50704记到各支行 begin... BY LZY 20150119**/
	memcpy(g_pub_tx.opn_br_no,sMdm_ac_rel.opn_br_no,sizeof(sMdm_ac_rel.opn_br_no)-1); 
	/**为垫付科目记账专门增加,为了将50704记到各支行 END ... BY LZY 20150119**/
	ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
	if(ret)
	{
		vtcp_log("%s,%d,调用会计接口程序失败!",__FILE__,__LINE__);
		goto ErrExit;
	}
	memcpy(g_pub_tx.reply,"0001",4);/*境外查询走收费分支*/

	/*组82域*/
	account_amt=account_amt-dJw_fee;           /*返回扣除手续费之后的账户余额信息*/
	/*available_amt=available_amt-dJw_fee;       返回扣除手续费之后的账户可用余额信息*/
	
	/**Add by hxc 重新计算账户可用余额 110303**/
	available_amt =0.00;
	available_amt = g_dd_mst.bal - g_dd_mst.hold_amt - g_dd_mst.ctl_amt-dJw_fee;
	sprintf(acErrMsg,"今日可用余额为: [%lf]  ",available_amt);
	WRITEMSG
	if(fee_mode[0]!='0' && fee_mode[0] != '6') /* ATM返回当日可取现金额 */
	{
		if (pub_base_CompDblVal(available_amt, limit_amt) == 1)
		{
			  available_amt = limit_amt;   /* 可用余额 */		  
		}
	}else {  /*POS返回的是余额-预授权金额-冻结金额数*/
	}
	sprintf(acErrMsg,"-------->available_amt===[%lf]", available_amt);
	WRITEMSG
	
	set_zd_double("0390", available_amt); 
	memcpy(tmpstr,"1001156C",8);
	sprintf(tmpstr+8, "%012.0f", account_amt*100);
	memcpy(tmpstr+20,"1002156C",8);
	sprintf(tmpstr+28, "%012.0f", available_amt*100);
	vtcp_log("%s,%d,tmpstr[%s]",__FILE__,__LINE__,tmpstr);	
	set_zd_data("0820",tmpstr);
	/*end by LiuHuafeng 2007-2-8 11:07 **/
	vtcp_log("%s,%d,境外查询收取费用",__FILE__,__LINE__);
	goto OkExit1;	 
	/* 给自助设备交易日记结构赋值
	chnl_self_trade.tx_date = g_pub_tx.tx_date;
	chnl_self_trade.tx_time = g_pub_tx.tx_time;
	chnl_self_trade.trace_no = g_pub_tx.trace_no;
	strcpy(chnl_self_trade.cur_no,g_pub_tx.cur_no);
	strcpy(chnl_self_trade.ac_no,g_pub_tx.crd_no);
	strcpy(chnl_self_trade.tx_code,g_pub_tx.tx_code);
	strcpy(chnl_self_trade.tx_br_no,g_pub_tx.tx_br_no);
	strcpy(chnl_self_trade.opn_br_no,g_pub_tx.opn_br_no);
	strcpy(chnl_self_trade.tel,g_pub_tx.tel);
	strcpy(chnl_self_trade.ttyp,"1");
	strcpy(chnl_self_trade.sctp, "AT");

	strcpy(chnl_self_trade.ttyn,g_pub_tx.tty);
	* 本交易对帐中心流水号 *
	strcpy(chnl_self_trade.brf_no,"ATMG");
	strcpy(chnl_self_trade.batch_flag,"0");
	strcpy(chnl_self_trade.stat,"0");
	strcpy(chnl_self_trade.info_source,"0");
	* 登记自助设备交易日记 *
	ret = pub_base_Insertselftrade(chnl_self_trade);
	if (ret != 0)
	{
		sprintf(acErrMsg,"登记自助设备交易日记错误!!");
		WRITEMSG
		goto ErrExit;
	}

	*  登记交易流水 *
	strcpy(g_pub_tx.brf,"ATM查询余额!!");

	if (pub_ins_trace_log())
	{
		sprintf(acErrMsg,"登记交易流水错误!!");
		WRITEMSG
		goto ErrExit;
	} 
	  */

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"success!!");
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
OkExit1:
	sprintf(acErrMsg,"success!!,境外查询交易收取手续费");
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	if(strlen(g_pub_tx.reply)==0 || strcmp(g_pub_tx.reply,"0000")==0)
	{
		vtcp_log("%s,%d,ATM查询失败!",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"P102");
	}
	sprintf(acErrMsg,"failed !! reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
int Get_Tdprdt_name(char *prdt_no,char *prdt_name)
{
	vtcp_log("%s,%d,得到定期产品名称!prdt_no[%s]",__FILE__,__LINE__,prdt_no);
	if(!memcmp(prdt_no,"204",3) || !memcmp(prdt_no,"205",3) || !memcmp(prdt_no,"206",3) || !memcmp(prdt_no,"207",3) || !memcmp(prdt_no,"201",3) || !memcmp(prdt_no,"202",3))
	{
		memcpy(prdt_name,"整整",4);
	}else if(!memcmp(prdt_no,"209",3) || !memcmp(prdt_no,"209",3) || !memcmp(prdt_no,"209",3))
	{
		memcpy(prdt_name,"零整",4);	
	}else if(!memcmp(prdt_no,"211",3) || !memcmp(prdt_no,"212",3) ||!memcmp(prdt_no,"213",3))
	{
		memcpy(prdt_name,"教育",4);
	}else if(!memcmp(prdt_no,"221",3) || !memcmp(prdt_no,"222",3) || !memcmp(prdt_no,"223",3))
	{
		memcpy(prdt_name,"整零",4);
	}else if(!memcmp(prdt_no,"231",3) || !memcmp(prdt_no,"232",3) || !memcmp(prdt_no,"233",3))
	{
		memcpy(prdt_name,"存本",4);
	}else if(!memcmp(prdt_no,"241",3))
	{
		memcpy(prdt_name,"定活",4);
	}else if(!memcmp(prdt_no,"242",4) || !memcmp(prdt_no,"243",3))
	{
		memcpy(prdt_name,"通知",4);
	}
	vtcp_log("%s,%d,得到定期产品名称!prdt_name[%s]",__FILE__,__LINE__,prdt_name);
	return(0);
}
int Zip_halfChinese(char *fstr,char *tstr)
{
	printf("%s,%d,输入数据[%s]输出数据[%s]\n",__FILE__,__LINE__,fstr,tstr);
	int len=0;
	char *strf;
	char *strt;
	len=strlen(fstr);
	if(len == 0)
	{
		return(0);
	}
	strt = tstr;
	strf = fstr;
	printf("strf[%s],fstr+len[%d]\n",strf,fstr+len);
	for(strf=fstr; strf <= fstr+len;)
	{
		if(*strf > 128)
		{
			if(*(strf+1) > 128)
			{
				*strt = *strf;
				*(strt+1) = *(strf+1);
			}else{
				*strt = 32;
				*(strt+1) = *(strf+1);
			}
			strf=strf+2;
			strt=strt+2;
			printf("tstr[%s]\n",tstr);
		}else{
			*strt = *strf;
			strf++;
			strt++;
			printf("tstr[%s]\n",tstr);
		}
	}
	printf("%s,%d,输出数据[%s]\n",__FILE__,__LINE__,tstr);
	return(0);
}
