/*************************************************
* 文 件 名:  spN114.c
* 功能描述： 制卡交易
*
* 作    者:  andy
* 完成日期： 2004年1月22日
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
#include "card_book_c.h"
#include "mdm_attr_c.h"
extern int pubf_card_loop_create_cardno(const char * start_no,const char * end_no,long int mk_date,char * filename,char **errmsg);
int spN114()
{
	char	cHstFlag[2];		/* 是否删除明细标志Y删除N不删除*/
								/* 如果不删除，将新增一条明细 */
	long	trace_no;			/* 流水号 */
	int	ret;
	int len1,len;
	long cnt,cnt1;
	struct card_book_c card_book;
	struct card_book_c o_card_book;
	struct mdm_attr_c mdm_attr;
	double dMaxno=0;
	char cMaxno[20];
	memset(cMaxno, 0 , sizeof(cMaxno));
	/* 初始化g_pub_tx结构 */
	pub_base_sysinit();
	/**本方权限检查**/
	if( pub_com_NoteAuthChk( g_pub_tx.tx_br_no,g_pub_tx.tel,101,0 ) )
	{
		vtcp_log("%s,%d 权限检查错误",__FILE__,__LINE__);
		goto ErrExit;
	}
	get_zd_data( "1161" , g_pub_tx.mdm_code );
	get_zd_data( "0370" , g_pub_tx.ac_no );
	get_zd_data( "0380" , g_pub_tx.ac_no1 );
	get_zd_long( "0520" , &cnt );

	len=strlen( g_pub_tx.ac_no );
	len1=strlen( g_pub_tx.ac_no1 );
	vtcp_log("%s,%d ac_no=[%s]",__FILE__,__LINE__,g_pub_tx.ac_no);
	if( len!=16 && len!=19 )
	{
		sprintf( acErrMsg,"卡号长度错误" );
		WRITEMSG
		strcpy( g_pub_tx.reply,"O023" );
		goto ErrExit;
	}
	if( len1!=16 && len1!=19 )
	{
		sprintf( acErrMsg,"卡号长度错误" );
		WRITEMSG
		strcpy( g_pub_tx.reply,"O023" );
		goto ErrExit;
	}
	sql_max_double("card_book","end_no",&dMaxno,"1=1");
	vtcp_log("%s,%d max no ==[%f]",__FILE__,__LINE__,dMaxno);
	dMaxno++;
	sprintf(cMaxno,"%08.0f",dMaxno);
	if(memcmp(cMaxno,g_pub_tx.ac_no+8,8))
	{
		sprintf( acErrMsg,"输入的最小号与原有最大号不连续错误" );
		WRITEMSG
		vtcp_log("%s,%d max_no=[%s],start_no=[%s]",__FILE__,__LINE__,cMaxno,g_pub_tx.ac_no);
		strcpy( g_pub_tx.reply,"D184" );
		goto ErrExit;		
	}
	/* rem by LiuHuafeng 感觉原来程序是制完卡后领回来后才用制卡程序
		这就不是真正的制卡过程了,所以把校验卡的校验位改为产生卡的校验位
	if( pub_card_par_1212( 0,g_pub_tx.ac_no,g_pub_tx.crd_no ) )
	{
		sprintf( acErrMsg,"卡号[%s]错误",g_pub_tx.ac_no );
		WRITEMSG
		strcpy( g_pub_tx.reply,"D182" );
		goto ErrExit;
	}
	if( pub_card_par_1212( 0,g_pub_tx.ac_no1,g_pub_tx.crd_no ) )
	{
		sprintf( acErrMsg,"卡号[%s]错误",g_pub_tx.ac_no1 );
		WRITEMSG
		strcpy( g_pub_tx.reply,"D183" );
		goto ErrExit;
	}
	*******以下是产生卡的校验位过程*******/
	if( pub_card_par_1212( 1,g_pub_tx.ac_no,g_pub_tx.crd_no ) )
	{
		sprintf( acErrMsg,"卡号[%s]错误",g_pub_tx.ac_no );
		WRITEMSG
		strcpy( g_pub_tx.reply,"D182" );
		goto ErrExit;
	}
	if( pub_card_par_1212( 1,g_pub_tx.ac_no1,g_pub_tx.crd_no ) )
	{
		sprintf( acErrMsg,"卡号[%s]错误",g_pub_tx.ac_no1 );
		WRITEMSG
		strcpy( g_pub_tx.reply,"D183" );
		goto ErrExit;
	}
	/* end by LiuHuafeng 20070125 ****/
	
	pub_card_CardToNote( g_pub_tx.ac_no,g_pub_tx.beg_note_no ) ;
vtcp_log("%s,%d end_not_no==%s,beg_note_no=[%s]",__FILE__,__LINE__,g_pub_tx.end_note_no,g_pub_tx.beg_note_no);
	pub_card_CardToNote( g_pub_tx.ac_no1,g_pub_tx.end_note_no ) ;
vtcp_log("%s,%d end_not_no==%s,beg_note_no=[%s]",__FILE__,__LINE__,g_pub_tx.end_note_no,g_pub_tx.beg_note_no);
	cnt1=atol(g_pub_tx.end_note_no+8)-atol(g_pub_tx.beg_note_no+8)+1;
	if( cnt!=cnt1 )
	{
		vtcp_log("%s,%d ac_no==%s,ac_no1=[%s]",__FILE__,__LINE__,g_pub_tx.ac_no,g_pub_tx.ac_no1);
		vtcp_log("%s,%d end_not_no==%s,beg_note_no=[%s]",__FILE__,__LINE__,g_pub_tx.end_note_no,g_pub_tx.beg_note_no);
		vtcp_log("%s,%d cnt=%d,cnt1=[%d]",__FILE__,__LINE__,cnt,cnt1);
		vtcp_log("%s,%d end_not=%d,beg_not=[%d]",__FILE__,__LINE__,atol(g_pub_tx.end_note_no+8),atol(g_pub_tx.beg_note_no+8));
		sprintf( acErrMsg,"卡号长度错误" );
		WRITEMSG
		strcpy( g_pub_tx.reply,"N079" );
		goto ErrExit;
	}

	memset( &card_book,0,sizeof(card_book) );

	strcpy( card_book.mdm_code,g_pub_tx.mdm_code );
	strcpy( card_book.beg_no,g_pub_tx.ac_no );
	strcpy( card_book.end_no,g_pub_tx.ac_no1 );
	card_book.cnt=cnt ;
	card_book.tx_date=g_pub_tx.tx_date ;
	strcpy( card_book.br_no,g_pub_tx.tx_br_no );
	strcpy( card_book.tel,g_pub_tx.tel );

	ret = Card_book_Sel( g_pub_tx.reply,&o_card_book,
		"mdm_code='%s' and beg_no>='%s' and end_no<='%s'",
		g_pub_tx.mdm_code,card_book.beg_no,card_book.end_no );
	if( ret==100 )
	{
		;
	}
	else if( ret )
	{
		TRACE
		goto ErrExit;
	}
	else
	{
		sprintf(acErrMsg,"部分存在[%s][%s][%s]",
			g_pub_tx.mdm_code,card_book.beg_no,card_book.end_no );
		WRITEMSG
		strcpy( g_pub_tx.reply,"D184" );
		goto ErrExit;
	}

	if( Card_book_Ins( card_book , g_pub_tx.reply ) )
		goto ErrExit;
	
	ret=Mdm_attr_Sel(g_pub_tx.reply,&mdm_attr,"mdm_code='%s'",g_pub_tx.mdm_code);
	if( ret )
	{
		TRACE
		goto ErrExit;
	}

	strcpy( g_pub_tx.note_type,mdm_attr.note_type );
	g_pub_tx.tx_amt1=(double)cnt;

	/* rem by LiuHuafeng 20070125
	 * 凭证入库让柜员自己入库这样比较灵活
	if( pzrk() )
		goto ErrExit;
	*****************************/
	/* add by LiuHuafeng 2007-1-26 16:45 增加写制作卡文件功能 */
	{
		char * pErrmsg;
		char cFilename[512];
		memset(cFilename , 0 , sizeof(cFilename));
		vtcp_log("%s,%d beg_no=[%s], end_no=[%s]",__FILE__,__LINE__,card_book.beg_no,card_book.end_no);
		g_reply_int=pubf_card_loop_create_cardno(card_book.beg_no,card_book.end_no,g_pub_tx.tx_date,cFilename,&pErrmsg);
		if(g_reply_int)
		{
			TRACE
			strncpy(g_pub_tx.reply,"P157",4);
			memcpy(acErrMsg,pErrmsg,40);
			goto ErrExit;
		}
	}
	/* end by LiuHuafeng 2007-1-26 16:46 */
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
int pzrk()
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
	ret = pub_com_ChkNoteMin( g_pub_tx.note_type, g_pub_tx.beg_note_no, \
				  g_pub_tx.tel );
	if ( ret == -1 )
	{
		sprintf( acErrMsg, "检查凭证最小号错" );
		WRITEMSG
		goto ErrExit;	
	}
	if ( !ret  )
	{
		/* 登记最小凭证号码 */
		ret = pub_com_RegNoteMin( g_pub_tx.tx_br_no, g_pub_tx.note_type, \
				g_pub_tx.beg_note_no, g_pub_tx.tel, cInOut);
		if ( ret )
		{
			sprintf( acErrMsg, "登记凭证最小号错" );
			WRITEMSG
			goto ErrExit;	
		}
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
	return 0;
ErrExit:
	return 1;
}

