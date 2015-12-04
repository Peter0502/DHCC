/*************************************************
* 文 件 名: sp9106.c
* 功能描述：日终后处理备份
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
#include <errno.h>
#define EXTERN
#include "public.h"
#include "com_sys_parm_c.h"

extern char *get_env_info(char *infoname);

int sp9106( )
{
    int ret;
    char bffs[2];
    char cdqk[2];
    long bfrq,bfrq_rm,bfrq_tmp;
    char dbname[20],datadev[50],logdev[50];
    char comm[1024],path[100],dump_dir[100];
		char dbuser[10],dbpasswd[10],filename[80];	
    struct com_sys_parm_c com_sys_parm_c;
    memset(&com_sys_parm_c, 0x0, sizeof(struct com_sys_parm_c));

    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"初始化公用结构错误!!");
        WRITEMSG
        goto ErrExit;
    }

    memset(path,0x0,sizeof(path));
    memset(dbname,0x0,sizeof(dbname));
    memset(dbuser,0x0,sizeof(dbuser));
    memset(dbpasswd,0x0,sizeof(dbpasswd));

    strcpy(path,getenv("BFLDIR"));
    strcpy(dbname,getenv("DBNAME"));

    memcpy(dbuser,get_env_info("DB_USER"),sizeof(dbuser));
    memcpy(dbpasswd,get_env_info("DB_PASSWD"),sizeof(dbpasswd));

    ret = Com_sys_parm_Sel(g_pub_tx.reply,&com_sys_parm_c,"1=1");
    if (ret != 0)
    {
        sprintf(acErrMsg,"查询公共参数表错误!![%d]",ret);
        WRITEMSG
        goto ErrExit;
    }
   
    bfrq = com_sys_parm_c.sys_date;

    ret = pub_base_deadlineD(bfrq,-1,&bfrq_tmp);
    if (ret != 0)
    {
        sprintf(acErrMsg,"取一天前日期错误!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"O136");
        goto ErrExit;
    }
    bfrq = bfrq_tmp;

		/**ORACLE用exp备份JYW***/
/*
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
*/    
    memset(comm,0,sizeof(comm));
    memset(filename,0,sizeof(filename));
    sprintf(filename,"expdat3_%d.dmp",bfrq);
    /*
    if(getenv("DB_NAME")!=NULL){
         sprintf(comm,"exp %s/%s@%s file=%s/%de/%s 1>/dev/null 2>/dev/null",dbuser,dbpasswd,getenv("DB_NAME"),path,bfrq,filename);
    }else{
	 		sprintf(comm,"exp %s/%s file=%s/%de/%s 1>/dev/null 2>/dev/null",dbuser,dbpasswd,path,bfrq,filename);
    }
    */
    if(getenv("DUMP_DIR") != NULL)
    {
			memset(dump_dir,0x00,sizeof(dump_dir));
			strcpy(dump_dir,getenv("DUMP_DIR"));
    	ret=sql_execute("create or replace directory dump_dir as '%s'",dump_dir); /*生成备份目录*/	 	
    }else{
    	ret=sql_execute("create or replace directory dump_dir as '%s/%de'",path,bfrq); /*生成备份目录*/	 	
    }
    if(getenv("DB_NAME")!=NULL){
    	sprintf(comm,"expdp %s/%s@%s directory=dump_dir dumpfile=%s parallel=4 logfile=exp3.log 1>/dev/null 2>/dev/null",dbuser,dbpasswd,getenv("DB_NAME"),filename);
    }else{
    	sprintf(comm,"expdp %s/%s directory=dump_dir dumpfile=%s parallel=4 logfile=exp3.log 1>/dev/null 2>/dev/null",dbuser,dbpasswd,filename);
    }    
    sprintf(acErrMsg,"comm[%s]",comm);
    WRITEMSG
    ret =  system(comm) ;
    if ( ret )
    {
        sprintf(acErrMsg,"日终后备份错误 [%d]",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"O250");
        goto ErrExit;
    }
            
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
    if(getenv("DUMP_DIR") != NULL)
    {
        sprintf(comm,"%s/bin/tel_ip.sh",getenv("HOME"));
    }else{
    	sprintf(comm,"compress -vf expdat3_%d.dmp",bfrq);
    }
    sprintf(acErrMsg,"comm[%s]",comm);
    WRITEMSG
    
    ret =  system(comm) ;
    if ( ret )
    {
        sprintf(acErrMsg,"压缩日终后备份错误 [%d]",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"O250");
        goto ErrExit;
    }
  
    strcpy(g_pub_tx.ac_no,"");
    strcpy(g_pub_tx.brf,"日终后处理备份");

/*JYW把修改系统状态放在这里,等于全部日终结束后修改为可以营业状态*/
    ret = Com_sys_parm_Dec_Upd(g_pub_tx.reply,"1=1");
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

	TRACE
    ret = Com_sys_parm_Fet_Upd(&com_sys_parm_c,g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"FETCH ERROR [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

	TRACE
    com_sys_parm_c.sys_sts = 0; /*修改系统状态*/
    ret = Com_sys_parm_Upd_Upd(com_sys_parm_c,g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"UPDATE ERROR [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }
    Com_sys_parm_Clo_Upd();
/****JYW更改系统状态结束****/

OkExit:
	sql_commit();
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"日终后处理备份成功 reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"日终后处理备份失败 reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
