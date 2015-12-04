/*******************************************************
*�������ƣ�spF254.c
*�������ܣ�ʵ�������¹�Աƾ֤
*����ˣ�  CHENGBO
*���ʱ�䣺2011-11-28
*
*�޸ļ�¼:
*      1���޸�ʱ��
*      2���޸���
*      3���޸�����
*
* insert into tx_def values('6254','��������Աƾ֤','10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000','1','6','2','3');
* insert into tx_flow_def values('6254','0','F254','#$');
* insert into tx_sub_def values('F254','��������Աƾ֤','spF254','0','0');
************************************************************/
#include <stdio.h>
#include "public.h"
#include "note_mst_c.h"
#include "note_mst_hst_c.h"

spF254()
{
    int iRet = 0;
    int i = 0;       /*ѭ����������*/
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

    get_zd_long("0550", &lPz_cnt);    /*������ƾ֤������*/
    get_zd_data("0590", cBeg_no);     /*ƾ֤��ʼ����*/
    get_zd_data("0890", cNote_type);  /*ƾ֤����*/

    /*���ƾ֤�����Ƿ����*/
    iRet = pub_com_ChkNoteUse(cNote_type);
    if( iRet )
    {
        sprintf(acErrMsg, "ƾ֤����ͣ��״̬!");
        strcpy( g_pub_tx.reply, "HV01");
        WRITEMSG
        goto ErrExit;
    }

    iRet = sql_sum_long( "note_mst", "cnt", &lRnum, "tel = '%s' \
                           and note_type = '%s' and sts = '0'", g_pub_tx.tel, cNote_type);
    if ( lRnum < lPz_cnt )
    {
        sprintf(acErrMsg, "��������[%d]ƾ֤��[%d]", lPz_cnt, lRnum );
        WRITEMSG
        sprintf(acErrMsg, "��Ա����ƾ֤����!!!" );
        WRITEMSG
        strcpy( g_pub_tx.reply, "SN34" );
        goto ErrExit;
    }
    iRet = pub_com_ChkNoteMin(cNote_type, cBeg_no, g_pub_tx.tel);
    if ( iRet == -1 )
    {
        sprintf( acErrMsg, "ƾ֤��С�ż�����!" );
        WRITEMSG
        goto ErrExit;
    }
    else if ( iRet == 2 )  /*������С����*/
    {
        sprintf( acErrMsg, "ƾ֤�Ų�����С��!" );
        WRITEMSG
        strcpy( g_pub_tx.reply, "N014" );
        goto ErrExit;
    }
    /*ѭ���¹�Աƾ֤*/
    i = 0;
    memcpy(g_pub_tx.beg_note_no, cBeg_no, sizeof(g_pub_tx.beg_note_no) - 1);
    memcpy(g_pub_tx.note_type, cNote_type, sizeof(g_pub_tx.note_type) - 1);
    for(i = 0; i < lPz_cnt; i++)
    {
        /*��Աƾ֤ʹ��*/
        iRet = pub_com_NoteUse(0, 0, cNote_type, g_pub_tx.beg_note_no, g_pub_tx.beg_note_no, g_pub_tx.tel);
        if( iRet )
        {
            sprintf(acErrMsg, "��Աʹ��ƾ֤����!");
            WRITEMSG
            strcpy(g_pub_tx.reply, "N007");
            goto ErrExit;
        }

        iRet = pub_com_GetNoteMin(g_pub_tx.note_type, g_pub_tx.beg_note_no, g_pub_tx.tel);
        if(iRet)
        {
            sprintf(acErrMsg, "��ѯ��Ա��С����ƾ֤����");
            WRITEMSG
            strcpy( g_pub_tx.reply, "N013" );
            goto ErrExit;
        }
    }
    /******
    strcpy(g_pub_tx.brf,"��������Աƾ֤");
    iRet=pub_ins_trace_log();
    if(iRet)
    {
        vtcp_log("%s,%d,�Ǽ���ˮ����!",__FILE__,__LINE__);
        sprintf(acErrMsg, "�Ǽǹ�Ա��ˮ����");
        WRITEMSG
        goto ErrExit;
    }
    ******/
OkExit:
    strcpy(g_pub_tx.reply, "0000");
    sprintf(acErrMsg, "��������Աƾ֤�ɹ���[%s]", g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 0;
ErrExit:
    if( strcmp(g_pub_tx.reply, "0000") == 0 || strcmp(g_pub_tx.reply, "") == 0)
    {
        strcpy(g_pub_tx.reply, "N019");
    }
    sprintf(acErrMsg, "��������Աƾ֤ʧ�ܣ�[%s]", g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 1;
}

