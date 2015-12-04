/*************************************************
* �� �� ��:  spB351.c
* ���������� ��ɱ��Ҳ�Э��¼�빦��
*
* ��    ��:  Terry
* ������ڣ� 2003��1��26��
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
#include "mo_sg_cash_po_c.h"
#include "mo_bank_acc_po_c.h"

int spB351()
{
	struct	mo_sg_cash_po_c sMoSgCash;		/* ���ҲֵǼǲ� */
	struct  mo_bank_acc_po_c sMoBankAcc;		/* �жһ�Ʊ */
	int	ret;

	memset( &sMoSgCash, 0x00, sizeof( struct mo_sg_cash_po_c ) );
	memset( &sMoBankAcc, 0x00, sizeof( struct mo_bank_acc_po_c ) );
	
	/* ���ݳ�ʼ�� */
	pub_base_sysinit();
	
	/* ��ǰ̨��Ļȡֵ */
	get_zd_data( "0580", sMoSgCash.acc_po_no );	/* �жһ�Ʊ���� */
	get_zd_data( "0590", sMoSgCash.sg_cash_no );/* ���Ҳ�Э���� */
	sMoSgCash.pickup_sum = 0;
	strcpy( sMoSgCash.br_no, g_pub_tx.tx_br_no );
	
	/* ȡ�жһ�Ʊ�ǼǱ���֤���� */
	ret = Mo_bank_acc_po_Sel( g_pub_tx.reply , &sMoBankAcc, \
			"acc_po_no = '%s'", sMoSgCash.acc_po_no );
	if ( ret == 100 )
	{
		sprintf( acErrMsg, "�޴˳жһ�Ʊ����" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B061" );
		goto ErrExit;	
	}else if ( ret )
	{
		sprintf( acErrMsg, "ִ��Mo_bank_acc_po_Sel��[%d]", ret );
		WRITEMSG
		goto ErrExit;		
	}
	

	pub_base_strpack( sMoBankAcc.bail_ac_no );
	
	if ( !strlen( sMoBankAcc.bail_ac_no ) )
	{
		sprintf( acErrMsg, "�жһ�Ʊ�ޱ�֤���ʺ�" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B070" );	
	}
	
	sMoSgCash.safeg_sum = sMoBankAcc.bail_amt;
	
	/* �Ǽǻ�Ʊ�ǼǱ� */
	ret = pub_sttl_RegMoSgCash( sMoSgCash );
	if ( ret )
    	{
        	sprintf( acErrMsg, "ִ��pub_sttl_RegMoSgCash����![%d]", ret );
        	WRITEMSG
        	goto ErrExit;
    	}
	
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
