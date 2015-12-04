/*********************************************************************
* 版 本 号 :  V1.0        
* 程序作者 :  gongliangliang    
* 日    期 :  2011-08-08   
* 所属模块 :  与平台的接口程序 (预授权完成撤销冲正)          
* 程序名称 :  spF246.c
* 程序作用 : 
* 函数列表 :  1 pub_base_CheckCrdNo()
*             2 pub_base_DesChk()
*             3 pubf_card_chk_msr()
*             4 pub_base_CompDblVal()     
*             5 pub_card_reg()
* 使用注意 :  添加预授权标志记录 5 预授权完成撤销冲正                                     
* 修改记录 :                                       
*     修改作者 :                           
*     修改时间 :                                    
*     修改内容 :
insert into tx_def values('6246','预授权完成撤销冲正','10000000000010000000000010000100000000000000000000000000000000000000000000000000011000000000000000000100000000000000000000000100','1','1','0','3',null);
insert into tx_sub_def values('6246','预授权完成撤销冲正','spF246','1','0');
insert into tx_flow_def values('6246','0','F246','#=0001@1@#=0002@2@#$');
insert into tx_flow_def values('6246','1','D003','#=0000@3@#$');
insert into tx_flow_def values('6246','2','A016','#=0000@3@#$');
insert into tx_flow_def values('6246','3','F000','#$');
insert into tx_dc_rel values('F246','0000','F04600');
下面这个有了就不用加了，F04600 用的原来的
insert into dc_entry values('F04600','1','0210','@0152','0930','1','1','2','0420','完成撤销冲正');
*************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "chnl_single_hold_c.h"
#include "card.h"
#include "card_reg_c.h"
#include "mdm_ac_rel_c.h"
#include "prdt_ac_id_c.h"
#include "dd_mst_c.h"
#include "in_mst_c.h" 
#include "com_item_c.h"

int spF246()
{
    int ret;
    long lOld_date;/*原平台日期*/
    char cOld_trace_no[13];/*原平台流水号*/
    char cAc_no[25];/*卡号*/
    char cCtrlcmd[11];/*控制标志本标志适用于新平台所有交易 73域接收 每个字节代表一种控制或检查
                      检查标志(0不检查1检查)：
                      [0]:密码检查标志0不查1查
                      [1]:磁道信息检查标志0不查1查
                      [2]:证件检查标志0不查1查
                      [3]:户名检查标志0不查不查
                      [4]:帐号检查标志0不查不查
                      [5]:收款人挂帐标志0帐号必须相符不查户名1帐号户名必须都相符2帐号必须相符，户名不符挂帐3帐号或户名有一不符挂帐,9强制挂帐
                      [6]:通存通兑标志0允许1不允许
                      [7]:凭证检查标志0不查1查柜员凭证2查客户凭证*/
    char cYsq_ind[2];/*预授权标志*/
    char cYsq_no[11];/*预授码*/
    char tmpstr[51],tmpbrf[21];
    char cFee_ind[2];/*POS标志，0为本行POS，1为他行POS*/
    char ct_ind[2];/*现转标识*/
    char cPt_trace_no[13];/*平台流水号*/
    char cMsr_str2[38];/*2磁道信息*/
    char cMsr_str3[142];/*3磁道信息*/
    char cGz_flag[2];/*挂账标志*/
    char cBbmk[5];/*系统类型*/
    struct chnl_single_hold_c sChnl_single_hold;
    struct mdm_ac_rel_c sMdm_ac_rel;
    struct prdt_ac_id_c sPrdt_ac_id; 
    struct dd_mst_c sDd_mst;
    struct in_mst_c sIn_mst;
    struct com_item_c sCom_item;
    
    memset(cFee_ind,0,sizeof(cFee_ind));
    memset(ct_ind,0,sizeof(ct_ind));
    memset(&sChnl_single_hold,0x0,sizeof(struct chnl_single_hold_c));
    memset(cAc_no,0,sizeof(cAc_no));
    memset(tmpstr,0,sizeof(tmpstr));
    memset(tmpbrf,0,sizeof(tmpbrf));
    memset(cCtrlcmd,0,sizeof(cCtrlcmd));
    memset(cMsr_str2,0,sizeof(cMsr_str2));
    memset(cMsr_str3,0,sizeof(cMsr_str3)); 
    memset(cYsq_no,0,sizeof(cYsq_no));
    memset(cYsq_ind,0,sizeof(cYsq_ind)); 
    memset(cPt_trace_no,0,sizeof(cPt_trace_no));
    memset(&sMdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
    memset(&sPrdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));
    memset(&sDd_mst,0x00,sizeof(struct dd_mst_c));
    memset(&sIn_mst,0x00,sizeof(struct in_mst_c));
    memset(&sCom_item,0x00,sizeof(struct com_item_c));
    memset(cGz_flag,0x00,sizeof(cGz_flag));/*调用pub_com_test时用到*/
    memset(cBbmk,0x00,sizeof(cBbmk));
    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"初始化公用结构错误!!");
        WRITEMSG
        goto ErrExit;
    }
    vtcp_log("[%s][%d],tx_date=[%d]",__FILE__,__LINE__,g_pub_tx.tx_date);
    set_zd_long("0460",g_pub_tx.tx_date);
    get_zd_data("0690",cFee_ind);/*本行他行POS标志，0为本行POS,1为他行POS*/
    get_zd_data("0300",g_pub_tx.crd_no); /* 卡号 */
    get_zd_data("0310",cAc_no);/*取得商户帐号*/
    vtcp_log("%s,%d,取得商户帐号为ac_no=[%s]",__FILE__,__LINE__,cAc_no);
    get_zd_double("0400",&g_pub_tx.tx_amt1);  /* 消费金额 */
    get_zd_data("0730",cCtrlcmd);/*控制标志*/
    get_zd_data("0810",tmpstr); /* 摘要*/
    memcpy(tmpbrf,tmpstr,20); 
    get_zd_data("0780",cOld_trace_no);  /*原终端流水号 */
    get_zd_long("0450",&lOld_date);  /*原平台日期*/
    get_zd_data("0520",cPt_trace_no);/*平台流水*/
	/*****
    get_zd_data("0790",g_pub_tx.draw_pwd);  支取密码 
	****/
    get_zd_data("0750",cMsr_str2);/*磁道2信息*/
    get_zd_data("0760",cMsr_str3);/*磁道3信息*/
    get_zd_data("0870",cYsq_no);/*预授码*/
    get_zd_data("0700",cYsq_ind);/*预授权标志 4位预授权完成撤销*/
    get_zd_data("0100",cBbmk);/*系统种类*/
    /*检查预授权标志是否为预授权完成撤销冲正*/
    vtcp_log("%s,%d,预授权标志【%c】",__FILE__,__LINE__,cYsq_ind[0]);
    if(cYsq_ind[0] != '5')/*此处要修改数据字典，添加预授权标志记录 5 预授权完成撤销冲正 add by gong 20110810*/
    {
        sprintf(acErrMsg,"预授权标志错误");
        WRITEMSG
        goto ErrExit;
    }
    
    /* 检查支取方式的合法性 */
    ret=pub_com_test(cCtrlcmd,"","",g_pub_tx.crd_no,"","","",cGz_flag);
    vtcp_log("%s,%d,cCtrlcmd=[%s],g_pub_tx.crd_no=[%s],cGz_flag=[%s]",__FILE__,__LINE__,cCtrlcmd,g_pub_tx.crd_no,cGz_flag);
    if (ret)
    {
        sprintf(acErrMsg,"%s,%d,公共函数pub_com_test()调用失败",__FILE__,__LINE__);
        WRITEMSG
        goto ErrExit;	
    }
    /* 检查卡的合法性 */
    if (pub_base_CheckCrdNo())
    {
        sprintf(acErrMsg,"检查卡的合法性错误!!");
        WRITEMSG
        goto ErrExit;
    }
    
    /* 查询预授权完成撤销的交易流水日记 */
    /**增加bbmk 字段 shangyongchao 2012/6/7 11:47:04
    ret = Chnl_single_hold_Dec_Upd(g_pub_tx.reply,"ac_no ='%s' and pt_date=%ld and pt_trace_no='%s' and premise_no='%s'",\
        g_pub_tx.crd_no,lOld_date,cOld_trace_no,cYsq_no);
    **/
    ret = Chnl_single_hold_Dec_Upd(g_pub_tx.reply,"ac_no ='%s' and pt_date=%ld and pt_trace_no='%s' and premise_no='%s' and bbmk='%s'",\
        g_pub_tx.crd_no,lOld_date,cOld_trace_no,cYsq_no,cBbmk);
    if (ret)
    {
        sprintf(acErrMsg,"要冲正的记录不存在!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"CU06");
        goto ErrExit;
    }
    ret = Chnl_single_hold_Fet_Upd(&sChnl_single_hold,g_pub_tx.reply);
    if(ret)
    {
        sprintf(acErrMsg,"要冲正的记录不存在!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"CU06");
        goto ErrExit;
    }
    if (sChnl_single_hold.sts[0] != '4')
    {
        sprintf(acErrMsg,"记录状态不正常!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"CU06");
        goto ErrExit;
    }
    if (memcmp(sChnl_single_hold.ac_no,g_pub_tx.crd_no,19)!=0)
    {
        vtcp_log("%s,%d,sChnl_single_hold.ac_no=[%s],g_pub_tx.crd_no=[%s]xk",__FILE__,__LINE__,sChnl_single_hold.ac_no,g_pub_tx.crd_no); 	
        sprintf(acErrMsg,"冲正卡号不一致!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"CU06");
        goto ErrExit;
    }
    if (pub_base_CompDblVal(sChnl_single_hold.tx_amt, g_pub_tx.tx_amt1)!=0)
    {
        sprintf(acErrMsg,"冲正金额于完成金额不一致!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"CU06");
        goto ErrExit;
    }
    /*增加bbmk 字段 shangyongchao 2012/6/7 11:48:04
    ret = sql_execute("update Chnl_single_hold set sts='3',tx_amt = %lf where ac_no ='%s' and  pt_date=%ld and pt_trace_no= '%s' and premise_no='%s' ",\
         sChnl_single_hold.tx_amt,sChnl_single_hold.ac_no,sChnl_single_hold.o_pt_date,sChnl_single_hold.o_pt_trace_no,cYsq_no);
    */
    ret = sql_execute("update Chnl_single_hold set sts='3',tx_amt = %lf where ac_no ='%s' and  pt_date=%ld and pt_trace_no= '%s' and premise_no='%s' and bbmk='%s'",\
         sChnl_single_hold.tx_amt,sChnl_single_hold.ac_no,sChnl_single_hold.o_pt_date,sChnl_single_hold.o_pt_trace_no,cYsq_no,sChnl_single_hold.bbmk);
    
    if (ret)
    {
        sprintf(acErrMsg,"更新原预授权记录失败!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"CU06");
        goto ErrExit;

    }
    strcpy(sChnl_single_hold.sts,"5");
    ret = Chnl_single_hold_Upd_Upd(sChnl_single_hold,g_pub_tx.reply);
    if(ret)
    {
        sprintf(acErrMsg,"更新预授权撤销记录失败!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"CU06");
        goto ErrExit;
    }
    Chnl_single_hold_Clo_Upd();
    memset(&sMdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
    ret= Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel," ac_no= '%s'",g_pub_tx.crd_no);
    if (ret)
    {
        vtcp_log("%s,%d,ac_no=[%s]查找错误!",__FILE__,__LINE__,g_pub_tx.crd_no);
        strcpy(g_pub_tx.reply,"P102");
        goto ErrExit;
    }

    /* 冲正前的初始化 */
    strcpy(g_pub_tx.cur_no,"01");
    set_zd_data("0210",g_pub_tx.cur_no);
    
    strcpy(g_pub_tx.ct_ind,"2");
    g_pub_tx.tx_amt1 =0 - sChnl_single_hold.tx_amt;
    g_pub_tx.tx_amt2 =0 - sChnl_single_hold.hold_amt; /*恢复控制金额*/
    g_pub_tx.ac_id_type[0]='1';  /*1-活期账户*/
    g_pub_tx.ac_id = sMdm_ac_rel.ac_id;
    g_pub_tx.add_ind[0] = '1'; /* 减少控制金额 */
    g_pub_tx.svc_ind = 1007;   /*  预授权完成  */
    g_pub_tx.hst_ind[0] = '1'; /* 入明细 */
    memcpy(g_pub_tx.brf,tmpstr,20);
    if (pub_acct_trance())
    {
        sprintf(acErrMsg,"pos 预授权--错误!!!");
        WRITEMSG
        goto ErrExit;
    }
    
    /*此处给分录entry_code=F0460赋值*/ 
    set_zd_double("0420",g_pub_tx.tx_amt1); /*交易金额*/
    set_zd_data("0930","2");/*部分冻结*/
    /*判断商户帐号是否存在,状态是否正常*/
    if (strlen(cAc_no)==0)
    {
        sprintf(acErrMsg,"未传入商户帐号");
        WRITEMSG
        strcpy(g_pub_tx.reply,"AT08");
        goto ErrExit;
    }
    else if(strlen(cAc_no)>10)
    {
        memset(&sMdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
        ret= Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel," ac_no= '%s'",cAc_no);
        if (ret)
        {
            vtcp_log("%s,%d,ac_no=[%s]查找错误!",__FILE__,__LINE__,cAc_no);
            strcpy(g_pub_tx.reply,"P102");
            goto ErrExit;
        }
        ret= Prdt_ac_id_Sel(g_pub_tx.reply,&sPrdt_ac_id,"ac_id=%ld",sMdm_ac_rel.ac_id);
        if (ret)
        {
            vtcp_log("%s,%d,ac_no[%s]查询prdt_ac_rel错误!",__FILE__,__LINE__,cAc_no);
            strcpy(g_pub_tx.reply,"K106");
            goto ErrExit;
        }
        if (sPrdt_ac_id.ac_id_type[0]=='1')/*商户帐号如果是活期*/
        {
            /*查询商户帐号是否存在*/
            ret=Dd_mst_Sel(g_pub_tx.reply,&sDd_mst,"ac_id=%ld",sMdm_ac_rel.ac_id);
            if (ret)
            {
                vtcp_log("%s,%d,ac_id[%s]查找错误!",__FILE__,__LINE__,sDd_mst.ac_id);
                strcpy(g_pub_tx.reply,"P102");
                goto ErrExit;
            }
            /*查询商户帐号状态是否正常*/
            if (sDd_mst.ac_sts[0]!='1')
            {
                vtcp_log("%s,%d,ac_no该账户不正常!",__FILE__,__LINE__);
                strcpy(g_pub_tx.reply,"O085");
                goto ErrExit;
            }
            set_zd_data("1021",cAc_no);/*商户帐号*/
            set_zd_int("1022",1);     /*账户序号*/
            set_zd_data("0192","0000");  /*打折标志*/
            set_zd_data("102Y",tmpstr);
            set_zd_data("102J","01");/*币种*/
            set_zd_data("102K","2");/*现转标记借方*/
            set_zd_double("102F",g_pub_tx.tx_amt1);/*交易金额记为负数*/
            strcpy(g_pub_tx.reply,"0001");/*调用D003活期借方记帐*/
        }
        else if (sPrdt_ac_id.ac_id_type[0]=='9')/*如果商户帐号是内部帐号*/
        {   
            vtcp_log("%s,%d,程序走到这了1",__FILE__,__LINE__);
            ret=In_mst_Sel(g_pub_tx.reply,&sIn_mst,"ac_id=%ld",sMdm_ac_rel.ac_id);
            if (ret)
            {
                vtcp_log("%s,%d,ac_no[%s]查找错误!",__FILE__,__LINE__,cAc_no);
                strcpy(g_pub_tx.reply,"P102");
                goto ErrExit;
            }
            if (sIn_mst.sts[0]!='1')
            {
                vtcp_log("%s,%d,ac_no[%s]该账户不正常!",__FILE__,__LINE__,cAc_no);
                strcpy(g_pub_tx.reply,"O085");
                goto ErrExit;
            }
            set_zd_data("1201",cAc_no);/*账号*/
            set_zd_data("1204","01"); /*币种*/
            set_zd_data("1205","2");/*现转标志*/
            set_zd_double("1208",g_pub_tx.tx_amt1);/*交易金额*/
            set_zd_data("81",tmpbrf);/*商户帐号*/
            strcpy(g_pub_tx.reply,"0002");/*调用A016内部帐号借方记帐*/
            vtcp_log("%s,%d,程序走到这了2",__FILE__,__LINE__);
        }
    }
    else if (strlen(cAc_no)<=10)
    {
        ret = Com_item_Sel(g_pub_tx.reply,&sCom_item,"acc_no = '%s'",cAc_no);
        if(ret)
        {
            sprintf(acErrMsg,"查询科目失败!!!");
            WRITEMSG
            strcpy(g_pub_tx.reply, "B172");
            goto ErrExit;
        }
        set_zd_data("1201",cAc_no);/*账号*/
        set_zd_data("1204","01"); /*币种*/
        set_zd_data("1205","2");/*现转标志*/
        set_zd_double("1208",g_pub_tx.tx_amt1);/*交易金额*/
        set_zd_data("81",tmpbrf);/*商户帐号*/
        strcpy(g_pub_tx.reply,"0002");/*调用A016内部帐号借方记帐*/
    }

    int atm_qy_free_flag=0;/* atm 查询免费标志 0免费 1收费 */
    int pos_qy_free_flag=0;/* pos 查询免费标志 0免费 1收费 */
    int pos_tx_free_flag=0;/* pos 交易免费标志 0免费 1收费 */
    int atm_tx_free_flag=0;/* atm 交易免费标志 0免费 1收费 */
    int dscpt = 0, iselflag=0;
    dscpt = CARD_TRADE_TRAN;
    vtcp_log("%s,%d,POS标志=[%c]",__FILE__,__LINE__,cFee_ind[0]);
    if (cFee_ind[0]=='0')
        iselflag = 0;/*add by lifei for nx 2009-8-2 10:31:00 有本行pos*/
    else   
        iselflag = 1; 
        
    /*完成撤销冲正交易，借方记正数*/
    ret = pub_card_reg(sChnl_single_hold.tx_amt,"POS",'0',&atm_qy_free_flag,&pos_qy_free_flag,&atm_tx_free_flag,&pos_tx_free_flag,iselflag,dscpt);
    if (ret)
    {
        sprintf(acErrMsg,"插入卡登记簿失败!");
        WRITEMSG
		strcpy(g_pub_tx.reply,"CU06");
        goto ErrExit;
    }
    /*set_zd_long("0040",sChnl_single_hold.trace_no);返回核心流水号  不要用第四域传流水，换个域使用*/
    vtcp_log("[%s][%d] trace_no =[%ld]",__FILE__,__LINE__,g_pub_tx.trace_no);
OkExit:
    sprintf(acErrMsg,"success!!");
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;

ErrExit:
    if (memcmp(g_pub_tx.reply,"0000",4)==0)
    {
        strcpy(g_pub_tx.reply,"CU06");
    }
    sprintf(acErrMsg,"failed !! reply is[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
