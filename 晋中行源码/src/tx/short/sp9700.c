/*************************************************
* �� �� ��:    sp9700.c
* ����������   
*              �����ʺŷ����˻����
*
* ��    ��:    rob
* ������ڣ�   2003��02��19��
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

sp9700()  
{ 		
	int ret=0,ac_seqn;
	char title[40],cur_no[3];
	char ac_no[20],name[50];

	/** ȡֵ����ֵ **/	
	get_zd_data("0310",ac_no);						/*�ʺ�*/
	pub_base_old_acno( ac_no );						/**�Ծ��ʺŵĴ���**/
	
	/* ���ø����ʺŷ����˻���ź��� */
	ret=pub_base_actoseqn(ac_no,&ac_seqn);			/*�˻����*/
	if(ret)
	{
		sprintf( acErrMsg, "���ú���pub_base_actoseqn����!!ret=[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	/* ������� */
	set_zd_int("0340",ac_seqn);

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
   
