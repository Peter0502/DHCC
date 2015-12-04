/*************************************************
* �� �� ��:  spL317.c
* ���������� ���Ϣ����/ֹ��
*
* ��    ��:  lance
* ������ڣ� 2003��08��19��
*
* �޸ļ�¼��
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/
#define  EXTERN
#include <string.h>
#include "public.h"
#include "mo_pun_acc_c.h"

int spL317()
{
	char oprt_flag[2];
	double rate=0.00;
	struct mo_pun_acc_c sMoPunAcc;
	struct mdm_ac_rel_c sMdmAcRel;

	memset( &sMoPunAcc, 0x00, sizeof( struct mo_pun_acc_c ) );
	memset( &sMdmAcRel, 0x00, sizeof( struct mdm_ac_rel_c ) );

	pub_base_sysinit();

	get_zd_data( "0700", oprt_flag );
	get_zd_data( "0370", sMdmAcRel.ac_no );
	get_zd_long( "0340", &sMdmAcRel.ac_seqn );


	g_reply_int = pub_ln_lndis_file(sMdmAcRel.ac_no,sMdmAcRel.ac_seqn,
		&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm ); /*��ȡ�����Ϣ*/
	if( g_reply_int ) 
	{
		goto ErrExit;
	}		
	pub_ln_GetRate ( g_ln_mst , g_ln_parm , &rate );	/*��ǰ����ֵ*/
	memcpy( &sMdmAcRel, &g_mdm_ac_rel, sizeof(struct mdm_ac_rel_c) ); 
	
	/*****
	g_reply_int = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdmAcRel, \
		"ac_no='%s' and ac_seqn='%d'", sMdmAcRel.ac_no, sMdmAcRel.ac_seqn );
	if( g_reply_int )
	{
		sprintf( acErrMsg, "��ѯ�����˺Ŷ��ձ����![%d]", g_reply_int );
		WRITEMSG
		goto ErrExit;			
	}
	g_reply_int = Ln_parm_Sel( g_pub_tx.reply, &g_ln_parm, "ac_id=%d and ac_seqn=%d", \
		sMdmAcRel.ac_id, sMdmAcRel.ac_seqn );
	if ( g_reply_int )
	{
		sprintf( acErrMsg, "��ѯ������������![%d]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}
	**********/
	if( oprt_flag[0] == '0' ) /*����*/
	{
		/*���*/
		g_reply_int = Ln_mst_Sel( g_pub_tx.reply, &g_ln_mst, "ac_id=%d and ac_seqn=%d", \
			sMdmAcRel.ac_id, sMdmAcRel.ac_seqn );
		if ( g_reply_int )
		{
			sprintf( acErrMsg, "��ѯ�������ļ�����![%d]", g_reply_int );
			WRITEMSG
			goto ErrExit;
		}
		g_reply_int = pub_base_CompDblVal( g_ln_mst.fine_rate, 0.00 );
		if ( g_reply_int <= 0 )
		{
			strcpy(g_pub_tx.reply,"L229");
			sprintf(acErrMsg,"���˻���δ���巣Ϣ����!");
			WRITEMSG
			goto ErrExit;
		}	

		g_reply_int = sql_count("mo_pun_acc", "sts=0 and ac_id='%d' and ac_seqn='%d'", \
			sMdmAcRel.ac_id, sMdmAcRel.ac_seqn );
		if( g_reply_int < 0 )
		{
			sprintf( acErrMsg, "ͳ�ƴ��Ϣ�Ǽǲ���¼��������![%d]", g_reply_int );
			WRITEMSG
			goto ErrExit;			
		}
		else if( g_reply_int > 0 )
		{
			strcpy( g_pub_tx.reply, "L227" );
			sprintf( acErrMsg, "���˻������÷�Ϣ!ac_id[%d]ac_seqn[%d]", \
				sMdmAcRel.ac_id, sMdmAcRel.ac_seqn );
			WRITEMSG
			goto ErrExit;			

		}

		/*�Ǽ�*/
		sMoPunAcc.ac_id = sMdmAcRel.ac_id;
		sMoPunAcc.ac_seqn = sMdmAcRel.ac_seqn;
		sMoPunAcc.sts = 0;    /*��Ϣ״̬��0-���� >0-ֹ��*/
		sMoPunAcc.beg_date = g_pub_tx.tx_date;
		strcpy( sMoPunAcc.beg_tel, g_pub_tx.tel );
		strcpy( sMoPunAcc.beg_br_no, g_pub_tx.tx_br_no );
		get_zd_data( "0260", sMoPunAcc.brf );

		vtcp_log("\n%d\n%d\n%d\n%d\n%s\n%s\n",
		sMoPunAcc.ac_id,
		sMoPunAcc.ac_seqn,
		sMoPunAcc.sts,
		sMoPunAcc.beg_date,
		sMoPunAcc.beg_tel,
		sMoPunAcc.beg_br_no);

		g_reply_int = Mo_pun_acc_Ins( sMoPunAcc, g_pub_tx.reply );
		if( g_reply_int )
		{
			sprintf( acErrMsg, "�ǼǴ��Ϣ�Ǽǲ�����![%d]", g_reply_int );
			WRITEMSG
			goto ErrExit;			
		}
	}
	else if( oprt_flag[0] == '1' ) /*ֹ��*/
	{
		/*���*/
		g_reply_int = sql_count("mo_pun_acc", "sts=0 and ac_id='%d' and ac_seqn='%d'", \
			sMdmAcRel.ac_id, sMdmAcRel.ac_seqn );
		if( g_reply_int < 0 )
		{
			sprintf( acErrMsg, "ͳ�ƴ��Ϣ�Ǽǲ���¼��������![%d]", g_reply_int );
			WRITEMSG
			goto ErrExit;			
		}
		else if( g_reply_int == 0 )
		{
			strcpy( g_pub_tx.reply, "L228" );
			sprintf( acErrMsg, "���˻�û�����÷�Ϣ!ac_id[%d]ac_seqn[%d]", \
				sMdmAcRel.ac_id, sMdmAcRel.ac_seqn );
			WRITEMSG
			goto ErrExit;			
		}

		/*�޸�*/
		g_reply_int = sql_max_int( "mo_pun_acc", "sts", "ac_id=%d and ac_seqn=%d", \
			sMdmAcRel.ac_id, sMdmAcRel.ac_seqn );
		if( g_reply_int < 0 )
		{
			sprintf( acErrMsg, "ȡ���Ϣ�Ǽǲ���¼���stsֵ����![%d]", g_reply_int );
			WRITEMSG
			goto ErrExit;			
		}

		g_reply_int = sql_execute( "update mo_pun_acc set sts=%d,end_date=%d,end_tel='%s',end_br_no='%s' where sts=0 and ac_id=%d and ac_seqn=%d", \
			g_reply_int + 1, g_pub_tx.tx_date, g_pub_tx.tel, \
			g_pub_tx.tx_br_no, sMdmAcRel.ac_id, sMdmAcRel.ac_seqn );
		if( g_reply_int )
		{
			sprintf( acErrMsg, "���´��Ϣ�Ǽǲ�����![%d]", g_reply_int );
			WRITEMSG
			goto ErrExit;			
		}
	}
	else goto ErrExit;
	
	/*�ǼǷֶλ�����*/
	g_reply_int= pub_ln_Ins_sect_rate ( g_ln_mst, g_ln_parm, rate );
	if( g_reply_int )
	{
		sprintf( acErrMsg, "�ǼǷֶλ�����ʧ��![%d]", g_reply_int );
		WRITEMSG
		goto ErrExit;			
	}	
	
	/* Pub_tx�ṹ��ֵ */
	strcpy( g_pub_tx.ac_no, g_mdm_ac_rel.ac_no);
	g_pub_tx.ac_id=g_ln_mst.ac_id;
	g_pub_tx.ac_seqn=g_ln_mst.ac_seqn;
	strcpy(g_pub_tx.add_ind,"1");		/* ����:0��1��2���� */	
	strcpy( g_pub_tx.ct_ind,"2");		/* ��ת:1��2ת */	
	/* �Ƿ��ӡ����(�������۱�־):1��0�� */
	strcpy ( g_pub_tx.prt_ind , "0" );	
	/* ����ϸ�ʱ�־:1�ռ���2���յ�����3���ջ����� */
	strcpy ( g_pub_tx.hst_ind , "1" );
	g_pub_tx.svc_ind = 3063;			
	strcpy( g_pub_tx.ac_wrk_ind , "0110000" );	
	strcpy ( g_pub_tx.brf,"���Ϣ����" );	
	sprintf(acErrMsg,"Pub_tx�ṹ��ֵ PASS!");
	WRITEMSG
		
	/*** ���׼��ʴ��� ***/
	if( pub_acct_trance() )
	{
		sprintf(acErrMsg,"���׼��ʴ������!");
		WRITEMSG
		goto ErrExit;
	}
	sprintf(acErrMsg,"���׼��ʴ��� PASS!");
	WRITEMSG
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
