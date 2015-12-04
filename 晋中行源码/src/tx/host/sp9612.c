/*************************************************
* 文 件 名: sp9612.c
* 功能描述：数据清理
*
* 作    者: rob 
* 完成日期：20030319
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

int sp9612( )
{
    int ret;
    char filename[30];
    char tabname[30];

    char filter[500];

    struct com_sys_parm_c com_sys_parm_c;

    memset(&com_sys_parm_c, 0x0, sizeof(struct com_sys_parm_c));
    memset(filename, 0x0, sizeof(filename));
    memset(tabname, 0x0, sizeof(tabname));
    memset(filter, 0x0, sizeof(filter));

    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"初始化公用结构错误!!");
        WRITEMSG
        goto ErrExit;
    }
 
    get_zd_long("0440",&g_pub_tx.open_date); /* 清理日期 */
    get_zd_data("0300",g_pub_tx.ac_no);   /* 表名 */
    get_zd_data("0310",g_pub_tx.ac_no1);  /* 字段名 */

    strcpy(tabname,g_pub_tx.ac_no);
    
    ret = Com_sys_parm(g_pub_tx.reply,&com_sys_parm_c,"1=1");
    if (ret != 0)
    {
        sprintf(acErrMsg,"查询系统参数表错误!! [%d]",ret);
        WRITEMSG
        return ret;
    }

    /* 判断系统是否已经备份完成 */
    if (com_sys_parm_c.sys_sts == 1)
    {
        sprintf(acErrMsg,"系统没有进行备份,不能进行数据清理!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"O130");
        goto ErrExit;
    }
       
    /* 生成sql语句将有用数据导出到文本 */
    sprintf(filter,"where %s >= %ld ",g_pub_tx.ac_no1,g_pub_tx.open_date);

    Pubscname(filename);
    ret = tab_unload(filename,tabname,filter,NULL);
    if (ret != 0)
    {
        sprintf(acErrMsg,"将数据由数据库倒入文本文件错误!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"O131");
        goto ErrExit;
    }

    /* 删除该表 */
    ret = sql_execute("drop table %s",tabname);
    if (ret != 0)
    {
        sprintf(acErrMsg,"执行删除表操作错误!![%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    /* 新建该表 */
    ret = pub_base_creattable(tabname,g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"新建该表错误!![%s]",tabname);
        WRITEMSG
        goto ErrExit;
    }

    /* 将数据文件倒入数据库中 */
    ret = tab_load(filename,tabname,NULL,NULL);
    if (ret != 0)
    {
        sprintf(acErrMsg,"将数据文件倒入数据库中错误!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"");
        goto ErrExit;
    }

    strcpy(g_pub_tx.ac_no,"");
    strcpy(g_pub_tx.brf,"数据清理");

    if (pub_ins_trace_log())
    {
        sprintf(acErrMsg,"登记交易流水错误!!");
        WRITEMSG
        goto ErrExit;
    }
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"数据清理成功 reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"数据清理失败 reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
