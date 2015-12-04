/*************************************************
* �� �� �ţ� V1.0
* ��    ��:  liubingcun
* ������ڣ� 20110811
* �� �� ��:  spF248.c
* ����ģ��:  ��ƽ̨�Ľӿڳ��� (�޿��������Ѵ���)  
* ���������� ����0001����A016��D099������0002����A016��A017������0003����D003��D099������0004����D003��A017�� 
*            
* �����б�:  pub_com_test();���Ʊ�־���
*            Chnl_single_acct_Ins();�Ǽ�chnl_single_acct��
* �޸ļ�¼�� 
* ��    ��:20110816
* �� �� �ˣ�Ѧ��
* �޸�����:pub_com_test.c()���Ʊ�־ ����־��������ƽ̨���н��� 73����� ÿ���ֽڴ���һ�ֿ��ƻ���
insert into tx_def      values ('6247' '�޿���������'    '100000000000000000000000000000000000001
00000000000000000000000000000000000000000000000000000000000000100000000000000000000000100' 
'1' '6' '0' '3' 'null');
insert into tx_flow_def values ('6247','0','F248','#=0001@1@#=0002@5@#=0003@3@#=0004@7@#$');
insert into tx_flow_def values ('6247','1','A016','#=0000@4@#$');
insert into tx_flow_def values ('6247','2','A017','#=0000@1@#$');
insert into tx_flow_def values ('6247','3','D003','#=0000@4@#$');
insert into tx_flow_def values ('6247','4','D099','#=0000@8@#$');
insert into tx_flow_def values ('6247','5','A016','#=0000@6@#$');
insert into tx_flow_def values ('6247','6','A017','#=0000@8@#$');
insert into tx_flow_def values ('6247','7','D003','#=0000@6@#$');
insert into tx_flow_def values ('6247','8','F000','#$');/�����շѽӿ�/
insert into tx_sub_def  values ('F248','�޿����ѽӿ�','spF248','0','0');
insert into tx_sub_rel  values ('D003','1','11','0191','');
insert into tx_sub_rel  values ('D003','1','000000','0192','');
insert into tx_sub_rel  values ('D099','1','11','0191','');
insert into tx_sub_rel  values ('D099','1','000000','0192','');
*************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "chnl_tradeadd_c.h"
#include "chnl_self_trade_c.h"
#include "chnl_tradelimit_c.h"
#include "card.h" 
#include "march_info_c.h"
#include "march_mst_c.h"
#include "auth_modify_ln_rate_c.h"
#include "mdm_ac_rel_c.h"
#include "chnl_single_acct_c.h"
static char cFd24[4]; /* ҵ������ */
int spF248()
{
    int  iRet=0;
    char cAC_no1[25],cAC_no2[25];
    char cTmp_Brf[51],cTmp_str[101];
    char cBrf20[21];
    char cDc_ind[2], cCt_ind[2],cFee_flag[2];
    char cPOS[2];/*POS��־0����1����*/
    double dTx_amt=0.0;
    double dFee_amt=0.00;
    long lPt_date=0;
    long lPt_trace_no=0;
    long lCz_date=0;
    long lCz_lPt_trace_no=0;
    char cLs_ac_no[25];
    char cBrf[128];/*����ժҪ*/
    /*����pub_com_test����ʱ�����շ���ֵ*/
    char cGz_flag[2];/*���˱�־*/
    char cCtrlcmd[11];/*���Ʊ�־*/
    char cCert_no[1];/*֤������*/
    
    struct march_info_c sMarch_info;
    struct march_mst_c sMarch_mst;
    struct mdm_ac_rel_c sMdm_ac_rel;
    struct dd_mst_c sDd_mst;
    struct chnl_single_acct_c sChnl_single_acct;
    
    memset(&sMarch_mst,0x00,sizeof(sMarch_mst));
    memset(&sMarch_info,0x00,sizeof(sMarch_info));
    memset(cLs_ac_no,0x00,sizeof(cLs_ac_no));
    memset(cCtrlcmd,0x00,sizeof(cCtrlcmd));/*����pub_com_testʱ�õ�*/
    memset(cFd24, 0x0, sizeof(cFd24));
    memset(cAC_no1, 0x0, sizeof(cAC_no1));
    memset(cAC_no2, 0x0, sizeof(cAC_no2));
    memset(cDc_ind, 0x0, sizeof(cDc_ind));
    memset(cCt_ind, 0x0, sizeof(cCt_ind));
    memset(cFee_flag, 0x0, sizeof(cFee_flag));
    memset(cTmp_Brf, 0x0, sizeof(cTmp_Brf));
    memset(cBrf20, 0x0, sizeof(cBrf20));
    memset(cTmp_str,0,sizeof(cTmp_str));
    memset(&sMdm_ac_rel,0,sizeof(struct mdm_ac_rel_c));
    memset(&sDd_mst,0,sizeof(struct dd_mst_c));
    memset(cCert_no, 0x0, sizeof(cCert_no));
    memset(cPOS,0x00,sizeof(cPOS));
    memset(&sChnl_single_acct,0x00,sizeof(struct chnl_single_acct_c));

    if (pub_base_sysinit())
    {
          sprintf(acErrMsg,"��ʼ�����ýṹ����!!");
          WRITEMSG
          goto ErrExit;
    }
    get_zd_data("0240",cFd24);
    vtcp_log("%s,%d cFd24=[%s]\n",__FILE__,__LINE__,cFd24);
    get_zd_long("0520",&lPt_trace_no);
    get_zd_long("0440",&lPt_date);
    get_zd_double("0410",&dFee_amt);
    get_zd_long("0450",&lCz_date);
    get_zd_long("0780",&lCz_lPt_trace_no);
    get_zd_data("0660",cDc_ind); /*�����־*/ 
    get_zd_data("0680",cCt_ind); /*��ת��־*/ 
    get_zd_data("0710",cFee_flag); /*�շ�ģʽ*/
    get_zd_data("0300", g_pub_tx.crd_no);/*����*/
    get_zd_data("0300", cAC_no1); /*�ͻ��˺�*/  
    get_zd_data("0310", cAC_no2); /*�̻��˺Ż�������˺�*/
    get_zd_data("0790",g_pub_tx.draw_pwd);    /* ���� */
    get_zd_double("0400",&dTx_amt);/*���׽��*/
    get_zd_data("0730",cCtrlcmd);/*���Ʊ�־*/
    get_zd_data("0810",cTmp_Brf);/*ժҪ*/
    get_zd_data("0890",g_pub_tx.note_type);/*ƾ֤����*/
    get_zd_data("0590",g_pub_tx.beg_note_no);/*ƾ֤����*/
    get_zd_data("0620", cCert_no);/*֤������*/
    get_zd_data("0690",cPOS);/*����POS*/
    vtcp_log("[%s][%d],beg_note_no=[%s]",__FILE__,__LINE__,g_pub_tx.beg_note_no);
    vtcp_log("[%s][%d],note_type=[%s]",__FILE__,__LINE__,g_pub_tx.note_type); 
    memcpy(cBrf20,cTmp_Brf,20);
    strcpy(g_pub_tx.cur_no, "01");
    g_pub_tx.ct_ind[0]=cCt_ind[0];

    vtcp_log("%s,%d,���[%s],��ת[%s],�շ�ģʽ[%s],����[%s],���[%.2f]",__FILE__,__LINE__,cDc_ind,cCt_ind,cFee_flag,cAC_no1,dTx_amt);
    vtcp_log("%s,%d,����[%s],����2[%s]ժҪ[%s]",__FILE__,__LINE__,cAC_no1,cAC_no2,cTmp_Brf);
    sprintf(acErrMsg,"��������=cFee_flag=[%s]",cFee_flag);
    WRITEMSG
    pub_base_old_acno(g_pub_tx.crd_no);
    strcpy(cAC_no1,g_pub_tx.crd_no);
    pub_base_old_acno(cAC_no2);

    /*73����Ʊ�־���*/
    iRet=pub_com_test(cCtrlcmd,cCert_no,"",g_pub_tx.crd_no,"","","","");
    vtcp_log("%s,%d,cCtrlcmd=[%s],cCert_no=[s%],g_pub_tx.crd_no=[%s],",__FILE__,__LINE__,cCtrlcmd,cCert_no,g_pub_tx.crd_no);
    if (iRet)
    {
         sprintf(acErrMsg,"%s,%d,��������pub_com_test()����ʧ��",__FILE__,__LINE__);
         WRITEMSG
         goto ErrExit;	
    }
    /*Ϊ����׼��*/
    set_zd_data("1023",g_pub_tx.note_type); /*ƾ֤����*/
    set_zd_data("1024",g_pub_tx.beg_note_no); /*ƾ֤����*/
    /*��ʼ����*/
    if( cCt_ind[0] == '1')
    {
        vtcp_log("%s,%d,cCt_ind[%s]��ת��־��,�ν��ײ������ֽ�!",__FILE__,__LINE__,cCt_ind);
        strcpy(g_pub_tx.reply,"B099");
        goto ErrExit;
    }
    else if (cCt_ind[0] == '2') /*ת��*/
    {
        if (cDc_ind[0]=='1') /*��*/
        {
            if (cAC_no1[0] != '\0' && (strlen(cAC_no1)<13 ||cAC_no1[0]=='9'))
            {     /*���ڲ���*/
                  set_zd_data("1201",cAC_no1);
                  set_zd_data("120A",cBrf20);    
                  set_zd_data("1205","2"); /*ת��*/
                  set_zd_data("1204",g_pub_tx.cur_no);    
                  set_zd_double("1208",dTx_amt);
                  if (strlen(cAC_no2)>12 && cAC_no2[0] != '9')
                  {
                        set_zd_data("1011",cAC_no2);
                        set_zd_data("1012","1"); /*�������*/
                        set_zd_double("1013",dTx_amt); /*���׽��*/
                        set_zd_data("101A",g_pub_tx.cur_no);
                        set_zd_data("101B","2"); /*ת��*/
                        set_zd_data("0630",cBrf20);
                        strcpy(g_pub_tx.reply,"0001"); /*��:������,��:�ͻ� spA016,spD099*/
                        vtcp_log("%s,%d,��:�ͻ�,��:�����֧!",__FILE__,__LINE__);
                  }
                  else if (cAC_no2[0] != '\0' && (strlen(cAC_no2)<13 || cAC_no2[0]=='9'))
                  {
                        set_zd_data("1211",cAC_no2);
                        set_zd_double("1218",dTx_amt);
                        set_zd_data("121A",cBrf20);
                        set_zd_data("1215","2");/*ת��*/
                        set_zd_data("1214",g_pub_tx.cur_no);
                        strcpy(g_pub_tx.reply,"0002");/*��:�ڲ�����:����,spA016,spA017*/
                        vtcp_log("%s,%d,���ڲ���,�������֧!",__FILE__,__LINE__);
                  }
                  else
                  {
                       vtcp_log("%s,%d,cAC_no1[s%],cAC_no2[%s]���Ų�����,����!",__FILE__,__LINE__,cAC_no1,cAC_no2);
                       strcpy(g_pub_tx.reply,"B099");
                       goto ErrExit;
                  }
            }
            else if (strlen(cAC_no1)>12 && cAC_no1[0]!='9')
            {
                  vtcp_log("[%s][%d],�����֧",__FILE__,__LINE__); 
                  /*��ͻ�*/
                  set_zd_data("1021",cAC_no1);
                  set_zd_data("1022","1");/*�������*/
                  if (cAC_no1[0]=='5')
                  {
                        set_zd_data("1023",g_pub_tx.note_type); /*Ʊ������*/
                        set_zd_data("1024",g_pub_tx.beg_note_no); /*Ʊ�ݺ���*/
                  }
                  memset(cTmp_str,0,sizeof(cTmp_str));
                  get_zd_data("0710",cTmp_str);
                  memset(cTmp_str,0,sizeof(cTmp_str));
                  set_zd_data("1028",g_pub_tx.draw_pwd);
                  set_zd_double("102F",dTx_amt);    
                  set_zd_data("102K","2");/*��ת��־*/
                  set_zd_data("102J","01");
                  set_zd_data("102Y",cBrf20);
                  if (strlen(cAC_no2)>12 && cAC_no2[0] !='9')
                  { 
                       /*��ͻ����ͻ� spD003,spD099*/
                       set_zd_data("1011",cAC_no2);
                       set_zd_data("1012","1");
                       set_zd_double("1013",dTx_amt);
                       set_zd_data("101A",g_pub_tx.cur_no);
                       set_zd_data("101B","2");
                       set_zd_data("0630",cBrf20);
                       set_zd_data("1023",g_pub_tx.note_type); /*Ʊ������*/
                       set_zd_data("1024",g_pub_tx.beg_note_no); /*Ʊ�ݺ���*/
                       vtcp_log("[%s],[%d],ƾ֤����:[%s] ƾ֤����:[%s]",__FILE__,__LINE__,g_pub_tx.note_type,g_pub_tx.beg_note_no);                      
                       strcpy(g_pub_tx.reply,"0003"); /*spD003,spD099*/
                       vtcp_log("%s,%d,��ͻ����ͻ���֧!",__FILE__,__LINE__);
                  }
                  else if (cAC_no2[0] != '\0' && (strlen(cAC_no2) <13 || cAC_no2[0]=='9'))
                  {
                       /*��ͻ�����:������*/
                       /*��:�ͻ�,��:������ spD003,spA017*/
                       set_zd_data("1211",cAC_no2);
                       set_zd_double("1218",dTx_amt);
                       set_zd_data("121A",cBrf20);
                       set_zd_data("1215","2");
                       set_zd_data("1214",g_pub_tx.cur_no);
                       strcpy(g_pub_tx.reply,"0004");/*��:�ͻ�,��:������ spD003,spA017*/
                       vtcp_log("%s,%d,���������ͻ���֧!",__FILE__,__LINE__);
                  }
                  else
                  {
                       vtcp_log("%s,%d,cAC_no1[s%],cAC_no2[%s]���Ų�����,����!",__FILE__,__LINE__,cAC_no1,cAC_no2);
                       strcpy(g_pub_tx.reply,"B099");
                       goto ErrExit;
                  }
            }
            else
            {
                vtcp_log("%s,%d,cAC_no1[s%],cAC_no2[%s]���Ų�����,����!",__FILE__,__LINE__,cAC_no1,cAC_no2);
                strcpy(g_pub_tx.reply,"B099");
                goto ErrExit;   
            }
        }
        else if(cDc_ind[0] == '2')
        { /*��*/
            if (cAC_no1[0] != '\0' && (strlen(cAC_no1)<13 || cAC_no1[0]=='9'))
            {    /*���ڲ���*/
                 set_zd_data("1211",cAC_no1);
                 set_zd_double("1218",dTx_amt);
                 set_zd_data("121A",cBrf20);
                 set_zd_data("1215","2");
                 set_zd_data("1214",g_pub_tx.cur_no);
                 if (strlen(cAC_no2)>12 && cAC_no2[0] !='9')/*�ͻ�*/
                 {
                      set_zd_data("0910",g_pub_tx.tx_br_no);    
                      set_zd_data("1021",cAC_no2);
                      set_zd_data("1022","1");/*�������*/
                      set_zd_data("1028",g_pub_tx.draw_pwd);
                      set_zd_double("102F",dTx_amt);    
                      set_zd_data("102K","2");/*��ת��־*/
                      set_zd_data("102J","01");
                      set_zd_data("102Y",cBrf20);
                      strcpy(g_pub_tx.reply,"0004");/*��:�ͻ�,��:�ͻ� spD003,spA017*/
                      vtcp_log("%s,%d,��ͻ�������!",__FILE__,__LINE__);
                }
                else if (strlen(cAC_no2)<13 || cAC_no2[0]=='9')
                {
                      set_zd_data("1201",cAC_no2);
                      set_zd_data("120A",cBrf20);    
                      set_zd_data("1205","2"); /*ת��*/
                      set_zd_data("1204",g_pub_tx.cur_no);    
                      set_zd_double("1208",dTx_amt);
                      strcpy(g_pub_tx.reply,"0002");/*��:�ڲ���,��:������spA016,spA017*/
                      vtcp_log("%s,%d,���ڲ������ڲ���!",__FILE__,__LINE__);
                }
                else
                {
                       vtcp_log("%s,%d,cAC_no1[s%],cAC_no2[%s]���Ų�����,����!",__FILE__,__LINE__,cAC_no1,cAC_no2);
                       strcpy(g_pub_tx.reply,"B099");
                       goto ErrExit;
                }
            }
            else if(strlen(cAC_no1)>12 && cAC_no1[0] !='9')
            {
                  /*���ͻ�*/
                  set_zd_data("1011",cAC_no1);
                  set_zd_data("1012","1"); /*�������*/
                  set_zd_double("1013",dTx_amt); /*���׽��*/
                  set_zd_data("101A",g_pub_tx.cur_no);
                  set_zd_data("101B","2"); /*ת��*/
                  /*set_zd_data("1016",cBrf20);*/
                  set_zd_data("0630",cBrf20);
                  if (strlen(cAC_no2)>12 && cAC_no2[0] !='9')
                  {    /*��:�ͻ���:�ͻ�*/
                       set_zd_data("1021",cAC_no2);
                       set_zd_data("1022","1"); /* ������� */
                       set_zd_data("1028",g_pub_tx.draw_pwd);
                       set_zd_double("102F",dTx_amt);    
                       set_zd_data("102K","2");/*��ת��־*/
                       set_zd_data("102J","01");/*����*/
                       set_zd_data("102Y",cBrf20);
                       strcpy(g_pub_tx.ac_wrk_ind,"0000000");
                       strcpy(g_pub_tx.reply,"0003"); /*spD003,spD099*/
                       vtcp_log("%s,%d,��ͻ����ͻ���֧!",__FILE__,__LINE__);
                  }
                  else if (cAC_no2[0] != '\0' && (strlen(cAC_no2)<13 || cAC_no2[0] =='9'))
                  {    /*���ڲ���,���ͻ�*/
                       /*��:�ڲ���,��:�ͻ� spA016,spD099*/    
                       vtcp_log("%s,%d,�ʺ�[%s]",__FILE__,__LINE__,cAC_no2);
                       set_zd_data("1201",cAC_no2);
                       set_zd_data("120A",cBrf20);    
                       set_zd_data("1205","2"); /*ת��*/
                       set_zd_data("1204",g_pub_tx.cur_no);    
                       set_zd_double("1208",dTx_amt);
                       strcpy(g_pub_tx.reply,"0001"); /*��:������,��:�ͻ� spA016,spD099*/
                       vtcp_log("%s,%d,��:�ͻ�,��:�����֧!",__FILE__,__LINE__);
                  }
                  else
                  {
                       vtcp_log("%s,%d,cAC_no1[s%],cAC_no2[%s]���Ų�����,����!",__FILE__,__LINE__,cAC_no1,cAC_no2);
                       strcpy(g_pub_tx.reply,"B099");
                       goto ErrExit;
                  }
            }
        }
        else
        {
            vtcp_log("%s,%d,cDc_ind[%s]�����־��!",__FILE__,__LINE__,cDc_ind);
            strcpy(g_pub_tx.reply,"P404");
            goto ErrExit;
        }
    }
    /*else
    {
        vtcp_log("%s,%d,cDc_ind[%s]�����־��!",__FILE__,__LINE__,cDc_ind);
        strcpy(g_pub_tx.reply,"P404");
        goto ErrExit;
    }*/
    /*���¿��Ǽǲ�*/
    int atm_qy_free_flag=0;/* atm��ѯ��ѱ�־ 0��� 1�շ� */
    int pos_qy_free_flag=0;/* pos ��ѯ��ѱ�־ 0��� 1�շ� */
    int pos_tx_free_flag=0;/* pos ������ѱ�־ 0��� 1�շ� */
    int atm_tx_free_flag=0;/* atm ������ѱ�־ 0��� 1�շ� */
    int dscpt = 0;
    int iselflag=0;
    dscpt = CARD_TRADE_TRAN;
    if(cPOS[0] == '0')
    {
      iselflag = 0;
    }
     else 
    {
      iselflag = 1;
    }
    /**�Ǽǿ��Ǽǲ�**/
    iRet=pub_card_reg(dTx_amt,"POS",'0',&atm_qy_free_flag,&pos_qy_free_flag,&atm_tx_free_flag,&pos_tx_free_flag,iselflag,dscpt);
    if(iRet)
    {
        sprintf(acErrMsg,"���뿨�Ǽǲ�ʧ��!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"CU07");
        goto ErrExit;
    }
    
    /*�Ǽǵ��ʼ��˵ǼǱ�**/
    sChnl_single_acct.tx_date = g_pub_tx.tx_date;    /** ϵͳ����  **/
    sprintf(sChnl_single_acct.tx_time,"%6ld", g_pub_tx.tx_time);    /** ϵͳʱ��  **/
    /*sChnl_single_acct.trace_no = g_pub_tx.ejfno;    ** ������ˮ �ǹ�Ա��ˮ�ţ��Ǽǵ��Ǻ��Ľ�����ˮ�� Mod by CHENGBO 20110727**/
    sChnl_single_acct.trace_no = g_pub_tx.trace_no;    /** ������ˮ  **/
    memcpy(sChnl_single_acct.cur_no,"01",2); /**  ��ʱд��Ϊ01����� ***/
    memcpy(sChnl_single_acct.ac_no, cAC_no1,sizeof( sChnl_single_acct.ac_no)-1);    /** ���˺� **/
    if(cDc_ind[0]=='1')
    {
        memcpy(sChnl_single_acct.ac_no1, cAC_no1,sizeof( sChnl_single_acct.ac_no)-1);    /** �����˺� **/
        memcpy(sChnl_single_acct.ac_no2, cAC_no2,sizeof( sChnl_single_acct.ac_no)-1);    /** �տ��˺� **/
    }
    else if(cDc_ind[0]=='2')
    {
        memcpy(sChnl_single_acct.ac_no1, cAC_no2,sizeof( sChnl_single_acct.ac_no)-1);    /** �����˺� **/
        memcpy(sChnl_single_acct.ac_no2, cAC_no1,sizeof( sChnl_single_acct.ac_no)-1);    /** �տ��˺� **/
    }
    memcpy(sChnl_single_acct.tx_code,g_pub_tx.tx_code,sizeof(sChnl_single_acct.tx_code)-1);    /** ������ **/
    memcpy(sChnl_single_acct.tx_br_no,g_pub_tx.tx_br_no,sizeof(sChnl_single_acct.tx_br_no)-1);    /** ���׻��� **/
    memcpy(sChnl_single_acct.opn_br_no,sChnl_single_acct.opn_br_no,sizeof(sChnl_single_acct.opn_br_no)-1);    /** �������� **/
    memcpy(sChnl_single_acct.tel,g_pub_tx.tel,sizeof(sChnl_single_acct.tel)-1);    /** ���׹�Ա  **/
    memcpy(sChnl_single_acct.bbmk,g_pub_tx.tty,sizeof(sChnl_single_acct.bbmk)-1);    /** ϵͳ���� **/
    memcpy(sChnl_single_acct.ttyn,g_pub_tx.tty,sizeof(sChnl_single_acct.ttyn)-1);    /** �ն˺� **/
    sChnl_single_acct.pt_date = lPt_date;    /** ƽ̨����  **/
    sprintf(sChnl_single_acct.pt_trace_no ,"%ld" ,lPt_trace_no); /** ƽ̨��ˮ **/
    sChnl_single_acct.tx_amt = dTx_amt ;    /** ���׽�� **/
    sChnl_single_acct.ct_ind[0] = '2';    /** ��ת��־  ������ֻ����ת�� **/
    memcpy(sChnl_single_acct.brf,cBrf,sizeof(sChnl_single_acct.brf)-1);    /** ����ժҪ **/
    sChnl_single_acct.sts[0] = '1';    /** ����״̬��ʼΪ�ɹ� **/
    sChnl_single_acct.dz_ind[0] = '0';    /**  ����״̬ **/
    iRet = Chnl_single_acct_Ins(sChnl_single_acct);/* ������±�*/
    if(iRet)
    {
        sprintf(acErrMsg,"�Ǽǵ��ʼ��˵ǼǱ�����!!!");
        WRITEMSG
        strcpy(g_pub_tx.reply, "D107");
        goto ErrExit;
    }
    
OkExit:
	  vtcp_log("[%s],[%d],ƾ֤����:[%s] ƾ֤����:[%s]",__FILE__,__LINE__,g_pub_tx.note_type,g_pub_tx.beg_note_no);
    sprintf(acErrMsg,"����ɹ�!!");
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"����������ʧ��!! ");
    WRITEMSG
    if ( !memcmp(g_pub_tx.reply,"0000",4) || strlen(g_pub_tx.reply) ==0)
    {
        strcpy(g_pub_tx.reply,"CU06");
    }
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}

