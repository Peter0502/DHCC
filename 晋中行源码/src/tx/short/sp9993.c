/*************************************************
* �� �� ��: sp9993.c
* ������������ѯ��Աβ��״̬�Ƿ�����
*
* ��    ��: jack
* ������ڣ�20031114 
*
* �޸ļ�¼�� 
* ��   ��:
* �� �� ��:
* �޸�����:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "cash_mst_c.h"

int sp9993()
{
	int ret=0;

	struct cash_mst_c sCash_mst;

	pub_base_sysinit();

	ret = Cash_mst_Sel( g_pub_tx.reply, &sCash_mst, \
			"br_no = %s and tel = %s and sts = '0'", \
			g_pub_tx.tx_br_no, g_pub_tx.tel );
	if( ret )
	{
		strcpy( g_pub_tx.reply, "D189" );
		goto ErrExit;
	}	

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf( acErrMsg, "---->��ѯ��Աβ�����[%s]", g_pub_tx.reply );
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
