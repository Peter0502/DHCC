/*************************************************************
* �� �� ��: gD999.c
* �������������Ժ����õ���ʱ����
*
* ��    ��: 
* �������: 
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

gD999()
{
	int ret = 0;

	ret=sql_begin(); /*������*/
	if( ret ) 
	{
		sprintf( acErrMsg, "������ʧ��!!!" );
		WRITEMSG
		goto ErrExit;
	}

    /**------- ��ʼ��ȫ�ֱ��� --------**/
	pub_base_sysinit();

vtcp_log("xxx--- look001");
	ret=sql_execute("update dd_mst set prdt_no='101' where ac_id=100000025 and ac_seqn=1 ");
	if( ret )
	{
		vtcp_log("sql_execute erro ![%d]",ret);
		strcpy(g_pub_tx.reply,"P001");
		return -1;
	}
vtcp_log("xxx --- look991");


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
