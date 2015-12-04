/*************************************************
* �� �� ��:    sp9805.c
* ����������   
*              �����Ʒ���뷵�ز�Ʒ��Ϣ
*
* ��    ��:    lance
* ������ڣ�   2003��01��13��
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
	
sp9805()  
{ 		
	int ret=0;
	char prod_no[4];
	double rate_val , over_rate_val;
	/* begin add by LiuHuafeng 2008-11-27 12:18:03 */
	rate_val=0.0;
	over_rate_val=0.0;
	/* end by LiuHuafeng 2008-11-27 12:18:10 */
	/** ���ݳ�ʼ�� **/
	/** ��ʼ��ȫ�ֱ��� **/
	pub_base_sysinit();
	
	/** ȡֵ����ֵ **/	
	
	get_zd_data( "0230" , prod_no );	/* ��Ʒ���� */
	
	/* ���ݲ�Ʒ���� ���ز�Ʒ���� */

	ret = pub_base_get_prdtparm( prod_no );
	if( ret )
	{
		sprintf(acErrMsg,"���ø��ݲ�Ʒ����ȡ��Ʒ������������");
		WRITEMSG
		goto ErrExit;
	}		  
	sprintf(acErrMsg,"����[%s]",g_ln_parm.title);
	WRITEMSG

	sprintf(acErrMsg,"���ʱ��[%s]",g_ln_parm.rate_no);
	WRITEMSG
	sprintf(acErrMsg,"����[%s]",g_ln_parm.cur_no);
	WRITEMSG
	sprintf(acErrMsg,"����[%ld]",g_pub_tx.tx_date);
	WRITEMSG
	sprintf(acErrMsg,"�Զ�����[%s]",g_ln_parm.auto_pay_ind);
	WRITEMSG	
	
	/* �������ʱ��ȡ����ֵ */
	ret = pub_base_getllz( g_ln_parm.rate_no , g_ln_parm.cur_no , g_pub_tx.tx_date , &rate_val );
	if( ret )
	{
		sprintf(acErrMsg,"���ø������ʱ��ȡ��������ֵ��������");
		WRITEMSG
		goto ErrExit;
	}		  
	sprintf(acErrMsg,"��������ֵ[%lf]",rate_val);
	WRITEMSG
	rate_val = rate_val/1.2;/* ת���������� */
	vtcp_log("%s,%d over_rate_type==%s,over_rate_def=%f",__FILE__,__LINE__,g_ln_parm.over_rate_type,g_ln_parm.over_rate_def*1.0);
	if( g_ln_parm.over_rate_type[0]=='0' )/* ����ȡ�������ʱ�� */
	{
		ret = pub_base_getllz( g_ln_parm.over_rate_no , g_ln_parm.cur_no , g_pub_tx.tx_date , &over_rate_val );
		if( ret )
		{
			sprintf(acErrMsg,"���ø������ʱ��ȡ��������ֵ��������");
			WRITEMSG
			goto ErrExit;
		}
		over_rate_val = over_rate_val/1.2;/* ת���������� */
	}
	if( g_ln_parm.over_rate_type[0]=='0' )/* ����ȡ�������ʱ�� */
	{
		over_rate_val = rate_val*(100+g_ln_parm.over_rate_def)/100;
	}
				  
	sprintf(acErrMsg,"��������ֵ[%lf]",over_rate_val);
	WRITEMSG


	sprintf(acErrMsg,"��������[%s]",g_ln_parm.ln_type);
	WRITEMSG
	/* ������� */
	set_zd_data( "0810", g_ln_parm.title );	/* ��Ʒ���� */
	set_zd_data( "0210", g_ln_parm.cur_no );/* ���� */
	set_zd_data( "0680", g_ln_parm.get_rate_type );/* ����ȡ�÷�ʽ */	
	set_zd_double( "0970", rate_val );	/* �������� */
	set_zd_double( "0840", rate_val );	/* �������� */
	set_zd_double( "0850", over_rate_val );	/* �������� */	
	set_zd_data( "0690", g_ln_parm.ln_pay_type );/* ����֧����ʽ */	
	set_zd_data( "0700", g_ln_parm.auto_pay_ind );/* �Զ������־ */	
	set_zd_data( "0710", g_ln_parm.intst_type );/* ��Ϣ���� */	
	set_zd_data( "0720", g_ln_parm.pay_pln_type );/* �ſ�ƻ����� */	
	set_zd_data( "0660", g_ln_parm.pay_pln_crt );/* ����ƻ��������� */		
	set_zd_data( "105A", g_ln_parm.trust_prot_ind );/* �Ƿ�ί��Э����� */	
	set_zd_data( "1145", g_ln_parm.ln_type );/* �������� */
			
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
   
