/***************************************************************
* �� �� ��:     pubf_base_card.c
* ����������    �����׵Ĺ�������
* ���ú���˵����
* ��    ��:     ChenMing
* ������ڣ�    2007��01��18��
**************************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "seqctl_c.h"

/*************************************************
* �� �� ��:  iGetPreAuth_Traceno
* ���������� ����Ԥ��Ȩ��
*            
* ��    ��:  ChenMing
* ������ڣ� 2007-1-17 
*************************************************/
long iGetPreAuth_Traceno()
{
	long lTrace_no;
	int ret=0;
	struct seqctl_c seqctl_c;
	memset(&seqctl_c, 0x00 , sizeof(seqctl_c));
	ret = Seqctl_Dec_Upd(g_pub_tx.reply," br_no='%s' and ac_ind='11'", TOT_BR_NO);
	if (ret)
	{
		sprintf(acErrMsg,"ִ��Seqctl_Dec_Upd��[%d]",ret);
		WRITEMSG
		goto ErrExit;
	}
	ret = Seqctl_Fet_Upd(&seqctl_c, g_pub_tx.reply);
	if (ret==100)
	{
		sprintf(acErrMsg,"ִ��Seqctl_Fet_Upd��ѯ���֧��������ű����![%d]",ret);
		WRITEMSG
		goto ErrExit;
	}
	else if (ret)
	{
		sprintf(acErrMsg,"ִ��seqctl_Fet_Upd ��[%d]",ret);
		WRITEMSG
		goto ErrExit;
	}
	lTrace_no = seqctl_c.seqno;
	if (seqctl_c.seqno==999999) seqctl_c.seqno=0;
	seqctl_c.seqno++;
	ret = Seqctl_Upd_Upd(seqctl_c, g_pub_tx.reply);
	if (ret!=0)
	{
		sprintf(acErrMsg,"�޸Ļ����������!");
		WRITEMSG
		goto ErrExit;
	}
	ret=Seqctl_Clo_Upd();
	
	OkExit:
	    vtcp_log("ȡ��֧���������=[%ld]�ɹ�![%s]", lTrace_no, g_pub_tx.reply);
	    return lTrace_no;
	ErrExit:
   	    vtcp_log("ȡ��֧���������ʧ��![%s]", g_pub_tx.reply);
		return -1;
}



