/*************************************************
* �� �� ��: spL310.c
* ����������   
*              �������
*
* ��    ��:    lance
* ������ڣ�   2003��06��04��
* �޸ļ�¼��   
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
*     2. 
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#define EXTERN
#include "public.h"
#include "ln_pay_pln_c.h"
#include "dc_acc_rel_c.h"
#include "ass_br_def_c.h"
#include "ass_gl_c.h"

	char brno[6];
spL310()  
{ 		
	int ret=0;
	long pay_date,term_opn_date;
	int i=0;
	int ln_cnt=0,month_cnt=0;
	int a=1;
	
	char dc_inf[5];
	
	
  struct ln_pay_pln_c	ln_pay_pln;	
	struct dc_acc_rel_c 	dc_acc_rel_tmp;
	struct ass_br_def_c 	ass_br_def_out;
	struct ass_br_def_c 	ass_br_def_in;	
	struct ass_gl_c 	ass_gl_out;
	struct ass_gl_c 	ass_gl_in;
	struct ln_mst_c 	p_ln_mst;
				
	memset(&dc_acc_rel_tmp,0x00,sizeof(struct dc_acc_rel_c));
	memset(&ln_pay_pln,0x00,sizeof(struct ln_pay_pln_c));	
	memset(&ass_br_def_out,0x00,sizeof(struct ass_br_def_c));	
	memset(&ass_br_def_in,0x00,sizeof(struct ass_br_def_c));	
	memset(&ass_gl_out,0x00,sizeof(struct ass_gl_c));
	memset(&ass_gl_in,0x00,sizeof(struct ass_gl_c));
	memset(&p_ln_mst,0x00,sizeof(struct ln_mst_c));
						
	/** ��ʼ��ȫ�ֱ��� **/
	pub_base_sysinit();
	sprintf(acErrMsg,"��ʼ��ȫ�ֱ��� PASS!");
	WRITEMSG	
	
	/** ���ݳ�ʼ�� **/
	if ( data_init_struct() )	ERR_DEAL;
	sprintf(acErrMsg,"���ݳ�ʼ�� PASS!");
	WRITEMSG
	
	/** ȡֵ����ֵ **/
	if ( data_set_struct() )    ERR_DEAL;
	sprintf(acErrMsg,"ȡֵ����ֵ PASS!");
	WRITEMSG	
	
	sprintf(acErrMsg,"��ȡ�����Ϣ[%s][%d]",g_pub_tx.ac_no,g_pub_tx.ac_seqn);
	WRITEMSG		
	
	
	/* ���ø��ݴ�����ʾ�˺ź��˺����ȡ�������ļ�����*/
	ret = pub_ln_lndis_file(g_pub_tx.ac_no,g_pub_tx.ac_seqn,
			&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm ); /*��ȡ�����Ϣ*/
	if( ret )
	{
		sprintf(acErrMsg,"��ȡ�����Ϣerror");
		WRITEMSG
		goto ErrExit;
	}

	sprintf(acErrMsg,"ac_id[%ld] [%d]",g_ln_mst.ac_id,g_ln_mst.ac_seqn );
	WRITEMSG
	
	
	/* ���ת��ת�����Ƿ���ͬһ���� */
	if( strcmp( g_ln_mst.opn_br_no , brno )==0 )
	{
		sprintf(acErrMsg,"ת��������������������Ϊͬһ����,���ɰ���!");
		WRITEMSG
		strcpy ( g_pub_tx.reply , "L221" );		
		goto ErrExit;
	}
	
	/* ���ת�����Ƿ��Ǳ����˺� */
	if( strcmp( g_ln_mst.opn_br_no , g_pub_tx.tx_br_no )==0 )
	{
		sprintf(acErrMsg,"�Ǳ����������˺�,���ɰ���!");
		WRITEMSG
		strcpy ( g_pub_tx.reply , "L222" );		
		goto ErrExit;
	}
		
	/* ȡ��Ŀ�� */
	/* ���ݴ���״̬ȡ��ƴ��� */
	if ( g_ln_mst.ac_sts[0]=='1' )	/* ���� */
	{
		strcpy( dc_inf , g_ln_parm.dc_code );
	}
	if ( g_ln_mst.ac_sts[0]=='2' )	/* ���� */
	{
		strcpy( dc_inf , g_ln_parm.over_dc_code );
	}	
	if ( g_ln_mst.ac_sts[0]=='3' )	/* ���� */
	{
		strcpy( dc_inf , g_ln_parm.sla_dc_code );
	}	
	if ( g_ln_mst.ac_sts[0]=='4' )	/* ���� */
	{
		strcpy( dc_inf , g_ln_parm.bad_dc_code );
	}
	/* ���ݻ�ƴ���ȡ��Ŀ�� */
	ret = Dc_acc_rel_Sel( g_pub_tx.reply , &dc_acc_rel_tmp , "dc_code='%s' and data_code='0158' " , dc_inf );
	if( ret && ret!=100 )
	{
		sprintf(acErrMsg,"ȡ��Ŀ���쳣����,�������Ļ�����ϵ!");
		WRITEMSG
		strcpy ( g_pub_tx.reply , "L128" );		
		goto ErrExit;
	}else if ( ret==100 ){
		sprintf(acErrMsg,"�޴˿�Ŀ��,�������Ļ�����ϵ!");
		WRITEMSG
		strcpy ( g_pub_tx.reply , "L129" );		
		goto ErrExit;
	}				
	sprintf(acErrMsg,"��Ŀ��[%s]ϵ!",dc_acc_rel_tmp.acc_hrt);
	WRITEMSG	


	/* ���ݻ������ڿ��˻����������ȡ���˻�������(����) */
	/* ���˻��������� */
	ret = Ass_br_def_Sel( g_pub_tx.reply , &ass_br_def_in , "sub_br_no='%s'" , brno );
	if( ret && ret!=100 )
	{
		sprintf(acErrMsg,"ȡ���˻�����Ϣ�쳣����,�������Ļ�����ϵ!");
		WRITEMSG
		strcpy ( g_pub_tx.reply , "L130" );		
		goto ErrExit;
	}else if ( ret==100 ){
		sprintf(acErrMsg,"�˻����޶�Ӧ�Ŀ��˻���!");
		WRITEMSG
		strcpy ( g_pub_tx.reply , "L131" );		
		goto ErrExit;
	}
	/* ���˻�����δ���� */
	ret = Ass_br_def_Sel( g_pub_tx.reply , &ass_br_def_in , "sub_br_no='%s' and sts='*' " ,brno );
	if( ret==0 )
	{
		sprintf(acErrMsg,"�ÿ��˻�����δ����!");
		WRITEMSG
		strcpy ( g_pub_tx.reply , "L132" );		
		goto ErrExit;
	}
	
	/* ���˻�����δ���� */
	ret = Ass_br_def_Sel( g_pub_tx.reply , &ass_br_def_in , "sub_br_no='%s' and sts='1' and beg_date>%ld" ,brno , g_pub_tx.tx_date );
	if( ret==0 )
	{
		sprintf(acErrMsg,"�ÿ��˻�����δ����!");
		WRITEMSG
		strcpy ( g_pub_tx.reply , "L132" );		
		goto ErrExit;
	}

	/* ���˻������� */
	ret = Ass_br_def_Sel( g_pub_tx.reply , &ass_br_def_in , "sub_br_no='%s' and sts='1' and end_date<%ld" ,brno , g_pub_tx.tx_date );
	if( ret==0 )
	{
		sprintf(acErrMsg,"�ÿ��˻����ѹ���!");
		WRITEMSG
		strcpy ( g_pub_tx.reply , "L133" );		
		goto ErrExit;
	}	
	sprintf(acErrMsg,"�ÿ��˻���(����)[%s]ԭ[%s]!",ass_br_def_in.br_no,brno);
	WRITEMSG		
	
	/* ���ݻ������ڿ��˻����������ȡ���˻������루�Ƴ��� */
	/* ���˻��������� */
	ret = Ass_br_def_Sel( g_pub_tx.reply , &ass_br_def_out , "sub_br_no='%s'" , g_ln_mst.opn_br_no );
	if( ret && ret!=100 )
	{
		sprintf(acErrMsg,"ȡ���˻�����Ϣ�쳣����,�������Ļ�����ϵ!");
		WRITEMSG
		strcpy ( g_pub_tx.reply , "L130" );		
		goto ErrExit;
	}else if ( ret==100 ){
		sprintf(acErrMsg,"�˻����޶�Ӧ�Ŀ��˻���!");
		WRITEMSG
		strcpy ( g_pub_tx.reply , "L131" );		
		goto ErrExit;
	}
		
	/* ���˻�����δ���� */
	ret = Ass_br_def_Sel( g_pub_tx.reply , &ass_br_def_out , "sub_br_no='%s' and sts='*' " ,g_ln_mst.opn_br_no );
	if( ret==0 )
	{
		sprintf(acErrMsg,"�ÿ��˻�����δ����!");
		WRITEMSG
		strcpy ( g_pub_tx.reply , "L132" );		
		goto ErrExit;
	}

	/* ���˻�����δ���� */
	ret = Ass_br_def_Sel( g_pub_tx.reply , &ass_br_def_out , "sub_br_no='%s' and sts='1' and beg_date>%ld" ,g_ln_mst.opn_br_no , g_pub_tx.tx_date );
	if( ret==0 )
	{
		sprintf(acErrMsg,"�ÿ��˻�����δ����!");
		WRITEMSG
		strcpy ( g_pub_tx.reply , "L132" );		
		goto ErrExit;
	}

	
	/* ���˻������� */
	ret = Ass_br_def_Sel( g_pub_tx.reply , &ass_br_def_out , "sub_br_no='%s' and sts='1' and end_date<%ld" ,g_ln_mst.opn_br_no , g_pub_tx.tx_date );
	if( ret==0 )
	{
		sprintf(acErrMsg,"�ÿ��˻����ѹ���!");
		WRITEMSG
		strcpy ( g_pub_tx.reply , "L133" );		
		goto ErrExit;
	}	
	sprintf(acErrMsg,"�ÿ��˻���(�Ƴ�)[%s]ԭ[%s]!",ass_br_def_out.br_no,g_ln_mst.opn_br_no);
	WRITEMSG
			
	
	/* ���¿�������-�Ƴ����� */
	ret = Ass_gl_Dec_Upd( g_pub_tx.reply ,"br_no='%s' and ass_code='%s'and sts='1'",ass_br_def_out.br_no,dc_acc_rel_tmp.acc_hrt);
	if( ret )
	{
		sprintf(acErrMsg,"�����ڴ˿�����Ŀ![%s][%s]",ass_br_def_out.br_no,dc_acc_rel_tmp.acc_hrt);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L134");
		goto ErrExit;
	}	
	ret = Ass_gl_Fet_Upd( &ass_gl_out , g_pub_tx.reply );
	if( ret==100 )
	{
		memset( &ass_gl_out,0x00,sizeof(struct ass_gl_c) );
		strcpy( ass_gl_out.br_no,ass_br_def_out.br_no );
		strcpy( ass_gl_out.ass_code,dc_acc_rel_tmp.acc_hrt );
		ass_gl_out.in_bal=0.00;
		ass_gl_out.out_bal=0.00;
		ass_gl_out.out_bal = ass_gl_out.out_bal-g_ln_mst.in_lo_intst;		
		ass_gl_out.in_intst_bal=0.00;
		ass_gl_out.out_intst_bal=0.00;
		ass_gl_out.sts[0]='1';
		ret = Ass_gl_Ins( ass_gl_out,g_pub_tx.reply );
		if( ret )
		{
	        	sprintf(acErrMsg,"Insert!! [%d]",ret);
	        	WRITEMSG
	        	goto ErrExit;
		}
	}else if(ret){
		sprintf(acErrMsg,"ȡ������Ŀ�쳣����![%s][%s]",ass_br_def_out.br_no,dc_acc_rel_tmp.acc_hrt);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L134");
		goto ErrExit;
	}else{
	
		ass_gl_out.out_bal = ass_gl_out.out_bal-g_ln_mst.in_lo_intst;
			
		ret = Ass_gl_Upd_Upd( ass_gl_out , g_pub_tx.reply);
		if( ret )
		{
			sprintf(acErrMsg,"�����ڴ˿�����Ŀ![%s][%s]",ass_br_def_out.br_no,dc_acc_rel_tmp.acc_hrt);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L134");
			goto ErrExit;
		}	
		Ass_gl_Clo_Upd();
	}
	
	/* ���¿�������-������� */
	ret = Ass_gl_Dec_Upd( g_pub_tx.reply ,"br_no='%s' and ass_code='%s'and sts='1'",ass_br_def_in.br_no,dc_acc_rel_tmp.acc_hrt);
	if( ret )
	{
		sprintf(acErrMsg,"�����ڴ˿�����Ŀ![%s][%s]",ass_br_def_in.br_no,dc_acc_rel_tmp.acc_hrt);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L134");
		goto ErrExit;
	}	
	ret = Ass_gl_Fet_Upd( &ass_gl_in , g_pub_tx.reply );
	if( ret==100 )
	{
		memset( &ass_gl_in,0x00,sizeof(struct ass_gl_c) );
		strcpy( ass_gl_in.br_no,ass_br_def_in.br_no );
		strcpy( ass_gl_in.ass_code,dc_acc_rel_tmp.acc_hrt );
		ass_gl_in.in_bal=0.00;
		ass_gl_in.out_bal=0.00;
		ass_gl_in.out_bal = ass_gl_in.out_bal+g_ln_mst.in_lo_intst;
		ass_gl_in.in_intst_bal=0.00;
		ass_gl_in.out_intst_bal=0.00;
		ass_gl_in.sts[0]='1';
		ret = Ass_gl_Ins( ass_gl_in,g_pub_tx.reply );
		if( ret )
		{
	        	sprintf(acErrMsg,"Insert!! [%d]",ret);
	        	WRITEMSG
	        	goto ErrExit;
		}
	}else if(ret){
		sprintf(acErrMsg,"ȡ������Ŀ�쳣����![%s][%s]",ass_br_def_in.br_no,dc_acc_rel_tmp.acc_hrt);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L134");
		goto ErrExit;
	}else{
	
		ass_gl_in.out_bal = ass_gl_in.out_bal+g_ln_mst.in_lo_intst;
			
		ret = Ass_gl_Upd_Upd( ass_gl_in , g_pub_tx.reply);
		if( ret )
		{
			sprintf(acErrMsg,"�����ڴ˿�����Ŀ![%s][%s]",ass_br_def_in.br_no,dc_acc_rel_tmp.acc_hrt);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L134");
			goto ErrExit;
		}	
		Ass_gl_Clo_Upd();
	}

	/* Pub_tx�ṹ��ֵ */
	g_pub_tx.tx_amt2 = g_ln_mst.in_lo_intst;	
	g_pub_tx.cif_no = g_ln_mst.cif_no;			/* �ͻ��� */
	strcpy ( g_pub_tx.ac_id_type , "3" );	/* �˺����� 3-���� */
	strcpy ( g_pub_tx.intst_type , g_ln_mst.intst_type );	/* ��Ϣ���� */	
	strcpy ( g_pub_tx.add_ind , "0" );		/* ����:0��1��2���� */	
	strcpy ( g_pub_tx.ct_ind , "2" );		/* ��ת:1��2ת */	
	/* �Ƿ��ӡ����(�������۱�־):1��0�� */
	strcpy ( g_pub_tx.prt_ind , "1" );	
	/* ����ϸ�ʱ�־:1�ռ���2���յ�����3���ջ����� */
	strcpy ( g_pub_tx.hst_ind , "1" );
	g_pub_tx.svc_ind = 3059;			/* �����ת */
	strcpy( g_pub_tx.ac_wrk_ind , "0110000" );
	strcpy ( g_pub_tx.brf,"�������(�Ƴ�)" );
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


	
	/* �޸Ĵ������ļ�,������������� */
	ret = Ln_mst_Dec_Upd( g_pub_tx.reply, "ac_id=%ld and ac_seqn=%d  ", g_mdm_ac_rel.ac_id , g_pub_tx.ac_seqn );
	if( ret )
	{
		sprintf( acErrMsg,"Prepare Ln_mst_Dec_Upd �α����[%d]",ret);
		WRITEMSG
		goto ErrExit;
	}	
	ret = Ln_mst_Fet_Upd( &p_ln_mst, g_pub_tx.reply );
	if( ret==100 )
	{
		sprintf( acErrMsg,"�޼�¼[%d]",ret);
		WRITEMSG
		goto ErrExit;
	}
   	if( ret )
 	{
	 	sprintf( acErrMsg,"fetch Ln_mst_Fet_Upd error code = [%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"D103" );
		goto ErrExit;
 	}
			
	strcpy( p_ln_mst.opn_br_no , brno );		
	
	ret = Ln_mst_Upd_Upd( p_ln_mst, g_pub_tx.reply  );
	if ( ret )
	{
		sprintf( acErrMsg,"Ln_mst_Upd_Upd ����[%d]",ret);
		WRITEMSG
		goto ErrExit;
	}		
		
	Ln_mst_Clo_Upd( );
	sprintf(acErrMsg,"�������ļ��л������� PASS");
	WRITEMSG		


	/* Pub_tx�ṹ��ֵ */
	g_pub_tx.tx_amt2 = g_ln_mst.in_lo_intst;	
	g_pub_tx.cif_no = g_ln_mst.cif_no;			/* �ͻ��� */
	strcpy ( g_pub_tx.ac_id_type , "3" );	/* �˺����� 3-���� */
	strcpy ( g_pub_tx.intst_type , g_ln_mst.intst_type );	/* ��Ϣ���� */	
	strcpy ( g_pub_tx.add_ind , "1" );		/* ����:0��1��2���� */	
	strcpy ( g_pub_tx.ct_ind , "2" );		/* ��ת:1��2ת */	
	/* �Ƿ��ӡ����(�������۱�־):1��0�� */
	strcpy ( g_pub_tx.prt_ind , "1" );	
	/* ����ϸ�ʱ�־:1�ռ���2���յ�����3���ջ����� */
	strcpy ( g_pub_tx.hst_ind , "1" );
	g_pub_tx.svc_ind = 3059;			/* �����ת */
	strcpy( g_pub_tx.ac_wrk_ind , "0110000" );
	strcpy ( g_pub_tx.brf,"�������(����)" );
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
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"�������ɹ���[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"�������ʧ�ܣ�[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
    
int data_init_struct()
{
	memset (&g_ln_mst, 0x00, sizeof(struct ln_mst_c));
	memset (&g_mdm_ac_rel, 0x00, sizeof(struct mdm_ac_rel_c));
	memset (&g_ln_mst, 0x00, sizeof(struct ln_mst_c));
	
	return 0;
	
}

int data_set_struct()
{    
	sprintf(acErrMsg,"���븳ֵ����");
	WRITEMSG
	
	
	get_zd_data("0370",g_pub_tx.ac_no);		/* �����˺� */	
	get_zd_data("0910",brno);			/* �������� */
		
	g_pub_tx.ac_seqn=0;				/* �����˺���� */

	sprintf(acErrMsg,"���븳ֵ���������˺�[%s]����[%s]",g_pub_tx.ac_no,brno);
	WRITEMSG						
	return 0;
}

