/*************************************************
* 文 件 名: sp9734.c
* 功能描述：根据帐号回显帐户支取信息_[支取信息维护交易]
* 作    者: rob
* 完成日期：2003年02月19日
* 修改记录：   
*     1. 日    期:
*        修 改 人:
*        修改内容:
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#define EXTERN
#include "public.h"
#include "mob_acct_type_hst_c.h"
#include "mob_acct_type_c.h"

sp9734()  
{ 		
	int ret=0;
	char ac_no[20];
	char mob_sts[8];         /*签约状态*/
	char tx_date[16];					/*签约日期*/
	char mobile[50];					/*签约手机，可多个*/
	/*新增类型，区分平台过来的交易*/
	char type[4];
	struct mob_acct_type_hst_c sMob_acct_type_hst;
	struct mob_acct_type_c sMob_acct_type;

	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&sMob_acct_type_hst,0x00,sizeof(sMob_acct_type_hst));
	memset(&sMob_acct_type,0x00,sizeof(sMob_acct_type));
	memset(&ac_no,0x00,sizeof(ac_no));
	memset(&mob_sts,0x00,sizeof(mob_sts));
	memset(&tx_date,0x00,sizeof(tx_date));
	memset(&mobile,0x00,sizeof(mobile));
	memset(&type,0x00,sizeof(type));
  /*新增神码查询短信状态*/
  get_zd_data("0720",type);
  if(0 == strcmp(type,"6"))
  {
  	get_zd_data("0300",ac_no);						/*帐号*/
		pub_base_old_acno( ac_no );  	/** 对旧帐号的处理 **/
		ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel, "ac_no='%s'",ac_no);
		if(ret)
		{
			sprintf(acErrMsg,"该账户不存在！ [%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"D104");
			set_zd_data(DC_REPLY,g_pub_tx.reply);
			return 0;
		}
		ret =  Mob_acct_type_Sel(g_pub_tx.reply,&sMob_acct_type,"ac_no = '%s' ",ac_no);
		if(ret==100)
		{
			sprintf(acErrMsg,"该账户未签约！ [%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"F038");
			set_zd_data(DC_REPLY,g_pub_tx.reply);
			return 0;
		}else if( ret )
		{
			sprintf(acErrMsg,"查找帐号出错!!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"D103");
			goto ErrExit;
		}
		strcpy(g_pub_tx.reply,"F036");
  	set_zd_data(DC_REPLY,g_pub_tx.reply);
  	set_zd_data("0810",sMob_acct_type.mobile);
		return 0;
  }
  
	/** 取值、赋值 **/	
	get_zd_data("0300",ac_no);						/*帐号*/
	pub_base_old_acno( ac_no );  	/** 对旧帐号的处理 **/
	set_zd_data("0300",ac_no);	/* 处理旧帐号 */
	
	/* 调用根据帐号返回账户序号函数 */
	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel, "ac_no='%s'",ac_no);
	if(ret==100)
	{
		sprintf(acErrMsg,"介质帐号对照表不存在该记录!! [%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}
	/*add by chenggongxun 20091023 晋中 beg*/
	else if( ret )
	{
		sprintf(acErrMsg,"查找帐号出错!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	/*if( g_mdm_ac_rel.note_sts[0]=='1' )20111117 YW  不做挂失判断
	{
		sprintf(acErrMsg,"该介质已挂失!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P170");
		goto ErrExit;
	}
	else if( g_mdm_ac_rel.note_sts[0]=='2' ) */
	if( g_mdm_ac_rel.note_sts[0]=='2' )
	{
		sprintf(acErrMsg,"该介质已挂失换证!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P171");
		goto ErrExit;
	}
	else if( g_mdm_ac_rel.note_sts[0]=='3' )
	{
		sprintf(acErrMsg,"该介质已被换证!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"M022");
		goto ErrExit;
	}
	else if( g_mdm_ac_rel.note_sts[0]=='4' )
	{
		sprintf(acErrMsg,"该介质已部提换证!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"D215");
		goto ErrExit;
	}
	else if( g_mdm_ac_rel.note_sts[0]=='*' )
	{
		sprintf(acErrMsg,"该介质已做销户处理!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"M012");
		goto ErrExit;
	}

	/*add by chenggongxun 20091023 晋中 beg*/
	set_zd_data("0670",g_mdm_ac_rel.draw_pwd_yn);	/*是否密码支取*/
	set_zd_data("0680",g_mdm_ac_rel.id_type);   /*证件类型*/
	set_zd_data("0620",g_mdm_ac_rel.id_no);     /*证件号码*/
 
	/* 20110728 add by yanqq */
	set_zd_data("0910",g_mdm_ac_rel.opn_br_no);  /*开户行号*/
	set_zd_data("0490",g_mdm_ac_rel.mdm_code);     /*账户介质代号[0010-存折 0013-存单 0020-磁条卡]*/

	/* 输出变量 */
	if(g_mdm_ac_rel.draw_pwd_yn[0]=='Y')			/*1-修改密码*/
		set_zd_data("0700","1");
	else if(g_mdm_ac_rel.draw_pwd_yn[0]=='N')		/*2-设置密码*/
		set_zd_data("0700","2");

	/** 查询短信历史签约信息 **/
	ret =  Mob_acct_type_hst_Sel(g_pub_tx.reply,&sMob_acct_type_hst,"ac_no = '%s' order by tx_date desc,trace_no desc,mob_sts",ac_no);
	if(100 == ret)
	{
			ret =  Mob_acct_type_Sel(g_pub_tx.reply,&sMob_acct_type,"ac_no = '%s' ",ac_no);
			if(ret != 0 && ret != 100)
			{
					vtcp_log("[%s][%d]查询mob_acct_type表出错![%d]\n",__FILE__,__LINE__,ret);
					goto ErrExit;
			}
			else if(100 == ret)
			{
					vtcp_log("[%s][%d]该账户没有签约手机![%d]\n",__FILE__,__LINE__,ret);
					set_zd_data("0250","关闭");
					goto OkExit;
			}
			set_zd_data("0250","开通");
			set_zd_data("0810",sMob_acct_type.mobile);
			sprintf(tx_date,"%ld",sMob_acct_type.tx_date);			
			set_zd_data("0860",tx_date);
			set_zd_data("0920",sMob_acct_type.tel);							/*操作柜员*/
			goto OkExit;
	}
	else if(ret != 0 && ret != 100)
	{
			vtcp_log("[%s][%d]查询mob_acct_type_hst表出错![%d]\n",__FILE__,__LINE__,ret);
			strcpy(g_pub_tx.reply,"D102");
			goto ErrExit;
	}
	if(0 == strcmp(sMob_acct_type_hst.mob_sts,"3"))
	{
		sprintf(tx_date,"%ld",sMob_acct_type_hst.tx_date);
		set_zd_data("0870",tx_date);															/*解约日期*/
		set_zd_data("0920",sMob_acct_type_hst.lst_tel);							/*操作柜员*/
		set_zd_data("0250","关闭");																/*状态*/
		goto OkExit;
	}
	
	/** 查询短信签约信息 **/
												
	strcpy(mobile,sMob_acct_type_hst.mobile);
	sprintf(tx_date,"%ld",sMob_acct_type_hst.tx_date);			
	set_zd_data("0860",tx_date);    										/*签约/有效日期*/					
	set_zd_data("0920",sMob_acct_type_hst.lst_tel);							/*操作柜员*/   
	set_zd_data("0810",mobile);													/*签约手机*/
	if(0 == strcmp(sMob_acct_type_hst.mob_sts,"2"))
	{
		set_zd_data("0250","暂停");
	}
	set_zd_data("0250","开通"); 

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
   
