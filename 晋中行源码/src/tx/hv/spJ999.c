/*************************************************
* �� �� ��:  spJ999.c
* ���������� ������� �Ǽ����� �ӽ���
*			       ���� �Ǽ����ʱ�
*			 
* ��    ��:  ChenMing
* ������ڣ� 2006��8��17��
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

int spJ999()
{
	char chkcmtno[4];
	char txtime[7];	
	
	vtcp_log(" --- ������� �Ǽ����� ��ʼ --- ");
	pub_base_sysinit();
	
	memset(chkcmtno,'\0',sizeof(chkcmtno));
	memset(txtime,'\0',sizeof(txtime));
	
  get_zd_data("0510",chkcmtno);	
  get_zd_data("0060",txtime);	

	vtcp_log(" --- chkcmtno =[%s] ",chkcmtno);	
	vtcp_log(" --- txtime =[%s] ",txtime);	 
 

	if (pub_hv_recv_reg_main(chkcmtno)) 
	  goto ErrExit; /* ���� ������ʵǼ����� ������ */
	
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

