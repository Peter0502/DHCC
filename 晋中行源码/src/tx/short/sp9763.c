/*************************************************
* �� �� ��: sp9763.c
* ��������������˺�and������Ϣ_����д��
*
* ��    ��: rob
* ������ڣ�2003��05��31��
* �޸ļ�¼�� 
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#define EXTERN
#include "public.h"

sp9763()  
{ 		
	int ret=0;

	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));

	/*ϵͳ���ݳ�ʼ��*/
	pub_base_sysinit();

	/** ȡֵ����ֵ **/	
	get_zd_data("0310",g_pub_tx.ac_no);					/*�����˺�*/
	pub_base_old_acno( g_pub_tx.ac_no );                 /**�Ծ��ʺŵĴ���**/

	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",g_pub_tx.ac_no);
	if( ret==100 )
	{
		sprintf(acErrMsg,"�˺Ų�����!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}

	if( strncmp(g_mdm_ac_rel.mdm_code,"0010",4) && strncmp(g_mdm_ac_rel.mdm_code,"0011",4) )
	{
		sprintf(acErrMsg,"�Ǵ������˻�,����ȷ���� :) ");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O183");
		goto ErrExit;
	}

	if( g_mdm_ac_rel.note_sts[0]=='*' )
	{
		sprintf(acErrMsg,"���˻�������������! ");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P245");
		goto ErrExit;
	}

	if( g_mdm_ac_rel.note_sts[0]=='1' )
	{
		sprintf(acErrMsg,"�ý����ѹ�ʧ,������Ҵ���! ");
		WRITEMSG
		strcpy(g_pub_tx.reply,"M009");
		goto ErrExit;
	}

	if( g_mdm_ac_rel.coll_sts[0]=='1' )
	{
		sprintf(acErrMsg,"�ý��ʴ��ڼ�ʰ״̬,������ʰ����! ");
		WRITEMSG
		strcpy(g_pub_tx.reply,"M014");
		goto ErrExit;
	}

	if( g_mdm_ac_rel.draw_pwd_yn[0]=='N'&& g_mdm_ac_rel.draw_id_yn[0]=='N')
	{
		set_zd_data("0710","1");
	}
	else if( g_mdm_ac_rel.draw_pwd_yn[0]=='Y'&& g_mdm_ac_rel.draw_id_yn[0]=='N')
	{
		set_zd_data("0710","2");
	}
	else if( g_mdm_ac_rel.draw_pwd_yn[0]=='N'&& g_mdm_ac_rel.draw_id_yn[0]=='Y')
	{
		set_zd_data("0710","3");
	}
	else if( g_mdm_ac_rel.draw_pwd_yn[0]=='Y'&& g_mdm_ac_rel.draw_id_yn[0]=='Y')
	{
		set_zd_data("0710","4");
	}

	set_zd_data("0670",g_mdm_ac_rel.draw_pwd_yn);
	set_zd_data("0680",g_mdm_ac_rel.draw_id_yn);
	set_zd_data("0690",g_mdm_ac_rel.id_type);
	set_zd_data("0310",g_pub_tx.ac_no);

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
   
