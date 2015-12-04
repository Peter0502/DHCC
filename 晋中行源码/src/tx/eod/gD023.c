/*************************************************************
* 文 件 名: gD023.c
* 功能描述：日终余额表处理初始化
**************************************************************/
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"
#include "com_sys_parm_c.h"


gD023()
{
	int ret = 0;
	struct com_sys_parm_c com_sys_parm_c;

    ret=sql_begin(); /*打开事务*/
    if(ret != 0)
    {
        sprintf( acErrMsg, "打开事务失败!!!" );
        WRITEMSG
        goto ErrExit;
    }

    /**------- 初始化全局变量 --------**/
    pub_base_sysinit();

	ret = Com_sys_parm_Sel(g_pub_tx.reply,&com_sys_parm_c,"1=1");
	if( ret ) goto ErrExit;

	ret=sql_count( "mo_jl_bal" , "1=1" );
	if( ret<0 ) goto ErrExit;
	if( ret )
	{
		sprintf(acErrMsg,"已经有数据了[%d]",ret);
		WRITEMSG
		goto OkExit;
	}

        sprintf(acErrMsg,"活期帐户主文件余额!!");
        WRITEMSG

  	sprintf(acErrMsg,
	"insert into mo_jl_bal select ac_id,ac_seqn,%d,99999999,bal from dd_mst",
		com_sys_parm_c.sys_date ); 
	ret=sql_execute( acErrMsg );
	if( ret ) goto ErrExit;

        sprintf(acErrMsg,"定期帐户主文件余额!!");
        WRITEMSG

  	sprintf(acErrMsg,
	"insert into mo_jl_bal select ac_id,ac_seqn,%d,99999999,bal from td_mst",
		com_sys_parm_c.sys_date ); 
	ret=sql_execute( acErrMsg );
	if( ret ) goto ErrExit;

        sprintf(acErrMsg,"贷款帐户主文件余额!!");
        WRITEMSG

  	sprintf(acErrMsg,
	"insert into mo_jl_bal select ac_id,ac_seqn,%d,99999999,bal from ln_mst",
		com_sys_parm_c.sys_date ); 
	ret=sql_execute( acErrMsg );
	if( ret ) goto ErrExit;

        sprintf(acErrMsg,"透支帐户主文件余额!!");
        WRITEMSG

  	sprintf(acErrMsg,
	"insert into mo_jl_bal select ac_id,ac_seqn,%d,99999999,bal from od_mst",
		com_sys_parm_c.sys_date ); 
	ret=sql_execute( acErrMsg );
	if( ret ) goto ErrExit;

        sprintf(acErrMsg,"内部帐户主文件余额!!");
        WRITEMSG

  	sprintf(acErrMsg,
	"insert into mo_jl_bal select ac_id,ac_seqn,%d,99999999,bal from in_mst",
		com_sys_parm_c.sys_date ); 
	ret=sql_execute( acErrMsg );
	if( ret ) goto ErrExit;

OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"余额表初始化处理成功!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
    if (strcmp(g_pub_tx.reply,"0000") == 0 || g_pub_tx.reply[0]=='\0')
        strcpy(g_pub_tx.reply,"G107");
	sql_rollback();	/*--事务回滚--*/
	sprintf(acErrMsg,"余额表初始化处理失败!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
