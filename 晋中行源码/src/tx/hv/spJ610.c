/****************************************************************************
* 文 件 名:  spJ610.c   对应老系统 V94.pc
* 功能描述： 把城市商业银行当天大额支付业务和ccpc发来的核对报文进行核对
*             核对成功返回成功标志,失败列出双方明细
* 作    者:  ChenMing 
* 完成日期： 2006年8月23日
*
* 修改记录： 
* 日    期:  2010-6-20 11:04:41
* 修 改 人:  xyy 
* 修改内容:  注意在hv_orno_brno中把9999开头的机构分给了村镇银行
*****************************************************************************/
#define EXTERN
#include <string.h>
#include <stdio.h>
#include "hv_define.h"
#include "hv_pub.h"
#include "hvnet.h"
#include "hvpack.h"
#include "hv_chkmr_c.h"
#include "hv_chkagemr_c.h"
#include "hv_zf_c.h"
#include "hvstat_c.h"
#include "hv_orno_brno_c.h"
#include "find_debug.h"

#include "syyh_czyh_qs_reg_c.h"
static int ret=0;
static char cStat;
static int iHv_date=0;
void vHv_chkmr_to_buf(struct hv_chkmr_c * pChkmr,char * buf);
void vHv_chkagemr_to_buf(struct hv_chkagemr_c * pChkagemr,char * buf);
  
int spJ610()
{
    struct hv_orno_brno_c sHv_orno_brno;
    
    memset(&sHv_orno_brno , 0 , sizeof(sHv_orno_brno));
    
    vtcp_log("spJ610 Start !   [%s][%d]",__FILE__,__LINE__);
    int ret=0;
    pub_base_sysinit();
    
    iHv_date = hv_getday();
    vtcp_log("[%s][%d]  iHv_date ===[%d]",__FILE__,__LINE__,iHv_date);       
    set_zd_long("0510",0);     
    ret = spJ610_Check();
    if(ret == 999) /* 检查未通过 不报错!*/
    {
        goto OkExit;  
    }       
    if(ret)  /* 对帐前检查 */
    {
        sprintf(acErrMsg," spJ610_Check() 对帐前检查 出错 !! ");
        WRITEMSG 
        goto ErrExit;  
    }
    vtcp_log("spJ610_Check Successed !   [%s][%d]",__FILE__,__LINE__);  
    /* begin add by LiuHuafeng 2009-5-14 14:55:22 for 村镇银行 */
    /* 如果是村镇银行，需要处理商行发来的对账文件的导入 */
    if((char*)getenv("CZYH")!=NULL && strncmp((char*)getenv("CZYH"),"YES_CZYH",8)==0)
    {
        vtcp_log("[%s][%d] 走到了村镇银行的分支!",__FILE__,__LINE__);
    }
    else
    {
        vtcp_log("[%s][%d] 准备循环导入村镇银行的往帐!!!",__FILE__,__LINE__);
        ret=Hv_orno_brno_Dec_Sel( g_pub_tx.reply, " br_no like '9999_' ");
        while(1)
        {
             memset(&sHv_orno_brno , 0 , sizeof(sHv_orno_brno));
             ret=Hv_orno_brno_Fet_Sel( &sHv_orno_brno,g_pub_tx.reply );
             if( ret==100 ) break;
             if( ret )
             {
                  sprintf(acErrMsg," 查找村镇银行机构 出错 !! ");
                  WRITEMSG 
                  goto ErrExit;  
             }
             /* 商业银行要导入村镇银行发来的往帐数据 modify by xyy 2010-6-20 11:01:07*/
             vtcp_log("[%s][%d] 商行需要处理村镇银行[%s]发来的往账数据",__FILE__,__LINE__,sHv_orno_brno.br_no);
             ret = pubf_com_hv_read_dz(sHv_orno_brno.br_no);
             if(ret)
             {
                 vtcp_log("%s,%d 导入上送往帐数据失败，ret = %ld",__FILE__,__LINE__,ret);
                 strncpy(g_pub_tx.reply,"P157",4);
                 set_zd_data("0130","没有收到村镇银行对账文件不能对账");
                 goto ErrExit;
             }
             vtcp_log("[%s][%d] [%s]村镇银行导入完毕!!",__FILE__,__LINE__,sHv_orno_brno.br_no);
        }
        Hv_orno_brno_Clo_Sel();

        
    }
    /* end by LiuHuafeng 2009-5-14 15:00:46 */
    if( spJ610_Process() )  
    {
        sprintf(acErrMsg," spJ610_Process()  大额对总帐处理 出错 !! ");
        WRITEMSG       
        goto ErrExit;  
    }       
    vtcp_log("spJ610_Process() Successed !   [%s][%d]",__FILE__,__LINE__);  

     
  OkExit:
       strcpy(g_pub_tx.reply,"0000");
       sprintf(acErrMsg,"大额支付申请明细下载及核对总帐结束![%s]",g_pub_tx.reply);
       WRITEMSG
       set_zd_data(DC_REPLY,g_pub_tx.reply);
       return 0;
     
  ErrExit:
        if(memcmp(g_pub_tx.reply,"0000",4)==0)
                MEMCPY_DEBUG(g_pub_tx.reply,"T063",4);
       sprintf(acErrMsg,"大额支付申请明细下载及核对总帐出错![%s]",g_pub_tx.reply);
       WRITEMSG
       set_zd_data(DC_REPLY,"0020");
       return 1;
}




/*******   对帐前检查    *********************  
     检查内容：
           1.处于日终状态
           2.收到总计核对报文,且状态不为对帐成功
           3.没有处于复核状态的业务  
***********************************************/
           
int spJ610_Check()
{

  int  nCheckmrCnt = 0;
  struct hv_chkmr_c hv_chkmr;
  
  /* 1. 检查是否处于日终状态  */
  cStat = hv_getstat();  
  vtcp_log("[%s][%d]=======cStat = [%c]",__FILE__,__LINE__,cStat);
  
  /*cStat = HV_STAT_WORKEND;  For Test   */
  
  if( cStat != HV_STAT_WORKEND && cStat != HV_STAT_CHECKERR && cStat != HV_STAT_CHECKRIGHT)
  {
      sprintf(acErrMsg,"[%s] [%d] 当前状态下不允许做此交易[%c]\n",__FILE__,__LINE__,cStat);
      WRITEMSG
      set_zd_long("0510",5);
      return 999;      
  }
  vtcp_log("[%s][%d] 1.  处于日终状态 ========   ",__FILE__,__LINE__);  
  
  /* 2. 检查是否收到总计核对报文,且状态不为对帐成功  Rem for Test */
  ret = Hv_chkmr_Sel(g_pub_tx.reply,&hv_chkmr," tx_date=%d ",iHv_date );
  if( ret == 100 )
  {
      sprintf(acErrMsg,"[%s] [%d] 尚未收到核对报文，不能做此交易!\n",__FILE__,__LINE__);
      WRITEMSG
      set_zd_long("0510",6);
      return 999;          
  }
  else if( ret )  
  {
      sprintf(acErrMsg,"[%s] [%d]查询大额业务核对主表出错！ \n",__FILE__,__LINE__);
      WRITEMSG
      return -1;
  }   
  vtcp_log("[%s][%d]  2.  收到总计核对报文 ======= ",__FILE__,__LINE__);  

  /* 检查异常发送数据     还需要吗 ？ chenming 2006-10-12 14:15
  if(iCheckSendRollback()) 
  {
      sprintf(acErrMsg,"[%s] [%d] 检查异常发送数据发生错误\n",__FILE__,__LINE__);
      WRITEMSG
      return -1;      
  }  */
  
  /* 3.  检查是否存在已经复核但未发送的支付交易  Rem for Test  */ 
  if(hv_hasnotsendtx())  
  {
      sprintf(acErrMsg,"[%s] [%d] 存在未发送的支付交易，不能做此交易 \n",__FILE__,__LINE__);
      WRITEMSG
      set_zd_long("0510",7);
      return 999;      
  }  
  vtcp_log("[%s][%d]  3. 不存在已经复核但未发送的支付交易 ======= ",__FILE__,__LINE__);  
  
  return 0;
}


/*检查异常发送数据  ？？ */
int iCheckSendRollback()
{
  
  return 0;
  
}


/*检查是否存在已经复核但未发送的支付交易*/
int hv_hasnotsendtx()
{
  struct  hv_zf_c hv_zf_c;
  int ret;
  
  memset(&hv_zf_c,'\0',sizeof(struct  hv_zf_c));  
  ret = Hv_zf_Sel(g_pub_tx.reply,&hv_zf_c,"tx_date=%d and hv_sts= '%c' ",iHv_date,'0'); /* 0: 往账登记记账*/
  if ( ret == 100 )
  {
      vtcp_log("[%s][%d] 检查成功，没有已经复核但未发送的支付交易！",__FILE__,__LINE__);
      return 0;
  }
  else if ( ret )
  {
      sprintf(acErrMsg,"查询大额往帐登记表 hv_zf 出错! ret=[%d]",ret);
      MBFEWRITEMSG
      return -100;
  }
  else
  {
       sprintf(acErrMsg,"尚有已经复核但未发送的支付交易，不能做此交易！");
      MBFEWRITEMSG
      return -1;
  }
  return -1;  
}

int spJ610_Process()
{
    int    iSendCnt=0;
    int    iSocketID=0;
    int    iReceiveCnt=0;
    
    char appdownflg[2];
    char failsuccess = '1';  /* '0'=成功    '1'=失败 */
    char   cCash_qs_no[13];
    char   tmpBuf[255];
    char   cTxday[9];
 
    double dSendAmt=0.0;    
    double dReceiveAmt=0.0;
    
    struct hv_chkmr_c hv_chkmr;
    struct hvstat_c shvstat;
    struct hv_orno_brno_c sHv_Or_Brno;
    
    THVPACKAGE pack659,respack;    
    /* add by LiuHuafeng 2007-1-31 10:27 **/
        memset(&shvstat,'\0',sizeof(shvstat) );
        ret = Hvstat_Sel(g_pub_tx.reply,&shvstat,"1=1");
        if(ret==100)
        {
          sprintf(acErrMsg,"记录不存在!ret=[%d]\n",ret);
          set_zd_data("0130",acErrMsg);
          WRITEMSG
          strcpy(g_pub_tx.reply,"P001");
          return -1;
        }
        else if(ret)
        {
          sprintf(acErrMsg,"数据库错误!ret=[%d]\n",ret);
          set_zd_data("0130",acErrMsg);
          strcpy(g_pub_tx.reply,"P001");
          WRITEMSG
          return -1;
        }
    /********end by LiuHuafeng 2007-1-31 10:28********/
    /* begin add by LiuHuafeng 2009-5-14 17:32:34 for 村镇银行 */
    /* 当状态是5的时候，立刻申请689报文 */
    if((char*)getenv("CZYH")!=NULL && strncmp((char*)getenv("CZYH"),"YES_CZYH",8)==0)
    {
    }
    else
    {
        if(shvstat.stat[0]==HV_STAT_WORKEND)
        {
            char cKinbr[6];
            char cTlrno[5];
            memset(&pack659,'\0',sizeof(pack659));
            memset(&respack,'\0',sizeof(respack));
            memset(cTxday,'\0',sizeof(cTxday));
                    
            MEMCPY_DEBUG(pack659.CMTCode,"659",3);
            get_zd_data("0050",cTxday);
            vtcp_log("[%s][%d]  cTxday=== [%s] ",__FILE__,__LINE__,cTxday);
            MEMCPY_DEBUG(pack659.workDate,cTxday,sizeof(pack659.workDate));
                    
            /*
             *apitoa(iHv_date,8, pack659.workDate);
             * vtcp_log("[%s][%d]  pack659.workDate=== [%s] ",__FILE__,__LINE__,pack659.workDate);     
             *MEMCPY_DEBUG(cTxday,pack659.workDate,sizeof(cTxday)-1);
             **/
            pack659.opeType='2';/*大额*/
            pack659.PRI='0';
            hv_set_tag(&pack659,"30A",cTxday);
            MEMCPY_DEBUG(cCash_qs_no, HV_QSBRNO, sizeof(HV_QSBRNO));
            hv_set_tag(&pack659,"CC6",cCash_qs_no);      /* 清算行行号 */
            hv_set_tag(&pack659,"0BE","1");       /*  交易种类－大额*/
            /* hv_set_tag(&pack659,"CD4","0017");      CMT列表数  */
            hv_set_tag(&pack659,"CD4","0019");    /*  CMT列表数  */
            /****
             *hv_set_tag(&pack659,"CL9","010001010102010501080109110011011102110311051108110901211122112311241725");CMT列表
             */
            hv_set_tag(&pack659,"CL9","0100010101020103010501080109110011011102110311051108110901211122112311241725");/*CMT列表*/
            iGetHvRefno(HV_BANKCODE_FLGNO,pack659.reNo);
                    
            ret=hv_gen_pack(&pack659);
            if(ret<0)
            {
                sprintf(acErrMsg,"组织申请明细报文错误");
                MBFEWRITEMSG
                return -1;
            }
            vtcp_log("[%s][%d]产生的申请明细报文: [%d]\n [%s]",__FILE__,__LINE__,pack659.length,pack659.body);
                    
            /*********插入发送列表登记簿************/
            memset(cKinbr, 0, sizeof(cKinbr));
            memset(cTlrno, 0, sizeof(cTlrno));
            get_zd_data("0030", cKinbr);
            get_zd_data("0070", cTlrno);
                    
            vtcp_log("[%s][%d]cKinbr==[%s]  cTlrno==[%s]",__FILE__,__LINE__,cKinbr,cTlrno);
                    
            ret=hv_insert_sendlist(&pack659,cKinbr,cTlrno,"000000","000000");
            vtcp_log("%s,%d,ret---------------------------%d\n",__FILE__,__LINE__,ret);
            if(ret)
            {
                sprintf(acErrMsg,"插入发送列表登记簿错误");
                MBFEWRITEMSG
                return -1;
            }
            vtcp_log("%s,%d,ret--------------%d  开始发送报文到前置机 !!!! ----- ",__FILE__,__LINE__,ret);
                    
                    
            vtcp_log("%s,%d,pack659.reNo=[%s]   ----- ",__FILE__,__LINE__,pack659.reNo);
                    
            /*** 发送报文到前置机 ***/
            ret=hv_sendpack(&pack659);
            if(ret)
            {
                sprintf(acErrMsg,"发送报文到前置机失败!");
                MBFEWRITEMSG
                return -1;
            }
            vtcp_log("%s,%d,After hv_get_tag() ---- 成功了 --- 去看看689有没有收到 !! ",__FILE__,__LINE__);
        }
    }
    /* end by LiuHuafeng 2009-5-14 17:32:46 */
    vtcp_log("[%s][%d]=======spJ610_Process()    Begin!!    ",__FILE__,__LINE__);
  
    /* 找到本行对应清算中心号   */   
    memset(cCash_qs_no,'\0',sizeof(cCash_qs_no) );   
    MEMCPY_DEBUG(cCash_qs_no,HV_QSBRNO,sizeof(HV_QSBRNO));        
   
    /* 一、我行： 首先从hv_zf表中找出往帐总笔数，来帐总笔数和即时转帐总笔数    */
    /* 往帐: 发起清算行是咱自己; 
       来帐:发起清算行不是咱自己即接收清算行是咱自己                */
   
    /* 1. 往帐: 只认为往账记账等待、往账记账清算、往账退回记账、
                往账撤销等待、退回等待、往账排队中等是我方成功往帐  */
    ret = get_SendCnt_SendAmt(cCash_qs_no,&iSendCnt,&dSendAmt);
    if( ret )
    {
       sprintf(acErrMsg,"计算往帐总笔数、往帐总金额时出错！");
       WRITEMSG
       return -1;
    }
    vtcp_log("[%s][%d] 1. 得到 我行往账总笔数  iSendCnt=[%d] ",__FILE__,__LINE__,iSendCnt);  
    vtcp_log("[%s][%d] 1. 得到 我行往账总金额 dSendAmt=[%lf] ",__FILE__,__LINE__,dSendAmt);  
    
    
    /* 2. 来帐: 只认为来帐退回、来账记账清算、来账核地方押错、来账核全国押错、
                来账内容不符、来账户名不符等为我方成功接收的来帐            */

                
     g_reply_int = get_ReceiveCnt_ReceiveAmt(cCash_qs_no,&iReceiveCnt,&dReceiveAmt);
     if( g_reply_int )
     {
       sprintf(acErrMsg,"计算来帐总笔数，来帐总金额出错！");
       WRITEMSG
       return -1;
     }  

    vtcp_log("[%s][%d] 2. 得到 我行来帐总笔数 iReceiveCnt=[%d] ",__FILE__,__LINE__,iReceiveCnt);  
    vtcp_log("[%s][%d] 2. 得到 我行来帐总金额 dReceiveAmt=[%lf] ",__FILE__,__LINE__,dReceiveAmt);  

  
    /* 3. 即时转帐: 目前暂不处理*/
     
    /* 二、人行:  从大额业务核对主表中找出人行来/往帐总笔数, 来/往帐总金额, 来/往帐分类项目数 */
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
    
    vtcp_log("[%s][%d] 2. 得到 人行往账总笔数 CCPC.send_cnt=[%d]  ",__FILE__,__LINE__,hv_chkmr.send_cnt);  
    vtcp_log("[%s][%d] 2. 得到 我行往账总笔数 DHCC.iSendCnt=[%d]",__FILE__,__LINE__,iSendCnt);  
    vtcp_log("[%s][%d] 2. 得到 人行往账总金额 CCPC.send_amt=[%lf]",__FILE__,__LINE__,hv_chkmr.send_amt);  
    vtcp_log("[%s][%d] 2. 得到 我行往账总金额 DHCC.dSendAmt=[%lf]",__FILE__,__LINE__,dSendAmt);  
    vtcp_log("[%s][%d] 2. 得到 人行来帐总笔数 CCPC.recevice_cnt=[%d] ",__FILE__,__LINE__,hv_chkmr.recevice_cnt);  
    vtcp_log("[%s][%d] 2. 得到 我行来帐总笔数 DHCC.iReceiveCnt=[%d]  ",__FILE__,__LINE__,iReceiveCnt);  
    vtcp_log("[%s][%d] 2. 得到 人行来帐总金额 CCPC.recevice_amt=[%lf] ",__FILE__,__LINE__,hv_chkmr.recevice_amt);  
    vtcp_log("[%s][%d] 2. 得到 我行来帐总金额 DHCC.dReceiveAmt=[%lf] ",__FILE__,__LINE__,dReceiveAmt);  

    
    /*  For Test   --------------------------------------
    hv_chkmr.recevice_cnt = iReceiveCnt;
    hv_chkmr.recevice_amt = dReceiveAmt; ---------------*/
       
     /* 比较： 如果所有的都成功给成功标志,否则给失败标志 */
    if( (hv_chkmr.send_cnt == iSendCnt) && ((hv_chkmr.send_amt - dSendAmt> -0.0001) || (hv_chkmr.send_amt - dSendAmt <0.0001))  && 
       (hv_chkmr.recevice_cnt == iReceiveCnt) && ((hv_chkmr.recevice_amt - dReceiveAmt> -0.0001) || (hv_chkmr.recevice_amt - dReceiveAmt <0.0001)))
    {
        failsuccess = '0';
    }
    /* 进一步比较分类项目   老系统中注释掉了 */
    if( failsuccess =='0' )  /**对账已平**/
    {
        char cTx_date[9];
        memset(cTx_date, 0 , sizeof(cTx_date));
        sprintf(cTx_date,"%08d",g_pub_tx.tx_date);
        vtcp_log("[%s][%d] 2. 得到 failsuccess=[%c]  日终对账已经平 ",__FILE__,__LINE__,failsuccess);  
        vtcp_log("[%s][%d] cTx_date=%s,hvstat.filler1=[%s] ",__FILE__,__LINE__,cTx_date,shvstat.filler1);  
        if(memcmp(cTx_date,shvstat.filler1,8)<0)
        {
            ret = hv_setstat(HV_STAT_PREPARE); /*设置大额营业状态为 '营业准备'*/
            if( ret )
            {
                sprintf(acErrMsg,"设置大额营业状态出错！");
                MBFEWRITEMSG
                return -1;
            }
        }
        else
        {
            ret = hv_setstat(HV_STAT_CHECKRIGHT); /*设置大额营业状态为 '日终对账已经平'*/
            if( ret )
            {
                sprintf(acErrMsg,"设置大额营业状态出错！");
                MBFEWRITEMSG
                return -1;
            }
        }
        
        
        /* 对帐已平则将日期置成下一天  收到日切后，下一天日期已在filler1中 */
        ret = Hvstat_Dec_Upd(g_pub_tx.reply," 1 = 1 ");
        if( ret )
        {
            sprintf(acErrMsg,"声明游标时出错! ");
            MBFEWRITEMSG
            return -1;
        }
        
        memset(&shvstat,'\0',sizeof(shvstat) );                 
        ret = Hvstat_Fet_Upd(&shvstat,g_pub_tx.reply);
        if(ret==100)
        {
            sprintf(acErrMsg,"记录不存在!ret=[%d]\n",ret);
            set_zd_data("0130",acErrMsg);
            WRITEMSG
            strcpy(g_pub_tx.reply,"P001");
            return -1;
        }
        else if(ret)
        {
            sprintf(acErrMsg,"数据库错误!ret=[%d]\n",ret);
            set_zd_data("0130",acErrMsg);
            strcpy(g_pub_tx.reply,"P001");
            WRITEMSG
            return -1;
        }
        
        shvstat.tx_date = apatoi(shvstat.filler1,8);
        vtcp_log("[%s][%d] shvstat.filler1====[%s]     shvstat.tx_date====[%d] ",__FILE__,__LINE__,shvstat.filler1,shvstat.tx_date);  
        ret = Hvstat_Upd_Upd(shvstat,g_pub_tx.reply);
        if(ret)
        {
            sprintf(acErrMsg,"更新大额状态主表出错!ret==[%d]\n",ret);
            WRITEMSG
            strcpy(g_pub_tx.reply,"D105");
            return (-1);
        }              
        Hvstat_Clo_Upd();
        
        set_zd_long("0510",1);
        /* begin add by LiuHuafeng 2009-5-14 15:08:22 处理对账平后生成村镇银行需要处理的对账文件 for 村镇银行 */
        if((char*)getenv("CZYH")!=NULL && strncmp((char*)getenv("CZYH"),"YES_CZYH",8)==0)
        {
            vtcp_log("[%s][%d] 村镇银行村镇银行\n",__FILE__,__LINE__);
        }
        else
        {
            struct hv_chkmr_c sHv_chkmr;
            struct hv_chkagemr_c sHv_chkagemr;
            char cBuf[1024];
            char cBasename[100];
            char cFullname[100];
            char cCmd[300];
            char cOldbrno[BRNO_LEN+1];
            char cOldFilename[100];
            struct syyh_czyh_qs_reg_c sQsreg;
            
            FILE * fp;
                 
            vtcp_log("[%s][%d] 准备循环导入村镇银行的往帐!!!",__FILE__,__LINE__);
            memset(cOldbrno, 0 , sizeof(cOldbrno));
            memset(cOldFilename, 0 , sizeof(cOldFilename));
            ret=Hv_orno_brno_Dec_Sel( g_pub_tx.reply, " br_no like '9999_' order by br_no ");
            while(1)
            {
                 memset(cFullname, 0 , sizeof(cFullname));
                 memset(cBasename, 0 , sizeof(cBasename));
                 memset(cBuf, 0 , sizeof(cBuf));
                 memset(&sHv_chkmr, 0 , sizeof(sHv_chkmr));
                 memset(cCmd, 0 , sizeof(cCmd));
                 memset(&sHv_chkagemr, 0 , sizeof(sHv_chkagemr));
                 vtcp_log("[%s][%d]准备给村镇银行传数据了\n",__FILE__,__LINE__);
                 memset(&sHv_Or_Brno , 0 , sizeof(sHv_Or_Brno));
                 ret=Hv_orno_brno_Fet_Sel( &sHv_Or_Brno,g_pub_tx.reply );
                 if( ret==100 ) break;
                 if( ret )
                 {
                      sprintf(acErrMsg," 查找村镇银行机构 出错 !! ");
                      WRITEMSG 
                      return -1;  
                 }

                 vtcp_log("[%s][%d] [%s]村镇银行开始登记大额清算登记簿!!",__FILE__,__LINE__,sHv_Or_Brno.br_no);
                  
                 
                 /* 准备下传的队长文件 */
                 sprintf(cBasename,"hv_xc_dz_%s_%08ld.txt",sHv_Or_Brno.br_no,g_pub_tx.tx_date);
                 sprintf(cFullname,"%s/hv_xc_dz_%s_%08ld.txt",(char*)getenv("YQSH_PATH"),sHv_Or_Brno.br_no,g_pub_tx.tx_date);
                 sprintf(cCmd,"ftpsend %s %s",(char*)getenv("YQSH_PATH"),cBasename);
             /* 1,组织汇总报文 hv_chkmr */
                 sHv_chkmr.tx_date=g_pub_tx.tx_date;
                 /* rem by LiuHuafeng 2010-9-26 17:44:04
                 sHv_chkmr.send_cnt=sql_count_sum_double ( "hv_zf" , "tx_amt" , &sHv_chkmr.send_amt , "tx_date=%ld and br_no='%s' and lw_ind='1' and ( (hv_sts in ('0', '1','2','3','7','B','J','K')) or (hv_sts='L' and checkflag is not null) ) " ,
                     g_pub_tx.tx_date, sHv_Or_Brno.br_no  );
            		**replace by next line 2010-9-26 17:44:31**/
                 sHv_chkmr.send_cnt=sql_count_sum_double ( "hv_zf" , "tx_amt" , &sHv_chkmr.send_amt , "tx_date=%ld and or_br_no='%s' and lw_ind='1' and ( (hv_sts in ('0', '1','2','3','7','B','J','K')) or (hv_sts='L' and checkflag is not null) ) " ,
                     g_pub_tx.tx_date, sHv_Or_Brno.or_br_no  );
                 
                 sHv_chkmr.recevice_cnt=sql_count_sum_double ( "hv_zf" , "tx_amt" , &sHv_chkmr.recevice_amt , "tx_date = %ld and br_no='%s' and lw_ind='2' and ( (hv_sts in ('4','9','B','F','G','H','I','K')) or (hv_sts='L' and checkflag is not null)  ) and cmtno!='812' and cmtno!='814' " ,
                     g_pub_tx.tx_date, sHv_Or_Brno.br_no  );
            		
                 sHv_chkmr.online_cnt=sql_count_sum_double ( "hv_zf" , "tx_amt" , &sHv_chkmr.online_amt , "tx_date = %ld and br_no='%s' and lw_ind='2' and cmtno='232' " ,
                     g_pub_tx.tx_date, sHv_Or_Brno.br_no  );
                 /* TODO 写对账文件 */
                 vHv_chkmr_to_buf(&sHv_chkmr,cBuf);
                 if(strlen(cOldbrno)==0)
                 {
                     fp = fopen(cFullname,"w");
                     memcpy(cOldFilename,cFullname,sizeof(cOldFilename)-1);
                     memcpy(cOldbrno,sHv_Or_Brno.br_no,BRNO_LEN);
                 }
                 else if(strlen(cOldbrno)!=0 && memcmp(cOldbrno,sHv_Or_Brno.br_no,BRNO_LEN)!=0)
                 {
                     /* 关闭原来文件 */
                     fclose(fp);
                     fp = fopen(cFullname,"w");
                     memcpy(cOldbrno,sHv_Or_Brno.br_no,BRNO_LEN);
                 }
                 if(fp==NULL)
                 {
                     vtcp_log("[%s][%d]打开文件失败,cFullname  == %s!\n",__FILE__,__LINE__,cFullname);
                     strncpy(g_pub_tx.reply,"P157",4);
                     return -1;
                 }
                 fprintf(fp,"%s\n",cBuf);
                 
                 memset(cBuf , 0 , sizeof(cBuf));
             /* 2,组织明细报文 hv_chkagemr*/
                 ret = Hv_chkagemr_Dec_Sel(g_pub_tx.reply, "tx_date=%ld   and br_no = '%s' ", g_pub_tx.tx_date,sHv_Or_Brno.br_no);
                 if ( ret )
                 {
                     sprintf(acErrMsg,"Hv_chkagemr_Dec_Sel() 出错[%d]",ret);
                     WRITEMSG
                     return ret;
                 }
                 while(1)
                 {
                     memset(&sHv_chkagemr, 0 , sizeof(sHv_chkagemr));
                     ret = Hv_chkagemr_Fet_Sel( &sHv_chkagemr , g_pub_tx.reply );
                     if(ret == 100)
                         break;
                     if (ret){
                         vtcp_log("%s,%d fetch hv_chkagemr error sqlcode= %ld",__FILE__,__LINE__,ret);
                         WRITEMSG
                         return ret;
                     }
                     memset(cBuf , 0 , sizeof(cBuf));
                     vtcp_log("%s,%d ",__FILE__,__LINE__);
                     vHv_chkagemr_to_buf(&sHv_chkagemr,cBuf);
                     /* todo 写明细文件 */
                     fprintf(fp,"%s\n",cBuf);            
                 }
                 fclose(fp);
                 Hv_chkagemr_Clo_Sel();
                 /* 下传对账文件到村镇银行 */
                 ret = system(cCmd);
                 if(ret)
                 {
                     vtcp_log("%s,%d 下传对账文件到村镇银行失败",__FILE__,__LINE__);
                     strncpy(g_pub_tx.reply,"P157",4);
                     return ret;
                 }
             /* 生产与村镇银行清算的数据 */
                 memset(&sQsreg , 0 , sizeof(sQsreg));
                 sQsreg.tx_date=g_pub_tx.tx_date;
                 sQsreg.flag[0]='1'; /* 大额 */
                 sQsreg.trace_no=g_pub_tx.trace_no;
                 sQsreg.wz_amt = sHv_chkmr.send_amt;
                 sQsreg.lz_amt = sHv_chkmr.recevice_amt;
                 sQsreg.zc_amt = sHv_chkmr.send_amt - sHv_chkmr.recevice_amt;
                 sQsreg.qs_flag[0]='1';/* 未清算 */
                 
                 memcpy(sQsreg.br_no , sHv_Or_Brno.br_no , BRNO_LEN);
                 
                 ret = Syyh_czyh_qs_reg_Ins( sQsreg , g_pub_tx.reply );
                 if(ret)
                 {
                     Syyh_czyh_qs_reg_Debug(sQsreg);
                     vtcp_log("%s,%d 写入与村镇银行清算表失败 ret = %d",__FILE__,__LINE__,ret);
                     set_zd_data("0130","写清算表失败");
                     return ret;
                 }
                 vtcp_log("[%s][%d] [%s]村镇银行大额清算登记簿写完!!",__FILE__,__LINE__,sQsreg.br_no);
             }
             Hv_orno_brno_Clo_Sel();
        }
        /* end by LiuHuafeng for 村镇银行 2009-5-14 15:09:09 */
    }
    else   /**对账不平**/
    {
        vtcp_log("[%s][%d] 2. 得到 failsuccess=[%c]  日终对账不平 ",__FILE__,__LINE__,failsuccess);  
         failsuccess = '1';
        ret = hv_setstat(HV_STAT_CHECKERR);  /*设置大额营业状态为 ' 日终对账不平'*/
        if( ret )
        {
              sprintf(acErrMsg,"设置大额营业状态出错！");
              MBFEWRITEMSG
              return -1;
        } 

        vtcp_log("  cStat=[%s]  HV_STAT_WORKEND=[%s]",cStat,HV_STAT_WORKEND);          
        /*账务不平则发送659报文, 申请下载明细报文689 */
        if( cStat == HV_STAT_WORKEND)   /*如果已经发送了申请下载，则不再发送*/
        {
            memset(&pack659,'\0',sizeof(pack659));   
            memset(&respack,'\0',sizeof(respack));  
            memset(cTxday,'\0',sizeof(cTxday));    
                                       
             MEMCPY_DEBUG(pack659.CMTCode,"659",3);
             get_zd_data("0050",cTxday);
             vtcp_log("[%s][%d]  cTxday=== [%s] ",__FILE__,__LINE__,cTxday);  
            MEMCPY_DEBUG(pack659.workDate,cTxday,sizeof(pack659.workDate));
            
/*
            apitoa(iHv_date,8, pack659.workDate);
             vtcp_log("[%s][%d]  pack659.workDate=== [%s] ",__FILE__,__LINE__,pack659.workDate);     
            MEMCPY_DEBUG(cTxday,pack659.workDate,sizeof(cTxday)-1);
*/                        
             pack659.opeType='2';/*大额*/
             pack659.PRI='0';
             hv_set_tag(&pack659,"30A",cTxday); 
             hv_set_tag(&pack659,"CC6",cCash_qs_no);      /* 清算行行号 */
             hv_set_tag(&pack659,"0BE","1");       /*  交易种类－大额*/
            /* hv_set_tag(&pack659,"CD4","0017");      CMT列表数  */
             hv_set_tag(&pack659,"CD4","0019");    /*  CMT列表数  */
/****
             hv_set_tag(&pack659,"CL9","010001010102010501080109110011011102110311051108110901211122112311241725");CMT列表*/
             hv_set_tag(&pack659,"CL9","0100010101020103010501080109110011011102110311051108110901211122112311241725");/*CMT列表*/
            iGetHvRefno(HV_BANKCODE_FLGNO,pack659.reNo);
            
             ret=hv_gen_pack(&pack659);
             if(ret<0)
             {
                sprintf(acErrMsg,"组织申请明细报文错误");
                MBFEWRITEMSG
                return -1;               
              }
             vtcp_log("[%s][%d]产生的申请明细报文: [%d]\n [%s]",__FILE__,__LINE__,pack659.length,pack659.body);

            /*********插入发送列表登记簿************/
            char cKinbr[6];
            char cTlrno[5];
            memset(cKinbr, 0, sizeof(cKinbr));
            memset(cTlrno, 0, sizeof(cTlrno));            
             get_zd_data("0030", cKinbr);
             get_zd_data("0070", cTlrno);                         
            
             vtcp_log("[%s][%d]cKinbr==[%s]  cTlrno==[%s]",__FILE__,__LINE__,cKinbr,cTlrno);
            
            ret=hv_insert_sendlist(&pack659,cKinbr,cTlrno,"000000","000000");
            vtcp_log("%s,%d,ret---------------------------%d\n",__FILE__,__LINE__,ret);
            if(ret)
            {
                sprintf(acErrMsg,"插入发送列表登记簿错误");
                MBFEWRITEMSG
                return -1; 
            }
            vtcp_log("%s,%d,ret--------------%d  开始发送报文到前置机 !!!! ----- ",__FILE__,__LINE__,ret);


             vtcp_log("%s,%d,pack659.reNo=[%s]   ----- ",__FILE__,__LINE__,pack659.reNo);
           
            /*** 发送报文到前置机 ***/
            ret=hv_sendpack(&pack659);
            if(ret)
            {
                sprintf(acErrMsg,"发送报文到前置机失败!");
                MBFEWRITEMSG
                return -1; 
            }            

            vtcp_log("%s,%d,After hv_get_tag() ---- 成功了 --- 去看看689有没有收到 !! ",__FILE__,__LINE__);
        }

        /*  准备输出数据 给8583域赋值  */
        set_zd_long("0510",0);
    }
     
    vtcp_log(" 2. 得到 人行往账总笔数 CCPC.send_cnt=[%d]  ",hv_chkmr.send_cnt);  
    vtcp_log(" 2. 得到 我行往账总笔数 DHCC.iSendCnt=[%d]", iSendCnt);  
    vtcp_log(" 2. 得到 人行往账总金额 CCPC.send_amt=[%lf]",hv_chkmr.send_amt);  
    vtcp_log(" 2. 得到 我行往账总金额 DHCC.dSendAmt=[%lf]",dSendAmt);  
    vtcp_log(" 2. 得到 人行来帐总笔数 CCPC.recevice_cnt=[%d] ", hv_chkmr.recevice_cnt);  
    vtcp_log(" 2. 得到 我行来帐总笔数 DHCC.iReceiveCnt=[%d]  ", iReceiveCnt);  
    vtcp_log(" 2. 得到 人行来帐总金额 CCPC.recevice_amt=[%lf] ",hv_chkmr.recevice_amt);  
    vtcp_log(" 2. 得到 我行来帐总金额 DHCC.dReceiveAmt=[%lf] ", dReceiveAmt);  
        
     /* For Test 
        hv_chkmr.send_cnt = 10;
        hv_chkmr.recevice_cnt = 100;
        hv_chkmr.send_amt = 1111.11;        
        hv_chkmr.recevice_amt = 2222.22;
        
        iSendCnt = 23;                
        iReceiveCnt = 45;         
        dSendAmt = 3333.33; 
        dReceiveAmt = 444.44; */

        set_zd_long("0520",hv_chkmr.send_cnt);     /* 人行往帐笔数 */
        set_zd_long("0530",iSendCnt);              /* 我行往帐笔数 */               
        set_zd_long("0540",hv_chkmr.recevice_cnt); /* 人行来帐笔数 */       
        set_zd_long("0550",iReceiveCnt);           /* 我行来帐笔数 */
               
        set_zd_double("0400",hv_chkmr.send_amt);      /* 人行往帐金额 */
        set_zd_double("0410",dSendAmt);               /* 我行往帐金额 */
        set_zd_double("0420",hv_chkmr.recevice_amt);  /* 人行来帐金额 */
        set_zd_double("0430",dReceiveAmt);            /* 我行来帐金额 */  

    return 0;
  
}


/* 计算往帐总笔数、往帐总金额 */
int get_SendCnt_SendAmt(char* cCash_qs_no, int* iSendCnt,double* dSendAmt)
{
    int sendcnt = 0;
    double sendamt = 0.0;
    struct hv_zf_c hv_zf;    

    vtcp_log("[%s][%d]=======get_SendCnt_SendAmt()    Begin!!    ",__FILE__,__LINE__);   

/*
    ret = Hv_zf_Dec_Sel(g_pub_tx.reply," tx_date = %ld  and pay_qs_no ='%s' and lw_ind='1' and ((hv_sts in ('1','2','3','7','B','J','K')) or (hv_sts='L' and checkflag not in ('1','2','3','B') ) ) ",g_pub_tx.tx_date, cCash_qs_no);
*/
    ret = Hv_zf_Dec_Sel(g_pub_tx.reply," tx_date = %ld  and pay_qs_no ='%s' and lw_ind='1' and ( (hv_sts in ('0', '1','2','3','7','B','J','K')) or (hv_sts='L' and checkflag is not null) )  ",g_pub_tx.tx_date, cCash_qs_no);
    if( ret )
    {
      sprintf(acErrMsg,"大额来帐数据库操作失败[%d]",ret);
      WRITEMSG
      return -1;
    }    
  
    while(1)
    {
        memset(&hv_zf,0,sizeof(struct hv_zf_c) );      
        ret = Hv_zf_Fet_Sel( &hv_zf, g_pub_tx.reply );
        if (ret == 100)
        {
            Hv_zf_Clo_Sel();    
            *iSendCnt = sendcnt;
            *dSendAmt = sendamt;          
            vtcp_log("[%s][%d]== 计算往帐总笔数、往帐总金额 完毕 ",__FILE__,__LINE__);                              
            WRITEMSG
            return 0;
        } 
        else if(ret)
        {
            sprintf(acErrMsg,"大额来帐数据库操作失败 [%d]",ret);          
            WRITEMSG
             return -1;
        }
        
        sendcnt = sendcnt + 1;
        sendamt = sendamt + hv_zf.tx_amt;
    }

    Hv_zf_Clo_Sel();
    return 0;
}


/* 计算来帐总笔数，来帐总金额 */
int get_ReceiveCnt_ReceiveAmt(char* cCash_qs_no, int* iReceiveCnt,double* dReceiveAmt)
{
    int receivecnt = 0;
    double receiveamt = 0.0;
    struct hv_zf_c hv_zf;    
    vtcp_log("[%s][%d]=======get_ReceiveCnt_ReceiveAmt()    Begin!!    ",__FILE__,__LINE__);   
   


/*  
    ret = Hv_zf_Dec_Sel(g_pub_tx.reply,"tx_date = %ld  and cash_qs_no ='%s' and lw_ind='2' and ( (hv_sts in ('4','9','F','G','H','I','K')) or (hv_sts='L' and checkflag not in ('1','2','3','B')  )) ",g_pub_tx.tx_date, cCash_qs_no);
*/    
    ret = Hv_zf_Dec_Sel(g_pub_tx.reply,"tx_date = %ld  and cash_qs_no ='%s' and lw_ind='2' and ( (hv_sts in ('4','9','B','F','G','H','I','K')) or (hv_sts='L' and checkflag is not null)  ) and cmtno!='812' and cmtno!='802' and cmtno!='814' ",g_pub_tx.tx_date, cCash_qs_no);
    if( ret )
    {
      sprintf(acErrMsg,"大额来帐数据库操作失败[%d]",ret);
      WRITEMSG
      return -1;
    }    
  
    while(1)
    {
        memset(&hv_zf,0,sizeof(struct hv_zf_c) );     
        ret = Hv_zf_Fet_Sel( &hv_zf, g_pub_tx.reply );
        if (ret == 100)
        {
            Hv_zf_Clo_Sel();  
            *iReceiveCnt = receivecnt;
            *dReceiveAmt = receiveamt; 
            vtcp_log("[%s][%d]=计算来帐总笔数，来帐总金额 完毕 ",__FILE__,__LINE__);                              
            return 0;
        } 
        else if(ret)
        {
            sprintf(acErrMsg,"大额来帐数据库操作失败 [%d]",ret);          
            WRITEMSG
             return -1;
        }
        
        receivecnt = receivecnt + 1;
        receiveamt = receiveamt + hv_zf.tx_amt;
    }    
    Hv_zf_Clo_Sel();
    return 0;

}
void vHv_chkmr_to_buf(struct hv_chkmr_c * pChkmr,char * buf)
{
    char cBuf[1024];
    memset(cBuf , 0 , sizeof(cBuf));
    sprintf(cBuf,"%08ld|%012s|%09ld|%16.2f|%09ld|%s|%09ld|%16.2f|%09ld|%s|%09ld|%16.2f|%09ld|%s|",
        pChkmr->tx_date       ,pChkmr->ccpc_no      ,pChkmr->send_cnt     ,pChkmr->send_amt     
        ,pChkmr->send_item    ,pChkmr->send_dtl     ,pChkmr->recevice_cnt ,pChkmr->recevice_amt 
        ,pChkmr->recevice_item,pChkmr->recevice_dtl ,pChkmr->online_cnt   ,pChkmr->online_amt   
        ,pChkmr->online_item  ,pChkmr->online_dtl);
    memcpy(buf,cBuf,sizeof(cBuf)-1);
}
void vHv_chkagemr_to_buf(struct hv_chkagemr_c * pChkagemr,char * buf)
{
    char cBuf[1024];
    memset(cBuf , 0 , sizeof(cBuf));
    vtcp_log("%s,%d ",__FILE__,__LINE__);
    sprintf(cBuf,"%s|%s|%s|%08ld|%s|%s|%ld|%s|%15.2f|%15.2f|%15.2f|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%ld|%s|%s|%s|%8.5f|%ld|%s|%s|%s|%s|%s|%s|%ld|%15.2f|%s|%s|%s|%s|%s|",
        pChkagemr->orderno         ,pChkagemr->pay_qs_no      ,pChkagemr->br_no          
        ,pChkagemr->tx_date        ,pChkagemr->cmtno          ,pChkagemr->o_cmtno        
        ,pChkagemr->tx_time        ,pChkagemr->cur_no         ,pChkagemr->tx_amt         
        ,pChkagemr->other_amt      ,pChkagemr->jf_amt         ,pChkagemr->or_br_no       
        ,pChkagemr->pay_no         ,pChkagemr->pay_ac_no      ,pChkagemr->payname        
        ,pChkagemr->payaddress     ,pChkagemr->cashqsactno    ,pChkagemr->acbrno         
        ,pChkagemr->cashno         ,pChkagemr->cash_ac_no     ,pChkagemr->cash_name      
        ,pChkagemr->cash_addr      ,pChkagemr->yw_type        ,pChkagemr->sendco         
        ,pChkagemr->receco         ,pChkagemr->operlevel      ,pChkagemr->obrno          
        ,pChkagemr->voctype        ,pChkagemr->vocnum         ,pChkagemr->p_date         
        ,pChkagemr->p_num          ,pChkagemr->hp_type        ,pChkagemr->pass_wd        
        ,pChkagemr->inst_rate      ,pChkagemr->term           ,pChkagemr->o_tx_date      
        ,pChkagemr->ou_tx_date     ,pChkagemr->o_orderno_hv   ,pChkagemr->o_tx_type      
        ,pChkagemr->p_orderno_hv   ,pChkagemr->dc             ,pChkagemr->pay_date       
        ,pChkagemr->intst          ,pChkagemr->reason         ,pChkagemr->repson         
        ,"0"        ,pChkagemr->brf1           ,pChkagemr->brf2);           
    memcpy(buf,cBuf,sizeof(cBuf)-1);
}
