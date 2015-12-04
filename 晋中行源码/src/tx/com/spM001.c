/*************************************************
* �� �� ��:  spM001.c
* ���������� �ֽ������ӽ��ס�
*
* ��    ��:  andy
* ������ڣ� 2004��1��12��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "cash_mst_c.h"

int spM001()
{
	int	ret;
	
	/* ϵͳ��ʼ������ʼ��g_pub_tx)���ݳ�ʼ�� */
	init_pub_tx( );	

	if( !strlen(g_pub_tx.brf) )
	strcpy( g_pub_tx.brf,"�����ֽ�");

	ret=pub_acct_cash();
	if( ret )
	{
		sprintf(acErrMsg,"�����ֽ��ո�����ʧ��!");
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
static int init_pub_tx( )
{
	pub_base_sysinit();

vtcp_log("TRACE [%d]",g_pub_tx.trace_no );
	
	get_zd_data( "1181" , g_pub_tx.cur_no );	/* ���� */ 
	get_zd_data( "1182", g_pub_tx.note_type );	/* ƾ֤���� */
	get_zd_data( "1183", g_pub_tx.beg_note_no );	/* ƾ֤���� */
	get_zd_data( "1183", g_pub_tx.end_note_no );	/* ƾ֤���� */
	get_zd_double( "1184", &g_pub_tx.tx_amt1 );	/* ������ */
	sprintf(acErrMsg,"Cur[%s] [%.2lf]", g_pub_tx.cur_no,g_pub_tx.tx_amt1 );
	WRITEMSG
	get_zd_data( "1185", g_pub_tx.cash_code );		/* �ֽ������ */
	get_zd_data( "1187", g_pub_tx.brf );		/* ժҪ */
	get_zd_data( "1188", g_pub_tx.name );		/* �Է��˺� */
	pub_base_old_acno(g_pub_tx.name);
vtcp_log("�Է��ʺ�Ϊ:[%s]",g_pub_tx.name);
	strcpy( g_pub_tx.add_ind, "1" ); 	/**�ֽ�����**/
	strcpy( g_pub_tx.ct_ind, "1" ); 	/**��ת��־**/
	strcpy(g_pub_tx.hst_ind,"1");

	return	0;
}
