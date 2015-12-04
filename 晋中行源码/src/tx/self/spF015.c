/*************************************************
* 文 件 名:  spF015.c
* 功能描述： ATM 取款(本行卡在外行atm上取款配置相关内部账参数程序)
*
* 作    者:  LiuHuafeng
* 完成日期： 2007-2-5 23:57
* NOTE：原来程序是sp006.c 但走帐部分不知道为何去掉了，为了不与原有程序冲突
*       特此新增交易替换6203种的sp006.c,并准备内部账相关数据
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*	6203	0	F003	#=0000@1@#=0001@3@#=0002@4@#$
*	6203	1	F004	#=0000@2@#$
*	6203	2	D003	#=0000@5@#$
*	6203	3	F004	#=0000@6@#$
*	6203	4	F005	#=0000@5@#$
*	6203	5	M002	#$
*	6203	6	D003	#=0000@7@#$
*	6203	7	F015	#=0000@8@#$    new
*	6203	8	A017	#$             new     
* --------手续费---------------------------------
*	6203	2	AT2	0330	B	0	@0680==0	0400	手续费2
*	6203	1	AT1	0640	B	0	@0680!=2	0400	手续费1
*	6203	0	ATM	0370	B	0	@0680==1	0400	手续费
*	01	ATM	11111111	99999999	KGBF	ATM取款收费	0	0.00000	2.00	2.00
*	01	AT1	11111111	99999999	KGBF	ATM开卡行费	0	0.00000	1.50	1.50
*	01	AT2	11111111	99999999	KGBF	ATM取款垫费	0	0.00000	2.00	2.00
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "chnl_tradeadd_c.h"
#include "chnl_self_trade_c.h"
#include "card.h"
int spF015()
{
	int ret;
	struct chnl_tradeadd_c chnl_tradeadd;
	struct chnl_self_trade_c chnl_self_trade;
	char cAcno_qs[25];

	memset(cAcno_qs, 0 , sizeof(cAcno_qs));
	memset(&chnl_tradeadd,0x0,sizeof(struct chnl_tradeadd_c));
	memset(&chnl_self_trade,0x0,sizeof(struct chnl_self_trade_c));

	if (pub_base_sysinit())
	{
		sprintf(acErrMsg,"初始化公用结构错误!!");
		WRITEMSG
		goto ErrExit;
	}
	get_zd_data("0370",g_pub_tx.crd_no); /* 卡号 */
	get_zd_data("0370",g_pub_tx.ac_no); /* 卡号 */
	get_zd_data("0790",g_pub_tx.draw_pwd); /* 支取密码 */
	get_zd_double("0400",&g_pub_tx.tx_amt1); /* 取款金额 */

	sprintf(acErrMsg,"crd_no=[%s]",g_pub_tx.crd_no);
	WRITEMSG

	strcpy(g_pub_tx.cur_no,"01");
	set_zd_data("0210",g_pub_tx.cur_no);

	/*  登记交易流水 */ 
	strcpy(g_pub_tx.brf,"ATM取款"); 
	strcpy(g_pub_tx.ac_no, TESTCARD);
	g_pub_tx.ac_id= TESTCARDID;

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
			if(atm_tx_free_flag==0)
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
	/* 准备总行手续费支出科目 1.6元 52701*/
	memset(cAcno_qs, 0 , sizeof(cAcno_qs));
	ret = pub_base_GetParm_Str(g_pub_tx.tx_br_no,PARM_CARD_OPEN_FEE,cAcno_qs);
	if (ret)
	{
		memset(cAcno_qs, 0 , sizeof(cAcno_qs));
		strncpy(cAcno_qs,"52701",sizeof(cAcno_qs)-1);
		vtcp_log("%s,%d 没有找到手续费支出科目，所以启用默认帐户=%s",__FILE__,__LINE__,cAcno_qs);
	}
	set_zd_data("0640",cAcno_qs);
	/* 准备总行垫付手续费科目 2元  139帐号 */
	memset(cAcno_qs, 0 , sizeof(cAcno_qs));
	ret = pub_base_GetParm_Str(g_pub_tx.tx_br_no,PARM_CARD_USER_FEE,cAcno_qs);
	if (ret)
	{
		memset(cAcno_qs, 0 , sizeof(cAcno_qs));
		strncpy(cAcno_qs,"13902",sizeof(cAcno_qs)-1);
		vtcp_log("%s,%d 没有找到垫款帐户，所以启用默认帐户[%s]",__FILE__,__LINE__,cAcno_qs);
	}
	set_zd_data("0330",cAcno_qs);
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

