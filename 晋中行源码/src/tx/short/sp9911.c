/*************************************************
* �� �� ��:  sp9911.c
* ���������� ��ѯ��λ�����Ϣ
*
* ��    ��:  jack
* ������ڣ� 2003��5��26��
*
* �޸ļ�¼�� 
* ��   ��:
* �� �� ��:
* �޸�����:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "ag_unitinfo_c.h"

int sp9911()
{
	struct ag_unitinfo_c sUnitInfo;	/*��λ��Ϣ��*/
	int ret = 0;
	char cAc_no[20];
	char cUnit_no[5];
	memset( cAc_no, 0x0, sizeof(cAc_no) );
	memset(cUnit_no,0x00,sizeof(cUnit_no));
	memset(&sUnitInfo,0x00,sizeof(struct ag_unitinfo_c));
	pub_base_sysinit();
	get_zd_data( "0300", cAc_no);
  get_zd_data( "0920",cUnit_no);
  
	ret=Ag_unitinfo_Sel( g_pub_tx.reply , &sUnitInfo ,\
			 "unit_no='%s'",cUnit_no);
	if( ret==100 )
	{
		sprintf( acErrMsg, "û�в�ѯ���õ�λ��Ŷ�Ӧ��¼!\
			unit_no = [%s]", cUnit_no );
		WRITEMSG
		strcpy( g_pub_tx.reply, "H009" );
		goto	ErrExit;
	}
	else if( ret )
	{
		sprintf( acErrMsg, "ִ��Ag_unitinfo����!ret=[%d]", ret );
		WRITEMSG
		goto	ErrExit;
	}

	set_zd_data( "0250",sUnitInfo.unit_name);
	set_zd_data( "0300",sUnitInfo.bank_acct_no);
	set_zd_data( "0710",sUnitInfo.opr_attri);
	set_zd_data( "0210",sUnitInfo.pay_type);
	set_zd_data( "0920",sUnitInfo.unit_no);
	set_zd_data( "0720",sUnitInfo.auto_flag);
	set_zd_data( "0350",sUnitInfo.up_unit);
	set_zd_data( "0910",sUnitInfo.opn_br_no);
	set_zd_data( "0490",sUnitInfo.opn_tel);
	set_zd_data( "0670",sUnitInfo.unit_stat);

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
