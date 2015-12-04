/***************************************************************
* 文 件 名: spD213.c
* 功能描述: 总行提出(手工)
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
#include "com_tx_br_rel_c.h"

spD213()
{
    int ret=0;
    int sum=0;
    char br_no[6];
	double d_amt=0.00, c_amt=0.00;

    struct mo_city_c mo_city_c;
    struct com_parm_c com_parm_c;
    struct com_tx_br_rel_c com_tx_br_rel_c;

    memset(&mo_city_c, 0x0, sizeof(struct mo_city_c));
    memset(&com_parm_c, 0x0, sizeof(struct com_parm_c));
    memset(&com_tx_br_rel_c, 0x0, sizeof(struct com_tx_br_rel_c));
	memset(br_no, 0x0, sizeof(br_no));

	/*** 初始化全局变量 ***/
	if(pub_base_sysinit())
	{
		strcpy(acErrMsg,"初始化全局变量出错!");
		WRITEMSG
		goto ErrExit;
	}

    memset(br_no, 0x0, sizeof(br_no));

    get_zd_data("0910",br_no);
    get_zd_long("0480",&mo_city_c.bat_no);
    /* get_zd_data("0670",mo_city_c.bill_type); */
    get_zd_int("0500",&sum);
    get_zd_double("0390",&g_pub_tx.tx_amt1);

    strcpy(mo_city_c.in_out_ind,"1");
    
    /* 检查该交易机构是否是该网点的上级机构 */
    ret = Com_tx_br_rel_Sel(g_pub_tx.reply,&com_tx_br_rel_c,"rel_type=3 \
              and br_no='%s'", br_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"查询交易机构关系表错误!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    if (strcmp(com_tx_br_rel_c.up_br_no,g_pub_tx.tx_br_no) != 0)
    {
        /* 该机构不是该笔业务的上级机构不能打印 */
        sprintf(acErrMsg,"bat_no=[%d],br_no=[%s]",mo_city_c.bat_no, br_no);
        WRITEMSG
        strcpy(g_pub_tx.reply,"D144");
        goto ErrExit;
    }

	/*******
    ret = sql_count("mo_city","tx_br_no='%s' and bat_no=%d and sts='2'", 
			br_no, mo_city_c.bat_no);
    if (ret < 0)
    {
        sprintf(acErrMsg,"统计同城登记簿错误!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }
	else if (ret == 0)
	{
		sprintf(acErrMsg,"没有该类票据!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"D160");
		goto ErrExit;
	}

    if (sum != ret)
    {
        sprintf(acErrMsg,"张数不正确!! [%d]",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"D127");
        goto ErrExit;
    }
	******/

	d_amt = 0.00;
   	ret = sql_sum_double("mo_city","bill_amt",&d_amt, 
		"tx_br_no='%s' and bat_no = %d and sts='2' and trans_mode='1' \
		and in_out_ind='1' and cd_ind='2'", br_no, mo_city_c.bat_no);
   	if (ret != 0)
   	{
       	sprintf(acErrMsg,"统计同城登记簿金额错误!! [%d]",ret);
       	WRITEMSG
       	goto ErrExit;
   	}

	sprintf(acErrMsg, "提出贷方金额为: [%lf]", d_amt);
	WRITEMSG

	c_amt = 0.00;
   	ret = sql_sum_double("mo_city","bill_amt",&c_amt, 
		"tx_br_no='%s' and bat_no = %d and sts='2' and trans_mode='1' \
		and in_out_ind='1' and cd_ind='1'", br_no, mo_city_c.bat_no);
   	if (ret != 0)
   	{
       	sprintf(acErrMsg,"统计同城登记簿金额错误!! [%d]",ret);
       	WRITEMSG
       	goto ErrExit;
   	}

	sprintf(acErrMsg, "提出借方金额为: [%lf]", c_amt);
	WRITEMSG

	mo_city_c.bill_amt = d_amt - c_amt;
   	if (pub_base_CompDblVal(mo_city_c.bill_amt, g_pub_tx.tx_amt1) != 0)
   	{
       	sprintf(acErrMsg,"金额不正确!! 输入金额=[%lf] 汇总金额=[%lf]",
               g_pub_tx.tx_amt1, mo_city_c.bill_amt);
       	WRITEMSG
       	strcpy(g_pub_tx.reply,"D128");
    	goto ErrExit;
   	}

    ret = sql_execute("update mo_city set sts='3' , tc_date=%ld where \
		tx_br_no='%s' and bat_no = %d and sts='2' and trans_mode='1' and \
		in_out_ind='1'", g_pub_tx.tx_date, br_no, mo_city_c.bat_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"更新同城登记簿的状态错误!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

	/*** 登记交易流水 ***/
	strcpy(g_pub_tx.brf, "手工提出票据核销");
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
	sprintf(acErrMsg,"总行提出处理成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"总行提出处理失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

