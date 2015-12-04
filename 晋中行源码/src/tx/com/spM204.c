/*************************************************
* �� �� ��:  spM204.c
* ���������� �ṩ�񳤶Թ�ԱǮ�����Ĺ��ܡ�
*
* ��    ��:  andy
* ������ڣ� 2004��4��30��
*
* �޸ļ�¼�� 
*   ��   ��: 2004��7��30��
*   �� �� ��: andy
*   �޸�����: 1.Ǯ�����ֻ�ܹ񳤲�����
*             2.ֻ����Ǯ��յ�ʱ�����ֹ��
*************************************************/

#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "cash_mst_c.h"
#include "com_tel_c.h"

int spM204()
{
	struct	cash_mst_c	sComMst;		/* �ֽ�̨�� */
	struct	com_tel_c	sComTel;		/* ��Ա��Ϣ */
	char	cOperCode[2];				/* ������ */
	int	ret;

	memset( &sComMst, 0x00, sizeof( struct cash_mst_c ) );
	memset( &sComTel, 0x00, sizeof( struct com_tel_c ) );
	memset( cOperCode, 0x00, sizeof( cOperCode ) );

	/* ���ݳ�ʼ�� */
	pub_base_sysinit();

	/* ��ǰ̨��Ļȡֵ */
	get_zd_data( "0670", cOperCode );		/* ������ */
	get_zd_data( "0920", sComMst.tel );		/* ��Ա���� */
	get_zd_data( "0210", sComMst.cur_no );		/* ���� */

	/* ���ݹ�Ա�Ų�ѯ��Ա��Ϣ */
	ret = Com_tel_Sel( g_pub_tx.reply, &sComTel, "tel='%s'", g_pub_tx.tel );
	if( ret )
	{
		sprintf( acErrMsg, "��ѯ��Ա��Ϣ����!tel[%s]sqlcode=[%d]", g_pub_tx.tel, ret );
		WRITEMSG
		goto ErrExit;
	}
	if( sComTel.lvl[0] != '3' )
	{
		sprintf( acErrMsg, "Ǯ�����ֻ���ɹ��鳤����!tel[%s]", g_pub_tx.tel );
		WRITEMSG
       	strcpy( g_pub_tx.reply, "M024" );
		goto ErrExit;
	}

	/* ���ݹ�Ա�źͱ��ֲ�ѯ�ֽ�̨�� */
	ret = Cash_mst_Dec_Upd( g_pub_tx.reply, "br_no='%s' and tel='%s' \
				and cur_no='%s'", g_pub_tx.tx_br_no,\
				sComMst.tel, sComMst.cur_no );
	if( ret )
	{
		sprintf( acErrMsg, "ִ��Cash_mst_Dec_Upd��!ret=[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	ret = Cash_mst_Fet_Upd( &sComMst, g_pub_tx.reply );
	if ( ret == 100 )
   	{
       	sprintf( acErrMsg, "�ֽ�̨���в����ڴ˹�ԱǮ���¼!" );
       	WRITEMSG
       	strcpy( g_pub_tx.reply, "M008" );
       	goto ErrExit;
   	}
	else if ( ret )
   	{
       	sprintf( acErrMsg, "ִ��Cash_mst_Fet_Upd��!ret=[%d]",ret );
       	WRITEMSG
       	goto ErrExit;
   	}
   	if( cOperCode[0]== sComMst.sts[0] ) /* �Ѿ����û����Ѿ�ͣ�� ��ֹ�ظ����� */
   	{
   		if(cOperCode[0] == '0')
   		{
   		sprintf( acErrMsg, "�ù�Ա�Ѿ�������״̬�������ظ�����!" );
			WRITEMSG
			  strcpy( g_pub_tx.reply, "T050" );
   		goto ErrExit;
   	  }
   	  else if (cOperCode[0] == '1')
   	  {
   	  sprintf( acErrMsg, "�ù�Ա�Ѿ���ֹ��״̬�������ظ�����!" );
			WRITEMSG
			  strcpy( g_pub_tx.reply, "T051" );
   		goto ErrExit;
   	  }
   		else 
   		{
   			sprintf( acErrMsg, "�޴�״̬!" );
			WRITEMSG
			  strcpy( g_pub_tx.reply, "M008" );
   		goto ErrExit;
   		}
   	}

   	if( cOperCode[0] == '0' )	/* ���� */
   		strcpy( sComMst.sts, "0" );	/* ״̬: 0������ */
   	else				/* ֹ�� */
	{
		/* �ж�Ǯ���Ƿ�Ϊ�� */
		if( pub_base_CompDblVal(sComMst.bal, 0.00) == 1 )
		{
			sprintf( acErrMsg, "�ù�ԱǮ�������,����ֹ��!" );
			WRITEMSG
   			strcpy( g_pub_tx.reply, "M025" );
			goto ErrExit;
		}

   		strcpy( sComMst.sts, "1" );	/* ״̬: 1�������� */
	}

   	ret = Cash_mst_Upd_Upd( sComMst, g_pub_tx.reply );
   	if( ret )
	{
		sprintf( acErrMsg, "ִ��Cash_book_Upd_Upd��!ret=[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

    	Cash_mst_Clo_Upd( );

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
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
