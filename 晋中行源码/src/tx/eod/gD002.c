/*************************************************************
* 文 件 名: gD002.c
* 功能描述：删除昨日报表,重新生成存放今日报表的目录
*
* 作    者: jack
* 完成日期: 2003年3月5日
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"
#include "com_branch_c.h"

gD002()
{
	int ret = 0;
	char command[64];
	char dirname[64];

	ret=sql_begin(); /*打开事务*/
	if( ret )
	{
		sprintf( acErrMsg, "打开事务失败!!!" );
		WRITEMSG
		goto ErrExit;
	}

    /**------- 初始化全局变量 --------**/
	pub_base_sysinit();

	/*------- 删除目录 -------*/
	memset( command,0x00,sizeof(command) );
	sprintf( command,"rm -f -r %s/report",getenv("HOME") );

	ret = system( command );
	if( ret )
	{
		sprintf( acErrMsg,"删除command[%s]错误[%d]run failed",command,ret );
		WRITEMSG  
		goto ErrExit;
	}

	sprintf( acErrMsg,"删除目录[%s]成功!!!",command );
	WRITEMSG

	/*------- 创建主目录 -------*/
	memset( dirname,0x00,sizeof(dirname) );
	sprintf( dirname,"%s/report",getenv("HOME") );
	ret = modify_dir_main(dirname);
	if( ret )
	{
		sprintf(acErrMsg,"创建目录[%s]失败",dirname );
		WRITEMSG
		goto ErrExit;
	}
	sprintf(acErrMsg,"创建日终报表主目录[%s]成功!!!",dirname );
	WRITEMSG

	/*-------- 创建机构报表存放目录 ----------*/
	ret = create_brno_dir();
	if( ret )
	{
		sprintf(acErrMsg,"创建机构报表目录失败");
		WRITEMSG
		goto ErrExit;
	}

OkExit:
	sql_commit();
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"整理报表目录成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();
	sprintf(acErrMsg,"整理报表目录失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

/*------------- 创建目录程序 -------------*/
int modify_dir_main(pathname)
char pathname[100];
{
	int  ret;

	ret=access(pathname,F_OK);
	if( ret && errno==ENOENT )
	{
		ret=mkdir(pathname,S_IRWXU|S_IRWXG|S_IRWXO);
		if( ret )
		{
			sprintf(acErrMsg,"mkdir failed err=%d,pathname=%s",errno,pathname);
			WRITEMSG
			strcpy(g_pub_tx.reply,"0210");
			goto ErrExit;
		}
		sprintf(acErrMsg,"目录创建成功[%s]",pathname);
		WRITEMSG
	}
	else if( ret )
	{
		sprintf(acErrMsg,"access() failed err=%d,pathname=%s",errno,pathname);
		WRITEMSG
		strcpy(g_pub_tx.reply,"0210");
		goto ErrExit;
	}

OkExit:
	return(0);
ErrExit:
	return(1);
}

/*------- 循环创建每个机构的报表存放目录 -------*/ 
int create_brno_dir()
{
	int ret = 0;
	char  pathname[100];

	struct com_branch_c sCom_branch;

	ret = Com_branch_Dec_Sel ( g_pub_tx.reply , "1 = 1 order by br_no");
	if( ret )
	{
		sprintf(acErrMsg,"执行Com_branch_Dec_Sel出错[%s]!!!",g_pub_tx.reply);
		WRITEMSG
		goto ErrExit;
	}

	while(1) 
	{
		memset(&sCom_branch,0x0,sizeof(sCom_branch));
		ret = Com_branch_Fet_Sel ( &sCom_branch, g_pub_tx.reply );
		if (ret == 100)
		{
			break;
		}
		if( ret )
		{
			sprintf(acErrMsg,"执行Com_branch_Fet_Sel出错[%s]!!!",g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}
		pub_base_strpack( sCom_branch.br_no );

		sprintf(pathname,"%s/report/%s",getenv("HOME"),sCom_branch.br_no );

		if( modify_dir_main( pathname ) ) 
		{
			goto ErrExit;
		}
	}

OkExit:
  	return 0;
ErrExit:
	return 1;
}
