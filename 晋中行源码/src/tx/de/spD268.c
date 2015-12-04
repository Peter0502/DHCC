/***************************************************************
* �� �� ��: spD268.c
* ��������: ��ͬ�������������
*
* ��    ��: jack
* �������: 2004��6��17��
*
* �޸ļ�¼��
*    ��    ��:
*    �� �� ��:
*    �޸�����:
*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "com_parm_c.h"

spD268()
{
    int ret;
    char flag[2];
    struct com_parm_c com_parm_c;

    memset(&com_parm_c, 0x0, sizeof(struct com_parm_c));

	/*** ��ʼ��ȫ�ֱ��� ***/
	if(pub_base_sysinit())
	{
		strcpy(acErrMsg,"��ʼ��ȫ�ֱ�������!");
		WRITEMSG
		goto ErrExit;
	}

    get_zd_double("0390",&g_pub_tx.tx_amt1);
    get_zd_data("0670",flag);


    ret=Com_parm_Sel(g_pub_tx.reply,&com_parm_c,"parm_code='TCKM' and \
        parm_seqn=2");
    if (ret != 0)
    {
        sprintf(acErrMsg,"��ѯ�������������!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    if (flag[0] == '1')
    {
        /* ��� ��408�� */
        set_zd_data("1201",com_parm_c.val);
        set_zd_double("1208",g_pub_tx.tx_amt1);
        set_zd_data("1205","2");
        set_zd_data("120A","��ͬ���������");
    }
    else
    {
        /* ���� ��408�� */
        set_zd_data("1211",com_parm_c.val);
        set_zd_double("1218",g_pub_tx.tx_amt1);
        set_zd_data("1215","2");
        set_zd_data("121A","��ͬ���������");
    }

    memset(&com_parm_c,0x0,sizeof(struct com_parm_c));
    ret=Com_parm_Sel(g_pub_tx.reply,&com_parm_c,"parm_code='TCKM' and \
        parm_seqn=5");
    if (ret != 0)
    {
        sprintf(acErrMsg,"��ѯ�������������!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }
    if (flag[0] == '1')
    {
        /* ��� ��114���˿�Ŀ */
        set_zd_data("1211",com_parm_c.val);
        set_zd_double("1218",g_pub_tx.tx_amt1);
        set_zd_data("1215","2");
        set_zd_data("121A","��ͬ���������");
    }
    else
    {
		/* ���� ��114��Ŀ�ʻ� */
        set_zd_data("1201",com_parm_c.val);
        set_zd_double("1208",g_pub_tx.tx_amt1);
        set_zd_data("1205","2");
        set_zd_data("120A","��ͬ���������");
    }
    
	/*** �Ǽǽ�����ˮ 
	if( pub_ins_trace_log() )
	{
		sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
		WRITEMSG
		goto ErrExit;
	}***/
	
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"����������ʳɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"�����������ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

