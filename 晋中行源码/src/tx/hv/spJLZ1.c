/*************************************************
* �� �� ��:	spJLZ1
* �������������ҵ�����ʹ��˻�������ù��˿�Ŀ
* ��    ��: LiuHuafeng
* �������: 2006-8-30 18:24
* �޸ļ�¼�� 
*   ��    ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#define	EXTERN
#include <string.h>
#include "public.h"
#include "hv_pub.h"
#include "hv_define.h"
#include "find_debug.h"

spJLZ1()
{
	char cAc_no[22];
	char cCheckStat[2];
	int iRet=0;
	char cKinbr[BRNO_LEN+1];
	memset(cKinbr, 0 , sizeof(cKinbr));
	memset(cAc_no , 0 , sizeof(cAc_no));
	memset(cCheckStat, 0 , sizeof(cCheckStat));
	get_zd_data("0980",cCheckStat);
	if(memcmp(g_pub_tx.reply,"0000",4))
	{
		/* ������һ����������ʧ��,����Ҫ������� */
		sql_rollback();
	}
	else
	{
		/* ��麯���������Ĺ�װ״̬ */
		sql_rollback();
	}

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
        if(memcmp(g_pub_tx.reply,"0000",4)==0)
                MEMCPY_DEBUG(g_pub_tx.reply,"T063",4);
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
