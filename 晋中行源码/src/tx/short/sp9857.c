/*************************************************
* 文 件 名:  sp9857.c
* 功能描述： 根据产品编号查询产品名称(贷款产品参数表)
*
* 作    者:  jane
* 完成日期： 2003年2月23日 
*
* 修改记录： 
* 日   期:
* 修 改 人:
* 修改内容:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"

int sp9857()
{
    struct ln_parm_c sLn_parm;
    int ret;
   
    memset(&sLn_parm, 0x00, sizeof(struct ln_parm_c));
   
    get_zd_data("0230", sLn_parm.prdt_no);

    ret = Ln_parm_Sel(g_pub_tx.reply,&sLn_parm,"prdt_no = '%s'",
                      sLn_parm.prdt_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"查询贷款产品参数表出错!ret=[%d]",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"B110");
        goto ErrExit;
    }

    set_zd_data("0810",sLn_parm.title);
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

