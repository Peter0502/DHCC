/************************************************************************
* 文 件 名:  spB565.c 票据已作废变更通知
* 功能描述： 票据到期后，在票据有效生命周期内(人行规定为2年)，持票人仍没有提示付款。
*            此种情况下，人行会将超过有效生命周期的票据变更已作废票据。
*            票据作废时承兑行会收到此报文通知。并进行账务处理。
*            由于票据到期时已经将保证金批量划入客户帐，所以本交易只处理承兑的表外账务。
*
* 作    者: yindianheng
* 完成日期： 2010年4月12日
*
* 修改记录：
*   日   期:
*   修 改 人: 
*   修改内容:
insert into tx_def values ('4965','票据已作废变更通知','10000000000000000000000000000000000000000100000000000000010000000000000000000000111000000000000000000000000000000000000000000000','1','3','2','3',null)
insert into tx_flow_def  values ('4965',0,'B565','#=0001@1@#$')
insert into tx_flow_def  values ('4965',1,'B337','#$')
insert into tx_sub_def values ('B565','票据已作废变更通知','spB565','0','0')
 
******************************************************************************************************/

#include <stdio.h>
#include <string.h>
#define EXTERN
#include "public.h"


#include "mo_discnt_c.h"
#include "ecds_recv_reg_c.h"

#include "mo_bank_acc_po_c.h"
#include "mo_bank_po_c.h"
#include "ln_gage_reg_c.h"
#include "gaga_good_reg_c.h"
#include "pact_no_gaga_rel_c.h"

int spB565()
{
     struct gaga_good_reg_c          sGaga_good_reg;
     struct pact_no_gaga_rel_c     sPact_no_gaga_rel;
     struct ln_gage_reg_c          sLn_gage_reg;
     struct ecds_recv_reg_c          sEcds_recv_reg;
     struct mo_bank_acc_po_c          sMo_bank_acc_po;
     struct mo_bank_po_c          sMo_bank_po;
     struct ecds_recv_reg_c     sEcdsRecvReg1;
     
     char     ctranSeqNb    [28+1];      /* 交易流水号          */ 
     char     cdrftCls      [1+1] ;      /* 票据分类            */ 
     char     cdrftTyp      [4+1] ;       /* 票据类型            */ 
     char     cdrftNb       [30+1];       /* 票据号              */ 

     long     ldt         =0 ;      /* 日期                */ 
     long     ldueDt      =0 ;          /* 到期日              */ 
     long     lisseDt     =0 ;          /*出票日*/
     double   ddrftAmt    =0.00 ;       /* 票面金额            */
     
     char	cbuff[61]; 
     int             ret = 0;
     vtcp_log("[%s][%d] begin spB565 !!!!",__FILE__,__LINE__);  
     
     memset(&sGaga_good_reg,0x00,sizeof(struct gaga_good_reg_c));
     memset(&sPact_no_gaga_rel,0x00,sizeof(struct pact_no_gaga_rel_c));
     memset(&sEcds_recv_reg,0x00,sizeof(struct ecds_recv_reg_c));
     memset(&sLn_gage_reg,0x00,sizeof(struct ln_gage_reg_c));
     memset(&sMo_bank_acc_po,0x00,sizeof(struct mo_bank_acc_po_c));
     memset(&sMo_bank_po,0x00,sizeof(struct mo_bank_po_c));
     memset(&sEcdsRecvReg1, 0x00, sizeof(struct ecds_recv_reg_c));
     
     
     memset(ctranSeqNb             ,0x00,sizeof(ctranSeqNb   ));
     memset(cdrftCls               ,0x00,sizeof(cdrftCls     ));
     memset(cdrftTyp               ,0x00,sizeof(cdrftTyp     ));
     memset(cdrftNb                ,0x00,sizeof(cdrftNb      ));
    
     memset(cbuff, 0x00, sizeof(cbuff)); 
     
     pub_base_sysinit();
          
     get_zd_long("0460",&ldt                 );       /* 日期                */        
     get_zd_data("0810",cbuff           );       /* 交易流水号          */   
     zip_space(cbuff);
     memcpy(ctranSeqNb,cbuff,sizeof(ctranSeqNb));
     memset(cbuff, 0x00, sizeof(cbuff));  
     get_zd_data("0670",cdrftCls             );       /* 票据分类            */     
     get_zd_data("0910",cbuff             );       /* 票据类型            */ 
     zip_space(cbuff);
     memcpy(cdrftTyp,cbuff,sizeof(cdrftTyp));
     memset(cbuff, 0x00, sizeof(cbuff));     
     get_zd_data("0270",cbuff              );       /* 票据号              */  
     zip_space(cbuff);
     memcpy(cdrftNb,cbuff,sizeof(cdrftNb));
     memset(cbuff, 0x00, sizeof(cbuff));    
     get_zd_double("0400",&ddrftAmt          );       /* 票面金额            */        
     get_zd_long("0450",&ldueDt              );       /* 到期日              */     
     get_zd_long("0470",&lisseDt          );     /*出票日*/
	
	vtcp_log("%s.%d   -------cdrftNb=%s  ...cdrftTyp=%s", __FILE__,__LINE__,cdrftNb,cdrftTyp);
   	vtcp_log(" [%s][%d] 日期:[%ld]",__FILE__,__LINE__,ldt);
    	vtcp_log(" [%s][%d] 交易流水号:[%s] ",__FILE__,__LINE__,ctranSeqNb);
	vtcp_log(" [%s][%d] 票据类型:[%s] ",__FILE__,__LINE__,cdrftTyp);
	vtcp_log(" [%s][%d] 票据分类:[%s] ",__FILE__,__LINE__,cdrftCls);
	 
	vtcp_log(" [%s][%d] 票据号码:[%s] ",__FILE__,__LINE__,cdrftNb);
	vtcp_log(" [%s][%d] 到期日期:[%ld] ",__FILE__,__LINE__,ldueDt);
	vtcp_log(" [%s][%d] 票面金额:[%f] ",__FILE__,__LINE__,ddrftAmt);
	vtcp_log(" [%s][%d] 出票日:[%ld] ",__FILE__,__LINE__,lisseDt);
		 
	
	
     /*-----------判断该电票是否已经处理过------------*/
     ret=Ecds_recv_reg_Sel(g_pub_tx.reply,&sEcdsRecvReg1,"ecds_tx_date='%ld' and ecds_trace_no='%s'",\
               ldt, ctranSeqNb);
     if(ret==0)
     {
           sprintf(acErrMsg,"Ecds_recv_reg该记录存在!! ret [%d],trace_no=[%s],date=[%ld] ",\
               ret,sEcdsRecvReg1.ecds_trace_no,sEcdsRecvReg1.ecds_tx_date);
           WRITEMSG  
           if(sEcdsRecvReg1.bf_flag[0]!='1')
           {
           	sprintf(acErrMsg,"错误,该记录已经处理过!! bf_flag [%s],trace_no=[%s],date=[%ld] ",\
                           sEcdsRecvReg1.bf_flag,sEcdsRecvReg1.ecds_trace_no,sEcdsRecvReg1.ecds_tx_date);
                   WRITEMSG 
                   strcpy(g_pub_tx.reply, "NXDP");       
                   goto ErrExit; 
           }
           memset(&sEcdsRecvReg1, 0x00, sizeof(sEcdsRecvReg1));
           ret=Ecds_recv_reg_Dec_Upd(g_pub_tx.reply , "ecds_trace_no='%s' and ecds_tx_date= '%ld'",\
                    ctranSeqNb,ldt);
           if (ret) 
           {
               sprintf(acErrMsg, "错误，Ecds_recv_reg_Dec_Upd![%d]", ret);
               WRITEMSG
               goto ErrExit;
           }
           ret=Ecds_recv_reg_Fet_Upd(&sEcdsRecvReg1,g_pub_tx.reply);
           if (ret) 
           {
               sprintf(acErrMsg, "错误，Ecds_recv_reg_Fet_Upd![%d]", ret);
               WRITEMSG
               goto ErrExit;
          }
          memcpy(sEcdsRecvReg1.bf_flag,"2",sizeof(sEcdsRecvReg1.bf_flag));    
          ret=Ecds_recv_reg_Upd_Upd(sEcdsRecvReg1,g_pub_tx.reply);
          if (ret)
          {
               sprintf(acErrMsg, "Ecds_recv_reg_Upd_Upd错![%d]", ret);
               WRITEMSG
               goto ErrExit;
          }
          Ecds_recv_reg_Clo_Upd();           
     }
     else if(ret==100)
     {
          vtcp_log(" [%s][%d] ret=[%d] 没有查到相应的信息!!",__FILE__,__LINE__,ret); 
           sEcds_recv_reg.ecds_tx_date=ldt ;                    /*日期*/     
           memcpy(sEcds_recv_reg.ecds_trace_no,     ctranSeqNb,     sizeof(sEcds_recv_reg.ecds_trace_no)-1); 
           sEcds_recv_reg.trace_no           = g_pub_tx.trace_no;
           sEcds_recv_reg.tx_date            = g_pub_tx.tx_date;        
           memcpy(sEcds_recv_reg.drftcls,          cdrftCls,sizeof(sEcds_recv_reg.drfttyp)-1);     
           memcpy(sEcds_recv_reg.drfttyp ,          cdrftTyp,     sizeof(sEcds_recv_reg.drftnb)-1);     
           memcpy(sEcds_recv_reg.drftnb ,          cdrftNb,     sizeof(sEcds_recv_reg.drftnb)-1);   
           sEcds_recv_reg.issedt                = lisseDt ;               
           sEcds_recv_reg.duedt                = ldueDt ;  
           memcpy(sEcds_recv_reg.tag_name,     "4965",sizeof( sEcds_recv_reg.tag_name)-1);      
           sEcds_recv_reg.sts[0]='1';
           sEcds_recv_reg.bf_flag[0]='0';
           memcpy(sEcds_recv_reg.check_flag,"2",sizeof(sEcds_recv_reg.check_flag)-1);/*对账标志*/
           Ecds_recv_reg_Debug(&sEcds_recv_reg);
           ret = Ecds_recv_reg_Ins(sEcds_recv_reg,g_pub_tx.reply);
           if(ret )
           {
               sprintf(acErrMsg, "插入Ecds_recv_reg错!!ret=[%d]", ret);
               WRITEMSG
               strcpy(g_pub_tx.reply, "NXC8");
               goto ErrExit;
           }    
     }
     else
     {
         sprintf(acErrMsg,"查询Ecds_recv_reg表错误!! ret [%d]",ret);
         WRITEMSG
         strcpy ( g_pub_tx.reply , "AT06" );
         goto ErrExit;
     }
     /*-----------判断该电票是否已经处理过------------*/
   
     vtcp_log("[%s][%d]--- 承兑汇票号码=%s", __FILE__,__LINE__,cdrftNb);
	
     set_zd_data("0580", cdrftNb);     /* 承兑汇票号码 */
     set_zd_double("100E", ddrftAmt);     /*   */
     set_zd_data("0670", "2");     /* 现转标志 */
     set_zd_data("1021", "");     /* 转存账号 */
GoodExit:
     vtcp_log("function end:file:%s---line:%d---", __FILE__,__LINE__);
     strcpy(g_pub_tx.reply, "0001");
     sprintf(acErrMsg, "Before OK return: reply [%s]", g_pub_tx.reply);
     WRITEMSG
     set_zd_data(DC_REPLY, g_pub_tx.reply);
     return 0; 
     
GoodExit1:
     vtcp_log("function end:file:%s---line:%d---", __FILE__,__LINE__);
     strcpy(g_pub_tx.reply, "0000");
     sprintf(acErrMsg, "Before OK return: reply [%s]", g_pub_tx.reply);
     WRITEMSG
     set_zd_data(DC_REPLY, g_pub_tx.reply);
     return 0;              

ErrExit:
    sprintf(acErrMsg, "Before return: reply is[%s]", g_pub_tx.reply);
    WRITEMSG
    if(!memcmp(g_pub_tx.reply,"0000",4))
    {
    	 strcpy(g_pub_tx.reply, "B565");   	
    }    	
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 1;
}
