/*************************************************
* 文 件 名:  spF017.c
* 功能描述： POS 消费撤销
*            (帐号不是本行的商户)
* 作    者:  LiuHuafeng
* 完成日期： 2007-2-9 22:08
* NOTE：原来程序是sp031.c 但走帐部分不知道为何去掉了，并且走的分户和科目不一致。
*       为了不与原有程序冲突
*       特此新增交易替换6302中的sp022.c,并准备内部账相关数据
*
* 修改记录： 
*   日   期: 
*   修 改 人:
*   修改内容:
* tx_def
*1	6302	0	F027	#=0000@1@#$
*2	6302	1	D099	#=0000@2@#$
*3	6302	2	F017	#=0000@3@#$
*4	6302	3	A016	#$
*   注意新增com_rollback_ctl
* ---- 注意本交易截至2007-2-10 没有配置手续费表，如果配置可以参考6203 ---
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "chnl_self_trade_c.h"
#include "chnl_marchinfo_c.h"
#include "card.h"
int spF017()
{
	int ret;
	char flag;
	struct chnl_marchinfo_c chnl_marchinfo;
	struct chnl_self_trade_c chnl_self_trade;
	char cAcno_qs[25];
	long oldTx_date;

	memset(cAcno_qs, 0 , sizeof(cAcno_qs));
	memset(&chnl_marchinfo,0x0,sizeof(struct chnl_marchinfo_c));
	memset(&chnl_self_trade,0x0,sizeof(struct chnl_self_trade_c));

	if (pub_base_sysinit())
	{
		sprintf(acErrMsg,"初始化公用结构错误!!");
		WRITEMSG
			goto ErrExit;
	}
	get_zd_data("0370",g_pub_tx.crd_no);      /*  卡号 */
	get_zd_double("0400",&g_pub_tx.tx_amt1);  /* 消费金额 */
/*	get_zd_long("0610",&g_pub_tx.old_trace_no);  原终端流水号 */

	strcpy(g_pub_tx.cur_no,"01");
	set_zd_data("0210",g_pub_tx.cur_no);
	strcpy(g_pub_tx.ct_ind,"2"); /* 转帐 */

	get_zd_long("0440", &g_pub_tx.old_trace_no);  /* 被冲交易的原平台流水 */
	get_zd_long("0450", &oldTx_date);  /* 被冲交易的原交易日期 */

	/* 更新原交易流水的状态 */
/*	ret = Chnl_self_trade_Dec_Upd(g_pub_tx.reply,"trace_no=%ld",
		g_pub_tx.old_trace_no);
changed by chenming 2007.07.28. */
	ret = Chnl_self_trade_Dec_Upd(g_pub_tx.reply,"stra=%ld and dz_date=%ld ",
		g_pub_tx.old_trace_no, oldTx_date);
	if (ret != 0)
	{
		sprintf(acErrMsg,"查询原POS消费的自助设备交易日记错误!![%d]",ret);
		WRITEMSG
			goto ErrExit;
	}
	ret = Chnl_self_trade_Fet_Upd(&chnl_self_trade,g_pub_tx.reply);
	if (ret != 0)
	{
		sprintf(acErrMsg,"Fetch chnl_self_trade error [%d]",ret);
		WRITEMSG
			Chnl_self_trade_Clo_Upd();
		goto ErrExit;
	}
	strcpy(chnl_self_trade.stat,"1");
	chnl_self_trade.strb = g_pub_tx.trace_no;

	ret = Chnl_self_trade_Upd_Upd(chnl_self_trade,g_pub_tx.reply);
	if (ret != 0)
	{
		sprintf(acErrMsg,"Update chnl_self_trade error [%d]",ret);
		WRITEMSG
			Chnl_self_trade_Clo_Upd();
		goto ErrExit;
	}
	Chnl_self_trade_Clo_Upd();

	/* 给自助设备交易日记结构赋值 */
	chnl_self_trade.tx_date = g_pub_tx.tx_date;
	chnl_self_trade.tx_time = g_pub_tx.tx_time;
	chnl_self_trade.trace_no = g_pub_tx.trace_no;
	strcpy(chnl_self_trade.cur_no,g_pub_tx.cur_no);

	strcpy(chnl_self_trade.ac_no,g_pub_tx.crd_no);

	strcpy(chnl_self_trade.tx_code,g_pub_tx.tx_code);
	strcpy(chnl_self_trade.tx_br_no,g_pub_tx.tx_br_no);
	strcpy(chnl_self_trade.opn_br_no,g_pub_tx.opn_br_no);
	strcpy(chnl_self_trade.tel,g_pub_tx.tel);
	strcpy(chnl_self_trade.ttyp,"2");
	strcpy(chnl_self_trade.ttyn,g_pub_tx.tty);
	strcpy(chnl_self_trade.spcd,chnl_marchinfo.mrchno);
	/* 本交易对帐中心流水号 */
	get_zd_long("0520", &chnl_self_trade.stra);

	chnl_self_trade.tx_amt = g_pub_tx.tx_amt1;
	chnl_self_trade.sl_sxamt = 0.00;
	chnl_self_trade.fk_sxamt = 0.00;

	chnl_self_trade.strb = g_pub_tx.old_trace_no;

	strcpy(chnl_self_trade.dz_flag,"0");
	strcpy(chnl_self_trade.ct_ind,"2");
	strcpy(chnl_self_trade.info_source,"1");
	strcpy(chnl_self_trade.deal_flag,"1");
	strcpy(chnl_self_trade.brf_no,"PSCX");
	strcpy(chnl_self_trade.add_ind,"1");
	strcpy(chnl_self_trade.batch_flag,"0");
	strcpy(chnl_self_trade.stat,"0");

	/* 登记自助设备交易日记 */
	ret = pub_base_Insertselftrade(chnl_self_trade);
	if (ret != 0)
	{
		sprintf(acErrMsg,"登记自助设备交易日记错误!!");
		WRITEMSG
			goto ErrExit;
	}
	/* 设置内部账相关域 */
	ret = pub_base_GetParm_Str(g_pub_tx.tx_br_no,PARM_CARD_QS,cAcno_qs);
	if (ret)
	{
		memset(cAcno_qs, 0 , sizeof(cAcno_qs));
		strncpy(cAcno_qs,TESTCARD,sizeof(cAcno_qs)-1);
		vtcp_log("%s,%d 没有找到待清算的存放太原（上海清算）款项，所以启用默认帐户[%s]",__FILE__,__LINE__,cAcno_qs);
	}
	set_zd_data("1201",cAcno_qs);
	set_zd_data("1204",g_pub_tx.cur_no);
	set_zd_data("1204","2");/* 转帐 */
	set_zd_double("1208",g_pub_tx.tx_amt1);
	set_zd_data("1209",g_pub_tx.brf);

	/*  登记交易流水 */
	strcpy(g_pub_tx.brf,"POS撤销!!");

	strcpy(g_pub_tx.ac_no, TESTCARD);
	g_pub_tx.ac_id= TESTCARDID;
	strcpy(g_pub_tx.ct_ind, "2");

	if (pub_ins_trace_log())
	{
		sprintf(acErrMsg,"登记交易流水错误!!");
		WRITEMSG
			goto ErrExit;
	}
OkExit:
		strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"success!!");
	WRITEMSG
		set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;

ErrExit:
		sprintf(acErrMsg,"failed !! reply is[%s]",g_pub_tx.reply);
	WRITEMSG
		set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

