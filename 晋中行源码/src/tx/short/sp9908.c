/*************************************************
* �� �� ��:    sp9908.c
* ����������   
*              ������ˮ����ʾ�˺š����������
*
* ��    ��:    
* ������ڣ�   2003��05��13��
* �޸ļ�¼��   
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#define EXTERN
#include "public.h"
#include "trace_log_c.h"
	
sp9908()  
{ 		
	struct  trace_log_c		sTraceLog;
	struct  dd_mst_c		sDdMst;
	char	name[51];
	int ret=0;
	
	/** ���ݳ�ʼ�� **/
	memset( &sTraceLog, 0x00, sizeof( struct trace_log_c ) );
	memset( &sDdMst, 0x00, sizeof( struct dd_mst_c ) );
	
	/** ��ʼ��ȫ�ֱ��� **/
	pub_base_sysinit();
	
	/** ȡֵ����ֵ **/	
	get_zd_long( "0280" , &g_pub_tx.trace_no );	/* ��ˮ�� */

	ret= Trace_log_Sel( g_pub_tx.reply, &sTraceLog, "trace_no=%d and \
	 tx_br_no='%s' and \
	 (sub_tx_code='D099' or sub_tx_code='D002' or sub_tx_code='A017') and  \
	 svc_ind<9000 " ,g_pub_tx.trace_no,g_pub_tx.tx_br_no);
	if( ret == 100 )
	{
		sprintf( acErrMsg, "��ˮ�Ų�����,��ñ���ˮ�Ǵ���![%ld]",g_pub_tx.trace_no);
		WRITEMSG
		strcpy( g_pub_tx.reply, "P297" );
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf( acErrMsg, "ȡ������ˮ�쳣����![%d][%d]",ret,g_pub_tx.trace_no);
		strcpy( g_pub_tx.reply, "P298" );
		WRITEMSG
		goto ErrExit;
	}

	/* jane��2003.7.27��
	pub_base_strpack(sTraceLog.tel);
	if( strcmp(g_pub_tx.tel,sTraceLog.tel) )
	{
		sprintf( acErrMsg,"ֻ�����ñʽ��׵Ĺ�Ա����ȷ��![%s][%s]",sTraceLog.tel,g_pub_tx.tel);
		strcpy( g_pub_tx.reply, "P299" );
		WRITEMSG
		goto ErrExit;
	}*/

/**
	if( sTraceLog.saving_notarize[0] == '1' )
	{
		sprintf( acErrMsg, "�ñʴ�����ҵ���Ѿ�ȷ�ι���!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "P300" );
		goto ErrExit;
	}
**/
	if( sTraceLog.saving_notarize[0]=='1' )
		set_zd_data( "0810", "�Ѿ�ȷ��" );
	else
		set_zd_data( "0810", "δȷ��" );

	ret= Dd_mst_Sel( g_pub_tx.reply, &sDdMst, "ac_id=%d and  ac_seqn=%d",sTraceLog.ac_id,sTraceLog.ac_seqn);
	if( ret )
	{
		sprintf( acErrMsg, "ȡ�������ļ��쳣����![%d][%d][%d]",ret,sTraceLog.ac_id,sTraceLog.ac_seqn);
		strcpy( g_pub_tx.reply, "W015" );
		WRITEMSG
		goto ErrExit;
	}

	/* ������� */
	set_zd_data( "0300", sTraceLog.ac_no );	/* ��/���� */	
	set_zd_long( "0340", sTraceLog.ac_seqn );/* �˺���� */
	set_zd_double( "0400", sTraceLog.amt );	/* ����� */
	set_zd_data( "0250", sDdMst.name );		/* ���� */
	/*set_zd_data("0920",sTraceLog.tel);*//*��Ա*/
	
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
