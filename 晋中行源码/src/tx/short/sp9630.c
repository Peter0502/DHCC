/*************************************************
* �� �� ��:  sp9630.c
* ���������� �ɹ�Ա�������еĿ�ת���ĵȼ���Ϣ
*
* ��    ��:  ligl 
* ������ڣ� 2008��8��5��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/
#include <string.h>
#include <errno.h>
#define EXTERN
#include "public.h"
#include "prdt_ac_id_c.h"
#include "com_tel_change_c.h"
#include "com_tel_c.h"

sp9630()
{
	int             ret=0, i = 0, vcnt=0;
	char            cBr_no[6];
	char            cTtyno[5];
	char            tel[5], filename[300];
  char            execstr[256],filename1[300],filename2[300];
	FILE           	*fp;

	struct com_tel_change_c 	g_com_tel_change;	/* ��Ʒ�˻��ṹ */
	struct com_tel_c 	vcom_tel;	/* ��Ʒ�˻��ṹ */
	/* ��� */
	memset(&g_com_tel_change	, 0x00, sizeof(struct com_tel_change_c));
	memset(&vcom_tel	, 0x00, sizeof( vcom_tel));
	memset(tel		, 0x00, sizeof(tel));
	memset(cBr_no ,0x0,sizeof(cBr_no));
	memset(cTtyno ,0x0,sizeof(cTtyno));
	memset(filename		, 0x00, sizeof(filename));
	memset(filename1	, 0x00, sizeof(filename1));
	memset(filename2	, 0x00, sizeof(filename2));
	/* ǰ̨ȡֵ */
	pub_base_sysinit();
	
	get_zd_data("0030",cBr_no);
	get_zd_data("0100",cTtyno);
	
	vtcp_log("%s,%d,#############cTtyno[%s] cBr_no[%s]",__FILE__,__LINE__,cTtyno,cBr_no);
	
	memset( execstr, 0x0, sizeof(execstr));
	sprintf(filename1,"%s/%s%s",getenv("FILDIR"),cBr_no,cTtyno);			/*ԭ�ļ���  */
	sprintf(filename2,"%s/%s%s_tmp",getenv("FILDIR"),cBr_no,cTtyno);			/*���ļ���  */
	vtcp_log("%s,%d,#############filename1[%s] filename2[%s]",__FILE__,__LINE__,filename1,filename2);
	sprintf( execstr, "cp %s %s", filename1, filename2);
	ret = system(execstr);
	if ( ret )
	{
		sprintf(acErrMsg,"�����ļ�ʧ��[%s][%s][%d][%s]", filename2, strerror(errno), ret, execstr );
		WRITEMSG
		strcpy( g_pub_tx.reply, "S047" );
		/*goto ErrExit;		*/
	}
	
  /* �����´�ȫ·���ļ���(����) */
	pub_base_AllDwnFilName(filename);
	vtcp_log("%s,%d,#############  filename[%s]  &&&&&&&&&&&",__FILE__,__LINE__,filename);
	fp = fopen(filename, "w");
	if (fp == NULL) {
		sprintf(acErrMsg, "open file error [%s]", filename);
		WRITEMSG
			strcpy(g_pub_tx.reply, "S047");
		goto ErrExit;
	}
	ret = Com_tel_change_Dec_Sel(g_pub_tx.reply,"tel='%s' and sts='1'",g_pub_tx.tel);
	if (ret == 100) 
	{
		sprintf( acErrMsg,"��Ա������[%s]",g_pub_tx.tel );
		WRITEMSG
		strcpy( g_pub_tx.reply,"O120" );
		goto ErrExit;
	} 
	while (1) {
		memset(&g_com_tel_change,0x00,sizeof(g_com_tel_change));
		memset(&vcom_tel,0x00,sizeof(vcom_tel));
		ret = Com_tel_change_Fet_Sel(&g_com_tel_change, g_pub_tx.reply);
		if (ret == 100)
		{
	 		break;
		}
		else if(ret!=0)
		{
			goto ErrExit;
		}
			/**��ʼ���ʺ���Ϣд���ļ�**/
			/*** ���ɲ�Ʒ�б� ***/	
			ret=Com_tel_Sel(g_pub_tx.reply,&vcom_tel,"tel='%s'",g_com_tel_change.to_tel);	
	    if(ret)
	    {
	    	goto ErrExit;
	    }	  
			  fprintf(fp, "%-4s|%-10s|\n", g_com_tel_change.to_tel,vcom_tel.tx_type);
	}
	Com_tel_change_Clo_Sel();
	
	set_zd_data(DC_FILE_SND, "1");
	fclose(fp);
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
