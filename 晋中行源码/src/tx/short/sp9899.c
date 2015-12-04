/*************************************************
* 文 件 名:  sp9899.c
* 功能描述： 查询客户ID状态
*
* 作    者:  rob
* 完成日期： 2003年2月3日
*
* 修改记录： 
* 日   期:
* 修 改 人:
* 修改内容:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "cif_basic_inf_c.h"


int sp9899()
{
	int ret = 0;
	long cif_no;

	struct	cif_basic_inf_c	g_cif_basic_inf;
	memset(&g_cif_basic_inf,0x00,sizeof(struct cif_basic_inf_c));

	/*初始化*/
	pub_base_sysinit();

	/*取值*/
	get_zd_long("0280",&cif_no);

	/*流程*/
	ret=Cif_basic_inf_Sel( g_pub_tx.reply , &g_cif_basic_inf ,
			 				"cif_no=%ld",cif_no);
	if( ret==100 )
	{
		sprintf(acErrMsg,"不存在该客户号!!cif_no=[%ld]",cif_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C007");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf( acErrMsg, "查询客户信息表异常!!ret=[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	set_zd_data( "0700",g_cif_basic_inf.sts);

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
