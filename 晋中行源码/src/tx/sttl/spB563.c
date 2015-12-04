/*************************************************
* �� �� ��:          spB563.c
  ����������        ����Ѻ֪ͨ
*
* ��    ��:  yindianheng
* ������ڣ� 2010-4-12
* ��������:  ��ҵ���ڻƺ������������Ŵ�ϵͳ������Ѻ����Ǽǣ��ƺ�������Ϊ��Ȩ�ˣ�
*            �����Ŵ�ϵͳ����ͨ���������Ϣ״̬���浽����ѺƷ�Ǽǲ��У�
*            �յ������ģ��������е���ѺƷ���⴦��
*
* �޸ļ�¼��
*   ��    ��:
*   �� �� ��:
*   �޸�����:    

insert into tx_def values ('4963','����Ѻ֪ͨ','10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000010000000000000000000000000100','1','3','2','3',null)
insert into tx_flow_def   values ('4963','0','B563','#=0001@1@#$')
insert into tx_flow_def   values ('4963','1','4541','#=0000@2@#$')
insert into tx_flow_def   values ('4963','2','J222','#=0000@3@#$')
insert into tx_flow_def   values ('4963','3','L314','#$')
insert into tx_sub_def     values ('B563','����Ѻ֪ͨ','spB563','0','0') 
 
   
*************************************************/
 
 



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

int spB563()
{    
    struct gaga_good_reg_c        sGaga_good_reg;
    struct pact_no_gaga_rel_c    sPact_no_gaga_rel;
    struct ln_gage_reg_c        sLn_gage_reg;
    struct ecds_recv_reg_c        sEcds_recv_reg;
    struct mo_bank_acc_po_c        sMo_bank_acc_po;
    struct mo_bank_po_c        sMo_bank_po;
    struct ecds_recv_reg_c    sEcdsRecvReg1;
    
    char    ctranSeqNb    [28+1];      /* ������ˮ��          */ 
    char    cdrftCls      [1+1] ;      /* Ʊ�ݷ���            */ 
    char    cdrftTyp      [4+1] ;       /* Ʊ������            */ 
    char    cdrftNb       [30+1];       /* Ʊ�ݺ�              */ 

    long    ldt         =0 ;      /* ����                */ 
    long    ldueDt      =0 ;      /* ������              */ 
    double  ddrftAmt    =0.00 ;   /* Ʊ����            */
    
    char	cbuff[61];
    
    
    int             ret = 0;
    vtcp_log("[%s][%d]    begin spB565!", __FILE__,__LINE__);  
    
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
    get_zd_long("0460",&ldt                 );              /* ����                */        
    get_zd_data("0810",cbuff           );                 /* ������ˮ��          */  
    zip_space(cbuff);
    memcpy(ctranSeqNb,cbuff,sizeof(ctranSeqNb));
    memset(cbuff, 0x00, sizeof(cbuff));   
    get_zd_data("0670",cdrftCls             );                 /* Ʊ�ݷ���            */     
    get_zd_data("0910",cbuff             );                 /* Ʊ������            */ 
    zip_space(cbuff);
    memcpy(cdrftTyp,cbuff,sizeof(cdrftTyp));
    memset(cbuff, 0x00, sizeof(cbuff));     
    get_zd_data("0270",cbuff              );                 /* Ʊ�ݺ�              */ 
    zip_space(cbuff);
    memcpy(cdrftNb,cbuff,sizeof(cdrftNb));
    memset(cbuff, 0x00, sizeof(cbuff));
         
    get_zd_double("0400",&ddrftAmt          );              /* Ʊ����            */        
    get_zd_long("0450",&ldueDt              );                 /* ������              */ 
        
          
	vtcp_log("---------------- file:[%s]---line:[%d] ---------------",__FILE__,__LINE__);     
	vtcp_log(" [%s][%d] ����:[%ld]",__FILE__,__LINE__,ldt);
    	vtcp_log(" [%s][%d] ������ˮ��:[%s] ",__FILE__,__LINE__,ctranSeqNb);
	vtcp_log(" [%s][%d] Ʊ������:[%s] ",__FILE__,__LINE__,cdrftTyp);
	vtcp_log(" [%s][%d] Ʊ�ݷ���:[%s] ",__FILE__,__LINE__,cdrftCls);
	 
	vtcp_log(" [%s][%d] Ʊ�ݺ���:[%s] ",__FILE__,__LINE__,cdrftNb);
	vtcp_log(" [%s][%d] ��������:[%ld] ",__FILE__,__LINE__,ldueDt);
	vtcp_log(" [%s][%d] Ʊ����:[%f] ",__FILE__,__LINE__,ddrftAmt);
	 	
    /*-----------�жϸõ�Ʊ�Ƿ��Ѿ������------------*/
    ret=Ecds_recv_reg_Sel(g_pub_tx.reply,&sEcdsRecvReg1,"ecds_tx_date='%ld' and ecds_trace_no='%s'",\
            ldt, ctranSeqNb);
    if(ret==0)
    {
        sprintf(acErrMsg,"Ecds_recv_reg�ü�¼����!! ret [%d],trace_no=[%s],date=[%ld] ",\
            ret,sEcdsRecvReg1.ecds_trace_no,sEcdsRecvReg1.ecds_tx_date);
        WRITEMSG  
        if(sEcdsRecvReg1.bf_flag[0]!='1')
        {
        	sprintf(acErrMsg,"����,�ü�¼�Ѿ������!! bf_flag [%s],trace_no=[%s],date=[%ld] ",\
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
            sprintf(acErrMsg, "����Ecds_recv_reg_Dec_Upd![%d]", ret);
            WRITEMSG
            goto ErrExit;
        }
        ret=Ecds_recv_reg_Fet_Upd(&sEcdsRecvReg1,g_pub_tx.reply);
        if (ret) 
        {
            sprintf(acErrMsg, "����Ecds_recv_reg_Fet_Upd![%d]", ret);
            WRITEMSG
            goto ErrExit;
       }
       memcpy(sEcdsRecvReg1.bf_flag,"2",sizeof(sEcdsRecvReg1.bf_flag));    
       ret=Ecds_recv_reg_Upd_Upd(sEcdsRecvReg1,g_pub_tx.reply);
       if (ret)
       {
            sprintf(acErrMsg, "Ecds_recv_reg_Upd_Upd��![%d]", ret);
            WRITEMSG
            goto ErrExit;
       }
       Ecds_recv_reg_Clo_Upd();     
    }
    else if(ret==100)
    {
         vtcp_log(" [%s][%d] ret=[%d] û�в鵽��Ӧ����Ϣ!!",__FILE__,__LINE__,ret); 
         sEcds_recv_reg.ecds_tx_date=ldt ;                /*����*/    
         memcpy(sEcds_recv_reg.ecds_trace_no,    ctranSeqNb,    sizeof(sEcds_recv_reg.ecds_trace_no)-1); 
         sEcds_recv_reg.trace_no         = g_pub_tx.trace_no;
         sEcds_recv_reg.tx_date          = g_pub_tx.tx_date;        
         memcpy(sEcds_recv_reg.drftcls,        cdrftCls,sizeof(sEcds_recv_reg.drfttyp)-1);     
         memcpy(sEcds_recv_reg.drfttyp ,        cdrftTyp,    sizeof(sEcds_recv_reg.drftnb)-1);     
         memcpy(sEcds_recv_reg.drftnb ,        cdrftNb,    sizeof(sEcds_recv_reg.drftnb)-1);   
         sEcds_recv_reg.issedt             = 0 ;  /*???????*/           
         sEcds_recv_reg.duedt             = ldueDt ;  
         memcpy(sEcds_recv_reg.tag_name,    "4963",sizeof( sEcds_recv_reg.tag_name)-1);      
         sEcds_recv_reg.sts[0]='1';
         sEcds_recv_reg.bf_flag[0]='0';
         memcpy(sEcds_recv_reg.check_flag,"2",sizeof(sEcds_recv_reg.check_flag)-1);/*���˱�־*/
         /*----------------------------------*/
         Ecds_recv_reg_Debug(&sEcds_recv_reg);
         ret = Ecds_recv_reg_Ins(sEcds_recv_reg,g_pub_tx.reply);
         if(ret )
         {
             sprintf(acErrMsg, "����Ecds_recv_reg��!!ret=[%d]", ret);
             WRITEMSG
             strcpy(g_pub_tx.reply, "NXC8");
             goto ErrExit;
         }   
    }
    else
    {
        sprintf(acErrMsg,"��ѯEcds_recv_reg�����!! ret [%d]",ret);
        WRITEMSG
        strcpy ( g_pub_tx.reply , "AT06" );
        goto ErrExit;
    }
   
     
   
    /*----����---Mo_bank_po.po_sts״̬???????--*/
    ret=Mo_bank_po_Sel(g_pub_tx.reply,&sMo_bank_po,"po_no='%s' and po_sts='1'",cdrftNb);
    if(ret )
    {
        sprintf(acErrMsg, "��ѯMo_bank_po��!!ret=[%d]", ret);
        WRITEMSG
        strcpy(g_pub_tx.reply, "B209");
        goto ErrExit;
    }
    /*
    ret=Mo_bank_acc_po_Sel(g_pub_tx.reply,&sMo_bank_acc_po,"pact_no='%s' ",sMo_bank_po.pact_no);
    if(ret )
    {
        sprintf(acErrMsg, "��ѯMo_bank_po��!!ret=[%d]", ret);
        WRITEMSG
        strcpy(g_pub_tx.reply, "B119");
        goto ErrExit;
    }
     */        
    ret=Ln_gage_reg_Sel(g_pub_tx.reply,&sLn_gage_reg,"pact_no='%s' ",sMo_bank_po.pact_no);
    if(ret )
    {
        sprintf(acErrMsg, "��ѯLn_gage_reg��!!ret=[%d]", ret);
        WRITEMSG
        strcpy(g_pub_tx.reply, "L211");
        goto ErrExit;
    }  
     vtcp_log("[%s][%d]--- ��ͬ��=%s", __FILE__,__LINE__,sMo_bank_po.pact_no);
	 
    /*--------------Ϊ������׼��------------*/  
    set_zd_data("0620", sMo_bank_po.pact_no);    /* ��ͬ�� */
    set_zd_data("0370",sMo_bank_po.pact_no);    /* ��ͬ��*/
    set_zd_data("0810",sLn_gage_reg.gage_inf);        /* ��ѺƷ��Ϣ */
    set_zd_data("0650", "3314");            /* ���״��� */
    /*--------------Ϊ������׼��------------*/  
GoodExit1:
    vtcp_log("function end:file:%s---line:%d---", __FILE__,__LINE__);
    strcpy(g_pub_tx.reply, "0001");
    sprintf(acErrMsg, "Before OK return: reply [%s]", g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 0;        
    
    
GoodExit:
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
    	 strcpy(g_pub_tx.reply, "B563");   	
    }    	
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 1;
}
