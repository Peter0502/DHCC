/*************************************************************
* �� �� ��: gD124.c
* �������������ն�����������
**************************************************************/
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"

gD124()
{
	int ret = 0;

    ret=sql_begin(); /*������*/
    if(ret != 0)
    {
        sprintf( acErrMsg, "������ʧ��!!!" );
        WRITEMSG
        goto ErrExit;
    }

    /**------- ��ʼ��ȫ�ֱ��� --------**/
    pub_base_sysinit();

    ret = pub_eod_tdmst_ysbal_upd();
    if (ret != 0)
    {
        sprintf(acErrMsg,"���¶����ʻ����ļ�������������!!");
        WRITEMSG
        goto ErrExit;
    }

OkExit:
	sql_commit();	/*--�ύ����--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"������������ɹ�!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
    if (strcmp(g_pub_tx.reply,"0000") == 0 || g_pub_tx.reply[0]=='\0')
        strcpy(g_pub_tx.reply,"G009");
	sql_rollback();	/*--����ع�--*/
	sprintf(acErrMsg,"������������ʧ��!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
/**********************************************************************
 * �� �� ����   pub_eod_tdmst_ysbal_upd
 * �������ܣ�   �޸Ķ����ʻ����ļ����������
********************************************************************/
int pub_eod_tdmst_ysbal_upd()
{
    int ret;
    struct td_mst_c td_mst_c;

	vtcp_log("BEGIN TD_MST");

    memset(&td_mst_c, 0x0, sizeof(struct td_mst_c));

    ret = Td_mst_Dec_Upd(g_pub_tx.reply,"1=1");
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR!! [%d]",ret);
        WRITEMSG
        return ret;
    }

    while(1)
    {
        ret = Td_mst_Fet_Upd(&td_mst_c,g_pub_tx.reply);
        if (ret != 0 && ret != 100)
        {
            sprintf(acErrMsg,"FETCH ERROR !! [%d]",ret);
            WRITEMSG
            return ret;
        }
        else if (ret == 100)
        {
            break;
        }
	ret = pub_eod_ye_bal ( td_mst_c.ys_bal , td_mst_c.bal , td_mst_c.ac_id , td_mst_c.ac_seqn);
	if(ret)		return ret;
	
        td_mst_c.ys_bal = td_mst_c.bal;
        ret = Td_mst_Upd_Upd(td_mst_c,g_pub_tx.reply);
        if (ret != 0)
        {
            sprintf(acErrMsg,"UPDATE ERROR !! [%d]",ret);
            WRITEMSG
            return ret;
        }
    }
    Td_mst_Clo_Upd();

	vtcp_log("OVER  TD_MST");

    return 0;
}
