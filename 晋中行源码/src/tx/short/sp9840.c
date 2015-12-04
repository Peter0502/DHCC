/*************************************************
* 文 件 名:  sp9840.c
* 功能描述： 输入协议编号弹出证券回购协议内容
*
* 作    者:  rob
* 完成日期： 2003年1月29日 
*
* 修改记录： 
* 日   期:
* 修 改 人:
* 修改内容:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "mo_stok_rebuy_prot_c.h"

int sp9840()
{
    struct mo_stok_rebuy_prot_c sMo_stok_rebuy_prot;
    char flag;
    int ret;
   
    memset(&sMo_stok_rebuy_prot,0x0,sizeof(struct mo_stok_rebuy_prot_c));

    get_zd_char("0670",&flag); /* 操作类型 */
    get_zd_data("0860",sMo_stok_rebuy_prot.pact_no); /* 证券回购协议编号 */

    if (flag == '0')
    {
        /* 录入 */
        goto OkExit;
    }
   
    ret = Mo_stok_rebuy_prot_Sel(g_pub_tx.reply,&sMo_stok_rebuy_prot,
          "pact_no='%s'",sMo_stok_rebuy_prot.pact_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"查询证券回购协议登记薄错误!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    set_zd_char("0700",sMo_stok_rebuy_prot.tcht_type[0]); /* 证券类型 */
    set_zd_double("0390",sMo_stok_rebuy_prot.par_value);  /* 证券面值*/
    set_zd_double("0940",sMo_stok_rebuy_prot.pledge_rate);/* 抵押率*/
    set_zd_char("0680",sMo_stok_rebuy_prot.buy_type[0]);  /* 借出/借入标志 */
    set_zd_double("0400",sMo_stok_rebuy_prot.bal);  /* 借出/借入金额 */
    set_zd_long("0440",sMo_stok_rebuy_prot.matr_date); /* 协议到期日 */
    set_zd_long("0480",sMo_stok_rebuy_prot.term); /* 证券回购期限 */
    set_zd_double("0850",sMo_stok_rebuy_prot.rate); /* 收益率 */
    if (sMo_stok_rebuy_prot.opt_acno[0] != '\0')
    {
        set_zd_char("0690",'2');
        set_zd_data("0300",sMo_stok_rebuy_prot.opt_acno);  /* 对方账号 */
        set_zd_data("0270",sMo_stok_rebuy_prot.opt_name);  /* 对方户名 */
    }
    else 
    {
        set_zd_char("0690",'1');
    }
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
