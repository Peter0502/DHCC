/*************************************************
* 文 件 名:  spM003.c
* 功能描述： 现金调剂子交易[付出]
*
* 作    者:  andy
* 完成日期： 2004年3月12日
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
#include "com_tel_connect_c.h"

int spM003()
{
	int ret;
	char teller[6];
	char prtime[9];
	int  hour,min,sec;

	struct	com_tel_connect_c	g_com_tel_connect;
	memset(&g_com_tel_connect,0x00,sizeof(struct com_tel_connect_c));

	/* 系统初始化（初始化g_pub_tx)数据初始化 */
	pub_base_sysinit();
  
	get_zd_data("0920",teller);					/* 对方柜员 */ 
	get_zd_data("0210",g_pub_tx.cur_no);		/* 币种 */ 
	get_zd_double("0410",&g_pub_tx.tx_amt1);	/* 发生额 */

	strcpy(g_pub_tx.add_ind,"0");				/* 减标志 */

	pub_base_strpack(teller);
	pub_base_strpack(g_pub_tx.tel);
	strcpy( g_pub_tx.brf, "现金调剂" );
	vtcp_log("[%s][%d] Ray1 = [%f]",__FILE__,__LINE__,g_pub_tx.tx_amt1);

	if( !strcmp(g_pub_tx.tel,teller) )
	{
		sprintf(acErrMsg,"只能向其他柜员做付出!!");
		WRITEMSG
		strcpy( g_pub_tx.reply, "O058" );
		goto ErrExit;
	}
	if( !strcmp(teller,"") )
	{
		sprintf(acErrMsg,"接收柜员不能为空!!!");
		WRITEMSG
		strcpy( g_pub_tx.reply, "C119" );
		goto ErrExit;
	}
	vtcp_log("[%s][%d] Ray2 = [%f]",__FILE__,__LINE__,g_pub_tx.tx_amt1);

	/*** 为晋中ATM加清钞交易tx_code=9325暂时屏蔽0.00 ***/
	vtcp_log("[%s][%d] g_pub_tx.tx_code = [%s]",__FILE__,__LINE__,g_pub_tx.tx_code);
  if( memcmp(g_pub_tx.tx_code,"9325",4) != 0 && pub_base_CompDblVal( g_pub_tx.tx_amt1,0.00 )==0 )
	{
		sprintf(acErrMsg,"请输入付出金额!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O163");
		goto ErrExit;
	}

	/*** 付出之前检查是否已有付出但未收入 ***/
	vtcp_log("[%s][%d] Ray3 = [%f]",__FILE__,__LINE__,g_pub_tx.tx_amt1);
	ret = Com_tel_connect_Sel(g_pub_tx.reply,&g_com_tel_connect, \
		 "tel='%s' and tw_tel='%s' and cur_no='%s' and ind='A'"
		, g_pub_tx.tel,teller,g_pub_tx.cur_no);
	vtcp_log("[%s][%d] Ray4 = [%f]",__FILE__,__LINE__,g_pub_tx.tx_amt1);
	if( ret==100 )
	{
	vtcp_log("[%s][%d] Ray6 = [%f]",__FILE__,__LINE__,g_pub_tx.tx_amt1);
		strcpy(g_com_tel_connect.ind,"A");					/*未取状态*/
		strcpy(g_com_tel_connect.tel,g_pub_tx.tel);			/*付出柜员*/
		strcpy(g_com_tel_connect.tw_tel,teller);			/*对方柜员*/
		strcpy(g_com_tel_connect.br_no,g_pub_tx.tx_br_no);	/*网点号*/
		strcpy(g_com_tel_connect.cur_no,g_pub_tx.cur_no);	/*币号*/
		g_com_tel_connect.amt=g_pub_tx.tx_amt1;				/*付出金额*/
		g_com_tel_connect.tx_date=g_pub_tx.tx_date;			/*交接日期*/		
		g_com_tel_connect.trace_no=g_pub_tx.trace_no;		/*流水号*/
		
		ret = Com_tel_connect_Ins( g_com_tel_connect,g_pub_tx.reply );
	vtcp_log("[%s][%d] Ray7 = [%f]",__FILE__,__LINE__,g_pub_tx.tx_amt1);
		if( ret )
		{
			sprintf(acErrMsg,"登记柜员交接登记薄异常出错!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"O236");	
			goto ErrExit;
		}
	}
	else if( ret==0 )
	{
		/**
		g_com_tel_connect.amt += g_pub_tx.tx_amt1;
	vtcp_log("[%s][%d] Ray5 = [%f]",__FILE__,__LINE__,g_pub_tx.tx_amt1);

		ret=sql_execute("update com_tel_connect set amt=%lf where \
			tel='%s' and tw_tel='%s' and cur_no='%s' and ind='A'", \
			g_com_tel_connect.amt,g_pub_tx.tel,teller,g_pub_tx.cur_no);
		if( ret )
		{
			sprintf(acErrMsg,"修改柜员交接登记薄失败!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"O235");			
			goto ErrExit;
		}
		**/
		sprintf(acErrMsg,"该柜员已有付出但对方柜员未收入的记录");
			WRITEMSG
			strcpy(g_pub_tx.reply,"O235");			
			goto ErrExit;
	}
	else 
	{
		sprintf(acErrMsg,"修改柜员交接登记薄异常出错!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O234");	
		goto ErrExit;
	}

	strcpy( g_pub_tx.ct_ind, "1" );    /*现转1-现2-转*/
	vtcp_log("[%s][%d] Ray8 = [%f]",__FILE__,__LINE__,g_pub_tx.tx_amt1);

	ret = pub_acct_cash( );
	vtcp_log("[%s][%d] Ray9 = [%f]",__FILE__,__LINE__,g_pub_tx.tx_amt1);
	if( ret )
	{
		sprintf(acErrMsg,"调用现金收付主控失败!");
		WRITEMSG
		goto ErrExit;
	}

	strcpy( g_pub_tx.cur_no, "01" );	/* 币种 */
	strcpy( g_pub_tx.ac_no, "10104" );	/* 在途人民币现金 */
	strcpy( g_pub_tx.ct_ind, "1" );	/* 现转 */
	strcpy( g_pub_tx.hst_ind, "1" );	/* 现转 */
	strcpy( g_pub_tx.brf, "现金调剂" );
	strcpy(g_pub_tx.add_ind,"1");				/* 减标志 */
	
	vtcp_log("[%s][%d] Ray10 = [%f]",__FILE__,__LINE__,g_pub_tx.tx_amt1);
	ret = pub_ins_trace_log();
	if ( ret )
	{
		sprintf( acErrMsg, "记流水日志错" );
		WRITEMSG
		goto ErrExit;
	}
	

	/*打印时间 add by andy*/
	pub_base_GetTime2( &hour, &min, &sec );
	memset( prtime, 0x00, sizeof( prtime ) );
	sprintf( prtime, "%02d:%02d:%02d", hour, min, sec );    /*打印时间*/
	set_zd_data( "0580", prtime );
vtcp_log( "zy   prtime[%s]",prtime );
	set_zd_data("1191",g_com_tel_connect.cur_no);
	set_zd_double("1194",g_pub_tx.tx_amt1);
	vtcp_log("[%s][%d] Ray11 = [%f]",__FILE__,__LINE__,g_pub_tx.tx_amt1);

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

