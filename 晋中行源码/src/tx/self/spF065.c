/*************************************************
* �� �� ��:  spF065.c
* ���������� �����ն�ת�ʷ���
*
* ��    ��:  jack
* ������ڣ� 2003��6��9��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"

int spF065()
{
	int ret = 0;
	char cAcno[20];
	char cAcno1[20];
	double dTram = 0.00;

	struct mdm_ac_rel_c sMdm_ac_rel;
	struct dd_mst_c sDd_mst;

	memset( cAcno, 0x0, sizeof(cAcno) );
	memset( cAcno1, 0x0, sizeof(cAcno1) );
	memset( &sMdm_ac_rel, 0x0, sizeof(sMdm_ac_rel) );
	memset( &sDd_mst, 0x0, sizeof(sDd_mst) );

	get_zd_data( "0370", cAcno );
	pub_base_old_acno( cAcno );
	set_zd_data( "0370", cAcno );

	get_zd_data( "0300", cAcno1 );
	pub_base_old_acno( cAcno1 );
	set_zd_data( "0300", cAcno1 );

	ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdm_ac_rel, \
		"ac_no = '%s'", cAcno );
	if ( ret )
	{
		sprintf(acErrMsg,"----->��ѯ�����˺Ŷ��ʱ����![%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	ret = Dd_mst_Sel( g_pub_tx.reply, &sDd_mst, "ac_id = %ld", \
			sMdm_ac_rel.ac_id );
	if ( ret )
	{
		sprintf(acErrMsg,"----->��ѯ�������ļ�����![%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	get_zd_double( "0400", &dTram );
	sDd_mst.bal = sDd_mst.bal - dTram;

	set_zd_double( "0420", sDd_mst.bal );
	set_zd_data( "0250", sDd_mst.name );

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"----->�����Ϣ�ɹ�" );
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;

ErrExit:
	sprintf(acErrMsg,"----->�����Ϣʧ��[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

