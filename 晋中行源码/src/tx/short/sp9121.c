/*************************************************
* �� �� ��:  sp9121.c
* ���������� �жϹ�Ա����
*
* ��    ��:  shiweina
* ������ڣ� 2009��7��8�� 
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "com_tel_c.h"

int 
sp9121()
{
	struct com_tel_c sComTel;	/* ��Ա��Ϣ��ṹ */
	int             ret;
	char cTel[TEL_LENGTH+1];
	char cTel2[TEL_LENGTH+1];
	char lvl1[2]; 
	char lvl2[2];
	/* begin add by LiuHuafeng 20090718 */
	memset(&sComTel , 0 , sizeof(sComTel));
	/* end by LiuHuafeng 20090718 */
	memset(cTel,0x00,sizeof(cTel));
	memset(cTel2,0x00,sizeof(cTel2));
	memset(lvl1,0x00,sizeof(lvl1));
	memset(lvl2,0x00,sizeof(lvl2));
	/****
	get_zd_data("0910",cTel); 
	Ҫ��ѯ�Ĳ���Ա 
	**/
	/* rem by LiuHuafeng 20090719 �õ�92���������Ա
	*get_zd_data("0280",cTel); 
	***Ҫ��ѯ�Ĳ���Ա ****/
	get_zd_data("0920",cTel); /****Ҫ��ѯ�Ĳ���Ա ****/
	get_zd_data("0070",g_pub_tx.tel);
	sprintf(acErrMsg, "tel[%s]", g_pub_tx.tel);
	WRITEMSG
	/****ȥ����Ա����Ŀո�****/
	pub_base_strpack_all(cTel);
	vtcp_log("%s,%d,��������",__FILE__,__LINE__);
		
	ret = Com_tel_Sel(g_pub_tx.reply, &sComTel, \
				  "tel='%s'", g_pub_tx.tel);
	if (ret == 100) {
			sprintf(acErrMsg, "ȡ��Ա��Ϣ���!\
				tel=[%s]", g_pub_tx.tel);
			WRITEMSG
				strcpy(g_pub_tx.reply, "S009");
			goto ErrExit;
	} else if (ret) {
			sprintf(acErrMsg, "ִ��Com_tel_Sel����!ret=[%d]", ret);
			WRITEMSG
				goto ErrExit;
	}
	vtcp_log("%s,%d,g_pub_tx.tel='%s'",__FILE__,__LINE__,g_pub_tx.tel);
	memcpy(lvl1,sComTel.lvl,sizeof(lvl1)-1); /**����Ա�ļ���**/
		
	ret = Com_tel_Sel(g_pub_tx.reply, &sComTel, \
				  "tel='%s'", cTel);
	if (ret == 100) {
		sprintf(acErrMsg, "ȡ��Ա��Ϣ���!\
			tel=[%s]", cTel);
		WRITEMSG
			strcpy(g_pub_tx.reply, "S009");
		goto ErrExit;
	} else if (ret) {
		sprintf(acErrMsg, "ִ��Com_tel_Sel����!ret=[%d]", ret);
		WRITEMSG
			goto ErrExit;
	}
	memcpy(lvl2,sComTel.lvl,sizeof(lvl2)-1);  /**Ҫ��ѯ�Ĺ�Ա�ļ���**/
	vtcp_log("%s,%d,��������",__FILE__,__LINE__);
	/**	
	 *if(strcmp(lvl1,"1")==0 || strcmp(lvl1,"2")==0)
	 *{
	 *	if(strcmp(g_pub_tx.tel,cTel)!=0)
	 *	{
	 *		vtcp_log("%s,%d,������ͼ���Ĺ�Ա%s,%s,%s,%s",__FILE__,__LINE__,g_pub_tx.tel,cTel,lvl1,lvl2);
	 *		sprintf(acErrMsg,"�����������Ĺ�Ա���ܻ���");
	 *		WRITEMSG
	 *		goto ErrExit;
	 *	}
	 *}
	 *else if(strcmp(lvl1,"5")==0)
	 *{
	 *	sprintf(acErrMsg,"ƾ֤����Ա���Բ������˵�ƾ֤");
	 *}
	 *else if(strcmp(lvl1,lvl2)<0)
	 *{
	 *	vtcp_log("%s,%d,���ܲ�ѯ�߼��Ĺ�Ա%s,%s",__FILE__,__LINE__,lvl1,lvl2);
	 *	sprintf(acErrMsg,"����ƾ֤����Ա�����ܲ�ѯ���Լ�����ߵĹ�Ա��ƾ֤");
	 *	WRITEMSG
	 *	goto ErrExit;
	 *}
	 **/
	memcpy(cTel2,g_pub_tx.tel,sizeof(cTel2)-1);
	vtcp_log("%s,%d,��������",__FILE__,__LINE__);
	set_zd_data("0680",lvl1);
	set_zd_data("0690",lvl2);
	set_zd_data("0790",cTel2);
	vtcp_log("%s,%d,�������׹�Ա='%s' ��Ա����%s",__FILE__,__LINE__,g_pub_tx.tel,lvl1);
	vtcp_log("%s,%d,������ѯ�Ĺ�Ա='%s' ",__FILE__,__LINE__,cTel);
		
	
OkExit:
	strcpy(g_pub_tx.reply, "0000");
	sprintf(acErrMsg, "Before OK return: reply is[%s]", g_pub_tx.reply);
	WRITEMSG
		set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg, "Before return: reply is[%s]", g_pub_tx.reply);
	WRITEMSG
		set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 1;
}
