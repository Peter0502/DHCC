/************************************************************************************
* �� �� ��:  spB558.c
* ���������� ������֪ͨ
*            ��ƽ̨�õ�������Ϣ��Ϊ�����ֵ�¼�����׼������.
* ��    ��: yindianheng
* ������ڣ� 2010��4��10��
*
* �޸ļ�¼��
*   ��   ��:
*   �� �� ��: xyy
*   �޸�����: 2010-4-27 22:45:44 

insert into tx_def values ('4958','������֪ͨ','10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000010000000000000000000000000100','1','3','2','3',null)

insert into tx_sub_def values ('B558','������֪ͨ','spB558','0','0')

insert into tx_flow_def   values ('4958','0','B558','#=0001@1@#=0002@5@#$')
insert into tx_flow_def   values ('4958','1','B943','#=0000@2@#$')
insert into tx_flow_def   values ('4958','2','B944','#=0000@4@#$')
----insert into tx_flow_def   values ('4958','3','A017','#=0000@4@#$')
insert into tx_flow_def   values ('4958','4','A016','#$')

insert into tx_flow_def   values ('4958','5','B943','#=0000@6@#$')
insert into tx_flow_def   values ('4958', 6, 'L201', '#=0000@7@#$');
insert into tx_flow_def   values ('4958','7','B944','#=0000@8@#$');
insert into tx_flow_def   values ('4958', 8, 'a016', '#=0000@9@#$');
insert into tx_flow_def   values ('4958', 9, 'L320', '#=0001@10@#$');
insert into tx_flow_def   values ('4958', 10, 'b016', '#=0000@11@#$');
insert into tx_flow_def   values ('4958', 11, 'a017', '#$');

insert into tx_sub_rel  values ('4958', 'a016', '0', '112A', '1201', '�ʺ�');
insert into tx_sub_rel  values ('4958', 'a016', '1', '01', '1204', '����');
insert into tx_sub_rel  values ('4958', 'a016', '1', '2', '1205', 'ת��');
insert into tx_sub_rel  values ('4958', 'a016', '0', '1002-1001', '1208', '������');

insert into tx_sub_rel  values ('4958', 'L320', '0', '0630', '0620', null);

insert into tx_sub_rel  values ('4958', 'L201', '1', '01', '0210', null);
insert into tx_sub_rel  values ('4958', 'L201', '1', '9', '0660', null);
insert into tx_sub_rel  values ('4958', 'L201', '0', '0330', '1071', null);
insert into tx_sub_rel  values ('4958', 'L201', '1', '0', '1072', null);
insert into tx_sub_rel  values ('4958', 'L201', '1', '1', '107A', null);
insert into tx_sub_rel  values ('4958', 'L201', '0', '1002', '107B', null);


insert into tx_sub_rel values ('4958','B944','0','0700','0670','')

insert into tx_sub_rel values ('4958','A016','0','0300','1201','�ʺ�')
insert into tx_sub_rel values ('4958','A016','1','01','1204','����')
insert into tx_sub_rel values ('4958','A016','1','2','1205','��ת')
insert into tx_sub_rel values ('4958','A016','0','1003','1208','���')
insert into tx_sub_rel values ('4958','A016','1','��Ʊ�����ֽ����','120A','����')

----insert into tx_sub_rel values ('4958','A017','1','202201','1211','�ʻ�')
----insert into tx_sub_rel values ('4958','A017','1','2','1215','��ת')
----insert into tx_sub_rel values ('4958','A017','0','1002','1218','���')
----insert into tx_sub_rel values ('4958','A017','1','��Ʊ�����ִ�����','121A','����')
**************************************************************************************************/
#define EXTERN
#include "public.h" 
#include <string.h>
#include "mo_discnt_c.h"
#include "ecds_recv_reg_c.h"
#include "ln_mst_c.h"
#include "hv_orno_brno_c.h"
#include "mdm_ac_rel_c.h"
#include "com_parm_c.h"

int 
spB558()
{ 
    /******����ƽ̨�����ı���******/
    
    int         ret = 0;
    long        dt=0;                   /*����  */
    long        dueDt=0;                /*������*/
    
    char        cKinbr[BRNO_LEN+1];
    char        stlFlg[4+1];            /*�����־*/
    char        drftCls[1+1];           /*Ʊ�ݷ���*/
    char        drftTyp[4+1];           /*Ʊ������*/
    char        drftNb[30+1];           /*Ʊ �� ��*/
    char        dscntTyp[4+1];          /*��������*/
    char        intrstTyp[1+1];         /*��������*/
    char        cRpact_no[20+1];        /*�����ֺ���*/
    char        tranSeqNb[28+1];        /*������ˮ��*/
    
    char        dscntInNm[60+1];        /*����������*/
    char        dscntOutNm[60+1];       /*����������*/
    char        dscntOutAcct[32+1];     /*�������˺�*/
    char        dscntInAcct[32+1];      /*�������˺�*/
    char        dscntOutAcctSvcr[12+1]; /*�������к�*/
    char        dscntInAcctSvcr[12+1];  /*�������к�*/
    char        agtAcctSvcr_1[12+1];    /*(����)�н����к�*/
    
    char        cbuff[61];
    double      intrstRate=0;         /*����*/
    double      amt=0.00;             /*ʵ�����*/
    double      drftAmt=0.00;         /*Ʊ����*/
    
    struct com_parm_c         sCom_parm;
    struct mo_discnt_c        sMo_discnt;
    struct mdm_ac_rel_c       sMdm_ac_rel;
    struct ecds_recv_reg_c    sEcds_recv_reg;   
    struct ecds_recv_reg_c    sEcdsRecvReg1;
    
    vtcp_log("[%s][%d] define  Variable!!!", __FILE__,__LINE__); 
     
    /*******��ʼ�������ͽṹ��******/
    memset(&sCom_parm     , 0 , sizeof(struct com_parm_c));
    memset(&sMo_discnt    , 0 , sizeof(struct mo_discnt_c));
    memset(&sMdm_ac_rel   , 0 , sizeof(struct mdm_ac_rel_c));
    memset(&sEcdsRecvReg1 , 0 , sizeof(struct ecds_recv_reg_c));
    memset(&sEcds_recv_reg, 0 , sizeof(struct ecds_recv_reg_c));
    
    memset ( cKinbr             , 0  ,sizeof( cKinbr           ));
    memset ( tranSeqNb          ,0x00,sizeof( tranSeqNb        )); 
    memset ( dscntTyp           ,0x00,sizeof( dscntTyp         )); 
    memset ( dscntInAcctSvcr    ,0x00,sizeof( dscntInAcctSvcr  )); 
    memset ( dscntInAcct        ,0x00,sizeof( dscntInAcct      )); 
    memset ( dscntInNm          ,0x00,sizeof( dscntInNm        )); 
    memset ( dscntOutAcctSvcr   ,0x00,sizeof( dscntOutAcctSvcr )); 
    memset ( dscntOutAcct       ,0x00,sizeof( dscntOutAcct     )); 
    memset ( dscntOutNm         ,0x00,sizeof( dscntOutNm       )); 
    memset ( agtAcctSvcr_1      ,0x00,sizeof( agtAcctSvcr_1    )); 
    memset ( stlFlg             ,0x00,sizeof( stlFlg           )); 
    memset ( drftCls            ,0x00,sizeof( drftCls          )); 
    memset ( drftTyp            ,0x00,sizeof( drftTyp          )); 
    memset ( drftNb             ,0x00,sizeof( drftNb           )); 
    memset ( intrstTyp          ,0x00,sizeof( intrstTyp        )); 
    memset ( cRpact_no		,0X00,sizeof( cRpact_no	       ));
    memset(cbuff, 0x00, sizeof(cbuff)); 
    
    pub_base_sysinit();    
    vtcp_log("[%s][%d] ��Ʊҵ���������֪ͨ����!",__FILE__,__LINE__);
    
    
  
    /**********************���յı�������*************************/
    get_zd_data ( "0810",      cbuff       );     /*������ˮ��*/   
    zip_space(cbuff);
    memcpy(tranSeqNb,cbuff,sizeof(tranSeqNb));
    memset(cbuff, 0x00, sizeof(cbuff));        
               
    get_zd_data ( "0580",      cbuff );     /*�������к�*/ 
    zip_space(cbuff);
    memcpy(dscntInAcctSvcr,cbuff,sizeof(dscntInAcctSvcr));
    memset(cbuff, 0x00, sizeof(cbuff));              
    get_zd_data ( "0820",      cbuff     );     /*�������˺�*/
    zip_space(cbuff);
    memcpy(dscntInAcct,cbuff,sizeof(dscntInAcct));
    memset(cbuff, 0x00, sizeof(cbuff));               
    
    vtcp_log("[%s][%d] tranSeqNb=[%s] ",__FILE__,__LINE__,tranSeqNb);
    vtcp_log("[%s][%d] dscntInAcctSvcr=[%s] dscntInAcct=[%s]",__FILE__,__LINE__,dscntInAcctSvcr,dscntInAcct);
    
    get_zd_data ( "0250",      dscntInNm       );     /*����������*/           
    get_zd_data ( "0590",      cbuff);     /*�������к�*/ 
    zip_space(cbuff);
    memcpy(dscntOutAcctSvcr,cbuff,sizeof(dscntOutAcctSvcr));
    memset(cbuff, 0x00, sizeof(cbuff));              
    get_zd_data ( "0330",      cbuff    );     /*�������˺�*/
    zip_space(cbuff);
    memcpy(dscntOutAcct,cbuff,sizeof(dscntOutAcct));
    memset(cbuff, 0x00, sizeof(cbuff));               
    get_zd_data ( "0260",      dscntOutNm      );     /*����������*/
    
    vtcp_log("[%s][%d] dscntInNm=[%s] dscntOutAcctSvcr=[%s]",__FILE__,__LINE__,dscntInNm,dscntOutAcctSvcr);
    vtcp_log("[%s][%d] dscntOutAcct=[%s] dscntOutNm=[%s]",__FILE__,__LINE__,dscntOutAcct,dscntOutNm);
    
    
    get_zd_data ( "0600",      cbuff   );     /*(����)�н����к�*/
    zip_space(cbuff);
    memcpy(agtAcctSvcr_1,cbuff,sizeof(agtAcctSvcr_1));
    memset(cbuff, 0x00, sizeof(cbuff));       
    get_zd_data ( "0340",      stlFlg          );     /*�����־*/             
    get_zd_data ( "0670",      drftCls         );     /*Ʊ�ݷ���*/             
    get_zd_data ( "0910",      cbuff         );     /*Ʊ������*/
    zip_space(cbuff);
    memcpy(drftTyp,cbuff,sizeof(drftTyp));
    memset(cbuff, 0x00, sizeof(cbuff));         
    
    vtcp_log("[%s][%d] agtAcctSvcr_1=[%s] stlFlg=[%s]",__FILE__,__LINE__,agtAcctSvcr_1,stlFlg);
    vtcp_log("[%s][%d] drftCls=[%s] drftTyp=[%s]",__FILE__,__LINE__,drftCls,drftTyp);   
    
    get_zd_data ( "0680",      intrstTyp       );     /*��������*/             
    get_zd_data ( "0270",      drftNb          );     /*Ʊ�ݺ�*/
    
    vtcp_log("[%s][%d] intrstTyp=[%s] drftNb=[%s]",__FILE__,__LINE__,intrstTyp,drftNb);
    get_zd_data ( "0640",      cbuff        );     /*��������*/ 
    zip_space(cbuff);
    memcpy(dscntTyp,cbuff,sizeof(dscntTyp));
    memset(cbuff, 0x00, sizeof(cbuff));  
    vtcp_log("[%s][%d] �������� dscntTyp=[%s]",__FILE__,__LINE__,dscntTyp);
    
    get_zd_long ( "0460",      &dt             );     /*����*/
    get_zd_long ( "0450",      &dueDt          );     /*������*/
    get_zd_double ( "0400",    &drftAmt        );     /*Ʊ����*/                    
    get_zd_double ( "0410",    &amt            );     /*ʵ�����*/
    get_zd_double ( "0840",    &intrstRate     );     /*����*/ 
    vtcp_log("[%s][%d] ���� intrstRate=[%f]",__FILE__,__LINE__,intrstRate);
    
    vtcp_log("[%s][%d] dt=[%ld] dueDt=[%ld]",__FILE__,__LINE__,dt,dueDt);
    vtcp_log("[%s][%d] drftAmt=[%f] amt=[%f]",__FILE__,__LINE__,drftAmt,amt);
    vtcp_log("[%s][%d] �������� dscntTyp=[%s]",__FILE__,__LINE__,dscntTyp);
    vtcp_log("[%s][%d] ���� intrstRate=[%f]",__FILE__,__LINE__,intrstRate);
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
        vtcp_log("[%s][%d] ret=[%d] Ecds_recv_regû�в鵽��Ϣ,insert",__FILE__,__LINE__,ret);
         
      /*-----------Ecds_recv_reg_Ins------------*/
        sEcds_recv_reg.ecds_tx_date     = dt;                    /**��������**/
        sEcds_recv_reg.trace_no         = g_pub_tx.trace_no;     /**������ˮ**/
        sEcds_recv_reg.tx_date          = g_pub_tx.tx_date;      /**��������**/
        sEcds_recv_reg.issedt           = 0  ;                   /**��Ʊ��,���ù���**/
        sEcds_recv_reg.duedt            = dueDt ;                /**��Ʊ������**/
        
        sEcds_recv_reg.sts[0]='1';   /**1-�������ռ���  2-����������  **/
        sEcds_recv_reg.bf_flag[0]='0';
        memcpy(sEcds_recv_reg.drftcls ,    drftCls,sizeof(sEcds_recv_reg.drftcls)-1);  /**E-��Ʊ F-ֽƱ **/   
        memcpy(sEcds_recv_reg.drfttyp ,    drftTyp,sizeof(sEcds_recv_reg.drfttyp)-1);   /**AC01-���� AC01-�̳�**/
        memcpy(sEcds_recv_reg.drftnb  ,    drftNb ,sizeof(sEcds_recv_reg.drftnb)-1);   /*Ʊ�ݺ�*/
        memcpy(sEcds_recv_reg.tag_name,    "CBS1000100",sizeof( sEcds_recv_reg.tag_name)-1); /**������֪ͨ**/
        memcpy(sEcds_recv_reg.check_flag,"2",sizeof(sEcds_recv_reg.check_flag)-1);     /*���˱�־*/
        memcpy(sEcds_recv_reg.ecds_trace_no,tranSeqNb , sizeof(sEcds_recv_reg.ecds_trace_no)-1);/*������ˮ��*/ 
        
        Ecds_recv_reg_Debug(&sEcds_recv_reg);
        ret = Ecds_recv_reg_Ins(sEcds_recv_reg,g_pub_tx.reply);
        if(ret )
        {
            sprintf(acErrMsg, "����Ecds_recv_reg��!!ret=[%d]", ret);
            WRITEMSG
            strcpy(g_pub_tx.reply, "B174");
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
   
    /*---------�ж�ת��Ʊ������-------------------*/
    vtcp_log("[%s][%d]--------Ʊ������:[%s]",__FILE__,__LINE__,sEcds_recv_reg.drfttyp);
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
        sprintf(acErrMsg, "---Ʊ�����ʹ���!! ");
        WRITEMSG
        strcpy ( g_pub_tx.reply , "AT06" );
        goto ErrExit;
        
    }
    /*--------------------��ѯԭ����Ʊ�ݵĻ�����Ϣ-----------------------*/
    ret = Mo_discnt_Sel(g_pub_tx.reply,&sMo_discnt," pnote_no= '%s' and  note_type ='%s' and sts='3' \
        and pn_type in ('1','2','3') ", drftNb,drftTyp);
    if (ret)
    {
        sprintf(acErrMsg,"��ѯmo_discnt�����!!ret= [%d]",ret);
        WRITEMSG
        strcpy ( g_pub_tx.reply , "B112" );
        goto ErrExit;
    }
    /**************У��ƽ̨������������*****************/
      
     vtcp_log("[%s][%d]  ���ݿ�:[%f]ƽ̨:[%f]",__FILE__,__LINE__,sMo_discnt.par_amt,drftAmt);
     
    if(pub_base_CompDblVal(drftAmt,sMo_discnt.par_amt) != 0)
    {
        vtcp_log("[%s][%d] sMo_discnt.par_amt=[%f] drftAmt =[%f]",__FILE__,__LINE__,sMo_discnt.par_amt,drftAmt);
        sprintf(acErrMsg,"���ƽ̨������Ʊ�������!");
        WRITEMSG
        strcpy ( g_pub_tx.reply , "B103" );
        goto ErrExit;
    } 
    vtcp_log("[%s][%d]  ���ݿ�--:[%f]ƽ̨:[%f]",__FILE__,__LINE__,sMo_discnt.par_amt,drftAmt);
     
    if(dueDt != sMo_discnt.not_matr_date)
    {
        vtcp_log("[%s][%d] sMo_discnt.not_matr_date=[%ld] duedt =[%ld]",__FILE__,__LINE__,sMo_discnt.not_matr_date,dueDt);
        sprintf(acErrMsg,"���ƽ̨�����Ļ�Ʊ�����մ���!");
        WRITEMSG
        strcpy ( g_pub_tx.reply , "B094" );
        goto ErrExit;
    }
    
    GetBr_noByOr_br_no(dscntOutAcctSvcr,cKinbr);
    vtcp_log("[%s][%d] cKinbr=[%s][%s] ",__FILE__,__LINE__,cKinbr,sMo_discnt.br_no);

    if(memcmp(cKinbr,sMo_discnt.br_no,BRNO_LEN)!=0)
    {
        sprintf(acErrMsg,"������Ʊ��=[%s][%s]",cKinbr,sMo_discnt.br_no);
        WRITEMSG
        strcpy ( g_pub_tx.reply , "Z10D" );
        goto ErrExit;
    }
    
    /***���ÿ��������ͽ��׻���***/
    set_zd_data("0020" , cKinbr);
    set_zd_data("0030" , cKinbr);
    
    vtcp_log("[%s][%d] ע������������:[%ld][%ld]",__FILE__,__LINE__,sMo_discnt.not_matr_date,dueDt);
    
  
    vtcp_log("[%s][%d] Ϊ�����ֵ�¼��ͼ�������׼��![%s]",__FILE__,__LINE__,dscntTyp);
    set_zd_data("0640",dscntTyp);/**RM00:���ʽ������  RM01:�ع�ʽ������**/
    
    /********************��α���һ��������Э����************************/
    char cTmpBuff[9];
    char cTraceNo[10];
    memset(cTmpBuff , 0 , sizeof(cTmpBuff));
    sprintf(cTmpBuff ,"%08d" , g_pub_tx.tx_date);
    vtcp_log("[%s][%d] cTmpBuff=[%s] ",__FILE__,__LINE__,cTmpBuff);
    
    sprintf(cRpact_no ,"%08d" , g_pub_tx.tx_date);
    memcpy(cRpact_no , cTmpBuff+2 , 6);
    vtcp_log("[%s][%d] cRpact_no=[%s] ",__FILE__,__LINE__,cRpact_no);
    strcat(cRpact_no , sMo_discnt.br_no );
    vtcp_log("[%s][%d] cRpact_no=[%s] trace_no=[%ld]",__FILE__,__LINE__,cRpact_no,g_pub_tx.trace_no);
    
    sprintf(cTraceNo,"%09d",g_pub_tx.trace_no);
    strcat(cRpact_no , cTraceNo );
    vtcp_log("[%s][%d]������Э����=[%s]", __FILE__,__LINE__,cRpact_no);

    /******************TODO ����Ҫ����һ�����ĸ��������������*****************/
    ret=Com_parm_Sel(g_pub_tx.reply,&sCom_parm, "parm_code ='%s' and parm_seqn = %d ",sMo_discnt.br_no,PARM_ECDS_TR_PAY_AC);
    if(ret){
         sprintf(acErrMsg,"��ѯ������������![%d]",__FILE__,__LINE__,ret);
         WRITEMSG
         strcpy(g_pub_tx.reply,"D101");
         goto ErrExit;
    }
    vtcp_log("[%s][%d] �����ֵļ����˺�=[%s]",__FILE__,__LINE__,sCom_parm.val);

    set_zd_data("0670", "0");                       /*  sp943.c Ϊ�������ͣ�0 ¼�� 1 �޸� */

    set_zd_data("0620", cRpact_no);                  /* ������Э���� */
    set_zd_data("0700","1");                         /* sp944.c���ʾ�տ��˻�����----tx_sub_rel��������1  TODO   */
    set_zd_double("1002", drftAmt);                  /* ��Ʊ���   */
    set_zd_double("1001", drftAmt - amt);            /* ��������Ϣ=��Ʊ���-ʵ�����s */
    set_zd_double("0840", intrstRate);               /* ��������   */
    set_zd_data("0250", dscntInNm);                  /* ���������� */
    
    set_zd_data( DC_OPN_BR_NO , sMo_discnt.br_no);/*������������ΪMo_discnt������*/   
    /***********���������ֶ���,�ع����ڲ���***********/
    set_zd_long("0480",  0 );                       /* ��Ʊ��;����Ϊ0 */
    set_zd_long("0460",  0 );                       /* �ع����� */     
    
    /*****************************���²�����ԭƱ����Ϣ***********************************/
    set_zd_long("0450", dueDt);                      /* ��Ʊ������ */
    set_zd_data("0310", sMo_discnt.rem_ac_no);       /* ��Ʊ���ʺ� */
    set_zd_data("0260", sMo_discnt.remitter);        /* ��Ʊ��ȫ�� */
    
    set_zd_data("0810", sMo_discnt.payer_brno_name); /* ������ȫ�� */
    set_zd_data("0320", sMo_discnt.payee_ac_no);     /* �տ����ʺ� */
    set_zd_data("0270", sMo_discnt.payee);           /* �տ���ȫ�� */
    set_zd_data("0820", sMo_discnt.payee_brno_name); /* �տ��˿�����ȫ�� */
    set_zd_data("0900", drftTyp);                    /* Ʊ������ */
    set_zd_data("0750", sMo_discnt.pnote_no);        /* Ʊ�ݺ�   */
    
    /************************��Ϊ���ʽ������Ҫ����ԭ�����ʲ�*************************/

    vtcp_log("[%s][%d] dscntTyp=[%s]",__FILE__,__LINE__,dscntTyp);
    if(memcmp(dscntTyp,"RM01",4)==0)/**�ع�ʽ**/
    {
        /***********ΪA016׼������***********/
        set_zd_data("0300", sCom_parm.val);              /* �跽�˻� */
        vtcp_log("[%s][%d] �跽�˻�=[%s]", __FILE__,__LINE__,sCom_parm.val);
        strcpy ( g_pub_tx.reply , "0001" );
    }
    else if (memcmp(dscntTyp,"RM00",4)==0)/**���ʽ**/
    {
        vtcp_log("[%s][%d] ����ac_id=[%ld]",__FILE__,__LINE__,sMo_discnt.ac_id);
        ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdm_ac_rel, " ac_id=%ld ", sMo_discnt.ac_id);
        if (ret) {
           sprintf(acErrMsg, "��ѯmdm_ac_rel����![%s]", sMo_discnt.ac_id);
           WRITEMSG
           strcpy(g_pub_tx.reply, "L015");
           goto ErrExit;
        }
        /***********L320**********/
        set_zd_data("0630" , sMo_discnt.pact_no);
        /***********a016**********/
        set_zd_data("112A", sCom_parm.val);              /* �跽�˻� */
        vtcp_log("[%s][%d] �跽�˻�=[%s]", __FILE__,__LINE__,sCom_parm.val);
        set_zd_double("100E", drftAmt);                  /* ��Ʊ���   */
        /************L201***********/
        set_zd_data("0330",sMdm_ac_rel.ac_no);
        vtcp_log("[%s][%d] �����˻�=[%s]",__FILE__,__LINE__,sMdm_ac_rel.ac_no);
        strcpy ( g_pub_tx.reply , "0002" ); 
    }
    else
    {
        sprintf(acErrMsg,"����,���������ʹ���!! [%s] ",dscntTyp);
        WRITEMSG 
        goto ErrExit; 
    }
     
    vtcp_log("[%s][%d] ��ƽ̨�����������[%s]!!",__FILE__,__LINE__,sMo_discnt.br_no);

OkExit:
    sprintf(acErrMsg, "Before OK return: reply [%s]", g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 0;
ErrExit:
    sprintf(acErrMsg, "Before return: reply is[%s]", g_pub_tx.reply);
    WRITEMSG
    if(!memcmp(g_pub_tx.reply,"0000",4))
    {
    	 strcpy(g_pub_tx.reply, "B558");   	
    }    	
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 1;
}
