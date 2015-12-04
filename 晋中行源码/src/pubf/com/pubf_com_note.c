#include "find_debug.h"
/*************************************************
* 文 件 名:  pubf_com_note.c
* 功能描述:  凭证处理函数族
*            pub_com_ChkNote    凭证检查处理
*
* 作    者:  jack
* 完成日期:  2003年12月16日
*
* 修改记录:
*   日    期:
*   修 改 人:
*   修改内容:
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "note_cheq_mst_c.h"
#include "note_cheq_hst_c.h"
#include "note_mst_c.h"
#include "note_mst_hst_c.h"
#include "note_min_no_c.h"
#include "note_parm_c.h"
#include "com_tx_br_rel_c.h"

/*********************************************************************
* 函 数 名:  pub_com_ChkNote
* 函数功能:  根据凭证行为判断凭证状态是否符合要求。
*
*       柜员凭证行为(前状态/本次状态)
*       1入库(无/0)  2下发(付出)(0/2)  3领入(收入)(2/0)  4凭证转移转出(0/2)
*       5凭证转移转入(2/0)  6出售(0/5)  7正常上缴(0/2)  8作废上缴(6/3)
*       9正常收缴(2/0)  A作废收缴(3/6)  B挂失(0/1)  C解挂(1/0)
*       D使用(0/4)  E作废(0/6)  F出售收回(5/6)  M 销毁(6/ *)  N撤消(4/6)
*
*       客户凭证行为(前状态/本次状态)
*       B挂失(1/2)  C解挂(2/1)  G客户使用(1/5)  H注销(1/4)  F出售收回(1/3)
*       N撤消(5/1)
*
*       柜员凭证状态(前状态/本次状态)
*       0正常  1挂失  2调出待收(正常)  3调出待收(作废)  *销毁
*       5出售  4柜员使用  6作废/出售回收
*
*       客户凭证状态
*       0无  1出售未使用  2挂失  3回收  4作废  5使用
*
* 作者/时间:  2003年12月16日
*
* 参  数:
*     输入:  char *fpNoteAct              凭证行为
*            struct note_mst_c fsNoteMst  柜员凭证
*
*     输出:  char g_pub_tx.reply[]        响应码
*
*   返回值:  0: 检查成功  1: 不成功
*
* 修改历史:  包括修改人、时间、修改内容, 以时间倒序方式
*
*********************************************************************/

int pub_com_ChkNote(char *fpNoteAct, struct note_mst_c fsNoteMst)
{
    struct note_parm_c      sNoteParm;  /* 凭证参数 */
    struct note_cheq_mst_c  sCheqMst;   /* 客户凭证台帐 */
    int    num, num1;
    int    i, from, to, qshm, zzhm;     /* qshm: 起始号码  zzhm: 终止号码 */
    int    ret;
    /*char   cHead[9];                     冠字头 */
    char   cHead[13];
    char   cPerNoteSts[2];
    char   cNoteSts[2];
    char   cWho[2];

    MEMSET_DEBUG(&sNoteParm,  0x00, sizeof(struct note_parm_c));
    MEMSET_DEBUG(cPerNoteSts, 0x00, sizeof(cPerNoteSts));
    MEMSET_DEBUG(cNoteSts,    0x00, sizeof(cNoteSts));
    MEMSET_DEBUG(cWho,        0x00, sizeof(cWho));

    num = 0;
    num1= 0;

    /* 根据凭证类型取凭证结构 */
    ret = Note_parm_Sel(g_pub_tx.reply, &sNoteParm, "note_type = '%s'", fsNoteMst.note_type);
    if(ret == 100)
    {
        sprintf(acErrMsg, "取凭证参数note_parm表错! note_type=[%s]", fsNoteMst.note_type);
        WRITEMSG
        strcpy(g_pub_tx.reply, "N001");
        return (-1);
    }else if (ret)
    {
        sprintf(acErrMsg, "执行Note_parm_Sel错误! ret=[%d]", ret);
        WRITEMSG
        strcpy(g_pub_tx.reply, "N001");
        return(-1);
    }

    strncpy(cHead, fsNoteMst.beg_note_no, sNoteParm.head_len);
    cHead[sNoteParm.head_len]='\0';

    qshm = atoi(fsNoteMst.beg_note_no + sNoteParm.head_len);
    zzhm = atoi(fsNoteMst.end_note_no + sNoteParm.head_len);

    qshm = qshm%sNoteParm.unit_cnt;
    zzhm = zzhm%sNoteParm.unit_cnt;

    if(zzhm == 0) zzhm=sNoteParm.unit_cnt;

    /**** 检查该段凭证是否存在 如果为入库不应存在, 其他应存在 ****/
    /**** 如果客户凭证存在, 则必须全部是销号状态(*), 才允许入库 ***/
    if (fpNoteAct[0] == '1')  /* 入库 */
    {
        /* 检查起始号码存在 或者终止号码存在, 或者起始号码终止号码都存在 */
        /* 检查柜员凭证台帐是否有重复的凭证号码 */
        /* 如果状态为销毁状态3, 则可以入库, 否则都不可以入库 */
        /* 如果状态为作废状态6, 则可以入库, 否则都不可以入库 */
        num = sql_count("note_mst", "note_type = '%s' \
                 and ((beg_note_no <= '%s' and end_note_no >= '%s') \
                 or (beg_note_no <= '%s' and end_note_no >= '%s') \
                 or (beg_note_no >= '%s' and end_note_no <= '%s')) \
                 and sts != '3' and sts != '6'", \
                 fsNoteMst.note_type, \
                 fsNoteMst.beg_note_no, fsNoteMst.beg_note_no, \
                 fsNoteMst.end_note_no, fsNoteMst.end_note_no, \
                 fsNoteMst.beg_note_no, fsNoteMst.end_note_no);

        if (num > 0)
        {
            sprintf(acErrMsg, "柜员凭证台帐中有重复的凭证号码");
            WRITEMSG
            strcpy(g_pub_tx.reply, "N006");
            return(-1);
        }

        /* 检查客户凭证台帐是否有重复的凭证号码 */
        num = sql_count("note_cheq_mst", "note_type = '%s' \
                  and ((beg_note_no <= '%s' and end_note_no >= '%s') \
                  or (beg_note_no <= '%s' and end_note_no >= '%s') \
                  or (beg_note_no >= '%s' and end_note_no <= '%s'))", \
                          fsNoteMst.note_type, \
                          fsNoteMst.beg_note_no, fsNoteMst.beg_note_no, \
                          fsNoteMst.end_note_no, fsNoteMst.end_note_no, \
                          fsNoteMst.beg_note_no, fsNoteMst.end_note_no);

        if (num > 0) /* 如果存在 */
        {
            ret = Note_cheq_mst_Dec_Sel(g_pub_tx.reply, "note_type = '%s' \
                         and substr(beg_note_no, 1, %d)='%s' and end_note_no >= '%s' \
                         order by beg_note_no", fsNoteMst.note_type, \
                         sNoteParm.head_len, cHead, fsNoteMst.beg_note_no);
            if(ret)
            {
                sprintf(acErrMsg, "执行Note_cheq_mst_Dec_Sel错!ret=[%d]", ret);
                WRITEMSG
                strcpy(g_pub_tx.reply, "D101");
                return(-1);
            }
            while(1)
            {
                ret = Note_cheq_mst_Fet_Sel(&sCheqMst, g_pub_tx.reply);
                if(ret == 100)
                {
                    break;
                }else if(ret)
                {
                    sprintf(acErrMsg, "执行Note_cheq_mst_Fet_Sel错!ret=[%d]", ret);
                    WRITEMSG;
                    strcpy(g_pub_tx.reply, "D101");
                    return(-1);
                }
                if(strcmp(sCheqMst.beg_note_no , fsNoteMst.end_note_no) > 0) break;

                from = atoi(sCheqMst.beg_note_no + sNoteParm.head_len);
                  to = atoi(sCheqMst.end_note_no + sNoteParm.head_len);
                from = from%sNoteParm.unit_cnt;
                  to = to%sNoteParm.unit_cnt;
                if(from == 0) from = sNoteParm.unit_cnt;
                if(to == 0) to = sNoteParm.unit_cnt;

                (strcmp(sCheqMst.beg_note_no, fsNoteMst.beg_note_no)<=0) ? (from=qshm-1):(from=from-1);
                (strcmp(sCheqMst.end_note_no, fsNoteMst.end_note_no)<=0) ? (to = to-1):(to = zzhm-1);

                if(from < 0) from = 0;

                for(i=from;i<=to;i++)
                {
                    if(sCheqMst.book_sts[i] !='0' && sCheqMst.book_sts[i] !='5')
                    {
                        sprintf(acErrMsg, "有尚未使用的支票[%s][%s][%s][%d]", \
                            sCheqMst.beg_note_no, sCheqMst.end_note_no, sCheqMst.book_sts, i);
                        WRITEMSG
                        strcpy(g_pub_tx.reply, "N037");
                        Note_cheq_mst_Clo_Sel();
                        return(-1);
                    }
                } /* end for */

                if(strcmp(sCheqMst.end_note_no , fsNoteMst.end_note_no) >= 0)
                {
                    break;
                }
            } /* end while */
            Note_cheq_mst_Clo_Sel();
        }
    }else  /* 非入库情况 */
    {
        /* 根据凭证行为找凭证执行所必须的状态和执行后的状态 */
        strcpy(cWho, "T");

        ret = pub_com_NoteActSts(cWho, fpNoteAct, cNoteSts, cPerNoteSts);
        if (ret)
        {
            sprintf(acErrMsg, "根据凭证行为取凭证状态错!");
            WRITEMSG
            strcpy(g_pub_tx.reply, "N002");
            return(-1);
        }

        num1 = sql_count("note_mst", "tel = '%s' and note_type = '%s' \
            and beg_note_no <= '%s' and end_note_no >= '%s' \
            and sts = '%s'", fsNoteMst.tel, fsNoteMst.note_type, \
            fsNoteMst.beg_note_no, fsNoteMst.end_note_no, cPerNoteSts);

        ret = sql_count("note_mst", "tel = '%s' and note_type = '%s' \
            and beg_note_no <= '%s' and end_note_no >= '%s'", \
            fsNoteMst.tel, fsNoteMst.note_type, \
            fsNoteMst.beg_note_no, fsNoteMst.end_note_no);

       if (num1 == 0 && ret > 0)
       {
            if (ret == 0)
            {
                sprintf(acErrMsg, "该批凭证不存在[%s][%s][%s][%s]", \
                    fsNoteMst.note_type, fsNoteMst.beg_note_no, \
                    fsNoteMst.end_note_no, fsNoteMst.tel);
                WRITEMSG
                strcpy(g_pub_tx.reply, "N040");
                return (-1);
            }else
            {
                if(fsNoteMst.sts[0] == '1')
                {
                    sprintf(acErrMsg, "该批凭证已挂失[%s][%s][%s][%s]", \
                        fsNoteMst.note_type, fsNoteMst.beg_note_no, \
                        fsNoteMst.end_note_no, fsNoteMst.tel);
                    WRITEMSG
                    strcpy(g_pub_tx.reply, "N022");
                    return (-1);
                }else if(fsNoteMst.sts[0] == '2')
                {
                    sprintf(acErrMsg, "该批凭证为调出待收(正常)状态[%s][%s][%s][%s]", \
                        fsNoteMst.note_type, fsNoteMst.beg_note_no, \
                        fsNoteMst.end_note_no, fsNoteMst.tel);
                    WRITEMSG
                    strcpy(g_pub_tx.reply, "N022");
                    return (-1);
                }else if(fsNoteMst.sts[0] == '3')
                {
                    sprintf(acErrMsg, "该批凭证为调出待收(作废)状态[%s][%s][%s][%s]", \
                        fsNoteMst.note_type, fsNoteMst.beg_note_no, \
                        fsNoteMst.end_note_no, fsNoteMst.tel);
                    WRITEMSG
                    strcpy(g_pub_tx.reply, "N022");
                    return (-1);
                }else if(fsNoteMst.sts[0] == '4')
                {
                    sprintf(acErrMsg, "该批凭证为柜员使用状态[%s][%s][%s][%s]", \
                        fsNoteMst.note_type, fsNoteMst.beg_note_no, \
                        fsNoteMst.end_note_no, fsNoteMst.tel);
                    WRITEMSG
                    strcpy(g_pub_tx.reply, "N022");
                    return (-1);
                }else if(fsNoteMst.sts[0] == '5')
                {
                    sprintf(acErrMsg, "该批凭证已出售[%s][%s][%s][%s]", \
                        fsNoteMst.note_type, fsNoteMst.beg_note_no, \
                        fsNoteMst.end_note_no, fsNoteMst.tel);
                    WRITEMSG
                    strcpy(g_pub_tx.reply, "N022");
                    return (-1);
                }else if(fsNoteMst.sts[0] == '6')
                {
                    sprintf(acErrMsg, "该批凭证已作废或出售回收[%s][%s][%s][%s]", \
                        fsNoteMst.note_type, fsNoteMst.beg_note_no, \
                        fsNoteMst.end_note_no, fsNoteMst.tel);
                    WRITEMSG
                    strcpy(g_pub_tx.reply, "N022");
                    return (-1);
                }else if(fsNoteMst.sts[0] == '*')
                {
                    sprintf(acErrMsg, "该批凭证已销毁[%s][%s][%s][%s]", \
                        fsNoteMst.note_type, fsNoteMst.beg_note_no, \
                        fsNoteMst.end_note_no, fsNoteMst.tel);
                    WRITEMSG
                    strcpy(g_pub_tx.reply, "N022");
                    return (-1);
                }
            }
        }
    } /* end if */

    if (num<0 || num1<0)
    {
        sprintf(acErrMsg, "查询 note_mst 错误![%s] [%s] [%s] [%s]", \
                fsNoteMst.note_type, fsNoteMst.beg_note_no, \
                fsNoteMst.end_note_no, fsNoteMst.tel);
        WRITEMSG
        strcpy(g_pub_tx.reply, "N038");
        return(-1);
    }

    return(0);
}

/**********************************************************************
* 函 数 名:  pub_com_CalNoteCnt
* 函数功能:  计算凭证数量。根据凭证起始与终止号码计算数量
* 作者/时间: 2003年12月22日
*
* 参    数:
*     输入:
*            char note_type[4]  凭证种类
*            char beg_no[17]    起始号码
*            char end_no[17]    终止号码
*
*     输出:
*            char g_pub_tx.reply[]   响应码
*
*   返回值:  >0: 凭证数量 <=0: 不成功
*
* 修改历史:   包括修改人、时间、修改内容, 以时间倒序方式
*
********************************************************************/

int pub_com_CalNoteCnt(
        char *note_type,
        char *beg_note_no,
        char *end_note_no)
{
    struct note_parm_c    note_parm;    /* 凭证参数 */
    char   cBegHead[17], cEndHead[17];  /* 冠子头 */
    long   lBegNo, lEndNo;
    int    iNoteCnt;                    /* 凭证数量 */
    int    ret;

    MEMSET_DEBUG(&note_parm, 0x00, sizeof(struct note_parm_c));
    MEMSET_DEBUG(cBegHead,   0x00, sizeof(cBegHead));
    MEMSET_DEBUG(cEndHead,   0x00, sizeof(cEndHead));

    /* 根据凭证类型取凭证结构 */
    ret = Note_parm_Sel(g_pub_tx.reply, &note_parm , "note_type = '%s'" , note_type);
    if(ret == 100)
    {
        sprintf(acErrMsg, "取凭证参数note_parm表错!note_type=[%s]", note_type);
        WRITEMSG
        strcpy(g_pub_tx.reply, "N001");
        return (-1);
    }
    else if (ret)
    {
        sprintf(acErrMsg, "执行Note_parm_Sel错误!ret=[%d]", ret);
        WRITEMSG
        strcpy(g_pub_tx.reply, "N001");
        return(-1);
    }

    /* 取冠子头 */
    strncpy(cBegHead, beg_note_no, note_parm.head_len);
    strncpy(cEndHead, end_note_no, note_parm.head_len);
    cBegHead[note_parm.head_len] = '\0';
    cEndHead[note_parm.head_len] = '\0';
    /* 比较冠子头 */
    if (strcmp(cBegHead, cEndHead))
    {
        sprintf(acErrMsg, "两个凭证号码的冠子头不同");
        WRITEMSG
        strcpy(g_pub_tx.reply, "N027");
        return(-1);
    }

    /* 去掉冠子头后起始号码、终止号码 */
    lBegNo = atol(beg_note_no + note_parm.head_len);
    lEndNo = atol(end_note_no + note_parm.head_len);
    /* 比较号码大小 */
    if (lBegNo > lEndNo)
    {
        sprintf(acErrMsg, "起始号码大于终止号码");
        WRITEMSG
        strcpy(g_pub_tx.reply, "N028");
        return(-1);
    }

    iNoteCnt = lEndNo - lBegNo + 1;

    return iNoteCnt;
}

/**********************************************************************
* 函 数 名:   pub_com_NoteActSts
* 函数功能:   根据凭证行为返回对应凭证状态
*     柜员凭证行为(前状态/本次状态)
*      1入库(无/0)  2下发(付出)(0/2)  3领入(收入)(2/0)  4凭证转移转出(0/2)
*      5凭证转移转入(2/0)  6出售(0/5)  7正常上缴(0/2)  8、作废上缴(6/3)
*      9正常收缴(2/0)  A作废收缴(3/6)  B挂失(0/1)  C解挂(1/0)
*      D使用(0/4)  E作废(0/6)  F出售收回(5/6)  M 销毁(6/ *) N撤消 (4/6)
*
*      客户凭证行为
*      K购买(0/1) B挂失(1/2) C解挂(2/1) G客户使用(1/5) H注销(1/4)
*      F出售收回(1/3) N撤消(5/1)
*
*          柜员凭证状态
*      0正常  1挂失  2调出待收(正常)3调出待收(作废)  *销毁
*      5出售  4柜员使用  6作废/出售回收
*
*      客户凭证状态
*      0无  1出售未使用  2挂失  3回收  4作废  5使用
*
* 作者/时间:  2003年12月16日
*
* 参    数:
*     输入:
*            char *who             使用者
*        char *note_act        凭证行为
*
*     输出:
*        char *note_sts        凭证状态
*        char *p_note_sts      凭证使用前状态
*            char *g_pub_tx.reply      响应码
*
*   返回值:   0: 成功 1: 不成功
*
* 修改历史:   包括修改人、时间、修改内容, 以时间倒序方式
*
********************************************************************/
int pub_com_NoteActSts(
    char *who,
    char *note_act,
    char *note_sts,
    char *p_note_sts)
{
    if (who[0] == 'T') /* 柜员行为 */
    {
        switch(note_act[0])
        {
            case '1':   /*1入库(无/0)*/
                p_note_sts[0] = '\0';
                note_sts[0] = '0';
                break;
            case '2':   /*2下发(0/2)*/
                p_note_sts[0] = '0';
                note_sts[0] = '2';
                break;
            case '3':   /*3领入(2/0)*/
                p_note_sts[0] = '2';
                note_sts[0] = '0';
                break;
            case '4':   /*4凭证转移转出(0/2)*/
                p_note_sts[0] = '0';
                note_sts[0] = '2';
                break;
            case '5':   /*5凭证转移转入(2/0)*/
                p_note_sts[0] = '2';
                note_sts[0] = '0';
                break;
            case '6':   /*6出售(0/5)*/
                p_note_sts[0] = '0';
                note_sts[0] = '5';
                break;
            case '7':   /*7正常上缴(0/2)*/
                p_note_sts[0] = '0';
                note_sts[0] = '2';
                break;
            case '8':   /*  8、作废上缴(6/3)  */
                p_note_sts[0] = '6';
                note_sts[0] = '3';
                break;
            case '9':   /*9正常收缴(2/0)*/
                p_note_sts[0] = '2';
                note_sts[0] = '0';
                break;
            case 'A':   /*  A作废收缴(3/6)*/
                p_note_sts[0] = '3';
                note_sts[0] = '6';
                break;
            case 'B':   /*  B挂失(0/1) */
                p_note_sts[0] = '0';
                note_sts[0] = '1';
                break;
            case 'C':   /* C解挂(1/0)  */
                p_note_sts[0] = '1';
                note_sts[0] = '0';
                break;
            case 'D':   /*D使用(0/4)*/
                p_note_sts[0] = '0';
                note_sts[0] = '4';
                break;
            case 'e':   /*  E作废(4/6)使用 */
                p_note_sts[0] = '4';
                note_sts[0] = '6';
                break;
            case 'E':   /*  E作废(0/6)*/
                p_note_sts[0] = '0';
                note_sts[0] = '6';
                break;
            case 'F':   /*  F出售收回(5/6)*/
                p_note_sts[0] = '5';
                note_sts[0] = '6';
                break;
            case 'M':   /*  M销毁(6/ *)*/
                p_note_sts[0] = '6';
                note_sts[0] = '*';
                break;
            case 'N':   /*  N撤消(使用撤销)*/
                p_note_sts[0] = '4';
                note_sts[0] = '6';
                break;
            default:
                sprintf(acErrMsg, "柜员凭证行为超出处理范围, 凭证行为=[%s]", note_act);
                    WRITEMSG
                    strcpy(g_pub_tx.reply, "N009");
                    return(-1);
        }
    }else if (who[0] == 'C')  /* 客户凭证行为 */
    {
        switch(note_act[0])
        {
            case 'K':   /* K购买(0/1) */
                p_note_sts[0] = '0';
                note_sts[0] = '1';
                break;
            case 'B':   /*B挂失(1/2)*/
                p_note_sts[0] = '1';
                note_sts[0] = '2';
                break;
            case 'C':   /*C解挂(2/1)*/
                p_note_sts[0] = '2';
                note_sts[0] = '1';
                break;
            case 'G':   /*G客户使用(1/5)*/
                p_note_sts[0] = '1';
                note_sts[0] = '5';
                break;
            case 'H':   /*H注销(1/4)*/
                p_note_sts[0] = '1';
                note_sts[0] = '4';
                break;
            case 'F':   /*  F出售收回(1/3)*/
                p_note_sts[0] = '1';
                note_sts[0] = '3';
                break;
            case 'N':   /*  N撤消(使用撤销)*/
                p_note_sts[0] = '5';
                note_sts[0] = '1';
                break;
            default:
                sprintf(acErrMsg, "客户凭证行为超出处理范围, 凭证行为=[%s]", note_act);
                    WRITEMSG
                    strcpy(g_pub_tx.reply, "N009");
                    return(-1);
        }

    }else  /* 凭证行为者参数错误 */
    {
        sprintf(acErrMsg, "凭证行为者参数错误, who =[%s]", who);
            WRITEMSG
            return(-1);
    }

    p_note_sts[1] = '\0';
    note_sts[1] = '\0';

        return(0);
}

/**********************************************************************
 * 函 数 名:   pub_com_NoteApart
 * 函数功能:   凭证分段处理。把原来属于一个区间段的凭证按照传入的起始
 *         号码和终止号码分开, 该区间的起始号码到传入起始号码前是
 *         一段传入起始号码到终止号码是一段。传入终止号码后到该区
 *         间终止号码为一段。如果是起始号码相同、终止号码相同或都
 *         同可能分两段或一段。
 *             分段后可能存在并段情况(即与状态相同的号码相连), 则需要进行并段处理
 *
 * 作者/时间:  jack/2003年12月16日
 *
 * 参数:
 *     输入:
 *            char   *note_act             凭证行为
 *            struct note_mst_c note_mst   柜员凭证台帐
 *
 *     输出:   char g_pub_tx.reply 相应码
 *
 *   返回值:   0: 并段成功 -1: 不成功
 *
 * 修改历史:   包括修改人、时间、修改内容, 以时间倒序方式
 *
********************************************************************/
int pub_com_NoteApart(
         char *note_act,
         struct note_mst_c note_mst)
{
    struct note_mst_c sNoteMst;  /* 从柜员凭证台帐取出的值 */
    struct note_mst_c sNoteMstTmp; /* 存放柜员凭证台帐的临时变量 */
    char   cNoteSts[2];          /* 凭证状态 */
    char   cPerNoteSts[2];  /* 行为执行的状态 */
    char   cWho[2];     /* 凭证行为执行者 */
    char   str_tmp[32];
    int    ret;

    MEMSET_DEBUG(&sNoteMst, 0x00, sizeof(struct note_mst_c));
    MEMSET_DEBUG(cNoteSts, 0x00, sizeof(cNoteSts));
    MEMSET_DEBUG(cPerNoteSts, 0x00, sizeof(cPerNoteSts));
    MEMSET_DEBUG(cWho, 0x00, sizeof(cWho));

    strcpy(cWho, "T");   /* T表示柜员 */

    /* 根据凭证行为找出凭证状态 */
    ret = pub_com_NoteActSts(cWho, note_act, cNoteSts, cPerNoteSts);
    if (ret)
    {
        sprintf(acErrMsg, "根据凭证行为取凭证状态错!");
        WRITEMSG
        strcpy(g_pub_tx.reply, "N002");
        return(-1);
    }

    strcpy(note_mst.sts, cNoteSts);   /* 凭证行为执行后凭证的状态 */

    /* 检查是否存在这样的区间 */
    vtcp_log("tel = '%s' and note_type = '%s' and beg_note_no <= '%s' and end_note_no >= '%s' and sts = '%s'",
        note_mst.tel, note_mst.note_type, note_mst.beg_note_no, note_mst.end_note_no, cPerNoteSts);
    /* 带原凭证状态的查找 */
    ret = Note_mst_Dec_Upd(g_pub_tx.reply, "tel = '%s' and note_type = '%s' \
                      and beg_note_no <= '%s' and end_note_no >= '%s' \
                      and sts = '%s'", note_mst.tel, note_mst.note_type, \
                      note_mst.beg_note_no, note_mst.end_note_no, cPerNoteSts);
    if (ret)
    {
        sprintf(acErrMsg, "执行Note_mst_Dec_Upd错!ret=[%d]", ret);
        WRITEMSG
        return(-1);
    }

    ret = Note_mst_Fet_Upd(&sNoteMst, g_pub_tx.reply);

    if (ret == 100)
    {
        sprintf(acErrMsg, "不存在这样的凭证区间!");
        WRITEMSG
        strcpy(g_pub_tx.reply, "N003");
        return(-1);
    }else if (ret)
    {
        sprintf(acErrMsg, "执行Note_mst_Fet_Upd出错!ret=[%d]", ret);
        WRITEMSG
        return(-1);
    }else                 /* 0 找到满足条件的记录 */
    {
        /* 检查头是否相同 */
        if(!strcmp(sNoteMst.beg_note_no, note_mst.beg_note_no)) /* 头相同 */
        {
            if(!strcmp(sNoteMst.end_note_no, note_mst.end_note_no)) /* 尾相同 */
            {
                /* 修改凭证状态 */
                strcpy(sNoteMst.sts , note_mst.sts);

                ret = Note_mst_Upd_Upd(sNoteMst, g_pub_tx.reply);
                if (ret == -239)
                {
                        sprintf(acErrMsg, "修改柜员凭证台帐note_mst错, 存在重复记录!");
                    WRITEMSG
                    strcpy(g_pub_tx.reply, "N019");
                    return(-1);
                }else if(ret)
                {
                    sprintf(acErrMsg, "执行Note_mst_Upd_Upd错!ret=[%d]", ret);
                    WRITEMSG
                    return(-1);
                }
                /**********************************************
                  做并段处理:
                           检查是否存在与修改后头尾相连的号码段
                           需要检查头尾是否同时相同的, 并sNoteMst或者note_mst
                **********************************************/
            ret = pub_com_NoteApartMerg(note_mst);
            if (ret == -1)
            {
                sprintf(acErrMsg, "凭证并段失败");
                WRITEMSG
                return(-1);
            }
            }else  /* 尾不同 */
            {
                /* 1: 插入一条新纪录*/
sprintf(acErrMsg, "插入一条新纪录 beg_no=[%s], end_no=[%s]", note_mst.beg_note_no, note_mst.end_note_no);
                WRITEMSG
                ret = Note_mst_Ins(note_mst, g_pub_tx.reply);
            if (ret == -239)
            {
                sprintf(acErrMsg, "向柜员凭证台帐note_mst插入记录错!有重复记录!");
                WRITEMSG
                strcpy(g_pub_tx.reply, "N020");
                return(-1);
            }else if(ret)
            {
                sprintf(acErrMsg, "执行Note_mst_Ins错!ret=[%d]", ret);
                WRITEMSG
                return(-1);
            }
                /*********************************************
                  做并段处理:
                           检查是否存在与修改后头尾相连的号码段
                           需要检查头是否同时相同的, 并note_mst
                **********************************************/
            ret = pub_com_NoteApartMerg(note_mst);
            if (ret == -1)
            {
                sprintf(acErrMsg, "凭证并段失败");
                WRITEMSG
                return(-1);
            }

                /* 2: 修改原记录的起始号码、状态、数量 */
                ret = pub_com_CalNoteNo(note_mst.note_type, \
                          note_mst.end_note_no, str_tmp, 1);
                if (ret == -1)
                {
                    sprintf(acErrMsg, "计算凭证号码错!");
                    WRITEMSG
                    strcpy(g_pub_tx.reply, "N004");
                    return(-1);
                }

                strcpy(sNoteMst.beg_note_no, str_tmp);

                /* 计算凭证数量 */
                sNoteMst.cnt -=  note_mst.cnt;

                /* 修改原凭证 */
                ret = Note_mst_Upd_Upd(sNoteMst, g_pub_tx.reply);
                if (ret == -239)
                {
                        sprintf(acErrMsg, "修改柜员凭证台帐note_mst错, 有重复记录!");
                    WRITEMSG
                    strcpy(g_pub_tx.reply, "N019");
                    return(-1);
                }else if(ret)
                {
                    sprintf(acErrMsg, "执行Note_mst_Upd_Upd错!ret=[%d]", ret);
                    WRITEMSG
                    return(-1);
                }
            }
        }else /* 头不同 */
        {
            if(!strcmp(sNoteMst.end_note_no, note_mst.end_note_no)) /* 尾相同 */
            {
                /* 1: 插入新的数据 */
                ret = Note_mst_Ins(note_mst, g_pub_tx.reply);
            if (ret == -239)
            {
                sprintf(acErrMsg, "向柜员凭证台帐note_mst插入记录错!有重复记录!");
                WRITEMSG
                strcpy(g_pub_tx.reply, "N020");
                return(-1);
            }else if(ret)
            {
                sprintf(acErrMsg, "执行Note_mst_Ins错!ret=[%d]", ret);
                WRITEMSG
                return(-1);
            }
                /**********************************************
                  做并段处理:
                           检查是否存在与修改后头尾相连的号码段
                           需要检查尾是否同时相同的, 并note_mst
                **********************************************/
            ret = pub_com_NoteApartMerg(note_mst);
            if (ret == -1)
            {
                sprintf(acErrMsg, "凭证并段失败");
                WRITEMSG
                return(-1);
            }

                /* 2: 修改原数据 */
                /* 计算凭证终止号码 */
                ret = pub_com_CalNoteNo(note_mst.note_type, \
                          note_mst.beg_note_no, str_tmp, -1);
                if (ret == -1)
                {
                        sprintf(acErrMsg, "计算凭证号码错!");
                    WRITEMSG
                    strcpy(g_pub_tx.reply, "N004");
                    return(-1);
                }
                strcpy(sNoteMst.end_note_no, str_tmp);

                /* 计算凭证数量 */
                sNoteMst.cnt = sNoteMst.cnt - note_mst.cnt;

                ret = Note_mst_Upd_Upd(sNoteMst, g_pub_tx.reply);
                if (ret == -239)
                {
                    sprintf(acErrMsg, "修改柜员凭证台帐note_mst错, 有重复记录!");
                    WRITEMSG
                    strcpy(g_pub_tx.reply, "N019");
                    return(-1);
                }else if(ret)
                {
                    sprintf(acErrMsg, "执行Note_mst_Upd_Upd错!ret=[%d]", ret);
                    WRITEMSG
                    return(-1);
                }
            }else   /* 尾不同 */
            {
                sNoteMstTmp = sNoteMst;

                /* 修改原来的凭证, 分成三段 */
                /* 1: 计算凭证终止号码 */
                ret = pub_com_CalNoteNo(sNoteMst.note_type, note_mst.beg_note_no, \
                                      sNoteMst.end_note_no, -1);
                if (ret == -1)
                {
                        sprintf(acErrMsg, "计算凭证号码错!");
                    WRITEMSG
                    strcpy(g_pub_tx.reply, "N004");
                    return(-1);
                }

                ret = pub_com_CalNoteCnt(note_mst.note_type, \
                        sNoteMst.beg_note_no, sNoteMst.end_note_no);
                if (ret == -1)
                {
                        sprintf(acErrMsg, "计算凭证数量错!");
                    WRITEMSG
                    strcpy(g_pub_tx.reply, "N004");
                    return(-1);
                }
                sNoteMst.cnt = ret;

                /* 修改柜员凭证 */
                ret = Note_mst_Upd_Upd(sNoteMst, g_pub_tx.reply);
                if (ret == -239)
                {
                        sprintf(acErrMsg, "修改柜员凭证台帐note_mst错!");
                    WRITEMSG
                    strcpy(g_pub_tx.reply, "N019");
                    return(-1);
                }else if(ret)
                {
                    sprintf(acErrMsg, "执行Note_mst_Upd_Upd错!ret=[%d]", ret);
                    WRITEMSG
                    return(-1);
                }

                /* 2: 插入新的 */
                /* 插入一条新纪录*/
                ret = Note_mst_Ins(note_mst, g_pub_tx.reply);
            if (ret == -239)
            {
                sprintf(acErrMsg, "向柜员凭证台帐note_mst插入记录错!有重复记录!");
                WRITEMSG
                strcpy(g_pub_tx.reply, "N020");
                return(-1);
            }else if(ret)
            {
                sprintf(acErrMsg, "执行Note_mst_Ins错!ret=[%d]", ret);
                WRITEMSG
                return(-1);
            }

                /* 3: 插入另一个 */
                /* 计算凭证起始号码 */
                ret = pub_com_CalNoteNo(sNoteMstTmp.note_type, \
                          note_mst.end_note_no, sNoteMstTmp.beg_note_no, 1);
                if (ret == -1)
                {
                        sprintf(acErrMsg, "计算凭证号码错!");
                    WRITEMSG
                    strcpy(g_pub_tx.reply, "N004");
                    return(-1);
                }
                /* 计算凭证数量 */
                ret = pub_com_CalNoteCnt(sNoteMstTmp.note_type, \
                        sNoteMstTmp.beg_note_no, sNoteMstTmp.end_note_no);
                if (ret == -1)
                {
                        sprintf(acErrMsg, "计算凭证号码错!");
                    WRITEMSG
                    strcpy(g_pub_tx.reply, "N004");
                    return(-1);
                }
                sNoteMstTmp.cnt = ret;
                /* 插入一条新纪录*/
                ret = Note_mst_Ins(sNoteMstTmp, g_pub_tx.reply);
            if (ret == -239)
            {
                sprintf(acErrMsg, "向柜员凭证台帐note_mst插入记录错!有重复记录!");
                WRITEMSG
                strcpy(g_pub_tx.reply, "N020");
                return(-1);
            }else if(ret)
            {
                sprintf(acErrMsg, "执行Note_mst_Ins错!ret=[%d]", ret);
                WRITEMSG
                return(-1);
            }
            }
        }
    }

    /* 关闭游标 */
    Note_mst_Clo_Upd();

    return(0);
}

/**********************************************************************
 * 函 数 名:   pub_com_NoteApartMerg
 * 函数功能:   凭证并段:分段过程中用到的并段函数
 * 作者/时间:  Terry.cui/2003年12月30日
 *
 * 参    数:
 *     输入:
 *          struct note_mst_c note_mst;
 *
 *     输出:
 *            char *g_pub_tx.reply               响应码
 *
 *   返回值:  0:   成功
 *            非0: 不成功
 *
 * 修改历史:   包括修改人、时间、修改内容, 以时间倒序方式
 *
********************************************************************/
int pub_com_NoteApartMerg(note_mst)
    struct note_mst_c note_mst;
{
    struct note_mst_c  note_mst1, note_mst2;        /* 柜员凭证台帐结构 */
    char   beg_no[17];                     /* 起始凭证号 */
    char   end_no[17];                             /* 终止凭证号 */
    char   cFilter[512];                           /* 删除表的where条件 */
    int    cnt;                                    /* 凭证数量   */
    int    ret;

    MEMSET_DEBUG(&note_mst1, 0x00, sizeof(struct note_mst_c));
    MEMSET_DEBUG(&note_mst2, 0x00, sizeof(struct note_mst_c));
    MEMSET_DEBUG(beg_no, 0x00, sizeof(beg_no));
    MEMSET_DEBUG(end_no, 0x00, sizeof(end_no));
    MEMSET_DEBUG(cFilter, 0x00, sizeof(cFilter));

    /* 检查参数note_mst中是否已经存在起始号码或终止号码重叠情况 */
sprintf(acErrMsg, "note_mst.beg_note_no[%s], note_mst.end_note_no[%s], sts=[%s]", \
         note_mst.beg_note_no, note_mst.end_note_no, note_mst.sts);
WRITEMSG

    ret = sql_count("note_mst", "tel = '%s' and note_type = '%s' \
                     and ((beg_note_no <= '%s' and end_note_no >= '%s') \
                     or (beg_note_no <= '%s' and end_note_no >= '%s')) \
                     and sts = '%s'", note_mst.tel, note_mst.note_type, \
                     note_mst.beg_note_no, note_mst.beg_note_no, \
                     note_mst.end_note_no, note_mst.end_note_no, note_mst.sts);
    if (ret < 0)
    {
        sprintf(acErrMsg, "检查凭证是否已经存在错");
        WRITEMSG
        strcpy(g_pub_tx.reply, "N035");
        return(-1);
    }else if (ret > 1)
    {
        sprintf(acErrMsg, "部分凭证已经存在");
        WRITEMSG
        strcpy(g_pub_tx.reply, "N034");
        return(-1);
    }

    /* 计算凭证起始号码 */
    ret = pub_com_CalNoteNo(note_mst.note_type, note_mst.beg_note_no, \
                              beg_no, -1);
    if (ret == -1)
    {
        sprintf(acErrMsg, "计算凭证号码错");
        WRITEMSG
        strcpy(g_pub_tx.reply, "N004");
        return(-1);
    }

    /* 计算凭证终止号码 */
    ret = pub_com_CalNoteNo(note_mst.note_type, note_mst.end_note_no, \
                              end_no, 1);
    if (ret == -1)
    {
        sprintf(acErrMsg, "计算凭证号码错");
        WRITEMSG
        strcpy(g_pub_tx.reply, "N004");
        return(-1);
    }

    /* 根据输入参数检查是否存在终止号码和输入参数起始号码连续的 */
    ret = Note_mst_Sel(g_pub_tx.reply, &note_mst1, "tel='%s' and sts='%s' \
                      and note_type='%s' and end_note_no='%s'", note_mst.tel, \
                      note_mst.sts, note_mst.note_type, beg_no);
    if (ret == 100)  /* 1:不存在凭证终止号码与输入起始号码连续的凭证 */
    {
        /* 根据输入参数检查是否存在起始号码和输入参数终止号码连续的 */
        ret=Note_mst_Sel(g_pub_tx.reply, &note_mst2, "tel = '%s' and \
                     sts = '%s' and note_type='%s' and beg_note_no='%s'", \
                     note_mst.tel, note_mst.sts, note_mst.note_type, end_no);
        /* 3333 if */
        if (ret==100)  /* 不存在凭证起始号码与输入终止号码连续的 */
        {
        return(0);

         }else if (ret)
         {
            sprintf(acErrMsg, "执行Note_mst_Sel错误!ret=[%d]", ret);
            WRITEMSG
            return(-1);
         }
         else  /* 第二种情况: 存在后半段相连, 存在凭证起始号码与输入终止号码连续的 */
         {
             note_mst2.cnt += note_mst.cnt;

             /* 并段处理, 将起始号码改为输入的起始号码 */
             ret = sql_execute("update note_mst set beg_note_no = '%s', \
                                cnt = %d where tel = '%s' and sts = '%s' \
                                and note_type = '%s' and beg_note_no = '%s'", \
                                note_mst.beg_note_no, note_mst2.cnt, \
                                note_mst.tel, note_mst.sts, note_mst.note_type, \
                                end_no);
             if (ret)
             {
                 sprintf(acErrMsg, "修改柜员凭证台帐note_mst错!");
                 WRITEMSG
                 strcpy(g_pub_tx.reply, "N019");
                 return(-1);
             }
         } /* end 3333 if */
    }else if (ret)
    {
        sprintf(acErrMsg, "执行Note_mst_Sel错误!ret=[%d]", ret);
        WRITEMSG
        return(-1);
    }else  /* 存在凭证终止号码与输入起始号码连续的凭证 */
    {
        /* 根据输入参数检查是否存在起始号码和输入参数终止号码连续的 */
        ret=Note_mst_Sel(g_pub_tx.reply, &note_mst2, "tel = '%s' and \
                     sts = '%s' and note_type='%s' and beg_note_no='%s'", \
                     note_mst.tel, note_mst.sts, note_mst.note_type, end_no);
        /* 4444 if */
        if (ret == 100)/*第三种: 前半段相连, 不存在凭证起始号码与输入终止号码连续的*/
        {
            /* 并段处理, 将终止号码改为输入的终止号码 */
                sprintf(acErrMsg, "第三种: 前半段相连, note_mst.end_note_no=[%s]!", note_mst.end_note_no);
                WRITEMSG
            strcpy(note_mst1.end_note_no, note_mst.end_note_no);
            note_mst1.cnt += note_mst.cnt;
      sprintf(acErrMsg, "第三种: 前半段相连, note_mst1.end_note_no=[%s]!", note_mst1.end_note_no);
                WRITEMSG
            ret = sql_execute("update note_mst set end_note_no = '%s', \
                               cnt = %d where tel = '%s' and sts = '%s' \
                               and note_type = '%s' and beg_note_no = '%s'", \
                               note_mst1.end_note_no, note_mst1.cnt, \
                               note_mst1.tel, note_mst1.sts, note_mst1.note_type, \
                               note_mst1.beg_note_no);
            if (ret)
            {
                sprintf(acErrMsg, "修改柜员凭证台帐note_mst错!");
                WRITEMSG
                strcpy(g_pub_tx.reply, "N019");
                return(-1);
            }
        }else if (ret)
        {
            sprintf(acErrMsg, "执行Note_mst_Dec_Upd错误!ret=[%d]", ret);
            WRITEMSG
            return(-1);
        }else
        {
            /* 第四中: 前后都相连 */
            /*重新组合凭证段首尾号*/
            strcpy(note_mst1.end_note_no, note_mst2.end_note_no);
            note_mst1.cnt = note_mst1.cnt + note_mst2.cnt + note_mst.cnt;

            /* 并段处理, 将输入的凭证号与原有凭证首尾相连 */
            ret = sql_execute("update note_mst set end_note_no = '%s', \
                               cnt = %d where tel = '%s' and sts = '%s' \
                               and note_type = '%s' and beg_note_no = '%s'", \
                               note_mst1.end_note_no, note_mst1.cnt, \
                               note_mst1.tel, note_mst1.sts, note_mst1.note_type, \
                               note_mst1.beg_note_no);
            if (ret)
            {
                sprintf(acErrMsg, "修改柜员凭证台帐note_mst错!");
                WRITEMSG
                strcpy(g_pub_tx.reply, "N019");
                return(-1);
            }

            /* 删除后一段记录note_mst2 */
            sprintf(cFilter, "tel = '%s' and note_type = '%s' \
                     and beg_note_no = '%s' and end_note_no = '%s' \
                     and sts = '%s'", note_mst2.tel, note_mst2.note_type, \
                     note_mst2.beg_note_no, note_mst2.end_note_no, note_mst2.sts);
            ret = deldb("note_mst", cFilter);
            if (ret)
            {
                 sprintf(acErrMsg, "删除柜员凭证台帐错");
                 WRITEMSG
                 strcpy(g_pub_tx.reply, "N005");
                 return(-1);
            }
        } /** end 4444 if */
    } /* end 1111 if */

    /* 删除已经写入的数据 */
    sprintf(cFilter, "tel = '%s' and note_type = '%s' \
                     and beg_note_no = '%s' and end_note_no = '%s' \
                     and sts = '%s'", note_mst.tel, note_mst.note_type, \
                     note_mst.beg_note_no, note_mst.end_note_no, note_mst.sts);
    ret = deldb("note_mst", cFilter);
    if (ret)
    {
        sprintf(acErrMsg, "删除柜员凭证台帐错");
        WRITEMSG
        strcpy(g_pub_tx.reply, "N005");
        return(-1);
    }

    return(0);
}

/**********************************************************************
 * 函 数 名:   pub_com_NoteMerg
 * 函数功能:   凭证并段
 * 作者/时间:  jack/2003年12月23日
 *
 * 参    数:
 *     输入:
 *          struct note_mst_c note_mst;
 *
 *     输出:
 *            char *g_pub_tx.reply               响应码
 *
 *   返回值:  0:   成功
 *            非0: 不成功
 *
 * 修改历史:   包括修改人、时间、修改内容, 以时间倒序方式
 *
********************************************************************/

int pub_com_NoteMerg(note_mst)
    struct note_mst_c note_mst;
{
    struct note_mst_c  note_mst1, note_mst2;        /* 柜员凭证台帐结构 */
    char   beg_no[17];                     /* 起始凭证号 */
    char   end_no[17];                             /* 终止凭证号 */
    char   cFilter[512];                           /* 删除表的where条件 */
    int    cnt;                                    /* 凭证数量   */
    int    ret;

    MEMSET_DEBUG(&note_mst1, 0x00, sizeof(struct note_mst_c));
    MEMSET_DEBUG(&note_mst2, 0x00, sizeof(struct note_mst_c));
    MEMSET_DEBUG(beg_no, 0x00, sizeof(beg_no));
    MEMSET_DEBUG(end_no, 0x00, sizeof(end_no));
    MEMSET_DEBUG(cFilter, 0x00, sizeof(cFilter));

    /* 检查参数note_mst中是否已经存在起始号码或终止号码重叠情况 */
sprintf(acErrMsg, "note_mst.beg_note_no[%s], note_mst.end_note_no[%s], sts=[%s]", \
         note_mst.beg_note_no, note_mst.end_note_no, note_mst.sts);
WRITEMSG

    ret = sql_count("note_mst", "tel = '%s' and note_type = '%s' \
                     and ((beg_note_no <= '%s' and end_note_no >= '%s') \
                     or (beg_note_no <= '%s' and end_note_no >= '%s')) \
                     and sts = '%s'", note_mst.tel, note_mst.note_type, \
                     note_mst.beg_note_no, note_mst.beg_note_no, \
                     note_mst.end_note_no, note_mst.end_note_no, note_mst.sts);
    if (ret < 0)
    {
        sprintf(acErrMsg, "检查凭证是否已经存在错");
        WRITEMSG
        strcpy(g_pub_tx.reply, "N035");
        return(-1);
    }else if (ret > 0)
    {
        sprintf(acErrMsg, "部分凭证已经存在");
        WRITEMSG
        strcpy(g_pub_tx.reply, "N034");
        return(-1);
    }

    /* 计算凭证起始号码 */
    ret = pub_com_CalNoteNo(note_mst.note_type, note_mst.beg_note_no, \
                              beg_no, -1);
    if (ret == -1)
    {
        sprintf(acErrMsg, "计算凭证号码错");
        WRITEMSG
        strcpy(g_pub_tx.reply, "N004");
        return(-1);
    }

    /* 计算凭证终止号码 */
    ret = pub_com_CalNoteNo(note_mst.note_type, note_mst.end_note_no, \
                              end_no, 1);
    if (ret == -1)
    {
        sprintf(acErrMsg, "计算凭证号码错");
        WRITEMSG
        strcpy(g_pub_tx.reply, "N004");
        return(-1);
    }

    /* 根据输入参数检查是否存在终止号码和输入参数起始号码连续的 */
    ret = Note_mst_Dec_Upd(g_pub_tx.reply, "tel='%s' and sts='%s' and note_type='%s' \
                      and end_note_no='%s'", note_mst.tel, note_mst.sts, \
                      note_mst.note_type, beg_no);
    if (ret)   /* 声明打开游标错误 */
    {
        sprintf(acErrMsg, "执行Note_mst_Dec_Upd错误!ret=[%d]", ret);
        WRITEMSG
        return(-1);
    }

    ret = Note_mst_Fet_Upd(&note_mst1, g_pub_tx.reply);
    /* 1111 if */
    if (ret == 100)  /* 1:不存在凭证终止号码与输入起始号码连续的凭证 */
    {
        /* 根据输入参数检查是否存在起始号码和输入参数终止号码连续的 */
        ret=Note_mst_Sel(g_pub_tx.reply, &note_mst2, "tel = '%s' and \
                     sts = '%s' and note_type='%s' and beg_note_no='%s'", \
                     note_mst.tel, note_mst.sts, note_mst.note_type, end_no);
        /* 3333 if */
        if (ret==100)  /* 不存在凭证起始号码与输入终止号码连续的 */
        {
            /* 第一种情况: 直接插入新记录, 将输入的凭证直接插入表中 */
            ret=Note_mst_Ins(note_mst, g_pub_tx.reply);
            if (ret == -239)
            {
                sprintf(acErrMsg, "向柜员凭证台帐note_mst插入记录错!有重复记录!");
            WRITEMSG
            strcpy(g_pub_tx.reply, "N020");
            return(-1);
             }else if(ret)
             {
             printf(acErrMsg, "执行Note_mst_Ins错!ret=[%d]", ret);
             WRITEMSG
             return(-1);
             }

         }else if (ret)
         {
            sprintf(acErrMsg, "执行Note_mst_Sel错误!ret=[%d]", ret);
            WRITEMSG
            return(-1);
         }
         else  /* 第二种情况: 存在后半段相连, 存在凭证起始号码与输入终止号码连续的 */
         {
             note_mst2.cnt += note_mst.cnt;

             /* 并段处理, 将起始号码改为输入的起始号码 */
             ret = sql_execute("update note_mst set beg_note_no = '%s', \
                                cnt = %d where tel = '%s' and sts = '%s' \
                                and note_type = '%s' and beg_note_no = '%s'", \
                                note_mst.beg_note_no, note_mst2.cnt, \
                                note_mst.tel, note_mst.sts, note_mst.note_type, \
                                end_no);
             if (ret)
             {
                 sprintf(acErrMsg, "修改柜员凭证台帐note_mst错!");
                 WRITEMSG
                 strcpy(g_pub_tx.reply, "N019");
                 return(-1);
             }
         } /* end 3333 if */
    }else if (ret)
    {
        sprintf(acErrMsg, "执行Note_mst_Fet_Upd错误!ret=[%d]", ret);
        WRITEMSG
        return(-1);
    }else  /* 存在凭证终止号码与输入起始号码连续的凭证 */
    {
        /* 根据输入参数检查是否存在起始号码和输入参数终止号码连续的 */
        ret=Note_mst_Sel(g_pub_tx.reply, &note_mst2, "tel = '%s' and \
                     sts = '%s' and note_type='%s' and beg_note_no='%s'", \
                     note_mst.tel, note_mst.sts, note_mst.note_type, end_no);
        /* 4444 if */
        if (ret == 100)/*第三种: 前半段相连, 不存在凭证起始号码与输入终止号码连续的*/
        {
            /* 并段处理, 将终止号码改为输入的终止号码 */
            strcpy(note_mst1.end_note_no, note_mst.end_note_no);
            note_mst1.cnt += note_mst.cnt;

            ret = Note_mst_Upd_Upd(note_mst1, g_pub_tx.reply);

            if (ret == -239)
            {
                sprintf(acErrMsg, "修改柜员凭证台帐note_mst错!");
                WRITEMSG
                strcpy(g_pub_tx.reply, "N019");
                return(-1);
            }else if(ret)
            {
                sprintf(acErrMsg, "执行Note_mst_Upd_Upd错!ret=[%d]", ret);
                WRITEMSG
                return(-1);
            }
        }else if (ret)
        {
            sprintf(acErrMsg, "执行Note_mst_Dec_Upd错误!ret=[%d]", ret);
            WRITEMSG
            return(-1);
        }else
        {
            /* 第四中: 前后都相连 */
            /*重新组合凭证段首尾号*/
            strcpy(note_mst1.end_note_no, note_mst2.end_note_no);
            note_mst1.cnt = note_mst1.cnt + note_mst2.cnt + note_mst.cnt;

            /* 并段处理, 将输入的凭证号与原有凭证首尾相连 */
            ret=Note_mst_Upd_Upd(note_mst1, g_pub_tx.reply);
            if (ret == -239)
            {
                sprintf(acErrMsg, "修改柜员凭证台帐note_mst错!");
                WRITEMSG
                strcpy(g_pub_tx.reply, "N019");
                return(-1);
            }else if(ret)
            {
                sprintf(acErrMsg, "执行Note_mst_Upd_Upd错!ret=[%d]", ret);
                WRITEMSG
                return(-1);
            }

            /* 删除后一段记录note_mst2 */
            sprintf(cFilter, "tel = '%s' and note_type = '%s' \
                     and beg_note_no = '%s' and end_note_no = '%s' \
                     and sts = '%s'", note_mst2.tel, note_mst2.note_type, \
                     note_mst2.beg_note_no, note_mst2.end_note_no, note_mst2.sts);
            ret = deldb("note_mst", cFilter);
            if (ret)
            {
                 sprintf(acErrMsg, "删除柜员凭证台帐错");
                 WRITEMSG
                 strcpy(g_pub_tx.reply, "N005");
                 return(-1);
            }
        } /** end 4444 if */
    } /* end 1111 if */
    /* 关闭游标 */
    Note_mst_Clo_Upd();

    return(0);
}


/**********************************************************************
 * 函 数 名:   pub_com_NoteDel
 * 函数功能:   凭证分段删除处理。
 *        把原来属于一个区间段的凭证按照传入的起始号码和终止号码分开, 并删除。
 *        该区间的起始号码到传入起始号码前是一段, 传入起始号码到终止号码是一
 *        段, 传入终止号码到该区间终止号码为一段。如果是起始号码相同、终止
 *        号码相同或都同可能分两段或一段。
 *        分成一段: 删除    二段: 修改  三段: 修改、增加、删除
 *
 * 作者/时间:  jack/2005年1月6日
 *
 * 参数:
 *     输入:
 *        char  *note_act       凭证行为
 *            struct note_mst_c note_mst    柜员凭证台帐
 *
 *     输出:   char g_pub_tx.reply      相应码
 *
 *   返回值:   0: 并段成功 -1: 不成功
 *
 * 修改历史:   包括修改人、时间、修改内容, 以时间倒序方式
 *
********************************************************************/
int pub_com_NoteDel(char *note_act, struct note_mst_c note_mst)
{
    struct note_mst_c sNoteMst;  /* 从柜员凭证台帐取出的值 */
    struct note_mst_c sNoteMstTmp; /* 存放柜员凭证台帐的临时变量 */

    char   cPerNoteSts[2];  /* 行为执行的状态 */
    char   cWho[2];     /* 凭证行为执行者 */
    char   str_tmp[32];

    int    ret;

    MEMSET_DEBUG(&sNoteMst, 0x00, sizeof(struct note_mst_c));
    MEMSET_DEBUG(cWho, 0x00, sizeof(cWho));
    MEMSET_DEBUG(cPerNoteSts, 0x00, sizeof(cPerNoteSts));

    strcpy(cWho, "T");   /* T表示柜员 */

    /* 根据凭证行为找出凭证状态 */
    ret = pub_com_NoteActSts(cWho, note_act, note_mst.sts, cPerNoteSts);
    if (ret)
    {
        sprintf(acErrMsg, "根据凭证行为取凭证状态错!");
        WRITEMSG
        strcpy(g_pub_tx.reply, "N002");
        return(-1);
    }

    strcpy(cWho, "T");   /* T表示柜员 */

    /* 检查是否存在这样的区间 */
    /* 带原凭证状态的查找 */
    ret = Note_mst_Dec_Upd(g_pub_tx.reply, "br_no = '%s' and tel = '%s' \
                and note_type = '%s' and beg_note_no <= '%s' \
                and end_note_no >= '%s' and sts = '%s'", note_mst.br_no, \
                note_mst.tel, note_mst.note_type, note_mst.beg_note_no, \
                note_mst.end_note_no, note_mst.sts);
    if (ret)
    {
        sprintf(acErrMsg, "执行Note_mst_Dec_Upd错!ret=[%d]", ret);
        WRITEMSG
        return(-1);
    }

    ret = Note_mst_Fet_Upd(&sNoteMst, g_pub_tx.reply);

    if (ret == 100)
    {
        sprintf(acErrMsg, "不存在这样的凭证区间!");
        WRITEMSG
        strcpy(g_pub_tx.reply, "N003");
        return(-1);
    }else if (ret)
    {
        sprintf(acErrMsg, "执行Note_mst_Fet_Upd出错!ret=[%d]", ret);
        WRITEMSG
        return(-1);
    }else                 /* 0 找到满足条件的记录 */
    {
        /* 检查头是否相同 */
        if(!strcmp(sNoteMst.beg_note_no, note_mst.beg_note_no)) /* 头相同 */
        {
            if(!strcmp(sNoteMst.end_note_no, note_mst.end_note_no)) /* 尾相同 */
            {
                /* 1: 头尾相同, 删除记录 */
        ret = sql_execute("delete from note_mst \
                where br_no='%s' and tel='%s'\
                                and note_type = '%s' and beg_note_no <= '%s' \
                                and end_note_no >= '%s' and sts = '%s'", \
                                note_mst.br_no, \
                note_mst.tel, note_mst.note_type, note_mst.beg_note_no, \
                note_mst.end_note_no, note_mst.sts);
                if (ret)
                {
                    sprintf(acErrMsg, "删除柜员凭证台帐错!");
                    WRITEMSG
                    strcpy(g_pub_tx.reply, "N005");
                    return(-1);
                }
            }else  /* 尾不同 */
            {
                /* 2: 头相同尾不同 */
                /* 修改原记录的起始号码、数量 */
                ret = pub_com_CalNoteNo(note_mst.note_type, note_mst.end_note_no, \
                             str_tmp, 1);
                if (ret == -1)
                {
                    sprintf(acErrMsg, "计算凭证号码错!");
                    WRITEMSG
                    return(-1);
                }

                strcpy(sNoteMst.beg_note_no, str_tmp);

                /* 计算凭证数量 */
                sNoteMst.cnt -=  note_mst.cnt;

                /* 修改原凭证 */
                ret = Note_mst_Upd_Upd(sNoteMst, g_pub_tx.reply);
                if (ret == -239)
                {
                        sprintf(acErrMsg, "修改柜员凭证台帐note_mst错, 有重复记录!");
                    WRITEMSG
                    strcpy(g_pub_tx.reply, "N019");
                    return(-1);
                }else if(ret)
                {
                    sprintf(acErrMsg, "执行Note_mst_Upd_Upd错!ret=[%d]", ret);
                    WRITEMSG
                    return(-1);
                }
            }
        }else /* 头不同 */
        {
            if(!strcmp(sNoteMst.end_note_no, note_mst.end_note_no)) /* 尾相同 */
            {
                /* 3: 头不同尾相同: 修改原数据 */
                /* 计算凭证终止号码 */
                ret = pub_com_CalNoteNo(note_mst.note_type, \
                          note_mst.beg_note_no, str_tmp, -1);
                if (ret == -1)
                {
                        sprintf(acErrMsg, "计算凭证号码错!");
                    WRITEMSG
                    return(-1);
                }
                strcpy(sNoteMst.end_note_no, str_tmp);

                /* 计算凭证数量 */
                sNoteMst.cnt -= note_mst.cnt;

                ret = Note_mst_Upd_Upd(sNoteMst, g_pub_tx.reply);
                if (ret == -239)
                {
                    sprintf(acErrMsg, "修改柜员凭证台帐note_mst错, 有重复记录!");
                    WRITEMSG
                    strcpy(g_pub_tx.reply, "N019");
                    return(-1);
                }else if(ret)
                {
                    sprintf(acErrMsg, "执行Note_mst_Upd_Upd错!ret=[%d]", ret);
                    WRITEMSG
                    return(-1);
                }
            }else   /* 尾不同 */
            {
                /* 4: 头尾都不同 */
                sNoteMstTmp = sNoteMst;

                /* 修改原来的凭证, 分成2段 */
                /* 1: 计算凭证终止号码 */
                ret = pub_com_CalNoteNo(sNoteMst.note_type, note_mst.beg_note_no, \
                                      sNoteMst.end_note_no, -1);
                if (ret == -1)
                {
                        sprintf(acErrMsg, "计算凭证号码错!");
                    WRITEMSG
                    return(-1);
                }

                /* 计算凭证数量 */
                ret = pub_com_CalNoteCnt(note_mst.note_type, \
                        sNoteMst.beg_note_no, sNoteMst.end_note_no);
                if (ret == -1)
                {
                        sprintf(acErrMsg, "计算凭证数量错!");
                    WRITEMSG
                    return(-1);
                }
                sNoteMst.cnt = ret;

                /* 修改柜员凭证 */
                ret = Note_mst_Upd_Upd(sNoteMst, g_pub_tx.reply);
                if (ret == -239)
                {
                        sprintf(acErrMsg, "修改柜员凭证台帐note_mst错!");
                    WRITEMSG
                    strcpy(g_pub_tx.reply, "N019");
                    return(-1);
                }else if(ret)
                {
                    sprintf(acErrMsg, "执行Note_mst_Upd_Upd错!ret=[%d]", ret);
                    WRITEMSG
                    return(-1);
                }

                /* 2: 插入另一个 */
                /* 计算凭证起始号码 */
                ret = pub_com_CalNoteNo(sNoteMstTmp.note_type, \
                          note_mst.end_note_no, sNoteMstTmp.beg_note_no, 1);
                if (ret == -1)
                {
                        sprintf(acErrMsg, "计算凭证号码错!");
                    WRITEMSG
                    return(-1);
                }
                /* 计算凭证数量 */
                ret = pub_com_CalNoteCnt(sNoteMstTmp.note_type, \
                        sNoteMstTmp.beg_note_no, sNoteMstTmp.end_note_no);
                if (ret == -1)
                {
                        sprintf(acErrMsg, "计算凭证号码错!");
                    WRITEMSG
                    return(-1);
                }
                sNoteMstTmp.cnt = ret;

                /* 插入一条新纪录*/
                ret = Note_mst_Ins(sNoteMstTmp, g_pub_tx.reply);
            if (ret == -239)
            {
                sprintf(acErrMsg, "向柜员凭证台帐note_mst插入记录错!有重复记录!");
                WRITEMSG
                strcpy(g_pub_tx.reply, "N020");
                return(-1);
            }else if(ret)
            {
                sprintf(acErrMsg, "执行Note_mst_Ins错!ret=[%d]", ret);
                WRITEMSG
                return(-1);
            }
            }
        }
    }

    /* 关闭游标 */
    Note_mst_Clo_Upd();

    return(0);
}

/**********************************************************************
 * 函 数 名:   pub_com_CalNoteNo
 * 函数功能:   根据凭证号码与数值计算新的凭证号码
 * 作者/时间:  jack / 2003年12月16日
 *
 * 参    数:
 *     输入:
 *            char *note_type    凭证种类
 *            char *note_no      起始号码
 *            int  cnt           凭证数量
 *
 *     输出:
 *            char *re_note_no   终止号码
 *            char *g_pub_tx.reply        响应码
 *
 *   返回值:   0: 成功 -1: 不成功
 *
 * 修改历史:   包括修改人、时间、修改内容, 以时间倒序方式
 *
********************************************************************/
int pub_com_CalNoteNo(
    char *note_type,
    char *note_no,
    char *re_note_no,
    int  cnt)
{
    struct  note_parm_c  sNoteParm;  /* 取凭证参数 */
    char    cForm[20], cNoteNo[17];
    long    lNoteNo;
    int     iValLen;      /* 去掉冠子头后凭证号长度 */
    int     ret;

    MEMSET_DEBUG(cForm, 0x00, sizeof(cForm));
    MEMSET_DEBUG(cNoteNo, 0x00, sizeof(cNoteNo));

    /* 根据凭证类型取凭证参数表 */
    ret = Note_parm_Sel(g_pub_tx.reply, &sNoteParm, "note_type = '%s'", note_type);
    if(ret==100)
    {
        sprintf(acErrMsg, "无此凭证种类!note_type=[%s]", note_type);
        WRITEMSG
        strcpy(g_pub_tx.reply, "N001");
        return (-1);
    }else if (ret)
    {
        sprintf(acErrMsg, "执行Note_parm_Sel错误!ret=[%d]", ret);
        WRITEMSG
        return(-1);
    }

    /* 去掉冠子头后凭证序号的长度 */
    iValLen = 16 - sNoteParm.head_len;

    /* 起始号码值 */
    lNoteNo = atol(note_no + sNoteParm.head_len);
    lNoteNo += cnt;

    /* 输出序号格式 */
    sprintf(cForm, "%%0%dld", iValLen);
    sprintf(cNoteNo, cForm, lNoteNo);    /* 终止号码 */

    /* 冠子头加上序号＝凭证号码 */
    strncpy(re_note_no, note_no, sNoteParm.head_len);
    re_note_no[sNoteParm.head_len] = '\0';
    strcat(re_note_no, cNoteNo);

    return(0);
}

/**********************************************************************
 * 函 数 名:   pub_com_RegTelNote
 * 函数功能:   登记柜员凭证
 * 作者/时间:  jack/2003年12月24日
 *
 * 参    数:
 *     输入:
  *           char *tel          柜员号
 *        char *br_no    机构号码
 *            char *pNoteAct     凭证行为
 *            char *note_typ     凭证种类
 *            char *beg_no       起始号码
 *            char *end_no       终止号码
 *
 *     输出:
 *            char g_pub_tx.reply[]       响应码
 *
 *   返回值:  0:   成功
 *            非0: 不成功
 *
 * 修改历史:   包括修改人、时间、修改内容, 以时间倒序方式
 *
********************************************************************/

int pub_com_RegTelNote(tel, br_no, pNoteAct, note_type, beg_no, end_no)
    char *tel;
    char *br_no;
    char *pNoteAct;
    char *note_type;
    char *beg_no;
    char *end_no;
{
    struct note_mst_c note_mst;     /* 柜员凭证台帐结构 */
    int    ret;
    char   pNoteSts[2];
    char   cPerNoteSts[2];  /* 行为执行的状态 */
    char   cWho[2];     /* 凭证行为执行者 */

    MEMSET_DEBUG(&note_mst, 0X00, sizeof(struct note_mst_c));
    MEMSET_DEBUG(cPerNoteSts, 0x00, sizeof(cPerNoteSts));
    MEMSET_DEBUG(cWho, 0x00, sizeof(cWho));

    strcpy(cWho, "T");

    /* 赋值 */
    strcpy(note_mst.tel, tel);        /* 柜员号   */
    strcpy(note_mst.br_no, br_no);        /* 机构号码 */
    strcpy(note_mst.note_type, note_type);    /* 凭证类型 */
    strcpy(note_mst.beg_note_no, beg_no); /* 起始号码 */
    strcpy(note_mst.end_note_no, end_no); /* 终止号码 */

    /* 根据凭证行为取出响应的凭证状态 */
    ret=pub_com_NoteActSts(cWho, pNoteAct, pNoteSts, cPerNoteSts);
    if(ret == 1)
    {
        sprintf(acErrMsg, "凭证行为超出处理范围!");
        WRITEMSG
        strcpy(g_pub_tx.reply, "N009");
        return(-1);
    }

    strcpy(note_mst.sts, pNoteSts);       /* 凭证状态 */

    /* 计算凭证数量 */
    ret=pub_com_CalNoteCnt(note_type, beg_no, end_no);
    if(ret <= 0)
    {
        sprintf(acErrMsg, "计算凭证数量错!");
        WRITEMSG
        return(-1);
    }

    note_mst.cnt = ret;             /* 凭证数量 */

    /* 凭证连续性检查(判断是否并段) */
    ret=pub_com_NoteMerg(note_mst);
    if(ret == -1)
    {
        sprintf(acErrMsg, "凭证并段处理出错!");
        WRITEMSG
        return(-1);
    }

    /* 凭证最小号检查并登记 */

    return(0);

}

/**********************************************************************
 * 函 数 名:  pub_com_ChkNoteMin
 * 函数功能:  凭证最小号码检查。 
 *            检查该凭证号码是否为最小号控制,
 *            如果需要控制检查是否为最小号码 
 * 作者/时间:  jack/2003年12月16日 
 *
 * 参    数:
 *     输入:
 *            char *note_type   凭证种类 
 *            char *beg_note_no 起始号码 
 *            char *tel         操 作 员 
 *
 *     输出:
 *            char *beg_note_no          起始号码(最小号)
 *            char *g_pub_tx.reply       响应码 
 *
 * 返 回 值:   0: 为最小号码 1: 不控制最小号 2:返回新的最小号 -1:出错 
 *
 * 修改历史:   包括修改人、时间、修改内容, 以时间倒序方式 
 *
********************************************************************/

int pub_com_ChkNoteMin(
       char *note_type,
       char *beg_note_no,
       char *tel)
{
    struct    note_mst_c    sNoteMst;
    struct    note_parm_c   sNoteParm;
    struct    note_min_no_c sNoteMinNo;
    char    cMinNo[17];     /* 保存最小号码 */
    char    cInHead[17];    /*待检查的凭证号的冠子头*/
    char    cMinHead[17];   /*凭证最小号码的冠子头*/
    long    lBegNo, lMinNo; /* 开始号码和最小号码(不带冠子头) */

    int    ret;

    MEMSET_DEBUG(&sNoteParm, 0x00, sizeof(struct note_parm_c));
    MEMSET_DEBUG(&sNoteMst, 0x00, sizeof(struct note_mst_c));
    MEMSET_DEBUG(&sNoteMinNo, 0x00, sizeof(struct note_min_no_c));

    /* 根据凭证类型取凭证结构 */
    ret = Note_parm_Sel(g_pub_tx.reply, &sNoteParm, "note_type = '%s'", note_type);
    if(ret== 100)
    {
        sprintf(acErrMsg, "无此凭证类型!note_type=[%s]", note_type);
        WRITEMSG
        strcpy(g_pub_tx.reply, "N001");
        return (-1);
    }else if (ret)
    {
        sprintf(acErrMsg, "执行Note_parm_Sel错误!ret=[%d]", ret);
        WRITEMSG
        strcpy(g_pub_tx.reply, "N001");
        return(-1);
    }

    /*** 最小号标志为N, 则不进行控制 ***/
    if (sNoteParm.order_ind[0] == 'N')
    {
        sprintf(acErrMsg, "该凭证类型不进行最小号控制");
        WRITEMSG
        return(0);
    }

    /* 取凭证号码冠子头 */
    strncpy(cInHead, beg_note_no, sNoteParm.head_len);
    cInHead[sNoteParm.head_len] = '\0';

    /* 取凭证最小号 */
    ret = Note_min_no_Sel(g_pub_tx.reply, &sNoteMinNo, "tel = '%s' \
                          and note_type = '%s'", tel, note_type);
    if (ret == 100)  /* 如果凭证最小号表中不存在记录, 则从note_mst表中查找最小号 */
    {
        sprintf(acErrMsg, "note_min_no表中无此凭证最小号!tel=[%s], note_type=[%s]", tel, note_type);
        WRITEMSG

        ret = sql_min_string("note_mst", "beg_note_no", cMinNo, sizeof(cMinNo)-1,
        "tel='%s' and note_type='%s' and substr(beg_note_no, 1, %ld)='%s' and sts='0'"
                 , tel, note_type, sNoteParm.head_len, cInHead);
        if(ret)
        {
            sprintf(acErrMsg, "执行sql_min_string错误！");
            WRITEMSG
            strcpy(g_pub_tx.reply, "D110");
            return(-1);
        }

        if(!strlen(cMinNo))
        {
            strcpy(g_pub_tx.reply, "N075");
            return(-1);
        }

        /* 检查是否是最小号(从note_mst种查找), 如果不是最小号, 则返回最小号 */
        if(strcmp(beg_note_no, cMinNo) > 0)
        {
            sprintf(acErrMsg, "凭证非最小号码, 凭证号[%s] 最小号[%s]", \
                    beg_note_no, cMinNo);
            WRITEMSG
            strcpy(beg_note_no, cMinNo);
            return(2);
        }
    }
    else if (ret)
    {
        sprintf(acErrMsg, "执行Note_min_no_Sel错误!ret=[%d]", ret);
        WRITEMSG
        strcpy(g_pub_tx.reply, "D101");
        return(-1);
    }

    /* 取凭证最小号码的冠子头 */
    strncpy(cMinHead, sNoteMinNo.note_min_no, sNoteParm.head_len);
    cMinHead[sNoteParm.head_len] = '\0';

    /* 去掉冠子头后凭证号码和最小号码 */
    lBegNo = atol(beg_note_no + sNoteParm.head_len);
    lMinNo = atol(sNoteMinNo.note_min_no + sNoteParm.head_len);

    /* 检查note_mst中是否存在以cInHead开头的凭证 */

    vtcp_log("tel = '%s' and sts = '0' and note_type = '%s' and beg_note_no[1, %d] = '%s'", tel, note_type, sNoteParm.head_len, cInHead);

    ret = sql_count("note_mst", "tel = '%s' and sts = '0' \
                      and note_type = '%s' and substr(beg_note_no, 1, %d) = '%s'", \
                      tel, note_type, sNoteParm.head_len, cInHead);
    if(ret < 0)
    {
        sprintf(acErrMsg, "执行sql_count错, ret=[%d]", ret);
        WRITEMSG
        strcpy(g_pub_tx.reply, "D109");
        return(-1);
    }
    if(ret == 0)
    {
        sprintf(acErrMsg, "柜员无此凭证tel=[%s], note_type[%s]", tel, note_type);
        WRITEMSG
        strcpy(g_pub_tx.reply, "N007");
        return(-1);
    }

    /* 比较冠子头是否相同 */
    if(!strcmp(cInHead, cMinHead))  /* 如果冠子头相同 */
    {
        if(lBegNo != lMinNo)  /* 比较凭证号码 */
        {
            sprintf(acErrMsg, "凭证非最小号码, 凭证号[%d]最小号[%d][%s]", lBegNo, lMinNo, note_type);
            WRITEMSG
            strcpy(g_pub_tx.reply, "N014");
            /* 返回前台最小号 */
            sprintf(beg_note_no, "%d", lMinNo);
            sprintf(acErrMsg, "最小号是%d", lMinNo);
            set_zd_data(DC_GET_MSG, acErrMsg);
            WRITEMSG
            return(2);
        }
    }else  /* 如果冠子头不同 */
    {
        ret = sql_min_string("note_mst", "beg_note_no", cMinNo, sizeof(cMinNo)-1, \
                 "tel = '%s' and note_type = '%s' and substr(beg_note_no, 1, %ld) = '%s' \
                 and sts = '0'", tel, note_type, sNoteParm.head_len, cInHead);
        if(ret)
        {
            sprintf(acErrMsg, "执行sql_min_string错误！");
            WRITEMSG
            strcpy(g_pub_tx.reply, "D110");
            return(-1);
        }
        /* 检查是否是最小号(从note_mst种查找) */
        if(strcmp(beg_note_no, cMinNo) != 0)
        {
            strcpy(g_pub_tx.reply, "N014");
            sprintf(acErrMsg, "凭证非最小号码, 凭证号[%s] 最小号[%s]", \
                    beg_note_no, cMinNo);
            WRITEMSG
            strcpy(beg_note_no, cMinNo);
            /* 返回前台最小号 */
            sprintf(acErrMsg, "最小号是%s", cMinNo);
            set_zd_data(DC_GET_MSG, acErrMsg);
            WRITEMSG
            return(2);
        }
    }

    return(0);
}

/**********************************************************************
 * 函 数 名:   pub_com_ChkNoteHead
 * 函数功能:   检查凭证是否带冠子头,
 *            如果带冠子头, 则直接退出；
 *            如果不带, 需要查找该柜员是否存在起始号码为该号码的凭证, 存在需要
 *                增加冠子头, 不存在则提示错误信息
 * 作者/时间:  jack/2003年12月24日
 *
 * 参    数:
 *     输入:
 *            char *note_type   凭证种类
 *            char *note_no     凭证号码(可能带冠子头, 可能不带冠子头)
 *            char *tel         操 作 员
 *
 *     输出:
 *            char *note_no     凭证号(带冠子头)
 *            char *g_pub_tx.reply       响应码
 *
 * 返 回 值:   0: 成功 -1: 失败
 *
 * 修改历史:   包括修改人、时间、修改内容, 以时间倒序方式
 *
********************************************************************/

int pub_com_ChkNoteHead(
    char *note_type,
    char *note_no,
    char *tel)
{
    struct    note_parm_c   sNoteParm;
    struct    note_mst_c    sNoteMst;

    char    cForm[32];      /* 格式化输出 */
    char    cNoteNo[17];    /* 如果输入号码不带冠子头, 则用这个变量 */
    char    cNoteNoTmp[17];

    long    lNoteNo;
    int     ret;

    MEMSET_DEBUG(&sNoteParm, 0x00, sizeof(struct note_parm_c));
    MEMSET_DEBUG(&sNoteMst, 0x00, sizeof(struct note_mst_c));

    /* 如果note_no长度为16位, 则直接返回正确 */
    if (strlen(note_no) == 16)
    {
        return(0);
    }

    /* 根据凭证类型取凭证结构 */
    ret = Note_parm_Sel(g_pub_tx.reply, &sNoteParm, "note_type = '%s'", note_type);
    if(ret==100)
    {
        sprintf(acErrMsg, "无此凭证种类!note_type=[%s]", note_type);
        WRITEMSG
        strcpy(g_pub_tx.reply, "N001");
        return (-1);
    }else if (ret)
    {
        sprintf(acErrMsg, "执行Note_parm_Sel错误!ret=[%d]", ret);
        WRITEMSG
        return(-1);
    }

    /* 将输入的凭证号转化成long类型 */
    lNoteNo = atol(note_no);

    /* 判断凭证号是否带冠子头 16位 如果不带, 则自动加上冠子头*/
    sprintf(cForm, "%%0%dd", 16 - sNoteParm.head_len);
    sprintf(cNoteNo, cForm, lNoteNo);

    /* 检查note_mst中是否存在以cInHead开头的凭证 */
    vtcp_log("AA  tel = '%s' and sts = '0' \
                  and note_type = '%s' and substr(beg_note_no, %d, 16-%d+1) <= '%s' \
                  and substr(end_note_no, %d, 16-%d+1) >= '%s'", tel, note_type, \
                  sNoteParm.head_len+1, sNoteParm.head_len+1, cNoteNo, sNoteParm.head_len+1, sNoteParm.head_len+1, cNoteNo);
    ret = sql_count("note_mst", "tel = '%s' and sts = '0' \
                  and note_type = '%s' and substr(beg_note_no, %d, 16-%d+1) <= '%s' \
                  and substr(end_note_no, %d, 16-%d+1) >= '%s'", tel, note_type, \
                  sNoteParm.head_len+1, sNoteParm.head_len+1, cNoteNo, sNoteParm.head_len+1, sNoteParm.head_len+1, cNoteNo);
    if(ret < 0)
    {
        sprintf(acErrMsg, "执行sql_count错!ret=[%d]", ret);
        WRITEMSG
        strcpy(g_pub_tx.reply, "D109");
        return(-1);
    }
    if(ret > 1)
    {
        sprintf(acErrMsg, "找到多条凭证, 输入号码太短, ret=[%d]", ret);
        WRITEMSG
        strcpy(g_pub_tx.reply, "N008");
        return(-1);
    }
    if(ret == 0)
    {
        sprintf(acErrMsg, "柜员无此凭证note_no[%s]", note_no);
        WRITEMSG
        strcpy(g_pub_tx.reply, "N007");
        return(-1);
    }

    /* 根据凭证号找起始凭证(带冠子头)*/
    ret = Note_mst_Sel(g_pub_tx.reply, &sNoteMst, "tel = '%s' and sts = '0' \
                  and note_type = '%s' and substr(beg_note_no, %d, 16-%d+1) <= '%s' \
                  and substr(end_note_no, %d, 16-%d+1) >= '%s'", tel, note_type, \
                  sNoteParm.head_len+1, sNoteParm.head_len+1, cNoteNo, sNoteParm.head_len+1, sNoteParm.head_len+1, cNoteNo);
    if(ret==100)
    {
        sprintf(acErrMsg, "柜员无此凭证!");
        WRITEMSG
        strcpy(g_pub_tx.reply, "N007");
        return(-1);
    }
    else if (ret)
    {
        sprintf(acErrMsg, "执行Note_mst_Sel错误!ret=[%d]", ret);
        WRITEMSG
        return(-1);
    }

    strncpy(cNoteNoTmp, sNoteMst.beg_note_no, sNoteParm.head_len);
    cNoteNoTmp[sNoteParm.head_len] = '\0';

    strcat(cNoteNoTmp, cNoteNo);

    strcpy(note_no, cNoteNoTmp);

    return(0);
}

/**********************************************************************
 * 函 数 名:   pub_com_ChkNoteHead1
 * 函数功能:   检查凭证是否带冠子头,
 *            如果带冠子头, 则直接退出；
 *            如果不带, 需要查找该柜员是否存在起始号码为该号码的凭证, 存在需要
 *                增加冠子头, 不存在则提示错误信息
 *            为银行承兑汇票退票、托收登记和核销时检查用；
 * 作者/时间:  jack/2003年12月24日
 *
 * 参    数:
 *     输入:
 *            char *note_type   凭证种类
 *            char *note_no     凭证号码(可能带冠子头, 可能不带冠子头)
 *
 *     输出:
 *            char *note_no     凭证号(带冠子头)
 *            char *g_pub_tx.reply       响应码
 *
 * 返 回 值:   0: 成功 -1: 失败
 *
 * 修改历史:   包括修改人、时间、修改内容, 以时间倒序方式
 *              jack/2005.08.04/凭证状态改为4
 *
********************************************************************/

int pub_com_ChkNoteHead1(
    char *note_type,
    char *note_no
)
{
    struct    note_parm_c   sNoteParm;
    struct    note_mst_c    sNoteMst;

    char    cForm[32];      /* 格式化输出 */
    char    cNoteNo[17];    /* 如果输入号码不带冠子头, 则用这个变量 */
    char    cNoteNoTmp[17];

    long    lNoteNo;
    int     ret;

    MEMSET_DEBUG(&sNoteParm, 0x00, sizeof(struct note_parm_c));
    MEMSET_DEBUG(&sNoteMst, 0x00, sizeof(struct note_mst_c));

    /* 如果note_no长度为16位, 则直接返回正确 */
    if (strlen(note_no) == 16)
    {
        return(0);
    }

    /* 根据凭证类型取凭证结构 */
    ret = Note_parm_Sel(g_pub_tx.reply, &sNoteParm, "note_type = '%s'", note_type);
    if(ret==100)
    {
        sprintf(acErrMsg, "无此凭证种类!note_type=[%s]", note_type);
        WRITEMSG
        strcpy(g_pub_tx.reply, "N001");
        return (-1);
    }else if (ret)
    {
        sprintf(acErrMsg, "执行Note_parm_Sel错误!ret=[%d]", ret);
        WRITEMSG
        return(-1);
    }

    /* 将输入的凭证号转化成long类型 */
    lNoteNo = atol(note_no);

    /* 判断凭证号是否带冠子头 16位 如果不带, 则自动加上冠子头*/
    sprintf(cForm, "%%0%dd", 16 - sNoteParm.head_len);
    sprintf(cNoteNo, cForm, lNoteNo);

    /* 检查note_mst中是否存在以cInHead开头的凭证 */
    vtcp_log("AA note_type = '%s' and beg_note_no[%d, 16] <= '%s' \
                  and end_note_no[%d, 16] >= '%s'", note_type, \
                  sNoteParm.head_len+1, cNoteNo, sNoteParm.head_len+1, cNoteNo);
    ret = sql_count("note_mst", " note_type = '%s' and substr(beg_note_no, %d, 16-%d+1) <= '%s' \
                  and substr(end_note_no, %d, 16-%d+1) >= '%s'", note_type, \
                  sNoteParm.head_len+1, sNoteParm.head_len+1, cNoteNo, sNoteParm.head_len+1, sNoteParm.head_len+1, cNoteNo);
    if(ret < 0)
    {
        sprintf(acErrMsg, "执行sql_count错!ret=[%d]", ret);
        WRITEMSG
        strcpy(g_pub_tx.reply, "D109");
        return(-1);
    }
    if(ret > 1)
    {
        sprintf(acErrMsg, "找到多条凭证, 输入号码太短, ret=[%d]", ret);
        WRITEMSG
        strcpy(g_pub_tx.reply, "N008");
        return(-1);
    }
    if(ret == 0)
    {
        sprintf(acErrMsg, "无此凭证note_no[%s]", note_no);
        WRITEMSG
        strcpy(g_pub_tx.reply, "N069");
        return(-1);
    }

    /* 根据凭证号找起始凭证(带冠子头)*/
    ret = Note_mst_Sel(g_pub_tx.reply, &sNoteMst, " note_type = '%s' and substr(beg_note_no, %d, 16-%d+1) <= '%s' \
                  and substr(end_note_no, %d, 16-%d+1) >= '%s'", note_type, \
                  sNoteParm.head_len+1, sNoteParm.head_len+1, cNoteNo, sNoteParm.head_len+1, sNoteParm.head_len+1, cNoteNo);
    if(ret==100)
    {
        sprintf(acErrMsg, "无此凭证!");
        WRITEMSG
        strcpy(g_pub_tx.reply, "N069");
        return(-1);
    }
    else if (ret)
    {
        sprintf(acErrMsg, "执行Note_mst_Sel错误!ret=[%d]", ret);
        WRITEMSG
        return(-1);
    }

    strncpy(cNoteNoTmp, sNoteMst.beg_note_no, sNoteParm.head_len);
    cNoteNoTmp[sNoteParm.head_len] = '\0';

    strcat(cNoteNoTmp, cNoteNo);

    strcpy(note_no, cNoteNoTmp);

    return(0);
}

/**********************************************************************
 * 函 数 名:   pub_com_RegNoteMin
 * 函数功能:   凭证最小号码检查登记。
 *            检查转入的是否比已有凭证号码小, 如果小登记;
 *            检查转出的凭证是否是当前凭证号码的最小号码, 是找出转出后最小号码
 *            并登记
 * 作者/时间:  jack/2003年12月26日
 *
 * 参    数:
 *     输入:
 *            char *br_no        机构编码
 *            char *note_type    凭证种类
 *            char *note_no      起始号码
 *            char *tel          操 作 员
 *            char *in_out       转移标志 I: 转入 O: 转出
 *
 *     输出:
 *            char *g_pub_tx.reply   响应码
 *
 *   返回值:   0: 成功 -1: 不成功
 *
 * 修改历史:   包括修改人、时间、修改内容, 以时间倒序方式
 *
********************************************************************/
int pub_com_RegNoteMin(
    char *br_no,
    char *note_type,
    char *note_no,
    char *tel,
    char *in_out)
{
    struct    note_parm_c     sNoteParm;
    struct    note_min_no_c   sNoteMinNoTmp;   /* 保存查找到的凭证最小号码 */
    struct    note_min_no_c   sNoteMinNo;      /* 保存入口参数的凭证最小号码 */
    char      cHead[17], cMinHead[17];

    int        ret;

    MEMSET_DEBUG(&sNoteParm, 0x00, sizeof(struct note_parm_c));
    MEMSET_DEBUG(&sNoteMinNoTmp, 0x00, sizeof(struct note_min_no_c));
    MEMSET_DEBUG(&sNoteMinNo, 0x00, sizeof(struct note_min_no_c));
    MEMSET_DEBUG(cHead, 0x00, sizeof(cHead));
    MEMSET_DEBUG(cMinHead, 0x00, sizeof(cMinHead));

    strcpy(sNoteMinNo.br_no, br_no);
    strcpy(sNoteMinNo.note_type, note_type);
    strcpy(sNoteMinNo.tel , tel);
    strcpy(sNoteMinNo.note_min_no, note_no);

    if (in_out[0] == 'I') /* 转入凭证 */
    {
        /* 取凭证参数表 */
        ret = Note_parm_Sel (g_pub_tx.reply, &sNoteParm, "note_type = '%s'", note_type);
        if (ret == 100)
        {
            sprintf(acErrMsg, "无此凭证种类!note_type=[%s]", note_type);
            WRITEMSG
            strcpy(g_pub_tx.reply, "N001");
            return(-1);
        }
        else if (ret)
        {
            sprintf(acErrMsg, "执行Note_parm_Sel错误!ret=[%d]", ret);
            WRITEMSG
            return(-1);
        }

    /* 取凭证最小号表 */
    ret = Note_min_no_Dec_Upd(g_pub_tx.reply, "tel = '%s' \
                              and note_type = '%s'", tel, note_type);
    if (ret)
    {
        sprintf(acErrMsg, "执行Note_min_no_Dec_Upd错误!ret=[%d]", ret);
        WRITEMSG
        return(-1);
    }

    ret = Note_min_no_Fet_Upd(&sNoteMinNoTmp, g_pub_tx.reply);
    if (ret == 100)   /* 没找到最小号, 将当前号码作为最小号*/
    {
        /* 找凭证最小号 */
            ret = pub_com_GetNoteMin(note_type, sNoteMinNo.note_min_no, tel);
            if(ret==-1)
        {
            sprintf(acErrMsg, "查找凭证最小号失败!");
            WRITEMSG
            strcpy(g_pub_tx.reply, "N036");
            return -1;
        }

            /* 插入最小号数据 */
            ret = Note_min_no_Ins(sNoteMinNo, g_pub_tx.reply);
            if (ret==-239)
            {
                sprintf(acErrMsg, "向凭证最小号表插入记录错误!有重复记录!");
                WRITEMSG
                strcpy(g_pub_tx.reply, "N017");
                return(-1);
            }else if(ret)
            {
                sprintf(acErrMsg, "执行Note_min_no_Ins错!ret=[%d]", ret);
                WRITEMSG
                return(-1);
            } /* end if */
    }else if(ret)
    {
        sprintf(acErrMsg, "执行Note_min_no_Fet_Upd错误!ret=[%d]", ret);
        WRITEMSG
        return(-1);
    }
    else   /* 找到最小号, 与输入号码进行比较, 并修改最小号表 */
        {
            /* 比较冠子头是否相同 */
            strncpy(cHead, sNoteMinNo.note_min_no, sNoteParm.head_len);
            strncpy(cMinHead, sNoteMinNoTmp.note_min_no, sNoteParm.head_len);
            cHead[sNoteParm.head_len] = '\0';
            cMinHead[sNoteParm.head_len] = '\0';

            if (!strcmp(cHead, cMinHead)) /* 如果冠子头相同 */
            {
                if(strcmp(sNoteMinNo.note_min_no, sNoteMinNoTmp.note_min_no)<0)
                {
                    /* 修改凭证最小号 */
                    strcpy(sNoteMinNoTmp.note_min_no, sNoteMinNo.note_min_no);
                }
            }else    /* 如果不同冠子头 */
            {
                ret = pub_com_GetNoteMin(note_type, sNoteMinNo.note_min_no, tel);
                if(ret == -1)
            {
                sprintf(acErrMsg, "查找凭证最小号失败!");
                WRITEMSG
                strcpy(g_pub_tx.reply, "N036");
                return -1;
            }
            strcpy(sNoteMinNoTmp.note_min_no, sNoteMinNo.note_min_no);
            }
            /* 修改凭证最小号 */
            ret = Note_min_no_Upd_Upd(sNoteMinNoTmp, g_pub_tx.reply);
            if (ret == -239)
            {
                sprintf(acErrMsg, "更新凭证最小号表note_min_no错!有重复记录!");
                WRITEMSG
                strcpy(g_pub_tx.reply, "N017");
                return(-1);
            }else if(ret)
            {
                sprintf(acErrMsg, "执行Note_min_no_Upd_Upd错!ret=[%d]", ret);
                WRITEMSG
                return(-1);
            }
        }
        Note_min_no_Clo_Upd();
    }

    else if (in_out[0] == 'O')  /* 转出 */
    {
    /* 找凭证最小号码 */
        ret = pub_com_GetNoteMin(note_type, sNoteMinNo.note_min_no, tel);
        if(ret==-1)
    {
        sprintf(acErrMsg, "查找凭证最小号失败!");
        WRITEMSG
        strcpy(g_pub_tx.reply, "N036");
        return -1;
    }

        /* 取凭证最小号表 */
    ret = Note_min_no_Dec_Upd(g_pub_tx.reply, "tel = '%s' \
                            and note_type = '%s'", tel, note_type);
    if (ret)
    {
        sprintf(acErrMsg, "执行Note_min_no_Dec_Upd错误!ret=[%d]", ret);
        WRITEMSG
        return(-1);
    }

    ret = Note_min_no_Fet_Upd(&sNoteMinNoTmp, g_pub_tx.reply);
    if (ret == 100)   /* 没找到最小号, 插入新记录 */
    {
            /* 插入最小凭证号码 */
            ret = Note_min_no_Ins(sNoteMinNo, g_pub_tx.reply);
            if (ret == -239)
            {
                sprintf(acErrMsg, "更新凭证最小号表错!有重复记录!");
                WRITEMSG
                strcpy(g_pub_tx.reply, "N017");
                return(-1);
            }else if(ret)
            {
                sprintf(acErrMsg, "执行Note_min_no_Ins错!ret=[%d]", ret);
                WRITEMSG
                return(-1);
            }
        }
        else if(ret)
    {
        sprintf(acErrMsg, "执行Note_min_no_Fet_Upd错误!ret=[%d]", ret);
        WRITEMSG
        return(-1);
        }else   /* 找到最小号, 则修改最小号码 */
        {
            strcpy(sNoteMinNoTmp.note_min_no, sNoteMinNo.note_min_no);

            /* 修改最小凭证号码 */
            ret = Note_min_no_Upd_Upd(sNoteMinNoTmp, g_pub_tx.reply);
            if (ret == -239)
            {
                sprintf(acErrMsg, "更新凭证最小号表错!有重复记录!");
                WRITEMSG
                strcpy(g_pub_tx.reply, "N017");
                return(-1);
            }else if(ret)
            {
                sprintf(acErrMsg, "执行Note_min_no_Upd_Upd错!ret=[%d]", ret);
                WRITEMSG
                return(-1);
            }
        }
        Note_min_no_Clo_Upd();
    }else
    {
        sprintf(acErrMsg, "凭证转移方向错误, brno=[%s], in_out=[%s]", br_no, in_out);
        WRITEMSG
        return(-1);
    }
    return(0);
}

/**********************************************************************
 * 函 数 名:   pub_com_NoteMstSts
 * 函数功能:   取凭证状态函数
 *            根据输入的凭证号码(起始、终止)、操作员、凭证类型获得凭证状态
 *
 * 作者/时间:  jack/2003年12月26日
 *
 * 参    数:
 *     输入:
 *            char *note_type    凭证种类
 *            char *beg_note_no  起始号码
 *            char *end_note_no  终止号码
  *           char *tel          操 作 员
 *
 *     输出:
 *            char *sts     凭证状态
 *            char *g_pub_tx.reply   响应码
 *
 *   返回值:   0: 成功 -1: 不成功
 *
 * 修改历史:   包括修改人、时间、修改内容, 以时间倒序方式
 *
********************************************************************/
int pub_com_NoteMstSts(
    char *note_type,
    char *beg_note_no,
    char *end_note_no,
    char *tel,
    char *sts)
{
    struct note_mst_c sNoteMst;
    int    ret;

    MEMSET_DEBUG(&sNoteMst, 0x00, sizeof(struct note_mst_c));

    ret = Note_mst_Sel(g_pub_tx.reply, &sNoteMst , "tel = '%s' \
             and beg_note_no <= '%s' and end_note_no >= '%s' \
             and note_type = '%s'", tel, beg_note_no, end_note_no, note_type);
        if(ret==100)
        {
                sprintf(acErrMsg, "柜员无此凭证!");
                WRITEMSG
                strcpy(g_pub_tx.reply, "N007");
            return(-1);
        }else if (ret)
        {
            sprintf(acErrMsg, "执行Note_mst_Sel错误!ret=[%d]", ret);
            WRITEMSG
            return(-1);
        }

        strcpy(sts, sNoteMst.sts);

        return(0);
}

/**********************************************************************
 * 函 数 名:   pub_com_RegNoteHst
 * 函数功能:   凭证变更登记处理。有凭证变更的时候都要登记该登记簿。
 * 作者/时间:  jack/2003年12月26日
 *
 * 参数:
 *     输入:
 *            char  *note_act   凭证行为
 *            struct note_mst_c note_mst 凭证台帐
 *            char  *tw_br_no   对方机构
 *        char  *tw_tel     对方操作员
 *
 *     输出:  char  *g_pub_tx.reply         相应码
 *
 *   返回值:   0: 登记成功 1: 登记失败
 *
 * 修改历史:   包括修改人、时间、修改内容, 以时间倒序方式
 *
********************************************************************/

int pub_com_RegNoteHst(note_act, note_mst, tw_br_no, tw_tel)
    char   *note_act;
    struct note_mst_c   note_mst;
    char   *tw_br_no;
    char   *tw_tel;
{
    struct note_mst_hst_c sNoteHst;
    int    ret;

    MEMSET_DEBUG(&sNoteHst, 0x00, sizeof(struct note_mst_hst_c));

    strcpy(sNoteHst.tel, g_pub_tx.tel);               /* 操作柜员 */
    strcpy(sNoteHst.note_type, note_mst.note_type);       /* 凭证类型 */
    strcpy(sNoteHst.br_no, note_mst.br_no);           /* 机构编码 */
    strcpy(sNoteHst.beg_note_no, note_mst.beg_note_no);   /* 起始号码 */
    strcpy(sNoteHst.end_note_no, note_mst.end_note_no);   /* 终止号码 */
    sNoteHst.note_cnt = note_mst.cnt;               /* 凭证数量 */
    sNoteHst.tx_date = g_pub_tx.tx_date;            /* 交易日期 */
    sNoteHst.tx_time = g_pub_tx.tx_time;            /* 交易时间 */
    sNoteHst.trace_no = g_pub_tx.trace_no;          /* 流 水 号 */
    sNoteHst.trace_cnt = g_pub_tx.trace_cnt;            /* 流水笔次 */
    strcpy(sNoteHst.auth, g_pub_tx.auth);         /* 授权人   */
    strcpy(sNoteHst.tw_br_no, tw_br_no);          /* 对方机构 */
    strcpy(sNoteHst.tw_tel, tw_tel);              /* 对方柜员 */
    strcpy(sNoteHst.sts_ind, note_act);           /* 操作代码 */

    ret = Note_mst_hst_Ins(sNoteHst, g_pub_tx.reply);
    if (ret == -239)
    {
        sprintf(acErrMsg, "向柜员凭证明细表插入记录错!有重复记录!");
        WRITEMSG
        strcpy(g_pub_tx.reply, "N024");
        return(-1);
    }else if(ret)
    {
        sprintf(acErrMsg, "执行Note_mst_hst_Ins错!ret=[%d]", ret);
        WRITEMSG
        return(-1);
    }

    return(0);
}

/**********************************************************************
 * 函  数 名:   pub_com_ChkNoteUse
 * 函数 功能:   柜员凭证检查:
 *                        检查凭证类型是否存在
 *                        检查凭证是否处于使用状态
 * 作者/时间:   jack/2003年12月29日
 *
 * 参  数:
 *     输入:   char note_type[4]        凭证类型
 *
 *     输出:   char g_pub_tx.reply[]    相应码
 *
 *   返回值:   0: 启用 -1: 停用
 *
 * 修改历史:   包括修改人、时间、修改内容, 以时间倒序方式
 *
********************************************************************/
int pub_com_ChkNoteUse(char *note_type)
{
    struct  note_parm_c note_parm;
    int ret;

    MEMSET_DEBUG(&note_parm, 0X00, sizeof(struct note_parm_c));

    /* 检查凭证类型是否存在 */
    ret=Note_parm_Sel(g_pub_tx.reply , &note_parm , "note_type='%s'" , note_type);
    if(ret==100)
    {
        sprintf(acErrMsg, "无此凭证类型!note_type=[%s]", note_type);
        WRITEMSG
        strcpy(g_pub_tx.reply, "N001");
        return(-1);
    }
    else if(ret)
    {
        sprintf(acErrMsg, "执行Note_parm_Sel错误!ret=[%d]", ret);
        WRITEMSG
        strcpy(g_pub_tx.reply, "N001");
        return(-1);
    }

    /* 检查凭证是否处于使用状态 */
    if(note_parm.use_ind[0] == 'N')
    {
        sprintf(acErrMsg, "凭证处于停用状态!");
        WRITEMSG
        strcpy(g_pub_tx.reply, "N025");
        return(-1);
    }

    return(0);
}

/**********************************************************************
 * 函 数 名:   pub_com_ChkBoeNote
 * 函数功能:   检查凭证起始、终止号码的连续性
 *            冠子头必须相同；终止号码必须大于等于起始号码
 *
 * 作者/时间:  Terry.cui/2003年12月30日
 *
 * 参数:
 *     输入:
 *            char      *note_type      凭证类型
 *            char  *beg_note_no    起始号码
 *        char  *end_note_no    终止号码
 *
 *     输出:  char  *g_pub_tx.reply         相应码
 *
 *   返回值:   0: 成功 -1: 失败
 *
 * 修改历史:   包括修改人、时间、修改内容, 以时间倒序方式
 *
********************************************************************/
int pub_com_ChkBoeNote(
        char *note_type,
        char *beg_note_no,
        char *end_note_no)
{
    struct note_parm_c sNoteParm;
    char    cBegHead[17], cEndHead[17];
    long    lBegNo, lEndNo;

    int ret;

    MEMSET_DEBUG(&sNoteParm, 0x00, sizeof(struct note_parm_c));
    MEMSET_DEBUG(cBegHead, 0x00, sizeof(cBegHead));
    MEMSET_DEBUG(cEndHead, 0x00, sizeof(cEndHead));

    ret=Note_parm_Sel(g_pub_tx.reply, &sNoteParm, "note_type='%s'", note_type);
    if(ret==100)
    {
        sprintf(acErrMsg, "无此凭证类型!note_type=[%s]", note_type);
        WRITEMSG
        strcpy(g_pub_tx.reply, "N001");
        return(-1);
    }
    else if(ret)
    {
        sprintf(acErrMsg, "执行Note_parm_Sel错误!ret=[%d]", ret);
        WRITEMSG
        return(-1);
    }

    strncpy(cBegHead, beg_note_no, sNoteParm.head_len);
    strncpy(cEndHead, end_note_no, sNoteParm.head_len);
    cBegHead[sNoteParm.head_len] = '\0';
    cEndHead[sNoteParm.head_len] = '\0';

    if (strcmp(cBegHead, cEndHead))
    {
        sprintf(acErrMsg, "起始号码和终止号码冠子头不同");
        WRITEMSG
        strcpy(g_pub_tx.reply, "N027");
        return(-1);
    }

    lBegNo = atol(beg_note_no + sNoteParm.head_len);
    lEndNo = atol(end_note_no + sNoteParm.head_len);

    if (lBegNo > lEndNo)
    {
        sprintf(acErrMsg, "起始号码大于终止号码");
        WRITEMSG
        strcpy(g_pub_tx.reply, "N028");
        return(-1);
    }

    return(0);

}

/**********************************************************************
 * 函 数 名:   pub_com_GetNoteMin
 * 函数功能:
 *            查找柜员凭证最小号(柜员凭证销号用)
 *
 * 作者/时间:  Terry.cui/2003年12月30日
 *
 * 参数:
 *     输入:
 *            char      *note_type      凭证类型
 *            char  *note_no    起始号码
 *        char  *tel        操作员
 *
 *     输出:  char  *g_pub_tx.reply         相应码
 *
 *   返回值:   0: 成功 -1: 失败
 *
 * 修改历史:   包括修改人、时间、修改内容, 以时间倒序方式
 *
********************************************************************/

int pub_com_GetNoteMin(
       char *note_type,
       char *note_no,
       char *tel)
{
    struct    note_parm_c   sNoteParm;
    char    cMinNo[17];     /* 保存最小号码 */
    char    cInHead[17];    /*待检查的凭证号的冠子头*/

    int    ret;

    MEMSET_DEBUG(&sNoteParm, 0x00, sizeof(struct note_parm_c));
    MEMSET_DEBUG(cMinNo, 0x00, sizeof(cMinNo));
    MEMSET_DEBUG(cInHead, 0x00, sizeof(cInHead));
        sprintf(acErrMsg, "pub_com_GetNoteMin, note_no=[%s]", note_no);
        WRITEMSG

    /* 根据凭证类型取凭证结构 */
    ret = Note_parm_Sel(g_pub_tx.reply, &sNoteParm, "note_type = '%s'", note_type);
    if(ret== 100)
    {
        sprintf(acErrMsg, "无此凭证类型!note_type=[%s]", note_type);
        WRITEMSG
        strcpy(g_pub_tx.reply, "N001");
        return (-1);
    }else if (ret)
    {
        sprintf(acErrMsg, "执行Note_parm_Sel错误!ret=[%d]", ret);
        WRITEMSG
        return(-1);
    }

    /* 取参数凭证号码冠子头 */
    strncpy(cInHead, note_no, sNoteParm.head_len);
    cInHead[sNoteParm.head_len] = '\0';

    /* 检查note_mst中是否存在以cInHead开头的凭证 */
    ret = sql_count("note_mst", "tel = '%s' and sts = '0' \
                      and note_type = '%s' and substr(beg_note_no, 1, %d) = '%s'", \
                      tel, note_type, sNoteParm.head_len, cInHead);
    if(ret < 0)
    {
        sprintf(acErrMsg, "执行sql_count错, ret=[%d]", ret);
        WRITEMSG
        strcpy(g_pub_tx.reply, "D109");
        return(-1);
    }else if(ret == 0) /* 如果不存在冠子头相同的, 则查找该柜员所有凭证的最小号 */
    {
        ret = sql_min_string("note_mst", "beg_note_no", cMinNo, \
                       sizeof(cMinNo)-1, "tel = '%s' and note_type = '%s' \
                       and sts = '0'", tel, note_type);
        if(ret)
        {
            sprintf(acErrMsg, "执行sql_min_string错误！ret = [%d]", ret);
            WRITEMSG
            strcpy(g_pub_tx.reply, "D110");
            return(-1);
        }
    }else     /* 从note_mst表中查找冠子头相同的最小号 */
    {
        ret = sql_min_string("note_mst", "beg_note_no", cMinNo, sizeof(cMinNo)-1, \
                  "tel = '%s' and note_type = '%s' and substr(beg_note_no, 1, %ld) = '%s' \
                  and sts = '0'", tel, note_type, sNoteParm.head_len, cInHead);
        if(ret)
        {
            sprintf(acErrMsg, "执行sql_min_string错误！");
            WRITEMSG
            strcpy(g_pub_tx.reply, "D110");
            return(-1);
        }
    }
    strcpy(note_no, cMinNo);

    return(0);
}

/**********************************************************************
 * 函 数 名:   pub_com_GetNoteHst
 * 函数功能:
 *            根据交易日期、凭证种类、起始凭证、终止凭证、对方机构、对方柜员、
 *        凭证行为查找柜员凭证明细, 并且返回
 *
 * 作者/时间:  Terry.cui/2003年12月30日
 *
 * 参数:
 *     输入:
 *      long    tx_date     交易日期
 *      char    *note_type  凭证类型
 *      char    *beg_note_no    起始号码
 *      char    *end_note_no    终止号码
 *      char    *tw_br_no   对方机构
 *      char    *tw_tel     对方柜员
 *      char    *note_act   凭证行为
 *
 *     输出:
 *      struct  note_mst_hst_c  *note_mst_hst   柜员凭证明细
 *      char    *g_pub_tx.reply         相应码
 *
 *   返回值:   0: 成功 -1: 失败
 *
 * 修改历史:   包括修改人、时间、修改内容, 以时间倒序方式
 *
********************************************************************/

int pub_com_GetNoteHst(
    long    tx_date,
    char    *note_type,
    char    *beg_note_no,
    char    *end_note_no,
    char    *tw_br_no,
    char    *tw_tel,
    char    *note_act,
    struct  note_mst_hst_c  *note_mst_hst)
{
    int icount = 0;
    int iret;

    vtcp_log("note_type = '%s'  and beg_note_no = '%s' and end_note_no = '%s'  and tx_date = %ld and tw_br_no = '%s' and tw_tel = '%s' and sts_ind = '%s'",
    note_type, beg_note_no, end_note_no, tx_date, tw_br_no, tw_tel, note_act);

    iret = Note_mst_hst_Dec_Sel(g_pub_tx.reply , "note_type = '%s' \
            and beg_note_no = '%s' and end_note_no = '%s' \
            and tx_date = %ld and tw_br_no = '%s' \
            and tw_tel = '%s' and sts_ind = '%s' order by trace_no DESC ",
            note_type, beg_note_no, end_note_no, tx_date, tw_br_no, \
            tw_tel, note_act);

    if (iret)
    {
        sprintf(acErrMsg, "执行    Note_mst_hst_Dec_Sel错, iret=[%d]", iret);
        WRITEMSG
        return(-1);
    }
    while(1)
    {
        iret = Note_mst_hst_Fet_Sel(note_mst_hst , g_pub_tx.reply);
        if (icount > 0 && iret == 100)
        {
            break;
        }
        if (iret == 100)
        {
            sprintf(acErrMsg, "柜员凭证明细不存在note_type = '%s' and beg_note_no = '%s' and end_note_no = '%s' and tx_date = %ld and tw_br_no = '%s' and tw_tel = '%s' and sts_ind = '%s'", note_type, beg_note_no, end_note_no, tx_date, tw_br_no, tw_tel, note_act);
            WRITEMSG
            strcpy(g_pub_tx.reply, "N041");
            return(-1);
        }else if (iret)
        {
            sprintf(acErrMsg, "查询柜员凭证明细错!iret=[%d]", iret);
            WRITEMSG
            return(-1);
        }

        icount++;
        break;
    }

    if (icount > 1)
    {
        sprintf(acErrMsg, "柜员凭证明细存在多条记录");
        WRITEMSG
        strcpy(g_pub_tx.reply, "N042");
        return(-1);
    }
    Note_mst_hst_Clo_Sel ();

    return(0);
}

/**********************************************************************
 * 函 数 名:   pub_com_GetNoteHst1
 * 函数功能:
 *            根据流水号、交易日期、凭证种类、起始凭证、终止凭证、对方机构、
 *              对方柜员、凭证行为查找柜员凭证明细, 并且返回
 *
 * 作者/时间:  jack/2003年12月30日
 *
 * 参数:
 *     输入:
 *      long    tx_date     交易日期
 *      long    trace_no    交易流水
 *      char    *note_type  凭证类型
 *      char    *beg_note_no    起始号码
 *      char    *end_note_no    终止号码
 *      char    *tw_br_no   对方机构
 *      char    *tw_tel     对方柜员
 *      char    *note_act   凭证行为
 *
 *     输出:
 *      struct  note_mst_hst_c  *note_mst_hst   柜员凭证明细
 *      char    *g_pub_tx.reply         相应码
 *
 *   返回值:   0: 成功 -1: 失败
 *
 * 修改历史:   包括修改人、时间、修改内容, 以时间倒序方式
 *
********************************************************************/

int pub_com_GetNoteHst1(
    long    trace_no,
    long    tx_date,
    char    *note_type,
    char    *beg_note_no,
    char    *end_note_no,
    char    *tw_br_no,
    char    *tw_tel,
    char    *note_act,
    struct  note_mst_hst_c  *note_mst_hst)
{
    int icount = 0;
    int iret;

    iret = Note_mst_hst_Dec_Sel(g_pub_tx.reply , "note_type = '%s' \
            and beg_note_no = '%s' and end_note_no = '%s' \
            and tx_date = %ld and tw_br_no = '%s' \
            and tw_tel = '%s' and sts_ind = '%s' and trace_no=%ld",
            note_type, beg_note_no, end_note_no, tx_date, tw_br_no, \
            tw_tel, note_act, trace_no);

    if (iret)
    {
        sprintf(acErrMsg, "执行    Note_mst_hst_Dec_Sel错, iret=[%d]", iret);
        WRITEMSG
        return(-1);
    }
    while(1)
    {
        iret = Note_mst_hst_Fet_Sel(note_mst_hst , g_pub_tx.reply);
        if (icount > 0 && iret == 100)
        {
            break;
        }
        if (iret == 100)
        {
            sprintf(acErrMsg, "柜员凭证明细不存在note_type = '%s' and beg_note_no = '%s' and end_note_no = '%s' and tx_date = %ld and tw_br_no = '%s' and tw_tel = '%s' and sts_ind = '%s' and trace_no=%ld", note_type, beg_note_no, end_note_no, tx_date, tw_br_no, tw_tel, note_act, trace_no);
            WRITEMSG
            strcpy(g_pub_tx.reply, "N041");
            return(-1);
        }else if (iret)
        {
            sprintf(acErrMsg, "查询柜员凭证明细错!iret=[%d]", iret);
            WRITEMSG
            return(-1);
        }

        icount++;
    }

    if (icount > 1)
    {
        sprintf(acErrMsg, "柜员凭证明细存在多条记录");
        WRITEMSG
        strcpy(g_pub_tx.reply, "N042");
        return(-1);
    }
    Note_mst_hst_Clo_Sel ();

    return(0);
}

/**********************************************************************
 * 函 数 名:   pub_com_ChkBrRel
 * 函数功能:
 *            根据关系类型、机构代码、上级机构号查找交易机构关系表
 *
 * 作者/时间:  jack/2005年3月26日
 *
 * 参数:
 *     输入:
 *      int *rel_type   关系类型
 *      char    *br_no      机构代码
 *      char    *up_br_no   上级机构号
 *
 *     输出:
 *      char    *g_pub_tx.reply         相应码
 *
 *   返回值:   0: 成功  1/2/3/4/5: 失败
 *
 * 修改历史:   包括修改人、时间、修改内容, 以时间倒序方式
 *
********************************************************************/

int pub_com_ChkBrRel(int rel_type, char *br_no, char *up_br_no)
{

    struct  com_tx_br_rel_c sComTxBrRel;    /* 交易机构关系表 */
    int ret;

    MEMSET_DEBUG(&sComTxBrRel, 0x00, sizeof(struct com_tx_br_rel_c));

    sprintf(acErrMsg, "CGH [%s], up [%s] rel_type=[%d]",
                    br_no, up_br_no, rel_type);
    WRITEMSG

    ret = Com_tx_br_rel_Sel(g_pub_tx.reply, &sComTxBrRel, "rel_type=%d \
                and br_no='%s' and up_br_no='%s'", rel_type, \
                br_no, up_br_no);
    if(ret == 100)
    {
        if(rel_type == 1)     /* 现金调拨关系 */
            return 2;
        else if(rel_type == 2)    /* 凭证调拨关系 */
            return 3;
        else if(rel_type == 3)    /* 同城票据传递 */
            return 4;
        else if(rel_type == 4)    /* 同城内部清算 */
            return 5;
        else if(rel_type == 5)    /* 银行汇票签发 */
            return 6;
        else                        /* 关系不存在 */
            return 1;
    }
    else if(ret)
    {
        sprintf(acErrMsg, "执行Com_tx_br_rel_Sel错误!ret=[%d]", ret);
        WRITEMSG
        return 1;
    }

    return(0);
}

/**********************************************************************
 * 函 数 名:   pub_com_ChkNtExt
 * 函数功能:
 *            使用凭证检查:检查柜员是否有此凭证并可以使用
 *
 * 作者/时间:  jack/2005年6月20日
 *
 * 参数:
 *     输入:
 *      char    *beg_note_no    起始凭证号码
 *      char    *end_note_no    终止凭证号码
 *      char    *note_type      凭证类型
 *      char    *br_no          机构代码
 *      char    *tel            柜员号
 *
 *   返回值:   0: 成功  1/2/3/4/5: 失败
 *
 * 修改历史:   包括修改人、时间、修改内容, 以时间倒序方式
 *
********************************************************************/

int pub_com_ChkNtExt(char *beg_note_no, char *end_note_no, char *note_type, char *br_no, char *tel)
{
    char cCond[200];
    char beg_note_no_tmp[17], end_note_no_tmp[17];
    char tmp_str[10];
    int note_no_len;
    struct note_parm_c sNoteParm;
    struct note_mst_c sNoteMst;

    MEMSET_DEBUG(&sNoteParm, 0x00, sizeof(struct note_parm_c));
    MEMSET_DEBUG(&sNoteMst, 0x00, sizeof(struct note_mst_c));

    g_reply_int = Note_parm_Sel(g_pub_tx.reply, &sNoteParm, "note_type='%s'",
        note_type);
    if(g_reply_int == 100)
    {
        strcpy(g_pub_tx.reply, "N001");
        sprintf(acErrMsg, "凭证存在检查: 凭证类型不存在[%s]", note_type);
        WRITEMSG
        return(-1);
    }
    else if(g_reply_int)
    {
        strcpy(g_pub_tx.reply, "D103");
        sprintf(acErrMsg, "凭证存在检查: 查询凭证参数出错[%d]", g_reply_int);
        WRITEMSG
        return(-1);
    }

    note_no_len = 16 - sNoteParm.head_len;
    MEMSET_DEBUG(beg_note_no_tmp, 0x00, sizeof(beg_note_no_tmp));
    if(strlen(beg_note_no) >= note_no_len)
        strncpy(beg_note_no_tmp, beg_note_no + (strlen(beg_note_no) - note_no_len), note_no_len);
    else if(strlen(beg_note_no) < note_no_len)
    {
        sprintf(tmp_str, "%%0%ldld", note_no_len);
        sprintf(beg_note_no_tmp, tmp_str, atol(beg_note_no));
    }
    MEMSET_DEBUG(end_note_no_tmp, 0x00, sizeof(end_note_no_tmp));
    if(strlen(end_note_no) >= note_no_len)
        strncpy(end_note_no_tmp, end_note_no + (strlen(end_note_no) - note_no_len), note_no_len);
    else if(strlen(end_note_no) < note_no_len)
    {
        sprintf(tmp_str, "%%0%ldld", note_no_len);
        sprintf(end_note_no_tmp, tmp_str, atol(end_note_no));
    }
    sprintf(cCond, "note_type='%s' and br_no='%s' and tel='%s' and \
                    substr(beg_note_no, %d, 16-%d+1)<='%s' and substr(end_note_no, %d, 16-%d+1)>='%s'", \
                    note_type, br_no, tel, sNoteParm.head_len+1, sNoteParm.head_len+1, beg_note_no_tmp, sNoteParm.head_len+1, sNoteParm.head_len+1, end_note_no_tmp);
    strcpy(acErrMsg, cCond);
    WRITEMSG

    g_reply_int = Note_mst_Sel(g_pub_tx.reply, &sNoteMst, cCond);
    if(g_reply_int == 100)
    {
        strcpy(g_pub_tx.reply, "N007");
        sprintf(acErrMsg, "凭证存在检查: 柜员无此凭证.类型[%s]起始号码[%s]终止号码[%s]",
            note_type, beg_note_no_tmp, end_note_no_tmp);
        WRITEMSG
        return(-1);
    }
    else if(g_reply_int)
    {
        strcpy(g_pub_tx.reply, "D103");
        sprintf(acErrMsg, "凭证存在检查: 查询柜员凭证台帐出错[%d]", g_reply_int);
        WRITEMSG
        return(-1);
    }

    if(sNoteMst.sts[0] != '0')
    {
        if(sNoteMst.sts[0] == '1')
        {
            strcpy(g_pub_tx.reply, "N081");
            sprintf(acErrMsg, "凭证存在检查: 凭证已挂失。类型[%s]起始号码[%s]终止号码[%s]",
                note_type, beg_note_no_tmp, end_note_no_tmp);
            WRITEMSG
            return(-1);
        }
        else if(sNoteMst.sts[0] == '3' || sNoteMst.sts[0] == '6' || sNoteMst.sts[0] == '*')
        {
            strcpy(g_pub_tx.reply, "N083");
            sprintf(acErrMsg, "凭证存在检查: 凭证已作废。类型[%s]起始号码[%s]终止号码[%s]",
                note_type, beg_note_no_tmp, end_note_no_tmp);
            WRITEMSG
            return(-1);
        }
        else if(sNoteMst.sts[0] == '4' || sNoteMst.sts[0] == '5')
        {
            strcpy(g_pub_tx.reply, "N084");
            sprintf(acErrMsg, "凭证存在检查: 凭证已使用。类型[%s]起始号码[%s]终止号码[%s]",
                note_type, beg_note_no_tmp, end_note_no_tmp);
            WRITEMSG
            return(-1);
        }
        else
        {
            strcpy(g_pub_tx.reply, "N022");
            sprintf(acErrMsg, "凭证存在检查: 凭证状态不正常。类型[%s]起始号码[%s]终止号码[%s]",
                note_type, beg_note_no_tmp, end_note_no_tmp);
            WRITEMSG
            return(-1);
        }
    }

    sprintf(acErrMsg, "凭证存在检查通过, 柜员有此凭证");
    WRITEMSG

    return(0);
}
