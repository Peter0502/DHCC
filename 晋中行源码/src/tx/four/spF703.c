/***********************************************************
* 文 件 名: spF703.c
* 功能描述：异地活期余额
* 作    者: ChenMing
* 完成日期：200h6年1月11日
*
* 修改记录：
* 日    期:
* 修 改 人: 
* 修改内容:	
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "card.h"
spF703()
{
	int ret = 0;
	char cCur_no[3];
	char cCity[5];
	char cPswd[7];
	char cAcno_qs[25];
	int iAc_seq = 0;

	memset(cAcno_qs, 0 , sizeof(cAcno_qs));
	memset(cCity,0x0,sizeof(cCity));
	memset(cCur_no,0x0,sizeof(cCur_no));
	memset(cPswd,0x0,sizeof(cPswd));
    memset(&g_dd_mst,0x0,sizeof(struct dd_mst_c));
    memset(&g_mdm_ac_rel,0x0,sizeof(struct mdm_ac_rel_c));

	/*** 初始化全局变量 ***/
	pub_base_sysinit();

	get_zd_data("0360",cCity);
	
/*  Remed by ChenMing for test 
	if( strcmp(cCity, "4302") != 0)
	{
			strcpy(g_pub_tx.reply,"F001");
			sprintf(acErrMsg,"非本行卡,不能在本行主机查询![%s]",g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
	}
*/

    get_zd_data("0370",g_mdm_ac_rel.ac_no);
    get_zd_data("0210",cCur_no);
    get_zd_data("0800",cPswd);
    iAc_seq = atoi(cCur_no);

    iAc_seq = 1;
    
    sprintf(acErrMsg,"----->iAc_seq====[%d]!", iAc_seq );
    WRITEMSG
   
    sprintf(acErrMsg,"----->ac_no=[%s],pass[%s]!",g_mdm_ac_rel.ac_no,cPswd);
    WRITEMSG
    
    ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &g_mdm_ac_rel, \
				"ac_no='%s'", g_mdm_ac_rel.ac_no );
    if(  ret == 100 )
    {
       	sprintf(acErrMsg,"----------->查询的账号不存在!! [%d]",ret);
       	WRITEMSG
       	strcpy(g_pub_tx.reply, "B004");
       	goto ErrExit;
    }
	if( ret ) 
	{
       	sprintf(acErrMsg,"------->查询账号信息错误!! [%d]", ret );
       	WRITEMSG
       	strcpy(g_pub_tx.reply, "B004");
       	goto ErrExit;
	}

	pub_base_EnDes( g_pub_tx.tx_date, "", cPswd );
	if ( strcmp( g_mdm_ac_rel.draw_pwd, cPswd ) )
	{
       	sprintf(acErrMsg,"密码错!");
       	WRITEMSG
       	strcpy(g_pub_tx.reply, "M005");
       	goto ErrExit;
	}

	ret = Dd_mst_Sel( g_pub_tx.reply,&g_dd_mst,"ac_id=%ld and ac_seqn=%d", \
			g_mdm_ac_rel.ac_id, iAc_seq );
    if( ret )
    {
        sprintf(acErrMsg,"查询户名和余额错误!! [%d]",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply, "B005");
        goto ErrExit;
    }
	/* begin add by LiuHuafeng 20070204 */
	ret = pubf_card_chk_msr(1,g_mdm_ac_rel.ac_no);
	if( ret ) 
	{
       	sprintf(acErrMsg,"%s,%d卡号在卡登记簿中不存在!! [%d]",__FILE__,__LINE__,ret );
       	WRITEMSG
       	strcpy(g_pub_tx.reply, "B004");
       	goto ErrExit;
	}
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
			if(atm_qy_free_flag==0)
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
	/* 准备总行手续费支出科目 0.1元 52701*/
	memset(cAcno_qs, 0 , sizeof(cAcno_qs));
	ret = pub_base_GetParm_Str(g_pub_tx.tx_br_no,PARM_CARD_OPEN_FEE,cAcno_qs);
	if (ret)
	{
		memset(cAcno_qs, 0 , sizeof(cAcno_qs));
		strncpy(cAcno_qs,"52701",sizeof(cAcno_qs)-1);
		vtcp_log("%s,%d 没有找到手续费支出科目，所以启用默认帐户=%s",__FILE__,__LINE__,cAcno_qs);
	}
	set_zd_data("0640",cAcno_qs);
	/* 准备总行垫付手续费科目 0.1元  139帐号 */
	memset(cAcno_qs, 0 , sizeof(cAcno_qs));
	ret = pub_base_GetParm_Str(g_pub_tx.tx_br_no,PARM_CARD_USER_FEE,cAcno_qs);
	if (ret)
	{
		memset(cAcno_qs, 0 , sizeof(cAcno_qs));
		strncpy(cAcno_qs,"13902",sizeof(cAcno_qs)-1);
		vtcp_log("%s,%d 没有找到垫款帐户，所以启用默认帐户[%s]",__FILE__,__LINE__,cAcno_qs);
	}
	set_zd_data("0330",cAcno_qs);
	
	/* end by LiuHuafeng 20070204 */

	set_zd_data("0250",g_dd_mst.name);
	set_zd_data( "0270", g_dd_mst.name );
	set_zd_double("0420",g_dd_mst.bal);

	/* 根据 账面余额 和 当日可用余额 设置 实际余额 FD74 */
	ret = iSetAdditional_Amounts( g_dd_mst.bal, g_dd_mst.bal);  /* 设置 FD74 实际余额 */
	if(ret)
	{
		/* rem by LiuHuafeng 2007-2-8 10:44 
      return -1;
		* end by LiuHuafeng 2007-2-8 10:44 */
		vtcp_log("%s,%d 设置金额出现错误",__FILE__,__LINE__);
		strncpy(g_pub_tx.reply,"AT06",4);
		goto ErrExit;
	}
OKExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"查询余额处理成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"查询余额处理失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
