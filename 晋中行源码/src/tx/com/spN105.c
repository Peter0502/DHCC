/***********************************************************
* 文 件 名:  spN105.c
* 功能描述:  将支票等凭证卖给客户, 可整本出售, 也可以零售
*
* 作    者:  andy
* 完成日期:  2004年1月8日
*
* 修改记录:
*   日    期:
*   修 改 人:
*   修改内容:
***********************************************************/

#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "note_mst_c.h"
#include "note_cheq_mst_c.h"
#include "note_mst_hst_c.h"
#include "note_parm_c.h"

/*********************  全局变量定义  *********************/
char g_sell_way[2];     /* 凭证出售方式 0零张, 1整本 */

int spN105()
{
    struct note_mst_c      sNoteMst;       /* 柜员凭证台账 */
    struct note_mst_c      sNoteMstTmp;    /* 柜员凭证台账 */
    struct note_cheq_mst_c sCheqMst;       /* 客户凭证台账 */
    struct note_mst_hst_c  sNoteMstHst;    /* 柜员凭证台账明细 */
    struct note_parm_c     sNoteParm;      /* 凭证属性参数表 */
    char                   cNoteAct[2];    /* 凭证行为 */
    char                   tw_br_no[6];    /* 对方机构号 */
    char                   tw_tel[6];      /* 对方操作员 */
    char                   cCheqSts[2];    /* 客户凭证状态 */
    char                   cOpenBrNo[6];   /* 开户机构号 */
    char                   cWho[2];        /* 凭证执行者, c: 客户, t: 柜员 */
    char                   cInOut[2];      /* 凭证最小号方向 */
    int                    ret;

    memset(&sNoteMst,    0x00, sizeof(struct note_mst_c));
    memset(&sNoteMstTmp, 0x00, sizeof(struct note_mst_c));
    memset(&sCheqMst,    0x00, sizeof(struct note_cheq_mst_c));
    memset(&sNoteMstHst, 0x00, sizeof(struct note_mst_hst_c));
    memset(&sNoteParm,   0x00, sizeof(struct note_parm_c));
    memset(cNoteAct,     0x00, sizeof(cNoteAct));
    memset(tw_br_no,     0x00, sizeof(tw_br_no));
    memset(tw_tel,       0x00, sizeof(tw_tel));
    memset(cCheqSts,     0x00, sizeof(cCheqSts));
    memset(cOpenBrNo,    0x00, sizeof(cOpenBrNo));
    memset(cWho,         0x00, sizeof(cWho));
    memset(cInOut,       0x00, sizeof(cInOut));

    /* 系统初始化(初始化g_pub_tx)数据初始化 */
    init_pub_tx();

    /* 本方权限检查 */
    if (pub_com_NoteAuthChk(g_pub_tx.tx_br_no, g_pub_tx.tel, 401, 0))
        goto ErrExit;

    strcpy(cNoteAct, "6");    /* 6表示柜员凭证出售操作 */
    strcpy(cInOut,   "O");

    /* 检查凭证是否可用(凭证种类表中是否已经定义, 是否定义为启用) */
    ret = pub_com_ChkNoteUse(g_pub_tx.note_type);
    if (ret)
    {
        sprintf(acErrMsg, "检查凭证是否存在, 是否启用错");
        WRITEMSG
        goto ErrExit;
    }

    ret = Note_parm_Sel(g_pub_tx.reply, &sNoteParm, "note_type='%s'",
        g_pub_tx.note_type);
    if (ret) goto ErrExit;

    /* 凭证起始号码、终止号码检查(检查冠字头) */
    ret = pub_com_ChkNoteHead(g_pub_tx.note_type, g_pub_tx.beg_note_no, g_pub_tx.tel);
    if (ret)
    {
        sprintf(acErrMsg, "检查凭证起始号码冠字头错");
        WRITEMSG
        goto ErrExit;
    }

    ret = pub_com_ChkNoteHead(g_pub_tx.note_type, g_pub_tx.end_note_no, g_pub_tx.tel);
    if (ret)
    {
        sprintf(acErrMsg, "检查凭证终止号码冠字头错");
        WRITEMSG
        goto ErrExit;
    }

    /* 检查是否是整本, 如果是, 则要判断起始号码终止号码是否构成整本 */
    if (g_sell_way[0] == '1')
    {
        ret = pub_com_ChkPile(g_pub_tx.note_type, g_pub_tx.beg_note_no, \
            g_pub_tx.end_note_no);
        if (ret)
        {
            sprintf(acErrMsg, "检查凭证是否是整本错");
            WRITEMSG
            goto ErrExit;
        }
    }

    strcpy(sNoteMst.tel,         g_pub_tx.tel);
    strcpy(sNoteMst.br_no,       g_pub_tx.tx_br_no);
    strcpy(sNoteMst.note_type,   g_pub_tx.note_type);
    strcpy(sNoteMst.beg_note_no, g_pub_tx.beg_note_no);
    strcpy(sNoteMst.end_note_no, g_pub_tx.end_note_no);
    sNoteMst.cnt = (int)g_pub_tx.tx_amt1;

    /* 柜员凭证有效性检查 */
    ret = pub_com_ChkNote(cNoteAct, sNoteMst);
    if (ret)
    {
        sprintf(acErrMsg, "检查柜员凭证错");
        WRITEMSG
        goto ErrExit;
    }

    /* 根据账号查询账户序号和账户ID */
    ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &g_mdm_ac_rel, "ac_no='%s'", g_pub_tx.ac_no);
    if (ret == 100)
    {
        sprintf(acErrMsg, "不存在该账号!!ac_no=[%s]", g_pub_tx.ac_no);
        WRITEMSG
        strcpy(g_pub_tx.reply, "M003");
        goto ErrExit;
    }else if (ret)
    {
        sprintf(acErrMsg, "执行Mdm_ac_rel_Sel错误!ret=[%d]", ret);
        WRITEMSG
        goto ErrExit;
    }
    sprintf(acErrMsg, "ac_seqn=[%d]", g_mdm_ac_rel.ac_seqn);
    WRITEMSG

    /* 账户有效性检查 */
    ret = pub_base_ChkAc(g_pub_tx.ac_no, g_mdm_ac_rel.ac_seqn, cOpenBrNo, &g_pub_tx.ac_id);
    if (ret)
    {
        sprintf(acErrMsg, "账户有效性检查错");
        WRITEMSG
        goto ErrExit;
    }

    if (strcmp(cOpenBrNo, g_pub_tx.tx_br_no) != 0)
    {
        sprintf(acErrMsg, "此账户开户机构非本交易机构, 不允许在本机构购买支票!");
        WRITEMSG
        strcpy(g_pub_tx.reply, "N087");
        goto ErrExit;
    }
        
    /* 保证金户和协定户不能出售凭证 20131224 wudawei */
     /* 印鉴卡不卡 20140522 wudawei 注释了  |||| 20140618 加了133产品  wudawei*/
   /* if (strncmp(g_pub_tx.note_type ,"026" ,3) != 0 )
    {
    */
		    if (g_dd_mst.ac_type[0]=='6' || strcmp(g_dd_mst.prdt_no,"131")==0 || strcmp(g_dd_mst.prdt_no,"132")==0 || strcmp(g_dd_mst.prdt_no,"133")==0 
		    	|| strcmp(g_dd_mst.prdt_no,"134")==0 || strcmp(g_dd_mst.prdt_no,"142")==0
		    	|| strcmp(g_dd_mst.prdt_no,"144")==0)
		    {
		        sprintf(acErrMsg, "此账户不可提现, 不能购买现金支票");
		        WRITEMSG
		        strcpy(g_pub_tx.reply, "D188");
		        goto ErrExit;
		    }
    /*}*/
    /* modify by wudawei end 20131224  */

    /* 对现金支票的特别处理***begin */
    if (g_dd_mst.ac_type[0]=='2' && !strcmp(sNoteMst.note_type, "001"))
    {
        sprintf(acErrMsg, "此账户不可提现, 不能购买现金支票");
        WRITEMSG
        strcpy(g_pub_tx.reply, "D188");
        goto ErrExit;
    }

    if (strcmp(sNoteMst.note_type, "005") && (g_dd_mst.ac_type[0] == '5' || g_dd_mst.ac_type[0] == '7'))
    {
        sprintf(acErrMsg,"此账户个人, 不能购买现金支票");
        WRITEMSG
        strcpy(g_pub_tx.reply,"D242");
        goto ErrExit;
    }
    /* 对现金支票的特别处理***end */

    g_pub_tx.ac_seqn = g_mdm_ac_rel.ac_seqn;

    /* 客户凭证检查 */
    ret = pub_com_ChkCheqSell(g_pub_tx.ac_id, g_pub_tx.ac_seqn, \
        sNoteMst.note_type, sNoteMst.beg_note_no, sNoteMst.end_note_no);
    if (ret)
    {
        sprintf(acErrMsg, "检查客户凭证错");
        WRITEMSG
        goto ErrExit;
    }

    sNoteMstTmp = sNoteMst;

    /* 对柜员凭证进行分段处理 */
    ret = pub_com_NoteApart(cNoteAct, sNoteMst);
    if (ret)
    {
        sprintf(acErrMsg, "柜员凭证分段处理失败!");
        WRITEMSG
        goto ErrExit;
    }

    ret = wrt_NoteUse_log(g_pub_tx.ac_id, g_pub_tx.ac_seqn, \
        sNoteMst.note_type, sNoteMst.beg_note_no, sNoteMst.end_note_no, \
        atol(sNoteMst.end_note_no + sNoteParm.head_len) -
        atol(sNoteMst.beg_note_no + sNoteParm.head_len) + 1);
    if (ret)
    {
        sprintf(acErrMsg, "柜员凭证使用记账错");
        WRITEMSG
        return(-1);
    }

    /* 登记柜员凭证明细 */
    ret = pub_com_RegNoteHst(cNoteAct, sNoteMst, tw_br_no, tw_tel);
    if (ret)
    {
        sprintf(acErrMsg, "登记柜员凭证明细错!");
        WRITEMSG
        goto ErrExit;
    }

    /* 登记柜员凭证最小号 */
    ret = pub_com_RegNoteMin(sNoteMst.br_no, sNoteMst.note_type, \
        sNoteMst.beg_note_no, sNoteMst.tel, cInOut);
    if (ret)
    {
        sprintf(acErrMsg, "登记凭证最小号错");
        WRITEMSG
        goto ErrExit;
    }

    strcpy(cNoteAct, "K");  /* 客户凭证行为: K购买 */

    /* 登记客户凭证台账 */
    ret = pub_com_RegCheq(sNoteMstTmp, g_pub_tx.ac_id, g_pub_tx.ac_seqn, \
        g_pub_tx.tx_date, cNoteAct);
    if (ret)
    {
        sprintf(acErrMsg, "登记客户凭证台账错!");
        WRITEMSG
        goto ErrExit;
    }

    /* 登记客户凭证台账明细 */
    sCheqMst.ac_id  = g_pub_tx.ac_id;
    sCheqMst.ac_seq = g_pub_tx.ac_seqn;
    sCheqMst.cnt    = (int)g_pub_tx.tx_amt1;
    strcpy(sCheqMst.br_no,       cOpenBrNo);
    strcpy(sCheqMst.note_type,   g_pub_tx.note_type);
    strcpy(sCheqMst.beg_note_no, g_pub_tx.beg_note_no);
    strcpy(sCheqMst.end_note_no, g_pub_tx.end_note_no);

    ret = pub_com_RegCheqHst(cNoteAct, sCheqMst, sNoteMstHst.tw_br_no, sNoteMstHst.tw_tel);
    if (ret == -1)
    {
        sprintf(acErrMsg, "登记客户凭证台账明细错!");
        WRITEMSG
        goto ErrExit;
    }

    g_pub_tx.tx_amt1 = 0.00;
    /* 流水登记凭证和凭证类型 del by wudawei 20131224
    strcpy(g_pub_tx.note_type, "150");
    strcpy(g_pub_tx.beg_note_no, "");
    strcpy(g_pub_tx.end_note_no, "");
		del end 20131224*/
    /* 记交易流水 */
    ret = pub_ins_trace_log();
    if (ret)
    {
        sprintf(acErrMsg, "记流水日志错");
        WRITEMSG
        goto ErrExit;
    }

OkExit:
    strcpy(g_pub_tx.reply, "0000");
    sprintf(acErrMsg, "Before OK return: reply is[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120", g_pub_tx.reply);
    return 0;
ErrExit:
    sprintf(acErrMsg, "Before return: reply is[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120", g_pub_tx.reply);
    return 1;
}

/*********************************************************************
* 函 数 名:  init_pub_tx
* 函数功能:  初始化g_pub_tx结构, 并且从屏幕取值放入g_pub_tx中
*
* 作    者:  andy
* 时    间:  2004年1月7日
*
* 参    数:
*     输入:
*
*     输出:
*
*   返回值:  0: 成功  -1: 失败
*
* 修改历史:  包括修改人、时间、修改内容, 以时间倒序方式
*
*********************************************************************/
static int init_pub_tx()
{
    long lNoteCnt;      /* 凭证数量 */

    pub_base_sysinit();
    memset(g_sell_way, 0x00, sizeof(g_sell_way));

    get_zd_data("0310", g_pub_tx.ac_no);
    get_zd_data("0890", g_pub_tx.note_type);
    get_zd_data("0670", g_sell_way);            /* 出售方式 */
    get_zd_data("0580", g_pub_tx.beg_note_no);
    get_zd_data("0590", g_pub_tx.end_note_no);
    get_zd_long("0520", &lNoteCnt);             /* 从前台域中取凭证数量 */
    g_pub_tx.tx_amt1 = (double)lNoteCnt;
    pub_base_old_acno(g_pub_tx.ac_no);
    set_zd_data("0310", g_pub_tx.ac_no);
    strcpy(g_pub_tx.brf, "凭证出售");
    return 0;
}

int wrt_NoteUse_log(ac_id, ac_seqn, note_type, beg_no, end_no, cnt)
    long ac_id,ac_seqn;
    char *note_type;
    char *beg_no;
    char *end_no;
    long cnt;
{
    struct pub_tx v_pub_tx;
    int ret = 0;

    memcpy(&v_pub_tx, &g_pub_tx, sizeof(g_pub_tx));

    g_pub_tx.ct_ind[0] = ' ';
    strcpy(g_pub_tx.ac_no, "702");
    g_pub_tx.ac_seqn = 0;
    g_pub_tx.ac_id   = 0;
    g_pub_tx.svc_ind = 7021;
    strcpy(g_pub_tx.note_type,   note_type);
    strcpy(g_pub_tx.beg_note_no, beg_no);
    strcpy(g_pub_tx.end_note_no, end_no);
    strcpy(g_pub_tx.prdt_code,   "");
    strcpy(g_pub_tx.cur_no,      "01");
    g_pub_tx.add_ind[0] = '0';
    g_pub_tx.hst_ind[0] = '1';
    g_pub_tx.tx_amt1 = (double)cnt;

    ret = pub_ins_trace_log();
    if (ret)
    {
        TRACE
        goto ErrExit;
    }

TRACE
    get_zd_double("0520", &g_pub_tx.tx_amt2);
    set_zd_double("0520", g_pub_tx.tx_amt1);
    memcpy(&g_pub_tx, &v_pub_tx, sizeof(g_pub_tx));
TRACE

    return 0;
ErrExit:
    return 1;
}
