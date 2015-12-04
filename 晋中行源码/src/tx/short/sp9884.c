/*************************************************
* 文 件 名:  sp9884.c
* 功能描述： 根据票据种类和票据号码判断同城登记簿中是否已经存在
*
* 作    者:  rob
* 完成日期： 2003年4月21日 
*
* 修改记录： 
* 日   期:
* 修 改 人:
* 修改内容:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "mo_city_c.h"

int sp9884()
{
    int ret;
	struct mo_city_c mo_city_c;
   
	memset(&mo_city_c, 0x0, sizeof(struct mo_city_c));

    get_zd_data("0670",mo_city_c.bill_type);
	get_zd_data("0580",mo_city_c.bill_no);

	sprintf(acErrMsg,"bill_type=[%s], bill_no=[%s]",mo_city_c.bill_type,
			mo_city_c.bill_no);
    WRITEMSG

	ret = Mo_city_Sel(g_pub_tx.reply,&mo_city_c,"bill_type='%s' and \
		  bill_no = '%s'",mo_city_c.bill_type,mo_city_c.bill_no);
    if (ret != 0 && ret != 100)
	{
		sprintf(acErrMsg,"查询同城登记簿错误!! [%d]",ret);
		WRITEMSG
		goto ErrExit;
	}
	else if (ret == 0)
	{
		sprintf(acErrMsg,"该票据己经存在!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"D161");
		goto ErrExit;
	}

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"success !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"fail !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
