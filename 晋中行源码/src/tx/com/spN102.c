/*************************************************
* 文 件 名:  spN102.c
* 功能描述： 
*	凭证由总行下发给管理支行,管理支行下发给网点;
*	凭证由总行收缴各支行上缴的凭证,或者各级支行
*	收缴各网点上缴的凭证;
*	此交易包括两个交易：凭证下发/凭证收缴;
*	下发的前提是入库,收缴的前提是上缴,
*	其中上缴分为正常凭证收缴和作废凭证收缴。
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
char	g_cTwBrno[6];			/* 下发机构 */
char	g_cTwTel[5];			/* 下发柜员 */

int spN102()
{
	struct 	note_mst_c	sNoteMst, sTwNoteMst;
	struct 	note_mst_hst_c	sNoteHst;
	struct	com_tel_c	sComTel;		/* 柜员信息表结构 */
	char	cNoteAct[2];		/* 凭证行为 */
	char	cTwNoteAct[2];		/* 对方柜员凭证行为 */
	char	cInOut[2];		/* 登记凭证最小号用：I转入 O转出 */
	int	iRelType;		/* 关系类型 */
	int	ret;			/* 函数返回值 */

	memset( &sNoteMst, 0x00, sizeof( struct note_mst_c ) );
	memset( &sTwNoteMst, 0x00, sizeof( struct note_mst_c ) );
	memset( &sNoteHst, 0x00, sizeof( struct note_mst_hst_c ) );
	memset( &sComTel, 0x00, sizeof( struct com_tel_c ) );
	memset( cNoteAct, 0x00, sizeof( cNoteAct ) );
	memset( cTwNoteAct, 0x00, sizeof( cTwNoteAct ) );
	memset( cInOut, 0x00, sizeof( cInOut ) );

	iRelType = 2;			/* 2：凭证调拨关系 */
	
	/* 初始化g_pub_tx结构 */
	init_pub_tx( );
	
	strcpy( sNoteMst.tel , g_pub_tx.tel );
	strcpy( sNoteMst.br_no, g_pub_tx.tx_br_no );
	strcpy( sNoteMst.note_type, g_pub_tx.note_type );
	strcpy( sNoteMst.beg_note_no, g_pub_tx.beg_note_no );
	strcpy( sNoteMst.end_note_no, g_pub_tx.end_note_no );
	sNoteMst.cnt = (long)g_pub_tx.tx_amt1;

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
	
	/* 凭证有效性检查:是否存在,是否处于使用状态 */
	ret = pub_com_ChkNoteUse( g_pub_tx.note_type );
	if ( ret )
	{
		sprintf( acErrMsg, "检查凭证是否存在，是否启用错" );
		WRITEMSG
		goto ErrExit;		
	}
	
	/* 检查操作码, 0 下发 1 收缴 */
	switch( g_cOperCode[0] )
	{
		case '0':			/* 下发 */
			/**本方权限检查**/
			if( pub_com_NoteAuthChk( g_pub_tx.tx_br_no,g_pub_tx.tel,102,0 ) )
			{
				vtcp_log("[%s][%d]本方权限错\n",__FILE__,__LINE__);
					goto ErrExit;
			}

			strcpy( cNoteAct, "2" );	/* 2表示下发 */
			strcpy( cInOut, "O" );		/* 表示转出 */

			strcpy( g_pub_tx.brf,"下发" );
			strcpy( g_pub_tx.add_ind,"0" );
			strcpy( g_pub_tx.ac_no,"702" );


			/* 检查下发机构是否为交易机构的下级 */
			sprintf(acErrMsg, "CGH [%s], up [%s]111", 
				g_cTwBrno, g_pub_tx.tx_br_no);
			WRITEMSG
			ret = pub_com_ChkBrRel( 2, g_cTwBrno, g_pub_tx.tx_br_no );
			if( ret == 3 )
			{
				sprintf( acErrMsg, "交易机构与下发机构不存在凭证调拨的上下级关系!" );
				WRITEMSG
				strcpy( g_pub_tx.reply, "N077" );
				goto ErrExit;
			}else if ( ret )
			{
				sprintf( acErrMsg, "检查机构之间的关系错" );
				WRITEMSG
				goto ErrExit;
			}

			/**对方权限检查**/
			if( !strcmp(g_cTwBrno,g_pub_tx.tx_br_no) )
			{
				if( pub_com_NoteAuthChk( g_cTwBrno,g_cTwTel,102,10 ) ){
					vtcp_log("[%s][%d]权限错\n",__FILE__,__LINE__);
					goto ErrExit;
				}
			}
			else
			{
				if( pub_com_NoteAuthChk( g_cTwBrno,g_cTwTel,102,1 ) ){
					vtcp_log("[%s][%d]权限错\n",__FILE__,__LINE__);
						goto ErrExit;
				}
			}
			/* 检查下发柜员是否为下发机构所属柜员 */
			ret=Com_tel_Sel( g_pub_tx.reply , &sComTel ,\
			 		"tel='%s'" , g_cTwTel );	
			if( ret==100 )
			{
				sprintf( acErrMsg, "取柜员信息表错!\
					tel=[%s]", g_pub_tx.tel );
				WRITEMSG
				strcpy( g_pub_tx.reply, "S009");
				goto	ErrExit;
			}
			else if( ret )
			{
				sprintf( acErrMsg, "执行Com_tel_Sel错误!ret=[%d]", ret );
				WRITEMSG
				goto	ErrExit;
			}

			/* add by rob at 20040908 **
			if (sComTel.csts[0] == '1' || sComTel.csts[0] == '2')
			{
				sprintf(acErrMsg, "该柜员已经签退不能下发!! ");
				WRITEMSG
				strcpy(g_pub_tx.reply, "N096");
				goto ErrExit;
			}
			*************20041102****************/
			if (sComTel.csts[0] != '0')
			{
				sprintf(acErrMsg, "该柜员不是签到状态不能下发!!");
				WRITEMSG
				strcpy(g_pub_tx.reply, "N097");
				goto ErrExit;
			}
			
			sprintf(acErrMsg, "注意tel:[%s], [%s]", sComTel.br_no, g_cTwBrno);
			WRITEMSG
			pub_base_strpack(sComTel.br_no);
			if( strcmp( sComTel.br_no, g_cTwBrno ) != 0 )
			{
				sprintf( acErrMsg, "下发机构无此柜员!");
				WRITEMSG
				strcpy( g_pub_tx.reply, "N076");
				goto	ErrExit;
			}

			/* 检查操作员是否拥有这批凭证 */
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
			break;

		case '1':			/* 收缴 */
			/**本方权限检查**/
			if( pub_com_NoteAuthChk( g_pub_tx.tx_br_no,g_pub_tx.tel,102,0 ) )
					goto ErrExit;

			strcpy( g_pub_tx.brf,"收缴" );
			strcpy( g_pub_tx.add_ind,"1" );
			strcpy( g_pub_tx.ac_no,"702" );

			/* 取柜员凭证明细 , 判断是否存在 */
			/* 根据交易日期.凭证种类.起始凭证.终止凭证.对方柜员.凭证行为 */
			strcpy( cTwNoteAct, "7" );
			ret = pub_com_GetNoteHst( g_pub_tx.tx_date, \
				g_pub_tx.note_type, g_pub_tx.beg_note_no, \
				g_pub_tx.end_note_no, g_pub_tx.tx_br_no, \
				g_pub_tx.tel, cTwNoteAct, &sNoteHst );
			if ( ret == 100 )
			{
				strcpy( cTwNoteAct, "8" );
				ret = pub_com_GetNoteHst( g_pub_tx.tx_date, \
					g_pub_tx.note_type, g_pub_tx.beg_note_no, \
					g_pub_tx.end_note_no, g_pub_tx.tx_br_no, \
					g_pub_tx.tel, cTwNoteAct, &sNoteHst );				
			}	
			if ( ret ) 
			{
				sprintf( acErrMsg, "查询柜员凭证明细错" );
				WRITEMSG
				goto ErrExit;	
			}
			
			strcpy( g_cTwBrno, sNoteHst.br_no );
			strcpy( g_cTwTel, sNoteHst.tel );
									
			/* 检查柜员凭证台帐是否存在对方记录 */
			if ( sNoteHst.sts_ind[0] == '7' )
			{
				strcpy( cNoteAct, "9" );	/* 9表示正常收缴 */
				strcpy( cInOut, "I" );		/*　表示转入 */
			}else if ( sNoteHst.sts_ind[0] == '8' )
			{
				strcpy( cNoteAct, "A" );	/* A表示作废收缴 */
			}else
			{
				sprintf( acErrMsg, "凭证行为错" );
				WRITEMSG
				strcpy( g_pub_tx.reply, "N041" );
				goto ErrExit;
			}
			
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
				
			/* 凭证并段（登记柜员凭证台帐) */	
			ret = pub_com_RegTelNote(g_pub_tx.tel, g_pub_tx.tx_br_no,\
				      cNoteAct, g_pub_tx.note_type, \
				      g_pub_tx.beg_note_no,g_pub_tx.end_note_no );
			if ( ret )
			{
				sprintf( acErrMsg, "登记柜员凭证台帐（并段处理）错" );
				WRITEMSG
				goto ErrExit;					
			}
			/* 修改/删除对方柜员凭证台帐 */
			ret = pub_com_NoteDel ( cTwNoteAct, sTwNoteMst );
			if ( ret )
			{
				sprintf( acErrMsg, "删除/修改下发柜员的凭证错" );
				WRITEMSG
				goto ErrExit;					
			}
			break;

		default:
			sprintf( acErrMsg, "凭证操作码不存在" );
			WRITEMSG
			goto ErrExit;
	}

	/* 登记柜员凭证明细 */
	ret=pub_com_RegNoteHst( cNoteAct, sNoteMst, g_cTwBrno, g_cTwTel );
	if ( ret == -1 )
	{
		sprintf( acErrMsg, "登记柜员凭证明细错" );
		WRITEMSG
		goto ErrExit;;
	}
	
	/* 登记凭证最小号 */	
	if ( strlen( cInOut ) )
	{
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
		vtcp_log("[%s][%d]!!!!!进入错误处理[%s]\n",__FILE__,__LINE__,g_pub_tx.reply);
		if(memcmp(g_pub_tx.reply,"0000",4)==0)
        	strcpy( g_pub_tx.reply, "0001" );
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
	memset( g_cTwBrno, 0x00, sizeof( g_cTwBrno ) );
	memset( g_cTwTel, 0x00, sizeof( g_cTwTel ) );
	memset( dc_ind,0,sizeof(dc_ind) );
	 
	pub_base_sysinit();

	get_zd_data( "0670", g_cOperCode );		/* 操作代码 */

	dc_ind[0]=g_cOperCode[0]+1;
	set_zd_data( "0680", dc_ind );		/* 操作代码 */

	get_zd_data( "0890", g_pub_tx.note_type );	/* 凭证类型 */
	get_zd_data( "0580", g_pub_tx.beg_note_no );
	get_zd_data( "0590", g_pub_tx.end_note_no );
	get_zd_long( "0520", &lNoteCnt );	/* 凭证数量 */
	g_pub_tx.tx_amt1 = lNoteCnt;
	get_zd_data( "0910", g_cTwBrno );		/* 下发机构 */
	get_zd_data( "0920", g_cTwTel );		/* 下发柜员 */

	return	0;
}
