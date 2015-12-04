/*************************************************
* 文 件 名:  sp9813.c
* 功能描述： 根据显示账号取得客户类型
*
* 作    者:  lance
* 完成日期： 2003年1月17日 
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"

int sp9813()
{			
	int ret=0;
	long cif_no;
	char cif_type[2];
	
	memset (&g_mdm_ac_rel, 0x00, sizeof(struct mdm_ac_rel_c));
	
	/* 根据显示账号查询介质与账户关系信息 */ 
	get_zd_data( "0300", g_pub_tx.ac_no );       /* 帐号 */

	ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &g_mdm_ac_rel, "ac_no='%s'",\
				g_pub_tx. ac_no );
	if( ret==100 )
	{
		sprintf(acErrMsg,"介质与账户关系信息中无此卡号或账号，请检查!\
			[%s]", g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L014");
		goto ErrExit;
	}else if( ret )
		{
			sprintf(acErrMsg,"读取介质与账户关系信息异常![%s]",\
				g_pub_tx.reply);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L015");
			goto ErrExit;
		}
	sprintf(acErrMsg,"根据显示账号查询其介质状态 PASS!");
	WRITEMSG  

	/* 调用函数 */
	
	ret = pub_cif_CifCheck ( g_mdm_ac_rel.id_type ,g_mdm_ac_rel.id_no ,\
				&cif_no , cif_type );
	if( ret )
		{
			sprintf(acErrMsg,"调用函数pub_cif_CifCheck错误!");
			WRITEMSG
			goto ErrExit;	
		}

	set_zd_data( "0690", cif_type );      /* 客户类型 */

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
