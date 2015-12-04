/*************************************************
* �� �� ��: sp9604.c
* ��������������Ӫҵ����
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

int sp9604( )
{
    int ret;
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
    get_zd_data("0250",com_branch_c.br_name);
    get_zd_data("0640",com_branch_c.up_br_no);
    get_zd_data("0580",com_branch_c.ipaddr);
    get_zd_data("0670",com_branch_c.br_type);   
    get_zd_data("0680",com_branch_c.ind);
	get_zd_data("0630", com_branch_c.br_tele);
       

TRACE
    /* �������Ƿ���� */   
    ret = Com_branch_Sel(g_pub_tx.reply,&com_branch_c,"br_no='%s'",
          com_branch_c.br_no);
    if (ret != 0 && ret != 100)
    {
        sprintf(acErrMsg,"��ѯ��������������!![%d]",ret);
        WRITEMSG
        goto ErrExit;
    }
    else if (ret == 0)
    {
        sprintf(acErrMsg,"��������������Ѿ����ڸü�¼!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"O125");
        goto ErrExit;
    }

TRACE
    /* ����ϼ������Ƿ���� */
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

TRACE
    strcpy(com_branch_c.wrk_sts,"0");
    com_branch_c.city_no = 0;
    strcpy(com_branch_c.spe_ind,"0");

TRACE
    ret = Com_branch_Ins(com_branch_c, g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"�򹫹������������Ӽ�¼����!![%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    strcpy(g_pub_tx.ac_no,"");
    strcpy(g_pub_tx.brf,"����Ӫҵ����");
    if (pub_ins_trace_log())
    {
        sprintf(acErrMsg,"�Ǽǽ�����ˮ����!!");
        WRITEMSG
        goto ErrExit;
    }

    /* ���ӻ�ƻ��� */
    

       
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"����Ӫҵ�����ɹ� reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"����Ӫҵ����ʧ�� reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
