/*******************************************************************************
*** 版 本 号:  V1.0
*** 程序作者: shangyongchao
*** 日    期: 20151011
*** 所属模块: 理财产品
*** 程序名称: spH002
*** 程序作用: 完成理财产品客户发起的撤销（该交易由客户发起）
*** 使用注意:
*** 修改记录:
***     修改作者:
***     修改时间:
***     修改内容:
*******************************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "de_public.h"
#include "macro.h"
#include "fn_appbuy_reg_c.h"
#include "fn_appbuy_reg_fun.h"
/*外部函数声明*/
extern int pub_date_CalTrueDays(long begrq,long endrq);
extern int pub_num_amtcmp(double amt1,double amt2);
extern int pub_de_call_ddBal(struct dd_mst_c dd_mst_tmp,int type,double *bal);
extern int pub_ins_trace_log();
extern int pub_de_FnQuota(char *prdt_no,double tx_amt,int mode);
extern double pub_num_dround ( double lx );
extern int pub_de_get_FnAcct_parm(char *cPrdt_no,char *dc_rel_code,char *cif_type,char *opn_br_no,char *fnqs_br_no,char *cAcct_prdt_no,char *cAc_no,char *br_type);
/*内部函数声明*/
static int _cancel_apt_buy(char *flag);

int spH001()
{
    int ret = 0;
    char cOper_flag[2];/*0预约撤销 1认购撤销 2申购撤销 4赎回撤销*/
    memset(cOper_flag,0x00,sizeof(cOper_flag));
    /** 初始化全局变量 **/
    pub_base_sysinit();
    get_zd_data("0670",cOper_flag);
    vtcp_trace("cOper_flag[%s]",cOper_flag);
    if(cOper_flag[0] == '0' || cOper_flag[0] == '1')
    {
        /*预约和认购状态的撤销*/
        ret = _cancel_apt_buy(cOper_flag);
        if(ret)
        {
            vtcp_exit("撤销失败了");
            goto ErrExit;
        }
    }
    else if(cOper_flag[0] == '2')
    {
       /*申购的撤销*/
    }
    else
    {
        vtcp_exit("操作类型错误 [%s]",cOper_flag);
        strcpy(g_pub_env.reply, "S035");
        goto ErrExit;
    }
    vtcp_exit("Before OK return: reply is[%s]", g_pub_env.reply);
    set_zd_data(DC_REPLY, g_pub_env.reply);
    return 0;
ErrExit:
    if(strlen(g_pub_env.reply) == 0 || strcmp(g_pub_env.reply,"0000") == 0)
    {
        strcpy(g_pub_env.reply,"S035");
    }
    vtcp_exit( "Before return: reply is[%s]", g_pub_env.reply);
    set_zd_data(DC_REPLY, g_pub_env.reply);
    return 1;
}

/*撤销预约和认购函数
输入flag: 0预约撤销 1认购撤销*/
static int _cancel_apt_buy(char *flag)
{
    int ret = 0;
    int iDays = 0;/*活期计息天数*/
    int iIntst_flag = 0;/*是否计活期利息 0否1是*/
    long lIc_date = 0;/*起息日期*/
    long lTx_date = 0;
    long lTrace_no = 0;
    double dIntst = 0.00;/*活期利息*/
    double dRate = 0.00;/*活期利率*/
    char cAc_no[33];
    char cAgent_name[NAME_LEN+1];
    char cAgent_id_no[21];
    char cAgent_id_type[2];
    char cDc_prdt_no[21];/*会计产品*/
    char cDc_ac_no[33];/*募集账号*/
    char cBr_type[2];/*机构类型*/
    char cCif_type[2]; /*客户类型1个人2对公 其他为同业不同类型*/
    struct fn_appbuy_reg_c sFn_appbuy_reg;
    struct mdm_ac_rel_c sMdm_ac_rel;
    struct dd_mst_c sDd_mst;

    memset(cAc_no,0x00,sizeof(cAc_no));
    memset(cAgent_id_no,0x00,sizeof(cAgent_id_no));
    memset(cAgent_name,0x00,sizeof(cAgent_name));
    memset(cAgent_id_type,0x00,sizeof(cAgent_id_type));
    memset(cDc_prdt_no,0x00,sizeof(cDc_prdt_no));
    memset(cDc_ac_no,0x00,sizeof(cDc_ac_no));
    memset(cBr_type,0x00,sizeof(cBr_type));
    memset(cCif_type,0x00,sizeof(cCif_type));
    memset(&sDd_mst,0x00,sizeof(sDd_mst));
    memset(&sFn_appbuy_reg,0x00,sizeof(sFn_appbuy_reg));
    memset(&sMdm_ac_rel,0x00,sizeof(sMdm_ac_rel));
    
    get_zd_data("0300",cAc_no);/*存款账号*/
    get_zd_long("0440",&lTx_date);/*预约或购买流水*/
    get_zd_long("0280",&lTrace_no);/*预约或购买流水*/
    get_zd_data("0631",cAgent_id_type);/*对公的代理人证件类型*/
    get_zd_data("0632",cAgent_id_no);/*对公的代理人证件号*/
    get_zd_data("0250",cAgent_name);/*对公的代理人姓名*/
    strcpy(g_pub_tx.agent_id_no, cAgent_id_type);
    strcpy(g_pub_tx.agent_id_no, cAgent_id_no);
    strcpy(g_pub_tx.agent_id_name, cAgent_name);

    vtcp_trace("ltx_date[%ld],lTrace_no[%ld]",lTx_date,lTrace_no);

    /*更新fn_appbuy_reg*/
    ret = Fn_appbuy_reg_Dec_Upd(g_pub_env.reply,"reg_date=%ld and reg_trace=%ld",
            lTx_date,lTrace_no);
    if(ret)
    {
        vtcp_exit("定义理财产品登记簿游标出错[%d] ",ret);
        strcpy(g_pub_env.reply,"D332");
        goto ErrExit;
    }
    ret = Fn_appbuy_reg_Fet_Upd(&sFn_appbuy_reg,g_pub_env.reply);
    if(ret)
    {
        vtcp_exit("取理财产品登记簿游标出错[%d] ",ret);
        strcpy(g_pub_env.reply,"D331");
        goto ErrExit;
    }
    /*检查是否这个账号的记录*/
    ret = Mdm_ac_rel_Sel(g_pub_env.reply,&sMdm_ac_rel,"ac_no='%s'",cAc_no);
    if(ret)
    {
        vtcp_exit("查询账号失败了[%d] ",ret);
        strcpy(g_pub_env.reply,"P084");
        goto ErrExit;
    }
    if(sFn_appbuy_reg.de_ac_id != sMdm_ac_rel.ac_id)
    {
        vtcp_exit("非同一个账号的认购记录 de_ac_id[%ld],ac_id[%ld] ",
            sFn_appbuy_reg.de_ac_id,sMdm_ac_rel.ac_id);
        strcpy(g_pub_env.reply,"非同一个账号的认购记录");
        goto ErrExit;
    }
    ret = Dd_mst_Sel(g_pub_env.reply,&sDd_mst,"ac_id=%ld and ac_seqn=%d",
        sFn_appbuy_reg.de_ac_id,sFn_appbuy_reg.de_ac_seqn);
    if(ret)
    {
        vtcp_exit("查询活期主文件失败了[%d] ",ret);
        goto ErrExit;
    }
    /*暂定柜面渠道购买的不能跨机构撤销*/
    if(strcmp(sFn_appbuy_reg.reg_br_no,g_pub_env.tx_br_no) != 0
        && strcmp(sFn_appbuy_reg.chnl_type,CHNL_BANK_TEL) == 0)
    {
        vtcp_exit("该记录非本机构交易[%s] ",sFn_appbuy_reg.reg_br_no);
        strcpy(g_pub_env.reply,"P301");
        goto ErrExit;
    }

    /*暂定认购撤销要检查产品属性 预约的可不检查*/
    if(flag[0] == '1' && strcmp(get_prdt_cparm(sFn_appbuy_reg.prdt_no,"bcancel_yn"),"Y") != 0)
    {
        vtcp_exit("该理财产品不能撤销");
        strcpy(g_pub_env.reply, "D324");
        goto ErrExit;
    }

    if(flag[0] == '0' && strcmp(get_prdt_cparm(sFn_appbuy_reg.prdt_no,"prdt_sts"),"6") != 0)
    {
        /*理论上不会出现这种情况，因为预约期结束后立即转认购了*/
        vtcp_exit("该产品非可预约状态[%s] ",get_prdt_cparm(sFn_appbuy_reg.prdt_no,"prdt_sts"));
        strcpy(g_pub_env.reply,"D395");
        goto ErrExit;
    }
    else if(flag[0] == '1' && strcmp(get_prdt_cparm(sFn_appbuy_reg.prdt_no,"prdt_sts"),"0") != 0)
    {
        vtcp_exit("该产品非募集状态[%s] ",get_prdt_cparm(sFn_appbuy_reg.prdt_no,"prdt_sts"));
        strcpy(g_pub_env.reply,"D395");
        goto ErrExit;
    }
    vtcp_trace("flag[%s] app_knd[%s] sts[%s]",flag,sFn_appbuy_reg.app_knd,sFn_appbuy_reg.sts);
    /*检查一下所选方式是不是对的*/
    if(flag[0] == '0')
    {
        strcpy(g_pub_tx.brf,"理财产品预约撤销");
        /*预约撤销*/
        if(sFn_appbuy_reg.app_knd[0] == '1' && sFn_appbuy_reg.sts[0] == '0')
        {
            iIntst_flag = 0;
            strcpy(sFn_appbuy_reg.sts,"1");
            vtcp_trace("预约撤销，还没转认购的撤销，不需处理控制金额");
        }
        else
        {
            vtcp_exit("该购买记录非预约购买，不能使用预约撤销[%d] ",ret);
            set_zd_data(DC_GET_UMSG,"该购买记录非预约购买，不能使用预约撤销");
            strcpy(g_pub_env.reply,"P275");
            goto ErrExit;
        }
    }
    else if(flag[0] == '1')
    {
        strcpy(g_pub_tx.brf,"理财产品认购撤销");
        /*认购的撤销 包含两种情况 预约转成认购的 和直接认购的*/
        if(sFn_appbuy_reg.app_knd[0] == '2' && sFn_appbuy_reg.sts[0] == '0')
        {
            lIc_date = sFn_appbuy_reg.reg_date;/*起息日为认购日*/
            strcpy(sFn_appbuy_reg.sts,"1");
            vtcp_trace("直接认购的撤销，需要更新控制金额");
        }
        else if(sFn_appbuy_reg.app_knd[0] == '1' && sFn_appbuy_reg.app_knd[0] =='5')
        {
            lIc_date = get_prdt_lparm(sFn_appbuy_reg.prdt_no,"coll_beg_date");
            if(lIc_date <= 0)
            {
                vtcp_exit("获取起息日期失败了[%ld]",lIc_date);
                strcpy(g_pub_env.reply,"CUD2");
                goto ErrExit;
            }
            strcpy(sFn_appbuy_reg.sts,"7");
            vtcp_trace("预约转认购的撤销，需要更新控制金额");
        }
        else
        {
            vtcp_exit("该购买记录非认购状态，不能使用认购撤销[%d] ",ret);
            set_zd_data(DC_GET_UMSG,"该购买记录非认购状态，不能使用认购撤销");
            strcpy(g_pub_env.reply,"P275");
            goto ErrExit;
        }
        iIntst_flag = 1;
    }

    ret = Fn_appbuy_reg_Upd_Upd(sFn_appbuy_reg,g_pub_env.reply);
    if(ret)
    {
        vtcp_exit("更新理财产品登记簿出错[%d] ",ret);
        strcpy(g_pub_env.reply,"D332");
        goto ErrExit;
    }
    Fn_appbuy_reg_Clo_Upd();
    /*获取产品的网点募集账号*/
    ret = pub_de_get_FnAcct_parm(sFn_appbuy_reg.prdt_no,"0001","0",g_pub_env.tx_br_no,
        QS_BR_NO,cDc_prdt_no,cDc_ac_no,cBr_type);
    if(ret)
    {
        vtcp_exit("获取理财募集账号失败了");
        goto ErrExit;
    }
    vtcp_trace("理财认购申购待上划户为[%s]",cDc_ac_no);
    set_zd_data("0310",cDc_ac_no);/*为下一个流程配置 todo*/
    if(cBr_type[0] == '1')
    {
        set_zd_data("0910",sFn_appbuy_reg.reg_br_no);
    }
    else
    {
        set_zd_data("0910",QS_BR_NO);
    }
    memset(cDc_prdt_no,0x00,sizeof(cDc_prdt_no));
    memset(cDc_ac_no,0x00,sizeof(cDc_ac_no));
    memset(cBr_type,0x00,sizeof(cBr_type));
    ret = pub_de_get_FnAcct_parm(sFn_appbuy_reg.prdt_no,"0005","0",g_pub_env.tx_br_no,
        QS_BR_NO,cDc_prdt_no,cDc_ac_no,cBr_type);
    if(ret)
    {
        vtcp_exit("获取理财募集账号失败了");
        goto ErrExit;
    }
    set_zd_data("0320",cDc_ac_no);/*为下一个流程配置 todo*/
    if(cBr_type[0] == '1')
    {
        set_zd_data("0640",sFn_appbuy_reg.reg_br_no);
    }
    else
    {
        set_zd_data("0640",QS_BR_NO);
    }
    vtcp_trace("iIntst_flag[%d]",iIntst_flag);
    /*计算活期利息归还客户*/
    if(iIntst_flag == 1)
    {
        /*计算天数*/
        iDays = pub_date_CalTrueDays(lIc_date,g_pub_env.tx_date);
        if(iDays < 0)
        {
            vtcp_exit("计算天数失败了[%d]",iDays);
            strcpy(g_pub_env.reply,"P382");
            goto ErrExit;
        }
        vtcp_trace("iDays[%d]",iDays);
        strcpy(g_pub_tx.cur_no,get_prdt_cparm(sFn_appbuy_reg.prdt_no,"cur_no"));
        /*获取活期利率*/
        ret = pub_rule_GetRate(HQLLBH, g_pub_tx.cur_no, g_pub_env.tx_date, &dRate);
        if (ret)
        {
            vtcp_exit("获取利率失败了[%d]",ret);
            goto ErrExit;
        }
        vtcp_trace("dRate[%lf]",dRate);
        dIntst = sFn_appbuy_reg.app_amt * dRate/L360/L100 * iDays;
        dIntst = pub_num_dround ( dIntst );
        vtcp_trace("活期利息[%lf]",dIntst);
        /*获取活期利息支出账号*/
        memset(cDc_prdt_no,0x00,sizeof(cDc_prdt_no));
        memset(cDc_ac_no,0x00,sizeof(cDc_ac_no));
        strcpy(g_pub_tx.opn_br_no,sDd_mst.opn_br_no);
        ret = pub_de_get_acct_parm(sDd_mst.prdt_no,"0153",cDc_prdt_no,cDc_ac_no);
        if(ret)
        {
            vtcp_exit("获取利息支出记账账号失败了[%d]",ret);
            goto ErrExit;
        }
        set_zd_data("0330",cDc_ac_no);
        set_zd_data("0650",sDd_mst.opn_br_no);
        strcpy(g_pub_tx.opn_br_no,g_pub_env.tx_br_no);
    }

    /*恢复产品相关额度*/
    ret = pub_de_FnQuota(sFn_appbuy_reg.prdt_no,sFn_appbuy_reg.app_amt,1);
    if(ret)
    {
        vtcp_exit("处理配额错误ret[%d]",ret);
        goto ErrExit;
    }
    strcpy(g_pub_tx.acc_ind,"0");
    strcpy(g_pub_tx.ac_no,cAc_no);
    g_pub_tx.tx_amt1 = sFn_appbuy_reg.app_amt;
    if( pub_ins_trace_log() )
    {
        vtcp_exit("登记交易流水出错");
        strcpy(g_pub_env.reply,"P431");
        goto ErrExit;
    }
    strcpy(g_pub_env.reply,"0001");/*走归还客户本金和利息的分支*/
    vtcp_exit("撤销成功");
    return 0;
ErrExit:
    vtcp_exit("撤销失败");
    return 1;
}
