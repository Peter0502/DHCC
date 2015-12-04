/*************************************************************
* 文 件 名: gD048.c
* 功能描述：生成产品动态表
*
* 作    者: jack
* 完成日期: 2003年3月16日
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#define MYSQLERR if( ret ) {\
	strcpy( g_pub_tx.reply,"AT03" );\
	sprintf(acErrMsg,"SQLERR [%d] [%d][%s]",ret,__LINE__,__FILE__);\
	WRITEMSG\
	goto ErrExit;\
	}
#define MYERR if( ret ) {\
	sprintf(acErrMsg,"SQLERR [%d] [%s][%d]",ret,__FILE__,__LINE__);\
	WRITEMSG\
	goto ErrExit;\
	}
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"
#include "com_sys_parm_c.h"

#include "gl_prdt_dyn_c.h"
#include "dd_mst_bk_c.h"
#include "td_mst_bk_c.h"
#include "ln_mst_bk_c.h"
#include "in_mst_bk_c.h"
#include "mo_opn_cls_c.h"
#include "trace_log_bk_c.h"

gD048()
{
	int ret = 0;
    struct com_sys_parm_c com_sys_parm_c;

    memset(&com_sys_parm_c, 0x0, sizeof(struct com_sys_parm_c));

    ret=sql_begin(); /*打开事务*/
	MYSQLERR

    /**------- 初始化全局变量 --------**/
    pub_base_sysinit();

    /* 查询系统参数表 */
    ret = Com_sys_parm_Sel(g_pub_tx.reply,&com_sys_parm_c,"1=1");
	MYSQLERR
    g_pub_tx.tx_date = com_sys_parm_c.lst_date; /* 翻盘后处理 */

	ret=sub_sum_dd_mst();
	MYERR

	ret=sub_sum_td_mst();
	MYERR

	ret=sub_sum_ln_mst();
	MYERR

	ret=sub_sum_in_mst();
	MYERR

	ret=sub_sum_trace_log();
	MYERR

	ret=sub_sum_opn_cls();
	MYERR

	ret=sub_do_other();
	MYERR
	
    ret = pub_eod_insert_oldglprdtdyn();
    if (ret != 0)
    {
        sprintf(acErrMsg,"将产品动态表导入到历史产品动态表里去!!");
        WRITEMSG
        goto ErrExit;
    }
	

OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"生成产品动态表成功!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
    if (strcmp(g_pub_tx.reply,"0000") == 0 || g_pub_tx.reply[0]=='\0')
        strcpy(g_pub_tx.reply,"G009");
	sql_rollback();	/*--事务回滚--*/
	sprintf(acErrMsg,"生成产品动态表失败!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
/**统计**/
int sub_do_other()
{
	struct gl_prdt_dyn_c vprdt;
	struct gl_prdt_dyn_c prdt_c;
	struct in_mst_bk_c in_mst_bk;
	int ret;
	int vread=0;
	int first=0;

	memset( &vprdt,0,sizeof(vprdt) );
	memset( &prdt_c,0,sizeof(prdt_c) );

	ret=Gl_prdt_dyn_Dec_Upd( g_pub_tx.reply ,"\"date\"<%d", g_pub_tx.tx_date );
	MYERR

	while(1)
	{
		ret=Gl_prdt_dyn_Fet_Upd( &prdt_c , g_pub_tx.reply );
		if( ret==100 ) break;
		else { MYERR }

		init_one_rec( &prdt_c );

		ret=Gl_prdt_dyn_Upd_Upd( prdt_c,g_pub_tx.reply );
		MYERR
	}
	Gl_prdt_dyn_Clo_Upd();

	return 0;
ErrExit:
	return 1;
}
/**统计分户dd_mst**/
int sub_sum_dd_mst()
{
	struct gl_prdt_dyn_c vprdt;
	struct gl_prdt_dyn_c prdt_c;
	struct dd_mst_bk_c dd_mst_bk;
	int ret;
	int vread=0;
	int first=0;

	memset( &vprdt,0,sizeof(vprdt) );
	memset( &prdt_c,0,sizeof(prdt_c) );

	ret=Dd_mst_bk_Dec_Sel( g_pub_tx.reply,"1=1 order by opn_br_no,prdt_no");
	MYERR

	while(1)
	{
		ret=Dd_mst_bk_Fet_Sel( &dd_mst_bk , g_pub_tx.reply );
		if( ret==100 ) break;
		else { MYERR }

		ret=Gl_prdt_dyn_Dec_Upd( g_pub_tx.reply ,"br_no='%s' and prdt_no='%s'",
			dd_mst_bk.opn_br_no,dd_mst_bk.prdt_no );
		MYERR

		ret=Gl_prdt_dyn_Fet_Upd( &prdt_c,g_pub_tx.reply );
		if( ret==100 )
		{
			memset( &prdt_c,0,sizeof(prdt_c) );
			strcpy( prdt_c.br_no, dd_mst_bk.opn_br_no );
			strcpy( prdt_c.prdt_no, dd_mst_bk.prdt_no );
			vread=1403;
		}
		else if( ret==0 )
		{
			vread=0;
		}
		else { MYERR }

		if( prdt_c.date<g_pub_tx.tx_date )
			init_one_rec( &prdt_c );

		prdt_c.cr_bal+=dd_mst_bk.bal;
		prdt_c.ac_cnt+=dd_mst_bk.cnt;

		prdt_c.date=g_pub_tx.tx_date;
		if( vread==1403 )
			ret=Gl_prdt_dyn_Ins( prdt_c,g_pub_tx.reply );
		else
			ret=Gl_prdt_dyn_Upd_Upd( prdt_c,g_pub_tx.reply );
		MYERR

		Gl_prdt_dyn_Clo_Upd();
	}
	Dd_mst_bk_Clo_Sel( );

	return 0;
ErrExit:
	return 1;
}
/**统计分户td_mst**/
int sub_sum_td_mst()
{
	struct gl_prdt_dyn_c vprdt;
	struct gl_prdt_dyn_c prdt_c;
	struct td_mst_bk_c td_mst_bk;
	int ret;
	int vread=0;
	int first=0;

	memset( &vprdt,0,sizeof(vprdt) );
	memset( &prdt_c,0,sizeof(prdt_c) );

	ret=Td_mst_bk_Dec_Sel( g_pub_tx.reply,"1=1 order by opn_br_no,prdt_no");
	MYERR

	while(1)
	{
		ret=Td_mst_bk_Fet_Sel( &td_mst_bk , g_pub_tx.reply );
		if( ret==100 ) break;
		else { MYERR }

		ret=Gl_prdt_dyn_Dec_Upd( g_pub_tx.reply ,"br_no='%s' and prdt_no='%s'",
			td_mst_bk.opn_br_no,td_mst_bk.prdt_no );
		MYERR

		ret=Gl_prdt_dyn_Fet_Upd( &prdt_c,g_pub_tx.reply );
		if( ret==100 )
		{
			memset( &prdt_c,0,sizeof(prdt_c) );
			strcpy( prdt_c.br_no, td_mst_bk.opn_br_no );
			strcpy( prdt_c.prdt_no, td_mst_bk.prdt_no );
			vread=1403;
		}
		else if( ret==0 )
		{
			vread=0;
		}
		else { MYERR }

		if( prdt_c.date<g_pub_tx.tx_date )
			init_one_rec( &prdt_c );

		prdt_c.cr_bal+=td_mst_bk.bal;
		prdt_c.ac_cnt+=td_mst_bk.cnt;

		prdt_c.date=g_pub_tx.tx_date;
		if( vread==1403 )
			ret=Gl_prdt_dyn_Ins( prdt_c,g_pub_tx.reply );
		else
			ret=Gl_prdt_dyn_Upd_Upd( prdt_c,g_pub_tx.reply );
		MYERR

		Gl_prdt_dyn_Clo_Upd();
	}
	Td_mst_bk_Clo_Sel( );

	return 0;
ErrExit:
	return 1;
}
/**统计分户ln_mst**/
int sub_sum_ln_mst()
{
	struct gl_prdt_dyn_c vprdt;
	struct gl_prdt_dyn_c prdt_c;
	struct ln_mst_bk_c ln_mst_bk;
	int ret;
	int vread=0;
	int first=0;

	memset( &vprdt,0,sizeof(vprdt) );
	memset( &prdt_c,0,sizeof(prdt_c) );

	ret=Ln_mst_bk_Dec_Sel( g_pub_tx.reply,"1=1 order by opn_br_no,prdt_no");
	MYERR

	while(1)
	{
		ret=Ln_mst_bk_Fet_Sel( &ln_mst_bk , g_pub_tx.reply );
		if( ret==100 ) break;
		else { MYERR }

		pub_base_strpack( ln_mst_bk.prdt_no );
		if( !strlen(ln_mst_bk.prdt_no) )
		{
			show_log("贷款参数为空");
			continue;
		}
		ret=Gl_prdt_dyn_Dec_Upd( g_pub_tx.reply ,"br_no='%s' and prdt_no='%s'",
			ln_mst_bk.opn_br_no,ln_mst_bk.prdt_no );
		MYERR

		ret=Gl_prdt_dyn_Fet_Upd( &prdt_c,g_pub_tx.reply );
		if( ret==100 )
		{
			memset( &prdt_c,0,sizeof(prdt_c) );
			strcpy( prdt_c.br_no, ln_mst_bk.opn_br_no );
			strcpy( prdt_c.prdt_no, ln_mst_bk.prdt_no );
			vread=1403;
		}
		else if( ret==0 )
		{
			vread=0;
		}
		else { MYERR }

		if( prdt_c.date<g_pub_tx.tx_date )
			init_one_rec( &prdt_c );

		prdt_c.cr_bal+=ln_mst_bk.bal;
		prdt_c.ac_cnt+=ln_mst_bk.cnt;

vtcp_log(" brno[%s] prdtno[%s]",prdt_c.br_no,prdt_c.prdt_no);
		prdt_c.date=g_pub_tx.tx_date;
		if( vread==1403 )
			ret=Gl_prdt_dyn_Ins( prdt_c,g_pub_tx.reply );
		else
			ret=Gl_prdt_dyn_Upd_Upd( prdt_c,g_pub_tx.reply );
		MYERR

		Gl_prdt_dyn_Clo_Upd();
	}
	Ln_mst_bk_Clo_Sel( );

	return 0;
ErrExit:
	return 1;
}
/**统计分户in_mst**/
int sub_sum_in_mst()
{
	struct gl_prdt_dyn_c vprdt;
	struct gl_prdt_dyn_c prdt_c;
	struct in_mst_bk_c in_mst_bk;
	int ret;
	int vread=0;
	int first=0;

	memset( &vprdt,0,sizeof(vprdt) );
	memset( &prdt_c,0,sizeof(prdt_c) );

	ret=In_mst_bk_Dec_Sel( g_pub_tx.reply,"1=1 order by opn_br_no,prdt_no");
	MYERR

	while(1)
	{
		ret=In_mst_bk_Fet_Sel( &in_mst_bk , g_pub_tx.reply );
		if( ret==100 ) break;
		else { MYERR }

		ret=Gl_prdt_dyn_Dec_Upd( g_pub_tx.reply ,"br_no='%s' and prdt_no='%s'",
			in_mst_bk.opn_br_no,in_mst_bk.prdt_no );
		MYERR

		ret=Gl_prdt_dyn_Fet_Upd( &prdt_c,g_pub_tx.reply );
		if( ret==100 )
		{
			memset( &prdt_c,0,sizeof(prdt_c) );
			strcpy( prdt_c.br_no, in_mst_bk.opn_br_no );
			strcpy( prdt_c.prdt_no, in_mst_bk.prdt_no );
			vread=1403;
		}
		else if( ret==0 )
		{
			vread=0;
		}
		else { MYERR }

		if( prdt_c.date<g_pub_tx.tx_date )
			init_one_rec( &prdt_c );

		prdt_c.cr_bal+=in_mst_bk.bal;
		prdt_c.ac_cnt+=in_mst_bk.cnt;

		prdt_c.date=g_pub_tx.tx_date;
		if( vread==1403 )
			ret=Gl_prdt_dyn_Ins( prdt_c,g_pub_tx.reply );
		else
			ret=Gl_prdt_dyn_Upd_Upd( prdt_c,g_pub_tx.reply );
		MYERR

		Gl_prdt_dyn_Clo_Upd();
	}
	In_mst_bk_Clo_Sel( );

	return 0;
ErrExit:
	return 1;
}
/**统计trace_log**/
int sub_sum_trace_log()
{
	struct gl_prdt_dyn_c vprdt;
	struct gl_prdt_dyn_c prdt_c;
	struct trace_log_bk_c trace_log_bk;
	int ret;
	int vread=0;
	int first=0;

	memset( &vprdt,0,sizeof(vprdt) );
	memset( &prdt_c,0,sizeof(prdt_c) );

	ret=pub_gl_prdt_dyn_sum_Dec( );
	MYERR

	while(1)
	{
		ret=pub_gl_prdt_dyn_sum_Fet( &trace_log_bk , g_pub_tx.reply );
		if( ret==1403 ) break;
		else { MYERR }

vtcp_log("TRACE br_no[%s] prdt_no[%s][%.2lf]",trace_log_bk.opn_br_no,trace_log_bk.prdt_no,trace_log_bk.amt);

		ret=Gl_prdt_dyn_Dec_Upd( g_pub_tx.reply ,"br_no='%s' and prdt_no='%s'",
			trace_log_bk.opn_br_no,trace_log_bk.prdt_no );
		MYERR

		ret=Gl_prdt_dyn_Fet_Upd( &prdt_c,g_pub_tx.reply );
		if( ret==100 )
		{
			memset( &prdt_c,0,sizeof(prdt_c) );
			strcpy( prdt_c.br_no, trace_log_bk.opn_br_no );
			strcpy( prdt_c.prdt_no, trace_log_bk.prdt_no );
			vread=1403;
		}
		else if( ret==0 )
		{
			vread=0;
		}
		else { MYERR }

		if( prdt_c.date<g_pub_tx.tx_date )
			init_one_rec( &prdt_c );

		if( trace_log_bk.add_ind[0]=='0' ) /*减*/
		{
			prdt_c.rdd_amt+=trace_log_bk.amt;
			prdt_c.rdd_cnt+=trace_log_bk.ac_id;
			prdt_c.td_dr_amt+=trace_log_bk.amt;
			prdt_c.td_dr_cnt+=trace_log_bk.ac_id;
			prdt_c.m_dr_amt+=trace_log_bk.amt;
			prdt_c.m_dr_cnt+=trace_log_bk.ac_id;
			prdt_c.q_dr_amt+=trace_log_bk.amt;
			prdt_c.q_dr_cnt+=trace_log_bk.ac_id;
			prdt_c.y_dr_amt+=trace_log_bk.amt;
			prdt_c.y_dr_cnt+=trace_log_bk.ac_id;
			if( trace_log_bk.ct_ind[0]=='1' )
			{
			prdt_c.cdd_amt+=trace_log_bk.amt;
			prdt_c.cdd_cnt+=trace_log_bk.ac_id;
			}
		}
		else if( trace_log_bk.add_ind[0]=='1' )
		{
			prdt_c.rcd_amt+=trace_log_bk.amt;
			prdt_c.rcd_cnt+=trace_log_bk.ac_id;
			prdt_c.td_cr_amt+=trace_log_bk.amt;
			prdt_c.td_cr_cnt+=trace_log_bk.ac_id;
			prdt_c.m_cr_amt+=trace_log_bk.amt;
			prdt_c.m_cr_cnt+=trace_log_bk.ac_id;
			prdt_c.q_cr_amt+=trace_log_bk.amt;
			prdt_c.q_cr_cnt+=trace_log_bk.ac_id;
			prdt_c.y_cr_amt+=trace_log_bk.amt;
			prdt_c.y_cr_cnt+=trace_log_bk.ac_id;
			if( trace_log_bk.ct_ind[0]=='1' )
			{
			prdt_c.ccd_amt+=trace_log_bk.amt;
			prdt_c.ccd_cnt+=trace_log_bk.ac_id;
			}
		}

		if( vread==1403 )
			ret=Gl_prdt_dyn_Ins( prdt_c,g_pub_tx.reply );
		else
			ret=Gl_prdt_dyn_Upd_Upd( prdt_c,g_pub_tx.reply );
		MYERR

		Gl_prdt_dyn_Clo_Upd();
	}
	pub_gl_prdt_dyn_sum_Clo();

	return 0;
ErrExit:
	return 1;
}
/**统计mo_opn_cls**/
int sub_sum_opn_cls()
{
	struct gl_prdt_dyn_c vprdt;
	struct gl_prdt_dyn_c prdt_c;
	struct mo_opn_cls_c mo_opn_cls;
	int ret;
	int vread=0;
	int first=0;

	memset( &vprdt,0,sizeof(vprdt) );
	memset( &prdt_c,0,sizeof(prdt_c) );

	ret=pub_gl_prdt_dyn_opncls_Dec( );
	MYERR

	while(1)
	{
		ret=pub_gl_prdt_dyn_opncls_Fet( &mo_opn_cls , g_pub_tx.reply );
		if( ret==1403 ) break;
		else { MYERR }

vtcp_log("MO_OPN br_no[%s] prdt_no[%s]",mo_opn_cls.opn_br_no,mo_opn_cls.prdt_no);
		pub_base_strpack( mo_opn_cls.prdt_no );
		if( !strlen(mo_opn_cls.prdt_no) )
		{
			show_log("开销户参数为空");
			continue;
		}

		ret=Gl_prdt_dyn_Dec_Upd( g_pub_tx.reply ,"br_no='%s' and prdt_no='%s'",
			mo_opn_cls.opn_br_no,mo_opn_cls.prdt_no );
		MYERR

		ret=Gl_prdt_dyn_Fet_Upd( &prdt_c,g_pub_tx.reply );
		if( ret==100 )
		{
			memset( &prdt_c,0,sizeof(prdt_c) );
			strcpy( prdt_c.br_no, mo_opn_cls.opn_br_no );
			strcpy( prdt_c.prdt_no, mo_opn_cls.prdt_no );
			vread=1403;
		}
		else if( ret==0 )
		{
			vread=0;
		}
		else { MYERR }

		if( prdt_c.date<g_pub_tx.tx_date )
			init_one_rec( &prdt_c );

		if( mo_opn_cls.wrk_sts[0]=='1' ) /*开*/
		{
			prdt_c.d_opn_cnt+=mo_opn_cls.ac_id;
		}
		else if( mo_opn_cls.wrk_sts[0]=='2' )
		{
			prdt_c.d_cls_cnt+=mo_opn_cls.ac_id;
		}

		if( vread==1403 )
			ret=Gl_prdt_dyn_Ins( prdt_c,g_pub_tx.reply );
		else
			ret=Gl_prdt_dyn_Upd_Upd( prdt_c,g_pub_tx.reply );
		MYERR

		Gl_prdt_dyn_Clo_Upd();
	}
	pub_gl_prdt_dyn_opncls_Clo();

	return 0;
ErrExit:
	return 1;
}
init_one_rec( struct gl_prdt_dyn_c *gl_prdt_dyn )
{
	struct gl_prdt_dyn_c gl_prdt_dyn_c;
	int ret=0;

	memset( &gl_prdt_dyn_c,0x00,sizeof(gl_prdt_dyn_c) );
	memcpy( &gl_prdt_dyn_c,gl_prdt_dyn,sizeof(gl_prdt_dyn_c) );

        gl_prdt_dyn_c.date = g_pub_tx.tx_date;
		gl_prdt_dyn_c.cr_bal=0.00;
		gl_prdt_dyn_c.ac_cnt=0;
        gl_prdt_dyn_c.lcd_bal = gl_prdt_dyn_c.cr_bal;
        gl_prdt_dyn_c.l_ac_cnt = gl_prdt_dyn_c.ac_cnt;

        ret = pub_base_day_xun(g_pub_tx.tx_date);
vtcp_log("XUN[%d]",ret );
        if (ret == 1)
        {
            /* 旬初 */
            gl_prdt_dyn_c.tdcr_bal = gl_prdt_dyn_c.cr_bal;
            gl_prdt_dyn_c.td_ac_cnt = gl_prdt_dyn_c.ac_cnt;
            gl_prdt_dyn_c.td_opn_cnt = 0;
            gl_prdt_dyn_c.td_cls_cnt = 0;
            gl_prdt_dyn_c.td_dr_cnt = 0;
            gl_prdt_dyn_c.td_cr_cnt = 0;
            gl_prdt_dyn_c.td_dr_amt = 0.00;
            gl_prdt_dyn_c.td_cr_amt = 0.00;
        }
        
        ret = pub_base_day_month(g_pub_tx.tx_date);
vtcp_log("MON[%d]",ret );
        if (ret == 1)
        {
            /* 月初 */
            gl_prdt_dyn_c.m_cr_bal = gl_prdt_dyn_c.cr_bal;
            gl_prdt_dyn_c.m_ac_cnt = gl_prdt_dyn_c.ac_cnt;
            gl_prdt_dyn_c.m_opn_cnt = 0;
            gl_prdt_dyn_c.m_cls_cnt = 0;
            gl_prdt_dyn_c.m_dr_cnt = 0;
            gl_prdt_dyn_c.m_cr_cnt = 0;
            gl_prdt_dyn_c.m_dr_amt = 0.00;
            gl_prdt_dyn_c.m_cr_amt = 0.00;

        }

        ret = pub_base_day_quarter(g_pub_tx.tx_date);
vtcp_log("SEN[%d]",ret );
        if (ret == 1)
        {
            /* 季初 */
            gl_prdt_dyn_c.q_cr_bal = gl_prdt_dyn_c.cr_bal;
            gl_prdt_dyn_c.q_ac_cnt = gl_prdt_dyn_c.ac_cnt;
            gl_prdt_dyn_c.q_opn_cnt = 0;
            gl_prdt_dyn_c.q_cls_cnt = 0;
            gl_prdt_dyn_c.q_dr_cnt = 0;
            gl_prdt_dyn_c.q_cr_cnt = 0;
            gl_prdt_dyn_c.q_dr_amt = 0.00;
            gl_prdt_dyn_c.q_cr_amt = 0.00;
        }

        ret = pub_base_day_year(g_pub_tx.tx_date);
vtcp_log("YEA[%d]",ret );
        if (ret == 1)
        {
            /* 年初 */
            gl_prdt_dyn_c.y_cr_bal = gl_prdt_dyn_c.cr_bal;
            gl_prdt_dyn_c.y_ac_cnt = gl_prdt_dyn_c.ac_cnt;
            gl_prdt_dyn_c.y_opn_cnt = 0;
            gl_prdt_dyn_c.y_cls_cnt = 0;
            gl_prdt_dyn_c.y_dr_cnt = 0;
            gl_prdt_dyn_c.y_cr_cnt = 0;
            gl_prdt_dyn_c.y_dr_amt = 0.00;
            gl_prdt_dyn_c.y_cr_amt = 0.00;
        }

	memcpy( gl_prdt_dyn,&gl_prdt_dyn_c,sizeof(gl_prdt_dyn_c) );
}
/**********************************************************************
 * 函 数 名：  pub_eod_insert_oldglprdtdyn
 * 函数功能：  将产品动态表中数据导入到历史产品动态表中
********************************************************************/
int pub_eod_insert_oldglprdtdyn()
{
    int ret;

    ret = sql_execute("insert into gl_prdt_dyn_hst select \"date\",prdt_no,br_no,\
          cr_bal,ac_cnt,lcd_bal,l_ac_cnt,d_opn_cnt,d_cls_cnt,rdd_cnt,rcd_cnt,\
          rdd_amt,rcd_amt,cdd_cnt,ccd_cnt,cdd_amt,ccd_amt,spe_ind from \
          gl_prdt_dyn");
    if (ret != 0)
    {
        sprintf(acErrMsg,"向历史产品动态表中备份数据错误!! [%d]",ret);
        WRITEMSG
        return ret;
    }
    return 0;
}
