/***************************************************************
* �� �� ��: spD255.c
* ��������: ��ͬ������(�ϳ�����)
*
* ��    ��: jack
* �������: 2004��4��02��
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

spD255()
{
    int ret;
    struct com_parm_c com_parm_c;

    memset(&com_parm_c, 0x0, sizeof(struct com_parm_c));

	/*** ��ʼ��ȫ�ֱ��� ***/
	if(pub_base_sysinit())
	{
		strcpy(acErrMsg,"��ʼ��ȫ�ֱ�������!");
		WRITEMSG
		goto ErrExit;
	}

    g_pub_tx.tx_amt1 =0.00;
    /* ȡ�ϳ�������˽�� */
    ret=Com_parm_Sel(g_pub_tx.reply,&com_parm_c,"parm_code='GZJE' and \
        parm_seqn=1");
    if (ret != 0)
    {
        sprintf(acErrMsg,"��ѯ�������������!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }
    g_pub_tx.tx_amt1 = atof(com_parm_c.val);
    sprintf(acErrMsg,"��ͬ�������ϳ����˽��=[%d]",g_pub_tx.tx_amt1);
    WRITEMSG

    if (pub_base_CompDblVal(g_pub_tx.tx_amt1,0.00) == 0)
    {
        goto OkExit1;
    }

    ret=Com_parm_Sel(g_pub_tx.reply,&com_parm_c,"parm_code='TCKM' and \
        parm_seqn=2");
    if (ret != 0)
    {
        sprintf(acErrMsg,"��ѯ�������������!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    /* ���� ��408�� */
    set_zd_data("1211",com_parm_c.val);
    set_zd_double("1218",g_pub_tx.tx_amt1);
    set_zd_data("1215","2");
    set_zd_data("121A","��ͬ���������");

    memset(&com_parm_c,0x0,sizeof(struct com_parm_c));
    ret=Com_parm_Sel(g_pub_tx.reply,&com_parm_c,"parm_code='TCKM' and \
        parm_seqn=4");
    if (ret != 0)
    {
        sprintf(acErrMsg,"��ѯ�������������!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    /* ��� ���Ŀ408���� */
    set_zd_data("1201",com_parm_c.val);
    set_zd_double("1208",g_pub_tx.tx_amt1);
    set_zd_data("1205","2");
    set_zd_data("120A","��ͬ���������");
    
	/*** �Ǽǽ�����ˮ 
	if( pub_ins_trace_log() )
	{
		sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
		WRITEMSG
		goto ErrExit;
	}***/
	
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"��ͬ�����㴦��ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
OkExit1:
	strcpy(g_pub_tx.reply,"0001");
	sprintf(acErrMsg,"��ͬ�����㴦��ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"��ͬ�����㴦��ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

