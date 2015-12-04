#include "find_debug.h" 
/***************************************************************
* 文 件 名:     pubf_base_self.c
* 功能描述：    自助设备的公用函数
* 调用函数说明：
*
*
* 作    者:     rob
* 完成日期：    2003年02月18日
*
* 修改记录：
* 1. 日    期:
*    修 改 人:
*    修改内容:
*
**************************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "chnl_tradeadd_c.h"
#include "chnl_tradelimit_c.h"
#include "chnl_self_trade_c.h"
#include "march_info_c.h"

/**********************************************************************
* 函数名：    pub_base_InsertTradeAdd()
* 函数功能：  向交易记录累计控制表中增加记录
* 作者/时间： rob/2003年02月18日
*
* 参数：
*     输入：
*     输出:
* 返 回 值: 0 成功
*
* 修改历史：
*
********************************************************************/
int pub_base_InsertTradeAdd(char tx_type[2])
{
    int ret;
    char wherelist[100];
    struct chnl_tradeadd_c chnl_tradeadd;

    MEMSET_DEBUG(&chnl_tradeadd,0x0,sizeof(struct chnl_tradeadd_c));
    chnl_tradeadd.tx_date = g_pub_tx.tx_date;
    strcpy(chnl_tradeadd.ac_no,g_pub_tx.crd_no);
    strcpy(chnl_tradeadd.tx_type,tx_type); /* atm */
    strcpy(chnl_tradeadd.stat,"0");    /* 正常 */

	/*
    sprintf(wherelist," tx_date = %ld and ac_no = '%s'",
            g_pub_tx.tx_date,g_pub_tx.crd_no);
    sprintf(acErrMsg,"insert tradeadd**** [%s] ****",wherelist);
    WRITEMSG
    ret = sql_count("chnl_tradeadd","%s",wherelist);
    if (ret <= 0)
    {
        * 向交易记录累计控制表中增加记录 */
     
sprintf(acErrMsg,"insert into chnl_tradeadd begin!!");
WRITEMSG

        chnl_tradeadd.add_pswd_cnt = 0;
        ret = Chnl_tradeadd_Ins(chnl_tradeadd,g_pub_tx.reply);
        if (ret != 0)
        {
            sprintf(acErrMsg,"向交易记录累计控制表中增加记录错误!! [%d]",ret);
            WRITEMSG
            return 1;
        }
sprintf(acErrMsg,"insert into chnl_tradeadd end!!");
WRITEMSG

    return 0;
}

/**********************************************************************
* 函数名：    pub_base_Checktradelimit(char chnl)
* 函数功能：  检查交易限制(每次取款金额 累计取款额 累计次数)
* 作者/时间： rob/2003年02月18日
*
* 参数：
*     输入：
*           0 --atm 1--pos
*     输出:
* 返 回 值: 0 成功  !0 失败
*
* 修改历史：
*
********************************************************************/
int pub_base_Checktradelimit(char chnl)
{
    struct chnl_tradelimit_c chnl_tradelimit;
    struct chnl_tradeadd_c chnl_tradeadd_c;
    int ret;
    char tx_type[2];

    MEMSET_DEBUG(&chnl_tradelimit,0x0,sizeof(struct chnl_tradelimit_c));
    MEMSET_DEBUG(&chnl_tradeadd_c,0x0,sizeof(struct chnl_tradeadd_c));

    ret = Chnl_tradelimit_Sel(g_pub_tx.reply,&chnl_tradelimit,
          "chnl='%c'",chnl);
    if (ret != 0)
    {
        sprintf(acErrMsg,"查询交易限制表出错!!! [%d]",ret);
        WRITEMSG
		strcpy(g_pub_tx.reply, "AT08");
        return ret;
    }

    if (chnl == '0')
    {
        strcpy(tx_type, "1"); /* atm */
    }
    else if (chnl == '1')
    {
        strcpy(tx_type, "3"); /* pos */
    }
    else
    {
		sprintf(acErrMsg,"交易类型错误!! [%c]",chnl);
		WRITEMSG
		strcpy(g_pub_tx.reply, "AT08");
        return 1;
    }
    
    ret = Chnl_tradeadd_Sel(g_pub_tx.reply, &chnl_tradeadd_c,"tx_date=%ld and \
      ac_no='%s' and tx_type ='%s'",g_pub_tx.tx_date,g_pub_tx.crd_no,tx_type);
    if (ret != 0 && ret != 100)
    {
        sprintf(acErrMsg,"查询交易记录累计控制表出错!! [%d]",ret);
        WRITEMSG
		strcpy(g_pub_tx.reply, "AT08");
        return ret;
    }
	else if (ret == 100)
	{
		/* 向chnl_tradeadd表中增加一条纪录 */
		chnl_tradeadd_c.tx_date = g_pub_tx.tx_date;
		strcpy(chnl_tradeadd_c.ac_no,g_pub_tx.crd_no);
		strcpy(chnl_tradeadd_c.tx_type,tx_type);
		chnl_tradeadd_c.add_amt = 0.00;
		chnl_tradeadd_c.add_qk_cnt = 0;
		chnl_tradeadd_c.add_pswd_cnt = 0;
		strcpy(chnl_tradeadd_c.stat,"0");
		ret = Chnl_tradeadd_Ins(chnl_tradeadd_c, g_pub_tx.reply);
		if (ret != 0)
		{
			sprintf(acErrMsg,"向交易记录累计控制表中增加纪录错误!! [%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply, "AT29");
			return ret;
		}
	}

    /* 检查取款累计次数 */
    if (chnl_tradeadd_c.add_qk_cnt + 1 > chnl_tradelimit.wd_cnt)
    {
        sprintf(acErrMsg,"累计取款次数超出每天的限制!! 已取款次数:[%d], \
			max取款次数:[%d]",chnl_tradeadd_c.add_qk_cnt, 
			chnl_tradelimit.wd_cnt);
        WRITEMSG
		strcpy(g_pub_tx.reply, "AT09");
        return 1;
    }

    /* 检查每次取款额是否超出限制 */
	ret = pub_base_CompDblVal(g_pub_tx.tx_amt1,chnl_tradelimit.wd_amt);
    if ( ret > 0)
    {
        sprintf(acErrMsg, "每次取款额超出限制,本次交易金额:[%lf], 每次max\
			取款金额:[%lf]",g_pub_tx.tx_amt1, chnl_tradelimit.wd_amt);
        WRITEMSG
		strcpy(g_pub_tx.reply, "AT10");
        return 1;
    }

    /* 检查每天取款总额超出限制 */
	ret = pub_base_CompDblVal(chnl_tradeadd_c.add_amt + g_pub_tx.tx_amt1,
			chnl_tradelimit.wd_ttl_max_amt);
    if (ret > 0)
    {
        sprintf(acErrMsg,"每天取款总额超出总的限制!! 已取款金额:[%lf], \
			本次交易金额:[%lf], max总取款金额:[%lf]", chnl_tradeadd_c.add_amt,
			g_pub_tx.tx_amt1, chnl_tradelimit.wd_ttl_max_amt);
        WRITEMSG
		strcpy(g_pub_tx.reply, "AT11");
        return 1;
    }

    return 0;
}
/**********************************************************************
* 函数名：    pub_base_Insertselftrade(struct chnl_self_trade_c)
* 函数功能：  向自助设备交易日记中增加记录
* 作者/时间： rob/2003年02月18日
*
* 参数：
*     输入：
*            struct chnl_self_trade_c chnl_self_trade
*     输出:
* 返 回 值: 0 成功  !0 失败
*
* 修改历史：
*
********************************************************************/
int pub_base_Insertselftrade(struct chnl_self_trade_c chnl_self_trade)
{
    int ret;

    ret = Chnl_self_trade_Ins(chnl_self_trade,g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"向自助设备交易日记中增加记录错误 [%d]",ret);
        WRITEMSG
        return ret;
    }
    return 0;
}
/**********************************************************************
* 函数名：    pub_base_Selmrchacno(char mrchno[],struct march_info_c *)
* 函数功能：  根据商户号查询商户信息表并检查该商户的信息
* 作者/时间： rob/2003年02月18日
*
* 参数：
*     输入：
*            marchno 商户号
*     输出:
*            struct march_info_c sMarch_info 
* 返 回 值: 0 成功  !0 失败
*
* 修改历史：
*
********************************************************************/
int pub_base_Selmrchacno(char mrchno[],struct march_info_c *sMarch_info)
{
    int ret;
   
    ret=March_info_Sel(g_pub_tx.reply,sMarch_info,"mrchno='%s'",mrchno);
    if (ret != 0)
    {
        sprintf(acErrMsg,"查询商户信息管理表错误!!! [%d]",ret);
        WRITEMSG
        return ret;
    }

    /* 检查该商户的状态 */
    if (strcmp(sMarch_info->mrchstat,"0") != 0)
    {
        sprintf(acErrMsg,"该商户的状态不正常!!");
        WRITEMSG
        return 1;
    }

    /* 检查该协议是否有效 */
    if ((g_pub_tx.tx_date < sMarch_info->beg_date) 
       || (g_pub_tx.tx_date >= sMarch_info->end_date))
    {
        sprintf(acErrMsg,"该商户的协议已过期!!!");
        WRITEMSG
        return 1;
    }
    return 0;
}
