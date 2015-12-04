/*************************************************
* 文 件 名:  sp9833.c
* 功能描述： 根据投资协议编号查询权益性投资登记簿
*
* 作    者:  rob
* 完成日期： 2003年1月17日 
*
* 修改记录:
* 日   期 :
* 修 改 人:
* 修改内容:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "mo_righ_invest_c.h"
int sp9833()
{
    struct mo_righ_invest_c sMo_righ_invest;
    char flag;
    int ret;
   
    memset(&sMo_righ_invest, 0x0, sizeof(struct mo_righ_invest_c));
    get_zd_char("0670",&flag);
    get_zd_data("0580",sMo_righ_invest.pact_no);
    if (flag == '0')
    {
        goto OkExit;
    }
    
    ret = Mo_righ_invest_Sel(g_pub_tx.reply,&sMo_righ_invest,
          "pact_no='%s'",sMo_righ_invest.pact_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"查询权益性投资登记簿错误 [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }
    set_zd_data("0680",sMo_righ_invest.invst_type);
    set_zd_data("0810",sMo_righ_invest.name);
    set_zd_double("0390",sMo_righ_invest.amt);
    set_zd_double("0530",sMo_righ_invest.num);
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
