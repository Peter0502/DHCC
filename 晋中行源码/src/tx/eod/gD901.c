/*************************************************************
* �� �� ��: gD901.c
* �������������ݿ��Ż�
*
* �������: 2003��3��5��
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
**************************************************************/
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"

gD901()
{
	int ret = 0;
	int br_type;
sprintf(acErrMsg, "%d@%s  ====> iimyaimyaimyai", __LINE__, __FILE__);
WRITEMSG
	ret = sql_execute("UPDATE STATISTICS");
	if ( ret )
	{
		sprintf( acErrMsg, "����!!!");
		WRITEMSG
		strcpy( g_pub_tx.reply, "G005" );
		goto ErrExit;
	}

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"���������Ż�ͨ��!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"���������Ż�ʧ��!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
