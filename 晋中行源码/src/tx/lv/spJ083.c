/************************************************************************
* 文 件 名:   spJ083.c
* 功能描述：  合同变更
*
* 作    者:   dongxy
* 完成日期：  2006-12-13
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
************************************************************************/
#define EXTERN
#include <string.h>
#include <stdio.h>
#include "hv_define.h"
#include "lv_define.h"
#include "hvpack.h"
#include "lvpack.h"
#define loc_t sys_loc_t
#include "hv_fzf_c.h"
#include "hv_fzfhtr_c.h"
#include "lv_pldsfqugx_c.h"


spJ083()
{
    int iRet,k;
    char  cOpflag[2];
    char  cErrcode[61];
    TLVPACKAGE lv;
    struct lv_pldsfqugx_c lv_pldsfqugx;

    memset(cOpflag, 0x00, sizeof(cOpflag));
    memset(cErrcode, 0x00, sizeof(cErrcode));
    memset(&lv_pldsfqugx, 0x00, sizeof(lv_pldsfqugx));

    vtcp_log("[%s][%d]==========开始初始化了",__FILE__,__LINE__);
    pub_base_sysinit();
    
    get_zd_long("0440",&lv_pldsfqugx.wt_date);     /** 委托日期 **/
    get_zd_data("0670", lv_pldsfqugx.xy_ind);  /** 合同（协议）类型 **/
    get_zd_data("0680", lv_pldsfqugx.ch_ind);  /** 变更方式 **/
    get_zd_data("0300", lv_pldsfqugx.pay_br_no);  /** 付款行行号 **/
    get_zd_data("0310", lv_pldsfqugx.pay_qs_no);  /** 付款清算行行号 **/
    get_zd_data("0320", lv_pldsfqugx.pay_opn_br_no);  /** 付款人开户行行号 **/
    get_zd_data("0330", lv_pldsfqugx.pay_ac_no);  /** 付款人账号 **/
    get_zd_data("0961", lv_pldsfqugx.xyno);
    get_zd_data("0962", lv_pldsfqugx.pay_name);
    get_zd_data("0963", lv_pldsfqugx.pay_addr);
    get_zd_data("0964", lv_pldsfqugx.content);
    
    vtcp_log("[%s][%d]==========lv_pldsfqugx.wt_date=[%d]",__FILE__,__LINE__,lv_pldsfqugx.wt_date);
    vtcp_log("[%s][%d]==========lv_pldsfqugx.xy_ind=[%c]",__FILE__,__LINE__,lv_pldsfqugx.xy_ind[0]);
    vtcp_log("[%s][%d]==========lv_pldsfqugx.ch_ind=[%c]",__FILE__,__LINE__,lv_pldsfqugx.ch_ind[0]);
    vtcp_log("[%s][%d]==========lv_pldsfqugx.pay_br_no=[%s]",__FILE__,__LINE__,lv_pldsfqugx.pay_br_no);
    vtcp_log("[%s][%d]==========lv_pldsfqugx.pay_qs_no=[%s]",__FILE__,__LINE__,lv_pldsfqugx.pay_qs_no);
    vtcp_log("[%s][%d]==========lv_pldsfqugx.pay_opn_br_no=[%s]",__FILE__,__LINE__,lv_pldsfqugx.pay_opn_br_no);
    vtcp_log("[%s][%d]==========lv_pldsfqugx.pay_ac_no=[%s]",__FILE__,__LINE__,lv_pldsfqugx.pay_ac_no);
    vtcp_log("[%s][%d]==========lv_pldsfqugx.xyno=[%s]",__FILE__,__LINE__,lv_pldsfqugx.xyno);
    vtcp_log("[%s][%d]==========lv_pldsfqugx.pay_name=[%s]",__FILE__,__LINE__,lv_pldsfqugx.pay_name);
    vtcp_log("[%s][%d]==========lv_pldsfqugx.pay_addr=[%s]",__FILE__,__LINE__,lv_pldsfqugx.pay_addr);
    vtcp_log("[%s][%d]==========lv_pldsfqugx.content=[%s]",__FILE__,__LINE__,lv_pldsfqugx.content);

    lv_create(&lv);
    iRet=lv_dsf_deal40507(&lv_pldsfqugx);
    if(iRet)
    {
            vtcp_log("[%s][%d],交易处理失败![%d]\n",__FILE__,__LINE__,iRet);
            cOpflag[0] = '1';
            memcpy(cErrcode, "交易处理失败",12);
            set_zd_data("0690", cOpflag);
            set_zd_data("0740", cErrcode);
            sprintf(acErrMsg,"RET [%d]",iRet);
            WRITEMSG
            goto ErrExit;
    }
    else 
    {
            cOpflag[0] = '0';
    }

    lv_destroy(&lv);

    /** 开始组织返回的变量 **/
    set_zd_data("0690", cOpflag);  /** 成功失败标志 **/
    set_zd_data("0280", lv_pldsfqugx.orderno);  /** 交易序号 **/
    set_zd_long("0290", lv_pldsfqugx.wt_date);  /** 包委托日期 **/
    set_zd_data("0440", lv_pldsfqugx.packid);  /** 包id **/
    
    set_zd_data("0961", lv_pldsfqugx.xyno);  /** 合同协议号**/
    set_zd_data("0962", lv_pldsfqugx.pay_name);  /** 付款人名称 **/
    set_zd_data("0963", lv_pldsfqugx.pay_addr);  /** 付款人地址 **/
    set_zd_data("0964", lv_pldsfqugx.content);  /** 附言 **/
    
    set_zd_data("0670", lv_pldsfqugx.xy_ind);  /** 合同（协议）类型 **/
    set_zd_data("0680", lv_pldsfqugx.ch_ind);  /** 变更方式 **/
    set_zd_data("0320", lv_pldsfqugx.pay_br_no);  /** 付款行行号 **/
    set_zd_data("0330", lv_pldsfqugx.pay_qs_no);  /** 付款清算行行号 **/
    set_zd_data("0370", lv_pldsfqugx.pay_opn_br_no);/** 付款人开户行行号 **/
    set_zd_data("0810", lv_pldsfqugx.pay_ac_no);  /** 付款人账号 **/

    /*beg add by Chenggx 2011-6-16  晋中 登记流水*/
    /* 登记交易流水 */
    if ( pub_ins_trace_log() )
    {
        sprintf(acErrMsg,"登记交易流水错误!");
        WRITEMSG
        goto ErrExit;
    }
    /*beg add by Chenggx 2011-6-16  晋中*/
OkExit:
        strcpy( g_pub_tx.reply, "0000" );
        sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
        WRITEMSG
        set_zd_data(DC_REPLY,g_pub_tx.reply);
        return 0;
ErrExit:
        sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
        WRITEMSG
        set_zd_data(DC_REPLY,g_pub_tx.reply);
        return 1;

}
