/*************************************************
* 文 件 名:  sp9859.c
* 功能描述： 计算转贴现利息。
*
* 作    者:  cfq
* 完成日期： 2015年11月26日
*
* 修改记录： 
* 日   期:
* 修 改 人:
* 修改内容:
* insert into TX_DEF (TX_CODE, TX_NAME, BIT_MAP, LOG_IND, TX_TYPE, WRK_STS) values ('9859', '计算转贴现利息', '10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000010000000000000000000000000000', '1', '3', '2');
* insert into TX_FLOW_DEF (TX_CODE, FLOW_CODE, SUB_TX_CODE, SUB_TX_RSLT) values ('9859', 0, '9859', '#$');
* insert into TX_SUB_DEF (SUB_TX_CODE, SUB_TX_NAME, TX_FUNC, ACCT_IND, CHNL_IND) values ('9859', '计算转贴现利息', 'sp9859', '0', '0');
*************************************************/

#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "mo_discnt_c.h"

int sp9859()
{
	struct	mo_discnt_c	sMoDiscnt1, sMoDiscnt2,sMoDiscnt3;	/* 贴现登记薄 */
	long	lPnDays;						/* 贴现天数 */
	int		ret,num;
	char	cOperCode[2];					/* 操作码 */
	char	cPn_knd[2];						/* 电票标志 */
 	char    cKinbr[BRNO_LEN+1];
  	char    cZsq[2];        			 /* 追索权 0：无追索权 1：有追索权*/

	/* 数据初始化 */
	memset( &sMoDiscnt1, 0x00, sizeof( struct mo_discnt_c ) );
	memset( &sMoDiscnt2, 0x00, sizeof( struct mo_discnt_c ) );
	memset( &sMoDiscnt3, 0x00, sizeof( struct mo_discnt_c ) );
	memset( cOperCode, 0x00, sizeof( cOperCode ) );
	memset( cPn_knd, 0x00, sizeof( cPn_knd ) );
	memset(cKinbr , 0 , sizeof(cKinbr));
  	memset(cZsq     , 0x00, sizeof(cZsq));
  
  	pub_base_sysinit();
	
	/* 从前台屏幕取值 */
	get_zd_data( "0690", cOperCode );				/* 操作码 */
	strcpy( sMoDiscnt1.br_no, g_pub_tx.tx_br_no );	/* 机构编码 */
	get_zd_double( "0840", &sMoDiscnt1.tpn_rate );	/* 转贴现率 */
	sMoDiscnt1.tpn_date = g_pub_tx.tx_date;			/* 转贴现日 */
	sMoDiscnt1.wrk_date = g_pub_tx.tx_date;			/* 转贴现信息录入时交易日期 */
	sMoDiscnt1.trace_no = g_pub_tx.trace_no;		/* 录入流水号 */
	get_zd_data( "0900", sMoDiscnt2.note_type );	/* 票据类型 */
	get_zd_data( "0600", sMoDiscnt2.pnote_no );		/* 票据号 */
	get_zd_data( "0680", sMoDiscnt1.local_ind );	/* 异地同城标志 */
	get_zd_data( "0250", sMoDiscnt1.tpn_name );		/* 对方行名称 */
	get_zd_data( "0980", cPn_knd);	/* 电票标志 */
    get_zd_data("0380", sMoDiscnt1.tpact_no );
 	get_zd_data( "0250", sMoDiscnt1.tpn_name );		/* 对方行名称 */
    get_zd_data("0720", cZsq); 
 	get_zd_data("0910" , cKinbr );
	if(cPn_knd[0]=='E')
	get_zd_data("1032",sMoDiscnt2.pnote_no);

    	ret = Mo_discnt_Sel( g_pub_tx.reply, &sMoDiscnt2, "note_type='%s' and pnote_no='%s' \
						and pn_type='4' ", sMoDiscnt2.note_type, sMoDiscnt2.pnote_no );
		if( ret == 0 )
		{
			sprintf( acErrMsg, "此票据已做转贴现转出!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B248" );
			goto ErrExit;
   		}
		else if( ret != 100 )
   		{
   			sprintf( acErrMsg, "执行Mo_discnt_Sel错![%d]", ret );
			WRITEMSG
			goto ErrExit;
   		}
		ret = Mo_discnt_Sel( g_pub_tx.reply, &sMoDiscnt3, "note_type='%s' and pnote_no='%s' \
							and pn_type !='4' ", sMoDiscnt2.note_type, sMoDiscnt2.pnote_no );
		if( ret )
		{
			sprintf( acErrMsg, "执行Mo_discnt_Sel3错!" );
			WRITEMSG
			goto ErrExit;
		}
	
		/* 计算贴现利息：其中，贴现天数＝ 票据到期日 -转贴现日期+ 在途天数,
					贴现利息 ＝ 汇票金额 * 贴现天数 * （年转贴现率/360天/100）） */
		ret = pub_base_CalDays( sMoDiscnt1.tpn_date, sMoDiscnt3.not_matr_date, "0", &lPnDays );
		if( ret )
		{
			sprintf( acErrMsg, "计算贴现天数错!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B127" );
			goto ErrExit;
		}

		sMoDiscnt1.tpn_int = sMoDiscnt3.par_amt * (lPnDays + sMoDiscnt3.way_days) * (sMoDiscnt1.tpn_rate/360/100);
		set_zd_double( "1001", sMoDiscnt1.tpn_int );		/* 转贴现利息 */
		vtcp_log("转贴现利息:[%.2lf]",sMoDiscnt1.tpn_int);



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