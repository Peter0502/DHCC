/*************************************************
* 文 件 名:  spB333.c
* 功能描述： 完成银行承兑汇票签发功能
*
* 作    者:  jane
* 完成日期： 2003年5月26日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "mo_bank_acc_po_c.h"
#include "mo_bank_po_c.h"
#include "impa_dep_reg_c.h"
#include "mo_hold_c.h"
#include "com_parm_c.h"
#include "mo_bank_po_fee_c.h"

int spB333()
{
    struct    mo_bank_acc_po_c    sMoBankAccPo;    /* 银行承兑汇票基本信息登记薄 */
    struct    mo_bank_po_c         sMoBankPo;        /* 银行承兑汇票登记薄 */
    struct    mdm_ac_rel_c         sMdmAcRel;        /* 介质账号对照表 */
    struct    dd_mst_c             sDdMst;            /* 活期主文件 */
    struct    impa_dep_reg_c         sImpaDepReg;    /* 质押存单登记薄 */
    struct    mo_hold_c             sMoHold;        /* 冻结登记簿 */
    struct    td_mst_c             sTdMst;            /* 定期主文件 */
    struct    com_parm_c             sComParm;        /* 公共参数表 */
    struct    mo_bank_po_fee_c      sMo_bank_po_fee;   /** 承兑汇票收费 **/
    struct    com_parm_c             wd_com_parm;    /* 公共参数表:取承兑科目 */
    char    cPactNo[21];                        /* 承兑协议编号 */
    char    cPoNo1[31];/**改成30**/                            /* 承兑汇票号码屏幕取值 */
    char    cPoNo[9];                            /* 承兑汇票号码有效位 */
    char    cPoNoHead[9];                        /* 承兑汇票号码冠字头 */
    char    cPoNo2[31];                            /* 承兑汇票号码冠字头+有效位 */
    double    dPoAmt;                                /* 已签发汇票总金额 */
    double    dSumPoAmt;                            /* 已签发的汇票金额总额与本次签发汇票金额之和 */
    long    lHoldSeqn;                            /* 冻结序号 */
    int        iPactNoNum;                            /* 编号 */
    int        ret, flag;
    int     parm_ind = 0;
    double  dTallAmt=0.00; 
    double  dKqAmt =0.00;
    /****************/
    /*****增加电票处理变量 ****/
    char ctype[2];
    /**************************/
    char cInd[2];
    char cPayee_acno[33];
    char cPayee_name[61];
    char cPayee_brname[61];
    char cCt_ind[2];/** add by chenchao 2011-7-28 10:48:10 手续费现转标志 **/
    double dFee_amt= 0.00;/** add by chenchao 手续费金额 **/
    double dPrm_amt = 0.00;/** add by chenchao 承诺金额 **/
    
    memset(cInd,0x00,sizeof(cInd));
    memset(cPayee_acno,0x00,sizeof(cPayee_acno));
    memset(cPayee_name,0x00,sizeof(cPayee_name));
    memset(cPayee_brname,0x00,sizeof(cPayee_brname));
    /******************/
    memset( &sMoBankAccPo, 0x00, sizeof( struct mo_bank_acc_po_c ) );
    memset( &sMoBankPo, 0x00, sizeof( struct mo_bank_po_c ) );
    memset( &sMdmAcRel, 0x00, sizeof( struct mdm_ac_rel_c ) );
    memset( &sDdMst, 0x00, sizeof( struct dd_mst_c ) );
    memset( &sImpaDepReg, 0x00, sizeof( struct impa_dep_reg_c ) );
    memset( &sMoHold, 0x00, sizeof( struct mo_hold_c ) );
    memset( &sTdMst, 0x00, sizeof( struct td_mst_c ) );
    memset( &sComParm, 0x00, sizeof( struct com_parm_c ) );
    memset( &wd_com_parm, 0x00, sizeof( struct com_parm_c ) );
    memset( cPactNo, 0x00, sizeof( cPactNo ) );
    memset( cPoNo1, 0x00, sizeof( cPoNo1 ) );
    memset( cPoNo2, 0x00, sizeof( cPoNo2 ) );
    memset( cPoNo, 0x00, sizeof( cPoNo ) );
    memset( cPoNoHead, 0x00, sizeof( cPoNoHead ) );
    memset( cCt_ind,0x00,sizeof(cCt_ind));

    /* 数据初始化 */
    pub_base_sysinit();

    /* 从前台屏幕取值 */
    get_zd_data( "0330", cPactNo );                        /* 承兑协议编号 */
    get_zd_int( "0500", &iPactNoNum );                    /* 编号 */
    get_zd_data( "0580", cPoNo1 );                        /* 承兑汇票号码 */
    vtcp_log( "zxxxyu111      cPoNo1[%s]", cPoNo1 );
    get_zd_double( "100E", &sMoBankPo.po_amt );            /* 票面金额 */
    get_zd_data("0930",ctype);    /*电票纸票标志* add by chenchao  20100419，P纸票，E电票*/
    if(ctype[0]=='E')
    get_zd_data("1243",cPoNo1);    
    get_zd_data("0670",cCt_ind);

    /**********************/
    get_zd_data("0930",cInd);
    get_zd_data("0270",cPayee_name);
    get_zd_data("0830",cPayee_acno);
    get_zd_data("0810",cPayee_brname);
    get_zd_double("1004",&dFee_amt);
    get_zd_double("1131",&dPrm_amt);
    /**********************/
    /* 取贴现产品编号:到参数表com_parm中取得(用pubf_base_GetParm.c中的函数) */
    /* 票面总金额不能大于等于1100万元人民币 */
    ret = pub_base_GetParm_double( "CDZGE", 1, &dTallAmt );
    if( ret )
    {
        sprintf( acErrMsg, "取承兑汇票最高限额产品编号错!" );
        WRITEMSG
        strcpy( g_pub_tx.reply, "B269" );
        goto ErrExit;
    }

    ret = pub_base_CompDblVal( sMoBankPo.po_amt, dTallAmt );
    if( ret != -1 )
    {
        sprintf( acErrMsg, "承兑汇票票面总金额不允许超过1100万元人民币![%lf]",sMoBankPo.po_amt);
        WRITEMSG
        strcpy( g_pub_tx.reply, "B270" );
        goto ErrExit;
    }

    strcpy( sMoBankPo.sign_br_no, g_pub_tx.tx_br_no );    /* 签发行行号 */
    sMoBankPo.sign_date = g_pub_tx.tx_date;                /* 签发日期 */
    strcpy( sMoBankPo.at_term_ind, "N" );                /* 票据到期标志:Y 到期；N 未到期 */
    strcpy( sMoBankPo.po_sts, "1" );                    /* 承兑汇票状态:1 签发 */
    strcpy( sMoBankPo.po_ind, "1" );                    /* 承兑汇票标志:1 正常 */
    if(ctype[0]=='E')/***电票的话不检查凭证号不销号**/
    {
    }
    else
    {
        /* 检查所输承兑汇票号码是否带冠字头 */
        ret = pub_com_ChkNoteHead( "111", cPoNo1, g_pub_tx.tel);
        if( ret )
        {
            sprintf( acErrMsg, "检查承兑汇票号码出错![%d]", ret );
            WRITEMSG
            goto ErrExit;
        }
        
        vtcp_log( "zxxxyu222      cPoNo1[%s]", cPoNo1 );
        /* 查询承兑汇票是否已被领入 */
        ret = Com_parm_Sel( g_pub_tx.reply, &sComParm, "parm_code='CDHP'" );
        if( ret )
        {
            sprintf( acErrMsg, "执行Com_parm_Sel错误![%d]", ret );
            WRITEMSG
            goto ErrExit;
        }
        
        pub_base_strpack( sComParm.val );
        
        vtcp_log("pono[%s]notetype[%s]brno[%s]tel[%s]",cPoNo1, sComParm.val,g_pub_tx.tx_br_no, g_pub_tx.tel);
        /* 承兑汇票销号处理 */
        g_pub_tx.svc_ind=7021;
        ret=pub_com_NoteUse( 0,0,sComParm.val,cPoNo1,cPoNo1,g_pub_tx.tel);
        if( ret) 
        {
            sprintf( acErrMsg, "银行承兑汇票销号出错!" );
            WRITEMSG
            goto ErrExit;
        }
    }
    /* 根据承兑协议编号查找银行承兑汇票基本信息登记薄 */
    ret = Mo_bank_acc_po_Sel( g_pub_tx.reply, &sMoBankAccPo, "pact_no='%s'", cPactNo );
    if( ret == 100 )
    {
        sprintf( acErrMsg, "无此承兑协议编号!" );
        WRITEMSG
        sprintf( g_pub_tx.reply, "B187" );
        goto ErrExit;
    }
    else if( ret )
    {
        sprintf( acErrMsg, "执行Mo_bank_acc_po_Sel错![%d]", ret );
        WRITEMSG
        goto ErrExit;
    }

/**
    if( sMoBankAccPo.acc_po_sts[0] == '2'&& sMoBankAccPo.acc_po_sts[0] == '3' )
**/
    if( sMoBankAccPo.acc_po_sts[0] == '2' )
    {
        sprintf( acErrMsg, "此承兑协议已做签发!" );
        WRITEMSG
        strcpy( g_pub_tx.reply, "B188" );
        goto ErrExit;
    }
    else if( sMoBankAccPo.acc_po_sts[0] == 'A' )
    {
        sprintf( acErrMsg, "此承兑未做录入!" );
        WRITEMSG
        strcpy( g_pub_tx.reply, "B274" );
        goto ErrExit;
    }else if( sMoBankAccPo.acc_po_sts[0] == '0' )
    {
        sprintf( acErrMsg, "此承兑未做审核!" );
        WRITEMSG
        strcpy( g_pub_tx.reply, "B275" );
        goto ErrExit;
    }

    set_zd_data( "0810", sMoBankAccPo.payee_br_name );    /* 收款人开户行行名(为打印准备) */

    /* 根据承兑协议编号和承兑协议状态为"审批"修改银行承兑汇票基本信息登记薄 */
    ret = Mo_bank_acc_po_Dec_Upd( g_pub_tx.reply , "pact_no='%s' \
                and acc_po_sts in ('1','3')", cPactNo );
    if ( ret )
    {
        sprintf( acErrMsg, "执行Mo_bank_acc_po_Dec_Upd错[%d]", ret );
        WRITEMSG
        goto ErrExit;
    }

    ret = Mo_bank_acc_po_Fet_Upd( &sMoBankAccPo, g_pub_tx.reply );
    if ( ret == 100 )
    {
        sprintf( acErrMsg, "无此承兑协议编号!" );
        WRITEMSG
        sprintf( g_pub_tx.reply, "B187" );
        goto ErrExit;
    }
    else if ( ret )
    {
        sprintf( acErrMsg, "执行Mo_bank_acc_po_Fet_Upd错[%d]", ret );
        WRITEMSG
        goto ErrExit;
    }

    /* 判断银行承兑汇票登记簿中对于同一个协议编号的汇票金额之和与录入时的票面总金额是否相等 */
    ret = sql_sum_double( "mo_bank_po", "po_amt", &dPoAmt, "pact_no='%s' and po_sts='1'", cPactNo );
    if( ret )
    {
        sprintf( acErrMsg, "执行sql_sum_double错[%d]", ret );
        WRITEMSG
        goto ErrExit;
    }
    sprintf( acErrMsg, "已签发的汇票金额总额=[%lf],sign_amt=[%lf]", dPoAmt + sMoBankPo.po_amt,sMoBankAccPo.sign_amt );
    WRITEMSG
    dSumPoAmt = dPoAmt + sMoBankPo.po_amt;    /* 已签发的汇票金额总额与本次签发汇票金额之和 */
    
    /*************add by xyy 2010-6-24 15:46:57 如果保证金额大于0 *************/
    if( pub_base_CompDblVal ( sMoBankAccPo.bail_amt,0.00 ) > 0 )
    {
        double dSumKyamt = 0.00;
        
        dSumKyamt = sMoBankAccPo.bail_amt / sMoBankAccPo.bail_amt_ratio*100;
        if( pub_base_CompDblVal( dSumPoAmt, dSumKyamt) > 0)
        {
            sprintf( acErrMsg, "保证金不足!" );
            WRITEMSG
            strcpy( g_pub_tx.reply, "B194" );
            goto ErrExit;
        }
        
        dKqAmt = dSumKyamt;
    }
    else
    {
        dKqAmt = sMoBankAccPo.sign_amt;
    }
    vtcp_log("[%s][%d] 可签发总金额=[%f]",__FILE__,__LINE__,dKqAmt);
    /*************end by xyy 2010-6-24 15:46:57 *************/
    
    ret = pub_base_CompDblVal( dSumPoAmt, dKqAmt );
    if( ret == 1 )
    {
        sprintf( acErrMsg, "已签发的汇票金额总额大于录入时的票面总金额!" );
        WRITEMSG
        strcpy( g_pub_tx.reply, "B194" );
        goto ErrExit;
    }
    else if( ret == 0 )
    {
        sprintf( acErrMsg, "已签发的汇票金额总额=[%lf],sign_amt=[%lf]", dPoAmt + sMoBankPo.po_amt,sMoBankAccPo.sign_amt );
        WRITEMSG
        /* 修改银行承兑汇票基本信息登记薄 */
        strcpy( sMoBankAccPo.acc_po_sts, "2" );                /* 汇票状态: 2 正常 */

        ret = Mo_bank_acc_po_Upd_Upd( sMoBankAccPo, g_pub_tx.reply );
            if ( ret )
        {
            sprintf( acErrMsg, "执行Mo_bank_acc_po_Upd_Upd错[%d]", ret );
            WRITEMSG
            goto ErrExit;    
        }
    }
    sprintf( acErrMsg, "已签发的汇票金额总额=[%lf],sign_amt=[%lf]", dPoAmt + sMoBankPo.po_amt,sMoBankAccPo.sign_amt );
    WRITEMSG
    Mo_bank_acc_po_Clo_Upd( );
    if(ctype[0]=='E')
    {
        get_zd_data("1243",cPoNo2);
    }
    else
    {
        /* 取承兑汇票冠字头 */
        strncpy( cPoNoHead, cPoNo1, 8 );
        cPoNoHead[8]='\0';
        pub_base_strpack( cPoNoHead );
    
        /* 取承兑汇票有效位 */
        strncpy( cPoNo, cPoNo1+8, 8 );
        cPoNo[8] = '\0';
        sprintf( acErrMsg, "zxxxyu    cPoNo[%s]", cPoNo );
        WRITEMSG
    
        /* 承兑汇票号码冠字头+有效位 */
        sprintf( cPoNo2, " %s%s ", cPoNoHead, cPoNo );
    }
    sprintf( acErrMsg, "zxxxyu    cPoNo2[%s]", cPoNo2 );
    WRITEMSG
    pub_base_strpack_all( cPoNo2 );

    /* 根据承兑汇票号码查找银行承兑汇票登记薄 */
    ret = Mo_bank_po_Sel( g_pub_tx.reply, &sMoBankPo, "po_no='%s'", cPoNo2 );
    TRACE
    if( ret == 0 )
    {
        sprintf( acErrMsg, "承兑汇票号码已存在!" );
        WRITEMSG
        sprintf( g_pub_tx.reply, "B209" );
        goto ErrExit;
    }
    else if( ret != 100 )
    {
        TRACE
        sprintf( acErrMsg, "执行Mo_bank_po_Sel错![%d]", ret );
        WRITEMSG
        goto ErrExit;
    }

    /* 根据承兑汇票编号查找银行承兑汇票登记薄 */
    ret = Mo_bank_po_Sel( g_pub_tx.reply, &sMoBankPo, "pact_no='%s' and pact_no_num=%d",cPactNo, iPactNoNum );
    if( ret == 0 )
    {
        sprintf( acErrMsg, "编号重复!" );
        WRITEMSG
        strcpy( g_pub_tx.reply, "B208" );
        goto ErrExit;
    }
    else if( ret != 100 )
    {
        TRACE
        sprintf( acErrMsg, "执行Mo_bank_po_Sel错![%d]", ret );
        WRITEMSG
        goto ErrExit;
    }

    /* 登记银行承兑汇票登记薄 */
    vtcp_log("[%s][%d] pactno=[%s] pono=[%s] ",__FILE__,__LINE__,cPactNo,cPoNo2);
    strcpy( sMoBankPo.pact_no, cPactNo );
    sMoBankPo.pact_no_num = iPactNoNum;
    strcpy( sMoBankPo.po_no, cPoNo2 );

    ret = Mo_bank_po_Ins( sMoBankPo, g_pub_tx.reply );
    if( ret )
    {
        Mo_bank_po_Debug(&sMoBankPo);
        sprintf( acErrMsg, "执行Mo_bank_po_Ins错[%d]", ret );
        WRITEMSG
        goto ErrExit;
    }

TRACE
    /* 冻结质押存单 */
    ret = Impa_dep_reg_Dec_Upd( g_pub_tx.reply, "pact_no='%s'", sMoBankAccPo.pact_num );
    if( ret )
    {
        sprintf( acErrMsg, "执行Impa_dep_reg_Dec_Upd错[%d]", ret );
        WRITEMSG
        goto ErrExit;
    }

TRACE
    flag = 0;
    while(1)
    {
        ret = Impa_dep_reg_Fet_Upd( &sImpaDepReg, g_pub_tx.reply );
        if ( ret == 100 )
        {
            break;
        }else if ( ret )
        {
            sprintf( acErrMsg, "执行Impa_dep_reg_Fet_Upd错[%d]", ret );
            WRITEMSG
            goto ErrExit;
        }
TRACE
        /* 根据质押存单号修改定期主文件 */
        ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdmAcRel, "ac_no='%s'",\
                        sImpaDepReg.mort_ac_no );
        if( ret == 100 )
        {
            sprintf( acErrMsg, "质押存单号不存在![%s]", sImpaDepReg.mort_ac_no );
            WRITEMSG
            strcpy( g_pub_tx.reply, "B183" );
            goto ErrExit;
        }
        else if( ret )
        {
            sprintf( acErrMsg, "执行Mdm_ac_rel_Sel出错![%d]", ret );
            WRITEMSG
            goto ErrExit;
        }

TRACE
        ret = Td_mst_Dec_Upd( g_pub_tx.reply , "ac_id=%ld", sMdmAcRel.ac_id );
        if ( ret )
        {
            sprintf( acErrMsg, "执行Td_mst_Dec_Upd错[%d]", ret );
            WRITEMSG
            goto ErrExit;
        }

TRACE
        ret = Td_mst_Fet_Upd( &sTdMst, g_pub_tx.reply );
        if ( ret == 100 )
        {
            sprintf( acErrMsg, "质押存单号不存在![%s]", sImpaDepReg.mort_ac_no );
            WRITEMSG
            strcpy( g_pub_tx.reply, "B183" );
            goto ErrExit;
        }else if ( ret )
        {
            sprintf( acErrMsg, "执行Td_mst_Fet_Upd错[%d]", ret );
            WRITEMSG
            goto ErrExit;
        }

        /**--- xxx ----- @20030906@ ---- 冻结过了就不用再冻结了 -----**/
        if( sTdMst.hold_sts[0]=='1' )
        {
            sprintf( acErrMsg, "已经冻结过了，不用再冻结了[%d][%ld]",ret,sTdMst.ac_id );
            WRITEMSG
            flag++;
            continue;
        }
        strcpy( sTdMst.hold_sts, "1" );        /* 冻结状态: 1 完全冻结*/
TRACE
        ret = Td_mst_Upd_Upd( sTdMst, g_pub_tx.reply );
            if ( ret )
        {
            sprintf( acErrMsg, "执行Td_mst_Upd_Upd错[%d]", ret );
            WRITEMSG
            goto ErrExit;    
        }
        Td_mst_Clo_Upd( );

TRACE
        /* 查询冻结登记簿中最大冻结序号 */
        ret = sql_max_long( "mo_hold", "hold_seqn", &lHoldSeqn , "1=1" );
        if( ret )
        {
            sprintf( acErrMsg, "执行sql_max_long错[%d]", ret );
            WRITEMSG
            goto ErrExit;
        }

TRACE
        /* 登记冻结登记簿 */
        memset(&sMoHold,0x00,sizeof(struct mo_hold_c));
        sMoHold.hold_seqn = lHoldSeqn + 1;    /* 冻结序号 */
        sMoHold.ac_id = sMdmAcRel.ac_id;    /* 冻结账户ID */
        sMoHold.ac_seqn = sMdmAcRel.ac_seqn;    /* 冻结账户序号 */
        strcpy( sMoHold.hold_typ, "1" );    /* 冻结状态: 1 全部冻结*/
        sMoHold.plan_hold_amt = 0.00;        /* 计划冻结金额 */
        strcpy( sMoHold.auto_unhold_ind, "Y" );    /* 自动解冻标志: Y 不自动解冻*/
        strcpy( sMoHold.hold_sts, "0" );    /* 冻结状态解除: 0 未解除*/
        sMoHold.beg_hold_date = g_pub_tx.tx_date;/* 冻结起始日期 */
        sMoHold.plan_unhold_date = 0;        /* 计划解冻日期 */
        sMoHold.fct_unhold_date = 0;        /* 实际解冻日期 */
        strcpy( sMoHold.hold_br_no, g_pub_tx.tx_br_no );/* 冻结机构号 */
        strcpy( sMoHold.hold_tel, g_pub_tx.tel );/* 冻结操作员*/
        strcpy( sMoHold.unhold_br_no, "0" );    /* 解冻机构号*/
        strcpy( sMoHold.unhold_tel, "0" );    /* 解冻操作员*/
        sMoHold.wrk_date = g_pub_tx.tx_date;    /* 交易日期 */
        sMoHold.trace_no = g_pub_tx.trace_no;    /* 流水号 */
        strcpy( sMoHold.hold_brf,"签发承兑汇票自动冻结");

TRACE
        ret = Mo_hold_Ins( sMoHold, g_pub_tx.reply );
        if( ret )
        {
            sprintf( acErrMsg, "执行Mo_hold_Ins错[%d]", ret );
            WRITEMSG
            goto ErrExit;
        }

TRACE
        ret = Impa_dep_reg_Upd_Upd( sImpaDepReg, g_pub_tx.reply );
            if ( ret )
        {
            sprintf( acErrMsg, "执行Impa_dep_reg_Upd_Upd错[%d]", ret );
            WRITEMSG
            goto ErrExit;
        }

        flag++;
    }

TRACE
    Impa_dep_reg_Clo_Upd( );

    if(sMoBankAccPo.acc_po_sts[0] == '9')    /** 外行汇票 **/
            parm_ind = 6;
    else{
            switch(sMoBankAccPo.acc_type[0])
            {
                case '1':    /** 差额承兑 **/
                    parm_ind = 1;
                    break;
                case '2':    /** 质压存单 **/
                    parm_ind = 2;
                    break;
                case '3':    /** 100%保证金 **/
                    parm_ind = 3;
                    break;
                case '4':    /** 担保承兑 **/
                    parm_ind = 4;
                    break;
                case '5':    /** 大票换小票 **/
                    parm_ind = 5;
                    break;
                default:
                    sprintf( acErrMsg, "[%s][%d]汇票类型错误![%s]",__FILE__,__LINE__,sMoBankAccPo.acc_type);
                    WRITEMSG
                    goto ErrExit;
                
            }
    }
    /** 从参数表com_parm取得不同类型承兑汇票的科目 **/
    ret = Com_parm_Sel(g_pub_tx.reply,&wd_com_parm,"parm_code='CDHPR' and parm_seqn=%d",parm_ind);   
    if(ret)
    {
            sprintf( acErrMsg, "[%s][%d]查询参数表错误![%d]",__FILE__,__LINE__, ret );
            WRITEMSG
            goto ErrExit;
    }

    /** 为记流水作准备 * xxx --- 改为调用表外记账子交易 ---**
    g_pub_tx.svc_ind = 0;  
    strcpy( g_pub_tx.cur_no, "01" );     * 币种 *
    strcpy( g_pub_tx.ac_no, "60201" );     * 表外：承兑汇票 *
    g_pub_tx.ac_seqn = 0;                  * 账户序号 *
    g_pub_tx.ac_id = 0;                  * 账户ID *
    g_pub_tx.tx_amt1 = sMoBankPo.po_amt;    * 承兑汇票票面金额 *
    strcpy( g_pub_tx.note_type, sComParm.val );
    strcpy( g_pub_tx.beg_note_no, cPoNo2 );
    strcpy( g_pub_tx.add_ind, "1" );    * 增减:0减1加 *
    strcpy( g_pub_tx.ct_ind, "2" );        * 现转:1现2转 *
    strcpy( g_pub_tx.brf, "收入承兑汇票" );* 备注 *
    strcpy(g_pub_tx.hst_ind,"1");         * 日间入明细 *
    **----------------------------------------------------**/

    /* 记流水日志 */
    /**----- xxx ---- 20030819 ---- 撤销必须要有协议编号 ----**/
    strcpy(g_pub_tx.ac_no,cPactNo);
    g_pub_tx.tx_amt1 = sMoBankPo.po_amt;    /* 承兑汇票票面金额 */
    strcpy( g_pub_tx.note_type, sComParm.val );
    memcpy( g_pub_tx.beg_note_no, cPoNo2,16 );
    g_pub_tx.svc_ind=0;
    strcpy(g_pub_tx.no_show,"1");
    ret = pub_ins_trace_log();
    if ( ret )
    {
        sprintf( acErrMsg, "记流水日志错" );
        WRITEMSG
        goto ErrExit;
    }

    sprintf( acErrMsg, "承兑科目为:[%s]",wd_com_parm.val );
    WRITEMSG

    /* 承兑汇票上需要打印的部分固定内容 */
    set_zd_data( "0640", wd_com_parm.val );        /* 承兑汇票记帐科目 */
    set_zd_data( "0250", "晋中银行" );        /* 付款行全称 */
    set_zd_data( "0300", "401175056393");    /* 付款行行号 */
    set_zd_data( "0820", "山西省晋中市榆次区东顺城街129号");    /* 付款行地址 */
    
    /**承兑汇票出票收手续费　add by chenchao 2011-7-28 10:13:51 begin **/
    ret = sql_execute("update mo_bank_po_fee set sts='0' where pact_no='%s' and num=%ld ",cPactNo,iPactNoNum);
    if(ret)
    {
        sprintf( acErrMsg, "修改承兑登记薄失败!!!" );
        WRITEMSG
        goto ErrExit;
    }
    /** 为记流水作准备**/
    /** 开始配置 记账流程  **/
    set_zd_double("0390",0.00);
    set_zd_double("0400",0.00);
    cCt_ind[0] = '2';
    vtcp_log("[%s][%d] dFee_amt=[%.2lf]dPrm_amt[%.2lf] ",__FILE__,__LINE__,dFee_amt,dPrm_amt);
    if(cCt_ind[0] == '1')/** 现金 **/
    {
         vtcp_log("[%s][%d] ct_ind[%s] ",__FILE__,__LINE__,cCt_ind);
         set_zd_data("0680","1");/** 配置现金流程  spM001 **/
         strcpy( g_pub_tx.reply, "0001" );
    }else if(cCt_ind[0] == '2')
    {
         vtcp_log("[%s][%d] ct_ind[%s] ",__FILE__,__LINE__,cCt_ind);
    	 strcpy( g_pub_tx.reply, "0002" );
         set_zd_data("0680","2");/** 配置转账流程  spD003 **/
         set_zd_data("0370",sMoBankAccPo.payer_ac_no);
    }else{
         sprintf( acErrMsg, "现转标志错误!ct_ind=[%s]", cCt_ind);
         WRITEMSG
         strcpy( g_pub_tx.reply, "B099" );
         goto ErrExit;
    }
    ret = sql_count("ln_gage_reg","pact_no='%s' and sts='X' ",cPactNo);
    if(ret)
    {
         sprintf( acErrMsg, "抵质押物还没有入库!ret=[%d]", ret);
         WRITEMSG
         strcpy( g_pub_tx.reply, "SN06" );
         goto ErrExit;
    }
OkExit:
    sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
ErrExit:
    sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
