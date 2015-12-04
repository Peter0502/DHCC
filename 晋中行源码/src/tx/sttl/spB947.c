/********************************************************************
* 文 件 名:  spB947.c
* 功能描述： 完成宁夏有追索权转贴现转出记账功能；更改原(转)贴现的状态。
*
* 作    者:  xyy
* 完成日期： 2009-9-18 21:28:50
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*********************************************************************/

#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "mo_discnt_c.h"
#include "com_parm_c.h"

int spB947()
{
    char tmp[10];
    char cKinbr[BRNO_LEN+1];
	  char	cTinAcNo[14];				/* 转贴现帐号 */
    struct mo_discnt_c sMoDiscnt, sMoDiscnt1;    /* 贴现登记薄 */
    double    dAmt1=0.00, dAmt2=0.00, dAmt3=0.00;
    int       ret=0;
		char cPn_knd[2];
    memset(&sMoDiscnt , 0x00, sizeof(struct mo_discnt_c));
    memset(&sMoDiscnt1, 0x00, sizeof(struct mo_discnt_c));
    memset(&g_pub_tx  , 0x00, sizeof(g_pub_tx));
    memset(cKinbr  ,   0 , sizeof(cKinbr));
    memset(cPn_knd  ,   0 , sizeof(cPn_knd));
    memset(cTinAcNo  ,   0 , sizeof(cTinAcNo));

    /* 数据初始化 */
    pub_base_sysinit();
    strcpy(g_pub_tx.mdm_code, NULL_MDM_CODE);    /* 空介质介质代码 */

    /* 从前台屏幕取值 */
    get_zd_data("0900", sMoDiscnt.note_type);   /* 票据类型 */
    get_zd_data("0600", sMoDiscnt.pnote_no);    /* 票据号 */
	  get_zd_data( "0370", cTinAcNo );				/* 转贴现帐号 */
    get_zd_data("0670", sMoDiscnt.tpay_ac_type);/* 付款账户类型 */
    get_zd_data("0910", cKinbr);
    get_zd_data("0980",cPn_knd);
    if(cPn_knd[0]=='E')
    {
    	get_zd_data("1032",sMoDiscnt.pnote_no);
    }
    sprintf(acErrMsg, "pnote_no=[%s]", sMoDiscnt.pnote_no);
    WRITEMSG
    /*
     * 检查票据是否存在，状态是否为录入，贴现类型是否为买断转出,修改贴现登
     * 记簿 
     */
    ret = Mo_discnt_Dec_Upd(g_pub_tx.reply, "note_type='%s' and pnote_no='%s' \
                and ((pn_type='4' and po_knd='P') or (pn_type='5' and po_knd='E')) and sts!='*' and br_no ='%s' ", \
          sMoDiscnt.note_type, sMoDiscnt.pnote_no,cKinbr);/**NEWDP**/
    if (ret) {
        sprintf(acErrMsg, "执行Mo_discnt_Dec_Upd错[%d]", ret);
        WRITEMSG
        goto ErrExit;
    }
    ret = Mo_discnt_Fet_Upd(&sMoDiscnt, g_pub_tx.reply);
    if (ret == 100) 
    {
        sprintf(acErrMsg, "此票据未作买断式转贴现转出!");
        WRITEMSG
        strcpy(g_pub_tx.reply, "B249");
        goto ErrExit;
    }
    else if (ret) 
    {
        sprintf(acErrMsg, "执行Mo_discnt_Fet_Upd错![%d]", ret);
        WRITEMSG
        goto ErrExit;
    }
    if (sMoDiscnt.sts[0] == '3') 
    {
        sprintf(acErrMsg, "此笔贴现已记帐!");
        WRITEMSG
        strcpy(g_pub_tx.reply, "B148");
        goto ErrExit;
    }
    /* 根据票据类型和票据号查询贴现登记薄中票据录入时的信息 */
    ret = Mo_discnt_Sel(g_pub_tx.reply, &sMoDiscnt1, "note_type='%s' and pnote_no='%s' \
        and (pn_type='1' or pn_type='2' or pn_type='3') and sts='4' ", sMoDiscnt.note_type, sMoDiscnt.pnote_no);
    if (ret == 100) 
    {
        sprintf(acErrMsg, "无此票据信息!");
        WRITEMSG
        strcpy(g_pub_tx.reply, "B130");
        goto ErrExit;
    } 
    else if (ret) 
    {
        sprintf(acErrMsg, "执行Mo_discnt_Sel错![%d]", ret);
        WRITEMSG
        goto ErrExit;
    }
    /* 修改转贴现登记薄，状态为正常 */
    strcpy(sMoDiscnt.sts, "3");    /* 标志：3 正常 */
	strcpy( sMoDiscnt.tin_ac_no, cTinAcNo );	/* 转贴现帐号 */
    sMoDiscnt.wrk_date = g_pub_tx.tx_date;    /* 流水日期 */
    sMoDiscnt.trace_no = g_pub_tx.trace_no;    /* 主机流水号 */

    ret = Mo_discnt_Upd_Upd(sMoDiscnt, g_pub_tx.reply);
    if (ret) 
    {
        sprintf(acErrMsg, "修改贴现登记薄错[%d]", ret);
        WRITEMSG
        goto ErrExit;
    }
    Mo_discnt_Clo_Upd();
    
    get_zd_double("1001", &dAmt1);    /* 转贴现利息 */
    get_zd_double("1002", &dAmt2);    /* 票据面额 */
    dAmt3 = dAmt2 - dAmt1;            /* 提入金额 */
    set_zd_double("1003", dAmt3);
    vtcp_log("[%s][%d] [%f]-[%f]=[%f]",__FILE__,__LINE__,dAmt2,dAmt1,dAmt3);

    /* 记流水日志 */
    strcpy(g_pub_tx.cur_no, "01");
    strcpy(g_pub_tx.no_show, "0");
    ret = pub_ins_trace_log();
    if (ret) 
    {
        sprintf(acErrMsg, "记流水日志错");
        WRITEMSG
        goto ErrExit;
    }
    vtcp_log("[%s][%d] 有追索权转贴现记账完成!!",__FILE__,__LINE__);
OkExit:
    memset(tmp,0,sizeof(tmp));
    get_zd_data("0670",tmp);
    vtcp_log("[%s][%d],CESHI-0670[%s]",__FILE__,__LINE__,tmp);
    strcpy(g_pub_tx.reply, "0000");
    sprintf(acErrMsg, "Before OK return: reply is[%s]", g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 0;
ErrExit:
    sprintf(acErrMsg, "Before return: reply is[%s]", g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 1;
}
