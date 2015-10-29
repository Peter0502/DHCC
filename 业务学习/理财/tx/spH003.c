/*************************************************************
* 文 件 名: spH003.c
* 功能描述: 产品确定募集失败，等待日终归还客户本金和活期利息
*
* 作    者: syc
* 完成日期: 2015/10/13 11:29:57
*
* 修改记录:
* 日    期:
* 修 改 人:
* 备    注:
**************************************************************/
#define EXTERN
#include "public.h"
#include "de_public.h"
#include "fn_base_inf_c.h"
#include "fn_appbuy_reg_c.h"
#include "mdm_ac_rel_c.h"

/*外部函数声明*/
extern int pub_num_amtcmp(double amt1,double amt2);
extern int pub_ins_trace_log();
int spH003()
{
    int ret = 0;
    char cClos_time[7];
    char cPrdt_no[21];   /**产品编号**/
    struct mdm_ac_rel_c sMdm_ac_rel;
    struct dd_mst_c sDd_mst;
    struct fn_base_inf_c sFn_base_inf;
    struct fn_appbuy_reg_c sFn_appbuy_reg;

    memset(cClos_time ,0x00 ,sizeof(cClos_time));
    memset(cPrdt_no ,0x00 ,sizeof(cPrdt_no));
    memset(&sMdm_ac_rel,0x00 ,sizeof(struct mdm_ac_rel_c));
    memset(&sDd_mst,0x00 ,sizeof(struct dd_mst_c));
    memset(&sFn_base_inf,0x00 ,sizeof(struct fn_base_inf_c));
    memset(&sFn_appbuy_reg,0x00,sizeof(sFn_appbuy_reg));
    /** 初始化全局变量 **/
    pub_base_sysinit();

    /**得到交易机构,产品代码**/

    get_zd_data("0230", cPrdt_no);
    vtcp_trace("prdt_code=[%s]", cPrdt_no);

    ret = Fn_base_inf_Dec_Upd(g_pub_env.reply, " prdt_no ='%s' ", cPrdt_no);
    if(ret)
    {
        vtcp_exit("定义fn_base_inf游标失败! ret=[%d]", ret);
        goto ErrExit;
    }
    ret = Fn_base_inf_Fet_Upd(&sFn_base_inf, g_pub_env.reply);
    if(ret == DB_NOTFOUND)
    {
        vtcp_exit( "没有这个产品 ret=[%d]", ret);
        goto ErrExit;
    }
    else if(ret)
    {
        vtcp_exit( "打开fn_base_inf游标失败! ret=[%d]", ret);
        goto ErrExit;
    }
    /*只有募集状态和暂停发售的 能做募集失败*/
    if(sFn_base_inf.prdt_sts[0] != '0' && sFn_base_inf.prdt_sts[0] != '4')
    {
        vtcp_exit( "理财产品状态 不能做募集失败! prdt_sts=[%s]", sFn_base_inf.prdt_sts);
        strcpy(g_pub_env.reply, "D334");
        goto ErrExit;
    }
    /*募集的最后一天必须等营业时间结束之后才能做失败处理
    防止有人在处理时认购*/
    memcpy(cClos_time,get_prdt_cparm(cPrdt_no,"bclose_time"),sizeof(cClos_time)-1);
    if(sFn_base_inf.prdt_sts[0] == '0' && g_pub_env.tx_time < atol(cClos_time))
    {
        vtcp_exit( "募集期的产品必须在产品营业时间结束后做失败处理=[%s]",cClos_time);
        set_zd_data(DC_GET_UMSG,"募集状态的产品必须在产品营业时间结束后做失败处理");
        strcpy(g_pub_env.reply, "S035");
        goto ErrExit;
    }
    strcpy(sFn_base_inf.prdt_sts,"8");/*等待日终归还客户*/
    ret = Fn_base_inf_Upd_Upd(sFn_base_inf,g_pub_env.reply);
    if (ret)
    {
        vtcp_exit("更新理财产品参数表失败!");
        strcpy(g_pub_env.reply, "D325");
        goto ErrExit;
    }
    Fn_base_inf_Clo_Upd();
    strcpy(g_pub_tx.ac_no,cPrdt_no);
    strcpy(g_pub_tx.brf,"理财产品募集失败处理");
    strcpy(g_pub_tx.acc_ind,"0");
    if( pub_ins_trace_log() )
    {
        vtcp_exit("登记交易流水出错");
        strcpy(g_pub_env.reply,"P431");
        goto ErrExit;
    }
    strcpy(g_pub_env.reply, "0000");
    vtcp_exit( "处理成功![%s]", g_pub_env.reply);
    set_zd_data("0120", g_pub_env.reply);
    return 0;

ErrExit:
    if(strlen(g_pub_env.reply) == 0 || strcmp(g_pub_env.reply,"0000") == 0)
    {
        strcpy(g_pub_env.reply,"S035");
    }
    vtcp_exit("处理失败![%s]", g_pub_env.reply);
    set_zd_data("0120", g_pub_env.reply);
    return 1;
}