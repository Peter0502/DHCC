/*************************************************
* �� �� ��: sp9111.c
* ��������������ҵ��ϵͳͨѶ
*
* ��    ��: rob
* ������ڣ�20030323
*
* �޸ļ�¼�� 
* ��   ��:
* �� �� ��:
* �޸�����:
*************************************************/

#include <stdio.h>
#include <math.h>
#include <errno.h>
#define EXTERN
#include "public.h"

int sp9111( )
{
    int ret;
    char comm[100];

    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"��ʼ�����ýṹ����!!");
        WRITEMSG
        goto ErrExit;
    }

    memset(comm,0x0,sizeof(comm));
    sprintf(comm,"tcpappsvr -p 6100 1>/home/xxx/log/null 2>/home/xxx/log/nul"); 
    ret = system(comm);
    if (ret != 0)
    {
        sprintf(acErrMsg,"����ҵ��ϵͳͨѶʧ��!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"O152");
        goto ErrExit;
    }

    memset(comm,0x0,sizeof(comm));
    sprintf(comm,"tcpappsvr 1>/home/xxx/log/null 2>/home/xxx/log/nul");
    ret = system(comm);
    if (ret != 0)
    {
        sprintf(acErrMsg,"����ҵ��ϵͳͨѶʧ��!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"O152");
        goto ErrExit;
    }

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"����ҵ��ϵͳͨѶ�ɹ� reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"����ҵ��ϵͳͨѶʧ�� reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
