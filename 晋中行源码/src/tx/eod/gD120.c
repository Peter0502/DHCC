/*************************************************************
* �� �� ��: gD120.c
* ��������������Ԥ����Ϣ
*
* ��    ��: mike
* �������: 20030705
**************************************************************/
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"

gD120()
{
	int ret = 0;

	ret=sql_begin();
	if( ret ) 
	{
		sprintf( acErrMsg, "������ʧ��!!!" );
		WRITEMSG
		goto ErrExit;
	}

    /**------- ��ʼ��ȫ�ֱ��� --------**/
	pub_base_sysinit();
	strcpy(g_pub_tx.tx_code,"Z120");
	strcpy(g_pub_tx.sub_tx_code,"Z120");
	strcpy(g_pub_tx.ct_ind,"2");
	strcpy(g_pub_tx.brf,"Ԥ����Ϣ");

	ret=pub_eod_put_intst();
	if( ret )
	{
		sprintf( acErrMsg, "Ԥ����Ϣ���غ�������!" );
		WRITEMSG
		goto ErrExit;
	}
OkExit:
	sql_commit();	/*--�ύ����--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--����ع�--*/
	sprintf(acErrMsg,"!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
