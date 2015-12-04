/*************************************
*�������ƣ�spF251.c�c
*���׹���:��ѯ���Ľ��׵Ľ���״̬���ظ�ƽ̨
*�� �� ��: CHENGBO	
*
*���ʱ��:2011-11-21
* insert into tx_def values('6251','����״̬��ѯ','10000000000000000000000000000000000000000000000000000000000000000010000000000000000000000000000000000000000000000000000000000000','0','4','0');
* insert into tx_flow_def  values ('6251', 0, 'F251', '#$');
* insert into tx_sub_def  values ('F251', '����״̬��ѯ', 'spF251', '0', '0');
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
              cSts[0]='0';   /*���˳ɹ�*/
          }
          else if( sChnl_single_acct.sts[0] == '2' )
          {
              cSts[0]='2';   /*���˼��˳ɹ�*/
          }
          else if( sChnl_single_acct.sts[0] == '3' ) 
          {
              cSts[0]='3';    /*���˺����˳ɹ�*/    
          }
          else if( sChnl_single_acct.sts[0] == '4' ) 
          {
              cSts[0]='4';    /*�����ɹ�*/    
          }
          else if( sChnl_single_acct.sts[0] == '5' ) 
          {
              cSts[0]='5';    /*�˻سɹ�*/    
          }
          else if( sChnl_single_acct.sts[0] == '6' ) 
          {
              cSts[0]='6';    /*�˻��ɹ�*/    
          }
     }
     else if(iRet == 100)
     {
          cSts[0] = '1';      /*���˲��ɹ����޴˼�¼*/ 
     }
     else if(iRet)
     {
          sprintf(acErrMsg,"��ѯ���ʼ��˵Ǽǲ�����!");
          WRITEMSG
          goto ErrExit;
     }
     set_zd_data("0670",cSts);
OkExit:
     strcpy( g_pub_tx.reply, "0000" );
     sprintf(acErrMsg,"��ѯ����״̬�ɹ�!");
     WRITEMSG
     set_zd_data(DC_REPLY,g_pub_tx.reply);
     return 0;
ErrExit:
     sprintf(acErrMsg,"failed !! reply is[%s]",g_pub_tx.reply);
     WRITEMSG
     set_zd_data(DC_REPLY,g_pub_tx.reply);
     return 1;
}
