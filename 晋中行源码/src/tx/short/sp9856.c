/*************************************************
* �� �� ��:  sp9856.c
* ���������� ��Э���Ż����ٴ���ǼǱ���Ϣ
*
* ��    ��:  jane
* ������ڣ� 2003��2��21��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "mo_again_ln_c.h"

int sp9856()
{
	struct	mo_again_ln_c	sMoAgainLn;	/* �ٴ���ǼǱ� */
	int	ret;

	memset( &sMoAgainLn, 0x00, sizeof( struct mo_again_ln_c ) );
	
	get_zd_data( "0580", sMoAgainLn.pact_no );	/* Э���� */

	/* ����ٴ���ǼǱ����Ƿ���ڴ�Э���� */
	ret = Mo_again_ln_Sel( g_pub_tx.reply, &sMoAgainLn, "pact_no='%s'", sMoAgainLn.pact_no );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "�ٴ���ǼǱ��в����ڴ�Э����!" );
                WRITEMSG
                strcpy( g_pub_tx.reply, "B107" );
                goto ErrExit;
  }else if( ret )
       	{
       		sprintf( acErrMsg, "ִ��Mo_again_ln_Sel��![%d]", ret );
                WRITEMSG
                goto ErrExit;
       	}

	/* ��ǰ̨��Ļ��ֵ */
	set_zd_double( "1001", sMoAgainLn.amt );		/* ������ */
	set_zd_long( "0450", sMoAgainLn.reg_date );   /* �Ǽ����� */
	set_zd_long( "0440", sMoAgainLn.to_date );		/* �������� */
	set_zd_double( "0840", sMoAgainLn.rate );		/* ���� */
	vtcp_log("wjwjw[%lf]",sMoAgainLn.rate);
	set_zd_data( "0670", sMoAgainLn.again_ln_type );/* �ٴ������� */

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




