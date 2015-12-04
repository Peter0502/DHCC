/*************************************************
* �� �� ��:  spB559.c
            ���������֪ͨ
* ��������: ��ƽ̨�������ݣ�׼���ع�ʽ������ҵ��������ݡ�
* ��    ��:  yindianheng
* ������ڣ� 2010-4-11
*
* �޸ļ�¼��
*   ��    ��:
*   �� �� ��:
*   �޸�����:
 
insert into tx_def values ('4959','���������֪ͨ','10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000010000000000000000000000000100','1','3','2','3',null)
insert into tx_flow_def  values ('4959','0','B559','#=0001@1@#$')  
insert into tx_flow_def  values ('4959','1','B945','#=0000@2@#$')  
insert into tx_flow_def  values ('4959','2','A016','#=0000@3@#$')           
insert into tx_flow_def  values ('4959','3','D005','#=0001@4@#=0002@5@#$')           
insert into tx_flow_def  values ('4959','4','L201','#$')           
insert into tx_flow_def  values ('4959','5','a017','#$')
insert into tx_sub_def values ('B559','���������֪ͨ','spB559','0','0')



insert into  tx_sub_rel (tx_code, in_sub_tx, out_ind,out_data,in_data,memo)
select '4959',in_sub_tx,out_ind,out_data,in_data,memo  from tx_sub_rel
where tx_code ='4945' ;


   

*************************************************/

#include "public.h" 
#include "mo_discnt_c.h"
#include "ecds_recv_reg_c.h"
#include "in_mst_c.h"
#include "hv_orno_brno_c.h"
 
#include "com_parm_c.h"
 
 
int spB559()
{ 
    struct ecds_recv_reg_c    sEcds_recv_reg;   
    struct mo_discnt_c    sMo_discnt; 
    struct hv_orno_brno_c    s_Hv_orno_brno;
    struct com_parm_c    sCom_parm;
    struct mdm_ac_rel_c    sMdm_ac_rel;
    struct ecds_recv_reg_c    sEcdsRecvReg1;
    struct in_mst_c		sIn_mst;
    
    /*�������*/
    int     ret = 0;
    long    dt           =0    ;              /* ����                */ 
    long    dueDt        =0 ;                 /* ������              */ 
           
    char    stlFlg              [4+1] ;       /* �����־            */ 
    char    drftCls             [1+1] ;       /* Ʊ�ݷ���            */ 
    char    drftTyp             [4+1] ;       /* Ʊ������            */ 
    char    drftNb              [30+1];       /* Ʊ�ݺ�              */ 
    char    tranSeqNb           [28+1];       /* ������ˮ��          */ 
    char    intrstTyp           [1+1] ;       /* ��������            */ 
    char    origDscntInAcctSvcr [12+1];       /* ԭ�������к�        */ 
    char    origDscntInAcct     [32+1];       /* ԭ�������˺�        */ 
    char    origDscntInNm       [60+1];       /* ԭ����������        */ 
    char    agtAcctSvcr_1       [12+1];       /* (ԭ����+1]�н����к�*/ 
    char    origDscntOutAcctSvcr[12+1];       /* ԭ�������к�        */ 
    char    origDscntOutAcct    [32+1];       /* ԭ�������˺�        */ 
    char    origDscntOutNm      [60+1];       /* ԭ����������        */ 
    char    agtAcctSvcr_2       [12+1];       /* (ԭ����+1]�н����к�*/ 
    
    char    cbuff[61];
    
    double    drftAmt     =0.00 ;             /* Ʊ����            */
    double    intrstRate  =0.00 ;             /* ����                */
    double    amt         =0.00 ;             /* ʵ�����            */
   
    /***********��ʼ���ṹ��ͱ���***********/
    vtcp_log("[%s][%d] ���������֪ͨ��ʼ��!!!", __FILE__,__LINE__);
    
    memset(cbuff, 0x00, sizeof(cbuff));
       
    memset(stlFlg                ,0x00,sizeof(stlFlg              ));
    memset(drftCls               ,0x00,sizeof(drftCls             ));
    memset(drftTyp               ,0x00,sizeof(drftTyp             ));
    memset(drftNb                ,0x00,sizeof(drftNb              ));
    memset(intrstTyp             ,0x00,sizeof(intrstTyp           ));
    memset(tranSeqNb             ,0x00,sizeof(tranSeqNb           ));
    memset(origDscntInAcctSvcr   ,0x00,sizeof(origDscntInAcctSvcr ));
    memset(origDscntInAcct       ,0x00,sizeof(origDscntInAcct     ));
    memset(origDscntInNm         ,0x00,sizeof(origDscntInNm       ));
    memset(agtAcctSvcr_1         ,0x00,sizeof(agtAcctSvcr_1       ));
    memset(origDscntOutAcctSvcr  ,0x00,sizeof(origDscntOutAcctSvcr));
    memset(origDscntOutAcct      ,0x00,sizeof(origDscntOutAcct    ));
    memset(origDscntOutNm        ,0x00,sizeof(origDscntOutNm      ));
    memset(agtAcctSvcr_2         ,0x00,sizeof(agtAcctSvcr_2       ));
    
    memset(&s_Hv_orno_brno,0x00,sizeof(struct hv_orno_brno_c));
    memset(&sEcds_recv_reg,0x00,sizeof(struct ecds_recv_reg_c));
    memset(&sMo_discnt,0x00,sizeof(struct mo_discnt_c));
    memset(&sCom_parm,0x00,sizeof(struct com_parm_c));
    memset(&sEcdsRecvReg1, 0x00, sizeof(sEcdsRecvReg1));
    memset(&sIn_mst, 0x00, sizeof(struct in_mst_c));
        
    pub_base_sysinit();
    /**********��ƽ̨�õ�����**********/
    get_zd_long("0460",&dt );      /* ����                */        
    
    get_zd_data("0810",cbuff);       /* ������ˮ��          */  
    zip_space(cbuff);
    memcpy(tranSeqNb,cbuff,sizeof(tranSeqNb));
    memset(cbuff, 0x00, sizeof(cbuff));
        
    get_zd_data("0580",cbuff );       /* ԭ�������к�        */ 
    zip_space(cbuff);
    memcpy(origDscntInAcctSvcr,cbuff,sizeof(origDscntInAcctSvcr));
    memset(cbuff, 0x00, sizeof(cbuff));    
    get_zd_data("0820",cbuff     );       /* ԭ�������˺�        */ 
    zip_space(cbuff);
    memcpy(origDscntInAcct,cbuff,sizeof(origDscntInAcct));
    memset(cbuff, 0x00, sizeof(cbuff));        
    get_zd_data("0250",origDscntInNm       );       /* ԭ����������        */     
    get_zd_data("0590",cbuff      );       /* (ԭ����+1]�н����к�*/
    zip_space(cbuff);
    memcpy(agtAcctSvcr_1,cbuff,sizeof(agtAcctSvcr_1));
    memset(cbuff, 0x00, sizeof(cbuff));      
    get_zd_data("0610",cbuff);       /* ԭ�������к�        */
    zip_space(cbuff);
    memcpy(origDscntOutAcctSvcr,cbuff,sizeof(origDscntOutAcctSvcr));
    memset(cbuff, 0x00, sizeof(cbuff));      
    get_zd_data("0330",cbuff    );       /* ԭ�������˺�        */   
    zip_space(cbuff);
    memcpy(origDscntOutAcct,cbuff,sizeof(origDscntOutAcct));
    memset(cbuff, 0x00, sizeof(cbuff));    
    get_zd_data("0260",origDscntOutNm      );       /* ԭ����������        */     
    get_zd_data("0600",cbuff       );       /* (ԭ����+1]�н����к�*/ 
    zip_space(cbuff);
    memcpy(agtAcctSvcr_2 ,cbuff,sizeof(agtAcctSvcr_2 ));
    memset(cbuff, 0x00, sizeof(cbuff));      
    get_zd_data("0340",stlFlg              );       /* �����־            */     
    get_zd_data("0670",drftCls             );       /* Ʊ�ݷ���            */     
    get_zd_data("0910",cbuff             );       /* Ʊ������            */
    zip_space(cbuff);
    memcpy(drftTyp ,cbuff,sizeof(drftTyp ));
    memset(cbuff, 0x00, sizeof(cbuff));      
    get_zd_data("0270",cbuff              );       /* Ʊ�ݺ�              */ 
    zip_space(cbuff);
    memcpy(drftNb ,cbuff,sizeof(drftNb ));
    memset(cbuff, 0x00, sizeof(cbuff));     
    get_zd_double("0400",&drftAmt          );       /* Ʊ����            */        
    get_zd_long("0450",&dueDt              );       /* ������              */     
    get_zd_data("0680",intrstTyp           );       /* ��������            */     
    get_zd_double("0840",&intrstRate       );       /* ����                */         
    get_zd_double("0410",&amt              );       /* ʵ�����            */        
    
     
    vtcp_log(" [%s][%d] ����:[%ld]",__FILE__,__LINE__,dt);
    vtcp_log(" [%s][%d] ������ˮ��:[%s] ",__FILE__,__LINE__,tranSeqNb);
    vtcp_log(" [%s][%d] ԭ�������к�:[%s] ",__FILE__,__LINE__,origDscntInAcctSvcr);
    vtcp_log(" [%s][%d] ԭ�������˺�:[%s] ",__FILE__,__LINE__,origDscntInAcct);
    vtcp_log(" [%s][%d] ԭ����������:[%s] ",__FILE__,__LINE__,origDscntInNm);
    vtcp_log(" [%s][%d] ԭ����+1]�н����к�:[%s] ",__FILE__,__LINE__,agtAcctSvcr_1);
    vtcp_log(" [%s][%d] ԭ�������к�:[%s] ",__FILE__,__LINE__,origDscntOutAcctSvcr);
    vtcp_log(" [%s][%d] ԭ�������˺�:[%s] ",__FILE__,__LINE__,origDscntOutAcct);
    vtcp_log(" [%s][%d] ԭ����������:[%s] ",__FILE__,__LINE__,origDscntOutNm);
    vtcp_log(" [%s][%d] ԭ����+1]�н����к�:[%s] ",__FILE__,__LINE__,agtAcctSvcr_2);
    vtcp_log(" [%s][%d] �����־:[%s] ",__FILE__,__LINE__,stlFlg);
                
    vtcp_log(" [%s][%d] Ʊ������:[%s] ",__FILE__,__LINE__,drftTyp);
    vtcp_log(" [%s][%d] Ʊ�ݷ���:[%s] ",__FILE__,__LINE__,drftCls);
     
    vtcp_log(" [%s][%d] Ʊ�ݺ���:[%s] ",__FILE__,__LINE__,drftNb);
    vtcp_log(" [%s][%d] ��������:[%ld] ",__FILE__,__LINE__,dueDt);
    vtcp_log(" [%s][%d] Ʊ����:[%f] ",__FILE__,__LINE__,drftAmt);
    vtcp_log(" [%s][%d] ������:[%ld] ",__FILE__,__LINE__,dueDt);
    vtcp_log(" [%s][%d] ��������:[%s] ",__FILE__,__LINE__,intrstTyp);
    vtcp_log(" [%s][%d] ����:    [%f] ",__FILE__,__LINE__,intrstRate);
    vtcp_log(" [%s][%d] ʵ�����:    [%f] ",__FILE__,__LINE__,amt);
         
    /*-----------�жϸõ�Ʊ�Ƿ��Ѿ������------------*/
    ret=Ecds_recv_reg_Sel(g_pub_tx.reply,&sEcdsRecvReg1,"ecds_tx_date='%ld' and ecds_trace_no='%s'",\
            dt, tranSeqNb);
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
                 tranSeqNb,dt);
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
        
    /* ------------Ecds_recv_reg����׼��-------------------------*/
         sEcds_recv_reg.ecds_tx_date=dt;
         memcpy(sEcds_recv_reg.ecds_trace_no,tranSeqNb,sizeof(sEcds_recv_reg.ecds_trace_no)-1); 
         sEcds_recv_reg.trace_no = g_pub_tx.trace_no;
         sEcds_recv_reg.tx_date = g_pub_tx.tx_date;        
         memcpy(sEcds_recv_reg.drftcls,drftCls,sizeof(sEcds_recv_reg.drfttyp)-1);     
         memcpy(sEcds_recv_reg.drfttyp ,    drftTyp,sizeof(sEcds_recv_reg.drftnb)-1);     
         memcpy(sEcds_recv_reg.drftnb ,drftNb,    sizeof(sEcds_recv_reg.drftnb)-1);   
         sEcds_recv_reg.issedt = 0 ;    /*???*/
         sEcds_recv_reg.duedt= dueDt ;  
         memcpy(sEcds_recv_reg.tag_name,"4959",sizeof( sEcds_recv_reg.tag_name)-1); 
         sEcds_recv_reg.sts[0]='1';   
         sEcds_recv_reg.bf_flag[0]='0'; 
         memcpy(sEcds_recv_reg.check_flag,"2",sizeof(sEcds_recv_reg.check_flag)-1);/*���˱�־*/
         /* ------------Ecds_recv_reg����׼��-------------------------*/
         Ecds_recv_reg_Debug(&sEcds_recv_reg);    
         ret = Ecds_recv_reg_Ins(sEcds_recv_reg,g_pub_tx.reply);
         if(ret )
         {
             sprintf(acErrMsg, "����Ecds_recv_reg��!!ret=[%d]", ret);
             WRITEMSG
             strcpy(g_pub_tx.reply, "0XC8");
             goto ErrExit;
         }
    }
    else
    {
        vtcp_log("��ѯEcds_recv_reg�����!! ret [%d]",ret);
        strcpy ( g_pub_tx.reply , "0T06" );
        goto ErrExit;
    }
     
    
    
    
    /*---------�ж�ת��Ʊ������-------------------*/
    vtcp_log("[%s][%d]--------Ʊ������:[%s]",__FILE__,__LINE__,drftTyp);
    if(!memcmp(drftTyp,"AC01",4))
    {
        memcpy(drftTyp,"111",4);
    }
    else if(!memcmp(drftTyp,"AC02",4))
    {
        memcpy(drftTyp,"112",4);        
    }
    else
    {
        sprintf(acErrMsg, "-Ʊ�����ʹ���!!:%s ",drftTyp);
        WRITEMSG
        strcpy ( g_pub_tx.reply , "0T06" );
        goto ErrExit;
        
    }
    /*-sts??---------���ԭ�ع�ʽ�����ֵĻ�����Ϣ--------------*/
    ret = Mo_discnt_Sel(g_pub_tx.reply,&sMo_discnt,\
        " pnote_no= '%s' and  note_type ='%s' and  pn_type='7' and sts='3' ",\
         drftNb,drftTyp);     
    if (ret)
    {
        sprintf(acErrMsg,"��ѯmo_discnt�����!!ret= [%d]",ret);
        WRITEMSG
        strcpy ( g_pub_tx.reply , "B174" );
        goto ErrExit;
    }
    /*****���Ҹ����˻���Ϣ*****/
    ret = Com_parm_Sel ( g_pub_tx.reply,&sCom_parm,"val_type='5' and  parm_seqn= %d and parm_code='%s' ",PARM_ECDS_TR_PAY_AC,sMo_discnt.br_no);
    if(ret)
    {
        sprintf(acErrMsg, "��ѯCom_parm�����!!ret  [%s]",ret);
        WRITEMSG
        strcpy ( g_pub_tx.reply , "AT06" );
        goto ErrExit;
    }
    memset(&sMdm_ac_rel , 0 , sizeof(sMdm_ac_rel));
    ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdm_ac_rel, "ac_no ='%s' ", sCom_parm.val);
    if (ret == 100) {
        sprintf(acErrMsg, "�����˺Ŷ��ձ��в����ڴ��˺�!");
        WRITEMSG
        strcpy(g_pub_tx.reply, "W010");
        goto ErrExit;
    } else if (ret) {
        sprintf(acErrMsg, "ִ��Mdm_ac_rel_Sel��![%d]", ret);
        WRITEMSG
         strcpy(g_pub_tx.reply, "W010");
        goto ErrExit;
    }
    vtcp_log(" [%s][%d]  sMdm_ac_rel.note_sts:[%s] ",__FILE__,__LINE__,sMdm_ac_rel.note_sts);
    /*****���Ӷ��ڲ��ʵļ��,�˻�״̬�����  TODO *****/
    if(sMdm_ac_rel.note_sts[0]!='0')
    {
    	sprintf(acErrMsg, " ����״̬������!sMdm_ac_rel.note_sts=[%s]", sMdm_ac_rel.note_sts);
        WRITEMSG
        strcpy(g_pub_tx.reply, "W010");
        goto ErrExit;
    	
    }
    vtcp_log(" [%s][%d]  sMdm_ac_rel.note_sts:[%s] ",__FILE__,__LINE__,sMdm_ac_rel.note_sts);
   
    ret =In_mst_Sel(g_pub_tx.reply, &sIn_mst, "ac_id ='%ld' and ac_seqn='%d' ", sMdm_ac_rel.ac_id,sMdm_ac_rel.ac_seqn);
    if (ret == 100) {
        sprintf(acErrMsg, "In_mst�в����ڴ�ac_id:%ld!",sMdm_ac_rel.ac_id);
        WRITEMSG
        strcpy(g_pub_tx.reply, "B124");
        goto ErrExit;
    } else if (ret) {
        sprintf(acErrMsg, "ִ��In_mst_Sel��![%d]", ret);
        WRITEMSG
        strcpy(g_pub_tx.reply, "B124");
        goto ErrExit;
    }
    if(sIn_mst.sts[0]!='1')
    {
    	sprintf(acErrMsg, " �ڲ��ʺ�״̬������!sLn_mst.sts=[%s]", sIn_mst.sts);
        WRITEMSG
        strcpy(g_pub_tx.reply, "W010");
        goto ErrExit;    	
    }
    vtcp_log("[%s][%d]--- amt[%f].sLn_mst.bal=[%f]", __FILE__,__LINE__,amt,sIn_mst.bal);
    if(amt>sIn_mst.bal)
    {
    	sprintf(acErrMsg, " ����! ��");
        WRITEMSG
        strcpy(g_pub_tx.reply, "W010");
        goto ErrExit;   
    	
    }
    
    
    vtcp_log("[%s][%d]--- sCom_parm.val=%s", __FILE__,__LINE__,sCom_parm.val);
    vtcp_log("[%s][%d]--- ������Э����=%s", __FILE__,__LINE__,sMo_discnt.rpact_no);
        
    vtcp_log("[%s][%d] rpact_no=[%s] val=[%s]", __FILE__,__LINE__,sMo_discnt.rpact_no,sCom_parm.val);
    set_zd_data("0330",sMdm_ac_rel.ac_no);
    set_zd_data("0620", sMo_discnt.rpact_no);    /* ������Э���� */
    set_zd_data("0300", sCom_parm.val);          /* ����׼�����˺� */
    set_zd_data("0720", "2");                    /* �����տ� 1: ����  2: δ��*/
    set_zd_double("0840",intrstRate);            /* �������� */
    vtcp_log("[%s][%d]----B945����׼���� ", __FILE__,__LINE__);
    /* ---------------------------------------*/
GoodExit1:
    strcpy(g_pub_tx.reply, "0001");
    sprintf(acErrMsg, "Before OK return: reply [%s]", g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 0;    
GoodExit:
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
    	 strcpy(g_pub_tx.reply, "B559");   	
    }    	
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 1;
}
