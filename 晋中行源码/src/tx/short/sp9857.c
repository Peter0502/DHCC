/*************************************************
* �� �� ��:  sp9857.c
* ���������� ���ݲ�Ʒ��Ų�ѯ��Ʒ����(�����Ʒ������)
*
* ��    ��:  jane
* ������ڣ� 2003��2��23�� 
*
* �޸ļ�¼�� 
* ��   ��:
* �� �� ��:
* �޸�����:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"

int sp9857()
{
    struct ln_parm_c sLn_parm;
    int ret;
   
    memset(&sLn_parm, 0x00, sizeof(struct ln_parm_c));
   
    get_zd_data("0230", sLn_parm.prdt_no);

    ret = Ln_parm_Sel(g_pub_tx.reply,&sLn_parm,"prdt_no = '%s'",
                      sLn_parm.prdt_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"��ѯ�����Ʒ���������!ret=[%d]",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"B110");
        goto ErrExit;
    }

    set_zd_data("0810",sLn_parm.title);
OkExit:
    strcpy( g_pub_tx.reply, "0000" );
    sprintf(acErrMsg,"success !!: reply is[%s]",g_pub_tx.reply);
    WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"fail !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

