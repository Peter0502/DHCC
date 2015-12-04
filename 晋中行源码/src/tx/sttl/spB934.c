/*************************************************
* �� �� ��:  spB934.c
* ���������� ��ɻع�ʽת���ּ��ˣ�ת�������ܣ��ֱ��
*	     �����������루408������ش���ӽ��ף����ޣ�
*	     ��NULL���棩�����ڲ��ʻ�233���ڲ��ʴ�����
*	     ���ӽ��ף��޸ĵǼǲ�״̬��
*
* ��    ��:  jane
* ������ڣ� 2003��4��14��
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
#include "mo_discnt_c.h"
#include "mo_city_c.h"
#include "com_tx_br_rel_c.h"
#include "com_parm_c.h"

int spB934()
{
	struct	mo_discnt_c		sMoDiscnt;	/* ���ֵǼǱ� */
	struct 	mo_city_c 		sMoCity;	/* ͬ�ǵǼǲ� */
	struct 	com_tx_br_rel_c 	sComTxBrRel;	/* ���׻�����ϵ�� */
	struct 	com_parm_c 		sComParm;	/* �������� */
	char	cTinAcNo[14];				/* ת�����ʺ� */
	double	dAmt1, dAmt2, dAmt3;
	int	ret;

	memset( &sMoDiscnt, 0x00, sizeof( struct mo_discnt_c ) );
	memset( &sMoCity, 0x00, sizeof( struct mo_city_c ) );
	memset( &sComTxBrRel, 0x00, sizeof( struct com_tx_br_rel_c ) );
	memset( &sComParm, 0x00, sizeof( struct com_parm_c ) );
	memset( &g_pub_tx, 0x00, sizeof( g_pub_tx ) );
	memset( cTinAcNo, 0x00, sizeof( cTinAcNo ) );

	/* ���ݳ�ʼ�� */
	pub_base_sysinit();
	strcpy( g_pub_tx.mdm_code, NULL_MDM_CODE );		/*�ս��ʽ��ʴ���*/
	
	/* ��ǰ̨��Ļȡֵ */
	get_zd_data( "0630", sMoDiscnt.tpact_no );		/* ת����Э���� */
	get_zd_data( "0370", cTinAcNo );				/* ת�����ʺ� */
	get_zd_data( "0670", sMoDiscnt.tpay_ac_type );  /* �����˻����� */

	/* ���ת����Э�����Ƿ���ڣ�״̬�Ƿ�Ϊ¼�룬���������Ƿ�Ϊ�ع�ת�� */
	ret = Mo_discnt_Dec_Upd( g_pub_tx.reply, "tpact_no='%s'", sMoDiscnt.tpact_no );
	if ( ret )
	{
		sprintf( acErrMsg, "ִ��Mo_discnt_Dec_Upd��[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	ret = Mo_discnt_Fet_Upd( &sMoDiscnt, g_pub_tx.reply );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "��ת����Э���Ų�����!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B133" );
		goto ErrExit;
   	}else if( ret )
   	{
   		sprintf( acErrMsg, "ִ��Mo_discnt_Fet_Upd��![%d]", ret );
		WRITEMSG
		goto ErrExit;
   	}
	sMoDiscnt.wrk_date = g_pub_tx.tx_date;			/* ��ˮ���� */
	sMoDiscnt.trace_no = g_pub_tx.trace_no;			/* ������ˮ�� */

	if( sMoDiscnt.pn_type[0] != '5' )
	{
		sprintf( acErrMsg, "��Э��ǻع�ʽת����ת������!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B166" );
		goto ErrExit;
	}

	if( sMoDiscnt.sts[0] == '3' )
	{
		sprintf( acErrMsg, "�˱������Ѽ���!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B148" );
		goto ErrExit;
	}else if( sMoDiscnt.sts[0] == '*' )
	{
		sprintf( acErrMsg, "�˱������ѵ��ڻع�!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B160" );
		goto ErrExit;
	}

	/* �޸����ֵǼǱ���״̬Ϊ���� */
	strcpy( sMoDiscnt.tin_ac_no, cTinAcNo );	/* ת�����ʺ� */
	strcpy( sMoDiscnt.sts, "3" );				/* ��־��3���� */

	ret = Mo_discnt_Upd_Upd( sMoDiscnt, g_pub_tx.reply );
	if ( ret )
	{
		sprintf( acErrMsg, "�޸����ֵǼǱ���[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	Mo_discnt_Clo_Upd();

	/* Ϊ�����J003ǰ̨������� */
	get_zd_double( "1001", &dAmt1 );		/* ת������Ϣ */
	get_zd_double( "1002", &dAmt2 );		/* ��Ʊ��� */
	dAmt3 = dAmt2 - dAmt1;					/* ������ */
	set_zd_double( "1003", dAmt3 );

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
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
