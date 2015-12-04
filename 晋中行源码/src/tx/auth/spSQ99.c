/*************************************************
* 文 件 名:  spSQ99.c
* 功能描述： 交易后授权子交易。
*************************************************/

#include <stdio.h>
#include <math.h>
#define EXTERN
#include "com_sys_parm_c.h"
#include "tx_def_c.h"
#include "tx_log_c.h"
#include "auth_def_c.h"
#include "auth_code_c.h"
#include "com_tel_c.h"
#include "com_branch_c.h"
#include "auth_cond_def_c.h"
#include "svrpub_c.h"
#include "auth_chk_def_c.h"
#include "auth_inf_c.h"
#include "auth_auth_auth_c.h"
#include "auth_auth_lmt_c.h"
#include "public.h"
extern struct  tx_def_c g_tx_def_c;

int spSQ99()
{
	int	ret;
	
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
