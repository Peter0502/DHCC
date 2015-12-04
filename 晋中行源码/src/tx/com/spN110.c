/*************************************************
* 文 件 名:  spN110.c
* 功能描述： 柜员全部或部分凭证注销使用此交易
*
* 作    者:  andy
* 完成日期： 2004年3月25日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "note_mst_c.h"

int spN110()
{
	struct	note_mst_c	sNoteMst;	/* 柜员凭证台账 */
	char	who[6];				/* 使用者 */
	char    cNoteSts[2];            	/* 凭证状态 */
	char    cPNoteSts[2];            	/* 凭证状态 */
	char	cNoteAct[2];			/* 凭证行为 */
	int	ret;

	memset( &sNoteMst, 0x00, sizeof( struct note_mst_c ) );
	memset( who, 0x00, sizeof( who ) );
	memset( cNoteSts, 0x00, sizeof( cNoteSts ) );
	memset( cPNoteSts, 0x00, sizeof( cPNoteSts ) );
	memset( cNoteAct, 0x00, sizeof( cNoteAct ) );
	
	strcpy( cNoteAct, "D" );		/* "D"表示使用 */
	
	/* 系统初始化（初始化g_pub_tx)数据初始化 */
	init_pub_tx( );	

	/**本方权限检查**/
	if( pub_com_NoteAuthChk( g_pub_tx.tx_br_no,g_pub_tx.tel,401,0 ) )
			goto ErrExit;

	/* 根据凭证行为返回对应凭证状态 */
	strcpy( who, "T" );			/* "T"表示柜员行为 */

	strcpy( g_pub_tx.brf,"注销" );
	strcpy( g_pub_tx.add_ind,"0" );
	strcpy( g_pub_tx.ac_no,"702" );

	ret=pub_com_NoteActSts( who, cNoteAct, cNoteSts, cPNoteSts );
	if ( ret )
	{
		sprintf( acErrMsg, "根据凭证行为返回对应凭证状态错" );
		WRITEMSG
		goto ErrExit;
	}
	
	/* 柜员凭证检查 */
	ret=pub_com_ChkNoteUse( g_pub_tx.note_type );
	if( ret )
	{
		sprintf( acErrMsg, "柜员凭证检查错" );
		WRITEMSG
		goto ErrExit;
	}
	if( cPNoteSts[0]!='0' )				/* 凭证状态应为正常 */
	{
		sprintf( acErrMsg, "凭证状态错" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "N022" );
		goto ErrExit;
	}
sprintf( acErrMsg, "note_type='%s' and beg_note_no='%s'\
	and end_note_no='%s' and tel='%s'", g_pub_tx.note_type, g_pub_tx.beg_note_no,\
	g_pub_tx.end_note_no, g_pub_tx.tel );
WRITEMSG
	/* 检查所输凭证是否属于交易柜员 */
	ret = pub_com_ChkNtExt( g_pub_tx.beg_note_no, g_pub_tx.end_note_no, \
				g_pub_tx.note_type, g_pub_tx.tx_br_no, g_pub_tx.tel );
	if( ret )
	{
		sprintf( acErrMsg, "调用pub_com_ChkNtExt错误!ret=[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	/* 柜员凭证销号处理 */
	ret=pub_com_NoteClsNo( g_pub_tx.note_type, g_pub_tx.beg_note_no, \
				g_pub_tx.end_note_no, g_pub_tx.tel );
	if( ret )
	{
		sprintf( acErrMsg, "柜员凭证销号处理错" );
		WRITEMSG
		goto ErrExit;
	}

	/* 记流水日志 */
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

/**********************************************************************
 * 函  数 名：  init_pub_tx
 * 函数 功能:   初始化g_pub_tx结构，并且从屏幕取值放入g_pub_tx中
 *             
 * 作者/时间：  andy/2004年3月25日
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
	long	lCount;
	
	pub_base_sysinit();

	get_zd_data( "0890", g_pub_tx.note_type );		/* 凭证种类 */
	get_zd_data( "0580", g_pub_tx.beg_note_no );		/* 起始号码 */
	get_zd_data( "0590", g_pub_tx.end_note_no );		/* 终止号码 */
	get_zd_long( "0520", &lCount );				/* 凭证数量 */
	g_pub_tx.tx_amt1 = lCount; 
	return	0; 
}
