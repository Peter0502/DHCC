/***************************************************************
* �� �� ��: spB314.c
* ������������Ʊ�˻��ӽ���
*
* ��    ��: jane
* ������ڣ�2003��11��10��
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
#include "mbfe_pub.h"
#include "com_parm_c.h"
#include "mo_cashier_c.h"

spB314()
{
	struct mo_cashier_c sMoCashier;
	struct com_parm_c sComParm;

	double sign_amt,tmp_amt;
	char requ_ac_no[21];

	memset( &sMoCashier, 0x00, sizeof( struct mo_cashier_c ) );
	memset( &sComParm, 0x00, sizeof( struct com_parm_c ) );

	pub_base_sysinit();

	get_zd_data( "0580", sMoCashier.cashier_no );
	get_zd_double( "0390", &sign_amt );
	get_zd_long( "0440", &sMoCashier.sign_date );
	get_zd_data( "0310", requ_ac_no );

	/*��鱾Ʊ��״̬����ʧ����Ʊ���*/
	g_reply_int = Mo_cashier_Dec_Upd( g_pub_tx.reply, "cashier_no='%s' and sign_date='%ld'", 
		sMoCashier.cashier_no, sMoCashier.sign_date );
	if( g_reply_int != 0 )
	{
		sprintf( acErrMsg, "��ѯ��Ʊ�Ǽǲ�����![%d]", g_reply_int );
		MBFEWRITEMSG
		goto ErrExit;
	}
	g_reply_int = Mo_cashier_Fet_Upd( &sMoCashier, g_pub_tx.reply );
	if( g_reply_int == 100 )
	{
		sprintf( acErrMsg, "��Ʊ���벻����!" );
		MBFEWRITEMSG
		vtcp_log("cashier_no[%s]", sMoCashier.cashier_no );
		goto ErrExit;
	}
	else if( g_reply_int != 0 )
	{
		sprintf( acErrMsg, "��ѯ��Ʊ�Ǽǲ�����![%d]", g_reply_int );
		MBFEWRITEMSG
		goto ErrExit;
	}

	if( sMoCashier.cashier_sts[0] == '*' )
	{
		sprintf( acErrMsg, "�ñ�Ʊ�Ѻ���!" );
		MBFEWRITEMSG
		goto ErrExit;
	}
	else if( sMoCashier.cashier_sts[0] == '0' )
	{
		sprintf( acErrMsg, "�ñ�Ʊ��δǩ��!" );
		MBFEWRITEMSG
		goto ErrExit;
	}
	else if( sMoCashier.cashier_sts[0] != '1' )
	{
		sprintf( acErrMsg, "�ñ�Ʊ״̬����,����ϵ���Ļ���![%s]", sMoCashier.cashier_sts );
		MBFEWRITEMSG
		goto ErrExit;
	}

	if( strcmp( g_pub_tx.tx_br_no, sMoCashier.sign_br_no ) != 0 )
	{
		sprintf( acErrMsg, "�Ǳ���ǩ����Ʊ�����˻�!" );
		MBFEWRITEMSG
		goto ErrExit;
	}

	if( sMoCashier.los_rpt_ind[0] == '1' )
	{
		sprintf( acErrMsg, "�ñ�Ʊ�ѹ�ʧ!" );
		MBFEWRITEMSG
		goto ErrExit;
	}
	else if( sMoCashier.los_rpt_ind[0]!='0' && sMoCashier.los_rpt_ind[0]!='2' )
	{
		sprintf( acErrMsg, "�ñ�Ʊ��ʧ״̬����,����ϵ���Ļ���![%s]", sMoCashier.los_rpt_ind );
		MBFEWRITEMSG
		goto ErrExit;
	}

	g_reply_int = pub_base_CompDblVal( sign_amt, sMoCashier.sign_amt );
	if( g_reply_int )
	{
		sprintf( acErrMsg, "��Ʊ�������!" );
		MBFEWRITEMSG
		vtcp_log("cli_val:%lf, db_val:%lf", sign_amt, sMoCashier.sign_amt);
		goto ErrExit;
	}

	pub_base_strpack( sMoCashier.requ_ac_no );
	pub_base_strpack( requ_ac_no );
	if( strcmp( sMoCashier.requ_ac_no, requ_ac_no ) != 0 )
	{
		sprintf( acErrMsg, "�������ʺ�����!" );
		MBFEWRITEMSG
		vtcp_log("scr_val:%s, db_val:%s", requ_ac_no, sMoCashier.requ_ac_no);
		goto ErrExit;
	}

	sMoCashier.can_date = g_pub_tx.tx_date;    /*��������*/
	sMoCashier.can_trace_no = g_pub_tx.trace_no;    /*������ˮ��*/
	strcpy( sMoCashier.cashier_sts, "*" );    /*����״̬*/
	strcpy( sMoCashier.use_ind, "4" );    /*������ʽ��4-�˻�*/
	sMoCashier.cash_amt = 0.00;    /*�Ҹ����*/
	sMoCashier.rem_amt = sMoCashier.sign_amt;    /*�˻ؽ��*/

	get_zd_data( "0300", sMoCashier.rem_ac_no );
	pub_base_strpack( sMoCashier.rem_ac_no );
	if( strlen( sMoCashier.rem_ac_no ) == 0 )
	{
		strcpy( sMoCashier.rem_ac_no, sMoCashier.requ_ac_no );
	}

	g_reply_int = Mo_cashier_Upd_Upd( sMoCashier, g_pub_tx.reply );
	if( g_reply_int != 0 )
	{
		sprintf( acErrMsg, "���±��[%d]!", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}
	Mo_cashier_Clo_Upd();

	/*��D099�ǿͻ��ʸ�ֵ*/
	set_zd_data( "0191", "1" );
	set_zd_data( "0192", "0001" );
	set_zd_data( "0193", "1001" );
	set_zd_data( "1011", sMoCashier.rem_ac_no );
	set_zd_double( "1013", sMoCashier.rem_amt );
	set_zd_data( "1016", "��Ʊ�˻�" );
	set_zd_data( "101A", "01" );
	set_zd_data( "101B", "2" );

	/*��A016���252��ֵ*/
	memset( &sComParm, 0x00, sizeof( struct com_parm_c ) );
	g_reply_int = Com_parm_Sel( g_pub_tx.reply, &sComParm, "parm_code='BPKM' and parm_seqn='2'" );
	if( g_reply_int )
	{
		sprintf( acErrMsg, "��ѯ��Ʊǩ����Ŀ����[%d]!", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}
	strcpy( sMoCashier.rec_ac_no, sComParm.val );
	set_zd_data( "1201", sComParm.val );
	set_zd_data( "1204", "01" );
	set_zd_data( "1205", "2" );
	set_zd_data( "1206", "113" );
	set_zd_data( "1207", sMoCashier.cashier_no );
	set_zd_double( "1208", sMoCashier.sign_amt );
	set_zd_data( "120A", "��Ʊ�˻�" );

	/*�ǽ�����־Ϊ������*/
	strcpy( g_pub_tx.no_show, "1" );/*����*/
	strcpy( g_pub_tx.ac_no, sMoCashier.cashier_no );
	strcpy( g_pub_tx.brf, "��Ʊ�˻�" );
	g_reply_int = pub_ins_trace_log();
	if( g_reply_int )
	{
		sprintf( acErrMsg," �ǽ�����־����![%ld]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"���±�Ʊǩ���Ǽǲ��ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"���±�Ʊǩ���Ǽǲ�ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
