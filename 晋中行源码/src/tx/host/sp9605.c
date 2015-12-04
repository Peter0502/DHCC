/*************************************************
* �� �� ��: sp9605.c
* �����������޸�Ӫҵ����
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

int sp9605( )
{
    int ret;
    char br_type[2];
    struct com_branch_c com_branch_c,com_branch;

    memset(&com_branch_c,0x0,sizeof(struct com_branch_c));
    memset(&com_branch, 0x0, sizeof(struct com_branch_c));

    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"��ʼ�����ýṹ����!!");
        WRITEMSG
        goto ErrExit;
    }
 
    get_zd_data("0910",com_branch_c.br_no);

    /* �������Ƿ���� */
    ret = Com_branch_Dec_Upd(g_pub_tx.reply,"br_no='%s'",com_branch_c.br_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR !! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }
    ret = Com_branch_Fet_Upd(&com_branch_c, g_pub_tx.reply);
    if (ret != 0 && ret != 100)
    {
        sprintf(acErrMsg,"��ѯ��������������!![%d]",ret);
        WRITEMSG
        goto ErrExit;
    }
    else if (ret == 100)
    {
        sprintf(acErrMsg,"������������в����ڻ���!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"O127");
        goto ErrExit;
    }

    if (atoi(com_branch_c.br_type) < 4 && \
             strcmp(com_branch_c.br_type,"") != 0)
    {
        sprintf(acErrMsg,"�û���û���ϼ�����!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"O128");
        goto ErrExit;
    }

    get_zd_data("0250",com_branch_c.br_name);
    if ( strcmp(com_branch_c.br_type,"") != 0)
    {
        /* ����ϼ������Ƿ���� */
        get_zd_data("0640",com_branch_c.up_br_no);
        ret = Com_branch_Sel(g_pub_tx.reply,&com_branch,"br_no='%s'",
                com_branch_c.up_br_no);
        if (ret != 0 && ret != 100)
        {
            sprintf(acErrMsg,"��ѯ��������������!![%d]",ret);
            WRITEMSG
            goto ErrExit;
        }
        else if (ret == 100)
        {
            sprintf(acErrMsg,"�������������û�и��ϼ�����!!");
            WRITEMSG
            strcpy(g_pub_tx.reply,"O126");
            goto ErrExit;
        }
        get_zd_data("0670",br_type);   
        if (atoi(br_type) < 4)
        {
            sprintf(acErrMsg,"�������Ͳ��ܴ�֧�иĳ����м���!!");
            WRITEMSG
            goto ErrExit;
        }
        strcpy(com_branch_c.br_type,br_type);
    }

    get_zd_data("0580",com_branch_c.ipaddr);
    get_zd_data("0680",com_branch_c.ind);
    get_zd_long("0480",&com_branch_c.city_no);
    get_zd_data("0730",com_branch_c.spe_ind);
	get_zd_data("0630", com_branch_c.br_tele);
	strcpy(com_branch_c.wrk_sts, "2");

    sprintf(acErrMsg,"spe_ind =[%s]",com_branch_c.spe_ind);
    WRITEMSG

    ret = Com_branch_Upd_Upd(com_branch_c, g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"UPDATE COM_BRANCH ERROR ! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }
    Com_branch_Clo_Upd();
       
    strcpy(g_pub_tx.ac_no,"");
    strcpy(g_pub_tx.brf,"�޸�Ӫҵ����");

    if (pub_ins_trace_log())
    {
        sprintf(acErrMsg,"�Ǽǽ�����ˮ����!!");
        WRITEMSG
        goto ErrExit;
    }

       
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"�޸�Ӫҵ�����ɹ� reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"�޸�Ӫҵ����ʧ�� reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
