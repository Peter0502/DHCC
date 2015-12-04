/*************************************************************
* �� �� ��: mycl.c
* �����������ֶ�������Ѻ
*
* ��    ��: jack
* �������: 2003��4��6��
* ����˵��:
			-------�����������Ǹ���������Ѻ
			��һλ��1������;2������;3������;4���ڲ���;
					5��͸֧;6�������˺Ŷ��ձ�;7������Ƿ���
			�ڶ�λ���˺�ID
			����λ���˺����
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
#include "public.h"
#include "svrpub_c.h"
#include "ln_lo_c.h"

main(int argc, char *argv[])
{
	int ret = 0;
	char str[3000];
	char mystery[17];
	long lAc_id;
	int iAc_seqn;

	struct dd_mst_c sDd_mst;
	struct td_mst_c sTd_mst;
	struct in_mst_c sIn_mst;
	struct ln_mst_c sLn_mst;
	struct od_mst_c sOd_mst;
	struct mdm_ac_rel_c sMdm_ac_rel;
	struct ln_lo_c sLn_lo;

	fprintf( stdout, "����˵��:mycl ������ �˺�ID �˺����\n");

	if ( (argc != 1) && (argc != 2) && (argc != 4) )
	{
		fprintf( stdout, "���ò�����!mycl ������ �˺�ID �˺����\n");
		sprintf( acErrMsg, "���ò�����!!!!!!!!!!");
		WRITEMSG
		goto ErrExit;
	}

	ret=sql_database(); /*�����ݿ�*/
	if( ret ) 
	{
		sprintf( acErrMsg, "�����ݿ�ʧ��!!!" );
		WRITEMSG
		goto ErrExit;
	}

	ret=sql_begin(); /*������*/
	if( ret ) 
	{
		sprintf( acErrMsg, "������ʧ��!!!" );
		WRITEMSG
		goto ErrExit;
	}

    /**------- ��ʼ��ȫ�ֱ��� --------**/
	pub_base_sysinit();

	if ( argc == 1 )		/* ȫ����Ѻ */
	{
		ret = Dd_mst_Dec_Upd1( g_pub_tx.reply , " 1 =1 ");
		if ( ret )
		{
			sprintf( acErrMsg, "ִ��Dd_mst_Dec_Upd1��!!!");
			WRITEMSG
			goto ErrExit;
		}

		while( 1 )
		{
			memset(&sDd_mst, 0x00, sizeof(sDd_mst) );
			ret = Dd_mst_Fet_Upd1( &sDd_mst, g_pub_tx.reply );
			if ( ret == 100 )
			{
				break;
			}
			if ( ret<0 )
			{
				sprintf( acErrMsg, "ִ��Dd_mst_Fet_Upd1��!!!");
				WRITEMSG
				goto ErrExit;
			}

			fprintf( stdout, "���Ļ�����Ѻ!!!\n" );

			memset( str, 0x0,sizeof(str) );
			memset( mystery, 0x0, sizeof(mystery) );
			ret = dd_mst_con ( sDd_mst, str );
			ret = put_mystery( str, mystery );
			strcpy( sDd_mst.mac, mystery );

			ret = Dd_mst_Upd_Upd1( sDd_mst, g_pub_tx.reply  );
			if ( ret )
			{
				sprintf( acErrMsg, "ִ��Dd_mst_Upd_Upd1��!!!");
				WRITEMSG
				goto ErrExit;
			}

		}

		Dd_mst_Clo_Upd1( );
		

	sql_commit();
	sql_begin(); /*������*/
	/*-----------------------------------------------------*/
		ret = Td_mst_Dec_Upd1( g_pub_tx.reply , "1 = 1");
		if ( ret )
		{
			sprintf( acErrMsg, "ִ��Td_mst_Dec_Upd1��!!!");
			WRITEMSG
			goto ErrExit;
		}

		while( 1 )
		{
			memset(&sTd_mst, 0x00, sizeof(sTd_mst) );
			ret = Td_mst_Fet_Upd1( &sTd_mst, g_pub_tx.reply );
			if ( ret == 100 )
			{
				break;
			}
			if ( ret<0 )
			{
				sprintf( acErrMsg, "ִ��Td_mst_Fet_Upd1��!!!");
				WRITEMSG
				goto ErrExit;
			}

			fprintf( stdout, "���Ķ�����Ѻ!!!\n" );

			memset( str, 0x0,sizeof(str) );
			memset( mystery, 0x0, sizeof(mystery) );
			ret = td_mst_con ( sTd_mst, str );
			ret = put_mystery( str, mystery );
			strcpy( sTd_mst.mac, mystery );

			ret = Td_mst_Upd_Upd1( sTd_mst, g_pub_tx.reply  );
			if ( ret )
			{
				sprintf( acErrMsg, "ִ��Td_mst_Upd_Upd1��!!!");
				WRITEMSG
				continue;
				/*
				goto ErrExit;
				*/
			}

		}

		Td_mst_Clo_Upd1( );
		

	sql_commit();
	sql_begin(); /*������*/
		/*-----------------------------------------------------*/
		ret = Ln_mst_Dec_Upd1( g_pub_tx.reply , " 1 = 1");
		if ( ret )
		{
			sprintf( acErrMsg, "ִ��Ln_mst_Dec_Upd1��!!!");
			WRITEMSG
			goto ErrExit;
		}

		while( 1 )
		{
			memset(&sLn_mst, 0x00, sizeof(sLn_mst) );
			ret = Ln_mst_Fet_Upd1( &sLn_mst, g_pub_tx.reply );
			if ( ret == 100 )
			{
				break;
			}
			if ( ret<0 )
			{
				sprintf( acErrMsg, "ִ��Ln_mst_Fet_Upd1��!!!");
				WRITEMSG
				goto ErrExit;
			}

			fprintf( stdout, "���Ĵ�����Ѻ!!!\n" );

			memset( str, 0x0,sizeof(str) );
			memset( mystery, 0x0, sizeof(mystery) );
			ret = ln_mst_con ( sLn_mst, str );
			ret = put_mystery( str, mystery );
			strcpy( sLn_mst.mac, mystery );

			ret = Ln_mst_Upd_Upd1( sLn_mst, g_pub_tx.reply  );
			if ( ret )
			{
				sprintf( acErrMsg, "ִ��Ln_mst_Upd_Upd1��!!!");
				WRITEMSG
				goto ErrExit;
			}

		}

		Ln_mst_Clo_Upd1( );
		

	sql_commit();
	sql_begin(); /*������*/
		/*-----------------------------------------------------*/
		ret = In_mst_Dec_Upd1( g_pub_tx.reply , "1 = 1");
		if ( ret )
		{
			sprintf( acErrMsg, "ִ��In_mst_Dec_Upd1��!!!");
			WRITEMSG
			goto ErrExit;
		}

		while( 1 )
		{
			memset(&sIn_mst, 0x00, sizeof(sIn_mst) );
			ret = In_mst_Fet_Upd1( &sIn_mst, g_pub_tx.reply );
			if ( ret == 100 )
			{
				break;
			}
			if ( ret<0 )
			{
				sprintf( acErrMsg, "ִ��In_mst_Fet_Upd1��!!!");
				WRITEMSG
				goto ErrExit;
			}

			fprintf( stdout, "�����ڲ�����Ѻ!!!\n" );

			memset( str, 0x0,sizeof(str) );
			memset( mystery, 0x0, sizeof(mystery) );
			ret = in_mst_con ( sIn_mst, str );
			ret = put_mystery( str, mystery );
			strcpy( sIn_mst.mac, mystery );

			ret = In_mst_Upd_Upd1( sIn_mst, g_pub_tx.reply  );
			if ( ret )
			{
				sprintf( acErrMsg, "ִ��In_mst_Upd_Upd1��!!!");
				WRITEMSG
				goto ErrExit;
			}

		}

		In_mst_Clo_Upd1( );
		

		/*-----------------------------------------------------*/
		ret = Od_mst_Dec_Upd1( g_pub_tx.reply , " 1 = 1");
		if ( ret )
		{
			sprintf( acErrMsg, "ִ��Od_mst_Dec_Upd1��!!!");
			WRITEMSG
			goto ErrExit;
		}

		while( 1 )
		{
			memset(&sOd_mst, 0x00, sizeof(sOd_mst) );
			ret = Od_mst_Fet_Upd1( &sOd_mst, g_pub_tx.reply );
			if ( ret == 100 )
			{
				break;
			}
			if ( ret<0 )
			{
				sprintf( acErrMsg, "ִ��Od_mst_Fet_Upd1��!!!");
				WRITEMSG
				goto ErrExit;
			}

			fprintf( stdout, "����͸֧��Ѻ!!!\n" );

			memset( str, 0x0,sizeof(str) );
			memset( mystery, 0x0, sizeof(mystery) );
			ret = od_mst_con ( sOd_mst, str );
			ret = put_mystery( str, mystery );
			strcpy( sOd_mst.mac, mystery );

			ret = Od_mst_Upd_Upd1( sOd_mst, g_pub_tx.reply  );
			if ( ret )
			{
				sprintf( acErrMsg, "ִ��Od_mst_Upd_Upd1��!!!");
				WRITEMSG
				goto ErrExit;
			}

		}

		Od_mst_Clo_Upd1( );

		/*-----------------------------------------------------*/
		ret = Mdm_ac_rel_Dec_Upd1( g_pub_tx.reply , " 1 = 1" );
		if ( ret )
		{
			sprintf( acErrMsg, "ִ��Mdm_ac_rel_Dec_Upd1��!!!");
			WRITEMSG
			goto ErrExit;
		}

		while( 1 )
		{
			memset(&sMdm_ac_rel, 0x00, sizeof(sMdm_ac_rel) );
			ret = Mdm_ac_rel_Fet_Upd1( &sMdm_ac_rel, g_pub_tx.reply );
			if ( ret == 100 )
			{
				break;
			}
			if ( ret<0 )
			{
				sprintf( acErrMsg, "ִ��Mdm_ac_rel_Fet_Upd1��!!!");
				WRITEMSG
				goto ErrExit;
			}

			fprintf( stdout, "���Ľ����˺Ŷ��ձ���Ѻ!!!\n" );

			memset( str, 0x0,sizeof(str) );
			memset( mystery, 0x0, sizeof(mystery) );
			ret = mdm_ac_rel_con ( sMdm_ac_rel, str );
			ret = put_mystery( str, mystery );
			strcpy( sMdm_ac_rel.mac, mystery );

			ret = Mdm_ac_rel_Upd_Upd1( sMdm_ac_rel, g_pub_tx.reply  );
			if ( ret )
			{
				sprintf( acErrMsg, "ִ��Mdm_ac_rel_Upd_Upd1��!!!");
				WRITEMSG
				goto ErrExit;
			}

		}

		Mdm_ac_rel_Clo_Upd1( );

		ret = Ln_lo_Dec_Upd1( g_pub_tx.reply , " 1 =1 ");
		if ( ret )
		{
			sprintf( acErrMsg, "ִ��Ln_lo_Dec_Upd1��!!!");
			WRITEMSG
			goto ErrExit;
		}

		while( 1 )
		{
			memset(&sLn_lo, 0x00, sizeof(sLn_lo) );
			ret = Ln_lo_Fet_Upd1( &sLn_lo, g_pub_tx.reply );
			if ( ret == 100 )
			{
				break;
			}
			if ( ret )
			{
				sprintf( acErrMsg, "ִ��Ln_lo_Fet_Upd1��!!!");
				WRITEMSG
				goto ErrExit;
			}

			fprintf( stdout, "���İ���Ƿ����Ѻ!!!\n" );

			memset( str, 0x0,sizeof(str) );
			memset( mystery, 0x0, sizeof(mystery) );
			ret = ln_lo_con ( sLn_lo, str );
			ret = put_mystery( str, mystery );
			strcpy( sLn_lo.mac, mystery );

			ret = Ln_lo_Upd_Upd1( sLn_lo, g_pub_tx.reply  );
			if ( ret )
			{
				sprintf( acErrMsg, "ִ��Ln_lo_Upd_Upd1��!!!");
				WRITEMSG
				goto ErrExit;
			}

		}

		Ln_lo_Clo_Upd1( );

	}
	if ( argc == 2 )		/* ��������Ѻ */
	{
		if ( argv[1][0] == '1' )
		{
			ret = Dd_mst_Dec_Upd1( g_pub_tx.reply , " 1=1 " );
			if ( ret )
			{
				sprintf( acErrMsg, "ִ��Dd_mst_Dec_Upd1��!!!");
				WRITEMSG
				goto ErrExit;
			}

			while( 1 )
			{
				memset(&sDd_mst, 0x00, sizeof(sDd_mst) );
				ret = Dd_mst_Fet_Upd1( &sDd_mst, g_pub_tx.reply );
				if ( ret == 100 )
				{
					break;
				}
				if ( ret<0 )
				{
					sprintf( acErrMsg, "ִ��Dd_mst_Fet_Upd1��!!!");
					WRITEMSG
					goto ErrExit;
				}

				memset( str, 0x0,sizeof(str) );
				memset( mystery, 0x0, sizeof(mystery) );
				ret = dd_mst_con ( sDd_mst, str );
				ret = put_mystery( str, mystery );
				strcpy( sDd_mst.mac, mystery );

				fprintf( stdout, "���Ļ�����Ѻ!!!\n" );

				ret = Dd_mst_Upd_Upd1( sDd_mst, g_pub_tx.reply  );
				if ( ret )
				{
					sprintf( acErrMsg, "ִ��Dd_mst_Upd_Upd1��!!!");
					WRITEMSG
					goto ErrExit;
				}

			}

			Dd_mst_Clo_Upd1( );
		}

		/*-----------------------------------------------------*/
		if ( argv[1][0] == '2' )
		{
			ret = Td_mst_Dec_Upd1( g_pub_tx.reply , " 1=1 " );
			if ( ret )
			{
				sprintf( acErrMsg, "ִ��Td_mst_Dec_Upd1��!!!");
				WRITEMSG
				goto ErrExit;
			}

			while( 1 )
			{
				memset(&sTd_mst, 0x00, sizeof(sTd_mst) );
				ret = Td_mst_Fet_Upd1( &sTd_mst, g_pub_tx.reply );
				if ( ret == 100 )
				{
					break;
				}
				if ( ret<0 )
				{
					sprintf( acErrMsg, "ִ��Td_mst_Fet_Upd1��!!!");
					WRITEMSG
					goto ErrExit;
				}

				memset( str, 0x0,sizeof(str) );
				memset( mystery, 0x0, sizeof(mystery) );
				ret = td_mst_con ( sTd_mst, str );
				ret = put_mystery( str, mystery );
				strcpy( sTd_mst.mac, mystery );

				ret = Td_mst_Upd_Upd1( sTd_mst, g_pub_tx.reply  );
				if ( ret )
				{
					sprintf( acErrMsg, "ִ��Td_mst_Upd_Upd1��!!!");
					WRITEMSG
					goto ErrExit;
				}

			}

			Td_mst_Clo_Upd1( );
		}

		/*-----------------------------------------------------*/
		if ( argv[1][0] == '3' )
		{
			ret = Ln_mst_Dec_Upd1( g_pub_tx.reply , " 1=1 " );
			if ( ret )
			{
				sprintf( acErrMsg, "ִ��Ln_mst_Dec_Upd1��!!!");
				WRITEMSG
				goto ErrExit;
			}

			while( 1 )
			{
				memset(&sLn_mst, 0x00, sizeof(sLn_mst) );
				ret = Ln_mst_Fet_Upd1( &sLn_mst, g_pub_tx.reply );
				if ( ret == 100 )
				{
					break;
				}
				if ( ret<0 )
				{
					sprintf( acErrMsg, "ִ��Ln_mst_Fet_Upd1��!!!");
					WRITEMSG
					goto ErrExit;
				}

				memset( str, 0x0,sizeof(str) );
				memset( mystery, 0x0, sizeof(mystery) );
				ret = ln_mst_con ( sLn_mst, str );
				ret = put_mystery( str, mystery );
				strcpy( sLn_mst.mac, mystery );

				ret = Ln_mst_Upd_Upd1( sLn_mst, g_pub_tx.reply  );
				if ( ret )
				{
					sprintf( acErrMsg, "ִ��Ln_mst_Upd_Upd1��!!!");
					WRITEMSG
					goto ErrExit;
				}

				fprintf( stdout, "--->���Ĵ�����Ѻ!!!\n" );

			}

			Ln_mst_Clo_Upd1( );
		}

		/*-----------------------------------------------------*/
		if ( argv[1][0] == '4' )
		{
			ret = In_mst_Dec_Upd1( g_pub_tx.reply , " 1=1 " );
			if ( ret )
			{
				sprintf( acErrMsg, "ִ��In_mst_Dec_Upd1��!!!");
				WRITEMSG
				goto ErrExit;
			}

			while( 1 )
			{
				memset(&sIn_mst, 0x00, sizeof(sIn_mst) );
				ret = In_mst_Fet_Upd1( &sIn_mst, g_pub_tx.reply );
				if ( ret == 100 )
				{
					break;
				}
				if ( ret<0 )
				{
					sprintf( acErrMsg, "ִ��In_mst_Fet_Upd1��!!!");
					WRITEMSG
					goto ErrExit;
				}

				memset( str, 0x0,sizeof(str) );
				memset( mystery, 0x0, sizeof(mystery) );
				ret = in_mst_con ( sIn_mst, str );
				ret = put_mystery( str, mystery );
				strcpy( sIn_mst.mac, mystery );

				ret = In_mst_Upd_Upd1( sIn_mst, g_pub_tx.reply  );
				if ( ret )
				{
					sprintf( acErrMsg, "ִ��In_mst_Upd_Upd1��!!!");
					WRITEMSG
					goto ErrExit;
				}

			}

			In_mst_Clo_Upd1( );
		}

		/*-----------------------------------------------------*/
		if ( argv[1][0] == '5' )
		{
			ret = Od_mst_Dec_Upd1( g_pub_tx.reply , " 1=1 " );
			if ( ret )
			{
				sprintf( acErrMsg, "ִ��Od_mst_Dec_Upd1��!!!");
				WRITEMSG
				goto ErrExit;
			}

			while( 1 )
			{
				memset(&sOd_mst, 0x00, sizeof(sOd_mst) );
				ret = Od_mst_Fet_Upd1( &sOd_mst, g_pub_tx.reply );
				if ( ret == 100 )
				{
					break;
				}
				if ( ret<0 )
				{
					sprintf( acErrMsg, "ִ��Od_mst_Fet_Upd1��!!!");
					WRITEMSG
					goto ErrExit;
				}

				memset( str, 0x0,sizeof(str) );
				memset( mystery, 0x0, sizeof(mystery) );
				ret = od_mst_con ( sOd_mst, str );
				ret = put_mystery( str, mystery );
				strcpy( sOd_mst.mac, mystery );

				ret = Od_mst_Upd_Upd1( sOd_mst, g_pub_tx.reply  );
				if ( ret )
				{
					sprintf( acErrMsg, "ִ��Od_mst_Upd_Upd1��!!!");
					WRITEMSG
					goto ErrExit;
				}

			}

			Od_mst_Clo_Upd1( );
		}

		/*-----------------------------------------------------*/
		if ( argv[1][0] == '6' )
		{
			ret = Mdm_ac_rel_Dec_Upd1( g_pub_tx.reply , " 1=1 " );
			if ( ret )
			{
				sprintf( acErrMsg, "ִ��Mdm_ac_rel_Dec_Upd1��!!!");
				WRITEMSG
				goto ErrExit;
			}

			while( 1 )
			{
				memset(&sMdm_ac_rel, 0x00, sizeof(sMdm_ac_rel) );
				ret = Mdm_ac_rel_Fet_Upd1( &sMdm_ac_rel, g_pub_tx.reply );
				if ( ret == 100 )
				{
					break;
				}
				if ( ret<0 )
				{
					sprintf( acErrMsg, "ִ��Mdm_ac_rel_Fet_Upd1��!!!");
					WRITEMSG
					goto ErrExit;
				}

				memset( str, 0x0,sizeof(str) );
				memset( mystery, 0x0, sizeof(mystery) );
				ret = mdm_ac_rel_con ( sMdm_ac_rel, str );
				ret = put_mystery( str, mystery );
				strcpy( sMdm_ac_rel.mac, mystery );

				ret = Mdm_ac_rel_Upd_Upd1( sMdm_ac_rel, g_pub_tx.reply  );
				if ( ret )
				{
					sprintf( acErrMsg, "ִ��Mdm_ac_rel_Upd_Upd1��!!!");
					WRITEMSG
					goto ErrExit;
				}

			}

			Mdm_ac_rel_Clo_Upd1( );
		}
		if ( argv[1][0] == '7' )
		{
			ret = Ln_lo_Dec_Upd1( g_pub_tx.reply , " 1 =1 ");
			if ( ret )
			{
				sprintf( acErrMsg, "ִ��Ln_lo_Dec_Upd1��!!!");
				WRITEMSG
				goto ErrExit;
			}

			while( 1 )
			{
				memset(&sLn_lo, 0x00, sizeof(sLn_lo) );
				ret = Ln_lo_Fet_Upd1( &sLn_lo, g_pub_tx.reply );
				if ( ret == 100 )
				{
					break;
				}
				if ( ret<0 )
				{
					sprintf( acErrMsg, "ִ��Ln_lo_Fet_Upd1��!!!");
					WRITEMSG
					goto ErrExit;
				}

				fprintf( stdout, "���İ���Ƿ����Ѻ!!!\n" );

				memset( str, 0x0,sizeof(str) );
				memset( mystery, 0x0, sizeof(mystery) );
				ret = ln_lo_con ( sLn_lo, str );
				ret = put_mystery( str, mystery );
				strcpy( sLn_lo.mac, mystery );

				ret = Ln_lo_Upd_Upd1( sLn_lo, g_pub_tx.reply  );
				if ( ret )
				{
					sprintf( acErrMsg, "ִ��Ln_lo_Upd_Upd1��!!!");
					WRITEMSG
					goto ErrExit;
				}
			}
			Ln_lo_Clo_Upd1( );
		}
	}
	if ( argc == 4 )		/* �����˻���Ѻ */
	{
		lAc_id = atol( argv[2] );
		iAc_seqn = atoi( argv[3] );

		if ( argv[1][0] == '1' )
		{
			ret = Dd_mst_Dec_Upd1( g_pub_tx.reply , "ac_id = %ld and \
					ac_seqn = %d " , lAc_id, iAc_seqn);
			if ( ret )
			{
				sprintf( acErrMsg, "ִ��Dd_mst_Dec_Upd1��!!!");
				WRITEMSG
				goto ErrExit;
			}

			while( 1 )
			{
				memset(&sDd_mst, 0x00, sizeof(sDd_mst) );
				ret = Dd_mst_Fet_Upd1( &sDd_mst, g_pub_tx.reply );
				if ( ret == 100 )
				{
					break;
				}
				if ( ret )
				{
					sprintf( acErrMsg, "ִ��Dd_mst_Fet_Upd1��!!!");
					WRITEMSG
					goto ErrExit;
				}

				memset( str, 0x0,sizeof(str) );
				memset( mystery, 0x0, sizeof(mystery) );
				ret = dd_mst_con ( sDd_mst, str );
				ret = put_mystery( str, mystery );
				strcpy( sDd_mst.mac, mystery );

				ret = Dd_mst_Upd_Upd1( sDd_mst, g_pub_tx.reply  );
				if ( ret )
				{
					sprintf( acErrMsg, "ִ��Dd_mst_Upd_Upd1��!!!");
					WRITEMSG
					goto ErrExit;
				}

			}

			Dd_mst_Clo_Upd1( );
		}

		/*-----------------------------------------------------*/
		if ( argv[1][0] == '2' )
		{
			ret = Td_mst_Dec_Upd1( g_pub_tx.reply , "ac_id = %ld and \
					ac_seqn = %d " , lAc_id, iAc_seqn);
			if ( ret )
			{
				sprintf( acErrMsg, "ִ��Td_mst_Dec_Upd1��!!!");
				WRITEMSG
				goto ErrExit;
			}

			while( 1 )
			{
				memset(&sTd_mst, 0x00, sizeof(sTd_mst) );
				ret = Td_mst_Fet_Upd1( &sTd_mst, g_pub_tx.reply );
				if ( ret == 100 )
				{
					break;
				}
				if ( ret )
				{
					sprintf( acErrMsg, "ִ��Td_mst_Fet_Upd1��!!!");
					WRITEMSG
					goto ErrExit;
				}

				memset( str, 0x0,sizeof(str) );
				memset( mystery, 0x0, sizeof(mystery) );
				ret = td_mst_con ( sTd_mst, str );
				ret = put_mystery( str, mystery );
				strcpy( sTd_mst.mac, mystery );

				ret = Td_mst_Upd_Upd1( sTd_mst, g_pub_tx.reply  );
				if ( ret )
				{
					sprintf( acErrMsg, "ִ��Td_mst_Upd_Upd1��!!!");
					WRITEMSG
					goto ErrExit;
				}

			}

			Td_mst_Clo_Upd1( );
		}

		/*-----------------------------------------------------*/
		if ( argv[1][0] == '3' )
		{
			ret = Ln_mst_Dec_Upd1( g_pub_tx.reply , "ac_id = %ld and \
					ac_seqn = %d " , lAc_id, iAc_seqn);
			if ( ret )
			{
				sprintf( acErrMsg, "ִ��Ln_mst_Dec_Upd1��!!!");
				WRITEMSG
				goto ErrExit;
			}

			while( 1 )
			{
				memset(&sLn_mst, 0x00, sizeof(sLn_mst) );
				ret = Ln_mst_Fet_Upd1( &sLn_mst, g_pub_tx.reply );
				if ( ret == 100 )
				{
					break;
				}
				if ( ret )
				{
					sprintf( acErrMsg, "ִ��Ln_mst_Fet_Upd1��!!!");
					WRITEMSG
					goto ErrExit;
				}

				memset( str, 0x0,sizeof(str) );
				memset( mystery, 0x0, sizeof(mystery) );
				ret = ln_mst_con ( sLn_mst, str );
				ret = put_mystery( str, mystery );
				strcpy( sLn_mst.mac, mystery );

				ret = Ln_mst_Upd_Upd1( sLn_mst, g_pub_tx.reply  );
				if ( ret )
				{
					sprintf( acErrMsg, "ִ��Ln_mst_Upd_Upd1��!!!");
					WRITEMSG
					goto ErrExit;
				}

			}

			Ln_mst_Clo_Upd1( );
		}

		/*-----------------------------------------------------*/
		if ( argv[1][0] == '4' )
		{
			ret = In_mst_Dec_Upd1( g_pub_tx.reply , "ac_id = %ld and \
					ac_seqn = %d " , lAc_id, iAc_seqn);
			if ( ret )
			{
				sprintf( acErrMsg, "ִ��In_mst_Dec_Upd1��!!!");
				WRITEMSG
				goto ErrExit;
			}

			while( 1 )
			{
				memset(&sIn_mst, 0x00, sizeof(sIn_mst) );
				ret = In_mst_Fet_Upd1( &sIn_mst, g_pub_tx.reply );
				if ( ret == 100 )
				{
					break;
				}
				if ( ret )
				{
					sprintf( acErrMsg, "ִ��In_mst_Fet_Upd1��!!!");
					WRITEMSG
					goto ErrExit;
				}

				memset( str, 0x0,sizeof(str) );
				memset( mystery, 0x0, sizeof(mystery) );
				ret = in_mst_con ( sIn_mst, str );
				ret = put_mystery( str, mystery );
				strcpy( sIn_mst.mac, mystery );

				ret = In_mst_Upd_Upd1( sIn_mst, g_pub_tx.reply  );
				if ( ret )
				{
					sprintf( acErrMsg, "ִ��In_mst_Upd_Upd1��!!!");
					WRITEMSG
					goto ErrExit;
				}

			}

			In_mst_Clo_Upd1( );
		}

		/*-----------------------------------------------------*/
		if ( argv[1][0] == '5' )
		{
			ret = Od_mst_Dec_Upd1( g_pub_tx.reply , "ac_id = %ld and \
					ac_seqn = %d " , lAc_id, iAc_seqn);
			if ( ret )
			{
				sprintf( acErrMsg, "ִ��Od_mst_Dec_Upd1��!!!");
				WRITEMSG
				goto ErrExit;
			}

			while( 1 )
			{
				memset(&sOd_mst, 0x00, sizeof(sOd_mst) );
				ret = Od_mst_Fet_Upd1( &sOd_mst, g_pub_tx.reply );
				if ( ret == 100 )
				{
					break;
				}
				if ( ret )
				{
					sprintf( acErrMsg, "ִ��Od_mst_Fet_Upd1��!!!");
					WRITEMSG
					goto ErrExit;
				}

				memset( str, 0x0,sizeof(str) );
				memset( mystery, 0x0, sizeof(mystery) );
				ret = od_mst_con ( sOd_mst, str );
				ret = put_mystery( str, mystery );
				strcpy( sOd_mst.mac, mystery );

				ret = Od_mst_Upd_Upd1( sOd_mst, g_pub_tx.reply  );
				if ( ret )
				{
					sprintf( acErrMsg, "ִ��Od_mst_Upd_Upd1��!!!");
					WRITEMSG
					goto ErrExit;
				}

			}

			Od_mst_Clo_Upd1( );
		}

		/*-----------------------------------------------------*/
		if ( argv[1][0] == '6' )
		{
			ret = Mdm_ac_rel_Dec_Upd1( g_pub_tx.reply , "ac_id = %ld and \
					ac_seqn = %d " , lAc_id, iAc_seqn);
			if ( ret )
			{
				sprintf( acErrMsg, "ִ��Mdm_ac_rel_Dec_Upd1��!!!");
				WRITEMSG
				goto ErrExit;
			}

			while( 1 )
			{
				memset(&sMdm_ac_rel, 0x00, sizeof(sMdm_ac_rel) );
				ret = Mdm_ac_rel_Fet_Upd1( &sMdm_ac_rel, g_pub_tx.reply );
				if ( ret == 100 )
				{
					break;
				}
				if ( ret )
				{
					sprintf( acErrMsg, "ִ��Mdm_ac_rel_Fet_Upd1��!!!");
					WRITEMSG
					goto ErrExit;
				}

				memset( str, 0x0,sizeof(str) );
				memset( mystery, 0x0, sizeof(mystery) );
				ret = mdm_ac_rel_con ( sMdm_ac_rel, str );
				ret = put_mystery( str, mystery );
				strcpy( sMdm_ac_rel.mac, mystery );

				ret = Mdm_ac_rel_Upd_Upd1( sMdm_ac_rel, g_pub_tx.reply  );
				if ( ret )
				{
					sprintf( acErrMsg, "ִ��Mdm_ac_rel_Upd_Upd1��!!!");
					WRITEMSG
					goto ErrExit;
				}

			}

			Mdm_ac_rel_Clo_Upd1( );
		}

		/*-----------------------------------------------------*/
		if ( argv[1][0] == '7' )
		{
			ret = Ln_lo_Dec_Upd1( g_pub_tx.reply , " ac_id = %ld and \
					ac_seqn = %d ", lAc_id, iAc_seqn);
			if ( ret )
			{
				sprintf( acErrMsg, "ִ��Ln_lo_Dec_Upd1��!!!");
				WRITEMSG
				goto ErrExit;
			}

			while( 1 )
			{
				memset(&sLn_lo, 0x00, sizeof(sLn_lo) );
				ret = Ln_lo_Fet_Upd1( &sLn_lo, g_pub_tx.reply );
				if ( ret == 100 )
				{
					break;
				}
				if ( ret )
				{
					sprintf( acErrMsg, "ִ��Ln_lo_Fet_Upd1��!!!");
					WRITEMSG
					goto ErrExit;
				}

				fprintf( stdout, "���İ���Ƿ����Ѻ!!!\n" );

				memset( str, 0x0,sizeof(str) );
				memset( mystery, 0x0, sizeof(mystery) );
				ret = ln_lo_con ( sLn_lo, str );
				ret = put_mystery( str, mystery );
				strcpy( sLn_lo.mac, mystery );

				ret = Ln_lo_Upd_Upd1( sLn_lo, g_pub_tx.reply  );
				if ( ret )
				{
					sprintf( acErrMsg, "ִ��Ln_lo_Upd_Upd1��!!!");
					WRITEMSG
					goto ErrExit;
				}

			}

			Ln_lo_Clo_Upd1( );
		}
	}


OkExit:
	sql_commit();
	sql_close();
	sprintf(acErrMsg,"�ֶ�������Ѻ�ɹ�!!!!!");
	printf("---------%s\n",acErrMsg);
	WRITEMSG
	return 0;
ErrExit:
	sql_rollback();
	sql_close();
	sprintf(acErrMsg,"�ֶ�������Ѻʧ��!!!!!");
	printf("---------%s\n",acErrMsg);
	WRITEMSG
	return 1;
}
