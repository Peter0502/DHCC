/*************************************************
* �� �� ��:  spJ000.c
* ���������� ������˹����ӽ���
*			 ���ʴ��� �������
*			 
* ��    ��:  janeg
* ������ڣ� 2003��8��15��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/
#define  EXTERN
#include <string.h>
#include "public.h"
#include "mbfe_pub.h"
#include "find_debug.h"

int spJ000()
{
	char reply[5];
	struct mbfe_hang_c hang_v;

	memset( &hang_v, 0x00, sizeof( struct mbfe_hang_c ) );

	pub_base_sysinit();

	get_zd_data( "0120", g_pub_tx.reply );
	vtcp_log( "������ʼ���ʧ�� ������[%s]", g_pub_tx.reply );
	
	/*  ��֧���� ������� �˺ź��˺�ID��Ϊ��*/
	if(memcmp(g_pub_tx.reply,"P101",4) == 0) 
	{
		vtcp_log( "������ʼ���ʧ�� ������[%s]", g_pub_tx.reply );
		goto GoodExit;
	}
  
	hang_v.consigndate = g_pub_tx.tx_date;
	strcpy( hang_v.txssno, mbfetran.txssno );
	strcpy( hang_v.odficode, mbfetran.odficode );
	strcpy( hang_v.hang_reply, g_pub_tx.reply );

	sprintf(acErrMsg, "[%ld], [%s], [%s],[%s]",hang_v.consigndate, 
		hang_v.txssno, hang_v.hang_reply, hang_v.odficode);
	WRITEMSG


	sql_rollback();    /*�ع�֮ǰ������*/

	sql_begin();    /*��ʼ�ǼǵǼǹ���ҵ����ʱ�������*/
	g_reply_int = Mbfe_hang_Ins( hang_v, reply );
	if( g_reply_int )
	{
		strcpy( g_pub_tx.reply, reply );
       		sprintf( acErrMsg, "�ǼǵǼǹ���ҵ����ʱ�����![%d]", g_reply_int );
       		WRITEMSG
		sql_rollback();    /*�ع��ǼǵǼǹ���ҵ����ʱ�������*/
	}
	else if( g_reply_int == 0 )
		sql_commit();    /*�ύ�ǼǵǼǹ���ҵ����ʱ�������*/

	sql_begin();    /*��ʼ׼������ʧ�ܵ�����*/

ErrExit:    /*һ���Ǵ������*/
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
	
GoodExit:
    strcpy( g_pub_tx.reply, "0000" );
    sprintf(acErrMsg,"Before OK return: reply [%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
	
}

