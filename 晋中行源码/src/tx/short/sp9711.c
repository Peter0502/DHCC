/*************************************************
* �� �� ��:    sp9711.c
* ����������   
*              �����ʺŻ��������Ϣ
*
* ��    ��:    rob
* ������ڣ�   2003��02��19��
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

sp9711()  
{ 		
	int ret=0;
	char tmp_note_no[20],ac_no[20],note_no[20],crd_no[20];

	struct cif_basic_inf_c g_cif_basic_inf;
	struct cif_id_code_rel_c g_cif_id_code_rel;
	struct prdt_ac_id_c g_prdt_ac_id;
	struct mdm_attr_c g_mdm_attr;

	memset(&g_cif_basic_inf,0x00,sizeof(struct cif_basic_inf_c));
	memset(&g_cif_id_code_rel,0x00,sizeof(struct cif_id_code_rel_c));
	memset(&g_prdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));
	memset(&g_mdm_attr,0x00,sizeof(struct mdm_attr_c));

	/** ȡֵ����ֵ **/	
	get_zd_data("0300",ac_no);							/*ԭ�ʺ�*/
	get_zd_data("0310",crd_no);							/*���ʺ�*/
	
		sprintf(acErrMsg,"!!ac_no=[%s]",ac_no);
		WRITEMSG

		sprintf(acErrMsg,"!!crd_no=[%ld]",crd_no);
		WRITEMSG

	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",ac_no);
	if( ret==100 )
	{
		sprintf(acErrMsg,"�����ڸ��ʺ�!!ac_no=[%s]",ac_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf( acErrMsg, "��ѯ�����ʺű��쳣!!ret=[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	/*** ��ȡ�������� ***/
	ret=Mdm_attr_Sel(&g_pub_tx.reply,&g_mdm_attr,
	 				 "mdm_code='%s'",g_mdm_ac_rel.mdm_code);
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
	 	 sprintf(acErrMsg,"ȡ���������쳣![%d]",g_reply_int);
		 WRITEMSG
		 strcpy(g_pub_tx.reply,"P133");
	 	 goto ErrExit;
	}

	 /*����ǿ��ࣺ��Ҫ���뿨�Ų��һ���ƾ֤��*/
	if( g_mdm_attr.no_ind[0]=='Y' )
	{
		ret = pub_card_CardToNote(crd_no,note_no);
	}
	else
	{
		strcpy(note_no,"");
	}

	/* ������� */
	set_zd_data("0590",note_no);

OkExit:
	strcpy(g_pub_tx.reply,"0000");
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
 
