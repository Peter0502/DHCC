/*************************************************
* 版 本 号： V1.0
* 作    者:  liubingcun
* 完成日期： 20110811
* 文 件 名:  spF248.c
* 所属模块:  与平台的接口程序 (无卡无密消费处理)  
* 功能描述： 返回0001记账A016和D099；返回0002记账A016和A017；返回0003记账D003和D099；返回0004记账D003和A017。 
*            
* 函数列表:  pub_com_test();控制标志检查
*            Chnl_single_acct_Ins();登记chnl_single_acct表
* 修改记录： 
* 日    期:20110816
* 修 改 人：薛珂
* 修改内容:pub_com_test.c()控制标志 本标志适用于新平台所有交易 73域接收 每个字节代表一种控制或检查
insert into tx_def      values ('6247' '无卡无折消费'    '100000000000000000000000000000000000001
00000000000000000000000000000000000000000000000000000000000000100000000000000000000000100' 
'1' '6' '0' '3' 'null');
insert into tx_flow_def values ('6247','0','F248','#=0001@1@#=0002@5@#=0003@3@#=0004@7@#$');
insert into tx_flow_def values ('6247','1','A016','#=0000@4@#$');
insert into tx_flow_def values ('6247','2','A017','#=0000@1@#$');
insert into tx_flow_def values ('6247','3','D003','#=0000@4@#$');
insert into tx_flow_def values ('6247','4','D099','#=0000@8@#$');
insert into tx_flow_def values ('6247','5','A016','#=0000@6@#$');
insert into tx_flow_def values ('6247','6','A017','#=0000@8@#$');
insert into tx_flow_def values ('6247','7','D003','#=0000@6@#$');
insert into tx_flow_def values ('6247','8','F000','#$');/调用收费接口/
insert into tx_sub_def  values ('F248','无卡消费接口','spF248','0','0');
insert into tx_sub_rel  values ('D003','1','11','0191','');
insert into tx_sub_rel  values ('D003','1','000000','0192','');
insert into tx_sub_rel  values ('D099','1','11','0191','');
insert into tx_sub_rel  values ('D099','1','000000','0192','');
*************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "chnl_tradeadd_c.h"
#include "chnl_self_trade_c.h"
#include "chnl_tradelimit_c.h"
#include "card.h" 
#include "march_info_c.h"
#include "march_mst_c.h"
#include "auth_modify_ln_rate_c.h"
#include "mdm_ac_rel_c.h"
#include "chnl_single_acct_c.h"
static char cFd24[4]; /* 业务类型 */
int spF248()
{
    int  iRet=0;
    char cAC_no1[25],cAC_no2[25];
    char cTmp_Brf[51],cTmp_str[101];
    char cBrf20[21];
    char cDc_ind[2], cCt_ind[2],cFee_flag[2];
    char cPOS[2];/*POS标志0本行1他行*/
    double dTx_amt=0.0;
    double dFee_amt=0.00;
    long lPt_date=0;
    long lPt_trace_no=0;
    long lCz_date=0;
    long lCz_lPt_trace_no=0;
    char cLs_ac_no[25];
    char cBrf[128];/*记账摘要*/
    /*调用pub_com_test函数时，接收返回值*/
    char cGz_flag[2];/*挂账标志*/
    char cCtrlcmd[11];/*控制标志*/
    char cCert_no[1];/*证件类型*/
    
    struct march_info_c sMarch_info;
    struct march_mst_c sMarch_mst;
    struct mdm_ac_rel_c sMdm_ac_rel;
    struct dd_mst_c sDd_mst;
    struct chnl_single_acct_c sChnl_single_acct;
    
    memset(&sMarch_mst,0x00,sizeof(sMarch_mst));
    memset(&sMarch_info,0x00,sizeof(sMarch_info));
    memset(cLs_ac_no,0x00,sizeof(cLs_ac_no));
    memset(cCtrlcmd,0x00,sizeof(cCtrlcmd));/*调用pub_com_test时用到*/
    memset(cFd24, 0x0, sizeof(cFd24));
    memset(cAC_no1, 0x0, sizeof(cAC_no1));
    memset(cAC_no2, 0x0, sizeof(cAC_no2));
    memset(cDc_ind, 0x0, sizeof(cDc_ind));
    memset(cCt_ind, 0x0, sizeof(cCt_ind));
    memset(cFee_flag, 0x0, sizeof(cFee_flag));
    memset(cTmp_Brf, 0x0, sizeof(cTmp_Brf));
    memset(cBrf20, 0x0, sizeof(cBrf20));
    memset(cTmp_str,0,sizeof(cTmp_str));
    memset(&sMdm_ac_rel,0,sizeof(struct mdm_ac_rel_c));
    memset(&sDd_mst,0,sizeof(struct dd_mst_c));
    memset(cCert_no, 0x0, sizeof(cCert_no));
    memset(cPOS,0x00,sizeof(cPOS));
    memset(&sChnl_single_acct,0x00,sizeof(struct chnl_single_acct_c));

    if (pub_base_sysinit())
    {
          sprintf(acErrMsg,"初始化公用结构错误!!");
          WRITEMSG
          goto ErrExit;
    }
    get_zd_data("0240",cFd24);
    vtcp_log("%s,%d cFd24=[%s]\n",__FILE__,__LINE__,cFd24);
    get_zd_long("0520",&lPt_trace_no);
    get_zd_long("0440",&lPt_date);
    get_zd_double("0410",&dFee_amt);
    get_zd_long("0450",&lCz_date);
    get_zd_long("0780",&lCz_lPt_trace_no);
    get_zd_data("0660",cDc_ind); /*借贷标志*/ 
    get_zd_data("0680",cCt_ind); /*现转标志*/ 
    get_zd_data("0710",cFee_flag); /*收费模式*/
    get_zd_data("0300", g_pub_tx.crd_no);/*卡号*/
    get_zd_data("0300", cAC_no1); /*客户账号*/  
    get_zd_data("0310", cAC_no2); /*商户账号或带清算账号*/
    get_zd_data("0790",g_pub_tx.draw_pwd);    /* 密码 */
    get_zd_double("0400",&dTx_amt);/*交易金额*/
    get_zd_data("0730",cCtrlcmd);/*控制标志*/
    get_zd_data("0810",cTmp_Brf);/*摘要*/
    get_zd_data("0890",g_pub_tx.note_type);/*凭证种类*/
    get_zd_data("0590",g_pub_tx.beg_note_no);/*凭证号码*/
    get_zd_data("0620", cCert_no);/*证件类型*/
    get_zd_data("0690",cPOS);/*接收POS*/
    vtcp_log("[%s][%d],beg_note_no=[%s]",__FILE__,__LINE__,g_pub_tx.beg_note_no);
    vtcp_log("[%s][%d],note_type=[%s]",__FILE__,__LINE__,g_pub_tx.note_type); 
    memcpy(cBrf20,cTmp_Brf,20);
    strcpy(g_pub_tx.cur_no, "01");
    g_pub_tx.ct_ind[0]=cCt_ind[0];

    vtcp_log("%s,%d,借贷[%s],现转[%s],收费模式[%s],卡号[%s],金额[%.2f]",__FILE__,__LINE__,cDc_ind,cCt_ind,cFee_flag,cAC_no1,dTx_amt);
    vtcp_log("%s,%d,卡号[%s],卡号2[%s]摘要[%s]",__FILE__,__LINE__,cAC_no1,cAC_no2,cTmp_Brf);
    sprintf(acErrMsg,"交易类型=cFee_flag=[%s]",cFee_flag);
    WRITEMSG
    pub_base_old_acno(g_pub_tx.crd_no);
    strcpy(cAC_no1,g_pub_tx.crd_no);
    pub_base_old_acno(cAC_no2);

    /*73域控制标志检查*/
    iRet=pub_com_test(cCtrlcmd,cCert_no,"",g_pub_tx.crd_no,"","","","");
    vtcp_log("%s,%d,cCtrlcmd=[%s],cCert_no=[s%],g_pub_tx.crd_no=[%s],",__FILE__,__LINE__,cCtrlcmd,cCert_no,g_pub_tx.crd_no);
    if (iRet)
    {
         sprintf(acErrMsg,"%s,%d,公共函数pub_com_test()调用失败",__FILE__,__LINE__);
         WRITEMSG
         goto ErrExit;	
    }
    /*为记账准备*/
    set_zd_data("1023",g_pub_tx.note_type); /*凭证种类*/
    set_zd_data("1024",g_pub_tx.beg_note_no); /*凭证号码*/
    /*开始记账*/
    if( cCt_ind[0] == '1')
    {
        vtcp_log("%s,%d,cCt_ind[%s]现转标志错,次交易不允许现金!",__FILE__,__LINE__,cCt_ind);
        strcpy(g_pub_tx.reply,"B099");
        goto ErrExit;
    }
    else if (cCt_ind[0] == '2') /*转帐*/
    {
        if (cDc_ind[0]=='1') /*借*/
        {
            if (cAC_no1[0] != '\0' && (strlen(cAC_no1)<13 ||cAC_no1[0]=='9'))
            {     /*借内部帐*/
                  set_zd_data("1201",cAC_no1);
                  set_zd_data("120A",cBrf20);    
                  set_zd_data("1205","2"); /*转帐*/
                  set_zd_data("1204",g_pub_tx.cur_no);    
                  set_zd_double("1208",dTx_amt);
                  if (strlen(cAC_no2)>12 && cAC_no2[0] != '9')
                  {
                        set_zd_data("1011",cAC_no2);
                        set_zd_data("1012","1"); /*活期序号*/
                        set_zd_double("1013",dTx_amt); /*交易金额*/
                        set_zd_data("101A",g_pub_tx.cur_no);
                        set_zd_data("101B","2"); /*转帐*/
                        set_zd_data("0630",cBrf20);
                        strcpy(g_pub_tx.reply,"0001"); /*借:待清算,贷:客户 spA016,spD099*/
                        vtcp_log("%s,%d,贷:客户,借:清算分支!",__FILE__,__LINE__);
                  }
                  else if (cAC_no2[0] != '\0' && (strlen(cAC_no2)<13 || cAC_no2[0]=='9'))
                  {
                        set_zd_data("1211",cAC_no2);
                        set_zd_double("1218",dTx_amt);
                        set_zd_data("121A",cBrf20);
                        set_zd_data("1215","2");/*转帐*/
                        set_zd_data("1214",g_pub_tx.cur_no);
                        strcpy(g_pub_tx.reply,"0002");/*借:内部户贷:待清,spA016,spA017*/
                        vtcp_log("%s,%d,借内部户,贷清算分支!",__FILE__,__LINE__);
                  }
                  else
                  {
                       vtcp_log("%s,%d,cAC_no1[s%],cAC_no2[%s]卡号不存在,请检查!",__FILE__,__LINE__,cAC_no1,cAC_no2);
                       strcpy(g_pub_tx.reply,"B099");
                       goto ErrExit;
                  }
            }
            else if (strlen(cAC_no1)>12 && cAC_no1[0]!='9')
            {
                  vtcp_log("[%s][%d],走入分支",__FILE__,__LINE__); 
                  /*借客户*/
                  set_zd_data("1021",cAC_no1);
                  set_zd_data("1022","1");/*活期序号*/
                  if (cAC_no1[0]=='5')
                  {
                        set_zd_data("1023",g_pub_tx.note_type); /*票据种类*/
                        set_zd_data("1024",g_pub_tx.beg_note_no); /*票据号码*/
                  }
                  memset(cTmp_str,0,sizeof(cTmp_str));
                  get_zd_data("0710",cTmp_str);
                  memset(cTmp_str,0,sizeof(cTmp_str));
                  set_zd_data("1028",g_pub_tx.draw_pwd);
                  set_zd_double("102F",dTx_amt);    
                  set_zd_data("102K","2");/*现转标志*/
                  set_zd_data("102J","01");
                  set_zd_data("102Y",cBrf20);
                  if (strlen(cAC_no2)>12 && cAC_no2[0] !='9')
                  { 
                       /*借客户贷客户 spD003,spD099*/
                       set_zd_data("1011",cAC_no2);
                       set_zd_data("1012","1");
                       set_zd_double("1013",dTx_amt);
                       set_zd_data("101A",g_pub_tx.cur_no);
                       set_zd_data("101B","2");
                       set_zd_data("0630",cBrf20);
                       set_zd_data("1023",g_pub_tx.note_type); /*票据种类*/
                       set_zd_data("1024",g_pub_tx.beg_note_no); /*票据号码*/
                       vtcp_log("[%s],[%d],凭证类型:[%s] 凭证号码:[%s]",__FILE__,__LINE__,g_pub_tx.note_type,g_pub_tx.beg_note_no);                      
                       strcpy(g_pub_tx.reply,"0003"); /*spD003,spD099*/
                       vtcp_log("%s,%d,借客户贷客户分支!",__FILE__,__LINE__);
                  }
                  else if (cAC_no2[0] != '\0' && (strlen(cAC_no2) <13 || cAC_no2[0]=='9'))
                  {
                       /*借客户，贷:待清算*/
                       /*借:客户,贷:待清算 spD003,spA017*/
                       set_zd_data("1211",cAC_no2);
                       set_zd_double("1218",dTx_amt);
                       set_zd_data("121A",cBrf20);
                       set_zd_data("1215","2");
                       set_zd_data("1214",g_pub_tx.cur_no);
                       strcpy(g_pub_tx.reply,"0004");/*借:客户,贷:待清算 spD003,spA017*/
                       vtcp_log("%s,%d,借待清算贷客户分支!",__FILE__,__LINE__);
                  }
                  else
                  {
                       vtcp_log("%s,%d,cAC_no1[s%],cAC_no2[%s]卡号不存在,请检查!",__FILE__,__LINE__,cAC_no1,cAC_no2);
                       strcpy(g_pub_tx.reply,"B099");
                       goto ErrExit;
                  }
            }
            else
            {
                vtcp_log("%s,%d,cAC_no1[s%],cAC_no2[%s]卡号不存在,请检查!",__FILE__,__LINE__,cAC_no1,cAC_no2);
                strcpy(g_pub_tx.reply,"B099");
                goto ErrExit;   
            }
        }
        else if(cDc_ind[0] == '2')
        { /*贷*/
            if (cAC_no1[0] != '\0' && (strlen(cAC_no1)<13 || cAC_no1[0]=='9'))
            {    /*贷内部帐*/
                 set_zd_data("1211",cAC_no1);
                 set_zd_double("1218",dTx_amt);
                 set_zd_data("121A",cBrf20);
                 set_zd_data("1215","2");
                 set_zd_data("1214",g_pub_tx.cur_no);
                 if (strlen(cAC_no2)>12 && cAC_no2[0] !='9')/*客户*/
                 {
                      set_zd_data("0910",g_pub_tx.tx_br_no);    
                      set_zd_data("1021",cAC_no2);
                      set_zd_data("1022","1");/*活期序号*/
                      set_zd_data("1028",g_pub_tx.draw_pwd);
                      set_zd_double("102F",dTx_amt);    
                      set_zd_data("102K","2");/*现转标志*/
                      set_zd_data("102J","01");
                      set_zd_data("102Y",cBrf20);
                      strcpy(g_pub_tx.reply,"0004");/*借:客户,贷:客户 spD003,spA017*/
                      vtcp_log("%s,%d,借客户贷清算!",__FILE__,__LINE__);
                }
                else if (strlen(cAC_no2)<13 || cAC_no2[0]=='9')
                {
                      set_zd_data("1201",cAC_no2);
                      set_zd_data("120A",cBrf20);    
                      set_zd_data("1205","2"); /*转帐*/
                      set_zd_data("1204",g_pub_tx.cur_no);    
                      set_zd_double("1208",dTx_amt);
                      strcpy(g_pub_tx.reply,"0002");/*借:内部户,贷:待清算spA016,spA017*/
                      vtcp_log("%s,%d,借内部户待内部户!",__FILE__,__LINE__);
                }
                else
                {
                       vtcp_log("%s,%d,cAC_no1[s%],cAC_no2[%s]卡号不存在,请检查!",__FILE__,__LINE__,cAC_no1,cAC_no2);
                       strcpy(g_pub_tx.reply,"B099");
                       goto ErrExit;
                }
            }
            else if(strlen(cAC_no1)>12 && cAC_no1[0] !='9')
            {
                  /*贷客户*/
                  set_zd_data("1011",cAC_no1);
                  set_zd_data("1012","1"); /*活期序号*/
                  set_zd_double("1013",dTx_amt); /*交易金额*/
                  set_zd_data("101A",g_pub_tx.cur_no);
                  set_zd_data("101B","2"); /*转帐*/
                  /*set_zd_data("1016",cBrf20);*/
                  set_zd_data("0630",cBrf20);
                  if (strlen(cAC_no2)>12 && cAC_no2[0] !='9')
                  {    /*借:客户贷:客户*/
                       set_zd_data("1021",cAC_no2);
                       set_zd_data("1022","1"); /* 活期序号 */
                       set_zd_data("1028",g_pub_tx.draw_pwd);
                       set_zd_double("102F",dTx_amt);    
                       set_zd_data("102K","2");/*现转标志*/
                       set_zd_data("102J","01");/*币种*/
                       set_zd_data("102Y",cBrf20);
                       strcpy(g_pub_tx.ac_wrk_ind,"0000000");
                       strcpy(g_pub_tx.reply,"0003"); /*spD003,spD099*/
                       vtcp_log("%s,%d,借客户贷客户分支!",__FILE__,__LINE__);
                  }
                  else if (cAC_no2[0] != '\0' && (strlen(cAC_no2)<13 || cAC_no2[0] =='9'))
                  {    /*借内部户,贷客户*/
                       /*借:内部户,贷:客户 spA016,spD099*/    
                       vtcp_log("%s,%d,帐号[%s]",__FILE__,__LINE__,cAC_no2);
                       set_zd_data("1201",cAC_no2);
                       set_zd_data("120A",cBrf20);    
                       set_zd_data("1205","2"); /*转帐*/
                       set_zd_data("1204",g_pub_tx.cur_no);    
                       set_zd_double("1208",dTx_amt);
                       strcpy(g_pub_tx.reply,"0001"); /*借:待清算,贷:客户 spA016,spD099*/
                       vtcp_log("%s,%d,贷:客户,借:清算分支!",__FILE__,__LINE__);
                  }
                  else
                  {
                       vtcp_log("%s,%d,cAC_no1[s%],cAC_no2[%s]卡号不存在,请检查!",__FILE__,__LINE__,cAC_no1,cAC_no2);
                       strcpy(g_pub_tx.reply,"B099");
                       goto ErrExit;
                  }
            }
        }
        else
        {
            vtcp_log("%s,%d,cDc_ind[%s]借贷标志错!",__FILE__,__LINE__,cDc_ind);
            strcpy(g_pub_tx.reply,"P404");
            goto ErrExit;
        }
    }
    /*else
    {
        vtcp_log("%s,%d,cDc_ind[%s]借贷标志错!",__FILE__,__LINE__,cDc_ind);
        strcpy(g_pub_tx.reply,"P404");
        goto ErrExit;
    }*/
    /*更新卡登记簿*/
    int atm_qy_free_flag=0;/* atm查询免费标志 0免费 1收费 */
    int pos_qy_free_flag=0;/* pos 查询免费标志 0免费 1收费 */
    int pos_tx_free_flag=0;/* pos 交易免费标志 0免费 1收费 */
    int atm_tx_free_flag=0;/* atm 交易免费标志 0免费 1收费 */
    int dscpt = 0;
    int iselflag=0;
    dscpt = CARD_TRADE_TRAN;
    if(cPOS[0] == '0')
    {
      iselflag = 0;
    }
     else 
    {
      iselflag = 1;
    }
    /**登记卡登记簿**/
    iRet=pub_card_reg(dTx_amt,"POS",'0',&atm_qy_free_flag,&pos_qy_free_flag,&atm_tx_free_flag,&pos_tx_free_flag,iselflag,dscpt);
    if(iRet)
    {
        sprintf(acErrMsg,"插入卡登记簿失败!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"CU07");
        goto ErrExit;
    }
    
    /*登记单笔记账登记薄**/
    sChnl_single_acct.tx_date = g_pub_tx.tx_date;    /** 系统日期  **/
    sprintf(sChnl_single_acct.tx_time,"%6ld", g_pub_tx.tx_time);    /** 系统时间  **/
    /*sChnl_single_acct.trace_no = g_pub_tx.ejfno;    ** 核心流水 非柜员流水号，登记的是核心交易流水号 Mod by CHENGBO 20110727**/
    sChnl_single_acct.trace_no = g_pub_tx.trace_no;    /** 核心流水  **/
    memcpy(sChnl_single_acct.cur_no,"01",2); /**  暂时写死为01人民币 ***/
    memcpy(sChnl_single_acct.ac_no, cAC_no1,sizeof( sChnl_single_acct.ac_no)-1);    /** 主账号 **/
    if(cDc_ind[0]=='1')
    {
        memcpy(sChnl_single_acct.ac_no1, cAC_no1,sizeof( sChnl_single_acct.ac_no)-1);    /** 付款账号 **/
        memcpy(sChnl_single_acct.ac_no2, cAC_no2,sizeof( sChnl_single_acct.ac_no)-1);    /** 收款账号 **/
    }
    else if(cDc_ind[0]=='2')
    {
        memcpy(sChnl_single_acct.ac_no1, cAC_no2,sizeof( sChnl_single_acct.ac_no)-1);    /** 付款账号 **/
        memcpy(sChnl_single_acct.ac_no2, cAC_no1,sizeof( sChnl_single_acct.ac_no)-1);    /** 收款账号 **/
    }
    memcpy(sChnl_single_acct.tx_code,g_pub_tx.tx_code,sizeof(sChnl_single_acct.tx_code)-1);    /** 交易码 **/
    memcpy(sChnl_single_acct.tx_br_no,g_pub_tx.tx_br_no,sizeof(sChnl_single_acct.tx_br_no)-1);    /** 交易机构 **/
    memcpy(sChnl_single_acct.opn_br_no,sChnl_single_acct.opn_br_no,sizeof(sChnl_single_acct.opn_br_no)-1);    /** 开户机构 **/
    memcpy(sChnl_single_acct.tel,g_pub_tx.tel,sizeof(sChnl_single_acct.tel)-1);    /** 交易柜员  **/
    memcpy(sChnl_single_acct.bbmk,g_pub_tx.tty,sizeof(sChnl_single_acct.bbmk)-1);    /** 系统种类 **/
    memcpy(sChnl_single_acct.ttyn,g_pub_tx.tty,sizeof(sChnl_single_acct.ttyn)-1);    /** 终端号 **/
    sChnl_single_acct.pt_date = lPt_date;    /** 平台日期  **/
    sprintf(sChnl_single_acct.pt_trace_no ,"%ld" ,lPt_trace_no); /** 平台流水 **/
    sChnl_single_acct.tx_amt = dTx_amt ;    /** 交易金额 **/
    sChnl_single_acct.ct_ind[0] = '2';    /** 现转标志  本交易只处理转账 **/
    memcpy(sChnl_single_acct.brf,cBrf,sizeof(sChnl_single_acct.brf)-1);    /** 记账摘要 **/
    sChnl_single_acct.sts[0] = '1';    /** 记账状态初始为成功 **/
    sChnl_single_acct.dz_ind[0] = '0';    /**  对账状态 **/
    iRet = Chnl_single_acct_Ins(sChnl_single_acct);/* 插入更新表*/
    if(iRet)
    {
        sprintf(acErrMsg,"登记单笔记账登记薄出错!!!");
        WRITEMSG
        strcpy(g_pub_tx.reply, "D107");
        goto ErrExit;
    }
    
OkExit:
	  vtcp_log("[%s],[%d],凭证类型:[%s] 凭证号码:[%s]",__FILE__,__LINE__,g_pub_tx.note_type,g_pub_tx.beg_note_no);
    sprintf(acErrMsg,"处理成功!!");
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"银联帐务处理失败!! ");
    WRITEMSG
    if ( !memcmp(g_pub_tx.reply,"0000",4) || strlen(g_pub_tx.reply) ==0)
    {
        strcpy(g_pub_tx.reply,"CU06");
    }
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}

