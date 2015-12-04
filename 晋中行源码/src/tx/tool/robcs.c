/*************************************************************
**************************************************************/
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include "public.h"
#include "svrpub_c.h"
#include "ln_lo_c.h"
#include "com_rate_c.h"

main(int argc, char *argv[])
{
	int ret = 0;
	char str[3000];
	char mystery[17];
	long lAc_id;
	int iAc_seqn;
	struct com_rate_c com_rate_c;
	long i=0;
	double llz;
	double sdsl;
	long j=0;

	ret=sql_database(); /*打开数据库*/
	if( ret ) 
	{
		sprintf( acErrMsg, "打开数据库失败!!!" );
		WRITEMSG
		goto ErrExit;
	}

	ret=sql_begin(); /*打开事务*/
	if( ret ) 
	{
		sprintf( acErrMsg, "打开事务失败!!!" );
		WRITEMSG
		goto ErrExit;
	}

	if( argc==2 ) j=atol(argv[1]);

		show_log("BEGIN[%d]",i);
	for( i=0;i<j;i++ )
	{
		show_log("OOOOOOOOO[%d]",i);
		 ret=pub_base_getllz("100","01",20030721,&llz);
		 if( ret ) goto ErrExit;
		 /*ret = pub_base_GetRate(SLBH, 20030721, &sdsl);
		 if( ret ) goto ErrExit;
		 */
	}
		show_log("end[%d]",i);
	
	vtcp_log("begin test===============");
	vtcp_log("end   test===============");
	/*
	ret=pub_pubdb_droptab( "dd_mst_a" );
	printf("drop [%d]\n",ret);

	sql_execute("select * from dd_mst_a");
	*/

OkExit:
	sql_rollback();
	sql_close();
	printf("手动更改密押成功!!!!!\n");
	WRITEMSG
	return 0;
ErrExit:
	sql_rollback();
	sql_close();
	printf("手动更改密押失败!!!!!\n");
	WRITEMSG
	return 1;
}
