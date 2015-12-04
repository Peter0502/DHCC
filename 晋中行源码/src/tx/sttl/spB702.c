/*************************************************
* 文 件 名:  spB702.c
* 功能描述： 证券回购借出借入记账
*
* 作    者:  
* 完成日期： 2003年1月29日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "mo_stok_rebuy_prot_c.h"
#include "com_parm_c.h"
#include "mbfe_pub.h"
#include "mo_city_c.h"

int spB702()
{
    struct mo_stok_rebuy_prot_c sMo_stok_rebuy_prot,sMo_stok_rebuy_prot1;   
	struct com_parm_c	sCom_parm;
	struct mbfeghvsendjrn_c mbfeghvsendjrn_v;
	struct mo_city_c	mo_city_c;
    char flag[2],zzlx[2];
	long   tmp_date=0,tmp_trace_no=0;
	double tmp_amt=0.00;
    int ret;
  
	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
	memset(&g_dd_parm,0x00,sizeof(struct dd_parm_c));
	memset(&g_td_mst,0x00,sizeof(struct td_mst_c));
	memset(&g_td_parm,0x00,sizeof(struct td_parm_c));
	memset(&g_ln_mst,0x00,sizeof(struct ln_mst_c));
	memset(&g_ln_parm,0x00,sizeof(struct ln_parm_c));
	memset(&g_in_mst,0x00,sizeof(struct in_mst_c));
	memset(&g_in_parm,0x00,sizeof(struct in_parm_c));
    memset(&sMo_stok_rebuy_prot,0x0,sizeof(struct mo_stok_rebuy_prot_c));
    memset(&sMo_stok_rebuy_prot1,0x0,sizeof(struct mo_stok_rebuy_prot_c));
	memset(&sCom_parm,0x00,sizeof(struct com_parm_c));
	memset(&mbfeghvsendjrn_v,0x00,sizeof(struct mbfeghvsendjrn_c));
	memset(&mo_city_c,0x00,sizeof(struct mo_city_c));

    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"初始化公用结构错误!!");
        WRITEMSG
        goto ErrExit;
    }
    
    get_zd_data("0860",sMo_stok_rebuy_prot.pact_no);
    get_zd_data("0700",sMo_stok_rebuy_prot.tcht_type);
    get_zd_double("0390",&sMo_stok_rebuy_prot.par_value);
    get_zd_double("0940",&sMo_stok_rebuy_prot.pledge_rate);
    get_zd_data("0680",sMo_stok_rebuy_prot.buy_type);			/*** 1-借出；2-借入 ***/
    get_zd_double("0400",&sMo_stok_rebuy_prot.bal);
    get_zd_long("0440",&sMo_stok_rebuy_prot.matr_date);
    get_zd_long("0480",&sMo_stok_rebuy_prot.term);
    get_zd_double("0850",&sMo_stok_rebuy_prot.rate);
    get_zd_data("0690",flag);  /*1 现金 2转帐*/
    get_zd_data("0210",g_pub_tx.cur_no);
    get_zd_data("0890",g_pub_tx.note_type);
    get_zd_data("0580",g_pub_tx.beg_note_no);
    get_zd_data("0580",g_pub_tx.end_note_no);
    get_zd_data("0710",g_pub_tx.tmp_ind);
    get_zd_data("0720",zzlx);   /*同城 本行 */
    get_zd_data("0270",sMo_stok_rebuy_prot.opt_name);

    if (flag[0] == '2')
    {
        get_zd_data("0300",sMo_stok_rebuy_prot.opt_acno);
    }
 
    /* 检查输入的信息和登记簿中的信息是否一致 */
    ret = Mo_stok_rebuy_prot_Sel(g_pub_tx.reply,&sMo_stok_rebuy_prot1,
          "pact_no='%s' and sts='1'",sMo_stok_rebuy_prot.pact_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"查询证券回购协议登记簿错误! [%d]",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"B091");
        goto ErrExit;
    }
   
    /* 检查证券类型 */
    strcpy(sMo_stok_rebuy_prot1.tcht_type,
             pub_base_strpack(sMo_stok_rebuy_prot1.tcht_type));
    if(strcmp(sMo_stok_rebuy_prot.tcht_type,sMo_stok_rebuy_prot1.tcht_type)!=0)
    {
        sprintf(acErrMsg,"证券类型和登记簿中的不一致!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"B088");
        goto ErrExit; 
    }

    /* 检查证券面值 */
    if (pub_base_CompDblVal(sMo_stok_rebuy_prot.par_value,
           sMo_stok_rebuy_prot1.par_value) != 0)
    {
        sprintf(acErrMsg,"证券面值输入的和登记簿中不一致!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"B089");
        goto ErrExit;
    }

    /* 检查抵押率 */
    if (pub_base_CompDblVal(sMo_stok_rebuy_prot.pledge_rate,
           sMo_stok_rebuy_prot1.pledge_rate) != 0)
    {
        sprintf(acErrMsg,"抵押率和登记簿中不一致!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"B090");
        goto ErrExit;
    }

    /* 检查借入借出标志 */
    if (strcmp(sMo_stok_rebuy_prot.buy_type,sMo_stok_rebuy_prot1.buy_type)!=0)
    {
        sprintf(acErrMsg,"借出借入标志和登记簿不一致!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"B092");
        goto ErrExit;
    }

    /* 检查借出借入金额查询 */
    if (pub_base_CompDblVal(sMo_stok_rebuy_prot.bal,
            sMo_stok_rebuy_prot1.bal)!=0)
    {
        sprintf(acErrMsg,"借出借入金额和登记簿中的不一致!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"B093");
        goto ErrExit;
    }
 
    /* 检查协议到期日 */
    if (sMo_stok_rebuy_prot.matr_date != sMo_stok_rebuy_prot1.matr_date)
    {
        sprintf(acErrMsg,"协议到期日和登记簿中的不一致!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"B094");
        goto ErrExit;
    }
   
    /* 检查证券回购期限 */
    if (sMo_stok_rebuy_prot.term != sMo_stok_rebuy_prot1.term)
    {
        sprintf(acErrMsg,"证券回购期限和登记簿中的不一致!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"B095");
        goto ErrExit;
    }

    /* 检查收益率 */
    if (pub_base_CompDblVal(sMo_stok_rebuy_prot.rate,
              sMo_stok_rebuy_prot1.rate) != 0)
    {
        sprintf(acErrMsg,"收益率和登记簿中的不一致!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"B096");
        goto ErrExit;
    }
    
    pub_base_strpack(sMo_stok_rebuy_prot1.opt_acno);
    if ( sMo_stok_rebuy_prot1.opt_acno[0] != '2' )
    {
         sprintf(acErrMsg,"收款付款方式与登记簿中的不一致!!");
         WRITEMSG
         strcpy(g_pub_tx.reply,"B129");
         goto ErrExit;
    }

    /* 给产品号赋值 */
    if (sMo_stok_rebuy_prot.buy_type[0] == '1')
    {
        /* 回购借出 */
        if (sMo_stok_rebuy_prot.tcht_type[0] == '1')
        {
            strcpy(g_pub_tx.prdt_code,REBUY_LOAN_NT);
        }
        else if (sMo_stok_rebuy_prot.tcht_type[0] == '2')
        {
            strcpy(g_pub_tx.prdt_code,REBUY_LOAN_ENT); 
        }
        else if (sMo_stok_rebuy_prot.tcht_type[0] == '4')
        {
            strcpy(g_pub_tx.prdt_code,REBUY_LOAN_FT); 
        }
		else
        {
            strcpy(g_pub_tx.prdt_code,REBUY_LOAN_QT); 
        }
    }   
    else if (sMo_stok_rebuy_prot.buy_type[0] == '2')
    {
        /* 回购借入 */
        if (sMo_stok_rebuy_prot.tcht_type[0] == '1')
        {
            strcpy(g_pub_tx.prdt_code,REBUY_BORROW_NT);
        }
        else if (sMo_stok_rebuy_prot.tcht_type[0] == '2')
        {
            strcpy(g_pub_tx.prdt_code,REBUY_BORROW_ENT);
        }
        else if (sMo_stok_rebuy_prot.tcht_type[0] == '4')
        {
            strcpy(g_pub_tx.prdt_code,REBUY_BORROW_FT);
        }
		else
        {
            strcpy(g_pub_tx.prdt_code,REBUY_BORROW_QT);
        }
    }
    else
    {
        sprintf(acErrMsg,"借出借入标志错误!!");
        WRITEMSG
        goto ErrExit;
    }

    /* 开介质 */
    strcpy(g_pub_tx.mdm_code,NULL_MDM_CODE);
    strcpy(g_pub_tx.cur_no,"01");

    ret = pub_acct_opn_in_mdm();
    if (ret != 0)
    {
        sprintf(acErrMsg,"开介质出错");
        WRITEMSG
        goto ErrExit;
    }

    sprintf(acErrMsg,"ac_no=[%s]",g_pub_tx.ac_no);
    WRITEMSG
    sprintf(acErrMsg,"开介质成功 PASS!!! ");
    WRITEMSG

    /* 开户 */
    strcpy(g_pub_tx.ac_id_type,"9");
    g_pub_tx.rate = sMo_stok_rebuy_prot.rate;
    g_pub_tx.add_ind[0] = '1';
    g_pub_tx.hst_ind[0] = '1';
    g_pub_tx.tx_amt1 = sMo_stok_rebuy_prot.bal;

    if(pub_acct_opn_ac())
    {
        sprintf(acErrMsg,"开户出错");
        WRITEMSG
        goto ErrExit;
    }

    set_zd_data("0310",g_pub_tx.ac_no);
    sMo_stok_rebuy_prot1.ac_id = g_pub_tx.ac_id;

    sprintf(acErrMsg,"开户成功 PASS!!! ac_no=[%s] ",g_pub_tx.ac_no);
    WRITEMSG

    /* 修改证券回购协议登记簿 */
    strcpy(sMo_stok_rebuy_prot1.sts,"2");
    ret = Mo_stok_rebuy_prot_Dec_Upd(g_pub_tx.reply,
          "pact_no='%s' and buy_type='%s'",sMo_stok_rebuy_prot1.pact_no,
           sMo_stok_rebuy_prot1.buy_type);
    if ( ret != 0 )
    {
        sprintf(acErrMsg,"declare for update error !! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    ret = Mo_stok_rebuy_prot_Fet_Upd(&sMo_stok_rebuy_prot,g_pub_tx.reply);
    if ( ret != 0 )
    {
        sprintf(acErrMsg,"Fetch for update error !! [%d]",ret);
        WRITEMSG
        Mo_stok_rebuy_prot_Clo_Upd();
        goto ErrExit;
    }

    ret = Mo_stok_rebuy_prot_Upd_Upd(sMo_stok_rebuy_prot1,g_pub_tx.reply);
    if ( ret != 0 )
    {
        sprintf(acErrMsg,"Update error !! [%d]",ret);
        WRITEMSG
        Mo_stok_rebuy_prot_Clo_Upd();
        goto ErrExit;
    }
    Mo_stok_rebuy_prot_Clo_Upd();

    if (flag[0] == '1') /*flag现金转帐标志*/
    {
        /* 调用现金子交易 */
		if(sMo_stok_rebuy_prot.buy_type[0] == '1') 	/** 借出 **/
        	goto OkExitCash_out;
		else
        	goto OkExitCash_in;
    }
    else 
    {
        /* 调用转帐子交易 */  
        strcpy(g_pub_tx.ac_no,sMo_stok_rebuy_prot1.opt_acno);
        g_pub_tx.ac_id = 0;
        g_pub_tx.ac_seqn = 0;
		if(sMo_stok_rebuy_prot.buy_type[0] == '1') 	/** 借出 **/
        	goto OkExit_d;
		else
        	goto OkExit_j;
    }

OkExitCash_in:
    strcpy( g_pub_tx.reply, "0001" );
    sprintf(acErrMsg,"success!!");
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
OkExitCash_out:
    strcpy( g_pub_tx.reply, "0003" );
    sprintf(acErrMsg,"success!!");
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
OkExit_j:
    strcpy( g_pub_tx.reply, "0002" );
    sprintf(acErrMsg,"success!!");
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
OkExit_d:
    strcpy( g_pub_tx.reply, "0004" );
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

