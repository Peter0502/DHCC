/*************************************************
* �� �� ��:  sp9913.c
* ���������� �ɳжһ�Ʊ������Գжһ�Ʊ��Ϣ
*
* ��    ��:  jane
* ������ڣ� 2003��5��30��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "mo_bank_po_c.h"
#include "mo_bank_acc_po_c.h"
#include "com_parm_c.h"

int sp9913()
{
	struct	com_parm_c		wd_com_parm;		/* �жһ�Ʊ�Ǽǲ� */
	struct	mo_bank_po_c		sMoBankPo;		/* �жһ�Ʊ�Ǽǲ� */
	struct	mo_bank_acc_po_c	sMoBankAccPo;	/* ���гжһ�Ʊ������Ϣ�ǼǱ� */
	char	cPoNo[17];							/* �жһ�Ʊ������Ļȡֵ */
	char	cPoNo1[9];							/* �жһ�Ʊ������Чλ */
	char	cPoNoHead[9];						/* �жһ�Ʊ�������ͷ */
	char	cPoNo2[17];							/* �жһ�Ʊ�������ͷ+��Чλ */
	int		ret=0;
	int     parm_ind = 0;
	char	flag[2];							/* �жһ�Ʊ�������ͷ+��Чλ */

	memset( &sMoBankPo, 0x00, sizeof( struct mo_bank_po_c ) );
	memset( &sMoBankAccPo, 0x00, sizeof( struct mo_bank_acc_po_c ) );

	pub_base_sysinit();

	get_zd_data( "0580", cPoNo );		/* �жһ�Ʊ���� */
	vtcp_log("[%s][%d]58����ĳжһ�Ʊ������[%s]\n",__FILE__,__LINE__,cPoNo);
	get_zd_data( "0720", flag );		/* �жһ�Ʊ���� */

	/* �������жһ�Ʊ�����Ƿ������ͷ */
	if(flag[0]!='1')
	ret = pub_com_ChkNoteHead1( "111", cPoNo );
	if( ret )
	{
		sprintf( acErrMsg, "���жһ�Ʊ�������![%d]", ret );
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("[%s][%d]582����ĳжһ�Ʊ������[%s]\n",__FILE__,__LINE__,cPoNo);

	/* ȡ�жһ�Ʊ����ͷ */
	strncpy( cPoNoHead, cPoNo, 8 );
	cPoNoHead[8]='\0';
	pub_base_strpack( cPoNoHead );

	/* ȡ�жһ�Ʊ��Чλ */
	strncpy( cPoNo1, cPoNo+8, 8 );
	cPoNo1[8] = '\0';
sprintf( acErrMsg, "xxxx    cPoNo1[%s]", cPoNo1 );
WRITEMSG

	/* �жһ�Ʊ�������ͷ+��Чλ */
	sprintf( cPoNo2, " %s%s ", cPoNoHead, cPoNo1 );
sprintf( acErrMsg, "xxxx    cPoNo2[%s]", cPoNo2 );
WRITEMSG
	pub_base_strpack_all( cPoNo2 );

	/* ���ݳжһ�Ʊ����������гжһ�Ʊ�ǼǱ� */
	ret = Mo_bank_po_Sel( g_pub_tx.reply, &sMoBankPo, "po_no='%s'", cPoNo2 );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "�޴˳жһ�Ʊ!" );
		WRITEMSG
		sprintf( g_pub_tx.reply, "B272" );
		goto ErrExit;
       	}else if( ret )
       	{
       		sprintf( acErrMsg, "ִ��Mo_bank_po_Sel��![%d]", ret );
                WRITEMSG
                goto ErrExit;
       	}

	/* ���ݳж�Э���Ų������гжһ�Ʊ������Ϣ�ǼǱ� */
	ret = Mo_bank_acc_po_Sel( g_pub_tx.reply, &sMoBankAccPo, "pact_no='%s'", sMoBankPo.pact_no );
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

	/* ��ǰ̨��Ļ��ֵ */	
	set_zd_data( "0260", sMoBankAccPo.payer_name );		/* ������ȫ�� */
	set_zd_data( "0380", sMoBankAccPo.payer_ac_no );	/* �������ʺ� */
	set_zd_data( "0300", sMoBankAccPo.payer_br_no );	/* �����˿������к� */
	set_zd_data( "0250", sMoBankAccPo.payer_br_name );	/* �����˿��������� */
	set_zd_data( "0270", sMoBankAccPo.payee_name );		/* �տ���ȫ�� */
	set_zd_data( "0830", sMoBankAccPo.payee_ac_no );	/* �տ����˺� */
	set_zd_data( "0810", sMoBankAccPo.payee_br_name );	/* �տ��˿��������� */
	set_zd_double( "1001", sMoBankAccPo.sign_amt );		/* ǩ��Ʊ���� */
	set_zd_long( "0440", sMoBankAccPo.matr_date );		/* �������� */
	set_zd_double( "1003", sMoBankAccPo.bail_amt );		/* ��֤���� */
	set_zd_double( "1002", sMoBankAccPo.tot_mort_amt );	/* ��Ѻ�ܽ�� */
	set_zd_data( "0320", sMoBankAccPo.bail_ac_no );		/* ��֤���ʺ� */
	set_zd_data( "0670", sMoBankAccPo.acc_type);		/* ��Ʊ���� */
	set_zd_double( "0400", sMoBankAccPo.bail_amt_ratio );	/* ��֤����� */
	set_zd_long( "0450", sMoBankPo.receive_date );		/* �������� */

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
