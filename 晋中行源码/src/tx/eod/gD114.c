/*************************************************
* �� �� ��:  gD114.c
* ���������� ���ֵǼǲ�������
*
* ��    ��:  rob
* ������ڣ� 2003��11��19��
*
* �޸ļ�¼��
* ��   ��:  
* �� �� ��: 
* �޸�����:
*************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"

gD114()
{
	int ret = 0;
	
	memset(&g_pub_tx,0x00,sizeof(g_pub_tx));

	sql_begin(); /*������*/

	/** ��ʼ��ȫ�ֱ��� **/
	pub_base_sysinit();



	ret = sql_execute("delete from mo_discnt where sts='1' and pn_type!='4'");
	if ( ret )
	{
		sprintf(acErrMsg, "�������ֵǼǲ�(¼��)����!![%d]", ret);
		WRITEMSG
		goto ErrExit;
	}

OkExit:
	sql_commit();	/*--�ύ����--*/
        strcpy(g_pub_tx.reply,"0000");
        sprintf(acErrMsg,"�������ֵǼǲ�(¼��)�ɹ�![%s]",g_pub_tx.reply);
        WRITEMSG
        set_zd_data("0120",g_pub_tx.reply);
        return 0;
ErrExit:
	sql_rollback(); /*--����ع�--*/
        sprintf(acErrMsg,"�������ֵǼǲ�(¼��)ʧ��![%s]",g_pub_tx.reply);
        WRITEMSG
        set_zd_data("0120",g_pub_tx.reply);
        return 1;
}
