/*************************************************
* 文 件 名:  spN107.c
* 功能描述： 
*	凭证销毁
*	将支行上缴的凭证由总行作统一出库销毁，只销毁作废
*	凭证或者收回凭证。
*
* 作    者:  andy
* 完成日期： 2004年1月10日
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
#include "note_mst_c.h"
#include "note_mst_hst_c.h"
#include "com_branch_c.h"

int spN107()
{
	struct 	note_mst_c	sNoteMst;
	char	cNoteAct[2];		/* 凭证行为 */
	char	cTwBrno[6];		/* 对方机构 */
	char	cTwTel[5];		/* 对方柜员 */
	int	ret;			/* 函数返回值 */

	memset( &sNoteMst, 0x00, sizeof( struct note_mst_c ) );
	memset( cNoteAct, 0x00, sizeof( cNoteAct ) );
	memset( cTwBrno, 0x00, sizeof( cTwBrno ) );
	memset( cTwTel, 0x00, sizeof( cTwTel ) );
	
	/* 初始化g_pub_tx结构 */
	init_pub_tx( );
	
	/**本方权限检查**/
	if( pub_com_NoteAuthChk( g_pub_tx.tx_br_no,g_pub_tx.tel,101,0 ) )
			goto ErrExit;

	strcpy( cNoteAct, "M" );	/* M表示销毁凭证 */
		
	strcpy( sNoteMst.tel , g_pub_tx.tel );
	strcpy( sNoteMst.br_no, g_pub_tx.tx_br_no );
	strcpy( sNoteMst.note_type, g_pub_tx.note_type );
	strcpy( sNoteMst.beg_note_no, g_pub_tx.beg_note_no );
	strcpy( sNoteMst.end_note_no, g_pub_tx.end_note_no );
	sNoteMst.cnt = (long)g_pub_tx.tx_amt1;

	/* 凭证有效性检查:是否存在,是否处于使用状态 */
	ret = pub_com_ChkNoteUse( g_pub_tx.note_type );
	if ( ret )
	{
		sprintf( acErrMsg, "检查凭证是否存在，是否启用错" );
		WRITEMSG
		goto ErrExit;		
	}
	
	/* 凭证起始号码、终止号码冠子头检查 */
	ret=pub_com_ChkNoteHead( g_pub_tx.note_type, g_pub_tx.beg_note_no, g_pub_tx.tel );
	if( ret )
	{
		sprintf( acErrMsg, "检查凭证起始号码冠子头错" );
		WRITEMSG
		goto ErrExit;	
	}
	ret=pub_com_ChkNoteHead( g_pub_tx.note_type, g_pub_tx.end_note_no, g_pub_tx.tel );
	if( ret )	
	{
		sprintf( acErrMsg, "检查凭证终止号码冠子头错" );
		WRITEMSG
		goto ErrExit;	
	}
			
	/* 检查操作员是否拥有这批凭证 */
	/* 并且状态为正常 */
	ret = pub_com_ChkNote( cNoteAct, sNoteMst );
	if( ret )
	{
		sprintf( acErrMsg, "检查柜员凭证错" );
		WRITEMSG
		goto ErrExit;
	}
	
	/* 分段处理(修改凭证状态) */
	ret = pub_com_NoteApart( cNoteAct, sNoteMst );
	if( ret )
	{
		sprintf( acErrMsg, "分段处理(修改凭证状态)错" );
		WRITEMSG
		goto ErrExit;;
	}

	/* 登记柜员凭证明细 */
	ret=pub_com_RegNoteHst( cNoteAct, sNoteMst, cTwBrno, cTwTel );
	if ( ret == -1 )
	{
		sprintf( acErrMsg, "登记柜员凭证明细错" );
		WRITEMSG
		goto ErrExit;;
	}
	
	
	/* 写交易日志 */
	ret = pub_ins_trace_log();
	if ( ret )
	{
		sprintf( acErrMsg, "记流水日志错" );
		WRITEMSG
		goto ErrExit;;
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

/**********************************************************************
 * 函  数 名：  init_pub_tx
 * 函数 功能:   初始化g_pub_tx结构，并且从屏幕取值放入g_pub_tx中
 *             
 * 作者/时间：  andy/2004年1月7日
 * 
 * 参  数：
 *     输入：  
 *
 *     输出：  
 *
 *   返回值：  0: 成功   -1: 失败
 *           
 * 修改历史：  包括修改人、时间、修改内容，以时间倒序方式
 *
********************************************************************/
static int init_pub_tx( )
{
	long	lNoteCnt;		/* 从前台域中取凭证数量 */
	 
	pub_base_sysinit();

	get_zd_data( "0890", g_pub_tx.note_type );	/* 凭证类型 */
	get_zd_data( "0580", g_pub_tx.beg_note_no );
	get_zd_data( "0590", g_pub_tx.end_note_no );
	get_zd_long( "0520", &lNoteCnt );	/* 凭证数量 */
	g_pub_tx.tx_amt1 = lNoteCnt;

	return	0;
}
