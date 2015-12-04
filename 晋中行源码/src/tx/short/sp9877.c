/*************************************************
* 文 件 名:  sp9877.c
* 功能描述： 缴费号码信息(欠费金额)查询 
*
* 作    者:  jack
* 完成日期： 2003年03月29日
*
* 修改记录：该交易无用了!!!! 
* 日   期:
* 修 改 人:
* 修改内容:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "ag_peritemmap_c.h"
#include "ag_itemlist_c.h"

int sp9877()
{
	struct ag_peritemmap_c sAg_peritemmap;	/*缴费号码信息表*/
	int ret = 0;
	char cItem_no[26];
	char cUnit_no[5];
	char cFlag[2];
	double dTram = 0.00;

	memset(cItem_no,0,sizeof(cItem_no));
	memset(cUnit_no,0,sizeof(cUnit_no));

	pub_base_sysinit();
	get_zd_data( "0360",cUnit_no);
	get_zd_data( "0630",cItem_no);
	get_zd_data( "0700",cFlag);

	ret=Ag_peritemmap_Sel( g_pub_tx.reply , &sAg_peritemmap ,\
			 "unit_no = '%s' and item_no = '%s' and stat = '0'", \
				cUnit_no, cItem_no );
	if( ret==100 )
	{
		sprintf( acErrMsg, "没有查询到该缴费号码编号对应记录!\
			unit_no =[%s],item_no=[%s]", cUnit_no,cItem_no );
		WRITEMSG
		if (cFlag[0] == '0')
		{
			goto OkExit;
		}
		strcpy( g_pub_tx.reply, "H010" );
		goto	ErrExit;
	}
	else if( ret )
	{
		sprintf( acErrMsg, "执行Ag_peritemmap错误!ret=[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}
	if (cFlag[0] == '0')
	{
		sprintf( acErrMsg, "缴费号码对应记录已经存在,不能再增加!!!");
		WRITEMSG
		strcpy( g_pub_tx.reply, "H011" );
		goto ErrExit;
	}

	/*------定义am06为本次应缴金额明细-------*/
	ret = sql_sum_double( "ag_itemlist", "am06", &dTram, "item = '%s' \
			and unit_no = '%s' and stcd in('0','2')", cItem_no, cUnit_no);
	if( ret )
	{
		sprintf( acErrMsg, "执行sql_sum_double错误!ret=[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	ret = 0;
	ret = sql_count( "ag_itemlist", "item_no = '%s' and unit_no = '%s' and \
				stcd in('0','2')", cItem_no, cUnit_no);
	if( ret < 1 )
	{
		strcpy( g_pub_tx.reply, "D104" );

		sprintf( acErrMsg, "欠费记录不存在!!!");
		WRITEMSG
		goto ErrExit;
	}

	set_zd_data( "0260",sAg_peritemmap.remark);
	set_zd_data( "0310",sAg_peritemmap.ac_no);
	set_zd_double( "0390",dTram);
	set_zd_data( "0720",sAg_peritemmap.bat_flag);

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
