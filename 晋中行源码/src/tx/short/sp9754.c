/*************************************************
* �� �� ��:    sp9754.c
* ����������   
*              ��ͬ���ʷ������ںͷ�Ϣ����
*
* ��    ��:    lance
* ������ڣ�   2003��05��25��
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
	
sp9754()  
{ 		
	int ret=0;
	char prod_no[4];
	double rate_val , over_rate_val , fine_rate_val;
	
	
	/** ���ݳ�ʼ�� **/
	/** ��ʼ��ȫ�ֱ��� **/
	pub_base_sysinit();
	
	/** ȡֵ����ֵ **/	
	
	get_zd_data( "0230" , prod_no );	/* ��Ʒ���� */
	get_zd_double( "0840" , &rate_val );	/* ��ͬ���� */
	
	/* ���ݲ�Ʒ���� ���ز�Ʒ��Ϣ */

	sprintf(acErrMsg,"��Ʒ����[%s]",prod_no);
	WRITEMSG	
	ret = pub_base_get_prdtparm( prod_no );
	if( ret )
	{
		sprintf(acErrMsg,"���ø��ݲ�Ʒ����ȡ��Ʒ������������");
		WRITEMSG
		goto ErrExit;
	}		  	
	/** ������������ֵ **/	
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
	if( g_ln_parm.over_rate_type[0]=='1' )/* ����ȡ�������ʱ�� */
	{
		over_rate_val = rate_val*(100+g_ln_parm.over_rate_def)/100;
	}
				  
	sprintf(acErrMsg,"��������ֵ[%lf]",over_rate_val);
	WRITEMSG

	/** ���㷣Ϣ����ֵ **/
	if( g_ln_parm.fine_rate_type[0]=='0' )/* ��Ϣȡ��Ϣ���ʱ�� */
	{
		ret = pub_base_getllz( g_ln_parm.fine_rate_no , g_ln_parm.cur_no , g_pub_tx.tx_date , &fine_rate_val );
		if( ret )
		{
			sprintf(acErrMsg,"���ø������ʱ��ȡ��Ϣ����ֵ��������");
			WRITEMSG
			goto ErrExit;
		}
		fine_rate_val = fine_rate_val/1.2;/* ת���������� */
	}
	if( g_ln_parm.fine_rate_type[0]=='1' )/* ��Ϣȡ��Ϣ���ʱ�� */
	{
		fine_rate_val = rate_val*(100+g_ln_parm.fine_rate_def)/100;
	}
				  
	sprintf(acErrMsg,"��Ϣ����ֵ[%lf]",fine_rate_val);
	WRITEMSG
	
	
	/* ������� */
	set_zd_double( "0850", over_rate_val );	/* �������� */
	set_zd_double( "0940", fine_rate_val );	/* ��Ϣ���� */		
			
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
   
