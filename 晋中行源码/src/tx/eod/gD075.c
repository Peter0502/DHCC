/*************************************************************
* �� �� ��: gD075.c
* �������������ռ����Ѻ��ȷ��
*
* ��    ��: jack
* �������: 
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
**************************************************************/
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"

gD075()
{
	int ret = 0;

	struct dd_mst_c sDd_mst;
	struct td_mst_c sTd_mst;
	struct ln_mst_c sLn_mst;
	struct in_mst_c sIn_mst;
	struct od_mst_c sOd_mst;
	struct mdm_ac_rel_c sMdm_ac_rel;

	ret=sql_begin(); /*������*/
	if( ret ) 
	{
		sprintf( acErrMsg, "������ʧ��!!!" );
		WRITEMSG
		goto ErrExit;
	}

    /**------- ��ʼ��ȫ�ֱ��� --------**/
	pub_base_sysinit();

	/*--- �������ļ� ---*/
	ret = Dd_mst_Dec_Sel( g_pub_tx.reply , " 1=1" );

	while (1)
	{
		memset( &sDd_mst,0x00,sizeof(sDd_mst) );
		ret = Dd_mst_Fet_Sel( &sDd_mst, g_pub_tx.reply );
		if ( ret == 100 )
		{
			break;
		}
		if ( ret )
		{
			sprintf(acErrMsg," ������Ѻ��֤ʧ��!!![%ld],[%d]", sDd_mst.ac_id, sDd_mst.ac_seqn );
			WRITEMSG
			goto ErrExit;
		}
	}

	ret = Dd_mst_Clo_Sel();

	/*--- ���� ---*/
	ret = Td_mst_Dec_Sel( g_pub_tx.reply , " 1=1" );

	while (1)
	{
		memset( &sTd_mst,0x00,sizeof(sTd_mst) );
		ret = Td_mst_Fet_Sel( &sTd_mst, g_pub_tx.reply );
		if ( ret == 100 )
		{
			break;
		}
		if ( ret )
		{
			sprintf(acErrMsg," ������Ѻ��֤ʧ��!!![%ld],[%d]", sTd_mst.ac_id, sTd_mst.ac_seqn );
			WRITEMSG
			goto ErrExit;
		}
	}

	ret = Td_mst_Clo_Sel();

	/*--- ���� ---*/
	ret = Ln_mst_Dec_Sel( g_pub_tx.reply , " 1=1" );

	while (1)
	{
		memset( &sLn_mst,0x00,sizeof(sLn_mst) );
		ret = Ln_mst_Fet_Sel( &sLn_mst, g_pub_tx.reply );
		if ( ret == 100 )
		{
			break;
		}
		if ( ret )
		{
			sprintf(acErrMsg," ������Ѻ��֤ʧ��!!![%ld],[%d]", sLn_mst.ac_id, sLn_mst.ac_seqn );
			WRITEMSG
			goto ErrExit;
		}
	}

	ret = Ln_mst_Clo_Sel();

	/*--- �ڲ��� ---*/
	ret = In_mst_Dec_Sel( g_pub_tx.reply , " 1=1" );

	while (1)
	{
		memset( &sIn_mst,0x00,sizeof(sIn_mst) );
		ret = In_mst_Fet_Sel( &sIn_mst, g_pub_tx.reply );
		if ( ret == 100 )
		{
			break;
		}
		if ( ret )
		{
			sprintf(acErrMsg," �ڲ�����Ѻ��֤ʧ��!!![%ld],[%d]", sIn_mst.ac_id, sIn_mst.ac_seqn );
			WRITEMSG
			goto ErrExit;
		}
	}

	ret = In_mst_Clo_Sel();

	/*--- ͸֧ ---*/
	ret = Od_mst_Dec_Sel( g_pub_tx.reply , " 1=1" );

	while (1)
	{
		memset( &sOd_mst,0x00,sizeof(sOd_mst) );
		ret = Od_mst_Fet_Sel( &sOd_mst, g_pub_tx.reply );
		if ( ret == 100 )
		{
			break;
		}
		if ( ret )
		{
			sprintf(acErrMsg," ͸֧��Ѻ��֤ʧ��!!![%ld],[%d]", sOd_mst.ac_id, sOd_mst.ac_seqn );
			WRITEMSG
			goto ErrExit;
		}
	}

	ret = Od_mst_Clo_Sel();

	/*--- �����˻����ձ� ---*/
	ret = Mdm_ac_rel_Dec_Sel( g_pub_tx.reply , " 1=1" );

	while (1)
	{
		memset( &sMdm_ac_rel,0x00,sizeof(sMdm_ac_rel) );
		ret = Mdm_ac_rel_Fet_Sel( &sMdm_ac_rel, g_pub_tx.reply );
		if ( ret == 100 )
		{
			break;
		}
		if ( ret )
		{
			sprintf(acErrMsg," �����˻���ϵ����Ѻ��֤ʧ��!!![%ld],[%d]", sMdm_ac_rel.ac_id, sMdm_ac_rel.ac_seqn );
			WRITEMSG
			goto ErrExit;
		}
	}

	ret = Mdm_ac_rel_Clo_Sel();

OkExit:
	sql_commit();	/*--�ύ����--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"��֤��Ѻ�ɹ����!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--����ع�--*/
	sprintf(acErrMsg,"��֤��Ѻʧ��!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
