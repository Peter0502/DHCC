/*************************************************
* �� �� ��:  spB107.c
* ���������� ������л�Ʊ���ڶҸ��Ĺ��ܡ�ǩ���и������Ʊ��
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

int spB107()
{
	struct	mo_bank_po_c	sMoBankPo;	/* ���л�Ʊ�Ǽǲ� */
	char	cOpenBrno[9];			/* �ʺſ������� */
	char	cFlag[2];			/* �Ƿ��Ǳ��б�־ */
	char	cOperCode[6];				/* ҵ����� */
	long	lAcId;
	int	num = 0;
	int	ret;
	
	/* ���ݳ�ʼ�� */
	memset( &sMoBankPo, 0x00, sizeof( struct mo_bank_po_c ) );
	memset( cOpenBrno, 0x00, sizeof( cOpenBrno ) );
	memset( cOperCode, 0x00, sizeof( cOperCode ) );
	
	pub_base_sysinit();
	
	/* ��ǰ̨��Ļȡֵ */
	get_zd_data( "0590", sMoBankPo.po_no );		/* ��Ʊ���� */
	get_zd_data( "0810", sMoBankPo.payee_name );	/* �տ���ȫ�� */
	get_zd_data( "0310", sMoBankPo.payee_ac_no );	/* �տ����ʺ� */
	get_zd_long( "0340", &sMoBankPo.payee_ac_seqn );	/* �տ����ʺ���� */
	
	get_zd_data( "0670", cFlag );			/* �տ����ʺ������� */
	strcpy( cOperCode, "10001" );
	
	/* ����տ����ʺ��Ǳ��У���Ҫ����ʺ��Ƿ���ڣ�״̬�Ƿ����� */
	if ( cFlag[0] == 'Y' )	/* �Ǳ��� */
	{
		ret = pub_base_ChkAc( sMoBankPo.payee_ac_no, \
				sMoBankPo.payee_ac_seqn, cOpenBrno, &lAcId );
		if ( ret )
		{
			sprintf( acErrMsg, "����ʻ�״̬��ȡ�ʻ�id��[%d]", ret );
			WRITEMSG
			goto ErrExit;	
		}
	}
	
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
	ret = sql_count( "mo_bank_po", "po_no = '%s' and po_sts = '2' \
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

	/* �������α� */
	ret = Mo_bank_po_Dec_Upd( g_pub_tx.reply , "po_no = '%s'and po_sts = '2' \
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
	sMoBankPo.bal_date = g_pub_tx.tx_date;		/* �������� */
	strcpy( sMoBankPo.po_sts, "3" );		/* ��Ʊ״̬ */
	strcpy( sMoBankPo.po_ind, "1" );		/* ��Ʊ��־ */	
	
	/* �޸Ļ�Ʊ�ǼǱ� */
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

