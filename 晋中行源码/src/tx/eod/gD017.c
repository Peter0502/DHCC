/*************************************************************
* �� �� ��: gD017.c
* �������������޶�����ļ�ά��
*
* ��    ��: jack
* �������: 2003��3��14��
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
**************************************************************/
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"
#include "chnl_tradeadd_c.h"
#include "com_parm_c.h"

gD017()
{
	int ret = 0;
	int days = 0;
	long clr_date = 0;

	struct chnl_tradeadd_c sChnl_tradeadd;
	struct com_parm_c sCom_parm;

	memset(&sChnl_tradeadd, 0x00, sizeof(sChnl_tradeadd) );
	memset(&sCom_parm, 0x00, sizeof(sCom_parm) );

	ret=sql_begin(); /*������*/
	if( ret ) 
	{
		sprintf( acErrMsg, "������ʧ��!!!" );
		WRITEMSG
		goto ErrExit;
	}

    /**------- ��ʼ��ȫ�ֱ��� --------**/
	pub_base_sysinit();

	ret = Com_parm_Sel( g_pub_tx.reply , sCom_parm, "parm_code = 'KXEQ' and parm_seqn = 1");
	{
		sprintf( acErrMsg, "��ѯ���޶��¼������������!!!" );
		WRITEMSG
		goto ErrExit;
	}

	days = atoi( sCom_parm.val );

	clr_date = pub_base_daynumSUB(g_pub_tx.tx_date, days );

	ret = sql_execute("delete from chnl_tradeadd where \
			tx_date <= %ld", clr_date);
	if (ret != 0)
	{
		sprintf( acErrMsg, "ɾ�����޶��¼����!!!" );
		WRITEMSG
		goto ErrExit;
	}

OkExit:
	sql_commit();	/*--�ύ����--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"���޶�����ļ�ά��ͨ��!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--����ع�--*/
	sprintf(acErrMsg,"���޶�����ļ�ά��ʧ��!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
