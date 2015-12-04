/***************************************************************
* 文 件 名:     spJC02.c
* 功能描述：	判断存取属性
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

spJC02()
{
	char flag[2];

	memset(flag,0,sizeof(flag));
	get_zd_data("0660",flag);


	if( flag[0]=='1' )		/*** 借 ***/
		goto ErrExit1;
	else if( flag[0]=='2') 		/*** 贷 ***/
		goto ErrExit2;
        else 
                goto ErrExit;

ErrExit:
	strcpy(g_pub_tx.reply,"P156");
	sprintf(acErrMsg,"判断借贷标志失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
ErrExit1:
	strcpy(g_pub_tx.reply,"0001");
	sprintf(acErrMsg,"判断借贷标志成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit2:
	strcpy(g_pub_tx.reply,"0002");
	sprintf(acErrMsg,"判断借贷标志成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
}
