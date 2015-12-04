/*************************************************************
*函数名称:  spF253.c
*函数功能:  实现销柜员凭证并记表外
*作    者:  chengbo
*完成时间:  2011年11月28日
*修改记录:
*     1.修改时间
*     2.修改人
*     3.修改内容
*insert into tx_def values('6253','单笔销柜员凭证','10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000','1','6','2','3');
*insert into tx_flow_def values('6253','0','F253','#$');
*insert into tx_sub_def values('F253','单笔销柜员凭证','spF253','0','0');
******************************************************************/
#include "stdio.h"
#define  EXTERN
#include "public.h"
#include "trace_log_c.h"
#define  EXTERN

spF253()
{
    int    ret;
    char   cNote_type[4];
    char   cNote_no[17];

    struct trace_log_c sTrace_log;

    memset(&sTrace_log, 0x00, sizeof(struct trace_log_c));
    memset(cNote_type, 0x0, sizeof(cNote_type));
    memset(cNote_no, 0x0, sizeof(cNote_no));

    pub_base_sysinit();
    get_zd_data("0890", cNote_type);
    get_zd_data("0580", cNote_no);

    /*检查凭证种类是否可用*/
    ret = pub_com_ChkNoteUse(cNote_type);
    if( ret )
    {
        sprintf(acErrMsg, "凭证处于停用状态!");
        strcpy( g_pub_tx.reply, "HV01");
        WRITEMSG
        goto ErrExit;
    }
    /*柜员凭证使用*/
    ret = pub_com_NoteUse(0, 0, cNote_type, cNote_no, cNote_no, g_pub_tx.tel);
    if( ret )
    {
        sprintf(acErrMsg, "柜员使用凭证错误!");
        WRITEMSG
        strcpy(g_pub_tx.reply, "N007");
        goto ErrExit;
    }
    /***为销掉凭证做准备插入trace_log 2010-6-30 20:02:55 ****
    sTrace_log.trace_no=g_pub_tx.trace_no;
    sTrace_log.trace_cnt=g_pub_tx.trace_cnt;
    sTrace_log.tx_date=g_pub_tx.tx_date;
    sTrace_log.tx_time=g_pub_tx.tx_time;
    strcpy(sTrace_log.tx_br_no,g_pub_tx.tx_br_no);
    strcpy(sTrace_log.opn_br_no,g_pub_tx.opn_br_no);
    strcpy(sTrace_log.tx_code,g_pub_tx.tx_code);
    strcpy(sTrace_log.sub_tx_code,g_pub_tx.sub_tx_code);
    strcpy(sTrace_log.cur_no,"01");
    strcpy(sTrace_log.prdt_no,g_pub_tx.prdt_code);
    strcpy(sTrace_log.note_type,cNote_type);
    strcpy(sTrace_log.note_no,cNote_no);
    strcpy(sTrace_log.add_ind,"0");
    sTrace_log.amt=1.00;
    strcpy(sTrace_log.brf,"销柜员凭证");
    strcpy(sTrace_log.tel,g_pub_tx.tel);
    strcpy(sTrace_log.sts,"0");
    sTrace_log.ejfno=g_pub_tx.ejfno;
    ret=Trace_log_Ins(sTrace_log,g_pub_tx.reply);
    if ( ret )
    {
        Trace_log_Debug(&sTrace_log);
        sprintf(acErrMsg,"登记交易流水错误[%d]!",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"P126");
        goto ErrExit;
    }
    ********/
OkExit:
    strcpy(g_pub_tx.reply, "0000");
    sprintf(acErrMsg, "成功![%s]", g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 0;
ErrExit:
    sprintf(acErrMsg, "失败![%s]", g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 1;
}
