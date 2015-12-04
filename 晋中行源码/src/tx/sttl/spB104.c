/*************************************************
* �� �� ��:  spB104.c
* ���������� ������л�Ʊ�Ҹ����ܣ����пͻ��������пͻ�
*		Я����Ʊ�����жҸ���Ʊ
*
* ��    ��:  Terry
* ������ڣ� 2003��1��19��
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
#include "mo_bank_po_c.h"

int spB104()
{
	struct	mo_bank_po_c	sMoBankPo;	/* ���л�Ʊ�Ǽǲ� */
	char	cOpenBrno[9];			/* �ʺſ������� */
	char	cFlag[2];			/* �Ƿ��Ǳ��б�־ */
	long	lAcId;				/* �տ����ʺ�ID */
	int	num = 0;
	int	ret;
	
	/* ���ݳ�ʼ�� */
	memset( &sMoBankPo, 0x00, sizeof( struct mo_bank_po_c ) );
	memset( cOpenBrno, 0x00, sizeof( cOpenBrno ) );
	memset( cFlag, 0x00, sizeof( cFlag ) );
	
	pub_base_sysinit();

	/* ��ǰ̨��Ļȡֵ */
	get_zd_data( "0870", sMoBankPo.sign_br_no );	/* ǩ���к� */
	get_zd_data( "0590", sMoBankPo.po_no );		/* ��Ʊ���� */
	get_zd_data( "0670", sMoBankPo.ct_ind );	/* ��ת��־ */
	get_zd_long( "0440", &sMoBankPo.sign_date );	/* ǩ������ */
	get_zd_data( "0680", sMoBankPo.payee_id_type );	/* ֤������ */
	get_zd_data( "0620", sMoBankPo.payee_id_no );	/* ֤������ */
	get_zd_data( "0810", sMoBankPo.payee_name );	/* �տ���ȫ�� */
	get_zd_data( "0310", sMoBankPo.payee_ac_no );	/* �տ����ʺ� */
	get_zd_long( "0340", &sMoBankPo.payee_ac_seqn );	/* �տ����ʺ���� */
	get_zd_double( "0390", &sMoBankPo.sign_amt );	/* ǩ����� */
	get_zd_double( "0400", &sMoBankPo.amt );	/* ������ */
	sMoBankPo.bal_date = g_pub_tx.tx_date;		/* �������� */
	strcpy( sMoBankPo.po_sts, "4" );		/* ��Ʊ״̬ */
	strcpy( sMoBankPo.po_ind, "1" );		/* ��Ʊ��־ */
	
	get_zd_data( "0690", cFlag );			/* �տ����ʺ������� */
	
	/* ����տ����ʺ��Ǳ��У���Ҫ����ʺ��Ƿ���ڣ�״̬�Ƿ����� */
	if ( cFlag[0] == 'Y' )	/* �Ǳ��� */
	{
		ret = pub_base_ChkAc( sMoBankPo.payee_ac_no, sMoBankPo.payee_ac_seqn,\
					cOpenBrno, &lAcId );
		if ( ret )
		{
			sprintf( acErrMsg, "����ʻ�״̬��ȡ�ʻ�id��[%d]", ret );
			WRITEMSG
			goto ErrExit;	
		}
	}
	
	/* ���ǩ�������Ƿ񳬹�һ���� */
	if ( pub_base_CalTrueDays( sMoBankPo.sign_date, sMoBankPo.bal_date ) > 31 )
	{
		sprintf( acErrMsg, "��Ʊ�Ѿ�����һ���£����ܶҸ�" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B029" );
		goto ErrExit;	
	}
	
	/* ����������Ƿ�С�ڵ���ǩ����� */
	if( pub_base_CompDblVal( sMoBankPo.sign_amt , sMoBankPo.amt ) == 1 )
	{
		sprintf( acErrMsg, "���������ǩ�����,���ܶҸ���Ʊ!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B030" );
		goto ErrExit;
	}
	
	/* ����Ʊ��־�Ƿ��Ѿ���ʧ��������Ǽ���ִ�� */
	ret = sql_count("mo_bank_po", "po_no = '%s' and po_ind='0'", \
			sMoBankPo.po_no);
	if ( ret < 0 )
	{
		sprintf( acErrMsg, "���һ�Ʊ�ǼǱ���![%d]", ret );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B018" );
		goto ErrExit;
	}else if ( ret > 0 )
	{
		sprintf( acErrMsg, "�����л�Ʊ�Ѿ���ʧ�����ܶҸ���Ʊ!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B031" );
		goto ErrExit;
	}
	
	/* �Ǽǻ�Ʊ�ǼǱ� */
	ret = pub_sttl_RegMoBankPo( sMoBankPo );
	if ( ret )
    	{
        	sprintf( acErrMsg, "ִ��pub_sttl_RegMoBankPo����!ret=[%d]", ret );
        	WRITEMSG
        	goto ErrExit;
        }        	
	
	/* ����ˮ��־ */
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
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

