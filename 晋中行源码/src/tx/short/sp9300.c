/*************************************************
* 文 件 名: sp9300.c
* 功能描述：查询柜员钱箱状态是否正常
*           此柜员此币种
* 作    者: martin
* 完成日期：20061010 
*
* 修改记录： 
* 日   期:
* 修 改 人:
* 修改内容:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "cash_mst_c.h"

int sp9300()
{
	int ret=0;

	struct cash_mst_c sCash_mst;
	
	memset(&sCash_mst,0x00,sizeof(struct cash_mst_c));

	get_zd_data("0210",g_pub_tx.cur_no);				/*币种*/
	get_zd_data("0920",g_pub_tx.tel);					/*操作员*/

	ret = Cash_mst_Sel( g_pub_tx.reply, &sCash_mst, \
			" tel = %s and cur_no = %s and sts ='0' ", \
			g_pub_tx.tel, g_pub_tx.cur_no );
	if( ret )
	{
		strcpy( g_pub_tx.reply, "D189" );
		goto ErrExit;
	}	

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf( acErrMsg, "---->该柜员此币种钱箱状态未开启[%s]", g_pub_tx.reply );
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
