/*************************************************
* 文 件 名:  spM202.c
* 功能描述： 提供本行内部进行现金调缴的功能。
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
#include "com_tx_br_rel_c.h"
#include "cash_book_c.h"
#include "com_branch_c.h"
#include "note_mst_c.h"
#include "note_parm_c.h"

int spM202()
{
	struct	com_tx_br_rel_c	sComTxBrRel;/* 交易机构关系表 */
	struct	cash_book_c		sCashBook;	/* 现金收付登记簿 */
	struct	com_branch_c	sComBranch;	/* 公共机构码表 */
	struct  note_mst_c    sNoteMst;   /* 柜员凭证台账 */
	struct  note_parm_c   sNoteParm;  /* 凭证参数 */
	char	cOperCode[2];				/* 操作码 */
	char	cTwBrNo[6];					/* 对方机构代码 */
	char	tBrNo[6];				  /* 清算对方机构 */
	char	oBrNo[6];				  /* 清算开户机构 */
	char	tTel[5];					/* 清算对方柜员 */
	
	long	lNoteNo;					/* 凭证号码 */
	long  tNoteNo;          /* 付出的流水号(封条)*/
	double	dAmt;						/* 金额 */
	int	ret, num = 0;


	memset( &sComTxBrRel, 0x00, sizeof( struct com_tx_br_rel_c ) );
	memset( &sCashBook, 0x00, sizeof( struct cash_book_c ) );
	memset( &sComBranch, 0x00, sizeof( struct com_branch_c ) );
	memset( &sNoteMst, 0x00, sizeof( struct note_mst_c ) );
	memset( &sNoteParm, 0x00, sizeof( struct note_parm_c ) );
	memset( cOperCode, 0x00, sizeof( cOperCode ) );
	memset( cTwBrNo, 0x00, sizeof( cTwBrNo ) );
	memset( tBrNo, 0x00, sizeof( tBrNo ) );
	memset( oBrNo, 0x00, sizeof( oBrNo ) );
	memset( tTel, 0x00, sizeof( tTel ) );


	/* 数据初始化 */
	pub_base_sysinit();

	/* 从前台屏幕取值 */
	get_zd_data( "0670", cOperCode );		/* 操作码 */
	get_zd_data( "0910", cTwBrNo );			/* 对方机构代码 */
	get_zd_data( "1181", sCashBook.cur_no );	/* 币种 */
	get_zd_long( "0440", &lNoteNo );		/* 凭证号码 */
	get_zd_double( "1184", &dAmt );			/* 金额 */
	
	get_zd_data("0450",g_pub_tx.new_note_no);   /* 凭证号码 */
  get_zd_long("0460", &tNoteNo );             /* 封条号码 */
  strcpy( g_pub_tx.note_type , "152" );   /* 凭证种类 */
vtcp_log("g_pub_tx.new_note_no:%s",lNoteNo);
	if( cOperCode[0] == '0' )			/* 现金付出 */
	{
		/*检查对方网点是否已签退*/
		ret=Com_branch_Sel( g_pub_tx.reply, &sComBranch, "br_no='%s'",
							cTwBrNo );
		if( ret )
		{
			sprintf( acErrMsg, "查询公共机构表出错!!" );
			WRITEMSG
			goto ErrExit;
		}

		if( sComBranch.wrk_sts[0] == '0' )
		{
			sprintf( acErrMsg, "对方网点已经签退!!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "O253" );
			goto ErrExit;
		}

		if( sComBranch.wrk_sts[0] == '*' )
		{
			sprintf( acErrMsg, "对方网点已被关闭!!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "O254" );
			goto ErrExit;
		}

		/* 检查交易机构关系表中是否存在上下级关系 */
		ret = Com_tx_br_rel_Sel( g_pub_tx.reply, &sComTxBrRel, "br_no='%s' and up_br_no='%s'\
							and rel_type=1", cTwBrNo,g_pub_tx.tx_br_no );
		if( ret != 100 && ret )
		{
			sprintf( acErrMsg, "执行Com_tx_br_rel_Sel错!ret=[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}else if( ret == 0 )
		{
			set_zd_data( "0580", "调头寸" );
		}else
		{
			/* 检查交易机构关系表中是否存在上下级关系 */
			ret = Com_tx_br_rel_Sel( g_pub_tx.reply, &sComTxBrRel, "br_no='%s' and up_br_no='%s'\
									and rel_type=1", g_pub_tx.tx_br_no, cTwBrNo );
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

			set_zd_data( "0580", "上介" );
		}
		
	   ret = Note_parm_Sel(g_pub_tx.reply, &sNoteParm, "note_type = '%s'", g_pub_tx.note_type );
	   if( ret == 100 )
	   {
	       sprintf(acErrMsg,"取凭证参数note_parm表错!note_type=[%s]",g_pub_tx.note_type);
	       WRITEMSG
	       strcpy( g_pub_tx.reply, "N001" );
	       goto ErrExit;
	   }else if ( ret )
	   {
	       sprintf( acErrMsg, "执行Note_parm_Sel错误![%d]", ret );
	       WRITEMSG
	       goto ErrExit;
	   }	
		/*** 现金调拨单凭证的处理 add by martin 20071031 ***/
		if( !strcmp(g_pub_tx.tx_br_no,"10001") )
		{
			 vtcp_log("清算开始凭证处理 !strcmp(g_pub_tx.tx_br_no[%s],10001)[%s][%d]\n",g_pub_tx.tx_br_no,__FILE__,__LINE__);
			  	/* 20140427 BY WYW */ 
			   ret=Note_mst_Sel( g_pub_tx.reply, &sNoteMst, " br_no='%s' and  sts='0' and note_type='152' and substr(beg_note_no,9,8)<='%s' and substr(end_note_no,9,8)>='%s'" ,cTwBrNo,g_pub_tx.new_note_no,g_pub_tx.new_note_no);
			   if(ret)
			   	  {
		          	 sprintf(acErrMsg,"支行凭证查询出错!");
		          	 WRITEMSG
		          	 goto ErrExit;
		        }
		     ret=pub_com_NoteClsNo(  g_pub_tx.note_type ,g_pub_tx.new_note_no , g_pub_tx.new_note_no , sNoteMst.tel );
		        if(ret)
		          {
		          	sprintf(acErrMsg,"处理凭证出错!");
		          	WRITEMSG
		          	goto ErrExit;
		          }
		          
		     strcpy( tBrNo , g_pub_tx.tx_br_no );
	       strcpy( oBrNo , g_pub_tx.opn_br_no );
	       /**strcpy( tTel , g_pub_tx.tel );**/
	       strcpy( g_pub_tx.tx_br_no , cTwBrNo );
	       strcpy( g_pub_tx.opn_br_no , cTwBrNo );
	       /**strcpy( g_pub_tx.tel , sNoteMst.tel );**/
	       num++;
	       
		}
			else 
			{
           vtcp_log("非清算开始凭证处理 type[%s][%s][%s][%d]\n",g_pub_tx.note_type,g_pub_tx.new_note_no,__FILE__,__LINE__);
		        
		  
		        ret=pub_com_NoteClsNo(  g_pub_tx.note_type ,g_pub_tx.new_note_no , g_pub_tx.new_note_no , g_pub_tx.tel );
		        if(ret)
		          {
		          	sprintf(acErrMsg,"处理凭证出错!");
		          	WRITEMSG
		          	goto ErrExit;
		          }
		  }		          
		sprintf( acErrMsg, "柜员凭证使用记帐" );
	  WRITEMSG
		sprintf( acErrMsg, "111111111111111svc_ind=[%d]",g_pub_tx.svc_ind);
		WRITEMSG
		ret = pub_acct_NoteUse_log( 0 , 0 , g_pub_tx.note_type, g_pub_tx.new_note_no, g_pub_tx.new_note_no ,atol(g_pub_tx.new_note_no+sNoteParm.head_len)-atol(g_pub_tx.new_note_no+sNoteParm.head_len)+1);
		if( ret )
		{
	   		sprintf( acErrMsg, "柜员凭证使用记帐错" );
	   		WRITEMSG
	   		goto ErrExit;		
		}
		if( num!=0 )
			{
				 vtcp_log("清算凭证处理结束num=[%d][%s][%d] \n",num,__FILE__,__LINE__);
		     
	       strcpy( g_pub_tx.tx_br_no , tBrNo );
	       strcpy( g_pub_tx.opn_br_no , oBrNo );
	       /**strcpy( g_pub_tx.tel , tTel );**/
			}
		
		/** end martin **/

		strcpy( sCashBook.tw_br_no, cTwBrNo );			/* 对方机构代码 */
		strcpy( sCashBook.br_no, g_pub_tx.tx_br_no );	/* 交易机构 */
		strcpy( sCashBook.tel, g_pub_tx.tel );			/* 柜员号码 */
		strcpy( sCashBook.ind, "0" );					/* 	获取标志: 0 未取 1 已取 */
		/*sCashBook.wrk_no=0;
		sCashBook.out_no=g_pub_tx.trace_no;*/
		sCashBook.wrk_no=g_pub_tx.trace_no;           /*** 付出暂为封条号 ***/
		sCashBook.out_no = atol(g_pub_tx.new_note_no);/*** 凭证号 ***/
		sCashBook.trace_no = g_pub_tx.trace_no;		/* 流水号，现金调拨单号码 */
		sCashBook.amt = dAmt;
		sCashBook.tx_date = g_pub_tx.tx_date;

		/* 登记现金收付登记簿 */
		ret = Cash_book_Ins( sCashBook, g_pub_tx.reply );
		if( ret )
		{
			sprintf( acErrMsg, "执行Cash_book_Ins错!ret=[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}

		/* 为记流水赋值 */
		strcpy( g_pub_tx.add_ind, "0" );			/* 增减:0减1加 */
			
		set_zd_data( "0680", "1" );		/* 操作码 */
	}
	else			/* 现金收入 */
	{
		/* 查询现金收付登记簿中是否存在与输入的对方机构代码，
		金额一致并且获取标志为"未取"的纪录 */
		/***sprintf(acErrMsg, "br_no='%s' \
					and tw_br_no='%s' and trace_no=%ld and tx_date=%d",\
					cTwBrNo, g_pub_tx.tx_br_no,\
					lNoteNo, g_pub_tx.tx_date );
		WRITEMSG
		ret = Cash_book_Dec_Upd( g_pub_tx.reply, "br_no='%s' \
					and tw_br_no='%s' and trace_no=%ld and tx_date=%d",\
					cTwBrNo, g_pub_tx.tx_br_no,\
					lNoteNo, g_pub_tx.tx_date );  20071101 by martin***/
		sprintf(acErrMsg, "br_no='%s' \
					and tw_br_no='%s' and wrk_no=%ld and out_no=%s and tx_date=%d",\
					cTwBrNo, g_pub_tx.tx_br_no,\
					tNoteNo,g_pub_tx.new_note_no, g_pub_tx.tx_date );
		WRITEMSG
		ret = Cash_book_Dec_Upd( g_pub_tx.reply, "br_no='%s' \
					and tw_br_no='%s' and wrk_no=%ld and out_no=%s and tx_date=%d",\
					cTwBrNo, g_pub_tx.tx_br_no,\
					tNoteNo,g_pub_tx.new_note_no, g_pub_tx.tx_date );
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
			strcpy( g_pub_tx.reply, "M006" );
			goto ErrExit;
		}else if ( ret )
		{
			sprintf( acErrMsg, "执行Cash_book_Fet_Upd错!ret=[%d]",ret );
			WRITEMSG
			goto ErrExit;
		}
sprintf( acErrMsg, "sCashBook.trace_no[%ld]lNoteNo[%ld]",sCashBook.trace_no,lNoteNo );
WRITEMSG
		if( sCashBook.ind[0] == '1' )
		{
			sprintf( acErrMsg, "该笔付出记录已作收入!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "M031" );
			goto ErrExit;
		}else if( sCashBook.ind[0] == '2' )
		{
			sprintf( acErrMsg, "该笔记录为预开票记录，总行营业部还未做核销记账!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "M033" );
			goto ErrExit;
		}

		if( pub_base_CompDblVal( sCashBook.amt, dAmt ) != 0 )
		{
			sprintf( acErrMsg, "所输金额与付出金额不符!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "M030" );
			goto ErrExit;
		}

		strcpy( sCashBook.ind, "1" );	/* 获取标志: 0 未取 1 已取 */
		sCashBook.wrk_no=g_pub_tx.trace_no;

		ret = Cash_book_Upd_Upd( sCashBook, g_pub_tx.reply );
		if( ret )
		{
			sprintf( acErrMsg, "执行Cash_book_Upd_Upd错!ret=[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}

    		Cash_book_Clo_Upd( );

		/* 为记流水赋值 */
		strcpy( g_pub_tx.add_ind, "1" );			/* 增减:0减1加 */

		set_zd_data( "0680", "2" );		/* 操作码 */
	}

	/* 记流水日志 */
/*	strcpy(g_pub_tx.no_show,"1");*/

	strcpy( g_pub_tx.cur_no, sCashBook.cur_no );	/* 币种 */
	strcpy( g_pub_tx.ac_no, "10104" );	/* 在途人民币现金 */
	strcpy( g_pub_tx.ct_ind, "1" );	/* 现转 */
	strcpy( g_pub_tx.hst_ind, "1" );	/* 现转 */
	strcpy( g_pub_tx.brf, "对内现金收付" );
	g_pub_tx.tx_amt1 = dAmt;
	if( cOperCode[0] == '0' )			/* 现金付出 */
		strcpy( g_pub_tx.add_ind, "1" );
	else						/* 现金收入 */
		strcpy( g_pub_tx.add_ind, "0" );
	strcpy(g_pub_tx.beg_note_no,cTwBrNo); /*将对方机构放到这里为了查流水时知道是给了哪个机构 */
	
	ret = pub_ins_trace_log();
	if ( ret )
	{
		sprintf( acErrMsg, "记流水日志错" );
		WRITEMSG
		goto ErrExit;
	}

	if( cOperCode[0] == '0' )			/* 现金付出 */
		goto OkExit1;
	else						/* 现金收入 */
		goto OkExit2;

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
OkExit1:
	strcpy( g_pub_tx.reply, "0001" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
OkExit2:
	strcpy( g_pub_tx.reply, "0002" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
}
