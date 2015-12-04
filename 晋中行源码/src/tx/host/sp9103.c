/*************************************************
* 文 件 名: sp9103.c
* 功能描述：磁带备份
*
* 作    者: rob
* 完成日期：20030310
*
* 修改记录： 
* 日   期:
* 修 改 人:
* 修改内容:
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
        sprintf(acErrMsg,"初始化公用结构错误!!");
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
        sprintf(acErrMsg,"没有放入磁带!");
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
        sprintf(acErrMsg,"没有放入磁带!");
        WRITEMSG
        strcpy( g_pub_tx.reply,"O132" );
        goto ErrExit;
    }

    pub_base_strpack(comm);
    sprintf(acErrMsg,"%s",comm);
    WRITEMSG
    if ( system(comm) )
    {
        sprintf(acErrMsg,"[%s]写磁带失败",comm);
        WRITEMSG
        strcpy( g_pub_tx.reply,"O133" );
        goto ErrExit;
    }
    
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"磁带备份成功 reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"磁带备份失败 reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
