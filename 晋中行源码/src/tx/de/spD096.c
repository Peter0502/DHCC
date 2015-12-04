/***************************************************************
* 文 件 名: spD096.c
* 功能描述：提出子交易
* 作    者: jack
* 完成日期：2004年3月29日
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
#include "com_parm_c.h"
#include "mo_city_c.h"

spD096()
{
    int ret;
    char flag[2];
    struct com_parm_c com_parm_c;
    struct mo_city_c mo_city_c;

    memset(&com_parm_c, 0x0, sizeof(struct com_parm_c));
    memset(&mo_city_c, 0x0, sizeof(struct mo_city_c));
	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
	memset(&g_dd_parm,0x00,sizeof(struct dd_parm_c));
	memset(&g_td_mst,0x00,sizeof(struct td_mst_c));
	memset(&g_td_parm,0x00,sizeof(struct td_parm_c));
	memset(&g_ln_mst,0x00,sizeof(struct ln_mst_c));
	memset(&g_ln_parm,0x00,sizeof(struct ln_parm_c));
	memset(&g_in_mst,0x00,sizeof(struct in_mst_c));
	memset(&g_in_parm,0x00,sizeof(struct in_parm_c));
	g_reply_int=0;

	/*** 初始化全局变量 ***/
	pub_base_sysinit();

	get_zd_double("1131",&g_pub_tx.tx_amt1);
    get_zd_data("1132",flag); /* 提出借贷标志 */
    get_zd_data("1133",g_pub_tx.cur_no);
	get_zd_data("1135",g_pub_tx.brf);

	if( strcmp(g_pub_tx.brf,"")==0 )
	{
   		if( flag[0] == '1')
    	{
	    	strcpy(g_pub_tx.add_ind,"0"); /* 提出借 减少 */
        	strcpy(g_pub_tx.brf,"同城提出存款");
    	}
   	 	else
    	{
        	strcpy(g_pub_tx.add_ind,"1"); /* 提出贷 增加 */
        	strcpy(g_pub_tx.brf,"同城提出取款");
    	}
	}

  	if( flag[0] == '1')
    {
	   	strcpy(g_pub_tx.add_ind,"0"); /* 提出借 减少 */
    }
   	else
    {
       	strcpy(g_pub_tx.add_ind,"1"); /* 提出贷 增加 */
    }

    ret = Com_parm_Sel(g_pub_tx.reply,&com_parm_c,"parm_code='TCKM' and \
          parm_seqn = 1");
    if( ret!=0 )
    {
        sprintf(acErrMsg,"查询公共参数表错误!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }
 
    pub_base_strpack(com_parm_c.val);
    strcpy(g_pub_tx.ac_no,com_parm_c.val);
    sprintf(acErrMsg,"ac_no = [%s]",g_pub_tx.ac_no);

    strcpy(g_pub_tx.ct_ind,"2");

	strcpy(g_pub_tx.ac_wrk_ind,"100");
	strcpy(g_pub_tx.hst_ind,"1");
	g_pub_tx.svc_ind=1001;
	get_zd_data("0191",g_pub_tx.using_ind);
    g_pub_tx.ac_seqn = 0;

	if( g_pub_tx.using_ind[0]=='1' )
	{
		get_zd_data("0192",g_pub_tx.ac_wrk_ind);
		get_zd_int("0193",&g_pub_tx.svc_ind);
	}

	sprintf(acErrMsg,"BEG D099[%s][%s][%s][%d]",g_pub_tx.ac_no,
		g_pub_tx.using_ind,g_pub_tx.ac_wrk_ind,g_pub_tx.svc_ind);
	WRITEMSG
	/** 币种 **/

	g_reply_int=pub_acct_trance();
	if( g_reply_int )
	{
		sprintf(acErrMsg,"调用存取款主控失败!");
		WRITEMSG
		goto ErrExit;
	}

    /* 登记同城登记簿 */
    mo_city_c.tx_date = g_pub_tx.tx_date;
    mo_city_c.tx_no = g_pub_tx.trace_no;
    strcpy(mo_city_c.tx_br_no, g_pub_tx.tx_br_no);
    strcpy(mo_city_c.tel,g_pub_tx.tel);
    strcpy(mo_city_c.in_out_ind,"1"); /* 提出 */
    if (flag[0] == '1')
    {
        strcpy(mo_city_c.cd_ind,"1");
        get_zd_data("1134",mo_city_c.payee_ac_no);
    }
    else
    {
        strcpy(mo_city_c.cd_ind,"2");
        get_zd_data("1134",mo_city_c.payer_ac_no);
    }
    mo_city_c.bill_amt = g_pub_tx.tx_amt1;
	strcpy(mo_city_c.brf, g_pub_tx.brf);
    strcpy(mo_city_c.sts,"0");

    mo_city_c.bat_no = 0;
    mo_city_c.no = 0;
    
    ret = Mo_city_Ins(mo_city_c, g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"登记同城登记簿错误!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"提出记26102科目成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"提出记26102科目失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
