/*************************************************
* �� �� ��:  sp9822.c
* ���������� ����ʾ�˺Ż��Կͻ����� 
*			
*
* ��    ��:  rob 
* ������ڣ� 2003��1��20�� 
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"

int sp9822()
{
	int	ret;

	get_zd_data( "0310", g_pub_tx.ac_no ); 			/* �ʺ� */
	get_zd_int( "0340",  &g_pub_tx.ac_seqn ); 		/* �˻���� */

	/*������ʾ�ʺź��˺���ŷ����˻����ͺ����ļ�*/
	ret = pub_base_disac_file(g_pub_tx.ac_no,g_pub_tx.ac_seqn,
								g_pub_tx.ac_id_type);
	if( ret )
	{
		sprintf(acErrMsg,"��������ʾ�ʺź��˺���ŷ����˻����ͺ����ļ�����!!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "W010" );
		goto	ErrExit;
	}

	switch(g_pub_tx.ac_id_type[0])
	{
		case '1':
				strcpy(g_pub_tx.name,g_dd_mst.name);
				break;
		case '2':
				strcpy(g_pub_tx.name,g_td_mst.name);
				break;
		default	:
				sprintf(acErrMsg,"�����˻���������!!");
				WRITEMSG
				strcpy( g_pub_tx.reply, "O007" );
				goto	ErrExit;
	}

	set_zd_data( "0250", g_pub_tx.name );			/* �˻����� */

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
