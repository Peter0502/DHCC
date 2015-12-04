/*************************************************
* 文 件 名:  spJ620.c  对应老系统 V95.pc
* 功能描述： 大额支付明细对帐
*             
* 作    者:  ChenMing 
* 完成日期： 2006年8月26 日
*
* 修改记录： 
* 日    期:
* 修 改 人:
* 修改内容:
*************************************************/
#define EXTERN
#include <string.h>
#include <stdio.h>
#include "hv_define.h"
#include "hv_pub.h"
#include "hvnet.h"
#include "hvpack.h"
#include "hv_chkagemr_c.h"
#include "hv_zf_c.h"
#include "hv_zfhtr_c.h"
#include "find_debug.h"

#include "com_parm_c.h"
#include "mo_merge_ac_c.h"
#include "prdt_ac_id_c.h"
#include "hv_chkerror_c.h"
#include "com_branch_c.h"
#include "hv_poinfo_c.h"
#include "hv_chkmr_c.h"



int spJ620()
{

    long iChecked = 0; 
  int ret = 0;
    pub_base_sysinit(); 

  vtcp_log("\n[%s][%d]  ==========   spJ620 Started ! ==========\n",__FILE__,__LINE__);  

    get_zd_long("0520",&iChecked);
  vtcp_log("[%s][%d]  iChecked=[%d]",__FILE__,__LINE__,iChecked);  
    
    if ( iChecked == 0 )  /* 只需要在第一次进入时执行 spJ620_Check*/ 
    {  
          ret = spJ620_Check();
      if(ret == 999) /* 检查未通过 不报错!*/
      {
           strcpy( g_pub_tx.reply, "0000" );
             goto OkExit;    
      }              
        if(ret)   
        {
                    
              goto ErrExit;    
        }
      vtcp_log(" \n [%s][%d] ====== spJ620_Check() Successed ! ======\n ",__FILE__,__LINE__);    
        iChecked = 1;      
          set_zd_long("0520",iChecked);       /* 已检查!! */
    }

  if( spJ620_Process() )  
    {
    
          goto ErrExit;    
    }      
  vtcp_log("\n[%s][%d]  ======spJ620_Process() Successed !   ======  \n",__FILE__,__LINE__);    

  double dTemp=0.0;
  get_zd_double("0390",&dTemp);
  vtcp_log("\n[%s][%d]  dTemp======[%lf]  \n",__FILE__,__LINE__,dTemp);    
  dTemp=0.0;
  get_zd_double("0420",&dTemp);
  vtcp_log("\n[%s][%d]  dTemp======[%lf]  \n",__FILE__,__LINE__,dTemp);      


OkExit:
/*set_zd_data("0690","0");    
    set_zd_data("0510","0");
    set_zd_data("0710","1");        
    strcpy( g_pub_tx.reply, "0000" );*/    
    sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
ErrExit:
/*set_zd_data("0690","1");        
    set_zd_data("0510","1");
    set_zd_data("0710","1");    */        
        if(memcmp(g_pub_tx.reply,"0000",4)==0)
                MEMCPY_DEBUG(g_pub_tx.reply,"T063",4);
    sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
    
}


int spJ620_Check()
{
    int ret;
    int iErrcnt=0;    
    char cCmd[512];
  char cStat;  
    char cQs_br_no[BRNO_LEN+1];
    long  iResult=0;
    
    memset(cCmd,0,sizeof(cCmd));
    memset(cQs_br_no,0,sizeof(cQs_br_no));    

  set_zd_long("0510",0);
      
    /*****************************
    * 判断交易机构是否是清算中心 *
    *****************************/
    if( GetBr_noByOr_br_no(HV_QS_BR_NO, cQs_br_no))
    {
          sprintf(acErrMsg,"根据 支付系统行号 查找 机构号码出错! " );
      WRITEMSG
          return -1;
    }
    
    vtcp_log("[%s][%d]=======  g_pub_tx.tx_br_no = [%s] 清算中心 cQs_br_no=[%s]",__FILE__,__LINE__,g_pub_tx.tx_br_no,cQs_br_no);    
    /***  判断交易机构是否是清算中心   Remed For Test ! ***/
    if(memcmp(g_pub_tx.tx_br_no,cQs_br_no,strlen(g_pub_tx.tx_br_no)-1))
    {
          sprintf(acErrMsg,"非清算中心不能做此交易!%s",g_pub_tx.tx_br_no);
      WRITEMSG
      iResult = 5;
      set_zd_long("0510",5);
          return 999;   
    } 
    
    /*****************************************************************
    *  判断大额状态： 是否可以对帐                                   *
    *****************************************************************/
    cStat = hv_getstat();
    vtcp_log("[%s][%d]=======cStat = [%c]",__FILE__,__LINE__,cStat);
    /* cStat = HV_STAT_CHECKERR;  For Test */
    if(cStat != HV_STAT_CHECKERR)
    {
          sprintf(acErrMsg,"系统当前状态不是核对明细帐状态! [%c]",cStat);
      WRITEMSG
      iResult = 6;
      set_zd_long("0510",6);
          return 999;   
    }
    vtcp_log("[%s][%d]=======  执行到这里了  可以对帐 ========",__FILE__,__LINE__);
    
    /*****************************************************************
    *  判断是否存在复核但未取消状态、复核但未发送的数据              *
    *****************************************************************/
    /*  Remed For Test ! ***/
    ret = iCheckHvForDateOver((char *)NULL,g_pub_tx.tx_date,&iErrcnt);
    if( ret )
    {
          sprintf(acErrMsg," 查找大额支付表错误! [%d]",ret);
      WRITEMSG
          return -1;
    }
    vtcp_log("[%s][%d]=======   iErrcnt=[%d]   ========",__FILE__,__LINE__,iErrcnt);
    if( iErrcnt != 0 )
    {
          sprintf(acErrMsg,"存在复核但未发送的数据[%d]",iErrcnt);
      WRITEMSG
      iResult = 7;
      set_zd_long("0510",7);
          return 999;   
    }
    vtcp_log("[%s][%d] =======执行到这里了,不存在复核但未发送的数据 ! =======",__FILE__,__LINE__);
    
    /*****************************************************************
    * 判断对帐明细是否下载  以及和总计核对报文是否对总               *
    *****************************************************************/    

    if( iCheckDownloadFile(&iResult) )
    {   
            vtcp_log("[%s][%d] ===iResult====[%d]",iResult);
          if( iResult==8)
          {
                sprintf(acErrMsg,"没有下载对帐明细 !");
          }
          else
          {
                  sprintf(acErrMsg,"对帐明细和总计核对报文不对总 !");
          }
          
      WRITEMSG
      set_zd_long("0510",iResult);
          return 999;   
    }    
    vtcp_log("[%s][%d]=======   执行到这里了   对帐明细已下载========",__FILE__,__LINE__);
    
    return 0;
}

/* 核对明细  老系统是调用 /Appl/batch/bin/checkCcpc */
int spJ620_Process()
{
      int ret=0;
    int iHandled =0;   /* 是否处理了一条记录 0=否    1=是 */     
      
      vtcp_log("[%s][%d] =======  spJ620_Process Start !!! ========",__FILE__,__LINE__);    
      
      /*****************************************************************
      * 1  检查 dhcc有 但cpcc没有 ( 发生额有出入的情况 不考虑)         *
      *****************************************************************/
      vtcp_log("[%s][%d]开始检查 dhcc有 但cpcc没有的数据 ",__FILE__,__LINE__);
      vtcp_log("[%s][%d]开始检查 dhcc有 但cpcc没有的数据 ",__FILE__,__LINE__);
                  
        ret = iCheckHv_zfTable(&iHandled);  /*根据hv_chkagemr核对Hv_zf*/
        if(ret)
        {
          vtcp_log("[%s][%d]======= iCheckHv_zfTable() 出错! ========",__FILE__,__LINE__);    
             sprintf(acErrMsg,"大额支付明细对帐出错[%d]",ret);
             WRITEMSG
             return(ret);
        }
      vtcp_log("[%s][%d]======= After iCheckHv_zfTable()!!========",__FILE__,__LINE__);    
      vtcp_log("[%s][%d] 是否处理了一条记录    iHandled=[%d]",__FILE__,__LINE__,iHandled);
    if( iHandled == 1)   /* 是否处理了一条记录  1=是 */    
    {
          vtcp_log("[%s][%d]  处理了一条记录，则不往下走了，跳转到DELZ 去走帐 !!!!!    iHandled=[%d]",__FILE__,__LINE__,iHandled);
          strcpy( g_pub_tx.reply, "0010" );        
          return 0;
    }  

      vtcp_log("[%s][%d] 检查 dhcc有 但cpcc没有的数据 结束!",__FILE__,__LINE__);
      vtcp_log("[%s][%d] 检查 dhcc有 但cpcc没有的数据 结束!",__FILE__,__LINE__);
          
      /*****************************************************************
      * 2  检查ccpc有的但dhcc没有的 ( 发生额有出入的情况 不考虑)       *
      *****************************************************************/                
      vtcp_log("[%s][%d]开始检查ccpc有的但dhcc没有的的数据 ",__FILE__,__LINE__);
      vtcp_log("[%s][%d]开始检查ccpc有的但dhcc没有的的数据 ",__FILE__,__LINE__);
        
        ret = iCheckHv_chkagemrTable(&iHandled); /* 根据Hv_zf核对hv_chkagemr */
        vtcp_log("ret========[%d]",ret);
        if(ret)
        {
          vtcp_log("[%s][%d]======= iCheckHv_chkagemrTable() 出错! ========",__FILE__,__LINE__);    
             sprintf(acErrMsg,"大额支付明细对帐出错[%d]",ret);
             WRITEMSG
             return(ret);
        }
        
      vtcp_log("[%s][%d]======= After iCheckHv_chkagemrTable()!!========",__FILE__,__LINE__);    
      vtcp_log("[%s][%d] 是否处理了一条记录    iHandled=[%d]",__FILE__,__LINE__,iHandled);
    if( iHandled == 1)   /* 是否处理了一条记录  1=是 */    
    {
          vtcp_log("[%s][%d]  处理了一条记录，则不往下走了，跳转到DELZ 去走帐 !!!!!    iHandled=[%d]",__FILE__,__LINE__,iHandled);
          strcpy( g_pub_tx.reply, "0010" );        
          return 0;
    }
       vtcp_log("[%s][%d] 检查ccpc有的但dhcc没有的的数据 结束! ",__FILE__,__LINE__);
      vtcp_log("[%s][%d] 检查ccpc有的但dhcc没有的的数据 结束! ",__FILE__,__LINE__);
   
      strcpy( g_pub_tx.reply, "0000" );         /* 到这里了，表示都处理完了*/    
    set_zd_long("0510",1);                 /* 到这里了，表示对帐成功了*/    

    vtcp_log("[%s][%d] ========= 都处理完了  ==========  ",__FILE__,__LINE__);
    vtcp_log("[%s][%d] ========= 都处理完了  ==========  ",__FILE__,__LINE__);

      return 0;
}     
    
/*** 判断对帐明细是否下载  以及和总计核对报文是否对总 ***/
int iCheckDownloadFile(int* iResult)
{
      int ret,ret2;
      struct hv_chkagemr_c hv_chkagemr;
    struct hv_chkmr_c hv_chkmr;    
    int    iFlag=0;
    int    iReceiveCnt=0;
    double dReceiveAmt=0.0;
    int    iSendCnt=0;
    double dSendAmt=0;

    memset(&hv_chkmr,0,sizeof(struct hv_chkmr_c) );    
       g_reply_int = Hv_chkmr_Sel(g_pub_tx.reply,&hv_chkmr," 1=1 ");
       if( g_reply_int==100 )
       {
           sprintf(acErrMsg,"大额业务核对主表无记录");
           WRITEMSG
           return -1;
       }
       else if( g_reply_int )
       {
           sprintf(acErrMsg,"查询大额业务核对主表数据库操作失败[%d]",g_reply_int);
           WRITEMSG
           return -1;
       }     
 
    /* 1 查核对明细表中 人行来/往帐总笔数, 来/往帐总金额  */   
    ret = Hv_chkagemr_Dec_Sel(g_pub_tx.reply,"tx_date = %ld and cmtno <> '232' ",g_pub_tx.tx_date);
    if( ret )
      {
          sprintf(acErrMsg,"大额来帐数据库操作失败[%d]",ret);
          WRITEMSG
          return -1;
      }   
  
    while(1)
    {
        memset(&hv_chkagemr,0,sizeof(struct hv_chkagemr_c) );       
          ret = Hv_chkagemr_Fet_Sel( &hv_chkagemr, g_pub_tx.reply );
          vtcp_log("[%s][%d]=   ret====[%d]",__FILE__,__LINE__,ret);  
          if (ret == 100)
          {
                if(iFlag == 0)  /* 一条记录也没有 */
                {
                         Hv_chkagemr_Clo_Sel();
                 vtcp_log("[%s][%d] 得到 核对主表中往账总笔数 CCPC.send_cnt=[%d]  ",__FILE__,__LINE__,hv_chkmr.send_cnt);    
                 vtcp_log("[%s][%d] 得到 核对主表中来帐总笔数 CCPC.recevice_cnt=[%d] ",__FILE__,__LINE__,hv_chkmr.recevice_cnt);    
                 if(hv_chkmr.send_cnt +    hv_chkmr.recevice_cnt > 0 ) 
                 {
                        /* 今天有业务，说明是689还没收到!!  */
                         sprintf(acErrMsg,"今天明细未下载成功 !");
                     WRITEMSG
                     *iResult = 8;
                         return -1;                          
                 }   
                 vtcp_log("[%s][%d]=  今天一笔业务都没有 !!! ",__FILE__,__LINE__);  
                }
                vtcp_log("[%s][%d]=计算来帐总笔数，来帐总金额 完毕 ",__FILE__,__LINE__);  
                break;                                    
          } 
          else if(ret)
          {
                sprintf(acErrMsg,"数据库操作失败 [%d]",ret);              
                WRITEMSG
                 return -1;
          }
          
          iFlag ++;
          
        ret = memcmp(hv_chkagemr.pay_qs_no, HV_QSBRNO, sizeof(hv_chkagemr.pay_qs_no)-1);
        ret2 = memcmp(hv_chkagemr.cashqsactno, HV_QSBRNO, sizeof(hv_chkagemr.cashqsactno)-1);
        if(ret && ret2)
        {
            vtcp_log("[%s][%d] 接收清算行和发起清算行都不为本行清算行!!! ret=[%d]ret2=[%d]",__FILE__,__LINE__,ret,ret2);
            sprintf(acErrMsg,"接收清算行和发起清算行都不为本行清算行!! ret=[%d] ret2=[%d]",ret,ret2);
            WRITEMSG
              return -1;    
        }         
        if(ret==0 && ret2==0)
        {
            vtcp_log("[%s][%d] 接收清算行和发起清算行都是本行清算行!!! ret=[%d]ret2=[%d]",__FILE__,__LINE__,ret,ret2);
            sprintf(acErrMsg,"接收清算行和发起清算行都是本行清算行!! ret=[%d] ret2=[%d]",ret,ret2);
            WRITEMSG
              return -1;    
        }      
        if(ret == 0)  /* 往帐 */
        {                    
              iSendCnt = iSendCnt + 1;
              dSendAmt = dSendAmt + hv_chkagemr.tx_amt;    
           }
        else    /* 来帐 */
        {
              iReceiveCnt = iReceiveCnt + 1;
              dReceiveAmt = dReceiveAmt + hv_chkagemr.tx_amt;
        }    
    }    
      Hv_chkagemr_Clo_Sel();

    vtcp_log("[%s][%d] 得到 核对明细表中 往账总笔数 iSendCnt=[%d] ",__FILE__,__LINE__,iSendCnt);    
    vtcp_log("[%s][%d] 得到 核对明细表中 往账总金额 dSendAmt=[%lf] ",__FILE__,__LINE__,dSendAmt);    
    vtcp_log("[%s][%d] 得到 核对明细表中 来帐总笔数 iReceiveCnt=[%d] ",__FILE__,__LINE__,iReceiveCnt);    
    vtcp_log("[%s][%d] 得到 核对明细表中 来帐总金额 dReceiveAmt=[%lf] ",__FILE__,__LINE__,dReceiveAmt);    

    /* 2 查核对主表中 人行来/往帐总笔数, 来/往帐总金额  */
    /*   提前到上面了    2006-11-4 23:22
    memset(&hv_chkmr,0,sizeof(struct hv_chkmr_c) );    
       g_reply_int = Hv_chkmr_Sel(g_pub_tx.reply,&hv_chkmr," 1=1 ");
       if( g_reply_int==100 )
       {
           sprintf(acErrMsg,"大额业务核对主表无记录");
           WRITEMSG
           return -1;
       }
       else if( g_reply_int )
       {
           sprintf(acErrMsg,"查询大额业务核对主表数据库操作失败[%d]",g_reply_int);
           WRITEMSG
           return -1;
       } */    
    vtcp_log("[%s][%d] 得到 核对主表中往账总笔数 CCPC.send_cnt=[%d]  ",__FILE__,__LINE__,hv_chkmr.send_cnt);    
    vtcp_log("[%s][%d] 得到 核对主表中往账总金额 CCPC.send_amt=[%lf]",__FILE__,__LINE__,hv_chkmr.send_amt);    
    vtcp_log("[%s][%d] 得到 核对主表中来帐总笔数 CCPC.recevice_cnt=[%d] ",__FILE__,__LINE__,hv_chkmr.recevice_cnt);    
    vtcp_log("[%s][%d] 得到 核对主表中来帐总金额 CCPC.recevice_amt=[%lf] ",__FILE__,__LINE__,hv_chkmr.recevice_amt);    

    /* 3 判断 对帐明细 和 总计核对报文 是否对总  */
    if( (hv_chkmr.send_cnt == iSendCnt) && (hv_chkmr.recevice_cnt == iReceiveCnt) && 
          (hv_chkmr.send_amt-dSendAmt > -0.0001 || hv_chkmr.send_amt-dSendAmt <0.0001)  && 
        (hv_chkmr.recevice_amt-dReceiveAmt > -0.0001 || hv_chkmr.recevice_amt-dReceiveAmt < 0.0001) )
    {
          vtcp_log("[%s][%d] ==== 对帐明细已下载  和总计核对报文相等!! =====",__FILE__,__LINE__);    
              return 0;
    }
    else
    {
               sprintf(acErrMsg,"大额业务核对主表无记录");
               WRITEMSG
               *iResult = 9;
              return -1;
    }
       return 0; 
}
/*******************************************************************************
* 函数名称:iCheckHvForDateOver                                                 *
* 描    述: 检查大额支付业务主表中是否存在已经复核记帐但未发送或等待清算       *
*          确认或排队的帐务                                                    *
* 参    数:chk_br_no , iErrCnt                                                 *
* 类    型:char *    , int *                                                   *
* 返 回 值:int                                                                 *
* -----------------------------------------------------------------------------*
* 输入 :要检查的机构号,为空表示全部                                            *
* 输出 :返回值 == 0, iErrCnt 返回存在的笔数,返回值 !=0 表示数据库错误          *
*******************************************************************************/
int iCheckHvForDateOver(char* chk_br_no,int tx_date,int* iErrCnt)
{
    int iCnt = 0;
    char cBr_no[8];
    struct hv_zf_c hv_zf;
    int ret;
  vtcp_log("[%s][%d]======= Begin iCheckHvForDateOver()!!========",__FILE__,__LINE__);    
    
    memset(&hv_zf, 0 , sizeof(struct hv_zf_c));    
    memset(cBr_no, 0 , sizeof(cBr_no));    
    
    MEMCPY_DEBUG(cBr_no,chk_br_no,sizeof(cBr_no)-1);
    
   if(strlen(chk_br_no) == 0)
   {
         ret = Hv_zf_Dec_Sel(g_pub_tx.reply," tx_date=%d and hv_sts='0'  ",tx_date);
       if( ret )
         {
              sprintf(acErrMsg,"数据库操作错误!");
              WRITEMSG
                     return -1;                 
         }      
      
   }
   else
   {
         ret = Hv_zf_Dec_Sel(g_pub_tx.reply," tx_date=%d and hv_sts='0' and br_no = '%s' ",tx_date, chk_br_no);
       if( ret )
         {
              sprintf(acErrMsg,"数据库操作错误!");
              WRITEMSG
                     return -1;                 
         }  
       
   }
             
    while(1)
    {
         ret = Hv_zf_Fet_Sel(&hv_zf,g_pub_tx.reply);
         if (ret == 100)   break;
         else if( ret )
         {
         sprintf(acErrMsg,"数据库操作错误!");
         WRITEMSG
                return -1;                 
         }         
         iCnt = iCnt + 1; 
    }
    
    *iErrCnt = iCnt;
  vtcp_log("[%s][%d]== End iCheckHvForDateOver()!!===iCnt===[%d]",__FILE__,__LINE__,iCnt);    
        
    
    Hv_zf_Clo_Sel();    
    return 0;    
}





/*******************************************************************************
* 函数名称:iCheckHv_zfTable                                                    *
* 描    述: 根据hv_chkagemr核对Hv_zf
            检查 dhcc有 但cpcc没有 的情况                                      *
            我有CCPC没有的情况,冲毁我方的帐务                                  *
* 参    数:                                                                    *
* 类    型:                                                                    *
* 返 回 值:int                                                                 *
* -----------------------------------------------------------------------------*
* 输入 :                                                                       *
* 输出 :                                                                       *
*******************************************************************************/
int iCheckHv_zfTable(int* iHandled)
{
      int ret;
      struct hv_zf_c hv_zf;
      struct hv_chkagemr_c hv_chkagemr;
      char cAcno110[20];
      int iCmtno=0;
                  
      vtcp_log("[%s][%d] ***** iCheckHv_zfTable() 开始 !  ***** ",__FILE__,__LINE__);
      vtcp_log("[%s][%d] ***** iCheckHv_zfTable() 开始 !  ***** ",__FILE__,__LINE__);
      /* 我方不记帐的情况不需要考虑， 所以只查出记帐的情况           */
      /* iCheckHv_chkagemrTable()中插入的记录 checkflag= 'D',不再处理    */
      /* 增加了一个复核状态 '0' 也走帐了   2006-10-30 23:07
      ret = Hv_zf_Dec_Upd(g_pub_tx.reply, 
      "tx_date=%d   and ( hv_sts not in ('5','6','A','C','8','D','L','K') ) ", g_pub_tx.tx_date); */
      ret = Hv_zf_Dec_Upd(g_pub_tx.reply, 
      "tx_date=%d   and ( hv_sts  in ('0','1','2','3','4','7','9','B','F','G','H','I','J') ) and (checkflag is null) and cmtno!='802' and cmtno!='812' and cmtno!='814'  and cmtno !='232' ", g_pub_tx.tx_date);       
      if ( ret )
      {
            sprintf(acErrMsg,"Hv_zf_Dec_Sel() 出错[%d]",ret);
            WRITEMSG
              return ret;
      }          
        
    while(1)
    {
        memset(&hv_zf,0, sizeof(hv_zf));        
        ret = Hv_zf_Fet_Upd(&hv_zf, g_pub_tx.reply);
        if( ret==100 )
        {
            vtcp_log("[%s][%d] 大额支付往帐记录循环处理完毕  ",__FILE__,__LINE__);    
            Hv_zf_Clo_Sel();                      
            break;
        }
        else if(ret)
        {
            vtcp_log("[%s][%d]数据库操作异常,请与科技科联系!",__FILE__,__LINE__);
            sprintf(acErrMsg,"数据库操作异常,请与科技科联系![%d]",ret);
            WRITEMSG
            return ret;
        }
        
        memset(&hv_chkagemr,0, sizeof( hv_chkagemr ) );
        ret = Hv_chkagemr_Dec_Upd(g_pub_tx.reply, " or_br_no='%s' and orderno='%s' and ( chk_flg ='0' or chk_flg is null )", hv_zf.or_br_no,hv_zf.orderno); 
        if(ret)
        {
            vtcp_log("[%s][%d]Hv_chkagemr_Dec_Upd() 出错!",__FILE__,__LINE__);
                sprintf(acErrMsg,"Hv_chkagemr_Dec_Upd() 出错![%d]",ret);
                WRITEMSG
                  return ret;
        } 
        ret = Hv_chkagemr_Fet_Upd(&hv_chkagemr, g_pub_tx.reply);
        if(ret && ret != 100)
        {
            vtcp_log("[%s][%d]数据库操作异常,请与科技科联系!",__FILE__,__LINE__);
                sprintf(acErrMsg,"数据库操作异常,请与科技科联系![%d]",ret);
                WRITEMSG
                  return ret;
        }else if(ret == 100)  /* dhcc有 但cpcc没有 冲毁我方的帐务 */
        {
                  /***********  将该笔往帐状态置为 REJECT_EACTCLS   *********/
                  
                  if(hv_zf.lw_ind[0] == '1')  /* 往帐*/
                  {
                     hv_zf.beg_sts[0] = hv_zf.hv_sts[0]; 
                       hv_zf.hv_sts[0] = '5';        /* 置为往帐拒绝 */
                  }
                  else  /* 来帐*/
                  {
                       hv_zf.hv_sts[0] = 'L';        /* 核对不符 冲帐 */
                  } 
           
            hv_zf.checkflag[0] = 'B';     /* DHCC有，CCPC没有 */       
            ret=Hv_zf_Upd_Upd(hv_zf, g_pub_tx.reply);
            if( ret)
            {
                vtcp_log("[%s][%d] 数据库操作异常,请与科技科联系!  ",__FILE__,__LINE__);    
                    sprintf(acErrMsg,"数据库操作异常,请与科技科联系![%d]",ret);
                    WRITEMSG
                      return -1;
            } 
                 vtcp_log("[%s][%d] ***** 更新往帐状态成功 !  ***** ",__FILE__,__LINE__);
                 vtcp_log("[%s][%d] hv_zf.cmtno=[%s] ",__FILE__,__LINE__,hv_zf.cmtno);

            iCmtno = atoi(hv_zf.cmtno);
            
            /* 汇票类DHCC多帐 需要更新汇票的状态 (CMT 121 122 123 124) */
            if(iCmtno==121 || iCmtno==122 || iCmtno==123 || iCmtno==124 )
            {  
                    vtcp_log("[%s,%d] 汇票类DHCC多帐 需要更新汇票的状态 !",__FILE__,__LINE__);                        
                          ret = iHv_DZ_ChangeHPStat(&hv_zf,iCmtno);
                  if(ret)
                  {
                        vtcp_log("%s,%d 更新汇票信息表错误 ret=[%d]",__FILE__,__LINE__,ret);
                        vtcp_log(acErrMsg,"更新汇票信息表错误");
                        WRITEMSG
                        return ret;
                  }        
                    vtcp_log("[%s,%d] 更新汇票状态成功!  ",__FILE__,__LINE__);        
            }

                  /***********   登记差错表     ***************/                          
            ret = iInsertHv_chkerror(&hv_zf,&hv_chkagemr);
            if(ret)
            {
                vtcp_log("[%s][%d]数据库操作异常,请与科技科联系!",__FILE__,__LINE__);
                    sprintf(acErrMsg,"数据库操作异常,请与科技科联系![%d]",ret);
                    WRITEMSG
                      return ret;
            }  
          
            vtcp_log("[%s][%d] ********************************* ",__FILE__,__LINE__);
            vtcp_log("[%s][%d] 我有CCPC没有的情况,冲毁我方的帐务 ",__FILE__,__LINE__);
            vtcp_log("[%s][%d] 我有CCPC没有的情况,冲毁我方的帐务 ",__FILE__,__LINE__);
            vtcp_log("[%s][%d] ********************************* ",__FILE__,__LINE__);
            /* begin add by LiuHuafeng 2009-5-14 17:01:51 for 阳泉村镇银行 */
            if(strncmp(hv_zf.br_no,HV_CUNZHEN_BR_NO,BRNO_LEN-2))
            {
            /* end by LiuHuafeng 2009-5-14 17:02:02 阳泉村镇银行 村镇银行的不进行账务处理，不设置8583域 */
                /*******  置8583域  冲毁我方的帐务  负金额走帐 *******/  
                if(iCmtno==100 || iCmtno==101 || iCmtno==102 || iCmtno==103 || 
                     iCmtno==105 || iCmtno==108 || iCmtno==122) 
                {  /* 普通类: (CMT 100 101 102 103 105 108 122)*/
                    
                    vtcp_log("[%s][%d] 普通类 负金额走帐 ",__FILE__,__LINE__);
                      
                    ret = iSet8583_For_CommonCMT(&hv_zf,-1);  /*** -1=负金额走帐 ***/
                    if(ret)
                    {
                        vtcp_log("[%s][%d]iSet8583_For_CommonCMT() 出错!",__FILE__,__LINE__);
                        sprintf(acErrMsg,"设置8583域时出错![%d]",ret);
                        WRITEMSG
                        return -1;
                    }  
                
                }
                else  if(iCmtno==121 || iCmtno==123 || iCmtno==124 )
                {  /*  特殊类 (CMT 121 123 124)  */
                
                         vtcp_log("[%s][%d] 特殊类  负金额走帐 ",__FILE__,__LINE__);   /*  tx_amt: 未用金额    jf_amt: 票面金额 */
                      ret = iSet8583_For_SpecialCMT(&hv_zf, hv_chkagemr.p_num, hv_chkagemr.p_date,hv_chkagemr.jf_amt,-1);  /*** -1=负金额走帐 ***/
                      if(ret)
                      {
                        vtcp_log("[%s][%d]iSet8583_For_SpecialCMT() 出错!",__FILE__,__LINE__);
                            sprintf(acErrMsg,"设置8583域时出错![%d]",ret);
                            WRITEMSG
                              return -1;
                      }    
                }
                else
                {
                         sprintf(acErrMsg," 该报文不在这里处理 !! iCmtno[%d]",iCmtno);
                         WRITEMSG
                           return -1;                
                }
            }
            /* 将chk_flg置为CCPC_SAME_BANK，在iCheckHv_chkagemrTable()中就不用处理这些记录了 */          
            hv_chkagemr.chk_flg[0]='B'; /* DHCC有，CCPC没有 */
            ret = Hv_chkagemr_Upd_Upd(hv_chkagemr, g_pub_tx.reply);
            if(ret)
            {
                vtcp_log("[%s][%d]数据库操作异常,请与科技科联系!",__FILE__,__LINE__);
                sprintf(acErrMsg,"数据库操作异常,请与科技科联系![%d]",ret);
                WRITEMSG
                return ret;
            }
            
            Hv_chkagemr_Clo_Upd();
            vtcp_log("[%s][%d] ***** iCheckHv_zfTable() 成功 !  ***** ",__FILE__,__LINE__);
            /* begin add by LiuHuafeng 2009-5-14 17:01:51 for 阳泉村镇银行 */
            if(strncmp(hv_zf.br_no,HV_CUNZHEN_BR_NO,BRNO_LEN-2))
            {
            /* end by LiuHuafeng 2009-5-14 17:02:02 阳泉村镇银行 村镇银行的不进行账务处理，
             *不设置8583域
             */
            /* end by LiuHuafeng 2009-5-14 17:03:28 */
                Hv_zf_Clo_Upd();
                *iHandled = 1;   /* 处理了一条记录  !! */
                return 0;
            /* begin add by LiuHuafeng 2009-5-14 17:04:06 for 阳泉村镇银行 */
            }
            else
            {
                vtcp_log("%s %d 村镇银行多，商行只做标识，不处理 ",__FILE__,__LINE__);
                *iHandled = 0;
                continue;
            }
            /* end by LiuHuafeng 2009-5-14 17:04:29 */
        }

        /* 老刘说不处理双方入帐金额不相符合的情况  */                        
        vtcp_log("[%s][%d] 我方有但CCPC也有的情况 ",__FILE__,__LINE__);
        vtcp_log("[%s][%d] hv_zf.tx_amt===[%lf] ",__FILE__,__LINE__,hv_zf.tx_amt);
        vtcp_log("[%s][%d] hv_chkagemr.tx_amt===[%lf] ",__FILE__,__LINE__,hv_chkagemr.tx_amt);

        hv_zf.checkflag[0] = 'A';    /* DHCC和CCPC相同 */
        ret = Hv_zf_Upd_Upd(hv_zf, g_pub_tx.reply);
        if( ret)
        {
            vtcp_log("[%s][%d] 数据库操作异常,请与科技科联系!  ",__FILE__,__LINE__);    
            sprintf(acErrMsg,"数据库操作异常,请与科技科联系![%d]",ret);
            WRITEMSG
              return -1;
        }
        vtcp_log("[%s][%d] ***** 更新往帐状态成功 !  ***** ",__FILE__,__LINE__);
          
        /* 将chk_flg置为CCPC_SAME_BANK，在iCheckHv_chkagemrTable()中就不用处理这些记录了 */          
        hv_chkagemr.chk_flg[0]= 'A';  /* CCPC_SAME_BANK DHCC和CCPC相同 */
        ret = Hv_chkagemr_Upd_Upd(hv_chkagemr, g_pub_tx.reply);
        if(ret)
        {
            vtcp_log("[%s][%d]数据库操作异常,请与科技科联系!",__FILE__,__LINE__);
            sprintf(acErrMsg,"数据库操作异常,请与科技科联系![%d]",ret);
            WRITEMSG
              return ret;
        } 
        vtcp_log("[%s][%d] *****这一笔DHCC和CCPC都有，继续循环往下找 !  ***** ",__FILE__,__LINE__);     
      }
      
      Hv_chkagemr_Clo_Upd(); 
      Hv_zf_Clo_Upd();   
    vtcp_log("[%s][%d] ***** iCheckHv_zfTable() 成功 !  ***** ",__FILE__,__LINE__);
    vtcp_log("[%s][%d] ***** iCheckHv_zfTable() 成功 !  ***** ",__FILE__,__LINE__);
      return 0;
}


/*******************************************************************************
* 函数名称: iCheckHv_chkagemrTable                                             *
* 描    述: 根据hv_chkagemr核对Hv_zf检查ccpc有 但dhcc没有 的情况 
            CCPC有 dhcc没有 的情况,                                            *
            来帐：按照ccpc插入hv_zf，走帐
            往帐：找到原来那条往帐，置标志，走帐                               *
* 参    数:                                                                    *
* 类    型:                                                                    *
* 返 回 值:int                                                                 *
*******************************************************************************/
int iCheckHv_chkagemrTable(int * iHandled)
{
      int ret = 0;
      int ret2 = 0;
      struct hv_zf_c hv_zf;
      struct hv_zfhtr_c hv_zfhtr;      
      struct hv_chkagemr_c hv_chkagemr;
      char cAcno110[20];
      int iInsertFlag = 0;
      int iLw_ind = 0;  /* iLw_ind=1往帐     iLw_ind=2来帐*/
      char cKinbr[BRNO_LEN+1];
      int iCzBankFlag = 0;/* 是否是村镇银行账务0=否    1=是 */
      
      
    /* dhcc和cpcc都有的情况，已在iCheckHv_zfTable()中将checkflag置为 'A'， 
       所以只查出checkflag='0' or checkflag is null 的情况, 就是 CCPC有 dhcc没有 的情况 */
      ret = Hv_chkagemr_Dec_Upd(g_pub_tx.reply, "tx_date=%d   and ( chk_flg ='0' or chk_flg is null ) and cmtno<>'232'  ", g_pub_tx.tx_date);
      if ( ret )
      {
            sprintf(acErrMsg,"Hv_chkagemr_Dec_Sel() 出错[%d]",ret);
            WRITEMSG
              return ret;
      }          
        
    while(1)
    {
        memset(&hv_chkagemr,0, sizeof( hv_chkagemr ) );               
        ret=Hv_chkagemr_Fet_Upd(&hv_chkagemr, g_pub_tx.reply);
        if( ret==100 )
        {
            vtcp_log("[%s][%d] 大额支付对帐明细表循环处理完毕  ",__FILE__,__LINE__); 
              Hv_zf_Clo_Sel();                      
            break;
        }
        else if(ret)
        {
            vtcp_log("[%s][%d]数据库操作异常,请与科技科联系!",__FILE__,__LINE__);
                sprintf(acErrMsg,"数据库操作异常,请与科技科联系![%d]",ret);
                WRITEMSG
                  return ret;
        }
        
        vtcp_log("[%s][%d] hv_chkagemr.pay_ac_no==[%s]  hv_chkagemr.cash_ac_no==[%s]  ",__FILE__,__LINE__, hv_chkagemr.pay_ac_no,hv_chkagemr.cash_ac_no); 
        
        
                /* ------根据 PAY_QS_NO 判断是来帐还是往帐 ---------------------------------*/
        /* ----- PAY_QS_NO 为本行清算行的为往帐，否则为来帐  -------------------- */
        vtcp_log("[%s][%d]  hv_chkagemr.pay_qs_no==[%s] hv_chkagemr.cashqsactno=[%s] HV_QSBRNO=[%s]",__FILE__,__LINE__,hv_chkagemr.pay_qs_no,hv_chkagemr.cashqsactno,HV_QSBRNO);        
        
        ret = memcmp(hv_chkagemr.pay_qs_no, HV_QSBRNO, sizeof(hv_chkagemr.pay_qs_no)-1);
        ret2 = memcmp(hv_chkagemr.cashqsactno, HV_QSBRNO, sizeof(hv_chkagemr.cashqsactno)-1);
        if(ret && ret2)
        {
            vtcp_log("[%s][%d] 接收清算行和发起清算行都不为本行清算行!!! ret=[%d]ret2=[%d]",__FILE__,__LINE__,ret,ret2);
            sprintf(acErrMsg,"接收清算行和发起清算行都不为本行清算行!! ret=[%d] ret2=[%d]",ret,ret2);
            WRITEMSG
               return -1;    
        } 
        
        if(ret==0 && ret2==0)
        {
            vtcp_log("[%s][%d] 接收清算行和发起清算行都是本行清算行!!! ret=[%d]ret2=[%d]",__FILE__,__LINE__,ret,ret2);
            sprintf(acErrMsg,"接收清算行和发起清算行都是本行清算行!! ret=[%d] ret2=[%d]",ret,ret2);
            WRITEMSG
               return -1;    
        }  
         
        if(ret)  /* 来帐 */
        {                    
             iLw_ind = 2;
          }
        else  /* 往帐 */
        {
             iLw_ind = 1;          
        }    
        vtcp_log("[%s][%d] iLw_ind=========[%d]  ",__FILE__,__LINE__, iLw_ind);   
        vtcp_log("[%s][%d] hv_chkagemr.pay_ac_no==[%s]  hv_chkagemr.cash_ac_no==[%s]  ",__FILE__,__LINE__, hv_chkagemr.pay_ac_no,hv_chkagemr.cash_ac_no); 

        
        /* ----- 1. 来帐：(没收到) 按照ccpc插入hv_zf，走帐            ------------- */ 
        /* ----- 2. 往帐：(极少见) 可能是往帐拒绝，ccpc又清算了，    ---------------*/
        /* ----- 所以找到原来那条往帐，置标志，走帐                  -------------- */  
        /* ----- 这里只根据关键字查找，因为如果存在这条记录就无法插入，只能修改-----*/  
        ret = Hv_zf_Dec_Upd(g_pub_tx.reply,"or_br_no='%s' and orderno='%s' and  cmtno='%s' and wt_date=%d ", 
                 hv_chkagemr.or_br_no,hv_chkagemr.orderno,hv_chkagemr.cmtno,hv_chkagemr.tx_date );
        if(ret)
        {
                sprintf(acErrMsg,"Hv_zf_Dec_Upd() 出错[%d]",ret);
                WRITEMSG
                  return ret;
        }
         
          memset(&hv_zf,0, sizeof(hv_zf));         
        ret = Hv_zf_Fet_Upd(&hv_zf,g_pub_tx.reply);
        if(ret && ret !=100)
        {
                sprintf(acErrMsg,"Hv_zf_Dec_Upd() 出错[%d]",ret);
                WRITEMSG
                  return ret;
        } 
        if(ret == 100)   /* 没找到 DHCC少帐 没有这笔交易的记录  按照ccpc插入hv_zf  正金额走帐 */
        {
              /* ==== 按照 hv_chkagemr 插入 hv_zf =======*/
              vtcp_log("[%s][%d] 没有这笔交易的记录 !",__FILE__,__LINE__);

            memset(&hv_zf,0,sizeof(hv_zf));    
            /*  
            vtcp_log("[%s][%d] ============ Hv_chkagemr_Debug() ========\n",__FILE__,__LINE__);    
            Hv_chkagemr_Debug(hv_chkagemr);
            vtcp_log("[%s][%d] ============ Hv_chkagemr_Debug() ========\n",__FILE__,__LINE__);    
            */      

            vtcp_log("[%s][%d] hv_chkagemr.pay_ac_no==[%s]  hv_chkagemr.cash_ac_no==[%s]  ",__FILE__,__LINE__, hv_chkagemr.pay_ac_no,hv_chkagemr.cash_ac_no); 
            vtcp_log("[%s][%d] hv_zf.pay_ac_no==[%s]  hv_zf.cash_ac_no==[%s]  ",__FILE__,__LINE__, hv_zf.pay_ac_no,hv_zf.cash_ac_no); 

            vtcp_log("[%s][%d] iLw_ind=========[%d]  ",__FILE__,__LINE__, iLw_ind);    
            if(iLw_ind == 2)  /* 来帐 */
            {                    
              hv_zf.lw_ind[0] = '2'; 
            }
            else if(iLw_ind == 1)   /* 往帐 */
            {
              hv_zf.lw_ind[0] = '1';     
            }else
            {
              sprintf(acErrMsg,"来往标识错[%d]",iLw_ind);
              WRITEMSG
                 return ret;
            }            
            vtcp_log("[%s][%d] hv_zf.lw_ind=[%s]  ",__FILE__,__LINE__,hv_zf.lw_ind);

            hv_zf.hv_sts[0] =  '9';  /* 默认为 来账记账清算，挂帐检查时再根据情况重设 */
            hv_zf.beg_sts[0] = hv_zf.hv_sts[0]; 
               iCopyHv_zfFromHv_chkagemr(&hv_zf,&hv_chkagemr);
            
            vtcp_log("[%s][%d] hv_chkagemr.pay_ac_no==[%s]  hv_chkagemr.cash_ac_no==[%s]  ",__FILE__,__LINE__, hv_chkagemr.pay_ac_no,hv_chkagemr.cash_ac_no); 
            vtcp_log("[%s][%d] hv_zf.pay_ac_no==[%s]  hv_zf.cash_ac_no==[%s]  ",__FILE__,__LINE__, hv_zf.pay_ac_no,hv_zf.cash_ac_no); 

            /* 冲帐后 置上标志，否则下一循环时还处理*/
            hv_chkagemr.chk_flg[0] = 'D';     /*NOT_FOUND_BANK  CCPC有，DHCC没有*/  
            /* begin add by LiuHuafeng 2009-5-14 17:09:11 */
            /* 如果是村镇银行，则不需要进行账务处理 */
            if(!strncmp(hv_chkagemr.br_no,HV_CUNZHEN_BR_NO,BRNO_LEN-2))
            {
                iCzBankFlag = 1;
                vtcp_log("%s,%d 村镇银行账务少，只做登记部处理",__FILE__,__LINE__);
            }
            else
            {
                iCzBankFlag = 0;
            }
            /* end by LiuHuafeng for 村镇银行 */
            *iHandled = 1;     /* 如果是商业银行的需要走帐，村镇银行的不需要走账 */   
            iInsertFlag = 1;   /* 标志 插入记录 */
        }
        else   /* 找到 也属于DHCC少帐 !!   找到这笔交易的记录  修改标志 正金额走帐 */
        {
              if( hv_zf.hv_sts[0] == '0' || hv_zf.hv_sts[0] == '1' || hv_zf.hv_sts[0] == '2' || hv_zf.hv_sts[0] == '3' ||
                     hv_zf.hv_sts[0] == '4' || hv_zf.hv_sts[0] == '7' || hv_zf.hv_sts[0] == '9' ||
                     hv_zf.hv_sts[0] == 'B' || hv_zf.hv_sts[0] == 'F' || hv_zf.hv_sts[0] == 'G' || 
                     hv_zf.hv_sts[0] == 'H' || hv_zf.hv_sts[0] == 'I' || hv_zf.hv_sts[0] == 'J'  )
              {      /*  这种情况应该是不可能的，这里还是判断一下 */
                    vtcp_log("[%s][%d] ==== 这里出错了，DHCC和CCPC都有，应该在 iCheckHv_zfTable()中处理 !!!!  ==== ",__FILE__,__LINE__);
                    vtcp_log("[%s][%d] ==== 这笔交易应该在 iCheckHv_zfTable()中处理，这里不管了 !!!!  ==== ",__FILE__,__LINE__);
            
                    /* hv_zf.checkflag[0] = 'A';    DHCC和CCPC相同 */
                    /* hv_chkagemr.chk_flg[0]= 'A';  CCPC_SAME_BANK DHCC和CCPC相同 */
                    /* *iHandled = 0;  不需要走帐 */
                         sprintf(acErrMsg,"===== 这里出错了 数据库中记录错误 !! ====[%s]",hv_zf.hv_sts);
                         WRITEMSG
                           return -1;
              }
              else
              {
                   vtcp_log("[%s][%d] 找到这笔交易的记录 修改标志 !",__FILE__,__LINE__);       
                     hv_zf.hv_sts[0] =  '9';  /* 默认为 来账记账清算，挂帐检查时再根据情况重设 */
                     hv_zf.beg_sts[0] = hv_zf.hv_sts[0]; 
                   hv_zf.checkflag[0] = 'D';     /*NOT_FOUND_BANK  CCPC有，DHCC没有*/

                   vtcp_log("[%s][%d] hv_chkagemr.pay_ac_no==[%s]  hv_chkagemr.cash_ac_no==[%s]  ",__FILE__,__LINE__, hv_chkagemr.pay_ac_no,hv_chkagemr.cash_ac_no); 
                   vtcp_log("[%s][%d] hv_zf.pay_ac_no==[%s]  hv_zf.cash_ac_no==[%s]  ",__FILE__,__LINE__, hv_zf.pay_ac_no,hv_zf.cash_ac_no); 

                   vtcp_log("[%s][%d] iLw_ind=========[%d]  ",__FILE__,__LINE__, iLw_ind);
                   if(iLw_ind == 2)  /* 来帐 */
                   {                    
                     hv_zf.lw_ind[0] = '2'; 
                   }
                   else if(iLw_ind == 1)   /* 往帐 */
                   {
                     hv_zf.lw_ind[0] = '1';     
                   }else
                   {
                     sprintf(acErrMsg,"来往标识错[%d]",iLw_ind);
                     WRITEMSG
                        return ret;
                   }            
                   vtcp_log("[%s][%d] hv_zf.lw_ind=[%s]  ",__FILE__,__LINE__,hv_zf.lw_ind);

                   /* 其他字段呢? 应该按照CCPC的，我们这边在Hv_zfhtr中还有一个备份 ?? */
                   iCopyHv_zfFromHv_chkagemr(&hv_zf,&hv_chkagemr);
                   vtcp_log("%s,%d brno=%s",__FILE__,__LINE__,hv_zf.br_no);
                   vtcp_log("[%s][%d] hv_chkagemr.pay_ac_no==[%s]  hv_chkagemr.cash_ac_no==[%s]  ",__FILE__,__LINE__, hv_chkagemr.pay_ac_no,hv_chkagemr.cash_ac_no); 
                   vtcp_log("[%s][%d] hv_zf.pay_ac_no==[%s]  hv_zf.cash_ac_no==[%s]  ",__FILE__,__LINE__, hv_zf.pay_ac_no,hv_zf.cash_ac_no); 

                   /* 冲帐后 置上标志，否则下一循环时还处理*/
                   hv_chkagemr.chk_flg[0] = 'D';     /*NOT_FOUND_BANK  CCPC有，DHCC没有*/
                   
                   /* 如果是村镇银行，则不需要进行账务处理 */
                   /* begin add by LiuHuafeng 2009-5-14 17:09:11 */
                   /* 如果是村镇银行，则不需要进行账务处理 */
                   if(!strncmp(hv_chkagemr.br_no,HV_CUNZHEN_BR_NO,BRNO_LEN-2))
                   {
                       iCzBankFlag = 1;
                       vtcp_log("%s,%d 村镇银行账务少，只做登记部处理",__FILE__,__LINE__);
                   }
                   else
                   {
                       iCzBankFlag = 0;
                   }
                   *iHandled = 1;     /* 需要走帐 */            
              }          
        } 

        ret = Hv_chkagemr_Upd_Upd(hv_chkagemr,g_pub_tx.reply);
        if(ret)
        {
             vtcp_log("[%s][%d] Hv_chkagemr_Upd_Upd() 出错!",__FILE__,__LINE__);
                 sprintf(acErrMsg,"数据库操作异常,请与科技科联系![%d]",ret);
                 WRITEMSG
                   return ret;
        }             
        vtcp_log("[%s][%d]  修改 Hv_chkagemr.chk_flg 标志成功!  ",__FILE__,__LINE__);      

        if(*iHandled == 0)   /* 不需要走帐，继续循环走帐 */
        {
                Hv_zf_Clo_Upd(); 
              vtcp_log("[%s][%d] 不需要走帐，继续循环走帐 !!!  ",__FILE__,__LINE__);
                continue;
        }

        int iCmtno = atoi(hv_chkagemr.cmtno);
        vtcp_log("[%s][%d]  cmtno==[%d] ",__FILE__,__LINE__,iCmtno);
        /* begin add by LiuHuafeng 2009-5-16 19:37:48 */
        if(iCzBankFlag == 0)
        {
        /* end by LiuHuafeng 2009-5-16 19:37:55 */
            if(iCmtno==100 || iCmtno==101 || iCmtno==102 || iCmtno==103 || 
                  iCmtno==105 || iCmtno==108 || iCmtno==122) 
            {  /* 普通类: (CMT 100 101 102 103 105 108 122)*/
                     vtcp_log("[%s][%d] 普通类 正金额走帐 ",__FILE__,__LINE__);
                vtcp_log("[%s][%d] hv_chkagemr.pay_ac_no==[%s]  hv_chkagemr.cash_ac_no==[%s]  ",__FILE__,__LINE__, hv_chkagemr.pay_ac_no,hv_chkagemr.cash_ac_no); 
                vtcp_log("[%s][%d] hv_zf.pay_ac_no==[%s]  hv_zf.cash_ac_no==[%s]  ",__FILE__,__LINE__, hv_zf.pay_ac_no,hv_zf.cash_ac_no); 
            
                  ret = iSet8583_For_CommonCMT(&hv_zf,1);  /*** 1=正金额走帐 ***/
                  if(ret)
                  {
                    vtcp_log("[%s][%d]iSet8583_For_CommonCMT() 出错!",__FILE__,__LINE__);
                  sprintf(acErrMsg,"设置8583域时出错![%d]",ret);
                  WRITEMSG
                    return ret;
                  }          
            
            }            
            else  if(iCmtno==121 || iCmtno==123 || iCmtno==124 )
            {  /*  特殊类 (CMT 121 123 124)  */
                     vtcp_log("[%s][%d] 特殊类 正金额走帐  iCmtno=[%d] ",__FILE__,__LINE__,iCmtno);
                vtcp_log("[%s][%d] hv_chkagemr.pay_ac_no==[%s]  hv_chkagemr.cash_ac_no==[%s]  ",__FILE__,__LINE__, hv_chkagemr.pay_ac_no,hv_chkagemr.cash_ac_no); 
                vtcp_log("[%s][%d] hv_zf.pay_ac_no==[%s]  hv_zf.cash_ac_no==[%s]  ",__FILE__,__LINE__, hv_zf.pay_ac_no,hv_zf.cash_ac_no); 
            
                  ret = iSet8583_For_SpecialCMT(&hv_zf, hv_chkagemr.p_num, hv_chkagemr.p_date,hv_chkagemr.jf_amt,1);  /*** 1=正金额走帐 ***/
                if(ret)
                  {
                    vtcp_log("[%s][%d]iSet8583_For_SpecialCMT() 出错!",__FILE__,__LINE__);
                  sprintf(acErrMsg,"设置8583域时出错![%d]",ret);
                  WRITEMSG
                    return ret;
                  }  
            }     
            else
            {
               sprintf(acErrMsg," 该报文不在这里处理!! ");
               WRITEMSG
                 return -1;                
            }
            memset(cKinbr,0,sizeof(cKinbr));
            get_zd_data("0030",cKinbr);
            MEMCPY_DEBUG(hv_zf.br_no,cKinbr,sizeof(hv_zf.br_no)-1);
        /*begin by LiuHuafeng 2009-5-16 19:38:06 for 村镇银行 */
        }
        /* end by LiuHuafeng 2009-5-16 19:38:23 */
        vtcp_log("[%s][%d]  hv_zf.br_no=====[%s]  ",__FILE__,__LINE__,hv_zf.br_no); 
        vtcp_log("[%s][%d]  hv_zf.br_no=====[%s]  ",__FILE__,__LINE__,hv_zf.br_no); 
        vtcp_log("[%s][%d] hv_chkagemr.pay_ac_no==[%s]  hv_chkagemr.cash_ac_no==[%s]  ",__FILE__,__LINE__, hv_chkagemr.pay_ac_no,hv_chkagemr.cash_ac_no); 
        vtcp_log("[%s][%d] hv_zf.pay_ac_no==[%s]  hv_zf.cash_ac_no==[%s]  ",__FILE__,__LINE__, hv_zf.pay_ac_no,hv_zf.cash_ac_no); 

        /*  置往帐的状态  */
        if(hv_zf.lw_ind[0] == '1')  /* 往帐对帐时有可能挂帐 */
        {
/*          if( hv_zf.hv_sts[0] != RECEIVE_CUSTNAME_ERR && hv_zf.hv_sts[0] != RECEIVE_DATA_ERR )
            {
                    hv_zf.hv_sts[0] = 'L'; 
            }
*/
            hv_zf.hv_sts[0] = 'L';        /* 置为 核对不符 */
        }                                

        if( iInsertFlag )   /* 没有找到相应记录 插� */
        {
              /* ==== 按照 hv_chkagemr 插入 hv_zf =======*/
              vtcp_log("[%s][%d] 没有这笔交易的记录，按照ccpc插入hv_zf!",__FILE__,__LINE__);
            vtcp_log("[%s][%d]  hv_zf.hv_sts=====[%s]  ",__FILE__,__LINE__,hv_zf.hv_sts);                

            vtcp_log("[%s][%d] hv_chkagemr.pay_ac_no==[%s]  hv_chkagemr.cash_ac_no==[%s]  ",__FILE__,__LINE__, hv_chkagemr.pay_ac_no,hv_chkagemr.cash_ac_no); 
            vtcp_log("[%s][%d] hv_zf.pay_ac_no==[%s]  hv_zf.cash_ac_no==[%s]  ",__FILE__,__LINE__, hv_zf.pay_ac_no,hv_zf.cash_ac_no); 

              if( iInsHv_zfByHv_chkagemr(&hv_zf,&hv_chkagemr ))
            {
              sprintf(acErrMsg,"iInsHv_zfByHv_chkagemr() 出错[%d]",ret);
              WRITEMSG
             return -1;
            }
            vtcp_log("[%s][%d]  插入成功!  ",__FILE__,__LINE__);  
        }
        else /* 更新 */
        {
              vtcp_log("[%s][%d]  hv_zf.hv_sts=====[%s]  ",__FILE__,__LINE__,hv_zf.hv_sts);  
              
            vtcp_log("[%s][%d] hv_chkagemr.pay_ac_no==[%s]  hv_chkagemr.cash_ac_no==[%s]  ",__FILE__,__LINE__, hv_chkagemr.pay_ac_no,hv_chkagemr.cash_ac_no); 
            vtcp_log("[%s][%d] hv_zf.pay_ac_no==[%s]  hv_zf.cash_ac_no==[%s]  ",__FILE__,__LINE__, hv_zf.pay_ac_no,hv_zf.cash_ac_no); 

            ret = Hv_zf_Upd_Upd(hv_zf,g_pub_tx.reply);
            if(ret)
            {
                   vtcp_log("[%s][%d] Hv_zf_Upd_Upd() 出错!",__FILE__,__LINE__);
                        sprintf(acErrMsg,"数据库操作异常,请与科技科联系![%d]",ret);
                        WRITEMSG
                          return -1;
            }             
            vtcp_log("[%s][%d]  修改标志成功!  ",__FILE__,__LINE__);             
        }    

        Hv_zf_Clo_Upd(); 
        *iHandled = 1;   /* 处理了一条记录  !! */

        /* 汇票类DHCC少帐 需要更新汇票的状态 (CMT 121 122 123 124) */
        if(iCmtno==121 || iCmtno==122 || iCmtno==123 || iCmtno==124 )
        {  
              vtcp_log("[%s,%d] 汇票类DHCC少帐 需要更新汇票的状态 !",__FILE__,__LINE__);    
              
              /* hv_chkagemr.other_amt 实际结算金额  hv_chkagemr.tx_amt 剩余金额 */
              if(hv_zf.lw_ind[0] == '1')   /* 往帐 */
              {
                    ret = iHv_SZ_ChangeHPStat(hv_chkagemr.or_br_no,hv_chkagemr.p_num, hv_chkagemr.other_amt, hv_chkagemr.tx_amt, iCmtno);
                   if(ret)
                   {
                             vtcp_log("%s,%d 更新汇票信息表错误 ret=[%d]",__FILE__,__LINE__,ret);
                             vtcp_log(acErrMsg,"更新汇票信息表错误");
                             WRITEMSG
                             return ret;
                   }                      
              }
              else  /* 来帐 */
              {
                    ret = iHv_SZ_ChangeHPStat(hv_chkagemr.acbrno,hv_chkagemr.p_num, hv_chkagemr.other_amt, hv_chkagemr.tx_amt, iCmtno);
                   if(ret)
                   {
                             vtcp_log("%s,%d 更新汇票信息表错误 ret=[%d]",__FILE__,__LINE__,ret);
                             vtcp_log(acErrMsg,"更新汇票信息表错误");
                             WRITEMSG
                             return ret;
                   }     
              }
               vtcp_log("[%s,%d] 更新汇票状态成功!  ",__FILE__,__LINE__); 
        }      

        /***********   登记差错表     ***************/                           
        ret = iInsertHv_chkerror(&hv_zf, &hv_chkagemr);
        if(ret)
        {
            vtcp_log("[%s][%d]数据库操作异常,请与科技科联系!",__FILE__,__LINE__);
            sprintf(acErrMsg,"数据库操作异常,请与科技科联系![%d]",ret);
            WRITEMSG
            return ret;
        }
        vtcp_log("[%s][%d]  登记差错表成功 ! ",__FILE__,__LINE__);
        if(iCzBankFlag == 1 &&  *iHandled == 1)
        {
            *iHandled = 0;
            vtcp_log("%s,%d 是村镇银行少账情况，只做登记，不进行账务处理，继续下一个循环",__FILE__,__LINE__);
            continue;
        }
        break;     
   } 

   Hv_chkagemr_Clo_Upd();
    vtcp_log("[%s][%d] ***** Hv_chkagemr_Clo_Upd() 成功 !  ***** ",__FILE__,__LINE__);
    vtcp_log("[%s][%d] ***** Hv_chkagemr_Clo_Upd() 成功 !  ***** ",__FILE__,__LINE__);      
   
      return 0;    
}


/********************************************************
* 函 数 名:  iSet8583_For_SpecialCMT()
* 功能描述： 为特殊类走帐设置8583域 (CMT121 123 124) 
   DHCC有 CCPC没有  负金额走帐     
   CCPC有 DHCC没有  正金额走帐                   
  
* 参数： ZFflg = 1   正交易走帐
         ZFflg = -1  负交易走帐    
********************************************************/
int iSet8583_For_SpecialCMT(struct hv_zf_c* hv_zf, char* p_num, long p_date, double dTotal_amt,int ZFflg)
{
        int ret;
      int iCmtno = atoi(hv_zf->cmtno);
      char cKinbr[BRNO_LEN+1];
         char cAc_no[20];   
         struct com_branch_c sCom_branch;
         char cStat[2];
         char cTmpStat[2];   
      char cOpenbr[BRNO_LEN+1];    

        memset(cOpenbr, 0 , sizeof(cOpenbr));


      vtcp_log("[%s][%d]  cOpenbr==[%s] ",__FILE__,__LINE__,cOpenbr);              
      vtcp_log("[%s][%d]  cmtno==[%d] ",__FILE__,__LINE__,iCmtno);  
      vtcp_log("[%s][%d]  ZFflg==[%d] ",__FILE__,__LINE__,ZFflg);   

        if((iCmtno == 121) && (hv_zf->lw_ind[0] == '1') )   /* 往帐 汇票资金移存*/
        {
            /*   借： 2440402 帐户           |0400 签发金额   */
            /*   贷： 41102                  |0390            */
            /*   借： 41102                  |0390            */
            /*   贷： 110 帐户               |0400                */
            vtcp_log("[%s][%d] ====== 该笔交易是往帐，以下开始走帐 !! iCmtno=[%d] ====== ",__FILE__,__LINE__,iCmtno);

            GetBr_noByOr_br_no(hv_zf->pay_opn_br_no, cOpenbr);  /* 付款人开户行  */
            vtcp_log("%s,%d  cOpenbr======[%s]",__FILE__,__LINE__,cOpenbr);  
                
               /*************    挂帐检查   ****************/
              /** 判断接收机构是否存在 */  
                  memset(cKinbr, 0 , sizeof(cKinbr));
              MEMCPY_DEBUG(cKinbr,hv_zf->br_no,BRNO_LEN); 
            memset(cStat, 0, sizeof(cStat));        
            memset(cTmpStat, 0, sizeof(cTmpStat));                
              set_zd_data("0980","9");     /*  */
              
               ret = iHvHang_Check_Brno(cOpenbr, cKinbr);  
            if(ret)
            {
               sprintf(acErrMsg,"数据库操作失败[%d]",g_reply_int);
               WRITEMSG
               return -1;
            }               
            get_zd_data(DC_TX_BR_NO,cKinbr);   
            vtcp_log("%s,%d 得到的交易机构是[%s]",__FILE__,__LINE__,cKinbr);
            iGetReceTel(cKinbr);
            get_zd_data("0980",cStat);
              vtcp_log("%s,%d 得到cStat====[%s]",__FILE__,__LINE__,cStat);
              if(ZFflg == 1) 
              {
                    hv_zf->hv_sts[0] = cStat[0];  /* 负金额走帐为冲帐，在这里不改变状态 */
                    hv_zf->beg_sts[0] = hv_zf->hv_sts[0]; 
              }
            
                /* 贷： 41101                   */
                /* 借： 41101                   */                                      
            set_zd_data("0640",HVWZGD); /* 大额往帐过渡科目 */                     
            set_zd_data("0210","01");                                             
            set_zd_data("0510",hv_zf->cmtno);  
                        
            /* 设置来帐的2440402，2440401帐号和余额，一定为0 否则会走帐*/ 
            memset(cAc_no, 0 , sizeof(cAc_no));
            ret=pub_base_GetParm_Str(cKinbr,PARM_HPQF,cAc_no);
            if(ret)
            {
                vtcp_log("%s,%d 没有找到交易机构指定的汇票签发户",__FILE__,__LINE__);
                return -1;
            }
            vtcp_log("[%s][%d] PARM_HPQF  cAc_no===[%s]  ",__FILE__,__LINE__,cAc_no);  
            set_zd_data("0320",cAc_no);
            memset(cAc_no, 0 , sizeof(cAc_no));
            ret=pub_base_GetParm_Str(cKinbr,PARM_HPYC,cAc_no);
            if(ret)
            {
                vtcp_log("%s,%d 没有找到交易机构指定的汇票移存户",__FILE__,__LINE__);
                return -1;
            }
            vtcp_log("[%s][%d]  PARM_HPYC  cAc_no===[%s]  ",__FILE__,__LINE__,cAc_no);  
            set_zd_data("0330",cAc_no);
               set_zd_double("0420",0.0 );  /* 这里不走2440401和2440402，2440402通过0300走 */   
      
            /* 借： 2440402 帐户            */                     
            set_zd_data("0660","1");  /* 设置客户帐方向借贷标志 1-借； **/               
              set_zd_data("0680","0");  /* 帐户类型 */
               set_zd_data("0300", cAc_no );  /* 2440402 PARM_HPYC 帐户 */                   
                 
                /* 贷： 110帐户                    */     
            memset(cAc_no, 0 , sizeof(cAc_no));                                      
            ret=pub_base_GetParm_Str(cKinbr,PARM_HVQS,cAc_no);
            if(ret)
            {
                    vtcp_log("%s,%d 系统配置错误，得到机构%s的110帐号错误[%d]",__FILE__,__LINE__,ret);
                    return -1;
            }
            vtcp_log("[%s][%d]  PARM_HVQS cAc_no===[%s] ",__FILE__,__LINE__,cAc_no);                                         
            set_zd_data("1201", cAc_no);
            set_zd_data("0720","2");  /* 设置110 方向借贷标志  2-贷 */ 

            set_zd_double("0390",hv_zf->tx_amt * ZFflg);  
            set_zd_double("0400",hv_zf->tx_amt * ZFflg );             
           /* set_zd_data("0070",hv_zf->tel);  */ 
             
        } else if( (iCmtno == 123) && (hv_zf->lw_ind[0] == '2')) /*来帐 多余资金划回*/
        {
               /*   借： 110 帐户                 |0400 未用金额    */
               /*   贷： 41102                    |0390             */
               /*   借： 41102                    |0390             */
               /*   贷： 客户 / 挂帐帐号          |0400             */
               /*   借： 2440401                  |0420 签发金额    */
               /*   贷： 2440402                  |0420                  */
            vtcp_log("[%s][%d] ====== 该笔交易是往帐，以下开始走帐 !! iCmtno=[%d] ====== ",__FILE__,__LINE__,iCmtno);
            GetBr_noByOr_br_no(hv_zf->pay_opn_br_no, cOpenbr);  /* 付款人开户行  */
            vtcp_log("%s,%d  cOpenbr======[%s]",__FILE__,__LINE__,cOpenbr); 
               /*   贷： 客户 / 挂帐帐号          |0400             */
               
               /*************   挂帐检查  *******************/    
               /*********** 主要是机构、帐号及其状态的检查   ******/
                  memset(cAc_no, 0 , sizeof(cAc_no));
                  memset(cKinbr, 0 , sizeof(cKinbr));
                  MEMCPY_DEBUG(cKinbr,hv_zf->br_no,BRNO_LEN);
            memset(cStat, 0, sizeof(cStat));        
            memset(cTmpStat, 0, sizeof(cTmpStat));                
              set_zd_data("0980","9");     /*  */

            /* 123 124 用付款人帐号走帐 
            ret = iHvHang_Check_Acno(hv_zf->cash_ac_no,hv_zf->cash_name,cKinbr);  设置好 0030 */
            vtcp_log("[%s][%d]  hv_zf->pay_ac_no==[%s] ",__FILE__,__LINE__,hv_zf->pay_ac_no); 
            vtcp_log("[%s][%d]  hv_zf->pay_name==[%s] ",__FILE__,__LINE__,hv_zf->pay_name); 
      
            ret = iHvHang_Check_Acno(hv_zf->pay_ac_no,hv_zf->pay_name,cKinbr);  /* 设置好 0300 */
              if(ret)
              {
                     vtcp_log("%s,%d iHvHang_Check_Acno()  出错 ",__FILE__,__LINE__);
                 sprintf(acErrMsg," iHvHang_Check_Acno()  出错 !! ");
                 WRITEMSG               
                     return ret;
              }
              
            get_zd_data(DC_TX_BR_NO,cKinbr);   
            vtcp_log("%s,%d 得到的交易机构是[%s]",__FILE__,__LINE__,cKinbr);
            iGetReceTel(cKinbr);
            get_zd_data("0980",cStat);
              vtcp_log("%s,%d 得到cStat====[%s]",__FILE__,__LINE__,cStat);
              if(ZFflg == 1) 
              {
                   hv_zf->hv_sts[0] = cStat[0];  /* 负金额走帐为冲帐，在这里不改变状态 */
                   hv_zf->beg_sts[0] = hv_zf->hv_sts[0]; 
              }
              
                            
                /* 借： 客户 (挂帐帐号)         */                     
            set_zd_double("0390",hv_zf->tx_amt * ZFflg);  /*  未用金额*/
            set_zd_double("0400",hv_zf->tx_amt * ZFflg);  /*  未用金额*/
               vtcp_log("%s,%d 得到的 0420 签发金额 tx_amt ==== [%lf]",__FILE__,__LINE__,hv_zf->tx_amt*ZFflg);    
            
            set_zd_data("0660","2");  /* 设置客户帐方向借贷标志 1-借；2-贷 **/
            /* set_zd_data("0070",hv_zf->tel);  */     
                      
                /* 贷： 41201                   */
                /* 借： 41201                   */                                      
            set_zd_data("0640",HVLZGD); /* 大额来帐过渡科目 */                     
            set_zd_data("0210","01");                                             
            set_zd_data("0510",hv_zf->cmtno);  
                 
                /* 贷： 110帐户                    */                           
            memset(cAc_no, 0 , sizeof(cAc_no));                
            ret=pub_base_GetParm_Str(cKinbr,PARM_HVQS,cAc_no);
            if(ret)
            {
                    vtcp_log("%s,%d 系统配置错误，得到机构%s的110帐号错误[%d]",__FILE__,__LINE__,ret);
                    return -1;
            }
            vtcp_log("[%s][%d]  PARM_HVQS cAc_no===[%s] ",__FILE__,__LINE__,cAc_no);                                         
            set_zd_data("1201", cAc_no);            
            set_zd_data("0720","1");  /* 设置110 方向借贷标志  1-借 */ 
                 
                                           
            /* 设置来帐的2440402，2440401帐号和金额  */ 
            memset(cAc_no, 0 , sizeof(cAc_no));
            ret=pub_base_GetParm_Str(cKinbr,PARM_HPQF,cAc_no);
            if(ret)
            {
                vtcp_log("%s,%d 没有找到交易机构指定的汇票签发户",__FILE__,__LINE__);
                return -1;
            }
            vtcp_log("[%s][%d]   PARM_HPQF  cAc_no===[%s]  ",__FILE__,__LINE__,cAc_no);  
            set_zd_data("0320",cAc_no);
            memset(cAc_no, 0 , sizeof(cAc_no));
            ret=pub_base_GetParm_Str(cKinbr,PARM_HPYC,cAc_no);
            if(ret)
            {
                vtcp_log("%s,%d 没有找到交易机构指定的汇票移存户",__FILE__,__LINE__);
                return -1;
            }
            vtcp_log("[%s][%d]  PARM_HPYC  cAc_no===[%s]  ",__FILE__,__LINE__,cAc_no);  
            set_zd_data("0330",cAc_no);
               set_zd_double("0420",dTotal_amt * ZFflg);  /*  签发金额*/  
               vtcp_log("%s,%d 得到的 0420 签发金额 dTotal_amt ==== [%lf]",__FILE__,__LINE__,dTotal_amt*ZFflg);    
        } else if( (iCmtno == 124) && (hv_zf->lw_ind[0] == '2'))  /*来帐 汇票资金未用退回*/
        {
             /*     借： 110 帐户                |0400 签发金额  */
             /*     贷： 41102                   |0390           */
             /*     借： 41102                   |0390           */
             /*   1 汇票日期逾期：                               */
             /*     贷： 2440403 帐户            |0400           */
             /*   2 汇票日期未逾期：                             */
             /*     贷： 2440402                 |0420           */
             /*     借： 2440401                 |0420           */
             /*     贷： 客户 挂帐帐号           |0400           */           

            vtcp_log("[%s][%d] ====== 该笔交易是来帐，以下开始走帐 !! iCmtno=[%d] ====== ",__FILE__,__LINE__,iCmtno);
            GetBr_noByOr_br_no(hv_zf->pay_opn_br_no, cOpenbr);  /* 付款人开户行  */
            vtcp_log("%s,%d  cOpenbr======[%s]",__FILE__,__LINE__,cOpenbr); 
            
               /*************   挂帐检查  *******************/    
               /*********** 主要是机构、帐号及其状态的检查   ******/
                  memset(cAc_no, 0 , sizeof(cAc_no));
                  memset(cKinbr, 0 , sizeof(cKinbr));
                  MEMCPY_DEBUG(cKinbr,hv_zf->br_no,BRNO_LEN);
                        
            if( iCheckYQ(p_num, p_date) ) /*   1 汇票日期逾期：  */
            {/*     贷： 2440403 帐户            |0400           */  
                
                memset(cStat, 0, sizeof(cStat));        
                memset(cTmpStat, 0, sizeof(cTmpStat));                
                  set_zd_data("0980","9");     /*  */
                  
                   ret = iHvHang_Check_Brno(cOpenbr,cKinbr);  
                if(ret)
                {
                   sprintf(acErrMsg,"数据库操作失败[%d]",g_reply_int);
                   WRITEMSG
                   return -1;
                } 
                get_zd_data(DC_TX_BR_NO,cKinbr);   
                vtcp_log("%s,%d 得到的交易机构是[%s]",__FILE__,__LINE__,cKinbr);
                iGetReceTel(cKinbr);
                get_zd_data("0980",cStat);
                  vtcp_log("%s,%d 得到cStat====[%s]",__FILE__,__LINE__,cStat);
                  if(ZFflg == 1)  /* 只处理正金额  负金额走帐为冲帐，在这里不改变状态 */
                  {
                       hv_zf->hv_sts[0] = cStat[0]; 
                       hv_zf->beg_sts[0] = hv_zf->hv_sts[0]; 
                  }
                  
                 set_zd_data("0680","6");/* 设置帐户类型 */
                  
                 memset(cAc_no, 0 , sizeof(cAc_no));
                 ret=pub_base_GetParm_Str(cKinbr,PARM_HPTH,cAc_no); /** 2440403机构汇票逾期退回帐号 */
                 if(ret)
                 {
                       vtcp_log("%s,%d 没有找到交易机构指定的汇票移存户",__FILE__,__LINE__);
                       return -1;
                 }

                 vtcp_log("[%s][%d]   PARM_HPTH cAc_no===[%s]  ",__FILE__,__LINE__,cAc_no);  
                 set_zd_data("0300",cAc_no);                
                    /* set_zd_double("0420", 0.0);   不走2440402，2440401  */ 

                 set_zd_data("0660","2");   /* 设置借贷标志 2-贷 **/                     
            }
            else   /*   2 汇票日期未逾期：   */
            {
                 /*     贷： 2440402                 |0420           */
                 /*     借： 2440401                 |0420           */             
                 /*     贷： 客户/挂帐帐号           |0400           */
                vtcp_log("[%s][%d] cKinbr===[%s] hv_zf->cash_ac_no===[%s] ",__FILE__,__LINE__,cKinbr,hv_zf->cash_ac_no);                                         
                vtcp_log("[%s][%d] cKinbr===[%s] hv_zf->cash_name ===[%s] ",__FILE__,__LINE__,hv_zf->cash_name);      

                vtcp_log("[%s][%d] cKinbr===[%s] hv_zf->pay_ac_no===[%s] ",__FILE__,__LINE__,cKinbr,hv_zf->pay_ac_no);                                         
                vtcp_log("[%s][%d] cKinbr===[%s] hv_zf->pay_name ===[%s] ",__FILE__,__LINE__,hv_zf->pay_name);      


                memset(cStat, 0, sizeof(cStat));        
                memset(cTmpStat, 0, sizeof(cTmpStat));                
                  set_zd_data("0980","9");     /*  */

                  /* 123 124 用付款人帐号走帐 
                ret = iHvHang_Check_Acno(hv_zf->cash_ac_no,hv_zf->cash_name,cKinbr);  设置好 0030 */
                   ret = iHvHang_Check_Acno(hv_zf->pay_ac_no,hv_zf->pay_name,cKinbr);  /* 设置好 0030 */
                   if(ret)
                   {
                          vtcp_log("%s,%d iHvHang_Check_Acno()  出错 ",__FILE__,__LINE__);
                         sprintf(acErrMsg," iHvHang_Check_Acno()  出错 !! ");
                         WRITEMSG 
                          return ret;
                   }
                get_zd_data(DC_TX_BR_NO,cKinbr);   
                vtcp_log("%s,%d 得到的交易机构是[%s]",__FILE__,__LINE__,cKinbr);
                iGetReceTel(cKinbr);
                get_zd_data("0980",cStat);
                  vtcp_log("%s,%d 得到cStat====[%s]",__FILE__,__LINE__,cStat);
                  if(ZFflg == 1) /* 只处理正金额  负金额走帐为冲帐，在这里不改变状态 */
                  {
                       hv_zf->hv_sts[0] = cStat[0];   
                       hv_zf->beg_sts[0] = hv_zf->hv_sts[0]; 
                  }
                               
                    /* 贷： 客户 (挂帐帐号)         */                     
                set_zd_double("0390",hv_zf->tx_amt * ZFflg);  /*  未用金额*/
                set_zd_double("0400",hv_zf->tx_amt * ZFflg);  /*  未用金额*/
                   set_zd_double("0420",hv_zf->tx_amt * ZFflg);  /* 走2440402，2440401  */
                set_zd_data("0660","2");   /* 设置借贷标志 2-贷 **/                        
            }
               set_zd_double("0420",hv_zf->tx_amt * ZFflg);  /* 不管是否逾期，都走 2440401 2440402 */            
            set_zd_double("0390",hv_zf->tx_amt * ZFflg);  /*  未用金额*/
            set_zd_double("0400",hv_zf->tx_amt * ZFflg);  /*  未用金额*/ 
            
                /* 贷： 41201                   */
                /* 借： 41201                   */                                      
            set_zd_data("0640",HVLZGD); /* 大额来帐过渡科目 */                     
            set_zd_data("0210","01");                                             
            set_zd_data("0510",hv_zf->cmtno);  
                 
                /* 借： 110帐户                    */                           
            memset(cAc_no, 0 , sizeof(cAc_no));
            /* ret=pub_base_GetParm_Str(hv_zf->br_no,PARM_HVQS,cAc_no);  2006-11-13 19:49 */
            ret=pub_base_GetParm_Str(cKinbr, PARM_HVQS,cAc_no);
            if(ret)
            {
                    vtcp_log("%s,%d 系统配置错误，得到机构%s的110帐号错误[%d]",__FILE__,__LINE__,ret);
                    return -1;
            }
            vtcp_log("[%s][%d]  PARM_HVQS  cAc_no===[%s]  ",__FILE__,__LINE__,cAc_no);              
            set_zd_data("1201",cAc_no);
            set_zd_data("0720","1");  /* 设置110 方向借贷标志  1-借 */ 
                                          
            /* 设置来帐的2440402，2440401帐号和余额，*/ 
            memset(cAc_no, 0 , sizeof(cAc_no));
            ret=pub_base_GetParm_Str(cKinbr, PARM_HPQF,cAc_no);
            if(ret)
            {
                vtcp_log("%s,%d 没有找到交易机构指定的汇票签发户",__FILE__,__LINE__);
                return -1;
            }
            vtcp_log("[%s][%d] PARM_HPQF  cAc_no===[%s]  ",__FILE__,__LINE__,cAc_no);  
            set_zd_data("0320",cAc_no);
            memset(cAc_no, 0 , sizeof(cAc_no));
            ret=pub_base_GetParm_Str(cKinbr, PARM_HPYC,cAc_no);
            if(ret)
            {
                vtcp_log("%s,%d 没有找到交易机构指定的汇票移存户",__FILE__,__LINE__);
                return -1;
            }
            vtcp_log("[%s][%d]  PARM_HPYC cAc_no===[%s]  ",__FILE__,__LINE__,cAc_no);  
            set_zd_data("0330",cAc_no);
        }
        else
        {
          sprintf(acErrMsg," 该报文不在这里处理 !! ");
          WRITEMSG
          return -1;
        }
        
      return 0;
}


int iCheckYQ(char* p_num,long p_date)
{
    
      int iYqFlag=0;/* 0未逾期,1逾期 */
      struct hv_poinfo_c sHv_poinfo;
  
      struct mdm_ac_rel_c sMdm_ac_rel;
      struct in_mst_c     sIn_mst;
      memset(&sMdm_ac_rel, 0 ,sizeof(sMdm_ac_rel));
      memset(&sIn_mst , 0 , sizeof(sIn_mst));
      memset(&sHv_poinfo, 0 , sizeof(sHv_poinfo));
  
    vtcp_log("[%s][%d]   p_num===[%s]  ",__FILE__,__LINE__, p_num);  
    vtcp_log("[%s][%d]   p_date===[%d]  ",__FILE__,__LINE__, p_date);  
      
      MEMCPY_DEBUG(sHv_poinfo.po_no,p_num,sizeof(sHv_poinfo.po_no));
      g_reply_int=Hv_poinfo_Sel(g_pub_tx.reply,&sHv_poinfo,"po_no='%s'",sHv_poinfo.po_no);
      if(g_reply_int)
      {
          /* 汇票不存在的情况按照逾期处理 */
          iYqFlag=1;
          return iYqFlag;
      }
    vtcp_log("[%s][%d]   sHv_poinfo.sign_date===[%d]  ",__FILE__,__LINE__, sHv_poinfo.sign_date);        
    vtcp_log("[%s][%d]   sHv_poinfo.pay_date===[%d]  ",__FILE__,__LINE__, sHv_poinfo.pay_date);        
      pub_base_deadlineM(sHv_poinfo.sign_date,1,&sHv_poinfo.pay_date);
    vtcp_log("[%s][%d]   sHv_poinfo.sign_date===[%d]  ",__FILE__,__LINE__, sHv_poinfo.sign_date);        
    vtcp_log("[%s][%d]   sHv_poinfo.pay_date===[%d]  ",__FILE__,__LINE__, sHv_poinfo.pay_date);        
      
      if(sHv_poinfo.pay_date < g_pub_tx.tx_date)
      {
           iYqFlag = 1;
      }
      else
      {
           iYqFlag = 0;
      }
    return iYqFlag;
}

/********************************************************
* 函 数 名:  iSet8583_For_CommonCMT()
* 功能描述： 为普通类走帐设置8583域 (CMT 100 101 102 103 105 108 122) 
   DHCC有 CCPC没有  负金额走帐     
   CCPC有 DHCC没有  正金额走帐                   
   来帐： 借 110   贷 客户
   往帐： 借 客户  贷 110    
* 参数： ZFflg = 1  正交易走帐
         ZFflg = -1  负交易走帐    
********************************************************/
int iSet8583_For_CommonCMT(struct hv_zf_c* hv_zf, int ZFflg)
{
    int ret;
      char cAcno110[20];
    char cStat[2];
    char cTmpStat[2]; 
    char cCJ_Acno[20];        /* 拆借帐号 */  
      
      memset(cAcno110,0,sizeof(cAcno110));
      memset(cCJ_Acno,0,sizeof(cCJ_Acno));

    vtcp_log("[%s][%d] 普通类 hv_zf->lw_ind=[%s] ",__FILE__,__LINE__,hv_zf->lw_ind);
       if(hv_zf->lw_ind[0] == '1')   /* 往帐 */
       {
            /* 借： 客户 (挂帐帐号)         */
            /* 贷： 41101                   */
            /* 借： 41101                   */
            /* 贷： 110帐户                   */     

             /*************   挂帐检查  *******************/    
             /*********** 主要是机构、帐号及其状态的检查   ******/
             char cKinbr[BRNO_LEN+1];
             char cOpenbr[BRNO_LEN+1];
                char cAc_no[20];
                memset(cAc_no, 0 , sizeof(cAc_no));
                memset(cKinbr, 0 , sizeof(cKinbr));
                memset(cOpenbr, 0 , sizeof(cOpenbr));
                
          memset(cStat, 0, sizeof(cStat));        
          memset(cTmpStat, 0, sizeof(cTmpStat));                
            set_zd_data("0980","9");     /*  */
          vtcp_log("[%s][%d] ====== 该笔交易是往帐，以下开始走帐 !! ====== ",__FILE__,__LINE__);
            
            MEMCPY_DEBUG(cKinbr, hv_zf->br_no, sizeof(cKinbr)-1);
            GetBr_noByOr_br_no(hv_zf->pay_opn_br_no, cOpenbr);  /* 付款人开户行  */
            
          vtcp_log("[%s][%d] 普通类 hv_zf->cmtno=[%s] ",__FILE__,__LINE__,hv_zf->cmtno);      
            if(memcmp(hv_zf->cmtno,"105",sizeof(hv_zf->cmtno)-1)==0)  /* 同业拆借支付报文 */
            { 
                  ret=pub_base_GetParm_Str(cKinbr,PARM_HVCJ,cCJ_Acno); /* 如果是 105 同业拆借支付报文，在这里设置拆借帐号 */ 
                  if(ret)
                  {
                      vtcp_log("%s,%d 得到[%s]HVCJ参数错误，直接设置科目2620401",__FILE__,__LINE__,cKinbr);
                      MEMCPY_DEBUG(cCJ_Acno,"2620401",7);
                  }
               /* 借： 拆借帐号    */    
                 ret = iHvHang_Check_AcnoNoName(cCJ_Acno, cKinbr); /* 不检查户名 */
              if(ret)
                 {
                        vtcp_log("%s,%d iHvHang_Check_AcnoNoName()  出错 ret=[%d]",__FILE__,__LINE__,ret);
                       sprintf(acErrMsg," iHvHang_Check_AcnoNoName()  出错 !! ");
                       WRITEMSG               
                        return -1;
                 }  
            }
            else /* 非同业拆借支付报文 */
            {
               /* 借： 客户 (挂帐帐号)         */
               
              /* 现金科目的往帐业务要拒绝到挂账科目上 */
              if( (strlen(hv_zf->pay_ac_no)==5 && memcmp(hv_zf->pay_ac_no, "10101", 5 )==0) ||
                    (strlen(hv_zf->pay_ac_no)==7 && memcmp(hv_zf->pay_ac_no,"1010100",7 )==0 ) )
              {
                    ret = iHvHang_Check_Brno(cOpenbr, hv_zf->br_no);  /* 设置好机构 */
                  if(ret)
                    {
                           vtcp_log("%s,%d iHvHang_Check_Brno()  出错 ret=[%d]",__FILE__,__LINE__,ret);
                            sprintf(acErrMsg," iHvHang_Check_Brno()  出错 !! ");
                            WRITEMSG               
                           return -1;
                    }
                  memset(cKinbr, 0 , sizeof(cKinbr));
                  get_zd_data(DC_TX_BR_NO,cKinbr);   
                  vtcp_log("%s,%d 得到的交易机构是[%s]",__FILE__,__LINE__,cKinbr);
                            
                  memset(cAc_no, 0 , sizeof(cAc_no));
                         /* 现金科目的往帐业务要拒绝到挂账科目上 */
                        ret=pub_base_GetParm_Str(cKinbr,PARM_HVGZ,cAc_no);
                        if(ret)
                        {
                            vtcp_log("%s,%d 系统配置错误，得到机构%s的挂账帐号错误[%d]",__FILE__,__LINE__,ret);
                            strncpy(acErrMsg,"得到本机构挂账账号错误",50);
                            set_zd_data("0130",acErrMsg);
                            return ret;
                        }  
                        set_zd_data("0300",cAc_no);      /* 设置好账号 */ 
                        set_zd_data("0980","5");         /* 拒绝 */           

              }else
              {
                    ret = iHvHang_Check_Acno(hv_zf->pay_ac_no, hv_zf->pay_name, cKinbr); /* 设置好 账号 机构 */
                  if(ret)
                    {
                           vtcp_log("%s,%d iHvHang_Check_Acno()  出错 ret=[%d]",__FILE__,__LINE__,ret);
                          sprintf(acErrMsg," iHvHang_Check_Acno()  出错 !! ");
                          WRITEMSG               
                           return -1;
                    }  
              }    
            }

          get_zd_data(DC_TX_BR_NO,cKinbr);   
          vtcp_log("%s,%d 得到的交易机构是[%s]",__FILE__,__LINE__,cKinbr);
          iGetReceTel(cKinbr);
          get_zd_data("0980",cStat);
            vtcp_log("%s,%d 得到cStat====[%s]",__FILE__,__LINE__,cStat);
            if(ZFflg == 1) 
            {
                 hv_zf->hv_sts[0] = cStat[0];  /* 负金额走帐为冲帐，在这里不改变状态 */
                 hv_zf->beg_sts[0] = hv_zf->hv_sts[0]; 
            }

         /* set_zd_data("0070",hv_zf->tel);  */ 
                       
              /* 借： 客户 (挂帐帐号)         */                     
          set_zd_double("0390",hv_zf->tx_amt * ZFflg);  /* 负金额 */
          set_zd_double("0400",hv_zf->tx_amt * ZFflg);  /* 负金额 */
          set_zd_data("0660","1");  /* 设置客户帐方向借贷标志 1-借；2-贷 **/
              /* 贷： 41101                   */
              /* 借： 41101                   */                                      
          set_zd_data("0640",HVWZGD); /* 大额往帐过渡科目 */                     
          set_zd_data("0210","01");                                             
          set_zd_data("0510",hv_zf->cmtno);  
               
              /* 贷： 110帐户                    */                           
          vtcp_log("[%s][%d] hv_zf->br_no===[%s] hv_zf->pay_ac_no===[%s] ",__FILE__,__LINE__,hv_zf->br_no,hv_zf->pay_ac_no);                                         
          memset(cAc_no, 0 , sizeof(cAc_no));
          /*ret=pub_base_GetParm_Str(cKinbr,PARM_HVQS,hv_zf->pay_ac_no);  2006-11-10 18:09*/
          ret=pub_base_GetParm_Str(cKinbr,PARM_HVQS,cAc_no); 
          if(ret)
          {
                  vtcp_log("%s,%d 系统配置错误，得到机构%s的110帐号错误[%d]",__FILE__,__LINE__,ret);
                  return -1;
          }
          set_zd_data("1201",cAc_no);
          set_zd_data("0720","2");  /* 设置110 方向借贷标志  2-贷 */ 
               
          /* J016 J017不走帐  金额置 0           */                                                      
          /* 设置来帐的2440402，2440401帐号和余额，一定为0 否则会走帐 */
          memset(cAc_no, 0 , sizeof(cAc_no));
          vtcp_log("%s,%d 得到的交易机构是[%s]",__FILE__,__LINE__,cKinbr);          
          ret=pub_base_GetParm_Str(cKinbr,PARM_HPQF,cAc_no);
          if(ret)
          {
              vtcp_log("%s,%d 没有找到交易机构指定的汇票签发户",__FILE__,__LINE__);
              return -1;
          }
           vtcp_log("[%s][%d]   cAc_no===[%s]  ",__FILE__,__LINE__,cAc_no);  
          set_zd_data("0320",cAc_no);
          memset(cAc_no, 0 , sizeof(cAc_no));
          ret=pub_base_GetParm_Str(cKinbr,PARM_HPYC,cAc_no);
          if(ret)
          {
              vtcp_log("%s,%d 没有找到交易机构指定的汇票移存户",__FILE__,__LINE__);
              return -1;
          }
          vtcp_log("[%s][%d]   cAc_no===[%s]  ",__FILE__,__LINE__,cAc_no);  
          set_zd_data("0330",cAc_no);
             set_zd_double("0420",0.0);                    
                             
       }
       else if(hv_zf->lw_ind[0] == '2') /*来帐*/
       {
          /* 借： 110 帐户               */
          /* 贷： 41102                  */
          /* 借： 41102                  */
          /* 贷： 客户 挂帐帐号          */   

        /*************   挂帐检查  *******************/    
        /*********** 主要是机构、帐号及其状态的检查   ******/
          char cKinbr[BRNO_LEN+1];
              char cAc_no[20];
              memset(cAc_no, 0 , sizeof(cAc_no));
              memset(cKinbr, 0 , sizeof(cKinbr));
        MEMCPY_DEBUG(cKinbr,hv_zf->br_no,BRNO_LEN); 

        memset(cStat, 0, sizeof(cStat));        
        memset(cTmpStat, 0, sizeof(cTmpStat));                
           set_zd_data("0980","9");     /*  */
        vtcp_log("[%s][%d] ====== 该笔交易是来帐，以下开始设置8583 !! ====== ",__FILE__,__LINE__);
        

        vtcp_log("[%s][%d] 普通类 hv_zf->cmtno=[%s] ",__FILE__,__LINE__,hv_zf->cmtno);      
          if(memcmp(hv_zf->cmtno,"105",sizeof(hv_zf->cmtno)-1)==0)  /* 同业拆借支付报文 */
          { 
                ret=pub_base_GetParm_Str(cKinbr,PARM_HVCJ,cCJ_Acno); /* 如果是 105 同业拆借支付报文，在这里设置拆借帐号 */ 
                if(ret)
                {
                    vtcp_log("%s,%d 得到[%s]HVCJ参数错误，直接设置科目2620401",__FILE__,__LINE__,cKinbr);
                    MEMCPY_DEBUG(cCJ_Acno,"2620401",7);
                }
             /* 借： 拆借帐号    */    
               ret = iHvHang_Check_AcnoNoName(cCJ_Acno, cKinbr); /* 不检查户名 设置好 0030 */
            if(ret)
               {
                      vtcp_log("%s,%d iHvHang_Check_AcnoNoName()  出错 ret=[%d]",__FILE__,__LINE__,ret);
                     sprintf(acErrMsg," iHvHang_Check_AcnoNoName()  出错 !! ");
                     WRITEMSG               
                      return -1;
               }  
          }
          else /* 非同业拆借支付报文 */
          {
             /* 贷： 客户 挂帐帐号          */     
              ret = iHvHang_Check_Acno(hv_zf->cash_ac_no,hv_zf->cash_name,cKinbr); /* 设置好 0030 */
           if(ret)
              {
                     vtcp_log("%s,%d iHvHang_Check_Acno()  出错 ",__FILE__,__LINE__);
                    sprintf(acErrMsg," iHvHang_Check_Acno()  出错 !! ");
                    WRITEMSG               
                     return ret;
              }
           }
           
        get_zd_data(DC_TX_BR_NO,cKinbr);   
        vtcp_log("%s,%d 得到的交易机构是[%s]",__FILE__,__LINE__,cKinbr);
        iGetReceTel(cKinbr);
        get_zd_data("0980",cStat);
          vtcp_log("%s,%d 得到cStat====[%s]",__FILE__,__LINE__,cStat);
          if(ZFflg == 1) 
          {
               hv_zf->hv_sts[0] = cStat[0];  /* 负金额走帐为冲帐，在这里不改变状态 */
               hv_zf->beg_sts[0] = hv_zf->hv_sts[0]; 
          }

                     
        /* set_zd_data("0070",hv_zf->tel);  */ 
        set_zd_data("0660","2");  /* 设置客户帐 借贷标志 1-借；2-贷 **/        
                                     
         /* 借： 110 帐户               */
        memset(cAcno110, 0 , sizeof(cAcno110)); 
        set_zd_double("0390",hv_zf->tx_amt * ZFflg);  /* 负金额 */
        set_zd_double("0400",hv_zf->tx_amt * ZFflg);  /* 负金额 */                    
           set_zd_data("0660","2");
           set_zd_data("0720","1");  /* 设置110  借贷方向标志  1-借 */ 
           ret = pub_base_GetParm_Str(cKinbr,PARM_HVQS,cAcno110);
           if(ret)
           {
               vtcp_log("%s,%d 没有找到交易机构指定的110户",__FILE__,__LINE__);
               return ret;
           }
           
           vtcp_log("%s,%d 找到交易机构指定的110户 cAcno110=[%s]",__FILE__,__LINE__,cAcno110);                        
           set_zd_data("1201",cAcno110);
           set_zd_data("1204","01");
           set_zd_data("1205","2");
  
           /* 贷： 41201                   */
           /* 借： 41201                   */                                      
        set_zd_data("0640",HVLZGD); /* 大额来帐过渡科目 */                     
        set_zd_data("0210","01");                                             
        set_zd_data("0510",hv_zf->cmtno);  

        /* J016 J017不走帐  金额置 0           */                                                      
        /* 设置来帐的2440402，2440401帐号和余额，一定为0 否则会走帐 */
        memset(cAc_no, 0 , sizeof(cAc_no));
        ret=pub_base_GetParm_Str(cKinbr,PARM_HPQF,cAc_no);
        if(ret)
        {
            vtcp_log("%s,%d 没有找到交易机构指定的汇票签发户",__FILE__,__LINE__);
            return -1;
        }
         vtcp_log("[%s][%d]   cAc_no===[%s]  ",__FILE__,__LINE__,cAc_no);  
        set_zd_data("0320",cAc_no);
        memset(cAc_no, 0 , sizeof(cAc_no));
        ret=pub_base_GetParm_Str(cKinbr,PARM_HPYC,cAc_no);
        if(ret)
        {
            vtcp_log("%s,%d 没有找到交易机构指定的汇票移存户",__FILE__,__LINE__);
            return -1;
        }
        vtcp_log("[%s][%d]   cAc_no===[%s]  ",__FILE__,__LINE__,cAc_no);  
        set_zd_data("0330",cAc_no);
        set_zd_double("0420",0.0);  
                              
       }
       else
       {
       sprintf(acErrMsg," 来往标识错误!! ");
       WRITEMSG
       return -1;
       }

    return 0;    
}



/*  按照 hv_chkagemr 插入 hv_zf  */
int iInsHv_zfByHv_chkagemr(struct hv_zf_c*  hv_zf, struct hv_chkagemr_c *hv_chkagemr )
{
      int ret;
   /*     
    memset(hv_zf,0,sizeof(hv_zf));    
  
    vtcp_log("[%s][%d] ============ Hv_chkagemr_Debug() ========\n",__FILE__,__LINE__);    
    Hv_chkagemr_Debug(hv_chkagemr);
    vtcp_log("[%s][%d] ============ Hv_chkagemr_Debug() ========\n",__FILE__,__LINE__);    
    */        
    vtcp_log("[%s][%d] Begin   iInsHv_zfByHv_chkagemr() !!",__FILE__,__LINE__);    
    
/*       
       iCopyHv_zfFromHv_chkagemr(hv_zf,hv_chkagemr);


    vtcp_log("[%s][%d] ============ Hv_zf_Debug() ========\n",__FILE__,__LINE__);    
    Hv_zf_Debug(hv_zf);
    vtcp_log("[%s][%d] ============ Hv_zf_Debug() ========\n",__FILE__,__LINE__);    
*/   

    vtcp_log("[%s][%d] hv_zf.pay_ac_no==[%s]  hv_zf.cash_ac_no==[%s]  ",__FILE__,__LINE__, hv_zf->pay_ac_no,hv_zf->cash_ac_no); 

      ret = Hv_zf_Ins(*(hv_zf), g_pub_tx.reply);
      if(ret)
      {
         vtcp_log("[%s][%d] Hv_zf_Ins() 出错!",__FILE__,__LINE__);
         sprintf(acErrMsg,"数据库操作异常,请与科技科联系![%d]",ret);
         WRITEMSG
            return ret;    
      }
    vtcp_log("[%s][%d] Hv_zf_Ins()  插入成功!",__FILE__,__LINE__);    
      /*    memset(&hv_zfhtr,0,sizeof(hv_zfhtr));
      iCopyHv_zfToHv_zfhtr(&hv_zf,&hv_zfhtr);    */
      ret = Hv_zfhtr_Ins(*(hv_zf), g_pub_tx.reply);
      if(ret)
      {
         vtcp_log("[%s][%d] Hv_zfhtr_Ins() 出错!",__FILE__,__LINE__);
         sprintf(acErrMsg,"数据库操作异常,请与科技科联系![%d]",ret);
         WRITEMSG
           return ret;    
       }        
     vtcp_log("[%s][%d]  Hv_zfhtr_Ins() 插入成功! \n ",__FILE__,__LINE__);    
    return 0;
}

/*  按照 hv_chkagemr 插入 hv_zf  */
int iCopyHv_zfFromHv_chkagemr(struct hv_zf_c*  hv_zf, struct hv_chkagemr_c *hv_chkagemr )
{
      int ret;
      char cTempstr[1024];
       hv_zf->wt_date = hv_chkagemr->tx_date; 
                                 
      MEMCPY_DEBUG(hv_zf->cmtno,hv_chkagemr->cmtno,sizeof(hv_zf->cmtno)-1);       
      MEMCPY_DEBUG(hv_zf->or_br_no,hv_chkagemr->or_br_no,sizeof(hv_zf->or_br_no)-1);    
      MEMCPY_DEBUG(hv_zf->orderno,hv_chkagemr->orderno,sizeof(hv_zf->orderno)-1); 


      /*set_zd_data("0030",hv_chkagemr->br_no);                
       ret = iGetHvOrderno(hv_zf->orderno);
       if(ret)
    {
        vtcp_log("[%s][%d] iGetHvOrderno() 出错!",__FILE__,__LINE__);
          sprintf(acErrMsg,"数据库操作异常,请与科技科联系![%d]",ret);
           WRITEMSG
           return ret;
    }  */    
    vtcp_log("[%s][%d] hv_zf->wt_date=[%d]  ",__FILE__,__LINE__,hv_zf->wt_date); 
    vtcp_log("[%s][%d] hv_zf->cmtno=[%s]  ",__FILE__,__LINE__,hv_zf->cmtno); 
    vtcp_log("[%s][%d] hv_zf->or_br_no=[%s]  ",__FILE__,__LINE__,hv_zf->or_br_no);        
    vtcp_log("[%s][%d] hv_zf->orderno=[%s]  ",__FILE__,__LINE__,hv_zf->orderno); 
                
       hv_zf->tx_date = hv_chkagemr->tx_date;
       hv_zf->tx_time = g_pub_tx.tx_time;             
    hv_zf->tx_amt = hv_chkagemr->tx_amt; 
    hv_zf->o_tx_date = apatoi(hv_chkagemr->o_tx_date,8);   
       hv_zf->o_wt_date = apatoi(hv_chkagemr->ou_tx_date,8);                
    /* hv_zf->hv_sts[0] = '9';   默认为 来账记账清算，挂帐检查时再根据情况重设 */
      /* hv_zf->beg_sts[0] = hv_zf->hv_sts[0];  默认为 来账记账清算，挂帐检查时再根据情况重设 */
      hv_zf->checkflag[0]= 'D'; /* NOT_FOUND_BANK */
      hv_zf->hv_prt_ind[0] = '0';
      get_zd_long(DC_TRACE_NO,&hv_zf->trace_no);

      MEMCPY_DEBUG(hv_zf->cur_no,hv_chkagemr->cur_no,sizeof(hv_zf->cur_no)-1);    
      MEMCPY_DEBUG(hv_zf->note_type,hv_chkagemr->voctype,sizeof(hv_zf->note_type)-1);
      MEMCPY_DEBUG(hv_zf->note_no,hv_chkagemr->vocnum,sizeof(hv_zf->note_no)-1);

      MEMCPY_DEBUG(hv_zf->pay_qs_no,hv_chkagemr->pay_qs_no,sizeof(hv_zf->pay_qs_no)-1);                
      MEMCPY_DEBUG(hv_zf->cash_qs_no,hv_chkagemr->cashqsactno,sizeof(hv_zf->cash_qs_no)-1);
      MEMCPY_DEBUG(hv_zf->ac_br_no,hv_chkagemr->acbrno,sizeof(hv_zf->ac_br_no)-1);
      MEMCPY_DEBUG(hv_zf->or_br_no,hv_chkagemr->or_br_no,sizeof(hv_zf->or_br_no)-1);

      /* 108 退汇支付报文的帐号和用户名特殊处理 */
      if( memcmp(hv_zf->cmtno,"108", 3) == 0)
      {
              /* 来往业务都一样 将'付款人相关项'与'收款人相关项  ' 反过来  */
            MEMCPY_DEBUG(hv_zf->pay_opn_br_no,hv_chkagemr->cashno,sizeof(hv_zf->pay_opn_br_no)-1);
            MEMCPY_DEBUG(hv_zf->pay_ac_no,hv_chkagemr->cash_ac_no,sizeof(hv_zf->pay_ac_no)-1);
            MEMCPY_DEBUG(hv_zf->pay_name,hv_chkagemr->cash_name,sizeof(hv_zf->pay_name)-1);                    
            MEMCPY_DEBUG(hv_zf->pay_addr,hv_chkagemr->cash_addr,sizeof(hv_zf->pay_addr)-1);
      
            MEMCPY_DEBUG(hv_zf->cash_opn_br_no,hv_chkagemr->pay_no,sizeof(hv_zf->cash_opn_br_no)-1);    
            MEMCPY_DEBUG(hv_zf->cash_ac_no,hv_chkagemr->pay_ac_no,sizeof(hv_zf->cash_ac_no)-1);                    
            MEMCPY_DEBUG(hv_zf->cash_name,hv_chkagemr->payname,sizeof(hv_zf->cash_name)-1);
            MEMCPY_DEBUG(hv_zf->cash_addr,hv_chkagemr->payaddress,sizeof(hv_zf->cash_addr)-1);
      }
      else
      {
            MEMCPY_DEBUG(hv_zf->pay_opn_br_no,hv_chkagemr->pay_no,sizeof(hv_zf->pay_opn_br_no)-1);
            MEMCPY_DEBUG(hv_zf->pay_ac_no,hv_chkagemr->pay_ac_no,sizeof(hv_zf->pay_ac_no)-1);
            MEMCPY_DEBUG(hv_zf->pay_name,hv_chkagemr->payname,sizeof(hv_zf->pay_name)-1);                    
            MEMCPY_DEBUG(hv_zf->pay_addr,hv_chkagemr->payaddress,sizeof(hv_zf->pay_addr)-1);
      
            MEMCPY_DEBUG(hv_zf->cash_opn_br_no,hv_chkagemr->cashno,sizeof(hv_zf->cash_opn_br_no)-1);    
            MEMCPY_DEBUG(hv_zf->cash_ac_no,hv_chkagemr->cash_ac_no,sizeof(hv_zf->cash_ac_no)-1);                    
            MEMCPY_DEBUG(hv_zf->cash_name,hv_chkagemr->cash_name,sizeof(hv_zf->cash_name)-1);
            MEMCPY_DEBUG(hv_zf->cash_addr,hv_chkagemr->cash_addr,sizeof(hv_zf->cash_addr)-1);
      }


    vtcp_log("[%s][%d] hv_zf->lw_ind=========[%s]  ",__FILE__,__LINE__, hv_zf->lw_ind); 
    memset(hv_zf->br_no, 0, sizeof(hv_zf->br_no));
    if(hv_zf->lw_ind[0] == '2')  /* 来帐 */
    {                    
      if( GetBr_noByOr_br_no(hv_zf->ac_br_no,hv_zf->br_no))
      {
            memset(hv_zf->br_no, 0, sizeof(hv_zf->br_no));
          /* MEMCPY_DEBUG(hv_zf->br_no,hv_chkagemr->br_no,sizeof(hv_zf->br_no)-1);     */          
      }
    }
    else if(hv_zf->lw_ind[0] == '1')   /* 往帐 */
    {
      if( GetBr_noByOr_br_no(hv_zf->or_br_no,hv_zf->br_no))
      {
            memset(hv_zf->br_no, 0, sizeof(hv_zf->br_no));
          /* MEMCPY_DEBUG(hv_zf->br_no,hv_chkagemr->br_no,sizeof(hv_zf->br_no)-1);       */         
      }          
    }else
    {
      sprintf(acErrMsg,"来往标识错[%s]",hv_zf->lw_ind);
      WRITEMSG
        return ret;
    }
    vtcp_log("[%s][%d] hv_zf->lw_ind=[%s]  ",__FILE__,__LINE__,hv_zf->lw_ind);
   
    vtcp_log("[%s][%d] hv_zf->pay_name==[%.60s] ",__FILE__,__LINE__,hv_zf->pay_name); 
    memset(cTempstr,0,sizeof(cTempstr));
    MEMCPY_DEBUG(cTempstr, hv_zf->pay_name, sizeof(hv_zf->pay_name)-1);  
    dbc_zipspace(cTempstr);  /*压缩全角空格 2006-11-7 23:45  */
    MEMCPY_DEBUG(hv_zf->pay_name, cTempstr, sizeof(hv_zf->pay_name)-1); 
    vtcp_log("[%s][%d] hv_zf->pay_name==[%.60s] ",__FILE__,__LINE__,hv_zf->pay_name);

    vtcp_log("[%s][%d] hv_zf->cash_name==[%.60s] ",__FILE__,__LINE__,hv_zf->cash_name); 
    memset(cTempstr,0,sizeof(cTempstr));
    MEMCPY_DEBUG(cTempstr, hv_zf->cash_name, sizeof(hv_zf->cash_name)-1);  
    dbc_zipspace(cTempstr);  /*压缩全角空格 2006-11-7 23:45  */
    MEMCPY_DEBUG(  hv_zf->cash_name, cTempstr, sizeof(hv_zf->cash_name)-1); 
    vtcp_log("[%s][%d] hv_zf->cash_name==[%.60s] ",__FILE__,__LINE__,hv_zf->cash_name);

    vtcp_log("[%s][%d] hv_zf->ac_br_no=[%s]  ",__FILE__,__LINE__,hv_zf->ac_br_no); 
    vtcp_log("[%s][%d] hv_zf->or_br_no=[%s]  ",__FILE__,__LINE__,hv_zf->or_br_no);     
    vtcp_log("[%s][%d] hv_zf->br_no=[%s]  ",__FILE__,__LINE__,hv_zf->br_no); 
        
      MEMCPY_DEBUG(hv_zf->yw_type,hv_chkagemr->yw_type,sizeof(hv_zf->yw_type)-1);                    
      MEMCPY_DEBUG(hv_zf->sendco,hv_chkagemr->sendco,sizeof(hv_zf->sendco)-1);                    
      MEMCPY_DEBUG(hv_zf->receco,hv_chkagemr->receco,sizeof(hv_zf->receco)-1);        
      
      if(strlen(hv_zf->sendco) < 4)
      {
            iGetSendco(hv_zf->sendco, hv_zf->pay_qs_no);
      }
      
      if(strlen(hv_zf->receco) < 4)
      {
            iGetSendco(hv_zf->receco, hv_zf->cash_qs_no);
      }
              
      /* MEMCPY_DEBUG(hv_zf->operlevel,hv_chkagemr->operlevel,sizeof(hv_zf->operlevel)-1);*/
      MEMCPY_DEBUG(hv_zf->o_orderno,hv_chkagemr->o_orderno_hv,sizeof(hv_zf->o_orderno)-1);                    
      MEMCPY_DEBUG(hv_zf->o_cmtno,hv_chkagemr->o_cmtno,sizeof(hv_zf->o_cmtno)-1);                    
      MEMCPY_DEBUG(hv_zf->hv_brf,hv_chkagemr->reason,sizeof(hv_zf->hv_brf)-1);
      
      /*  ========= 处理汇票类的附加域 ======== */      
      char cPdate[9];
      memset(cPdate,0,sizeof(cPdate));
      apitoa(hv_chkagemr->p_date, 8, cPdate);
      vtcp_log("[%s][%d] cPdate=======[%s] ",__FILE__,__LINE__, cPdate);
      
      if(memcmp(hv_chkagemr->cmtno,"101",3)==0 )
      {
            HV_WTSK_ADD  hv_wtsk;
            memset(&hv_wtsk, 0 ,sizeof(hv_wtsk));
            MEMCPY_DEBUG(hv_wtsk.pj_date, cPdate,sizeof(hv_wtsk.pj_date));
            MEMCPY_DEBUG(hv_wtsk.pj_num, hv_chkagemr->p_num, sizeof(hv_wtsk.pj_num));
            iHvSetAddit(cPdate,cPdate,hv_zf->addid,"000",(char*)&hv_wtsk,sizeof(hv_wtsk));
      }else if(memcmp(hv_chkagemr->cmtno,"102",3)==0 )
      {
            HV_TSCF_ADD hv_tscf; 
            memset(&hv_tscf, 0 ,sizeof(hv_tscf));
            MEMCPY_DEBUG(hv_tscf.pj_date, cPdate,sizeof(hv_tscf.pj_date));
            MEMCPY_DEBUG(hv_tscf.pj_num, hv_chkagemr->p_num, sizeof(hv_tscf.pj_num));
            iHvSetAddit(cPdate,cPdate,hv_zf->addid,"000",(char*)&hv_tscf,sizeof(hv_tscf));
      }else if(memcmp(hv_chkagemr->cmtno,"121",3)==0 )
      {
            HV_121_ADD hv_hp121;
            memset(&hv_hp121, 0 ,sizeof(hv_hp121));
            MEMCPY_DEBUG(hv_hp121.pj_date, cPdate,sizeof(hv_hp121.pj_date));
            MEMCPY_DEBUG(hv_hp121.pj_num, hv_chkagemr->p_num, sizeof(hv_hp121.pj_num));
            iHvSetAddit(cPdate,cPdate,hv_zf->addid,"000",(char*)&hv_hp121,sizeof(hv_hp121));
      }else if(memcmp(hv_chkagemr->cmtno,"122",3)==0 )
      {
            HV_HP_ADD  hv_hp122; 
            memset(&hv_hp122, 0 ,sizeof(hv_hp122));
            MEMCPY_DEBUG(hv_hp122.pj_date, cPdate,sizeof(hv_hp122.pj_date));
            MEMCPY_DEBUG(hv_hp122.pj_num, hv_chkagemr->p_num, sizeof(hv_hp122.pj_num));
            iHvSetAddit(cPdate,cPdate,hv_zf->addid,"000",(char*)&hv_hp122,sizeof(hv_hp122));
      }else if(memcmp(hv_chkagemr->cmtno,"123",3)==0 )
      {
            HV_123_ADD hv_hp123;
            memset(&hv_hp123, 0 ,sizeof(hv_hp123));
            MEMCPY_DEBUG(hv_hp123.pj_date, cPdate,sizeof(hv_hp123.pj_date));
            MEMCPY_DEBUG(hv_hp123.pj_num, hv_chkagemr->p_num, sizeof(hv_hp123.pj_num));
        vtcp_log("[%s][%d]  ========== ", __FILE__,__LINE__);
        vtcp_log("[%s][%d]  ========== ", __FILE__,__LINE__);
        vtcp_log("[%s][%d]  ========== ", __FILE__,__LINE__);
        /* Added by ChenMing   2006-11-22 17:20  */
                dbl2str(&hv_chkagemr->other_amt, 0, sizeof(hv_hp123.real_js_amt)-1,0, hv_hp123.real_js_amt); /*实际结算金额*/
            MEMCPY_DEBUG(hv_hp123.cash_opn_br_no, hv_chkagemr->cashno, sizeof(hv_hp123.cash_opn_br_no));  /*最后持票人开户行*/
        vtcp_log("[%s][%d]  ========== ", __FILE__,__LINE__);
        vtcp_log("[%s][%d]  ========== ", __FILE__,__LINE__);
            MEMCPY_DEBUG(hv_hp123.last_ac_no, hv_chkagemr->cash_ac_no, sizeof(hv_hp123.last_ac_no));    /*最后持票人帐号*/              
            MEMCPY_DEBUG(hv_hp123.last_name, hv_chkagemr->cash_name, sizeof(hv_hp123.last_name));        /*最后持票人姓名*/
        vtcp_log("[%s][%d]  ========== ", __FILE__,__LINE__);
        vtcp_log("[%s][%d]  ========== ", __FILE__,__LINE__);
            apitoa(hv_chkagemr->pay_date, sizeof(hv_hp123.return_date), hv_hp123.return_date);  /*提示付款日期*/       
        vtcp_log("[%s][%d]  ========== ", __FILE__,__LINE__);
        vtcp_log("[%s][%d]  ========== ", __FILE__,__LINE__);
             
            iHvSetAddit(cPdate,cPdate,hv_zf->addid,"000",(char*)&hv_hp123,sizeof(hv_hp123));
        vtcp_log("[%s][%d]  ========== ", __FILE__,__LINE__);
        vtcp_log("[%s][%d]  ========== ", __FILE__,__LINE__);
        vtcp_log("[%s][%d]  ========== ", __FILE__,__LINE__);
      }else if(memcmp(hv_chkagemr->cmtno,"124",3)==0 )
      {
            HV_124_ADD hv_hp124;
            memset(&hv_hp124, 0 ,sizeof(hv_hp124));
            MEMCPY_DEBUG(hv_hp124.pj_date, cPdate,sizeof(hv_hp124.pj_date));
            MEMCPY_DEBUG(hv_hp124.pj_num, hv_chkagemr->p_num, sizeof(hv_hp124.pj_num));
            iHvSetAddit(cPdate,cPdate,hv_zf->addid,"000",(char*)&hv_hp124,sizeof(hv_hp124));
      }

      vtcp_log("[%s][%d] hv_zf->addid=======[%s] ",__FILE__,__LINE__, hv_zf->addid);
   return 0;
}



/*
int iCopyHv_zfToHv_zfhtr(struct hv_zf_c *hv_zf, struct hv_zfhtr_c *hv_zfhtr)
{
    
    hv_zfhtr->tx_date = hv_zf->tx_date;
    hv_zfhtr->wt_date = hv_zf->wt_date;    
    hv_zfhtr->tx_time = hv_zf->tx_time;    
    
    hv_zfhtr->tx_amt = hv_zf->tx_amt;    
    hv_zfhtr->chrg_amt = hv_zf->chrg_amt;        
    
    
    MEMCPY_DEBUG(hv_zfhtr_c->br_no ,hv_zf->br_no , sizeof(hv_zfhtr_c->br_no)-1);
    
    MEMCPY_DEBUG(hv_zfhtr_c->orderno ,hv_zf->orderno , sizeof(hv_zfhtr_c->orderno)-1);    
    MEMCPY_DEBUG(hv_zfhtr_c->cur_no ,hv_zf->cur_no , sizeof(hv_zfhtr_c->cur_no)-1);    
    MEMCPY_DEBUG(hv_zfhtr_c->note_type ,hv_zf->note_type , sizeof(hv_zfhtr_c->note_type)-1);    
    MEMCPY_DEBUG(hv_zfhtr_c->note_no ,hv_zf->note_no , sizeof(hv_zfhtr_c->note_no)-1);
    
    MEMCPY_DEBUG(hv_zfhtr_c->tx_chrg_ind ,hv_zf->tx_chrg_ind , sizeof(hv_zfhtr_c->tx_chrg_ind)-1);    
    MEMCPY_DEBUG(hv_zfhtr_c->pay_qs_no ,hv_zf->pay_qs_no , sizeof(hv_zfhtr_c->pay_qs_no)-1);    
    MEMCPY_DEBUG(hv_zfhtr_c->or_br_no ,hv_zf->or_br_no , sizeof(hv_zfhtr_c->or_br_no)-1);    
    MEMCPY_DEBUG(hv_zfhtr_c->pay_opn_br_no ,hv_zf->pay_opn_br_no , sizeof(hv_zfhtr_c->pay_opn_br_no)-1);
    
    MEMCPY_DEBUG(hv_zfhtr_c->pay_ac_no ,hv_zf->pay_ac_no , sizeof(hv_zfhtr_c->pay_ac_no)-1);    
    MEMCPY_DEBUG(hv_zfhtr_c->pay_name ,hv_zf->pay_name , sizeof(hv_zfhtr_c->pay_name)-1);    
    MEMCPY_DEBUG(hv_zfhtr_c->pay_addr ,hv_zf->pay_addr , sizeof(hv_zfhtr_c->pay_addr)-1);    
    MEMCPY_DEBUG(hv_zfhtr_c->cash_qs_no ,hv_zf->cash_qs_no , sizeof(hv_zfhtr_c->cash_qs_no)-1);
    MEMCPY_DEBUG(hv_zfhtr_c->ac_br_no ,hv_zf->ac_br_no , sizeof(hv_zfhtr_c->ac_br_no)-1);        
        

    MEMCPY_DEBUG(hv_zfhtr_c->pay_name ,hv_zf->pay_name , sizeof(hv_zfhtr_c->pay_name)-1);    
    MEMCPY_DEBUG(hv_zfhtr_c->pay_addr ,hv_zf->pay_addr , sizeof(hv_zfhtr_c->pay_addr)-1);    
    MEMCPY_DEBUG(hv_zfhtr_c->cash_qs_no ,hv_zf->cash_qs_no , sizeof(hv_zfhtr_c->cash_qs_no)-1);
    
            
        
        
        
    
    return 0;
}*/


int iInsertHv_chkerror(struct hv_zf_c *hv_zf, struct hv_chkagemr_c *hv_chkagemr)
{
      int ret;
      struct hv_chkerror_c hv_chkerror;
      memset(&hv_chkerror,0,sizeof(hv_chkerror));
      
    vtcp_log("[%s][%d] iInsertHv_chkerror () 开始 !!!",__FILE__,__LINE__);

    vtcp_log("[%s][%d] hv_zf->tx_date=[%d] ",__FILE__,__LINE__,hv_zf->tx_date);
    vtcp_log("[%s][%d] hv_zf->orderno=[%s] ",__FILE__,__LINE__,hv_zf->orderno);
    vtcp_log("[%s][%d] hv_zf->or_br_no=[%s] ",__FILE__,__LINE__,hv_zf->or_br_no);
    vtcp_log("[%s][%d] hv_zf->cmtno=[%s] ",__FILE__,__LINE__,hv_zf->cmtno);
      
      hv_chkerror.tx_date = hv_zf->tx_date;
      MEMCPY_DEBUG(hv_chkerror.orderno,hv_zf->orderno,sizeof(hv_chkerror.orderno)-1 );
      MEMCPY_DEBUG(hv_chkerror.or_br_no,hv_zf->or_br_no,sizeof(hv_chkerror.or_br_no)-1 );
      MEMCPY_DEBUG(hv_chkerror.cmtno,hv_chkagemr->cmtno,sizeof(hv_chkerror.cmtno)-1 );
      
      hv_chkerror.tx_time = hv_chkagemr->tx_time;    
      hv_chkerror.tx_amt = hv_chkagemr->tx_amt;
      hv_chkerror.tx_amt_ccb = hv_zf->tx_amt;    
      hv_chkerror.tx_amt_ccpc = hv_chkagemr->tx_amt;          
        
      MEMCPY_DEBUG(hv_chkerror.br_no,hv_chkagemr->br_no,sizeof(hv_chkerror.br_no)-1 );
      MEMCPY_DEBUG(hv_chkerror.pay_qs_no,hv_chkagemr->pay_qs_no,sizeof(hv_chkerror.pay_qs_no)-1 );
      MEMCPY_DEBUG(hv_chkerror.o_sts,hv_zf->hv_sts,sizeof(hv_chkerror.o_sts)-1 );
      MEMCPY_DEBUG(hv_chkerror.sts,hv_zf->hv_sts,sizeof(hv_chkerror.sts)-1 );
      MEMCPY_DEBUG(hv_chkerror.chk_flg,hv_chkagemr->chk_flg,sizeof(hv_chkerror.chk_flg)-1 );
      MEMCPY_DEBUG(hv_chkerror.title,"",sizeof(hv_chkerror.title)-1 );
      MEMCPY_DEBUG(hv_chkerror.prt_flag,"0",sizeof(hv_chkerror.prt_flag)-1 );
      MEMCPY_DEBUG(hv_chkerror.brf,hv_chkagemr->brf1,sizeof(hv_chkerror.brf)-1 );
    
    ret = Hv_chkerror_Ins(hv_chkerror,g_pub_tx.reply);
      if(ret)
      {
         vtcp_log("[%s][%d] Hv_chkerror_Ins() 出错!",__FILE__,__LINE__);
         sprintf(acErrMsg,"数据库操作异常,请与科技科联系![%d]",ret);
         WRITEMSG
            return ret;    
      }
    vtcp_log("[%s][%d] Hv_chkerror_Ins()  插入成功!",__FILE__,__LINE__);    
      return 0;
}


/*  汇票类 DHCC多帐 需要更新汇票的状态   */
int iHv_DZ_ChangeHPStat(struct hv_zf_c * hv_zf_wz, int iCmtno)
{
        HV_121_ADD hv_hp121;
    HV_HP_ADD  hv_hp122; 
    HV_123_ADD hv_hp123;
        HV_124_ADD hv_hp124;
        char cTx_date[9];
        char cWt_date[9];
        char cLast_sts;
        
      int ret=0;
      struct hv_poinfo_c hv_poinfo;
      memset(&hv_poinfo, 0 , sizeof(hv_poinfo));
      memset(&hv_hp121, 0 ,sizeof(hv_hp121));
      memset(&hv_hp122, 0 ,sizeof(hv_hp122));
      memset(&hv_hp123, 0 ,sizeof(hv_hp123));
      memset(&hv_hp124, 0 ,sizeof(hv_hp124));
      memset(cTx_date, 0 ,sizeof(cTx_date));      
      memset(cWt_date, 0 ,sizeof(cWt_date));
      apitoa(hv_zf_wz->tx_date,sizeof(cTx_date)-1, cTx_date);
      apitoa(hv_zf_wz->wt_date,sizeof(cWt_date)-1, cWt_date);
            
    if( iCmtno==121 )
    {
          /*得到附加域 */
          ret=iHvGetAddit(cTx_date,cWt_date,hv_zf_wz->addid,"000",(char*)&hv_hp121,sizeof(hv_hp121));
          if(ret)
          {
              vtcp_log("%s,%d 读取附加域错误 %d",__FILE__,__LINE__,ret);
              WRITEMSG
              return -1;
          }
        vtcp_log("[%s,%d] hv_hp121.pj_num===[%s]",__FILE__,__LINE__,hv_hp124.pj_num);
        vtcp_log("[%s,%d] hv_poinfo.po_no===[%s]",__FILE__,__LINE__,hv_poinfo.po_no);
        vtcp_log("[%s,%d] sizeof(hv_poinfo.po_no)===[%d]",__FILE__,__LINE__, sizeof(hv_poinfo.po_no));
          
          MEMCPY_DEBUG(hv_poinfo.po_no,hv_hp121.pj_num,HV_POLEN);
          MEMCPY_DEBUG(hv_poinfo.pay_br_no,hv_zf_wz->pay_opn_br_no,sizeof(hv_poinfo.pay_br_no)-1);
        vtcp_log("[%s,%d] hv_hp121.pj_num===[%s]",__FILE__,__LINE__,hv_hp124.pj_num);
        vtcp_log("[%s,%d] hv_poinfo.po_no===[%s]",__FILE__,__LINE__,hv_poinfo.po_no);
          
          
    }else if( iCmtno==122 )
    {
          /*得到附加域 */
          ret=iHvGetAddit(cTx_date,cWt_date,hv_zf_wz->addid,"000",(char*)&hv_hp122,sizeof(hv_hp122));
          if(ret)
          {
              vtcp_log("%s,%d 读取附加域错误 %d",__FILE__,__LINE__,ret);
              WRITEMSG
              return -1;
          }
        vtcp_log("[%s,%d] hv_hp122.pj_num===[%s]",__FILE__,__LINE__,hv_hp124.pj_num);
        vtcp_log("[%s,%d] hv_poinfo.po_no===[%s]",__FILE__,__LINE__,hv_poinfo.po_no);
          
          MEMCPY_DEBUG(hv_poinfo.po_no,hv_hp122.pj_num,HV_POLEN);
          MEMCPY_DEBUG(hv_poinfo.pay_br_no,hv_zf_wz->pay_opn_br_no,sizeof(hv_poinfo.pay_br_no)-1);

        vtcp_log("[%s,%d] hv_hp122.pj_num===[%s]",__FILE__,__LINE__,hv_hp124.pj_num);
        vtcp_log("[%s,%d] hv_poinfo.po_no===[%s]",__FILE__,__LINE__,hv_poinfo.po_no);

    }else if( iCmtno==123 )
    {
          /*得到附加域 */
          ret=iHvGetAddit(cTx_date,cWt_date,hv_zf_wz->addid,"000",(char*)&hv_hp123,sizeof(hv_hp123));
          if(ret)
          {
              vtcp_log("%s,%d 读取附加域错误 %d",__FILE__,__LINE__,ret);
              WRITEMSG
              return -1;
          }
        vtcp_log("[%s,%d] hv_hp123.pj_num===[%s]",__FILE__,__LINE__,hv_hp124.pj_num);
        vtcp_log("[%s,%d] hv_poinfo.po_no===[%s]",__FILE__,__LINE__,hv_poinfo.po_no);
          
          MEMCPY_DEBUG(hv_poinfo.po_no,hv_hp123.pj_num, HV_POLEN);
          MEMCPY_DEBUG(hv_poinfo.pay_br_no,hv_zf_wz->pay_opn_br_no,sizeof(hv_poinfo.pay_br_no)-1);    

        vtcp_log("[%s,%d] hv_hp123.pj_num===[%s]",__FILE__,__LINE__,hv_hp124.pj_num);
        vtcp_log("[%s,%d] hv_poinfo.po_no===[%s]",__FILE__,__LINE__,hv_poinfo.po_no);

    }else if( iCmtno==124 )
    {
          /*得到附加域 */
          ret=iHvGetAddit(cTx_date,cWt_date,hv_zf_wz->addid,"000",(char*)&hv_hp124,sizeof(hv_hp124));
          if(ret)
          {
              vtcp_log("%s,%d 读取附加域错误 %d",__FILE__,__LINE__,ret);
              WRITEMSG
              return -1;
          }
        vtcp_log("[%s,%d] hv_hp124.pj_num===[%s]",__FILE__,__LINE__,hv_hp124.pj_num);
        vtcp_log("[%s,%d] hv_poinfo.po_no===[%s]",__FILE__,__LINE__,hv_poinfo.po_no);
          
          MEMCPY_DEBUG(hv_poinfo.po_no,hv_hp124.pj_num, HV_POLEN);
          MEMCPY_DEBUG(hv_poinfo.pay_br_no,hv_zf_wz->pay_opn_br_no,sizeof(hv_poinfo.pay_br_no)-1);

        vtcp_log("[%s,%d] hv_hp124.pj_num===[%s]",__FILE__,__LINE__,hv_hp124.pj_num);
        vtcp_log("[%s,%d] hv_poinfo.po_no===[%s]",__FILE__,__LINE__,hv_poinfo.po_no);

    }else
    {
          vtcp_log("[%s,%d] iCmtno 错误!!!  iCmtno===[%d]",__FILE__,__LINE__,iCmtno);
          WRITEMSG
          return -1;    
    }

      ret = Hv_poinfo_Dec_Upd(g_pub_tx.reply,"po_no='%s'  and  pay_br_no='%s'",hv_poinfo.po_no,hv_poinfo.pay_br_no);
      if(ret)
      {
          sprintf(acErrMsg,"申明游标错误!ret=[%d]\n",ret);
          strcpy(g_pub_tx.reply,"P001");
          WRITEMSG
          return -1;;
      }
      memset(&hv_poinfo, 0 , sizeof(hv_poinfo));      
      ret = Hv_poinfo_Fet_Upd(&hv_poinfo,g_pub_tx.reply);
      if(ret&&ret!=100)
      {
          sprintf(acErrMsg,"取游标错误!ret=[%d]\n",ret);
          strcpy(g_pub_tx.reply,"P001");
          WRITEMSG
          return -1;
      }
      else if(ret==100)
      {
          sprintf(acErrMsg,"查询的记录不存在!ret=[%d]\n",ret);
          WRITEMSG
          strcpy(g_pub_tx.reply,"P001");
          /* return -1;   Remed by ChenMing   2006-11-5 23:43
          因为机构可能找不到，或者不对，所以，如果没找到汇票记录这里不报错 */
      }
      
      cLast_sts = hv_poinfo.po_sts[0];
      
    if( iCmtno==121 )
    {
        hv_poinfo.po_sts[0] = hv_poinfo.l_po_sts[0];   /* 置回原状态 */
        hv_poinfo.l_po_sts[0] = cLast_sts;
        
    }
    /* else if( iCmtno==122 )
    {
         if( hv_poinfo.po_sts[0] == '4')   汇票兑付
         {
                hv_poinfo.po_sts[0] = hv_poinfo.l_po_sts[0];  
         }
    }else if( iCmtno==123 )
    {
         if( hv_poinfo.po_sts[0] == '6')   部分退回 
         {
                hv_poinfo.po_sts[0] = hv_poinfo.l_po_sts[0];  
         }
    }else if( iCmtno==124 )
    {
         if( hv_poinfo.po_sts[0] == '7')   未用退回 
         {
                hv_poinfo.po_sts[0] = hv_poinfo.l_po_sts[0];  
         }
    }*/

      ret = Hv_poinfo_Upd_Upd(hv_poinfo,g_pub_tx.reply);
      if(ret)
      {
          sprintf(acErrMsg,"执行函数hv_poinfo_upd_upd错误!ret==[%d]\n",ret);
          WRITEMSG
          strcpy(g_pub_tx.reply,"D105");
          return (-1);
      }
      
      Hv_poinfo_Clo_Upd();
      return 0;
}


/* 汇票类 DHCC少帐  */
/* dCash_amt 实际结算金额  dRem_amt  剩余金额 */
int iHv_SZ_ChangeHPStat( char* cPay_br_no, char* cPo_no, double dCash_amt, double dRem_amt, int iCmtno )
{
    int ret;
    char cLast_sts;
    struct hv_poinfo_c hv_poinfo;

    vtcp_log("%s,%d iCmtno    ===[%d]",__FILE__,__LINE__,iCmtno);  
    vtcp_log("%s,%d cPay_br_no===[%s]",__FILE__,__LINE__,cPay_br_no);    
    vtcp_log("%s,%d cPo_no    ===[%s]",__FILE__,__LINE__,cPo_no);        
    vtcp_log("[%s][%d]   dCash_amt===[%lf]  ",__FILE__,__LINE__, dCash_amt);             
    vtcp_log("[%s][%d]   dRem_amt===[%lf]  ",__FILE__,__LINE__, dRem_amt); 
               
      ret = Hv_poinfo_Dec_Upd(g_pub_tx.reply,"po_no='%s'  and  pay_br_no='%s'",cPo_no, cPay_br_no);
      if(ret)
      {
          sprintf(acErrMsg,"申明游标错误!ret=[%d]\n",ret);
          strcpy(g_pub_tx.reply,"P001");
          WRITEMSG
          return -1;;
      }
      memset(&hv_poinfo, 0 , sizeof(hv_poinfo));      
      ret = Hv_poinfo_Fet_Upd(&hv_poinfo,g_pub_tx.reply);
      if(ret&&ret!=100)
      {
          sprintf(acErrMsg,"取游标错误!ret=[%d]\n",ret);
          strcpy(g_pub_tx.reply,"P001");
          WRITEMSG
          return -1;
      }
      else if(ret==100)
      {
          sprintf(acErrMsg,"查询的记录不存在!ret=[%d]\n",ret);
          WRITEMSG
          strcpy(g_pub_tx.reply,"P001");
          return 0;
          /* return -1;   Remed by ChenMing   2006-11-5 23:43
          因为机构可能找不到，或者不对，所以，如果没找到汇票记录这里不报错 */
      }
      
    vtcp_log("[%s][%d] hv_poinfo.po_sts========[%s] ",__FILE__,__LINE__,hv_poinfo.po_sts);
    vtcp_log("[%s][%d] hv_poinfo.l_po_sts======[%s] ",__FILE__,__LINE__,hv_poinfo.l_po_sts);
        
    cLast_sts = hv_poinfo.po_sts[0];  /* 汇票的上一次状态 */
    hv_poinfo.l_tx_date = g_pub_tx.tx_date;  
    hv_poinfo.td_cnt ++;   

    vtcp_log("[%s][%d] iCmtno========[%d] ",__FILE__,__LINE__,iCmtno); 

    /* 少帐，根据不同情况设置回汇票的标志 */
    if( iCmtno==121 )
    {
        /*
        if( hv_poinfo.po_sts[0] == HV_HP_SIGN )  
        {
           hv_poinfo.po_sts[0] =  HV_HP_MV_CENTER; 资金移存记帐 
           hv_poinfo.l_po_sts[0] = cLast_sts;
        }
        else
        {
            Hv_poinfo_Clo_Upd();  
            sprintf(acErrMsg," 汇票记录当前状态不对 ! hv_poinfo.po_sts=[%s] ",hv_poinfo.po_sts);
            WRITEMSG
            return -1;
        } 
     */
    }else if( iCmtno==122 )
    {
        if(hv_poinfo.po_sts[0] == HV_HP_JQ) /* 725先收到 已全额结清 */
        {
            vtcp_log(" [%s][%d] 已全额结清 725先收到 不用更新了!  ",__FILE__,__LINE__); 
            Hv_poinfo_Clo_Upd();  
            return 0; 
        }
        else if( hv_poinfo.po_sts[0] == HV_HP_ASKCASH || 
                   hv_poinfo.po_sts[0] == HV_HP_UNLOSS ||
                   hv_poinfo.po_sts[0] == HV_HP_MV_CENTER ||
                   hv_poinfo.po_sts[0] == HV_HP_LOSS )  /* 申请兑付 */  
        {
            hv_poinfo.po_sts[0] = HV_HP_CASH;           /* 汇票兑付 */
            hv_poinfo.l_po_sts[0] = cLast_sts;
        }
        else
        {
            Hv_poinfo_Clo_Upd();  
            sprintf(acErrMsg," 汇票记录当前状态不对 ! hv_poinfo.po_sts=[%s] ",hv_poinfo.po_sts);
            WRITEMSG
            return -1;
        }    
    }else if( iCmtno == 123 )
    {
       /* if( hv_poinfo.po_sts[0] == HV_HP_ASKCASH || 
              hv_poinfo.po_sts[0] == HV_HP_CASH ||
              hv_poinfo.po_sts[0] == HV_HP_LOSS || 
              hv_poinfo.po_sts[0] == HV_HP_MV_CENTER ||
              hv_poinfo.po_sts[0] == HV_HP_UNLOSS ||
            hv_poinfo.po_sts[0] == HV_HP_ASKRETURN  )  
        {  123  124  老刘说不用检查状态了  2006-11-22 17:52  */
           hv_poinfo.po_sts[0] =  HV_HP_PARTBACK;   /* 部分退回 */
           hv_poinfo.l_po_sts[0] = cLast_sts;
           hv_poinfo.cash_amt = dCash_amt ;   /* 实际结算金额 */           
           hv_poinfo.rem_amt = dRem_amt;   /* 退回金额 */ 
        /*}
        else
        {
            Hv_poinfo_Clo_Upd();  
            sprintf(acErrMsg," 汇票记录当前状态不对 ! hv_poinfo.po_sts=[%s] ",hv_poinfo.po_sts);
            WRITEMSG
            return -1;
        } 123  124  老刘说不用检查状态了  2006-11-22 17:52   */
    }else if( iCmtno==124 )
    {
         long iPay_date;
         vtcp_log("[%s][%d]   hv_poinfo.sign_date===[%d]  ",__FILE__,__LINE__, hv_poinfo.sign_date);      
         vtcp_log("[%s][%d]   iPay_date===[%d]  ",__FILE__,__LINE__, iPay_date);      
         pub_base_deadlineM(hv_poinfo.sign_date,1,&iPay_date);
         vtcp_log("[%s][%d]   g_pub_tx.tx_date===[%d]  ",__FILE__,__LINE__, g_pub_tx.tx_date);      
         vtcp_log("[%s][%d]   iPay_date===[%d]  ",__FILE__,__LINE__, iPay_date);      
         if(iPay_date < g_pub_tx.tx_date)
         {
            hv_poinfo.po_sts[0] = HV_HP_OVERDATE_UNTREAD;  /* 逾期未用退回 */
            hv_poinfo.l_po_sts[0] = cLast_sts;
         }
         else
         {
            hv_poinfo.po_sts[0] = HV_HP_FULLBACK;  /* 未用退回 */
            hv_poinfo.l_po_sts[0] = cLast_sts;
         }  
    }          
    else
    {
        sprintf(acErrMsg,"iCmtno== [%d] 不在这里处理! ",iCmtno);
        WRITEMSG
        return -1;
    }     
 
    vtcp_log("[%s][%d] hv_poinfo.po_sts========[%s] ",__FILE__,__LINE__,hv_poinfo.po_sts);
    vtcp_log("[%s][%d] hv_poinfo.l_po_sts======[%s] ",__FILE__,__LINE__,hv_poinfo.l_po_sts);

      ret = Hv_poinfo_Upd_Upd(hv_poinfo,g_pub_tx.reply);
      if(ret)
      {
          sprintf(acErrMsg,"执行函数hv_poinfo_upd_upd错误!ret==[%d]\n",ret);
          WRITEMSG
          strcpy(g_pub_tx.reply,"D105");
          return (-1);
      }
      
      Hv_poinfo_Clo_Upd();
      return 0;
}
