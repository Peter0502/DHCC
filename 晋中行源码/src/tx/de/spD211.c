/***************************************************************
* 文 件 名: spD211.c
* 功能描述: 网点票据录入(手工)
*
* 作    者: jack
* 完成日期: 2004年3月30日
*
* 修改记录：
*    日    期:
*    修 改 人:
*    修改内容:
*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "mo_city_c.h"
#include "com_parm_c.h"

spD211()
{
    int ret;

    struct mo_city_c mo_city_c;
    struct com_parm_c com_parm_c;

    memset(&mo_city_c, 0x0, sizeof(struct mo_city_c));
    memset(&com_parm_c, 0x0, sizeof(struct com_parm_c));

	/*** 初始化全局变量 ***/
	if(pub_base_sysinit())
	{
		strcpy(acErrMsg,"初始化全局变量出错!");
		WRITEMSG
		goto ErrExit;
	}

    get_zd_double("0390",&g_pub_tx.tx_amt1);
    get_zd_data("0680",mo_city_c.cd_ind);

    strcpy(mo_city_c.in_out_ind,"1");
    
    
    if (mo_city_c.cd_ind[0] == '2')
    {
    	get_zd_data("0300",g_pub_tx.ac_no);
		sprintf(acErrMsg,"提出贷方:ac_no= [%s], br_no=[%s]",
				g_pub_tx.ac_no, g_pub_tx.tx_br_no);
		WRITEMSG

        ret = Mo_city_Dec_Upd(g_pub_tx.reply,"payer_ac_no='%s' and \
              in_out_ind='1' and cd_ind='%s' and bill_amt=%lf and sts='0' \
			  and tx_br_no='%s'", g_pub_tx.ac_no, mo_city_c.cd_ind, 
			  g_pub_tx.tx_amt1, g_pub_tx.tx_br_no);
        if (ret != 0)
        {
            sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR !! [%d]",ret);
            WRITEMSG
            goto ErrExit;
        }
    }
    else
    {
    	get_zd_data("0310",g_pub_tx.ac_no);
		sprintf(acErrMsg,"提出借方:ac_no= [%s], br_no=[%s]",
				g_pub_tx.ac_no, g_pub_tx.tx_br_no);
		WRITEMSG

        ret = Mo_city_Dec_Upd(g_pub_tx.reply,"payee_ac_no='%s' and \
              in_out_ind='1' and cd_ind='%s' and bill_amt=%lf and sts='0' \
			  and tx_br_no='%s'", g_pub_tx.ac_no, mo_city_c.cd_ind, 
			  g_pub_tx.tx_amt1, g_pub_tx.tx_br_no);
        if (ret != 0)
        {
            sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR !! [%d]",ret);
            WRITEMSG
            goto ErrExit;
        }
    }

    ret = Mo_city_Fet_Upd(&mo_city_c,g_pub_tx.reply);
    if (ret != 0 && ret != 100)
    {
        sprintf(acErrMsg,"查询同城登记簿错误!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }
    else if (ret == 100)
    {
       	sprintf(acErrMsg,"同城登记簿中没有登记该笔业务! [%d]",ret);
       	WRITEMSG
       	strcpy(g_pub_tx.reply,"D122");
       	goto ErrExit;
    }
 
    if (mo_city_c.tx_date != g_pub_tx.tx_date)
    {
        sprintf(acErrMsg,"不是当天交易不准录入!! [%d] [%d]",
				mo_city_c.tx_date, g_pub_tx.tx_date);
        WRITEMSG
        strcpy(g_pub_tx.reply,"D123");
        goto ErrExit;
    }

    if (strcmp(mo_city_c.tx_br_no,g_pub_tx.tx_br_no) != 0)
    {
        sprintf(acErrMsg,"不是同一个机构不允许做该笔业务!! [%s],[%s]",
				mo_city_c.tx_br_no, g_pub_tx.tx_br_no);
        WRITEMSG
        strcpy(g_pub_tx.reply,"D124");
        goto ErrExit;
    }

    if (strcmp(mo_city_c.tel,g_pub_tx.tel) != 0)
    {
        sprintf(acErrMsg,"该柜员不允许做该笔业务!! [%s]",g_pub_tx.tel);
        WRITEMSG
        strcpy(g_pub_tx.reply,"D125");
        goto ErrExit;
    }

    strcpy(mo_city_c.trans_mode,"1");
    get_zd_data("0670",mo_city_c.bill_type);
	/*******
    get_zd_data("0580",mo_city_c.bill_no);
	******/
    get_zd_data("0310",mo_city_c.payee_ac_no);
    get_zd_data("0300",mo_city_c.payer_ac_no);
    if (mo_city_c.cd_ind[0] == '2')
    {
        get_zd_data("0250",mo_city_c.payer_name);
    }
    else
    {
        get_zd_data("0250",mo_city_c.payee_name);
    }
	/******
    get_zd_data("0810",mo_city_c.brf);
    get_zd_data("0820",mo_city_c.remark);
	******/
    strcpy(mo_city_c.sts,"1");
	g_pub_tx.ac_id = mo_city_c.tx_no;
    mo_city_c.tx_no = g_pub_tx.trace_no;


    ret = Mo_city_Upd_Upd(mo_city_c, g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"UPDATE ERROR !! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }
    Mo_city_Clo_Upd();

TRACE
    ret=Com_parm_Sel(g_pub_tx.reply,&com_parm_c,"parm_code='TCKM' and \
        parm_seqn=1");
    if (ret != 0)
    {
        sprintf(acErrMsg,"查询公共参数表错误!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    if (mo_city_c.cd_ind[0] == '2')
    {
		/* 借方 26102*/
        set_zd_data("1201",com_parm_c.val);
        set_zd_double("1208",g_pub_tx.tx_amt1);
        set_zd_data("1205","2");
        set_zd_data("120A","手工录入记帐");
    }
    else
    {
		/* 贷方 */
        set_zd_data("1211",com_parm_c.val);
        set_zd_double("1218",g_pub_tx.tx_amt1);
        set_zd_data("1215","2");
        set_zd_data("121A","手工录入记帐");
    }

    memset(&com_parm_c,0x0,sizeof(struct com_parm_c));
    ret=Com_parm_Sel(g_pub_tx.reply,&com_parm_c,"parm_code='TCKM' and \
        parm_seqn=2");
    if (ret != 0)
    {
        sprintf(acErrMsg,"查询公共参数表错误!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }
    if (mo_city_c.cd_ind[0] == '2')
    {
        set_zd_data("1211",com_parm_c.val);
        set_zd_double("1218",g_pub_tx.tx_amt1);
        set_zd_data("1215","2");
        set_zd_data("121A","手工录入记帐");
    }
    else
    {
        set_zd_data("1201",com_parm_c.val);
        set_zd_double("1208",g_pub_tx.tx_amt1);
        set_zd_data("1205","2");
        set_zd_data("120A","手工录入记帐");
    }
    
	strcpy(g_pub_tx.ac_no, mo_city_c.payer_ac_no);
	strcpy(g_pub_tx.no_show, "1");
	strcpy(g_pub_tx.brf, "手工提出票据录入");

	/*** 登记交易流水 ***/
	if( pub_ins_trace_log() )
	{
		sprintf(acErrMsg,"登记交易流水错误!");
		WRITEMSG
		goto ErrExit;
	}
	
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"网点票据录入(手工)处理成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"网点票据录入(手工)处理失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

