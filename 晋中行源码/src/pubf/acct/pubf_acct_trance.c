/***************************************************************
* 文 件 名:     pubf_acct_trance.c
* 功能描述：
*              1、检查账号、账号ID和账号序号的合法性；
*              2、客户支票销号处理;
*              3、检查产品参数表，检查账号主文件，修改主文件全局变量；
*              4、登记明细帐；
*              5、生成打折文件；
*              6、通兑修改407明细帐；
*              7、如果销户登记开销户登记簿；
*              8、登记交易流水；
**标志用法：
**需要核对凭证号** g_pub_tx.ac_wrk_ind[0]='1'
**                 g_pub_tx.hst_ind[0]=='1'日间入明细
**                 g_pub_tx.prt_ind[0]!='1'是否打折
*
* 作    者: jack    
* 完成日期：    2003年12月27日
*
* 修改记录：
*    日    期:20110827
*    修 改 人:gongliangliang
*    修改内容:晋中要求担保保证金也限制
*
**************************************************************/
#define ERR_DEAL if(ret){sprintf(acErrMsg,"ERROR");WRITEMSG goto ErrExit;}
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "card.h"
#include "prdt_ac_id_c.h"
#include "trace_log_c.h"
#include "dd_mst_hst_c.h"
#include "td_mst_hst_c.h"
#include "ln_mst_hst_c.h"
#include "in_mst_hst_c.h"
#include "mdm_unprt_hst_c.h"
#include "mdm_attr_c.h"
#include "note_parm_c.h"
#include "com_item_c.h"
#include "gl_mst_c.h"
#include "tx_dc_br_rel_c.h"
#include "mo_prdt_prot_c.h"
#include "mo_prot_ac_rel_c.h"
#include "card_consum_lmt_c.h"
#include "remark_ctl_c.h"
#include "auth_ac_rec_c.h"
#include "mo_hold_c.h"
#include "draw_loan_hst_c.h"
#include "draw_loan_limit_c.h"
/*#include "mo_ic_ac_rel_c.h"*/
#include "chnl_premise_c.h"                             /* add by yanqq 20111213 追加预授权处理 */
#include "intst_hst_c.h"

struct mdm_attr_c v_mdm_attr;
struct prdt_ac_id_c prdt_ac_id_c;
struct mdm_attr_c mdm_attr_c;
struct note_parm_c note_parm_c;
struct card_consum_lmt_c g_card_consum_lmt;
static  struct ln_mst_c ln_mst_acm;
static  struct ln_mst_c o_td_mst;
static int wdr_flag=0;
int partflag=0;

pub_acct_trance()
{
    int ret=0;
    int ass_ind=0;
    int factday,lastday;
    struct pub_tx v_pub_tx;

    memset(&prdt_ac_id_c,0x00,sizeof(struct prdt_ac_id_c));
    memset(&mdm_attr_c,0x00,sizeof(struct mdm_attr_c));
    memset(&note_parm_c,0x00,sizeof(struct note_parm_c));
    /**初始化***/
    partflag=0;
    wdr_flag=0;
    sprintf(acErrMsg,"[%s][%d]进入pub_acct_trance 函数svc_ind=[%d],get_ind[%s]",__FILE__,__LINE__,g_pub_tx.svc_ind,g_pub_tx.ac_get_ind);
    vtcp_log("zzzzzzzzzzzzz====g_pub_tx.ac_id_type=[%s],hst_ind[%s]ac_no=[%s]",g_pub_tx.ac_id_type,g_pub_tx.hst_ind,g_pub_tx.ac_no);
    WRITEMSG
    /**初始化主文件内容**/

    pub_base_strpack(g_pub_tx.ac_no);
    pub_base_strpack(g_pub_tx.crd_no);
    if( strlen(g_pub_tx.crd_no) && !strlen(g_pub_tx.ac_no) )
    {
         strcpy( g_pub_tx.ac_no,g_pub_tx.crd_no);
    }
    if( g_pub_tx.ct_ind[0]!='1' ) g_pub_tx.ct_ind[0]='2';

    sprintf(acErrMsg,"g_pub_tx.acno= [%s] id=[%ld] seqn=[%d] g_pub_tx.opn_br_no [%s] g_pub_tx.tx_br_no [%s]", \
            g_pub_tx.ac_no,g_pub_tx.ac_id,g_pub_tx.ac_seqn,g_pub_tx.opn_br_no,g_pub_tx.tx_br_no);
    WRITEMSG
    sprintf(acErrMsg,"发生额和贷款余额[%.2lf][%.2lf]",
                    g_pub_tx.tx_amt1,g_ln_mst.bal );
    WRITEMSG
    sprintf(acErrMsg,"[%s][%d]svc_ind=[%d]",__FILE__,__LINE__,g_pub_tx.svc_ind);
    WRITEMSG
    ret=strlen(g_pub_tx.ac_no);
    if(ret==3||ret==5||ret==7)
    {
        /**对科目记账**/
        ret=Chg_acchrt_mst(&ass_ind);
        ERR_DEAL
    }
    else
    {
        /**初始化主文件内容**/
    sprintf(acErrMsg,"[%s][%d]svc_ind=[%d],[%s]",__FILE__,__LINE__,g_pub_tx.svc_ind,g_pub_tx.ac_get_ind);
    WRITEMSG
        ret=pub_sub_ac();
        if( ret==100 ) return 0; /***TEMP他行账号***/
        ERR_DEAL

        /**检查是否有协定**/
        vtcp_log("-==========g_pub_tx.ac_id_type=[%s],ac_no=[%s]",g_pub_tx.ac_id_type,g_mdm_ac_rel.ac_no);
    sprintf(acErrMsg,"[%s][%d]svc_ind=[%d]",__FILE__,__LINE__,g_pub_tx.svc_ind);
    WRITEMSG
        ret=pub_sub_prdt_chk();
        ERR_DEAL

        /**凭证处理**/
         /**   add by shiweina   200711217  **/

  if(strcmp(g_pub_tx.tx_code,"2430")!=0)
   {
        vtcp_log("-========g_pub_tx.ac_id_type=[%s],ac_no=[%s]",g_pub_tx.ac_id_type,g_mdm_ac_rel.ac_no);
    sprintf(acErrMsg,"[%s][%d]svc_ind=[%d]",__FILE__,__LINE__,g_pub_tx.svc_ind);
    WRITEMSG
        ret=pub_sub_note();
        ERR_DEAL
     }
             /*** add到此结束 **/

        /***检查处理分户帐***/
        vtcp_log("-========g_pub_tx.ac_id_type=[%s],ac_no=[%s]",g_pub_tx.ac_id_type,g_mdm_ac_rel.ac_no);
    sprintf(acErrMsg,"[%s][%d]svc_ind=[%d]",__FILE__,__LINE__,g_pub_tx.svc_ind);
    WRITEMSG
        ret=pub_sub_mst();
        ERR_DEAL

        /***补登折处理***/
        vtcp_log("-=========g_pub_tx.ac_id_type=[%s],ac_no=[%s]",g_pub_tx.ac_id_type,g_mdm_ac_rel.ac_no);
        ret=pub_unprt_hst_ins(g_pub_tx.ac_id_type);
        ERR_DEAL

        /***补登折处理***/
        ret=pub_sub_prt_book();
        ERR_DEAL
    }

    /***通存通兑处理***/
    /**
    ret=pub_sub_liqu();
    ERR_DEAL
    **/

sprintf(acErrMsg, "8888888888888************* ic_date=[%ld]", g_td_mst.ic_date);
WRITEMSG

    vtcp_log("[%s][%d]lovexier g_pub_tx.ac_id_type[0]==[%c],ass_ind===[%d]\n",__FILE__,__LINE__,g_pub_tx.ac_id_type[0],ass_ind);
    vtcp_log("[%s][%d]lovexier g_pub_tx.ac_id_type[0]==[%c],ass_ind===[%d]\n",__FILE__,__LINE__,g_pub_tx.ac_id_type[0],ass_ind);
    /***登记交易流水***/
    if( g_pub_tx.ac_id_type[0]!='3' )
    {
        /***交易特别处理***/
        ret=pub_sub_spe_do();
        ERR_DEAL
    /**modified by ligl 2006-12-23 0:07 不管考核不考核都记流水**/
        /*if( ass_ind==0 )      *** 非考核科目 ***
        {*/
            vtcp_log("[%s][%d]哈哈从这里开始写流水了!\n",__FILE__,__LINE__);
            if( pub_ins_trace_log() )
            {
                sprintf(acErrMsg,"登记交易流水出错!");
                WRITEMSG
                goto ErrExit;
            }
        /*}*/
    }
sprintf(acErrMsg,"WDR部提%d",wdr_flag);
WRITEMSG
sprintf(acErrMsg, "8888888888888************* ic_date=[%ld]", g_td_mst.ic_date);
WRITEMSG
    if( wdr_flag )
    {
        memcpy( &v_pub_tx,&g_pub_tx,sizeof( v_pub_tx ) );

        strcpy( g_pub_tx.beg_note_no,g_pub_tx.new_note_no );
        strcpy( g_pub_tx.end_note_no,g_pub_tx.new_note_no );
        strcpy( g_pub_tx.ac_wrk_ind,"0110000" );
        strcpy( g_pub_tx.brf,"部提换证" );
        if( pub_acct_chg_mdm(10) )
        {
            sprintf(acErrMsg,"部提更换帐号错" );
            WRITEMSG
            goto ErrExit;
        }

        strcpy( v_pub_tx.ac_no1,g_pub_tx.ac_no );
        strcpy( v_pub_tx.note_type,g_pub_tx.note_type);
sprintf(acErrMsg,"ac_no1[%s]",v_pub_tx.ac_no1 );
WRITEMSG
        memcpy( &g_pub_tx,&v_pub_tx,sizeof( v_pub_tx ) );
    }

sprintf(acErrMsg, "8888888888888************* ic_date=[%ld]", g_td_mst.ic_date);
WRITEMSG
     sprintf(acErrMsg,"dc_code!!!!!!=[%s]",g_pub_tx.dc_code );
     WRITEMSG

    set_zd_data(DC_CODE,g_pub_tx.dc_code);

OKExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"业务子系统记账处理成功!");
    WRITEMSG
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"业务子系统记账处理失败![%s]",g_pub_tx.reply);
    WRITEMSG
    return 1;
}


int Chg_dd_mst()
{
    int ret;
    double tmpje=0.00;
    double tmpbal=0.00;
    struct dd_mst_c TmpDd_mst;
    char msg[51];
    struct  remark_ctl_c rem;
    struct mo_hold_c   g_mo_hold;
    int chk_ctl_flag=0;
    char  Brf[21];
    char  cCmtno[4];
    char kh_type[2];
    char  minholdtyp[2];
    long   hold_seqn;
    long   ac_seqn;
    double ysqwc_rate=0.0; /*预授权完成时，最高完成比率 20091117*/
    double zg_amt=0.0;

    memset(Brf,0,sizeof(Brf));
    memset(cCmtno,0,sizeof(cCmtno));
    memset(minholdtyp,0,sizeof(minholdtyp));
    memset(&rem,0x00,sizeof(struct remark_ctl_c));
    memset(&g_mo_hold,0x00,sizeof(struct mo_hold_c));
    /*** 调用活期产品参数各项检查函数 ***/
    sprintf(acErrMsg,"[%s][%d]svc_ind=[%d]",__FILE__,__LINE__,g_pub_tx.svc_ind);
    WRITEMSG
    strcpy(Brf,g_pub_tx.brf);/**涉及七天乐摘要问题20140325**/
    vtcp_log("[%s][%d] ssssssBrf =[%s]",__FILE__,__LINE__,Brf);
    vtcp_log("[%s][%d] g_dd_mst.ac_id =[%d]",__FILE__,__LINE__,g_dd_mst.ac_id);
    vtcp_log("[%s][%d] 帐号=[%s] 帐户ID=[%d] ",__FILE__,__LINE__,g_pub_tx.ac_no,g_pub_tx.ac_id);
    /*** 活期主文件检查 ***/
    if( g_pub_tx.ac_get_ind[1]=='1' )
    {
sprintf(acErrMsg,"dd_mst已经渎了不能锁表了[%s]",g_pub_tx.tx_code);
WRITEMSG
        ret=0;
    }
    else
    {
sprintf(acErrMsg,"dd_mst开始锁表[%s]",g_pub_tx.tx_code);
WRITEMSG
        /*** xxxx 为了锁表 **
        ret=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,
            "ac_id='%ld' and ac_seqn='%d' ",g_pub_tx.ac_id,g_pub_tx.ac_seqn);
        ****************/
        ret=Dd_mst_Dec_Upd(g_pub_tx.reply,
            "ac_id='%ld' and ac_seqn='%d' ",g_pub_tx.ac_id,g_pub_tx.ac_seqn);
        if( ret )
        {
            sprintf(acErrMsg,"账号不存在[%d-%d][%d]",g_pub_tx.ac_id,g_pub_tx.ac_seqn,ret);
            WRITEMSG
            strcpy(g_pub_tx.reply,"P102");
            goto ErrExit;
        }
        ret=Dd_mst_Fet_Upd(&g_dd_mst,g_pub_tx.reply);
        if( ret )
        {
            sprintf(acErrMsg,"账号不存在[%d-%d][%d]",g_pub_tx.ac_id,g_pub_tx.ac_seqn,ret);
            WRITEMSG
            strcpy(g_pub_tx.reply,"P102");
            goto ErrExit;
        }
        ret=Dd_mst_Upd_Upd(g_dd_mst,g_pub_tx.reply);
        if( ret )
        {
            sprintf(acErrMsg,"账号不存在[%d-%d][%d]",g_pub_tx.ac_id,g_pub_tx.ac_seqn,ret);
            WRITEMSG
            strcpy(g_pub_tx.reply,"P102");
            goto ErrExit;
        }
    }
    if( ret )
    {
        sprintf(acErrMsg,"账号不存在[%d-%d][%d]",g_pub_tx.ac_id,g_pub_tx.ac_seqn,ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"P102");
        goto ErrExit;
    }
    
    sprintf(acErrMsg,"bal[%lf][%lf][%lf][%lf]",
        g_dd_mst.ctl_amt,g_dd_mst.hold_amt,g_pub_tx.tx_amt1,g_dd_mst.bal);
    WRITEMSG
    sprintf(acErrMsg,"[%s][%d]svc_ind=[%d]",__FILE__,__LINE__,g_pub_tx.svc_ind);
    WRITEMSG

    vtcp_log("[%s][%d] g_dd_mst.ac_id =[%d]",__FILE__,__LINE__,g_dd_mst.ac_id);
    if( (!strcmp(g_dd_mst.prdt_no,"131")||!strcmp(g_dd_mst.prdt_no,"142")) /** add by chenchao 2011/8/27 18:50:07 晋中要求担保保证金也限制***/
        && strcmp(g_pub_tx.tx_code,"4335") && strcmp(g_pub_tx.tx_code,"9001") 
        && strcmp(g_pub_tx.tx_code,"4334") && strcmp(g_pub_tx.tx_code,"G090") 
        && strcmp(g_pub_tx.tx_code,"4373") && strcmp(g_pub_tx.tx_code,"5801")
        && strcmp(g_pub_tx.tx_code,"5839") && strcmp(g_pub_tx.tx_code,"G037")
        && strcmp(g_pub_tx.tx_code,"2208") 
        && strcmp(g_pub_tx.tx_code,"4380") /*4380承兑汇票到期联机交易*/
        && strcmp(g_pub_tx.tx_code,"4964") /*4964撤票通知*/
        && g_pub_tx.svc_ind != 1005  /*结息不控制**/
    )
    {
		/** 如果是3201 贷款还本 3202 贷款还息 不卡 支持还贷款时扣划保证金 武大为 20150305 **/
		if(strcmp(g_pub_tx.tx_code,"3201")!=0 && strcmp(g_pub_tx.tx_code,"3202")!=0&& strcmp(g_pub_tx.tx_code,"5111")!=0)
		{
			vtcp_log("承兑保证金得去承兑保证金交易里去做\n",__FILE__,__LINE__);
			vtcp_log("%s,%d txcode===[%s]",__FILE__,__LINE__,g_pub_tx.tx_code);
			strcpy(g_pub_tx.reply,"P471");
			goto ErrExit;
		}
    }

    /**读取产品参数**/
    if( g_pub_tx.ac_get_ind[2]=='1' )
        ret=0;
    else
        ret = Dd_parm_Sel( g_pub_tx.reply,&g_dd_parm,
            " prdt_no='%s' ",g_dd_mst.prdt_no );
    ERR_DEAL

    strcpy(g_pub_tx.cur_no,g_dd_parm.cur_no);
    strcpy(g_pub_tx.dc_code,g_dd_parm.dc_code);
    strcpy(g_pub_tx.prdt_code,g_dd_mst.prdt_no);

    strcpy(g_pub_tx.name,g_dd_mst.name);
    strcpy(g_pub_tx.opn_br_no,g_dd_mst.opn_br_no);
    g_pub_tx.cif_no=g_dd_mst.cif_no;

    /** yud 添加对于结算户可以强行扣款标志ac_wrk_ind ***
    *** 第10位等于1时对于非结算户也可以扣款 20030819 **/
    /****NEWTCTD9:TODO:小额通存通兑允许非结算户****/
    if( g_pub_tx.ct_ind[0]!='1' && g_dd_mst.sttl_type[0]=='S' &&
         g_pub_tx.ac_wrk_ind[7]!='J' && g_pub_tx.svc_ind != 1012 &&
         g_pub_tx.ac_wrk_ind[9]!='1' &&
        memcmp(g_pub_tx.tx_code,"5840",4)!=0)
    {
        sprintf(acErrMsg,"非结算户不允许转账!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"P105");
        goto ErrExit;
    }
    sprintf(acErrMsg,"[%s][%d]svc_ind=[%d]",__FILE__,__LINE__,g_pub_tx.svc_ind);
    WRITEMSG

  /*Begin   Modified by ChenMing   725走帐时报错，所以加上以下条件  
    收到725时，客户帐号状态可能发生了改变，所以不检查帐号的状态*/
    
  get_zd_data("0510",cCmtno);
  vtcp_log("[%s][%d]  cCmtno======[%s]",__FILE__,__LINE__,cCmtno );
  vtcp_log("[%s][%d]  725不走客户帐，所以不检查帐号的状态  cCmtno======[%s]",__FILE__,__LINE__,cCmtno );
    if(memcmp(cCmtno,"725",3))
    {
        if( g_dd_mst.ac_sts[0]=='3' )
        {
            sprintf(acErrMsg,"该账户已挂失结清!");
            WRITEMSG
            strcpy(g_pub_tx.reply,"O174");
            goto ErrExit;
        }
        else if( g_dd_mst.ac_sts[0]=='5' )
        {
            sprintf(acErrMsg,"该账户处于临时销户状态!");
            WRITEMSG
            strcpy(g_pub_tx.reply,"O173");
            goto ErrExit;
        }
        else if( g_dd_mst.ac_sts[0]=='*' )
        {
            sprintf(acErrMsg,"该账户已作销户处理!");
            WRITEMSG
            strcpy(g_pub_tx.reply,"P245");
            goto ErrExit;
        }
        sprintf(acErrMsg,"[%s][%d]svc_ind=[%d]",__FILE__,__LINE__,g_pub_tx.svc_ind);
        WRITEMSG

        if( g_dd_mst.hold_sts[0]=='1' )
        {
            if( g_pub_tx.ac_wrk_ind[7]=='J' )
                vtcp_log("完全冻结仍要结息[%s]",g_pub_tx.ac_wrk_ind );
                /** 修改冻结扣划 **/
                /**   add by shiweina 20071217  **/
            else if (strcmp(g_pub_tx.tx_code,"2430")==0)
            {
                sprintf(acErrMsg,"[%s][%d]tx_code=%ld",__FILE__,__LINE__,g_pub_tx.tx_code);
                WRITEMSG
             
            }
            else if(memcmp(g_pub_tx.tx_code,"4713",4) == 0)/*增加扣划特殊处理 2011/11/13 14:00:48 */
            {
                vtcp_log("[%s][%d] 全部冻结仍可以扣划[%s]",__FILE__,__LINE__,g_pub_tx.tx_code );
            }
            else
            {
                sprintf(acErrMsg,"该账号已被完全冻结!");
                WRITEMSG
                strcpy(g_pub_tx.reply,"P110");
                goto ErrExit;
            }
        }

    }
  /*End    Modified by ChenMing  725不走客户帐，所以不检查帐号的状态   */
  
  sprintf(acErrMsg,"[%s][%d]svc_ind=[%d]",__FILE__,__LINE__,g_pub_tx.svc_ind);
  WRITEMSG

    vtcp_log("------look------[%s]",g_mdm_ac_rel.ac_no);
    /*取款、销户、协定销户*/
    if( g_pub_tx.add_ind[0]=='0')
    {
        /** add by chenchao 贷款检查委托支取协议 2011-7-31 17:51:47 begin **/
        if( (memcmp(g_pub_tx.tx_code,"2430",4)!=0)  /** 扣划不检查  **/
            && (memcmp(g_pub_tx.tx_code,"G083",4)!=0) /** 贷款计息 还息不检查 **/
            && (memcmp(g_pub_tx.tx_code,"G085",4)!=0) /** 还款计划表日终不检查 **/
            && (memcmp(g_pub_tx.tx_code,"G086",4)!=0) /** 按揭欠款表每日不检查  **/
            && (memcmp(g_pub_tx.tx_code,"G088",4)!=0) /** 日终贷款到期 不检查 **/
            && (memcmp(g_pub_tx.tx_code,"4713",4)!=0) /*扣划特殊处理 2011/11/13 14:05:41*/
            && (dd_limit()))
            
        {
            vtcp_log("[%s][%d]金额支取限制错误",__FILE__,__LINE__);
            goto ErrExit;
        }
        /** add by chenchao 贷款检查委托支取协议 2011-7-31 17:51:47 end **/
        if( g_dd_mst.ac_sts[0]=='0' )
        {   
            sprintf(acErrMsg,"该账户处于开户待确认状态，请先作开户确认!");
            WRITEMSG
            strcpy(g_pub_tx.reply,"O172");
            goto ErrExit;
        }
        sprintf(acErrMsg,"[%s][%d]svc_ind=[%d]",__FILE__,__LINE__,g_pub_tx.svc_ind);
        WRITEMSG

        /*** 承兑保证金户不允许取销 ***/
        if( !strncmp(g_dd_parm.prdt_no,"131",3) && g_pub_tx.ac_wrk_ind[8]!='1' && strcmp(g_pub_tx.tx_code,"5801")  && strcmp(g_pub_tx.tx_code,"2208"))
        {
            sprintf(acErrMsg,"承兑保证金户不允许取款!");
            WRITEMSG
            strcpy(g_pub_tx.reply,"D212");
            goto ErrExit;
        }
    sprintf(acErrMsg,"[%s][%d]svc_ind=[%d]",__FILE__,__LINE__,g_pub_tx.svc_ind);
    WRITEMSG

        if( g_pub_tx.svc_ind!=1003 && g_pub_tx.svc_ind!=1004 )
        {
            if( g_dd_parm.draw_ind[0]=='N' )
            if ( g_dd_parm.draw_ind[0]=='N' && memcmp(g_pub_tx.tx_code,"4713",4 )!=0)/* 扣划特殊处理 2011/11/13 14:06:51*/
            {
                sprintf(acErrMsg,"不可续取[%s]!",g_dd_parm.draw_ind);
                WRITEMSG
                strcpy(g_pub_tx.reply,"A023");
                goto ErrExit;
            }
            if( g_dd_parm.thr_draw_ind[0]=='N' 
            && strcmp(g_pub_tx.tx_br_no,g_pub_tx.opn_br_no) &&
            g_pub_tx.ac_wrk_ind[8]!='1' && g_pub_tx.ac_wrk_ind[9]!='1' )
            {
                sprintf(acErrMsg,"不可通兑[%s]!",g_dd_parm.thr_draw_ind);
                WRITEMSG
                strcpy(g_pub_tx.reply,"A011");
                goto ErrExit;
            }
        }
    sprintf(acErrMsg,"[%s][%d]svc_ind=[%d],ct_ind[%c],cash_ind[%c]",__FILE__,__LINE__,g_pub_tx.svc_ind,g_pub_tx.ct_ind[0],g_dd_mst.cash_ind[0]);
    WRITEMSG

        if( g_pub_tx.ct_ind[0]=='1' ) /**取现**/
        {
            /*临时户、专用户需检查提现标志 一般户也不允许提现 20070116*/
            if( g_dd_mst.ac_type[0]!='1' && g_dd_mst.ac_type[0]!='4' && \
                g_dd_mst.ac_type[0]!='5' && g_dd_mst.ac_type[0]!='7') 
                
            {
                if(g_dd_mst.cash_ind[0]=='N')
                {
                    sprintf(acErrMsg,"此账户不允许取现!");
                    WRITEMSG
                    strcpy(g_pub_tx.reply,"P107");
                    goto ErrExit;
                }
            }

            if( g_dd_mst.odtt_ind[0]=='1' )
            {
                sprintf(acErrMsg,"汇户不允许取现!");
                WRITEMSG
                strcpy(g_pub_tx.reply,"P108");
                goto ErrExit;
            }

            if( g_dd_parm.cash_ind[0]=='N' && g_pub_tx.tx_amt1>0.001)/*0金额的就不管了 2006-12-11 */
            {
                sprintf(acErrMsg,"产品不允许取现!");
                WRITEMSG
                strcpy(g_pub_tx.reply,"A012");
                goto ErrExit;
            }
        }
        else if( g_pub_tx.ct_ind[0]=='2' )
        {
            /****NEWTCTD9:TODO:小额通存通兑允许非结算户****/
            if( g_dd_mst.sttl_type[0]=='S' && g_pub_tx.ac_wrk_ind[7]!='J' && \
                g_pub_tx.svc_ind != 1012 && g_pub_tx.ac_wrk_ind[9]!='1' &&
                memcmp(g_pub_tx.tx_code,"5840",4)!=0)
            {
                sprintf(acErrMsg,"非结算户不允许转账![%s][%d]",
                    g_pub_tx.svc_ind,g_pub_tx.ac_wrk_ind);
                WRITEMSG
                strcpy(g_pub_tx.reply,"O170");
                goto ErrExit;
            }
        }

        sprintf(acErrMsg,"OKOKOKOKOKOKac_wrk_ind=[%s]!", g_pub_tx.ac_wrk_ind);
        WRITEMSG
        /*** 调用校验支取方式、校验密码函数 ***/
        if(g_pub_tx.ac_wrk_ind[3]=='1' )
        {
            sprintf(acErrMsg,"%s,%d,draw_pwd_yn=[%c],draw_id_yn=[%c]",
                __FILE__,__LINE__,g_mdm_ac_rel.draw_pwd_yn[0],g_mdm_ac_rel.draw_id_yn[0]);
            WRITEMSG
            vtcp_log("%s,%d,g_pub_tx.ac_no=[%s]",__FILE__,__LINE__,g_pub_tx.ac_no);
            /*add by hxc 20091012*/
                if(memcmp(g_pub_tx.ac_no,CARDHEADCODE,6)==0 || memcmp(g_pub_tx.ac_no,ICCARDHEADCODE,6)==0){/**20091012 晋中新增条件memcmp(g_pub_tx.ac_no,CARDHEADCODE,6)==0**/
                    g_mdm_ac_rel.draw_pwd_yn[0] = 'N';
                    vtcp_log("%s,%d,晋中卡核心不检查密码draw_pwd_yn[%s]g_pub_tx.tx_amt1[%f]",__FILE__,__LINE__,g_mdm_ac_rel.draw_pwd_yn,g_pub_tx.tx_amt1);
                }
                if(memcmp(g_pub_tx.tx_code,"6789",sizeof(g_pub_tx.tx_code)-1)==0){
                    g_mdm_ac_rel.draw_pwd_yn[0] = 'N';
                    vtcp_log("%s,%d,晋中财税库银扣税不检查密码draw_pwd_yn[%s]",__FILE__,__LINE__,g_mdm_ac_rel.draw_pwd_yn);
                }
            /*end by hxc 20091012*/
            {/*add by wyw 20080316*/
                vtcp_log("%s,%d,draw_pwd_yn[%s]g_pub_tx.tx_amt1[%f]",__FILE__,__LINE__,g_pub_tx.draw_pwd_yn,g_pub_tx.tx_amt1);
                if(memcmp(g_pub_tx.tx_code,"6203",4)==0/**&& g_pub_tx.tx_amt1<0.001***/){/*20090729宁夏去掉条件&& g_pub_tx.tx_amt1<0.001*/
                    g_mdm_ac_rel.draw_pwd_yn[0] = g_pub_tx.draw_pwd_yn[0];
                    vtcp_log("%s,%d,draw_pwd_yn[%s]g_pub_tx.tx_amt1[%f]",__FILE__,__LINE__,g_pub_tx.draw_pwd_yn,g_pub_tx.tx_amt1);
                }
                
            }/*add by wyw 20080316*/
            ret = pub_base_check_draw("001");
            ERR_DEAL
        }
    sprintf(acErrMsg,"[%s][%d]svc_ind=[%d]",__FILE__,__LINE__,g_pub_tx.svc_ind);
    WRITEMSG

        /*** 协定划款处理 ***
        if( g_dd_mst.bal-g_pub_tx.tx_amt1<g_dd_mst.ctl_amt+g_dd_mst.hold_amt
        || g_dd_mst.bal-g_pub_tx.tx_amt1<g_dd_mst.ctl_amt+g_dd_parm.min_bal )
        if( g_dd_mst.bal-g_pub_tx.tx_amt1<g_dd_mst.ctl_amt+g_dd_mst.hold_amt+g_dd_parm.min_bal && g_dd_mst.ac_type[0]!='6' )
        **********************/
        /****修改为用pub_base_CompDblVal函数来比较大小 modify at 20120108****/
       if( pub_base_CompDblVal(g_dd_mst.bal-g_pub_tx.tx_amt1,g_dd_mst.ctl_amt+g_dd_mst.hold_amt+g_dd_parm.min_bal)<0 && g_dd_mst.ac_type[0]!='6' )
        {
            ret=pub_base_CompDblVal(g_dd_mst.hold_amt,g_dd_parm.min_bal);
            if( ret>=0 ) 
        tmpje=g_dd_mst.ctl_amt+g_dd_mst.hold_amt+g_pub_tx.tx_amt1-g_dd_mst.bal;
            else 
        tmpje=g_dd_mst.ctl_amt+g_dd_parm.min_bal+g_pub_tx.tx_amt1-g_dd_mst.bal;
            
            if( pub_sub_prot(tmpje) )
            {
                sprintf(acErrMsg,"协定划款处理错误!");
                WRITEMSG
                goto ErrExit;
            }
        }
    vtcp_log("[%s][%d] 222222g_dd_mst.bal =%17.2f]",__FILE__,__LINE__,g_dd_mst.bal);
    sprintf(acErrMsg,"[%s][%d]svc_ind=[%d]",__FILE__,__LINE__,g_pub_tx.svc_ind);
    WRITEMSG
        /*** 调用算利息积数的函数 ***/
        if( g_pub_tx.svc_ind!=1006 && g_pub_tx.svc_ind!=1012 )
        {
            if( g_dd_mst.lst_date<g_dd_mst.ic_date && g_dd_mst.ic_date>19000000 )
            {
                if( pub_base_CalAcm(g_dd_mst.ic_date,g_pub_tx.tx_date,
                g_dd_parm.intst_term_type,g_dd_parm.acm_calc_type,
                g_dd_mst.bal,-g_pub_tx.tx_amt1,&g_dd_mst.intst_acm,
                g_dd_mst.intst_type,g_dd_parm.intst_mon,g_dd_parm.intst_day) )
                {
                    sprintf(acErrMsg,"计算利息积数错误!");
                    WRITEMSG
                    goto ErrExit;
                }
            }
            else
            {
                if( pub_base_CalAcm(g_dd_mst.lst_date,g_pub_tx.tx_date,
                g_dd_parm.intst_term_type,g_dd_parm.acm_calc_type,
                g_dd_mst.bal,-g_pub_tx.tx_amt1,&g_dd_mst.intst_acm,
                g_dd_mst.intst_type,g_dd_parm.intst_mon,g_dd_parm.intst_day) )
                {
                    sprintf(acErrMsg,"计算利息积数错误!");
                    WRITEMSG
                    goto ErrExit;
                }
            }
        }
    sprintf(acErrMsg,"[%s][%d]svc_ind=[%d]",__FILE__,__LINE__,g_pub_tx.svc_ind);
    WRITEMSG
    vtcp_log("[%s][%d] 333333g_dd_mst.bal =%17.2f]",__FILE__,__LINE__,g_dd_mst.bal);


        switch( g_pub_tx.svc_ind )
        {
            case 1001:      /*存取款，此处为取款*/   /* 按财务部要求对协定账户取款不做限制 wudawei 20130717 */
                vtcp_log("[%s][%d] g_dd_mst.ac_id =[%d]",__FILE__,__LINE__,g_dd_mst.ac_id);
               /* if( g_dd_mst.ac_type[0]=='6' || g_dd_mst.ac_type[0]=='7' ) */
                if( g_dd_mst.ac_type[0]=='7' )
                {
                    sprintf(acErrMsg,"此户为协定或理财账号，不能直接存取款!");
                    WRITEMSG
                    strcpy(g_pub_tx.reply,"P293");
                    goto ErrExit;
                }
    sprintf(acErrMsg,"[%s][%d]g_dd_mst.ac_type[0]=[%c]",__FILE__,__LINE__,g_dd_mst.ac_type[0]);
    WRITEMSG
            case 1009:      /*** 公积金户取款 ***/
                if( g_pub_tx.svc_ind==1009 )
                {
                    g_pub_tx.tx_amt1=g_dd_mst.bal;
                    if( strcmp(g_dd_mst.prdt_no,"102") )
                    {
                        sprintf(acErrMsg,"非公积金户![%s]",g_dd_mst.prdt_no);
                        WRITEMSG
                        strcpy(g_pub_tx.reply,"P416");
                        goto ErrExit;
                    }
                }
        sprintf(acErrMsg,"[%s][%d]g_dd_mst.ac_type[0]=[%c]",__FILE__,__LINE__,g_dd_mst.ac_type[0]);
        WRITEMSG
            case 1008:      /*协定户存取款，此处为取款*/
                /*** 取款时检查卡限额登记薄并更新 ***/
                /*** ADD COMMENCE BY Azz.05.03.26 ***/
                ret = Card_consum_lmt_Dec_Upd( g_pub_tx.reply,"ac_no='%s'", \
                                                g_pub_tx.ac_no); 
        sprintf(acErrMsg,"[%s][%d]注意 ret=[%d]ac_no=[%s]",__FILE__,__LINE__,ret,g_pub_tx.ac_no);
        WRITEMSG
                if( ret )
                {
                    sprintf(acErrMsg,"DECLARE 卡限额登记薄异常!");
                    WRITEMSG
                    strcpy(g_pub_tx.reply,"P121");
                    goto ErrExit;
                }

                ret = Card_consum_lmt_Fet_Upd( &g_card_consum_lmt, \
                                                g_pub_tx.reply);
                if( ret&&ret!=100 )
                {
                    sprintf(acErrMsg,"FETCH 卡限额登记薄异常!");
                    WRITEMSG
                    strcpy(g_pub_tx.reply,"P120");
                    goto ErrExit;
                }
                else if( ret==0 )
                {
                    /*** 取现 ***/
                    vtcp_log("[%s][%d] g_dd_mst.ac_id =[%d]",__FILE__,__LINE__,g_dd_mst.ac_id);
                    if( g_pub_tx.ct_ind[0]=='1' && \
                    pub_base_CompDblVal(g_card_consum_lmt.da_cash_lmt,0.00)>0 )
                    {
                        /*** 取现检查卡限额表 ****/
                        if( pub_base_CompDblVal( g_pub_tx.tx_amt1+g_card_consum_lmt.da_all_cash_lmt,g_card_consum_lmt.da_cash_lmt) > 0 )
                        {
                            sprintf(acErrMsg,"超过每日最大取现限额!");
                            WRITEMSG
                            strcpy(g_pub_tx.reply,"P231");
                            goto ErrExit;
                        }
                        if( pub_base_CompDblVal( g_pub_tx.tx_amt1+g_card_consum_lmt.mo_all_cash_lmt,g_card_consum_lmt.mo_cash_lmt) > 0 )
                        {
                            sprintf(acErrMsg,"超过每月最大取现限额!");
                            WRITEMSG
                            strcpy(g_pub_tx.reply,"P232");
                            goto ErrExit;
                        }

                        g_card_consum_lmt.da_all_cash_lmt = \
                        g_pub_tx.tx_amt1+g_card_consum_lmt.da_all_cash_lmt;

                        g_card_consum_lmt.mo_all_cash_lmt = \
                        g_pub_tx.tx_amt1+g_card_consum_lmt.mo_all_cash_lmt;
                    }
                    else if( pub_base_CompDblVal(g_card_consum_lmt.da_amt_lmt, \
                                                 0.00)>0 )
                    {
                        if( pub_base_CompDblVal( g_pub_tx.tx_amt1+g_card_consum_lmt.da_all_amt_lmt,g_card_consum_lmt.da_amt_lmt ) > 0 )
                        {
                            sprintf(acErrMsg,"超过每日最大取款限额![%.2lf] \
                            [%.2lf][%.2lf]",g_pub_tx.tx_amt1,g_card_consum_lmt.da_all_amt_lmt,g_card_consum_lmt.da_amt_lmt);
                            WRITEMSG
                            strcpy(g_pub_tx.reply,"P233");
                            goto ErrExit;
                        }
                        if( pub_base_CompDblVal( g_pub_tx.tx_amt1+g_card_consum_lmt.mo_all_amt_lmt,g_card_consum_lmt.mo_amt_lmt) > 0 )
                        {
                            sprintf(acErrMsg,"超过每月最大取款限额!");
                            WRITEMSG
                            strcpy(g_pub_tx.reply,"P234");
                            goto ErrExit;
                        }

                        g_card_consum_lmt.da_all_amt_lmt = \
                        g_pub_tx.tx_amt1+g_card_consum_lmt.da_all_amt_lmt;

                        g_card_consum_lmt.mo_all_amt_lmt = \
                        g_pub_tx.tx_amt1+g_card_consum_lmt.mo_all_amt_lmt;
                    }

                    ret = Card_consum_lmt_Upd_Upd( g_card_consum_lmt, \
                                                    g_pub_tx.reply);
                    if( ret )
                    {
                        sprintf(acErrMsg,"UPDATE ERROR CODE=[%d]!",ret);
                        WRITEMSG
                        strcpy(g_pub_tx.reply,"D105");
                        goto ErrExit;
                    }
                }
                Card_consum_lmt_Clo_Upd();
                /*** ADD COMPLETE ***/
                vtcp_log("[%s][%d] 5555555555g_dd_mst.bal =%17.2f]",__FILE__,__LINE__,g_dd_mst.bal);

        sprintf(acErrMsg,"!!![%s][%d] 进行余额变化ret=[%d]ac_no=[%s]",__FILE__,__LINE__,ret,g_pub_tx.ac_no);
                g_dd_mst.bal=g_dd_mst.bal-g_pub_tx.tx_amt1;
                vtcp_log("[%s][%d] 666666666g_dd_mst.bal =%17.2f]",__FILE__,__LINE__,g_dd_mst.bal);
                g_dd_mst.acbo_bal=g_dd_mst.acbo_bal-g_pub_tx.tx_amt1;
                break;
            case 1006:      /*预授权*/
                g_dd_mst.ctl_amt=g_dd_mst.ctl_amt+g_pub_tx.tx_amt1;
                vtcp_log("%s,%d,POS预授权 g_pub_tx.tx_amt1[%lf],ctl[%lf]",__FILE__,__LINE__,g_pub_tx.tx_amt1,g_dd_mst.ctl_amt);
                break;
            case 1007:      /*预授权完成*/
                /*BEGIN:del by hxc 091117*/
                /**if( pub_base_CompDblVal(g_dd_mst.ctl_amt,g_pub_tx.tx_amt1)<0 )
                {
                    sprintf(acErrMsg,"预授权完成金额错[%.2lf]<[%.2lf]!",
                        g_dd_mst.ctl_amt,g_pub_tx.tx_amt1);
                    WRITEMSG
                    strcpy( g_pub_tx.reply,"A031" );
                    goto ErrExit;
                }
                g_dd_mst.ctl_amt=0.00;
                g_dd_mst.bal=g_dd_mst.bal-g_pub_tx.tx_amt1;
                g_dd_mst.acbo_bal=g_dd_mst.acbo_bal-g_pub_tx.tx_amt1;
                break;**/
                /*End:del by hxc 091117*/
                /*BEGIN:add by hxc 091117 预授权完成金额可以是控制金额的115% */
                ret = pub_base_GetParm_double("YSQWC",1,&ysqwc_rate);
                if(ret)
                {
                    strcpy(g_pub_tx.reply,"CD06");
                    vtcp_log("%s,%d,取预授权完成上浮比率失败!",__FILE__,__LINE__);
                    goto ErrExit;
                }
                vtcp_log("%s,%d,上浮比率[%lf]",__FILE__,__LINE__,ysqwc_rate);
                zg_amt = g_dd_mst.ctl_amt * ( 1 + ysqwc_rate);
                vtcp_log("%s,%d,上浮比率[%.2f],最高金额[%.2f]",__FILE__,__LINE__,ysqwc_rate,zg_amt);
                
                if ( pub_base_CompDblVal(g_pub_tx.tx_amt1,zg_amt)==1 )
                {
                    sprintf(acErrMsg,"预授权完成金额过高[%.2lf]>[%.2lf]!",
                        g_pub_tx.tx_amt1,zg_amt);
                    WRITEMSG
                        strcpy( g_pub_tx.reply,"A031" );
                    goto ErrExit;
                }
                /*g_dd_mst.ctl_amt=0.00;*/
                            /*预授权完成业务有冲正和撤销,因此要恢复原预授权金额*/
                vtcp_log("%s,%d,g_dd_mst.ctl_amt=[%.2f],g_pub_tx.tx_amt2=[%.2f]",__FILE__,__LINE__,g_dd_mst.ctl_amt,g_pub_tx.tx_amt2);
                g_dd_mst.ctl_amt = g_dd_mst.ctl_amt - g_pub_tx.tx_amt2;
                vtcp_log("%s,%d,g_dd_mst.ctl_amt=[%.2f],g_pub_tx.tx_amt2=[%.2f]",__FILE__,__LINE__,g_dd_mst.ctl_amt,g_pub_tx.tx_amt2);
                if(pub_base_CompDblVal(g_dd_mst.ctl_amt,0.00)<0)
                {
                    vtcp_log("%s,%d,控制金额小于预授权金额!ctl[%lf],tx_amt2[%lf]",__FILE__,__LINE__,g_dd_mst.ctl_amt,g_pub_tx.tx_amt2);
                    strcpy(g_pub_tx.reply,"CU14");
                    goto ErrExit;
                }
                g_dd_mst.bal=g_dd_mst.bal-g_pub_tx.tx_amt1;
                g_dd_mst.acbo_bal=g_dd_mst.acbo_bal-g_pub_tx.tx_amt1;
                /*END: add by hxc 091117*/
                break;
            case 1010:      /*公积金活期销户*/
                g_pub_tx.tx_amt1=g_dd_mst.bal;
                if( strcmp(g_dd_mst.prdt_no,"102") )
                {
                    sprintf(acErrMsg,"非公积金户![%s]",g_dd_mst.prdt_no);
                    WRITEMSG
                    strcpy(g_pub_tx.reply,"P416");
                    goto ErrExit;
                }
            case 1003:      /*销户*/

                vtcp_log("[%s][%d] g_dd_mst.ac_id =[%d]",__FILE__,__LINE__,g_dd_mst.ac_id);
                if( g_dd_parm.thr_cls_ind[0]=='N' 
                    && strcmp(g_pub_tx.tx_br_no,g_pub_tx.opn_br_no) )
                {
                    sprintf(acErrMsg,"不可通销[%s]!",g_dd_parm.thr_draw_ind);
                    WRITEMSG
                    strcpy(g_pub_tx.reply,"A007");
                    goto ErrExit;
                }
                if( pub_base_CompDblVal(g_dd_mst.bal,g_pub_tx.tx_amt1) )
                {
                    sprintf(acErrMsg,"销户后余额不为零![.%2lf][%.2lf]", \
                            g_dd_mst.bal,g_pub_tx.tx_amt1);
                    WRITEMSG
                    strcpy(g_pub_tx.reply,"P116");
                    goto ErrExit;
                }

                /*检查帐户是否还有介质未销*/
                ret = sql_count("mdm_ac_rel","ac_id=%ld and note_sts not in('2','3','4','5','*')", \
                                g_mdm_ac_rel.ac_id );
                if( ret<0 )
                {
                    sprintf(acErrMsg,"TRANSFER FUNCTION SQL_COUNT ERROR!");
                    WRITEMSG
                    goto ErrExit;
                }
                else if( ret>1 )
                {
                    sprintf(acErrMsg,"该帐户还有其他介质未销，请先做销介质!");
                    WRITEMSG
                    strcpy(g_pub_tx.reply,"O199");
                    goto ErrExit;
                }

                /* add by xxx at 20030805 有未登折明细的不能销户 先补登 */
                sprintf(acErrMsg, "CGH *******trace_no=[%ld], tx_date=[%ld]", 
                    g_pub_tx.trace_no, g_pub_tx.tx_date);
                WRITEMSG
                sprintf(acErrMsg, "ac_id= [%ld], ac_seqn=[%d]", 
                    g_dd_mst.ac_id, g_dd_mst.ac_seqn);
                WRITEMSG
                vtcp_log("[%s][%d] g_dd_mst.ac_id =[%d]",__FILE__,__LINE__,g_dd_mst.ac_id);
                ret = sql_count("mdm_unprt_hst", "(trace_no!=%ld or \
                    tx_date != %ld) and ac_id=%ld and ac_seqn=%d",
                    g_pub_tx.trace_no, g_pub_tx.tx_date, g_dd_mst.ac_id,
                    g_dd_mst.ac_seqn);
                if (ret < 0)
                {
                    sprintf(acErrMsg,"TRANSFER FUNCTION SQL_COUNT ERROR!");
                    WRITEMSG
                    goto ErrExit;
                }
                else if (ret>0)
                {
                    /*增加扣划特殊处理 2011/11/13 14:09:26*/
                    if(memcmp(g_pub_tx.tx_code,"4713",sizeof(g_pub_tx.tx_code)-1)==0 && memcmp(g_pub_tx.brf,"扣划业务",8)==0)
                    {
                        vtcp_log("[%s][%d]扣划不检查未登折明细",__FILE__,__LINE__);
                    }
                    /* 储蓄销户时 挂失销户不检查未登折明细 wudawei 20130716 */
                   	else if(memcmp(g_pub_tx.tx_code,"2204",sizeof(g_pub_tx.tx_code)-1)==0 && memcmp(g_mdm_ac_rel.note_sts,"1",1)==0)
                   	{
                   			vtcp_log("[%s][%d]储蓄挂失销户不检查未登折明细",__FILE__,__LINE__);
                   	}
                   	/* add end 20130716 */
                    else 
                    {
                        sprintf(acErrMsg, "还有未登折明细，不能销户!!");
                        WRITEMSG
                        strcpy(g_pub_tx.reply, "D217");
                        goto ErrExit;
                    }
                }
                sprintf(acErrMsg, "CGH***********ret =[%d]", ret);
                WRITEMSG

                
                /* 调用计息函数 */
                memset(&g_pub_intst, 0x0, sizeof(struct S_pub_intst));
                if (g_dd_mst.ac_sts[0] != '#')
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
                    sprintf(acErrMsg,"CCCC**************利息为 = [%lf]",
                    g_pub_intst.factval);
                    WRITEMSG
                }

                strcpy(g_dd_mst.ac_sts,"*");

                g_dd_mst.bal=g_dd_mst.bal-g_pub_tx.tx_amt1;
                g_dd_mst.acbo_bal=g_dd_mst.acbo_bal-g_pub_tx.tx_amt1;
                strcpy(g_pub_tx.brf, "销户");
                break;
            case 1004:      /*协定销户*/
                if( g_dd_parm.thr_cls_ind[0]=='N' 
                    && strcmp(g_pub_tx.tx_br_no,g_pub_tx.opn_br_no) )
                {
                    sprintf(acErrMsg,"不可通销[%s]!",g_dd_parm.thr_draw_ind);
                    WRITEMSG
                    strcpy(g_pub_tx.reply,"A007");
                    goto ErrExit;
                }
                /* 判断先计息后销户 added by liuxuan 20071010*/
                if(g_dd_mst.lst_date-g_pub_tx.tx_date!=0 && g_dd_mst.intst_acm>0.005)
                {
                    sprintf(acErrMsg,"先计息后销户!!");
                    WRITEMSG
                    strcpy(g_pub_tx.reply,"P098");
                    goto ErrExit;
                }
                /* added over */
                g_pub_tx.tx_amt1=g_dd_mst.bal;
                strcpy(g_dd_mst.ac_sts,"*");
                g_dd_mst.bal=g_dd_mst.bal-g_pub_tx.tx_amt1;
                g_dd_mst.acbo_bal=g_dd_mst.acbo_bal-g_pub_tx.tx_amt1;
                strcpy(g_pub_tx.brf, "协定销户");
                break;
            default :
                vtcp_log("[%s][%d] g_dd_mst.ac_id =[%d]",__FILE__,__LINE__,g_dd_mst.ac_id);
                g_dd_mst.bal=g_dd_mst.bal-g_pub_tx.tx_amt1;
                g_dd_mst.acbo_bal=g_dd_mst.acbo_bal-g_pub_tx.tx_amt1;
                sprintf( acErrMsg,"操作类型[%d]视同取款",g_pub_tx.svc_ind );
                WRITEMSG
                break;
        }
        
        if( g_pub_tx.hst_ind[0]=='1' )      /*日间入明细*/
        {
            g_dd_mst.hst_bal=g_dd_mst.hst_bal-g_pub_tx.tx_amt1;
        }
        /** 扣划  **/
    /***    add by  shiweina   **/
    if(strcmp(g_pub_tx.tx_code,"2430")==0)
    {   get_zd_data("0700",kh_type);
        vtcp_log("%s,%d,Gethere7!!!!notice!FD70kh_type=[%s]\n",__FILE__,__LINE__,kh_type);
        if(kh_type[0]=='2')
        {
      if(g_dd_mst.hold_sts[0]=='3' )  
      {
         vtcp_log("%s,%d,Gethere7\n",__FILE__,__LINE__);
       get_zd_long("0350",&hold_seqn);
       get_zd_long("0340",&ac_seqn);
       vtcp_log("[%s][%d] hold_seqn=%ld ",__FILE__,__LINE__,hold_seqn);
       ret = Mo_hold_Sel(g_pub_tx.reply,&g_mo_hold,"hold_seqn=%ld  and ac_id=%ld and ac_seqn=%ld",hold_seqn,g_pub_tx.ac_id,ac_seqn);
        
        if(ret==100)
        {
           sprintf(acErrMsg," 数据库无此记录[%s]",g_pub_tx.reply);
           WRITEMSG
           goto ErrExit;
        }
        if(ret!=0)
        {
          sprintf(acErrMsg,"查询冻结登记簿出错[%s]",g_pub_tx.reply);
          WRITEMSG
          goto ErrExit;
        } 
          vtcp_log("%s,%d,Gethere7\n",__FILE__,__LINE__);
        if( fabs(g_mo_hold.plan_hold_amt-g_pub_tx.tx_amt1)<0.005)
        {  
            vtcp_log("%s,%d,Gethere7\n",__FILE__,__LINE__);
          vtcp_log("[%s][%d] jinjinjinjinjin=%.2f ",__FILE__,__LINE__,g_mo_hold.plan_hold_amt);
          vtcp_log("[%s][%d] eeeeeeeeeeeeee=%.2f ",__FILE__,__LINE__,g_pub_tx.tx_amt1);
          ret = Mo_hold_Dec_Upd(g_pub_tx.reply,"hold_seqn=%ld and  ac_id=%ld and ac_seqn=%ld",hold_seqn,g_pub_tx.ac_id,ac_seqn);
          vtcp_log("%s,%d,shiweinashiweina\n",__FILE__,__LINE__);
          if(ret)
          {

             sprintf(acErrMsg,"执行Mo_hold_Dec_Upd出错[%d]",ret);
             WRITEMSG
             goto ErrExit;
          }
           vtcp_log("%s,%d,shiweinashiweina\n",__FILE__,__LINE__);
           ret = Mo_hold_Fet_Upd( &g_mo_hold,g_pub_tx.reply);
           vtcp_log("%s,%d,shiweinashiweina\n",__FILE__,__LINE__);
           if(ret==100)
           {
              sprintf( acErrMsg,"查询登记簿错,无记录");
              WRITEMSG
              goto ErrExit;
           }
           else if (ret)
           {
              sprintf( acErrMsg,"执行Mo_hold_Fet_Upd出错[%d]",ret);
              WRITEMSG
              goto ErrExit;
           }   
           vtcp_log("%s,%d,shiweinashiweina\n",__FILE__,__LINE__);  
           strcpy(g_mo_hold.hold_sts,"1");
           g_mo_hold.fct_unhold_date = g_pub_tx.tx_date;
           strcpy(g_mo_hold.unhold_br_no,g_pub_tx.tx_br_no);
           strcpy(g_mo_hold.unhold_tel,g_pub_tx.tel);
           strcpy(g_mo_hold.unhold_brf,g_pub_tx.brf);
           g_mo_hold.wrk_date = g_pub_tx.tx_date;
           g_mo_hold.trace_no = g_pub_tx.trace_no;
             vtcp_log("%s,%d,Gethere7\n",__FILE__,__LINE__);
           vtcp_log("%s,%d,shiweinashiweina\n",__FILE__,__LINE__);
           ret = Mo_hold_Upd_Upd(g_mo_hold,g_pub_tx.reply);
           vtcp_log("%s,%d,shiweinashiweina\n",__FILE__,__LINE__);
           if( ret )
           {
               sprintf(acErrMsg," 修改记录出错[%d]",ret);
               WRITEMSG
               goto ErrExit;
           }
             vtcp_log("%s,%d,Gethere7\n",__FILE__,__LINE__);
           Mo_hold_Clo_Upd();
           }
           else if (fabs( g_mo_hold.plan_hold_amt-g_pub_tx.tx_amt1)>0.005)
           {
              vtcp_log("[%s][%d],oooooooooo=%.2f",__FILE__,__LINE__,g_mo_hold.plan_hold_amt);
              vtcp_log("[%s][%d],qqqqqqqqqqq=%.2f",__FILE__,__LINE__,g_pub_tx.tx_amt1); 
                vtcp_log("%s,%d,Gethere7\n",__FILE__,__LINE__);
              ret = Mo_hold_Dec_Upd ( g_pub_tx.reply,"hold_seqn=%ld and ac_id=%ld and ac_seqn=%ld",hold_seqn,g_pub_tx.ac_id,ac_seqn);

              if(ret)
              {
                 sprintf(acErrMsg,"执行Mo_hold_Dec_Upd出错[%d]",ret);
                 WRITEMSG
                 goto ErrExit;
              }
              ret = Mo_hold_Fet_Upd( &g_mo_hold,g_pub_tx.reply);
              if(ret==100)
              {
                 sprintf( acErrMsg,"查询登记簿错,无记录");
                 WRITEMSG
                 goto ErrExit;
              }
              else if(ret)
              {
                 sprintf( acErrMsg,"执行Mo_hold_Fet_Upd出错[%d]",ret);
                 WRITEMSG
                 goto ErrExit;
              } 
              g_mo_hold.plan_hold_amt=g_mo_hold.plan_hold_amt-g_pub_tx.tx_amt1;
              g_mo_hold.wrk_date = g_pub_tx.tx_date;
              g_mo_hold.trace_no = g_pub_tx.trace_no;
              ret = Mo_hold_Upd_Upd(g_mo_hold,g_pub_tx.reply);
              if(ret)
              {
                  sprintf(acErrMsg," 修改记录出错[%d]",ret);
                  WRITEMSG
                  goto ErrExit;
              }
              Mo_hold_Clo_Upd();
            } 
            else if ( fabs(g_pub_tx.tx_amt1-g_mo_hold.plan_hold_amt)>0.005)
            {
                sprintf(acErrMsg,"扣划金额大于冻结金额");
                WRITEMSG
                goto ErrExit;
            }   
       }
       else if (g_dd_mst.hold_sts[0]=='2' || g_dd_mst.hold_sts[0]=='1' )
          {
            get_zd_long("0350",&hold_seqn);
            get_zd_long("0340",&ac_seqn);
              vtcp_log("%s,%d,Gethere7\n",__FILE__,__LINE__);
            ret = Mo_hold_Dec_Upd(g_pub_tx.reply,"hold_seqn=%ld and ac_id=%ld and  ac_seqn=%ld",hold_seqn,g_pub_tx.ac_id,ac_seqn );
            vtcp_log("[%s][%d] retretret======%d",__FILE__,__LINE__,ret);
            if(ret)
            {
                 sprintf(acErrMsg,"执行Mo_hold_Dec_Upd出错[%d]",ret);
                 WRITEMSG
                 goto ErrExit;
            }
            ret = Mo_hold_Fet_Upd( &g_mo_hold,g_pub_tx.reply);
            vtcp_log("[%s][%d] retretret======%d",__FILE__,__LINE__,ret);
            if(ret==100)
            {
                 sprintf( acErrMsg,"查询登记簿错,无记录");
                 WRITEMSG
                 goto ErrExit;
            }
            else if (ret)
            {
                 sprintf( acErrMsg,"执行Mo_hold_Fet_Upd出错[%d]",ret);
                 WRITEMSG
                 goto ErrExit;
            }
            g_mo_hold.wrk_date = g_pub_tx.tx_date;
            g_mo_hold.trace_no = g_pub_tx.trace_no;
            ret = Mo_hold_Upd_Upd(g_mo_hold,g_pub_tx.reply);
            if(ret)
            {
                 sprintf(acErrMsg," 修改记录出错[%d]",ret);
                 WRITEMSG
                 goto ErrExit;
            }
               Mo_hold_Clo_Upd();
             vtcp_log("%s,%d,Gethere7\n",__FILE__,__LINE__);
         }
             vtcp_log("%s,%d,Gethere7\n",__FILE__,__LINE__);
           double dj_dbal=0.00;
     /**        ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no=%ld ",g_mdm_ac_rel.ac_no);
           if(ret==100)
           {
                sprintf(acErrMsg,"数据库无此记录[%s]",g_pub_tx.reply);
                WRITEMSG
                goto ErrExit;
           }
           if(ret)
           { 
                sprintf(acErrMsg,"查询Mdm_ac_rel出错[%s]",g_pub_tx.reply);
                WRITEMSG
                goto ErrExit;
           }   ** /
           ret=Dd_mst_Dec_Upd(g_pub_tx.reply,"ac_id=%ld  and ac_seqn=%ld",g_mdm_ac_rel.ac_id,ac_seqn);
           if(ret)
           {
                sprintf(acErrMsg,"执行Dd_mst_Dec_Upd出错[%s]",g_pub_tx.reply);
                WRITEMSG
                goto ErrExit;
           }
           ret=Dd_mst_Fet_Upd(&g_dd_mst,g_pub_tx.reply);
           if(ret==100)
           {
               sprintf(acErrMsg,"查询存款主表出错,无记录");
               WRITEMSG
               goto ErrExit;
           }
           if(ret)
           {
              sprintf(acErrMsg,"查询主表程序调用异常[%s]".g_pub_tx.reply);
              WRITEMSG
              goto ErrExit;
           }    
          ** g_dd_mst.bal=g_dd_mst.bal-g_pub_tx.tx_amt1;  **/
           ret=sql_sum_double("mo_hold","plan_hold_amt",&dj_dbal,"hold_sts='0' and ac_id=%ld  and ac_seqn=%ld ",g_pub_tx.ac_id,ac_seqn);
           vtcp_log("[%s][%d] rettttttttt=%d",__FILE__,__LINE__,ret);
           vtcp_log("[%s][%d] dj_dbal=%.2f",__FILE__,__LINE__,dj_dbal);
           if(ret)
           {
              sprintf(acErrMsg,"程序调用异常[%s]",g_pub_tx.reply);
              WRITEMSG
              goto ErrExit;
           }
           vtcp_log("[%s][%d], nanananananana=%.2f ",__FILE__,__LINE__,g_dd_mst.bal);  
           g_dd_mst.hold_amt=dj_dbal;
           vtcp_log("[%s][%d] hold_amt=%d",__FILE__,__LINE__,g_dd_mst.hold_amt);
           vtcp_log("[%s][%d] kouhuakouhuakouhua=%.2f ",__FILE__,__LINE__,g_pub_tx.tx_amt1);  
           ret=sql_min_string("mo_hold","hold_typ",minholdtyp,1,"hold_sts='0' and ac_id=%ld and ac_seqn=%ld",g_pub_tx.ac_id,ac_seqn);    
           vtcp_log("[%s][%d] shishishishi=%d ",__FILE__,__LINE__,ret);
           vtcp_log("[%s][%d] shishishishi='%s' ",__FILE__,__LINE__,minholdtyp); 
           if(ret)
           {
              sprintf(acErrMsg,"程序调用异常[%s]",g_pub_tx.reply);
              WRITEMSG
              goto ErrExit;
           }
           strcpy(g_dd_mst.hold_sts,minholdtyp);
          vtcp_log("[%s][%d] hold_sts='%s' ",__FILE__,__LINE__,g_dd_mst.hold_sts);
          ret=Dd_mst_Upd_Upd(g_dd_mst,g_pub_tx.reply);
          if(ret)
          {
               sprintf(acErrMsg,"Dd_mst_Upd_Upd出错=[%d]",ret);
               WRITEMSG
               goto ErrExit;
          }
         
           Dd_mst_Clo_Upd();
         }
       
         
  }                       /**  到此结束   **/
  else 
  {
        
        /*增加扣划特殊处理 2011/11/13 14:16:53*/
        if ( g_dd_mst.hold_sts[0]=='1' && memcmp(g_pub_tx.tx_code,"4713",4)!= 0)
        {
            sprintf(acErrMsg,"该账户处于完全冻结状态!");
            WRITEMSG
            strcpy(g_pub_tx.reply,"P111");
            goto ErrExit;
        }
        /*增加扣划特殊处理 2011/11/13 14:16:53*/
        if( g_dd_mst.hold_sts[0]=='2' && memcmp(g_pub_tx.tx_code,"4713",4)!= 0)
        {
            sprintf(acErrMsg,"该账户处于只进不出冻结状态!");
            WRITEMSG
            strcpy(g_pub_tx.reply,"P111");
            goto ErrExit;
        }
        /*增加扣划特殊处理 2011/11/13 14:16:53*/
        /**** 修改为用pub_base_CompDblVal函数比较金额  modify at 20120108
        if( g_dd_mst.hold_sts[0]=='3' && g_dd_mst.bal<g_dd_mst.hold_amt && memcmp(g_pub_tx.tx_code,"4713",4)!= 0)
        ****/
        vtcp_log("[%s][%d] g_dd_mst.ac_id =[%d] g_pub_tx.tx_code=[%s]",__FILE__,__LINE__,g_dd_mst.ac_id,g_pub_tx.tx_code);
        if( g_dd_mst.hold_sts[0]=='3' && pub_base_CompDblVal(g_dd_mst.bal,g_dd_mst.hold_amt)<0 && memcmp(g_pub_tx.tx_code,"4713",4)!= 0)
        {
            sprintf(acErrMsg,"该账号已被部分冻结!g_dd_mst.bal===[%.2lf], \
            g_dd_mst.hold_amt======[%.2lf]",g_dd_mst.bal,g_dd_mst.hold_amt);
            WRITEMSG
            strcpy(g_pub_tx.reply,"P112");
            goto ErrExit;
        }
    }
        /**
        if ( g_pub_tx.add_ind[0]=='0' )  
        {
            if( g_dd_mst.bal<g_dd_mst.ctl_amt+g_dd_mst.hold_amt 
                && ( g_dd_mst.ctl_amt>0.005 || g_dd_mst.hold_amt>0.005) )
            {
                sprintf(acErrMsg,"该账号余额低于控制金额!");
                WRITEMSG
                if( pub_base_CompDblVal(g_dd_mst.ctl_amt,0.00) )
                {
                    Remark_ctl_Sel(g_pub_tx.reply,&rem,"ac_id=%d AND rem_sts!='2'",g_dd_mst.ac_id);
                    strcpy( g_pub_tx.reply,"D247");
                    rem.rem[50]='\0';
                    sprintf(msg,"%-50s",rem.rem);
                    set_zd_data("0130",msg);
                    vtcp_log(" MSG=[%s]",msg);
                }
                else strcpy(g_pub_tx.reply,"A030");
                goto ErrExit;
            }
        }
        **/

        if( g_dd_mst.bal<-0.005 && g_dd_mst.od_ind[0]=='Y' )
        {
            /*
            tmpbal=-g_dd_mst.bal;
            ret=pubf_od_proc(g_dd_mst.ac_id,g_dd_mst.ac_seqn,
                g_dd_mst.od_ind,g_pub_tx.add_ind,g_dd_mst.prdt_no,&tmpbal);
            if( ret )
            {
                sprintf(acErrMsg,"透支处理错误");
                WRITEMSG
                goto ErrExit;
            }
            */
            ;
        }

        if( (g_pub_tx.svc_ind==1001 || g_pub_tx.svc_ind==1003) && g_pub_tx.ac_wrk_ind[8]!='1' )
        {
            if( pub_sub_chkadd(g_dd_mst.ys_bal,g_pub_tx.tx_amt1) )
            {
                goto ErrExit;
            }
            
        }
    }
    else if( g_pub_tx.add_ind[0]=='1' )     /*存款*/
    {
        vtcp_log("[%s][%d] g_dd_mst.ac_id =[%d]",__FILE__,__LINE__,g_dd_mst.ac_id);
        /* add begin */
        if( g_dd_mst.ac_sts[0]=='0' )
        {   
            sprintf(acErrMsg,"该账户处于开户待确认状态，请先作开户确认!");
            WRITEMSG
            strcpy(g_pub_tx.reply,"O172");
            goto ErrExit;
        }
        /* add over liuxuan 2006-10-18 */
        if( g_dd_parm.dpst_ind[0]=='N' )
        {
            sprintf(acErrMsg,"不可续存[%s]!",g_dd_parm.dpst_ind);
            WRITEMSG
            strcpy(g_pub_tx.reply,"A009");
            goto ErrExit;
        }
        if( g_dd_parm.thr_dpst_ind[0]=='N' 
            && strcmp(g_pub_tx.tx_br_no,g_pub_tx.opn_br_no))
        {
            sprintf(acErrMsg,"不可通存[%s]!",g_dd_parm.thr_dpst_ind);
            WRITEMSG
            strcpy(g_pub_tx.reply,"A010");
            goto ErrExit;
        }
        if( g_pub_tx.svc_ind!=1006 && g_pub_tx.svc_ind!=1012 )
        {
            if( g_dd_mst.lst_date<g_dd_mst.ic_date && g_dd_mst.ic_date>19000000 )
            {
                vtcp_log("[%s][%d] g_dd_mst.ac_id =[%d]",__FILE__,__LINE__,g_dd_mst.ac_id);
                if( pub_base_CalAcm(g_dd_mst.ic_date,g_pub_tx.tx_date,
                g_dd_parm.intst_term_type,g_dd_parm.acm_calc_type,
                g_dd_mst.bal,g_pub_tx.tx_amt1,&g_dd_mst.intst_acm,
                g_dd_mst.intst_type,g_dd_parm.intst_mon,g_dd_parm.intst_day) )
                {
                    sprintf(acErrMsg,"计算利息积数错误!");
                    WRITEMSG
                    goto ErrExit;
                }
            }
            else
            {
                vtcp_log("[%s][%d] g_dd_mst.ac_id =[%d]",__FILE__,__LINE__,g_dd_mst.ac_id);
                if( pub_base_CalAcm(g_dd_mst.lst_date,g_pub_tx.tx_date,
                g_dd_parm.intst_term_type,g_dd_parm.acm_calc_type,
                g_dd_mst.bal,g_pub_tx.tx_amt1,&g_dd_mst.intst_acm,
                g_dd_mst.intst_type,g_dd_parm.intst_mon,g_dd_parm.intst_day) )
                {
                    sprintf(acErrMsg,"计算利息积数错误!");
                    WRITEMSG
                    goto ErrExit;
                }
            }
        }

        if( g_dd_mst.od_ind[0]=='Y' )
        {
            /*
            ret=pubf_od_proc(g_dd_mst.ac_id,g_dd_mst.ac_seqn,
                g_dd_mst.od_ind,g_pub_tx.add_ind,g_dd_mst.prdt_no,&g_pub_tx.tx_amt1) ;
            if( ret )
            {
                sprintf(acErrMsg,"透支处理错误");
                WRITEMSG
                goto ErrExit;
            }
            */
            ;
        }

        switch( g_pub_tx.svc_ind )
        {
            case 1001:      /*存款*/
                vtcp_log("[%s][%d] g_dd_mst.ac_id =[%d],g_pub_tx.tx_code=[%s]",__FILE__,__LINE__,g_dd_mst.ac_id,g_pub_tx.tx_code);
                /* 按财务部要求 产品转换可以过 wudawei 20131010
                if( g_dd_mst.ac_type[0]=='6' )
                end 20131010*/ 
                if( g_dd_mst.ac_type[0]=='6' && memcmp(g_pub_tx.tx_code,"3319",4) != 0)
                {
                    sprintf(acErrMsg,"此户为协定或理财账号，不能直接存款!");
                    WRITEMSG
                    strcpy(g_pub_tx.reply,"P293");
                    goto ErrExit;
                }
            case 1005:
            case 1008:      /*存款*/
                vtcp_log("[%s][%d] g_dd_mst.ac_id =[%d]",__FILE__,__LINE__,g_dd_mst.ac_id);
                g_dd_mst.bal=g_dd_mst.bal+g_pub_tx.tx_amt1;
                g_dd_mst.acbo_bal=g_dd_mst.acbo_bal+g_pub_tx.tx_amt1;
                break;
            case 1006:      /*预授权*/
                g_dd_mst.ctl_amt=g_dd_mst.ctl_amt-g_pub_tx.tx_amt1;
                vtcp_log("[%s][%d] g_dd_mst.ac_id =[%d]",__FILE__,__LINE__,g_dd_mst.ac_id);
                if( pub_base_CompDblVal(g_dd_mst.ctl_amt,0.00)<0 )
                {
                    sprintf(acErrMsg,"预授权完成金额错[%.2lf]<[%.2lf]!",
                            g_dd_mst.ctl_amt,g_pub_tx.tx_amt1);
                    WRITEMSG
                    strcpy( g_pub_tx.reply,"A031" );
                    goto ErrExit;
                }
                break;
            case 1007: /*BEGIN:add by wyw 20080319 预授权完成撤销时用 add by hxc 091117*/
            /*撤销时增加*/
            vtcp_log("[%s][%d] g_dd_mst.ac_id =[%d]",__FILE__,__LINE__,g_dd_mst.ac_id);
            vtcp_log("%s,%d,g_dd_mst.ctl_amt=[%.2f],g_pub_tx.tx_amt2=[%.2f]",__FILE__,__LINE__,g_dd_mst.ctl_amt,g_pub_tx.tx_amt2);
            g_dd_mst.ctl_amt = g_dd_mst.ctl_amt + g_pub_tx.tx_amt2;
            vtcp_log("%s,%d,g_dd_mst.ctl_amt=[%.2f],g_pub_tx.tx_amt2=[%.2f]",__FILE__,__LINE__,g_dd_mst.ctl_amt,g_pub_tx.tx_amt2);
            g_dd_mst.bal=g_dd_mst.bal+g_pub_tx.tx_amt1;
            g_dd_mst.acbo_bal=g_dd_mst.acbo_bal+g_pub_tx.tx_amt1;
            break;
            /*END:add by wyw 20080319 预授权完成撤销时用 add by hxc 091117*/
            default :
                sprintf( acErrMsg,"操作类型[%d]视同存款",g_pub_tx.svc_ind );
                WRITEMSG
                g_dd_mst.bal=g_dd_mst.bal+g_pub_tx.tx_amt1;
                g_dd_mst.acbo_bal=g_dd_mst.acbo_bal+g_pub_tx.tx_amt1;
                break;
        }
        /*** 调用算利息积数的函数 ***/

        vtcp_log("[%s][%d] g_dd_mst.ac_id =[%d]",__FILE__,__LINE__,g_dd_mst.ac_id);
        if( g_pub_tx.hst_ind[0]=='1' )      /*日间入明细*/
        {
            g_dd_mst.hst_bal=g_dd_mst.hst_bal+g_pub_tx.tx_amt1;
        }
    }
    else
    {
        sprintf(acErrMsg,"存取标志错误g_pub_tx.add_ind=[%s]!",g_pub_tx.add_ind);
        WRITEMSG
        strcpy(g_pub_tx.reply,"P106");
        goto ErrExit;
    }
    vtcp_log("------look------[%s]",g_mdm_ac_rel.ac_no);

    g_dd_mst.lst_date=g_pub_tx.tx_date;

    if( g_pub_tx.ac_wrk_ind[7]=='J' )
        ;
    else if( g_dd_mst.od_ind[0]=='Y' )
    {
        ;
    }
    else if( 
        pub_base_CompDblVal(g_dd_mst.bal-g_dd_mst.ctl_amt,g_dd_parm.min_bal)<0 
        && g_dd_mst.ac_sts[0]=='1' && g_dd_mst.ac_type[0]!='6' )
    {
        vtcp_log("[%s][%d] 帐号=[%s] 帐户ID=[%d] ",__FILE__,__LINE__,g_pub_tx.ac_no,g_pub_tx.ac_id);
        vtcp_log("[%s][%d] g_dd_mst.ac_id =[%d]",__FILE__,__LINE__,g_dd_mst.ac_id);
        if ( g_pub_tx.add_ind[0]=='0' )  /* 取款处理判断 YUDONG 20030901*/
        {
            
             vtcp_log("[%s][%d] 帐号=[%s] 帐户ID=[%d] ",__FILE__,__LINE__,g_pub_tx.ac_no,g_pub_tx.ac_id);
             vtcp_log("%s,%d ctl_amt=%lf",__FILE__,__LINE__,g_dd_mst.ctl_amt);
             vtcp_log("[%s][%d] g_dd_mst.ac_id =[%d]",__FILE__,__LINE__,g_dd_mst.ac_id);
             vtcp_log("%s,%d ac_id=%ld",__FILE__,__LINE__,g_pub_tx.ac_id);
             /**add by zyl 20110413 七天乐日常记账**/
             /**0.变量定义**/
             char cAc_no_tmp[25];       /**用于 3.保存当前g_pub_tx数据. **/
             long lAc_id_tmp = 0;       /**用于 3.保存当前g_pub_tx数据. **/
             int  iAc_seqn_tmp = 0;     /**用于 3.保存当前g_pub_tx数据. **/
             double dTx_amt_tmp = 0.0;  /**用于 3.保存当前g_pub_tx数据. **/
             char cAdd_ind_tmp[2];      /**用于 3.保存当前g_pub_tx数据. **/
             char cHst_ind_tmp[2];      /**用于 3.保存当前g_pub_tx数据. **/
             char cAc_wrk_ind_tmp[11];  /**用于 3.保存当前g_pub_tx数据. **/
             int  iSvc_ind_tmp = 0;     /**用于 3.保存当前g_pub_tx数据. **/
             char cCt_ind_tmp[2];       /**用于 3.保存当前g_pub_tx数据. **/
             char cAc_id_type_tmp[2];   /**用于 3.保存当前g_pub_tx数据. **/
             char cNote_type_tmp[4];    /**用于 3.保存当前g_pub_tx数据. **/
             char cBeg_note_no_tmp[17]; /**用于 3.保存当前g_pub_tx数据. **/
             double b=0.0 ;
             struct mo_prdt_prot_c sMo_prdt_prot;
             struct mo_prot_ac_rel_c sMo_prot_ac_rel;
             struct dd_mst_c sDd_mst_virtual;
             struct dd_parm_c sDd_parm_virtual;/*add by slg 2013-02-05 11:02:16*/
             struct dd_mst_c sMain_dd_mst;
             struct dd_parm_c sDd_parm_main;   /*add by slg 2013-02-05 11:02:16*/
             struct S_pub_intst   sum_intst;
             memset(cAc_no_tmp,0x00,sizeof(cAc_no_tmp));
             memset(cAdd_ind_tmp,0x00,sizeof(cAdd_ind_tmp));
             memset(cHst_ind_tmp,0x00,sizeof(cHst_ind_tmp));
             memset(cAc_wrk_ind_tmp,0x00,sizeof(cAc_wrk_ind_tmp));
             memset(cCt_ind_tmp,0x00,sizeof(cCt_ind_tmp));
             memset(cAc_id_type_tmp,0x00,sizeof(cAc_id_type_tmp));
             memset(cNote_type_tmp,0x00,sizeof(cNote_type_tmp));
             memset(cBeg_note_no_tmp,0x00,sizeof(cBeg_note_no_tmp));
             
             memset(&sMain_dd_mst,0x00,sizeof(struct dd_mst_c));
             memset(&sDd_mst_virtual,0x00,sizeof(struct dd_mst_c));
             memset(&sDd_parm_virtual,0x00,sizeof(struct dd_parm_c));
             memset(&sDd_parm_main,0x00,sizeof(struct dd_parm_c));
             memset(&sMo_prdt_prot,0x00,sizeof(struct mo_prdt_prot_c));
             memset(&sMo_prot_ac_rel,0x00,sizeof(struct mo_prot_ac_rel_c));
             
             /**1.检查.是否是七天乐账户.**/
             vtcp_log("[%s][%d] 帐号=[%s] 帐户ID=[%d] ",__FILE__,__LINE__,g_pub_tx.ac_no,g_pub_tx.ac_id);
             ret = Mo_prdt_prot_Sel(g_pub_tx.reply,&sMo_prdt_prot," prot_type='TK7' and sts!='*' and main_ac_id = %ld ",g_pub_tx.ac_id);
             if(ret == 100)
             {
                 vtcp_log("[%s][%d] 帐号=[%s] 帐户ID=[%d] ",__FILE__,__LINE__,g_pub_tx.ac_no,g_pub_tx.ac_id);
                 /**无七天乐签约.继续原程序.**/
             }
             else if(ret)
             {
                 sprintf(acErrMsg,"Mo_prdt_prot_Sel error ret=[%d]",ret);
                 WRITEMSG
                 strcpy( g_pub_tx.reply,"D103");
                 goto ErrExit;
             }
             else
             {
                 vtcp_log("[%s][%d]主账户 zyl 20110415 [%17.2f]",__FILE__,__LINE__,g_dd_mst.bal);
                 vtcp_log("[%s][%d] g_dd_mst.ac_id =[%d]",__FILE__,__LINE__,g_dd_mst.ac_id);

                 vtcp_log("[%s][%d] 帐号=[%s] 帐户ID=[%d] ",__FILE__,__LINE__,g_pub_tx.ac_no,g_pub_tx.ac_id);
                 ret = Mo_prot_ac_rel_Sel(g_pub_tx.reply,&sMo_prot_ac_rel," prot_type='TK7' and sts!='*' and main_ac_id = %ld ",g_pub_tx.ac_id);
                 if(ret == 100)
                 {
                   /**七天乐签约信息存在.与虚拟账户对照关系不存在.**/
                     sprintf(acErrMsg,"七天乐签约信息存在.与虚拟账户对照关系不存在.");
                     WRITEMSG
                     strcpy( g_pub_tx.reply,"D104");
                     goto ErrExit;
                 }
                 else if(ret)
                 {
                     sprintf(acErrMsg,"Mo_prot_ac_rel_Sel error ret=[%d]",ret);
                     WRITEMSG
                     strcpy( g_pub_tx.reply,"D103");
                     goto ErrExit;
                 }
                 
                 /**2.七天乐账户信息..签约账户,虚拟账户信息已取得.**/
                 ret = Dd_mst_Sel(g_pub_tx.reply,&sDd_mst_virtual," ac_id = %ld ",sMo_prot_ac_rel.app_ac_id);
                 if(ret == 100)
                 {
                     sprintf(acErrMsg,"dd_mst中无虚拟账户信息.");
                     WRITEMSG
                     strcpy( g_pub_tx.reply,"D104");
                     goto ErrExit;
                 }
                 else if(ret)
                 {
                     sprintf(acErrMsg,"Dd_mst_Sel error ret=[%d]",ret);
                     WRITEMSG
                     strcpy( g_pub_tx.reply,"D103");
                     goto ErrExit;
                 }
                 /*BEGIN ---add  by slg 2013-02-02 */
                 vtcp_log("[%s][%d]虚拟账户余额=[%17.2f]",__LINE__,__FILE__,sDd_mst_virtual.bal);
                 vtcp_log("[%s][%d] g_dd_mst.ac_id =[%d]",__FILE__,__LINE__,g_dd_mst.ac_id);
                 vtcp_log("[%s][%d] g_pub_tx.tx_amt1 =[%17.2f]",__FILE__,__LINE__,g_pub_tx.tx_amt1);
                 vtcp_log("[%s][%d] g_dd_mst.bal =%17.2f]",__FILE__,__LINE__,g_dd_mst.bal);
                 vtcp_log("[%s][%d] g_dd_mst.ctl_amt =[%17.2f]",__FILE__,__LINE__,g_dd_mst.ctl_amt);
                 vtcp_log("[%s][%d] g_dd_parm.min_bal =[%d]",__FILE__,__LINE__,g_dd_parm.min_bal);
                 vtcp_log("[%s][%d] aaaaaaaaaam.min_bal =[%17.2f]",__FILE__,__LINE__,g_dd_mst.bal+sDd_mst_virtual.bal+g_pub_tx.tx_amt1-g_dd_mst.ctl_amt-g_dd_parm.min_bal);
                 if (pub_base_CompDblVal(g_pub_tx.tx_amt1,
                    g_dd_mst.bal+sDd_mst_virtual.bal+g_pub_tx.tx_amt1-g_dd_mst.ctl_amt-g_dd_parm.min_bal) > 0)/*by wjl 20130311 判断虚拟账号与主账号之和*/
                 {   
                     sprintf(acErrMsg,"取款金额大于主账户余额和七天乐余额之和!!!");
                     WRITEMSG
                     strcpy(g_pub_tx.reply,"O214");
                     goto ErrExit;
                 }
                 
                 vtcp_log("[%s][%d] 帐号=[%s] 帐户ID=[%d] ",__FILE__,__LINE__,g_pub_tx.ac_no,g_pub_tx.ac_id);
                 ret = Dd_parm_Sel(g_pub_tx.reply, &sDd_parm_virtual, "prdt_no='%s'",sDd_mst_virtual.prdt_no);
                 if (ret)
                 {
                     sprintf(acErrMsg, "查询活期产品表错误ret=[%d]虚拟prdt_no=[%s]!!!",ret,sDd_mst_virtual.prdt_no);
                     WRITEMSG
                     strcpy(g_pub_tx.reply, "W015");
                     goto ErrExit;
                 }
                
                 /*计算虚拟帐户的利息*/
                 vtcp_log("[%s][%d] 帐号=[%s] 帐户ID=[%d] ",__FILE__,__LINE__,g_pub_tx.ac_no,g_pub_tx.ac_id);
                 ret=app_cal_intst(sMo_prdt_prot.sts,sDd_mst_virtual,sDd_parm_virtual);
                 if (ret)
                 {
                     sprintf(acErrMsg, "计算虚拟帐户利息错误ret=[%d]!!!", ret);
                     WRITEMSG
                     goto ErrExit;
                 }
                 sum_intst.dealval=g_pub_intst.dealval;/*应付利息*/
                 sum_intst.val=g_pub_intst.val;/*利息税*/
                 
                 vtcp_log("[%s][%d]主账户产品号是[%s]",__FILE__,__LINE__,g_dd_mst.prdt_no);
                 /*查询活期产品表*/
                 vtcp_log("[%s][%d] g_dd_mst.ac_id =[%d]",__FILE__,__LINE__,g_dd_mst.ac_id);
                 memset(&sDd_parm_main ,0x00,sizeof(struct dd_parm_c));
                 ret = Dd_parm_Sel(g_pub_tx.reply, &sDd_parm_main, "prdt_no='%s'",g_dd_mst.prdt_no);
                 if (ret)
                 {
                     sprintf(acErrMsg, "查询活期产品表错误ret=[%d]prdt_no=[%s]!!!",ret,g_dd_mst.prdt_no);
                     WRITEMSG
                     goto ErrExit;
                 }
                 
                  /**3.保存当前g_pub_tx数据.**/
                 strcpy(cAc_no_tmp,g_pub_tx.ac_no);
                 
                 lAc_id_tmp = g_pub_tx.ac_id;
                 iAc_seqn_tmp = g_pub_tx.ac_seqn;
                 dTx_amt_tmp = g_pub_tx.tx_amt1;
                 strcpy(cAdd_ind_tmp,g_pub_tx.add_ind);
                 strcpy(cHst_ind_tmp,g_pub_tx.hst_ind);
                 strcpy(cAc_wrk_ind_tmp,g_pub_tx.ac_wrk_ind);
                 iSvc_ind_tmp = g_pub_tx.svc_ind;
                 strcpy(cCt_ind_tmp,g_pub_tx.ct_ind);
                 strcpy(cAc_id_type_tmp,g_pub_tx.ac_id_type);
                 strcpy(cNote_type_tmp,g_pub_tx.note_type);
                 strcpy(cBeg_note_no_tmp,g_pub_tx.beg_note_no);
                 
                 vtcp_log("[%s][%d] 帐号=[%s] 帐户ID=[%d] ",__FILE__,__LINE__,g_pub_tx.ac_no,g_pub_tx.ac_id);
                 vtcp_log("[%s][%d] g_dd_mst.ac_id =[%d]",__FILE__,__LINE__,g_dd_mst.ac_id);
                 
                 /****把g_pub_tx.ac_no赋值为空****/
                 strcpy(g_pub_tx.ac_no,"");
                 
                 ret=main_intst_acct(sDd_mst_virtual,sDd_parm_virtual,sum_intst);
                 if (ret)
                 {
                     sprintf(acErrMsg, "主账号记账处理失败ret=[%d]!",ret);
                     WRITEMSG
                     goto ErrExit;
                 }
                 /*END ---add  by  slg   2013-02-02 09:39:50*/
                 
                 /**4.准备递归.pub_acct_trance()数据.**/
                 /**5.pub_acct_trance().**/
                 /**a 虚拟账户划款**/
                 strcpy(g_pub_tx.ac_no,"");
                 g_pub_tx.ac_id=sMo_prot_ac_rel.app_ac_id;
                 g_pub_tx.ac_seqn=1;
                 /****这里应增加利息到余额中 modify at 20130219
                 g_pub_tx.tx_amt1=sDd_mst_virtual.bal;
                 modify at next line****/
                 g_pub_tx.tx_amt1=sDd_mst_virtual.bal+g_pub_intst.dealval+g_pub_intst.keepval-g_pub_intst.val;
                 /****end 20130218***/
                 strcpy(g_pub_tx.add_ind,"0"); 
                 strcpy(g_pub_tx.hst_ind,"1"); /**日间入明细**/
                 strcpy(g_pub_tx.ac_wrk_ind,"0000010011");
                 g_pub_tx.svc_ind=1001; /**处理代码***/
                 strcpy(g_pub_tx.ct_ind,"2");  /*** 转账 ***/
                 strcpy(g_pub_tx.ac_id_type,"");
                 strcpy(g_pub_tx.note_type,"");
                 strcpy(g_pub_tx.beg_note_no,"");
                 
                 if( pub_acct_trance() )
                 {
                   sprintf(acErrMsg,"调用存取款主控失败!");
                   WRITEMSG
                   return 1;
                 }
                 set_zd_data("102K",g_pub_tx.ct_ind);
                 /****改用100域第1子域  modify at 20130218
                 set_zd_double("102I",sDd_mst_virtual.bal);
                 ****/
                 set_zd_double("1002",sDd_mst_virtual.bal);
                 vtcp_log("%s,%dddddddd=[%f]",__FILE__,__LINE__,sDd_mst_virtual.bal);
                 if ( pubf_acct_proc("LC03") )
                 {
                   sprintf(acErrMsg,"登记会计流水错误!");
                   WRITEMSG
                   return 1;
                 }
                 /**b 签约账户存款 虚拟户.所有金额.**/
                 strcpy(g_pub_tx.ac_no,"");
                 g_pub_tx.ac_id=sMo_prdt_prot.main_ac_id;
                 g_pub_tx.ac_seqn=1;
                 /****这里应增加利息到余额中 modify at 20130219
                 g_pub_tx.tx_amt1=sDd_mst_virtual.bal;
                 modify at next line****/
                 g_pub_tx.tx_amt1=sDd_mst_virtual.bal+g_pub_intst.dealval+g_pub_intst.keepval-g_pub_intst.val;
                 vtcp_log("zyl 201104277 see bal=[%17.2f]",sDd_mst_virtual.bal);
                 strcpy(g_pub_tx.add_ind,"1"); 
                 strcpy(g_pub_tx.hst_ind,"1"); /**日间入明细**/
                 strcpy(g_pub_tx.ac_wrk_ind,"0000010011");
                 g_pub_tx.svc_ind=1001; /**处理代码***/
                 strcpy(g_pub_tx.ct_ind,"2");  /*** 转账 ***/
                 strcpy(g_pub_tx.ac_id_type,"");
                 strcpy(g_pub_tx.note_type,"");
                 strcpy(g_pub_tx.beg_note_no,"");
                 
                 vtcp_log("[%s][%d] 帐号=[%s] 帐户ID=[%d] ",__FILE__,__LINE__,g_pub_tx.ac_no,g_pub_tx.ac_id);
                 if( pub_acct_trance() )
                 {
                   sprintf(acErrMsg,"调用存取款主控失败!");
                   WRITEMSG
                   return 1;
                 }
                 set_zd_data("102K",g_pub_tx.ct_ind);
                 /****将划转额和利息归入主帐户,改用100域第1子域  modify at 20130218****/
                 set_zd_double("1002",sDd_mst_virtual.bal+g_pub_intst.dealval+g_pub_intst.keepval-g_pub_intst.val);
                  vtcp_log("wjllll see bal=[%17.2f]",sDd_mst_virtual.bal);
                 vtcp_log("wjllll see bal=[%17.2f]",sDd_mst_virtual.bal+g_pub_intst.dealval+g_pub_intst.keepval-g_pub_intst.val);
                 /****end 20130218***/
                 if ( pubf_acct_proc("LC02") )
                 {
                   sprintf(acErrMsg,"登记会计流水错误!");
                   WRITEMSG
                   return 1;
                 }
                 vtcp_log("[%s][%d] zyl 20110415 [%17.2f]",__FILE__,__LINE__,g_dd_mst.bal);
                 /**6.还原递归前g_pub_tx信息.**/
                 strcpy(g_pub_tx.ac_no,cAc_no_tmp);
                 g_pub_tx.ac_id = lAc_id_tmp;
                 g_pub_tx.ac_seqn = iAc_seqn_tmp;
                 g_pub_tx.tx_amt1 = dTx_amt_tmp;
                 strcpy(g_pub_tx.add_ind,cAdd_ind_tmp);
                 strcpy(g_pub_tx.hst_ind,cHst_ind_tmp);
                 strcpy(g_pub_tx.ac_wrk_ind,cAc_wrk_ind_tmp);
                 g_pub_tx.svc_ind = iSvc_ind_tmp;
                 strcpy(g_pub_tx.ct_ind,cCt_ind_tmp);
                 strcpy(g_pub_tx.ac_id_type,cAc_id_type_tmp);
                 strcpy(g_pub_tx.note_type,cNote_type_tmp);
                 strcpy(g_pub_tx.beg_note_no,cBeg_note_no_tmp);
                 
                 set_zd_data("102K",g_pub_tx.ct_ind);/**程序流程..**/
                 
                 /****在此处减去D003的交易额  20130222****/
                 g_dd_mst.hst_bal=g_dd_mst.hst_bal-g_pub_tx.tx_amt1;
                 /****将利息值清空，不让D003接收到 add at 20130218****/
                 g_pub_intst.dealval = 0.00;
                 g_pub_intst.keepval = 0.00;
                 g_pub_intst.val     = 0.00;
                 /****end 20130218****/
                 
                 /**7.七天乐日常记账.其他处理.虚拟账户状态.签约对照记录状态.**/
                 memset(&sMo_prdt_prot,0x00,sizeof(struct mo_prdt_prot_c));
                 memset(&sMo_prot_ac_rel,0x00,sizeof(struct mo_prot_ac_rel_c));
                 ret = Mo_prdt_prot_Dec_Upd(g_pub_tx.reply," prot_type='TK7' and sts!='*' and main_ac_id = %ld ",g_pub_tx.ac_id);
                 if(ret)
                 {
                       sprintf(acErrMsg, "定义游标错误!");
                       WRITEMSG
                       strcpy(g_pub_tx.reply, "P162");
                       goto ErrExit;
                 }
                 ret = Mo_prdt_prot_Fet_Upd(&sMo_prdt_prot,g_pub_tx.reply);
                 if(ret == 100)
                 {
                     sprintf(acErrMsg,"无记录..");
                     WRITEMSG
                     strcpy( g_pub_tx.reply,"D104");
                     goto ErrExit;
                 }
                 else if(ret)
                 {
                     sprintf(acErrMsg,"Mo_prdt_prot_Fet_Upd error ret=[%d]",ret);
                     WRITEMSG
                     strcpy( g_pub_tx.reply,"D103");
                     goto ErrExit;
                 }
                 sMo_prdt_prot.sts[0] = '2';/**临时解约.**/
                 ret = Mo_prdt_prot_Upd_Upd(sMo_prdt_prot,g_pub_tx.reply);
                 if(ret)
                 {
                    sprintf(acErrMsg, "修改记录错误!");
                    WRITEMSG
                    strcpy(g_pub_tx.reply, "P162");
                    goto ErrExit;
                 }
                 Mo_prdt_prot_Clo_Upd();
                 vtcp_log("[%s][%d] ssssssBrf =[%s]",__FILE__,__LINE__,Brf);
                 strcpy(g_pub_tx.brf, Brf);/** 20140325**/
                 vtcp_log("[%s][%d] g_pub_tx.brf =[%s]",__FILE__,__LINE__,g_pub_tx.brf);
                 vtcp_log("[%s][%d] 帐号=[%s] 帐户ID=[%d] ",__FILE__,__LINE__,g_pub_tx.ac_no,g_pub_tx.ac_id);
                 /**8.继续当前.trance处理.**/
                 vtcp_log("[%s][%d] zyl 20110415 [%17.2f]",__FILE__,__LINE__,g_dd_mst.bal);
                 /**a.余额变化.当前程序的余额变化代码.未更改到数据库.签约账户存款时 trance()中.余额更改到数据库.**/
                 sprintf(acErrMsg,"!!![%s][%d] 进行余额变化ret=[%d]ac_no=[%s]",__FILE__,__LINE__,ret,g_pub_tx.ac_no);
                 WRITEMSG
                 vtcp_log("看看交易金额1=[%lf] bal=[%lf] ac_id=[%ld] ac_seqn=[%d]",g_pub_tx.tx_amt1,g_dd_mst.bal,g_dd_mst.ac_id,g_dd_mst.ac_seqn);
                 g_dd_mst.bal=g_dd_mst.bal-g_pub_tx.tx_amt1;
                     vtcp_log("看看交易金额2=[%lf]",g_dd_mst.bal);
                 g_dd_mst.acbo_bal=g_dd_mst.acbo_bal-g_pub_tx.tx_amt1;     
                 sprintf(acErrMsg,"[%s][%d]svc_ind=[%d]",__FILE__,__LINE__,g_pub_tx.svc_ind);
                 WRITEMSG
                 /*** 调用算利息积数的函数 ***/

             }
            if(pub_base_CompDblVal(g_dd_mst.bal-g_dd_mst.ctl_amt,g_dd_parm.min_bal)<0 && g_dd_mst.ac_sts[0]=='1' && g_dd_mst.ac_type[0]!='6')
             { 
                if( pub_base_CompDblVal(g_dd_mst.ctl_amt,0.00) )
                {
                    Remark_ctl_Sel(g_pub_tx.reply,&rem,"ac_id=%d AND rem_sts!='2'",g_dd_mst.ac_id);
                    strcpy( g_pub_tx.reply,"D247");
                    rem.rem[50]='\0';
                    sprintf(msg,"%-50s",rem.rem);
                    set_zd_data("0130",msg);
                    vtcp_log(" MSG=[%s]",msg);
                    goto ErrExit;
                }
                sprintf(acErrMsg,"存款余额[%.2lf]低于下限[%.2lf]",g_dd_mst.bal,g_dd_parm.min_bal );
                WRITEMSG
                strcpy(g_pub_tx.reply,"A022" );
                goto ErrExit;
             }
        }
    }
    else if( g_pub_tx.add_ind[0]=='0' )
    {
        if( pub_base_CompDblVal(g_dd_mst.bal,g_dd_mst.ctl_amt)<0 )
        {
            if( pub_base_CompDblVal(g_dd_mst.ctl_amt,0.00) )
            {
                if( Remark_ctl_Sel(g_pub_tx.reply,&rem,
                    "ac_id=%d AND rem_sts!='2'", g_dd_mst.ac_id) )
                {
                    strcpy( rem.rem,"" );
                }
                pub_base_strpack( rem.rem );
                rem.rem[50]='\0';

                strcpy( g_pub_tx.reply,"D247");
                sprintf(msg,"%-50s",rem.rem);
                set_zd_data("0130",msg);
                vtcp_log(" MSG=[%s]",msg);
                goto ErrExit;
            }
        }
    }
vtcp_log("XXXXXXX[%.2lf]",g_dd_mst.bal);
    /******对于渠道过来的业务极端情况下判断不准确   by jk 20110515
    if( g_dd_mst.bal<-0.005 && g_pub_tx.add_ind[0]!='1' )
    ****/
    if( g_dd_mst.bal<-0.005 && g_dd_mst.od_ind[0]!='Y')
    {
        strcpy(g_pub_tx.reply,"A022" );
        goto ErrExit;
    }

    if( pub_base_CompDblVal(g_dd_mst.bal,g_dd_parm.max_bal)>0 
        && pub_base_CompDblVal(g_dd_parm.max_bal,0.00) 
        && g_dd_mst.ac_type[0]!='6' )
    {
        sprintf(acErrMsg,"存款余额[%.2lf]高于上限[%.2lf]",
                g_dd_mst.bal,g_dd_parm.max_bal );
        WRITEMSG
        strcpy(g_pub_tx.reply,"A008" );
        goto ErrExit;
    }

    if( g_pub_tx.svc_ind==1003 )        /*销户*/
    {
        /**利息存入**/
        /**利息税支取**/
        /**更改支取金额**/

        /* add by xxx at 20030726 ***/
        if( g_pub_tx.hst_ind[0]=='1' )      /*日间入明细*/
        {
            ret=pub_ins_dd_hst();
            ERR_DEAL
        }
    }
    else if( g_pub_tx.hst_ind[0]=='1' )     /*日间入明细*/
    {
        ret=pub_ins_dd_hst();
        ERR_DEAL
    }
    if(pub_base_CompDblVal(g_pub_tx.tx_amt1,0.00) != 0)  
    {
      vtcp_log("%s,%d,零金额不发短信通知[%.2f]\n",__FILE__,__LINE__,g_pub_tx.tx_amt1);
      pub_mob_sendmail( g_pub_tx.brf, g_pub_tx.tx_code,g_mdm_ac_rel.ac_no,g_pub_tx.add_ind,g_pub_tx.tx_amt1, g_dd_mst.bal );
		}
		
    vtcp_log("------look------[%s]",g_mdm_ac_rel.ac_no);
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
    
    sprintf(acErrMsg,"===chenming:  g_pub_tx.tx_amt1=[%.2lf]  g_dd_mst.bal=[%.2lf] ===",g_pub_tx.tx_amt1,g_dd_mst.bal);
        WRITEMSG 
    ret=Dd_mst_Upd_Upd(g_dd_mst,g_pub_tx.reply);
    if( ret )
    {
        sprintf(acErrMsg,"账号不存在[%d][%d]",g_pub_tx.ac_id,ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"P102");
        goto ErrExit;
    }
    Dd_mst_Clo_Upd();

    ret=pub_acct_ac_rec( g_dd_mst.ac_id,g_dd_mst.ac_seqn,
        g_pub_tx.add_ind,g_pub_tx.ct_ind,g_pub_tx.tx_code,g_pub_tx.tx_amt1,"0000" );
    if( ret ) goto ErrExit;

OkExit:
    return 0;
ErrExit:
    return 1;
}

int Chg_td_mst()
{
    struct mdm_ac_rel_c o_mdm_ac_rel;
    int ret;
    long l_date;
    long num;
    char msg[51];
    char kh_type[2];
    struct td_mst_c TmpTd_mst;
    struct  remark_ctl_c rem;
    struct  mo_hold_c  g_mo_hold;
    char  minholdtyp[2];
  long   hold_seqn;
  long   ac_seqn;
  memset(minholdtyp,0,sizeof(minholdtyp));
    memset(&g_mo_hold,0x00,sizeof(struct mo_hold_c));


    /*** 调用定期产品参数各项检查函数 ***/
    partflag=0;
    wdr_flag=0;

    /*** 定期主文件检查 ***/
    ret=Td_mst_Sel( g_pub_tx.reply,&g_td_mst,"ac_id='%ld' and ac_seqn='%d' ", \
                    g_pub_tx.ac_id,g_pub_tx.ac_seqn);
    if( ret )
    {
        sprintf(acErrMsg,"账号不存在[%d][%d]",g_pub_tx.ac_id,ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"P102");
        goto ErrExit;
    }
    memcpy( &o_td_mst,&g_td_mst,sizeof(g_td_mst) );
    vtcp_log("[%s][%d] partflag = %d\n",__FILE__,__LINE__,partflag);
    vtcp_log("[%s][%d] tx_amt1  = %.2f tx_amt2 =%.2f\n",__FILE__,__LINE__,g_pub_tx.tx_amt1,g_pub_tx.tx_amt2);

sprintf(acErrMsg, "8888888888888************* ic_date=[%ld]", g_td_mst.ic_date);
WRITEMSG
    vtcp_log("[%s][%d] *********hst_cnt =[%d]\n",__FILE__,__LINE__,g_td_mst.hst_cnt);
    if( g_td_mst.ac_sts[0]=='*' )
    {
        sprintf(acErrMsg,"账户已销!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"P245");
        goto ErrExit;
    }

    if( g_td_mst.ac_sts[0]!='1' && g_td_mst.ac_sts[0] != '#')
    {
        sprintf(acErrMsg,"账号状态非正常状态![%s]",g_td_mst.ac_sts);
        WRITEMSG
        strcpy(g_pub_tx.reply,"P109");
        goto ErrExit;
    }
    /**  add by  shiweina  20071217  **/
  if(strcmp(g_pub_tx.tx_code,"2430")==0)
  {
    sprintf(acErrMsg,"[%s][%d]tx_code=%ld",__FILE__,__LINE__,g_pub_tx.tx_code);
   

  }                /**  到此结束   **/
  else
  {
    /*增加扣划特殊处理 2011/11/13 14:25:13*/
    if( g_td_mst.hold_sts[0]=='1' && memcmp(g_pub_tx.tx_code,"4713",4)!=0)
    {
        sprintf(acErrMsg,"该账号已被完全冻结!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"P110");
        goto ErrExit;
    }
  }
    /* 给定期产品结构赋值 */
    ret=Td_parm_Sel(g_pub_tx.reply,&g_td_parm,"prdt_no='%s'",g_td_mst.prdt_no);
    ERR_DEAL  

    strcpy(g_pub_tx.cur_no,g_td_parm.cur_no);
    strcpy(g_pub_tx.dc_code,g_td_parm.dc_code);
    strcpy(g_pub_tx.prdt_code,g_td_mst.prdt_no);

    strcpy(g_pub_tx.name,g_td_mst.name);
    strcpy(g_pub_tx.opn_br_no,g_td_mst.opn_br_no);
    g_pub_tx.cif_no=g_td_mst.cif_no;

    /***** add by xxx **/
    g_pub_tx.mach_date = g_td_mst.mtr_date;
    vtcp_log("[%s][%d] partflag = %d\n",__FILE__,__LINE__,partflag);
    vtcp_log("[%s][%d] tx_amt1  = %.2f tx_amt2 =%.2f\n",__FILE__,__LINE__,g_pub_tx.tx_amt1,g_pub_tx.tx_amt2);

    /*取款、销户、协定销户*/
    if( g_pub_tx.add_ind[0]=='0' )
    {
        if (g_pub_tx.ct_ind[0]=='1' && g_td_parm.cash_ind[0] == 'N')
        {
            sprintf(acErrMsg,"该产品不允许提现!!!");
            WRITEMSG
            strcpy(g_pub_tx.reply,"P223");
            goto ErrExit;
        }

        if( g_pub_tx.ct_ind[0]=='1' && g_td_mst.odtt_ind[0]=='1' )
        {
            sprintf(acErrMsg,"汇户不允许取现!");
            WRITEMSG
            strcpy(g_pub_tx.reply,"P108");
            goto ErrExit;
        }

        /**
        if( g_td_parm.thr_draw_ind[0]=='N'
              && strcmp(g_pub_tx.tx_br_no,g_pub_tx.opn_br_no) )
        {
            sprintf(acErrMsg,"不可通兑[%s]!",g_td_parm.thr_draw_ind);
            WRITEMSG
            strcpy(g_pub_tx.reply,"A011");
            goto ErrExit;
        }
        **/

        /*** 调用校验支取方式、校验密码函数 ***/
        /*增加扣划特殊处理  扣划不校验密码2011/11/13 15:35:51*/
        if(memcmp(g_pub_tx.tx_code,"4713",4) != 0)
        {
            ret = pub_base_check_draw("001");
            ERR_DEAL
        }
        
        /**   add  by shiweina  20071217  **/
        if(strcmp(g_pub_tx.tx_code,"2430")==0)
        {
             sprintf(acErrMsg,"[%s][%d]tx_code=%ld",__FILE__,__LINE__,g_pub_tx.tx_code);
          }      /**   到此结束  **/
          else 
        {
                /*增加扣划特殊处理  2011/11/13 14:26:11*/
              if( g_td_mst.hold_sts[0]=='2' && memcmp(g_pub_tx.tx_code,"4713",4)!=0 )
              {
                sprintf(acErrMsg,"该账号已被只进不出冻结!");
                WRITEMSG
                strcpy(g_pub_tx.reply,"P111");
                goto ErrExit;
              }
        }

sprintf(acErrMsg, "--------> 到期日期:[%ld]", g_pub_tx.mach_date);
WRITEMSG

        /*** 调用计算利息积数的函数 **** 定额续取 定额续存 教育储蓄*/
        if( g_td_parm.td_type[0] == '2' || g_td_parm.td_type[0] == '1' ||
            g_td_parm.td_type[0] == '7' )
        {
            if( pub_base_CalAcm(g_td_mst.lst_date,g_pub_tx.tx_date>g_td_mst.mtr_date?g_td_mst.mtr_date:g_pub_tx.tx_date,
                g_td_parm.intst_term_type,g_td_parm.acm_calc_mode,
                g_td_mst.bal,-g_pub_tx.tx_amt1,&g_td_mst.intst_acm,
                g_td_mst.intst_type,g_td_parm.intst_mon,g_td_parm.intst_day) )
            {
                sprintf(acErrMsg,"计算利息积数错误!");
                WRITEMSG
                goto ErrExit;
            }
        }
    vtcp_log("[%s][%d] *********hst_cnt =[%d]\n",__FILE__,__LINE__,g_td_mst.hst_cnt);
        if( g_pub_tx.svc_ind==1004 )
            g_pub_tx.tx_amt1=g_td_mst.bal;

        if( g_td_parm.td_type[0]=='3' && g_pub_tx.svc_ind==1001 )
        {
            ;
        }
        else
        {
            g_td_mst.bal=g_td_mst.bal-g_pub_tx.tx_amt1;
            g_td_mst.acbo_bal=g_td_mst.acbo_bal-g_pub_tx.tx_amt1;
            if( g_pub_tx.hst_ind[0]=='1' )      /*日间入明细*/
            {
                g_td_mst.hst_bal=g_td_mst.hst_bal-g_pub_tx.tx_amt1;
            }
        }
    vtcp_log("[%s][%d] partflag = %d\n",__FILE__,__LINE__,partflag);
    vtcp_log("[%s][%d] tx_amt1  = %.2f tx_amt2 =%.2f\n",__FILE__,__LINE__,g_pub_tx.tx_amt1,g_pub_tx.tx_amt2);

        switch(g_pub_tx.svc_ind)
        {
            case 1012:
            case 1001:  /*取款*/
                sprintf(acErrMsg, "CCCCCCCC*********交易日期:[%ld]", 
                    g_pub_tx.tx_date);
                WRITEMSG
                if( g_td_parm.thr_draw_ind[0]=='N'
                    && strcmp(g_pub_tx.tx_br_no,g_pub_tx.opn_br_no) )
                {
                    sprintf(acErrMsg,"不可通兑[%s]!",g_td_parm.thr_draw_ind);
                    WRITEMSG
                    strcpy(g_pub_tx.reply,"A011");
                    goto ErrExit;
                }

                /* 检查是否可以做取款交易 ** 定额续存 定活两便 */
                if (g_pub_tx.svc_ind != 1012)
                {
                    if ( g_td_parm.td_type[0] == '1' || 
                         g_td_parm.td_type[0] == '6' || 
                         g_td_parm.td_type[0] == '7' )
                    {
                        /*增加扣划特殊处理 2011/11/13 14:27:10*/
                        if(memcmp(g_pub_tx.tx_code,"4713",4)!=0)
                        {
                            sprintf(acErrMsg,"该产品不允许做取款交易!!");
                            WRITEMSG
                            strcpy(g_pub_tx.reply,"P219");
                            goto ErrExit;   
                        }
                        
                    }
                }

                /* 如果是检查是否允许部提 */
                if (g_td_parm.td_type[0] == '0' && g_pub_tx.svc_ind != 1012)
                /* 普通整存整取 */
                {
                    /* 按财务要求 自动转存不卡部提 add by wudawei 20131108
			if (g_pub_tx.tx_date >= g_td_mst.mtr_date)
                    {
                        sprintf(acErrMsg,"该账户已经到期,不允许做部提!!!");
                        WRITEMSG
                        strcpy(g_pub_tx.reply,"A034");
                        goto ErrExit;
                    }
			add end 20131108 */
                    if (g_td_parm.wdr_ind[0] == 'N')
                    {
                        sprintf(acErrMsg,"普通整存整取不允许部提!!");
                        WRITEMSG
                        strcpy(g_pub_tx.reply,"P215");
                        goto ErrExit;
                    }

                    g_td_mst.tx_cnt = g_td_mst.tx_cnt + 1; /* 部提次数 */

                    if ( g_td_parm.wdr_lmt_cnt != 0 
                        && g_td_mst.tx_cnt > g_td_parm.wdr_lmt_cnt)
                    {
                        sprintf(acErrMsg,"部提次数超过规定的次数!!!");
                        WRITEMSG
                        strcpy(g_pub_tx.reply,"P216");
                        goto ErrExit;
                    }
                    vtcp_log("AC_SEQN[%d][%s]",g_pub_tx.ac_seqn,g_pub_tx.ac_no);
                    if( !g_pub_tx.ac_seqn )
                    {
                        /*扣划特殊处理 2011/11/13 14:31:56 扣划不销凭证*/
                        if(memcmp(g_pub_tx.tx_code,"4713",4)!=0)
                        {
                            wdr_flag=1;
                        }
                        vtcp_log("[%s][%d] 测试跟踪wdr_flag = %d\n",__FILE__,__LINE__,wdr_flag);
                    }
                    /**--- xxxx --- 不需要在此处理 ----**
                    else
                    {
                        **一本通要多加一行**
                        g_mdm_ac_rel.prt_line++;
                    }
                    ***------------------------------***/

                    partflag=1;
    sprintf(acErrMsg,"WDR部提%d",wdr_flag);
    WRITEMSG
    vtcp_log("[%s][%d] partflag = %d\n",__FILE__,__LINE__,partflag);
    vtcp_log("[%s][%d] tx_amt1  = %.2f tx_amt2 =%.2f\n",__FILE__,__LINE__,g_pub_tx.tx_amt1,g_pub_tx.tx_amt2);
                }
            if ( (g_td_parm.td_type[0] == '3' || g_td_parm.td_type[0]=='1') && g_pub_tx.svc_ind != 1012 )
                /* 定额续取 定额取息 */
                {
                    if( pub_base_CompDblVal(g_pub_tx.tx_amt1,
                                     g_td_mst.td_amt)!=0 )
                    {
                        /*增加扣划特殊处理 2011/11/13 14:33:52*/
                        if(memcmp(g_pub_tx.tx_code,"4713",4)!=0)
                        {
                            sprintf(acErrMsg,"取款金额错!");
                            WRITEMSG
                            strcpy(g_pub_tx.reply,"P115");
                            goto ErrExit;
                        }
                    }
 
                    g_td_mst.tx_cnt++;
                    if( g_td_mst.ttl_cnt<g_td_mst.tx_cnt )
                    {
                        sprintf(acErrMsg,"取款超过总次数!");
                        WRITEMSG
                        strcpy(g_pub_tx.reply,"P117");
                        goto ErrExit;
                    }
                    /**********/
                    if (g_td_mst.ttl_cnt == g_td_mst.tx_cnt)
                    {
                        sprintf(acErrMsg,"本次是最后一次交易应做销户交易!!");
                        WRITEMSG
                        strcpy(g_pub_tx.reply,"P229");
                        goto ErrExit;
                    }
                    /***********/
                    switch(g_td_parm.intvl_type[0])
                    {
                        case 'Y':
                            break;
                        case 'Q':
                            break;
                        case 'M':
                            num = g_td_parm.dedr_intvl * g_td_mst.tx_cnt;
                            ret = pub_base_deadlineM(g_td_mst.ic_date,num,
                                  &l_date);
                            if (ret != 0)
                            {
                                sprintf(acErrMsg,"计算间隔取款时间错误!!");
                                WRITEMSG
                                strcpy(g_pub_tx.reply,"P228");
                                goto ErrExit;
                            }
                            if (g_pub_tx.tx_date < l_date)
                            {
                                sprintf(acErrMsg,"还没有到每次取款的时间!!");
                                WRITEMSG
                                strcpy(g_pub_tx.reply,"P218");
                                goto ErrExit;
                            }
                            break;
                        case 'D':
                            num = g_td_parm.dedr_intvl * g_td_mst.tx_cnt;
                            ret = pub_base_deadlineD(g_td_mst.ic_date,num,
                                  &l_date);
                            if (ret != 0)
                            {
                                sprintf(acErrMsg,"计算间隔取款时间错误!!");
                                WRITEMSG
                                strcpy(g_pub_tx.reply,"P228");
                                goto ErrExit;
                            }
                            if (g_pub_tx.tx_date < l_date)
                            {
                                sprintf(acErrMsg,"还没有到每次取款的时间!!");
                                WRITEMSG
                                strcpy(g_pub_tx.reply,"P218");
                                goto ErrExit;
                            }
                            break;
                        default:
                            break;
                    }
                }

sprintf(acErrMsg,"*****WDR部提%d",wdr_flag);
WRITEMSG
    vtcp_log("[%s][%d] partflag = %d\n",__FILE__,__LINE__,partflag);
    vtcp_log("[%s][%d] tx_amt1  = %.2f tx_amt2 =%.2f\n",__FILE__,__LINE__,g_pub_tx.tx_amt1,g_pub_tx.tx_amt2);

sprintf(acErrMsg, "td_type=[%s], svc_ind=[%ld], tx_amt1=[%lf], bal=[%lf],\
    ac_seqn=[%d]", g_td_parm.td_type, g_pub_tx.svc_ind, g_pub_tx.tx_amt1,
    g_td_mst.bal, g_pub_tx.ac_seqn);
WRITEMSG
                /* 通知取款的检查 delete by xxx at 20030822
                if ( g_td_parm.td_type[0]=='4' && g_pub_tx.svc_ind != 1012)
                {
                    if( pub_base_CompDblVal(g_pub_tx.tx_amt1,g_td_mst.bal)<0 )
                    {
                        if( !g_pub_tx.ac_seqn )
                            wdr_flag=1;
                    }

                    partflag=1;
                }*/

                /* 通知取款的检查 add by xxx */
                if ( g_td_parm.td_type[0]=='4' && g_pub_tx.svc_ind != 1012)
                {
                    if( pub_base_CompDblVal(g_td_mst.bal) > 0 )
                    {
                        if( !g_pub_tx.ac_seqn )
                        {
                            /*增加扣划特殊处理 扣划不销凭证*/
                            if(memcmp(g_pub_tx.tx_code,"4713",4)!=0)
                            {   
                                wdr_flag=1;
                            }
                            else
                            {
                                wdr_flag=0;
                            }
                            vtcp_log("[%s][%d] 通知 wdr_flag = %d",__FILE__,__LINE__,wdr_flag);
                        }    
                    }

                    partflag=1;
                }
sprintf(acErrMsg,"*****WDR部提%d, partflag=[%d]",wdr_flag, partflag);
WRITEMSG
                /* 计算利息 ** 定额续取 存本取息 不用计算利息 */
                if ( g_td_parm.td_type[0] != '2' && \
                     g_td_parm.td_type[0] != '3' && \
                     g_pub_tx.svc_ind != 1012 )
                {
                sprintf(acErrMsg, "CCCCCCCC*********交易日期:[%ld]", 
                    g_pub_tx.tx_date);
                WRITEMSG
                    ret = pub_base_CalIntst(&g_pub_intst);
                    if (ret != 0)
                    {
                        sprintf(acErrMsg,"计算存本取息利息错误!!");
                        WRITEMSG
                        strcpy(g_pub_tx.reply,"P220");
                        goto ErrExit;
                    }
                    sprintf(acErrMsg,"取款利息为:,[%lf]",g_pub_intst.factval);
                    WRITEMSG
                }

                /* 检查最低取款金额 */
                if (g_pub_tx.svc_ind != 1012)
                {
                    if (pub_base_CompDblVal(g_pub_tx.tx_amt1,
                                        g_td_parm.draw_min_amt) == -1)
                    {   
                        sprintf(acErrMsg,"取款金额小于最低取款金额!!!");
                        WRITEMSG
                        strcpy(g_pub_tx.reply,"P217");
                        goto ErrExit;
                    }

                    if( partflag ) g_pub_tx.svc_ind=1011; /**部提的标志**/
                }

                break;
            case 1004:              /*理财销户*/
                strcpy(g_pub_tx.brf, "理财销户");
            case 1003:                  /*销户*/
                if( g_pub_tx.svc_ind==1003 )
                    strcpy(g_pub_tx.brf, "销户");
                /* 检查是否提前支取 */
                if (g_td_parm.td_type[0] != '6' \
                         && g_pub_tx.tx_date < g_td_mst.mtr_date)
                {
                    /* 定活两便没有提前支取 */
                    ret = pub_base_checktxdate(g_pub_tx.tx_date,
                          g_td_mst.mtr_date);
                    if (ret != 0 && ret != 100) 
                    {
                        sprintf(acErrMsg,"判断是否提前支取错误!!!");
                        WRITEMSG
                        goto ErrExit;
                    }
                    else if (ret == 100)
                    {
                        if (g_td_parm.fwd_draw_ind[0] == 'N')
                        {
                            sprintf(acErrMsg,"该产品不允许提前支取!!!");
                            WRITEMSG
                            strcpy(g_pub_tx.reply,"P222");
                            goto ErrExit;
                        }
                    }
                }

                if( g_td_parm.thr_cls_ind[0]=='N'
                    && strcmp(g_pub_tx.tx_br_no,g_pub_tx.opn_br_no) )
                {
                    sprintf(acErrMsg,"该产品不允许通兑销户!!!![%s]",
                            g_td_parm.thr_cls_ind);
                    WRITEMSG
                    strcpy(g_pub_tx.reply,"P224");
                    goto ErrExit;
                }
                
                /*** 特种存单不允许通兑销户 ***/
                if( (!strncmp(g_mdm_ac_rel.mdm_code,"0014",4) ||
                     !strncmp(g_pub_tx.mdm_code,"0014",4) ) 
                    && strcmp(g_pub_tx.tx_br_no,g_pub_tx.opn_br_no) )
                {
                    sprintf(acErrMsg,"特种存单账户不允许通兑销户!!");
                    WRITEMSG
                    strcpy(g_pub_tx.reply,"D219");
                    goto ErrExit;
                }

                if( pub_base_CompDblVal(g_td_mst.bal,0.00) 
                   || pub_base_CompDblVal(g_td_mst.acbo_bal,0.00) )
                {
                    sprintf(acErrMsg,"销户后余额不为零![%lf][%lf]", \
                            g_td_mst.bal,g_td_mst.acbo_bal);
                    WRITEMSG
                    strcpy(g_pub_tx.reply,"P116");
                    goto ErrExit;
                }
                /* 检查销户限制条件码 */
                /**---- xxxx ---- 一本通销户不能加 ----**/
            /***定期一本通不需要以下命令    
                if( g_pub_tx.ac_seqn )
                {
                    g_mdm_ac_rel.prt_line=g_mdm_ac_rel.prt_line-2;
                }
                长治管晓军***/
                /**-----------------------------**/

                /* 计算利息 */
                if (g_td_mst.ac_sts[0] != '#')
                {
                    ret = pub_base_CalIntst(&g_pub_intst);
                    if (ret != 0)
                    {
                        sprintf(acErrMsg,"计算存本取息利息错误!!");
                        WRITEMSG
                        strcpy(g_pub_tx.reply,"P220");
                        goto ErrExit;
                    }
                    sprintf(acErrMsg,"CCCCCCC************取款利息为:,[%lf]",
                        g_pub_intst.factval);
                    WRITEMSG
                }

                strcpy(g_td_mst.ac_sts,"*");
                break;
            default:
                sprintf( acErrMsg,"操作类型错误[%d]",g_pub_tx.svc_ind );
                WRITEMSG
                strcpy( g_pub_tx.reply,"D999" );
                goto ErrExit;
        }
        /**   add by  shiweina **/
    if(strcmp(g_pub_tx.tx_code,"2430")==0)     
    {  get_zd_data("0700",kh_type);
        vtcp_log("%s,%d,Gethere7!!!!notice!FD70kh_type=[%s]\n",__FILE__,__LINE__,kh_type);
        if(kh_type[0]=='2')
        {
            vtcp_log("%s,%d,Gethere7!!!!notice!FD70kh_type=[%s]\n",__FILE__,__LINE__,kh_type);
      if(g_td_mst.hold_sts[0]=='3' )/*部分冻结*/
      {
        get_zd_long("0350",&hold_seqn);
        get_zd_long("0340",&ac_seqn);
        ret = Mo_hold_Sel(g_pub_tx.reply,&g_mo_hold,"hold_seqn=%ld  and ac_id=%ld  and ac_seqn=%ld",hold_seqn,g_pub_tx.ac_id,ac_seqn);
        if(ret==100)
        {
           sprintf(acErrMsg," 数据库无此记录[%s]",g_pub_tx.reply);
           WRITEMSG
           goto ErrExit;
        }
        if(ret!=0)
        {
           sprintf(acErrMsg,"查询冻结登记簿出错[%s]",g_pub_tx.reply);
           WRITEMSG
           goto ErrExit;
        }
        if(fabs( g_mo_hold.plan_hold_amt-g_pub_tx.tx_amt1)<0.005)
        {
           ret = Mo_hold_Dec_Upd(g_pub_tx.reply,"hold_seqn=%ld and ac_id=%ld  and  ac_seqn=%ld",hold_seqn,g_pub_tx.ac_id,ac_seqn);
           if(ret)
           {
              sprintf(acErrMsg,"执行Mo_hold_Dec_Upd出错[%d]",ret);
              WRITEMSG
              goto ErrExit;
           }
           ret = Mo_hold_Fet_Upd( &g_mo_hold,g_pub_tx.reply);
           if(ret==100)
           {
              sprintf( acErrMsg,"查询登记簿错,无记录");
               WRITEMSG
              goto ErrExit;
           }
           else if (ret)
           {
              sprintf( acErrMsg,"执行Mo_hold_Fet_Upd出错[%d]",ret);
              WRITEMSG
              goto ErrExit;
            }
           strcpy(g_mo_hold.hold_sts,"1");
           g_mo_hold.fct_unhold_date = g_pub_tx.tx_date;
           strcpy(g_mo_hold.unhold_br_no,g_pub_tx.tx_br_no);
           strcpy(g_mo_hold.unhold_tel,g_pub_tx.tel);
           strcpy(g_mo_hold.unhold_brf,g_pub_tx.brf);
           g_mo_hold.wrk_date = g_pub_tx.tx_date;
           g_mo_hold.trace_no = g_pub_tx.trace_no;
           ret = Mo_hold_Upd_Upd(g_mo_hold,g_pub_tx.reply);
           if(ret)
           {
               sprintf(acErrMsg," 修改记录出错[%d]",ret);
               WRITEMSG
               goto ErrExit;
           }
               Mo_hold_Clo_Upd();
        } 
        else if (fabs( g_mo_hold.plan_hold_amt-g_pub_tx.tx_amt1)>0.005)
        {
          ret = Mo_hold_Dec_Upd(g_pub_tx.reply,"hold_seqn=%ld and  ac_id=%ld and ac_seqn=%ld",hold_seqn,g_pub_tx.ac_id,ac_seqn);
          if (ret)
          {
              sprintf(acErrMsg,"执行Mo_hold_Dec_Upd出错[%d]",ret);
              WRITEMSG
              goto ErrExit;
          }
          ret = Mo_hold_Fet_Upd( &g_mo_hold,g_pub_tx.reply);
          if(ret==100)
          {
              sprintf( acErrMsg,"查询登记簿错,无记录");
              WRITEMSG
              goto ErrExit;
          }
          else if(ret)
          {
              sprintf( acErrMsg,"执行Mo_hold_Fet_Upd出错[%d]",ret);                           WRITEMSG
              goto ErrExit;
          } 
  
          g_mo_hold.plan_hold_amt=g_mo_hold.plan_hold_amt-g_pub_tx.tx_amt1;
         g_mo_hold.wrk_date = g_pub_tx.tx_date;
         g_mo_hold.trace_no = g_pub_tx.trace_no;
         ret = Mo_hold_Upd_Upd(g_mo_hold,g_pub_tx.reply);
         if(ret)
         {
             sprintf(acErrMsg," 修改记录出错[%d]",ret);
             WRITEMSG
             goto ErrExit;
         }
            Mo_hold_Clo_Upd();
        }  else if(fabs(g_pub_tx.tx_amt1-g_mo_hold.plan_hold_amt)>0.005)                   
             {
               sprintf(acErrMsg,"扣划金额大于冻结金额");
               WRITEMSG
               goto ErrExit;
           }
    }
    else if(g_td_mst.hold_sts[0]=='2' || g_td_mst.hold_sts[0]=='1' )/*非部分冻结*/
    {
      get_zd_long("0350",&hold_seqn);
      get_zd_long("0340",&ac_seqn);
      ret = Mo_hold_Dec_Upd(g_pub_tx.reply,"hold_seqn=%ld and ac_id=%ld  and ac_seqn=%ld",hold_seqn,g_pub_tx.ac_id,ac_seqn);
      if(ret)
      {
         sprintf(acErrMsg,"执行Mo_hold_Dec_Upd出错[%d]",ret);
         WRITEMSG
         goto ErrExit;
      }
      ret = Mo_hold_Fet_Upd( &g_mo_hold,g_pub_tx.reply);
      if(ret==100)
      {
        sprintf( acErrMsg,"查询登记簿错,无记录");
        WRITEMSG
        goto ErrExit;
      }
      else if (ret)
      {
        sprintf( acErrMsg,"执行Mo_hold_Fet_Upd出错[%d]",ret);
        WRITEMSG
        goto ErrExit;
      }
       g_mo_hold.wrk_date = g_pub_tx.tx_date;
       g_mo_hold.trace_no = g_pub_tx.trace_no;
      ret = Mo_hold_Upd_Upd(g_mo_hold,g_pub_tx.reply);
      if(ret)
      {
         sprintf(acErrMsg," 修改记录出错[%d]",ret);
         WRITEMSG
         goto ErrExit;
      }
         Mo_hold_Clo_Upd();
    } 
    vtcp_log("[%s][%d] *********hst_cnt =[%d]\n",__FILE__,__LINE__,g_td_mst.hst_cnt);
    double dj_dbal=0.00;
    ret=Td_mst_Dec_Upd(g_pub_tx.reply,"ac_id=%ld  and ac_seqn=%ld",g_mdm_ac_rel.ac_id,ac_seqn);
    if(ret)
    {
       sprintf(acErrMsg,"执行Td_mst_Dec_Upd出错[%s]",g_pub_tx.reply);
       WRITEMSG
       goto ErrExit;
    } 
    ret=Td_mst_Fet_Upd(&g_td_mst,g_pub_tx.reply);
    if(ret==100)
    {
       sprintf(acErrMsg,"查询存款主表出错,无记录");
       WRITEMSG
       goto ErrExit;
    }
    if(ret)
    {
       sprintf(acErrMsg,"查询主表程序调用异常[%s]",g_pub_tx.reply);
       WRITEMSG
       goto ErrExit;
    }
    ret=sql_sum_double("mo_hold","plan_hold_amt",&dj_dbal,"hold_sts='0' and ac_id=%ld and ac_seqn=%ld",g_pub_tx.ac_id,ac_seqn);
    if(ret)
    {
         sprintf(acErrMsg,"程序调用异常[%s]",g_pub_tx.reply);
         WRITEMSG
          goto ErrExit;
    }
    g_td_mst.hold_amt=dj_dbal; 
    ret=sql_min_string("mo_hold","hold_typ",minholdtyp,1,"hold_sts='0' and ac_id=%ld  and ac_seqn",g_pub_tx.ac_id,ac_seqn);
    if(ret)
    {
        sprintf(acErrMsg,"程序调用异常[%s]",g_pub_tx.reply);
        WRITEMSG
        goto ErrExit;
    }
    vtcp_log("[%s][%d] *********hst_cnt =[%d]\n",__FILE__,__LINE__,g_td_mst.hst_cnt);
    strcpy(g_td_mst.hold_sts,minholdtyp);
    ret=Td_mst_Upd_Upd(g_td_mst,g_pub_tx.reply);
    if(ret)
    {
       sprintf(acErrMsg,"Td_mst_Upd_Upd出错=[%d]",ret);
       WRITEMSG
       goto ErrExit;
    }
       Td_mst_Clo_Upd();
    }
    
    }      /**     到此结束   **/
    else
    {
	if( g_td_mst.hold_sts[0]=='3' && g_td_mst.bal<g_td_mst.hold_amt && memcmp(g_pub_tx.tx_code,"4713",4)!=0)
          {
            sprintf(acErrMsg,"该账号已被部分冻结!");
            WRITEMSG
            strcpy(g_pub_tx.reply,"P111");
            goto ErrExit;
          }
      }
        /* 检查帐户余额 */
        if ( pub_base_CompDblVal(g_td_mst.bal,0.00)<0 )
        {
            sprintf(acErrMsg,"取款额大于帐户余额!!bal=[%lf] amt1=[%lf]",
                g_td_mst.bal,g_pub_tx.tx_amt1);
            WRITEMSG
            strcpy(g_pub_tx.reply,"P214");
            goto ErrExit;
        }
vtcp_log("CDBZ[%s][%s]",g_pub_tx.tx_code,g_pub_tx.tel);
        if( !strcmp(g_pub_tx.tx_code,"G090") && 
            !strcmp(g_pub_tx.tel,"9999") )
        {
            vtcp_log("承兑扣款不管备注CDKKBGBZ");
        }
        else if( pub_base_CompDblVal(g_td_mst.bal,g_td_mst.ctl_amt)<0 )
        {
            if( pub_base_CompDblVal(g_td_mst.ctl_amt,0.00) )
            {
                if( Remark_ctl_Sel(g_pub_tx.reply,&rem,
                    "ac_id=%d AND rem_sts!='2'", g_td_mst.ac_id) )
                {
                    strcpy( rem.rem,"" );
                }
                pub_base_strpack( rem.rem );
                rem.rem[50]='\0';

                strcpy( g_pub_tx.reply,"D247");
                sprintf(msg,"%-50s",rem.rem);
                set_zd_data("0130",msg);
                vtcp_log(" MSG=[%s]",msg);
                TRACE
                goto ErrExit;
            }
        }
    }
    else if( g_pub_tx.add_ind[0]=='1' )     /*存款*/
    {
    vtcp_log("[%s][%d] *********hst_cnt =[%d]\n",__FILE__,__LINE__,g_td_mst.hst_cnt);
        if( g_pub_tx.svc_ind!=1001 && g_pub_tx.svc_ind!=1000 \
            && g_pub_tx.svc_ind!=1008 && g_pub_tx.svc_ind!= 1012)
        {
            sprintf( acErrMsg,"操作类型错误[%d]",g_pub_tx.svc_ind );
            WRITEMSG
            strcpy( g_pub_tx.reply,"D999" );
            goto ErrExit;
        }

        /** 1-定额续存，7-不定额续存 **/
        if (g_pub_tx.svc_ind != 1012)
        {
            if (g_td_parm.td_type[0] != '1' && g_td_parm.td_type[0] != '7')
            {
                sprintf(acErrMsg,"该产品的定期存款类型不允许做存款交易!!! \
                    [%s]",g_td_parm.td_type);
                WRITEMSG
                strcpy(g_pub_tx.reply,"P225");
                goto ErrExit;
            }
            if (g_td_parm.dpst_ind[0] == 'N')
            {
                sprintf(acErrMsg,"该产品不允许续存!!");
                WRITEMSG
                strcpy(g_pub_tx.reply,"P225");
                goto ErrExit;
            }
        }

        if ( strcmp(g_pub_tx.tx_br_no,g_pub_tx.opn_br_no) 
              && g_td_parm.thr_dpst_ind[0] == 'N' && g_pub_tx.svc_ind!=1012 )
        {
            sprintf(acErrMsg,"该产品不允许通存!!");
            WRITEMSG
            strcpy(g_pub_tx.reply,"P226");
            goto ErrExit;
        }

        /*** 调用计算利息积数的函数 **** 定额续存 */
        if (g_pub_tx.svc_ind != 1012)
        {
            if ( g_td_parm.td_type[0] == '1' || g_td_parm.td_type[0] == '7' )
            {
                if( pub_base_CalAcm(g_td_mst.lst_date,g_pub_tx.tx_date,
                    g_td_parm.intst_term_type,g_td_parm.acm_calc_mode,
                    g_td_mst.bal,g_pub_tx.tx_amt1,&g_td_mst.intst_acm,
                    g_td_mst.intst_type,g_td_parm.intst_mon,
                    g_td_parm.intst_day) )
                {
                    sprintf(acErrMsg,"计算利息积数错误!");
                    WRITEMSG
                    goto ErrExit;
                }
            }
        }

        g_td_mst.bal=g_td_mst.bal+g_pub_tx.tx_amt1;
        g_td_mst.acbo_bal=g_td_mst.acbo_bal+g_pub_tx.tx_amt1;

    vtcp_log("[%s][%d] *********hst_cnt =[%d]\n",__FILE__,__LINE__,g_td_mst.hst_cnt);
        if( g_pub_tx.hst_ind[0]=='1' )      /*日间入明细*/
        {
            g_td_mst.hst_bal=g_td_mst.hst_bal+g_pub_tx.tx_amt1;
            g_td_mst.hst_cnt++;
        }
    vtcp_log("[%s][%d] *********hst_cnt =[%d]\n",__FILE__,__LINE__,g_td_mst.hst_cnt);

        if (g_pub_tx.svc_ind != 1012)
        {
            if( pub_base_CompDblVal(g_pub_tx.tx_amt1,g_td_mst.td_amt)!=0 
                && g_td_parm.td_type[0]=='1' )
            {
                sprintf(acErrMsg,"存款金额错!");
                WRITEMSG
                strcpy(g_pub_tx.reply,"P118");
                goto ErrExit;
            }

            /*** 零整存款类检查 ***/
            if( g_td_parm.td_type[0]=='1' && pub_chk_td_hst(1)!=0 )
            {
                sprintf(acErrMsg,"零整存款检查错误!");
                WRITEMSG
                goto ErrExit;
            }

            /*** 教育存款类检查 ***/
            if( g_td_parm.td_type[0]=='7' && pub_chk_td_hst(2)!=0 )
            {
                sprintf(acErrMsg,"教育类存款检查错误!");
                WRITEMSG
                goto ErrExit;
            }

            /** add by xxx at 20030727 ***/
            if (g_td_parm.td_type[0] == '7' && pub_chk_JYCX() != 0)
            {
                sprintf(acErrMsg, "一个月内不能存两万!!");
                WRITEMSG
                goto ErrExit;
            }

            if (pub_base_CompDblVal(g_pub_tx.tx_amt1,g_td_parm.dpst_min_amt) == -1)
            {
                sprintf(acErrMsg,"存款金额小于最低续存金额!!");
                WRITEMSG
                strcpy(g_pub_tx.reply,"P227");
                goto ErrExit;
            }
    vtcp_log("[%s][%d] *********hst_cnt =[%d]\n",__FILE__,__LINE__,g_td_mst.hst_cnt);

            g_td_mst.tx_cnt++;
    vtcp_log("[%s][%d] *********hst_cnt =[%d]\n",__FILE__,__LINE__,g_td_mst.hst_cnt);
            if( g_td_mst.ttl_cnt<g_td_mst.tx_cnt )
            {
                sprintf(acErrMsg,"存款超过总次数!");
                WRITEMSG
                strcpy(g_pub_tx.reply,"P119");
                goto ErrExit;
    vtcp_log("[%s][%d] *********hst_cnt =[%d]\n",__FILE__,__LINE__,g_td_mst.hst_cnt);
            }
    vtcp_log("[%s][%d] *********hst_cnt =[%d]\n",__FILE__,__LINE__,g_td_mst.hst_cnt);
        }
    vtcp_log("[%s][%d] *********hst_cnt =[%d]\n",__FILE__,__LINE__,g_td_mst.hst_cnt);
    }
    else
    {
        sprintf(acErrMsg,"存取标志错误g_pub_tx.add_ind=[%s]!",g_pub_tx.add_ind);
        WRITEMSG
        strcpy(g_pub_tx.reply,"P106");
        goto ErrExit;
    }

    vtcp_log("[%s][%d] *********hst_cnt =[%d]\n",__FILE__,__LINE__,g_td_mst.hst_cnt);
sprintf(acErrMsg,"WDR部提%d",wdr_flag);
WRITEMSG
    g_td_mst.lst_date=g_pub_tx.tx_date;

    /* add by xxx at 20030728 ** 销户不用检查 */
    sprintf(acErrMsg, "******** svc_ind = [%ld]", g_pub_tx.svc_ind);
    WRITEMSG
sprintf(acErrMsg, "8888888888888************* ic_date=[%ld]", g_td_mst.ic_date);
WRITEMSG

    if( pub_base_CompDblVal(g_td_mst.bal,g_td_parm.min_bal)<0 
        && g_td_mst.ac_sts[0]=='1' && memcmp(g_pub_tx.tx_code,"4713",4)!=0) /*增加扣划特殊处理 2011/11/13 14:43:45*/
    {
        sprintf(acErrMsg,"存款余额[%.2lf]低于下限[%.2lf]",
            g_td_mst.bal,g_td_parm.min_bal );
        WRITEMSG
        strcpy(g_pub_tx.reply,"A022" );
        goto ErrExit;
    }
    vtcp_log("[%s][%d] *********hst_cnt =[%d]\n",__FILE__,__LINE__,g_td_mst.hst_cnt);

    if( pub_base_CompDblVal(g_td_mst.bal,g_td_parm.max_bal)>0 
        && pub_base_CompDblVal(g_td_parm.max_bal,0.00) 
        && g_pub_tx.svc_ind != 1012)
    {
        sprintf(acErrMsg,"存款余额[%.2lf]高于上限[%.2lf]",
            g_td_mst.bal,g_td_parm.max_bal );
        WRITEMSG
        strcpy(g_pub_tx.reply,"A008" );
        goto ErrExit;
    }
    vtcp_log("[%s][%d] *********hst_cnt =[%d]\n",__FILE__,__LINE__,g_td_mst.hst_cnt);

    sprintf(acErrMsg, "******日间入明细标志:[%s]", g_pub_tx.hst_ind);
    WRITEMSG
    if (g_pub_tx.hst_ind[0]== '1') /*日间入明细*/
    {
        strcpy(g_pub_tx.name,g_td_mst.name);
        g_pub_tx.cif_no=g_td_mst.cif_no;
    vtcp_log("[%s][%d] *********hst_cnt =[%d]\n",__FILE__,__LINE__,g_td_mst.hst_cnt);
        ret=pub_ins_td_hst();
    vtcp_log("[%s][%d] *********hst_cnt =[%d]\n",__FILE__,__LINE__,g_td_mst.hst_cnt);
        ERR_DEAL
    }
    vtcp_log("[%s][%d] *********hst_cnt =[%d]\n",__FILE__,__LINE__,g_td_mst.hst_cnt);

    ret=Td_mst_Dec_Upd( g_pub_tx.reply,"ac_id='%ld' and ac_seqn='%d'", \
                        g_pub_tx.ac_id,g_pub_tx.ac_seqn);
    if( ret )
    {
        sprintf(acErrMsg,"账号不存在[%d][%d]",g_pub_tx.ac_id,ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"P102");
        goto ErrExit;
    }
    if(pub_base_CompDblVal(g_pub_tx.tx_amt1,0.00) != 0)  
    {
      vtcp_log("%s,%d,零金额不发短信通知[%.2f]\n",__FILE__,__LINE__,g_pub_tx.tx_amt1);
      pub_mob_sendmail(  g_pub_tx.brf, g_pub_tx.tx_code,g_mdm_ac_rel.ac_no,g_pub_tx.add_ind,g_pub_tx.tx_amt1, g_td_mst.bal );
		}

    ret=Td_mst_Fet_Upd(&TmpTd_mst,g_pub_tx.reply);
    if( ret )
    {
        sprintf(acErrMsg,"账号不存在[%d][%d]",g_pub_tx.ac_id,ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"P102");
        goto ErrExit;
    }
sprintf(acErrMsg, "8888888888888************* ic_date=[%ld]", g_td_mst.ic_date);
WRITEMSG
    vtcp_log("[%s][%d] *********hst_cnt =[%d]\n",__FILE__,__LINE__,g_td_mst.hst_cnt);
    ret=Td_mst_Upd_Upd(g_td_mst,g_pub_tx.reply);
    if( ret )
    {
        sprintf(acErrMsg,"账号不存在[%d][%d]",g_pub_tx.ac_id,ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"P102");
        goto ErrExit;
    }
    Td_mst_Clo_Upd();

    ret=pub_acct_ac_rec( g_td_mst.ac_id,g_td_mst.ac_seqn,
        g_pub_tx.add_ind,g_pub_tx.ct_ind,g_pub_tx.tx_code,g_pub_tx.tx_amt1,"0000" );
    if( ret ) goto ErrExit;
OkExit:
    return 0;
ErrExit:
    return 1;
}

int Chg_ln_mst()
{
    int ret;
    int do_flag=0;
    double yqlx=0.00;
    struct ln_mst_c tmp_ln_mst;
    TRACE
    sprintf(acErrMsg,"前(本金积数)PASS为 [%lf]",g_ln_mst.intst_acm );
    WRITEMSG
    memset( &tmp_ln_mst , 0x00 , sizeof( struct ln_mst_c ));

    /*** 调用贷款产品参数各项检查函数 ***/
    sprintf(acErrMsg,"发生额和贷款余额,已放JJJJ[%.2lf][%.2lf][%.2lf][%s]",
                    g_pub_tx.tx_amt1,g_ln_mst.bal ,g_ln_mst.ttl_prvd_amt,g_pub_tx.ac_get_ind);
    WRITEMSG
    sprintf(acErrMsg,"发生额和贷款余额[%.2lf][%.2lf]",
                    g_pub_tx.tx_amt1,g_ln_mst.bal );
    WRITEMSG
    if( g_pub_tx.ac_get_ind[1]!='1' ) /**未读取ln_mst**/
    {
        TRACE
    sprintf(acErrMsg,"发生额和贷款余额,已放[%.2lf][%.2lf][%.2lf]",
                    g_pub_tx.tx_amt1,g_ln_mst.bal ,g_ln_mst.ttl_prvd_amt);
    WRITEMSG
        memset( &g_ln_mst , 0x00 , sizeof( struct ln_mst_c ));
    sprintf(acErrMsg,"发生额和贷款余额,已放[%.2lf][%.2lf][%.2lf]",
                    g_pub_tx.tx_amt1,g_ln_mst.bal ,g_ln_mst.ttl_prvd_amt);
    WRITEMSG
        ret=Ln_mst_Dec_Upd(g_pub_tx.reply,"ac_id='%ld' and ac_seqn='%d' ",
            g_pub_tx.ac_id,g_pub_tx.ac_seqn);
    WRITEMSG
        ERR_DEAL
    sprintf(acErrMsg,"发生额和贷款余额,已放[%.2lf][%.2lf][%.2lf]",
                    g_pub_tx.tx_amt1,g_ln_mst.bal ,g_ln_mst.ttl_prvd_amt);
    WRITEMSG

        ret=Ln_mst_Fet_Upd(&g_ln_mst,g_pub_tx.reply);
        if( ret )
        {
            sprintf(acErrMsg,"账号不存在[%d][%d]",g_pub_tx.ac_id,ret);
            WRITEMSG
            if( ret==100 ) strcpy(g_pub_tx.reply,"P102");
            goto ErrExit;
        }
    sprintf(acErrMsg,"发生额和贷款余额,已放[%.2lf][%.2lf][%.2lf]",
                    g_pub_tx.tx_amt1,g_ln_mst.bal ,g_ln_mst.ttl_prvd_amt);
    WRITEMSG
    }else{
         TRACE
         ret=Ln_mst_Dec_Upd(g_pub_tx.reply,"ac_id='%ld' and ac_seqn='%d' ",
                    g_pub_tx.ac_id,g_pub_tx.ac_seqn);
         ERR_DEAL

         ret=Ln_mst_Fet_Upd(&tmp_ln_mst,g_pub_tx.reply);
         if( ret )
         {
             sprintf(acErrMsg,"账号不存在[%d][%d]",g_pub_tx.ac_id,ret);
             WRITEMSG
             if( ret==100 ) strcpy(g_pub_tx.reply,"P102");
             goto ErrExit;
         }
    }

    sprintf(acErrMsg,"前(本金积数)PASS为 [%lf]",g_ln_mst.intst_acm );
    WRITEMSG

    sprintf(acErrMsg,"发生额和贷款余额,已放[%.2lf][%.2lf][%.2lf]",
                    g_pub_tx.tx_amt1,g_ln_mst.bal ,g_ln_mst.ttl_prvd_amt);
    WRITEMSG
    TRACE
    ret = Ln_parm_Sel( g_pub_tx.reply,&g_ln_parm,
            " prdt_no='%s' ",g_ln_mst.prdt_no );
    ERR_DEAL
    sprintf(acErrMsg,"发生额和贷款余额[%.2lf][%.2lf]",
                    g_pub_tx.tx_amt1,g_ln_mst.bal );
    WRITEMSG
    strcpy( g_pub_tx.cur_no,g_ln_parm.cur_no );
    strcpy( g_pub_tx.opn_br_no,g_ln_mst.opn_br_no );
    set_zd_data("0020", g_pub_tx.opn_br_no);
    strcpy( g_pub_tx.name,g_ln_mst.name );
    g_pub_tx.cif_no=g_ln_mst.cif_no;
    strcpy(g_pub_tx.prdt_code,g_ln_parm.prdt_no);
    sprintf(acErrMsg,"发生额和贷款余额[%.2lf][%.2lf]",
                    g_pub_tx.tx_amt1,g_ln_mst.bal );
    WRITEMSG
    /**账户状态检查**/
    if( g_ln_mst.ac_sts[0]=='*' )
    {
        sprintf(acErrMsg,"贷款账号已销户![%s]",g_ln_mst.ac_sts);
        WRITEMSG
        strcpy(g_pub_tx.reply,"P120");
        goto ErrExit;
    }
    TRACE
    if( g_ln_mst.ac_sts[0]=='9' )
    {
        sprintf(acErrMsg,"贷款账号已撤销![%s]",g_ln_mst.ac_sts);
        WRITEMSG
        strcpy(g_pub_tx.reply,"L036");
        goto ErrExit;
    }
    /*lpj20151129之后没有贷款逾期所以dc_code都是ln_parm.dc_code*/
		strcpy(g_pub_tx.dc_code,g_ln_parm.dc_code); 
		TRACE
   /* if( g_ln_mst.ac_sts[0]=='1' )* 正常 *
    {
    TRACE
        strcpy(g_pub_tx.dc_code,g_ln_parm.dc_code); 
    }
    if( g_ln_mst.ac_sts[0]=='2' )* 逾期90内 *
    {
    TRACE
        strcpy(g_pub_tx.dc_code,g_ln_parm.over_dc_code);    
    }
    **add by ligl2006-9-25 17:31*
    if(g_ln_mst.ac_sts[0]=='5')**逾期90外*
    {
    TRACE
       strcpy(g_pub_tx.dc_code,g_ln_parm.over90_dc_code);
    }
    **end*
    if( g_ln_mst.ac_sts[0]=='3' )* 呆滞 *
    {
    TRACE
        strcpy(g_pub_tx.dc_code,g_ln_parm.sla_dc_code); 
    }
    if( g_ln_mst.ac_sts[0]=='4' )* 呆账 *
    {
    TRACE
        strcpy(g_pub_tx.dc_code,g_ln_parm.bad_dc_code); 
    }lpj20151129end*/
    sprintf(acErrMsg,"发生额和贷款余额[%.2lf][%.2lf]",
                    g_pub_tx.tx_amt1,g_ln_mst.bal );
    WRITEMSG

    sprintf(acErrMsg,"前(本金积数)PASS为 [%lf]",g_ln_mst.intst_acm );
    WRITEMSG
    /*** 调用计算利息积数的函数(状态为正常的按揭贷款不处理) ***/
    if(!((g_ln_parm.ln_pay_type[0]=='3'||g_ln_parm.ln_pay_type[0]=='4')
                                            &&g_ln_mst.ac_sts[0]=='1') )
    {
        TRACE
        ret = pub_base_CalAcm_Ln( g_ln_mst.lst_date , g_pub_tx.tx_date , "0" ,
            "2",g_ln_mst.bal,g_pub_tx.tx_amt1 , 
            &g_ln_mst.intst_acm ,"0",0,0 );
        ERR_DEAL
        ln_mst_acm.intst_acm = g_ln_mst.intst_acm;
        
        ret = pub_base_CalAcm_Ln( g_ln_mst.lst_date , g_pub_tx.tx_date , "0" ,
            "2",g_ln_mst.in_lo_intst,g_pub_tx.tx_amt2, 
            &g_ln_mst.in_lo_acm ,"0",0,0 );
        ERR_DEAL
        ln_mst_acm.in_lo_acm = g_ln_mst.in_lo_acm;
                
        ret = pub_base_CalAcm_Ln( g_ln_mst.lst_date , g_pub_tx.tx_date , "0" ,
            "2" , g_ln_mst.out_lo_intst , g_pub_tx.tx_amt3,
            &g_ln_mst.out_lo_acm ,"0",0,0 );
        ERR_DEAL
        ln_mst_acm.out_lo_acm = g_ln_mst.out_lo_acm;

        ret = pub_base_CalAcm_Ln( g_ln_mst.lst_date , g_pub_tx.tx_date , "0" ,
            "2" , g_ln_mst.cmpd_lo_intst , g_pub_tx.tx_amt4,
            &g_ln_mst.cmpd_lo_acm ,"0",0,0 );
        ERR_DEAL
        ln_mst_acm.cmpd_lo_acm = g_ln_mst.cmpd_lo_acm;

    }
    g_ln_mst.lst_date=g_pub_tx.tx_date;
    sprintf(acErrMsg,"发生额和贷款余额[%.2lf][%.2lf]",
                        g_pub_tx.tx_amt1,g_ln_mst.bal );
    WRITEMSG

    sprintf(acErrMsg,"前(本金积数)PASS为 [%lf]",g_ln_mst.intst_acm );
    WRITEMSG

    /**如果是积数结息，清理积数，注：分段积数由各程序自己清**/
    if( g_pub_tx.svc_ind==3011 ||g_pub_tx.svc_ind==3012 || 
                g_pub_tx.svc_ind==3052 ||g_pub_tx.svc_ind==3063 )
    {   
        g_ln_mst.intst_acm=0.00;
        g_ln_mst.ic_date=g_pub_tx.tx_date;  
    }   
    if( g_pub_tx.svc_ind==3021||g_pub_tx.svc_ind==3051||
                g_pub_tx.svc_ind==3052 ||g_pub_tx.svc_ind==3063 )
    {
        g_ln_mst.in_lo_acm=0.00;
        g_ln_mst.ic_date=g_pub_tx.tx_date;
    }   
    if( g_pub_tx.svc_ind==3031||g_pub_tx.svc_ind==3051||
                g_pub_tx.svc_ind==3052 ||g_pub_tx.svc_ind==3063 )
    {
        g_ln_mst.out_lo_acm=0.00;
        g_ln_mst.ic_date=g_pub_tx.tx_date;  
    }
    if( g_pub_tx.svc_ind==3041||g_pub_tx.svc_ind==3051||
                g_pub_tx.svc_ind==3052 ||g_pub_tx.svc_ind==3063 )
    {
        g_ln_mst.cmpd_lo_acm=0.00;
        g_ln_mst.ic_date=g_pub_tx.tx_date;  
    }
    do_flag=0;
     		
    TRACE
    if( g_pub_tx.add_ind[0]=='1' ) /*增加*/
    {
        if( g_pub_tx.svc_ind==3010 ) /*贷款本金增加，即放款*/
        {
            TRACE
            sprintf(acErrMsg,"现转标志![%s]",g_pub_tx.ct_ind);
            WRITEMSG
            if( g_ln_mst.ac_sts[0]!='1' && g_ln_parm.ac_sts[0]=='1' )
            {
                TRACE
                sprintf(acErrMsg, "状态不对[%s]", g_ln_mst.ac_sts);
                WRITEMSG
                strcpy( g_pub_tx.reply,"L067" );
                goto ErrExit;
            }
            TRACE
            if( pub_base_CompDblVal(g_ln_mst.amt_lmt,0.00 ) )
            {
                if( pub_base_CompDblVal(g_ln_mst.amt_lmt,
                    g_ln_mst.ttl_prvd_amt+g_pub_tx.tx_amt1)<0 )
                {
                    sprintf(acErrMsg,
                    "超过发放限额：已放[%.2lf]新[%.2lf]>限额[%.2lf]",
                    g_ln_mst.ttl_prvd_amt,g_pub_tx.tx_amt1,g_ln_mst.amt_lmt);
                    WRITEMSG
                    strcpy( g_pub_tx.reply,"P122" );
                    goto ErrExit;
                }
            }
            g_ln_mst.bal+=g_pub_tx.tx_amt1;
            g_ln_mst.ttl_prvd_amt+=g_pub_tx.tx_amt1;
            ret=pub_ins_ln_hst( 0 , 0 );
            ERR_DEAL
            do_flag=1;
        }
        
        if(g_pub_tx.svc_ind==3065)/*贷款展期*/
        {
            ret=pub_ins_ln_hst( 0 , 0 );
            ERR_DEAL
            do_flag=1;
        }
                
        if( g_pub_tx.svc_ind==3091 ) /*补记贷款本金*/
        {
            TRACE
            sprintf(acErrMsg,"现转标志![%s]",g_pub_tx.ct_ind);
            WRITEMSG
            if( pub_base_CompDblVal(g_ln_mst.amt_lmt,
                g_ln_mst.ttl_prvd_amt+g_pub_tx.tx_amt1)<0 )
            {
                sprintf(acErrMsg,
                    "超过发放限额：已放[%.2lf]新[%.2lf]>限额[%.2lf]",
                    g_ln_mst.ttl_prvd_amt,g_pub_tx.tx_amt1,g_ln_mst.amt_lmt);
                WRITEMSG
                strcpy( g_pub_tx.reply,"P122" );
                goto ErrExit;
            }
            g_ln_mst.bal+=g_pub_tx.tx_amt1;
            g_ln_mst.ttl_prvd_amt+=g_pub_tx.tx_amt1;
            if( pub_base_CompDblVal(g_ln_mst.amt_lmt,
                g_ln_mst.bal)<0 )
            {
                sprintf(acErrMsg,
                    "超过发放限额：已放[%.2lf]新[%.2lf]>限额[%.2lf]",
                    g_ln_mst.ttl_prvd_amt,g_pub_tx.tx_amt1,g_ln_mst.amt_lmt);
                WRITEMSG
                strcpy( g_pub_tx.reply,"P122" );
                goto ErrExit;
            }
            ret=pub_ins_ln_hst( 0 , 0 );
            ERR_DEAL
            do_flag=1;
        }
        /*****ligl*******   
        if(g_pub_tx.svc_ind==3019)
        {
            TRACE
            sprintf(acErrMsg,"现转标志![%s]",g_pub_tx.ct_ind);
            WRITEMSG
            if( pub_base_CompDblVal(g_ln_mst.amt_lmt,
                g_ln_mst.ttl_prvd_amt+g_pub_tx.tx_amt1)<0 )
            {
                sprintf(acErrMsg,
                    "超过发放限额：已放[%.2lf]新[%.2lf]>限额[%.2lf]",
                    g_ln_mst.ttl_prvd_amt,g_pub_tx.tx_amt1,g_ln_mst.amt_lmt);
                WRITEMSG
                strcpy( g_pub_tx.reply,"P122" );
                goto ErrExit;
            }
            g_ln_mst.bal+=-g_pub_tx.tx_amt1;
            g_ln_mst.ttl_prvd_amt+=-g_pub_tx.tx_amt1;
            ret=pub_ins_ln_hst( 0 , 0 );
            ERR_DEAL
            do_flag=1;
        }   
      **************/
        if( g_pub_tx.svc_ind==3092 ) /*补记贷款抵押品金额*/
        {
            g_ln_mst.gage_amt+=g_pub_tx.tx_amt1;
            ret=pub_ins_ln_hst( 8 , 0 );
            ERR_DEAL
            do_flag=1;
        }               
        if(g_pub_tx.svc_ind==3011||g_pub_tx.svc_ind==3012)/*本金积数产生的利息*/
        {
            ret=pub_ins_ln_hst( 4 , 0 );
            ERR_DEAL
            do_flag=1;
        }
        /*贷款表内利息增加，即挂息*/
        if(g_pub_tx.svc_ind==3020||g_pub_tx.svc_ind==3050||
                                                g_pub_tx.svc_ind==3052) 
        {
            if( pub_base_CompDblVal( g_pub_tx.tx_amt2 , 0.00 ) )
            {
                strcpy(g_pub_tx.brf,"贷款表内挂息");
                g_ln_mst.in_lo_intst+=g_pub_tx.tx_amt2;
                ret=pub_ins_ln_hst( 1 , 0 );
                ERR_DEAL
                do_flag=1;
            }
        }
        /*贷款表内积数产生的利息*/
        if( g_pub_tx.svc_ind==3021 || g_pub_tx.svc_ind==3051 ) 
        {
            strcpy(g_pub_tx.brf,"计算表内积数利息");
            ret=pub_ins_ln_hst( 5 , 0);
            ERR_DEAL
            do_flag=1;
        }
        /*贷款表外利息增加，即挂息*/
        if( g_pub_tx.svc_ind==3030 || g_pub_tx.svc_ind==3050 ||
                                            g_pub_tx.svc_ind==3052) 
        {
            if( pub_base_CompDblVal( g_pub_tx.tx_amt3 , 0.00 ) )
            {
                strcpy(g_pub_tx.brf,"贷款表外挂息");
                g_ln_mst.out_lo_intst+=g_pub_tx.tx_amt3;
                ret=pub_ins_ln_hst( 2 , 0 );
                ERR_DEAL
                do_flag=1;
            }
        }
        /*贷款表外积数产生的利息*/
        if( g_pub_tx.svc_ind==3031 || g_pub_tx.svc_ind==3051 ) 
        {
            strcpy(g_pub_tx.brf,"计算表外积数利息");
            ret=pub_ins_ln_hst( 6 , 0 );
            ERR_DEAL
            do_flag=1;
        }
        /*贷款复利增加，即挂息*/
        if( g_pub_tx.svc_ind==3040 || g_pub_tx.svc_ind==3050||
                                                g_pub_tx.svc_ind==3052 ) 
        {
            if( pub_base_CompDblVal( g_pub_tx.tx_amt4 , 0.00 ) )
            {
                strcpy(g_pub_tx.brf,"贷款复利挂息");
                g_ln_mst.cmpd_lo_intst+=g_pub_tx.tx_amt4;
                ret=pub_ins_ln_hst( 3 , 0 );
                ERR_DEAL
                do_flag=1;
            }
        }
        /*贷款复利积数产生的利息*/
        if( g_pub_tx.svc_ind==3031 || g_pub_tx.svc_ind==3051 ) 
        {
            strcpy(g_pub_tx.brf,"计算复利积数利息");
            ret=pub_ins_ln_hst( 7 , 0 );
            ERR_DEAL
            do_flag=1;
        }
        /* 贷款五级认定 */
        if( g_pub_tx.svc_ind==3060 ) 
        {
             ret=pub_ins_ln_hst( 0 , 0 );
             ERR_DEAL
             do_flag=1;
        }
        /* 贷款结转--转出 */
        if( g_pub_tx.svc_ind==3063 ) 
        {
             ret=pub_ins_ln_hst( 0 , 0 );
             ERR_DEAL
             do_flag=1;
        }           
        /* 生成按揭还款计划 */
        if( g_pub_tx.svc_ind==3062 ) 
        {
             ret=pub_ins_ln_hst( 0 , 0 );
             ERR_DEAL
             do_flag=1;
        }       
        if( g_pub_tx.svc_ind==3059 ) 
        {
            ret=pub_ins_ln_hst( 1 , 0 );
            ERR_DEAL
            do_flag=1;
        }
        /* 抵押品入库 */
        if( g_pub_tx.svc_ind==3061 )
        {
            ret=pub_ins_ln_hst( 8 , 0 );
            ERR_DEAL
            do_flag=1;
        }
        if( g_pub_tx.hst_ind[0]=='1' )      /*日间入明细*/
        {
            if( g_pub_tx.svc_ind==3010 ) 
            {
                g_ln_mst.hst_bal=g_ln_mst.hst_bal+g_pub_tx.tx_amt1;
            }
        }
        if( g_pub_tx.svc_ind==3052 )
        {
            g_ln_mst.ic_date = g_pub_tx.tx_date ;   /* 修改起息日期 */
        }   
    }
    else if( g_pub_tx.add_ind[0]=='0' ) /*减少*/
    {
        if( g_pub_tx.svc_ind==3019 ) /*贷款本金减少，即还本*/
        {
            if( pub_base_CompDblVal(g_ln_mst.bal,g_pub_tx.tx_amt1)>=0 )
            {
                g_ln_mst.bal-=g_pub_tx.tx_amt1;
            }else{
                g_pub_tx.tx_amt1=g_ln_mst.bal;
                g_ln_mst.bal=0.00;
            }           

            ret=pub_ins_ln_hst( 0 , 0 );
            ERR_DEAL
            do_flag=1;
        }
        /****ligl****/
        if(g_pub_tx.svc_ind==3091)
        {
            if( pub_base_CompDblVal(g_ln_mst.bal,g_pub_tx.tx_amt1)>=0 )
            {
                g_ln_mst.bal-=g_pub_tx.tx_amt1;
            }else{
                /*
                g_pub_tx.tx_amt1-=g_ln_mst.bal;
                g_ln_mst.bal=0.00;*/
                sprintf(acErrMsg,
                    "超过发放限额：已放[%.2lf]新[%.2lf]>限额[%.2lf]",
                    g_ln_mst.ttl_prvd_amt,g_pub_tx.tx_amt1,g_ln_mst.amt_lmt);
                WRITEMSG
                strcpy( g_pub_tx.reply,"P122" );
                goto ErrExit;
            }           

            ret=pub_ins_ln_hst( 0 , 0 );
            ERR_DEAL
            do_flag=1;
        }
        /*********/
        if( g_pub_tx.svc_ind==3092 ) /*冲正贷款抵押品金额*/
        {
            g_ln_mst.gage_amt-=g_pub_tx.tx_amt1;
            ret=pub_ins_ln_hst( 8 , 0 );
            ERR_DEAL
            do_flag=1;
        }       
        /*贷款表内利息减少，即还欠息*/
        if( g_pub_tx.svc_ind==3059 ) 
        {
            ret=pub_ins_ln_hst( 1 , 0 );
            ERR_DEAL
            do_flag=1;
        }
        if( g_pub_tx.svc_ind==3071 ) 
        {
            if( pub_base_CompDblVal( g_pub_tx.tx_amt2 , 0.00 ) )
            {               
                g_ln_mst.in_lo_intst-=g_pub_tx.tx_amt2;
                if( pub_base_CompDblVal(g_ln_mst.in_lo_intst,0.00)<0 )
                {
                    sprintf(acErrMsg,"还息金额过大[%.2lf]>[%.2lf]",
                        g_pub_tx.tx_amt2,g_ln_mst.in_lo_intst );
                    WRITEMSG
                    strcpy( g_pub_tx.reply,"L039" );
                    goto ErrExit;
                }
                strcpy ( g_pub_tx.brf , "还按揭表内利息部分(旧)" );
                ret=pub_ins_ln_hst( 1 , 0 );
                ERR_DEAL
                do_flag=1;
            }
            do_flag=1;
        }       
        if( g_pub_tx.svc_ind==3072 ) 
        {
            if( pub_base_CompDblVal( g_pub_tx.tx_amt4 , 0.00 ) )
            {               
                g_ln_mst.out_lo_intst-=g_pub_tx.tx_amt4;
                if( pub_base_CompDblVal(g_ln_mst.out_lo_intst,0.00)<0 )
                {
                    sprintf(acErrMsg,"还息金额过大[%.2lf]>[%.2lf]",
                        g_pub_tx.tx_amt2,g_ln_mst.out_lo_intst );
                    WRITEMSG
                    strcpy( g_pub_tx.reply,"L039" );
                    goto ErrExit;
                }
                strcpy ( g_pub_tx.brf , "还按揭表外利息部分(旧)" );
                ret=pub_ins_ln_hst( 3 , 0 );
                ERR_DEAL
                do_flag=1;
            }
            do_flag=1;
        }
        if( g_pub_tx.svc_ind==3068 ) /*按揭贷款还利息*/
        {
            if( pub_base_CompDblVal(g_pub_tx.tx_amt2 , 0.00 ) )
            {
                strcpy ( g_pub_tx.brf,"按揭还款利息部分" );
                ret=pub_ins_ln_hst( 1 , 0 );
                ERR_DEAL
                do_flag=1;
            }
            
        }
        if( g_pub_tx.svc_ind==3069 ) /*按揭贷款还罚息*/
        {
            if( pub_base_CompDblVal(g_pub_tx.tx_amt4 , 0.00 ) )
            {
                strcpy ( g_pub_tx.brf,"按揭还款罚息部分" );
                ret=pub_ins_ln_hst( 3 , 0 );
                ERR_DEAL
                do_flag=1;
            }
            
        }
        if(g_pub_tx.svc_ind==3011||g_pub_tx.svc_ind==3012)/*本金积数产生的利息*/
        {
            strcpy( g_pub_tx.add_ind , "1" );
            strcpy( g_pub_tx.brf,"本金积数结息" );          
            ret=pub_ins_ln_hst( 11 , 0 );
            ERR_DEAL
            
            strcpy( g_pub_tx.add_ind , "0" );           
            strcpy( g_pub_tx.brf,"还本金积数" );            
            ret=pub_ins_ln_hst( 15 , 0 );
            ERR_DEAL
            
            do_flag=1;
        }               
        if( g_pub_tx.svc_ind==3010 ) /*贷款本金减少，即还本*/
        {
            sprintf(acErrMsg,"发生额和贷款余额[%.2lf][%.2lf]",
                    g_pub_tx.tx_amt1,g_ln_mst.bal );
            WRITEMSG            
            g_ln_mst.bal=g_ln_mst.bal-g_pub_tx.tx_amt1;
            if( pub_base_CompDblVal(g_ln_mst.bal,0.00)<0 )
            {
                TRACE
                sprintf(acErrMsg,"还本金额过大[%.2lf]>[%.2lf]",
                    g_pub_tx.tx_amt1,g_ln_mst.bal );
                WRITEMSG
                strcpy( g_pub_tx.reply,"L039" );
                goto ErrExit;
            }
            sprintf(acErrMsg,"发生额和贷款余额[%.2lf][%.2lf]",
                    g_pub_tx.tx_amt1,g_ln_mst.bal );
            WRITEMSG
            ret=pub_ins_ln_hst( 0 , 0 );
            sprintf(acErrMsg,"发生额和贷款余额[%.2lf][%.2lf]",
                    g_pub_tx.tx_amt1,g_ln_mst.bal );
            ERR_DEAL
            do_flag=1;
        }
        if( g_pub_tx.svc_ind==3018 ) /*还款计划变更*/
        {
            sprintf(acErrMsg,"发生额和贷款余额[%.2lf][%.2lf]",
                    g_pub_tx.tx_amt1,g_ln_mst.bal );
            g_ln_mst.amt_lmt = g_ln_mst.amt_lmt - g_pub_tx.tx_amt1;
            /****
            g_ln_mst.bal = g_ln_mst.bal - g_pub_tx.tx_amt1;
            ****/
            WRITEMSG            
            ret=pub_ins_ln_hst( 0 , 0 );
            ERR_DEAL
            do_flag=1;
        }       
        /*贷款表内利息减少，即还欠息*/
        if( g_pub_tx.svc_ind==3020 || g_pub_tx.svc_ind==3050 ) 
        {
            if( pub_base_CompDblVal( g_pub_tx.tx_amt2 , 0.00 ) )
            {
                g_ln_mst.in_lo_intst-=g_pub_tx.tx_amt2;
            
                if( pub_base_CompDblVal(g_ln_mst.in_lo_intst,0.00)<0 )
                {
                    sprintf(acErrMsg,"还息金额过大[%d][%.2lf]>[%.2lf]",
                        g_ln_mst.ac_id,g_pub_tx.tx_amt2,g_ln_mst.in_lo_intst );
                    WRITEMSG
                    strcpy( g_pub_tx.reply,"L039" );
                    goto ErrExit;
                }
                if( strlen(g_pub_tx.brf)==0 )
                {
                    strcpy ( g_pub_tx.brf , "还表内欠息" );
                }
                ret=pub_ins_ln_hst( 1 , 0 );
                ERR_DEAL
                do_flag=1;
            }
        }
        /*贷款表外利息减少，即还欠息*/
        if( g_pub_tx.svc_ind==3030 || g_pub_tx.svc_ind==3050 ) 
        {
            if( pub_base_CompDblVal( g_pub_tx.tx_amt3 , 0.00 ) )
            {
                g_ln_mst.out_lo_intst-=g_pub_tx.tx_amt3;
                if( pub_base_CompDblVal(g_ln_mst.out_lo_intst,0.00)<0 )
                {
                    sprintf(acErrMsg,"还息金额过大[%.2lf]>[%.2lf]",
                        g_pub_tx.tx_amt2,g_ln_mst.out_lo_intst );
                    WRITEMSG
                    strcpy( g_pub_tx.reply,"L039" );
                    goto ErrExit;
                }
                if( strlen(g_pub_tx.brf)==0 )
                {
                    strcpy ( g_pub_tx.brf , "还表外欠息" );         
                }
                ret=pub_ins_ln_hst( 2 , 0 );
                ERR_DEAL
                do_flag=1;
            }
        }
        /*贷款复利减少，即还欠息*/
        if( g_pub_tx.svc_ind==3040 || g_pub_tx.svc_ind==3050 ) 
        {
            TRACE
            sprintf(acErrMsg,"金额[%.2lf]",
                    g_pub_tx.tx_amt4,g_ln_mst.cmpd_lo_intst );
            WRITEMSG

            if( pub_base_CompDblVal( g_pub_tx.tx_amt4 , 0.00 ) )
            {
                TRACE
                sprintf(acErrMsg,"!!!!金额[%.2lf]",
                            g_pub_tx.tx_amt4,g_ln_mst.cmpd_lo_intst );
                WRITEMSG

                g_ln_mst.cmpd_lo_intst-=g_pub_tx.tx_amt4;

                if( pub_base_CompDblVal(g_ln_mst.cmpd_lo_intst,0.00)<0 )
                {
                    sprintf(acErrMsg,"还息金额过大[%.2lf]>[%.2lf]",
                        g_pub_tx.tx_amt2,g_ln_mst.cmpd_lo_intst );
                    WRITEMSG
                    strcpy( g_pub_tx.reply,"L039" );
                    goto ErrExit;
                }
                if( strlen(g_pub_tx.brf)==0 )
                {
                    strcpy ( g_pub_tx.brf , "还复利欠息" );         
                }
                ret=pub_ins_ln_hst( 3 , 0 );
                ERR_DEAL
                do_flag=1;
            }
        }
        /*贷款表内积数产生的利息*/
        if( g_pub_tx.svc_ind==3051 ) 
        {
            strcpy( g_pub_tx.add_ind , "1" );
            strcpy ( g_pub_tx.brf,"表内积数结息" );
            ret=pub_ins_ln_hst( 12 , 0 );
            ERR_DEAL            
            
            strcpy( g_pub_tx.add_ind , "0" );
            strcpy ( g_pub_tx.brf,"还表内积数利息" );
            ret=pub_ins_ln_hst( 5 , 0 );
            ERR_DEAL
            do_flag=1;

        }
        /*贷款表外积数产生的利息*/
        if( g_pub_tx.svc_ind==3051 ) 
        {
            strcpy( g_pub_tx.add_ind , "1" );
            strcpy ( g_pub_tx.brf,"表外积数结息" );
            ret=pub_ins_ln_hst( 13 , 0 );
            ERR_DEAL
            
            strcpy( g_pub_tx.add_ind , "0" );
            strcpy ( g_pub_tx.brf,"还表外积数利息" );
            ret=pub_ins_ln_hst( 6 , 0 );
            ERR_DEAL
            do_flag=1;

        }
        /*贷款复利积数产生的利息*/
        if( g_pub_tx.svc_ind==3051 ) 
        {
            strcpy( g_pub_tx.add_ind , "1" );
            strcpy ( g_pub_tx.brf,"复利积数结息" );
            ret=pub_ins_ln_hst( 14 , 0 );
            ERR_DEAL

            strcpy( g_pub_tx.add_ind , "0" );
            strcpy ( g_pub_tx.brf,"还复利积数利息" );
            ret=pub_ins_ln_hst( 7 , 0 );
            ERR_DEAL
            do_flag=1;
        }       
        /* 贷款五级认定 */
        if( g_pub_tx.svc_ind==3060 )
        {
            ret=pub_ins_ln_hst( 0 , 0 );
            ERR_DEAL
            do_flag=1;
        }
        /* 贷款结转--转出 */
        if( g_pub_tx.svc_ind==3063 ) 
        {
             ret=pub_ins_ln_hst( 0 , 0 );
             ERR_DEAL
             do_flag=1;
        }       
        /* 抵押品出库 */
        if( g_pub_tx.svc_ind==3061 )
        {
            ret=pub_ins_ln_hst( 8 , 0 );
            ERR_DEAL
            do_flag=1;
        }       
        if( g_pub_tx.hst_ind[0]=='1' )      /*日间入明细*/
        {
            if( g_pub_tx.svc_ind==3019 || g_pub_tx.svc_ind==3010 ||
                                                g_pub_tx.svc_ind==3018)
            {
                g_ln_mst.hst_bal=g_ln_mst.hst_bal-g_pub_tx.tx_amt1;
            }
        }
        /***3013晋中增加  委托贷款还息交易，增加贷款明细表数据******/
        if( g_pub_tx.svc_ind==3013 ) 
        {       		
        		get_zd_double("102F",&g_pub_tx.tx_amt1);  /*交易金额 */
						sprintf(acErrMsg,"委托贷款还息取款金额![%.2lf]",g_pub_tx.tx_amt1);
						WRITEMSG
        		ret=pub_ins_ln_hst( 1 , 1 );
            ERR_DEAL
            do_flag=1;
        }
    }
    else if( g_pub_tx.svc_ind==2 )
    {
        do_flag=1;
    }
    else
    {
        sprintf(acErrMsg,"增减标志错误[%s]",g_pub_tx.add_ind);
        WRITEMSG
        strcpy( g_pub_tx.reply,"S048" );
        goto ErrExit;
    }

    if( do_flag==0 )
    {
        sprintf(acErrMsg,"标志错误[%s][%d]",g_pub_tx.add_ind,g_pub_tx.svc_ind);
        WRITEMSG
        strcpy( g_pub_tx.reply,"S048" );
        goto ErrExit;
    }

    /**贷款销户，0金额销户**/
    if( g_pub_tx.svc_ind==2 )
    {
        /*  modified by liuxuan 2006-11-28 贷款销户在销户交易里已经检查过了
        if( pub_base_CompDblVal(g_ln_mst.bal,0.00)
            ||  pub_base_CompDblVal(g_ln_mst.intst_acm,0.00)
            ||  pub_base_CompDblVal(g_ln_mst.in_lo_intst,0.00)
            ||  pub_base_CompDblVal(g_ln_mst.in_lo_acm,0.00)
            ||  pub_base_CompDblVal(g_ln_mst.out_lo_intst,0.00)
            ||  pub_base_CompDblVal(g_ln_mst.out_lo_acm,0.00)
            ||  pub_base_CompDblVal(g_ln_mst.cmpd_lo_intst,0.00)
            ||  pub_base_CompDblVal(g_ln_mst.cmpd_lo_acm,0.00) )
        {
            sprintf(acErrMsg,"贷款未还清");
            WRITEMSG
            strcpy( g_pub_tx.reply,"L068" );
            goto ErrExit;
        }
        */
            if(g_pub_tx.add_ind[0]=='2')    /* 贷款销户重置了这个标志后得在这里改回来 */
                g_pub_tx.add_ind[0]='0';
            ret=pub_ins_ln_hst( 0 , 0 );
            ERR_DEAL
            do_flag=1;
    }
            sprintf(acErrMsg,"发生额和贷款余额[%.2lf][%.2lf]",
                    g_pub_tx.tx_amt1,g_ln_mst.bal );
    ret=Ln_mst_Upd_Upd(g_ln_mst,g_pub_tx.reply);
    if( ret )
    {
        sprintf(acErrMsg,"账号不存在[%d][%d]",g_pub_tx.ac_id,ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"P102");
        goto ErrExit;
    }
            sprintf(acErrMsg,"发生额和贷款余额[%.2lf][%.2lf]",
                    g_pub_tx.tx_amt1,g_ln_mst.bal );
    Ln_mst_Clo_Upd();
            sprintf(acErrMsg,"发生额和贷款余额[%.2lf][%.2lf]",
                    g_pub_tx.tx_amt1,g_ln_mst.bal );
OkExit:
    return 0;
ErrExit:
    return 1;
}

int Chg_in_mst()
{
    struct com_item_c v_com_item;
    int ret;
    /*** 调用内部产品参数各项检查函数 ***/

    /*** 内部主文件检查 ***/
    ret=In_mst_Dec_Upd(g_pub_tx.reply,"ac_id='%ld' and ac_seqn='%d' ",
        g_pub_tx.ac_id,g_pub_tx.ac_seqn);
    if( ret )
    {
        sprintf(acErrMsg,"账号不存在[%d][%d]",g_pub_tx.ac_id,ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"P102");
        goto ErrExit;
    }

    ret=In_mst_Fet_Upd(&g_in_mst,g_pub_tx.reply);
    if( ret )
    {
        sprintf(acErrMsg,"账号不存在[%d][%d]",g_pub_tx.ac_id,ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"A029");
        goto ErrExit;
    }

    strcpy( g_pub_tx.opn_br_no,g_in_mst.opn_br_no );
vtcp_log("%s,%d tx_br_no=[%s],opn_br_no=[%s]",__FILE__,__LINE__,g_pub_tx.tx_br_no,g_pub_tx.opn_br_no);
    set_zd_data("0020", g_pub_tx.opn_br_no);
    strcpy(g_pub_tx.name,g_in_mst.name);
    /**读取产品参数**/
    ret = In_parm_Sel( g_pub_tx.reply,&g_in_parm,
            " prdt_no='%s' ",g_in_mst.prdt_no );
    ERR_DEAL

    strcpy(g_pub_tx.cur_no,g_in_parm.cur_no);
    strcpy(g_pub_tx.dc_code,g_in_parm.dc_code);             /*会计代码*/
    strcpy(g_pub_tx.prdt_code,g_in_mst.prdt_no);            /*会计代码*/

    /**读取科目定义**/
    ret = Com_item_Sel( g_pub_tx.reply,&v_com_item,
            " acc_hrt='%s' ",g_in_parm.acc_hrt );
    ERR_DEAL

sprintf(acErrMsg,"yefx[%s][%s]",v_com_item.dc_ind,g_in_parm.acc_hrt);
WRITEMSG

    if( g_in_mst.sts[0]!='1' && g_in_mst.sts[0]!='2' )
    {
        sprintf(acErrMsg,"账号状态非正常状态![%ld,%s]",g_in_mst.ac_id,g_in_mst.sts);
        WRITEMSG
        strcpy(g_pub_tx.reply,"P109");
        goto ErrExit;
    }

    g_in_mst.lst_date=g_pub_tx.tx_date;

sprintf(acErrMsg, "内部: add_ind =[%s]", g_pub_tx.add_ind);
WRITEMSG

    if( g_pub_tx.add_ind[0]=='0' )  /*借*/
    {
        if( g_in_mst.sts[0]=='2' && v_com_item.dc_ind[0]=='2' )
        {
            sprintf(acErrMsg,"该账户已经反向冻结!sts[%s],add_ind[%s],dc[%s]",
                g_in_mst.sts,g_pub_tx.add_ind,v_com_item.dc_ind);
            WRITEMSG
            strcpy(g_pub_tx.reply,"P125");
            goto ErrExit;
        }
        if( v_com_item.amt_dc_ind[0]=='2' )
        {
            sprintf(acErrMsg,"科目发生额方向贷方");
            WRITEMSG
            strcpy(g_pub_tx.reply,"A025");
            goto ErrExit;
        }
        if( g_in_parm.amt_dc[0]=='2' )
        {
            sprintf(acErrMsg,"产品发生额方向贷方");
            WRITEMSG
            strcpy(g_pub_tx.reply,"A027");
            goto ErrExit;
        }

        if( v_com_item.dc_ind[0]=='2' )
        {
            /*** 调用计算利息积数函数 ***/
            if( pub_base_CalAcm(g_in_mst.lst_date,g_pub_tx.tx_date,
                g_in_parm.cal_day_type,g_in_parm.acm_type,
                g_in_mst.bal,-g_pub_tx.tx_amt1,&g_in_mst.intst_acm,
                g_in_parm.intst_knd,g_in_parm.intst_mon,g_in_parm.intst_date) )
            {
                sprintf(acErrMsg,"计算利息积数错误!");
                WRITEMSG
                goto ErrExit;
            }

            g_in_mst.bal=g_in_mst.bal-g_pub_tx.tx_amt1;
            if( g_pub_tx.hst_ind[0]=='1' )      /*日间入明细*/
                g_in_mst.hst_bal=g_in_mst.hst_bal-g_pub_tx.tx_amt1;

            strcpy(g_pub_tx.add_ind,"0");
        }
        else
        {
            /*** 调用计算利息积数函数 ***/
            if( pub_base_CalAcm(g_in_mst.lst_date,g_pub_tx.tx_date,
                g_in_parm.cal_day_type,g_in_parm.acm_type,
                g_in_mst.bal,g_pub_tx.tx_amt1,&g_in_mst.intst_acm,
                g_in_parm.intst_knd,g_in_parm.intst_mon,g_in_parm.intst_date) )
            {
                sprintf(acErrMsg,"计算利息积数错误!");
                WRITEMSG
                goto ErrExit;
            }

            g_in_mst.bal=g_in_mst.bal+g_pub_tx.tx_amt1;
            if( g_pub_tx.hst_ind[0]=='1' )      /*日间入明细*/
                g_in_mst.hst_bal=g_in_mst.hst_bal+g_pub_tx.tx_amt1;
            strcpy(g_pub_tx.add_ind,"1");
        }
    }
    else if ( g_pub_tx.add_ind[0]=='1' )    /*贷*/
    {
        if( g_in_mst.sts[0]=='2' && v_com_item.dc_ind[0]=='1' )
        {
            sprintf(acErrMsg,"该账户已经反向冻结!sts[%s],add_ind[%s],dc[%s]",
                g_in_mst.sts,g_pub_tx.add_ind,v_com_item.dc_ind);
            WRITEMSG
            strcpy(g_pub_tx.reply,"P125");
            goto ErrExit;
        }
        if( v_com_item.amt_dc_ind[0]=='1' )
        {
            sprintf(acErrMsg,"科目发生额方向借方");
            WRITEMSG
            strcpy(g_pub_tx.reply,"A024");
            goto ErrExit;
        }
        if( g_in_parm.amt_dc[0]=='1' )
        {
            sprintf(acErrMsg,"产品发生额方向借方");
            WRITEMSG
            strcpy(g_pub_tx.reply,"A026");
            goto ErrExit;
        }
        if( v_com_item.dc_ind[0]=='2' )
        {
            /*** 调用计算利息积数函数 ***/
            if( pub_base_CalAcm(g_in_mst.lst_date,g_pub_tx.tx_date,
                g_in_parm.cal_day_type,g_in_parm.acm_type,
                g_in_mst.bal,g_pub_tx.tx_amt1,&g_in_mst.intst_acm,
                g_in_parm.intst_knd,g_in_parm.intst_mon,g_in_parm.intst_date) )
            {
                sprintf(acErrMsg,"计算利息积数错误!");
                WRITEMSG
                goto ErrExit;
            }
            g_in_mst.bal=g_in_mst.bal+g_pub_tx.tx_amt1;
            if( g_pub_tx.hst_ind[0]=='1' )      /*日间入明细*/
                g_in_mst.hst_bal=g_in_mst.hst_bal+g_pub_tx.tx_amt1;
            strcpy(g_pub_tx.add_ind,"1");
        }
        else
        {
            /*** 调用计算利息积数函数 ***/
            if( pub_base_CalAcm(g_in_mst.lst_date,g_pub_tx.tx_date,
                g_in_parm.cal_day_type,g_in_parm.acm_type,
                g_in_mst.bal,-g_pub_tx.tx_amt1,&g_in_mst.intst_acm,
                g_in_parm.intst_knd,g_in_parm.intst_mon,g_in_parm.intst_date) )
            {
                sprintf(acErrMsg,"计算利息积数错误!");
                WRITEMSG
                goto ErrExit;
            }
            g_in_mst.bal=g_in_mst.bal-g_pub_tx.tx_amt1;
            if( g_pub_tx.hst_ind[0]=='1' )      /*日间入明细*/
                g_in_mst.hst_bal=g_in_mst.hst_bal-g_pub_tx.tx_amt1;
            strcpy(g_pub_tx.add_ind,"0");
        }
    }
    else
    {
        sprintf(acErrMsg,"存取标志错误g_pub_tx.add_ind=[%s]!",g_pub_tx.add_ind);
        WRITEMSG
        strcpy(g_pub_tx.reply,"P106");
        goto ErrExit;
    }

sprintf(acErrMsg, "内部: add_ind =[%s]", g_pub_tx.add_ind);
WRITEMSG
        if( g_pub_tx.svc_ind==9002 ) /**销户**/
        {
            strcpy(g_in_mst.sts,"*");
            if( pub_base_CompDblVal(g_in_mst.bal,0.00)  )
            {
                sprintf(acErrMsg,"销户后余额不为零!");
                WRITEMSG
                strcpy(g_pub_tx.reply,"P116");
                goto ErrExit;
            }
            strcpy(g_pub_tx.add_ind,"0");
        }

        if( pub_base_CompDblVal(g_in_mst.bal,0.00)<0 
            && g_in_mst.od_ind[0]=='N')
        {
            sprintf(acErrMsg,"该账户[%ld]余额不允许透支![%.2f][%.2f]",
                g_in_mst.ac_id,g_in_mst.bal,g_pub_tx.tx_amt1);
            WRITEMSG
            strcpy(g_pub_tx.reply,"P124");
            goto ErrExit;
        }
        if( g_in_mst.od_ind[0]=='Y' 
            && pub_base_CompDblVal(g_in_parm.od_amt,0.00)
            && pub_base_CompDblVal(-g_in_mst.bal,g_in_parm.od_amt)>0 )
        {
            sprintf(acErrMsg,"透支超限![%.2f][%.2f]",
                g_in_mst.bal,g_in_parm.od_amt );
            WRITEMSG
            strcpy(g_pub_tx.reply,"T004");
            goto ErrExit;
        }
        vtcp_log("%s,%d 准备写入历史明细金额 ===%f",__FILE__,__LINE__,g_pub_tx.tx_amt1);
        if( g_pub_tx.hst_ind[0]=='1' )      /*日间入明细*/
        {
            ret=pub_ins_in_hst();
            ERR_DEAL
        }

    ret=In_mst_Upd_Upd(g_in_mst,g_pub_tx.reply);
    if( ret )
    {
        sprintf(acErrMsg,"账号不存在[%d][%d]",g_pub_tx.ac_id,ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"P102");
        goto ErrExit;
    }
    In_mst_Clo_Upd();

OkExit:
    return 0;
ErrExit:
    return 1;
}
/**对科目记账**/
int Chg_acchrt_mst(int *ass_ind)
{
    int ret=0; 
    double amt;
    char wherelist[200];
    int i=0;
    char TmpAdd_ind[2];
    char brno[6];

    struct gl_mst_c v_gl_mst;
    struct com_item_c v_com_item;
    struct tx_dc_br_rel_c v_tx_dc_br_rel;

    memset(&v_gl_mst,0x00,sizeof(struct gl_mst_c));
    memset(&v_com_item,0x00,sizeof(struct com_item_c));
    memset(brno,0x00,sizeof(brno));

    sprintf(acErrMsg,"存取标志=[%s]!",g_pub_tx.add_ind);
    WRITEMSG

    if(memcmp(g_pub_tx.tx_code,"3206",4)!=0
        &&memcmp(g_pub_tx.tx_code,"3207",4)!=0
        &&memcmp(g_pub_tx.tx_code,"5402",4)!=0
        &&memcmp(g_pub_tx.tx_code,"5404",4)!=0
        &&memcmp(g_pub_tx.tx_code,"5202",4)!=0
        &&memcmp(g_pub_tx.tx_code,"5840",4)!=0
        &&memcmp(g_pub_tx.tx_code,"5842",4)!=0
        &&memcmp(g_pub_tx.tx_code,"5844",4)!=0
        &&memcmp(g_pub_tx.tx_code,"5846",4)!=0
        &&memcmp(g_pub_tx.tx_code,"5848",4)!=0
        &&memcmp(g_pub_tx.tx_code,"5849",4)!=0
        &&memcmp(g_pub_tx.tx_code,"5855",4)!=0
        &&memcmp(g_pub_tx.tx_code,"5856",4)!=0
        &&memcmp(g_pub_tx.tx_code,"D304",4)!=0
        &&memcmp(g_pub_tx.tx_code,"D302",4)!=0
        &&memcmp(g_pub_tx.tx_code,"D131",4)!=0
        &&memcmp(g_pub_tx.tx_code,"5901",4)!=0
        &&memcmp(g_pub_tx.tx_code,"5709",4)!=0
        &&memcmp(g_pub_tx.tx_code,"F000",4)!=0/*add fupengfei  2014/6/30 二代支付单笔冲账，批量记账、批量冲账、主机对账时不需要更改开户机构为交易机构*/
        &&memcmp(g_pub_tx.tx_code,"F059",4)!=0
        &&memcmp(g_pub_tx.tx_code,"7023",4)!=0
        &&memcmp(g_pub_tx.tx_code,"6254",4)!=0
        &&memcmp(g_pub_tx.tx_code,"6247",4)!=0
        &&memcmp(g_pub_tx.tx_code,"7022",4)!=0
        &&memcmp(g_pub_tx.tx_code,"7024",4)!=0/*add fupengfei  2014/6/30*/
        &&memcmp(g_pub_tx.tx_code,"7028",4)!=0
        &&memcmp(g_pub_tx.tx_code,"6238",4)!=0
        &&memcmp(g_pub_tx.tx_code,"2225",4)!=0   /*add by lwb 20150420*/
        )  /****此条对财务扣划错帐处理 20070624****/
        strcpy( g_pub_tx.opn_br_no,g_pub_tx.tx_br_no );
        vtcp_log("%s,%d tx_code=[%s] opn_br_no===[%s]",__FILE__,__LINE__, g_pub_tx.tx_code, g_pub_tx.opn_br_no);
    
    /*add beg by ChengGX 晋中 2010-1-26 11:09 */
   /** if(memcmp(g_pub_tx.tx_code,"6203",4)==0 && memcmp(g_pub_tx.ac_no,"41404",5)==0 && memcmp(g_pub_tx.sub_tx_code,"A016",4)==0) **/
   vtcp_log("%s,%d g_pub_tx.ac_no===[%s]g_pub_tx.sub_tx_code[%s]",__FILE__,__LINE__,g_pub_tx.ac_no,g_pub_tx.sub_tx_code);
   vtcp_log("%s,%d g_pub_tx.tx_code[%s]",__FILE__,__LINE__,g_pub_tx.tx_code);
   if(memcmp(g_pub_tx.ac_no,"41404",5)==0 && (memcmp(g_pub_tx.sub_tx_code,"A016",4)==0 || memcmp(g_pub_tx.sub_tx_code,"A017",4)==0 || memcmp(g_pub_tx.tx_code,"Z129",4)==0 || memcmp(g_pub_tx.tx_code,"CFEE",4)==0 )) 
    {
        vtcp_log("%s,%d opn_br_no===[%s]",__FILE__,__LINE__,g_pub_tx.opn_br_no);
    memcpy(g_pub_tx.opn_br_no,"10001",5);/*晋中的41404科目记账都记到清算中心*/
        vtcp_log("%s,%d opn_br_no===[%s]",__FILE__,__LINE__,g_pub_tx.opn_br_no);
    
    }
    /*add beg by ChengGX 晋中 2010-1-26 11:09 */
    /*Add by hxc晋中 20100203  */
    /**Mod by hxc 101218 修改记账科目5111203->5111401**/
    if((memcmp(g_pub_tx.ac_no,"52704",5)==0 || memcmp(g_pub_tx.ac_no,"5111401",7)==0 || memcmp(g_pub_tx.ac_no,"5111601",7)==0 ) && (memcmp(g_pub_tx.sub_tx_code,"F071",4)==0 || memcmp(g_pub_tx.sub_tx_code,"F000",4)==0) ) 
    {
            memset(brno,0x00,sizeof(brno));
            get_zd_data("0770",brno);
            vtcp_log("%s,%d brno===[%s]",__FILE__,__LINE__,brno);
            memcpy(g_pub_tx.opn_br_no,brno,5);/*晋中的他代本类ATM收入支出科目记账都记到各支行*/
    }
    /*End by hxc晋中 20100203*/
    /**网银交易小额业务41403科目记账记到清算中心 20130922 hzh start**/
   if(memcmp(g_pub_tx.ac_no,"41403",5)==0 && memcmp(g_pub_tx.tx_code,"6388",4)==0) 
    {
        vtcp_log("%s,%d opn_br_no===[%s]",__FILE__,__LINE__,g_pub_tx.opn_br_no);
        memcpy(g_pub_tx.opn_br_no,"10001",5);/*晋中的41403科目记账都记到清算中心*/
        vtcp_log("%s,%d opn_br_no===[%s]",__FILE__,__LINE__,g_pub_tx.opn_br_no);
    
    }
    /**网银交易小额业务41403科目记账记到清算中心 20130922 hzh   end**/
   /**二代支付小额业务41403科目记账记到清算中心 2014/7/11 fupengfei start**/
   vtcp_log("%s,%d !!!ac_no===[%s]",__FILE__,__LINE__,g_pub_tx.ac_no);
   vtcp_log("%s,%d !!!tx_code===[%s]",__FILE__,__LINE__,g_pub_tx.tx_code);
   if(memcmp(g_pub_tx.ac_no,"41403",5)==0 && (memcmp(g_pub_tx.tx_code,"6236",4)==0 || 
   	memcmp(g_pub_tx.tx_code,"7023",4)==0 || memcmp(g_pub_tx.tx_code,"7021",4)==0 || 
   	memcmp(g_pub_tx.tx_code,"7022",4)==0 || memcmp(g_pub_tx.tx_code,"7024",4)==0))
    {
        vtcp_log("%s,%d opn_br_no===[%s]",__FILE__,__LINE__,g_pub_tx.opn_br_no);
        memcpy(g_pub_tx.opn_br_no,"10001",5);/*晋中的41403科目记账都记到清算中心*/
        vtcp_log("%s,%d opn_br_no===[%s]",__FILE__,__LINE__,g_pub_tx.opn_br_no);
    
    }
    /**二代支付小额业务41403科目记账记到清算中心 2014/7/11 fupengfei   end**/
	
	/*** 同城清算业务41401科目记账到清算中心 ***/    
	if (memcmp(g_pub_tx.ac_no, "41401", 5) == 0 && 
		(memcmp(g_pub_tx.tx_code, "6238", 4) == 0 || 
		memcmp(g_pub_tx.tx_code, "7028", 4) == 0 ||
		memcmp(g_pub_tx.tx_code, "6253", 4) == 0 ||
		memcmp(g_pub_tx.tx_code, "6254", 4) == 0))
	{
		vtcp_log("[%s][%d] opn_br_no===[%s]", __FILE__, __LINE__, g_pub_tx.opn_br_no);
		memcpy(g_pub_tx.opn_br_no, "10001", 5);
		vtcp_log("[%s][%d] opn_br_no===[%s]", __FILE__, __LINE__, g_pub_tx.opn_br_no);
	}

    set_zd_data("0020", g_pub_tx.opn_br_no);
    vtcp_log("%s,%d opn_br_no===[%s]",__FILE__,__LINE__,g_pub_tx.opn_br_no);
    strcpy( g_pub_tx.prdt_code,"" );

    pub_base_strpack(g_pub_tx.cur_no);
    if( !strlen(g_pub_tx.cur_no) ) /**默认为人民币**/
        strcpy( g_pub_tx.cur_no,"01" );

    /*检查科目类型*/
    pub_base_strpack(g_pub_tx.ac_no);
    ret=Com_item_Sel(g_pub_tx.reply,&v_com_item,"acc_no='%s'",g_pub_tx.ac_no);
    if( ret==100 )
    {
        sprintf(acErrMsg,"该科目号不存在[%s]",g_pub_tx.ac_no);
        WRITEMSG
        strcpy(g_pub_tx.reply,"P102");
        goto ErrExit;
    }
    else if( ret )
    {
        sprintf(acErrMsg,"查询公共科目表异常[%s]",g_pub_tx.ac_no);
        WRITEMSG
        goto ErrExit;
    }
    strcpy(g_pub_tx.name,v_com_item.acc_name);

    /*科目类型必须为无帐户类型*/
    if(v_com_item.acc_knd[0]!='3')
    {
        sprintf(acErrMsg,"该科目类型不是无帐户科目!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"O073");
        goto ErrExit;
    }
    /**必须无下级科目**/
    if(v_com_item.sub_acc_yn[0]=='Y')
    {
        sprintf(acErrMsg,"该科目还有下级科目!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"O073");
        goto ErrExit;
    }

    strcpy( g_pub_tx.dc_code,"ACHR" );
    set_zd_data("0152",v_com_item.acc_hrt );
    sprintf(acErrMsg, "CGH********acc_hrt[%s]", v_com_item.acc_hrt);
    WRITEMSG

    switch(g_pub_tx.add_ind[0])                 /*交易借贷类型*/
    {   
        case '0': /**借方记账**/
            if(v_com_item.amt_dc_ind[0]=='2')
            {
                if(!memcmp(g_pub_tx.tx_code,"5839",4) && g_pub_tx.tx_amt1==0)
                {
                /** add by LiuHuafeng 大额来帐业务如果金额为0可以不考虑 **/
                    vtcp_log("%s,%d txcode=[%s],tx_amt1=[%f]",__FILE__,__LINE__,g_pub_tx.tx_code,g_pub_tx.tx_amt1);
                }
                else
                {
                sprintf(acErrMsg,"此科目只能记贷方帐![%s]",g_pub_tx.ac_no);
                WRITEMSG
                strcpy(g_pub_tx.reply,"A025");
                goto ErrExit;
                }
            }
            if(v_com_item.dc_ind[0]=='1' || v_com_item.dc_ind[0]=='0')
                strcpy(TmpAdd_ind,"1");     /** 增加 **/
            else
                strcpy(TmpAdd_ind,"0");     /** 减少 **/
            break;
        case '1': /*贷方记账*/
            if(v_com_item.amt_dc_ind[0]=='1')
            {
                if(!memcmp(g_pub_tx.tx_code,"5839",4) && g_pub_tx.tx_amt1==0)
                {
                    vtcp_log("%s,%d txcode=[%s],tx_amt1=[%f]",__FILE__,__LINE__,g_pub_tx.tx_code,g_pub_tx.tx_amt1);
                /** add by LiuHuafeng 大额来帐业务如果金额为0可以不考虑 **/
                }
                else
                {
                sprintf(acErrMsg,"此科目只能记借方帐![%s]",g_pub_tx.ac_no);
                WRITEMSG
                strcpy(g_pub_tx.reply,"A024");
                goto ErrExit;
                }
            }
            if(v_com_item.dc_ind[0]=='2')
                strcpy(TmpAdd_ind,"1");     /** 增加 **/
            else
                strcpy(TmpAdd_ind,"0");     /** 减少 **/
            break;
        default:
            sprintf(acErrMsg,"存取标志错误=[%s]!",g_pub_tx.add_ind);
            WRITEMSG
            strcpy(g_pub_tx.reply,"P106");
            goto ErrExit;
            break;
    }

    if( v_com_item.in_ind[0]=='A' )     /*** 该科目是考核科目***/
        *ass_ind=1;
    else
    {
        *ass_ind=0;
        /*** 将借贷标志转换为增减标志 ***/
        strcpy(g_pub_tx.add_ind,TmpAdd_ind);
    }

    sprintf(acErrMsg, "增减标志为: [%s]", g_pub_tx.add_ind);
    WRITEMSG

OkExit:
    return 0;
ErrExit:
    return 1;
}
/**对科目记账  旧的实时改总帐**/
int Chg_acchrt_mst_old()
{
    int ret=0; 
    double amt;
    char wherelist[200];
    int i=0;

    struct gl_mst_c v_gl_mst;
    struct com_item_c v_com_item;
    struct com_item_c u_com_item;
    struct tx_dc_br_rel_c v_tx_dc_br_rel;

    memset(&v_gl_mst,0x00,sizeof(struct gl_mst_c));
    memset(&v_com_item,0x00,sizeof(struct com_item_c));

                sprintf(acErrMsg,"存取标志错误=[%s]!",g_pub_tx.add_ind);
                WRITEMSG

    pub_base_strpack(g_pub_tx.cur_no);
    if( !strlen(g_pub_tx.cur_no) ) /**默认为人民币**/
        strcpy( g_pub_tx.cur_no,"01" );
    /*检查科目类型*/
    pub_base_strpack(g_pub_tx.ac_no);
    ret=Com_item_Sel(g_pub_tx.reply,&v_com_item,"acc_no='%s'",g_pub_tx.ac_no);
    if( ret==100 )
    {
        sprintf(acErrMsg,"该科目号不存在[%s]",g_pub_tx.ac_no);
        WRITEMSG
        strcpy(g_pub_tx.reply,"P102");
        goto ErrExit;
    }
    else if( ret )
    {
        sprintf(acErrMsg,"查询公共科目表异常[%s]",g_pub_tx.ac_no);
        WRITEMSG
        goto ErrExit;
    }

    /*科目类型必须为无帐户类型*/
    if(v_com_item.acc_knd[0]!='3')
    {
        sprintf(acErrMsg,"该科目类型不是无帐户科目!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"O073");
        goto ErrExit;
    }
    /**必须无下级科目**/
    if(v_com_item.sub_acc_yn[0]=='Y')
    {
        sprintf(acErrMsg,"该科目还有下级科目!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"O073");
        goto ErrExit;
    }

    /**取得对应的会计机构**/
    ret=Tx_dc_br_rel_Sel(g_pub_tx.reply,&v_tx_dc_br_rel,"tx_br_no='%s'",
        g_pub_tx.tx_br_no );
    if( ret )
    {
        sprintf(acErrMsg,"交易机构无对应会计机构[%s]",g_pub_tx.tx_br_no);
        WRITEMSG
        strcpy(g_pub_tx.reply,"S029");
        goto ErrExit;
    }

    memset( wherelist,0,sizeof(wherelist) );
    memcpy( &u_com_item,&v_com_item,sizeof(u_com_item) );
    pub_base_strpack( u_com_item.acc_hrt );
    pub_base_strpack( u_com_item.up_acc_hrt );

    strcpy( g_pub_tx.dc_code,"ACHR" );
    set_zd_data("0152",v_com_item.acc_hrt );
    sprintf(acErrMsg, "CGH********acc_hrt[%s]", v_com_item.acc_hrt);
    WRITEMSG

    sprintf( wherelist,"br_no='%s' and cur_no='%s' and acc_hrt in('%s'",
        v_tx_dc_br_rel.dc_br_no,g_pub_tx.cur_no, u_com_item.acc_hrt );
    /**取得上级科目**/
    for ( i=0;i<2;i++ )
    {
        if( !strcmp(u_com_item.acc_hrt,u_com_item.up_acc_hrt) )
            break;

        ret=Com_item_Sel(g_pub_tx.reply,&u_com_item,"acc_hrt='%s'",
            u_com_item.up_acc_hrt);
        if( ret )
        {
            sprintf(acErrMsg,"该科目控制字不存在[%s]",u_com_item.up_acc_hrt);
            WRITEMSG
            strcpy(g_pub_tx.reply,"P102");
            goto ErrExit;
        }
        pub_base_strpack( u_com_item.acc_hrt );
        pub_base_strpack( u_com_item.up_acc_hrt );
        strcat( wherelist,",'" );
        strcat( wherelist,u_com_item.acc_hrt );
        strcat( wherelist,"'" );
    }

    strcat( wherelist,")" );

    sprintf( acErrMsg,"GL_MST where [%s]",wherelist );
    WRITEMSG

    /*修改总帐*/
    ret=Gl_mst_Dec_Upd(g_pub_tx.reply,wherelist);
    if( ret )
    {
        sprintf(acErrMsg,"查询总帐异常!![%s]",v_com_item.acc_hrt);
        WRITEMSG
        goto ErrExit;
    }

    i=0;
    while( 1 )
    {
        ret=Gl_mst_Fet_Upd(&v_gl_mst,g_pub_tx.reply);
        if( ret==100 ) break;
        if( ret )
        {
            sprintf(acErrMsg,"查询总帐异常!![%s]",v_com_item.acc_hrt);
            WRITEMSG
            goto ErrExit;
        }
        i++;

        switch(g_pub_tx.add_ind[0])                 /*交易借贷类型*/
        {   
            case '0': /**借方记账**/
                if(v_com_item.amt_dc_ind[0]=='2')
                {
                    sprintf(acErrMsg,"此科目只能记贷方帐![%s]",g_pub_tx.ac_no);
                    WRITEMSG
                    strcpy(g_pub_tx.reply,"A025");
                    goto ErrExit;
                }

                if(v_com_item.dc_ind[0]=='1')               /*借处理*/
                {
                    v_gl_mst.dr_bal+=g_pub_tx.tx_amt1;
                }
                else if(v_com_item.dc_ind[0]=='2')          /*贷处理*/
                {
                    v_gl_mst.cr_bal-=g_pub_tx.tx_amt1;
                }   
                else if(v_com_item.dc_ind[0]=='0')          /*双向处理*/
                {
                    amt=v_gl_mst.dr_bal-v_gl_mst.cr_bal+g_pub_tx.tx_amt1;
                    ret=pub_base_CompDblVal(amt,0.00);
                    if(ret==1)
                    {
                        v_gl_mst.dr_bal=amt;        /*当前借方余额*/
                        v_gl_mst.cr_bal=0.00;       /*当前贷方余额*/
                    }
                    else if(ret==-1)
                    {
                        v_gl_mst.dr_bal=0.00;       /*当前借方余额*/
                        v_gl_mst.cr_bal=-amt;       /*当前贷方余额*/
                    }
                    else
                    {
                        v_gl_mst.dr_bal=0.00;       /*当前借方余额*/
                        v_gl_mst.cr_bal=0.00;       /*当前贷方余额*/
                    }
                }
                v_gl_mst.rdd_cnt+=1;                    /*本日借笔数*/
                v_gl_mst.rdd_amt+=g_pub_tx.tx_amt1;     /*本日借发生额*/
                v_gl_mst.tddr_cnt+=1;                   /*本旬借笔数*/
                v_gl_mst.tddr_amt+=g_pub_tx.tx_amt1;    /*本旬借发生额*/
                v_gl_mst.mdr_cnt+=1;                    /*本月借笔数*/
                v_gl_mst.mdr_amt+=g_pub_tx.tx_amt1;     /*本月借发生额*/
                v_gl_mst.qdr_cnt+=1;                    /*本季借笔数*/
                v_gl_mst.qdr_amt+=g_pub_tx.tx_amt1;     /*本季借发生额*/
                v_gl_mst.ydr_cnt+=1;                    /*本年借笔数*/
                v_gl_mst.ydr_amt+=g_pub_tx.tx_amt1;     /*本年借发生额*/
                break;
            case '1': /*贷方记账*/
                if(v_com_item.amt_dc_ind[0]=='1')
                {
                    sprintf(acErrMsg,"此科目只能记借方帐![%s]",g_pub_tx.ac_no);
                    WRITEMSG
                    strcpy(g_pub_tx.reply,"A024");
                    goto ErrExit;
                }

                if(v_com_item.dc_ind[0]=='1')               /*借处理*/
                {
                    v_gl_mst.dr_bal-=g_pub_tx.tx_amt1;
                }
                else if(v_com_item.dc_ind[0]=='2')          /*贷处理*/
                {
                    v_gl_mst.cr_bal+=g_pub_tx.tx_amt1;
                }   
                else if(v_com_item.dc_ind[0]=='0')          /*双向处理*/
                {
                    amt=v_gl_mst.dr_bal-v_gl_mst.cr_bal-g_pub_tx.tx_amt1;
                    ret=pub_base_CompDblVal(amt,0.00);
                    if(ret==1)
                    {
                        v_gl_mst.dr_bal=amt;        /*当前借方余额*/
                        v_gl_mst.cr_bal=0.00;       /*当前贷方余额*/
                    }
                    else if(ret==-1)
                    {
                        v_gl_mst.dr_bal=0.00;       /*当前借方余额*/
                        v_gl_mst.cr_bal=-amt;       /*当前贷方余额*/
                    }
                    else
                    {
                        v_gl_mst.dr_bal=0.00;       /*当前借方余额*/
                        v_gl_mst.cr_bal=0.00;       /*当前贷方余额*/
                    }
                }
                v_gl_mst.rcd_cnt+=1;                    /*本日贷笔数*/
                v_gl_mst.rcd_amt+=g_pub_tx.tx_amt1;     /*本日贷发生额*/
                v_gl_mst.tdcr_cnt+=1;                   /*本旬贷笔数*/
                v_gl_mst.tdcr_amt+=g_pub_tx.tx_amt1;    /*本旬贷发生额*/
                v_gl_mst.mcr_cnt+=1;                    /*本月贷笔数*/
                v_gl_mst.mcr_amt+=g_pub_tx.tx_amt1;     /*本月贷发生额*/
                v_gl_mst.qcr_cnt+=1;                    /*本季贷笔数*/
                v_gl_mst.qcr_amt+=g_pub_tx.tx_amt1;     /*本季贷发生额*/
                v_gl_mst.ycr_cnt+=1;                    /*本年贷笔数*/
                v_gl_mst.ycr_amt+=g_pub_tx.tx_amt1;     /*本年贷发生额*/
                break;
            default:
                sprintf(acErrMsg,"存取标志错误=[%s]!",g_pub_tx.add_ind);
                WRITEMSG
                strcpy(g_pub_tx.reply,"P106");
                goto ErrExit;
                break;
        }

        ret=Gl_mst_Upd_Upd(v_gl_mst,g_pub_tx.reply);
        if( ret )
        {
            sprintf(acErrMsg,"Update 总帐出错!![%s]",ret);
            WRITEMSG
            strcpy(g_pub_tx.reply,"P102");
            goto ErrExit;
        }
    }
    Gl_mst_Clo_Upd();

    if( i==0 )
    {
        sprintf(acErrMsg,"总帐中不存在该科目v_gl_mst.acc_hrt=[%s]", \
            v_gl_mst.acc_hrt);
        WRITEMSG
        strcpy(g_pub_tx.reply,"O075");
        goto ErrExit;
    }


OkExit:
    return 0;
ErrExit:
    return 1;
}

int pub_ins_ln_trace_log( double amt )
{
    double v_amt;
    int ret=0;

    v_amt=g_pub_tx.tx_amt1;
    g_pub_tx.tx_amt1=amt;
    ret=pub_ins_trace_log();
    TRACE
    g_pub_tx.tx_amt1=v_amt;
    ERR_DEAL

OkExit:
    return 0;
ErrExit:
    return 1;
}
int pub_ins_trace_log()
{
    struct trace_log_c  trace_log_c;
    int ret;

    memset(&trace_log_c,0x00,sizeof(struct trace_log_c));
    
    trace_log_c.trace_no=g_pub_tx.trace_no;
    trace_log_c.tx_date=g_pub_tx.tx_date;
    trace_log_c.tx_time=g_pub_tx.tx_time;

    strcpy(trace_log_c.tx_br_no,g_pub_tx.tx_br_no);
    strcpy(trace_log_c.opn_br_no,g_pub_tx.opn_br_no);
    if(memcmp(g_pub_tx.tx_code,"6203",4)==0){/**2010/6/10 11:53*ADD BY ZHGF**/
        char fd60[17]; /* 柜员机构 */
        char fd24[4]; /* 业务类型 */
        get_zd_data("0240",fd24);
        if(memcmp(fd24,"gmt",3)==0)
        {
            memset(fd60,0,sizeof(fd60));
            get_zd_data("0600",fd60);
            vtcp_log("%s,%d fd60====[%s]\n",__FILE__,__LINE__,fd60);
            memcpy(trace_log_c.tx_br_no,fd60,5);
        }
    }
    strcpy(trace_log_c.tx_code,g_pub_tx.tx_code);
    strcpy(trace_log_c.sub_tx_code,g_pub_tx.sub_tx_code);
    trace_log_c.svc_ind=g_pub_tx.svc_ind;
    vtcp_log("CT IND[%s],svc_ind[%d]",g_pub_tx.ct_ind,g_pub_tx.svc_ind);
    trace_log_c.ct_ind[0]=g_pub_tx.ct_ind[0];

    strcpy(trace_log_c.cur_no,g_pub_tx.cur_no);
    strcpy(trace_log_c.prdt_no,g_pub_tx.prdt_code);
    /***
    strcpy(trace_log_c.ac_no,g_pub_tx.ac_no);
    ***/
    /**modify by xyy 2006-11-20 12:59 **/
    /*YW 20111216 如果为IC卡时子账户转换为主账户 按目前情况只能通过prdt_code区分*/
/**    ret=pub_get_ic_main(g_pub_tx.ac_no,trace_log_c.ac_no,trace_log_c.ac_id);
    if(ret!=0){
       strncpy(trace_log_c.ac_no,g_pub_tx.ac_no,19);
       trace_log_c.ac_id=g_pub_tx.ac_id;
       strcpy(g_pub_tx.reply,"0000");候毅让增加 不影响理财签约
    }
    vtcp_log("[%s][%d] ret =[%d][%s][%ld]\n",__FILE__,__LINE__,ret,trace_log_c.ac_no,trace_log_c.ac_id);
    trace_log_c.ac_seqn=g_pub_tx.ac_seqn;**/

		strncpy(trace_log_c.ac_no,g_pub_tx.ac_no,19);
    trace_log_c.ac_seqn=g_pub_tx.ac_seqn;
    trace_log_c.ac_id=g_pub_tx.ac_id;

    strcpy(trace_log_c.note_type,g_pub_tx.note_type);
    /**** 20110521 modify
    strcpy(trace_log_c.note_no,g_pub_tx.beg_note_no);
    ****/
    memcpy(trace_log_c.note_no,g_pub_tx.beg_note_no,16);

    if( g_pub_tx.add_ind[0]=='1' )
        strcpy(trace_log_c.add_ind,g_pub_tx.add_ind);
    else
        strcpy(trace_log_c.add_ind,"0");
    trace_log_c.amt=g_pub_tx.tx_amt1;

    strcpy(trace_log_c.brf,g_pub_tx.brf);
    strcpy(trace_log_c.hst_ind,g_pub_tx.hst_ind);
    strcpy(trace_log_c.tel,g_pub_tx.tel);
    strcpy(trace_log_c.chk,g_pub_tx.chk);
    strcpy(trace_log_c.aut,g_pub_tx.auth);
sprintf(acErrMsg," TEL [%s] CHK [%s]",g_pub_tx.tel,g_pub_tx.chk);
WRITEMSG
    strcpy(trace_log_c.saving_notarize,"0");
    strcpy(trace_log_c.sts,"0");
    strcpy(trace_log_c.no_show,g_pub_tx.no_show);
    if( trace_log_c.no_show[0]!='1' )
        trace_log_c.no_show[0]='0';
    TRACE

    if( g_pub_tx.ac_wrk_ind[1]=='2' )
    {
        get_zd_long( DC_TRACE_CNT , &g_pub_tx.trace_cnt);
        g_pub_tx.trace_cnt++;
        set_zd_long( DC_TRACE_CNT , g_pub_tx.trace_cnt);
        sprintf(acErrMsg,"强制不记流水");
        WRITEMSG
    }
    else if( !pub_base_CompDblVal(trace_log_c.amt,0.00) 
        && g_pub_tx.ac_wrk_ind[1]=='0' )
    {
        sprintf(acErrMsg,"0金额不记流水");
        WRITEMSG
    }
    else
    {
            get_zd_long( DC_TRACE_CNT , &g_pub_tx.trace_cnt);
        g_pub_tx.trace_cnt++;
        trace_log_c.trace_cnt=g_pub_tx.trace_cnt;
            set_zd_long( DC_TRACE_CNT , g_pub_tx.trace_cnt);
        ret=Trace_log_Ins(trace_log_c,g_pub_tx.reply);
        if( ret )
        {
            Trace_log_Debug(&trace_log_c);
            vtcp_log("=====[%.2lf]",trace_log_c.amt);
            sprintf(acErrMsg,"登记交易流水错误[%d]!",ret);
            WRITEMSG
            strcpy(g_pub_tx.reply,"P126");
            goto ErrExit;
        }
    }
    TRACE
    return 0;
ErrExit:
    return 1;
}

int pub_ins_dd_hst()
{
    int ret;

    struct dd_mst_hst_c dd_mst_hst_c;
    memset(&dd_mst_hst_c,0x00,sizeof(struct dd_mst_hst_c));


    dd_mst_hst_c.trace_no=g_pub_tx.trace_no;
    dd_mst_hst_c.trace_cnt=g_pub_tx.trace_cnt;
    strcpy(dd_mst_hst_c.opn_br_no,g_pub_tx.opn_br_no);  
    strcpy(dd_mst_hst_c.tx_br_no,g_pub_tx.tx_br_no);    
    dd_mst_hst_c.ac_id=g_pub_tx.ac_id;
    dd_mst_hst_c.ac_seqn=g_pub_tx.ac_seqn;
    strcpy(dd_mst_hst_c.tx_code,g_pub_tx.tx_code);  
    strcpy(dd_mst_hst_c.sub_tx_code,g_pub_tx.sub_tx_code);  

    if( g_pub_tx.add_ind[0]=='2' || g_pub_tx.add_ind[0]=='0' 
        || g_pub_tx.add_ind[0]=='3' )
        strcpy(dd_mst_hst_c.add_ind,"0");   
    else
        strcpy(dd_mst_hst_c.add_ind,"1");   
    strcpy(dd_mst_hst_c.ct_ind,g_pub_tx.ct_ind);
    dd_mst_hst_c.tx_amt=g_pub_tx.tx_amt1;
    dd_mst_hst_c.bal=g_dd_mst.hst_bal;
    dd_mst_hst_c.acbo_bal=g_dd_mst.acbo_bal;
    dd_mst_hst_c.intst_acm=g_dd_mst.intst_acm;
    dd_mst_hst_c.tx_date=g_pub_tx.tx_date;
    dd_mst_hst_c.tx_time=g_pub_tx.tx_time;
    strcpy(dd_mst_hst_c.note_type,g_pub_tx.note_type);
    /**** 20110521 modify
    strcpy(dd_mst_hst_c.note_no,g_pub_tx.beg_note_no);
    ****/
    memcpy(dd_mst_hst_c.note_no,g_pub_tx.beg_note_no,16);
    
    /*--------------by xxx--20030729----------------
    strncpy(dd_mst_hst_c.brf,g_pub_tx.brf,10);
    ----------------------------------------------*/
    pub_base_unprt_brf( dd_mst_hst_c.brf );
    vtcp_log("-----摘-----要-----g_pub_tx.brf=[%s],dd_mst_hst_c.brf=[%s]",g_pub_tx.brf,dd_mst_hst_c.brf);
    /*mod beg by zzz20121010 与平台约定专有标志串"JZFSSR缴款"*/
    const char sFssrStr[]="JZFSSR缴款";
    char sFssrTag[128]={'\0'};
    get_zd_data("0830",sFssrTag);
    zip_space(sFssrTag);
    vtcp_log("[%s][%d][%s] ",__FILE__,__LINE__,sFssrTag);
    if (0==strcmp(sFssrTag,sFssrStr))
    {
        strcpy(dd_mst_hst_c.brf,sFssrStr);
    }
    /*mod end by zzz20121010*/
    strcpy(dd_mst_hst_c.tel,g_pub_tx.tel);  
    strcpy(dd_mst_hst_c.chk,g_pub_tx.chk);  
    strcpy(dd_mst_hst_c.auth,g_pub_tx.auth);    
    get_zd_long( DC_TRACE_CNT , &dd_mst_hst_c.trace_cnt);
    dd_mst_hst_c.trace_cnt++;

    if( ( pub_base_CompDblVal(dd_mst_hst_c.tx_amt,0.00) 
        || g_pub_tx.ac_wrk_ind[2]=='1') && g_pub_tx.hst_ind[0]=='1' )
    {
        g_dd_mst.hst_cnt++;
        dd_mst_hst_c.hst_cnt=g_dd_mst.hst_cnt;
        if(memcmp(g_pub_tx.tx_code,"6203",4) ==0)
        {/**2010/6/10 11:53*ADD BY ZHGF**/
        
            char fd60[17]; /* 柜员机构 */
            char fd24[4]; /* 业务类型 */
            get_zd_data("0240",fd24);
            if(memcmp(fd24,"gmt",3)==0)
            {
                memset(fd60,0,sizeof(fd60));
                get_zd_data("0600",fd60);
                vtcp_log("%s,%d fd60====[%s]\n",__FILE__,__LINE__,fd60);
                memcpy(dd_mst_hst_c.tx_br_no,fd60,5);
                memcpy(dd_mst_hst_c.tel,fd60+5,6);
            }
        }

        ret=Dd_mst_hst_Ins(dd_mst_hst_c,g_pub_tx.reply);
        Dd_mst_hst_Debug(&dd_mst_hst_c);
        if( ret )
        {
            sprintf(acErrMsg,"登记活期明细帐错误[%d]!",ret);
            WRITEMSG
            strcpy(g_pub_tx.reply,"P127");
            goto ErrExit;
        }
    }

OkExit: 
    return 0;
ErrExit:
    return 1;
}

int pub_ins_td_hst()
{
    int ret;
    struct td_mst_hst_c td_mst_hst_c;
    memset(&td_mst_hst_c,0x00,sizeof(struct td_mst_hst_c));

    td_mst_hst_c.trace_no=g_pub_tx.trace_no;
    td_mst_hst_c.trace_cnt=g_pub_tx.trace_cnt;
    strcpy(td_mst_hst_c.opn_br_no,g_pub_tx.opn_br_no);  
    strcpy(td_mst_hst_c.tx_br_no,g_pub_tx.tx_br_no);    
    td_mst_hst_c.ac_id=g_pub_tx.ac_id;
    td_mst_hst_c.ac_seqn=g_pub_tx.ac_seqn;
    strcpy(td_mst_hst_c.tx_code,g_pub_tx.tx_code);  
    strcpy(td_mst_hst_c.sub_tx_code,g_pub_tx.sub_tx_code);  
TRACE
    if( g_pub_tx.add_ind[0]=='2' || g_pub_tx.add_ind[0]=='0' )
        strcpy(td_mst_hst_c.add_ind,"0");   
    else
        strcpy(td_mst_hst_c.add_ind,"1");   
    strcpy(td_mst_hst_c.ct_ind,g_pub_tx.ct_ind);
    td_mst_hst_c.tx_amt=g_pub_tx.tx_amt1;
    td_mst_hst_c.bal=g_td_mst.hst_bal;
    td_mst_hst_c.acbo_bal=g_td_mst.acbo_bal;
    td_mst_hst_c.intst_acm=g_td_mst.intst_acm;
    td_mst_hst_c.intst=g_pub_intst.factval;         /*** 取利息 ***/
    td_mst_hst_c.tx_date=g_pub_tx.tx_date;
    td_mst_hst_c.tx_time=g_pub_tx.tx_time;
TRACE
    strcpy(td_mst_hst_c.note_type,g_pub_tx.note_type);
    /**** 20110521 modify
    strcpy(td_mst_hst_c.note_no,g_pub_tx.beg_note_no);
    ****/
    memcpy(td_mst_hst_c.note_no,g_pub_tx.beg_note_no,16);
    
    /*--------------by xxx--20030729----------------
    strcpy(td_mst_hst_c.brf,g_pub_tx.brf);  
    ---------------------------------------------*/
    pub_base_unprt_brf( td_mst_hst_c.brf );
    
    strcpy(td_mst_hst_c.tel,g_pub_tx.tel);  
    strcpy(td_mst_hst_c.chk,g_pub_tx.chk);  
    strcpy(td_mst_hst_c.auth,g_pub_tx.auth);    
    get_zd_long( DC_TRACE_CNT , &td_mst_hst_c.trace_cnt);
    td_mst_hst_c.trace_cnt++;

    vtcp_log("td_mst_hst_c.tx_amt===[%.2lf], g_pub_tx.ac_wrk_ind[2]===[%c], g_pub_tx.hst_ind====[%s]", td_mst_hst_c.tx_amt, g_pub_tx.ac_wrk_ind[2], g_pub_tx.hst_ind);
    vtcp_log("[%s][%d] partflag = %d\n",__FILE__,__LINE__,partflag);
    vtcp_log("[%s][%d] tx_amt1  = %.2f tx_amt2 =%.2f\n",__FILE__,__LINE__,g_pub_tx.tx_amt1,g_pub_tx.tx_amt2);

    if( ( pub_base_CompDblVal(td_mst_hst_c.tx_amt,0.00) 
        || g_pub_tx.ac_wrk_ind[2]=='1') && g_pub_tx.hst_ind[0]=='1' )
    {
    vtcp_log("[%s][%d] partflag = %d\n",__FILE__,__LINE__,partflag);
    vtcp_log("[%s][%d] tx_amt1  = %.2f tx_amt2 =%.2f\n",__FILE__,__LINE__,g_pub_tx.tx_amt1,g_pub_tx.tx_amt2);
        if( partflag==1 )
        {
            g_td_mst.hst_cnt++;
    vtcp_log("[%s][%d] *********hst_cnt =[%d]\n",__FILE__,__LINE__,g_td_mst.hst_cnt);
            td_mst_hst_c.hst_cnt=g_td_mst.hst_cnt;
    vtcp_log("[%s][%d] *********td_mst_hst.hst_cnt =[%d]\n",__FILE__,__LINE__,td_mst_hst_c.hst_cnt);
        /*  td_mst_hst_c.tx_amt=o_td_mst.bal;以前这种方式看的明细不清楚*/
            td_mst_hst_c.tx_amt=g_pub_tx.tx_amt1;
            td_mst_hst_c.bal=0.00;
            strcpy(td_mst_hst_c.add_ind,"0");   
            ret=Td_mst_hst_Ins(td_mst_hst_c,g_pub_tx.reply);
            if( ret )
            {
                sprintf(acErrMsg,"登记定期明细帐错误[%d]!",ret);
                WRITEMSG
                strcpy(g_pub_tx.reply,"P128");
                goto ErrExit;
            }
            td_mst_hst_c.tx_amt=o_td_mst.bal-g_pub_tx.tx_amt1;
            td_mst_hst_c.bal=o_td_mst.bal-g_pub_tx.tx_amt1;
            strcpy(td_mst_hst_c.add_ind,"1");   
            strcpy(td_mst_hst_c.note_no,g_pub_tx.new_note_no);  
            strcpy( td_mst_hst_c.brf,"部提开" );
            partflag = 0; /**** 控制部提新开多一条明细的问题 add at 20120108****/
        }
    vtcp_log("[%s][%d] partflag = %d\n",__FILE__,__LINE__,partflag);
    vtcp_log("[%s][%d] tx_amt1  = %.2f tx_amt2 =%.2f\n",__FILE__,__LINE__,g_pub_tx.tx_amt1,g_pub_tx.tx_amt2);
        g_td_mst.hst_cnt++;
    vtcp_log("[%s][%d] *********hst_cnt =[%d]\n",__FILE__,__LINE__,g_td_mst.hst_cnt);
        td_mst_hst_c.hst_cnt=g_td_mst.hst_cnt;
    vtcp_log("[%s][%d] *********td_mst_hst.hst_cnt =[%d]\n",__FILE__,__LINE__,td_mst_hst_c.hst_cnt);
        ret=Td_mst_hst_Ins(td_mst_hst_c,g_pub_tx.reply);
        if( ret )
        {
            sprintf(acErrMsg,"登记定期明细帐错误[%d]!",ret);
            WRITEMSG
            strcpy(g_pub_tx.reply,"P128");
            goto ErrExit;
        }
    }
    vtcp_log("[%s][%d] partflag = %d\n",__FILE__,__LINE__,partflag);
    vtcp_log("[%s][%d] tx_amt1  = %.2f tx_amt2 =%.2f\n",__FILE__,__LINE__,g_pub_tx.tx_amt1,g_pub_tx.tx_amt2);

    sprintf(acErrMsg,"登记定期帐户明细成功!!!");
    WRITEMSG
    return 0;
ErrExit:
    sprintf(acErrMsg,"登记定期帐户明细失败!!!");
    WRITEMSG
    return 1;
}
/* mode 1本金 2表内 3表外 4复利 9-按揭还利息*/
/* log_ind  0-记流水    1-不记流水 */
int pub_ins_ln_hst( int mode , int log_ind )
{
    struct ln_mst_hst_c ln_mst_hst_c;
    int ret=0;
    sprintf(acErrMsg,"进入记明细函数, 发生额和贷款余额[%.2lf][%.2lf]",
                    g_pub_tx.tx_amt1,g_ln_mst.bal );
    WRITEMSG
    memset(&ln_mst_hst_c,0x00,sizeof(ln_mst_hst_c));

    ln_mst_hst_c.trace_no=g_pub_tx.trace_no;

    strcpy(ln_mst_hst_c.opn_br_no,g_pub_tx.opn_br_no);
    strcpy(ln_mst_hst_c.tx_br_no,g_pub_tx.tx_br_no);
    ln_mst_hst_c.ac_id=g_pub_tx.ac_id;
    ln_mst_hst_c.ac_seqn=g_pub_tx.ac_seqn;

    strcpy(ln_mst_hst_c.tx_code,g_pub_tx.tx_code);
    strcpy(ln_mst_hst_c.sub_tx_code,g_pub_tx.sub_tx_code);
    strcpy(ln_mst_hst_c.add_ind,g_pub_tx.add_ind);
    /**
    if(g_pub_tx.svc_ind==3091)
    ***/
    if(memcmp(g_pub_tx.tx_code,"3315",sizeof(g_pub_tx.tx_code)-1)==0)
    {
        if(g_pub_tx.add_ind[0]=='1')
            strcpy(ln_mst_hst_c.add_ind,"0");
        else
            strcpy(ln_mst_hst_c.add_ind,"1");
    }
    strcpy(ln_mst_hst_c.ct_ind,g_pub_tx.ct_ind);
    sprintf(acErrMsg,"现转标志![%s]",g_pub_tx.ct_ind);
    WRITEMSG    
    ln_mst_hst_c.tx_date=g_pub_tx.tx_date;
    ln_mst_hst_c.tx_time=g_pub_tx.tx_time;
    strcpy(ln_mst_hst_c.note_type,g_pub_tx.note_type);
    /**** 20110521 modify 
    strcpy(ln_mst_hst_c.note_no,g_pub_tx.beg_note_no);
    ****/
    memcpy(ln_mst_hst_c.note_no,g_pub_tx.beg_note_no,16);

    strncpy(ln_mst_hst_c.brf,g_pub_tx.brf,20);
    strcpy(ln_mst_hst_c.tel,g_pub_tx.tel);
    strcpy(ln_mst_hst_c.chk,g_pub_tx.chk);
    strcpy(ln_mst_hst_c.auth,g_pub_tx.auth);

    if( mode==0 )
    {
        TRACE
        strcpy(ln_mst_hst_c.ln_tx_type,"0");
        ln_mst_hst_c.tx_amt=g_pub_tx.tx_amt1;
        ln_mst_hst_c.bal=g_ln_mst.bal;
        if(g_pub_tx.svc_ind==3063)  /* 贷款正常转逾期转出余额就成0 */
            ln_mst_hst_c.bal=0;
        ln_mst_hst_c.intst_acm=g_ln_mst.intst_acm;
        /*****
        ln_mst_hst_c.intst_acm=ln_mst_acm.intst_acm;
        ***/
    }
    else if( mode==1 )
    {
        strcpy(ln_mst_hst_c.ln_tx_type,"1");
        ln_mst_hst_c.tx_amt=g_pub_tx.tx_amt2;
        ln_mst_hst_c.bal=g_ln_mst.in_lo_intst;
        
        ln_mst_hst_c.intst_acm=g_ln_mst.in_lo_acm;
        /***
        ln_mst_hst_c.intst_acm=ln_mst_acm.in_lo_acm;
        ***/
    }
    else if( mode==2 )
    {
        strcpy(ln_mst_hst_c.ln_tx_type,"2");
        ln_mst_hst_c.tx_amt=g_pub_tx.tx_amt3;
        ln_mst_hst_c.bal=g_ln_mst.out_lo_intst;
        
        ln_mst_hst_c.intst_acm=g_ln_mst.out_lo_acm;
        /****
        ln_mst_hst_c.intst_acm=ln_mst_acm.out_lo_acm;
        ***/
    }
    else if( mode==3 )
    {
        strcpy(ln_mst_hst_c.ln_tx_type,"3");
        ln_mst_hst_c.tx_amt=g_pub_tx.tx_amt4;
        ln_mst_hst_c.bal=g_ln_mst.cmpd_lo_intst;
    
        ln_mst_hst_c.intst_acm=g_ln_mst.cmpd_lo_acm;
        /***
        ln_mst_hst_c.intst_acm=ln_mst_acm.cmpd_lo_acm;
        ***/
    }
    else if( mode==4 )
    {
        strcpy(ln_mst_hst_c.ln_tx_type,"4");
        ln_mst_hst_c.tx_amt=g_pub_tx.tx_amt1;
        ln_mst_hst_c.bal=g_ln_mst.bal;
        
        ln_mst_hst_c.intst_acm=g_ln_mst.intst_acm;
        /***
        ln_mst_hst_c.intst_acm=ln_mst_acm.intst_acm;
        ***/
    }
    else if( mode==5 )
    {
        strcpy(ln_mst_hst_c.ln_tx_type,"3");
        ln_mst_hst_c.tx_amt=g_pub_tx.tx_amt2;

        if( g_pub_tx.svc_ind==3051 )
        {
            ln_mst_hst_c.bal=0.00;
        }else{
            ln_mst_hst_c.bal=g_ln_mst.in_lo_intst;
        }
                    
        ln_mst_hst_c.intst_acm=g_ln_mst.in_lo_acm;
        /***
        ln_mst_hst_c.intst_acm=ln_mst_acm.in_lo_acm;
        ***/
    }
    else if( mode==6 )
    {
        strcpy(ln_mst_hst_c.ln_tx_type,"3");
        ln_mst_hst_c.tx_amt=g_pub_tx.tx_amt3;
        if( g_pub_tx.svc_ind==3051 )
        {
            ln_mst_hst_c.bal=0.00;
        }else{
            ln_mst_hst_c.bal=g_ln_mst.out_lo_intst;
        }
        
        ln_mst_hst_c.intst_acm=g_ln_mst.out_lo_acm;
        /****
        ln_mst_hst_c.intst_acm=ln_mst_acm.out_lo_acm;
        ***/
    }
    else if( mode==7 )
    {
        strcpy(ln_mst_hst_c.ln_tx_type,"3");
        ln_mst_hst_c.tx_amt=g_pub_tx.tx_amt4;
        if( g_pub_tx.svc_ind==3051 )
        {
            ln_mst_hst_c.bal=0.00;
        }else{
            ln_mst_hst_c.bal=g_ln_mst.cmpd_lo_intst;
        }   
        
        ln_mst_hst_c.intst_acm=g_ln_mst.cmpd_lo_acm;
        /***    
        ln_mst_hst_c.intst_acm=ln_mst_acm.cmpd_lo_acm;
        ***/
    }
    else if( mode==8 )
    {
        strcpy(ln_mst_hst_c.ln_tx_type,"8");
        ln_mst_hst_c.tx_amt=g_pub_tx.tx_amt1;
        if( g_pub_tx.svc_ind==3051 )
        {
            ln_mst_hst_c.bal=0.00;
        }else if( g_pub_tx.svc_ind==3092 ){
            ln_mst_hst_c.bal=g_ln_mst.gage_amt;
        }else{
            ln_mst_hst_c.bal=g_ln_mst.bal;
        }
        
        ln_mst_hst_c.intst_acm=g_ln_mst.intst_acm;
        /***
        ln_mst_hst_c.intst_acm=ln_mst_acm.intst_acm;
        ***/
    }
    else if( mode==9 )
    {
        strcpy(ln_mst_hst_c.ln_tx_type,"9");
        ln_mst_hst_c.tx_amt=g_pub_tx.tx_amt4+g_pub_tx.tx_amt2;
        ln_mst_hst_c.bal=0.00;
        ln_mst_hst_c.intst_acm=0.00;
    }
    else if( mode==10 )
    {
        strcpy(ln_mst_hst_c.ln_tx_type,"A");
        ln_mst_hst_c.tx_amt=g_pub_tx.tx_amt1;
        ln_mst_hst_c.bal=0.00;
        ln_mst_hst_c.intst_acm=0.00;
    }           
    else if( mode==11 )
    {
        if( g_pub_tx.svc_ind==3011 )
        {
            strcpy(ln_mst_hst_c.ln_tx_type,"1");
            ln_mst_hst_c.bal=g_ln_mst.in_lo_intst+g_pub_tx.tx_amt1;
        }else if( g_pub_tx.svc_ind==3012 )
        {
            strcpy(ln_mst_hst_c.ln_tx_type,"2");
            ln_mst_hst_c.bal=g_ln_mst.out_lo_intst+g_pub_tx.tx_amt1;
        }
        
        ln_mst_hst_c.tx_amt=g_pub_tx.tx_amt1;
        ln_mst_hst_c.intst_acm=0.00;
    }
    else if( mode==12 )
    {
        strcpy(ln_mst_hst_c.ln_tx_type,"3");
        ln_mst_hst_c.tx_amt=g_pub_tx.tx_amt2;

        ln_mst_hst_c.bal=g_ln_mst.cmpd_lo_intst+g_pub_tx.tx_amt2;           
        ln_mst_hst_c.intst_acm=0.00;
    }
    else if( mode==13 )
    {
        strcpy(ln_mst_hst_c.ln_tx_type,"3");
        ln_mst_hst_c.tx_amt=g_pub_tx.tx_amt3;

        ln_mst_hst_c.bal=g_ln_mst.cmpd_lo_intst+g_pub_tx.tx_amt3;           
        ln_mst_hst_c.intst_acm=0.00;
    }
    else if( mode==14 )
    {
        strcpy(ln_mst_hst_c.ln_tx_type,"3");
        ln_mst_hst_c.tx_amt=g_pub_tx.tx_amt4;

        ln_mst_hst_c.bal=g_ln_mst.cmpd_lo_intst+g_pub_tx.tx_amt4;           
        ln_mst_hst_c.intst_acm=0.00;
    }
    else if( mode==15 )
    {
        if( g_pub_tx.svc_ind==3011 )
        {
            strcpy(ln_mst_hst_c.ln_tx_type,"1");
            ln_mst_hst_c.bal=g_ln_mst.in_lo_intst;
        }else if( g_pub_tx.svc_ind==3012 ){
            strcpy(ln_mst_hst_c.ln_tx_type,"2");
            ln_mst_hst_c.bal=g_ln_mst.out_lo_intst;
        }           
        
        ln_mst_hst_c.tx_amt=g_pub_tx.tx_amt1;
        ln_mst_hst_c.intst_acm=g_ln_mst.intst_acm;
        /***
        ln_mst_hst_c.intst_acm=ln_mst_acm.intst_acm;
        ***/
    }                           
    else
    {
        sprintf(acErrMsg,"登记贷款明细帐mode错误[%d]!",mode);
        WRITEMSG
        strcpy(g_pub_tx.reply,"P129");
        goto ErrExit;
    }

    ln_mst_hst_c.trace_cnt=g_pub_tx.trace_cnt;
    get_zd_long( DC_TRACE_CNT , &ln_mst_hst_c.trace_cnt);
    ln_mst_hst_c.trace_cnt++;

    if( ( pub_base_CompDblVal(ln_mst_hst_c.tx_amt,0.00) 
        || g_pub_tx.ac_wrk_ind[2]=='1') && g_pub_tx.hst_ind[0]=='1' )
    {
        g_ln_mst.hst_cnt++;
        ln_mst_hst_c.hst_cnt=g_ln_mst.hst_cnt;

        sprintf(acErrMsg,"发生额和贷款余额[%.2lf][%.2lf]",
                    g_pub_tx.tx_amt1,g_ln_mst.bal );
        WRITEMSG        

        ret=Ln_mst_hst_Ins(ln_mst_hst_c,g_pub_tx.reply);
        if( ret )
        {
            sprintf(acErrMsg,"登记贷款明细帐错误[%d]!",ret);
            WRITEMSG
            strcpy(g_pub_tx.reply,"P129");
            goto ErrExit;
        }
    }
    if( log_ind==0 )
    {
        TRACE
        ret=pub_ins_ln_trace_log(ln_mst_hst_c.tx_amt);
        ERR_DEAL
        TRACE
    }
    
GoodExit:
    return 0;
ErrExit:
    return 1;
}

int pub_ins_in_hst()
{
    int ret;
    struct in_mst_hst_c in_mst_hst_c;
    memset(&in_mst_hst_c,0x00,sizeof(struct in_mst_hst_c));

    in_mst_hst_c.trace_no=g_pub_tx.trace_no;
    in_mst_hst_c.trace_cnt=g_pub_tx.trace_cnt;
    strcpy(in_mst_hst_c.opn_br_no,g_pub_tx.opn_br_no);  
    strcpy(in_mst_hst_c.tx_br_no,g_pub_tx.tx_br_no);    
    in_mst_hst_c.tx_date=g_pub_tx.tx_date;
    in_mst_hst_c.tx_time=g_pub_tx.tx_time;
    in_mst_hst_c.ac_id=g_pub_tx.ac_id;
    in_mst_hst_c.ac_seqn=g_pub_tx.ac_seqn;
    if( g_pub_tx.add_ind[0]=='2' || g_pub_tx.add_ind[0]=='0' )
        strcpy(in_mst_hst_c.add_ind,"0");   
    else
        strcpy(in_mst_hst_c.add_ind,"1");   
    vtcp_log("%s,%d 准备写入历史明细金额 ===%f",__FILE__,__LINE__,g_pub_tx.tx_amt1);
    in_mst_hst_c.tx_amt=g_pub_tx.tx_amt1;
    vtcp_log("%s,%d 历史明细金额 ===%f",__FILE__,__LINE__,in_mst_hst_c.tx_amt);
    in_mst_hst_c.bal=g_in_mst.hst_bal;
    in_mst_hst_c.intst_acm=g_in_mst.intst_acm;
    strcpy(in_mst_hst_c.note_type,g_pub_tx.note_type);
    strcpy(in_mst_hst_c.ct_ind,g_pub_tx.ct_ind); /*add by lwb 20150508*/
    /**** 20110521 modify
    strcpy(in_mst_hst_c.note_no,g_pub_tx.beg_note_no);
    ****/
    memcpy(in_mst_hst_c.note_no,g_pub_tx.beg_note_no,16);

    strncpy(in_mst_hst_c.brf,g_pub_tx.brf,20);  
    strcpy(in_mst_hst_c.tel,g_pub_tx.tel);  
    strcpy(in_mst_hst_c.chk,g_pub_tx.chk);  
    strcpy(in_mst_hst_c.auth,g_pub_tx.auth);    
    get_zd_long( DC_TRACE_CNT , &in_mst_hst_c.trace_cnt);
    in_mst_hst_c.trace_cnt++;

    if( ( pub_base_CompDblVal(in_mst_hst_c.tx_amt,0.00) 
        || g_pub_tx.ac_wrk_ind[2]=='1') && g_pub_tx.hst_ind[0]=='1' )
    {
        g_in_mst.hst_cnt++;
        in_mst_hst_c.hst_cnt=g_in_mst.hst_cnt;
        ret=In_mst_hst_Ins(in_mst_hst_c,g_pub_tx.reply);
        if( ret )
        {
            sprintf(acErrMsg,"登记内部明细帐错误[%d]!",ret);
            WRITEMSG
            strcpy(g_pub_tx.reply,"P128");
            goto ErrExit;
        }
    }
    
    return 0;
ErrExit:
    return 1;
}

int pub_unprt_hst_ins(char ac_type[2])
{
    int ret;
    char Cbrf[21];
    struct mdm_unprt_hst_c  mdm_unprt_hst_c;

    memset(Cbrf,0x00,sizeof(Cbrf));
    memset(&mdm_unprt_hst_c,0x00,sizeof(struct mdm_unprt_hst_c));

        vtcp_log("dddddddddd---");
        ret=sql_count("mdm_ac_rel","ac_id=%ld and (ac_seqn=%d or ac_seqn=%d) and (note_sts='0' or note_sts='1') and mdm_code in (select mdm_code from mdm_attr where prt_ind='Y' ) ",
        g_pub_tx.ac_id,g_pub_tx.ac_seqn,ALL_SEQN);
    if( ret==0 )
    {
        sprintf(acErrMsg,"不需要打印[%d][%d][%d]!",g_pub_tx.ac_id,g_pub_tx.ac_seqn,ret);
        WRITEMSG
        goto OkExit;
    }
    else if( ret>0 )
    {
        mdm_unprt_hst_c.trace_no=g_pub_tx.trace_no;
        mdm_unprt_hst_c.trace_cnt=g_pub_tx.trace_cnt;
        strcpy(mdm_unprt_hst_c.tx_code,g_pub_tx.tx_code);
        strcpy(mdm_unprt_hst_c.tx_br_no,g_pub_tx.tx_br_no);
        mdm_unprt_hst_c.ac_id=g_pub_tx.ac_id;
        mdm_unprt_hst_c.ac_seqn=g_pub_tx.ac_seqn;
        strcpy(mdm_unprt_hst_c.add_ind,g_pub_tx.add_ind);
        mdm_unprt_hst_c.tx_amt=g_pub_tx.tx_amt1;
        if( ac_type[0]=='1' )
            mdm_unprt_hst_c.bal=g_dd_mst.hst_bal;
        else if( ac_type[0]=='2' )
            {
            mdm_unprt_hst_c.bal=g_td_mst.hst_bal;
            if((!strncmp(g_pub_tx.tx_code,"2203",4))&&(!strncmp(g_pub_tx.mdm_code,"0016",4)))
        {   
            mdm_unprt_hst_c.hst_cnt=g_td_mst.book_line+1;
            vtcp_log("1111111111[%d][%d]\n",mdm_unprt_hst_c.hst_cnt,g_td_mst.book_line);
            }
            else if((!strncmp(g_pub_tx.tx_code,"2204",4))&&(!strncmp(g_pub_tx.mdm_code,"0016",4)))
            {
            mdm_unprt_hst_c.hst_cnt=g_td_mst.book_line+2;
            vtcp_log("222222222[%d][%d]\n",mdm_unprt_hst_c.hst_cnt,g_td_mst.book_line);
            }
            else
            {
            mdm_unprt_hst_c.hst_cnt=g_td_mst.book_line;
    vtcp_log("333333333[%d][%d]\n",mdm_unprt_hst_c.hst_cnt,g_td_mst.book_line);
    }   
            }
        else if( ac_type[0]=='3' )
            mdm_unprt_hst_c.bal=g_ln_mst.hst_bal;
        else if( ac_type[0]=='9' )
            mdm_unprt_hst_c.bal=g_in_mst.hst_bal;
        else
        {
            sprintf(acErrMsg,"登记未打印明细帐账号类型错误[%d]!",ret);
            WRITEMSG
            strcpy(g_pub_tx.reply,"P131");
            goto ErrExit;
        }
        mdm_unprt_hst_c.tx_date=g_pub_tx.tx_date;
        strcpy(mdm_unprt_hst_c.tel,g_pub_tx.tel);
        
        /*pub_base_unprt_brf(mdm_unprt_hst_c.brf);  del by hxc 100302*/
        /*strncpy(mdm_unprt_hst_c.brf,g_pub_tx.brf,10);*/
        /**Add by hxc 100302**/
        memset(Cbrf,0x00,sizeof(Cbrf));
        pub_base_unprt_brf(Cbrf);
        sprintf(acErrMsg,"[%s],[%d],Cbrf=[%s]!",__FILE__,__LINE__,Cbrf);
            WRITEMSG
        if(strlen(Cbrf)>10)
            {
                strncpy(mdm_unprt_hst_c.brf,Cbrf,10);
            }
        else
            {
                strcpy(mdm_unprt_hst_c.brf,Cbrf);
            }
        sprintf(acErrMsg,"[%s],[%d],登记未打印明细帐账号摘要为[%s]!",__FILE__,__LINE__,mdm_unprt_hst_c.brf);
            WRITEMSG
        /**End by hxc 100302**/


        if ( ( pub_base_CompDblVal(mdm_unprt_hst_c.tx_amt,0.00) || \
                g_pub_tx.ac_wrk_ind[2]=='1') )
        {
            if(memcmp(g_pub_tx.tx_code,"6203",4) ==0)
            {/**2010/6/10 11:53*ADD BY ZHGF**/
            
                char fd60[17]; /* 柜员机构 */
                char fd24[4]; /* 业务类型 */
                get_zd_data("0240",fd24);
                if(memcmp(fd24,"gmt",3)==0)
                {
                    memset(fd60,0,sizeof(fd60));
                    get_zd_data("0600",fd60);
                    vtcp_log("%s,%d fd60====[%s]\n",__FILE__,__LINE__,fd60);
                    memcpy(mdm_unprt_hst_c.tx_br_no,fd60,5);
                    memcpy(mdm_unprt_hst_c.tel,fd60+5,6);
                }
            }
                
            ret=Mdm_unprt_hst_Ins(mdm_unprt_hst_c,g_pub_tx.reply);
            if( ret )
            {
                sprintf(acErrMsg,"登记未打印明细帐错误[%d]!,traceno=[%ld],[%ld],[%ld][%ld]",ret,
                mdm_unprt_hst_c.trace_no,mdm_unprt_hst_c.ac_id,mdm_unprt_hst_c.tx_date,mdm_unprt_hst_c.trace_cnt);
                WRITEMSG
                strcpy(g_pub_tx.reply,"P131");
                goto ErrExit;
            }
        }
    }
    else
    {
        sprintf(acErrMsg,"判断是否打印出错[%d]!",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"P130");
        goto ErrExit;
    }

OkExit:
    return 0;
ErrExit:
    return 1;
}
static pub_sub_ac()
{
    struct mdm_ac_rel_c v_mdm_ac_rel;
    struct prdt_ac_id_c prdt_ac_id_c;
    struct prdt_ac_id_c s_prdt_ac_id;
    struct mdm_attr_c s_mdm_attr;
/*    struct mo_ic_ac_rel_c sMo_ic_ac_rel;*/
    char v_ac_get_ind[2];
    int ret=0;
    
    

    v_ac_get_ind[0]=g_pub_tx.ac_get_ind[0];
    vtcp_log("++++++++[%d]ac_id_type[%s],ac_id[%ld],mdm_ac_id[%ld],get_ind[%s]+++++++\n",__LINE__,g_pub_tx.ac_id_type,g_pub_tx.ac_id,g_mdm_ac_rel.ac_id,g_pub_tx.ac_get_ind);

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
sprintf(acErrMsg,"ac_seqn = [%d],ac_no[%s] ",g_pub_tx.ac_seqn,g_pub_tx.ac_no);
WRITEMSG
        /*** 检查账号、账号ID和账号序号 ***/
        if( g_pub_tx.ac_no[0]!=NULL )
        {
sprintf(acErrMsg,"ac_seqn = [%d] ",g_pub_tx.ac_seqn);
WRITEMSG
            ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,
                "ac_no='%s' ",g_pub_tx.ac_no);
            vtcp_log("%s,%d  notests=[%c]",__FILE__,__LINE__,g_mdm_ac_rel.note_sts[0]);
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
                " ac_id=%ld order by note_sts",g_pub_tx.ac_id );
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
    set_zd_data("0020", g_pub_tx.opn_br_no);

    /*** Add commence By Azz ***/
    if( g_mdm_ac_rel.coll_sts[0]=='1' && g_pub_tx.ac_wrk_ind[8]!='1')
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

sprintf(acErrMsg,"ac_seqn= [%d] cur_no[%s],ac_id[%ld] ",g_pub_tx.ac_seqn,g_pub_tx.cur_no,g_pub_tx.ac_id);
WRITEMSG

    vtcp_log("%s,%d notests=[%c]",__FILE__,__LINE__,g_mdm_ac_rel.note_sts[0]);
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
        vtcp_log("%s,%d notests=[%c]",__FILE__,__LINE__,g_mdm_ac_rel.note_sts[0]);
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
        else
        {
            g_pub_tx.ac_seqn=g_mdm_ac_rel.ac_seqn;
        }
    }
    vtcp_log("============[%d]ac_seqn[%d],ac_id[%ld]=======\n",__LINE__,g_pub_tx.ac_seqn,g_pub_tx.ac_id);
        vtcp_log("%s,%d notests=[%c],ac_wrk_ind=[%s],ac_get_ind=[%s]",__FILE__,__LINE__,g_mdm_ac_rel.note_sts[0],g_pub_tx.ac_wrk_ind,g_pub_tx.ac_get_ind);
    /**介质状态检查**/
    if( g_pub_tx.ac_wrk_ind[7]=='J' )
    {
        strcpy(g_pub_tx.mdm_code,g_mdm_ac_rel.mdm_code);
    }
    else if( g_pub_tx.ac_get_ind[0]=='1' )
    {
        strcpy(g_pub_tx.mdm_code,g_mdm_ac_rel.mdm_code);
        /* 卡部玉萍要求燃气退费 只要挂失状态就卡住 wudawei 20140620 */
				vtcp_log("==看下燃气的摘要是什么===[%d]brf=[%s],g_pub_tx.tx_code=[%s]\n",__LINE__,g_pub_tx.brf,g_pub_tx.tx_code);
				if( g_mdm_ac_rel.note_sts[0]=='1' && strcmp(g_pub_tx.tx_code,"6203")==0 && strcmp(g_pub_tx.brf,"燃气退费")==0)
				{
						vtcp_log("%s,%d notests=[%c]",__FILE__,__LINE__,g_mdm_ac_rel.note_sts[0]);
            sprintf(acErrMsg,"该介质处于挂失状态,并且是燃气退费交易，卡住，武大为于20140620加!");
            WRITEMSG
            strcpy(g_pub_tx.reply,"M009");
            goto ErrExit;
				}
				/* 20140620 wudawei end*/
        if( (g_mdm_ac_rel.note_sts[0]=='1' &&  strcmp(g_pub_tx.tx_code,"2204")!=0 )
            && strcmp(g_pub_tx.tx_code,"Z016") 
            && strcmp(g_pub_tx.tx_code,"9001") 
            && g_pub_tx.add_ind[0]=='0' 
            && g_pub_tx.ac_wrk_ind[8]!='1'
            && memcmp(g_pub_tx.ac_no,CARDHEADCODE,6)!=0 
            && memcmp(g_pub_tx.ac_no,ICCARDHEADCODE,6)!=0 )  /*商行磁条卡和IC卡挂失可销户20121222 wzw*/
        {
            vtcp_log("%s,%d notests=[%c]",__FILE__,__LINE__,g_mdm_ac_rel.note_sts[0]);
            sprintf(acErrMsg,"该介质处于挂失状态,请作解挂处理!");
            WRITEMSG
            strcpy(g_pub_tx.reply,"M009");
            goto ErrExit;
        }
        else if( g_mdm_ac_rel.note_sts[0]=='2' )
        {
            sprintf(acErrMsg,"该介质已挂失换证!");
            WRITEMSG
            strcpy(g_pub_tx.reply,"P171");
            goto ErrExit;
        }
        else if( g_mdm_ac_rel.note_sts[0]=='3' &&  strcmp(g_pub_tx.tx_code,"5840")!=0 ) /*小额来帐换证不卡 wudawei 20140721*/
        {
            sprintf(acErrMsg,"该介质已换证!");
            WRITEMSG
            strcpy(g_pub_tx.reply,"M011");
            goto ErrExit;
        }
        else if( g_mdm_ac_rel.note_sts[0]=='4' 
            && g_pub_tx.svc_ind != 1012 )
        {
            sprintf(acErrMsg,"该账户已作部提!!");
            WRITEMSG
            strcpy(g_pub_tx.reply,"M017");
            goto ErrExit;
        }
        else if( g_mdm_ac_rel.note_sts[0]=='5' )
        {
            sprintf(acErrMsg,"该账户已作撤销!!");
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
            if( g_mdm_ac_rel.coll_sts[0]=='2' && g_pub_tx.ac_wrk_ind[8]!='1' )
            {
                sprintf(acErrMsg,"该介质处于吞卡状态,请作卡归还!");
                WRITEMSG
                strcpy(g_pub_tx.reply,"M015");
                goto ErrExit;
            }
            else if( g_mdm_ac_rel.coll_sts[0]!='0' && g_pub_tx.ac_wrk_ind[8]!='1' )
            {
                sprintf(acErrMsg,"该介质捡拾状态不正常!");
                WRITEMSG
                strcpy(g_pub_tx.reply,"M016");
                goto ErrExit;
            }
        }
    }
    vtcp_log("++++++++[%d]ac_id_type[%s],ac_id[%ld]+++++++\n",__LINE__,g_pub_tx.ac_id_type,g_pub_tx.ac_id);
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
    else if( g_pub_tx.ac_id_type[0]=='3' )
    {
        g_pub_tx.ac_seqn=0 ;

        ret=Prdt_ac_id_Sel(g_pub_tx.reply,&prdt_ac_id_c,
                "ac_id=%ld and ac_seqn=%d ",g_pub_tx.ac_id,g_pub_tx.ac_seqn);
        if( ret )
        {
            sprintf(acErrMsg,"账号不存在![%ld][%d]",
                g_pub_tx.ac_id,g_pub_tx.ac_seqn);
            WRITEMSG
            strcpy(g_pub_tx.reply,"P102");
            goto ErrExit;
        }
        strcpy(g_pub_tx.prdt_code,prdt_ac_id_c.prdt_no);
    }
    else if( g_pub_tx.ac_id_type[0]=='1' )
    {
        g_pub_tx.ac_seqn=1 ;

        ret=Prdt_ac_id_Sel(g_pub_tx.reply,&prdt_ac_id_c,
                "ac_id=%ld and ac_seqn=%d ",g_pub_tx.ac_id,g_pub_tx.ac_seqn);
        if( ret )
        {
            sprintf(acErrMsg,"账号不存在![%ld][%d]",
                g_pub_tx.ac_id,g_pub_tx.ac_seqn);
            WRITEMSG
            strcpy(g_pub_tx.reply,"P102");
            goto ErrExit;
        }
        strcpy(g_pub_tx.prdt_code,prdt_ac_id_c.prdt_no);
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
    
    /*******ic卡子账户通存通兑处理 侯毅20111220*******/
/*    memset(&sMo_ic_ac_rel,0x00,sizeof(sMo_ic_ac_rel));
    ret=Mo_ic_ac_rel_Sel(g_pub_tx.reply,&sMo_ic_ac_rel,"app_ac_no='%s'",g_pub_tx.ac_no);
    if(ret!=100&&ret)
    {
    	vtcp_log("[%s][%d] 查询mo_ic_ac_rel错误 %d",__FILE__,__LINE__,ret);
    	strcpy(g_pub_tx.reply,"D103");
    	goto ErrExit;
    }*/
    /*******ic卡子账户通存通兑处理 侯毅20111220*******/

    /*** 没有密码不允许通兑,承兑签发不能检查 ***/
    if( strcmp(g_pub_tx.tx_br_no,QS_BR_NO) && g_pub_tx.ac_wrk_ind[8]!='1'
    && strcmp(g_pub_tx.tx_code,"4333")
    && strcmp(g_pub_tx.tx_code,"4334")
    && strcmp(g_pub_tx.tx_code,"4335")
    /**&& strcmp(g_pub_tx.tx_code,"3201") 晋中网银企业用户贷款还本交易修改 lzy 20141210**/
    /**&& strcmp(g_pub_tx.tx_code,"3202") 晋中网银企业用户贷款还息交易修改 lzy 20141210**/
    && strcmp(g_pub_tx.tx_code,"6388")/**晋中网银企业用户小额交易修改 hzh 20130602**/
    && strcmp(g_pub_tx.tx_code,"6389")/**晋中网银企业用户大额交易修改 hzh 20130602**/
    && strcmp(g_pub_tx.tx_code,"7102")/*lpj20151028财务支付记账允许同存**/
    /*&& strcmp(g_pub_tx.tx_code,"6236")*fupengfei 2014/7/14 11:50:39 银行汇票**/
/*    && (ret==100)    */                  /**ic卡子账户没有密码，但应该能够通存通兑 侯毅20111220**/
    && g_pub_tx.ac_id_type[0]!='9' 
    && strcmp(g_pub_tx.tx_br_no,g_pub_tx.opn_br_no) 
    && g_mdm_ac_rel.draw_pwd_yn[0]!='Y' && g_mdm_ac_rel.pwd_mach_yn[0]!='Y'
        && (g_pub_tx.ac_id_type[0]=='1' || g_pub_tx.ac_id_type[0]=='2') 
        && g_pub_tx.add_ind[0]=='0' )
    {
        sprintf(acErrMsg,"非留密户不允许通兑![%s]",g_pub_tx.tx_code);
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

int pub_sub_prdt_chk()
{
    int ret=0;

    if( g_pub_tx.svc_ind!=1003 && g_pub_tx.svc_ind!=1004 )  /** 销户 **/
        goto GoodExit;

    if( g_pub_tx.svc_ind==1003 )
    {
        /* 判断有错误。主账号不能销户 modify by martin 2009/7/28 17:11:58 */
        /* ret=sql_count("mo_prot_ac_rel"," (main_ac_id=%ld and main_ac_seqn=%d) or (app_ac_id=%ld and app_ac_seqn=%d) and sts='1'",g_pub_tx.ac_id,g_pub_tx.ac_seqn,g_pub_tx.ac_id,g_pub_tx.ac_seqn);*/
        ret=sql_count("mo_prot_ac_rel"," ((main_ac_id=%ld and main_ac_seqn=%d) or (app_ac_id=%ld and app_ac_seqn=%d)) and sts='1'",g_pub_tx.ac_id,g_pub_tx.ac_seqn,g_pub_tx.ac_id,g_pub_tx.ac_seqn);
        if( ret<0 )
        {
            sprintf(acErrMsg,"检查是否为协定理财户出错![%d]",ret);
            WRITEMSG
            strcpy(g_pub_tx.reply,"P280");
            goto ErrExit;
        }

        if( ret>0 )
        {
            sprintf(acErrMsg,"该户为协定理财户,不能直接销户![%d]",ret);
            WRITEMSG
            strcpy(g_pub_tx.reply,"P281");
            goto ErrExit;
        }
    }

    if( g_pub_tx.svc_ind==1004 )
    {
        ret=sql_count("mo_prot_ac_rel"," app_ac_id=%ld and app_ac_seqn=%d and sts='1'",g_pub_tx.ac_id,g_pub_tx.ac_seqn);
        if( ret<0 )
        {
            sprintf(acErrMsg,"检查是否为协定理财户出错![%d]",ret);
            WRITEMSG
            strcpy(g_pub_tx.reply,"P280");
            goto ErrExit;
        }
        if( ret==0 )
        {
            sprintf(acErrMsg,"该户非协定理财户,操作码错误!");
            WRITEMSG
            strcpy(g_pub_tx.reply,"P282");
            goto ErrExit;
        }
    }
    
GoodExit:
    return 0;
ErrExit:
    return 1;
}

pub_sub_note()
{
    int ret=0;
    vtcp_log("mdm_code is [%s],ac_no[%s],ac_id[%ld]\n",g_mdm_ac_rel.mdm_code,g_pub_tx.ac_no,g_pub_tx.ac_id);
    /**无支取介质**/
    if(!strcmp(g_mdm_ac_rel.mdm_code,NULL_MDM_CODE) || !memcmp(g_pub_tx.tx_code,"6236",4) || !memcmp(g_pub_tx.tx_code,"6238",4))
    {
        /*** 凭证销号处理 ***/
        if( strlen(g_pub_tx.note_type) && strlen(g_pub_tx.beg_note_no) )
        {
            ret=pub_com_NoteUse(g_pub_tx.ac_id,g_pub_tx.ac_seqn,
                g_pub_tx.note_type,g_pub_tx.beg_note_no,
                g_pub_tx.end_note_no,g_pub_tx.tel);
            if( ret )
            {
                sprintf(acErrMsg,"调用凭证销号错误!");
                WRITEMSG
                goto ErrExit;
            }
        }
    }
    else
    {
        ret=Mdm_attr_Sel(g_pub_tx.reply,&v_mdm_attr," mdm_code='%s' ", \
                        g_mdm_ac_rel.mdm_code );
        if( ret )
        {
            sprintf(acErrMsg,"取mdm_attr失败![%s]",g_mdm_ac_rel.mdm_code);
            WRITEMSG
            strcpy( g_pub_tx.reply,"N055" );
            goto ErrExit;
        }

        /**需要核对凭证号**/
        if( g_pub_tx.ac_wrk_ind[0]=='1' && memcmp(g_pub_tx.tx_code,"6789",sizeof(g_pub_tx.tx_code)-1)!=0)
        {
            if( strcmp(v_mdm_attr.note_type,g_pub_tx.note_type) ||
                strcmp(g_mdm_ac_rel.note_no,g_pub_tx.beg_note_no) )
            {
                sprintf(acErrMsg,"凭证号不符![%s][%s],mdm[%s][%s]",
                    g_pub_tx.note_type,g_pub_tx.beg_note_no,
                    v_mdm_attr.note_type,g_mdm_ac_rel.note_no);
                WRITEMSG
                strcpy(g_pub_tx.reply,"P169");
                goto ErrExit;
            }
        }
    }

    return 0;
ErrExit:
    return 1;
}
/*** 补登折处理 ***/
pub_sub_prt_book()
{
    int ret=0;
    int totalnum = 0;

    totalnum=v_mdm_attr.pg_num*v_mdm_attr.pg_line;

    sprintf(acErrMsg,"XXXXXXXXXX! g_mdm_ac_rel.mdm_code=[%s]", \
             g_mdm_ac_rel.mdm_code);
        WRITEMSG
    /**无支取介质**/
    if( !strcmp(g_mdm_ac_rel.mdm_code,NULL_MDM_CODE) )
        goto GoodExit;

    sprintf(acErrMsg,"XXXXXXXXXX! v_mdm_attr.prt_ind=[%s]", \
             v_mdm_attr.prt_ind);
        WRITEMSG
    if( v_mdm_attr.prt_ind[0]!='Y' )
        goto GoodExit;

    sprintf(acErrMsg,"XXXXXXXXXX! g_pub_tx.prt_ind=[%s],ac_no=[%s]", \
                 g_pub_tx.prt_ind,g_mdm_ac_rel.ac_no);
        WRITEMSG
    if( g_pub_tx.prt_ind[0]!='1' )
        goto GoodExit;

    vtcp_log("<<<<<<<<<<<<<<PRT_LINE====[%d]",g_mdm_ac_rel.prt_line);
    /*add by xx 判断存折满提示换折*/
    if(strncmp(v_mdm_attr.mdm_code,"0016",4))
    {
    if( g_mdm_ac_rel.prt_line % totalnum == 0 )
    {
        strcpy( g_pub_tx.reply, "P509" );
        sprintf(acErrMsg,"存折已满先换折再做交易![%d]",ret);
        WRITEMSG
        goto ErrExit;
    }
    }

    ret=pub_base_patch_book(v_mdm_attr);
    if( ret==0 )    set_zd_data("0110","0");
    else if( ret>0 )    set_zd_data("0110","1");
    else
    {
        sprintf(acErrMsg,"补登折错误![%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    vtcp_log("<<<<<<<<<<<<<<PRT_LINE2====[%d]",g_mdm_ac_rel.prt_line);
    vtcp_log("补登折PASSPASSPASSPASSPASSPASS!!");   

GoodExit:
    return 0;
ErrExit:
    return 1;
}
pub_sub_mst()
{
    int ret=0 ;
    sprintf(acErrMsg,"[%s][%d]svc_ind=[%d]",__FILE__,__LINE__,g_pub_tx.svc_ind);
    WRITEMSG

    vtcp_log("[%s][%d] HST_BAL==[%.2lf] ACBO_BAL==[%.2lf] ", __FILE__,__LINE__, g_dd_mst.hst_bal,g_dd_mst.acbo_bal);
    vtcp_log("[%s][%d] TX_AMT1==[%.2lf] TX_AMT2==[%.2lf] ", __FILE__,__LINE__, g_pub_tx.tx_amt1,g_pub_tx.tx_amt2);
    /*** 检查处理分户帐,明细账 ***/
    switch( g_pub_tx.ac_id_type[0] )
    {
        case '1':   
            ret=Chg_dd_mst();
            ERR_DEAL
            break;
        case '2':   
            ret=Chg_td_mst();
            ERR_DEAL
            break;
        case '3':   
            ret=Chg_ln_mst();
            ERR_DEAL
            break;
        case '9':   
            ret=Chg_in_mst();
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
pub_sub_spe_do()
{
    int ret;
    int cnt1,cnt2;
    struct pub_tx v_pub_tx;

    /** 不允许用红子记账(单边记账交易出外) **/
    vtcp_log("[%s],[%d] g_pub_tx.tx_code=[%s]",__FILE__,__LINE__,g_pub_tx.tx_code);
    if( strcmp(g_pub_tx.tel,"9999") &&
        strcmp(g_pub_tx.tx_code,"2502") && strcmp(g_pub_tx.tx_code,"LNRE") &&
        strcmp(g_pub_tx.tx_code,"ZTSC") && strcmp(g_pub_tx.tx_code,"7711") &&
        strcmp(g_pub_tx.tx_code,"5839") &&
        strcmp(g_pub_tx.tx_code,"4162") &&
        strcmp(g_pub_tx.tx_code,"4334") &&
        strcmp(g_pub_tx.tx_code,"5840") &&
        strcmp(g_pub_tx.tx_code,"5380") &&
        strcmp(g_pub_tx.tx_code,"3206") &&
        strcmp(g_pub_tx.tx_code,"6203") &&
        strcmp(g_pub_tx.tx_code,"D131") &&
        strcmp(g_pub_tx.tx_code,"3319") &&
        strcmp(g_pub_tx.tx_code,"D129") &&
        strcmp(g_pub_tx.tx_code,"8619") &&
        strcmp(g_pub_tx.tx_code,"CFEE") &&
        /*BEGIN ADD BY XJ 20101021 */
        strcmp(g_pub_tx.tx_code,"6789") &&
        /*END ADD BY XJ 20101021 */
	/***add zgf 20131031 联机单笔冲正验证借口***/
        strcmp(g_pub_tx.tx_code,"6235") &&/*单笔冲账接口发生额必须是负数 Added by YangMeng 2014/7/12 17:22:09*/
        strcmp(g_pub_tx.tx_code,"6247") && /*批量冲账接口发生额必须是负数 Added by YangMeng 2014/7/12 17:22:09*/
         strcmp(g_pub_tx.tx_code,"7023") &&/*主机对账冲账接口发生额必须是负数 Added by YangMeng 2014/7/12 17:22:09*/
        strcmp(g_pub_tx.tx_code,"7024") &&/*主机对账冲账接口发生额必须是负数 Added by YangMeng 2014/7/12 17:22:09*/
        strcmp(g_pub_tx.tx_code,"7028") &&/*主机对账冲账接口发生额必须是负数 Added by YangMeng 2014/7/12 17:22:09*/
        strcmp(g_pub_tx.tx_code,"6254") &&/*主机对账冲账接口发生额必须是负数 Added by YangMeng 2014/7/12 17:22:09*/
        strcmp(g_pub_tx.tx_code,"5111") &&/*客户凭证注销退费 冲账 Added by wudawei 2014/8/6 15:20:09*/
        strcmp(g_pub_tx.tx_code,"6234") &&/*单笔转账接口发生额必须为负 Added by guanjianfeng 2015/8/20 15:20:09*/
        strcmp(g_pub_tx.tx_code,"7102") &&/*财务单笔支付收款记账接口，撤销交易7102 lipeijin 20151027*/
        strcmp(g_pub_tx.tx_code,"9014") &&/*直贴/买断式转贴现转入票据信息查询批量接口,直贴票据信息查询9014  chenfuqiang 20151029*/
        /***add zgf end***/
	strcmp(g_pub_tx.tel,"9999") &&
         (pub_base_CompDblVal(g_pub_tx.tx_amt1,0.00)<0 
        || pub_base_CompDblVal(g_pub_tx.tx_amt2,0.00)<0 
        || pub_base_CompDblVal(g_pub_tx.tx_amt3,0.00)<0 
        || pub_base_CompDblVal(g_pub_tx.tx_amt4,0.00)<0) )
    {
        sprintf(acErrMsg,"发生额不能为负数![%lf][%lf][%lf][%lf]",g_pub_tx.tx_amt1,g_pub_tx.tx_amt2,g_pub_tx.tx_amt3,g_pub_tx.tx_amt4);
        WRITEMSG
        strcpy(g_pub_tx.reply,"P121");
        goto ErrExit;
    }

    /*** 检查是否有理财产品 **** 前面检查过来 ***
    if( g_pub_tx.add_ind[0]=='2' )
    {
        ret=sql_count("mo_prdt_prot"," main_ac_id=%ld and sts='1' ",g_pub_tx.ac_id,g_pub_tx.ac_id);
        if( ret!=0 )
        {
            sprintf(acErrMsg,"该户有理财或协定产品不能销户![%d]",ret);
            WRITEMSG
            strcpy(g_pub_tx.reply,"P182");
            goto ErrExit;
       }
    }
    ****/

    if( g_pub_tx.svc_ind==1003 || g_pub_tx.svc_ind==1004 
        || g_pub_tx.svc_ind==1010 || g_pub_tx.svc_ind==9002 )
    {
        ret=pub_base_InsMo_opn_cls(g_pub_tx.name,"","2");
        if( ret )
        {
            sprintf(acErrMsg,"登记开销户登记簿出错!");
            WRITEMSG
            goto ErrExit;
        }
    }

    /*** 如果是存折(非一本通)、存单销介质 ***/
    if( g_pub_tx.svc_ind==1003 || g_pub_tx.svc_ind==1004
            || g_pub_tx.svc_ind==1009 || g_pub_tx.svc_ind==1010 )
    {
        if( !strcmp(g_mdm_ac_rel.mdm_code,"0016") )
        {
vtcp_log("CNT 一本通不销介质[%s][%s]",v_mdm_attr.mdm_code,v_mdm_attr.ac_num_ind);
        }
        else if( !strcmp(v_mdm_attr.mdm_code,NULL_MDM_CODE) 
            || v_mdm_attr.ac_num_ind[0]=='Y' )
        {
vtcp_log("CNT [%s][%s]",v_mdm_attr.mdm_code,v_mdm_attr.ac_num_ind);
            strcpy( v_pub_tx.opn_br_no,g_pub_tx.opn_br_no );
            ret=pub_acct_cls_mdm();
            if( ret )
            {
                sprintf(acErrMsg,"调用销介质模块败!");
                WRITEMSG
                goto ErrExit;
            }
            strcpy( g_pub_tx.opn_br_no,v_pub_tx.opn_br_no );
        }
        else
        {
            /*
            cnt1=sql_count("dd_mst","ac_id=%ld and ac_sts='1' ",g_mdm_ac_rel.ac_id );
            if( cnt1<0 )
            {
                strcpy( g_pub_tx.reply,"D103" );
                goto ErrExit;
            }
            cnt2=sql_count("td_mst","ac_id=%ld and ac_sts='1' ",g_mdm_ac_rel.ac_id );
            if( cnt2<0 )
            {
                strcpy( g_pub_tx.reply,"D103" );
                goto ErrExit;
            }
vtcp_log("CNT [%d][%d]",cnt1,cnt2);
            if( cnt1+cnt2==0 )
            {
                ret=pub_acct_cls_mdm();
                if( ret )
                {
                    sprintf(acErrMsg,"调用销介质模块败!");
                    WRITEMSG
                    goto ErrExit;
                }
            }
            */
        }
    }
    return 0;
ErrExit:
    return 1;
}
pub_sub_liqu()
{
    int ret;

    /*** 通存通兑修改407台帐 ***/
    if( strcmp(g_pub_tx.tx_br_no,g_pub_tx.opn_br_no) )
    {
        /*
        ret=pubf_ass_liqu();
        if( ret )
        {
            sprintf(acErrMsg,"修改考核台帐出错!");
            WRITEMSG
            goto ErrExit;
        }
        */
    }
    return 0;
ErrExit:
    return 1;
}

int pub_sub_prot(double tmpje)
{
    struct mo_prdt_prot_c   mo_prdt_prot_c;
    int ret=0;
    memset(&mo_prdt_prot_c,0x00,sizeof(struct mo_prdt_prot_c));
    
    vtcp_log("理财协定划款开始!");
    ret=Mo_prdt_prot_Sel(g_pub_tx.reply,&mo_prdt_prot_c,"main_ac_id=%ld and main_ac_seqn=%d and sts='1' ",g_pub_tx.ac_id,g_pub_tx.ac_seqn);
    if( ret==100 )  
    {
        sprintf(acErrMsg,"无协定户或理财户![%d][%d]",g_pub_tx.ac_id,g_pub_tx.ac_seqn);
        WRITEMSG
        return 0;
    }
    else if( ret )
    {
        sprintf(acErrMsg,"检查是否为协定理财户出错![%ld][%d][%d]",g_pub_tx.ac_id,g_pub_tx.ac_seqn,ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"P280");
        goto ErrExit;
    }
    
    switch(mo_prdt_prot_c.prot_type[0])
    {
        case 'X':
                if( pub_sub_prot_x(tmpje,mo_prdt_prot_c) )
                {
                    sprintf(acErrMsg,"协定划款化回错误!");
                    WRITEMSG
                    goto ErrExit;
                }
                break;
        case 'T':
                if( pub_sub_prot_t(tmpje,mo_prdt_prot_c) )
                {
                    sprintf(acErrMsg,"理财户划款化回错误!");
                    WRITEMSG
                    goto ErrExit;
                }
                break;
        default:
            sprintf(acErrMsg,"协定划款非可日间化回型!");
            WRITEMSG
            break;
    }
    vtcp_log("理财协定划款成功!");
    
    return 0;
ErrExit:
    return 1;
}

int pub_sub_prot_x(double tmpje,struct mo_prdt_prot_c mo_prdt_prot_c)
{
    int ret=0;
    struct mo_prot_ac_rel_c     mo_prot_ac_rel_c;
    struct dd_mst_c             prot_dd_mst;

    struct pub_tx           tmp_pub_tx;
    struct mdm_ac_rel_c     tmp_mdm_ac_rel;
    struct dd_mst_c         tmp_dd_mst_c;
    struct prdt_ac_id_c     tmp_prdt_ac_id_c;
    struct mdm_attr_c       tmp_mdm_attr_c;
    struct note_parm_c      tmp_note_parm_c;
    char   main_dc_code[5];
    char   T_ct_ind[2];
    double T_tx_amt=0.00;

    memset(&mo_prot_ac_rel_c,0x00,sizeof(struct mo_prot_ac_rel_c));
    memset(&prot_dd_mst,0x00,sizeof(struct dd_mst_c));

    memset(&tmp_pub_tx,0x00,sizeof(struct pub_tx));
    memset(&tmp_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
    memset(&tmp_dd_mst_c,0x00,sizeof(struct dd_mst_c));
    memset(&tmp_prdt_ac_id_c,0x00,sizeof(struct prdt_ac_id_c));
    memset(&tmp_mdm_attr_c,0x00,sizeof(struct mdm_attr_c));
    memset(&tmp_note_parm_c,0x00,sizeof(struct note_parm_c));
    get_zd_data("102K",T_ct_ind);
    get_zd_double("102I",&T_tx_amt);

    ret=Mo_prot_ac_rel_Sel(g_pub_tx.reply,&mo_prot_ac_rel_c,"main_ac_id=%ld and main_ac_seqn=%d and sts='1' ",g_pub_tx.ac_id,g_pub_tx.ac_seqn);
    if( ret )
    {
        sprintf(acErrMsg,"取协定账号错误![%d][%d][%d]",g_pub_tx.ac_id,g_pub_tx.ac_seqn,ret);
        WRITEMSG
        return 0;
    }

    ret=pub_base_currentid_file(mo_prot_ac_rel_c.app_ac_id,mo_prot_ac_rel_c.app_ac_seqn,&prot_dd_mst);
    if( ret )
    {
        sprintf(acErrMsg,"取协定账号主文件错误!");
        WRITEMSG
        return 0;
    }

    if( pub_base_CompDblVal(prot_dd_mst.bal,tmpje)<0 )
    {
        sprintf(acErrMsg,"协定户余额不足!");
        WRITEMSG
        return 0;
    }
    if( pub_base_CompDblVal(prot_dd_mst.bal,tmpje)>0 )/* 转回部分add by martin 2009/6/16 17:30:28*/
        {
        }else
            {
          tmpje=prot_dd_mst.bal;    /** 结算户余额不足，一笔化回 **/
        }
        vtcp_log("%s,%d g_pub_tx.tx_amt1=[%lf]",__FILE__,__LINE__,g_pub_tx.tx_amt1);

    memcpy(&tmp_pub_tx,&g_pub_tx,sizeof(struct pub_tx));
    memcpy(&tmp_mdm_ac_rel,&g_mdm_ac_rel,sizeof(struct mdm_ac_rel_c));
    memcpy(&tmp_dd_mst_c,&g_dd_mst,sizeof(struct dd_mst_c));
    memcpy(&tmp_prdt_ac_id_c,&prdt_ac_id_c,sizeof(struct prdt_ac_id_c));
    memcpy(&tmp_mdm_attr_c,&mdm_attr_c,sizeof(struct mdm_attr_c));
    memcpy(&tmp_note_parm_c,&note_parm_c,sizeof(struct note_parm_c));

    /*** 结算户存款 ***/
    if( pub_base_CompDblVal(prot_dd_mst.bal,tmpje)>0 )/*转回部分add by martin 2009/6/16 17:30:28*/
        {
                g_pub_tx.tx_amt1=tmpje;
        }else
            {
            g_pub_tx.tx_amt1=prot_dd_mst.bal;
        }
 vtcp_log("%s,%d g_pub_tx.tx_amt1=[%lf]",__FILE__,__LINE__,g_pub_tx.tx_amt1);
  /**g_pub_tx.tx_amt1=prot_dd_mst.bal; del by martin 2009/6/16 17:30:17**/
    strcpy(g_pub_tx.ct_ind,"2");
    strcpy(g_pub_tx.add_ind,"1");
    strcpy(g_pub_tx.hst_ind,"1");
    strcpy(g_pub_tx.ac_wrk_ind,"0000");
    g_pub_tx.svc_ind=1001;
    strcpy(g_pub_tx.ac_id_type,"");
    strcpy(g_pub_tx.note_type,"");
    strcpy(g_pub_tx.beg_note_no,"");
    strcpy(g_pub_tx.end_note_no,"");
    g_reply_int=pub_acct_trance();
    if( g_reply_int )
    {
        sprintf(acErrMsg,"调用存取款主控失败!");
        WRITEMSG
        goto ErrExit;
    }
    set_zd_data("102K",g_pub_tx.ct_ind);
    /**set_zd_double("102I",g_pub_tx.tx_amt1);**/
    set_zd_double("1002",g_pub_tx.tx_amt1);
    strcpy(main_dc_code,g_pub_tx.dc_code);
    vtcp_log("%s,%bbbbbbbb=[%f]",__FILE__,__LINE__,g_pub_tx.tx_amt1);
    vtcp_log("%s,%d tx_br_no=[%s],opn_br_no=[%s]",__FILE__,__LINE__,g_pub_tx.tx_br_no,g_pub_tx.opn_br_no);
    g_reply_int=pubf_acct_proc("LC02");
    if( g_reply_int )
    {
        sprintf(acErrMsg,"登记会计流水错误!");
        WRITEMSG
        goto ErrExit;
    }

    tmp_dd_mst_c.bal=g_dd_mst.bal;
    tmp_dd_mst_c.acbo_bal=g_dd_mst.acbo_bal;
    tmp_dd_mst_c.hst_bal=g_dd_mst.hst_bal;
    tmp_dd_mst_c.intst_acm=g_dd_mst.intst_acm;
    tmp_dd_mst_c.hst_cnt=g_dd_mst.hst_cnt;
    tmp_dd_mst_c.lst_date=g_dd_mst.lst_date;
    strcpy(tmp_dd_mst_c.mac,g_dd_mst.mac);
    vtcp_log("[%s][%d] HST_BAL==[%.2lf] ACBO_BAL==[%.2lf] ", __FILE__,__LINE__,g_dd_mst.hst_bal,g_dd_mst.acbo_bal);

    /*** 协定户取款 ***/
    strcpy(g_pub_tx.ac_no,"");
    g_pub_tx.ac_id=mo_prot_ac_rel_c.app_ac_id;
    g_pub_tx.ac_seqn=mo_prot_ac_rel_c.app_ac_seqn;
    strcpy(g_pub_tx.draw_uncon_yn,"Y"); 
    strcpy(g_pub_tx.ct_ind,"2");
    strcpy(g_pub_tx.add_ind,"0"); /**减少**/
    strcpy(g_pub_tx.hst_ind,"1"); /**日间入明细**/
    strcpy(g_pub_tx.ac_wrk_ind,"0000");
    g_pub_tx.svc_ind=1008; /**存取款***/
    if( pub_base_CompDblVal(prot_dd_mst.bal,tmpje)>0 )/*转回部分add by martin 2009/6/16 17:30:28*/
        {
                g_pub_tx.tx_amt1=tmpje;
        }else
            {
            g_pub_tx.tx_amt1=prot_dd_mst.bal;
        }
vtcp_log("%s,%d g_pub_tx.tx_amt1=[%lf]",__FILE__,__LINE__,g_pub_tx.tx_amt1);
    /*g_pub_tx.tx_amt1=prot_dd_mst.bal;*/
    strcpy(g_pub_tx.ac_id_type,"");
    strcpy(g_pub_tx.note_type,"");
    strcpy(g_pub_tx.beg_note_no,"");
    strcpy(g_pub_tx.end_note_no,"");
    ret=pub_acct_trance();
    if( ret )
    {
        sprintf(acErrMsg,"调用存取款主控失败!");
        WRITEMSG
        goto ErrExit;
    }
    set_zd_data("102K",g_pub_tx.ct_ind);
    /****改用100域第1子域  modify at 20130218
    set_zd_double("102I",g_pub_tx.tx_amt1);
    ****/
    set_zd_double("1002",g_pub_tx.tx_amt1);
    strcpy(main_dc_code,g_pub_tx.dc_code); /* 加了20301单位协定存款后  划款记账错误 更正 add by wjl 20131022*/
    set_zd_data(DC_CODE,main_dc_code);
    vtcp_log("%s,%eeeeeeee=[%f],main_dc_code=[%s]",__FILE__,__LINE__,g_pub_tx.tx_amt1,main_dc_code);
    vtcp_log("%s,%d tx_br_no=[%s],opn_br_no=[%s]",__FILE__,__LINE__,g_pub_tx.tx_br_no,g_pub_tx.opn_br_no);
    g_reply_int=pubf_acct_proc("LC03");
    if( g_reply_int )
    {
        sprintf(acErrMsg,"登记会计流水错误!");
        WRITEMSG
        goto ErrExit;
    }

    set_zd_double("102I",T_tx_amt);
    memcpy(&g_pub_tx,&tmp_pub_tx,sizeof(struct pub_tx));
    memcpy(&g_mdm_ac_rel,&tmp_mdm_ac_rel,sizeof(struct mdm_ac_rel_c));
    memcpy(&g_dd_mst,&tmp_dd_mst_c,sizeof(struct dd_mst_c));
    memcpy(&prdt_ac_id_c,&tmp_prdt_ac_id_c,sizeof(struct prdt_ac_id_c));
    memcpy(&mdm_attr_c,&tmp_mdm_attr_c,sizeof(struct mdm_attr_c));
    memcpy(&note_parm_c,&tmp_note_parm_c,sizeof(struct note_parm_c));
    set_zd_data("102K",T_ct_ind);

    return 0;
ErrExit:
    return 1;
}

int pub_sub_prot_t(double tmpje,struct mo_prdt_prot_c mo_prdt_prot_c)
{
    int ret=0;
    double turn_amt=0.00;
    struct mo_prot_ac_rel_c     mo_prot_ac_rel_c;
    struct td_mst_c             prot_td_mst;
    char T_ct_ind[2];
    double T_tx_amt=0.00;

    struct pub_tx tmp_pub_tx;
    struct mdm_ac_rel_c tmp_mdm_ac_rel;
    struct dd_mst_c tmp_dd_mst_c;
    struct prdt_ac_id_c tmp_prdt_ac_id_c;
    struct mdm_attr_c tmp_mdm_attr_c;
    struct note_parm_c tmp_note_parm_c;

    memset(&mo_prot_ac_rel_c,0x00,sizeof(struct mo_prot_ac_rel_c));
    memset(&prot_td_mst,0x00,sizeof(struct td_mst_c));

    memset(&tmp_pub_tx,0x00,sizeof(struct pub_tx));
    memset(&tmp_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
    memset(&tmp_dd_mst_c,0x00,sizeof(struct dd_mst_c));
    memset(&tmp_prdt_ac_id_c,0x00,sizeof(struct prdt_ac_id_c));
    memset(&tmp_mdm_attr_c,0x00,sizeof(struct mdm_attr_c));
    memset(&tmp_note_parm_c,0x00,sizeof(struct note_parm_c));

    memcpy(&tmp_pub_tx,&g_pub_tx,sizeof(struct pub_tx));
    memcpy(&tmp_mdm_ac_rel,&g_mdm_ac_rel,sizeof(struct mdm_ac_rel_c));
    memcpy(&tmp_dd_mst_c,&g_dd_mst,sizeof(struct dd_mst_c));
    memcpy(&tmp_prdt_ac_id_c,&prdt_ac_id_c,sizeof(struct prdt_ac_id_c));
    memcpy(&tmp_mdm_attr_c,&mdm_attr_c,sizeof(struct mdm_attr_c));
    memcpy(&tmp_note_parm_c,&note_parm_c,sizeof(struct note_parm_c));
    get_zd_data("102K",T_ct_ind);
    get_zd_double("102I",&T_tx_amt);

    ret=Mo_prot_ac_rel_Dec_Upd(g_pub_tx.reply,"main_ac_id=%ld and main_ac_seqn=%d and sts='1' ",g_pub_tx.ac_id,g_pub_tx.ac_seqn);
    if( ret )
    {
        sprintf(acErrMsg,"取协定账号错误!");
        WRITEMSG
        return 0;
    }

    while(1)
    {
        ret=Mo_prot_ac_rel_Fet_Upd(&mo_prot_ac_rel_c,g_pub_tx.reply);
        if( ret==100 ) break;
        if( ret )
        {
            sprintf(acErrMsg,"取协定账号错误!");
            WRITEMSG
            strcpy(g_pub_tx.reply,"P181");
            return 1;
        }

        ret=pub_base_termiid_file(mo_prot_ac_rel_c.app_ac_id,mo_prot_ac_rel_c.app_ac_seqn,&prot_td_mst);
        if( ret )
        {
            sprintf(acErrMsg,"取协定账号主文件错误!");
            WRITEMSG
            return 0;
        }

        /*** 理财户销户 ***/
        strcpy(g_pub_tx.ac_no,"");
        g_pub_tx.ac_id=mo_prot_ac_rel_c.app_ac_id;
        g_pub_tx.ac_seqn=mo_prot_ac_rel_c.app_ac_seqn;
        strcpy(g_pub_tx.draw_uncon_yn,"Y"); 
        strcpy(g_pub_tx.ct_ind,"2");
        strcpy(g_pub_tx.add_ind,"0");
        strcpy(g_pub_tx.hst_ind,"1"); /**日间入明细**/
        strcpy(g_pub_tx.ac_wrk_ind,"0000");
        g_pub_tx.svc_ind = 1004;
        g_pub_tx.tx_amt1=prot_td_mst.bal;
        strcpy(g_pub_tx.ac_id_type,"");
        ret=pub_acct_trance();
        if( ret )
        {
            sprintf(acErrMsg,"调用存取款主控失败!");
            WRITEMSG
            goto ErrExit;
        }
        set_zd_data("102K",g_pub_tx.ct_ind);
        set_zd_double("102I",g_pub_tx.tx_amt1);
        set_zd_double("102P",g_pub_intst.dealval+g_pub_intst.keepval);
        set_zd_double("102Q",g_pub_intst.val);
    vtcp_log("%s,%d tx_br_no=[%s],opn_br_no=[%s]",__FILE__,__LINE__,g_pub_tx.tx_br_no,g_pub_tx.opn_br_no);
        g_reply_int=pubf_acct_proc("LC01");
        if( g_reply_int )
        {
            sprintf(acErrMsg,"登记会计流水错误!");
            WRITEMSG
            goto ErrExit;
        }

        strcpy(mo_prot_ac_rel_c.sts,"*");
        ret=Mo_prot_ac_rel_Upd_Upd(mo_prot_ac_rel_c,g_pub_tx.reply);
        if( ret )
        {
            sprintf(acErrMsg,"mo_prot_ac_rel_upd_upd erro ![%d]",ret);
            WRITEMSG
            strcpy(g_pub_tx.reply,"P283");
            goto ErrExit;
        }

        turn_amt=turn_amt+g_pub_tx.tx_amt1+g_pub_intst.factval+g_pub_intst.keepval;
        if( tmpje<turn_amt )
            break;
    }
    Mo_prot_ac_rel_Clo_Upd();

    /*** 结算户存款 ***/
    strcpy(g_pub_tx.ct_ind,"2");
    strcpy(g_pub_tx.add_ind,"1");
    strcpy(g_pub_tx.hst_ind,"1");
    strcpy(g_pub_tx.ac_wrk_ind,"0000");
    g_pub_tx.svc_ind=1001;
    g_pub_tx.tx_amt1=turn_amt;
    strcpy(g_pub_tx.ac_id_type,"");
    g_reply_int=pub_acct_trance();
    if( g_reply_int )
    {
        sprintf(acErrMsg,"调用存取款主控失败!");
        WRITEMSG
        goto ErrExit;
    }
    set_zd_data("102K",g_pub_tx.ct_ind);
    /**set_zd_double("102I",g_pub_tx.tx_amt1);**/
    set_zd_double("1002",g_pub_tx.tx_amt1);
    vtcp_log("%s,%cccccccc=[%f]",__FILE__,__LINE__,g_pub_tx.tx_amt1);
    vtcp_log("%s,%d tx_br_no=[%s],opn_br_no=[%s]",__FILE__,__LINE__,g_pub_tx.tx_br_no,g_pub_tx.opn_br_no);
    g_reply_int=pubf_acct_proc("LC02");
    if( g_reply_int )
    {
        sprintf(acErrMsg,"登记会计流水错误!");
        WRITEMSG
        goto ErrExit;
    }

    tmp_dd_mst_c.bal=g_dd_mst.bal;
    tmp_dd_mst_c.acbo_bal=g_dd_mst.acbo_bal;
    tmp_dd_mst_c.hst_bal=g_dd_mst.hst_bal;
    tmp_dd_mst_c.intst_acm=g_dd_mst.intst_acm;
    tmp_dd_mst_c.hst_cnt=g_dd_mst.hst_cnt;
    tmp_dd_mst_c.lst_date=g_dd_mst.lst_date;
    strcpy(tmp_dd_mst_c.mac,g_dd_mst.mac);
    vtcp_log("[%s][%d] HST_BAL==[%.2lf] ACBO_BAL==[%.2lf] ",  __FILE__,__LINE__,g_dd_mst.hst_bal,g_dd_mst.acbo_bal);

    memcpy(&g_pub_tx,&tmp_pub_tx,sizeof(struct pub_tx));
    memcpy(&g_mdm_ac_rel,&tmp_mdm_ac_rel,sizeof(struct mdm_ac_rel_c));
    memcpy(&g_dd_mst,&tmp_dd_mst_c,sizeof(struct dd_mst_c));
    memcpy(&prdt_ac_id_c,&tmp_prdt_ac_id_c,sizeof(struct prdt_ac_id_c));
    memcpy(&mdm_attr_c,&tmp_mdm_attr_c,sizeof(struct mdm_attr_c));
    memcpy(&note_parm_c,&tmp_note_parm_c,sizeof(struct note_parm_c));
    set_zd_data("102K",T_ct_ind);
    set_zd_double("102I",T_tx_amt);
    set_zd_double("102P",0.00);
    set_zd_double("102Q",0.00);

    return 0;
ErrExit:
    return 1;
}

int pub_sub_chkadd(double ys_bal,double tx_amt1)
{
    int ret=0;
    double tmpamt=0.00;
    double tmpamt1=0.00;
    double tmpamt2=0.00;

    /***目前不需要判断异地存取可取金额***/
    return(0);

    /***临时TEMP**/
    if( !strncmp(g_pub_tx.tx_code,"9001",4) ) 
        return 0;

    ret=sql_sum_double("trace_log","amt",&tmpamt, 
        "sts='0' and (sub_tx_code='D099' or sub_tx_code='A017') and tx_code!='6604' and ac_id=%d and ac_seqn=%d and tx_br_no!='%s' and saving_notarize='0' ",
        g_pub_tx.ac_id,g_pub_tx.ac_seqn,g_pub_tx.tx_br_no);
    if( ret )
    {
        sprintf(acErrMsg,"计算本网点可取金额失败!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"P295");
        goto ErrExit;
    }
    if( pub_base_CompDblVal(tmpamt,0.00)==0 )
    {
        sprintf(acErrMsg,"没有需要确认的存款!");
        WRITEMSG
        return 0;
    }

    ret=sql_sum_double("trace_log","amt",&tmpamt1,
    "sts='0' and ac_id=%d and ac_seqn=%d and (((sub_tx_code='D099' or sub_tx_code='A017') and (tx_br_no='%s' or saving_notarize='1')) or sub_tx_code='D002' or tx_code='6604' ) ",
        g_pub_tx.ac_id,g_pub_tx.ac_seqn,g_pub_tx.tx_br_no); 
    if( ret )
    {
        sprintf(acErrMsg,"计算本网点可取金额失败!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"P295");
        goto ErrExit;
    }

    ret=sql_sum_double("trace_log","amt",&tmpamt2,
        "sts='0' and (sub_tx_code='D003' or sub_tx_code='A016') and ac_id=%ld and ac_seqn=%d ",
        g_pub_tx.ac_id,g_pub_tx.ac_seqn); 
    if( ret )
    {
        sprintf(acErrMsg,"计算本网点可取金额失败!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"P295");
        goto ErrExit;
    }

    vtcp_log("ys_bal=[%lf],tmpamt1=[%lf],tx_amt1=[%lf],tmpamt2=[%f]",
        ys_bal,tmpamt1,tx_amt1,tmpamt2);

    if( pub_base_CompDblVal(ys_bal+tmpamt1,tx_amt1+tmpamt2)<0 )
    {
        sprintf(acErrMsg,"本网点可取金额不足，请其他网点确认存款!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"P296");
        goto ErrExit;
    }

    return 0;
ErrExit:
    return 1;
}

pub_acct_NoteUse_log( ac_id, ac_seqn, note_type, beg_no, end_no, cnt )
 long ac_id,ac_seqn;
 char *note_type;
 char *beg_no;
 char *end_no;
 long cnt;
{
    struct pub_tx v_pub_tx;
    int ret=0;

    memcpy( &v_pub_tx,&g_pub_tx,sizeof(g_pub_tx) );
    
    g_pub_tx.ct_ind[0]=' ';
    strcpy(g_pub_tx.ac_no,"702");
    g_pub_tx.ac_seqn=0;
    /*g_pub_tx.ac_id=ac_id;*/
    g_pub_tx.ac_id=0;
    g_pub_tx.svc_ind=7021;
    strcpy( g_pub_tx.note_type,note_type );
    strcpy( g_pub_tx.beg_note_no,beg_no );
    strcpy( g_pub_tx.end_note_no,end_no );
    strcpy( g_pub_tx.prdt_code,"" );
    strcpy( g_pub_tx.cur_no,"01" );
    g_pub_tx.add_ind[0]='0';
    g_pub_tx.hst_ind[0]='1';
    g_pub_tx.tx_amt1=(double)cnt;

    ret=pub_ins_trace_log();
    if( ret ) 
    {
        TRACE
        goto ErrExit;
    }
TRACE
    get_zd_double("0520",&g_pub_tx.tx_amt2);
    set_zd_double("0520",g_pub_tx.tx_amt1);
    vtcp_log("%s,%d tx_br_no=[%s],opn_br_no=[%s]",__FILE__,__LINE__,g_pub_tx.tx_br_no,g_pub_tx.opn_br_no);
    ret=pubf_acct_proc( "N001" );
    if( ret )
    {
        goto ErrExit;
    }
    set_zd_double("0520",g_pub_tx.tx_amt2);

    memcpy( &g_pub_tx,&v_pub_tx,sizeof(g_pub_tx) );
TRACE   
    return 0;
ErrExit:
    return 1;
}

/* add by xxx at 20030727 **/
int pub_chk_JYCX()
{
    double t_amt=0.00;
    int ret=0;
    struct td_mst_hst_c sTd_mst_hst;

    memset(&sTd_mst_hst, 0x0, sizeof(struct td_mst_hst_c));

    /* 教育储蓄一个月内不能存2万 */
    sprintf(acErrMsg, "CGH1*** 教育储蓄检查!!");
    WRITEMSG

    if (g_pub_tx.tx_date / 100 == g_td_mst.opn_date / 100)
    {
        sprintf(acErrMsg, "CGH2*** 教育储蓄检查!!");
        WRITEMSG

        ret = Td_mst_hst_Dec_Sel(g_pub_tx.reply, "ac_id=%ld and \
            ac_seqn=%d order by tx_date", g_td_mst.ac_id, g_td_mst.ac_seqn);
        if (ret != 0)
        {
            sprintf(acErrMsg, "DECLARE FOR SELECT ERROR !! [%d]", ret);
            WRITEMSG
            goto ErrExit;
        }

        while(1)
        {
            memset(&sTd_mst_hst, 0x0, sizeof(struct td_mst_hst_c));
            ret = Td_mst_hst_Fet_Sel(&sTd_mst_hst, g_pub_tx.reply);
            if (ret != 0 && ret != 100)
            {
                sprintf(acErrMsg, "Fetch for select error !! [%d]", ret);
                WRITEMSG
                goto ErrExit;
            }
            else if (ret == 100)
            {
                break;
            }

            if (sTd_mst_hst.tx_date / 100 == g_pub_tx.tx_date / 100)
            {
                 t_amt += sTd_mst_hst.tx_amt;
            }
            else
            {
                break;
            }
        }
        Td_mst_hst_Clo_Sel();

        t_amt += g_pub_tx.tx_amt1;
        if (pub_base_CompDblVal(t_amt, 20000.00) >= 0)
        {
            sprintf(acErrMsg, "一个月内不能存超过两万!! [%lf]", t_amt);
            WRITEMSG
            strcpy(g_pub_tx.reply, "O241");
            goto ErrExit;
        }
    }
    return 0;
ErrExit:
    return 1;
}
int pub_acct_ac_rec( ac_id,ac_seqn,add,ct,tx_code,amt,ind )
 long ac_id;
 int ac_seqn;
 char *add;
 char *ct;
 char *tx_code;
 double amt;
 char *ind;
{
    int ret=0;
    struct auth_ac_rec_c auth_ac_rec;
    char dc[2];

    dc[0]=add[0]+1;
    dc[1]='\0';

    if( !strcmp(g_pub_tx.tel,"9999") ) return 0;
    if( !strcmp(tx_code,"5702") ) return 0;

    ret=Auth_ac_rec_Dec_Upd( g_pub_tx.reply,
        "ac_id=%ld and ac_seqn=%d and dc='%s' and ct='%s' ",
        ac_id,ac_seqn,dc,ct );
    ERR_DEAL

    ret=Auth_ac_rec_Fet_Upd( &auth_ac_rec,g_pub_tx.reply );
    if( ret==100 )
    {
        memset( &auth_ac_rec,0,sizeof(auth_ac_rec) );
        auth_ac_rec.ac_id=ac_id;
        auth_ac_rec.ac_seqn=ac_seqn;
        memcpy( auth_ac_rec.dc,dc,sizeof(auth_ac_rec.dc)-1);
        memcpy( auth_ac_rec.ct,ct,sizeof(auth_ac_rec.ct)-1);
        auth_ac_rec.amt=amt;
        memcpy( auth_ac_rec.tx_code,tx_code,sizeof(auth_ac_rec.tx_code)-1);
        ret=Auth_ac_rec_Ins( auth_ac_rec,g_pub_tx.reply );
        if( ret ) goto ErrExit;
    }
    else if( ret )
    {
        Auth_ac_rec_Clo_Upd( );
        goto ErrExit;
    }
    else
    {
        auth_ac_rec.amt+=amt;
        ret=Auth_ac_rec_Upd_Upd( auth_ac_rec,g_pub_tx.reply  );
        if( ret ) goto ErrExit;
    }

    Auth_ac_rec_Clo_Upd( );

    return 0;
ErrExit:
    sprintf(acErrMsg, "更新帐户授权累计金额表出错!! [%d]",ret);
    WRITEMSG
    Auth_ac_rec_Debug(&auth_ac_rec);
    strcpy(g_pub_tx.reply, "D103");
    return 1;
}
/*******************************
add by chenchao 2011-7-31 17:21:40 
1.如果传入的支付协议号为空，则取活期的自有资金部分,即除了(贷款放款额-已自主支取-已委托支取)之外的部分
2.如果传入支付协议号非空，则取贷款部分的金额,即(贷款放款额-已自主支取-已委托支取-支付许可金额)之外的部分
add/modify by yanqq 2011-12-13 11:20
1.如果当前是预授权交易时取得当前预授权控制金额，tmp值加当前预授权控制金额
*******************************/
int dd_limit()
{
    int    ret;
    char   cDraw_no[61];
    double dRest_amt=0.0,temp;
    double dBal_virtul = 0.0;                     
    long premise_no;                                /* add by yanqq 20111213 追加预授权处理 */
    char premise_flg[2];                            /* add by yanqq 20111213 预授权判断标志 */
    char chongzheng_flg[2];                         /* add by yanqq 20111213 冲正判断标志 */

    struct draw_loan_hst_c   sDraw_hst;
    struct dd_mst_c          sDd_mst;
    struct chnl_premise_c mChnl_premise;            /* add by yanqq 20111213 追加预授权处理 */

    memset(cDraw_no,0x00,sizeof(cDraw_no));
    memset(&mChnl_premise,0x0,sizeof(struct chnl_premise_c));   /* add by yanqq 20111213 追加预授权处理 */
    memset(premise_flg,0x00,sizeof(premise_flg));               /* add by yanqq 20111213 预授权判断标志 */
    memset(chongzheng_flg,0x00,sizeof(chongzheng_flg));         /* add by yanqq 20111213 冲正判断标志 */

    if(pub_base_CompDblVal(g_pub_tx.tx_amt1,0.00)==0)
        return(0);

    /* add beg by yanqq 20111213 取得当前预授权号对应的控制金额 */
    if(strcmp(g_pub_tx.tx_code,"6203")==0)
    {
        /* 6203交易中【70】域值为2,3,4时，为预授权类交易 */
        get_zd_data("0700",premise_flg);
        /* 6203交易中【67】域值为0时，为非冲正类交易 */
        get_zd_data("0670",chongzheng_flg);
        if((premise_flg[0] == '2'|| premise_flg[0] == '3'|| premise_flg[0] == '4') && chongzheng_flg[0] == '0')
	    {
	        get_zd_long("0870",&premise_no); 
            ret = Chnl_premise_Sel(g_pub_tx.reply,&mChnl_premise,"ac_no='%s' and premise_no=%ld",g_pub_tx.crd_no,premise_no);
            vtcp_log("%s,%d,当前帐卡号=[%s],当前预授权号=[%ld],控制金额=[%lf]",__FILE__,__LINE__,g_pub_tx.crd_no,premise_no,mChnl_premise.premise_amt);
        }
    }
    /* add end by yanqq 20111213 */

    memset(&sDd_mst,0x00,sizeof(sDd_mst));
    ret=Dd_mst_Sel(g_pub_tx.reply,&sDd_mst,"ac_id=%ld and ac_seqn=%d",g_pub_tx.ac_id,g_pub_tx.ac_seqn);
    if(ret)
    {
        vtcp_log("[%s][%d]活期表读取错误[%d]",__FILE__,__LINE__,ret);
        strcpy(g_pub_tx.reply,"W014");
        return 1;
    }

    if(sDd_mst.od_ind[0]=='Y')  /* 可透支户就不控制 */
        return (0);

    if(strcmp(g_pub_tx.tx_code,"5802")==0 ||strcmp(g_pub_tx.tx_code,"5842")==0)
    {
        get_zd_data("1188",cDraw_no);
    }
    else
    {
        get_zd_data("1126",cDraw_no);
    }
    zip_space(cDraw_no);
    vtcp_log("[%s][%d]支取协议：%s",__FILE__,__LINE__,cDraw_no);
    if(strlen(cDraw_no)==0)/**没有传入支取协议号**/
    {
        ret=limit_draw_no_less(&dRest_amt);
        if(ret)
        {
            vtcp_log("[%s][%d] branch 1 许可可用金额：%.2f",__FILE__,__LINE__,dRest_amt);
            return 1;
        }
    }
    else                   /**传入支取协议号**/
    {
        /*  by Wang Yongwei 对与有支付单编号的，一个交易如果下两次账(收费)状态就变为已支取了 20101201*/
        if(memcmp(g_pub_tx.sub_tx_code,"SF",2)==0)
        {
            vtcp_log("%s,%d,收费不检查是否委托支付!",__FILE__,__LINE__);
        }else{
            ret=limit_draw_no_yes(&dRest_amt,cDraw_no);
            if(ret)
            {
                vtcp_log("[%s][%d]branch 2 许可可用金额：%.2f",__FILE__,__LINE__,dRest_amt);
                return 1;
            }
        }
    }
		/**add by zyl 20121211 7天乐产品检查,并修改temp的计算**/
    ret = iGet7DaysVtlBal(g_pub_tx.ac_id,&dBal_virtul);
    if(ret)
    {
        vtcp_log("[%s][%d] 检查发生错误...",__FILE__,__LINE__);
        return 1;
    }
    /**end by zyl 20121211**/
    /* modify beg by yanqq 20111213 追加预授权处理 */
    /* temp=sDd_mst.bal-sDd_mst.hold_amt-sDd_mst.ctl_amt-dRest_amt-g_pub_tx.tx_amt1; */
    /* vtcp_log("[%s][%d]sDd_mst.bal=%.2f sDd_mst.hold_amt=%.2f sDd_mst.ctl_amt=%.2f dRest_amt=%.2f g_pub_tx.tx_amt1=%.2f  temp=%.2f ",__FILE__,__LINE__,sDd_mst.bal,sDd_mst.hold_amt,sDd_mst.ctl_amt,dRest_amt,g_pub_tx.tx_amt1,temp); */
    temp=sDd_mst.bal-sDd_mst.hold_amt-sDd_mst.ctl_amt-dRest_amt-g_pub_tx.tx_amt1+mChnl_premise.premise_amt+dBal_virtul;/**temp增加dBal_virtul**/
    vtcp_log("[%s][%d]sDd_mst.bal=%.2f sDd_mst.hold_amt=%.2f sDd_mst.ctl_amt=%.2f mChnl_premise.premise_amt=%.2f dRest_amt=%.2f g_pub_tx.tx_amt1=%.2f  temp=%.2f ",__FILE__,__LINE__,sDd_mst.bal,sDd_mst.hold_amt,sDd_mst.ctl_amt,mChnl_premise.premise_amt,dRest_amt,g_pub_tx.tx_amt1,temp);
    /* modify end by yanqq 20111213 */
    if(pub_base_CompDblVal(temp,0.00)>=0)
    {
        return (0);
    }
    else 
    {
	if(pub_base_CompDblVal(dRest_amt,0.00)>0) /*add by lwb 20150508*/
	{
        	sprintf(acErrMsg,"贷款委托支付未授权  "); 
		strcpy(g_pub_tx.reply,"H034");
		set_zd_data("0130",acErrMsg);
		WRITEMSG
       		return 1;
		
	}
	else {
        vtcp_log("[%s][%d]输入的金额或信贷审批支取金额错误,或余额已被扣划",__FILE__,__LINE__);
        strcpy(g_pub_tx.reply,"LS51");
        return 1;
	}
    }
}
int limit_draw_no_less(double *amt)
{
    int    ret;
    struct draw_loan_limit_c sDraw_limit;
    vtcp_log("[%s][%d]进入limit_draw_no_less",__FILE__,__LINE__);
    ret=Draw_loan_limit_Dec_Sel(g_pub_tx.reply,"ac_id=%ld and sts='2'",g_pub_tx.ac_id);
    if(ret)
    {
        vtcp_log("[%s][%d]Draw_loan_limit游标定义错误[%d]",__FILE__,__LINE__,ret);
        strcpy(g_pub_tx.reply,"D101");
        goto ErrExit;
    }
    (*amt)=0.0;
    while(1)
    {
        memset(&sDraw_limit,0x00,sizeof(sDraw_limit));
        ret=Draw_loan_limit_Fet_Sel(&sDraw_limit,g_pub_tx.reply);
        if(ret==100)
        {
            break;
        }
        else if(ret)
        {
            vtcp_log("[%s][%d]Draw_loan_limit取游标错误[%d]",__FILE__,__LINE__,ret);
            strcpy(g_pub_tx.reply,"D102");
            goto ErrExit;
        }
        /* 暂时只限制委托支付,不限制自主支付 2011-7-31 17:23:34
        (*amt)+=sDraw_limit.save_amt-sDraw_limit.trust_draw_amt-sDraw_limit.self_draw_amt;
        ***********/

        *amt += sDraw_limit.trust_amt-sDraw_limit.trust_draw_amt;
    }
    Draw_loan_limit_Clo_Sel();
    vtcp_log("[%s][%d] 正常结束 %.2f",__FILE__,__LINE__,*amt);
OkExit:
    return 0;
ErrExit:
    return 1;
}
int limit_draw_no_yes(double *amt,char *cDraw_no)
{
    int    ret;
    double approvel_amt;
    double dTmp1,dTmp2;
    struct draw_loan_limit_c sDraw_limit;
    struct draw_loan_hst_c   sDraw_hst;
    approvel_amt=0.0;
    vtcp_log("[%s][%d]进入limit_draw_no_yes amt=%.2f",__FILE__,__LINE__,amt);
    memset(&sDraw_limit,0x00,sizeof(sDraw_limit));
    memset(&sDraw_hst,0x00,sizeof(sDraw_hst));
    ret=limit_draw_no_less(&approvel_amt);
    if(ret)
    {  
        vtcp_log("[%s][%d] ret=%d approvel_amt=%.2f",__FILE__,__LINE__,ret,approvel_amt);
        strcpy(g_pub_tx.reply,"B384");
        goto ErrExit;
    }
    if(pub_base_CompDblVal(approvel_amt,0.0)<0)
    {
        vtcp_log("[%s][%d] approvel_amt=%.2f",__FILE__,__LINE__,amt);
        strcpy(g_pub_tx.reply,"B384");
        goto ErrExit;
    }

    /* by Wang Yongwei 这里只是控制委托支付自主支付不在控制 20101215*/
    ret=Draw_loan_hst_Dec_Upd(g_pub_tx.reply,"draw_no='%s' and ac_id=%ld and sts='1' and st_flag='2'",cDraw_no,g_pub_tx.ac_id);
    if(ret)
    {
        vtcp_log("[%s][%d]Draw_loan_hst定义游标错误[%d]",__FILE__,__LINE__,ret);
        strcpy(g_pub_tx.reply,"D101");
        goto ErrExit;
    }
    ret=Draw_loan_hst_Fet_Upd(&sDraw_hst,g_pub_tx.reply);
    if(ret)
    {
        vtcp_log("[%s][%d]Draw_loan_hst取游标错误",__FILE__,__LINE__,ret);
        strcpy(g_pub_tx.reply,"D104");
        goto ErrExit;
    }
    sDraw_hst.sts[0]='2';
    sDraw_hst.trace_no=g_pub_tx.trace_no;
    sDraw_hst.tx_date=g_pub_tx.tx_date; /* by Wang Yongwei 20110131 */
    strcpy(sDraw_hst.tel,g_pub_tx.tel);
    approvel_amt-=sDraw_hst.draw_amt;
    *amt=approvel_amt;
    vtcp_log("[%s][%d] 在输入支取单编号的情况下可以支取的贷款余额:  %.2f",__FILE__,__LINE__,*amt);
    ret=Draw_loan_hst_Upd_Upd(sDraw_hst,g_pub_tx.reply);
    if(ret)
    {
        vtcp_log("[%s][%d]Draw_loan_hst更新错误[%d]",__FILE__,__LINE__,ret);
        strcpy(g_pub_tx.reply,"D105");
        goto ErrExit;
    }
    Draw_loan_hst_Clo_Upd();

    pub_base_strpack(sDraw_hst.pact_no);

    ret=Draw_loan_limit_Dec_Upd(g_pub_tx.reply,"pact_no='%s' and ac_id=%ld and sts='2'",sDraw_hst.pact_no,sDraw_hst.ac_id);
    if(ret)
    {
        vtcp_log("[%s][%d]Draw_loan_limit定义游标错误[%d]",__FILE__,__LINE__,ret);
        strcpy(g_pub_tx.reply,"D101");
        goto ErrExit;
    }
    ret=Draw_loan_limit_Fet_Upd(&sDraw_limit,g_pub_tx.reply);
    if(ret)
    {
        vtcp_log("[%s][%d]Draw_loan_limit取游标错误[%d]",__FILE__,__LINE__,ret);
        strcpy(g_pub_tx.reply,"D102");
        goto ErrExit;
    }
    if(sDraw_hst.st_flag[0]=='1')
    {
        dTmp2=sDraw_limit.self_amt-sDraw_limit.self_draw_amt-sDraw_hst.draw_amt;
        if(pub_base_CompDblVal(dTmp2,0.0)<0)
        {
            vtcp_log("[%s][%d]审批的自主支付金额大于可取的自主支付金额",__FILE__,__LINE__);
            strcpy(g_pub_tx.reply,"SN41");
            goto ErrExit;
        }
        sDraw_limit.self_draw_amt+=sDraw_hst.draw_amt;
    }
    else
    {
        dTmp1=sDraw_limit.trust_amt-sDraw_limit.trust_draw_amt-sDraw_hst.draw_amt;
        if(pub_base_CompDblVal(dTmp1,0.0)<0)
        {
            vtcp_log("[%s][%d]审批的委托支付金额大于可取的自主委托支付金额",__FILE__,__LINE__);
            strcpy(g_pub_tx.reply,"SN41");
            goto ErrExit;
        }
        sDraw_limit.trust_draw_amt+=sDraw_hst.draw_amt;
    }
    dTmp1=sDraw_limit.trust_amt-sDraw_limit.trust_draw_amt;
    dTmp2=sDraw_limit.self_amt-sDraw_limit.self_draw_amt;

    /* by Wang Yongwei 这里只是检查委托支付就可以了 20101215
    if( pub_base_CompDblVal(dTmp1,0.0)==0 && pub_base_CompDblVal(dTmp2,0.0)==0 )
    {
        sDraw_limit.sts[0]='*';
    }
    **/
    if(pub_base_CompDblVal(dTmp1,0.0)==0 )
    {
        sDraw_limit.sts[0]='*';
        sDraw_limit.self_draw_amt=sDraw_limit.self_amt; 
    }

    ret=Draw_loan_limit_Upd_Upd(sDraw_limit,g_pub_tx.reply);
    if(ret)
    {
        vtcp_log("[%s][%d]Draw_loan_limit更新数据库错误[%d]",__FILE__,__LINE__,ret);
        strcpy(g_pub_tx.reply,"D105");
        goto ErrExit;
    }
    Draw_loan_limit_Clo_Upd();

OkExit:
    return 0;
ErrExit:
    set_zd_data("0120",g_pub_tx.reply);
    return 1;
}
/**根据主账户ac_id,得到7天乐虚拟账户余额.若未签约,则余额为0 add by zyl 20121211**/
int iGet7DaysVtlBal(long ac_id,double *bal)
{
    struct mo_prdt_prot_c sMo_prdt_prot;
    struct mo_prot_ac_rel_c sMo_prot_ac_rel;
    struct dd_mst_c sDd_mst_virtual;
    memset(&sDd_mst_virtual,0x00,sizeof(struct dd_mst_c));
    memset(&sMo_prdt_prot,0x00,sizeof(struct mo_prdt_prot_c));
    memset(&sMo_prot_ac_rel,0x00,sizeof(struct mo_prot_ac_rel_c));
    int ret = 0;
    ret = Mo_prdt_prot_Sel(g_pub_tx.reply,&sMo_prdt_prot," prot_type='TK7' and sts!='*' and main_ac_id = %ld ",ac_id);
    if(ret == 100)
    {
        *bal = 0.0;
    }
    else if(ret)
    {
        sprintf(acErrMsg,"Mo_prdt_prot_Sel error ret=[%d]",ret);
            WRITEMSG
        strcpy( g_pub_tx.reply,"D103");
        return 1;
    }
    else
    {        
        ret = Mo_prot_ac_rel_Sel(g_pub_tx.reply,&sMo_prot_ac_rel," prot_type='TK7' and sts!='*' and main_ac_id = %ld ",ac_id);
        if(ret == 100)
        {
          /**七天乐签约信息存在.与虚拟账户对照关系不存在.**/
            sprintf(acErrMsg,"七天乐签约信息存在.与虚拟账户对照关系不存在.");
                WRITEMSG
            strcpy( g_pub_tx.reply,"D104");
            return 1;
        }
        else if(ret)
        {
            sprintf(acErrMsg,"Mo_prot_ac_rel_Sel error ret=[%d]",ret);
                WRITEMSG
            strcpy( g_pub_tx.reply,"D103");
            return 1;
        }
        
        /**2.七天乐账户信息..签约账户,虚拟账户信息已取得.**/
        ret = Dd_mst_Sel(g_pub_tx.reply,&sDd_mst_virtual," ac_id = %ld ",sMo_prot_ac_rel.app_ac_id);
        if(ret == 100)
        {
            sprintf(acErrMsg,"dd_mst中无虚拟账户信息.");
                WRITEMSG
            strcpy( g_pub_tx.reply,"D104");
            return 1;
        }
        else if(ret)
        {
            sprintf(acErrMsg,"Dd_mst_Sel error ret=[%d]",ret);
                WRITEMSG
            strcpy( g_pub_tx.reply,"D103");
            return 1;
        }
        *bal = sDd_mst_virtual.bal;
        
    }
    return 0;
}
/*七天乐虚拟账户计算利息*/
int app_cal_intst(cSts, dd_mst_c, dd_parm_c)
char   *cSts;
struct dd_mst_c  dd_mst_c;
struct dd_parm_c dd_parm_c;
{
    vtcp_log("[%s][%d],签约状态cSts=[%s]\n",__FILE__,__LINE__,cSts);
    int    ret=0;
    long   lDays=0;
    double dLx=0.00;       /*应付利息*/
    double dLxacm=0.00;    /*利息积数*/
    double dHq_rate=0.00; /*利率*/
    double dLxs=0.00;      /*利息税*/
    double dLxsl=0.00;     /*利息税税率*/
    double dSflx=0.00;     /*实付利息*/
    
    struct intst_hst_c T_intst_hst;
    struct dd_mst_c tmp_dd_mst;
    
    memset(&T_intst_hst, 0x00, sizeof(struct intst_hst_c));
    memset(&g_pub_intst, 0x0, sizeof(g_pub_intst));
    memset(&tmp_dd_mst, 0x00, sizeof(struct dd_mst_c));
    
    
    memcpy(&tmp_dd_mst, &dd_mst_c, sizeof(tmp_dd_mst));
    memcpy(&g_dd_parm, &dd_parm_c, sizeof(g_dd_parm));
    
    if(cSts[0]=='1')  /*状态为1的时候才可以临时解约*/
    {
        /*临时解约的要取当前的活期利率*/
        ret = pub_base_getllz("100","01",g_pub_tx.tx_date,&dHq_rate);
        if (ret != 0)
        {
            sprintf(acErrMsg,"从公共利率表中取利率值错误 [%d]",ret);
            WRITEMSG
            strcpy(g_pub_tx.reply,"W110");
            goto ErrExit;
        }
        vtcp_log("[%s][%d],活期利率dHq_rate=[%.2lf]\n",__FILE__,__LINE__,&dHq_rate);
        /**滚积数结息**没有算出来积数，如何算利息？add at 20120207****/
        if( pub_base_CalAcm(tmp_dd_mst.lst_date,g_pub_tx.tx_date, \
          g_dd_parm.intst_term_type,g_dd_parm.acm_calc_type, \
          tmp_dd_mst.bal,g_pub_tx.tx_amt1,&tmp_dd_mst.intst_acm, \
          tmp_dd_mst.intst_type,g_dd_parm.intst_mon,g_dd_parm.intst_day) )
          {
              sprintf(acErrMsg,"计算利息积数错误!");
              WRITEMSG
              goto ErrExit;
          }
        /****end at 20120207****/
        
        vtcp_log("[%s][%d],cSts=[%s],dHq_rate=[%.2lf] 利息积数=[%.2lf]\n",__FILE__,__LINE__,cSts,dHq_rate,tmp_dd_mst.intst_acm);
        dLx = (tmp_dd_mst.intst_acm * dHq_rate)/L360/L100;/**应付利息**/
        vtcp_log("[%s][%d],dLx=[%.2lf]\n",__FILE__,__LINE__,dLx);
        dLx = pub_base_PubDround_1(dLx);
        /*如果有利息税，是按活期利息税收还是按七天乐这种产品的利息税来收?*/
        if(strcmp(g_dd_parm.tax_no,SLBH) == 0)
        {
            ret = pub_base_GetRate(SLBH,g_pub_tx.tx_date, &dLxsl);
            if ((ret != 0) && (ret == 1))
            {
                sprintf(acErrMsg,"利息税税率不存在 ret=[%d]",ret);
                WRITEMSG
                strcpy(g_pub_tx.reply,"W110");
                goto ErrExit;
            }
            dLxs=dLx*dLxsl;
            dLxs=pub_base_PubDround_1(dLxs) ;/**利息税**/
            vtcp_log("[%s][%d]汇总记帐dLx=[%.2lf]dLxs=[%.2lf]dLxsl=[%.2lf]",__FILE__,__LINE__,dLx,dLxs,dLxsl);
            dSflx=dLx-dLxs;
        }
        g_pub_intst.dealval=dLx;
        g_pub_intst.val=dLxs;
        vtcp_log("[%s][%d] sts=[%s],dealval=[%.2f],val=[%.2lf]",__FILE__,__LINE__,cSts,g_pub_intst.dealval,g_pub_intst.val);     
    }
    else
    {
       sprintf(acErrMsg, "类型错误!");
       WRITEMSG
       strcpy(g_pub_tx.reply, "C108");
       goto ErrExit;
    }
    vtcp_log("[%s][%d],准备登记结息登记簿\n",__FILE__,__LINE__);
    strcpy(T_intst_hst.br_no, tmp_dd_mst.opn_br_no);
    T_intst_hst.ac_id = tmp_dd_mst.ac_id;
    T_intst_hst.ac_seqn = tmp_dd_mst.ac_seqn;
    strcpy(T_intst_hst.prdt_no, tmp_dd_mst.prdt_no);
    strcpy(T_intst_hst.cur_no, g_dd_parm.cur_no);
    T_intst_hst.bic_bal = tmp_dd_mst.bal;
    T_intst_hst.bic_acm = tmp_dd_mst.intst_acm;
    T_intst_hst.fct_intst = g_pub_intst.dealval;
    T_intst_hst.intst_tax = g_pub_intst.val;
    T_intst_hst.aic_bal = tmp_dd_mst.bal;
    T_intst_hst.aic_acm = 0.00;
    T_intst_hst.ic_date = tmp_dd_mst.ic_date;    
    T_intst_hst.intst_date = g_pub_tx.tx_date;
    strcpy(T_intst_hst.ac_ind, "1");
    strcpy(T_intst_hst.pt_ind, "1");
    T_intst_hst.rate = dHq_rate;
    T_intst_hst.intst_date=g_pub_tx.tx_date;
    ret = Intst_hst_Ins(T_intst_hst, g_pub_tx.reply);
    if( ret )
    {
       sprintf(acErrMsg,"SQLERR [%d]",ret);
       WRITEMSG
       goto ErrExit;
    }
    /*更新活期主文件*/
    ret = Dd_mst_Dec_Upd(g_pub_tx.reply, "ac_id=%d and ac_seqn=%d",tmp_dd_mst.ac_id, tmp_dd_mst.ac_seqn);
    if( ret )
    {
       sprintf(acErrMsg,"SQLERR [%d]",ret);
       WRITEMSG
       goto ErrExit;
    }
    ret = Dd_mst_Fet_Upd(&tmp_dd_mst, g_pub_tx.reply);
    if( ret )
    {
       sprintf(acErrMsg,"SQLERR [%d]",ret);
       WRITEMSG
       goto ErrExit;
    }
    tmp_dd_mst.intst_acm = 0.00;/**应付利息积数**/
    tmp_dd_mst.lst_date=g_pub_tx.tx_date;
    tmp_dd_mst.ic_date=g_pub_tx.tx_date;
    
    ret = Dd_mst_Upd_Upd(tmp_dd_mst, g_pub_tx.reply);
    if( ret )
    {
       sprintf(acErrMsg,"SQLERR [%d]",ret);
       WRITEMSG
       goto ErrExit;
    }
    vtcp_log("[%s][%d],更新活期主表结束\n",__FILE__,__LINE__);
    Dd_mst_Clo_Upd();
    return 0;
ErrExit:
    return 1;
} 
/*七天乐临时解约--结息记账*/
   int main_intst_acct(dd_mst_c, dd_parm_c, sum_intst)
    struct dd_mst_c dd_mst_c;
    struct dd_parm_c dd_parm_c;
    struct S_pub_intst sum_intst;
    {
        int  ret = 0;
        char tmp_ac_no[25];
        struct dd_mst_c v_dd_mst;
        struct dd_parm_c v_dd_parm;
        
        memset(&v_dd_mst,0x00,sizeof(struct dd_mst_c));
        memset(&v_dd_parm,0x00,sizeof(struct dd_parm_c));
        memset(tmp_ac_no,0x00,sizeof(tmp_ac_no));
        
        memcpy(&v_dd_mst, &dd_mst_c, sizeof(v_dd_mst));
        memcpy(&v_dd_parm, &dd_parm_c, sizeof(v_dd_parm));
        
        vtcp_log("[%s][%d] 帐号=[%s] 帐户ID=[%d] ",__FILE__,__LINE__,g_pub_tx.ac_no,g_pub_tx.ac_id);
        /****不需要赋此值 20130220
        strcpy(g_pub_tx.tx_br_no, v_dd_mst.opn_br_no);
        ****/
        strcpy(g_pub_tx.opn_br_no, v_dd_mst.opn_br_no);
        strcpy(g_pub_tx.cur_no, v_dd_parm.cur_no);
        strcpy(g_pub_tx.ac_wrk_ind, "0000010011");
        g_pub_tx.svc_ind = 1005;
        strcpy(g_pub_tx.ct_ind, "2");
        strcpy(g_pub_tx.add_ind, "1");
        strcpy(g_pub_tx.prdt_code, v_dd_parm.prdt_no);
        g_pub_tx.ac_id = v_dd_mst.ac_id;
        g_pub_tx.ac_seqn = v_dd_mst.ac_seqn;
        strcpy(g_pub_tx.note_type, "");
        strcpy(g_pub_tx.beg_note_no, "");
        g_pub_tx.tx_amt1 = sum_intst.dealval;
        strcpy(g_pub_tx.brf, "七天通知存款划款结息");
        /* 进行利息的交易记帐处理 */
        vtcp_log("[%s][%d] v_dd_mst帐户ID=[%d] ",__FILE__,__LINE__,v_dd_mst.ac_id);
        vtcp_log("[%s][%d] 帐号=[%s] 帐户ID=[%d] ",__FILE__,__LINE__,g_pub_tx.ac_no,g_pub_tx.ac_id);
        ret = pub_acct_trance();
        if (ret)
        {
            sprintf(acErrMsg, "记账错误!!");
            WRITEMSG
            goto ErrExit1;
        }    
        /* 进行利息税的交易记帐处理 */
        vtcp_log("[%s][%d] 帐号=[%s] 帐户ID=[%d] ",__FILE__,__LINE__,g_pub_tx.ac_no,g_pub_tx.ac_id);
        strcpy(g_pub_tx.brf, "七天通知存款划款结息");
        strcpy(g_pub_tx.no_show, "0");
        strcpy(g_pub_tx.add_ind, "0");
        strcpy(g_pub_tx.ac_wrk_ind, "0000010011");
        strcpy(g_pub_tx.hst_ind, "1");
        g_pub_tx.tx_amt1 = sum_intst.val;
        vtcp_log("[%s][%d] 帐号=[%s] 帐户ID=[%d] ",__FILE__,__LINE__,g_pub_tx.ac_no,g_pub_tx.ac_id);
        ret = pub_acct_trance();
        if (ret)
        {
            sprintf(acErrMsg, "记账错误!!");
            WRITEMSG
            goto ErrExit1;
        }   
        /* 进行会计记帐 */
        vtcp_log("[%s][%d] 帐号=[%s] 帐户ID=[%d] ",__FILE__,__LINE__,g_pub_tx.ac_no,g_pub_tx.ac_id);
        set_zd_data(DC_CODE, v_dd_parm.dc_code);
        set_zd_double("102P", sum_intst.dealval-sum_intst.val);
        set_zd_double("102Q", sum_intst.val);
        set_zd_data("102J", v_dd_parm.cur_no);
        set_zd_data("102K", g_pub_tx.ct_ind);/****赋予转帐标志****/
        strcpy(g_pub_tx.prdt_code, v_dd_mst.prdt_no);
        /****用储蓄销户计息子交易代码****/
        strcpy(g_pub_tx.sub_tx_code,"D202");
        vtcp_log("[%s][%d] 帐号=[%s] 帐户ID=[%d] ",__FILE__,__LINE__,g_pub_tx.ac_no,g_pub_tx.ac_id);
        ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
        if (ret != 0)
        {
            sprintf(acErrMsg, "会计记帐不成功!!");
            WRITEMSG
            goto ErrExit1;
        }
        
        return 0;
    ErrExit1:
        return 1;
    }
