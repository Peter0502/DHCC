/***************************************************************
* �� �� ��:     spD403.c
* ����������	��Ϣ����
*
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

spD403()
{
    char lx[2];

	 /*��սṹ*/
    if (init_struct())
    {
        strcpy(acErrMsg,"�ṹ��ճ���!");
        WRITEMSG
        goto ErrExit;
    }

    /*** ��ʼ��ȫ�ֱ��� ***/
    if (pub_base_sysinit())
    {
        strcpy(acErrMsg,"��ʼ��ȫ�ֱ�������!");
        WRITEMSG
        goto ErrExit;
    }
    


    get_zd_data("0670",lx);
    if (lx[0] == '2')
    {
        goto ErrExit;
    }

	/*** ������� ***/
OkExit:
	sprintf(acErrMsg,"��Ϣ����Ϣ�ӽ���!! [%s]",lx);
	WRITEMSG
	strcpy(g_pub_tx.reply,"0000");
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"�����ʻ���Ϣ�ӽ���!! [%s]",lx);
	WRITEMSG
    strcpy(g_pub_tx.reply,"0003");
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
}

int init_struct()
{
    return 0;
}
