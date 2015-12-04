/*************************************************
* 文 件 名:  sp9324.c
* 功能描述： ATM加清钞做现金调剂,在这里先查询余额,再去做现金调剂
	     长短款情况由柜员自行清点现金作清机加钞
*
* 作    者:  andy
* 完成日期： 2004年3月12日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "com_tel_connect_c.h"
#include "com_tel_c.h"
#include "cash_mst_c.h"
#include "com_tel_connect_c.h"

int 
sp9324()
{
	int             ret;
	char            teller[7];
	char            atm_tel[7];
	char            clearflag[2];
	char            tjfs[2];
	double atm_amt;
	struct com_tel_connect_c g_com_tel_connect;
	struct com_tel_c sCom_tel_c;
	struct com_tel_c st_com_tel;
	struct com_tel_c st_atm_tel;
	struct cash_mst_c st_atm_cash;
	struct com_tel_connect_c st_cash_connect;
	memset(&g_com_tel_connect, 0x00, sizeof(struct com_tel_connect_c));
	memset(&st_atm_cash, 0x00, sizeof(struct cash_mst_c));
	memset(&st_cash_connect, 0x00, sizeof(struct com_tel_connect_c));
	memset(&sCom_tel_c, 0x00, sizeof(sCom_tel_c));
	memset(&st_com_tel, 0x00, sizeof(st_com_tel));
	memset(&st_atm_tel, 0x00, sizeof(st_atm_tel));
	memset(clearflag, 0x00, sizeof(clearflag));
	memset(tjfs, 0x00, sizeof(tjfs));
	memset(teller, 0x00, sizeof(teller));
	memset(atm_tel, 0x00, sizeof(atm_tel));

	/* 系统初始化（初始化g_pub_tx)数据初始化 */
	pub_base_sysinit();

	get_zd_data("0670", tjfs);	/* 调剂方式 */
	get_zd_data("0680", clearflag);	/* 加清钞标志 0加钞1清钞 */
	get_zd_data("0920", teller);	/* 对方柜员 */
	get_zd_data("0070", atm_tel);	/* ATM柜员 */
	get_zd_data("0210", g_pub_tx.cur_no);	/* 币种 */
	get_zd_double("0410", &g_pub_tx.tx_amt1);	/* 发生额 */
		vtcp_log("清钞标志[%s]",clearflag);
		vtcp_log("调剂方式[%s]",tjfs);
	if(!memcmp(clearflag,"1",1)){/*若果是ATM清钞需要查出库存金额赋予41域*/
		/*****************
		验证加钞为同一机构柜员,且级别为4
		******************/
		vtcp_log("清钞");
		ret = Com_tel_Sel(g_pub_tx.reply, &st_atm_tel, "tel='%s' ",atm_tel);
		if(ret)
		{
			sprintf(acErrMsg, "查询ATM柜员错误!!");
			WRITEMSG
			strcpy(g_pub_tx.reply, "C118");
			goto ErrExit;
		}
		ret = Com_tel_Sel(g_pub_tx.reply, &st_com_tel, "tel='%s' and lvl='4' and br_no='%s' and csts='0' ",teller,st_atm_tel.br_no);
		if(ret)
		{
			sprintf(acErrMsg, "查询加钞柜员错误!!");
			WRITEMSG
			strcpy(g_pub_tx.reply, "C118");
			goto ErrExit;
		}
		ret = Cash_mst_Sel(g_pub_tx.reply, &st_atm_cash, "tel='%s'",atm_tel);
		if(ret)
		{
			sprintf(acErrMsg, "查询ATM柜员钱箱错误!!");
			WRITEMSG
			strcpy(g_pub_tx.reply, "C118");
			goto ErrExit;
		}
		set_zd_double("0410", st_atm_cash.bal);
	}else if(!memcmp(clearflag,"0",1)){
		/*若是ATM加钞取库管员付出金额与ATM金额比较*/
		vtcp_log("加钞");
		ret = Cash_mst_Sel(g_pub_tx.reply, &st_atm_cash, "tel='%s'",atm_tel);
		if(ret)
		{
			sprintf(acErrMsg, "钱箱查询错误!!");
			WRITEMSG
			strcpy(g_pub_tx.reply, "C118");
			goto ErrExit;
		}
		vtcp_log("加钞ATM钱箱[%f]",st_atm_cash.bal);
		if(st_atm_cash.bal>0.001){
			sprintf(acErrMsg, "钱箱不为空不能加钞!!");
			WRITEMSG
			strcpy(g_pub_tx.reply, "CU12");
			goto ErrExit;
		}
		ret = Com_tel_connect_Sel(g_pub_tx.reply, &st_cash_connect, "tel='%s' and tw_tel='%s' and ind='A'",teller,atm_tel);
		get_zd_double("0390", &atm_amt);	/* 发生额 */
		vtcp_log("%s %d cashamt[%f],atmamt[%f]",__FILE__,__LINE__,st_cash_connect.amt,atm_amt);
		if (pub_base_CompDblVal(st_cash_connect.amt,atm_amt)!=0){
			sprintf(acErrMsg, "加钞金额与库管员付出不一致!!");
			WRITEMSG
			strcpy(g_pub_tx.reply, "CU13");
			goto ErrExit;
		}
	}
	if(!memcmp(tjfs,"1",1)){/*付出*/
		strcpy(g_pub_tx.reply, "0001");
	}else if(!memcmp(tjfs,"2",1)){/*收入*/
		strcpy(g_pub_tx.reply, "0002");
	}else{
		sprintf(acErrMsg, "不允许此交易!!");
		WRITEMSG
		strcpy(g_pub_tx.reply, "CU12");
		goto ErrExit;
	}

OkExit:
	sprintf(acErrMsg, "Before OK return: reply is[%s]", g_pub_tx.reply);
	WRITEMSG
		set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg, "Before return: reply is[%s]", g_pub_tx.reply);
	WRITEMSG
		set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 1;
}
