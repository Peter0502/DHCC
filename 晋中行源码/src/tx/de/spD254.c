/***************************************************************
* �� �� ��: spD254.c
* ��������: ��ͬ������(��������)
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

spD254()
{
    int ret;
    int no;
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

    g_pub_tx.tx_amt1 =0.00;
    /* ͳ�Ʊ�����ͬ�������� */
    ret = sql_sum_double("mo_city","bill_amt",&g_pub_tx.tx_amt1,
          "bill_type='4' and in_out_ind='1' and no=0");
    if (ret != 0)
    {
        sprintf(acErrMsg,"ͳ�Ʊ�����ͬ��������������!![%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    if (pub_base_CompDblVal(g_pub_tx.tx_amt1, 0.00) == 0)
    {
        goto OkExit1;
    }

    /* �޸ı����εĳ��κ��� */
    ret = sql_max_int("mo_city","no","bill_type='4' and in_out_ind='1' and \
          tx_date=%ld", g_pub_tx.tx_date);
    if (ret < 0)
    {
        sprintf(acErrMsg,"ȡ�������ڵĳ������ֵ����!![%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    no= ret +1;

    /* �޸ı����εĳ��κ��� */
    ret = sql_execute("update mo_city set no =%d where bill_type='4' and \
          in_out_ind='1' and no = '0'",no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"�޸ı�����Ʊ�ݵĳ��κ������!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    /* �޸��ϳ����˽�� */
    memset(&com_parm_c, 0x0, sizeof(struct com_parm_c));
    sprintf(com_parm_c.val,"%lf",g_pub_tx.tx_amt1);
    ret = sql_execute("update com_parm set val='%s' where parm_code='GZJE' \
          and parm_seqn=1",com_parm_c.val);
    if (ret != 0)
    {
        sprintf(acErrMsg,"�޸��ϳ����˽�����!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    /* �����ι��� */
    ret=Com_parm_Sel(g_pub_tx.reply,&com_parm_c,"parm_code='TCKM' and \
        parm_seqn=2");
    if (ret != 0)
    {
        sprintf(acErrMsg,"��ѯ�������������!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    /* ��� ��408�� */
    set_zd_data("1201",com_parm_c.val);
    set_zd_double("1208",g_pub_tx.tx_amt1);
    set_zd_data("1205","2");
    set_zd_data("120A","��ͬ���������");

    memset(&com_parm_c,0x0,sizeof(struct com_parm_c));
    ret=Com_parm_Sel(g_pub_tx.reply,&com_parm_c,"parm_code='TCKM' and \
        parm_seqn=4");
    if (ret != 0)
    {
        sprintf(acErrMsg,"��ѯ�������������!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    /* ��� ����Ŀ408���� */
    set_zd_data("1211",com_parm_c.val);
    set_zd_double("1218",g_pub_tx.tx_amt1);
    set_zd_data("1215","2");
    set_zd_data("121A","��ͬ���������");
    
	/*** �Ǽǽ�����ˮ 
	if( pub_ins_trace_log() )
	{
		sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
		WRITEMSG
		goto ErrExit;
	}***/
	
OkExit:
	strcpy(g_pub_tx.reply,"0001");
	sprintf(acErrMsg,"��ͬ�����㴦��ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
OkExit1:
	strcpy(g_pub_tx.reply,"0000");
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

