/*************************************************
* �� �� ��: gD010.c
* ���������������������˰����ʱ���
*
* ��    ��: jack
* �������: 2003��07��06��
* �޸�����:
* �� �� ��:
* �޸�����:
*
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;}
#define EXTERN
#include "public.h"
#include <errno.h>
#include "ag_unitinfo_c.h"
#include "ag_peritemmap_c.h"
#include "pay_agent_c.h"
#include "ag_paytlcs_c.h"

gD010()
{
	int ret=0;

	char cUnit_no[5];
	
	char filename[64];
	FILE *fp;

	int i=0;

	struct ag_unitinfo_c sAg_unitinfo;
	struct ag_peritemmap_c sAg_peritemmap;
	struct pay_agent_c sPay_agent;
	struct ag_paytlcs_c sAg_paytlcs;

	memset(cUnit_no,0x0,sizeof(cUnit_no));

	strcpy( cUnit_no, "1004" );		/* ��ʱ��Ϊ1004	*/

	ret = sql_begin();
	if ( ret )
	{
		sprintf( acErrMsg, "������ʧ��!!" );
		WRITEMSG
		goto ErrExit;
	}

	/** ��ʼ��ȫ�ֱ��� **/
	pub_base_sysinit();
	
	memset( &sAg_unitinfo, 0x0, sizeof(sAg_unitinfo) );
	ret=Ag_unitinfo_Sel( g_pub_tx.reply , &sAg_unitinfo ,\
			 "unit_no = '%s' and and pl_date = %ld and \
			 unit_stat = '0'", cUnit_no, g_pub_tx.tx_date );
	if( ret==100 )
	{
		sprintf( acErrMsg, "���շ�˰��λ[%s]����������", cUnit_no );
		WRITEMSG
		strcpy( g_pub_tx.reply, "0000" );
		return 0;
	}
	else if( ret )
	{
		sprintf( acErrMsg, "ִ��Ag_unitinfo����!ret=[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	memset( filename, 0x0, sizeof(filename) );
	sprintf( filename, "%s/Respondsyyh%ld002.txt", \
		getenv("FILDIR"), g_pub_tx.tx_date );

	fp = fopen( filename, "w" );
	if ( fp == NULL )
	{
		strcpy( g_pub_tx.reply, "S047" );
		sprintf( acErrMsg, "��д���۽���ļ���!!" );
		WRITEMSG
		goto ErrExit;
	}

	ret = Ag_paytlcs_Dec_Sel( g_pub_tx.reply , "unit_no = '%s' \
		and stat = '0' and ckmk_flag = '0' \
		order by trn_date,bank_tlcs", cUnit_no );
	if ( ret )
	{
		sprintf( acErrMsg, "ִ��Ag_paytlcs_Dec_Sel��!" );
		WRITEMSG
		goto ErrExit;
	}

	while (1)
	{
		memset( &sAg_paytlcs, 0x00, sizeof(sAg_paytlcs) );
		ret = Ag_paytlcs_Fet_Sel( &sAg_paytlcs, g_pub_tx.reply );
		if ( ret == 100 )
		{
			break;
		}
		else if ( ret )
		{
			sprintf( acErrMsg, "ִ��Ag_paytlcs_Fet_Sel��[%d]!", ret );
			WRITEMSG
			goto ErrExit;
		}

		memset( &sPay_agent, 0x0, sizeof(sPay_agent) );
		ret = Pay_agent_Sel( g_pub_tx.reply, &sPay_agent, \
			"unit_no = '%s' and item_no = '%s'", \
			cUnit_no, sAg_paytlcs.item_no );
		if ( ret )
		{
			sprintf( acErrMsg, "ִ��Pay_agent_Sel��[%d]!", ret );
			WRITEMSG
			goto ErrExit;
		}

		pub_base_strpack( sAg_paytlcs.item_no );
		pub_base_strpack( sPay_agent.name );
		pub_base_strpack( sAg_paytlcs.bank_acct_no );
		fprintf( fp, "%s\t%s\t\t%s\t00\t%s\t%lf\n", \
			sAg_paytlcs.item_no, sPay_agent.name, \
			sAg_paytlcs.bank_acct_no, sAg_paytlcs.trn_date, \
			sAg_paytlcs.trn_amt );

		ret = sql_execute( "update ag_paytlcs set ckmk_flag = '1' \
			where unit_no = '%s' and item_no = '%s' \
			and bank_tlcs = '%s'", cUnit_no, sAg_paytlcs.item_no, \
			sAg_paytlcs.bank_tlcs );
		if ( ret )
		{
			sprintf( acErrMsg, "�޸�������ˮ��¼�ж��ʱ�־ \
				����[%s][%s][%d]", cUnit_no, \
				sAg_paytlcs.item_no, ret );
			WRITEMSG
			goto ErrExit;
		}
	}

	fclose( fp );

	ret = sql_execute( "update ag_unitinfo set pl_date = 99999999 \
		where unit_no = '%s'", cUnit_no );
	if ( ret )
	{
		sprintf( acErrMsg, "�޸ĵ�λ[%s]��Ϣ���������ڳ���[%d]", \
			cUnit_no, ret );
		WRITEMSG
		goto ErrExit;
	}

	ret = sql_execute( "delete from pay_agent \
		where unit_no = '%s' and result = '0'", \
		cUnit_no );		/*----ɾ�������ѽɷѵļ�¼----*/
	if ( ret )
	{
		sprintf( acErrMsg, "ɾ����λ[%s]δ�ɼ�¼ʧ��!!!", cUnit_no );
		WRITEMSG
		goto ErrExit;
	}

OkExit:
	sql_commit();
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"----->����˰����ʱ���ɹ���[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
	fclose( fp );
	sql_rollback();
    sprintf(acErrMsg,"----->����˰����ʱ���ʧ�ܣ�[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
