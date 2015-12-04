/*************************************************
* 文 件 名:  spF047.c
* 功能描述： POS 余额查询(查询本行的余额)
*
* 作    者:  rob
* 完成日期： 2003年2月22日
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

int spF047()
{

    int ret;
    char flag;
    long old_date;
    char tty[9];
    char note_no[21];
    char in_type[5];
    double ky_amt=0.0;

    struct chnl_marchinfo_c chnl_marchinfo;
    struct chnl_self_trade_c chnl_self_trade;

    memset(&chnl_marchinfo,0x0,sizeof(struct chnl_marchinfo_c));
    memset(&chnl_self_trade,0x0,sizeof(struct chnl_self_trade_c));

    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"初始化公用结构错误!!");
        WRITEMSG
        goto ErrExit;
    }

    get_zd_data("0370",g_pub_tx.crd_no);   /* 卡号 */
    get_zd_data("0800",g_pub_tx.draw_pwd); /* 支取密码 */
    get_zd_data("0380", tty);              /* 终端号 */
    get_zd_data("0620", note_no);          /* 商户号 */
    get_zd_data("0910", in_type);          /* 商户服务方式 */

    sprintf(acErrMsg, "卡号为: [%s]", g_pub_tx.crd_no);
	WRITEMSG
    /* 检查卡的合法性 */
    if (pub_base_CheckCrdNo())
    {
        sprintf(acErrMsg,"检查卡的合法性错误!!");
        WRITEMSG
		strcpy(g_pub_tx.reply, "AT01");
        goto ErrExit;
    }

	if ( pub_mobile_pay(in_type, tty, note_no) )
	{
		/* 不是移动支付 */
    	/* 检查卡的支取方式 */
    	if (g_mdm_ac_rel.draw_pwd_yn[0] != 'Y')
    	{
        	sprintf(acErrMsg,"该卡的支取方式错误,必须是凭密码支取!!");
        	WRITEMSG
        	strcpy(g_pub_tx.reply,"AT02");
        	goto ErrExit;
    	}

    	/* 检查支取方式的合法性 */
    	ret = pub_base_DesChk(g_pub_tx.tx_date, g_pub_tx.crd_no, g_pub_tx.draw_pwd,g_mdm_ac_rel.draw_pwd);
    	if (ret != 0)
    	{
        	sprintf(acErrMsg,"卡的支取密码错误!!!");
        	WRITEMSG
        	strcpy(g_pub_tx.reply,"AT04");
        	goto ErrExit;
    	}
	}

    /* 查询活期帐户主文件 */
    strcpy(g_pub_tx.cur_no,"01");
    ret = pub_base_CurGetddFile(g_pub_tx.crd_no,g_pub_tx.cur_no,&g_dd_mst);
    if (ret != 0)
    {
        sprintf(acErrMsg,"查询活期帐户主文件错误!!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"AT06");
        goto ErrExit;
    }

    /*set_zd_double("0420",g_dd_mst.bal);    20080311帐户余额 */
	ky_amt = g_dd_mst.bal - g_dd_mst.ctl_amt; /*可用余额，余额减去控制金额*/
    set_zd_double("0390",ky_amt);   /* 可用余额*/
    set_zd_double("0400",g_dd_mst.bal);   /* 帐户余额 */
	vtcp_log("%s,%d,余额[%.2f],可用余额[%.2f]\n",__FILE__,__LINE__,g_dd_mst.bal,ky_amt);
    set_zd_data("0270",g_dd_mst.name);    /* 户名 */
	
	/*begin add by LiuHuafeng 200701022 */
	ret=pubf_card_chk_msr(1,g_pub_tx.crd_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"查询卡户登记簿错误!!!");
        WRITEMSG
		if(memcmp(g_pub_tx.reply,"0000",4)==0)
            strcpy(g_pub_tx.reply,"AT06");
        goto ErrExit;
    }
	/* end by LiuHuafeng 20070122 */

    /* 根据 账面余额 和 当日可用余额 设置 实际余额 FD74 */
    ret = iSetAdditional_Amounts( g_dd_mst.bal, g_dd_mst.bal);  /* 设置 FD74 实际余额*/
    if(ret)
    {
        return -1;
     }
	

    /* 给自助设备交易日记结构赋值 
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
   
    get_zd_long("0520", &chnl_self_trade.stra);

    chnl_self_trade.tx_amt = 0.00;
    chnl_self_trade.sl_sxamt = 0.00;
    chnl_self_trade.fk_sxamt = 0.00;

    strcpy(chnl_self_trade.dz_flag,"0");
    * strcpy(chnl_self_trade.ct_ind,g_pub_tx.ct_ind); *
    strcpy(chnl_self_trade.info_source,&flag);
    strcpy(chnl_self_trade.deal_flag,"0");
    strcpy(chnl_self_trade.brf_no,"PSYC");
    strcpy(chnl_self_trade.add_ind,"");
    strcpy(chnl_self_trade.batch_flag,"0");
    strcpy(chnl_self_trade.stat,"0");*/


    /* 登记自助设备交易日记 
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
	sprintf(acErrMsg,"failed !! reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

