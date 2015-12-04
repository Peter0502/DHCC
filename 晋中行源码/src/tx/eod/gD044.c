/*************************************************************
* �� �� ��: gD044.c
* ��������������Աҵ����ͳ��
*
**************************************************************/
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"
#include "trace_log_c.h"
#include "com_sys_parm_c.h"

gD044()
{
	int ret = 0;
	struct com_sys_parm_c com_sys_parm_c;

	ret=sql_begin(); /*������*/
	if( ret ) 
	{
		sprintf( acErrMsg, "������ʧ��!!!" );
		WRITEMSG
		goto ErrExit;
	}

    /**------- ��ʼ��ȫ�ֱ��� --------**/
	pub_base_sysinit();

    /* ��ѯϵͳ������ */
    ret = Com_sys_parm_Sel(g_pub_tx.reply,&com_sys_parm_c,"1=1");
	if( ret ) goto ErrExit;
    g_pub_tx.tx_date = com_sys_parm_c.lst_date; /* ���̺��� */

	ret=pub_pubdb_gyywl_Dec();
	if( ret ) goto ErrExit;

OkExit:
	sql_commit();
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"����Աҵ����ͳ�Ƴɹ�!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();
	sprintf(acErrMsg,"����Աҵ����ͳ��ʧ��!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
