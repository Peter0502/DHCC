	/*************************************************
* �� �� ��:  sp9841.c
* ���������� ��ѯ��λ�����Ϣ
*
* ��    ��:  jack
* ������ڣ� 2003��1��30��
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
#include "ag_qs_gd_rel_c.h"

int sp9841()
{
	struct ag_unitinfo_c sUnitInfo;	/*��λ��Ϣ��*/
	struct ag_qs_gd_rel_c sAg_qs_gd_rel;/* �����˻�������˻������� */
	int ret = 0;
	char cUnit_no[5];
	char cFlag[2];

	memset(cUnit_no,0,sizeof(cUnit_no));
	memset(cFlag,0x0,sizeof(cFlag));

	pub_base_sysinit();
	/*get_zd_data( "0360",cUnit_no);*/
	get_zd_data( "0920", cUnit_no);
	get_zd_data( "0700", cFlag);

	if ( cFlag[0] == '0' )
	{
		goto OkExit;
	}

	memset( &sUnitInfo, 0x00, sizeof(sUnitInfo) );
	ret=Ag_unitinfo_Sel( g_pub_tx.reply , &sUnitInfo ,\
			 "unit_no = '%s' and unit_stat = '0'",cUnit_no);
	if( ret==100 )
	{
		sprintf( acErrMsg, "û�в�ѯ���õ�λ��Ŷ�Ӧ��¼!\
			unit_no=[%s]", cUnit_no );
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
/*
 ret = pub_ins_trace_log();
    if ( ret )
    {
        sprintf( acErrMsg, "����ˮ��־��" );
        WRITEMSG
        goto ErrExit;
    }
*/
	/**���ݲ���Ҫ����ϷǱ������Ĵ�����λ ֱ�ӷ��ش��� ���ü���ִ�в��� add by wudawei 20130424 **/
	if ( strcmp( g_pub_tx.tx_br_no, sUnitInfo.opn_br_no ) )
	{
		sprintf( acErrMsg, "�Ǵ���λ��������,����[%s]������Ա���˽���", sUnitInfo.opn_br_no );
		WRITEMSG
		sprintf( acErrMsg, "����[%s]������Ա���˽���", sUnitInfo.opn_br_no );
		set_zd_data( "0130", acErrMsg );
		strcpy( g_pub_tx.reply, "H034" );
		set_zd_data( DC_REPLY, g_pub_tx.reply );
   		return 1;
	}
	/** add end 20130424 **/
	/** Ϊ���������ag_qs_gd_rel�������˻��͹����˻������� ��ѯ��λ����ʱ �������˻����� add by wudawei 20130820 **/
	memset( &sAg_qs_gd_rel, 0x00, sizeof(sAg_qs_gd_rel) );
	ret = Ag_qs_gd_rel_Sel( g_pub_tx.reply , &sAg_qs_gd_rel ,\
			 "unit_no = '%s' ",cUnit_no);
	if( ret==100 )
	{
		sprintf( acErrMsg, "û�в�ѯ�������˻��͹����˻�������Ӧ��¼!\
			unit_no=[%s]", cUnit_no );
		WRITEMSG
		
	}
	else if( ret )
	{
		sprintf( acErrMsg, "ִ��sAg_qs_gd_rel��ѯ����!ret=[%d]", ret );
		WRITEMSG
		
	}
	else if( ret==0 )
	{
		set_zd_data( "0310", sAg_qs_gd_rel.qs_ac_no );
	}
	/** add end 20130820 **/
	set_zd_data( "0250", sUnitInfo.unit_name );
	set_zd_data( "0710", sUnitInfo.opr_attri );
	set_zd_data( "0210", sUnitInfo.pay_type );
	set_zd_data( "0300", sUnitInfo.bank_acct_no );
	set_zd_data( "0720", sUnitInfo.auto_flag );
	set_zd_data( "0670", sUnitInfo.clr_flag );
	set_zd_data( "0350", sUnitInfo.up_unit );
	
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
