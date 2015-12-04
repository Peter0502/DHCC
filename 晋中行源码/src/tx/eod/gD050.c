/*************************************************************
* 文 件 名: gD050.c
* 功能描述：余额变动较大情况统计表
*
* 作    者: lance
* 完成日期: 2003年3月17日
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
#include "com_sys_parm_c.h"
#include "dd_mst_hst_c.h"
#include "td_mst_hst_c.h"
#include "com_parm_c.h"
#include "mo_chg_bal_c.h"


gD050()
{
    int ret = 0;
    struct com_sys_parm_c com_sys_parm_c;

    memset(&com_sys_parm_c, 0x0, sizeof(struct com_sys_parm_c));

    ret=sql_begin(); /*打开事务*/
    if(ret != 0)
    {
        sprintf( acErrMsg, "打开事务失败!!!" );
        WRITEMSG
        goto ErrExit;
    }

    /**------- 初始化全局变量 --------**/
    pub_base_sysinit();
   
    /* 查询系统参数表 */
    ret = Com_sys_parm_Sel(g_pub_tx.reply,&com_sys_parm_c,"1=1");
    if (ret != 0)
    {
        sprintf(acErrMsg,"查询系统参数表错误!! [%d]",ret);
        WRITEMSG
        return ret;
    }
    g_pub_tx.tx_date = com_sys_parm_c.lst_date; /* 翻盘后处理 */

    ret = pub_eod_ins_mochgbal();
    if (ret != 0)
    {
        sprintf(acErrMsg,"登记余额变动较大情况统计表错误!!");
        WRITEMSG
        goto ErrExit;
    }

OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"登记余额变动较大情况统计表成功!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
    if (strcmp(g_pub_tx.reply,"0000") == 0 || g_pub_tx.reply[0]=='\0')
    {
        strcpy(g_pub_tx.reply,"G009");
    }
	sql_rollback();	/*--事务回滚--*/
	sprintf(acErrMsg,"登记余额变动较大情况统计表失败!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

/**********************************************************************
 * 函 数 名：   pub_eod_ins_mochgbal
 * 函数功能：   登记余额变动较大情况统计表
 * 作者/时间：  lance/20030317
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
int pub_eod_ins_mochgbal()
{
    int ret;

    ret = pub_eod_stat_ddmstchgbal();
    if (ret != 0)
    {
        sprintf(acErrMsg,"统计活期帐户主文件的余额变化错误!!");
        WRITEMSG
        return ret;
    }

    ret = pub_eod_stat_tdmstchgbal();
    if (ret != 0)
    {
        sprintf(acErrMsg,"统计定期帐户主文件的余额变化错误!!");
        WRITEMSG
        return ret;
    }
    return 0;
 }
/**********************************************************************
 * 函 数 名：   pub_eod_stat_ddmstchgbal
 * 函数功能：   统计活期帐户主文件的余额变化
 * 作者/时间：  lance/20030317
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
int pub_eod_stat_ddmstchgbal()
{
    int ret;
    double max_amt;
    double chg_bal;

    struct dd_mst_c dd_mst_c;
    struct com_parm_c com_parm_c;
    struct mo_chg_bal_c mo_chg_bal_c;

    memset(&dd_mst_c, 0x0, sizeof(struct dd_mst_c));
    memset(&com_parm_c, 0x0, sizeof(struct com_parm_c));

    ret = Com_parm_Sel(g_pub_tx.reply,&com_parm_c,"parm_code='DEQX' and \
          parm_seqn=1");
    if (ret != 0)
    {
        sprintf(acErrMsg,"查询公共参数表表错误!! [%d]",ret);
        WRITEMSG
        return ret;
    }
    
    max_amt = atof(com_parm_c.val);
    sprintf(acErrMsg,"余额变化的参数为：[%lf]",max_amt);
    WRITEMSG
    
    ret = Dd_mst_Dec_Sel(g_pub_tx.reply,"1=1");
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR SELECT ERROR!! [%d]",ret);
        WRITEMSG
        return ret;
    }
    
    while(1)
    {
        ret = Dd_mst_Fet_Sel(&dd_mst_c, g_pub_tx.reply);
        if (ret != 0 && ret != 100)
        {
            sprintf(acErrMsg,"FETCH FOR SELECT ERROR!! [%d]",ret);
            WRITEMSG
            return ret;
        }
        else if (ret == 100)
        {
            break;
        }

        chg_bal = dd_mst_c.bal - dd_mst_c.ys_bal;
        memset(&mo_chg_bal_c, 0x0, sizeof(struct mo_chg_bal_c));

        if (pub_base_CompDblVal(chg_bal,0.00) < 0 && \
            pub_base_CompDblVal(-chg_bal,max_amt) >= 0)
        {
            strcpy(mo_chg_bal_c.br_no, dd_mst_c.opn_br_no);
            mo_chg_bal_c.ac_id = dd_mst_c.ac_id;
            mo_chg_bal_c.tx_date = g_pub_tx.tx_date;
            mo_chg_bal_c.ac_seqn = dd_mst_c.ac_seqn;
            mo_chg_bal_c.y_bal = dd_mst_c.ys_bal;
            mo_chg_bal_c.by_bal = dd_mst_c.bal;
            mo_chg_bal_c.chg_bal = chg_bal;

            ret = Mo_chg_bal_Ins(mo_chg_bal_c, g_pub_tx.reply);
            if (ret != 0)
            {
                sprintf(acErrMsg,"向余额变动较大情况统计表中插入数据错误[%d]",
                        ret);
                WRITEMSG
                return ret;
            }
        }

        if (pub_base_CompDblVal(chg_bal,0.00) > 0 && \
            pub_base_CompDblVal(chg_bal,max_amt) >= 0)
        {
            strcpy(mo_chg_bal_c.br_no, dd_mst_c.opn_br_no);
            mo_chg_bal_c.ac_id = dd_mst_c.ac_id;
            mo_chg_bal_c.tx_date = g_pub_tx.tx_date;
            mo_chg_bal_c.ac_seqn = dd_mst_c.ac_seqn;
            mo_chg_bal_c.y_bal = dd_mst_c.ys_bal;
            mo_chg_bal_c.by_bal = dd_mst_c.bal;
            mo_chg_bal_c.chg_bal = chg_bal;
         
            ret = Mo_chg_bal_Ins(mo_chg_bal_c, g_pub_tx.reply);
            if (ret != 0)
            {
                sprintf(acErrMsg,"向余额变动较大情况统计表中插入数据错误[%d]",
                        ret);
                WRITEMSG
                return ret;
            }
        }
    }
    Dd_mst_Clo_Sel();

    return 0;
}
/**********************************************************************
 * 函 数 名：   pub_eod_stat_tdmstchgbal
 * 函数功能：   统计定期帐户主文件的余额变化
 * 作者/时间：  lance/20030317
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
int pub_eod_stat_tdmstchgbal()
{
    int ret;
    double max_amt;
    double chg_bal;

    struct td_mst_c td_mst_c;
    struct com_parm_c com_parm_c;
    struct mo_chg_bal_c mo_chg_bal_c;

    memset(&td_mst_c, 0x0, sizeof(struct td_mst_c));
    memset(&com_parm_c, 0x0, sizeof(struct com_parm_c));

    ret = Com_parm_Sel(g_pub_tx.reply,&com_parm_c,"parm_code='DEQX' and \
          parm_seqn=1");
    if (ret != 0)
    {
        sprintf(acErrMsg,"查询公共参数表表错误!! [%d]",ret);
        WRITEMSG
        return ret;
    }
    
    max_amt = atof(com_parm_c.val);
    sprintf(acErrMsg,"余额变化的参数为：[%lf]",max_amt);
    WRITEMSG
    
    ret = Td_mst_Dec_Sel(g_pub_tx.reply,"1=1");
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR SELECT ERROR!! [%d]",ret);
        WRITEMSG
        return ret;
    }
    
    while(1)
    {
        ret = Td_mst_Fet_Sel(&td_mst_c, g_pub_tx.reply);
        if (ret != 0 && ret != 100)
        {
            sprintf(acErrMsg,"FETCH FOR SELECT ERROR!! [%d]",ret);
            WRITEMSG
            return ret;
        }
        else if (ret == 100)
        {
            break;
        }

        chg_bal = td_mst_c.bal - td_mst_c.ys_bal;
        memset(&mo_chg_bal_c, 0x0, sizeof(struct mo_chg_bal_c));

        if (pub_base_CompDblVal(chg_bal,0.00) < 0 && \
            pub_base_CompDblVal(-chg_bal,max_amt) >= 0)
        {
            strcpy(mo_chg_bal_c.br_no, td_mst_c.opn_br_no);
            mo_chg_bal_c.ac_id = td_mst_c.ac_id;
            mo_chg_bal_c.tx_date = g_pub_tx.tx_date;
            mo_chg_bal_c.ac_seqn = td_mst_c.ac_seqn;
            mo_chg_bal_c.y_bal = td_mst_c.ys_bal;
            mo_chg_bal_c.by_bal = td_mst_c.bal;
            mo_chg_bal_c.chg_bal = chg_bal;

            ret = Mo_chg_bal_Ins(mo_chg_bal_c, g_pub_tx.reply);
            if (ret != 0)
            {
                sprintf(acErrMsg,"向余额变动较大情况统计表中插入数据错误[%d]",
                        ret);
                WRITEMSG
                return ret;
            }
        }

        if (pub_base_CompDblVal(chg_bal,0.00) > 0 && \
            pub_base_CompDblVal(chg_bal,max_amt) >= 0)
        {
            strcpy(mo_chg_bal_c.br_no, td_mst_c.opn_br_no);
            mo_chg_bal_c.ac_id = td_mst_c.ac_id;
            mo_chg_bal_c.tx_date = g_pub_tx.tx_date;
            mo_chg_bal_c.ac_seqn = td_mst_c.ac_seqn;
            mo_chg_bal_c.y_bal = td_mst_c.ys_bal;
            mo_chg_bal_c.by_bal = td_mst_c.bal;
            mo_chg_bal_c.chg_bal = chg_bal;
         
            ret = Mo_chg_bal_Ins(mo_chg_bal_c, g_pub_tx.reply);
            if (ret != 0)
            {
                sprintf(acErrMsg,"向余额变动较大情况统计表中插入数据错误[%d]",
                        ret);
                WRITEMSG
                return ret;
            }
        }
    }
    Td_mst_Clo_Sel();

    return 0;
}
