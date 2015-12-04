/*************************************************************
* 文 件 名: gD090.c
* 功能描述：银行承兑汇票到期自动划缴票款。
*
* 作    者: jane
* 完成日期: 2003年6月2日
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
#include "mo_bank_acc_po_c.h"
#include "mo_bank_po_c.h"
#include "impa_dep_reg_c.h"
#include "mo_bail_rate_c.h"
#include "dc_acc_rel_c.h"
#include "mo_hold_c.h"
#include "com_parm_c.h"

struct    dd_mst_c         sDdMst;        /* 活期主文件 */
struct    td_mst_c         sTdMst;        /* 定期主文件 */
struct    mdm_ac_rel_c     sMdmAcRel;        /* 介质账号对照表 */
struct  com_parm_c    com_parm;

static char  J_RATE_FG;
char *pubf_acct_acctoact(char * ,char *);
double amt_tmp=0.00;/*临时金额*/
gD090()
{
    struct    mo_bank_acc_po_c     sMoBankAccPo;    /* 承兑汇票基本信息登记簿 */
    struct    mo_bank_po_c         sMoBankPo;        /* 承兑汇票登记簿 */
    struct    mo_bail_rate_c       sMoBailRate;    /* 保证金利率登记簿 */
    struct    impa_dep_reg_c       sImpaDepReg;    /* 质押存单登记薄 */
    struct    dc_acc_rel_c         sDcAccRel;        /* 会计科目对照表 */
    
    int     i=0, flag=0, num=0, ret = 0;
    long     day=0;
    long     HQday=0;
    
    char    intst_flag[2];                /* 保证金计息标志 */
    double  td_amt=0.00;
    double  db_amt=0.00;
    double  HQ_rate=0.00;
    double  HQdealval=0.00;
    double  basic_rate=0.00;
    double  sum_po_amt=0.00;   /**add by xyy 2010-6-28 19:58:59 for 退票**/
    double  bzjje=0.00;
    
    double    dTmpAmt, dTempAmt1, dTempAmt2, dTempAmt3;
    
    char    cAcNo[25];                    /* 逾期贷款账号 */
    char    intst_acc[6];
    char    cNewAcno[21];
    char    cNewPactno[17];

    J_RATE_FG = '1'; /**晋中商行要求计息**/
    memset( &sMoBankAccPo, 0x00, sizeof( struct mo_bank_acc_po_c ) );
    memset( &sMoBankPo, 0x00, sizeof( struct mo_bank_po_c ) );
    memset( &sMoBailRate, 0x00, sizeof( struct mo_bail_rate_c ) );
    memset( &sImpaDepReg, 0x00, sizeof( struct impa_dep_reg_c ) );
    memset( &sMdmAcRel, 0x00, sizeof( struct mdm_ac_rel_c ) );
    memset( &sDdMst, 0x00, sizeof( struct dd_mst_c ) );
    memset( &sTdMst, 0x00, sizeof( struct td_mst_c ) );
    memset( &sDcAccRel, 0x00, sizeof( struct dc_acc_rel_c ) );
    memset( cAcNo, 0x00, sizeof( cAcNo ) );
    memset( cNewPactno,0x00,sizeof(cNewPactno));

    ret=sql_begin(); /*打开事务*/
    if( ret ) 
    {
        sprintf( acErrMsg, "打开事务失败!!!" );
        WRITEMSG
        goto ErrExit;
    }

      /**------- 初始化全局变量 --------**/
    pub_base_sysinit();
    strcpy( g_pub_tx.tx_code, "G090");
    if ( pub_base_PubQlsh(&g_pub_tx.trace_no,g_pub_tx.reply))
    {
          sprintf(acErrMsg,"取主机流水号错 [%d]",g_pub_tx.trace_no);
          WRITEMSG
          goto ErrExit;
    }
    set_zd_long( DC_TRACE_NO , g_pub_tx.trace_no);   /* 流水号 */

    /* 根据承兑汇票到期日期查找银行承兑汇票基本信息登记薄 */
    vtcp_log("--------tx_date=[%ld]---------",g_pub_tx.tx_date);
    ret = Mo_bank_acc_po_Dec_Upd( g_pub_tx.reply, "matr_date=%ld and acc_po_ind='0' ", g_pub_tx.tx_date );
    if( ret )
    {
        sprintf( acErrMsg, "执行Mo_bank_acc_po_Dec_Sel错![%d]", ret );
        WRITEMSG
        goto ErrExit;
    }

    sprintf( acErrMsg, "matr_date=%ld", g_pub_tx.tx_date );
    WRITEMSG
    while(1)
    {
        sum_po_amt=0.00;
        td_amt=0.00; /**add by xyy 2010-6-30 9:11:04**/
        db_amt=0.00; /**add by xyy 2010-6-30 9:11:04**/
        memset( &sMoBankAccPo, 0x00, sizeof( struct mo_bank_acc_po_c ) );
        memset(&g_pub_intst, 0x0, sizeof(struct S_pub_intst));
        dTmpAmt=dTempAmt1=dTempAmt2=dTempAmt3=0.00;
        ret = Mo_bank_acc_po_Fet_Upd( &sMoBankAccPo, g_pub_tx.reply );
        if( ret == 100 && flag == 0 )
        {
            break;
        }
        else if( ret == 100 && flag > 0 )
        {
            break;
        }
        else if( ret )
        {
            sprintf( acErrMsg, "执行Mo_bank_acc_po_Fet_Sel错![%d]", ret );
            WRITEMSG
            goto ErrExit;
        }

        pub_base_strpack(sMoBankAccPo.pact_no );
        vtcp_log("[%s][%d] sMoBankAccPo=[%s]",__FILE__,__LINE__,sMoBankAccPo.pact_no);
        if ( pub_base_PubQlsh(&g_pub_tx.trace_no,g_pub_tx.reply))
        {
            sprintf(acErrMsg,"取主机流水号错 [%d]",g_pub_tx.trace_no);
            WRITEMSG
            goto ErrExit;
        }
        set_zd_long( DC_TRACE_NO , g_pub_tx.trace_no);   /* 流水号 */

        /** 到期后自动转帐到申请行243，不存在通兑 **/
        /** 注意: 担保，保证金，质押存单，存款户必须都是申请行帐户 **/
        strcpy(g_pub_tx.opn_br_no,sMoBankAccPo.payer_br_no);
        strcpy(g_pub_tx.tx_br_no,sMoBankAccPo.payer_br_no);

        if( sMoBankAccPo.acc_po_sts[0] != '2' )
        {
            sprintf( acErrMsg, "此承兑协议未做签发或未全部签发!" );
            WRITEMSG
            pub_base_prom_inf( g_pub_tx.tx_br_no , '0' , "承兑协议[%s]未做签发或未全部签发", 
                    sMoBankAccPo.pact_no );
            continue;
        }
        vtcp_log("[%s][%d] 到期处理标志acc_po_ind[%s]",__FILE__,__LINE__,sMoBankAccPo.acc_po_ind);
        if( sMoBankAccPo.acc_po_ind[0] == '1' )
        {
            continue;
        }

        strcpy(intst_flag,"");
        vtcp_log("[%s][%d] bail_amt[%.2lf]",__FILE__,__LINE__,sMoBankAccPo.bail_amt);
        bzjje=sMoBankAccPo.bail_amt;
        if( pub_base_CompDblVal(sMoBankAccPo.bail_amt,sMoBankAccPo.sign_amt)==1 )
        {
            sMoBankAccPo.bail_amt=sMoBankAccPo.sign_amt;
        }
        
        ret=sql_sum_double( "mo_bank_po","po_amt",&sum_po_amt,"pact_no='%s' and po_sts<>'3' ",sMoBankAccPo.pact_no);
        vtcp_log("[%s][%d] 签发总金额=[%f] 实际总金额=[%f]",__FILE__,__LINE__,sMoBankAccPo.sign_amt,sum_po_amt);
        
        /** 因为有的承兑协议编号太长,放到凭证号字段超长,这里特殊处理一下 **/
        if(strlen(sMoBankAccPo.pact_no)>16)
        {
          memcpy(cNewPactno,sMoBankAccPo.pact_no,7);
          memcpy(cNewPactno+7,sMoBankAccPo.pact_no+(strlen(sMoBankAccPo.pact_no)-9),9);
        }
        else
        {
            strcpy(cNewPactno,sMoBankAccPo.pact_no);
        }

        /* 如果有保证金，则扣保证金户上的保证金 */
        if( sMoBankAccPo.bail_amt > 0.005 )
        {
            memset(cNewAcno,0,sizeof(cNewAcno));
            strcpy(cNewAcno,sMoBankAccPo.bail_ac_no);
            pub_base_old_acno( cNewAcno );
            ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdmAcRel, "ac_no='%s'",cNewAcno );
            if( ret == 100 )
            {
                sprintf( acErrMsg, "保证金账号不存在![%s]", cNewAcno );
                WRITEMSG
                strcpy( g_pub_tx.reply, "B195" );
                goto ErrExit;
            }
            else if( ret )
            {
                sprintf( acErrMsg, "执行Mdm_ac_rel_Sel出错![%d]", ret );
                WRITEMSG
                goto ErrExit;
            }
            
            /** 判断如果保证金帐户开户行不是申请行需要做通兑处理 **/
            memset(g_pub_tx.tx_br_no,0x00,sizeof(g_pub_tx.tx_br_no));
            memset(g_pub_tx.opn_br_no,0x00,sizeof(g_pub_tx.opn_br_no));
            strcpy(g_pub_tx.tx_br_no,sMoBankAccPo.payer_br_no);
            strcpy(g_pub_tx.opn_br_no,sMdmAcRel.opn_br_no);

            ret = Dd_mst_Dec_Upd( g_pub_tx.reply,"ac_id=%ld and ac_seqn=%d",\
                    sMdmAcRel.ac_id, sMdmAcRel.ac_seqn );
            if( ret )
            {
                sprintf( acErrMsg, "执行Dd_mst_Sel错误![%d]", ret );
                WRITEMSG
                goto ErrExit;
            }

            ret = Dd_mst_Fet_Upd( &sDdMst, g_pub_tx.reply );
            if( ret == 100)
            {
                sprintf( acErrMsg, "保证金账号不是活期账号!" );
                WRITEMSG
                strcpy( g_pub_tx.reply, "B196" );
                goto ErrExit;
            }
            else if( ret )
            {
                sprintf( acErrMsg, "执行Dd_mst_Sel错误![%d]", ret );
                WRITEMSG
                goto ErrExit;
            }

            /** 如果是冻结, 先解冻 **/
            ret = igD090_DisFrz('1');
            if( ret )
            {
                sprintf( acErrMsg, "自动解冻错误![%d]", ret );
                WRITEMSG
                goto ErrExit;
            }

            ret = Dd_mst_Upd_Upd( sDdMst, g_pub_tx.reply );
            if( ret )
            {
                sprintf( acErrMsg, "保证金账号不是活期账号!" );
                WRITEMSG
                strcpy( g_pub_tx.reply, "B196" );
                goto ErrExit;
            }
            
            ret = Dd_mst_Clo_Upd();
            /***sMoBankAccPo.bail_amt = 0.00; del by chenchao  20111008 清零操作放在下面* 更新保证金金额 ***/

            strcpy( g_pub_tx.ac_no, sMdmAcRel.ac_no );
            /*strcpy( g_pub_tx.ac_id_type, "1" );*/
            g_pub_tx.ac_id_type[0] = '1';
            g_pub_tx.ac_id = sDdMst.ac_id;
            g_pub_tx.ac_seqn = sMdmAcRel.ac_seqn;

           
            strcpy( g_pub_tx.cur_no, "01" );
            strcpy( g_pub_tx.ct_ind, "2" );    
            strcpy( g_pub_tx.prt_ind, "0" );     /*是否打折标志*/
            strcpy( g_pub_tx.brf, "扣保证金" );
						g_pub_tx.add_ind[0] = '0' ; 
            /*strcpy( g_pub_tx.add_ind, "0" ); */    /**减少**/
            strcpy( g_pub_tx.hst_ind, "1" );     /**日间入明细**/
             g_pub_tx.tx_amt1 = sMoBankAccPo.bail_amt;
            vtcp_log("[%s][%d] 保证金取款ac_id=[%ld] [%f]",__FILE__,__LINE__,g_pub_tx.ac_id,g_pub_tx.tx_amt1);
            strcpy( g_pub_tx.beg_note_no, cNewPactno );
						
						strcpy( g_pub_tx.ac_get_ind,"00" ); /*本程序未读取分户*/
            /**检查凭证、零不记流水、零不记明细、检查支取方式**/
            strcpy(g_pub_tx.ac_wrk_ind,"000001001"); 
            g_pub_tx.svc_ind=1001;             /**存取款***/
            /** 记交易流水 **/
            strcpy( g_pub_tx.sub_tx_code, "D003" );
            ret = pub_acct_trance();
            if( ret )
            {
                sprintf(acErrMsg,"调用存取款主控失败!");
                WRITEMSG
                goto ErrExit;
            }
            /* 取款方－会计记帐 */
            set_zd_data( "102J", "01" );    
            set_zd_double( "102F", g_pub_tx.tx_amt1 );
            strcpy( g_pub_tx.sub_tx_code, "D003" );
            strcpy( g_pub_tx.prdt_code, sDdMst.prdt_no );
            ret = pubf_acct_proc( g_pub_tx.sub_tx_code );
            if (ret != 0)
            {
                sprintf(acErrMsg,"会计记帐不成功!!");
                WRITEMSG
                goto ErrExit;
            }

            /**--- xxx ---- 20031107 ---- 计算保证金利息 ----**/
            ret = Mo_bail_rate_Dec_Upd(g_pub_tx.reply," bail_type='1' and pact_no='%s' and sts='1' ", sMoBankAccPo.pact_no );
            if( ret )
            {
                sprintf( acErrMsg, "执行Mo_bail_rate_Dec_Upd错![%d][%s]",ret,sMoBankAccPo.pact_no);
                WRITEMSG
                sprintf( g_pub_tx.reply, "B334" );
                goto ErrExit;
            }
            ret = Mo_bail_rate_Fet_Upd(&sMoBailRate,g_pub_tx.reply);
            if( ret==100 )    
            {
                strcpy(intst_flag,"0");
            }
            else if( ret==0 ) 
            {
                strcpy(intst_flag,"1");/*add by xyy 晋中商行算利息 2010-6-30 10:57:06*/
            }
            else if( ret )
            {
                sprintf( acErrMsg, "执行Mo_bail_rate_Fet_Upd错![%d][%s]",ret,sMoBankAccPo.pact_no);
                WRITEMSG
                sprintf( g_pub_tx.reply, "B334" );
                goto ErrExit;
            }
            vtcp_log("[%s][%d] intst_flag=[%s]",__FILE__,__LINE__,intst_flag);
            if( intst_flag[0]=='1' )
            {
                ret = pub_base_getllz(HQLLBH,"01",g_pub_tx.tx_date,&HQ_rate);
                if( ret )
                {
                    sprintf( acErrMsg, "取利率错误!" );
                    WRITEMSG
                    sprintf( g_pub_tx.reply, "O195" );
                    goto ErrExit;    
                }

                /** 判断是否到期 **/
                if( g_pub_tx.tx_date<sMoBailRate.end_date )     /** 提前支取 **/
                {
                    /** 计算天数 **/
                    if( pub_base_CalDays(sMoBailRate.beg_date,g_pub_tx.tx_date,"0",&HQday) )
                    {
                        sprintf( acErrMsg, "计算天数错误!");
                        WRITEMSG
                        goto ErrExit;
                    }
                    day=0;
                }
                else
                {
                    /** 计算天数 **/
                    if( pub_base_CalDays(sMoBailRate.beg_date,sMoBailRate.end_date,"0",&day) )
                    {
                        sprintf( acErrMsg, "计算天数错误!");
                        WRITEMSG
                        goto ErrExit;
                    }
                    if( pub_base_CalDays(sMoBailRate.end_date,g_pub_tx.tx_date,"0",&HQday) )
                    {
                        sprintf( acErrMsg, "计算天数错误!");
                        WRITEMSG
                        goto ErrExit;
                    }
                }
                sprintf(acErrMsg,"HAO --- look --- HQday=[%ld]HQ_rate=[%lf],day=[%ld],ic_rate=[%lf]",HQday,HQ_rate,day,sMoBailRate.ic_rate);
                WRITEMSG
                /** 计算利息 **/
                /**HQdealval = HQ_rate * sMoBailRate.amt * HQday / L360 / L100; 晋中不计算活期利息了 2011-8-9 16:33:03 **/
                g_pub_intst.dealval = sMoBailRate.ic_rate * sMoBailRate.amt * day / L360 / L100;
                g_pub_intst.dealval = g_pub_intst.dealval + HQdealval;
                g_pub_intst.dealval = pub_base_PubDround_1(g_pub_intst.dealval);
                
                sprintf(acErrMsg,"HAO --- look --- dealval=[%lf]",g_pub_intst.dealval);
                WRITEMSG
                
                if ( pub_base_CompDblVal(g_pub_intst.dealval)<0 )
                {
                    sprintf(acErrMsg,"计算利息错误错误!");
                    WRITEMSG
                    strcpy(g_pub_tx.reply,"D158");
                    goto ErrExit;
                }

                /** 更新保证金计息金额 **/
                sMoBailRate.intst_date=g_pub_tx.tx_date;
                sMoBailRate.end_trace_no=g_pub_tx.trace_no;
                sMoBailRate.intst_amt=g_pub_intst.dealval;
                strcpy(sMoBailRate.sts,"*");
                ret = Mo_bail_rate_Upd_Upd(sMoBailRate,g_pub_tx.reply);
                if( ret )
                {
                    sprintf( acErrMsg, "执行Mo_bail_rate_Upd_Upd错![%d][%s]",ret,sMoBankAccPo.pact_no);
                    WRITEMSG
                    sprintf( g_pub_tx.reply, "B334" );
                    goto ErrExit;
                }
                Mo_bail_rate_Clo_Upd();

                g_pub_intst.tax_intst=g_pub_intst.dealval;    /** 应税利息 **/
                g_pub_intst.factval=g_pub_intst.dealval;    /** 实付利息 **/
                g_pub_intst.val=0.00;                        /** 利息税 **/
                g_pub_intst.keepval=0.00;                    /** 保值利息 **/

                /** 利息和利息税不需要做通兑处理 **/
                memset(g_pub_tx.tx_br_no,0x00,sizeof(g_pub_tx.tx_br_no));
                memset(g_pub_tx.opn_br_no,0x00,sizeof(g_pub_tx.opn_br_no));
                strcpy(g_pub_tx.tx_br_no,sMdmAcRel.opn_br_no);
                strcpy(g_pub_tx.opn_br_no,sMdmAcRel.opn_br_no);

                /**--- xxx --- 20031106 ------ 保证金利息入帐 *
                if( pub_base_CompDblVal(g_pub_intst.dealval,0.00)>0 )
                    **/
                if(J_RATE_FG=='1')/* 到期处理不计息，改到销户时计息 */
                {
                   /** 登记利息会计流水 **/
                   g_pub_tx.tx_amt1 = g_pub_intst.dealval+g_pub_intst.keepval-g_pub_intst.val;
                   set_zd_data("0153","52101");        /** 活期利息支出科目 **/
                   set_zd_double("102P",g_pub_tx.tx_amt1);
                   ret = pubf_acct_proc("B373");
                   if (ret != 0)
                   {
                        sprintf(acErrMsg,"会计记帐不成功!!");
                        WRITEMSG
                        goto ErrExit;
                   }
                   set_zd_double("102P",0.00);
                

                    /* 检查付款帐号的有效性:活期账号、账户状态正常 */
                    ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdmAcRel, "ac_no='%s'",sMoBailRate.ac_no);
                    if( ret == 100 )
                    {
                        sprintf( acErrMsg, "保证金收息账号不存在![%s]", sMoBankAccPo.payer_ac_no );
                        WRITEMSG
                        strcpy( g_pub_tx.reply, "B339" );
                        goto ErrExit;
                    }
                    else if( ret )
                    {
                        sprintf( acErrMsg, "执行Mdm_ac_rel_Sel出错![%d]", ret );
                        WRITEMSG
                        goto ErrExit;
                    }

                    ret = Dd_mst_Sel( g_pub_tx.reply, &sDdMst, "ac_id=%ld and ac_seqn=%d",\
                        sMdmAcRel.ac_id, sMdmAcRel.ac_seqn );
                    if( ret == 100 )
                    {
                        sprintf( acErrMsg, "保证金账号不是活期账号!" );
                        WRITEMSG
                        strcpy( g_pub_tx.reply, "B179" );
                        goto ErrExit;
                    }
                    else if( ret )
                    {
                        sprintf( acErrMsg, "执行Dd_mst_Sel错误![%d]", ret );
                        WRITEMSG
                        goto ErrExit;
                    }

                    if( sDdMst.ac_sts[0] == '*' )
                    {
                        sprintf( acErrMsg, "此付款人账号已销户!" );
                        WRITEMSG
                        strcpy( g_pub_tx.reply, "B231" );
                        goto ErrExit;
                    }

                    /** 如果保证金和利息收入帐户不同机构,利息入帐做通存处理 **/
                    memset(g_pub_tx.opn_br_no,0x00,sizeof(g_pub_tx.opn_br_no));
                    strcpy(g_pub_tx.opn_br_no,sMdmAcRel.opn_br_no);
              
                    strcpy( g_pub_tx.ac_no, sMoBailRate.ac_no );
                    strcpy( g_pub_tx.ac_id_type, "1" );
                    g_pub_tx.ac_id = sDdMst.ac_id;
                    g_pub_tx.ac_seqn = sMdmAcRel.ac_seqn;
                    strcpy( g_pub_tx.cur_no, "01" );
                    strcpy( g_pub_tx.ct_ind, "2" );    
                    strcpy( g_pub_tx.prt_ind, "0" );     /*是否打折标志*/
                    strcpy( g_pub_tx.brf, "利息入账" );
                    strcpy( g_pub_tx.add_ind, "1" );     /**增加**/
                    strcpy( g_pub_tx.hst_ind, "1" );     /**日间入明细**/

                    vtcp_log("[%s][%d] 利息入账ac_id=[%d] [%f]",__FILE__,__LINE__,g_pub_tx.ac_id,g_pub_tx.tx_amt1);

                    /**检查凭证、零不记流水、零不记明细、检查支取方式**/
                    strcpy(g_pub_tx.ac_wrk_ind,"100001001"); 
                    g_pub_tx.svc_ind=1001;             /**存取款***/
                    /** 记交易流水 **/
                    strcpy( g_pub_tx.sub_tx_code, "D099" );
                    ret = pub_acct_trance();
                    if( ret )
                    {
                        sprintf(acErrMsg,"调用存取款主控失败!");
                        WRITEMSG
                        goto ErrExit;
                    }
                    /* 会计记帐 */
                    set_zd_data( "101A", "01" );    
                    set_zd_double( "1013", g_pub_tx.tx_amt1 );
                    strcpy( g_pub_tx.sub_tx_code, "D099" );
                    strcpy( g_pub_tx.prdt_code, sDdMst.prdt_no );
                    ret = pubf_acct_proc( g_pub_tx.sub_tx_code );
                    if (ret != 0)
                    {
                         sprintf(acErrMsg,"会计记帐不成功!!");
                         WRITEMSG
                         goto ErrExit;
                    }
                    
                }
            }
            WRITEMSG
        }

        /* 如果有质押存单的，则支取存单，存单利息归还结算户 */
        /**************************************************************************/
        /*********注意:晋中不采用自动下存单账务的方式,这儿直接跳过*****************/
        /*************** modify by chenchao 2011-8-6 10:26:06**********************/
        if(1==2 && sMoBankAccPo.tot_mort_amt > 0.005 && sMoBankAccPo.acc_type[0]=='2')
        {
            vtcp_log("[%s][%d] 质押总金额tot_mort_amt[%.2lf] 签发总金额=[%f]",__FILE__,__LINE__,sMoBankAccPo.tot_mort_amt,sum_po_amt);
            memset(&g_pub_intst, 0x0, sizeof(struct S_pub_intst));
            ret = Impa_dep_reg_Dec_Sel( g_pub_tx.reply, "pact_no='%s'", sMoBankAccPo.pact_no );
            if( ret )
            {
                sprintf( acErrMsg, "执行Impa_dep_reg_Dec_Sel错![%d]", ret );
                WRITEMSG
                goto ErrExit;
            }
            td_amt=0.00;
            while(1)
            {
                memset(&sImpaDepReg , 0 , sizeof(sImpaDepReg));
                ret = Impa_dep_reg_Fet_Sel( &sImpaDepReg, g_pub_tx.reply );
                if( ret == 100 && num == 0 )
                {
                    vtcp_log("[%s][%d] num=[%d]",__FILE__,__LINE__,num);
                    break;
                }
                else if( ret == 100 && num > 0 )
                {
                    vtcp_log("[%s][%d] 定单总金额td_amt=[%f] sum_po_amt=[%f]",__FILE__,__LINE__,sum_po_amt);
                    /*****************add by xyy 2010-6-28 20:25:25 定单合计金额大于承兑汇票签发总金额******************/
                    if( pub_base_CompDblVal(td_amt,sum_po_amt)>0 )
                    {
                        /* 检查付款帐号的有效性:活期账号、账户状态正常 */
                        vtcp_log("[%s][%d] 定单合计值多,其活期账户为=[%s]",__FILE__,__LINE__,sMoBankAccPo.payer_ac_no);
                        ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdmAcRel, "ac_no='%s'",sMoBankAccPo.payer_ac_no);
                        if( ret == 100 )
                        {
                            sprintf( acErrMsg, "付款人账号不存在![%s]", sMoBankAccPo.payer_ac_no );
                            WRITEMSG
                            strcpy( g_pub_tx.reply, "D164" );
                            goto ErrExit;
                        }
                        else if( ret )
                        {
                            sprintf( acErrMsg, "执行Mdm_ac_rel_Sel出错![%d]", ret );
                            WRITEMSG
                            goto ErrExit;
                        }
                        
                        ret = Dd_mst_Sel( g_pub_tx.reply, &sDdMst, "ac_id=%ld and ac_seqn=%d",\
                                sMdmAcRel.ac_id, sMdmAcRel.ac_seqn );
                        if( ret == 100 )
                        {
                            sprintf( acErrMsg, "付款人账号不是活期账号!" );
                            WRITEMSG
                            strcpy( g_pub_tx.reply, "B179" );
                            goto ErrExit;
                        }
                        else if( ret )
                        {
                            sprintf( acErrMsg, "执行Dd_mst_Sel错误![%d]", ret );
                            WRITEMSG
                            goto ErrExit;
                        }
                        
                        if( sDdMst.ac_sts[0] == '*' )
                        {
                            sprintf( acErrMsg, "此付款人账号已销户!" );
                            WRITEMSG
                            strcpy( g_pub_tx.reply, "B231" );
                            goto ErrExit;
                        }
                        
                        /** 质压存单和利息入帐行不同时，利息通存处理 **/
                        memset(g_pub_tx.opn_br_no,0x00,sizeof(g_pub_tx.opn_br_no));
                        strcpy(g_pub_tx.opn_br_no,sMdmAcRel.opn_br_no);
                        
                        g_pub_tx.tx_amt1 = td_amt - sum_po_amt;   
                        vtcp_log("[%s][%d] 多余款项划入活期户[%f]",__FILE__,__LINE__,g_pub_tx.tx_amt1);         
                        td_amt = td_amt -g_pub_tx.tx_amt1;
                        vtcp_log("[%s][%d] 划转后的定期存单合计值[%f]",__FILE__,__LINE__,td_amt);
                        
                        
                        strcpy( g_pub_tx.ac_no, sMoBankAccPo.payer_ac_no );
                        strcpy( g_pub_tx.ac_id_type, "1" );
                        g_pub_tx.ac_id = sDdMst.ac_id;
                        g_pub_tx.ac_seqn = sMdmAcRel.ac_seqn;
                        strcpy( g_pub_tx.cur_no, "01" );
                        strcpy( g_pub_tx.ct_ind, "2" );    
                        strcpy( g_pub_tx.prt_ind, "0" );     /*是否打折标志*/
                        strcpy( g_pub_tx.brf, "定单多余款" );
                        
                        strcpy( g_pub_tx.add_ind, "1" );     /**增加**/
                        strcpy( g_pub_tx.hst_ind, "1" );     /**日间入明细**/
                        
                        /**检查凭证、零不记流水、零不记明细、检查支取方式**/
                        strcpy(g_pub_tx.ac_wrk_ind,"100001001"); 
                        g_pub_tx.svc_ind=1001;             /**存取款***/
                        /** 记交易流水 **/
                        strcpy( g_pub_tx.sub_tx_code, "d099" );
                        ret = pub_acct_trance();
                        if( ret )
                        {
                            sprintf(acErrMsg,"调用存取款主控失败!");
                            WRITEMSG
                            goto ErrExit;
                        }
                        /* 会计记帐 */
                        set_zd_data( "101A", "01" );    
                        set_zd_double( "1013", g_pub_tx.tx_amt1 );
                        strcpy( g_pub_tx.sub_tx_code, "d099" );
                        strcpy( g_pub_tx.prdt_code, sDdMst.prdt_no );
                        ret = pubf_acct_proc( g_pub_tx.sub_tx_code );
                        if (ret != 0)
                        {
                             sprintf(acErrMsg,"会计记帐不成功!!");
                             WRITEMSG
                             goto ErrExit;
                        }
                    }
                    vtcp_log("[%s][%d] 钱还不够?",__FILE__,__LINE__);
                    break;
                }
                else if( ret )
                {
                    sprintf( acErrMsg, "执行Impa_dep_reg_Fet_Sel错![%d]", ret );
                    WRITEMSG
                    goto ErrExit;
                }
                memset(&g_pub_intst, 0x0, sizeof(struct S_pub_intst));

                /* 检查质押存单的有效性:定期账号、账户状态为完全冻结,先解冻再销户 */
                ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdmAcRel, "ac_no='%s'",\
                            sImpaDepReg.mort_ac_no );
                if( ret == 100 )
                {
                    sprintf( acErrMsg, "质押存单不存在![%s]", sImpaDepReg.mort_ac_no );
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

                /** 判断如果保证金帐户开户行不是申请行需要做通兑处理 **/
                memset(g_pub_tx.tx_br_no,0x00,sizeof(g_pub_tx.tx_br_no));
                memset(g_pub_tx.opn_br_no,0x00,sizeof(g_pub_tx.opn_br_no));
                strcpy(g_pub_tx.tx_br_no,sMoBankAccPo.payer_br_no);
                strcpy(g_pub_tx.opn_br_no,sMdmAcRel.opn_br_no);

                ret = Td_mst_Dec_Upd( g_pub_tx.reply , "ac_id=%ld and ac_seqn=%d",\
                            sMdmAcRel.ac_id, sMdmAcRel.ac_seqn );
                if ( ret )
                {
                    sprintf( acErrMsg, "执行Td_mst_Dec_Upd错[%d]", ret );
                    WRITEMSG
                    goto ErrExit;
                }

                ret = Td_mst_Fet_Upd( &sTdMst, g_pub_tx.reply );
                if( ret == 100 )
                {
                    sprintf( acErrMsg, "质押账号不是定期存单号!" );
                    WRITEMSG
                    strcpy( g_pub_tx.reply, "B184" );
                    goto ErrExit;
                }
                else if( ret )
                {
                    sprintf( acErrMsg, "执行Td_mst_Fet_Upd错误![%d]", ret );
                    WRITEMSG
                    goto ErrExit;
                }
                /* add by rob at 20031206 */
                if ( sTdMst.ac_sts[0] != '1' )
                {
                    continue;
                }

                /** 如果是冻结, 先解冻 **/
                ret = igD090_DisFrz('2');
                if( ret )
                {
                    sprintf( acErrMsg, "自动解冻错误![%d]", ret );
                    WRITEMSG
                    goto ErrExit;
                }
                vtcp_log("[%s][%d] 存单支取=[%ld] bal=[%f]",__FILE__,__LINE__,sMdmAcRel.ac_id,sTdMst.bal);
                ret = Td_mst_Upd_Upd( sTdMst, g_pub_tx.reply );
                    if ( ret )
                {
                    sprintf( acErrMsg, "执行Td_mst_Upd_Upd错[%d]", ret );
                    WRITEMSG
                    goto ErrExit;    
                }
                Td_mst_Clo_Upd( );


                g_pub_tx.tx_amt1 = sTdMst.bal;
                td_amt = td_amt + sTdMst.bal;
                strcpy( g_pub_tx.ac_no, sImpaDepReg.mort_ac_no );
                strcpy( g_pub_tx.ac_id_type, "2" );
                g_pub_tx.ac_id = sTdMst.ac_id;
                g_pub_tx.ac_seqn = sMdmAcRel.ac_seqn;
                strcpy( g_pub_tx.cur_no, "01" );
                strcpy( g_pub_tx.ct_ind, "2" );    
                strcpy( g_pub_tx.prt_ind, "0" );     /*是否打折标志*/
                strcpy( g_pub_tx.brf, "支取存单" );

                strcpy( g_pub_tx.add_ind, "0" );     /**减少**/
                strcpy( g_pub_tx.hst_ind, "1" );     /**日间入明细**/

                strcpy( g_pub_tx.beg_note_no, cNewPactno );

                /**检查凭证、零不记流水、零不记明细、检查支取方式**/
                strcpy(g_pub_tx.ac_wrk_ind,"000001001"); 
                g_pub_tx.svc_ind=1003;             /**销户***/
                /** 记交易流水 **/
                vtcp_log("[%s][%d] 调用销户交易 ac_id=[%ld]",__FILE__,__LINE__,sTdMst.ac_id);
                strcpy( g_pub_tx.sub_tx_code, "D203" );
                ret = pub_acct_trance();
                if( ret )
                {
                    sprintf(acErrMsg,"调用存取款主控失败!");
                    WRITEMSG
                    goto ErrExit;
                }
                /* 销户方－会计记帐 */
                set_zd_data( "102J", "01" );    
                set_zd_double( "102I", g_pub_tx.tx_amt1 );
                strcpy( g_pub_tx.sub_tx_code, "D203" );
                strcpy( g_pub_tx.prdt_code, sTdMst.prdt_no );
                ret = pubf_acct_proc( g_pub_tx.sub_tx_code );
                if (ret != 0)
                {
                    sprintf(acErrMsg,"会计记帐不成功!!");
                    WRITEMSG
                    goto ErrExit;
                }

                /* 利息会计记账 */
                vtcp_log("[%s][%d] 准备计算利息dc_code=[%s]",__FILE__,__LINE__,g_td_parm.dc_code);
                ret=Dc_acc_rel_Sel(g_pub_tx.reply,&sDcAccRel,"data_code='0153' and dc_code='%s' ",g_td_parm.dc_code);
                if( ret )
                {
                    sprintf(acErrMsg,"取产品利息科目错误!!");
                    WRITEMSG
                    strcpy(g_pub_tx.reply,"B345");
                    goto ErrExit;
                }
        
                /** 利息和利息税不需要做通兑处理 **/
                memset(g_pub_tx.tx_br_no,0x00,sizeof(g_pub_tx.tx_br_no));
                memset(g_pub_tx.opn_br_no,0x00,sizeof(g_pub_tx.opn_br_no));
                strcpy(g_pub_tx.tx_br_no,sMdmAcRel.opn_br_no);
                strcpy(g_pub_tx.opn_br_no,sMdmAcRel.opn_br_no);
                vtcp_log("[%s][%d] 应付利息 dealval=[%f] ",__FILE__,__LINE__,g_pub_intst.dealval);
                vtcp_log("[%s][%d] 保值利息 keepval=[%f] ",__FILE__,__LINE__,g_pub_intst.keepval);
                vtcp_log("[%s][%d] 利息税       val=[%f] ",__FILE__,__LINE__,g_pub_intst.val);
                
                /******************modify by xyy 2010-6-29 17:44:28 应该包含利息税的
                g_pub_tx.tx_amt1 = g_pub_intst.dealval+g_pub_intst.keepval-g_pub_intst.val;
                *******************/
                g_pub_tx.tx_amt1 = g_pub_intst.dealval+g_pub_intst.keepval;
                
                
                strncpy(intst_acc,sDcAccRel.acc_hrt,5);
                vtcp_log("[%s][%d] 利息科目为[%s] 利息金额tx_amt1=[%f]",__FILE__,__LINE__,intst_acc,g_pub_tx.tx_amt1);
                intst_acc[5]='\0';
                set_zd_data("0153",intst_acc);
                set_zd_double("102P",g_pub_tx.tx_amt1);
                ret = pubf_acct_proc("B373");
                if (ret != 0)
                {
                     sprintf(acErrMsg,"会计记帐不成功!!");
                     WRITEMSG
                     goto ErrExit;
                }
                set_zd_double("102P",0.00);

                /** 利息税 **/
                vtcp_log("[%s][%d] 利息税g_pub_intst.val=[%f]",__FILE__,__LINE__,g_pub_intst.val);
                if( pub_base_CompDblVal(g_pub_intst.val,0.00) >0 )
                {
                     /** 登记利息税会计流水 modify by xyy 2010-6-29 21:39:34 for 利息税科目记错**/
                     char cAccHrt[6];
                     memset(cAccHrt , 0 , sizeof(cAccHrt));
                     get_zd_data(DC_CODE,cAccHrt);
                     vtcp_log("[%s][%d] cAccHrt =[%s] ",__FILE__,__LINE__,cAccHrt);
                     set_zd_data(DC_CODE,"ACHR");                     
                     g_pub_tx.tx_amt1 = g_pub_intst.val;
                     set_zd_data("0153","26800");        /** 活期利息税支出科目 **/
                     set_zd_double("102P",g_pub_tx.tx_amt1);
                     /****** 这里需要改一下借贷标志,这是错误的*****
                     ret = pubf_acct_proc("B373");
                     **********************************************/
                     ret = pubf_acct_proc("B3XX");
                     if (ret != 0)
                     {
                          sprintf(acErrMsg,"会计记帐不成功!!");
                          WRITEMSG
                          goto ErrExit;
                     }
                     set_zd_data(DC_CODE,cAccHrt);
                     /*****************************end by xyy 2010-6-29 21:39:57*****************/
                     
                     set_zd_double("102P",0.00);
                }

                /* 存单利息归付款账号 */
                /* 检查付款帐号的有效性:活期账号、账户状态正常 */
                ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdmAcRel, "ac_no='%s'",sMoBankAccPo.payer_ac_no);
                if( ret == 100 )
                {
                    sprintf( acErrMsg, "付款人账号不存在![%s]", sMoBankAccPo.payer_ac_no );
                    WRITEMSG
                    strcpy( g_pub_tx.reply, "D164" );
                    goto ErrExit;
                }
                else if( ret )
                {
                    sprintf( acErrMsg, "执行Mdm_ac_rel_Sel出错![%d]", ret );
                    WRITEMSG
                    goto ErrExit;
                }

                ret = Dd_mst_Sel( g_pub_tx.reply, &sDdMst, "ac_id=%ld and ac_seqn=%d",\
                        sMdmAcRel.ac_id, sMdmAcRel.ac_seqn );
                if( ret == 100 )
                {
                    sprintf( acErrMsg, "付款人账号不是活期账号!" );
                    WRITEMSG
                    strcpy( g_pub_tx.reply, "B179" );
                    goto ErrExit;
                }
                else if( ret )
                {
                    sprintf( acErrMsg, "执行Dd_mst_Sel错误![%d]", ret );
                    WRITEMSG
                    goto ErrExit;
                }

                if( sDdMst.ac_sts[0] == '*' )
                {
                    sprintf( acErrMsg, "此付款人账号已销户!" );
                    WRITEMSG
                    strcpy( g_pub_tx.reply, "B231" );
                    goto ErrExit;
                }

                /** 质压存单和利息入帐行不同时，利息通存处理 **/
                memset(g_pub_tx.opn_br_no,0x00,sizeof(g_pub_tx.opn_br_no));
                strcpy(g_pub_tx.opn_br_no,sMdmAcRel.opn_br_no);

                g_pub_tx.tx_amt1 = g_pub_intst.dealval+g_pub_intst.keepval-g_pub_intst.val;            
                strcpy( g_pub_tx.ac_no, sMoBankAccPo.payer_ac_no );
                strcpy( g_pub_tx.ac_id_type, "1" );
                g_pub_tx.ac_id = sDdMst.ac_id;
                g_pub_tx.ac_seqn = sMdmAcRel.ac_seqn;
                strcpy( g_pub_tx.cur_no, "01" );
                strcpy( g_pub_tx.ct_ind, "2" );    
                strcpy( g_pub_tx.prt_ind, "0" );     /*是否打折标志*/
                strcpy( g_pub_tx.brf, "利息入账" );

                strcpy( g_pub_tx.add_ind, "1" );     /**增加**/
                strcpy( g_pub_tx.hst_ind, "1" );     /**日间入明细**/

                /**检查凭证、零不记流水、零不记明细、检查支取方式**/
                strcpy(g_pub_tx.ac_wrk_ind,"100001001"); 
                g_pub_tx.svc_ind=1001;             /**存取款***/
                /** 记交易流水 **/
                strcpy( g_pub_tx.sub_tx_code, "D099" );
                ret = pub_acct_trance();
                if( ret )
                {
                    sprintf(acErrMsg,"调用存取款主控失败!");
                    WRITEMSG
                    goto ErrExit;
                }
                /* 会计记帐 */
                set_zd_data( "101A", "01" );    
                set_zd_double( "1013", g_pub_tx.tx_amt1 );
                strcpy( g_pub_tx.sub_tx_code, "D099" );
                strcpy( g_pub_tx.prdt_code, sDdMst.prdt_no );
                ret = pubf_acct_proc( g_pub_tx.sub_tx_code );
                if (ret != 0)
                {
                     sprintf(acErrMsg,"会计记帐不成功!!");
                     WRITEMSG
                     goto ErrExit;
                }
                num++;
            }
            Impa_dep_reg_Clo_Sel();
        }

        /** 担保承兑，扣担保帐户 **/
        if(sMoBankAccPo.acc_type[0] == '4')
        {
                dTempAmt1 = sMoBankAccPo.bail_amt + td_amt;
                /****
                ret = pub_base_CompDblVal( dTempAmt1, sMoBankAccPo.sign_amt );
                ****/
                ret = pub_base_CompDblVal( dTempAmt1, sum_po_amt);
                if( ret < 0 )
                {
                    memset(cNewAcno,0,sizeof(cNewAcno));
                    strcpy(cNewAcno,sMoBankAccPo.bail_ac_no);
                    pub_base_old_acno( cNewAcno );
                    
                    /* 检查付款帐号的有效性:活期账号、账户状态正常 */
                    ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdmAcRel, "ac_no='%s'",cNewAcno);
                    if( ret == 100 )
                    {
                        sprintf( acErrMsg, "付款人账号不存在![%s]", cNewAcno );
                        WRITEMSG
                        strcpy( g_pub_tx.reply, "D164" );
                        goto ErrExit;
                    }
                    else if( ret )
                    {
                        sprintf( acErrMsg, "执行Mdm_ac_rel_Sel出错![%d]", ret );
                        WRITEMSG
                        goto ErrExit;
                    }

                    ret = Dd_mst_Sel( g_pub_tx.reply, &sDdMst, "ac_id=%ld and ac_seqn=%d",\
                            sMdmAcRel.ac_id, sMdmAcRel.ac_seqn );
                    if( ret == 100 )
                    {
                        sprintf( acErrMsg, "付款人账号不是活期账号!" );
                        WRITEMSG
                        strcpy( g_pub_tx.reply, "B179" );
                        goto ErrExit;
                    }
                    else if( ret )
                    {
                        sprintf( acErrMsg, "执行Dd_mst_Sel错误![%d]", ret );
                        WRITEMSG
                        goto ErrExit;
                    }

                    /* 仅当账户状态正常时才去扣账 */
                    if( sDdMst.ac_sts[0] == '1' )
                    {
                        if( sDdMst.bal > 0.005 )
                        {
                            /*****
                            dTmpAmt = sMoBankAccPo.sign_amt - dTempAmt1;
                            ******/
                            dTmpAmt = sum_po_amt - dTempAmt1;
                            /**----- xxx ---- 冻结部分不能自动扣
                            ----------- 保留最低余额 ----------**/
                            ret=Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm,"prdt_no='%s' ",sDdMst.prdt_no);
                            if( ret )
                            {
                                sprintf(acErrMsg,"取产品参数表错误!");
                                WRITEMSG
                                strcpy(g_pub_tx.reply,"W024");
                                goto ErrExit;
                            }
                            if( sDdMst.hold_sts[0]=='1' || sDdMst.hold_sts[0]=='2' )
                            {
                                vtcp_log("[%s][%d] goto BBBBB!!!",__FILE__,__LINE__);
                                goto BBBBB;
                            }
                            else if( sDdMst.hold_sts[0]=='3' )
                            {
                                ret=pub_base_CompDblVal(sDdMst.bal-sDdMst.hold_amt-sDdMst.ctl_amt-g_dd_parm.min_bal,0.00);
                                if( ret<=0 )
                                {
                                    vtcp_log("[%s][%d] goto BBBBB!!!",__FILE__,__LINE__);
                                    goto BBBBB;
                                }

                                ret=pub_base_CompDblVal(dTmpAmt,sDdMst.bal-sDdMst.hold_amt-sDdMst.ctl_amt-g_dd_parm.min_bal);
                                if( ret<=0 )
                                {
                                    g_pub_tx.tx_amt1 = dTmpAmt;
                                }
                                else
                                {
                                    g_pub_tx.tx_amt1 = sDdMst.bal-sDdMst.hold_amt-sDdMst.ctl_amt-g_dd_parm.min_bal;                
                                }
                                vtcp_log("[%s][%d] tx_amt1=[%f]",__FILE__,__LINE__,g_pub_tx.tx_amt1);
                            }
                            else    /** 非冻结 **/
                            {
                                ret=pub_base_CompDblVal(sDdMst.bal-sDdMst.ctl_amt-g_dd_parm.min_bal,0.00);
                                if( ret<=0 )
                                {
                                    vtcp_log("[%s][%d] goto BBBBB!!!",__FILE__,__LINE__);
                                    goto BBBBB;
                                }

                                ret=pub_base_CompDblVal(dTmpAmt,sDdMst.bal-sDdMst.ctl_amt-g_dd_parm.min_bal);
                                if( ret<=0 )
                                {
                                    g_pub_tx.tx_amt1 = dTmpAmt;
                                }
                                else
                                {
                                    g_pub_tx.tx_amt1 = sDdMst.bal-sDdMst.ctl_amt-g_dd_parm.min_bal;
                                }
                                vtcp_log("[%s][%d] tx_amt1=[%f]",__FILE__,__LINE__,g_pub_tx.tx_amt1);
                            }
                            /**--------- xxx ------------ end ----------**/
                            /** 判断如果保证金帐户开户行不是申请行需要做通兑处理 **/
                            memset(g_pub_tx.tx_br_no,0x00,sizeof(g_pub_tx.tx_br_no));
                            memset(g_pub_tx.opn_br_no,0x00,sizeof(g_pub_tx.opn_br_no));
                            strcpy(g_pub_tx.tx_br_no,sMoBankAccPo.payer_br_no);
                            strcpy(g_pub_tx.opn_br_no,sMdmAcRel.opn_br_no);
                            
                            db_amt = g_pub_tx.tx_amt1;
                            vtcp_log("[%s][%d] 承兑汇票扣款金额db_amt=[%f]",__FILE__,__LINE__,db_amt);
                            strcpy( g_pub_tx.ac_no, cNewAcno );
                            strcpy( g_pub_tx.ac_id_type, "1" );
                            strcpy( g_pub_tx.beg_note_no, cNewPactno );
                            g_pub_tx.ac_id = sDdMst.ac_id;
                            g_pub_tx.ac_seqn = sMdmAcRel.ac_seqn;
                                strcpy( g_pub_tx.cur_no, "01" );
                            strcpy( g_pub_tx.ct_ind, "2" );    
                            strcpy( g_pub_tx.prt_ind, "0" );     /*是否打折标志*/
                            strcpy( g_pub_tx.brf, "承兑扣款" );

                            strcpy( g_pub_tx.add_ind, "0" );     /**减少**/
                            strcpy( g_pub_tx.hst_ind, "1" );     /**日间入明细**/

                            /**检查凭证、零不记流水、零不记明细、检查支取方式**/
                            strcpy(g_pub_tx.ac_wrk_ind,"000001001"); 
                            g_pub_tx.svc_ind=1001;             /**存取款***/
                            /** 记交易流水 **/
                                strcpy( g_pub_tx.sub_tx_code, "D003" );
                            ret = pub_acct_trance();
                            if( ret )
                            {
                                sprintf(acErrMsg,"调用存取款主控失败!");
                                WRITEMSG
                                goto ErrExit;
                            }
                            /* 取款方－会计记帐 */
                            set_zd_data( "102J", "01" );    
                            set_zd_double( "102F", g_pub_tx.tx_amt1 );
                            strcpy( g_pub_tx.sub_tx_code, "D003" );
                            strcpy( g_pub_tx.prdt_code, sDdMst.prdt_no );
                            ret = pubf_acct_proc( g_pub_tx.sub_tx_code );
                            if (ret != 0)
                            {
                                    sprintf(acErrMsg,"会计记帐不成功!!");
                                    WRITEMSG
                                    goto ErrExit;
                            }
                        }
                    }
                }

        }

BBBBB:
        /* 如果金额仍然不足，扣客户帐 */
        /* xxx 20031108 按实际销户的金额计算 *
        dTempAmt1 = sMoBankAccPo.bail_amt + sMoBankAccPo.tot_mort_amt;
        **************************************/
        vtcp_log("[%s][%d] bail_amt=[%f][%f][%f]",__FILE__,__LINE__,sMoBankAccPo.bail_amt , td_amt , db_amt);
        dTempAmt1 = sMoBankAccPo.bail_amt + td_amt + db_amt;
        vtcp_log("[%s][%d]dTempAmt1=[%f] sum_po_amt=[%f]",__FILE__,__LINE__,dTempAmt1,sum_po_amt);
        /****
        ret = pub_base_CompDblVal( dTempAmt1, sMoBankAccPo.sign_amt );
        ******/
        ret = pub_base_CompDblVal( dTempAmt1, sum_po_amt );
        if( ret < 0 )
        {
            /* 检查付款帐号的有效性:活期账号、账户状态正常 */
            memset(cNewAcno,0,sizeof(cNewAcno));
            strcpy(cNewAcno,sMoBankAccPo.payer_ac_no);
            pub_base_old_acno( cNewAcno );
            ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdmAcRel, "ac_no='%s'",cNewAcno);
            if( ret == 100 )
            {
                sprintf( acErrMsg, "付款人账号不存在![%s]", sMoBankAccPo.payer_ac_no );
                WRITEMSG
                strcpy( g_pub_tx.reply, "D164" );
                goto ErrExit;
            }
            else if( ret )
            {
                sprintf( acErrMsg, "执行Mdm_ac_rel_Sel出错![%d]", ret );
                WRITEMSG
                goto ErrExit;
            }

            ret = Dd_mst_Sel( g_pub_tx.reply, &sDdMst, "ac_id=%ld and ac_seqn=%d",\
                    sMdmAcRel.ac_id, sMdmAcRel.ac_seqn );
            if( ret == 100 )
            {
                sprintf( acErrMsg, "付款人账号不是活期账号!" );
                WRITEMSG
                strcpy( g_pub_tx.reply, "B179" );
                goto ErrExit;
            }
            else if( ret )
            {
                sprintf( acErrMsg, "执行Dd_mst_Sel错误![%d]", ret );
                WRITEMSG
                goto ErrExit;
            }

            /* 仅当账户状态正常时才去扣账 */
            if( sDdMst.ac_sts[0] == '1' )
            {
                if( sDdMst.bal > 0.005 )
                {
                    /******
                    dTmpAmt = sMoBankAccPo.sign_amt - dTempAmt1;
                    ******/
                    
                    dTmpAmt = sum_po_amt - dTempAmt1;
                    /**----- xxx ---- 冻结部分不能自动扣
                    ----------- 保留最低余额 ----------**/
                    ret=Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm,"prdt_no='%s' ",sDdMst.prdt_no);
                    if( ret )
                    {
                        sprintf(acErrMsg,"取产品参数表错误!");
                        WRITEMSG
                        strcpy(g_pub_tx.reply,"W024");
                        goto ErrExit;
                    }
                    vtcp_log("[%s][%d] hold_sts=[%s]",__FILE__,__LINE__,sDdMst.hold_sts);
                    if( sDdMst.hold_sts[0]=='1' || sDdMst.hold_sts[0]=='2' )
                    {
                        vtcp_log("[%s][%d] goto AAAAA!!!",__FILE__,__LINE__);
                        goto AAAAA;
                    }
                    else if( sDdMst.hold_sts[0]=='3' )
                    {
                        ret=pub_base_CompDblVal(sDdMst.bal-sDdMst.hold_amt-sDdMst.ctl_amt-g_dd_parm.min_bal,0.00);
                        if( ret<=0 )
                        {
                            vtcp_log("[%s][%d] AAAAA [%f][%f][%f][%f]",__FILE__,__LINE__,sDdMst.bal,sDdMst.hold_amt,sDdMst.ctl_amt,g_dd_parm.min_bal);
                            goto AAAAA;
                        }

                        ret=pub_base_CompDblVal(dTmpAmt,sDdMst.bal-sDdMst.hold_amt-sDdMst.ctl_amt-g_dd_parm.min_bal);
                        if( ret<=0 )
                        {
                            vtcp_log("[%s][%d] dTmpAmt=[%f]",__FILE__,__LINE__,dTmpAmt);
                            g_pub_tx.tx_amt1 = dTmpAmt;
                        }
                        else
                        {
                            g_pub_tx.tx_amt1 = sDdMst.bal-sDdMst.hold_amt-sDdMst.ctl_amt-g_dd_parm.min_bal;                
                        }
                        vtcp_log("[%s][%d] g_pub_tx.tx_amt1=[%f]",__FILE__,__LINE__,g_pub_tx.tx_amt1);
                    }
                    else    /** 非冻结 **/
                    {
                        ret=pub_base_CompDblVal(sDdMst.bal-sDdMst.ctl_amt-g_dd_parm.min_bal,0.00);
                        if( ret<=0 )
                        {
                            vtcp_log("[%s][%d] AAAAA [%f][%f][%f]",__FILE__,__LINE__,sDdMst.bal,sDdMst.ctl_amt,g_dd_parm.min_bal);
                            goto AAAAA;
                        }

                        ret=pub_base_CompDblVal(dTmpAmt,sDdMst.bal-sDdMst.ctl_amt-g_dd_parm.min_bal);
                        if( ret<=0 )
                        {
                            vtcp_log("[%s][%d] dTmpAmt=[%f]",__FILE__,__LINE__,dTmpAmt);
                            g_pub_tx.tx_amt1 = dTmpAmt;
                        }
                        else
                        {
                            g_pub_tx.tx_amt1 = sDdMst.bal-sDdMst.ctl_amt-g_dd_parm.min_bal;
                        }
                        vtcp_log("[%s][%d] g_pub_tx.tx_amt1=[%f]",__FILE__,__LINE__,g_pub_tx.tx_amt1);
                    }
                    /**--------- xxx ------------ end ----------**/
                    /** 判断如果保证金帐户开户行不是申请行需要做通兑处理 **/
                    memset(g_pub_tx.tx_br_no,0x00,sizeof(g_pub_tx.tx_br_no));
                    memset(g_pub_tx.opn_br_no,0x00,sizeof(g_pub_tx.opn_br_no));
                    strcpy(g_pub_tx.tx_br_no,sMoBankAccPo.payer_br_no);
                    strcpy(g_pub_tx.opn_br_no,sMdmAcRel.opn_br_no);
                    
                    dTempAmt3 = g_pub_tx.tx_amt1;
                    strcpy( g_pub_tx.ac_no, cNewAcno );
                    strcpy( g_pub_tx.ac_id_type, "1" );
                    strcpy( g_pub_tx.beg_note_no, cNewPactno);
                    g_pub_tx.ac_id = sDdMst.ac_id;
                    vtcp_log("[%s][%d] 取款 ac_id=[%ld] tx_amt=[%f]",__FILE__,__LINE__,g_pub_tx.ac_id,g_pub_tx.tx_amt1);
                    g_pub_tx.ac_seqn = sMdmAcRel.ac_seqn;
                    strcpy( g_pub_tx.cur_no, "01" );
                    strcpy( g_pub_tx.ct_ind, "2" );    
                    strcpy( g_pub_tx.prt_ind, "0" );     /*是否打折标志*/
                    strcpy( g_pub_tx.brf, "承兑扣款" );

                    strcpy( g_pub_tx.add_ind, "0" );     /**减少**/
                    strcpy( g_pub_tx.hst_ind, "1" );     /**日间入明细**/

                    /**检查凭证、零不记流水、零不记明细、检查支取方式**/
                    strcpy(g_pub_tx.ac_wrk_ind,"000001001"); 
                    g_pub_tx.svc_ind=1001;             /**存取款***/
                    /** 记交易流水 **/
                    strcpy( g_pub_tx.sub_tx_code, "D003" );
                    ret = pub_acct_trance();
                    if( ret )
                    {
                        sprintf(acErrMsg,"调用存取款主控失败!");
                        WRITEMSG
                        goto ErrExit;
                    }
                    /* 取款方－会计记帐 */
                    set_zd_data( "102J", "01" );    
                    set_zd_double( "102F", g_pub_tx.tx_amt1 );
                    strcpy( g_pub_tx.sub_tx_code, "D003" );
                    strcpy( g_pub_tx.prdt_code, sDdMst.prdt_no );
                    ret = pubf_acct_proc( g_pub_tx.sub_tx_code );
                    if (ret != 0)
                    {
                        sprintf(acErrMsg,"会计记帐不成功!!");
                        WRITEMSG
                        goto ErrExit;
                    }
                }
            }
        }

        /** 银行垫款由申请行付出 **/
        memset(g_pub_tx.tx_br_no,0x00,sizeof(g_pub_tx.tx_br_no));
        memset(g_pub_tx.opn_br_no,0x00,sizeof(g_pub_tx.opn_br_no));
        strcpy(g_pub_tx.tx_br_no,sMoBankAccPo.payer_br_no);
        strcpy(g_pub_tx.opn_br_no,sMoBankAccPo.payer_br_no);

        /* 如果金额还是不足则需要银行垫款，分四部分 */
AAAAA:    dTempAmt2 = dTempAmt1 + dTempAmt3;
        sprintf(acErrMsg,"dTempAmt2=[%lf],dTempAmt1=[%lf],dTempAmt3=[%lf]",dTempAmt2, dTempAmt1, dTempAmt3);
        WRITEMSG
        /******
        ret = pub_base_CompDblVal( dTempAmt2, sMoBankAccPo.sign_amt );
        *******/
        
        ret = pub_base_CompDblVal( dTempAmt2, sum_po_amt );
        if( ret < 0 )
        {
            /* 第一部分：借：13602银行垫款 */
            /*需要开一个13602垫款的内部户*/
            ret = pub_base_GetParm_Str( "CDDK", 1, g_pub_tx.prdt_code );
            if( ret )
            {
                sprintf( acErrMsg, "取承兑垫款产品编号错!" );
                WRITEMSG
                strcpy( g_pub_tx.reply, "B225" );
                goto ErrExit;
            }
						g_pub_tx.mach_date=21000101; /**到期日期**/
						strcpy(g_pub_tx.name,sMoBankAccPo.payer_name); /**户名**/
            strcpy(g_pub_tx.prt_ind,"0");
						strcpy(g_pub_tx.brf,"内部帐开户");
						strcpy(g_pub_tx.ac_wrk_ind,"000");
						strcpy(g_pub_tx.add_ind,"1");
						strcpy(g_pub_tx.ct_ind,"2");
						strcpy(g_pub_tx.prt_ind,"0");
						
  					g_pub_tx.tx_amt1=0.00;
						/***开介质***/
						ret=pub_acct_opn_in_mdm();
						if( ret )
						{
							sprintf(acErrMsg,"开介质错误![%d]",ret);
							WRITEMSG
							goto ErrExit;
						}
  					
						strcpy(g_pub_tx.ac_wrk_ind,"011");
						/***开户***/
						ret=pub_acct_opn_ac();
						if( ret )
						{
							sprintf(acErrMsg,"开账号错误![%d]",ret);
							WRITEMSG
							goto ErrExit;
						}
						vtcp_log("开的账号为ac_no=【%s】，%s,%ld ",g_pub_tx.ac_no,__FILE__,__LINE__);
						/*lpj20151126*/
            g_pub_tx.ac_id=0;
            g_pub_tx.ac_seqn=0;
            /******
            g_pub_tx.tx_amt1 = sMoBankAccPo.sign_amt - dTempAmt2;
            ******/
            g_pub_tx.tx_amt1 = sum_po_amt - dTempAmt2;
            /*strcpy( g_pub_tx.ac_no, pubf_acct_acctoact("13602",g_pub_tx.tx_br_no) );  lpj20151126重新生成的内部户*/  /* 承兑垫款 */
            strcpy( g_pub_tx.cur_no, "01" );
            strcpy( g_pub_tx.ct_ind, "2" );
            strcpy( g_pub_tx.brf, "借承兑垫款" );

            strcpy( g_pub_tx.ac_id_type,"9" ); /*账户类型为内部*/
            strcpy( g_pub_tx.ac_get_ind,"00" ); /*本程序未读取分户*/
            /*strcpy( g_pub_tx.ac_wrk_ind,"000" ); *不核对凭证号，零金额不计流水、明细*/
            strcpy(g_pub_tx.ac_wrk_ind,"000001001"); 
            strcpy( g_pub_tx.end_note_no, g_pub_tx.beg_note_no);
            strcpy(g_pub_tx.add_ind,"0");    /*借方标志*/
            strcpy( g_pub_tx.prt_ind,"0" ); /*不登折*/
            g_pub_tx.svc_ind=9001;      /*内部帐存取*/
            strcpy(g_pub_tx.prt_ind,"0");
            strcpy(g_pub_tx.hst_ind,"1"); /*日间入明细*/
            /** 记交易流水 **/
            strcpy( g_pub_tx.sub_tx_code, "A016" );
            ret = pub_acct_trance();
            if( ret )
            {
                sprintf(acErrMsg,"调用存取款主控失败!");
                WRITEMSG
                goto ErrExit;
            }
            /* 取款方－会计记帐 */
            set_zd_data( "1204", "01" );
            set_zd_double( "1208", g_pub_tx.tx_amt1 );
            strcpy( g_pub_tx.sub_tx_code, "A016" );

            /* 取承兑垫款产品编号:到参数表com_parm中取得(用pubf_base_GetParm.c中的函数) */
            ret = pub_base_GetParm_Str( "CDDK", 1, g_pub_tx.prdt_code );
            if( ret )
            {
                sprintf( acErrMsg, "取承兑垫款产品编号错!" );
                WRITEMSG
                strcpy( g_pub_tx.reply, "B225" );
                goto ErrExit;
            }
            ret = pubf_acct_proc( g_pub_tx.sub_tx_code );
            if (ret != 0)
            {
                sprintf(acErrMsg,"会计记帐不成功!!");
                WRITEMSG
                goto ErrExit;
            }
            /* 修改银行汇票基本信息登记簿 */
            sMoBankAccPo.advance_amt = g_pub_tx.tx_amt1;    /* 银行垫款额 */
            ret = Mo_bank_acc_po_Upd_Upd( sMoBankAccPo, g_pub_tx.reply );
            if ( ret )
            {
                sprintf( acErrMsg, "修改承兑汇票基本信息登记薄错[%d]", ret );
                WRITEMSG
                goto ErrExit;
            }
  					strcpy(cAcNo, g_pub_tx.ac_no);            /** 登记承兑汇票基本信息登记簿用 */
            /* 第二部分：银行垫款同时开逾期贷款户 */
            /* 取承兑逾期贷款产品编号:到参数表com_parm中取得(用pubf_base_GetParm.c中的函数) */
            /*lpj20151126不再开贷款户*/
            /*ret = pub_base_GetParm_Str( "CDYQ", 1, g_pub_tx.prdt_code );
            if( ret )
            {
                sprintf( acErrMsg, "取承兑垫款转逾期贷款产品编号错!" );
                WRITEMSG
                strcpy( g_pub_tx.reply, "B265" );
                goto ErrExit;
            }
            
            ret = Ln_parm_Sel( g_pub_tx.reply , &g_ln_parm , "prdt_no='%s' " , g_pub_tx.prdt_code );
            if(ret)    
            {
                sprintf(acErrMsg,"取贷款参数信息失败!");
                WRITEMSG
                goto ErrExit;    
            }
            lpj20151126*/
            /*    为以后需要检查还款账户状态时备用
            ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdmAcRel, "ac_no='%s'",\
                        sMoBankAccPo.payer_ac_no);
            if( ret == 100 )
            {
                sprintf( acErrMsg, "付款人账号不存在![%s]", sMoBankAccPo.payer_ac_no );
                WRITEMSG
                strcpy( g_pub_tx.reply, "D164" );
                goto ErrExit;
            }else if( ret )
            {
                sprintf( acErrMsg, "执行Mdm_ac_rel_Sel出错![%d]", ret );
                WRITEMSG
                goto ErrExit;
            }
            
            ret = Dd_mst_Sel( g_pub_tx.reply, &sDdMst, "ac_id=%ld and ac_seqn=%d",\
                    sMdmAcRel.ac_id, sMdmAcRel.ac_seqn );
            if( ret == 100 )
            {
                sprintf( acErrMsg, "付款人账号不是活期账号!" );
                WRITEMSG
                strcpy( g_pub_tx.reply, "B179" );
                goto ErrExit;
            }else if( ret )
            {
                sprintf( acErrMsg, "执行Dd_mst_Sel错误![%d]", ret );
                WRITEMSG
                goto ErrExit;
            }
            
            if( sDdMst.ac_sts[0] == '*' )
            {
                sprintf( acErrMsg, "此付款人账号已销户!" );
                WRITEMSG
                strcpy( g_pub_tx.reply, "B231" );
                goto ErrExit;
            }
            */

            /* 截取出票人账号的前八位，客户号 */
            /*lpj20151126*/
           /* g_pub_tx.cif_no = sDdMst.cif_no;

            sprintf( acErrMsg, "cif_no=[%ld]", g_pub_tx.cif_no );
            WRITEMSG
            sprintf( acErrMsg, "ln_bal=[%lf]", g_ln_mst.bal );
            WRITEMSG
            g_ln_mst.opn_date = g_pub_tx.tx_date;            * 开户日期 *      
            strcpy( g_ln_mst.pact_no, sMoBankAccPo.pact_no );    * 合同号:承兑汇票协议编号 *   
            ******
            g_ln_mst.amt_lmt = sMoBankAccPo.sign_amt - dTempAmt2;    **  * 计划发放额，贷款限额:银行垫款额 *
            
            g_ln_mst.amt_lmt = sum_po_amt - dTempAmt2;
            
            g_ln_mst.rate=sMoBankAccPo.over_rate;
            g_ln_mst.over_rate = sMoBankAccPo.over_rate;        * 逾期利率:录入时输入的罚息利率 *
            g_ln_mst.fine_rate = sMoBankAccPo.over_rate;        * 罚息利率:录入时输入的罚息利率 *
            strcpy( g_pub_tx.cur_no, "01");                * 币种 *
            strcpy( g_ln_mst.repay_type, g_ln_parm.ln_pay_type );    * 贷款支付方式 *    
            g_ln_mst.mtr_date = g_pub_tx.tx_date;            * 到期日期:逾期当天日期 *
            strcpy( g_pub_tx.intst_type, g_ln_parm.intst_type );    * 计息类型 *    
            strcpy( g_pub_tx.name, sMoBankAccPo.payer_name );    * 贷款户名:承兑汇票出票人户名 *        
            strcpy( g_ln_mst.cmpd_intst_ind,g_ln_parm.cmpd_intst_ind);
            g_ln_mst.cif_no=g_pub_tx.cif_no;
            sprintf( acErrMsg, "ln_bal=[%lf]", g_ln_mst.bal );
            WRITEMSG            
            *ac_seqn_tmp = ;                         账号序号 
            *strcpy( g_pub_tx.ac_no1, sMoBankAccPo.payer_ac_no );     还款账号:承兑出票人账号，如果此账号有变，则在贷款还款计划中维护 *
            
            * 取还款账号ID，为以后需要检查还款账户状态时备用    
            pub_base_strpack(g_pub_tx.ac_no1);
            if( strlen(g_pub_tx.ac_no1) )
            {
                ret = pub_ln_AcCheck( g_pub_tx.ac_no1,ac_seqn_tmp,
                    &cif_no_tmp,&g_ln_mst.repay_ac_id );
                if( ret )
                {
                    sprintf(acErrMsg,"调用函数pub_ln_AcCheck错误!");
                    WRITEMSG
                    return 1;
                }    
            }*
            
            ret = pub_base_getllz( g_ln_parm.rate_no, g_ln_parm.cur_no, g_pub_tx.tx_date , &basic_rate );
            if( ret )
            {
                sprintf(acErrMsg,"调用根据利率编号取得利率函数错误!");
                WRITEMSG
                return -1;    
            }
            basic_rate = basic_rate/1.2;* 转换成月利率 *
            sprintf( acErrMsg, "ln_bal=[%lf]", g_ln_mst.bal );
            WRITEMSG            
            * 利率浮动比例计算 *
            g_ln_mst.over_rate_flt = pub_base_PubDround_1( g_ln_mst.over_rate*100/basic_rate);
            g_ln_mst.fine_rate_flt = pub_base_PubDround_1( g_ln_mst.fine_rate*100/basic_rate);
            
            strcpy ( g_pub_tx.ac_wrk_ind,"0110" );
            strcpy ( g_pub_tx.ac_id_type , "3" );        * 账号类型 3-贷款 *
            strcpy ( g_pub_tx.add_ind , "1" );        * 增减1加 *    
            strcpy ( g_pub_tx.ct_ind , "2" );        * 现转:1现2转 *    
            strcpy ( g_pub_tx.prt_ind , "0" );        * 是否打印存折(有折无折标志)0否 *
            strcpy ( g_pub_tx.hst_ind , "1" );        * 入明细帐标志:1日间入2日终单笔入3日终汇总入 *
            strcpy( g_pub_tx.brf,"贷款开户开介质" );        
            
            TRACE
            sprintf( acErrMsg, "cif_no=[%ld]", g_pub_tx.cif_no );
            WRITEMSG
            g_pub_tx.ac_id =0 ;
            ***生成账号***
            if( pub_acct_opn_cif_mdm() )
            {
                sprintf(acErrMsg,"生成账号错误!");
                WRITEMSG
                goto ErrExit;
            }
            sprintf(acErrMsg,"贷款账号[%s]",g_pub_tx.ac_no);
            WRITEMSG            
            sprintf( acErrMsg, "ln_bal=[%lf]", g_ln_mst.bal );
            WRITEMSG
            
            strcpy(cAcNo, g_pub_tx.ac_no);            * 登记承兑汇票基本信息登记簿用 *
            TRACE
            * Pub_tx结构赋值 *
            strcpy ( g_pub_tx.ac_wrk_ind,"0110" );
            strcpy ( g_pub_tx.ac_id_type , "3" );        * 账号类型 3-贷款 *
            strcpy ( g_pub_tx.add_ind , "1" );        * 增减1加 *    
            strcpy ( g_pub_tx.ct_ind , "2" );        * 现转:1现2转 *    
            strcpy ( g_pub_tx.prt_ind , "0" );        * 是否打印存折(有折无折标志)0否 *
            strcpy ( g_pub_tx.hst_ind , "1" );        * 入明细帐标志:1日间入2日终单笔入3日终汇总入 *
            g_pub_tx.svc_ind = 3001;
            strcpy( g_pub_tx.brf,"贷款开户" );
            TRACE
            sprintf( acErrMsg, "cif_no=[%ld]", g_pub_tx.cif_no );
            WRITEMSG
            sprintf( acErrMsg, "ac_id=[%ld]", g_pub_tx.ac_id );
            WRITEMSG
            g_pub_tx.tx_amt1 = 0.00;
            ***生成账户ID***
            if( pub_acct_opn_ac() )
            {
                sprintf(acErrMsg,"生成账号ID错误!");
                WRITEMSG
                goto ErrExit;
            }
            sprintf( acErrMsg, "ln_bal=[%lf]", g_ln_mst.bal );
            WRITEMSG
            * 第三部分：逾期贷款放款，借：13001逾期贷款 *
            sprintf(acErrMsg,"贷款帐号[%s]",g_pub_tx.ac_no);
            WRITEMSG
            
            g_pub_tx.ac_seqn = g_ln_mst.ac_seqn;    * 贷款账户序号 *
            sprintf(acErrMsg,"贷款账户序号[%d]",g_pub_tx.ac_seqn);
            WRITEMSG    
            
            g_pub_tx.tx_amt1 = g_ln_mst.amt_lmt;    * 发生额 *
            strcpy( g_pub_tx.ct_ind, "2");                        * 现转标志 *
            sprintf(acErrMsg,"现转标志![%s]",g_pub_tx.ct_ind);
            WRITEMSG
            sprintf( acErrMsg, "ln_bal=[%lf]", g_ln_mst.bal );
            WRITEMSG            
            ret = pub_ln_lndis_file(g_pub_tx.ac_no,g_pub_tx.ac_seqn,
                &g_mdm_ac_rel,&g_ln_mst,&g_ln_parm ); *读取贷款户信息*
            if( ret ) {return ret;}
            sprintf( acErrMsg, "ln_bal=[%lf]", g_ln_mst.bal );
            WRITEMSG            
            strcpy ( g_pub_tx.add_ind , "1" );        * 增减:0减1加2销户 *   
            
            * 是否打印存折(有折无折标志):1是0否 *
            strcpy ( g_pub_tx.prt_ind , "0" );    
            * 入明细帐标志:1日间入2日终单笔入3日终汇总入 *
            strcpy ( g_pub_tx.hst_ind , "1" );
            g_pub_tx.svc_ind = 3010 ;
            strcpy( g_pub_tx.brf,"承兑逾期贷款放款" );
            
            sprintf(acErrMsg,"Pub_tx结构赋值 PASS!");
            WRITEMSG
            sprintf( acErrMsg, "ln_bal=[%lf]", g_ln_mst.bal );
            WRITEMSG            
            *** 交易记帐处理 ***
            strcpy( g_pub_tx.sub_tx_code, "L104" );
            if( pub_acct_trance() )
            {
                sprintf(acErrMsg,"交易记帐处理错误!");
                WRITEMSG
                goto ErrExit;
            }
            sprintf(acErrMsg,"交易记帐处理 PASS!");
            WRITEMSG
            
            * 登记贷款登记薄 *
            ret = pub_ln_ln_reg( g_ln_mst,g_ln_mst.opn_date,g_ln_mst.mtr_date,"1",g_ln_mst.bal,0.00,"0",0 );    
            if( ret )
            {
                sprintf(acErrMsg,"登记贷款登记薄失败");
                WRITEMSG
                strcpy(g_pub_tx.reply,"L082");
                goto ErrExit;
            }                        
            sprintf(acErrMsg,"登记贷款登记薄 PASS!");
            WRITEMSG
            
            set_zd_data("0210",g_ln_parm.cur_no);* 对币种赋值，记会计账时在dc_entry中用到 *
            
            * 借130－会计记帐 *
            set_zd_data( "0210", "01" );    
            set_zd_double( "0430", g_pub_tx.tx_amt1 );
            strcpy( g_pub_tx.sub_tx_code, "L104" );
            ret = pubf_acct_proc( g_pub_tx.sub_tx_code );
            if (ret != 0)
            {
                sprintf(acErrMsg,"会计记帐不成功!!");
                WRITEMSG
                goto ErrExit;
            }

            * 第四部分：贷：13602承兑垫款 *
            g_pub_tx.ac_id=0;    
            g_pub_tx.ac_seqn=0;
            strcpy( g_pub_tx.ac_no, pubf_acct_acctoact("13602",g_pub_tx.tx_br_no) );    * 承兑垫款 *
                strcpy( g_pub_tx.cur_no, "01" );
            strcpy( g_pub_tx.ct_ind, "2" );
            strcpy( g_pub_tx.brf, "贷承兑垫款" );
        
            strcpy( g_pub_tx.ac_id_type,"9" ); *账户类型为内部*
            strcpy( g_pub_tx.ac_get_ind,"00" ); *本程序未读取分户*
            *strcpy( g_pub_tx.ac_wrk_ind,"000" ); *不核对凭证号，零金额不计流水、明细*
            strcpy(g_pub_tx.ac_wrk_ind,"000001001"); 
            strcpy( g_pub_tx.beg_note_no, cNewPactno );
            strcpy( g_pub_tx.end_note_no,g_pub_tx.beg_note_no);
            strcpy(g_pub_tx.add_ind,"1");*贷方标志*
            strcpy( g_pub_tx.prt_ind,"0" ); *不登折*
            g_pub_tx.svc_ind=9001;  *内部帐存取*
            strcpy(g_pub_tx.prt_ind,"0");
            strcpy(g_pub_tx.hst_ind,"1"); *日间入明细*
            ** 记交易流水 **
            strcpy( g_pub_tx.sub_tx_code, "A017" );
            ret = pub_acct_trance();
            if( ret )
            {
                sprintf(acErrMsg,"调用存取款主控失败!");
                WRITEMSG
                goto ErrExit;
            }
            * 贷方－会计记帐 *
            set_zd_data( "1214", "01" );
            set_zd_double( "1218", g_pub_tx.tx_amt1 );
            strcpy( g_pub_tx.sub_tx_code, "A017" );
        
            * 取承兑垫款产品编号:到参数表com_parm中取得(用pubf_base_GetParm.c中的函数) *
            ret = pub_base_GetParm_Str( "CDDK", 1, g_pub_tx.prdt_code );
            if( ret )
            {
                sprintf( acErrMsg, "取承兑垫款产品编号错!" );
                WRITEMSG
                strcpy( g_pub_tx.reply, "B225" );
                goto ErrExit;
            }
            ret = pubf_acct_proc( g_pub_tx.sub_tx_code );
            if (ret != 0)
            {
                sprintf(acErrMsg,"会计记帐不成功!!");
                WRITEMSG
                goto ErrExit;
            }
            lpj20151126*/
        }
				
        /* 所扣所有款项均划入244科目 */
        ret = Com_parm_Sel(g_pub_tx.reply,&com_parm,"parm_code='%s' and parm_seqn=10 ",g_pub_tx.opn_br_no);
        if(ret)
        {
            sprintf(acErrMsg,"临时存款户不存在!!");
            WRITEMSG
            goto ErrExit;
        }
        /*****
        g_pub_tx.tx_amt1 = sMoBankAccPo.sign_amt;
        ******/
        g_pub_tx.tx_amt1 = sum_po_amt;
        
        g_pub_tx.ac_id=0;
        strcpy( g_pub_tx.ac_no, com_parm.val);    /* 银行承兑汇票 */
        strcpy( g_pub_tx.cur_no, "01" );
        strcpy( g_pub_tx.ct_ind, "2" );
        strcpy( g_pub_tx.brf, "银行承兑汇票" );
    
        strcpy( g_pub_tx.ac_id_type,"1" ); /*账户类型为内部*/
        strcpy( g_pub_tx.ac_get_ind,"00" ); /*本程序未读取分户*/
        /*strcpy( g_pub_tx.ac_wrk_ind,"000" ); *不核对凭证号，零金额不计流水、明细*/
        strcpy(g_pub_tx.ac_wrk_ind,"000001001"); 
        strcpy( g_pub_tx.end_note_no,g_pub_tx.beg_note_no);
        strcpy(g_pub_tx.add_ind,"1");/*贷方标志*/
        strcpy( g_pub_tx.prt_ind,"0" ); /*不登折*/
        g_pub_tx.svc_ind=9001;  /*内部帐存取*/
        strcpy(g_pub_tx.prt_ind,"0");
        strcpy(g_pub_tx.hst_ind,"1"); /*日间入明细*/
        /** 记交易流水 **/
        strcpy( g_pub_tx.sub_tx_code, "A017" );
        ret = pub_acct_trance();
        if( ret )
        {
            sprintf(acErrMsg,"调用存取款主控失败!");
            WRITEMSG
            goto ErrExit;
        }
        /* 贷方－会计记帐 */
        set_zd_data( "1214", "01" );
        set_zd_double( "1218", g_pub_tx.tx_amt1 );
        strcpy( g_pub_tx.sub_tx_code, "A017" );
    
        /* 取银行承兑汇票产品编号:到参数表com_parm中取得(用pubf_base_GetParm.c中的函数) */
        ret = pub_base_GetParm_Str( "CDHP", 1, g_pub_tx.prdt_code );
        if( ret )
        {
            sprintf( acErrMsg, "取银行承兑汇票产品编号错!" );
            WRITEMSG
            strcpy( g_pub_tx.reply, "B226" );
            goto ErrExit;
        }
        ret = pubf_acct_proc( g_pub_tx.sub_tx_code );
        if (ret != 0)
        {
            sprintf(acErrMsg,"会计记帐不成功!!");
            WRITEMSG
            goto ErrExit;
        }
        sprintf( acErrMsg, "pact_no=[%s]", sMoBankAccPo.pact_no );
        WRITEMSG
        pub_base_strpack( sMoBankAccPo.pact_no );
        sprintf( acErrMsg, "pact_no=[%s]", sMoBankAccPo.pact_no );
        WRITEMSG

        /* 修改承兑汇票登记薄，状态置为到期 */
        ret = Mo_bank_po_Dec_Upd( g_pub_tx.reply, "pact_no='%s'", sMoBankAccPo.pact_no );
        if ( ret )
        {
            sprintf( acErrMsg, "执行Mo_bank_po_Dec_Upd错[%d]", ret );
            WRITEMSG
            goto ErrExit;
        }

        while(1)
        {
            ret = Mo_bank_po_Fet_Upd( &sMoBankPo, g_pub_tx.reply );
            if( ret == 100 && i == 0 )
            {
                sprintf( acErrMsg, "无此承兑协议编号![%s]",sMoBankAccPo.pact_no );
                WRITEMSG
                sprintf( g_pub_tx.reply, "B187" );
                goto ErrExit;
                   }else if( ret == 100 && i > 0 )
                   {
                       break;
                   }else if( ret )
                   {
                       sprintf( acErrMsg, "执行Mo_bank_po_Fet_Upd错![%d]", ret );
                        WRITEMSG
                        goto ErrExit;
                   }

            if( sMoBankPo.at_term_ind[0] == 'Y' )
            {
                break;
            }

            /* 修改承兑汇票登记簿，状态为到期*/
            strcpy( sMoBankPo.at_term_ind, "Y" );    /* 票据到期标志:Y 到期；N 未到期 */

            ret = Mo_bank_po_Upd_Upd( sMoBankPo, g_pub_tx.reply );
            if ( ret )
            {
                sprintf( acErrMsg, "修改承兑汇票登记薄错[%d]", ret );
                WRITEMSG
                goto ErrExit;
            }
            i++;
        }
        Mo_bank_po_Clo_Upd();

        /* 修改银行汇票基本信息登记簿 */
        strcpy( sMoBankAccPo.acc_po_ind, "1" );    /* 承兑协议到期处理标志: 1 已到期处理 */

        /* 存在银行垫款，登记逾期贷款账号 */
        /******
        ret = pub_base_CompDblVal( dTempAmt2, sMoBankAccPo.sign_amt );
        *******/
        ret = pub_base_CompDblVal( dTempAmt2, sum_po_amt );
        if( ret < 0 )
        {
            strcpy( sMoBankAccPo.over_ac_no, cAcNo );
        }

        /**sMoBankAccPo.bail_amt=bzjje;***/
        sMoBankAccPo.bail_amt = sMoBankAccPo.bail_amt-bzjje;/** add by chenchao 20111008 **/

        ret = Mo_bank_acc_po_Upd_Upd( sMoBankAccPo, g_pub_tx.reply );
        if ( ret )
        {
            sprintf( acErrMsg, "修改承兑汇票基本信息登记薄错[%d]", ret );
            WRITEMSG
            goto ErrExit;
        }
        flag++;
    }
    Mo_bank_acc_po_Clo_Upd();

OkExit:
    sql_commit();    /*--提交事务--*/
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"!!!!![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120",g_pub_tx.reply);
    return 0;
ErrExit:
    sql_rollback();    /*--事务回滚--*/
    sprintf(acErrMsg,"!!!!![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120",g_pub_tx.reply);
    return 1;
}

int igD090_DisFrz(char ac_type)
{
        int  g_reply_int = 0;
        struct mo_hold_c f_mo_hold;

        /*更新冻结登记薄*/
        g_reply_int = Mo_hold_Dec_Upd(g_pub_tx.reply, \
                            "ac_id=%ld and ac_seqn=%d and hold_sts='0' ", \
                            sMdmAcRel.ac_id, sMdmAcRel.ac_seqn);
        if( g_reply_int )
        {
            sprintf(acErrMsg,"查询冻结登记薄异常!");
            WRITEMSG
            goto ErrExit;
        }

        while(1)
        {

        memset(&f_mo_hold,0,sizeof(f_mo_hold));
        g_reply_int = Mo_hold_Fet_Upd(&f_mo_hold,g_pub_tx.reply);

        /** 旧数的冻结有记录, 新开承兑的冻结只改了主表状态 **/
        if( g_reply_int==100 )
        {
            sprintf(acErrMsg,"已全部解冻!");
            WRITEMSG
            break;
        }
        else if( g_reply_int )
        {
            sprintf(acErrMsg,"冻结登记薄中不存在该笔记录!");
            WRITEMSG
            goto ErrExit;
        }

        /*** 冻结已解除 ***/
        if( f_mo_hold.hold_sts[0]=='1' )
        {
            sprintf(acErrMsg,"该账户已解除冻结!");
            WRITEMSG
            strcpy (g_pub_tx.reply,"D225");
            goto ErrExit;
        }

        /*** 完全冻结、未解除、非完全解冻 ***/
        f_mo_hold.hold_sts[0]='1';                        /*改为解除状态*/
        strcpy(f_mo_hold.unhold_br_no,g_pub_tx.opn_br_no);
        strcpy(f_mo_hold.unhold_tel,"9999");
        strcpy(f_mo_hold.unhold_brf,"承兑汇票到期自动划款");

        /*更新冻结登记薄*/
        g_reply_int = Mo_hold_Upd_Upd(f_mo_hold,g_pub_tx.reply);
        if( g_reply_int )
        {
            sprintf(acErrMsg,"更新冻结登记薄失败!");
            WRITEMSG
            goto ErrExit;
        }

        }
        Mo_hold_Clo_Upd( );

        /*** 修改主文件 ***/
         switch( ac_type )
          {
            case '1':
                if( sDdMst.hold_sts[0]=='0' )
                {
                    sprintf(acErrMsg,"该账户已处于正常状态!");
                    WRITEMSG
                    goto OkExit;
                }

                sDdMst.hold_sts[0]='0'; 
                sDdMst.hold_amt = 0.00;

                 break;
            case '2':
                if( sTdMst.hold_sts[0]=='0' )
                {
                    sprintf(acErrMsg,"该账户已处于正常状态!!");
                    WRITEMSG
                    goto OkExit;
                }

                sTdMst.hold_sts[0]='0'; 
                sTdMst.hold_amt = 0.00;
                 break;
        default:
                 sprintf(acErrMsg,"该账户不属于解冻账户范围!!");
                 WRITEMSG
                 goto ErrExit;
                 break;
           }/*switch结束*/

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"冻结解冻操作成功![%s]",g_pub_tx.reply);
    WRITEMSG
    return 0;
ErrExit:
    sprintf(acErrMsg,"冻结解冻操作失败![%s]",g_pub_tx.reply);
    WRITEMSG
    return 1;

}
