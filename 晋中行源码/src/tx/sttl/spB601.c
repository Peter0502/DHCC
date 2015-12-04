/*************************************************
* 文 件 名:  spB601.c
* 功能描述： 债券投资买入信息维护
*
* 作    者:  
* 完成日期： 2003年1月20日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "mo_invest_c.h"

int spB601()
{
    struct mo_invest_c sMo_invest,sMo_invest1;
    char flag[2];
    int ret;
  
    memset(&sMo_invest,0x0,sizeof(struct mo_invest_c));
    memset(&sMo_invest1,0x0,sizeof(struct mo_invest_c));

    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"初始化公用结构错误!!");
        WRITEMSG
        goto ErrExit;
    }
    
    get_zd_data("0670",flag); /* 操作标志 0 录入 1 修改 2 删除 */
    get_zd_data("0580",sMo_invest.pact_no);    /* 投资协议编号 */
    get_zd_data("0230",sMo_invest.bkrl_code);  /* 产品编号 */
    get_zd_data("0860",sMo_invest.ticket_type);/* 买入债券品种 */
    get_zd_double("0390",&sMo_invest.buy_amt); /* 买入价 */
    get_zd_double("0400",&sMo_invest.par_bal); /* 票面价 */
    get_zd_long("0440",&sMo_invest.isuue_date);/* 发行日期 */
    get_zd_long("0480",&sMo_invest.term);       /* 债券期限 */
    get_zd_long("0450",&sMo_invest.mtr_date);  /* 到期日期 */
    get_zd_long("0460",&sMo_invest.buy_date);  /* 买入日期 */
    get_zd_data("0680",sMo_invest.intst_type); /* 债券计息方式 */
    get_zd_double("0840",&sMo_invest.rate);    /* 收益率 */
	get_zd_double("0410",&sMo_invest.pre_intst); /* 应记利息 */
	get_zd_double("0420",&sMo_invest.overflow_amt); /* 溢价 */
	get_zd_double("0430",&sMo_invest.abate_amt);	  /* 折价 */
 /*	get_zd_data("0810",&sMo_invest.ticket_type); */    /*投资债券品种 */
/*	get_zd_double("1003",&sMo_invest.singl_feeval);	   手续费 */
    strcpy(sMo_invest.sts,"1");
    sMo_invest.wrk_date = g_pub_tx.tx_date;
    sMo_invest.trace_no = g_pub_tx.trace_no;
    strcpy(sMo_invest.br_no,g_pub_tx.tx_br_no);
    strcpy(sMo_invest.tel,g_pub_tx.tel);
    strcpy(sMo_invest.chk,g_pub_tx.chk);
    strcpy(sMo_invest.auth,g_pub_tx.auth);

    
    switch (atoi(flag))
    {
        case 0 :
            /* 录入 */
            
            sprintf(acErrMsg,"录入!!!!");
            WRITEMSG
           
            /* 检查该投资合同号是否有重复 */
            ret = Mo_invest_Sel(g_pub_tx.reply,&sMo_invest1," pact_no = '%s' ",
                  sMo_invest.pact_no);
            vtcp_log("aaaaaaaaaaaaaaaaaaa [%s]\n",sMo_invest1.pact_no);
            if (ret != 0 && ret == !100)
            {
                sprintf(acErrMsg,"查询投资登记簿错误!! [%d]",ret);
                WRITEMSG
                strcpy(g_pub_tx.reply,"B082");
                goto ErrExit;
            }
            else if (ret == 0)
            {
                sprintf(acErrMsg,"该投资协议编号已经存在!!");
                WRITEMSG
                strcpy(g_pub_tx.reply,"B083");
                goto ErrExit;
            }

            ret = Mo_invest_Ins(sMo_invest,g_pub_tx.reply);
            if (ret != 0)
            {
                sprintf(acErrMsg,"登记投资登记簿出错 !! [%d]",ret);
                WRITEMSG
                goto ErrExit;
            }
            strcpy(g_pub_tx.brf,"债券投资买入信息录入");
            sprintf(acErrMsg,"向投资登记簿插入数据 pass!!!");
            WRITEMSG
            break;

        case 1 :
            /* 修改 */
            
            ret = Mo_invest_Dec_Upd(g_pub_tx.reply,"pact_no ='%s'",
                  sMo_invest.pact_no);
            if (ret != 0)
            {
                sprintf(acErrMsg,"dec for update error [%d] ",ret);
                WRITEMSG
                goto ErrExit;
            }
            ret = Mo_invest_Fet_Upd(&sMo_invest1,g_pub_tx.reply);
            if (ret != 0)
            {
                sprintf(acErrMsg,"fet for update error [%d]",ret);
                WRITEMSG
                Mo_invest_Clo_Upd();
                strcpy(g_pub_tx.reply,"B082");
                goto ErrExit;
            }
            
            if (sMo_invest1.sts[0] != '1')
            {
                sprintf(acErrMsg,"该项不允许修改!!!");
                WRITEMSG
                Mo_invest_Clo_Upd();
                strcpy(g_pub_tx.reply,"B009");
                goto ErrExit;
            }
	/*	memcpy(&sMo_invest1, &sMo_invest, sizeof(sMo_invest));*/
		strcpy(sMo_invest.rowid, sMo_invest1.rowid);
            
        sprintf(acErrMsg,"UPDATE %d@%s 买入价结构[%lf]\n", __LINE__, __FILE__, sMo_invest1.buy_amt);
        WRITEMSG
            ret = Mo_invest_Upd_Upd(sMo_invest,g_pub_tx.reply);
            if (ret != 0)
            {
                sprintf(acErrMsg,"修改投资登记簿出错 !! [%d]",ret);
                WRITEMSG
                Mo_invest_Clo_Upd();
                strcpy(g_pub_tx.reply,"B036");
                goto ErrExit;
            }
            strcpy(g_pub_tx.brf,"债券投资信息修改");
            Mo_invest_Clo_Upd();
            sprintf(acErrMsg,"修改投资登记簿  pass!! ");
            WRITEMSG
            break;

        case 2 :
            /* 删除 */

            ret = Mo_invest_Sel(g_pub_tx.reply,&sMo_invest1,"pact_no = '%s'",
                  sMo_invest.pact_no);
            if (ret != 0)
            {
                sprintf(acErrMsg,"select mo_invest error !! [%d]",ret);
                WRITEMSG
                goto ErrExit;
            }
            if (sMo_invest1.sts[0] != '1')
            {
                sprintf(acErrMsg,"该项不允许删除!!!");
                WRITEMSG
                strcpy(g_pub_tx.reply,"B010");
                goto ErrExit;
            }
            ret = sql_execute("delete from mo_invest where pact_no='%s' and \
                  sts ='%s'",sMo_invest.pact_no, sMo_invest.sts);
            if (ret != 0)
            {
                sprintf(acErrMsg,"删除投资登记簿出错!! [%d] ",ret);
                WRITEMSG
                strcpy(g_pub_tx.reply,"B037");
                goto ErrExit;
            }
            strcpy(g_pub_tx.brf,"债券投资信息删除");
            sprintf(acErrMsg,"删除投资登记簿中记录 pass!! ");
            WRITEMSG
            break;

        default:
            goto ErrExit;
    }
 
    strcpy(g_pub_tx.prdt_code,sMo_invest.bkrl_code);
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

