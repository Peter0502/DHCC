/*************************************************
* �� �� ��: sp9735.c
* ���������������������ȷ�Է��ر�־_[֧ȡ��Ϣά������]
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

sp9735()  
{ 		
	int ret=0;
	char ac_no[20],tmp_pass[7];
	
	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	pub_base_sysinit();

	/** ȡֵ����ֵ **/	
	get_zd_data("0300",ac_no);						/*�ʺ�*/
	get_zd_data("0790",tmp_pass);					/*����*/

	/* ���ø����ʺŷ����˻���ź��� */
	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel, "ac_no='%s'",ac_no);
	if(ret==100)
	{
		sprintf(acErrMsg,"�����ʺŶ��ձ����ڸü�¼!! [%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}

	strcpy(g_pub_tx.ac_no,ac_no);
	strcpy(g_pub_tx.draw_pwd,tmp_pass);

	/** Ϊ���֤��֧ȡ��������֤�������⣬�����޸�Ϊ��֤��֧ȡ **/
	g_mdm_ac_rel.draw_id_yn[0] = 'N';

	if( !strlen(tmp_pass) )
	{
		sprintf( acErrMsg, "ԭ�������벻��ȷ!!tmp_pass=[%s], \
				g_mdm_ac_rel.draw_pwd=[%s]",tmp_pass,g_mdm_ac_rel.draw_pwd );
		WRITEMSG
		strcpy(g_pub_tx.reply,"O157");
		goto ErrExit;
	}
	if( pub_base_check_draw("001") )
	{
		sprintf( acErrMsg, "ԭ�������벻��ȷ!!tmp_pass=[%s], \
				g_mdm_ac_rel.draw_pwd=[%s]",tmp_pass,g_mdm_ac_rel.draw_pwd );
		WRITEMSG
		/* strcpy(g_pub_tx.reply,"O157"); */
		goto ErrExit;
	}

	pub_base_EnDes(g_pub_tx.tx_date,"",tmp_pass);
    if( strcmp(tmp_pass,g_mdm_ac_rel.draw_pwd) )
	{
		sprintf( acErrMsg, "ԭ�������벻��ȷ!!tmp_pass=[%s], \
				g_mdm_ac_rel.draw_pwd=[%s]",tmp_pass,g_mdm_ac_rel.draw_pwd );
		WRITEMSG
		strcpy(g_pub_tx.reply,"O157");
		goto ErrExit;
	}

	/* ������� */
	set_zd_data("0710","1");

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
   
