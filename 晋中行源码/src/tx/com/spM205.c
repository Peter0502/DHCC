/*************************************************
* 文 件 名:  spM205.c
* 功能描述： 提供本行内部进行现金调缴预开户的功能。
*
* 作    者:  andy
* 完成日期： 2004年1月6日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "com_tel_c.h"
#include "com_tx_br_rel_c.h"
#include "cash_book_c.h"
#include "com_branch_c.h"
#include "com_sys_parm_c.h"

int spM205()
{
	struct	com_tel_c		sComTel;		/* 柜员信息 */
	struct	com_tx_br_rel_c	sComTxBrRel;	/* 交易机构关系表 */
	struct	cash_book_c		sCashBook;		/* 现金收付登记簿 */
	struct	com_branch_c	sComBranch;		/* 公共机构码表 */
	char	cOperCode[2];					/* 操作码 */
	char	cTwBrNo[6];						/* 对方机构代码 */
	long	lNoteNo;						/* 凭证号码 */
	long	lYtDate;						/* 预开票打印日期 */
	int		ret;
	struct  com_sys_parm_c sys_parm;

	memset( &sComTel, 0x00, sizeof( struct com_tel_c ) );
	memset( &sComTxBrRel, 0x00, sizeof( struct com_tx_br_rel_c ) );
	memset( &sCashBook, 0x00, sizeof( struct cash_book_c ) );
	memset( &sComBranch, 0x00, sizeof( struct com_branch_c ) );
	memset( cOperCode, 0x00, sizeof( cOperCode ) );
	memset( cTwBrNo, 0x00, sizeof( cTwBrNo ) );
	memset( &g_pub_tx, 0x00, sizeof( g_pub_tx ) );

	/* 数据初始化 */
	pub_base_sysinit();
	ret = Com_sys_parm_Sel(g_pub_tx.reply,&sys_parm,"1=1");
	if( ret ) goto ErrExit;

	/* 从前台屏幕取值 */
	get_zd_data( "0670", cOperCode );		/* 操作码 */
	get_zd_data( "0910", cTwBrNo );			/* 对方机构代码 */
	get_zd_data( "1181", sCashBook.cur_no );/* 币种 */
	get_zd_long( "0440", &lNoteNo );		/* 凭证号码 */
	get_zd_double( "1184", &sCashBook.amt );/* 金额 */

	if( cOperCode[0] == '0' )	
	{
		strcpy( g_pub_tx.brf,"预开票" );
		g_pub_tx.tx_amt1=sCashBook.amt ;
		/* 检查交易机构关系表中是否存在上下级关系 */
		ret = Com_tx_br_rel_Sel( g_pub_tx.reply, &sComTxBrRel, 
			"br_no='%s' and up_br_no='%s' and rel_type=1", 
			g_pub_tx.tx_br_no, cTwBrNo );
		if( ret == 0 )
		{
			sprintf( acErrMsg,"不允许下级机构预开票!本级构[%s]上级机构[%s]",
				g_pub_tx.tx_br_no, cTwBrNo );
			WRITEMSG
			strcpy( g_pub_tx.reply, "N094" );
			goto ErrExit;
		}else if( ret != 100 && ret )
		{
			sprintf( acErrMsg, "执行Com_tx_br_rel_Sel错!ret=[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}

		/* 检查交易机构关系表中是否存在上下级关系 */
		ret = Com_tx_br_rel_Sel( g_pub_tx.reply, &sComTxBrRel, 
			"br_no='%s' and up_br_no='%s' and rel_type=1", 
				cTwBrNo,g_pub_tx.tx_br_no );
		if( ret != 100 && ret )
		{
			sprintf( acErrMsg, "执行Com_tx_br_rel_Sel错!ret=[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}else if( ret == 100 )
		{
			sprintf( acErrMsg, "检查交易机构关系表中不存在此上下级关系![%s][%s]",cTwBrNo,g_pub_tx.tx_br_no );
			WRITEMSG
			strcpy( g_pub_tx.reply, "N068" );
			goto ErrExit;
		}

		strcpy( sCashBook.tw_br_no, cTwBrNo );		/* 对方机构代码 */
		strcpy( sCashBook.br_no, g_pub_tx.tx_br_no);/* 交易机构 */
		strcpy( sCashBook.tel, g_pub_tx.tel );		/* 柜员号码 */
		strcpy( sCashBook.ind, "2" );				/* 获取标志: 2 预开*/
		sCashBook.trace_no = g_pub_tx.trace_no;		/* 流水号，现金调拨单号码 */
		sCashBook.wrk_no = 0;		/* 流水号 */
		sCashBook.out_no = 0;		/* 流水号 */
		sCashBook.tx_date = g_pub_tx.tx_date;
		set_zd_long("0440",sCashBook.trace_no);
		/* 为打印现金调拨单赋值 */
		set_zd_data( "0580", "调头寸" );			/* 用途 */
		ret = pub_base_deadlineD( g_pub_tx.tx_date, 1, &lYtDate);
		if( ret )
		{
			sprintf( acErrMsg, "计算交易日期的下一日错误!" );
			WRITEMSG
			goto ErrExit;
		}
		set_zd_long( "0450", lYtDate );				/* 预开票打印日期 */

		/* 登记现金收付登记簿 */
		ret = Cash_book_Ins( sCashBook, g_pub_tx.reply );
		if( ret )
		{
			sprintf( acErrMsg, "执行Cash_book_Ins错!ret=[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}
	}
	else		
	{
		strcpy( g_pub_tx.brf,"核销预开票" );
		vtcp_log( "YKPHX核销预开票" );
		g_pub_tx.tx_amt1=sCashBook.amt ;
		/* 查询现金收付登记簿中是否存在与输入的对方机构代码，
		金额一致并且获取标志为"未取"的纪录 */
		vtcp_log( "tx_date=%d and br_no='%s' and tw_br_no='%s' and amt=%.2lf and ind='2'", sys_parm.lst_date,g_pub_tx.tx_br_no,cTwBrNo,sCashBook.amt );
		ret = Cash_book_Dec_Upd( g_pub_tx.reply, "tx_date=%d and br_no='%s' and tw_br_no='%s' and amt='%.2lf'and ind='2'",sys_parm.lst_date,g_pub_tx.tx_br_no,cTwBrNo,sCashBook.amt);
		if( ret )
		{
			sprintf( acErrMsg, "执行Cash_book_Dec_Upd错!ret=[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}

		ret = Cash_book_Fet_Upd( &sCashBook, g_pub_tx.reply );
		if ( ret == 100 )
    		{
       		sprintf( acErrMsg, "现金收付登记簿中不存在此付出记录!" );
       		WRITEMSG
       		strcpy( g_pub_tx.reply, "M026" );
       		goto ErrExit;
    		}else if ( ret )
    		{
       		sprintf( acErrMsg, "执行Cash_book_Fet_Upd错!ret=[%d]",ret );
       		WRITEMSG
       		goto ErrExit;
    		}else
    		{
    		if( sCashBook.trace_no != lNoteNo )
			{
				sprintf( acErrMsg, "所输凭证号码有误![%ld]",lNoteNo );
				WRITEMSG
				strcpy( g_pub_tx.reply, "M020" );
				goto ErrExit;
			}
			
			strcpy( sCashBook.ind, "3" );	/* 获取标志: 核销*/
			sCashBook.out_no=g_pub_tx.trace_no;

    		ret = Cash_book_Upd_Upd( sCashBook, g_pub_tx.reply );
    		if( ret )
			{
				sprintf( acErrMsg, "执行Cash_book_Upd_Upd错!ret=[%d]", ret );
				WRITEMSG
				goto ErrExit;
			}
			sCashBook.tx_date=g_pub_tx.tx_date;
			sCashBook.out_no=g_pub_tx.trace_no;
			sCashBook.wrk_no=0;
			strcpy( sCashBook.ind, "0" );	/* 获取标志: 0 未取 1 已取 */
			/* 登记现金收付登记簿 */
			ret = Cash_book_Ins( sCashBook, g_pub_tx.reply );
			if( ret )
			{
				sprintf( acErrMsg, "执行Cash_book_Ins错!ret=[%d]", ret );
				WRITEMSG
				goto ErrExit;
			}
    	}

    	Cash_book_Clo_Upd( );

	}

	/* 记流水日志 */
	strcpy(g_pub_tx.no_show,"0");
	ret = pub_ins_trace_log();
	if ( ret )
	{
		sprintf( acErrMsg, "记流水日志错" );
		WRITEMSG
		goto ErrExit;
	}

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
