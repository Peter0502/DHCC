/*************************************************
* 文 件 名:  sp9904.c
* 功能描述： 查询(上级)单位编号信息
*
* 作    者:  jane
* 完成日期： 2003年4月29日
*
* 修改记录： 
* 日   期:
* 修 改 人:
* 修改内容:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "ag_unitinfo_c.h"

int sp9904()
{
	struct ag_unitinfo_c sUnitInfo;	/*单位信息表*/
	int ret = 0;
	char cUnit_no[5];
	char cFlag[2];

	memset(cUnit_no,0,sizeof(cUnit_no));
	memset(cFlag,0x0,sizeof(cFlag));

	pub_base_sysinit();
	get_zd_data( "0350",cUnit_no);

	if ( (strlen( cUnit_no ) == 0) || (cUnit_no[0] == '0') )
	{
		goto OkExit;
	}

	ret=Ag_unitinfo_Sel( g_pub_tx.reply , &sUnitInfo ,\
			 "unit_no = '%s' and unit_stat = '0'",cUnit_no);
	if( ret==100 )
	{
		sprintf( acErrMsg, "没有查询到该单位编号对应记录!\
			unit_no=[%s]", cUnit_no );
		WRITEMSG
		strcpy( g_pub_tx.reply, "H009" );
		goto	ErrExit;
	}
	else if( ret )
	{
		sprintf( acErrMsg, "执行Ag_unitinfo错误!ret=[%d]", ret );
		WRITEMSG
		goto	ErrExit;
	}

	set_zd_data( "0270",sUnitInfo.unit_name );

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
