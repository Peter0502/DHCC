/*************************************************************
* 文 件 名: spH004.c
* 功能描述: 人工确定理财募集成功，等待日终扣款
*
* 作    者: syc
* 完成日期: 2015/10/26 10:14:05
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
int spH004()
{
    int ret = 0;
    char cClos_time[7];
    char cPrdt_no[21];   /**产品编号**/
    struct fn_base_inf_c sFn_base_inf;

    memset(cClos_time ,0x00 ,sizeof(cClos_time));
    memset(cPrdt_no ,0x00 ,sizeof(cPrdt_no));
    memset(&sFn_base_inf,0x00 ,sizeof(struct fn_base_inf_c));
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
    /*判断这个产品是否手工处理的*/
    if(sFn_base_inf.auto_flag[0] != '2')
    {
        vtcp_exit( "该理财产品为自动处理，不能人工处理!");
        strcpy(g_pub_env.reply, "D334");
        goto ErrExit;
    }
    /*只有募集状态可宣布成立*/
    if(sFn_base_inf.prdt_sts[0] != '0')
    {
        vtcp_exit( "理财产品状态 不能宣布成立! prdt_sts=[%s]", sFn_base_inf.prdt_sts);
        strcpy(g_pub_env.reply, "D334");
        goto ErrExit;
    }
    /*募集的最后一天必须等营业时间结束之后才能做成功处理防止有人在处理时认购*/
    memcpy(cClos_time,get_prdt_cparm(cPrdt_no,"bclose_time"),sizeof(cClos_time)-1);
    if(g_pub_env.tx_time < atol(cClos_time))
    {
        vtcp_exit( "募集期的产品必须在产品营业时间结束后做失败处理=[%s]",cClos_time);
        set_zd_data(DC_GET_UMSG,"募集状态的产品必须在产品营业时间结束后做失败处理");
        strcpy(g_pub_env.reply, "S035");
        goto ErrExit;
    }

    strcpy(sFn_base_inf.prdt_sts,"5");/*宣布产品成立*/
    ret = Fn_base_inf_Upd_Upd(sFn_base_inf,g_pub_env.reply);
    if (ret)
    {
        vtcp_exit("更新理财产品参数表失败!");
        strcpy(g_pub_env.reply, "D325");
        goto ErrExit;
    }
    Fn_base_inf_Clo_Upd();
    strcpy(g_pub_tx.ac_no,cPrdt_no);
    strcpy(g_pub_tx.brf,"理财产品宣布成立");
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


