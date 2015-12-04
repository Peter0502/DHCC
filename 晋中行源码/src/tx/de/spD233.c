/***************************************************************
* 文 件 名: spD233.c
* 功能描述: 网点提入(手工)
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

spD233()
{
    int ret;
    int sum;
	double amt1,amt2;

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


    get_zd_long("0480",&mo_city_c.bat_no);
	/********
    get_zd_data("0670",mo_city_c.bill_type);
    get_zd_int("0500",&sum);
	********/
    get_zd_double("0390",&g_pub_tx.tx_amt1);

    strcpy(mo_city_c.in_out_ind,"2");

		/********
    if (mo_city_c.bill_type[0] == '6')
    {
        ret = sql_count("mo_city","payer_br_no='%s' and bat_no=%d and \
              sts='B'", g_pub_tx.tx_br_no, mo_city_c.bat_no);
        if (ret < 0)
        {
            sprintf(acErrMsg,"统计同城登记簿错误!! [%d]",ret);
            WRITEMSG
            goto ErrExit;
        }

        if (sum != ret)
        {
            sprintf(acErrMsg,"张数不正确!! [%d]",ret);
            WRITEMSG
            strcpy(g_pub_tx.reply,"D127");
            goto ErrExit;
        }
		***********/

        ret = sql_sum_double("mo_city","bill_amt",&amt1,
          	"payee_br_no='%s' and cd_ind='2' and bat_no = %d and sts='B'",
          	g_pub_tx.tx_br_no, mo_city_c.bat_no);
        if (ret != 0)
        {
            sprintf(acErrMsg,"统计同城登记簿金额错误!! [%d]",ret);
            WRITEMSG
			strcpy(g_pub_tx.reply,"D174");
            goto ErrExit;
        }

	/*********
        if (pub_base_CompDblVal(mo_city_c.bill_amt, g_pub_tx.tx_amt1) != 0)
        {
            sprintf(acErrMsg,"金额不正确!! 输入金额=[%lf] 汇总金额=[%lf]",
               g_pub_tx.tx_amt1, mo_city_c.bill_amt);
            WRITEMSG
            strcpy(g_pub_tx.reply,"D128");
            goto ErrExit;
        }

        ret = sql_execute("update mo_city set sts='C' where payer_br_no='%s' \
              and bat_no = %d and bill_type='%s' and sts='B'",
              g_pub_tx.tx_br_no,mo_city_c.bat_no, mo_city_c.bill_type);
        if (ret != 0)
        {
            sprintf(acErrMsg,"更新同城登记簿的状态错误!! [%d]",ret);
            WRITEMSG
            goto ErrExit;
        }
    }
    else
    {
        ret = sql_count("mo_city","payee_br_no='%s' and bat_no=%d and \
              bill_type='%s' and sts='B'", g_pub_tx.tx_br_no, mo_city_c.bat_no, 
              mo_city_c.bill_type);
        if (ret < 0)
        {
            sprintf(acErrMsg,"统计同城登记簿错误!! [%d]",ret);
            WRITEMSG
            goto ErrExit;
        }

        if (sum != ret)
        {
            sprintf(acErrMsg,"张数不正确!! [%d]",ret);
            WRITEMSG
            strcpy(g_pub_tx.reply,"D127");
            goto ErrExit;
        }
	***********/

	sprintf(acErrMsg,"提出贷方金额为:[%lf]",amt1);
	WRITEMSG

        ret = sql_sum_double("mo_city","bill_amt",&amt2, "payer_br_no='%s' \
				and bat_no = %d and cd_ind='1' and sts='B'",
          		g_pub_tx.tx_br_no, mo_city_c.bat_no);
        if (ret != 0)
        {
            sprintf(acErrMsg,"统计同城登记簿金额错误!! [%d]",ret);
            WRITEMSG
			strcpy(g_pub_tx.reply,"D175");
            goto ErrExit;
        }

	sprintf(acErrMsg,"提出借方金额为:[%lf]",amt2);
	WRITEMSG

		if (pub_base_CompDblVal(amt1,amt2) >= 0)
		{
			mo_city_c.bill_amt = amt1 - amt2;
		}
		else if (pub_base_CompDblVal(amt1,amt2) < 0)
		{
			mo_city_c.bill_amt = amt2 - amt1;
		}

	sprintf(acErrMsg,"汇总金额为:[%lf]",mo_city_c.bill_amt);
	WRITEMSG

        if (pub_base_CompDblVal(mo_city_c.bill_amt, g_pub_tx.tx_amt1) != 0)
        {
            sprintf(acErrMsg,"金额不正确!! 输入金额=[%lf] 汇总金额=[%lf]",
               g_pub_tx.tx_amt1, mo_city_c.bill_amt);
            WRITEMSG
            strcpy(g_pub_tx.reply,"D128");
            goto ErrExit;
        }

        ret = sql_execute("update mo_city set sts='C' where bat_no = %d \
				and sts='B'",mo_city_c.bat_no);
        if (ret != 0)
        {
            sprintf(acErrMsg,"更新同城登记簿的状态错误!! [%d]",ret);
            WRITEMSG
            goto ErrExit;
        }

	/*** 登记交易流水 ***/
	strcpy(g_pub_tx.brf, "手工提入网点核销");
	strcpy(g_pub_tx.no_show, "1");
	g_pub_tx.ac_seqn = mo_city_c.bat_no;
	if( pub_ins_trace_log() )
	{
		sprintf(acErrMsg,"登记交易流水错误!");
		WRITEMSG
		goto ErrExit;
	}
	
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"网点提入(核销)处理成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"网点提入(核销)处理失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

