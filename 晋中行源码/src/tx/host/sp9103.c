/*************************************************
* �� �� ��: sp9103.c
* �����������Ŵ�����
*
* ��    ��: rob
* ������ڣ�20030310
*
* �޸ļ�¼�� 
* ��   ��:
* �� �� ��:
* �޸�����:
*************************************************/

#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"

int sp9103( )
{
    int ret;
    char bffs[2];
    char cdqk[2];
    long bfrq;
    char comm[200],path[100];

    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"��ʼ�����ýṹ����!!");
        WRITEMSG
        goto ErrExit;
    }
 

    get_zd_data("0670",bffs);
    get_zd_data("0680",cdqk);
    get_zd_long("0440",&bfrq);

    strcpy(path,getenv("BFLDIR"));
    memset(comm,0x0,sizeof(comm));
    sprintf(comm,"%s",path);
    sprintf(acErrMsg,"%s",comm);
    WRITEMSG
    chdir(comm);

    memset(comm,0x0,sizeof(comm));

    if ( bffs[0]=='1' && cdqk[0]=='Y')
	{
        sprintf(comm,"tar cvf /dev/rmt0  RZQ%d ",bfrq);
	}
    else if ( bffs[0]=='1' && cdqk[0]=='N')
    {
        sprintf(acErrMsg,"û�з���Ŵ�!");
        WRITEMSG
        strcpy( g_pub_tx.reply,"O132" );
        goto ErrExit;
    }

    if ( bffs[0]=='2' && cdqk[0]=='Y')
	{
        sprintf(comm,"tar rvf /dev/rmt0 RZQ%d ",bfrq);
	}
    else if ( bffs[0]=='2' && cdqk[0]=='N')
    {
        sprintf(acErrMsg,"û�з���Ŵ�!");
        WRITEMSG
        strcpy( g_pub_tx.reply,"O132" );
        goto ErrExit;
    }

    pub_base_strpack(comm);
    sprintf(acErrMsg,"%s",comm);
    WRITEMSG
    if ( system(comm) )
    {
        sprintf(acErrMsg,"[%s]д�Ŵ�ʧ��",comm);
        WRITEMSG
        strcpy( g_pub_tx.reply,"O133" );
        goto ErrExit;
    }
    
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"�Ŵ����ݳɹ� reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"�Ŵ�����ʧ�� reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
