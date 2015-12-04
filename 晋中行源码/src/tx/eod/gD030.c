/*************************************************************
* �� �� ��: gD030.c
* �����������������ƽ����
*
* ��    ��: jack
* �������: 2003��3��12��
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

gD030()
{
	int ret = 0;
    struct dd_mst_c dd_mst_c;
    struct dd_parm_c dd_parm_c;
    struct com_sys_parm_c com_sys_parm_c;

    memset(&dd_mst_c,0x0,sizeof(struct dd_mst_c));
    memset(&dd_parm_c,0x0,sizeof(struct dd_parm_c));
    memset(&com_sys_parm_c,0x0,sizeof(struct com_sys_parm_c));

    ret=sql_begin(); /*������*/
    if(ret != 0)
    {
        sprintf( acErrMsg, "������ʧ��!!!" );
        WRITEMSG
        goto ErrExit;
    }

    /**------- ��ʼ��ȫ�ֱ��� --------**/
    pub_base_sysinit();

	/***xxx 20031208***
    ret = pub_eod_checktrace_mst();
    if (ret != 0)
    {
        sprintf(acErrMsg,"ҵ����ˮ�����ļ����ƽ�����!!");
        WRITEMSG
        goto ErrExit;
    }
	**************/

	/********xxx 20031307
    ret = pub_eod_checkmst_trace();
    if (ret != 0)
    {
        sprintf(acErrMsg,"���ļ���ҵ����ˮ���ƽ�����!!");
        WRITEMSG
        goto ErrExit;
    }
	*********/

	/**------ xxx --------**/
	ret = pub_eod_check_dc_log();
    if (ret != 0)
    {
        sprintf(acErrMsg,"�����ˮ������!!");
        WRITEMSG
        goto ErrExit;
    }
	ret = pub_eod_check_gl_mst();
	if( ret )
	{
        sprintf(acErrMsg,"�ܷ�ƽ�������!");
        WRITEMSG
        goto ErrExit;
	}

    /* �޸�ϵͳ״̬Ϊ���� * ����������ĵ�ϵͳ�ȶ����
    ret = Com_sys_parm_Dec_Upd(g_pub_tx.reply,"1=1");
    if (ret != 0)
    {
		sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR [%d]",ret);
		WRITEMSG
		goto ErrExit;
    }

    ret = Com_sys_parm_Fet_Upd(&com_sys_parm_c,g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"FETCH ERROR [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

	com_sys_parm_c.sys_sts = 0;

    ret = Com_sys_parm_Upd_Upd(com_sys_parm_c,g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"UPDATE ERROR [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }
    Com_sys_parm_Clo_Upd();
	*/

OkExit:
	sql_commit();	/*--�ύ����--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"������ƽ��ļ��ɹ�!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
    if (strcmp(g_pub_tx.reply,"0000") == 0 || g_pub_tx.reply[0]=='\0')
    {
        strcpy(g_pub_tx.reply,"G009");
    }
	sql_rollback();	/*--����ع�--*/
	sprintf(acErrMsg,"������ƽ��ļ��ʧ��!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
