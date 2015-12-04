/***************************************************************
* �� �� ��: spJ802.c
* ����������֧��ҵ���շ���������
*			�Ǽ�֧��ҵ���շѻ��ܱ�
*			�Ǽ�֧��ҵ���շ���ϸ��
*
* ��    ��: jane
* ������ڣ�2004��07��14��
*
* �޸ļ�¼��
*    ��    ��:
*    �� �� ��:
*    �޸�����:
*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "hv_pub.h"
#include "hv_define.h"
#include "com_parm_c.h"
#include "find_debug.h"

spJ802()
{
	int i;
	char *feedetail_p;
	char feedetail_buf[100];

	struct mbfeoprtfeecl_c oprtfeecl_v;
	struct mbfeoprtfeelt_c oprtfeelt_v;
	struct com_parm_c com_parm_v;

	memset( &oprtfeecl_v, 0x00, sizeof( struct mbfeoprtfeecl_c ) );
	memset( &com_parm_v, 0x00, sizeof( struct com_parm_c ) );

	oprtfeecl_v.consigndate = atol( mbfetran.consigndate );
	strcpy( oprtfeecl_v.rdficenter, mbfetran.rdficenter );
	strcpy( oprtfeecl_v.feebkcode, mbfetran.isdficode );
	oprtfeecl_v.feebgdate = atol( mbfetran.feebgdate );
	oprtfeecl_v.feeeddate = atol( mbfetran.feeeddate );
	oprtfeecl_v.feeammunt = atof( mbfetran.feeammunt ) / 100;
	oprtfeecl_v.sapsrate = atof( mbfetran.sapsrate ) / 10000;
	oprtfeecl_v.bktprate = atof( mbfetran.bktprate ) / 10000;
	oprtfeecl_v.mothrate = atof( mbfetran.mothrate ) / 10000;
	oprtfeecl_v.hvpfeeamt = atof( mbfetran.hvpfeeamt ) / 100;
	oprtfeecl_v.hvpfeecnt = atol( mbfetran.hvpfeecnt );
	oprtfeecl_v.singlefeeamt = atof( mbfetran.singlefeeamt ) / 100;
	oprtfeecl_v.singlefeecnt = atol( mbfetran.singlefeecnt );
	oprtfeecl_v.infofeeamt = atof( mbfetran.infofeeamt ) / 100;
	oprtfeecl_v.infofeecnt = atol( mbfetran.infofeecnt );
	g_reply_int = Com_parm_Sel( g_pub_tx.reply, &com_parm_v, "parm_code='DEKM' and parm_seqn='4'");
	if( g_reply_int )
	{
		sprintf( acErrMsg, "��ѯ���֧��ҵ���շѼƷѿ�Ŀ����![%ld]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}
	strcpy( oprtfeecl_v.feeacno, com_parm_v.val );
	oprtfeecl_v.printno = 0;

	g_reply_int = Mbfeoprtfeecl_Ins( oprtfeecl_v, g_pub_tx.reply );
	if( g_reply_int )
	{
		sprintf( acErrMsg, "�Ǽ�֧��ҵ���շѻ��ܱ����ݿ����ʧ��[%d]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

	feedetail_p = mbfetran.hvpfeedetail;
	for( i=0; i<oprtfeecl_v.hvpfeecnt; i++ )
	{
		memset( &oprtfeelt_v, 0x00, sizeof( struct mbfeoprtfeelt_c ) );

		oprtfeelt_v.row_id = i + 1;
		oprtfeelt_v.consigndate = atol( mbfetran.consigndate );
		strcpy( oprtfeelt_v.paytools, "1" );
		MEMCPY_DEBUG( oprtfeelt_v.cmtno, feedetail_p, 3 ); feedetail_p += 3;
		MEMCPY_DEBUG( oprtfeelt_v.srflag, feedetail_p, 1 ); feedetail_p += 1;
		MEMCPY_DEBUG( oprtfeelt_v.feeptmlow, feedetail_p, 20 ); feedetail_p += 20;
		MEMCPY_DEBUG( oprtfeelt_v.feeptmupp, feedetail_p, 20 ); feedetail_p += 20;
		MEMCPY_DEBUG( oprtfeelt_v.feecnt, feedetail_p, 10 ); feedetail_p += 10;
		MEMCPY_DEBUG( feedetail_buf, feedetail_p, 6 );
		feedetail_buf[6] = '\0'; feedetail_p += 6;
		oprtfeelt_v.feeprice = atof( feedetail_buf ) / 100;
		MEMCPY_DEBUG( feedetail_buf, feedetail_p, 6 );
		feedetail_buf[6] = '\0'; feedetail_p += 6;
		oprtfeelt_v.feerate = atof( feedetail_buf ) / 100;
		MEMCPY_DEBUG( feedetail_buf, feedetail_p, 6 );
		feedetail_buf[6] = '\0'; feedetail_p += 6;
		oprtfeelt_v.timespec = atof( feedetail_buf ) / 10000;
		MEMCPY_DEBUG( feedetail_buf, feedetail_p, 15 );
		feedetail_buf[15] = '\0'; feedetail_p += 15;
		oprtfeelt_v.feeamnt = atof( feedetail_buf ) / 100;

		vtcp_log("row_id[%ld]consigndate[%ld]paytools[%s]cmtno[%s]srflag[%s]feeptmlow[%s]feeptmupp[%s]feecnt[%s]feeprice[%lf]feerate[%lf]timespec[%lf]feeamnt[%lf]",oprtfeelt_v.row_id,oprtfeelt_v.consigndate,oprtfeelt_v.paytools,oprtfeelt_v.cmtno,oprtfeelt_v.srflag,oprtfeelt_v.feeptmlow,oprtfeelt_v.feeptmupp,oprtfeelt_v.feecnt,oprtfeelt_v.feeprice,oprtfeelt_v.feerate,oprtfeelt_v.timespec,oprtfeelt_v.feeamnt );
		g_reply_int = Mbfeoprtfeelt_Ins( oprtfeelt_v, g_pub_tx.reply );
		if( g_reply_int )
		{
			sprintf( acErrMsg, "�ǼǴ��֧��ҵ���շ���ϸ�����ݿ����ʧ��[%d]", g_reply_int );
			WRITEMSG
			goto ErrExit;
		}
	}

	feedetail_p = mbfetran.singlefeedetail;
	for( i=0; i<oprtfeecl_v.singlefeecnt; i++ )
	{
		memset( &oprtfeelt_v, 0x00, sizeof( struct mbfeoprtfeelt_c ) );

		oprtfeelt_v.row_id = i + 1 + oprtfeecl_v.hvpfeecnt;
		oprtfeelt_v.consigndate = atol( mbfetran.consigndate );
		strcpy( oprtfeelt_v.paytools, "3" );
		MEMCPY_DEBUG( oprtfeelt_v.cmtno, feedetail_p, 3 ); feedetail_p += 3;
		MEMCPY_DEBUG( oprtfeelt_v.feeptmlow, feedetail_p, 20 ); feedetail_p += 20;
		MEMCPY_DEBUG( oprtfeelt_v.feeptmupp, feedetail_p, 20 ); feedetail_p += 20;
		MEMCPY_DEBUG( oprtfeelt_v.feecnt, feedetail_p, 10 ); feedetail_p += 10;
		MEMCPY_DEBUG( feedetail_buf, feedetail_p, 6 );
		feedetail_buf[6] = '\0'; feedetail_p += 6;
		oprtfeelt_v.feeprice = atof( feedetail_buf ) / 100;
		MEMCPY_DEBUG( feedetail_buf, feedetail_p, 6 );
		feedetail_buf[6] = '\0'; feedetail_p += 6;
		oprtfeelt_v.feerate = atof( feedetail_buf ) / 100;
		MEMCPY_DEBUG( feedetail_buf, feedetail_p, 6 );
		feedetail_buf[6] = '\0'; feedetail_p += 6;
		oprtfeelt_v.timespec = atof( feedetail_buf ) / 10000;
		MEMCPY_DEBUG( feedetail_buf, feedetail_p, 15 );
		feedetail_buf[15] = '\0'; feedetail_p += 15;
		oprtfeelt_v.feeamnt = atof( feedetail_buf ) / 100;

		vtcp_log("row_id[%ld]consigndate[%ld]paytools[%s]cmtno[%s]srflag[%s]feeptmlow[%s]feeptmupp[%s]feecnt[%s]feeprice[%lf]feerate[%lf]timespec[%lf]feeamnt[%lf]",oprtfeelt_v.row_id,oprtfeelt_v.consigndate,oprtfeelt_v.paytools,oprtfeelt_v.cmtno,oprtfeelt_v.srflag,oprtfeelt_v.feeptmlow,oprtfeelt_v.feeptmupp,oprtfeelt_v.feecnt,oprtfeelt_v.feeprice,oprtfeelt_v.feerate,oprtfeelt_v.timespec,oprtfeelt_v.feeamnt );
		g_reply_int = Mbfeoprtfeelt_Ins( oprtfeelt_v, g_pub_tx.reply );
		if( g_reply_int )
		{
			sprintf( acErrMsg, "�Ǽǵ���֧��ҵ���շ���ϸ�����ݿ����ʧ��[%d]", g_reply_int );
			WRITEMSG
			goto ErrExit;
		}
	}

	feedetail_p = mbfetran.infofeedetail;
	for( i=0; i<oprtfeecl_v.infofeecnt; i++ )
	{
		memset( &oprtfeelt_v, 0x00, sizeof( struct mbfeoprtfeelt_c ) );

		oprtfeelt_v.row_id = i + 1 + oprtfeecl_v.hvpfeecnt + oprtfeecl_v.singlefeecnt;
		oprtfeelt_v.consigndate = atol( mbfetran.consigndate );
		strcpy( oprtfeelt_v.paytools, "3" );
		MEMCPY_DEBUG( oprtfeelt_v.cmtno, feedetail_p, 3 ); feedetail_p += 3;
		MEMCPY_DEBUG( oprtfeelt_v.srflag, feedetail_p, 1 ); feedetail_p += 1;
		MEMCPY_DEBUG( oprtfeelt_v.feecnt, feedetail_p, 10 ); feedetail_p += 10;
		MEMCPY_DEBUG( feedetail_buf, feedetail_p, 6 );
		feedetail_buf[6] = '\0'; feedetail_p += 6;
		oprtfeelt_v.feeprice = atof( feedetail_buf ) / 100;
		MEMCPY_DEBUG( feedetail_buf, feedetail_p, 6 );
		feedetail_buf[6] = '\0'; feedetail_p += 6;
		oprtfeelt_v.feerate = atof( feedetail_buf ) / 100;
		MEMCPY_DEBUG( feedetail_buf, feedetail_p, 15 );
		feedetail_buf[15] = '\0'; feedetail_p += 15;
		oprtfeelt_v.feeamnt = atof( feedetail_buf ) / 100;

		vtcp_log("row_id[%ld]consigndate[%ld]paytools[%s]cmtno[%s]srflag[%s]feeptmlow[%s]feeptmupp[%s]feecnt[%s]feeprice[%lf]feerate[%lf]timespec[%lf]feeamnt[%lf]",oprtfeelt_v.row_id,oprtfeelt_v.consigndate,oprtfeelt_v.paytools,oprtfeelt_v.cmtno,oprtfeelt_v.srflag,oprtfeelt_v.feeptmlow,oprtfeelt_v.feeptmupp,oprtfeelt_v.feecnt,oprtfeelt_v.feeprice,oprtfeelt_v.feerate,oprtfeelt_v.timespec,oprtfeelt_v.feeamnt );
		g_reply_int = Mbfeoprtfeelt_Ins( oprtfeelt_v, g_pub_tx.reply );
		if( g_reply_int )
		{
			sprintf( acErrMsg, "�Ǽ���Ϣ����֧��ҵ���շ���ϸ�����ݿ����ʧ��[%d]", g_reply_int );
			WRITEMSG
			goto ErrExit;
		}
	}

	/*Ϊ�����ӽ��״�ֵ*/
	set_zd_data( "1201", com_parm_v.val );
	set_zd_data( "1204", "01" );
	set_zd_data( "1205", "2" );
	set_zd_double( "1208", oprtfeecl_v.feeammunt );
	set_zd_data( "120A", "֧��ҵ��������" );
	memset( &com_parm_v, 0x00, sizeof( struct com_parm_c ) );
	g_reply_int = Com_parm_Sel( g_pub_tx.reply, &com_parm_v, "parm_code='YHZBJ' and parm_seqn='1'");
	if( g_reply_int )
	{
		sprintf( acErrMsg, "��ѯ����׼�����˺ų���![%ld]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}
	set_zd_data( "1211", com_parm_v.val );
	set_zd_data( "1214", "01" );
	set_zd_data( "1215", "2" );
	set_zd_double( "1218", oprtfeecl_v.feeammunt );
	set_zd_data( "121A", "֧��ҵ��������" );


OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"�Ǽ�֧��ҵ���շѱ��ĳɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
        if(memcmp(g_pub_tx.reply,"0000",4)==0)
                MEMCPY_DEBUG(g_pub_tx.reply,"T063",4);
	sprintf(acErrMsg,"�Ǽ�֧��ҵ���շѱ���ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
