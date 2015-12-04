/*********************************************************************
* 版 本 号 :  V1.0        
* 程序作者 :  CHENGBO          
* 日    期 :  2011-08-03   
* 所属模块 :  与平台的接口程序 (收费处理)           
* 程序名称 :  spF000.c
* 程序作用 :  处理平台发送的收费记账，当有多条收费记录时要循环记账
* 函数列表 :       
*         1、pub_acct_trance();
*         2、pubf_acct_proc();                              
* 使用注意 :                                       
* 修改记录 :                                       
*     修改作者 :    gong                       
*     修改时间 :  2011/10/18 10:20:22                                  
*     修改内容 :不传71域，123域添加借贷标志，需要根据借贷标志判断借贷方记账
insert into tx_sub_def   values ('F000', '收费子交易', 'spF000', '0', '0');
******************************************************************/
#include <stdio.h>
#include "public.h"
#include "mdm_ac_rel_c.h"
#include "dd_mst_c.h"
#include "in_mst_c.h"
#include "com_item_c.h"
#include "prdt_ac_id_c.h"
#include "chnl_single_acct_c.h"
#include "chnl_single_hold_c.h"
int nGetOpnbrno(char * opn_brno)
{
	struct mdm_ac_rel_c stMdm_ac_rel;
	int    iRet=0;
	char sAcno[32]={0};
	memset(&stMdm_ac_rel,0x00,sizeof(stMdm_ac_rel));
	get_zd_data("0300",sAcno);
	iRet= Mdm_ac_rel_Sel(g_pub_tx.reply,&stMdm_ac_rel," ac_no= '%s'",sAcno);
  if( iRet )
  {
  	vtcp_log("%s,%d,ac_no=[%s]查找错误!",__FILE__,__LINE__,sAcno);
  	strcpy(g_pub_tx.reply,"P102");
  	return -1;
  }
  strcpy(opn_brno,stMdm_ac_rel.opn_br_no);
  return 0;
}
spF000()
{
    int    iRet=0;
    int    i=0;            /*循环时使用*/
    int    j=0;            /*判断现金科目用*/
    double dSum=0.00;     /*求和时使用*/
    double dInit=0.00;     /*初始化*/
    long   lFee_cnt=0;     /*收费条数*/
    char   cFee_flag[2];   /*收费标志*/
    char   cCt_ind[2];     /*现转标志*/
    char   cDc_ind[2];     /*借贷标志*/
    char   cFee_cnt[3];    /*收费条数*/
    char   cFd123[1000];   /*接收123域内容*/
    char   cAc_no[25];     /*收费主账号*/
    char   cAc_no2[25];    /*第二账号*/
    char   cFee_acno[25];  /*费用账号或科目*/
    char   cFee_amt[13];   /*收费金额*/
    char   cFee_brf[11];   /*费用描述*/
    char   cCzTh_ind[2];   /*冲正退回标志*/
    double dFee_amt=0.00;  /*收费金额*/
    char   cAc_no_tmp[25]; /*临时slz*/
    char   sOpn_brno[8]={0}; /*主账号的开户机构*/
    char   sOpn_flag[2]={0};/*记账机构标志 1：记交易机构，2：记主账号开户机构*/   
    
    struct mdm_ac_rel_c  sMdm_ac_rel;
    struct dd_mst_c      sDd_mst;
    struct in_mst_c      sIn_mst;
    struct com_item_c    sCom_item;
    struct chnl_single_acct_c sChnl_single_acct;
    struct prdt_ac_id_c  sPrdt_ac_id;
    struct chnl_single_hold_c  sChnl_single_hold;
    
    memset(&sMdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
    memset(&sDd_mst,0x00,sizeof(struct dd_mst_c));
    memset(&sIn_mst,0x00,sizeof(struct in_mst_c));
    memset(&sCom_item,0x00,sizeof(struct com_item_c));
    memset(&sChnl_single_acct,0x00,sizeof(struct chnl_single_acct_c));
    memset(&sPrdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));
    memset(&sChnl_single_hold,0x00,sizeof(struct chnl_single_acct_c));
    memset(cFee_flag,0x00,sizeof(cFee_flag));
    memset(cFd123,0x00,sizeof(cFd123));
    memset(cAc_no,0x00,sizeof(cAc_no));
    memset(cFee_acno,0x00,sizeof(cFee_acno));
    memset(cFee_amt,0x00,sizeof(cFee_amt));
    memset(cFee_brf,0x00,sizeof(cFee_brf));
    memset(cAc_no2,0x00,sizeof(cAc_no2));
    memset(cCt_ind,0x00,sizeof(cCt_ind));
    memset(cDc_ind,0x00,sizeof(cDc_ind));
    memset(cFee_cnt,0x00,sizeof(cFee_cnt));
    memset(cAc_no_tmp,0x00,sizeof(cAc_no_tmp));
    pub_base_sysinit();
    
    
    get_zd_data("0710",cFee_flag); /*71域 modified by gong 2011/10/17 16:56:12收费标志0-不收，1-收*/
    
    if( cFee_flag[0] == '0' || strlen(cFee_flag)==0)
    {
        vtcp_log("[%s][%d],不收费直接返回成功!!!",__FILE__,__LINE__);
        goto OkExit;
    }
    
    get_fd_data("1230",cFd123);
    
    memcpy(cCt_ind,cFd123,1);
    vtcp_log("[%s][%d],cCt_ind=[%c]",__FILE__,__LINE__,cCt_ind[0]);
    memcpy(cFee_cnt,cFd123+1,2);
    vtcp_log("[%s][%d],cFee_cnt=[%s]",__FILE__,__LINE__,cFee_cnt);
    lFee_cnt=atoi(cFee_cnt);
    vtcp_log("[%s][%d],lFee_cnt=[%d]",__FILE__,__LINE__,lFee_cnt);
    
    i=0;
    j=0;
    dSum=0.00;
    for(i=0;lFee_cnt > i; i++)
    {
        memset(cDc_ind,0x00,sizeof(cDc_ind));
        memset(cFee_amt,0x00,sizeof(cFee_amt));
        memset(cFee_acno,0x00,sizeof(cFee_acno));
        dFee_amt=0.00;
        memcpy(cFee_acno,cFd123+3 + 48 * i,24);
        zip_space(cFee_acno);
        if( strlen(cFee_acno) == 0 )
        {
             vtcp_log("[%s][%d],取到的账号为空，停止取值",__FILE__,__LINE__);
             WRITEMSG
             strcpy(g_pub_tx.reply,"YY72");
             goto ErrExit;
        }
        else if (strcmp(cFee_acno,"10101") == 0 && cCt_ind[0] == '1')
        {
             j=1111;
             vtcp_log("[%s][%d],j的数值为=[%d]",__FILE__,__LINE__,j);
        }
        memcpy(cDc_ind,cFd123+27 + 48 * i,1);
        vtcp_log("[%s][%d],cDc_ind=[%c]",__FILE__,__LINE__,cDc_ind[0]);
        memcpy(cFee_amt,cFd123+29 + 48 * i,12);
        vtcp_log("[%s][%d],出过来的cFee_amt=[%s]是没有浮点的字符串",__FILE__,__LINE__,cFee_amt);
        dFee_amt=atof(cFee_amt) / 100;
        
        if(cDc_ind[0] == '1')
        {
            dFee_amt= -1 * dFee_amt;
        }
        dSum+=dFee_amt;
	vtcp_log("[%s][%d],dFee_amt的数值为=[%lf]",__FILE__,__LINE__,dFee_amt);
	vtcp_log("[%s][%d],kankan dSum的数值为=[%lf]",__FILE__,__LINE__,dSum);
    }
    vtcp_log("[%s][%d],j的数值为=[%d]",__FILE__,__LINE__,j);
    vtcp_log("[%s][%d],zaikankan dSum的数值为=[%lf]",__FILE__,__LINE__,dSum);
    if( pub_base_CompDblVal(dSum,0.00) !=0 )
    {
        sprintf(acErrMsg, "传过来的借贷数据不平衡！");
        set_zd_data(DC_GET_MSG,acErrMsg);
        WRITEMSG
        goto ErrExit;
    }
    else if (j !=1111 && cCt_ind[0] == '1')
    { 
        sprintf(acErrMsg, "现转标志为现金的时候，必须有现金科目！");
        set_zd_data(DC_GET_MSG,acErrMsg);
        WRITEMSG
        goto ErrExit;
    }
    else
    {
        if( cCt_ind[0] == '1')      /*现金*/
        {
            i=0;
            for(i=0;lFee_cnt > i; i++)
            {
                 memset(cFee_acno,0x00,sizeof(cFee_acno));
                 memset(cFee_amt,0x00,sizeof(cFee_amt));
                 memset(cFee_brf,0x00,sizeof(cFee_brf));
                 memset(cDc_ind,0x00,sizeof(cDc_ind));
                 dFee_amt=0.00;
                 memcpy(cFee_acno,cFd123+3 + 48 * i,24);
                 zip_space(cFee_acno);
                 vtcp_log("[%s][%d],cFee_acno=[%s]",__FILE__,__LINE__,cFee_acno);
                 if(strlen(cFee_acno) == 0 )
                 {
                      vtcp_log("[%s][%d],取到的账号为空，停止取值",__FILE__,__LINE__);
                      WRITEMSG
                      strcpy(g_pub_tx.reply,"YY72");
                      goto ErrExit;
                 }
                 memcpy(cDc_ind,cFd123+27 + 48 * i,1);
                 vtcp_log("[%s][%d],cDc_ind=[%c]",__FILE__,__LINE__,cDc_ind[0]);
                 memcpy(cFee_amt,cFd123+29 + 48 * i,12);
                 vtcp_log("[%s][%d],出过来的cFee_amt=[%s]是没有浮点的字符串",__FILE__,__LINE__,cFee_amt);
                 dFee_amt=atof(cFee_amt) / 100;
                 vtcp_log("[%s][%d],dFee_amt=[%lf]",__FILE__,__LINE__,dFee_amt);
                /*memcpy(cFee_brf,cFd123+48 + 48 * i,10);*/ /*20150918*/
                memcpy(cFee_brf,cFd123+41 + 48 * i,10);
                 vtcp_log("[%s][%d],cFee_brf=[%s]",__FILE__,__LINE__,cFee_brf);
                 
                 if(memcmp(g_pub_tx.tx_code,"6235",4)==0 || 
                 	memcmp(g_pub_tx.tx_code,"6240",4)==0 || 
                 	memcmp(g_pub_tx.tx_code,"7023",4)==0 || 
                 	memcmp(g_pub_tx.tx_code,"6254",4)==0 || 
                    memcmp(g_pub_tx.tx_code,"6246",4)==0 ||
                    (memcmp(g_pub_tx.tx_code,"6236",4)==0 && memcmp(cFee_flag,"2",1)==0))
                 {   /*冲正类记负数(当交易代码为6236并且收费标志为2-贷记退回时，收费金额负数冲回 ADD BY CHENGBO 20120106)*/
                     dFee_amt= -1 * dFee_amt;
                 	 vtcp_log("[%s][%d],dFee_amt=[%lf]",__FILE__,__LINE__,dFee_amt);
                 }
                 zip_space(cFee_acno);
                 if( strlen(cFee_acno) >= 13 )    /*账号*/
                 {
                      memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
                      iRet= Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel," ac_no= '%s'",cFee_acno);
                      if( iRet )
                      {
                           vtcp_log("%s,%d,ac_no=[%s]查找错误!",__FILE__,__LINE__,cFee_acno);
                           strcpy(g_pub_tx.reply,"P102");
                           goto ErrExit;
                      }
                      memset(&sPrdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));
                      iRet = Prdt_ac_id_Sel(g_pub_tx.reply,&sPrdt_ac_id,"ac_id=%ld ",g_mdm_ac_rel.ac_id);
                      if (iRet)
                      {
                           vtcp_log("%s,%d,ac_no[%s]查询prdt_ac_rel错误!",__FILE__,__LINE__,cFee_acno);
                           strcpy(g_pub_tx.reply,"K106");
                           goto ErrExit;
                      }
                      if(sPrdt_ac_id.ac_id_type[0] == '1')  /*活期*/
                      {    
                           vtcp_log("[%s],[%d],跟踪---1!",__FILE__,__LINE__);
                           memset(&sDd_mst,0x00,sizeof(struct dd_mst_c));
                           iRet = Dd_mst_Sel(g_pub_tx.reply,&sDd_mst,"ac_id=%ld",g_mdm_ac_rel.ac_id);
                           if (iRet)
                           {
                             vtcp_log("%s,%d,ac_id[%s]查找错误!",__FILE__,__LINE__,sDd_mst.ac_id);
                             strcpy(g_pub_tx.reply,"P102");
                             goto ErrExit;
                           }
                           if(sDd_mst.ac_sts[0] != '1')
                           {
                             vtcp_log("%s,%d,ac_no该账户不正常!",__FILE__,__LINE__);
                             strcpy(g_pub_tx.reply,"O085");
                             goto ErrExit;
                           }
                           g_pub_tx.ac_id=sDd_mst.ac_id;
                           g_pub_tx.ac_seqn=sDd_mst.ac_seqn;
                           g_pub_tx.ac_id_type[0]='1';
                           strncpy(g_pub_tx.opn_br_no, g_mdm_ac_rel.opn_br_no ,sizeof(g_pub_tx.opn_br_no)-1);
                           strcpy(g_pub_tx.ac_no,cFee_acno);
                           g_pub_tx.tx_amt1=dFee_amt;
                           strcpy(g_pub_tx.cur_no,"01");    /* 币种 */
                           strcpy(g_pub_tx.ct_ind,cCt_ind); /*现转*/
                           
                           if( cDc_ind[0] == '1')
                           {
                               strcpy(g_pub_tx.add_ind,"0");/*增减(借贷):0减(借) 1加(贷)*/
                           }
                           else if( cDc_ind[0] == '2' )
                           {
                               strcpy(g_pub_tx.add_ind,"1");/*增减(借贷):0减(借) 1加(贷)*/
                           }
                           else
                           {
                               vtcp_log("[%s][%d] 借贷标志错误!!!",__FILE__,__LINE__);
                               strcpy(g_pub_tx.reply,"P404");
                               goto ErrExit;
                           }
                           strcpy(g_pub_tx.prt_ind,"0");/*是否打折标志*/
                           strcpy(g_pub_tx.hst_ind,"1");      /**日间入明细**/
                           strcpy(g_pub_tx.ac_wrk_ind,"0000000");
                           g_pub_tx.svc_ind=1001;             /**存取款***/
                           strcpy(g_pub_tx.ac_get_ind,"000");
                           strcpy(g_pub_tx.brf,cFee_brf);
                           iRet=pub_acct_trance();
                           if ( iRet )
                           {
                               sprintf(acErrMsg,"调用存取款主控失败!");
                               WRITEMSG
                               strcpy(g_pub_tx.reply,"T999");
                               goto ErrExit;
                           }
                           if( cDc_ind[0] == '2')/*贷方记账*/ 
                           {
                               set_zd_data("101A",g_pub_tx.cur_no);
                               set_zd_data("101B", cCt_ind);
                               set_zd_double("1013", g_pub_tx.tx_amt1);
                               iRet = pubf_acct_proc("D099");
                               if (iRet)
                               {
                                 vtcp_log("%s,%d,记账错误!ret[%ld]",__FILE__,__LINE__,iRet);
                                 strcpy(g_pub_tx.reply,"T999");
                                 goto ErrExit;
                               }
                           }else if( cDc_ind[0] == '1'){/*借方记账*/ 
                               set_zd_data("102J", g_pub_tx.cur_no);
                               set_zd_data("102K", cCt_ind);
                               set_zd_double("102F", g_pub_tx.tx_amt1);
                               set_zd_double("102P",dInit);
                               set_zd_double("102Q",dInit);
                               iRet = pubf_acct_proc("D003");
                               if (iRet)
                               {
                                   vtcp_log("%s,%d,记账错误!ret[%ld]",__FILE__,__LINE__,iRet);
                                   strcpy(g_pub_tx.reply,"T999");
                                   goto ErrExit;
                               }
                           }
                           else
                           {
                               vtcp_log("[%s][%d] 借贷标志错误!!!",__FILE__,__LINE__);
                               strcpy(g_pub_tx.reply,"P404");
                               goto ErrExit;
                           } 
                      }
                      else if(sPrdt_ac_id.ac_id_type[0] == '9')  /*内部*/
                      {
                           vtcp_log("[%s],[%d],跟踪---2!",__FILE__,__LINE__);
                           memset(&sIn_mst,0x00,sizeof(struct in_mst_c));
                           iRet = In_mst_Sel(g_pub_tx.reply,&sIn_mst,"ac_id=%ld",g_mdm_ac_rel.ac_id);
                           if (iRet)
                           {
                             vtcp_log("%s,%d,ac_no[%s]查找错误!",__FILE__,__LINE__,cFee_acno);
                             strcpy(g_pub_tx.reply,"P102");
                             goto ErrExit;
                           }
                           if(sIn_mst.sts[0] != '1')
                           {
                             vtcp_log("%s,%d,ac_no[%s]该账户不正常!",__FILE__,__LINE__,cFee_acno);
                             strcpy(g_pub_tx.reply,"O085");
                             goto ErrExit;
                           }
                           g_pub_tx.ac_id=sIn_mst.ac_id;
                           g_pub_tx.ac_seqn=sIn_mst.ac_seqn;
                           g_pub_tx.ac_id_type[0]='9';
                           strncpy(g_pub_tx.opn_br_no, g_mdm_ac_rel.opn_br_no ,sizeof(g_pub_tx.opn_br_no)-1);
                           strcpy(g_pub_tx.ac_no,cFee_acno);
                           g_pub_tx.tx_amt1=dFee_amt;
                           strcpy(g_pub_tx.cur_no,"01");    /* 币种 */
                           strcpy(g_pub_tx.ct_ind,cCt_ind);
                           if( cDc_ind[0] == '2')
                           {
                               strcpy(g_pub_tx.add_ind,"1");/*增减(借贷):0减(借) 1加(贷)*/
                           }
                           else if( cDc_ind[0] == '1' )
                           {
                               strcpy(g_pub_tx.add_ind,"1");/*增减(借贷):0减(借) 1加(贷)*/
                           }
                           else
                           {
                               sprintf(acErrMsg,"借贷标志错误!");
                               WRITEMSG
                               strcpy(g_pub_tx.reply,"P404");
                               goto ErrExit;
                           }
                           strcpy(g_pub_tx.prt_ind,"0");/*是否打折标志*/
                           strcpy(g_pub_tx.hst_ind,"1");/**日间入明细**/
                           strcpy(g_pub_tx.ac_wrk_ind,"0000000");
                           g_pub_tx.svc_ind=9001;/* 内部帐存取 */
                           strcpy(g_pub_tx.ac_get_ind,"000");/**存取款***/
                           strcpy(g_pub_tx.brf,cFee_brf);
                           iRet=pub_acct_trance();
                           if ( iRet )
                           {
                               sprintf(acErrMsg,"调用存取款主控失败!");
                               WRITEMSG
                               strcpy(g_pub_tx.reply,"T999");
                               goto ErrExit;
                           }
                           if( cDc_ind[0] == '2')/*贷方记账*/ 
                           {
                               set_zd_data("1214",g_pub_tx.cur_no);
                               set_zd_data("1215",cCt_ind );
                               set_zd_double("1218",g_pub_tx.tx_amt1);
                               iRet = pubf_acct_proc("A017");
                               if (iRet)
                               {
                                   sprintf(acErrMsg, "调用存取款主控失败[%s]", g_pub_tx.reply);
                                   WRITEMSG
                                   strcpy(g_pub_tx.reply,"T999");
                                   goto ErrExit;
                               }
                           }
                           else if( cDc_ind[0] == '1')   /*借方记账*/
                           { 
                               set_zd_data("1204",g_pub_tx.cur_no);
                               set_zd_data("1205",cCt_ind );
                               set_zd_double("1208",g_pub_tx.tx_amt1);
                               iRet = pubf_acct_proc("A016");
                               if (iRet)
                               {
                                   sprintf(acErrMsg, "调用存取款主控失败[%s]", g_pub_tx.reply);
                                   WRITEMSG
                                   strcpy(g_pub_tx.reply,"T999");
                                   goto ErrExit;
                               }
                           }
                           else
                           {
                               sprintf(acErrMsg, "借贷标志错误!!!");
                               WRITEMSG
                               strcpy(g_pub_tx.reply,"P404");
                               goto ErrExit;
                           }
                      }
                 }
                 else if( strlen(cFee_acno) < 13) /*科目*/
                 {
                     if( memcmp(cFee_acno,"10101",5)==0)/*现金记账*/
                     {
                           if( cDc_ind[0] == '1')
                           {
                               g_pub_tx.svc_ind=0;
                               g_pub_tx.ac_id=0; 
                               strcpy(g_pub_tx.cur_no,"01");
                               g_pub_tx.tx_amt1=dFee_amt;
                               strcpy(g_pub_tx.brf,cFee_brf);
                               strcpy(g_pub_tx.name,cFee_acno);         /**对方账号**/
                               strcpy(g_pub_tx.add_ind, "1");           /**现金收入**/
                               strcpy(g_pub_tx.ct_ind, "1");            /**现转标志**/
                               strcpy(g_pub_tx.hst_ind, "1");
                               iRet = pub_acct_cash();
                               if (iRet) 
                               {
                                   sprintf(acErrMsg, "调用现金收付主控失败!");
                                   WRITEMSG
                                   goto ErrExit;
                               }
                               
                               set_zd_data("1181", g_pub_tx.cur_no);
                               set_zd_double("1184", g_pub_tx.tx_amt1);    /* 发生额 */
                               iRet = pubf_acct_proc("M001");
                               if (iRet)
                               {
                                    sprintf(acErrMsg, "调用存取款主控失败");
                                    WRITEMSG
                                    strcpy(g_pub_tx.reply,"T999");
                                    goto ErrExit;
                               }
                          }
                          else if( cDc_ind[0] == '2' )
                          {
                               g_pub_tx.svc_ind=0;
                               g_pub_tx.ac_id=0; 
                               strcpy(g_pub_tx.cur_no,"01");
                               g_pub_tx.tx_amt1=dFee_amt;
                               strcpy(g_pub_tx.brf,cFee_brf);
                               strcpy(g_pub_tx.name,cFee_acno);         /**对方账号**/
                               strcpy(g_pub_tx.add_ind, "0");           /**现金付出**/
                               strcpy(g_pub_tx.ct_ind, "1");            /**现转标志**/
                               strcpy(g_pub_tx.hst_ind, "1");
                               iRet = pub_acct_cash();
                               if (iRet) 
                               {
                                   sprintf(acErrMsg, "调用现金收付主控失败!");
                                   WRITEMSG
                                   goto ErrExit;
                               }
                               
                               set_zd_data("1191", g_pub_tx.cur_no);
                               set_zd_double("1194", g_pub_tx.tx_amt1);    /* 发生额 */
                               iRet = pubf_acct_proc("M002");
                               if (iRet)
                               {
                                    sprintf(acErrMsg, "调用存取款主控失败");
                                    WRITEMSG
                                    strcpy(g_pub_tx.reply,"T999");
                                    goto ErrExit;
                               }
                          }
                     }
                     else
                     {
                           memset(&sCom_item,0x00,sizeof(struct com_item_c));
                           iRet = Com_item_Sel(g_pub_tx.reply,&sCom_item,"acc_no = '%s'",cFee_acno);
                           if(iRet)
                           {
                               sprintf(acErrMsg,"查询科目失败!!!");
                               WRITEMSG
                               strcpy(g_pub_tx.reply, "B172");
                               goto ErrExit;
                           }
                          	
                           strcpy(g_pub_tx.ac_no,cFee_acno);
                           g_pub_tx.ac_seqn=0;
                           g_pub_tx.tx_amt1=dFee_amt;
                           strcpy(g_pub_tx.cur_no,"01");    /* 币种 */
                           strcpy(g_pub_tx.ct_ind,cCt_ind);
                           if( cDc_ind[0] == '1')
                           {
                               strcpy(g_pub_tx.add_ind,"0");/*增减(借贷):0减(借) 1加(贷)*/
                           }
                           else if( cDc_ind[0] == '2')
                           {
                               strcpy(g_pub_tx.add_ind,"1");/*增减(借贷):0减(借) 1加(贷)*/
                           }
                           else
                           {
                               sprintf(acErrMsg, "借贷标志错误!!!");
                               WRITEMSG
                               strcpy(g_pub_tx.reply,"P404");
                               goto ErrExit;
                           }
                           strcpy(g_pub_tx.prt_ind,"0");/*是否打折标志*/
                           strcpy(g_pub_tx.hst_ind,"1");/**日间入明细**/  
                           strcpy(g_pub_tx.ac_wrk_ind,"0000000");
                           g_pub_tx.svc_ind=9001;/* 内部帐存取 */
                           strcpy(g_pub_tx.ac_get_ind,"000");/**存取款***/
                           strcpy(g_pub_tx.brf,cFee_brf);
                           iRet=pub_acct_trance();
                           if ( iRet )
                           {
                               sprintf(acErrMsg,"调用存取款主控失败!");
                               WRITEMSG
                               strcpy(g_pub_tx.reply,"T999");
                               goto ErrExit;
                           }
                           if( cDc_ind[0] == '2')/*贷方记账*/ 
                           {
                               set_zd_data("1214",g_pub_tx.cur_no);
                               set_zd_data("1215",cCt_ind );
                               set_zd_double("1218",g_pub_tx.tx_amt1);
                               iRet = pubf_acct_proc("A017");
                               if (iRet)
                               {
                                   sprintf(acErrMsg, "调用存取款主控失败[%s]", g_pub_tx.reply);
                                   WRITEMSG
                                   strcpy(g_pub_tx.reply,"T999");
                                   goto ErrExit;
                               }
                           }else if( cDc_ind[0] == '1'){/*借方记账*/ 
                               set_zd_data("1204",g_pub_tx.cur_no);
                               set_zd_data("1205",cCt_ind );
                               set_zd_double("1208",g_pub_tx.tx_amt1);
                               iRet = pubf_acct_proc("A016");
                               if (iRet)
                               {
                                   sprintf(acErrMsg, "调用存取款主控失败[%s]", g_pub_tx.reply);
                                   WRITEMSG
                                   strcpy(g_pub_tx.reply,"T999");
                                   goto ErrExit;
                               }
                           }
                           else
                           {
                               sprintf(acErrMsg, "借贷标志错误!!!");
                               WRITEMSG
                               strcpy(g_pub_tx.reply,"P404");
                               goto ErrExit;
                           }
                         }
                     }
            }
        }
        else if( cCt_ind[0] == '2') /*转账*/
        {
            i=0;
            for(i=0;lFee_cnt > i; i++)
            {
                 memset(cFee_acno,0x00,sizeof(cFee_acno));
                 memset(cFee_amt,0x00,sizeof(cFee_amt));
                 memset(cFee_brf,0x00,sizeof(cFee_brf));
                 memset(cDc_ind,0x00,sizeof(cDc_ind));
                 dFee_amt=0.00;
                 memcpy(cFee_acno,cFd123+3 + 48 * i,24);
                 zip_space(cFee_acno);
                 vtcp_log("[%s][%d],cFee_acno=[%s]",__FILE__,__LINE__,cFee_acno);
                 memcpy(cDc_ind,cFd123+27 + 48 * i,1);
                 vtcp_log("[%s][%d],cDc_ind=[%c]",__FILE__,__LINE__,cDc_ind[0]);
                 if(strcmp(cFee_acno,"") == 0 )
                 {
                      vtcp_log("[%s][%d],取到的账号为空，停止取值",__FILE__,__LINE__);
                      WRITEMSG
                      strcpy(g_pub_tx.reply,"YY72");
                      goto ErrExit;
                 }
                 memcpy(sOpn_flag,cFd123+28 + 48 * i,1);
                 vtcp_log("[%s][%d],出过来的sOpn_flag=[%s]是没有浮点的字符串",__FILE__,__LINE__,sOpn_flag);
                 memcpy(cFee_amt,cFd123+29 + 48 * i,12);
                 vtcp_log("[%s][%d],出过来的cFee_amt=[%s]是没有浮点的字符串",__FILE__,__LINE__,cFee_amt);
                 dFee_amt=atof(cFee_amt) / 100;
                 vtcp_log("[%s][%d],dFee_amt=[%lf]",__FILE__,__LINE__,dFee_amt);
                 memcpy(cFee_brf,cFd123+41 + 48 * i,10);
                 vtcp_log("[%s][%d],cFee_brf=[%s]",__FILE__,__LINE__,cFee_brf); 
                 
                 if(memcmp(g_pub_tx.tx_code,"6235",4)==0 || 
                 	memcmp(g_pub_tx.tx_code,"6240",4)==0 || 
                    memcmp(g_pub_tx.tx_code,"6246",4)==0 || 
                    memcmp(g_pub_tx.tx_code,"7023",4)==0 ||
                    memcmp(g_pub_tx.tx_code,"6254",4)==0 || 
                    (memcmp(g_pub_tx.tx_code,"6236",4)==0 && memcmp(cFee_flag,"2",1)==0))
                 {   /*冲正类记负数(当交易代码为6236并且收费标志为2-贷记退回时，收费金额负数冲回 ADD BY CHENGBO 20120106)*/
                     dFee_amt= -1 * dFee_amt;
                 	 vtcp_log("[%s][%d],冲账金额为负数dFee_amt=[%lf]",__FILE__,__LINE__,dFee_amt);
                 }
                 zip_space(cFee_acno);
                 if( strlen(cFee_acno) >= 13 )    /*账号*/
                 {
                      memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
                      iRet= Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel," ac_no= '%s'",cFee_acno);
                      if( iRet )
                      {
                           vtcp_log("%s,%d,ac_no=[%s]查找错误!",__FILE__,__LINE__,cFee_acno);
                           strcpy(g_pub_tx.reply,"P102");
                           goto ErrExit;
                      }
                      memset(&sPrdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));
                      iRet = Prdt_ac_id_Sel(g_pub_tx.reply,&sPrdt_ac_id,"ac_id=%ld ",g_mdm_ac_rel.ac_id);
                      if (iRet)
                      {
                           vtcp_log("%s,%d,ac_no[%s]查询prdt_ac_rel错误!",__FILE__,__LINE__,cFee_acno);
                           strcpy(g_pub_tx.reply,"K106");
                           goto ErrExit;
                      }
                      if(sPrdt_ac_id.ac_id_type[0] == '1')  /*活期*/
                      {    
                           vtcp_log("[%s],[%d],跟踪---5!",__FILE__,__LINE__);
                           memset(&sDd_mst,0x00,sizeof(struct dd_mst_c));
                           iRet = Dd_mst_Sel(g_pub_tx.reply,&sDd_mst,"ac_id=%ld",g_mdm_ac_rel.ac_id);
                           if (iRet)
                           {
                               vtcp_log("%s,%d,ac_id[%s]查找错误!",__FILE__,__LINE__,sDd_mst.ac_id);
                               strcpy(g_pub_tx.reply,"P102");
                               goto ErrExit;
                           }
                           if(sDd_mst.ac_sts[0] != '1')
                           {
                               vtcp_log("%s,%d,ac_no该账户不正常!",__FILE__,__LINE__);
                               strcpy(g_pub_tx.reply,"O085");
                               goto ErrExit;
                           }
                           g_pub_tx.ac_id=sDd_mst.ac_id;
                           g_pub_tx.ac_seqn=sDd_mst.ac_seqn;
                           g_pub_tx.ac_id_type[0]='1';
                           strncpy(g_pub_tx.opn_br_no, g_mdm_ac_rel.opn_br_no ,sizeof(g_pub_tx.opn_br_no)-1);
                           strcpy(g_pub_tx.ac_no,cFee_acno);
                           g_pub_tx.tx_amt1=dFee_amt;
                           strcpy(g_pub_tx.cur_no,"01");    /* 币种 */
                           strcpy(g_pub_tx.ct_ind,cCt_ind); /*现转*/
                           if( cDc_ind[0] == '1' )
                           {
                           strcpy(g_pub_tx.add_ind,"0");/*增减(借贷):0减(借) 1加(贷)*/
                           }
                           else if( cDc_ind[0] == '2')
                           {
                           strcpy(g_pub_tx.add_ind,"1");/*增减(借贷):0减(借) 1加(贷)*/
                           }
                           strcpy(g_pub_tx.prt_ind,"0");/*是否打折标志*/
                           strcpy(g_pub_tx.hst_ind,"1");      /**日间入明细**/
                           strcpy(g_pub_tx.ac_wrk_ind,"0000000");
                           g_pub_tx.svc_ind=1001;             /**存取款***/
                           strcpy(g_pub_tx.ac_get_ind,"000");
                           strcpy(g_pub_tx.brf,cFee_brf);
                           iRet=pub_acct_trance();
                           if ( iRet )
                           {
                               sprintf(acErrMsg,"调用存取款主控失败!");
                               WRITEMSG
                               strcpy(g_pub_tx.reply,"T999");
                               goto ErrExit;
                           }
                           if( cDc_ind[0] == '2')/*贷方记账*/ 
                           {
                               set_zd_data("101A",g_pub_tx.cur_no);
                               set_zd_data("101B", cCt_ind);
                               set_zd_double("1013", g_pub_tx.tx_amt1);
                               iRet = pubf_acct_proc("D099");
                               if (iRet)
                               {
                                 vtcp_log("%s,%d,记账错误!ret[%ld]",__FILE__,__LINE__,iRet);
                                 strcpy(g_pub_tx.reply,"T999");
                                 goto ErrExit;
                               }
                           }
                           else if( cDc_ind[0] == '1')  /*借方记账*/ 
                           {
                               set_zd_data("102J", g_pub_tx.cur_no);
                               set_zd_data("102K", cCt_ind);
                               set_zd_double("102F", g_pub_tx.tx_amt1);
                               set_zd_double("102P",dInit);
                               set_zd_double("102Q",dInit);
                               iRet = pubf_acct_proc("D003");
                               if (iRet)
                               {
                                   vtcp_log("%s,%d,记账错误!ret[%ld]",__FILE__,__LINE__,iRet);
                                   strcpy(g_pub_tx.reply,"T999");
                                   goto ErrExit;
                               }
                           }  
                      }    
                      else if(sPrdt_ac_id.ac_id_type[0] == '9')  /*内部*/
                      {    
                           vtcp_log("[%s],[%d],跟踪---6!",__FILE__,__LINE__);
                           memset(&sIn_mst,0x00,sizeof(struct in_mst_c));
                           iRet = In_mst_Sel(g_pub_tx.reply,&sIn_mst,"ac_id=%ld",g_mdm_ac_rel.ac_id);
                           if (iRet)
                           {
                               vtcp_log("%s,%d,ac_no[%s]查找错误!",__FILE__,__LINE__,cFee_acno);
                               strcpy(g_pub_tx.reply,"P102");
                               goto ErrExit;
                           }
                           if(sIn_mst.sts[0] != '1')
                           {
                               vtcp_log("%s,%d,ac_no[%s]该账户不正常!",__FILE__,__LINE__,cFee_acno);
                               strcpy(g_pub_tx.reply,"O085");
                               goto ErrExit;
                           }
                           g_pub_tx.ac_id=sIn_mst.ac_id;
                           g_pub_tx.ac_seqn=sIn_mst.ac_seqn;
                           g_pub_tx.ac_id_type[0]='9';
                           strncpy(g_pub_tx.opn_br_no, g_mdm_ac_rel.opn_br_no ,sizeof(g_pub_tx.opn_br_no)-1);
                           strcpy(g_pub_tx.ac_no,cFee_acno);
                           g_pub_tx.tx_amt1=dFee_amt;
                           strcpy(g_pub_tx.cur_no,"01");    /* 币种 */
                           strcpy(g_pub_tx.ct_ind,cCt_ind);
                           if( cDc_ind[0] == '1')
                           {
                           strcpy(g_pub_tx.add_ind,"0");/*增减(借贷):0减(借) 1加(贷)*/
                           }
                           else if( cDc_ind[0] == '2')
                           {
                           strcpy(g_pub_tx.add_ind,"0");/*增减(借贷):0减(借) 1加(贷)*/
                           }
                           strcpy(g_pub_tx.prt_ind,"0");/*是否打折标志*/
                           strcpy(g_pub_tx.hst_ind,"1");/**日间入明细**/
                           strcpy(g_pub_tx.ac_wrk_ind,"0000000");
                           g_pub_tx.svc_ind=9001;/* 内部帐存取 */
                           strcpy(g_pub_tx.ac_get_ind,"000");/**存取款***/
                           strcpy(g_pub_tx.brf,cFee_brf);
                           iRet=pub_acct_trance();
                           if ( iRet )
                           {
                               sprintf(acErrMsg,"调用存取款主控失败!");
                               WRITEMSG
                               strcpy(g_pub_tx.reply,"T999");
                               goto ErrExit;
                           }
                           if( cDc_ind[0] == '2')/*借方记账*/ 
                           {
                               set_zd_data("1214",g_pub_tx.cur_no);
                               set_zd_data("1215",cCt_ind );
                               set_zd_double("1218",g_pub_tx.tx_amt1);
                               iRet = pubf_acct_proc("A017");
                               if (iRet)
                               {
                                   sprintf(acErrMsg, "调用存取款主控失败[%s]", g_pub_tx.reply);
                                   WRITEMSG
                                   strcpy(g_pub_tx.reply,"T999");
                                   goto ErrExit;
                               }
                           }
                           else if( cDc_ind[0] == '1') /*贷方记账*/
                           { 
                               set_zd_data("1204",g_pub_tx.cur_no);
                               set_zd_data("1205",cCt_ind );
                               set_zd_double("1208",g_pub_tx.tx_amt1);
                               iRet = pubf_acct_proc("A016");
                               if (iRet)
                               {
                                   sprintf(acErrMsg, "调用存取款主控失败[%s]", g_pub_tx.reply);
                                   WRITEMSG
                                   strcpy(g_pub_tx.reply,"T999");
                                   goto ErrExit;
                               }
                           }
                      }
                 }
                 else if( strlen(cFee_acno) < 13) /*科目*/
                 {
                       
                       if(sOpn_flag[0]=='2') /*记主账号开户机构*/
                       {
                       		memset(sOpn_brno,0x00,sizeof(sOpn_brno));
                       		iRet=nGetOpnbrno(sOpn_brno);
                       		if(iRet)
                       		{
                       			vtcp_log("[%s][%d],取开户机构错误[%s]",__FILE__,__LINE__,sOpn_brno);
                       			goto ErrExit;
                       		}
                       		vtcp_log("[%s][%d],cFee_brf=[%s]",__FILE__,__LINE__,sOpn_brno);
                       		memcpy(g_pub_tx.opn_br_no,sOpn_brno,sizeof(g_pub_tx.opn_br_no)-1);
                       }else{ /*记交易机构*/
                       		/*人行拒绝冲账手续费收入需要冲回原交易机构 Modefied by YangMeng 2014/7/12 16:31:08 */
                       		if (memcmp(g_pub_tx.tx_code,"7023",4) && memcmp(g_pub_tx.tx_code,"6254",4)){
                       			memcpy(g_pub_tx.opn_br_no,g_pub_tx.tx_br_no,sizeof(g_pub_tx.opn_br_no)-1);
                       			vtcp_log("[%s][%d],g_pub_tx.opn_br_no[%s] sOpn_brno=[%s]",__FILE__,__LINE__,g_pub_tx.opn_br_no,sOpn_brno);
                       		}else{
                       			vtcp_log("[%s][%d],g_pub_tx.opn_br_no[%s] sOpn_brno=[%s]",__FILE__,__LINE__,g_pub_tx.opn_br_no,sOpn_brno);
                       		}
                       }
                       memset(&sCom_item,0x00,sizeof(struct com_item_c));
                       iRet = Com_item_Sel(g_pub_tx.reply,&sCom_item,"acc_no = '%s'",cFee_acno);
                       if(iRet)
                       {
                           sprintf(acErrMsg,"查询科目失败!!!");
                           WRITEMSG
                           strcpy(g_pub_tx.reply, "B172");
                           goto ErrExit;
                       }
                       vtcp_log("[%s][%d],g_pub_tx.opn_br_no[%s] sOpn_brno=[%s]",__FILE__,__LINE__,g_pub_tx.opn_br_no,sOpn_brno);
                       strcpy(g_pub_tx.ac_no,cFee_acno);
                       g_pub_tx.tx_amt1=dFee_amt;
                       g_pub_tx.ac_id=0;
                       g_pub_tx.ac_seqn=0;
                       strcpy(g_pub_tx.cur_no,"01");    /* 币种 */
                       strcpy(g_pub_tx.ct_ind,cCt_ind);
                       if( cDc_ind[0] == '1')
                       {
                       strcpy(g_pub_tx.add_ind,"0");/*增减(借贷):0减(借) 1加(贷)*/
                       }
                       else if( cDc_ind[0] =='2')
                       {
                       strcpy(g_pub_tx.add_ind,"1");/*增减(借贷):0减(借) 1加(贷)*/
                       }
                       strcpy(g_pub_tx.prt_ind,"0");/*是否打折标志*/
                       strcpy(g_pub_tx.hst_ind,"1");/**日间入明细**/
                       strcpy(g_pub_tx.ac_wrk_ind,"0000000");
                       g_pub_tx.svc_ind=9001;/* 内部帐存取 */
                       strcpy(g_pub_tx.ac_get_ind,"000");/**存取款***/
                       strcpy(g_pub_tx.brf,cFee_brf);
                       iRet=pub_acct_trance();
                       if ( iRet )
                       {
                           sprintf(acErrMsg,"调用存取款主控失败!");
                           WRITEMSG
                           strcpy(g_pub_tx.reply,"T999");
                           goto ErrExit;
                       }
                       if( cDc_ind[0] == '2')/*贷方记账*/ 
                       {
                           char tmp_opn_br_no[6]={0};
                           char tmp_tx_br_no[6]={0};
                           vtcp_log("[%s][%d] cFee_acno=[%s]",__FILE__,__LINE__,cFee_acno);
                           if(!memcmp(cFee_acno,"4140301",7) && !memcmp(cFee_flag,"2",1)){
                       /*add by chenggx 这个科目贷记调整要记清算中心 20120106*/
                               strcpy(tmp_tx_br_no,g_pub_tx.tx_br_no);
                               strcpy(tmp_opn_br_no,g_pub_tx.opn_br_no);
                               strcpy(g_pub_tx.tx_br_no,"10001");
                               strcpy(g_pub_tx.opn_br_no,"10001");
                           }
                           set_zd_data("1214",g_pub_tx.cur_no);
                           set_zd_data("1215",cCt_ind );
                           set_zd_double("1218",g_pub_tx.tx_amt1);
                           iRet = pubf_acct_proc("A017");
                           if (iRet)
                           {
                               sprintf(acErrMsg, "调用存取款主控失败[%s]", g_pub_tx.reply);
                               WRITEMSG
                               strcpy(g_pub_tx.reply,"T999");
                               goto ErrExit;
                           }
                           if(!memcmp(cFee_acno,"4140301",5) && !memcmp(cFee_flag,"2",1)){
                       /*add by chenggx 这个科目贷记调整要记清算中心 20120106*/
                               strcpy(g_pub_tx.tx_br_no,tmp_tx_br_no);
                               strcpy(g_pub_tx.opn_br_no,tmp_opn_br_no);
                           }
                           
                       }
                       else if( cDc_ind[0] == '1')  /*借方记账*/
                       { 
                           set_zd_data("1204",g_pub_tx.cur_no);
                           set_zd_data("1205",cCt_ind );
                           set_zd_double("1208",g_pub_tx.tx_amt1);
                           iRet = pubf_acct_proc("A016");
                           if (iRet)
                           {
                               sprintf(acErrMsg, "调用存取款主控失败[%s]", g_pub_tx.reply);
                               WRITEMSG
                               strcpy(g_pub_tx.reply,"T999");
                               goto ErrExit;
                           }
                       }
                 }
            }
        }
        else
        {
            vtcp_log("[%s][%d] cCt_ind=[%c]",__FILE__,__LINE__,cCt_ind[0]);
            sprintf(acErrMsg,"现转标志错误!!!");
            WRITEMSG
            strcpy(g_pub_tx.reply, "B099");
            goto ErrExit;
        }
    }
    
OkExit:
 
    strcpy(g_pub_tx.reply, "0000");
    sprintf(acErrMsg, "Before OK return: reply is[%s]", g_pub_tx.reply);
    WRITEMSG
         set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 0;
ErrExit:
    if(memcmp(g_pub_tx.reply,"0000",4) == 0)
    {
         strcpy(g_pub_tx.reply, "H034");
    }
    sprintf(acErrMsg, "Before return: reply is[%s]", g_pub_tx.reply);
    WRITEMSG
         set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 1;
}


