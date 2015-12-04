/*************************************************
* �� �� ��:    sp9835.c
* ����������   
*              ���ݴ����˺�,�˻���ŷ��ش�����Ϣ
*
* ��    ��:    lance
* ������ڣ�   2003��01��25��
* �޸ļ�¼��   
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
*     2. 
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#define EXTERN
#include "public.h"
#include "cif_basic_inf_c.h"
#include "cif_id_code_rel_c.h"

sp9835()  
{ 		
	int ret=0;
	char ac_no[20];
	int ac_seqn,back_ac_seqn;
	
	struct cif_basic_inf_c        cif_basic_inf; 
	struct cif_id_code_rel_c      cif_id_code_rel; 
	
	memset (&cif_basic_inf, 0x00, sizeof(struct cif_basic_inf_c));	
	memset (&cif_id_code_rel, 0x00, sizeof(struct cif_id_code_rel_c));	
		
	/** ���ݳ�ʼ�� **/
	/** ��ʼ��ȫ�ֱ��� **/
	pub_base_sysinit();
	
	/** ȡֵ����ֵ **/	
	
	get_zd_data( "0310" , ac_no );	/* �����ʺ� */
	pub_base_old_acno( ac_no );  	/** �Ծ��ʺŵĴ��� **/
	set_zd_data("0310",ac_no);	/* ������ʺ� */	
		
	get_zd_int( "0350" , &ac_seqn );	/* �˻���� */
	
	/* ��ȡ�����Ϣ */

	ret = pub_ln_lndis_file( ac_no, ac_seqn,&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm );
	if( ret ) {return ret;}	
	
	/* ���ݿͻ��Ų�ѯ�ͻ���Ϣ */		  
	ret = Cif_basic_inf_Sel( g_pub_tx.reply , &cif_basic_inf , 
					" cif_no=%ld " , g_ln_mst.cif_no );
	if( ret==100 )
	{
		sprintf(acErrMsg,"�ÿͻ��Ų�����[%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C007");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"ȡ�ͻ�������Ϣ�쳣![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C006");
		goto ErrExit;
	}
	
	/* ���ݿͻ��Ų�ѯ�ͻ�֤����Ϣ */		  
	ret = Cif_id_code_rel_Sel( g_pub_tx.reply , &cif_id_code_rel , 
					" cif_no=%ld " , g_ln_mst.cif_no );
	if( ret==100 )
	{
		sprintf(acErrMsg,"�ÿͻ��Ų�����[%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C007");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"ȡ�ͻ�֤����Ϣ�쳣![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C004");
		goto ErrExit;
	}						  
	
	/* ��ѯ�����˻���Ϣ */
	ret = pub_base_CurToSeqn( g_ln_parm.cur_no , &back_ac_seqn );
	if( ret ) {return ret;}	
	if( g_ln_mst.repay_ac_id !=0 )
	{
		ret = pub_base_currentid_file( g_ln_mst.repay_ac_id,back_ac_seqn, &g_dd_mst );
		if( ret ) {return ret;}

		Mdm_ac_rel_Sel( g_pub_tx.reply , &g_mdm_ac_rel , 
						" ac_id=%ld " , g_ln_mst.repay_ac_id );
		if( ret ) {return ret;}	
	}
	
	/* ������� */
	set_zd_data( "0270", g_ln_mst.name );/* ����� */
	set_zd_data( "0230", g_ln_mst.prdt_no );/* ��Ʒ��� */
	set_zd_data( "0810", g_ln_parm.title );	/* ��Ʒ���� */
	set_zd_data( "0630", g_ln_mst.pact_no );	/* ��ͬ�� */	
	set_zd_data( "0610", g_ln_mst.cal_code );	/* ָ����ϵ���� */		
	set_zd_data( "0210", g_ln_parm.cur_no );/* ���� */
	set_zd_data( "0680", g_ln_parm.get_rate_type );/* ����ȡ�÷�ʽ */	
	set_zd_double( "0840", g_ln_mst.rate );	/* �������� */
	set_zd_double( "0400", g_ln_mst.advis_amt_pln );	/* ������ʷ� */	
	set_zd_double( "0850", g_ln_mst.over_rate );	/* �������� */		
	set_zd_data( "0690", g_ln_parm.ln_pay_type );/* ����֧����ʽ */	
	set_zd_data( "0700", g_ln_parm.auto_pay_ind );/* �Զ������־ */	
	set_zd_data( "0710", g_ln_parm.intst_type );/* ��Ϣ���� */
	set_zd_long( "0440", g_ln_mst.opn_date );	/* �������� */	
	set_zd_long( "0450", g_ln_mst.mtr_date );	/* �������� */		
	set_zd_double( "0430", g_ln_mst.amt_lmt );	/* �ƻ����Ŷ� */
	set_zd_data( "0250", cif_basic_inf.name );	/* �ͻ����� */	
	set_zd_data( "0670", cif_id_code_rel.id_type );	/* ֤������ */		
	set_zd_data( "0620", cif_id_code_rel.id_no );	/* ֤������ */		
	set_zd_data( "0260", g_dd_mst.name );	/* �����˻����� */
	if( g_ln_mst.repay_ac_id !=0 )
	{
		set_zd_int ( "0340", back_ac_seqn );	/* �����˻���� */
		set_zd_data( "0380", g_mdm_ac_rel.ac_no );	/* �����˻� */	
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
   
