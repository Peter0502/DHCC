/*************************************************
* �� �� ��:  spB937.c
* ���������� ��ɻع�ʽת����ת�����ڻع����ܣ������޸�
*	     ���ֵǼǲ��������������/��ش���ӽ��ס�
*	     �ڲ��ʽ跽�ӽ��ף�233ȡ���
*
* ��    ��:  jane
* ������ڣ� 2003��4��15��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����: ����¼�뽻��
*************************************************/

#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "mo_discnt_c.h"

int spB937()
{
	struct	mo_discnt_c		sMoDiscnt;		/* ���ֵǼǱ� */
	char    cAccHrt[6];                     /* ͬ�ǿ�Ŀ������ */
	int		ret;
	double  T_amt=0.00;

	memset( &sMoDiscnt, 0x00, sizeof( struct mo_discnt_c ) );
	memset( cAccHrt, 0x00, sizeof( cAccHrt ) );

	/* ���ݳ�ʼ�� */
	pub_base_sysinit();
	
	/* ��ǰ̨��Ļȡֵ */
	get_zd_data( "0630", sMoDiscnt.tpact_no );		/* ת����Э���� */

	/* ������ֵǼǲ���ת����Э���š���������Ϊ�ع�ת����״̬Ϊ�����ļ�¼�Ƿ���� */
	ret = Mo_discnt_Dec_Upd( g_pub_tx.reply, "tpact_no='%s'", sMoDiscnt.tpact_no );
	if ( ret )
	{
		sprintf( acErrMsg, "ִ��Mo_pay_discnt_Dec_Upd��[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	ret = Mo_discnt_Fet_Upd( &sMoDiscnt, g_pub_tx.reply );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "��ת����Э���Ų�����!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B133" );
		goto ErrExit;
   	}else if( ret )
   	{
   		sprintf( acErrMsg, "ִ��Mo_discnt_Fet_Upd��![%d]", ret );
		WRITEMSG
		goto ErrExit;
   	}

	if( sMoDiscnt.pn_type[0] != '5' )
	{
		sprintf( acErrMsg, "��Э��ǻع�ʽת����ת������!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B166" );
		goto ErrExit;
	}

	if( sMoDiscnt.sts[0] == '1' )
	{
		sprintf( acErrMsg, "�˱�����δ����!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B163" );
		goto ErrExit;
	}else if( sMoDiscnt.sts[0] == '*' )
	{
		sprintf( acErrMsg, "�˱������ѵ��ڻع�!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B160" );
		goto ErrExit;
	}else if( sMoDiscnt.sts[0] == '4' )
	{
		sprintf( acErrMsg, "�˱�������¼��!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B326" );
		goto ErrExit;
	}

	/* �޸����ֵǼǱ���״̬Ϊת����ת�����ڻع�¼�� */
	strcpy( sMoDiscnt.sts, "4" );

	ret = Mo_discnt_Upd_Upd( sMoDiscnt, g_pub_tx.reply );
	if ( ret )
	{
		sprintf( acErrMsg, "�޸����ֵǼǱ���[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	Mo_discnt_Clo_Upd();

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
