/*************************************************
* �� �� ��:  sp9974.c
* ���������� ���Ҳֱ�֤���˺ſ��
*
* ��    ��:  jane
* ������ڣ� 2003��8��8��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "mo_bank_acc_po_c.h"
#include "mo_bank_po_c.h"

int sp9974()
{
	struct	mo_bank_acc_po_c	sMoBankAccPo;	/* ���гжһ�Ʊ������Ϣ�ǼǱ� */
	struct	mo_bank_po_c		sMoBankPo;	/* �жһ�Ʊ�Ǽǲ� */
	double	dPoAmt;					/* ��ǩ��Ʊ���ܽ�� */
	char	cPoType[2];				/* �ж����� */
	char	cBailAcNo[25];				/* ��֤���ʺ� */
	double	dBailAmtRatio;				/* ��֤����� */
	int	ret;

	memset( &sMoBankAccPo, 0x00, sizeof( struct mo_bank_acc_po_c ) );
	memset( &sMoBankPo, 0x00, sizeof( struct mo_bank_po_c ) );
	memset( cPoType, 0x00, sizeof( cPoType ) );
	memset( cBailAcNo, 0x00, sizeof( cBailAcNo ) );

	get_zd_data( "0330", sMoBankAccPo.pact_no );	/* �ж�Э���� */
	get_zd_data( "0320", cBailAcNo );		/* ��֤���ʺ� */
	get_zd_data( "0670", cPoType );			/* �ж����� */
	get_zd_double( "0400", &dBailAmtRatio );/* ��֤����� */
sprintf( acErrMsg, "sMoBankAccPo.pact_no[%s]cBailAcNo[%s]cPoType[%s]",sMoBankAccPo.pact_no,cBailAcNo,cPoType );
WRITEMSG
	/* ���ݳж�Э���Ų������гжһ�Ʊ������Ϣ�ǼǱ� */
	ret = Mo_bank_acc_po_Sel( g_pub_tx.reply, &sMoBankAccPo, "pact_no='%s'", sMoBankAccPo.pact_no );
	if( ret == 100 )
	{
		ret = pub_base_CompDblVal( dBailAmtRatio, 0.00 );
		if( ret == 1 )
		{
			sprintf( acErrMsg, "���ѡ��֤��жң�����������ɴ汣֤����!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B264" );
			goto ErrExit;
		}
   	}else if( ret )
   	{
   		sprintf( acErrMsg, "ִ��Mo_bank_acc_po_Sel��![%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	pub_base_strpack( sMoBankAccPo.bail_ac_no );
sprintf( acErrMsg, "sts[%s]", sMoBankAccPo.acc_po_sts );
WRITEMSG
	if( sMoBankAccPo.acc_po_sts[0] != 'A' && sMoBankAccPo.acc_po_sts[0] != 'B' )
	{
		sprintf( acErrMsg, "�˳ж�Э�����Ѵ���!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B191" );
		goto ErrExit;
	}

	/* ���ж�����Ϊ���Ҳ֣�����Ҫ��鱣֤���˺ż���� */
sprintf( acErrMsg, "sMoBankAccPo.pact_no[%s]cBailAcNo[%s]bail_ac_no[%s]",sMoBankAccPo.pact_no,cBailAcNo,sMoBankAccPo.bail_ac_no );
WRITEMSG	
	if( strcmp( sMoBankAccPo.bail_ac_no, cBailAcNo ) != 0 )
	{
		sprintf( acErrMsg, "��֤���˺ű�����ɴ汣֤��ʱ¼��һ��!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B258" );
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
