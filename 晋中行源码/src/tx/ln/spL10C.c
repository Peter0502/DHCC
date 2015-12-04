/*************************************************
* �� �� ��: spL10C.c
* ����������   
*              ����ƻ�(�Զ����ɲ���)
*
* ��    ��:    lance
* ������ڣ�   2003��03��29��
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

spL10C()  
{ 		
	int ret=0;
	long pay_date,term_opn_date;
	int i=0;
	int ln_cnt=0,month_cnt=0;
	int a=1;
	
	
        struct ln_pay_pln_c	ln_pay_pln;	
	memset (&ln_pay_pln,0x00,sizeof(struct ln_pay_pln_c) );	
		
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

	sprintf(acErrMsg,"ac_id[%d] [%d]",g_ln_mst.ac_id,g_ln_mst.ac_seqn );
	WRITEMSG
	
	/* �жϻ���ƻ����� N�� M�ֶ� */
	if ( g_ln_parm.pay_pln_crt[0]!='A' )
	{
		TRACE
		goto OkExit;
	}
  
	/* ���������� */
	month_cnt = pub_base_CalMouths( g_ln_mst.opn_date , g_ln_mst.mtr_date ); 
	if( ret ) goto ErrExit;
		
	if( g_ln_parm.pay_term_type[0]=='M' )
	{
		if( month_cnt%g_ln_parm.pay_amt_term==0 )
		{
			ln_cnt = month_cnt/g_ln_parm.pay_amt_term ;
		}else{
			ln_cnt = month_cnt/g_ln_parm.pay_amt_term +1;
		}	
	}
	vtcp_log("[%s][%d][%ld]",__FILE__,__LINE__,g_ln_mst.ac_id);
	TRACE
	/* ���ɻ���ƻ� */
	ret = pub_ln_PayPlan( g_ln_mst , g_ln_parm , a , &ln_pay_pln ,0 );	
	if (ret)
	{
		sprintf(acErrMsg,"���ú���pub_ln_PayPlan����!");
		WRITEMSG
		ERR_DEAL	
	}
	TRACE											
	/* ��ӻ���ƻ��� */	
	ret = Ln_pay_pln_Ins( ln_pay_pln , g_pub_tx.reply );
	if(ret)
	{
		sprintf(acErrMsg,"�Ǽǻ���ƻ���ʧ��");
		WRITEMSG
		strcpy(g_pub_tx.reply,"L077");
		goto ErrExit;
	}
	TRACE
	/* Pub_tx�ṹ��ֵ */
	strcpy ( g_pub_tx.ac_wrk_ind,"0" );
	g_pub_tx.tx_amt1 = 0.00 ;
	strcpy ( g_pub_tx.ac_id_type , "3" );	/* �˺����� 3-���� */
	strcpy ( g_pub_tx.add_ind , "1" );	/* ����1�� */	
	strcpy ( g_pub_tx.ct_ind , "2" );	/* ��ת:1��2ת */	
	strcpy ( g_pub_tx.prt_ind , "0" );	/* �Ƿ��ӡ����(�������۱�־)0�� */
	strcpy ( g_pub_tx.hst_ind , "1" );	/* ����ϸ�ʱ�־:1�ռ���2���յ�����3���ջ����� */
	g_pub_tx.svc_ind = 3062 ;
	strcpy( g_pub_tx.brf,"�����ƻ�(�Զ�)" );
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
    sprintf(acErrMsg,"�Զ����ɻ���ƻ��ɹ���[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"�Զ����ɻ���ƻ�ʧ�ܣ�[%s]",g_pub_tx.reply);
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
	get_zd_int("0340",&g_pub_tx.ac_seqn);		/* �����˺���� */

	sprintf(acErrMsg,"���븳ֵ���������˺�[%s]���[%d]",g_pub_tx.ac_no,g_pub_tx.ac_seqn);
	WRITEMSG						
	return 0;
}

