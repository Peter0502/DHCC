/*************************************************
* �� �� ��:  spJ620.c  ��Ӧ��ϵͳ V95.pc
* ���������� ���֧����ϸ����
*             
* ��    ��:  ChenMing 
* ������ڣ� 2006��8��26 ��
*
* �޸ļ�¼�� 
* ��    ��:
* �� �� ��:
* �޸�����:
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
    
    if ( iChecked == 0 )  /* ֻ��Ҫ�ڵ�һ�ν���ʱִ�� spJ620_Check*/ 
    {  
          ret = spJ620_Check();
      if(ret == 999) /* ���δͨ�� ������!*/
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
          set_zd_long("0520",iChecked);       /* �Ѽ��!! */
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
    * �жϽ��׻����Ƿ����������� *
    *****************************/
    if( GetBr_noByOr_br_no(HV_QS_BR_NO, cQs_br_no))
    {
          sprintf(acErrMsg,"���� ֧��ϵͳ�к� ���� �����������! " );
      WRITEMSG
          return -1;
    }
    
    vtcp_log("[%s][%d]=======  g_pub_tx.tx_br_no = [%s] �������� cQs_br_no=[%s]",__FILE__,__LINE__,g_pub_tx.tx_br_no,cQs_br_no);    
    /***  �жϽ��׻����Ƿ�����������   Remed For Test ! ***/
    if(memcmp(g_pub_tx.tx_br_no,cQs_br_no,strlen(g_pub_tx.tx_br_no)-1))
    {
          sprintf(acErrMsg,"���������Ĳ������˽���!%s",g_pub_tx.tx_br_no);
      WRITEMSG
      iResult = 5;
      set_zd_long("0510",5);
          return 999;   
    } 
    
    /*****************************************************************
    *  �жϴ��״̬�� �Ƿ���Զ���                                   *
    *****************************************************************/
    cStat = hv_getstat();
    vtcp_log("[%s][%d]=======cStat = [%c]",__FILE__,__LINE__,cStat);
    /* cStat = HV_STAT_CHECKERR;  For Test */
    if(cStat != HV_STAT_CHECKERR)
    {
          sprintf(acErrMsg,"ϵͳ��ǰ״̬���Ǻ˶���ϸ��״̬! [%c]",cStat);
      WRITEMSG
      iResult = 6;
      set_zd_long("0510",6);
          return 999;   
    }
    vtcp_log("[%s][%d]=======  ִ�е�������  ���Զ��� ========",__FILE__,__LINE__);
    
    /*****************************************************************
    *  �ж��Ƿ���ڸ��˵�δȡ��״̬�����˵�δ���͵�����              *
    *****************************************************************/
    /*  Remed For Test ! ***/
    ret = iCheckHvForDateOver((char *)NULL,g_pub_tx.tx_date,&iErrcnt);
    if( ret )
    {
          sprintf(acErrMsg," ���Ҵ��֧�������! [%d]",ret);
      WRITEMSG
          return -1;
    }
    vtcp_log("[%s][%d]=======   iErrcnt=[%d]   ========",__FILE__,__LINE__,iErrcnt);
    if( iErrcnt != 0 )
    {
          sprintf(acErrMsg,"���ڸ��˵�δ���͵�����[%d]",iErrcnt);
      WRITEMSG
      iResult = 7;
      set_zd_long("0510",7);
          return 999;   
    }
    vtcp_log("[%s][%d] =======ִ�е�������,�����ڸ��˵�δ���͵����� ! =======",__FILE__,__LINE__);
    
    /*****************************************************************
    * �ж϶�����ϸ�Ƿ�����  �Լ����ܼƺ˶Ա����Ƿ����               *
    *****************************************************************/    

    if( iCheckDownloadFile(&iResult) )
    {   
            vtcp_log("[%s][%d] ===iResult====[%d]",iResult);
          if( iResult==8)
          {
                sprintf(acErrMsg,"û�����ض�����ϸ !");
          }
          else
          {
                  sprintf(acErrMsg,"������ϸ���ܼƺ˶Ա��Ĳ����� !");
          }
          
      WRITEMSG
      set_zd_long("0510",iResult);
          return 999;   
    }    
    vtcp_log("[%s][%d]=======   ִ�е�������   ������ϸ������========",__FILE__,__LINE__);
    
    return 0;
}

/* �˶���ϸ  ��ϵͳ�ǵ��� /Appl/batch/bin/checkCcpc */
int spJ620_Process()
{
      int ret=0;
    int iHandled =0;   /* �Ƿ�����һ����¼ 0=��    1=�� */     
      
      vtcp_log("[%s][%d] =======  spJ620_Process Start !!! ========",__FILE__,__LINE__);    
      
      /*****************************************************************
      * 1  ��� dhcc�� ��cpccû�� ( �������г������� ������)         *
      *****************************************************************/
      vtcp_log("[%s][%d]��ʼ��� dhcc�� ��cpccû�е����� ",__FILE__,__LINE__);
      vtcp_log("[%s][%d]��ʼ��� dhcc�� ��cpccû�е����� ",__FILE__,__LINE__);
                  
        ret = iCheckHv_zfTable(&iHandled);  /*����hv_chkagemr�˶�Hv_zf*/
        if(ret)
        {
          vtcp_log("[%s][%d]======= iCheckHv_zfTable() ����! ========",__FILE__,__LINE__);    
             sprintf(acErrMsg,"���֧����ϸ���ʳ���[%d]",ret);
             WRITEMSG
             return(ret);
        }
      vtcp_log("[%s][%d]======= After iCheckHv_zfTable()!!========",__FILE__,__LINE__);    
      vtcp_log("[%s][%d] �Ƿ�����һ����¼    iHandled=[%d]",__FILE__,__LINE__,iHandled);
    if( iHandled == 1)   /* �Ƿ�����һ����¼  1=�� */    
    {
          vtcp_log("[%s][%d]  ������һ����¼�����������ˣ���ת��DELZ ȥ���� !!!!!    iHandled=[%d]",__FILE__,__LINE__,iHandled);
          strcpy( g_pub_tx.reply, "0010" );        
          return 0;
    }  

      vtcp_log("[%s][%d] ��� dhcc�� ��cpccû�е����� ����!",__FILE__,__LINE__);
      vtcp_log("[%s][%d] ��� dhcc�� ��cpccû�е����� ����!",__FILE__,__LINE__);
          
      /*****************************************************************
      * 2  ���ccpc�еĵ�dhccû�е� ( �������г������� ������)       *
      *****************************************************************/                
      vtcp_log("[%s][%d]��ʼ���ccpc�еĵ�dhccû�еĵ����� ",__FILE__,__LINE__);
      vtcp_log("[%s][%d]��ʼ���ccpc�еĵ�dhccû�еĵ����� ",__FILE__,__LINE__);
        
        ret = iCheckHv_chkagemrTable(&iHandled); /* ����Hv_zf�˶�hv_chkagemr */
        vtcp_log("ret========[%d]",ret);
        if(ret)
        {
          vtcp_log("[%s][%d]======= iCheckHv_chkagemrTable() ����! ========",__FILE__,__LINE__);    
             sprintf(acErrMsg,"���֧����ϸ���ʳ���[%d]",ret);
             WRITEMSG
             return(ret);
        }
        
      vtcp_log("[%s][%d]======= After iCheckHv_chkagemrTable()!!========",__FILE__,__LINE__);    
      vtcp_log("[%s][%d] �Ƿ�����һ����¼    iHandled=[%d]",__FILE__,__LINE__,iHandled);
    if( iHandled == 1)   /* �Ƿ�����һ����¼  1=�� */    
    {
          vtcp_log("[%s][%d]  ������һ����¼�����������ˣ���ת��DELZ ȥ���� !!!!!    iHandled=[%d]",__FILE__,__LINE__,iHandled);
          strcpy( g_pub_tx.reply, "0010" );        
          return 0;
    }
       vtcp_log("[%s][%d] ���ccpc�еĵ�dhccû�еĵ����� ����! ",__FILE__,__LINE__);
      vtcp_log("[%s][%d] ���ccpc�еĵ�dhccû�еĵ����� ����! ",__FILE__,__LINE__);
   
      strcpy( g_pub_tx.reply, "0000" );         /* �������ˣ���ʾ����������*/    
    set_zd_long("0510",1);                 /* �������ˣ���ʾ���ʳɹ���*/    

    vtcp_log("[%s][%d] ========= ����������  ==========  ",__FILE__,__LINE__);
    vtcp_log("[%s][%d] ========= ����������  ==========  ",__FILE__,__LINE__);

      return 0;
}     
    
/*** �ж϶�����ϸ�Ƿ�����  �Լ����ܼƺ˶Ա����Ƿ���� ***/
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
           sprintf(acErrMsg,"���ҵ��˶������޼�¼");
           WRITEMSG
           return -1;
       }
       else if( g_reply_int )
       {
           sprintf(acErrMsg,"��ѯ���ҵ��˶��������ݿ����ʧ��[%d]",g_reply_int);
           WRITEMSG
           return -1;
       }     
 
    /* 1 ��˶���ϸ���� ������/�����ܱ���, ��/�����ܽ��  */   
    ret = Hv_chkagemr_Dec_Sel(g_pub_tx.reply,"tx_date = %ld and cmtno <> '232' ",g_pub_tx.tx_date);
    if( ret )
      {
          sprintf(acErrMsg,"����������ݿ����ʧ��[%d]",ret);
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
                if(iFlag == 0)  /* һ����¼Ҳû�� */
                {
                         Hv_chkagemr_Clo_Sel();
                 vtcp_log("[%s][%d] �õ� �˶������������ܱ��� CCPC.send_cnt=[%d]  ",__FILE__,__LINE__,hv_chkmr.send_cnt);    
                 vtcp_log("[%s][%d] �õ� �˶������������ܱ��� CCPC.recevice_cnt=[%d] ",__FILE__,__LINE__,hv_chkmr.recevice_cnt);    
                 if(hv_chkmr.send_cnt +    hv_chkmr.recevice_cnt > 0 ) 
                 {
                        /* ������ҵ��˵����689��û�յ�!!  */
                         sprintf(acErrMsg,"������ϸδ���سɹ� !");
                     WRITEMSG
                     *iResult = 8;
                         return -1;                          
                 }   
                 vtcp_log("[%s][%d]=  ����һ��ҵ��û�� !!! ",__FILE__,__LINE__);  
                }
                vtcp_log("[%s][%d]=���������ܱ����������ܽ�� ��� ",__FILE__,__LINE__);  
                break;                                    
          } 
          else if(ret)
          {
                sprintf(acErrMsg,"���ݿ����ʧ�� [%d]",ret);              
                WRITEMSG
                 return -1;
          }
          
          iFlag ++;
          
        ret = memcmp(hv_chkagemr.pay_qs_no, HV_QSBRNO, sizeof(hv_chkagemr.pay_qs_no)-1);
        ret2 = memcmp(hv_chkagemr.cashqsactno, HV_QSBRNO, sizeof(hv_chkagemr.cashqsactno)-1);
        if(ret && ret2)
        {
            vtcp_log("[%s][%d] ���������кͷ��������ж���Ϊ����������!!! ret=[%d]ret2=[%d]",__FILE__,__LINE__,ret,ret2);
            sprintf(acErrMsg,"���������кͷ��������ж���Ϊ����������!! ret=[%d] ret2=[%d]",ret,ret2);
            WRITEMSG
              return -1;    
        }         
        if(ret==0 && ret2==0)
        {
            vtcp_log("[%s][%d] ���������кͷ��������ж��Ǳ���������!!! ret=[%d]ret2=[%d]",__FILE__,__LINE__,ret,ret2);
            sprintf(acErrMsg,"���������кͷ��������ж��Ǳ���������!! ret=[%d] ret2=[%d]",ret,ret2);
            WRITEMSG
              return -1;    
        }      
        if(ret == 0)  /* ���� */
        {                    
              iSendCnt = iSendCnt + 1;
              dSendAmt = dSendAmt + hv_chkagemr.tx_amt;    
           }
        else    /* ���� */
        {
              iReceiveCnt = iReceiveCnt + 1;
              dReceiveAmt = dReceiveAmt + hv_chkagemr.tx_amt;
        }    
    }    
      Hv_chkagemr_Clo_Sel();

    vtcp_log("[%s][%d] �õ� �˶���ϸ���� �����ܱ��� iSendCnt=[%d] ",__FILE__,__LINE__,iSendCnt);    
    vtcp_log("[%s][%d] �õ� �˶���ϸ���� �����ܽ�� dSendAmt=[%lf] ",__FILE__,__LINE__,dSendAmt);    
    vtcp_log("[%s][%d] �õ� �˶���ϸ���� �����ܱ��� iReceiveCnt=[%d] ",__FILE__,__LINE__,iReceiveCnt);    
    vtcp_log("[%s][%d] �õ� �˶���ϸ���� �����ܽ�� dReceiveAmt=[%lf] ",__FILE__,__LINE__,dReceiveAmt);    

    /* 2 ��˶������� ������/�����ܱ���, ��/�����ܽ��  */
    /*   ��ǰ��������    2006-11-4 23:22
    memset(&hv_chkmr,0,sizeof(struct hv_chkmr_c) );    
       g_reply_int = Hv_chkmr_Sel(g_pub_tx.reply,&hv_chkmr," 1=1 ");
       if( g_reply_int==100 )
       {
           sprintf(acErrMsg,"���ҵ��˶������޼�¼");
           WRITEMSG
           return -1;
       }
       else if( g_reply_int )
       {
           sprintf(acErrMsg,"��ѯ���ҵ��˶��������ݿ����ʧ��[%d]",g_reply_int);
           WRITEMSG
           return -1;
       } */    
    vtcp_log("[%s][%d] �õ� �˶������������ܱ��� CCPC.send_cnt=[%d]  ",__FILE__,__LINE__,hv_chkmr.send_cnt);    
    vtcp_log("[%s][%d] �õ� �˶������������ܽ�� CCPC.send_amt=[%lf]",__FILE__,__LINE__,hv_chkmr.send_amt);    
    vtcp_log("[%s][%d] �õ� �˶������������ܱ��� CCPC.recevice_cnt=[%d] ",__FILE__,__LINE__,hv_chkmr.recevice_cnt);    
    vtcp_log("[%s][%d] �õ� �˶������������ܽ�� CCPC.recevice_amt=[%lf] ",__FILE__,__LINE__,hv_chkmr.recevice_amt);    

    /* 3 �ж� ������ϸ �� �ܼƺ˶Ա��� �Ƿ����  */
    if( (hv_chkmr.send_cnt == iSendCnt) && (hv_chkmr.recevice_cnt == iReceiveCnt) && 
          (hv_chkmr.send_amt-dSendAmt > -0.0001 || hv_chkmr.send_amt-dSendAmt <0.0001)  && 
        (hv_chkmr.recevice_amt-dReceiveAmt > -0.0001 || hv_chkmr.recevice_amt-dReceiveAmt < 0.0001) )
    {
          vtcp_log("[%s][%d] ==== ������ϸ������  ���ܼƺ˶Ա������!! =====",__FILE__,__LINE__);    
              return 0;
    }
    else
    {
               sprintf(acErrMsg,"���ҵ��˶������޼�¼");
               WRITEMSG
               *iResult = 9;
              return -1;
    }
       return 0; 
}
/*******************************************************************************
* ��������:iCheckHvForDateOver                                                 *
* ��    ��: �����֧��ҵ���������Ƿ�����Ѿ����˼��ʵ�δ���ͻ�ȴ�����       *
*          ȷ�ϻ��Ŷӵ�����                                                    *
* ��    ��:chk_br_no , iErrCnt                                                 *
* ��    ��:char *    , int *                                                   *
* �� �� ֵ:int                                                                 *
* -----------------------------------------------------------------------------*
* ���� :Ҫ���Ļ�����,Ϊ�ձ�ʾȫ��                                            *
* ��� :����ֵ == 0, iErrCnt ���ش��ڵı���,����ֵ !=0 ��ʾ���ݿ����          *
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
              sprintf(acErrMsg,"���ݿ��������!");
              WRITEMSG
                     return -1;                 
         }      
      
   }
   else
   {
         ret = Hv_zf_Dec_Sel(g_pub_tx.reply," tx_date=%d and hv_sts='0' and br_no = '%s' ",tx_date, chk_br_no);
       if( ret )
         {
              sprintf(acErrMsg,"���ݿ��������!");
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
         sprintf(acErrMsg,"���ݿ��������!");
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
* ��������:iCheckHv_zfTable                                                    *
* ��    ��: ����hv_chkagemr�˶�Hv_zf
            ��� dhcc�� ��cpccû�� �����                                      *
            ����CCPCû�е����,����ҷ�������                                  *
* ��    ��:                                                                    *
* ��    ��:                                                                    *
* �� �� ֵ:int                                                                 *
* -----------------------------------------------------------------------------*
* ���� :                                                                       *
* ��� :                                                                       *
*******************************************************************************/
int iCheckHv_zfTable(int* iHandled)
{
      int ret;
      struct hv_zf_c hv_zf;
      struct hv_chkagemr_c hv_chkagemr;
      char cAcno110[20];
      int iCmtno=0;
                  
      vtcp_log("[%s][%d] ***** iCheckHv_zfTable() ��ʼ !  ***** ",__FILE__,__LINE__);
      vtcp_log("[%s][%d] ***** iCheckHv_zfTable() ��ʼ !  ***** ",__FILE__,__LINE__);
      /* �ҷ������ʵ��������Ҫ���ǣ� ����ֻ������ʵ����           */
      /* iCheckHv_chkagemrTable()�в���ļ�¼ checkflag= 'D',���ٴ���    */
      /* ������һ������״̬ '0' Ҳ������   2006-10-30 23:07
      ret = Hv_zf_Dec_Upd(g_pub_tx.reply, 
      "tx_date=%d   and ( hv_sts not in ('5','6','A','C','8','D','L','K') ) ", g_pub_tx.tx_date); */
      ret = Hv_zf_Dec_Upd(g_pub_tx.reply, 
      "tx_date=%d   and ( hv_sts  in ('0','1','2','3','4','7','9','B','F','G','H','I','J') ) and (checkflag is null) and cmtno!='802' and cmtno!='812' and cmtno!='814'  and cmtno !='232' ", g_pub_tx.tx_date);       
      if ( ret )
      {
            sprintf(acErrMsg,"Hv_zf_Dec_Sel() ����[%d]",ret);
            WRITEMSG
              return ret;
      }          
        
    while(1)
    {
        memset(&hv_zf,0, sizeof(hv_zf));        
        ret = Hv_zf_Fet_Upd(&hv_zf, g_pub_tx.reply);
        if( ret==100 )
        {
            vtcp_log("[%s][%d] ���֧�����ʼ�¼ѭ���������  ",__FILE__,__LINE__);    
            Hv_zf_Clo_Sel();                      
            break;
        }
        else if(ret)
        {
            vtcp_log("[%s][%d]���ݿ�����쳣,����Ƽ�����ϵ!",__FILE__,__LINE__);
            sprintf(acErrMsg,"���ݿ�����쳣,����Ƽ�����ϵ![%d]",ret);
            WRITEMSG
            return ret;
        }
        
        memset(&hv_chkagemr,0, sizeof( hv_chkagemr ) );
        ret = Hv_chkagemr_Dec_Upd(g_pub_tx.reply, " or_br_no='%s' and orderno='%s' and ( chk_flg ='0' or chk_flg is null )", hv_zf.or_br_no,hv_zf.orderno); 
        if(ret)
        {
            vtcp_log("[%s][%d]Hv_chkagemr_Dec_Upd() ����!",__FILE__,__LINE__);
                sprintf(acErrMsg,"Hv_chkagemr_Dec_Upd() ����![%d]",ret);
                WRITEMSG
                  return ret;
        } 
        ret = Hv_chkagemr_Fet_Upd(&hv_chkagemr, g_pub_tx.reply);
        if(ret && ret != 100)
        {
            vtcp_log("[%s][%d]���ݿ�����쳣,����Ƽ�����ϵ!",__FILE__,__LINE__);
                sprintf(acErrMsg,"���ݿ�����쳣,����Ƽ�����ϵ![%d]",ret);
                WRITEMSG
                  return ret;
        }else if(ret == 100)  /* dhcc�� ��cpccû�� ����ҷ������� */
        {
                  /***********  ���ñ�����״̬��Ϊ REJECT_EACTCLS   *********/
                  
                  if(hv_zf.lw_ind[0] == '1')  /* ����*/
                  {
                     hv_zf.beg_sts[0] = hv_zf.hv_sts[0]; 
                       hv_zf.hv_sts[0] = '5';        /* ��Ϊ���ʾܾ� */
                  }
                  else  /* ����*/
                  {
                       hv_zf.hv_sts[0] = 'L';        /* �˶Բ��� ���� */
                  } 
           
            hv_zf.checkflag[0] = 'B';     /* DHCC�У�CCPCû�� */       
            ret=Hv_zf_Upd_Upd(hv_zf, g_pub_tx.reply);
            if( ret)
            {
                vtcp_log("[%s][%d] ���ݿ�����쳣,����Ƽ�����ϵ!  ",__FILE__,__LINE__);    
                    sprintf(acErrMsg,"���ݿ�����쳣,����Ƽ�����ϵ![%d]",ret);
                    WRITEMSG
                      return -1;
            } 
                 vtcp_log("[%s][%d] ***** ��������״̬�ɹ� !  ***** ",__FILE__,__LINE__);
                 vtcp_log("[%s][%d] hv_zf.cmtno=[%s] ",__FILE__,__LINE__,hv_zf.cmtno);

            iCmtno = atoi(hv_zf.cmtno);
            
            /* ��Ʊ��DHCC���� ��Ҫ���»�Ʊ��״̬ (CMT 121 122 123 124) */
            if(iCmtno==121 || iCmtno==122 || iCmtno==123 || iCmtno==124 )
            {  
                    vtcp_log("[%s,%d] ��Ʊ��DHCC���� ��Ҫ���»�Ʊ��״̬ !",__FILE__,__LINE__);                        
                          ret = iHv_DZ_ChangeHPStat(&hv_zf,iCmtno);
                  if(ret)
                  {
                        vtcp_log("%s,%d ���»�Ʊ��Ϣ����� ret=[%d]",__FILE__,__LINE__,ret);
                        vtcp_log(acErrMsg,"���»�Ʊ��Ϣ�����");
                        WRITEMSG
                        return ret;
                  }        
                    vtcp_log("[%s,%d] ���»�Ʊ״̬�ɹ�!  ",__FILE__,__LINE__);        
            }

                  /***********   �Ǽǲ���     ***************/                          
            ret = iInsertHv_chkerror(&hv_zf,&hv_chkagemr);
            if(ret)
            {
                vtcp_log("[%s][%d]���ݿ�����쳣,����Ƽ�����ϵ!",__FILE__,__LINE__);
                    sprintf(acErrMsg,"���ݿ�����쳣,����Ƽ�����ϵ![%d]",ret);
                    WRITEMSG
                      return ret;
            }  
          
            vtcp_log("[%s][%d] ********************************* ",__FILE__,__LINE__);
            vtcp_log("[%s][%d] ����CCPCû�е����,����ҷ������� ",__FILE__,__LINE__);
            vtcp_log("[%s][%d] ����CCPCû�е����,����ҷ������� ",__FILE__,__LINE__);
            vtcp_log("[%s][%d] ********************************* ",__FILE__,__LINE__);
            /* begin add by LiuHuafeng 2009-5-14 17:01:51 for ��Ȫ�������� */
            if(strncmp(hv_zf.br_no,HV_CUNZHEN_BR_NO,BRNO_LEN-2))
            {
            /* end by LiuHuafeng 2009-5-14 17:02:02 ��Ȫ�������� �������еĲ�����������������8583�� */
                /*******  ��8583��  ����ҷ�������  ��������� *******/  
                if(iCmtno==100 || iCmtno==101 || iCmtno==102 || iCmtno==103 || 
                     iCmtno==105 || iCmtno==108 || iCmtno==122) 
                {  /* ��ͨ��: (CMT 100 101 102 103 105 108 122)*/
                    
                    vtcp_log("[%s][%d] ��ͨ�� ��������� ",__FILE__,__LINE__);
                      
                    ret = iSet8583_For_CommonCMT(&hv_zf,-1);  /*** -1=��������� ***/
                    if(ret)
                    {
                        vtcp_log("[%s][%d]iSet8583_For_CommonCMT() ����!",__FILE__,__LINE__);
                        sprintf(acErrMsg,"����8583��ʱ����![%d]",ret);
                        WRITEMSG
                        return -1;
                    }  
                
                }
                else  if(iCmtno==121 || iCmtno==123 || iCmtno==124 )
                {  /*  ������ (CMT 121 123 124)  */
                
                         vtcp_log("[%s][%d] ������  ��������� ",__FILE__,__LINE__);   /*  tx_amt: δ�ý��    jf_amt: Ʊ���� */
                      ret = iSet8583_For_SpecialCMT(&hv_zf, hv_chkagemr.p_num, hv_chkagemr.p_date,hv_chkagemr.jf_amt,-1);  /*** -1=��������� ***/
                      if(ret)
                      {
                        vtcp_log("[%s][%d]iSet8583_For_SpecialCMT() ����!",__FILE__,__LINE__);
                            sprintf(acErrMsg,"����8583��ʱ����![%d]",ret);
                            WRITEMSG
                              return -1;
                      }    
                }
                else
                {
                         sprintf(acErrMsg," �ñ��Ĳ������ﴦ�� !! iCmtno[%d]",iCmtno);
                         WRITEMSG
                           return -1;                
                }
            }
            /* ��chk_flg��ΪCCPC_SAME_BANK����iCheckHv_chkagemrTable()�оͲ��ô�����Щ��¼�� */          
            hv_chkagemr.chk_flg[0]='B'; /* DHCC�У�CCPCû�� */
            ret = Hv_chkagemr_Upd_Upd(hv_chkagemr, g_pub_tx.reply);
            if(ret)
            {
                vtcp_log("[%s][%d]���ݿ�����쳣,����Ƽ�����ϵ!",__FILE__,__LINE__);
                sprintf(acErrMsg,"���ݿ�����쳣,����Ƽ�����ϵ![%d]",ret);
                WRITEMSG
                return ret;
            }
            
            Hv_chkagemr_Clo_Upd();
            vtcp_log("[%s][%d] ***** iCheckHv_zfTable() �ɹ� !  ***** ",__FILE__,__LINE__);
            /* begin add by LiuHuafeng 2009-5-14 17:01:51 for ��Ȫ�������� */
            if(strncmp(hv_zf.br_no,HV_CUNZHEN_BR_NO,BRNO_LEN-2))
            {
            /* end by LiuHuafeng 2009-5-14 17:02:02 ��Ȫ�������� �������еĲ�����������
             *������8583��
             */
            /* end by LiuHuafeng 2009-5-14 17:03:28 */
                Hv_zf_Clo_Upd();
                *iHandled = 1;   /* ������һ����¼  !! */
                return 0;
            /* begin add by LiuHuafeng 2009-5-14 17:04:06 for ��Ȫ�������� */
            }
            else
            {
                vtcp_log("%s %d �������ж࣬����ֻ����ʶ�������� ",__FILE__,__LINE__);
                *iHandled = 0;
                continue;
            }
            /* end by LiuHuafeng 2009-5-14 17:04:29 */
        }

        /* ����˵������˫�����ʽ�����ϵ����  */                        
        vtcp_log("[%s][%d] �ҷ��е�CCPCҲ�е���� ",__FILE__,__LINE__);
        vtcp_log("[%s][%d] hv_zf.tx_amt===[%lf] ",__FILE__,__LINE__,hv_zf.tx_amt);
        vtcp_log("[%s][%d] hv_chkagemr.tx_amt===[%lf] ",__FILE__,__LINE__,hv_chkagemr.tx_amt);

        hv_zf.checkflag[0] = 'A';    /* DHCC��CCPC��ͬ */
        ret = Hv_zf_Upd_Upd(hv_zf, g_pub_tx.reply);
        if( ret)
        {
            vtcp_log("[%s][%d] ���ݿ�����쳣,����Ƽ�����ϵ!  ",__FILE__,__LINE__);    
            sprintf(acErrMsg,"���ݿ�����쳣,����Ƽ�����ϵ![%d]",ret);
            WRITEMSG
              return -1;
        }
        vtcp_log("[%s][%d] ***** ��������״̬�ɹ� !  ***** ",__FILE__,__LINE__);
          
        /* ��chk_flg��ΪCCPC_SAME_BANK����iCheckHv_chkagemrTable()�оͲ��ô�����Щ��¼�� */          
        hv_chkagemr.chk_flg[0]= 'A';  /* CCPC_SAME_BANK DHCC��CCPC��ͬ */
        ret = Hv_chkagemr_Upd_Upd(hv_chkagemr, g_pub_tx.reply);
        if(ret)
        {
            vtcp_log("[%s][%d]���ݿ�����쳣,����Ƽ�����ϵ!",__FILE__,__LINE__);
            sprintf(acErrMsg,"���ݿ�����쳣,����Ƽ�����ϵ![%d]",ret);
            WRITEMSG
              return ret;
        } 
        vtcp_log("[%s][%d] *****��һ��DHCC��CCPC���У�����ѭ�������� !  ***** ",__FILE__,__LINE__);     
      }
      
      Hv_chkagemr_Clo_Upd(); 
      Hv_zf_Clo_Upd();   
    vtcp_log("[%s][%d] ***** iCheckHv_zfTable() �ɹ� !  ***** ",__FILE__,__LINE__);
    vtcp_log("[%s][%d] ***** iCheckHv_zfTable() �ɹ� !  ***** ",__FILE__,__LINE__);
      return 0;
}


/*******************************************************************************
* ��������: iCheckHv_chkagemrTable                                             *
* ��    ��: ����hv_chkagemr�˶�Hv_zf���ccpc�� ��dhccû�� ����� 
            CCPC�� dhccû�� �����,                                            *
            ���ʣ�����ccpc����hv_zf������
            ���ʣ��ҵ�ԭ���������ʣ��ñ�־������                               *
* ��    ��:                                                                    *
* ��    ��:                                                                    *
* �� �� ֵ:int                                                                 *
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
      int iLw_ind = 0;  /* iLw_ind=1����     iLw_ind=2����*/
      char cKinbr[BRNO_LEN+1];
      int iCzBankFlag = 0;/* �Ƿ��Ǵ�����������0=��    1=�� */
      
      
    /* dhcc��cpcc���е����������iCheckHv_zfTable()�н�checkflag��Ϊ 'A'�� 
       ����ֻ���checkflag='0' or checkflag is null �����, ���� CCPC�� dhccû�� ����� */
      ret = Hv_chkagemr_Dec_Upd(g_pub_tx.reply, "tx_date=%d   and ( chk_flg ='0' or chk_flg is null ) and cmtno<>'232'  ", g_pub_tx.tx_date);
      if ( ret )
      {
            sprintf(acErrMsg,"Hv_chkagemr_Dec_Sel() ����[%d]",ret);
            WRITEMSG
              return ret;
      }          
        
    while(1)
    {
        memset(&hv_chkagemr,0, sizeof( hv_chkagemr ) );               
        ret=Hv_chkagemr_Fet_Upd(&hv_chkagemr, g_pub_tx.reply);
        if( ret==100 )
        {
            vtcp_log("[%s][%d] ���֧��������ϸ��ѭ���������  ",__FILE__,__LINE__); 
              Hv_zf_Clo_Sel();                      
            break;
        }
        else if(ret)
        {
            vtcp_log("[%s][%d]���ݿ�����쳣,����Ƽ�����ϵ!",__FILE__,__LINE__);
                sprintf(acErrMsg,"���ݿ�����쳣,����Ƽ�����ϵ![%d]",ret);
                WRITEMSG
                  return ret;
        }
        
        vtcp_log("[%s][%d] hv_chkagemr.pay_ac_no==[%s]  hv_chkagemr.cash_ac_no==[%s]  ",__FILE__,__LINE__, hv_chkagemr.pay_ac_no,hv_chkagemr.cash_ac_no); 
        
        
                /* ------���� PAY_QS_NO �ж������ʻ������� ---------------------------------*/
        /* ----- PAY_QS_NO Ϊ���������е�Ϊ���ʣ�����Ϊ����  -------------------- */
        vtcp_log("[%s][%d]  hv_chkagemr.pay_qs_no==[%s] hv_chkagemr.cashqsactno=[%s] HV_QSBRNO=[%s]",__FILE__,__LINE__,hv_chkagemr.pay_qs_no,hv_chkagemr.cashqsactno,HV_QSBRNO);        
        
        ret = memcmp(hv_chkagemr.pay_qs_no, HV_QSBRNO, sizeof(hv_chkagemr.pay_qs_no)-1);
        ret2 = memcmp(hv_chkagemr.cashqsactno, HV_QSBRNO, sizeof(hv_chkagemr.cashqsactno)-1);
        if(ret && ret2)
        {
            vtcp_log("[%s][%d] ���������кͷ��������ж���Ϊ����������!!! ret=[%d]ret2=[%d]",__FILE__,__LINE__,ret,ret2);
            sprintf(acErrMsg,"���������кͷ��������ж���Ϊ����������!! ret=[%d] ret2=[%d]",ret,ret2);
            WRITEMSG
               return -1;    
        } 
        
        if(ret==0 && ret2==0)
        {
            vtcp_log("[%s][%d] ���������кͷ��������ж��Ǳ���������!!! ret=[%d]ret2=[%d]",__FILE__,__LINE__,ret,ret2);
            sprintf(acErrMsg,"���������кͷ��������ж��Ǳ���������!! ret=[%d] ret2=[%d]",ret,ret2);
            WRITEMSG
               return -1;    
        }  
         
        if(ret)  /* ���� */
        {                    
             iLw_ind = 2;
          }
        else  /* ���� */
        {
             iLw_ind = 1;          
        }    
        vtcp_log("[%s][%d] iLw_ind=========[%d]  ",__FILE__,__LINE__, iLw_ind);   
        vtcp_log("[%s][%d] hv_chkagemr.pay_ac_no==[%s]  hv_chkagemr.cash_ac_no==[%s]  ",__FILE__,__LINE__, hv_chkagemr.pay_ac_no,hv_chkagemr.cash_ac_no); 

        
        /* ----- 1. ���ʣ�(û�յ�) ����ccpc����hv_zf������            ------------- */ 
        /* ----- 2. ���ʣ�(���ټ�) ���������ʾܾ���ccpc�������ˣ�    ---------------*/
        /* ----- �����ҵ�ԭ���������ʣ��ñ�־������                  -------------- */  
        /* ----- ����ֻ���ݹؼ��ֲ��ң���Ϊ�������������¼���޷����룬ֻ���޸�-----*/  
        ret = Hv_zf_Dec_Upd(g_pub_tx.reply,"or_br_no='%s' and orderno='%s' and  cmtno='%s' and wt_date=%d ", 
                 hv_chkagemr.or_br_no,hv_chkagemr.orderno,hv_chkagemr.cmtno,hv_chkagemr.tx_date );
        if(ret)
        {
                sprintf(acErrMsg,"Hv_zf_Dec_Upd() ����[%d]",ret);
                WRITEMSG
                  return ret;
        }
         
          memset(&hv_zf,0, sizeof(hv_zf));         
        ret = Hv_zf_Fet_Upd(&hv_zf,g_pub_tx.reply);
        if(ret && ret !=100)
        {
                sprintf(acErrMsg,"Hv_zf_Dec_Upd() ����[%d]",ret);
                WRITEMSG
                  return ret;
        } 
        if(ret == 100)   /* û�ҵ� DHCC���� û����ʽ��׵ļ�¼  ����ccpc����hv_zf  ��������� */
        {
              /* ==== ���� hv_chkagemr ���� hv_zf =======*/
              vtcp_log("[%s][%d] û����ʽ��׵ļ�¼ !",__FILE__,__LINE__);

            memset(&hv_zf,0,sizeof(hv_zf));    
            /*  
            vtcp_log("[%s][%d] ============ Hv_chkagemr_Debug() ========\n",__FILE__,__LINE__);    
            Hv_chkagemr_Debug(hv_chkagemr);
            vtcp_log("[%s][%d] ============ Hv_chkagemr_Debug() ========\n",__FILE__,__LINE__);    
            */      

            vtcp_log("[%s][%d] hv_chkagemr.pay_ac_no==[%s]  hv_chkagemr.cash_ac_no==[%s]  ",__FILE__,__LINE__, hv_chkagemr.pay_ac_no,hv_chkagemr.cash_ac_no); 
            vtcp_log("[%s][%d] hv_zf.pay_ac_no==[%s]  hv_zf.cash_ac_no==[%s]  ",__FILE__,__LINE__, hv_zf.pay_ac_no,hv_zf.cash_ac_no); 

            vtcp_log("[%s][%d] iLw_ind=========[%d]  ",__FILE__,__LINE__, iLw_ind);    
            if(iLw_ind == 2)  /* ���� */
            {                    
              hv_zf.lw_ind[0] = '2'; 
            }
            else if(iLw_ind == 1)   /* ���� */
            {
              hv_zf.lw_ind[0] = '1';     
            }else
            {
              sprintf(acErrMsg,"������ʶ��[%d]",iLw_ind);
              WRITEMSG
                 return ret;
            }            
            vtcp_log("[%s][%d] hv_zf.lw_ind=[%s]  ",__FILE__,__LINE__,hv_zf.lw_ind);

            hv_zf.hv_sts[0] =  '9';  /* Ĭ��Ϊ ���˼������㣬���ʼ��ʱ�ٸ���������� */
            hv_zf.beg_sts[0] = hv_zf.hv_sts[0]; 
               iCopyHv_zfFromHv_chkagemr(&hv_zf,&hv_chkagemr);
            
            vtcp_log("[%s][%d] hv_chkagemr.pay_ac_no==[%s]  hv_chkagemr.cash_ac_no==[%s]  ",__FILE__,__LINE__, hv_chkagemr.pay_ac_no,hv_chkagemr.cash_ac_no); 
            vtcp_log("[%s][%d] hv_zf.pay_ac_no==[%s]  hv_zf.cash_ac_no==[%s]  ",__FILE__,__LINE__, hv_zf.pay_ac_no,hv_zf.cash_ac_no); 

            /* ���ʺ� ���ϱ�־��������һѭ��ʱ������*/
            hv_chkagemr.chk_flg[0] = 'D';     /*NOT_FOUND_BANK  CCPC�У�DHCCû��*/  
            /* begin add by LiuHuafeng 2009-5-14 17:09:11 */
            /* ����Ǵ������У�����Ҫ���������� */
            if(!strncmp(hv_chkagemr.br_no,HV_CUNZHEN_BR_NO,BRNO_LEN-2))
            {
                iCzBankFlag = 1;
                vtcp_log("%s,%d �������������٣�ֻ���Ǽǲ�����",__FILE__,__LINE__);
            }
            else
            {
                iCzBankFlag = 0;
            }
            /* end by LiuHuafeng for �������� */
            *iHandled = 1;     /* �������ҵ���е���Ҫ���ʣ��������еĲ���Ҫ���� */   
            iInsertFlag = 1;   /* ��־ �����¼ */
        }
        else   /* �ҵ� Ҳ����DHCC���� !!   �ҵ���ʽ��׵ļ�¼  �޸ı�־ ��������� */
        {
              if( hv_zf.hv_sts[0] == '0' || hv_zf.hv_sts[0] == '1' || hv_zf.hv_sts[0] == '2' || hv_zf.hv_sts[0] == '3' ||
                     hv_zf.hv_sts[0] == '4' || hv_zf.hv_sts[0] == '7' || hv_zf.hv_sts[0] == '9' ||
                     hv_zf.hv_sts[0] == 'B' || hv_zf.hv_sts[0] == 'F' || hv_zf.hv_sts[0] == 'G' || 
                     hv_zf.hv_sts[0] == 'H' || hv_zf.hv_sts[0] == 'I' || hv_zf.hv_sts[0] == 'J'  )
              {      /*  �������Ӧ���ǲ����ܵģ����ﻹ���ж�һ�� */
                    vtcp_log("[%s][%d] ==== ��������ˣ�DHCC��CCPC���У�Ӧ���� iCheckHv_zfTable()�д��� !!!!  ==== ",__FILE__,__LINE__);
                    vtcp_log("[%s][%d] ==== ��ʽ���Ӧ���� iCheckHv_zfTable()�д������ﲻ���� !!!!  ==== ",__FILE__,__LINE__);
            
                    /* hv_zf.checkflag[0] = 'A';    DHCC��CCPC��ͬ */
                    /* hv_chkagemr.chk_flg[0]= 'A';  CCPC_SAME_BANK DHCC��CCPC��ͬ */
                    /* *iHandled = 0;  ����Ҫ���� */
                         sprintf(acErrMsg,"===== ��������� ���ݿ��м�¼���� !! ====[%s]",hv_zf.hv_sts);
                         WRITEMSG
                           return -1;
              }
              else
              {
                   vtcp_log("[%s][%d] �ҵ���ʽ��׵ļ�¼ �޸ı�־ !",__FILE__,__LINE__);       
                     hv_zf.hv_sts[0] =  '9';  /* Ĭ��Ϊ ���˼������㣬���ʼ��ʱ�ٸ���������� */
                     hv_zf.beg_sts[0] = hv_zf.hv_sts[0]; 
                   hv_zf.checkflag[0] = 'D';     /*NOT_FOUND_BANK  CCPC�У�DHCCû��*/

                   vtcp_log("[%s][%d] hv_chkagemr.pay_ac_no==[%s]  hv_chkagemr.cash_ac_no==[%s]  ",__FILE__,__LINE__, hv_chkagemr.pay_ac_no,hv_chkagemr.cash_ac_no); 
                   vtcp_log("[%s][%d] hv_zf.pay_ac_no==[%s]  hv_zf.cash_ac_no==[%s]  ",__FILE__,__LINE__, hv_zf.pay_ac_no,hv_zf.cash_ac_no); 

                   vtcp_log("[%s][%d] iLw_ind=========[%d]  ",__FILE__,__LINE__, iLw_ind);
                   if(iLw_ind == 2)  /* ���� */
                   {                    
                     hv_zf.lw_ind[0] = '2'; 
                   }
                   else if(iLw_ind == 1)   /* ���� */
                   {
                     hv_zf.lw_ind[0] = '1';     
                   }else
                   {
                     sprintf(acErrMsg,"������ʶ��[%d]",iLw_ind);
                     WRITEMSG
                        return ret;
                   }            
                   vtcp_log("[%s][%d] hv_zf.lw_ind=[%s]  ",__FILE__,__LINE__,hv_zf.lw_ind);

                   /* �����ֶ���? Ӧ�ð���CCPC�ģ����������Hv_zfhtr�л���һ������ ?? */
                   iCopyHv_zfFromHv_chkagemr(&hv_zf,&hv_chkagemr);
                   vtcp_log("%s,%d brno=%s",__FILE__,__LINE__,hv_zf.br_no);
                   vtcp_log("[%s][%d] hv_chkagemr.pay_ac_no==[%s]  hv_chkagemr.cash_ac_no==[%s]  ",__FILE__,__LINE__, hv_chkagemr.pay_ac_no,hv_chkagemr.cash_ac_no); 
                   vtcp_log("[%s][%d] hv_zf.pay_ac_no==[%s]  hv_zf.cash_ac_no==[%s]  ",__FILE__,__LINE__, hv_zf.pay_ac_no,hv_zf.cash_ac_no); 

                   /* ���ʺ� ���ϱ�־��������һѭ��ʱ������*/
                   hv_chkagemr.chk_flg[0] = 'D';     /*NOT_FOUND_BANK  CCPC�У�DHCCû��*/
                   
                   /* ����Ǵ������У�����Ҫ���������� */
                   /* begin add by LiuHuafeng 2009-5-14 17:09:11 */
                   /* ����Ǵ������У�����Ҫ���������� */
                   if(!strncmp(hv_chkagemr.br_no,HV_CUNZHEN_BR_NO,BRNO_LEN-2))
                   {
                       iCzBankFlag = 1;
                       vtcp_log("%s,%d �������������٣�ֻ���Ǽǲ�����",__FILE__,__LINE__);
                   }
                   else
                   {
                       iCzBankFlag = 0;
                   }
                   *iHandled = 1;     /* ��Ҫ���� */            
              }          
        } 

        ret = Hv_chkagemr_Upd_Upd(hv_chkagemr,g_pub_tx.reply);
        if(ret)
        {
             vtcp_log("[%s][%d] Hv_chkagemr_Upd_Upd() ����!",__FILE__,__LINE__);
                 sprintf(acErrMsg,"���ݿ�����쳣,����Ƽ�����ϵ![%d]",ret);
                 WRITEMSG
                   return ret;
        }             
        vtcp_log("[%s][%d]  �޸� Hv_chkagemr.chk_flg ��־�ɹ�!  ",__FILE__,__LINE__);      

        if(*iHandled == 0)   /* ����Ҫ���ʣ�����ѭ������ */
        {
                Hv_zf_Clo_Upd(); 
              vtcp_log("[%s][%d] ����Ҫ���ʣ�����ѭ������ !!!  ",__FILE__,__LINE__);
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
            {  /* ��ͨ��: (CMT 100 101 102 103 105 108 122)*/
                     vtcp_log("[%s][%d] ��ͨ�� ��������� ",__FILE__,__LINE__);
                vtcp_log("[%s][%d] hv_chkagemr.pay_ac_no==[%s]  hv_chkagemr.cash_ac_no==[%s]  ",__FILE__,__LINE__, hv_chkagemr.pay_ac_no,hv_chkagemr.cash_ac_no); 
                vtcp_log("[%s][%d] hv_zf.pay_ac_no==[%s]  hv_zf.cash_ac_no==[%s]  ",__FILE__,__LINE__, hv_zf.pay_ac_no,hv_zf.cash_ac_no); 
            
                  ret = iSet8583_For_CommonCMT(&hv_zf,1);  /*** 1=��������� ***/
                  if(ret)
                  {
                    vtcp_log("[%s][%d]iSet8583_For_CommonCMT() ����!",__FILE__,__LINE__);
                  sprintf(acErrMsg,"����8583��ʱ����![%d]",ret);
                  WRITEMSG
                    return ret;
                  }          
            
            }            
            else  if(iCmtno==121 || iCmtno==123 || iCmtno==124 )
            {  /*  ������ (CMT 121 123 124)  */
                     vtcp_log("[%s][%d] ������ ���������  iCmtno=[%d] ",__FILE__,__LINE__,iCmtno);
                vtcp_log("[%s][%d] hv_chkagemr.pay_ac_no==[%s]  hv_chkagemr.cash_ac_no==[%s]  ",__FILE__,__LINE__, hv_chkagemr.pay_ac_no,hv_chkagemr.cash_ac_no); 
                vtcp_log("[%s][%d] hv_zf.pay_ac_no==[%s]  hv_zf.cash_ac_no==[%s]  ",__FILE__,__LINE__, hv_zf.pay_ac_no,hv_zf.cash_ac_no); 
            
                  ret = iSet8583_For_SpecialCMT(&hv_zf, hv_chkagemr.p_num, hv_chkagemr.p_date,hv_chkagemr.jf_amt,1);  /*** 1=��������� ***/
                if(ret)
                  {
                    vtcp_log("[%s][%d]iSet8583_For_SpecialCMT() ����!",__FILE__,__LINE__);
                  sprintf(acErrMsg,"����8583��ʱ����![%d]",ret);
                  WRITEMSG
                    return ret;
                  }  
            }     
            else
            {
               sprintf(acErrMsg," �ñ��Ĳ������ﴦ��!! ");
               WRITEMSG
                 return -1;                
            }
            memset(cKinbr,0,sizeof(cKinbr));
            get_zd_data("0030",cKinbr);
            MEMCPY_DEBUG(hv_zf.br_no,cKinbr,sizeof(hv_zf.br_no)-1);
        /*begin by LiuHuafeng 2009-5-16 19:38:06 for �������� */
        }
        /* end by LiuHuafeng 2009-5-16 19:38:23 */
        vtcp_log("[%s][%d]  hv_zf.br_no=====[%s]  ",__FILE__,__LINE__,hv_zf.br_no); 
        vtcp_log("[%s][%d]  hv_zf.br_no=====[%s]  ",__FILE__,__LINE__,hv_zf.br_no); 
        vtcp_log("[%s][%d] hv_chkagemr.pay_ac_no==[%s]  hv_chkagemr.cash_ac_no==[%s]  ",__FILE__,__LINE__, hv_chkagemr.pay_ac_no,hv_chkagemr.cash_ac_no); 
        vtcp_log("[%s][%d] hv_zf.pay_ac_no==[%s]  hv_zf.cash_ac_no==[%s]  ",__FILE__,__LINE__, hv_zf.pay_ac_no,hv_zf.cash_ac_no); 

        /*  �����ʵ�״̬  */
        if(hv_zf.lw_ind[0] == '1')  /* ���ʶ���ʱ�п��ܹ��� */
        {
/*          if( hv_zf.hv_sts[0] != RECEIVE_CUSTNAME_ERR && hv_zf.hv_sts[0] != RECEIVE_DATA_ERR )
            {
                    hv_zf.hv_sts[0] = 'L'; 
            }
*/
            hv_zf.hv_sts[0] = 'L';        /* ��Ϊ �˶Բ��� */
        }                                

        if( iInsertFlag )   /* û���ҵ���Ӧ��¼ ��� */
        {
              /* ==== ���� hv_chkagemr ���� hv_zf =======*/
              vtcp_log("[%s][%d] û����ʽ��׵ļ�¼������ccpc����hv_zf!",__FILE__,__LINE__);
            vtcp_log("[%s][%d]  hv_zf.hv_sts=====[%s]  ",__FILE__,__LINE__,hv_zf.hv_sts);                

            vtcp_log("[%s][%d] hv_chkagemr.pay_ac_no==[%s]  hv_chkagemr.cash_ac_no==[%s]  ",__FILE__,__LINE__, hv_chkagemr.pay_ac_no,hv_chkagemr.cash_ac_no); 
            vtcp_log("[%s][%d] hv_zf.pay_ac_no==[%s]  hv_zf.cash_ac_no==[%s]  ",__FILE__,__LINE__, hv_zf.pay_ac_no,hv_zf.cash_ac_no); 

              if( iInsHv_zfByHv_chkagemr(&hv_zf,&hv_chkagemr ))
            {
              sprintf(acErrMsg,"iInsHv_zfByHv_chkagemr() ����[%d]",ret);
              WRITEMSG
             return -1;
            }
            vtcp_log("[%s][%d]  ����ɹ�!  ",__FILE__,__LINE__);  
        }
        else /* ���� */
        {
              vtcp_log("[%s][%d]  hv_zf.hv_sts=====[%s]  ",__FILE__,__LINE__,hv_zf.hv_sts);  
              
            vtcp_log("[%s][%d] hv_chkagemr.pay_ac_no==[%s]  hv_chkagemr.cash_ac_no==[%s]  ",__FILE__,__LINE__, hv_chkagemr.pay_ac_no,hv_chkagemr.cash_ac_no); 
            vtcp_log("[%s][%d] hv_zf.pay_ac_no==[%s]  hv_zf.cash_ac_no==[%s]  ",__FILE__,__LINE__, hv_zf.pay_ac_no,hv_zf.cash_ac_no); 

            ret = Hv_zf_Upd_Upd(hv_zf,g_pub_tx.reply);
            if(ret)
            {
                   vtcp_log("[%s][%d] Hv_zf_Upd_Upd() ����!",__FILE__,__LINE__);
                        sprintf(acErrMsg,"���ݿ�����쳣,����Ƽ�����ϵ![%d]",ret);
                        WRITEMSG
                          return -1;
            }             
            vtcp_log("[%s][%d]  �޸ı�־�ɹ�!  ",__FILE__,__LINE__);             
        }    

        Hv_zf_Clo_Upd(); 
        *iHandled = 1;   /* ������һ����¼  !! */

        /* ��Ʊ��DHCC���� ��Ҫ���»�Ʊ��״̬ (CMT 121 122 123 124) */
        if(iCmtno==121 || iCmtno==122 || iCmtno==123 || iCmtno==124 )
        {  
              vtcp_log("[%s,%d] ��Ʊ��DHCC���� ��Ҫ���»�Ʊ��״̬ !",__FILE__,__LINE__);    
              
              /* hv_chkagemr.other_amt ʵ�ʽ�����  hv_chkagemr.tx_amt ʣ���� */
              if(hv_zf.lw_ind[0] == '1')   /* ���� */
              {
                    ret = iHv_SZ_ChangeHPStat(hv_chkagemr.or_br_no,hv_chkagemr.p_num, hv_chkagemr.other_amt, hv_chkagemr.tx_amt, iCmtno);
                   if(ret)
                   {
                             vtcp_log("%s,%d ���»�Ʊ��Ϣ����� ret=[%d]",__FILE__,__LINE__,ret);
                             vtcp_log(acErrMsg,"���»�Ʊ��Ϣ�����");
                             WRITEMSG
                             return ret;
                   }                      
              }
              else  /* ���� */
              {
                    ret = iHv_SZ_ChangeHPStat(hv_chkagemr.acbrno,hv_chkagemr.p_num, hv_chkagemr.other_amt, hv_chkagemr.tx_amt, iCmtno);
                   if(ret)
                   {
                             vtcp_log("%s,%d ���»�Ʊ��Ϣ����� ret=[%d]",__FILE__,__LINE__,ret);
                             vtcp_log(acErrMsg,"���»�Ʊ��Ϣ�����");
                             WRITEMSG
                             return ret;
                   }     
              }
               vtcp_log("[%s,%d] ���»�Ʊ״̬�ɹ�!  ",__FILE__,__LINE__); 
        }      

        /***********   �Ǽǲ���     ***************/                           
        ret = iInsertHv_chkerror(&hv_zf, &hv_chkagemr);
        if(ret)
        {
            vtcp_log("[%s][%d]���ݿ�����쳣,����Ƽ�����ϵ!",__FILE__,__LINE__);
            sprintf(acErrMsg,"���ݿ�����쳣,����Ƽ�����ϵ![%d]",ret);
            WRITEMSG
            return ret;
        }
        vtcp_log("[%s][%d]  �Ǽǲ���ɹ� ! ",__FILE__,__LINE__);
        if(iCzBankFlag == 1 &&  *iHandled == 1)
        {
            *iHandled = 0;
            vtcp_log("%s,%d �Ǵ����������������ֻ���Ǽǣ�������������������һ��ѭ��",__FILE__,__LINE__);
            continue;
        }
        break;     
   } 

   Hv_chkagemr_Clo_Upd();
    vtcp_log("[%s][%d] ***** Hv_chkagemr_Clo_Upd() �ɹ� !  ***** ",__FILE__,__LINE__);
    vtcp_log("[%s][%d] ***** Hv_chkagemr_Clo_Upd() �ɹ� !  ***** ",__FILE__,__LINE__);      
   
      return 0;    
}


/********************************************************
* �� �� ��:  iSet8583_For_SpecialCMT()
* ���������� Ϊ��������������8583�� (CMT121 123 124) 
   DHCC�� CCPCû��  ���������     
   CCPC�� DHCCû��  ���������                   
  
* ������ ZFflg = 1   ����������
         ZFflg = -1  ����������    
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

        if((iCmtno == 121) && (hv_zf->lw_ind[0] == '1') )   /* ���� ��Ʊ�ʽ��ƴ�*/
        {
            /*   �裺 2440402 �ʻ�           |0400 ǩ�����   */
            /*   ���� 41102                  |0390            */
            /*   �裺 41102                  |0390            */
            /*   ���� 110 �ʻ�               |0400                */
            vtcp_log("[%s][%d] ====== �ñʽ��������ʣ����¿�ʼ���� !! iCmtno=[%d] ====== ",__FILE__,__LINE__,iCmtno);

            GetBr_noByOr_br_no(hv_zf->pay_opn_br_no, cOpenbr);  /* �����˿�����  */
            vtcp_log("%s,%d  cOpenbr======[%s]",__FILE__,__LINE__,cOpenbr);  
                
               /*************    ���ʼ��   ****************/
              /** �жϽ��ջ����Ƿ���� */  
                  memset(cKinbr, 0 , sizeof(cKinbr));
              MEMCPY_DEBUG(cKinbr,hv_zf->br_no,BRNO_LEN); 
            memset(cStat, 0, sizeof(cStat));        
            memset(cTmpStat, 0, sizeof(cTmpStat));                
              set_zd_data("0980","9");     /*  */
              
               ret = iHvHang_Check_Brno(cOpenbr, cKinbr);  
            if(ret)
            {
               sprintf(acErrMsg,"���ݿ����ʧ��[%d]",g_reply_int);
               WRITEMSG
               return -1;
            }               
            get_zd_data(DC_TX_BR_NO,cKinbr);   
            vtcp_log("%s,%d �õ��Ľ��׻�����[%s]",__FILE__,__LINE__,cKinbr);
            iGetReceTel(cKinbr);
            get_zd_data("0980",cStat);
              vtcp_log("%s,%d �õ�cStat====[%s]",__FILE__,__LINE__,cStat);
              if(ZFflg == 1) 
              {
                    hv_zf->hv_sts[0] = cStat[0];  /* ���������Ϊ���ʣ������ﲻ�ı�״̬ */
                    hv_zf->beg_sts[0] = hv_zf->hv_sts[0]; 
              }
            
                /* ���� 41101                   */
                /* �裺 41101                   */                                      
            set_zd_data("0640",HVWZGD); /* ������ʹ��ɿ�Ŀ */                     
            set_zd_data("0210","01");                                             
            set_zd_data("0510",hv_zf->cmtno);  
                        
            /* �������ʵ�2440402��2440401�ʺź���һ��Ϊ0 ���������*/ 
            memset(cAc_no, 0 , sizeof(cAc_no));
            ret=pub_base_GetParm_Str(cKinbr,PARM_HPQF,cAc_no);
            if(ret)
            {
                vtcp_log("%s,%d û���ҵ����׻���ָ���Ļ�Ʊǩ����",__FILE__,__LINE__);
                return -1;
            }
            vtcp_log("[%s][%d] PARM_HPQF  cAc_no===[%s]  ",__FILE__,__LINE__,cAc_no);  
            set_zd_data("0320",cAc_no);
            memset(cAc_no, 0 , sizeof(cAc_no));
            ret=pub_base_GetParm_Str(cKinbr,PARM_HPYC,cAc_no);
            if(ret)
            {
                vtcp_log("%s,%d û���ҵ����׻���ָ���Ļ�Ʊ�ƴ滧",__FILE__,__LINE__);
                return -1;
            }
            vtcp_log("[%s][%d]  PARM_HPYC  cAc_no===[%s]  ",__FILE__,__LINE__,cAc_no);  
            set_zd_data("0330",cAc_no);
               set_zd_double("0420",0.0 );  /* ���ﲻ��2440401��2440402��2440402ͨ��0300�� */   
      
            /* �裺 2440402 �ʻ�            */                     
            set_zd_data("0660","1");  /* ���ÿͻ��ʷ�������־ 1-�裻 **/               
              set_zd_data("0680","0");  /* �ʻ����� */
               set_zd_data("0300", cAc_no );  /* 2440402 PARM_HPYC �ʻ� */                   
                 
                /* ���� 110�ʻ�                    */     
            memset(cAc_no, 0 , sizeof(cAc_no));                                      
            ret=pub_base_GetParm_Str(cKinbr,PARM_HVQS,cAc_no);
            if(ret)
            {
                    vtcp_log("%s,%d ϵͳ���ô��󣬵õ�����%s��110�ʺŴ���[%d]",__FILE__,__LINE__,ret);
                    return -1;
            }
            vtcp_log("[%s][%d]  PARM_HVQS cAc_no===[%s] ",__FILE__,__LINE__,cAc_no);                                         
            set_zd_data("1201", cAc_no);
            set_zd_data("0720","2");  /* ����110 ��������־  2-�� */ 

            set_zd_double("0390",hv_zf->tx_amt * ZFflg);  
            set_zd_double("0400",hv_zf->tx_amt * ZFflg );             
           /* set_zd_data("0070",hv_zf->tel);  */ 
             
        } else if( (iCmtno == 123) && (hv_zf->lw_ind[0] == '2')) /*���� �����ʽ𻮻�*/
        {
               /*   �裺 110 �ʻ�                 |0400 δ�ý��    */
               /*   ���� 41102                    |0390             */
               /*   �裺 41102                    |0390             */
               /*   ���� �ͻ� / �����ʺ�          |0400             */
               /*   �裺 2440401                  |0420 ǩ�����    */
               /*   ���� 2440402                  |0420                  */
            vtcp_log("[%s][%d] ====== �ñʽ��������ʣ����¿�ʼ���� !! iCmtno=[%d] ====== ",__FILE__,__LINE__,iCmtno);
            GetBr_noByOr_br_no(hv_zf->pay_opn_br_no, cOpenbr);  /* �����˿�����  */
            vtcp_log("%s,%d  cOpenbr======[%s]",__FILE__,__LINE__,cOpenbr); 
               /*   ���� �ͻ� / �����ʺ�          |0400             */
               
               /*************   ���ʼ��  *******************/    
               /*********** ��Ҫ�ǻ������ʺż���״̬�ļ��   ******/
                  memset(cAc_no, 0 , sizeof(cAc_no));
                  memset(cKinbr, 0 , sizeof(cKinbr));
                  MEMCPY_DEBUG(cKinbr,hv_zf->br_no,BRNO_LEN);
            memset(cStat, 0, sizeof(cStat));        
            memset(cTmpStat, 0, sizeof(cTmpStat));                
              set_zd_data("0980","9");     /*  */

            /* 123 124 �ø������ʺ����� 
            ret = iHvHang_Check_Acno(hv_zf->cash_ac_no,hv_zf->cash_name,cKinbr);  ���ú� 0030 */
            vtcp_log("[%s][%d]  hv_zf->pay_ac_no==[%s] ",__FILE__,__LINE__,hv_zf->pay_ac_no); 
            vtcp_log("[%s][%d]  hv_zf->pay_name==[%s] ",__FILE__,__LINE__,hv_zf->pay_name); 
      
            ret = iHvHang_Check_Acno(hv_zf->pay_ac_no,hv_zf->pay_name,cKinbr);  /* ���ú� 0300 */
              if(ret)
              {
                     vtcp_log("%s,%d iHvHang_Check_Acno()  ���� ",__FILE__,__LINE__);
                 sprintf(acErrMsg," iHvHang_Check_Acno()  ���� !! ");
                 WRITEMSG               
                     return ret;
              }
              
            get_zd_data(DC_TX_BR_NO,cKinbr);   
            vtcp_log("%s,%d �õ��Ľ��׻�����[%s]",__FILE__,__LINE__,cKinbr);
            iGetReceTel(cKinbr);
            get_zd_data("0980",cStat);
              vtcp_log("%s,%d �õ�cStat====[%s]",__FILE__,__LINE__,cStat);
              if(ZFflg == 1) 
              {
                   hv_zf->hv_sts[0] = cStat[0];  /* ���������Ϊ���ʣ������ﲻ�ı�״̬ */
                   hv_zf->beg_sts[0] = hv_zf->hv_sts[0]; 
              }
              
                            
                /* �裺 �ͻ� (�����ʺ�)         */                     
            set_zd_double("0390",hv_zf->tx_amt * ZFflg);  /*  δ�ý��*/
            set_zd_double("0400",hv_zf->tx_amt * ZFflg);  /*  δ�ý��*/
               vtcp_log("%s,%d �õ��� 0420 ǩ����� tx_amt ==== [%lf]",__FILE__,__LINE__,hv_zf->tx_amt*ZFflg);    
            
            set_zd_data("0660","2");  /* ���ÿͻ��ʷ�������־ 1-�裻2-�� **/
            /* set_zd_data("0070",hv_zf->tel);  */     
                      
                /* ���� 41201                   */
                /* �裺 41201                   */                                      
            set_zd_data("0640",HVLZGD); /* ������ʹ��ɿ�Ŀ */                     
            set_zd_data("0210","01");                                             
            set_zd_data("0510",hv_zf->cmtno);  
                 
                /* ���� 110�ʻ�                    */                           
            memset(cAc_no, 0 , sizeof(cAc_no));                
            ret=pub_base_GetParm_Str(cKinbr,PARM_HVQS,cAc_no);
            if(ret)
            {
                    vtcp_log("%s,%d ϵͳ���ô��󣬵õ�����%s��110�ʺŴ���[%d]",__FILE__,__LINE__,ret);
                    return -1;
            }
            vtcp_log("[%s][%d]  PARM_HVQS cAc_no===[%s] ",__FILE__,__LINE__,cAc_no);                                         
            set_zd_data("1201", cAc_no);            
            set_zd_data("0720","1");  /* ����110 ��������־  1-�� */ 
                 
                                           
            /* �������ʵ�2440402��2440401�ʺźͽ��  */ 
            memset(cAc_no, 0 , sizeof(cAc_no));
            ret=pub_base_GetParm_Str(cKinbr,PARM_HPQF,cAc_no);
            if(ret)
            {
                vtcp_log("%s,%d û���ҵ����׻���ָ���Ļ�Ʊǩ����",__FILE__,__LINE__);
                return -1;
            }
            vtcp_log("[%s][%d]   PARM_HPQF  cAc_no===[%s]  ",__FILE__,__LINE__,cAc_no);  
            set_zd_data("0320",cAc_no);
            memset(cAc_no, 0 , sizeof(cAc_no));
            ret=pub_base_GetParm_Str(cKinbr,PARM_HPYC,cAc_no);
            if(ret)
            {
                vtcp_log("%s,%d û���ҵ����׻���ָ���Ļ�Ʊ�ƴ滧",__FILE__,__LINE__);
                return -1;
            }
            vtcp_log("[%s][%d]  PARM_HPYC  cAc_no===[%s]  ",__FILE__,__LINE__,cAc_no);  
            set_zd_data("0330",cAc_no);
               set_zd_double("0420",dTotal_amt * ZFflg);  /*  ǩ�����*/  
               vtcp_log("%s,%d �õ��� 0420 ǩ����� dTotal_amt ==== [%lf]",__FILE__,__LINE__,dTotal_amt*ZFflg);    
        } else if( (iCmtno == 124) && (hv_zf->lw_ind[0] == '2'))  /*���� ��Ʊ�ʽ�δ���˻�*/
        {
             /*     �裺 110 �ʻ�                |0400 ǩ�����  */
             /*     ���� 41102                   |0390           */
             /*     �裺 41102                   |0390           */
             /*   1 ��Ʊ�������ڣ�                               */
             /*     ���� 2440403 �ʻ�            |0400           */
             /*   2 ��Ʊ����δ���ڣ�                             */
             /*     ���� 2440402                 |0420           */
             /*     �裺 2440401                 |0420           */
             /*     ���� �ͻ� �����ʺ�           |0400           */           

            vtcp_log("[%s][%d] ====== �ñʽ��������ʣ����¿�ʼ���� !! iCmtno=[%d] ====== ",__FILE__,__LINE__,iCmtno);
            GetBr_noByOr_br_no(hv_zf->pay_opn_br_no, cOpenbr);  /* �����˿�����  */
            vtcp_log("%s,%d  cOpenbr======[%s]",__FILE__,__LINE__,cOpenbr); 
            
               /*************   ���ʼ��  *******************/    
               /*********** ��Ҫ�ǻ������ʺż���״̬�ļ��   ******/
                  memset(cAc_no, 0 , sizeof(cAc_no));
                  memset(cKinbr, 0 , sizeof(cKinbr));
                  MEMCPY_DEBUG(cKinbr,hv_zf->br_no,BRNO_LEN);
                        
            if( iCheckYQ(p_num, p_date) ) /*   1 ��Ʊ�������ڣ�  */
            {/*     ���� 2440403 �ʻ�            |0400           */  
                
                memset(cStat, 0, sizeof(cStat));        
                memset(cTmpStat, 0, sizeof(cTmpStat));                
                  set_zd_data("0980","9");     /*  */
                  
                   ret = iHvHang_Check_Brno(cOpenbr,cKinbr);  
                if(ret)
                {
                   sprintf(acErrMsg,"���ݿ����ʧ��[%d]",g_reply_int);
                   WRITEMSG
                   return -1;
                } 
                get_zd_data(DC_TX_BR_NO,cKinbr);   
                vtcp_log("%s,%d �õ��Ľ��׻�����[%s]",__FILE__,__LINE__,cKinbr);
                iGetReceTel(cKinbr);
                get_zd_data("0980",cStat);
                  vtcp_log("%s,%d �õ�cStat====[%s]",__FILE__,__LINE__,cStat);
                  if(ZFflg == 1)  /* ֻ���������  ���������Ϊ���ʣ������ﲻ�ı�״̬ */
                  {
                       hv_zf->hv_sts[0] = cStat[0]; 
                       hv_zf->beg_sts[0] = hv_zf->hv_sts[0]; 
                  }
                  
                 set_zd_data("0680","6");/* �����ʻ����� */
                  
                 memset(cAc_no, 0 , sizeof(cAc_no));
                 ret=pub_base_GetParm_Str(cKinbr,PARM_HPTH,cAc_no); /** 2440403������Ʊ�����˻��ʺ� */
                 if(ret)
                 {
                       vtcp_log("%s,%d û���ҵ����׻���ָ���Ļ�Ʊ�ƴ滧",__FILE__,__LINE__);
                       return -1;
                 }

                 vtcp_log("[%s][%d]   PARM_HPTH cAc_no===[%s]  ",__FILE__,__LINE__,cAc_no);  
                 set_zd_data("0300",cAc_no);                
                    /* set_zd_double("0420", 0.0);   ����2440402��2440401  */ 

                 set_zd_data("0660","2");   /* ���ý����־ 2-�� **/                     
            }
            else   /*   2 ��Ʊ����δ���ڣ�   */
            {
                 /*     ���� 2440402                 |0420           */
                 /*     �裺 2440401                 |0420           */             
                 /*     ���� �ͻ�/�����ʺ�           |0400           */
                vtcp_log("[%s][%d] cKinbr===[%s] hv_zf->cash_ac_no===[%s] ",__FILE__,__LINE__,cKinbr,hv_zf->cash_ac_no);                                         
                vtcp_log("[%s][%d] cKinbr===[%s] hv_zf->cash_name ===[%s] ",__FILE__,__LINE__,hv_zf->cash_name);      

                vtcp_log("[%s][%d] cKinbr===[%s] hv_zf->pay_ac_no===[%s] ",__FILE__,__LINE__,cKinbr,hv_zf->pay_ac_no);                                         
                vtcp_log("[%s][%d] cKinbr===[%s] hv_zf->pay_name ===[%s] ",__FILE__,__LINE__,hv_zf->pay_name);      


                memset(cStat, 0, sizeof(cStat));        
                memset(cTmpStat, 0, sizeof(cTmpStat));                
                  set_zd_data("0980","9");     /*  */

                  /* 123 124 �ø������ʺ����� 
                ret = iHvHang_Check_Acno(hv_zf->cash_ac_no,hv_zf->cash_name,cKinbr);  ���ú� 0030 */
                   ret = iHvHang_Check_Acno(hv_zf->pay_ac_no,hv_zf->pay_name,cKinbr);  /* ���ú� 0030 */
                   if(ret)
                   {
                          vtcp_log("%s,%d iHvHang_Check_Acno()  ���� ",__FILE__,__LINE__);
                         sprintf(acErrMsg," iHvHang_Check_Acno()  ���� !! ");
                         WRITEMSG 
                          return ret;
                   }
                get_zd_data(DC_TX_BR_NO,cKinbr);   
                vtcp_log("%s,%d �õ��Ľ��׻�����[%s]",__FILE__,__LINE__,cKinbr);
                iGetReceTel(cKinbr);
                get_zd_data("0980",cStat);
                  vtcp_log("%s,%d �õ�cStat====[%s]",__FILE__,__LINE__,cStat);
                  if(ZFflg == 1) /* ֻ���������  ���������Ϊ���ʣ������ﲻ�ı�״̬ */
                  {
                       hv_zf->hv_sts[0] = cStat[0];   
                       hv_zf->beg_sts[0] = hv_zf->hv_sts[0]; 
                  }
                               
                    /* ���� �ͻ� (�����ʺ�)         */                     
                set_zd_double("0390",hv_zf->tx_amt * ZFflg);  /*  δ�ý��*/
                set_zd_double("0400",hv_zf->tx_amt * ZFflg);  /*  δ�ý��*/
                   set_zd_double("0420",hv_zf->tx_amt * ZFflg);  /* ��2440402��2440401  */
                set_zd_data("0660","2");   /* ���ý����־ 2-�� **/                        
            }
               set_zd_double("0420",hv_zf->tx_amt * ZFflg);  /* �����Ƿ����ڣ����� 2440401 2440402 */            
            set_zd_double("0390",hv_zf->tx_amt * ZFflg);  /*  δ�ý��*/
            set_zd_double("0400",hv_zf->tx_amt * ZFflg);  /*  δ�ý��*/ 
            
                /* ���� 41201                   */
                /* �裺 41201                   */                                      
            set_zd_data("0640",HVLZGD); /* ������ʹ��ɿ�Ŀ */                     
            set_zd_data("0210","01");                                             
            set_zd_data("0510",hv_zf->cmtno);  
                 
                /* �裺 110�ʻ�                    */                           
            memset(cAc_no, 0 , sizeof(cAc_no));
            /* ret=pub_base_GetParm_Str(hv_zf->br_no,PARM_HVQS,cAc_no);  2006-11-13 19:49 */
            ret=pub_base_GetParm_Str(cKinbr, PARM_HVQS,cAc_no);
            if(ret)
            {
                    vtcp_log("%s,%d ϵͳ���ô��󣬵õ�����%s��110�ʺŴ���[%d]",__FILE__,__LINE__,ret);
                    return -1;
            }
            vtcp_log("[%s][%d]  PARM_HVQS  cAc_no===[%s]  ",__FILE__,__LINE__,cAc_no);              
            set_zd_data("1201",cAc_no);
            set_zd_data("0720","1");  /* ����110 ��������־  1-�� */ 
                                          
            /* �������ʵ�2440402��2440401�ʺź���*/ 
            memset(cAc_no, 0 , sizeof(cAc_no));
            ret=pub_base_GetParm_Str(cKinbr, PARM_HPQF,cAc_no);
            if(ret)
            {
                vtcp_log("%s,%d û���ҵ����׻���ָ���Ļ�Ʊǩ����",__FILE__,__LINE__);
                return -1;
            }
            vtcp_log("[%s][%d] PARM_HPQF  cAc_no===[%s]  ",__FILE__,__LINE__,cAc_no);  
            set_zd_data("0320",cAc_no);
            memset(cAc_no, 0 , sizeof(cAc_no));
            ret=pub_base_GetParm_Str(cKinbr, PARM_HPYC,cAc_no);
            if(ret)
            {
                vtcp_log("%s,%d û���ҵ����׻���ָ���Ļ�Ʊ�ƴ滧",__FILE__,__LINE__);
                return -1;
            }
            vtcp_log("[%s][%d]  PARM_HPYC cAc_no===[%s]  ",__FILE__,__LINE__,cAc_no);  
            set_zd_data("0330",cAc_no);
        }
        else
        {
          sprintf(acErrMsg," �ñ��Ĳ������ﴦ�� !! ");
          WRITEMSG
          return -1;
        }
        
      return 0;
}


int iCheckYQ(char* p_num,long p_date)
{
    
      int iYqFlag=0;/* 0δ����,1���� */
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
          /* ��Ʊ�����ڵ�����������ڴ��� */
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
* �� �� ��:  iSet8583_For_CommonCMT()
* ���������� Ϊ��ͨ����������8583�� (CMT 100 101 102 103 105 108 122) 
   DHCC�� CCPCû��  ���������     
   CCPC�� DHCCû��  ���������                   
   ���ʣ� �� 110   �� �ͻ�
   ���ʣ� �� �ͻ�  �� 110    
* ������ ZFflg = 1  ����������
         ZFflg = -1  ����������    
********************************************************/
int iSet8583_For_CommonCMT(struct hv_zf_c* hv_zf, int ZFflg)
{
    int ret;
      char cAcno110[20];
    char cStat[2];
    char cTmpStat[2]; 
    char cCJ_Acno[20];        /* ����ʺ� */  
      
      memset(cAcno110,0,sizeof(cAcno110));
      memset(cCJ_Acno,0,sizeof(cCJ_Acno));

    vtcp_log("[%s][%d] ��ͨ�� hv_zf->lw_ind=[%s] ",__FILE__,__LINE__,hv_zf->lw_ind);
       if(hv_zf->lw_ind[0] == '1')   /* ���� */
       {
            /* �裺 �ͻ� (�����ʺ�)         */
            /* ���� 41101                   */
            /* �裺 41101                   */
            /* ���� 110�ʻ�                   */     

             /*************   ���ʼ��  *******************/    
             /*********** ��Ҫ�ǻ������ʺż���״̬�ļ��   ******/
             char cKinbr[BRNO_LEN+1];
             char cOpenbr[BRNO_LEN+1];
                char cAc_no[20];
                memset(cAc_no, 0 , sizeof(cAc_no));
                memset(cKinbr, 0 , sizeof(cKinbr));
                memset(cOpenbr, 0 , sizeof(cOpenbr));
                
          memset(cStat, 0, sizeof(cStat));        
          memset(cTmpStat, 0, sizeof(cTmpStat));                
            set_zd_data("0980","9");     /*  */
          vtcp_log("[%s][%d] ====== �ñʽ��������ʣ����¿�ʼ���� !! ====== ",__FILE__,__LINE__);
            
            MEMCPY_DEBUG(cKinbr, hv_zf->br_no, sizeof(cKinbr)-1);
            GetBr_noByOr_br_no(hv_zf->pay_opn_br_no, cOpenbr);  /* �����˿�����  */
            
          vtcp_log("[%s][%d] ��ͨ�� hv_zf->cmtno=[%s] ",__FILE__,__LINE__,hv_zf->cmtno);      
            if(memcmp(hv_zf->cmtno,"105",sizeof(hv_zf->cmtno)-1)==0)  /* ͬҵ���֧������ */
            { 
                  ret=pub_base_GetParm_Str(cKinbr,PARM_HVCJ,cCJ_Acno); /* ����� 105 ͬҵ���֧�����ģ����������ò���ʺ� */ 
                  if(ret)
                  {
                      vtcp_log("%s,%d �õ�[%s]HVCJ��������ֱ�����ÿ�Ŀ2620401",__FILE__,__LINE__,cKinbr);
                      MEMCPY_DEBUG(cCJ_Acno,"2620401",7);
                  }
               /* �裺 ����ʺ�    */    
                 ret = iHvHang_Check_AcnoNoName(cCJ_Acno, cKinbr); /* ����黧�� */
              if(ret)
                 {
                        vtcp_log("%s,%d iHvHang_Check_AcnoNoName()  ���� ret=[%d]",__FILE__,__LINE__,ret);
                       sprintf(acErrMsg," iHvHang_Check_AcnoNoName()  ���� !! ");
                       WRITEMSG               
                        return -1;
                 }  
            }
            else /* ��ͬҵ���֧������ */
            {
               /* �裺 �ͻ� (�����ʺ�)         */
               
              /* �ֽ��Ŀ������ҵ��Ҫ�ܾ������˿�Ŀ�� */
              if( (strlen(hv_zf->pay_ac_no)==5 && memcmp(hv_zf->pay_ac_no, "10101", 5 )==0) ||
                    (strlen(hv_zf->pay_ac_no)==7 && memcmp(hv_zf->pay_ac_no,"1010100",7 )==0 ) )
              {
                    ret = iHvHang_Check_Brno(cOpenbr, hv_zf->br_no);  /* ���úû��� */
                  if(ret)
                    {
                           vtcp_log("%s,%d iHvHang_Check_Brno()  ���� ret=[%d]",__FILE__,__LINE__,ret);
                            sprintf(acErrMsg," iHvHang_Check_Brno()  ���� !! ");
                            WRITEMSG               
                           return -1;
                    }
                  memset(cKinbr, 0 , sizeof(cKinbr));
                  get_zd_data(DC_TX_BR_NO,cKinbr);   
                  vtcp_log("%s,%d �õ��Ľ��׻�����[%s]",__FILE__,__LINE__,cKinbr);
                            
                  memset(cAc_no, 0 , sizeof(cAc_no));
                         /* �ֽ��Ŀ������ҵ��Ҫ�ܾ������˿�Ŀ�� */
                        ret=pub_base_GetParm_Str(cKinbr,PARM_HVGZ,cAc_no);
                        if(ret)
                        {
                            vtcp_log("%s,%d ϵͳ���ô��󣬵õ�����%s�Ĺ����ʺŴ���[%d]",__FILE__,__LINE__,ret);
                            strncpy(acErrMsg,"�õ������������˺Ŵ���",50);
                            set_zd_data("0130",acErrMsg);
                            return ret;
                        }  
                        set_zd_data("0300",cAc_no);      /* ���ú��˺� */ 
                        set_zd_data("0980","5");         /* �ܾ� */           

              }else
              {
                    ret = iHvHang_Check_Acno(hv_zf->pay_ac_no, hv_zf->pay_name, cKinbr); /* ���ú� �˺� ���� */
                  if(ret)
                    {
                           vtcp_log("%s,%d iHvHang_Check_Acno()  ���� ret=[%d]",__FILE__,__LINE__,ret);
                          sprintf(acErrMsg," iHvHang_Check_Acno()  ���� !! ");
                          WRITEMSG               
                           return -1;
                    }  
              }    
            }

          get_zd_data(DC_TX_BR_NO,cKinbr);   
          vtcp_log("%s,%d �õ��Ľ��׻�����[%s]",__FILE__,__LINE__,cKinbr);
          iGetReceTel(cKinbr);
          get_zd_data("0980",cStat);
            vtcp_log("%s,%d �õ�cStat====[%s]",__FILE__,__LINE__,cStat);
            if(ZFflg == 1) 
            {
                 hv_zf->hv_sts[0] = cStat[0];  /* ���������Ϊ���ʣ������ﲻ�ı�״̬ */
                 hv_zf->beg_sts[0] = hv_zf->hv_sts[0]; 
            }

         /* set_zd_data("0070",hv_zf->tel);  */ 
                       
              /* �裺 �ͻ� (�����ʺ�)         */                     
          set_zd_double("0390",hv_zf->tx_amt * ZFflg);  /* ����� */
          set_zd_double("0400",hv_zf->tx_amt * ZFflg);  /* ����� */
          set_zd_data("0660","1");  /* ���ÿͻ��ʷ�������־ 1-�裻2-�� **/
              /* ���� 41101                   */
              /* �裺 41101                   */                                      
          set_zd_data("0640",HVWZGD); /* ������ʹ��ɿ�Ŀ */                     
          set_zd_data("0210","01");                                             
          set_zd_data("0510",hv_zf->cmtno);  
               
              /* ���� 110�ʻ�                    */                           
          vtcp_log("[%s][%d] hv_zf->br_no===[%s] hv_zf->pay_ac_no===[%s] ",__FILE__,__LINE__,hv_zf->br_no,hv_zf->pay_ac_no);                                         
          memset(cAc_no, 0 , sizeof(cAc_no));
          /*ret=pub_base_GetParm_Str(cKinbr,PARM_HVQS,hv_zf->pay_ac_no);  2006-11-10 18:09*/
          ret=pub_base_GetParm_Str(cKinbr,PARM_HVQS,cAc_no); 
          if(ret)
          {
                  vtcp_log("%s,%d ϵͳ���ô��󣬵õ�����%s��110�ʺŴ���[%d]",__FILE__,__LINE__,ret);
                  return -1;
          }
          set_zd_data("1201",cAc_no);
          set_zd_data("0720","2");  /* ����110 ��������־  2-�� */ 
               
          /* J016 J017������  ����� 0           */                                                      
          /* �������ʵ�2440402��2440401�ʺź���һ��Ϊ0 ��������� */
          memset(cAc_no, 0 , sizeof(cAc_no));
          vtcp_log("%s,%d �õ��Ľ��׻�����[%s]",__FILE__,__LINE__,cKinbr);          
          ret=pub_base_GetParm_Str(cKinbr,PARM_HPQF,cAc_no);
          if(ret)
          {
              vtcp_log("%s,%d û���ҵ����׻���ָ���Ļ�Ʊǩ����",__FILE__,__LINE__);
              return -1;
          }
           vtcp_log("[%s][%d]   cAc_no===[%s]  ",__FILE__,__LINE__,cAc_no);  
          set_zd_data("0320",cAc_no);
          memset(cAc_no, 0 , sizeof(cAc_no));
          ret=pub_base_GetParm_Str(cKinbr,PARM_HPYC,cAc_no);
          if(ret)
          {
              vtcp_log("%s,%d û���ҵ����׻���ָ���Ļ�Ʊ�ƴ滧",__FILE__,__LINE__);
              return -1;
          }
          vtcp_log("[%s][%d]   cAc_no===[%s]  ",__FILE__,__LINE__,cAc_no);  
          set_zd_data("0330",cAc_no);
             set_zd_double("0420",0.0);                    
                             
       }
       else if(hv_zf->lw_ind[0] == '2') /*����*/
       {
          /* �裺 110 �ʻ�               */
          /* ���� 41102                  */
          /* �裺 41102                  */
          /* ���� �ͻ� �����ʺ�          */   

        /*************   ���ʼ��  *******************/    
        /*********** ��Ҫ�ǻ������ʺż���״̬�ļ��   ******/
          char cKinbr[BRNO_LEN+1];
              char cAc_no[20];
              memset(cAc_no, 0 , sizeof(cAc_no));
              memset(cKinbr, 0 , sizeof(cKinbr));
        MEMCPY_DEBUG(cKinbr,hv_zf->br_no,BRNO_LEN); 

        memset(cStat, 0, sizeof(cStat));        
        memset(cTmpStat, 0, sizeof(cTmpStat));                
           set_zd_data("0980","9");     /*  */
        vtcp_log("[%s][%d] ====== �ñʽ��������ʣ����¿�ʼ����8583 !! ====== ",__FILE__,__LINE__);
        

        vtcp_log("[%s][%d] ��ͨ�� hv_zf->cmtno=[%s] ",__FILE__,__LINE__,hv_zf->cmtno);      
          if(memcmp(hv_zf->cmtno,"105",sizeof(hv_zf->cmtno)-1)==0)  /* ͬҵ���֧������ */
          { 
                ret=pub_base_GetParm_Str(cKinbr,PARM_HVCJ,cCJ_Acno); /* ����� 105 ͬҵ���֧�����ģ����������ò���ʺ� */ 
                if(ret)
                {
                    vtcp_log("%s,%d �õ�[%s]HVCJ��������ֱ�����ÿ�Ŀ2620401",__FILE__,__LINE__,cKinbr);
                    MEMCPY_DEBUG(cCJ_Acno,"2620401",7);
                }
             /* �裺 ����ʺ�    */    
               ret = iHvHang_Check_AcnoNoName(cCJ_Acno, cKinbr); /* ����黧�� ���ú� 0030 */
            if(ret)
               {
                      vtcp_log("%s,%d iHvHang_Check_AcnoNoName()  ���� ret=[%d]",__FILE__,__LINE__,ret);
                     sprintf(acErrMsg," iHvHang_Check_AcnoNoName()  ���� !! ");
                     WRITEMSG               
                      return -1;
               }  
          }
          else /* ��ͬҵ���֧������ */
          {
             /* ���� �ͻ� �����ʺ�          */     
              ret = iHvHang_Check_Acno(hv_zf->cash_ac_no,hv_zf->cash_name,cKinbr); /* ���ú� 0030 */
           if(ret)
              {
                     vtcp_log("%s,%d iHvHang_Check_Acno()  ���� ",__FILE__,__LINE__);
                    sprintf(acErrMsg," iHvHang_Check_Acno()  ���� !! ");
                    WRITEMSG               
                     return ret;
              }
           }
           
        get_zd_data(DC_TX_BR_NO,cKinbr);   
        vtcp_log("%s,%d �õ��Ľ��׻�����[%s]",__FILE__,__LINE__,cKinbr);
        iGetReceTel(cKinbr);
        get_zd_data("0980",cStat);
          vtcp_log("%s,%d �õ�cStat====[%s]",__FILE__,__LINE__,cStat);
          if(ZFflg == 1) 
          {
               hv_zf->hv_sts[0] = cStat[0];  /* ���������Ϊ���ʣ������ﲻ�ı�״̬ */
               hv_zf->beg_sts[0] = hv_zf->hv_sts[0]; 
          }

                     
        /* set_zd_data("0070",hv_zf->tel);  */ 
        set_zd_data("0660","2");  /* ���ÿͻ��� �����־ 1-�裻2-�� **/        
                                     
         /* �裺 110 �ʻ�               */
        memset(cAcno110, 0 , sizeof(cAcno110)); 
        set_zd_double("0390",hv_zf->tx_amt * ZFflg);  /* ����� */
        set_zd_double("0400",hv_zf->tx_amt * ZFflg);  /* ����� */                    
           set_zd_data("0660","2");
           set_zd_data("0720","1");  /* ����110  ��������־  1-�� */ 
           ret = pub_base_GetParm_Str(cKinbr,PARM_HVQS,cAcno110);
           if(ret)
           {
               vtcp_log("%s,%d û���ҵ����׻���ָ����110��",__FILE__,__LINE__);
               return ret;
           }
           
           vtcp_log("%s,%d �ҵ����׻���ָ����110�� cAcno110=[%s]",__FILE__,__LINE__,cAcno110);                        
           set_zd_data("1201",cAcno110);
           set_zd_data("1204","01");
           set_zd_data("1205","2");
  
           /* ���� 41201                   */
           /* �裺 41201                   */                                      
        set_zd_data("0640",HVLZGD); /* ������ʹ��ɿ�Ŀ */                     
        set_zd_data("0210","01");                                             
        set_zd_data("0510",hv_zf->cmtno);  

        /* J016 J017������  ����� 0           */                                                      
        /* �������ʵ�2440402��2440401�ʺź���һ��Ϊ0 ��������� */
        memset(cAc_no, 0 , sizeof(cAc_no));
        ret=pub_base_GetParm_Str(cKinbr,PARM_HPQF,cAc_no);
        if(ret)
        {
            vtcp_log("%s,%d û���ҵ����׻���ָ���Ļ�Ʊǩ����",__FILE__,__LINE__);
            return -1;
        }
         vtcp_log("[%s][%d]   cAc_no===[%s]  ",__FILE__,__LINE__,cAc_no);  
        set_zd_data("0320",cAc_no);
        memset(cAc_no, 0 , sizeof(cAc_no));
        ret=pub_base_GetParm_Str(cKinbr,PARM_HPYC,cAc_no);
        if(ret)
        {
            vtcp_log("%s,%d û���ҵ����׻���ָ���Ļ�Ʊ�ƴ滧",__FILE__,__LINE__);
            return -1;
        }
        vtcp_log("[%s][%d]   cAc_no===[%s]  ",__FILE__,__LINE__,cAc_no);  
        set_zd_data("0330",cAc_no);
        set_zd_double("0420",0.0);  
                              
       }
       else
       {
       sprintf(acErrMsg," ������ʶ����!! ");
       WRITEMSG
       return -1;
       }

    return 0;    
}



/*  ���� hv_chkagemr ���� hv_zf  */
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
         vtcp_log("[%s][%d] Hv_zf_Ins() ����!",__FILE__,__LINE__);
         sprintf(acErrMsg,"���ݿ�����쳣,����Ƽ�����ϵ![%d]",ret);
         WRITEMSG
            return ret;    
      }
    vtcp_log("[%s][%d] Hv_zf_Ins()  ����ɹ�!",__FILE__,__LINE__);    
      /*    memset(&hv_zfhtr,0,sizeof(hv_zfhtr));
      iCopyHv_zfToHv_zfhtr(&hv_zf,&hv_zfhtr);    */
      ret = Hv_zfhtr_Ins(*(hv_zf), g_pub_tx.reply);
      if(ret)
      {
         vtcp_log("[%s][%d] Hv_zfhtr_Ins() ����!",__FILE__,__LINE__);
         sprintf(acErrMsg,"���ݿ�����쳣,����Ƽ�����ϵ![%d]",ret);
         WRITEMSG
           return ret;    
       }        
     vtcp_log("[%s][%d]  Hv_zfhtr_Ins() ����ɹ�! \n ",__FILE__,__LINE__);    
    return 0;
}

/*  ���� hv_chkagemr ���� hv_zf  */
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
        vtcp_log("[%s][%d] iGetHvOrderno() ����!",__FILE__,__LINE__);
          sprintf(acErrMsg,"���ݿ�����쳣,����Ƽ�����ϵ![%d]",ret);
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
    /* hv_zf->hv_sts[0] = '9';   Ĭ��Ϊ ���˼������㣬���ʼ��ʱ�ٸ���������� */
      /* hv_zf->beg_sts[0] = hv_zf->hv_sts[0];  Ĭ��Ϊ ���˼������㣬���ʼ��ʱ�ٸ���������� */
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

      /* 108 �˻�֧�����ĵ��ʺź��û������⴦�� */
      if( memcmp(hv_zf->cmtno,"108", 3) == 0)
      {
              /* ����ҵ��һ�� ��'�����������'��'�տ��������  ' ������  */
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
    if(hv_zf->lw_ind[0] == '2')  /* ���� */
    {                    
      if( GetBr_noByOr_br_no(hv_zf->ac_br_no,hv_zf->br_no))
      {
            memset(hv_zf->br_no, 0, sizeof(hv_zf->br_no));
          /* MEMCPY_DEBUG(hv_zf->br_no,hv_chkagemr->br_no,sizeof(hv_zf->br_no)-1);     */          
      }
    }
    else if(hv_zf->lw_ind[0] == '1')   /* ���� */
    {
      if( GetBr_noByOr_br_no(hv_zf->or_br_no,hv_zf->br_no))
      {
            memset(hv_zf->br_no, 0, sizeof(hv_zf->br_no));
          /* MEMCPY_DEBUG(hv_zf->br_no,hv_chkagemr->br_no,sizeof(hv_zf->br_no)-1);       */         
      }          
    }else
    {
      sprintf(acErrMsg,"������ʶ��[%s]",hv_zf->lw_ind);
      WRITEMSG
        return ret;
    }
    vtcp_log("[%s][%d] hv_zf->lw_ind=[%s]  ",__FILE__,__LINE__,hv_zf->lw_ind);
   
    vtcp_log("[%s][%d] hv_zf->pay_name==[%.60s] ",__FILE__,__LINE__,hv_zf->pay_name); 
    memset(cTempstr,0,sizeof(cTempstr));
    MEMCPY_DEBUG(cTempstr, hv_zf->pay_name, sizeof(hv_zf->pay_name)-1);  
    dbc_zipspace(cTempstr);  /*ѹ��ȫ�ǿո� 2006-11-7 23:45  */
    MEMCPY_DEBUG(hv_zf->pay_name, cTempstr, sizeof(hv_zf->pay_name)-1); 
    vtcp_log("[%s][%d] hv_zf->pay_name==[%.60s] ",__FILE__,__LINE__,hv_zf->pay_name);

    vtcp_log("[%s][%d] hv_zf->cash_name==[%.60s] ",__FILE__,__LINE__,hv_zf->cash_name); 
    memset(cTempstr,0,sizeof(cTempstr));
    MEMCPY_DEBUG(cTempstr, hv_zf->cash_name, sizeof(hv_zf->cash_name)-1);  
    dbc_zipspace(cTempstr);  /*ѹ��ȫ�ǿո� 2006-11-7 23:45  */
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
      
      /*  ========= �����Ʊ��ĸ����� ======== */      
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
                dbl2str(&hv_chkagemr->other_amt, 0, sizeof(hv_hp123.real_js_amt)-1,0, hv_hp123.real_js_amt); /*ʵ�ʽ�����*/
            MEMCPY_DEBUG(hv_hp123.cash_opn_br_no, hv_chkagemr->cashno, sizeof(hv_hp123.cash_opn_br_no));  /*����Ʊ�˿�����*/
        vtcp_log("[%s][%d]  ========== ", __FILE__,__LINE__);
        vtcp_log("[%s][%d]  ========== ", __FILE__,__LINE__);
            MEMCPY_DEBUG(hv_hp123.last_ac_no, hv_chkagemr->cash_ac_no, sizeof(hv_hp123.last_ac_no));    /*����Ʊ���ʺ�*/              
            MEMCPY_DEBUG(hv_hp123.last_name, hv_chkagemr->cash_name, sizeof(hv_hp123.last_name));        /*����Ʊ������*/
        vtcp_log("[%s][%d]  ========== ", __FILE__,__LINE__);
        vtcp_log("[%s][%d]  ========== ", __FILE__,__LINE__);
            apitoa(hv_chkagemr->pay_date, sizeof(hv_hp123.return_date), hv_hp123.return_date);  /*��ʾ��������*/       
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
      
    vtcp_log("[%s][%d] iInsertHv_chkerror () ��ʼ !!!",__FILE__,__LINE__);

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
         vtcp_log("[%s][%d] Hv_chkerror_Ins() ����!",__FILE__,__LINE__);
         sprintf(acErrMsg,"���ݿ�����쳣,����Ƽ�����ϵ![%d]",ret);
         WRITEMSG
            return ret;    
      }
    vtcp_log("[%s][%d] Hv_chkerror_Ins()  ����ɹ�!",__FILE__,__LINE__);    
      return 0;
}


/*  ��Ʊ�� DHCC���� ��Ҫ���»�Ʊ��״̬   */
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
          /*�õ������� */
          ret=iHvGetAddit(cTx_date,cWt_date,hv_zf_wz->addid,"000",(char*)&hv_hp121,sizeof(hv_hp121));
          if(ret)
          {
              vtcp_log("%s,%d ��ȡ��������� %d",__FILE__,__LINE__,ret);
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
          /*�õ������� */
          ret=iHvGetAddit(cTx_date,cWt_date,hv_zf_wz->addid,"000",(char*)&hv_hp122,sizeof(hv_hp122));
          if(ret)
          {
              vtcp_log("%s,%d ��ȡ��������� %d",__FILE__,__LINE__,ret);
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
          /*�õ������� */
          ret=iHvGetAddit(cTx_date,cWt_date,hv_zf_wz->addid,"000",(char*)&hv_hp123,sizeof(hv_hp123));
          if(ret)
          {
              vtcp_log("%s,%d ��ȡ��������� %d",__FILE__,__LINE__,ret);
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
          /*�õ������� */
          ret=iHvGetAddit(cTx_date,cWt_date,hv_zf_wz->addid,"000",(char*)&hv_hp124,sizeof(hv_hp124));
          if(ret)
          {
              vtcp_log("%s,%d ��ȡ��������� %d",__FILE__,__LINE__,ret);
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
          vtcp_log("[%s,%d] iCmtno ����!!!  iCmtno===[%d]",__FILE__,__LINE__,iCmtno);
          WRITEMSG
          return -1;    
    }

      ret = Hv_poinfo_Dec_Upd(g_pub_tx.reply,"po_no='%s'  and  pay_br_no='%s'",hv_poinfo.po_no,hv_poinfo.pay_br_no);
      if(ret)
      {
          sprintf(acErrMsg,"�����α����!ret=[%d]\n",ret);
          strcpy(g_pub_tx.reply,"P001");
          WRITEMSG
          return -1;;
      }
      memset(&hv_poinfo, 0 , sizeof(hv_poinfo));      
      ret = Hv_poinfo_Fet_Upd(&hv_poinfo,g_pub_tx.reply);
      if(ret&&ret!=100)
      {
          sprintf(acErrMsg,"ȡ�α����!ret=[%d]\n",ret);
          strcpy(g_pub_tx.reply,"P001");
          WRITEMSG
          return -1;
      }
      else if(ret==100)
      {
          sprintf(acErrMsg,"��ѯ�ļ�¼������!ret=[%d]\n",ret);
          WRITEMSG
          strcpy(g_pub_tx.reply,"P001");
          /* return -1;   Remed by ChenMing   2006-11-5 23:43
          ��Ϊ���������Ҳ��������߲��ԣ����ԣ����û�ҵ���Ʊ��¼���ﲻ���� */
      }
      
      cLast_sts = hv_poinfo.po_sts[0];
      
    if( iCmtno==121 )
    {
        hv_poinfo.po_sts[0] = hv_poinfo.l_po_sts[0];   /* �û�ԭ״̬ */
        hv_poinfo.l_po_sts[0] = cLast_sts;
        
    }
    /* else if( iCmtno==122 )
    {
         if( hv_poinfo.po_sts[0] == '4')   ��Ʊ�Ҹ�
         {
                hv_poinfo.po_sts[0] = hv_poinfo.l_po_sts[0];  
         }
    }else if( iCmtno==123 )
    {
         if( hv_poinfo.po_sts[0] == '6')   �����˻� 
         {
                hv_poinfo.po_sts[0] = hv_poinfo.l_po_sts[0];  
         }
    }else if( iCmtno==124 )
    {
         if( hv_poinfo.po_sts[0] == '7')   δ���˻� 
         {
                hv_poinfo.po_sts[0] = hv_poinfo.l_po_sts[0];  
         }
    }*/

      ret = Hv_poinfo_Upd_Upd(hv_poinfo,g_pub_tx.reply);
      if(ret)
      {
          sprintf(acErrMsg,"ִ�к���hv_poinfo_upd_upd����!ret==[%d]\n",ret);
          WRITEMSG
          strcpy(g_pub_tx.reply,"D105");
          return (-1);
      }
      
      Hv_poinfo_Clo_Upd();
      return 0;
}


/* ��Ʊ�� DHCC����  */
/* dCash_amt ʵ�ʽ�����  dRem_amt  ʣ���� */
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
          sprintf(acErrMsg,"�����α����!ret=[%d]\n",ret);
          strcpy(g_pub_tx.reply,"P001");
          WRITEMSG
          return -1;;
      }
      memset(&hv_poinfo, 0 , sizeof(hv_poinfo));      
      ret = Hv_poinfo_Fet_Upd(&hv_poinfo,g_pub_tx.reply);
      if(ret&&ret!=100)
      {
          sprintf(acErrMsg,"ȡ�α����!ret=[%d]\n",ret);
          strcpy(g_pub_tx.reply,"P001");
          WRITEMSG
          return -1;
      }
      else if(ret==100)
      {
          sprintf(acErrMsg,"��ѯ�ļ�¼������!ret=[%d]\n",ret);
          WRITEMSG
          strcpy(g_pub_tx.reply,"P001");
          return 0;
          /* return -1;   Remed by ChenMing   2006-11-5 23:43
          ��Ϊ���������Ҳ��������߲��ԣ����ԣ����û�ҵ���Ʊ��¼���ﲻ���� */
      }
      
    vtcp_log("[%s][%d] hv_poinfo.po_sts========[%s] ",__FILE__,__LINE__,hv_poinfo.po_sts);
    vtcp_log("[%s][%d] hv_poinfo.l_po_sts======[%s] ",__FILE__,__LINE__,hv_poinfo.l_po_sts);
        
    cLast_sts = hv_poinfo.po_sts[0];  /* ��Ʊ����һ��״̬ */
    hv_poinfo.l_tx_date = g_pub_tx.tx_date;  
    hv_poinfo.td_cnt ++;   

    vtcp_log("[%s][%d] iCmtno========[%d] ",__FILE__,__LINE__,iCmtno); 

    /* ���ʣ����ݲ�ͬ������ûػ�Ʊ�ı�־ */
    if( iCmtno==121 )
    {
        /*
        if( hv_poinfo.po_sts[0] == HV_HP_SIGN )  
        {
           hv_poinfo.po_sts[0] =  HV_HP_MV_CENTER; �ʽ��ƴ���� 
           hv_poinfo.l_po_sts[0] = cLast_sts;
        }
        else
        {
            Hv_poinfo_Clo_Upd();  
            sprintf(acErrMsg," ��Ʊ��¼��ǰ״̬���� ! hv_poinfo.po_sts=[%s] ",hv_poinfo.po_sts);
            WRITEMSG
            return -1;
        } 
     */
    }else if( iCmtno==122 )
    {
        if(hv_poinfo.po_sts[0] == HV_HP_JQ) /* 725���յ� ��ȫ����� */
        {
            vtcp_log(" [%s][%d] ��ȫ����� 725���յ� ���ø�����!  ",__FILE__,__LINE__); 
            Hv_poinfo_Clo_Upd();  
            return 0; 
        }
        else if( hv_poinfo.po_sts[0] == HV_HP_ASKCASH || 
                   hv_poinfo.po_sts[0] == HV_HP_UNLOSS ||
                   hv_poinfo.po_sts[0] == HV_HP_MV_CENTER ||
                   hv_poinfo.po_sts[0] == HV_HP_LOSS )  /* ����Ҹ� */  
        {
            hv_poinfo.po_sts[0] = HV_HP_CASH;           /* ��Ʊ�Ҹ� */
            hv_poinfo.l_po_sts[0] = cLast_sts;
        }
        else
        {
            Hv_poinfo_Clo_Upd();  
            sprintf(acErrMsg," ��Ʊ��¼��ǰ״̬���� ! hv_poinfo.po_sts=[%s] ",hv_poinfo.po_sts);
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
        {  123  124  ����˵���ü��״̬��  2006-11-22 17:52  */
           hv_poinfo.po_sts[0] =  HV_HP_PARTBACK;   /* �����˻� */
           hv_poinfo.l_po_sts[0] = cLast_sts;
           hv_poinfo.cash_amt = dCash_amt ;   /* ʵ�ʽ����� */           
           hv_poinfo.rem_amt = dRem_amt;   /* �˻ؽ�� */ 
        /*}
        else
        {
            Hv_poinfo_Clo_Upd();  
            sprintf(acErrMsg," ��Ʊ��¼��ǰ״̬���� ! hv_poinfo.po_sts=[%s] ",hv_poinfo.po_sts);
            WRITEMSG
            return -1;
        } 123  124  ����˵���ü��״̬��  2006-11-22 17:52   */
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
            hv_poinfo.po_sts[0] = HV_HP_OVERDATE_UNTREAD;  /* ����δ���˻� */
            hv_poinfo.l_po_sts[0] = cLast_sts;
         }
         else
         {
            hv_poinfo.po_sts[0] = HV_HP_FULLBACK;  /* δ���˻� */
            hv_poinfo.l_po_sts[0] = cLast_sts;
         }  
    }          
    else
    {
        sprintf(acErrMsg,"iCmtno== [%d] �������ﴦ��! ",iCmtno);
        WRITEMSG
        return -1;
    }     
 
    vtcp_log("[%s][%d] hv_poinfo.po_sts========[%s] ",__FILE__,__LINE__,hv_poinfo.po_sts);
    vtcp_log("[%s][%d] hv_poinfo.l_po_sts======[%s] ",__FILE__,__LINE__,hv_poinfo.l_po_sts);

      ret = Hv_poinfo_Upd_Upd(hv_poinfo,g_pub_tx.reply);
      if(ret)
      {
          sprintf(acErrMsg,"ִ�к���hv_poinfo_upd_upd����!ret==[%d]\n",ret);
          WRITEMSG
          strcpy(g_pub_tx.reply,"D105");
          return (-1);
      }
      
      Hv_poinfo_Clo_Upd();
      return 0;
}
