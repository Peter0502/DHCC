/*************************************************
* �� �� �� :  V1.0        
* �������� :  ����Ӫ         
* ��    �� :  2011-07-12   
* ����ģ�� :  ��ƽ̨�Ľӿڳ��� (Ԥ��Ȩ���������)
* �������� :  sp6240.c
* �������� : Ԥ��Ȩ���������
*             ��1��Ԥ��Ȩ����
*             ��2��Ԥ��Ȩ��������
*             ��3��Ԥ��Ȩ��ɳ���
*�����б���1��pub_base_sysinit()
*          ��2��pub_acct_trance()
*           (3) pubf_acct_proc()
*ʹ��ע�⣺�������ƽ̨70��ȡֵ���жϳ������ͣ�������Ӧ���͵ĳ���
*          ʹ�ó���Ҫ�޸�pubf_acct_trance.c ��5784�д�������´��룺
*          strcmp(g_pub_tx.tx_code,"6240") &&���棬���±��룬������
*          ����
*�޸����� :                           
*�޸�ʱ�� :                                    
*�޸����� :
*���׶�����������£�
*insert into tx_def values('6240'��'Ԥ��Ȩ���������','10000000000010000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000100','1','1','0','3',null)
*insert into tx_flow_def values('6240','0','6240','#=0001@1@#=0002@2@#$');
*insert into tx_flow_def values('6240',1,'D099','#=0000@3@#$');
*insert into tx_flow_def values('6240','2','A017','#=0000@3@#$');
*insert into tx_flow_def values('6240','3','F000','#$');
*insert into tx_sub_def values('6240','Ԥ��Ȩ���������','sp6240','0','0');
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
#include "dd_parm_c.h"
int Return_bal(); /*��ѯ�������*/
int sp6240()
{
 int ret;
 long old_date;/*ԭƽ̨����*/
 char old_trace_no[13];/*ԭƽ̨��ˮ��*/
 char cFlag[2];/*�ж��Ƿ��Ǳ���POS,0�Ǳ��У�1�����У���ƽ̨�����ж�*/
 double tx_amt2=0.00;/*�������뿨�Ǽǲ�*/
 char brf[51];/*����ժҪ*/
 char ct_ind[2];/*��ת��ʶ*/
 char dc_ind[2];/*������*/
 char tmpstr[51];/*��ע*/
 char tmp_str[51];/*��������*/
 char ysq_ind[2];/*Ԥ��Ȩ��־*/
 char msr_str2[38];/*2�ŵ���Ϣ*/
 char msr_str3[142];/*3�ŵ���Ϣ*/
 char ac_no[25];/*����*/
 char cCom_acno[25];/*�̻��ʺŻ�������ʺ�*/
 char cCtrlcmd[10];/*���Ʊ�־*/
 char cGz_flag[2];/*���˱�־*/
 char cAc_no[25];
 char cBbmk[5];/*ϵͳ����*/
 struct chnl_single_hold_c sChnl_single_hold;
 struct chnl_single_hold_c mChnl_single_hold;
 struct mdm_ac_rel_c sMdm_ac_rel;
 struct prdt_ac_id_c sPrdt_ac_id; 
 struct dd_mst_c sDd_mst;
 struct in_mst_c sIn_mst;
 struct com_item_c sCom_item;
 
 memset(old_trace_no,0,sizeof(old_trace_no));
 memset(brf,0,sizeof(brf));
 memset(cFlag,0x00,sizeof(cFlag));
 memset(ct_ind,0,sizeof(ct_ind));
 memset(tmpstr,0,sizeof(tmpstr));
 memset(msr_str2,0,sizeof(msr_str2));
 memset(dc_ind,0,sizeof(dc_ind));
 memset(ac_no,0,sizeof(ac_no));
 memset(ysq_ind,0,sizeof(ysq_ind));
 memset(tmp_str,0,sizeof(tmp_str));
 memset(cCom_acno,0x00,sizeof(cCom_acno));
 memset(msr_str3,0,sizeof(msr_str3));
 memset(&sChnl_single_hold,0x00,sizeof(struct chnl_single_hold_c));
 memset(&mChnl_single_hold,0x00,sizeof(struct chnl_single_hold_c));
 memset(&sMdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
 memset(&sPrdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));
 memset(&sDd_mst,0x00,sizeof(struct dd_mst_c));
 memset(&sIn_mst,0x00,sizeof(struct in_mst_c));
 memset(&sCom_item,0x00,sizeof(struct com_item_c));
 memset(cCtrlcmd,0x00,sizeof(cCtrlcmd));
 memset(cGz_flag,0x00,sizeof(cGz_flag));
 memset(cAc_no,0x00,sizeof(cAc_no));
 memset(cBbmk,0x00,sizeof(cBbmk));
 /*��ʼ�����ýṹ*/
 if (pub_base_sysinit())
  {
     sprintf(acErrMsg,"��ʼ�����ýṹʧ��!!");
     WRITEMSG
     goto ErrExit;
  }
  vtcp_log("[%s][%d],tx_date=[%d]",__FILE__,__LINE__,g_pub_tx.tx_date);
  set_zd_long("0460",g_pub_tx.tx_date);
 /*ȡ����ֵ*/
 get_zd_data("0300",g_pub_tx.crd_no);/*ȡ�ÿ���*/
 get_zd_data("0310",cCom_acno);/*ȡ���̻��ʺ�*/
 vtcp_log("[%s][%d],�̻��ʺ�1=[%s]",__FILE__,__LINE__,cCom_acno);
 get_zd_double("0400",&g_pub_tx.tx_amt1);/*ȡ�����ѽ��*/
 get_zd_long("0450",&old_date);/*ȡ��ԭƽ̨����*/
 get_zd_data("0660",dc_ind);/*ȡ�ý�����*/
 get_zd_data("0690",cFlag);/*POS��ʶ*/
 get_zd_data("0700",ysq_ind);/*ȡ��Ԥ��Ȩ��־*/
 get_zd_data("0730",cCtrlcmd);/*������*/
 get_zd_data("0750",msr_str2);/*2�ŵ���Ϣ*/
 get_zd_data("0760",msr_str3);
 get_zd_data("0780",old_trace_no);/*ȡ��ԭƽ̨��ˮ��*/
 get_zd_data("0100",cBbmk);/*ϵͳ����*/
 /*
 get_zd_data("0790",g_pub_tx.draw_pwd);  ֧ȡ���� 
 */
 get_zd_data("0810",tmpstr);/*ժҪ*/
 strcpy(cAc_no,g_pub_tx.crd_no);
 vtcp_log("[%s][%d],�̻��ʺ�4=[%s]",__FILE__,__LINE__,cCom_acno);
 vtcp_log("%s,%d,ԭƽ̨��ˮ��==g_pub_tx.old_trace_no==[%s]",__FILE__,__LINE__,old_trace_no);
 vtcp_log("%s,%d,Ԥ��Ȩ��־==0700==[%s]",__FILE__,__LINE__,ysq_ind) ;
 vtcp_log("%s,%d,ȡ�ÿ���=[%s]",__FILE__,__LINE__,g_pub_tx.crd_no) ;
 vtcp_log("%s,%d,cCom_acno=[%s]",__FILE__,__LINE__,cCom_acno) ;
 
 ret=pub_com_test(cCtrlcmd,"","",g_pub_tx.crd_no,"","","",cGz_flag);
 if (ret)
  {
    sprintf(acErrMsg,"���ü����������");
    WRITEMSG
    goto ErrExit;
  }
  
  /*����Ǽ�Ԥ��Ȩ״̬���α�*/
  /*���Ӳ�ѯ���� shangyongchao 2012/6/7 10:58:39
  ret=Chnl_single_hold_Dec_Upd(g_pub_tx.reply,"pt_date=%ld and pt_trace_no='%s'",old_date,old_trace_no);*/
  ret=Chnl_single_hold_Dec_Upd(g_pub_tx.reply,"pt_date=%ld and pt_trace_no='%s' and bbmk='%s'",old_date,old_trace_no,cBbmk);
  if(ret)
   {
      sprintf(acErrMsg,"�����α����");
      strcpy(g_pub_tx.reply,"CU25");
      WRITEMSG
      goto ErrExit;
   }
    
  /*��ѯԤ��Ȩ��¼*/
  ret=Chnl_single_hold_Fet_Upd(&mChnl_single_hold,g_pub_tx.reply);
  if(ret)
  {
     sprintf(acErrMsg,"δ�ҵ���¼");
     WRITEMSG
     strcpy(g_pub_tx.reply,"CU25");
     Chnl_single_hold_Clo_Upd(); 
     goto ErrExit;
  }
   
  vtcp_log("%s,%d,hold_amt==[%.2f]",__FILE__,__LINE__,mChnl_single_hold.hold_amt);
  vtcp_log("%s,%d,tx_amt==[%.2f]",__FILE__,__LINE__,g_pub_tx.tx_amt1);
  
  if(memcmp(mChnl_single_hold.ac_no,g_pub_tx.crd_no,13)!=0)
   {
      sprintf(acErrMsg,"Ҫ�����Ŀ��Ų�һ��!mac_no is [%s],crd_no is [%s]",mChnl_single_hold.ac_no,g_pub_tx.crd_no);
      WRITEMSG
      strcpy(g_pub_tx.reply,"CU06");
      goto ErrExit;
   }
  
  if(mChnl_single_hold.sts[0]=='0')
   {
      sprintf(acErrMsg,"������Ч");
      WRITEMSG
      Chnl_single_hold_Clo_Upd();
      strcpy(g_pub_tx.reply,"CU64");
      goto ErrExit;
   }
   
  if (ysq_ind[0]=='1')/*�����Ԥ��Ȩ����*/
  {
     if (mChnl_single_hold.sts[0]!='1')
      {
         sprintf(acErrMsg,"Ҫ�����Ľ��׷�Ԥ��Ȩ");
         WRITEMSG
         Chnl_single_hold_Clo_Upd();
         strcpy(g_pub_tx.reply,"CU64");
         goto ErrExit;
      }
     vtcp_log("%s,%d",__FILE__,__LINE__) ;
     if(pub_base_CompDblVal(mChnl_single_hold.hold_amt,g_pub_tx.tx_amt1)==0)
      {
         mChnl_single_hold.sts[0]='0';
         mChnl_single_hold.hold_amt=0.00;
      }
      else if(pub_base_CompDblVal(mChnl_single_hold.hold_amt,g_pub_tx.tx_amt1)>0)
     {
        mChnl_single_hold.hold_amt=mChnl_single_hold.hold_amt-g_pub_tx.tx_amt1;
        memcpy(mChnl_single_hold.brf,tmpstr,30);
        mChnl_single_hold.sts[0]='0';
        vtcp_log("%s,%d",__FILE__,__LINE__) ;
     }
     else
     {
        vtcp_log("%s,%d,����������Ԥ��Ȩ���",__FILE__,__LINE__);
        strcpy(g_pub_tx.reply,"CU64");
        WRITEMSG
        Chnl_single_hold_Clo_Upd();
        goto ErrExit;
     }
     ret=Chnl_single_hold_Upd_Upd(mChnl_single_hold,g_pub_tx.reply);
     vtcp_log("%s,%d,hold_amt=[%.2f]",__FILE__,__LINE__,mChnl_single_hold.hold_amt) ;
     if(ret)
     {
        sprintf(acErrMsg,"���¼�¼����");
        strcpy(g_pub_tx.reply,"CU25");
        Chnl_single_hold_Clo_Upd();
        WRITEMSG
        goto ErrExit;
     }
     Chnl_single_hold_Clo_Upd();
     vtcp_log("%s,%d,hold_amt=[%.2f]",__FILE__,__LINE__,mChnl_single_hold.hold_amt) ;
   
       /*����ǰ��ʼ��*/
     g_pub_tx.tx_amt1=0-g_pub_tx.tx_amt1;
     g_pub_tx.add_ind[0]='0';/* �������ƽ��0�����٣�1������ */
     g_pub_tx.svc_ind=1006;/*  Ԥ��Ȩ  */
     g_pub_tx.hst_ind[0]='0';/* ������ϸ */
     memcpy(g_pub_tx.brf,tmpstr,20);
     if (pub_acct_trance())
     {
        sprintf(acErrMsg,"pos Ԥ��Ȩ--����!!!");
        WRITEMSG
        goto ErrExit;
     }
     strcpy(g_pub_tx.reply,"0000");
  } 
  
  if(ysq_ind[0]=='2') /*Ԥ��Ȩ��������*/
  {
      if (mChnl_single_hold.sts[0]!='2')
      {
         sprintf(acErrMsg,"Ҫ�����Ľ��׷�Ԥ��Ȩ����");
         WRITEMSG
         Chnl_single_hold_Clo_Upd();
         strcpy(g_pub_tx.reply,"CU64");
         goto ErrExit;
      }
      vtcp_log("%s,%d,ac_no=%s,pt_date=%ld,pt_trace_no=%s",__FILE__,__LINE__, g_pub_tx.crd_no,mChnl_single_hold.o_pt_date,mChnl_single_hold.o_pt_trace_no);
      /*���� bbmk �ֶ���Ϊ��ѯ���� shangyongchao 2012/6/7 11:03:38
      ret=sql_execute("update chnl_single_hold set sts='1' where ac_no='%s'  and pt_date=%ld and pt_trace_no='%s'",\
                       g_pub_tx.crd_no,mChnl_single_hold.o_pt_date,mChnl_single_hold.o_pt_trace_no);
      */
      ret=sql_execute("update chnl_single_hold set sts='1' where ac_no='%s'  and pt_date=%ld and pt_trace_no='%s' and bbmk='%s'",\
                       g_pub_tx.crd_no,mChnl_single_hold.o_pt_date,mChnl_single_hold.o_pt_trace_no,mChnl_single_hold.bbmk);
      vtcp_log("%s,%d,Ԥ��Ȩ��������ac_no=[%s],pt_date=[%ld],pt_trace_no=[%s]",__FILE__,__LINE__,g_pub_tx.crd_no,old_date,old_trace_no) ; 
      if (ret)
      {
        sprintf(acErrMsg,"���±�chnl_single_hold����");
        WRITEMSG
        strcpy(g_pub_tx.reply,"CU06");
        Chnl_single_hold_Clo_Upd();
        goto ErrExit;
      }
    
     vtcp_log("%s,%d,Ԥ��Ȩ��������",__FILE__,__LINE__) ; 
     /* ����ǰ�ĳ�ʼ�� */
     strcpy(g_pub_tx.cur_no,"01");/*����Ϊ�����*/
     set_zd_data("0210",g_pub_tx.cur_no);
     strcpy(g_pub_tx.ct_ind,"2");
     /*g_pub_tx.tx_amt1 = 0-mChnl_single_hold.tx_amt;*/
     g_pub_tx.tx_amt1 = 0-mChnl_single_hold.hold_amt;
     g_pub_tx.ac_id = 0;
     g_pub_tx.add_ind[0] = '1'; /* ���ӿ��ƽ�� */
     g_pub_tx.svc_ind = 1006;   /*  Ԥ��Ȩ���  */
     g_pub_tx.hst_ind[0] = '0'; /* ������ϸ */
     memcpy(g_pub_tx.brf,tmpstr,20);
     if (pub_acct_trance())
     {
        sprintf(acErrMsg,"pos Ԥ��Ȩ--����!!!");
        Chnl_single_hold_Clo_Upd();
        WRITEMSG
        goto ErrExit;
     }
     Chnl_single_hold_Clo_Upd();
     strcpy(g_pub_tx.reply,"0000");
  }
  if(ysq_ind[0]=='3')/*Ԥ��Ȩ��ɳ���*/
  {
      if (mChnl_single_hold.sts[0]!='3')
      {
         sprintf(acErrMsg,"Ҫ�����Ľ��׷�Ԥ��Ȩ���");
         WRITEMSG
         Chnl_single_hold_Clo_Upd();
         strcpy(g_pub_tx.reply,"CU64");
         goto ErrExit;
      }
      memcpy(mChnl_single_hold.brf,tmpstr,30);
      
      /*�жϳ����������ɽ���Ƿ�һ��*/
     if (pub_base_CompDblVal(mChnl_single_hold.tx_amt,g_pub_tx.tx_amt1)!=0)
     {
         sprintf(acErrMsg,"�����������ɽ�һ��");
         WRITEMSG
         strcpy(g_pub_tx.reply,"CU06");
         goto ErrExit;
      }
      vtcp_log("[%s][%d],pt_date=[%ld],pt_trace=[%s]",__FILE__,__LINE__,mChnl_single_hold.o_pt_date,mChnl_single_hold.o_pt_trace_no);  
      /**����bbmk �ֶ� shangyongchao 2012/6/7 11:05:06
      ret = sql_execute("update chnl_single_hold set sts='1', brf='%s' where ac_no='%s' and pt_date=%ld and pt_trace_no='%s'",\
      mChnl_single_hold.brf,g_pub_tx.crd_no,mChnl_single_hold.o_pt_date,mChnl_single_hold.o_pt_trace_no);
      */
      ret = sql_execute("update chnl_single_hold set sts='1', brf='%s' where ac_no='%s' and pt_date=%ld and pt_trace_no='%s' and bbmk='%s'",\
      mChnl_single_hold.brf,g_pub_tx.crd_no,mChnl_single_hold.o_pt_date,mChnl_single_hold.o_pt_trace_no,mChnl_single_hold.bbmk);
      if(ret)
      {
        sprintf(acErrMsg,"����ԭ����״̬��!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"CU25");
        Chnl_single_hold_Clo_Upd();
        goto ErrExit; 
      }
      /* ����ǰ�ĳ�ʼ�� */
      /* ����ʻ����ļ��Ŀ��ƽ�� */
      strcpy(g_pub_tx.cur_no,"01");
      set_zd_data("0210",g_pub_tx.cur_no);
      g_pub_tx.ac_id = 0;
     /*g_pub_tx.add_ind[0] = '1';*/ /* ȡ�� */
      g_pub_tx.add_ind[0] = '0';   /*ͬ���򰴸�������*/
      g_pub_tx.svc_ind = 1007;   /*  Ԥ��Ȩ���  */
      g_pub_tx.hst_ind[0] = '1';
      g_pub_tx.tx_amt1 =0-g_pub_tx.tx_amt1;
      g_pub_tx.tx_amt2=0-mChnl_single_hold.hold_amt; /*�ָ�ԭ���ƽ��*/
      memcpy(g_pub_tx.brf,tmpstr,20);
      if (pub_acct_trance())
      {
        sprintf(acErrMsg,"pos Ԥ��Ȩ��ɳ���--����!!!");
        Chnl_single_hold_Clo_Upd();
        WRITEMSG
        goto ErrExit;
      }
     /*�跽����*/
     set_zd_data("1021",g_pub_tx.crd_no);/*����*/
     set_zd_data("102J","01");/*����*/
     set_zd_double("102F",g_pub_tx.tx_amt1);/*���׽��*/
     set_zd_data("102K","2");/*��ת���*/
     ret=pubf_acct_proc("D003");
     if (ret)
     {
       sprintf(acErrMsg,"����D003�跽���ʴ���");
       WRITEMSG
       vtcp_log("%s,%d���ü��ʳ������",__FILE__,__LINE__);
       strcpy(g_pub_tx.reply,"T999");
       goto ErrExit;
     }
     vtcp_log("%s,%d,Ԥ��Ȩ��ɳ���",__FILE__,__LINE__);
     Chnl_single_hold_Clo_Upd();
     
     /*���뿨�Ǽǲ�*/
     tx_amt2=0-g_pub_tx.tx_amt1;/*��ɽ������Ǹ����*/
     int atm_qy_free_flag=0;/* atm��ѯ��ѱ�־ 0��� 1�շ� */
     int pos_qy_free_flag=0;/* pos ��ѯ��ѱ�־ 0��� 1�շ� */
     int pos_tx_free_flag=0;/* pos ������ѱ�־ 0��� 1�շ� */
     int atm_tx_free_flag=0;/* atm ������ѱ�־ 0��� 1�շ� */
     int dscpt = 0;
     dscpt = CARD_TRADE_TRAN;
     
     ret= pub_card_reg(tx_amt2,"POS",g_pub_tx.add_ind[0],&atm_qy_free_flag,&pos_qy_free_flag,&atm_tx_free_flag,&pos_tx_free_flag,"",dscpt);
     if (ret)
     { 
       sprintf(acErrMsg,"���¿��Ǽǲ�����");
       WRITEMSG
       strcpy(g_pub_tx.reply,"CU06");
       goto ErrExit;
     }
    
     /*�ж��̻��ʺ��Ƿ����,״̬�Ƿ�����*/
     if (strlen(cCom_acno)==0)
     {
        sprintf(acErrMsg,"δ�����̻��ʺ�");
        WRITEMSG
        strcpy(g_pub_tx.reply,"AT08");
        goto ErrExit;
      }
     if(strlen(cCom_acno)>10)
      {
        ret= Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel," ac_no= '%s'",cCom_acno);
        if (ret)
          {
            vtcp_log("%s,%d,ac_no=[%s]���Ҵ���!",__FILE__,__LINE__,cCom_acno);
            strcpy(g_pub_tx.reply,"P102");
            goto ErrExit;
          }
        ret= Prdt_ac_id_Sel(g_pub_tx.reply,&sPrdt_ac_id,"ac_id=%ld",g_mdm_ac_rel.ac_id);
        if (ret)
          {
             vtcp_log("%s,%d,ac_no[%s]��ѯprdt_ac_rel����!",__FILE__,__LINE__,cCom_acno);
             strcpy(g_pub_tx.reply,"K106");
             goto ErrExit;
          }
         if (sPrdt_ac_id.ac_id_type[0]=='1')/*�̻��ʺ�����ǻ���*/
          {
            /*��ѯ�̻��ʺ��Ƿ����*/
            ret=Dd_mst_Sel(g_pub_tx.reply,&sDd_mst,"ac_id=%ld",g_mdm_ac_rel.ac_id);
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
            set_zd_data("101A","01");/*����*/
            set_zd_data("101B","2");/*��ת��Ǵ���*/
            set_zd_double("1013",g_pub_tx.tx_amt1);/*���׽���Ϊ����*/
            set_zd_data("1011",cCom_acno);/*�̻��ʺ�*/
            strcpy(g_pub_tx.reply,"0001");/*����D099���ڴ�������*/
            
             
          }
         else if (sPrdt_ac_id.ac_id_type[0]=='9')/*����̻��ʺ����ڲ��ʺ�*/
          {
            ret=In_mst_Sel(g_pub_tx.reply,&sIn_mst,"ac_id=%ld",g_mdm_ac_rel.ac_id);
            if (ret)
              {
                vtcp_log("%s,%d,ac_no[%s]���Ҵ���!",__FILE__,__LINE__,cCom_acno);
                strcpy(g_pub_tx.reply,"P102");
                goto ErrExit;
              }
            if (sIn_mst.sts[0]!='1')
              {
                vtcp_log("%s,%d,ac_no[%s]���˻�������!",__FILE__,__LINE__,cCom_acno);
                strcpy(g_pub_tx.reply,"O085");
                goto ErrExit;
              }
            set_zd_data("1214","01");/*����*/
            set_zd_data("1215","2");/*��ת��־*/
            set_zd_double("1218",g_pub_tx.tx_amt1);/*���׽��*/
            set_zd_data("1211",cCom_acno);/*�̻��ʺ�*/
            strcpy(g_pub_tx.reply,"0002");/*����A017�ڲ��ʺŴ�������*/
            
            
          }
      }
     else if (strlen(cCom_acno)<=10)
     {
        ret = Com_item_Sel(g_pub_tx.reply,&sCom_item,"acc_no = '%s'",cCom_acno);
        if(ret)
       {
          sprintf(acErrMsg,"��ѯ��Ŀʧ��!!!");
          WRITEMSG
          strcpy(g_pub_tx.reply, "B172");
          goto ErrExit;
       }
       set_zd_data("1214","01");/*����*/
       set_zd_data("1215","2");/*��ת��־*/
       set_zd_double("1218",g_pub_tx.tx_amt1);
       set_zd_data("1211",cCom_acno);/*��Ŀ��*/
       strcpy(g_pub_tx.reply,"0002");/*����A017�ڲ��ʺŴ�������*/
       
     }
    
  }
  ret = Return_bal();/*�������*/
  if(ret)
  {
          sprintf(acErrMsg,"����������!!!");
          WRITEMSG
          strcpy(g_pub_tx.reply, "D103");
          goto ErrExit;
  }
OkExit:
 /*strcpy(g_pub_tx.reply,"0000");*/
 sprintf(acErrMsg,"Success!!");
 WRITEMSG
 set_zd_data(DC_REPLY,g_pub_tx.reply); 
 return 0;
ErrExit:
 sprintf(acErrMsg,"failed!,reply is [%s]",g_pub_tx.reply);
 set_zd_data(DC_REPLY,g_pub_tx.reply);
 return 1;
 
}
/*�������ĺ���*/
int Return_bal()
{
  int ret=0;
  double today_amt=0.0;
  double account_amt=0.0,limit_amt=0.0;
  double ky_bal=0.0;
  char tmpstr[41];
  
  struct dd_mst_c sDd_mst;
  struct dd_parm_c sDd_parm;
  
  memset(tmpstr,0,sizeof(tmpstr));

  strcpy(g_pub_tx.cur_no,"01");
  
  memset(&sDd_mst,0,sizeof(sDd_mst));
  memset(&sDd_parm,0x00,sizeof(struct dd_parm_c));
  
  ret = pub_base_CurGetddFile(g_pub_tx.crd_no,g_pub_tx.cur_no,&sDd_mst);
  account_amt = sDd_mst.bal; /* �������*/
  vtcp_log("%s,%d,sDd_mst_bal[%f]",__FILE__,__LINE__,sDd_mst.bal);
  
  ret = Dd_parm_Sel(g_pub_tx.reply,&sDd_parm,"prdt_no='%s'",sDd_mst.prdt_no);
  if(ret)
  {
      sprintf(acErrMsg,"���һ��ڲ�Ʒ���������!!!");
      WRITEMSG
      strcpy(g_pub_tx.reply, "W015");
      return 1;
  }
  ky_bal = sDd_mst.bal - sDd_mst.ctl_amt - sDd_mst.hold_amt - sDd_parm.min_bal;
  
  if (pub_base_CompDblVal(sDd_mst.bal,ky_bal)<0)
  {
    ky_bal = sDd_mst.bal;
  }
  if(sDd_mst.hold_sts[0] == '1' || sDd_mst.hold_sts[0] == '2')
  {
    ky_bal = 0.00;
  }
  vtcp_log("%s,%d,���ý��[%lf],���[%lf]",__FILE__,__LINE__,ky_bal,account_amt);
  memcpy(tmpstr,"1001156C",8);
  sprintf(tmpstr+8,"%012.0f",account_amt*100);
  memcpy(tmpstr+20,"1002156C",8);	/**modify 2012-04-12**/
  sprintf(tmpstr+28,"%012.0f",ky_bal*100);
  vtcp_log("%s,%d,tmpstr[%s]",__FILE__,__LINE__,tmpstr);
  vtcp_log("%s,%d,���������ʽ��54��[%s]",__FILE__,__LINE__,tmpstr);
  set_zd_data("0820",tmpstr);
  return(0);
}
