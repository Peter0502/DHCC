/*************************************************
* �� �� ��: sp9710.c
* ��������: �����ʺŻ��������Ϣ___��������ר��
* ��    ��: rob
* ������ڣ�2003��02��19��
* �޸ļ�¼��   
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#define EXTERN
#include "public.h"
#include "cif_basic_inf_c.h"
#include "cif_id_code_rel_c.h"
#include "prdt_ac_id_c.h"
#include "mdm_attr_c.h"

sp9710()  
{ 		
	int ret=0;
	long cif_no;
	char tmp_note_no[20],ac_no[20],note_no[20];
	char name[51];
	char tag[2];

	struct cif_basic_inf_c g_cif_basic_inf;
	struct cif_id_code_rel_c g_cif_id_code_rel;
	struct prdt_ac_id_c g_prdt_ac_id;
	struct mdm_attr_c g_mdm_attr;

	memset(&g_cif_basic_inf,0x00,sizeof(struct cif_basic_inf_c));
	memset(&g_cif_id_code_rel,0x00,sizeof(struct cif_id_code_rel_c));
	memset(&g_prdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));
	memset(&g_mdm_attr,0x00,sizeof(struct mdm_attr_c));

	/** ȡֵ����ֵ **/	
	get_zd_data("0670",tag);					/*** ��֤���� ***/
	get_zd_data("0300",ac_no);					/*** �ʺ� ***/
	pub_base_old_acno( ac_no );                 /*** �Ծ��ʺŵĴ��� ***/
	
	ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",ac_no);
	if( ret==100 )
	{
		sprintf(acErrMsg,"�����ڸ��ʺ�!!ac_no=[%ld]",ac_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"���Ҵ���");
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	if( g_mdm_ac_rel.note_sts[0]=='*' )
	{
		sprintf(acErrMsg,"�ý���������������!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"M012");
		goto ErrExit;
	}
	else if( g_mdm_ac_rel.note_sts[0]=='2' )
	{
		sprintf(acErrMsg,"�ý���������ʧ��֤����!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P171");
		goto ErrExit;
	}
	else if( g_mdm_ac_rel.note_sts[0]=='3' )
	{
		sprintf(acErrMsg,"�ý���������ʧ����!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"M022");
		goto ErrExit;
	}
	else if( g_mdm_ac_rel.note_sts[0]=='4' )
	{
		sprintf(acErrMsg,"�ý����������ỻ֤����!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"D215");
		goto ErrExit;
	}
	else if( g_mdm_ac_rel.note_sts[0]=='5' )
	{
		sprintf(acErrMsg,"�ý���������������!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"M012");
		goto ErrExit;
	}

	/*** �ҿͻ��� ***/
	if( pub_base_acgetname( g_mdm_ac_rel.ac_no, &cif_no, name ) )
	{
		sprintf(acErrMsg,"���ҿͻ��š�����ʧ��!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"D249");
		goto ErrExit;
	}
  set_zd_data( "0250",name );
  vtcp_log("[%s][%d] �ͻ�name==[%s]",__FILE__,__LINE__,name);
	strcpy(note_no,g_mdm_ac_rel.note_no);
	
	/*** ��ȡ�������� ***/
	ret = Mdm_attr_Sel(&g_pub_tx.reply,&g_mdm_attr," mdm_code='%s' ", \
						g_mdm_ac_rel.mdm_code);
	if( ret==100 )
	{
		 sprintf(acErrMsg,"�ý������Բ�����![%d][%s]",
		 		g_reply_int,g_pub_tx.mdm_code);
		 WRITEMSG
		 strcpy(g_pub_tx.reply,"P132");
		 goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"���Ҵ���");
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	 /*����ǿ��ࣺ��Ҫ���뿨�Ų��һ���ƾ֤��*/
	if( g_mdm_attr.no_ind[0]=='Y' )
	{
		ret = pub_card_CardToNote(ac_no,note_no);
	}
	else if( tag[0]!='1' )
	{;
	}
	else
	{
		set_zd_data("0310",ac_no);
	}

	/* ������� */
	set_zd_data("1142",g_mdm_ac_rel.draw_pwd_yn);
	set_zd_data("1145",g_mdm_ac_rel.draw_id_yn);
	set_zd_data("1148",g_mdm_ac_rel.id_type);
	/**����֤������-wangwenkui-20110113**/
	set_zd_data("0370",g_mdm_ac_rel.id_no);
	/**end**/
	set_zd_data("1168",note_no);
	set_zd_data("1161",g_mdm_ac_rel.mdm_code);
	
	/*set_zd_data( "0250",name );*/
	 vtcp_log("[%s][%d] �ͻ�name==[%s]",__FILE__,__LINE__,name);
	set_zd_data( "0300",ac_no );

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
 
