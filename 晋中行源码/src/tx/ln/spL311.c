/*************************************************
* �� �� ��: spL311.c
* ����������   
*              ���Ҵ����ƻ��޸�
*
* ��    ��:    lance
* ������ڣ�   2003��04��01��
* �޸ļ�¼��   
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
*     2. 
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "stdio.h" 
#define EXTERN
#include "public.h"
#include "ln_lo_c.h"
#include "ln_pay_pln_c.h"
#include "cif_basic_inf_c.h"

static struct ln_lo_c		ln_lo;
static struct ln_pay_pln_c	ln_pay_pln;
static struct cif_basic_inf_c	cif_basic_inf;
static double chg_rate;
static long end_day;
	
spL311()  
{ 	
	int ret;
	

	/** ��ʼ��ȫ�ֱ��� **/
	pub_base_sysinit();
	sprintf(acErrMsg,"��ʼ��ȫ�ֱ��� PASS!");
	WRITEMSG	
	
	/** ���ݳ�ʼ�� **/
	if ( data_init_struct() )	ERR_DEAL;
	sprintf(acErrMsg,"���ݳ�ʼ�� PASS!");
	WRITEMSG
		
	/** ȡֵ����ֵ **/
	if ( data_get_struct() )        ERR_DEAL;
	sprintf(acErrMsg,"ȡֵ����ֵ PASS!");
	WRITEMSG
	
	/** �������� **/	
	if( pub_base_CompDblVal(g_ln_mst.bal , 0.00 )==0 )
	{
		sprintf(acErrMsg,"���Ҵ����ѻ���!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"L157");
	}
	
	/*����Ƿ���ϲ�ƷҪ��*/	
	/* ���ݿͻ��Ų�ѯ�ͻ����� */			  
	ret = Cif_basic_inf_Sel( g_pub_tx.reply , &cif_basic_inf , 
					" cif_no=%ld " , g_ln_mst.cif_no );
	if( ret==100 )
	{
		sprintf(acErrMsg,"��֤����Ӧ�Ŀͻ��Ų�����[%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C020");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"ȡ�ͻ�������Ϣ�쳣![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C006");
		goto ErrExit;
	}				
	TRACE
	/* �ͻ������Ƿ�Ϻ���Ʒ���� */
	ret = pub_base_check_cltprdt( cif_basic_inf.type, g_ln_parm.prdt_no );
	if( ret )
	{
		sprintf(acErrMsg,"�˵��������ϴ����ƷҪ��,�����Ƿ����ڻ����ʳ���![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L233");
		ERR_DEAL;	
	}		
	TRACE
	
	
						
	/***** �����µĻ���ƻ� *********/
	ret = pub_ln_Chg_PayPlan( g_ln_mst , g_ln_parm , chg_rate , g_ln_mst.bal-g_pub_tx.tx_amt1 , g_pub_tx.tx_date , end_day );
	if( ret ) goto ErrExit;
	
	/* Pub_tx�ṹ��ֵ */
	g_pub_tx.ac_id = g_ln_mst.ac_id;		/* �˻���� */
	g_pub_tx.ac_seqn = g_ln_mst.ac_seqn;	/* �˻���� */
	strcpy ( g_pub_tx.ac_id_type , "3" );	/* �˺����� 3-���� */
	strcpy ( g_pub_tx.intst_type , "1" );	/* ��Ϣ���� */	
	strcpy ( g_pub_tx.add_ind , "0" );		/* ����:0��1��2���� */	
	strcpy ( g_pub_tx.ct_ind , "2" );		/* ��ת:1��2ת */	
	strcpy ( g_pub_tx.prt_ind , "0" );	/*�Ƿ��ӡ����(�������۱�־):1��0�� */
	strcpy ( g_pub_tx.hst_ind , "1" );	/*����ϸ:1�ռ���2���յ�����3���ջ�����*/
	strcpy( g_pub_tx.ac_wrk_ind , "0100000" );	
	g_pub_tx.svc_ind = 3060 ;
	strcpy ( g_pub_tx.brf,"���Ҵ����ƻ��޸�" );

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
    sprintf(acErrMsg,"���Ҵ����ƻ��޸ĳɹ���[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"���Ҵ����ƻ��޸�ʧ�ܣ�[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
    
int data_init_struct()
{
	memset (&g_ln_mst, 0x00, sizeof(struct ln_mst_c));
	memset (&g_dd_mst, 0x00, sizeof(struct dd_mst_c));
	memset (&g_mdm_ac_rel, 0x00, sizeof(struct mdm_ac_rel_c));
	memset (&cif_basic_inf, 0x00, sizeof(struct cif_basic_inf_c));
		
	memset(&ln_pay_pln ,0x00 , sizeof(struct ln_pay_pln_c) );
		
	return 0;
	
}
int data_get_struct()
{    
	int ret=0;

	sprintf(acErrMsg,"���븳ֵ����");
	WRITEMSG
	
	get_zd_data("0310",g_pub_tx.ac_no);		/* �ʺ� */
	get_zd_int("0350",&g_pub_tx.ac_seqn);		/* �˻���� */
	get_zd_long("0470",&end_day);			/* ������ */
	get_zd_double("1123",&chg_rate);		/* ���� */

	ret = pub_ln_lndis_file(g_pub_tx.ac_no,g_pub_tx.ac_seqn,
		&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm ); /*��ȡ�����Ϣ*/
	if( ret ) {return ret;}

	/*ret = pub_base_check_mdm_sts( g_mdm_ac_rel ) *������״̬*
	if( ret ) return ret; */
	
	/**
	ret = pub_ln_check_sts( g_ln_mst,0 ); *�����״̬*
	if( ret ) {return ret;}
	**/
					
	if( g_ln_parm.ln_pay_type[0]!='3' && g_ln_parm.ln_pay_type[0]!='4' )
	{
		sprintf(acErrMsg,"�ô���ǰ��Ҵ���![%s]",g_ln_parm.ln_pay_type);
		WRITEMSG
		strcpy( g_pub_tx.reply,"L042" );
		return 1;
	}
	return 0;
}

