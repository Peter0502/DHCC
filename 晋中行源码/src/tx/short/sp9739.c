/*************************************************
* �� �� ��:    sp9739.c
* ����������   
*              ���ݽ������෵��֧ȡ��ʽ
*
* ��    ��:    
* ������ڣ�   2003��01��13��
* �޸ļ�¼��   
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#define EXTERN
#include "public.h"
#include "mdm_attr_c.h"

sp9739()  
{ 		
	int ret=0;
	char mdm_code[5];

	struct mdm_attr_c g_mdm_attr;

	memset(&g_mdm_attr,0x00,sizeof(struct mdm_attr_c));

	/** ȡֵ����ֵ **/	
	get_zd_data("1161",mdm_code);					/*��������*/
	
	/* ���ݽ�������ȡ��ز���*/
	ret=Mdm_attr_Sel(g_pub_tx.reply,&g_mdm_attr,"mdm_code='%s'",mdm_code);
  	if( ret==100 )
	{
	  	sprintf(acErrMsg,"�����ڸ�ƾ֤����!!mdm_code=[%s]",mdm_code);
	  	WRITEMSG
		strcpy(g_pub_tx.reply,"H009");
		goto ErrExit;
	}

	/* ������� */
	if(g_mdm_attr.draw_pwd_yn[0]=='O')
		strcpy(g_mdm_attr.draw_pwd_yn,"N");

	if(g_mdm_attr.draw_id_yn[0]=='O')
		strcpy(g_mdm_attr.draw_id_yn,"N");

	if(g_mdm_attr.pwd_mach_yn[0]=='O')
		strcpy(g_mdm_attr.pwd_mach_yn,"N");

	set_zd_data("1155", g_mdm_attr.draw_id_yn);
	set_zd_data("1159", g_mdm_attr.qry_pwd_yn);
	set_zd_data("1152", g_mdm_attr.draw_pwd_yn);
	set_zd_data("115A", g_mdm_attr.pwd_mach_yn);

	sprintf(acErrMsg,"AZZ=================g_mdm_attr.no_ind[%s]", \
			g_mdm_attr.no_ind);
	WRITEMSG
	set_zd_data("1169", g_mdm_attr.no_ind);			/*��������*/

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
