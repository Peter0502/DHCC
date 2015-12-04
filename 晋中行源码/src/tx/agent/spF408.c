/*************************************************
* 文 件 名: spF408.c
* 功能描述：判断是本行还是外行
*
* 作    者: mike
* 完成日期: 2004年03月28日
* 修改日期:
* 修 改 人:
* 修改内容:
*
*************************************************/
#define EXTERN
#include "public.h"

spF408()  
{ 	
    char flag[2];
	char sign[2];

    get_zd_data("0710",flag);

	get_zd_double("0420",&g_pub_tx.tx_amt1);
	sprintf(acErrMsg,"金额为[%lf]",g_pub_tx.tx_amt1);
	WRITEMSG

	get_zd_data("0680",sign);
	sprintf(acErrMsg, "付款人分类: [%s], 账号类型: [%s]", sign, flag);
	WRITEMSG

	if (sign[0] == '1')
	{
		/* 非税支出 财政账户取款 */
		sprintf(acErrMsg,"AAAAAAAAAAAAAAAAAAAAAAA");
		WRITEMSG
		set_zd_data("0191","1");
		set_zd_data("0192","00000000");
		set_zd_data("0193","1001");
	}

    if (flag[0] == '2')
    {
        sprintf(acErrMsg,"转本行客户的处理交易!!!");
        WRITEMSG
        goto OkExit1;
    }
	else if (flag[0] != '1')
	{
		/* 同城大额 */
		if (sign[0] == '0')
		{
			/* 一般缴款书 提入 */
			if (flag[0] == '3')
			{
				/* 同城 */
			}
			else
			{
				/* 大额 */
			}
			goto OkExit;
		}
		else
		{
			/* 非税支出  提出 */
			if (flag[0] == '3')
			{
				/* 同城 */
				goto OkExit2;
			}
			else
			{
				/* 大额 */
				goto OkExit3;
			}
		}
	}
	sprintf(acErrMsg, "交易流程错误，应走现金缴费流程!!");
	WRITEMSG
	strcpy(g_pub_tx.reply, "H041");

ErrExit:
    sprintf(acErrMsg,"ErrExit!!! [%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"转外行客户的处理(提入)OKExit !!! [%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;

OkExit1:
    strcpy(g_pub_tx.reply,"0001");
    sprintf(acErrMsg,"转本行客户的处理OKExit !!! [%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
OkExit2:
    strcpy(g_pub_tx.reply,"0002");
    sprintf(acErrMsg,"转(同城提出)交易OKExit !!! [%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
OkExit3:
    strcpy(g_pub_tx.reply,"0003");
    sprintf(acErrMsg,"转(大额往账)交易OKExit !!! [%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
}
