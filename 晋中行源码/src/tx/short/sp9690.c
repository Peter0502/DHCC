/*************************************************
* �� �� ��:  sp9690.c
* ���������� ������Э���Ż������ֵǼǱ���Ϣ
*
* ��    ��:  rob 
* ������ڣ� 2003��9��27��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "mo_discnt_c.h"

int sp9690()
{
	struct	mo_discnt_c	sMoDiscnt;	/* ���ֵǼǱ� */
	struct	mdm_ac_rel_c	sMdmAcRel;	/* �������˻���ϵ�� */
	int	ret;

	memset( &sMoDiscnt, 0x00, sizeof( struct mo_discnt_c ) );
	memset( &sMdmAcRel, 0x00, sizeof( struct mdm_ac_rel_c ) );
	
	get_zd_data( "0630", sMoDiscnt.pact_no );	/* ����Э���� */
vtcp_log( "zy   pact_no[%s]",sMoDiscnt.pact_no );
	/* ������ֵǼǱ����Ƿ���ڴ�����Э���� */
	ret = Mo_discnt_Sel( g_pub_tx.reply, &sMoDiscnt, "pact_no='%s'", sMoDiscnt.pact_no );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "���ֵǼǱ��в����ڴ�����Э����!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B113" );
		goto ErrExit;
   	}else if( ret )
   	{
   		sprintf( acErrMsg, "ִ��Mo_discnt_Sel��![%d]", ret );
		WRITEMSG
		goto ErrExit;
   	}
vtcp_log( "zy    ac_id[%ld]  ac_seqn[%d]",sMoDiscnt.ac_id, sMoDiscnt.ac_seqn );
	/* ��ѯ�����ʺ� *
	ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdmAcRel, "ac_id=%ld and ac_seqn=%d",\
				sMoDiscnt.ac_id, sMoDiscnt.ac_seqn );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "�����ʺŲ�����!" );
   		WRITEMSG
   		strcpy( g_pub_tx.reply, "B242" );
		goto OkExit;
	}else if( ret )
	{
		sprintf( acErrMsg, "ִ��Mdm_ac_rel_Sel��![%d]", ret );
       	WRITEMSG
       	goto ErrExit;
	}*/

	/* ��ǰ̨��Ļ��ֵ */
/*
	if( sMoDiscnt.pn_type[0] == '2' || sMoDiscnt.pn_type[0] == '4' )	* ��������  *
		set_zd_data( "0690", "1" );			* ���ת�루ת���� *
	if( sMoDiscnt.pn_type[0] == '3' || sMoDiscnt.pn_type[0] == '5' )	* ��������  *
		set_zd_data( "0690", "2" );			* �ع�ת�루ת���� */
    	set_zd_double( "1002", sMoDiscnt.par_amt );	/* ��Ʊ��� */
	set_zd_double( "0840", sMoDiscnt.pn_rate );	/* ������ */
	set_zd_double( "0850", sMoDiscnt.tpn_rate );	/* ת������ */
	set_zd_double( "1005", sMoDiscnt.rtpn_rate );	/* �������� */
	set_zd_long( "0450", sMoDiscnt.not_matr_date );/* ��Ʊ������ */
	set_zd_data( "0310", sMoDiscnt.rem_ac_no );	/* ��Ʊ���ʺ� */
	set_zd_data( "0260", sMoDiscnt.remitter );	/* ��Ʊ��ȫ�� */
	set_zd_data( "0810", sMoDiscnt.payer_brno_name );/* ������ȫ�� */
	set_zd_data( "0320", sMoDiscnt.payee_ac_no );	/* �տ����ʺ� */
	set_zd_data( "0270", sMoDiscnt.payee );		/* �տ���ȫ�� */
	set_zd_data( "0820", sMoDiscnt.payee_brno_name );/* �տ��˿�����ȫ�� */
	set_zd_data( "0900", sMoDiscnt.note_type );	/* Ʊ������ */
	set_zd_data( "0600", sMoDiscnt.pnote_no );	/* Ʊ�ݺ� */
	set_zd_data( "0670", sMoDiscnt.ac_type );	/* �տ��˻����� */
	set_zd_data( "0830", sMoDiscnt.pn_ac_no );	/* �����������˺� */
	set_zd_data( "0250", sMoDiscnt.pn_name );	/* ���������˻��� */
	set_zd_data( "0680", sMoDiscnt.local_ind );	/* ���ͬ�Ǳ�־ */
	set_zd_long( "0480", sMoDiscnt.way_days );	/* ��;���� */
	set_zd_double( "1001", sMoDiscnt.pn_int );	/* ������Ϣ */
	set_zd_long( "0440", sMoDiscnt.pn_date );	/* ת�������� */
	set_zd_long( "0460", sMoDiscnt.return_date );	/* �ع����� */
	set_zd_data( "0330", sMdmAcRel.ac_no );		/* �����˺� */

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
