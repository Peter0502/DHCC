/*************************************************
* �� �� ��:  spB923.c
* ���������� ������ת��Ʊ������,��ɻع�ʽת������Ϣ¼��(ת��)���ܡ�
*
* ��    ��:  jane
* ������ڣ� 2005��4��14��
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

int spB923()
{
	struct	mo_discnt_c	sMoDiscnt;	/* ���ֵǼǱ� */
	struct	mo_discnt_c	sMoDiscnt1;	/* ���ֵǼǱ� */
	long	lPnDays;				/* �������� */
	double  rate_val=0.00;          /* ���� */
	int	ret;
	char	cOperCode[2];				/* ������ */

	/* ���ݳ�ʼ�� */
	memset( &sMoDiscnt, 0x00, sizeof( struct mo_discnt_c ) );
	memset( &sMoDiscnt1, 0x00, sizeof( struct mo_discnt_c ) );
	memset( cOperCode, 0x00, sizeof( cOperCode ) );
	pub_base_sysinit();
	
	/* ��ǰ̨��Ļȡֵ */
	get_zd_data( "0690", cOperCode );				/* ������ */
	get_zd_data( "0630", sMoDiscnt.pact_no );		/* ת����Э���� */
	strcpy( sMoDiscnt.br_no, g_pub_tx.tx_br_no );	/* �������� */
	get_zd_double( "0840", &sMoDiscnt.pn_rate );	/* ת������ */
	get_zd_double( "1002", &sMoDiscnt.par_amt );	/* ����Ʊ�ݽ�� */
	get_zd_long( "0440", &sMoDiscnt.pn_date );		/* ת�������� */
	sMoDiscnt.wrk_date = g_pub_tx.tx_date;			/* ת������Ϣ¼��ʱ�������� */
	sMoDiscnt.trace_no = g_pub_tx.trace_no;			/* ¼����ˮ�� */
	get_zd_long( "0460", &sMoDiscnt.return_date );	/* �ع����� */
	get_zd_data( "0680", sMoDiscnt.local_ind );		/* ���ͨ�Ǳ�־ */
	get_zd_data( "0250", sMoDiscnt.pn_name );		/* ת���������˻��� */

	if( cOperCode[0] == '0' )			/* ¼�� */
	{
		/* ������ֵǼǱ����Ƿ��Ѿ����ڴ�ת����Э���� */
		ret = Mo_discnt_Sel( g_pub_tx.reply, &sMoDiscnt, "pact_no='%s'", sMoDiscnt.pact_no );
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
		vtcp_log( "pact_no[%s]", sMoDiscnt.pact_no );
		/* ������ֵǼǱ����Ƿ���ڴ�����Э���� */
		ret = Mo_discnt_Sel( g_pub_tx.reply, &sMoDiscnt1, "pact_no='%s'",sMoDiscnt.pact_no );
		if( ret == 100 )
		{
			sprintf( acErrMsg, "������Э���Ų�����!" );
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
		if( sMoDiscnt1.pn_type[0] != '3' )
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
		ret = sql_execute( "DELETE from mo_discnt WHERE pact_no='%s'",\ 
					 		sMoDiscnt.pact_no );
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
	ret = pub_base_GetParm_Str( "TXCP", 6, sMoDiscnt.bkrl_code );
	if( ret )
	{
		sprintf( acErrMsg, "ȡ���ֲ�Ʒ��Ŵ�!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B126" );
		goto ErrExit;
	}

	/**** ����������������Ƿ���Ϲ涨  2005��8��17���ʽ�Ӫ�˲���������ʲ������ƣ����Է��  by jane *
	* ���ݴ����Ʒ���ȡ������� *
	ret = Ln_parm_Sel( g_pub_tx.reply, &g_ln_parm, "prdt_no='%s'", sMoDiscnt.bkrl_code );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "�����ֲ�Ʒ��Ų�����!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B240" );
		goto ErrExit;
   	}else if( ret )
   	{
   		sprintf( acErrMsg, "ִ��Ln_parm_Sel��![%d]", ret );
		WRITEMSG
		goto ErrExit;
   	}

	* ȡ���ʱ�Ŷ�Ӧ������ *
	ret = pub_base_getllz(g_ln_parm.rate_no,g_ln_parm.cur_no,
						g_pub_tx.tx_date,&rate_val);
	if( ret ) goto ErrExit;
	sprintf(acErrMsg,"ת��ǰ����[%lf]",rate_val);
	WRITEMSG
	
	rate_val = rate_val/1.2;* ת���������� *
	sprintf(acErrMsg,"ת������������[%lf]",rate_val);
	WRITEMSG

	if( g_ln_parm.get_rate_type[0]=='0' ) * �������� *
	{
		* �Ƚ�������ֵ�Ƿ���ȣ�ǰһ������Ļȡֵ����һ�������ʱ��еĹ̶�����ֵ *
		if( pub_base_CompDblVal( sMoDiscnt.pn_rate, rate_val ) != 0 )
		{
			sprintf( acErrMsg, "��������������涨����ֵ����!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B241" );
			goto ErrExit;
		}	
	}else		* �������ʼ���ϸ��¸����� *
	{
		* �������ʼ�� *
		if( pub_base_CompDblVal( sMoDiscnt.pn_rate,
				rate_val*( (100+g_ln_parm.max_flt_ratio)/100.00 ) )>0 )
		{
vtcp_log("=[%d][%s]",g_ln_parm.max_flt_ratio,sMoDiscnt.bkrl_code );
			sprintf(acErrMsg,"current rate is too high[%lf][%lf][%d] ",
				rate_val,rate_val*((100+g_ln_parm.max_flt_ratio)/100.00)
				,g_ln_parm.max_flt_ratio );
			WRITEMSG
			strcpy ( g_pub_tx.reply , "A006" );
			goto ErrExit;
		}
sprintf(acErrMsg,"���������ȷʱ[%lf][%lf][%d] ", rate_val,
	rate_val*((100+g_ln_parm.max_flt_ratio)/100.00) ,g_ln_parm.max_flt_ratio );
WRITEMSG

		if( pub_base_CompDblVal( sMoDiscnt.pn_rate,
			rate_val*((100+g_ln_parm.min_flt_ratio)/100.00) )<0 )
		{
			sprintf(acErrMsg,"������ʴ���ʱ[%lf][%lf][%d] ", rate_val,
	 			rate_val*((100+g_ln_parm.min_flt_ratio)/100.00) ,g_ln_parm.min_flt_ratio );
			WRITEMSG
			strcpy ( g_pub_tx.reply , "A005" );
			goto ErrExit;
		}
sprintf(acErrMsg,"���������ȷʱ[%lf][%lf][%d] ", rate_val,
	rate_val*((100+g_ln_parm.min_flt_ratio)/100.00) ,g_ln_parm.min_flt_ratio );
WRITEMSG	
	}****/

	/* ����������Ϣ��������Ϣ �� ����Ʊ�ݽ��*(�ع�����-ת������*��ת������/360/100)*/
	ret = pub_base_CalDays( sMoDiscnt.pn_date, sMoDiscnt.return_date, "0", &lPnDays );
	if( ret )
	{
		sprintf( acErrMsg, "��������������!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B127" );
		goto ErrExit;
	}

	sMoDiscnt.pn_int = sMoDiscnt.par_amt * lPnDays * (sMoDiscnt.pn_rate/360/100);

	set_zd_double( "1001", sMoDiscnt.pn_int );		/* ������Ϣ */

	/* �Ǽ����ֵǼǱ���״̬Ϊ¼�� */
	strcpy( sMoDiscnt.sts, "1" );				/* ��־��1¼�� */
	strcpy( sMoDiscnt.pn_type, "3" );			/* ��������: 3 �ع�ת�� */
	sMoDiscnt.tranx_bal = sMoDiscnt.par_amt;	/* ��� */

	ret = Mo_discnt_Ins( sMoDiscnt, g_pub_tx.reply );
	if ( ret )
   	{
       	sprintf( acErrMsg, "ִ��Mo_discnt_Ins����!ret=[%d]", ret );
       	WRITEMSG
       	goto ErrExit;
   	}
		
	/* ����ˮ��־ */
	/**--- xxx ---@20050818@ ---- ����ͬ�Ÿ����ʺų���ʱ�� ---**/
	strcpy(g_pub_tx.ac_no,sMoDiscnt.pact_no);
/**
	strcpy( g_pub_tx.brf, "�ع�ʽת����ת����Ϣ¼��" );
**/
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
