/****************************************************
*程序名称:gD322.c
*功能描述:实现七天通知存款产品中当期临时解约和当期正常的周末结息和划转，具体如下：
*         (1)当期状态为临时解约的：
*         a、如果为本周解约的，根据其积数自动算出利息，利息按活期利率计算，并将
*            利息划转到备用金帐户；
*         b、如果为以前临时解约的，理论上虚拟子帐户上没有积数，不处理；
*         (2)当期状态为正常的：
*         a、本周签约的，虚拟帐户上没有余额，不处理；
*         b、以前签约的，计算利息积数，按七天通知存款的利率计算利息，并将利息划转到备用金帐户上； 
*         c. 上周签约..签约后,钱数少于可划转金额..未划转..本周结息时,计算天数不正确啊.
*作    者:
*完成时间:
*修改记录:
*    1、修改时间
*    2、修改人
*    3、修改内容
*<EG321  DESC="当日不为周日!"/>
*<EG322  DESC="七天通知存款周末批量结息失败!" />
*
insert into com_eod_exec values( '846','D322','gD322', '七天通知存款结息划款','1111111111','0','0','00','00','0','Y','Y','N','2','Y');
******************************************************/
#define MYSQLERR if( ret ) {\
    strcpy( g_pub_tx.reply,"G322" );\
    sprintf(acErrMsg,"SQLERR [%d]",ret);\
    WRITEMSG\
    goto ErrExit;\
}
#define MYERR if( ret ) {\
    sprintf(acErrMsg,"SQLERR [%d]",ret);\
    WRITEMSG\
    goto ErrExit;\
}
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <math.h>
#define  EXTERN
#include "public.h"
#include "intst_hst_c.h"
#include "mo_prdt_prot_c.h"
#include "mo_prot_ac_rel_c.h"
#include "com_sys_parm_c.h"
#include "mdm_ac_rel_c.h"
#include "dd_parm_c.h"
#include "dd_mst_c.h"
#include "prdt_ac_id_c.h"

static  double  dQt_rate=0.00;
static  double  dHq_rate=0.00;

gD322()
{
	  int  ret = 0;
    int  iRet = 0;
    int  iSunday = 0;
    
    struct mo_prot_ac_rel_c sMo_prot_ac_rel;
    struct mo_prdt_prot_c sMo_prdt_prot;
    struct com_sys_parm_c sCom_sys_parm;
    struct mdm_ac_rel_c sMdm_ac_rel;
    struct S_pub_intst sum_intst;
    struct dd_parm_c sApp_dd_parm,sMain_dd_parm;
    struct dd_mst_c sApp_dd_mst,sMain_dd_mst;
        
    memset(&sMo_prot_ac_rel,0x00,sizeof(struct mo_prot_ac_rel_c));
    memset(&sMo_prdt_prot,0x00,sizeof(struct mo_prdt_prot_c));
    memset(&sCom_sys_parm,0x00,sizeof(struct com_sys_parm_c));
    memset(&sMdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
    memset(&sum_intst, 0x00, sizeof(sum_intst));
    memset(&sApp_dd_parm,0x00,sizeof(struct dd_parm_c));
    memset(&sMain_dd_parm,0x00,sizeof(struct dd_parm_c));
    memset(&sApp_dd_mst,0x00,sizeof(struct dd_mst_c));
    memset(&sMain_dd_mst,0x00,sizeof(struct dd_mst_c));
    
    /* 打开事务 */
    iRet = sql_begin();  
    if (iRet != 0)
    {
        sprintf(acErrMsg, "打开事务失败!!!");
        WRITEMSG
        goto ErrExit;
    }
    
    /* 初始化全局变量 */
    pub_base_sysinit();
    
    iRet = Com_sys_parm_Sel(g_pub_tx.reply, &sCom_sys_parm, "1=1");
    if (iRet) 
    {
        sprintf(acErrMsg, "取系统日期错误![%d]", iRet);
        WRITEMSG
        strcpy(g_pub_tx.reply,"CUD2");
        goto ErrExit;
    }
    g_pub_tx.tx_date = sCom_sys_parm.sys_date;
    
    /*检查日期的合法性*/
    iRet=pub_base_dayvalicheck(g_pub_tx.tx_date);
    if(iRet)
    {
        sprintf(acErrMsg, "系统日期不合法![%d]", iRet);
        WRITEMSG
        strcpy(g_pub_tx.reply,"CUD2");
        goto ErrExit;
    }
    
    /*检查当天是否是周日*/
    iSunday=pub_base_day_week(g_pub_tx.tx_date);
    vtcp_log("zylsee11122 date=[%ld]",g_pub_tx.tx_date);
    vtcp_log("zylsee11122 dat=[%d]",iSunday);
    /**周日时 pub_base_day_week返回99 by zyl 20110511**/
    if(iSunday != 99)
    {
        sprintf(acErrMsg, "当天不为星期日,不做批量结息![%d] [%d]",__LINE__, iSunday);
        WRITEMSG
        goto OkExit;
    }
    /*临时解约的要取当前的活期利率*/
    iRet = pub_base_getllz("100","01",g_pub_tx.tx_date,&dHq_rate);
    if (iRet != 0)
    {
        sprintf(acErrMsg,"从公共利率表中取利率值错误 [%d]",iRet);
        WRITEMSG
        strcpy(g_pub_tx.reply,"W110");
        goto ErrExit;
    }
    vtcp_log("[%s][%d],dHq_rate=[%.2lf]\n",__FILE__,__LINE__,dHq_rate);
    /*正式签约的取七天通知存款利率 七天通知存款利率 B00*/
    iRet = pub_base_getllz("B00","01",g_pub_tx.tx_date,&dQt_rate);
    if (iRet != 0)
    {
        sprintf(acErrMsg,"从公共利率表中取利率值错误 [%d]",iRet);
        WRITEMSG
        strcpy(g_pub_tx.reply,"W110");
        goto ErrExit;
    }
    vtcp_log("[%s][%d],dQt_rate=[%.2lf]\n",__FILE__,__LINE__,dQt_rate);
    /*取主机流水号*/
    if (pub_base_PubQlsh(&g_pub_tx.trace_no, g_pub_tx.reply))
    {
        sprintf(acErrMsg, "取主机流水号出错! trace_no=[%ld]",g_pub_tx.trace_no);
        WRITEMSG
        goto ErrExit;
    }
    iRet = Mo_prdt_prot_Dec_Sel1( g_pub_tx.reply , "  prot_type='TK7' and sts in('1','2') order by sts,main_ac_id" );
    if (iRet)
    {
        sprintf(acErrMsg, "设定mo_prdt_prot游标错误![%d] [%d]",__LINE__,iRet);
        WRITEMSG
        goto ErrExit;
    }
    
    while(1)
    {
    	  strcpy(g_pub_tx.ac_no,"");
        memset(&sMo_prdt_prot,0x00,sizeof(struct mo_prdt_prot_c));
        iRet=Mo_prdt_prot_Fet_Sel1( &sMo_prdt_prot , g_pub_tx.reply );
        if(iRet==100)
        {
            break;
        }
        else if (iRet)
        {
            sprintf(acErrMsg, "读取mo_prdt_prot游标错误!");
            WRITEMSG
            goto ErrExit;
        }
        /*查询划款协议账户对照表*/
        memset(&sMo_prot_ac_rel,0x00,sizeof(struct mo_prot_ac_rel_c));
        iRet=Mo_prot_ac_rel_Sel(g_pub_tx.reply,&sMo_prot_ac_rel," prot_type='TK7' and main_ac_id=%ld and main_ac_seqn=%d and sts in('1','2')",sMo_prdt_prot.main_ac_id,sMo_prdt_prot.main_ac_seqn);
        if(iRet)
        {
            sprintf(acErrMsg, "读取划款协议帐户对照表错误!iRet=[%d]",iRet);
            WRITEMSG
            goto ErrExit;
        }
        /*查询活期主表*/
        memset(&sApp_dd_mst,0x00,sizeof(struct dd_mst_c));
        iRet=Dd_mst_Sel(g_pub_tx.reply,&sApp_dd_mst," ac_id=%ld ",sMo_prot_ac_rel.app_ac_id);
        if(iRet)
        {
            sprintf(acErrMsg, "读取活期主文件错误!iRet=[%d]",iRet);
            WRITEMSG
            goto ErrExit;
        }
        /*查询活期产品表*/
        memset(&sApp_dd_parm,0x00,sizeof(struct dd_parm_c));
        iRet = Dd_parm_Sel(g_pub_tx.reply, &sApp_dd_parm, "prdt_no='%s'",sApp_dd_mst.prdt_no);
        if (iRet)
        {
            sprintf(acErrMsg, "查询活期产品表错误iRet=[%d]prdt_no=[%s]!",iRet,sApp_dd_mst.prdt_no);
            WRITEMSG
            goto ErrExit;
        }
        /*计算虚拟帐户的利息*/
        memset(&sum_intst, 0x00, sizeof(sum_intst));
        iRet=app_cal_intst(sMo_prdt_prot.sts,sApp_dd_mst,sApp_dd_parm);
        if (iRet)
        {
            sprintf(acErrMsg, "计算虚拟帐户利息错误ret=[%d]!!!", iRet);
            WRITEMSG
            goto ErrExit;
        }
        sum_intst.dealval=g_pub_intst.dealval;
        sum_intst.val=g_pub_intst.val;
        
        vtcp_log("[%s][%d]g_pub_intst.dealval=[%.2lf],g_pub_intst.val=[%.2lf]",__FILE__,__LINE__,g_pub_intst.dealval,g_pub_intst.val);
        vtcp_log("[%s][%d]  sum_intst.dealval=[%.2lf],  sum_intst.val=[%.2lf]",__FILE__,__LINE__,sum_intst.dealval,sum_intst.val);
        
        memset(&sMain_dd_mst,0x00,sizeof(struct dd_mst_c));
        iRet=Dd_mst_Sel(g_pub_tx.reply,&sMain_dd_mst," ac_id=%ld ",sMo_prot_ac_rel.main_ac_id);
        if(iRet)
        {
            sprintf(acErrMsg, "读取活期主文件错误!iRet=[%d]",iRet);
            WRITEMSG
            goto ErrExit;
        }
        /*查询活期产品表*/
        memset(&sMain_dd_parm ,0x00,sizeof(struct dd_parm_c));
        iRet = Dd_parm_Sel(g_pub_tx.reply, &sMain_dd_parm, "prdt_no='%s'",sMain_dd_mst.prdt_no);
        if (iRet)
        {
            sprintf(acErrMsg, "查询活期产品表错误iRet=[%d]prdt_no=[%s]!!!",iRet,sApp_dd_mst.prdt_no);
            WRITEMSG
            goto ErrExit;
        }
        /****/
        vtcp_log("[%s][%d] see143 ac_no=[%s]",__FILE__,__LINE__,g_pub_tx.ac_no);
        vtcp_log("[%s][%d] see143 ac_id=[%ld]",__FILE__,__LINE__,g_pub_tx.ac_id);
        vtcp_log("[%s][%d] see143 sMo_prdt_prot.sts=[%s]",__FILE__,__LINE__,sMo_prdt_prot.sts);
        iRet=main_intst_acct(sMain_dd_mst,sMain_dd_parm,sum_intst,sMo_prdt_prot.sts);
        if (iRet)
        {
            sprintf(acErrMsg, "主账号记账处理失败iRet=[%d]!",iRet);
            WRITEMSG
            goto ErrExit;
        }
        vtcp_log("One of the fet end.");
        
        /**add by zyl 20110516 结息后,将虚拟户的钱划回主账户..以备翻牌后,重新开始七天通知存款划转.**/
        /**1.七天通知存款虚拟账户取款.**/
        ret=acct_Z016(sApp_dd_mst.ac_id,1,1001,"0",sApp_dd_mst.bal,"LC03");
        if(ret)
        {
            sprintf(acErrMsg,"七天通知存款虚拟账户取款 出错. [%ld][%d]",
            sApp_dd_mst.ac_id,ret);
            WRITEMSG
            goto ErrExit;
        }
        /**2.七天通知存款主账户存款.**/
        ret=acct_Z016(sMain_dd_mst.ac_id,1,1001,"1",sApp_dd_mst.bal,"LC02");
        if(ret)
        {
            sprintf(acErrMsg,"七天通知存款主账户存款处理出错. [%ld][%d]",
            sMain_dd_mst.ac_id,ret);
            WRITEMSG
            goto ErrExit;
        }
        /**end by zyl 20110516**/

    }
    Mo_prdt_prot_Clo_Sel();
    vtcp_log("vvvvvvvvvvvvvvvvvvv[%d]",g_reply_int);
    /***zgf add 20130725**/  
    pub_mob_sendmail(  g_pub_tx.brf, g_pub_tx.tx_code,g_pub_tx.ac_no,g_pub_tx.add_ind,g_pub_tx.tx_amt1, sApp_dd_mst.bal );
    /***zgf add end**/   
        
OkExit:
    sql_commit();        /*--提交事务--*/
    strcpy(g_pub_tx.reply, "0000");
    sprintf(acErrMsg, "七天通知存款周日计息程序成功!!!!!![%s]", g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120", g_pub_tx.reply);
    return 0;
ErrExit:
    if (strcmp(g_pub_tx.reply, "0000") == 0 || g_pub_tx.reply[0] == '\0')
    {
        strcpy(g_pub_tx.reply, "G322");
    }
    sql_rollback();        /*--事务回滚--*/
    sprintf(acErrMsg, "七天通知存款周日计息程序失败!!!!!![%s]", g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120", g_pub_tx.reply);
    return 1;
}



int app_cal_intst(cSts, dd_mst_c, dd_parm_c)
    char   *cSts;
    struct dd_mst_c dd_mst_c;
    struct dd_parm_c dd_parm_c;
{
    vtcp_log("[%s][%d],签约状态cSts=[%s]\n",__FILE__,__LINE__,cSts);
    int    ret=0;
    long   lDays=0;
    double dLx=0.00;       /*应付利息*/
    double dLxacm=0.00;    /*利息积数*/
    double dLxs=0.00;      /*利息税*/
    double dLxsl=0.00;     /*利息税税率*/
    double dSflx=0.00;     /*实付利息*/
    
    struct intst_hst_c T_intst_hst;
    struct dd_mst_c tmp_dd_mst;
    
    memset(&T_intst_hst, 0x00, sizeof(struct intst_hst_c));
    memset(&g_pub_intst, 0x0, sizeof(g_pub_intst));
    
    memcpy(&g_dd_mst, &dd_mst_c, sizeof(g_dd_mst));
    memcpy(&g_dd_parm, &dd_parm_c, sizeof(g_dd_parm));
    
    /*更新活期主文件*/
    ret = Dd_mst_Dec_Upd(g_pub_tx.reply, "ac_id=%d and ac_seqn=%d",g_dd_mst.ac_id, g_dd_mst.ac_seqn);
    MYERR
    
    if(cSts[0]=='2')   /*临时解约*/
    {        
        vtcp_log("[%s][%d],cSts=[%s],dHq_rate=[%.2lf]\n",__FILE__,__LINE__,cSts,dHq_rate);
        dLx = (g_dd_mst.intst_acm * dHq_rate)/L360/L100;/**应付利息**/
        vtcp_log("[%s][%d],dLx=[%.2lf]\n",__FILE__,__LINE__,dLx);
        dLx = pub_base_PubDround_1(dLx);
        /*如果有利息税，是按活期利息税收还是按七天通知存款这种产品的利息税来收*/
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
        T_intst_hst.rate = dHq_rate;
        vtcp_log("[%s][%d] sts=[%s],dealval=[%.2f],val=[%.2lf]",__FILE__,__LINE__,cSts,g_pub_intst.dealval,g_pub_intst.val);
    }
    else if(cSts[0]=='1')  /*正式签约*/
    {
        vtcp_log("[%s][%d],dQt_rate=[%.2lf]\n",__FILE__,__LINE__,dQt_rate);        
        /**滚积数结息**/
        vtcp_log("zylsee145 g_dd_mst.lst_date=[%ld]",g_dd_mst.lst_date);
        vtcp_log("zylsee145 g_pub_tx.tx_date=[%ld]",g_pub_tx.tx_date);
        ret=pub_base_CalIntstDays(g_dd_mst.lst_date,g_pub_tx.tx_date,"0",&lDays);
        lDays++;
        lDays = 7;
        if (ret !=0)
        {
            sprintf(acErrMsg,"计算天数错误!!");
            WRITEMSG
            goto ErrExit;
        }
        if (lDays!=0 && lDays % 7 == 0) 
        {
            dLx = (lDays * dQt_rate * g_dd_mst.bal)/L360/L100;/**应付利息**/
            dLx = pub_base_PubDround_1(dLx);
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
                vtcp_log("[%s][%d],dLx=[%.2lf],dLxs=[%.2lf],dLxsl=[%.2lf]",__FILE__,__LINE__,dLx,dLxs,dLxsl);
                dSflx=dLx-dLxs;
            }
            g_pub_intst.dealval=dLx;
            g_pub_intst.val=dLxs;
            g_dd_mst.intst_acm=g_dd_mst.bal * lDays;  /*利息积数*/
            T_intst_hst.rate = dQt_rate;
            vtcp_log("[%s][%d] sts=[%s]汇总dealval=[%.2f],val=[%.2lf]",__FILE__,__LINE__,cSts,g_pub_intst.dealval,g_pub_intst.val);
        }
        else if( lDays < 7)
        {
            vtcp_log("[%s][%d] lDays=[%ld] 签约日期在本周且不足七天",__FILE__,__LINE__,lDays);
        }
        else
        {
        	  if(pub_base_CompDblVal(g_dd_mst.bal,0.00) == 0)
        	  {
        	      vtcp_log("[%s][%d] lDays=[%ld] 签约日期在上周,大于七天,未划款.",__FILE__,__LINE__,lDays);
        	  }
        	  else
        	  {
                sprintf(acErrMsg,"七天通知存款天数错误! lDays=[%ld]",lDays);
                WRITEMSG
                goto ErrExit;
            }
        }
    }
    vtcp_log("[%s][%d],准备登记结息登记簿\n",__FILE__,__LINE__);
    strcpy(T_intst_hst.br_no, g_dd_mst.opn_br_no);
    T_intst_hst.ac_id = g_dd_mst.ac_id;
    T_intst_hst.ac_seqn = g_dd_mst.ac_seqn;
    strcpy(T_intst_hst.prdt_no, g_dd_mst.prdt_no);
    strcpy(T_intst_hst.cur_no, g_dd_parm.cur_no);
    T_intst_hst.bic_bal = g_dd_mst.bal;
    T_intst_hst.bic_acm = g_dd_mst.intst_acm;
    T_intst_hst.fct_intst = g_pub_intst.dealval;
    T_intst_hst.intst_tax = g_pub_intst.val;
    T_intst_hst.aic_bal = g_dd_mst.bal;
    T_intst_hst.aic_acm = 0.00;
    T_intst_hst.ic_date = g_dd_mst.ic_date;    
    T_intst_hst.intst_date = g_pub_tx.tx_date;
    strcpy(T_intst_hst.ac_ind, "1");
    strcpy(T_intst_hst.pt_ind, "1");
    T_intst_hst.intst_date=g_pub_tx.tx_date;
    ret = Intst_hst_Ins(T_intst_hst, g_pub_tx.reply);
    MYERR
    
    ret = Dd_mst_Fet_Upd(&tmp_dd_mst, g_pub_tx.reply);
    MYERR
    g_dd_mst.intst_acm = 0.00;/**应付利息积数**/
    g_dd_mst.lst_date=g_pub_tx.tx_date;
    g_dd_mst.ic_date=g_pub_tx.tx_date;
    ret = Dd_mst_Upd_Upd(g_dd_mst, g_pub_tx.reply);
    MYERR
    vtcp_log("[%s][%d],更新活期主表结束\n",__FILE__,__LINE__);
    Dd_mst_Clo_Upd();
    
    

    return 0;
ErrExit:
    return 1;
}

int main_intst_acct(dd_mst_c, dd_parm_c, sum_intst,cSts)
    struct dd_mst_c dd_mst_c;
    struct dd_parm_c dd_parm_c;
    struct S_pub_intst sum_intst;
    char   *cSts;
{
    int  ret = 0;
    /**add by zyl 20110511**/
    struct mdm_ac_rel_c sMdm_ac_rel;
    struct prdt_ac_id_c sPrdt_ac_id;
    memset(&sPrdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));
    memset(&sMdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
    ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_id = %ld ",dd_mst_c.ac_id);
    if(ret)
    {
        sprintf(acErrMsg, "Mdm_ac_rel_Sel error! ret=[%ld]",ret);
        WRITEMSG
        goto ErrExit;
    }
		ret=Prdt_ac_id_Sel(g_pub_tx.reply,&sPrdt_ac_id," ac_id=%ld ",dd_mst_c.ac_id);
		if( ret )
		{
			sprintf(acErrMsg,"账号不存在![%ld][%d]",
				g_pub_tx.ac_id,g_pub_tx.ac_seqn);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P102");
			goto ErrExit;
		}
		strcpy(g_pub_tx.prdt_code,sPrdt_ac_id.prdt_no);
		strcpy(g_pub_tx.ac_id_type,sPrdt_ac_id.ac_id_type);
		vtcp_log("zylsee134  ac_id_type=[%s]",sPrdt_ac_id.ac_id_type);
    /**end by zyl 20110511**/    
    strcpy(g_pub_tx.tx_br_no, dd_mst_c.opn_br_no);
    strcpy(g_pub_tx.opn_br_no, dd_mst_c.opn_br_no);
    strcpy(g_pub_tx.cur_no, dd_parm_c.cur_no);
    strcpy(g_pub_tx.ac_wrk_ind, "0000000");
    g_pub_tx.svc_ind = 1005;
    strcpy(g_pub_tx.ct_ind, "2");
    strcpy(g_pub_tx.add_ind, "1");
    strcpy(g_pub_tx.prdt_code, dd_parm_c.prdt_no);
    g_pub_tx.ac_id = dd_mst_c.ac_id;
    vtcp_log("zylsee12331 g_pub_tx.ac_id=[%ld]",g_pub_tx.ac_id);    
    g_pub_tx.ac_seqn = sMdm_ac_rel.ac_seqn;/**mod by zyl 20110512**/
    strcpy(g_pub_tx.note_type, "");
    strcpy(g_pub_tx.beg_note_no, "");
    g_pub_tx.tx_amt1 = sum_intst.dealval;
    strcpy(g_pub_tx.brf, "七天通知存款结息");
    strcpy(g_pub_tx.hst_ind, "1"); /*少一条明细20121210 by jk*/
    /* 进行利息的交易记帐处理 */
    ret = pub_acct_trance();
    if (ret)
    {
        sprintf(acErrMsg, "记账错误!!");
        WRITEMSG
        goto ErrExit;
    }    
    /* 进行利息税的交易记帐处理 */
    strcpy(g_pub_tx.brf, "利息税");
    strcpy(g_pub_tx.no_show, "0");
    strcpy(g_pub_tx.add_ind, "0");
    strcpy(g_pub_tx.ac_wrk_ind, "0000000");
    strcpy(g_pub_tx.hst_ind, "1");
    g_pub_tx.tx_amt1 = sum_intst.val;
    ret = pub_acct_trance();
    if (ret)
    {
        sprintf(acErrMsg, "记账错误!!");
        WRITEMSG
        goto ErrExit;
    }   
    /* 进行会计记帐 */
    set_zd_data(DC_CODE, dd_parm_c.dc_code);
    set_zd_double("102P", sum_intst.dealval-sum_intst.val);
    set_zd_double("102Q", sum_intst.val);
    set_zd_data("102J", dd_parm_c.cur_no);
    strcpy(g_pub_tx.prdt_code, dd_mst_c.prdt_no);
    /**mod by zyl 20110522 todo test**/
    if(cSts[0]=='1')
    {
    	strcpy(g_pub_tx.sub_tx_code,"Z027");/**签约状态.**/
    }
    else if(cSts[0]=='2')
    {
    	strcpy(g_pub_tx.sub_tx_code,"Z032");/**临时解约状态.**/
    }
    else
    	goto ErrExit;
    /**mod end by zyl 20110522**/
    ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
    if (ret != 0)
    {
        sprintf(acErrMsg, "会计记帐不成功!!");
        WRITEMSG
        goto ErrExit;
    }
    /****/
    char cTmpbuf[256];
    memset(cTmpbuf,0x00,sizeof(cTmpbuf));
    get_zd_data("0152",cTmpbuf);
    vtcp_log("zylsee109 0152=[%s]",cTmpbuf);
    memset(cTmpbuf,0x00,sizeof(cTmpbuf));
    get_zd_data("0153",cTmpbuf);
    vtcp_log("zylsee109 0153=[%s]",cTmpbuf);
    /****/
    return 0;
ErrExit:
    return 1;
}


int acct_Z016(long ac_id,int ac_seqn,long deal_code,char *add_ind,double amt,char *sub_tx_code)
{
	strcpy(g_pub_tx.ac_no,"");
	g_pub_tx.ac_id=ac_id;
	g_pub_tx.ac_seqn=ac_seqn;
	g_pub_tx.tx_amt1=amt;
	strcpy(g_pub_tx.add_ind,add_ind); 
	strcpy(g_pub_tx.hst_ind,"1"); /**日间入明细**/
	strcpy(g_pub_tx.ac_wrk_ind,"0000010011");
	g_pub_tx.svc_ind=deal_code; /**处理代码***/
	strcpy(g_pub_tx.ct_ind,"2");	/*** 转账 ***/
	strcpy(g_pub_tx.ac_id_type,"");
	strcpy(g_pub_tx.note_type,"");
	strcpy(g_pub_tx.beg_note_no,"");
	strcpy(g_pub_tx.brf, "七天通知存款划款");
	if ( pub_acct_trance() )
	{
		sprintf(acErrMsg,"调用存取款主控失败!");
		WRITEMSG
		return 1;
	}
	set_zd_data("102K",g_pub_tx.ct_ind);
	/***修改为使用100域第2子域 modify at 20130517
	set_zd_double("102I",amt);
	****/
	set_zd_double("1002",amt);
	if ( pubf_acct_proc(sub_tx_code) )
	{
		sprintf(acErrMsg,"登记会计流水错误!");
		WRITEMSG
		return 1;
	}
	return 0;	
}
