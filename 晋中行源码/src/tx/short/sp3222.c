/*************************************************
* �� �� ��:    sp3222.c
* ����������   �������˺���Ϣ��ѯ
*              �����˺Ų�ѯ�������������
* 
* ��    ��:    shaoliguang
* ������ڣ�   2011��4��06��
*   ���õĽ�������
 insert into tx_def values('3222','�������˺���Ϣ��ѯ','10000000000010000000000010000000000000011110000000000000001001110000000000000000011000001000000000000000000000000000000000000100','0','4','0');
 insert into tx_flow_def values('3222','0','3222','#$');
 insert into tx_sub_def values('3222','�������˺���Ϣ��ѯ','sp3222','0','0');
* �޸ļ�¼��  
* 
*************************************************/
#include <string.h>
#define EXTERN
#include "public.h"
#include "com_sys_parm_c.h"
#include "mdm_ac_rel_c.h"
#include "dd_mst_c.h"
#include "td_mst_c.h"
#include "mo_prdt_prot_c.h"
#include "cif_email_inf_c.h"
#include "cif_addr_inf_c.h"
#include "mo_prot_ac_rel_c.h"

static struct mdm_ac_rel_c     g_mdm_ac_rel;
static struct dd_mst_c         sDd_mst,g_dd_mst;
static struct mo_prdt_prot_c   sMo_prdt_prot;
static struct cif_email_inf_c  sCif_email_inf;
static struct cif_addr_inf_c   sCif_addr_inf;
static struct mo_prot_ac_rel_c sMo_prot_ac_rel;
int sp3222()
{
  int             ret = 0, ac_seqn;
  char            ac_no[20], name[51];
  char            idtype[40];
  double          ac_bal=0.0;
  double          bal=0.00;
  memset(ac_no, 0x00, sizeof(ac_no));  
  memset(name, 0x00, sizeof(name));
  memset( idtype ,0x00,sizeof(idtype));
  memset(&g_mdm_ac_rel,   0x00, sizeof(struct mdm_ac_rel_c));
  memset(&sMo_prot_ac_rel,0x00, sizeof(struct mo_prot_ac_rel_c));
  memset(&sMo_prdt_prot,  0x00, sizeof(struct mo_prdt_prot_c));
  memset(&sCif_email_inf, 0x00, sizeof(struct cif_email_inf_c));
  memset(&sCif_addr_inf,  0x00, sizeof(struct cif_addr_inf_c));
  memset(&sDd_mst,        0x00, sizeof(struct dd_mst_c));
  memset(&g_dd_mst,       0x00, sizeof(struct dd_mst_c));

  /** ȡֵ����ֵ **/
  get_zd_data("0300", ac_no);  /* �ʺ� */

  pub_base_old_acno(ac_no);  /* �Ծ��ʺŵĴ��� */
  /* ��ʼ��ȫ�ֱ��� */
  pub_base_sysinit();
  
  
  vtcp_log("���׻����ǡ�%s��",g_pub_tx.tx_br_no);
  
  /* �����ʺŲ�ѯ�˻���ź��˻�ID */
  ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &g_mdm_ac_rel, "ac_no='%s'", ac_no);
  if (ret == 100) {
    sprintf(acErrMsg, "�����ڸ��ʺ�!!ac_no=[%s]", ac_no);
    WRITEMSG
    strcpy(g_pub_tx.reply, "AT01");
    goto ErrExit;
  } else if (ret) {
    sprintf(acErrMsg, "ִ��Mdm_ac_rel_Sel����!ret=[%d]", ret);
    WRITEMSG
    goto ErrExit;
  }
  vtcp_log("���������ǡ�%s��",g_mdm_ac_rel.opn_br_no);
  if(memcmp(g_mdm_ac_rel.opn_br_no,g_pub_tx.tx_br_no,5))
    {
      sprintf(acErrMsg, "ֻ���ڿ�����ǩԼ!!");
      WRITEMSG
      strcpy(g_pub_tx.reply, "D245"); 
      goto ErrExit;
    }
  set_zd_data("0620",g_mdm_ac_rel.id_no);                        /*֤������*/
  pub_base_dic_show_str(idtype,"id_type",g_mdm_ac_rel.id_type);  /*֤������*/
  set_zd_data("0630",idtype);                                    /*֤������*/
  /*��ѯ���������ȡ����������*/
  ret=Dd_mst_Sel(g_pub_tx.reply,&sDd_mst,"ac_sts='1' and ac_id=%ld",g_mdm_ac_rel.ac_id);
  if (ret == 100) {
    sprintf(acErrMsg, "�����ڸ�ac_id!!ac_id=[%s]", g_mdm_ac_rel.ac_id);
    WRITEMSG
    strcpy(g_pub_tx.reply, "C115");
    goto ErrExit;
  } else if (ret) {
    sprintf(acErrMsg, "ִ��dd_mst����!ret=[%d]", ret);
    WRITEMSG
    goto ErrExit;
  }
  set_zd_data("0250", sDd_mst.name);        /*�ͻ�����*/
  set_zd_double("0430",sDd_mst.bal);        /*���˻����ý��*/


   ret=Cif_addr_inf_Sel(g_pub_tx.reply,&sCif_addr_inf,"cif_no=%d ",sDd_mst.cif_no);
    if (ret == 100) {
    sprintf(acErrMsg, "�ͻ���ַ��Ϣ�����޴˼�¼!cif_no=[%d]", sDd_mst.cif_no);
    WRITEMSG
      strcpy(g_pub_tx.reply, "C066");
    goto ErrExit;
  } else if (ret) {
    sprintf(acErrMsg, "ִ�д���!ret=[%d]", ret);
    WRITEMSG
    goto ErrExit;
  }
  set_zd_data("0820", sCif_addr_inf.addr);/*��ϵ��ַ*/
  

   ret=Cif_email_inf_Sel(g_pub_tx.reply,&sCif_email_inf,"cif_no=%d ",sDd_mst.cif_no);
    if (ret == 100) {
    sprintf(acErrMsg, "�ͻ����ӵ�ַ��Ϣ�����޴˼�¼!!cif_no=[%d]", sDd_mst.cif_no);
    WRITEMSG
      strcpy(g_pub_tx.reply, "C061");
    goto ErrExit;
  } else if (ret) {
    sprintf(acErrMsg, "ִ�д���!ret=[%d]", ret);
    WRITEMSG
    goto ErrExit;
  }
  set_zd_data("0830", sCif_email_inf.email);/*�ƶ��绰*/

  /*��ѯ����Э��ǼǱ�ȡ�˻��Ŀ��ý��ͱ��ý��*/
  ret=Mo_prdt_prot_Sel(g_pub_tx.reply,&sMo_prdt_prot,"prot_type='TK7' and sts in('1','2') and  main_ac_id=%ld ",g_mdm_ac_rel.ac_id);
  if (ret == 100) {
    sprintf(acErrMsg, "��¼�����ڣ��˺�δǩԼ!!main_ac_id=[%d]", g_mdm_ac_rel.ac_id);
    WRITEMSG
    strcpy(g_pub_tx.reply, "B363");
    goto ErrExit;
  } else if (ret) {
    sprintf(acErrMsg, "ִ�д���!ret=[%d]", ret);
    WRITEMSG
    goto ErrExit;
  }

  vtcp_log("���ý��=��%.2f��",sMo_prdt_prot.leave_amt);
  vtcp_log("�ǼǱ���ת���=��%.2f��",sMo_prdt_prot.turn_amt);
  
  set_zd_double("0410",sMo_prdt_prot.leave_amt);         /*������*/
  
  ret=Mo_prot_ac_rel_Sel(g_pub_tx.reply,&sMo_prot_ac_rel,"prot_type='TK7' and main_ac_id=%ld and sts in('1','2') ",g_mdm_ac_rel.ac_id);
  if(ret)
  {
     sprintf(acErrMsg, "��ѯ����Э���˻����ձ����[%d]", ret);
     WRITEMSG
     strcpy(g_pub_tx.reply, "X013");
     goto ErrExit;
  }
  vtcp_log("�����˻�app_ac_id=��%d��",sMo_prot_ac_rel.app_ac_id);
  /*��ѯ�����˻����*/
  ret=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,"ac_id=%ld and ac_seqn=%d",sMo_prot_ac_rel.app_ac_id,sMo_prot_ac_rel.app_ac_seqn);
  if(ret)
  {
     sprintf(acErrMsg, "��ѯ�����������[%d]", ret);
     WRITEMSG
     strcpy(g_pub_tx.reply, "C115");
     goto ErrExit;
  }
  vtcp_log("�����˻����=��%.2lf��",g_dd_mst.bal);
  set_zd_double("0420",g_dd_mst.bal);                    /*��ת���*/
  bal=sDd_mst.bal+g_dd_mst.bal;
  set_zd_double("0400", bal);                            /*�����*/

OkExit:
  strcpy(g_pub_tx.reply, "0000");
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
