/*************************************************
* �� �� ��:  sp5839.c
* ���������� ������˹����ӽ���
*			       ���� �ǼǱ�
*			 
* ��    ��:  ChenMing
* ������ڣ� 2006��8��17��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/
#define  EXTERN
#include <string.h>
#include <stdio.h>
#include "public.h"
#include "hv_pub.h"
#include "hv_define.h"

int sp5839()
{
	pub_base_sysinit();

	if (pub_hv_recv_main()) goto ErrExit; /* ���ô�����ʴ��������� */
		strcpy( g_pub_tx.reply,"0000" );

OkExit:
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	strcpy( g_pub_tx.reply,"1111" );
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}


