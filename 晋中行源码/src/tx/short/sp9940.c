/*************************************************
* �� �� ��:    sp9940.c
* ����������   
*              ���ݽ�ݺ�ȡ����������Ϣ
*
* ��    ��:    lance
* ������ڣ�   2003��06��28��
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
	
sp9940()  
{ 		
	int ret=0;
	char sts[2];
	char pact_no[21];
	char cBr_no[6];
	char cMpact_no[21];
	struct ln_auth_c	ln_auth;
	memset( &ln_auth , 0x00 , sizeof( struct ln_auth_c ) );	
	memset(cBr_no,0x00,sizeof(cBr_no));
	memset(cMpact_no,0x00,sizeof(cMpact_no));
	/** ȡֵ����ֵ **/	
	
	get_zd_data( "0660" , sts );	/* ������־ 0��¼�� 1���޸� 2��ɾ�� */
	get_zd_data( "0630" , pact_no );	/* ��ݺ� */
  get_zd_data("0030",cBr_no);
	if( sts[0]=='0' )
	{
		goto OkExit;
	}
	
	/* ȡ�����Ϣ */
	ret = Ln_auth_Sel( g_pub_tx.reply , &ln_auth , "pact_no='%s'" , pact_no );
	if(ret&&ret!=100)
	{
		sprintf(acErrMsg,"��ȡ�������ļ���Ϣ�쳣![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L164");
		ERR_DEAL;		
	}else if( ret==100 ){
		sprintf(acErrMsg,"�˽�ݺŲ�����!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"L176");
		ERR_DEAL;		
	}
	/* ������� */
	sprintf(acErrMsg,"�����������");
	WRITEMSG

	set_zd_data("0250" , ln_auth.name);	/* ����� */
	set_zd_data("0380" , ln_auth.pay_ac_no);/* �����˺� */
	set_zd_data("0320" , ln_auth.save_ac_no);/* ����˻� */
	set_zd_double("0400" , ln_auth.amt);	/* ������ */
	set_zd_double("0840" , ln_auth.rate);	/* ��ͬ���� */	
	set_zd_long("0350" , ln_auth.time_lmt);	/* �������� */
		
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
   
