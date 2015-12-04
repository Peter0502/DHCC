#include "find_debug.h" 
/*************************************************
* �� �� ��:    pubf_comm_app.c
* ����������
*              Encrypt     ͨѶ�����ܺ���
*              bal_open_database     �����ݿ�
*              bal_close_database    �ر����ݿ�
*
* ��    ��:    jack
* ������ڣ�   2003��12��16��
* �޸ļ�¼��
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
*     2.
*************************************************/
# include <stdio.h>
#define EXTERN
# include "public.h"
# include "com_sys_parm_c.h"
# include "svrpub_c.h"

/*******************************************/
int Encrypt(char *string,int len,char *key,int flag)
{
    register i;
    int     klen;

    klen=strlen(key);

    for(i=0;i<len;i++) 
    {
        if(flag) string[i]=string[i]+key[i%klen];
        else string[i]=string[i]-key[i%klen];

    }

    return 0;
}

long bal_open_database()
{

    if ( sql_database( RtCode ) )
    {
        vtcp_log("connect error");
        return -1;
    }
    if ( bal_set_lockwaittime( ) )
    {
		vtcp_log("set lock mode to wait 5 err");
        return -1;
	}

    return 0;
}

long bal_close_database()
{
    if ( sql_close( ) )
    {
        return -1;
    }

    return 0;
}
