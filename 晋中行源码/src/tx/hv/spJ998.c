/*************************************************
* �� �� ��:  spJ998.c
* ���������� ������� ���ʼ�� �ӽ���
*			       
*			 
* ��    ��:  ChenMing
* ������ڣ� 2006��8��30��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/
#define  EXTERN
#include <string.h>
#include <stdio.h>
#include "public.h"
#include "hv_pub.h"
#include "hv_define.h"
#include "find_debug.h"

int spJ998()
{

	char cStat[2];
	memset(cStat,0,sizeof(cStat));
	pub_base_sysinit();
	vtcp_log("spJ998() ������� ���ʼ�� �ӽ��� started! ");  
	if (pub_hv_recv_check_main())
	{
		vtcp_log("%s,%d pub_hv_recv_check_main ���ع��˴������",__FILE__,__LINE__);
		goto ErrExit; /* ���� ������ʹ��ʼ�� ������ */
	}
	get_zd_data("0980",cStat);
	vtcp_log("[%s][%d]  cStat=====[%s]",__FILE__,__LINE__,cStat);

	/* strcpy( g_pub_tx.reply,"0000" ); */


OkExit:
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

