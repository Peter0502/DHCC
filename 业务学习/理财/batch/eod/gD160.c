/*************************************************************
* 文 件 名: gD160.c
* 功能描述：日终理财募集开始，将预约记录改为认购状态
* 作	者: syc
* 完成日期: 2015/10/13 9:17:30
* 修改记录:
* 日	期:
* 修 改 人:
* 修改内容:
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "fn_base_inf_c.h"
#include "fn_appbuy_reg_c.h"
#include "com_sys_parm_c.h"
#include "dd_mst_c.h"
/*外部函数声明*/
extern int pub_acct_trance_de();
extern int pub_de_get_FnAcct_parm(char *cPrdt_no,char *dc_rel_code,char *cif_type,char *opn_br_no,char *fnqs_br_no,char *cAcct_prdt_no,char *cAc_no,char *br_type);
extern int sql_begin(void);
extern int sql_commit(void);
extern int sql_rollback(void);
extern int pub_num_amtcmp(double amt1,double amt2);
extern int pub_de_call_ddBal(struct dd_mst_c dd_mst_tmp,int type,double *bal);
extern int pub_ins_trace_log();
extern int pub_base_PubQlsh(long *zjlsh, char *xym);
/*内部函数声明*/
static int _deal_appbuy_reg(char *prdt_no);
int gD160()
{
    int ret=0;
    struct fn_base_inf_c sFn_base_inf;
    struct com_sys_parm_c sCom_sys_parm;
    memset(&sFn_base_inf,0x00,sizeof(sFn_base_inf));
    memset(&sCom_sys_parm,0x00,sizeof(sCom_sys_parm));

    /**------- 初始化全局变量 --------**/
    pub_base_sysinit();

    ret = sql_begin();	/* 打开事务 */
    if (ret)
    {
        strcpy(g_pub_env.reply,"S072");
        vtcp_exit( "打开事务失败,ret=[%d]!",ret);
        goto ErrExit;
    }

    ret = Com_sys_parm_Sel(g_pub_env.reply,&sCom_sys_parm,"1=1");
    if(ret)
    {
        vtcp_exit( "获取系统日期错误,ret=[%d]!",ret);
        goto ErrExit;
    }
    vtcp_trace("tx_date[%ld]",sCom_sys_parm.sys_date);
    /*循环取产品*/
    ret = Fn_base_inf_Dec_Upd(g_pub_env.reply,"coll_beg_date=%ld and prdt_sts='6'",sCom_sys_parm.sys_date);
    if(ret)
    {
        vtcp_exit( "定义fn_base_inf查询游标失败,ret=[%d]!",ret);
        goto ErrExit;
    }
    while(1)
    {
        memset(&sFn_base_inf,0x00,sizeof(sFn_base_inf));
        ret = Fn_base_inf_Fet_Upd(&sFn_base_inf,g_pub_env.reply);
        if(ret != DB_OK && ret != DB_NOTFOUND)
        {
            vtcp_exit( "定义fn_base_inf查询游标失败,ret=[%d]!",ret);
            goto ErrExit;
        }
        else if(ret == DB_NOTFOUND)
        {
            vtcp_trace("所有的产品都处理完了");
            break;
        }
        /*一个产品取一次流水*/
        ret = pub_base_PubQlsh(&g_pub_env.trace_no,g_pub_env.reply);
        if (ret)
        {
            vtcp_exit("取主机流水号错 [%ld]", g_pub_env.trace_no);
            goto ErrExit;
        }
        /*处理某个产品*/
        ret = _deal_appbuy_reg(sFn_base_inf.prdt_no);
        if(ret)
        {
            vtcp_exit( "处理失败,ret=[%d]!",ret);
            goto ErrExit;
        }
        strcpy(sFn_base_inf.prdt_sts,"0");
        ret = Fn_base_inf_Upd_Upd(sFn_base_inf,g_pub_env.reply);
        if(ret)
        {
            vtcp_exit( "更新产品状态错误,ret=[%d]!",ret);
            goto ErrExit;
        }
    }
    Fn_base_inf_Clo_Upd();

    sql_commit();/*--提交事务--*/
    strcpy(g_pub_env.reply, "0000");
    vtcp_exit("理财预约转认购成功![%s]", g_pub_env.reply);
    set_zd_data(DC_REPLY, g_pub_env.reply);
    return 0;
ErrExit:
    if(strlen(g_pub_env.reply) == 0 || strcmp(g_pub_env.reply,"0000") == 0)
    {
        strcpy(g_pub_env.reply,"S035");
    }
    sql_rollback();		/*--事务回滚--*/
    vtcp_exit("理财预约转认购失败![%s]", g_pub_env.reply);
    set_zd_data(DC_REPLY, g_pub_env.reply);
    return 1;
}
/*根据系统日期 找当天要募集开始的产品 处理预约记录*/
static int _deal_appbuy_reg(char *prdt_no)
{
    int ret = 0;
    int iFail_flag  = 0;/*预约转失败标志 0成功 1失败*/
    char cDc_prdt_no[21];/*会计产品*/
    char cDc_ac_no[33];/*募集账号*/
    char cBr_type[2];/*机构类型*/
    double dAvbal = 0.00;/*活期户可用余额*/
    struct fn_appbuy_reg_c sFn_appbuy_reg;
    struct dd_mst_c sDd_mst;
    struct mdm_ac_rel_c sMdm_ac_rel;

    memset(cDc_prdt_no,0x00,sizeof(cDc_prdt_no));
    memset(cDc_ac_no,0x00,sizeof(cDc_ac_no));
    memset(cBr_type,0x00,sizeof(cBr_type));
    memset(&sFn_appbuy_reg,0x00,sizeof(sFn_appbuy_reg));
    memset(&sDd_mst,0x00,sizeof(sDd_mst));
    memset(&sMdm_ac_rel,0x00,sizeof(sMdm_ac_rel));

    ret = Fn_appbuy_reg_Dec_Upd(g_pub_env.reply,"prdt_no='%s' and app_knd='1' \
        and sts='0' order reg_date,reg_trace",prdt_no);
    if(ret)
    {
        vtcp_exit("定义购买登记簿游标失败[%d]",ret);
        goto ErrExit;
    }
    while(1)
    {
        iFail_flag = 0;
        dAvbal = 0.00;
        memset(&sFn_appbuy_reg,0x00,sizeof(sFn_appbuy_reg));
        memset(&sDd_mst,0x00,sizeof(sDd_mst));
        memset(&sMdm_ac_rel,0x00,sizeof(sMdm_ac_rel));

        ret = Fn_appbuy_reg_Fet_Upd(&sFn_appbuy_reg,g_pub_env.reply);
        if(ret == DB_NOTFOUND)
        {
            vtcp_exit("产品[%s]处理完了",prdt_no);
            break;
        }
        else if(ret)
        {
            vtcp_exit("获取fn_appbuy_reg游标失败了[%d]",ret);
            goto ErrExit;
        }
        /*查询出账号*/
        ret = Mdm_ac_rel_Sel(g_pub_env.reply,&sMdm_ac_rel,"ac_id=%ld and mdm_sts in('0','1')\
             and main_ind='1')",sFn_appbuy_reg.de_ac_id);
        if(ret)
        {
            vtcp_exit("查询账号失败了[%d]",ret);
            goto ErrExit;
        }
        if(sFn_appbuy_reg.ct_ind[0] == '1')
        {
            /*现金预约的直接改状态为转认购*/
            strcpy(sFn_appbuy_reg.sts,"5");
        }
        else
        {
            /*转账的处理主表的控制金额*/
            ret = Dd_mst_Sel(g_pub_env.reply,&sDd_mst,"ac_id= %ld and ac_seqn=%d",
                sFn_appbuy_reg.de_ac_id,sFn_appbuy_reg.de_ac_seqn);
            if(ret)
            {
                vtcp_exit("查询活期主文件失败ret[%d]",ret);
                strcpy(g_pub_env.reply,"D227");
                goto ErrExit;
            }
            if(sDd_mst.ac_sts[0] != '1')
            {
                iFail_flag = 1;
            }
            if(sDd_mst.hold_sts[0] == '1' || sDd_mst.hold_sts[0] == '2'
                    || sDd_mst.zf_sts[0] == '1' || sDd_mst.zf_sts[0] == '2')
            {
                iFail_flag = 1;
            }
            /*检查下可用余额 暂时不要海智通的余额*/
            pub_de_call_ddBal(sDd_mst,0,&dAvbal);
            /*检查账户可用余额*/
            if(pub_num_amtcmp(sFn_appbuy_reg.app_amt,dAvbal) > 0)
            {
                vtcp_exit("账户可用余额不足");
                iFail_flag = 1;
            }
            if(iFail_flag == 1)
            {
                strcpy(sFn_appbuy_reg.sts,"2");
            }
            else
            {
                strcpy(sFn_appbuy_reg.sts,"5");
                /*借客户 贷募集*/
                sprintf(g_pub_tx.brf,"理财扣款%s",sFn_appbuy_reg.prdt_no);
                strcpy(g_pub_tx.ac_no,sMdm_ac_rel.ac_no);
                g_pub_tx.ac_id = sDd_mst.ac_id;
                g_pub_tx.ac_seqn = sDd_mst.ac_seqn;
                g_pub_tx.cif_no = sDd_mst.cif_no;
                strcpy(g_pub_tx.cur_no,sDd_mst.cur_no);
                strcpy(g_pub_tx.name,sMdm_ac_rel.name);
                strcpy(g_pub_tx.ct_ind,"2");
                strcpy(g_pub_tx.acc_ind,"1");
                strcpy(g_pub_tx.opn_br_no,sDd_mst.opn_br_no);
                strcpy(g_pub_tx.ac_id_type,"1");
                strcpy(g_pub_tx.ac_get_ind,"00000");
                g_pub_tx.svc_ind = 1001;
                strcpy(g_pub_tx.add_ind,"0");
                strcpy(g_pub_tx.dc_ind,"1");
                g_pub_tx.tx_amt1 = sFn_appbuy_reg.app_amt;
                strcpy(g_pub_tx.prt_ind,"0");
                strcpy(g_pub_tx.hst_ind,"1");
                ret = pub_acct_trance_de();
                if (ret)
                {
                    vtcp_exit( "调用存取款主控失败[%d]!",ret);
                    goto ErrExit;
                }
                /*获取理财交易机构的募集账号*/
                ret = pub_de_get_FnAcct_parm(sFn_appbuy_reg.prdt_no,"0001","",
                    sFn_appbuy_reg.reg_br_no,QS_BR_NO,cDc_prdt_no,cDc_ac_no,cBr_type);
                if(ret)
                {
                    vtcp_exit("获取理财募集账号失败了");
                    goto ErrExit;
                }
                strcpy(g_pub_tx.ac_no,cDc_ac_no);
                strcpy(g_pub_tx.opn_br_no,sFn_appbuy_reg.reg_br_no);
                g_pub_tx.ac_id = 0;
                g_pub_tx.ac_seqn = 0;
                g_pub_tx.cif_no = 0;
                strcpy(g_pub_tx.name,"");
                strcpy(g_pub_tx.acc_ind,"1");
                strcpy(g_pub_tx.ct_ind,"2");
                strcpy(g_pub_tx.ac_id_type,"9");
                strcpy(g_pub_tx.ac_get_ind,"00000");
                g_pub_tx.svc_ind = 9001;
                strcpy(g_pub_tx.add_ind,"1");
                strcpy(g_pub_tx.dc_ind,"2");
                g_pub_tx.tx_amt1 = sFn_appbuy_reg.app_amt;
                strcpy(g_pub_tx.prt_ind,"0");
                strcpy(g_pub_tx.hst_ind,"1");
                ret = pub_acct_trance_de();/*todo*/
                if (ret)
                {
                    vtcp_exit( "调用存取款主控失败[%d]!",ret);
                    goto ErrExit;
                }
            }
        }
        sFn_appbuy_reg.sp_date = g_pub_env.tx_date;
        sFn_appbuy_reg.sp_trace = g_pub_env.trace_no;
        sFn_appbuy_reg.sp_time = g_pub_env.tx_time;
        strcpy(sFn_appbuy_reg.sp_tel,g_pub_env.tel);
        strcpy(g_pub_tx.brf,"预约转认购");
        /*todo 获取利率上浮信息*/

        ret = Fn_appbuy_reg_Upd_Upd(sFn_appbuy_reg,g_pub_env.reply);
        if(ret)
        {
            vtcp_exit("更新购买登记簿游标失败[%d]",ret);
            goto ErrExit;
        }
        g_pub_env.trace_cnt ++;/*流水号不变流水笔次增加*/
        strcpy(g_pub_tx.brf,"预约转认购");
        g_pub_tx.tx_amt1 = sFn_appbuy_reg.app_amt;
        strcpy(g_pub_tx.ac_no,sMdm_ac_rel.ac_no);
        strcpy(g_pub_tx.acc_ind,"0");/*不记账*/
        if( pub_ins_trace_log() )
        {
            vtcp_exit("登记交易流水出错");
            strcpy(g_pub_env.reply,"P431");
            goto ErrExit;
        }
    }
    Fn_appbuy_reg_Clo_Upd();
    strcpy(g_pub_env.reply, "0000");
    vtcp_exit("理财预约转认购成功![%s]", g_pub_env.reply);
    set_zd_data(DC_REPLY, g_pub_env.reply);
    return 0;
ErrExit:
    vtcp_exit("理财预约转认购失败![%s]", g_pub_env.reply);
    set_zd_data(DC_REPLY, g_pub_env.reply);
    return 1;
}
