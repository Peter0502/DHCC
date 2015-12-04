/*************************************************************
* 文 件 名: rpt004.c
* 功能描述：
*			日终报表打包处理...
*			日终生成的报表进行打包，压缩和备份 为mis准备数据
* 作    者: 
* 完成日期: 
* 修改记录：
* 日    期: 2003年4月29日
* 修 改 人: rob
* 修改内容:
**************************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "com_sys_parm_c.h"

extern char *get_env_info(char *infoname);

int rpt802()
{
	int ret=0;
	char	comm[1024];
	char filename[64],path[64],dbuser[16],dbpasswd[16];
	struct com_sys_parm_c	s_com_sys_parm;

	memset(dbuser,0x00,sizeof(dbuser));
	memset(dbpasswd,0x00,sizeof(dbpasswd));
	memset( &s_com_sys_parm , 0x00 , sizeof(struct com_sys_parm_c));

	/* 取前一天交易日期 */
	ret = Com_sys_parm_Sel( g_pub_tx.reply , &s_com_sys_parm , "1=1" );
	ERR_DEAL
	 


    strcpy(path,getenv("BINDIR"));
    /****改为新加密形式  20120604
    strcpy(dbuser,getenv("DB_USER"));
    strcpy(dbpasswd,getenv("DB_PASSWD"));
    ****/
    memcpy(dbuser,get_env_info("DB_USER"),sizeof(dbuser));
    memcpy(dbpasswd,get_env_info("DB_PASSWD"),sizeof(dbpasswd));

    vtcp_log("[%s][%d]  DB_USER==[%s] DB_PASSWD==[%s]",__FILE__,__LINE__,dbuser,dbpasswd);

    sprintf(comm,"%s/",path);
    ret = chdir(comm);
    sprintf(acErrMsg,"comm[%s]",comm);
    WRITEMSG
    if ( ret )
    {
        sprintf(acErrMsg,"进入目录失败[%d]",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"O141");
        goto ErrExit;
    }

    memset(comm,0,sizeof(comm));
    memset(filename,0,sizeof(filename));

	sprintf(comm,"cdktj.sh %d %s %s 1>/dev/null 2>&1 ",s_com_sys_parm.lst_date,get_env_info("DB_USER"),get_env_info("DB_PASSWD"));
	ret=system(comm);
	if ( ret )
	{
		vtcp_log("comm[%s][%d]",comm,ret);
		sprintf(acErrMsg,"存贷款统计失败1[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"S047");
		goto ErrExit;
	}
	vtcp_log("comm[%s][%d]",comm,ret);

GoodExit:
	strcpy(g_pub_tx.reply,"0000");
	return 0;
ErrExit:
	return 1;
}
