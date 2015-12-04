/*************************************************
* �� �� ��: spC602.c
* ��������: �ͻ�������Ͷ����Ϣά��
* ��    ��: andy
* ������ڣ�2004��2��19��
* �޸ļ�¼��
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
*************************************************/
#include "stdio.h"
#define EXTERN
#include "public.h"
#include "cif_req_log_c.h"

spC602()
{
	struct	cif_req_log_c	sCifReqLog;		/* �ͻ�������־ */
	char	cOptcode[2];					/* ������ */
	char	cRequest[41];					/* �������� */
	char	cReqClerkNo[5];					/* Ͷ�߶��� */
	char	comm[100];						/* ɾ�����¼��where���� */
	int 	ret;
 	
	memset( &sCifReqLog, 0x00, sizeof( struct cif_req_log_c ) );
	memset( cOptcode, 0x00, sizeof( cOptcode ) );
	memset( cRequest, 0x00, sizeof( cRequest ) );
	memset( cReqClerkNo, 0x00, sizeof( cReqClerkNo ) );
	memset( comm, 0x00, sizeof( comm ) );

        /* ���ݳ�ʼ�� */
        pub_base_sysinit();

        /* ��ǰ̨��Ļȡֵ */
        get_zd_data( "0670", cOptcode );  			/* ������: 1�޸� 2ɾ�� */
        get_zd_long( "0280", &sCifReqLog.cif_no );		/* �ͻ��� */
        get_zd_long( "0480", &sCifReqLog.seq_no );		/* ��� */
    	get_zd_data( "0810", cRequest );  			/* �������� */
    	get_zd_data( "0920", cReqClerkNo );             	/* Ͷ�߶��� */
       	
       	/* ������޸Ĳ��� */
       	if( cOptcode[0] == '1' )
       	{
       		/* ���ͻ�������־ */
		ret = Cif_req_log_Dec_Upd( g_pub_tx.reply , "cif_no=%ld and seq_no=%d", \
						sCifReqLog.cif_no, sCifReqLog.seq_no );
		if ( ret )
		{
			sprintf( acErrMsg, "ִ��Cif_req_log_Dec_Upd��[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}

		/* ȡ����¼ */
		ret = Cif_req_log_Fet_Upd( &sCifReqLog, g_pub_tx.reply );
		if ( ret == 100 )
		{
			sprintf( acErrMsg, "��ѯ�ͻ�������־���޼�¼!" );
			WRITEMSG
			sprintf( g_pub_tx.reply, "C064" );
			goto ErrExit;
		}else if( ret )
		{
			sprintf( acErrMsg, "ִ��Cif_req_log_Fet_Upd��[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}

		/* �޸Ŀͻ�������־ */
		strcpy( sCifReqLog.request, cRequest );
		strcpy( sCifReqLog.req_clerk_no, cReqClerkNo );
		
		ret = Cif_req_log_Upd_Upd( sCifReqLog, g_pub_tx.reply );
		if ( ret )
		{
			sprintf( acErrMsg, "�޸Ŀͻ�������־��[%d]", ret );
			WRITEMSG
			strcpy( g_pub_tx.reply, "C074" );
			goto ErrExit;	
		}
		
		Cif_req_log_Clo_Upd();
	}
	else		/* �����ɾ������ */
        {
        	/* ׼��ɾ�����¼��where���� */
        	sprintf( comm, "cif_no=%ld and seq_no=%d and request='%s' \
        		and req_clerk_no='%s'",sCifReqLog.cif_no, sCifReqLog.seq_no, \
        		cRequest, cReqClerkNo );

        	ret = deldb( "cif_req_log", comm );
        	if( ret )
        	{
        		sprintf( acErrMsg, "ɾ�����¼��[%d]", ret );
			WRITEMSG
			goto ErrExit;
        	}
	}
	
        /* ����ˮ��־ */
	ret = pub_ins_trace_log();
	if ( ret )
	{
		sprintf( acErrMsg, "����ˮ��־��" );
		WRITEMSG
		goto ErrExit;
	}

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
    	return 1;
}
