/*************************************************
* 文 件 名:  sp9842.c
* 功能描述： 缴费号码对应信息查询
*
* 作    者:  jack
* 完成日期： 2003年1月30日 
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

int sp9842()
{
	struct ag_peritemmap_c sPerItemMap;	/*个人缴费号码控制表*/
	int ret = 0;
	char cPer_no[13];
	char cUnit_no[5];
	char cItem[21];
	char proc_flag[2];

	memset(cPer_no,0,sizeof(cPer_no));
	memset(cUnit_no,0,sizeof(cUnit_no));
	memset(cItem,0,sizeof(cItem));
	memset(proc_flag,0x0,sizeof(proc_flag));

	pub_base_sysinit();
	get_zd_data("0590",cPer_no); 
	get_zd_data("0360",cUnit_no);
	get_zd_data("0630",cItem);
	get_zd_data("0700",proc_flag);

	ret=Ag_peritemmap_Sel( g_pub_tx.reply , &sPerItemMap ,\
			 "per_no = '%s' and unit_no = '%s' and item_no = '%s' and stat ='0'",cPer_no,cUnit_no,cItem);
	if( ret==100 )
	{
		if (proc_flag[0] == '0')
		{
			goto	OkExit;
		}
		sprintf( acErrMsg, "没有查询到该个人缴费号码对应记录!\
			per_no=[%s]", cPer_no );
		WRITEMSG
		strcpy( g_pub_tx.reply, "H010" );
		goto	ErrExit;
	}
	else if( ret )
	{
		sprintf( acErrMsg, "执行Ag_peritemmap错误!ret=[%d]", ret );
		WRITEMSG
		goto	ErrExit;
	}

	if (proc_flag[0] == '0')
	{
		sprintf( acErrMsg, "资料已经存在,不能再增加per_no=[%s]", cPer_no );
		WRITEMSG
		strcpy( g_pub_tx.reply, "H001" );
		goto	ErrExit;
	}

	set_zd_data( "0710",sPerItemMap.bat_flag);

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
