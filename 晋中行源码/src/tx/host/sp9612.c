/*************************************************
* �� �� ��: sp9612.c
* ������������������
*
* ��    ��: rob 
* ������ڣ�20030319
*
* �޸ļ�¼�� 
* ��   ��:
* �� �� ��:
* �޸�����:
*************************************************/

#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "com_sys_parm_c.h"

int sp9612( )
{
    int ret;
    char filename[30];
    char tabname[30];

    char filter[500];

    struct com_sys_parm_c com_sys_parm_c;

    memset(&com_sys_parm_c, 0x0, sizeof(struct com_sys_parm_c));
    memset(filename, 0x0, sizeof(filename));
    memset(tabname, 0x0, sizeof(tabname));
    memset(filter, 0x0, sizeof(filter));

    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"��ʼ�����ýṹ����!!");
        WRITEMSG
        goto ErrExit;
    }
 
    get_zd_long("0440",&g_pub_tx.open_date); /* �������� */
    get_zd_data("0300",g_pub_tx.ac_no);   /* ���� */
    get_zd_data("0310",g_pub_tx.ac_no1);  /* �ֶ��� */

    strcpy(tabname,g_pub_tx.ac_no);
    
    ret = Com_sys_parm(g_pub_tx.reply,&com_sys_parm_c,"1=1");
    if (ret != 0)
    {
        sprintf(acErrMsg,"��ѯϵͳ���������!! [%d]",ret);
        WRITEMSG
        return ret;
    }

    /* �ж�ϵͳ�Ƿ��Ѿ�������� */
    if (com_sys_parm_c.sys_sts == 1)
    {
        sprintf(acErrMsg,"ϵͳû�н��б���,���ܽ�����������!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"O130");
        goto ErrExit;
    }
       
    /* ����sql��佫�������ݵ������ı� */
    sprintf(filter,"where %s >= %ld ",g_pub_tx.ac_no1,g_pub_tx.open_date);

    Pubscname(filename);
    ret = tab_unload(filename,tabname,filter,NULL);
    if (ret != 0)
    {
        sprintf(acErrMsg,"�����������ݿ⵹���ı��ļ�����!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"O131");
        goto ErrExit;
    }

    /* ɾ���ñ� */
    ret = sql_execute("drop table %s",tabname);
    if (ret != 0)
    {
        sprintf(acErrMsg,"ִ��ɾ�����������!![%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    /* �½��ñ� */
    ret = pub_base_creattable(tabname,g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"�½��ñ����!![%s]",tabname);
        WRITEMSG
        goto ErrExit;
    }

    /* �������ļ��������ݿ��� */
    ret = tab_load(filename,tabname,NULL,NULL);
    if (ret != 0)
    {
        sprintf(acErrMsg,"�������ļ��������ݿ��д���!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"");
        goto ErrExit;
    }

    strcpy(g_pub_tx.ac_no,"");
    strcpy(g_pub_tx.brf,"��������");

    if (pub_ins_trace_log())
    {
        sprintf(acErrMsg,"�Ǽǽ�����ˮ����!!");
        WRITEMSG
        goto ErrExit;
    }
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"��������ɹ� reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"��������ʧ�� reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
