/*************************************************************
* �� �� ��: spF777.c
* ����������д������־
* ��    ��: jack
* ������ڣ�2003��2��23��
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "four_tradelog_c.h"

spF777()
{
	int ret = 0;
	char cAcno[21];

	struct four_tradelog_c sFour_Tradelog;
	struct mdm_ac_rel_c sMdm_ac_rel;
	struct dd_mst_c sDd_mst;

	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();

	memset( cAcno, 0x0, sizeof(cAcno) );
	get_zd_data( "0370", cAcno );

	memset( &sMdm_ac_rel, 0x0, sizeof(sMdm_ac_rel) );
	ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdm_ac_rel, \
		"ac_no = '%s'", cAcno );
	if ( ret == 100 )
	{
		goto aaaa;
	}
	else if ( ret )
	{
		sprintf( acErrMsg, "ִ��Mdm_ac_rel_Sel��[%s][%d]", cAcno, ret );
		WRITEMSG
		goto ErrExit;
	}

	memset( &sDd_mst, 0x0, sizeof(sDd_mst) );
	ret = Dd_mst_Sel( g_pub_tx.reply, &sDd_mst, \
		"ac_id = %ld", sMdm_ac_rel.ac_id );
	if ( ret == 100 )
	{
		goto aaaa;
	}
	else if ( ret )
	{
		sprintf( acErrMsg, "ִ��Dd_mst_Sel��[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

aaaa:

	set_zd_data( "0270", sDd_mst.name );
	set_zd_data( "0250", sDd_mst.name );
	set_zd_long( "0280", sDd_mst.cif_no );
	set_zd_double( "0420", sDd_mst.bal );

	memset(&sFour_Tradelog,0x0,sizeof(sFour_Tradelog));
	sFour_Tradelog.tx_date = g_pub_tx.tx_date;
	sFour_Tradelog.tx_time = g_pub_tx.tx_time;
	strcpy( sFour_Tradelog.tx_code , g_pub_tx.tx_code );
	/*get_zd_data( "0340", sFour_Tradelog.sl_bank );*/
	strcpy( sFour_Tradelog.sl_bank , g_pub_tx.tx_br_no );
	strcpy( sFour_Tradelog.tx_tel , g_pub_tx.tel );
	get_zd_data( "0360", sFour_Tradelog.city_no );
	get_zd_long( "0600", &sFour_Tradelog.chang_no );	
	strcpy(sFour_Tradelog.cur_no , "01");	
	sFour_Tradelog.trace_no = g_pub_tx.trace_no;
	get_zd_data("0370",sFour_Tradelog.ac_no);
	get_zd_data("0250",sFour_Tradelog.name);

	get_zd_double("0400",&sFour_Tradelog.tx_amt);
	get_zd_double("0410",&sFour_Tradelog.sx_amt);
	get_zd_double("0420",&sFour_Tradelog.bal);	
	get_zd_double("0840",&sFour_Tradelog.sx_rate);
	get_zd_data( "0920", sFour_Tradelog.cancle_trcode );
	get_zd_long( "0610", &sFour_Tradelog.cancle_trace_no );
	strcpy( sFour_Tradelog.old_tel, "  " );
	strcpy( sFour_Tradelog.sl_ckmk, "0" );
	strcpy( sFour_Tradelog.fk_ckmk, "0" );
	strcpy(sFour_Tradelog.tx_stat,"0");

	ret = Four_tradelog_Ins( sFour_Tradelog , g_pub_tx.reply );
	if (ret )
	{
		sprintf(acErrMsg,"Four_tradelog_Insʧ��![%s]",g_pub_tx.reply);
		WRITEMSG
		goto ErrExit;
	}


OKExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"д����ͨ����־�ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	strcpy(g_pub_tx.reply,"P156");
	sprintf(acErrMsg,"д����ͨ����־ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
