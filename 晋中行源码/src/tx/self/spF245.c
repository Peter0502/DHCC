/*********************************************************************
* �� �� �� :  V1.0        
* �������� :  ������          
* ��    �� :  2011-07-20   
* ����ģ�� :  POSԤ��Ȩ��ɳ�����ֻ�����п���           
* �������� :  spF245.c
* �������� : 
* �����б� :       
*         1��pub_base_sysinit();��ʼ������
*         2��pub_base_CheckCrdNo();��鿨�Ϸ��Ժ���
*         3��pubf_card_chk_msr();��鿨�ŵ���Ϣ����
*         4��pub_base_DesChk();���֧�����뺯��
*         5��pub_base_CompDblVal();��ֵ�ȽϺ���
*         6��pub_acct_trance();���˺���                 
* ʹ��ע�� :                                       
* �޸ļ�¼ :                                       
*      ��    ��:20110816
*      �� �� �ˣ�Ѧ��
*      �޸�����:pub_com_test.c()���Ʊ�־ ����־��������ƽ̨���н��� 73����� ÿ���ֽڴ���һ�ֿ��ƻ���
*insert into tx_def values('6245','Ԥ��Ȩ��ɳ���','10000000000010000000000010000100000000000000000000000000000000000000000000000000011000000000000000000100000000000000000000000100','1','1','0','3',null);
*insert into tx_flow_def values('6245','0','F245','#=0000@1@#=0001@2@#$');
*insert into tx_flow_def values('6245','1','A016','#=0000@3@#$');
*insert into tx_flow_def values('6245','2','D003','#=0000@3@#$');
*insert into tx_flow_def values('6245','3','F000','#$');
*insert into tx_sub_def values('F245','Ԥ��Ȩ��ɳ���','spF245','1','0');
*insert into tx_dc_rel values('F245','0000','F04200');
*insert into tx_sub_rel values('6245','A017','0','0020','0020','��������');
*insert into tx_sub_rel values('6245','A016','0','0020','0020','��������');
*insert into tx_sub_rel values('6245','D003','1','1001','0193','��ͨ��ȡ');
*insert into tx_sub_rel values('6245','D003','1','000110','0192','���Ʊ�־');
*insert into tx_sub_rel values('6245','D003','1','1','0191','����0190');
����������˾Ͳ��ü��ˣ�F04200 �õ�ԭ����
*insert into dc_entry values('F04200','1','0210','@0152','0930','1','1','2','0420','POSԤ��Ȩ��ɳ���');
*************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "card.h"
#include "chnl_single_hold_c.h"
#include "mdm_ac_rel_c.h"
#include "prdt_ac_id_c.h"
#include "com_item_c.h"
#include "in_mst_c.h"
int spF245()
{
  int ret;
  char flag;
  long lOld_pt_date;/*ԭƽ̨����*/
  char cOld_pt_trace_no[13]/*ԭƽ̨��ˮ*/;
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
  char cYsq_no[11];/*Ԥ��Ȩ��*/
  /*char cDc_ind[2];�����־*/
  char cFee_ind[2];/*POS��־��0Ϊ����POS��1Ϊ����POS*/
  /*double dFee_amt=0.00;��ʱû�õ�*/
  long lPt_date=0;/*ƽ̨����*/
  char cPt_time[7];/*ƽ̨ʱ��*/
  char cPt_trace_no[13];/*ƽ̨��ˮ��*/  
  char cBrf[51];/*ժҪ*/
  char cDz_ref[77];/*���˲ο�����*/
  char cHx_term[5];/*�ն˺�*/    
  char cMsr_str2[38];/*2�ŵ���Ϣ*/
  char cMsr_str3[142];/*3�ŵ���Ϣ*/
  char cTmp_str[51];
  double dTx_amt2=0.00;
  char cAc_no[25];
  char cTmp_brf[21];
  char cLs_ac_no[25];
  /*����pub_com_test����ʱ�����շ���ֵ*/
  char cGz_flag[2];/*���˱�־*/
  
  double account_amt=0.0;/*�������*/
  double ky_bal=0.0;/*�������*/
  char tmpstr[41];
  struct chnl_single_hold_c sChnl_single_hold_old;/*Ԥ��Ȩ��ɵļ�¼*/
  struct chnl_single_hold_c sChnl_single_hold_ysq;/*Ԥ��Ȩ��¼*/
  struct chnl_single_hold_c sChnl_single_hold_new;/*�µļ�¼*/
  struct mdm_ac_rel_c  sMdm_ac_rel;
  struct dd_mst_c sDd_mst;
  struct mdm_ac_rel_c sMdm_ac_rel_sh;
  struct prdt_ac_id_c sPrdt_ac_id;
  struct com_item_c sCom_item;
  struct in_mst_c sIn_mst;
  
  /*
  struct march_info_c sMarch_info;
  struct march_mst_c sMarch_mst;
  
  memset(&sMarch_mst,0x00,sizeof(sMarch_mst));
  memset(&sMarch_info,0x00,sizeof(sMarch_info));
  */
  memset(cOld_pt_trace_no,0,sizeof(cOld_pt_trace_no));
  memset(cYsq_no,0,sizeof(cYsq_no));
  memset(cYsq_ind,0,sizeof(cYsq_ind));
  /*memset(cDc_ind,0,sizeof(cDc_ind));*/
  memset(cFee_ind,0,sizeof(cFee_ind));
  memset(cPt_time,0,sizeof(cPt_time));
  memset(cPt_trace_no,0,sizeof(cPt_trace_no));
  memset(cBrf,0,sizeof(cBrf));
  memset(cDz_ref,0,sizeof(cDz_ref));
  memset(cTmp_str,0,sizeof(cTmp_str));
  memset(cHx_term,0,sizeof(cHx_term));
  memset(cMsr_str2,0,sizeof(cMsr_str2));
  memset(cMsr_str3,0,sizeof(cMsr_str3));  
  memset(cCtrlcmd,0,sizeof(cCtrlcmd));
  memset(cAc_no,0,sizeof(cAc_no));
  memset(cLs_ac_no,0x00,sizeof(cLs_ac_no)); 
  memset(cTmp_brf,0,sizeof(cTmp_brf));
  memset(tmpstr,0,sizeof(tmpstr));
  memset(&sMdm_ac_rel,0,sizeof(struct mdm_ac_rel_c));
  memset(&sChnl_single_hold_new,0x0,sizeof(struct chnl_single_hold_c));
  memset(&sChnl_single_hold_ysq,0x0,sizeof(struct chnl_single_hold_c));
  memset(&sChnl_single_hold_old,0x0,sizeof(struct chnl_single_hold_c));
  memset(&sDd_mst,0x0,sizeof(struct dd_mst_c));
  memset(&sMdm_ac_rel_sh,0x0,sizeof(struct mdm_ac_rel_c));
  memset(&sPrdt_ac_id,0x0,sizeof(struct prdt_ac_id_c));
  memset(&sCom_item,0x0,sizeof(struct com_item_c));
  memset(&sIn_mst,0x0,sizeof(struct in_mst_c));
  /*����pub_com_testʱ�õ�*/
  memset(cGz_flag,0x00,sizeof(cGz_flag));
  /*
  memset(&chnl_marchinfo,0x0,sizeof(struct chnl_marchinfo_c));
  */
  
  /*��ʼ��*/
  if (pub_base_sysinit())
  {
    sprintf(acErrMsg,"��ʼ�����ýṹ����!!");
    WRITEMSG
    goto ErrExit;
  }
  vtcp_log("[%s][%d],tx_date=[%d]",__FILE__,__LINE__,g_pub_tx.tx_date);
  set_zd_long("0460",g_pub_tx.tx_date);
    
  get_zd_long("0440",&lPt_date);/*ƽ̨����*/
  vtcp_log("%s,%d,ƽ̨����=[%ld]",__FILE__,__LINE__,lPt_date);
  get_zd_data("0170",cPt_time);/*ƽ̨ʱ��*/
  vtcp_log("%s,%d,ƽ̨ʱ��=[%s]",__FILE__,__LINE__,cPt_time);
  get_zd_data("0520",cPt_trace_no);/*ƽ̨��ˮ*/
  vtcp_log("%s,%d,ƽ̨��ˮ=[%s]",__FILE__,__LINE__,cPt_trace_no);
  get_zd_data("0100",cHx_term);/*�ն˺�*/
  vtcp_log("%s,%d,�ն˺�=[%s]",__FILE__,__LINE__,cHx_term);
  get_zd_data("0700",cYsq_ind);/*Ԥ��Ȩ��־ 4λԤ��Ȩ��ɳ���*/
  vtcp_log("%s,%d,Ԥ��Ȩ��־=[%c]",__FILE__,__LINE__,cYsq_ind[0]);  
  /*get_zd_data("0660",cDc_ind);�����־��1�裬2��*/

  get_zd_data("0750",cMsr_str2);/*�ŵ�2��Ϣ*/
  vtcp_log("%s,%d,2�ŵ���Ϣ=[%s]",__FILE__,__LINE__,cMsr_str2);
  get_zd_data("0760",cMsr_str3);/*�ŵ�3��Ϣ*/
  vtcp_log("%s,%d,3�ŵ���Ϣ=[%s]",__FILE__,__LINE__,cMsr_str3);
  get_zd_data("0730",cCtrlcmd);/*���Ʊ�־*/
  vtcp_log("%s,%d,���Ʊ�־=[%s]",__FILE__,__LINE__,cCtrlcmd);
  get_zd_data("0810",cBrf);/*ժҪ*/
  vtcp_log("%s,%d,ժҪ=[%s]",__FILE__,__LINE__,cBrf);
  get_zd_data("0810",cTmp_str);
  get_zd_long("0450",&lOld_pt_date);/*ԭƽ̨����*/
  vtcp_log("%s,%d,ԭƽ̨����=[%ld]",__FILE__,__LINE__,lOld_pt_date);
  get_zd_data("0780",cOld_pt_trace_no);/*ԭƽ̨��ˮ*/ 
  vtcp_log("%s,%d,ԭƽ̨��ˮ=[%s]",__FILE__,__LINE__,cOld_pt_trace_no);
  get_zd_data("0740",cDz_ref);/*���˲ο�*/
  vtcp_log("%s,%d,���˲ο�=[%s]",__FILE__,__LINE__,cDz_ref);
  get_zd_data("0870",cYsq_no);/*Ԥ��Ȩ��*/
  vtcp_log("%s,%d,Ԥ��Ȩ��=[%s]",__FILE__,__LINE__,cYsq_no);
  get_zd_data("0300",g_pub_tx.crd_no);/*����*/
  vtcp_log("%s,%d,����=[%s]",__FILE__,__LINE__,g_pub_tx.crd_no);
  get_zd_double("0400",&g_pub_tx.tx_amt1);/*���ƽ��*/

  vtcp_log("%s,%d,�������[%.2f]",__FILE__,__LINE__,g_pub_tx.tx_amt1);
 /********   
  get_zd_data("0790",g_pub_tx.draw_pwd);��������
******/
  /*vtcp_log("%s,%d,��������=[%s]",__FILE__,__LINE__,g_pub_tx.draw_pwd);*/
  /*get_zd_double("0410",&dFee_amt);*/
  get_zd_data("0310",cAc_no);/*�˺�*/
  vtcp_log("%s,%d,cAc_no[%s]",__FILE__,__LINE__,cAc_no);
  
  get_zd_data("0690",cFee_ind);/*��������POS��־��0Ϊ����POS,1Ϊ����POS*/
  vtcp_log("%s,%d,POS��־=[%c]",__FILE__,__LINE__,cFee_ind[0]);
  memcpy(g_pub_tx.ac_no1,cAc_no,strlen(cAc_no)); /*�ڶ����ʺ�*/
  /* get_zd_data("0730",chnl_marchinfo.mrchno); �̻��� */
  memcpy(cTmp_brf,cTmp_str,20);   
  

  /*���Ԥ��Ȩ��־�Ƿ�ΪԤ��Ȩ��ɳ���*/
  vtcp_log("%s,%d,Ԥ��Ȩ��־��%c��",__FILE__,__LINE__,cYsq_ind[0]);
  if(cYsq_ind[0] != '4')
  {
        sprintf(acErrMsg,"Ԥ��Ȩ��־����");
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
    /*73����Ʊ�־���*/
   ret=pub_com_test(cCtrlcmd,"","",g_pub_tx.crd_no,"","","",cGz_flag);
   vtcp_log("%s,%d,cCtrlcmd=[%s],g_pub_tx.crd_no=[%s],cGz_flag=[%s]",__FILE__,__LINE__,cCtrlcmd,g_pub_tx.crd_no,cGz_flag);
   if (ret)
   {
     sprintf(acErrMsg,"%s,%d,��������pub_com_test()����ʧ��",__FILE__,__LINE__);
     WRITEMSG
     goto ErrExit;	
   }
  /* ��ѯԤ��Ȩ��ɵļ�¼ */
  /*����bbmk �ֶ� ������2012/6/7 11:30:32
  ret = Chnl_single_hold_Sel(g_pub_tx.reply,&sChnl_single_hold_old,"ac_no='%s' and pt_date=%ld and pt_trace_no= '%s' and premise_no='%s'",\
  g_pub_tx.crd_no,lOld_pt_date,cOld_pt_trace_no,cYsq_no);
  **/
  ret = Chnl_single_hold_Sel(g_pub_tx.reply,&sChnl_single_hold_old,\
        "ac_no='%s' and pt_date=%ld and pt_trace_no= '%s' and premise_no='%s' and bbmk='%s'",\
        g_pub_tx.crd_no,lOld_pt_date,cOld_pt_trace_no,cYsq_no,cHx_term);
  if (ret)
  { 
    sprintf(acErrMsg,"Ԥ��Ȩ��ɵļ�¼������!!");
    WRITEMSG
    strcpy(g_pub_tx.reply,"CU06");
    goto ErrExit;
  }
  
  /*���ԭ��¼״̬�Ƿ�Ϊ���״̬*/
  if (sChnl_single_hold_old.sts[0]!='3')
  {
      sprintf(acErrMsg,"��¼״̬������!!");
      WRITEMSG
      strcpy(g_pub_tx.reply,"CU06");
      goto ErrExit;
  }
  /*�����*/
  if(pub_base_CompDblVal(sChnl_single_hold_old.tx_amt, g_pub_tx.tx_amt1)!=0)
  {
    sprintf(acErrMsg,"�����������ɽ�һֱ!!");
    vtcp_log("%s,%d,��ɽ��=[%f]",__FILE__,__LINE__,sChnl_single_hold_old.tx_amt);
    WRITEMSG
    strcpy(g_pub_tx.reply,"CU06");
    goto ErrExit;
  }
  /*��Ԥ��Ȩ��ɼ�¼�� Ԥ��Ȩƽ̨���ڡ�Ԥ��Ȩƽ̨��ˮ��Ԥ��Ȩ�Ų���ԭԤ��Ȩ��¼*/
  /*����bbmk�ֶ� shangyongchao 2012/6/7 11:34:09 ��Ϊ�α����
  ret = Chnl_single_hold_Sel(g_pub_tx.reply,&sChnl_single_hold_ysq,"ac_no='%s' and pt_date =%ld and pt_trace_no = '%s' and premise_no='%s'",\
      g_pub_tx.crd_no,sChnl_single_hold_old.o_pt_date,sChnl_single_hold_old.o_pt_trace_no,cYsq_no);*/ 
  ret = Chnl_single_hold_Dec_Upd(g_pub_tx.reply,"ac_no='%s' and pt_date =%ld and pt_trace_no = '%s' and premise_no='%s' and bbmk='%s'",\
      g_pub_tx.crd_no,sChnl_single_hold_old.o_pt_date,sChnl_single_hold_old.o_pt_trace_no,cYsq_no,sChnl_single_hold_old.bbmk);
  if (ret)
  {
    sprintf(acErrMsg,"�����α����!!");
    WRITEMSG
    strcpy(g_pub_tx.reply,"CU06");
    goto ErrExit;
  }
  ret = Chnl_single_hold_Fet_Upd(&sChnl_single_hold_ysq,g_pub_tx.reply);
  if (ret)
  {
    sprintf(acErrMsg,"Ԥ��Ȩ�ļ�¼������!!");
    WRITEMSG
    strcpy(g_pub_tx.reply,"CU06");
    goto ErrExit;
  }
  vtcp_log("%s,%d,�鵽ԭԤ��Ȩ�ļ�¼��ƽ̨���ڡ�%ld��ƽ̨��ˮΪ[%s]",__FILE__,__LINE__,sChnl_single_hold_ysq.pt_date,sChnl_single_hold_ysq.pt_trace_no);
  
  sChnl_single_hold_ysq.tx_amt=0.00;
  sChnl_single_hold_ysq.sts[0]='1';
  
  /*����Ԥ��Ȩ��¼��״̬Ϊ1,��ɽ��Ϊ0*/
  /*��Ϊ�α����
  ret = sql_execute("update chnl_single_hold set sts='1',tx_amt=0 where ac_no='%s' and pt_date=%ld and pt_trace_no= '%s' and premise_no='%s'\
            ",g_pub_tx.crd_no,sChnl_single_hold_old.o_pt_date,sChnl_single_hold_old.o_pt_trace_no,cYsq_no);
  */
  ret = Chnl_single_hold_Upd_Upd(sChnl_single_hold_ysq,g_pub_tx.reply);
  if (ret)
  {
     sprintf(acErrMsg,"����Ԥ��Ȩ��¼����!!");
     WRITEMSG
     strcpy(g_pub_tx.reply,"CU06");
     goto ErrExit;
    
  }
  Chnl_single_hold_Clo_Upd();
  vtcp_log("%s,%d,������ԭԤ��Ȩ��¼��״̬�ˣ�״̬��Ϊ1��Ԥ��Ȩ״̬��",__FILE__,__LINE__);
  /* ������Ԥ��Ȩ�¼�¼�ṹ��ֵ */  
  sChnl_single_hold_new.tx_date = g_pub_tx.tx_date;/*��������*/
  sprintf(sChnl_single_hold_new.tx_time,"%d",g_pub_tx.tx_time);/*���Ľ���ʱ��*/
  sChnl_single_hold_new.trace_no = g_pub_tx.trace_no;/*������ˮ��*/
  strcpy(sChnl_single_hold_new.cur_no,g_pub_tx.cur_no);/*���֣�01*/
  strcpy(sChnl_single_hold_new.ac_no,g_pub_tx.crd_no);/*����*/
  strcpy(sChnl_single_hold_new.tx_code,g_pub_tx.tx_code);/*���״���*/
  strcpy(sChnl_single_hold_new.tx_br_no,g_pub_tx.tx_br_no);/*���׻���*/
  strcpy(sChnl_single_hold_new.opn_br_no,g_pub_tx.tx_br_no);/*��������*/
  strncpy(sChnl_single_hold_new.tel,g_pub_tx.tel,6);/*��Ա��*/
  /*strcpy(sChnl_single_hold_new.bbmk,"COMM");ϵͳ����*/
  strcpy(sChnl_single_hold_new.bbmk,cHx_term);/*ϵͳ���� ��Ϊ��10 ���� shangyongchao 2012/6/7 11:29:25*/
  strcpy(sChnl_single_hold_new.ttyn,cHx_term);/*�ն˺�*/
  sChnl_single_hold_new.pt_date = lPt_date;/*ƽ̨����*/
  strcpy(sChnl_single_hold_new.pt_time,cPt_time);/*ƽ̨ʱ��*/
  strcpy(sChnl_single_hold_new.pt_trace_no,cPt_trace_no);/*ƽ̨��ˮ*/
  strcpy(sChnl_single_hold_new.premise_no,sChnl_single_hold_old.premise_no);/*Ԥ��Ȩ��*/
  sChnl_single_hold_new.hold_amt = sChnl_single_hold_old.hold_amt;/*���ƽ��*/
  sChnl_single_hold_new.tx_amt = g_pub_tx.tx_amt1;/*���׽��*/
  strcpy(sChnl_single_hold_new.brf,cBrf);/*ժҪ*/
  strcpy(sChnl_single_hold_new.sts,"4");/*Ԥ��Ȩ��ɳ���״̬*/
  sChnl_single_hold_new.o_tx_date = sChnl_single_hold_ysq.tx_date;/*Ԥ��Ȩ��������*/
  sprintf(sChnl_single_hold_new.o_trace_no,"%d",sChnl_single_hold_ysq.trace_no);/*Ԥ��Ȩ������ˮ*/
  sChnl_single_hold_new.o_pt_date = sChnl_single_hold_ysq.pt_date;/*Ԥ��Ȩƽ̨ʱ��*/
  strcpy(sChnl_single_hold_new.o_pt_trace_no,sChnl_single_hold_ysq.pt_trace_no);/*Ԥ��Ȩƽ̨��ˮ*/
  strcpy(sChnl_single_hold_new.dz_ind,"0");/*���˱�־*/
  strcpy(sChnl_single_hold_new.dz_ref,cDz_ref); /*���˲ο�*/   
      
  Chnl_single_hold_Debug(&sChnl_single_hold_new);
  ret = Chnl_single_hold_Ins(sChnl_single_hold_new,g_pub_tx.reply);
  if (ret)
  {
    sprintf(acErrMsg,"pos ���뵥��Ԥ��Ȩ�Ǽǲ�ʧ��!!");
    WRITEMSG
    goto ErrExit;
  }
  
  /* ���Ŀ���� ��� */
  strcpy(g_pub_tx.cur_no,"01");
  set_zd_data("0210",g_pub_tx.cur_no);
  /*���ݱ��������˻����*/
  ret = pub_base_CurToSeqn(g_pub_tx.cur_no,&g_pub_tx.ac_seqn);
  if ( ret != 0)
  {
    sprintf(acErrMsg,"���ݱ��������ʻ���Ŵ���!! cur_no=[%s]",g_pub_tx.cur_no);
    WRITEMSG
    goto ErrExit;
  }
  g_pub_tx.tx_amt1=sChnl_single_hold_old.tx_amt; /*���׽��*/
  g_pub_tx.tx_amt2= sChnl_single_hold_old.hold_amt; /*�ָ����ƽ��*/
  g_pub_tx.svc_ind = 1007;   /*  Ԥ��Ȩ���  */ 
  g_pub_tx.add_ind[0] = '1';
  /*g_pub_tx.add_ind[0] = '0';*/

  g_pub_tx.hst_ind[0] = '1'; /* �ռ�����ϸ */
  g_pub_tx.ac_id = 0;
  strcpy(g_pub_tx.ct_ind,"2"); /* ת�� */
  memcpy(g_pub_tx.brf,cTmp_str,20);
  if (pub_acct_trance())
  {
    sprintf(acErrMsg,"���û�������!!!");
    WRITEMSG
    goto ErrExit;
  }
  /*�˴�����¼entry_code=F0420��ֵ*/
  set_zd_double("0420",g_pub_tx.tx_amt1);
  set_zd_data("0930","2");
  /*********************
  set_zd_data("1201",cAc_no);    

  set_zd_data("120A",cTmp_brf);
  set_zd_data("1205","2");
  set_zd_data("1204","01");
  set_zd_double("1208",g_pub_tx.tx_amt1);
  ******************/
/***#endif****/
    /*�Դ������˺Ų�ͬ����Ӧ�Ĵ�������*/
    if(strlen(cAc_no) >= 13)   /**�̻��˺�*/
    {
        ret= Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel_sh," ac_no= '%s'",cAc_no);
        if( ret )
         {
             vtcp_log("%s,%d,ac_no=[%s]���Ҵ���!",__FILE__,__LINE__,cAc_no);
             strcpy(g_pub_tx.reply,"P102");
             goto ErrExit;
         }
        ret = Prdt_ac_id_Sel(g_pub_tx.reply,&sPrdt_ac_id,"ac_id=%ld ",sMdm_ac_rel_sh.ac_id);
        if (ret)
         {
             vtcp_log("%s,%d,ac_no[%s]��ѯprdt_ac_id����!",__FILE__,__LINE__,cAc_no);
             strcpy(g_pub_tx.reply,"K106");
             goto ErrExit;
         }
         if(sPrdt_ac_id.ac_id_type[0] == '1')  /*����*/
          {
              ret=Dd_mst_Sel(g_pub_tx.reply,&sDd_mst,"ac_id=%ld",sMdm_ac_rel_sh.ac_id);
              if(ret)
                {
                       vtcp_log("%s,%d,ac_id[%s]���Ҵ���!",__FILE__,__LINE__,sMdm_ac_rel_sh.ac_id);
                       strcpy(g_pub_tx.reply,"P102");
                       goto ErrExit;
                   }
              if(sDd_mst.ac_sts[0] != '1')
                {
                       vtcp_log("%s,%d,ac_no���˻�������!",__FILE__,__LINE__);
                       strcpy(g_pub_tx.reply,"O085");
                       goto ErrExit;
                  }
              set_zd_data("1021",cAc_no);
              set_zd_data("102K","2");
              set_zd_double("102F",g_pub_tx.tx_amt1);
              strcpy(g_pub_tx.reply,"0001");
          }
         else if(sPrdt_ac_id.ac_id_type[0] == '9') /*�ڲ��˺�*/
          {
            ret=In_mst_Sel(g_pub_tx.reply,&sIn_mst,"ac_id=%ld",sMdm_ac_rel_sh.ac_id);
              if(ret)
                {
                       vtcp_log("%s,%d,ac_id[%s]���Ҵ���!",__FILE__,__LINE__,sMdm_ac_rel_sh.ac_id);
                       strcpy(g_pub_tx.reply,"P102");
                       goto ErrExit;
                   }
              if(sIn_mst.sts[0] != '1')
                {
                       vtcp_log("%s,%d,ac_no���˻�������!",__FILE__,__LINE__);
                       strcpy(g_pub_tx.reply,"O085");
                       goto ErrExit;
                  }
              set_zd_data("1201",cAc_no);    
              set_zd_data("120A",cTmp_brf);
              set_zd_data("1205","2");
              set_zd_data("1204","01");
              set_zd_double("1208",g_pub_tx.tx_amt1);
              strcpy(g_pub_tx.reply,"0000");
          }
         else
          {
              sprintf(acErrMsg,"���˻����ʹ���!!!");
              WRITEMSG
              goto ErrExit;
          }
    }
    else if(strlen(cAc_no)<13)
    {
        ret= Com_item_Sel(g_pub_tx.reply,&sCom_item,"acc_no='%s'",cAc_no);
        if(ret)
        {
            sprintf(acErrMsg,"��ѯ��Ŀʧ��!!!");
            WRITEMSG
            strcpy(g_pub_tx.reply, "B172");
            goto ErrExit;
        }
        set_zd_data("1201",cAc_no);
        set_zd_data("120A",cTmp_brf);
        set_zd_data("1205","2");
        set_zd_data("1204","01");
        set_zd_double("1208",g_pub_tx.tx_amt1);
        strcpy(g_pub_tx.reply,"0000");
    }
    else
    {
        sprintf(acErrMsg,"δ�����̻��˺Ż�������˺�!");
        WRITEMSG
        strcpy(g_pub_tx.reply, "AT08");
        goto ErrExit;
    }   
         

  int atm_qy_free_flag=0;/* atm��ѯ��ѱ�־ 0��� 1�շ� */
  int pos_qy_free_flag=0;/* pos ��ѯ��ѱ�־ 0��� 1�շ� */
  int pos_tx_free_flag=0;/* pos ������ѱ�־ 0��� 1�շ� */
  int atm_tx_free_flag=0;/* atm ������ѱ�־ 0��� 1�շ� */
  int dscpt = 0;
  int iselflag=0;
  dscpt = CARD_TRADE_TRAN;/*01*/
  vtcp_log("%s,%d,POS��־=[%c]",__FILE__,__LINE__,cFee_ind[0]); 
  if(cFee_ind[0] == '0')
  {
    iselflag = 0;
  }
  else 
  {
    iselflag = 1;
  }
  vtcp_log("%s,%d,����iselflag====%d",__FILE__,__LINE__,iselflag);
 
  
  /*�������׸���card_reg��ʱ�跽�Ǹ���*/
  
  ret = pub_card_reg(-1 * g_pub_tx.tx_amt1,"POS",'0',&atm_qy_free_flag,&pos_qy_free_flag,&atm_tx_free_flag,&pos_tx_free_flag,iselflag,dscpt);
  if (ret)
  {
    sprintf(acErrMsg,"���뿨�Ǽǲ�ʧ��!");
    WRITEMSG
    strcpy(g_pub_tx.reply,"CU06");
    goto ErrExit;
  }
  /*�������*/
  ret = pub_base_CurGetddFile(g_pub_tx.crd_no,g_pub_tx.cur_no,&sDd_mst);/*���ݱ��֣����Ų��Ҵ����Ϣ*/
  if(ret)
  {
        sprintf(acErrMsg,"��ѯ������!!");
        WRITEMSG
        goto ErrExit;
  }
  account_amt = sDd_mst.bal; /* �������*/
  vtcp_log("%s,%d,sDd_mst_bal[%f]",__FILE__,__LINE__,sDd_mst.bal);
  ky_bal = sDd_mst.bal - sDd_mst.ctl_amt - sDd_mst.hold_amt;
  if (pub_base_CompDblVal(sDd_mst.bal,ky_bal)<0)
  {
    ky_bal = sDd_mst.bal;
  }
  vtcp_log("%s,%d,���ý��[%lf],���[%lf]",__FILE__,__LINE__,ky_bal,account_amt);
  memcpy(tmpstr,0,sizeof(tmpstr));
  memcpy(tmpstr,"1001156C",8);
  sprintf(tmpstr+8,"%012.0f",account_amt*100);
  memcpy(tmpstr+20,"1002156C",8);	/**modify 2012-04-12**/
  sprintf(tmpstr+28,"%012.0f",ky_bal*100);
  vtcp_log("%s,%d,tmpstr[%s]",__FILE__,__LINE__,tmpstr);
  set_zd_data("0820",tmpstr);
  set_zd_long("0040",sChnl_single_hold_new.trace_no);/*���غ�����ˮ��*/
    /*����������*/
    
OkExit:
    /*strcpy( g_pub_tx.reply, "0000" ); �������0000 ���߲����Ƿֻ��˵ķ�֧�� 2011/12/1 16:18:52*/
    sprintf(acErrMsg,"success!!");
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;

ErrExit:
    if (strlen(g_pub_tx.reply) == 0 || memcmp(g_pub_tx.reply,"0000",4)==0)
    {
      strcpy(g_pub_tx.reply,"CU06");
    }
    sprintf(acErrMsg,"failed !! reply is[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
