/*************************************************
* 文 件 名:  spB504.c
* 功能描述： 同业拆出记账
*
* 作    者:  rob
* 完成日期： 2003年1月17日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "mo_ibl_c.h"

int spB504()
{
    struct mo_ibl_c sMo_ibl_c,sMo_ibl_c1;
    struct mdm_ac_rel_c sMdm_ac_rel_c;
    struct in_parm_c  sIn_parm;
    char ac_no[20];
    int ret;
  
    memset(&sMo_ibl_c,0x0,sizeof(struct mo_ibl_c));
    memset(&sMo_ibl_c1,0x0,sizeof(struct mo_ibl_c));
    memset(&sMdm_ac_rel_c,0x0,sizeof(struct mdm_ac_rel_c));
    memset(&g_pub_tx,0x0,sizeof(struct pub_tx));
    

    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"初始化公用结构错误!!");
        WRITEMSG
        goto ErrExit;
    }

    get_zd_data("0730",sMo_ibl_c.pact_no);      /*合同号*/

    /* delete by rob 20030706
	get_zd_data("0230",sMo_ibl_c.prdt_no);      *产品编号*
    get_zd_data("0250",sMo_ibl_c.op_name);         *拆借户名*
    get_zd_data("0300",ac_no);                  *对方账号*
    get_zd_double("0400",&sMo_ibl_c.amt);       *拆借金额*
    get_zd_double("0840",&sMo_ibl_c.rate);      *拆借利率*
    get_zd_double("0850",&sMo_ibl_c.over_rate); *逾期利率*
    get_zd_long("0440",&sMo_ibl_c.matr_date);   *到期日期*


    ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel_c,"ac_no ='%s'",ac_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"查询对方账号id错误!! ret=[%d]",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply, "B006");
        goto ErrExit;
    }

    sMo_ibl_c.op_ac_id = sMdm_ac_rel_c.ac_id;
	********/

    ret = Mo_ibl_Dec_Upd(g_pub_tx.reply,"pact_no = '%s' and sign = '2' \
          and sts = '1'", sMo_ibl_c.pact_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"dec for update error !! ret=[%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    ret = Mo_ibl_Fet_Upd(&sMo_ibl_c1,g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"没有该项合同或合同号输错或已记账!!ret=[%d]",ret);
        WRITEMSG
        Mo_ibl_Clo_Upd();
        strcpy(g_pub_tx.reply,"B003");
        goto ErrExit;
    }

    strcpy(g_pub_tx.prdt_code,sMo_ibl_c1.prdt_no);
    
	/******* delete by rob at 20030706 
    if(strcmp(sMo_ibl_c1.prdt_no,sMo_ibl_c.prdt_no) != 0)
    {
        sprintf(acErrMsg,"产品号输入错误");
        WRITEMSG
        Mo_ibl_Clo_Upd();
        strcpy(g_pub_tx.reply,"B022");
        goto ErrExit;
    }
    
    strcpy(sMo_ibl_c1.op_name,pub_base_strpack(sMo_ibl_c1.op_name));
    sprintf(acErrMsg,"拆借户名 [%s] [%s]",sMo_ibl_c1.op_name,sMo_ibl_c.op_name);
    WRITEMSG
    if(strcmp(sMo_ibl_c1.op_name,sMo_ibl_c.op_name) != 0)
    {
        sprintf(acErrMsg,"拆借户名输入错误");
        WRITEMSG
        Mo_ibl_Clo_Upd();
        strcpy(g_pub_tx.reply,"B023");
        goto ErrExit;
    }

    if(sMo_ibl_c1.op_ac_id != sMo_ibl_c.op_ac_id)
    {
        sprintf(acErrMsg,"对方帐号输入错误");
        WRITEMSG
        Mo_ibl_Clo_Upd();
        strcpy(g_pub_tx.reply,"B024");
        goto ErrExit;
    }

     
    if(pub_base_CompDblVal(sMo_ibl_c1.amt, sMo_ibl_c.amt) != 0)
    {
        sprintf(acErrMsg,"拆借金额输入错误");
        WRITEMSG
        Mo_ibl_Clo_Upd();
        strcpy(g_pub_tx.reply,"B025");
        goto ErrExit;
    }

    if(pub_base_CompDblVal(sMo_ibl_c1.rate, sMo_ibl_c.rate) != 0)
    {
        sprintf(acErrMsg,"拆借利率输入错误");
        WRITEMSG
        Mo_ibl_Clo_Upd();
        strcpy(g_pub_tx.reply,"B026");
        goto ErrExit;
    }

    if(pub_base_CompDblVal(sMo_ibl_c1.over_rate, sMo_ibl_c.over_rate) != 0)
    {
        sprintf(acErrMsg,"逾期利率输入错误");
        WRITEMSG
        Mo_ibl_Clo_Upd();
        strcpy(g_pub_tx.reply,"B027");
        goto ErrExit;
    }

    if(sMo_ibl_c1.matr_date !=  sMo_ibl_c.matr_date)
    {
        sprintf(acErrMsg,"到期日期输入错误");
        WRITEMSG
        Mo_ibl_Clo_Upd();
        strcpy(g_pub_tx.reply,"B028");
        goto ErrExit;
    }
	*******/

    strcpy(sMo_ibl_c1.sts,"2");
    sMo_ibl_c1.wrk_date = g_pub_tx.tx_date;
    sMo_ibl_c1.trance_no = g_pub_tx.trace_no;
    strcpy(sMo_ibl_c1.tel, g_pub_tx.tel);
    strcpy(sMo_ibl_c1.chk, g_pub_tx.chk);
    strcpy(sMo_ibl_c1.auth,g_pub_tx.auth);

    ret = Mo_ibl_Upd_Upd(sMo_ibl_c1, g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"更新拆借登记簿错误!! ret =[%s]",ret);
        WRITEMSG
        Mo_ibl_Clo_Upd();
        strcpy(g_pub_tx.reply,"B008");
        goto ErrExit;
    }
    Mo_ibl_Clo_Upd();

	memcpy(&sMo_ibl_c, &sMo_ibl_c1, sizeof(struct mo_ibl_c));

    ret = In_parm_Sel(g_pub_tx.reply,&sIn_parm,"prdt_no = '%s'",
          sMo_ibl_c1.prdt_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"查询内部账产品表错误",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"W058");
        goto ErrExit;
    }

    /* 开介质 */
    strcpy(g_pub_tx.mdm_code,NULL_MDM_CODE);
    strcpy(g_pub_tx.cur_no,"01");
    strcpy(g_pub_tx.name, sMo_ibl_c.op_name);
    g_pub_tx.rate = sMo_ibl_c.rate;
    g_pub_tx.over_rate = sMo_ibl_c.over_rate;
	g_pub_tx.svc_ind = 9000;

    ret = pub_acct_opn_in_mdm();
    if (ret != 0)
    {
        sprintf(acErrMsg,"开介质出错");
        WRITEMSG
        goto ErrExit;
    }
    
    sprintf(acErrMsg,"ac_no=[%s],ac_id=[%ld]",g_pub_tx.ac_no,g_pub_tx.ac_id);
    WRITEMSG
    sprintf(acErrMsg,"开介质成功 PASS!!! ");
    WRITEMSG

    /* 开户 */
    /*strcpy(g_pub_tx.ac_id_type,"9");*/
    g_pub_tx.ac_id_type[0] = '9';
    g_pub_tx.ac_id_type[1] = '\0';
    g_pub_tx.add_ind[0] = '0'; /* 借 */
    g_pub_tx.ac_seqn = 0;
    g_pub_tx.tx_amt1 = sMo_ibl_c1.amt;
    g_pub_tx.hst_ind[0] = '1';
	strcpy(g_pub_tx.ct_ind, "2");
    strcpy(g_pub_tx.brf,"同业拆出记账");

    if(pub_acct_opn_ac())
    {
        sprintf(acErrMsg,"开户出错");
        WRITEMSG
        goto ErrExit;
    }
    
    sprintf(acErrMsg,"开户成功 PASS!!! ac_no=[%s] ",g_pub_tx.ac_no);
    WRITEMSG

    /* 修改拆借登记簿 */
    ret = Mo_ibl_Dec_Upd(g_pub_tx.reply,"pact_no = '%s' and sts = '2'",
                  sMo_ibl_c.pact_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"dec for update error !! ret=[%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    ret = Mo_ibl_Fet_Upd(&sMo_ibl_c1,g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"fecth for update error !! [%d]",ret);
        WRITEMSG
        Mo_ibl_Clo_Upd();
        goto ErrExit;
    }
    sMo_ibl_c1.ac_id = g_pub_tx.ac_id;
    sMo_ibl_c1.opn_date = g_pub_tx.tx_date;
    sMo_ibl_c1.trance_no = g_pub_tx.trace_no;

    ret = Mo_ibl_Upd_Upd(sMo_ibl_c1, g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"更新拆借登记簿错误!! ret =[%s]",ret);
        WRITEMSG
        Mo_ibl_Clo_Upd();
        strcpy(g_pub_tx.reply,"B008");
        goto ErrExit;
    }
    Mo_ibl_Clo_Upd();

    set_zd_data("0310", g_pub_tx.ac_no);

    set_zd_double("0410",g_pub_tx.tx_amt1);

	/***** delete by rob at 20030706 
    * 对方账户取款 *
    g_pub_tx.add_ind[0] = '0';
    strcpy(g_pub_tx.ac_no,ac_no);
    g_pub_tx.ac_id = sMo_ibl_c1.op_ac_id;
    g_pub_tx.ac_seqn = 0;

    if (pub_acct_trance())
    {
        sprintf(acErrMsg,"对方账户取款错误");
        WRITEMSG
        goto ErrExit;
    }

    sprintf(acErrMsg,"g_in_mst.bal=[%lf]",g_in_mst.bal);
    WRITEMSG
    set_zd_double("0420",g_in_mst.bal);

    if (pub_ins_trace_log())
    {
        sprintf(acErrMsg,"登记交易流水错误!!");
        WRITEMSG
        goto ErrExit;
    }
	*********/
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
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

