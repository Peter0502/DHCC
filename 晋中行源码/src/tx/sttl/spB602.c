/*************************************************
* 文 件 名:  spB602.c
* 功能描述： 债券投资买入记账
*
* 作    者:  
* 完成日期： 2003年1月20日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "mo_invest_c.h"
#include "com_parm_c.h"
#include "mbfe_pub.h"
#include "mo_city_c.h"

int spB602()
{
    struct mo_invest_c sMo_invest,sMo_invest1;
    struct in_parm_c sIn_parm;
    struct com_parm_c sCom_parm;
	struct mbfeghvsendjrn_c mbfeghvsendjrn_v;
	struct mo_city_c mo_city_c;
    char flag[2],zzlx[2];
    int ret;

    memset(&sMo_invest,0x0,sizeof(struct mo_invest_c));
    memset(&sMo_invest1,0x0,sizeof(struct mo_invest_c));
    memset(&sIn_parm, 0x0, sizeof(struct in_parm_c));
	memset(&mbfeghvsendjrn_v,0x00,sizeof(struct mbfeghvsendjrn_c));
	memset(&sCom_parm,0x00,sizeof(struct com_parm_c));
	memset(&mo_city_c, 0x0, sizeof(struct mo_city_c));

    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"初始化公用结构错误!!");
        WRITEMSG
        goto ErrExit;
    }

    get_zd_data("0580",sMo_invest.pact_no);    /* 投资协议编号 */
    get_zd_data("0230",sMo_invest.bkrl_code);  /* 产品编号 */
    get_zd_data("0860",sMo_invest.ticket_type);/* 买入债券品种 */
    get_zd_double("0390",&sMo_invest.buy_amt); /* 买入价 */
    get_zd_double("0400",&sMo_invest.par_bal); /* 票面价 */
    get_zd_long("0440",&sMo_invest.isuue_date);/* 发行日期 */
    get_zd_long("0480",&sMo_invest.term);       /* 债券期限 */
    get_zd_long("0450",&sMo_invest.mtr_date);  /* 到期日期 */
    get_zd_data("0680",sMo_invest.intst_type); /* 债券计息方式 */
    get_zd_double("0840",&sMo_invest.rate);    /* 收益率 */
    get_zd_char("0690",flag);                 /* 现转标志 */
	get_zd_double("0410",&sMo_invest.pre_intst); /* 应记利息 */
	get_zd_double("0420",&sMo_invest.overflow_amt); /* 溢价 */
	get_zd_double("0430",&sMo_invest.abate_amt);	  /* 折价 */
/*	get_zd_double("1003",&sMo_invest.singl_feeval);	   手续费 */
    strcpy(sMo_invest.sts,"1");

    ret = Mo_invest_Sel(g_pub_tx.reply,&sMo_invest1,"pact_no ='%s' ",
          sMo_invest.pact_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"投资协议编号错误!! [%d]",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"B041");
        goto ErrExit;
    }
    if (sMo_invest1.sts[0] != '1')
    {
        sprintf(acErrMsg,"该投资协议编号已记帐!! [%d]",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"B042");
        goto ErrExit;
    }
    if (strcmp(sMo_invest.bkrl_code,sMo_invest1.bkrl_code) != 0)
    {
        sprintf(acErrMsg,"投资产品编号输入错误!! ");
        WRITEMSG
        strcpy(g_pub_tx.reply,"B045");
        goto ErrExit;
    }
    strcpy(sMo_invest1.ticket_type,pub_base_strpack(sMo_invest1.ticket_type));
    if (strcmp(sMo_invest.ticket_type,sMo_invest1.ticket_type) != 0)
    {
        sprintf(acErrMsg,"买入债券品种输入错误!! ");
        WRITEMSG
        strcpy(g_pub_tx.reply,"B046");
        goto ErrExit;
    }
    if (pub_base_CompDblVal(sMo_invest.buy_amt,sMo_invest1.buy_amt) != 0)
    {
        sprintf(acErrMsg,"买入价输入错误!! ");
        WRITEMSG
        strcpy(g_pub_tx.reply,"B048");
        goto ErrExit;
    }
    if (pub_base_CompDblVal(sMo_invest.par_bal,sMo_invest1.par_bal) != 0)
    {
        sprintf(acErrMsg,"票面价输入错误!! ");
        WRITEMSG
        strcpy(g_pub_tx.reply,"B050");
        goto ErrExit;
    }
    if (sMo_invest.isuue_date != sMo_invest1.isuue_date)
    {
        sprintf(acErrMsg,"发行日期输入错误!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"B051");
        goto ErrExit;
    }
    if (sMo_invest.term != sMo_invest1.term)
    {
        sprintf(acErrMsg,"债券期限输入错误!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"B052");
        goto ErrExit;
    }
    if (sMo_invest.mtr_date != sMo_invest1.mtr_date)
    {
        sprintf(acErrMsg,"到期日期输入错误!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"B053");
        goto ErrExit;
    }
    if (strcmp(sMo_invest.intst_type,sMo_invest1.intst_type) != 0)
    {
        sprintf(acErrMsg,"债券计息方式输入错误!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"B054");
        goto ErrExit;
    }
    if (pub_base_CompDblVal(sMo_invest.rate,sMo_invest1.rate) != 0)
    {
        sprintf(acErrMsg,"收益率输入错误!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"B055");
        goto ErrExit;
    }
/*    if (pub_base_CompDblVal(sMo_invest.singl_feeval,sMo_invest1.singl_feeval) != 0)
    {
        sprintf(acErrMsg,"手续费金额错误!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"B296");
        goto ErrExit;
    }
*/
    ret = In_parm_Sel(g_pub_tx.reply,&sIn_parm,"prdt_no = '%s'",
          sMo_invest.bkrl_code);
    if (ret != 0)
    {
        sprintf(acErrMsg,"查询内部账产品表错误",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"W035");
        goto ErrExit;
    }

    /* 开介质 */
    strcpy(g_pub_tx.mdm_code,NULL_MDM_CODE);
    strcpy(g_pub_tx.cur_no,"01");
    strcpy(g_pub_tx.prdt_code,sMo_invest.bkrl_code);

    ret = pub_acct_opn_in_mdm();
    if (ret != 0)
    {
        sprintf(acErrMsg,"开介质出错");
        WRITEMSG
        goto ErrExit;
    }

    sprintf(acErrMsg,"ac_no=[%s]",g_pub_tx.ac_no);
    WRITEMSG
    sprintf(acErrMsg,"开介质成功 PASS!!! ");
    WRITEMSG

    /* 开户 */
    strcpy(g_pub_tx.ac_id_type,"9");
    g_pub_tx.rate = sMo_invest.rate;
    strcpy(g_pub_tx.add_ind,"1");
    g_pub_tx.hst_ind[0] = '1';
    /**
    if ((sIn_parm.invst_type[0] == '1') || (sIn_parm.invst_type[0] == '3'))
    {
       
        g_pub_tx.tx_amt1 = sMo_invest.buy_amt;
    }
    else if (sIn_parm.invst_type[0] == '2')
    {

        g_pub_tx.tx_amt1 = sMo_invest.par_bal;
    }
**/
g_pub_tx.tx_amt1=0;
    if(pub_acct_opn_ac())
    {
        sprintf(acErrMsg,"开户出错");
        WRITEMSG
        goto ErrExit;
    }
	/**
	ret = pubf_acct_proc("460A");
	if( ret )
	{
        sprintf(acErrMsg,"登记会计流水错误!");
        WRITEMSG
        goto ErrExit;
	}
***/
    set_zd_data("0310",g_pub_tx.ac_no);
    sMo_invest.ac_id = g_pub_tx.ac_id;
    sprintf(acErrMsg,"投资产品开户成功 PASS!!! ac_no=[%s] ",g_pub_tx.ac_no);
    WRITEMSG

/*	get_zd_data("0700",zzlx);
  	if( zzlx[0]=='2' )		大额
	{
		strcpy(mbfeghvsendjrn_v.payeracc,g_pub_tx.ac_no);
		strcpy(mbfeghvsendjrn_v.payername,g_pub_tx.name);
	}
	else if( zzlx[0]=='1' )
*/
		strcpy(mo_city_c.payer_ac_no,g_pub_tx.ac_no);
		strcpy(mo_city_c.payer_name,g_pub_tx.name);
		get_zd_data("0300",mo_city_c.payee_ac_no);
		get_zd_data("0270",mo_city_c.payee_name);
	
/* 利息科目开户 20060928 */
	  if (sIn_parm.invst_type[0] == '2')
	{
		if (sMo_invest.intst_type[0]=='4')
	{
	  sMo_invest.lx_ac_id = 0;
     vtcp_log("[%s][%d]开利息帐户\n",__FILE__,__LINE__);
      /* 开介质 **/
    strcpy(g_pub_tx.ac_no,"");
		g_pub_tx.ac_id=0;
		g_pub_tx.ac_seqn=0;
    	strcpy(g_pub_tx.mdm_code,NULL_MDM_CODE);
    	strcpy(g_pub_tx.cur_no,"01");
    	strcpy(g_pub_tx.prdt_code,INVEST_LX);
    	
    	ret = pub_acct_opn_in_mdm();
    	if (ret != 0)
    	{
        	sprintf(acErrMsg,"开介质出错");
        	WRITEMSG
       		goto ErrExit;
    	}
    	
      sprintf(acErrMsg,"ac_no=[%s]",g_pub_tx.ac_no);
    	WRITEMSG
    	sprintf(acErrMsg,"利息开介质成功 PASS!!! ");
    	WRITEMSG
    	
    	/* 开户 **/
    	strcpy(g_pub_tx.ac_id_type,"9");
    	g_pub_tx.rate = 0 ;
    	strcpy(g_pub_tx.add_ind,"1");
    	g_pub_tx.hst_ind[0] = '1';
      g_pub_tx.tx_amt1 = 0;
      
      if(pub_acct_opn_ac())
    	{
        	sprintf(acErrMsg,"利息开户出错");
        	WRITEMSG
        	goto ErrExit;
    	}
    	
    	set_zd_data("0330",g_pub_tx.ac_no);
    	sMo_invest.lx_ac_id = g_pub_tx.ac_id;
	}
	else
		{
	   vtcp_log("[%s][%d]到期一次付款开14204帐户\n",__FILE__,__LINE__);
	    /* 开介质 **/
    strcpy(g_pub_tx.ac_no,"");
		g_pub_tx.ac_id=0;
		g_pub_tx.ac_seqn=0;
    strcpy(g_pub_tx.mdm_code,NULL_MDM_CODE);
    strcpy(g_pub_tx.cur_no,"01");
   	strcpy(g_pub_tx.prdt_code,INVEST_LX1);
    
    	ret = pub_acct_opn_in_mdm();
    	if (ret != 0)
    	{
        	sprintf(acErrMsg,"开介质出错");
        	WRITEMSG
       		goto ErrExit;
    	}
    	
      sprintf(acErrMsg,"ac_no=[%s]",g_pub_tx.ac_no);
    	WRITEMSG
    	sprintf(acErrMsg,"利息开介质成功 PASS!!! ");
    	WRITEMSG
    	
    	/* 开户 **/
    	strcpy(g_pub_tx.ac_id_type,"9");
    	g_pub_tx.rate = 0 ;
    	strcpy(g_pub_tx.add_ind,"1");
    	g_pub_tx.hst_ind[0] = '1';
      g_pub_tx.tx_amt1 = 0;
      
      if(pub_acct_opn_ac())
    	{
        	sprintf(acErrMsg,"利息开户出错");
        	WRITEMSG
        	goto ErrExit;
    	}
    	
    	set_zd_data("0330",g_pub_tx.ac_no);
    	sMo_invest.lx_ac_id = g_pub_tx.ac_id;
    }
	
	/* 溢价科目开户 */
    
				
	sMo_invest.overflow_ac_id = 0;
	if( pub_base_CompDblVal(sMo_invest.overflow_amt,0.00)>0 )
	{
		vtcp_log("[%s][%d]开溢价帐户\n",__FILE__,__LINE__);
    	/* 开介质 */
		strcpy(g_pub_tx.ac_no,"");
		g_pub_tx.ac_id=0;
		g_pub_tx.ac_seqn=0;
    strcpy(g_pub_tx.mdm_code,NULL_MDM_CODE);
    strcpy(g_pub_tx.cur_no,"01");
    strcpy(g_pub_tx.prdt_code,INVEST_OVER);

    ret = pub_acct_opn_in_mdm();
    	if (ret != 0)
    	{
        	sprintf(acErrMsg,"开介质出错");
        	WRITEMSG
       		goto ErrExit;
    	}

    	sprintf(acErrMsg,"ac_no=[%s]",g_pub_tx.ac_no);
    	WRITEMSG
    	sprintf(acErrMsg,"溢价开介质成功 PASS!!! ");
    	WRITEMSG

    	/* 开户 */
    	strcpy(g_pub_tx.ac_id_type,"9");
    	g_pub_tx.rate = sMo_invest.rate;
    	strcpy(g_pub_tx.add_ind,"1");
    	g_pub_tx.hst_ind[0] = '0';
     	g_pub_tx.tx_amt1 = 0;

    	if(pub_acct_opn_ac())
    	{
        	sprintf(acErrMsg,"溢价开户出错");
        	WRITEMSG
        	goto ErrExit;
    	}

    	set_zd_data("0320",g_pub_tx.ac_no);
    	sMo_invest.overflow_ac_id = g_pub_tx.ac_id;
	}

	/* 折价科目开户 */
    sMo_invest.abate_ac_id = 0;
	if( pub_base_CompDblVal(sMo_invest.abate_amt,0.00)>0 )
	{
		vtcp_log("[%s][%d]开折价帐户\n",__FILE__,__LINE__);
    	/* 开介质 */
		strcpy(g_pub_tx.ac_no,"");
		g_pub_tx.ac_id=0;
		g_pub_tx.ac_seqn=0;
    	strcpy(g_pub_tx.mdm_code,NULL_MDM_CODE);
    	strcpy(g_pub_tx.cur_no,"01");
    	strcpy(g_pub_tx.prdt_code,INVEST_ABATE);

    	ret = pub_acct_opn_in_mdm();
    	if (ret != 0)
    	{
        	sprintf(acErrMsg,"折价开介质出错");
        	WRITEMSG
       		goto ErrExit;
    	}

    	sprintf(acErrMsg,"ac_no=[%s]",g_pub_tx.ac_no);
    	WRITEMSG
    	sprintf(acErrMsg,"折价开介质成功 PASS!!! ");
    	WRITEMSG

    	/* 开户 */
    	strcpy(g_pub_tx.ac_id_type,"9");
    	g_pub_tx.rate = sMo_invest.rate;
    	strcpy(g_pub_tx.add_ind,"1");
    	g_pub_tx.hst_ind[0] = '1';
      g_pub_tx.tx_amt1 = 0;

    	if(pub_acct_opn_ac())
    	{
        	sprintf(acErrMsg,"折价开户出错");
        	WRITEMSG
        	goto ErrExit;
    	}

    	set_zd_data("0320",g_pub_tx.ac_no);
    	sMo_invest.abate_ac_id = g_pub_tx.ac_id;
	}
}

    /* 修改投资登记簿 */
    ret = Mo_invest_Dec_Upd(g_pub_tx.reply,"pact_no='%s' and sts='1'",
          sMo_invest.pact_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"declare for update error [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }
    ret = Mo_invest_Fet_Upd(&sMo_invest1,g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"Fetch mo_invest error !! [%d]",ret);
        WRITEMSG
        Mo_invest_Clo_Upd();
        goto ErrExit;
    }
    strcpy(sMo_invest1.sts,"2");
    sMo_invest1.ac_id = sMo_invest.ac_id;
    sMo_invest1.lx_ac_id = sMo_invest.lx_ac_id;
    sMo_invest1.overflow_ac_id = sMo_invest.overflow_ac_id;
    sMo_invest1.abate_ac_id = sMo_invest.abate_ac_id;
    sMo_invest1.wrk_date = g_pub_tx.tx_date;
    sMo_invest1.trace_no = g_pub_tx.trace_no;
    strcpy(sMo_invest1.tel,g_pub_tx.tel);
    strcpy(sMo_invest1.chk,g_pub_tx.chk);
    strcpy(sMo_invest1.auth,g_pub_tx.auth);

    ret = Mo_invest_Upd_Upd(sMo_invest1,g_pub_tx.reply);
    if (ret != 0) 
    {
        sprintf(acErrMsg,"修改投资登记簿错误!! [%d]",ret);
        WRITEMSG
        Mo_invest_Clo_Upd();
        strcpy(g_pub_tx.reply,"B057");
        goto ErrExit;
    }

	/*set_zd_double("0390",sMo_invest.buy_amt-sMo_invest.singl_feeval);手续费本交易还不处理*/
/***	if ( zzlx[0]=='1' ) 			 同城 
	{
		ret = Com_parm_Sel( g_pub_tx.reply, &sCom_parm, "parm_code='TCKM' and parm_seqn='2'" );
		if( ret )
		{
			sprintf( acErrMsg, "查询公共参数表出错[%d]!", g_reply_int );
			WRITEMSG
			goto ErrExit;
		}
***/
/*		set_zd_data("0300",sCom_parm.val);
		mo_city_c.tx_date = g_pub_tx.tx_date;
		mo_city_c.tx_no = g_pub_tx.trace_no;
		strcpy(mo_city_c.tx_br_no, g_pub_tx.tx_br_no);
		strcpy(mo_city_c.tel,g_pub_tx.tel);
		strcpy(mo_city_c.trans_mode,"1");
		strcpy(mo_city_c.in_out_ind,"1"); 	 提出 */
/*		strcpy(mo_city_c.cd_ind,"2");		 提出贷 */
/*		get_zd_double("0390",&mo_city_c.bill_amt);
		strcpy(mo_city_c.sts,"3");
		strcpy(mo_city_c.bill_type,"1");  票据种类：同城 */
/*		mo_city_c.bat_no = 0;
		mo_city_c.no = 0;
		ret = Mo_city_Ins(mo_city_c, g_pub_tx.reply);
		if (ret != 0)
		{
			sprintf(acErrMsg,"登记同城登记簿错误!! [%d]",ret);
			WRITEMSG
			goto ErrExit;
		}
	}
	else if( zzlx[0]=='2' )			 大额 ***/
/***	{
		strcpy(mbfeghvsendjrn_v.tx_br_no,g_pub_tx.tx_br_no);
		strcpy(mbfeghvsendjrn_v.tx_code,g_pub_tx.tx_code);
		mbfeghvsendjrn_v.consigndate=g_pub_tx.tx_date;
		mbfeghvsendjrn_v.trace_no=g_pub_tx.trace_no;
		strcpy(mbfeghvsendjrn_v.procstate,"0");
		get_zd_double("0390",&mbfeghvsendjrn_v.amount);

		ret = Mbfeghvsendjrn_Ins(mbfeghvsendjrn_v, g_pub_tx.reply);
		if( ret )
    	{
        	sprintf(acErrMsg,"登记大额汇兑登记簿错误![%d]",g_reply_int);
        	WRITEMSG
        	goto ErrExit;
    	}
		ret = Com_parm_Sel( g_pub_tx.reply, &sCom_parm, "parm_code='DEKM' and parm_seqn='1'" );
		if( ret )
		{
			sprintf( acErrMsg, "查询公共参数表出错[%d]!", g_reply_int );
			WRITEMSG
			goto ErrExit;
		}
		set_zd_data("0300",sCom_parm.val);
	}
***/
    if (flag[0] == '1')
    {
        /* 调用现金子交易 */
        goto OkExitCash;
    }
    else
    {
        /* 调用转帐子交易 */
        get_zd_data("0300",g_pub_tx.ac_no);
        g_pub_tx.ac_id = 0;
        g_pub_tx.ac_seqn = 0;
        g_pub_tx.tx_amt1 = sMo_invest.buy_amt;
        if (sIn_parm.invst_type[0] == '2')
		goto YOkExit;
		else 
		goto ZYOkExit;
}

OkExitCash:
    strcpy( g_pub_tx.reply, "0001" );
    sprintf(acErrMsg,"success!!");
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
YOkExit:   /*转帐出口*/
    strcpy( g_pub_tx.reply, "0002" );
    sprintf(acErrMsg,"success!!");
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
ZYOkExit:
	  strcpy( g_pub_tx.reply, "0003" );
    sprintf(acErrMsg,"success!!");
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
ErrExit:
	sprintf(acErrMsg,"failed !! reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
