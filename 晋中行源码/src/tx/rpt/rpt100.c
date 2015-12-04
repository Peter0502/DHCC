/*************************************************************
* 文 件 名: rpt100.c
* 功能描述：
*			年终试结时将试结后的报表打包并且变为1232日期
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

int rpt100()
{
	int ret=0;
	char	comm[1024];
	char filename[64],path[64],dbuser[16],dbpasswd[16];
	struct com_sys_parm_c	s_com_sys_parm;
	memset( &s_com_sys_parm , 0x00 , sizeof(struct com_sys_parm_c));

	/* 取前一天交易日期 */
	ret = Com_sys_parm_Sel( g_pub_tx.reply , &s_com_sys_parm , "1=1" );
	ERR_DEAL
	s_com_sys_parm.lst_date +=1; 	/* 试结生成的报表都试 1232日期 */
	sprintf(comm,"cp_rpt.sh %d",s_com_sys_parm.lst_date);
	ret=system(comm);
	vtcp_log("comm[%s][%d]",comm,ret);

/****
 sprintf(comm,"cp -R %s/report/ * %s/spool/%d/",
         getenv("HOME"),getenv("DATADIR"),s_com_sys_parm.lst_date);
	ret=system(comm);
	vtcp_log("CP comm[%s][%d]",comm,ret);

		char tmp_comm[128];
*****/

    strcpy(path,getenv("BFLDIR"));

	/* 将报表 */
	/****改为新加密形式  20121105
	sprintf(comm,"ftp_report.sh %d 1>/dev/null 2>&1 ",s_com_sys_parm.lst_date);
	****/

	sprintf(comm,"ftp_report.sh %d %s %s %s %s 1>/dev/null 2>&1 ",s_com_sys_parm.lst_date,get_env_info("REPORT_FTP_USER"),get_env_info("REPORT_FTP_PWD"),get_env_info("CRDIT_FTP_USER"),get_env_info("CRDIT_FTP_PWD"));

	ret=system(comm);
	vtcp_log("comm[%s][%d]",comm,ret);
/*
	sprintf(comm,"pack_rpt.sh %d >/dev/null 2>&1 ",g_pub_tx.tx_date);
	system(comm);
	vtcp_log(comm);

	sprintf(comm,"rm -f %s/rpt*.Z >/dev/null 2>&1 ",getenv("FILDIR"));
	system(comm);
	vtcp_log(comm);
*/
GoodExit:
	strcpy(g_pub_tx.reply,"0000");
	return 0;
ErrExit:
	return 1;
}
