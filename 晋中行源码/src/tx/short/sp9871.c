/*************************************************
* �� �� ��:  sp9871.c
* ���������� ����ˮ�Ż�����ˮ��־��Ϣ
*
* ��    ��:  jane
* ������ڣ� 2003��3��25��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "trace_log_c.h"

int sp9871()
{
	struct	trace_log_c	sTraceLog;	/* ҵ����ˮ�� */
	long	lNoteNum;			/* ƾ֤���� */
	int	ret;

	memset( &sTraceLog, 0x00, sizeof( struct trace_log_c ) );
	
	get_zd_long( "0520", &sTraceLog.trace_no );	/* ��ˮ�� */

	/* ���ҵ����ˮ�����Ƿ���ڴ���ˮ�� */
	ret = Trace_log_Sel( g_pub_tx.reply, &sTraceLog, "trace_no=%ld", sTraceLog.trace_no );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "ҵ����ˮ���в����ڴ���ˮ��!" );
                WRITEMSG
                strcpy( g_pub_tx.reply, "N067" );
                goto ErrExit;
       	}else if( ret )
       	{
       		sprintf( acErrMsg, "ִ��Trace_log_Sel��![%d]", ret );
                WRITEMSG
                goto ErrExit;
       	}

	lNoteNum = (long)sTraceLog.amt;	/* �˴������ƾ֤���� */	

	/* ��ǰ̨��Ļ��ֵ */
	set_zd_data( "0300", sTraceLog.ac_no );	/* �˺� */
	set_zd_data( "0230", sTraceLog.note_type );	/* ƾ֤���� */
	set_zd_data( "0580", sTraceLog.note_no );	/* ƾ֤�� */
	set_zd_long( "0530", lNoteNum );		/* ƾ֤���� */
	set_zd_data( "0920", sTraceLog.tel );		/* ����Ա */

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




