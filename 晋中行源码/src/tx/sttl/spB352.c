/*************************************************
* �� �� ��:  spB352.c
* ���������� ��ɱ�֤�����
*
* ��    ��:  jane
* ������ڣ� 2003��7��28��
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

int spB352()
{
	struct	mo_sg_cash_po_c sMoSgCash;	/* ���ҲֵǼǲ� */
	struct  mo_bank_acc_po_c sMoBankAcc;/* �жһ�Ʊ������Ϣ�Ǽǲ� */
	double	dAmt;						/* ������ */
	int	ret;

	memset( &sMoSgCash, 0x00, sizeof( struct mo_sg_cash_po_c ) );
	memset( &sMoBankAcc, 0x00, sizeof( struct mo_bank_acc_po_c ) );
	memset( cSgCashNo, 0x00, sizeof( cSgCashNo ) );
	
	/* ���ݳ�ʼ�� */
	pub_base_sysinit();
	
	/* ��ǰ̨��Ļȡֵ */
	get_zd_data( "0330", sMoSgCash.pact_no );	/* �ж�Э���� */
	get_zd_double( "0390", &dAmt ); 			/* ������ */
	
	/* ���Э�����Ƿ���� */
	ret = sql_count("mo_sg_cash_po", "sg_cash_no = '%s'", cSgCashNo );
	if ( ret < 0 )
	{
		sprintf( acErrMsg, "���ұ��Ҳ�Э���Ŵ�[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}else if ( !ret )
	{
		sprintf( acErrMsg, "�ޱ��Ҳ�Э����" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B072" );
		goto ErrExit;
	}else if ( ret > 1 )
	{
		sprintf( acErrMsg, "���ڶ������Ҳ�Э����" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B073" );
		goto ErrExit;
	}
	
	/* ȡ���ҲֵǼǱ���¼ */
	ret = Mo_sg_cash_po_Dec_Upd( g_pub_tx.reply , "sg_cash_no = '%s'", \
					cSgCashNo );
	if ( ret )
	{
		sprintf( acErrMsg, "ִ��Mo_sg_cash_po_Dec_Upd��[%d]", ret );
		WRITEMSG
		goto ErrExit;	
	}
	
	ret = Mo_sg_cash_po_Fet_Upd( &sMoSgCash , g_pub_tx.reply );
	if ( ret == 100 )
	{
		sprintf( acErrMsg, "�޴˱��Ҳ�Э���¼" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B072" );
		goto ErrExit;
	}else if ( ret )
	{
		sprintf( acErrMsg, "ִ��Mo_sg_cash_po_Fet_Upd��[%d]", ret );
		WRITEMSG
		goto ErrExit;	
	}
	
	/* ��ѯ�жһ�Ʊ������Ϣ�Ǽǲ������ж�Э�����Ƿ���ڣ��ж������Ƿ�Ϊ���Ҳ� */
	ret = Mo_bank_acc_po_Sel( g_pub_tx.reply , &sMoBankAcc , \
				"pact_no = '%s'", sMoSgCash.pact_no );
	if ( ret == 100 )
	{
		sprintf( acErrMsg, "�˳ж�Э���Ų�����!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B199" );
		goto ErrExit;
	}else if ( ret )
	{
		sprintf( acErrMsg, "ִ��Mo_bank_acc_po_Sel��[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	if( strcmp( sMoBankAcc.acc_type, "4" ) != 0 )
	{
		sprintf( acErrMsg, "�˳ж����ͷǱ��Ҳ�!acc_type=[%s]", sMoBankAcc.acc_type );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B256" );
		goto ErrExit;
	}

	sMoSgCash.safeg_sum += dAmt;
	
	/* ����Ѵ汣֤���ܽ���Ƿ񳬹��жһ�Ʊ��� */
	if ( pub_base_CompDblVal(sMoSgCash.safeg_sum, sMoBankAcc.bail_amt) == 1 )
	{
		sprintf( acErrMsg, "�Ѵ汣֤������ڳжһ�Ʊ��֤����" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B074" );
		goto ErrExit;
	}
	
	/* �޸ı��ҲֵǼǱ��Ѵ��뱣֤���ܽ�� */
	ret = Mo_sg_cash_po_Upd_Upd( sMoSgCash , g_pub_tx.reply  );
	if ( ret )
	{
		sprintf( acErrMsg, "�޸ı��ҲֵǼǱ���[%d]", ret );
		WRITEMSG
		goto ErrExit;	
	}
	
	Mo_sg_cash_po_Clo_Upd( );
	
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
