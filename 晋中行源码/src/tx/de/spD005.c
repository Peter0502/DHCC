/***************************************************************
* �� �� ��:     spD005.c
* ����������	�ж��ֽ�ת��
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

spD005()
{
	get_zd_data("1171",g_pub_tx.ct_ind);
	if( g_pub_tx.ct_ind[0]=='1' )			/*** �ֽ� ***/
		goto ErrExit1;
	else if( g_pub_tx.ct_ind[0]=='2') 		/*** ת�� ***/
		goto ErrExit2;
    else if (g_pub_tx.ct_ind[0] == '3')		/*** ͬ�� ***/
        goto ErrExit3;
    else if (g_pub_tx.ct_ind[0] == '4')		/*** ��� ***/
        goto ErrExit4;
    else if (g_pub_tx.ct_ind[0] == '5')		/*** ���� ***/
        goto ErrExit5;

ErrExit:
	strcpy(g_pub_tx.reply,"P156");
	sprintf(acErrMsg,"�ж���ת��־ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
ErrExit1:
	strcpy(g_pub_tx.reply,"0001");
	sprintf(acErrMsg,"�ж���ת��־�ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit2:
	strcpy(g_pub_tx.reply,"0002");
	sprintf(acErrMsg,"�ж���ת��־�ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit3:
    strcpy(g_pub_tx.reply,"0003");
    sprintf(acErrMsg,"�ж���ת��־�ɹ�![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
ErrExit4:
    strcpy(g_pub_tx.reply,"0004");
    sprintf(acErrMsg,"�ж���ת��־�ɹ�![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
ErrExit5:
    strcpy(g_pub_tx.reply,"0005");
    sprintf(acErrMsg,"�ж���ת��־�ɹ�![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;

}
