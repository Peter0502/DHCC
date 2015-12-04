/*************************************************************
* 文 件 名: gD021.c
* 功能描述：日终入明细(分单笔和按账号汇总)
*
* 作    者: jack
* 完成日期: 2003年3月15日
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"
#include "trace_log_c.h"
#include "dd_mst_hst_c.h"
#include "td_mst_hst_c.h"
#include "ln_mst_hst_c.h"
#include "in_mst_hst_c.h"


gD021()
{
	int ret = 0;
    /**------- 初始化全局变量 --------**/
	pub_base_sysinit();

	ret = ZZ_mst_hst();
	if ( ret )
	{
		sprintf(acErrMsg,"日终单笔入明细函数调用失败!![%s]",g_pub_tx.reply);
		WRITEMSG
		goto ErrExit;
	}

	ret = HZ_mst_hst();
	if ( ret )
	{
		sprintf(acErrMsg,"日终汇总入明细函数调用失败!![%s]",g_pub_tx.reply);
		WRITEMSG
		goto ErrExit;
	}


OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"日终入明细成功!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"日终入明细失败!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

/*---------------------------------------------
* 功能：根据交易流水逐笔入明细
*
*
*--------------------------------------------*/
int ZB_mst_hst()
{
	int ret = 0;

	struct trace_log_c sTrace_log;
	struct dd_mst_c sDd_mst;
	struct td_mst_c sTd_mst;
	struct ln_mst_c sLn_mst;
	struct in_mst_c sIn_mst;

	struct dd_mst_hst_c sDd_mst_hst;
	struct td_mst_hst_c sTd_mst_hst;
	struct ln_mst_hst_c sLn_mst_hst;
	struct in_mst_hst_c sIn_mst_hst;


	ret=sql_begin(); /*打开事务*/
	if( ret ) 
	{
		sprintf( acErrMsg, "打开事务失败!!!" );
		WRITEMSG
		goto ErrExit;
	}

	ret = Trace_log_Dec_Sel ( g_pub_tx.reply , "hst_ind = '1' \
			and tx_date = %ld and sts = '0' order by trace_no", \
			g_pub_tx.tx_date);
	if( ret )
	{
		sprintf( acErrMsg, "执行Trace_log_Dec_Sel失败!!!");
		WRITEMSG
		goto ErrExit;
	}

	while(1)
	{
		memset(&sTrace_log,0x00,sizeof(sTrace_log));

		ret = Trace_log_Fet_Sel ( &sTrace_log, g_pub_tx.reply );
		if( ret )
		{
			sprintf( acErrMsg, "执行Trace_log_Fet_Sel失败!!!");
			WRITEMSG
			goto ErrExit;
		}

		ret = pub_base_disac_file ( sTrace_log.ac_no, sTrace_log.ac_seqn, g_pub_tx.ac_type );
		if( ret )
		{
			sprintf( acErrMsg, "执行Trace_log_Fet_Sel失败!!!");
			WRITEMSG
			goto ErrExit;
		}

		switch( g_pub_tx.ac_type[0] )
		{
			case '1':		/*活期*/
				memset(&sDd_mst, 0x00, sizeof(sDd_mst) );
				memset(&sDd_mst_hst, 0x00, sizeof(sDd_mst_hst) );

				/*-----查询活期主文件-----*/
				ret = Dd_mst_Sel( g_pub_tx.reply, &sDd_mst, \
						"ac_id = %d and ac_seqn = %d", \
						sTrace_log.ac_id,sTrace_log.ac_seqn);
				if( ret )
				{
					sprintf( acErrMsg, "执行Dd_mst_Sel失败!!!ac_id = [%d] and ac_seqn = [%d]" ,sTrace_log.ac_id,sTrace_log.ac_seqn);
					WRITEMSG
					goto ErrExit;
				}

				sDd_mst_hst.trace_no = sTrace_log.trace_no;
				sDd_mst_hst.trace_cnt = sTrace_log.trace_cnt;
				strcpy(sDd_mst_hst.opn_br_no, sDd_mst.opn_br_no);
				strcpy(sDd_mst_hst.tx_br_no, sTrace_log.tx_br_no);
				sDd_mst_hst.ac_id = sTrace_log.ac_id;
				sDd_mst_hst.ac_seqn = sTrace_log.ac_seqn;
				strcpy(sDd_mst_hst.tx_code, sTrace_log.tx_code);
				strcpy(sDd_mst_hst.sub_tx_code, sTrace_log.sub_tx_code);
				strcpy(sDd_mst_hst.add_ind, sTrace_log.add_ind);
				strcpy(sDd_mst_hst.ct_ind, sTrace_log.ct_ind);

				sDd_mst_hst.tx_amt = sTrace_log.amt;

				if ( sTrace_log.add_ind[0] == '0' )
				{
					sDd_mst_hst.bal = sDd_mst.hst_bal - sTrace_log.amt;
				}
				if ( sTrace_log.add_ind[0] == '1' )
				{
					sDd_mst_hst.bal = sDd_mst.hst_bal + sTrace_log.amt;
				}

				sDd_mst_hst.acbo_bal = 0.00;
				sDd_mst_hst.intst_acm = sDd_mst.intst_acm;
				sDd_mst_hst.tx_date = sTrace_log.tx_date;
				sDd_mst_hst.tx_time = sTrace_log.tx_time;
				strcpy(sDd_mst_hst.note_type, sTrace_log.note_type);
				strcpy(sDd_mst_hst.note_no, sTrace_log.note_no);
				sDd_mst_hst.hst_cnt = sDd_mst.hst_cnt + 1;
				strcpy(sDd_mst_hst.brf, sTrace_log.brf);
				strcpy(sDd_mst_hst.tel, sTrace_log.tel);
				strcpy(sDd_mst_hst.chk, sTrace_log.chk);
				strcpy(sDd_mst_hst.auth, sTrace_log.aut);

				ret = Dd_mst_hst_Ins( sDd_mst_hst , g_pub_tx.reply );
				if( ret )
				{
					sprintf( acErrMsg, "插入活期明细文件失败!!!ac_id = [%d] and ac_seqn = [%d]" ,sTrace_log.ac_id,sTrace_log.ac_seqn);
					WRITEMSG
					goto ErrExit;
				}

				ret = sql_execute ("update dd_mst set hst_bal = %f,hst_cnt = %d", sDd_mst_hst.bal,sDd_mst_hst.hst_cnt);
				if( ret )
				{
					sprintf( acErrMsg, "修改活期主文件失败!!ac_id = [%d] and ac_seqn = [%d]" ,sTrace_log.ac_id,sTrace_log.ac_seqn);
					WRITEMSG
					goto ErrExit;
				}

				break;
			case '2':		/*定期*/
				memset(&sTd_mst, 0x00, sizeof(sTd_mst) );
				memset(&sTd_mst_hst, 0x00, sizeof(sTd_mst_hst) );

				/*-----查询定期主文件-----*/
				ret = Dd_mst_Sel( g_pub_tx.reply, &sTd_mst, \
						"ac_id = %d and ac_seqn = %d", \
						sTrace_log.ac_id,sTrace_log.ac_seqn);
				if( ret )
				{
					sprintf( acErrMsg, "执行Dd_mst_Sel失败!!!ac_id = [%d] and ac_seqn = [%d]" ,sTrace_log.ac_id,sTrace_log.ac_seqn);
					WRITEMSG
					goto ErrExit;
				}

				sTd_mst_hst.trace_no = sTrace_log.trace_no;
				sTd_mst_hst.trace_cnt = sTrace_log.trace_cnt;
				strcpy(sTd_mst_hst.opn_br_no, sTd_mst.opn_br_no);
				strcpy(sTd_mst_hst.tx_br_no, sTrace_log.tx_br_no);
				sTd_mst_hst.ac_id = sTrace_log.ac_id;
				sTd_mst_hst.ac_seqn = sTrace_log.ac_seqn;
				strcpy(sTd_mst_hst.tx_code, sTrace_log.tx_code);
				strcpy(sTd_mst_hst.sub_tx_code, sTrace_log.sub_tx_code);
				strcpy(sTd_mst_hst.add_ind, sTrace_log.add_ind);
				strcpy(sTd_mst_hst.ct_ind, sTrace_log.ct_ind);
				sTd_mst_hst.tx_amt = sTrace_log.amt;

				if ( sTrace_log.add_ind[0] == '0' )
				{
					sTd_mst_hst.bal = sTd_mst.hst_bal - sTrace_log.amt;
				}
				if ( sTrace_log.add_ind[0] == '1' )
				{
					sTd_mst_hst.bal = sTd_mst.hst_bal + sTrace_log.amt;
				}

				sTd_mst_hst.acbo_bal = 0.00;
				sTd_mst_hst.intst_acm = sTd_mst.intst_acm;
				sTd_mst_hst.tx_date = sTrace_log.tx_date;
				sTd_mst_hst.tx_time = sTrace_log.tx_time;
				strcpy(sTd_mst_hst.note_type, sTrace_log.note_type);
				strcpy(sTd_mst_hst.note_no, sTrace_log.note_no);
				sTd_mst_hst.hst_cnt = sTd_mst.hst_cnt + 1;
				strcpy(sTd_mst_hst.brf, sTrace_log.brf);
				strcpy(sTd_mst_hst.tel, sTrace_log.tel);
				strcpy(sTd_mst_hst.chk, sTrace_log.chk);
				strcpy(sTd_mst_hst.auth, sTrace_log.aut);

				ret = Dd_mst_hst_Ins( sTd_mst_hst , g_pub_tx.reply );
				if( ret )
				{
					sprintf( acErrMsg, "插入定期明细文件失败!!!ac_id = [%d] and ac_seqn = [%d]" ,sTrace_log.ac_id,sTrace_log.ac_seqn);
					WRITEMSG
					goto ErrExit;
				}

				ret = sql_execute ("update td_mst set hst_bal = %f,hst_cnt = %d", sTd_mst_hst.bal,sTd_mst_hst.hst_cnt);
				if( ret )
				{
					sprintf( acErrMsg, "修改定期主文件失败!!ac_id = [%d] and ac_seqn = [%d]" ,sTrace_log.ac_id,sTrace_log.ac_seqn);
					WRITEMSG
					goto ErrExit;
				}

				break;
			case '3':		/*贷款*/

				memset(&sLn_mst, 0x00, sizeof(sLn_mst) );
				memset(&sLn_mst_hst, 0x00, sizeof(sLn_mst_hst) );

				/*-----查询贷款主文件-----*/
				ret = Dd_mst_Sel( g_pub_tx.reply, &sLn_mst, \
						"ac_id = %d and ac_seqn = %d", \
						sTrace_log.ac_id,sTrace_log.ac_seqn);
				if( ret )
				{
					sprintf( acErrMsg, "执行Dd_mst_Sel失败!!!ac_id = [%d] and ac_seqn = [%d]" ,sTrace_log.ac_id,sTrace_log.ac_seqn);
					WRITEMSG
					goto ErrExit;
				}

				sLn_mst_hst.trace_no = sTrace_log.trace_no;
				sLn_mst_hst.trace_cnt = sTrace_log.trace_cnt;
				strcpy(sLn_mst_hst.opn_br_no, sLn_mst.opn_br_no);
				strcpy(sLn_mst_hst.tx_br_no, sTrace_log.tx_br_no);
				sLn_mst_hst.ac_id = sTrace_log.ac_id;
				sLn_mst_hst.ac_seqn = sTrace_log.ac_seqn;
				strcpy(sLn_mst_hst.tx_code, sTrace_log.tx_code);
				strcpy(sLn_mst_hst.sub_tx_code, sTrace_log.sub_tx_code);
				strcpy(sLn_mst_hst.ln_tx_type, "0");	/*--暂定--*/
				strcpy(sLn_mst_hst.add_ind, sTrace_log.add_ind);
				strcpy(sLn_mst_hst.ct_ind, sTrace_log.ct_ind);
				sLn_mst_hst.tx_amt = sTrace_log.amt;

				if ( sTrace_log.add_ind[0] == '0' )
				{
					sLn_mst_hst.bal = sLn_mst.hst_bal - sTrace_log.amt;
				}
				if ( sTrace_log.add_ind[0] == '1' )
				{
					sLn_mst_hst.bal = sLn_mst.hst_bal + sTrace_log.amt;
				}

				sLn_mst_hst.intst_acm = sLn_mst.intst_acm;
				sLn_mst_hst.tx_date = sTrace_log.tx_date;
				sLn_mst_hst.tx_time = sTrace_log.tx_time;
				strcpy(sLn_mst_hst.note_type, sTrace_log.note_type);
				strcpy(sLn_mst_hst.note_no, sTrace_log.note_no);
				sLn_mst_hst.hst_cnt = sLn_mst.hst_cnt + 1;
				strcpy(sLn_mst_hst.brf, sTrace_log.brf);
				strcpy(sLn_mst_hst.tel, sTrace_log.tel);
				strcpy(sLn_mst_hst.chk, sTrace_log.chk);
				strcpy(sLn_mst_hst.auth, sTrace_log.aut);

				ret = Dd_mst_hst_Ins( sLn_mst_hst , g_pub_tx.reply );
				if( ret )
				{
					sprintf( acErrMsg, "插入贷款明细文件失败!!!ac_id = [%d] and ac_seqn = [%d]" ,sTrace_log.ac_id,sTrace_log.ac_seqn);
					WRITEMSG
					goto ErrExit;
				}

				ret = sql_execute ("update ln_mst set hst_bal = %f,hst_cnt = %d", sLn_mst_hst.bal,sLn_mst_hst.hst_cnt);
				if( ret )
				{
					sprintf( acErrMsg, "修改贷款主文件失败!!ac_id = [%d] and ac_seqn = [%d]" ,sTrace_log.ac_id,sTrace_log.ac_seqn);
					WRITEMSG
					goto ErrExit;
				}

				break;
			case '9':		/*内部账*/

				memset(&sIn_mst, 0x00, sizeof(sIn_mst) );
				memset(&sIn_mst_hst, 0x00, sizeof(sIn_mst_hst) );

				/*-----查询内部账主文件-----*/
				ret = Dd_mst_Sel( g_pub_tx.reply, &sIn_mst, \
						"ac_id = %d and ac_seqn = %d", \
						sTrace_log.ac_id,sTrace_log.ac_seqn);
				if( ret )
				{
					sprintf( acErrMsg, "执行Dd_mst_Sel失败!!!ac_id = [%d] and ac_seqn = [%d]" ,sTrace_log.ac_id,sTrace_log.ac_seqn);
					WRITEMSG
					goto ErrExit;
				}

				sIn_mst_hst.trace_no = sTrace_log.trace_no;
				sIn_mst_hst.trace_cnt = sTrace_log.trace_cnt;
				strcpy(sIn_mst_hst.opn_br_no, sIn_mst.opn_br_no);
				strcpy(sIn_mst_hst.tx_br_no, sTrace_log.tx_br_no);
				sIn_mst_hst.ac_id = sTrace_log.ac_id;
				sIn_mst_hst.ac_seqn = sTrace_log.ac_seqn;
				strcpy(sIn_mst_hst.tx_code, sTrace_log.tx_code);
				strcpy(sIn_mst_hst.sub_tx_code, sTrace_log.sub_tx_code);
				strcpy(sIn_mst_hst.add_ind, sTrace_log.add_ind);
				strcpy(sIn_mst_hst.ct_ind, sTrace_log.ct_ind);
				sIn_mst_hst.tx_amt = sTrace_log.amt;

				if ( sTrace_log.add_ind[0] == '0' )
				{
					sIn_mst_hst.bal = sIn_mst.hst_bal - sTrace_log.amt;
				}
				if ( sTrace_log.add_ind[0] == '1' )
				{
					sIn_mst_hst.bal = sIn_mst.hst_bal + sTrace_log.amt;
				}

				sIn_mst_hst.intst_acm = sIn_mst.intst_acm;
				sIn_mst_hst.tx_date = sTrace_log.tx_date;
				sIn_mst_hst.tx_time = sTrace_log.tx_time;
				strcpy(sIn_mst_hst.note_type, sTrace_log.note_type);
				strcpy(sIn_mst_hst.note_no, sTrace_log.note_no);
				sIn_mst_hst.hst_cnt = sIn_mst.hst_cnt + 1;
				strcpy(sIn_mst_hst.brf, sTrace_log.brf);
				strcpy(sIn_mst_hst.tel, sTrace_log.tel);
				strcpy(sIn_mst_hst.chk, sTrace_log.chk);
				strcpy(sIn_mst_hst.auth, sTrace_log.aut);

				ret = Dd_mst_hst_Ins( sIn_mst_hst , g_pub_tx.reply );
				if( ret )
				{
					sprintf( acErrMsg, "插入内部账明细文件失败!!!ac_id = [%d] and ac_seqn = [%d]" ,sTrace_log.ac_id,sTrace_log.ac_seqn);
					WRITEMSG
					goto ErrExit;
				}

				ret = sql_execute ("update in_mst set hst_bal = %f,hst_cnt = %d", sIn_mst_hst.bal,sIn_mst_hst.hst_cnt);
				if( ret )
				{
					sprintf( acErrMsg, "修改内部账主文件失败!!ac_id = [%d] and ac_seqn = [%d]" ,sTrace_log.ac_id,sTrace_log.ac_seqn);
					WRITEMSG
					goto ErrExit;
				}

				break;
			default:		/*----不处理透支----*/
				sprintf( acErrMsg, "账户类型错!!!" );
				WRITEMSG
				goto ErrExit;

		}

	}

	Trace_log_Clo_Sel ();

OkExit:
	sql_commit();	/*--提交事务--*/
	return 0;

ErrExit:
	sql_rollback();	/*--事务回滚--*/
	Trace_log_Clo_Sel ();
	return 1;
}

/*------------------------------------------------
* 功能：根据交易流水按账号ID和账号序号汇总入明细
*
*
*-----------------------------------------------*/
int HZ_mst_hst()
{
	int ret = 0;

	struct trace_log_c sTrace_log;
	struct dd_mst_c sDd_mst;
	struct td_mst_c sTd_mst;
	struct ln_mst_c sLn_mst;
	struct in_mst_c sIn_mst;

	struct dd_mst_hst_c sDd_mst_hst;
	struct td_mst_hst_c sTd_mst_hst;
	struct ln_mst_hst_c sLn_mst_hst;
	struct in_mst_hst_c sIn_mst_hst;

	double tram = 0.00;

	struct trace_log_c tTrace_log;

	ret=sql_begin(); /*打开事务*/
	if( ret ) 
	{
		sprintf( acErrMsg, "打开事务失败!!!" );
		WRITEMSG
		goto ErrExit;
	}

	ret = Trace_log_Dec_Sel ( g_pub_tx.reply , "hst_ind = '2' \
			and tx_date = %ld and sts = '0' \
			order by ac_id,ac_seqn,add_ind", g_pub_tx.tx_date);
	if( ret )
	{
		sprintf( acErrMsg, "执行Trace_log_Dec_Sel失败!!!");
		WRITEMSG
		goto ErrExit;
	}

	memset(&tTrace_log,0x00,sizeof(tTrace_log));

	ret = Trace_log_Fet_Sel ( &tTrace_log, g_pub_tx.reply );
	if( ret )
	{
		sprintf( acErrMsg, "执行Trace_log_Fet_Sel失败!!!");
		WRITEMSG
		goto ErrExit;
	}

	tram = tTrace_log.amt;

	memcpy( &sTrace_log, &tTrace_log, sizeof(sTrace_log) );

	while(1)
	{
		memset(&tTrace_log,0x00,sizeof(tTrace_log));

		ret = Trace_log_Fet_Sel ( &tTrace_log, g_pub_tx.reply );
		if ( ret == 100 )
		{
			break;
		}
		if( ret )
		{
			sprintf( acErrMsg, "执行Trace_log_Fet_Sel失败!!!");
			WRITEMSG
			goto ErrExit;
		}

		if ( (tTrace_log.ac_id == sTrace_log.ac_id) && \
			 ( tTrace_log.ac_seqn == sTrace_log.ac_seqn) && \
			 ( strcmp( tTrace_log.add_ind, sTrace_log.add_ind) ==0 ) ) 
		{
			tram = tram + tTrace_log.amt;
			continue;
		}

		ret = pub_base_disac_file ( sTrace_log.ac_no, sTrace_log.ac_seqn, g_pub_tx.ac_type );
		if( ret )
		{
			sprintf( acErrMsg, "执行Trace_log_Fet_Sel失败!!!");
			WRITEMSG
			goto ErrExit;
		}

		switch( g_pub_tx.ac_type[0] )
		{
			case '1':		/*活期*/
				memset(&sDd_mst, 0x00, sizeof(sDd_mst) );
				memset(&sDd_mst_hst, 0x00, sizeof(sDd_mst_hst) );

				/*-----查询活期主文件-----*/
				ret = Dd_mst_Sel( g_pub_tx.reply, &sDd_mst, \
						"ac_id = %d and ac_seqn = %d", \
						sTrace_log.ac_id,sTrace_log.ac_seqn);
				if( ret )
				{
					sprintf( acErrMsg, "执行Dd_mst_Sel失败!!!ac_id = [%d] and ac_seqn = [%d]" ,sTrace_log.ac_id,sTrace_log.ac_seqn);
					WRITEMSG
					goto ErrExit;
				}

				sDd_mst_hst.trace_no = sTrace_log.trace_no;
				sDd_mst_hst.trace_cnt = sTrace_log.trace_cnt;
				strcpy(sDd_mst_hst.opn_br_no, sDd_mst.opn_br_no);
				strcpy(sDd_mst_hst.tx_br_no, sTrace_log.tx_br_no);
				sDd_mst_hst.ac_id = sTrace_log.ac_id;
				sDd_mst_hst.ac_seqn = sTrace_log.ac_seqn;
				strcpy(sDd_mst_hst.tx_code, sTrace_log.tx_code);
				strcpy(sDd_mst_hst.sub_tx_code, sTrace_log.sub_tx_code);
				strcpy(sDd_mst_hst.add_ind, sTrace_log.add_ind);
				strcpy(sDd_mst_hst.ct_ind, sTrace_log.ct_ind);

				sDd_mst_hst.tx_amt = tram;

				if ( sTrace_log.add_ind[0] == '0' )
				{
					sDd_mst_hst.bal = sDd_mst.hst_bal - tram;
				}
				if ( sTrace_log.add_ind[0] == '1' )
				{
					sDd_mst_hst.bal = sDd_mst.hst_bal + tram;
				}

				sDd_mst_hst.acbo_bal = 0.00;
				sDd_mst_hst.intst_acm = sDd_mst.intst_acm;
				sDd_mst_hst.tx_date = sTrace_log.tx_date;
				sDd_mst_hst.tx_time = sTrace_log.tx_time;
				strcpy(sDd_mst_hst.note_type, sTrace_log.note_type);
				strcpy(sDd_mst_hst.note_no, sTrace_log.note_no);
				sDd_mst_hst.hst_cnt = sDd_mst.hst_cnt + 1;
				strcpy(sDd_mst_hst.brf, sTrace_log.brf);
				strcpy(sDd_mst_hst.tel, sTrace_log.tel);
				strcpy(sDd_mst_hst.chk, sTrace_log.chk);
				strcpy(sDd_mst_hst.auth, sTrace_log.aut);

				ret = Dd_mst_hst_Ins( sDd_mst_hst , g_pub_tx.reply );
				if( ret )
				{
					sprintf( acErrMsg, "插入活期明细文件失败!!!ac_id = [%d] and ac_seqn = [%d]" ,sTrace_log.ac_id,sTrace_log.ac_seqn);
					WRITEMSG
					goto ErrExit;
				}

				ret = sql_execute ("update dd_mst set hst_bal = %f,hst_cnt = %d", sDd_mst_hst.bal,sDd_mst_hst.hst_cnt);
				if( ret )
				{
					sprintf( acErrMsg, "修改活期主文件失败!!ac_id = [%d] and ac_seqn = [%d]" ,sTrace_log.ac_id,sTrace_log.ac_seqn);
					WRITEMSG
					goto ErrExit;
				}

				break;
			case '2':		/*定期*/
				memset(&sTd_mst, 0x00, sizeof(sTd_mst) );
				memset(&sTd_mst_hst, 0x00, sizeof(sTd_mst_hst) );

				/*-----查询定期主文件-----*/
				ret = Dd_mst_Sel( g_pub_tx.reply, &sTd_mst, \
						"ac_id = %d and ac_seqn = %d", \
						sTrace_log.ac_id,sTrace_log.ac_seqn);
				if( ret )
				{
					sprintf( acErrMsg, "执行Dd_mst_Sel失败!!!ac_id = [%d] and ac_seqn = [%d]" ,sTrace_log.ac_id,sTrace_log.ac_seqn);
					WRITEMSG
					goto ErrExit;
				}

				sTd_mst_hst.trace_no = sTrace_log.trace_no;
				sTd_mst_hst.trace_cnt = sTrace_log.trace_cnt;
				strcpy(sTd_mst_hst.opn_br_no, sTd_mst.opn_br_no);
				strcpy(sTd_mst_hst.tx_br_no, sTrace_log.tx_br_no);
				sTd_mst_hst.ac_id = sTrace_log.ac_id;
				sTd_mst_hst.ac_seqn = sTrace_log.ac_seqn;
				strcpy(sTd_mst_hst.tx_code, sTrace_log.tx_code);
				strcpy(sTd_mst_hst.sub_tx_code, sTrace_log.sub_tx_code);
				strcpy(sTd_mst_hst.add_ind, sTrace_log.add_ind);
				strcpy(sTd_mst_hst.ct_ind, sTrace_log.ct_ind);
				sTd_mst_hst.tx_amt = tram;

				if ( sTrace_log.add_ind[0] == '0' )
				{
					sTd_mst_hst.bal = sTd_mst.hst_bal - tram;
				}
				if ( sTrace_log.add_ind[0] == '1' )
				{
					sTd_mst_hst.bal = sTd_mst.hst_bal + tram;
				}

				sTd_mst_hst.acbo_bal = 0.00;
				sTd_mst_hst.intst_acm = sTd_mst.intst_acm;
				sTd_mst_hst.tx_date = sTrace_log.tx_date;
				sTd_mst_hst.tx_time = sTrace_log.tx_time;
				strcpy(sTd_mst_hst.note_type, sTrace_log.note_type);
				strcpy(sTd_mst_hst.note_no, sTrace_log.note_no);
				sTd_mst_hst.hst_cnt = sTd_mst.hst_cnt + 1;
				strcpy(sTd_mst_hst.brf, sTrace_log.brf);
				strcpy(sTd_mst_hst.tel, sTrace_log.tel);
				strcpy(sTd_mst_hst.chk, sTrace_log.chk);
				strcpy(sTd_mst_hst.auth, sTrace_log.aut);

				ret = Dd_mst_hst_Ins( sTd_mst_hst , g_pub_tx.reply );
				if( ret )
				{
					sprintf( acErrMsg, "插入定期明细文件失败!!!ac_id = [%d] and ac_seqn = [%d]" ,sTrace_log.ac_id,sTrace_log.ac_seqn);
					WRITEMSG
					goto ErrExit;
				}

				ret = sql_execute ("update td_mst set hst_bal = %f,hst_cnt = %d", sTd_mst_hst.bal,sTd_mst_hst.hst_cnt);
				if( ret )
				{
					sprintf( acErrMsg, "修改定期主文件失败!!ac_id = [%d] and ac_seqn = [%d]" ,sTrace_log.ac_id,sTrace_log.ac_seqn);
					WRITEMSG
					goto ErrExit;
				}

				break;
			case '3':		/*贷款*/

				memset(&sLn_mst, 0x00, sizeof(sLn_mst) );
				memset(&sLn_mst_hst, 0x00, sizeof(sLn_mst_hst) );

				/*-----查询贷款主文件-----*/
				ret = Dd_mst_Sel( g_pub_tx.reply, &sLn_mst, \
						"ac_id = %d and ac_seqn = %d", \
						sTrace_log.ac_id,sTrace_log.ac_seqn);
				if( ret )
				{
					sprintf( acErrMsg, "执行Dd_mst_Sel失败!!!ac_id = [%d] and ac_seqn = [%d]" ,sTrace_log.ac_id,sTrace_log.ac_seqn);
					WRITEMSG
					goto ErrExit;
				}

				sLn_mst_hst.trace_no = sTrace_log.trace_no;
				sLn_mst_hst.trace_cnt = sTrace_log.trace_cnt;
				strcpy(sLn_mst_hst.opn_br_no, sLn_mst.opn_br_no);
				strcpy(sLn_mst_hst.tx_br_no, sTrace_log.tx_br_no);
				sLn_mst_hst.ac_id = sTrace_log.ac_id;
				sLn_mst_hst.ac_seqn = sTrace_log.ac_seqn;
				strcpy(sLn_mst_hst.tx_code, sTrace_log.tx_code);
				strcpy(sLn_mst_hst.sub_tx_code, sTrace_log.sub_tx_code);
				strcpy(sLn_mst_hst.ln_tx_type, "0");	/*--暂定--*/
				strcpy(sLn_mst_hst.add_ind, sTrace_log.add_ind);
				strcpy(sLn_mst_hst.ct_ind, sTrace_log.ct_ind);
				sLn_mst_hst.tx_amt = tram;

				if ( sTrace_log.add_ind[0] == '0' )
				{
					sLn_mst_hst.bal = sLn_mst.hst_bal - tram;
				}
				if ( sTrace_log.add_ind[0] == '1' )
				{
					sLn_mst_hst.bal = sLn_mst.hst_bal + tram;
				}

				sLn_mst_hst.intst_acm = sLn_mst.intst_acm;
				sLn_mst_hst.tx_date = sTrace_log.tx_date;
				sLn_mst_hst.tx_time = sTrace_log.tx_time;
				strcpy(sLn_mst_hst.note_type, sTrace_log.note_type);
				strcpy(sLn_mst_hst.note_no, sTrace_log.note_no);
				sLn_mst_hst.hst_cnt = sLn_mst.hst_cnt + 1;
				strcpy(sLn_mst_hst.brf, sTrace_log.brf);
				strcpy(sLn_mst_hst.tel, sTrace_log.tel);
				strcpy(sLn_mst_hst.chk, sTrace_log.chk);
				strcpy(sLn_mst_hst.auth, sTrace_log.aut);

				ret = Dd_mst_hst_Ins( sLn_mst_hst , g_pub_tx.reply );
				if( ret )
				{
					sprintf( acErrMsg, "插入贷款明细文件失败!!!ac_id = [%d] and ac_seqn = [%d]" ,sTrace_log.ac_id,sTrace_log.ac_seqn);
					WRITEMSG
					goto ErrExit;
				}

				ret = sql_execute ("update ln_mst set hst_bal = %f,hst_cnt = %d", sLn_mst_hst.bal,sLn_mst_hst.hst_cnt);
				if( ret )
				{
					sprintf( acErrMsg, "修改贷款主文件失败!!ac_id = [%d] and ac_seqn = [%d]" ,sTrace_log.ac_id,sTrace_log.ac_seqn);
					WRITEMSG
					goto ErrExit;
				}

				break;
			case '9':		/*内部账*/

				memset(&sIn_mst, 0x00, sizeof(sIn_mst) );
				memset(&sIn_mst_hst, 0x00, sizeof(sIn_mst_hst) );

				/*-----查询内部账主文件-----*/
				ret = Dd_mst_Sel( g_pub_tx.reply, &sIn_mst, \
						"ac_id = %d and ac_seqn = %d", \
						sTrace_log.ac_id,sTrace_log.ac_seqn);
				if( ret )
				{
					sprintf( acErrMsg, "执行Dd_mst_Sel失败!!!ac_id = [%d] and ac_seqn = [%d]" ,sTrace_log.ac_id,sTrace_log.ac_seqn);
					WRITEMSG
					goto ErrExit;
				}

				sIn_mst_hst.trace_no = sTrace_log.trace_no;
				sIn_mst_hst.trace_cnt = sTrace_log.trace_cnt;
				strcpy(sIn_mst_hst.opn_br_no, sIn_mst.opn_br_no);
				strcpy(sIn_mst_hst.tx_br_no, sTrace_log.tx_br_no);
				sIn_mst_hst.ac_id = sTrace_log.ac_id;
				sIn_mst_hst.ac_seqn = sTrace_log.ac_seqn;
				strcpy(sIn_mst_hst.tx_code, sTrace_log.tx_code);
				strcpy(sIn_mst_hst.sub_tx_code, sTrace_log.sub_tx_code);
				strcpy(sIn_mst_hst.add_ind, sTrace_log.add_ind);
				strcpy(sIn_mst_hst.ct_ind, sTrace_log.ct_ind);
				sIn_mst_hst.tx_amt = tram;

				if ( sTrace_log.add_ind[0] == '0' )
				{
					sIn_mst_hst.bal = sIn_mst.hst_bal - tram;
				}
				if ( sTrace_log.add_ind[0] == '1' )
				{
					sIn_mst_hst.bal = sIn_mst.hst_bal + tram;
				}

				sIn_mst_hst.intst_acm = sIn_mst.intst_acm;
				sIn_mst_hst.tx_date = sTrace_log.tx_date;
				sIn_mst_hst.tx_time = sTrace_log.tx_time;
				strcpy(sIn_mst_hst.note_type, sTrace_log.note_type);
				strcpy(sIn_mst_hst.note_no, sTrace_log.note_no);
				sIn_mst_hst.hst_cnt = sIn_mst.hst_cnt + 1;
				strcpy(sIn_mst_hst.brf, sTrace_log.brf);
				strcpy(sIn_mst_hst.tel, sTrace_log.tel);
				strcpy(sIn_mst_hst.chk, sTrace_log.chk);
				strcpy(sIn_mst_hst.auth, sTrace_log.aut);

				ret = Dd_mst_hst_Ins( sIn_mst_hst , g_pub_tx.reply );
				if( ret )
				{
					sprintf( acErrMsg, "插入内部账明细文件失败!!!ac_id = [%d] and ac_seqn = [%d]" ,sTrace_log.ac_id,sTrace_log.ac_seqn);
					WRITEMSG
					goto ErrExit;
				}

				ret = sql_execute ("update in_mst set hst_bal = %f,hst_cnt = %d", sIn_mst_hst.bal,sIn_mst_hst.hst_cnt);
				if( ret )
				{
					sprintf( acErrMsg, "修改内部账主文件失败!!ac_id = [%d] and ac_seqn = [%d]" ,sTrace_log.ac_id,sTrace_log.ac_seqn);
					WRITEMSG
					goto ErrExit;
				}

				break;
			default:		/*----不处理透支----*/
				sprintf( acErrMsg, "账户类型错!!!" );
				WRITEMSG
				goto ErrExit;

		}

		memset( &sTrace_log,0x00,sizeof(sTrace_log) );
		memcpy( &sTrace_log, &tTrace_log, sizeof(sTrace_log) );

		tram = tTrace_log.amt;

	}

	Trace_log_Clo_Sel ();

OkExit:
	sql_commit();	/*--提交事务--*/
	return 0;

ErrExit:
	sql_rollback();	/*--事务回滚--*/
	Trace_log_Clo_Sel ();
	return 1;
}
