/*************************************************
* �� �� ��:  spB374.c
* ���������� ������гжһ�Ʊ��Ѻ�浥���⹦��
*
* ��    ��:  
* ������ڣ� 2003��11��07��
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
#include "impa_dep_reg_c.h"
#include "mo_hold_c.h"

int spB374()
{
	struct	impa_dep_reg_c 		sImpaDepReg;	/* ��Ѻ�浥�Ǽǲ� */
	struct	mo_hold_c			sMoHold;
	struct  mdm_ac_rel_c		sMdmAcRel;
	char    cLoadName[50];       		   		/* �ϴ��ļ��� */
	double	sum_je=0.00;
	int		ret=0;

	/* ���ݳ�ʼ�� */
	memset( &sImpaDepReg, 0x00, sizeof( struct impa_dep_reg_c ) );
	memset( &sMoHold, 0x00 ,sizeof( struct mo_hold_c ) );
	memset( &sMdmAcRel, 0x00 ,sizeof( struct mdm_ac_rel_c ) );
	memset( cLoadName, 0x00, sizeof( cLoadName ) );
	pub_base_sysinit();

	/* ��ǰ̨��Ļȡֵ */
	get_zd_data( "0630", sImpaDepReg.pact_no );			/* ��Ѻ��� */

	/* �����Ѻ�浥�ǼǱ����Ƿ��Ѵ��ڴ���ѺЭ���� */
	ret = Impa_dep_reg_Sel( g_pub_tx.reply, &sImpaDepReg, " pact_no='%s' ", sImpaDepReg.pact_no );
	if( ret )
	{
		sprintf( acErrMsg, "ִ��Impa_dep_reg_Sel����![%d][%s]", ret,sImpaDepReg.pact_no );
		WRITEMSG
		goto ErrExit;
	}

	/* �����Ѻ�ܽ���Ƿ���� */
	ret = sql_sum_double("impa_dep_reg","mort_amt",&sum_je," pact_no='%s' ",sImpaDepReg.pact_no );
	if( ret )
	{
		sprintf( acErrMsg, "�����ܽ�����![%d][%s]", ret,sImpaDepReg.pact_no );
		WRITEMSG
		goto ErrExit;
	}
	get_zd_double( "1002", &sImpaDepReg.mort_amt );			/* ��Ѻ�ܽ�� */
	if( pub_base_CompDblVal(sum_je,sImpaDepReg.mort_amt)!=0 )
	{
		sprintf( acErrMsg, "�ܽ�����![%.2f][%.2f][%s]",sum_je,sImpaDepReg.mort_amt,sImpaDepReg.pact_no );
		WRITEMSG
		strcpy(g_pub_tx.reply,"B186");
		goto ErrExit;
	}

	ret = Impa_dep_reg_Dec_Upd( g_pub_tx.reply, " pact_no='%s' ", sImpaDepReg.pact_no );
	if( ret )
	{
		sprintf(acErrMsg,"ִ��Impa_dep_reg_Dec_Upd����![%d]",ret);
		WRITEMSG
		goto ErrExit;
	}

	while(1)
	{
		ret = Impa_dep_reg_Fet_Upd( &sImpaDepReg, g_pub_tx.reply );
		if( ret == 100 ) break;
		else if( ret )
		{
			sprintf( acErrMsg, "ִ��Impa_dep_reg_Fet_Upd����![%d]", ret );
			WRITEMSG
			goto ErrExit;
		}

		/* ������Ѻ�浥���޸Ķ������ļ� */
		ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdmAcRel, "ac_no='%s'",sImpaDepReg.mort_ac_no );
		if( ret == 100 )
		{
			sprintf( acErrMsg, "��Ѻ�浥�Ų�����![%s]", sImpaDepReg.mort_ac_no );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B183" );
			goto ErrExit;
		}
		else if( ret )
		{
			sprintf( acErrMsg, "ִ��Mdm_ac_rel_Sel����![%d]", ret );
			WRITEMSG
			goto ErrExit;
		}

		ret = Td_mst_Dec_Upd( g_pub_tx.reply , "ac_id=%ld", sMdmAcRel.ac_id );
		if ( ret )
		{
			sprintf( acErrMsg, "ִ��Td_mst_Dec_Upd��[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}

		ret = Td_mst_Fet_Upd( &g_td_mst, g_pub_tx.reply );
		if ( ret == 100 )
		{
			sprintf( acErrMsg, "��Ѻ�浥�Ų�����![%s]", sImpaDepReg.mort_ac_no );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B183" );
			goto ErrExit;
		}else if ( ret )
		{
			sprintf( acErrMsg, "ִ��Td_mst_Fet_Upd��[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}

		if( g_td_mst.hold_sts[0]!='1' )
		{
			sprintf( acErrMsg, "û�б�����[%d][%ld]",ret,g_td_mst.ac_id );
			WRITEMSG
			continue;
		}
		strcpy( g_td_mst.hold_sts, "0" );		/* ����״̬: 0 ���� */
		g_td_mst.hold_amt=0.00;
TRACE
		ret = Td_mst_Upd_Upd( g_td_mst, g_pub_tx.reply );
    		if ( ret )
		{
			sprintf( acErrMsg, "ִ��Td_mst_Upd_Upd��[%d]", ret );
			WRITEMSG
			goto ErrExit;	
		}
		Td_mst_Clo_Upd( );

TRACE
		/* �޸Ķ���Ǽǲ� */
		ret = Mo_hold_Dec_Upd( g_pub_tx.reply , "ac_id=%ld and hold_sts='0' ", sMdmAcRel.ac_id );
		if ( ret )
		{
			sprintf( acErrMsg, "ִ��Mo_hold_Dec_Upd��[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}
		while(1)
		{
			memset(&sMoHold,0x00,sizeof(struct mo_hold_c));
			ret = Mo_hold_Fet_Upd( &sMoHold, g_pub_tx.reply );
			if ( ret==100 )	break;
			else if ( ret )
			{
				sprintf( acErrMsg, "ִ��Mo_hold_Fet_Upd��[%d]", ret );
				WRITEMSG
				goto ErrExit;
			}
			strcpy( sMoHold.hold_sts, "1" );				/* ����״̬���: 1 ���*/
			sMoHold.fct_unhold_date = g_pub_tx.tx_date;		/* ʵ�ʽⶳ���� */
			strcpy( sMoHold.unhold_br_no,g_pub_tx.tx_br_no);/* �ⶳ������*/
			strcpy( sMoHold.unhold_tel, g_pub_tx.tel);		/* �ⶳ����Ա*/
			sMoHold.wrk_date = g_pub_tx.tx_date;			/* �������� */
			sMoHold.trace_no = g_pub_tx.trace_no;			/* ��ˮ�� */
			strcpy( sMoHold.unhold_brf,"�жһ�Ʊ��Ѻ�浥����" );

			/**
			pub_base_strpack(sMoHold.hold_brf);
			if( !strcmp(sMoHold.hold_brf,"ǩ���жһ�Ʊ�Զ�����") 
				&& strcmp(sMoHold.hold_br_no,g_pub_tx.tx_br_no) )
			{
				sprintf( acErrMsg, "�Ǹû���������Ѻ�浥¼��[%s][%s]",sMoHold.hold_br_no,g_pub_tx.tx_br_no);
				WRITEMSG
				strcpy(g_pub_tx.reply,"B344");
				goto ErrExit;
			}
			**/
			ret = Mo_hold_Upd_Upd( sMoHold, g_pub_tx.reply );
			if ( ret )
			{
				sprintf( acErrMsg, "ִ��Mo_hold_Upd_Upd��[%d]", ret );
				WRITEMSG
				goto ErrExit;
			}
TRACE
		}
		Mo_hold_Clo_Upd( );
		ret = Impa_dep_reg_Del_Upd(sImpaDepReg,g_pub_tx.reply);
		if( ret )
		{
			sprintf( acErrMsg, "ִ��Impa_dep_reg_Del_Upd����![%d]", ret );
			WRITEMSG
			goto ErrExit;
		}

	}
	Impa_dep_reg_Clo_Sel ();

	/* Ϊ����ˮ��ֵ */
	strcpy( g_pub_tx.brf, "�жһ�Ʊ��Ѻ�浥����" );	/* ��ע */
	/* ����ˮ��־ */
	if(strlen(sImpaDepReg.pact_no)>19)
	{
		memcpy(g_pub_tx.ac_no,sImpaDepReg.pact_no+1,sizeof(g_pub_tx.ac_no)-1);
	}else
	{
		memcpy(g_pub_tx.ac_no,sImpaDepReg.pact_no,sizeof(g_pub_tx.ac_no)-1);
	}
	memcpy(g_pub_tx.beg_note_no,sImpaDepReg.pact_no,16);
	g_pub_tx.tx_amt1=0.00;
	strcpy(g_pub_tx.add_ind,"");
	ret = pub_ins_trace_log();
	if ( ret )
	{
		sprintf( acErrMsg, "����ˮ��־��" );
		WRITEMSG
		goto ErrExit;
	}
	set_zd_data("0370",sImpaDepReg.pact_no);/*** �޸ĵ���Ѻ�ǼǱ� add by chenchao 20110806 **/
	set_zd_double("0390",sum_je);		/*** �޸ĵ���Ѻ�ǼǱ� add by chenchao 20110806 **/

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
