/*************************************************
* �� �� ��: spC601.c
* ��������: �ͻ�������Ͷ����Ϣ¼��
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

spC601()
{
	struct	cif_req_log_c	sCifReqLog;		/* �ͻ�������־ */
	int 	ret, num;

	memset( &sCifReqLog, 0x00, sizeof( struct cif_req_log_c ) );

    /* ���ݳ�ʼ�� */
    pub_base_sysinit();

    /* ��ǰ̨��Ļȡֵ */
    get_zd_long( "0280", &sCifReqLog.cif_no );				/* �ͻ��� */
 	get_zd_data( "0810", sCifReqLog.request );             	/* �������� */
   	get_zd_data( "0670", sCifReqLog.advice_type );          /* �������� */
   	get_zd_data( "0920", sCifReqLog.req_clerk_no );         /* Ͷ�߶��� */

	/* ���ͻ�������־���Ƿ��Ѵ��ڴ˼�¼ */
   	if( strcmp( sCifReqLog.req_clerk_no, "" ) == 0 )
    {
       	ret = Cif_req_log_Sel( g_pub_tx.reply, &sCifReqLog, \
				"cif_no=%ld and request='%s' and advice_type='%s' and \
				req_clerk_no is NULL", sCifReqLog.cif_no, \
				sCifReqLog.request, sCifReqLog.advice_type );
       	if( ret == 0 )
       	{
			sprintf( acErrMsg, "�ͻ�������־���Ѵ��ڴ˼�¼!" );
            WRITEMSG
            strcpy( g_pub_tx.reply, "C065" );
			goto ErrExit;
		}
		else if( ret != 100 )
       	{
       		sprintf( acErrMsg, "��ȡ�ͻ�������־�쳣![%d]", ret );
       		WRITEMSG
       		goto ErrExit;
       	}
	}
	else
	{
		ret = Cif_req_log_Sel( g_pub_tx.reply, &sCifReqLog, \
				"cif_no=%ld and request='%s' and advice_type='%s' and \
				req_clerk_no='%s'", sCifReqLog.cif_no,\
				sCifReqLog.request, sCifReqLog.advice_type, \
				sCifReqLog.req_clerk_no );
       	if( ret == 0 )
       	{
			sprintf( acErrMsg, "�ͻ�������־���Ѵ��ڴ˼�¼!" );
            WRITEMSG
            strcpy( g_pub_tx.reply, "C065" );
			goto ErrExit;
		}
		else if( ret != 100 )
       	{
       		sprintf( acErrMsg, "��ȡ�ͻ�������־�쳣![%d]", ret );
       		WRITEMSG
       		goto ErrExit;
       	}
	}
	
	/* ��ѯ�ͻ�������־�д˿ͻ��Ŷ�Ӧ�������� */
    num=sql_max_int("cif_req_log","seq_no","cif_no=%ld",sCifReqLog.cif_no);
	if(num<0)
  	{
      	sprintf( acErrMsg, "ִ��sql_max_int����!ret=[%d]", ret );
       	WRITEMSG
       	goto ErrExit;
    }

	/* �Ǽǿͻ�������־ */
    sCifReqLog.seq_no = ++num;						/* ��� */
    strcpy( sCifReqLog.clerk_no, g_pub_tx.tel );	/* ְԱ��� */
    sCifReqLog.date = g_pub_tx.tx_date;				/* ����ʱ�� */

    ret =Cif_req_log_Ins( sCifReqLog, g_pub_tx.reply );
    if( ret == -239 )
    {
       	sprintf( acErrMsg, "�Ǽǿͻ�������־ʧ��!���ظ���¼!" );
       	WRITEMSG
       	strcpy( g_pub_tx.reply, "C063" );
       	goto ErrExit;
    }
	else if ( ret )
    {
       	sprintf( acErrMsg, "ִ��Cif_req_log_Ins����!ret=[%d]", ret );
       	WRITEMSG
       	goto ErrExit;
    }

    /* ����ˮ��־ */
	ret = pub_ins_trace_log();
	if ( ret )
	{
		sprintf( acErrMsg, "����ˮ��־��" );
		WRITEMSG
		goto ErrExit;
	}

	set_zd_int("0480",num);

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
