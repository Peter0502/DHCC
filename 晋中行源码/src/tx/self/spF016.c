/*************************************************
* 文 件 名:  spF016.c
* 功能描述： POS 消费(新的POS消费准备相关帐务参数)
*
* 作    者:  LiuHuafeng
* 完成日期： 2007-2-9 22:08
* NOTE：原来程序是sp022.c 但走帐部分不知道为何去掉了，为了不与原有程序冲突
*       特此新增交易替换6301中的sp022.c,并准备内部账相关数据
*
* 修改记录： 
*   日   期: 
*   修 改 人:
*   修改内容:
* tx_def
*	6301	0	F021	#=0000@1@#$
*	6301	1	D003	#=0000@2@#$
*	6301	2	F016	#=0000@3@#$
*	6301	3	A017	#$
* ---- 注意本交易截至2007-2-10 没有配置手续费表，如果配置可以参考6203 ---
*  注意新增com_rollback_ctl
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "chnl_self_trade_c.h"
#include "card.h"

int spF016()
{
	int ret=0;
	char crd_no[25];
	char cAcno_qs[25];
	struct chnl_self_trade_c chnl_self_trade;

	memset(&chnl_self_trade, 0x0, sizeof(struct chnl_self_trade_c));
	memset(crd_no, 0x0, sizeof(crd_no));
	memset(cAcno_qs, 0 , sizeof(cAcno_qs));

	if (pub_base_sysinit())
	{
		sprintf(acErrMsg,"初始化公用结构错误!!");
		WRITEMSG
		goto ErrExit;
	}
	get_zd_data("0370",g_pub_tx.crd_no); /* 卡号 */ 
	get_zd_double("0400",&g_pub_tx.tx_amt1); /* 交易金额 */ 

	strcpy(g_pub_tx.cur_no,"01"); 
	set_zd_data("0210",g_pub_tx.cur_no); 
	strcpy(g_pub_tx.ct_ind,"2"); 

	strcpy(g_pub_tx.opn_br_no, g_pub_tx.tx_br_no); 

	/* 给自助设备交易日记结构赋值 */ 
	chnl_self_trade.tx_date = g_pub_tx.tx_date; 
	chnl_self_trade.tx_time = g_pub_tx.tx_time; 
	chnl_self_trade.trace_no = g_pub_tx.trace_no; 
	strcpy(chnl_self_trade.cur_no,g_pub_tx.cur_no); 

	get_zd_data("0300", crd_no);
	/* rem by LiuHuafeng 20070127
	if (strncmp(crd_no, "694302", 6) == 0)
	******replace by next line**********/
	if ((strncmp(crd_no, CARDHEADCODE , 6) == 0 || strncmp(crd_no, ICCARDHEADCODE , 6) == 0) && memcmp(crd_no+6,CARD_UNION_AREA,2) ==0 )
	{
		get_zd_data("0300", chnl_self_trade.ac_no);
	}
	else
	{
		strcpy(chnl_self_trade.ac_no,g_pub_tx.crd_no); 
	}
	strcpy(chnl_self_trade.tx_code,g_pub_tx.tx_code); 
	strcpy(chnl_self_trade.tx_br_no,g_pub_tx.tx_br_no); 
	strcpy(chnl_self_trade.opn_br_no,g_pub_tx.opn_br_no); 
	strcpy(chnl_self_trade.tel,g_pub_tx.tel); 
	strcpy(chnl_self_trade.ttyp,"2");
	strcpy(chnl_self_trade.sctp,"PS");
	strcpy(chnl_self_trade.ttyn,g_pub_tx.tty);

	/* 本交易对帐中心流水号 */
	get_zd_long("0520", &chnl_self_trade.stra);
	/* 前台交易日期Added by ChenMing 2007.07.28.*/
	get_zd_long("0450", &chnl_self_trade.dz_date); 

	get_zd_data("0380", chnl_self_trade.ttyn);
	get_zd_data("0620", chnl_self_trade.spcd);

	chnl_self_trade.tx_amt = g_pub_tx.tx_amt1; 
	chnl_self_trade.sl_sxamt = 0.00; 
	chnl_self_trade.fk_sxamt = 0.00; 

	strcpy(chnl_self_trade.dz_flag,"0"); 
	strcpy(chnl_self_trade.ct_ind,g_pub_tx.ct_ind); 
	strcpy(chnl_self_trade.info_source,"1"); 
	strcpy(chnl_self_trade.deal_flag,"1"); 
	strcpy(chnl_self_trade.brf_no,"PSXF"); 
	strcpy(chnl_self_trade.add_ind,"0"); 
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
	/*  登记交易流水 */ 

	strcpy(g_pub_tx.brf,"POS消费!!"); 

	/* 对卡登记簿进行处理 */
	{
		char cTradetype[4];/* 设别类型 */
		int iSelflag=0;  /*清算组织标志 */
		int atm_qy_free_flag=0;/* atm查询免费标志 0免费 1收费 */
		int pos_qy_free_flag=0;/* pos 查询免费标志 0免费 1收费 */
		int pos_tx_free_flag=0;/* pos 交易免费标志 0免费 1收费 */
		int atm_tx_free_flag=0;/* atm 交易免费标志 0免费 1收费 */
		int iDscpt=0;/* 现转查询标志 */

		iSelflag=0;  /*清算组织标志 */                        
		atm_qy_free_flag=0;/* atm查询免费标志 0免费 1收费 */  
		pos_qy_free_flag=0;/* pos 查询免费标志 0免费 1收费 */ 
		pos_tx_free_flag=0;/* pos 交易免费标志 0免费 1收费 */ 
		atm_tx_free_flag=0;/* atm 交易免费标志 0免费 1收费 */ 
		iDscpt=0;/* 现转查询标志 */                           

		memset(cTradetype, 0 , sizeof(cTradetype));
		iGetTradeType(cTradetype,&iSelflag,&iDscpt);
		g_reply_int=pub_card_reg(g_pub_tx.tx_amt1,cTradetype,'0',&atm_qy_free_flag,
			&pos_qy_free_flag,&pos_tx_free_flag,&atm_tx_free_flag,iSelflag,iDscpt);
		if(g_reply_int)
		{
			sprintf(acErrMsg,"登记卡登记簿错误");
			strncpy(g_pub_tx.reply,"AT06",4);
			WRITEMSG
			vtcp_log("%s,%d 处理卡交易登记簿错误 ret=[%d]",__FILE__,__LINE__,g_reply_int);
			goto ErrExit;
		}
		/* 设置ATM手续费免费标志 */
		if(iSelflag==0)
		{
			/* 本清算组织内的都不收取费用 */
			vtcp_log("%s,%d 客户银行两得利，都不收费",__FILE__,__LINE__);
			set_zd_data("0680","2");
		}
		else
		{
			if(pos_tx_free_flag==0)
			{
				/* 免费情况 */
				set_zd_data("0680","0");
				vtcp_log("%s,%d 恭喜，免费了",__FILE__,__LINE__);
			}
			else
			{
				set_zd_data("0680","1");
				vtcp_log("%s,%d 收费没有商量",__FILE__,__LINE__);
			}
		}
	}
	/*** note 由于现在不清楚是否收费所以暂时设定不收费,如果收费那么还有设置相关收费参数 **/
	vtcp_log("%s %d 注意这里设置都不收费",__FILE__,__LINE__);
	set_zd_data("0680","2");
	vtcp_log("%s %d 注意这里设置都不收费",__FILE__,__LINE__);
	/* 以下设定收费科目
	* 准备总行手续费支出科目 1.6元 52701  *
	memset(cAcno_qs, 0 , sizeof(cAcno_qs));
	ret = pub_base_GetParm_Str(g_pub_tx.tx_br_no,PARM_CARD_OPEN_FEE,cAcno_qs);
	if (ret)
	{
		memset(cAcno_qs, 0 , sizeof(cAcno_qs));
		strncpy(cAcno_qs,"52701",sizeof(cAcno_qs)-1);
		vtcp_log("%s,%d 没有找到手续费支出科目，所以启用默认帐户=%s",__FILE__,__LINE__,cAcno_qs);
	}
	set_zd_data("0640",cAcno_qs);
	* 准备总行垫付手续费科目 2元  139帐号 *
	memset(cAcno_qs, 0 , sizeof(cAcno_qs));
	ret = pub_base_GetParm_Str(g_pub_tx.tx_br_no,PARM_CARD_USER_FEE,cAcno_qs);
	if (ret)
	{
		memset(cAcno_qs, 0 , sizeof(cAcno_qs));
		strncpy(cAcno_qs,"13902",sizeof(cAcno_qs)-1);
		vtcp_log("%s,%d 没有找到垫款帐户，所以启用默认帐户[%s]",__FILE__,__LINE__,cAcno_qs);
	}
	set_zd_data("0330",cAcno_qs);
	******************************************************/
	/** note 由于现在不清楚是否收费所以暂时设定不收费 **/
	/* 设置内部账相关域 */
	ret = pub_base_GetParm_Str(g_pub_tx.tx_br_no,PARM_CARD_QS,cAcno_qs);
	if (ret)
	{
		memset(cAcno_qs, 0 , sizeof(cAcno_qs));
		strncpy(cAcno_qs,TESTCARD,sizeof(cAcno_qs)-1);
		vtcp_log("%s,%d 没有找到待清算的存放太原（上海清算）款项，所以启用默认帐户[%s]",__FILE__,__LINE__,cAcno_qs);
	}
	set_zd_data("1211",cAcno_qs);
	set_zd_data("1214",g_pub_tx.cur_no);
	set_zd_data("1214","2");/* 转帐 */
	set_zd_double("1218",g_pub_tx.tx_amt1);
	set_zd_data("1219",g_pub_tx.brf);

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

