/***************************************************************
* �� �� ��: spF626.c
* ���������������˰ҵ������¼����
*
* ��    ��: jack
* ������ڣ�2004��06��26��
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��: 
* �޸�����: 
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "pay_agent_c.h"
#include "ag_paytlcs_c.h"
#include "ag_unitinfo_c.h"

spF626()
{
	int ret = 0;

	char cItem_no[21];
	char cUnit_no[5];
	char cTrace_no[13];
	double dTram = 0.00;

	struct ag_unitinfo_c sAg_unitinfo;	/*��λ��Ϣ*/
	struct ag_paytlcs_c sAg_paytlcs;   /*�ɷ���ˮ��*/

	memset(cItem_no,0x0,sizeof(cItem_no));
	memset(cUnit_no,0x0,sizeof(cUnit_no));
	memset( cTrace_no, 0x0, sizeof(cTrace_no) );

	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();

	get_zd_data( "0920", cUnit_no );
	get_zd_data( "0630", cItem_no );
	get_zd_data( "0610", cTrace_no );
	get_zd_double( "0400", &dTram );

	memset( &sAg_unitinfo, 0x0, sizeof(sAg_unitinfo) );
	ret = Ag_unitinfo_Sel( g_pub_tx.reply, &sAg_unitinfo, \
			"unit_no = '%s' and unit_stat = '0'", cUnit_no );
	if ( ret )
	{
		sprintf( acErrMsg, "��ѯ����λ��Ϣ����[%s]!!!", cUnit_no );
		WRITEMSG
		goto ErrExit;
	}

	memset( &sAg_paytlcs, 0x0, sizeof(sAg_paytlcs) );
	ret = Ag_paytlcs_Sel( g_pub_tx.reply, &sAg_paytlcs, \
			"unit_no = '%s' and item_no = '%s' and bank_tlcs = '%s' \
			and trn_amt = %f and stat = '0'", cUnit_no, cItem_no, \
			cTrace_no, dTram );
	if ( ret )
	{
		sprintf( acErrMsg, "--------->Ҫ�����ļ�¼����!!![%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	ret = sql_execute( "update pay_agent set sata = '1', result = '1' \
			where unit_no = '%s' and item_no = '%s' and \
			result = '0'", cUnit_no, cItem_no );
	if ( ret )
	{
		sprintf(acErrMsg,"----->�޸�δ�ɼ�¼״̬ʧ��![%s]", ret);
		WRITEMSG
		goto ErrExit;
	}

	ret = sql_execute( "update ag_paytlcs set stat = '1' where \
			unit_no = '%s' and item_no = '%s' and bank_tlcs = '%s' \
			and trn_amt = %f and stat = '0'", cUnit_no, cItem_no, \
			cTrace_no, dTram );
	if ( ret )
	{
		sprintf(acErrMsg,"---->�޸Ĵ�������ҵ����ˮ״̬ʧ��![%s]", ret);
		WRITEMSG
		goto ErrExit;
	}

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf( acErrMsg, "------>�����ɷѼ�¼����ɹ�!!!!" );
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"------>�����ɷѼ�¼����ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
