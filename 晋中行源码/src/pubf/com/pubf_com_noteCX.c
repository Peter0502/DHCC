#include "find_debug.h" 
/*************************************************
* 文件名:    pubf_com_noteUndo.c
* 功能描述： 凭证撤销处理函数族（凭证管理交易用的撤销函数）
*		1. pub_com_NoteCX    凭证使用撤销函数
*		2. pub_com_NoteNoCX  客户凭证撤销处理
*		3. pub_com_CheqNoCX  柜员凭证撤销处理
*
* 作   者:  jack
* 完成日期： 2005年1月22日
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
#include "note_mst_hst_c.h"
#include "note_cheq_mst_c.h"
#include "note_cheq_hst_c.h"
#include "note_parm_c.h"

/**********************************************************************
 * 函  数 名：  pub_com_NoteCX
 * 函数 功能:   凭证撤销处理（撤销使用过的凭证）
 *	       包括撤销客户凭证和柜员凭证两部分，根据凭证类型来判断，如果可以出售
 *		给客户，则销客户凭证，如果不可出售给客户，销柜员凭证。
 * 作者/时间：  jack/2005年1月22日
 * 
 * 参  数：
 *     输入：   
 *        long     trace_no        流水号
 *        long     trace_seq       流水序号
 *        long     tx_date         交易日期
 *        char     *hst_flag       是否删除明细记录标志 Y删除 N不删除
 *
 *     输出：  char g_pub_tx.reply[]     响应码
 *
 *   返回值：  0: 成功    -1: 不成功
 *           
 * 修改历史：  包括修改人、时间、修改内容，以时间倒序方式
 *
********************************************************************/
int pub_com_NoteCX(
	long	trace_no,
	long	trace_cnt,
	long	tx_date,
	char	*hst_flag)
{
	int	num = 0;
	int	ret;
	
	/* 检查柜员凭证明细表 */
	ret = sql_count( "note_mst_hst", "trace_no = %ld and trace_cnt = %d \
			and tx_date = %ld", trace_no, trace_cnt, tx_date );
	if ( ret < 0 )
	{
		sprintf( acErrMsg, "查找柜员凭证明细错[%d]", ret );
		WRITEMSG
		strcpy( g_pub_tx.reply, "N057" );
		return(-1);
	}else if ( ret > 1 )
	{
		sprintf( acErrMsg, "存在多条符合条件的柜员凭证明细记录" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "N042" );
		return(-1);
	}else if ( ret == 1 )
	{
		ret = pub_com_NoteNoCX( trace_no, trace_cnt, tx_date, hst_flag );
		if ( ret )
		{
	   		sprintf( acErrMsg, "销柜员凭证错" );
	   		WRITEMSG
	   		return(-1);		
		}
		num++;
	}
	
	/* 检查客户凭证明细表 */
	ret = sql_count( "note_cheq_hst", "trace_no = %ld and trace_cnt = %d \
			and tx_date = %ld", trace_no, trace_cnt, tx_date );
	if ( ret < 0 )
	{
		sprintf( acErrMsg, "查找客户凭证明细错[%d]", ret );
		WRITEMSG
		strcpy( g_pub_tx.reply, "N058" );
		return(-1);
	}else if ( ret > 1 )
	{
		sprintf( acErrMsg, "存在多条符合条件的客户凭证明细记录" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "N060" );
		return(-1);
	}else if ( ret == 1 )
	{
		ret = pub_com_CheqNoCX( trace_no, trace_cnt, tx_date, hst_flag );
		if ( ret )
		{
	   		sprintf( acErrMsg, "销柜员凭证错" );
	   		WRITEMSG
	   		return(-1);				
		}   
		num++;
	}
	
	if ( !num )
	{
		sprintf( acErrMsg, "该流水号不存在" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "S045" );
		return(-1);
	}
   	return(0);
}


/**********************************************************************
 * 函  数 名：  pub_com_NoteNoCX
 * 函数 功能:   柜员凭证撤销函数
 * 作者/时间：  jack/2005年1月4日
 * 
 * 参  数：
 *     输入：      
 *        long     trace_no        流水号
 *        long     trace_seq       流水序号
 *        long     tx_date         交易日期
 *        char     *hst_flag       是否删除note_hst记录标志 Y删除 N不删除
 *
 *     输出：  char g_pub_tx.reply[]     响应码
 *
 *   返回值：  0: 成功    -1: 不成功
 *           
 * 修改历史：  包括修改人、时间、修改内容，以时间倒序方式
 *
********************************************************************/
int pub_com_NoteNoCX( trace_no, trace_cnt, tx_date, hst_flag )
       long     trace_no;
       long     trace_cnt;
       long     tx_date;
       char     *hst_flag;
{
	struct	note_mst_c	sNoteMst;	/* 柜员凭证台帐 */
	struct 	note_mst_hst_c 	sNoteHst;	/* 柜员凭证明细 */
	struct 	note_mst_hst_c 	sNoteHstTmp;	/* 柜员凭证明细插入用临时变量 */
	char	cNoteSts[2], cPNoteSts[2];	/* 凭证当前状态和之前的状态 */
	char	cNoteAct[2];			/* 凭证行为 */
	
	int	ret;
	
	MEMSET_DEBUG( &sNoteMst, 0x00, sizeof( struct note_mst_c ) );
	MEMSET_DEBUG( &sNoteHst, 0x00, sizeof( struct note_mst_hst_c ) );
	MEMSET_DEBUG( &sNoteHstTmp, 0x00, sizeof( struct note_mst_hst_c ) );
	
	MEMSET_DEBUG( cNoteSts, 0x00, sizeof( cNoteSts ) );
	MEMSET_DEBUG( cPNoteSts, 0x00, sizeof( cPNoteSts ) );
	MEMSET_DEBUG( cNoteAct, 0x00, sizeof( cNoteAct ) );
	
	/* 检查凭证明细是否存在 */
	ret = sql_count( "note_mst_hst", "tx_date = %ld and trace_no = %ld \
			and trace_cnt = %ld", tx_date, trace_no, trace_cnt );
	if ( ret < 0 )
	{
		sprintf( acErrMsg, "查找柜员凭证明细错[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply, "N057" );
		return(-1);
	}else if ( ret = 0 )
	{
		sprintf( acErrMsg, "没找到符合条件的记录");
		WRITEMSG
		strcpy( g_pub_tx.reply, "N041" );
		return(-1);
	}else if ( ret > 1 )
	{
		sprintf( acErrMsg, "找到多条符合条件的记录" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "N042" );
		return(-1);
	}
	
	/* 根据流水号取柜员凭证明细 sNoteHst */
	ret = Note_mst_hst_Sel( g_pub_tx.reply , &sNoteHst , "tx_date = %ld \
				and trace_no = %ld and trace_cnt = %d", \
				tx_date, trace_no, trace_cnt );
	if ( ret == 100 )
	{
		sprintf( acErrMsg, "柜员台帐明细不存在符合条件的记录，ret = [%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply, "N041" );
		return(-1);
	}else if ( ret )
	{
		sprintf( acErrMsg, "执行Note_mst_hst_Fet_Sel错，ret = [%d]",ret);
		WRITEMSG
		return(-1);			
	}
	
	/* 判断是否是本柜员做的业务 */
	pub_base_strpack( g_pub_tx.tel );
	pub_base_strpack( sNoteHst.tel );
	pub_base_strpack( g_pub_tx.tx_br_no );
	pub_base_strpack( sNoteHst.br_no );
	
sprintf( acErrMsg, "g_pub_tx.tel=[%s],sNoteHst.tel=[%s],g_pub_tx.tx_br_no=[%s],sNoteHst.br_no=[%s]",\
	 g_pub_tx.tel, sNoteHst.tel, g_pub_tx.tx_br_no, sNoteHst.br_no );
WRITEMSG

	if ( strcmp(g_pub_tx.tel, sNoteHst.tel) || strcmp(g_pub_tx.tx_br_no, sNoteHst.br_no) )
	{
		sprintf( acErrMsg, "非本柜员业务，不能撤销" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "S046" );
		return(-1);
	}
	
	/* 将明细赋值给 sNoteMst */
	strcpy( sNoteMst.tel, sNoteHst.tel );
	strcpy( sNoteMst.note_type, sNoteHst.note_type );
	strcpy( sNoteMst.br_no, sNoteHst.br_no );
	strcpy( sNoteMst.beg_note_no, sNoteHst.beg_note_no );
	strcpy( sNoteMst.end_note_no, sNoteHst.end_note_no );	
	sNoteMst.cnt = sNoteHst.note_cnt;
	strcpy( cNoteAct, sNoteHst.sts_ind );	
	
	/* 修改note_mst表 */
	/* 分段处理 */
	ret = pub_com_NoteApartCX( cNoteAct, sNoteMst);
	if ( ret == -1 )
	{
		sprintf( acErrMsg, "凭证分段错");
		WRITEMSG
		strcpy( g_pub_tx.reply, "N023" );
		return(-1);				
	}
	/* 修改凭证明细表 */
	if ( hst_flag[0] == 'Y' )	/* 删除原来的明细 */
	{
		/* 删除入库之前的数据 */
		ret = sql_execute("delete from note_mst_hst where tx_date = %ld \
				and trace_no = %ld and trace_cnt = %ld", \
				sNoteHst.tx_date, sNoteHst.trace_no, \
				sNoteHst.trace_cnt );
		if ( ret )
		{
			sprintf( acErrMsg, "删除柜员凭证明细note_hst错!" );
			WRITEMSG
			strcpy(g_pub_tx.reply,"N043");
			return(-1);
		}		
	}else				/* 不删除明细，需要增加明细 */
	{
		sNoteHstTmp = sNoteHst;
		sNoteHstTmp.tx_date = g_pub_tx.tx_date;
		sNoteHstTmp.tx_time = g_pub_tx.tx_time;
		sNoteHstTmp.trace_no = g_pub_tx.trace_no;
		sNoteHstTmp.trace_cnt = g_pub_tx.trace_cnt;
		sNoteHstTmp.trace_cnt = g_pub_tx.trace_cnt;
		strcpy( sNoteHstTmp.sts_ind, "N" );
				
		ret = Note_mst_hst_Ins( sNoteHstTmp, g_pub_tx.reply );
		if ( ret == -239 )
		{
			sprintf( acErrMsg, "插入柜员凭证明细错，有重复记录!" );
			WRITEMSG
			strcpy(g_pub_tx.reply,"N015");
			return(-1);			
		}else if ( ret )
		{
			sprintf( acErrMsg, "插入柜员凭证明细错!ret=[%d]", ret );
			WRITEMSG
			return(-1);			
		}
	}
	
    return 0;
}

/**********************************************************************
 * 函  数 名：  pub_com_CheqNoCX
 * 函数 功能:   客户凭证撤销
 *             
 * 作者/时间：  jack/2005年1月4日
 * 
 * 参  数：
 *     输入：  
 *            	long	trace_no	流水号
 *		long	trace_cnt	流水笔次
 *            	long   	tx_date  	交易日期
 *            	char   	*hst_flag    	标志位（"Y"删除明细，"N"不删除明细）
 *
 *     输出：  
 *            	char    *g_pub_tx.reply	相应码
 *
 *   返回值：  0: 成功   -1: 失败
 *           
 * 修改历史：  包括修改人、时间、修改内容，以时间倒序方式
 *
********************************************************************/

int pub_com_CheqNoCX( trace_no, trace_cnt, tx_date, hst_flag )
	long	trace_no;
	long	trace_cnt;
	long	tx_date;
	char	*hst_flag;
{
	struct	note_cheq_hst_c	note_cheq_hst;
	struct	note_cheq_mst_c	note_cheq_mst;
	struct  note_mst_c sNoteMst;
	char	cNoteAct[2];			/* 凭证行为 */		
	char	note_sts[2];			/* 凭证状态 */
	char	p_note_sts[2];			/* 凭证使用前状态 */
	char    tw_br_no[6];            	/* 对方机构 */
    	char    tw_tel[6];            		/* 对方操作员 */
    	char 	who[2];		       		/* 使用者 */
    	long	ac_id;				/* 帐户ID */
	int	ret;

	MEMSET_DEBUG( &note_cheq_hst, 0x00, sizeof(struct note_cheq_hst_c) );
	MEMSET_DEBUG( &note_cheq_mst, 0x00, sizeof(struct note_cheq_mst_c) );
	MEMSET_DEBUG( &sNoteMst, 0x00, sizeof(struct note_mst_c) );
	
	MEMSET_DEBUG( cNoteAct, 0x00, sizeof(cNoteAct) );
	MEMSET_DEBUG( note_sts, 0x00, sizeof(note_sts) );
	MEMSET_DEBUG( p_note_sts, 0x00, sizeof(p_note_sts) );
	MEMSET_DEBUG( tw_br_no, 0x00, sizeof( tw_br_no ) );
    	MEMSET_DEBUG( tw_tel, 0x00, sizeof( tw_tel ) );
    	MEMSET_DEBUG( who, 0x00, sizeof( who ) );

	strcpy( who, "C" );			/* 表示客户凭证行为 */
		
	/* 检查凭证明细是否存在 */
	/*
	ret = sql_count( "note_cheq_hst", "tx_date = %ld and trace_no = %ld \
			and trace_cnt = %ld", tx_date, trace_no, trace_cnt );
	*/
	/*vtcp_log("tx_date = %ld and trace_no = %ld ", tx_date, trace_no );*/
	ret = sql_count( "note_cheq_hst", "tx_date = %ld and trace_no = %ld ", 
		tx_date, trace_no );
	if ( ret < 0 )
	{
		sprintf( acErrMsg, "查找客户凭证明细错[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply, "N058" );
		return(-1);
	}else if ( ret = 0 )
	{
		sprintf( acErrMsg, "没找到符合条件的记录");
		WRITEMSG
		strcpy( g_pub_tx.reply, "N059" );
		return(-1);
	}else if ( ret > 1 )
	{
		sprintf( acErrMsg, "找到多条符合条件的记录" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "N060" );
		return(-1);
	}
		
	/* 根据流水号、序号、交易日期取note_cheq_hst */
	ret=Note_cheq_hst_Sel( g_pub_tx.reply , &note_cheq_hst , 
		"trace_no=%ld and tx_date=%ld" , trace_no, tx_date );
	if ( ret == 100 )
    	{
        	sprintf( acErrMsg, "没有附和条件记录!" );
        	WRITEMSG
        	strcpy( g_pub_tx.reply, "N059" );
        	return(-1);
    	}else if ( ret )
    	{
        	sprintf( acErrMsg, "执行Note_cheq_hst_Sel错![%d]", ret );
        	WRITEMSG
        	return(-1);
    	}
	
	/* 判断是否是本柜员做的业务 */
	pub_base_strpack( g_pub_tx.tel );
	pub_base_strpack( note_cheq_hst.tel );
	pub_base_strpack( g_pub_tx.tx_br_no );
	pub_base_strpack( note_cheq_hst.tx_br_no );
	
	if ( strcmp(g_pub_tx.tel, note_cheq_hst.tel) \
	   || strcmp(g_pub_tx.tx_br_no, note_cheq_hst.tx_br_no) )
	{
		sprintf( acErrMsg, "非本柜员业务，不能撤销" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "S046" );
		return(-1);
	}
	
	strcpy( cNoteAct, note_cheq_hst.use_ind );		/* 表示行为 */	
	
	/* 根据客户凭证行为取凭证前后状态 *
	ret=pub_com_NoteActSts( who, cNoteAct, note_sts, p_note_sts );
	*/
	ret=pub_com_NoteActSts( who, "N", note_sts, p_note_sts );
	if ( ret )
	{
		sprintf( acErrMsg, "根据客户凭证行为取凭证状态不成功!" );
		WRITEMSG
		return(-1);
	}
vtcp_log("YYYY [%s][%s][%s]",who,note_sts,p_note_sts );
		
	/* 检查客户凭证台账note_cheq_mst */
	ret=pub_com_ChkAcCheq( note_cheq_hst.ac_id, note_cheq_hst.ac_seq, \
			note_cheq_hst.note_type, note_cheq_hst.beg_note_no, \
			note_cheq_hst.end_note_no, p_note_sts );
	if ( ret )
	{
		sprintf( acErrMsg, "客户凭证台帐中不存在此凭证" );
		WRITEMSG
		return(-1);	
	}
vtcp_log("YYYY [%s][%s][%s]",who,note_sts,p_note_sts );
		
	strcpy( sNoteMst.tel, note_cheq_hst.tel );
	strcpy( sNoteMst.note_type, note_cheq_hst.note_type );
	strcpy( sNoteMst.beg_note_no, note_cheq_hst.beg_note_no );
	strcpy( sNoteMst.end_note_no, note_cheq_hst.end_note_no );
	sNoteMst.cnt = note_cheq_hst.cnt;
	
/*vtcp_log("YYYY [%s][%s][%s]",cNoteAct );*/
	/* 登记客户凭证台帐 */
	/*
	ret = pub_com_RegCheqCX( sNoteMst, note_cheq_hst.ac_id, \
			note_cheq_hst.ac_seq, g_pub_tx.tx_date, cNoteAct );
	*/
	ret = pub_com_RegCheqCX( sNoteMst, note_cheq_hst.ac_id, \
			note_cheq_hst.ac_seq, g_pub_tx.tx_date, "N" );
	if ( ret )
	{
		sprintf( acErrMsg, "登记客户凭证台帐错" );
		WRITEMSG
		return(-1);	
	}
        	
	/* 根据标志位，判断是否删除明细 */
	if( hst_flag[0] == 'Y' )
	{
		/* 删除入库之前的数据 */
		ret = sql_execute( "delete from note_cheq_hst where trace_no=%ld \
				and trace_cnt=%ld and tx_date=%ld" , trace_no, \
				trace_cnt, tx_date );
		if ( ret )
		{
			sprintf( acErrMsg, "删除客户凭证台帐明细note_cheq_hst错!" );
			WRITEMSG
			strcpy(g_pub_tx.reply,"N044");
			return(-1);
		}
	}
	
	/* 如果不删除，则需要增加一条明细 */
	else
	{
		note_cheq_hst.tx_date = g_pub_tx.tx_date;
		note_cheq_hst.tx_time = g_pub_tx.tx_time;
		note_cheq_hst.trace_no = g_pub_tx.trace_no;
		note_cheq_hst.trace_cnt = g_pub_tx.trace_cnt;
		strcpy( note_cheq_hst.tel, g_pub_tx.tel );
		strcpy( note_cheq_hst.use_ind, "N" );
		
		ret=Note_cheq_hst_Ins( note_cheq_hst , g_pub_tx.reply );
		if ( ret == -239 )
    		{
        		sprintf(acErrMsg,"向客户凭证明细表插入记录错!有重复记录!");
        		WRITEMSG
        		strcpy(g_pub_tx.reply,"N024");
        		return(-1);
    		}else if( ret )
    		{
        		sprintf( acErrMsg,"执行Note_cheq_hst_Ins错!ret=[%d]",ret );
        		WRITEMSG
        		return(-1);
    		}
	}
	
	return 0; 
}
