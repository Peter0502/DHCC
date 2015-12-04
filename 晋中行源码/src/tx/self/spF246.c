/*********************************************************************
* �� �� �� :  V1.0        
* �������� :  gongliangliang    
* ��    �� :  2011-08-08   
* ����ģ�� :  ��ƽ̨�Ľӿڳ��� (Ԥ��Ȩ��ɳ�������)          
* �������� :  spF246.c
* �������� : 
* �����б� :  1 pub_base_CheckCrdNo()
*             2 pub_base_DesChk()
*             3 pubf_card_chk_msr()
*             4 pub_base_CompDblVal()     
*             5 pub_card_reg()
* ʹ��ע�� :  ���Ԥ��Ȩ��־��¼ 5 Ԥ��Ȩ��ɳ�������                                     
* �޸ļ�¼ :                                       
*     �޸����� :                           
*     �޸�ʱ�� :                                    
*     �޸����� :
insert into tx_def values('6246','Ԥ��Ȩ��ɳ�������','10000000000010000000000010000100000000000000000000000000000000000000000000000000011000000000000000000100000000000000000000000100','1','1','0','3',null);
insert into tx_sub_def values('6246','Ԥ��Ȩ��ɳ�������','spF246','1','0');
insert into tx_flow_def values('6246','0','F246','#=0001@1@#=0002@2@#$');
insert into tx_flow_def values('6246','1','D003','#=0000@3@#$');
insert into tx_flow_def values('6246','2','A016','#=0000@3@#$');
insert into tx_flow_def values('6246','3','F000','#$');
insert into tx_dc_rel values('F246','0000','F04600');
����������˾Ͳ��ü��ˣ�F04600 �õ�ԭ����
insert into dc_entry values('F04600','1','0210','@0152','0930','1','1','2','0420','��ɳ�������');
*************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "chnl_single_hold_c.h"
#include "card.h"
#include "card_reg_c.h"
#include "mdm_ac_rel_c.h"
#include "prdt_ac_id_c.h"
#include "dd_mst_c.h"
#include "in_mst_c.h" 
#include "com_item_c.h"

int spF246()
{
    int ret;
    long lOld_date;/*ԭƽ̨����*/
    char cOld_trace_no[13];/*ԭƽ̨��ˮ��*/
    char cAc_no[25];/*����*/
    char cCtrlcmd[11];/*���Ʊ�־����־��������ƽ̨���н��� 73����� ÿ���ֽڴ���һ�ֿ��ƻ���
                      ����־(0�����1���)��
                      [0]:�������־0����1��
                      [1]:�ŵ���Ϣ����־0����1��
                      [2]:֤������־0����1��
                      [3]:��������־0���鲻��
                      [4]:�ʺż���־0���鲻��
                      [5]:�տ��˹��ʱ�־0�ʺű���������黧��1�ʺŻ������붼���2�ʺű��������������������3�ʺŻ�����һ��������,9ǿ�ƹ���
                      [6]:ͨ��ͨ�ұ�־0����1������
                      [7]:ƾ֤����־0����1���Աƾ֤2��ͻ�ƾ֤*/
    char cYsq_ind[2];/*Ԥ��Ȩ��־*/
    char cYsq_no[11];/*Ԥ����*/
    char tmpstr[51],tmpbrf[21];
    char cFee_ind[2];/*POS��־��0Ϊ����POS��1Ϊ����POS*/
    char ct_ind[2];/*��ת��ʶ*/
    char cPt_trace_no[13];/*ƽ̨��ˮ��*/
    char cMsr_str2[38];/*2�ŵ���Ϣ*/
    char cMsr_str3[142];/*3�ŵ���Ϣ*/
    char cGz_flag[2];/*���˱�־*/
    char cBbmk[5];/*ϵͳ����*/
    struct chnl_single_hold_c sChnl_single_hold;
    struct mdm_ac_rel_c sMdm_ac_rel;
    struct prdt_ac_id_c sPrdt_ac_id; 
    struct dd_mst_c sDd_mst;
    struct in_mst_c sIn_mst;
    struct com_item_c sCom_item;
    
    memset(cFee_ind,0,sizeof(cFee_ind));
    memset(ct_ind,0,sizeof(ct_ind));
    memset(&sChnl_single_hold,0x0,sizeof(struct chnl_single_hold_c));
    memset(cAc_no,0,sizeof(cAc_no));
    memset(tmpstr,0,sizeof(tmpstr));
    memset(tmpbrf,0,sizeof(tmpbrf));
    memset(cCtrlcmd,0,sizeof(cCtrlcmd));
    memset(cMsr_str2,0,sizeof(cMsr_str2));
    memset(cMsr_str3,0,sizeof(cMsr_str3)); 
    memset(cYsq_no,0,sizeof(cYsq_no));
    memset(cYsq_ind,0,sizeof(cYsq_ind)); 
    memset(cPt_trace_no,0,sizeof(cPt_trace_no));
    memset(&sMdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
    memset(&sPrdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));
    memset(&sDd_mst,0x00,sizeof(struct dd_mst_c));
    memset(&sIn_mst,0x00,sizeof(struct in_mst_c));
    memset(&sCom_item,0x00,sizeof(struct com_item_c));
    memset(cGz_flag,0x00,sizeof(cGz_flag));/*����pub_com_testʱ�õ�*/
    memset(cBbmk,0x00,sizeof(cBbmk));
    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"��ʼ�����ýṹ����!!");
        WRITEMSG
        goto ErrExit;
    }
    vtcp_log("[%s][%d],tx_date=[%d]",__FILE__,__LINE__,g_pub_tx.tx_date);
    set_zd_long("0460",g_pub_tx.tx_date);
    get_zd_data("0690",cFee_ind);/*��������POS��־��0Ϊ����POS,1Ϊ����POS*/
    get_zd_data("0300",g_pub_tx.crd_no); /* ���� */
    get_zd_data("0310",cAc_no);/*ȡ���̻��ʺ�*/
    vtcp_log("%s,%d,ȡ���̻��ʺ�Ϊac_no=[%s]",__FILE__,__LINE__,cAc_no);
    get_zd_double("0400",&g_pub_tx.tx_amt1);  /* ���ѽ�� */
    get_zd_data("0730",cCtrlcmd);/*���Ʊ�־*/
    get_zd_data("0810",tmpstr); /* ժҪ*/
    memcpy(tmpbrf,tmpstr,20); 
    get_zd_data("0780",cOld_trace_no);  /*ԭ�ն���ˮ�� */
    get_zd_long("0450",&lOld_date);  /*ԭƽ̨����*/
    get_zd_data("0520",cPt_trace_no);/*ƽ̨��ˮ*/
	/*****
    get_zd_data("0790",g_pub_tx.draw_pwd);  ֧ȡ���� 
	****/
    get_zd_data("0750",cMsr_str2);/*�ŵ�2��Ϣ*/
    get_zd_data("0760",cMsr_str3);/*�ŵ�3��Ϣ*/
    get_zd_data("0870",cYsq_no);/*Ԥ����*/
    get_zd_data("0700",cYsq_ind);/*Ԥ��Ȩ��־ 4λԤ��Ȩ��ɳ���*/
    get_zd_data("0100",cBbmk);/*ϵͳ����*/
    /*���Ԥ��Ȩ��־�Ƿ�ΪԤ��Ȩ��ɳ�������*/
    vtcp_log("%s,%d,Ԥ��Ȩ��־��%c��",__FILE__,__LINE__,cYsq_ind[0]);
    if(cYsq_ind[0] != '5')/*�˴�Ҫ�޸������ֵ䣬���Ԥ��Ȩ��־��¼ 5 Ԥ��Ȩ��ɳ������� add by gong 20110810*/
    {
        sprintf(acErrMsg,"Ԥ��Ȩ��־����");
        WRITEMSG
        goto ErrExit;
    }
    
    /* ���֧ȡ��ʽ�ĺϷ��� */
    ret=pub_com_test(cCtrlcmd,"","",g_pub_tx.crd_no,"","","",cGz_flag);
    vtcp_log("%s,%d,cCtrlcmd=[%s],g_pub_tx.crd_no=[%s],cGz_flag=[%s]",__FILE__,__LINE__,cCtrlcmd,g_pub_tx.crd_no,cGz_flag);
    if (ret)
    {
        sprintf(acErrMsg,"%s,%d,��������pub_com_test()����ʧ��",__FILE__,__LINE__);
        WRITEMSG
        goto ErrExit;	
    }
    /* ��鿨�ĺϷ��� */
    if (pub_base_CheckCrdNo())
    {
        sprintf(acErrMsg,"��鿨�ĺϷ��Դ���!!");
        WRITEMSG
        goto ErrExit;
    }
    
    /* ��ѯԤ��Ȩ��ɳ����Ľ�����ˮ�ռ� */
    /**����bbmk �ֶ� shangyongchao 2012/6/7 11:47:04
    ret = Chnl_single_hold_Dec_Upd(g_pub_tx.reply,"ac_no ='%s' and pt_date=%ld and pt_trace_no='%s' and premise_no='%s'",\
        g_pub_tx.crd_no,lOld_date,cOld_trace_no,cYsq_no);
    **/
    ret = Chnl_single_hold_Dec_Upd(g_pub_tx.reply,"ac_no ='%s' and pt_date=%ld and pt_trace_no='%s' and premise_no='%s' and bbmk='%s'",\
        g_pub_tx.crd_no,lOld_date,cOld_trace_no,cYsq_no,cBbmk);
    if (ret)
    {
        sprintf(acErrMsg,"Ҫ�����ļ�¼������!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"CU06");
        goto ErrExit;
    }
    ret = Chnl_single_hold_Fet_Upd(&sChnl_single_hold,g_pub_tx.reply);
    if(ret)
    {
        sprintf(acErrMsg,"Ҫ�����ļ�¼������!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"CU06");
        goto ErrExit;
    }
    if (sChnl_single_hold.sts[0] != '4')
    {
        sprintf(acErrMsg,"��¼״̬������!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"CU06");
        goto ErrExit;
    }
    if (memcmp(sChnl_single_hold.ac_no,g_pub_tx.crd_no,19)!=0)
    {
        vtcp_log("%s,%d,sChnl_single_hold.ac_no=[%s],g_pub_tx.crd_no=[%s]xk",__FILE__,__LINE__,sChnl_single_hold.ac_no,g_pub_tx.crd_no); 	
        sprintf(acErrMsg,"�������Ų�һ��!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"CU06");
        goto ErrExit;
    }
    if (pub_base_CompDblVal(sChnl_single_hold.tx_amt, g_pub_tx.tx_amt1)!=0)
    {
        sprintf(acErrMsg,"�����������ɽ�һ��!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"CU06");
        goto ErrExit;
    }
    /*����bbmk �ֶ� shangyongchao 2012/6/7 11:48:04
    ret = sql_execute("update Chnl_single_hold set sts='3',tx_amt = %lf where ac_no ='%s' and  pt_date=%ld and pt_trace_no= '%s' and premise_no='%s' ",\
         sChnl_single_hold.tx_amt,sChnl_single_hold.ac_no,sChnl_single_hold.o_pt_date,sChnl_single_hold.o_pt_trace_no,cYsq_no);
    */
    ret = sql_execute("update Chnl_single_hold set sts='3',tx_amt = %lf where ac_no ='%s' and  pt_date=%ld and pt_trace_no= '%s' and premise_no='%s' and bbmk='%s'",\
         sChnl_single_hold.tx_amt,sChnl_single_hold.ac_no,sChnl_single_hold.o_pt_date,sChnl_single_hold.o_pt_trace_no,cYsq_no,sChnl_single_hold.bbmk);
    
    if (ret)
    {
        sprintf(acErrMsg,"����ԭԤ��Ȩ��¼ʧ��!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"CU06");
        goto ErrExit;

    }
    strcpy(sChnl_single_hold.sts,"5");
    ret = Chnl_single_hold_Upd_Upd(sChnl_single_hold,g_pub_tx.reply);
    if(ret)
    {
        sprintf(acErrMsg,"����Ԥ��Ȩ������¼ʧ��!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"CU06");
        goto ErrExit;
    }
    Chnl_single_hold_Clo_Upd();
    memset(&sMdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
    ret= Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel," ac_no= '%s'",g_pub_tx.crd_no);
    if (ret)
    {
        vtcp_log("%s,%d,ac_no=[%s]���Ҵ���!",__FILE__,__LINE__,g_pub_tx.crd_no);
        strcpy(g_pub_tx.reply,"P102");
        goto ErrExit;
    }

    /* ����ǰ�ĳ�ʼ�� */
    strcpy(g_pub_tx.cur_no,"01");
    set_zd_data("0210",g_pub_tx.cur_no);
    
    strcpy(g_pub_tx.ct_ind,"2");
    g_pub_tx.tx_amt1 =0 - sChnl_single_hold.tx_amt;
    g_pub_tx.tx_amt2 =0 - sChnl_single_hold.hold_amt; /*�ָ����ƽ��*/
    g_pub_tx.ac_id_type[0]='1';  /*1-�����˻�*/
    g_pub_tx.ac_id = sMdm_ac_rel.ac_id;
    g_pub_tx.add_ind[0] = '1'; /* ���ٿ��ƽ�� */
    g_pub_tx.svc_ind = 1007;   /*  Ԥ��Ȩ���  */
    g_pub_tx.hst_ind[0] = '1'; /* ����ϸ */
    memcpy(g_pub_tx.brf,tmpstr,20);
    if (pub_acct_trance())
    {
        sprintf(acErrMsg,"pos Ԥ��Ȩ--����!!!");
        WRITEMSG
        goto ErrExit;
    }
    
    /*�˴�����¼entry_code=F0460��ֵ*/ 
    set_zd_double("0420",g_pub_tx.tx_amt1); /*���׽��*/
    set_zd_data("0930","2");/*���ֶ���*/
    /*�ж��̻��ʺ��Ƿ����,״̬�Ƿ�����*/
    if (strlen(cAc_no)==0)
    {
        sprintf(acErrMsg,"δ�����̻��ʺ�");
        WRITEMSG
        strcpy(g_pub_tx.reply,"AT08");
        goto ErrExit;
    }
    else if(strlen(cAc_no)>10)
    {
        memset(&sMdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
        ret= Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel," ac_no= '%s'",cAc_no);
        if (ret)
        {
            vtcp_log("%s,%d,ac_no=[%s]���Ҵ���!",__FILE__,__LINE__,cAc_no);
            strcpy(g_pub_tx.reply,"P102");
            goto ErrExit;
        }
        ret= Prdt_ac_id_Sel(g_pub_tx.reply,&sPrdt_ac_id,"ac_id=%ld",sMdm_ac_rel.ac_id);
        if (ret)
        {
            vtcp_log("%s,%d,ac_no[%s]��ѯprdt_ac_rel����!",__FILE__,__LINE__,cAc_no);
            strcpy(g_pub_tx.reply,"K106");
            goto ErrExit;
        }
        if (sPrdt_ac_id.ac_id_type[0]=='1')/*�̻��ʺ�����ǻ���*/
        {
            /*��ѯ�̻��ʺ��Ƿ����*/
            ret=Dd_mst_Sel(g_pub_tx.reply,&sDd_mst,"ac_id=%ld",sMdm_ac_rel.ac_id);
            if (ret)
            {
                vtcp_log("%s,%d,ac_id[%s]���Ҵ���!",__FILE__,__LINE__,sDd_mst.ac_id);
                strcpy(g_pub_tx.reply,"P102");
                goto ErrExit;
            }
            /*��ѯ�̻��ʺ�״̬�Ƿ�����*/
            if (sDd_mst.ac_sts[0]!='1')
            {
                vtcp_log("%s,%d,ac_no���˻�������!",__FILE__,__LINE__);
                strcpy(g_pub_tx.reply,"O085");
                goto ErrExit;
            }
            set_zd_data("1021",cAc_no);/*�̻��ʺ�*/
            set_zd_int("1022",1);     /*�˻����*/
            set_zd_data("0192","0000");  /*���۱�־*/
            set_zd_data("102Y",tmpstr);
            set_zd_data("102J","01");/*����*/
            set_zd_data("102K","2");/*��ת��ǽ跽*/
            set_zd_double("102F",g_pub_tx.tx_amt1);/*���׽���Ϊ����*/
            strcpy(g_pub_tx.reply,"0001");/*����D003���ڽ跽����*/
        }
        else if (sPrdt_ac_id.ac_id_type[0]=='9')/*����̻��ʺ����ڲ��ʺ�*/
        {   
            vtcp_log("%s,%d,�����ߵ�����1",__FILE__,__LINE__);
            ret=In_mst_Sel(g_pub_tx.reply,&sIn_mst,"ac_id=%ld",sMdm_ac_rel.ac_id);
            if (ret)
            {
                vtcp_log("%s,%d,ac_no[%s]���Ҵ���!",__FILE__,__LINE__,cAc_no);
                strcpy(g_pub_tx.reply,"P102");
                goto ErrExit;
            }
            if (sIn_mst.sts[0]!='1')
            {
                vtcp_log("%s,%d,ac_no[%s]���˻�������!",__FILE__,__LINE__,cAc_no);
                strcpy(g_pub_tx.reply,"O085");
                goto ErrExit;
            }
            set_zd_data("1201",cAc_no);/*�˺�*/
            set_zd_data("1204","01"); /*����*/
            set_zd_data("1205","2");/*��ת��־*/
            set_zd_double("1208",g_pub_tx.tx_amt1);/*���׽��*/
            set_zd_data("81",tmpbrf);/*�̻��ʺ�*/
            strcpy(g_pub_tx.reply,"0002");/*����A016�ڲ��ʺŽ跽����*/
            vtcp_log("%s,%d,�����ߵ�����2",__FILE__,__LINE__);
        }
    }
    else if (strlen(cAc_no)<=10)
    {
        ret = Com_item_Sel(g_pub_tx.reply,&sCom_item,"acc_no = '%s'",cAc_no);
        if(ret)
        {
            sprintf(acErrMsg,"��ѯ��Ŀʧ��!!!");
            WRITEMSG
            strcpy(g_pub_tx.reply, "B172");
            goto ErrExit;
        }
        set_zd_data("1201",cAc_no);/*�˺�*/
        set_zd_data("1204","01"); /*����*/
        set_zd_data("1205","2");/*��ת��־*/
        set_zd_double("1208",g_pub_tx.tx_amt1);/*���׽��*/
        set_zd_data("81",tmpbrf);/*�̻��ʺ�*/
        strcpy(g_pub_tx.reply,"0002");/*����A016�ڲ��ʺŽ跽����*/
    }

    int atm_qy_free_flag=0;/* atm ��ѯ��ѱ�־ 0��� 1�շ� */
    int pos_qy_free_flag=0;/* pos ��ѯ��ѱ�־ 0��� 1�շ� */
    int pos_tx_free_flag=0;/* pos ������ѱ�־ 0��� 1�շ� */
    int atm_tx_free_flag=0;/* atm ������ѱ�־ 0��� 1�շ� */
    int dscpt = 0, iselflag=0;
    dscpt = CARD_TRADE_TRAN;
    vtcp_log("%s,%d,POS��־=[%c]",__FILE__,__LINE__,cFee_ind[0]);
    if (cFee_ind[0]=='0')
        iselflag = 0;/*add by lifei for nx 2009-8-2 10:31:00 �б���pos*/
    else   
        iselflag = 1; 
        
    /*��ɳ����������ף��跽������*/
    ret = pub_card_reg(sChnl_single_hold.tx_amt,"POS",'0',&atm_qy_free_flag,&pos_qy_free_flag,&atm_tx_free_flag,&pos_tx_free_flag,iselflag,dscpt);
    if (ret)
    {
        sprintf(acErrMsg,"���뿨�Ǽǲ�ʧ��!");
        WRITEMSG
		strcpy(g_pub_tx.reply,"CU06");
        goto ErrExit;
    }
    /*set_zd_long("0040",sChnl_single_hold.trace_no);���غ�����ˮ��  ��Ҫ�õ�������ˮ��������ʹ��*/
    vtcp_log("[%s][%d] trace_no =[%ld]",__FILE__,__LINE__,g_pub_tx.trace_no);
OkExit:
    sprintf(acErrMsg,"success!!");
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;

ErrExit:
    if (memcmp(g_pub_tx.reply,"0000",4)==0)
    {
        strcpy(g_pub_tx.reply,"CU06");
    }
    sprintf(acErrMsg,"failed !! reply is[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
