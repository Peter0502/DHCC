/*************************************************
* 文 件 名:  spN109.c
* 功能描述： 出售给客户的凭证由于特殊原因原样返还给银行使用此交易;
*	     收回的凭证要登记号码;
*	     收回的凭证不能再进行出售;
*	     授权使用。
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
#define EXTERN
#include "public.h"
#include "note_mst_c.h"
#include "note_cheq_mst_c.h"
#include "note_mst_hst_c.h"

/*************************  全局变量定义 *******************/
char	g_sell_way[2];		/* 凭证出售方式 0零星，1整本*/

int spN109()
{
	struct	note_mst_c	sNoteMst;	/* 柜员凭证台帐 */
	struct	note_cheq_mst_c	sCheqMst;	/* 客户凭证台帐 */
	struct	note_mst_hst_c	sNoteMstHst;	/* 柜员凭证台帐明细 */
	char	cOpenBrNo[6];			/* 开户机构号 */
	char    cNoteSts[2];            	/* 凭证状态 */
	char    cPNoteSts[2];            	/* 凭证状态 */
	char	cNoteAct[2];			/* 凭证行为 */
	char	tw_br_no[6];			/* 对方机构号 */
	char	tw_tel[6];			/* 对方操作员 */
	char	who[6];				/* 使用者 */
	int	ret;
	
	memset( &sNoteMst, 0x00, sizeof( struct note_mst_c ) );
	memset( &sCheqMst, 0x00, sizeof( struct note_cheq_mst_c ) );
	memset( &sNoteMstHst, 0x00, sizeof( struct note_mst_hst_c ) );
	memset( cOpenBrNo, 0x00, sizeof( cOpenBrNo ) );
	memset( cNoteSts, 0x00, sizeof( cNoteSts ) );
	memset( cPNoteSts, 0x00, sizeof( cPNoteSts ) );
	memset( cNoteAct, 0x00, sizeof( cNoteAct ) );
	memset( tw_br_no, 0x00, sizeof( tw_br_no ) );
	memset( tw_tel, 0x00, sizeof( tw_tel ) );
	memset( who, 0x00, sizeof( who ) );
	
	strcpy( cNoteAct, "F" );		/* "F"表示为凭证出售收回操作 */

	/* 系统初始化（初始化g_pub_tx)数据初始化 */
	init_pub_tx( );	
	
	/**本方权限检查**/
	if( pub_com_NoteAuthChk( g_pub_tx.tx_br_no,g_pub_tx.tel,401,0 ) )
			goto ErrExit;

	/* 检查凭证是否存在，是否启用 */
	ret = pub_com_ChkNoteUse( g_pub_tx.note_type );
	if ( ret )
	{
		sprintf( acErrMsg, "检查凭证是否存在，是否启用错" );
		WRITEMSG
		goto ErrExit;
	}
	
	/* 凭证起始号码、终止号码检查（检查冠子头） */
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

	/* 检查是否是整本，如果是，则要判断起始号码终止号码是否构成整本 */
	if ( g_sell_way[0] == '1' )
	{
		ret = pub_com_ChkPile( g_pub_tx.note_type, g_pub_tx.beg_note_no, \
					g_pub_tx.end_note_no );
		if ( ret )
		{
			sprintf( acErrMsg, "检查凭证是否是整本错" );
			WRITEMSG
			goto ErrExit;	
		}
	}
	
/***add amelia  根据帐号查询账户序号和账户ID  **/

	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",g_pub_tx.ac_no);
	if( ret==100 )
	{
		sprintf(acErrMsg,"不存在该帐号!!ac_no=[%s]",g_pub_tx.ac_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}else if( ret )
	{
		sprintf(acErrMsg,"执行Mdm_ac_rel_Sel错误!ret=[%d]",ret);
		WRITEMSG
		goto ErrExit;
	}
        sprintf( acErrMsg, "ac_seqn=[%d]",g_mdm_ac_rel.ac_seqn );
        WRITEMSG
/***add over***/

	/* 帐户有效性检查 */
	ret= pub_base_ChkAc( g_pub_tx.ac_no, g_pub_tx.ac_seqn,cOpenBrNo , &g_pub_tx.ac_id );
	if( ret )
	{
		sprintf( acErrMsg, "帐户有效性检查错" );
		WRITEMSG
		goto ErrExit;
	}
	
	/* 根据凭证行为返回对应凭证状态 */
	strcpy( who, "C" );			/* "C"表示客户行为 */
	ret=pub_com_NoteActSts( who, cNoteAct, cNoteSts, cPNoteSts );
	if ( ret )
	{
		sprintf( acErrMsg, "根据凭证行为返回对应凭证状态错" );
		WRITEMSG
		goto ErrExit;
	}
	
	/* 客户凭证检查 */
	ret=pub_com_ChkAcCheq( g_pub_tx.ac_id, g_pub_tx.ac_seqn, \
				g_pub_tx.note_type, g_pub_tx.beg_note_no, \
				g_pub_tx.end_note_no, cPNoteSts);
	if( ret )
	{
		sprintf( acErrMsg, "客户凭证检查错" );
		WRITEMSG
		goto ErrExit;
	}
	if( cPNoteSts[0]!='1' )				/* 支票状态应为出售未使用 */
	{
		sprintf( acErrMsg, "支票状态错" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "N022" );
		goto ErrExit;
	}
	
	/* 修改客户凭证台帐（修改状态） */
	strcpy( sNoteMst.br_no, g_pub_tx.tx_br_no );
	strcpy( sNoteMst.tel, g_pub_tx.tel );
	strcpy( sNoteMst.note_type, g_pub_tx.note_type );
	strcpy( sNoteMst.beg_note_no, g_pub_tx.beg_note_no );
	strcpy( sNoteMst.end_note_no, g_pub_tx.end_note_no );
	sNoteMst.cnt = (int)g_pub_tx.tx_amt1;
	
	ret=pub_com_RegCheq( sNoteMst, g_pub_tx.ac_id, g_pub_tx.ac_seqn, \
				g_pub_tx.tx_date, cNoteAct );
	if( ret )
	{
		sprintf( acErrMsg, "修改客户凭证台帐错" );
		WRITEMSG
		goto ErrExit;
	}

	/* 登记客户凭证台帐明细 */
	sCheqMst.ac_id = g_pub_tx.ac_id;
	sCheqMst.ac_seq = g_pub_tx.ac_seqn;
	sCheqMst.cnt = (int)g_pub_tx.tx_amt1;
	strcpy( sCheqMst.br_no, cOpenBrNo );
	strcpy( sCheqMst.note_type, g_pub_tx.note_type );
	strcpy( sCheqMst.beg_note_no, g_pub_tx.beg_note_no );
	strcpy( sCheqMst.end_note_no, g_pub_tx.end_note_no );
	
	ret=pub_com_RegCheqHst( cNoteAct, sCheqMst, tw_br_no, tw_tel );
	if ( ret )
	{
		sprintf( acErrMsg, "登记客户凭证台帐明细错!" );
		WRITEMSG
		goto ErrExit;
	}
	
	/* 根据凭证行为返回对应凭证状态 */
	strcpy( who, "T" );			/* "T"表示柜员行为 */
	ret=pub_com_NoteActSts( who, cNoteAct, cNoteSts, cPNoteSts );
	if ( ret )
	{
		sprintf( acErrMsg, "根据凭证行为返回对应凭证状态错" );
		WRITEMSG
		goto ErrExit;
	}
	
	/* 登记柜员凭证台帐（凭证分段处理） */
	ret=pub_com_NoteApart( cNoteAct, sNoteMst );
	if ( ret )
	{
		sprintf( acErrMsg, "凭证分段处理错!" );
		WRITEMSG
		goto ErrExit;
	}

	/* 登记柜员凭证明细 */
	ret=pub_com_RegNoteHst( cNoteAct, sNoteMst, tw_br_no, tw_tel );
	if ( ret )
	{
		sprintf( acErrMsg, "登记柜员凭证明细错" );
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
 * 作者/时间：  andy/2004年1月10日
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
	 memset( g_sell_way, 0x00, sizeof( g_sell_way ) );
	 	 
	get_zd_data( "0310", g_pub_tx.ac_no );			/* 帐号 */
    pub_base_old_acno( g_pub_tx.ac_no );
	get_zd_int( "0340", &g_pub_tx.ac_seqn );		/* 账户序号 */
	get_zd_data( "0890", g_pub_tx.note_type );		/* 凭证种类 */
	get_zd_data( "0670", g_sell_way );			/* 收回方式 */
	get_zd_data( "0580", g_pub_tx.beg_note_no );		/* 起始号码 */
	get_zd_data( "0590", g_pub_tx.end_note_no );		/* 终止号码 */
	get_zd_long( "0520", &lCount );				/* 收回数量 */
	g_pub_tx.tx_amt1 = lCount; 
	return	0;
	 
}
