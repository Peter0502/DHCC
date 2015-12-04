/*************************************************
* �� �� ��:  sp8612.c
* ���������� ���ͬҵ�ʻ���Ϣ
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

int sp8612()
{
	char cFileName[50];			/* ��ʱ�ļ��� */
	int	 ret = 0;
	FILE *fp;

	char cAc_sts[16];

	struct in_mst_c	sIn_mst;

	memset( &sIn_mst, 0x00, sizeof(sIn_mst) );

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

	ret = In_mst_Dec_Sel( g_pub_tx.reply , \
			"prdt_no in('911','917') order by opn_br_no,prdt_no" );
	if ( ret )
	{
		sprintf( acErrMsg, "ִ��In_mst_Dec_Sel��[%d]", ret );
		WRITEMSG
		fclose(fp);
		goto ErrExit;
	}

	fprintf( fp, "~%s|%s|%s|%s|%s|%s|\n", "��������", "��Ʒ���",\
	"���", "�������", "�˻�״̬", "����" );
	
	while(1)
	{
		memset( &sIn_mst, 0x00, sizeof(sIn_mst) );
		
		ret = In_mst_Fet_Sel( &sIn_mst, g_pub_tx.reply );
		if ( ret == 100)
		{
			break;
		}
		else if( ret )
		{
		sprintf( acErrMsg, "ִ��In_mst_Fet_Sel��[%d]", ret );
			WRITEMSG
			fclose(fp);
			goto ErrExit;
		}

		memset( cAc_sts, 0x0, sizeof(cAc_sts) );
		pub_base_strpack( sIn_mst.sts );

		if ( sIn_mst.sts[0] == '1' )
		{
			strcpy( cAc_sts, "����" );
		}
		else if ( sIn_mst.sts[0] == '2' )
		{
			strcpy( cAc_sts, "�����򶳽�" );
		}
		else if ( sIn_mst.sts[0] == '3' )
		{
			strcpy( cAc_sts, "�ѿ�Ŀ��ת" );
		}
		else if ( sIn_mst.sts[0] == '*' )
		{
			strcpy( cAc_sts, "����" );
		}

	fprintf(fp,"%s|%s|%.2lf|%.2lf|%s|%s|\n", sIn_mst.opn_br_no,\
	sIn_mst.prdt_no,sIn_mst.bal, sIn_mst.ys_bal, cAc_sts, sIn_mst.name );
	}
	fclose(fp);

	Dd_mst_Clo_Sel( );

	set_zd_data( DC_FILE_SND, "1" );		/* �������ͱ�־ */


OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"�����������������ļ��ļ��ɹ�!!!");
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	In_mst_Clo_Sel( );
	sprintf(acErrMsg,"�����������������ļ��ļ�ʧ��reply[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
