/*************************************************
* �� �� ��: sp9300.c
* ������������ѯ��ԱǮ��״̬�Ƿ�����
*           �˹�Ա�˱���
* ��    ��: martin
* ������ڣ�20061010 
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

int sp9300()
{
	int ret=0;

	struct cash_mst_c sCash_mst;
	
	memset(&sCash_mst,0x00,sizeof(struct cash_mst_c));

	get_zd_data("0210",g_pub_tx.cur_no);				/*����*/
	get_zd_data("0920",g_pub_tx.tel);					/*����Ա*/

	ret = Cash_mst_Sel( g_pub_tx.reply, &sCash_mst, \
			" tel = %s and cur_no = %s and sts ='0' ", \
			g_pub_tx.tel, g_pub_tx.cur_no );
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
	sprintf( acErrMsg, "---->�ù�Ա�˱���Ǯ��״̬δ����[%s]", g_pub_tx.reply );
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
