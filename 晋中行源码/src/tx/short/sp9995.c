/*************************************************
* �� �� ��:  sp9995.c
* ���������� �����ŷ�Ʊ��Сƾ֤��
*
* ��    ��:  jack
* ������ڣ� 2003��11��18��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "mdm_attr_c.h"

int sp9995()
{
	int	ret;

	pub_base_sysinit();

	strcpy( g_pub_tx.note_type, "142" );
	get_zd_data("0610",g_pub_tx.beg_note_no); 

	ret=pub_com_ChkNoteMin( g_pub_tx.note_type, g_pub_tx.beg_note_no, \
				 g_pub_tx.tel);
	if( ret )
	{
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

