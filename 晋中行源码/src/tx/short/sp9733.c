/*************************************************
* �� �� ��: sp9733.c
* ���������������ʺŻ�����Ϣ___[�����ʽ���]
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
#include "prdt_ac_id_c.h"
#include "mdm_attr_c.h"

sp9733( )  
{ 		
	int ret=0;
	double bal;
	char ac_no[20],note_no[20],name[51];
	
	struct prdt_ac_id_c g_prdt_ac_id;
	struct mdm_attr_c g_mdm_attr;

	/** ȡֵ����ֵ **/	
	get_zd_data("1051",ac_no);					/*�ʺ�*/
	pub_base_old_acno( ac_no );                 /**�Ծ��ʺŵĴ���**/

	ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",ac_no);
	if( ret==100 )
	{
		 sprintf(acErrMsg,"��Ʒ�ʺŶ��ձ����ڸü�¼!![%d]",ret);
		 WRITEMSG
		 strcpy(g_pub_tx.reply,"M003");
		 goto ErrExit;
	}
	else if( ret )
	{
		 sprintf(acErrMsg,"���ҳ���!![%d]",ret);
		 WRITEMSG
		 strcpy(g_pub_tx.reply,"D103");
		 goto ErrExit;
	}

	if( g_mdm_ac_rel.note_sts[0]=='*')
	{
		 sprintf(acErrMsg,"�ý�������!");
		 WRITEMSG
		 strcpy(g_pub_tx.reply,"M012");
		 goto ErrExit;
	}

	ret = Mdm_attr_Sel( g_pub_tx.reply,&g_mdm_attr,"mdm_code='%s'", \
					 	g_mdm_ac_rel.mdm_code);
	if( ret==100 )
	{
		 sprintf(acErrMsg,"�����ʺŶ��ձ����ڸü�¼!![%d]",ret);
		 WRITEMSG
		 strcpy(g_pub_tx.reply,"M003");
		 goto ErrExit;
	}
	else if( ret )
	{
		 sprintf(acErrMsg,"���ҳ���!![%d]",ret);
		 WRITEMSG
		 strcpy(g_pub_tx.reply,"D103");
		 goto ErrExit;
	}

	ret = Prdt_ac_id_Sel( g_pub_tx.reply,&g_prdt_ac_id,"ac_id=%ld", \
						   g_mdm_ac_rel.ac_id);
	if( ret==100 )
	{
		 sprintf(acErrMsg,"��Ʒ�ʺŶ��ձ����ڸü�¼!![%d]",ret);
		 WRITEMSG
		 strcpy(g_pub_tx.reply,"M003");
		 goto ErrExit;
	}
	else if( ret )
	{
		 sprintf(acErrMsg,"���ҳ���!![%d]",ret);
		 WRITEMSG
		 strcpy(g_pub_tx.reply,"D103");
		 goto ErrExit;
	}

	switch(g_prdt_ac_id.prdt_no[0])
	{
		case '1': 
				ret = Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,"ac_id=%ld", \
								 g_mdm_ac_rel.ac_id);
				if( ret==100 )
				{
		 			sprintf(acErrMsg," �˺Ų�����![%ld]",g_mdm_ac_rel.ac_id );
		 			WRITEMSG
		 			strcpy(g_pub_tx.reply,"M003");
		 			goto ErrExit;
				}
				else if( ret )
				{
		 			sprintf(acErrMsg,"���ҳ���!![%d]",ret);
					WRITEMSG
		 			strcpy(g_pub_tx.reply,"D103");
		 			goto ErrExit;
				}
				strcpy(name,g_dd_mst.name);
				break;
		case '2':
				ret=Td_mst_Sel(g_pub_tx.reply,&g_td_mst,"ac_id=%ld", \
								 g_mdm_ac_rel.ac_id);
				if( ret==100 )
				{
		 			sprintf(acErrMsg,"�˺Ų�����!![%ld]",g_mdm_ac_rel.ac_id );
		 			WRITEMSG
		 			strcpy(g_pub_tx.reply,"M003");
		 			goto ErrExit;
				}
				else if( ret )
				{
		 			sprintf(acErrMsg,"���ҳ���!![%d]",ret);
					WRITEMSG
		 			strcpy(g_pub_tx.reply,"D103");
		 			goto ErrExit;
				}
				strcpy(name,g_td_mst.name);
				break;
		default :
		 		sprintf(acErrMsg,"�Ǵ�ȡ�����ʻ�!![%s][%s]", \
						g_prdt_ac_id.prdt_no, ac_no);
		 		WRITEMSG
		 		strcpy(g_pub_tx.reply,"O166");
		 		goto ErrExit;
				break;
	}

	set_zd_data("105B",name);						/*����*/
	set_zd_data("1051",ac_no);						/*�˺�*/
	set_zd_data("1023",g_mdm_attr.note_type);		/*ƾ֤����*/
	set_zd_data("1024",g_mdm_ac_rel.note_no);		/*ƾ֤����*/

	set_zd_data("1054",g_mdm_ac_rel.draw_pwd_yn);	/*�Ƿ�����֧ȡ*/
	set_zd_data("1057",g_mdm_ac_rel.draw_id_yn);	/*�Ƿ�֤��֧ȡ*/

	if(g_mdm_ac_rel.draw_id_yn[0]=='Y')
	{
		set_zd_data("105A",g_mdm_ac_rel.id_type);  /*֤������*/
		set_zd_data("1058",g_mdm_ac_rel.id_no);    /*֤������*/
	}

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
   
