/*************************************************************
* �� �� ��: gD025A.c
* ����������Ϊ������׼����ˮdc_log->dc_log_bk
*
* ��    ��: jack
* �������: 2003��3��8��
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
#include "com_sys_parm_c.h"

gD025A()
{
    int ret=0;
	double d_amt=0.00, c_amt=0.00;
    struct com_sys_parm_c com_sys_parm_c;

    sprintf(acErrMsg,"���̿�ʼ!!!!");
    WRITEMSG

	ret=sql_begin(); /*������*/
	if(ret != 0) 
	{
		sprintf( acErrMsg, "������ʧ��!!!" );
		WRITEMSG
		goto ErrExit;
	}

    /**------- ��ʼ��ȫ�ֱ��� --------**/
	pub_base_sysinit();

    /* ɾ����ʱ�� �����˴��������ڱ����� gl_mst_hst*/
    ret = pub_pubdb_delcretable();
    if (ret != 0)
    {
        sprintf(acErrMsg,"ɾ���������ݱ����!!");
        WRITEMSG
        goto ErrExit;
    }
OkExit:
	sql_commit();	/*--�ύ����--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"����׼����������ˮ�ɹ�!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--����ع�--*/
	if (strcmp(g_pub_tx.reply,"0000") == 0)
		strcpy(g_pub_tx.reply,"G108");
	sprintf(acErrMsg,"����׼����������ˮʧ��!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
