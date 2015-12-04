/*************************************************
* 文 件 名: spF401.c
* 功能描述：判断是现金还是转帐
*
* 作    者: mike
* 完成日期: 2004年02月28日
* 修改日期:
* 修 改 人:
* 修改内容:
*
*************************************************/
#define EXTERN
#include "public.h"

spF401()  
{ 	
 	char sign[2];

    if (strcmp(g_pub_tx.tx_code,"6401") == 0)
    {
        /* 对校验码进行检查 */
    }

    get_zd_data("0710",g_pub_tx.ct_ind);
    get_zd_double("0420",&g_pub_tx.tx_amt1);

    strcpy(g_pub_tx.cur_no,"01");
    set_zd_data("0210",g_pub_tx.cur_no);

    sprintf(acErrMsg,"现转标志为:[%s]",g_pub_tx.ct_ind);
	WRITEMSG


	get_zd_data("0680",sign);

	if (sign[0] == '1')
	{
		/* 非税支出 财政账户取款 */ 
		sprintf(acErrMsg,"AAAAAAAAAAAAAAAAAAAAAAA"); 
		WRITEMSG 
		set_zd_data("0191","1"); 
		set_zd_data("0192","00000000"); 
		set_zd_data("0193","1001"); 
	}
	else 
	{
		set_zd_data("0940","0");
	}


    if (g_pub_tx.ct_ind[0] == '1')
    {
        sprintf(acErrMsg,"转现金子交易!!!");
        WRITEMSG
		set_zd_data("0670", g_pub_tx.ct_ind);
        goto OkExit1;
    }
	else
	{
		strcpy(g_pub_tx.ct_ind, "2");
	}
	set_zd_data("0670", g_pub_tx.ct_ind);

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"转转帐处理OKExit !!! [%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;

OkExit1:
    strcpy(g_pub_tx.reply,"0001");
    sprintf(acErrMsg,"转现金处理OKExit !!! [%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;

ErrExit:
    sprintf(acErrMsg,"ErrExit!!! [%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
