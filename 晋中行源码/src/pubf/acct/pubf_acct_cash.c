/***************************************************************
* �� �� ��:     pubf_acct_cash.c
* ����������	�ֽ����롢����
*              1�����ݽ��׻����ź͹�Ա����ȡ��Ա�ֽ��˺ţ�
*              2���޸��ֽ�̨�ʣ�
*              3���Ǽ��ֽ���ϸ��
*
* ��    ��:	jack
* ������ڣ�    2003��12��27��
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
#include "cash_mst_c.h"
#include "cash_mst_hst_c.h"

pub_acct_cash()
{
	int ret=0;

	struct cash_mst_c	cash_mst_c;
	struct cash_mst_hst_c	cash_mst_hst_c;
	memset(&cash_mst_c,0x00,sizeof(struct cash_mst_c));
	memset(&cash_mst_hst_c,0x00,sizeof(struct cash_mst_hst_c));

	strcpy( g_pub_tx.ct_ind,"1" );
	strcpy( g_pub_tx.opn_br_no,g_pub_tx.tx_br_no );

	/*** ���ݽ��׻����ź͹�Ա����ȡ�ֽ��˺�ID ***/
	ret=Cash_mst_Dec_Upd(g_pub_tx.reply,
		" br_no='%s' and cur_no='%s' and tel='%s' ",
		g_pub_tx.tx_br_no,g_pub_tx.cur_no,g_pub_tx.tel);
	if( ret )
	{
		sprintf(acErrMsg,"Ǯ�����ļ��쳣���������Ļ�����ϵ!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P153");
		goto ErrExit;
	}

	ret=Cash_mst_Fet_Upd(&cash_mst_c,g_pub_tx.reply);
	vtcp_log("brno[%s] cur_no[%s] tel[%s]",g_pub_tx.tx_br_no,g_pub_tx.cur_no,g_pub_tx.tel);
	if( ret==100 )
	{
		sprintf(acErrMsg,"�˹�Ա��Ǯ��[%s][%s][%s]!",
			g_pub_tx.tx_br_no,g_pub_tx.cur_no,g_pub_tx.tel);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P154");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"Ǯ�����ļ��쳣���������Ļ�����ϵ!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P153");
		goto ErrExit;
	}

	/* �������Ƿ���ڣ��Ƿ����� */
	ret = pub_base_ChkCurno( g_pub_tx.cur_no );
	if ( ret )
	{
		sprintf( acErrMsg, "�����ִ�" );
		WRITEMSG
		goto ErrExit;
	}

	if( cash_mst_c.sts[0]!='0' )
	{
		sprintf(acErrMsg,"�ֽ��˺�δ���û�״̬������!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P150");
		goto ErrExit;
	}
	strcpy(g_pub_tx.ac_no,cash_mst_c.ac_no);

	if( g_pub_tx.add_ind[0]=='0' && g_pub_tx.tx_amt1-cash_mst_c.bal>=0.01)
	{
		sprintf(acErrMsg,"�ֽ���!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P152");
		goto ErrExit;
	}

    if (g_pub_tx.add_ind[0]=='0')
		cash_mst_c.bal=cash_mst_c.bal-g_pub_tx.tx_amt1;
	else if( g_pub_tx.add_ind[0]=='1' )
		cash_mst_c.bal=cash_mst_c.bal+g_pub_tx.tx_amt1;
	else
	{
		sprintf(acErrMsg,"������־����![%s]",g_pub_tx.add_ind);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P106");
		goto ErrExit;
	}

	ret=Cash_mst_Upd_Upd(cash_mst_c,g_pub_tx.reply);
	if( ret )
	{
		sprintf(acErrMsg,"Ǯ�����ļ��쳣���������Ļ�����ϵ!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P153");
		goto ErrExit;
	}
	Cash_mst_Clo_Upd();

	strcpy( g_pub_tx.dc_code,NULL_DC_CODE );

	/*** �Ǽ�Ǯ����ϸ ***/
	cash_mst_hst_c.trace_no=g_pub_tx.trace_no;
    vtcp_log("B TRACE [%d]",g_pub_tx.trace_no );
	cash_mst_hst_c.trace_cnt=g_pub_tx.trace_cnt;
	strcpy(cash_mst_hst_c.br_no,g_pub_tx.tx_br_no);
	strcpy(cash_mst_hst_c.ac_no,g_pub_tx.ac_no);
	strcpy(cash_mst_hst_c.tw_ac_no,g_pub_tx.name);
	strcpy(cash_mst_hst_c.tel,g_pub_tx.tel);
	strcpy(cash_mst_hst_c.cur_no,g_pub_tx.cur_no);
	strcpy(cash_mst_hst_c.add_ind,g_pub_tx.add_ind);
	cash_mst_hst_c.amt=g_pub_tx.tx_amt1;
	cash_mst_hst_c.tx_date=g_pub_tx.tx_date;
	cash_mst_hst_c.tx_time=g_pub_tx.tx_time;
	strcpy(cash_mst_hst_c.brf,g_pub_tx.brf);
	strcpy(cash_mst_hst_c.cash_code,g_pub_tx.cash_code);
	ret=Cash_mst_hst_Ins(cash_mst_hst_c,g_pub_tx.reply);
	if( ret )
	{
		sprintf(acErrMsg,"�Ǽ�Ǯ����ϸ����![%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P155");
		goto ErrExit;
	}

	if( pub_ins_trace_log() )
	{
		sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
		WRITEMSG
		goto ErrExit;
	}

	set_zd_data(DC_CODE,g_pub_tx.dc_code);

OKExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"�ֽ���ɹ�!");
	WRITEMSG
	return 0;
	
ErrExit:
	sprintf(acErrMsg,"�ֽ���ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	return 1;
}
