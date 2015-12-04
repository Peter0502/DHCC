/*************************************************
* 文 件 名:  spN101.c
* 功能描述： 凭证由系统外进入系统使用此交易;入库的前提是凭证都不出现
*	     在柜员凭证台帐或者客户凭证台帐中，或者已经销毁。
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
#include "note_mst_c.h"
#include "com_branch_c.h"

int spN101()
{
	struct	note_mst_c	sNoteMst;	/* 柜员凭证台帐 */
	char	cNoteAct[2];			/* 凭证行为 */
	char	tw_br_no[6];			/* 对方机构号 */
	char	tw_tel[6];			/* 对方操作员 */
	char	cInOut[2];       		/* 转移标志 */
	int	ret;
	
	memset( &sNoteMst, 0x00, sizeof( struct note_mst_c ) );
	memset( cNoteAct, 0x00, sizeof( cNoteAct ) );
	memset( tw_br_no, 0x00, sizeof( tw_br_no ) );
	memset( tw_tel, 0x00, sizeof( tw_tel ) );
	memset( cInOut, 0x00, sizeof( cInOut ) );
	
	strcpy( cNoteAct, "1" );		/* "1"表示为凭证入库操作 */
	strcpy( cInOut, "I" );			/* I：转入 O：转出 */
	
	/* 系统初始化（初始化g_pub_tx)数据初始化 */
	ret = init_pub_tx( );	
	if( ret )	
	{
		sprintf( acErrMsg, "初始化g_pub_tx错" );
		WRITEMSG
		goto ErrExit;
	}

	/**权限检查**/
	if( pub_com_NoteAuthChk( g_pub_tx.tx_br_no,g_pub_tx.tel,101,0 ) )
		goto ErrExit;

	/* 凭证起始号码、终止号码检查（检查冠子头） */
	ret=pub_com_ChkNoteHead( g_pub_tx.note_type, g_pub_tx.beg_note_no, g_pub_tx.tel );
	if( ret == -1 )
	{
		sprintf( acErrMsg, "检查凭证起始号码冠子头错" );
		WRITEMSG
		goto ErrExit;	
	}
	ret=pub_com_ChkNoteHead( g_pub_tx.note_type, g_pub_tx.end_note_no, g_pub_tx.tel );
	if( ret == -1 )	
	{
		sprintf( acErrMsg, "检查凭证终止号码冠子头错" );
		WRITEMSG
		goto ErrExit;	
	}
	/* 检查凭证是否存在，是否启用 */
	ret = pub_com_ChkNoteUse( g_pub_tx.note_type );
	if ( ret )
	{
		sprintf( acErrMsg, "检查凭证是否存在，是否启用错" );
		WRITEMSG
		goto ErrExit;		
	}

	/* 柜员是否拥有凭证 */
	strcpy( sNoteMst.tel, g_pub_tx.tel );
	strcpy( sNoteMst.br_no,  g_pub_tx.tx_br_no );
	strcpy( sNoteMst.note_type,  g_pub_tx.note_type );
	strcpy( sNoteMst.beg_note_no,  g_pub_tx.beg_note_no );
	strcpy( sNoteMst.end_note_no,  g_pub_tx.end_note_no );
	sNoteMst.cnt = (long)g_pub_tx.tx_amt1;
	
	/* 检查柜员凭证 */
	ret=pub_com_ChkNote( cNoteAct, sNoteMst );
	if( ret == -1 )
	{
		sprintf( acErrMsg, "检查柜员凭证错" );
		WRITEMSG
		goto ErrExit;
	}
	
	/* 登记柜员凭证台帐 */
	ret=pub_com_RegTelNote( g_pub_tx.tel, g_pub_tx.tx_br_no, cNoteAct, \
				g_pub_tx.note_type, g_pub_tx.beg_note_no, \
				g_pub_tx.end_note_no );
	if( ret == -1 )
	{
		sprintf( acErrMsg, "登记柜员凭证台帐错" );
		WRITEMSG
		goto ErrExit;
	}	

	/* 登记柜员凭证明细 */
	ret=pub_com_RegNoteHst( cNoteAct, sNoteMst, tw_br_no, tw_tel );
	if ( ret == -1 )
	{
		sprintf( acErrMsg, "登记柜员凭证明细错" );
		WRITEMSG
		goto ErrExit;
	}

	/* 检查凭证最小号 */
		/* 登记最小凭证号码 */
		ret = pub_com_RegNoteMin( g_pub_tx.tx_br_no, g_pub_tx.note_type, \
				g_pub_tx.beg_note_no, g_pub_tx.tel, cInOut);
		if ( ret )
		{
			sprintf( acErrMsg, "登记凭证最小号错" );
			WRITEMSG
			goto ErrExit;	
		}

	/***
	ret = pub_com_ChkNoteMin( g_pub_tx.note_type, g_pub_tx.beg_note_no, \
				  g_pub_tx.tel );
	if ( ret == -1 )
	{
		sprintf( acErrMsg, "检查凭证最小号错beg_note_no=[%s]", g_pub_tx.beg_note_no );
		WRITEMSG
		goto ErrExit;	
	}
sprintf( acErrMsg, "ret=[%d]",ret );
WRITEMSG
**/
	strcpy( g_pub_tx.brf,"入库" );
	strcpy( g_pub_tx.add_ind,"1" );
	strcpy( g_pub_tx.ac_no,"702" );

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
 * 作者/时间：  zy/2005年1月6日
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
	 
	get_zd_data( "0890", g_pub_tx.note_type );
	get_zd_data( "0580", g_pub_tx.beg_note_no );
	get_zd_data( "0590", g_pub_tx.end_note_no );
	get_zd_long( "0520", &lCount );
	g_pub_tx.tx_amt1 = (double)lCount; 
	return	0;
	 
}
