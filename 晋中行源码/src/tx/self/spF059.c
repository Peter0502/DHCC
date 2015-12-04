/*********************************************************************
* 版 本 号 :  V1.0        
* 程序作者 :  CHENGBO          
* 日    期 :  2012-07-07   
* 所属模块 :  与平台的接口程序 (第二分录记账处理)           
* 程序名称 :  spF059.c
* 程序作用 :  处理平台发送的第二分录记账，当有多条分录时要循环记账
* 函数列表 :       
*         1、pub_acct_trance();
*         2、pubf_acct_proc();                              
* 使用注意 :                                       
* 修改记录 :                                       
*     修改作者 :                       
*     修改时间 :                                  
*     修改内容 :
insert into tx_sub_def   values ('F059', '第二分录记账', 'spF059', '0', '0');
******************************************************************/
#include <stdio.h>
#include "public.h"
#include "mdm_ac_rel_c.h"
#include "dd_mst_c.h"
#include "in_mst_c.h"
#include "com_item_c.h"
#include "prdt_ac_id_c.h"
#include "chnl_single_cnaps_c.h"
#include "chnl_single_acct_c.h"

int spF059()
{
    int    iRet=0;
    int    i=0;            /*控制循环*/
    double dSum=0.00;      /*求和时使用*/
    char   cDc_ind[2];     /*借贷标志*/
    char   cFd95[601];     /*接收95域内容*/
    char   cFd81[601];     /*接收81域内容*/
    char   cAc_no[25];     /*第二分录账号*/
    char   cCt_ind[2];     /*现转标志*/
    char   cTx_amt[17];    /*交易金额*/
    char   cTx_brf[11];    /*交易描述*/
    char   cCount[3];      /*分录条数*/
    int    iCount=0;       /*分录条数*/
    double dTx_amt=0.00;   /*交易金额*/
    char   sOpn_brno[8]={0}; /*主账号的开户机构*/
    char   sOpn_flag[2]={0};/*记账机构标志 1：记交易机构，2：记主账号开户机构*/   
    
    struct mdm_ac_rel_c  sMdm_ac_rel;
    struct dd_mst_c      sDd_mst;
    struct in_mst_c      sIn_mst;
    struct com_item_c    sCom_item;
    struct chnl_single_cnaps_c stChnlCnaps;
    struct chnl_single_acct_c stChnlAcct; 
    struct prdt_ac_id_c  sPrdt_ac_id;

    memset(&sMdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
    memset(&sDd_mst,0x00,sizeof(struct dd_mst_c));
    memset(&sIn_mst,0x00,sizeof(struct in_mst_c));
    memset(&sCom_item,0x00,sizeof(struct com_item_c));
    memset(&stChnlCnaps,0x00,sizeof(struct chnl_single_cnaps_c));
    memset(&stChnlAcct,0x00,sizeof(struct chnl_single_acct_c));  /*zhengyq 20140811 add to sxps*/
    memset(&sPrdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));
    memset(cCount,0x00,sizeof(cCount));
    memset(cFd95,0x00,sizeof(cFd95));
    memset(cFd81,0x00,sizeof(cFd81));
    memset(cAc_no,0x00,sizeof(cAc_no));
    memset(cTx_amt,0x00,sizeof(cTx_amt));
    memset(cTx_brf,0x00,sizeof(cTx_brf));
    memset(cDc_ind,0x00,sizeof(cDc_ind));
    memset(cCt_ind,0x00,sizeof(cCt_ind));

    pub_base_sysinit();
    
    get_fd_data("0810",cFd81);
    get_fd_data("0950",cFd95);
    if( strlen(cFd95)==0 || memcmp(cFd95,"END",3) == 0 )
    {
        vtcp_log("[%s][%d],没有第二分录直接返回成功!!!",__FILE__,__LINE__);
        goto OkExit;
    }
    /*得到分录条数*/
    memcpy(cCount,cFd95,2); 
    iCount=atoi(cCount);
	/*add by lw 20120719*/
	if(iCount == 0)
	{
		vtcp_log("[%s][%d],没有第二分录直接返回成功!!!",__FILE__,__LINE__);
		goto OkExit;
	}
	vtcp_log("[%s][%d]分录条数=%d",__FILE__,__LINE__,iCount);
    /*得到现转标志*/
    memcpy(cCt_ind,cFd95+2,1);
    for( i=0; i < iCount; i++ )
    {
        memset(cAc_no,0x00,sizeof(cAc_no));
        memset(cTx_amt,0x00,sizeof(cTx_amt));
        memset(cDc_ind,0x00,sizeof(cDc_ind));
        memset(sOpn_brno,0x00,sizeof(sOpn_brno));
        
        memcpy(sOpn_brno,cFd95 + 3 + 46 * i,5);    /*开户机构*/
        memcpy(cAc_no,cFd95 + 8 + 46 * i,24);      /*账号或科目号*/
        memcpy(cDc_ind,cFd95 + 32 + 46 * i,1);     /*借贷标志*/
        memcpy(cTx_amt,cFd95 + 33 + 46 * i,16);    /*交易金额*/
        
        dTx_amt=atof(cTx_amt) / 100;               /*交易金额转为浮点*/
        
        if(cDc_ind[0] == '1')
        {
            dTx_amt= -1 * dTx_amt;
        }
        dSum += dTx_amt;
    }
    if( pub_base_CompDblVal(dSum,0.00) !=0 )
    {
        sprintf(acErrMsg, "传过来的借贷数据不平衡！");
        set_zd_data(DC_GET_MSG,acErrMsg);
        WRITEMSG
        goto ErrExit;
    }
    
    i=0;
    for( i=0; i < iCount; i++ )
    {
        memset(cAc_no,0x00,sizeof(cAc_no));
        memset(cTx_amt,0x00,sizeof(cTx_amt));
        memset(cDc_ind,0x00,sizeof(cDc_ind));
        memset(sOpn_brno,0x00,sizeof(sOpn_brno));
        
        memcpy(sOpn_brno,cFd95 + 3 + 46 * i,5);    /*开户机构或交易机构*/
        memcpy(cAc_no,cFd95 + 8 + 46 * i,24);      /*账号或科目号*/
        memcpy(cDc_ind,cFd95 + 32 + 46 * i,1);     /*借贷标志*/
        memcpy(cTx_amt,cFd95 + 33 + 46 * i,16);    /*交易金额*/
        
        dTx_amt=atof(cTx_amt) / 100;               /*交易金额转为浮点*/
        vtcp_log("%s,%d,ac_no=[%s]账号或科目号!",__FILE__,__LINE__,cAc_no);
        vtcp_log("%s,%d,cDc_ind=[%s]借贷标志!",__FILE__,__LINE__,cDc_ind);
        if(memcmp(g_pub_tx.tx_code,"6235",4)==0 || memcmp(g_pub_tx.tx_code,"7023",4)==0 || memcmp(g_pub_tx.tx_code,"6254",4)==0 )
        {   /*冲正类记负数*/
        	dTx_amt= -1 * dTx_amt;
        	get_zd_data("0100",stChnlCnaps.bbmk);
		if (memcmp(stChnlCnaps.bbmk,"sxps",4) == 0)
		{     
			 /*zhengyq 20140811 add to sxps  begin*/
			/**add by lw 20120719 检查是否有原交易,不用检查交易状态因为在6235、7023里面已经检查过**/
			get_zd_long("0450",&stChnlAcct.pt_date);
			long lPt_old_trace=0;
			get_zd_long("0780",&lPt_old_trace);
			sprintf(stChnlAcct.pt_trace_no,"%ld",lPt_old_trace);
			sprintf(stChnlAcct.bbmk,"%s",stChnlCnaps.bbmk);
			iRet=Chnl_single_acct_Sel(g_pub_tx.reply,&stChnlAcct,"pt_date=%ld and pt_trace_no='%s' and bbmk='%s'",stChnlAcct.pt_date,stChnlAcct.pt_trace_no,stChnlAcct.bbmk);
			if(iRet)
			{
				strcpy(g_pub_tx.reply, "D103");
				sprintf(acErrMsg,"查询原交易出错");
				WRITEMSG
				goto ErrExit;
			}
			vtcp_log("[%s][%d],g_pub_tx.tx_code[%s] stChnlAcct.sts=[%s]",__FILE__,__LINE__,g_pub_tx.tx_code,stChnlAcct.sts);
			
		}
		else
		{
			/**add by lw 20120719 检查是否有原交易,不用检查交易状态因为在6235、7023里面已经检查过**/
			get_zd_long("0450",&stChnlCnaps.pt_date);
			long lPt_old_trace=0;
			get_zd_long("0780",&lPt_old_trace);
			sprintf(stChnlCnaps.pt_trace_no,"%ld",lPt_old_trace);
			iRet=Chnl_single_cnaps_Sel(g_pub_tx.reply,&stChnlCnaps,"pt_date=%ld and pt_trace_no='%s' and bbmk='%s'",stChnlCnaps.pt_date,stChnlCnaps.pt_trace_no,stChnlCnaps.bbmk);
			if(iRet)
			{
				strcpy(g_pub_tx.reply, "D103");
				sprintf(acErrMsg,"查询原交易出错");
				WRITEMSG
				goto ErrExit;
			}
			vtcp_log("[%s][%d],g_pub_tx.tx_code[%s] stChnlCnaps.sts=[%s]",__FILE__,__LINE__,g_pub_tx.tx_code,stChnlCnaps.sts);
		}
        }
        zip_space(cAc_no);
        if( strlen(cAc_no) >= 13 )    /*账号*/
        {
             memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
             iRet= Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel," ac_no= '%s'",cAc_no);
             if( iRet )
             {
                  vtcp_log("%s,%d,ac_no=[%s]查找错误!",__FILE__,__LINE__,cAc_no);
                  strcpy(g_pub_tx.reply,"P102");
                  goto ErrExit;
             }
             memset(&sPrdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));
             iRet = Prdt_ac_id_Sel(g_pub_tx.reply,&sPrdt_ac_id,"ac_id=%ld ",g_mdm_ac_rel.ac_id);
             if (iRet)
             {
                  vtcp_log("%s,%d,ac_no[%s]查询prdt_ac_rel错误!",__FILE__,__LINE__,cAc_no);
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
                  strcpy(g_pub_tx.ac_no,cAc_no);
                  g_pub_tx.tx_amt1=dTx_amt;
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
                  strcpy(g_pub_tx.brf,cFd81);
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
                  else if( cDc_ind[0] == '1')
                  {/*借方记账*/ 
                      set_zd_data("102J", g_pub_tx.cur_no);
                      set_zd_data("102K", cCt_ind);
                      set_zd_double("102F", g_pub_tx.tx_amt1);
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
                  vtcp_log("%s,%d,cDc_ind=[%s]跟踪--借贷标志!",__FILE__,__LINE__,cDc_ind);
                  memset(&sIn_mst,0x00,sizeof(struct in_mst_c));
                  iRet = In_mst_Sel(g_pub_tx.reply,&sIn_mst,"ac_id=%ld",g_mdm_ac_rel.ac_id);
                  if (iRet)
                  {
                    vtcp_log("%s,%d,ac_no[%s]查找错误!",__FILE__,__LINE__,cAc_no);
                    strcpy(g_pub_tx.reply,"P102");
                    goto ErrExit;
                  }
                  if(sIn_mst.sts[0] != '1')
                  {
                    vtcp_log("%s,%d,ac_no[%s]该账户不正常!",__FILE__,__LINE__,cAc_no);
                    strcpy(g_pub_tx.reply,"O085");
                    goto ErrExit;
                  }
                  g_pub_tx.ac_id=sIn_mst.ac_id;
                  g_pub_tx.ac_seqn=sIn_mst.ac_seqn;
                  g_pub_tx.ac_id_type[0]='9';
                  strncpy(g_pub_tx.opn_br_no, g_mdm_ac_rel.opn_br_no ,sizeof(g_pub_tx.opn_br_no)-1);
                  strcpy(g_pub_tx.ac_no,cAc_no);
                  g_pub_tx.tx_amt1=dTx_amt;
                  strcpy(g_pub_tx.cur_no,"01");    /* 币种 */
                  strcpy(g_pub_tx.ct_ind,cCt_ind);
                  vtcp_log("%s,%d,cDc_ind=[%s]跟踪2--借贷标志!",__FILE__,__LINE__,cDc_ind);
                  if( cDc_ind[0] == '2')
                  {
                      strcpy(g_pub_tx.add_ind,"1");/*增减(借贷):0减(借) 1加(贷)*/
                  }
                  else if( cDc_ind[0] == '1' )
                  {
                      strcpy(g_pub_tx.add_ind,"0");/*增减(借贷):0减(借) 1加(贷)*/
                  }
                  else
                  {
                      sprintf(acErrMsg,"借贷标志错误!");
                      WRITEMSG
                      strcpy(g_pub_tx.reply,"P404");
                      goto ErrExit;
                  }
                  vtcp_log("%s,%d,cDc_ind=[%s]跟踪3--借贷标志!",__FILE__,__LINE__,cDc_ind);
                  vtcp_log("%s,%d,g_pub_tx.add_ind=[%s]跟踪--借贷标志!",__FILE__,__LINE__,g_pub_tx.add_ind);
                  strcpy(g_pub_tx.prt_ind,"0");/*是否打折标志*/
                  strcpy(g_pub_tx.hst_ind,"1");/**日间入明细**/
                  strcpy(g_pub_tx.ac_wrk_ind,"0000000");
                  g_pub_tx.svc_ind=9001;/* 内部帐存取 */
                  strcpy(g_pub_tx.ac_get_ind,"000");/**存取款***/
                  strcpy(g_pub_tx.brf,cFd81);
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
        else if( strlen(cAc_no) < 13) /*科目*/
        {
            /*如果为科目记账，取平台传来的开户机构*/
            memcpy(g_pub_tx.opn_br_no,sOpn_brno,sizeof(g_pub_tx.opn_br_no)-1);
            
            if( memcmp(cAc_no,"100101",5)==0)/*现金记账*/
            {
                  if( cDc_ind[0] == '1')
                  {
                      g_pub_tx.svc_ind=0;
                      g_pub_tx.ac_id=0; 
                      strcpy(g_pub_tx.cur_no,"01");
                      g_pub_tx.tx_amt1=dTx_amt;
                      strcpy(g_pub_tx.brf,cFd81);
                      strcpy(g_pub_tx.name,cAc_no);         /**对方账号**/
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
                      g_pub_tx.tx_amt1=dTx_amt;
                      strcpy(g_pub_tx.brf,cFd81);
                      strcpy(g_pub_tx.name,cAc_no);         /**对方账号**/
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
                  iRet = Com_item_Sel(g_pub_tx.reply,&sCom_item,"acc_no = '%s'",cAc_no);
                  if(iRet)
                  {
                      sprintf(acErrMsg,"查询科目失败!!!");
                      WRITEMSG
                      strcpy(g_pub_tx.reply, "B172");
                      goto ErrExit;
                  }
				strncpy(g_pub_tx.opn_br_no,sOpn_brno,sizeof(g_pub_tx.opn_br_no)-1);
				vtcp_log("[%s][%d],g_pub_tx.opn_br_no[%s] sOpn_brno=[%s]",__FILE__,__LINE__,g_pub_tx.opn_br_no,sOpn_brno);
                  strcpy(g_pub_tx.ac_no,cAc_no);
                  g_pub_tx.ac_seqn=0;
                  g_pub_tx.tx_amt1=dTx_amt;
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
                  strcpy(g_pub_tx.brf,cFd81);
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
                  else if( cDc_ind[0] == '1')
                  {/*借方记账*/ 
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




