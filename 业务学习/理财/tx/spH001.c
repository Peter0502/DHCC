/*******************************************************************************
*** 版 本 号:  V1.0
*** 程序作者: shangyongchao
*** 日    期: 20151011
*** 所属模块: 理财产品
*** 程序名称: spH001
*** 程序作用: 完成理财产品认购和预约
*** 使用注意:
*** 修改记录:
***     修改作者:
***     修改时间:
***     修改内容:
*******************************************************************************/
#define EXTERN
#include "public.h"
#include "de_public.h"
#include "macro.h"
#include "fn_appbuy_reg_c.h"
#include "fn_appbuy_reg_fun.h"
/*外部函数声明*/
extern int pub_de_get_FnAcct_parm(char *cPrdt_no,char *dc_rel_code,char *cif_type,char *opn_br_no,char *fnqs_br_no,char *cAcct_prdt_no,char *cAc_no,char *br_type);
extern double pub_num_dround ( double lx );
extern int pub_com_CheqClsNo( long ac_id,int ac_seqn,char *note_type,char *cheq_beg_no,char *cheq_end_no);
extern int pub_cif_get_ciflvl(long cif_no, char *cif_lvl);
extern int pub_cif_ac2ciftype(char *ac_no, char *cif_type);
extern int pub_num_amtcmp(double amt1,double amt2);
extern int pub_de_call_ddBal(struct dd_mst_c dd_mst_tmp,int type,double *bal);
extern long pub_time_date();
extern int pub_ins_trace_log();
extern int pub_de_FnQuota(char *prdt_no,double tx_amt,int mode);
/*内部函数声明*/
static int _buy_func(void);
static int _apt_func(void);
static int _check_note_sts(char *note_sts);

int spH001(void)
{
    int iRet = 0;
    char cOper_flag[2];/*0预约 1认购 2撤销*/
    memset(cOper_flag,0x00,sizeof(cOper_flag));
    /** 初始化全局变量 **/
    pub_base_sysinit();
    get_zd_data("0670",cOper_flag);
    vtcp_trace("cOper_flag[%s]",cOper_flag);
    /*根据cOper_flag进行不同处理*/
    if(cOper_flag[0] == '0')
    {
        /*预约*/
        iRet = _apt_func();
        if(iRet)
        {
            vtcp_exit("预约失败了");
            goto ErrExit;
        }
    }
    else if(cOper_flag[0] == '1')
    {
        /*认购*/
        iRet = _buy_func();
        if(iRet)
        {
            vtcp_exit("认购失败了");
            goto ErrExit;
        }
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

/*认购函数*/
static int _buy_func(void)
{
    int iRet = 0;
    int iAc_seqn = 0;/*账户序号*/
    char cDc_prdt_no[21];/*会计产品*/
    char cDc_ac_no[33];/*募集账号*/
    char cBr_type[2];/*机构类型*/
    char cOpn_time[7];/*营业开始时间*/
    char cClos_time[7];/*营业结束时间*/
    char cCt_ind[2];/*现转标志*/
    char cPrdt_no[21]; /*产品代码*/
    char cCif_type[2]; /*客户类型1个人2对公 其他为同业不同类型*/
    char cAgent_id_type[2]; /*对公户的代理人证件类型*/
    char cAgent_id_no[21]; /*对公户的代理人证件*/
    char cAgent_name[NAME_LEN+1]; /*对公户的代理人姓名*/
    char cAc_no[33]; /*账号*/
    char cPact_no[21]; /*合同号*/
    char cMang[TEL_LEN+1]; /*客户经理号*/
    char cCif_lvl[2];
    char cNote_type[4]; /*凭证类型*/
    char cNote_no[17]; /*凭证号码*/
    char cFn_note_no[17]; /*理财凭证号*/
    double dAvbal = 0.00; /*账户可用余额*/
    double dFlt_radio = 0.00;/*收益上浮比例*/
    double dTx_amt = 0.00;/*认购金额*/

    struct fn_appbuy_reg_c sFn_appbuy_reg;
    struct dd_mst_c sDd_mst;
    /*初始化变量*/
    memset(cOpn_time,0x00,sizeof(cOpn_time));
    memset(cClos_time,0x00,sizeof(cClos_time));
    memset(cCt_ind,0x00,sizeof(cCt_ind));
    memset(cPrdt_no,0x00,sizeof(cPrdt_no));
    memset(cCif_type,0x00,sizeof(cCif_type));
    memset(cAgent_id_type,0x00,sizeof(cAgent_id_type));
    memset(cAgent_id_no,0x00,sizeof(cAgent_id_no));
    memset(cAgent_name,0x00,sizeof(cAgent_name));
    memset(cAc_no,0x00,sizeof(cAc_no));
    memset(cPact_no,0x00,sizeof(cPact_no));
    memset(cMang,0x00,sizeof(cMang));
    memset(cCif_lvl,0x00,sizeof(cCif_lvl));
    memset(cNote_type,0x00,sizeof(cNote_type));
    memset(cNote_no,0x00,sizeof(cNote_no));
    memset(cFn_note_no,0x00,sizeof(cFn_note_no));
    memset(cDc_prdt_no,0x00,sizeof(cDc_prdt_no));
    memset(cDc_ac_no,0x00,sizeof(cDc_ac_no));
    memset(cBr_type,0x00,sizeof(cBr_type));

    memset(&sFn_appbuy_reg,0x00,sizeof(sFn_appbuy_reg));
    memset(&sDd_mst,0x00,sizeof(sDd_mst));

    get_zd_data("0670",cCt_ind);/*现转标志*/
    get_zd_int("0360",&iAc_seqn);/*活期账户序号*/
    get_zd_double("0390",&dTx_amt);/*金额*/
    get_zd_data("0300",cAc_no);/*活期账号*/
    get_zd_data("0631",cAgent_id_type);/*对公的代理人证件类型*/
    get_zd_data("0632",cAgent_id_no);/*对公的代理人证件号*/
    get_zd_data("0250",cAgent_name);/*对公的代理人姓名*/
    get_zd_data("0230",cPrdt_no);/*购买产品号*/
    get_zd_data("0890",cPact_no);/*购买协议号*/
    get_zd_data("0840",cMang);/*客户经理*/
    get_zd_data("0581",cNote_type);/*凭证类型*/
    get_zd_data("0582",cNote_no);/*凭证号码*/
    get_zd_data("0592",cFn_note_no);/*理财凭证号*/

    strcpy(g_pub_tx.agent_id_no, cAgent_id_type);
    strcpy(g_pub_tx.agent_id_no, cAgent_id_no);
    strcpy(g_pub_tx.agent_id_name, cAgent_name);

    /*先检查账号信息*/
    pub_base_old_acno(cAc_no);
    iRet = pub_cif_ac2ciftype(cAc_no,cCif_type);
    if (iRet)
    {
        vtcp_exit( "查找客户类型错误[%d]", iRet);
        strcpy(g_pub_env.reply, "P336");
        goto ErrExit;
    }
    /*检查介质状态*/
    iRet = _check_note_sts(g_mdm_ac_rel.mdm_sts);
    if(iRet)
    {
        vtcp_exit("介质状态不正常");
        goto ErrExit;
    }
    /*检查账户余额，同时更改控制金额*/
    iRet = Dd_mst_Dec_Upd(g_pub_env.reply,"ac_id= %ld and ac_seqn=%d",g_mdm_ac_rel.ac_id,iAc_seqn);
    if(iRet)
    {
        vtcp_exit("定义活期主文件更新游标失败iRet[%d]",iRet);
        strcpy(g_pub_env.reply,"D227");
        goto ErrExit;
    }
    iRet = Dd_mst_Fet_Upd(&sDd_mst,g_pub_env.reply);
    if(iRet != DB_OK && iRet != DB_NOTFOUND)
    {
        vtcp_exit("定义活期主文件更新游标失败iRet[%d]",iRet);
        strcpy(g_pub_env.reply,"D230");
        goto ErrExit;
    }
    else if(iRet == DB_NOTFOUND)
    {
        vtcp_exit("没有这个账户");
        strcpy(g_pub_env.reply,"D229");
        goto ErrExit;
    }
    if(sDd_mst.ac_sts[0] != '1')
    {
        vtcp_exit("活期账户账户状态不正常");
        strcpy(g_pub_env.reply,"P183");
        goto ErrExit;
    }
    if(sDd_mst.hold_sts[0] == '1' || sDd_mst.hold_sts[0] == '2'
            || sDd_mst.zf_sts[0] == '1' || sDd_mst.zf_sts[0] == '2')
    {
        vtcp_exit("账户被全部冻结或全部止付或只进不出");
        strcpy(g_pub_env.reply,"D130");
        goto ErrExit;
    }
    /*检查下可用余额 暂时不要海智通的余额*/
    pub_de_call_ddBal(sDd_mst,0,&dAvbal);
    /*检查下账户的币种*/
    if(strcmp(get_prdt_cparm(cPrdt_no,"cur_no"),sDd_mst.cur_no) != 0)
    {
        vtcp_exit("币种不支持parm[%s] dd_mst[%s]",get_prdt_cparm(cPrdt_no,"cur_no"),sDd_mst.cur_no);
        strcpy(g_pub_env.reply,"D337");
        goto ErrExit;
    }
    if(pub_num_amtcmp(dTx_amt,dAvbal) > 0)
    {
        vtcp_exit("账户可用余额不足");
        strcpy(g_pub_env.reply,"P066");
        goto ErrExit;
    }
    iRet =  Dd_mst_Upd_Upd(sDd_mst,g_pub_env.reply);
    if(iRet)
    {
        vtcp_exit("更新活期主文件失败iRet[%d]",iRet);
        strcpy(g_pub_env.reply,"D227");
        goto ErrExit;
    }
    Dd_mst_Clo_Upd();
    /*根据客户号获取客户级别*/
    iRet = pub_cif_get_ciflvl(sDd_mst.cif_no,cCif_lvl);
    if(iRet)
    {
        vtcp_exit("获取客户级别失败[%d]",iRet);
        goto ErrExit;
    }
    vtcp_trace("客户级别[%s]客户类型[%s]",cCif_lvl,cCif_type);
    /*对公户 销客户凭证*/
    if(cCif_type[0]  == '2' && strcmp(cNote_type,"299") != 0)
    {
        iRet = pub_com_CheqClsNo(g_mdm_ac_rel.ac_id,iAc_seqn, cNote_type,cNote_no,cNote_no);
        if(iRet)
        {
            vtcp_exit("销记客户凭证失败了[%d]",iRet);
            goto ErrExit;
        }
    }
    /*理财产品状态*/
    if(strcmp(get_prdt_cparm(cPrdt_no, "prdt_sts"),"0") != 0)
    {
        vtcp_exit("该产品非认购状态[%s] ",get_prdt_cparm(cPrdt_no, "prdt_sts"));
        strcpy(g_pub_env.reply,"D334");
        goto ErrExit;
    }
    /*检查是否在募集期*/
    if(get_prdt_lparm(cPrdt_no, "coll_beg_date") > g_pub_env.tx_date
        || get_prdt_lparm(cPrdt_no, "coll_end_date") < g_pub_env.tx_date)
    {
        vtcp_exit("该产品非认购状态");
        strcpy(g_pub_env.reply,"C334");
        goto ErrExit;
    }
    /*验证现转*/
    if(strcmp(get_prdt_cparm(cPrdt_no,"bdpst_cash_yn"),"N") == 0 && cCt_ind[0] == '1')
    {
        vtcp_exit("该产品不能现金购买");
        strcpy(g_pub_env.reply,"D392");
        goto ErrExit;
    }
    if(strcmp(get_prdt_cparm(cPrdt_no,"bdpst_tran_yn"),"N") == 0 && cCt_ind[0] == '2')
    {
        vtcp_exit("该产品不能转账购买");
        strcpy(g_pub_env.reply,"D393");
        goto ErrExit;
    }
    /*认购渠道 todo*/
    /*认购时间的控制，必须在有效营业时间内*/
    memcpy(cOpn_time,get_prdt_cparm(cPrdt_no,"bopen_time"),sizeof(cOpn_time)-1);
    memcpy(cClos_time,get_prdt_cparm(cPrdt_no,"bclose_time"),sizeof(cClos_time)-1);
    if(g_pub_env.tx_time < atol(cOpn_time) || g_pub_env.tx_time > atol(cClos_time))
    {
        vtcp_exit("不在产品的营业时间内");
        strcpy(g_pub_env.reply,"D396");
        goto ErrExit;
    }
    /*验证客户类型*/
    if(strcmp(get_prdt_cparm(cPrdt_no,"cif_type"),"1") == 0 && cCif_type[0] != '1')/*个人*/
    {
        vtcp_exit("客户类型不符[%s] ",cCif_type);
        strcpy(g_pub_env.reply,"P336");
        goto ErrExit;
    }
    else if(strcmp(get_prdt_cparm(cPrdt_no, "cif_type"),"2") == 0 && cCif_type[0] != '2')/*对公*/
    {
        vtcp_exit("客户类型不符[%s] ",cCif_type);
        strcpy(g_pub_env.reply,"P336");
        goto ErrExit;
    }
    else if(strcmp(get_prdt_cparm(cPrdt_no, "cif_type"),"3") == 0 && cCif_type[0] != '4')/*同业*/
    {
        vtcp_exit("客户类型不符[%s] ",cCif_type);
        strcpy(g_pub_env.reply,"P336");
        goto ErrExit;
    }
    /*处理产品额度*/
    iRet = pub_de_FnQuota(cPrdt_no,dTx_amt,0);
    if(iRet)
    {
        vtcp_exit("处理配额错误iRet[%d]",iRet);
        goto ErrExit;
    }
    /*是否满足最低认购额*/
    if(cCif_type[0] == '1' && pub_num_amtcmp(dTx_amt,get_prdt_dparm(cPrdt_no,"bdpst_pmin_amt")) < 0)
    {
        vtcp_exit("低于最低认购金额[%lf][%lf]",dTx_amt,get_prdt_dparm(cPrdt_no,"bdpst_pmin_amt"));
        strcpy(g_pub_env.reply,"D338");
        goto ErrExit;
    }
    else if(pub_num_amtcmp(dTx_amt,get_prdt_dparm(cPrdt_no,"bdpst_cmin_amt")) < 0)
    {
        vtcp_exit("低于最低认购金额[%lf][%lf]",dTx_amt,get_prdt_dparm(cPrdt_no,"bdpst_cmin_amt"));
        strcpy(g_pub_env.reply,"D338");
        goto ErrExit;
    }
    /*是否高于最高认购额*/
    if(cCif_type[0] == '1' && pub_num_amtcmp(dTx_amt,get_prdt_dparm(cPrdt_no,"bdpst_pmax_amt")) > 0)
    {
        vtcp_exit("高于最高认购金额[%lf][%lf]",dTx_amt,get_prdt_dparm(cPrdt_no,"bdpst_pmax_amt"));
        strcpy(g_pub_env.reply,"D391");
        goto ErrExit;
    }
    else if(pub_num_amtcmp(dTx_amt,get_prdt_dparm(cPrdt_no,"bdpst_cmax_amt")) > 0)
    {
        vtcp_exit("高于最高认购金额[%lf][%lf]",dTx_amt,get_prdt_dparm(cPrdt_no,"bdpst_cmax_amt"));
        strcpy(g_pub_env.reply,"D391");
        goto ErrExit;
    }
    /*单位递增额的校验todo*/
    /*获取收益浮动比例 todo*/
    strcpy(sFn_appbuy_reg.flt_type,"");
    sFn_appbuy_reg.flt_radio = dFlt_radio;
    /*给购买登记簿各个字段赋值*/
    sFn_appbuy_reg.app_amt  = dTx_amt;
    sFn_appbuy_reg.share_amt = dTx_amt / (get_prdt_dparm(cPrdt_no,"unit_amt"));
    /*获取产品预期收益率 todo*/
    sFn_appbuy_reg.rate = 0.00;
    /*计算预期收益*/
    if(sFn_appbuy_reg.flt_type[0] == '1')/*浮点上浮*/
    {
        sFn_appbuy_reg.intst=sFn_appbuy_reg.app_amt * get_prdt_iparm(cPrdt_no,"term")
                         * (sFn_appbuy_reg.rate + sFn_appbuy_reg.flt_radio)/get_prdt_iparm(cPrdt_no,"intst_rate_mode")/L100;
    }
    else if(sFn_appbuy_reg.flt_type[0] == '2')/*浮比上浮*/
    {
        sFn_appbuy_reg.intst=sFn_appbuy_reg.app_amt * get_prdt_iparm(cPrdt_no,"term")
                         * sFn_appbuy_reg.rate * (1+ sFn_appbuy_reg.flt_radio)/get_prdt_iparm(cPrdt_no,"intst_rate_mode")/L100;
    }
    sFn_appbuy_reg.intst = pub_num_dround(sFn_appbuy_reg.intst);
    set_zd_double("0400",sFn_appbuy_reg.intst);
    set_zd_double("0960",sFn_appbuy_reg.flt_radio);
    sFn_appbuy_reg.reg_date = g_pub_env.tx_date;
    sFn_appbuy_reg.reg_trace=g_pub_env.trace_no;
    strcpy(sFn_appbuy_reg.reg_br_no,g_pub_env.tx_br_no);
    strcpy(sFn_appbuy_reg.reg_tel,g_pub_env.tel);
    strcpy(sFn_appbuy_reg.chnl_type,g_pub_rule.chnl_type);
    sFn_appbuy_reg.nat_date=pub_time_date();
    sFn_appbuy_reg.reg_time=g_pub_env.tx_time;
    strcpy(sFn_appbuy_reg.prdt_no,cPrdt_no);
    sFn_appbuy_reg.de_ac_id = sDd_mst.ac_id;
    sFn_appbuy_reg.de_ac_seqn = sDd_mst.ac_seqn;
    sFn_appbuy_reg.cif_no = sDd_mst.cif_no;
    strcpy(sFn_appbuy_reg.cif_type,cCif_type);
    strcpy(sFn_appbuy_reg.note_type,cNote_type);
    strcpy(sFn_appbuy_reg.note_no,cNote_no);
    strcpy(sFn_appbuy_reg.fn_note_no,cFn_note_no);
    strcpy(sFn_appbuy_reg.pact_no,cPact_no);
    strcpy(sFn_appbuy_reg.agent_id_type,cAgent_id_type);
    strcpy(sFn_appbuy_reg.agent_id_no,cAgent_id_no);
    strcpy(sFn_appbuy_reg.agent_name,cAgent_name);
    strcpy(sFn_appbuy_reg.ct_ind,cCt_ind);
    if(cCt_ind[0] == '1')
    {
        strcpy(sFn_appbuy_reg.pay_yn,"Y");
        strcpy(sFn_appbuy_reg.fee_pay_yn,"Y");
    }
    else
    {
        strcpy(sFn_appbuy_reg.pay_yn,"N");/*转账购买的都等成立再扣钱*/
        strcpy(sFn_appbuy_reg.fee_pay_yn,"N");/*手续费是否已扣todo*/
    }
    sFn_appbuy_reg.fee_amt = 0.00;/*手续费 todo*/
    sFn_appbuy_reg.curr_issue = get_prdt_iparm(cPrdt_no,"curr_issue");/*认购的都是从1开始的*/
    strcpy(sFn_appbuy_reg.app_knd,"2");
    strcpy(sFn_appbuy_reg.brf,"认购理财产品");
    strcpy(sFn_appbuy_reg.sts,"0");
    iRet = Fn_appbuy_reg_Ins(sFn_appbuy_reg,g_pub_env.reply);
    if(iRet)
    {
        vtcp_exit("登记理财产品登记簿错误iRet[%d]",iRet);
        strcpy(g_pub_env.reply,"D331");
        goto ErrExit;
    }

    /*登记流水*/
    strcpy(g_pub_tx.ac_no,cAc_no);
    strcpy(g_pub_tx.brf,"理财产品认购");
    g_pub_tx.tx_amt1 = dTx_amt;
    strcpy(g_pub_tx.acc_ind,"0");
    if( pub_ins_trace_log() )
    {
        vtcp_exit("登记交易流水出错");
        strcpy(g_pub_env.reply,"P431");
        goto ErrExit;
    }
    /*获取产品的网点募集账号*/
    iRet = pub_de_get_FnAcct_parm(cPrdt_no,"0001","0",g_pub_env.tx_br_no,
        QS_BR_NO,cDc_prdt_no,cDc_ac_no,cBr_type);
    if(iRet)
    {
        vtcp_exit("获取理财募集账号失败了");
        goto ErrExit;
    }
    if(cCt_ind[0] == '1')
    {
        strcpy(g_pub_env.reply,"0001");/*现金的下一个流程要收取现金*/
    }
    else
    {
        strcpy(g_pub_env.reply,"0002");/*转账的扣款*/
    }
    vtcp_exit("认购成功[%s]",g_pub_env.reply);
    return 0;
ErrExit:
    vtcp_exit("认购失败");
    return 1;
}
/*预约函数 预约不控制客户资金 只更新产品额度*/
static int _apt_func(void)
{
    int iRet = 0;
    int iAc_seqn = 0;/*账户序号*/
    char cOpn_time[7];/*营业开始时间*/
    char cClos_time[7];/*营业结束时间*/
    char cCt_ind[2];/*先转标志*/
    char cPrdt_no[21]; /*产品代码*/
    char cCif_type[2]; /*客户类型1个人2对公 其他为同业不同类型*/
    char cAgent_id_type[2]; /*对公户的代理人证件类型*/
    char cAgent_id_no[21]; /*对公户的代理人证件*/
    char cAgent_name[NAME_LEN+1]; /*对公户的代理人姓名*/
    char cAc_no[33]; /*账号*/
    char cPact_no[21]; /*合同号*/
    char cMang[TEL_LEN+1]; /*客户经理号*/
    char cCif_lvl[2];
    char cNote_type[4]; /*凭证类型*/
    char cNote_no[17]; /*凭证号码*/
    char cFn_note_no[17]; /*理财凭证号*/
    double dFlt_radio = 0.00;/*收益上浮比例*/
    double dTx_amt = 0.00;/*认购金额*/

    struct fn_appbuy_reg_c sFn_appbuy_reg;
    struct dd_mst_c sDd_mst;
    /*初始化变量*/
    memset(cOpn_time,0x00,sizeof(cOpn_time));
    memset(cClos_time,0x00,sizeof(cClos_time));
    memset(cCt_ind,0x00,sizeof(cCt_ind));
    memset(cPrdt_no,0x00,sizeof(cPrdt_no));
    memset(cCif_type,0x00,sizeof(cCif_type));
    memset(cAgent_id_type,0x00,sizeof(cAgent_id_type));
    memset(cAgent_id_no,0x00,sizeof(cAgent_id_no));
    memset(cAgent_name,0x00,sizeof(cAgent_name));
    memset(cAc_no,0x00,sizeof(cAc_no));
    memset(cPact_no,0x00,sizeof(cPact_no));
    memset(cMang,0x00,sizeof(cMang));
    memset(cCif_lvl,0x00,sizeof(cCif_lvl));
    memset(cNote_type,0x00,sizeof(cNote_type));
    memset(cNote_no,0x00,sizeof(cNote_no));
    memset(cFn_note_no,0x00,sizeof(cFn_note_no));
    memset(&sFn_appbuy_reg,0x00,sizeof(sFn_appbuy_reg));
    memset(&sDd_mst,0x00,sizeof(sDd_mst));

    get_zd_data("0670",cCt_ind);/*现转*/
    get_zd_int("0360",&iAc_seqn);/*活期账户序号*/
    get_zd_double("0390",&dTx_amt);/*金额*/
    get_zd_data("0300",cAc_no);/*活期账号*/
    get_zd_data("0631",cAgent_id_type);/*对公的代理人证件类型*/
    get_zd_data("0632",cAgent_id_no);/*对公的代理人证件号*/
    get_zd_data("0250",cAgent_name);/*对公的代理人姓名*/
    get_zd_data("0230",cPrdt_no);/*购买产品号*/
    get_zd_data("0890",cPact_no);/*购买协议号*/
    get_zd_data("0840",cMang);/*客户经理*/
    get_zd_data("0581",cNote_type);/*凭证类型*/
    get_zd_data("0582",cNote_no);/*凭证号码*/
    get_zd_data("0592",cFn_note_no);/*理财凭证号*/

    strcpy(g_pub_tx.agent_id_no, cAgent_id_type);
    strcpy(g_pub_tx.agent_id_no, cAgent_id_no);
    strcpy(g_pub_tx.agent_id_name, cAgent_name);

    /*先检查账号信息*/
    pub_base_old_acno(cAc_no);
    iRet = pub_cif_ac2ciftype(cAc_no,cCif_type);
    if (iRet)
    {
        vtcp_exit( "查找客户类型错误[%d]", iRet);
        strcpy(g_pub_env.reply, "P336");
        goto ErrExit;
    }
    /*检查介质状态*/
    iRet = _check_note_sts(g_mdm_ac_rel.mdm_sts);
    if(iRet)
    {
        vtcp_exit("介质状态不正常");
        goto ErrExit;
    }
    /*检查账户余额，同时更改控制金额*/
    iRet = Dd_mst_Dec_Upd(g_pub_env.reply,"ac_id= %ld and ac_seqn=%d",g_mdm_ac_rel.ac_id,iAc_seqn);
    if(iRet)
    {
        vtcp_exit("定义活期主文件更新游标失败iRet[%d]",iRet);
        strcpy(g_pub_env.reply,"D227");
        goto ErrExit;
    }
    iRet = Dd_mst_Fet_Upd(&sDd_mst,g_pub_env.reply);
    if(iRet != DB_OK && iRet != DB_NOTFOUND)
    {
        vtcp_exit("定义活期主文件更新游标失败iRet[%d]",iRet);
        strcpy(g_pub_env.reply,"D230");
        goto ErrExit;
    }
    else if(iRet == DB_NOTFOUND)
    {
        vtcp_exit("没有这个账户");
        strcpy(g_pub_env.reply,"D229");
        goto ErrExit;
    }
    if(sDd_mst.ac_sts[0] != '1')
    {
        vtcp_exit("活期账户账户状态不正常");
        strcpy(g_pub_env.reply,"P183");
        goto ErrExit;
    }
    if(sDd_mst.hold_sts[0] == '1' || sDd_mst.hold_sts[0] == '2'
            || sDd_mst.zf_sts[0] == '1' || sDd_mst.zf_sts[0] == '2')
    {
        vtcp_exit("账户被全部冻结或全部止付或只进不出");
        strcpy(g_pub_env.reply,"D130");
        goto ErrExit;
    }
    /*检查下账户的币种*/
    if(strcmp(get_prdt_cparm(cPrdt_no,"cur_no"),sDd_mst.cur_no) != 0)
    {
        vtcp_exit("币种不支持parm[%s] dd_mst[%s]",get_prdt_cparm(cPrdt_no,"cur_no"),sDd_mst.cur_no);
        strcpy(g_pub_env.reply,"D337");
        goto ErrExit;
    }
    Dd_mst_Clo_Upd();
    /*根据客户号获取客户级别*/
    iRet = pub_cif_get_ciflvl(sDd_mst.cif_no,cCif_lvl);
    if(iRet)
    {
        vtcp_exit("获取客户级别失败[%d]",iRet);
        goto ErrExit;
    }
    vtcp_trace("客户级别[%s]客户类型[%s]",cCif_lvl,cCif_type);
    
    /*理财产品状态*/
    if(strcmp(get_prdt_cparm(cPrdt_no, "prdt_sts"),"6") != 0)
    {
        vtcp_exit("该产品非预约状态[%s] ",get_prdt_cparm(cPrdt_no, "prdt_sts"));
        strcpy(g_pub_env.reply,"D395");
        goto ErrExit;
    }
    /*检查是否在预约期*/
    if(g_pub_env.tx_date >= get_prdt_lparm(cPrdt_no, "coll_beg_date"))
    {
        vtcp_exit("产品已经过了预约期");
        strcpy(g_pub_env.reply,"D394");
        goto ErrExit;
    }
    /*验证现转*/
    if(strcmp(get_prdt_cparm(cPrdt_no,"bdpst_cash_yn"),"N") == 0 && cCt_ind[0] == '1')
    {
        vtcp_exit("该产品不能现金购买");
        strcpy(g_pub_env.reply,"D392");
        goto ErrExit;
    }
    if(strcmp(get_prdt_cparm(cPrdt_no,"bdpst_tran_yn"),"N") == 0 && cCt_ind[0] == '2')
    {
        vtcp_exit("该产品不能转账购买");
        strcpy(g_pub_env.reply,"D393");
        goto ErrExit;
    }
    /*认购渠道 todo*/
    /*认购时间的控制，必须在有效营业时间内*/
    memcpy(cOpn_time,get_prdt_cparm(cPrdt_no,"bopen_time"),sizeof(cOpn_time)-1);
    memcpy(cClos_time,get_prdt_cparm(cPrdt_no,"bclose_time"),sizeof(cClos_time)-1);
    if(g_pub_env.tx_time < atol(cOpn_time) || g_pub_env.tx_time > atol(cClos_time))
    {
        vtcp_exit("不在产品的营业时间内");
        strcpy(g_pub_env.reply,"D396");
        goto ErrExit;
    }
    /*验证客户类型*/
    if(strcmp(get_prdt_cparm(cPrdt_no,"cif_type"),"1") == 0 && cCif_type[0] != '1')/*个人*/
    {
        vtcp_exit("客户类型不符[%s] ",cCif_type);
        strcpy(g_pub_env.reply,"P336");
        goto ErrExit;
    }
    else if(strcmp(get_prdt_cparm(cPrdt_no, "cif_type"),"2") == 0 && cCif_type[0] != '2')/*对公*/
    {
        vtcp_exit("客户类型不符[%s] ",cCif_type);
        strcpy(g_pub_env.reply,"P336");
        goto ErrExit;
    }
    else if(strcmp(get_prdt_cparm(cPrdt_no, "cif_type"),"3") == 0 && cCif_type[0] != '4')/*同业*/
    {
        vtcp_exit("客户类型不符[%s] ",cCif_type);
        strcpy(g_pub_env.reply,"P336");
        goto ErrExit;
    }
    /*获取该机构配额*/
    iRet = pub_de_FnQuota(cPrdt_no,dTx_amt,0);
    if(iRet)
    {
        vtcp_exit("处理配额错误iRet[%d]",iRet);
        goto ErrExit;
    }
    /*是否满足最低认购额*/
    if(cCif_type[0] == '1' && pub_num_amtcmp(dTx_amt,get_prdt_dparm(cPrdt_no,"bdpst_pmin_amt")) < 0)
    {
        vtcp_exit("低于最低认购金额[%lf][%lf]",dTx_amt,get_prdt_dparm(cPrdt_no,"bdpst_pmin_amt"));
        strcpy(g_pub_env.reply,"D338");
        goto ErrExit;
    }
    else if(pub_num_amtcmp(dTx_amt,get_prdt_dparm(cPrdt_no,"bdpst_cmin_amt")) < 0)
    {
        vtcp_exit("低于最低认购金额[%lf][%lf]",dTx_amt,get_prdt_dparm(cPrdt_no,"bdpst_cmin_amt"));
        strcpy(g_pub_env.reply,"D338");
        goto ErrExit;
    }
    /*是否高于最高认购额*/
    if(cCif_type[0] == '1' && pub_num_amtcmp(dTx_amt,get_prdt_dparm(cPrdt_no,"bdpst_pmax_amt")) > 0)
    {
        vtcp_exit("高于最高认购金额[%lf][%lf]",dTx_amt,get_prdt_dparm(cPrdt_no,"bdpst_pmax_amt"));
        strcpy(g_pub_env.reply,"D391");
        goto ErrExit;
    }
    else if(pub_num_amtcmp(dTx_amt,get_prdt_dparm(cPrdt_no,"bdpst_cmax_amt")) > 0)
    {
        vtcp_exit("高于最高认购金额[%lf][%lf]",dTx_amt,get_prdt_dparm(cPrdt_no,"bdpst_cmax_amt"));
        strcpy(g_pub_env.reply,"D391");
        goto ErrExit;
    }
    /*对公户 销客户凭证*/
    if(cCif_type[0]  == '2' && strcmp(cNote_type,"299") != 0)
    {
        iRet = pub_com_CheqClsNo(g_mdm_ac_rel.ac_id,iAc_seqn, cNote_type,cNote_no,cNote_no);
        if(iRet)
        {
            vtcp_exit("销记客户凭证失败了[%d]",iRet);
            goto ErrExit;
        }
    }
    /*单位递增额的校验todo*/
    /*获取收益浮动比例 todo*/
    strcpy(sFn_appbuy_reg.flt_type,"");
    sFn_appbuy_reg.flt_radio = dFlt_radio;
    /*给购买登记簿各个字段赋值*/
    sFn_appbuy_reg.app_amt  = dTx_amt;
    sFn_appbuy_reg.share_amt = dTx_amt / (get_prdt_dparm(cPrdt_no,"unit_amt"));
    /*获取产品预期收益率 todo*/
    sFn_appbuy_reg.rate = 0.00;
    /*计算预期收益*/
    if(sFn_appbuy_reg.flt_type[0] == '1')/*浮点上浮*/
    {
        sFn_appbuy_reg.intst=sFn_appbuy_reg.app_amt * get_prdt_iparm(cPrdt_no,"term")
                         * (sFn_appbuy_reg.rate + sFn_appbuy_reg.flt_radio)/get_prdt_iparm(cPrdt_no,"intst_rate_mode")/L100;
    }
    else if(sFn_appbuy_reg.flt_type[0] == '2')/*浮比上浮*/
    {
        sFn_appbuy_reg.intst=sFn_appbuy_reg.app_amt * get_prdt_iparm(cPrdt_no,"term")
                         * sFn_appbuy_reg.rate * (1+ sFn_appbuy_reg.flt_radio)/get_prdt_iparm(cPrdt_no,"intst_rate_mode")/L100;
    }
    sFn_appbuy_reg.intst = pub_num_dround(sFn_appbuy_reg.intst);
    set_zd_double("0400",sFn_appbuy_reg.intst);
    set_zd_double("0960",sFn_appbuy_reg.flt_radio);
    sFn_appbuy_reg.reg_date = g_pub_env.tx_date;
    sFn_appbuy_reg.reg_trace=g_pub_env.trace_no;
    strcpy(sFn_appbuy_reg.reg_br_no,g_pub_env.tx_br_no);
    strcpy(sFn_appbuy_reg.reg_tel,g_pub_env.tel);
    strcpy(sFn_appbuy_reg.chnl_type,g_pub_rule.chnl_type);
    sFn_appbuy_reg.nat_date=pub_time_date();
    sFn_appbuy_reg.reg_time=g_pub_env.tx_time;
    strcpy(sFn_appbuy_reg.prdt_no,cPrdt_no);
    sFn_appbuy_reg.de_ac_id = sDd_mst.ac_id;
    sFn_appbuy_reg.de_ac_seqn = sDd_mst.ac_seqn;
    sFn_appbuy_reg.cif_no = sDd_mst.cif_no;
    strcpy(sFn_appbuy_reg.cif_type,cCif_type);
    strcpy(sFn_appbuy_reg.note_type,cNote_type);
    strcpy(sFn_appbuy_reg.note_no,cNote_no);
    strcpy(sFn_appbuy_reg.fn_note_no,cFn_note_no);
    strcpy(sFn_appbuy_reg.pact_no,cPact_no);
    strcpy(sFn_appbuy_reg.agent_id_type,cAgent_id_type);
    strcpy(sFn_appbuy_reg.agent_id_no,cAgent_id_no);
    strcpy(sFn_appbuy_reg.agent_name,cAgent_name);
    strcpy(sFn_appbuy_reg.ct_ind,cCt_ind);
    if(cCt_ind[0] == '1')
    {
        strcpy(sFn_appbuy_reg.pay_yn,"Y");
        strcpy(sFn_appbuy_reg.fee_pay_yn,"Y");
    }
    else
    {
        strcpy(sFn_appbuy_reg.pay_yn,"N");/*转账购买的都等成立再扣钱*/
        strcpy(sFn_appbuy_reg.fee_pay_yn,"N");/*手续费是否已扣todo*/
    }
    sFn_appbuy_reg.fee_amt = 0.00;/*手续费 todo*/
    sFn_appbuy_reg.curr_issue = get_prdt_iparm(cPrdt_no,"curr_issue");/*认购的都是从1开始的*/
    strcpy(sFn_appbuy_reg.app_knd,"1");
    strcpy(sFn_appbuy_reg.brf,"预约理财产品");
    strcpy(sFn_appbuy_reg.sts,"0");
    iRet = Fn_appbuy_reg_Ins(sFn_appbuy_reg,g_pub_env.reply);
    if(iRet)
    {
        vtcp_exit("登记理财产品登记簿错误iRet[%d]",iRet);
        strcpy(g_pub_env.reply,"D331");
        goto ErrExit;
    }

    /*登记流水*/
    strcpy(g_pub_tx.ac_no,cAc_no);
    strcpy(g_pub_tx.brf,"理财产品预约");
    g_pub_tx.tx_amt1 = dTx_amt;
    strcpy(g_pub_tx.acc_ind,"0");
    if( pub_ins_trace_log() )
    {
        vtcp_exit("登记交易流水出错");
        strcpy(g_pub_env.reply,"P431");
        goto ErrExit;
    }
    if(cCt_ind[0] == '1')
    {
        strcpy(g_pub_env.reply,"0001");/*现金的下一个流程要收取现金*/
    }
    else
    {
        strcpy(g_pub_env.reply,"0000");
    }
    vtcp_exit("预约成功[%s]",g_pub_env.reply);
    return 0;
ErrExit:
    vtcp_exit("认购失败");
    return 1;
}
#if 0
/*得到特殊客户的收益上浮比例,判断客户类型和账号是否一致*/
int get_flt_radio(char *prdt_code,char *radio_type,double tx_amt,char * cif_lvl,long ac_id,double *flt_radio)
{
    int iRet = 0;
    double dBuy_amt = 0.00;
    struct fn_flt_radio_parm_c sFn_flt_rp;
    memset(&sFn_flt_rp,0x00,sizeof(sFn_flt_rp));
    /*取这个产品的上浮规则*/
    if(radio_type[0] == '1')
    {
        /*客户级别*/
        iRet = Fn_flt_radio_parm_Sel(g_pub_env.reply,&sFn_flt_rp,"prdt_code='%s' and radio_type ='1' and cif_lvl1 <='%s' and cif_lvl2 >='%s'",prdt_code,cif_lvl,cif_lvl);
        if(iRet != DB_NOTFOUND && iRet != DB_OK)
        {
            vtcp_exit("查询理财产品浮动利率表错误!iRet[%d]",iRet);
            strcpy(g_pub_env.reply, "D333");
            goto ErrExit;
        }
        else if(iRet == DB_NOTFOUND)
        {
            vtcp_exit("理财产品浮动利率表未配置该区段上浮利率!iRet[%d]",iRet);
            strcpy(g_pub_env.reply, "D343");
            goto ErrExit;
        }
        else
        {
            *flt_radio = sFn_flt_rp.flt_radio1;
        }
    }
    else if(radio_type[0] == '2')
    {
        /*单次认购*/
        iRet = Fn_flt_radio_parm_Sel(g_pub_env.reply,&sFn_flt_rp,"prdt_code='%s' and radio_type ='2' and buy_amt1 <=%lf and buy_amt2 >=%lf",prdt_code,tx_amt,tx_amt);
        if(iRet != DB_NOTFOUND && iRet != DB_OK)
        {
            vtcp_exit("查询理财产品浮动利率表错误!iRet[%d]",iRet);
            strcpy(g_pub_env.reply, "D333");
            goto ErrExit;
        }
        else if(iRet == DB_NOTFOUND)
        {
            vtcp_exit("理财产品浮动利率表未配置该区段上浮利率!iRet[%d]",iRet);
            strcpy(g_pub_env.reply, "D343");
            goto ErrExit;
        }
        else
        {
            *flt_radio = sFn_flt_rp.flt_radio2;
        }
    }
    else if(radio_type[0] == '3')
    {
        /*认购总额 只算已经认购了的 不算本次的*/
        /*统计这个账号这个产品一共买了多少*/
        iRet = sql_sum_double("fn_reg","tx_amt",&dBuy_amt,"prdt_code='%s' and ac_id=%ld and sts='0'",\
                              prdt_code,ac_id);
        if(iRet)
        {
            vtcp_exit("统计fn_reg错误!");
            strcpy(g_pub_env.reply, "D330");
            goto ErrExit;
        }
        vtcp_trace("已经认购金额[%lf]",dBuy_amt);
        iRet = Fn_flt_radio_parm_Sel(g_pub_env.reply,&sFn_flt_rp,"prdt_code='%s' and radio_type ='3' and total_amt1 <=%lf and total_amt2 >= %lf",prdt_code,dBuy_amt,dBuy_amt);
        if(iRet != DB_NOTFOUND && iRet != DB_OK)
        {
            vtcp_exit("查询理财产品浮动利率表错误!iRet[%d]",iRet);
            strcpy(g_pub_env.reply, "D333");
            goto ErrExit;
        }
        else if(iRet == DB_NOTFOUND)
        {
            vtcp_exit("理财产品浮动利率表未配置该区段上浮利率!iRet[%d]",iRet);
            strcpy(g_pub_env.reply, "D343");
            goto ErrExit;
        }
        else
        {
            *flt_radio = sFn_flt_rp.flt_radio3;
        }
    }
    else
    {
        vtcp_exit("错误的浮动方式! radio_type[%s]",radio_type);
        strcpy(g_pub_env.reply, "S035");
        goto ErrExit;
    }
    vtcp_trace("上浮比例[%lf]",*flt_radio);
    strcpy(g_pub_env.reply,"0000");
    vtcp_exit("获取上浮比例成功");
    return 0;
ErrExit:
    vtcp_exit("获取上浮比例失败");
    return 1;
}
#endif

/*检查账号的介质状态*/
static int _check_note_sts(char *note_sts)
{
    /*检查介质*/
    if (note_sts[0] == '1')
    {
        vtcp_exit( "该介质已挂失!");
        strcpy(g_pub_env.reply, "P126");
        goto ErrExit;
    }
    else if (note_sts[0] == '2')
    {
        vtcp_exit( "该介质已挂失换证!");
        strcpy(g_pub_env.reply, "P108");
        goto ErrExit;
    }
    else if (note_sts[0] == '3')
    {
        vtcp_exit( "该介质已正常换证!");
        strcpy(g_pub_env.reply, "P110");
        goto ErrExit;
    }
    else if (note_sts[0] == '*')
    {
        vtcp_exit("该介质已销户!");
        strcpy(g_pub_env.reply, "P109");
        goto ErrExit;
    }
    else if (note_sts[0] == '4')
    {
        vtcp_exit( "该介质已做部提，请输入新帐号!");
        strcpy(g_pub_env.reply, "P115");
        goto ErrExit;
    }
    else if(note_sts[0] != '0')
    {
        vtcp_exit( "介质状态不正常!");
        strcpy(g_pub_env.reply, "P107");
        goto ErrExit;
    }
    return 0;
ErrExit:
    return 1;
}
