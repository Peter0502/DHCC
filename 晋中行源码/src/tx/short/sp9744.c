/*************************************************
* �� �� ��:    sp9744.c
* ����������   
*              ������Ȩ����������Ϣ
*
* ��    ��:    lance
* ������ڣ�   2003��03��25��
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
#include "auth_code_c.h"

sp9744()  
{ 		
	int ret=0;
	char auth_no[51];


    struct auth_code_c          auth_code;
    
TRACE	
	memset (&auth_code, 0x00, sizeof(struct auth_code_c));	



TRACE	
	/** ���ݳ�ʼ�� **/
	/** ��ʼ��ȫ�ֱ��� **/
	pub_base_sysinit();
	
TRACE	
	/** ȡֵ����ֵ **/	
	
	get_zd_data( "0250" ,auth_no );                            	/* ��Ȩ�� */


	/*������Ȩ���ѯδ���˽�����Ϣ*/
    ret=Auth_code_Sel(g_pub_tx.reply,&auth_code,
				  " auth_no=%s " ,auth_no);
   	
	if( ret==100 )
    {
         sprintf(acErrMsg,"����Ȩ�벻����[%s]",g_pub_tx.reply);
		 WRITEMSG
		 strcpy(g_pub_tx.reply,"E022");
		 goto ErrExit;
    }
    else if( ret )
   {
    	 sprintf(acErrMsg,"ȡ��Ȩ�������Ϣ�쳣![%s]",g_pub_tx.reply);
		 WRITEMSG
		 strcpy(g_pub_tx.reply,"E023");
		 goto ErrExit;
   }



	/* ������� */
	set_zd_long( "0290", auth_code.trace_no );                 /* ��ˮ�� */
	set_zd_data( "0640", auth_code.tx_code );                  /* ���״��� */
	set_zd_data( "0670", auth_code.auth_ind );                 /* ��Ȩ��־ */
	set_zd_long( "0340", auth_code.cond_seqn );                /* ������� */
	set_zd_data( "0680", auth_code.sts );                    	 /* ʹ��״̬ */
	set_zd_data( "0580", auth_code.tel );	                     /* ����Ա */	
	set_zd_data( "0590", auth_code.auth );                     /* ��Ȩ�� */
	set_zd_data( "0600", auth_code.aud );                      /* ������ */	
	set_zd_data( "0610", auth_code.chk );                      /* ����Ա */	
				
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


