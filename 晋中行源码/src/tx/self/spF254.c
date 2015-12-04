/*******************************************************
*函数名称：spF254.c
*函数功能：实现批量下柜员凭证
*完成人：  CHENGBO
*完成时间：2011-11-28
*
*修改记录:
*      1、修改时间
*      2、修改人
*      3、修改内容
*
* insert into tx_def values('6254','批量销柜员凭证','10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000','1','6','2','3');
* insert into tx_flow_def values('6254','0','F254','#$');
* insert into tx_sub_def values('F254','批量销柜员凭证','spF254','0','0');
************************************************************/
#include <stdio.h>
#include "public.h"
#include "note_mst_c.h"
#include "note_mst_hst_c.h"

spF254()
{
    int iRet = 0;
    int i = 0;       /*循环控制条数*/
    long lPz_cnt = 0;
    long lRnum = 0;
    char cBeg_no[17];
    char cNote_type[4];
    struct note_mst_c sNote_mst;
    struct note_mst_hst_c sNote_mst_hst;

    memset(&sNote_mst, 0x00, sizeof(struct note_mst_c));
    memset(&sNote_mst_hst, 0x00, sizeof(struct note_mst_hst_c));
    memset(cBeg_no, 0x00, sizeof(cBeg_no));
    memset(cNote_type, 0x00, sizeof(cNote_type));

    pub_base_sysinit();

    get_zd_long("0550", &lPz_cnt);    /*批量销凭证的张数*/
    get_zd_data("0590", cBeg_no);     /*凭证起始号码*/
    get_zd_data("0890", cNote_type);  /*凭证种类*/

    /*检查凭证种类是否可用*/
    iRet = pub_com_ChkNoteUse(cNote_type);
    if( iRet )
    {
        sprintf(acErrMsg, "凭证处于停用状态!");
        strcpy( g_pub_tx.reply, "HV01");
        WRITEMSG
        goto ErrExit;
    }

    iRet = sql_sum_long( "note_mst", "cnt", &lRnum, "tel = '%s' \
                           and note_type = '%s' and sts = '0'", g_pub_tx.tel, cNote_type);
    if ( lRnum < lPz_cnt )
    {
        sprintf(acErrMsg, "开户户数[%d]凭证数[%d]", lPz_cnt, lRnum );
        WRITEMSG
        sprintf(acErrMsg, "柜员可用凭证不足!!!" );
        WRITEMSG
        strcpy( g_pub_tx.reply, "SN34" );
        goto ErrExit;
    }
    iRet = pub_com_ChkNoteMin(cNote_type, cBeg_no, g_pub_tx.tel);
    if ( iRet == -1 )
    {
        sprintf( acErrMsg, "凭证最小号检查出错!" );
        WRITEMSG
        goto ErrExit;
    }
    else if ( iRet == 2 )  /*不是最小号码*/
    {
        sprintf( acErrMsg, "凭证号不是最小号!" );
        WRITEMSG
        strcpy( g_pub_tx.reply, "N014" );
        goto ErrExit;
    }
    /*循环下柜员凭证*/
    i = 0;
    memcpy(g_pub_tx.beg_note_no, cBeg_no, sizeof(g_pub_tx.beg_note_no) - 1);
    memcpy(g_pub_tx.note_type, cNote_type, sizeof(g_pub_tx.note_type) - 1);
    for(i = 0; i < lPz_cnt; i++)
    {
        /*柜员凭证使用*/
        iRet = pub_com_NoteUse(0, 0, cNote_type, g_pub_tx.beg_note_no, g_pub_tx.beg_note_no, g_pub_tx.tel);
        if( iRet )
        {
            sprintf(acErrMsg, "柜员使用凭证错误!");
            WRITEMSG
            strcpy(g_pub_tx.reply, "N007");
            goto ErrExit;
        }

        iRet = pub_com_GetNoteMin(g_pub_tx.note_type, g_pub_tx.beg_note_no, g_pub_tx.tel);
        if(iRet)
        {
            sprintf(acErrMsg, "查询柜员最小可用凭证出错");
            WRITEMSG
            strcpy( g_pub_tx.reply, "N013" );
            goto ErrExit;
        }
    }
    /******
    strcpy(g_pub_tx.brf,"批量销柜员凭证");
    iRet=pub_ins_trace_log();
    if(iRet)
    {
        vtcp_log("%s,%d,登记流水错误!",__FILE__,__LINE__);
        sprintf(acErrMsg, "登记柜员流水出错");
        WRITEMSG
        goto ErrExit;
    }
    ******/
OkExit:
    strcpy(g_pub_tx.reply, "0000");
    sprintf(acErrMsg, "批量销柜员凭证成功！[%s]", g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 0;
ErrExit:
    if( strcmp(g_pub_tx.reply, "0000") == 0 || strcmp(g_pub_tx.reply, "") == 0)
    {
        strcpy(g_pub_tx.reply, "N019");
    }
    sprintf(acErrMsg, "批量销柜员凭证失败！[%s]", g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 1;
}

