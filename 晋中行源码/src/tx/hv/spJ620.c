/*************************************************
* ÎÄ ¼þ Ãû:  spJ620.c  ¶ÔÓ¦ÀÏÏµÍ³ V95.pc
* ¹¦ÄÜÃèÊö£º ´ó¶îÖ§¸¶Ã÷Ï¸¶ÔÕÊ
*             
* ×÷    Õß:  ChenMing 
* Íê³ÉÈÕÆÚ£º 2006Äê8ÔÂ26 ÈÕ
*
* ÐÞ¸Ä¼ÇÂ¼£º 
* ÈÕ    ÆÚ:
* ÐÞ ¸Ä ÈË:
* ÐÞ¸ÄÄÚÈÝ:
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
    
    if ( iChecked == 0 )  /* Ö»ÐèÒªÔÚµÚÒ»´Î½øÈëÊ±Ö´ÐÐ spJ620_Check*/ 
    {  
          ret = spJ620_Check();
      if(ret == 999) /* ¼ì²éÎ´Í¨¹ý ²»±¨´í!*/
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
          set_zd_long("0520",iChecked);       /* ÒÑ¼ì²é!! */
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
    * ÅÐ¶Ï½»Ò×»ú¹¹ÊÇ·ñÊÇÇåËãÖÐÐÄ *
    *****************************/
    if( GetBr_noByOr_br_no(HV_QS_BR_NO, cQs_br_no))
    {
          sprintf(acErrMsg,"¸ù¾Ý Ö§¸¶ÏµÍ³ÐÐºÅ ²éÕÒ »ú¹¹ºÅÂë³ö´í! " );
      WRITEMSG
          return -1;
    }
    
    vtcp_log("[%s][%d]=======  g_pub_tx.tx_br_no = [%s] ÇåËãÖÐÐÄ cQs_br_no=[%s]",__FILE__,__LINE__,g_pub_tx.tx_br_no,cQs_br_no);    
    /***  ÅÐ¶Ï½»Ò×»ú¹¹ÊÇ·ñÊÇÇåËãÖÐÐÄ   Remed For Test ! ***/
    if(memcmp(g_pub_tx.tx_br_no,cQs_br_no,strlen(g_pub_tx.tx_br_no)-1))
    {
          sprintf(acErrMsg,"·ÇÇåËãÖÐÐÄ²»ÄÜ×ö´Ë½»Ò×!%s",g_pub_tx.tx_br_no);
      WRITEMSG
      iResult = 5;
      set_zd_long("0510",5);
          return 999;   
    } 
    
    /*****************************************************************
    *  ÅÐ¶Ï´ó¶î×´Ì¬£º ÊÇ·ñ¿ÉÒÔ¶ÔÕÊ                                   *
    *****************************************************************/
    cStat = hv_getstat();
    vtcp_log("[%s][%d]=======cStat = [%c]",__FILE__,__LINE__,cStat);
    /* cStat = HV_STAT_CHECKERR;  For Test */
    if(cStat != HV_STAT_CHECKERR)
    {
          sprintf(acErrMsg,"ÏµÍ³µ±Ç°×´Ì¬²»ÊÇºË¶ÔÃ÷Ï¸ÕÊ×´Ì¬! [%c]",cStat);
      WRITEMSG
      iResult = 6;
      set_zd_long("0510",6);
          return 999;   
    }
    vtcp_log("[%s][%d]=======  Ö´ÐÐµ½ÕâÀïÁË  ¿ÉÒÔ¶ÔÕÊ ========",__FILE__,__LINE__);
    
    /*****************************************************************
    *  ÅÐ¶ÏÊÇ·ñ´æÔÚ¸´ºËµ«Î´È¡Ïû×´Ì¬¡¢¸´ºËµ«Î´·¢ËÍµÄÊý¾Ý              *
    *****************************************************************/
    /*  Remed For Test ! ***/
    ret = iCheckHvForDateOver((char *)NULL,g_pub_tx.tx_date,&iErrcnt);
    if( ret )
    {
          sprintf(acErrMsg," ²éÕÒ´ó¶îÖ§¸¶±í´íÎó! [%d]",ret);
      WRITEMSG
          return -1;
    }
    vtcp_log("[%s][%d]=======   iErrcnt=[%d]   ========",__FILE__,__LINE__,iErrcnt);
    if( iErrcnt != 0 )
    {
          sprintf(acErrMsg,"´æÔÚ¸´ºËµ«Î´·¢ËÍµÄÊý¾Ý[%d]",iErrcnt);
      WRITEMSG
      iResult = 7;
      set_zd_long("0510",7);
          return 999;   
    }
    vtcp_log("[%s][%d] =======Ö´ÐÐµ½ÕâÀïÁË,²»´æÔÚ¸´ºËµ«Î´·¢ËÍµÄÊý¾Ý ! =======",__FILE__,__LINE__);
    
    /*****************************************************************
    * ÅÐ¶Ï¶ÔÕÊÃ÷Ï¸ÊÇ·ñÏÂÔØ  ÒÔ¼°ºÍ×Ü¼ÆºË¶Ô±¨ÎÄÊÇ·ñ¶Ô×Ü               *
    *****************************************************************/    

    if( iCheckDownloadFile(&iResult) )
    {   
            vtcp_log("[%s][%d] ===iResult====[%d]",iResult);
          if( iResult==8)
          {
                sprintf(acErrMsg,"Ã»ÓÐÏÂÔØ¶ÔÕÊÃ÷Ï¸ !");
          }
          else
          {
                  sprintf(acErrMsg,"¶ÔÕÊÃ÷Ï¸ºÍ×Ü¼ÆºË¶Ô±¨ÎÄ²»¶Ô×Ü !");
          }
          
      WRITEMSG
      set_zd_long("0510",iResult);
          return 999;   
    }    
    vtcp_log("[%s][%d]=======   Ö´ÐÐµ½ÕâÀïÁË   ¶ÔÕÊÃ÷Ï¸ÒÑÏÂÔØ========",__FILE__,__LINE__);
    
    return 0;
}

/* ºË¶ÔÃ÷Ï¸  ÀÏÏµÍ³ÊÇµ÷ÓÃ /Appl/batch/bin/checkCcpc */
int spJ620_Process()
{
      int ret=0;
    int iHandled =0;   /* ÊÇ·ñ´¦ÀíÁËÒ»Ìõ¼ÇÂ¼ 0=·ñ    1=ÊÇ */     
      
      vtcp_log("[%s][%d] =======  spJ620_Process Start !!! ========",__FILE__,__LINE__);    
      
      /*****************************************************************
      * 1  ¼ì²é dhccÓÐ µ«cpccÃ»ÓÐ ( ·¢Éú¶îÓÐ³öÈëµÄÇé¿ö ²»¿¼ÂÇ)         *
      *****************************************************************/
      vtcp_log("[%s][%d]¿ªÊ¼¼ì²é dhccÓÐ µ«cpccÃ»ÓÐµÄÊý¾Ý ",__FILE__,__LINE__);
      vtcp_log("[%s][%d]¿ªÊ¼¼ì²é dhccÓÐ µ«cpccÃ»ÓÐµÄÊý¾Ý ",__FILE__,__LINE__);
                  
        ret = iCheckHv_zfTable(&iHandled);  /*¸ù¾Ýhv_chkagemrºË¶ÔHv_zf*/
        if(ret)
        {
          vtcp_log("[%s][%d]======= iCheckHv_zfTable() ³ö´í! ========",__FILE__,__LINE__);    
             sprintf(acErrMsg,"´ó¶îÖ§¸¶Ã÷Ï¸¶ÔÕÊ³ö´í[%d]",ret);
             WRITEMSG
             return(ret);
        }
      vtcp_log("[%s][%d]======= After iCheckHv_zfTable()!!========",__FILE__,__LINE__);    
      vtcp_log("[%s][%d] ÊÇ·ñ´¦ÀíÁËÒ»Ìõ¼ÇÂ¼    iHandled=[%d]",__FILE__,__LINE__,iHandled);
    if( iHandled == 1)   /* ÊÇ·ñ´¦ÀíÁËÒ»Ìõ¼ÇÂ¼  1=ÊÇ */    
    {
          vtcp_log("[%s][%d]  ´¦ÀíÁËÒ»Ìõ¼ÇÂ¼£¬Ôò²»ÍùÏÂ×ßÁË£¬Ìø×ªµ½DELZ È¥×ßÕÊ !!!!!    iHandled=[%d]",__FILE__,__LINE__,iHandled);
          strcpy( g_pub_tx.reply, "0010" );        
          return 0;
    }  

      vtcp_log("[%s][%d] ¼ì²é dhccÓÐ µ«cpccÃ»ÓÐµÄÊý¾Ý ½áÊø!",__FILE__,__LINE__);
      vtcp_log("[%s][%d] ¼ì²é dhccÓÐ µ«cpccÃ»ÓÐµÄÊý¾Ý ½áÊø!",__FILE__,__LINE__);
          
      /*****************************************************************
      * 2  ¼ì²éccpcÓÐµÄµ«dhccÃ»ÓÐµÄ ( ·¢Éú¶îÓÐ³öÈëµÄÇé¿ö ²»¿¼ÂÇ)       *
      *****************************************************************/                
      vtcp_log("[%s][%d]¿ªÊ¼¼ì²éccpcÓÐµÄµ«dhccÃ»ÓÐµÄµÄÊý¾Ý ",__FILE__,__LINE__);
      vtcp_log("[%s][%d]¿ªÊ¼¼ì²éccpcÓÐµÄµ«dhccÃ»ÓÐµÄµÄÊý¾Ý ",__FILE__,__LINE__);
        
        ret = iCheckHv_chkagemrTable(&iHandled); /* ¸ù¾ÝHv_zfºË¶Ôhv_chkagemr */
        vtcp_log("ret========[%d]",ret);
        if(ret)
        {
          vtcp_log("[%s][%d]======= iCheckHv_chkagemrTable() ³ö´í! ========",__FILE__,__LINE__);    
             sprintf(acErrMsg,"´ó¶îÖ§¸¶Ã÷Ï¸¶ÔÕÊ³ö´í[%d]",ret);
             WRITEMSG
             return(ret);
        }
        
      vtcp_log("[%s][%d]======= After iCheckHv_chkagemrTable()!!========",__FILE__,__LINE__);    
      vtcp_log("[%s][%d] ÊÇ·ñ´¦ÀíÁËÒ»Ìõ¼ÇÂ¼    iHandled=[%d]",__FILE__,__LINE__,iHandled);
    if( iHandled == 1)   /* ÊÇ·ñ´¦ÀíÁËÒ»Ìõ¼ÇÂ¼  1=ÊÇ */    
    {
          vtcp_log("[%s][%d]  ´¦ÀíÁËÒ»Ìõ¼ÇÂ¼£¬Ôò²»ÍùÏÂ×ßÁË£¬Ìø×ªµ½DELZ È¥×ßÕÊ !!!!!    iHandled=[%d]",__FILE__,__LINE__,iHandled);
          strcpy( g_pub_tx.reply, "0010" );        
          return 0;
    }
       vtcp_log("[%s][%d] ¼ì²éccpcÓÐµÄµ«dhccÃ»ÓÐµÄµÄÊý¾Ý ½áÊø! ",__FILE__,__LINE__);
      vtcp_log("[%s][%d] ¼ì²éccpcÓÐµÄµ«dhccÃ»ÓÐµÄµÄÊý¾Ý ½áÊø! ",__FILE__,__LINE__);
   
      strcpy( g_pub_tx.reply, "0000" );         /* µ½ÕâÀïÁË£¬±íÊ¾¶¼´¦ÀíÍêÁË*/    
    set_zd_long("0510",1);                 /* µ½ÕâÀïÁË£¬±íÊ¾¶ÔÕÊ³É¹¦ÁË*/    

    vtcp_log("[%s][%d] ========= ¶¼´¦ÀíÍêÁË  ==========  ",__FILE__,__LINE__);
    vtcp_log("[%s][%d] ========= ¶¼´¦ÀíÍêÁË  ==========  ",__FILE__,__LINE__);

      return 0;
}     
    
/*** ÅÐ¶Ï¶ÔÕÊÃ÷Ï¸ÊÇ·ñÏÂÔØ  ÒÔ¼°ºÍ×Ü¼ÆºË¶Ô±¨ÎÄÊÇ·ñ¶Ô×Ü ***/
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
           sprintf(acErrMsg,"´ó¶îÒµÎñºË¶ÔÖ÷±íÎÞ¼ÇÂ¼");
           WRITEMSG
           return -1;
       }
       else if( g_reply_int )
       {
           sprintf(acErrMsg,"²éÑ¯´ó¶îÒµÎñºË¶ÔÖ÷±íÊý¾Ý¿â²Ù×÷Ê§°Ü[%d]",g_reply_int);
           WRITEMSG
           return -1;
       }     
 
    /* 1 ²éºË¶ÔÃ÷Ï¸±íÖÐ ÈËÐÐÀ´/ÍùÕÊ×Ü±ÊÊý, À´/ÍùÕÊ×Ü½ð¶î  */   
    ret = Hv_chkagemr_Dec_Sel(g_pub_tx.reply,"tx_date = %ld and cmtno <> '232' ",g_pub_tx.tx_date);
    if( ret )
      {
          sprintf(acErrMsg,"´ó¶îÀ´ÕÊÊý¾Ý¿â²Ù×÷Ê§°Ü[%d]",ret);
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
                if(iFlag == 0)  /* Ò»Ìõ¼ÇÂ¼Ò²Ã»ÓÐ */
                {
                         Hv_chkagemr_Clo_Sel();
                 vtcp_log("[%s][%d] µÃµ½ ºË¶ÔÖ÷±íÖÐÍùÕË×Ü±ÊÊý CCPC.send_cnt=[%d]  ",__FILE__,__LINE__,hv_chkmr.send_cnt);    
                 vtcp_log("[%s][%d] µÃµ½ ºË¶ÔÖ÷±íÖÐÀ´ÕÊ×Ü±ÊÊý CCPC.recevice_cnt=[%d] ",__FILE__,__LINE__,hv_chkmr.recevice_cnt);    
                 if(hv_chkmr.send_cnt +    hv_chkmr.recevice_cnt > 0 ) 
                 {
                        /* ½ñÌìÓÐÒµÎñ£¬ËµÃ÷ÊÇ689»¹Ã»ÊÕµ½!!  */
                         sprintf(acErrMsg,"½ñÌìÃ÷Ï¸Î´ÏÂÔØ³É¹¦ !");
                     WRITEMSG
                     *iResult = 8;
                         return -1;                          
                 }   
                 vtcp_log("[%s][%d]=  ½ñÌìÒ»±ÊÒµÎñ¶¼Ã»ÓÐ !!! ",__FILE__,__LINE__);  
                }
                vtcp_log("[%s][%d]=¼ÆËãÀ´ÕÊ×Ü±ÊÊý£¬À´ÕÊ×Ü½ð¶î Íê±Ï ",__FILE__,__LINE__);  
                break;                                    
          } 
          else if(ret)
          {
                sprintf(acErrMsg,"Êý¾Ý¿â²Ù×÷Ê§°Ü [%d]",ret);              
                WRITEMSG
                 return -1;
          }
          
          iFlag ++;
          
        ret = memcmp(hv_chkagemr.pay_qs_no, HV_QSBRNO, sizeof(hv_chkagemr.pay_qs_no)-1);
        ret2 = memcmp(hv_chkagemr.cashqsactno, HV_QSBRNO, sizeof(hv_chkagemr.cashqsactno)-1);
        if(ret && ret2)
        {
            vtcp_log("[%s][%d] ½ÓÊÕÇåËãÐÐºÍ·¢ÆðÇåËãÐÐ¶¼²»Îª±¾ÐÐÇåËãÐÐ!!! ret=[%d]ret2=[%d]",__FILE__,__LINE__,ret,ret2);
            sprintf(acErrMsg,"½ÓÊÕÇåËãÐÐºÍ·¢ÆðÇåËãÐÐ¶¼²»Îª±¾ÐÐÇåËãÐÐ!! ret=[%d] ret2=[%d]",ret,ret2);
            WRITEMSG
              return -1;    
        }         
        if(ret==0 && ret2==0)
        {
            vtcp_log("[%s][%d] ½ÓÊÕÇåËãÐÐºÍ·¢ÆðÇåËãÐÐ¶¼ÊÇ±¾ÐÐÇåËãÐÐ!!! ret=[%d]ret2=[%d]",__FILE__,__LINE__,ret,ret2);
            sprintf(acErrMsg,"½ÓÊÕÇåËãÐÐºÍ·¢ÆðÇåËãÐÐ¶¼ÊÇ±¾ÐÐÇåËãÐÐ!! ret=[%d] ret2=[%d]",ret,ret2);
            WRITEMSG
              return -1;    
        }      
        if(ret == 0)  /* ÍùÕÊ */
        {                    
              iSendCnt = iSendCnt + 1;
              dSendAmt = dSendAmt + hv_chkagemr.tx_amt;    
           }
        else    /* À´ÕÊ */
        {
              iReceiveCnt = iReceiveCnt + 1;
              dReceiveAmt = dReceiveAmt + hv_chkagemr.tx_amt;
        }    
    }    
      Hv_chkagemr_Clo_Sel();

    vtcp_log("[%s][%d] µÃµ½ ºË¶ÔÃ÷Ï¸±íÖÐ ÍùÕË×Ü±ÊÊý iSendCnt=[%d] ",__FILE__,__LINE__,iSendCnt);    
    vtcp_log("[%s][%d] µÃµ½ ºË¶ÔÃ÷Ï¸±íÖÐ ÍùÕË×Ü½ð¶î dSendAmt=[%lf] ",__FILE__,__LINE__,dSendAmt);    
    vtcp_log("[%s][%d] µÃµ½ ºË¶ÔÃ÷Ï¸±íÖÐ À´ÕÊ×Ü±ÊÊý iReceiveCnt=[%d] ",__FILE__,__LINE__,iReceiveCnt);    
    vtcp_log("[%s][%d] µÃµ½ ºË¶ÔÃ÷Ï¸±íÖÐ À´ÕÊ×Ü½ð¶î dReceiveAmt=[%lf] ",__FILE__,__LINE__,dReceiveAmt);    

    /* 2 ²éºË¶ÔÖ÷±íÖÐ ÈËÐÐÀ´/ÍùÕÊ×Ü±ÊÊý, À´/ÍùÕÊ×Ü½ð¶î  */
    /*   ÌáÇ°µ½ÉÏÃæÁË    2006-11-4 23:22
    memset(&hv_chkmr,0,sizeof(struct hv_chkmr_c) );    
       g_reply_int = Hv_chkmr_Sel(g_pub_tx.reply,&hv_chkmr," 1=1 ");
       if( g_reply_int==100 )
       {
           sprintf(acErrMsg,"´ó¶îÒµÎñºË¶ÔÖ÷±íÎÞ¼ÇÂ¼");
           WRITEMSG
           return -1;
       }
       else if( g_reply_int )
       {
           sprintf(acErrMsg,"²éÑ¯´ó¶îÒµÎñºË¶ÔÖ÷±íÊý¾Ý¿â²Ù×÷Ê§°Ü[%d]",g_reply_int);
           WRITEMSG
           return -1;
       } */    
    vtcp_log("[%s][%d] µÃµ½ ºË¶ÔÖ÷±íÖÐÍùÕË×Ü±ÊÊý CCPC.send_cnt=[%d]  ",__FILE__,__LINE__,hv_chkmr.send_cnt);    
    vtcp_log("[%s][%d] µÃµ½ ºË¶ÔÖ÷±íÖÐÍùÕË×Ü½ð¶î CCPC.send_amt=[%lf]",__FILE__,__LINE__,hv_chkmr.send_amt);    
    vtcp_log("[%s][%d] µÃµ½ ºË¶ÔÖ÷±íÖÐÀ´ÕÊ×Ü±ÊÊý CCPC.recevice_cnt=[%d] ",__FILE__,__LINE__,hv_chkmr.recevice_cnt);    
    vtcp_log("[%s][%d] µÃµ½ ºË¶ÔÖ÷±íÖÐÀ´ÕÊ×Ü½ð¶î CCPC.recevice_amt=[%lf] ",__FILE__,__LINE__,hv_chkmr.recevice_amt);    

    /* 3 ÅÐ¶Ï ¶ÔÕÊÃ÷Ï¸ ºÍ ×Ü¼ÆºË¶Ô±¨ÎÄ ÊÇ·ñ¶Ô×Ü  */
    if( (hv_chkmr.send_cnt == iSendCnt) && (hv_chkmr.recevice_cnt == iReceiveCnt) && 
          (hv_chkmr.send_amt-dSendAmt > -0.0001 || hv_chkmr.send_amt-dSendAmt <0.0001)  && 
        (hv_chkmr.recevice_amt-dReceiveAmt > -0.0001 || hv_chkmr.recevice_amt-dReceiveAmt < 0.0001) )
    {
          vtcp_log("[%s][%d] ==== ¶ÔÕÊÃ÷Ï¸ÒÑÏÂÔØ  ºÍ×Ü¼ÆºË¶Ô±¨ÎÄÏàµÈ!! =====",__FILE__,__LINE__);    
              return 0;
    }
    else
    {
               sprintf(acErrMsg,"´ó¶îÒµÎñºË¶ÔÖ÷±íÎÞ¼ÇÂ¼");
               WRITEMSG
               *iResult = 9;
              return -1;
    }
       return 0; 
}
/*******************************************************************************
* º¯ÊýÃû³Æ:iCheckHvForDateOver                                                 *
* Ãè    Êö: ¼ì²é´ó¶îÖ§¸¶ÒµÎñÖ÷±íÖÐÊÇ·ñ´æÔÚÒÑ¾­¸´ºË¼ÇÕÊµ«Î´·¢ËÍ»òµÈ´ýÇåËã       *
*          È·ÈÏ»òÅÅ¶ÓµÄÕÊÎñ                                                    *
* ²Î    Êý:chk_br_no , iErrCnt                                                 *
* Àà    ÐÍ:char *    , int *                                                   *
* ·µ »Ø Öµ:int                                                                 *
* -----------------------------------------------------------------------------*
* ÊäÈë :Òª¼ì²éµÄ»ú¹¹ºÅ,Îª¿Õ±íÊ¾È«²¿                                            *
* Êä³ö :·µ»ØÖµ == 0, iErrCnt ·µ»Ø´æÔÚµÄ±ÊÊý,·µ»ØÖµ !=0 ±íÊ¾Êý¾Ý¿â´íÎó          *
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
              sprintf(acErrMsg,"Êý¾Ý¿â²Ù×÷´íÎó!");
              WRITEMSG
                     return -1;                 
         }      
      
   }
   else
   {
         ret = Hv_zf_Dec_Sel(g_pub_tx.reply," tx_date=%d and hv_sts='0' and br_no = '%s' ",tx_date, chk_br_no);
       if( ret )
         {
              sprintf(acErrMsg,"Êý¾Ý¿â²Ù×÷´íÎó!");
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
         sprintf(acErrMsg,"Êý¾Ý¿â²Ù×÷´íÎó!");
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
* º¯ÊýÃû³Æ:iCheckHv_zfTable                                                    *
* Ãè    Êö: ¸ù¾Ýhv_chkagemrºË¶ÔHv_zf
            ¼ì²é dhccÓÐ µ«cpccÃ»ÓÐ µÄÇé¿ö                                      *
            ÎÒÓÐCCPCÃ»ÓÐµÄÇé¿ö,³å»ÙÎÒ·½µÄÕÊÎñ                                  *
* ²Î    Êý:                                                                    *
* Àà    ÐÍ:                                                                    *
* ·µ »Ø Öµ:int                                                                 *
* -----------------------------------------------------------------------------*
* ÊäÈë :                                                                       *
* Êä³ö :                                                                       *
*******************************************************************************/
int iCheckHv_zfTable(int* iHandled)
{
      int ret;
      struct hv_zf_c hv_zf;
      struct hv_chkagemr_c hv_chkagemr;
      char cAcno110[20];
      int iCmtno=0;
                  
      vtcp_log("[%s][%d] ***** iCheckHv_zfTable() ¿ªÊ¼ !  ***** ",__FILE__,__LINE__);
      vtcp_log("[%s][%d] ***** iCheckHv_zfTable() ¿ªÊ¼ !  ***** ",__FILE__,__LINE__);
      /* ÎÒ·½²»¼ÇÕÊµÄÇé¿ö²»ÐèÒª¿¼ÂÇ£¬ ËùÒÔÖ»²é³ö¼ÇÕÊµÄÇé¿ö           */
      /* iCheckHv_chkagemrTable()ÖÐ²åÈëµÄ¼ÇÂ¼ checkflag= 'D',²»ÔÙ´¦Àí    */
      /* Ôö¼ÓÁËÒ»¸ö¸´ºË×´Ì¬ '0' Ò²×ßÕÊÁË   2006-10-30 23:07
      ret = Hv_zf_Dec_Upd(g_pub_tx.reply, 
      "tx_date=%d   and ( hv_sts not in ('5','6','A','C','8','D','L','K') ) ", g_pub_tx.tx_date); */
      ret = Hv_zf_Dec_Upd(g_pub_tx.reply, 
      "tx_date=%d   and ( hv_sts  in ('0','1','2','3','4','7','9','B','F','G','H','I','J') ) and (checkflag is null) and cmtno!='802' and cmtno!='812' and cmtno!='814'  and cmtno !='232' ", g_pub_tx.tx_date);       
      if ( ret )
      {
            sprintf(acErrMsg,"Hv_zf_Dec_Sel() ³ö´í[%d]",ret);
            WRITEMSG
              return ret;
      }          
        
    while(1)
    {
        memset(&hv_zf,0, sizeof(hv_zf));        
        ret = Hv_zf_Fet_Upd(&hv_zf, g_pub_tx.reply);
        if( ret==100 )
        {
            vtcp_log("[%s][%d] ´ó¶îÖ§¸¶ÍùÕÊ¼ÇÂ¼Ñ­»·´¦ÀíÍê±Ï  ",__FILE__,__LINE__);    
            Hv_zf_Clo_Sel();                      
            break;
        }
        else if(ret)
        {
            vtcp_log("[%s][%d]Êý¾Ý¿â²Ù×÷Òì³£,ÇëÓë¿Æ¼¼¿ÆÁªÏµ!",__FILE__,__LINE__);
            sprintf(acErrMsg,"Êý¾Ý¿â²Ù×÷Òì³£,ÇëÓë¿Æ¼¼¿ÆÁªÏµ![%d]",ret);
            WRITEMSG
            return ret;
        }
        
        memset(&hv_chkagemr,0, sizeof( hv_chkagemr ) );
        ret = Hv_chkagemr_Dec_Upd(g_pub_tx.reply, " or_br_no='%s' and orderno='%s' and ( chk_flg ='0' or chk_flg is null )", hv_zf.or_br_no,hv_zf.orderno); 
        if(ret)
        {
            vtcp_log("[%s][%d]Hv_chkagemr_Dec_Upd() ³ö´í!",__FILE__,__LINE__);
                sprintf(acErrMsg,"Hv_chkagemr_Dec_Upd() ³ö´í![%d]",ret);
                WRITEMSG
                  return ret;
        } 
        ret = Hv_chkagemr_Fet_Upd(&hv_chkagemr, g_pub_tx.reply);
        if(ret && ret != 100)
        {
            vtcp_log("[%s][%d]Êý¾Ý¿â²Ù×÷Òì³£,ÇëÓë¿Æ¼¼¿ÆÁªÏµ!",__FILE__,__LINE__);
                sprintf(acErrMsg,"Êý¾Ý¿â²Ù×÷Òì³£,ÇëÓë¿Æ¼¼¿ÆÁªÏµ![%d]",ret);
                WRITEMSG
                  return ret;
        }else if(ret == 100)  /* dhccÓÐ µ«cpccÃ»ÓÐ ³å»ÙÎÒ·½µÄÕÊÎñ */
        {
                  /***********  ½«¸Ã±ÊÍùÕÊ×´Ì¬ÖÃÎª REJECT_EACTCLS   *********/
                  
                  if(hv_zf.lw_ind[0] == '1')  /* ÍùÕÊ*/
                  {
                     hv_zf.beg_sts[0] = hv_zf.hv_sts[0]; 
                       hv_zf.hv_sts[0] = '5';        /* ÖÃÎªÍùÕÊ¾Ü¾ø */
                  }
                  else  /* À´ÕÊ*/
                  {
                       hv_zf.hv_sts[0] = 'L';        /* ºË¶Ô²»·û ³åÕÊ */
                  } 
           
            hv_zf.checkflag[0] = 'B';     /* DHCCÓÐ£¬CCPCÃ»ÓÐ */       
            ret=Hv_zf_Upd_Upd(hv_zf, g_pub_tx.reply);
            if( ret)
            {
                vtcp_log("[%s][%d] Êý¾Ý¿â²Ù×÷Òì³£,ÇëÓë¿Æ¼¼¿ÆÁªÏµ!  ",__FILE__,__LINE__);    
                    sprintf(acErrMsg,"Êý¾Ý¿â²Ù×÷Òì³£,ÇëÓë¿Æ¼¼¿ÆÁªÏµ![%d]",ret);
                    WRITEMSG
                      return -1;
            } 
                 vtcp_log("[%s][%d] ***** ¸üÐÂÍùÕÊ×´Ì¬³É¹¦ !  ***** ",__FILE__,__LINE__);
                 vtcp_log("[%s][%d] hv_zf.cmtno=[%s] ",__FILE__,__LINE__,hv_zf.cmtno);

            iCmtno = atoi(hv_zf.cmtno);
            
            /* »ãÆ±ÀàDHCC¶àÕÊ ÐèÒª¸üÐÂ»ãÆ±µÄ×´Ì¬ (CMT 121 122 123 124) */
            if(iCmtno==121 || iCmtno==122 || iCmtno==123 || iCmtno==124 )
            {  
                    vtcp_log("[%s,%d] »ãÆ±ÀàDHCC¶àÕÊ ÐèÒª¸üÐÂ»ãÆ±µÄ×´Ì¬ !",__FILE__,__LINE__);                        
                          ret = iHv_DZ_ChangeHPStat(&hv_zf,iCmtno);
                  if(ret)
                  {
                        vtcp_log("%s,%d ¸üÐÂ»ãÆ±ÐÅÏ¢±í´íÎó ret=[%d]",__FILE__,__LINE__,ret);
                        vtcp_log(acErrMsg,"¸üÐÂ»ãÆ±ÐÅÏ¢±í´íÎó");
                        WRITEMSG
                        return ret;
                  }        
                    vtcp_log("[%s,%d] ¸üÐÂ»ãÆ±×´Ì¬³É¹¦!  ",__FILE__,__LINE__);        
            }

                  /***********   µÇ¼Ç²î´í±í     ***************/                          
            ret = iInsertHv_chkerror(&hv_zf,&hv_chkagemr);
            if(ret)
            {
                vtcp_log("[%s][%d]Êý¾Ý¿â²Ù×÷Òì³£,ÇëÓë¿Æ¼¼¿ÆÁªÏµ!",__FILE__,__LINE__);
                    sprintf(acErrMsg,"Êý¾Ý¿â²Ù×÷Òì³£,ÇëÓë¿Æ¼¼¿ÆÁªÏµ![%d]",ret);
                    WRITEMSG
                      return ret;
            }  
          
            vtcp_log("[%s][%d] ********************************* ",__FILE__,__LINE__);
            vtcp_log("[%s][%d] ÎÒÓÐCCPCÃ»ÓÐµÄÇé¿ö,³å»ÙÎÒ·½µÄÕÊÎñ ",__FILE__,__LINE__);
            vtcp_log("[%s][%d] ÎÒÓÐCCPCÃ»ÓÐµÄÇé¿ö,³å»ÙÎÒ·½µÄÕÊÎñ ",__FILE__,__LINE__);
            vtcp_log("[%s][%d] ********************************* ",__FILE__,__LINE__);
            /* begin add by LiuHuafeng 2009-5-14 17:01:51 for ÑôÈª´åÕòÒøÐÐ */
            if(strncmp(hv_zf.br_no,HV_CUNZHEN_BR_NO,BRNO_LEN-2))
            {
            /* end by LiuHuafeng 2009-5-14 17:02:02 ÑôÈª´åÕòÒøÐÐ ´åÕòÒøÐÐµÄ²»½øÐÐÕËÎñ´¦Àí£¬²»ÉèÖÃ8583Óò */
                /*******  ÖÃ8583Óò  ³å»ÙÎÒ·½µÄÕÊÎñ  ¸º½ð¶î×ßÕÊ *******/  
                if(iCmtno==100 || iCmtno==101 || iCmtno==102 || iCmtno==103 || 
                     iCmtno==105 || iCmtno==108 || iCmtno==122) 
                {  /* ÆÕÍ¨Àà: (CMT 100 101 102 103 105 108 122)*/
                    
                    vtcp_log("[%s][%d] ÆÕÍ¨Àà ¸º½ð¶î×ßÕÊ ",__FILE__,__LINE__);
                      
                    ret = iSet8583_For_CommonCMT(&hv_zf,-1);  /*** -1=¸º½ð¶î×ßÕÊ ***/
                    if(ret)
                    {
                        vtcp_log("[%s][%d]iSet8583_For_CommonCMT() ³ö´í!",__FILE__,__LINE__);
                        sprintf(acErrMsg,"ÉèÖÃ8583ÓòÊ±³ö´í![%d]",ret);
                        WRITEMSG
                        return -1;
                    }  
                
                }
                else  if(iCmtno==121 || iCmtno==123 || iCmtno==124 )
                {  /*  ÌØÊâÀà (CMT 121 123 124)  */
                
                         vtcp_log("[%s][%d] ÌØÊâÀà  ¸º½ð¶î×ßÕÊ ",__FILE__,__LINE__);   /*  tx_amt: Î´ÓÃ½ð¶î    jf_amt: Æ±Ãæ½ð¶î */
                      ret = iSet8583_For_SpecialCMT(&hv_zf, hv_chkagemr.p_num, hv_chkagemr.p_date,hv_chkagemr.jf_amt,-1);  /*** -1=¸º½ð¶î×ßÕÊ ***/
                      if(ret)
                      {
                        vtcp_log("[%s][%d]iSet8583_For_SpecialCMT() ³ö´í!",__FILE__,__LINE__);
                            sprintf(acErrMsg,"ÉèÖÃ8583ÓòÊ±³ö´í![%d]",ret);
                            WRITEMSG
                              return -1;
                      }    
                }
                else
                {
                         sprintf(acErrMsg," ¸Ã±¨ÎÄ²»ÔÚÕâÀï´¦Àí !! iCmtno[%d]",iCmtno);
                         WRITEMSG
                           return -1;                
                }
            }
            /* ½«chk_flgÖÃÎªCCPC_SAME_BANK£¬ÔÚiCheckHv_chkagemrTable()ÖÐ¾Í²»ÓÃ´¦ÀíÕâÐ©¼ÇÂ¼ÁË */          
            hv_chkagemr.chk_flg[0]='B'; /* DHCCÓÐ£¬CCPCÃ»ÓÐ */
            ret = Hv_chkagemr_Upd_Upd(hv_chkagemr, g_pub_tx.reply);
            if(ret)
            {
                vtcp_log("[%s][%d]Êý¾Ý¿â²Ù×÷Òì³£,ÇëÓë¿Æ¼¼¿ÆÁªÏµ!",__FILE__,__LINE__);
                sprintf(acErrMsg,"Êý¾Ý¿â²Ù×÷Òì³£,ÇëÓë¿Æ¼¼¿ÆÁªÏµ![%d]",ret);
                WRITEMSG
                return ret;
            }
            
            Hv_chkagemr_Clo_Upd();
            vtcp_log("[%s][%d] ***** iCheckHv_zfTable() ³É¹¦ !  ***** ",__FILE__,__LINE__);
            /* begin add by LiuHuafeng 2009-5-14 17:01:51 for ÑôÈª´åÕòÒøÐÐ */
            if(strncmp(hv_zf.br_no,HV_CUNZHEN_BR_NO,BRNO_LEN-2))
            {
            /* end by LiuHuafeng 2009-5-14 17:02:02 ÑôÈª´åÕòÒøÐÐ ´åÕòÒøÐÐµÄ²»½øÐÐÕËÎñ´¦Àí£¬
             *²»ÉèÖÃ8583Óò
             */
            /* end by LiuHuafeng 2009-5-14 17:03:28 */
                Hv_zf_Clo_Upd();
                *iHandled = 1;   /* ´¦ÀíÁËÒ»Ìõ¼ÇÂ¼  !! */
                return 0;
            /* begin add by LiuHuafeng 2009-5-14 17:04:06 for ÑôÈª´åÕòÒøÐÐ */
            }
            else
            {
                vtcp_log("%s %d ´åÕòÒøÐÐ¶à£¬ÉÌÐÐÖ»×ö±êÊ¶£¬²»´¦Àí ",__FILE__,__LINE__);
                *iHandled = 0;
                continue;
            }
            /* end by LiuHuafeng 2009-5-14 17:04:29 */
        }

        /* ÀÏÁõËµ²»´¦ÀíË«·½ÈëÕÊ½ð¶î²»Ïà·ûºÏµÄÇé¿ö  */                        
        vtcp_log("[%s][%d] ÎÒ·½ÓÐµ«CCPCÒ²ÓÐµÄÇé¿ö ",__FILE__,__LINE__);
        vtcp_log("[%s][%d] hv_zf.tx_amt===[%lf] ",__FILE__,__LINE__,hv_zf.tx_amt);
        vtcp_log("[%s][%d] hv_chkagemr.tx_amt===[%lf] ",__FILE__,__LINE__,hv_chkagemr.tx_amt);

        hv_zf.checkflag[0] = 'A';    /* DHCCºÍCCPCÏàÍ¬ */
        ret = Hv_zf_Upd_Upd(hv_zf, g_pub_tx.reply);
        if( ret)
        {
            vtcp_log("[%s][%d] Êý¾Ý¿â²Ù×÷Òì³£,ÇëÓë¿Æ¼¼¿ÆÁªÏµ!  ",__FILE__,__LINE__);    
            sprintf(acErrMsg,"Êý¾Ý¿â²Ù×÷Òì³£,ÇëÓë¿Æ¼¼¿ÆÁªÏµ![%d]",ret);
            WRITEMSG
              return -1;
        }
        vtcp_log("[%s][%d] ***** ¸üÐÂÍùÕÊ×´Ì¬³É¹¦ !  ***** ",__FILE__,__LINE__);
          
        /* ½«chk_flgÖÃÎªCCPC_SAME_BANK£¬ÔÚiCheckHv_chkagemrTable()ÖÐ¾Í²»ÓÃ´¦ÀíÕâÐ©¼ÇÂ¼ÁË */          
        hv_chkagemr.chk_flg[0]= 'A';  /* CCPC_SAME_BANK DHCCºÍCCPCÏàÍ¬ */
        ret = Hv_chkagemr_Upd_Upd(hv_chkagemr, g_pub_tx.reply);
        if(ret)
        {
            vtcp_log("[%s][%d]Êý¾Ý¿â²Ù×÷Òì³£,ÇëÓë¿Æ¼¼¿ÆÁªÏµ!",__FILE__,__LINE__);
            sprintf(acErrMsg,"Êý¾Ý¿â²Ù×÷Òì³£,ÇëÓë¿Æ¼¼¿ÆÁªÏµ![%d]",ret);
            WRITEMSG
              return ret;
        } 
        vtcp_log("[%s][%d] *****ÕâÒ»±ÊDHCCºÍCCPC¶¼ÓÐ£¬¼ÌÐøÑ­»·ÍùÏÂÕÒ !  ***** ",__FILE__,__LINE__);     
      }
      
      Hv_chkagemr_Clo_Upd(); 
      Hv_zf_Clo_Upd();   
    vtcp_log("[%s][%d] ***** iCheckHv_zfTable() ³É¹¦ !  ***** ",__FILE__,__LINE__);
    vtcp_log("[%s][%d] ***** iCheckHv_zfTable() ³É¹¦ !  ***** ",__FILE__,__LINE__);
      return 0;
}


/*******************************************************************************
* º¯ÊýÃû³Æ: iCheckHv_chkagemrTable                                             *
* Ãè    Êö: ¸ù¾Ýhv_chkagemrºË¶ÔHv_zf¼ì²éccpcÓÐ µ«dhccÃ»ÓÐ µÄÇé¿ö 
            CCPCÓÐ dhccÃ»ÓÐ µÄÇé¿ö,                                            *
            À´ÕÊ£º°´ÕÕccpc²åÈëhv_zf£¬×ßÕÊ
            ÍùÕÊ£ºÕÒµ½Ô­À´ÄÇÌõÍùÕÊ£¬ÖÃ±êÖ¾£¬×ßÕÊ                               *
* ²Î    Êý:                                                                    *
* Àà    ÐÍ:                                                                    *
* ·µ »Ø Öµ:int                                                                 *
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
      int iLw_ind = 0;  /* iLw_ind=1ÍùÕÊ     iLw_ind=2À´ÕÊ*/
      char cKinbr[BRNO_LEN+1];
      int iCzBankFlag = 0;/* ÊÇ·ñÊÇ´åÕòÒøÐÐÕËÎñ0=·ñ    1=ÊÇ */
      
      
    /* dhccºÍcpcc¶¼ÓÐµÄÇé¿ö£¬ÒÑÔÚiCheckHv_zfTable()ÖÐ½«checkflagÖÃÎª 'A'£¬ 
       ËùÒÔÖ»²é³öcheckflag='0' or checkflag is null µÄÇé¿ö, ¾ÍÊÇ CCPCÓÐ dhccÃ»ÓÐ µÄÇé¿ö */
      ret = Hv_chkagemr_Dec_Upd(g_pub_tx.reply, "tx_date=%d   and ( chk_flg ='0' or chk_flg is null ) and cmtno<>'232'  ", g_pub_tx.tx_date);
      if ( ret )
      {
            sprintf(acErrMsg,"Hv_chkagemr_Dec_Sel() ³ö´í[%d]",ret);
            WRITEMSG
              return ret;
      }          
        
    while(1)
    {
        memset(&hv_chkagemr,0, sizeof( hv_chkagemr ) );               
        ret=Hv_chkagemr_Fet_Upd(&hv_chkagemr, g_pub_tx.reply);
        if( ret==100 )
        {
            vtcp_log("[%s][%d] ´ó¶îÖ§¸¶¶ÔÕÊÃ÷Ï¸±íÑ­»·´¦ÀíÍê±Ï  ",__FILE__,__LINE__); 
              Hv_zf_Clo_Sel();                      
            break;
        }
        else if(ret)
        {
            vtcp_log("[%s][%d]Êý¾Ý¿â²Ù×÷Òì³£,ÇëÓë¿Æ¼¼¿ÆÁªÏµ!",__FILE__,__LINE__);
                sprintf(acErrMsg,"Êý¾Ý¿â²Ù×÷Òì³£,ÇëÓë¿Æ¼¼¿ÆÁªÏµ![%d]",ret);
                WRITEMSG
                  return ret;
        }
        
        vtcp_log("[%s][%d] hv_chkagemr.pay_ac_no==[%s]  hv_chkagemr.cash_ac_no==[%s]  ",__FILE__,__LINE__, hv_chkagemr.pay_ac_no,hv_chkagemr.cash_ac_no); 
        
        
                /* ------¸ù¾Ý PAY_QS_NO ÅÐ¶ÏÊÇÀ´ÕÊ»¹ÊÇÍùÕÊ ---------------------------------*/
        /* ----- PAY_QS_NO Îª±¾ÐÐÇåËãÐÐµÄÎªÍùÕÊ£¬·ñÔòÎªÀ´ÕÊ  -------------------- */
        vtcp_log("[%s][%d]  hv_chkagemr.pay_qs_no==[%s] hv_chkagemr.cashqsactno=[%s] HV_QSBRNO=[%s]",__FILE__,__LINE__,hv_chkagemr.pay_qs_no,hv_chkagemr.cashqsactno,HV_QSBRNO);        
        
        ret = memcmp(hv_chkagemr.pay_qs_no, HV_QSBRNO, sizeof(hv_chkagemr.pay_qs_no)-1);
        ret2 = memcmp(hv_chkagemr.cashqsactno, HV_QSBRNO, sizeof(hv_chkagemr.cashqsactno)-1);
        if(ret && ret2)
        {
            vtcp_log("[%s][%d] ½ÓÊÕÇåËãÐÐºÍ·¢ÆðÇåËãÐÐ¶¼²»Îª±¾ÐÐÇåËãÐÐ!!! ret=[%d]ret2=[%d]",__FILE__,__LINE__,ret,ret2);
            sprintf(acErrMsg,"½ÓÊÕÇåËãÐÐºÍ·¢ÆðÇåËãÐÐ¶¼²»Îª±¾ÐÐÇåËãÐÐ!! ret=[%d] ret2=[%d]",ret,ret2);
            WRITEMSG
               return -1;    
        } 
        
        if(ret==0 && ret2==0)
        {
            vtcp_log("[%s][%d] ½ÓÊÕÇåËãÐÐºÍ·¢ÆðÇåËãÐÐ¶¼ÊÇ±¾ÐÐÇåËãÐÐ!!! ret=[%d]ret2=[%d]",__FILE__,__LINE__,ret,ret2);
            sprintf(acErrMsg,"½ÓÊÕÇåËãÐÐºÍ·¢ÆðÇåËãÐÐ¶¼ÊÇ±¾ÐÐÇåËãÐÐ!! ret=[%d] ret2=[%d]",ret,ret2);
            WRITEMSG
               return -1;    
        }  
         
        if(ret)  /* À´ÕÊ */
        {                    
             iLw_ind = 2;
          }
        else  /* ÍùÕÊ */
        {
             iLw_ind = 1;          
        }    
        vtcp_log("[%s][%d] iLw_ind=========[%d]  ",__FILE__,__LINE__, iLw_ind);   
        vtcp_log("[%s][%d] hv_chkagemr.pay_ac_no==[%s]  hv_chkagemr.cash_ac_no==[%s]  ",__FILE__,__LINE__, hv_chkagemr.pay_ac_no,hv_chkagemr.cash_ac_no); 

        
        /* ----- 1. À´ÕÊ£º(Ã»ÊÕµ½) °´ÕÕccpc²åÈëhv_zf£¬×ßÕÊ            ------------- */ 
        /* ----- 2. ÍùÕÊ£º(¼«ÉÙ¼û) ¿ÉÄÜÊÇÍùÕÊ¾Ü¾ø£¬ccpcÓÖÇåËãÁË£¬    ---------------*/
        /* ----- ËùÒÔÕÒµ½Ô­À´ÄÇÌõÍùÕÊ£¬ÖÃ±êÖ¾£¬×ßÕÊ                  -------------- */  
        /* ----- ÕâÀïÖ»¸ù¾Ý¹Ø¼ü×Ö²éÕÒ£¬ÒòÎªÈç¹û´æÔÚÕâÌõ¼ÇÂ¼¾ÍÎÞ·¨²åÈë£¬Ö»ÄÜÐÞ¸Ä-----*/  
        ret = Hv_zf_Dec_Upd(g_pub_tx.reply,"or_br_no='%s' and orderno='%s' and  cmtno='%s' and wt_date=%d ", 
                 hv_chkagemr.or_br_no,hv_chkagemr.orderno,hv_chkagemr.cmtno,hv_chkagemr.tx_date );
        if(ret)
        {
                sprintf(acErrMsg,"Hv_zf_Dec_Upd() ³ö´í[%d]",ret);
                WRITEMSG
                  return ret;
        }
         
          memset(&hv_zf,0, sizeof(hv_zf));         
        ret = Hv_zf_Fet_Upd(&hv_zf,g_pub_tx.reply);
        if(ret && ret !=100)
        {
                sprintf(acErrMsg,"Hv_zf_Dec_Upd() ³ö´í[%d]",ret);
                WRITEMSG
                  return ret;
        } 
        if(ret == 100)   /* Ã»ÕÒµ½ DHCCÉÙÕÊ Ã»ÓÐÕâ±Ê½»Ò×µÄ¼ÇÂ¼  °´ÕÕccpc²åÈëhv_zf  Õý½ð¶î×ßÕÊ */
        {
              /* ==== °´ÕÕ hv_chkagemr ²åÈë hv_zf =======*/
              vtcp_log("[%s][%d] Ã»ÓÐÕâ±Ê½»Ò×µÄ¼ÇÂ¼ !",__FILE__,__LINE__);

            memset(&hv_zf,0,sizeof(hv_zf));    
            /*  
            vtcp_log("[%s][%d] ============ Hv_chkagemr_Debug() ========\n",__FILE__,__LINE__);    
            Hv_chkagemr_Debug(hv_chkagemr);
            vtcp_log("[%s][%d] ============ Hv_chkagemr_Debug() ========\n",__FILE__,__LINE__);    
            */      

            vtcp_log("[%s][%d] hv_chkagemr.pay_ac_no==[%s]  hv_chkagemr.cash_ac_no==[%s]  ",__FILE__,__LINE__, hv_chkagemr.pay_ac_no,hv_chkagemr.cash_ac_no); 
            vtcp_log("[%s][%d] hv_zf.pay_ac_no==[%s]  hv_zf.cash_ac_no==[%s]  ",__FILE__,__LINE__, hv_zf.pay_ac_no,hv_zf.cash_ac_no); 

            vtcp_log("[%s][%d] iLw_ind=========[%d]  ",__FILE__,__LINE__, iLw_ind);    
            if(iLw_ind == 2)  /* À´ÕÊ */
            {                    
              hv_zf.lw_ind[0] = '2'; 
            }
            else if(iLw_ind == 1)   /* ÍùÕÊ */
            {
              hv_zf.lw_ind[0] = '1';     
            }else
            {
              sprintf(acErrMsg,"À´Íù±êÊ¶´í[%d]",iLw_ind);
              WRITEMSG
                 return ret;
            }            
            vtcp_log("[%s][%d] hv_zf.lw_ind=[%s]  ",__FILE__,__LINE__,hv_zf.lw_ind);

            hv_zf.hv_sts[0] =  '9';  /* Ä¬ÈÏÎª À´ÕË¼ÇÕËÇåËã£¬¹ÒÕÊ¼ì²éÊ±ÔÙ¸ù¾ÝÇé¿öÖØÉè */
            hv_zf.beg_sts[0] = hv_zf.hv_sts[0]; 
               iCopyHv_zfFromHv_chkagemr(&hv_zf,&hv_chkagemr);
            
            vtcp_log("[%s][%d] hv_chkagemr.pay_ac_no==[%s]  hv_chkagemr.cash_ac_no==[%s]  ",__FILE__,__LINE__, hv_chkagemr.pay_ac_no,hv_chkagemr.cash_ac_no); 
            vtcp_log("[%s][%d] hv_zf.pay_ac_no==[%s]  hv_zf.cash_ac_no==[%s]  ",__FILE__,__LINE__, hv_zf.pay_ac_no,hv_zf.cash_ac_no); 

            /* ³åÕÊºó ÖÃÉÏ±êÖ¾£¬·ñÔòÏÂÒ»Ñ­»·Ê±»¹´¦Àí*/
            hv_chkagemr.chk_flg[0] = 'D';     /*NOT_FOUND_BANK  CCPCÓÐ£¬DHCCÃ»ÓÐ*/  
            /* begin add by LiuHuafeng 2009-5-14 17:09:11 */
            /* Èç¹ûÊÇ´åÕòÒøÐÐ£¬Ôò²»ÐèÒª½øÐÐÕËÎñ´¦Àí */
            if(!strncmp(hv_chkagemr.br_no,HV_CUNZHEN_BR_NO,BRNO_LEN-2))
            {
                iCzBankFlag = 1;
                vtcp_log("%s,%d ´åÕòÒøÐÐÕËÎñÉÙ£¬Ö»×öµÇ¼Ç²¿´¦Àí",__FILE__,__LINE__);
            }
            else
            {
                iCzBankFlag = 0;
            }
            /* end by LiuHuafeng for ´åÕòÒøÐÐ */
            *iHandled = 1;     /* Èç¹ûÊÇÉÌÒµÒøÐÐµÄÐèÒª×ßÕÊ£¬´åÕòÒøÐÐµÄ²»ÐèÒª×ßÕË */   
            iInsertFlag = 1;   /* ±êÖ¾ ²åÈë¼ÇÂ¼ */
        }
        else   /* ÕÒµ½ Ò²ÊôÓÚDHCCÉÙÕÊ !!   ÕÒµ½Õâ±Ê½»Ò×µÄ¼ÇÂ¼  ÐÞ¸Ä±êÖ¾ Õý½ð¶î×ßÕÊ */
        {
              if( hv_zf.hv_sts[0] == '0' || hv_zf.hv_sts[0] == '1' || hv_zf.hv_sts[0] == '2' || hv_zf.hv_sts[0] == '3' ||
                     hv_zf.hv_sts[0] == '4' || hv_zf.hv_sts[0] == '7' || hv_zf.hv_sts[0] == '9' ||
                     hv_zf.hv_sts[0] == 'B' || hv_zf.hv_sts[0] == 'F' || hv_zf.hv_sts[0] == 'G' || 
                     hv_zf.hv_sts[0] == 'H' || hv_zf.hv_sts[0] == 'I' || hv_zf.hv_sts[0] == 'J'  )
              {      /*  ÕâÖÖÇé¿öÓ¦¸ÃÊÇ²»¿ÉÄÜµÄ£¬ÕâÀï»¹ÊÇÅÐ¶ÏÒ»ÏÂ */
                    vtcp_log("[%s][%d] ==== ÕâÀï³ö´íÁË£¬DHCCºÍCCPC¶¼ÓÐ£¬Ó¦¸ÃÔÚ iCheckHv_zfTable()ÖÐ´¦Àí !!!!  ==== ",__FILE__,__LINE__);
                    vtcp_log("[%s][%d] ==== Õâ±Ê½»Ò×Ó¦¸ÃÔÚ iCheckHv_zfTable()ÖÐ´¦Àí£¬ÕâÀï²»¹ÜÁË !!!!  ==== ",__FILE__,__LINE__);
            
                    /* hv_zf.checkflag[0] = 'A';    DHCCºÍCCPCÏàÍ¬ */
                    /* hv_chkagemr.chk_flg[0]= 'A';  CCPC_SAME_BANK DHCCºÍCCPCÏàÍ¬ */
                    /* *iHandled = 0;  ²»ÐèÒª×ßÕÊ */
                         sprintf(acErrMsg,"===== ÕâÀï³ö´íÁË Êý¾Ý¿âÖÐ¼ÇÂ¼´íÎó !! ====[%s]",hv_zf.hv_sts);
                         WRITEMSG
                           return -1;
              }
              else
              {
                   vtcp_log("[%s][%d] ÕÒµ½Õâ±Ê½»Ò×µÄ¼ÇÂ¼ ÐÞ¸Ä±êÖ¾ !",__FILE__,__LINE__);       
                     hv_zf.hv_sts[0] =  '9';  /* Ä¬ÈÏÎª À´ÕË¼ÇÕËÇåËã£¬¹ÒÕÊ¼ì²éÊ±ÔÙ¸ù¾ÝÇé¿öÖØÉè */
                     hv_zf.beg_sts[0] = hv_zf.hv_sts[0]; 
                   hv_zf.checkflag[0] = 'D';     /*NOT_FOUND_BANK  CCPCÓÐ£¬DHCCÃ»ÓÐ*/

                   vtcp_log("[%s][%d] hv_chkagemr.pay_ac_no==[%s]  hv_chkagemr.cash_ac_no==[%s]  ",__FILE__,__LINE__, hv_chkagemr.pay_ac_no,hv_chkagemr.cash_ac_no); 
                   vtcp_log("[%s][%d] hv_zf.pay_ac_no==[%s]  hv_zf.cash_ac_no==[%s]  ",__FILE__,__LINE__, hv_zf.pay_ac_no,hv_zf.cash_ac_no); 

                   vtcp_log("[%s][%d] iLw_ind=========[%d]  ",__FILE__,__LINE__, iLw_ind);
                   if(iLw_ind == 2)  /* À´ÕÊ */
                   {                    
                     hv_zf.lw_ind[0] = '2'; 
                   }
                   else if(iLw_ind == 1)   /* ÍùÕÊ */
                   {
                     hv_zf.lw_ind[0] = '1';     
                   }else
                   {
                     sprintf(acErrMsg,"À´Íù±êÊ¶´í[%d]",iLw_ind);
                     WRITEMSG
                        return ret;
                   }            
                   vtcp_log("[%s][%d] hv_zf.lw_ind=[%s]  ",__FILE__,__LINE__,hv_zf.lw_ind);

                   /* ÆäËû×Ö¶ÎÄØ? Ó¦¸Ã°´ÕÕCCPCµÄ£¬ÎÒÃÇÕâ±ßÔÚHv_zfhtrÖÐ»¹ÓÐÒ»¸ö±¸·Ý ?? */
                   iCopyHv_zfFromHv_chkagemr(&hv_zf,&hv_chkagemr);
                   vtcp_log("%s,%d brno=%s",__FILE__,__LINE__,hv_zf.br_no);
                   vtcp_log("[%s][%d] hv_chkagemr.pay_ac_no==[%s]  hv_chkagemr.cash_ac_no==[%s]  ",__FILE__,__LINE__, hv_chkagemr.pay_ac_no,hv_chkagemr.cash_ac_no); 
                   vtcp_log("[%s][%d] hv_zf.pay_ac_no==[%s]  hv_zf.cash_ac_no==[%s]  ",__FILE__,__LINE__, hv_zf.pay_ac_no,hv_zf.cash_ac_no); 

                   /* ³åÕÊºó ÖÃÉÏ±êÖ¾£¬·ñÔòÏÂÒ»Ñ­»·Ê±»¹´¦Àí*/
                   hv_chkagemr.chk_flg[0] = 'D';     /*NOT_FOUND_BANK  CCPCÓÐ£¬DHCCÃ»ÓÐ*/
                   
                   /* Èç¹ûÊÇ´åÕòÒøÐÐ£¬Ôò²»ÐèÒª½øÐÐÕËÎñ´¦Àí */
                   /* begin add by LiuHuafeng 2009-5-14 17:09:11 */
                   /* Èç¹ûÊÇ´åÕòÒøÐÐ£¬Ôò²»ÐèÒª½øÐÐÕËÎñ´¦Àí */
                   if(!strncmp(hv_chkagemr.br_no,HV_CUNZHEN_BR_NO,BRNO_LEN-2))
                   {
                       iCzBankFlag = 1;
                       vtcp_log("%s,%d ´åÕòÒøÐÐÕËÎñÉÙ£¬Ö»×öµÇ¼Ç²¿´¦Àí",__FILE__,__LINE__);
                   }
                   else
                   {
                       iCzBankFlag = 0;
                   }
                   *iHandled = 1;     /* ÐèÒª×ßÕÊ */            
              }          
        } 

        ret = Hv_chkagemr_Upd_Upd(hv_chkagemr,g_pub_tx.reply);
        if(ret)
        {
             vtcp_log("[%s][%d] Hv_chkagemr_Upd_Upd() ³ö´í!",__FILE__,__LINE__);
                 sprintf(acErrMsg,"Êý¾Ý¿â²Ù×÷Òì³£,ÇëÓë¿Æ¼¼¿ÆÁªÏµ![%d]",ret);
                 WRITEMSG
                   return ret;
        }             
        vtcp_log("[%s][%d]  ÐÞ¸Ä Hv_chkagemr.chk_flg ±êÖ¾³É¹¦!  ",__FILE__,__LINE__);      

        if(*iHandled == 0)   /* ²»ÐèÒª×ßÕÊ£¬¼ÌÐøÑ­»·×ßÕÊ */
        {
                Hv_zf_Clo_Upd(); 
              vtcp_log("[%s][%d] ²»ÐèÒª×ßÕÊ£¬¼ÌÐøÑ­»·×ßÕÊ !!!  ",__FILE__,__LINE__);
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
            {  /* ÆÕÍ¨Àà: (CMT 100 101 102 103 105 108 122)*/
                     vtcp_log("[%s][%d] ÆÕÍ¨Àà Õý½ð¶î×ßÕÊ ",__FILE__,__LINE__);
                vtcp_log("[%s][%d] hv_chkagemr.pay_ac_no==[%s]  hv_chkagemr.cash_ac_no==[%s]  ",__FILE__,__LINE__, hv_chkagemr.pay_ac_no,hv_chkagemr.cash_ac_no); 
                vtcp_log("[%s][%d] hv_zf.pay_ac_no==[%s]  hv_zf.cash_ac_no==[%s]  ",__FILE__,__LINE__, hv_zf.pay_ac_no,hv_zf.cash_ac_no); 
            
                  ret = iSet8583_For_CommonCMT(&hv_zf,1);  /*** 1=Õý½ð¶î×ßÕÊ ***/
                  if(ret)
                  {
                    vtcp_log("[%s][%d]iSet8583_For_CommonCMT() ³ö´í!",__FILE__,__LINE__);
                  sprintf(acErrMsg,"ÉèÖÃ8583ÓòÊ±³ö´í![%d]",ret);
                  WRITEMSG
                    return ret;
                  }          
            
            }            
            else  if(iCmtno==121 || iCmtno==123 || iCmtno==124 )
            {  /*  ÌØÊâÀà (CMT 121 123 124)  */
                     vtcp_log("[%s][%d] ÌØÊâÀà Õý½ð¶î×ßÕÊ  iCmtno=[%d] ",__FILE__,__LINE__,iCmtno);
                vtcp_log("[%s][%d] hv_chkagemr.pay_ac_no==[%s]  hv_chkagemr.cash_ac_no==[%s]  ",__FILE__,__LINE__, hv_chkagemr.pay_ac_no,hv_chkagemr.cash_ac_no); 
                vtcp_log("[%s][%d] hv_zf.pay_ac_no==[%s]  hv_zf.cash_ac_no==[%s]  ",__FILE__,__LINE__, hv_zf.pay_ac_no,hv_zf.cash_ac_no); 
            
                  ret = iSet8583_For_SpecialCMT(&hv_zf, hv_chkagemr.p_num, hv_chkagemr.p_date,hv_chkagemr.jf_amt,1);  /*** 1=Õý½ð¶î×ßÕÊ ***/
                if(ret)
                  {
                    vtcp_log("[%s][%d]iSet8583_For_SpecialCMT() ³ö´í!",__FILE__,__LINE__);
                  sprintf(acErrMsg,"ÉèÖÃ8583ÓòÊ±³ö´í![%d]",ret);
                  WRITEMSG
                    return ret;
                  }  
            }     
            else
            {
               sprintf(acErrMsg," ¸Ã±¨ÎÄ²»ÔÚÕâÀï´¦Àí!! ");
               WRITEMSG
                 return -1;                
            }
            memset(cKinbr,0,sizeof(cKinbr));
            get_zd_data("0030",cKinbr);
            MEMCPY_DEBUG(hv_zf.br_no,cKinbr,sizeof(hv_zf.br_no)-1);
        /*begin by LiuHuafeng 2009-5-16 19:38:06 for ´åÕòÒøÐÐ */
        }
        /* end by LiuHuafeng 2009-5-16 19:38:23 */
        vtcp_log("[%s][%d]  hv_zf.br_no=====[%s]  ",__FILE__,__LINE__,hv_zf.br_no); 
        vtcp_log("[%s][%d]  hv_zf.br_no=====[%s]  ",__FILE__,__LINE__,hv_zf.br_no); 
        vtcp_log("[%s][%d] hv_chkagemr.pay_ac_no==[%s]  hv_chkagemr.cash_ac_no==[%s]  ",__FILE__,__LINE__, hv_chkagemr.pay_ac_no,hv_chkagemr.cash_ac_no); 
        vtcp_log("[%s][%d] hv_zf.pay_ac_no==[%s]  hv_zf.cash_ac_no==[%s]  ",__FILE__,__LINE__, hv_zf.pay_ac_no,hv_zf.cash_ac_no); 

        /*  ÖÃÍùÕÊµÄ×´Ì¬  */
        if(hv_zf.lw_ind[0] == '1')  /* ÍùÕÊ¶ÔÕÊÊ±ÓÐ¿ÉÄÜ¹ÒÕÊ */
        {
/*          if( hv_zf.hv_sts[0] != RECEIVE_CUSTNAME_ERR && hv_zf.hv_sts[0] != RECEIVE_DATA_ERR )
            {
                    hv_zf.hv_sts[0] = 'L'; 
            }
*/
            hv_zf.hv_sts[0] = 'L';        /* ÖÃÎª ºË¶Ô²»·û */
        }                                

        if( iInsertFlag )   /* Ã»ÓÐÕÒµ½ÏàÓ¦¼ÇÂ¼ ²åÈ */
        {
              /* ==== °´ÕÕ hv_chkagemr ²åÈë hv_zf =======*/
              vtcp_log("[%s][%d] Ã»ÓÐÕâ±Ê½»Ò×µÄ¼ÇÂ¼£¬°´ÕÕccpc²åÈëhv_zf!",__FILE__,__LINE__);
            vtcp_log("[%s][%d]  hv_zf.hv_sts=====[%s]  ",__FILE__,__LINE__,hv_zf.hv_sts);                

            vtcp_log("[%s][%d] hv_chkagemr.pay_ac_no==[%s]  hv_chkagemr.cash_ac_no==[%s]  ",__FILE__,__LINE__, hv_chkagemr.pay_ac_no,hv_chkagemr.cash_ac_no); 
            vtcp_log("[%s][%d] hv_zf.pay_ac_no==[%s]  hv_zf.cash_ac_no==[%s]  ",__FILE__,__LINE__, hv_zf.pay_ac_no,hv_zf.cash_ac_no); 

              if( iInsHv_zfByHv_chkagemr(&hv_zf,&hv_chkagemr ))
            {
              sprintf(acErrMsg,"iInsHv_zfByHv_chkagemr() ³ö´í[%d]",ret);
              WRITEMSG
             return -1;
            }
            vtcp_log("[%s][%d]  ²åÈë³É¹¦!  ",__FILE__,__LINE__);  
        }
        else /* ¸üÐÂ */
        {
              vtcp_log("[%s][%d]  hv_zf.hv_sts=====[%s]  ",__FILE__,__LINE__,hv_zf.hv_sts);  
              
            vtcp_log("[%s][%d] hv_chkagemr.pay_ac_no==[%s]  hv_chkagemr.cash_ac_no==[%s]  ",__FILE__,__LINE__, hv_chkagemr.pay_ac_no,hv_chkagemr.cash_ac_no); 
            vtcp_log("[%s][%d] hv_zf.pay_ac_no==[%s]  hv_zf.cash_ac_no==[%s]  ",__FILE__,__LINE__, hv_zf.pay_ac_no,hv_zf.cash_ac_no); 

            ret = Hv_zf_Upd_Upd(hv_zf,g_pub_tx.reply);
            if(ret)
            {
                   vtcp_log("[%s][%d] Hv_zf_Upd_Upd() ³ö´í!",__FILE__,__LINE__);
                        sprintf(acErrMsg,"Êý¾Ý¿â²Ù×÷Òì³£,ÇëÓë¿Æ¼¼¿ÆÁªÏµ![%d]",ret);
                        WRITEMSG
                          return -1;
            }             
            vtcp_log("[%s][%d]  ÐÞ¸Ä±êÖ¾³É¹¦!  ",__FILE__,__LINE__);             
        }    

        Hv_zf_Clo_Upd(); 
        *iHandled = 1;   /* ´¦ÀíÁËÒ»Ìõ¼ÇÂ¼  !! */

        /* »ãÆ±ÀàDHCCÉÙÕÊ ÐèÒª¸üÐÂ»ãÆ±µÄ×´Ì¬ (CMT 121 122 123 124) */
        if(iCmtno==121 || iCmtno==122 || iCmtno==123 || iCmtno==124 )
        {  
              vtcp_log("[%s,%d] »ãÆ±ÀàDHCCÉÙÕÊ ÐèÒª¸üÐÂ»ãÆ±µÄ×´Ì¬ !",__FILE__,__LINE__);    
              
              /* hv_chkagemr.other_amt Êµ¼Ê½áËã½ð¶î  hv_chkagemr.tx_amt Ê£Óà½ð¶î */
              if(hv_zf.lw_ind[0] == '1')   /* ÍùÕÊ */
              {
                    ret = iHv_SZ_ChangeHPStat(hv_chkagemr.or_br_no,hv_chkagemr.p_num, hv_chkagemr.other_amt, hv_chkagemr.tx_amt, iCmtno);
                   if(ret)
                   {
                             vtcp_log("%s,%d ¸üÐÂ»ãÆ±ÐÅÏ¢±í´íÎó ret=[%d]",__FILE__,__LINE__,ret);
                             vtcp_log(acErrMsg,"¸üÐÂ»ãÆ±ÐÅÏ¢±í´íÎó");
                             WRITEMSG
                             return ret;
                   }                      
              }
              else  /* À´ÕÊ */
              {
                    ret = iHv_SZ_ChangeHPStat(hv_chkagemr.acbrno,hv_chkagemr.p_num, hv_chkagemr.other_amt, hv_chkagemr.tx_amt, iCmtno);
                   if(ret)
                   {
                             vtcp_log("%s,%d ¸üÐÂ»ãÆ±ÐÅÏ¢±í´íÎó ret=[%d]",__FILE__,__LINE__,ret);
                             vtcp_log(acErrMsg,"¸üÐÂ»ãÆ±ÐÅÏ¢±í´íÎó");
                             WRITEMSG
                             return ret;
                   }     
              }
               vtcp_log("[%s,%d] ¸üÐÂ»ãÆ±×´Ì¬³É¹¦!  ",__FILE__,__LINE__); 
        }      

        /***********   µÇ¼Ç²î´í±í     ***************/                           
        ret = iInsertHv_chkerror(&hv_zf, &hv_chkagemr);
        if(ret)
        {
            vtcp_log("[%s][%d]Êý¾Ý¿â²Ù×÷Òì³£,ÇëÓë¿Æ¼¼¿ÆÁªÏµ!",__FILE__,__LINE__);
            sprintf(acErrMsg,"Êý¾Ý¿â²Ù×÷Òì³£,ÇëÓë¿Æ¼¼¿ÆÁªÏµ![%d]",ret);
            WRITEMSG
            return ret;
        }
        vtcp_log("[%s][%d]  µÇ¼Ç²î´í±í³É¹¦ ! ",__FILE__,__LINE__);
        if(iCzBankFlag == 1 &&  *iHandled == 1)
        {
            *iHandled = 0;
            vtcp_log("%s,%d ÊÇ´åÕòÒøÐÐÉÙÕËÇé¿ö£¬Ö»×öµÇ¼Ç£¬²»½øÐÐÕËÎñ´¦Àí£¬¼ÌÐøÏÂÒ»¸öÑ­»·",__FILE__,__LINE__);
            continue;
        }
        break;     
   } 

   Hv_chkagemr_Clo_Upd();
    vtcp_log("[%s][%d] ***** Hv_chkagemr_Clo_Upd() ³É¹¦ !  ***** ",__FILE__,__LINE__);
    vtcp_log("[%s][%d] ***** Hv_chkagemr_Clo_Upd() ³É¹¦ !  ***** ",__FILE__,__LINE__);      
   
      return 0;    
}


/********************************************************
* º¯ Êý Ãû:  iSet8583_For_SpecialCMT()
* ¹¦ÄÜÃèÊö£º ÎªÌØÊâÀà×ßÕÊÉèÖÃ8583Óò (CMT121 123 124) 
   DHCCÓÐ CCPCÃ»ÓÐ  ¸º½ð¶î×ßÕÊ     
   CCPCÓÐ DHCCÃ»ÓÐ  Õý½ð¶î×ßÕÊ                   
  
* ²ÎÊý£º ZFflg = 1   Õý½»Ò××ßÕÊ
         ZFflg = -1  ¸º½»Ò××ßÕÊ    
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

        if((iCmtno == 121) && (hv_zf->lw_ind[0] == '1') )   /* ÍùÕÊ »ãÆ±×Ê½ðÒÆ´æ*/
        {
            /*   ½è£º 2440402 ÕÊ»§           |0400 Ç©·¢½ð¶î   */
            /*   ´û£º 41102                  |0390            */
            /*   ½è£º 41102                  |0390            */
            /*   ´û£º 110 ÕÊ»§               |0400                */
            vtcp_log("[%s][%d] ====== ¸Ã±Ê½»Ò×ÊÇÍùÕÊ£¬ÒÔÏÂ¿ªÊ¼×ßÕÊ !! iCmtno=[%d] ====== ",__FILE__,__LINE__,iCmtno);

            GetBr_noByOr_br_no(hv_zf->pay_opn_br_no, cOpenbr);  /* ¸¶¿îÈË¿ª»§ÐÐ  */
            vtcp_log("%s,%d  cOpenbr======[%s]",__FILE__,__LINE__,cOpenbr);  
                
               /*************    ¹ÒÕÊ¼ì²é   ****************/
              /** ÅÐ¶Ï½ÓÊÕ»ú¹¹ÊÇ·ñ´æÔÚ */  
                  memset(cKinbr, 0 , sizeof(cKinbr));
              MEMCPY_DEBUG(cKinbr,hv_zf->br_no,BRNO_LEN); 
            memset(cStat, 0, sizeof(cStat));        
            memset(cTmpStat, 0, sizeof(cTmpStat));                
              set_zd_data("0980","9");     /*  */
              
               ret = iHvHang_Check_Brno(cOpenbr, cKinbr);  
            if(ret)
            {
               sprintf(acErrMsg,"Êý¾Ý¿â²Ù×÷Ê§°Ü[%d]",g_reply_int);
               WRITEMSG
               return -1;
            }               
            get_zd_data(DC_TX_BR_NO,cKinbr);   
            vtcp_log("%s,%d µÃµ½µÄ½»Ò×»ú¹¹ÊÇ[%s]",__FILE__,__LINE__,cKinbr);
            iGetReceTel(cKinbr);
            get_zd_data("0980",cStat);
              vtcp_log("%s,%d µÃµ½cStat====[%s]",__FILE__,__LINE__,cStat);
              if(ZFflg == 1) 
              {
                    hv_zf->hv_sts[0] = cStat[0];  /* ¸º½ð¶î×ßÕÊÎª³åÕÊ£¬ÔÚÕâÀï²»¸Ä±ä×´Ì¬ */
                    hv_zf->beg_sts[0] = hv_zf->hv_sts[0]; 
              }
            
                /* ´û£º 41101                   */
                /* ½è£º 41101                   */                                      
            set_zd_data("0640",HVWZGD); /* ´ó¶îÍùÕÊ¹ý¶É¿ÆÄ¿ */                     
            set_zd_data("0210","01");                                             
            set_zd_data("0510",hv_zf->cmtno);  
                        
            /* ÉèÖÃÀ´ÕÊµÄ2440402£¬2440401ÕÊºÅºÍÓà¶î£¬Ò»¶¨Îª0 ·ñÔò»á×ßÕÊ*/ 
            memset(cAc_no, 0 , sizeof(cAc_no));
            ret=pub_base_GetParm_Str(cKinbr,PARM_HPQF,cAc_no);
            if(ret)
            {
                vtcp_log("%s,%d Ã»ÓÐÕÒµ½½»Ò×»ú¹¹Ö¸¶¨µÄ»ãÆ±Ç©·¢»§",__FILE__,__LINE__);
                return -1;
            }
            vtcp_log("[%s][%d] PARM_HPQF  cAc_no===[%s]  ",__FILE__,__LINE__,cAc_no);  
            set_zd_data("0320",cAc_no);
            memset(cAc_no, 0 , sizeof(cAc_no));
            ret=pub_base_GetParm_Str(cKinbr,PARM_HPYC,cAc_no);
            if(ret)
            {
                vtcp_log("%s,%d Ã»ÓÐÕÒµ½½»Ò×»ú¹¹Ö¸¶¨µÄ»ãÆ±ÒÆ´æ»§",__FILE__,__LINE__);
                return -1;
            }
            vtcp_log("[%s][%d]  PARM_HPYC  cAc_no===[%s]  ",__FILE__,__LINE__,cAc_no);  
            set_zd_data("0330",cAc_no);
               set_zd_double("0420",0.0 );  /* ÕâÀï²»×ß2440401ºÍ2440402£¬2440402Í¨¹ý0300×ß */   
      
            /* ½è£º 2440402 ÕÊ»§            */                     
            set_zd_data("0660","1");  /* ÉèÖÃ¿Í»§ÕÊ·½Ïò½è´û±êÖ¾ 1-½è£» **/               
              set_zd_data("0680","0");  /* ÕÊ»§ÀàÐÍ */
               set_zd_data("0300", cAc_no );  /* 2440402 PARM_HPYC ÕÊ»§ */                   
                 
                /* ´û£º 110ÕÊ»§                    */     
            memset(cAc_no, 0 , sizeof(cAc_no));                                      
            ret=pub_base_GetParm_Str(cKinbr,PARM_HVQS,cAc_no);
            if(ret)
            {
                    vtcp_log("%s,%d ÏµÍ³ÅäÖÃ´íÎó£¬µÃµ½»ú¹¹%sµÄ110ÕÊºÅ´íÎó[%d]",__FILE__,__LINE__,ret);
                    return -1;
            }
            vtcp_log("[%s][%d]  PARM_HVQS cAc_no===[%s] ",__FILE__,__LINE__,cAc_no);                                         
            set_zd_data("1201", cAc_no);
            set_zd_data("0720","2");  /* ÉèÖÃ110 ·½Ïò½è´û±êÖ¾  2-´û */ 

            set_zd_double("0390",hv_zf->tx_amt * ZFflg);  
            set_zd_double("0400",hv_zf->tx_amt * ZFflg );             
           /* set_zd_data("0070",hv_zf->tel);  */ 
             
        } else if( (iCmtno == 123) && (hv_zf->lw_ind[0] == '2')) /*À´ÕÊ ¶àÓà×Ê½ð»®»Ø*/
        {
               /*   ½è£º 110 ÕÊ»§                 |0400 Î´ÓÃ½ð¶î    */
               /*   ´û£º 41102                    |0390             */
               /*   ½è£º 41102                    |0390             */
               /*   ´û£º ¿Í»§ / ¹ÒÕÊÕÊºÅ          |0400             */
               /*   ½è£º 2440401                  |0420 Ç©·¢½ð¶î    */
               /*   ´û£º 2440402                  |0420                  */
            vtcp_log("[%s][%d] ====== ¸Ã±Ê½»Ò×ÊÇÍùÕÊ£¬ÒÔÏÂ¿ªÊ¼×ßÕÊ !! iCmtno=[%d] ====== ",__FILE__,__LINE__,iCmtno);
            GetBr_noByOr_br_no(hv_zf->pay_opn_br_no, cOpenbr);  /* ¸¶¿îÈË¿ª»§ÐÐ  */
            vtcp_log("%s,%d  cOpenbr======[%s]",__FILE__,__LINE__,cOpenbr); 
               /*   ´û£º ¿Í»§ / ¹ÒÕÊÕÊºÅ          |0400             */
               
               /*************   ¹ÒÕÊ¼ì²é  *******************/    
               /*********** Ö÷ÒªÊÇ»ú¹¹¡¢ÕÊºÅ¼°Æä×´Ì¬µÄ¼ì²é   ******/
                  memset(cAc_no, 0 , sizeof(cAc_no));
                  memset(cKinbr, 0 , sizeof(cKinbr));
                  MEMCPY_DEBUG(cKinbr,hv_zf->br_no,BRNO_LEN);
            memset(cStat, 0, sizeof(cStat));        
            memset(cTmpStat, 0, sizeof(cTmpStat));                
              set_zd_data("0980","9");     /*  */

            /* 123 124 ÓÃ¸¶¿îÈËÕÊºÅ×ßÕÊ 
            ret = iHvHang_Check_Acno(hv_zf->cash_ac_no,hv_zf->cash_name,cKinbr);  ÉèÖÃºÃ 0030 */
            vtcp_log("[%s][%d]  hv_zf->pay_ac_no==[%s] ",__FILE__,__LINE__,hv_zf->pay_ac_no); 
            vtcp_log("[%s][%d]  hv_zf->pay_name==[%s] ",__FILE__,__LINE__,hv_zf->pay_name); 
      
            ret = iHvHang_Check_Acno(hv_zf->pay_ac_no,hv_zf->pay_name,cKinbr);  /* ÉèÖÃºÃ 0300 */
              if(ret)
              {
                     vtcp_log("%s,%d iHvHang_Check_Acno()  ³ö´í ",__FILE__,__LINE__);
                 sprintf(acErrMsg," iHvHang_Check_Acno()  ³ö´í !! ");
                 WRITEMSG               
                     return ret;
              }
              
            get_zd_data(DC_TX_BR_NO,cKinbr);   
            vtcp_log("%s,%d µÃµ½µÄ½»Ò×»ú¹¹ÊÇ[%s]",__FILE__,__LINE__,cKinbr);
            iGetReceTel(cKinbr);
            get_zd_data("0980",cStat);
              vtcp_log("%s,%d µÃµ½cStat====[%s]",__FILE__,__LINE__,cStat);
              if(ZFflg == 1) 
              {
                   hv_zf->hv_sts[0] = cStat[0];  /* ¸º½ð¶î×ßÕÊÎª³åÕÊ£¬ÔÚÕâÀï²»¸Ä±ä×´Ì¬ */
                   hv_zf->beg_sts[0] = hv_zf->hv_sts[0]; 
              }
              
                            
                /* ½è£º ¿Í»§ (¹ÒÕÊÕÊºÅ)         */                     
            set_zd_double("0390",hv_zf->tx_amt * ZFflg);  /*  Î´ÓÃ½ð¶î*/
            set_zd_double("0400",hv_zf->tx_amt * ZFflg);  /*  Î´ÓÃ½ð¶î*/
               vtcp_log("%s,%d µÃµ½µÄ 0420 Ç©·¢½ð¶î tx_amt ==== [%lf]",__FILE__,__LINE__,hv_zf->tx_amt*ZFflg);    
            
            set_zd_data("0660","2");  /* ÉèÖÃ¿Í»§ÕÊ·½Ïò½è´û±êÖ¾ 1-½è£»2-´û **/
            /* set_zd_data("0070",hv_zf->tel);  */     
                      
                /* ´û£º 41201                   */
                /* ½è£º 41201                   */                                      
            set_zd_data("0640",HVLZGD); /* ´ó¶îÀ´ÕÊ¹ý¶É¿ÆÄ¿ */                     
            set_zd_data("0210","01");                                             
            set_zd_data("0510",hv_zf->cmtno);  
                 
                /* ´û£º 110ÕÊ»§                    */                           
            memset(cAc_no, 0 , sizeof(cAc_no));                
            ret=pub_base_GetParm_Str(cKinbr,PARM_HVQS,cAc_no);
            if(ret)
            {
                    vtcp_log("%s,%d ÏµÍ³ÅäÖÃ´íÎó£¬µÃµ½»ú¹¹%sµÄ110ÕÊºÅ´íÎó[%d]",__FILE__,__LINE__,ret);
                    return -1;
            }
            vtcp_log("[%s][%d]  PARM_HVQS cAc_no===[%s] ",__FILE__,__LINE__,cAc_no);                                         
            set_zd_data("1201", cAc_no);            
            set_zd_data("0720","1");  /* ÉèÖÃ110 ·½Ïò½è´û±êÖ¾  1-½è */ 
                 
                                           
            /* ÉèÖÃÀ´ÕÊµÄ2440402£¬2440401ÕÊºÅºÍ½ð¶î  */ 
            memset(cAc_no, 0 , sizeof(cAc_no));
            ret=pub_base_GetParm_Str(cKinbr,PARM_HPQF,cAc_no);
            if(ret)
            {
                vtcp_log("%s,%d Ã»ÓÐÕÒµ½½»Ò×»ú¹¹Ö¸¶¨µÄ»ãÆ±Ç©·¢»§",__FILE__,__LINE__);
                return -1;
            }
            vtcp_log("[%s][%d]   PARM_HPQF  cAc_no===[%s]  ",__FILE__,__LINE__,cAc_no);  
            set_zd_data("0320",cAc_no);
            memset(cAc_no, 0 , sizeof(cAc_no));
            ret=pub_base_GetParm_Str(cKinbr,PARM_HPYC,cAc_no);
            if(ret)
            {
                vtcp_log("%s,%d Ã»ÓÐÕÒµ½½»Ò×»ú¹¹Ö¸¶¨µÄ»ãÆ±ÒÆ´æ»§",__FILE__,__LINE__);
                return -1;
            }
            vtcp_log("[%s][%d]  PARM_HPYC  cAc_no===[%s]  ",__FILE__,__LINE__,cAc_no);  
            set_zd_data("0330",cAc_no);
               set_zd_double("0420",dTotal_amt * ZFflg);  /*  Ç©·¢½ð¶î*/  
               vtcp_log("%s,%d µÃµ½µÄ 0420 Ç©·¢½ð¶î dTotal_amt ==== [%lf]",__FILE__,__LINE__,dTotal_amt*ZFflg);    
        } else if( (iCmtno == 124) && (hv_zf->lw_ind[0] == '2'))  /*À´ÕÊ »ãÆ±×Ê½ðÎ´ÓÃÍË»Ø*/
        {
             /*     ½è£º 110 ÕÊ»§                |0400 Ç©·¢½ð¶î  */
             /*     ´û£º 41102                   |0390           */
             /*     ½è£º 41102                   |0390           */
             /*   1 »ãÆ±ÈÕÆÚÓâÆÚ£º                               */
             /*     ´û£º 2440403 ÕÊ»§            |0400           */
             /*   2 »ãÆ±ÈÕÆÚÎ´ÓâÆÚ£º                             */
             /*     ´û£º 2440402                 |0420           */
             /*     ½è£º 2440401                 |0420           */
             /*     ´û£º ¿Í»§ ¹ÒÕÊÕÊºÅ           |0400           */           

            vtcp_log("[%s][%d] ====== ¸Ã±Ê½»Ò×ÊÇÀ´ÕÊ£¬ÒÔÏÂ¿ªÊ¼×ßÕÊ !! iCmtno=[%d] ====== ",__FILE__,__LINE__,iCmtno);
            GetBr_noByOr_br_no(hv_zf->pay_opn_br_no, cOpenbr);  /* ¸¶¿îÈË¿ª»§ÐÐ  */
            vtcp_log("%s,%d  cOpenbr======[%s]",__FILE__,__LINE__,cOpenbr); 
            
               /*************   ¹ÒÕÊ¼ì²é  *******************/    
               /*********** Ö÷ÒªÊÇ»ú¹¹¡¢ÕÊºÅ¼°Æä×´Ì¬µÄ¼ì²é   ******/
                  memset(cAc_no, 0 , sizeof(cAc_no));
                  memset(cKinbr, 0 , sizeof(cKinbr));
                  MEMCPY_DEBUG(cKinbr,hv_zf->br_no,BRNO_LEN);
                        
            if( iCheckYQ(p_num, p_date) ) /*   1 »ãÆ±ÈÕÆÚÓâÆÚ£º  */
            {/*     ´û£º 2440403 ÕÊ»§            |0400           */  
                
                memset(cStat, 0, sizeof(cStat));        
                memset(cTmpStat, 0, sizeof(cTmpStat));                
                  set_zd_data("0980","9");     /*  */
                  
                   ret = iHvHang_Check_Brno(cOpenbr,cKinbr);  
                if(ret)
                {
                   sprintf(acErrMsg,"Êý¾Ý¿â²Ù×÷Ê§°Ü[%d]",g_reply_int);
                   WRITEMSG
                   return -1;
                } 
                get_zd_data(DC_TX_BR_NO,cKinbr);   
                vtcp_log("%s,%d µÃµ½µÄ½»Ò×»ú¹¹ÊÇ[%s]",__FILE__,__LINE__,cKinbr);
                iGetReceTel(cKinbr);
                get_zd_data("0980",cStat);
                  vtcp_log("%s,%d µÃµ½cStat====[%s]",__FILE__,__LINE__,cStat);
                  if(ZFflg == 1)  /* Ö»´¦ÀíÕý½ð¶î  ¸º½ð¶î×ßÕÊÎª³åÕÊ£¬ÔÚÕâÀï²»¸Ä±ä×´Ì¬ */
                  {
                       hv_zf->hv_sts[0] = cStat[0]; 
                       hv_zf->beg_sts[0] = hv_zf->hv_sts[0]; 
                  }
                  
                 set_zd_data("0680","6");/* ÉèÖÃÕÊ»§ÀàÐÍ */
                  
                 memset(cAc_no, 0 , sizeof(cAc_no));
                 ret=pub_base_GetParm_Str(cKinbr,PARM_HPTH,cAc_no); /** 2440403»ú¹¹»ãÆ±ÓâÆÚÍË»ØÕÊºÅ */
                 if(ret)
                 {
                       vtcp_log("%s,%d Ã»ÓÐÕÒµ½½»Ò×»ú¹¹Ö¸¶¨µÄ»ãÆ±ÒÆ´æ»§",__FILE__,__LINE__);
                       return -1;
                 }

                 vtcp_log("[%s][%d]   PARM_HPTH cAc_no===[%s]  ",__FILE__,__LINE__,cAc_no);  
                 set_zd_data("0300",cAc_no);                
                    /* set_zd_double("0420", 0.0);   ²»×ß2440402£¬2440401  */ 

                 set_zd_data("0660","2");   /* ÉèÖÃ½è´û±êÖ¾ 2-´û **/                     
            }
            else   /*   2 »ãÆ±ÈÕÆÚÎ´ÓâÆÚ£º   */
            {
                 /*     ´û£º 2440402                 |0420           */
                 /*     ½è£º 2440401                 |0420           */             
                 /*     ´û£º ¿Í»§/¹ÒÕÊÕÊºÅ           |0400           */
                vtcp_log("[%s][%d] cKinbr===[%s] hv_zf->cash_ac_no===[%s] ",__FILE__,__LINE__,cKinbr,hv_zf->cash_ac_no);                                         
                vtcp_log("[%s][%d] cKinbr===[%s] hv_zf->cash_name ===[%s] ",__FILE__,__LINE__,hv_zf->cash_name);      

                vtcp_log("[%s][%d] cKinbr===[%s] hv_zf->pay_ac_no===[%s] ",__FILE__,__LINE__,cKinbr,hv_zf->pay_ac_no);                                         
                vtcp_log("[%s][%d] cKinbr===[%s] hv_zf->pay_name ===[%s] ",__FILE__,__LINE__,hv_zf->pay_name);      


                memset(cStat, 0, sizeof(cStat));        
                memset(cTmpStat, 0, sizeof(cTmpStat));                
                  set_zd_data("0980","9");     /*  */

                  /* 123 124 ÓÃ¸¶¿îÈËÕÊºÅ×ßÕÊ 
                ret = iHvHang_Check_Acno(hv_zf->cash_ac_no,hv_zf->cash_name,cKinbr);  ÉèÖÃºÃ 0030 */
                   ret = iHvHang_Check_Acno(hv_zf->pay_ac_no,hv_zf->pay_name,cKinbr);  /* ÉèÖÃºÃ 0030 */
                   if(ret)
                   {
                          vtcp_log("%s,%d iHvHang_Check_Acno()  ³ö´í ",__FILE__,__LINE__);
                         sprintf(acErrMsg," iHvHang_Check_Acno()  ³ö´í !! ");
                         WRITEMSG 
                          return ret;
                   }
                get_zd_data(DC_TX_BR_NO,cKinbr);   
                vtcp_log("%s,%d µÃµ½µÄ½»Ò×»ú¹¹ÊÇ[%s]",__FILE__,__LINE__,cKinbr);
                iGetReceTel(cKinbr);
                get_zd_data("0980",cStat);
                  vtcp_log("%s,%d µÃµ½cStat====[%s]",__FILE__,__LINE__,cStat);
                  if(ZFflg == 1) /* Ö»´¦ÀíÕý½ð¶î  ¸º½ð¶î×ßÕÊÎª³åÕÊ£¬ÔÚÕâÀï²»¸Ä±ä×´Ì¬ */
                  {
                       hv_zf->hv_sts[0] = cStat[0];   
                       hv_zf->beg_sts[0] = hv_zf->hv_sts[0]; 
                  }
                               
                    /* ´û£º ¿Í»§ (¹ÒÕÊÕÊºÅ)         */                     
                set_zd_double("0390",hv_zf->tx_amt * ZFflg);  /*  Î´ÓÃ½ð¶î*/
                set_zd_double("0400",hv_zf->tx_amt * ZFflg);  /*  Î´ÓÃ½ð¶î*/
                   set_zd_double("0420",hv_zf->tx_amt * ZFflg);  /* ×ß2440402£¬2440401  */
                set_zd_data("0660","2");   /* ÉèÖÃ½è´û±êÖ¾ 2-´û **/                        
            }
               set_zd_double("0420",hv_zf->tx_amt * ZFflg);  /* ²»¹ÜÊÇ·ñÓâÆÚ£¬¶¼×ß 2440401 2440402 */            
            set_zd_double("0390",hv_zf->tx_amt * ZFflg);  /*  Î´ÓÃ½ð¶î*/
            set_zd_double("0400",hv_zf->tx_amt * ZFflg);  /*  Î´ÓÃ½ð¶î*/ 
            
                /* ´û£º 41201                   */
                /* ½è£º 41201                   */                                      
            set_zd_data("0640",HVLZGD); /* ´ó¶îÀ´ÕÊ¹ý¶É¿ÆÄ¿ */                     
            set_zd_data("0210","01");                                             
            set_zd_data("0510",hv_zf->cmtno);  
                 
                /* ½è£º 110ÕÊ»§                    */                           
            memset(cAc_no, 0 , sizeof(cAc_no));
            /* ret=pub_base_GetParm_Str(hv_zf->br_no,PARM_HVQS,cAc_no);  2006-11-13 19:49 */
            ret=pub_base_GetParm_Str(cKinbr, PARM_HVQS,cAc_no);
            if(ret)
            {
                    vtcp_log("%s,%d ÏµÍ³ÅäÖÃ´íÎó£¬µÃµ½»ú¹¹%sµÄ110ÕÊºÅ´íÎó[%d]",__FILE__,__LINE__,ret);
                    return -1;
            }
            vtcp_log("[%s][%d]  PARM_HVQS  cAc_no===[%s]  ",__FILE__,__LINE__,cAc_no);              
            set_zd_data("1201",cAc_no);
            set_zd_data("0720","1");  /* ÉèÖÃ110 ·½Ïò½è´û±êÖ¾  1-½è */ 
                                          
            /* ÉèÖÃÀ´ÕÊµÄ2440402£¬2440401ÕÊºÅºÍÓà¶î£¬*/ 
            memset(cAc_no, 0 , sizeof(cAc_no));
            ret=pub_base_GetParm_Str(cKinbr, PARM_HPQF,cAc_no);
            if(ret)
            {
                vtcp_log("%s,%d Ã»ÓÐÕÒµ½½»Ò×»ú¹¹Ö¸¶¨µÄ»ãÆ±Ç©·¢»§",__FILE__,__LINE__);
                return -1;
            }
            vtcp_log("[%s][%d] PARM_HPQF  cAc_no===[%s]  ",__FILE__,__LINE__,cAc_no);  
            set_zd_data("0320",cAc_no);
            memset(cAc_no, 0 , sizeof(cAc_no));
            ret=pub_base_GetParm_Str(cKinbr, PARM_HPYC,cAc_no);
            if(ret)
            {
                vtcp_log("%s,%d Ã»ÓÐÕÒµ½½»Ò×»ú¹¹Ö¸¶¨µÄ»ãÆ±ÒÆ´æ»§",__FILE__,__LINE__);
                return -1;
            }
            vtcp_log("[%s][%d]  PARM_HPYC cAc_no===[%s]  ",__FILE__,__LINE__,cAc_no);  
            set_zd_data("0330",cAc_no);
        }
        else
        {
          sprintf(acErrMsg," ¸Ã±¨ÎÄ²»ÔÚÕâÀï´¦Àí !! ");
          WRITEMSG
          return -1;
        }
        
      return 0;
}


int iCheckYQ(char* p_num,long p_date)
{
    
      int iYqFlag=0;/* 0Î´ÓâÆÚ,1ÓâÆÚ */
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
          /* »ãÆ±²»´æÔÚµÄÇé¿ö°´ÕÕÓâÆÚ´¦Àí */
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
* º¯ Êý Ãû:  iSet8583_For_CommonCMT()
* ¹¦ÄÜÃèÊö£º ÎªÆÕÍ¨Àà×ßÕÊÉèÖÃ8583Óò (CMT 100 101 102 103 105 108 122) 
   DHCCÓÐ CCPCÃ»ÓÐ  ¸º½ð¶î×ßÕÊ     
   CCPCÓÐ DHCCÃ»ÓÐ  Õý½ð¶î×ßÕÊ                   
   À´ÕÊ£º ½è 110   ´û ¿Í»§
   ÍùÕÊ£º ½è ¿Í»§  ´û 110    
* ²ÎÊý£º ZFflg = 1  Õý½»Ò××ßÕÊ
         ZFflg = -1  ¸º½»Ò××ßÕÊ    
********************************************************/
int iSet8583_For_CommonCMT(struct hv_zf_c* hv_zf, int ZFflg)
{
    int ret;
      char cAcno110[20];
    char cStat[2];
    char cTmpStat[2]; 
    char cCJ_Acno[20];        /* ²ð½èÕÊºÅ */  
      
      memset(cAcno110,0,sizeof(cAcno110));
      memset(cCJ_Acno,0,sizeof(cCJ_Acno));

    vtcp_log("[%s][%d] ÆÕÍ¨Àà hv_zf->lw_ind=[%s] ",__FILE__,__LINE__,hv_zf->lw_ind);
       if(hv_zf->lw_ind[0] == '1')   /* ÍùÕÊ */
       {
            /* ½è£º ¿Í»§ (¹ÒÕÊÕÊºÅ)         */
            /* ´û£º 41101                   */
            /* ½è£º 41101                   */
            /* ´û£º 110ÕÊ»§                   */     

             /*************   ¹ÒÕÊ¼ì²é  *******************/    
             /*********** Ö÷ÒªÊÇ»ú¹¹¡¢ÕÊºÅ¼°Æä×´Ì¬µÄ¼ì²é   ******/
             char cKinbr[BRNO_LEN+1];
             char cOpenbr[BRNO_LEN+1];
                char cAc_no[20];
                memset(cAc_no, 0 , sizeof(cAc_no));
                memset(cKinbr, 0 , sizeof(cKinbr));
                memset(cOpenbr, 0 , sizeof(cOpenbr));
                
          memset(cStat, 0, sizeof(cStat));        
          memset(cTmpStat, 0, sizeof(cTmpStat));                
            set_zd_data("0980","9");     /*  */
          vtcp_log("[%s][%d] ====== ¸Ã±Ê½»Ò×ÊÇÍùÕÊ£¬ÒÔÏÂ¿ªÊ¼×ßÕÊ !! ====== ",__FILE__,__LINE__);
            
            MEMCPY_DEBUG(cKinbr, hv_zf->br_no, sizeof(cKinbr)-1);
            GetBr_noByOr_br_no(hv_zf->pay_opn_br_no, cOpenbr);  /* ¸¶¿îÈË¿ª»§ÐÐ  */
            
          vtcp_log("[%s][%d] ÆÕÍ¨Àà hv_zf->cmtno=[%s] ",__FILE__,__LINE__,hv_zf->cmtno);      
            if(memcmp(hv_zf->cmtno,"105",sizeof(hv_zf->cmtno)-1)==0)  /* Í¬Òµ²ð½èÖ§¸¶±¨ÎÄ */
            { 
                  ret=pub_base_GetParm_Str(cKinbr,PARM_HVCJ,cCJ_Acno); /* Èç¹ûÊÇ 105 Í¬Òµ²ð½èÖ§¸¶±¨ÎÄ£¬ÔÚÕâÀïÉèÖÃ²ð½èÕÊºÅ */ 
                  if(ret)
                  {
                      vtcp_log("%s,%d µÃµ½[%s]HVCJ²ÎÊý´íÎó£¬Ö±½ÓÉèÖÃ¿ÆÄ¿2620401",__FILE__,__LINE__,cKinbr);
                      MEMCPY_DEBUG(cCJ_Acno,"2620401",7);
                  }
               /* ½è£º ²ð½èÕÊºÅ    */    
                 ret = iHvHang_Check_AcnoNoName(cCJ_Acno, cKinbr); /* ²»¼ì²é»§Ãû */
              if(ret)
                 {
                        vtcp_log("%s,%d iHvHang_Check_AcnoNoName()  ³ö´í ret=[%d]",__FILE__,__LINE__,ret);
                       sprintf(acErrMsg," iHvHang_Check_AcnoNoName()  ³ö´í !! ");
                       WRITEMSG               
                        return -1;
                 }  
            }
            else /* ·ÇÍ¬Òµ²ð½èÖ§¸¶±¨ÎÄ */
            {
               /* ½è£º ¿Í»§ (¹ÒÕÊÕÊºÅ)         */
               
              /* ÏÖ½ð¿ÆÄ¿µÄÍùÕÊÒµÎñÒª¾Ü¾øµ½¹ÒÕË¿ÆÄ¿ÉÏ */
              if( (strlen(hv_zf->pay_ac_no)==5 && memcmp(hv_zf->pay_ac_no, "10101", 5 )==0) ||
                    (strlen(hv_zf->pay_ac_no)==7 && memcmp(hv_zf->pay_ac_no,"1010100",7 )==0 ) )
              {
                    ret = iHvHang_Check_Brno(cOpenbr, hv_zf->br_no);  /* ÉèÖÃºÃ»ú¹¹ */
                  if(ret)
                    {
                           vtcp_log("%s,%d iHvHang_Check_Brno()  ³ö´í ret=[%d]",__FILE__,__LINE__,ret);
                            sprintf(acErrMsg," iHvHang_Check_Brno()  ³ö´í !! ");
                            WRITEMSG               
                           return -1;
                    }
                  memset(cKinbr, 0 , sizeof(cKinbr));
                  get_zd_data(DC_TX_BR_NO,cKinbr);   
                  vtcp_log("%s,%d µÃµ½µÄ½»Ò×»ú¹¹ÊÇ[%s]",__FILE__,__LINE__,cKinbr);
                            
                  memset(cAc_no, 0 , sizeof(cAc_no));
                         /* ÏÖ½ð¿ÆÄ¿µÄÍùÕÊÒµÎñÒª¾Ü¾øµ½¹ÒÕË¿ÆÄ¿ÉÏ */
                        ret=pub_base_GetParm_Str(cKinbr,PARM_HVGZ,cAc_no);
                        if(ret)
                        {
                            vtcp_log("%s,%d ÏµÍ³ÅäÖÃ´íÎó£¬µÃµ½»ú¹¹%sµÄ¹ÒÕËÕÊºÅ´íÎó[%d]",__FILE__,__LINE__,ret);
                            strncpy(acErrMsg,"µÃµ½±¾»ú¹¹¹ÒÕËÕËºÅ´íÎó",50);
                            set_zd_data("0130",acErrMsg);
                            return ret;
                        }  
                        set_zd_data("0300",cAc_no);      /* ÉèÖÃºÃÕËºÅ */ 
                        set_zd_data("0980","5");         /* ¾Ü¾ø */           

              }else
              {
                    ret = iHvHang_Check_Acno(hv_zf->pay_ac_no, hv_zf->pay_name, cKinbr); /* ÉèÖÃºÃ ÕËºÅ »ú¹¹ */
                  if(ret)
                    {
                           vtcp_log("%s,%d iHvHang_Check_Acno()  ³ö´í ret=[%d]",__FILE__,__LINE__,ret);
                          sprintf(acErrMsg," iHvHang_Check_Acno()  ³ö´í !! ");
                          WRITEMSG               
                           return -1;
                    }  
              }    
            }

          get_zd_data(DC_TX_BR_NO,cKinbr);   
          vtcp_log("%s,%d µÃµ½µÄ½»Ò×»ú¹¹ÊÇ[%s]",__FILE__,__LINE__,cKinbr);
          iGetReceTel(cKinbr);
          get_zd_data("0980",cStat);
            vtcp_log("%s,%d µÃµ½cStat====[%s]",__FILE__,__LINE__,cStat);
            if(ZFflg == 1) 
            {
                 hv_zf->hv_sts[0] = cStat[0];  /* ¸º½ð¶î×ßÕÊÎª³åÕÊ£¬ÔÚÕâÀï²»¸Ä±ä×´Ì¬ */
                 hv_zf->beg_sts[0] = hv_zf->hv_sts[0]; 
            }

         /* set_zd_data("0070",hv_zf->tel);  */ 
                       
              /* ½è£º ¿Í»§ (¹ÒÕÊÕÊºÅ)         */                     
          set_zd_double("0390",hv_zf->tx_amt * ZFflg);  /* ¸º½ð¶î */
          set_zd_double("0400",hv_zf->tx_amt * ZFflg);  /* ¸º½ð¶î */
          set_zd_data("0660","1");  /* ÉèÖÃ¿Í»§ÕÊ·½Ïò½è´û±êÖ¾ 1-½è£»2-´û **/
              /* ´û£º 41101                   */
              /* ½è£º 41101                   */                                      
          set_zd_data("0640",HVWZGD); /* ´ó¶îÍùÕÊ¹ý¶É¿ÆÄ¿ */                     
          set_zd_data("0210","01");                                             
          set_zd_data("0510",hv_zf->cmtno);  
               
              /* ´û£º 110ÕÊ»§                    */                           
          vtcp_log("[%s][%d] hv_zf->br_no===[%s] hv_zf->pay_ac_no===[%s] ",__FILE__,__LINE__,hv_zf->br_no,hv_zf->pay_ac_no);                                         
          memset(cAc_no, 0 , sizeof(cAc_no));
          /*ret=pub_base_GetParm_Str(cKinbr,PARM_HVQS,hv_zf->pay_ac_no);  2006-11-10 18:09*/
          ret=pub_base_GetParm_Str(cKinbr,PARM_HVQS,cAc_no); 
          if(ret)
          {
                  vtcp_log("%s,%d ÏµÍ³ÅäÖÃ´íÎó£¬µÃµ½»ú¹¹%sµÄ110ÕÊºÅ´íÎó[%d]",__FILE__,__LINE__,ret);
                  return -1;
          }
          set_zd_data("1201",cAc_no);
          set_zd_data("0720","2");  /* ÉèÖÃ110 ·½Ïò½è´û±êÖ¾  2-´û */ 
               
          /* J016 J017²»×ßÕÊ  ½ð¶îÖÃ 0           */                                                      
          /* ÉèÖÃÀ´ÕÊµÄ2440402£¬2440401ÕÊºÅºÍÓà¶î£¬Ò»¶¨Îª0 ·ñÔò»á×ßÕÊ */
          memset(cAc_no, 0 , sizeof(cAc_no));
          vtcp_log("%s,%d µÃµ½µÄ½»Ò×»ú¹¹ÊÇ[%s]",__FILE__,__LINE__,cKinbr);          
          ret=pub_base_GetParm_Str(cKinbr,PARM_HPQF,cAc_no);
          if(ret)
          {
              vtcp_log("%s,%d Ã»ÓÐÕÒµ½½»Ò×»ú¹¹Ö¸¶¨µÄ»ãÆ±Ç©·¢»§",__FILE__,__LINE__);
              return -1;
          }
           vtcp_log("[%s][%d]   cAc_no===[%s]  ",__FILE__,__LINE__,cAc_no);  
          set_zd_data("0320",cAc_no);
          memset(cAc_no, 0 , sizeof(cAc_no));
          ret=pub_base_GetParm_Str(cKinbr,PARM_HPYC,cAc_no);
          if(ret)
          {
              vtcp_log("%s,%d Ã»ÓÐÕÒµ½½»Ò×»ú¹¹Ö¸¶¨µÄ»ãÆ±ÒÆ´æ»§",__FILE__,__LINE__);
              return -1;
          }
          vtcp_log("[%s][%d]   cAc_no===[%s]  ",__FILE__,__LINE__,cAc_no);  
          set_zd_data("0330",cAc_no);
             set_zd_double("0420",0.0);                    
                             
       }
       else if(hv_zf->lw_ind[0] == '2') /*À´ÕÊ*/
       {
          /* ½è£º 110 ÕÊ»§               */
          /* ´û£º 41102                  */
          /* ½è£º 41102                  */
          /* ´û£º ¿Í»§ ¹ÒÕÊÕÊºÅ          */   

        /*************   ¹ÒÕÊ¼ì²é  *******************/    
        /*********** Ö÷ÒªÊÇ»ú¹¹¡¢ÕÊºÅ¼°Æä×´Ì¬µÄ¼ì²é   ******/
          char cKinbr[BRNO_LEN+1];
              char cAc_no[20];
              memset(cAc_no, 0 , sizeof(cAc_no));
              memset(cKinbr, 0 , sizeof(cKinbr));
        MEMCPY_DEBUG(cKinbr,hv_zf->br_no,BRNO_LEN); 

        memset(cStat, 0, sizeof(cStat));        
        memset(cTmpStat, 0, sizeof(cTmpStat));                
           set_zd_data("0980","9");     /*  */
        vtcp_log("[%s][%d] ====== ¸Ã±Ê½»Ò×ÊÇÀ´ÕÊ£¬ÒÔÏÂ¿ªÊ¼ÉèÖÃ8583 !! ====== ",__FILE__,__LINE__);
        

        vtcp_log("[%s][%d] ÆÕÍ¨Àà hv_zf->cmtno=[%s] ",__FILE__,__LINE__,hv_zf->cmtno);      
          if(memcmp(hv_zf->cmtno,"105",sizeof(hv_zf->cmtno)-1)==0)  /* Í¬Òµ²ð½èÖ§¸¶±¨ÎÄ */
          { 
                ret=pub_base_GetParm_Str(cKinbr,PARM_HVCJ,cCJ_Acno); /* Èç¹ûÊÇ 105 Í¬Òµ²ð½èÖ§¸¶±¨ÎÄ£¬ÔÚÕâÀïÉèÖÃ²ð½èÕÊºÅ */ 
                if(ret)
                {
                    vtcp_log("%s,%d µÃµ½[%s]HVCJ²ÎÊý´íÎó£¬Ö±½ÓÉèÖÃ¿ÆÄ¿2620401",__FILE__,__LINE__,cKinbr);
                    MEMCPY_DEBUG(cCJ_Acno,"2620401",7);
                }
             /* ½è£º ²ð½èÕÊºÅ    */    
               ret = iHvHang_Check_AcnoNoName(cCJ_Acno, cKinbr); /* ²»¼ì²é»§Ãû ÉèÖÃºÃ 0030 */
            if(ret)
               {
                      vtcp_log("%s,%d iHvHang_Check_AcnoNoName()  ³ö´í ret=[%d]",__FILE__,__LINE__,ret);
                     sprintf(acErrMsg," iHvHang_Check_AcnoNoName()  ³ö´í !! ");
                     WRITEMSG               
                      return -1;
               }  
          }
          else /* ·ÇÍ¬Òµ²ð½èÖ§¸¶±¨ÎÄ */
          {
             /* ´û£º ¿Í»§ ¹ÒÕÊÕÊºÅ          */     
              ret = iHvHang_Check_Acno(hv_zf->cash_ac_no,hv_zf->cash_name,cKinbr); /* ÉèÖÃºÃ 0030 */
           if(ret)
              {
                     vtcp_log("%s,%d iHvHang_Check_Acno()  ³ö´í ",__FILE__,__LINE__);
                    sprintf(acErrMsg," iHvHang_Check_Acno()  ³ö´í !! ");
                    WRITEMSG               
                     return ret;
              }
           }
           
        get_zd_data(DC_TX_BR_NO,cKinbr);   
        vtcp_log("%s,%d µÃµ½µÄ½»Ò×»ú¹¹ÊÇ[%s]",__FILE__,__LINE__,cKinbr);
        iGetReceTel(cKinbr);
        get_zd_data("0980",cStat);
          vtcp_log("%s,%d µÃµ½cStat====[%s]",__FILE__,__LINE__,cStat);
          if(ZFflg == 1) 
          {
               hv_zf->hv_sts[0] = cStat[0];  /* ¸º½ð¶î×ßÕÊÎª³åÕÊ£¬ÔÚÕâÀï²»¸Ä±ä×´Ì¬ */
               hv_zf->beg_sts[0] = hv_zf->hv_sts[0]; 
          }

                     
        /* set_zd_data("0070",hv_zf->tel);  */ 
        set_zd_data("0660","2");  /* ÉèÖÃ¿Í»§ÕÊ ½è´û±êÖ¾ 1-½è£»2-´û **/        
                                     
         /* ½è£º 110 ÕÊ»§               */
        memset(cAcno110, 0 , sizeof(cAcno110)); 
        set_zd_double("0390",hv_zf->tx_amt * ZFflg);  /* ¸º½ð¶î */
        set_zd_double("0400",hv_zf->tx_amt * ZFflg);  /* ¸º½ð¶î */                    
           set_zd_data("0660","2");
           set_zd_data("0720","1");  /* ÉèÖÃ110  ½è´û·½Ïò±êÖ¾  1-½è */ 
           ret = pub_base_GetParm_Str(cKinbr,PARM_HVQS,cAcno110);
           if(ret)
           {
               vtcp_log("%s,%d Ã»ÓÐÕÒµ½½»Ò×»ú¹¹Ö¸¶¨µÄ110»§",__FILE__,__LINE__);
               return ret;
           }
           
           vtcp_log("%s,%d ÕÒµ½½»Ò×»ú¹¹Ö¸¶¨µÄ110»§ cAcno110=[%s]",__FILE__,__LINE__,cAcno110);                        
           set_zd_data("1201",cAcno110);
           set_zd_data("1204","01");
           set_zd_data("1205","2");
  
           /* ´û£º 41201                   */
           /* ½è£º 41201                   */                                      
        set_zd_data("0640",HVLZGD); /* ´ó¶îÀ´ÕÊ¹ý¶É¿ÆÄ¿ */                     
        set_zd_data("0210","01");                                             
        set_zd_data("0510",hv_zf->cmtno);  

        /* J016 J017²»×ßÕÊ  ½ð¶îÖÃ 0           */                                                      
        /* ÉèÖÃÀ´ÕÊµÄ2440402£¬2440401ÕÊºÅºÍÓà¶î£¬Ò»¶¨Îª0 ·ñÔò»á×ßÕÊ */
        memset(cAc_no, 0 , sizeof(cAc_no));
        ret=pub_base_GetParm_Str(cKinbr,PARM_HPQF,cAc_no);
        if(ret)
        {
            vtcp_log("%s,%d Ã»ÓÐÕÒµ½½»Ò×»ú¹¹Ö¸¶¨µÄ»ãÆ±Ç©·¢»§",__FILE__,__LINE__);
            return -1;
        }
         vtcp_log("[%s][%d]   cAc_no===[%s]  ",__FILE__,__LINE__,cAc_no);  
        set_zd_data("0320",cAc_no);
        memset(cAc_no, 0 , sizeof(cAc_no));
        ret=pub_base_GetParm_Str(cKinbr,PARM_HPYC,cAc_no);
        if(ret)
        {
            vtcp_log("%s,%d Ã»ÓÐÕÒµ½½»Ò×»ú¹¹Ö¸¶¨µÄ»ãÆ±ÒÆ´æ»§",__FILE__,__LINE__);
            return -1;
        }
        vtcp_log("[%s][%d]   cAc_no===[%s]  ",__FILE__,__LINE__,cAc_no);  
        set_zd_data("0330",cAc_no);
        set_zd_double("0420",0.0);  
                              
       }
       else
       {
       sprintf(acErrMsg," À´Íù±êÊ¶´íÎó!! ");
       WRITEMSG
       return -1;
       }

    return 0;    
}



/*  °´ÕÕ hv_chkagemr ²åÈë hv_zf  */
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
         vtcp_log("[%s][%d] Hv_zf_Ins() ³ö´í!",__FILE__,__LINE__);
         sprintf(acErrMsg,"Êý¾Ý¿â²Ù×÷Òì³£,ÇëÓë¿Æ¼¼¿ÆÁªÏµ![%d]",ret);
         WRITEMSG
            return ret;    
      }
    vtcp_log("[%s][%d] Hv_zf_Ins()  ²åÈë³É¹¦!",__FILE__,__LINE__);    
      /*    memset(&hv_zfhtr,0,sizeof(hv_zfhtr));
      iCopyHv_zfToHv_zfhtr(&hv_zf,&hv_zfhtr);    */
      ret = Hv_zfhtr_Ins(*(hv_zf), g_pub_tx.reply);
      if(ret)
      {
         vtcp_log("[%s][%d] Hv_zfhtr_Ins() ³ö´í!",__FILE__,__LINE__);
         sprintf(acErrMsg,"Êý¾Ý¿â²Ù×÷Òì³£,ÇëÓë¿Æ¼¼¿ÆÁªÏµ![%d]",ret);
         WRITEMSG
           return ret;    
       }        
     vtcp_log("[%s][%d]  Hv_zfhtr_Ins() ²åÈë³É¹¦! \n ",__FILE__,__LINE__);    
    return 0;
}

/*  °´ÕÕ hv_chkagemr ²åÈë hv_zf  */
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
        vtcp_log("[%s][%d] iGetHvOrderno() ³ö´í!",__FILE__,__LINE__);
          sprintf(acErrMsg,"Êý¾Ý¿â²Ù×÷Òì³£,ÇëÓë¿Æ¼¼¿ÆÁªÏµ![%d]",ret);
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
    /* hv_zf->hv_sts[0] = '9';   Ä¬ÈÏÎª À´ÕË¼ÇÕËÇåËã£¬¹ÒÕÊ¼ì²éÊ±ÔÙ¸ù¾ÝÇé¿öÖØÉè */
      /* hv_zf->beg_sts[0] = hv_zf->hv_sts[0];  Ä¬ÈÏÎª À´ÕË¼ÇÕËÇåËã£¬¹ÒÕÊ¼ì²éÊ±ÔÙ¸ù¾ÝÇé¿öÖØÉè */
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

      /* 108 ÍË»ãÖ§¸¶±¨ÎÄµÄÕÊºÅºÍÓÃ»§ÃûÌØÊâ´¦Àí */
      if( memcmp(hv_zf->cmtno,"108", 3) == 0)
      {
              /* À´ÍùÒµÎñ¶¼Ò»Ñù ½«'¸¶¿îÈËÏà¹ØÏî'Óë'ÊÕ¿îÈËÏà¹ØÏî  ' ·´¹ýÀ´  */
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
    if(hv_zf->lw_ind[0] == '2')  /* À´ÕÊ */
    {                    
      if( GetBr_noByOr_br_no(hv_zf->ac_br_no,hv_zf->br_no))
      {
            memset(hv_zf->br_no, 0, sizeof(hv_zf->br_no));
          /* MEMCPY_DEBUG(hv_zf->br_no,hv_chkagemr->br_no,sizeof(hv_zf->br_no)-1);     */          
      }
    }
    else if(hv_zf->lw_ind[0] == '1')   /* ÍùÕÊ */
    {
      if( GetBr_noByOr_br_no(hv_zf->or_br_no,hv_zf->br_no))
      {
            memset(hv_zf->br_no, 0, sizeof(hv_zf->br_no));
          /* MEMCPY_DEBUG(hv_zf->br_no,hv_chkagemr->br_no,sizeof(hv_zf->br_no)-1);       */         
      }          
    }else
    {
      sprintf(acErrMsg,"À´Íù±êÊ¶´í[%s]",hv_zf->lw_ind);
      WRITEMSG
        return ret;
    }
    vtcp_log("[%s][%d] hv_zf->lw_ind=[%s]  ",__FILE__,__LINE__,hv_zf->lw_ind);
   
    vtcp_log("[%s][%d] hv_zf->pay_name==[%.60s] ",__FILE__,__LINE__,hv_zf->pay_name); 
    memset(cTempstr,0,sizeof(cTempstr));
    MEMCPY_DEBUG(cTempstr, hv_zf->pay_name, sizeof(hv_zf->pay_name)-1);  
    dbc_zipspace(cTempstr);  /*Ñ¹ËõÈ«½Ç¿Õ¸ñ 2006-11-7 23:45  */
    MEMCPY_DEBUG(hv_zf->pay_name, cTempstr, sizeof(hv_zf->pay_name)-1); 
    vtcp_log("[%s][%d] hv_zf->pay_name==[%.60s] ",__FILE__,__LINE__,hv_zf->pay_name);

    vtcp_log("[%s][%d] hv_zf->cash_name==[%.60s] ",__FILE__,__LINE__,hv_zf->cash_name); 
    memset(cTempstr,0,sizeof(cTempstr));
    MEMCPY_DEBUG(cTempstr, hv_zf->cash_name, sizeof(hv_zf->cash_name)-1);  
    dbc_zipspace(cTempstr);  /*Ñ¹ËõÈ«½Ç¿Õ¸ñ 2006-11-7 23:45  */
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
      
      /*  ========= ´¦Àí»ãÆ±ÀàµÄ¸½¼ÓÓò ======== */      
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
                dbl2str(&hv_chkagemr->other_amt, 0, sizeof(hv_hp123.real_js_amt)-1,0, hv_hp123.real_js_amt); /*Êµ¼Ê½áËã½ð¶î*/
            MEMCPY_DEBUG(hv_hp123.cash_opn_br_no, hv_chkagemr->cashno, sizeof(hv_hp123.cash_opn_br_no));  /*×îºó³ÖÆ±ÈË¿ª»§ÐÐ*/
        vtcp_log("[%s][%d]  ========== ", __FILE__,__LINE__);
        vtcp_log("[%s][%d]  ========== ", __FILE__,__LINE__);
            MEMCPY_DEBUG(hv_hp123.last_ac_no, hv_chkagemr->cash_ac_no, sizeof(hv_hp123.last_ac_no));    /*×îºó³ÖÆ±ÈËÕÊºÅ*/              
            MEMCPY_DEBUG(hv_hp123.last_name, hv_chkagemr->cash_name, sizeof(hv_hp123.last_name));        /*×îºó³ÖÆ±ÈËÐÕÃû*/
        vtcp_log("[%s][%d]  ========== ", __FILE__,__LINE__);
        vtcp_log("[%s][%d]  ========== ", __FILE__,__LINE__);
            apitoa(hv_chkagemr->pay_date, sizeof(hv_hp123.return_date), hv_hp123.return_date);  /*ÌáÊ¾¸¶¿îÈÕÆÚ*/       
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
      
    vtcp_log("[%s][%d] iInsertHv_chkerror () ¿ªÊ¼ !!!",__FILE__,__LINE__);

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
         vtcp_log("[%s][%d] Hv_chkerror_Ins() ³ö´í!",__FILE__,__LINE__);
         sprintf(acErrMsg,"Êý¾Ý¿â²Ù×÷Òì³£,ÇëÓë¿Æ¼¼¿ÆÁªÏµ![%d]",ret);
         WRITEMSG
            return ret;    
      }
    vtcp_log("[%s][%d] Hv_chkerror_Ins()  ²åÈë³É¹¦!",__FILE__,__LINE__);    
      return 0;
}


/*  »ãÆ±Àà DHCC¶àÕÊ ÐèÒª¸üÐÂ»ãÆ±µÄ×´Ì¬   */
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
          /*µÃµ½¸½¼ÓÓò */
          ret=iHvGetAddit(cTx_date,cWt_date,hv_zf_wz->addid,"000",(char*)&hv_hp121,sizeof(hv_hp121));
          if(ret)
          {
              vtcp_log("%s,%d ¶ÁÈ¡¸½¼ÓÓò´íÎó %d",__FILE__,__LINE__,ret);
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
          /*µÃµ½¸½¼ÓÓò */
          ret=iHvGetAddit(cTx_date,cWt_date,hv_zf_wz->addid,"000",(char*)&hv_hp122,sizeof(hv_hp122));
          if(ret)
          {
              vtcp_log("%s,%d ¶ÁÈ¡¸½¼ÓÓò´íÎó %d",__FILE__,__LINE__,ret);
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
          /*µÃµ½¸½¼ÓÓò */
          ret=iHvGetAddit(cTx_date,cWt_date,hv_zf_wz->addid,"000",(char*)&hv_hp123,sizeof(hv_hp123));
          if(ret)
          {
              vtcp_log("%s,%d ¶ÁÈ¡¸½¼ÓÓò´íÎó %d",__FILE__,__LINE__,ret);
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
          /*µÃµ½¸½¼ÓÓò */
          ret=iHvGetAddit(cTx_date,cWt_date,hv_zf_wz->addid,"000",(char*)&hv_hp124,sizeof(hv_hp124));
          if(ret)
          {
              vtcp_log("%s,%d ¶ÁÈ¡¸½¼ÓÓò´íÎó %d",__FILE__,__LINE__,ret);
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
          vtcp_log("[%s,%d] iCmtno ´íÎó!!!  iCmtno===[%d]",__FILE__,__LINE__,iCmtno);
          WRITEMSG
          return -1;    
    }

      ret = Hv_poinfo_Dec_Upd(g_pub_tx.reply,"po_no='%s'  and  pay_br_no='%s'",hv_poinfo.po_no,hv_poinfo.pay_br_no);
      if(ret)
      {
          sprintf(acErrMsg,"ÉêÃ÷ÓÎ±ê´íÎó!ret=[%d]\n",ret);
          strcpy(g_pub_tx.reply,"P001");
          WRITEMSG
          return -1;;
      }
      memset(&hv_poinfo, 0 , sizeof(hv_poinfo));      
      ret = Hv_poinfo_Fet_Upd(&hv_poinfo,g_pub_tx.reply);
      if(ret&&ret!=100)
      {
          sprintf(acErrMsg,"È¡ÓÎ±ê´íÎó!ret=[%d]\n",ret);
          strcpy(g_pub_tx.reply,"P001");
          WRITEMSG
          return -1;
      }
      else if(ret==100)
      {
          sprintf(acErrMsg,"²éÑ¯µÄ¼ÇÂ¼²»´æÔÚ!ret=[%d]\n",ret);
          WRITEMSG
          strcpy(g_pub_tx.reply,"P001");
          /* return -1;   Remed by ChenMing   2006-11-5 23:43
          ÒòÎª»ú¹¹¿ÉÄÜÕÒ²»µ½£¬»òÕß²»¶Ô£¬ËùÒÔ£¬Èç¹ûÃ»ÕÒµ½»ãÆ±¼ÇÂ¼ÕâÀï²»±¨´í */
      }
      
      cLast_sts = hv_poinfo.po_sts[0];
      
    if( iCmtno==121 )
    {
        hv_poinfo.po_sts[0] = hv_poinfo.l_po_sts[0];   /* ÖÃ»ØÔ­×´Ì¬ */
        hv_poinfo.l_po_sts[0] = cLast_sts;
        
    }
    /* else if( iCmtno==122 )
    {
         if( hv_poinfo.po_sts[0] == '4')   »ãÆ±¶Ò¸¶
         {
                hv_poinfo.po_sts[0] = hv_poinfo.l_po_sts[0];  
         }
    }else if( iCmtno==123 )
    {
         if( hv_poinfo.po_sts[0] == '6')   ²¿·ÖÍË»Ø 
         {
                hv_poinfo.po_sts[0] = hv_poinfo.l_po_sts[0];  
         }
    }else if( iCmtno==124 )
    {
         if( hv_poinfo.po_sts[0] == '7')   Î´ÓÃÍË»Ø 
         {
                hv_poinfo.po_sts[0] = hv_poinfo.l_po_sts[0];  
         }
    }*/

      ret = Hv_poinfo_Upd_Upd(hv_poinfo,g_pub_tx.reply);
      if(ret)
      {
          sprintf(acErrMsg,"Ö´ÐÐº¯Êýhv_poinfo_upd_upd´íÎó!ret==[%d]\n",ret);
          WRITEMSG
          strcpy(g_pub_tx.reply,"D105");
          return (-1);
      }
      
      Hv_poinfo_Clo_Upd();
      return 0;
}


/* »ãÆ±Àà DHCCÉÙÕÊ  */
/* dCash_amt Êµ¼Ê½áËã½ð¶î  dRem_amt  Ê£Óà½ð¶î */
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
          sprintf(acErrMsg,"ÉêÃ÷ÓÎ±ê´íÎó!ret=[%d]\n",ret);
          strcpy(g_pub_tx.reply,"P001");
          WRITEMSG
          return -1;;
      }
      memset(&hv_poinfo, 0 , sizeof(hv_poinfo));      
      ret = Hv_poinfo_Fet_Upd(&hv_poinfo,g_pub_tx.reply);
      if(ret&&ret!=100)
      {
          sprintf(acErrMsg,"È¡ÓÎ±ê´íÎó!ret=[%d]\n",ret);
          strcpy(g_pub_tx.reply,"P001");
          WRITEMSG
          return -1;
      }
      else if(ret==100)
      {
          sprintf(acErrMsg,"²éÑ¯µÄ¼ÇÂ¼²»´æÔÚ!ret=[%d]\n",ret);
          WRITEMSG
          strcpy(g_pub_tx.reply,"P001");
          return 0;
          /* return -1;   Remed by ChenMing   2006-11-5 23:43
          ÒòÎª»ú¹¹¿ÉÄÜÕÒ²»µ½£¬»òÕß²»¶Ô£¬ËùÒÔ£¬Èç¹ûÃ»ÕÒµ½»ãÆ±¼ÇÂ¼ÕâÀï²»±¨´í */
      }
      
    vtcp_log("[%s][%d] hv_poinfo.po_sts========[%s] ",__FILE__,__LINE__,hv_poinfo.po_sts);
    vtcp_log("[%s][%d] hv_poinfo.l_po_sts======[%s] ",__FILE__,__LINE__,hv_poinfo.l_po_sts);
        
    cLast_sts = hv_poinfo.po_sts[0];  /* »ãÆ±µÄÉÏÒ»´Î×´Ì¬ */
    hv_poinfo.l_tx_date = g_pub_tx.tx_date;  
    hv_poinfo.td_cnt ++;   

    vtcp_log("[%s][%d] iCmtno========[%d] ",__FILE__,__LINE__,iCmtno); 

    /* ÉÙÕÊ£¬¸ù¾Ý²»Í¬Çé¿öÉèÖÃ»Ø»ãÆ±µÄ±êÖ¾ */
    if( iCmtno==121 )
    {
        /*
        if( hv_poinfo.po_sts[0] == HV_HP_SIGN )  
        {
           hv_poinfo.po_sts[0] =  HV_HP_MV_CENTER; ×Ê½ðÒÆ´æ¼ÇÕÊ 
           hv_poinfo.l_po_sts[0] = cLast_sts;
        }
        else
        {
            Hv_poinfo_Clo_Upd();  
            sprintf(acErrMsg," »ãÆ±¼ÇÂ¼µ±Ç°×´Ì¬²»¶Ô ! hv_poinfo.po_sts=[%s] ",hv_poinfo.po_sts);
            WRITEMSG
            return -1;
        } 
     */
    }else if( iCmtno==122 )
    {
        if(hv_poinfo.po_sts[0] == HV_HP_JQ) /* 725ÏÈÊÕµ½ ÒÑÈ«¶î½áÇå */
        {
            vtcp_log(" [%s][%d] ÒÑÈ«¶î½áÇå 725ÏÈÊÕµ½ ²»ÓÃ¸üÐÂÁË!  ",__FILE__,__LINE__); 
            Hv_poinfo_Clo_Upd();  
            return 0; 
        }
        else if( hv_poinfo.po_sts[0] == HV_HP_ASKCASH || 
                   hv_poinfo.po_sts[0] == HV_HP_UNLOSS ||
                   hv_poinfo.po_sts[0] == HV_HP_MV_CENTER ||
                   hv_poinfo.po_sts[0] == HV_HP_LOSS )  /* ÉêÇë¶Ò¸¶ */  
        {
            hv_poinfo.po_sts[0] = HV_HP_CASH;           /* »ãÆ±¶Ò¸¶ */
            hv_poinfo.l_po_sts[0] = cLast_sts;
        }
        else
        {
            Hv_poinfo_Clo_Upd();  
            sprintf(acErrMsg," »ãÆ±¼ÇÂ¼µ±Ç°×´Ì¬²»¶Ô ! hv_poinfo.po_sts=[%s] ",hv_poinfo.po_sts);
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
        {  123  124  ÀÏÁõËµ²»ÓÃ¼ì²é×´Ì¬ÁË  2006-11-22 17:52  */
           hv_poinfo.po_sts[0] =  HV_HP_PARTBACK;   /* ²¿·ÖÍË»Ø */
           hv_poinfo.l_po_sts[0] = cLast_sts;
           hv_poinfo.cash_amt = dCash_amt ;   /* Êµ¼Ê½áËã½ð¶î */           
           hv_poinfo.rem_amt = dRem_amt;   /* ÍË»Ø½ð¶î */ 
        /*}
        else
        {
            Hv_poinfo_Clo_Upd();  
            sprintf(acErrMsg," »ãÆ±¼ÇÂ¼µ±Ç°×´Ì¬²»¶Ô ! hv_poinfo.po_sts=[%s] ",hv_poinfo.po_sts);
            WRITEMSG
            return -1;
        } 123  124  ÀÏÁõËµ²»ÓÃ¼ì²é×´Ì¬ÁË  2006-11-22 17:52   */
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
            hv_poinfo.po_sts[0] = HV_HP_OVERDATE_UNTREAD;  /* ÓâÆÚÎ´ÓÃÍË»Ø */
            hv_poinfo.l_po_sts[0] = cLast_sts;
         }
         else
         {
            hv_poinfo.po_sts[0] = HV_HP_FULLBACK;  /* Î´ÓÃÍË»Ø */
            hv_poinfo.l_po_sts[0] = cLast_sts;
         }  
    }          
    else
    {
        sprintf(acErrMsg,"iCmtno== [%d] ²»ÔÚÕâÀï´¦Àí! ",iCmtno);
        WRITEMSG
        return -1;
    }     
 
    vtcp_log("[%s][%d] hv_poinfo.po_sts========[%s] ",__FILE__,__LINE__,hv_poinfo.po_sts);
    vtcp_log("[%s][%d] hv_poinfo.l_po_sts======[%s] ",__FILE__,__LINE__,hv_poinfo.l_po_sts);

      ret = Hv_poinfo_Upd_Upd(hv_poinfo,g_pub_tx.reply);
      if(ret)
      {
          sprintf(acErrMsg,"Ö´ÐÐº¯Êýhv_poinfo_upd_upd´íÎó!ret==[%d]\n",ret);
          WRITEMSG
          strcpy(g_pub_tx.reply,"D105");
          return (-1);
      }
      
      Hv_poinfo_Clo_Upd();
      return 0;
}
