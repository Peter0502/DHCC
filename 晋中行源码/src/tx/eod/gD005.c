/*************************************************************
* 文 件 名: gD005.c
* 功能描述：财政记账FTP取记账文件
**************************************************************/
#define EXTERN
#include <stdio.h>
#include "public.h"
#include "com_parm_c.h"
#include "com_sys_parm_c.h"

gD005()
{
	char cmd_str[100];
	char ftp_addr[21], ftp_usr[21], ftp_pass[21];
	struct com_parm_c sComParm;
	struct com_sys_parm_c sComSysParm;

	memset( &sComSysParm, 0x00, sizeof( struct com_sys_parm_c ) );

    g_reply_int = sql_begin(); /*打开事务*/
	if( g_reply_int )
	{
		sprintf( acErrMsg, "数据库操作 打开事务失败![%ld]", g_reply_int);
		WRITEMSG
		goto ErrExit;
	}

	g_reply_int = pub_base_GetSysparm( &sComSysParm );
	if ( g_reply_int )
	{
		sprintf( acErrMsg, "取公共系统参数表错![%ld]", g_reply_int );
		WRITEMSG
		goto ErrExit;			
	}

    pub_base_sysinit();

	g_pub_tx.tx_date = sComSysParm.sys_date;

	memset( &sComParm, 0x00, sizeof( struct com_parm_c ) );
	g_reply_int = Com_parm_Sel( g_pub_tx.reply, &sComParm, "parm_code='CZJZ' and parm_seqn='1'" );
	if( g_reply_int )
	{
		sprintf( acErrMsg, "取得取财政记账文件FTP地址失败![%ld]", g_reply_int );
		WRITEMSG
		return -1;
	}
	strcpy( ftp_addr, sComParm.val );
	memset( &sComParm, 0x00, sizeof( struct com_parm_c ) );
	g_reply_int = Com_parm_Sel( g_pub_tx.reply, &sComParm, "parm_code='CZJZ' and parm_seqn='2'" );
	if( g_reply_int )
	{
		sprintf( acErrMsg, "取得取财政记账文件登录用户名失败![%ld]", g_reply_int );
		WRITEMSG
		return -1;
	}
	strcpy( ftp_usr, sComParm.val );
	memset( &sComParm, 0x00, sizeof( struct com_parm_c ) );
	g_reply_int = Com_parm_Sel( g_pub_tx.reply, &sComParm, "parm_code='CZJZ' and parm_seqn='3'" );
	if( g_reply_int )
	{
		sprintf( acErrMsg, "取得取财政记账文件登录用户密码失败![%ld]", g_reply_int );
		WRITEMSG
		return -1;
	}
	strcpy( ftp_pass, sComParm.val );

	memset( cmd_str, 0x00, sizeof( cmd_str ) );
	pub_base_strpack( ftp_addr );
	pub_base_strpack( ftp_usr );
	pub_base_strpack( ftp_pass );
	sprintf( cmd_str, "ftp -vin %s 1>/home/dhccb/log/get_fin_file.err 2>&1 <<!\nverbose off\nuser %s %s\ncd tmp\npwd\ndir\nlcd %s\nget MFAP%ld.txt\nbye\n!", ftp_addr, ftp_usr, ftp_pass, getenv( "AUTOACCTDIR" ), g_pub_tx.tx_date );
	vtcp_log( "command str: [%s]", cmd_str );
	system( cmd_str );

OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy( g_pub_tx.reply, "0000" );
	sprintf( acErrMsg, "财政记账取文件成功![%s]", g_pub_tx.reply );
	WRITEMSG
	fin_report( "---------- %ld - 财政记账取文件成功结束 ----------", g_pub_tx.tx_date );
	set_zd_data( "0120", g_pub_tx.reply );
	return 0;
ErrExit:
	sql_rollback();	/*--事务回滚--*/
	sprintf( acErrMsg, "财政记账取文件失败![%s]", g_pub_tx.reply );
	WRITEMSG
	fin_report( "---------- %ld - 财政记账取文件失败结束 ----------", g_pub_tx.tx_date );
	set_zd_data( "0120", g_pub_tx.reply );
	return 1;
}
