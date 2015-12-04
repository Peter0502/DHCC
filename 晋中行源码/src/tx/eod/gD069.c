/***************************************************************
* �� �� ��: gD069.c
* ��������: ������������(����)
*
* ��    ��: mike
* �������: 2003��4��04��
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

gD069()
{
    int ret;
	int flag;
	double amt1,amt2;
    struct com_parm_c com_parm_c;

    memset(&com_parm_c, 0x0, sizeof(struct com_parm_c));

    ret=sql_begin(); /*������*/
    if(ret != 0)
    {
        sprintf( acErrMsg, "������ʧ��!!!" );
        WRITEMSG
        goto ErrExit;
    }

	/*** ��ʼ��ȫ�ֱ��� ***/
	if(pub_base_sysinit())
	{
		strcpy(acErrMsg,"��ʼ��ȫ�ֱ�������!");
		WRITEMSG
		goto ErrExit;
	}

    /* ͳ�ƽ�� */
	/* ����跽 */
    ret = sql_sum_double("mo_city","bill_amt",&amt1,"in_out_ind = '2' and \
			cd_ind = '1' and (sts='A' or sts='H')");
    if (ret != 0)
    {
        sprintf(acErrMsg,"ͳ��ͬ��������˽����� [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    sprintf(acErrMsg,"����跽���=[%lf]",amt1);
    WRITEMSG

	/* ������� */
    ret = sql_sum_double("mo_city","bill_amt",&amt2,"in_out_ind = '2' and \
			cd_ind = '2' and (sts='A' or sts='H')");
    if (ret != 0)
    {
        sprintf(acErrMsg,"ͳ��ͬ��������˽����� [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    sprintf(acErrMsg,"����������=[%lf]",amt1);
    WRITEMSG

	if (pub_base_CompDblVal(amt2,amt1) > 0)
	{
		g_pub_tx.tx_amt1 = amt2 - amt1;
		flag = 2;
	}
	else if (pub_base_CompDblVal(amt2,amt1) < 0)
	{
		g_pub_tx.tx_amt1 = amt2 - amt1;
		flag = 1;
	}
	else
	{
		goto OkExit;
	}

    sprintf(acErrMsg,"g_pub_tx.tx_amt=[%lf]",g_pub_tx.tx_amt1);
    WRITEMSG

    ret=Com_parm_Sel(g_pub_tx.reply,&com_parm_c,"parm_code='TCKM' and \
        parm_seqn=3");
    if (ret != 0)
    {
        sprintf(acErrMsg,"��ѯ�������������!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

	if (flag == 1)
	{
    	/* ���� ��408�� */
    	set_zd_data("0210","01");
    	set_zd_data("0152",com_parm_c.val);
    	set_zd_double("0390",g_pub_tx.tx_amt1);
	}
	else
	{
    	/* ���� ��408�� */
    	set_zd_data("0210","01");
    	set_zd_data("0155",com_parm_c.val);
    	set_zd_double("0390",g_pub_tx.tx_amt1);
	}

    memset(&com_parm_c,0x0,sizeof(struct com_parm_c));
    ret=Com_parm_Sel(g_pub_tx.reply,&com_parm_c,"parm_code='TCKM' and \
        parm_seqn=6");
    if (ret != 0)
    {
        sprintf(acErrMsg,"��ѯ�������������!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

	if (flag == 1)
	{
        /* ���� ��408���˿�Ŀ */
    	set_zd_data("0155",com_parm_c.val);
	}
	else
	{
        /* ���� ��408���˿�Ŀ */
    	set_zd_data("0152",com_parm_c.val);
	}

    ret = pubf_acct_proc("g066");
    if( ret )
    {
        sprintf(acErrMsg,"���ô�ȡ������ʧ��!");
        WRITEMSG
        goto ErrExit;
    }

    
	/*** �Ǽǽ�����ˮ 
	if( pub_ins_trace_log() )
	{
		sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
		WRITEMSG
		goto ErrExit;
	}***/
	
OkExit:
    sql_commit();   /*--�ύ����--*/
    strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"�����������ʳɹ�![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120",g_pub_tx.reply);
    return 0;
ErrExit:
    if (strcmp(g_pub_tx.reply,"0000") == 0 || g_pub_tx.reply[0]=='\0')
    {
        strcpy(g_pub_tx.reply,"G009");
    }
    sql_rollback(); /*--����ع�--*/
	sprintf(acErrMsg,"������������ʧ��![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120",g_pub_tx.reply);
    return 1;
}

