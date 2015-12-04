/*************************************************
* 文 件 名:  sp9995.c
* 功能描述： 检查电信发票最小凭证号
*
* 作    者:  jack
* 完成日期： 2003年11月18日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
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

