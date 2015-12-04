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
EXEC SQL include "dc_log.h";

main( int argc,char *argv[] )
{
	$char dbname[20];
	$struct trace_log t;
	$struct dc_log dc;
	$char khjg[6];
	$char kmh[8];

	strcpy( dbname,getenv("DBNAME") );

	$database :dbname;
	MYSQLERR
	$begin work;

	$declare cur_tr cursor for 
		select * into :t from trace_log_bk 
		where prdt_no[1] in('1','2')
		and ac_id>0
		order by tx_date,trace_no
		;
	MYSQLERR

	$open cur_tr;

	while(1)
	{
		$fetch cur_tr;
		if( sqlca.sqlcode==100 ) break;
		MYSQLERR

		if( t.prdt_no[0]=='1' )
		{
			$select opn_br_no into :khjg from dd_mst where ac_id=:t.ac_id and ac_seqn=1;
			$select acc_hrt into :kmh from dc_acc_rel
				where dc_code=(select dc_code from dd_parm where prdT_no=:t.prdt_no)
				and data_code='0152';
		}
		else if( t.prdt_no[0]=='2' )
		{
			$select opn_br_no into :khjg from td_mst where ac_id=:t.ac_id and ac_seqn=:t.ac_seqn;
			$select acc_hrt into :kmh from dc_acc_rel
				where dc_code=(select dc_code from td_parm where prdT_no=:t.prdt_no)
				and data_code='0152';
		}
		else if( t.prdt_no[0]=='3' )
		{
			$select opn_br_no into :khjg from ln_mst where ac_id=:t.ac_id and ac_seqn=0;
			$select acc_hrt into :kmh from dc_acc_rel
				where dc_code=(select dc_code from ln_parm where prdT_no=:t.prdt_no)
				and data_code='0152';
		}
		else if( t.prdt_no[0]=='9' )
		{
			$select opn_br_no into :khjg from in_mst where ac_id=:t.ac_id and ac_seqn=0;
			$select acc_hrt into :kmh from dc_acc_rel
				where dc_code=(select dc_code from in_parm where prdT_no=:t.prdt_no)
				and data_code='0152';
		}
		else
		{
			continue;
		}

		vtcp_log("acid[%d-%d]",t.ac_id,t.ac_seqn);
		MYSQLERR

		if( strcmp(t.opn_br_no,khjg) )
		{
			printf("[%d-%d] tx[%s-%s] jg[%s-%s] [%d]\n",
				t.tx_date,t.trace_no,t.tx_code,t.tel,t.opn_br_no,khjg,t.ac_id);
		}
		else
			continue;

		$declare cur_dc cursor for select * into :dc from dc_log_bk
			where tx_date=:t.tx_date and trace_no=:t.trace_no
			and (acc_hrt=:kmh or acc_hrt[1,3]='521' or acc_hrt[1,3]='268' or acc_hrt[1,3]='260') 
			and tx_opn_br_no!=:khjg
			for update;
		MYSQLERR
		$open cur_dc;
		MYSQLERR
		while(1)
		{
			$fetch cur_dc;
			if( sqlca.sqlcode==100 ) break;
			MYSQLERR

			printf("\t[%s]--[%s]--[%s]--[%.2lf]\n",dc.tx_opn_br_no,dc.acc_hrt,dc.dc_ind,dc.amt );

			$update dc_log_bk set tx_opn_br_no=:khjg where current of cur_dc;
			MYSQLERR
		}
		$close cur_dc;
	}
	$close cur_tr;
/******************程序出口************************/
/*
* 正常出口
*/
printf( "commit ?\n" );
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
