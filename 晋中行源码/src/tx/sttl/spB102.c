/*************************************************
* �� �� ��:  spB102.c
* ���������� ���ǩ�����л�Ʊ���ܣ�ϵͳ���ڼ�Ϊ��Ʊ���ڣ���ӡ����Ʊ�ϡ�
*
* ��    ��:  jane
* ������ڣ� 2003��1��18��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "mo_bank_po_c.h"

int spB102()
{
	struct	mo_bank_po_c	sMoBankPo;		/* ���л�Ʊ�Ǽǲ� */
	char	cPoRequNo[17];				/* ��������� */
	char	cPoNo[17];				/* ��Ʊ���� */
	char	cOperCode[6];				/* ҵ����� */
	int	ret;

	memset( &sMoBankPo, 0x00, sizeof( struct mo_bank_po_c ) );
	memset( cPoRequNo, 0x00, sizeof( cPoRequNo ) );
	memset( cPoNo, 0x00, sizeof( cPoNo ) );
	memset( cOperCode, 0x00, sizeof( cOperCode ) );
	
	/* ���ݳ�ʼ�� */
	pub_base_sysinit();
	
	/* ��ǰ̨��Ļȡֵ */
	get_zd_data( "0580", cPoRequNo );
	get_zd_data( "0590", cPoNo );
	strcpy( cOperCode, "10001" );
	
	/* ���ݻ�Ʊ���롢�����������һ�Ʊ�ǼǱ� */
	ret = sql_count( "mo_bank_po", "po_no='%s' and po_requ_no='%s'",\
			cPoNo, cPoRequNo );
	if ( ret < 0 )
	{
		sprintf( acErrMsg, "��ѯ��Ʊ�ǼǱ���,ret=[%d]", ret );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B018" );
		goto ErrExit;	
	}else if ( ret == 0 )
	{
		sprintf( acErrMsg, "�޴����л�Ʊ" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B012" );
		goto ErrExit;	
	}
	
	/* ����Ʊ״̬�Ƿ�Ϊ¼��0���߱�־�Ƿ�Ϊ����1 */
	ret = sql_count( "mo_bank_po", "po_no='%s' and po_requ_no='%s' and po_sts='%s' \
			and po_ind='%s'",cPoNo, cPoRequNo, "0", "1" );
	if ( ret < 0 )
	{
		sprintf( acErrMsg, "��ѯ��Ʊ�ǼǱ���,ret=[%d]", ret );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B018" );
		goto ErrExit;	
	}else if ( ret == 0 )
	{
		sprintf( acErrMsg, "���л�Ʊ״̬���߱�־������" );
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
	
	/* �������α� */
	ret = Mo_bank_po_Dec_Upd( g_pub_tx.reply , "po_no='%s' \
				and po_requ_no='%s' and po_sts='%s' \
				and po_ind='%s'", cPoNo, cPoRequNo ,"0", "1" );
	if ( ret )
	{
		sprintf( acErrMsg, "ִ��Mo_bank_po_Dec_Upd��[%d]", ret );
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

	/* ����Ա���ڻ����Ƿ����ǩ��֧ƱȨ��,�������Ƿ����ڱ���ǩ�� */
	ret = pub_sttl_ChkSignBrno( cOperCode, sMoBankPo.tx_br_no, g_pub_tx.tx_br_no );
	if ( ret )
	{
		sprintf( acErrMsg, "������Ȩ�޴�!ret=[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	/* ���л�Ʊ�Ǽǲ�״̬λ��Ϊǩ�� */
	strcpy( sMoBankPo.sign_br_no, g_pub_tx.tx_br_no );	/* ǩ�����к� */
	strcpy( sMoBankPo.po_sts, "1" );			/* "1"��ʾǩ��״̬ */
	sMoBankPo.sign_date = g_pub_tx.tx_date;			/* ǩ������ */
	
	ret = Mo_bank_po_Upd_Upd( sMoBankPo, g_pub_tx.reply );
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
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
