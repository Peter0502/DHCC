/*************************************************
* �� �� ��:  spJ098.c
* ���������� С������ ���ʼ�� �ӽ���
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
#include "lv_pub.h"
#include "lv_define.h"

int spJ098()
{
	long iSubpackIndex=0;
    char cTx_br_no[BRNO_LEN+1];
	pub_base_sysinit();
    
    memset(cTx_br_no,0,sizeof(cTx_br_no));
	get_zd_long("0520",&iSubpackIndex);
	vtcp_log("spJ098() С������ ���ʼ�� �ӽ��� started! [%ld]",iSubpackIndex);
	vLvBrnoDebug(__FILE__,__LINE__);
	g_reply_int=pubf_lv_recv_check_main();
	if(g_reply_int==9999)
	{
			vtcp_log("%s,%d �����ж��Ƿ���Խ�������"__FILE__,__LINE__);
			strncpy(g_pub_tx.reply,"P046",4);
			WRITEMSG
			goto ErrExit; 
	}
	else if (g_reply_int) /* ���� С�����ʹ��ʼ�� ������ */
	{
		/*���ʼ��ʧ��*/ 
		set_zd_data("0500","1");       /* ���Ĵ������ */ 
		strcpy( g_pub_tx.reply,"0060" );
		vLvBrnoDebug(__FILE__,__LINE__);
		goto ErrExit; 
	}
    get_zd_data("0030",cTx_br_no);
  iGetReceTel(cTx_br_no);    /*  ��Ա�� ==> 0070 */
	vLvBrnoDebug(__FILE__,__LINE__);
OkExit:
	get_zd_long("0520",&iSubpackIndex);
	vtcp_log("%s,%d spJ098() С������ ������� �ӽ��׽��� end! [%ld]",__FL__,iSubpackIndex);
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	get_zd_long("0520",&iSubpackIndex);
	vtcp_log("%s,%d spJ098() С������ ������� �ӽ��׽��� end! [%ld]",__FL__,iSubpackIndex);
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

