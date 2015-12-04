/*************************************************************
* 文 件 名: rpt991.c
* 功能描述：下传报表文件交易
* 作    者: 
* 完成日期: 
* 修改记录：
* 日    期: 2003年4月30日
* 修 改 人: rob
* 修改内容:
**************************************************************/
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

int rpt991()
{
	long	date;
	char	br_no[100];
	char 	filename[100];
	char 	comm[100];
	char 	tmp[100];

	get_zd_long("0440",&date);
	get_zd_data("0910",br_no);

	/*** 初始化全局变量 ***/
	pub_base_sysinit();

	if(!date) date=g_pub_tx.tx_date;
	if( !strlen(br_no) ) strcpy( br_no,g_pub_tx.tx_br_no );

	pub_base_AllDwnFilName( filename );
	vtcp_log(filename);

	sprintf(comm,"%s/rpt.%5s.%d.Z",getenv("RPTDIR"),br_no,date);
	vtcp_log(comm);
	
	sprintf(tmp,"cp %s %s 1>/dev/null 2>/dev/null",comm, filename);

	system(tmp);
	vtcp_log(tmp);

	set_zd_data( DC_FILE_SND,"1" );

GoodExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before bad return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
