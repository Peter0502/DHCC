/*************************************************
* �� �� ��: sp9607.c
* �����������ϲ�Ӫҵ����
*
* ��    ��: rob
* ������ڣ�20030310
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
#include "com_branch_c.h"

int sp9607( )
{
    int ret;
    struct com_branch_c com_branch_c;

    memset(&com_branch_c,0x0,sizeof(struct com_branch_c));

    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"��ʼ�����ýṹ����!!");
        WRITEMSG
        goto ErrExit;
    }
 
    get_zd_data("0910",com_branch_c.br_no);
    get_zd_data("0250",com_branch_c.br_name);
    get_zd_data("0640",com_branch_c.up_br_no);
    get_zd_data("0260",g_pub_tx.name);

       
    strcpy(g_pub_tx.ac_no,"");
    strcpy(g_pub_tx.brf,"�ϲ�Ӫҵ����");

    if (pub_ins_trace_log())
    {
        sprintf(acErrMsg,"�Ǽǽ�����ˮ����!!");
        WRITEMSG
        goto ErrExit;
    }

       
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"�ϲ�Ӫҵ�����ɹ� reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"�ϲ�Ӫҵ����ʧ�� reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
