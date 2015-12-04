/*************************************************************
* �� �� ��: gD123.c
* �������������ջ�����������
*
* ��    ��: rob
* �������: 2003��3��15��
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

gD123()
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

    ret = pub_eod_ddmst_ysbal_upd();
    if (ret != 0)
    {
        sprintf(acErrMsg,"���»����ʻ����ļ�������������!!");
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
 * �� �� ����   pub_eod_ddmst_ysbal_upd
 * �������ܣ�   �޸Ļ����ʻ����ļ����������
 * ����/ʱ�䣺  rob/20030315
********************************************************************/
int pub_eod_ddmst_ysbal_upd()
{
    int ret;
    struct dd_mst_c dd_mst_c;
	int i=0;

	vtcp_log("BEGIN DD_MST");

    memset(&dd_mst_c, 0x0, sizeof(struct dd_mst_c));

    ret = Dd_mst_Dec_Upd(g_pub_tx.reply,"1=1");
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR!! [%d]",ret);
        WRITEMSG
        return ret;
    }

	i=0;
    while(1)
    {
        ret = Dd_mst_Fet_Upd(&dd_mst_c,g_pub_tx.reply);
        if (ret == 100) break;
        if ( ret )
        {
            sprintf(acErrMsg,"FETCH ERROR !! [%d]",ret);
            WRITEMSG
            return ret;
        }

		i++;
		vtcp_log("[%d-%d,%d]",i,dd_mst_c.ac_id,dd_mst_c.ac_seqn);
        
		ret = pub_eod_ye_bal ( dd_mst_c.ys_bal , dd_mst_c.bal , 
			dd_mst_c.ac_id , dd_mst_c.ac_seqn);
		if(ret)		return ret;
	        
		if( pub_base_CompDblVal(dd_mst_c.ys_bal,dd_mst_c.bal) )
		{
        	dd_mst_c.ys_bal = dd_mst_c.bal;
                
        	ret = Dd_mst_Upd_Upd(dd_mst_c,g_pub_tx.reply);
        	if ( ret )
        	{
            	sprintf(acErrMsg,"UPDATE ERROR !! [%d]",ret);
            	WRITEMSG
            	return ret;
        	}
		}
    }
    Dd_mst_Clo_Upd();

	vtcp_log("OVER  DD_MST");

    return 0;
}
