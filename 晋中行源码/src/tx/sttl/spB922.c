/*************************************************
* �� �� ��:  spB922.c
* ���������� ���ת�������ת����˹���,�ֱ����������ӽ��ס�
*	    ��ش���ӽ���;�����ֻ�(�����129);�޸ĵǼǲ�״̬��
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

static	struct	mo_discnt_c	sMoDiscnt;	/* ���ֵǼǱ� */

int spB922()
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
	***----------------------------------------***/
	/* ����ת����Э���ź����������޸����ֵǼǲ� */
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
		sprintf( acErrMsg, "���ֵǼǱ��в����ڴ�ת����Э����!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B133" );
		goto ErrExit;
	}else if( ret )
   	{
		sprintf( acErrMsg, "ִ��Mo_discnt_Fet_Upd��![%d]", ret );
		WRITEMSG
		goto ErrExit;
 	}
	get_zd_data( "0700", sMoDiscnt.get_ac_type );   /* �տ��˻����� */
	sMoDiscnt.wrk_date = g_pub_tx.tx_date;			/* ��ˮ���� */
	sMoDiscnt.trace_no = g_pub_tx.trace_no;			/* ������ˮ�� */

	if( sMoDiscnt.pn_type[0] != '2' )
	{
		sprintf( acErrMsg, "��Э������ʽת����ת������!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B162" );
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
		sprintf( acErrMsg, "�˱������ѻ���!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B149" );
		goto ErrExit;
	}

	/* ���ô���������������ʺ�id���ʺ���� */
	if( ln_open_ac() )
	{
		sprintf(acErrMsg,"���ô������������!");
		WRITEMSG
		goto ErrExit;
	}

	/* �޸����ֵǼǱ���״̬Ϊ���� */
	sMoDiscnt.ac_id = g_ln_mst.ac_id;		/* �����˺�ID */
	sMoDiscnt.ac_seqn = g_ln_mst.ac_seqn;	/* �����˺���� */
	strcpy( sMoDiscnt.sts, "3" );			/* ��־��3���� */

	ret = Mo_discnt_Upd_Upd( sMoDiscnt, g_pub_tx.reply );
	if ( ret )
	{
		sprintf( acErrMsg, "�޸����ֵǼǱ���[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}
	Mo_discnt_Clo_Upd();

	/* �������ͬ���ò���������Ҳ������ * ǰ̨��61��ֵ��
	if( sMoDiscnt.get_ac_type[0] == '1' )
	{
		* ȡͬ�ǿ�Ŀ��Ʒ���:��������com_parm��ȡ��(��pubf_base_GetParm.c�еĺ���) *
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
		sprintf( acErrMsg, "zxxxyu    cAccHrt[%s]", cAccHrt );
		WRITEMSG
	}
	*/
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

int ln_open_ac()
{
	long	lMtrDate, lWayDays, lTempDays;
	int		ret1;

	get_zd_data("0620",g_ln_mst.pact_no);		/* ��ͬ��(�˴�Ϊ����Э����) */
	get_zd_long("0450",&lMtrDate); 				/* Ʊ�ݵ������� */
	get_zd_long("0480",&lWayDays); 				/* ��;���� */

	lTempDays = lWayDays ;					/* ������󻹿�������Ʊ�ݵ�����+��;����֮���ټ���5�� */

	ret1 = pub_base_deadlineD( lMtrDate, lTempDays, &g_ln_mst.mtr_date );
	if( ret1 )
	{
		sprintf( acErrMsg, "���㵽�����ڴ���!" );
		WRITEMSG
		return 1;
	}
/**	int	ret1; *****JYW*****/

	get_zd_data("0620",g_ln_mst.pact_no);		/* ��ͬ��(�˴�Ϊת����Э����) */
	get_zd_long("0450",&g_ln_mst.mtr_date); 	/**��������**/
sprintf(acErrMsg,"��������=[%ld]",g_ln_mst.mtr_date);
WRITEMSG
	get_zd_double("0840",&g_ln_mst.rate); 		/**����**/
	get_zd_data("0250",g_pub_tx.name); 			/**����**/
	get_zd_double("1002",&g_ln_mst.amt_lmt);	/* �ƻ����Ŷ�����޶� */

	/* Pub_tx�ṹ��ֵ */
	strcpy ( g_pub_tx.ac_wrk_ind,"0110" );
	strcpy ( g_pub_tx.ac_id_type , "3" );		/* �˺����� 3-���� */
	strcpy ( g_pub_tx.add_ind , "1" );			/* ����1�� */	
	strcpy ( g_pub_tx.ct_ind , "2" );			/* ��ת:1��2ת */	
	strcpy ( g_pub_tx.prt_ind , "0" );			/* �Ƿ��ӡ����(�������۱�־)0�� */
	strcpy ( g_pub_tx.hst_ind , "1" );			/* ����ϸ�ʱ�־:1�ռ���2���յ�����3���ջ����� */
	g_pub_tx.svc_ind = 3001;
	strcpy(g_pub_tx.brf,"ת����ת�뿪��");
	strcpy( g_pub_tx.prdt_code, sMoDiscnt.bkrl_code );/* ��Ʒ���� */

	/* ���ݴ����Ʒ���ȡ������� */
	ret1 = Ln_parm_Sel( g_pub_tx.reply, &g_ln_parm, "prdt_no='%s'", sMoDiscnt.bkrl_code );
	if( ret1 == 100 )
	{
		sprintf( acErrMsg, "�����ֲ�Ʒ��Ų�����!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B240" );
		return 1;
   	}else if( ret1 )
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
