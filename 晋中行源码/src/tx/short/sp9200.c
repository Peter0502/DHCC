/*************************************************
* �� �� ��:  sp9200.c
* ���������� �жϹ�Ա��
*
* ��    ��:  wyb
* ������ڣ� 2006��12��12��
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

int sp9200()
{
	int ret=0;
	char tel[5];

	struct com_tel_c g_com_tel;

	memset( &g_com_tel, 0x00, sizeof( struct com_tel_c ) );
pub_base_sysinit();
	get_zd_data( "0920", tel );					/* ��Ա�� */

	ret = Com_tel_Sel(g_pub_tx.reply,&g_com_tel,"tel='%s'",tel);
	if( ret )
	{
		sprintf(acErrMsg,"�����ڸù�Ա!!tel=[%s]",tel);
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}
	vtcp_log("aaaaaa[%s]/n",g_pub_tx.tx_br_no);
	vtcp_log("aaaaaa[%s]/n",g_com_tel.br_no);
	if(strcmp(g_com_tel.br_no,g_pub_tx.tx_br_no))
		{
			sprintf(acErrMsg,"�ù�Ա���Ǳ�������Ա!!tel=[%s]",tel);
			WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
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
