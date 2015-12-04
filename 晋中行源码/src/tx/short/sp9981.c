/*************************************************
* �� �� ��:  sp9981.c
* ���������� ������ˮ���������ƿ�� 
*
* ��    ��:  jane
* ������ڣ� 2003��8��16�� 
*
* �޸ļ�¼�� 
*   ��   ��: 20060824
*   �� �� ��:ligl
*   �޸�����:
*   ��   ��: 20100607
*   �� �� ��:Han Xichao
*   �޸�����: ���Ӳ�ѯʱ��trace_log���з��ؽ��״���,�˺�,���������Ϣ
update tx_def set bit_map='10000000000000000000000000000000100101000100000000000000000000000000000000000000000000000000000000000000000000000000000000000000' where tx_code='9981';
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "trace_log_c.h"
#include "tx_def_c.h"

int sp9981()
{
	struct	trace_log_c	sTraceLog;	/* ҵ����ˮ */
	struct	tx_def_c	sTx_def;		/* ���׶���ṹ */
	int	ret;
	char   tx_code[5];

	memset( &sTraceLog, 0x00, sizeof( struct trace_log_c ) );
	memset( &sTx_def, 0x00, sizeof( struct tx_def_c ) );
	memset( tx_code, 0x00, sizeof(tx_code ) );

	get_zd_long( "0440", &g_pub_tx.trace_no );	/* ������ˮ�� */
	get_zd_long( "0050", &g_pub_tx.tx_date);
	/* ����������ˮ�Ų�ѯҵ����ˮ�� */
	ret = Trace_log_Sel( g_pub_tx.reply, &sTraceLog, "trace_no=%ld and tx_date=%ld", g_pub_tx.trace_no,g_pub_tx.tx_date );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "�޴���ˮ��!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "S045" );
		goto ErrExit;
   	}else if( ret )
   	{
   		sprintf( acErrMsg, "ִ��Trace_log_Sel��![%d]", ret );
			WRITEMSG
			goto ErrExit;
		}
	vtcp_log( "[%s],[%d]  tx_code=[%s]", __FILE__,__LINE__,sTraceLog.tx_code );
	/**Add by HXC 100607 Begin**/
	memset( tx_code, 0x00, sizeof(tx_code) );
	memcpy(tx_code,sTraceLog.tx_code,4);
	vtcp_log( "[%s],[%d]  tx_code=[%s]", __FILE__,__LINE__,tx_code );
	if(memcmp(tx_code,"5802",4)==0)
		{
			memset( &sTraceLog, 0x00, sizeof( struct trace_log_c ) );
			ret = Trace_log_Sel( g_pub_tx.reply, &sTraceLog, "trace_no=%ld and tx_date=%ld and tx_code='%s' and sub_tx_code='DEWZ'", g_pub_tx.trace_no,g_pub_tx.tx_date,tx_code );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "�޴���ˮ��!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "S045" );
		goto ErrExit;
   	}else if( ret )
   	{
   		sprintf( acErrMsg, "ִ��Trace_log_Sel��![%d]", ret );
		WRITEMSG
		goto ErrExit;
	 }
	}
	if(memcmp(tx_code,"5842",4)==0 || memcmp(tx_code,"5844",4)==0 || memcmp(tx_code,"5846",4)==0)
		{
			memset( &sTraceLog, 0x00, sizeof( struct trace_log_c ) );
			ret = Trace_log_Sel( g_pub_tx.reply, &sTraceLog, "trace_no=%ld and tx_date=%ld and tx_code='%s' and sub_tx_code='XEJZ'", g_pub_tx.trace_no,g_pub_tx.tx_date,tx_code );
	    if( ret == 100 )
	     {
		     sprintf( acErrMsg, "�޴���ˮ��!" );
		     WRITEMSG
		     strcpy( g_pub_tx.reply, "S045" );
		     goto ErrExit;
   	   }
   	  else if( ret )
   	  {
   		 sprintf( acErrMsg, "ִ��Trace_log_Sel��![%d]", ret );
		   WRITEMSG
		   goto ErrExit;
	    }
	  }
	vtcp_log( "[%s],[%d]  tx_code=[%s],ac_no=[%s],amt=[%.2f]", __FILE__,__LINE__,tx_code,sTraceLog.ac_no,sTraceLog.amt);
	set_zd_data( "0360", tx_code);			/* ������ */
	set_zd_data( "0330", sTraceLog.ac_no);			 /* �����˺� */
	set_zd_double( "0420", sTraceLog.amt);			/* ���׷����� */
	/**Add by HXC 100607 End**/
	/* ���ݽ��״����ѯ�˵��ṹ */
	ret = Tx_def_Sel( g_pub_tx.reply, &sTx_def, "tx_code='%s'", tx_code );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "�޴˽��״���!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "S061" );
		goto ErrExit;
   	}else if( ret )
   	{
   		sprintf( acErrMsg, "ִ��Tx_def_Sel��![%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	vtcp_log( "[%s],[%d]  tx_name[%s]", __FILE__,__LINE__,sTx_def.tx_name );
	set_zd_data( "0380", sTx_def.tx_name);			/* �������� */

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
