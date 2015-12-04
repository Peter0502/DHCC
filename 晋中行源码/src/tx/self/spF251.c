/*************************************
*交易名称：spF251.c
*交易功能:查询核心交易的交易状态返回给平台
*完 成 人: CHENGBO	
*
*完成时间:2011-11-21
* insert into tx_def values('6251','交易状态查询','10000000000000000000000000000000000000000000000000000000000000000010000000000000000000000000000000000000000000000000000000000000','0','4','0');
* insert into tx_flow_def  values ('6251', 0, 'F251', '#$');
* insert into tx_sub_def  values ('F251', '交易状态查询', 'spF251', '0', '0');
*
***********************************/
#include <stdio.h>
#include "public.h"
#include "chnl_single_acct_c.h"

spF251()
{
     int iRet=0;
     long lPt_date=0;
     long lPt_trace=0;
     char cPt_trace[13];
     char cSts[2];
    
     struct chnl_single_acct_c sChnl_single_acct;

     memset(&sChnl_single_acct,0x00,sizeof(struct chnl_single_acct_c));
     memset(cPt_trace,0x00,sizeof(cPt_trace));
     memset(cSts,0x00,sizeof(cSts));     

     pub_base_sysinit();
     
     get_zd_long("0450",&lPt_date);
     get_zd_long("0780",&lPt_trace);
     
     sprintf(cPt_trace,"%ld",lPt_trace);

     iRet=Chnl_single_acct_Sel(g_pub_tx.reply,&sChnl_single_acct," pt_date=%ld                     and pt_trace_no='%s' ",lPt_date,cPt_trace);
     if(iRet == 0)
     {
          if( sChnl_single_acct.sts[0] == '1' )
          {
              cSts[0]='0';   /*记账成功*/
          }
          else if( sChnl_single_acct.sts[0] == '2' )
          {
              cSts[0]='2';   /*挂账记账成功*/
          }
          else if( sChnl_single_acct.sts[0] == '3' ) 
          {
              cSts[0]='3';    /*挂账后入账成功*/    
          }
          else if( sChnl_single_acct.sts[0] == '4' ) 
          {
              cSts[0]='4';    /*冲正成功*/    
          }
          else if( sChnl_single_acct.sts[0] == '5' ) 
          {
              cSts[0]='5';    /*退回成功*/    
          }
          else if( sChnl_single_acct.sts[0] == '6' ) 
          {
              cSts[0]='6';    /*退货成功*/    
          }
     }
     else if(iRet == 100)
     {
          cSts[0] = '1';      /*记账不成功或无此记录*/ 
     }
     else if(iRet)
     {
          sprintf(acErrMsg,"查询单笔记账登记簿错误!");
          WRITEMSG
          goto ErrExit;
     }
     set_zd_data("0670",cSts);
OkExit:
     strcpy( g_pub_tx.reply, "0000" );
     sprintf(acErrMsg,"查询交易状态成功!");
     WRITEMSG
     set_zd_data(DC_REPLY,g_pub_tx.reply);
     return 0;
ErrExit:
     sprintf(acErrMsg,"failed !! reply is[%s]",g_pub_tx.reply);
     WRITEMSG
     set_zd_data(DC_REPLY,g_pub_tx.reply);
     return 1;
}
