/***************************************************************
* 文 件 名:     pubf_de_check_mst.c
* 功能描述：    检查主文件账户状态
*
* 作    者:	jack
* 完成日期：    2003年12月27日
*
* 修改记录：
*    日    期:
*    修 改 人:
*    修改内容:
*
**************************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"

/***************************************************************
* 函 数 名:     pubf_de_chk_dd_mst.c
* 功能描述：    检查活期主文件账户状态
*			flag[0]:第一位是否已取主文件 0-未取 1-已取；
*			flag[1]:第二位是否检查帐户有效日期 0-否 1-是；
*			flag[2]:第三位是否检查帐户状态 0-否 1-是；
*			flag[3]:第四位是否检查帐户冻结 0-否 1-是；
*
* 作    者:
* 完成日期：    2003年12月27日
*
* 修改记录：
*    日    期:
*    修 改 人:
*    修改内容:
*
**************************************************************/
int pub_de_chk_dd_mst(char flag[10])
{
	int ret=0;

	if( flag[0]=='0' )
	{
		ret=pub_base_currentdis_file(g_pub_tx.ac_no,g_pub_tx.ac_seqn,&g_dd_mst);
		if( ret )
		{
			sprintf(acErrMsg,"取活期结算户主文件错误![%d][%s][%d]",ret,g_pub_tx.ac_no,g_pub_tx.ac_seqn);
			WRITEMSG
			return 1;
		}
	}

	if( flag[1]=='1' && g_dd_mst.val_date!=0 
		&& g_dd_mst.val_date<g_pub_tx.tx_date )
	{
		sprintf(acErrMsg,"该账户已经过有效期，请销户![%d][%d]",g_dd_mst.val_date,g_pub_tx.tx_date);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P285");
		return 1;
	}

	if( flag[2]=='1' && g_dd_mst.ac_sts[0]!='1' )
	{
		sprintf("该账户账户状态不正常![%s]",g_dd_mst.ac_sts);
		WRITEMSG
		strcpy(g_pub_tx.reply,"O085");
		return 1;
	}

	if( flag[3]=='1' && g_dd_mst.hold_sts[0]!='0' )
	{
		sprintf("该账户账户已经被冻结![%s]",g_dd_mst.hold_sts);
		WRITEMSG
		strcpy(g_pub_tx.reply,"O084");
		return 1;
	}

	return 0;
ErrExit:
	return 1;
}

/***************************************************************
* 函 数 名:     pubf_de_chk_td_mst.c
* 功能描述：    检查定期主文件账户状态
*				flag[0]:第一位是否已取主文件 0-未取 1-已取；
*				flag[1]:第二位是否检查帐户支取次数是否超过总次数 0-否 1-是；
*				flag[2]:第三位是否检查帐户状态 0-否 1-是；
*				flag[3]:第四位是否检查帐户冻结 0-否 1-是；
*
* 作    者:
* 完成日期：    2003年12月27日
*
* 修改记录：
*    日    期:
*    修 改 人:
*    修改内容:
*
**************************************************************/
int pub_de_chk_td_mst(char flag[10])
{
	int ret=0;

	if( flag[0]=='0' )
	{
		ret=pub_base_termidis_file(g_pub_tx.ac_no,g_pub_tx.ac_seqn,&g_td_mst);
		if( ret )
		{
			sprintf(acErrMsg,"取活期结算户主文件错误![%d][%s][%d]",ret,g_pub_tx.ac_no,g_pub_tx.ac_seqn);
			WRITEMSG
			return 1;
		}
	}

	if( flag[1]=='1' && g_td_mst.ttl_cnt!=0 && g_td_mst.ttl_cnt<g_td_mst.tx_cnt )
	{
		sprintf(acErrMsg,"该账户已经超过支取次数，请销户![%d][%d]",g_td_mst.ttl_cnt,g_td_mst.tx_cnt);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P286");
		return 1;
	}

	if( flag[2]=='1' && g_td_mst.ac_sts[0]!='1' )
	{
		sprintf("该账户账户状态不正常![%s]",g_td_mst.ac_sts);
		WRITEMSG
		strcpy(g_pub_tx.reply,"O085");
		return 1;
	}

	if( flag[3]=='1' && g_td_mst.hold_sts[0]!='0' )
	{
		sprintf("该账户账户已经被冻结![%s]",g_td_mst.hold_sts);
		WRITEMSG
		strcpy(g_pub_tx.reply,"O084");
		return 1;
	}

	return 0;
ErrExit:
	return 1;
}
