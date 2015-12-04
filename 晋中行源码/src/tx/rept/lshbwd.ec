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
EXEC SQL include "gl_bal.h";

main( int argc,char *argv[] )
{
	$char dbname[20];
	$char khjg[6];
	$char brjg[6];
	$char kmh[8];
	EXEC SQL BEGIN DECLARE SECTION;
		struct gl_bal r;
		struct gl_bal v;
		long date;
	EXEC SQL END DECLARE SECTION;

	strcpy( dbname,getenv("DBNAME") );

	$database :dbname;
	MYSQLERR
	$begin work;

	if( argc<3 ) 
	{
		printf("parm need 3\n");
		return 1;
	}
	strcpy( khjg,argv[1] );
	strcpy( brjg,argv[2] );

	EXEC SQL declare cur_upsub cursor for 
		select "111",cur_no,acc_hrt,date,dc_ind,up_acc_hrt,
		sum(dr_bal),sum(cr_bal),sum(ac_cnt),
		sum(ldd_bal),sum(lcd_bal),sum(lac_cnt),
		sum(rdd_cnt),sum(rcd_cnt),sum(rdd_amt),sum(rcd_amt),
		sum(cdd_cnt),sum(ccd_cnt),sum(cdd_amt),sum(ccd_amt),
		sum(tddr_bal),sum(tdcr_bal),sum(tdac_cnt),
		sum(tddr_cnt),sum(tdcr_cnt),
		sum(tddr_amt),sum(tdcr_amt),
		sum(mdr_bal),sum(mcr_bal),sum(mac_cnt),
		sum(mdr_cnt),sum(mcr_cnt),
		sum(mdr_amt),sum(mcr_amt),
		sum(qdr_bal),sum(qcr_bal),sum(qac_cnt),
		sum(qdr_cnt),sum(qcr_cnt),
		sum(qdr_amt),sum(qcr_amt),
		sum(ydr_bal),sum(ycr_bal),sum(yac_cnt),
		sum(ydr_cnt),sum(ycr_cnt),
		sum(ydr_amt),sum(ycr_amt)
	into 
		:r.br_no,:r.cur_no,:r.acc_hrt,:r.date,:r.dc_ind,:r.up_acc_hrt,
		:r.dr_bal,:r.cr_bal,:r.ac_cnt,
		:r.ldd_bal,:r.lcd_bal,:r.lac_cnt,
		:r.rdd_cnt,:r.rcd_cnt,:r.rdd_amt,:r.rcd_amt,
		:r.cdd_cnt,:r.ccd_cnt,:r.cdd_amt,:r.ccd_amt,
		:r.tddr_bal,:r.tdcr_bal,:r.tdac_cnt,
		:r.tddr_cnt,:r.tdcr_cnt,
		:r.tddr_amt,:r.tdcr_amt,
		:r.mdr_bal,:r.mcr_bal,:r.mac_cnt,
		:r.mdr_cnt,:r.mcr_cnt,
		:r.mdr_amt,:r.mcr_amt,
		:r.qdr_bal,:r.qcr_bal,:r.qac_cnt,
		:r.qdr_cnt,:r.qcr_cnt,
		:r.qdr_amt,:r.qcr_amt,
		:r.ydr_bal,:r.ycr_bal,:r.yac_cnt,
		:r.ydr_cnt,:r.ycr_cnt,
		:r.ydr_amt,:r.ycr_amt
	from gl_bal 
	where br_no=:khjg or br_no=:brjg
	group by 2,3,4,5,6
	order by 2,3,4,5,6 ;
	MYSQLERR

	EXEC SQL open cur_upsub;
	MYSQLERR

	while(1)
	{
		EXEC SQL fetch cur_upsub;
		if( sqlca.sqlcode==100 ) break;
		MYSQLERR

		strcpy( r.br_no,brjg );

		EXEC SQL declare cur_sum1 cursor for
			select * into :v from gl_bal_sum
			where br_no=:r.br_no and cur_no=:r.cur_no and acc_hrt=:r.acc_hrt
			and date=:r.date for UPDATE;
		MYSQLERR

		EXEC SQL open cur_sum1;
		MYSQLERR

		EXEC SQL fetch cur_sum1;
		if( sqlca.sqlcode==100 )
		{
			EXEC SQL insert into gl_bal_sum values(:r);
			MYSQLERR
		}
		else
		{
			MYSQLERR
			EXEC SQL update gl_bal_sum set *=(:r)
				where current of cur_sum1;
			MYSQLERR
		}

		EXEC SQL close cur_sum1;
	}
	EXEC SQL close cur_upsub;

/******************程序出口************************/
/*
* 正常出口
*/
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
