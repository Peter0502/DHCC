/*************************************************
* 文 件 名:  spB604.c
* 功能描述： 债券投资出售兑付记账
*
* 作    者:  rob
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
#include "mo_city_c.h"

int spB604()
{
    struct mo_invest_c sMo_invest,sMo_invest1;
    struct mdm_ac_rel_c sMdm_ac_rel;   
    struct in_mst_c sIn_mst;
    struct in_parm_c sIn_parm;
	struct com_parm_c  sCom_parm;
	struct mo_city_c  mo_city_c;
    char flag[2],zqzl[2],zzlx[2];
	long   tmp_date=0,tmp_trace_no=0;
	double	tmp_amt=0.00;
    int ret;

    memset(&sMo_invest, 0x0, sizeof(struct mo_invest_c));
    memset(&sMo_invest1, 0x0, sizeof(struct mo_invest_c));
    memset(&sMdm_ac_rel, 0x0, sizeof(struct mdm_ac_rel_c));
    memset(&sIn_mst, 0x0, sizeof(struct in_mst_c));
		memset(&sIn_parm,0x0,sizeof(struct in_parm_c));
	memset(&sCom_parm,0x00,sizeof(struct com_parm_c));
	memset(&mo_city_c,0x00,sizeof(struct mo_city_c));
	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));

    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"初始化公用结构错误!!");
        WRITEMSG
        goto ErrExit;
    }
    
    get_zd_data("0580",sMo_invest.pact_no);  /* 投资协议编号 */
    get_zd_char("0690",flag);                /* 现转标志 */
    get_zd_double("0390",&sMo_invest1.sol_amt); /* 卖出价 */
    get_zd_double("0400",&sMo_invest1.par_bal); /* 票面价 */
    get_zd_double("0410",&sMo_invest1.pre_intst); /* 应记利息 */
	get_zd_double("0420",&sMo_invest1.overflow_amt);
	get_zd_double("0430",&sMo_invest1.abate_amt);
	get_zd_double("1002",&sMo_invest1.buy_amt);		/* 买入成本价?? */
	strcpy(g_pub_tx.cur_no,"01");
	get_zd_data("0670",zqzl);

    ret = Mo_invest_Sel(g_pub_tx.reply,&sMo_invest,"pact_no ='%s'",sMo_invest.pact_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"投资协议编号错误!! [%s][%s][%d]",g_pub_tx.tx_br_no,sMo_invest.pact_no,ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"B041");
        goto ErrExit;
    }
    
   vtcp_log("开始查找登记簿主账户[%ld]\n",sMo_invest.ac_id);
     /* 找登记簿主账户的id */
    ret = Mdm_ac_rel_Dec_Sel(g_pub_tx.reply,"ac_id = '%ld' ",sMo_invest.ac_id);
    if (ret != 0)
    {
        sprintf(acErrMsg,"declare for Sel error [%s][%ld][%d]",g_pub_tx.tx_br_no,sMo_invest.ac_id,ret);
        WRITEMSG
        goto ErrExit;
    }
    ret = Mdm_ac_rel_Fet_Sel(&sMdm_ac_rel, g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"Fetch mdm_ac_rel_Fet error [%s][%ld][%d]",g_pub_tx.tx_br_no,sMo_invest.ac_id,ret);
        WRITEMSG
        Mdm_ac_rel_Clo_Sel();
        goto ErrExit;
    }
    Mdm_ac_rel_Clo_Sel();
 
	if( sMo_invest.sts[0]!='2' )
	{
        sprintf(acErrMsg,"投资协议状态不正确!![%s]",sMo_invest.sts);
        WRITEMSG
        strcpy(g_pub_tx.reply,"B041");
        goto ErrExit;
	}

    if (pub_base_CompDblVal(sMo_invest.par_bal,sMo_invest1.par_bal) )
    {
        sprintf(acErrMsg,"票面价大于登记簿里的票面价!! ");
        WRITEMSG
        strcpy(g_pub_tx.reply,"B085");
        goto ErrExit;
    }
    if (pub_base_CompDblVal(sMo_invest.buy_amt,sMo_invest1.buy_amt) == -1)
    {
        sprintf(acErrMsg,"买入成本大于买入价!! ");
        WRITEMSG
        strcpy(g_pub_tx.reply,"P422");
        goto ErrExit;
    }
  
    ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_no= '%s' ",
          sMdm_ac_rel.ac_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"查询介质账号关系表错误 [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }
    if (sMdm_ac_rel.ac_id != sMo_invest.ac_id)
    {
        sprintf(acErrMsg,"投资账号输入错误 !!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"B058");
        goto ErrExit;
    }
   
    sMo_invest.sol_amt += sMo_invest1.sol_amt;
    sMo_invest.sol_date = g_pub_tx.tx_date;

    ret = In_mst_Sel(g_pub_tx.reply,&sIn_mst,"ac_id=%ld and ac_seqn = %d",
          sMo_invest.ac_id,sMdm_ac_rel.ac_seqn);
    if (ret != 0)
    {
        sprintf(acErrMsg,"查询投资账户主文件出错!! [%d]",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"B059");
        goto ErrExit;
    }

    ret = In_parm_Sel(g_pub_tx.reply,&sIn_parm,"prdt_no='%s'",
          sMo_invest.bkrl_code);
    if (ret != 0)
    {
        sprintf(acErrMsg,"查询内部产品参数表错误!!![%d]",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"W058");
        goto ErrExit;
    }

    g_pub_tx.ac_seqn = 0;
    g_pub_tx.ac_id = 0;
    g_pub_tx.hst_ind[0] = '1';

    strcpy(g_pub_tx.ac_no,sMdm_ac_rel.ac_no);
    g_pub_tx.ac_id = sMdm_ac_rel.ac_id;
    g_pub_tx.ac_seqn = 0;

	if( zqzl[0]=='1' && sIn_parm.invst_type[0]!='1' )
	{
        sprintf(acErrMsg,"此笔协议购入的债券不是短期债券!![%s]",sMo_invest.bkrl_code);
        WRITEMSG
        strcpy(g_pub_tx.reply,"P420");
        goto ErrExit;
	}
	if( zqzl[0]=='2' && sIn_parm.invst_type[0]!='2' )
	{
        sprintf(acErrMsg,"此笔协议购入的债券不是中长期债券!![%s]",sMo_invest.bkrl_code);
        WRITEMSG
        strcpy(g_pub_tx.reply,"P421");
        goto ErrExit;
	}

    if (sIn_parm.invst_type[0] == '1' || sIn_parm.invst_type[0] == '3')
    {
        /* 短期债券投资  可转换债券投资 */
		    g_pub_tx.tx_amt1 = sMo_invest1.par_bal;
		vtcp_log("DDD开始票面价记帐g_pub_tx.tx_amt1=[%ld]sMo_invest1.par_bal=[%ld]\n",g_pub_tx.tx_amt1,sMo_invest1.par_bal);
    }
    else if (sIn_parm.invst_type[0] == '2')
    {
        /* 中长期债券投资类别 */
        g_pub_tx.tx_amt1 = sMo_invest1.par_bal;
        vtcp_log("CCC开始票面价记帐g_pub_tx.tx_amt1=[%ld]sMo_invest1.par_bal=[%ld]\n",g_pub_tx.tx_amt1,sMo_invest1.par_bal);
    }
    else 
    {
        WRITEMSG
        goto ErrExit;
    }
   
    if ( pub_base_CompDblVal(sIn_mst.bal,g_pub_tx.tx_amt1)<0 )
	{
        sprintf(acErrMsg,"票面价大于登记簿里的票面价!! ");
        WRITEMSG
        strcpy(g_pub_tx.reply,"B085");
        goto ErrExit;
	}
	else if( pub_base_CompDblVal(g_pub_tx.tx_amt1,0.00)>0 )
    {
    	vtcp_log("KKK开始票面价记帐g_pub_tx.tx_amt1=[%ld]sMo_invest1.par_bal=[%ld]\n",g_pub_tx.tx_amt1,sMo_invest1.par_bal);
        sprintf(acErrMsg,"bal=[%lf],tx_amt=[%lf]",sIn_mst.bal,g_pub_tx.tx_amt1);
        WRITEMSG
        g_pub_tx.add_ind[0] = '1';	/** 贷方,不是增加 **/
		strcpy(g_pub_tx.ct_ind,flag);
		if( pub_base_CompDblVal(sIn_mst.bal,g_pub_tx.tx_amt1)==0 )
			g_pub_tx.svc_ind=9002;
		else
			g_pub_tx.svc_ind=9001;
        if (pub_acct_trance())
        {
            sprintf(acErrMsg,"投资账户取款错误!!");
            WRITEMSG
            goto ErrExit;
        }
		ret = pubf_acct_proc("460B");
		if( ret )
		{
        	sprintf(acErrMsg,"登记会计流水错误!");
        	WRITEMSG
        	goto ErrExit;
		}
    }
    if (sIn_parm.invst_type[0] == '2')
   {
    sMo_invest.par_bal -= sMo_invest1.par_bal;
    sMo_invest.buy_amt -= sMo_invest1.buy_amt;


   vtcp_log("开始利息户记帐sMo_invest.lx_ac_id=[%ld]\n",sMo_invest.lx_ac_id);
   
    /*  利息的记帐  */
    memset(&sIn_mst,0x00,sizeof(struct in_mst_c));
    	ret = In_mst_Sel(g_pub_tx.reply,&sIn_mst,"ac_id=%ld and ac_seqn=0 ",
          sMo_invest.lx_ac_id);
    vtcp_log(" 利息记帐金额sIn_mst.bal=[%ld]\n",sIn_mst.bal);
    	if (ret != 0)
    	{
        	sprintf(acErrMsg,"查询投资账户主文件出错!! [%d]",ret);
        	WRITEMSG
        	strcpy(g_pub_tx.reply,"B059");
        	goto ErrExit;
    	}
    	  strcpy(g_pub_tx.ac_no,"");
		    g_pub_tx.ac_id=sMo_invest.lx_ac_id;
        g_pub_tx.add_ind[0] = '1';	/** 贷方,不是增加 **/
	    	g_pub_tx.tx_amt1=sIn_mst.bal;
	      ret=In_parm_Sel(g_pub_tx.reply,&sIn_parm,"prdt_no=%s",sIn_mst.prdt_no);
	      vtcp_log("利息产品表sIn_parm.acc_hrt=[%s]\n",sIn_parm.acc_hrt);
	      if (ret != 0)
    	{
        	sprintf(acErrMsg,"查询投资账户主文件出错!! [%d]",ret);
        	WRITEMSG
        	strcpy(g_pub_tx.reply,"B059");
        	goto ErrExit;
    	}
    	set_zd_data("0153",sIn_parm.acc_hrt);  /*利息科目*/
		    set_zd_double("1002",sIn_mst.bal);  /*  利息金额 */
		    strcpy(g_pub_tx.ct_ind,flag);
			   g_pub_tx.svc_ind=9002;
			    vtcp_log("269 利息记帐金额sIn_mst.bal=[%ld]\n",sIn_mst.bal);
       if (pub_acct_trance())
        {
            sprintf(acErrMsg,"投资账户取款错误!!");
            WRITEMSG
            goto ErrExit;
        }
     sMo_invest.pre_intst -= sIn_mst.bal;
    
    /* 溢价的处理 */
    if (pub_base_CompDblVal(sMo_invest1.overflow_amt,0.00) > 0)
    {
    vtcp_log("KIsMo_invest.overflow_ac_id的值!![%ld]\n",sMo_invest.overflow_ac_id);
		memset(&sIn_mst,0x00,sizeof(struct in_mst_c));
    	ret = In_mst_Sel(g_pub_tx.reply,&sIn_mst,"ac_id=%ld and ac_seqn=0 ",
          sMo_invest.overflow_ac_id);
    	if (ret != 0)
    	{
        	sprintf(acErrMsg,"查询投资账户主文件出错!! [%d]",ret);
        	WRITEMSG
        	strcpy(g_pub_tx.reply,"B059");
        	goto ErrExit;
    	}

		strcpy(g_pub_tx.ac_no,"");
		g_pub_tx.ac_id=sMo_invest.overflow_ac_id;
        g_pub_tx.add_ind[0] = '1';	/** 贷方,不是增加 **/
		g_pub_tx.tx_amt1=sMo_invest1.overflow_amt;
		strcpy(g_pub_tx.ct_ind,flag);
		if ( pub_base_CompDblVal(sMo_invest1.overflow_amt,sIn_mst.bal)==0 )
			g_pub_tx.svc_ind=9002;
		else 
			g_pub_tx.svc_ind=9001;
        if (pub_acct_trance())
        {
            sprintf(acErrMsg,"投资账户取款错误!!");
            WRITEMSG
            goto ErrExit;
        }
		sMo_invest.overflow_amt -= sMo_invest1.overflow_amt;
    }

    /* 折价的处理 */
    if (pub_base_CompDblVal(sMo_invest1.abate_amt,0.00) > 0)
    {
    	vtcp_log("KIsMo_invest.abate_ac_id的值!![%ld]\n",sMo_invest.abate_ac_id);
		memset(&sIn_mst,0x00,sizeof(struct in_mst_c));
    	ret = In_mst_Sel(g_pub_tx.reply,&sIn_mst,"ac_id=%ld and ac_seqn=0 ",
          sMo_invest.abate_ac_id);
    	if (ret != 0)
    	{
        	sprintf(acErrMsg,"查询投资账户主文件出错!! [%d]",ret);
        	WRITEMSG
        	strcpy(g_pub_tx.reply,"B059");
        	goto ErrExit;
    	}
 
		strcpy(g_pub_tx.ac_no,"");
		g_pub_tx.ac_id=sMo_invest.abate_ac_id;
		g_pub_tx.tx_amt1=sMo_invest1.abate_amt;
        g_pub_tx.add_ind[0] = '0';	/** 借方,不是增加 **/
		strcpy(g_pub_tx.ct_ind,flag);
		if ( pub_base_CompDblVal(sMo_invest1.abate_amt,sIn_mst.bal)==0 )
			g_pub_tx.svc_ind=9002;
		else 
			g_pub_tx.svc_ind=9001;
        if (pub_acct_trance())
        {
            sprintf(acErrMsg,"投资账户取款错误!!");
            WRITEMSG
            goto ErrExit;
        }
		sMo_invest.abate_amt -= sMo_invest1.abate_amt;
    }

    sMo_invest.trace_no = g_pub_tx.trace_no;   
    strcpy(sMo_invest.tel, g_pub_tx.tel);
    strcpy(sMo_invest.chk, g_pub_tx.chk);
    strcpy(sMo_invest.auth,g_pub_tx.auth);
 
    double abc=0;
    get_zd_double("1002",&abc);   /*利息金额 */
    vtcp_log(" 利息金额是[%.0f]\n",abc);    
    get_zd_double("0400",&sMo_invest1.par_bal); /* 票面价 */
  	get_zd_double("0420",&sMo_invest1.overflow_amt);
	  get_zd_double("0430",&sMo_invest1.abate_amt);
	if( pub_base_CompDblVal(sMo_invest.par_bal,0.00)==0
		&& pub_base_CompDblVal(sMo_invest.overflow_amt,0.00)==0
		&& pub_base_CompDblVal(sMo_invest.abate_amt,0.00)==0
		)
		strcpy(sMo_invest.sts,"*");
}
    /* 修改投资登记簿 */
    ret = Mo_invest_Dec_Upd(g_pub_tx.reply,"br_no='%s' and pact_no='%s' and sts='2'",g_pub_tx.tx_br_no,sMo_invest.pact_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"declare for update error [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    ret = Mo_invest_Fet_Upd(&sMo_invest1,g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"Fetch mo_invest error !! [%s][%s][%d]",g_pub_tx.tx_br_no,sMo_invest.pact_no,ret);
        WRITEMSG
        Mo_invest_Clo_Upd();
        goto ErrExit;
    }

    ret = Mo_invest_Upd_Upd(sMo_invest,g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"修改投资登记簿错误!! [%d]",ret);
        WRITEMSG
        Mo_invest_Clo_Upd();
        strcpy(g_pub_tx.reply,"B057");
        goto ErrExit;
    }

		ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_id=%ld",sMo_invest.ac_id);
		if( ret )
		{
			sprintf( acErrMsg, "查询公共帐号介质对照表出错[%d]!",ret );
			WRITEMSG
			goto ErrExit;
		}
    get_zd_data("0300",g_pub_tx.ac_no);      /* 转账账号 */
    if (flag[0] == '1')
    {
        /* 调用现金子交易 */
        goto OkExitCash;
    }
    else
    {
        /* 调用转帐子交易 */
        g_pub_tx.ac_id = 0;
        g_pub_tx.ac_seqn = 0;
        goto OkExit;
    }

OkExitCash:
    strcpy( g_pub_tx.reply, "0001" );
    sprintf(acErrMsg,"success!!");
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;


OkExit:
    strcpy( g_pub_tx.reply, "0002" );
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

