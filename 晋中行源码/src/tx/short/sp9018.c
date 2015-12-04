/*********************************************************************
* 版 本 号 :  V1.0
* 程序作者 :  邵理广
* 日    期 :  2012/11/23 11:22:21
* 所属模块 :  理财产品信息查询
* 程序名称 :  sp9018.c
* 程序作用 :
* 函数列表 :
* 使用注意 :
* 修改记录 :
*     修改作者 : shangyongchao
*     修改时间 : 2013/5/17 8:43:28
*     修改内容 : 改成新的理财产品需要的短交易，原来的不要了
交易配置：
insert into tx_def values ('8018', '理财产品信息查询', '10000000000000110000110010010011111100111111111110011111100001110111111100000000000111000000100011111110000000000000000000000000', '0','', '4112', '2', '3', null);
insert into tx_flow_def values ('8018', 0, '9018', '#$');
insert into tx_sub_def values ('9018', '理财产品信息查询', 'sp9018', '0', '0200000000');
***********************************************************************/
#define EXTERN
#include "public.h"
#include "fn_parm_c.h"
#include "fn_reg_c.h"
#include "fn_br_quota_parm_c.h"
#include "cif_ass_reg_c.h"
#include "mdm_ac_rel_c.h"
#include "dd_mst_c.h"
#include "dd_parm_c.h"
#include "cif_basic_inf_c.h"
#include "prdt_ac_id_c.h"
#include "fn_ac_rel_c.h"
#include "fn_flt_radio_parm_c.h"
#include "mo_prdt_prot_c.h"

#define L365 365

int sp9018()
{
    char cTxCode[11];/*交易代码*/
    
    memset(cTxCode,0x00,sizeof(cTxCode));
    
    /**初始化公用结构**/
    if (pub_base_sysinit())
    {
        vtcp_log("初始化公用结构错误!!");
        strcpy(g_pub_tx.reply, "CU06");
        goto ErrExit;
    }
    
    /*获取代码进行不同分支*/
    get_zd_data("0650",cTxCode);
    vtcp_log("获取的代码为 cTxCode = %s ",cTxCode);
    
    if(memcmp(cTxCode,"0001",4) == 0)
    {
        /*获取指定理财产品的详细信息*/
        if(tx_0001())
        {
            vtcp_log("根据产品种类，返回产品金额错误");
            goto ErrExit;
        }
    }
    else if(memcmp(cTxCode,"0002",4) == 0)
    {
        /*获取处于募集期且是认购状态的产品返回前台生成产品列表*/
        if(tx_0002())
        {
            vtcp_log("理财产品返回参数表里的产品种类错误");
            goto ErrExit;
        }
    }
    else if(memcmp(cTxCode,"0003",4) == 0)
    {
        /*根据账号查询客户的风险等级 账号信息*/
        if(tx_0003())
        {
            vtcp_log("查询账号信息和客户风险等级");
            goto ErrExit;
        }
    }
    else if(memcmp(cTxCode,"0004",4) == 0)
    {
        /*根据账号查询客户认购信息*/
        if(tx_0004())
        {
            vtcp_log("根据账号返回账号认购的理财产品信息");
            goto ErrExit;
        }
    }
    else if(memcmp(cTxCode,"0005",4) == 0)
    {
        /*根据状态返回相应的产品代码*/
        if(tx_0005())
        {
            vtcp_log("查询理财产品错误");
            goto ErrExit;
        }
    }
    else if(memcmp(cTxCode,"0006",4) == 0)
    {
        /*根据输入信息计算预期总收益*/
        if(tx_0006())
        {
            vtcp_log("计算总收益错误");
            goto ErrExit;
        }
    }
    else if(memcmp(cTxCode,"0007",4) == 0)
    {
        if(tx_0007())
        {
            vtcp_log("查询机构限额信息错误");
            goto ErrExit;
        }
    }
    else if(memcmp(cTxCode,"0008",4) == 0)/*查询这个账号是否认购在募集期的理财产品 1 有   0 无*/
    {
        if(tx_0008())
        {
            vtcp_log("查询账号是否认购理财产品错误");
            goto ErrExit;
        }
    }
    else if(memcmp(cTxCode,"0009",4) == 0)/*查询这个账号是否认购在募集期的特定产品*/
    {
        if(tx_0009())
        {
            vtcp_log("查询账号是否认购特定理财产品错误");
            goto ErrExit;
        }
    }
    else if(memcmp(cTxCode,"0010",4) == 0)/*查询帐号认购的且已经扣款的产品*/
    {
        if(tx_0010())
        {
            vtcp_log("查询账号认购理财产品已扣款错误");
            goto ErrExit;
        }
    }
    else if(memcmp(cTxCode,"0011",4) == 0)/*统计认购理财产品总额和赎回金额*/
    {
        if(tx_0011())
        {
            vtcp_log("统计认购理财产品总额和赎回金额错误");
            goto ErrExit;
        }
    }
    else
    {
        vtcp_log("交易代码错误Fd65[%s]",cTxCode);
        strcpy(g_pub_tx.reply, "S090");
        goto ErrExit;
    }

OkExit:
    strcpy(g_pub_tx.reply, "0000");
    vtcp_log("success !!: reply is[%s]", g_pub_tx.reply);
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 0;
ErrExit:
    vtcp_log("fail !!: reply is[%s]", g_pub_tx.reply);
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 1;
}
/***************************
根据产品种类，返回产品金额
****************************/
static int tx_0001()
{
    int ret  =0;
    char cPrdt_code[21];
    char cTerm_type[3];
    char cSts[2];
    double dTx_amt = 0.00;
    double dRedempt_amt = 0.00;
    double dBuying_amt = 0.00;
    struct fn_parm_c sFn_parm;
    struct fn_flt_radio_parm_c sFn_flt_radio_parm;
    struct fn_br_quota_parm_c sFn_br_qp;
    struct fn_ac_rel_c sFn_ac_rel;/*查询理财清算账号——2013040306 wjl*/
    
    memset(cPrdt_code,0x00,sizeof(cPrdt_code));
    memset(cTerm_type,0x00,sizeof(cTerm_type));
    memset(cSts,0x00,sizeof(cSts));
    

    get_zd_data("0620",cPrdt_code);
    memset(&sFn_br_qp,0x00,sizeof(sFn_br_qp));
    memset(&sFn_parm,0x00,sizeof(sFn_parm));
    memset(&sFn_flt_radio_parm,0x00,sizeof(sFn_flt_radio_parm));
    ret=Fn_parm_Sel(g_pub_tx.reply,&sFn_parm,"prdt_code='%s'",cPrdt_code);
    if( ret==100 )
    {
        vtcp_log("fatal error! table no record ");
        strcpy(g_pub_tx.reply,"T062");
        goto ErrExit;
    }
    else if( ret )
    {
        vtcp_log("database error! ret[%d] ",ret);
        strcpy(g_pub_tx.reply,"D324");
        goto ErrExit;
    }
   /**2014040701——wjl 回显浮动比率*/ 
    ret=Fn_flt_radio_parm_Sel(g_pub_tx.reply,&sFn_flt_radio_parm,"prdt_code='%s'",cPrdt_code);
    if(ret)
		{sprintf(acErrMsg,"查询理财产品参数表错误![%s]",g_pub_tx.reply);
		WRITEMSG
			strcpy(g_pub_tx.reply,"D334");
			goto ErrExit;
		}
    set_zd_double("0400",sFn_flt_radio_parm.buy_amt);
    set_zd_double("1013",sFn_flt_radio_parm.buy_amt1);
    set_zd_double("015B",sFn_flt_radio_parm.buy_amt2);
    set_zd_double("1004",sFn_flt_radio_parm.buy_amt3);
    set_zd_double("015C",sFn_flt_radio_parm.buy_amt4);
    vtcp_log("aaaaaaaaaaaaaa[%f][%f]",sFn_flt_radio_parm.buy_amt4,sFn_flt_radio_parm.buy_amt2);
    sFn_flt_radio_parm.flt_radio2=sFn_flt_radio_parm.flt_radio2*100;
    set_zd_double("0640",sFn_flt_radio_parm.flt_radio2);
    sFn_flt_radio_parm.flt_radio21=sFn_flt_radio_parm.flt_radio21*100;
    set_zd_double("0340",sFn_flt_radio_parm.flt_radio21);
    sFn_flt_radio_parm.flt_radio22=sFn_flt_radio_parm.flt_radio22*100;
    set_zd_double("0350",sFn_flt_radio_parm.flt_radio22);
    sFn_flt_radio_parm.flt_radio23=sFn_flt_radio_parm.flt_radio23*100;
    set_zd_double("0360",sFn_flt_radio_parm.flt_radio23);
    sFn_flt_radio_parm.flt_radio24=sFn_flt_radio_parm.flt_radio24*100;
    set_zd_double("0480",sFn_flt_radio_parm.flt_radio24);
    /**end 20140701**/
    set_zd_data("0250",sFn_parm.title);
    set_zd_data("0670",sFn_parm.prdt_type);
    set_zd_data("0680",sFn_parm.intst_type);
    set_zd_long("0440",sFn_parm.beg_date);
    set_zd_long("0450",sFn_parm.end_date);
    set_zd_long("0460",sFn_parm.ic_date);
    set_zd_long("0470",sFn_parm.mtr_date);
    pub_base_dic_show_str(cTerm_type,"term_type",sFn_parm.term_type);
    set_zd_data("0220",cTerm_type);
    set_zd_long("0280",sFn_parm.term);
    set_zd_data("0700",sFn_parm.cif_type);/*客户类型：0都可，1个人，2对公*/
    set_zd_data("0690",sFn_parm.risk_lvl);
    set_zd_data("0720",sFn_parm.invest_type);
    set_zd_data("0210",sFn_parm.cur_no);
    set_zd_data("0990",sFn_parm.redempt);/*LJ*/
    /*set_zd_long("0540",sFn_parm.fre_date);LJ*/
    /*set_zd_long("0550",sFn_parm.fre_term);LJ*/
    /*set_zd_data("0980",sFn_parm.fre_type);LJ*/
    /*set_zd_long("0530",sFn_parm.fre_num);LJ*/
    set_zd_double("0840",sFn_parm.rate);
    set_zd_double("0850",sFn_parm.chrg_rate);
    set_zd_double("1005",sFn_parm.trust_rate);
    set_zd_double("0390",sFn_parm.sale_amt);
    vtcp_log(" 卖出去的金额是： 【 %.2lf 】",sFn_parm.sale_amt);
    set_zd_double("1002",sFn_parm.add_amt);
    set_zd_double("1003",sFn_parm.min_amt);
    set_zd_double("0410",sFn_parm.per_min_amt);
    set_zd_double("0420",sFn_parm.com_min_amt);
    set_zd_data("0930",sFn_parm.cancel_flag);
    set_zd_data("0710",sFn_parm.float_rate_flag);
    set_zd_data("0660",sFn_parm.stat);
    set_zd_data("0310",sFn_parm.com_ac_no);
    set_zd_data("0320",sFn_parm.per_ac_no);
    /*add by lwb */
    set_zd_double("0560",sFn_parm.fct_rate);
    vtcp_log(" 实际利率是 【 %.2lf 】",sFn_parm.fct_rate);
    set_zd_long("0860",sFn_parm.return_date);
    set_zd_double("0570",sFn_parm.capital_percent);
    vtcp_log("capital_percent = %.2lf",sFn_parm.capital_percent);
    /*add end*/
    ret=Fn_ac_rel_Sel(g_pub_tx.reply,&sFn_ac_rel,"prdt_code='%s'",cPrdt_code);
    if( ret==100 )
    {
        vtcp_log("fatal error! table no record ");
        strcpy(g_pub_tx.reply,"T062");
        goto ErrExit;
    }
    else if( ret )
    {
        vtcp_log("database error! ret[%d] ",ret);
        strcpy(g_pub_tx.reply,"D324");
        goto ErrExit;
    }
    set_zd_data("0330",sFn_ac_rel.qs_ac_no);
    /*stat为0时是已募集金额，stat为1时是正在运行金额*/
    if(sFn_parm.stat[0] == '1' || sFn_parm.stat[0] == '0')
    {
        cSts[0] = sFn_parm.stat[0];
    }
    else if(sFn_parm.stat[0] == '4')
    {
        cSts[0] = '0';
    }
    else if(sFn_parm.stat[0] == '6')
    {
        cSts[0] = '1';
    }
    vtcp_log("  fn_parm.stat= %s ",sFn_parm.stat);
    ret=sql_sum_double("fn_reg","tx_amt",&dTx_amt,"prdt_code='%s' and sts='%s'",sFn_parm.prdt_code,sFn_parm.stat);
    if( ret )
    {
        vtcp_log("sql_sum_double error! ret[%d] ",ret);
        strcpy(g_pub_tx.reply,"D331");
        goto ErrExit;
    }
    set_zd_double("0430",dTx_amt);/*已经认购的总额*/
    /*获取该机构配额*/
    if(sFn_parm.quota_flag[0] == 'Y')
    {
        /*检查本机构理财产品是否能认购*/
        ret = Fn_br_quota_parm_Sel(g_pub_tx.reply,&sFn_br_qp,"prdt_code='%s' and br_no='%s'",\
            sFn_parm.prdt_code,g_pub_tx.tx_br_no);
        if(ret != 0 && ret != 100)
        {
            vtcp_log("查询理财产品配额参数表失败iRet[%d]",ret);
            strcpy(g_pub_tx.reply,"D327");
            goto ErrExit;
        }
        else if(ret == 100)
        {
            /*这个机构没配额，取全行*/
            if(sFn_parm.quota_flag[1] == 'Y')
            {
                /*统计总行已经赎回的总额 只有状态是1的时候才有赎回和申购*/
                if(sFn_parm.stat[0] == '1')
                {
                    /*正在运行的 申购时还有申购中的金额*/
                    ret = sql_sum_double("fn_reg","redempt_amt",&dRedempt_amt,"prdt_code='%s' and sts='%s'",\
                        sFn_parm.prdt_code,sFn_parm.stat);
                    if(ret)
                    {
                        vtcp_log("统计fn_reg错误!");
                        strcpy(g_pub_tx.reply, "D331");
                        goto ErrExit;
                    }
                    ret = sql_sum_double("fn_reg","tx_amt",&dBuying_amt,"prdt_code='%s' and sts='4'",\
                        sFn_parm.prdt_code);
                    if(ret)
                    {
                        vtcp_log("统计fn_reg错误!");
                        strcpy(g_pub_tx.reply, "D331");
                        goto ErrExit;
                    }
                }
                vtcp_log("dRedempt_amt[%lf]",dRedempt_amt);
                vtcp_log("dBuying_amt[%lf]",dBuying_amt);
                set_zd_double("1001",sFn_parm.sale_amt - (dTx_amt - dRedempt_amt) - dBuying_amt);
            }
            else
            {
                set_zd_double("1001",0.00);
            }
        }
        else
        {
            set_zd_double("1001",sFn_br_qp.available_amt);
        }
    }
    else/*产品不设置配额，取全行的*/
    {
        /*统计总行已经赎回的总额 只有状态是1的时候才有赎回和申购*/
        if(sFn_parm.stat[0] == '1')
        {
            ret = sql_sum_double("fn_reg","redempt_amt",&dRedempt_amt,"prdt_code='%s' and sts='%s'",\
                sFn_parm.prdt_code,sFn_parm.stat);
            if(ret)
            {
                vtcp_log("统计fn_reg错误!");
                strcpy(g_pub_tx.reply, "D331");
                goto ErrExit;
            }
            ret = sql_sum_double("fn_reg","tx_amt",&dBuying_amt,"prdt_code='%s' and sts='4'",\
                sFn_parm.prdt_code);
            if(ret)
            {
                vtcp_log("统计fn_reg错误!");
                strcpy(g_pub_tx.reply, "D331");
                goto ErrExit;
            }
        }
        vtcp_log("dRedempt_amt[%lf]",dRedempt_amt);
        vtcp_log("dBuying_amt[%lf]",dBuying_amt);
        set_zd_double("1001",sFn_parm.sale_amt - (dTx_amt - dRedempt_amt) - dBuying_amt);
    }
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    vtcp_log("success !!: reply is[%s]", g_pub_tx.reply);
    return 0;
ErrExit:
    vtcp_log("fail !!: reply is[%s]", g_pub_tx.reply);
    return 1;
}
/***************************
为理财产品返回参数表里的产品种类
****************************/
static int tx_0002()
{
    int ret = 0;
    int iCnt = 0;
    char cFilename[256];
    FILE *fp = NULL;
    struct fn_parm_c sFn_parm;
    char cFlag[2];  /*为1时要查已分配客户收益的*/
    
    memset(cFlag,0x00,sizeof(cFlag));
    memset(cFilename,0x00,sizeof(cFilename));
    
    get_zd_data("0680",cFlag);
    /* 生成文件 */
    pub_base_AllDwnFilName(cFilename);
    fp=fopen(cFilename,"w");
    if( fp==NULL )
    {
        vtcp_log("open file [%s] error ",cFilename);
        strcpy(g_pub_tx.reply,"SM100");
        goto ErrExit;
    }
    /*ret=Fn_parm_Dec_Sel(g_pub_tx.reply,"(chnl_type ='****'or chnl_type ='%s') and beg_date <= %ld and end_date >= %ld and stat='0'",\
                g_pub_rule.chnl_type,g_pub_tx.tx_date,g_pub_tx.tx_date);*/
   	
    if(cFlag[0] == '1')
    {
    	 ret=Fn_parm_Dec_Sel(g_pub_tx.reply," stat='2' ");    	
    }
    else 
    {
    	ret=Fn_parm_Dec_Sel(g_pub_tx.reply,"beg_date <= %ld and end_date >= %ld and stat='0'",\
                g_pub_tx.tx_date,g_pub_tx.tx_date);         
		}
		   
   if( ret )
    {
        vtcp_log("declare cursor error! ret[%d] ",ret);
        strcpy( g_pub_tx.reply,"D324" );
        goto ErrExit;
    }
    while(1)
    {
        memset(&sFn_parm,0x00,sizeof(sFn_parm));
        ret=Fn_parm_Fet_Sel(&sFn_parm,g_pub_tx.reply);
        if( ret==100 )
        {
            vtcp_log("fn_parm fetch over! ");
            break;
        }
        else if( ret )
        {
            vtcp_log("fn_parm fetch error! ret[%d] ",ret);
            strcpy(g_pub_tx.reply,"D324");
            goto ErrExit;
        }
        fprintf(fp,"9018|%-60s|%-20s|\n",sFn_parm.title,sFn_parm.prdt_code);
        iCnt ++;
    }
    Fn_parm_Clo_Sel();
    if(iCnt == 0)
    {
        vtcp_log("没有符合条件的理财产品! ret[%d] ",ret);
        strcpy(g_pub_tx.reply,"T062");
        goto ErrExit;
    }

    set_zd_data(DC_FILE_SND, "2");
    fclose(fp);
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    vtcp_log("查询产品种类成功[%s]", g_pub_tx.reply);
    return 0;
ErrExit:
    vtcp_log("查询产品种类失败[%s]", g_pub_tx.reply);
    if(fp != NULL)
    {
        fclose(fp);
    }
    return 1;
}
/*根据账号查询客户信息和风险登记*/
static int tx_0003()
{
    int ret = 0;
    int iAc_seqn = 0;
    long lCif_no = 0;
    char cAc_no[25];
    char cFlag[2];/*特殊标识1只为得到户名2需要客户风险评估信息*/
    double dAv_bal = 0.00;/*可用余额*/
    struct cif_ass_reg_c sCif_ass_reg;
    struct mdm_ac_rel_c sMdm_ac_rel;
    struct dd_mst_c sDd_mst;
    struct dd_parm_c sDd_parm;
    struct cif_basic_inf_c sCif_basic_inf;
    struct prdt_ac_id_c sPrdt_ac_id;
    struct mo_prdt_prot_c sMo_prdt_prot;
    int iFlag = 0;/*是否做了七天通知存款标志*/
    
    memset(&sMo_prdt_prot,0x00,sizeof(sMo_prdt_prot));
    memset(cAc_no,0x00,sizeof(cAc_no));
    memset(cFlag,0x00,sizeof(cFlag));
    memset(&sCif_ass_reg,0x00,sizeof(sCif_ass_reg));
    memset(&sMdm_ac_rel,0x00,sizeof(sMdm_ac_rel));
    memset(&sDd_mst,0x00,sizeof(sDd_mst));
    memset(&sDd_parm,0x00,sizeof(sDd_parm));
    memset(&sCif_basic_inf,0x00,sizeof(sCif_basic_inf));
    memset(&sPrdt_ac_id,0x00,sizeof(sPrdt_ac_id));
    get_zd_data("0300",cAc_no);
    get_zd_data("0670",cFlag);
    get_zd_int("0480",&iAc_seqn);
    ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_no='%s'",cAc_no);
    if(ret != 0 && ret != 100)
    {
        vtcp_log("查询介质账户关系表失败ret[%d]",ret);
        strcpy(g_pub_tx.reply,"W011");
        goto ErrExit;
    }
    else if(ret == 100)
    {
        vtcp_log("没有这个账号");
        strcpy(g_pub_tx.reply,"B117");
        goto ErrExit;
    }
    if(cFlag[0] == '1')
    {
        /*set_zd_data("0250",sMdm_ac_rel.name);*/
        set_zd_data("0020",sMdm_ac_rel.opn_br_no);
        goto OkExit;
    }
    if(sMdm_ac_rel.note_sts[0] == '*')
    {
        vtcp_log( "介质状态不正常!");
        strcpy(g_pub_tx.reply, "M013");
        goto ErrExit;
    }
    if(iAc_seqn  == 0)
    {
        ret = Prdt_ac_id_Sel(g_pub_tx.reply,&sPrdt_ac_id,"ac_id= %ld",sMdm_ac_rel.ac_id);
        if(ret)
        {
            vtcp_log("查询prdt_ac_id失败[%d]",ret);
            strcpy(g_pub_tx.reply,"P144");
            goto ErrExit;
        }
        lCif_no = sPrdt_ac_id.cif_no;
    }
    else
    {
        ret = Dd_mst_Sel(g_pub_tx.reply,&sDd_mst,"ac_id =%ld and ac_seqn = %d",sMdm_ac_rel.ac_id ,iAc_seqn);
        if(ret != 0 && ret != 100)
        {
            vtcp_log("查询活期主文件失败[%d]",ret);
            strcpy(g_pub_tx.reply,"W015");
            goto ErrExit;
        }
        else if(ret == 100)
        {
            vtcp_log("没有这个活期账户");
            strcpy(g_pub_tx.reply,"D162");
            goto ErrExit;
        }
        if(sDd_mst.ac_sts[0] != '1')
        {
            vtcp_log("活期账户账户状态不正常");
            strcpy(g_pub_tx.reply,"O085");
            goto ErrExit;
        }
        /*if(sDd_mst.hold_sts[0] == '1' || sDd_mst.hold_sts[0] == '2'
          || sDd_mst.zf_sts[0] == '1' || sDd_mst.zf_sts[0] == '2')
        {
            vtcp_log("账户被全部冻结或全部止付或只进不出");
            strcpy(g_pub_tx.reply,"D130");
            goto ErrExit;
        }*/
        
        ret = Dd_parm_Sel(g_pub_tx.reply,&sDd_parm,"prdt_no='%s'",sDd_mst.prdt_no);
        if(ret)
        {
            vtcp_log("查询活期产品参数表失败");
            strcpy(g_pub_tx.reply,"W025");
            goto ErrExit;
        }
        
        /*dAv_bal = sDd_mst.bal -  sDd_mst.hold_amt - sDd_mst.zf_bal - sDd_mst.ctl_amt - sDd_parm.min_bal;*/
        dAv_bal = sDd_mst.bal -  sDd_mst.hold_amt - sDd_mst.ctl_amt - sDd_parm.min_bal;
        lCif_no = sDd_mst.cif_no;
  	/*add by lwb  20140925  检查下该账户是否做了七天通知存款业务*/
  	ret = Mo_prdt_prot_Sel(g_pub_tx.reply,&sMo_prdt_prot,"main_ac_id = %d and prot_type ='TK7' and sts !='*' ",sMdm_ac_rel.ac_id);
  	if(ret&&ret!=100)
  	{
        	vtcp_log("查询划款协议登记表失败");
        	strcpy(g_pub_tx.reply,"X013");
        	goto ErrExit;	
  	}
  	else if(ret == 0)
  	{
  		iFlag = 1;	 /*做了七天通知存款置为1*/
  	}
  	/*add end*/
    }

    if(cFlag[0] == '2')
    {
        /*查询客户风险等级*/
        ret =  Cif_ass_reg_Sel(g_pub_tx.reply,&sCif_ass_reg,"cif_no=%ld",lCif_no);
        if(ret != 0 && ret != 100)
        {
            vtcp_log("查询客户风险等级评估表错误[%d]",ret);
            strcpy(g_pub_tx.reply,"C150");
            goto ErrExit;
        }
        else if(ret == 100)
        {
            vtcp_log("客户没有进行风险等级评估");
            strcpy(g_pub_tx.reply,"C151");
            goto ErrExit;
        }
    }

    /*查询客户信息返回客户类型*/
    ret =  Cif_basic_inf_Sel(g_pub_tx.reply,&sCif_basic_inf,"cif_no=%ld",lCif_no);
    if(ret != 0 && ret != 100)
    {
        vtcp_log("查询客户基本信息表失败[%d]",ret);
        strcpy(g_pub_tx.reply,"C113 ");
        goto ErrExit;
    }
    else if(ret == 100)
    {
        vtcp_log("没有该客户的基本信息");
        strcpy(g_pub_tx.reply,"P134");
        goto ErrExit;
    }
    /*往前台返回值*/
    set_zd_int("0340",iFlag);  
    /*set_zd_data("0210",sDd_mst.cur_no);*/
    set_zd_data("0210","1");
    /*set_zd_data("0250",sMdm_ac_rel.name);*/
    set_zd_data("0250",sDd_mst.name);
    set_zd_data("0660",sMdm_ac_rel.note_sts);
    set_zd_data("0670",sMdm_ac_rel.id_type);
    set_zd_data("0620",sMdm_ac_rel.id_no);
    set_zd_data("0020",sMdm_ac_rel.opn_br_no);
    set_zd_double("0390",sDd_mst.bal);
    set_zd_double("0400",dAv_bal);
    set_zd_data("0680",sCif_ass_reg.ass_lvl);
    set_zd_long("0440",sCif_ass_reg.ass_date);
    set_zd_long("0450",sCif_ass_reg.end_date);
    set_zd_data("0690",sCif_basic_inf.type);
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    vtcp_log("查询客户信息成功[%s]", g_pub_tx.reply);
    return 0;
ErrExit:
    vtcp_log("查询客户信息失败[%s]", g_pub_tx.reply);
    return 1;
}
/*客户认购信息*/
static int tx_0004()
{
    int ret = 0;
    char cAc_no[25];
    char cFlag[2];/*查询标志*/
    char cSts[12];
    struct fn_parm_c sFn_parm;
    struct fn_reg_c sFn_reg;
    FILE *fp  = NULL;
    char cFilename[256];
    char cWherelist[256];
    char cTerm_type[3];
    
    memset(cAc_no,0x00,sizeof(cAc_no));
    memset(cFilename,0x00,sizeof(cFilename));
    memset(cWherelist,0x00,sizeof(cWherelist));
    memset(cSts,0x00,sizeof(cSts));
    memset(cFlag,0x00,sizeof(cFlag));
    memset(&sFn_parm,0x00,sizeof(sFn_parm));
    memset(&sFn_reg,0x00,sizeof(sFn_reg));
    get_zd_data("0300",cAc_no);
    get_zd_data("0670",cFlag);
    pub_base_AllDwnFilName(cFilename);
    fp=fopen(cFilename,"w");
    if(fp == NULL)
    {
        vtcp_log("打开文件失败[%s]",cFilename);
        strcpy(g_pub_tx.reply,"SM100");
        goto ErrExit;
    }
    /*认购撤销0 赎回1和撤销申购4*/
    if(cFlag[0] == '0' || cFlag[0] == '1' || cFlag[0] == '4')
    {
        /*认购撤销时只查认购状态的就行了*/
        sprintf(cWherelist,"ac_no ='%s' and sts = '%s'",cAc_no,cFlag);
        fprintf(fp,"~认购日期|流水|交易机构|协议编号|产品代码|产品名称|是否能撤销|是否能赎回|期限|金额|预期年化收益率|预期收益|状态|到期日期|\n");
    }
    else
    {
        /*所有的都查出来*/
        sprintf(cWherelist,"ac_no ='%s'",cAc_no);
        fprintf(fp,"~交易日期|主机流水号|交易机构|协议编号|产品代码|认购份数|认购金额|赎回金额|返回本金|利率浮动比例|返还收益利率|预期收益|起息日期|到期日期|收益|分润金额|分润标志|交易摘要|状态|扣款日期|扣款流水号|返还收益日期|返还收益流水号|签约柜员|客户经理号|代理人证件类型|代理人证件|代理人姓名|\n");
    }
    /*循环取记录，不符合条件的跳过*/
    ret = Fn_reg_Dec_Sel(g_pub_tx.reply,"%s",cWherelist);
    if(ret)
    {
        vtcp_log("定义理财登记簿游标错误");
        strcpy(g_pub_tx.reply,"D101");
        goto ErrExit;
    }
    
    while(1)
    {
        memset(&sFn_parm,0x00,sizeof(sFn_parm));
        memset(&sFn_reg,0x00,sizeof(sFn_reg));
        memset(cSts,0x00,sizeof(cSts));
        memset(cTerm_type,0x00,sizeof(cTerm_type));
        
        ret = Fn_reg_Fet_Sel(&sFn_reg,g_pub_tx.reply);
        if(ret == 100)
        {
            break;
        }
        else if(ret != 100 && ret != 0)
        {
            vtcp_log("取理财登记簿游标错误");
            strcpy(g_pub_tx.reply,"D330");
            goto ErrExit;
        }
        ret = Fn_parm_Sel(g_pub_tx.reply,&sFn_parm,"prdt_code='%s'",sFn_reg.prdt_code);
        if(ret)
        {
            vtcp_log("查询理财产品参数错误");
            strcpy(g_pub_tx.reply,"D324");
            goto ErrExit;
        }
        vtcp_log("sFn_parm.per_min_amt = %.2lf",sFn_parm.per_min_amt);
        vtcp_log("sFn_parm.com_min_amt = %.2lf",sFn_parm.com_min_amt);
        set_zd_double("0410",sFn_parm.per_min_amt);
        set_zd_double("0420",sFn_parm.com_min_amt);
        pub_base_dic_show_str(cTerm_type,"term_type",sFn_parm.term_type);
        if(strcmp(cTerm_type,"日") == 0)
        {
            strcpy(cTerm_type,"天");
        }
        pub_base_dic_show_str(cSts,"fn_reg_sts",sFn_reg.sts);
        /*根据不同标志写不同的文件*/
        if(cFlag[0] == '0' || cFlag[0] == '4' || cFlag[0]  == '1')
        {
            
            fprintf(fp,"%ld|%ld|%s|%20s|%20s|%60s|%2s|%2s|%8d%2s|%16.2lf|%9.6lf|%16.2lf|%12s|%ld|\n",\
                    sFn_reg.tx_date,sFn_reg.trace_no,sFn_reg.tx_br_no,sFn_reg.pact_no,sFn_reg.prdt_code,\
                sFn_parm.title,sFn_parm.cancel_flag[0] == 'Y' ?"是":"否",sFn_parm.redempt[0] == 'Y' ? "是":"否",sFn_parm.term,cTerm_type,sFn_reg.tx_amt,sFn_parm.rate,sFn_reg.intst,\
                cSts,sFn_parm.mtr_date);
            vtcp_log("sFn_reg.redempt_amt = %.2lf",sFn_reg.redempt_amt);
            set_zd_double("0430",sFn_reg.redempt_amt);
        }
        else
        {
            fprintf(fp,"%ld|%9ld|%s|%20s|%20s|%9ld|%16.2f|%16.2f|%16.2f|%9.6f|%9.6f|%16.2f|%ld|%ld|%16.2lf|%16.2f|%s|%s|%12s|%ld|%9ld|%ld|%9ld|%s|%s|%s|%s|%s|\n",\
            sFn_reg.tx_date,sFn_reg.trace_no,sFn_reg.tx_br_no,sFn_reg.pact_no,\
            sFn_reg.prdt_code,sFn_reg.count,sFn_reg.tx_amt,sFn_reg.redempt_amt,\
            sFn_reg.return_capital,sFn_reg.flt_radio,sFn_reg.return_rate,sFn_reg.intst,\
            sFn_reg.ic_date,sFn_reg.end_date,sFn_reg.fct_intst,sFn_reg.profit_amt,\
            sFn_reg.profit_flag[0] == 'Y' ?"是":"否",sFn_reg.brf,cSts,sFn_reg.pay_date,\
            sFn_reg.pay_trace_no,sFn_reg.return_date,sFn_reg.return_trace_no,sFn_reg.tel,\
            sFn_reg.mang,sFn_reg.agent_id_type,sFn_reg.agent_id,sFn_reg.agent_name);
        }

    }
    Fn_reg_Clo_Sel();
    set_zd_data(DC_FILE_SND,"2");
OkExit:
    fclose(fp);
    fp = NULL;
    strcpy(g_pub_tx.reply,"0000");
    vtcp_log("查询账号对应的理财产品信息成功[%s]", g_pub_tx.reply);
    return 0;
ErrExit:
    if(fp != NULL)
    {
        fclose(fp);
        fp = NULL;
    }
    vtcp_log("查询账号对应的理财产品信息失败[%s]", g_pub_tx.reply);
    return 1;
}
/*根据传入的状态 返回相应的产品代码*/
static int tx_0005()
{
    int ret = 0;
    char cStat[2];
    char cFilename[256];
    char tmp_date[9];
    char flag1[2];
    FILE *fp = NULL;
    struct fn_parm_c sFn_parm;
    struct fn_parm_c s_Fn_parm;
    
    memset(tmp_date,0x00,sizeof(tmp_date));
    memset(cStat,0x00,sizeof(cStat));
    memset(flag1,0x00,sizeof(flag1));
    memset(cFilename,0x00,sizeof(cFilename));
    memset(&s_Fn_parm,0x00,sizeof(struct fn_parm_c));
    get_zd_data("0670",cStat);
    get_zd_data("0690",flag1);
    get_zd_long("0470",&s_Fn_parm.mtr_date);
    /* 生成文件 */
    pub_base_AllDwnFilName(cFilename);
    fp=fopen(cFilename,"w");
    if( fp==NULL )
    {
        vtcp_log("open file [%s] error ",cFilename);
        strcpy(g_pub_tx.reply,"SM100");
        goto ErrExit;
    }
    vtcp_log("%s %d aaaaaaaaa  cStat=[%s] flag1=[%s]",__FILE__,__LINE__,cStat,flag1);
    if(cStat[0] == 'A')
    {
    	/*判断是否提前终止 add by wjl20140411*/
    	if(flag1[0]=='1')
    		{
    			ret=Fn_parm_Dec_Sel(g_pub_tx.reply,"(stat='1' or stat='6') and mtr_date<='%ld'",s_Fn_parm.mtr_date);
    		vtcp_log("wjl[%ld]",s_Fn_parm.mtr_date);
    		}
    	else if((flag1[0]=='2'))
    		ret=Fn_parm_Dec_Sel(g_pub_tx.reply,"(stat='1' or stat='6') and mtr_date>'%ld'",s_Fn_parm.mtr_date);
        /*到期处理 找正在运行和停止申购的， 停止申购是特殊的运行状态*/
    }
    else if(cStat[0] == 'B')
    {
        /*恢复原认购和申购  找被停止的4和被停止申购的*/
        ret=Fn_parm_Dec_Sel(g_pub_tx.reply,"stat='4' or stat='6'",cStat);
    }
    else if(cStat[0] == 'C') /*募集失败找募集期和暂停的*/
    {
        ret=Fn_parm_Dec_Sel(g_pub_tx.reply,"stat='0' or stat='4'");
    }
    else if(cStat[0] == 'D')
    {
    		/*判断是否提前终止 add by wjl20140411*/
    	if(flag1[0]=='1')
    		{
    			ret=Fn_parm_Dec_Sel(g_pub_tx.reply,"(stat='7') and mtr_date<='%ld'",s_Fn_parm.mtr_date);
    		vtcp_log("[%ld]",s_Fn_parm.mtr_date);
    		}
    	else if((flag1[0]=='2'))
    		ret=Fn_parm_Dec_Sel(g_pub_tx.reply,"(stat='7') and mtr_date>='%ld'",s_Fn_parm.mtr_date);
        /*到期处理 找正在运行和停止申购的， 停止申购是特殊的运行状态*/    	
    }
   /*LJ 20151102start*/
    else if(cStat[0] == 'E') /*开放式产品开放找正在运行的*/
    {
        ret=Fn_parm_Dec_Sel(g_pub_tx.reply,"stat='1' and prdt_type='2'");
    }
    /*LJ end*/
    /*LJ 20151104start*/
    else if(cStat[0] == 'F') /*开放式产品阶段收益分配完成找“8”状态，时间在正在运行阶段的*/
    {
        ret=Fn_parm_Dec_Sel(g_pub_tx.reply,"stat='8' and prdt_type='2' and ic_date<=%ld and mtr_date>%ld",g_pub_tx.tx_date,g_pub_tx.tx_date);
    }
    /*LJ end*/
    /*lpj20151103*/
   
    else if(cStat[0] == 'H') /*开放式产品可以进行提前开放的*/
    {
    	if(flag1[0]=='1')
    	{
        ret=Fn_parm_Dec_Sel(g_pub_tx.reply,"stat='1' and prdt_type='2' and ic_date<=%ld and mtr_date>%ld",g_pub_tx.tx_date,g_pub_tx.tx_date);
    	}
    	else if(flag1[0]=='2')
    	{
    		  ret=Fn_parm_Dec_Sel(g_pub_tx.reply,"stat='7' and prdt_type='2' and ic_date<=%ld and mtr_date>%ld",g_pub_tx.tx_date,g_pub_tx.tx_date);
    	
    	}
    }
    /*lpj20151103新增产品查询*/
    else
    {
        ret=Fn_parm_Dec_Sel(g_pub_tx.reply,"stat='%s'",cStat);
    }
    if( ret )
    {
        vtcp_log("declare cursor error! ret[%d] ",ret);
        strcpy( g_pub_tx.reply,"D324" );
        goto ErrExit;
    }
    while(1)
    {
        memset(&sFn_parm,0x00,sizeof(sFn_parm));
        ret=Fn_parm_Fet_Sel(&sFn_parm,g_pub_tx.reply);
        if( ret==100 )
        {
            vtcp_log("fn_parm fetch over! ");
            break;
        }
        else if( ret )
        {
            vtcp_log("fn_parm fetch error! ret[%d] ",ret);
            strcpy(g_pub_tx.reply,"D324");
            goto ErrExit;
        }
        fprintf(fp,"9018|%-60s|%-20s|\n",sFn_parm.title,sFn_parm.prdt_code);
    }
    Fn_parm_Clo_Sel();

    set_zd_data(DC_FILE_SND, "2");
    fclose(fp);
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    vtcp_log("查询产品种类成功[%s]", g_pub_tx.reply);
    return 0;
ErrExit:
    vtcp_log("查询产品种类失败[%s]", g_pub_tx.reply);
    if(fp != NULL)
    {
        fclose(fp);
    }
    return 1;
}
/*计算器*/
static int tx_0006()
{
    int ret = 0;
    long lCnt = 0;/*每页的行数*/
    long lDate = 0;
    long lDays = 0;
    double dRate = 0.000000;/*利率*/
    double dProfit_rate = 0.000000;/*分润利率*/
    double dIntst = 0.00;
    double dProfit_amt = 0.00;
    double dCapital = 0.00;
    double dTmp_intst = 0.00;
    char cPrdt_code[21];
    char cFlag[2];
    char cSts_list[100];
    char sTmp_rate[10];
    char cOld_br_no[BRNO_LEN +1];
    char cFilename[256];
    FILE *fp = NULL;
    struct fn_parm_c sFn_parm;
    struct fn_reg_c sFn_reg;
    struct total_info
    {
        long cnt;/*笔数*/
        double tx_amt;/*认购金额*/
        double redempt_amt;/*赎回金额*/
        double return_capital1;/*本次返回的本金*/
        double fct_intst1;/*本次实际收益*/
        double profit_amt1;/*本次分润金额*/
        double trust_amt;/*托管费金额*/
        double return_capital2;/*累计返回的本金*/
        double fct_intst2;/*累计实际收益*/
        double profit_amt2;/*累计分润金额*/
    }sTotal_qs,sTotal_jg;
    memset(sTmp_rate,0x00,sizeof(sTmp_rate));
    memset(&sTotal_jg,0x00,sizeof(sTotal_jg));
    memset(&sTotal_qs,0x00,sizeof(sTotal_qs));
    memset(cFilename,0x00,sizeof(cFilename));
    memset(cPrdt_code,0x00,sizeof(cPrdt_code));
    memset(cFlag,0x00,sizeof(cFlag));
    memset(cOld_br_no,'-',sizeof(cOld_br_no));
    memset(cSts_list,0x00,sizeof(cSts_list));
    memset(&sFn_parm,0x00,sizeof(sFn_parm));

    get_zd_data("0660",cFlag);
    get_zd_data("0630",cPrdt_code);
    get_zd_long("0440",&lDate);
     get_zd_data("0730",sTmp_rate);
    /*dRate=atoi(sTmp_rate)/1000000;mod by lwb 20150414*/
    dRate=atoi(sTmp_rate);
    dRate=dRate/1000000;

    get_zd_double("0850",&dProfit_rate);
    
      /* 生成文件 */
    pub_base_AllDwnFilName(cFilename);
    fp=fopen(cFilename,"w");
    if( fp==NULL )
    {
        vtcp_log("open file [%s] error ",cFilename);
        strcpy(g_pub_tx.reply,"SM100");
        goto ErrExit;
    }
    ret=Fn_parm_Sel(g_pub_tx.reply,&sFn_parm,"prdt_code='%s'",cPrdt_code);
    if( ret != 0 && ret != 100)
    {
        vtcp_log("declare cursor error! ret[%d] ",ret);
        strcpy( g_pub_tx.reply,"D324" );
        goto ErrExit;
    }
    else if(ret  == 100)
    {
        vtcp_log("没有这个产品! ret[%d]prdt_code[%s] ",ret,cPrdt_code);
        strcpy( g_pub_tx.reply,"D104" );
        goto ErrExit;
    }
    /*根据产品状态转换成 登记簿的查询条件*/
    if(sFn_parm.stat[0] == '0')
    {
        sprintf(cSts_list," sts = '0'");
    }
    else if(sFn_parm.stat[0] == '1')/*正在运行的 考虑上 开放式的申购*/
    {
        sprintf(cSts_list," (sts = '1' or sts = '4')");
    }
    else if(sFn_parm.stat[0] == '2')
    {
        sprintf(cSts_list," sts = '2'");
    }
    else if(sFn_parm.stat[0] == '3')
    {
        sprintf(cSts_list," sts = '3'");
    }
    else if(sFn_parm.stat[0] == '4')/*暂停发售的 登记簿是认购*/
    {
        sprintf(cSts_list," sts = '0'");
    }
    else if(sFn_parm.stat[0] == '6')/*暂停申购的 登记簿是认购和申购的*/
    {
        sprintf(cSts_list," (sts = '1'  or sts = '4')");
    }
    else if(sFn_parm.stat[0] == '5')/*暂停申购的 登记簿是认购和申购的*/
    {
        sprintf(cSts_list," (sts = '5')");
    }
    else
    {
        vtcp_log("不支持这个状态的产品 stat[%s] ",sFn_parm.stat);
        strcpy( g_pub_tx.reply,"S089" );
        goto ErrExit;
    }
    fprintf(fp,"\n\n\n");
    fprintf(fp, "                                   理财产品[%s]收益测算\n",sFn_parm.prdt_code);
    fprintf(fp, "                                                              日期:%d年%d月%d日\n",g_pub_tx.tx_date/10000,g_pub_tx.tx_date/100%100,g_pub_tx.tx_date%100);
    fprintf(fp, "==============================================================================================================================\n" );
    fprintf(fp, " %-6.6s  %-8.8s%-17.17s%-17.17s%-17.17s%-17.17s\n","机构号","总比数","总客户理财金额","总赎回金额","返还客户本金","总客户收益");
    fprintf(fp, "==============================================================================================================================\n" );
   
    ret = Fn_reg_Dec_Sel(g_pub_tx.reply,"prdt_code='%s'  and %s order by tx_br_no ",sFn_parm.prdt_code,cSts_list);
    if(ret)
    {
        vtcp_log("declare cursor error! ret[%d] ",ret);
        strcpy( g_pub_tx.reply,"D330" );
        goto ErrExit;
    }
    while(1)
    {
        lDays  = 0;
        dTmp_intst = 0.00;
        memset(&sFn_reg,0x00,sizeof(sFn_reg));
        ret =  Fn_reg_Fet_Sel(&sFn_reg,g_pub_tx.reply);
        if( ret != 0 && ret != 100)
        {
            vtcp_log("declare cursor error! ret[%d] ",ret);
            strcpy( g_pub_tx.reply,"D330" );
            goto ErrExit;
        }
        else if(ret == 100)
        {
            /*写最后一个机构的信息*/
            if(lCnt % 40 == 0 && lCnt > 0)
            {
                fprintf(fp,"\f");
            }
            fprintf(fp," %-6.6s  %-8d%-17.2lf%-17.2lf%-17.2lf%-17.2lf\n",\
                    cOld_br_no,sTotal_jg.cnt,sTotal_jg.tx_amt,\
                    sTotal_jg.redempt_amt,sTotal_jg.return_capital1,sTotal_jg.fct_intst1);
            /*写总行*/
            fprintf(fp, "==============================================================================================================================\n" );
            fprintf(fp, " 合计:   %-8d%-17.2lf%-17.2lf%-17.2lf%-17.2lf\n",\
                    sTotal_qs.cnt,sTotal_qs.tx_amt,\
                    sTotal_qs.redempt_amt,sTotal_qs.return_capital1,sTotal_qs.fct_intst1);
            break;
        }
        if(strcmp(cOld_br_no,sFn_reg.tx_br_no) != 0)
        {
            if(lCnt > 0)
            {
                /*切换机构了 写上个机构的信息*/
                fprintf(fp," %-6.6s  %-8d%-17.2lf%-17.2lf%-17.2lf%-17.2lf\n",\
                    cOld_br_no,sTotal_jg.cnt,sTotal_jg.tx_amt,\
                    sTotal_jg.redempt_amt,sTotal_jg.return_capital1,sTotal_jg.fct_intst1);
                if(lCnt % 40 == 0)
                {
                    /*机构换一页*/
                    fprintf(fp,"\f");
                }
                lCnt = 0;
            }
            
            lCnt ++ ;
            strcpy(cOld_br_no,sFn_reg.tx_br_no);
            memset(&sTotal_jg,0x00,sizeof(sTotal_jg));
        }
        /*根据不同计算 取不同的天数*/
        if(cFlag[0] == '0')/*输入利率原到期日期*/
        {
            lDays = pub_base_CalTrueDays(sFn_reg.ic_date,sFn_reg.end_date);
            vtcp_log("ic_date[%ld] end_date[%ld] lDays[%ld]",sFn_reg.ic_date,sFn_reg.end_date,lDays);
        }
        else if(cFlag[0] =='1')/*输入利率 输入中止日期*/
        {
            lDays = pub_base_CalTrueDays(sFn_reg.ic_date,lDate);
            vtcp_log("ic_date[%ld] end_date[%ld] lDays[%ld]",sFn_reg.ic_date,lDate,lDays);
        }
        else if(cFlag[0] =='2')/*都按原来的算*/
        {
            dRate = sFn_parm.rate;
            /*dProfit_rate = sFn_parm.profit_rate;*/
            lDays = pub_base_CalTrueDays(sFn_reg.ic_date,sFn_reg.end_date);
            vtcp_log("ic_date[%ld] end_date[%ld] lDays[%ld]",sFn_reg.ic_date,sFn_reg.end_date,lDays);
        }
        else if(cFlag[0] =='3')/*原利率 输入中止日期*/
        {
            dRate = sFn_parm.rate;
            /*dProfit_rate = sFn_parm.profit_rate;*/
            lDays = pub_base_CalTrueDays(sFn_reg.ic_date,lDate);
            vtcp_log("ic_date[%ld] end_date[%ld] lDays[%ld]",sFn_reg.ic_date,lDate,lDays);
        }
        
        vtcp_log("取到的利率dRate[%lf],分润率dProfit_rate[%lf]天数lDays[%ld]",dRate,dProfit_rate,lDays);
        /*计算利息*/
        /*机构信息累加*/
        if(lDays == 366)
        	lDays = 365;
        sTotal_jg.cnt ++;
        sTotal_jg.tx_amt += sFn_reg.tx_amt;
        sTotal_jg.redempt_amt += sFn_reg.redempt_amt;
        sTotal_jg.return_capital1 += sFn_reg.tx_amt - sFn_reg.redempt_amt;
        dTmp_intst = (sFn_reg.tx_amt - sFn_reg.redempt_amt) * lDays * dRate * (1+ sFn_reg.flt_radio)/L365/L100;
        vtcp_log("dtmp_intst [%lf]",dTmp_intst);
        sTotal_jg.fct_intst1 += pub_base_PubDround_1(dTmp_intst);
        /*
        dTmp_intst = (sFn_reg.tx_amt - sFn_reg.redempt_amt) * lDays * dProfit_rate/L365/L100;
        vtcp_log("dtmp_intst [%lf]",dTmp_intst);
        dProfit_amt += pub_base_PubDround_1(dTmp_intst);
        */
        /*累加总行的*/
        sTotal_qs.cnt ++;
        sTotal_qs.tx_amt += sFn_reg.tx_amt;;
        sTotal_qs.redempt_amt += sFn_reg.redempt_amt;
        sTotal_qs.return_capital1 += sFn_reg.tx_amt - sFn_reg.redempt_amt;;
        sTotal_qs.fct_intst1 += pub_base_PubDround_1(dTmp_intst);

    }
    Fn_reg_Clo_Sel();
    fclose(fp);
    fp = NULL;
    set_zd_data(DC_FILE_SND, "2");
    vtcp_log("计算结果lCnt[%ld],dIntst[%lf],dProfit[%lf]dCapital[%lf]",lCnt,dIntst,dProfit_amt,dCapital);
    set_zd_double("0390",dIntst);
    set_zd_double("0400",dProfit_amt);
    set_zd_double("0410",dCapital);

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    vtcp_log("计算预期收益成功[%s]", g_pub_tx.reply);
    return 0;
ErrExit:
    if(fp != NULL)
    {
        fclose(fp);
        fp =NULL;
    }
    vtcp_log("计算预期收益失败[%s]", g_pub_tx.reply);
    return 1;
}
/*查询机构限额表 返回文件*/
static int tx_0007()
{
    int ret  =0;
    char cPrdt_code[21];
    char cFilename[256];
    struct fn_br_quota_parm_c sFn_br_qp;
    FILE *fp = NULL;
    
    memset(cPrdt_code,0x00,sizeof(cPrdt_code));
    memset(cFilename,0x00,sizeof(cFilename));
    memset(&sFn_br_qp,0x00,sizeof(sFn_br_qp));

    get_zd_data("0630",cPrdt_code);
    
    pub_base_AllDwnFilName(cFilename);
    fp=fopen(cFilename,"w");
    if(fp == NULL)
    {
        vtcp_log("打开文件失败[%s]",cFilename);
        strcpy(g_pub_tx.reply,"SM100");
        goto ErrExit;
    }
    /*写个标题*/
    fprintf(fp,"~机构|限定总额|剩余额度|\n");
    ret = Fn_br_quota_parm_Dec_Sel(g_pub_tx.reply,"prdt_code='%s'",cPrdt_code);
    if(ret)
    {
        vtcp_log("查询理财产品配额参数表失败iRet[%d]",ret);
        strcpy(g_pub_tx.reply,"D327");
        goto ErrExit;
    }
    while(1)
    {
        ret  = Fn_br_quota_parm_Fet_Sel(&sFn_br_qp,g_pub_tx.reply);
        if(ret != 0 && ret != 100)
        {
            vtcp_log("查询理财产品配额参数表失败iRet[%d]",ret);
            strcpy(g_pub_tx.reply,"D327");
            goto ErrExit;
        }
        else if(ret == 100)
        {
            break;
        }
        else
        {
            fprintf(fp,"%s|%.2lf|%.2lf|\n",sFn_br_qp.br_no,sFn_br_qp.total_amt,sFn_br_qp.available_amt);
        }
    }
    Fn_br_quota_parm_Clo_Sel();
    set_zd_data(DC_FILE_SND, "2");
    fclose(fp);
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    vtcp_log("success !!: reply is[%s]", g_pub_tx.reply);
    return 0;
ErrExit:
    if(fp != NULL)
    {
        fclose(fp);
        fp  = NULL;
    }
    vtcp_log("fail !!: reply is[%s]", g_pub_tx.reply);
    return 1;
}

static int tx_0008()  /*查询这个账号是否认购在募集期的理财产品 1 有   0 无*/
{
	int ret = 0;
	char cAc_no[20];
	char flag = '0';
	struct fn_reg_c sFn_reg;
	
	memset(cAc_no,0x00,sizeof(cAc_no));
	memset(&sFn_reg,0x00,sizeof(sFn_reg));
	
	get_zd_data("0300",cAc_no);

	

	ret = Fn_reg_Sel(g_pub_tx.reply,&sFn_reg,"ac_no = '%s' and sts in ('0','1')",cAc_no);
	if(ret == 100)	
	{
		sprintf(acErrMsg,"数据库fn_reg中无此记录");
		WRITEMSG
		set_zd_data("0670","0");
	}
	else if(ret)
	{
		sprintf(acErrMsg,"数据库fn_reg查询错误");
		WRITEMSG
		strcpy(g_pub_tx.reply,"D330");
		goto ErrExit;	
	}
	if(sFn_reg.sts[0]=='0')   /*募集期*/
	set_zd_data("0670","1");
	else if(sFn_reg.sts[0]=='1')  /*运行期 */
	set_zd_data("0670","2");


OkExit:
    strcpy(g_pub_tx.reply,"0000");
    vtcp_log("success !!: reply is[%s]", g_pub_tx.reply);
    return 0;
ErrExit:
    
    vtcp_log("fail !!: reply is[%s]", g_pub_tx.reply);
    return 1;	
}

static int tx_0009()  /*查询这个账号是否认购在募集期的特定理财产品 */
{
	int ret = 0;
	char cAc_no[20];
	double sTmt = 0.00;
	char cPrdt_code[21];
	struct fn_reg_c sFn_reg;
	
	memset(cAc_no,0x00,sizeof(cAc_no));
	memset(&sFn_reg,0x00,sizeof(sFn_reg));
	memset(cPrdt_code,0x00,sizeof(cPrdt_code));
	
	get_zd_data("0300",cAc_no);
	get_zd_data("0630",cPrdt_code);
	vtcp_log("################0400sTmt[%s][%s]",cAc_no,cPrdt_code);
	ret = Fn_reg_Dec_Sel(g_pub_tx.reply,"ac_no = '%s' and prdt_code = '%s' ",cAc_no,cPrdt_code);
    if(ret)
    {
        vtcp_log("定义理财登记簿游标错误");
        strcpy(g_pub_tx.reply,"D101");
        goto ErrExit;
    }
    
    while(1)
    {      
        ret = Fn_reg_Fet_Sel(&sFn_reg,g_pub_tx.reply);
	        if(ret != 100 && ret != 0)
	        {
	            vtcp_log("取理财登记簿游标错误");
	            strcpy(g_pub_tx.reply,"D330");
	            goto ErrExit;
	        }
	        else if(ret == 0)
	        {       
	        	vtcp_log("找到了");
	        	vtcp_log("=========sFn_reg.tx_amt[%.2lf]",sFn_reg.tx_amt);
	        	vtcp_log("=========sFn_reg.redempt_amt[%.2lf]",sFn_reg.redempt_amt);
	        	sTmt = sFn_reg.tx_amt - sFn_reg.redempt_amt;
	        	vtcp_log("=========0400sTmt[%.2lf]",sTmt);
	        }
	        else
	        	break;
    }
  vtcp_log("0400sTmt[%.2lf]",sTmt);
	set_zd_double("0400",sTmt);
	    Fn_reg_Clo_Sel();


OkExit:
    strcpy(g_pub_tx.reply,"0000");
    vtcp_log("success !!: reply is[%s]", g_pub_tx.reply);
    return 0;
ErrExit:
    
    vtcp_log("fail !!: reply is[%s]", g_pub_tx.reply);
    return 1;	
}
static int tx_0010()  /*查询帐号认购且已经扣款的理财产品 */
{
	int ret = 0;
	char cAc_no[20];
	char cFilename[256];
	char cPrdt_code[21];
    FILE *fp = NULL;
	
	struct fn_reg_c sFn_reg;
	struct fn_parm_c sFn_parm;
	
	memset(cAc_no,0x00,sizeof(cAc_no));
	memset(&sFn_reg,0x00,sizeof(sFn_reg));
	memset(&sFn_parm,0x00,sizeof(sFn_parm));
    memset(cFilename,0x00,sizeof(cFilename));
    memset(cPrdt_code,0x00,sizeof(cPrdt_code));
    
    /* 生成文件 */
    pub_base_AllDwnFilName(cFilename);
    fp=fopen(cFilename,"w");
    if( fp==NULL )
    {
        vtcp_log("open file [%s] error ",cFilename);
        strcpy(g_pub_tx.reply,"SM100");
        goto ErrExit;
    }
	get_zd_data("0300",cAc_no);
	vtcp_log("################ac_no = [%s]",cAc_no);
	ret = Fn_reg_Dec_Sel(g_pub_tx.reply,"ac_no = '%s' and sts = '1' ",cAc_no);
	if(ret)
	{
		sprintf(acErrMsg,"定义理财产品登记薄游标错误");
		WRITEMSG
		strcpy(g_pub_tx.reply,"D330");
		goto ErrExit;	
	}
	while(1)
	{
		ret = Fn_reg_Fet_Sel(&sFn_reg,g_pub_tx.reply);
		if(ret != 100 && ret != 0)
	    {
	        vtcp_log("取理财登记簿游标错误");
	        strcpy(g_pub_tx.reply,"D330");
	        goto ErrExit;
	    }
	    if(ret == 0)
	    {     
	    	if(strcmp(cPrdt_code,sFn_reg.prdt_code) != 0)
	    	{
	        	ret=Fn_parm_Sel(g_pub_tx.reply,&sFn_parm,"prdt_code = '%s'",sFn_reg.prdt_code);
				if( ret==100 )
				{
				    sprintf(acErrMsg,"数据库fn_reg中无此记录");
					WRITEMSG
				}
				else if( ret )
				{
				    sprintf(acErrMsg,"数据库fn_parm查询错误");
					WRITEMSG
					strcpy(g_pub_tx.reply,"D330");
					goto ErrExit;
				}
				fprintf(fp,"9018|%-60s|%-20s|\n",sFn_parm.title,sFn_reg.prdt_code);
				strcpy(cPrdt_code,sFn_reg.prdt_code);
			}
	    }
    	if(ret == 100)	
		{
			vtcp_log("没有查询到数据 走到这里了");
			sprintf(acErrMsg,"帐号暂无认购的记录");
			strcpy(g_pub_tx.reply,"D336");
			set_zd_data("0130","帐号暂无认购记录！ ");
			goto ErrExit;
			
		}
	     else
	        break;
	}
	ret = Fn_reg_Clo_Sel();
	set_zd_data(DC_FILE_SND, "2");
	fclose(fp);



OkExit:
    strcpy(g_pub_tx.reply,"0000");
    vtcp_log("success !!: reply is[%s]", g_pub_tx.reply);
    return 0;
ErrExit:
    
    vtcp_log("fail !!: reply is[%s]", g_pub_tx.reply);
    if(fp != NULL)
    {
        fclose(fp);
    }
    return 1;	
}
static int tx_0011()  /*统计认购理财产品总额和赎回金额*/
{
	int ret = 0;
	char cAc_no[20];
	char cPrdt_code[21];
	double sCut = 0.00;
	double sRem = 0.00;
	
	struct fn_reg_c sFn_reg;
	struct fn_parm_c sFn_parm;
	
	memset(cAc_no,0x00,sizeof(cAc_no));
	memset(&sFn_reg,0x00,sizeof(sFn_reg));
	memset(&sFn_parm,0x00,sizeof(sFn_parm));
    memset(cPrdt_code,0x00,sizeof(cPrdt_code));
    
    /* 生成文件 */
	get_zd_data("0300",cAc_no);
	get_zd_data("0620",cPrdt_code);
	vtcp_log("################ac_no = [%s],prdt_code = [%s]",cAc_no,cPrdt_code);
	ret = Fn_parm_Sel(g_pub_tx.reply,&sFn_parm," prdt_code = '%s' ",cPrdt_code);
	if(ret == 100)	
	{
		sprintf(acErrMsg,"数据库中无此记录");
		WRITEMSG
	}
	else if(ret)
	{
		sprintf(acErrMsg,"数据库查询错误");
		WRITEMSG
		strcpy(g_pub_tx.reply,"D330");
		goto ErrExit;	
	}
	set_zd_data("0250",sFn_parm.title);
	set_zd_double("0420",sFn_parm.per_min_amt);
    set_zd_double("0430",sFn_parm.com_min_amt);
	ret = Fn_reg_Dec_Sel(g_pub_tx.reply,"ac_no = '%s' and prdt_code = '%s' ",cAc_no,cPrdt_code);
    if(ret)
    {
        vtcp_log("定义理财登记簿游标错误");
        strcpy(g_pub_tx.reply,"D101");
        goto ErrExit;
    }
    while(1)
    {      
        ret = Fn_reg_Fet_Sel(&sFn_reg,g_pub_tx.reply);
	        if(ret != 100 && ret != 0)
	        {
	            vtcp_log("取理财登记簿游标错误");
	            strcpy(g_pub_tx.reply,"D330");
	            goto ErrExit;
	        }
	        if(ret == 0)
	        {       
	        	vtcp_log("找到了");
	        	vtcp_log("=========sFn_reg.tx_amt[%.2lf]",sFn_reg.tx_amt);
	        	vtcp_log("=========sFn_reg.redempt_amt[%.2lf]",sFn_reg.redempt_amt);
	        	sCut += sFn_reg.tx_amt;
	        	sRem += sFn_reg.redempt_amt;
	        	vtcp_log("=========0400sTmt[%.2lf]",sCut);
	        }
	        else
	        	break;
    }
  	vtcp_log("0400sTmt[%.2lf]",sCut);
	set_zd_double("0400",sCut);
	set_zd_double("0410",sRem);
	Fn_reg_Clo_Sel();
	    
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    vtcp_log("success !!: reply is[%s]", g_pub_tx.reply);
    return 0;
ErrExit:
    vtcp_log("fail !!: reply is[%s]", g_pub_tx.reply);
    return 1;	
}