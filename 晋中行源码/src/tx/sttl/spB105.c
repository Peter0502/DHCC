/*************************************************
* �� �� ��:  spB105.c
* ���������� ������л�Ʊ�����Ĺ��ܡ�ǩ���нӵ���������
*	    �Ľ������н跽�����ͽ���֪ͨʱ������ԭ������
*	    �Ŀ�Ƭ��֮�˶�����ִ�иý��ס�
*
* ��    ��:  Terry
* ������ڣ� 2003��1��19��
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
#include "mo_bank_po_c.h"

int spB105()
{
	struct	mo_bank_po_c	sMoBankPo;	/* ���л�Ʊ�Ǽǲ� */
	char	cOperCode[6];				/* ҵ����� */
	int	ret;
	
	/* ���ݳ�ʼ�� */
	memset( &sMoBankPo, 0x00, sizeof( struct mo_bank_po_c ) );
	memset( cOperCode, 0x00, sizeof( cOperCode ) );
	
	pub_base_sysinit();
	
	/* ��ǰ̨��Ļȡֵ */
	get_zd_data( "0590", sMoBankPo.po_no );		/* ��Ʊ���� */
	get_zd_double( "0390", &sMoBankPo.sign_amt );	/* ǩ����� */
	get_zd_double( "0400", &sMoBankPo.amt );	/* ������ */
	sMoBankPo.bal_date = g_pub_tx.tx_date;		/* �������� */
	strcpy( cOperCode, "10001" );
	
	/* ���һ�Ʊ�Ƿ���� */
	ret = sql_count( "mo_bank_po", "po_no = '%s'", sMoBankPo.po_no );
	if ( ret < 0 )
	{
		sprintf( acErrMsg, "���һ�Ʊ�ǼǱ���[%d]", ret );
		WRITEMSG
		sprintf( g_pub_tx.reply, "B018" );
		goto ErrExit;
	}else if ( !ret )
	{
		sprintf( acErrMsg, "�޴����л�Ʊ" );
		WRITEMSG
		sprintf( g_pub_tx.reply, "B012" );
		goto ErrExit;
	}
	
	/* ����Ʊ״̬���߱�־�Ƿ����� */
	ret = sql_count( "mo_bank_po", "po_no = '%s' and po_sts = '1' \
			and po_ind = '1'", sMoBankPo.po_no );
	if ( ret < 0 )
	{
		sprintf( acErrMsg, "���һ�Ʊ�ǼǱ���[%d]", ret );
		WRITEMSG
		sprintf( g_pub_tx.reply, "B018" );
		goto ErrExit;
	}else if ( !ret )
	{
		sprintf( acErrMsg, "���л�Ʊ״̬���߱�־������ret=[%d]",ret );
		WRITEMSG
		sprintf( g_pub_tx.reply, "B032" );
		goto ErrExit;
	}else if ( ret > 1 )
	{
		sprintf( acErrMsg, "���л�Ʊ�ǼǱ����ڶ�����¼" );
		WRITEMSG
		sprintf( g_pub_tx.reply, "B019" );
		goto ErrExit;	
	}

	/* ����������Ƿ�С�ڵ���ǩ����� */
	if( pub_base_CompDblVal( sMoBankPo.sign_amt , sMoBankPo.amt ) == 1 )
	{
		sprintf( acErrMsg, "���������ǩ�����,���ܶҸ���Ʊ!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B030" );
		goto ErrExit;
	}
	
	/* �������α� */
	ret = Mo_bank_po_Dec_Upd( g_pub_tx.reply , "po_no = '%s'and po_sts = '1' \
					and po_ind = '1'", sMoBankPo.po_no );
	if ( ret )
	{
		sprintf( acErrMsg, "ִ��	Mo_bank_po_Dec_Upd��[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}
	
	ret = Mo_bank_po_Fet_Upd( &sMoBankPo, g_pub_tx.reply );
	if ( ret == 100 )
	{
		sprintf( acErrMsg, "���һ�Ʊ�ǼǱ����޴����л�Ʊ" );
		WRITEMSG
		sprintf( g_pub_tx.reply, "B012" );
		goto ErrExit;
	}else if ( ret )
	{
		sprintf( acErrMsg, "ִ��Mo_bank_po_Fet_Upd��[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}
			
	/* ��齻�׻����Ƿ����Ȩ�� */
	ret = pub_sttl_ChkSignBrno( cOperCode, sMoBankPo.tx_br_no, g_pub_tx.tx_br_no );
	if ( ret )
	{
		sprintf( acErrMsg, "��齻�׻����Ƿ���ǩ���д�" );
		WRITEMSG
		goto ErrExit;	
	}
	
	/* �޸Ļ�Ʊ�ǼǱ� */
	strcpy( sMoBankPo.po_sts, "6" );		/* ��Ʊ״̬ */
	
	ret = Mo_bank_po_Upd_Upd( sMoBankPo , g_pub_tx.reply  );
	if ( ret )
	{
		sprintf( acErrMsg, "�޸����л�Ʊ�ǼǱ���[%d]", ret );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B017" );
		goto ErrExit;	
	}
	
	Mo_bank_po_Clo_Upd( );
	
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


