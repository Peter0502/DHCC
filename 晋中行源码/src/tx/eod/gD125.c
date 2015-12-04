/*************************************************************
* �� �� ��: gD125.c
* �������������մ�����������
**************************************************************/
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"
#include "cash_mst_c.h"
#include "ass_mst_c.h"
#include "mo_jl_bal_c.h"


gD125()
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

    ret = pub_eod_lnmst_ysbal_upd();
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
 * �� �� ����   pub_eod_lnmst_ysbal_upd
 * �������ܣ�   �޸Ĵ����ʻ����ļ����������
 * ����/ʱ�䣺  rob/20030315
********************************************************************/
int pub_eod_lnmst_ysbal_upd()
{
    int ret;
    struct ln_mst_c ln_mst_c;
	int i=0;

	vtcp_log("BEGIN LN_MST");

    memset(&ln_mst_c, 0x0, sizeof(struct ln_mst_c));

    ret = Ln_mst_Dec_Upd(g_pub_tx.reply,"1=1");
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR!! [%d]",ret);
        WRITEMSG
        return ret;
    }

    while(1)
    {
        ret = Ln_mst_Fet_Upd(&ln_mst_c,g_pub_tx.reply);
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

		vtcp_log("[%d-%d,%d]",++i,ln_mst_c.ac_id,ln_mst_c.ac_seqn );

		ret = pub_eod_ye_bal ( ln_mst_c.ys_bal , ln_mst_c.bal , 
			ln_mst_c.ac_id , ln_mst_c.ac_seqn);
		if(ret)		return ret;
	
        ln_mst_c.ys_bal = ln_mst_c.bal;
        ret = Ln_mst_Upd_Upd(ln_mst_c,g_pub_tx.reply);
        if (ret != 0)
        {
            sprintf(acErrMsg,"UPDATE ERROR !! [%d]",ret);
            WRITEMSG
            return ret;
        }
    }
    Ln_mst_Clo_Upd();

	vtcp_log("OVER  LN_MST");

    return 0;
}
