/*************************************************
* 文 件 名:  sp9850.c
* 功能描述： 由客户经理代码回显客户经理名称
*
* 作    者:  jane
* 完成日期： 2003年2月4日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "cif_mger_inf_c.h"

int sp9850()
{
	struct 	cif_mger_inf_c   sCifMgerInf;		/* 客户经理信息表 */
	int	ret;

	get_zd_char( "0920", g_pub_tx.mdm_code );       /* 经理代码 */

	/* 检查经理代码是否存在 */
       	ret = Cif_mger_inf_Sel( g_pub_tx.reply, &sCifMgerInf, "mang='%s'", g_pub_tx.mdm_code );
       	if( ret==100 )
       	{
                sprintf( acErrMsg, "无此经理代码，请检查!mang=[%s]", g_pub_tx.mdm_code );
                WRITEMSG
                strcpy( g_pub_tx.reply, "C014" );
                goto ErrExit;
       	}else if( ret )
       	{
                sprintf( acErrMsg, "读取客户经理信息异常![%d]", ret );
                WRITEMSG
                strcpy( g_pub_tx.reply, "C015" );
                goto ErrExit;
        }
	
	/* 向前台屏幕赋值 */
	set_zd_data( "0620", sCifMgerInf.mang_name );	/* 客户经理名称 */
	set_zd_data( "0700", sCifMgerInf.mang_type );	/* 客户经理类型 */
        sprintf( acErrMsg, "客户经理类型![%s]", sCifMgerInf.mang_type );
        WRITEMSG		

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

