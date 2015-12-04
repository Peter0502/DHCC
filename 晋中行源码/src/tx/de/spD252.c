/***************************************************************
* 文 件 名: spD252.c
* 功能描述: 中心提入通存(记客户帐-存款)
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
#include "com_tx_br_rel_c.h"

spD252()
{
    int ret;

    struct mo_city_c mo_city_c; /***同城登记薄***/
    struct com_parm_c com_parm_c; /***公共参数表***/
    struct com_tx_br_rel_c com_tx_br_rel_c; /***交易机构关系表****/

    memset(&mo_city_c, 0x0, sizeof(struct mo_city_c));
    memset(&com_parm_c, 0x0, sizeof(struct com_parm_c));
    memset(&com_tx_br_rel_c, 0x0, sizeof(struct com_tx_br_rel_c));

	/*** 初始化全局变量 ***/
	if(pub_base_sysinit())
	{
		strcpy(acErrMsg,"初始化全局变量出错!");
		WRITEMSG
		goto ErrExit;
	}

    get_zd_data("0300", g_pub_tx.ac_no);
	pub_base_old_acno( g_pub_tx.ac_no );
    get_zd_double("0390",&g_pub_tx.tx_amt1);
    get_zd_data("0670",mo_city_c.bill_type);
    /* get_zd_data("0580",mo_city_c.bill_no); */

    strcpy(mo_city_c.in_out_ind,"2");
    
    
	/*** delelet by xxx ***
    ret = Mo_city_Dec_Upd(g_pub_tx.reply,"payee_ac_no='%s' and in_out_ind='2'\
          and cd_ind='2' and bill_amt='%lf' and sts \
          in ('B','H')", g_pub_tx.ac_no, g_pub_tx.tx_amt1);
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
 
    * 判断是否是该交易机构的上级机构 *
    ret = Com_tx_br_rel_Sel(g_pub_tx.reply,&com_tx_br_rel_c,"rel_type=3 \
          and br_no='%s'", mo_city_c.payee_br_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"查询交易机构关系表错误!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    if (strcmp(com_tx_br_rel_c.up_br_no,g_pub_tx.tx_br_no) != 0)
    {
        * 该机构不是该笔业务的上级机构不能打印 *
        sprintf(acErrMsg,"bat_no=[%d],br_no=[%s]",mo_city_c.bat_no,
                mo_city_c.tx_br_no);
        WRITEMSG
		strcpy(g_pub_tx.reply,"D166");
        goto ErrExit;
    }

    get_zd_data("0810",mo_city_c.brf);
    get_zd_data("0820",mo_city_c.remark);
    strcpy(mo_city_c.sts,"P");

    ret = Mo_city_Upd_Upd(mo_city_c, g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"UPDATE ERROR !! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }
    Mo_city_Clo_Upd();
	*****/

	/* 记408外科目--借 */
    ret=Com_parm_Sel(g_pub_tx.reply,&com_parm_c,"parm_code='TCKM' and \
        parm_seqn=2");
    if (ret != 0)
    {
        sprintf(acErrMsg,"查询公共参数表错误!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

       /****** 
    set_zd_data("1201",com_parm_c.val);
    set_zd_double("1208",g_pub_tx.tx_amt1);
    set_zd_data("1205","2");
    set_zd_data("120A","中心提入通存");
	*******/

 	strcpy(g_pub_tx.ac_no, com_parm_c.val);
	get_zd_double("1208",&g_pub_tx.tx_amt1);
	get_zd_data("1206",g_pub_tx.note_type);
	get_zd_data("1207",g_pub_tx.beg_note_no);
	get_zd_data("120A",g_pub_tx.brf);
	get_zd_data("1205",g_pub_tx.ct_ind); 
	get_zd_data("1204",g_pub_tx.cur_no); 
	strcpy( g_pub_tx.ac_id_type,"9" ); /*账户类型为内部*/ 
	strcpy( g_pub_tx.ac_get_ind,"00" ); /*本程序未读取分户*/ 
	strcpy( g_pub_tx.ac_wrk_ind,"000" ); /*不核对凭证号，零金额不计流水、明细*/ 
	strcpy( g_pub_tx.end_note_no,g_pub_tx.beg_note_no); 
	strcpy(g_pub_tx.add_ind,"0");/*借方标志*/ 
	strcpy( g_pub_tx.prt_ind,"0" ); /*不登折*/ 
	g_pub_tx.svc_ind=9001;  /*内部帐存取*/ 
	strcpy(g_pub_tx.prt_ind,"0"); 
	strcpy(g_pub_tx.hst_ind,"1"); /*日间入明细*/

	ret=pub_acct_trance();
	if( ret )
	{
		sprintf(acErrMsg,"调用存取款主控失败!");
		WRITEMSG
		goto ErrExit;
	}


	/*** 登记交易流水 
	if( pub_ins_trace_log() )
	{
		sprintf(acErrMsg,"登记交易流水错误!");
		WRITEMSG
		goto ErrExit;
	}***/
	
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"中心提入通存处理成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"中心提入通存处理失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

