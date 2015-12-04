/*************************************************
* �� �� ��:  spB432.c
* ���������� ��ɵǼ�ί���տ��嵥¼�빦�ܡ�
*
* ��    ��:  jane
* ������ڣ� 2003��2��2��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"

int spB432()
{
	char	cLoadName[50];			/* �ϴ��ļ��� */
	char	cOperCode[6];			/* ҵ����� */
	int	ret;
	
	memset( cLoadName, 0x00, sizeof( cLoadName ) );
	memset( cOperCode, 0x00, sizeof( cOperCode ) );
	
	/* ���ݳ�ʼ�� */
	pub_base_sysinit();
	
	strcpy( cOperCode, "10006" );
	
	/* ��齻�׻����Ƿ����Ȩ�� */
	ret = pub_sttl_ChkInputBrno( cOperCode, g_pub_tx.tx_br_no );
	if ( ret )
	{
		sprintf( acErrMsg, "��齻�׻���Ȩ�޴�" );
		WRITEMSG
		goto ErrExit;
	}
	
	/* ���ļ�������ʱ��mo_trust_list�� */
	Pubscname( cLoadName );
	ret = tab_load( cLoadName, "mo_trust_list", "turst_pay_no,payer_seqn,payer_ac_no,\
			payer_name,payer_br_no,payer_br_name,amt,'1'" );
		
	/* ����ˮ��־ */
	ret = pub_ins_trace_log();
	if ( ret )
	{
		sprintf( acErrMsg, "����ˮ��־��" );
		WRITEMSG
		goto ErrExit;
	}

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
