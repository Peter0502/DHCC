/*************************************************************
* 文 件 名: gD043.c
* 功能描述：产品销售情况统计
*
* 作    者: mike
* 完成日期: 2003年3月8日
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
#include "trace_log_c.h"
#include "prdt_ac_id_c.h"
#include "ass_prdt_sale_sta_c.h"

gD043()
{
	int ret = 0;
    int flag ; /* 1 插入　2 更新 */
    long last_trace_no;
    double tx_amt;

    struct trace_log_c trace_log_c;
    struct prdt_ac_id_c prdt_ac_id_c;
    struct ass_prdt_sale_sta_c ass_prdt_sale_sta_c;

    last_trace_no = 0;
    memset(&trace_log_c,0x0,sizeof(struct trace_log_c));
    memset(&prdt_ac_id_c,0x0,sizeof(struct prdt_ac_id_c));
    memset(&ass_prdt_sale_sta_c,0x0,sizeof(struct ass_prdt_sale_sta_c));

	ret=sql_begin(); /*打开事务*/
	if(ret != 0) 
	{
		sprintf( acErrMsg, "打开事务失败!!!" );
		WRITEMSG
		goto ErrExit;
	}

    /**------- 初始化全局变量 --------**/
	pub_base_sysinit();

    /* 查询业务流水 */
    ret = Trace_log_Dec_Sel(g_pub_tx.reply,"tx_date=%ld order by ac_id,ac_seqn",g_pub_tx.tx_date);
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR SELECT ERROR!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    while(1)
    {
        ret = Trace_log_Fet_Sel(&trace_log_c,g_pub_tx.reply);
        if (ret != 0 && ret != 100)
        {
            sprintf(acErrMsg,"FETCH FOR SELECT ERROR!! [%d]",ret);
            WRITEMSG
            goto ErrExit;
        }
        else if (ret == 100)
        {
            sprintf(acErrMsg,"业务流水中的记录统计完毕!!!");
            WRITEMSG
            break;
        }

        /****
        if (trace_log_c.ac_id == 0)
        {
            sprintf(acErrMsg,"该流水没有帐号!!");
            WRITEMSG
            continue;
        }
        ****/
 
        /* 判断是否是开户交易 */
        if ((strcmp(trace_log_c.tx_code,"3101") != 0)  /* 贷款开户 */
         && (strcmp(trace_log_c.tx_code,"2101") != 0)  /* 无介质活期开户 */
         && (strcmp(trace_log_c.tx_code,"2102") != 0)  /* 有介质活期开户 */
         && (strcmp(trace_log_c.tx_code,"2103") != 0)  /* 无介质定期开户 */
         && (strcmp(trace_log_c.tx_code,"2104")) != 0) /* 有介质定期开户 */
        {
            continue;
        }
        
        pub_base_strpack(trace_log_c.sub_tx_code);
        if ((strcmp(trace_log_c.sub_tx_code,"D002") != 0)
          && (strcmp(trace_log_c.sub_tx_code,"L101") != 0))
        {
            sprintf(acErrMsg,"该流水不是开户子交易!!");
            WRITEMSG
            continue;
        }       

        sprintf(acErrMsg,"该流水是开户子交易!!");
        WRITEMSG

        /* 查询产品帐号对照表 *
        sprintf(acErrMsg,"ac_id=[%ld], ac_seqn = [%d]",trace_log_c.ac_id,
                trace_log_c.ac_seqn);
        WRITEMSG
        ********/

        ret = Prdt_ac_id_Sel(g_pub_tx.reply,&prdt_ac_id_c,"ac_id=%ld and \
              ac_seqn=%d", trace_log_c.ac_id, trace_log_c.ac_seqn);
        if (ret != 0 && ret != 100)
        {
            sprintf(acErrMsg,"查询产品帐号对照表错误!!![%d]",ret);
            WRITEMSG
            goto ErrExit;
        }
        else if (ret == 100)
        {
            sprintf(acErrMsg,"产品帐号中没有该记录!!!");
            WRITEMSG
            continue;
        }

        TRACE
        strcpy(ass_prdt_sale_sta_c.br_no,trace_log_c.opn_br_no);
        strcpy(ass_prdt_sale_sta_c.prdt_code,prdt_ac_id_c.prdt_no);
        ass_prdt_sale_sta_c.date = g_pub_tx.tx_date;

        TRACE
        ret = Ass_prdt_sale_sta_Sel(g_pub_tx.reply,&ass_prdt_sale_sta_c,
              " prdt_code='%s' and br_no='%s' and \"date\"=%ld ", 
              ass_prdt_sale_sta_c.prdt_code, ass_prdt_sale_sta_c.br_no,
              ass_prdt_sale_sta_c.date);
        if (ret != 0 && ret != 100)
        {
            sprintf(acErrMsg,"查询产品销售情况统计表错误!! [%d]",ret);
            WRITEMSG
            goto ErrExit;
        }
        else if (ret == 100)
        {
            sprintf(acErrMsg,"向产品销售情况统计表中插入一条记录!!");
            WRITEMSG
            flag = 1; /* 插入 */
        }
        else
        {
            sprintf(acErrMsg,"更新产品销售情况统计中的记录");
            WRITEMSG
            flag = 2; /* 更新 */
        }

        TRACE
        if (last_trace_no == trace_log_c.trace_no)
        {
            continue;
        }
        last_trace_no = trace_log_c.trace_no;

        pub_base_strpack(trace_log_c.tx_code);
        if (flag == 1)
        {
            ass_prdt_sale_sta_c.sal_cnt = 1;
            ass_prdt_sale_sta_c.ttl_amt = 0.00;

            ret = sql_sum_double("trace_log","amt",&ass_prdt_sale_sta_c.ttl_amt,
                  "trace_no=%ld", trace_log_c.trace_no);
            if (ret != 0)
            {
                sprintf(acErrMsg,"汇总交易日记表中该笔交易的发生额!!!");
                WRITEMSG
                goto ErrExit;
            } 

            ass_prdt_sale_sta_c.ttl_amt = ass_prdt_sale_sta_c.ttl_amt / 2;

            ret = Ass_prdt_sale_sta_Ins(ass_prdt_sale_sta_c,g_pub_tx.reply);
            if (ret != 0)
            {
                sprintf(acErrMsg,"向产品销售情况统计表中插入数据错误!!");
                WRITEMSG
                goto ErrExit;
            }
        }
        else
        {

            ass_prdt_sale_sta_c.sal_cnt += 1;

            ret = sql_sum_long("trace_log","amt",&tx_amt,
                  "trace_no=%ld", trace_log_c.trace_no);
            if (ret != 0)
            {
                sprintf(acErrMsg,"汇总交易日记表中该笔交易的发生额!!!");
                WRITEMSG
                goto ErrExit;
            } 
            ass_prdt_sale_sta_c.ttl_amt += tx_amt / 2;

            ret = pub_prdt_sale_sta_upd(ass_prdt_sale_sta_c,g_pub_tx.reply);
            if (ret != 0)
            {
                sprintf(acErrMsg,"插入或更新产品销售情况统计表错误!!");
                WRITEMSG
                goto ErrExit;
            }
        }
    }   
    Trace_log_Clo_Sel();

OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"产品销售情况统计成功!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
    Trace_log_Clo_Sel();
	sql_rollback();	/*--事务回滚--*/
	sprintf(acErrMsg,"产品销售情况统计失败!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
