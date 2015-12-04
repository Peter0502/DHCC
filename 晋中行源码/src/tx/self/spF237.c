/*************************************************
* �� �� ��:  spF237.c
* ���������� Ԥ��Ȩ�����ᣩ����
*
* ��    ��:  ʷ��֥
* ������ڣ� 2011��7����

*--Ԥ��Ȩ���ף�bit_map��δ�޸ģ�log_ind=1 :��¼ͨѶ��־��tx_type=4:������wrk_sts=0:������ǩ����
insert into tx_def
values ('6237', 'Ԥ��Ȩ����', '10000000000000000000000000000000000000001000000010000000000000000000000000000000000000000000000000000000000000000000000000000100', '1', '6', '0', '3', null);

--����ԭ����6203���ף�����½�������6237
insert into tx_flow_def 
values ('6237', 0, 'F237', '#$');


--����ӽ���,acct_ind,chnl_ind ͬF036�ӽ���
insert into tx_sub_def 
values ('F237', 'Ԥ��Ȩ����', 'spF237', '0', '0');

*  �޸ļ�¼��
*   ��   ��:2011��8��2��
*   �� �� ��:ʷ��֥
*   �޸�����:Ԥ��Ȩ����ԭ����ƽ̨��ˮ��FD52 ��Ϊ FD87 
*   ��   ��:2011��8��4��
*   �� �� ��:ʷ��֥
*   �޸�����:����check_ky_bal() ��������� 
*      ��    ��:20110816
*      �� �� �ˣ�Ѧ��
*      �޸�����:pub_com_test.c()���Ʊ�־ ����־��������ƽ̨���н��� 73����� ÿ���ֽڴ���һ�ֿ��ƻ���
*************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "chnl_single_hold_c.h"
#include "card.h"
#include "card_reg_c.h"
#include "dd_mst_c.h"
#include "dd_parm_c.h"

#define DEBUG_spF237 1
/* ע�⣺
 ��Ϊ�޷����Կ��˻���飬������ʱ��ס�ˣ�
 ���п��˻���ϵͳ����DEBUG_spF237 �궨��Ϊ 1
 */
int Return_bal(char *dc_ind); /*��ѯ�������*/
int check_ky_bal();/*����������Ƿ��㹻*/

int spF237()
{
    int ret=0;
    char cDc_ind[2];/*�����־��1�裬2��*/
    char cMsr_str2[38];/*2�ŵ���Ϣ*/
    char cMsr_str3[142];/*3�ŵ���Ϣ*/
    char cCtrlcmd[11];/*���Ʊ�־*/
    char cTtyn[21]; /*�ն˺�*/
    char cTel[7];
    char cBrf[51];/*ժҪ*/
    long lPt_date=0;/*ƽ̨����*/
    char cPt_time[7];/*ƽ̨ʱ��*/
    char lPt_trace_no[13];/*ƽ̨��ˮ*/
    char cYsq_no[11];/*Ԥ��Ȩ�� */
    /*����pub_com_test����ʱ�����շ���ֵ*/
    char cGz_flag[2];/*���˱�־*/
    struct chnl_single_hold_c sChnl_single_hold;    
    memset(&sChnl_single_hold,0x0,sizeof(struct chnl_single_hold_c));
    memset(cDc_ind,0,sizeof(cDc_ind)); 
    memset(cMsr_str2,0,sizeof(cMsr_str2));
    memset(cMsr_str3,0,sizeof(cMsr_str3));
    memset(cCtrlcmd,0,sizeof(cCtrlcmd));
    memset(cBrf,0,sizeof(cBrf));
    memset(cTel,0,sizeof(cTel));
    /*����pub_com_testʱ�õ�*/
    memset(cGz_flag,0x00,sizeof(cGz_flag));
    strcpy(g_pub_tx.cur_no,"01"); /*����*/
    
    if(pub_base_sysinit())
    {
        sprintf(acErrMsg,"��ʼ�����ýṹ����!!");
        WRITEMSG
        goto ErrExit;
    }
    vtcp_log("[%s][%d],tx_date=[%d]",__FILE__,__LINE__,g_pub_tx.tx_date);
    set_zd_long("0460",g_pub_tx.tx_date);
    
    get_zd_data("0070",cTel);/*����Ա��*/
    get_zd_data("0100",cTtyn); /*�ն˺�*/
    get_zd_data("0660",cDc_ind);/*�����־��1�裬2��*/
    get_zd_data("0750",cMsr_str2);/*�ŵ�2��Ϣ*/
    get_zd_data("0760",cMsr_str3);/*�ŵ�3��Ϣ*/
    get_zd_data("0730",cCtrlcmd);/*���Ʊ�־*/
    get_zd_data("0810",cBrf);/*ժҪ*/
    get_zd_long("0440",&lPt_date);/*ƽ̨����*/
    get_zd_data("0170",cPt_time); /*ƽ̨ʱ��*/
    get_zd_data("0520",lPt_trace_no);/*ƽ̨��ˮ*/
    get_zd_data("0870",cYsq_no);/*Ԥ��Ȩ��*/
    get_zd_data("0300",g_pub_tx.crd_no);/*����*/
    get_zd_double("0400",&g_pub_tx.tx_amt1);/*���ƽ��*/
    /*
    get_zd_data("0790",g_pub_tx.draw_pwd);��������
    */
    get_zd_data("0730",cCtrlcmd);
    
    vtcp_log("[%s],[%d]slzh123",__FILE__,__LINE__);
    vtcp_log("[%s][%d]�����־=[%s]",__FILE__,__LINE__,cDc_ind);
    vtcp_log("[%s][%d]�ն˺�=[%s]",__FILE__,__LINE__,cTtyn);
    vtcp_log("[%s][%d]2�ŵ�=[%s]",__FILE__,__LINE__,cMsr_str2);
    vtcp_log("[%s][%d]3�ŵ�=[%s]",__FILE__,__LINE__,cMsr_str3);
    vtcp_log("[%s][%d]���Ʊ�־=[%s]",__FILE__,__LINE__,cCtrlcmd);
    vtcp_log("[%s][%d]ժҪ=[%s]",__FILE__,__LINE__,cBrf);
    vtcp_log("[%s][%d]ƽ̨����=[%ld]",__FILE__,__LINE__,lPt_date);
    vtcp_log("[%s][%d]ƽ̨��ˮ=[%s]",__FILE__,__LINE__,lPt_trace_no);
    vtcp_log("[%s][%d]Ԥ��Ȩ��=[%s]",__FILE__,__LINE__,cYsq_no);
    vtcp_log("[%s][%d]����=[%s]",__FILE__,__LINE__,g_pub_tx.crd_no);
    vtcp_log("[%s][%d]���=[%lf]",__FILE__,__LINE__,g_pub_tx.tx_amt1);
    /*vtcp_log("[%s][%d]��������=[%s]",__FILE__,__LINE__,g_pub_tx.draw_pwd);*/
    vtcp_log("%s,%d,�ŵ�������λ[%c]",__FILE__,__LINE__,cCtrlcmd[1]);
    vtcp_log("%s,%d,��ˮ��[%d]",__FILE__,__LINE__,g_pub_tx.trace_no);
  
   
    /* ��鿨�ĺϷ��� */
  #if DEBUG_spF237
  
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
    #endif
    ret=check_ky_bal();
    if(ret)
    {
        strcpy( g_pub_tx.reply, "A030" );
        sprintf(acErrMsg,"�������С�ڿ��ƽ��");
        WRITEMSG
        goto ErrExit;
    }
  /*��Ԥ��Ȩ״̬��ֵ*/
  
    vtcp_log("[%s],[%d]slzh:insert into chnl_single_hold ",__FILE__,__LINE__);
    sChnl_single_hold.tx_date=g_pub_tx.tx_date; /*��������*/
    sprintf(sChnl_single_hold.tx_time,"%d",g_pub_tx.tx_time); /*����ʱ�䣬long -> char[6] */
    sChnl_single_hold.trace_no=g_pub_tx.trace_no; /*������ˮ*/
    memcpy(sChnl_single_hold.cur_no,g_pub_tx.cur_no,strlen(g_pub_tx.cur_no)); /*����*/
    memcpy(sChnl_single_hold.ac_no,g_pub_tx.crd_no,strlen(g_pub_tx.crd_no)); /*���˺�*/
    memcpy(sChnl_single_hold.tx_code,g_pub_tx.tx_code,strlen(g_pub_tx.tx_code)); /*���״���*/
    
    memcpy(sChnl_single_hold.tx_br_no,g_pub_tx.tx_br_no,strlen(g_pub_tx.tx_br_no)); /*���׻���*/
    memcpy(sChnl_single_hold.opn_br_no,g_pub_tx.opn_br_no,strlen(g_pub_tx.opn_br_no)); /*��������*/
    memcpy(sChnl_single_hold.tel,cTel,strlen(cTel)); /*����Ա*/
    memcpy(sChnl_single_hold.ttyn,cTtyn,strlen(cTtyn));/*�ն˺�ttyn */
    
    sChnl_single_hold.pt_date=lPt_date; /*ƽ̨����*/
    memcpy(sChnl_single_hold.pt_time,cPt_time,strlen(cPt_time));/*ƽ̨ʱ��*/
    
    memcpy(sChnl_single_hold.pt_trace_no,lPt_trace_no,strlen(lPt_trace_no)); /*ƽ̨��ˮ��*/
    memcpy(sChnl_single_hold.premise_no,cYsq_no,strlen(cYsq_no)); /*Ԥ��Ȩ��*/
    memcpy(sChnl_single_hold.brf,cBrf,strlen(cBrf)); /*ժҪ*/
    sChnl_single_hold.sts[0]='1'; /*Ԥ��Ȩ����*/
    sChnl_single_hold.dz_ind[0]='0'; /*���˱�־*/ 
    /*strcpy(sChnl_single_hold.bbmk,"COMM"); ϵͳ���� COMM���� */
    strcpy(sChnl_single_hold.bbmk,cTtyn); /*ϵͳ���� COMM����  ����д�� ��10���� shangyongchao 2012/6/7 10:42:31*/
    sChnl_single_hold.hold_amt = g_pub_tx.tx_amt1; /*Ԥ��Ȩ���*/
    sChnl_single_hold.tx_amt = 0.00; /*��ɽ�Ԥ��Ȩ���ʱ����Ϊ���޸�ΪԤ��Ȩ���*/
    Chnl_single_hold_Debug(&sChnl_single_hold);
    ret = Chnl_single_hold_Ins(sChnl_single_hold,g_pub_tx.reply);   
    if(ret)
    {
        sprintf(acErrMsg,"pos ����Ǽǲ�����!!");
          WRITEMSG
          strcpy(g_pub_tx.reply,"CU06");
          goto ErrExit;
    }
        
    /* �޸��ʻ��Ŀ��ƽ����ʻ��ļ�� */
    strcpy(g_pub_tx.cur_no,"01");
    set_zd_data("0210",g_pub_tx.cur_no);
    ret = pub_base_CurToSeqn(g_pub_tx.cur_no,&g_pub_tx.ac_seqn);
    if ( ret != 0)
    {
        sprintf(acErrMsg,"���ݱ��������ʻ���Ŵ���!! cur_no=[%s]",
        g_pub_tx.cur_no);
        WRITEMSG
        strcpy(g_pub_tx.reply,"CU06");
        goto ErrExit;
    }
    
       g_pub_tx.ac_id = 0; /**/
       g_pub_tx.add_ind[0] = '0'; /* ���ӿ��ƽ�� ,���ͻ�*/
    
    
    memcpy(g_pub_tx.brf,cBrf,20);
    
    g_pub_tx.svc_ind = 1006;   /*  Ԥ��Ȩ  */
    g_pub_tx.hst_ind[0] = '0'; /* ������ϸ */
    if (pub_acct_trance())
    {
        sprintf(acErrMsg,"�Ƿֻ���ʧ��!!!");
        WRITEMSG
        goto ErrExit;
    }
    ret = Return_bal(cDc_ind);  /*���*/
    if(ret)
    {
        sprintf(acErrMsg,"��ѯ������!!!");
        WRITEMSG
        goto ErrExit;    
    }
          
OkExit:
  strcpy( g_pub_tx.reply, "0000" );
  sprintf(acErrMsg,"slzh Ԥ��Ȩsuccess!!");
  WRITEMSG
  set_zd_data(DC_REPLY,g_pub_tx.reply);
  set_zd_long("0040",g_pub_tx.trace_no);/*������ˮ*/
  return 0;

ErrExit:
  if(strlen(g_pub_tx.reply) == 0 || memcmp(g_pub_tx.reply,"0000",4)==0)
    {
       strcpy(g_pub_tx.reply,"CU06");
    }
  WRITEMSG
  set_zd_data(DC_REPLY,g_pub_tx.reply);
  vtcp_log("[%s][%d]ErrExit:reply=%s",__FILE__,__LINE__,g_pub_tx.reply);
  return 1;
  
}
/**********************************************************************
* �� �� ����  Return_bal
* �������ܣ�  ���ػ����˻���ֻ�����ļ�ʹ��
* ��    �ߣ�  ʷ��֥
* ���ʱ�䣺  2011��7����
*
* ��    ����
*     ��  �룺��
*      
*     ��  ����0820��ֵ ���������ʽ��54��
*
*     ����ֵ��0����ȷִ��
*
* �޸���ʷ��
*                   
********************************************************************/
int Return_bal(char *dc_ind)
{
  int ret=0;
  double today_amt=0.0;
  double account_amt=0.0,limit_amt=0.0;
  double ky_bal=0.0;
  char tmpstr[41];
  
  struct dd_mst_c sDd_mst;
  struct dd_parm_c sDd_parm;
  
  memset(tmpstr,0x00,sizeof(tmpstr));

  strcpy(g_pub_tx.cur_no,"01");
  
  memset(&sDd_mst,0x00,sizeof(sDd_mst));
  memset(&sDd_parm,0x00,sizeof(struct dd_parm_c));
  
  ret = pub_base_CurGetddFile(g_pub_tx.crd_no,g_pub_tx.cur_no,&sDd_mst);
  account_amt = sDd_mst.bal; /* �������*/
  vtcp_log("%s,%d,sDd_mst_bal[%f]",__FILE__,__LINE__,sDd_mst.bal);
  /*���Ӷ�������ļ���*/
  ret= Dd_parm_Sel(g_pub_tx.reply,&sDd_parm,"prdt_no ='%s'",sDd_mst.prdt_no);
  if(ret)
  {
      sprintf(acErrMsg,"��ѯdd_parm����!!!");
      strcpy(g_pub_tx.reply,"W015");
      WRITEMSG
      return 1;
  }
  
  ky_bal = sDd_mst.bal - sDd_mst.ctl_amt - sDd_mst.hold_amt - sDd_parm.min_bal;
  
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
  vtcp_log("%s,%d,���������ʽ��54��[%s]",__FILE__,__LINE__,tmpstr);
  set_zd_data("0820",tmpstr);
  
  
  return(0);
}
/**********************************************************************
* �� �� ����  check_ky_bal
* �������ܣ�  ����������Ƿ���ڶ����ֻ�����ļ�ʹ��
* ��    �ߣ�  ʷ��֥
* ���ʱ�䣺  2011��8��4��
*
* ��    ����
*     ��  �룺��
*      
*     ��  ����
*
*     ����ֵ��0����1�������
*
* �޸���ʷ��
*                   
********************************************************************/

int check_ky_bal()
{
  int ret=0;
  double account_amt=0.0;
  double ky_bal=0.0;
  struct dd_mst_c sDd_mst;
  struct dd_parm_c sDd_parm;

  strcpy(g_pub_tx.cur_no,"01");
  memset(&sDd_mst,0,sizeof(sDd_mst));
  memset(&sDd_parm,0x00,sizeof(struct dd_parm_c));
  
  ret = pub_base_CurGetddFile(g_pub_tx.crd_no,g_pub_tx.cur_no,&sDd_mst);
  account_amt = sDd_mst.bal; /* �������*/
  vtcp_log("%s,%d,sDd_mst_bal[%f]",__FILE__,__LINE__,sDd_mst.bal);
  ret= Dd_parm_Sel(g_pub_tx.reply,&sDd_parm,"prdt_no ='%s'",sDd_mst.prdt_no);
  if(ret)
  {
      sprintf(acErrMsg,"��ѯdd_parm����!!!");
      strcpy(g_pub_tx.reply,"W015");
      WRITEMSG
      return 1;
  } 
  ky_bal = sDd_mst.bal - sDd_mst.ctl_amt - sDd_mst.hold_amt;
    
  if (pub_base_CompDblVal(ky_bal,g_pub_tx.tx_amt1)<0)
  {
    return 1;
  }
   return 0;
}
