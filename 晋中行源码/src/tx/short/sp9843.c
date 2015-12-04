/*************************************************
* 文 件 名:  sp9843.c
* 功能描述： 单位临时账号查询(对应机构)
*
* 作    者:  jack
* 完成日期： 2003年1月31日 
*
* 修改记录： 
* 日   期:
* 修 改 人:
* 修改内容:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "ag_unitacctmap_c.h"

int sp9843()
{
	struct ag_unitacctmap_c sUnitAcctMap;	/*单位临时账号管理表*/
	int ret = 0;

	char cUnit_no[5];
	char cBr_no[6];
	char proc_flag[2];

	memset(cUnit_no,0,sizeof(cUnit_no));
	memset(cBr_no,0,sizeof(cBr_no));
	memset(proc_flag,0x0,sizeof(proc_flag));

	pub_base_sysinit();
	get_zd_data("0360",cUnit_no);
	get_zd_data("0910",cBr_no);
	get_zd_data("0700",proc_flag);
		sprintf( acErrMsg, "处理标志为 [%s]",proc_flag);
		WRITEMSG

	ret=Ag_unitacctmap_Sel( g_pub_tx.reply , &sUnitAcctMap ,\
			 "br_no = '%s' and unit_no = '%s' and stat ='0'",cBr_no,cUnit_no);
	if( ret==100 )
	{
		if (proc_flag[0] == '0')
		{
			goto	OkExit;
		}
		sprintf( acErrMsg, "没有查询到该单位临时账户记录!");
		WRITEMSG
		strcpy( g_pub_tx.reply, "H010" );
		goto	ErrExit;
	}
	else if( ret )
	{
		sprintf( acErrMsg, "执行Ag_unitacctmap错误!ret=[%d]", ret );
		WRITEMSG
		goto	ErrExit;
	}

	if (proc_flag[0] == '0')
	{
		sprintf( acErrMsg, "资料已经存在,不能再增加");
		WRITEMSG
		strcpy( g_pub_tx.reply, "H001" );
		goto	ErrExit;
	}

	set_zd_data( "0670",sUnitAcctMap.acct_type);
	set_zd_data( "0300",sUnitAcctMap.bank_acct_no);

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
