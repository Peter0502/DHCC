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

#include   <stdio.h>
#include   <time.h>
#define    EXTERN
#include   "svrpub_c.h"

int pubf_comm_free_buf()
{
	free(Pub_Buf);
	free(Old_Pub_Buf);
	free(Pu_Val);
	free(Old_Pu_Val);
	return 0;
}
