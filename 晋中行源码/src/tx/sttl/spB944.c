/*************************************************
* �� �� ��:  spB944.c
* ���������� ��ɻع�ʽ�����ּ��˹��ܣ��������������루408����
*	     ���ڲ��ʻ�233���ڲ��ʴ��������ӽ��ף��޸ĵǼǲ�״̬��
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
#include "com_parm_c.h"

int spB944()
{
	struct	mo_discnt_c		sMoDiscnt;	/* ���ֵǼǱ� */
	struct 	com_parm_c 		sComParm;	/* �������� */
	struct  mdm_ac_rel_c    sMdmAcRel;	/* �������˻���ϵ�� */
	struct  in_mst_c		sInMst;		/* �ڲ�̨�����ļ� */
/*	char	cRinAcNo[14];				 �������˺� */
	char	cRinAcNo[21];				/* �������˺� */
/*	char	cAcNo[14];					 ����׼�����˺� */
	char	cAcNo[25];					/* ����׼�����˺� */
	int		ret;

	memset( &sMoDiscnt, 0x00, sizeof( struct mo_discnt_c ) );
	memset( &sComParm, 0x00, sizeof( struct com_parm_c ) );
	memset( &g_pub_tx, 0x00, sizeof( g_pub_tx ) );
	memset( &sMdmAcRel, 0x00, sizeof( struct mdm_ac_rel_c ) );
	memset( &sInMst, 0x00, sizeof( struct in_mst_c ) );
	memset( cRinAcNo, 0x00, sizeof( cRinAcNo ) );
	memset( cAcNo, 0x00, sizeof( cAcNo ) );

	/* ���ݳ�ʼ�� */
	pub_base_sysinit();
	strcpy( g_pub_tx.mdm_code, NULL_MDM_CODE );		/*�ս��ʽ��ʴ���*/
	
	/* ��ǰ̨��Ļȡֵ */
	get_zd_data( "0620", sMoDiscnt.rpact_no );	/* ������Э���� */
	get_zd_data( "0370", cRinAcNo );			/* �������˺� */
	get_zd_data( "0300", cAcNo );				/* ����׼�����˺� */

	/* ���������Э�����Ƿ���ڣ�״̬�Ƿ�Ϊ¼�룬���������Ƿ�Ϊ�ع������� */
	ret = Mo_discnt_Dec_Upd( g_pub_tx.reply, "rpact_no='%s'", sMoDiscnt.rpact_no );
	if ( ret )
	{
		sprintf( acErrMsg, "ִ��Mo_discnt_Dec_Upd��[%d]", ret );
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

	if( sMoDiscnt.pn_type[0] != '7' )
	{
		sprintf( acErrMsg, "��Э��ǻع�ʽ����������!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B170" );
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

	/* �޸����ֵǼǱ���״̬Ϊ���� */
	strcpy( sMoDiscnt.rin_ac_no, cRinAcNo );	/* �������ʺ� */
	strcpy( sMoDiscnt.sts, "3" );				/* ��־��3���� */
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

	/* ����ˮ��־ */
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
