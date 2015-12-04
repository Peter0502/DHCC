/*************************************************
* �� �� ��:  
* ���������� 
*
* ��    ��:  
* ������ڣ� 
*
* �޸ļ�¼�� 
* ��   ��:
* �� �� ��:
* �޸�����:
*************************************************/

#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"

int sp8899()
{
	char cFileName[50];			/* ��ʱ�ļ��� */
	int	 ret = 0;
	FILE *fp;

	char cAc_sts[11];

	struct dd_mst_c	sDd_mst;
	struct mdm_ac_rel_c sMdm_ac_rel;

	memset( cFileName, 0x00, sizeof( cFileName ) );
	
	/* ���ݳ�ʼ�� */
	pub_base_sysinit();

	/* ���ݹ�Ա�š��ն˺�������ʱ�ļ� */
	ret = pub_base_AllDwnFilName( cFileName ); 
	if( ret )
	{
		sprintf( acErrMsg, "�����´�ȫ·���ļ���(����)��" );
		WRITEMSG
		goto ErrExit;
	}

	fp = fopen( cFileName, "w" );
	if( fp==NULL )
	{
		sprintf( acErrMsg, "��ʱ�ļ�����!" );
		WRITEMSG
		sprintf( g_pub_tx.reply, "B080" );
		goto ErrExit;
	}

	ret = Dd_mst_Dec_Sel( g_pub_tx.reply , \
			"ac_type in ('1', '3', '4' ) and cash_ind = 'Y' \
			and ac_type != '*' order by opn_br_no,ac_type" );
	if ( ret )
	{
		sprintf( acErrMsg, "ִ��Dd_mst_Dec_Sel��[%d]", ret );
		WRITEMSG
		fclose(fp);
		goto ErrExit;
	}

	fprintf( fp, "~%s|%s|%s|%s|\n", "��������", "����", "�˺�", "�˻�����" );
	
	while(1)
	{
		memset( &sDd_mst, 0x00, sizeof(sDd_mst) );
		
		ret = Dd_mst_Fet_Sel( &sDd_mst, g_pub_tx.reply );
		if ( ret == 100)
		{
			break;
		}
		else if( ret )
		{
			sprintf( acErrMsg, "ִ��Dd_mst_Fet_Sel��[%d]", ret );
			WRITEMSG
			fclose(fp);
			goto ErrExit;
		}

		memset( &sMdm_ac_rel, 0x0, sizeof(sMdm_ac_rel) );
		ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdm_ac_rel, \
			"ac_id = %ld", sDd_mst.ac_id );

		memset( cAc_sts, 0x0, sizeof(cAc_sts) );
		pub_base_strpack( sDd_mst.ac_type );

		if ( sDd_mst.ac_type[0] == '1' )
		{
			strcpy( cAc_sts, "������" );
		}
		else if ( sDd_mst.ac_type[0] == '3' )
		{
			strcpy( cAc_sts, "ר�ô�" );
		}
		else if ( sDd_mst.ac_type[0] == '4' )
		{
			strcpy( cAc_sts, "��ʱ��" );
		}

		fprintf( fp, "%s|%s|%s|%s|\n", sDd_mst.opn_br_no, sDd_mst.name, sMdm_ac_rel.ac_no, cAc_sts );
	}
	fclose(fp);

	Dd_mst_Clo_Sel( );

	set_zd_data( DC_FILE_SND, "1" );		/* �������ͱ�־ */


OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"�ɹ�!!!");
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	Dd_mst_Clo_Sel( );
	sprintf(acErrMsg,"ʧ��reply[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
