/*************************************************
* 文 件 名:  spB337.c
* 功能描述： 用于银行接到持票人开户行寄来的委托收款
*	     凭证及汇票后进行核销,一般是到期后核销，
*	     但是如果持票人在到期当天来兑付允许24403
*	     借方余额先核销汇票（允许此余额为零），
*	     日终处理汇票到期。
*
* 作    者:  jane
* 完成日期： 2003年5月30日
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
#include "mo_bank_po_c.h"
#include "mo_bank_acc_po_c.h"
#include "com_parm_c.h"
#include "com_item_c.h"

int spB337()
{
	struct	com_item_c		wd_com_item;	/* 承兑汇票登记簿 */
	struct	com_parm_c		wd_com_parm;	/* 承兑汇票登记簿 */
	struct	mo_bank_po_c		sMoBankPo;	/* 承兑汇票登记簿 */
	struct	mo_bank_acc_po_c	sMoBankAccPo;/* 银行承兑汇票基本信息登记薄 */
	struct	mdm_ac_rel_c 		sMdmAcRel;	/* 介质账号对照表 */
	struct	dd_mst_c 			sDdMst;		/* 活期主文件 */
	double	dPoAmt;							/* 票面金额 */
	char	cCtInd[2];						/* 现转标志 */
	char	cPoNo[17];						/* 承兑汇票号码有效位 */
	char	cPoNoHead[9];					/* 承兑汇票号码冠子头 */
	char    cPoNo2[17];					/* 承兑汇票号码冠字头+有效位 */
	char    cPoNo4[9];					/* 承兑汇票号码有效位中有效数字 */
	int		iPoNo;						/* 承兑汇票号码有效位中有效数字 */
	char	cAccHrt[6];						/* 同城科目控制字 */
	char	cAcNo[25];						/* 转账屏幕中所输的转存账号 */
	char	cTemp[30];						/* 为打印承兑协议编号加编号定义的临时变量 */
	int		ret;
	int     parm_ind = 0;     
	char	cPact_no[21];
	double	dSum_amt=0.00;

	memset( &sMoBankPo, 0x00, sizeof( struct mo_bank_po_c ) );
	memset( &sMoBankAccPo, 0x00, sizeof( struct mo_bank_acc_po_c ) );
	memset( &sMdmAcRel, 0x00, sizeof( struct mdm_ac_rel_c ) );
	memset( &sDdMst, 0x00, sizeof( struct dd_mst_c ) );
	memset( cCtInd, 0x00, sizeof( cCtInd ) );
	memset( cPoNo, 0x00, sizeof( cPoNo ) );
	memset( cPoNoHead, 0x00, sizeof( cPoNoHead ) );
	memset( cPoNo2, 0x00, sizeof( cPoNo2 ) );
	memset( cPoNo4, 0x00, sizeof( cPoNo4 ) );
	memset( cAccHrt, 0x00, sizeof( cAccHrt ) );
	memset( cAcNo, 0x00, sizeof( cAcNo ) );
	memset( cTemp, 0x00, sizeof( cTemp ) );
	memset( cPact_no, 0x00, sizeof( cPact_no ) );

	/* 数据初始化 */
	pub_base_sysinit();

	/* 从前台屏幕取值
	get_zd_data( "0580", sMoBankPo.po_no );	* 承兑汇票号码 *
	sMoBankPo.cash_date = g_pub_tx.tx_date;		* 核销日期 *
	get_zd_double( "100E", &dPoAmt );		* 票面金额 *
	get_zd_data( "0670", cCtInd );			* 现转标志 *
	get_zd_data( "1021", cAcNo );			* 转存账号 *
	**************************/
	get_zd_data("0370",cPact_no);		/*** 承兑协议号 **/
	/****
	* 检查所输承兑汇票号码是否带冠字头 *
	ret = pub_com_ChkNoteHead1( "111", sMoBankPo.po_no );
	if ( ret )
	{
		sprintf( acErrMsg, "检查承兑汇票号码出错![%d]", ret );
		WRITEMSG
		goto ErrExit;
	}
	* 取承兑汇票冠字头 *
	strncpy( cPoNoHead, sMoBankPo.po_no, 8 );
	cPoNoHead[8] = '\0';
	pub_base_strpack( cPoNoHead );

	 取承兑汇票有效位 
	strncpy( cPoNo, sMoBankPo.po_no+8, 8 );
	cPoNo[8] = '\0';
	set_zd_data( "0610", cPoNo );
	sprintf( acErrMsg, "zxxxyu    cPoNo[%s]", cPoNo );
	WRITEMSG

	 为给大额登记簿传值做准备，只传有效位中的有效数字 
	iPoNo = atol( cPoNo );				 将有效位转换成整型 
	sprintf( cPoNo4, "%d", iPoNo );			再将其转换成字符型 
	pub_base_strpack_all( cPoNo4 );
	set_zd_data( "0620", cPoNo4 );
	sprintf( acErrMsg, "zxxxyu    iPoNo[%d]  cPoNo4[%s]",iPoNo, cPoNo4 );
	WRITEMSG

	 承兑汇票号码冠字头+有效位，为打印承兑汇票作准备 
	sprintf( cPoNo2, " %s%s ", cPoNoHead, cPoNo );
	pub_base_strpack_all( cPoNo2 );
	set_zd_data( "0310", cPoNo2 );
	sprintf( acErrMsg, "zxxxyu    cPoNo2[%s]", cPoNo2 );
	WRITEMSG

	 修改承兑汇票登记薄，状态置为核销 */
	
	/* 根据承兑协议编号查找银行承兑汇票基本信息登记薄 */
	ret = Mo_bank_acc_po_Sel( g_pub_tx.reply, &sMoBankAccPo, "pact_no='%s'", cPact_no );
	if ( ret == 100 )
	{
		sprintf( acErrMsg, "此承兑协议编号不存在!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B199" );
		goto ErrExit;
	}else if( ret )
	{
		sprintf( acErrMsg, "执行Mo_bank_acc_po_Sel错![%d]", ret );
		WRITEMSG
		goto ErrExit;
	}
	ret = Mo_bank_po_Dec_Upd( g_pub_tx.reply, "pact_no='%s' and po_sts not in ('2','3')", sMoBankAccPo.pact_no );
	if ( ret )
	{
		sprintf( acErrMsg, "执行Mo_bank_po_Dec_Upd错[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}
	while(1)
	{
		memset(&sMoBankPo,0x00,sizeof(sMoBankPo));
		ret = Mo_bank_po_Fet_Upd( &sMoBankPo, g_pub_tx.reply );
		if ( ret == 100 )
		{
			break;
		}else if( ret )
		{
			sprintf( acErrMsg, "执行Mo_bank_po_Fet_Upd错![%d]", ret );
			WRITEMSG
			goto ErrExit;
		}
		if ( sMoBankPo.po_sts[0] != '4' )
		{
			sprintf( acErrMsg, "此承兑汇票未做托收登记!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B221" );
			goto ErrExit;
		}
		/* 为打印银行承兑汇票借方传票做准备 */
		pub_base_strpack( sMoBankPo.pact_no );	
		sprintf( cTemp, "%s--%d", sMoBankPo.pact_no, sMoBankPo.pact_no_num );
		set_zd_data( "0330", cTemp );	
	
		if ( g_pub_tx.tx_date < sMoBankAccPo.matr_date )
		{
			sprintf( acErrMsg, "只能在汇票到期日当天或到期之后进行核销!" );
			WRITEMSG
			sprintf( g_pub_tx.reply, "B227" );
			goto ErrExit;
		}
	
		/* 修改承兑汇票基本信息登记簿，状态为核销 */
		strcpy( sMoBankPo.po_sts, "2" );		/* 标志：2 核销 */
		sMoBankPo.cash_date=g_pub_tx.tx_date;
		
		ret = Mo_bank_po_Upd_Upd( sMoBankPo, g_pub_tx.reply );
		if ( ret )
		{
			sprintf( acErrMsg, "修改承兑汇票登记薄错[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}
		/* 为记会计流水时赋值用 */
		strcpy( g_pub_tx.note_type, "111" );				/* 银行承兑汇票 */
		strcpy( g_pub_tx.beg_note_no, sMoBankPo.po_no );	/* 银行承兑汇票号码 */
        	strcpy(g_pub_tx.ac_no,sMoBankPo.pact_no);
		/* 记流水日志 */
		/**----- xxx ---- 20030819 ---- 撤销必须要有协议编号 ----**/
		g_pub_tx.tx_amt1 = sMoBankPo.po_amt;    /* 承兑汇票票面金额 */
		g_pub_tx.svc_ind=0;
		strcpy( g_pub_tx.brf, "银行承兑汇票核销" );
		strcpy(g_pub_tx.no_show,"0");
		
		ret = pub_ins_trace_log();
		if ( ret )
		{
			sprintf( acErrMsg, "记流水日志错" );
			WRITEMSG
			goto ErrExit;
		}
		dSum_amt += sMoBankPo.po_amt;
	}
	Mo_bank_po_Clo_Upd();
	set_zd_double("100E",dSum_amt);
	if (sMoBankAccPo.acc_po_sts[0] == '9')   /** 外行汇票 **/
		parm_ind = 6;
	else
	{
		switch(sMoBankAccPo.acc_type[0])
		{  
		case '1':   /** 差额承兑 **/
			parm_ind = 1;
			break;
		case '2':   /** 质压存单 **/
			parm_ind = 2;
			break;
		case '3':   /** 100%保证金 **/
			parm_ind = 3;
			break;
		case '4':   /** 担保承兑 **/
			parm_ind = 4;
			break;
		case '5':   /** 大票换小票 **/
			parm_ind = 5;
			break;
		default:
			sprintf( acErrMsg, "[%s][%d]汇票类型错误![%s]",__FILE__,__LINE__,sMoBankAccPo.acc_type);
			WRITEMSG
			strcpy( g_pub_tx.reply, "T068" );
			goto ErrExit;

		}
	}
	/** 从参数表com_parm取得不同类型承兑汇票的科目 **/
	ret = Com_parm_Sel(g_pub_tx.reply,&wd_com_parm,"parm_code='CDHPR' and parm_seqn=%d",parm_ind);
	if (ret)
	{
		sprintf( acErrMsg, "[%s][%d]查询参数表错误![%d]",__FILE__,__LINE__, ret );
		WRITEMSG
		goto ErrExit;
	}
	pub_base_strpack(wd_com_parm.val);
	/** 承兑汇票的科目 **/
	ret = Com_item_Sel(g_pub_tx.reply,&wd_com_item,"acc_no='%s'",wd_com_parm.val);
	if (ret)
	{
		sprintf( acErrMsg, "[%s][%d]查询科目错误![%d][%s]",__FILE__,__LINE__, ret,wd_com_parm.val );
		WRITEMSG
		goto ErrExit;
	}
	
	sprintf( acErrMsg, "承兑科目为:[%s][%s]",wd_com_parm.val,wd_com_item.acc_hrt );
	WRITEMSG

	set_zd_data( "0640", wd_com_item.acc_hrt );     /* 承兑汇票记帐科目 */
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	if(memcmp(g_pub_tx.reply,"0000",4)==0)
		memcpy(g_pub_tx.reply,"H034",4);
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
