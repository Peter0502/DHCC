/*************************************************
* �� �� ��:    sp9703.c
* ����������   
*              �����ʺŻ����˻����[����]
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

sp9703()  
{ 		
	int ret=0,ac_seqn;
	char ac_no[20];

	/** ȡֵ����ֵ **/	
	get_zd_data("1011",ac_no);						/*�ʺ�*/
	
	/* ���ø����ʺŷ����˻���ź��� */
	ret=pub_base_actoseqn(ac_no,&ac_seqn);			/*�˻����*/
	if(ret)
	{
		sprintf( acErrMsg, "���ú���pub_base_actoseqn����!!ret=[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	vtcp_log("��ѯ�������Ϊ:[%ld]",ac_seqn);

	/* ������� */
	set_zd_int("1012",ac_seqn);

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
   
