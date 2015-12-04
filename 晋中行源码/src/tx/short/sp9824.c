/*************************************************
* 文 件 名:  sp9824.c
* 功能描述： 根据产品编号查询产品名称和到期日期
*
* 作    者:  rob
* 完成日期： 2003年1月21日 
*
* 修改记录： 
* 日   期:
* 修 改 人:
* 修改内容:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"

int sp9824()
{
    struct in_parm_c sIn_parm;
    int ret;
   
    memset(&sIn_parm, 0x0, sizeof(struct in_parm_c));
   
    get_zd_data("0230", sIn_parm.prdt_no);
    sprintf(acErrMsg," prdt_no = [%s] ",sIn_parm.prdt_no);
    WRITEMSG
    ret = In_parm_Sel(g_pub_tx.reply,&sIn_parm,"prdt_no = '%s'",
                      sIn_parm.prdt_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"查询内部产品表出错 sqlcode=[%d]",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"W035");
        goto ErrExit;
    }

    set_zd_data("0810",sIn_parm.prdt_mo);
    set_zd_long("0450",sIn_parm.stop_date);
OkExit:
    strcpy( g_pub_tx.reply, "0000" );
    sprintf(acErrMsg,"success !!: reply is[%s]",g_pub_tx.reply);
    WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"fail !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
