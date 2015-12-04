/*************************************************
* �� �� ��: sp9964.c
* ���������������˺Ų�ѯ���ͻ�����(���ڻ����ڲ���)
*
* ��    ��: jack
* ������ڣ�2003��7��15��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"

int sp9964()
{
	int ret=0;

	char cAcno[20];
	long lCif_no = 0;
	char cName[50];

	struct mdm_ac_rel_c sMdm_ac_rel;
	struct in_mst_c sIn_mst;

	memset( cAcno, 0x0, sizeof(cAcno) );
	memset( cName, 0x0, sizeof(cName) );

	get_zd_data( "0300", cAcno );
  pub_base_old_acno(cAcno);
	if ( cAcno[0] != '9' )
	{
		ret = pub_base_acgetname( cAcno, &lCif_no, cName );
		if ( ret )
		{
			sprintf( acErrMsg, "��ѯ������ !!!" );
			WRITEMSG
			goto ErrExit;
		}
		set_zd_data( "0250", cName );
	}
	else
	{
		memset( &sMdm_ac_rel, 0x0, sizeof(sMdm_ac_rel) );
		ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdm_ac_rel, \
			"ac_no = '%s'", cAcno );
		if ( ret )
		{
			sprintf( acErrMsg, "ִ��Mdm_ac_rel_Sel��[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}

		memset( &sIn_mst, 0x0, sizeof(sIn_mst) );
		ret = In_mst_Sel( g_pub_tx.reply, &sIn_mst, \
			"ac_id = %ld", sMdm_ac_rel.ac_id );
		if ( ret )
		{
			sprintf( acErrMsg, "ִ��In_mst_Sel��[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}

		set_zd_data( "0250", sIn_mst.name );
	}

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK !!!! " );
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
