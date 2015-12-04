/*********************************************************************
* �� �� �� :  V1.0        
* �������� :  CHENGBO          
* ��    �� :  2011-07-13   
* ����ģ�� :  ��ƽ̨�Ľӿڳ���  (���������˻ؽ���)            
* �������� :  spF236.c
* �������� : 
*             (1)�������ʳ�����Ҫ���ڸ��ս��ף�ֻ����ת�ʲ������ֽ�  
*             (2)������Ҫ����ո����ʺ���ԭ�����ո����ʺ��Ƿ�һ�£��˻ؽ��׽���*                ���޻��߳�������Ƿ���ԭ���׽��һ�µȡ�
*             (3)�������ɺ��ĸ��ݵǼǲ���¼�Զ���֯���������Ƿ��˻�����Ӧ�����*                ָ���������˻���ֱ���˻ص������ʻ��ϣ����������ֽ�
* �����б� :       
* ʹ��ע�� :                                       
* �޸ļ�¼ :                                       
*      ��    ��:20110816
*      �� �� �ˣ�xueke
*      �޸�����:pub_com_test.c()���Ʊ�־ ����־��������ƽ̨���н��� 73����� ÿ���ֽڴ���һ�ֿ��ƻ���
*     <EYY83 DESC="ԭ���������˻�"/>
*     <EYY03 DESC="�˻ؽ���ԭ���׽��!"/>
*     <EYY04 DESC="�������ֽ���˵��˻�!"/>
      <EYY06 DESC="�˻������ѳ���ԭ������!"/>

insert into tx_def  values ('6236', '��������ת���˻�', '10000000000000000000000000000110000000010000000000000000000001000100000000000000100000000000000000000000000000000000000000000000', '1', '1', '0', '3', null);
insert into tx_flow_def  values ('6236', 0, 'F236', '#=0001@1@#=0002@2@#$');
insert into tx_flow_def  values ('6236', 1, 'D003', '#=0000@3@#$');
insert into tx_flow_def  values ('6236', 2, 'A016', '#=0000@3@#$');
insert into tx_flow_def  values ('6236', 3, 'HPDF', '#=0000@4@#$');
insert into tx_flow_def  values ('6236', 4, 'F000', '#$');

insert into tx_sub_def  values ('F236', '��������ת���˻�', 'spF236', '0', '0');

insert into tx_sub_rel  values ('6236', 'D003', '0', '0310', '1021', '�˺�');
insert into tx_sub_rel  values ('6236', 'D003', '1', '0000', '0192', '���Ʊ�־');
insert into tx_sub_rel  values ('6236', 'D003', '1', '1001', '0193', '��ͨ��ȡ');
insert into tx_sub_rel  values ('6236', 'D003', '1', '1', '0191', '���Ʊ�־');
insert into tx_sub_rel  values ('6236', 'D003', '1', '1', '1022', '�˻����');
insert into tx_sub_rel  values ('6236', 'D003', '0', '0400', '102F', '���');
insert into tx_sub_rel  values ('6236', 'D003', '1', '01', '102J', '����');
insert into tx_sub_rel  values ('6236', 'D003', '1', '2', '102K', '��ת��־');
insert into tx_sub_rel  values ('6236', 'D003', '0', '0810', '102Y', 'ժҪ');

insert into tx_sub_rel  values ('6236', 'A016', '0', '0310', '1201', '�˺�');
insert into tx_sub_rel  values ('6236', 'A016', '1', '01', '1204', '����');
insert into tx_sub_rel  values ('6236', 'A016', '1', '2', '1205', 'ת��');
insert into tx_sub_rel  values ('6236', 'A016', '0', '0400', '1208', '���');
insert into tx_sub_rel  values ('6236', 'A016', '0', '0810', '120A', 'ժҪ');

insert into tx_sub_rel  values ('6236', 'HPDF', '1', '2', '0660', '�����־');
insert into tx_sub_rel  values ('6236', 'HPDF', '1', '01', '0210', '����');
insert into tx_sub_rel  values ('6236', 'HPDF', '0', '0300', '0300', '�˺�');
insert into tx_sub_rel  values ('6236', 'HPDF', '1', '2', '0670', '��ת��־');
insert into tx_sub_rel  values ('6236', 'HPDF', '0', '0400', '0400', '���');
insert into tx_sub_rel  values ('6236', 'HPDF', '0', '0810', '0880', 'ժҪ');

******************************************************************/

#include <stdio.h>
#include <math.h>
#define  EXTERN
#include "public.h"
#include "chnl_single_acct_c.h"
#include "com_item_c.h"
#include "in_mst_c.h"
#include "dd_mst_c.h"
#include "prdt_ac_id_c.h"
#include "mdm_ac_rel_c.h"

spF236()
{
    int iRet=0;
    char cAc_no[25];   /*�����˺�*/
    /*char cHx_cert[21];    ֤�����͡�֤������*/
    /*char cHx_ctrlcmd[11]; ���Ʊ�־*/
    char  cCert_no[21];  /**֤������/����**/
    char  cCtrlcmd[11];  /**���Ʊ�־ **/
    char  cFee_flag[2]; /*�շѱ�־*/
    char cHx_msr2[28];    /*���ŵ���Ϣ*/
    char cHx_msr3[105];   /*���ŵ���Ϣ*/
    /*char cHx_pwd[7];      �ͻ�����*/
    char  cPwd[17];  /** �ͻ����� **/
    char cHx_brf[51];     /*ժҪ*/
    char cBbmk[5];        /*ϵͳ����*/
    char cHx_trace_no[13];/*������ˮ*/
    char cPt_trace_no[13];/*ԭƽ̨��ˮ*/
    char cPt_trano[13];   /*ƽ̨��ˮ*/
    double dHx_amt=0.00;  /*���׽��*/
    double dBj_fee=0.00;  /*�Ƚ�������ʹ��*/
    long lHx_old_date=0;  /*ԭƽ̨����*/
    long lHx_old_trace=0; /*ԭƽ̨��ˮ*/
    long lPt_trace_no=0;  /*ƽ̨��ˮ*/
    long lPt_date=0;
    long lHx_trace=0;  /*������ˮ*/
    
    int i;/*ѭ��ʱʹ��*/
    long   lFee_cnt=0;     /*�շ�����*/
    char   cFee_cnt[3];    /*�շ�����*/
    char   cFd123[1000];   /*����123������*/
    char   cFee_amt[13];   /*�˻��շѽ��*/
    double dFee_amt=0.00;  /*�˻��շѽ��*/
    char   cFee_dc_ind[2]; /*�շ� �����־ 1 ��2 ��*/
    /*����pub_com_test����ʱ�����շ���ֵ*/
    char cGz_flag[2];/*���˱�־*/
    double dTh_amt=0.00; /*�ۼ��˻ؽ��20111011 xk*/
   
    struct chnl_single_acct_c sChnl_single_acct;
    struct chnl_single_acct_c sChnl_single_acct_1;
    struct chnl_single_acct_c sChnl_single_acct_2;
    struct com_item_c   sCom_item;
    struct com_item_c   sCom_item_1;
    struct dd_mst_c     sDd_mst;
    struct dd_mst_c     sDd_mst_1;
    struct in_mst_c     sIn_mst;
    struct in_mst_c     sIn_mst_1;
    struct prdt_ac_id_c sPrdt_ac_id;
    struct prdt_ac_id_c sPrdt_ac_id_1;
    struct mdm_ac_rel_c sMdm_ac_rel;
    struct mdm_ac_rel_c sMdm_ac_rel_1;

    memset(cAc_no,0x00,sizeof(cAc_no)); 
    /*memset(cHx_cert, 0x00,sizeof(cHx_cert)); */
    memset(cCert_no, 0x00, sizeof(cCert_no));
    memset(cCtrlcmd,0x00,sizeof(cCtrlcmd)); 
    memset(cFee_flag,0x00,sizeof(cFee_flag)); 
    memset(cHx_msr2,0x00,sizeof(cHx_msr2)); 
    memset(cHx_msr3,0x00,sizeof(cHx_msr3)); 
    /*memset(cHx_pwd,0x00,sizeof(cHx_pwd));*/ 
    memset(cPwd, 0x00, sizeof(cPwd));
    memset(cHx_brf,0x00,sizeof(cHx_brf));
    memset(cBbmk,0x00,sizeof(cBbmk));
    memset(cHx_trace_no,0x00,sizeof(cHx_trace_no));
    memset(cPt_trace_no,0x00,sizeof(cPt_trace_no));
    memset(cPt_trano,0x00,sizeof(cPt_trano));   /*ƽ̨��ˮ*/
    memset(&sChnl_single_acct,0x00,sizeof(struct chnl_single_acct_c));
    memset(&sChnl_single_acct_1,0x00,sizeof(struct chnl_single_acct_c));
    memset(&sChnl_single_acct_2,0x00,sizeof(struct chnl_single_acct_c));
    memset(&sCom_item,0x00,sizeof(struct com_item_c));
    memset(&sDd_mst,0x00,sizeof(struct dd_mst_c));
    memset(&sDd_mst_1,0x00,sizeof(struct dd_mst_c));
    memset(&sIn_mst,0x00,sizeof(struct in_mst_c));
    memset(&sIn_mst_1,0x00,sizeof(struct in_mst_c));
    memset(&sPrdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));
    memset(&sPrdt_ac_id_1,0x00,sizeof(struct prdt_ac_id_c));
    memset(&sMdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
    memset(&sMdm_ac_rel_1,0x00,sizeof(struct mdm_ac_rel_c));
    /*����pub_com_testʱ�õ�*/
    memset(cGz_flag,0x00,sizeof(cGz_flag));
    memset(cFd123,0x00,sizeof(cFd123));
    memset(cFee_amt,0x00,sizeof(cFee_amt));
    memset(cFee_cnt,0x00,sizeof(cFee_cnt));
    memset(cFee_dc_ind,0x00,sizeof(cFee_dc_ind));
    
    /*��ʼ������*/
    pub_base_sysinit();
    vtcp_log("[%s][%d],tx_date=[%d]",__FILE__,__LINE__,g_pub_tx.tx_date);
    set_zd_long("0460",g_pub_tx.tx_date);
    get_zd_data("0300",cAc_no);
    get_zd_data("0620",cCert_no);
    get_zd_data("0730",cCtrlcmd);
    get_zd_data("0710",cFee_flag);
    get_zd_data("0750",cHx_msr2);
    get_zd_data("0760",cHx_msr3);
/*****
    get_zd_data("0790",cPwd);
******/
    get_zd_data("0810",cHx_brf);
    get_zd_data("0100",cBbmk);
    get_zd_long("0450",&lHx_old_date);
    get_zd_long("0780",&lHx_old_trace);
    get_zd_double("0400",&dHx_amt);
    get_zd_long("0440",&lPt_date);
    get_zd_long("0520",&lPt_trace_no); 
    /*ƽ̨��ˮ��������ת��*/
    sprintf(cPt_trace_no,"%d",lHx_old_trace);
    
    /*�¾��˺�ת��*/ 
    pub_base_old_acno(cAc_no);
    memcpy(g_pub_tx.draw_pwd,cPwd,sizeof(g_pub_tx.draw_pwd)-1);
    /**����bbmk �ֶ� shangyongchao 2012/6/7 11:21:31
    iRet = Chnl_single_acct_Sel(g_pub_tx.reply,&sChnl_single_acct," pt_date=%ld and pt_trace_no='%s' ",lHx_old_date,cPt_trace_no);
    **/
    iRet = Chnl_single_acct_Sel(g_pub_tx.reply,&sChnl_single_acct," pt_date=%ld and pt_trace_no='%s' and bbmk='%s' ",\
    lHx_old_date,cPt_trace_no,cBbmk);
    if(iRet)
    {
        strcpy(g_pub_tx.reply, "D103");
        sprintf(acErrMsg, "��ѯҪ���˻صĽ��׼�¼����!");
        WRITEMSG
        goto ErrExit;
    }
    
    if(sChnl_single_acct.ct_ind[0]=='1')
    {
        strcpy(g_pub_tx.reply,"YY04");
        sprintf(acErrMsg,"�������ֽ���˵��˻�!");
        WRITEMSG
        goto ErrExit;
    } 
    if(sChnl_single_acct.sts[0]=='5')
    {
        strcpy(g_pub_tx.reply,"YY83");
        sprintf(acErrMsg, "��ת���ѱ��˻�");
        WRITEMSG
        goto ErrExit;
    }
    
    if( strcmp(sChnl_single_acct.ac_no,cAc_no) != 0)
    {
        strcpy(g_pub_tx.reply, "YY01");
        sprintf(acErrMsg,"���˺���ԭ�˺Ų���!");
        WRITEMSG
        goto ErrExit;
    } 
    vtcp_log("%s,%d,dHx_amt=[%f],th_amt=[%f],tx_amt=[%f]",__FILE__,__LINE__,dHx_amt,sChnl_single_acct.th_amt,sChnl_single_acct.tx_amt);
    if( pub_base_CompDblVal(sChnl_single_acct.tx_amt,sChnl_single_acct.th_amt+dHx_amt) < 0)
    {
        strcpy(g_pub_tx.reply,"YY03");
        sprintf(acErrMsg,"�˻ؽ���ԭ���׽��!");
        WRITEMSG
        goto ErrExit;
    }
    /*����˻������� slz*/
    get_fd_data("1230",cFd123);
    memcpy(cFee_cnt,cFd123+1,2);
    vtcp_log("[%s][%d],cFee_cnt=[%s]",__FILE__,__LINE__,cFee_cnt);
    lFee_cnt=atoi(cFee_cnt);
    vtcp_log("[%s][%d],lFee_cnt=[%d]",__FILE__,__LINE__,lFee_cnt);
    dFee_amt=0.00;
    for(i=0;lFee_cnt > i; i++)
    {   
        memset(cFee_dc_ind,0x00,sizeof(cFee_dc_ind)); 
        memset(cFee_amt,0x00,sizeof(cFee_amt));
        memcpy(cFee_dc_ind,cFd123+27+48*i,1);
        memcpy(cFee_amt,cFd123+29 + 48 * i,12);
        vtcp_log("[%s][%d],cFee_dc_ind=[%s]",__FILE__,__LINE__,cFee_dc_ind);
        vtcp_log("[%s][%d],cFee_amt=[%s]",__FILE__,__LINE__,cFee_amt);
        dBj_fee=atof(cFee_amt) / 100;
        if(cFee_dc_ind[0] == '2')
        {
            dFee_amt+=dBj_fee;
        }
    }
    /**add by chenggx 20120106 �����ѱ�־2���ǵ���ʹ�ò����**/
    if(cFee_flag[0]=='2'){
        vtcp_log("%s,%d,���ǵ��������0710[%s]!",__FILE__,__LINE__,cFee_flag);
    }else{
        if( pub_base_CompDblVal(sChnl_single_acct.fee_amt,dFee_amt) < 0)
        {
            strcpy(g_pub_tx.reply,"YY06");
            sprintf(acErrMsg,"�˻������ѳ���ԭ������!");
            WRITEMSG
            goto ErrExit;
        }
    }
     /* end ����˻������� slz*/
    if( strlen ( sChnl_single_acct.ac_no2 ) >10) /*** ���������,ֻ�����˺� ***/
    {
        iRet = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_no='%s'",sChnl_single_acct.ac_no2);
        if (iRet)
        {
            vtcp_log("%s,%d,ac_no[%s]���Ҵ���!",__FILE__,__LINE__,sChnl_single_acct.ac_no2);
            strcpy(g_pub_tx.reply,"P102");
            goto ErrExit;
        }
        if(sMdm_ac_rel.note_sts[0] != '0')
        {
            vtcp_log("%s,%d,ac_no[%s]����״̬������!",__FILE__,__LINE__,sChnl_single_acct.ac_no2);
            strcpy(g_pub_tx.reply,"D140");
            goto ErrExit;
        }
        iRet = Prdt_ac_id_Sel(g_pub_tx.reply,&sPrdt_ac_id,"ac_id=%ld ",sMdm_ac_rel.ac_id);
        if (iRet)
        {
            vtcp_log("%s,%d,ac_no[%s]��ѯprdt_ac_id����!",__FILE__,__LINE__,sChnl_single_acct.ac_no2);
            strcpy(g_pub_tx.reply,"K106");
            goto ErrExit;
        }
        if(sPrdt_ac_id.ac_id_type[0] == '1')
        {
            iRet = Dd_mst_Sel(g_pub_tx.reply,&sDd_mst,"ac_id=%ld",sMdm_ac_rel.ac_id);
            if (iRet)
            {
                vtcp_log("%s,%d,ac_no[%s]���Ҵ���!",__FILE__,__LINE__,sChnl_single_acct.ac_no2);
                strcpy(g_pub_tx.reply,"P102");
                goto ErrExit;
            }
            if(sDd_mst.ac_sts[0] != '1')
            {
                vtcp_log("%s,%d,ac_no[%s]���˻�������!",__FILE__,__LINE__,sChnl_single_acct.ac_no2);
                strcpy(g_pub_tx.reply,"O085");
                goto ErrExit;
            }
            strcpy(g_pub_tx.reply, "0001");
        }
        else if(sPrdt_ac_id.ac_id_type[0] == '9')
        {
            iRet = In_mst_Sel(g_pub_tx.reply,&sIn_mst,"ac_id=%ld",sMdm_ac_rel.ac_id);
            if (iRet)
            {
                vtcp_log("%s,%d,ac_no[%s]���Ҵ���!",__FILE__,__LINE__,sChnl_single_acct.ac_no2);
                strcpy(g_pub_tx.reply,"P102");
                goto ErrExit;
            }
            if(sIn_mst.sts[0] != '1')
            {
                vtcp_log("%s,%d,ac_no[%s]���˻�������!",__FILE__,__LINE__,sChnl_single_acct.ac_no2);
                strcpy(g_pub_tx.reply,"O085");
                goto ErrExit;
            }
            strcpy(g_pub_tx.reply, "0002");
        }
               
    }
    else
    {
        /**** �����ǿ�Ŀ  **/
        iRet = Com_item_Sel(g_pub_tx.reply,&sCom_item,"acc_no = '%s'",sChnl_single_acct.ac_no2);
        if(iRet)
        {
            sprintf(acErrMsg,"��ѯ��Ŀʧ��!!!");
            WRITEMSG
            strcpy(g_pub_tx.reply, "B172");
            goto ErrExit;
        }
        /*** ��ʼ���ü���Ҫ�� **/
        strcpy(g_pub_tx.reply, "0002");
    }
    
    if( strlen ( sChnl_single_acct.ac_no1 ) >10) /*** ���������,ֻ�����˺� ***/
    {
        iRet = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel_1,"ac_no='%s'",sChnl_single_acct.ac_no1);
        if (iRet)
        {
            vtcp_log("%s,%d,ac_no[%s]���Ҵ���!",__FILE__,__LINE__,sChnl_single_acct.ac_no1);
            strcpy(g_pub_tx.reply,"P102");
            goto ErrExit;
        }
        if(sMdm_ac_rel_1.note_sts[0] != '0')
        {
            vtcp_log("%s,%d,ac_no[%s]����״̬������!",__FILE__,__LINE__,sChnl_single_acct.ac_no1);
            strcpy(g_pub_tx.reply,"D140");
            goto ErrExit;
        }
        iRet = Prdt_ac_id_Sel(g_pub_tx.reply,&sPrdt_ac_id_1,"ac_id=%ld ",sMdm_ac_rel_1.ac_id);
        if (iRet)
        {
            vtcp_log("%s,%d,ac_no[%s]��ѯprdt_ac_id����!",__FILE__,__LINE__,sChnl_single_acct.ac_no1);
            strcpy(g_pub_tx.reply,"K106");
            goto ErrExit;
        }
        if(sPrdt_ac_id_1.ac_id_type[0] == '1')
        {
            iRet = Dd_mst_Sel(g_pub_tx.reply,&sDd_mst_1,"ac_id=%ld",sMdm_ac_rel_1.ac_id);
            if (iRet)
            {
                vtcp_log("%s,%d,ac_no[%s]���Ҵ���!",__FILE__,__LINE__,sChnl_single_acct.ac_no1);
                strcpy(g_pub_tx.reply,"P102");
                goto ErrExit;
            }
            if(sDd_mst_1.ac_sts[0] != '1')
            {
                vtcp_log("%s,%d,ac_no[%s]���˻�������!",__FILE__,__LINE__,sChnl_single_acct.ac_no1);
                strcpy(g_pub_tx.reply,"O085");
                goto ErrExit;
            }
            set_zd_data("0680","1");/** �����ʻ� **/
        }
        else if(sPrdt_ac_id_1.ac_id_type[0] == '9')
        {
            iRet = In_mst_Sel(g_pub_tx.reply,&sIn_mst_1,"ac_id=%ld",sMdm_ac_rel_1.ac_id);
            if (iRet)
            {
                vtcp_log("%s,%d,ac_no[%s]���Ҵ���!",__FILE__,__LINE__,sChnl_single_acct.ac_no1);
                strcpy(g_pub_tx.reply,"P102");
                goto ErrExit;
            }
            if(sIn_mst_1.sts[0] != '1')
            {
                vtcp_log("%s,%d,ac_no[%s]���˻�������!",__FILE__,__LINE__,sChnl_single_acct.ac_no1);
                strcpy(g_pub_tx.reply,"O085");
                goto ErrExit;
            }
            set_zd_data("0680","9");/** �ڲ��ʻ� **/
        }
        
             /*73����Ʊ�־���*/
        iRet=pub_com_test(cCtrlcmd,cCert_no,"",cAc_no,"","","",cGz_flag);
        vtcp_log("%s,%d,cCtrlcmd=[%s],cCert_no=[%s],cAc_no=[%s],cGz_flag=[%s]",__FILE__,__LINE__,cCtrlcmd,cCert_no,cAc_no,cGz_flag); 
        if (iRet)
        {
            sprintf(acErrMsg,"%s,%d,��������pub_com_test()����ʧ��",__FILE__,__LINE__);
            WRITEMSG
            goto ErrExit;	
        }
       
    }
    else
    {
        /**** ������ڸÿ�Ŀ  **/
        iRet = Com_item_Sel(g_pub_tx.reply,&sCom_item_1,"acc_no = '%s'",sChnl_single_acct.ac_no1);
        if(iRet)
        {
            sprintf(acErrMsg,"��ѯ��Ŀʧ��!!!");
            WRITEMSG
            strcpy(g_pub_tx.reply, "B172");
            goto ErrExit;
        }
    }
    
    /*׼������ԭ��¼*/
    memset(&sChnl_single_acct_2,0x00,sizeof(struct chnl_single_acct_c));
    /**���� bbmk�ֶ� shangyongchao 2012/6/7 11:22:53
    iRet= Chnl_single_acct_Dec_Upd(g_pub_tx.reply,"pt_date=%ld and pt_trace_no='%s' ",lHx_old_date,cPt_trace_no);
    **/
    iRet= Chnl_single_acct_Dec_Upd(g_pub_tx.reply,"pt_date=%ld and pt_trace_no='%s' and bbmk='%s'",\
            lHx_old_date,cPt_trace_no,cBbmk);
    if( iRet)
    {
        sprintf(acErrMsg,"���µ��ʼ��˵Ǽǲ�����! iRet=[%d]",iRet);
        WRITEMSG
        goto ErrExit;
    }
    
    iRet= Chnl_single_acct_Fet_Upd(&sChnl_single_acct_2,g_pub_tx.reply);
    if( iRet )
    {
        sprintf(acErrMsg,"�õ����ʼ��˵Ǽǲ���¼����!");
        WRITEMSG
        goto ErrExit;
    }
    /*�ж��ۼƽ����ԭ�������ȣ��˻سɹ������С�ڣ������˻���20111011 xk*/
     dTh_amt=sChnl_single_acct_2.th_amt;
     
     iRet=pub_base_CompDblVal(sChnl_single_acct.tx_amt,dTh_amt + dHx_amt);
     if( iRet == 0 )
     {
        sChnl_single_acct_2.sts[0]='5';  
                                   /*
                                   0:������Ч(���������)
                                   1:���˳ɹ�(�������)
                                   2�����ʳɹ�(�������)
                                   3���Һ����˳ɹ�(�������)
                                   4�������ɹ�����������ˣ�
                                   5���˻سɹ�(�������)
                                   6: �˻���־(�������)
                                   */
    }
    else if ( iRet == 1 )
    {
       sChnl_single_acct_2.sts[0]='6';
       sChnl_single_acct_2.th_amt+=dHx_amt;
    }
   
    sChnl_single_acct_2.o_tx_date=g_pub_tx.tx_date;
    sprintf(cHx_trace_no,"%12ld",sChnl_single_acct.trace_no);
    strcpy(sChnl_single_acct_2.o_trace_no,cHx_trace_no); 
    sChnl_single_acct_2.o_pt_date=sChnl_single_acct.pt_date;
    
    strcpy(sChnl_single_acct_2.o_pt_trace_no,sChnl_single_acct.pt_trace_no);
    
    
    iRet = Chnl_single_acct_Upd_Upd(sChnl_single_acct_2,g_pub_tx.reply);
    if(iRet)
    {
         sprintf(acErrMsg, "�޸ĵ��ʼ��˵Ǽǲ�����![%d]",iRet);
         WRITEMSG
         goto ErrExit;
    }
    
    Chnl_single_acct_Clo_Upd();
    /*****�ڼ��˽�����Ǽ�
    if (pub_ins_trace_log())
    {
         sprintf(acErrMsg, "�Ǽǽ�����ˮ����!");
         WRITEMSG
         goto ErrExit;
    }
   ****/ 
    /*������˻ؼ�¼�Ǽǵ����ʼ��˵Ǽǲ���*/
    
    memset(&sChnl_single_acct_1,0x00,sizeof(struct chnl_single_acct_c));
    
    sChnl_single_acct_1.tx_date=g_pub_tx.tx_date;
    sprintf(sChnl_single_acct_1.tx_time,"%6d",g_pub_tx.tx_time);
    /*sChnl_single_acct_1.trace_no=g_pub_tx.ejfno;�Ǽǵķǹ�Ա��ˮ�ţ���Щ����û�й�Ա��ˮ����һ�����Ϊ���Ľ�����ˮ�� 20110727 Mod by CHENGBO*/
    sChnl_single_acct_1.trace_no=g_pub_tx.trace_no;
    memcpy(sChnl_single_acct_1.cur_no,"01",2);
    memcpy(sChnl_single_acct_1.ac_no,sChnl_single_acct.ac_no,sizeof(sChnl_single_acct_1.ac_no)-1);
    memcpy(sChnl_single_acct_1.ac_no1,sChnl_single_acct.ac_no2,sizeof(sChnl_single_acct_1.ac_no1)-1);
    memcpy(sChnl_single_acct_1.ac_no2,sChnl_single_acct.ac_no1,sizeof(sChnl_single_acct_1.ac_no2)-1);
    memcpy(sChnl_single_acct_1.new_ac_no2,"",sizeof(sChnl_single_acct_1.new_ac_no2)-1);
    memcpy(sChnl_single_acct_1.new_ac_name2,"",sizeof(sChnl_single_acct_1.new_ac_name2)-1);
    memcpy(sChnl_single_acct_1.tx_code,g_pub_tx.tx_code,sizeof(sChnl_single_acct_1.tx_code)-1);
    memcpy(sChnl_single_acct_1.g_ac_no2,"",sizeof(sChnl_single_acct_1.g_ac_no2)-1);
    memcpy(sChnl_single_acct_1.tx_code,g_pub_tx.tx_code,sizeof(sChnl_single_acct_1.tx_code)-1);
    memcpy(sChnl_single_acct_1.tx_br_no,g_pub_tx.tx_br_no,sizeof(sChnl_single_acct_1.tx_br_no)-1);
    memcpy(sChnl_single_acct_1.tel,g_pub_tx.tel,sizeof(sChnl_single_acct_1.tel)-1);
    memcpy(sChnl_single_acct_1.bbmk,cBbmk,sizeof(sChnl_single_acct_1.bbmk)-1);
    memcpy(sChnl_single_acct_1.ttyn,g_pub_tx.tty,sizeof(sChnl_single_acct_1.ttyn)-1);
    sChnl_single_acct_1.pt_date=lPt_date;
    sprintf(sChnl_single_acct_1.pt_time,"%6d",g_pub_tx.tx_time);
    sprintf(cPt_trano,"%ld",lPt_trace_no); /*ƽ̨��ˮ��*/
    memcpy(sChnl_single_acct_1.pt_trace_no,cPt_trano,sizeof(sChnl_single_acct_1.pt_trace_no)-1);
    sChnl_single_acct_1.tx_amt=dHx_amt;
    sChnl_single_acct_1.ct_ind[0]='2';
    memcpy(sChnl_single_acct_1.brf,cHx_brf,sizeof(sChnl_single_acct_1.brf)-1); 
    sChnl_single_acct_1.fee_amt=dFee_amt;
    sChnl_single_acct_1.sts[0]='1';
    sChnl_single_acct_1.dz_ind[0]='0';
    sChnl_single_acct_1.o_pt_date = sChnl_single_acct.pt_date;
    strcpy(sChnl_single_acct_1.o_pt_trace_no,sChnl_single_acct.pt_trace_no);
    
    iRet = Chnl_single_acct_Ins(sChnl_single_acct_1,g_pub_tx.reply);
    if( iRet )
    {
        sprintf(acErrMsg,"���뵥�ʼ��˵Ǽǲ�����!");
        WRITEMSG
        goto ErrExit;
    } 
    
    set_zd_data("0310",sChnl_single_acct.ac_no2);   /*�跽�˺�*/
    set_zd_data("0300",sChnl_single_acct.ac_no1);   /*�����˺�*/
    
OkExit:
       sprintf(acErrMsg, "Before OK return: reply is[%s]", g_pub_tx.reply);
       WRITEMSG
               set_zd_data(DC_REPLY, g_pub_tx.reply);
       return 0;
ErrExit:
       sprintf(acErrMsg, "Before return: reply is[%s]", g_pub_tx.reply);
       WRITEMSG
               set_zd_data(DC_REPLY, g_pub_tx.reply);
       return 1;
}
