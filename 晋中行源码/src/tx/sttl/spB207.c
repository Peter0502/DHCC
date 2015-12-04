/*************************************************
* �� �� ��:  sp3207.c
* ���������� ���֧��ת������������ת����˹���,�ֱ����������ӽ��ס�
*	    �����ֻ�(�����129);�޸ĵǼǲ�״̬��
*
* ��    ��:  LiuHuafeng
* ������ڣ� 2006-10-28 18:06
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*  ---------------------------------------------------

*************************************************/

#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "mo_discnt_c.h"

static	struct	mo_discnt_c	sMoDiscnt;	/* ���ֵǼǱ� */

int spB207()
{
	char    cAccHrt[6];                     /* ͬ�ǿ�Ŀ������ */
	int		ret;

	memset( &sMoDiscnt, 0x00, sizeof( struct mo_discnt_c ) );
	memset ( &g_ln_mst, 0x00, sizeof(struct ln_mst_c) );		/* �������ļ� */
	memset( &g_pub_tx, 0x00, sizeof( g_pub_tx ) );
	memset( cAccHrt, 0x00, sizeof( cAccHrt ) );

	/* ���ݳ�ʼ�� */
	pub_base_sysinit();
	strcpy( g_pub_tx.mdm_code, NULL_MDM_CODE );		/*�ս��ʽ��ʴ���*/
	get_zd_data( "0620", sMoDiscnt.pact_no );		/* ת����Э���� */

	/* �����˺��ҵ�����Э����,��ѯ���ֵǼǲ� */
	ret = Mo_discnt_Dec_Upd( g_pub_tx.reply, "pact_no='%s' ",sMoDiscnt.pact_no);
	if ( ret )
	{
		sprintf( acErrMsg, "ִ��Mo_discnt_Dec_Upd��[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	ret = Mo_discnt_Fet_Upd( &sMoDiscnt, g_pub_tx.reply );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "��ת����Э���Ų�����!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B133" );
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf( acErrMsg, "ִ��Mo_discnt_Sel��![%d]", ret );
		WRITEMSG
		goto ErrExit;
	}
	
	if( sMoDiscnt.sts[0] != '3' )
	{
		sprintf( acErrMsg, "�˱�����δ����!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B148" );
		goto ErrExit;
	}else if( sMoDiscnt.sts[0] == '*' )
	{
		sprintf( acErrMsg, "�˱������ѻ���!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B149" );
		goto ErrExit;
	}

	if(sMoDiscnt.pn_type[0]!='1')
	{
		strcpy( g_pub_tx.reply, "B146" );
		vtcp_log("%s,%d �������ʹ���=[%c]",__FILE__,__LINE__,sMoDiscnt.pn_type[0]);
		WRITEMSG
		goto ErrExit;
	}
	if(!memcmp(sMoDiscnt.br_no,QS_BR_NO,BRNO_LEN))
	{
		strcpy( g_pub_tx.reply, "T062" );
		vtcp_log("%s,%d ���׻�������=[%s]",__FILE__,__LINE__,sMoDiscnt.br_no);
		set_zd_data("0130","���׻�������������������");
		WRITEMSG
		goto ErrExit;
	}
	sMoDiscnt.sts[0] = '*';
	sMoDiscnt.tranx_bal=0;
	/* ����ת����Э���ź����������޸����ֵǼǲ� */
	sMoDiscnt.wrk_date = g_pub_tx.tx_date;			/* ��ˮ���� */
	sMoDiscnt.trace_no = g_pub_tx.trace_no;			/* ������ˮ�� */

	/* ���ô���������������ʺ�id���ʺ���� */
	if( ln_open_ac() )
	{
		sprintf(acErrMsg,"���ô������������!");
		WRITEMSG
		goto ErrExit;
	}
	get_zd_data("0630",sMoDiscnt.tpact_no);
	get_zd_long(DC_DATE,&sMoDiscnt.return_date);
	get_zd_long(DC_DATE,&sMoDiscnt.tpn_date);
	get_zd_double("0840",&sMoDiscnt.tpn_rate);
	ret = Mo_discnt_Upd_Upd( sMoDiscnt, g_pub_tx.reply );
	if ( ret )
	{
		sprintf( acErrMsg, "�޸����ֵǼǱ���[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}
	Mo_discnt_Clo_Upd();
	
	/* �����µ����ּ�¼ */
	memset(sMoDiscnt.pact_no, 0 , sizeof(sMoDiscnt.pact_no));
	memcpy(sMoDiscnt.pact_no,sMoDiscnt.tpact_no,sizeof(sMoDiscnt.pact_no)-1);
	
	/* �޸����ֵǼǱ���״̬Ϊ���� */
	sMoDiscnt.ac_id = g_ln_mst.ac_id;		/* �����˺�ID */
	sMoDiscnt.ac_seqn = g_ln_mst.ac_seqn;	/* �����˺���� */
	strcpy( sMoDiscnt.sts, "3" );			/* ��־��3���� */
	sMoDiscnt.tranx_bal=sMoDiscnt.par_amt;
	memcpy(sMoDiscnt.br_no,QS_BR_NO,BRNO_LEN);
	strncpy(sMoDiscnt.pn_ac_no,g_pub_tx.ac_no,sizeof(sMoDiscnt.pn_ac_no)-1);
	get_zd_long("0480",&sMoDiscnt.way_days);
	get_zd_long(DC_TRACE_NO,&sMoDiscnt.trace_no);
	sMoDiscnt.return_date=0;
	ret = Mo_discnt_Ins( sMoDiscnt, g_pub_tx.reply );
	if ( ret )
	{
		sprintf( acErrMsg, "���������ݵ����ֵǼǱ���[%d]", ret );
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

int ln_open_ac()
{
	long	lMtrDate, lWayDays, lTempDays;
	int		ret1;
	
	lTempDays=0;
	get_zd_data("0630",g_ln_mst.pact_no);		/* ��ͬ��(�˴�Ϊ����Э����) */
	lMtrDate=sMoDiscnt.not_matr_date; /* Ʊ�ݵ������� */
	get_zd_long("0480",&lWayDays); 				/* ��;���� */

	lTempDays = lWayDays ;				/* ������󻹿�������Ʊ�ݵ�����+��;����֮���ټ���5�� */
/** �����������û�����ˣ�ֱ����Ʊ�ݵ�������
	ret1 = pub_base_deadlineD( lMtrDate, lTempDays, &g_ln_mst.mtr_date );
	if( ret1 )
	{
		sprintf( acErrMsg, "���㵽�����ڴ���!" );
		WRITEMSG
		return 1;
	}
******************************/
/**	int	ret1; *****JYW*****/

	get_zd_data("0630",g_ln_mst.pact_no);		/* ��ͬ��(�˴�Ϊת����Э����) */
	/**��������**/
	g_ln_mst.mtr_date=lMtrDate;
sprintf(acErrMsg,"��������=[%ld]",g_ln_mst.mtr_date);
WRITEMSG
	get_zd_double("0840",&g_ln_mst.rate); 		/**����**/
	memcpy(g_pub_tx.name,sMoDiscnt.remitter,sizeof(g_pub_tx.name)-1);/**����**/
	get_zd_double("1002",&g_ln_mst.amt_lmt);	/* �ƻ����Ŷ�����޶� */

	/* Pub_tx�ṹ��ֵ */
	strcpy ( g_pub_tx.ac_wrk_ind,"0110" );
	strcpy ( g_pub_tx.ac_id_type , "3" );		/* �˺����� 3-���� */
	strcpy ( g_pub_tx.add_ind , "1" );			/* ����1�� */	
	strcpy ( g_pub_tx.ct_ind , "2" );			/* ��ת:1��2ת */	
	strcpy ( g_pub_tx.prt_ind , "0" );		/* �Ƿ��ӡ����(�������۱�־)0�� */
	strcpy ( g_pub_tx.hst_ind , "1" );		/* ����ϸ�ʱ�־:1�ռ���2���յ�����3���ջ����� */
	g_pub_tx.svc_ind = 3001;
	strcpy(g_pub_tx.brf,"֧��ת����ת�뿪��");
	strcpy( g_pub_tx.prdt_code, "3Z1" );/* ��Ʒ���� */

	/* ���ݴ����Ʒ���ȡ������� */
	ret1 = Ln_parm_Sel( g_pub_tx.reply, &g_ln_parm, "prdt_no='%s'", sMoDiscnt.bkrl_code );
	if( ret1 == 100 )
	{
		sprintf( acErrMsg, "�����ֲ�Ʒ��Ų�����!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B240" );
		return 1;
	}
	else if( ret1 )
	{
		sprintf( acErrMsg, "ִ��Ln_parm_Sel��![%d]", ret1 );
		WRITEMSG
		return 1;
	}

	strcpy( g_pub_tx.intst_type, g_ln_parm.intst_type );	/* ��Ϣ���� */
	strcpy( g_ln_mst.repay_type, g_ln_parm.ln_pay_type );	/* ����֧����ʽ */
	/**add by jane 2003-09-02********/
	strcpy( g_ln_mst.cal_code , "0000000000" );	/* ָ����ϵ���� */
	strcpy( g_ln_mst.cmpd_intst_ind , g_ln_parm.cmpd_intst_ind );	/*�Ƿ�Ǹ�����־*/
	memcpy(g_pub_tx.tx_br_no,QS_BR_NO,BRNO_LEN);
	/* �����˺ſ����� */
	ret1 = pub_acct_opn_discnt_mdm();
	if( ret1 )
	{
		sprintf(acErrMsg,"�����ʴ���![%d]",ret1);
		WRITEMSG
		return 1;
	}

	/***����***/
	ret1 = pub_acct_opn_ac();
	if( ret1 )
	{
		sprintf(acErrMsg,"��������![%d]",ret1);
		WRITEMSG
		return 1;
	}

	/*** ������� ***/
	sprintf(acErrMsg,"ac_no[%s]",g_pub_tx.ac_no);
	WRITEMSG
	set_zd_data("0330",g_pub_tx.ac_no);
	return 0;
}
