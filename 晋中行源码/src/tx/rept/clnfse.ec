/***********************************************************
处理旧流水开户机构的补丁程序
***********************************************************/

#define loc_t sys_loc_t
#define MYSQLERR if( sqlca.sqlcode && sqlca.sqlcode != 100){\
		printf( "SQL Error %d %d\n",__LINE__,sqlca.sqlcode );\
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
EXEC SQL include "gl_bal.h";
EXEC SQL include "gl_sub.h";
EXEC SQL include "dd_mst_hst.h";

main( int argc,char *argv[] )
{
	$char dbname[20];
	$char khjg[6];
	$char brjg[6];
	$char kmh[8];
	$struct dd_mst_hst r,o;
	long i=0;

	strcpy( dbname,getenv("DBNAME") );

	$database :dbname;
	MYSQLERR
	$begin work;

printf("opening\n");
	EXEC SQL declare cur_mx cursor for 
		select * into :r from dd_mst_hst
		where tx_date>=20050921 and tx_date<20051221
		order by ac_id,tx_date,trace_no;
	MYSQLERR

	$open cur_mx;
	MYSQLERR
printf("open over\n");

	o.ac_id=0;
	i=0;
	while(1)
	{
		$fetch cur_mx;
		if( sqlca.sqlcode==100 )break;
		MYSQLERR

		if( o.ac_id==0 )
		{
			memcpy( &o,&r,sizeof(o) );
		}

		if( o.ac_id==r.ac_id && r.intst_acm<o.intst_acm 
			&& strcmp(r.tx_code,"5702")
			&& strcmp(r.tx_code,"5701") 
			&& r.tx_amt>0)
		{
			printf("[%d] amt[%.2lf] bal[%.2lf] acm[%.2lf] [%d][%d]\n"
				,o.ac_id,r.tx_amt,o.bal,r.intst_acm,r.tx_date,r.trace_no );
		}
			memcpy( &o,&r,sizeof(o) );
		if( i%10000==0 )
			printf("----[%d]\n",i+1);
		i++;
	}
	$close cur_mx;
/******************程序出口************************/
/*
* 正常出口
*/
printf("are you commit\n");
getchar();
	$commit work;
	exec sql close database;
	printf("Before OK return: reply is [%s]\n",g_pub_tx.reply);
	return (0);
/*
* 错误出口
*/
ErrExit:
	$rollback work;
	exec sql close database;
	printf("Before bad return: reply is [%s]\n",g_pub_tx.reply);
	return (1);
}
cl_702()
{
	EXEC SQL BEGIN DECLARE SECTION;
		struct gl_sub r;
		struct gl_sub v831;
		long date;
		double jfse,dfse,tmpdb;
		int z1,z2;
	EXEC SQL END DECLARE SECTION;

	EXEC SQL declare cur_upsub cursor for 
		select * into :r
		from gl_sub
		where acc_hrt='70200'
		order by br_no,cur_no,acc_hrt ;
	MYSQLERR

	EXEC SQL open cur_upsub;
	MYSQLERR

	while(1)
	{
		EXEC SQL fetch cur_upsub;
		if( sqlca.sqlcode==100 ) break;
		MYSQLERR

		printf("br_no[%s] acc_hrt[%s]\n",r.br_no,r.acc_hrt);

		EXEC SQL select * into :r from gl_sub_826
			where br_no=:r.br_no and cur_no=:r.cur_no and acc_hrt=:r.acc_hrt;
		if( sqlca.sqlcode==100 ) memset( &r,0,sizeof(r) );
		MYSQLERR

		tmpdb=r.ycr_amt;
		r.ycr_amt=r.ydr_amt;
		r.ydr_amt=tmpdb;


		EXEC SQL select * into :v831 from zz831:gl_sub
			where br_no=:r.br_no and cur_no=:r.cur_no and acc_hrt=:r.acc_hrt;
		MYSQLERR

		r.dr_bal=v831.dr_bal;
		r.cr_bal=v831.cr_bal;

		EXEC SQL select sum(amt) into :jfse:z1 from zz831:dc_log_bk
			where tx_opn_br_no=:r.br_no and acc_hrt=:r.acc_hrt
			 and sts='0' and dc_ind='1';
		MYSQLERR
		if( z1 ) jfse=0.00;

		EXEC SQL select sum(amt) into :dfse:z2 from zz831:dc_log_bk
			where tx_opn_br_no=:r.br_no and acc_hrt=:r.acc_hrt
			 and sts='0' and dc_ind='2';
		MYSQLERR
		if( z2 ) dfse=0.00;

		r.mdr_amt+=jfse;
		r.mcr_amt+=dfse;

		r.ydr_amt+=jfse;
		r.ycr_amt+=dfse;

		printf("      yc[%.2lf] [%.2lf] [%.2lf] ce[%.2lf]\n",
			r.mdr_bal-r.mcr_bal, r.mdr_amt-r.mcr_amt, r.dr_bal-r.cr_bal,
			r.mdr_bal-r.mcr_bal+(r.mdr_amt-r.mcr_amt)-(r.dr_bal-r.cr_bal) );

		printf("      nc[%.2lf] [%.2lf] [%.2lf] ce[%.2lf]\n",
			r.ydr_bal-r.ycr_bal, r.ydr_amt-r.ycr_amt, r.dr_bal-r.cr_bal,
			r.ydr_bal-r.ycr_bal+(r.ydr_amt-r.ycr_amt)-(r.dr_bal-r.cr_bal) );
	}
	EXEC SQL close cur_upsub;

	return 0;
ErrExit:
	return 1;
}
cl_702b()
{
	EXEC SQL BEGIN DECLARE SECTION;
		struct gl_sub r;
		struct gl_sub v0930;
		long date;
		double jfse,dfse,tmpdb;
		int z1,z2;
	EXEC SQL END DECLARE SECTION;

	EXEC SQL declare cur_upsubb cursor for 
		select * into :r
		from zz1031:gl_sub
		where acc_hrt='70200'
		order by br_no,cur_no,acc_hrt ;
	MYSQLERR

	EXEC SQL open cur_upsubb;
	MYSQLERR

	while(1)
	{
		EXEC SQL fetch cur_upsubb;
		if( sqlca.sqlcode==100 ) break;
		MYSQLERR

		printf("br_no[%s] acc_hrt[%s]\n",r.br_no,r.acc_hrt);

		EXEC SQL select * into :v0930 from zz930:gl_sub
			where br_no=:r.br_no and cur_no=:r.cur_no and acc_hrt=:r.acc_hrt;
		MYSQLERR

		r.mdr_bal=v0930.dr_bal;
		r.mcr_bal=v0930.cr_bal;

		printf("      yc[%.2lf] [%.2lf] [%.2lf] ce[%.2lf]\n",
			r.mdr_bal-r.mcr_bal, r.mdr_amt-r.mcr_amt, r.dr_bal-r.cr_bal,
			r.mdr_bal-r.mcr_bal+(r.mdr_amt-r.mcr_amt)-(r.dr_bal-r.cr_bal) );
	}
	EXEC SQL close cur_upsubb;

	return 0;
ErrExit:
	return 1;
}
cl_702c()
{
	EXEC SQL BEGIN DECLARE SECTION;
		struct gl_sub r;
		struct gl_sub v1031;
		long date;
		double jfse,dfse,tmpdb;
		int z1,z2;
	EXEC SQL END DECLARE SECTION;

	EXEC SQL declare cur_upsub3 cursor for 
		select * into :r
		from zz1130:gl_sub
		where acc_hrt='70200'
		order by br_no,cur_no,acc_hrt ;
	MYSQLERR

	EXEC SQL open cur_upsub3;
	MYSQLERR

	while(1)
	{
		EXEC SQL fetch cur_upsub3;
		if( sqlca.sqlcode==100 ) break;
		MYSQLERR

		printf("br_no[%s] acc_hrt[%s]\n",r.br_no,r.acc_hrt);

		EXEC SQL select * into :v1031 from zz1031:gl_sub
			where br_no=:r.br_no and cur_no=:r.cur_no and acc_hrt=:r.acc_hrt;
		MYSQLERR

		r.mdr_bal=v1031.dr_bal;
		r.mcr_bal=v1031.cr_bal;

		printf("      yc[%.2lf] [%.2lf] [%.2lf] ce[%.2lf]\n",
			r.mdr_bal-r.mcr_bal, r.mdr_amt-r.mcr_amt, r.dr_bal-r.cr_bal,
			r.mdr_bal-r.mcr_bal+(r.mdr_amt-r.mcr_amt)-(r.dr_bal-r.cr_bal) );
	}
	EXEC SQL close cur_upsub3;

	return 0;
ErrExit:
	return 1;
}
