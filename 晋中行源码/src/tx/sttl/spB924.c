/*************************************************
* �� �� ��:  spB924.c
* ���������� ��ɻع�ʽת���ּ��ˣ�ת�룩,�ֱ����������ӽ��ס�
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
#include "mdm_ac_rel_c.h"
#include "dd_mst_c.h"
#include "ln_mst_c.h"
#include "in_mst_c.h"
#include "dd_parm_c.h"
#include "dc_acc_rel_c.h"
static	struct	mo_discnt_c	sMoDiscnt;	/* ���ֵǼǱ� */

int spB924()
{
	char    cAccHrt[6];                     /* ͬ�ǿ�Ŀ������ */
	int		ret;
  char  skno[24];
  struct	mdm_ac_rel_c	sMdm_ac_rel;
  struct	dd_mst_c	sDd_mst;
  struct	dd_parm_c	sDd_parm;
  struct	dc_acc_rel_c	sDc_acc_rel;
  
	memset( &sMoDiscnt, 0x00, sizeof( struct mo_discnt_c ) );
	memset ( &sMdm_ac_rel, 0x00, sizeof(struct mdm_ac_rel_c) );
	memset ( &sDd_mst, 0x00, sizeof(struct dd_mst_c) );
	memset ( &sDd_parm, 0x00, sizeof(struct dd_parm_c) );
	memset ( &sDc_acc_rel, 0x00, sizeof(struct dc_acc_rel_c) );
	memset ( &g_ln_mst, 0x00, sizeof(struct ln_mst_c) );		/* �������ļ� */
	memset ( &g_in_mst, 0x00, sizeof(struct in_mst_c) );		/* �ڲ������ļ� */
	memset( &g_pub_tx, 0x00, sizeof( g_pub_tx ) );
	memset( cAccHrt, 0x00, sizeof( cAccHrt ) );
	memset( skno, 0x00, sizeof( skno ) );

	/* ���ݳ�ʼ�� */
	pub_base_sysinit();
	strcpy( g_pub_tx.mdm_code, NULL_MDM_CODE );		/*�ս��ʽ��ʴ���*/
	
	/* ��ǰ̨��Ļȡֵ */
	get_zd_data( "0620", sMoDiscnt.pact_no );		/* ת����Э���� */
  get_zd_data( "0300", skno );	  /*�տ��˺�*/
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
		sprintf( acErrMsg, "ִ��Mo_discnt_Dec_Upd��[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	ret = Mo_discnt_Fet_Upd( &sMoDiscnt, g_pub_tx.reply );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "���ֵǼǱ��в����ڴ�ת����Э���Ż�״̬������!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B135" );
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

	if( sMoDiscnt.pn_type[0] != '3' )
	{
		sprintf( acErrMsg, "��Э��ǻع�ʽת����ת������!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B161" );
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

	/* ���ô���������������ʺ�id���ʺ���� *
	if( ln_open_ac() )
	*/
	if( in_open_ac() )
	{
		sprintf(acErrMsg,"���ô������������!");
		WRITEMSG
		goto ErrExit;
	}

	/* �޸����ֵǼǱ���״̬Ϊ���� */
	sMoDiscnt.ac_id = g_in_mst.ac_id;	/* �����˺�ID */
	sMoDiscnt.ac_seqn = g_in_mst.ac_seqn;	/* �����˺���� */
	strcpy( sMoDiscnt.sts, "3" );		/* ��־��3���� */

	ret = Mo_discnt_Upd_Upd( sMoDiscnt, g_pub_tx.reply );
	if ( ret )
	{
		sprintf( acErrMsg, "�޸����ֵǼǱ���[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	Mo_discnt_Clo_Upd();
  vtcp_log("%s,%��������===[%s]\n",__FILE__,__LINE__,sMoDiscnt.get_ac_type);
  {
	/*if( sMoDiscnt.get_ac_type[0] == '1' )
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

		* ͬ��ʱ����Ŀֵ���ڲ��˴����ӽ��� *//*20151127   cfq*/
		/*ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdm_ac_rel, "ac_no='%s'", skno );
		if( ret == 100 )
		{
			sprintf( acErrMsg, "��ȡ�����˺Ŷ��ձ�ʧ��,�������Ļ�����ϵ!!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "C114" );
			goto ErrExit;
		}else if( ret )
		{
			sprintf( acErrMsg, "ִ��Mdm_ac_rel_Sel����![%d]", ret );
			WRITEMSG
			goto ErrExit;
		}
		ret =Dd_mst_Sel( g_pub_tx.reply, &sDd_mst, "ac_id=%ld",sMdm_ac_rel.ac_id);
		if( ret == 100 )
		{
			sprintf( acErrMsg, "���˻��ǻ��ڻ�![%s]", sMdm_ac_rel.ac_id );
	   		WRITEMSG
	   		strcpy( g_pub_tx.reply, "M018" );
			goto ErrExit;
   	}else if( ret )
   	{
   		sprintf( acErrMsg, "ִ��Dd_mst_Sel��![%d]", ret );
       	WRITEMSG
       	goto ErrExit;
   	}
   	ret = Dd_parm_Sel(g_pub_tx.reply,&sDd_parm,"prdt_no=%ld",sDd_mst.prdt_no);
   	if( ret == 100 )
		{
			sprintf( acErrMsg, "���ڲ�Ʒ�������в����ڸü�¼![%s]", sMdm_ac_rel.ac_id );
	   		WRITEMSG
	   		strcpy( g_pub_tx.reply, "W024" );
			goto ErrExit;
   	}else if( ret )
   	{
   		sprintf( acErrMsg, "ִ��Dd_parm_Sel��![%d]", ret );
       	WRITEMSG
       	goto ErrExit;
   	}
   	ret = Dc_acc_rel_Sel(g_pub_tx.reply,&sDc_acc_rel,"dc_code=%ld",sDd_parm.dc_code);
   	if( ret == 100 )
		{
			sprintf( acErrMsg, "�����ڸû�ƴ���![%s]", sMdm_ac_rel.ac_id );
	   		WRITEMSG
	   		strcpy( g_pub_tx.reply, "O169" );
			goto ErrExit;
   	}else if( ret )
   	{
   		sprintf( acErrMsg, "ִ��Dc_acc_rel_Sel��![%d]", ret );
       	WRITEMSG
       	goto ErrExit;
   	}
   	strcpy(cAccHrt,sDc_acc_rel.acc_hrt);*/
		set_zd_data( "0610", skno );
sprintf( acErrMsg, "zxxxyu    cAccHrt[%s]", cAccHrt );
WRITEMSG
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
	int	ret1;

	get_zd_data("0620",g_ln_mst.pact_no);		/* ��ͬ��(�˴�Ϊת����Э����) */
	get_zd_long("0460",&g_ln_mst.mtr_date); 	/**��������**/
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

int in_open_ac()
{
	int ret=0;

	strcpy( g_pub_tx.prdt_code, sMoDiscnt.bkrl_code );/* ��Ʒ���� */
	g_pub_tx.mach_date=0; 					/**��������**/
	strcpy(g_pub_tx.tmp_ind,"0"); 			/**��Ϣ����**/
	g_pub_tx.over_rate=0.00; 				/**��������**/
	get_zd_double("0840",&g_pub_tx.rate); 		/**����**/
	get_zd_data("0250",g_pub_tx.name); 			/**����**/
	strcpy(g_pub_tx.brf,"ת����ת�뿪��");

	ret=In_parm_Sel( g_pub_tx.reply , &g_ln_parm ,"prdt_no='%s'" , g_pub_tx.prdt_code );	
	if( ret==100 )
	{
		sprintf( acErrMsg, "��ѯ�ڲ��ʲ�Ʒ���������!prdt_no=[%s]", g_pub_tx.prdt_code );
		WRITEMSG
		strcpy( g_pub_tx.reply, "W058" );
		goto	ErrExit;
	}
	else if( ret )
	{
		sprintf( acErrMsg, "ִ��In_parm_Sel����!ret=[%d]", ret );
		WRITEMSG
		goto	ErrExit;
	}

	ret=pub_acct_opn_in_mdm();
	if( ret )
	{
		sprintf(acErrMsg,"�����ʴ���![%d]",ret);
		WRITEMSG
		goto ErrExit;
	}

	/*** ������� ***/
	sprintf(acErrMsg,"ac_no[%s]",g_pub_tx.ac_no);
	WRITEMSG
	set_zd_data("0330",g_pub_tx.ac_no);

	ret=pub_acct_opn_ac();
	if( ret )
	{
		sprintf(acErrMsg,"�����ʴ���![%d]",ret);
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
