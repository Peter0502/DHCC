/*************************************************
* �� �� ��:  sp9862.c
* ���������� ���ݺ�ͬ�Ų�ѯ�������ļ�
*
* ��    ��:  rob
* ������ڣ� 2003��3��05�� 
*
* �޸ļ�¼�� 
* ��   ��:
* �� �� ��:
* �޸�����:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "ass_mst_c.h"

int sp9862()
{
    int ret;
    char flag[2];
    struct ass_mst_c ass_mst;
 
    memset(&ass_mst,0x0,sizeof(struct ass_mst_c));

    get_zd_long("0470",&ass_mst.pact_no);
    get_zd_data("0670",flag);

    ret = Ass_mst_Sel(g_pub_tx.reply,&ass_mst,"pact_no=%ld",ass_mst.pact_no);
    if (ret != 0 && ret != 100)
    {
        sprintf(acErrMsg,"��ѯ�������ļ�����!! [%d]",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"O104");
        goto ErrExit;
    }
    else if (ret == 100)
    {
        sprintf(acErrMsg,"û�иú�ͬ�ŵ�ϵͳ�ڲ����!!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"O111");
        goto ErrExit;
    }
    if (ass_mst.sts[0] == '*')
    {
        sprintf(acErrMsg,"�ú�ͬ�ŵ��ڲ�����Ѿ�����!!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"O112");
        goto ErrExit;
    }
    if (ass_mst.sign[0] != flag[0])
    {
        sprintf(acErrMsg,"����־����!!!");
        WRITEMSG
        if (ass_mst.sign[0] == '1')
        {
            strcpy(g_pub_tx.reply,"O114");
        }
        else
        {
            strcpy(g_pub_tx.reply,"O113");
        }
        goto ErrExit;
    }

    set_zd_data("0910",ass_mst.sub_br_no);
    set_zd_double("0390",ass_mst.bal);
    char s_bal[30];
	  memset( s_bal, 0x00, sizeof(s_bal) );
	  sprintf( s_bal, "%.2f", ass_mst.bal);
	  set_zd_data("0260",s_bal);/* �޸�֧�ָ��� martin 2008/12/26 10:32:40 */
    set_zd_long("0440",ass_mst.end_date);
OkExit:
    strcpy( g_pub_tx.reply, "0000" );
    sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
ErrExit:
    sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
