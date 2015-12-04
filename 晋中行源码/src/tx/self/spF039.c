/*************************************************
* 文 件 名:  spF039.c
* 功能描述： POS 预授权撤销冲正
*
* 作    者:  rob
* 完成日期： 2003年2月21日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "chnl_marchinfo_c.h"
#include "chnl_self_trade_c.h"
#include "chnl_premise_c.h"

int spF039()
{

    int ret;
    char flag;
    long old_date;
    long old_trace_no;
    long premise_no;
    char dc_ind[2],ct_ind[2],fee_ind[2];
    struct chnl_marchinfo_c chnl_marchinfo;
    struct chnl_self_trade_c chnl_self_trade;
    struct chnl_premise_c    sChnl_premise;
    struct chnl_premise_c    mChnl_premise;

    memset(&chnl_marchinfo,0x0,sizeof(struct chnl_marchinfo_c));
    memset(&chnl_self_trade,0x0,sizeof(struct chnl_self_trade_c));
    memset(&sChnl_premise,0x0,sizeof(struct chnl_premise_c));
    memset(&mChnl_premise,0x0,sizeof(struct chnl_premise_c));
    memset(dc_ind,0,sizeof(dc_ind));
    memset(ct_ind,0,sizeof(ct_ind));
    memset(fee_ind,0,sizeof(fee_ind));

    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"初始化公用结构错误!!");
        WRITEMSG
        goto ErrExit;
    }

    get_zd_data("0660",dc_ind);
    get_zd_data("0300",g_pub_tx.crd_no); /* 卡号 */
    get_zd_double("0400",&g_pub_tx.tx_amt1);  /* 消费金额 */
    get_zd_data("0790",g_pub_tx.draw_pwd); /* 支取密码 */
    get_zd_data("0730",chnl_marchinfo.mrchno); /* 商户号 */
	
    get_zd_long("0520",&chnl_self_trade.stra); /* 平台流水号 */
    get_zd_long("0440",&chnl_self_trade.dz_date); /* 平台日期 */
   
    get_zd_long("0450",&old_date); /* 原日期 */
    get_zd_long("0780",&old_trace_no); /* 原平台流水号 */
    get_zd_long("0870",&premise_no); /* 预授权号 */

    vtcp_log(" 平台流水号 0770 g_pub_tx.trace_no===[%d]",g_pub_tx.trace_no);
    vtcp_log(" 原平台流水号 0780 g_pub_tx.old_trace_no===[%d]",g_pub_tx.old_trace_no);

    /* 检查卡的合法性 */
    if (pub_base_CheckCrdNo())
    {
        sprintf(acErrMsg,"检查卡的合法性错误!!");
        WRITEMSG
        goto ErrExit;
    }

    /* 检查卡的支取方式 *******TJB************
    if (g_mdm_ac_rel.draw_pwd_yn[0] != 'Y')
    {
        sprintf(acErrMsg,"该卡的支取方式错误,必须是凭密码支取!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"CU06");
        goto ErrExit;
    }
	************不检查密码，由神码检查***********/

    /* 检查支取方式的合法性 */
	/*
    ret = pub_base_DesChk(g_pub_tx.tx_date, g_pub_tx.crd_no,
                      g_pub_tx.draw_pwd,g_mdm_ac_rel.draw_pwd);
    if (ret != 0)
    {
        sprintf(acErrMsg,"卡的支取密码错误!!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"CU55");
        goto ErrExit;
    }
	POS撤销冲正不需要校验密码,不传密码过来,20080311*/

    ret = Chnl_premise_Sel(g_pub_tx.reply,&mChnl_premise,"pt_trace_no=%ld and pt_date=%ld", old_trace_no,old_date);
    if(ret)
    {
        sprintf(acErrMsg,"要撤销的记录不存在!!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"CU25");
        goto ErrExit;
    }
    vtcp_log("%s,%d,sts[%s]",__FILE__,__LINE__,mChnl_premise.sts);
    /* 检查该记录是否正常 */
    if (mChnl_premise.sts[0] != '1')
    {
        sprintf(acErrMsg,"该记录已被冲正!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"CU06");
        goto ErrExit;
    }
    char tmpstr[51];
    memset(tmpstr,0,sizeof(tmpstr));
    get_zd_data("0810",tmpstr);
    memcpy(mChnl_premise.filler,tmpstr,30);
    ret = sql_execute2("update chnl_premise set sts='0',filler='%s' where pt_date=%ld and pt_trace_no=%ld",mChnl_premise.filler,old_date,old_trace_no); 
    if(ret)
    {
        sprintf(acErrMsg,"更新预授权记录状态失败!!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"CU25");
        goto ErrExit;
    }

    /* 修改帐户的控制金额并对帐户的检查 */
    g_pub_tx.ac_id = 0;
    if(dc_ind[0]=='1')
    {
    	g_pub_tx.add_ind[0] = '0'; /* 增加控制金额 */
    }else{
    	g_pub_tx.add_ind[0] = '1'; /* 增加控制金额 */
    }
    g_pub_tx.tx_amt1 = 0 - mChnl_premise.premise_amt;
    g_pub_tx.svc_ind = 1006;   /*  预授权  */
    g_pub_tx.hst_ind[0] = '0'; /* 不入明细 */
    memcpy(g_pub_tx.brf,tmpstr,20);
    if (pub_acct_trance())
    {
        sprintf(acErrMsg,"pos 预授权--错误!!!");
        WRITEMSG
        goto ErrExit;
    }

    /* 给自助设备交易日记结构赋值 */
    
    /* 登记自助设备交易日记 
    memset(&chnl_self_trade,0x0,sizeof(struct chnl_self_trade_c));	
    chnl_self_trade.tx_date = g_pub_tx.tx_date;
    chnl_self_trade.tx_time = g_pub_tx.tx_time;
    chnl_self_trade.trace_no = g_pub_tx.trace_no;
    strcpy(chnl_self_trade.cur_no,g_pub_tx.cur_no);

    strcpy(chnl_self_trade.ac_no,g_pub_tx.crd_no);

    strcpy(chnl_self_trade.tx_code,g_pub_tx.tx_code);
    strcpy(chnl_self_trade.tx_br_no,g_pub_tx.tx_br_no);
    strcpy(chnl_self_trade.opn_br_no,g_pub_tx.opn_br_no);
    strcpy(chnl_self_trade.tel,g_pub_tx.tel);
    strcpy(chnl_self_trade.ttyp,"2");
    strcpy(chnl_self_trade.ttyn,g_pub_tx.tty);
    strcpy(chnl_self_trade.spcd,chnl_marchinfo.mrchno);

    chnl_self_trade.tx_amt = g_pub_tx.tx_amt1;

       
    chnl_self_trade.strb = g_pub_tx.old_trace_no;
    get_zd_long("0520",&chnl_self_trade.stra);
    get_zd_long("0440",&chnl_self_trade.dz_date);
	
    strcpy(chnl_self_trade.dz_flag,"0");
    
    strcpy(chnl_self_trade.info_source,&flag);
    strcpy(chnl_self_trade.deal_flag,"0");
    strcpy(chnl_self_trade.add_ind,"0");
    strcpy(chnl_self_trade.batch_flag,"0");
    strcpy(chnl_self_trade.stat,"0");

	
    ret = pub_base_Insertselftrade(chnl_self_trade);
    if (ret != 0)
    {
        sprintf(acErrMsg,"登记自助设备交易日记错误!!");
        WRITEMSG
        goto ErrExit;
    }*/

OkExit:
    strcpy( g_pub_tx.reply, "0000" );
    sprintf(acErrMsg,"success!!");
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;

ErrExit:
	if(strlen(g_pub_tx.reply) == 0 || memcmp(g_pub_tx.reply,"0000",4)==0)
        {
                strcpy(g_pub_tx.reply,"CU06");
        }

	sprintf(acErrMsg,"failed !! reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}


