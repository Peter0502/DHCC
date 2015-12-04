/*************************************************
* 文 件 名:  spN103.c
* 功能描述： 
*	领入上级行下拨的凭证;将本行凭证上缴到上级行
*	分为：0领入、1(正常上缴、作废上缴)
*	领入的前提是下发；
*	正常上缴的前提是正常状态凭证，后继是正常收缴
*	作废上缴的前提是作废状态凭证，后继是作废收缴
*
* 作    者:  andy
* 完成日期： 2004年1月7日
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
#include "com_tel_c.h"

/******************* 定义全局变量 ********************/
char	g_cOperCode[2];			/* 操作代码 */
char	g_cTwBrno[6];			/* 上缴时对方机构 */
char	g_cTwTel[5];			/* 上缴时对方柜员 */


int spN103()
{
	struct 	note_mst_c	sNoteMst;	/* 柜员凭证台帐 */
	struct 	note_mst_c	sTwNoteMst;	/* 对方柜员凭证台帐 */
	struct	note_mst_hst_c	sNoteHst;	/* 柜员凭证明细 */
	struct  com_tel_c	sComTel;	/* 柜员文件 */
	
	char	cNoteAct[2];		/* 凭证行为 */
	char	cTwNoteAct[2];		/* 对方凭证行为 */
	char	cNoteSts[2];		/* 凭证状态，上缴时候用 */
	char	cInOut[2];
	int	iTxRelType;		/* 关系类型 */
	
	int	ret;			/* 函数返回值 */

	memset( &sNoteMst, 0x00, sizeof( struct note_mst_c ) );
	memset( &sTwNoteMst, 0x00, sizeof( struct note_mst_c ) );
	memset( &sNoteHst, 0x00, sizeof( struct note_mst_hst_c ) );
	memset( &sComTel, 0x00, sizeof( struct com_tel_c ) );
	memset( cNoteAct, 0x00, sizeof( cNoteAct ) );
	memset( cTwNoteAct, 0x00, sizeof( cTwNoteAct ) );
	memset( cNoteSts, 0x00, sizeof( cNoteSts ) );
	memset( cInOut, 0x00, sizeof( cInOut ) );

	iTxRelType = 2;			/* 2：凭证调拨关系 */
	
	/* 初始化g_pub_tx结构 */
	init_pub_tx( );
		
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
	if ( ret )
	{
		sprintf( acErrMsg, "检查凭证起始号码冠子头错" );
		WRITEMSG
		goto ErrExit;
	}
	ret=pub_com_ChkNoteHead( g_pub_tx.note_type, g_pub_tx.end_note_no, g_pub_tx.tel );
	if ( ret )	
	{
		sprintf( acErrMsg, "检查凭证终止号码冠子头错" );
		WRITEMSG
		goto ErrExit;
	}
			
	/* 检查操作码, 0 领入 1 上缴 */
	switch( g_cOperCode[0] )
	{
		case '0':		/* 领入 */
			/**本方权限检查**/
			if( pub_com_NoteAuthChk( g_pub_tx.tx_br_no,g_pub_tx.tel,103,0 ) )
					goto ErrExit;

			strcpy( cNoteAct, "3" );	/* 3表示领入 */
			strcpy( cTwNoteAct, "2" );	/* 2表示下发行为 */			
			strcpy( cInOut, "I" );

			strcpy( g_pub_tx.brf,"领入" );
			strcpy( g_pub_tx.add_ind,"1" );
			strcpy( g_pub_tx.ac_no,"702" );
			
			/* 查找柜员台帐明细 */
			/* 根据交易日期.凭证种类.起始凭证.终止凭证.对方柜员.凭证行为 */
			ret = pub_com_GetNoteHst( g_pub_tx.tx_date, \
				g_pub_tx.note_type, g_pub_tx.beg_note_no, \
				g_pub_tx.end_note_no, g_pub_tx.tx_br_no, \
				g_pub_tx.tel, cTwNoteAct, &sNoteHst );
			if ( ret ) 
			{
				sprintf( acErrMsg, "查询柜员凭证明细错" );
				WRITEMSG
				goto ErrExit;	
			}

			strcpy( g_cTwBrno, sNoteHst.br_no );
			strcpy( g_cTwTel, sNoteHst.tel );

			/* 检查柜员凭证台帐是否存在该批凭证 */
			strcpy( sTwNoteMst.tel , sNoteHst.tel );
			strcpy( sTwNoteMst.br_no , sNoteHst.br_no );
			strcpy( sTwNoteMst.note_type , sNoteHst.note_type );
			strcpy( sTwNoteMst.beg_note_no , sNoteHst.beg_note_no );
			strcpy( sTwNoteMst.end_note_no , sNoteHst.end_note_no );
			
			ret = pub_com_ChkNote( cNoteAct, sTwNoteMst );
			if( ret )
			{
				sprintf( acErrMsg, "检查柜员凭证错" );
				WRITEMSG
				goto ErrExit;
			}

			/* 登记柜员凭证台帐（并段处理） */
			ret = pub_com_RegTelNote(g_pub_tx.tel, g_pub_tx.tx_br_no,\
				      cNoteAct, g_pub_tx.note_type, \
				      g_pub_tx.beg_note_no,g_pub_tx.end_note_no );
			if ( ret )
			{
				sprintf( acErrMsg, "登记柜员凭证台帐（并段处理）错" );
				WRITEMSG
				goto ErrExit;					
			}

			/* 删除/修改下发柜员的凭证 */
			ret = pub_com_NoteDel ( cTwNoteAct, sTwNoteMst );
			if ( ret )
			{
				sprintf( acErrMsg, "删除/修改下发柜员的凭证错" );
				WRITEMSG
				goto ErrExit;					
			}
			break;

		case '1':		/* 上缴 */
			strcpy( g_pub_tx.brf,"上缴" );
			strcpy( g_pub_tx.add_ind,"0" );
			strcpy( g_pub_tx.ac_no,"702" );
			
			/* 检查对方柜员机构是否是交易机构的上级机构 */
			ret = pub_base_GetTelInfo( g_cTwTel, &sComTel );
			if ( ret )
			{
				sprintf( acErrMsg, "根据柜员号取柜员信息错" );
				WRITEMSG
				goto ErrExit;
			}

			/* add by rob at 20040908 **del xxx 20041102
			if (sComTel.csts[0] == '1' || sComTel.csts[0] == '2') 
			{ 
				sprintf(acErrMsg, "该柜员已经签退不能下发!! "); 
				WRITEMSG 
				strcpy(g_pub_tx.reply, "N096"); 
				goto ErrExit; 
			} 
			if (sComTel.csts[0] != '0') 
			{ 
				sprintf(acErrMsg, "该柜员不是签到状态不能下发!!"); 
				WRITEMSG 
				strcpy(g_pub_tx.reply, "N097"); 
				goto ErrExit; 
			}
			**/

			/**对方权限检查**/
			if( pub_com_NoteAuthChk( sComTel.br_no,g_cTwTel,104,1 ) )
					goto ErrExit;

			/**本方权限检查**/
			if( !strcmp(g_pub_tx.tx_br_no,sComTel.br_no) )
			{
			if( pub_com_NoteAuthChk( g_pub_tx.tx_br_no,g_pub_tx.tel,104,10 ) )
					goto ErrExit;
			}
			else
			{
			if( pub_com_NoteAuthChk( g_pub_tx.tx_br_no,g_pub_tx.tel,104,0 ) )
					goto ErrExit;
			}
			ret = pub_com_ChkBrRel( 2, g_pub_tx.tx_br_no, sComTel.br_no );
			if( ret == 3 )
			{
				sprintf( acErrMsg, "所输对方机构与交易机构不存在凭证调拨的上下级关系!" );
				WRITEMSG
				strcpy( g_pub_tx.reply, "N078" );
				goto ErrExit;
			}else if ( ret )
			{
				sprintf( acErrMsg, "检查机构之间的关系错" );
				WRITEMSG
				goto ErrExit;
			}
			
			strcpy( g_cTwBrno, sComTel.br_no );
			
			/* 检查凭证存在，取出状态 */
			ret = pub_com_NoteMstSts( sNoteMst.note_type, \
				sNoteMst.beg_note_no, sNoteMst.end_note_no, \
				sNoteMst.tel, cNoteSts );
			if ( ret )
			{
				sprintf( acErrMsg, "检查凭证存在否，取出状态错" );
				WRITEMSG
				/**return(-1);**/	
			}
			
			/* 判断做正常上缴还是作废上缴 */
			if ( cNoteSts[0] == '0' )	/* 正常上缴 */
			{
				strcpy( cNoteAct, "7" );/* 7表示正常上缴 */
				strcpy( cInOut, "O" );
			}else if ( cNoteSts[0] == '7' )	/* 作废上缴 */
			{
				strcpy( cNoteAct, "8" );/* 8表示作废上缴 */
			}else
			{
				sprintf( acErrMsg, "凭证非正常或作废状态，不能上缴!" );
				WRITEMSG
				strcpy( g_pub_tx.reply, "N050" );
				/**return(-1);**/
				goto ErrExit;
			}
			
			/* 检查操作员是否拥有这批凭证 */
			ret = pub_com_ChkNote( cNoteAct, sNoteMst );
			if( ret )
			{
				sprintf( acErrMsg, "检查柜员凭证错" );
				WRITEMSG
				goto ErrExit;
			}
			
			/* 登记柜员凭证台帐（分段处理） */
			ret = pub_com_NoteApart( cNoteAct, sNoteMst );
			if ( ret )
			{
				sprintf( acErrMsg, "登记柜员凭证台帐（分段处理）错" );
				WRITEMSG
				goto ErrExit;				
			}
			break;
			
		default:
			sprintf( acErrMsg, "凭证操作码不存在,操作码=[%s]", g_cOperCode );
			WRITEMSG
			goto ErrExit;
	}	/* end of switch */
	
	/* 登记柜员凭证明细 */
	ret=pub_com_RegNoteHst( cNoteAct, sNoteMst, g_cTwBrno, g_cTwTel );
	if ( ret == -1 )
	{
		sprintf( acErrMsg, "登记柜员凭证明细错" );
		WRITEMSG
		goto ErrExit;
	}
		
	/* 判断该批凭证状态是否为正常0 */
	/* 登记凭证最小号 	
	if ( strlen( cInOut ) )
	{
		* 检查凭证最小号 *
		ret = pub_com_ChkNoteMin( g_pub_tx.note_type, \
					g_pub_tx.beg_note_no, g_pub_tx.tel );
		if ( ret == -1 )
		{
			sprintf( acErrMsg, "检查凭证最小号错" );
			WRITEMSG
			goto ErrExit;	
		}
		if ( !ret )
		{	*/
			/* 登记凭证最小号 */
			ret = pub_com_RegNoteMin( g_pub_tx.tx_br_no, \
					g_pub_tx.note_type, g_pub_tx.beg_note_no, \
					g_pub_tx.tel, cInOut );
			if ( ret )
			{
				sprintf( acErrMsg, "登记凭证最小号错" );
				WRITEMSG
				goto ErrExit;	
			}
		/*}*/
	/*}*/

	/* 写交易日志 */
	ret = pub_ins_trace_log();
	if ( ret )
	{
		sprintf( acErrMsg, "记流水日志错" );
		WRITEMSG
		goto ErrExit;
	}
		
OkExit:
        strcpy( g_pub_tx.reply, "0000" );
		set_zd_data(DC_GET_MSG,"");
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
	char dc_ind[2];
	
	memset( g_cOperCode, 0x00, sizeof( g_cOperCode ) );
	memset( g_cTwTel, 0x00, sizeof( g_cTwTel ) );
	memset( dc_ind,0,sizeof(dc_ind) );
	 
	pub_base_sysinit();

	get_zd_data( "0670", g_cOperCode );		/* 操作代码 */

	if( g_cOperCode[0]=='0' )
		set_zd_data( "0680", "2" );
	else
		set_zd_data( "0680", "1" );

	get_zd_data( "0890", g_pub_tx.note_type );	/* 凭证类型 */
	get_zd_data( "0580", g_pub_tx.beg_note_no );
	get_zd_data( "0590", g_pub_tx.end_note_no );
	get_zd_long( "0520", &lNoteCnt );		/* 凭证数量 */
	g_pub_tx.tx_amt1 = lNoteCnt;
	get_zd_data( "0920", g_cTwTel );		/* 上缴对方柜员 */

	return	0;
}
