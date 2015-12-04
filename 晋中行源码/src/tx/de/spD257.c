/***************************************************************
* 文 件 名: spD251.c
* 功能描述: 网点提入记帐(记客户帐-存款)
*
* 作    者: jack
* 完成日期: 2004年04月01日
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

spD251()
{
    int ret;
	char flag[2];
	char note_type[2];
	char ac_no[20];

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

    get_zd_data("0690",flag);
	if (flag[0] == '1')
	{
    	get_zd_data("0300",g_pub_tx.ac_no);
		strcpy(ac_no, g_pub_tx.ac_no);
	}
	else
	{
    	get_zd_data("0310",g_pub_tx.ac_no);
		get_zd_data("0300", ac_no);
	}

    get_zd_double("0390",&g_pub_tx.tx_amt1);
    get_zd_data("0670",note_type);
    /* get_zd_data("0580",mo_city_c.bill_no); */

    strcpy(mo_city_c.in_out_ind,"2");
    
    
    ret = Mo_city_Dec_Upd(g_pub_tx.reply,"payee_ac_no='%s' and in_out_ind='2'\
          and cd_ind='2' and bill_amt=%lf and sts in ('C', 'I')",
		  ac_no,g_pub_tx.tx_amt1);
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR !! [%d]",ret);
        WRITEMSG
        goto ErrExit;
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

    if (mo_city_c.sts[0]=='O' || mo_city_c.sts[0] == 'P')
    {
        sprintf(acErrMsg,"该票已经记帐!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"D152");
        goto ErrExit;
    }

    if (strcmp(mo_city_c.payee_br_no,g_pub_tx.tx_br_no) != 0)
    {
        sprintf(acErrMsg,"该网点不允许做该笔业务!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"D151");
        goto ErrExit;
    }


    get_zd_data("0810",mo_city_c.brf);
    get_zd_data("0820",mo_city_c.remark);
    strcpy(mo_city_c.sts,"O");
	mo_city_c.tc_date = g_pub_tx.tx_date;

    ret = Mo_city_Upd_Upd(mo_city_c, g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"UPDATE ERROR !! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }
    Mo_city_Clo_Upd();

    /* 客户帐户存款 */
    g_pub_tx.ac_seqn = 0;
    set_zd_int("0350",g_pub_tx.ac_seqn);
	/* set_zd_data("0300",ac_no); */

    ret=Com_parm_Sel(g_pub_tx.reply,&com_parm_c,"parm_code='TCKM' and \
        parm_seqn=3");
    if (ret != 0)
    {
        sprintf(acErrMsg,"查询公共参数表错误!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

        set_zd_data("1201",com_parm_c.val);
        set_zd_double("1208",g_pub_tx.tx_amt1);
        set_zd_data("1205","2");
        set_zd_data("120A","网点提入记帐");

	/*** 登记交易流水 ***/
	strcpy(g_pub_tx.brf, "网点提入记帐");
	g_pub_tx.ac_id = mo_city_c.tx_no;
	g_pub_tx.tx_amt1 = mo_city_c.tx_date;
	strcpy(g_pub_tx.no_show, "1");
	if( pub_ins_trace_log() )
	{
		sprintf(acErrMsg,"登记交易流水错误!");
		WRITEMSG
		goto ErrExit;
	}
	
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"网点提入记帐处理成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"网点提入记帐处理失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

