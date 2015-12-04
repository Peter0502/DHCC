/*************************************************
* �� �� ��:  sp5624.c
* �����������жҵ���弶�������
*
* ��    ��: cfq 
* ������ڣ�20150518
* �������ã�insert into TX_DEF (TX_CODE, TX_NAME, BIT_MAP, LOG_IND, TX_TYPE, WRK_STS) values ('5625', '�жһ�Ʊ��Ϣ����', '11000000000000000000000010010100000000111001110000000000000000000000111100000000001000000000000000000000000000000000000000000000', '0', '3', '0');
*          insert into TX_FLOW_DEF (TX_CODE, FLOW_CODE, SUB_TX_CODE, SUB_TX_RSLT) values ('5625', 0, '5624', '#$');
*          insert into TX_SUB_DEF (SUB_TX_CODE, SUB_TX_NAME, TX_FUNC, ACCT_IND, CHNL_IND) values ('5624', '�жҵ���弶״̬����', 'sp5624', '0', '0');
* �޸ļ�¼��
* ��   ��:
* �� �� ��:
* �޸�����:
*************************************************/
#include "stdio.h" 
#include "public.h"
#include "ln_mst_c.h"
#include "mdm_ac_rel_c.h"  
#include "mo_bank_acc_po_c.h"

sp5624()
{
	  int       ret=0;
	  char      pact_no[24];
	  double    bal=0.00;
	  double    lo_intst=0.00,in_lo_intst=0.00,out_lo_intst=0.00;
	  double    aign_amt=0.00;
    
	  struct	mo_bank_acc_po_c 	g_mo_bank_acc_po;
    struct  mdm_ac_rel_c      g_mdm_ac_rel;  
    struct  ln_mst_c          g_ln_mst;   
    memset(pact_no,0x00,sizeof(pact_no));
    memset(&g_mo_bank_acc_po,0x00, sizeof(struct mo_bank_acc_po_c));  
    memset(&g_mdm_ac_rel,0x00, sizeof(struct mdm_ac_rel_c));
    memset(&g_ln_mst,0x00, sizeof(struct ln_mst_c));
    get_zd_data("0620",pact_no); 
    pub_base_sysinit();
    ret = Mo_bank_acc_po_Sel(g_pub_tx.reply, &g_mo_bank_acc_po, "pact_no='%s'", pact_no);
    if (ret == 100) 
  {
    sprintf(acErrMsg, "�����ڸóж�Э����!pact_no=[%s]", pact_no);
    WRITEMSG
    strcpy(g_pub_tx.reply, "B199");
    goto ErrExit;
  } 
  else if (ret) 
  {
    sprintf(acErrMsg, "ִ��Mo_bank_acc_po_Sel����!ret=[%d]", ret);
    WRITEMSG
    strcpy(g_pub_tx.reply, "B060");
    goto ErrExit;
  }
    set_zd_data("0860",g_mo_bank_acc_po.tx_br_no);
    set_zd_data("0690",g_mo_bank_acc_po.acc_type);
    set_zd_double("0390",g_mo_bank_acc_po.sign_amt);
    set_zd_long("0440",g_mo_bank_acc_po.req_date);
    set_zd_long("0450",g_mo_bank_acc_po.matr_date);
    set_zd_data("0300",g_mo_bank_acc_po.over_ac_no);
    
    ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &g_mdm_ac_rel, "ac_no='%s'", g_mo_bank_acc_po.over_ac_no);
     if (ret == 100) 
  {
    sprintf(acErrMsg, "�����ڸô����˺�!ac_no=[%s]", g_mo_bank_acc_po.over_ac_no);
    WRITEMSG
    strcpy(g_pub_tx.reply, "B117");
    goto ErrExit;
  } 
  else if (ret) 
  {
    sprintf(acErrMsg, "ִ��Mdm_ac_rel_Sel����!ret=[%d]", ret);
    WRITEMSG
    strcpy(g_pub_tx.reply, "L015");
    goto ErrExit;
  }
    ret = Ln_mst_Sel(g_pub_tx.reply, &g_ln_mst, "ac_id='%d'", g_mdm_ac_rel.ac_id);
      if (ret == 100) 
  {
    sprintf(acErrMsg, "�����ڸ��˻�ID!ac_id=[%s]", g_mdm_ac_rel.ac_id);
    WRITEMSG
    strcpy(g_pub_tx.reply, "W018");
    goto ErrExit;
  } 
  else if (ret) 
  {
    sprintf(acErrMsg, "ִ��Ln_mst_Sel����!ret=[%d]", ret);
    WRITEMSG
    strcpy(g_pub_tx.reply, "L021");
    goto ErrExit;
  }
    set_zd_data("0700",g_ln_mst.ac_sts);
    set_zd_long("0280",g_ln_mst.cif_no);
    set_zd_data("0250",g_ln_mst.name);
    set_zd_long("0460",g_ln_mst.opn_date);
    set_zd_data("0710",g_ln_mst.intst_type);
    set_zd_double("0400",g_ln_mst.bal);
    lo_intst=g_ln_mst.in_lo_intst+g_ln_mst.out_lo_intst;
    set_zd_double("0410",lo_intst);
    set_zd_data("0720",g_ln_mst.five_sts);
    vtcp_log("�ͻ���%d",g_ln_mst.cif_no);
    
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
