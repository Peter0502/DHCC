/*************************************************
* �� �� ��: sp9994.c
* ������������ѯβ��״̬�Ƿ�������β���ֽ��Ƿ����
*
* ��    ��: jack
* ������ڣ�20031115 
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

int sp9994()
{
	int ret=0;

	double dTram = 0.00;

	struct cash_mst_c sCash_mst;

	pub_base_sysinit();

	get_zd_double( "0400", &dTram );

	ret = Cash_mst_Sel( g_pub_tx.reply, &sCash_mst, \
			"br_no = %s and tel = %s and sts = '0'", \
			g_pub_tx.tx_br_no, g_pub_tx.tel );
	if( ret )
	{
		strcpy( g_pub_tx.reply, "D189" );
		goto ErrExit;
	}	

	ret = 0;
	ret = pub_base_CompDblVal( dTram, sCash_mst.bal );
	if ( ret >= 0 )
	{
		strcpy( g_pub_tx.reply, "O263" );
		goto ErrExit;
	}

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf( acErrMsg, "--->��ѯ��Աβ��������[%s]", g_pub_tx.reply );
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
