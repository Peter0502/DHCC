/*************************************************
* �� �� ��:  spB941.c
* ���������� ������ʽ��������Ϣ¼�빦�ܣ�ͬ���ʽ
*	     ת���֣����Ǽ���ʱ���黹ԭ129�˻����Է�ֻ��ͬ�ǡ�
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

int spB941()
{
	struct	mo_discnt_c	sMoDiscnt1, sMoDiscnt2;	/* ���ֵǼǱ� */
	long	lPnDays;				/* �������� */
	int	ret;
	char	payName[51];

	memset( &sMoDiscnt1, 0x00, sizeof( struct mo_discnt_c ) );
	memset( &sMoDiscnt2, 0x00, sizeof( struct mo_discnt_c ) );
	memset( payName, 0x00, sizeof( payName ) );
	
	/* ���ݳ�ʼ�� */
	pub_base_sysinit();
	
	/* ��ǰ̨��Ļȡֵ */
	get_zd_data( "0620", sMoDiscnt1.rpact_no );	/* ������Э���� */
	strcpy( sMoDiscnt1.br_no, g_pub_tx.tx_br_no );	/* �������� */
	get_zd_double( "1002", &sMoDiscnt1.par_amt );	/* ��Ʊ��� */
	get_zd_double( "0840", &sMoDiscnt1.rtpn_rate );	/* �������� */
	sMoDiscnt1.rtpn_date = g_pub_tx.tx_date;	/* �������� */
	get_zd_long( "0450", &sMoDiscnt1.not_matr_date );/* ��Ʊ������ */
	get_zd_data( "0310", sMoDiscnt1.rem_ac_no );	/* ��Ʊ���ʺ� */
	get_zd_data( "0260", sMoDiscnt1.remitter );	/* ��Ʊ��ȫ�� */
	get_zd_data( "0810", sMoDiscnt1.payer_brno_name );/* ������ȫ�� */
	get_zd_data( "0320", sMoDiscnt1.payee_ac_no );	/* �տ����ʺ� */
	get_zd_data( "0270", sMoDiscnt1.payee );	/* �տ���ȫ�� */
/*�ֶγ���������	get_zd_data( "0820", sMoDiscnt1.payee_brno_name ); �տ��˿�����ȫ�� */
	get_zd_data( "0820", payName );/* �տ��˿�����ȫ�� */
	memcpy(sMoDiscnt1.payee_brno_name,payName,sizeof(sMoDiscnt1.payee_brno_name)-1);
	get_zd_data( "0900", sMoDiscnt2.note_type );	/* Ʊ������ */
	get_zd_data( "0600", sMoDiscnt2.pnote_no );	/* Ʊ�ݺ� */
	get_zd_long( "0480", &sMoDiscnt1.way_days );	/* ��;���� */

	/* ������ֵǼǱ����Ƿ��Ѿ����ڴ�������Э���� */
	ret = Mo_discnt_Sel( g_pub_tx.reply, &sMoDiscnt1, "rpact_no='%s'", sMoDiscnt1.rpact_no );
	if( ret == 0 )
	{
		sprintf( acErrMsg, "���ֵǼǱ����Ѿ����ڴ�������Э����!" );
                WRITEMSG
                strcpy( g_pub_tx.reply, "B137" );
                goto ErrExit;
       	}else if( ret != 100 )
       	{
       		sprintf( acErrMsg, "ִ��Mo_discnt_Sel��![%d]", ret );
                WRITEMSG
                goto ErrExit;
       	}

	/* ����Ʊ�����ͺ�Ʊ�ݺ��ҳ���¼ */
	ret = Mo_discnt_Dec_Upd( g_pub_tx.reply, "note_type='%s' and pnote_no='%s'",\
			 	sMoDiscnt2.note_type, sMoDiscnt2.pnote_no );
	if( ret )
	{
		sprintf( acErrMsg, "ִ��Mo_discnt_Dec_Upd��[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	ret = Mo_discnt_Fet_Upd( &sMoDiscnt2, g_pub_tx.reply );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "���ֵǼǱ��в����ڴ�Ʊ����Ϣ!" );
                WRITEMSG
                strcpy( g_pub_tx.reply, "B130" );
                goto ErrExit;
       	}else if( ret )
       	{
       		sprintf( acErrMsg, "ִ��Mo_discnt_Fet_Upd��![%d]", ret );
                WRITEMSG
                goto ErrExit;
       	}

	/* ���Ʊ��״̬�Ƿ�Ϊ����������������Ϊ���ֻ������ת�� */
	if( sMoDiscnt2.pn_type[0] != '1' && sMoDiscnt2.pn_type[0] != '2' )
	{
		sprintf( acErrMsg, "��Ʊ�ݷ����ֻ����ת�����ã����������ʽ������!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B168" );
		goto ErrExit;
	}

	if( sMoDiscnt2.pn_type[0] == '1' )	/* ���� */
	{
		if( sMoDiscnt2.sts[0] == '1' )
		{
			sprintf( acErrMsg, "�˱�����δ���!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B147" );
			goto ErrExit;
		}else if( sMoDiscnt2.sts[0] == '2' )
		{
			sprintf( acErrMsg, "�˱�����δ����!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B163" );
			goto ErrExit;
		}
	}
	else					/* ���ת�� */
	{
		if( sMoDiscnt2.sts[0] == '1' )
		{
			sprintf( acErrMsg, "�˱�����δ����!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B163" );
			goto ErrExit;
		}
	}
	if( sMoDiscnt2.sts[0] == '*' )
	{
		sprintf( acErrMsg, "�˱�������������!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B149" );
		goto ErrExit;
	}

	/* ȡ���ֲ�Ʒ���:��������com_parm��ȡ��(��pubf_base_GetParm.c�еĺ���) */
	ret = pub_base_GetParm_Str( "TXCP", 4, sMoDiscnt1.bkrl_code );
	if( ret )
	{
		sprintf( acErrMsg, "ȡ���ֲ�Ʒ��Ŵ�!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B126" );
		goto ErrExit;
	}

	/* ����������Ϣ�����У����������� Ʊ�ݵ����� -����������+ ��;����,
		��������Ϣ �� ��Ʊ��� * �������� * ������������/30��/1000���� */
	ret = pub_base_CalDays( sMoDiscnt1.rtpn_date, sMoDiscnt1.not_matr_date, "0", &lPnDays );
	if( ret )
	{
		sprintf( acErrMsg, "��������������!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B127" );
		goto ErrExit;
	}

	sMoDiscnt1.rpn_int = sMoDiscnt1.par_amt * (lPnDays + sMoDiscnt1.way_days) * (sMoDiscnt1.rtpn_rate/30/1000);

	set_zd_double( "1001", sMoDiscnt1.rpn_int );		/* ��������Ϣ */

	/* �Ǽ����ֵǼǱ���״̬Ϊ¼�� */
	strcpy( sMoDiscnt1.note_type, sMoDiscnt2.note_type );	/* Ʊ������ */
	strcpy( sMoDiscnt1.pnote_no, sMoDiscnt2.pnote_no );	/* Ʊ�ݺ� */
	strcpy( sMoDiscnt1.sts, "1" );				/* ��־��1¼�� */
	strcpy( sMoDiscnt1.pn_type, "6" );			/* ��������: 6��������� */
	sMoDiscnt1.tranx_bal = sMoDiscnt1.par_amt;		/* ��� */

	ret = Mo_discnt_Ins( sMoDiscnt1, g_pub_tx.reply );
	if ( ret )
    	{
        	sprintf( acErrMsg, "ִ��Mo_discnt_Ins����!ret=[%d]", ret );
        	WRITEMSG
        	goto ErrExit;
    	}

	/* �޸����ֵǼǲ�ԭ���ּ�¼ */
	strcpy( sMoDiscnt2.sts, "*" );		/* ��־��* ���� */

	ret = Mo_discnt_Upd_Upd( sMoDiscnt2, g_pub_tx.reply );
	if ( ret )
	{
		sprintf( acErrMsg, "�޸����ֵǼǱ���[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	Mo_discnt_Clo_Upd();

    strcpy(g_pub_tx.ac_no,sMoDiscnt1.rpact_no);
    g_pub_tx.tx_amt1=sMoDiscnt1.par_amt;
    strcpy(g_pub_tx.note_type,sMoDiscnt1.note_type);
    strcpy(g_pub_tx.beg_note_no,sMoDiscnt1.pnote_no);
    strcpy(g_pub_tx.brf,"���ʽ������¼��");

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
