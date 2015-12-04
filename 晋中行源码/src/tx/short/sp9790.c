/*************************************************
* �� �� ��:    sp9790.c
* ����������   ��֤���׿�ݲ�ѯ
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
#include "prdt_ac_id_c.h"

sp9790()  
{ 		
	int ret=0,ac_seqn;
	char title[40],cur_no[3];
	char ac_no[21],name[50];
	char ind[2];
	double bal;

	struct prdt_ac_id_c g_prdt_ac_id;
	struct cif_basic_inf_c g_cif_basic_inf;

	memset(&g_prdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));
	memset(&g_cif_basic_inf,0x00,sizeof(struct cif_basic_inf_c));

	/** ȡֵ����ֵ **/	
	get_zd_data("1021",ac_no);							/*�����ʺ�*/
	get_zd_data("0670",ind);							/*��������*/
	pub_base_old_acno( ac_no );                 /**�Ծ��ʺŵĴ���**/
	
	/* ���ø����ʺŷ����˻���ź��� */
	ret=pub_base_actoseqn(ac_no,&ac_seqn);          	/*�˻����*/
	if(ret)
	{
	 	sprintf( acErrMsg, "���ú���pub_base_actoseqn����!!ret=[%d]", ret );
				 WRITEMSG
	 	goto ErrExit;
	}

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
	    sprintf(acErrMsg,"���ҳ���");
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
		sprintf(acErrMsg,"�ý���������ʧ����!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P170");
		goto ErrExit;
	}
	else if( g_mdm_ac_rel.note_sts[0]=='3' )
	{
		sprintf(acErrMsg,"�ý���������ʧ��֤����!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P171");
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

	ret = Prdt_ac_id_Sel(g_pub_tx.reply, &g_prdt_ac_id, "ac_id=%ld", \
						g_mdm_ac_rel.ac_id);
   	if( ret==100 )
	{
		sprintf(acErrMsg,"�����ڸ��˻�!!ac_id=[%ld],ac_seqn=[%d]",
		 		g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_seqn);
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}
	else if( ret )
	{
	    sprintf(acErrMsg,"���ҳ���");
	    WRITEMSG
	 	strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	ret=Cif_basic_inf_Sel(g_pub_tx.reply,&g_cif_basic_inf,"cif_no=%d", \
							g_prdt_ac_id.cif_no);
   	if( ret==100 )
	{
		sprintf(acErrMsg,"�����ڸÿͻ�!!ac_id=[%ld],ac_seqn=[%d]",
		 		g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_seqn);
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}
	else if( ret )
	{
	    sprintf(acErrMsg,"���ҳ���");
	    WRITEMSG
	 	strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	/* ������� */
	if(ind[0]=='2')
	{
		set_zd_data("1142",g_mdm_ac_rel.draw_pwd_yn);
		set_zd_data("1145",g_mdm_ac_rel.draw_id_yn);
		set_zd_data("1148",g_mdm_ac_rel.id_type);
		set_zd_data("0250",g_cif_basic_inf.name);
	}
	else 
		set_zd_data("0250",g_cif_basic_inf.name);

	set_zd_data("1021", ac_no);                /**�Ծ��ʺŵĴ���**/

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
 
