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
#include   "public.h"
#include   "svrpub_c.h"

void pubf_comm_free_buf()
{
	vtcp_log("[%s][%d]��ʼ��������ڴ�ռ亯��!\n",__FILE__,__LINE__);
	free(Pub_Buf);
	free(Old_Pub_Buf);
	free(Pu_Val);
	free(Old_Pu_Val);
	vtcp_log("[%s][%d]��ϲ!��ϲ!�ڴ�ռ䱻������������.......\n",__FILE__,__LINE__);
}
