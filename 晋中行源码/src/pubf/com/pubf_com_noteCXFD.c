#include "find_debug.h" 
#include <stdio.h>  
#define EXTERN
#include "public.h"
#include "note_cheq_mst_c.h"
#include "note_cheq_hst_c.h"
#include "note_mst_c.h"
#include "note_mst_hst_c.h"
#include "note_min_no_c.h"
#include "note_parm_c.h"

/**********************************************************************
 * 函 数 名：  pub_com_NoteApartCX
 * 函数功能:   凭证分段处理。把原来属于一个区间段的凭证按照传入的起始
 *	       号码和终止号码分开，该区间的起始号码到传入起始号码前是
 *	       一段传入起始号码到终止号码是一段。传入终止号码后到该区
 *	       间终止号码为一段。如果是起始号码相同、终止号码相同或都
 *	       同可能分两段或一段。
 *             分段后可能存在并段情况（即与状态相同的号码相连），则需要进行并段处理
 *		如果入库撤销，需要删除台帐
 *
 *		（凭证管理交易中撤销用此函数）
 *
 * 作者/时间： jack/2004年12月16日
 * 
 * 参数：
 *     输入：
 *            char   *note_act             凭证行为
 *            struct note_mst_c note_mst   柜员凭证台帐
 *
 *     输出：  char g_pub_tx.reply 相应码
 *
 *   返回值：  0: 并段成功 -1: 不成功
 *           
 * 修改历史：  包括修改人、时间、修改内容，以时间倒序方式
 *
********************************************************************/
int pub_com_NoteApartCX(
         char *note_act,
         struct note_mst_c note_mst )
{
    struct note_mst_c sNoteMst;  /* 从柜员凭证台帐取出的值 */
    struct note_mst_c sNoteMstTmp; /* 存放柜员凭证台帐的临时变量 */
    char   cNoteSts[2];          /* 凭证状态 */
    char   cPerNoteSts[2];	/* 行为执行的状态 */
    char   cWho[2];		/* 凭证行为执行者 */
    char   str_tmp[32];
    int    ret;
    
    MEMSET_DEBUG(&sNoteMst, 0x00, sizeof( struct note_mst_c ));
    MEMSET_DEBUG(cNoteSts, 0x00, sizeof(cNoteSts) );
    MEMSET_DEBUG(cPerNoteSts, 0x00, sizeof(cPerNoteSts) );
    MEMSET_DEBUG(cWho, 0x00, sizeof(cWho) );
    
    strcpy( cWho, "T" );   /* T表示柜员 */

    /* 根据凭证行为找出凭证状态 */
    ret = pub_com_NoteActSts( cWho, note_act, cNoteSts,cPerNoteSts);
    if ( ret )
    {
        sprintf(acErrMsg, "根据凭证行为取凭证状态错!");
        WRITEMSG
        strcpy( g_pub_tx.reply, "N002" );
        return(-1);    
    }
    strcpy( note_mst.sts, cPerNoteSts );   /* 凭证行为执行后凭证的状态 */
   
    /* 检查是否存在这样的区间 */
    /* 带原凭证状态的查找 */
    ret = Note_mst_Dec_Upd(g_pub_tx.reply, "tel = '%s' and note_type = '%s' \
                      and beg_note_no <= '%s' and end_note_no >= '%s' \
                      and sts = '%s'", note_mst.tel, note_mst.note_type,\
                      note_mst.beg_note_no, note_mst.end_note_no, cNoteSts );
    if ( ret )
    {
        sprintf( acErrMsg, "执行Note_mst_Dec_Upd错!ret=[%d]",ret );
        WRITEMSG
        return(-1);
    }
   
    ret = Note_mst_Fet_Upd( &sNoteMst,g_pub_tx.reply );
    
    if ( ret == 100 )
    {
        sprintf( acErrMsg, "不存在这样的凭证区间!" );
        WRITEMSG
        strcpy( g_pub_tx.reply, "N007" );
        return(-1);
    }else if ( ret )
    {
        sprintf( acErrMsg, "执行Note_mst_Fet_Upd出错!ret=[%d]",ret );
        WRITEMSG
        return(-1);
    }else                 /* 0 找到满足条件的记录 */
    {
        /* 检查头是否相同 */
        if( !strcmp(sNoteMst.beg_note_no, note_mst.beg_note_no) ) /* 头相同 */
        {
            if( !strcmp(sNoteMst.end_note_no, note_mst.end_note_no) ) /* 尾相同 */
            {
                /* 修改凭证状态 */
                strcpy( sNoteMst.sts , note_mst.sts );
                
                ret = Note_mst_Upd_Upd( sNoteMst, g_pub_tx.reply );
             	if ( ret == -239 )
                {
                        sprintf( acErrMsg, "修改柜员凭证台帐note_mst错,存在重复记录!" );
                	WRITEMSG
                	strcpy(g_pub_tx.reply,"N019");
                	return(-1);
            	}else if( ret )
                {
                    sprintf( acErrMsg,"执行Note_mst_Upd_Upd错!ret=[%d]",ret );
                    WRITEMSG
                    return(-1);
                }
                /**********************************************
                  做并段处理：                                
                           检查是否存在与修改后头尾相连的号码段  
                           需要检查头尾是否同时相同的,并sNoteMst或者note_mst           
                **********************************************/
    		ret = pub_com_NoteApartMerg( note_mst );
    		if ( ret == -1 )
    		{
    			sprintf( acErrMsg, "凭证并段失败" );
    			WRITEMSG
    			return(-1);
    		}
            }else  /* 尾不同 */
            {
                /* 1: 插入一条新纪录*/
                ret = Note_mst_Ins( note_mst,g_pub_tx.reply );
    		if ( ret == -239 )
    		{
    			sprintf(acErrMsg,"向柜员凭证台帐note_mst插入记录错!有重复记录!");
        		WRITEMSG
        		strcpy(g_pub_tx.reply,"N020");
        		return(-1);
    		}else if( ret )
    		{
        		sprintf( acErrMsg,"执行Note_mst_Ins错!ret=[%d]",ret );
        		WRITEMSG
        		return(-1);
    		}
                /*********************************************
                  做并段处理：                                
                           检查是否存在与修改后头尾相连的号码段  
                           需要检查头是否同时相同的，并note_mst          
                **********************************************/
    		ret = pub_com_NoteApartMerg( note_mst );
    		if ( ret == -1 )
    		{
    			sprintf( acErrMsg, "凭证并段失败" );
    			WRITEMSG
    			return(-1);
    		}
                            
                /* 2: 修改原记录的起始号码、状态、数量 */
                ret = pub_com_CalNoteNo(note_mst.note_type,\
                          note_mst.end_note_no, str_tmp,1);
                if ( ret == -1 ) 
                {
                	sprintf( acErrMsg, "计算凭证号码错!" );
                	WRITEMSG
                	strcpy( g_pub_tx.reply, "N004" );
                	return(-1);    
                }
                
                strcpy( sNoteMst.beg_note_no, str_tmp );
                
                /* 计算凭证数量 */
                sNoteMst.cnt -=  note_mst.cnt;

                /* 修改原凭证 */
                ret = Note_mst_Upd_Upd( sNoteMst,g_pub_tx.reply );
             	if ( ret == -239 )
                {
                        sprintf( acErrMsg, "修改柜员凭证台帐note_mst错,有重复记录!" );
                	WRITEMSG
                	strcpy(g_pub_tx.reply,"N019");
                	return(-1);
            	}else if( ret )
                {
                    sprintf( acErrMsg,"执行Note_mst_Upd_Upd错!ret=[%d]",ret );
                    WRITEMSG
                    return(-1);
                }
            }
        }else /* 头不同 */
        {
            if( !strcmp(sNoteMst.end_note_no, note_mst.end_note_no) ) /* 尾相同 */
            {
                /* 1：插入新的数据 */
                ret = Note_mst_Ins( note_mst,g_pub_tx.reply );
    		if ( ret == -239 )
    		{
    			sprintf(acErrMsg,"向柜员凭证台帐note_mst插入记录错!有重复记录!");
        		WRITEMSG
        		strcpy(g_pub_tx.reply,"N020");
        		return(-1);
    		}else if( ret )
    		{
        		sprintf( acErrMsg,"执行Note_mst_Ins错!ret=[%d]",ret );
        		WRITEMSG
        		return(-1);
    		}
                /**********************************************
                  做并段处理：                                
                           检查是否存在与修改后头尾相连的号码段  
                           需要检查尾是否同时相同的,并note_mst        
                **********************************************/
    		ret = pub_com_NoteApartMerg( note_mst );
    		if ( ret == -1 )
    		{
    			sprintf( acErrMsg, "凭证并段失败" );
    			WRITEMSG
    			return(-1);
    		}    		    		
    		
                /* 2：修改原数据 */
                /* 计算凭证终止号码 */
                ret = pub_com_CalNoteNo(note_mst.note_type,\
                          note_mst.beg_note_no, str_tmp, -1);
                if ( ret == -1 ) 
                {
                    	sprintf( acErrMsg, "计算凭证号码错!" );
                	WRITEMSG
                	strcpy( g_pub_tx.reply, "N004" );
                	return(-1);    
                }                
                strcpy( sNoteMst.end_note_no, str_tmp );
                
                /* 计算凭证数量 */
                sNoteMst.cnt = sNoteMst.cnt - note_mst.cnt;
                
                ret = Note_mst_Upd_Upd( sNoteMst,g_pub_tx.reply );
             	if ( ret == -239 )
                {
                    sprintf( acErrMsg, "修改柜员凭证台帐note_mst错,有重复记录!" );
                    WRITEMSG
                    strcpy(g_pub_tx.reply,"N019");
                    return(-1);
            	}else if( ret )
                {
                    sprintf( acErrMsg,"执行Note_mst_Upd_Upd错!ret=[%d]",ret );
                    WRITEMSG
                    return(-1);
                }
            }else   /* 尾不同 */
            {
                sNoteMstTmp = sNoteMst;
                
                /* 修改原来的凭证，分成三段 */
                /* 1：计算凭证终止号码 */
                ret = pub_com_CalNoteNo(sNoteMst.note_type,note_mst.beg_note_no,\
                                      sNoteMst.end_note_no, -1);
                if ( ret == -1 ) 
                {
                    	sprintf( acErrMsg, "计算凭证号码错!" );
                	WRITEMSG
                	strcpy( g_pub_tx.reply, "N004" );
               		return(-1);    
                }
                
                ret = pub_com_CalNoteCnt( note_mst.note_type, \
                        sNoteMst.beg_note_no, sNoteMst.end_note_no );
                if ( ret == -1 ) 
                {
                    	sprintf( acErrMsg, "计算凭证数量错!" );
                	WRITEMSG
                	strcpy( g_pub_tx.reply, "N004" );
                	return(-1);    
                }
                sNoteMst.cnt = ret;
             
                /* 修改柜员凭证 */                             
                ret = Note_mst_Upd_Upd( sNoteMst,g_pub_tx.reply );
             	if ( ret == -239 )
                {
                        sprintf( acErrMsg, "修改柜员凭证台帐note_mst错!" );
                	WRITEMSG
                	strcpy(g_pub_tx.reply,"N019");
                	return(-1);
            	}else if( ret )
                {
                    sprintf( acErrMsg,"执行Note_mst_Upd_Upd错!ret=[%d]",ret );
                    WRITEMSG
                    return(-1);
                }
            
                /* 2：插入新的 */
                /* 插入一条新纪录*/
                ret = Note_mst_Ins( note_mst,g_pub_tx.reply );
    		if ( ret == -239 )
    		{
    			sprintf(acErrMsg,"向柜员凭证台帐note_mst插入记录错!有重复记录!");
        		WRITEMSG
        		strcpy(g_pub_tx.reply,"N020");
        		return(-1);
    		}else if( ret )
    		{
        		sprintf( acErrMsg,"执行Note_mst_Ins错!ret=[%d]",ret );
        		WRITEMSG
        		return(-1);
    		}
                            
                /* 3：插入另一个 */
                /* 计算凭证起始号码 */
                ret = pub_com_CalNoteNo(sNoteMstTmp.note_type,\
                          note_mst.end_note_no, sNoteMstTmp.beg_note_no, 1);
                if ( ret == -1 ) 
                {
                    	sprintf( acErrMsg, "计算凭证号码错!" );
                	WRITEMSG
                	strcpy( g_pub_tx.reply, "N004" );
                	return(-1);
                }
                /* 计算凭证数量 */
                ret = pub_com_CalNoteCnt( sNoteMstTmp.note_type, \
                        sNoteMstTmp.beg_note_no, sNoteMstTmp.end_note_no );
                if ( ret == -1 ) 
                {
                    	sprintf( acErrMsg, "计算凭证号码错!" );
                	WRITEMSG
                	strcpy( g_pub_tx.reply, "N004" );
                	return(-1);    
                }
                sNoteMstTmp.cnt = ret;
                /* 插入一条新纪录*/
                ret = Note_mst_Ins( sNoteMstTmp,g_pub_tx.reply );
    		if ( ret == -239 )
    		{
    			sprintf(acErrMsg,"向柜员凭证台帐note_mst插入记录错!有重复记录!");
        		WRITEMSG
        		strcpy(g_pub_tx.reply,"N020");
        		return(-1);
    		}else if( ret )
    		{
        		sprintf( acErrMsg,"执行Note_mst_Ins错!ret=[%d]",ret );
        		WRITEMSG
        		return(-1);
    		}               
            } 
        }
        
        /* 如果为入库行为，需要删除 */
        if ( note_act[0] == '1' )
        {
        	ret = sql_execute("delete from note_mst where note_type = '%s' \
        			and beg_note_no = '%s' and end_note_no = '%s' \
        			and tel = '%s'", note_mst.note_type, \
        			note_mst.beg_note_no, note_mst.end_note_no, \
        			note_mst.tel );
        	if ( ret )
        	{
        		sprintf( acErrMsg, "删除柜员凭证登记薄错[%d]", ret );
        		WRITEMSG
        		strcpy( g_pub_tx.reply, "N061" );
        		return(-1);	
        	}
        }
    }
    
    /* 关闭游标 */
    Note_mst_Clo_Upd( );
    
    return(0);
}

/**********************************************************************
 * 函  数 名：  pub_com_RegCheqCX
 * 函数 功能:   登记支票登记簿/修改凭证状态,按照支票的号码登记对应的记录。如果已经存在这本
 *             凭证，则需要修改对应状态、凭证数量、是否占用本数、购买日期。如
 *             不存在，则新增加一条记录既可。
 *             
 *             （凭证管理交易中撤销用此函数）
 *
 * 作者/时间：  jack//2004年12月27日
 * 
 * 参  数：
 *     输入：  
 *            struct note_mst_c note_mst  柜员凭证台帐
 *            long ac_id       帐户id
 *	      long ac_seqn	帐号序号
 *            long tx_date     交易日期
 *            char *note_act   客户凭证行为
 *
 *     输出：  
 *
 *   返回值：  0: 成功   -1: 失败
 *           
 * 修改历史：  包括修改人、时间、修改内容，以时间倒序方式
 *
********************************************************************/
int pub_com_RegCheqCX(
    struct note_mst_c note_mst,
    int ac_id,
    long ac_seqn,
    long tx_date,
    char *note_act)
{
    struct note_mst_c sNoteMst;       	/* 柜员凭证台帐 */
    struct note_parm_c sNoteParm;       /* 凭证参数 */
    struct note_cheq_mst_c sCheqMst;    /* 客户凭证台帐 */
    
    char    cBegNoteNo[17], cEndNoteNo[17];
    char    cHead[17];            /* 冠子头 */
    char    str_tmp[17];
    char    cForm[32];            /* 输入格式 */
    char    cBookSts[51];         /* 支票状态 */
    char    cABookSts[2];	  /* 支票执行之后的状态 */
    char    cBBookSts[2];	  /* 支票执行之前的状态 */
    char    cWho[2];  		  /* 凭证执行者 */
	    
    long    lBegNoteNo, lEndNoteNo;    /* note_mst中的起始、终止号码 */
    long    lBegNo, lEndNo;    	/* 总的起始、终止号码 */
    long    lDbBeg, lDbEnd; 	/* 查找数据库时候用到的区间段 */
    
    int    i, j, k, m = 0;    	/* 循环用,m 用来记录总共循环几次 */    
    int    iNoteCnt;    	/* 可用凭证数量 */
    int    ret;
    
    MEMSET_DEBUG( &sNoteMst, 0x00, sizeof( struct note_mst_c ) );
    MEMSET_DEBUG( &sNoteParm, 0x00, sizeof( struct note_parm_c ) );
    MEMSET_DEBUG( &sCheqMst, 0x00, sizeof( struct note_cheq_mst_c ) );
    
    MEMSET_DEBUG( cBegNoteNo, 0x00, sizeof( cBegNoteNo ) );
    MEMSET_DEBUG( cEndNoteNo, 0x00, sizeof( cEndNoteNo ) );
    MEMSET_DEBUG( cBookSts, 0x00, sizeof( cBookSts ) );
    MEMSET_DEBUG( cABookSts, 0x00, sizeof( cABookSts ) );
    MEMSET_DEBUG( cBBookSts, 0x00, sizeof( cBBookSts ) );
        
    MEMCPY_DEBUG( &sNoteMst, &note_mst, sizeof( struct note_mst_c ) );
    strcpy( cWho , "C" );		/* 表示客户 */
    
    /* 取凭证参数长度 */
    ret = Note_parm_Sel(g_pub_tx.reply, &sNoteParm, "note_type = '%s'", sNoteMst.note_type );
    if ( ret == 100 )
    {
        sprintf( acErrMsg, "无此凭证类型note_type=[%s]", sNoteMst.note_type );
        WRITEMSG
        strcpy( g_pub_tx.reply, "N001" );
        return(-1);
    }else if ( ret )
    {
        sprintf( acErrMsg, "执行Note_parm_Sel错![%d]", ret );
        WRITEMSG
        return(-1);        
    }    
        
    /* 将凭证状态初始化成0 */
    MEMSET_DEBUG( &cBookSts, '0', sNoteParm.unit_cnt );
    
    /* 根据客户凭证行为取执行之后的凭证状态 */
    ret = pub_com_NoteActSts( cWho, note_act, cABookSts, cBBookSts );
    if ( ret )
    {
        sprintf(acErrMsg, "根据凭证行为取凭证状态错!");
        WRITEMSG
        strcpy( g_pub_tx.reply, "N002" );
        return(-1);    
    }
   vtcp_log("ROL aaaaaaaaaa[%s][%s][%s][%s]", cWho, note_act, cABookSts, cBBookSts );
    
    /* 去掉冠子头后凭证的起始号码和终止号码 */
    lBegNoteNo = atol( sNoteMst.beg_note_no + sNoteParm.head_len );
    lEndNoteNo = atol( sNoteMst.end_note_no + sNoteParm.head_len );
    
    /* 总的起始号码、终止号码 */
    lBegNo = (lBegNoteNo - 1) / sNoteParm.unit_cnt * sNoteParm.unit_cnt + 1;
    lEndNo = ( (lEndNoteNo-1) / sNoteParm.unit_cnt + 1 ) * sNoteParm.unit_cnt;
   
    for ( i = lBegNo; i <= lEndNo; i = i + sNoteParm.unit_cnt )
    {
        m++;
        
        /* 查找数据库用的凭证起始、终止号码 */
        lDbBeg = i;
        lDbEnd = i + sNoteParm.unit_cnt - 1;
        
        /* 加入冠子头 */
        strncpy( cBegNoteNo, sNoteMst.beg_note_no, sNoteParm.head_len );
        strncpy( cEndNoteNo, sNoteMst.end_note_no, sNoteParm.head_len );
        cBegNoteNo[sNoteParm.head_len] = '\0';
        cEndNoteNo[sNoteParm.head_len] = '\0';
        
        /* 带冠子头的起始号码 */
        sprintf( cForm, "%%0%dld", 16 - sNoteParm.head_len );
        sprintf( str_tmp, cForm, lDbBeg );
        strcat( cBegNoteNo, str_tmp);
        
        /* 带冠子头的终止号码 */
        sprintf( str_tmp, cForm, lDbEnd );
        strcat( cEndNoteNo, str_tmp);
     
        /* 根据帐号、凭证种类、起始号码、终止号码查找数据库 */
        ret = Note_cheq_mst_Dec_Upd(g_pub_tx.reply, "ac_id = %ld \
        		and ac_seq = %d and note_type = '%s' \
        		and beg_note_no = '%s' and end_note_no = '%s'", \
        		ac_id, ac_seqn, sNoteMst.note_type, cBegNoteNo, \
        		cEndNoteNo );
        if ( ret )
        {
            sprintf( acErrMsg, "执行Note_cheq_mst_Dec_Upd错!ret= [%d]",ret);
            WRITEMSG
            return(-1);
        }
        ret = Note_cheq_mst_Fet_Upd( &sCheqMst, g_pub_tx.reply);
        if ( ret == 100 )    /* 没找到这样的记录 */
        {
		sprintf( acErrMsg, "无此记录" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "N012" );
		return(-1);
        }else if ( ret )
        {
            sprintf( acErrMsg, "执行Note_cheq_mst_Fet_Upd错!ret= [%d]",ret);
            WRITEMSG
            return(-1);            
        }else
        {        	
            /* 修改标志 */
            iNoteCnt = 0;
        
   vtcp_log("ROL bbbbbbbbbbbbbbbb[%s][%s][%s][%s]", cWho, note_act, cABookSts, cBBookSts );
            for ( j = lDbBeg, k = 0; j <= lDbEnd; j++, k++ )
            {
                if( j >= lBegNoteNo && j <= lEndNoteNo )
                {
		/***MODIFY BY ZHGF 20100514 晋中商行
		为什么赋值为cBBookSts？？错误的，
		应该是cABookSts赋为处理前的状态
			sCheqMst.book_sts[k] = cBBookSts[0]; 
		**XRB cB->cA*/
			sCheqMst.book_sts[k] = cABookSts[0]; /**XRB cB->cA*/
			iNoteCnt++;
                }
            } /* end for */
            
            
             /* 检查撤销后凭证标志是否都是0,如果都是，则删除 */   
             /** 按照wangyongwei改的修改到这 20100514 ADD BY ZHGF 晋中商行
             add by wangyongwei 20070827
	     for ( j = lBegNoteNo, k = 0; j <= lEndNoteNo; j++, k++ )
             **/
             for (j=lDbBeg,k=0; j<=lDbEnd; j++,k++)
             {
                if( sCheqMst.book_sts[k] == '0' )
                {
                    	continue;
                }else
                {
                	break;	
                }
            } /* end for */ 
           
            /* 删除全为0得记录 */
            if ( k == sNoteParm.unit_cnt )
            {
            	continue;
            	ret = sql_execute("delete from note_cheq_mst where  ac_id = %d \
            			and ac_seq = %d and note_type = '%s' \
            			and beg_note_no = '%s' and end_note_no = '%s'", \
            			ac_id, ac_seqn, sNoteMst.note_type, cBegNoteNo, \
            			cEndNoteNo );
            	if ( ret )
            	{
            		sprintf( acErrMsg, "删除客户凭证台帐错[%d]", ret );
            		WRITEMSG
            		strcpy( g_pub_tx.reply, "B061" );
            		return(-1);
            	}
            	continue;
            }
                        
            /* 如果凭证行为是购买或者解挂，凭证数增加 */
            if ( cABookSts[0] == '1' && cBBookSts[0] != '1' )
            {
		/**REPLACE BY NEXT 20100514 ZHGF
		原来是1处理后不是1，说明是处理了，撤销时应该恢复
		sCheqMst.cnt -= iNoteCnt;            
		**/
		sCheqMst.cnt += iNoteCnt;            
            }else if ( cABookSts[0] != '1' && cBBookSts[0] == '1' )
            {
		/* ADD BY ZHGF 20100514 晋中商行
		当原来不是1而处理后是1，因为是撤销所以正好是反的
            	sCheqMst.cnt += iNoteCnt;
		*/
            	sCheqMst.cnt -= iNoteCnt;
            }

            /* 如果可用凭证数量超过规定数量，则占用本数 */
            if ( sCheqMst.cnt >= sNoteParm.ocp_cnt )
            {
                sCheqMst.grp_ind[0] = 'Y';
            }else
            {
                sCheqMst.grp_ind[0] = 'N';
            }
	    
	   
            /* 否则，修改数据 */
            ret = Note_cheq_mst_Upd_Upd( sCheqMst, g_pub_tx.reply);
            if ( ret == -239 )
            {
                sprintf( acErrMsg, "修改客户凭证台帐数据错,有重复记录" );
                WRITEMSG
                strcpy( g_pub_tx.reply, "N021" );
                return(-1);                
            }else if ( ret )
            {
                sprintf( acErrMsg, "执行Note_cheq_mst_Upd_Upd错" );
                WRITEMSG
                return(-1);
            }
        } /* end else */
    }/* end for */
    Note_cheq_mst_Clo_Upd( );  
      
    return(0);
}
