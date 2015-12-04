/*************************************************
* 文 件 名:     pubf_eod_crd.c
* 功能描述：    日终卡类处理
* 作    者:     jack
* 完成日期：    2003年07月30日
* 修改记录：
*
* 日    期:
* 修 改 人:
* 修改内容:
*************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "com_parm_c.h"
#include "com_chrg_rate_c.h"
#include "public.h"
#include "mdm_attr_c.h"
#include "com_chrg_rate_c.h"
#include "com_parm_c.h"
#include "dc_acc_rel_c.h"
#include "com_chrg_hst_c.h"
#include "mdm_ac_rel_c.h"
#include "dd_mst_c.h"
#include "dd_mst_hst_c.h"
#include "com_item_c.h"
#include "card_reg_c.h"
#include "com_branch_c.h"
#include "com_sys_parm_c.h"
#include "card.h"


/**********************************************************************
 * 函 数 名：   pub_eod_crd_chrg
 * 函数功能：   日终卡收费处理 
 * 作者/时间：  
 *
 * 参数：
 *     输入: 收费代码
 *     输出: 
 *     返回: <>0失败
             =0成功
 *
 * 修改历史：
 *
********************************************************************/
int pub_eod_crd_chrg(char chrg_rate_code[4])
{
	int ret=0;
	struct com_chrg_rate_c  sCom_chrg_rate;
	memset(&sCom_chrg_rate,0x00,sizeof(struct com_chrg_rate_c));
	memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
	memset(&g_dd_parm,0x00,sizeof(struct dd_parm_c));

	ret=Com_chrg_rate_Sel(g_pub_tx.reply,&sCom_chrg_rate,"chrg_rate_code='%s' ",chrg_rate_code);
	if( ret )
	{
		printf(acErrMsg, "取费率代码错误!![%d][%s]",ret,chrg_rate_code);
		WRITEMSG
		strcpy(g_pub_tx.reply,"W117");
		return -1;
	}
	/*** 检查余额是否够 ***/
	ret=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,"ac_id=%ld and ac_seqn=1 ",g_mdm_ac_rel.ac_id);
	if( ret )
	{
		printf(acErrMsg, "取活期主文件错误!![%d][%s]",ret,g_mdm_ac_rel.ac_id);
		WRITEMSG
		strcpy(g_pub_tx.reply,"W117");
		return 1;
	}
	ret=Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm,"prdt_no='%s' ",g_dd_mst.prdt_no);
	if( ret )
	{
		printf(acErrMsg, "取活期产品参数误!![%d][%s]",ret,g_dd_mst.prdt_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"W024");
		return 1;
	}
	strcpy(g_pub_tx.cur_no,g_dd_parm.cur_no);
	if( g_dd_mst.hold_sts[0]=='1' || g_dd_mst.hold_sts[0]=='2' )
		return 1;
	else if( g_dd_mst.hold_sts[0]=='3' )
	{
		ret=pub_base_CompDblVal(sCom_chrg_rate.val,g_dd_mst.bal-g_dd_mst.hold_amt-g_dd_mst.ctl_amt-g_dd_parm.min_bal);
		if( ret>0 )		
			return 1;
	}
	else	/*** 非冻结 ***/
	{
		ret=pub_base_CompDblVal(sCom_chrg_rate.val,g_dd_mst.bal-g_dd_mst.ctl_amt-g_dd_parm.min_bal);
		if( ret>0 )		
			return 1;
	}
	strcpy(g_pub_tx.ac_no,g_mdm_ac_rel.ac_no);
	g_pub_tx.ac_id=g_mdm_ac_rel.ac_id;
	g_pub_tx.ac_seqn=1;
	strcpy(g_pub_tx.id_type,g_mdm_ac_rel.id_type);
	strcpy(g_pub_tx.id_no,g_mdm_ac_rel.id_no);
	g_pub_tx.tx_amt1=sCom_chrg_rate.val;
	strcpy(g_pub_tx.ct_ind,"2");
	strcpy(g_pub_tx.brf,"年费");
	strcpy(g_pub_tx.add_ind,"0"); /**减少**/
	strcpy(g_pub_tx.prt_ind,"0"); /*是否打折标志*/
	strcpy(g_pub_tx.hst_ind,"1"); /**日间入明细**/
	strcpy(g_pub_tx.ac_wrk_ind,"000000001");
	strcpy(g_pub_tx.ac_get_ind,"111");
	g_pub_tx.svc_ind=1001; /**存取款***/
	ret=pub_acct_trance();
	if( ret )
	{
		sprintf(acErrMsg,"调用存取款主控失败!");
		WRITEMSG
		return 1;
	}
	return 0;
}

/**********************************************************************
* 函 数 名：   pub_eod_crd_chrg_chk
* 函数功能：   日终卡收费处理 
* 作者/时间：  
*
* 参数：
    double *amt        为收费金额,输入参数同时返回可使用最小金额
    char    chrg_typ    '1' 扣账户可用最小金额，'0' 金额不足不处理
    char    *fld_brf    扣费描述
    char    *succmsg    为处理结果信息，出错返回错误原因
    
*     输入: 收费代码
*     输出: 
*     返回: <>0失败
             =0成功
*
* 修改历史：
*/
int pub_eod_crd_chrg_chk(double *amt, char chrg_typ, char* fld_brf, char *succmsg)
{
    int ret=0;
    memset(&g_dd_parm,0x00,sizeof(struct dd_parm_c));
    strcpy(g_pub_tx.tx_br_no,g_mdm_ac_rel.opn_br_no);
    strcpy(g_pub_tx.opn_br_no,g_mdm_ac_rel.opn_br_no);
    vtcp_log("[%s][%d],进入分户收年费,%lf, bal[%lf]",__FILE__,__LINE__,*amt, g_dd_mst.bal);
    /*** 检查余额是否够 ***/
    ret=Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm,"prdt_no='%s' ",g_dd_mst.prdt_no);
    if ( ret )
    {
        printf(acErrMsg, "取活期产品参数误!![%d][%s]",ret,g_dd_mst.prdt_no);
        WRITEMSG
        strcpy(succmsg, "产品错误");
        return 1;
    }
    strcpy(g_pub_tx.cur_no,g_dd_parm.cur_no);
    vtcp_log("%s,%d,进入分户收年费,%lf, bal[%lf]",__FILE__,__LINE__,*amt, g_dd_mst.bal);
    Dd_mst_Debug(&g_dd_mst);
    if ( g_dd_mst.hold_sts[0]=='1' || g_dd_mst.hold_sts[0]=='2' ){
        *amt=0;
        strcpy(succmsg, "账户状态不正常");
        return 1;
    }
    else if ( g_dd_mst.hold_sts[0]=='3' )
    {
        vtcp_log("%s,%d,进入分户收年费,%lf",__FILE__,__LINE__,*amt);
        ret=pub_base_CompDblVal(*amt,g_dd_mst.bal-g_dd_mst.hold_amt-g_dd_mst.ctl_amt-g_dd_parm.min_bal);
        if(ret>0  && '1' == chrg_typ )
        {
            *amt=g_dd_mst.bal-g_dd_mst.hold_amt-g_dd_mst.ctl_amt-g_dd_parm.min_bal; 
            if(*amt<0.00001)
            {
                *amt=0;
                strcpy(succmsg, "余额不足");
                return 1;
            }
            else
            {
                strcpy(succmsg, "扣费成功");
            }
        }else if ( ret>0 ) /* by Wang Yongwei 20100414 应该是else没有加 */
        {
            strcpy(succmsg, "账户状态不正常");
            return 1;
        }
        vtcp_log("%s,%d,进入分户收年费,%lf",__FILE__,__LINE__,*amt);
    }
    else    /*** 非冻结 ***/
    {
        vtcp_log("%s,%d,进入分户收年费,%lf",__FILE__,__LINE__,*amt);
        ret=pub_base_CompDblVal(*amt,g_dd_mst.bal-g_dd_mst.ctl_amt-g_dd_parm.min_bal);
        if(ret>0  && '1' == chrg_typ )
        {
            vtcp_log("%s,%d,进入分户收年费,%lf ,可用 %lf %lf %lf %lf ",__FILE__,__LINE__,*amt, g_dd_mst.bal, g_dd_mst.ctl_amt, g_dd_parm.min_bal, g_dd_mst.bal-g_dd_mst.ctl_amt-g_dd_parm.min_bal); 
            *amt=g_dd_mst.bal-g_dd_mst.ctl_amt-g_dd_parm.min_bal;
            vtcp_log("%s,%d,进入分户收年费,%lf",__FILE__,__LINE__,*amt); 
            if(*amt<0.00001)
            {
                *amt=0;
                strcpy(succmsg, "余额不足");
                return 1;
            }
            else
            {
                strcpy(succmsg, "扣费成功");
            }
            ret=0;
        }
        if ( ret>0 )
        {
            strcpy(succmsg, "账户状态不正常");
            return 1;
        }
    }
    vtcp_log("%s,%d,进入分户收年费,%lf",__FILE__,__LINE__,*amt);
    strcpy(g_pub_tx.ac_no,g_mdm_ac_rel.ac_no);
    g_pub_tx.ac_id=g_mdm_ac_rel.ac_id;
    g_pub_tx.ac_seqn=1;
    strcpy(g_pub_tx.id_type,g_mdm_ac_rel.id_type);
    strcpy(g_pub_tx.id_no,g_mdm_ac_rel.id_no);
    g_pub_tx.tx_amt1=*amt;
    strcpy(g_pub_tx.ct_ind,"2");
    strcpy(g_pub_tx.brf,fld_brf);
    strcpy(g_pub_tx.add_ind,"0"); /**减少**/
    strcpy(g_pub_tx.ac_id_type, "1");    /* 活期账户 */
    strcpy(g_pub_tx.prt_ind,"0"); /*是否打折标志*/
    strcpy(g_pub_tx.hst_ind,"1"); /**日间入明细**/
    strcpy(g_pub_tx.ac_wrk_ind,"000000001");
    strcpy(g_pub_tx.ac_get_ind,"111");
    g_pub_tx.svc_ind=1001; /**存取款***/
    ret=pub_acct_trance();
    if ( ret )
    {
        sprintf(acErrMsg,"调用存取款主控失败!");
        WRITEMSG
        strcpy(succmsg, "记账失败");
        return 1;
    }
    set_zd_data("102O", "0");
    set_zd_data("102J", "01");
    set_zd_data("102K", "2");
    set_zd_double("102F", g_pub_tx.tx_amt1);
    set_zd_double("102P", 0.00);
    set_zd_double("102Q", 0.00);
    ret = pubf_acct_proc("D003");
    if (ret){
        sprintf(acErrMsg, "调用存取款主控失败[%s]", g_pub_tx.reply);
        WRITEMSG
        strcpy(succmsg, "记账失败");
        return 1;
    }
    vtcp_log("%s,%d,分户成功,%lf",__FILE__,__LINE__,*amt);
    return 0;
}
/**********************************************************************
* 函 数 名：   pub_eod_crd_fee
* 函数功能：   卡收费用处理
* 作者/时间：   gujy
*
* 参数：
*     输入:  feetyp 为一个串，第一位代表年费，第二位为小额管理费，如果收则为1
            如收年费为'10',收小额管理费为'01',都收为'11'
        fee_br_no    收费机构，为空则为所有机构
        fee_ac_no    收费卡号, 为空则为所有符合条件的卡
        fee_hst_typ    '1'为只收欠费，只查Com_chrg_hst中不成功记录
                 '2' 为不检查Com_chrg_hst记录
                '0' 先处理欠费在收费
        succmsg        处理结果，错误则返回错误原因
        fee_fact    如果大于0则扣费金额，否则取参数表
*     输出: 
*     返回: <>0失败
=0成功
*
* 修改历史：
*
********************************************************************/
int pub_eod_crd_fee(char *feetyp, char *fee_br_no, char *fee_ac_no , char fee_hst_typ, char *succmsg, double *fee_fact)
{
    
    struct mdm_attr_c sMdm_attr;
    struct com_chrg_rate_c sCom_chrg_rate;
    struct com_parm_c sCom_parm;
    struct dc_acc_rel_c sDc_acc_rel;
    struct com_chrg_hst_c com_chrg_hst;
    struct mdm_ac_rel_c   mdm_ac_rel;
    struct dd_mst_c       dd_mst;
    struct dd_mst_hst_c   dd_mst_hst;
    struct com_item_c      com_item;
    struct card_reg_c      card_reg;
    struct com_branch_c    tmp_com_branch_c;
    char        fld_br_no[10];
    char        fld_ac_no[20];
    char        fld[20];
    char        fld_item[20];
    char        cMsg[51];
    char        fld_brf[40];
    char        kou_typ[2]; /*扣划标志，'0' 足额扣 '1' 有多少扣多少*/
    double      kou_amt=0.00; /*成功扣划金额*/
    int         times;
    char        chrg_rate_code[4];
    char        tmp_char[2];
    char        opn_br_no[6];
    double      AMT=0.00;
    double      dFee_amt=0.00;
    char        cAcc_hrt[6];
    long        kk_date=0;
    char        tx_date[9];
    long        dq_date=0;
    long        Ldate=0;
    long        sf_date = 0, tmp_date = 0;
    int         ret = 0;
    int         i = 0;
    int         ALL_FLAG = 0;
    int         VAL = 0;
    int         cnt1 = 0;
    int         cnt2 = 0;
    int         a=0;
    int         iCount=0;
    int     iJzcnt=0;
    memset(&g_mdm_ac_rel, 0x00, sizeof(struct mdm_ac_rel_c));
    memset(&com_chrg_hst, 0x00, sizeof(struct com_chrg_hst_c));
    memset(&sCom_chrg_rate, 0x00, sizeof(struct com_chrg_rate_c));
    memset(&dd_mst, 0x00, sizeof(struct dd_mst_c));
    memset(&dd_mst_hst, 0x00, sizeof(struct dd_mst_hst_c));
    memset(&com_item, 0x00, sizeof(struct com_item_c));
    memset(&card_reg,0x00,sizeof(struct card_reg_c));    
    memset(&tmp_com_branch_c,0x00,sizeof(struct com_branch_c));    
    memset(cAcc_hrt,0x00,sizeof(cAcc_hrt));
    memset(tx_date,0x00,sizeof(tx_date));
    
    memset(cMsg    , 0 , sizeof(cMsg));
    memset(kou_typ , 0 , sizeof(kou_typ));
    memset(opn_br_no,0x00,sizeof(opn_br_no));
    memset(chrg_rate_code,0x00,sizeof(chrg_rate_code));
    memset(&sDc_acc_rel,0x00,sizeof(sDc_acc_rel));
    memset(&com_chrg_hst,0x00,sizeof(com_chrg_hst));
    memset(&sMdm_attr, 0x00, sizeof(struct mdm_attr_c));
    memset(&sCom_parm, 0x00, sizeof(struct com_parm_c));
    /*** 初始化chrg_dc结构 ***/
    
    /**------- 初始化全局变量 --------**/
    strcpy(fld_br_no, "%%%%");
    strcpy(fld_ac_no, "%%%%");
    if(strlen(fee_br_no)>0)
        strcpy(fld_br_no, fee_br_no);
    if(strlen(fee_ac_no)>0)
        strcpy(fld_ac_no, fee_ac_no);
    vtcp_log("[%s][%d] tx_date [%d] /10 [%d]", __FILE__, __LINE__, g_pub_tx.tx_date, g_pub_tx.tx_date%100);
    ret = pub_base_GetParm_Str("K00TY",1,kou_typ);/**收年费标志**/
    if (ret){
        sprintf(acErrMsg,"公共参数无对应信息[%d]",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"EO004");
        goto ErrExit;
    }
    for(times=0; times<1; times++){ /*times=0 处理收年费 times=1 处理小额管理费 暂时没有小额管理费 times<2 修改为 times<1*/
        vtcp_log("[%s][%d] feetyp[%s] times=[%d] [%f] ",__FILE__,__LINE__,feetyp,times,*fee_fact);
        if(0 == times)
        {
            if('1' != feetyp[0])continue;
            vtcp_log("[%s][%d] 开始收卡年费feetyp[%s][%f]",__FILE__,__LINE__, feetyp,*fee_fact);
            
            /****宁夏没有免费次数，没有年费科目，没有年费金额，用的就是神码发过来的金额******
            ret = pub_base_GetParm_int("POSCS",1,&VAL);/ **pos免费次数** /
            if (ret){
                sprintf(acErrMsg,"公共参数无对应信息[%d]",ret);
                WRITEMSG
                strcpy(g_pub_tx.reply,"EO004");
                goto ErrExit;
            }
            
            ret = pub_base_GetParm_Str("K00",1,cAcc_hrt);/ **年费科目** /
            if (ret){
                sprintf(acErrMsg,"公共参数无对应信息[%d]",ret);
                WRITEMSG
                strcpy(g_pub_tx.reply,"EO004");
                goto ErrExit;
            }
            
            ret = pub_base_GetParm_double("SNFJE",1,&dFee_amt);/ **年费金额** /
            if (ret){
                sprintf(acErrMsg,"公共参数无对应信息[%d]",ret);
                WRITEMSG
                strcpy(g_pub_tx.reply,"EO004");
                goto ErrExit;
            }
            ***********************************/
            /* 晋中卡年费记到科目下 ChengGX 2010-3-25 11:19 */
            ret = pub_base_GetParm_Str("JZCAR",3,cAcc_hrt);/**卡年费科目**/
            if (ret){
                sprintf(acErrMsg,"公共参数无对应信息[%d]",ret);
                WRITEMSG
                strcpy(g_pub_tx.reply,"EO004");
                goto ErrExit;
            }
            strcpy(g_pub_tx.ac_no,cAcc_hrt);
            /* 晋中卡年费记到科目下 ChengGX 2010-3-25 11:19 */
            if(*fee_fact > 0.0001) dFee_amt=*fee_fact;
            vtcp_log("[%s][%d] dFee_amt =[%f]",__FILE__,__LINE__,dFee_amt);
            strcpy(fld_item, "K00");
            strcpy(fld_brf, "卡收年费");
        }
        else
            break;
/*********** ChengGX 暂时没有小额管理费2010-3-26 9:54
        {
            if('1' != feetyp[1]&&'A' != feetyp[1])
                continue;
            vtcp_log("[%s][%d] 开始收卡小额管理费feetyp[%s]",__FILE__,__LINE__,feetyp);
            ret = pub_base_GetParm_Str("K01",1,cAcc_hrt);**管理费目**
            if (ret){
                sprintf(acErrMsg,"公共参数无对应信息[%d]",ret);
                WRITEMSG
                strcpy(g_pub_tx.reply,"EO004");
                goto ErrExit;
            }
            ret = pub_base_GetParm_double("JEK01",1,&dFee_amt);**管理费金额**
            if (ret){
                sprintf(acErrMsg,"公共参数无对应信息[%d]",ret);
                WRITEMSG
                strcpy(g_pub_tx.reply,"EO004");
                goto ErrExit;
            }
            if(*fee_fact > 0.0001) 
                dFee_amt=*fee_fact;
            strcpy(fld_item, "K01");
            strcpy(fld_brf, "收小额管理费");
        }
***********/
        if(((g_pub_tx.tx_date/10000)%4==1) && ((g_pub_tx.tx_date%10000)==228) )
        {
            Ldate=(g_pub_tx.tx_date/10000 - 1)*10000+g_pub_tx.tx_date%10000+1;   /** 得到一年前是闰年的2月29日 **/
        }
        else
        {
            Ldate=(g_pub_tx.tx_date/10000 - 1)*10000+g_pub_tx.tx_date%10000;/***得到一年前日期**/
        }
        sprintf(tx_date,"%ld",g_pub_tx.tx_date);
        ret=Com_branch_Dec_Sel1(g_pub_tx.reply,"br_no like '%s' and  br_type ='4' and wrk_sts!='*' order by br_no", fld_br_no);
        if(ret)
        {
            sprintf(acErrMsg,"查询公共机构表异常[%d']",ret);
            WRITEMSG
            strcpy(g_pub_tx.reply,"EO092");
            goto ErrExit;
        }
        while(1)
        {
            memset(&tmp_com_branch_c,0x00,sizeof(struct com_branch_c));
            AMT=0.00;/**每个机构初始一下**/
            ret=Com_branch_Fet_Sel1(&tmp_com_branch_c,g_pub_tx.reply);
            if(ret==100)
            {
                break;
            }
            else if(ret)
            {
                sprintf(acErrMsg,"查询公共机构表异常[%d']",ret);
                WRITEMSG
                strcpy(g_pub_tx.reply,"EO092");
                goto ErrExit;
            }
            char filler2[9];
            memset(filler2,0x00,sizeof(filler2));
            sprintf(filler2,"%ld",Ldate);
            vtcp_log("[%s][%d]  filler2=[%s]",__FILE__,__LINE__,filler2);
            /**0表示新开卡一次费都没扣过，和取上次统计pos次数的截止日期 跨度为一年**/
            
            /**modify by xyy 增加对账户的判断，只查账户是621223&&621780开头的和介质状态为正常和挂失的**/
            if(0 == strcmp("K00", fld_item)){
                if(0 != memcmp("%%%%", fld_ac_no, 4)){
                    ret = Card_reg_Dec_Upd1(g_pub_tx.reply," ac_no = '%s'", fld_ac_no);
                }
                else
                {
                    ret = Card_reg_Dec_Upd1(g_pub_tx.reply," ac_no in (select ac_no from mdm_ac_rel where opn_br_no = '%s') and ( filler2= %s or (filler2 is null and ac_no in(select ac_no from mdm_ac_rel where opn_br_no='%s' and beg_date=%ld and substr(ac_no,0,6) in ('%s','%s') and  note_sts in ('0','1')) ))",tmp_com_branch_c.br_no, filler2,tmp_com_branch_c.br_no,Ldate,CARDHEADCODE,ICCARDHEADCODE);
                }
            }
            else
            {
                if('A'==feetyp[1])
                { /*从文件中取*/
                    ret = Card_reg_Dec_Upd1(g_pub_tx.reply," ac_no = '%s'", fld_ac_no);
                }
                else
                {
                    ret = Card_reg_Dec_Upd1(g_pub_tx.reply," ac_no in (select a.ac_no from dynamic_acm a, mdm_ac_rel b   where a.ac_no=b.ac_no and opn_br_no='%s' and y_acm/(to_date('%ld', 'yyyymmdd')-to_date('%ld0101', 'yyyymmdd')) < 200) ", tmp_com_branch_c.br_no, g_pub_tx.tx_date, g_pub_tx.tx_date/10000);
                }
            }
            if(ret){
                 sprintf(acErrMsg,"查询卡登记簿错误[%d]",ret);
                 WRITEMSG
                 memcpy(cMsg,"无此卡号",8);
                 *fee_fact=0.00;
                 goto OkExit;
            }
            iCount=0;
            iJzcnt=0;
            while(1) 
            {
                 memset(&card_reg,0x00,sizeof(card_reg));
                 ret = Card_reg_Fet_Upd1(&card_reg,g_pub_tx.reply);
                 if(ret)
                 {
                     vtcp_log("[%s][%d] ret=[%d] iCount=[%d]",__FILE__,__LINE__,ret,iCount);
                     if(ret == 100 && iCount >0)
                     {
                         if(iJzcnt >0)
                         {
                             sprintf(acErrMsg, "cardreg查询完毕,再记科目!![%d]", ret);
                             WRITEMSG
                             break;
                         }
                         else
                         {
                             sprintf(acErrMsg, "cardreg查询完毕,没有记账,跳出!![%d]", ret);
                             WRITEMSG
                             memcpy(cMsg,"扣款失败",8);/** 不知道为什么会出现这种情况 ChengGX 2010-3-26 17:13 **/
                             *fee_fact=0.00;
                             goto OkExit;
                         }
                     }
                     else
                     {
                         sprintf(acErrMsg, "cardreg无此卡号!![%d]", ret);
                         WRITEMSG
                         memcpy(cMsg,"无此卡号",8);
                         *fee_fact=0.00;
                         goto OkExit;
                     }
                 }
                 vtcp_log("[%s][%d]  收年费=[%s]",__FILE__,__LINE__,card_reg.ac_no);
                 memset(&g_mdm_ac_rel,0x00,sizeof(g_mdm_ac_rel));
                 ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",card_reg.ac_no);
                 if (ret)
                 {
                      sprintf(acErrMsg, "取介质帐号对照表错误!![%d]", ret);
                      WRITEMSG
                      memcpy(cMsg,"介质错误",8);
                      *fee_fact=0.00;
                      goto OkExit;
                 }
                 memcpy(g_pub_tx.ac_no,g_mdm_ac_rel.ac_no,strlen(card_reg.ac_no));
                 vtcp_log("[%s][%d],收年费,%lf",__FILE__,__LINE__,dFee_amt);
                 memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
                 ret=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,"ac_id=%ld and ac_seqn=1 ",g_mdm_ac_rel.ac_id);
                 if ( ret )
                 {
                     sprintf(acErrMsg, "取活期主文件错误!![%d][%ld]",ret,g_mdm_ac_rel.ac_id);
                     WRITEMSG
                     memcpy(cMsg,"无此账户",8);
                     *fee_fact=0.00;
                     goto OkExit;
                 }
                 if(g_dd_mst.ac_sts[0]=='*')
                 {
                     vtcp_log("[%s][%d] 该账户已经销户!!!",__FILE__,__LINE__);
                     memcpy(cMsg,"已销户",8);
                     *fee_fact=0.00;
                     goto OkExit;
                 }
                 kou_amt=dFee_amt;
                 vtcp_log("[%s][%d] dd_mst.bal[%lf] name[%s]%ld",__FILE__,__LINE__, g_dd_mst.bal, g_dd_mst.name, g_dd_mst.ac_id);
                 vtcp_log("[%s][%d]  WYW--ret[%d] kou_amt[%lf] dFee_amt[%lf],kou_typ[%s]",__FILE__,__LINE__,ret, kou_amt, dFee_amt,kou_typ);
                 ret = pub_eod_crd_chrg_chk(&kou_amt, kou_typ[0], fld_brf, cMsg);
                 vtcp_log("[%s][%d]  ret[%d] kou_amt[%lf]",__FILE__,__LINE__, ret, kou_amt);
            
                 if (ret) 
                 {    /*** 扣款不成功 ***/
                     vtcp_log("[%s][%d] 扣款不成功!!!!!",__FILE__,__LINE__);
                     *fee_fact=0;
                     strcpy(g_pub_tx.brf, fld_brf);
                     ret = pub_base_ins_chrg_mst(fld_item, "N", dFee_amt);
                     if (ret)
                     {
                           sprintf(acErrMsg, "登记收费明细帐错误!!");
                           WRITEMSG
                           goto ErrExit;
                     }
                 }
                 else 
                 {   /*** 扣款成功 ***/
                     vtcp_log("[%s][%d] 扣款成功!!!!!",__FILE__,__LINE__);
                     strcpy(g_pub_tx.brf, fld_brf);
                     AMT= AMT + kou_amt;/***加上上面欠费表的累加值**/
                     vtcp_log("[%s][%d] AMT=[%lf]",__FILE__,__LINE__,AMT);
                     *fee_fact=kou_amt;
                     if(1 == pub_base_CompDblVal( dFee_amt , kou_amt))
                     {
                         ret = pub_base_ins_chrg_mst(fld_item, "N", dFee_amt-kou_amt);
                         vtcp_log( "[%s][%d]pub_base_ins_chrg_mst(%s, 'N', %lf)",__FILE__,__LINE__,fld_item, kou_amt);
                     }
                     else
                     {
                         vtcp_log( "[%s][%d]pub_base_ins_chrg_mst(%s, 'Y', %lf)",__FILE__,__LINE__,fld_item, dFee_amt);
                         ret = pub_base_ins_chrg_mst(fld_item, "Y", dFee_amt);
                     }
                     if (ret)
                     {
                          sprintf(acErrMsg, "已经记账了,但是到这里却错了!!ret=[%d]", ret);
                          WRITEMSG
                          strcpy(g_pub_tx.reply, "O169");
                          goto ErrExit;
                     }
                     memcpy(cMsg,"扣费成功",8);
                     iJzcnt++;
                 }
                 vtcp_log("tx_date %ld trace_no %ld , ac_no %s, tx_code %s, rate_code %s brf %s", \
                 g_pub_tx.tx_date,g_pub_tx.trace_no,g_pub_tx.ac_no,g_pub_tx.tx_code,fld_item,g_pub_tx.brf);
                 strcpy(card_reg.filler1,"0"); /***将POS次数置零***/
                 strcpy(card_reg.filler2,tx_date);/****保存当前日期***/
                 /*Card_reg_Debug(&card_reg);*/
                 ret=Card_reg_Upd_Upd1(card_reg,g_pub_tx.reply);
                 if(ret)
                 {
                      sprintf(acErrMsg,"更新卡登记簿错误[%d] ",ret);
                      WRITEMSG
                      strcpy(g_pub_tx.reply, "O169");
                      Card_reg_Clo_Upd();
                      goto ErrExit;
                 }
                 iCount++;
                 vtcp_log("[%s][%d] iCount=[%d]记账=[%d]",__FILE__,__LINE__,iCount,iJzcnt);
            }
            Card_reg_Clo_Upd1();
KMJZ:
            vtcp_log("[%s][%d] 科目记账!!----%lf------",__FILE__,__LINE__,AMT);
            /***开始记科目账**/
            if(AMT>0.0001) /*金额为0不记账*/
            {
                 g_pub_tx.tx_amt1 = AMT;
                 /** 晋中直接使用科目 不需要在查询了。 ChengGX 2010-3-25 11:33
                 if(strlen(cAcc_hrt)>0)如果科目控制字不为空
                 {
                     ret = Com_item_Sel(g_pub_tx.reply, &com_item, " acc_hrt='%s' ",cAcc_hrt);
                     if (ret){
                         sprintf(acErrMsg, "取会计科目错误![%d][%s]", ret,sDc_acc_rel.acc_hrt );
                         WRITEMSG
                         strcpy(g_pub_tx.reply, "O169");
                         goto ErrExit;
                     }
                     strcpy(g_pub_tx.ac_no, com_item.acc_no);
                 }
                 else
                 {
                     char cAc_no[25];
                     memset(cAc_no , 0 , sizeof(cAc_no));
                     
                     ret=pub_base_GetParm_Str(tmp_com_branch_c.br_no,PARM_CARD_AC,cAc_no);
                     if (ret){
                         sprintf(acErrMsg,"公共参数无对应信息[%d]",ret);
                         WRITEMSG
                         strcpy(g_pub_tx.reply,"EO004");
                         goto ErrExit;
                     }
                     vtcp_log("[%s][%d] cAc_no=[%s]",__FILE__,__LINE__,cAc_no);
                     strcpy(g_pub_tx.ac_no, cAc_no);
                 }
                 晋中直接使用科目 不需要在查询了。 ChengGX 2010-3-25 11:33 **/
                 strcpy(g_pub_tx.ac_no, cAcc_hrt);
                 strcpy(g_pub_tx.tx_br_no,tmp_com_branch_c.br_no);
                 strcpy(g_pub_tx.opn_br_no,tmp_com_branch_c.br_no);
                 vtcp_log("%s,%d,tmp_com_branch_c.br_no='%s'",__FILE__,__LINE__,tmp_com_branch_c.br_no);
                 vtcp_log("%s,%d,tmp_com_branch_c.br_no='%s'",__FILE__,__LINE__,tmp_com_branch_c.br_no);
                 
                 g_pub_tx.ac_id = 0;
                 g_pub_tx.ac_seqn = 0;
                 strcpy(g_pub_tx.ac_id_type, "9");    /* 账户类型为内部 */
                 strcpy(g_pub_tx.ac_get_ind, "00");    /* 本程序未读取分户 */
                 strcpy(g_pub_tx.ac_wrk_ind, "000");    /* 不核对凭证号,零金额不?                                  * 屏魉?明细 */
                 strcpy(g_pub_tx.add_ind, "1");    /* 贷方标志 */
                 strcpy(g_pub_tx.prt_ind, "0");    /* 不登折 */
                 g_pub_tx.svc_ind = 9001;    /* 内部帐存取 */
                 strcpy(g_pub_tx.hst_ind, "1");    /* 日间入明细 */
                 ret = pub_acct_trance();
                 if (ret)
                 {
                     sprintf(acErrMsg, "调用存取款主控失败!");
                     WRITEMSG
                     goto ErrExit;
                 }
                 set_zd_data("1214", g_pub_tx.cur_no);
                 set_zd_data("1215", g_pub_tx.ct_ind);
                 set_zd_double("1218", g_pub_tx.tx_amt1);
                 ret = pubf_acct_proc("A017");
                 if (ret)
                 {
                     sprintf(acErrMsg, "贷方会计记帐不成功!!");
                     WRITEMSG
                     goto ErrExit;
                 }
                 set_zd_data("1214", "");
                 set_zd_data("1215", "");
                 set_zd_double("1218",0.0);
            }
        }
        Com_branch_Clo_Sel1();
    }
OkExit:
    strcpy(g_pub_tx.reply, "0000");
    memcpy(succmsg,cMsg,strlen(cMsg));
    sprintf(acErrMsg, "[%s]交易处理成功!!!!![%s]",__FILE__, g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120", g_pub_tx.reply);
    return 0;
ErrExit:
    if(0 == strcmp("0000", g_pub_tx.reply)) 
        strcpy(g_pub_tx.reply, "E000");
    sprintf(acErrMsg, "!!!!![%s]", g_pub_tx.reply);
    sprintf(succmsg, "处理失败,错误码%s", g_pub_tx.reply);
    *fee_fact=AMT;
    WRITEMSG
    set_zd_data("0120", g_pub_tx.reply);
    return 1;
}
