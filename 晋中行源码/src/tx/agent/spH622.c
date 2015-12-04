/***************************************************************
* �� �� ��: spH622.c
* ���������������˰ҵ���¼����(�����ֽ��ת�ʽɷѱ�־)
*
* ��    ��: jack
* ������ڣ�2005��03��29��
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��: jack
* �޸�����: 2004��06��25��
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "pay_agent_c.h"
#include "ag_paytlcs_c.h"
#include "ag_unitinfo_c.h"
#include "ag_peritemmap_c.h"

spH622()
{
	int ret = 0;
	int num = 0;
	char cItem_no[21];
	char cUnit_no[5];
	char cAc_no[20];

	char cFlag[2];

	char cDate[9];
	char cJfny[7];

	char filename[64];
	FILE *fp;
	char cCt_ind[5];

	struct ag_unitinfo_c sAg_unitinfo;	/*��λ��Ϣ*/
	struct ag_paytlcs_c sAg_paytlcs;   /*�ɷ���ˮ��*/
	struct ag_peritemmap_c sAg_peritemmap;	/*������Ϣ��*/

	struct dd_mst_c sDd_mst;
	struct mdm_ac_rel_c sMdm_ac_rel;

	memset(cItem_no,0x0,sizeof(cItem_no));
	memset(cUnit_no,0x0,sizeof(cUnit_no));
	memset( cAc_no, 0x0, sizeof(cAc_no) );
	memset(cDate,0x0,sizeof(cDate));
	memset(cJfny,0x0,sizeof(cJfny));

	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();

	get_zd_data( "0920", cUnit_no );
	get_zd_data( "0630", cItem_no );
	get_zd_data( "0330", cAc_no);
	pub_base_old_acno( cAc_no );
	set_zd_data( "0330", cAc_no);	

	sprintf(cDate, "%ld", g_pub_tx.tx_date);
	memcpy( cJfny, cDate, 6 );

	memset( &sAg_unitinfo, 0x0, sizeof(sAg_unitinfo) );
	ret = Ag_unitinfo_Sel( g_pub_tx.reply, &sAg_unitinfo, \
			"unit_no = '%s' and unit_stat = '0'", cUnit_no );
	if ( ret )
	{
		sprintf( acErrMsg, "��ѯ����λ��Ϣ����[%s]!!!", cUnit_no );
		WRITEMSG
		goto ErrExit;
	}

	memset( &sAg_peritemmap, 0x0, sizeof(sAg_peritemmap) );
	ret = Ag_peritemmap_Sel( g_pub_tx.reply, &sAg_peritemmap, \
			"item_no = '%s' and unit_no = '%s'", cItem_no, cUnit_no );
	if ( ret )
	{
		sprintf( acErrMsg, "��ѯ����������Ϣ����[%s]!!!", cItem_no );
		WRITEMSG
		goto ErrExit;
	}

	pub_base_strpack( sAg_peritemmap.ac_no );
	if ( strcmp( cAc_no, sAg_peritemmap.ac_no ) )
	{
		strcpy( g_pub_tx.reply, "H032" );
		sprintf( acErrMsg, "�������еĽ�˰�˺�,���������е��˺Ž�˰[%s]!!!", sAg_peritemmap.ac_no );
		WRITEMSG
		goto ErrExit;
	}

	pub_base_strpack( sAg_unitinfo.bank_acct_no );
	set_zd_data( "0310", sAg_unitinfo.bank_acct_no );

	ret = sql_execute( "update pay_agent set sata = '0', result = '0' \
			where unit_no = '%s' and item_no = '%s' and \
			result = '1'", cUnit_no, cItem_no );
	if ( ret )
	{
		sprintf(acErrMsg,"----------->�޸�δ�ɼ�¼״̬ʧ��![%s]", ret);
		WRITEMSG
		goto ErrExit;
	}

	memset(&sAg_paytlcs,0x0,sizeof(sAg_paytlcs));
	strcpy( sAg_paytlcs.unit_no, cUnit_no );
	sprintf( sAg_paytlcs.bank_tlcs, "%ld", g_pub_tx.trace_no );
	strcpy( sAg_paytlcs.batch_no, "0000" );
	strcpy( sAg_paytlcs.unit_tlcs, "0000" );
	strcpy( sAg_paytlcs.pre_no, "12345678" );
	get_zd_data( "0330", sAg_paytlcs.bank_acct_no );
	sprintf( sAg_paytlcs.trn_date, "%ld", g_pub_tx.tx_date );
	sprintf( sAg_paytlcs.trn_time, "%ld", g_pub_tx.tx_time );
	strcpy( sAg_paytlcs.br_no, g_pub_tx.tx_br_no );
	strcpy( sAg_paytlcs.tel_no, g_pub_tx.tel );
	get_zd_double( "0400", &sAg_paytlcs.trn_amt );
	get_zd_data( "0160", sAg_paytlcs.trcode );
	strcpy( sAg_paytlcs.item_no, cItem_no );
	strcpy( sAg_paytlcs.pay_month, cJfny );
	strcpy( sAg_paytlcs.pay_way, "0" );	/* 1-���� */
	strcpy( sAg_paytlcs.bill_flag, "0" );
	get_zd_data( "0610", sAg_paytlcs.bill_no );
	strcpy( sAg_paytlcs.stat, "0" );	/* ���� */

	ret = Ag_paytlcs_Ins( sAg_paytlcs, g_pub_tx.reply );
	if ( ret )
	{
		sprintf(acErrMsg,"����ɷ���ˮ��ʧ��![%s]", ret);
		WRITEMSG
		goto ErrExit;
	}

	memset( cFlag, 0x0, sizeof(cFlag) );
	get_zd_data( "0670", cFlag );

	if ( cFlag[0] == '1' )
	{
		set_zd_data( "102O", "1" );
	}

	memset( cCt_ind, 0x0, sizeof(cCt_ind) );
	memset( &sMdm_ac_rel, 0x0, sizeof(sMdm_ac_rel) );
	memset( &sDd_mst, 0x0, sizeof(sDd_mst) );
	if ( cFlag[0] == '0' )
	{
		strcpy( cCt_ind, "�ֽ�" );
	}
	else
	{
		strcpy( cCt_ind, "ת��" );
		ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdm_ac_rel, \
			"ac_no = '%s'", cAc_no );
		if ( ret )
		{
			sprintf(acErrMsg,"ִ��Mdm_ac_rel_Sel����!!!" );
			WRITEMSG
			goto ErrExit;
		}

		ret = Dd_mst_Sel( g_pub_tx.reply, &sDd_mst, \
			"ac_id = %ld", sMdm_ac_rel.ac_id );
		if ( ret )
		{
			sprintf(acErrMsg,"ִ��Dd_mst_Sel����!!!" );
			WRITEMSG
			goto ErrExit;
		}
	}

	memset( filename, 0x0, sizeof(filename) );
	pub_base_AllDwnFilName( filename );

	fp = fopen( filename, "w" );
	if ( fp == NULL )
	{
		sprintf(acErrMsg," open file [%s] error ",filename );
		WRITEMSG
		strcpy( g_pub_tx.reply,"S047" );
		goto ErrExit;
	}

	pub_base_strpack( sAg_peritemmap.name );
	pub_base_strpack( sDd_mst.name );
	fprintf( fp, "DLPT����˰��(%s):%s|%s|%lf|%ld|%s|%s|%s|%ld|%s|%s|\n", \
		cUnit_no, cItem_no, sAg_peritemmap.name, sAg_paytlcs.trn_amt, \
		g_pub_tx.tx_date, g_pub_tx.tel, cAc_no, sDd_mst.name, \
		g_pub_tx.trace_no, cCt_ind, g_pub_tx.tx_br_no );

	fclose( fp );

	if ( cFlag[0] == '0' )
	{
		set_zd_data( "101B", "1" );
		goto OkExit1;	/*	�ֽ�ɷ�	*/
	}
	else
	{
		set_zd_data( "101B", "2" );
		set_zd_data( "102K", "2" );
		set_zd_data( "102Y", "��˰" );
		goto OkExit2;	/*	ת�ʽɷ�	*/
	}

OkExit1:
	strcpy(g_pub_tx.reply,"0001");
	sprintf(acErrMsg,"------>�ֽ�ɷѼ�¼����ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
OkExit2:
	strcpy(g_pub_tx.reply,"0002");
	sprintf(acErrMsg,"------>ת�ʽɷѼ�¼����ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"------>�ɷѼ�¼����ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
