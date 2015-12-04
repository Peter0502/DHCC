/*************************************************
* �� �� ��:  spB109.c
* ���������� �ǼǶҸ��з����Ĳ�ѯ�������ݣ��Լ���ѯ״����
*
* ��    ��:  jane
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
#include "mo_po_qry_c.h"

int spB109()
{
	struct	mo_bank_po_c	sMoBankPo;		/* ���л�Ʊ�Ǽǲ� */
	struct	mo_po_qry_c	sMoPoQry;		/* ��ѯ�鸴�ǼǱ� */
	
	char	cPoNo[17];				/* ��Ʊ���� */
	char	cBrf[201];				/* ���� */
	char	cOperCode[6];				/* ҵ����� */
	int	ret;
	
	memset( &sMoBankPo, 0x00, sizeof( struct mo_bank_po_c ) );
	memset( &sMoPoQry, 0x00, sizeof( struct mo_po_qry_c ) );
	memset( cPoNo, 0x00, sizeof( cPoNo ) );
	memset( cBrf, 0x00, sizeof( cBrf ) );
	memset( cOperCode, 0x00, sizeof( cOperCode ) );
	
	/* ���ݳ�ʼ�� */
	pub_base_sysinit();
	
	/* ��ǰ̨��Ļȡֵ */
	get_zd_data( "0590", cPoNo );
	get_zd_data( "0810", cBrf );
	strcpy( cOperCode, "10001" );
	
	/* ���ݻ�Ʊ������һ�Ʊ�ǼǱ� */
	ret = Mo_bank_po_Sel( g_pub_tx.reply, &sMoBankPo, "po_no='%s'", cPoNo );
	if( ret == 100 )
    	{
        	sprintf(acErrMsg,"���һ�Ʊ�ǼǱ���!,�޴˻�Ʊ��Ϣ!po_no=[%s]",cPoNo);
        	WRITEMSG
       	 	strcpy( g_pub_tx.reply, "B012" );
        	goto ErrExit;
    	}else if ( ret )
    	{
        	sprintf( acErrMsg, "ִ��Mo_bank_po_Sel����!ret=[%d]", ret );
        	WRITEMSG
        	goto ErrExit;
    	}
	
	/* ��齻�׻����Ƿ��ǻ�Ʊ��ǩ���� */
	ret = pub_sttl_ChkSignBrno( cOperCode, sMoBankPo.tx_br_no, g_pub_tx.tx_br_no );
	if ( ret )
	{
		sprintf( acErrMsg, "������Ȩ�޴�!ret=[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}
	
	/* Ϊ��ѯ�鸴�Ǽǲ��ṹ��ֵ */
	strcpy( sMoPoQry.qry_typ, "0" );			/* ��ѯ�鸴����Ϊ0��ѯ */
	sMoPoQry.tx_date = g_pub_tx.tx_date;			/* �������� */
	strcpy( sMoPoQry.brf, cBrf );				/* ���� */
	/*strcpy( sMoPoQry.sts, "" );				 ״̬ */
	strcpy( sMoPoQry.tel, g_pub_tx.tel );			/* ��Ա */
	strcpy( sMoPoQry.br_no, g_pub_tx.tx_br_no );		/* ���� */
	
	/* �Ǽǲ�ѯ�鸴�Ǽǲ� */
	ret = pub_sttl_RegMoPoQry( sMoPoQry );
	if ( ret )
    	{
        	sprintf( acErrMsg, "ִ��pub_sttl_RegMoPoQry����!ret=[%d]", ret );
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
