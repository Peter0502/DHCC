/*************************************************
* 文 件 名:  sp9839.c
* 功能描述： 查询个人编号信息
*
* 作    者:  jack
* 完成日期： 2003年1月28日 
*
* 修改记录： 
* 日   期:
* 修 改 人:
* 修改内容:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "ag_peracctinfo_c.h"

int sp9839()
{
	struct ag_peracctinfo_c sPerAcctInfo;	/*个人信息表*/
	int ret = 0;
	char cPer_no[13];
	char cPer_pwd[7];
	int pwd_len = 0;

	memset(cPer_no,0,sizeof(cPer_no));
	memset(cPer_pwd,0,sizeof(cPer_pwd));

	pub_base_sysinit();
	get_zd_data( "0590",cPer_no); 
	get_zd_data( "0800",cPer_pwd);

	pwd_len = strlen(cPer_pwd);
	if (pwd_len == 0)
	{
		ret=Ag_peracctinfo_Sel( g_pub_tx.reply , &sPerAcctInfo ,\
			 "per_no = '%s' and per_stat ='0'",cPer_no);
	}
	else if (pwd_len == 6)
	{
		ret=Ag_peracctinfo_Sel( g_pub_tx.reply , &sPerAcctInfo ,\
			 "per_no = '%s' and per_pwd = '%s' and per_stat ='0'",cPer_no,cPer_pwd);
	}
	if( ret==100 )
	{
		sprintf( acErrMsg, "没有查询到该个人编号对应记录!\
			per_no=[%s]", cPer_no );
		WRITEMSG
		strcpy( g_pub_tx.reply, "H003" );
		goto	ErrExit;
	}
	else if( ret )
	{
		sprintf( acErrMsg, "执行Ag_peracctinfo错误!ret=[%d]", ret );
		WRITEMSG
		goto	ErrExit;
	}

	set_zd_data( "0300",sPerAcctInfo.bank_acct_no);
	set_zd_data( "0670",sPerAcctInfo.acct_type);
	set_zd_data( "0250",sPerAcctInfo.per_name);

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
