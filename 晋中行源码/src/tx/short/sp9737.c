/*************************************************
* �� �� ��:    sp9737.c
* ����������   
*              �����Ʒ���뷵�ز�Ʒ��Ϣ
*
* ��    ��:    lance
* ������ڣ�   2003��06��06��
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
	
sp9737()  
{ 		
	int ret=0;
	char prod_no[4];
	double rate_val , over_rate_val;
	
	
	/** ���ݳ�ʼ�� **/
	/** ��ʼ��ȫ�ֱ��� **/
	pub_base_sysinit();
	
	/** ȡֵ����ֵ **/	
	
	get_zd_data( "0240" , prod_no );	/* ��Ʒ���� */
	
	/* ���ݲ�Ʒ���� ���ز�Ʒ���� */

	ret = pub_base_get_prdtparm( prod_no );
	if( ret )
	{
		sprintf(acErrMsg,"���ø��ݲ�Ʒ����ȡ��Ʒ������������");
		WRITEMSG
		goto ErrExit;
	}		  
	
	/* ������� */
	set_zd_data( "0710", g_ln_parm.time_type );	/* ��Ʒ���� */

			
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
   
