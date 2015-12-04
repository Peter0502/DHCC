#include "find_debug.h"
/*********************************************************************
* 文 件 名:  pubf_com_cheq.c
* 功能描述:  客户凭证处理函数族
*
* 作    者:  jack
* 完成日期:  2004年12月16日
*
* 修改记录:
*   日    期:
*   修 改 人:
*   修改内容:
*********************************************************************/
#define MYSQLERR if (ret) { \
            sprintf(acErrMsg, "读数据库错[%d]", ret);\
            WRITEMSG \
            goto ErrExit; \
            }
#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "note_mst_c.h"
#include "note_mst_hst_c.h"
#include "note_min_no_c.h"
#include "note_cheq_mst_c.h"
#include "note_cheq_hst_c.h"
#include "note_parm_c.h"

/* 根据起止号码确定冠字头 */
int pub_com_ChkCheqHead_all(ac_id, ac_seqn, note_type, cheq_beg_no, cheq_end_no)
    long ac_id;
    int  ac_seqn;
    char *note_type;
    char *cheq_beg_no;
    char *cheq_end_no;
{
    struct note_parm_c sNoteParm;
    int len;
    int ret;

    pub_base_strpack(cheq_beg_no);
    pub_base_strpack(cheq_end_no);

    if (strlen(cheq_beg_no) < 16)
        ;
    else if (cheq_beg_no[0] != '0' && cheq_beg_no[0] != ' ')
        goto OkExit;
    else
        ;

    /* 根据凭证类型取凭证参数表 */
    ret = Note_parm_Sel(g_pub_tx.reply, &sNoteParm, \
          "note_type = '%s'", note_type);
    if (ret == 100)
    {
        sprintf(acErrMsg, "无此凭证类型note_type=[%s]", note_type);
        WRITEMSG
        strcpy(g_pub_tx.reply, "N001");
        goto ErrExit;
    }
    MYSQLERR

    /* 去掉冠字头后号码长度 */
/*
    len=strlen(cheq_beg_no);

    lCheqNo = atol(cheq_beg_no);
    sprintf(cForm, "%%0%dd", 16 - sNoteParm.head_len);
    sprintf(cCheqNo, cForm, lCheqNo);

    * 检查note_cheq_mst中是否存在号码为cheq_no的凭证 *
    ret = sql_count("note_cheq_mst", "ac_id = '%ld' and ac_seq =%d \
                and note_type = '%s' \
                and beg_note_no[%d,16]<='%s' and end_note_no[%d,16]>='%s'",\
                ac_id, ac_seqn, note_type, sNoteParm.head_len+1, cCheqNo, \
                sNoteParm.head_len+1, cCheqNo);
    if (ret < 0)
    {
        sprintf(acErrMsg,"执行sql_count错, ret=[%d]",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply, "D109");
        return(-1);
    }
    if (ret > 1)
    {
        sprintf(acErrMsg,"找到多条凭证,输入号码太短,ret=[%d]",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply, "N008");
        return 1;
    }
    if (ret == 0)
    {
        sprintf(acErrMsg,"柜员无此凭证note_no[%s]", cheq_no);
        WRITEMSG
        strcpy(g_pub_tx.reply, "N007");
        return(-1);
    }

    * 根据凭证号找起始凭证(带冠字头)*
    ret = Note_cheq_mst_Sel(g_pub_tx.reply, &sCheqMst, "ac_id = '%ld' and ac_seq = %d \
                    and note_type = '%s' and beg_note_no[%d,16]<='%s' and \
                    end_note_no[%d,16]>='%s'", ac_id, ac_seqn, note_type, \
                    sNoteParm.head_len+1, cCheqNo, sNoteParm.head_len+1, cCheqNo);
    if (ret == 100)
    {
        sprintf(acErrMsg,"执行Note_mst_Sel错,[%d]", ret);
        WRITEMSG
        strcpy(g_pub_tx.reply, "N012");
        return(-1);
    }else if (ret)
    {
        sprintf(acErrMsg,"执行Note_mst_Sel错,[%d]", ret);
        WRITEMSG
        return(-1);
    }

    * 取冠字头+凭证号码,返回 *
    strncpy(cHead, sCheqMst.beg_note_no, sNoteParm.head_len);
    cHead[sNoteParm.head_len] = '\0';

    strcpy(cheq_no, cHead);
    strcat(cheq_no, cCheqNo);
*/

OkExit:
    return 0;
ErrExit:
    return 1;
}

/*********************************************************************
* 函 数 名:  pub_com_ChkCheqHead
* 函数功能:  检查输入的客户凭证号码是否小于16位
*            如果小于16位, 则检查是否有重复的冠字头, 如果有, 返回错误提示,
*            如果没有, 则返回带冠字头的客户凭证号码
*            如果等于16位, 则返回客户凭证号码
*
* 作    者:  jack
* 时    间:  2004年12月27日
*
* 参    数:
*     输入:
*            long  ac_id       账户标识号
*            int   ac_seqn     账户序号
*            char  *note_type  凭证种类
*            char  *cheq_no    支票号码(可能带冠字头, 也可能不带)
*
*     输出:
*            char  *cheq_no    支票号码(带冠字头)
*
*   返回值:  0: 成功  1: 失败
*
* 修改历史:  包括修改人、时间、修改内容, 以时间倒序方式
*
*********************************************************************/

int pub_com_ChkCheqHead(
    long  ac_id,
    int   ac_seqn,
    char  *note_type,
    char  *cheq_no)
{
    struct  note_cheq_mst_c  sCheqMst;
    struct  note_parm_c      sNoteParm;
    char    cForm[32];
    char    cCheqNo[17];
    char    cHead[9];
    long    lCheqNo;          /* 去掉冠字头后支票号码 */
    int     iNoHeadLen;       /* 去掉冠字头后号码长度 */
    int     ret;

    /* 如果凭证号长度16位, 则直接返回 */
    if (strlen(cheq_no) == 16)
    {
        sprintf(acErrMsg, "凭证号长度是16位, 不检查冠字头[%s]", cheq_no);
        WRITEMSG
        return 0;
    }

    /* 根据凭证类型取凭证参数表 */
    ret = Note_parm_Sel(g_pub_tx.reply, &sNoteParm, \
          "note_type = '%s'", note_type);
    if (ret == 100)
    {
        sprintf(acErrMsg, "无此凭证类型note_type=[%s]", note_type);
        WRITEMSG
        strcpy(g_pub_tx.reply, "N001");
        return(-1);
    }else if (ret)
    {
        sprintf(acErrMsg, "执行Note_parm_Sel错![%d]", ret);
        WRITEMSG
        return(-1);
    }

    /* 去掉冠字头后号码长度 */
    lCheqNo = atol(cheq_no);            /* 凭证号码 */
    sprintf(cForm, "%%0%dd", 16 - sNoteParm.head_len);
    sprintf(cCheqNo, cForm, lCheqNo);

    /* 检查note_cheq_mst中是否存在号码为cheq_no的凭证 */
    ret = sql_count("note_cheq_mst", "ac_id = '%ld' and ac_seq = %d \
        and note_type = '%s' \
        and beg_note_no[%d,16] <= '%s' and end_note_no[%d,16] >= '%s'",\
        ac_id, ac_seqn, note_type, sNoteParm.head_len+1, cCheqNo, \
        sNoteParm.head_len+1, cCheqNo);
    if (ret < 0)
    {
        sprintf(acErrMsg,"执行sql_count错, ret=[%d]",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply, "D109");
        return(-1);
    }
    if (ret > 1)
    {
        sprintf(acErrMsg,"找到多条凭证,输入号码太短,ret=[%d]",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply, "N008");
        return 1;
    }
    if (ret == 0)
    {
        sprintf(acErrMsg, "柜员无此凭证note_no[%s]", cheq_no);
        WRITEMSG
        strcpy(g_pub_tx.reply, "N007");
        return(-1);
    }

    /* 根据凭证号找起始凭证(带冠字头)*/
    ret = Note_cheq_mst_Sel(g_pub_tx.reply, &sCheqMst, "ac_id = '%ld' and ac_seq = %d \
                    and note_type = '%s' and beg_note_no[%d,16]<='%s' and \
                    end_note_no[%d,16]>='%s'", ac_id, ac_seqn, note_type, \
                    sNoteParm.head_len+1, cCheqNo, sNoteParm.head_len+1, cCheqNo);
    if (ret == 100)
    {
        sprintf(acErrMsg,"执行Note_mst_Sel错,[%d]", ret);
        WRITEMSG
        strcpy(g_pub_tx.reply, "N012");
        return(-1);
    }else if (ret)
    {
        sprintf(acErrMsg,"执行Note_mst_Sel错,[%d]", ret);
        WRITEMSG
        return(-1);
    }

    /* 取冠字头+凭证号码,返回 */
    strncpy(cHead, sCheqMst.beg_note_no, sNoteParm.head_len);
    cHead[sNoteParm.head_len] = '\0';

    strcpy(cheq_no, cHead);
    strcat(cheq_no, cCheqNo);

    return(0);
}

/**********************************************************************
 * 函  数 名:   pub_com_ChkAcCheq
 * 函数 功能:   检查账号、凭证种类、凭证起始、终止号码是否存在,
 *             如果存在返回凭证状态,其中, 该号码区间的凭证必须是连续的,
 *             并且状态相同。
 *
 * 作者/时间:   jack/2004年12月27日
 *
 * 参  数:
 *     输入:
 *            long    ac_id       账户标识号
 *        int    ac_seqn     账号序号
 *            char   *note_type  凭证种类
 *            char   *cheq_beg_no    支票起始号码
 *            char   *cheq_end_no    支票终止号码
 *
 *     输出:
 *            char   *book_sts   凭证状态
 *
 *   返回值:   0: 有凭证   -1: 无凭证
 *
 * 修改历史:   包括修改人、时间、修改内容, 以时间倒序方式
 *
********************************************************************/

int pub_com_ChkAcCheq(
    long     ac_id,
    int     ac_seqn,
    char    *note_type,
    char    *cheq_beg_no,
    char    *cheq_end_no,
    char    *book_sts)
{
    struct note_parm_c sNoteParm;        /* 凭证参数 */
    struct note_cheq_mst_c sCheqMst;    /* 客户凭证台账 */

    char    cBegNoteNo[17], cEndNoteNo[17];
    char    str_tmp[17];
    char    cForm[32];            /* 输入格式 */
    char    cBookSts[2];            /* 支票状态 */

    long    lBegNoteNo, lEndNoteNo;    /* note_mst中的起始、终止号码 */
    long    lBegNo, lEndNo;    /* 总的起始、终止号码 */
    long    lDbBeg, lDbEnd; /* 查找数据库时候用到的区间段 */

    int    i, j, k;    /* 循环用 */
    int    iNoteCnt;    /* 可用凭证数量 */
    int    iFlag = 0;
    int    ret;

    MEMSET_DEBUG(&sNoteParm, 0x00, sizeof(struct note_parm_c));
    MEMSET_DEBUG(&sCheqMst, 0x00, sizeof(struct note_cheq_mst_c));

    MEMSET_DEBUG(cBegNoteNo, 0x00, sizeof(cBegNoteNo));
    MEMSET_DEBUG(cEndNoteNo, 0x00, sizeof(cEndNoteNo));
    MEMSET_DEBUG(cBookSts, 0x00, sizeof(cBookSts));

    /* 取凭证参数长度 */
    ret = Note_parm_Sel(g_pub_tx.reply, &sNoteParm, "note_type = '%s'", note_type);
    if (ret == 100)
    {
        sprintf(acErrMsg, "无此凭证类型note_type=[%s]", note_type);
        WRITEMSG
        strcpy(g_pub_tx.reply, "N001");
        return(-1);
    }else if (ret)
    {
        sprintf(acErrMsg, "执行Note_parm_Sel错![%d]", ret);
        WRITEMSG
        return(-1);
    }

    vtcp_log("cheq_beg_no的起始号[%16s]",cheq_beg_no);
    vtcp_log("cheq_end_no的终止号[%16s]",cheq_end_no);


    /* 去掉冠字头后凭证的起始号码和终止号码 */
    lBegNoteNo = atol(cheq_beg_no + sNoteParm.head_len);
    lEndNoteNo = atol(cheq_end_no + sNoteParm.head_len);

    vtcp_log("lBegNotoNo1的起始号[%ld]",lBegNoteNo);
    vtcp_log("lEndNotoNo1的终止号[%ld]",lEndNoteNo);

    /* 总的起始号码、终止号码 */
    vtcp_log("lBegNo的起始号1[%ld]",lBegNo);
    lBegNo = (lBegNoteNo - 1) / sNoteParm.unit_cnt * sNoteParm.unit_cnt + 1;
    lEndNo = ((lEndNoteNo-1) / sNoteParm.unit_cnt + 1) * sNoteParm.unit_cnt;
    vtcp_log("lBegNo的起始号[%ld]",lBegNo);
    vtcp_log("lEndNo的终止号[%ld]",lEndNo);
    for (i = lBegNo; i <= lEndNo; i = i + sNoteParm.unit_cnt)
    {
        /* 查找数据库用的凭证起始、终止号码 */
        lDbBeg = i;
        lDbEnd = i + sNoteParm.unit_cnt - 1;

        /* 加入冠字头 */
        strncpy(cBegNoteNo, cheq_beg_no, sNoteParm.head_len);
        strncpy(cEndNoteNo, cheq_end_no, sNoteParm.head_len);
        cBegNoteNo[sNoteParm.head_len] = '\0';
        cEndNoteNo[sNoteParm.head_len] = '\0';

        /* 带冠字头的起始号码 */
        sprintf(cForm, "%%0%dld", 16 - sNoteParm.head_len);
        sprintf(str_tmp, cForm, lDbBeg);
        strcat(cBegNoteNo, str_tmp);

        /* 带冠字头的终止号码 */
        sprintf(str_tmp, cForm, lDbEnd);
        strcat(cEndNoteNo, str_tmp);

        /* 根据账号、凭证种类、起始号码、终止号码查找数据库 */
        sprintf(acErrMsg,"ac_id=%ld and ac_seq=%d and note_type='%s' and beg_note_no<='%s' and end_note_no>='%s'",
                ac_id, ac_seqn, note_type, cBegNoteNo, cEndNoteNo);
            WRITEMSG
        ret = Note_cheq_mst_Dec_Upd(g_pub_tx.reply, "ac_id = %ld \
                and ac_seq = %d and note_type = '%s' \
                and beg_note_no<='%s' and end_note_no>='%s'", \
                ac_id, ac_seqn, note_type, cBegNoteNo, cEndNoteNo);
        if (ret)
        {
            sprintf(acErrMsg,
                "ac_id=%ld and ac_seq=%d and note_type='%s' and beg_note_no<='%s' and end_note_no>='%s'",
                ac_id, ac_seqn, note_type, cBegNoteNo, cEndNoteNo);
            WRITEMSG
            return(-1);
        }

        ret = Note_cheq_mst_Fet_Upd(&sCheqMst, g_pub_tx.reply);
        if (ret == 100)    /* 没找到这样的记录 */
        {
            sprintf(acErrMsg, "部分凭证不存在");
            WRITEMSG
        strcpy(g_pub_tx.reply, "N069");
            return(-1);
        }
        else if (ret)
        {
            sprintf(acErrMsg, "执行Note_cheq_mst_Fet_Upd错!ret= [%d]",ret);
            WRITEMSG
            return(-1);
        }
        else
        {
sprintf(acErrMsg, "lBegNoteNo=[%ld],lEndNoteNo=[%ld]",lBegNoteNo,lEndNoteNo);
WRITEMSG
sprintf(acErrMsg, "lBeg=[%ld],lEnd=[%ld][%s]",lDbBeg,lDbEnd,sCheqMst.book_sts);
WRITEMSG
            for (j = lDbBeg, k = 0; j <= lDbEnd; j++, k++)
            {
                if (j >= lBegNoteNo && j <= lEndNoteNo)
                {
sprintf(acErrMsg, "lBeglEnd=[%ld] k[%d]",j,k);
WRITEMSG
                    iFlag++;
                    if (iFlag == 1)
                    {
                        cBookSts[0] = sCheqMst.book_sts[k];
                    }
                    if (sCheqMst.book_sts[k] == '0')
                    {
                        sprintf(acErrMsg, "部分凭证不存在");
                        WRITEMSG
                        strcpy(g_pub_tx.reply, "N069");
                        return(-1);
                    }else
                    {
                        if (cBookSts[0] != sCheqMst.book_sts[k])
                        {
                            sprintf(acErrMsg, "凭证状态不一致");
                            WRITEMSG
                            strcpy(g_pub_tx.reply, "N070");
                            return(-1);
                        }
                    }
                    cBookSts[0] = sCheqMst.book_sts[k];
                }
            } /* end for */
            strcpy(book_sts, cBookSts);
        }
        Note_cheq_mst_Clo_Upd();
    }/* end for */

    return(0);
}

/**********************************************************************
 * 函  数 名:   pub_com_RegCheq
 * 函数 功能:   登记支票登记簿/修改凭证状态, 按照支票的号码登记对应的记录。如果已经存在这本
 *             凭证, 则需要修改对应状态、凭证数量、是否占用本数、购买日期。如
 *             不存在, 则新增加一条记录既可。
 *
 *             支票转移/出售/使用/作废
 *                   0:无  1:出售未使用  2:挂失  3:回收  4:作废5使用
 *
 * 作者/时间:   jack//2004年12月27日
 *
 * 参  数:
 *     输入:
 *            struct note_mst_c note_mst  柜员凭证台账
 *            long ac_id       账户id
 *        long ac_seqn  账号序号
 *            long tx_date     交易日期
 *            char *note_act   客户凭证行为
 *
 *     输出:
 *
 *   返回值:   0: 成功   -1: 失败
 *
 * 修改历史:   包括修改人、时间、修改内容, 以时间倒序方式
 *
********************************************************************/
int pub_com_RegCheq(
    struct note_mst_c note_mst,
    long ac_id,
    int ac_seqn,
    long tx_date,
    char *note_act)
{
    struct note_mst_c sNoteMst;         /* 柜员凭证台账 */
    struct note_parm_c sNoteParm;       /* 凭证参数 */
    struct note_cheq_mst_c sCheqMst;    /* 客户凭证台账 */

    char    cBegNoteNo[17], cEndNoteNo[17];
    char    cHead[17];            /* 冠字头 */
    char    str_tmp[17];
    char    cForm[32];            /* 输入格式 */
    char    cBookSts[51];         /* 支票状态 */
    char    cABookSts[2];     /* 支票执行之后的状态 */
    char    cBBookSts[2];     /* 支票执行之前的状态 */
    char    cSts[2];              /* 临时保存状态, 用来比较数量的增减 */
    char    cWho[2];          /* 凭证执行者 */

    long    lBegNoteNo, lEndNoteNo;    /* note_mst中的起始、终止号码 */
    long    lBegNo, lEndNo;     /* 总的起始、终止号码 */
    long    lDbBeg, lDbEnd;     /* 查找数据库时候用到的区间段 */

    int    i, j, k, m = 0;      /* 循环用,m 用来记录总共循环几次 */
    int    iNoteCnt;        /* 可用凭证数量 */
    int    ret;

    MEMSET_DEBUG(&sNoteMst, 0x00, sizeof(struct note_mst_c));
    MEMSET_DEBUG(&sNoteParm, 0x00, sizeof(struct note_parm_c));
    MEMSET_DEBUG(&sCheqMst, 0x00, sizeof(struct note_cheq_mst_c));

    MEMSET_DEBUG(cBegNoteNo, 0x00, sizeof(cBegNoteNo));
    MEMSET_DEBUG(cEndNoteNo, 0x00, sizeof(cEndNoteNo));
    MEMSET_DEBUG(cBookSts, 0x00, sizeof(cBookSts));
    MEMSET_DEBUG(cABookSts, 0x00, sizeof(cABookSts));
    MEMSET_DEBUG(cBBookSts, 0x00, sizeof(cBBookSts));

    MEMCPY_DEBUG(&sNoteMst, &note_mst, sizeof(struct note_mst_c));
    strcpy(cWho , "C");     /* 表示客户 */

    /* 取凭证参数长度 */
    ret = Note_parm_Sel(g_pub_tx.reply, &sNoteParm, "note_type = '%s'", sNoteMst.note_type);
    if (ret == 100)
    {
        sprintf(acErrMsg, "无此凭证类型note_type=[%s]", sNoteMst.note_type);
        WRITEMSG
        strcpy(g_pub_tx.reply, "N001");
        return(-1);
    }else if (ret)
    {
        sprintf(acErrMsg, "执行Note_parm_Sel错![%d]", ret);
        WRITEMSG
        return(-1);
    }

    /* 将凭证状态痴思化成0 */
    MEMSET_DEBUG(&cBookSts, '0', sNoteParm.unit_cnt);

    /* 根据客户凭证行为取执行之后的凭证状态 */
    ret = pub_com_NoteActSts(cWho, note_act, cABookSts, cBBookSts);
    if (ret)
    {
        sprintf(acErrMsg, "根据凭证行为取凭证状态错!");
        WRITEMSG
        strcpy(g_pub_tx.reply, "N002");
        return(-1);
    }

    /* 去掉冠字头后凭证的起始号码和终止号码 */
    lBegNoteNo = atol(sNoteMst.beg_note_no + sNoteParm.head_len);
    lEndNoteNo = atol(sNoteMst.end_note_no + sNoteParm.head_len);

    /* 总的起始号码、终止号码 */
    lBegNo = (lBegNoteNo - 1) / sNoteParm.unit_cnt * sNoteParm.unit_cnt + 1;
    lEndNo = ((lEndNoteNo-1) / sNoteParm.unit_cnt + 1) * sNoteParm.unit_cnt;

    for (i = lBegNo; i <= lEndNo; i = i + sNoteParm.unit_cnt)
    {
        m++;

        /* 查找数据库用的凭证起始、终止号码 */
        lDbBeg = i;
        lDbEnd = i + sNoteParm.unit_cnt - 1;

        /* 加入冠字头 */
        strncpy(cBegNoteNo, sNoteMst.beg_note_no, sNoteParm.head_len);
        strncpy(cEndNoteNo, sNoteMst.end_note_no, sNoteParm.head_len);
        cBegNoteNo[sNoteParm.head_len] = '\0';
        cEndNoteNo[sNoteParm.head_len] = '\0';
        /* 带冠字头的起始号码 */
        sprintf(cForm, "%%0%dld", 16 - sNoteParm.head_len);
        sprintf(str_tmp, cForm, lDbBeg);
        strcat(cBegNoteNo, str_tmp);

        /* 带冠字头的终止号码 */
        sprintf(str_tmp, cForm, lDbEnd);
        strcat(cEndNoteNo, str_tmp);

        /* 根据账号、凭证种类、起始号码、终止号码查找数据库 */
        ret = Note_cheq_mst_Dec_Upd(g_pub_tx.reply, "ac_id = %ld \
                and ac_seq = %d and note_type = '%s' \
                and beg_note_no = '%s' and end_note_no = '%s'", \
                ac_id, ac_seqn, sNoteMst.note_type, cBegNoteNo, cEndNoteNo);
        if (ret)
        {
            sprintf(acErrMsg, "执行Note_cheq_mst_Dec_Upd错!ret= [%d]",ret);
            WRITEMSG
            return(-1);
        }
        ret = Note_cheq_mst_Fet_Upd(&sCheqMst, g_pub_tx.reply);
        if (ret == 100)    /* 没找到这样的记录 */
        {
            /* 如果note_act=K,则表示新增加凭证 */
            if (note_act[0] != 'K')
            {
                sprintf(acErrMsg, "客户无此凭证");
                WRITEMSG
                sprintf(g_pub_tx.reply, "N012");
                return(-1);
            }else
            {
                /* 增加一条记录 */
                sCheqMst.ac_id = ac_id;    /* 账号id */
                sCheqMst.ac_seq = ac_seqn;
        strcpy(sCheqMst.br_no, g_pub_tx.tx_br_no);
                strcpy(sCheqMst.note_type, sNoteMst.note_type);
                strcpy(sCheqMst.beg_note_no, cBegNoteNo);
                strcpy(sCheqMst.end_note_no, cEndNoteNo);
                strcpy(sCheqMst.book_sts, cBookSts);
                sCheqMst.sell_date = tx_date;
                iNoteCnt = 0;

                sprintf(acErrMsg, "33333333 lBegNoteNo=[%d], lEndNoteNo=[%d]", lBegNoteNo, lEndNoteNo);
                WRITEMSG
                sprintf(acErrMsg, "33333333 sCheqMst.cnt=[%d]", sCheqMst.cnt);
                WRITEMSG
                for (j = lDbBeg, k = 0; j <= lDbEnd; j++, k++)
        {
                    if (j >= lBegNoteNo && j <= lEndNoteNo)
                    {
                        sCheqMst.book_sts[k] = cABookSts[0];
                            iNoteCnt++;
                    }
                } /* end for */
                sprintf(acErrMsg, "33333333 iNoteCnt=[%d]", iNoteCnt);
                WRITEMSG
                sCheqMst.cnt = iNoteCnt;
                /* 如果可用凭证数量超过规定数量, 则占用本数 */
                if (sCheqMst.cnt >= sNoteParm.ocp_cnt)
                {
                    sCheqMst.grp_ind[0] = 'Y';
                }else
                {
                    sCheqMst.grp_ind[0] = 'N';
                }
sprintf(acErrMsg, "111111111111sCheqMst.grp_ind=[%s],sNoteParm.ocp_cnt=[%d] ",\
    sCheqMst.grp_ind,sNoteParm.ocp_cnt);
WRITEMSG
                /* 写入数据库表中 */
            ret = Note_cheq_mst_Ins(sCheqMst, g_pub_tx.reply);
            if (ret == -239)
            {
                sprintf(acErrMsg, "向客户凭证台账插入数据错,有重复记录");
                WRITEMSG
                strcpy(g_pub_tx.reply, "N021");
                return(-1);
            }else if (ret)
            {
                sprintf(acErrMsg, "执行Note_cheq_mst_Ins错");
                WRITEMSG
                return(-1);
            }
        }
        }else if (ret)
        {
            sprintf(acErrMsg, "执行    Note_cheq_mst_Fet_Upd错!ret= [%d]",ret);
            WRITEMSG
            return(-1);
        }else
        {
            /* 修改标志 */
            iNoteCnt = 0;
            if (m == 1)  /* 用来记录第一个凭证的状态 */
            {
                cSts[0] = sCheqMst.book_sts[lBegNoteNo - lDbBeg];
            }

            for (j = lDbBeg, k = 0; j <= lDbEnd; j++, k++)
            {
                if (j >= lBegNoteNo && j <= lEndNoteNo)
                {
                    sCheqMst.book_sts[k] = cABookSts[0];
                    iNoteCnt++;
                }
            } /* end for */

            /* 如果凭证行为是购买或者解挂, 凭证数增加 */
            /*
            if (note_act[0] == 'K' || note_act[0] == 'C')
            {
                sCheqMst.cnt += iNoteCnt;
            }else if (cSts[0] == '1' && (note_act[0] != 'K'||note_act[0] != 'C'))
            {
                sCheqMst.cnt -= iNoteCnt;
            }
            */
            if ((cBBookSts[0] == '5' || cBBookSts[0] == '0') && cABookSts[0] == '1')
            {
                sCheqMst.cnt += iNoteCnt;
            }else if ((cABookSts[0] == '5' || cABookSts[0] == '3' || cABookSts[0] == '4') && cBBookSts[0] == '1')
            {
                sCheqMst.cnt -= iNoteCnt;
            }

            /* 如果可用凭证数量超过规定数量, 则占用本数 */
            if (sCheqMst.cnt >= sNoteParm.ocp_cnt)
            {
                sCheqMst.grp_ind[0] = 'Y';
            }else
            {
                sCheqMst.grp_ind[0] = 'N';
            }
            sCheqMst.sell_date = tx_date;
sprintf(acErrMsg, "2222222222sCheqMst.grp_ind=[%s],sNoteParm.ocp_cnt=[%d]",\
    sCheqMst.grp_ind,sNoteParm.ocp_cnt);
WRITEMSG
            /* 修改数据 */
            ret = Note_cheq_mst_Upd_Upd(sCheqMst, g_pub_tx.reply);
            if (ret == -239)
            {
                sprintf(acErrMsg, "修改客户凭证台账数据错,有重复记录");
                WRITEMSG
                strcpy(g_pub_tx.reply, "N021");
                return(-1);
            }else if (ret)
            {
                sprintf(acErrMsg, "执行Note_cheq_mst_Upd_Upd错");
                WRITEMSG
                return(-1);
            }
        }
        Note_cheq_mst_Clo_Upd();
    }/* end for */

    return(0);
}

/**********************************************************************
 * 函 数 名:   pub_com_RegCheqHst
 * 函数功能:   客户凭证变更登记处理。客户凭证变更的时候都要登记note_cheq_hst
 * 作者/时间:  jack/2004年12月26日
 *
 * 参数:
 *     输入:
 *            char      *note_act      凭证行为
 *            struct note_cheq_mst_c cheq_mst 客户凭证台账
 *            char    *tw_br_no    对方机构
 *          char    *tw_tel        对方操作员
 *
 *     输出:  char     *g_pub_tx.reply         相应码
 *
 *   返回值:   0: 登记成功 -1: 登记失败
 *
 * 修改历史:   包括修改人、时间、修改内容, 以时间倒序方式
 *
********************************************************************/

int pub_com_RegCheqHst(note_act, cheq_mst, tw_br_no, tw_tel)
    char   *note_act;
    struct note_cheq_mst_c   cheq_mst;
    char   *tw_br_no;
    char   *tw_tel;
{
    struct note_cheq_hst_c sCheqHst;
    int    ret;

    MEMSET_DEBUG(&sCheqHst, 0x00, sizeof(struct note_cheq_hst_c));

    sCheqHst.ac_id = cheq_mst.ac_id;                           /* 账户id */
    sCheqHst.ac_seq = cheq_mst.ac_seq;                /* 账户序号 */
    strcpy(sCheqHst.note_type, cheq_mst.note_type);        /* 凭证类型 */
    strcpy(sCheqHst.tx_br_no, g_pub_tx.tx_br_no);        /* 机构编码 */
    strcpy(sCheqHst.beg_note_no, cheq_mst.beg_note_no);    /* 起始号码 */
    strcpy(sCheqHst.end_note_no, cheq_mst.end_note_no);    /* 终止号码 */
    strcpy(sCheqHst.use_ind, note_act);            /* 使用标志 */
    sCheqHst.cnt = cheq_mst.cnt;                /* 凭证数量 */
    sCheqHst.tx_date = g_pub_tx.tx_date;            /* 交易日期 */
    sCheqHst.tx_time = g_pub_tx.tx_time;            /* 交易时间 */
    sCheqHst.trace_no = g_pub_tx.trace_no;            /* 流 水 号 */
    sCheqHst.trace_cnt = g_pub_tx.trace_cnt;            /* 流水笔次 */
    strcpy(sCheqHst.tel, g_pub_tx.tel);                /* 操作柜员 */
    strcpy(sCheqHst.chk, g_pub_tx.chk);            /* 复核员   */
    strcpy(sCheqHst.auth, g_pub_tx.auth);            /* 授权人   */

    ret = Note_cheq_hst_Ins(sCheqHst, g_pub_tx.reply);
    if (ret == -239)
    {
        sprintf(acErrMsg,"向客户凭证明细表插入记录错!有重复记录!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"N024");
        return(-1);
    }else if (ret)
    {
        sprintf(acErrMsg,"执行Note_cheq_hst_Ins错!ret=[%d]",ret);
        WRITEMSG
        return(-1);
    }

    return(0);
}

/**********************************************************************
 * 函 数 名:   pub_com_ChkCheqSell
 * 函数功能:   出售凭证时检查凭证的有效性。
 *            1、根据凭证类型取凭证参数；
 *            2、判断是否存在这种凭证；
 *            3、判断是否可以出售给客户；
 *            4、判断是否是可用标志；
 *            5、判断是否是使用客户类型；
 *            6、判断是否每次可出售本数
 *            7、判断是否超过每次可出售张数
 *            8、判断是否累计可出售本数
 *            9、判断是否累计每次可出售张数
 *
 * 作者/时间:  jack/2004年12月26日
 *
 * 参数:
 *     输入:
 *            long      ac_id           账号id
 *      int ac_seqn
 *            char    *note_type    凭证种类
 *          char    *beg_note_no    凭证号码
 *      char    *end_note_no
 *
 *     输出:  char     *g_pub_tx.reply         相应码
 *
 *   返回值:   0: 成功 -1: 失败
 *
 * 修改历史:   包括修改人、时间、修改内容, 以时间倒序方式
 *
********************************************************************/
int pub_com_ChkCheqSell(
        long ac_id,
        int ac_seqn,
        char *note_type,
        char *beg_note_no,
        char *end_note_no)
{
    struct note_parm_c sNoteParm;

    int    iGrp;        /* 凭证本数 */
    int    iCnt;        /* 凭证张数 */
    int    iGrp_Count;  /* 累计凭证本数 */
    long   iCnt_Count;  /* 累计凭证张数 */
    long   ret;

    MEMSET_DEBUG(&sNoteParm, 0x00, sizeof(struct note_parm_c));

    /* 根据账号id取客户类型 */

    /* 根据凭证种类取凭证参数 */
    /* 1:判断是否存在该凭证类型 */
    ret = Note_parm_Sel(g_pub_tx.reply, &sNoteParm, "note_type = '%s'", note_type);
    if (ret == 100)
    {
        sprintf(acErrMsg, "无此凭证类型note_type=[%s]", note_type);
        WRITEMSG
        strcpy(g_pub_tx.reply, "N001");
        return(-1);
    }else if (ret)
    {
        sprintf(acErrMsg, "执行Note_parm_Sel错![%d]", ret);
        WRITEMSG
        return(-1);
    }

    /* 2:检查是否可以出售给客户 */
    if (sNoteParm.sell_ind[0] == 'N')
    {
        sprintf(acErrMsg, "该凭证类型不能出售给客户note_type=[%s]", note_type);
        WRITEMSG
        strcpy(g_pub_tx.reply, "N016");
        return(-1);
    }

    /* 4:判断是否是可用标志 */
    if (sNoteParm.use_ind[0] == 'N')
    {
        sprintf(acErrMsg, "该凭证类型是停止使用状态note_type=[%s]", note_type);
        WRITEMSG
        strcpy(g_pub_tx.reply, "N025");
        return(-1);
    }

    /* 计算本次凭证张数 */
    iCnt = pub_com_CalNoteCnt(note_type, beg_note_no, end_note_no);
    if (iCnt == -1)
    {
        sprintf(acErrMsg, "计算凭证数量错,note_type=[%s],beg_note_no=[%s],\
                end_note_no=[%s]",note_type,beg_note_no, end_note_no);
        WRITEMSG
        strcpy(g_pub_tx.reply, "N010");
        return(-1);
    }

    sprintf(acErrMsg,"oooooooooooo  iCnt[%d]", iCnt);WRITEMSG

    /* 判断是否超过规定张数 */
    if (iCnt > sNoteParm.buy_cnt)
    {
        sprintf(acErrMsg, "出售凭证数量超过规定数量!note_type=[%s],beg_note_no=[%s],\
                end_note_no=[%s]",note_type,beg_note_no, end_note_no);
        WRITEMSG
        strcpy(g_pub_tx.reply, "N026");
        return(-1);
    }

    /* 计算本次凭证本数 */
    iGrp = pub_com_CalPileCnt(sNoteParm, beg_note_no, end_note_no);
    if (iGrp == -1)
    {
        sprintf(acErrMsg, "计算凭证本数错");
        WRITEMSG
        return(-1);
    }
    /* 判断是否每次可出售本数 */
    if (iGrp > sNoteParm.buy_unit)
    {
        sprintf(acErrMsg, "出售凭证本数超过了限制本数");
        WRITEMSG
        strcpy(g_pub_tx.reply, "N029");
        return(-1);
    }

    /* 计算累计凭证张数 */
    iCnt_Count = pub_com_CalAcCnt(ac_id, ac_seqn, note_type);
    if (iCnt_Count == -1)
    {
        sprintf(acErrMsg, "计算凭证数量错,note_type=[%s]", note_type);
        WRITEMSG
        strcpy(g_pub_tx.reply, "N010");
        return(-1);
    }

    sprintf(acErrMsg,"oooooooooooo  iCnt_Count[%l]", iCnt_Count);WRITEMSG

    /* 判断是否超过累计张数 */
    if ((iCnt_Count + iCnt) > sNoteParm.cnt_lnt)
    {
        sprintf(acErrMsg, "客户持有凭证超过规定数量!note_type=[%s],计划购买[%d]\
                累计持有[%l]",note_type,iCnt, iCnt_Count);
        WRITEMSG
        strcpy(g_pub_tx.reply, "N032");
        return(-1);
    }

    /* 计算累计凭证本数 */
    iGrp_Count = pub_com_CalAcGrp(ac_id, ac_seqn, note_type);
    if (iGrp_Count == -1)
    {
        sprintf(acErrMsg, "计算凭证数量错,note_type=[%s]",note_type);
        WRITEMSG
        strcpy(g_pub_tx.reply, "N010");
        return(-1);
    }

    /* 判断是否超过累计本数 */
    if ((iGrp_Count + iGrp) > sNoteParm.unit_lmt)
    {
        sprintf(acErrMsg, "客户持有凭证超过规定数量!note_type=[%s],计划购买[%d]\
                累计持有[%l]",note_type,iGrp, iGrp_Count);
        WRITEMSG
        strcpy(g_pub_tx.reply, "N033");
        return(-1);
    }

    return(0);
}

/**********************************************************************
 * 函 数 名:   pub_com_CalPileCnt
 * 函数功能:   根据凭证起始号码、终止号码确定凭证本数
 *
 * 作者/时间:  jack/2004年12月26日
 *
 * 参数:
 *     输入:
 *            struct note_parm_c note_parm 凭证参数
 *            char    *beg_note_no    起始号码
 *            char    *end_note_no    终止号码
 *
 *     输出:
 *
 *   返回值:   >0: 成功(凭证本数) -1: 失败
 *
 * 修改历史:   包括修改人、时间、修改内容, 以时间倒序方式
 *
********************************************************************/

int pub_com_CalPileCnt(
        struct note_parm_c note_parm,
        char    *beg_note_no,
        char    *end_note_no)
{
    char    cBegHead[17], cEndHead[17];    /* 起始号码、终止号码的冠字头 */

    long    lBegNo, lEndNo;
    long    lBeg, lEnd;

    int    iPile = 0;    /* 本数 */
    int    ret;

    /* 取冠字头 */
    strncpy(cBegHead, beg_note_no, note_parm.head_len);
    strncpy(cEndHead, end_note_no, note_parm.head_len);
    cBegHead[note_parm.head_len] = '\0';
    cEndHead[note_parm.head_len] = '\0';

    if (strcmp(cBegHead, cEndHead))
    {
        sprintf(acErrMsg, "两个凭证号码的冠字头不同");
        WRITEMSG
        strcpy(g_pub_tx.reply, "N027");
        return(-1);
    }
    /* 去掉冠字头后起始号码、终止号码 */
    lBegNo = atol(beg_note_no + note_parm.head_len);
    lEndNo = atol(end_note_no + note_parm.head_len);

    if (lBegNo > lEndNo)
    {
        sprintf(acErrMsg, "起始号码大于终止号码");
        WRITEMSG
        strcpy(g_pub_tx.reply, "N028");
        return(-1);
    }

    /* 计算总的起始、终止号码 */
    lBeg = (lBegNo - 1)/note_parm.unit_cnt * note_parm.unit_cnt +1;
    lEnd = ((lEndNo - 1)/note_parm.unit_cnt + 1) * note_parm.unit_cnt;

    /* 计算本数 */
    iPile = (lEnd - lBeg + 1) / note_parm.unit_cnt;

    return iPile ;
}

/**********************************************************************
 * 函 数 名:   pub_com_CalAcGrp
 * 函数功能:   查找账户id所拥有的凭证本数
 *
 * 作者/时间:  jack/2004年12月28日
 *
 * 参数:
 *     输入:
 *            long    ac_id;       账户id
 *      int ac_seqn
 *            char    *note_type    凭证类型
 *
 *     输出:
 *
 *   返回值:   >0: 成功(凭证本数) -1: 失败
 *
 * 修改历史:   包括修改人、时间、修改内容, 以时间倒序方式
 *
********************************************************************/

int pub_com_CalAcGrp(
        long    ac_id,
        int ac_seqn,
        char    *note_type)
{
    int    ret;

    ret = sql_count("note_cheq_mst", "ac_id = %ld and ac_seq = %d \
            and note_type = '%s' and grp_ind = '%s' and cnt > 0", ac_id, ac_seqn, note_type, "Y");
    if (ret < 0)
    {
        sprintf(acErrMsg, "计算账号凭证本数错ac_id=[%ld],note_type=[%s]", \
                 ac_id, note_type);
        WRITEMSG
        return(-1);
    }

    return ret;
}

/**********************************************************************
 * 函 数 名:   pub_com_CalAcCnt
 * 函数功能:   查找账户id所拥有的凭证可用张数
 *
 * 作者/时间:  jack/2004年12月28日
 *
 * 参数:
 *     输入:
 *            long    ac_id;       账户id
 *            char    *note_type    凭证类型
 *
 *     输出:
 *
 *   返回值:   >0: 成功(凭证本数) -1: 失败
 *
 * 修改历史:   包括修改人、时间、修改内容, 以时间倒序方式
 *
********************************************************************/

int pub_com_CalAcCnt(
        long    ac_id,
        int ac_seqn,
        char    *note_type)
{
    long    lSumCnt;
    int    ret;

    ret = sql_sum_long("note_cheq_mst", "cnt", &lSumCnt, "ac_id= %ld \
                    and ac_seq = %d and note_type = '%s'", \
                    ac_id, ac_seqn, note_type);
    if (ret == -1)
    {
        sprintf(acErrMsg, "求和可用凭证张数错!ac_id=[%ld],note_type=[%s]",\
                 ac_id, note_type);
        WRITEMSG
        strcpy(g_pub_tx.reply, "N010");
        return(-1);
    }

    return lSumCnt;
}

/**********************************************************************
 * 函 数 名:   pub_com_ChkPile
 * 函数功能:   根据凭证起始号码、终止号码确定是否是整本凭证
 *
 * 作者/时间:  jack/2003年1月11日
 *
 * 参数:
 *     输入:
 *        char    *note_type      凭证类型
 *            char    *beg_note_no    起始号码
 *            char    *end_note_no    终止号码
 *
 *     输出:
 *
 *   返回值:   0: 是 -1: 否
 *
 * 修改历史:   包括修改人、时间、修改内容, 以时间倒序方式
 *
********************************************************************/

int pub_com_ChkPile(
    char    *note_type,
        char    *beg_note_no,
        char    *end_note_no)
{
    struct note_parm_c sNoteParm;

    char    cBegHead[17], cEndHead[17];    /* 起始号码、终止号码的冠字头 */

    long    lBegNo, lEndNo;
    long    lBeg, lEnd;

    int    iPile = 0;    /* 本数 */
    int    ret;

    MEMSET_DEBUG(&sNoteParm, 0x00, sizeof(struct note_parm_c));
    MEMSET_DEBUG(cBegHead, 0x00, sizeof(cBegHead));
    MEMSET_DEBUG(cEndHead, 0x00, sizeof(cEndHead));

    /* 根据凭证种类取凭证参数 */
    ret = Note_parm_Sel(g_pub_tx.reply, &sNoteParm, "note_type = '%s'", note_type);
    if (ret == 100)
    {
        sprintf(acErrMsg, "无此凭证类型note_type=[%s]", note_type);
        WRITEMSG
        strcpy(g_pub_tx.reply, "N001");
        return(-1);
    }else if (ret)
    {
        sprintf(acErrMsg, "执行Note_parm_Sel错![%d]", ret);
        WRITEMSG
        return(-1);
    }

    /* 取冠字头 */
    strncpy(cBegHead, beg_note_no, sNoteParm.head_len);
    strncpy(cEndHead, end_note_no, sNoteParm.head_len);
    cBegHead[sNoteParm.head_len] = '\0';
    cEndHead[sNoteParm.head_len] = '\0';

    /* 判断输入的起始号码和终止号码冠字头是否相同 */
    if (strcmp(cBegHead, cEndHead))
    {
        sprintf(acErrMsg, "两个凭证号码的冠字头不同");
        WRITEMSG
        strcpy(g_pub_tx.reply, "N027");
        return(-1);
    }

    /* 去掉冠字头后起始号码、终止号码 */
    lBegNo = atol(beg_note_no + sNoteParm.head_len);
    lEndNo = atol(end_note_no + sNoteParm.head_len);

    /* 如果起始号码大于终止号码, 则重新输入 */
    if (lBegNo > lEndNo)
    {
        sprintf(acErrMsg, "起始号码大于终止号码");
        WRITEMSG
        strcpy(g_pub_tx.reply, "N028");
        return(-1);
    }

    if (((lBegNo % sNoteParm.unit_cnt) != 1)
        || ((lEndNo % sNoteParm.unit_cnt) != 0))
    {
        sprintf(acErrMsg, "输入号码不是整本凭证的开始和结束号码");
        WRITEMSG
        strcpy(g_pub_tx.reply, "N052");
        return(-1);
    }

    return(0);
}
