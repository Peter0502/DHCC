#include "find_debug.h" 
/*************************************************
* 文件名:    pubf_com_noteUse.c
* 功能描述： 凭证使用处理函数族
*		1. pub_com_NoteUse    凭证使用函数
*		2. pub_com_CheqClsNo  客户凭证销号处理
*		3. pub_com_NoteClsNo  柜员凭证销号处理
*
* 作   者:  jack
* 完成日期： 2003年12月16日
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
#include "note_min_no_c.h"
#include "note_cheq_mst_c.h"
#include "note_cheq_hst_c.h"
#include "note_parm_c.h"

/**********************************************************************
 * 函  数 名：  pub_com_NoteUse
 * 函数 功能:   凭证销号处理（交易过程中使用凭证）
 *	      包括销客户凭证和柜员凭证两部分，根据凭证类型来判断，如果可以出售
 *		给客户，则销客户凭证，如果不可出售给客户，销柜员凭证。如果不管
 *		理号码，则直接返回0，不进行凭证处理
 * 作者/时间：  Terry.Cui/2003年1月12日
 * 
 * 参  数：
 *     输入：      
 *        char     note_type[4]    凭证类型
 *        char     beg_no[17]      起始号码
 *        char     end_no[17]      终止号码
 *        char     tel[5]          柜员号
 *
 *     输出：  char g_pub_tx.reply[]     响应码
 *
 *   返回值：  0: 成功    -1: 不成功
 *           
 * 修改历史：  包括修改人、时间、修改内容，以时间倒序方式
 *
********************************************************************/
int pub_com_NoteUse(
	long	ac_id,
	int	ac_seqn,
    	char    *note_type,
    	char    *beg_no,
    	char    *end_no,
    	char    *tel)
{
	struct note_parm_c sNoteParm;
	
	int	ret;
	
	MEMSET_DEBUG( &sNoteParm, 0x00, sizeof( struct note_parm_c ) );
	
	/* 取凭证参数表:根据凭证类型取凭证结构 */
	ret = Note_parm_Sel(g_pub_tx.reply, &sNoteParm, "note_type = '%s'", note_type );
	if( ret == 100 )
	{
	    sprintf(acErrMsg,"取凭证参数note_parm表错!note_type=[%s]",note_type);
	    WRITEMSG
	    strcpy( g_pub_tx.reply, "N001" );
	    return (-1);
	}else if ( ret )
	{
	    sprintf( acErrMsg, "执行Note_parm_Sel错误![%d]", ret );
	    WRITEMSG
	    return(-1);
	}	
	
	/* 判断是否管理，如果不管理直接返回正确 */
	if ( sNoteParm.ctl_no_ind[0] == 'N' )
	{
		return(0);	/* 不管理号码，直接返回正确0 */
	}

	
	sprintf( acErrMsg, "111111111111111sNoteParm.sell_ind=[%s]",sNoteParm.sell_ind);
	WRITEMSG
	sprintf( acErrMsg, "111111111111111svc_ind=[%d]",g_pub_tx.svc_ind);
	WRITEMSG

	/* 判断是否允许卖给客户 */
	if ( sNoteParm.sell_ind[0] == 'N' )	/* 不允许出售给客户 */
	{
		
		/**本方权限检查**/
		if( pub_com_NoteAuthChk( g_pub_tx.tx_br_no,g_pub_tx.tel,401,0 ) )
			return -1;

		ret = pub_com_NoteClsNo( note_type, beg_no, end_no, tel );
		if ( ret )
		{
	   		sprintf( acErrMsg, "销柜员凭证错" );
	   		WRITEMSG
	   		return(-1);				
		}
	   		sprintf( acErrMsg, "柜员凭证使用记帐" );
	   		WRITEMSG
		sprintf( acErrMsg, "111111111111111svc_ind=[%d]",g_pub_tx.svc_ind);
		WRITEMSG
		ret = pub_acct_NoteUse_log( ac_id, ac_seqn, note_type, beg_no, end_no
			,atol(end_no+sNoteParm.head_len)-atol(beg_no+sNoteParm.head_len)+1);
		if( ret )
		{
	   		sprintf( acErrMsg, "柜员凭证使用记帐错" );
	   		WRITEMSG
	   		return(-1);				
		}
	}else if ( sNoteParm.sell_ind[0] == 'Y' )
   	{
   		ret = pub_com_CheqClsNo( ac_id, ac_seqn, note_type, beg_no, end_no );
		if ( ret )
		{
	   		sprintf( acErrMsg, "销客户凭证错" );
	   		WRITEMSG
	   		return(-1);				
		}
   	}else
   	{
   		sprintf( acErrMsg, "凭证种类出售标志错，sell_ind=[%s]",sNoteParm.sell_ind );
   		WRITEMSG
   		return(-1);
   	}
   	
   	return(0);
}


/**********************************************************************
 * 函  数 名：  pub_com_NoteClsNo
 * 函数 功能:   柜员凭证销号处理（柜员交易过程中使用凭证）
 * 作者/时间：  jack/2003年12月30日
 * 
 * 参  数：
 *     输入：      
 *        char     note_type[4]    凭证类型
 *        char     beg_no[17]      起始号码
 *        char     end_no[17]      终止号码
 *        char     tel[5]          柜员号
 *
 *     输出：  char g_pub_tx.reply[]     响应码
 *
 *   返回值：  0: 成功    -1: 不成功
 *           
 * 修改历史：  包括修改人、时间、修改内容，以时间倒序方式
 *
********************************************************************/
int pub_com_NoteClsNo( note_type, beg_no, end_no, tel )
    char    *note_type;
    char    *beg_no;
    char    *end_no;
    char    *tel;
{
    struct    note_mst_c    sNoteMst;
    struct    note_min_no_c    sNoteMinNo;
    
    char    cNoteAct[2];            /* 凭证行为 */
    char    tw_br_no[6];            /* 对方机构 */
    char    tw_tel[6];            /* 对方操作员 */
    char     cInOut[2];            /* 转移标志 I：转入 O：转出 */
    
    int    iFlag;                /* 是否管理最小号码 */
    int    ret;
        
    MEMSET_DEBUG( &sNoteMinNo, 0x00, sizeof( struct note_min_no_c ) );
    MEMSET_DEBUG( &sNoteMst, 0x00, sizeof( struct note_mst_c ) );
    
    MEMSET_DEBUG( cNoteAct, 0x00, sizeof( cNoteAct ) );
    MEMSET_DEBUG( tw_br_no, 0x00, sizeof( tw_br_no ) );
    MEMSET_DEBUG( tw_tel, 0x00, sizeof( tw_tel ) );
    MEMSET_DEBUG( cInOut, 0x00, sizeof( cInOut ) );
    
    strcpy( cNoteAct, "D" ); /* 表示柜员使用凭证 */
    strcpy( cInOut, "O" );   /* 柜员使用凭证,属于转出处理 */

    ret = pub_com_ChkNoteHead( note_type, beg_no , tel );	
    if ( ret )
    {
	sprintf( acErrMsg, "检查起始凭证冠子头错" );
	WRITEMSG
	return(-1);
    }
    ret = pub_com_ChkNoteHead( note_type, end_no , tel );	
    if ( ret )
    {
	sprintf( acErrMsg, "检查终止凭证冠子头错" );
	WRITEMSG
	return(-1);
    }
    
    strcpy( sNoteMst.tel, tel );
    strcpy( sNoteMst.note_type, note_type );
    strcpy( sNoteMst.br_no, g_pub_tx.tx_br_no );
    strcpy( sNoteMst.beg_note_no, beg_no );
    strcpy( sNoteMst.end_note_no, end_no );

    /* 计算凭证数量 */
    ret = pub_com_CalNoteCnt( note_type, beg_no, end_no );
    if ( ret == -1 ) 
    {
     	sprintf( acErrMsg, "计算凭证数量错!" );
    	WRITEMSG
   	/*strcpy( g_pub_tx.reply, "N004" );*/	/* 公用函数对reply赋值了 */
  	return(-1);    
    }
    sNoteMst.cnt = ret;
    
    /* 柜员凭证检查:凭证类型是否存在，凭证是否处于使用状态 */
    ret=pub_com_ChkNoteUse( note_type );
    if( ret==-1 )
    {
        sprintf( acErrMsg, "柜员凭证检查出错!" );
        WRITEMSG
        return -1;
    }

    /* 检查柜员是否有此凭证 */
    ret=pub_com_ChkNote( cNoteAct, sNoteMst );
    if( ret==-1 )	/* 以前这里判断的竟然是 1  变态直接不是0不完了 modified by liuxuan 2006-12-13*/
    {
        sprintf( acErrMsg, "凭证状态不正常!" );
        WRITEMSG
        /*strcpy( g_pub_tx.reply, "N002" );*/ /* 公用函数里已经赋值了 */
        return -1;
    }
 
    /* 凭证最小号码检查 */
    iFlag = pub_com_ChkNoteMin( note_type, beg_no, tel );
    if( iFlag == -1 )
    {
        sprintf( acErrMsg, "凭证最小号检查出错!" );
        WRITEMSG
        return -1;
    }
    else if( iFlag == 2 )  /* 不是最小号码，返回最小号码 */
    {
        sprintf( acErrMsg, "凭证号不是最小号!" );
        WRITEMSG
        strcpy( g_pub_tx.reply, "N014" );
        return -1;
    }
    else
    {
        /* 凭证分段处理 */
        ret=pub_com_NoteApart( cNoteAct, sNoteMst );
        if( ret==-1 )
        {
            sprintf( acErrMsg, "凭证分段不成功!" );
            WRITEMSG
            strcpy( g_pub_tx.reply, "N023" );
            return -1;
        }

        /* 登记柜员凭证台账明细 */
        ret=pub_com_RegNoteHst( cNoteAct, sNoteMst, tw_br_no, tw_tel );
        if( ret==-1 )
        {
            sprintf( acErrMsg, "登记柜员凭证台账明细失败!" );
            WRITEMSG
            strcpy( g_pub_tx.reply, "N015" );
            return -1;
        }
            
        if ( !iFlag ) /* 如果iFlag = 0, 则管理最小号，需要登记最小号 */
        {
            /* 登记凭证最小号登记簿（仅当凭证最小号检查返回值为0时登记）*/
            ret=pub_com_RegNoteMin( g_pub_tx.tx_br_no, note_type, beg_no, tel, cInOut);
            if( ret==-1 )
            {
                sprintf( acErrMsg, "登记凭证最小号登记簿失败!" );
                WRITEMSG
                strcpy( g_pub_tx.reply, "N017" );
                return -1;
            }
        }
    }
    return 0;
}

/**********************************************************************
 * 函  数 名：  pub_com_CheqClsNo
 * 函数 功能:   客户凭证销号处理
 * 作者/时间：  jack/2003年12月30日
 * 
 * 参  数：
 *     输入：  	
 *       	long ac_id		帐户ID
 *		int  ac_seqn		帐户序号
 *              char *note_type         凭证类型
 *		char *cheq_beg_no	起始号码
 *		char *cheq_end_no	终止号码
 *
 *     输出：  char g_pub_tx.reply[] 	响应码
 *
 *   返回值：  0: 成功 	非0: 失败
 *           
 * 修改历史：  包括修改人、时间、修改内容，以时间倒序方式
 *
********************************************************************/
int pub_com_CheqClsNo( ac_id, ac_seqn, note_type, cheq_beg_no, cheq_end_no )
	long	ac_id;
	int	ac_seqn;
	char	*note_type;
	char	*cheq_beg_no;
	char	*cheq_end_no;
{
	struct	note_cheq_mst_c	sCheqMst;	/* 记录客户凭证台帐 */
	struct	note_mst_c	sNoteMst;
	char	sts[51];	/* 支票状态 */
	long	tx_date;	/* 交易日期 */
	char	tw_br_no[6];/* 对方机构号 */
   	char   	tw_tel[6];	/* 对方柜员号 */
	int	ret;
	
	MEMSET_DEBUG( &sCheqMst, 0x00, sizeof( struct note_cheq_mst_c ) );
	MEMSET_DEBUG( &sNoteMst, 0x00, sizeof( struct note_mst_c ) );
	
	/***xxx 20030624 upd 检查起始号码是否带冠字头 *
	ret = pub_com_ChkCheqHead( ac_id, ac_seqn, note_type, cheq_beg_no );
	if( ret == -1 )
	{
		sprintf( acErrMsg, "检查是否带冠字头失败!ret=[%d]",ret );
		WRITEMSG
		return(-1);
	}
	* 检查终止号码是否带冠字头 *
	ret = pub_com_ChkCheqHead( ac_id, ac_seqn, note_type, cheq_end_no );
	if( ret == -1 )
	{
		sprintf( acErrMsg, "检查是否带冠字头失败!ret=[%d]",ret );
		WRITEMSG
		return(-1);
	}
	*********************/
	/*
	ret=pub_com_ChkCheqHead_all( ac_id,ac_seqn,note_type,
		cheq_beg_no,cheq_end_no );
	if( ret )
	{
		sprintf( acErrMsg, "检查是否带冠字头失败!ret=[%d]",ret );
		WRITEMSG
		return(-1);
	}
	*/
	
	/* 检查起始号码、终止号码的连续性,冠子头是否相同 */
	ret = pub_com_ChkBoeNote( note_type, cheq_beg_no, cheq_end_no);
	if ( ret == -1 )
	{
		sprintf( acErrMsg, "检查连续性,冠子头失败!ret=[%d]",ret );
		WRITEMSG
		return(-1);		
	}
	/*******************************/
	
	/* 根据帐户ID检查支票是否为该帐号所有 */
	ret=pub_com_ChkAcCheq( ac_id, ac_seqn, note_type, cheq_beg_no, cheq_end_no, sts );
	if( ret == -1 )
	{
		sprintf( acErrMsg, "客户无此凭证或凭证状态不一致!ac_id=[%d],\
		         note_type=[%s],cheq_beg_no=[%s],cheq_end_no=[%s]",\
		         ac_id,note_type,cheq_beg_no, cheq_end_no );
		WRITEMSG
		return(-1);
	}

	if ( sts[0] == '0' )
	{
		sprintf( acErrMsg, "客户无此凭证!sts=[%s]", sts);	
		WRITEMSG
		strcpy( g_pub_tx.reply, "N080" );
		return(-1);
	}else if ( sts[0] == '2' )
	{
		sprintf( acErrMsg, "此凭证已挂失!sts=[%s]", sts);	
		WRITEMSG
		strcpy( g_pub_tx.reply, "N081" );
		return(-1);
	}else if ( sts[0] == '3' )
	{
		sprintf( acErrMsg, "此凭证已回收!sts=[%s]", sts);	
		WRITEMSG
		strcpy( g_pub_tx.reply, "N082" );
		return(-1);
	}else if ( sts[0] == '4' )
	{
		sprintf( acErrMsg, "此凭证已作废!sts=[%s]", sts);	
		WRITEMSG
		strcpy( g_pub_tx.reply, "N083" );
		return(-1);
	}else if ( sts[0] == '5' )
	{
		sprintf( acErrMsg, "此凭证已使用!sts=[%s]", sts);	
		WRITEMSG
		strcpy( g_pub_tx.reply, "N084" );
		return(-1);
	}
	
	/*****************************/
	
	/* 将客户凭证台帐中该支票号码标志置为'5'（使用状态） */
	strcpy( sNoteMst.note_type, note_type );
	strcpy( sNoteMst.beg_note_no, cheq_beg_no );
	strcpy( sNoteMst.end_note_no, cheq_end_no );
	tx_date=g_pub_tx.tx_date; /*JYW JINZHONG20070421*/
	
	ret=pub_com_RegCheq( sNoteMst, ac_id, ac_seqn, tx_date, "G" );
	if( ret == -1 )
	{
		sprintf( acErrMsg, "修改支票状态位错误!ret=[%d]",ret );
		WRITEMSG
		return(-1);
	}
	/* 计算凭证数量 */
	ret = pub_com_CalNoteCnt( note_type,  cheq_beg_no, cheq_end_no );
	if ( ret == -1 )
	{
		sprintf( acErrMsg, "计算凭证数量错，ret = [%d]", ret );	
		WRITEMSG
		strcpy( g_pub_tx.reply, "N010" );
		return(-1);
	}	
	sCheqMst.cnt = ret ;
	sCheqMst.ac_id = g_pub_tx.ac_id;
	sCheqMst.ac_seq = g_pub_tx.ac_seqn;
	strcpy( sCheqMst.note_type, note_type );
	strcpy( sCheqMst.beg_note_no, cheq_beg_no );
	strcpy( sCheqMst.end_note_no ,cheq_end_no );
	
	/* 登记客户凭证台帐明细 C表示客户使用凭证 */
	strcpy( tw_br_no , "" );
	strcpy( tw_tel, "" );
	
	ret=pub_com_RegCheqHst( "C", sCheqMst, tw_br_no, tw_tel );
	if( ret == -1 )
	{
		sprintf( acErrMsg, "登记客户凭证台帐明细失败!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "N021" );
		return(-1);
	}
	
	return 0;
}
