/***************************************************************
* �� �� ��: spJ803.c
* �����������ռ�͸֧��Ϣ��������
*			�Ǽ��ռ�͸֧��Ϣ���ܱ�
*			�Ǽ��ռ�͸֧��Ϣ��ϸ��
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

spJ803()
{
	int i;
	char *accrualinfo_p;
	char accrualinfo_buf[30];

	struct mbfeoverdraftcl_c overdraftcl_v;
	struct mbfeoverdraftlt_c overdraftlt_v;
	struct com_parm_c com_parm_v;

	memset( &overdraftcl_v, 0x00, sizeof( struct mbfeoverdraftcl_c ) );
	memset( &com_parm_v, 0x00, sizeof( struct com_parm_c ) );

	overdraftcl_v.consigndate = atol( mbfetran.consigndate );
	strcpy( overdraftcl_v.rdficenter, mbfetran.rdficenter );
	strcpy( overdraftcl_v.osdficode, mbfetran.osdficode );
	strcpy( overdraftcl_v.rdficode, mbfetran.osdficode );
	overdraftcl_v.accrualamnt = atof( mbfetran.accrualamnt ) / 100;
	g_reply_int = Com_parm_Sel( g_pub_tx.reply, &com_parm_v, "parm_code='DEKM' and parm_seqn='5'");
	if( g_reply_int )
	{
		sprintf( acErrMsg, "��ѯ����ռ�͸֧��Ϣ��Ŀ����![%ld]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}
	strcpy( overdraftcl_v.accruacno, com_parm_v.val );
	overdraftcl_v.printno = 0;

	g_reply_int = Mbfeoverdraftcl_Ins( overdraftcl_v, g_pub_tx.reply );
	if( g_reply_int )
	{
		sprintf( acErrMsg, "�Ǽ��ռ�͸֧��Ϣ���ܱ����ݿ����ʧ��[%d]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

	for( i=0; i<atol( mbfetran.number ); i++ )
	{
		memset( &overdraftlt_v, 0x00, sizeof( struct mbfeoverdraftlt_c ) );

		overdraftlt_v.row_id = i + 1;
		overdraftlt_v.consigndate = atol( mbfetran.consigndate );
		MEMCPY_DEBUG( overdraftlt_v.accrualtime, accrualinfo_p, 6 ); accrualinfo_p += 6;
		MEMCPY_DEBUG( accrualinfo_buf, accrualinfo_p, 22 );
		accrualinfo_buf[22] = '\0'; accrualinfo_p += 22;
		overdraftlt_v.overdraftamnt = atof( accrualinfo_buf ) / 100;
		MEMCPY_DEBUG( accrualinfo_buf, accrualinfo_p, 6 );
		accrualinfo_buf[6] = '\0'; accrualinfo_p += 6;
		overdraftlt_v.rate = atof( accrualinfo_buf ) / 10000;
		MEMCPY_DEBUG( accrualinfo_buf, accrualinfo_p, 22 );
		accrualinfo_buf[22] = '\0'; accrualinfo_p += 22;
		overdraftlt_v.accrualamnt = atof( accrualinfo_buf ) / 100;

		g_reply_int = Mbfeoverdraftlt_Ins( overdraftlt_v, g_pub_tx.reply );
		if( g_reply_int )
		{
			sprintf( acErrMsg, "�Ǽ��ռ�͸֧��Ϣ��ϸ�����ݿ����ʧ��[%d]", g_reply_int );
			WRITEMSG
			goto ErrExit;
		}
	}

	/*Ϊ�����ӽ��״�ֵ*/
	set_zd_data( "1201", com_parm_v.val );
	set_zd_data( "1204", "01" );
	set_zd_data( "1205", "2" );
	set_zd_double( "1208", overdraftcl_v.accrualamnt );
	set_zd_data( "120A", "����ռ�͸֧��Ϣ" );
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
	set_zd_double( "1218", overdraftcl_v.accrualamnt );
	set_zd_data( "121A", "����ռ�͸֧��Ϣ" );


OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"�����ռ�͸֧��Ϣ���ĳɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
        if(memcmp(g_pub_tx.reply,"0000",4)==0)
                MEMCPY_DEBUG(g_pub_tx.reply,"T063",4);
	sprintf(acErrMsg,"�����ռ�͸֧��Ϣ����ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
