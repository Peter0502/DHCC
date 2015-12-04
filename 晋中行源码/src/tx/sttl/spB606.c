/*************************************************
* 文 件 名:  spB606.c
* 功能描述： 股权投资购入记账
*
* 作    者:  rob
* 完成日期： 2003年1月25日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "mo_righ_invest_c.h"
#include "com_parm_c.h"
#include "mbfe_pub.h"
#include "mo_city_c.h"

int spB606()
{
    struct mo_righ_invest_c sMo_righ_invest,sMo_righ_invest1;
    struct in_parm_c sIn_parm;
	struct com_parm_c	sCom_parm;
	struct mbfeghvsendjrn_c mbfeghvsendjrn_v;
	struct mo_city_c	mo_city_c;
    char flag[2],zzlx[2];
    int ret;
  
    memset(&sMo_righ_invest,0x0,sizeof(struct mo_righ_invest_c));
    memset(&sMo_righ_invest1,0x0,sizeof(struct mo_righ_invest_c));
    memset(&sIn_parm,0x0,sizeof(struct in_parm_c));
	memset(&sCom_parm,0x00,sizeof(struct com_parm_c));
	memset(&mbfeghvsendjrn_v,0x00,sizeof(struct mbfeghvsendjrn_c));
	memset(&mo_city_c,0x00,sizeof(struct mo_city_c));

    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"初始化公用结构错误!!");
        WRITEMSG
        goto ErrExit;
    }
    
    get_zd_data("0580",sMo_righ_invest.pact_no);    /* 投资协议编号 */
    get_zd_data("0680",sMo_righ_invest.invst_type); /* 股权投资类别 */
    get_zd_data("0810",sMo_righ_invest.name);       /* 被投资单位名称 */
    get_zd_double("0390",&sMo_righ_invest.amt); /* 买入价 */
    get_zd_long("0530",&sMo_righ_invest.num);    /* 股数量 */
    get_zd_char("0690",flag);                  /* 现转标志 */
 
    g_pub_tx.tx_amt1 = sMo_righ_invest.amt;
  
    ret = Mo_righ_invest_Sel(g_pub_tx.reply,&sMo_righ_invest1,"pact_no='%s'",
          sMo_righ_invest.pact_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"查询权益性投资登记簿错误!! [%d]",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"B045");
        goto ErrExit;
    }

    if (strcmp(sMo_righ_invest.invst_type,sMo_righ_invest1.invst_type) != 0)
    {
        sprintf(acErrMsg,"股权投资类别输入错误!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"B064");
        goto ErrExit;
    }

    if (pub_base_CompDblVal(sMo_righ_invest.amt,sMo_righ_invest1.amt) != 0)
    {
        sprintf(acErrMsg,"买入价输入错误!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"B065");
        goto ErrExit;
    }

    if (sMo_righ_invest.num != sMo_righ_invest1.num)
    {
        sprintf(acErrMsg,"股数量输入错误!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"B066");
        goto ErrExit;
    }

    strcpy(sMo_righ_invest1.name,pub_base_strpack(sMo_righ_invest1.name));
    if (strcmp(sMo_righ_invest.name,sMo_righ_invest1.name) != 0)
    {
        sprintf(acErrMsg,"被投资单位名称输入错误!![%s][%s]",
          sMo_righ_invest.name,sMo_righ_invest1.name);
        WRITEMSG
        strcpy(g_pub_tx.reply,"B067");
        goto ErrExit;
    }
    g_pub_tx.hst_ind[0] = '1';

    ret = In_parm_Sel(g_pub_tx.reply,&sIn_parm,"prdt_no='%s'",
                     sMo_righ_invest1.bkrl_code);
    if (ret != 0)
    {
        sprintf(acErrMsg,"查询内部产品参数错误!! [%d]",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"W035");
        goto ErrExit;
    }

    /* 开空介质 */
    strcpy(g_pub_tx.mdm_code,NULL_MDM_CODE);
    strcpy(g_pub_tx.cur_no,"01");
    strcpy(g_pub_tx.prdt_code,sMo_righ_invest1.bkrl_code);
	strcpy(g_pub_tx.brf,"权益性投资买入记账");

    if (pub_acct_opn_in_mdm())
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
    g_pub_tx.add_ind[0] = '1';
    if (pub_acct_opn_ac())
    {
        sprintf(acErrMsg,"开户出错");
        WRITEMSG
        goto ErrExit;
    }

    set_zd_data("0310",g_pub_tx.ac_no);

    sprintf(acErrMsg,"开户成功 PASS!!! ac_no=[%s] ",g_pub_tx.ac_no);
    WRITEMSG

    /* 修改权益性投资登记簿 */
    sMo_righ_invest1.ac_id = g_pub_tx.ac_id;
    sMo_righ_invest1.wrk_date = g_pub_tx.tx_date;
    strcpy(sMo_righ_invest1.sts,"2");

    ret = Mo_righ_invest_Dec_Upd(g_pub_tx.reply,"pact_no='%s'",
                  sMo_righ_invest1.pact_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"dec for update error [%d] ",ret);
        WRITEMSG
        goto ErrExit;
    }

    ret = Mo_righ_invest_Fet_Upd(&sMo_righ_invest,g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"fet for update error [%d]",ret);
        WRITEMSG
        Mo_righ_invest_Clo_Upd();
        goto ErrExit;
    }

    ret = Mo_righ_invest_Upd_Upd(sMo_righ_invest1,g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"修改权益性投资登记簿出错 !! [%d]",ret);
        WRITEMSG
        Mo_righ_invest_Clo_Upd();
        strcpy(g_pub_tx.reply,"B036");
        goto ErrExit;
    }
    Mo_righ_invest_Clo_Upd();
    sprintf(acErrMsg,"修改权益性投资登记簿  pass!! ");
    WRITEMSG


	get_zd_data("0700",zzlx);
	if ( zzlx[0]=='1' ) 			/*** 同城 ***/
	{
		strcpy(mo_city_c.payer_ac_no,g_pub_tx.ac_no);
		strcpy(mo_city_c.payer_name,g_pub_tx.name);
		get_zd_data("0300",mo_city_c.payee_ac_no);
		get_zd_data("0270",mo_city_c.payee_name);
		mo_city_c.tx_date = g_pub_tx.tx_date;
		mo_city_c.tx_no = g_pub_tx.trace_no;
		strcpy(mo_city_c.tx_br_no, g_pub_tx.tx_br_no);
		strcpy(mo_city_c.tel,g_pub_tx.tel);
		strcpy(mo_city_c.trans_mode,"1");
		strcpy(mo_city_c.in_out_ind,"1"); 	/* 提出 */
		strcpy(mo_city_c.cd_ind,"2");		/* 提出贷 */
		get_zd_double("0390",&mo_city_c.bill_amt);
		strcpy(mo_city_c.sts,"3");
		strcpy(mo_city_c.bill_type,"1"); /* 票据种类：同城 */
		mo_city_c.bat_no = 0;
		mo_city_c.no = 0;
		ret = Mo_city_Ins(mo_city_c, g_pub_tx.reply);
		if (ret != 0)
		{
			sprintf(acErrMsg,"登记同城登记簿错误!! [%d]",ret);
			WRITEMSG
			goto ErrExit;
		}
		ret = Com_parm_Sel( g_pub_tx.reply, &sCom_parm, "parm_code='TCKM' and parm_seqn='2'" );
		if( ret )
		{
			sprintf( acErrMsg, "查询公共参数表出错[%d]!",ret);
			WRITEMSG
			goto ErrExit;
		}
		set_zd_data("0300",sCom_parm.val);
	}
	else if( zzlx[0]=='2' )			/*** 大额 ***/
	{
		strcpy(mbfeghvsendjrn_v.tx_br_no,g_pub_tx.tx_br_no);
		strcpy(mbfeghvsendjrn_v.tx_code,g_pub_tx.tx_code);
		mbfeghvsendjrn_v.consigndate=g_pub_tx.tx_date;
		mbfeghvsendjrn_v.trace_no=g_pub_tx.trace_no;
		strcpy(mbfeghvsendjrn_v.procstate,"0");
		strcpy(mbfeghvsendjrn_v.payeracc,g_pub_tx.ac_no);
		get_zd_double("0390",&mbfeghvsendjrn_v.amount);
		strcpy(mbfeghvsendjrn_v.payername,g_pub_tx.name);

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

    if (flag[0] == '1')
    {
        /* 调用现金子交易 */
        goto OkExitCash;
    }
    else
    {
        /* 调用转帐子交易 */
    	get_zd_data("0300",g_pub_tx.ac_no);         /* 转账账号 */
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

