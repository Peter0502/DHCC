/***************************************************************************
* �� �� ����spMK06.c
* �����������ͻ���Ϣ���˻���Ϣ��ѯ
* ��    �ߣ�wuhaifan
* ������ڣ�2015-09-24
insert into tx_def (TX_CODE, TX_NAME, BIT_MAP, LOG_IND, TX_TYPE, WRK_STS)
values ('8131', '�ͻ���Ϣ�˻���Ϣ��ѯ', '10100000000000000001000110010000000001000011100001000000000000000110100000001000010000000000000000000000000000000000000000000000', '1', '5', '0');
insert into tx_flow_def (TX_CODE, FLOW_CODE, SUB_TX_CODE, SUB_TX_RSLT)
values ('8131', 0, 'MK06', '#$');
insert into tx_sub_def (SUB_TX_CODE, SUB_TX_NAME, TX_FUNC, ACCT_IND, CHNL_IND)
values ('MK06', '�ͻ���Ϣ�˻���Ϣ��ѯ', 'spMK06', '0', '0');
***************************************************************************/
#include <stdio.h>
#include <string.h>
#include "public.h"
#include "mdm_ac_rel_c.h"
#include "prdt_ac_id_c.h"
#include "dd_mst_c.h"
#include "td_mst_c.h"
#include "com_branch_c.h"
#include "mob_acct_type_c.h"

int spMK06()
{
	const	char * trs_desc="�����˻���֤";
	int	  ret=0;
	char	cAc_no[25]={0};
	char	cDraw_pwd[7]={0};	
	char	cType[2]={0};
	char  Id_type[2];
	char  Id_no[21];
	char  nochkpin[10];
	char  Name[60+1];
	struct mdm_ac_rel_c	g_mdm_ac_rel;
	struct prdt_ac_id_c sPrdt_ac_id;
	struct dd_mst_c	g_dd_mst;
	struct td_mst_c	g_td_mst;
	struct com_branch_c	g_com_branch;
	struct mob_acct_type_c sMob_acct_type;	/*�������пͻ��˺������Ʒ�ֶ�Ӧ��*/
	
	memset(Name,0x00, sizeof(Name));
	memset(cAc_no,0x00, sizeof(cAc_no));
	memset(cDraw_pwd,0x00, sizeof(cDraw_pwd));
	memset(cType,0x00, sizeof(cType));
	memset(Id_type,0x00, sizeof(Id_type));
	memset(Id_no,0x00, sizeof(Id_no));
	memset(nochkpin,0x00, sizeof(nochkpin));
	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&sPrdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));
	memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
	memset(&g_td_mst,0x00,sizeof(struct td_mst_c));
	memset(&g_com_branch,0x00,sizeof(struct com_branch_c));
	memset(&sMob_acct_type,0x00,sizeof(sMob_acct_type));
	
	vtcp_log("[%s],[%d],[%s]����ʼ...",__FILE__,__LINE__,trs_desc);
	pub_base_sysinit();              /*��ʼ����������*/	
	get_zd_data("0300",cAc_no);      /*�˺�*/
	
	pub_base_strpack(cAc_no);
	vtcp_log("[%s][%d],�˺�Ϊ[%s]",__FILE__,__LINE__,cAc_no);
	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",cAc_no);
	if( 100==ret ) 
	{
		sprintf(acErrMsg,"���˻�������!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"D104");
		set_zd_data("0130","�˻�������");
		goto ErrExit;
	}
	else if( ret ) {
		sprintf(acErrMsg,"��ѯ���ʺ��ʻ���ϵ�����!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"AT06");
		goto ErrExit;
	}
	
	set_zd_data("0670", g_mdm_ac_rel.note_sts);	/*����״̬*/
	set_zd_data("0770", g_mdm_ac_rel.opn_br_no);	/*������*/
	set_zd_data("0200", g_mdm_ac_rel.id_type);	/*֤������*/
	set_zd_data("0380", g_mdm_ac_rel.id_no);	/*֤����*/
	
	/**���ڶ�������**/
	ret = Prdt_ac_id_Sel(g_pub_tx.reply, &sPrdt_ac_id,"ac_id=%ld", g_mdm_ac_rel.ac_id);
	if( 100==ret ) 
	{
	   sprintf(acErrMsg,"���˻��Ų�����!");
	   WRITEMSG
	   strcpy(g_pub_tx.reply,"D104");
	   set_zd_data("0130","�˻�������");
	   goto ErrExit;
	}
	else if( ret ) 
	{
	   sprintf(acErrMsg,"��ѯ��Ʒ�˺Ŷ��ձ����!");
	   WRITEMSG
	   strcpy(g_pub_tx.reply,"AT06");
	   goto ErrExit;
	 }
	if (sPrdt_ac_id.ac_id_type[0] == '1')                   /**��ѯ���ڲ�Ʒ!***/
	{
	     /**��ȡ�˻��ͻ������˻�״̬���˻�����**/
	     ret=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,"ac_id=%ld ",g_mdm_ac_rel.ac_id);
	     if( 100==ret ) 
	     {
	     	sprintf(acErrMsg,"���˻��Ų�����!");
	     	WRITEMSG
	     	strcpy(g_pub_tx.reply,"D104");
	     	set_zd_data("0130","�˻�������");
	     	goto ErrExit;
	     }
	     else if( ret ) 
	     {
	     	sprintf(acErrMsg,"��ѯ���ڴ�����ļ������!");
	     	WRITEMSG
	     	strcpy(g_pub_tx.reply,"AT06");
	     	goto ErrExit;
	     }
	     
		sprintf(acErrMsg,"[%s],[%d]Name=[%s],�˻��ͻ���=[%s],�˻�����=[%s],�˻�ƾ֤״̬=[%s],�˻�����״̬=[%s]",__FILE__,__LINE__,Name,g_dd_mst.name,g_dd_mst.prdt_no,g_dd_mst.ac_sts,g_dd_mst.hold_sts);
		WRITEMSG
		set_zd_data("0250", g_dd_mst.name);		/*�ͻ�����*/
		set_zd_data("0660", g_dd_mst.ac_sts);	/*�˻�״̬*/
		set_zd_data("0690", g_dd_mst.hold_sts);	/*����״̬*/
		set_zd_long("0530", g_dd_mst.opn_date);	/*��������*/
    }
	else if (sPrdt_ac_id.ac_id_type[0] == '2')               /**��ѯ���ڲ�Ʒ!***/
	{
	     /**��ȡ�˻��ͻ������˻�״̬���˻�����**/
	     ret=Td_mst_Sel(g_pub_tx.reply,&g_td_mst,"ac_id=%ld ",g_mdm_ac_rel.ac_id);
	     if( 100==ret ) 
	     {
	     	sprintf(acErrMsg,"���˻��Ų�����!");
	     	WRITEMSG
	     	strcpy(g_pub_tx.reply,"D104");
	     	set_zd_data("0130","�˻�������");
	     	goto ErrExit;
	     }
	     else if( ret ) 
	     {
	     	sprintf(acErrMsg,"��ѯ���ڴ�����ļ������!");
	     	WRITEMSG
	     	strcpy(g_pub_tx.reply,"AT06");
	     	goto ErrExit;
	     }
	    
		sprintf(acErrMsg,"[%s],[%d]Name=[%s],�˻��ͻ���=[%s],�˻�����=[%s],�˻�ƾ֤״̬=[%s],�˻�����״̬=[%s]",__FILE__,__LINE__,Name,g_td_mst.name,g_td_mst.prdt_no,g_td_mst.ac_sts,g_td_mst.hold_sts);
		WRITEMSG
		set_zd_data("0250", g_td_mst.name);		/*�ͻ�����*/
		set_zd_data("0660", g_td_mst.ac_sts);	/*�˻�״̬*/
		set_zd_data("0690", g_td_mst.hold_sts);	/*����״̬*/
		set_zd_long("0530", g_td_mst.opn_date);	/*��������*/
    }
    set_zd_double("0280",sPrdt_ac_id.cif_no);/**�ͻ���**/
    set_zd_data("0240", sPrdt_ac_id.ac_id_type);	/*�˻�����*/
    
	/**��ȡ�����������к�**/
	ret=Com_branch_Sel(g_pub_tx.reply,&g_com_branch,"br_no='%s'",g_mdm_ac_rel.opn_br_no);
	if( 100==ret ) 
	{
		sprintf(acErrMsg,"�û����Ų�����!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"D104");
		goto ErrExit;
	}
	else if( ret ) 
	{
		sprintf(acErrMsg,"��ѯ��������������!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"AT06");
		goto ErrExit;
	}
	sprintf(acErrMsg,"[%s],[%d]�����к�=[%s],��������=[%s]���˺�����=[%s]",__FILE__,__LINE__,g_com_branch.br_no,g_com_branch.br_name,sPrdt_ac_id.ac_id_type);
	WRITEMSG
	set_zd_data("0820", g_com_branch.br_name);	/*��������*/
	
	/*��ѯ����ǩԼ��*/
	ret = Mob_acct_type_Sel(g_pub_tx.reply,&sMob_acct_type,"ac_no='%s' and mob_sts='1' ",cAc_no);	
	if(ret == 100)
	{
		vtcp_log("[%s],[%d] �˿ͻ�δ��������ǩԼ���ѽ�Լ!",__FILE__,__LINE__);
		set_zd_data("0580","");
		/*goto ErrExit;*/
	}
	else if( ret )
	{
		vtcp_log("[%s][%d]��ѯ��Mob_acct_type_Sel����!",__FILE__,__LINE__);
	  strcpy(g_pub_tx.reply,"D104");
	  set_zd_data("0130","��ѯ����ǩԼ�����");
	  goto ErrExit; 
	}
	else
	{
		vtcp_log("[%s],[%d] ���������пͻ��˺������Ʒ�ֶ�Ӧ��!",__FILE__,__LINE__);  
		vtcp_log("[%s],[%d] sMob_acct_type.mobile=[%s]",__FILE__,__LINE__,sMob_acct_type.mobile);     
  	set_zd_data("0580",sMob_acct_type.mobile);
	}	
	
OkExit:
	vtcp_log("[%s],[%d],[%s]�ɹ�!",__FILE__,__LINE__,trs_desc);
	strcpy(g_pub_tx.reply, "0000");
	set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 0;

ErrExit:
	vtcp_log("[%s],[%d],[%s]ʧ��!",__FILE__,__LINE__,trs_desc);
	set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 1;
}
