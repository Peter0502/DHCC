/*************************************************
* �� �� ��:  sp9204.c
* ���������� ����ƾ֤����
* ��    ��:  YW
* ������ڣ�
*
* �޸ļ�¼��
* ��   ��:   20111206
* �� �� ��:
* �޸�����:
* ���ױ����ݣ�
------------------------------
insert into tx_def values('9204', '����ƾ֤����','10000000000000000000000000000000000000000000000000000000000100000000000000000000000000000000000000000000000000000000000000000000','0', '4', '0', '3', '');
insert into tx_flow_def values('9204', '0', '9204', '#$');
insert into tx_sub_def values('9204','����ƾ֤����', 'sp9204','0','0');
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"


int sp9204()
{
	long lBeg_note_no=0.00;
	long lEnd_note_no=0.00;
	long lSum_cnt=0.00;

	vtcp_log("����9204����\n");
	get_zd_long("0580", &lBeg_note_no);
	get_zd_long("0590", &lEnd_note_no);

	lSum_cnt=lEnd_note_no-lBeg_note_no+1;
	vtcp_log("��ʼƾ֤��[%ld],��ֹƾ֤��[%ld],ƾ֤����:[%ld]\n",lBeg_note_no,lEnd_note_no,lSum_cnt);

	set_zd_long("0600", lSum_cnt);

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
