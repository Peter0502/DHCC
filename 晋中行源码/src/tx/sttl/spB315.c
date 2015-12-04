/***************************************************************
* �� �� ��: spB315.c
* ������������Ʊ��ʧ
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
#include "mo_cashier_los_rpt_c.h"

spB315()
{
	struct mo_cashier_c sMoCashier;
	struct mo_cashier_los_rpt_c sMoCashierLosRpt;
	struct com_parm_c sComParm;

	char deal_flag[2];    /*������־0-��ʧ1-���*/

	char los_rpt_id_type[2];
	char los_rpt_id_no[21];
	char los_rpt_name[51];

	memset( &sMoCashier, 0x00, sizeof( struct mo_cashier_c ) );
	memset( &sComParm, 0x00, sizeof( struct com_parm_c ) );
	memset( &sMoCashierLosRpt, 0x00, sizeof( struct mo_cashier_los_rpt_c ) );

	pub_base_sysinit();

	get_zd_data( "0670", deal_flag );
	get_zd_data( "0580", sMoCashier.cashier_no );
	get_zd_long( "0440", &sMoCashier.sign_date );

	get_zd_data( "0680", los_rpt_id_type );
	get_zd_data( "0620", los_rpt_id_no );
	get_zd_data( "0270", los_rpt_name );

	/*��鱾Ʊ��״̬*/
	g_reply_int = Mo_cashier_Sel( g_pub_tx.reply, &sMoCashier, "cashier_no='%s' and sign_date='%ld'", 
		sMoCashier.cashier_no, sMoCashier.sign_date );
	if( g_reply_int == 100 )
	{
		sprintf( acErrMsg, "��Ʊ���벻����!" );
		MBFEWRITEMSG
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
		sprintf( acErrMsg, "�Ǳ���ǩ����Ʊ�������˽���!" );
		MBFEWRITEMSG
		goto ErrExit;
	}

	if( deal_flag[0] == '0' )    /*��ʧ*/
	{
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
		/*���µǼǲ�*/
		g_reply_int = sql_execute( "update mo_cashier set los_rpt_date=%ld,los_rpt_trace_no=%ld,clos_rpt_date=0,los_rpt_trace_no=0,los_rpt_ind='1' where cashier_no='%s' and sign_date=%ld",
			g_pub_tx.tx_date, g_pub_tx.trace_no, sMoCashier.cashier_no, sMoCashier.sign_date );
		if( g_reply_int )
		{
			sprintf( acErrMsg, "���±�Ʊ�Ǽǲ�����![%ld]", g_reply_int );
			WRITEMSG
			goto ErrExit;
		}

		/*�Ǽǹ�ʧ��Ϣ*/
		sMoCashierLosRpt.los_rpt_date = g_pub_tx.tx_date;
		sMoCashierLosRpt.los_rpt_trace_no = g_pub_tx.trace_no;
		get_zd_data( "0680", sMoCashierLosRpt.los_rpt_id_type );
		get_zd_data( "0620", sMoCashierLosRpt.los_rpt_id_no );
		get_zd_data( "0270", sMoCashierLosRpt.los_rpt_name );
		sMoCashierLosRpt.sign_date = sMoCashier.sign_date;
		strcpy( sMoCashierLosRpt.los_rpt_br_no, g_pub_tx.tx_br_no );
		strcpy( sMoCashierLosRpt.cashier_no, sMoCashier.cashier_no );
		strcpy( sMoCashierLosRpt.sts, "0" );
		g_reply_int = Mo_cashier_los_rpt_Ins( sMoCashierLosRpt, g_pub_tx.reply );
		if( g_reply_int )
		{
			sprintf( acErrMsg, "�Ǽǹ�ʧ�Ǽǲ�����![%ld]", g_reply_int );
			WRITEMSG
			goto ErrExit;
		}
	}
	else if( deal_flag[0] == '1' )    /*���*/
	{
		if( sMoCashier.los_rpt_ind[0] != '1' )
		{
			sprintf( acErrMsg, "�ñ�Ʊδ��ʧ!" );
			MBFEWRITEMSG
			goto ErrExit;
		}
		/*������*/
		g_reply_int = Mo_cashier_los_rpt_Sel( g_pub_tx.reply, &sMoCashierLosRpt, 
			"cashier_no='%s' and sign_date='%ld' and sts='0'", sMoCashier.cashier_no, sMoCashier.sign_date );
		if( g_reply_int )
		{
			sprintf( acErrMsg, "��ѯ��Ʊ��ʧ�Ǽǲ�����![%ld]", g_reply_int );
			WRITEMSG
			goto ErrExit;
		}
		pub_base_strpack( los_rpt_id_no );
		pub_base_strpack( los_rpt_name );
		pub_base_strpack( sMoCashierLosRpt.los_rpt_id_no );
		pub_base_strpack( sMoCashierLosRpt.los_rpt_name );
		if( strcmp( sMoCashierLosRpt.los_rpt_id_type, los_rpt_id_type ) != 0 )
		{
			strcpy( g_pub_tx.reply, "P210" );
			sprintf( acErrMsg, "֤�����Ͳ���ȷ" );
			WRITEMSG
			vtcp_log("cli_val:%s, db_val:%s", los_rpt_id_type, sMoCashierLosRpt.los_rpt_id_type );
			goto ErrExit;
		}
		if( strcmp( sMoCashierLosRpt.los_rpt_id_no, los_rpt_id_no ) != 0 )
		{
			strcpy( g_pub_tx.reply, "M004" );
			sprintf( acErrMsg, "֤�����벻��ȷ" );
			WRITEMSG
			vtcp_log("cli_val:%s, db_val:%s", los_rpt_id_no, sMoCashierLosRpt.los_rpt_id_no );
			goto ErrExit;
		}
		if( strcmp( sMoCashierLosRpt.los_rpt_name, los_rpt_name ) != 0 )
		{
			sprintf( acErrMsg, "�������������Ͳ���ȷ!" );
			MBFEWRITEMSG
			vtcp_log("cli_val:%s, db_val:%s", los_rpt_name, sMoCashierLosRpt.los_rpt_name );
			goto ErrExit;
		}

		/*���µǼǲ�*/
		g_reply_int = sql_execute( "update mo_cashier set clos_rpt_date=%ld,clos_rpt_trace_no=%ld,los_rpt_ind='2' where cashier_no='%s' and sign_date=%ld",
			g_pub_tx.tx_date, g_pub_tx.trace_no, sMoCashier.cashier_no, sMoCashier.sign_date );
		if( g_reply_int )
		{
			sprintf( acErrMsg, "���±�Ʊ�Ǽǲ�����![%ld]", g_reply_int );
			WRITEMSG
			goto ErrExit;
		}

		/*���¹�ʧ�Ǽǲ�*/
		g_reply_int = sql_execute( "update mo_cashier_los_rpt set clos_rpt_date=%ld,clos_rpt_trace_no=%ld,sts='1' where cashier_no='%s' and sign_date=%ld and sts='0'",
			g_pub_tx.tx_date, g_pub_tx.trace_no, sMoCashier.cashier_no, sMoCashier.sign_date );
		if( g_reply_int )
		{
			sprintf( acErrMsg, "���±�Ʊ�Ǽǲ�����![%ld]", g_reply_int );
			WRITEMSG
			goto ErrExit;
		}
	}
	else
	{
		sprintf( acErrMsg, "������־����![%s]", deal_flag );
		MBFEWRITEMSG
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
