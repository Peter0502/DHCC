/*************************************************
* �� �� ��:  spB942.c
* ���������� ������ʽ�����ּ��˹��ܣ��ֱ�����˹����ӽ��ף�rob����
*	     ��ش���ӽ��ף����ޣ���NULL���棩�����ڲ��ʻ�233��
*	    �ڲ��ʴ������ʡ��ڲ��ʽ跽���ʡ�������ӽ��ף��޸ĵǼǲ�״̬��
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
#include "mo_city_c.h"
#include "com_tx_br_rel_c.h"
#include "com_parm_c.h"

int spB942()
{
	struct	mo_discnt_c		sMoDiscnt;	/* ���ֵǼǱ� */
	struct 	mo_city_c 		sMoCity;	/* ͬ�ǵǼǲ� */
	struct 	com_tx_br_rel_c 	sComTxBrRel;	/* ���׻�����ϵ�� */
	struct 	com_parm_c 		sComParm;	/* �������� */
/*37��20λ	char	cRinAcNo[11];				 �������˺� */
	char	cRinAcNo[21];				/* �������˺� */
	int	ret;

	memset( &sMoDiscnt, 0x00, sizeof( struct mo_discnt_c ) );
	memset( &sMoCity, 0x00, sizeof( struct mo_city_c ) );
	memset( &sComTxBrRel, 0x00, sizeof( struct com_tx_br_rel_c ) );
	memset( &sComParm, 0x00, sizeof( struct com_parm_c ) );
	memset( &g_pub_tx, 0x00, sizeof( g_pub_tx ) );
	memset( cRinAcNo, 0x00, sizeof( cRinAcNo ) );

	/* ���ݳ�ʼ�� */
	pub_base_sysinit();
	strcpy( g_pub_tx.mdm_code, NULL_MDM_CODE );		/*�ս��ʽ��ʴ���*/
	
	/* ��ǰ̨��Ļȡֵ */
	get_zd_data( "0620", sMoDiscnt.rpact_no );	/* ������Э���� */
vtcp_log( "62 sMoDiscnt.rpact_no[%s] ", sMoDiscnt.rpact_no );
	get_zd_data( "0370", cRinAcNo );		/* �������ʺ� */
vtcp_log( "37 cRinAcNo[%s] ", cRinAcNo );
vtcp_log( "37hou62 sMoDiscnt.rpact_no[%s] ", sMoDiscnt.rpact_no );

	/* ���������Э�����Ƿ���ڣ�״̬�Ƿ�Ϊ¼�룬���������Ƿ�Ϊ��������� */
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

	if( sMoDiscnt.pn_type[0] != '6' )
	{
		sprintf( acErrMsg, "��Э������ʽ����������!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B169" );
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

	/* Ϊͬ�����������׼�� �����������������,�����ʼǵ�110��Ŀ��,��ǰ̨������*/
	ret=Com_parm_Sel( g_pub_tx.reply, &sComParm, "parm_code='TCKM'\
			and parm_seqn=2" );
    	if (ret != 0)
    	{
        	sprintf(acErrMsg,"��ѯ�������������!! [%d]",ret);
        	WRITEMSG
        	goto ErrExit;
    	}
/*
        set_zd_data( "1201", sComParm.val );
*/
        set_zd_double( "1208", ( sMoDiscnt.par_amt - sMoDiscnt.tpn_int ) );
        set_zd_data( "1205", "2" );
        set_zd_data( "120A", "������������" );

	/* �޸����ֵǼǱ���״̬Ϊ���� */
	strcpy( sMoDiscnt.rin_ac_no, cRinAcNo );	/* �������ʺ� */
	strcpy( sMoDiscnt.sts, "*" );			/* ��־����Ϊ���� ��Ϊת���������Ѿ� */
    sMoDiscnt.wrk_date = g_pub_tx.tx_date;          /* ��ˮ���� */
    sMoDiscnt.trace_no = g_pub_tx.trace_no;         /* ������ˮ�� */

	ret = Mo_discnt_Upd_Upd( sMoDiscnt, g_pub_tx.reply );
	if ( ret )
	{
		sprintf( acErrMsg, "�޸����ֵǼǱ���[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	Mo_discnt_Clo_Upd();

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
