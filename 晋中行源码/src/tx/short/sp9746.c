/*************************************************
* �� �� ��:    sp9746.c
* ����������   
*              ��ѯ��ͬ���Ƿ����
*
* ��    ��:    lance
* ������ڣ�   2003��05��03��
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
#include "ln_auth_c.h"
	
sp9746()  
{ 		
	int ret=0;
	struct ln_auth_c  ln_auth;
	
	memset( &ln_auth , 0x00 , sizeof(struct ln_auth_c) );
	
	/** ȡֵ����ֵ **/	
	get_zd_data( "0630" , g_ln_mst.pact_no );	/* ֤������ */
	
	/* ��� ��ͬ�� */
	ret = Ln_auth_Sel( g_pub_tx.reply , &ln_auth , 
					" pact_no='%s' and sts!='0' " , g_ln_mst.pact_no );
	if( ret!=0 && ret!=100)
	{
		sprintf(acErrMsg,"��ȡ�������ļ���Ϣ�쳣![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L164");
		ERR_DEAL;
	}else if ( ret == 0 ){
		sprintf(acErrMsg,"��ͬ���Ѵ���![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L020");			
		ERR_DEAL;
	}
	
	sprintf(acErrMsg,"��� ��ͬ�� PASS!");
	WRITEMSG
	

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
   
