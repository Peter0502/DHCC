/*************************************************
* �� �� ��:  spB353.c
* ���������� ��ɿ����������
*
* ��    ��:  jane 
* ������ڣ� 2003��8��9��
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
#include "mo_sg_cash_po_bill_c.h"
#include "mo_bank_acc_po_c.h"

int spB353()
{
	struct	mo_sg_cash_po_c 		sMoSgCash;			/* ���ҲֵǼǲ� */
	struct	mo_sg_cash_po_bill_c	sMoSgCashBill;		/* ���Ҳ�������Ǽǲ� */
	struct  mo_bank_acc_po_c		sMoBankAccPo;		/* �жһ�Ʊ������Ϣ�Ǽǲ� */
	char	cTemp[20];									/* ��������� */
	double	dAmt;										/* ������ */
	double	dUseAmt;									/* ���Կ��������� */
	int		ret;

	memset( &sMoSgCash, 0x00, sizeof( struct mo_sg_cash_po_c ) );
	memset( &sMoSgCashBill, 0x00, sizeof( struct mo_sg_cash_po_bill_c ) );
	memset( &sMoBankAccPo, 0x00, sizeof( struct mo_bank_acc_po_c ) );
	memset( cTemp, 0x00, sizeof( cTemp ) );

	/* ���ݳ�ʼ�� */
	pub_base_sysinit();
	
	/* ��ǰ̨��Ļȡֵ */
	get_zd_data( "0330", sMoSgCashBill.pact_no );	/* �ж�Э���� */
	get_zd_double( "0400", &dAmt ); 				/* ���������� */
	
	/* ���ݳж�Э���Ų������гжһ�Ʊ������Ϣ�ǼǱ� */
	ret = Mo_bank_acc_po_Sel( g_pub_tx.reply, &sMoBankAccPo, "pact_no='%s'", sMoSgCashBill.pact_no );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "�޴˳ж�Э����!" );
		WRITEMSG
		sprintf( g_pub_tx.reply, "B187" );
		goto ErrExit;
	}else if( ret )
	{
		sprintf( acErrMsg, "ִ��Mo_bank_acc_po_Sel��![%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	/* �ж�ֻ���ڳжһ�Ʊȫ��ǩ�����֮����ܿ������ */
	if( sMoBankAccPo.acc_po_sts[0] == '0' )
	{
		sprintf( acErrMsg, "�˳ж�Э��Ϊ¼��״̬!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B213" );
		goto ErrExit;
	}else if( sMoBankAccPo.acc_po_sts[0] == '1' )
	{
		sprintf( acErrMsg, "�˳ж�Э��δ��ǩ����δȫ��ǩ��!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B211" );
		goto ErrExit;
	}

	/* ���ֻ�гжһ�Ʊ¼���в���ȥ������Ʊ�Ŀ���������� */
	if( strcmp( sMoBankAccPo.tx_br_no, g_pub_tx.tx_br_no ) )
	{
		sprintf( acErrMsg, "���׻����Ǵ˳ж�Э��¼����!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B262" );
		goto ErrExit;
	}

	/* Ϊ��ӡ���֪ͨ�鴫�ж�Э��¼������ */
	set_zd_long( "0520", sMoBankAccPo.req_date );
vtcp_log( "ǩ������[%ld]",sMoBankAccPo.req_date );

	/* ȡ���ҲֵǼǱ���¼ */
	ret = Mo_sg_cash_po_Dec_Upd( g_pub_tx.reply, "pact_no = '%s'", sMoSgCashBill.pact_no );
	if( ret )
	{
		sprintf( acErrMsg, "ִ��Mo_sg_cash_po_Dec_Upd��[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}
	
	ret = Mo_sg_cash_po_Fet_Upd( &sMoSgCash, g_pub_tx.reply );
	if ( ret == 100 )
	{
		sprintf( acErrMsg, "�˳жҷǱ��Ҳ�!" );
		WRITEMSG
		sprintf( g_pub_tx.reply, "B267" );
		goto ErrExit;
	}else if ( ret )
	{
		sprintf( acErrMsg, "ִ��Mo_sg_cash_po_Fet_Upd��[%d]", ret );
		WRITEMSG
		goto ErrExit;	
	}

	dUseAmt = sMoSgCash.safeg_sum - sMoSgCash.pickup_sum;	/* �ɿ��������� */
vtcp_log( "janejanejane    safeg_sum[%lf]  pickup_sum[%lf]  dUseAmt[%lf]",sMoSgCash.safeg_sum, sMoSgCash.pickup_sum, dUseAmt );
	/* ������Ƿ񳬹��ɿ��������� */
	if ( pub_base_CompDblVal( dAmt, dUseAmt ) == 1 )
	{
		sprintf( acErrMsg, "������������ڿɿ���������!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B076" );
		goto ErrExit;
	}

	sMoSgCash.pickup_sum += dAmt;		/* �ѿ�������������ν�� */
vtcp_log( "�ѿ�����������ν��Ϊ[%lf]", sMoSgCash.pickup_sum );

	ret = Mo_sg_cash_po_Upd_Upd( sMoSgCash , g_pub_tx.reply  );
	if ( ret )
	{
		sprintf( acErrMsg, "�޸ı��ҲֵǼǱ���[%d]", ret );
		WRITEMSG
		goto ErrExit;	
	}

	Mo_sg_cash_po_Clo_Upd( );

	/* �ǼǱ��Ҳ�������ǼǱ� */
	
	/* ��ȡ���Ҳ�������Ǽǲ��ж�Ӧ��Э���������������� */
	ret = Mo_sg_cash_po_bill_Sel( g_pub_tx.reply, &sMoSgCashBill, "pact_no='%s'", sMoSgCashBill.pact_no );
	if( ret != 100 && ret )
	{
		sprintf( acErrMsg, "ִ��Mo_bank_acc_po_Sel��![%d]", ret );
		WRITEMSG
		goto ErrExit;
	}else if( ret == 100 )
	{
		sMoSgCashBill.bill_num = 1;		/* ��һ�ο����������������Ϊ1 */
	}else
	{
		pub_base_strpack( sMoSgCashBill.pact_no );
		/* ������� */
		ret = sql_max_int( "mo_sg_cash_po_bill", "bill_num", "pact_no='%s'", sMoSgCashBill.pact_no );
		if( ret < 0 )
		{
			sprintf(acErrMsg,"ִ��sql_min_string����![%d]", ret) ;
			WRITEMSG
			strcpy( g_pub_tx.reply, "D110" ) ;
			goto ErrExit;
		}
		
		/* �Ǽ����������:�ж�Э���ţ���� */
		sMoSgCashBill.bill_num = ret + 1;		/* �ٿ������ʱ������Զ���1 */
	}

	pub_base_strpack( sMoSgCashBill.pact_no );
	sprintf( cTemp, " %s%03d ", sMoSgCashBill.pact_no, sMoSgCashBill.bill_num );
	pub_base_strpack( cTemp );
	strcpy( sMoSgCashBill.lading_bill, cTemp );		/* ��������� */
vtcp_log("lading_bill[%s]",sMoSgCashBill.lading_bill);

	sMoSgCashBill.pickup_amt = dAmt;		/* �������� */

    vtcp_log("JYW pact_no=%s,bill=%d,,ladbill=%s,pick_amt=%lf",sMoSgCashBill.pact_no,
                 sMoSgCashBill.bill_num,sMoSgCashBill.lading_bill,sMoSgCashBill.pickup_amt);

	ret = Mo_sg_cash_po_bill_Ins( sMoSgCashBill, g_pub_tx.reply );
	if( ret )
	{
		sprintf( acErrMsg, "ִ��Mo_sg_cash_po_bill_Ins��[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	/* Ϊ��ӡ��ֵ */
	set_zd_double( "1001", sMoSgCash.pickup_sum );
	set_zd_long( "0500", sMoSgCashBill.bill_num );

	/* ����ˮ��־ */
	/**----- xxx ---- @20030819@ ---- ��������Ҫ��Э���� ----**/
	strcpy(g_pub_tx.ac_no,sMoSgCashBill.pact_no);
	g_pub_tx.tx_amt1=dAmt;
	strcpy(g_pub_tx.beg_note_no,sMoSgCashBill.lading_bill);
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
