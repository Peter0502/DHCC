/*************************************************************
* 文 件 名: rpt092.c
* 功能描述：
*		为征信提供数据，暂停反洗钱提取数据，反洗钱目前通过ODS提供数据
* 作    者: 
* 完成日期: 
* 修改记录：
* 日    期: 2007年10月29日
* 修 改 人: chenhw
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

int rpt092()
{
	int ret=0;
	char	comm[1024];
	char filename[64],path[64],dbuser[16],dbpasswd[16];
	struct com_sys_parm_c	s_com_sys_parm;
	memset( &s_com_sys_parm , 0x00 , sizeof(struct com_sys_parm_c));

	/* 取前一天交易日期 */
	ret = Com_sys_parm_Sel( g_pub_tx.reply , &s_com_sys_parm , "1=1" );
	ERR_DEAL
	
	memset(comm,0,sizeof(comm));
  strcpy(path,getenv("HOME"));

  strcpy(dbuser,get_env_info("DB_USER"));
  strcpy(dbpasswd,get_env_info("DB_PASSWD"));
/*  
  sprintf(comm,"%s/%s/",path,"fxmoneydata");
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
	**/
	
 /*为征信系统提取数据 begin 20120628 */
	sprintf(comm,"ftp_to_cis.sh %s %s %s %s %d",get_env_info("CIS_USER"),get_env_info("CIS_PWD"),get_env_info("DB_USER"),get_env_info("DB_PASSWD"),s_com_sys_parm.lst_date);
	sprintf(acErrMsg,"comm[%s]",comm);
	ret=system(comm);
	if ( ret )
	{
	    sprintf(acErrMsg,"为征信系统提取数据错误[%d],[%s]",ret,comm);
	    WRITEMSG
	    strcpy(g_pub_tx.reply,"AT13");
	    goto ErrExit;
	}
	sprintf(acErrMsg,"为征信系统提取数据完成[%s]",comm);
	WRITEMSG
   /*为征信系统提取数据 end  20120628 */

GoodExit:
	strcpy(g_pub_tx.reply,"0000");
	return 0;
ErrExit:
	return 1;
}
