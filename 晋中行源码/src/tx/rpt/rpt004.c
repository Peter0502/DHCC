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

int rpt004()
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
    /****改为新加密形式  20120604
    strcpy(dbuser,getenv("DB_USER"));
    strcpy(dbpasswd,getenv("DB_PASSWD"));
    ****/
    memcpy(dbuser,get_env_info("DB_USER"),sizeof(dbuser));
    memcpy(dbpasswd,get_env_info("DB_PASSWD"),sizeof(dbpasswd));

    vtcp_log("[%s][%d]  DB_USER==[%s] DB_PASSWD==[%s]",__FILE__,__LINE__,dbuser,dbpasswd);

    sprintf(comm,"%s/%de/",path,s_com_sys_parm.lst_date);
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
    /*
    sprintf(filename,"cms%ld.dmp",s_com_sys_parm.lst_date);
    if(getenv("DB_NAME")!=NULL){
        sprintf(comm,"exp %s/%s@%s file=%s/%de/%s tables=com_sys_parm,ln_mst,dd_parm,mdm_ac_rel,ln_expand,ln_parm,com_branch,com_tel,mo_bank_po,mo_bank_acc_po,dd_mst,td_mst,gl_tsub,ln_reg,gl_mst_hst,ln_mst_hst 1>/dev/null 2>/dev/null",dbuser,dbpasswd,getenv("DB_NAME"),path,s_com_sys_parm.lst_date,filename);
    }else{
        sprintf(comm,"exp %s/%s file=%s/%de/%s tables=com_sys_parm,ln_mst,dd_parm,mdm_ac_rel,ln_expand,ln_parm,com_branch,com_tel,mo_bank_po,mo_bank_acc_po,dd_mst,td_mst,gl_tsub,ln_reg,gl_mst_hst,ln_mst_hst 1>/dev/null 2>/dev/null",dbuser,dbpasswd,path,s_com_sys_parm.lst_date,filename);
    }
    sprintf(acErrMsg,"comm[%s]",comm);
    WRITEMSG
    ret =  system(comm) ;
    if ( ret )
    {
        sprintf(acErrMsg,"为mis备份数据错误[%d]",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"S047");
        goto ErrExit;
    }
  */
	/* 将报表,mis数据传送 */
	/* system("sleep 60"); 先歇60秒再运行 */

	/****改为新加密形式  20120604
	sprintf(comm,"ftp_report.sh %d 1>/dev/null 2>&1 ",s_com_sys_parm.lst_date);
	****/
	sprintf(comm,"ftp_report.sh %d %s %s %s %s 1>/dev/null 2>&1 ",s_com_sys_parm.lst_date,get_env_info("REPORT_FTP_USER"),get_env_info("REPORT_FTP_PWD"),get_env_info("CRDIT_FTP_USER"),get_env_info("CRDIT_FTP_PWD"));
	WRITEMSG
	ret=system(comm);
	if ( ret )
	{
		vtcp_log("comm[%s][%d]",comm,ret);
		sprintf(acErrMsg,"打包、压缩报表失败! 为前台传送报表失败[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"S047");
		goto ErrExit;
	}
	vtcp_log("comm[%s][%d]",comm,ret);

	sprintf(comm,"ftp_to_spool.sh %d %s %s 1>/dev/null 2>&1 ",s_com_sys_parm.lst_date,get_env_info("SPOOL_FTP_USER"),get_env_info("SPOOL_FTP_PWD"));
	ret=system(comm);
	if ( ret )
	{
		vtcp_log("comm[%s][%d]",comm,ret);
		sprintf(acErrMsg,"为报表服务器传送报表失败[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"S047");
		goto ErrExit;
	}
	vtcp_log("comm[%s][%d]",comm,ret);


	/***为征信系统抽数***/
	/****已经在rpt092中加了处理，为什么还在此处加????  20121105
	ret=system("ftp_to_cis.sh 1>/dev/null 2>&1");
	vtcp_log("[%s][%d]ftp_to_cis.sh ret[%d]",__FILE__,__LINE__,ret);
	****/
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
