/*************************************************************
* �� �� ��: spF701.c
* �����������ж��Ƿ����˺�ҵ��(��ɳ���к�Ϊ:4302)
* ��    ��: jack
* ������ڣ�2003��2��20��
*
* �޸ļ�¼��
* ��    ��: 2003��5��23��
* �� �� ��: jack
* �޸�����: �����������������ֿ�
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"

spF701()
{
	char cCity[5];
	int ret = 0;

	char cTrcode[5];
	char cRemark[20];
	char cName[50];
	char cAc_no[20];
	
	char filename[64];
	FILE *fp;

	double tram = 0.00;		/*���׽��*/

	memset(cCity,0x0,sizeof(cCity));
	memset( cTrcode, 0x0, sizeof(cTrcode) );
	memset( cName, 0x0, sizeof(cName) );
	memset( cRemark, 0x0, sizeof(cRemark) );
	memset( cAc_no, 0x0, sizeof(cAc_no) );

	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();

	get_zd_data( "0160", cTrcode );
	get_zd_data( "0270", cName );
	get_zd_data( "0370", cAc_no );

	get_zd_data("0360",cCity);
	if( strncmp(cCity, "4302", 4) == 0 )	/*	Ŀ���в����ǳ�ɳ */
	{
		sprintf( acErrMsg, "Ŀ���д�!!!" );
		WRITEMSG
		goto ErrExit;
	}
	
	memset( filename, 0x0, sizeof(filename) );
	pub_base_AllDwnFilName( filename );

	fp = fopen( filename, "w" );
	if ( fp == NULL )
	{
		sprintf(acErrMsg," open file [%s] error ",filename );
		WRITEMSG
		strcpy( g_pub_tx.reply,"S047" );
		goto ErrExit;
	}

	if ( strncmp( cTrcode, "7701", 4 ) == 0 )
	{
		strcpy( cRemark, "����˻�����" );
	}
	else if ( strncmp( cTrcode, "7702", 4 ) == 0 )
	{
		strcpy( cRemark, "����˻�ȡ��" );
	}
	get_zd_double("0400",&tram);

	fprintf( fp, "DLPT%s|%s|%lf|%ld|%s|%s|%s|%ld|%s|%s|\n", \
		cRemark, "", tram, \
		g_pub_tx.tx_date, g_pub_tx.tel, cAc_no, cName, \
		g_pub_tx.trace_no, "�ֽ�", g_pub_tx.tx_br_no );

	fclose( fp );

	set_zd_data("0310",TESTCARD );		/* ֱ�Ӽ������Ŀ*/
	set_zd_int("0350",1);				/*�����˺����*/
	
	set_zd_data( "0110", "1" );

OKExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"���˻�Ϊ�����˻�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	strcpy( g_pub_tx.reply,"F006" );
	sprintf( acErrMsg,"����Ŀ���д�![%s]",cCity );
	WRITEMSG
	set_zd_data( "0120",g_pub_tx.reply );
	return 1;
}
