/*************************************************************
* ÎÄ ¼ş Ãû:  sp9948.c
* ¹¦ÄÜÃèÊö£º ´ó¶îÖ§¸¶¿ì½İ²éÑ¯
*			 ÒµÎñÊÕ·Ñ»ã×ÜĞÅÏ¢
*
* ×÷    Õß:  robº
* Íê³ÉÈÕÆÚ£º 2003Äê7ÔÂ15ÈÕ
*
* ĞŞ¸Ä¼ÇÂ¼£º 
*   ÈÕ   ÆÚ:
*   ĞŞ ¸Ä ÈË:
*   ĞŞ¸ÄÄÚÈİ:
*************************************************************/

#define EXTERN
#include "public.h"
#include "mbfe_pub.h"
#include "com_parm_c.h"

int sp9948()
{
	long year_ld, month_ld;
	struct mbfeoprtfeecl_c oprtfeecl_v;
	struct com_parm_c com_parm_v;

	memset( &oprtfeecl_v, 0x00, sizeof( struct mbfeoprtfeecl_c ) );
	memset( &com_parm_v, 0x00, sizeof( struct com_parm_c ) );

	get_zd_long( "0340", &year_ld );
	get_zd_long( "0350", &month_ld );

	g_reply_int = Mbfeoprtfeecl_Sel( g_pub_tx.reply, &oprtfeecl_v, "consigndate between '%ld' and '%ld'", 
		year_ld*10000+month_ld*100, year_ld*10000+month_ld*100+99 );
	if( g_reply_int == 100 )
	{
		sprintf( acErrMsg, "Ã»ÓĞ¸ÃÔÂ·İµÄÊÕ·ÑĞÅÏ¢!" );
		MBFEWRITEMSG
		goto ErrExit;
	}
	else if( g_reply_int )
	{
		sprintf( acErrMsg, "²éÑ¯Ö§¸¶ÒµÎñÊÕ·Ñ»ã×Ü±í³ö´í![%ld]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

	set_zd_data( "0300", oprtfeecl_v.feeacno );
	set_zd_double( "0390", oprtfeecl_v.hvpfeeamt );
	set_zd_double( "0400", oprtfeecl_v.singlefeeamt );
	set_zd_double( "0410", oprtfeecl_v.infofeeamt );
	set_zd_double( "0420", oprtfeecl_v.feeammunt );
	set_zd_long( "0440", oprtfeecl_v.consigndate );
	set_zd_long( "0450", oprtfeecl_v.feebgdate );
	vtcp_log("bgdate[%ld]eddate[%ld]",oprtfeecl_v.feebgdate,oprtfeecl_v.feeeddate);
	set_zd_long( "0460", oprtfeecl_v.feeeddate );
	set_zd_long( "0500", oprtfeecl_v.printno+1 );
	set_zd_long( "0530", oprtfeecl_v.hvpfeecnt );
	set_zd_long( "0540", oprtfeecl_v.singlefeecnt );
	set_zd_long( "0550", oprtfeecl_v.infofeecnt );

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
