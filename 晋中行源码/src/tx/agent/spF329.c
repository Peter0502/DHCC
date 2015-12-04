/*************************************************
* �� �� ��:  spF329.c
* ���������� ������֤����
*
* ��    ��:  mike
* ������ڣ� 2004��11��09��
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
#include "protect_note_c.h"

int spF329()
{
	struct	protect_note_c 		sProtectNote;	/* �����Ǽǲ� */
	struct	mdm_ac_rel_c 		sMdmAcRel;		/* �����˺Ŷ��ձ� */
	struct	dd_mst_c 			sDdMst;			/* �������ļ� */
	double	dAmt;								/* ���ɽ�� */
	char	cOperCode[2];						/* ������ */
	char	cAmtSource[2];						/* �����Դ */
	char	cAcNo[20];							/* �ۿ��˺� */
	char	cBailAcNo[25];						/* ��֤���ʺ� */
	int		ret;

	/* ���ݳ�ʼ�� */
	memset( &sProtectNote, 0x00, sizeof( struct protect_note_c ) );
	memset( &sMdmAcRel, 0x00, sizeof( struct mdm_ac_rel_c ) );
	memset( &sDdMst, 0x00, sizeof( struct dd_mst_c ) );
	memset( cOperCode, 0x00, sizeof( cOperCode ) );
	memset( cAmtSource, 0x00, sizeof( cAmtSource ) );
	memset( cAcNo, 0x00, sizeof( cAcNo ) );
	memset( cBailAcNo, 0x00, sizeof( cBailAcNo ) );
	pub_base_sysinit();

	/* ��ǰ̨��Ļȡֵ */
	get_zd_data( "0690", cOperCode );			/* ������ */
	get_zd_data( "0330", sProtectNote.pact_no );/* �жһ�ƱЭ���� */
	get_zd_double( "0420", &dAmt );				/* ���ɽ�� */
	get_zd_data( "0680", cAmtSource );			/* �����Դ */
	get_zd_data( "0380", cAcNo );				/* �ۿ��˺� */
	get_zd_data( "0320", cBailAcNo );			/* ��֤���ʺ� */
	if( cOperCode[0] == '1' )			/* �ɴ汣֤�� */
	{
		/* ���ݳж�Э���Ų������гжһ�Ʊ������Ϣ�ǼǱ� */
		ret = Protect_note_Sel( g_pub_tx.reply, &sProtectNote, "pact_no='%s'", sProtectNote.pact_no );
		if( ret == 0 )
		{
			sprintf( acErrMsg, "�˺�ͬ���Ѵ���!" );
			WRITEMSG
			sprintf( g_pub_tx.reply, "B071" );
			goto ErrExit;
		}
		else if( ret != 100 && ret )
	   	{
	   		sprintf( acErrMsg, "ִ��Protect_note_Sel��![%d]", ret );
			WRITEMSG
			goto ErrExit;
		}

		/* ��鱣֤���ʺŵ���Ч��:�����˺š��˻�״̬���� */
		ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdmAcRel, "ac_no='%s'", cBailAcNo );
		if( ret == 100 )
		{
			sprintf( acErrMsg, "��֤���˺Ų�����![%s]", cBailAcNo );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B195" );
			goto ErrExit;
		}
		else if( ret )
		{
			sprintf( acErrMsg, "ִ��Mdm_ac_rel_Sel����![%d]", ret );
			WRITEMSG
			goto ErrExit;
		}

		ret=Dd_mst_Sel(g_pub_tx.reply,&sDdMst,"ac_id=%ld and ac_seqn=%d",sMdmAcRel.ac_id, sMdmAcRel.ac_seqn );
		if( ret == 100 )
		{
			sprintf( acErrMsg, "����ı�֤���˺ŷǻ����˺�!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B196" );
			goto ErrExit;
		}
		else if( ret )
		{
			sprintf( acErrMsg, "ִ��Dd_mst_Sel����![%d]", ret );
			WRITEMSG
			goto ErrExit;
		}

		if( strcmp( sDdMst.prdt_no, "142" ) !=0 )
		{
			sprintf( acErrMsg, "����ı�֤���˺ŷǵ�����֤���˺�!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B346" );
			goto ErrExit;
		}

		if( sDdMst.ac_sts[0] != '1' )
		{
			sprintf( acErrMsg, "�˱�֤���˺�״̬������!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B197" );
			goto ErrExit;
		}

		/* �Ǽǻ�Ʊ�ǼǱ� */
		strcpy( sProtectNote.bail_ac_no, cBailAcNo );	/* ��֤���ʺ� */
		sProtectNote.bail_amt = dAmt;					/* ��֤���� */
		strcpy( sProtectNote.protect_sts, "A" );       	/* ����Э��״̬��A �汣֤�� */
		sProtectNote.opn_date = g_pub_tx.tx_date;		/* ��¼�汣֤������ */

		ret=Protect_note_Ins( sProtectNote, g_pub_tx.reply );
		if( ret == -239 )
    	{
        	sprintf( acErrMsg, "�򱣺��ǼǱ������¼����!���ظ���¼!" );
        	WRITEMSG
        	strcpy( g_pub_tx.reply, "B347" );
        	return (-1);
    	}
		else if ( ret )
    	{
        	sprintf( acErrMsg, "ִ��Protect_note_Ins����!ret=[%d]", ret );
        	WRITEMSG
        	return(-1);
    	}

		/* Ϊ����ˮ��׼�� */
		strcpy( g_pub_tx.brf, "�жһ�Ʊ�ɴ汣֤��" );	/* ��ע */
	}
	else						/* ���ɱ�֤�� */
	{
		/* �����Դ���Ϊ�ۻ�������Ҫ���ۿ��˺ŵ���Ч�ԣ����л����˺ţ��˻�״̬���� */
		if( cAmtSource[0] == '2' )
		{
			ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdmAcRel, "ac_no='%s'", cAcNo );
			if( ret == 100 )
			{
				sprintf( acErrMsg, "�ۿ��˺Ų�����![%s]", cAcNo );
				WRITEMSG
				strcpy( g_pub_tx.reply, "B214" );
				goto ErrExit;
			}
			else if( ret )
			{
				sprintf( acErrMsg, "ִ��Mdm_ac_rel_Sel����![%d]", ret );
				WRITEMSG
				goto ErrExit;
			}
	
			ret = Dd_mst_Sel( g_pub_tx.reply, &sDdMst, "ac_id=%ld and ac_seqn=%d",\
						sMdmAcRel.ac_id, sMdmAcRel.ac_seqn );
			if( ret == 100 )
			{
				sprintf( acErrMsg, "�˿ۿ��˺ŷǻ����˺�!" );
				WRITEMSG
				strcpy( g_pub_tx.reply, "B215" );
				goto ErrExit;
			}
			else if( ret )
			{
				sprintf( acErrMsg, "ִ��Dd_mst_Sel����![%d]", ret );
				WRITEMSG
				goto ErrExit;
			}
	
			if( sDdMst.ac_sts[0] != '1' )
			{
				sprintf( acErrMsg, "�˿ۿ��˺�״̬������!" );
				WRITEMSG
				strcpy( g_pub_tx.reply, "B216" );
				goto ErrExit;
			}
		}

		/* �����ɽ��ӵ��жһ�Ʊ������Ϣ�Ǽǲ��б�֤���һ���� */
		ret = Protect_note_Dec_Upd( g_pub_tx.reply, "pact_no='%s'", sProtectNote.pact_no );
		if ( ret )
		{
			sprintf( acErrMsg, "ִ��Protect_note_Dec_Upd��[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}
	
		ret = Protect_note_Fet_Upd( &sProtectNote, g_pub_tx.reply );
		if( ret == 100 )
		{
			sprintf( acErrMsg, "�˺�ͬ�Ų�����!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B117" );
			goto ErrExit;
		}
		else if( ret )
	   	{
	   		sprintf( acErrMsg, "ִ��Protect_note_Fet_Upd��![%d]", ret );
	       	WRITEMSG
			goto ErrExit;
		}

		if( sProtectNote.protect_sts[0] == '1' )
		{
			sprintf( acErrMsg, "��Э���ѵ���!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B348" );
			goto ErrExit;
		}

		/*---- ��鱣֤���ʺ��Ƿ���ȷ ----*/
		pub_base_strpack(sProtectNote.bail_ac_no);
		sprintf( acErrMsg, "��֤���ʺ�[%s][%s]",sProtectNote.bail_ac_no,cBailAcNo);
		WRITEMSG
		if( strlen(sProtectNote.bail_ac_no)>0 && strcmp(sProtectNote.bail_ac_no,cBailAcNo) )
		{
	   		sprintf( acErrMsg, "��֤���ʺŴ���![%s][%s]",sProtectNote.bail_ac_no,cBailAcNo);
			WRITEMSG
			strcpy(g_pub_tx.reply,"B258");
			goto ErrExit;
		}
		if( strlen(sProtectNote.bail_ac_no)==0 )
		{
			strcpy(sProtectNote.bail_ac_no,cBailAcNo);
		}

		/* �޸ĳжһ�Ʊ������Ϣ�Ǽǲ� */
		sProtectNote.bail_amt += dAmt;		/* ��֤��� */
		ret = Protect_note_Upd_Upd( sProtectNote, g_pub_tx.reply );
		if ( ret )
		{
			sprintf( acErrMsg, "�޸ĳжһ�Ʊ������Ϣ�ǼǱ���[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}
		Protect_note_Clo_Upd();
		strcpy( g_pub_tx.brf, "���ɵ�����֤��" );	/* ��ע */
	}

	/* ����ˮ��־ */
	strcpy(g_pub_tx.ac_no,sProtectNote.pact_no);
	g_pub_tx.tx_amt1=dAmt;
	strcpy(g_pub_tx.no_show,"1");
	strcpy(g_pub_tx.add_ind,cOperCode);		/** �ɴ油�� **/
	ret = pub_ins_trace_log();
	if ( ret )
	{
		sprintf( acErrMsg, "����ˮ��־��" );
		WRITEMSG
		goto ErrExit;
	}
	set_zd_data( "0820", sDdMst.name );    /* Ϊ��ӡ������ */

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
