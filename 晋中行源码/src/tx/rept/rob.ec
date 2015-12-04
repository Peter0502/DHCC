 /***********************************************************
 处理旧流水开户机构的补丁程序
 ***********************************************************/

#define loc_t sys_loc_t
#define MYSQLERR if( sqlca.sqlcode && sqlca.sqlcode != 100){\
		printf( "SQL Error %d \n",sqlca.sqlcode );\
		sprintf( acErrMsg,"SQL Error %d",sqlca.sqlcode );\
		WRITEMSG;\
		strcpy(g_pub_tx.reply,"BB13");\
		goto ErrExit;\
	}
#define ERR_DEAL {WRITEMSG goto ErrExit;}

#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "public.h"
EXEC SQL include sqlca;
EXEC SQL include sqlda;
EXEC SQL include "trace_log.h";
EXEC SQL include "gl_amt.h";

main( int argc,char *argv[] )
{
	$char dbname[20];
	$char khjg[6];
	$char brjg[6];
	$char kmh[8];
	EXEC SQL BEGIN DECLARE SECTION;
		struct gl_amt r;
		struct gl_amt v;
		long date;
	EXEC SQL END DECLARE SECTION;

	strcpy( dbname,getenv("DBNAME") );

	$database :dbname;
	MYSQLERR
	$begin work;

	EXEC SQL update com_sys_parm set lst_date='11111111' where 1=1; 
	if (sqlca.sqlcode) 
	{ 
		sprintf(acErrMsg, "------error[%d]", sqlca.sqlcode);
		WRITEMSG 
		goto ErrExit; 
	} 
	sprintf(acErrMsg, "sqltype=[%d], sqllen=[%d], sqldata=[%s]", 
		(sqlda.sqlval)->sqltype, 
		(sqlda.sqlval)->sqllen, 
		(sqlda.sqlval)->sqldata ); 
	WRITEMSG

/*
* 正常出口
*/
printf("are you commit\n");
getchar();
	$commit work;
	exec sql close database;
	sprintf(acErrMsg,"Before OK return: reply is [%s]",g_pub_tx.reply);
	WRITEMSG
	printf("Before OK return: reply is [%s]\n",g_pub_tx.reply);
	return (0);
/*
* 错误出口
*/
ErrExit:
	$rollback work;
	exec sql close database;
	sprintf(acErrMsg,"Before return: reply is [%s]",g_pub_tx.reply);
	WRITEMSG
	printf("Before bad return: reply is [%s]\n",g_pub_tx.reply);
	return (1);
}
