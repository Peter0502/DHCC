/*************************************************
* �� �� ��:  sp9803.c
* ���������� �ɹ�Ա�Ż��Թ�Ա����
*
* ��    ��:  jane
* ������ڣ� 2003��1��12�� 
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "com_tel_c.h"

int sp9803()
{
	struct	com_tel_c	sComTel;		/* ��Ա��Ϣ��ṹ */
	int	ret;

	get_zd_data( "0920", g_pub_tx.tel );		/* ��Ա�� */
sprintf(acErrMsg,"tel[%s]",g_pub_tx.tel );
WRITEMSG
	if( strcmp( g_pub_tx.tel, "" ) != 0 )
	{
		ret=Com_tel_Sel( g_pub_tx.reply , &sComTel ,\
			 	"tel='%s'" , g_pub_tx.tel );	
		if( ret==100 )
		{
			sprintf( acErrMsg, "ȡ��Ա��Ϣ���!\
				tel=[%s]", g_pub_tx.tel );
			WRITEMSG
			strcpy( g_pub_tx.reply, "S009");
			goto	ErrExit;
		}
		else if( ret )
		{
			sprintf( acErrMsg, "ִ��Com_tel_Sel����!ret=[%d]", ret );
			WRITEMSG
			goto	ErrExit;
		}

		set_zd_data( "0260", sComTel.name );	/* �������� */
		set_zd_data( "0910", sComTel.br_no );	/* �������� 20070425 */
		set_zd_data( "0700", sComTel.csts );	/* ��Ա״̬ 20100707 */
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
