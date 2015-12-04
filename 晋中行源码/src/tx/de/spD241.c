/***************************************************************
* 文 件 名: spD241.c
* 功能描述: 中心录入(自动)
*
* 作    者: jack
* 完成日期: 2004年3月31日
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
#include "com_tx_br_rel_c.h"

spD241()
{
    int ret;
	char flag[2];

    struct mo_city_c mo_city_c;
    struct com_parm_c com_parm_c;
    struct com_tx_br_rel_c com_tx_br_rel_c;
    struct mdm_ac_rel_c mdm_ac_rel_c;

    memset(&mo_city_c, 0x0, sizeof(struct mo_city_c));
    memset(&com_parm_c, 0x0, sizeof(struct com_parm_c));
    memset(&com_tx_br_rel_c, 0x0, sizeof(struct com_tx_br_rel_c));
    memset(&mdm_ac_rel_c, 0x0, sizeof(struct mdm_ac_rel_c));

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
    get_zd_data("0670",mo_city_c.bill_type);
	/***
    get_zd_data("0580",mo_city_c.bill_no);
    get_zd_data("0680",mo_city_c.cd_ind);
	****/
	strcpy(mo_city_c.cd_ind,"2");
    get_zd_data("0810",mo_city_c.brf);
    get_zd_data("0820",mo_city_c.remark);
	get_zd_data("0690",flag);
	get_zd_data("0910",mo_city_c.payee_br_no);

	sprintf(acErrMsg, " -------> [%s]", mo_city_c.payee_br_no);
	WRITEMSG
    strcpy(mo_city_c.in_out_ind,"2");
    strcpy(mo_city_c.trans_mode,"2");

	/********
    if (flag[0] == '0')
	{
    	ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&mdm_ac_rel_c,"ac_no='%s'",
          	mo_city_c.payee_ac_no);
    	if (ret != 0)
    	{
        	sprintf(acErrMsg,"查询介质账号关系表错误!! [%d]",ret);
        	WRITEMSG
        	goto ErrExit;
    	}
	}
 
    strcpy(mo_city_c.payee_br_no,mdm_ac_rel_c.opn_br_no);
	*********/

    /* 判断交易机构是否是该网点的上级机构 */
    ret = Com_tx_br_rel_Sel(g_pub_tx.reply,&com_tx_br_rel_c,"rel_type=3 \
		and br_no='%s'", mo_city_c.payee_br_no);
    if (ret != 0 && ret != 100)
    {
        sprintf(acErrMsg,"查询交易机构关系表错误!! [%d] [%s]",
				ret, mo_city_c.payee_br_no);
        WRITEMSG
		strcpy(g_pub_tx.reply,"D167");
        goto ErrExit;
    }
	else if (ret == 100)
	{
        sprintf(acErrMsg,"交易机构关系表中没有定义该网点的票据传递的\
				上级机构!! [%d] [%s]",ret, mo_city_c.payee_br_no);
        WRITEMSG
		strcpy(g_pub_tx.reply,"D168");
        goto ErrExit;
	}

    ret = Com_tx_br_rel_Sel(g_pub_tx.reply,&com_tx_br_rel_c,"rel_type=3 \
		and up_br_no='%s'", g_pub_tx.tx_br_no);
    if (ret != 0 && ret != 100)
    {
        sprintf(acErrMsg,"查询交易机构关系表错误!! [%d] [%s]",
				ret, g_pub_tx.tx_br_no);
        WRITEMSG
		strcpy(g_pub_tx.reply,"D167");
        goto ErrExit;
    }
	else if (ret == 100)
	{
        sprintf(acErrMsg,"不是中心不能做录入!! [%s]", g_pub_tx.tx_br_no);
        WRITEMSG
		strcpy(g_pub_tx.reply,"D241");
        goto ErrExit;
	}

	/**** delete by rob at 20030819**
    if (strcmp(com_tx_br_rel_c.up_br_no,g_pub_tx.tx_br_no) != 0)
    {
        * 该机构不是该笔业务的上级机构不能打印 *
        sprintf(acErrMsg,"bat_no=[%d],br_no=[%s]",mo_city_c.bat_no,
                mdm_ac_rel_c.opn_br_no);
        WRITEMSG
        strcpy(g_pub_tx.reply,"D144");
        goto ErrExit;
    }
	*****/

    strcpy(mo_city_c.sts,"H");
    mo_city_c.tx_date = g_pub_tx.tx_date;
    mo_city_c.tx_no = g_pub_tx.trace_no;
    strcpy(mo_city_c.tx_br_no, g_pub_tx.tx_br_no);
    strcpy(mo_city_c.tel,g_pub_tx.tel);

    /* 生成批次号 */
    ret = sql_max_int("mo_city","bat_no","in_out_ind='2' and \
			trans_mode='2'",g_pub_tx.tx_date);
    if (ret < 0)
    {
        sprintf(acErrMsg,"查询同城登记簿!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    mo_city_c.bat_no = ret + 1;

    /* 检查该批次号是否已存在并且状态为6的 
    while(1)
    {
        ret = sql_count("mo_city","bat_no=%d and sts in ('H','I')",
              mo_city_c.bat_no);
        if (ret < 0)
        {
            sprintf(acErrMsg,"统计同城登记簿错误!! [%d]",ret);
            WRITEMSG
            goto ErrExit;
        }
        else if (ret > 0)
        {
            mo_city_c.bat_no += 1;
            continue;
        }
        else
        {
            break;
        }
    }*/

    ret = Mo_city_Ins(mo_city_c, g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"INSERT ERROR !! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    ret=Com_parm_Sel(g_pub_tx.reply,&com_parm_c,"parm_code='TCKM' and \
        parm_seqn=3");
    if (ret != 0)
    {
        sprintf(acErrMsg,"查询公共参数表错误!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    /* 贷408内部清算科目 */
    set_zd_data("1211",com_parm_c.val);
    set_zd_double("1218",mo_city_c.bill_amt);
    set_zd_data("1215","2");
    set_zd_data("121A","自动中心录入(提入)");

    memset(&com_parm_c,0x0,sizeof(struct com_parm_c));
    ret=Com_parm_Sel(g_pub_tx.reply,&com_parm_c,"parm_code='TCKM' and \
        parm_seqn=2");
    if (ret != 0)
    {
        sprintf(acErrMsg,"查询公共参数表错误!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    /* 借408外部清算科目 */
    set_zd_data("1201",com_parm_c.val);
    set_zd_double("1208",mo_city_c.bill_amt);
    set_zd_data("1205","2");
    set_zd_data("120A","自动中心录入(提入)");
    
	/*** 登记交易流水 ***/
	strcpy(g_pub_tx.brf, "自动提入录入");
	strcpy(g_pub_tx.no_show, "1");

	if( pub_ins_trace_log() )
	{
		sprintf(acErrMsg,"登记交易流水错误!");
		WRITEMSG
		goto ErrExit;
	}
	
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"中心录入(自动提入)处理成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"中心录入(自动提入)处理失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

