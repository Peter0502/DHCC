/*************************************************
* �� �� ��:    sp9741.c
* ����������   
*              �����ʺŻ����ʻ����
*
* ��    ��:    lance
* ������ڣ�   2003��03��14��
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
#include "prdt_ac_id_c.h"

sp9741()  
{ 		
	int ret=0,ac_seqn;
	char title[40];
	char ac_no[20];
	double bal;

	struct prdt_ac_id_c g_prdt_ac_id;

	memset(&g_prdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));

	/** ȡֵ����ֵ **/	
	get_zd_data("0300",ac_no);							/*�ʺ�*/
	if(pub_base_old_acno( ac_no ))                 /**�Ծ��ʺŵĴ���**/
		goto ErrExit;

	/*�����ʺ��ҵ��˻����*/
	pub_base_actoseqn(ac_no,&ac_seqn);
TRACE

    set_zd_int("0340",ac_seqn);                         /*���*/
    set_zd_data("0300",ac_no);                          /*�ʺ�*/

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

