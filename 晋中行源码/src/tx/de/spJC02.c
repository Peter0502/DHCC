/***************************************************************
* �� �� ��:     spJC02.c
* ����������	�жϴ�ȡ����
* ��    ��:     jack
* ������ڣ�    2004��1��10��
*
* �޸ļ�¼��
*    ��    ��:
*    �� �� ��:
*    �޸�����:
*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"

spJC02()
{
	char flag[2];

	memset(flag,0,sizeof(flag));
	get_zd_data("0660",flag);


	if( flag[0]=='1' )		/*** �� ***/
		goto ErrExit1;
	else if( flag[0]=='2') 		/*** �� ***/
		goto ErrExit2;
        else 
                goto ErrExit;

ErrExit:
	strcpy(g_pub_tx.reply,"P156");
	sprintf(acErrMsg,"�жϽ����־ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
ErrExit1:
	strcpy(g_pub_tx.reply,"0001");
	sprintf(acErrMsg,"�жϽ����־�ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit2:
	strcpy(g_pub_tx.reply,"0002");
	sprintf(acErrMsg,"�жϽ����־�ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
}
