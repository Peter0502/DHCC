/***************************************************************
* 文 件 名: spD221.c
* 功能描述: 网点录入(自动)
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

spD221()
{
    int ret;

    struct mo_city_c mo_city_c,mo_city_c1;
    struct com_parm_c com_parm_c;

    memset(&g_pub_tx, 0x0, sizeof(g_pub_tx));
    memset(&mo_city_c, 0x0, sizeof(struct mo_city_c));
    memset(&mo_city_c1, 0x0, sizeof(struct mo_city_c));
    memset(&com_parm_c, 0x0, sizeof(struct com_parm_c));

	/*** 初始化全局变量 ***/
	if(pub_base_sysinit())
	{
		strcpy(acErrMsg,"初始化全局变量出错!");
		WRITEMSG
		goto ErrExit;
	}

    get_zd_data("0300",mo_city_c.payer_ac_no);
    get_zd_data("0250",mo_city_c.payer_name);
    get_zd_data("0270",mo_city_c.payer_br_name);
    get_zd_data("0310",mo_city_c.payee_ac_no);
    get_zd_data("0260",mo_city_c.payee_name);
    get_zd_data("0830",mo_city_c.payee_br_name);
    get_zd_double("0390",&mo_city_c.bill_amt);
	/*******
    get_zd_data("0670",mo_city_c.bill_type);
    get_zd_data("0580",mo_city_c.bill_no);
	********/
    get_zd_data("0680",mo_city_c.bill_type);
    strcpy(mo_city_c.cd_ind,"2");
    get_zd_data("0810",mo_city_c.brf);
    get_zd_data("0820",mo_city_c.remark);

    strcpy(mo_city_c.in_out_ind,"1");
	strcpy(mo_city_c.cd_ind,"2");

    /* 只有提出贷方 */
    ret = Mo_city_Dec_Upd(g_pub_tx.reply,"tel='%s'and payer_ac_no='%s' and \
          in_out_ind='1' and cd_ind='2' and bill_amt=%lf and sts='0'",
          g_pub_tx.tel, mo_city_c.payer_ac_no, mo_city_c.bill_amt);
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR !! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    ret = Mo_city_Fet_Upd(&mo_city_c1,g_pub_tx.reply);
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
 
    if (mo_city_c1.tx_date != g_pub_tx.tx_date)
    {
        sprintf(acErrMsg,"不是当天交易不准录入!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"D123");
        goto ErrExit;
    }

    if (strcmp(mo_city_c1.tx_br_no,g_pub_tx.tx_br_no) != 0)
    {
        sprintf(acErrMsg,"不是同一个机构不允许做该笔业务!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"D124");
        goto ErrExit;
    }

    if (strcmp(mo_city_c1.tel,g_pub_tx.tel) != 0)
    {
        sprintf(acErrMsg,"该柜员不允许做该笔业务!! [%s]",g_pub_tx.tel);
        WRITEMSG
        strcpy(g_pub_tx.reply,"D125");
        goto ErrExit;
    }

    strcpy(mo_city_c1.trans_mode,"2");
    strcpy(mo_city_c1.bill_type,mo_city_c.bill_type);
    strcpy(mo_city_c1.payer_ac_no,mo_city_c.payer_ac_no);
    strcpy(mo_city_c1.payer_name,mo_city_c.payer_name);
    strcpy(mo_city_c1.payer_br_name,mo_city_c.payer_br_name);
    strcpy(mo_city_c1.payee_ac_no,mo_city_c.payee_ac_no);
    strcpy(mo_city_c1.payee_name,mo_city_c.payee_name);
    strcpy(mo_city_c1.payee_br_name,mo_city_c.payee_br_name);
    /* strcpy(mo_city_c1.bill_no,mo_city_c.bill_no); */
    strcpy(mo_city_c1.brf,mo_city_c.brf);
    strcpy(mo_city_c1.remark,mo_city_c.remark);
    strcpy(mo_city_c1.sts,"6");
vtcp_log("AAASSS");
	g_pub_tx.ac_seqn = mo_city_c1.tx_no%30000;
vtcp_log("AAAAA[%d]",g_pub_tx.ac_seqn);
vtcp_log("AAAAB[%d]",g_pub_tx.svc_ind);
    mo_city_c1.tx_no = g_pub_tx.trace_no;

    /* 生成批次号 */
    ret = sql_max_int("mo_city","bat_no","trans_mode='2' and \
			in_out_ind='1'",g_pub_tx.tx_date);
    if (ret < 0)
    {
        sprintf(acErrMsg,"查询同城登记簿!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    mo_city_c1.bat_no = ret + 1;

    /* 检查该批次号是否已存在并且状态为6的 
    while(1)
    {
        ret = sql_count("mo_city","bat_no=%d and sts='6'",mo_city_c1.bat_no);
        if (ret < 0)
        {
            sprintf(acErrMsg,"统计同城登记簿错误!! [%d]",ret);
            WRITEMSG
            goto ErrExit;
        }
        else if (ret > 0)
        {
            mo_city_c1.bat_no += 1;
            continue;
        }
        else
        {
            break;
        }
    }
	*/

    ret = Mo_city_Upd_Upd(mo_city_c1, g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"UPDATE ERROR !! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }
    Mo_city_Clo_Upd();

TRACE
    ret=Com_parm_Sel(g_pub_tx.reply,&com_parm_c,"parm_code='TCKM' and \
        parm_seqn=2");
    if (ret != 0)
    {
        sprintf(acErrMsg,"查询公共参数表错误!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

	/* 贷方 40801*/
    set_zd_data("1211",com_parm_c.val);
    set_zd_double("1218",mo_city_c.bill_amt);
    set_zd_data("1215","2");
    set_zd_data("121A","自动网点录入记帐");

    memset(&com_parm_c,0x0,sizeof(struct com_parm_c));
    ret=Com_parm_Sel(g_pub_tx.reply,&com_parm_c,"parm_code='TCKM' and \
        parm_seqn=1");
    if (ret != 0)
    {
        sprintf(acErrMsg,"查询公共参数表错误!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }
	/* 借方 26102*/
    set_zd_data("1201",com_parm_c.val);
    set_zd_double("1208",mo_city_c.bill_amt);
    set_zd_data("1205","2");
    set_zd_data("120A","自动网点录入记帐");
    
	/*** 登记交易流水 ***/
	strcpy(g_pub_tx.brf, "自动提出录入");
	strcpy(g_pub_tx.no_show, "1");
	g_pub_tx.tx_amt1=0.00;	
vtcp_log("AAAAA[%d]",g_pub_tx.ac_seqn);
vtcp_log("AAAAB[%d]",g_pub_tx.svc_ind);
	if( pub_ins_trace_log() )
	{
		sprintf(acErrMsg,"登记交易流水错误!");
		WRITEMSG
		goto ErrExit;
	}
	
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"网点录入(自动)处理成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"网点录入(自动)处理失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

