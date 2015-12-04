/*************************************************
* �� �� ��:  sp8611.c
* ���������� ͬҵ����˻���Ϣ
*
* ��    ��:  rob
* ������ڣ� 2003��12��20��
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

int sp8611()
{
	char cFileName[50];			/* ��ʱ�ļ��� */
	int	 ret = 0;
	FILE *fp;

	char cAc_sts[11];

	struct dd_mst_c	sDd_mst;

	memset( &sDd_mst, 0x00, sizeof(sDd_mst) );

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
			"prdt_no in('135','136','137') order by prdt_no" );
	if ( ret )
	{
		sprintf( acErrMsg, "ִ��Dd_mst_Dec_Sel��[%d]", ret );
		WRITEMSG
		fclose(fp);
		goto ErrExit;
	}

	fprintf( fp, "~%s|%s|%s|%s|%s|%s|\n", "��������", "��Ʒ���", "���", "����\���", "�˻�״̬", "����" );
	
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

		memset( cAc_sts, 0x0, sizeof(cAc_sts) );
		pub_base_strpack( sDd_mst.ac_sts );

		if ( sDd_mst.ac_sts[0] == '0' )
		{
			strcpy( cAc_sts, "������ȷ��" );
		}
		else if ( sDd_mst.ac_sts[0] == '1' )
		{
			strcpy( cAc_sts, "����" );
		}
		else if ( sDd_mst.ac_sts[0] == '3' )
		{
			strcpy( cAc_sts, "��ʧ����" );
		}
		else if ( sDd_mst.ac_sts[0] == '4' )
		{
			strcpy( cAc_sts, "��������" );
		}
		else if ( sDd_mst.ac_sts[0] == '5' )
		{
			strcpy( cAc_sts, "��ʱ����" );
		}
		else if ( sDd_mst.ac_sts[0] == '*' )
		{
			strcpy( cAc_sts, "����" );
		}
		else if ( sDd_mst.ac_sts[0] == '#' )
		{
			strcpy( cAc_sts, "��Ϣ����" );
		}

		fprintf(fp,"%s|%s|%.2lf|%.2lf|%s|%s|\n", sDd_mst.opn_br_no, sDd_mst.prdt_no, sDd_mst.bal, sDd_mst.ys_bal, cAc_sts, sDd_mst.name );
	}
	fclose(fp);

	Dd_mst_Clo_Sel( );

	set_zd_data( DC_FILE_SND, "1" );		/* �������ͱ�־ */


OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"ͬҵ����˻���Ϣ�ɹ�!!!");
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	Dd_mst_Clo_Sel( );
	sprintf(acErrMsg,"ͬҵ����˻���Ϣʧ��reply[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
