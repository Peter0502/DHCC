/*************************************************************
* 文 件 名: gD990.c
* 功能描述：入分行gl_sub，刚上线时用一次
**************************************************************/
#define MYSQLERR if( ret ) {\
	strcpy( g_pub_tx.reply,"AT03" );\
	sprintf(acErrMsg,"SQLERR [%d]",ret);\
	WRITEMSG\
	goto ErrExit;\
	}
#define MYERR if( ret ) {\
	sprintf(acErrMsg,"SQLERR [%d]",ret);\
	WRITEMSG\
	goto ErrExit;\
	}
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "com_eod_exec_c.h"
#include "gl_sub_c.h"
#include "com_item_c.h"

EXEC SQL BEGIN DECLARE SECTION;
	struct	gl_sub_c		gl_sub;

EXEC SQL  END  DECLARE SECTION;
	struct com_branch_c sCom_branch;


int gD990()
{
	int ret=0;
	struct gl_sub_c sGl_sub;
	struct com_item_c sCom_item;
		
	memset(sGl_sub,0x00,sizeof(sGl_sub));
	memset(sCom_item,0x00,sizeof(sCom_item));
	
	sql_begin(); /*打开事务*/
	MYSQLERR
	
	ret=sum_gl();
	if(ret)
	goto ErrExit;

OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"汇总!OK!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--事务回滚--*/
	sprintf(acErrMsg,"汇总!ERROR!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
	 
}


int sum_gl()
{
	EXEC SQL BEGIN DECLARE SECTION;
		struct gl_sub_c vgl_sub;
		struct com_item_c v_item;
		int z1,z2;
	EXEC SQL END DECLARE SECTION;
	int ret=0;
	int first=0;

	EXEC SQL 
	insert into gl_sub select 11000,cur_no,acc_hrt,data,dc_ind,up_acc_hrt,sum(dr_bal),sum(cr_bal),sum(ldd_bal),sum(lcd_bal),count(rdd_cnt),count(rcd_cnt),sum(rdd_amt),sum(rcd_amt),count(cdd_cnt),count(cdd_cnt),sum(cdd_amt),sum(ccd_amt),sum(tddr_bal),sum(tdcr_bal),count(tddr_cnt),count(tcdr_cnt),sum(tddr_amt),sum(tdcr_amt),sum(mdr_bal),sum(mcr_bal),count(mdr_cnt),count(mcr_cnt),sum(mdr_amt),sum(mcr_amt),sum(qdr_bal),sum(qcr_bal),count(qdr_cnt),count(qcr_cnt),sum(qdr_amt)
,sum(qcr_amt),sum(ydr_bal),sum(ycr_bal),count(ydr_cnt),count(ycr_cnt),sum(ydr_amt),sum(ycr_amt)
		from gl_sub
		where br_no=(select br_no from com_branch where up_br_no=11000)
		group by acc_hrt

	return 0;
ErrExit:
	return 1;
}