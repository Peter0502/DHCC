/***************************************************************
* �� �� ��: spB311.c
* ������������Ʊǩ����Ϣ¼���ӽ���
*           �ǼǱ�Ʊǩ���Ǽǲ�(mo_cashier)
*
* ��    ��: jane
* ������ڣ�2003��11��07��
*
* �޸ļ�¼��
*    ��    ��:
*    �� �� ��: 
*    �޸�����: 
*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "com_parm_c.h"
#include "mo_cashier_c.h"

spB311()
{
	struct mo_cashier_c sMoCashier;
	struct com_parm_c com_parm_v;

	memset( &sMoCashier, 0x00, sizeof( struct mo_cashier_c ) );

	pub_base_sysinit();


	if( get_serialno_val( "14", sMoCashier.pact_no ) ) goto ErrExit;    /*ҵ����*/
	strcpy( sMoCashier.tx_br_no, g_pub_tx.tx_br_no );    /*¼�������*/
	sMoCashier.tx_date = g_pub_tx.tx_date;    /*¼������*/
	get_zd_data( "0210", sMoCashier.ct_ind );    /*��ת����*/
	get_zd_data( "0690", sMoCashier.ration_ind );    /*��������*/
	get_zd_data( "0300", sMoCashier.requ_ac_no );    /*�������ʺ�*/
	get_zd_data( "0250", sMoCashier.requ_name );    /*����������*/
	get_zd_data( "0962", sMoCashier.requ_addr );    /*�����˵�ַ*/
	get_zd_data( "0810", sMoCashier.payee_ac_no );    /*�տ����ʺ�*/
	get_zd_data( "0961", sMoCashier.payee_name );    /*�տ�������*/
	get_zd_double( "0390", &sMoCashier.sign_amt );    /*��Ʊ���*/
	get_zd_data( "0370", sMoCashier.rmk );    /*��ע*/
	sMoCashier.rec_trace_no = g_pub_tx.trace_no;    /*¼����ˮ��*/
	strcpy( sMoCashier.rec_tel, g_pub_tx.tel );    /*¼�����Ա*/
	strcpy( sMoCashier.cashier_sts, "0" );    /*��Ʊ״̬*/
	strcpy( sMoCashier.use_ind, "0" );    /*����״̬*/
	strcpy( sMoCashier.los_rpt_ind, "0" );    /*��ʧ״̬*/

	sprintf( acErrMsg, "mo_cashier: N pact_no[%s] N tx_br_no[%s] tx_date[%ld] sign_amt[%lf] ct_ind[%s] ration_ind[%s] N requ_ac_no[%s] N requ_name[%s] requ_addr[%s] payee_ac_no[%s] payee_name[%s] rmk[%s] rec_trace_no[%ld] rec_tel[%s] N cashier_sts[%s] N use_ind[%s] N los_rpt_ind[%s]", 
		sMoCashier.pact_no, sMoCashier.tx_br_no, sMoCashier.tx_date, 
		sMoCashier.sign_amt, sMoCashier.ct_ind, sMoCashier.ration_ind, 
		sMoCashier.requ_ac_no, sMoCashier.requ_name, sMoCashier.requ_addr, 
		sMoCashier.payee_ac_no, sMoCashier.payee_name, sMoCashier.rmk, 
		sMoCashier.rec_trace_no, sMoCashier.rec_tel, sMoCashier.cashier_sts, 
		sMoCashier.use_ind, sMoCashier.los_rpt_ind );
	WRITEMSG

	g_reply_int = Mo_cashier_Ins( sMoCashier, g_pub_tx.reply );
	if( g_reply_int )
   	{
       	sprintf( acErrMsg, "�ǼǱ�Ʊǩ���Ǽǲ�����![%d]", g_reply_int );
       	WRITEMSG
       	goto ErrExit;
   	}

	set_zd_data( "0730", sMoCashier.pact_no );

	/*�����ӽ��״�ֵ*/
	memset( &com_parm_v, 0x00, sizeof( struct com_parm_c ) );
	g_reply_int = Com_parm_Sel( g_pub_tx.reply, &com_parm_v, "parm_code='BPKM' and parm_seqn='1'" );
	if( g_reply_int )
	{
		sprintf( acErrMsg, "��ѯ�������������[%d]!", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}
	set_zd_data( "1211", com_parm_v.val );    /*�˺�*/

	/*�ǽ�����־Ϊ������*/
	strcpy( g_pub_tx.no_show, "1" );/*����*/
	strcpy( g_pub_tx.ac_no, sMoCashier.pact_no );
	strcpy( g_pub_tx.add_ind, "1" );
	strcpy( g_pub_tx.brf, "��Ʊ��Ϣ¼��" );
	g_reply_int = pub_ins_trace_log();
	if( g_reply_int )
	{
		sprintf( acErrMsg," �ǽ�����־����![%ld]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"��Ʊ��Ϣ¼��ɹ�![%s]", g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"��Ʊ��Ϣ¼��ʧ��![%s]", g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
