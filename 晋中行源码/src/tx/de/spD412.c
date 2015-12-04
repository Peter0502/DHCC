/***************************************************************
* �� �� ��: spD412.c
* �����������̿�����
*
* ��    ��: jack
* ������ڣ�2004��03��23��
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "cif_id_code_rel_c.h"

spD412()
{
	int ret = 0;
	char cFlag[2];
	char cAc_no[20];
	char cId_type[2];
	char cId_no[21];

	struct mdm_ac_rel_c sMdm_ac_rel;
	struct dd_mst_c sDd_mst;
	struct cif_id_code_rel_c sCif_id_code_rel;

	memset( &sMdm_ac_rel, 0x00, sizeof(sMdm_ac_rel) );
	memset( &sDd_mst, 0x00, sizeof(sDd_mst) );
	memset( &sCif_id_code_rel, 0x00, sizeof(sCif_id_code_rel) );
	memset( cFlag, 0x0, sizeof(cFlag) );
	memset( cAc_no, 0x0, sizeof(cAc_no) );
	memset( cId_type, 0x0, sizeof(cId_type) );
	memset( cId_no, 0x0, sizeof(cId_no) );

	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();

	/*-------- ȡ�������  ---------*/
	get_zd_data( "0700", cFlag );
	get_zd_data( "0370", cAc_no );

	if ( cFlag[0] == '0' )
	{
		g_reply_int=Mdm_ac_rel_Dec_Upd(g_pub_tx.reply, "ac_no='%s'", cAc_no);
		if(g_reply_int)
		{
			sprintf(acErrMsg,"�����α����!");
			strcpy(g_pub_tx.reply, "P162");
			return -1;
		}

		g_reply_int=Mdm_ac_rel_Fet_Upd(&g_mdm_ac_rel,g_pub_tx.reply);
		if(g_reply_int&&g_reply_int!=100)
		{
			sprintf(acErrMsg,"ȡ��¼����!");
			strcpy(g_pub_tx.reply, "P162");
			return -1;
		}

		strcpy(g_mdm_ac_rel.coll_sts,"2");

		g_reply_int=Mdm_ac_rel_Upd_Upd(g_mdm_ac_rel,g_pub_tx.reply);
		if(g_reply_int)
		{
			sprintf(acErrMsg,"�޸ļ�¼����!");
			strcpy(g_pub_tx.reply, "P162");
			return -1;
		}

		Mdm_ac_rel_Clo_Upd( );
	}
	else if ( cFlag[0] == '1' )
	{
		get_zd_data( "0670", cId_type );
		get_zd_data( "0620", cId_no );

		ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdm_ac_rel, "ac_no = '%s'", cAc_no );
		if ( ret )
		{
			sprintf(acErrMsg,"��ѯ�����˺Ŷ��ձ����[%s]",g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}

		pub_base_strpack( sMdm_ac_rel.coll_sts );
		if ( sMdm_ac_rel.coll_sts[0] != '2' )
		{
			strcpy( g_pub_tx.reply, "A036" );
			sprintf( acErrMsg,"û����Ҫ������̿���¼" );
			WRITEMSG
			goto ErrExit;
		}

		pub_base_strpack( cId_type );
		pub_base_strpack( cId_no );
		
		ret = Dd_mst_Sel( g_pub_tx.reply, &sDd_mst, "ac_id = %ld", sMdm_ac_rel.ac_id );
		if ( ret )
		{
			sprintf(acErrMsg,"��ѯ�������ļ������[%s]",g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}
		
		ret = Cif_id_code_rel_Sel( g_pub_tx.reply, &sCif_id_code_rel, "cif_no = %ld", sDd_mst.cif_no );
		if ( ret )
		{
			sprintf( acErrMsg, "��ѯ�ͻ��ź�֤����Ӧ��ϵ�����[%s]", g_pub_tx.reply );
			WRITEMSG
			goto ErrExit;
		}
		
		pub_base_strpack( sCif_id_code_rel.id_type );
		pub_base_strpack( sCif_id_code_rel.id_no );
		
		if ( ( strcmp( sCif_id_code_rel.id_type, cId_type) != 0) \
			|| ( strcmp( sCif_id_code_rel.id_no, cId_no) != 0) )
		{
			sprintf(acErrMsg,"֤�����ͻ�֤�������[%s][%s][%s][%s]!!!",cId_type,sCif_id_code_rel.id_type,cId_no,sCif_id_code_rel.id_no);
			WRITEMSG
			strcpy( g_pub_tx.reply, "M004" );
			goto ErrExit;
		}

		g_reply_int=Mdm_ac_rel_Dec_Upd(g_pub_tx.reply, "ac_no='%s'", cAc_no);
		if(g_reply_int)
		{
			sprintf(acErrMsg,"�����α����!");
			strcpy(g_pub_tx.reply, "P162");
			return -1;
		}

		g_reply_int=Mdm_ac_rel_Fet_Upd(&g_mdm_ac_rel,g_pub_tx.reply);
		if(g_reply_int&&g_reply_int!=100)
		{
			sprintf(acErrMsg,"ȡ��¼����!");
			strcpy(g_pub_tx.reply, "P162");
			return -1;
		}

		strcpy(g_mdm_ac_rel.coll_sts,"0");

		g_reply_int=Mdm_ac_rel_Upd_Upd(g_mdm_ac_rel,g_pub_tx.reply);
		if(g_reply_int)
		{
			sprintf(acErrMsg,"�޸ļ�¼����!");
			strcpy(g_pub_tx.reply, "P162");
			return -1;
		}

		Mdm_ac_rel_Clo_Upd( );
	}
	else
	{
			sprintf(acErrMsg,"�������ʹ�!!!");
			WRITEMSG
			strcpy( g_pub_tx.reply, "D999" );
			goto ErrExit;
	}

	/*** ������� ***/
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"�̿�����ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"�̿�����ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
