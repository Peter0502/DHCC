/*************************************************
* �� �� ��:  spB943.c
* ���������� ��ɻع�ʽ��������Ϣ¼�빦�ܡ�
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

int spB943()
{
	struct	mo_discnt_c	sMoDiscnt;	/* ���ֵǼǱ� */
	struct	mo_discnt_c	sMoDiscnt1;	/* ���ֵǼǱ� */
	int	ret;
	char	cOperCode[2];				/* ������ */

	/* ���ݳ�ʼ�� */
	memset( &sMoDiscnt, 0x00, sizeof( struct mo_discnt_c ) );
	memset( &sMoDiscnt1, 0x00, sizeof( struct mo_discnt_c ) );
	memset( cOperCode, 0x00, sizeof( cOperCode ) );
	pub_base_sysinit();
	
	/* ��ǰ̨��Ļȡֵ */
	get_zd_data( "0690", cOperCode );				/* ������ */
	get_zd_data( "0630", sMoDiscnt.rpact_no );		/* ������Э���� */
	strcpy( sMoDiscnt.br_no, g_pub_tx.tx_br_no );	/* �������� */
	sMoDiscnt.wrk_date = g_pub_tx.tx_date;			/* ��������Ϣ¼��ʱ�������� */
	sMoDiscnt.trace_no = g_pub_tx.trace_no;			/* ¼����ˮ�� */
	get_zd_double( "1002", &sMoDiscnt.par_amt );	/* ����Ʊ�ݽ�� */
	get_zd_long( "0440", &sMoDiscnt.rtpn_date );	/* ���������� */
	get_zd_long( "0460", &sMoDiscnt.return_date );	/* �ع����� */

	if( cOperCode[0] == '0' )			/* ¼�� */
	{
		/* ������ֵǼǱ����Ƿ��Ѿ����ڴ�������Э���� */
		ret = Mo_discnt_Sel( g_pub_tx.reply, &sMoDiscnt, "rpact_no='%s'", sMoDiscnt.rpact_no );
		if( ret == 0 )
		{
			sprintf( acErrMsg, "���ֵǼǱ����Ѿ����ڴ�������Э����!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B137" );
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
		vtcp_log( "rpact_no[%s]", sMoDiscnt.rpact_no );
		/* ������ֵǼǱ����Ƿ���ڴ�����Э���� */
		ret = Mo_discnt_Sel( g_pub_tx.reply, &sMoDiscnt1, "rpact_no='%s'",sMoDiscnt.rpact_no );
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
		if( sMoDiscnt1.pn_type[0] != '7' )
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
		ret=sql_execute("DELETE from mo_discnt WHERE rpact_no='%s'",sMoDiscnt.rpact_no );
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
	ret = pub_base_GetParm_Str( "TXCP", 4, sMoDiscnt.bkrl_code );
	if( ret )
	{
		sprintf( acErrMsg, "ȡ���ֲ�Ʒ��Ŵ�!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B126" );
		goto ErrExit;
	}

	/* �Ǽ����ֵǼǱ���״̬Ϊ¼��,����Ϊ�ع������� */
	strcpy( sMoDiscnt.sts, "1" );				/* ��־��1¼�� */
	strcpy( sMoDiscnt.pn_type, "7" );			/* ��������: 7 �ع������� */
	sMoDiscnt.tranx_bal = sMoDiscnt.par_amt;	/* ��� */

	ret = Mo_discnt_Ins( sMoDiscnt, g_pub_tx.reply );
	if ( ret )
   	{
       	sprintf( acErrMsg, "ִ��Mo_discnt_Ins����!ret=[%d]", ret );
       	WRITEMSG
       	goto ErrExit;
   	}
		
	/* ����ˮ��־ */
	/**---- xxx --- @20030818@ ---- ��Э���Ÿ����ʺų���ʱ�� ---**/
	strcpy(g_pub_tx.ac_no,sMoDiscnt.rpact_no);
	g_pub_tx.tx_amt1=0.00;
	strcpy( g_pub_tx.brf, "�ع�ʽ��������Ϣ¼��" );
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
