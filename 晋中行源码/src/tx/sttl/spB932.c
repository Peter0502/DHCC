/*************************************************
* �� �� ��:  spB932.c
* ���������� ������ʽת���ּ��ˣ�ת�������ܣ��ֱ��
*	     ���˹����ӽ��ף�rob������ش���ӽ���
*	    �����ޣ���NULL���棩�����ڲ��ʻ�233���ڲ���
*	     �������ʡ��ڲ��ʽ跽���ʡ�������ӽ��ף�
*	     �޸ĵǼǲ�״̬��
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
#include "com_parm_c.h"

int spB932()
{
	struct	mo_discnt_c		sMoDiscnt,sMoDiscnt1;	/* ���ֵǼǱ� */
	struct	mdm_ac_rel_c	sMdmAcRel;				/* �������˻���ϵ�� */
	struct 	com_parm_c 		sComParm;				/* �������� */
	char	cTinAcNo[11];							/* ת�����ʺ� */
	double	dAmt1, dAmt2, dAmt3;
	int		ret;
   char    cKinbr[BRNO_LEN+1];
	char	cPn_knd[2];							/* ����Ʊ��־ */

	memset( &sMoDiscnt, 0x00, sizeof( struct mo_discnt_c ) );
	memset( &sMoDiscnt1, 0x00, sizeof( struct mo_discnt_c ) );
	memset( &sMdmAcRel, 0x00, sizeof( struct mdm_ac_rel_c ) );
	memset( &sComParm, 0x00, sizeof( struct com_parm_c ) );
	memset( &g_pub_tx, 0x00, sizeof( g_pub_tx ) );
	memset( cTinAcNo, 0x00, sizeof( cTinAcNo ) );
	memset( cPn_knd, 0x00, sizeof( cPn_knd ) );
	memset(cKinbr      , 0x00, sizeof(cKinbr));

	/* ���ݳ�ʼ�� */
	pub_base_sysinit();
	strcpy( g_pub_tx.mdm_code, NULL_MDM_CODE );	/*�ս��ʽ��ʴ���*/

	/* ��ǰ̨��Ļȡֵ */
	get_zd_data( "0900", sMoDiscnt.note_type );		/* Ʊ������ */
	get_zd_data( "0600", sMoDiscnt.pnote_no );		/* Ʊ�ݺ� */
	get_zd_data( "0370", cTinAcNo );				/* ת�����ʺ� */
	get_zd_data( "0670", sMoDiscnt.tpay_ac_type );  /* �����˻����� */
	get_zd_data( "0980", cPn_knd );		/* Ʊ�ݺ� */
  get_zd_data("0910", cKinbr);
	if(cPn_knd[0]=='E')
	get_zd_data("1032",sMoDiscnt.pnote_no);

sprintf( acErrMsg, "cTinAcNo=[%s]", cTinAcNo );
WRITEMSG
	/* ���Ʊ���Ƿ���ڣ�״̬�Ƿ�Ϊ¼�룬���������Ƿ�Ϊ���ת��,�޸����ֵǼǲ� */
	ret = Mo_discnt_Dec_Upd( g_pub_tx.reply, "note_type='%s' and pnote_no='%s' \
				and pn_type='4' and sts!='*' ", sMoDiscnt.note_type, sMoDiscnt.pnote_no );
	if ( ret )
	{
		sprintf( acErrMsg, "ִ��Mo_discnt_Dec_Upd��[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	ret = Mo_discnt_Fet_Upd( &sMoDiscnt, g_pub_tx.reply );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "��Ʊ��δ�����ʽת����ת��!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B249" );
		goto ErrExit;
   	}else if( ret )
   	{
   		sprintf( acErrMsg, "ִ��Mo_discnt_Fet_Upd��![%d]", ret );
		WRITEMSG
		goto ErrExit;
   	}

	if( sMoDiscnt.sts[0] == '3' )
	{
		sprintf( acErrMsg, "�˱������Ѽ���!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B148" );
		goto ErrExit;
	}

	/* ����Ʊ�����ͺ�Ʊ�ݺŲ�ѯ���ֵǼǱ���Ʊ��¼��ʱ����Ϣ */
	ret = Mo_discnt_Sel( g_pub_tx.reply, &sMoDiscnt1, "note_type='%s' and pnote_no='%s' \
		and (pn_type='1' or pn_type='2') and sts='*' ", sMoDiscnt.note_type,sMoDiscnt.pnote_no );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "�޴�Ʊ����Ϣ!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B130" );
		goto ErrExit;
   	}else if( ret )
   	{
   		sprintf( acErrMsg, "ִ��Mo_discnt_Sel��![%d]", ret );
		WRITEMSG
		goto ErrExit;
   	}

	/* ���ݴ����˺�ID���˺���Ų�ѯ�����˺ţ�Ϊ�������׼�� */
	ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdmAcRel, "ac_id=%ld and ac_seqn=%d", \
				sMoDiscnt1.ac_id, sMoDiscnt1.ac_seqn );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "�����˺Ŷ��ձ��в����ڴ��˺�!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "W010" );
		goto ErrExit;
   	}else if( ret )
   	{
   		sprintf( acErrMsg, "ִ��Mdm_ac_rel_Sel��![%d]", ret );
		WRITEMSG
		goto ErrExit;
   	}
	set_zd_data( "0330", sMdmAcRel.ac_no );
	set_zd_long( "0340", sMoDiscnt1.ac_seqn );

	/* �޸����ֵǼǱ���״̬Ϊ���� */
	strcpy( sMoDiscnt.tin_ac_no, cTinAcNo );	/* ת�����ʺ� */
	strcpy( sMoDiscnt.sts, "*" );				/* ��־��* ���� */
	sMoDiscnt.wrk_date = g_pub_tx.tx_date;			/* ��ˮ���� */
	sMoDiscnt.trace_no = g_pub_tx.trace_no;			/* ������ˮ�� */

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
	strcpy(g_pub_tx.cur_no,"01");
	strcpy(g_pub_tx.no_show,"1");
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
