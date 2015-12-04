/*************************************************
* 文 件 名: sp9101.c
* 功能描述：日终前备份
*
* 作    者: rob
* 完成日期：20030321
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
#include "com_sys_parm_c.h"

extern char *get_env_info(char *infoname);

int sp9101( )
{
    int ret;
    long bfrq;
    char dbname[20],log_file[60],datadev[60],logdev[60];
    char comm[300],path[60],dump_dir[60];
    char dbuser[20],dbpasswd[20],filename[50];

    struct com_sys_parm_c com_sys_parm_c;
    memset(&com_sys_parm_c, 0x0, sizeof(struct com_sys_parm_c));
    if ( pub_base_sysinit() )
    {
        sprintf(acErrMsg,"初始化公用结构错误!!");
        WRITEMSG
        goto ErrExit;
    }
     
    /* 检查是否还有网点没有签退 暂时不考虑atm pos签退 ***/
    ret = sql_count( "com_branch","wrk_sts='1' and br_type <> '0' and \
          br_type <> '6' and br_type <> '7'" );
    if (ret > 0)
    {
        sprintf(acErrMsg,"还有网点在营业!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"O140");
        goto ErrExit;
    }

    ret = Com_sys_parm_Sel(g_pub_tx.reply,&com_sys_parm_c,"1=1");
    if (ret != 0)
    {
        sprintf(acErrMsg,"查询系统参数表错误!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    bfrq = com_sys_parm_c.sys_date;

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

    /* 日终前备份 */
    strcpy(path,getenv("BFLDIR"));

    memset(dbuser,0,sizeof(dbuser));
    memset(dbpasswd,0,sizeof(dbpasswd));

    memcpy(dbuser,get_env_info("DB_USER"),sizeof(dbuser));
    memcpy(dbpasswd,get_env_info("DB_PASSWD"),sizeof(dbpasswd));

    sprintf(acErrMsg,"BFLDIR [%s] dbuser [%s]  dbpasswd [%s]",getenv("BFLDIR"),dbuser,dbpasswd);
    /*WRITEMSG*/

    /** exp备份 ***/
    memset(comm,0x00,sizeof(comm));
    sprintf(comm,"%s/",path);
    pub_base_strpack(comm);
    sprintf(acErrMsg,"comm[%s]",comm);
    WRITEMSG
    chdir(comm);

    memset(comm,0x00,sizeof(comm));
    sprintf(comm,"rm -fr %de",bfrq);
    sprintf(acErrMsg,"comm[%s]",comm);
    WRITEMSG
    ret=system(comm);
    sprintf(acErrMsg,"PPPP comm[%s][%d]",comm,ret);
    WRITEMSG

		memset(comm, 0x00, sizeof(comm));
		sprintf(comm, "mkdir %de;chmod 777 %de", bfrq,bfrq);
		sprintf(acErrMsg, "comm[%s]", comm);
		WRITEMSG
		ret = system(comm);
		sprintf(acErrMsg, "PPPP comm[%s][%d]", comm, ret);
		WRITEMSG

    memset(comm,0x00,sizeof(comm));
    sprintf(comm,"%s/%de/",path,bfrq);
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
    sprintf(filename,"expdat2_%d.dmp",bfrq);
   /* sprintf(comm,"dbexport  %s -ss -o ./ 1>/dev/null",dbname); */
		/*
    if(getenv("DB_NAME")!=NULL){
	    sprintf(comm,"exp %s/%s@%s file=%s/%de/%s 1>/dev/null 2>/dev/null",dbuser,dbpasswd,getenv("DB_NAME"),path,bfrq,filename);
    }else{
	    sprintf(comm,"exp %s/%s file=%s/%de/%s 1>/dev/null 2>/dev/null",dbuser,dbpasswd,path,bfrq,filename);
    }
    */
    if(getenv("DUMP_DIR")!=NULL)
    {
	    memset(dump_dir,0x00,sizeof(dump_dir));
	    strcpy(dump_dir,getenv("DUMP_DIR"));
	    ret=sql_execute("create or replace directory dump_dir as '%s'",dump_dir); /*生成备份目录*/  	
    }else{
	    ret=sql_execute("create or replace directory dump_dir as '%s/%sde'",path,bfrq); /*生成备份目录*/  	
    }
    if(ret)
    {
	vtcp_log("%s,%d,建立备份目录失败!",__FILE__,__LINE__);
	strcpy(g_pub_tx.reply,"H034");
	goto ErrExit;
    }
    if(getenv("DB_NAME")!=NULL){
	  sprintf(comm,"expdp %s/%s@%s directory=dump_dir dumpfile=%s parallel=4 logfile=exp2.log 1>/dev/null 2>/dev/null",dbuser,dbpasswd,getenv("DB_NAME"),filename);
    }else{
	  sprintf(comm,"expdp %s/%s directory=dump_dir dumpfile=%s parallel=4 logfile=exp2.log 1>/dev/null 2>/dev/null",dbuser,dbpasswd,filename);
    } 
    sprintf(acErrMsg,"comm[%s]",comm);
    WRITEMSG
    ret =  system(comm) ;
    if ( ret )
    {
        sprintf(acErrMsg,"日终前备份错误 [%d]",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"O250");
        goto ErrExit;
    }

    memset(comm,0,sizeof(comm));
    if(getenv("DUMP_DIR") != NULL)
    {
        sprintf(comm,"%s/bin/tel_ip.sh",getenv("HOME"));
    }else{
    	sprintf(comm,"compress -vf expdat2_%d.dmp",bfrq);
    }
    sprintf(acErrMsg,"comm[%s]",comm);
    WRITEMSG
    ret =  system(comm) ;
    if ( ret )
    {
        sprintf(acErrMsg,"压缩备份错误 [%d]",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"O250");
        goto ErrExit;
    }

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

    ret = sql_execute("update com_sys_parm set sys_sts='1' where 1=1");
    if (ret != 0)
    {
        sprintf(acErrMsg,"更新公共系统参数表错误!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }
    
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"日终前备份成功 reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"日终前备份失败 reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
