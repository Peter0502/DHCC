/*************************************************
* �� �� ��:  sp9854.c
* ���������� �ɿͻ��ź���Ż��Խ������ݺ�Ͷ�߶��󣨿ͻ�������Ͷ����Ϣά����
*
* ��    ��:  jane
* ������ڣ� 2003��2��17��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "cif_req_log_c.h"

int sp9854()
{
	struct	cif_req_log_c	sCifReqLog;		/* �ͻ�������־ */
	int	ret;

	memset( &sCifReqLog, 0x00, sizeof( struct cif_req_log_c ) );
	
	get_zd_long( "0280", &g_pub_tx.cif_no );	/* �ͻ��� */
	get_zd_int( "0480", &g_pub_tx.ac_seqn );	/* ��� */

	/* ���ͻ�������־���Ƿ���� */
       	ret = Cif_req_log_Sel( g_pub_tx.reply, &sCifReqLog, "cif_no=%ld and seq_no=%d", \
        			g_pub_tx.cif_no, g_pub_tx.ac_seqn );
        if( ret == 100 )
        {
		sprintf( acErrMsg, "��ѯ�ͻ�������־���޼�¼!" );
		WRITEMSG
		sprintf( g_pub_tx.reply, "C064" );
		goto ErrExit;
	}else if( ret )
        {
            	sprintf( acErrMsg, "��ȡ�ͻ�������־�쳣![%d]", ret );
            	WRITEMSG
            	goto ErrExit;
        }
	
	/* ��ǰ̨��Ļ��ֵ */
	set_zd_data( "0810", sCifReqLog.request );             	/* �������� */
    	set_zd_data( "0920", sCifReqLog.req_clerk_no );         /* Ͷ�߶��� */

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



