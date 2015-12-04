/*************************************************
* 文 件 名:  spB605.c
* 功能描述： 股权购入信息维护（录入、修改、删除）
*
* 作    者:  rob
* 完成日期： 2003年1月25日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "mo_righ_invest_c.h"

int spB605()
{
    struct mo_righ_invest_c sMo_righ_invest,sMo_righ_invest1;
    char flag[2];
    int ret;
  
    memset(&sMo_righ_invest,0x0,sizeof(struct mo_righ_invest_c));
    memset(&sMo_righ_invest1,0x0,sizeof(struct mo_righ_invest_c));
    
    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"初始化公用结构错误!!");
        WRITEMSG
        goto ErrExit;
    }
    
    get_zd_data("0670",flag); /* 操作标志 0 录入 1 修改 2 删除 */
    get_zd_data("0580",sMo_righ_invest.pact_no);    /* 投资协议编号 */
    get_zd_data("0680",sMo_righ_invest.invst_type); /* 股权投资类别 */
    get_zd_data("0810",sMo_righ_invest.name);   /* 被投资单位名称 */
    get_zd_double("0390",&sMo_righ_invest.amt); /* 买入价 */
    get_zd_long("0530",&sMo_righ_invest.num); /* 股数量 */
	vtcp_log("amt=[%lf]num=[%d]",sMo_righ_invest.amt,sMo_righ_invest.num);
    
    strcpy(sMo_righ_invest.br_no,g_pub_tx.tx_br_no);
    if (sMo_righ_invest.invst_type[0] == '1')
    {
        /* 短期权益性投资 */
        strcpy(sMo_righ_invest.bkrl_code,"938");
    }
    else
    {
        /* 中长期权益性投资 */
        strcpy(sMo_righ_invest.bkrl_code,"938");
    }
    sMo_righ_invest.wrk_date = g_pub_tx.tx_date;
    strcpy(sMo_righ_invest.sts,"1");
    sMo_righ_invest.trace_no = g_pub_tx.trace_no;

    switch (atoi(flag))
    {
        case 0 :
            /* 录入 */
            
            sprintf(acErrMsg,"录入!!!!");
            WRITEMSG
  
            /* 检查该投资协议编号是否已经存在 */
            ret = Mo_righ_invest_Sel(g_pub_tx.reply,&sMo_righ_invest1,
                  "pact_no='%s'",sMo_righ_invest.pact_no);
            if (ret != 0 && ret != 100)
            {
                sprintf(acErrMsg,"查询权益性投资登记簿错误!! [%d]",ret);
                WRITEMSG
                goto ErrExit;
            }
            else if (ret == 0)
            {
                sprintf(acErrMsg,"权益性投资登记簿中已经存在该协议编号");
                WRITEMSG
                strcpy(g_pub_tx.reply,"B086");
                goto ErrExit;
            }

		vtcp_log("amt=[%lf]num=[%d]",sMo_righ_invest.amt,sMo_righ_invest.num);
            ret = Mo_righ_invest_Ins(sMo_righ_invest,g_pub_tx.reply);
            if (ret != 0)
            {
                sprintf(acErrMsg,"登记权益性投资登记簿出错 !! [%d]",ret);
                WRITEMSG
                goto ErrExit;
            }
            strcpy(g_pub_tx.brf,"股权购入信息录入");
            sprintf(acErrMsg,"向权益性投资登记簿插入数据 pass!!!");
            WRITEMSG
            break;

        case 1 :
            /* 修改 */
            ret = Mo_righ_invest_Dec_Upd(g_pub_tx.reply,"pact_no='%s'",
                  sMo_righ_invest.pact_no);
            if (ret != 0)
            {
                sprintf(acErrMsg,"dec for update error [%d] ",ret);
                WRITEMSG
                goto ErrExit;
            }

            ret = Mo_righ_invest_Fet_Upd(&sMo_righ_invest1,g_pub_tx.reply);
            if (ret != 0)
            {
                sprintf(acErrMsg,"fet for update error [%d]",ret);
                WRITEMSG
                Mo_righ_invest_Clo_Upd();
                goto ErrExit;
            }
            
            if (sMo_righ_invest1.sts[0] != '1')
            {
                sprintf(acErrMsg,"该项不允许修改!!!");
                WRITEMSG
                Mo_righ_invest_Clo_Upd();
                strcpy(g_pub_tx.reply,"B009");
                goto ErrExit;
            }
            
            ret = Mo_righ_invest_Upd_Upd(sMo_righ_invest,g_pub_tx.reply);
            if (ret != 0)
            {
                sprintf(acErrMsg,"修改权益性投资登记簿出错 !! [%d]",ret);
                WRITEMSG
                Mo_righ_invest_Clo_Upd();
                strcpy(g_pub_tx.reply,"B036");
                goto ErrExit;
            }
            strcpy(g_pub_tx.brf,"股权购入信息修改");
            Mo_righ_invest_Clo_Upd();
            sprintf(acErrMsg,"修改权益性投资登记簿  pass!! ");
            WRITEMSG
            break;

        case 2 :
            /* 删除 */

            ret = Mo_righ_invest_Sel(g_pub_tx.reply,&sMo_righ_invest1,
                  "pact_no='%s'",sMo_righ_invest.pact_no);
            if (ret != 0)
            {
                sprintf(acErrMsg,"select mo_invest error !! [%d]",ret);
                WRITEMSG
                goto ErrExit;
            }
            if (sMo_righ_invest1.sts[0] != '1')
            {
                sprintf(acErrMsg,"该项不允许删除!!!");
                WRITEMSG
                strcpy(g_pub_tx.reply,"B010");
                goto ErrExit;
            }
            ret = sql_execute("delete from mo_righ_invest where pact_no='%s' \
                   and sts ='1'",sMo_righ_invest.pact_no);
            if (ret != 0)
            {
                sprintf(acErrMsg,"删除权益性投资登记簿出错!! [%d] ",ret);
                WRITEMSG
                strcpy(g_pub_tx.reply,"B037");
                goto ErrExit;
            }

            strcpy(g_pub_tx.brf,"股权购入信息删除");
            sprintf(acErrMsg,"删除权益性投资登记簿中记录 pass!! ");
            WRITEMSG
            break;

        default:
            goto ErrExit;
    }
 
    if (pub_ins_trace_log())
    {
        sprintf(acErrMsg,"登记交易流水错误!!!");
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
