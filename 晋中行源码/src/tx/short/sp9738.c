/*************************************************
* �� �� ��:    sp9738.c
* ����������   
*              ���ݿ��ŷ���ƾ֤����
*
* ��    ��:    
* ������ڣ�   2003��01��13��
* �޸ļ�¼��   
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#define EXTERN
#include "public.h"
#include "cif_basic_inf_c.h"
#include "cif_id_code_rel_c.h"

sp9738()  
{ 		
	int ret=0;
	char tmp_str1[20],tmp_str[20];
	char card_no[20];

	memset(tmp_str,0,sizeof(tmp_str));
	memset(tmp_str1,0,sizeof(tmp_str1));

	/** ȡֵ����ֵ **/	
	get_zd_data("1162",card_no);					/*����*/
	
	/* ����֤�����ͺ�֤������ȡ�ͻ��� */
	pub_card_CardToNote(card_no,tmp_str1);

	/* ������� */
	set_zd_data("1168", tmp_str1);

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
   
