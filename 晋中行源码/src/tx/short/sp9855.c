/*************************************************
* �� �� ��:  sp9855.c
* ���������� �ɲ�Ʒ��Ż����������ͣ��ڲ���Ʒ������
*
* ��    ��:  jane
* ������ڣ� 2003��2��20��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"

int sp9855()
{
	struct	in_parm_c sInParm;	/* �ڲ���Ʒ������ */
	int	ret;

	memset( &sInParm, 0x00, sizeof( struct in_parm_c ) );
	
	get_zd_data( "0890", g_pub_tx.prdt_code );		/* ��Ʒ��� */

	/* ����Ʒ����Ƿ���� */
	ret = In_parm_Sel( g_pub_tx.reply, &sInParm, "prdt_no='%s'", g_pub_tx.prdt_code );
	if( ret == 100 )
        {
		sprintf( acErrMsg, "�޴˲�Ʒ���![%s]", g_pub_tx.prdt_code );
            	WRITEMSG
            	strcpy( g_pub_tx.reply, "A028" );
		goto ErrExit;
        }else if( ret )
        {
            	sprintf( acErrMsg, "ִ��In_parm_Sel��![%d]", ret );
            	WRITEMSG
            	goto ErrExit;
        }

	/* ��ǰ̨��Ļ��ֵ */
	if( sInParm.term_type[0] == 'Y' )			/* �������� */
		set_zd_data( "0210", "��" );
	else if( sInParm.term_type[0] == 'M' )
		set_zd_data( "0210", "��" );
	else if( sInParm.term_type[0] == 'Q' )
		set_zd_data( "0210", "��" );
	else	set_zd_data( "0210", "��" );

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




