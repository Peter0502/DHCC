/*************************************************************
*��������:  spF253.c
*��������:  ʵ������Աƾ֤���Ǳ���
*��    ��:  chengbo
*���ʱ��:  2011��11��28��
*�޸ļ�¼:
*     1.�޸�ʱ��
*     2.�޸���
*     3.�޸�����
*insert into tx_def values('6253','��������Աƾ֤','10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000','1','6','2','3');
*insert into tx_flow_def values('6253','0','F253','#$');
*insert into tx_sub_def values('F253','��������Աƾ֤','spF253','0','0');
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

    /*���ƾ֤�����Ƿ����*/
    ret = pub_com_ChkNoteUse(cNote_type);
    if( ret )
    {
        sprintf(acErrMsg, "ƾ֤����ͣ��״̬!");
        strcpy( g_pub_tx.reply, "HV01");
        WRITEMSG
        goto ErrExit;
    }
    /*��Աƾ֤ʹ��*/
    ret = pub_com_NoteUse(0, 0, cNote_type, cNote_no, cNote_no, g_pub_tx.tel);
    if( ret )
    {
        sprintf(acErrMsg, "��Աʹ��ƾ֤����!");
        WRITEMSG
        strcpy(g_pub_tx.reply, "N007");
        goto ErrExit;
    }
    /***Ϊ����ƾ֤��׼������trace_log 2010-6-30 20:02:55 ****
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
    strcpy(sTrace_log.brf,"����Աƾ֤");
    strcpy(sTrace_log.tel,g_pub_tx.tel);
    strcpy(sTrace_log.sts,"0");
    sTrace_log.ejfno=g_pub_tx.ejfno;
    ret=Trace_log_Ins(sTrace_log,g_pub_tx.reply);
    if ( ret )
    {
        Trace_log_Debug(&sTrace_log);
        sprintf(acErrMsg,"�Ǽǽ�����ˮ����[%d]!",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"P126");
        goto ErrExit;
    }
    ********/
OkExit:
    strcpy(g_pub_tx.reply, "0000");
    sprintf(acErrMsg, "�ɹ�![%s]", g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 0;
ErrExit:
    sprintf(acErrMsg, "ʧ��![%s]", g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 1;
}
