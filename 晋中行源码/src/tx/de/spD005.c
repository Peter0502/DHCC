/***************************************************************
* 文 件 名:     spD005.c
* 功能描述：	判断现金转账
* 作    者:     jack
* 完成日期：    2004年1月10日
*
* 修改记录：
*    日    期:
*    修 改 人:
*    修改内容:
*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"

spD005()
{
	get_zd_data("1171",g_pub_tx.ct_ind);
	if( g_pub_tx.ct_ind[0]=='1' )			/*** 现金 ***/
		goto ErrExit1;
	else if( g_pub_tx.ct_ind[0]=='2') 		/*** 转账 ***/
		goto ErrExit2;
    else if (g_pub_tx.ct_ind[0] == '3')		/*** 同城 ***/
        goto ErrExit3;
    else if (g_pub_tx.ct_ind[0] == '4')		/*** 大额 ***/
        goto ErrExit4;
    else if (g_pub_tx.ct_ind[0] == '5')		/*** 单边 ***/
        goto ErrExit5;

ErrExit:
	strcpy(g_pub_tx.reply,"P156");
	sprintf(acErrMsg,"判断现转标志失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
ErrExit1:
	strcpy(g_pub_tx.reply,"0001");
	sprintf(acErrMsg,"判断现转标志成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit2:
	strcpy(g_pub_tx.reply,"0002");
	sprintf(acErrMsg,"判断现转标志成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit3:
    strcpy(g_pub_tx.reply,"0003");
    sprintf(acErrMsg,"判断现转标志成功![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
ErrExit4:
    strcpy(g_pub_tx.reply,"0004");
    sprintf(acErrMsg,"判断现转标志成功![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
ErrExit5:
    strcpy(g_pub_tx.reply,"0005");
    sprintf(acErrMsg,"判断现转标志成功![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;

}
