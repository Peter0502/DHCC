/*************************************************************
* 文 件 名: gD013.c
* 功能描述：卡限额日终处理
*
* 作    者: rob
* 完成日期: 2004.03.26
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"
#include "card_consum_lmt_c.h"

gD013()
{
	int ret = 0;
	long y,m,d,bz;
	struct	card_consum_lmt_c	card_consum_lmt_c;

	ret=sql_begin(); /*打开事务*/
	if( ret ) 
	{
		sprintf( acErrMsg, "打开事务失败!!!" );
		WRITEMSG
		goto ErrExit;
	}

    /**------- 初始化全局变量 --------**/
	pub_base_sysinit();


    memset(&card_consum_lmt_c,0x0,sizeof(struct card_consum_lmt_c));

	ret = Card_consum_lmt_Dec_Upd(g_pub_tx.reply,"1=1");
	if( ret )
	{
		sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR!! [%d]",ret);
		WRITEMSG
		return ret;
	}

	while(1)
	{
		ret = Card_consum_lmt_Fet_Upd(&card_consum_lmt_c,g_pub_tx.reply);
	 	if( ret==100 ) break;
	 	else if( ret )
		{
		 	sprintf(acErrMsg,"FETCH ERROR !! [%d]",ret);
		 	WRITEMSG
			return ret;
		}
		
		/*** 每日累计金额清0 ***/
		card_consum_lmt_c.da_all_cash_lmt = 0.00;
		card_consum_lmt_c.da_all_amt_lmt = 0.00;
		card_consum_lmt_c.da_all_con_lmt = 0.00;

		ret = pub_base_get_ymd(&y,&m,&d,g_pub_tx.tx_date,&bz);
		if( ret )
		{
		 	sprintf(acErrMsg,"调用判断月末函数出错!![%d]",ret);
		 	WRITEMSG
			return ret;
		}

		if( bz==4 )						/*月末*/
		{
			card_consum_lmt_c.mo_all_cash_lmt=0.00;
			card_consum_lmt_c.mo_all_amt_lmt=0.00;
			card_consum_lmt_c.mo_all_con_lmt=0.00;
		}

		ret = Card_consum_lmt_Upd_Upd(card_consum_lmt_c,g_pub_tx.reply);
	 	if( ret )
		{
			sprintf(acErrMsg,"UPDATE ERROR !! [%d]",ret);
			WRITEMSG
		 	return ret;
		}
	}

	Card_consum_lmt_Clo_Upd();

OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--事务回滚--*/
	sprintf(acErrMsg,"!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
