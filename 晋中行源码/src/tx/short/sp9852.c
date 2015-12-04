/*************************************************
* 文 件 名:  sp9852.c
* 功能描述： 由客户号回显客户关系代码及关联客户名称
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
#include "cif_cif_rel_c.h"

int sp9852()
{
	struct 	cif_basic_inf_c   	sCifBasicInf;		/* 客户基本信息表 */
	struct 	cif_cif_rel_c   	sCifCifRel;		/* 客户-客户关系表 */
	int	ret;

	memset ( &sCifBasicInf, 0x00, sizeof( struct cif_basic_inf_c ) );
	memset ( &sCifCifRel, 0x00, sizeof( struct cif_cif_rel_c ) );
	
	get_zd_long( "0290", &g_pub_tx.cif_no );       	/* 客户号 */
	get_zd_long( "0280", &g_pub_tx.ac_id );       	/* 关联客户号 */

	/* 检查客户号是否存在 */
       	ret = Cif_basic_inf_Sel( g_pub_tx.reply, &sCifBasicInf, "cif_no=%ld", g_pub_tx.ac_id );
       	if( ret==100 )
       	{
                sprintf( acErrMsg,"无此客户号，请检查!cif_no=[%ld]", g_pub_tx.ac_id );
                WRITEMSG
                strcpy( g_pub_tx.reply, "C007" );
                goto ErrExit;
       	}
        else if( ret )
       	{
                sprintf( acErrMsg, "读取客户基本信息异常![%d]", ret );
                WRITEMSG
                strcpy( g_pub_tx.reply, "C006" );
                goto ErrExit;
        }
	
	/* 检查客户关系是否存在 */
       	ret = Cif_cif_rel_Sel( g_pub_tx.reply, &sCifCifRel, "cif_no=%ld and rel_cid_no=%ld", \
       				g_pub_tx.cif_no, g_pub_tx.ac_id );
       	if( ret==100 )
       	{
                sprintf( acErrMsg, "客户-客户关系表中无相关记录!" );
                WRITEMSG
                strcpy( g_pub_tx.reply, "C046" );
                goto ErrExit;
       	}else if( ret )
       	{
                sprintf( acErrMsg, "读取客户-客户关系表异常![%d]", ret );
                WRITEMSG
                goto ErrExit;
        }
	
	/* 向前台屏幕赋值 */
	set_zd_data( "0260", sCifBasicInf.name );		/* 关联客户名称 */
	set_zd_data( "0230", sCifCifRel.rel_code );		/* 客户关系代码 */

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

