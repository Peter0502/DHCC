/*************************************************
* 文 件 名:  sp9848.c
* 功能描述： 由客户ID(0290)回显客户名称(0250)
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
#include "cif_basic_inf_c.h"

int sp9848()
{
	struct 	cif_basic_inf_c   sCifBasicInf;		/* 客户基本信息表 */
	int	ret;

	get_zd_long( "0290", &g_pub_tx.cif_no );       	/* 客户ID */

	/* 检查客户号是否存在 */
	if(g_pub_tx.cif_no)
	{
       	ret=Cif_basic_inf_Sel(g_pub_tx.reply,&sCifBasicInf, \
							  "cif_no=%ld",g_pub_tx.cif_no );
       	if( ret==100 )
       	{
            sprintf(acErrMsg,"无此客户号，请检查!cif_no=[%ld]", \
					g_pub_tx.cif_no );
            WRITEMSG
            strcpy( g_pub_tx.reply, "C007" );
            goto ErrExit;
       	}else if( ret )
       	{
            sprintf( acErrMsg, "读取客户基本信息异常![%d]", ret );
            WRITEMSG
            strcpy( g_pub_tx.reply, "C006" );
            goto ErrExit;
        }
	}
	else{
        sprintf(acErrMsg,"客户号未输入!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"C075");
		goto ErrExit;
	}

	/* 向前台屏幕赋值 */
	set_zd_data( "0250", sCifBasicInf.name );

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

