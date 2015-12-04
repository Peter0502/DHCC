/***************************************************************
* 文 件 名: spD253.c
* 功能描述: 网点提入退票
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

spD253()
{
    int ret;
	char br_no[6];

    struct mo_city_c mo_city_c, sMo_city;
    struct com_parm_c com_parm_c;
    struct mdm_ac_rel_c mdm_ac_rel_c;

    memset(&mo_city_c, 0x0, sizeof(struct mo_city_c));
    memset(&com_parm_c, 0x0, sizeof(struct com_parm_c));
    memset(&mdm_ac_rel_c, 0x0, sizeof(struct mdm_ac_rel_c));
	memset(&sMo_city, 0x0, sizeof(struct mo_city_c));

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
    get_zd_data("0680",mo_city_c.cd_ind);

    strcpy(mo_city_c.in_out_ind,"2");
    strcpy(mo_city_c.trans_mode,"2");

    /******* 可能由于账号不正确才退票
    ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&mdm_ac_rel_c,"ac_no='%s'",
          mo_city_c.payee_ac_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"查询介质账号关系表错误!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }
 
    strcpy(mo_city_c.payee_br_no,mdm_ac_rel_c.opn_br_no);


    if (strcmp(mdm_ac_rel_c.opn_br_no,g_pub_tx.tx_br_no) != 0)
    {
        sprintf(acErrMsg,"该机构不允许做该票据的业务[%s]",
                mdm_ac_rel_c.opn_br_no);
        WRITEMSG
        strcpy(g_pub_tx.reply,"D154");
        goto ErrExit;
    }
	***********/

	sprintf(acErrMsg, "ac_no = '%s' , bill_amt= %lf", mo_city_c.payee_ac_no, mo_city_c.bill_amt);
	WRITEMSG

    ret = Mo_city_Dec_Upd(g_pub_tx.reply,"payee_ac_no='%s' and bill_amt=%lf \
          and (sts = 'C' or sts = 'I') and in_out_ind = '2' and \
		  cd_ind='%s'", mo_city_c.payee_ac_no, mo_city_c.bill_amt,
		  mo_city_c.cd_ind);
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR ! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    ret = Mo_city_Fet_Upd(&mo_city_c, g_pub_tx.reply);
    if (ret != 0 && ret != 100)
    {
        sprintf(acErrMsg,"FETCH FOR UPDATE ERROR !! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }
    else if (ret == 100)
    {
        sprintf(acErrMsg,"没有该票据!! [%d]",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"D155");
        goto ErrExit;
    }

	if (mo_city_c.cd_ind[0] == '2')
	{
		strcpy(br_no, mo_city_c.payee_br_no);
	}
	else
	{
		strcpy(br_no, mo_city_c.payer_br_no);
	}

    if (strcmp(br_no,g_pub_tx.tx_br_no) != 0)
    {
        sprintf(acErrMsg,"该机构不允许做该票据的业务[%s]",
                mdm_ac_rel_c.opn_br_no);
        WRITEMSG
        strcpy(g_pub_tx.reply,"D154");
        goto ErrExit;
    }

    strcpy(mo_city_c.sts,"Q");
	mo_city_c.tc_date = g_pub_tx.tx_date;

    ret = Mo_city_Upd_Upd(mo_city_c, g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"UPDATE MO_CITY ERROR !! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }
	Mo_city_Clo_Upd();

	sMo_city.tx_date = g_pub_tx.tx_date;
	sMo_city.tx_no = g_pub_tx.trace_no;
	strcpy(sMo_city.tx_br_no, g_pub_tx.tx_br_no);
	strcpy(sMo_city.tel, g_pub_tx.tel);
	strcpy(sMo_city.trans_mode, mo_city_c.trans_mode);
	sMo_city.bat_no = 0;
	sMo_city.no = 0;
	strcpy(sMo_city.in_out_ind, "1");
	strcpy(sMo_city.cd_ind, mo_city_c.cd_ind);
	if (sMo_city.trans_mode[0] == '1')
	{
		strcpy(sMo_city.bill_type, "6");
		strcpy(sMo_city.sts, "1");
	}
	else
	{
		strcpy(sMo_city.bill_type, "3");
		strcpy(sMo_city.sts, "6");

		/* 生成批次号 */
		ret = sql_max_int("mo_city","bat_no","trans_mode='2' and \
				in_out_ind='1'");
		if (ret < 0)
		{ 
			sprintf(acErrMsg,"查询同城登记簿!! [%d]",ret); 
			WRITEMSG 
			strcpy(g_pub_tx.reply,"D181");
			goto ErrExit; 
		}

		sMo_city.bat_no = ret + 1;
	}
    strcpy(sMo_city.payer_ac_no, mo_city_c.payee_ac_no);
	strcpy(sMo_city.payer_name, mo_city_c.payee_name);
	strcpy(sMo_city.payer_br_no, mo_city_c.payee_br_no);
	strcpy(sMo_city.payer_br_name, mo_city_c.payee_br_name);
	strcpy(sMo_city.payee_ac_no, mo_city_c.payer_ac_no);
	strcpy(sMo_city.payee_name, mo_city_c.payer_name);
	strcpy(sMo_city.payee_br_no, mo_city_c.payer_br_no);
	strcpy(sMo_city.payee_br_name, mo_city_c.payer_br_name);

	sMo_city.sign_date = 0;
	sMo_city.matr_date = 0;

	sMo_city.pr_cnt = 0;

    get_zd_data("0810",mo_city_c.brf);
    get_zd_data("0820",mo_city_c.remark);

	sMo_city.bill_amt = mo_city_c.bill_amt; 
	strcpy(sMo_city.brf, mo_city_c.brf);
	strcpy(sMo_city.remark, mo_city_c.remark);

	ret = Mo_city_Ins(sMo_city, g_pub_tx.reply);
	if (ret != 0)
	{
		sprintf(acErrMsg, "向同城登记簿中增加提出纪录错误!! [%d]",ret);
		WRITEMSG
		goto ErrExit;
	}


    ret=Com_parm_Sel(g_pub_tx.reply,&com_parm_c,"parm_code='TCKM' and \
        parm_seqn=2");
    if (ret != 0)
    {
        sprintf(acErrMsg,"查询公共参数表错误!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    /* 贷408内部清算科目 */
	if (mo_city_c.cd_ind[0] == '2')
	{
    	set_zd_data("1211",com_parm_c.val);
    	set_zd_double("1218",mo_city_c.bill_amt);
    	set_zd_data("1215","2");
    	set_zd_data("121A","网点提入退票");
	}
	else
	{
    	set_zd_data("1201",com_parm_c.val);
    	set_zd_double("1208",mo_city_c.bill_amt);
    	set_zd_data("1205","2");
    	set_zd_data("120A","网点提入退票");
	}

    memset(&com_parm_c,0x0,sizeof(struct com_parm_c));
    ret=Com_parm_Sel(g_pub_tx.reply,&com_parm_c,"parm_code='TCKM' and \
        parm_seqn=3");
    if (ret != 0)
    {
        sprintf(acErrMsg,"查询公共参数表错误!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    /* 借408外部清算科目 */
	if (mo_city_c.cd_ind[0] == '2')
	{
    	set_zd_data("1201",com_parm_c.val);
    	set_zd_double("1208",mo_city_c.bill_amt);
    	set_zd_data("1205","2");
    	set_zd_data("120A","网点提入退票");
	}
	else
	{
    	set_zd_data("1211",com_parm_c.val);
    	set_zd_double("1218",mo_city_c.bill_amt);
    	set_zd_data("1215","2");
    	set_zd_data("121A","网点提入退票");
	}
    
	/*** 登记交易流水 ***/
	strcpy(g_pub_tx.brf, "网点提入退票");
	strcpy(g_pub_tx.no_show, "1");
	g_pub_tx.ac_id = mo_city_c.tx_no;
	g_pub_tx.tx_amt1 = mo_city_c.tx_date;
	if( pub_ins_trace_log() )
	{
		sprintf(acErrMsg,"登记交易流水错误!");
		WRITEMSG
		goto ErrExit;
	}
	
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"网点提入退票处理成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"网点提入退票处理失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

