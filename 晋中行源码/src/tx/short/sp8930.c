/*************************************************
* �� �� ��:  sp8930.c
* ���������� 9630�ķ�cp ������+�ն˺�_tmp ������+�ն˺�
*
* ��    ��:  martin 
* ������ڣ� 2008��8��5��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
  insert into tx_def values('8930','9630��cp','10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000','0','4','0');
	insert into tx_flow_def values('8930','0','8930','#$');
	insert into tx_sub_def values('8930','9630��cp','sp8930','0','0');
*************************************************/
#include <string.h>
#include <errno.h>
#define EXTERN
#include "public.h"

sp8930()
{
	int             ret=0, i = 0, vcnt=0;
	char            cBr_no[6];
	char            cTtyno[5];
	char            tel[5], filename[300];
  char            execstr[256],filename1[300],filename2[300];

	
	/* ��� */
	memset(cBr_no ,0x0,sizeof(cBr_no));
	memset(cTtyno ,0x0,sizeof(cTtyno));
	memset(filename		, 0x00, sizeof(filename));
	memset(filename1	, 0x00, sizeof(filename1));
	memset(filename2	, 0x00, sizeof(filename2));
	/* ǰ̨ȡֵ */
	pub_base_sysinit();
	
	get_zd_data("0030",cBr_no);
	get_zd_data("0100",cTtyno);
	
	memset( execstr, 0x0, sizeof(execstr));
	sprintf(filename1,"%s/%s%s",getenv("FILDIR"),cBr_no,cTtyno);			/*ԭ�ļ���  */
	sprintf(filename2,"%s/%s%s_tmp",getenv("FILDIR"),cBr_no,cTtyno);			/*���ļ���  */
	sprintf( execstr, "cp %s %s", filename2, filename1);
	ret = system(execstr);
	if ( ret )
	{
		sprintf(acErrMsg,"�����ļ�ʧ��[%s][%s][%d][%s]", filename2, strerror(errno), ret, execstr );
		WRITEMSG
		strcpy( g_pub_tx.reply, "S047" );
		goto ErrExit;		
	}
	
  
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
