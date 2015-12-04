/*************************************************
* �� �� ��:  spB945.c
* ���������� ��������ֵ��ڻع����������ʽ�����ֺͻع�ʽ�����֣����ܣ�
*	     �����޸����ֵǼǲ�����������������ڲ��ʽ跽�ӽ��ף�233ȡ���
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

int spB945()
{
	struct	mo_discnt_c		sMoDiscnt;		/* ���ֵǼǱ� */
	struct  mdm_ac_rel_c    sMdmAcRel;  	/* �������˻���ϵ�� */
	struct  in_mst_c        sInMst;			/* �ڲ�̨�����ļ� */
	char    cAcNo[14];						/* ����׼�����˺� */
	char    cAccHrt[6];                     /* ͬ�ǿ�Ŀ������ */	
	long	lPnDays;						/* �������� */
	int		ret;
	double  T_amt=0.00;

	memset( &sMoDiscnt, 0x00, sizeof( struct mo_discnt_c ) );
	memset( &sMdmAcRel, 0x00, sizeof( struct mdm_ac_rel_c ) );
	memset( &sInMst, 0x00, sizeof( struct in_mst_c ) );
	memset( cAccHrt, 0x00, sizeof( cAccHrt ) );
	memset( cAcNo, 0x00, sizeof( cAcNo ) );

	/* ���ݳ�ʼ�� */
	pub_base_sysinit();
	
	/* ��ǰ̨��Ļȡֵ */
	get_zd_data( "0620", sMoDiscnt.rpact_no );	/* ������Э���� */
	get_zd_data( "0300", cAcNo );               /* ����׼�����˺� */

	/* ������ֵǼǲ���������Э���š�״̬Ϊ�����ļ�¼�Ƿ���� */
	ret = Mo_discnt_Dec_Upd( g_pub_tx.reply, "rpact_no='%s'", sMoDiscnt.rpact_no );
	if ( ret )
	{
		sprintf( acErrMsg, "ִ��Mo_pay_discnt_Dec_Upd��[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	ret = Mo_discnt_Fet_Upd( &sMoDiscnt, g_pub_tx.reply );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "��������Э���Ų�����!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B138" );
		goto ErrExit;
   	}else if( ret )
   	{
   		sprintf( acErrMsg, "ִ��Mo_discnt_Fet_Upd��![%d]", ret );
		WRITEMSG
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
	}

	/* �������׼�����˺���Ч�� */
	ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdmAcRel, "ac_no='%s'", cAcNo );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "��������׼�����˺�����!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B291" );
		goto ErrExit;
	}else if( ret )
	{
		sprintf( acErrMsg, "ִ��Mdm_ac_rel_Sel����![%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	/* �����˺�ID���ʻ���Ų�ѯ�ڲ�̨�����ļ� */
	ret =In_mst_Sel( g_pub_tx.reply, &sInMst, "ac_id=%ld and ac_seqn=%d",\
				sMdmAcRel.ac_id, sMdmAcRel.ac_seqn );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "��������׼�����˺ŷ��ڲ��˺�![%s]", cAcNo );
   		WRITEMSG
   		strcpy( g_pub_tx.reply, "B292" );
		goto ErrExit;
   	}else if( ret )
   	{
   		sprintf( acErrMsg, "ִ��In_mst_Sel��![%d]", ret );
       	WRITEMSG
       	goto ErrExit;
   	}

	if( strcmp( sInMst.prdt_no, "902" ) )
	{
		sprintf( acErrMsg, "�����˺ŷǴ����������׼�������Ʒ!prdt_no[%s]", sInMst.prdt_no );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B293" );
		goto ErrExit;
	}

sprintf( acErrMsg, "rtpn_date=[%ld],return_date=[%ld]", sMoDiscnt.rtpn_date,sMoDiscnt.return_date);
WRITEMSG

	get_zd_double( "0840", &sMoDiscnt.rtpn_rate );		/* �������� */

	/*�����������Ϊ�ع�ʽ�������������������Ϣ=����Ʊ�ݽ��*���ع����콻������-��������*��������/30/1000��*/
	if( sMoDiscnt.pn_type[0] == '7' )
	{
		ret = pub_base_CalDays( sMoDiscnt.rtpn_date, g_pub_tx.tx_date, "0", &lPnDays );
		if( ret )
		{
			sprintf( acErrMsg, "��������������!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B127" );
			goto ErrExit;
		}
sprintf( acErrMsg, "lPnDays=[%ld],par_amt=[%lf],rtpn_rate=[%lf]", lPnDays,sMoDiscnt.par_amt,sMoDiscnt.rtpn_rate);
WRITEMSG
		sMoDiscnt.rpn_int = sMoDiscnt.par_amt * lPnDays * (sMoDiscnt.rtpn_rate/30/1000);
sprintf( acErrMsg, "rpn_int=[%lf]", sMoDiscnt.rpn_int);
WRITEMSG
		set_zd_double( "1001", sMoDiscnt.rpn_int );		/* ��������Ϣ */
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

	set_zd_data( "0370", sMoDiscnt.rin_ac_no );		/* �������ʺ� */
	/* ȡͬ�ǿ�Ŀ��Ʒ���:��������com_parm��ȡ��(��pubf_base_GetParm.c�еĺ���) *
	ret = pub_base_GetParm_Str( "TCKM", 2, cAccHrt );
	if( ret )
	{
		sprintf( acErrMsg, "ȡͬ�ǿ�Ŀ��Ʒ��Ŵ�!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B257" );
		goto ErrExit;
	}

	* ͬ��ʱ����Ŀֵ���ڲ��˴����ӽ��� *
	set_zd_data( "0610", cAccHrt );
sprintf( acErrMsg, "xxxxxx    cAccHrt[%s]", cAccHrt );
WRITEMSG*/

	/* ����ˮ��־ */
	/**---- xxx -- @20030818@ ---- ��Э���Ÿ����ʺų���ʱ�� ---**/
	strcpy(g_pub_tx.ac_no,sMoDiscnt.rpact_no);
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
