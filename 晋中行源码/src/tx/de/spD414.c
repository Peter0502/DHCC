/***************************************************************
* �� �� ��: spD414.c
* ��������������д��
* ��    ��: mike
* ������ڣ�2003��06��05��
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"

spD414()
{
	int ret = 0;
	char tmp_ac_no[20];

	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));

	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();

	/*** ȡ�������  ***/
	get_zd_data("0310",g_pub_tx.ac_no);							/*�ʺ�*/

	get_zd_data("0670",g_pub_tx.draw_pwd_yn);
	get_zd_data("0790",g_pub_tx.draw_pwd);
 	get_zd_data("0680",g_pub_tx.draw_id_yn);
	get_zd_data("0690",g_pub_tx.id_type);
	get_zd_data("0620",g_pub_tx.id_no);

	ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'", \
						 g_pub_tx.ac_no);
	if( ret==100 )
	{
	  	sprintf(acErrMsg,"�����˻����ձ��в����ڸü�¼!");
	   	WRITEMSG
		strcpy(g_pub_tx.reply,"P237");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"���Ҽ�¼����[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	/*���֧ȡ��ʽ�Ƿ�Ϸ�*/
	ret=pub_base_check_draw("001");
	if( ret )
	{
	 	sprintf(acErrMsg,"���ü��֧ȡ��ʽ�Ƿ�Ϸ���������!!");
		WRITEMSG
		goto ErrExit;
	}

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"����д�ųɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"����д��ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
