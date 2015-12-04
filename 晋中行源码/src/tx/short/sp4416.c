/*************************************************
* �� �� ��: sp4416.c
* ����������   
*              ί�д��Ϣ���ݴ���˺Ż�ȡ��Ϣ
*
* ��    ��:    zgf
* ������ڣ�   2013��05��28��
* �޸ļ�¼��   
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
*     2. 
*************************************************/

#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#define EXTERN
#include "public.h"
#include "cif_basic_inf_c.h"
#include "prdt_ac_id_c.h"
#include "cif_id_code_rel_c.h"
#include "mdm_attr_c.h"
#include "mo_trust_prot_c.h"


sp4416()  
{ 
	int ret;
	char trust_no[20]={0};
	struct mdm_attr_c g_mdm_attr;
	struct mo_trust_prot_c mo_trust_prot ;
	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&mo_trust_prot,0x00,sizeof(struct mo_trust_prot_c));
	memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
	memset(&g_mdm_attr,0x00,sizeof(struct mdm_attr_c));

	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();

	get_zd_data("1021",g_pub_tx.ac_no);
	pub_base_old_acno( g_pub_tx.ac_no );
	
	ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",g_pub_tx.ac_no);
  if( ret )
  {
  	sprintf(acErrMsg,"�����ʺŶ��ձ����ڸü�¼!![%s]",g_pub_tx.ac_no);
    WRITEMSG
    strcpy(g_pub_tx.reply,"M003");
    goto ErrExit;
  }
  
  ret = Mdm_attr_Sel( g_pub_tx.reply,&g_mdm_attr,"mdm_code='%s'", \
					    g_mdm_ac_rel.mdm_code );
	if( ret==100 )
	{
		sprintf(acErrMsg,"���ʲ�����! ");
 		WRITEMSG
  		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"���ҳ���! ");
 		WRITEMSG
  		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}
  set_zd_data("0900",g_mdm_attr.note_type);/**ƾ֤���� **/
  set_zd_data("0590",g_mdm_ac_rel.note_no);/**ƾ֤���� **/  
    
  ret = Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst, \
							"ac_id=%ld and ac_seqn=%d", \
							g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_seqn);
	if( ret )
	{
		sprintf(acErrMsg,"read dd_mst error" );
		WRITEMSG
		goto ErrExit;
	}
	set_zd_data("0250",g_dd_mst.name);/**���� **/		
	

	get_zd_data("0370",trust_no);

  /* ȡί��Э����Ϣ */

  vtcp_log("trust_no is [%s]",trust_no);
  ret = pub_ln_trust_info(trust_no, &mo_trust_prot );
    if( ret ) goto ErrExit;
	
	vtcp_log("ac_no is [%d]",g_mdm_ac_rel.ac_id);
	vtcp_log("afund_ac_id is [%d]",mo_trust_prot.afund_ac_id);
	
	if( mo_trust_prot.afund_ac_id != g_mdm_ac_rel.ac_id )
	{		
		strcpy(g_pub_tx.reply,"L094");
		sprintf(acErrMsg,"read mo_trust_prot error" );
		WRITEMSG
		goto ErrExit;
	}	
	sprintf(acErrMsg,"read mo_trust_prot pass..." );
	WRITEMSG

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"����˺Ż�ȡ��Ϣ PASS![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"����˺Ż�ȡ��Ϣ ERROR![%s]",g_pub_tx.reply);
	WRITEMSG
	if(memcmp(g_pub_tx.reply,"0000",4)==0)
		strcpy(g_pub_tx.reply,"D103");
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
         
}
