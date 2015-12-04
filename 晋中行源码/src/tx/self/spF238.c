/*********************************************************************
* �� �� �� :  V1.0        
* �������� :  ������          
* ��    �� :  2011-07-18   
* ����ģ�� :  POSԤ��Ȩ����           
* �������� :  spF238.c
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
insert into tx_def values('6238','Ԥ��Ȩ����','10000000000010000000000010000100000000000000000000000000000000000000000000000000011000000000000000000100000000000000000000000100','1','1','0','3',null);
insert into tx_flow_def values('6238','0','F238','#$');
insert into tx_sub_def values('F238','Ԥ��Ȩ����','spF238','0','0');
*************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "chnl_single_hold_c.h"
#include "mdm_ac_rel_c.h"
#include "dd_mst_c.h"
#include "dd_parm_c.h"
int spF238()
{
          
    int ret=0;
    char cYsq_ind[2];/*Ԥ��Ȩ��־ 2ΪԤ��Ȩ����*/
    /*char cDc_ind[2];�����־��1�裬2��*/
    char cMsr_str2[38];/*2�ŵ���Ϣ*/
    char cMsr_str3[142];/*3�ŵ���Ϣ*/
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
    char cBrf[51];/*ժҪ*/
    char cYsq_no[11];/*Ԥ��Ȩ��*/
    long lOld_pt_date=0;/*ԭƽ̨����*/
    char cOld_pt_trace_no[13];/*ԭƽ̨��ˮ*/
    long lPt_date=0;/*ƽ̨����*/
    char cPt_time[7];/*ƽ̨ʱ��*/
    char cPt_trace_no[13];/*ƽ̨��ˮ*/
    char cDz_ref[77];/*���˲ο�����*/
    char cHx_term[5];/*�ն˺�*/
    double account_amt=0.0;/*�������*/
    double ky_bal=0.0;/*�������*/
    char tmpstr[41];
    /*����pub_com_test����ʱ�����շ���ֵ*/
    char cGz_flag[2];/*���˱�־*/
    struct chnl_single_hold_c sChnl_single_hold_old;
    struct chnl_single_hold_c sChnl_single_hold_new;
    struct mdm_ac_rel_c sMdm_ac_rel;
    struct dd_mst_c sDd_mst;
    struct dd_parm_c sDd_parm;

    memset(&sChnl_single_hold_old,0x0,sizeof(struct chnl_single_hold_c));
    memset(&sChnl_single_hold_new,0x0,sizeof(struct chnl_single_hold_c));
    memset(&sMdm_ac_rel,0x0,sizeof(struct mdm_ac_rel_c));
    memset(&sDd_mst,0x0,sizeof(struct dd_mst_c));
    memset(&sDd_parm,0x00,sizeof(struct dd_parm_c));
    memset(cYsq_ind,0,sizeof(cYsq_ind));
   /* memset(cDc_ind,0,sizeof(cDc_ind));*/ 
    memset(cMsr_str2,0,sizeof(cMsr_str2));
    memset(cMsr_str3,0,sizeof(cMsr_str3));
    memset(cCtrlcmd,0,sizeof(cCtrlcmd));
    memset(cBrf,0,sizeof(cBrf));
    memset(cYsq_no,0,sizeof(cYsq_no));
    memset(cPt_trace_no,0,sizeof(cPt_trace_no));
    memset(cPt_time,0,sizeof(cPt_time));
    memset(cOld_pt_trace_no,0,sizeof(cOld_pt_trace_no));
    memset(cDz_ref,0,sizeof(cDz_ref));
    memset(cHx_term,0,sizeof(cHx_term));
    memset(tmpstr,0,sizeof(tmpstr));
    /*����pub_com_testʱ�õ�*/
    memset(cGz_flag,0x00,sizeof(cGz_flag));
    /*��ʼ��*/
    if(pub_base_sysinit())
    {
       sprintf(acErrMsg,"��ʼ�����ýṹ����!!");
       WRITEMSG
       goto ErrExit;
    }
    vtcp_log("[%s][%d],tx_date=[%d]",__FILE__,__LINE__,g_pub_tx.tx_date);
    set_zd_long("0460",g_pub_tx.tx_date);
    /*ȡ����*/
    get_zd_long("0440",&lPt_date);/*ƽ̨����*/
    vtcp_log("%s,%d,ƽ̨����=[%ld]",__FILE__,__LINE__,lPt_date);
    get_zd_data("0170",cPt_time);/*ƽ̨ʱ��*/
    vtcp_log("%s,%d,ƽ̨ʱ��=[%s]",__FILE__,__LINE__,cPt_time);
    get_zd_data("0520",cPt_trace_no);/*ƽ̨��ˮ*/
    vtcp_log("%s,%d,ƽ̨��ˮ=[%s]",__FILE__,__LINE__,cPt_trace_no);
    get_zd_data("0100",cHx_term);/*�ն˺�*/
    vtcp_log("%s,%d,�ն˺�=[%s]",__FILE__,__LINE__,cHx_term);
    get_zd_data("0700",cYsq_ind);/*Ԥ��Ȩ��־*/
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
    vtcp_log("%s,%d,���ƽ��=[%lf]",__FILE__,__LINE__,g_pub_tx.tx_amt1);
    /*
    get_zd_data("0790",g_pub_tx.draw_pwd);��������
    vtcp_log("%s,%d,��������=[%s]",__FILE__,__LINE__,g_pub_tx.draw_pwd);
    */
    
    /*���Ԥ��Ȩ��־�Ƿ�ΪԤ��Ȩ����*/
    if(cYsq_ind[0] != '2')
    {
      sprintf(acErrMsg,"Ԥ��Ȩ��־����");
      WRITEMSG
        goto ErrExit;
    }
    
    /*��鿨�ĺϷ���*/
    
    if(pub_base_CheckCrdNo())
    {
        sprintf(acErrMsg,"��鿨�ĺϷ��Դ���");
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
    /*����ԭ��¼*/
    /*����bbmk�ֶ���Ϊ��ѯ���� shangyongchao 2012/6/7 10:48:00 ��Ϊ�α����
    ret = Chnl_single_hold_Sel(g_pub_tx.reply,&sChnl_single_hold_old,"ac_no='%s' and pt_date=%ld and pt_trace_no= %s \
        and premise_no='%s' ",g_pub_tx.crd_no,lOld_pt_date,cOld_pt_trace_no,cYsq_no);*/
    ret = Chnl_single_hold_Dec_Upd(g_pub_tx.reply,"ac_no='%s' and pt_date=%ld and pt_trace_no= %s \
        and premise_no='%s' and bbmk='%s' ",g_pub_tx.crd_no,lOld_pt_date,cOld_pt_trace_no,cYsq_no,cHx_term);
    if(ret)
    {
        sprintf(acErrMsg,"�����α����!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"CU06");
        goto ErrExit;
    }
    ret = Chnl_single_hold_Fet_Upd(&sChnl_single_hold_old,g_pub_tx.reply);
    if(ret)
    {
        sprintf(acErrMsg,"ȡ��¼����!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"CU06");
        goto ErrExit;
    }
    vtcp_log("%s,%d,ԭsts=[%c]",__FILE__,__LINE__,sChnl_single_hold_old.sts[0]);
    
    /*���ԭ��¼״̬*/
    if(sChnl_single_hold_old.sts[0] != '1')
    {
      sprintf(acErrMsg,"ԭ��¼״̬��ΪԤ��Ȩ");
      WRITEMSG
      strcpy(g_pub_tx.reply,"CU06");
      goto ErrExit;
    } 
    /*�����*/
    vtcp_log("%s,%d,��Ȩ���=[%f]",__FILE__,__LINE__,sChnl_single_hold_old.hold_amt);
    if(pub_base_CompDblVal(sChnl_single_hold_old.hold_amt,g_pub_tx.tx_amt1) != 0)
    {
      sprintf(acErrMsg,"���������Ԥ��Ȩ������!!");
      WRITEMSG
      strcpy(g_pub_tx.reply,"P008");
      goto ErrExit;
    }
      
    /*����Ҫ�����ļ�¼*/
    vtcp_log("%s,%d,��Ȩ��=[%s]",__FILE__,__LINE__,sChnl_single_hold_old.premise_no);
    /*����ԭ��¼*/
    /*��Ϊ�α����
    ret=sql_execute2("update chnl_single_hold set sts='2' where ac_no='%s' \
  and pt_date=%ld and pt_trace_no= %s and premise_no='%s'",g_pub_tx.crd_no,lOld_pt_date,cOld_pt_trace_no,cYsq_no);
  */
    sChnl_single_hold_old.sts[0] ='2';
    ret = Chnl_single_hold_Upd_Upd(sChnl_single_hold_old,g_pub_tx.reply);
    if(ret)
    {
        sprintf(acErrMsg,"���³�����¼����!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"CU06");
        goto ErrExit;
    }
    Chnl_single_hold_Clo_Upd();
     /*���¼�¼�ṹ��ֵ*/    
    sChnl_single_hold_new.tx_date = g_pub_tx.tx_date;/*��������*/
    sprintf(sChnl_single_hold_new.tx_time,"%d",g_pub_tx.tx_time);/*���Ľ���ʱ��*/
    sChnl_single_hold_new.trace_no = g_pub_tx.trace_no;/*������ˮ��*/
    strcpy(sChnl_single_hold_new.cur_no,g_pub_tx.cur_no);/*���֣�01*/
    strcpy(sChnl_single_hold_new.ac_no,g_pub_tx.crd_no);/*����*/
    strcpy(sChnl_single_hold_new.tx_code,g_pub_tx.tx_code);/*���״���*/
    strcpy(sChnl_single_hold_new.tx_br_no,g_pub_tx.tx_br_no);/*���׻���*/
    strcpy(sChnl_single_hold_new.opn_br_no,g_pub_tx.opn_br_no);/*��������*/
    strcpy(sChnl_single_hold_new.tel,g_pub_tx.tel);/*��Ա��*/
    /*strcpy(sChnl_single_hold_new.bbmk,"COMM");ϵͳ����*/
    strcpy(sChnl_single_hold_new.bbmk,cHx_term);/*��Ϊ��10���͵�ֵ shangyongchao 2012/6/7 10:44:22*/
    strcpy(sChnl_single_hold_new.ttyn,cHx_term);/*�ն˺�*/
    sChnl_single_hold_new.pt_date = lPt_date;/*ƽ̨����*/
    strcpy(sChnl_single_hold_new.pt_time,cPt_time);/*ƽ̨ʱ��*/
    strcpy(sChnl_single_hold_new.pt_trace_no,cPt_trace_no);/*ƽ̨��ˮ*/
    strcpy(sChnl_single_hold_new.premise_no,sChnl_single_hold_old.premise_no);/*Ԥ��Ȩ��*/
    sChnl_single_hold_new.hold_amt = g_pub_tx.tx_amt1;/*���ƽ��*/
    sChnl_single_hold_new.tx_amt = sChnl_single_hold_old.tx_amt;/*���׽��*/
    strcpy(sChnl_single_hold_new.brf,cBrf);/*ժҪ*/
    strcpy(sChnl_single_hold_new.sts,"2");/*Ԥ��Ȩ״̬*/
    sChnl_single_hold_new.o_tx_date = sChnl_single_hold_old.tx_date;/*Ԥ��Ȩ��������*/
    sprintf(sChnl_single_hold_new.o_trace_no,"%d",sChnl_single_hold_old.trace_no);/*Ԥ��Ȩ������ˮ*/
    sChnl_single_hold_new.o_pt_date = sChnl_single_hold_old.pt_date;/*Ԥ��Ȩƽ̨ʱ��*/
    strcpy(sChnl_single_hold_new.o_pt_trace_no,sChnl_single_hold_old.pt_trace_no);/*Ԥ��Ȩƽ̨��ˮ*/
    strcpy(sChnl_single_hold_new.dz_ind,"0");/*���˱�־*/
    strcpy(sChnl_single_hold_new.dz_ref,cDz_ref); /*���˲ο�*/   
    Chnl_single_hold_Debug(&sChnl_single_hold_new);
    /* �Ǽǵ���Ԥ��Ȩ�Ǽǲ�*/
    ret =Chnl_single_hold_Ins(sChnl_single_hold_new,g_pub_tx.reply);
    if (ret)
    {
        sprintf(acErrMsg,"�Ǽǵ���Ԥ��Ȩ�Ǽǲ�����!!");
        WRITEMSG
        goto ErrExit;
    }    
       
    /* �޸��ʻ��Ŀ��ƽ����ʻ��ļ�� */ 
    strcpy(g_pub_tx.cur_no,"01");/*�����趨Ϊ�����*/
    set_zd_data("0210",g_pub_tx.cur_no);
    ret=pub_base_CurToSeqn(g_pub_tx.cur_no,&g_pub_tx.ac_seqn);/*���ݱ��������˻����*/
    if(ret)
    {
        sprintf(acErrMsg,"���ݱ��������˻���Ŵ��� cur_no=[%s]",g_pub_tx.cur_no); 
        WRITEMSG
        strcpy(g_pub_tx.reply,"AT08");
        goto ErrExit;
    }
    g_pub_tx.ac_id = 0;
    g_pub_tx.tx_amt1 = sChnl_single_hold_old.hold_amt; /*���*/
    g_pub_tx.add_ind[0] = '1';  /*���ٿ��ƽ��*/
    g_pub_tx.svc_ind = 1006;    /*Ԥ��Ȩ*/ 
    g_pub_tx.hst_ind[0] = '0'; /*������ϸ*/
    memcpy(g_pub_tx.brf,cBrf,20);
         
   
    if (pub_acct_trance())
    {
        sprintf(acErrMsg,"Ԥ��Ȩ����--����!!!");
        WRITEMSG
        goto ErrExit;
    }
    /* end of �޸��ʻ��Ŀ��ƽ����ʻ��ļ�� */
     
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
    ret = Dd_parm_Sel(g_pub_tx.reply,&sDd_parm,"prdt_no='%s'",sDd_mst.prdt_no);
    if(ret)
    {
        sprintf(acErrMsg,"��ѯdd_parm����!!!");
        strcpy(g_pub_tx.reply,"W015");
        WRITEMSG
        goto ErrExit;
    }
    ky_bal = sDd_mst.bal - sDd_mst.ctl_amt - sDd_mst.hold_amt;
    if (pub_base_CompDblVal(sDd_mst.bal,ky_bal)<0)
    {
        ky_bal = sDd_mst.bal;
    }
    if(sDd_mst.hold_sts[0] == '1' || sDd_mst.hold_sts[0] == '0')
    {
        ky_bal = 0.00;
    }
    vtcp_log("%s,%d,���ý��[%lf],���[%lf]",__FILE__,__LINE__,ky_bal,account_amt);
    memcpy(tmpstr,"1001156C",8);
    sprintf(tmpstr+8,"%012.0f",account_amt*100);
    memcpy(tmpstr+20,"1002156C",8);	/**modify 2012-04-12**/
    sprintf(tmpstr+28,"%012.0f",ky_bal*100);
    vtcp_log("%s,%d,tmpstr[%s]",__FILE__,__LINE__,tmpstr);
    set_zd_data("0820",tmpstr);
    set_zd_long("0040",sChnl_single_hold_new.trace_no);/*���غ�����ˮ��*/
    /*����������*/
    
    
OkExit:
  strcpy( g_pub_tx.reply, "0000" );
  sprintf(acErrMsg,"success!!");
  WRITEMSG
  set_zd_data(DC_REPLY,g_pub_tx.reply);
  return 0;

 ErrExit:
  if(strlen(g_pub_tx.reply) == 0 || memcmp(g_pub_tx.reply,"0000",4)==0)
  {
      strcpy(g_pub_tx.reply,"CU06");
  }
  WRITEMSG
  set_zd_data(DC_REPLY,g_pub_tx.reply);
  return 1;
}
    
  
