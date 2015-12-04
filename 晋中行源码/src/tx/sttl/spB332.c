/*************************************************
* �� �� ��:  spB332.c
* ���������� ������гжһ�Ʊ��Ϣ��˹���
*
* ��    ��:  jane
* ������ڣ� 2003��5��25��
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
#include "mo_bank_acc_po_c.h"

int spB332()
{
	struct	mo_bank_acc_po_c 	sMoBankAcc;	/* �жһ�Ʊ������Ϣ�Ǽǲ� */
	struct	mdm_ac_rel_c 		sMdmAcRel;	/* �����˺Ŷ��ձ� */
	struct	dd_mst_c 		sDdMst;		/* �������ļ� */
	char	cPayeeName[51];				/* �տ���ȫ�� */
	/****Modified by SSH,2006.12.6,�ŵ�33λ,��ʱͨ��****/
	char	cPayeeAcNo[33];				/* �տ����˺� */
	char	cPayeeBrName[41];			/* �տ��˿��������� */
	int	ret;

	memset( &sMoBankAcc, 0x00, sizeof( struct mo_bank_acc_po_c ) );
	memset( &sMdmAcRel, 0x00, sizeof( struct mdm_ac_rel_c ) );
	memset( &sDdMst, 0x00, sizeof( struct dd_mst_c ) );
	memset( cPayeeName, 0x00, sizeof( cPayeeName ) );
	memset( cPayeeAcNo, 0x00, sizeof( cPayeeAcNo ) );
	memset( cPayeeBrName, 0x00, sizeof( cPayeeBrName ) );

	/* ���ݳ�ʼ�� */
	pub_base_sysinit();

	/* ��ǰ̨��Ļȡֵ */
	get_zd_data( "0330", sMoBankAcc.pact_no );	/* �жһ�ƱЭ���� */
	get_zd_data( "0270", cPayeeName );		/* �տ���ȫ�� */
	get_zd_data( "0830", cPayeeAcNo );		/* �տ����˺� */
	get_zd_data( "0810", cPayeeBrName );		/* �տ��˿��������� */

	/* ���ݳж�Э���Ų������гжһ�Ʊ������Ϣ�ǼǱ� */
	ret = Mo_bank_acc_po_Sel( g_pub_tx.reply, &sMoBankAcc, "pact_no='%s'", sMoBankAcc.pact_no );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "�˳ж�Э���Ų�����!" );
                WRITEMSG
                strcpy( g_pub_tx.reply, "B199" );
                goto ErrExit;
       	}else if( ret )
       	{
       		sprintf( acErrMsg, "ִ��Mo_bank_acc_po_Sel��![%d]", ret );
                WRITEMSG
                goto ErrExit;
       	}

	if( sMoBankAcc.acc_po_sts[0] == '1' )
	{
		sprintf( acErrMsg, "�˳ж�Э�������!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B200" );
		goto ErrExit;
	}else if( sMoBankAcc.acc_po_sts[0] == '2' )
	{
		sprintf( acErrMsg, "�˱ʳж���ǩ��!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B201" );
		goto ErrExit;
	}else if( sMoBankAcc.acc_po_sts[0] == '3' )
	{
		sprintf( acErrMsg, "�˱ʳж�������Ʊ�޸�!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B250" );
		goto ErrExit;
	}

	/* ����տ�����Ϣ�����Ķ�������ݳж�Э�����޸ĳжһ�Ʊ������Ϣ�ǼǱ� */
	ret = Mo_bank_acc_po_Dec_Upd( g_pub_tx.reply, "pact_no='%s' and acc_po_sts='%s'", \
				sMoBankAcc.pact_no, "0" );
	if ( ret )
	{
		sprintf( acErrMsg, "ִ��Mo_bank_acc_po_Dec_Upd��[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	ret = Mo_bank_acc_po_Fet_Upd( &sMoBankAcc, g_pub_tx.reply );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "�˳ж�Э���Ų�����!" );
                WRITEMSG
                strcpy( g_pub_tx.reply, "B199" );
                goto ErrExit;
       	}else if( ret )
       	{
       		sprintf( acErrMsg, "ִ��Mo_bank_acc_po_Fet_Upd��![%d]", ret );
                WRITEMSG
                goto ErrExit;
       	}

	/* 2008/12/9 17:01 add by martin ��˲�����¼��Ϊͬһ����*/
	if(!strncmp(sMoBankAcc.tx_br_no,g_pub_tx.tel,sizeof(g_pub_tx.tel)-1))
		{
			sprintf(acErrMsg,"¼��͸��˲���ͬһ������Ա!");
      WRITEMSG
      strcpy( g_pub_tx.reply,"P080");
      goto ErrExit;
		}
	strcpy( sMoBankAcc.tx_br_no, g_pub_tx.tx_br_no );	/* ���׻��� */
	
	/* �޸ĳжһ�Ʊ������Ϣ�Ǽǲ���״̬Ϊ���*/
	if( strcmp( cPayeeName, sMoBankAcc.payee_name ) != 0 )
		strcpy( sMoBankAcc.payee_name, cPayeeName );
	if( strcmp( cPayeeAcNo, sMoBankAcc.payee_ac_no ) != 0 )
		strcpy( sMoBankAcc.payee_ac_no, cPayeeAcNo );
	if( strcmp( cPayeeBrName, sMoBankAcc.payee_br_name ) != 0 )
		strcpy( sMoBankAcc.payee_br_name, cPayeeBrName );

	strcpy( sMoBankAcc.acc_po_sts, "1" );			/* ��־��1 ��� */

	ret = Mo_bank_acc_po_Upd_Upd( sMoBankAcc, g_pub_tx.reply );
	if ( ret )
	{
		sprintf( acErrMsg, "�޸ĳжһ�Ʊ������Ϣ�ǼǱ���[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	Mo_bank_acc_po_Clo_Upd();

	/* ����ˮ��־ */
	/**----- rob ---- 20030819 ---- ��������Ҫ��Э���� ----**/
	strcpy(g_pub_tx.ac_no,sMoBankAcc.pact_no);
	g_pub_tx.tx_amt1=0.00;
	strcpy( g_pub_tx.brf, "���гжһ�Ʊ����" );	/* ��ע */
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
