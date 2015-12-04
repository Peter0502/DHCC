/*************************************************
* �� �� ��:  spB566.c
* �����������������Ԥ����֪ͨ    
*
* ��    ��: yindianheng
* ������ڣ� 2010��4��8��
*----------------------------ת�˲��ֲο��绰����ת��
* �޸ļ�¼��
*   ��   ��:
*   �� �� ��: 
*   �޸�����:

insert into tx_def values ('4966',' �������Ԥ����֪ͨ','10000000000000000000000000000000000000000100000000000000010000000000000000000000111000000000000000000000000000000000000000000000','1','3','2','3',null);
insert into tx_flow_def  values ('4966',0,'B566','#=0001@1@#$');
insert into tx_flow_def  values ('4966',1,'D003','#=0000@2@#$');
insert into tx_flow_def  values ('4966',2,'D099','#$');
insert into tx_sub_def values ('B566','  �������Ԥ����֪ͨ','spB566','0','0');
      
 

        
*************************************************/

#include <stdio.h>
#include <string.h>
#define EXTERN
#include "public.h"
#include "mo_discnt_c.h"
#include "ecds_recv_reg_c.h"
#include "dd_mst_c.h"
#include "com_parm_c.h"
#include "mdm_ac_rel_c.h"
#include "trace_log_c.h"
#include "mdm_attr_c.h"

int spB566()
{
    struct mdm_attr_c z_mdm_attr,v_mdm_attr;
    
    struct mo_discnt_c    sMoDiscnt,sMoDiscnt1;    /* ���ֵǼǱ� */
    struct ecds_recv_reg_c    sEcdsRecvReg;    
    struct dd_mst_c        sDdMst;
    struct dd_mst_c        sDdMst1;
    struct mdm_ac_rel_c    sMdmAcRel,sMdmAcRel1;
    struct com_parm_c    sComParm;
    struct trace_log_c    sTraceLog;
    struct ecds_recv_reg_c    sEcdsRecvReg1;
    
    int           ret;    
    double        damt;
    
    char        cpn_ac_no[33];        /* �������˺� */
    char        cpayee_ac_no[33];    /* �����˺� */
    char        cinitTyp[2];/*Ԥ�������� 0Ԥ���� 1����Ԥ����*/
   
    char    cbuff[61];
         
    memset(&z_mdm_attr, 0x00, sizeof(struct mdm_attr_c));
    memset(&v_mdm_attr, 0x00, sizeof(struct mdm_attr_c));
    
    memset(&sEcdsRecvReg1, 0x00, sizeof(struct ecds_recv_reg_c));
    memset(&sMoDiscnt1, 0x00, sizeof(struct mo_discnt_c));    
    
    memset(&sMoDiscnt, 0x00, sizeof(struct mo_discnt_c));
    memset(&sEcdsRecvReg, 0x00, sizeof(struct ecds_recv_reg_c));    
    memset(&sDdMst, 0x00, sizeof(struct dd_mst_c));
    memset(&sDdMst1, 0x00, sizeof(struct dd_mst_c));
    memset(&sMdmAcRel, 0x00, sizeof(struct mdm_ac_rel_c));
    memset(&sMdmAcRel1, 0x00, sizeof(struct mdm_ac_rel_c));
    memset(&sComParm, 0x00, sizeof(struct com_parm_c));
    memset(&sTraceLog, 0x00, sizeof(struct trace_log_c));        
    memset(cpn_ac_no, 0x00, sizeof(cpn_ac_no));
    memset(cpayee_ac_no, 0x00, sizeof(cpayee_ac_no));    
    memset(cinitTyp, 0x00, sizeof(cinitTyp));
    
   
    memset(cbuff, 0x00, sizeof(cbuff));
    

    
    /* ���ݳ�ʼ�� */
    pub_base_sysinit();
    
    
    get_zd_long("0450", &sEcdsRecvReg.duedt);       /* �������ڡ� */ 
      
    get_zd_data("0300", cbuff);    /* ԭ�������˺� */
    zip_space(cbuff);
    memcpy(cpn_ac_no,cbuff,sizeof(cpn_ac_no));
    memset(cbuff, 0x00, sizeof(cbuff));
    
    get_zd_data("0670", cinitTyp);/*Ԥ�������� */
    get_zd_double("0410",&damt);/*Ӧ�����*/    
    get_zd_long("0460",&sEcdsRecvReg.ecds_tx_date);/*��������*/   
         
    get_zd_data("0810", cbuff);    /* ������ˮ�� */
    zip_space(cbuff);
    memcpy(sEcdsRecvReg.ecds_trace_no,cbuff,sizeof(sEcdsRecvReg.ecds_trace_no));
    memset(cbuff, 0x00, sizeof(cbuff));
    
    get_zd_data("0680",sEcdsRecvReg.drftcls); /* Ʊ�ݷ��� */         
    get_zd_long("0520",&sTraceLog.pt_trace_no);
    get_zd_long("0440",&sTraceLog.tx_date);
    
    get_zd_data("0910", cbuff);    /* Ʊ������ */
    zip_space(cbuff);
    memcpy(sEcdsRecvReg.drfttyp,cbuff,sizeof(sEcdsRecvReg.drfttyp));
    memset(cbuff, 0x00, sizeof(cbuff));
    
    get_zd_data("0270", cbuff);    /* Ʊ�ݺ��� */
    vtcp_log(" [%s][%d] Ʊ�ݺ���buff:[%s] ",__FILE__,__LINE__,cbuff);
    zip_space(cbuff);
    memcpy(sEcdsRecvReg.drftnb,cbuff,sizeof(sEcdsRecvReg.drftnb));
    memset(cbuff, 0x00, sizeof(cbuff));
   
    
    vtcp_log("%s.%d   -------cdrftNb=%s  ...cdrftTyp=%s", __FILE__,__LINE__,sEcdsRecvReg.drftnb,sEcdsRecvReg.drftcls);
    vtcp_log(" [%s][%d] ����:[%ld]",__FILE__,__LINE__,sEcdsRecvReg.ecds_tx_date);
    vtcp_log(" [%s][%d] Ʊ�ݺ���:[%s] ",__FILE__,__LINE__,sEcdsRecvReg.drftnb);
    vtcp_log(" [%s][%d] ��������:[%ld] ",__FILE__,__LINE__,sEcdsRecvReg.duedt);
    vtcp_log(" [%s][%d] ԭ�������˺�:[%s] ",__FILE__,__LINE__,cpn_ac_no);
    vtcp_log(" [%s][%d] Ʊ������:[%s] ",__FILE__,__LINE__,sEcdsRecvReg.drfttyp);
    vtcp_log(" [%s][%d] Ԥ��������:[%s] ",__FILE__,__LINE__,cinitTyp);
    vtcp_log(" [%s][%d] ������ˮ��:[%s] ",__FILE__,__LINE__,sEcdsRecvReg.ecds_trace_no);
    vtcp_log(" [%s][%d] Ʊ�ݷ���:[%s] ",__FILE__,__LINE__,sEcdsRecvReg.drftcls);
    vtcp_log(" [%s][%d] Ӧ�����:[%f] ",__FILE__,__LINE__,damt);
    vtcp_log(" [%s][%d] sTraceLog.tx_date:[%d] ",__FILE__,__LINE__,sTraceLog.tx_date);
    vtcp_log(" [%s][%d] sTraceLog.pt_trace_no:[%d] ",__FILE__,__LINE__,sTraceLog.pt_trace_no);
     
    
    
    sEcdsRecvReg.issedt=0;    /* ��Ʊ�� */     
    sEcdsRecvReg.trace_no=g_pub_tx.trace_no;    /* ������ˮ�� */    
    sEcdsRecvReg.tx_date=g_pub_tx.tx_date;    /* ���Ľ������� */
    sEcdsRecvReg.bf_flag[0]='0';
    memcpy(sEcdsRecvReg.tag_name,"4956",sizeof(sEcdsRecvReg.tag_name));    /* �������� */
    memcpy(sEcdsRecvReg.sts,"1",sizeof(sEcdsRecvReg.sts));
    memcpy(sEcdsRecvReg.check_flag,"2",sizeof(sEcdsRecvReg.check_flag)-1);/*���˱�־*/
      
         
          /*---------�ж�ת��Ʊ������-------------------*/
    vtcp_log("[%s][%d]--------Ʊ������:[%s]",__FILE__,__LINE__,sEcdsRecvReg.drfttyp);
    if(!memcmp(sEcdsRecvReg.drfttyp,"AC01",4))
    {
        memcpy(sEcdsRecvReg.drfttyp,"111",4);
    }
    else if(!memcmp(sEcdsRecvReg.drfttyp,"AC02",4))
    {
        memcpy(sEcdsRecvReg.drfttyp,"112",4);        
    }
    else
    {
        sprintf(acErrMsg, "---Ʊ�����ʹ���!! ");
        WRITEMSG
        strcpy ( g_pub_tx.reply , "AT06" );
        goto ErrExit;
        
    }
   
    /*Ԥ�������� 0 Ԥ���� 1����Ԥ����*/
    if(cinitTyp[0]=='0')
    {
        ret=Ecds_recv_reg_Sel(g_pub_tx.reply,&sEcdsRecvReg1,"ecds_tx_date='%ld' and ecds_trace_no='%s'",\
            sEcdsRecvReg.ecds_tx_date, sEcdsRecvReg.ecds_trace_no);
        if(ret==0)
        { 
            int iflag=0;
            sprintf(acErrMsg,"Ecds_recv_reg�ü�¼����!! ret [%d],trace_no=[%s],date=[%ld] ",\
                   ret,sEcdsRecvReg1.ecds_trace_no,sEcdsRecvReg1.ecds_tx_date);
            WRITEMSG  	        	
            if(sEcdsRecvReg1.bf_flag[0]!='1')
            {
        	sprintf(acErrMsg," �ü�¼�Ѿ������!! bf_flag [%s],trace_no=[%s],date=[%ld] ",\
                        sEcdsRecvReg1.bf_flag,sEcdsRecvReg1.ecds_trace_no,sEcdsRecvReg1.ecds_tx_date);
                WRITEMSG
                   /*ret=0��ʾ�Ѿ�Ԥ�������sts=2��ʾ��Ʊ������sts=1Ԥ����*/
                 if(sEcdsRecvReg1.sts[0]=='1')
                 {
                     sprintf(acErrMsg,"�õ�Ʊ�Ѿ�����!! ret [%d],trace_no=[%s],date=[%ld] ",\
                        ret,sEcdsRecvReg1.ecds_trace_no,sEcdsRecvReg1.ecds_tx_date);
                     WRITEMSG
                     strcpy(g_pub_tx.reply, "NXDP");       
                     goto ErrExit; 
                 } 
                 vtcp_log(" [%s][%d] ret=[%d] Ecds_recv_reg.sts=[%s]!!",__FILE__,__LINE__,ret,sEcdsRecvReg1.sts);  
                 iflag=1;      
            }
            /*����Ecds_recv_reg״̬*/
             ret=Ecds_recv_reg_Dec_Upd(g_pub_tx.reply , "ecds_trace_no='%s' and ecds_tx_date= '%ld'",\
                     sEcdsRecvReg.ecds_trace_no,sEcdsRecvReg.ecds_tx_date);
             if (ret) 
             {
                 sprintf(acErrMsg, "Ecds_recv_reg_Dec_Upd![%d]", ret);
                 WRITEMSG
                 goto ErrExit;
             }
             ret=Ecds_recv_reg_Fet_Upd(&sEcdsRecvReg,g_pub_tx.reply);
             if (ret) 
             {
                 sprintf(acErrMsg, "Ecds_recv_reg_Fet_Upd![%d]", ret);
                 WRITEMSG
                 goto ErrExit;
            }
            if(iflag==1)/*bf_flag=(0,2)ʱiflag=1������Ҫ����bf_flag����sts=2�����Ԥ��������sts=1*/   
            {
                  memcpy(sEcdsRecvReg.sts,"1",sizeof(sEcdsRecvReg.sts)); 
            }
            else/*bf_flag=1ʱiflag=0����Ҫ����bf_flag=2��*/   
            {
                  memcpy(sEcdsRecvReg.bf_flag,"2",sizeof(sEcdsRecvReg1.bf_flag));    
            }  
            ret=Ecds_recv_reg_Upd_Upd(sEcdsRecvReg,g_pub_tx.reply);
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
            /*-----------Ecds_recv_reg_Ins------------*/
            ret=Ecds_recv_reg_Ins(sEcdsRecvReg,g_pub_tx.reply);    
            if (ret) 
            {
                sprintf(acErrMsg, "����Ecds_recv_reg��![%d]", ret);
                WRITEMSG
                goto ErrExit;
            }   
        }
        else
        {
             sprintf(acErrMsg,"��ѯEcds_recv_reg�����!! ret [%s]",ret);
             WRITEMSG
             strcpy ( g_pub_tx.reply , "AT06" );
             goto ErrExit;
        }
         
        /* ������ֵǼǱ���  */
        ret = Mo_discnt_Sel(g_pub_tx.reply, &sMoDiscnt, "pnote_no='%s' and pn_type='1' and note_type='%s'  and sts ='3'", \
              sEcdsRecvReg.drftnb,sEcdsRecvReg.drfttyp );    
        if (ret ==100) 
        {
        	/********************��α���һ��������Э����************************/
            char cTmpBuff[9];
            char cTraceNo[10];
            char cRpact_no[21];
            memset(cTmpBuff , 0 , sizeof(cTmpBuff));
            memset(cTraceNo , 0 , sizeof(cTraceNo));
            memset(cRpact_no , 0 , sizeof(cRpact_no));
            
            sprintf(cTmpBuff ,"%08d" , g_pub_tx.tx_date);
            vtcp_log("[%s][%d] cTmpBuff=[%s] ",__FILE__,__LINE__,cTmpBuff);    
            sprintf(cRpact_no ,"%08d" , g_pub_tx.tx_date);
            memcpy(cRpact_no , cTmpBuff+2 , 6);
            vtcp_log("[%s][%d] cRpact_no=[%s] ",__FILE__,__LINE__,cRpact_no);
            
            ret=Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdmAcRel,"ac_no='%s'",cpn_ac_no);
            if (ret ==100)
            {
                  sprintf(acErrMsg, "�������˻����в����ڴ��ʺ�!");
                  WRITEMSG
                  strcpy(g_pub_tx.reply, "B174");
                  goto ErrExit;
            } 
            else if (ret)
            {
                  sprintf(acErrMsg, "ִ��Mdm_ac_rel��![%d]", ret);
                  WRITEMSG
                  goto ErrExit;
            }        
 	     strcat(cRpact_no , sMdmAcRel.opn_br_no );
 	    
 	     vtcp_log("[%s][%d] cRpact_no=[%s] trace_no=[%ld]",__FILE__,__LINE__,cRpact_no,g_pub_tx.trace_no);
 	     sprintf(cTraceNo,"%09d",g_pub_tx.trace_no);
 	     strcat(cRpact_no , cTraceNo );
 	     vtcp_log("[%s][%d]������Э����=[%s]", __FILE__,__LINE__,cRpact_no);
 	     memcpy(sMoDiscnt.pact_no  , cRpact_no , sizeof(sMoDiscnt.pact_no)-1);
 	     memcpy(sMoDiscnt.br_no,sMdmAcRel.opn_br_no,sizeof(sMoDiscnt.br_no)-1); 	    
 	     memcpy(sMoDiscnt.pn_type ,"1",sizeof(sMoDiscnt.pn_type)); 	    
 	     sMoDiscnt.ic_date=sEcdsRecvReg.ecds_tx_date;
 	     sMoDiscnt.pn_date=sEcdsRecvReg.ecds_tx_date;
 	     memcpy(sMoDiscnt.pn_ac_no,cpn_ac_no,sizeof(sMoDiscnt.pn_ac_no));
 	     memcpy(sMoDiscnt.pay_ac_no,cpn_ac_no,sizeof(sMoDiscnt.pn_ac_no));
 	     memcpy(sMoDiscnt.sts ,"3",sizeof(sMoDiscnt.sts ));
 	     sMoDiscnt.not_matr_date=sEcdsRecvReg.duedt;
 	     memcpy(sMoDiscnt.note_type,sEcdsRecvReg.drfttyp,sizeof(sMoDiscnt.note_type));
             memcpy(sMoDiscnt.pnote_no,sEcdsRecvReg.drftnb,sizeof(sMoDiscnt.pnote_no));
             sMoDiscnt.wrk_date=g_pub_tx.tx_date;
             sMoDiscnt.trace_no=g_pub_tx.trace_no;
             memcpy(sMoDiscnt.po_knd,"E",sizeof(sMoDiscnt.po_knd));
             get_zd_long("1261",&sMoDiscnt.tt_no);
             
             ret = Mo_discnt_Ins(sMoDiscnt, g_pub_tx.reply);
             if (ret)
             {
                  sprintf(acErrMsg, "ִ��Mo_discnt_Ins����!ret=[%d]", ret);
                  WRITEMSG
                  goto ErrExit;
             }
                     	
            memset(&sMdmAcRel, 0x00, sizeof(struct mdm_ac_rel_c)); 
            
        } else if (ret) {
            sprintf(acErrMsg, "ִ��Mo_discnt_Sel��![%d]", ret);
            WRITEMSG
            goto ErrExit;
        }
        if(sMoDiscnt.pt_knd[0]=='0') 
        {
            sprintf(acErrMsg, "��Ʊ���Ѿ�Ԥ�����,��! ");
            WRITEMSG
            strcpy(g_pub_tx.reply, "0174");
            goto ErrExit;            
        }
    }
    else if(cinitTyp[0]=='1')/*Ԥ�������� 0 Ԥ���� 1����Ԥ����*/
    {
        ret=Ecds_recv_reg_Sel(g_pub_tx.reply,&sEcdsRecvReg1,"ecds_tx_date='%ld' and ecds_trace_no='%s'",\
            sEcdsRecvReg.ecds_tx_date, sEcdsRecvReg.ecds_trace_no);
        if(ret==0)
        {              
            sprintf(acErrMsg," ����Ԥ������Ecds_recv_reg���ü�¼����!! ret [%d],trace_no=[%s],date=[%ld] ",\
                   ret,sEcdsRecvReg1.ecds_trace_no,sEcdsRecvReg1.ecds_tx_date);
            WRITEMSG  	        	
            if(sEcdsRecvReg1.bf_flag[0]=='1')
            {
        	sprintf(acErrMsg," ����!!���ü�¼δԤ���� bf_flag [%s],trace_no=[%s],date=[%ld] ",\
                        sEcdsRecvReg1.bf_flag,sEcdsRecvReg1.ecds_trace_no,sEcdsRecvReg1.ecds_tx_date);
                WRITEMSG  
                goto ErrExit; 
            } 
              /*ret=0��ʾ�Ѿ�Ԥ�������sts=2��ʾ��Ʊ������sts=1δ����*/
            if(sEcdsRecvReg1.sts[0]=='2')
            {
                sprintf(acErrMsg,"�õ�Ʊ�Ѿ�����,ֱ�ӷ���!! ret [%d],trace_no=[%s],date=[%ld] ",\
                       ret,sEcdsRecvReg1.ecds_trace_no,sEcdsRecvReg1.ecds_tx_date);
                WRITEMSG
                strcpy(g_pub_tx.reply, "NXDP");       
                goto ErrExit; 
            }
            vtcp_log(" [%s][%d] ret=[%d] Ecds_recv_reg.sts=[%s]!!",__FILE__,__LINE__,ret,sEcdsRecvReg1.sts);  
                         
        }
        else if(ret==100)
        {
            sprintf(acErrMsg," �޼�¼����Ʊû��Ԥ�����޷�����������!! ret=[%d]",ret); 
            WRITEMSG 
            goto ErrExit;      
        }
        else
        {
            sprintf(acErrMsg,"��ѯEcds_recv_reg�����!! ret [%s]",ret);
            WRITEMSG
            strcpy ( g_pub_tx.reply , "AT06" );
            goto ErrExit;
        }
                
        /*-----����Ecds_recv_reg.sts==2------*/
        ret=Ecds_recv_reg_Dec_Upd(g_pub_tx.reply , "ecds_trace_no='%s' and ecds_tx_date= '%ld'",\
                 sEcdsRecvReg.ecds_trace_no,sEcdsRecvReg.ecds_tx_date);
        if (ret) {
            sprintf(acErrMsg, "Ecds_recv_reg_Dec_Upd![%d]", ret);
            WRITEMSG
            goto ErrExit;
        }
        ret=Ecds_recv_reg_Fet_Upd(&sEcdsRecvReg,g_pub_tx.reply);
        if (ret) {
            sprintf(acErrMsg, "Ecds_recv_reg_Fet_Upd![%d]", ret);
            WRITEMSG
            goto ErrExit;
        }
        memcpy(sEcdsRecvReg.sts,"2",sizeof(sEcdsRecvReg.sts));    
        ret=Ecds_recv_reg_Upd_Upd(sEcdsRecvReg,g_pub_tx.reply);
        if (ret) {
            sprintf(acErrMsg, "Ecds_recv_reg_Upd_Upd��![%d]", ret);
            WRITEMSG
            goto ErrExit;
        }
        Ecds_recv_reg_Clo_Upd();
        /*-----����Ecds_recv_reg------*/         
        
        ret=Trace_log_Sel(g_pub_tx.reply,&sTraceLog,"tx_date=%ld and trace_no=%ld",sTraceLog.tx_date,sTraceLog.pt_trace_no);
        if (ret ==100)
        {
            ret=Trace_log_bk_Sel(g_pub_tx.reply,&sTraceLog,"tx_date=%ld and trace_no=%ld",sTraceLog.tx_date,sTraceLog.pt_trace_no);
            if (ret) 
            {
                sprintf(acErrMsg, "�޴˽�����ˮ![%d]", ret);
                WRITEMSG
                goto ErrExit;
            }
            
        } 
        else if (ret) {
            sprintf(acErrMsg, "ִ��Trace_log��![%d]", ret);
            WRITEMSG
            goto ErrExit;
        }
        /*Ƿȱ�ģ���sTraceLog.trace_no�����صǼǲ��е�״̬��
        ��������Ԥ����״̬�ɳ�����������״̬Ϊ������
        Mo_discnt_call_backδ����         
        */  
        
        damt=-damt;
        
        
        /* ������ֵǼǱ���  */
        ret = Mo_discnt_Sel(g_pub_tx.reply, &sMoDiscnt, "pnote_no='%s' and pn_type='1' and note_type='%s'  and sts ='3'", \
            sEcdsRecvReg.drftnb,sEcdsRecvReg.drfttyp );    
        if (ret ==100) {
             sprintf(acErrMsg, "���ֵǼǱ��в����ڴ� Ʊ�ݺ�! ");
             WRITEMSG
             strcpy(g_pub_tx.reply, "B174");
             goto ErrExit;
        } 
        else if (ret) {
             sprintf(acErrMsg, "ִ��Mo_discnt_Sel��![%d]", ret);
             WRITEMSG
             goto ErrExit;
        }
        if(sMoDiscnt.pt_knd[0]!='0') 
        {
             sprintf(acErrMsg, "��Ʊ���Ѿ�Ԥ��������,ֱ�ӷ���! ");
             WRITEMSG
             goto OkExit;            
        }        
    }
    else
    {
        sprintf(acErrMsg, "Ԥ�������ʹ���[%s]",cinitTyp);
        WRITEMSG
        goto ErrExit;
        
    } 
    /*-----��cinitTyp��sMoDiscnt1.pt_knd����----------*/
    ret = Mo_discnt_Dec_Upd(g_pub_tx.reply, "pact_no='%s' and note_type='%s' and pnote_no='%s' and pn_type='1' and sts ='3' ",\
            sMoDiscnt.pact_no,sEcdsRecvReg.drfttyp,sEcdsRecvReg.drftnb);
    if (ret) 
    {
        sprintf(acErrMsg, "ִ��Mo_discnt_Dec_Upd��[%d]", ret);
        WRITEMSG
        goto ErrExit;
    }
    ret = Mo_discnt_Fet_Upd(&sMoDiscnt1, g_pub_tx.reply);
    if (ret == 100) 
    {
        sprintf(acErrMsg, "���ֵǼǱ��в����ڴ�Ʊ����Ϣ![%s],[%s]", sMoDiscnt1.note_type, sMoDiscnt1.pnote_no);
        WRITEMSG
        strcpy(g_pub_tx.reply, "B130");
        goto ErrExit;
    } 
    else if (ret) {
       sprintf(acErrMsg, "ִ��Mo_discnt_Fet_Upd��![%d]", ret);
       WRITEMSG
       goto ErrExit;
    } 
    memcpy(sMoDiscnt1.pt_knd,cinitTyp,sizeof(sMoDiscnt1.pt_knd));
    ret =Mo_discnt_Upd_Upd(sMoDiscnt1, g_pub_tx.reply);
    if (ret)
    {
        sprintf(acErrMsg, "�޸����ֵǼǱ���[%d]", ret);
        WRITEMSG
        goto ErrExit;
    }
    Mo_discnt_Clo_Upd();  
    
 
    /* -cpn_ac_no='0'ʱ����- */
    if((strlen(cpn_ac_no)==1)&&(cpn_ac_no[0]=='0'))
    {
        sprintf(acErrMsg, "���������������������!ԭ�������˺�:%s",cpn_ac_no);
        WRITEMSG        
        memcpy(cpn_ac_no,sMoDiscnt.pay_ac_no,sizeof(cpn_ac_no));    
    }
    /*----�����˵Ļ����˻���ѯ--*/
    ret=Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdmAcRel,"ac_no='%s'",cpn_ac_no);
    if (ret ==100) {
        sprintf(acErrMsg, "�������˻����в����ڴ��ʺ�!");
        WRITEMSG
        strcpy(g_pub_tx.reply, "B174");
        goto ErrExit;
    } 
    else if (ret) {
        sprintf(acErrMsg, "ִ��Mdm_ac_rel��![%d]", ret);
        WRITEMSG
        goto ErrExit;
    }
        
    ret=Dd_mst_Sel(g_pub_tx.reply,&sDdMst,"ac_id='%ld' ",sMdmAcRel.ac_id);
    if (ret ==100) {
        sprintf(acErrMsg, "���ڴ���Dd_mst�в����ڴ��ʺ�!");
        WRITEMSG
        strcpy(g_pub_tx.reply, "B174");
        goto ErrExit;
    } 
    else if (ret) {
        sprintf(acErrMsg, "ִ��Dd_mst��![%d]", ret);
        WRITEMSG
        goto ErrExit;
    }
    if(sDdMst.bal<damt)
    {
        sprintf(acErrMsg, "����![%s]",cpn_ac_no);
        WRITEMSG
        strcpy(g_pub_tx.reply, "0174");
        goto ErrExit;
        
    }        
    /*----�����˵Ļ����˻���ѯ--*/    
    
    /*----Ӧ�����˻�----- ������??-*/
    ret=Com_parm_Sel(g_pub_tx.reply,&sComParm,"parm_code='%s' and parm_seqn='%d' ",sMoDiscnt.br_no,PARM_ECDS_TC_AC);
    if (ret ==100) {
        sprintf(acErrMsg, "�����������в����ڴ��ʺ�!");
        WRITEMSG
        goto ErrExit;
    } 
    else if (ret) {
        sprintf(acErrMsg, "ִ��Com_parm��![%d]", ret);
        WRITEMSG
        goto ErrExit;
    }        
    ret=Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdmAcRel1,"ac_no='%s'",sComParm.val);
    if (ret ==100) {
        sprintf(acErrMsg, "�������˻����в����ڴ��ʺ�!");
        WRITEMSG
        goto ErrExit;
    } 
    else if (ret) {
        sprintf(acErrMsg, "ִ��Mdm_ac_rel��![%d]", ret);
        WRITEMSG
        goto ErrExit;
    }
    
    /*Ƿȱ��д��һ��������صǼǲ��У�����״̬Ϊ���Ԥ����״̬��
    ��¼ƽ̨��ũ��������ˮ�����ڡ�*/
    
    
    
    /*-------Ϊ����һ��������׼��--------------------*/
    
    /****����mdm_code����ת������note_type***/
    ret=Mdm_attr_Sel(g_pub_tx.reply,&v_mdm_attr," mdm_code='%s' ",sMdmAcRel.mdm_code );
    if(ret)
    {
        sprintf(acErrMsg,"ȡmdm_attrʧ��![%s]",sMdmAcRel.mdm_code);
        WRITEMSG
        strcpy( g_pub_tx.reply,"N055" );
        goto ErrExit;
    }
    
    ret=Mdm_attr_Sel(g_pub_tx.reply,&z_mdm_attr," mdm_code='%s' ",sMdmAcRel1.mdm_code );
    if(ret)
    {
        sprintf(acErrMsg,"ȡmdm_attrʧ��![%s]",sMdmAcRel1.mdm_code);
        WRITEMSG
        strcpy( g_pub_tx.reply,"N055" );
        goto ErrExit;
    }
    /*--------------------------*/
     memcpy(g_pub_tx.ac_no,cpn_ac_no,sizeof(g_pub_tx.ac_no));
     vtcp_log("%s,%d ����ת�����ʻ�g_pub_tx.ac_no=[%s]",__FILE__,__LINE__,g_pub_tx.ac_no);
   
  
    vtcp_log("[%s][%d]--- ���=%f", __FILE__,__LINE__,damt);  
     
    /*****end****/
    {/****��ʼΪspD003.c������׼��******/
        vtcp_log("[%s][%d]--- ת���˺�=%s", __FILE__,__LINE__,g_pub_tx.ac_no);    
        
        set_zd_data("1021",cpn_ac_no);/**ת���˺�**/
        set_zd_data("1023",v_mdm_attr.note_type);/**ƾ֤����**/
        set_zd_data("1024",sMdmAcRel.note_no);/**ƾ֤����**/
        set_zd_data("1022","0001");/**�ʻ����**/         
        set_zd_data("1025","");
        set_zd_data("1026","N");/**ƾ����֧ȡ??**/
        set_zd_data("1027","");/**��ѯ����**/
        
        set_zd_data("1029","N");/**֤��֧ȡ**/
        set_zd_data("102A","");/**֤������**/
        set_zd_data("102B","");
        set_zd_data("102C","");
        set_zd_data("102D","");
        g_pub_tx.mach_date=0;
        set_zd_double("102F",damt);
        set_zd_data("102K","2");/**��ת��־**/
        set_zd_data("102Y","");
        set_zd_data("0191","0");/**����19��**/
        set_zd_data("102O","1");/**���۱�־**/
        set_zd_data("102J","01");/**����**/
        set_zd_data("0910",g_pub_tx.tx_br_no);
    }
    /****����mdm_code����ת�뷽��note_type***/
    vtcp_log("[%s][%d]--- ---", __FILE__,__LINE__); 
    {/****Ϊ����spD099.c׼������***/
        vtcp_log("[%s][%d]--- ת���˺�=%s", __FILE__,__LINE__,sComParm.val);   
        set_zd_data("1011",sComParm.val);/**ת���˺�**/  
        set_zd_data("1014",z_mdm_attr.note_type);/**ƾ֤����**/
        set_zd_data("1015",sMdmAcRel1.note_no);      
        get_zd_data("0310","");        
        set_zd_data("1012","0000");/**�ʻ����**/        
        set_zd_data("101A","01");/**����**/
       
        set_zd_data("101B","2");/***��ת��־***/
        set_zd_double("1013",damt);/***ת����***/        
    }
    vtcp_log("[%s][%d] reply=[%s] ",__FILE__,__LINE__,g_pub_tx.reply);
OkExit1:
    strcpy(g_pub_tx.reply, "0001");
    sprintf(acErrMsg, "Before OK return: reply is[%s]", g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 0;
OkExit:
    strcpy(g_pub_tx.reply, "0000");
    sprintf(acErrMsg, "Before OK return: reply is[%s]", g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 0;
ErrExit:
    sprintf(acErrMsg, "Before return: reply is[%s]", g_pub_tx.reply);
    WRITEMSG
    if(!memcmp(g_pub_tx.reply,"0000",4))
    {
    	 strcpy(g_pub_tx.reply, "B566");   	
    }    	
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 1;
}
