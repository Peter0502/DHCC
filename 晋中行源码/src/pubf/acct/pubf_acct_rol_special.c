/***************************************************************
* 文 件 名:     pubf_acct_rol_special.c
* 功能描述：    撤销中对交易的特殊处理
**标志用法：
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
#define ERR_DEAL if(ret){sprintf(acErrMsg,"ERROR");WRITEMSG goto ErrExit;}
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "dc_log_c.h"
#include "trace_log_c.h"
#include "tx_dc_br_rel_c.h"
#include "mo_invest_c.h"

int pub_acct_Rol_special( ptrace_log )
 struct trace_log_c *ptrace_log;
{
	int ret=0;

	/*子交易1111的特殊处理**/
	if( !strcmp(ptrace_log->sub_tx_code,"1111") )
	{
	}
	/*交易1111的特殊处理**/
	if( !strcmp(ptrace_log->tx_code,"1111") )
	{
	}

OkExit:
	return 0;
ErrExit:
	return 1;
}

int pub_acct_Rol_spe( ptrace_log )
 struct trace_log_c *ptrace_log;
{
	int ret=0;

	if( !strcmp(ptrace_log->sub_tx_code,"4603") )
	{
		;
	}

OkExit:
	return 0;
ErrExit:
	return 1;
}
