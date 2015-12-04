/******************************************************************
* 文件名:	sp9555.c
* 功能描述:	查表得到授权人(在第2 3种方式的情况下用)
* added by lyz 20061020 
*
******************************************************************/
#define MYRETERR if(ret){\
	sprintf(acErrMsg,"return error[%d]",ret); \
	WRITEMSG \
	goto ErrExit; \
	}

#include <stdio.h>
#include <string.h>
#define EXTERN
#include "public.h"
#include "auth_inf_c.h"
#include "auth_code_c.h"
#include "com_tel_change_c.h"

struct auth_inf_c g_auth_inf;
struct auth_code_c g_auth_code;
struct com_tel_change_c g_com_tel_change;

int sp9555()
{
	int ret=0;
	char auth_no[8];
	char auth_num[8];
	char s;
	 long lauth_no=0,lauth_num=0; 
	
	memset(&g_auth_code,0x00,sizeof(struct auth_code_c));
	memset(auth_no,0x00,sizeof(auth_no));
	memset(auth_num,0x00,sizeof(auth_num));
	memset(&g_auth_inf,0x00,sizeof(struct auth_inf_c));
	memset(&g_com_tel_change,0x00,sizeof(struct com_tel_change_c));
	pub_base_sysinit();

	get_zd_long("0140",&lauth_no);
	get_zd_data("0200",&s);
vtcp_log("gogogo 9555 auth_no=[%ld],s=[%c] iauth_no=[%ld] 哈哈[%s]--[%d]",lauth_no,s,lauth_no,__FILE__,__LINE__);
	if(s == '3') /*第3种*/
	{ 
	get_zd_long("0280",&lauth_num);
	ret=Auth_inf_Sel(g_pub_tx.reply,&g_auth_inf,"auth_no=%ld",lauth_no); 
	MYRETERR
	set_zd_data("0340",g_auth_inf.auth_tel);
	ret=Auth_code_Sel(g_pub_tx.reply,&g_auth_code," auth_no=%ld " ,lauth_num);
	MYRETERR
	set_zd_data("0680",g_auth_code.sts);
vtcp_log("g_auth_inf.auth_tel=[%s] [%s]--[%d]",g_auth_inf.auth_tel,__FILE__,__LINE__);	
	}
	else if(s == '2')/*第2种*/
	{
	ret=Auth_code_Sel(g_pub_tx.reply,&g_auth_code,"auth_no=%ld",lauth_no);
	MYRETERR
	set_zd_data("0340",g_auth_code.auth);
	set_zd_data("0680",g_auth_code.sts);
vtcp_log("g_auth_code.auth=[%s] [%s]--[%d]",g_auth_code.auth,__FILE__,__LINE__);	
	}
	else if(s == '1')/*第1种*/
	{
	ret=Auth_code_Sel(g_pub_tx.reply,&g_auth_code,"auth_no=%ld",lauth_no);
	MYRETERR
	set_zd_data("0680",g_auth_code.sts);
vtcp_log("g_auth_code.auth=[%s] $FD67=[%s] [%s]--[%d]",g_auth_code.auth,g_auth_code.sts,__FILE__,__LINE__);	
	}
	/*add by martin 2008-11-23 10:03*/
	ret=Com_tel_change_Sel(g_pub_tx.reply,&g_com_tel_change," to_tel=%s and sts='1' ",g_auth_code.auth);
	set_zd_data("0270",g_com_tel_change.name);
	vtcp_log("g_com_tel_change.name=[%s] [%s]--[%d]",g_com_tel_change.name,__FILE__,__LINE__);	
	/*add by martin end 2008-11-23 10:03*/
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

