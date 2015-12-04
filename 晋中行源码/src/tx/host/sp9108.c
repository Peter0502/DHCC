/*************************************************
* �� �� ��: sp9108.c
* �����������ָ�Ӫҵ��״̬
*
* ��    ��: rob
* ������ڣ�20030322
*
* �޸ļ�¼�� 
* ��   ��:
* �� �� ��:
* �޸�����:
*************************************************/

#include <stdio.h>
#include <math.h>
#include <errno.h>
#define EXTERN
#include "public.h"
#include "com_branch_c.h"

int sp9108( )
{
    int ret;
    char br_no[6];
  
    struct com_branch_c com_branch_c;

    memset(&com_branch_c, 0x0, sizeof(struct com_branch_c));

    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"��ʼ�����ýṹ����!!");
        WRITEMSG
        goto ErrExit;
    }

    get_zd_data("0910",br_no);

    ret = Com_branch_Dec_Upd(g_pub_tx.reply,"br_no='%s'",br_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR ERROR !! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    ret = Com_branch_Fet_Upd(&com_branch_c, g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"FETCH ERROR !! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    if (com_branch_c.wrk_sts[0] == '1')
    {
        sprintf(acErrMsg,"��Ӫҵ���Ѿ�ǩ��!![%s]",
                com_branch_c.wrk_sts);
        WRITEMSG
        strcpy(g_pub_tx.reply,"O069");
        goto ErrExit;
    }

    if (com_branch_c.wrk_sts[0] == '*')
    {
        sprintf(acErrMsg,"��Ӫҵ���Ѿ��ر�!![%s]",
                com_branch_c.wrk_sts);
        WRITEMSG
        strcpy(g_pub_tx.reply,"O068");
        goto ErrExit;
    }

    strcpy(com_branch_c.wrk_sts, "1");
    ret = Com_branch_Upd_Upd(com_branch_c, g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"UPDATE ERROR !! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }
    Com_branch_Clo_Upd();

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"�ָ�Ӫҵ��״̬�ɹ� reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"�ָ�Ӫҵ��״̬ʧ�� reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
