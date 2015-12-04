/*************************************************
* 文 件 名:  sp9906.c
* 功能描述： 小屏幕缴费号码查询
*
* 作    者:  jake
* 完成日期： 2003年4月30日 
*
* 修改记录： 
* 日   期:
* 修 改 人:
* 修改内容:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "ag_peritemmap_c.h"

int sp9906()
{
	struct ag_peritemmap_c sPerItemMap;	/*个人缴费号码控制表*/
	int ret = 0;
	char cItem_no[13];
	char cUnit_no[5];
  long lMaxitem_no=0;
	memset(cItem_no,0,sizeof(cItem_no));
	memset(cUnit_no,0,sizeof(cUnit_no));
	pub_base_sysinit();
	get_zd_data("0330",cItem_no); 
	get_zd_data("0920",cUnit_no);

	sprintf( acErrMsg, "-----------------结果为:[%s][%s]",cItem_no, cUnit_no );
	WRITEMSG

	memset( &sPerItemMap, 0x00, sizeof(sPerItemMap) );
	ret=Ag_peritemmap_Sel( g_pub_tx.reply , &sPerItemMap ,\
			"item_no = '%s' and unit_no = '%s' and stat ='0'", \
			cItem_no,cUnit_no );
	if( ret != 0 && ret != 100 )
	{
		sprintf( acErrMsg, "执行Ag_peritemmap错误!ret=[%d]", ret );
		WRITEMSG
		goto	ErrExit;
	}
  
		sprintf( acErrMsg, "-----------------结果为:[%s][%s][%s]",sPerItemMap.name ,sPerItemMap.ac_no ,sPerItemMap.id_no );
		WRITEMSG

	set_zd_data( "0260",sPerItemMap.name );
	set_zd_data( "0370",sPerItemMap.ac_no );
	set_zd_data( "0620",sPerItemMap.id_no );
	set_zd_data( "0200",sPerItemMap.id_type);
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
