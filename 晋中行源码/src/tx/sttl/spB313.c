/***************************************************************
* �� �� ��: spB313.c
* ������������Ʊ�����ӽ���(����)
*
* ��    ��: jane
* ������ڣ�2003��11��09��
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
#include "mo_city_c.h"

spB313()
{
	struct mo_cashier_c sMoCashier;
	struct mo_city_c sMoCity;
	struct com_parm_c sComParm;

	char deal_flag[2];    /*���巽ʽ*/
	char ct_ind[2];    /*��ת��־*/
	double sign_amt,tmp_amt;

	memset( &sMoCashier, 0x00, sizeof( struct mo_cashier_c ) );
	memset( &sComParm, 0x00, sizeof( struct com_parm_c ) );

	pub_base_sysinit();

	get_zd_data( "0690", deal_flag );
	get_zd_data( "0580", sMoCashier.cashier_no );
	get_zd_double( "0390", &sign_amt );

	/*��鱾Ʊ��״̬����ʧ����Ʊ���*/
	g_reply_int = Mo_cashier_Dec_Upd( g_pub_tx.reply, "cashier_no='%s'", sMoCashier.cashier_no );
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

	get_zd_double( "0400", &sMoCashier.cash_amt );
	get_zd_double( "0410", &sMoCashier.rem_amt );
	get_zd_data( "0320", sMoCashier.rem_ac_no );
	sMoCashier.can_date = g_pub_tx.tx_date;
	sMoCashier.can_trace_no = g_pub_tx.trace_no;
	strcpy( sMoCashier.cashier_sts, "*" );

	if(deal_flag[0]=='1')
	{
		strcpy( ct_ind, "2" );
		/*��鲢����ͬ�ǵǼǲ�*/
		strcpy( sMoCashier.use_ind, "1" );
		get_zd_data( "0300", sMoCashier.pay_ac_no );
		get_zd_double( "0400", &sMoCashier.cash_amt );
		memset( &sMoCity, 0x00, sizeof( struct mo_city_c ) );
		g_reply_int = Mo_city_Dec_Upd( g_pub_tx.reply, "payer_ac_no='%s' and bill_amt='%lf' and sts='C'",
			sMoCashier.pay_ac_no, sMoCashier.cash_amt );
		if( g_reply_int )
		{
			sprintf( acErrMsg, "�����α��[%d]!", g_reply_int );
			WRITEMSG
			vtcp_log("ac_no[%s], bill_amt[%lf]", sMoCashier.pay_ac_no, sMoCashier.cash_amt );
			goto ErrExit;
		}
		g_reply_int = Mo_city_Fet_Upd( &sMoCity, g_pub_tx.reply );
		if( g_reply_int == 100 )
		{
			sprintf( acErrMsg, "ͬ�ǵǼǲ��в����ڸñʼ�¼!" );
			MBFEWRITEMSG
			vtcp_log("ac_no[%s], bill_amt[%lf]", sMoCashier.pay_ac_no, sMoCashier.cash_amt );
			goto ErrExit;
		}
		else if( g_reply_int != 0 )
		{
			sprintf( acErrMsg, "ȡ�α��[%d]!", g_reply_int );
			WRITEMSG
			vtcp_log("ac_no[%s], bill_amt[%lf]", sMoCashier.pay_ac_no, sMoCashier.cash_amt );
			goto ErrExit;
		}

		strcpy( sMoCity.sts, "O" );

		g_reply_int = Mo_city_Upd_Upd( sMoCity, g_pub_tx.reply );
		if( g_reply_int )
		{
			sprintf( acErrMsg, "���±��[%d]!", g_reply_int );
			WRITEMSG
			goto ErrExit;
		}
		Mo_city_Clo_Upd();

		/*����40802��ֵ*/
		memset( &sComParm, 0x00, sizeof( struct com_parm_c ) );
		g_reply_int = Com_parm_Sel( g_pub_tx.reply, &sComParm, "parm_code='TCKM' and parm_seqn='3'" );
		if( g_reply_int )
		{
			sprintf( acErrMsg, "��ѯͬ�������Ŀ����[%d]!", g_reply_int );
			WRITEMSG
			goto ErrExit;
		}
		strcpy( sMoCashier.rec_ac_no, sComParm.val );
		set_zd_data( "1211", sComParm.val );
		set_zd_data( "1214", "01" );
		set_zd_data( "1215", "2" );
		set_zd_data( "1216", "113" );
		set_zd_data( "1217", sMoCashier.cashier_no );
		set_zd_double( "1218", sMoCashier.cash_amt );
		set_zd_data( "121A", "��Ʊ����" );

		strcpy(g_pub_tx.reply,"0001");
	}
	else if(deal_flag[0]=='2')
	{
		strcpy( ct_ind, "2" );
		strcpy( sMoCashier.use_ind, "2" );
		/*�ǿͻ��ʸ�ֵ*/
		get_zd_data( "0310", sMoCashier.rec_ac_no );

		set_zd_data( "0191", "1" );
		set_zd_data( "0192", "0001" );
		set_zd_data( "0193", "1001" );
		set_zd_data( "1011", sMoCashier.rec_ac_no );
		set_zd_double( "1013", sMoCashier.cash_amt );
		set_zd_data( "1016", "��Ʊ�Ҹ�" );
		set_zd_data( "101A", "01" );
		set_zd_data( "101B", "2" );

		strcpy(g_pub_tx.reply,"0002");
	}
	else if( deal_flag[0] == '3' )
	{
		strcpy( ct_ind, "1" );
		if( sMoCashier.ct_ind[0] != '1' )
		{
			sprintf( acErrMsg, "���ֽ�Ʊ,���ܶҸ��ֽ�" );
			MBFEWRITEMSG
			goto ErrExit;
		}

		g_reply_int = pub_base_CompDblVal( sMoCashier.rem_amt, 0.005 );
		if( g_reply_int > 0 )
		{
			sprintf( acErrMsg, "�ֽ�Ʊ��ȫ��Ҹ�" );
			MBFEWRITEMSG
			goto ErrExit;
		}
		
		strcpy( sMoCashier.use_ind, "3" );
		strcpy( sMoCashier.rec_ac_no, "101" );

		set_zd_data( "1191", "01" );
		set_zd_data( "1192", "113" );
		set_zd_data( "1193", sMoCashier.cashier_no );
		set_zd_double( "1194", sMoCashier.cash_amt );
		set_zd_data( "1197", "�ֽ�Ʊ�Ҹ�" );

		strcpy( g_pub_tx.reply, "0003" );
	}
	else
	{
		sprintf( acErrMsg, "�Ҹ����巽ʽ����![%s]", deal_flag );
		MBFEWRITEMSG
		goto ErrExit;
	}

	g_reply_int = Mo_cashier_Upd_Upd( sMoCashier, g_pub_tx.reply );
	if( g_reply_int != 0 )
	{
		sprintf( acErrMsg, "���±��[%d]!", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}
	Mo_cashier_Clo_Upd();

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
	set_zd_data( "1205", ct_ind );
	set_zd_data( "1206", "113" );
	set_zd_data( "1207", sMoCashier.cashier_no );
	set_zd_double( "1208", sMoCashier.sign_amt );
	set_zd_data( "120A", "��Ʊ����Ҹ�" );

	/*�ǽ�����־Ϊ������*/
	strcpy( g_pub_tx.no_show, "1" );/*����*/
	strcpy( g_pub_tx.beg_note_no, sMoCashier.cashier_no );
	strcpy( g_pub_tx.ac_no, sMoCashier.pay_ac_no );
	g_pub_tx.tx_amt1 = sMoCashier.cash_amt;
	strcpy( g_pub_tx.ct_ind, deal_flag );
	strcpy( g_pub_tx.brf, "��Ʊ����Ҹ�" );
	g_reply_int = pub_ins_trace_log();
	if( g_reply_int )
	{
		sprintf( acErrMsg," �ǽ�����־����![%ld]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

OkExit:
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
