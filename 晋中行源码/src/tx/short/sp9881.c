/*************************************************
* �� �� ��:  sp9881.c
* ���������� ���ݶԷ�����������������������ڲ��˼��˿�Ŀ110(���в���Ϊ�ֽ��ո�����)
*
* ��    ��:  jane
* ������ڣ� 2003��3��31�� 
*
* �޸ļ�¼�� 
* ��   ��:
* �� �� ��:
* �޸�����:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "com_item_c.h"

int sp9881()
{
    	struct 	com_item_c 	sCom_item_c;		/* ������Ŀ�� */
    	char	cTw_br_no[2];				/* �Է��������� */
    	int ret;

    	memset( &sCom_item_c, 0x00, sizeof(struct com_item_c) );
    	memset( &cTw_br_no, 0x00, sizeof(cTw_br_no) );

    	get_zd_data( "0680", cTw_br_no );

	if( cTw_br_no[0] == '1' )			/* ���� */
		set_zd_data( "0300", "110"); 		/* �ͻ��˺� */
	else
		set_zd_data( "0300", ""); 		/* �ͻ��˺� */

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"success !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"fail !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
