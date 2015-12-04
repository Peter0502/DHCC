/*************************************************
* �� �� ��:  spB925.c
* ���������� ��ɻع�ʽת����ת�뵽�ڻع����ܣ�
*	     �����޸����ֵǼǲ����������
*	    ������������/��ش���ӽ��ף�
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
#include "mo_city_c.h"
#include "com_tx_br_rel_c.h"

int spB925()
{
	struct	mo_discnt_c		sMoDiscnt;		/* ���ֵǼǱ� */
	struct	mdm_ac_rel_c	sMdmAcRel;		/* �������˻���ϵ */
	struct 	com_parm_c 		sComParm;		/* �������� */
	struct 	mo_city_c 		sMoCity;		/* ͬ�ǵǼǲ� */
	struct 	com_tx_br_rel_c sComTxBrRel;	/* ���׻�����ϵ�� */
	char	cPayAcType[2];					/* �����ʻ����� */
	char	cLnAcNo[25];					/* �����˺� */
	int		ret;
	double  T_amt=0.00;

	memset( &sMoDiscnt, 0x00, sizeof( struct mo_discnt_c ) );
	memset( &sComParm, 0x00, sizeof( struct com_parm_c ) );
	memset( &sMoCity, 0x00, sizeof( struct mo_city_c ) );
	memset( &sComTxBrRel, 0x00, sizeof( struct com_tx_br_rel_c ) );
	memset( cPayAcType, 0x00, sizeof( cPayAcType ) );
	memset( cLnAcNo, 0x00, sizeof( cLnAcNo ) );
	
	/* ���ݳ�ʼ�� */
	pub_base_sysinit();
	
	/* ��ǰ̨��Ļȡֵ */
	get_zd_data( "0620", sMoDiscnt.pact_no );	/* ת����Э���� */
	get_zd_data( "0690", cPayAcType );			/* �����ʻ����� */

	/* ����ת����Э���Ų�ѯ���ֵǼǲ� */
	/**------- xxx ------- @20030817@ ---------**
	ret = Mo_discnt_Sel( g_pub_tx.reply, &sMoDiscnt, "pact_no='%s'", sMoDiscnt.pact_no );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "��ת����Э���Ų�����!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B133" );
		goto ErrExit;
   	}else if( ret )
   	{
   		sprintf( acErrMsg, "ִ��Mo_discnt_Sel��![%d]", ret );
		WRITEMSG
		goto ErrExit;
	}
	***--------------------------------------------**/
	/* ����ת����Э���ź����������޸����ֵǼǲ� */
	ret = Mo_discnt_Dec_Upd( g_pub_tx.reply, "pact_no='%s' ",sMoDiscnt.pact_no);
	if ( ret )
	{
		sprintf( acErrMsg, "ִ��Mo_pay_discnt_Dec_Upd��[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	ret = Mo_discnt_Fet_Upd( &sMoDiscnt, g_pub_tx.reply );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "���ֵǼǱ��в����ڴ˼�¼!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B121" );
		goto ErrExit;
   	}else if( ret )
   	{
   		sprintf( acErrMsg, "ִ��Mo_discnt_Fet_Upd��![%d]", ret );
		WRITEMSG
		goto ErrExit;
   	}

	if( sMoDiscnt.pn_type[0] != '3' )
	{
		sprintf( acErrMsg, "��Э��ǻع�ʽת����ת������!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B161" );
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

	/* �������ֵǼǲ��еĴ����˺�ID�ʹ����˺���Ų�ѯ�����˺� */
	ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdmAcRel, "ac_id=%ld and ac_seqn=%d", \
				sMoDiscnt.ac_id, sMoDiscnt.ac_seqn );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "�������˻���ϵ���в����ڴ��ʻ���Ϣ!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B117" );
		goto ErrExit;
   	}else if( ret )
   	{
   		sprintf( acErrMsg, "ִ��Mdm_ac_rel_Sel��![%d]", ret );
		WRITEMSG
		goto ErrExit;
   	}

	/* Ϊ������ӽ���׼�� */
	strcpy( cLnAcNo, sMdmAcRel.ac_no );
	set_zd_data( "0330", cLnAcNo );				/* �����˺� */
	set_zd_long( "0350", sMoDiscnt.ac_seqn );		/* �����˺���� */

	/* �޸����ֵǼǱ��������-������=0��״̬Ϊ����,�ǼǸ����ʻ����� */
	strcpy( sMoDiscnt.tpay_ac_type, cPayAcType );	/* �����ʻ����� */
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

	/* ����ˮ��־ */
	/**---- xxx --- @20030818@ ---- ��Э���Ÿ����ʺų���ʱ�� ---**/
	strcpy(g_pub_tx.ac_no,sMoDiscnt.pact_no);
	strcpy(g_pub_tx.no_show,"1");
	g_pub_tx.tx_amt1=T_amt;
	ret = pub_ins_trace_log();
	if ( ret )
	{
		sprintf( acErrMsg, "����ˮ��־��" );
		WRITEMSG
		goto ErrExit;
	}

	/*if( cPayAcType[0] == '1' )		 �����ʻ� 
	{
		sprintf(acErrMsg,"�����ʻ�");
		WRITEMSG
		goto	OkExit1;
	}
	else					 ����ʻ�
	{
		sprintf(acErrMsg,"����ʻ�");
		WRITEMSG
		goto	OkExit2;
	}*//*20151202*/

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
/*OkExit1:
	strcpy( g_pub_tx.reply, "0001" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
OkExit2:
	strcpy( g_pub_tx.reply, "0002" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;*//*20151202*/
}
