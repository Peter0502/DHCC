/*************************************************
* 文 件 名:    sp3222.c
* 功能描述：   七天乐账号信息查询
*              根据账号查询户名及可用余额
* 
* 作    者:    shaoliguang
* 完成日期：   2011年4月06日
*   配置的交易流程
 insert into tx_def values('3222','七天乐账号信息查询','10000000000010000000000010000000000000011110000000000000001001110000000000000000011000001000000000000000000000000000000000000100','0','4','0');
 insert into tx_flow_def values('3222','0','3222','#$');
 insert into tx_sub_def values('3222','七天乐账号信息查询','sp3222','0','0');
* 修改记录：  
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

  /** 取值、赋值 **/
  get_zd_data("0300", ac_no);  /* 帐号 */

  pub_base_old_acno(ac_no);  /* 对旧帐号的处理 */
  /* 初始化全局变量 */
  pub_base_sysinit();
  
  
  vtcp_log("交易机构是【%s】",g_pub_tx.tx_br_no);
  
  /* 根据帐号查询账户序号和账户ID */
  ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &g_mdm_ac_rel, "ac_no='%s'", ac_no);
  if (ret == 100) {
    sprintf(acErrMsg, "不存在该帐号!!ac_no=[%s]", ac_no);
    WRITEMSG
    strcpy(g_pub_tx.reply, "AT01");
    goto ErrExit;
  } else if (ret) {
    sprintf(acErrMsg, "执行Mdm_ac_rel_Sel错误!ret=[%d]", ret);
    WRITEMSG
    goto ErrExit;
  }
  vtcp_log("开户机构是【%s】",g_mdm_ac_rel.opn_br_no);
  if(memcmp(g_mdm_ac_rel.opn_br_no,g_pub_tx.tx_br_no,5))
    {
      sprintf(acErrMsg, "只能在开户行签约!!");
      WRITEMSG
      strcpy(g_pub_tx.reply, "D245"); 
      goto ErrExit;
    }
  set_zd_data("0620",g_mdm_ac_rel.id_no);                        /*证件号码*/
  pub_base_dic_show_str(idtype,"id_type",g_mdm_ac_rel.id_type);  /*证件类型*/
  set_zd_data("0630",idtype);                                    /*证件类型*/
  /*查询活期主表获取户名、余额等*/
  ret=Dd_mst_Sel(g_pub_tx.reply,&sDd_mst,"ac_sts='1' and ac_id=%ld",g_mdm_ac_rel.ac_id);
  if (ret == 100) {
    sprintf(acErrMsg, "不存在该ac_id!!ac_id=[%s]", g_mdm_ac_rel.ac_id);
    WRITEMSG
    strcpy(g_pub_tx.reply, "C115");
    goto ErrExit;
  } else if (ret) {
    sprintf(acErrMsg, "执行dd_mst错误!ret=[%d]", ret);
    WRITEMSG
    goto ErrExit;
  }
  set_zd_data("0250", sDd_mst.name);        /*客户名称*/
  set_zd_double("0430",sDd_mst.bal);        /*主账户可用金额*/


   ret=Cif_addr_inf_Sel(g_pub_tx.reply,&sCif_addr_inf,"cif_no=%d ",sDd_mst.cif_no);
    if (ret == 100) {
    sprintf(acErrMsg, "客户地址信息表中无此记录!cif_no=[%d]", sDd_mst.cif_no);
    WRITEMSG
      strcpy(g_pub_tx.reply, "C066");
    goto ErrExit;
  } else if (ret) {
    sprintf(acErrMsg, "执行错误!ret=[%d]", ret);
    WRITEMSG
    goto ErrExit;
  }
  set_zd_data("0820", sCif_addr_inf.addr);/*联系地址*/
  

   ret=Cif_email_inf_Sel(g_pub_tx.reply,&sCif_email_inf,"cif_no=%d ",sDd_mst.cif_no);
    if (ret == 100) {
    sprintf(acErrMsg, "客户电子地址信息表中无此记录!!cif_no=[%d]", sDd_mst.cif_no);
    WRITEMSG
      strcpy(g_pub_tx.reply, "C061");
    goto ErrExit;
  } else if (ret) {
    sprintf(acErrMsg, "执行错误!ret=[%d]", ret);
    WRITEMSG
    goto ErrExit;
  }
  set_zd_data("0830", sCif_email_inf.email);/*移动电话*/

  /*查询划款协议登记薄取账户的可用金额和备用金额*/
  ret=Mo_prdt_prot_Sel(g_pub_tx.reply,&sMo_prdt_prot,"prot_type='TK7' and sts in('1','2') and  main_ac_id=%ld ",g_mdm_ac_rel.ac_id);
  if (ret == 100) {
    sprintf(acErrMsg, "记录不存在，账号未签约!!main_ac_id=[%d]", g_mdm_ac_rel.ac_id);
    WRITEMSG
    strcpy(g_pub_tx.reply, "B363");
    goto ErrExit;
  } else if (ret) {
    sprintf(acErrMsg, "执行错误!ret=[%d]", ret);
    WRITEMSG
    goto ErrExit;
  }

  vtcp_log("可用金额=【%.2f】",sMo_prdt_prot.leave_amt);
  vtcp_log("登记薄化转金额=【%.2f】",sMo_prdt_prot.turn_amt);
  
  set_zd_double("0410",sMo_prdt_prot.leave_amt);         /*留存金额*/
  
  ret=Mo_prot_ac_rel_Sel(g_pub_tx.reply,&sMo_prot_ac_rel,"prot_type='TK7' and main_ac_id=%ld and sts in('1','2') ",g_mdm_ac_rel.ac_id);
  if(ret)
  {
     sprintf(acErrMsg, "查询划款协议账户对照表出错[%d]", ret);
     WRITEMSG
     strcpy(g_pub_tx.reply, "X013");
     goto ErrExit;
  }
  vtcp_log("虚拟账户app_ac_id=【%d】",sMo_prot_ac_rel.app_ac_id);
  /*查询虚拟账户余额*/
  ret=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,"ac_id=%ld and ac_seqn=%d",sMo_prot_ac_rel.app_ac_id,sMo_prot_ac_rel.app_ac_seqn);
  if(ret)
  {
     sprintf(acErrMsg, "查询活期主表出错[%d]", ret);
     WRITEMSG
     strcpy(g_pub_tx.reply, "C115");
     goto ErrExit;
  }
  vtcp_log("虚拟账户余额=【%.2lf】",g_dd_mst.bal);
  set_zd_double("0420",g_dd_mst.bal);                    /*化转金额*/
  bal=sDd_mst.bal+g_dd_mst.bal;
  set_zd_double("0400", bal);                            /*总余额*/

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
