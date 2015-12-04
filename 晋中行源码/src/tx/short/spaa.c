/*************************************************************
* 文 件 名: sp8890.c
* 功能描述：短信单笔扣费
*
* 作    者: xujian
* 完成日期: 2010-12-26 15:09
* 功    能:          接收前台传的扣费帐户、金额、解约账号等要素，校验扣费帐户，修改签约表中lst_fee_date字段，
*                    借扣费帐户、贷短信服务业务收入
* 修改记录:
* 日    期:
* 修 改 人:
* 修改内容:
* insert into tx_def values('8890','短信银行解约扣费','10000000000000010000000010000000000010011000000100000000000000000000111100000000000001000000000000000000000000000000000000000100','1','4','0','3','');
* insert into tx_flow_def values('8891','0','8890','#=0000@1@#$')
* insert into tx_flow_def values('8891','1','8891','#$');
* insert into tx_sub_def values('8890','短信银行解约扣费','sp8890','0','0');
**************************************************************/
#define EXTERN
#include "public.h"
#include "mob_acct_type_c.h"
#include "com_item_c.h"
#include "mob_server_type_c.h"
#include "mob_fee_mx_c.h"

sp8890()
{
	vtcp_log("ddddddddddddddddddddddddddddddd");

	int  months, months2;
	int  iDate,iMon,iYear;
	int txflag=0;	/*判断是否需要进行本记账交易 0 --需要 1--不需要*/
	double dAmt_should,dAmt_fact;/**单笔应收，单笔实收**/
	double bal;
	long cnt;
	char dc_ind[2], ct_ind[2] ,fee_ind[2] ;
	char cOpt[2];/**操作类型0开通1修改2注销**/
	struct dd_mst_c stDd_mst;
	struct dd_mst_c st_Dd_mst_1;
	struct mdm_ac_rel_c stMdm_ac_rel;
	struct mdm_ac_rel_c stMdm_ac_rel_1;

	struct com_item_c stCom_item;
	struct mob_acct_type_c stMob_acct_type;
	struct mob_fee_mx_c sMob_fee;
	struct ln_mst_c st_ln_mst;
	char acc_no[20];		/*记手续费科目*/
	char tx_br_no[5+1] ;
	char in_br_no[5+1];
	
	memset(in_br_no,0x00,sizeof(in_br_no));
	memset(&st_ln_mst,0x00,sizeof(st_ln_mst));
	memset(&stMdm_ac_rel,0x00,sizeof(stMdm_ac_rel));
	memset(&stMdm_ac_rel_1,0x00,sizeof(stMdm_ac_rel_1));
	memset(&stDd_mst,0x00,sizeof(stDd_mst));
	memset(&st_Dd_mst_1,0x00,sizeof(st_Dd_mst_1));
	memset(&stMob_acct_type,0x00,sizeof(stMob_acct_type));
	memset(&sMob_fee,0x00,sizeof(sMob_fee));
	memset(dc_ind, 0x0, sizeof(dc_ind));
	memset(ct_ind, 0x0, sizeof(ct_ind));
	memset(fee_ind, 0x0, sizeof(fee_ind));
	memset(cOpt,0x00,sizeof(cOpt));
	memset(acc_no,0x00,sizeof(acc_no));
	memset(tx_br_no,0x00,sizeof(tx_br_no));
	memset(tx_br_no,0,sizeof(tx_br_no));

	months=0;
	
	if (pub_base_sysinit())
	{
		sprintf(acErrMsg,"%s","初始化公用结构错误!!");
		WRITEMSG
		strcpy(g_pub_tx.reply, "CU06");
		goto ErrExit;
	}
	get_zd_data("0030",tx_br_no);/*交易机构*/
	get_zd_data("0370",g_pub_tx.crd_no);   /*签约帐号*/
	get_zd_double("0400",&g_pub_tx.tx_amt1);/*交易金额*/
	get_zd_data("0660",dc_ind);		/*借贷标志 1--借 2---贷*/
	get_zd_data("0680",ct_ind);		/*现转标志 1--现金 2--转账*/
	get_zd_data("0700",fee_ind);	/*收费标志 0 不收 1 现*/
	get_zd_data("0720",cOpt);

	get_zd_data("0790",g_pub_tx.draw_pwd); /*支付密码*/
	get_zd_data("0310",g_pub_tx.ac_no1);   /*扣款帐号*/
	
	zip_space(g_pub_tx.crd_no);
	zip_space(fee_ind);

	/** modify by liaohx 2011-2-23 16:31:47 不打印密码明文
	vtcp_log("%s,%d,现转[%s],卡号[%s],收费账号[%s],金额[%.2f],密码[%s]\n",__FILE__,__LINE__,ct_ind,g_pub_tx.crd_no,g_pub_tx.ac_no1,g_pub_tx.tx_amt1,g_pub_tx.draw_pwd);
	**/
	vtcp_log("%s,%d,现转[%s],卡号[%s],收费账号[%s],金额[%.2f]\n",__FILE__,__LINE__,ct_ind,g_pub_tx.crd_no,g_pub_tx.ac_no1,g_pub_tx.tx_amt1);

	if( strcmp(cOpt,"0") ==0  || strcmp(cOpt,"1") ==0 )  /*签约与变更时直接返回0000,不进行此交易的处理*/
	{
		vtcp_log("%s,%d,当前情况无需记账!![%s]\n",__FILE__,__LINE__,cOpt);
		txflag=1;
		strcpy(g_pub_tx.reply,"0000");
		goto OkExit;
	}

	g_reply_int=Mob_acct_type_Sel(g_pub_tx.reply,&stMob_acct_type,"(ac_no='%s' or filler='%s') ",g_pub_tx.crd_no,g_pub_tx.crd_no);
	if(g_reply_int)
	{
	    vtcp_log("[%s][%d]查询表Mob_acct_type_Sel错误[%d]",__FILE__,__LINE__,g_reply_int);
	    strcpy(g_pub_tx.reply,"D104");
	    goto ErrExit ;
	}


    if(strcmp(stMob_acct_type.sfbz,"0") ==0 ){
		vtcp_log("%s,%d,收费标志为[%s]不记账[%.2f]\n",__FILE__,__LINE__,stMob_acct_type.sfbz);
		/*strcpy(g_pub_tx.reply, "0001"); */
		goto OkExit;
	}
    if(g_pub_tx.tx_amt1 ==0.0)	{
		vtcp_log("%s,%d,零金额收费不记账[%.2f]\n",__FILE__,__LINE__,g_pub_tx.tx_amt1);
		/*strcpy(g_pub_tx.reply, "0001"); */
		goto OkExit;
	}

	vtcp_log("[%s][%d] 短信扣费开始记账-----g_pub_tx.tx_amt1=[%f]",__FILE__,__LINE__,g_pub_tx.tx_amt1);
	
	
	
	/*新旧账号转换*/
	strcpy(g_pub_tx.cur_no, "01");
	if(g_pub_tx.ac_no1[0]>='0' && g_pub_tx.ac_no1[0]<='9')
	{
		pub_base_old_acno(g_pub_tx.ac_no1);
	}

	if(stMob_acct_type.ac_type[0]=='1'){
	
		g_reply_int = Mdm_ac_rel_Sel(g_pub_tx.reply,&stMdm_ac_rel_1,"ac_no='%s'",g_pub_tx.crd_no);	
		if (g_reply_int)
		{
			vtcp_log("[%s][%d]查询表Mdm_ac_rel_Sel错误[%d]",__FILE__,__LINE__,g_reply_int);
			strcpy(g_pub_tx.reply,"D104");
			goto ErrExit;
		}
		g_reply_int=Dd_mst_Sel(g_pub_tx.reply,&st_Dd_mst_1,"ac_id=%d",stMdm_ac_rel_1.ac_id);
		if(g_reply_int){
			vtcp_log("[%s][%d]查询表Dd_mst_Sel错误[%d]",__FILE__,__LINE__,g_reply_int);
			strcpy(g_pub_tx.reply,"D104");
			goto ErrExit;
		}
		memcpy(in_br_no,st_Dd_mst_1.opn_br_no,5);
	}

	if(stMob_acct_type.ac_type[0]=='3'){
	/* 	edit by zhanglb 20130318	g_reply_int=Ln_mst_Sel(g_pub_tx.reply,&st_ln_mst,"pact_no='%s' and ac_sts<>'*' ",g_pub_tx.crd_no); */
	   g_reply_int=Ln_mst_Sel(g_pub_tx.reply,&st_ln_mst,"pact_no='%s'  ",g_pub_tx.crd_no);

		if (g_reply_int)
		{
			vtcp_log("[%s][%d]查询表Ln_mst_Sel错误[%d]",__FILE__,__LINE__,g_reply_int);
			strcpy(g_pub_tx.reply,"D104");
			goto ErrExit;
		}
		memcpy(in_br_no,st_ln_mst.opn_br_no,5);
	}

	if(stMob_acct_type.ac_type[0]=='A'){
	/*	edit by zhanglb 20130318 g_reply_int=Ln_mst_Sel(g_pub_tx.reply,&st_ln_mst,"contract_no='%s' and ac_sts='1' ",g_pub_tx.crd_no); */
		g_reply_int=Ln_mst_Sel(g_pub_tx.reply,&st_ln_mst,"contract_no='%s' ",g_pub_tx.crd_no);
		if (g_reply_int)
		{
			vtcp_log("[%s][%d]查询表Ln_mst_Sel错误[%d]",__FILE__,__LINE__,g_reply_int);
			strcpy(g_pub_tx.reply,"D104");
			goto ErrExit;
		}
		memcpy(in_br_no,st_ln_mst.opn_br_no,5);
	}


	
	pub_base_GetParm_Str("88002",66,acc_no);		/*贷记手续费收入科目60219903*/	

	cnt=0;
	memset(&sMob_fee,0x00,sizeof(sMob_fee));

	if( ct_ind[0]=='2' )
	{/*转账*/
		g_reply_int = Mdm_ac_rel_Sel(g_pub_tx.reply,&stMdm_ac_rel,"ac_no='%s'",g_pub_tx.ac_no1);	
		if (g_reply_int)
		{
        		vtcp_log("[%s][%d]查询表Mdm_ac_rel_Sel错误[%d]",__FILE__,__LINE__,g_reply_int);
        		strcpy(g_pub_tx.reply,"D104");
			goto ErrExit;
		}
		g_reply_int=Dd_mst_Sel(g_pub_tx.reply,&stDd_mst,"ac_id=%d",stMdm_ac_rel.ac_id);
		if(g_reply_int){
	       		vtcp_log("[%s][%d]查询表Dd_mst_Sel错误[%d]",__FILE__,__LINE__,g_reply_int);
        		strcpy(g_pub_tx.reply,"D104");
			goto ErrExit;
		}
		/*-----借记客户账,检查是否账户正常------*/
		if(stMdm_ac_rel.note_sts[0]!='0')/**介质状态不正常**/
		{
		    g_reply_int=-1;
		    vtcp_log("[%s][%d]介质状态不正常[%d]",__FILE__,__LINE__,g_reply_int);
		    strcpy(g_pub_tx.reply,"D139");
		    goto ErrExit;
		}
		if(stDd_mst.ac_sts[0]!='1' ||stDd_mst.hold_sts[0]!='0')/**账户状态不正常**/
		{
		    g_reply_int=-1;
		    vtcp_log("[%s][%d]账户状态不正常[%d]",__FILE__,__LINE__,g_reply_int);
		    strcpy(g_pub_tx.reply,"L183");
		    goto ErrExit;
		}
		vtcp_log("%s,%d,stMdm_ac_rel.ac_id=[%ld]!",__FILE__,__LINE__,stMdm_ac_rel.ac_id);
		
		
		/**下客户账**/
		g_reply_int=sub_tran_acct(stMdm_ac_rel.ac_no,g_pub_tx.tx_amt1,"短信扣费",stDd_mst.opn_br_no,"2");
		if(g_reply_int)
		{
			goto ErrExit;
		}
		/**记科目账**/
		g_reply_int=sub_cr_acct(acc_no,g_pub_tx.tx_amt1,"短信扣费",in_br_no,"2");
		if(g_reply_int)
		{
			goto ErrExit;
		}
	}		

	if( ct_ind[0]=='1' )
	{
		/*贷短信服务业务收入60219903 借现金M001*/
		memcpy(g_pub_tx.ac_no1,"100102",6);
		g_reply_int=sub_dr_cash_acct(g_pub_tx.ac_no1,g_pub_tx.tx_amt1,"短信扣费",g_pub_tx.tx_br_no,"1");
		if(g_reply_int)
		{
				goto ErrExit;
			}
		g_reply_int=sub_cr_acct(acc_no,g_pub_tx.tx_amt1,"短信扣费",in_br_no,"1");
		if(g_reply_int)
		{
				goto ErrExit;
		}

	}

		
	/**修改mob_acct_type*/
	vtcp_log("[%s][%d] 修改签约表上次扣费日期 g_pub_tx.crd_no=[%s]",__FILE__,__LINE__,g_pub_tx.crd_no);
	g_reply_int=Mob_acct_type_Dec_Upd(g_pub_tx.reply," ac_no='%s' or filler='%s' ",g_pub_tx.crd_no,g_pub_tx.crd_no);
	if(g_reply_int)
	{
		sprintf(acErrMsg, "取声明游标错误Mob_acct_type [%d]", g_reply_int);
		WRITEMSG
		goto ErrExit;
	}

	g_reply_int=Mob_acct_type_Fet_Upd(&stMob_acct_type,g_pub_tx.reply);
	if(g_reply_int==100){
		sprintf(acErrMsg, "短信签约表无此记录 [%d][%s]", g_reply_int,g_pub_tx.reply);
		WRITEMSG
		goto ErrExit;
	}
	else if(g_reply_int)
	{/*错误处理*/
		sprintf(acErrMsg, "取短信收费签约记录失败 [%d][%s]", g_reply_int,g_pub_tx.reply);
		WRITEMSG
		Mob_acct_type_Clo_Sel();
		goto ErrExit;
	}
	
	months=pub_base_CalTrueDays(stMob_acct_type.lst_fee_date,g_pub_tx.tx_date)/30;
	sMob_fee.lst_fee_date=stMob_acct_type.lst_fee_date;
	memcpy(sMob_fee.sign_ac_no,stMob_acct_type.filler,sizeof(stMob_acct_type.filler));

	stMob_acct_type.lst_fee_date=g_pub_tx.tx_date;
	
	g_reply_int=Mob_acct_type_Upd_Upd(stMob_acct_type,g_pub_tx.reply);
	if(g_reply_int)
	{
		sprintf(acErrMsg, "更新Mob_acct_type [%d]错误", g_reply_int);
		WRITEMSG
		Mob_acct_type_Clo_Sel();
		goto ErrExit;
	}

	
	
	cnt++;
	
	/*插入Mob_fee_mx_Ins*/
	memcpy(sMob_fee.tel,g_pub_tx.tel,6);
	memcpy(sMob_fee.tx_code,g_pub_tx.tx_code,4);
	memcpy(sMob_fee.ct_ind,ct_ind,1);
	memcpy(sMob_fee.br_no,g_pub_tx.tx_br_no,sizeof(g_pub_tx.tx_br_no)-1);
	if(stMob_acct_type.ac_type[0]!='1'){
		memcpy(sMob_fee.name,st_ln_mst.name,sizeof(st_ln_mst.name)-1);
		set_zd_data("0250",st_ln_mst.name);
		memcpy(sMob_fee.sign_ac_no,stMob_acct_type.filler,sizeof(stMob_acct_type.filler));
	}else{
		memcpy(sMob_fee.name,st_Dd_mst_1.name,sizeof(st_Dd_mst_1.name)-1);
		set_zd_data("0250",st_Dd_mst_1.name);
		memcpy(sMob_fee.sign_ac_no,stMob_acct_type.ac_no,sizeof(stMob_acct_type.ac_no));
	}

	memcpy(sMob_fee.ac_no,g_pub_tx.ac_no1,sizeof(g_pub_tx.ac_no1));

	memcpy(sMob_fee.server_type,stMob_acct_type.server_type,sizeof(sMob_fee.server_type)-1);
	sMob_fee.amt_should=g_pub_tx.tx_amt1;
	sMob_fee.amt_fact=g_pub_tx.tx_amt1;
	sMob_fee.cnt=months;
	sMob_fee.tx_date=g_pub_tx.tx_date;
	sMob_fee.trace_no=g_pub_tx.trace_no;
	sMob_fee.print_cnt=1;
	memcpy(sMob_fee.memo,"短信服务费扣费",20);
	memcpy(sMob_fee.sts,"0",1);
	vtcp_log("1111111111111111111");
	
	g_reply_int=Mob_fee_mx_Ins(sMob_fee,g_pub_tx.reply);
	if(g_reply_int)
	{
		vtcp_log("33333333333333333333");
		sprintf(acErrMsg, "插入Mob_fee_Ins [%d]错误", g_reply_int);
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("22222222222222222222");
OkExit:
	sprintf(acErrMsg,"%s","短信扣费sp8890处理结束!");
	WRITEMSG
	if(txflag==0){
		Mob_acct_type_Clo_Sel();
	}
	strcpy(g_pub_tx.reply, "0000"); /*reply=0001 不收费*/
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	vtcp_log("%s,%d,g_pub_tx.reply[%s]",__FILE__,__LINE__,g_pub_tx.reply);
	return 0;
	
	
ErrExit:
	vtcp_log("[%s][%d]短信扣费处理失败! tx_code=[sp8890],prog_name=[sp8890]",__FILE__,__LINE__);
	sprintf(acErrMsg,"%s","短信扣费处理失败! ");
	WRITEMSG
	if (strlen(g_pub_tx.reply)==0 || memcmp(g_pub_tx.reply,"0000",4)==0)
	{
		strcpy(g_pub_tx.reply,"CU06");
	}
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;

	
	
}


int sub_tran_acct( char *dr_ac_no,double amt,char *brf,char * br_no,char * ct_ind)
{
	int ret=0;
	vtcp_log("借方记帐科目[%s][%.2lf]",dr_ac_no,amt);
	g_pub_tx.ac_id = 0;	 /*test clean */
	g_pub_tx.ac_seqn = 0;
	strcpy( g_pub_tx.tx_code, "8891");
	strcpy( g_pub_tx.ac_no, dr_ac_no );
	strcpy(g_pub_tx.tx_br_no,g_pub_tx.tx_br_no);
	strcpy(g_pub_tx.opn_br_no,br_no);
	g_pub_tx.tx_amt1 = amt;
	if(dr_ac_no[0]=='9')
	{
			strcpy( g_pub_tx.ac_id_type,"9" ); /*账户类型为内部*/
			strcpy( g_pub_tx.ac_get_ind,"00" ); /*本程序未读取分户*/
			g_pub_tx.svc_ind=9001;
			memset( g_pub_tx.sub_tx_code, 0x0, sizeof(g_pub_tx.sub_tx_code) );
			strcpy( g_pub_tx.sub_tx_code, "A016" );
			set_zd_data( DC_SUB_TX_CODE, g_pub_tx.sub_tx_code );
			set_zd_data( "1204", "01" );
			set_zd_data( "1205", "2" );
			set_zd_double( "1208", g_pub_tx.tx_amt1 );
	}
	else
	{
			strcpy( g_pub_tx.ac_id_type,"1" ); /*账户类型为活期*/
			strcpy( g_pub_tx.ac_get_ind,"00" ); /*本程序未读取分户*/
			g_pub_tx.svc_ind=1001;
			memset( g_pub_tx.sub_tx_code, 0x0, sizeof(g_pub_tx.sub_tx_code) );
			strcpy( g_pub_tx.sub_tx_code, "D003" );
			set_zd_data( DC_SUB_TX_CODE, g_pub_tx.sub_tx_code );
			strcpy( g_pub_tx.prt_ind, "0" );
			set_zd_data( "102O", "0" );
			set_zd_data( "102J", "01" );
			set_zd_data( "102K", "2" );
			set_zd_double( "102F", g_pub_tx.tx_amt1 );
			set_zd_double( "102P", 0.00 );
			set_zd_double( "102Q", 0.00 );
	}
  /*strcpy( g_pub_tx.brf,brf );*/
	strcpy( g_pub_tx.brf,"短信扣费" );
	strcpy( g_pub_tx.no_show,"0" );
	strcpy(g_pub_tx.add_ind,"0");	

	strcpy(g_pub_tx.ct_ind,"2");
	/*strcpy(g_pub_tx.ac_wrk_ind,"001001001");*/
	strcpy(g_pub_tx.ac_wrk_ind,"000000101");
	strcpy(g_pub_tx.hst_ind,"1");
	ret=pub_acct_trance();
	if( ret ) goto ErrExit;
  /* 进行会计记帐 */
	ret = pubf_acct_proc(g_pub_tx.sub_tx_code );
	if (ret != 0)
	{
			sprintf(acErrMsg,"会计记帐不成功!!");
			WRITEMSG
			goto ErrExit;
	}
	set_zd_data("102O","" );
	set_zd_data("102J","" );
	set_zd_double("102F",0.00 );
	set_zd_data("102K","" );
	set_zd_double("102P",0.00 );
	set_zd_double("102Q",0.00 );
	set_zd_data( "1204", "" );
	set_zd_data( "1205", "" );
	set_zd_double( "1208", 0.00 );
	strcpy( g_pub_tx.add_ind,"" );
	return 0;
ErrExit:
	return 1;
}
int sub_cr_acct(char * cr_ac_no,double amt,char *brf,char * br_no,char * ct_ind)
{
	int ret=0;
	vtcp_log("贷方记帐科目[%s][%.2lf]",cr_ac_no,amt);
	strcpy( g_pub_tx.tx_code, "8891");
	strcpy(g_pub_tx.tx_br_no,g_pub_tx.tx_br_no);
	strcpy(g_pub_tx.opn_br_no,br_no);
	strcpy( g_pub_tx.ac_id_type,"9" ); /*账户类型为内部*/
	strcpy( g_pub_tx.ac_get_ind,"00" ); /*本程序未读取分户*/
	strcpy(g_pub_tx.cur_no,"01");
	strcpy(g_pub_tx.ac_wrk_ind,"001001001");
	strcpy(g_pub_tx.hst_ind,"1");
	g_pub_tx.svc_ind=9001;
	strcpy( g_pub_tx.ct_ind,ct_ind );
	strcpy( g_pub_tx.add_ind,"1" );
	strcpy( g_pub_tx.prdt_code,"" );
	strcpy( g_pub_tx.ac_no,cr_ac_no );
	g_pub_tx.ac_id=0;
	g_pub_tx.ac_seqn=0;
	g_pub_tx.tx_amt1=amt;
	strcpy(g_pub_tx.brf,brf);
	strcpy( g_pub_tx.no_show,"0");
	strcpy( g_pub_tx.sub_tx_code, "A017");

	/*** 调用会计分录自动记 ***/
	ret=pub_acct_trance();
	if( ret ) goto ErrExit;
	/* 进行会计记帐 */
	set_zd_data("1214","01" );
	set_zd_data("1215",ct_ind );
	set_zd_double("1218",amt);
	ret = pubf_acct_proc("A017");
	if (ret != 0)
	{
		sprintf(acErrMsg,"会计记帐不成功!!");
		WRITEMSG
		goto ErrExit;
	}
	set_zd_data("1204","" );
	set_zd_data("1205","" );
	set_zd_double("1208",0.00 );
	set_zd_data("1214","" );
	set_zd_data("1215","" );
	set_zd_double("1218",0.00 );
    strcpy( g_pub_tx.add_ind,"" );

	return 0;
ErrExit:
	return 1;
}
/************************
借现金
*************************/
int sub_dr_cash_acct(char * dr_ac_no,double amt,char *brf,char * br_no,char * ct_ind)
{
	int ret=0;
	
	vtcp_log("现金科目[%s][%.2lf]",dr_ac_no,amt);
	strcpy( g_pub_tx.tx_code, "8891");
	strcpy( g_pub_tx.sub_tx_code, "M001");
	strcpy(g_pub_tx.tx_br_no,br_no);
	strcpy(g_pub_tx.opn_br_no,br_no);
	strcpy(g_pub_tx.cur_no,"01");
	strcpy(g_pub_tx.ac_wrk_ind,"0000000");
	g_pub_tx.svc_ind=0;
	strcpy( g_pub_tx.ct_ind,ct_ind );
	strcpy( g_pub_tx.add_ind,"1" );
	strcpy( g_pub_tx.prdt_code,"" );
	g_pub_tx.ac_id=0;
	g_pub_tx.ac_seqn=0;
	g_pub_tx.tx_amt1=amt;
  strcpy(g_pub_tx.brf,brf);
	strcpy( g_pub_tx.no_show,"0" );
	get_zd_data("0070",g_pub_tx.tel);
	strcpy(g_pub_tx.ac_no,dr_ac_no);
	strcpy(g_pub_tx.hst_ind,"1");
	strcpy( g_pub_tx.cash_code,"12J08" );
	ret=pub_acct_cash();
	if( ret ) goto ErrExit;
  /* 进行会计记帐 */
	set_zd_data("1181","01" );
  set_zd_double("1184",amt);
  ret = pubf_acct_proc("M001");
  if (ret != 0)
  {
     sprintf(acErrMsg,"会计记帐不成功!!");
     WRITEMSG
     goto ErrExit;
  }
		set_zd_data("1181","" );
    set_zd_double("1184",0.00 );
    strcpy( g_pub_tx.add_ind,"" );
	return 0;
ErrExit:
	return 1;
}
