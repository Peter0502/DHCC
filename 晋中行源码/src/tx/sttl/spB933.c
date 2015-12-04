/*************************************************
* �� �� ��:  spB933.c
* ���������� ��ɻع�ʽת������Ϣ¼�루ת�������ܡ�
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

int spB933()
{
	struct	mo_discnt_c	sMoDiscnt;	/* ���ֵǼǱ� */
	struct	mo_discnt_c	sMoDiscnt1;	/* ���ֵǼǱ� */
	long	lPnDays;				/* �������� */
	int	ret;
	char	cOperCode[2];				/* ������ */

	/* ���ݳ�ʼ�� */
	memset( &sMoDiscnt, 0x00, sizeof( struct mo_discnt_c ) );
	memset( &sMoDiscnt1, 0x00, sizeof( struct mo_discnt_c ) );
	memset( cOperCode, 0x00, sizeof( cOperCode ) );
	pub_base_sysinit();
	
	/* ��ǰ̨��Ļȡֵ */
	get_zd_data( "0690", cOperCode );				/* ������ */
	get_zd_data( "0630", sMoDiscnt.tpact_no );		/* ת����Э���� */
	strcpy( sMoDiscnt.br_no, g_pub_tx.tx_br_no );	/* �������� */
	sMoDiscnt.wrk_date = g_pub_tx.tx_date;			/* ת������Ϣ¼��ʱ�������� */
	sMoDiscnt.trace_no = g_pub_tx.trace_no;			/* ¼����ˮ�� */
	get_zd_double( "0850", &sMoDiscnt.tpn_rate );	/* ת������ */
	get_zd_double( "1002", &sMoDiscnt.par_amt );	/* ����Ʊ�ݽ�� */
	get_zd_long( "0440", &sMoDiscnt.tpn_date );		/* ת�������� */
	get_zd_long( "0460", &sMoDiscnt.return_date );	/* �ع����� */
	get_zd_data( "0250", sMoDiscnt.tpn_name );		/* ���������� */
	pub_base_strpack(sMoDiscnt.tpact_no);

	if( cOperCode[0] == '0' )			/* ¼�� */
	{
		/* ������ֵǼǱ����Ƿ��Ѿ����ڴ�ת����Э���� */
		ret = Mo_discnt_Sel( g_pub_tx.reply, &sMoDiscnt, "tpact_no='%s'", sMoDiscnt.tpact_no );
		if( ret == 0 )
		{
			sprintf( acErrMsg, "���ֵǼǱ����Ѿ����ڴ�ת����Э����!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B132" );
			goto ErrExit;
   		}
		else if( ret != 100 )
   		{
   			sprintf( acErrMsg, "ִ��Mo_discnt_Sel��![%d]", ret );
			WRITEMSG
			goto ErrExit;
   		}
   	}
	else if( cOperCode[0] == '1' || cOperCode[0] == '2' )			/* �޸�,ɾ�� */
	{
		vtcp_log( "tpact_no[%s]", sMoDiscnt.tpact_no );
		/* ������ֵǼǱ����Ƿ���ڴ�����Э���� */
		ret = Mo_discnt_Sel( g_pub_tx.reply, &sMoDiscnt1, "tpact_no='%s'",sMoDiscnt.tpact_no );
		if( ret == 100 )
		{
			sprintf( acErrMsg, "���ֵǼǱ��в����ڴ�ת����Э����!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B113" );
			goto ErrExit;
       	}
		else if( ret )
       	{
       		sprintf( acErrMsg, "ִ��Mo_discnt_Sel��![%d]", ret );
			WRITEMSG
			goto ErrExit;
		}
vtcp_log( "pn_type[%s]", sMoDiscnt1.pn_type );
		if( sMoDiscnt1.pn_type[0] != '5' )
		{
			sprintf( acErrMsg, "����������!pn_type[%s]", sMoDiscnt1.pn_type );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B276" );
			goto ErrExit;
		}

		/* ��������Э��״̬ */
		if( sMoDiscnt1.sts[0] == '2' )
		{
			sprintf( acErrMsg, "������Э�������!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B152" );
			goto ErrExit;
		}else if( sMoDiscnt1.sts[0] == '3' )
		{
			sprintf( acErrMsg, "�˱������Ѽ���!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B148" );
			goto ErrExit;
		}else if( sMoDiscnt1.sts[0] == '*' )
		{
			sprintf( acErrMsg, "�˱������ѻ���!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B149" );
			goto ErrExit;
		}

		/* ɾ��������Э���Ŷ�Ӧ�ļ�¼ */
		ret=sql_execute("DELETE from mo_discnt WHERE tpact_no='%s'",sMoDiscnt.tpact_no );
		if( ret ) 
		{
      		sprintf( acErrMsg, "ִ��sql_execute��![%d]", ret );
			WRITEMSG
			goto ErrExit;
      	}

		if( cOperCode[0] == '2' )		/*** ɾ�� ***/
		{
      		sprintf( acErrMsg, "ɾ���ɹ�" );
			WRITEMSG
			goto OkExit;
		}
	}

	/* ȡ���ֲ�Ʒ���:��������com_parm��ȡ��(��pubf_base_GetParm.c�еĺ���) */
	ret = pub_base_GetParm_Str( "TXCP", 3, sMoDiscnt.bkrl_code );
	if( ret )
	{
		sprintf( acErrMsg, "ȡ���ֲ�Ʒ��Ŵ�!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B126" );
		goto ErrExit;
	}

	/* ����������Ϣ��������Ϣ �� ����Ʊ�ݽ��*(�ع�����-ת������*��ת������/360/100)*/
	ret = pub_base_CalDays( sMoDiscnt.tpn_date, sMoDiscnt.return_date, "0", &lPnDays );
	if( ret )
	{
		sprintf( acErrMsg, "��������������!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B127" );
		goto ErrExit;
	}

	sMoDiscnt.tpn_int = sMoDiscnt.par_amt * lPnDays * (sMoDiscnt.tpn_rate/360/100);
	get_zd_double( "1001", &sMoDiscnt.tpn_int );		/* ת������Ϣ */
	set_zd_double( "1001", sMoDiscnt.tpn_int );		/* ת������Ϣ */
	/* �Ǽ����ֵǼǱ���״̬Ϊ¼�� */
	strcpy( sMoDiscnt.sts, "1" );				/* ��־��1¼�� */
	strcpy( sMoDiscnt.pn_type, "5" );			/* ��������: 5 �ع�ת�� */
	sMoDiscnt.tranx_bal = sMoDiscnt.par_amt;	/* ��� */

	ret = Mo_discnt_Ins( sMoDiscnt, g_pub_tx.reply );
	if ( ret )
   	{
       	sprintf( acErrMsg, "ִ��Mo_discnt_Ins����!ret=[%d]", ret );
       	WRITEMSG
       	goto ErrExit;
   	}
		
	/* ����ˮ��־ */
	/**----- xxx ---- @20030819@ ---- ��������Ҫ��Э���� ----**/
	strcpy(g_pub_tx.ac_no,sMoDiscnt.tpact_no);
	g_pub_tx.tx_amt1=0.00;
	strcpy( g_pub_tx.brf, "�ع�ʽת����ת��¼��" );
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
