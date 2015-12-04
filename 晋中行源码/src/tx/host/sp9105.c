/*************************************************
* 文 件 名: sp9105.c
* 功能描述：数据恢复
*
* 作    者: rob
* 完成日期：20030322
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

int sp9105( )
{
    int ret;
    long qlrq;
    char dev_type[2];
    char str_tmp[256];
    char comm[30];
    char db_path[30],db_name[20];

    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"初始化公用结构错误!!");
        WRITEMSG
        goto ErrExit;
    }
 
    get_zd_data("0670",dev_type);
	get_zd_long("0440",&qlrq);

    memset(db_path, 0x00, sizeof(db_path));
	memset(db_name, 0x00, sizeof(db_name));

	strcpy(db_path, getenv("BFLDIR"));
	strcpy(db_name, getenv("DBNAME"));

    sprintf(acErrMsg,"===[%s]===",dev_type);
    WRITEMSG

	/******备份在磁带上******/
	if ( dev_type[0] == '1' )
    {
		sprintf(str_tmp,"%s",db_path);
		chdir(str_tmp);
		sprintf(comm,"tar xv RZQ%d",qlrq);
		sprintf(acErrMsg,"取磁带备份 [%s]",comm);
        WRITEMSG

		if (system(comm))
        {
			sprintf(acErrMsg,"磁带上无此日期备份 [%s]",comm);
            WRITEMSG
			strcpy(g_pub_tx.reply,"O143");
            goto ErrExit;
		}
	}

    /***** 备份在硬盘上 ****/
    /***  进入到备份目录  ***/
	sprintf(acErrMsg,"cd [%s]",db_path);
    WRITEMSG
	chdir(db_path);
		
	/*** 事先清理目录  **/
	sprintf( comm, "rm -fr %d", qlrq);
	system( comm );

	/***  tar 备份文件  ***/
	sprintf( comm, "tar xvf RZQ%d", qlrq );
	sprintf(acErrMsg,"取硬盘备份 [%s]",comm);

	if ( system(comm) )
    {
		sprintf(acErrMsg,"硬盘上无此日期备份 [%s]",comm);
        WRITEMSG
		strcpy(g_pub_tx.reply,"O143");
        goto ErrExit;
	}
	sprintf(acErrMsg,"tar xvf succuess");
    WRITEMSG

    /***  进入到tar 之后的目录:以时间为目录  ***/
	memset(str_tmp, 0x00, sizeof(str_tmp));
	sprintf(str_tmp,"%s/%d/%s.exp",db_path,qlrq,db_name);
	chdir(str_tmp);

	sprintf(acErrMsg,"[%s]",str_tmp);
    WRITEMSG
	/*** 解压文件 ***/
	sprintf( comm, "%s", "uncompress *.Z");
	if (system(comm))
    {
		sprintf(acErrMsg,"解压缩错误 [%s]",comm);
        WRITEMSG
		strcpy(g_pub_tx.reply,"O144");
        goto ErrExit;
	}
	sprintf(acErrMsg,"解压文件成功");
    WRITEMSG

    ret = sql_commit();
    if (ret != 0)
    {
        sprintf(acErrMsg,"提交事务错误!!");
        WRITEMSG
        goto ErrExit;
    }

    ret = sql_close();
    if (ret != 0)
    {
        sprintf(acErrMsg,"关闭数据库错误!!!");
        WRITEMSG
        goto ErrExit;
    }

    ret = sql_drop();
    if (ret != 0)
    {
        sprintf(acErrMsg,"删除数据库错误!!!");
        WRITEMSG
        goto ErrExit;
    }
    sprintf(acErrMsg,"删除数据库成功 ");
    WRITEMSG

    /***  开始导入数据库  ***/
	sprintf(str_tmp, "%s/%d", db_path, qlrq);
	chdir(str_tmp);
    sprintf(acErrMsg,"str_tmp=[%s]",str_tmp);

	sprintf(comm,"dbimport %s -d datadbs",db_name);
	if (system(comm))
    {
		sprintf(acErrMsg,"恢复数据库失败 [%s]",comm);
		strcpy(g_pub_tx.reply,"O145");
        goto ErrExit;
	}
	sprintf(acErrMsg,"恢复数据库成功 ");
    WRITEMSG

    sleep(20);
	/***  启动日志  ***/
	sprintf(comm,"ontape -s -U %s 1>/tmp/a.txt 2>>/tmp/a.txt",db_name);
	if (system(comm))
	{
		sprintf(acErrMsg,"启动日志失败 [%s]",comm);
        WRITEMSG
		strcpy(g_pub_tx.reply,"O146");
		return 1;
	}
	sprintf(acErrMsg,"启动日志成功 ");
	sleep(60);
	/****  硬盘恢复数据库结束  ***/

    ret = sql_database();
    if (ret != 0)
    {
        sprintf(acErrMsg,"打开数据库错误!!");
        WRITEMSG
        goto ErrExit;
    }
    ret = sql_begin();
    if (ret != 0)
    {
        sprintf(acErrMsg,"开始事务错误!!");
        WRITEMSG
        goto ErrExit;
    }

    
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"数据恢复成功 reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"数据恢复失败 reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
