/*************************************************
* �� �� ��:	spJ312.c
* ��������������Ӧ�������ӽ���
* ��    ��: jane
* �������: 2004��6��25��
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

spJ312()
{
	if (!strcmp(mbfetran.cancelresult,"0"))	 /*����Ӧ��Ϊ����*/
	{
		g_reply_int=sql_execute("update mbfeghvsendlist set procstate='30' where txssno='%s' and consigndate='%s'",
					mbfetran.origtxssno,mbfetran.consigndate);
		if (g_reply_int)
		{
			sprintf(acErrMsg,"�޸�ԭ������˴���״̬���� [%d]",g_reply_int);
			WRITEMSG
			goto ErrExit;
		}
	}

	g_reply_int=sql_execute("update mbfeoprtcancel set cancelresult='%s',resultstrinfo='%s',procstate='00' where cancelno='%s' and consigndate='%s'",
				mbfetran.cancelresult,mbfetran.strinfo,
				mbfetran.origrtnappno,mbfetran.consigndate);
	if (g_reply_int)
	{
		sprintf(acErrMsg,"�޸�ҵ�����Ǽǲ����� [%d]",g_reply_int);
		WRITEMSG
		goto ErrExit;
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
