/*************************************************
* �� �� ��: spH673.c
* ��������������(����)����ǰ����
*
* ��    ��: mike
* ������ڣ�2004��06��13��
*
* �޸ļ�¼�� 
* ��    ��:
* �� �� ��:
* �޸�����:
*************************************************/

#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "ag_paytlcs_c.h"
#include "ag_itemlist_c.h"

int spH673()
{
	int ret = 0;
	int i = 0;
	char tmpstr[32];

	char cUnit_no[5];
	char cItem_no[21];
	char cTrace_no[16];
	double dTram = 0.00;

	struct ag_paytlcs_c sAg_paytlcs;

	if (pub_base_sysinit())
	{
		sprintf(acErrMsg,"��ʼ�����ýṹ����!!");
		WRITEMSG
		goto ErrExit;
	}

	memset( cUnit_no, 0x0, sizeof(cUnit_no) );
	memset( cItem_no, 0x0, sizeof(cItem_no) );
	memset( cTrace_no, 0x0, sizeof(cTrace_no) );

	memset( &sAg_paytlcs, 0x00, sizeof(sAg_paytlcs) );

	get_zd_data( "0920", cUnit_no );
	get_zd_data( "0610", cTrace_no );
	get_zd_data( "0630", cItem_no );
	get_zd_double( "0400", &dTram );
	
	sprintf( acErrMsg,"---->��������Ϊunit_no = '%s' and bank_tlcs = '%s' and item_no = '%s' \
			and trn_amt = %f", cUnit_no, cTrace_no, cItem_no, dTram );
	WRITEMSG

	ret = Ag_paytlcs_Sel( g_pub_tx.reply, &sAg_paytlcs, \
			"unit_no = '%s' and bank_tlcs = '%s' and item_no = '%s' \
			and trn_amt = %f", cUnit_no, cTrace_no, cItem_no, dTram );
	if ( ret == 100 )
	{
		sprintf( acErrMsg,"Ҫ�������ļ�¼������!!!" );
		WRITEMSG
		goto ErrExit;
	}
	else if ( ret )
	{
		sprintf( acErrMsg,"����Ҫ�������ļ�¼����!!!" );
		WRITEMSG
		goto ErrExit;
	}

	ret = sql_execute( "update ag_paytlcs set stat = '1' where \
			unit_no = '%s' and bank_tlcs = '%s' and item_no = '%s' \
			and trn_amt = %f", cUnit_no, cTrace_no, cItem_no, dTram );
	if ( ret )
	{
		strcpy( g_pub_tx.reply, "H020" );
		sprintf( acErrMsg,"�޸ı������ļ�¼״̬��!!!" );
		WRITEMSG
		goto ErrExit;
	}

	ret = sql_execute( "delete from ag_itemlist where \
			unit_no = '%s' and bank_tlcs = '%s' and item_no = '%s' \
			and am30 = %f", cUnit_no, cTrace_no, cItem_no, dTram );
	if ( ret )
	{
		strcpy( g_pub_tx.reply, "H020" );
		sprintf( acErrMsg, "ɾ���ɷ���ϸ����!!!" );
		WRITEMSG
		goto ErrExit;
	}

OkExit:
	sprintf( acErrMsg,"�ɷѳ���ǰ����ɹ�!!!" );
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"�ɷѳ���ǰ����ʧ��[%s]", g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
