/*************************************************
* �� �� ��:  spB556.c
*    ת����֪ͨ
*    ��Ϊ�����У��յ�ת����֪ͨ��Ҫ����ԭ�����ʲ����������ָ�ծ��
*    ��Ϊ�����У��յ�ת����֪ͨ��Ҫ���������ʲ�������ת���ֵǼǣ�
*    ����ת�������Ŵ�ϵͳ���𣬺���ϵͳ��˺�������������Ϣ��״̬��
* ��    ��:  chenchao
* ������ڣ� 2010-4-9 20:30:43
*
*  
* �޸ļ�¼��
*   ��    ��:
*   �� �� ��: xyy
*   �޸�����:
*   ע    �⣺ ũ�������ĵ�ͬ־˵���������ֻ������������ģ���ô��˵�����ܴ���ͬ�������ĵ�ҵ��xyy
insert into tx_def values ('4956','ת����֪ͨ','10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000010000000000000000000000000100','1','3','2','3',null)
insert into tx_flow_def  values ('4956',0,'B556','#=0001@1@#=0002@14@#=0003@15@#$');
insert into tx_flow_def  values ('4956',1,'B921','#=0000@2@#$');
insert into tx_flow_def  values ('4956',2,'B922','#=0000@5@#$');

insert into tx_flow_def  values ('4956', 3, 'L201', '#=0000@7@#$');
insert into tx_flow_def  values ('4956', 4, 'B947', '#=0000@12@#$');

insert into tx_flow_def  values ('4956', 5, 'L104', '#=0002@6@#$');
insert into tx_flow_def  values ('4956', 6, 'a017', '#$');

insert into tx_flow_def  values ('4956', 7,  'B932', '#=0000@8@#$');
insert into tx_flow_def  values ('4956', 8,  'a016', '#=0000@9@#$');
insert into tx_flow_def  values ('4956', 9,  'L320', '#=0001@10@#$');
insert into tx_flow_def  values ('4956', 10, 'A016', '#=0000@11@#$');
insert into tx_flow_def  values ('4956', 11, 'A017', '#$');

insert into tx_flow_def  values ('4956', 12, 'b017', '#=0000@13@#$');
insert into tx_flow_def  values ('4956', 13, 'b016', '#$');

insert into tx_flow_def  values ('4956', 14, 'B931', '#=0000@3@#$');
insert into tx_flow_def  values ('4956', 15, 'B931', '#=0000@4@#$');


insert into tx_sub_def values ('B556','ת����֪ͨ','spB556','0','0')

insert into tx_sub_rel  values ('4956', 'L104', '1', '0', '0340', null);
insert into tx_sub_rel  values ('4956', 'L104', '0', '0330', '0370', null);
insert into tx_sub_rel  values ('4956', 'L104', '0', '1002', '0390', null);
insert into tx_sub_rel  values ('4956', 'L104', '1', '2', '0660', null);

insert into tx_sub_rel  values ('4956', 'a016', '0', '112A', '1201', '�ʺ�');
insert into tx_sub_rel  values ('4956', 'a016', '1', '01', '1204', '����');
insert into tx_sub_rel  values ('4956', 'a016', '1', '2', '1205', 'ת��');
insert into tx_sub_rel  values ('4956', 'a016', '0', '1002-1001', '1208', '������');

insert into tx_sub_rel  values ('4956', 'a017', '0', '0300', '1211', 'ͬ�����');
insert into tx_sub_rel  values ('4956', 'a017', '1', '01', '1214', '����');
insert into tx_sub_rel  values ('4956', 'a017', '1', '2', '1215', 'ת��');
insert into tx_sub_rel  values ('4956', 'a017', '0', '0890', '1216', 'ƾ֤����');
insert into tx_sub_rel  values ('4956', 'a017', '0', '0580', '1217', 'ƾ֤����');
insert into tx_sub_rel  values ('4956', 'a017', '0', '1002-1001', '1218', '���');
insert into tx_sub_rel  values ('4956', 'a017', '1', 'ͬ�����', '121A', 'ժҪ');

insert into tx_sub_rel  values ('4956', 'L201', '1', '01', '0210', null);
insert into tx_sub_rel  values ('4956', 'L201', '1', '9', '0660', null);
insert into tx_sub_rel  values ('4956', 'L201', '0', '0330', '1071', null);
insert into tx_sub_rel  values ('4956', 'L201', '1', '0', '1072', null);
insert into tx_sub_rel  values ('4956', 'L201', '1', '1', '107A', null);
insert into tx_sub_rel  values ('4956', 'L201', '0', '1002', '107B', null);

insert into tx_sub_rel  values ('4956', 'b016', '0', '0300', '1201', '�ʺ�');
insert into tx_sub_rel  values ('4956', 'b016', '1', '01', '1204', '����');
insert into tx_sub_rel  values ('4956', 'b016', '1', '2', '1205', '��ת');
insert into tx_sub_rel  values ('4956', 'b016', '0', '1003', '1208', '���');
insert into tx_sub_rel  values ('4956', 'b016', '1', '��׷��Ȩת���ּ���', '120A', '����');

insert into tx_sub_rel  values ('4956', 'b017', '1', '202101', '1211', '�ʻ�');
insert into tx_sub_rel  values ('4956', 'b017', '1', '2', '1215', '��ת');
insert into tx_sub_rel  values ('4956', 'b017', '0', '1002', '1218', '���');
insert into tx_sub_rel  values ('4956', 'b017', '1', '��׷��Ȩת���ּ���', '121A', '����');
*************************************************/
#define ERR_DEAL if( ret ) \
{\
    sprintf( acErrMsg, "sql error" ); \
        WRITEMSG \
        goto ErrExit; \
}
#define EXTERN
#include "public.h" 
#include "hv_define.h"
#include "mo_discnt_c.h"
#include "ecds_recv_reg_c.h"
#include "ln_mst_c.h"
#include "mdm_ac_rel_c.h"
#include "com_parm_c.h"
#include "hv_uniontab_c.h"

int 
spB556() 
{ 
    struct ecds_recv_reg_c    sEcds_recv_reg,s_Ecds_recv_reg;   
    struct mo_discnt_c    sMo_discnt; 
    struct com_parm_c    sCom_parm;
    struct mdm_ac_rel_c   sMdm_ac_rel;
    struct hv_uniontab_c     sHv_uniontab1 , sHv_uniontab2;

    /******************��������ͻ�����***************/
    int        ttlnum = 0;            /**��ȡ��������**/
    int        cnt=0;
    int        i = 0;
    int        ret = 0;
    
    long        dt=0;                /*����*/
    long        dueDt=0;             /*������*/
    long        l_date=0;
    char        cTmpBuff[101];
    char        cTpact_no[20+1];        /*ת���ֺ���*/
    char        cKinbr[BRNO_LEN+1];      
    char        stlFlg[4+1];             /*�����־*/
    char        drftCls[1+1];            /*Ʊ�ݷ���*/
    char        drftTyp[4+1];            /*Ʊ������*/
    char        drftNb[30+1];            /*Ʊ�ݺ�  */
    char        drwrAcct[32+1];          /*��Ʊ���˺�*/
    char        drwrNm[60+1];            /*��Ʊ������*/  
    char        tranSeqNb[28+1];         /*������ˮ��*/
    char        intrstTyp[1+1];          /*��������*/
    char        dscntTyp[4+1];           /*��������*/
    char        dscntInNm[60+1];         /*����������*/
    char        dscntOutNm[60+1];        /*����������*/
    char        dscntInAcctSvcr[12+1];   /*�������к�*/
    char        dscntInAcct[32+1];       /*�������˺�*/
    char        dscntOutAcctSvcr[12+1];  /*�������к�*/
    char        dscntOutAcct[32+1];      /*�������˺�*/
    char        agtAcctSvcr_1[12+1];     /*(����)�н����к�*/
    char    lw_ind[2]; 
    double      drftAmt=0.00;            /*Ʊ����*/
    double      intrstRate=0.00;         /*����    */
    double      amt=0.00;                /*ʵ�����*/
    
    vtcp_log("[%s][%d]", __FILE__,__LINE__);  
    
    memset(&sMo_discnt    , 0 , sizeof(struct mo_discnt_c));
    memset(&sCom_parm     , 0 , sizeof(struct com_parm_c));
    memset(&sMdm_ac_rel   , 0 , sizeof(struct mdm_ac_rel_c));
    memset(&sHv_uniontab1 , 0 , sizeof(struct hv_uniontab_c));
    memset(&sHv_uniontab2 , 0 , sizeof(struct hv_uniontab_c));
    memset(&sEcds_recv_reg, 0 , sizeof(struct ecds_recv_reg_c));
    memset(&s_Ecds_recv_reg, 0 , sizeof(struct ecds_recv_reg_c));
    memset(lw_ind,0x00,sizeof(lw_ind));
    memset ( cKinbr             ,0 ,sizeof(cKinbr            ));
    memset ( stlFlg             ,0 ,sizeof( stlFlg           )); 
    memset ( drftCls            ,0 ,sizeof( drftCls          )); 
    memset ( drftTyp            ,0 ,sizeof( drftTyp          )); 
    memset ( drftNb             ,0 ,sizeof( drftNb           )); 
    memset ( drwrNm             ,0 ,sizeof( drwrNm           ));
    memset ( intrstTyp          ,0 ,sizeof( intrstTyp        )); 
    memset ( cTpact_no          ,0 ,sizeof( cTpact_no        ));
    memset ( drwrAcct           ,0 ,sizeof( drwrAcct         )); 
    memset ( tranSeqNb          ,0 ,sizeof( tranSeqNb        )); 
    memset ( dscntTyp           ,0 ,sizeof( dscntTyp         )); 
    memset ( dscntInAcctSvcr    ,0 ,sizeof( dscntInAcctSvcr  )); 
    memset ( dscntInAcct        ,0 ,sizeof( dscntInAcct      )); 
    memset ( dscntInNm          ,0 ,sizeof( dscntInNm        )); 
    memset ( dscntOutAcctSvcr   ,0 ,sizeof( dscntOutAcctSvcr )); 
    memset ( dscntOutAcct       ,0 ,sizeof( dscntOutAcct     )); 
    memset ( dscntOutNm         ,0 ,sizeof( dscntOutNm       )); 
    memset ( agtAcctSvcr_1      ,0 ,sizeof( agtAcctSvcr_1    )); 

    vtcp_log("[%s][%d]", __FILE__,__LINE__);
    pub_base_sysinit();
    /**********************��ƽ̨�ĵ�����!!!*********************/
    get_zd_long ( "0440",   &dt             );   /*��      ��*/
    get_zd_long ( "0450",   &dueDt          );   /*������    */
    get_zd_double ( "0390", &drftAmt        );   /*Ʊ����  */
    get_zd_double ( "0840", &intrstRate     );   /*����      */
    get_zd_double ( "0420", &amt            );   /*ʵ�����  */
     
    memset(cTmpBuff , 0 , sizeof(cTmpBuff));
    get_zd_data ( "101F",   cTmpBuff       );   /*������ˮ��*/
    zip_space(cTmpBuff);
    memcpy(tranSeqNb , cTmpBuff , sizeof(tranSeqNb)-1);
    
    memset(cTmpBuff , 0 , sizeof(cTmpBuff));
    get_zd_data ( "0980",   cTmpBuff       );   /*������ˮ��*/
    zip_space(cTmpBuff);
    memcpy(lw_ind , cTmpBuff , sizeof(lw_ind)-1);
    
    memset(cTmpBuff , 0 , sizeof(cTmpBuff));
    get_zd_data ( "0640",   cTmpBuff        );   /*��������  */
    zip_space(cTmpBuff);
    memcpy(dscntTyp , cTmpBuff , sizeof(dscntTyp)-1);
    
    memset(cTmpBuff , 0 , sizeof(cTmpBuff));
    get_zd_data ( "0580",   cTmpBuff );   /*�������к�*/
    zip_space(cTmpBuff);
    memcpy(dscntInAcctSvcr , cTmpBuff , sizeof(dscntInAcctSvcr)-1);
    
    memset(cTmpBuff , 0 , sizeof(cTmpBuff));
    get_zd_data ( "0810",   cTmpBuff     );   /*�������˺�*/
    zip_space(cTmpBuff);
    memcpy(dscntInAcct , cTmpBuff , sizeof(dscntInAcct)-1);
    
    memset(cTmpBuff , 0 , sizeof(cTmpBuff));
    get_zd_data ( "0961",   cTmpBuff       );   /*����������*/
    zip_space(cTmpBuff);
    memcpy(dscntInNm , cTmpBuff , sizeof(dscntInNm)-1);
    
    memset(cTmpBuff , 0 , sizeof(cTmpBuff));
    get_zd_data ( "0600",   cTmpBuff);   /*�������к�*/
    zip_space(cTmpBuff);
    memcpy(dscntOutAcctSvcr , cTmpBuff , sizeof(dscntOutAcctSvcr)-1);
    
    memset(cTmpBuff , 0 , sizeof(cTmpBuff));
    get_zd_data ( "0962",   cTmpBuff      );   /*����������*/
    zip_space(cTmpBuff);
    memcpy(dscntOutNm , cTmpBuff , sizeof(dscntOutNm)-1);
    
    memset(cTmpBuff , 0 , sizeof(cTmpBuff));
    get_zd_data ( "0610",   cTmpBuff   );   /*(����)�н����к�*/
    zip_space(cTmpBuff);
    memcpy(agtAcctSvcr_1 , cTmpBuff , sizeof(agtAcctSvcr_1)-1);
    
    memset(cTmpBuff , 0 , sizeof(cTmpBuff));
    get_zd_data ( "1011",   cTmpBuff          );   /*�����־  */
    zip_space(cTmpBuff);
    memcpy(stlFlg , cTmpBuff , sizeof(stlFlg)-1);
    
    memset(cTmpBuff , 0 , sizeof(cTmpBuff));
    get_zd_data ( "0670",   cTmpBuff         );   /*Ʊ�ݷ���  */
    zip_space(cTmpBuff);
    memcpy(drftCls , cTmpBuff , sizeof(drftCls)-1);
    
    memset(cTmpBuff , 0 , sizeof(cTmpBuff));
    get_zd_data ( "0910",   cTmpBuff         );   /*Ʊ������  */
    zip_space(cTmpBuff);
    memcpy(drftTyp , cTmpBuff , sizeof(drftTyp)-1);
    
    memset(cTmpBuff , 0 , sizeof(cTmpBuff));
    get_zd_data ( "1032",   cTmpBuff          );   /*Ʊ�ݺ�    */
    zip_space(cTmpBuff);
    memcpy(drftNb , cTmpBuff , sizeof(drftNb)-1);
    
    memset(cTmpBuff , 0 , sizeof(cTmpBuff));
    get_zd_data ( "0830",   cTmpBuff        );   /*��Ʊ���˺�*/
    zip_space(cTmpBuff);
    memcpy(drwrAcct , cTmpBuff , sizeof(drwrAcct)-1);
    
    
    memset(cTmpBuff , 0 , sizeof(cTmpBuff));
    get_zd_data ( "0963",   cTmpBuff          );   /*��Ʊ������*/
    zip_space(cTmpBuff);
    memcpy(drwrNm , cTmpBuff , sizeof(drwrNm)-1);
    
    memset(cTmpBuff , 0 , sizeof(cTmpBuff));
    get_zd_data ( "0690",   cTmpBuff       );   /*��������  */
    zip_space(cTmpBuff);
    memcpy(intrstTyp , cTmpBuff , sizeof(intrstTyp)-1);
    
    vtcp_log("[%s][%d]����--[%d]",__FILE__,__LINE__,dt);
    vtcp_log("[%s][%d]������ˮ��--[%s]",__FILE__,__LINE__,tranSeqNb);
    vtcp_log("[%s][%d]��������--[%s]",__FILE__,__LINE__,dscntTyp);
    vtcp_log("[%s][%d]�������к�--[%s]",__FILE__,__LINE__,dscntInAcctSvcr);
    vtcp_log("[%s][%d]�������˺�--[%s]",__FILE__,__LINE__,dscntInAcct);
    vtcp_log("[%s][%d]����������--[%s]",__FILE__,__LINE__,dscntInNm);
    vtcp_log("[%s][%d]�������к�--[%s]",__FILE__,__LINE__,dscntOutAcctSvcr);
    vtcp_log("[%s][%d]�������˺�--[%s]",__FILE__,__LINE__,dscntOutAcct);
    vtcp_log("[%s][%d]����������--[%s]",__FILE__,__LINE__,dscntOutNm);
    vtcp_log("[%s][%d](����)�н����к�--[%s]",__FILE__,__LINE__,agtAcctSvcr_1);
    vtcp_log("[%s][%d]�����־--[%s]",__FILE__,__LINE__,stlFlg);
    vtcp_log("[%s][%d]Ʊ�ݷ���--[%s]",__FILE__,__LINE__,drftCls);
    vtcp_log("[%s][%d]Ʊ������--[%s]",__FILE__,__LINE__,drftTyp);
    vtcp_log("[%s][%d]Ʊ�ݺ�--[%s]",__FILE__,__LINE__,drftNb);
    vtcp_log("[%s][%d]Ʊ����--[%lf]",__FILE__,__LINE__,drftAmt);
    vtcp_log("[%s][%d]������--[%d]",__FILE__,__LINE__,dueDt);
    vtcp_log("[%s][%d]��������--[%s]",__FILE__,__LINE__,intrstTyp);
    vtcp_log("[%s][%d]����--[%lf]",__FILE__,__LINE__,intrstRate);
    vtcp_log("[%s][%d]ʵ�����--[%lf]",__FILE__,__LINE__,amt);
    vtcp_log("[%s][%d]��Ʊ���˺�--[%s]",__FILE__,__LINE__,drwrAcct);
    get_zd_data("0030",g_pub_tx.tx_br_no);
    get_zd_data("0020",g_pub_tx.opn_br_no);
	vtcp_log("[%s][%d]��Ʊ������--[%s]",__FILE__,__LINE__,drwrNm);     
    vtcp_log("[%s][%d]���׻���--[%s]",__FILE__,__LINE__,g_pub_tx.tx_br_no);     

    vtcp_log("[%s][%d] drftNb=[%s]  drftTyp=[%s]", __FILE__,__LINE__,drftNb,drftTyp);
    
    if(!memcmp(drftTyp,"AC01",sizeof(drftTyp)-1))
    {
        memset(drftTyp,0 ,sizeof(drftTyp));
        memcpy(drftTyp,"111",3);
    }
    else if (!memcmp(drftTyp,"AC02",sizeof(drftTyp)-1))
    {
        memset(drftTyp,0 ,sizeof(drftTyp));
        memcpy(drftTyp,"112",3);
    }
    else 
    {
        sprintf(acErrMsg, "���ʹ���,����[%s]",drftTyp);
        WRITEMSG
        strcpy(g_pub_tx.reply,"P051");
        goto ErrExit;
    }
    
    ret = sql_count("ecds_recv_reg","ecds_tx_date = %ld and ecds_trace_no ='%s' ",dt,tranSeqNb);
    if (ret!=0)
    {
        vtcp_log("[%s][%d] �Ѿ������˱ʱ�����Ϣret[%d]",__FILE__,__LINE__,ret);
        WRITEMSG
        
        ret = Ecds_recv_reg_Dec_Upd(g_pub_tx.reply,"ecds_tx_date = %ld and ecds_trace_no ='%s' and bf_flag='1' ",dt,tranSeqNb);
        if (ret)
        {
            sprintf(acErrMsg, "ִ��Ecds_recv_reg_Dec_Upd��[%d]", ret);
            WRITEMSG
            goto ErrExit;
        }
        ret = Ecds_recv_reg_Fet_Upd(&s_Ecds_recv_reg, g_pub_tx.reply);/***û���ҵ�����1��,�Ǿ�˵������0����2**/
        if (ret) {
           sprintf(acErrMsg, "ִ��Mo_discnt_Fet_Upd��![%d]", ret);
           WRITEMSG
           strcpy(g_pub_tx.reply, "NXDP");
           goto ErrExit;
        }
        s_Ecds_recv_reg.bf_flag[0]='2';/**���²�����־Ϊ��ʽ����**/
        ret = Ecds_recv_reg_Upd_Upd(s_Ecds_recv_reg, g_pub_tx.reply);
        if (ret)
        {
            sprintf(acErrMsg, "ִ��Ecds_recv_reg_Dec_Upd��[%d]", ret);
            WRITEMSG
            goto ErrExit;
        }
        Ecds_recv_reg_Clo_Upd();
    }
    else
    {
        vtcp_log("[%s][%d]----", __FILE__,__LINE__);
        /**************׼������ECDS_RECV_REG�Ǽǲ�****************/
        sEcds_recv_reg.issedt       = 0 ;
        sEcds_recv_reg.duedt        = dueDt;
        sEcds_recv_reg.ecds_tx_date = dt;
        sEcds_recv_reg.trace_no     = 0;
        sEcds_recv_reg.tx_date      = g_pub_tx.tx_date;
        
        sEcds_recv_reg.sts[0]='1';
        sEcds_recv_reg.check_flag[0]='1';
        
        memcpy(sEcds_recv_reg.tag_name,         "4956",     sizeof( sEcds_recv_reg.tag_name)-1);
        memcpy(sEcds_recv_reg.ecds_trace_no,    tranSeqNb,  sizeof(sEcds_recv_reg.ecds_trace_no)-1);
        memcpy(sEcds_recv_reg.drftcls,          drftCls,    sizeof(sEcds_recv_reg.drfttyp)-1);
        memcpy(sEcds_recv_reg.drfttyp ,         drftTyp,    sizeof(sEcds_recv_reg.drftnb)-1);
        memcpy(sEcds_recv_reg.drftnb ,          drftNb,     sizeof(sEcds_recv_reg.drftnb)-1);   
        
        Ecds_recv_reg_Debug(&sEcds_recv_reg);
        ret = Ecds_recv_reg_Ins(sEcds_recv_reg,g_pub_tx.reply);
        if (ret )
        {
            sprintf(acErrMsg, "����Ecds_recv_reg��!!ret=[%d]", ret);
            WRITEMSG
            strcpy(g_pub_tx.reply, "NXC8");
            goto ErrExit;
        }
    }
    ret = Hv_uniontab_Sel(g_pub_tx.reply,&sHv_uniontab1," or_br_no= '%s' ",dscntInAcctSvcr);
    if (ret)
    {
        vtcp_log("��ѯHv_orno_brno�����!! ret [%d]",ret);
        WRITEMSG
        strcpy ( g_pub_tx.reply , "AT06" );
        goto ErrExit;
    }
    ret = Hv_uniontab_Sel(g_pub_tx.reply,&sHv_uniontab2," or_br_no= '%s' ",dscntOutAcctSvcr);
    if (ret)
    {
        vtcp_log("��ѯHv_orno_brno�����!! ret [%d]",ret);
        WRITEMSG 
        strcpy ( g_pub_tx.reply , "AT06" );
        goto ErrExit;
    }
    /************�����жϣ������еĴ���************/
    /*if(memcmp(sHv_uniontab1.qs_no,HV_QS_BR_NO,12)==0 && memcmp(sHv_uniontab2.qs_no,HV_QS_BR_NO,12)!=0)modify by ligl 20100506**/
    if(memcmp(sHv_uniontab1.qs_no,HV_QS_BR_NO,12)==0 && memcmp(sHv_uniontab2.or_br_no,sHv_uniontab1.or_br_no,12)!=0 && lw_ind[0]=='1')
    {
        ret = Mo_discnt_Sel(g_pub_tx.reply,&sMo_discnt," pnote_no= '%s' and  note_type ='%s' and sts='X' ", drftNb,drftTyp);     
        if (ret)
        {
            sprintf(acErrMsg,"��ѯmo_discnt�����!!ret= [%d]",ret);
            WRITEMSG
            strcpy ( g_pub_tx.reply , "B112" );
            goto ErrExit;
        }
        if(sMo_discnt.po_knd[0]!='E')
        {
            sprintf(acErrMsg,"�ǵ�Ʊ�����ô�ҵ��=[%s]",sMo_discnt.po_knd);
            WRITEMSG
            strcpy ( g_pub_tx.reply , "B112" );
            goto ErrExit;
        }
	/**add by ligl 20100506 yq***/
        if(memcmp(sHv_uniontab1.qs_no,sHv_uniontab2.qs_no,12)==0)
	{
	   if(memcmp(stlFlg,"SM00",4)==0)
	   {
		strcpy(g_pub_tx.reply,"B121");
	 	goto ErrExit;	
	   }
	} 
        /**add end by ligl 20100506 yq***/
        GetBr_noByOr_br_no(dscntInAcctSvcr,cKinbr);
        vtcp_log("[%s][%d] cKinbr=[%s] ",__FILE__,__LINE__,cKinbr);
        
        if(memcmp(cKinbr,sMo_discnt.br_no,BRNO_LEN)!=0)
        {
            sprintf(acErrMsg,"������Ʊ��=[%s][%s]",cKinbr,sMo_discnt.br_no);
            WRITEMSG
            strcpy ( g_pub_tx.reply , "Z10D" );
            goto ErrExit;
        }
        set_zd_data("0910" , cKinbr);
        
        /***���ÿ��������ͽ��׻���***/
        set_zd_data("0020", cKinbr);
        set_zd_data("0030", cKinbr);
        ret = Com_parm_Sel (g_pub_tx.reply,&sCom_parm,"parm_code='%s' and parm_seqn= %d ",cKinbr,PARM_ECDS_TC_AC);
        if (ret)
        {
            sprintf(acErrMsg, "��ѯCom_parm�����!!ret  [%s]",ret);
            WRITEMSG
            strcpy ( g_pub_tx.reply , "AT06" );
            goto ErrExit;
        }
        vtcp_log("[%s][%d] sCom_parm.val=[%s] ",__FILE__,__LINE__,sCom_parm.val);
        /***********************ΪA017׼��������********************************/
        set_zd_data("0300",sCom_parm.val);
        /***********************B921,ת����¼��***********************************/
        set_zd_long("0480",0);                                 /* ��;���� */
        set_zd_data("0690","1");                               /* ������  */
        set_zd_data("0620",sMo_discnt.pact_no);                /* ת����Э����*/
        set_zd_long("0450",sMo_discnt.not_matr_date);          /* ��Ʊ������*/
        set_zd_data("0310",sMo_discnt.rem_ac_no);              /* ��Ʊ���ʺ�*/
        set_zd_data("0260",sMo_discnt.remitter);               /* ��Ʊ��ȫ��*/
        set_zd_data("0810",sMo_discnt.payer_brno_name);        /* ������ȫ��*/    
        set_zd_data("0320",sMo_discnt.payee_ac_no);            /* �տ����ʺ�*/
        set_zd_data("0270",sMo_discnt.payee);                  /* �տ���ȫ��*/
        set_zd_data("0820",sMo_discnt.payee_brno_name);        /* �տ��˿�����ȫ��*/
        set_zd_data("0900",sMo_discnt.note_type);              /* Ʊ������ */
        set_zd_data("0980",drftCls);/*��Ʊ��־*/
        set_zd_data("0600",sMo_discnt.pnote_no);               /* Ʊ�ݺ�   */
        set_zd_data("0680",sMo_discnt.local_ind);              /* ���ͬ�Ǳ�־*/
        set_zd_data("0250",sMo_discnt.pn_name);                /* ����������*/
	      sMo_discnt.tpn_int=sMo_discnt.par_amt-amt; 
        set_zd_double("1001",sMo_discnt.tpn_int);              /* ��Ʊ���*/
        set_zd_double("1002",sMo_discnt.par_amt);              /* ��Ʊ���*/
        set_zd_double("0840",sMo_discnt.pn_rate);              /* ת������*/
        if ( memcmp( dscntTyp, "RM00",4 )==0)
        {
            set_zd_data("0990","2");               /* �������� 2:���ʽ   */
        }
        else if(memcmp( dscntTyp, "RM01",4 )==0)
        {
            set_zd_data("0990","3");               /* �������� 3:�ع�ʽ   */
        }
        else 
        {
            sprintf(acErrMsg,"��Ϣ����,����[%s]",dscntTyp);
            WRITEMSG
            strcpy(g_pub_tx.reply,"P051");
            goto ErrExit;
        }
        /***********************B922,ת���ּ���***********************************/
        set_zd_data("0700", "1");           /* �տ��˻����� XXXXXX*/ 

        strcpy ( g_pub_tx.reply , "0001" );
        goto GoodExit1; 
    }
    /***else if(memcmp(sHv_uniontab2.qs_no,HV_QS_BR_NO,12)==0 && memcmp(sHv_uniontab1.qs_no,HV_QS_BR_NO,12)!=0)**modify by ligl 20100506 **//**�����еĴ���**/
    else if(memcmp(sHv_uniontab2.qs_no,HV_QS_BR_NO,12)==0 && memcmp(sHv_uniontab1.or_br_no,sHv_uniontab2.or_br_no,12)!=0 && lw_ind[0]=='2')/**�����еĴ���**/
    {
       
        ret = Mo_discnt_Sel(g_pub_tx.reply,&sMo_discnt," pnote_no= '%s' and  note_type ='%s' and sts='3' ", drftNb,drftTyp);     
        if (ret)
        {
            sprintf(acErrMsg,"��ѯmo_discnt�����!!ret= [%d]",ret);
            WRITEMSG
            strcpy ( g_pub_tx.reply , "B112" );
            goto ErrExit;
        }
        GetBr_noByOr_br_no(dscntOutAcctSvcr,cKinbr);
        vtcp_log("[%s][%d] cKinbr=[%s] br_no=[%s]",__FILE__,__LINE__,cKinbr,sMo_discnt.br_no);
        if(memcmp(cKinbr,sMo_discnt.br_no,BRNO_LEN)!=0)
        {
            sprintf(acErrMsg,"��ѯmo_discnt�����!!ret= [%d]cKinbr[%s]sMo_discnt.br_no[%s]",ret,cKinbr,sMo_discnt.br_no);
            WRITEMSG
            strcpy ( g_pub_tx.reply , "B305" );
            goto ErrExit;
        }
	/***add by ligl 20100506 yq**/
        if(memcmp(sHv_uniontab1.qs_no,sHv_uniontab2.qs_no,12)==0)
	{
	   if(memcmp(stlFlg,"SM00",4)==0)
	   {
		strcpy(g_pub_tx.reply,"B121");
	 	goto ErrExit;	
	   }
	} /**end***/
        set_zd_data("0910" , cKinbr);
        
        /***���ÿ��������ͽ��׻���***/
        set_zd_data("0020", cKinbr);
        set_zd_data("0030", cKinbr);
     	        char tx_tel[5];
                memset(tx_tel,0x00,sizeof(tx_tel));
                memcpy(tx_tel,cKinbr+1,2);
                memcpy(tx_tel+2,"99",2);
                strcpy(g_pub_tx.tel,tx_tel);
  	set_zd_data("0070",g_pub_tx.tel); 
        ret = Com_parm_Sel ( g_pub_tx.reply,&sCom_parm," parm_code='%s'  and parm_seqn= %d ",cKinbr,PARM_ECDS_TR_PAY_AC);
        if (ret)
        {
            sprintf(acErrMsg, "��ѯCom_parm�����!!ret  [%s]",ret);
            WRITEMSG
            strcpy ( g_pub_tx.reply , "AT06" );
            goto ErrExit;
        }
        
        
        /********************��α���һ��ת����Э����************************/
        char cTmpBuff[9];
        char cTraceNo[10];
        memset(cTmpBuff , 0 , sizeof(cTmpBuff));
        sprintf(cTmpBuff ,"%08d" , g_pub_tx.tx_date);
        vtcp_log("[%s][%d] cTmpBuff=[%s] ",__FILE__,__LINE__,cTmpBuff);
        
        /*sprintf(cTpact_no ,"%08d" , g_pub_tx.tx_date);**/
        memcpy(cTpact_no , cTmpBuff+2 , 6);
        vtcp_log("[%s][%d] cTpact_no=[%s] ",__FILE__,__LINE__,cTpact_no);
        strcat(cTpact_no , sMo_discnt.br_no );
        vtcp_log("[%s][%d] cTpact_no=[%s] trace_no=[%ld]",__FILE__,__LINE__,cTpact_no,g_pub_tx.trace_no);
        
        sprintf(cTraceNo,"%09d",g_pub_tx.trace_no);
        strcat(cTpact_no , cTraceNo );
        vtcp_log("[%s][%d]ת����Э����=[%s]", __FILE__,__LINE__,cTpact_no);

        /************************ΪB931׼������**************************/
        set_zd_data("0690","0");/*������   0¼��1�޸�2ɾ��*/
        set_zd_double("0840",sMo_discnt.tpn_rate);/*ת������*/
        set_zd_data("0900",sMo_discnt.note_type);/*Ʊ������*/
        set_zd_data("0600",sMo_discnt.pnote_no);/*Ʊ�ݺ�*/
        set_zd_data("0680",sMo_discnt.local_ind);/*���ͬ�Ǳ�־*/
        set_zd_data("0980",drftCls);/*��Ʊ��־*/
        set_zd_data("0250",sMo_discnt.tpn_name);/*�Է�������*/
        set_zd_data("0380",cTpact_no);
	    sMo_discnt.tpn_int=sMo_discnt.par_amt-amt; 
        set_zd_double("1001",sMo_discnt.tpn_int);/*ת������Ϣ*/
        set_zd_data("0720",sMo_discnt.tpn_style);/*׷��Ȩ0����1����*/
        /*set_zd_long("1261",sMo_discnt.tt_no);*/
        vtcp_log("[%s][%d]----�������������ʽcKinbr=%s", __FILE__,__LINE__,cKinbr);
        
        vtcp_log("[%s][%d] dscntTyp=[%s] ",__FILE__,__LINE__,dscntTyp);
        if ( memcmp( dscntTyp, "RM00",4 )==0)/*�����������ʽ*/
        {
            /************************ΪL201׼������**************************/
            
            vtcp_log("[%s][%d] ����ac_id=[%ld]",__FILE__,__LINE__,sMo_discnt.ac_id);
            ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdm_ac_rel, " ac_id=%ld ", sMo_discnt.ac_id);
            if (ret) {
               sprintf(acErrMsg, "��ѯmdm_ac_rel����![%s]", sMo_discnt.ac_id);
               WRITEMSG
               strcpy(g_pub_tx.reply, "L015");
               goto ErrExit;
            }
            set_zd_data("0330",sMdm_ac_rel.ac_no);
            vtcp_log("[%s][%d] �����˻�=[%s]",__FILE__,__LINE__,sMdm_ac_rel.ac_no);
            
            /************************ΪB932׼������**************************/
            set_zd_data("0900", sMo_discnt.note_type);    /* Ʊ������ */
            set_zd_data("0600", sMo_discnt.pnote_no);     /* Ʊ�ݺ� */
            set_zd_data("0980", drftCls);     /* Ʊ�ݺ� */
            set_zd_data("0370", sMo_discnt.tin_ac_no);    /* ת�����ʺ� */
            set_zd_data("0670", sMo_discnt.tpay_ac_type); /* �����˻����� */
	    sMo_discnt.tpn_int=sMo_discnt.par_amt-amt; 
            set_zd_double("1001",sMo_discnt.tpn_int);     /* ת������Ϣ */
            set_zd_double("1002", sMo_discnt.par_amt);    /* ��Ʊ��� */
            set_zd_data("0700", "1"); /* �����˻����� */
            /***********************ΪA016׼��*****************/
            set_zd_data("112A",sCom_parm.val);
            set_zd_data("0990","4");               /* ת����ת�� 4:���ʽ  */
        
            strcpy ( g_pub_tx.reply , "0002" );
            goto GoodExit1;
        }
        else if( memcmp( dscntTyp, "RM01",4 ) ==0 )/*�������лع�ʽ*/
        {
            vtcp_log("[%s][%d]----���������лع�ʽ", __FILE__,__LINE__);
            /**********************ΪB947׼������****************************/
            set_zd_data("0900", sMo_discnt.note_type);   /* Ʊ������ */
            set_zd_data("0600", sMo_discnt.pnote_no);    /* Ʊ�ݺ� */
            set_zd_data("0980", drftCls);    /* Ʊ�ݺ� */
            set_zd_data("0670", sMo_discnt.tpay_ac_type);/* �����˻����� */
	    sMo_discnt.tpn_int=sMo_discnt.par_amt-amt; 
            set_zd_double("1001", sMo_discnt.tpn_int);        /* ת������Ϣ */
            set_zd_double("1002", drftAmt);              /* Ʊ����� */
            /***********************ΪA016׼��*****************/
            set_zd_data("0300",sCom_parm.val);
            set_zd_data("0990","5");               /* ת����ת�� 6:�ع�ʽ   */
            strcpy ( g_pub_tx.reply , "0003" );
            goto GoodExit1;
        } 
        else 
        {
            sprintf(acErrMsg,"��Ϣ����,����[%s]",dscntTyp);
            WRITEMSG
            strcpy(g_pub_tx.reply,"P051");
            goto ErrExit;
        }
    }
    else
    {
        sprintf(acErrMsg,"��Ϣ����,����[%s][%s]",sHv_uniontab1.qs_no,sHv_uniontab2.qs_no);
        WRITEMSG
        strcpy(g_pub_tx.reply,"P051");
        goto ErrExit;
    }
    vtcp_log("[%s][%d] tx_br_no=[%s] opn_br_no=[%s][%s]",__FILE__,__LINE__,g_pub_tx.tx_br_no,g_pub_tx.opn_br_no,cKinbr);
GoodExit:
    strcpy(g_pub_tx.reply, "0000");
    sprintf(acErrMsg, "Before OK return: reply [%s]", g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 0;
GoodExit1:
    sprintf(acErrMsg, "Before OK return: reply [%s]", g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 0;
ErrExit:
    sprintf(acErrMsg, "Before bad return: reply [%s]", g_pub_tx.reply);
    WRITEMSG
    if(memcmp(g_pub_tx.reply,"0000",4)==0)
        memcpy(g_pub_tx.reply,"B556" ,4);
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 1;
}
