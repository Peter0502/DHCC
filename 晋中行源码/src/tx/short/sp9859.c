/*************************************************
* �� �� ��:  sp9859.c
* ���������� ����ת������Ϣ��
*
* ��    ��:  cfq
* ������ڣ� 2015��11��26��
*
* �޸ļ�¼�� 
* ��   ��:
* �� �� ��:
* �޸�����:
* insert into TX_DEF (TX_CODE, TX_NAME, BIT_MAP, LOG_IND, TX_TYPE, WRK_STS) values ('9859', '����ת������Ϣ', '10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000010000000000000000000000000000', '1', '3', '2');
* insert into TX_FLOW_DEF (TX_CODE, FLOW_CODE, SUB_TX_CODE, SUB_TX_RSLT) values ('9859', 0, '9859', '#$');
* insert into TX_SUB_DEF (SUB_TX_CODE, SUB_TX_NAME, TX_FUNC, ACCT_IND, CHNL_IND) values ('9859', '����ת������Ϣ', 'sp9859', '0', '0');
*************************************************/

#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "mo_discnt_c.h"

int sp9859()
{
	struct	mo_discnt_c	sMoDiscnt1, sMoDiscnt2,sMoDiscnt3;	/* ���ֵǼǱ� */
	long	lPnDays;						/* �������� */
	int		ret,num;
	char	cOperCode[2];					/* ������ */
	char	cPn_knd[2];						/* ��Ʊ��־ */
 	char    cKinbr[BRNO_LEN+1];
  	char    cZsq[2];        			 /* ׷��Ȩ 0����׷��Ȩ 1����׷��Ȩ*/

	/* ���ݳ�ʼ�� */
	memset( &sMoDiscnt1, 0x00, sizeof( struct mo_discnt_c ) );
	memset( &sMoDiscnt2, 0x00, sizeof( struct mo_discnt_c ) );
	memset( &sMoDiscnt3, 0x00, sizeof( struct mo_discnt_c ) );
	memset( cOperCode, 0x00, sizeof( cOperCode ) );
	memset( cPn_knd, 0x00, sizeof( cPn_knd ) );
	memset(cKinbr , 0 , sizeof(cKinbr));
  	memset(cZsq     , 0x00, sizeof(cZsq));
  
  	pub_base_sysinit();
	
	/* ��ǰ̨��Ļȡֵ */
	get_zd_data( "0690", cOperCode );				/* ������ */
	strcpy( sMoDiscnt1.br_no, g_pub_tx.tx_br_no );	/* �������� */
	get_zd_double( "0840", &sMoDiscnt1.tpn_rate );	/* ת������ */
	sMoDiscnt1.tpn_date = g_pub_tx.tx_date;			/* ת������ */
	sMoDiscnt1.wrk_date = g_pub_tx.tx_date;			/* ת������Ϣ¼��ʱ�������� */
	sMoDiscnt1.trace_no = g_pub_tx.trace_no;		/* ¼����ˮ�� */
	get_zd_data( "0900", sMoDiscnt2.note_type );	/* Ʊ������ */
	get_zd_data( "0600", sMoDiscnt2.pnote_no );		/* Ʊ�ݺ� */
	get_zd_data( "0680", sMoDiscnt1.local_ind );	/* ���ͬ�Ǳ�־ */
	get_zd_data( "0250", sMoDiscnt1.tpn_name );		/* �Է������� */
	get_zd_data( "0980", cPn_knd);	/* ��Ʊ��־ */
    get_zd_data("0380", sMoDiscnt1.tpact_no );
 	get_zd_data( "0250", sMoDiscnt1.tpn_name );		/* �Է������� */
    get_zd_data("0720", cZsq); 
 	get_zd_data("0910" , cKinbr );
	if(cPn_knd[0]=='E')
	get_zd_data("1032",sMoDiscnt2.pnote_no);

    	ret = Mo_discnt_Sel( g_pub_tx.reply, &sMoDiscnt2, "note_type='%s' and pnote_no='%s' \
						and pn_type='4' ", sMoDiscnt2.note_type, sMoDiscnt2.pnote_no );
		if( ret == 0 )
		{
			sprintf( acErrMsg, "��Ʊ������ת����ת��!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B248" );
			goto ErrExit;
   		}
		else if( ret != 100 )
   		{
   			sprintf( acErrMsg, "ִ��Mo_discnt_Sel��![%d]", ret );
			WRITEMSG
			goto ErrExit;
   		}
		ret = Mo_discnt_Sel( g_pub_tx.reply, &sMoDiscnt3, "note_type='%s' and pnote_no='%s' \
							and pn_type !='4' ", sMoDiscnt2.note_type, sMoDiscnt2.pnote_no );
		if( ret )
		{
			sprintf( acErrMsg, "ִ��Mo_discnt_Sel3��!" );
			WRITEMSG
			goto ErrExit;
		}
	
		/* ����������Ϣ�����У����������� Ʊ�ݵ����� -ת��������+ ��;����,
					������Ϣ �� ��Ʊ��� * �������� * ����ת������/360��/100���� */
		ret = pub_base_CalDays( sMoDiscnt1.tpn_date, sMoDiscnt3.not_matr_date, "0", &lPnDays );
		if( ret )
		{
			sprintf( acErrMsg, "��������������!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B127" );
			goto ErrExit;
		}

		sMoDiscnt1.tpn_int = sMoDiscnt3.par_amt * (lPnDays + sMoDiscnt3.way_days) * (sMoDiscnt1.tpn_rate/360/100);
		set_zd_double( "1001", sMoDiscnt1.tpn_int );		/* ת������Ϣ */
		vtcp_log("ת������Ϣ:[%.2lf]",sMoDiscnt1.tpn_int);



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