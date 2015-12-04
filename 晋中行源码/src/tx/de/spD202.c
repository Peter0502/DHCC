/***************************************************************
* 文 件 名: spD202.c
* 功能描述：销户计息
* 作    者: jack
* 完成日期：2004年7月6日
*
* 修改记录：
*    日    期:
*    修 改 人:
*    修改内容:
*
**************************************************************/
#define ERR_DEAL if(ret){sprintf(acErrMsg,"ERROR");WRITEMSG goto ErrExit;}
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "prdt_ac_id_c.h"
#include "mdm_attr_c.h"
#include "dc_acc_rel_c.h"
#include "com_rate_c.h"
#include "com_chrg_rate_c.h"
#include "com_intst_tax_rate_c.h"
#include "mo_bank_acc_po_c.h"
#include "mo_put_intst_c.h"
#include "com_parm_c.h"
#include "card.h"
#include "mo_loss_c.h"

spD202()
{
    int ret=0;

    memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
    memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
    memset(&g_dd_parm,0x00,sizeof(struct dd_parm_c));
    memset(&g_td_mst,0x00,sizeof(struct td_mst_c));
    memset(&g_td_parm,0x00,sizeof(struct td_parm_c));
    memset(&g_ln_mst,0x00,sizeof(struct ln_mst_c));
    memset(&g_ln_parm,0x00,sizeof(struct ln_parm_c));
    memset(&g_in_mst,0x00,sizeof(struct in_mst_c));
    memset(&g_in_parm,0x00,sizeof(struct in_parm_c));
    memset(&g_pub_intst,0x00,sizeof(g_pub_intst));
    g_reply_int=0;

TRACE
    /*** 初始化全局变量 ***/
    pub_base_sysinit();
    set_zd_long("0460",g_pub_tx.tx_date);/**add by spj 20140110**/
    sub_get_data(); /*读取上传数据*/
    strcpy(g_pub_tx.cur_no, "01");

    /*卡号处理*/
    pub_base_strpack(g_pub_tx.ac_no);
    pub_base_old_acno( g_pub_tx.ac_no);
    pub_base_strpack(g_pub_tx.crd_no);
    if( strlen(g_pub_tx.crd_no) && !strlen(g_pub_tx.ac_no) )
         strcpy( g_pub_tx.ac_no,g_pub_tx.crd_no);

    sprintf(acErrMsg, "CGH******教育证明为:[%s]", g_pub_intst.edu_ind);
    WRITEMSG
    ret=sub_ac();   /* 查介质状态 */
    ERR_DEAL

    ret=sub_mst();
    ERR_DEAL


    /************************************************************
    g_pub_tx.tx_amt1 = g_pub_intst.dealval + g_pub_intst.keepval;
    if (pub_base_CompDblVal(g_pub_tx.tx_amt1, 0.00) == 0)
    {
        strcpy(g_pub_tx.ac_wrk_ind, "000000");
    }
    strcpy(g_pub_tx.brf, "利息");
    strcpy(g_pub_tx.add_ind,"1");
    strcpy(g_pub_tx.ct_ind, "2");
    g_reply_int=pub_acct_trance();
    if( g_reply_int )
    {
        sprintf(acErrMsg,"调用存取款主控失败!");
        WRITEMSG
        goto ErrExit;
    }

    sprintf(acErrMsg, "CGH ********* [%s]", g_pub_tx.reply);
    WRITEMSG

    g_pub_tx.tx_amt1 = g_pub_intst.val;
    if (pub_base_CompDblVal(g_pub_tx.tx_amt1, 0.00) == 0)
    {
        strcpy(g_pub_tx.ac_wrk_ind, "000000");
    }
    strcpy(g_pub_tx.brf, "利息税");
    strcpy(g_pub_tx.add_ind,"0");
    strcpy(g_pub_tx.ct_ind, "2");
    g_reply_int=pub_acct_trance();
    if( g_reply_int )
    {
        sprintf(acErrMsg,"调用存取款主控失败!");
        WRITEMSG
        goto ErrExit;
    }
    **************************************************/

    get_zd_double("102I", &g_pub_tx.tx_amt1);
    if( sub_wrt_file() )
        goto ErrExit;

    /* set_zd_double("102I",g_pub_tx.tx_amt1);  */
    sprintf(acErrMsg, "********账户余额:[%lf]", g_pub_tx.tx_amt1);
    WRITEMSG
    /* set_zd_double("102I", 
    g_pub_tx.tx_amt1+g_pub_intst.factval+g_pub_intst.keepval); */
    set_zd_data(DC_CODE,g_pub_tx.dc_code);  
    set_zd_double("102P",g_pub_intst.dealval+g_pub_intst.keepval);  /*利息合计*/
    set_zd_double("102Q",g_pub_intst.val);                          /*利 息 税*/
    set_zd_double("102R",g_pub_intst.keepval);                      /*保值利息*/
    set_zd_double("102S",
    g_pub_tx.tx_amt1+g_pub_intst.dealval+g_pub_intst.keepval-g_pub_intst.val);
                                                                    /*应付总额*/
    sprintf(acErrMsg, "[%s][%d]交易金额:[%f]", __FILE__,__LINE__,g_pub_tx.tx_amt1);
    WRITEMSG

    if (g_pub_tx.prdt_code[0]=='1')
    {
        sprintf(acErrMsg, "----------------> 账户状态:[%s]", g_dd_mst.ac_sts);
        WRITEMSG
    }
    else if (g_pub_tx.prdt_code[0] == '2')
    {
        sprintf(acErrMsg, "----------------> 账户状态:[%s]", g_td_mst.ac_sts);
        WRITEMSG
    }

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"销户计息成功![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120",g_pub_tx.reply);
    return 0;
ErrExit:
    /*
    strcpy(g_pub_tx.reply,"A033");
    */
    sprintf(acErrMsg,"销户计息失败![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120",g_pub_tx.reply);
    return 1;
}
sub_get_data()
{
    get_zd_data("1021",g_pub_tx.ac_no);
    get_zd_int("1022",&g_pub_tx.ac_seqn);
    vtcp_log("AKKKING---------------------AC_SEQN=[%d]",g_pub_tx.ac_seqn);
    get_zd_data("1023",g_pub_tx.note_type);
    strcpy( g_pub_tx.note_type,"" );
    get_zd_data("1024",g_pub_tx.beg_note_no);
    strcpy( g_pub_tx.beg_note_no,"" );
    get_zd_data("1024",g_pub_tx.end_note_no);
    get_zd_data("1025",g_pub_tx.draw_uncon_yn); 
    get_zd_data("1026",g_pub_tx.draw_pwd_yn);   
    get_zd_data("1027",g_pub_tx.qry_pwd);
TRACE
    get_zd_data("1028",g_pub_tx.draw_pwd);
    get_zd_data("1029",g_pub_tx.draw_id_yn);    
    get_zd_data("102G",g_pub_tx.id_type);
    get_zd_data("102A",g_pub_tx.id_no); 
    get_zd_data("102B",g_pub_tx.draw_seal_yn);  
    get_zd_data("102C",g_pub_tx.pwd_mach_yn);   
    get_zd_data("102D",g_pub_tx.draw_mach); 
    get_zd_long("102E",&g_pub_tx.mach_date);
    get_zd_double("102I",&g_pub_tx.tx_amt1);
    get_zd_data("102X",g_pub_intst.edu_ind); /**教育证明 1有**/
    sprintf(acErrMsg, "CGH******教育证明为:[%s]", g_pub_intst.edu_ind);
    WRITEMSG
        get_zd_data("102J",g_pub_tx.cur_no);
        /*get_zd_data("102K",g_pub_tx.ct_ind); */
    g_pub_tx.ct_ind[0] = '2';
    get_zd_data("102O",g_pub_tx.prt_ind);   /*是否打折标志*/
    strcpy(g_pub_tx.prt_ind, "0");
        /*get_zd_data("102Y",g_pub_tx.brf);         *摘要内容*/

    strcpy( g_pub_tx.brf,"计息" );

    strcpy(g_pub_tx.add_ind,"0");
        g_pub_tx.svc_ind = 1012;
        strcpy(g_pub_tx.hst_ind,"1"); /* 日间入明细 */

    get_zd_data("0191",g_pub_tx.using_ind);
    if( g_pub_tx.using_ind[0]=='1' )
    {
        get_zd_data("0192",g_pub_tx.ac_wrk_ind);
        get_zd_int("0193",&g_pub_tx.svc_ind);
    }
}
int sub_wrt_file()
{
    char tmpname[100],intst_type[17],ct_name[5],term[3],memo[5];
    FILE *fp;
    char ct_ind[2];
    int ret = 0;
    double Td_rate=0.00;
    double Tax_rate=0.00;
    struct dc_acc_rel_c g_dc_acc_rel;
    struct com_rate_c sCom_rate;
    
    struct com_intst_tax_rate_c sCom_intst_tax_rate; /**Add by hxc 091101 用于返回税率**/
    
  memset(&sCom_rate,0x00,sizeof(struct com_rate_c));
    memset(&sCom_intst_tax_rate,0x00,sizeof(struct com_intst_tax_rate_c));
    memset(ct_ind, 0x0, sizeof(ct_ind));
    memset(&g_dc_acc_rel, 0x0, sizeof(struct dc_acc_rel_c));
  memset(term,0x00,sizeof(term));
    strcpy(ct_ind, g_pub_tx.ct_ind);
    get_zd_data("102K",g_pub_tx.ct_ind); 
        sprintf(acErrMsg, "hehe:[%s]", g_pub_tx.ct_ind);
    WRITEMSG
    if (g_pub_tx.ct_ind[0]!='1' && g_pub_tx.ct_ind[0]!='2' 
        && g_pub_tx.ct_ind[0]!='3' && g_pub_tx.ct_ind[0]!='4' )
    {
        vtcp_log("CCCCC[%s]",g_pub_tx.ct_ind);
        TRACE
        strcpy( g_pub_tx.reply,"P156" );
        goto ErrExit;
    }

    if(g_pub_tx.ct_ind[0]=='2')
        strcpy( memo,"转销" );
    else
        strcpy( memo,"销户" );

    strcpy(g_pub_tx.ct_ind, ct_ind);
    /***得到利息科目 20060926***/
    ret=Dc_acc_rel_Sel(g_pub_tx.reply,&g_dc_acc_rel,"dc_code='%s'and data_code='0153'",g_pub_tx.dc_code);
        if( ret )
        {   
            sprintf(acErrMsg,"TRANSFER DC_ACC_REL ERROR,CODE=[%d]!",ret);
            WRITEMSG
            goto ErrExit;
        }
        vtcp_log("[%s][%d] acc_hrt=[%s]\n",__FILE__,__LINE__,g_dc_acc_rel.acc_hrt);
    
    /***Add By Azz.Part For Print***/
    pub_base_AllDwnFilName( tmpname );
    fp=fopen(tmpname,"a");

    /**Add by hxc 091101 用于取得税率(打印第10位为税率)**/
      ret=Com_intst_tax_rate_Sel(g_pub_tx.reply,&sCom_intst_tax_rate,"cur_no='01' and tax_rate_code='01' and end_date=99999999");
        if( ret)
        {
            sprintf(acErrMsg,"取税率错误!! [%d]",ret);
            WRITEMSG
            goto ErrExit;
        }
        Tax_rate=sCom_intst_tax_rate.tax_rate_val*100;
        vtcp_log("[%s][%d]取得税率tax_rate_val=[%.21f],Tax_rate=[%.21f]",__FILE__,__LINE__,sCom_intst_tax_rate.tax_rate_val,Tax_rate);
        /**End by hxc 091101 用于取得税率(打印第10位为税率)**/

    pub_base_dic_show_str(ct_name,"ct_ind",g_pub_tx.ct_ind);
    if(g_pub_tx.prdt_code[0]=='1')
    {
        g_reply_int = Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm, "prdt_no='%s'", \
                                  g_pub_tx.prdt_code);
        if( g_reply_int )
        {   
            sprintf(acErrMsg,"TRANSFER DD_PARM ERROR,CODE=[%d]!",g_reply_int);
            WRITEMSG
            goto ErrExit;
        }
        
        /***取得利率***/
      ret=Com_rate_Sel(g_pub_tx.reply,&sCom_rate,"cur_no='01' and end_date=99999999 and rate_code='%s'",g_dd_parm.rate_no);
        if( ret)
        {
            sprintf(acErrMsg,"取利率错误!! [%d]",ret);
            WRITEMSG
            goto ErrExit;
        }
				
				Td_rate=sCom_rate.rate_val;
        pub_base_strpack(g_pub_tx.ac_no);
        pub_base_strpack(g_dd_mst.name);
        pub_base_strpack(g_dd_parm.title);
        pub_base_dic_show_str(intst_type,"intst_knd",g_dd_mst.intst_type);
        
    

        /***增加返回利息科目(g_dc_acc_rel.acc_hrt) add by wanglei 20060926***/
        /***增加返回税率(Tax_rate) add by hxc 20091101***/
        if( pub_base_CompDblVal(g_pub_intst.tax_intst, \
                                g_pub_intst.dealval+g_pub_intst.keepval)>=0 )
        {
         fprintf(fp,"LXQD%s|%s|%.2lf|%s|%ld|%s|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|%s|%s|%d|%s|%d|%s|%s|%d|%ld|%d|%lf|%s|%s|\n",
         g_pub_tx.ac_no,g_dd_mst.name,g_pub_tx.tx_amt1,g_dd_parm.title,
         g_dd_mst.opn_date,g_dd_mst.opn_br_no,g_dd_mst.bal,
         g_pub_intst.dealval+g_pub_intst.keepval,
         g_pub_intst.val,Tax_rate,g_pub_intst.dealval+g_pub_intst.keepval,
         g_pub_intst.dealval+g_pub_intst.keepval-g_pub_intst.val,
       g_pub_tx.tx_amt1+g_pub_intst.dealval+g_pub_intst.keepval-g_pub_intst.val,
       g_dd_mst.intst_acm,intst_type,memo,g_pub_tx.ac_seqn,
       "",g_mdm_ac_rel.prt_line,g_pub_tx.brf,"",0,0,0,Td_rate,g_dd_parm.prdt_no,g_dc_acc_rel.acc_hrt);
       }
       else
       {
         fprintf(fp,"LXQD%s|%s|%.2lf|%s|%ld|%s|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|%s|%s|%d|%s|%d|%s|%s|%d|%ld|%d|%lf|%s|%s|\n",
         g_pub_tx.ac_no,g_dd_mst.name,g_pub_tx.tx_amt1,g_dd_parm.title,
         g_dd_mst.opn_date,g_dd_mst.opn_br_no,g_dd_mst.bal,
         g_pub_intst.dealval+g_pub_intst.keepval,
         g_pub_intst.val,Tax_rate,g_pub_intst.tax_intst,
         g_pub_intst.dealval+g_pub_intst.keepval-g_pub_intst.val,
       g_pub_tx.tx_amt1+g_pub_intst.dealval+g_pub_intst.keepval-g_pub_intst.val,
       g_dd_mst.intst_acm,intst_type,memo,g_pub_tx.ac_seqn,
       "",g_mdm_ac_rel.prt_line,g_pub_tx.brf,"",0,0,0,Td_rate,g_dd_parm.prdt_no,g_dc_acc_rel.acc_hrt);
       }
    }
    else if(g_pub_tx.prdt_code[0]=='2')
    {
        sprintf(term,"%02d",g_td_parm.term);
        g_reply_int = Td_parm_Sel(g_pub_tx.reply,&g_td_parm, "prdt_no='%s'", \
                                  g_pub_tx.prdt_code);
        if( g_reply_int )
        {   
            sprintf(acErrMsg,"TRANSFER DD_PARM ERROR,CODE=[%d]!",g_reply_int);
            WRITEMSG
            goto ErrExit;
        }

        pub_base_strpack(g_pub_tx.ac_no);
        pub_base_strpack(g_td_mst.name);
        pub_base_strpack(g_td_parm.title);
        pub_base_dic_show_str(intst_type,"intst_knd",g_td_mst.intst_type);
        /**增加定活两便的利率打印的修改 add by martin 2009/3/27 14:09:59**/
        if(g_td_parm.td_type[0]=='6')
            {
                g_reply_int=pub_base_CalRateDHLB(&Td_rate);
                if( g_reply_int )
             {  
                sprintf(acErrMsg," pub_base_CalRateDHLB ERROR,CODE=[%d]!",g_reply_int);
                WRITEMSG
                goto ErrExit;
             }
             sprintf(acErrMsg," pub_base_CalRateDHLB OK定活两便利率,Td_rate=[%lf]!",Td_rate);
                WRITEMSG
            }else
            {
                Td_rate=g_td_mst.rate;
            }

        /***增加返回利息科目(g_dc_acc_rel.acc_hrt) add by wanglei 20060926***/
        /***增加返回税率(Tax_rate) add by hxc 20091101***/
        vtcp_log("%s,%d g_pub_intst.tax_intst=[%lf],g_pub_intst.dealval+g_pub_intst.keepval=[%lf]\n",__FILE__,__LINE__,g_pub_intst.tax_intst,g_pub_intst.dealval+g_pub_intst.keepval);
        if( pub_base_CompDblVal(g_pub_intst.tax_intst,g_pub_intst.dealval+g_pub_intst.keepval)>=0 )
        {
         fprintf(fp,"LXQD%s|%s|%.2lf|%s|%ld|%s|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|%s|%s|%d|%s|%d|%s|%s|%d|%ld|%d|%lf|%s|%s|\n",
         g_pub_tx.ac_no,g_td_mst.name,g_pub_tx.tx_amt1,g_td_parm.title,
         g_td_mst.opn_date,g_td_mst.opn_br_no,g_td_mst.bal,
         g_pub_intst.dealval+g_pub_intst.keepval,g_pub_intst.val,
         Tax_rate,g_pub_intst.dealval+g_pub_intst.keepval,
         g_pub_intst.dealval+g_pub_intst.keepval-g_pub_intst.val,
         g_pub_tx.tx_amt1+g_pub_intst.dealval+g_pub_intst.keepval-g_pub_intst.val,
        g_td_mst.intst_acm,intst_type,"销户",g_pub_tx.ac_seqn,
        term,g_mdm_ac_rel.prt_line,g_pub_tx.brf,g_pub_tx.ac_no1,
        g_td_mst.book_line,g_td_mst.mtr_date,0,Td_rate,g_td_parm.prdt_no,g_dc_acc_rel.acc_hrt);
        /*g_td_mst.book_line,g_td_mst.mtr_date,0,g_td_mst.rate,g_td_parm.prdt_no,g_dc_acc_rel.acc_hrt);*/
        }
        else
        {
         fprintf(fp,"LXQD%s|%s|%.2lf|%s|%ld|%s|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|%s|%s|%d|%s|%d|%s|%s|%d|%ld|%d|%lf|%s|%s|\n",
         g_pub_tx.ac_no,g_td_mst.name,g_pub_tx.tx_amt1,g_td_parm.title,
         g_td_mst.opn_date,g_td_mst.opn_br_no,g_td_mst.bal,
         g_pub_intst.dealval+g_pub_intst.keepval,g_pub_intst.val,
         Tax_rate,
         g_pub_intst.tax_intst,
         g_pub_intst.dealval+g_pub_intst.keepval-g_pub_intst.val,
        g_pub_tx.tx_amt1+g_pub_intst.dealval+g_pub_intst.keepval-g_pub_intst.val,
        g_td_mst.intst_acm,intst_type,"销户",g_pub_tx.ac_seqn,
        term,g_mdm_ac_rel.prt_line,g_pub_tx.brf,g_pub_tx.ac_no1,
        g_td_mst.book_line,g_td_mst.mtr_date,0,Td_rate,g_td_parm.prdt_no,g_dc_acc_rel.acc_hrt);
        /*g_td_mst.book_line,g_td_mst.mtr_date,0,g_td_mst.rate,g_td_parm.prdt_no,g_dc_acc_rel.acc_hrt);*/
        }
    }
    fclose(fp);
    set_zd_data(DC_FILE_SND,"1");

    return 0;
ErrExit:
    return 1;
}
sub_mst()
{
    int ret=0 ;

    /*** 检查处理分户帐,明细账 ***/
    switch( g_pub_tx.ac_id_type[0] )
    {
        case '1':   
            ret=Intst_dd_mst();
            ERR_DEAL
            break;
        case '2':   
            ret=Intst_td_mst();
            ERR_DEAL
            break;
        case '3':   
            sprintf(acErrMsg,"贷款不在这处理![%s]",g_pub_tx.ac_type);
            WRITEMSG
            break;
        case '9':   
            ret=Intst_in_mst();
            ERR_DEAL
            break;
        default:    
            sprintf(acErrMsg,"无此账号类型![%s]",g_pub_tx.ac_type);
            WRITEMSG
            strcpy(g_pub_tx.reply,"P104");
            goto ErrExit;
    }   

    return 0;
ErrExit:
    return 1;
}
int Intst_dd_mst()
{
    int ret;
    double tmpje=0.00;
    double tmpbal=0.00;
    double    dIntst_sxc = 0.00;
    char      cSxc_flag[3];
    
    struct dd_mst_c TmpDd_mst;

    /*** 调用活期产品参数各项检查函数 ***/
    g_pub_tx.ac_seqn=1;

    /*** 活期主文件检查 ***/
    ret=Dd_mst_Sel( g_pub_tx.reply,&g_dd_mst,"ac_id='%ld' and ac_seqn='%d' ", \
                    g_pub_tx.ac_id,g_pub_tx.ac_seqn);
    if( ret )
    {
        sprintf(acErrMsg,"账号不存在[%d][%d][%d]", \
                g_pub_tx.ac_id,g_pub_tx.ac_seqn,ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"P102");
        goto ErrExit;
    }

    /**读取产品参数**/
    ret = Dd_parm_Sel( g_pub_tx.reply,&g_dd_parm," prdt_no='%s' ",g_dd_mst.prdt_no );
    ERR_DEAL

    strcpy(g_pub_tx.cur_no,g_dd_parm.cur_no);
    strcpy(g_pub_tx.dc_code,g_dd_parm.dc_code);
    strcpy(g_pub_tx.prdt_code,g_dd_mst.prdt_no);

    strcpy(g_pub_tx.name,g_dd_mst.name);
    g_pub_tx.cif_no=g_dd_mst.cif_no;
    /**不做状态判断**/
    /**不校验支取方式**/
    /**协定划款问题**/
    vtcp_log("算前lst_date[%ld],ic_date[%ld]\n",g_dd_mst.lst_date,g_dd_mst.ic_date);

        /*** 调用算利息积数的函数 ***/
    vtcp_log("算前lst_date[%ld],acm=[%.2lf]\n",g_dd_mst.lst_date,g_dd_mst.intst_acm);
        if( pub_base_CalAcm(g_dd_mst.lst_date,g_pub_tx.tx_date,
            g_dd_parm.intst_term_type,g_dd_parm.acm_calc_type,
            g_dd_mst.bal,-g_pub_tx.tx_amt1,&g_dd_mst.intst_acm,
            g_dd_mst.intst_type,g_dd_parm.intst_mon,g_dd_parm.intst_day) )
        {
            sprintf(acErrMsg,"计算利息积数错误!");
            WRITEMSG
            goto ErrExit;
        }
    vtcp_log("算后lst_date[%ld],acm=[%.2lf]\n",g_dd_mst.lst_date,g_dd_mst.intst_acm);
         /* 调用计息函数 */
         if( g_dd_mst.intst_type[0]!='0' )
         {
            g_pub_intst.mode = 1;  /* 销户计息 */
            ret = pub_base_CalIntst(&g_pub_intst);
            if (ret != 0)
            {
               sprintf(acErrMsg,"计算利息错误!!");
               WRITEMSG
               strcpy(g_pub_tx.reply,"A032");
               goto ErrExit;
            }
         }
         else
         {
            g_pub_intst.dealval=0.00;
            g_pub_intst.tax_intst=0.00;
            g_pub_intst.keepval=0.00;
            g_pub_intst.factval=0.00;
            g_pub_intst.val=0.00;
         }
        sprintf(acErrMsg,"教育证明[%s]",g_pub_intst.edu_ind);
        WRITEMSG
        sprintf(acErrMsg,"应付利息[%.2lf] 应税利息[%.2lf] 保值利息[%.2lf]",
            g_pub_intst.dealval, g_pub_intst.tax_intst, g_pub_intst.keepval );
        WRITEMSG
        sprintf(acErrMsg,"实付利息[%.2lf] 利息税[%.2lf] ",
            g_pub_intst.factval, g_pub_intst.val );
        WRITEMSG
        
        /***添加随心存利息 add by zqbo 20141125***/
		    memset(cSxc_flag, 0x00, sizeof(cSxc_flag));
		    get_zd_data("100A", cSxc_flag);
		    vtcp_log("%s, %d, bbbbbbbb看看走到里面没20150917[%s] ",__FILE__,__LINE__,cSxc_flag);
		    if( strcmp(cSxc_flag, "sx") == 0 )
		    {
		        get_zd_double("100E", &dIntst_sxc);
		        if(pub_base_CompDblVal(dIntst_sxc, 0.00) > 0)
		        {
		            g_pub_intst.dealval += dIntst_sxc;
		        }
		        vtcp_log("%s, %d, aaaaaaaaaa看看走到里面没20150917[%lf][%lf] ",__FILE__,__LINE__,g_pub_intst.dealval,dIntst_sxc);
		    }
		    vtcp_log(" %s, %d, edu_ind[%s] ",__FILE__,__LINE__,g_pub_intst.edu_ind);
		    sprintf(acErrMsg, "应付利息[%.2lf] 应税利息[%.2lf] 保值利息[%.2lf]",g_pub_intst.dealval, g_pub_intst.tax_intst, g_pub_intst.keepval);
		    WRITEMSG
		    sprintf(acErrMsg, "实付利息[%.2lf] 利息税[%.2lf] ",g_pub_intst.factval, g_pub_intst.val);
		    WRITEMSG

    /****利息入账****
    if( pub_base_CompDblVal(g_pub_intst.dealval,0.00) ) 
    {
        g_pub_tx.tx_amt1=g_pub_intst.dealval;

        if( g_pub_tx.hst_ind[0]=='1' )      *日间入明细*
        {
            g_dd_mst.hst_bal=g_dd_mst.hst_bal+g_pub_tx.tx_amt1;
        }
    }
    **
    ****利息税扣除****
    if( pub_base_CompDblVal(g_pub_intst.val,0.00) ) 
    {
        g_pub_tx.tx_amt1=g_pub_intst.val;
    }
    **/


    /*** 调用算利息积数的函数 ***/
    g_dd_mst.lst_date=g_pub_tx.tx_date;

    /* 
    if( g_pub_tx.hst_ind[0]=='1' )      *日间入明细*
    {
        ret=pub_ins_dd_hst();
        ERR_DEAL
    }
    *
    *******/
    vtcp_log("算后lst_date[%ld],acm=[%.2lf]\n",g_dd_mst.lst_date,g_dd_mst.intst_acm);

    ret=Dd_mst_Dec_Upd(g_pub_tx.reply,
        "ac_id='%ld' and ac_seqn='%d' ",g_pub_tx.ac_id,g_pub_tx.ac_seqn);
    if( ret )
    {
        sprintf(acErrMsg,"账号不存在[%d][%d]",g_pub_tx.ac_id,ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"P102");
        goto ErrExit;
    }

    ret=Dd_mst_Fet_Upd(&TmpDd_mst,g_pub_tx.reply);
    if( ret )
    {
        sprintf(acErrMsg,"账号不存在[%d][%d]",g_pub_tx.ac_id,ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"P102");
        goto ErrExit;
    }
    vtcp_log("[%s][%d]算后lst_date[%ld],TmpDd_mst.acm1=[%.2lf],afteracm=[%.2lf]\n",__FILE__,__LINE__,g_dd_mst.lst_date,TmpDd_mst.intst_acm,g_dd_mst.intst_acm);

      /*add by zgf 20140919 如果交易为2409久悬户处理  则不处理dd_mst状态*/
		vtcp_log("[%s][%d]aaaaaatx_code[%s]\n",__FILE__,__LINE__,g_pub_tx.tx_code);
    if(!strcmp(g_pub_tx.tx_code,"2409"))
    {
			;
    }
    else
    {
    	strcpy(TmpDd_mst.ac_sts, "#");
    	strcpy(g_dd_mst.ac_sts, "#");
    }
	/*add end*/

    
    /***modified by ligl 2006-12-22 23:12 因为acct_trance 记账函数对积数更新了 这里就不再加了***/
    /**原程序是将积数翻倍了 导致 销户撤销,再销户 导致利息增加**/
    /*TmpDd_mst.intst_acm = g_dd_mst.intst_acm;*/
    /***end***/
    vtcp_log("[%s][%d]算后lst_date[%ld],TmpDd_mst.acm1=[%.2lf],afteracm=[%.2lf]\n",__FILE__,__LINE__,g_dd_mst.lst_date,TmpDd_mst.intst_acm,g_dd_mst.intst_acm);
    ret=Dd_mst_Upd_Upd(TmpDd_mst,g_pub_tx.reply);
    if( ret )
    {
        sprintf(acErrMsg,"账号不存在[%d][%d]",g_pub_tx.ac_id,ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"P102");
        goto ErrExit;
    }
    Dd_mst_Clo_Upd();

OkExit:
    return 0;
ErrExit:
    return 1;
}

int Intst_td_mst()
{
    int ret;
    struct td_mst_c TmpTd_mst;
    memset(&TmpTd_mst, 0x0, sizeof(struct td_mst_c));

    ret = Td_mst_Sel(g_pub_tx.reply, &g_td_mst, "ac_id=%ld and \
        ac_seqn=%d", g_pub_tx.ac_id, g_pub_tx.ac_seqn);
    if (ret != 0)
    {
        sprintf(acErrMsg, "查询定期主文件错误!! [%d]", ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"P102");
        goto ErrExit;
    }
    
    ret = Td_parm_Sel(g_pub_tx.reply, &g_td_parm, "prdt_no='%s'",
        g_td_mst.prdt_no);
    if (ret != 0)
    {
        sprintf(acErrMsg, "查询定期产品表错误!! [%d]", ret);
        WRITEMSG
        goto ErrExit;
    }
    /**add by ligl 2006-9-15 20:55**/
    if(g_td_parm.td_type[0]=='5')/**如果为协议存款帐户,销户同时解除关联*/
    {
     ret=deldb("ac_ac_rel","ac_id='%ld'",g_pub_tx.ac_id);
    }
    strcpy(g_pub_tx.cur_no,g_td_parm.cur_no);
    strcpy(g_pub_tx.dc_code,g_td_parm.dc_code);
    strcpy(g_pub_tx.prdt_code,g_td_mst.prdt_no);

    strcpy(g_pub_tx.name,g_td_mst.name);
    g_pub_tx.cif_no=g_td_mst.cif_no;

    g_pub_tx.mach_date = g_td_mst.mtr_date;
    sprintf(acErrMsg, "CGH******教育证明为:[%s]", g_pub_intst.edu_ind);
    WRITEMSG

    /*** 调用计算利息积数的函数 **** 定额续取 定额续存 教育储蓄*/ 
    if( g_td_parm.td_type[0] == '2' || g_td_parm.td_type[0] == '1' || 
        g_td_parm.td_type[0] == '7' ) 
    { 
        if( pub_base_CalAcm(g_td_mst.lst_date, g_pub_tx.tx_date>g_td_mst.mtr_date?g_td_mst.mtr_date:g_pub_tx.tx_date, 
            g_td_parm.intst_term_type,g_td_parm.acm_calc_mode, 
            g_td_mst.bal,-g_pub_tx.tx_amt1,&g_td_mst.intst_acm, 
            g_td_mst.intst_type,g_td_parm.intst_mon,g_td_parm.intst_day) ) 
        { 
            sprintf(acErrMsg,"计算利息积数错误!"); 
            WRITEMSG 
            goto ErrExit; 
        } 
    }
    sprintf(acErrMsg, "CGH *********积数为:[%lf]", g_td_mst.intst_acm);
    WRITEMSG

    /* 调用计息函数 */ 
    /* 加判断如果计息方式是不计息 则不去计算利息 wudawei 20140126 */
    if( g_td_mst.intst_type[0]!='0' )
    {
	    g_pub_intst.mode = 1;  /* 销户计息 */ 
	    ret = pub_base_CalIntst(&g_pub_intst); 
	    if (ret != 0) 
	    { 
	        sprintf(acErrMsg,"计算利息错误!!"); 
	        WRITEMSG 
	        strcpy(g_pub_tx.reply,"A032"); 
	        goto ErrExit; 
	    }
 		}
 		else
         {
            g_pub_intst.dealval=0.00;
            g_pub_intst.tax_intst=0.00;
            g_pub_intst.keepval=0.00;
            g_pub_intst.factval=0.00;
            g_pub_intst.val=0.00;
         }
         /* 加判断如果计息方式是不计息 则不去计算利息 wudawei 20140126  end */
    sprintf(acErrMsg,"教育证明[%s]",g_pub_intst.edu_ind);
    WRITEMSG
    sprintf(acErrMsg,"应付利息[%.2lf] 应税利息[%.2lf] 保值利息[%.2lf]",
    g_pub_intst.dealval, g_pub_intst.tax_intst, g_pub_intst.keepval );
    WRITEMSG
    sprintf(acErrMsg,"实付利息[%.2lf] 利息税[%.2lf] ",
    g_pub_intst.factval, g_pub_intst.val );
    WRITEMSG
    sprintf(acErrMsg, "CGH ---------->:[%lf]", g_pub_intst.tax_intst);
    WRITEMSG

    /****利息入账**** 
    if( pub_base_CompDblVal(g_pub_intst.dealval,0.00) ) 
    { 
        g_pub_tx.tx_amt1=g_pub_intst.dealval; 
        
        if( g_pub_tx.hst_ind[0]=='1' )      *日间入明细*
        { 
            g_dd_mst.hst_bal=g_dd_mst.hst_bal-g_pub_tx.tx_amt1; 
        } 
    }
    ***/

    ret = Td_mst_Dec_Upd(g_pub_tx.reply, "ac_id=%ld and \
        ac_seqn = %d", g_pub_tx.ac_id, g_pub_tx.ac_seqn);
    if (ret != 0)
    {
        sprintf(acErrMsg, "Declare for select error !! [%d]", ret);
        WRITEMSG
        goto ErrExit;
    }

    ret = Td_mst_Fet_Upd(&TmpTd_mst, g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg, "Fetch for select error !! [%d]", ret);
        WRITEMSG
        goto ErrExit;
    }

    strcpy(TmpTd_mst.ac_sts, "#");
    /* 积数在其他里运算过了 modified by liuxuan 2007-7-30
    TmpTd_mst.intst_acm = g_td_mst.intst_acm;
    */
    ret = Td_mst_Upd_Upd(TmpTd_mst, g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg, "UPDATE error !! [%d]", ret);
        WRITEMSG
        goto ErrExit;
    }
    Td_mst_Clo_Upd();

OkExit:
    return 0;
ErrExit:
    return 1;
}

int Intst_in_mst()
{
OkExit:
    return 0;
ErrExit:
    return 1;
}

static sub_ac()
{
    struct mdm_ac_rel_c v_mdm_ac_rel;
    struct prdt_ac_id_c prdt_ac_id_c;
    struct prdt_ac_id_c s_prdt_ac_id;
    struct mdm_attr_c s_mdm_attr;
    struct mo_loss_c sMo_loss;
    char v_ac_get_ind[2];
    int ret=0;

    v_ac_get_ind[0]=g_pub_tx.ac_get_ind[0];

    vtcp_log("AKKKING2------------------------AC_SEQN==[%d]",g_pub_tx.ac_seqn);
    pub_base_strpack(g_pub_tx.ac_no);
    pub_base_strpack(g_pub_tx.crd_no);
    if( strlen(g_pub_tx.crd_no) && !strlen(g_pub_tx.ac_no) )
    {
        strcpy( g_pub_tx.ac_no,g_pub_tx.crd_no);
    }

sprintf(acErrMsg,"ac_seqn = [%d] ",g_pub_tx.ac_seqn);
WRITEMSG
    if( g_pub_tx.ac_get_ind[0]=='1' ) /**已经读取了介质账号**/
    {
        if( g_mdm_ac_rel.ac_id<=0 ) 
        {
            sprintf(acErrMsg,"标识ac_get_ind 错误[%s]",
                g_pub_tx.ac_get_ind,g_mdm_ac_rel.ac_id);
            WRITEMSG
            strcpy(g_pub_tx.reply,"P102");
            goto ErrExit;
        }
        g_pub_tx.ac_id=g_mdm_ac_rel.ac_id;
        strcpy( g_pub_tx.ac_no,g_mdm_ac_rel.ac_no );
        strcpy(g_pub_tx.mdm_code,g_mdm_ac_rel.mdm_code);
    }
    else
    {
sprintf(acErrMsg,"ac_seqn = [%d] ",g_pub_tx.ac_seqn);
WRITEMSG
        /*** 检查账号、账号ID和账号序号 ***/
        if( g_pub_tx.ac_no[0]!=NULL )
        {
sprintf(acErrMsg,"ac_seqn = [%d] ",g_pub_tx.ac_seqn);
WRITEMSG
            ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,
                "ac_no='%s' ",g_pub_tx.ac_no);
            if( ret )
            {
                sprintf(acErrMsg,"显示账号不存在[%s][%s]",g_pub_tx.ac_no,g_pub_tx.ac_wrk_ind);
                WRITEMSG
                if( ret==100 ) 
                {
                    if( g_pub_tx.ac_wrk_ind[6]=='1' )
                    {
                        strcpy(g_pub_tx.reply,"P102");
                        goto ErrExit2;
                    }   
                    strcpy(g_pub_tx.reply,"P102");
                }
                goto ErrExit;
            }
            if( g_pub_tx.ac_id && g_pub_tx.ac_id!=g_mdm_ac_rel.ac_id )
            {
                sprintf(acErrMsg,"账号[%s]和账号ID[%ld]不是一个户",
                    g_pub_tx.ac_no,g_pub_tx.ac_id);
                WRITEMSG
                strcpy(g_pub_tx.reply,"P102");
                goto ErrExit;
            }
            g_pub_tx.ac_id=g_mdm_ac_rel.ac_id;
            strcpy(g_pub_tx.mdm_code,g_mdm_ac_rel.mdm_code);
            g_pub_tx.ac_get_ind[0]='1';
        }
        else if( g_pub_tx.ac_id )
        {
sprintf(acErrMsg,"ac_seqn = [%d] ",g_pub_tx.ac_seqn);
WRITEMSG
            /**没法读取mdm_ac_rel,找最贴近的一个**/
            g_pub_tx.ac_get_ind[0]='0';
            ret=Mdm_ac_rel_Dec_Sel( g_pub_tx.reply,
                " ac_id=%ld ",g_pub_tx.ac_id );
            ERR_DEAL
            while(1)
            {
                ret=Mdm_ac_rel_Fet_Sel( &v_mdm_ac_rel,g_pub_tx.reply );
                if( ret==100 ) break;
                ERR_DEAL

                if( v_mdm_ac_rel.ac_seqn==g_pub_tx.ac_seqn )
                {
                    memcpy( &g_mdm_ac_rel,&v_mdm_ac_rel,sizeof(g_mdm_ac_rel) );
                    g_pub_tx.ac_get_ind[0]='1';
                    break;
                }
                else if( v_mdm_ac_rel.ac_seqn==ALL_SEQN && 
                    (v_mdm_ac_rel.note_sts[0]=='0' || 
                    v_mdm_ac_rel.note_sts[0]=='1') 
                    && g_mdm_ac_rel.ac_id!=g_pub_tx.ac_id )
                {
                    g_pub_tx.ac_get_ind[0]='1';
                    memcpy( &g_mdm_ac_rel,&v_mdm_ac_rel,sizeof(g_mdm_ac_rel) );
                }
            }
            if( g_pub_tx.ac_get_ind[0]=='0' )
            {
                sprintf(acErrMsg,"mdm_ac_rel中无匹配纪录acid=[%d]ac_seqn=[%d]",
                    g_pub_tx.ac_id,g_mdm_ac_rel.ac_seqn );
                WRITEMSG
                strcpy(g_pub_tx.reply,"P102");
                goto ErrExit;
            }
            strcpy( g_pub_tx.ac_no,g_mdm_ac_rel.ac_no );
            strcpy(g_pub_tx.mdm_code,g_mdm_ac_rel.mdm_code);
        }
        else
        {
            sprintf(acErrMsg,"账号[%s]和账号ID[%ld]都为空",
                g_pub_tx.ac_no,g_pub_tx.ac_id);
            WRITEMSG
            strcpy(g_pub_tx.reply,"P101");
            goto ErrExit;
        }
    }
    strcpy(g_pub_tx.opn_br_no,g_mdm_ac_rel.opn_br_no);

    /*** Add commence By Azz ***/
    if( g_mdm_ac_rel.coll_sts[0]=='1' )
    {
        if( pub_base_checkcollyn( g_mdm_ac_rel.ac_no ) )
        {
            sprintf(acErrMsg,"调用捡拾检查函数出错");
            WRITEMSG
            goto ErrExit;
        }
    }
    /*** complete ***/

    /**检查已经输入的序号与找到的比较一下**/
sprintf(acErrMsg,"ac_seqn = [%d][%s] ",g_pub_tx.ac_seqn,g_pub_tx.cur_no);
WRITEMSG
    vtcp_log("AKKKING3------------------------AC_SEQN==[%d]",g_pub_tx.ac_seqn);

    if( g_pub_tx.ac_seqn>0 )
    {
        if( g_mdm_ac_rel.ac_seqn!=ALL_SEQN && 
            g_mdm_ac_rel.ac_seqn!=g_pub_tx.ac_seqn )
        {
            sprintf(acErrMsg,"mdm_ac_rel序号不同acid=[%d]seqn=[%d]in[%d]",
                g_pub_tx.ac_id,g_mdm_ac_rel.ac_seqn,g_pub_tx.ac_seqn );
            WRITEMSG
            strcpy(g_pub_tx.reply,"P102");
            goto ErrExit;
        }
    }
    else if( g_pub_tx.ac_seqn==0 )
    {
        ret = Mdm_attr_Sel( g_pub_tx.reply, &s_mdm_attr, "mdm_code='%s'" , \
                            g_mdm_ac_rel.mdm_code );
        if( ret==100 )
        {
            sprintf(acErrMsg,"介质代码不存在=[%s]",g_mdm_ac_rel.mdm_code);
            WRITEMSG
            strcpy(g_pub_tx.reply,"P102");
            goto ErrExit;
        }
        else if( ret )
        {
            sprintf(acErrMsg,"查找出错=[%s][%d]",g_mdm_ac_rel.mdm_code,ret);
            WRITEMSG
            strcpy(g_pub_tx.reply,"D103");
            goto ErrExit;
        }

        vtcp_log("AKKKING4---------------------s_mdm_attr.ac_num_ind==[%s]", \
                 s_mdm_attr.ac_num_ind );
        if( s_mdm_attr.ac_num_ind[0]=='N' )
        {
            ret = Prdt_ac_id_Sel( g_pub_tx.reply,&s_prdt_ac_id,"ac_id=%ld", \
                                  g_pub_tx.ac_id );
            if( ret==100 )
            {
                sprintf(acErrMsg,"账号不存在=[%ld]",g_pub_tx.ac_id );
                WRITEMSG
                strcpy(g_pub_tx.reply,"P102");
                goto ErrExit;
            }
            else if( ret )
            {
                sprintf(acErrMsg,"查找错=[%d]",ret);
                WRITEMSG
                strcpy(g_pub_tx.reply,"D103");
                goto ErrExit;
            }

            if( s_prdt_ac_id.ac_id_type[0]=='2' )
               g_pub_tx.ac_seqn = 200;
            else
               g_pub_tx.ac_seqn = s_prdt_ac_id.ac_seqn;
        }
    }

    /**介质状态检查**/
    if( g_pub_tx.ac_get_ind[0]=='1' )
    {
        strcpy(g_pub_tx.mdm_code,g_mdm_ac_rel.mdm_code);
        /*增加扣划特殊处理  挂失可销户2011/11/13 13:45:36
        if( g_mdm_ac_rel.note_sts[0]=='1' && strcmp(g_pub_tx.tx_code,"Z016") )
        */
        /*begin IC卡书挂时可以直接销户20120618 add by wzw*/
        /**按财务要求 折子也能挂失销户
         if(memcmp(g_pub_tx.ac_no,ICCARDHEADCODE,6) && memcmp(g_pub_tx.ac_no,CARDHEADCODE,6))
        {
        	if( g_mdm_ac_rel.note_sts[0]=='1' && strcmp(g_pub_tx.tx_code,"Z016") && memcmp(g_pub_tx.tx_code,"4713",4))
        	{
        	    sprintf(acErrMsg,"该介质处于挂失状态,请作解挂处理!");
        	    WRITEMSG
        	    strcpy(g_pub_tx.reply,"M009");
        	    goto ErrExit;
        	}
        }
        else
        {
        **/
        	if(g_mdm_ac_rel.note_sts[0]=='1')
        	{
        		memset(&sMo_loss,0x00,sizeof(struct mo_loss_c));
        		ret=Mo_loss_Sel(g_pub_tx.reply, &sMo_loss, "ac_no=%s and unloss_ind='0'", g_pub_tx.ac_no);
        		if(ret)
        		{
        			sprintf(acErrMsg,"查询错误!");
        			WRITEMSG
        			strcpy(g_pub_tx.reply,"D104");
        			goto ErrExit;
        		}
        		if(sMo_loss.loss_ind[0]!='2')
        		{
        			sprintf(acErrMsg,"请作解挂处理!");
        			WRITEMSG
        		  strcpy(g_pub_tx.reply,"M009");
        		  goto ErrExit;
        		}else
        		{/****修改挂失登记簿，将解挂状态置为'7'  20130702****/
        			sMo_loss.unloss_ind[0]='7';/****挂失销户状态****/
        			sMo_loss.unloss_date=g_pub_tx.tx_date;/****挂失销户日期****/
        			memcpy(sMo_loss.unloss_tel,g_pub_tx.tel,sizeof(sMo_loss.unloss_tel));/****挂失销户柜员****/
        			ret=Mo_loss_Upd_Upd(sMo_loss,g_pub_tx.reply);
        			if(ret)
        			{
        				sprintf(acErrMsg,"修改挂失登记簿出错!");
        				WRITEMSG
        				strcpy(g_pub_tx.reply,"D105");
        				goto ErrExit;
        			}
        			vtcp_log("[%s][%d]sMo_loss.unloss_ind=[%c]",__FILE__,__LINE__,sMo_loss.unloss_ind[0]);
        			vtcp_log("[%s][%d]sMo_loss.unloss_date=[%ld]",__FILE__,__LINE__,sMo_loss.unloss_date);
        			vtcp_log("[%s][%d]sMo_loss.unloss_tel=[%s]",__FILE__,__LINE__,sMo_loss.unloss_tel);
        		}
        	}
        /** 
        }
        **/
        /*end*/
        if( g_mdm_ac_rel.note_sts[0]=='2' )
        {
            sprintf(acErrMsg,"该介质已挂失换证!");
            WRITEMSG
            strcpy(g_pub_tx.reply,"P171");
            goto ErrExit;
        }
        else if( g_mdm_ac_rel.note_sts[0]=='3' )
        {
            sprintf(acErrMsg,"该介质已换证!");
            WRITEMSG
            strcpy(g_pub_tx.reply,"M011");
            goto ErrExit;
        }
        else if( g_mdm_ac_rel.note_sts[0]=='4' )
        {
            sprintf(acErrMsg,"该账户已作部提!");
            WRITEMSG
            strcpy(g_pub_tx.reply,"M017");
            goto ErrExit;
        }
        else if( g_mdm_ac_rel.note_sts[0]=='5' )
        {
            sprintf(acErrMsg,"该账户已作撤销!");
            WRITEMSG
            strcpy(g_pub_tx.reply,"M019");
            goto ErrExit;
        }
        else if( g_mdm_ac_rel.note_sts[0]=='*' )
        {
            sprintf(acErrMsg,"该介质已作销户处理!");
            WRITEMSG
            strcpy(g_pub_tx.reply,"M012");
            goto ErrExit;
        }
        else if( g_mdm_ac_rel.note_sts[0]!='0' && g_mdm_ac_rel.note_sts[0]!='1')
        {
            sprintf(acErrMsg,"该介质状态不正常!");
            WRITEMSG
            strcpy(g_pub_tx.reply,"M013");
            goto ErrExit;
        }

        /*** 存款不处理 ***/
        if( g_pub_tx.add_ind[0]=='0' )
        {
            if( g_mdm_ac_rel.coll_sts[0]=='2' )
            {
                sprintf(acErrMsg,"该介质处于吞卡状态,请作卡归还!");
                WRITEMSG
                strcpy(g_pub_tx.reply,"M015");
                goto ErrExit;
            }
            else if( g_mdm_ac_rel.coll_sts[0]!='0' )
            {
                sprintf(acErrMsg,"该介质捡拾状态不正常!");
                WRITEMSG
                strcpy(g_pub_tx.reply,"M016");
                goto ErrExit;
            }
        }
    }

    /*** 取账户类型,产品编码 如果已经确定了就不必了***/
    if( g_pub_tx.ac_id_type[0]=='0' || g_pub_tx.ac_id_type[0]==NULL )
    {
        if( g_pub_tx.ac_seqn>0 )
            ret=Prdt_ac_id_Sel(g_pub_tx.reply,&prdt_ac_id_c,
                "ac_id=%ld and ac_seqn=%d ",g_pub_tx.ac_id,g_pub_tx.ac_seqn);
        else
            ret=Prdt_ac_id_Sel(g_pub_tx.reply,&prdt_ac_id_c,
                "ac_id=%ld ",g_pub_tx.ac_id);
        if( ret )
        {
            sprintf(acErrMsg,"账号不存在![%ld][%d]",
                g_pub_tx.ac_id,g_pub_tx.ac_seqn);
            WRITEMSG
            strcpy(g_pub_tx.reply,"P102");
            goto ErrExit;
        }
        strcpy(g_pub_tx.prdt_code,prdt_ac_id_c.prdt_no);
        strcpy(g_pub_tx.ac_id_type,prdt_ac_id_c.ac_id_type);
    }
    else
    {
        /**已经确定类型,由类型取账户序号**/
        if( g_pub_tx.ac_seqn<=0 )
        {
            ret=Prdt_ac_id_Sel(g_pub_tx.reply,&prdt_ac_id_c,
                "ac_id=%ld and ac_id_type='%s' ",
                g_pub_tx.ac_id,g_pub_tx.ac_id_type);
            if( ret )
            {
                sprintf(acErrMsg,"账号不存在![%ld][%d][%s]",
                    g_pub_tx.ac_id,g_pub_tx.ac_seqn,g_pub_tx.ac_id_type);
                WRITEMSG
                strcpy(g_pub_tx.reply,"P102");
                goto ErrExit;
            }
            strcpy(g_pub_tx.prdt_code,prdt_ac_id_c.prdt_no);
            strcpy(g_pub_tx.ac_id_type,prdt_ac_id_c.ac_id_type);
        }
    }

    if( g_pub_tx.ac_id_type[0]=='9' ) /**内部帐，不区分序号**/
    {
        if( g_pub_tx.ac_seqn==0 )
            g_pub_tx.ac_seqn=SINGLE_SEQN;
    }
    sprintf(acErrMsg,"ac_no[%s] ac_id[%d] ac_seqn[%d] type[%s]",
        g_pub_tx.ac_no,g_pub_tx.ac_id,g_pub_tx.ac_seqn,g_pub_tx.ac_id_type);
    WRITEMSG

    g_pub_tx.ac_get_ind[0]=v_ac_get_ind[0];

    /*** 没有密码不允许通兑 ***/
    if( strcmp(g_pub_tx.tx_br_no,"99999") && g_pub_tx.ac_wrk_ind[8]!='1'
    && strcmp(g_pub_tx.tx_br_no,g_pub_tx.opn_br_no) 
    && g_mdm_ac_rel.draw_pwd_yn[0]!='Y' && g_mdm_ac_rel.pwd_mach_yn[0]!='Y'
        && (g_pub_tx.ac_id_type[0]=='1' || g_pub_tx.ac_id_type[0]=='2') 
        && g_pub_tx.add_ind[0]=='0' )
    {
        sprintf(acErrMsg,"非留密户不允许通兑!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"P412");
        goto ErrExit;
    }

GoodExit:
    return 0;
ErrExit:
    return 1;
ErrExit2:
    return 100;
}
/**********************************************************************
* 函数名：    pub_base_CalRateDHLB()
* 函数功能：  定活两便计算利率
* 作者/时间： 2009年03月27日martin
*
* 参数：
*     输入：
*     输出: 利率          double
* 返 回 值: 0 成功
*
* 修改历史：
*
********************************************************************/
int pub_base_CalRateDHLB(double *rate)
{
    char  intst_term_type[2] ;/* 计息天数类型 */
    long  dayterm,monterm;
    char llbh[4];  /* 利率编号 */
  char cur_no[3];   /* 币种 */ 
  double rate_val,rate_val1; /* 利率值 */
    long tx_date;    /* 交易日期 */
  long ic_date;    /* 起息日期 */
  long date99,date9303,date9201,date8910;
  int ret;
  
  memset(cur_no, 0x0, sizeof(cur_no));
  strcpy(cur_no, g_td_parm.cur_no);
  ic_date = g_td_mst.ic_date;
  tx_date = g_pub_tx.tx_date;
  
  date99 = 19991101;
  date9303 = 19930301;
  date9201 = 19920101;
  date8910 = 19891001;
  (*rate) = 0.00;
    
    if (ic_date == 0 || tx_date == 0)
    {
        sprintf(acErrMsg,"起息日[%ld]或交易日[%ld]为空无法计息",
                ic_date, tx_date);
        WRITEMSG
        memset(g_pub_tx.reply, 0x0, sizeof(g_pub_tx.reply));
        strcpy(g_pub_tx.reply, "W112");
        return 1; 
    }
    
    strcpy(intst_term_type,g_td_parm.intst_term_type);/***按实际天数计息***/
    if (intst_term_type[0] != '0' && intst_term_type[0] != '1')
    {
        sprintf(acErrMsg,"产品表中的计息天数类型错误 type=[%s]",
                g_td_parm.intst_term_type );
        WRITEMSG
        memset(g_pub_tx.reply, 0x0, sizeof(g_pub_tx.reply));
        strcpy(g_pub_tx.reply, "W107");
        return 1;
    }
    ret=pub_base_CalIntstDays(ic_date,tx_date,intst_term_type,&dayterm);
    if (ret != 0)
    {
        sprintf(acErrMsg,"计算天数错误!!");
        WRITEMSG
        return 1;
    }
    
    monterm = dayterm / 30;
    memset(llbh,0x0,sizeof(llbh));
    
sprintf(acErrMsg,"开始计算定活两便利息g_td_mst.ic_date==[%ld]",g_td_mst.ic_date);
WRITEMSG
  if (g_td_mst.ic_date >= date9303)
    {
        if (monterm < 3)
        {
            strcpy(llbh,HQLLBH);
        }      
        else if (monterm < 6)
        {
            strcpy(llbh,ZCZQ03);
            
        }
        else if (monterm < 12)
        {
            strcpy(llbh,ZCZQ06);
        }
        else 
        {
            strcpy(llbh,ZCZQ12);
        }
 sprintf(acErrMsg,"llbh =  [%s]",llbh);
 WRITEMSG

        ret = pub_base_getllz(HQLLBH,cur_no,tx_date,&rate_val);
        if (ret != 0)
        {
            sprintf(acErrMsg,"从com_rate取利率值错误 [%d]",ret);
            WRITEMSG
            memset(g_pub_tx.reply, 0x0, sizeof(g_pub_tx.reply));
            strcpy(g_pub_tx.reply,"W110");
            return ret;
        }

sprintf(acErrMsg,"rate_val =  [%f]",rate_val);
WRITEMSG

        if (strcmp(llbh,HQLLBH) != 0)
        {
            ret = pub_base_getllz(llbh,cur_no,tx_date,&rate_val1);
            if (ret != 0)
            {
                sprintf(acErrMsg,"从com_rate取利率值错误 [%d]",ret);
                WRITEMSG
                memset(g_pub_tx.reply, 0x0, sizeof(g_pub_tx.reply));
                strcpy(g_pub_tx.reply,"W110");
                return ret;
            }
            rate_val1 = rate_val1 * 0.60; /*这个地方原来是注释掉的*/
            if (rate_val <= rate_val1)
            {
                rate_val = rate_val1;
            }
        }
sprintf(acErrMsg,"rate_val =  [%f]",rate_val);
WRITEMSG
    }
    else if (g_td_mst.ic_date >= date9201)
    {
    }
    else if (g_td_mst.ic_date >= date8910)
    {
    }
    else 
    {
    }
    (*rate)=rate_val;
/* add over */
    return 0;
}
