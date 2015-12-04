/*************************************************
* �� �� ��:  spB935.c
* ���������� ��ɻع�ʽת����ת�����ڻع����ܣ������޸�
*	     ���ֵǼǲ��������������/��ش���ӽ��ס�
*	     �ڲ��ʽ跽�ӽ��ף�233ȡ���
*
* ��    ��:  jane
* ������ڣ� 2003��4��15��
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

int spB935()
{
	struct	mo_discnt_c		sMoDiscnt;		/* ���ֵǼǱ� */
	char    cAccHrt[6];                     /* ͬ�ǿ�Ŀ������ */
	int		ret;
	double  T_amt=0.00;

	memset( &sMoDiscnt, 0x00, sizeof( struct mo_discnt_c ) );
	memset( cAccHrt, 0x00, sizeof( cAccHrt ) );

	/* ���ݳ�ʼ�� */
	pub_base_sysinit();
	
	/* ��ǰ̨��Ļȡֵ */
	get_zd_data( "0630", sMoDiscnt.tpact_no );		/* ת����Э���� */

	/* ������ֵǼǲ���ת����Э���š���������Ϊ�ع�ת����״̬Ϊ�����ļ�¼�Ƿ���� */
	ret = Mo_discnt_Dec_Upd( g_pub_tx.reply, "tpact_no='%s' and sts!='*'", sMoDiscnt.tpact_no );
	if ( ret )
	{
		sprintf( acErrMsg, "ִ��Mo_pay_discnt_Dec_Upd��[%d]", ret );
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

	if( sMoDiscnt.pn_type[0] != '5' )
	{
		sprintf( acErrMsg, "��Э��ǻع�ʽת����ת������!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B166" );
		goto ErrExit;
	}

	if( sMoDiscnt.sts[0] == '1' )
	{
		sprintf( acErrMsg, "�˱�����δ����!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B163" );
		goto ErrExit;
	}else if( sMoDiscnt.sts[0] == '*' )
	{
		sprintf( acErrMsg, "�˱������ѵ��ڻع�!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B160" );
		goto ErrExit;
	}else if( sMoDiscnt.sts[0] != '3' )/***��Ϊ3 add by ligl 20100513***/
	{
		sprintf( acErrMsg, "�˱����ֻع�δ¼��!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B327" );
		goto ErrExit;
	}

	/* �޸����ֵǼǱ��������-������=0��״̬Ϊ���� */
	strcpy( sMoDiscnt.sts, "*" );
	T_amt=sMoDiscnt.tranx_bal;
	sMoDiscnt.tranx_bal = 0;

	ret = Mo_discnt_Upd_Upd( sMoDiscnt, g_pub_tx.reply );
	if ( ret )
	{
		sprintf( acErrMsg, "�޸����ֵǼǱ���[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	Mo_discnt_Clo_Upd();

	set_zd_data( "0370", sMoDiscnt.tin_ac_no );		/* ת�����ʺ� */
	get_zd_data( "0670", sMoDiscnt.get_ac_type );   /* �տ��˻����� */
	if( sMoDiscnt.get_ac_type[0] == '1' )
	{
		/* ȡͬ�ǿ�Ŀ��Ʒ���:��������com_parm��ȡ��(��pubf_base_GetParm.c�еĺ���) */
		ret = pub_base_GetParm_Str( "TCKM", 2, cAccHrt );
		if( ret )
		{
			sprintf( acErrMsg, "ȡͬ�ǿ�Ŀ��Ʒ��Ŵ�!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B257" );
			goto ErrExit;
		}

		/* ͬ��ʱ����Ŀֵ���ڲ��˴����ӽ��� */
		set_zd_data( "0610", cAccHrt );
sprintf( acErrMsg, "zxxxyu    cAccHrt[%s]", cAccHrt );
WRITEMSG
	}

	set_zd_long("0450",sMoDiscnt.not_matr_date);        /* Ʊ�ݵ������ڣ�Ϊ��Ȩ���� */

	/* ����ˮ��־ */
	/**---- xxx --- @20030818@ ---- ��Э���Ÿ����ʺų���ʱ�� ---**/
	strcpy(g_pub_tx.ac_no,sMoDiscnt.tpact_no);
	strcpy(g_pub_tx.no_show,"1");
	g_pub_tx.tx_amt1=T_amt;
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
