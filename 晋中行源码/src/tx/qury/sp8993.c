/*************************************************
* ÎÄ ¼ş Ãû:  sp8993.c
* ¹¦ÄÜÃèÊö£º ÖÕÖ¹±¨±í½»Ò×¯
*
* ×÷    Õß:  rob
* Íê³ÉÈÕÆÚ£º 
*
* ĞŞ¸Ä¼ÇÂ¼£º
* ÈÕ   ÆÚ:
* ĞŞ ¸Ä ÈË:
* ĞŞ¸ÄÄÚÈİ:
*************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define EXTERN
#include "public.h"
int ret;

int sp8993()
{
	pub_base_sysinit();
	ERR_DEAL

GoodExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before bad return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
