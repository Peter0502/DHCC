/*************************************************************
* �� �� ��: spA019.c
* �����������ж��ֽ�ת��
* ��    ��: jack
* ������ڣ�2004��1��20��
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"

spA019()
{
	g_reply_int=0;

	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();

	get_zd_data("1171",g_pub_tx.ct_ind);
	if( g_pub_tx.ct_ind[0]=='1' )	/** �ֽ� **/
		goto ErrExit1;
	else if( g_pub_tx.ct_ind[0]=='2' )
		goto ErrExit2;

ErrExit:
	strcpy(g_pub_tx.reply,"P156");
	sprintf(acErrMsg,"�ж���ת��־ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
ErrExit1:
	strcpy(g_pub_tx.reply,"0001");
	sprintf(acErrMsg,"�ж���ת��־�ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit2:
	strcpy(g_pub_tx.reply,"0002");
	sprintf(acErrMsg,"�ж���ת��־�ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
}
