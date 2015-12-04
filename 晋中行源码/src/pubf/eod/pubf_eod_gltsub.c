/*************************************************
* 文 件 名:     pubf_eod_gltsub.c
* 功能描述：    统计周期总帐
* 作    者:     rob
* 完成日期：    2004年03月17日
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "com_parm_c.h"
#include "gl_mst_hst_c.h"
#include "gl_mst_c.h"
#include "gl_tsub_c.h"

/**********************************************************************
 * 函 数 名：   pub_eod_ins_gltsub
 * 函数功能：   统计周期总帐
 * 作者/时间：  rob/20040317
 *
 * 参数：
 *     输入: 
 *     输出: 
 *     返回: <>0失败
             =0成功
 *
 * 修改历史：
 *
********************************************************************/
int pub_eod_ins_gltsub()
{
    int ret;

    ret = pub_eod_stat_five_gltsub();
    if (ret != 0)
    {
        sprintf(acErrMsg,"统计五天周期总帐错误!!");
        WRITEMSG
        return ret;
    }

    ret = pub_eod_stat_seven_gltsub();
    if (ret != 0)
    {
        sprintf(acErrMsg,"统计七天周期总帐错误!!");
        WRITEMSG
        return ret;
    }
    
    ret = pub_eod_stat_td_gltsub();
    if (ret != 0)
    {
        sprintf(acErrMsg,"统计旬周期总帐错误!!");
        WRITEMSG
        return ret;
    }
    
    ret = pub_eod_stat_halfmonth_gltsub();
    if (ret != 0)
    {
        sprintf(acErrMsg,"统计半月周期总帐错误!!");
        WRITEMSG
        return ret;
    }
    
    ret = pub_eod_stat_month_gltsub();
    if (ret != 0)
    {
        sprintf(acErrMsg,"统计月周期总帐错误!!");
        WRITEMSG
        return ret;
    }
    
    ret = pub_eod_stat_season_gltsub();
    if (ret != 0)
    {
        sprintf(acErrMsg,"统计季周期总帐错误!!");
        WRITEMSG
        return ret;
    }
    
    ret = pub_eod_stat_halfyear_gltsub();
    if (ret != 0)
    {
        sprintf(acErrMsg,"统计半年周期总帐错误!!");
        WRITEMSG
        return ret;
    }
    
    ret = pub_eod_stat_year_gltsub();
    if (ret != 0)
    {
        sprintf(acErrMsg,"统计年周期总帐错误!!");
        WRITEMSG
        return ret;
    }
    return 0;
}
/**********************************************************************
 * 函 数 名：   pub_eod_stat_five_gltsub
 * 函数功能：   统计五天周期总帐
 * 作者/时间：  rob/20040317
 *
 * 参数：
 *     输入: 
 *     输出: 
 *     返回: <>0失败
             =0成功
 *
 * 修改历史：
 *
********************************************************************/
int pub_eod_stat_five_gltsub()
{
    int ret;
    long last_date;
    char val[10];
    
    struct com_parm_c com_parm_c;
    struct gl_mst_hst_c gl_mst_hst_c;
    struct gl_tsub_c gl_tsub_c;

    memset(&com_parm_c, 0x0, sizeof(struct com_parm_c));
    memset(&gl_mst_hst_c, 0x0, sizeof(struct gl_mst_hst_c));

    ret = Com_parm_Sel(g_pub_tx.reply,&com_parm_c,"parm_code='ZQWT' and \
          parm_seqn=1");
    if (ret != 0)
    {
        sprintf(acErrMsg,"查询系统参数表错误!![%d]",ret);
        WRITEMSG
        return ret;
    }
    last_date = atol(com_parm_c.val);

    if (pub_base_CalTrueDays(last_date,g_pub_tx.tx_date) < 5)
    {
        sprintf(acErrMsg,"与上次统计不足五天!!");
        return 0;
    }
	/*
    else if (pub_base_CalTrueDays(last_date,g_pub_tx.tx_date) != 5)
    {
        sprintf(acErrMsg,"五天统计里不可能有间隔超过五天的!!");
        WRITEMSG
        sprintf(acErrMsg,"上次统计日期=[%ld],本次统计日期=[%ld]",
                last_date, g_pub_tx.tx_date);
        WRITEMSG
        return -1;
    }
	*/

    /* 进行五天周期总帐统计 */
    ret = Gl_mst_hst_Dec_Sel(g_pub_tx.reply,"\"date\"=%ld",last_date);
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR SELECT ERROR [%d]",ret);
        WRITEMSG
        return ret;
    }

    while(1)
    {
        ret = Gl_mst_hst_Fet_Sel(&gl_mst_hst_c, g_pub_tx.reply);
        if (ret != 0 && ret != 100)
        {
            sprintf(acErrMsg,"FETCH FOR SELECT ERROR [%d]",ret);
            WRITEMSG
            return ret;
        }
        else if (ret == 100)
        {
            break;
        }

        memset(&gl_tsub_c, 0x0, sizeof(struct gl_tsub_c));
        strcpy(gl_tsub_c.br_no,gl_mst_hst_c.br_no);
        strcpy(gl_tsub_c.cur_no,gl_mst_hst_c.cur_no);
        strcpy(gl_tsub_c.acc_hrt,gl_mst_hst_c.acc_hrt);
        gl_tsub_c.date = g_pub_tx.tx_date;
        strcpy(gl_tsub_c.bal_d,gl_mst_hst_c.dc_ind);
        gl_tsub_c.bt_dr_bal = gl_mst_hst_c.dr_bal;
        gl_tsub_c.bt_cr_bal = gl_mst_hst_c.cr_bal;
        strcpy(gl_tsub_c.term_type,"1");
    
        /* 统计本期借方发生额 */
        ret = sql_sum_double("gl_mst_hst","rdd_amt",&gl_tsub_c.r_dr_amt,
              "\"date\" > last_date and \"date\" <= %ld", g_pub_tx.tx_date);
        if (ret != 0)
        {
            sprintf(acErrMsg,"统计本期借方发生额错误!! [%d]",ret);
            WRITEMSG
            return ret;
        }
  
        /* 统计本期贷方发生额 */
        ret = sql_sum_double("gl_mst_hst","rcd_amt",&gl_tsub_c.r_cr_amt,
              "\"date\" > last_date and \"date\" <= %ld", g_pub_tx.tx_date);
        if (ret != 0)
        {
            sprintf(acErrMsg,"统计本期贷方发生额错误!! [%d]",ret);
            WRITEMSG
            return ret;
        }

        /* 统计本期借方笔数 */
        ret = sql_sum_long("gl_mst_hst","rdd_cnt",&gl_tsub_c.r_dr_cnt,
              "\"date\" > last_date and \"date\" <= %ld", g_pub_tx.tx_date);
        if (ret != 0)
        {
            sprintf(acErrMsg,"统计本期借方笔数错误!! [%d]",ret);
            WRITEMSG
            return ret;
        }
 
        /* 统计本期贷方笔数 */
        ret = sql_sum_long("gl_mst_hst","rcd_cnt",&gl_tsub_c.r_cr_amt,
              "\"date\" > last_date and \"date\" <= %ld", g_pub_tx.tx_date);
        if (ret != 0)
        {
            sprintf(acErrMsg,"统计本期贷方笔数错误!! [%d]",ret);
            WRITEMSG
            return ret;
        }

        ret = Gl_mst_hst_Sel(g_pub_tx.reply,&gl_mst_hst_c,"\"date\"=%ld and \
              acc_hrt='%s'",g_pub_tx.tx_date,gl_mst_hst_c.acc_hrt);
        if (ret != 0)
        {
            sprintf(acErrMsg,"查询历史总帐表错误!! [%d]",ret);
            WRITEMSG
            return ret;
        }
        gl_tsub_c.r_dr_bal = gl_mst_hst_c.dr_bal;
        gl_tsub_c.r_cr_bal = gl_mst_hst_c.cr_bal;

        ret = Gl_tsub_Ins(gl_tsub_c, g_pub_tx.reply);
        if (ret != 0)
        {
            sprintf(acErrMsg,"向周期总帐里插入数据错误!! [%d]",ret);
            WRITEMSG
            return ret;
        }
    }
    Gl_mst_hst_Clo_Sel();

    /* 更新系统参数表中五天周期统计时间 */
    ret = Com_parm_Dec_Upd(g_pub_tx.reply,"parm_code='ZQWT' and parm_seqn=1");
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR!! [%d]",ret);
        WRITEMSG
        return ret;
    }

    ret = Com_parm_Fet_Upd(&com_parm_c,g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"FETCH ERROR !! [%d]",ret);
        WRITEMSG
        return ret;
    }

    sprintf(val,"%lf",g_pub_tx.tx_date);
    val[8] = 0;
    strcpy(com_parm_c.val,val);
    ret = Com_parm_Upd_Upd(com_parm_c, g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"UPDATE ERROR !! [%d]",ret);
        WRITEMSG
        return ret;
    }
    Com_parm_Clo_Upd();
    return 0;
}
/**********************************************************************
 * 函 数 名：   pub_eod_stat_seven_gltsub
 * 函数功能：   统计七天周期总帐
 * 作者/时间：  rob/20040317
 *
 * 参数：
 *     输入: 
 *     输出: 
 *     返回: <>0失败
             =0成功
 *
 * 修改历史：
 *
********************************************************************/
int pub_eod_stat_seven_gltsub()
{
    int ret;
    long last_date;
    char val[10];
    
    struct com_parm_c com_parm_c;
    struct gl_mst_hst_c gl_mst_hst_c;
    struct gl_tsub_c gl_tsub_c;

    memset(&com_parm_c, 0x0, sizeof(struct com_parm_c));
    memset(&gl_mst_hst_c, 0x0, sizeof(struct gl_mst_hst_c));

    ret = Com_parm_Sel(g_pub_tx.reply,&com_parm_c,"parm_code='ZQQT' and \
          parm_seqn=1");
    if (ret != 0)
    {
        sprintf(acErrMsg,"查询系统参数表错误!![%d]",ret);
        WRITEMSG
        return ret;
    }
    last_date = atol(com_parm_c.val);

    if (pub_base_CalTrueDays(last_date,g_pub_tx.tx_date) < 7)
    {
        sprintf(acErrMsg,"与上次统计不足七天!!");
        return 0;
    }
	/*
    else if (pub_base_CalTrueDays(last_date,g_pub_tx.tx_date) != 7)
    {
        sprintf(acErrMsg,"七天统计里不可能有间隔超过七天的!!");
        WRITEMSG
        sprintf(acErrMsg,"上次统计日期=[%ld],本次统计日期=[%ld]",
                last_date, g_pub_tx.tx_date);
        WRITEMSG
        return -1;
    }
	*/

    /* 进行七天周期总帐统计 */
    ret = Gl_mst_hst_Dec_Sel(g_pub_tx.reply,"\"date\"=%ld",last_date);
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR SELECT ERROR [%d]",ret);
        WRITEMSG
        return ret;
    }

    while(1)
    {
        ret = Gl_mst_hst_Fet_Sel(&gl_mst_hst_c, g_pub_tx.reply);
        if (ret != 0 && ret != 100)
        {
            sprintf(acErrMsg,"FETCH FOR SELECT ERROR [%d]",ret);
            WRITEMSG
            return ret;
        }
        else if (ret == 100)
        {
            break;
        }

        memset(&gl_tsub_c, 0x0, sizeof(struct gl_tsub_c));
        strcpy(gl_tsub_c.br_no,gl_mst_hst_c.br_no);
        strcpy(gl_tsub_c.cur_no,gl_mst_hst_c.cur_no);
        strcpy(gl_tsub_c.acc_hrt,gl_mst_hst_c.acc_hrt);
        gl_tsub_c.date = g_pub_tx.tx_date;
        strcpy(gl_tsub_c.bal_d,gl_mst_hst_c.dc_ind);
        gl_tsub_c.bt_dr_bal = gl_mst_hst_c.dr_bal;
        gl_tsub_c.bt_cr_bal = gl_mst_hst_c.cr_bal;
        strcpy(gl_tsub_c.term_type,"2");
    
        /* 统计本期借方发生额 */
        ret = sql_sum_double("gl_mst_hst","rdd_amt",&gl_tsub_c.r_dr_amt,
              "\"date\" > last_date and \"date\" <= %ld", g_pub_tx.tx_date);
        if (ret != 0)
        {
            sprintf(acErrMsg,"统计本期借方发生额错误!! [%d]",ret);
            WRITEMSG
            return ret;
        }
  
        /* 统计本期贷方发生额 */
        ret = sql_sum_double("gl_mst_hst","rcd_amt",&gl_tsub_c.r_cr_amt,
              "\"date\" > last_date and \"date\" <= %ld", g_pub_tx.tx_date);
        if (ret != 0)
        {
            sprintf(acErrMsg,"统计本期贷方发生额错误!! [%d]",ret);
            WRITEMSG
            return ret;
        }

        /* 统计本期借方笔数 */
        ret = sql_sum_long("gl_mst_hst","rdd_cnt",&gl_tsub_c.r_dr_cnt,
              "\"date\" > last_date and \"date\" <= %ld", g_pub_tx.tx_date);
        if (ret != 0)
        {
            sprintf(acErrMsg,"统计本期借方笔数错误!! [%d]",ret);
            WRITEMSG
            return ret;
        }
 
        /* 统计本期贷方笔数 */
        ret = sql_sum_long("gl_mst_hst","rcd_cnt",&gl_tsub_c.r_cr_amt,
              "\"date\" > last_date and \"date\" <= %ld", g_pub_tx.tx_date);
        if (ret != 0)
        {
            sprintf(acErrMsg,"统计本期贷方笔数错误!! [%d]",ret);
            WRITEMSG
            return ret;
        }

        ret = Gl_mst_hst_Sel(g_pub_tx.reply,&gl_mst_hst_c,"\"date\"=%ld and \
              acc_hrt='%s'",g_pub_tx.tx_date,gl_mst_hst_c.acc_hrt);
        if (ret != 0)
        {
            sprintf(acErrMsg,"查询历史总帐表错误!! [%d]",ret);
            WRITEMSG
            return ret;
        }
        gl_tsub_c.r_dr_bal = gl_mst_hst_c.dr_bal;
        gl_tsub_c.r_cr_bal = gl_mst_hst_c.cr_bal;

        ret = Gl_tsub_Ins(gl_tsub_c, g_pub_tx.reply);
        if (ret != 0)
        {
            sprintf(acErrMsg,"向周期总帐里插入数据错误!! [%d]",ret);
            WRITEMSG
            return ret;
        }
    }
    Gl_mst_hst_Clo_Sel();

    /* 更新系统参数表中七天周期统计时间 */
    ret = Com_parm_Dec_Upd(g_pub_tx.reply,"parm_code='ZQQT' and parm_seqn=1");
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR!! [%d]",ret);
        WRITEMSG
        return ret;
    }

    ret = Com_parm_Fet_Upd(&com_parm_c,g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"FETCH ERROR !! [%d]",ret);
        WRITEMSG
        return ret;
    }

    sprintf(val,"%lf",g_pub_tx.tx_date);
    val[8] = 0;
    strcpy(com_parm_c.val,val);
    ret = Com_parm_Upd_Upd(com_parm_c, g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"UPDATE ERROR !! [%d]",ret);
        WRITEMSG
        return ret;
    }
    Com_parm_Clo_Upd();
    return 0;
}
/**********************************************************************
 * 函 数 名：   pub_eod_stat_td_gltsub
 * 函数功能：   统计旬周期总帐
 * 作者/时间：  rob/20040317
 *
 * 参数：
 *     输入: 
 *     输出: 
 *     返回: <>0失败
             =0成功
 *
 * 修改历史：
 *
********************************************************************/
int pub_eod_stat_td_gltsub()
{
    int ret;
    long xcrq;
    struct gl_mst_c gl_mst_c;
    struct gl_tsub_c gl_tsub_c;
    
    memset(&gl_mst_c, 0x0, sizeof(struct gl_mst_c));
    memset(&gl_tsub_c, 0x0, sizeof(struct gl_tsub_c));
    
    ret = pub_base_PubXmcl(g_pub_tx.tx_date,&xcrq);
    if (ret != 0)
    {
        sprintf(acErrMsg,"今天不是旬末,不用统计旬周期总帐!![%ld]",
                g_pub_tx.tx_date);
        WRITEMSG
        return 0;
    }
   
    ret= Gl_sub_Dec_Sel(g_pub_tx.reply,"date=%ld",g_pub_tx.tx_date);
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR SELECT ERROR !! [%d]",ret);
        WRITEMSG
        return ret;
    }
    while(1)
    {
        ret = Gl_sub_Fet_Sel(&gl_mst_c, g_pub_tx.reply);
        if (ret != 0 && ret != 100)
        {
            sprintf(acErrMsg,"FETCH FOR SELECT ERROR !! [%d]",ret);
            WRITEMSG
            return ret;
        }
        else if (ret == 100)
        {
            break;
        }
        
        memset(&gl_tsub_c, 0x0, sizeof(struct gl_tsub_c));
        strcpy(gl_tsub_c.br_no,gl_mst_c.br_no);
        strcpy(gl_tsub_c.cur_no,gl_mst_c.cur_no);
        strcpy(gl_tsub_c.acc_hrt,gl_mst_c.acc_hrt);
        gl_tsub_c.date = g_pub_tx.tx_date;
        strcpy(gl_tsub_c.bal_d,gl_mst_c.dc_ind);
        
        gl_tsub_c.bt_dr_bal = gl_mst_c.tddr_bal;
        gl_tsub_c.bt_cr_bal = gl_mst_c.tdcr_bal;
        strcpy(gl_tsub_c.term_type,"3");
        
        gl_tsub_c.r_dr_amt = gl_mst_c.tddr_amt;
        gl_tsub_c.r_cr_amt = gl_mst_c.tdcr_amt;
        gl_tsub_c.r_dr_cnt = gl_mst_c.tddr_cnt;
        gl_tsub_c.r_cr_cnt = gl_mst_c.tdcr_cnt;
        gl_tsub_c.r_dr_bal = gl_mst_c.dr_bal;
        gl_tsub_c.r_cr_bal = gl_mst_c.cr_bal;
        
        ret = Gl_tsub_Ins(gl_tsub_c, g_pub_tx.reply);
        if (ret != 0)
        {
            sprintf(acErrMsg,"向周期总帐中插入旬记录错误!! [%d]",ret);
            WRITEMSG
            return ret;
        }
    }
    Gl_sub_Clo_Sel();
    return 0;
}
/**********************************************************************
 * 函 数 名：   pub_eod_stat_halfmonth_gltsub
 * 函数功能：   统计半月周期总帐
 * 作者/时间：  rob/20040317
 *
 * 参数：
 *     输入: 
 *     输出: 
 *     返回: <>0失败
             =0成功
 *
 * 修改历史：
 *
********************************************************************/
int pub_eod_stat_halfmonth_gltsub()
{
    int ret;
    long year,month,day,bz;
    long last_date;
    char val[10];
    
    struct com_parm_c com_parm_c;
    struct gl_mst_hst_c gl_mst_hst_c;
    struct gl_tsub_c gl_tsub_c;

    memset(&com_parm_c, 0x0, sizeof(struct com_parm_c));
    memset(&gl_mst_hst_c, 0x0, sizeof(struct gl_mst_hst_c));
    memset(&gl_tsub_c, 0x0, sizeof(struct gl_tsub_c));
    
    ret = pub_base_get_ymd(&year,&month,&day,g_pub_tx.tx_date,&bz);
    if (ret != 0)
    {
        sprintf(acErrMsg,"取年月日错误!!");
        WRITEMSG
        return ret;
    }
    
    if (day != 15 && bz != 4 && bz != 6 && bz != 8 && bz != 10)
    {
        sprintf(acErrMsg,"没有到半月末!!");
        WRITEMSG
        return 0;
    }

    ret = Com_parm_Sel(g_pub_tx.reply,&com_parm_c,"parm_code='ZQBY' and \
          parm_seqn=1");
    if (ret != 0)
    {
        sprintf(acErrMsg,"查询系统参数表错误!![%d]",ret);
        WRITEMSG
        return ret;
    }
    last_date = atol(com_parm_c.val);
    
    /* 进行半月周期总帐统计 */
    ret = Gl_mst_hst_Dec_Sel(g_pub_tx.reply,"\"date\"=%ld",last_date);
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR SELECT ERROR [%d]",ret);
        WRITEMSG
        return ret;
    }

    while(1)
    {
        ret = Gl_mst_hst_Fet_Sel(&gl_mst_hst_c, g_pub_tx.reply);
        if (ret != 0 && ret != 100)
        {
            sprintf(acErrMsg,"FETCH FOR SELECT ERROR [%d]",ret);
            WRITEMSG
            return ret;
        }
        else if (ret == 100)
        {
            break;
        }

        memset(&gl_tsub_c, 0x0, sizeof(struct gl_tsub_c));
        strcpy(gl_tsub_c.br_no,gl_mst_hst_c.br_no);
        strcpy(gl_tsub_c.cur_no,gl_mst_hst_c.cur_no);
        strcpy(gl_tsub_c.acc_hrt,gl_mst_hst_c.acc_hrt);
        gl_tsub_c.date = g_pub_tx.tx_date;
        strcpy(gl_tsub_c.bal_d,gl_mst_hst_c.dc_ind);
        gl_tsub_c.bt_dr_bal = gl_mst_hst_c.dr_bal;
        gl_tsub_c.bt_cr_bal = gl_mst_hst_c.cr_bal;
        strcpy(gl_tsub_c.term_type,"4");
    
        /* 统计本期借方发生额 */
        ret = sql_sum_double("gl_mst_hst","rdd_amt",&gl_tsub_c.r_dr_amt,
              "\"date\" > last_date and \"date\" <= %ld", g_pub_tx.tx_date);
        if (ret != 0)
        {
            sprintf(acErrMsg,"统计本期借方发生额错误!! [%d]",ret);
            WRITEMSG
            return ret;
        }
  
        /* 统计本期贷方发生额 */
        ret = sql_sum_double("gl_mst_hst","rcd_amt",&gl_tsub_c.r_cr_amt,
              "\"date\" > last_date and \"date\" <= %ld", g_pub_tx.tx_date);
        if (ret != 0)
        {
            sprintf(acErrMsg,"统计本期贷方发生额错误!! [%d]",ret);
            WRITEMSG
            return ret;
        }

        /* 统计本期借方笔数 */
        ret = sql_sum_long("gl_mst_hst","rdd_cnt",&gl_tsub_c.r_dr_cnt,
              "\"date\" > last_date and \"date\" <= %ld", g_pub_tx.tx_date);
        if (ret != 0)
        {
            sprintf(acErrMsg,"统计本期借方笔数错误!! [%d]",ret);
            WRITEMSG
            return ret;
        }
 
        /* 统计本期贷方笔数 */
        ret = sql_sum_long("gl_mst_hst","rcd_cnt",&gl_tsub_c.r_cr_amt,
              "\"date\" > last_date and \"date\" <= %ld", g_pub_tx.tx_date);
        if (ret != 0)
        {
            sprintf(acErrMsg,"统计本期贷方笔数错误!! [%d]",ret);
            WRITEMSG
            return ret;
        }

        ret = Gl_mst_hst_Sel(g_pub_tx.reply,&gl_mst_hst_c,"\"date\"=%ld and \
              acc_hrt='%s'",g_pub_tx.tx_date,gl_mst_hst_c.acc_hrt);
        if (ret != 0)
        {
            sprintf(acErrMsg,"查询历史总帐表错误!! [%d]",ret);
            WRITEMSG
            return ret;
        }
        gl_tsub_c.r_dr_bal = gl_mst_hst_c.dr_bal;
        gl_tsub_c.r_cr_bal = gl_mst_hst_c.cr_bal;

        ret = Gl_tsub_Ins(gl_tsub_c, g_pub_tx.reply);
        if (ret != 0)
        {
            sprintf(acErrMsg,"向周期总帐里插入数据错误!! [%d]",ret);
            WRITEMSG
            return ret;
        }
    }
    Gl_mst_hst_Clo_Sel();

    /* 更新系统参数表中半月周期统计时间 */
    ret = Com_parm_Dec_Upd(g_pub_tx.reply,"parm_code='ZQBY' and parm_seqn=1");
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR!! [%d]",ret);
        WRITEMSG
        return ret;
    }

    ret = Com_parm_Fet_Upd(&com_parm_c,g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"FETCH ERROR !! [%d]",ret);
        WRITEMSG
        return ret;
    }

    sprintf(val,"%lf",g_pub_tx.tx_date);
    val[8] = 0;
    strcpy(com_parm_c.val,val);
    ret = Com_parm_Upd_Upd(com_parm_c, g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"UPDATE ERROR !! [%d]",ret);
        WRITEMSG
        return ret;
    }
    Com_parm_Clo_Upd();
    return 0;
}
/**********************************************************************
 * 函 数 名：   pub_eod_stat_month_gltsub
 * 函数功能：   统计月周期总帐
 * 作者/时间：  rob/20040317
 *
 * 参数：
 *     输入: 
 *     输出: 
 *     返回: <>0失败
             =0成功
 *
 * 修改历史：
 *
********************************************************************/
int pub_eod_stat_month_gltsub()
{
    int ret;
    long y,m,d,bz;
    struct gl_mst_c gl_mst_c;
    struct gl_tsub_c gl_tsub_c;
    
    memset(&gl_mst_c, 0x0, sizeof(struct gl_mst_c));
    memset(&gl_tsub_c, 0x0, sizeof(struct gl_tsub_c));
    
    ret = pub_base_get_ymd(&y,&m,&d,g_pub_tx.tx_date,&bz);
    if (ret != 0)
    {
        sprintf(acErrMsg,"取年月日错误!!");
        WRITEMSG
        return ret;
    }
    
    if (bz != 4 && bz != 6 && bz != 8 && bz != 10)
    {
        sprintf(acErrMsg,"没有到月末!!");
        WRITEMSG
        return 0;
    }
   
    ret= Gl_sub_Dec_Sel(g_pub_tx.reply,"\"date\"=%ld",g_pub_tx.tx_date);
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR SELECT ERROR !! [%d]",ret);
        WRITEMSG
        return ret;
    }
    while(1)
    {
        ret = Gl_sub_Fet_Sel(&gl_mst_c, g_pub_tx.reply);
        if (ret != 0 && ret != 100)
        {
            sprintf(acErrMsg,"FETCH FOR SELECT ERROR !! [%d]",ret);
            WRITEMSG
            return ret;
        }
        else if (ret == 100)
        {
            break;
        }
        
        memset(&gl_tsub_c, 0x0, sizeof(struct gl_tsub_c));
        strcpy(gl_tsub_c.br_no,gl_mst_c.br_no);
        strcpy(gl_tsub_c.cur_no,gl_mst_c.cur_no);
        strcpy(gl_tsub_c.acc_hrt,gl_mst_c.acc_hrt);
        gl_tsub_c.date = g_pub_tx.tx_date;
        strcpy(gl_tsub_c.bal_d,gl_mst_c.dc_ind);
        
        gl_tsub_c.bt_dr_bal = gl_mst_c.mdr_bal;
        gl_tsub_c.bt_cr_bal = gl_mst_c.mcr_bal;
        strcpy(gl_tsub_c.term_type,"5");
        
        gl_tsub_c.r_dr_amt = gl_mst_c.mdr_amt;
        gl_tsub_c.r_cr_amt = gl_mst_c.mcr_amt;
        gl_tsub_c.r_dr_cnt = gl_mst_c.mdr_cnt;
        gl_tsub_c.r_cr_cnt = gl_mst_c.mcr_cnt;
        gl_tsub_c.r_dr_bal = gl_mst_c.dr_bal;
        gl_tsub_c.r_cr_bal = gl_mst_c.cr_bal;
        
        ret = Gl_tsub_Ins(gl_tsub_c, g_pub_tx.reply);
        if (ret != 0)
        {
            sprintf(acErrMsg,"向周期总帐中插入月记录错误!! [%d]",ret);
            WRITEMSG
            return ret;
        }
    }
    Gl_sub_Clo_Sel();
    return 0;
}
/**********************************************************************
 * 函 数 名：   pub_eod_stat_season_gltsub
 * 函数功能：   统计季周期总帐
 * 作者/时间：  rob/20040317
 *
 * 参数：
 *     输入: 
 *     输出: 
 *     返回: <>0失败
             =0成功
 *
 * 修改历史：
 *
********************************************************************/
int pub_eod_stat_season_gltsub()
{
    int ret;
    long year,month,day,bz;
    struct gl_mst_c gl_mst_c;
    struct gl_tsub_c gl_tsub_c;
    
    memset(&gl_mst_c, 0x0, sizeof(struct gl_mst_c));
    memset(&gl_tsub_c, 0x0, sizeof(struct gl_tsub_c));
    
    ret = pub_base_get_ymd(&year,&month,&day,g_pub_tx.tx_date,&bz);
    if (ret != 0)
    {
        sprintf(acErrMsg,"取年月日错误!!");
        WRITEMSG
        return ret;
    }
    
    if (bz != 6 && bz != 8 && bz != 10)
    {
        sprintf(acErrMsg,"没有到季末!!");
        WRITEMSG
        return 0;
    }
   
    ret= Gl_sub_Dec_Sel(g_pub_tx.reply,"\"date\"=%ld",g_pub_tx.tx_date);
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR SELECT ERROR !! [%d]",ret);
        WRITEMSG
        return ret;
    }
    while(1)
    {
        ret = Gl_sub_Fet_Sel(&gl_mst_c, g_pub_tx.reply);
        if (ret != 0 && ret != 100)
        {
            sprintf(acErrMsg,"FETCH FOR SELECT ERROR !! [%d]",ret);
            WRITEMSG
            return ret;
        }
        else if (ret == 100)
        {
            break;
        }
        
        memset(&gl_tsub_c, 0x0, sizeof(struct gl_tsub_c));
        strcpy(gl_tsub_c.br_no,gl_mst_c.br_no);
        strcpy(gl_tsub_c.cur_no,gl_mst_c.cur_no);
        strcpy(gl_tsub_c.acc_hrt,gl_mst_c.acc_hrt);
        gl_tsub_c.date = g_pub_tx.tx_date;
        strcpy(gl_tsub_c.bal_d,gl_mst_c.dc_ind);
        
        gl_tsub_c.bt_dr_bal = gl_mst_c.qdr_bal;
        gl_tsub_c.bt_cr_bal = gl_mst_c.qcr_bal;
        strcpy(gl_tsub_c.term_type,"6");
        
        gl_tsub_c.r_dr_amt = gl_mst_c.qdr_amt;
        gl_tsub_c.r_cr_amt = gl_mst_c.qcr_amt;
        gl_tsub_c.r_dr_cnt = gl_mst_c.qdr_cnt;
        gl_tsub_c.r_cr_cnt = gl_mst_c.qcr_cnt;
        gl_tsub_c.r_dr_bal = gl_mst_c.dr_bal;
        gl_tsub_c.r_cr_bal = gl_mst_c.cr_bal;
        
        ret = Gl_tsub_Ins(gl_tsub_c, g_pub_tx.reply);
        if (ret != 0)
        {
            sprintf(acErrMsg,"向周期总帐中插入季记录错误!! [%d]",ret);
            WRITEMSG
            return ret;
        }
    }
    Gl_sub_Clo_Sel();
    return 0;
}
/**********************************************************************
 * 函 数 名：   pub_eod_stat_halfyear_gltsub
 * 函数功能：   统计半年周期总帐
 * 作者/时间：  rob/20040317
 *
 * 参数：
 *     输入: 
 *     输出: 
 *     返回: <>0失败
             =0成功
 *
 * 修改历史：
 *
********************************************************************/
int pub_eod_stat_halfyear_gltsub()
{
    int ret;
    long year,month,day,bz;
    long half_date;
    struct gl_mst_c gl_mst_c;
    struct gl_tsub_c gl_tsub_c,gl_tsub;
    
    memset(&gl_mst_c, 0x0, sizeof(struct gl_mst_c));
    memset(&gl_tsub_c, 0x0, sizeof(struct gl_tsub_c));
    memset(&gl_tsub, 0x0, sizeof(struct gl_tsub_c));
    
    ret = pub_base_get_ymd(&year,&month,&day,g_pub_tx.tx_date,&bz);
    if (ret != 0)
    {
        sprintf(acErrMsg,"取年月日错误!!");
        WRITEMSG
        return ret;
    }
    
    if (bz != 8 && bz != 10)
    {
        sprintf(acErrMsg,"没有到半年末!!");
        WRITEMSG
        return 0;
    }
    
    ret= Gl_sub_Dec_Sel(g_pub_tx.reply,"\"date\"=%ld",g_pub_tx.tx_date);
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR SELECT ERROR !! [%d]",ret);
        WRITEMSG
        return ret;
    }
    while(1)
    {
        ret = Gl_sub_Fet_Sel(&gl_mst_c, g_pub_tx.reply);
        if (ret != 0 && ret != 100)
        {
            sprintf(acErrMsg,"FETCH FOR SELECT ERROR !! [%d]",ret);
            WRITEMSG
            return ret;
        }
        else if (ret == 100)
        {
            break;
        }
        
        memset(&gl_tsub_c, 0x0, sizeof(struct gl_tsub_c));
        strcpy(gl_tsub_c.br_no,gl_mst_c.br_no);
        strcpy(gl_tsub_c.cur_no,gl_mst_c.cur_no);
        strcpy(gl_tsub_c.acc_hrt,gl_mst_c.acc_hrt);
        gl_tsub_c.date = g_pub_tx.tx_date;
        strcpy(gl_tsub_c.bal_d,gl_mst_c.dc_ind);
        
        if (bz == 8)
        {
            /* 半年末 */
            gl_tsub_c.bt_dr_bal = gl_mst_c.ydr_bal;
            gl_tsub_c.bt_cr_bal = gl_mst_c.ycr_bal;
            strcpy(gl_tsub_c.term_type,"7");
        
            gl_tsub_c.r_dr_amt = gl_mst_c.ydr_amt;
            gl_tsub_c.r_cr_amt = gl_mst_c.ycr_amt;
            gl_tsub_c.r_dr_cnt = gl_mst_c.ydr_cnt;
            gl_tsub_c.r_cr_cnt = gl_mst_c.ycr_cnt;
            gl_tsub_c.r_dr_bal = gl_mst_c.dr_bal;
            gl_tsub_c.r_cr_bal = gl_mst_c.cr_bal;
        }
        if (bz == 10)
        {
            /* 年末 */ 
            half_date = (g_pub_tx.tx_date / 10000 ) * 10000 + 630;
            ret = Gl_tsub_Sel(g_pub_tx.reply,&gl_tsub,"\"date\"=%ld and \
                  br_no = '%s' and acc_hrt ='%s'",half_date,gl_tsub_c.br_no,
                  gl_tsub_c.acc_hrt);
            if (ret == 100)
			{
				memset( &gl_tsub,0,sizeof(gl_tsub) );
			}
            else if (ret != 0)
            {
                sprintf(acErrMsg,"查询630号周期总帐错误!! [%d]",ret);
                WRITEMSG
                return ret;
            }
            gl_tsub_c.bt_dr_bal = gl_tsub.r_dr_bal;
            gl_tsub_c.bt_cr_bal = gl_tsub.r_cr_bal;
            strcpy(gl_tsub_c.term_type,"7");
        
            gl_tsub_c.r_dr_amt = gl_mst_c.ydr_amt - gl_tsub.r_dr_amt;
            gl_tsub_c.r_cr_amt = gl_mst_c.ycr_amt - gl_tsub.r_cr_amt;
            gl_tsub_c.r_dr_cnt = gl_mst_c.ydr_cnt - gl_tsub.r_dr_cnt;
            gl_tsub_c.r_cr_cnt = gl_mst_c.ycr_cnt - gl_tsub.r_cr_cnt;
            gl_tsub_c.r_dr_bal = gl_mst_c.dr_bal;
            gl_tsub_c.r_cr_bal = gl_mst_c.cr_bal;
        }
        
        ret = Gl_tsub_Ins(gl_tsub_c, g_pub_tx.reply);
        if (ret != 0)
        {
            sprintf(acErrMsg,"向周期总帐中插入年记录错误!! [%d]",ret);
            WRITEMSG
            return ret;
        }
    }
    Gl_sub_Clo_Sel();
    return 0;
}
/**********************************************************************
 * 函 数 名：   pub_eod_stat_year_gltsub
 * 函数功能：   统计年周期总帐
 * 作者/时间：  rob/20040317
 *
 * 参数：
 *     输入: 
 *     输出: 
 *     返回: <>0失败
             =0成功
 *
 * 修改历史：
 *
********************************************************************/
int pub_eod_stat_year_gltsub()
{
    int ret;
    long year,month,day,bz;
    struct gl_mst_c gl_mst_c;
    struct gl_tsub_c gl_tsub_c;
    
    memset(&gl_mst_c, 0x0, sizeof(struct gl_mst_c));
    memset(&gl_tsub_c, 0x0, sizeof(struct gl_tsub_c));
    
    ret = pub_base_get_ymd(&year,&month,&day,g_pub_tx.tx_date,&bz);
    if (ret != 0)
    {
        sprintf(acErrMsg,"取年月日错误!!");
        WRITEMSG
        return ret;
    }
    
    if (bz != 10)
    {
        sprintf(acErrMsg,"没有到年末!!");
        WRITEMSG
        return 0;
    }
 
    ret= Gl_sub_Dec_Sel(g_pub_tx.reply,"\"date\"=%ld",g_pub_tx.tx_date);
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR SELECT ERROR !! [%d]",ret);
        WRITEMSG
        return ret;
    }
    while(1)
    {
        ret = Gl_sub_Fet_Sel(&gl_mst_c, g_pub_tx.reply);
        if (ret != 0 && ret != 100)
        {
            sprintf(acErrMsg,"FETCH FOR SELECT ERROR !! [%d]",ret);
            WRITEMSG
            return ret;
        }
        else if (ret == 100)
        {
            break;
        }
        
        memset(&gl_tsub_c, 0x0, sizeof(struct gl_tsub_c));
        strcpy(gl_tsub_c.br_no,gl_mst_c.br_no);
        strcpy(gl_tsub_c.cur_no,gl_mst_c.cur_no);
        strcpy(gl_tsub_c.acc_hrt,gl_mst_c.acc_hrt);
        gl_tsub_c.date = g_pub_tx.tx_date;
        strcpy(gl_tsub_c.bal_d,gl_mst_c.dc_ind);
        
        gl_tsub_c.bt_dr_bal = gl_mst_c.ydr_bal;
        gl_tsub_c.bt_cr_bal = gl_mst_c.ycr_bal;
        strcpy(gl_tsub_c.term_type,"8");
        
        gl_tsub_c.r_dr_amt = gl_mst_c.ydr_amt;
        gl_tsub_c.r_cr_amt = gl_mst_c.ycr_amt;
        gl_tsub_c.r_dr_cnt = gl_mst_c.ydr_cnt;
        gl_tsub_c.r_cr_cnt = gl_mst_c.ycr_cnt;
        gl_tsub_c.r_dr_bal = gl_mst_c.dr_bal;
        gl_tsub_c.r_cr_bal = gl_mst_c.cr_bal;
        
        ret = Gl_tsub_Ins(gl_tsub_c, g_pub_tx.reply);
        if (ret != 0)
        {
            sprintf(acErrMsg,"向周期总帐中插入年记录错误!! [%d]",ret);
            WRITEMSG
            return ret;
        }
    }
    Gl_sub_Clo_Sel();
    return 0;
}
