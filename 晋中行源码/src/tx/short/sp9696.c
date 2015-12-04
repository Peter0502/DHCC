/*************************************************
* 文 件 名:  sp9696.c
* 功能描述： 根据票号顺序号查询本票详细信息 
* 完成日期： 2007年12月18日
* 修改记录： 
* 日    期:
* 修 改 人:
* 修改内容:
*************************************************/
#include <string.h>
#include "public.h"
#include "cashier_c.h"
struct D37TOS_GROUP{
    char    pono[16];           /* 票据号码 0 */        /* 20110422 [12->16] */
    char    brno[7];            /* 机构号  12 */
    char    txday[8];           /* 交易日期 19 */
    char    avbal[15];          /* 票面金额[出票金额] 27 */
    char    useamt[15];         /* 实际结算金额 42 */
    char    feeamt[15];         /* 手续费 57 */
    char    leavebal[15];       /* 多余金额 72 */
    char    paybrno[12];        /* 付款人开户行行号 87 */
    char    payactno[32];       /* 付款人帐号 99 */
    char    payname[60];        /* 付款人名字 131 */
    char    paybrname[60];      /* 付款人行名 191 */
    char    cashactno[32];      /* 收款人帐号 251 */
    char    cashbrno[12];       /* 指定兑付机构 283 */
    char    cashbrname[60];     /* 指定兑付机构名称 295 */
    char    cashname[60];       /* 收款人名字 355 */
    char    stat;               /* 当前状态 415 */
    char    regday[8];          /* 登记日期 416 */
    char    signday[8];         /* 签约日期 424 */
    char    cashday[8];         /* 总付日期 432 */
    char    jqday[8];           /* 结清日期 440 */
    char    endday[8];          /* 到期日期 448 */
    char    passwd[10];         /* 密押 456 */
    char     avbaltype;         /* 1.定额，2不定额 466 */
    char     bptype;            /* 本票类型 467 */
    char     signtype;          /* 转出标志0.现金　1.转帐帐号 2.科目对转 468 */
    char     cashflag;          /* 转出标志0.现金　1.转帐帐号 2.科目对转 469 */
    char     reason[60];        /* 附言 470 */
    char     feeflag;           /* 手续费标志0.现金1.现金加急2.转帐3.转帐加急4不收530*/
    char     optlrno[6];        /* 录入柜员 531 */
    char     chktlrno[6];       /* 符合柜员 537 */
    char     passtlrno[6];      /* 密押操作员 543 */
    char     cashtlrno[6];      /* 兑付操作员 549 */
    char     jqtlrno[6];        /* 结清操作员 555 */
    char     reqno[10];         /* 申请书号 561 */
}tos;
int sp9696()
{
    int    ret;
    long lPono=0;
    char    cQuery[2];
    char    cHnbz[2];
    char    cPono[17];      /* 20110422 [13->17] */
    char    cTemp[21];
    struct    cashier_c    cashier;    /*本票基本信息表*/
    
    memset(cQuery  , 0 , sizeof(cQuery));
    memset(cHnbz   , 0 , sizeof(cHnbz));
    memset(cPono   , 0 , sizeof(cPono));
    memset(cTemp   , 0 , sizeof(cTemp));
    memset(&cashier, 0 , sizeof(struct cashier_c));
    
    get_zd_long("0440",&cashier.signday);     /*本票日期*/
    get_zd_data("0300",cPono);                /*本票号码*/
    get_zd_data("0200",cHnbz);                /*行内标志*/
    get_zd_data("0110",cQuery);               /*操作标志*/
    
    vtcp_log("[%s][%d]cPono==[%s]",__FILE__,__LINE__,cPono);
    vtcp_log("[%s][%d]cHnbz==[%c]",__FILE__,__LINE__,cHnbz[0]);
    vtcp_log("[%s][%d]cQuery==[%c]",__FILE__,__LINE__,cQuery[0]);
    cashier.inflag[0]=cHnbz[0];
    
    memcpy(cTemp,"00000000000000000000",sizeof(cTemp)-1);
    vtcp_log("[%s][%d]cTemp==[%s]  ",__FILE__,__LINE__,cTemp);
    memcpy(cTemp+4,cPono,sizeof(cPono)-1);     /* 20110422 [8->4] */
    vtcp_log("[%s][%d]cTemp==[%s]  ",__FILE__,__LINE__,cTemp);
    memcpy(cashier.pono,cTemp,sizeof(cashier.pono)-1);
    
    vtcp_log("[%s][%d]cashier.pono==[%s]  ",__FILE__,__LINE__,cashier.pono);
    vtcp_log("[%s][%d]cashier.signday=[%ld]",__FILE__,__LINE__,cashier.signday);
    vtcp_log("[%s][%d]cashier.inflag=[%c] ",__FILE__,__LINE__,cashier.inflag[0]);
    if(cashier.signday==0)
    {
        cashier.signday=g_pub_tx.tx_date;
    }
    ret=Cashier_Sel(g_pub_tx.reply,&cashier,"pono='%s' and signday='%ld' ",cashier.pono,cashier.signday);
    if(ret==100)
    {
        vtcp_log("[%s][%d]基本信息表里无此本票号码[%s]\n",__FILE__,__LINE__,cashier.pono);
        sprintf(acErrMsg,"基本信息表里无此本票号码");
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }else if(ret)
    {
        vtcp_log("[%s][%d]查询基本信息表错误[%d]\n",__FILE__,__LINE__,ret);
        sprintf(acErrMsg,"执行Cashier_Sel错[%d]",ret);
        set_zd_data(DC_GET_MSG,acErrMsg);
        strcpy(g_pub_tx.reply,"8888");
        goto ErrExit;
    }
    /*向前台屏幕赋值*/
    set_zd_data("0250",  cashier.payname); /*付款人名称*/
    vtcp_log("[%s][%d]cashier.payname=[%s]\n",__FILE__,__LINE__,cashier.payname);
    set_zd_data("0260",  cashier.cashname); /*收款人名称*/
    vtcp_log("[%s][%d]cashier.cashname=[%s]\n",__FILE__,__LINE__,cashier.cashname);
    set_zd_data("0330",  cashier.payactno);/*付款人帐号*/
    vtcp_log("[%s][%d]cashier.payactno=[%s]\n",__FILE__,__LINE__,cashier.payactno);
    set_zd_data("0310",  cashier.cashactno);/*收款人帐号*/
    vtcp_log("[%s][%d]cashier.cashactno=[%s]\n",__FILE__,__LINE__,cashier.cashactno);
    set_zd_double("0390",cashier.avbal);   /*出票金额*/
    vtcp_log("[%s][%d]cashier.avbal=[%f]\n",__FILE__,__LINE__,cashier.avbal);
    set_zd_double("0400",cashier.useamt);  /*结算金额*/
    vtcp_log("[%s][%d]cashier.useamt=[%f]\n",__FILE__,__LINE__,cashier.useamt);
    set_zd_double("0410",cashier.feeamt);  /*手续费*/
    vtcp_log("[%s][%d]cashier.feeamt=[%f]\n",__FILE__,__LINE__,cashier.feeamt);
    set_zd_double("0420",cashier.leavebal);/*多余金额*/
    vtcp_log("[%s][%d]cashier.leavebal=[%f]\n",__FILE__,__LINE__,cashier.leavebal);
    set_zd_long("0440", cashier.regday);    /*登记日期*/
    vtcp_log("[%s][%d]cashier.regday=[%ld]\n",__FILE__,__LINE__,cashier.regday);
    set_zd_long("0450", cashier.signday);   /*签约日期*/
    vtcp_log("[%s][%d]cashier.signday=[%ld]\n",__FILE__,__LINE__,cashier.signday);
    set_zd_long("0460", cashier.cashday);   /*兑付日期*/
    vtcp_log("[%s][%d]cashier.cashday=[%ld]\n",__FILE__,__LINE__,cashier.cashday);
    set_zd_long("0470", cashier.jqday);     /*结清日期*/
    vtcp_log("[%s][%d]cashier.jqday=[%ld]\n",__FILE__,__LINE__,cashier.jqday);
    set_zd_data("0520",cashier.paybrno);   /*付款人开户行行号*/
    vtcp_log("[%s][%d]cashier.paybrno=[%s]\n",__FILE__,__LINE__,cashier.paybrno);
    set_zd_data("0530",cashier.passwd);    /*密押*/
    vtcp_log("[%s][%d]cashier.passwd=[%s]\n",__FILE__,__LINE__,cashier.passwd);
    set_zd_data("0580",cashier.cashbrno);  /*指定兑付机构*/
    vtcp_log("[%s][%d]cashier.cashbrno=[%s]\n",__FILE__,__LINE__,cashier.cashbrno);
    set_zd_data("0590",cashier.chktlrno);  /*复核柜员*/
    vtcp_log("[%s][%d]cashier.chktlrno=[%s]\n",__FILE__,__LINE__,cashier.chktlrno);
    set_zd_data("0600",cashier.passtlrno); /*密押操作员*/
    vtcp_log("[%s][%d]cashier.passtlrno=[%s]\n",__FILE__,__LINE__,cashier.passtlrno);
    set_zd_data("0610",cashier.cashtlrno); /*兑付操作员*/
    vtcp_log("[%s][%d]cashier.cashtlrno=[%s]\n",__FILE__,__LINE__,cashier.cashtlrno);
    set_zd_data("0620",cashier.pono);      /*汇票号码*/
    vtcp_log("[%s][%d]cashier.pono=[%s]\n",__FILE__,__LINE__,cashier.pono);
    set_zd_data("0640",cashier.jqtlrno);   /*结清操作员*/
    vtcp_log("[%s][%d]cashier.jqtlrno=[%s]\n",__FILE__,__LINE__,cashier.jqtlrno);
    set_zd_data("0650",cashier.optlrno);   /*录入柜员*/
    vtcp_log("[%s][%d]cashier.optlrno=[%s]\n",__FILE__,__LINE__,cashier.optlrno);
    set_zd_data("0670",cashier.avbaltype); /*定额标志1定额,2不定额*/
    vtcp_log("[%s][%d]cashier.avbaltype=[%s]\n",__FILE__,__LINE__,cashier.avbaltype);
    set_zd_data("0680",cashier.bptype);    /*本票类型*/
    vtcp_log("[%s][%d]cashier.bptype=[%s]\n",__FILE__,__LINE__,cashier.bptype);
    set_zd_data("0690",cashier.ccflag);    /*转出标志*/
    vtcp_log("[%s][%d]cashier.ccflag=[%s]\n",__FILE__,__LINE__,cashier.ccflag);
    set_zd_data("0700",cashier.cashflag);  /*转出标志*/
    vtcp_log("[%s][%d]cashier.cashflag=[%s]\n",__FILE__,__LINE__,cashier.cashflag);
    set_zd_data("0740",cashier.reason);    /*附言*/
    vtcp_log("[%s][%d]cashier.reason=[%s]\n",__FILE__,__LINE__,cashier.reason);
    set_zd_long("0780", cashier.endday);    /*到期日期*/
    vtcp_log("[%s][%d]cashier.endday=[%ld]\n",__FILE__,__LINE__,cashier.endday);
    set_zd_data("0810",cashier.paybrname); /*付款人行名*/
    vtcp_log("[%s][%d]cashier.paybrname=[%s]\n",__FILE__,__LINE__,cashier.paybrname);
    set_zd_data("0820",cashier.cashbrname);/*指定兑付机构名称*/
    vtcp_log("[%s][%d]cashier.cashbrname=[%s]\n",__FILE__,__LINE__,cashier.cashbrname);
    set_zd_data("0610",cashier.ovocnum);   /*申请书号*/
    vtcp_log("[%s][%d]cashier.ovocnum=[%s]\n",__FILE__,__LINE__,cashier.ovocnum);
    set_zd_data("0910",cashier.brno);      /*机构号*/
    vtcp_log("[%s][%d]cashier.brno=[%s]\n",__FILE__,__LINE__,cashier.brno);
    set_zd_data("0930",cashier.feeflag);   /*手续费标志*/
    vtcp_log("[%s][%d]cashier.feeflag=[%s]\n",__FILE__,__LINE__,cashier.feeflag);
    set_zd_data("0980",cashier.stat);      /*当前状态*/
    vtcp_log("[%s][%d]cashier.stat=[%s]\n",__FILE__,__LINE__,cashier.stat);
OkExit:
    strcpy( g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
ErrExit:
    sprintf(acErrMsg,"Before return:reply is[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
