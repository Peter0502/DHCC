/*************************************************************
* �� �� ��: gD025.c
* �������������շ��̴���
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

gD025()
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
TRACE

    /* �޸�ϵͳ���ڡ�״̬����ʼ����ˮ����������״̬ */
    ret = Com_sys_parm_Dec_Upd(g_pub_tx.reply,"1=1");
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

TRACE
    ret = Com_sys_parm_Fet_Upd(&com_sys_parm_c,g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"FETCH ERROR [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

TRACE
/*	 modified by liuxuan 2006-11-03 �ĵ�����ƽ����gD030��
    com_sys_parm_c.sys_sts = 0;
*/
    com_sys_parm_c.lst_date=com_sys_parm_c.sys_date;
   
    ret = pub_base_end_month(com_sys_parm_c.sys_date);
    if (ret == 0)
    {
        if (pub_base_end_year(com_sys_parm_c.sys_date) == 0)
        {
            com_sys_parm_c.sys_date = \
                  (com_sys_parm_c.sys_date /10000 + 1) * 10000 + 101;
        }
        else
        {
            com_sys_parm_c.sys_date = \
                        (com_sys_parm_c.sys_date / 100 + 1 ) * 100 + 1;
        }
    }
    else
    {
        com_sys_parm_c.sys_date = com_sys_parm_c.sys_date + 1;
    }

TRACE
    ret = Com_sys_parm_Upd_Upd(com_sys_parm_c,g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"UPDATE ERROR [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }
    Com_sys_parm_Clo_Upd();

TRACE
    /* ��ʼ����ˮ */
    ret= set_tlsrno(0);
    if (ret < 0)
    {
        sprintf(acErrMsg,"��ʼ����ˮ����!!");
        WRITEMSG
        goto ErrExit;
    }

TRACE
    /* ��������״̬ */
    ret = sql_execute("update com_branch set wrk_sts='2' where wrk_sts!='*' and br_type not in('0') ");
    if (ret != 0)
    {
        sprintf(acErrMsg,"�޸�����״̬����!!");
        WRITEMSG
        goto ErrExit;
    }

OkExit:
	sql_commit();	/*--�ύ����--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"���̳ɹ�!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--����ع�--*/
	if (strcmp(g_pub_tx.reply,"0000") == 0)
		strcpy(g_pub_tx.reply,"G108");
	sprintf(acErrMsg,"����ʧ��!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
