#include "find_debug.h" 
/*************************************************
* �� �� ��:    pubf_comm_SvrPub.c
* ����������
*              sql_err     �������ݿ����������Ϣ
*
* ��    ��:    jack
* ������ڣ�   2003��12��16��
* �޸ļ�¼��
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
*     2.
*************************************************/
#include    <stdio.h>
#define EXTERN
#include    "public.h"
#include    "svrpub_c.h"

char *sqlerr(char *tabl,int errcode)
{
    char errmesg[50];

    MEMSET_DEBUG(errmesg,0,sizeof(errmesg));
    sprintf(errmesg,"�������ݿ� [%s] �������� -> [%d]",tabl,errcode);

    return errmesg;
}
