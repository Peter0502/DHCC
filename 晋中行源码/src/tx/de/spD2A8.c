/***************************************************************
* �� �� ��:     spD2A8.c
* ����������	������ȷ��
* ��    ��:     jack
* ������ڣ�    2004��05��13��
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
#include "trace_log_c.h"

spD2A8()
{
	struct	trace_log_c	sTraceLog;
	memset(&sTraceLog,0x00,sizeof(struct trace_log_c));
	g_reply_int=0;

	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();
	get_zd_long("0280",&g_pub_tx.trace_no);

	g_reply_int=Trace_log_Dec_Upd(g_pub_tx.reply,"trace_no=%d and \
		tx_br_no='%s' \
		and (sub_tx_code='D099' or sub_tx_code='D002' or sub_tx_code='A017') \
		and svc_ind<9000 ",g_pub_tx.trace_no,g_pub_tx.tx_br_no);
	if( g_reply_int )
	{
		sprintf( acErrMsg, "ȡ������ˮ�쳣����![%d][%d]",g_reply_int,g_pub_tx.trace_no);
		strcpy( g_pub_tx.reply, "P298" );
		WRITEMSG
		goto ErrExit;
	}

	g_reply_int=Trace_log_Fet_Upd(&sTraceLog,g_pub_tx.reply);
	if( g_reply_int==100 )
	{
		sprintf( acErrMsg, "��ˮ�Ų�����,��ñ���ˮ�Ǵ���![%d]",g_pub_tx.trace_no);
		WRITEMSG
		strcpy( g_pub_tx.reply, "P297" );
		goto ErrExit;
	}
	else if( g_reply_int )
	{
		sprintf( acErrMsg, "ȡ������ˮ�쳣����![%d][%d]",g_reply_int,g_pub_tx.trace_no);
		strcpy( g_pub_tx.reply, "P298" );
		WRITEMSG
		goto ErrExit;
	}

/*	zy��2005.7.27��
	pub_base_strpack(sTraceLog.tel);
	if( strcmp(g_pub_tx.tel,sTraceLog.tel) )
	{
		sprintf( acErrMsg,"ֻ�����ñʽ��׵Ĺ�Ա����ȷ��![%s][%s]",sTraceLog.tel,g_pub_tx.tel);
		strcpy( g_pub_tx.reply, "P299" );
		WRITEMSG
		goto ErrExit;
	}
*/
	if( sTraceLog.saving_notarize[0] == '1' )
	{
		sprintf( acErrMsg, "�ñʴ�����ҵ���Ѿ�ȷ�ι���!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "P300" );
		goto ErrExit;
	}

	strcpy(sTraceLog.saving_notarize,"1");
	set_zd_data("0810","�Ѿ�ȷ��");
	g_reply_int=Trace_log_Upd_Upd(sTraceLog,g_pub_tx.reply);
	if( g_reply_int )
	{
		sprintf( acErrMsg, "ȡ������ˮ�쳣����![%d][%d]",g_reply_int,g_pub_tx.trace_no);
		strcpy( g_pub_tx.reply, "P298" );
		WRITEMSG
		goto ErrExit;
	}

	Trace_log_Clo_Upd();

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"testw��ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"wstudttest��ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
