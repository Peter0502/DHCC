/*************************************************
* �� �� ��: sp9991.c
* �����������ʺŻ��Ի���_[�ڲ��ʶ��ʵ�]
* ��    ��: rob
* ������ڣ�2003��09��07��
* �޸ļ�¼��   
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#define EXTERN
#include "public.h"

sp9991()  
{ 		
	int ret=0;
	char ac_no[20];
	long cif_no;
	char name[51];
	
	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&g_in_mst,0x00,sizeof(struct in_mst_c));

	/** ȡֵ����ֵ **/	
	get_zd_data("0310",ac_no);						/** �ڲ��ʺ� **/
	pub_base_old_acno( ac_no );  					/** �¾�ת�� **/
	set_zd_data("0310",ac_no);						/** ������ʺ� **/
	
	/* ���ø����ʺŷ����˻���ź��� */
	ret = Mdm_ac_rel_Sel( g_pub_tx.reply,&g_mdm_ac_rel, "ac_no='%s'",ac_no );
	if( ret==100 )
	{
		sprintf(acErrMsg,"�����ʺŶ��ձ����ڸü�¼!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"����ʧ��! [%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	/*** �ҿͻ��� ***/
	ret = In_mst_Sel( g_pub_tx.reply,&g_in_mst,"ac_id=%ld",g_mdm_ac_rel.ac_id );
	if( ret==100 )
	{
		sprintf(acErrMsg,"���ҿͻ��š�����ʧ��!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"����ʧ��! [%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	set_zd_data("0250",g_in_mst.name);						/*��    ��*/

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
