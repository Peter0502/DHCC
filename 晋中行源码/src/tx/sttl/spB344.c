/*************************************************
* �� �� ��:  spB344.c
* ���������� ��ɳжҲ��ԭʼƱ��Ʊ���ջع��ܣ�
*
* ��    ��:  jane
* ������ڣ� 2003��8��25��
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
#include "mo_bank_split_po_c.h"
#include "com_parm_c.h"

int spB344()
{
	struct  mo_bank_split_po_c  sMoBankSplitPo; /* �жҲ��ԭʼƱ�ݵǼǲ� */
	char	cPayAcType[2];					/* �����ʻ����� */
	int		ret;

	memset( &sMoBankSplitPo, 0x00, sizeof( struct mo_bank_split_po_c ) );
	memset( cPayAcType, 0x00, sizeof( cPayAcType ) );

	/* ���ݳ�ʼ�� */
	pub_base_sysinit();

	/* ��ǰ̨��Ļȡֵ */
	get_zd_data( "0630", sMoBankSplitPo.pact_no );	/* ��Ѻ��� */
vtcp_log("��Ѻ���:[%s]",sMoBankSplitPo.pact_no);
	get_zd_data( "0670", cPayAcType );				/* �����ʻ����� */

	/* ������Ѻ��Ų��ҳжҲ��ԭʼƱ�ݵǼǲ� */
	ret = Mo_bank_split_po_Dec_Upd( g_pub_tx.reply, "pact_no='%s'", sMoBankSplitPo.pact_no );
	if ( ret )
	{
		sprintf( acErrMsg, "ִ��Mo_bank_split_po_Dec_Upd��[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	ret = Mo_bank_split_po_Fet_Upd( &sMoBankSplitPo, g_pub_tx.reply );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "�жҲ��ԭʼƱ�ݵǼǱ��в����ڴ���Ѻ���!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B308" );
		goto ErrExit;
   	}else if( ret )
   	{
   		sprintf( acErrMsg, "ִ��Mo_bank_split_po_Fet_Upd��![%d]", ret );
		WRITEMSG
		goto ErrExit;
   	}

	/* ���״̬�Ƿ�Ϊ¼�� */
	if( sMoBankSplitPo.sts[0] == '2' )
	{
		sprintf( acErrMsg, "���ųжһ�Ʊ����Ʊ���ջ�!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B304" );
		goto ErrExit;
	}

	/* �޸ĳжҲ��ԭʼƱ�ݵǼǱ� */
	strcpy( sMoBankSplitPo.pay_type, cPayAcType );	/* �����ʻ����� */
	strcpy( sMoBankSplitPo.sts, "2" );				/* 2 ��Ʊ���ջ� */

	ret = Mo_bank_split_po_Upd_Upd( sMoBankSplitPo, g_pub_tx.reply );
	if ( ret )
	{
		sprintf( acErrMsg, "�޸ĳжҲ��ԭʼƱ�ݵǼǱ���[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}
	Mo_bank_split_po_Clo_Upd();

vtcp_log( "janejanejane   [0450]=[%ld]", sMoBankSplitPo.matr_date );
	set_zd_long("0450",sMoBankSplitPo.matr_date);		/* Ʊ�ݵ������ڣ�Ϊ��Ȩ���� */

	/* ����ˮ��־ */
	/**--- xxx --- @20030818@ ---- ��Э���Ÿ����ʺų���ʱ�� ---**/
	strcpy(g_pub_tx.ac_no,sMoBankSplitPo.pact_no);
	g_pub_tx.tx_amt1=0.00;
	strcpy(g_pub_tx.brf,"�жҲ��ԭʼƱ��Ʊ���ջ�");
	ret = pub_ins_trace_log();
	if ( ret )
	{
		sprintf( acErrMsg, "����ˮ��־��" );
		WRITEMSG
		goto ErrExit;
	}
	
	if( cPayAcType[0] == '1' )		/* �����ʻ� */
	{
		sprintf(acErrMsg,"�����ʻ�");
		WRITEMSG
		goto	OkExit1;
	}else if( cPayAcType[0] == '2' )	/* ����ʻ� */
	{		
		sprintf(acErrMsg,"����ʻ�");
		WRITEMSG
		goto	OkExit2;
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
OkExit1:
	strcpy( g_pub_tx.reply, "0001" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
OkExit2:
	strcpy( g_pub_tx.reply, "0002" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
}
