/***************************************************************
* �� �� ��: spD308.c
* ��������: �ж��Ƿ��ǲ����ʺ�
*
* ��    ��: mike
* �������: 2004��1��16��
*
* �޸ļ�¼��
*    ��    ��:
*    �� �� ��:
*    �޸�����:
*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "mo_city_c.h"
#include "com_parm_c.h"

spD308()
{
	int ret = 0;

	struct com_parm_c sCom_parm;

	pub_base_sysinit();

	get_zd_data("1011", g_pub_tx.ac_no);
	sprintf(acErrMsg, "---------�ʺ�:[%s]", g_pub_tx.ac_no);
	WRITEMSG
    ret=Com_parm_Sel(g_pub_tx.reply,&sCom_parm,"parm_code='CZZH' and \
        val='%s'", g_pub_tx.ac_no);
    if (ret != 0 && ret != 100)
    {
        sprintf(acErrMsg,"��ѯ�������������!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }
	else if ( ret == 0 )
	{
		sprintf(acErrMsg, "���ʺŲ���������!![%s]", g_pub_tx.ac_no);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D261");
		/* set_zd_data("0130", "���ʺŲ������˽���,��[6400]�еĽ���"); */
		goto ErrExit;
	}

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"���ʺŲ��ǲ����ʺ�[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"���ʺ��ǲ����ʺ�[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

