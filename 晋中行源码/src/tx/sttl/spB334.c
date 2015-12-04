/*************************************************
* 文 件 名:  spB334.c
* 功能描述： 完成银行承兑汇票退票功能
*
* 作    者:  jane
* 完成日期： 2003年5月26日
*
* 修改记录： 
*   日   期: 2010-6-24 11:08:19
*   修 改 人:xyy
*   修改内容:
1.退票时必须协议状态为正常时才可以(即全部签完)；
新编号为000才可以去退票，且退票金额对应保证金返回对公户，去掉更新协议状态为3协议修改.
2.换票时，更改原票据状态，处理协议状态的情况如下：
a.大票换小票，若还之前协议已经为正常了，则修改协议状态。
b.小票换大票，换完票以后的金额刚好把所有的票签完，则把状态修改为正常。
*************************************************/

#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "mo_bank_acc_po_c.h"
#include "mo_bank_po_c.h"
#include "com_parm_c.h"
#include "mo_bail_rate_c.h"
#include "mo_bank_po_fee_c.h"

int spB334()
{
	int     ret;
	long     day=0;
	long     HQday=0;
	int     iPactNoNum1;                /* 原编号 */
	int     iPactNoNum2;                /* 新编号 */

	char    cCg_flag[2];                /* 状态变更标志  */
	char    cAccPoNo1[17];              /* 原承兑汇票号码屏幕取值 */
	char    cPoNo1[9];                  /* 原承兑汇票号码有效位 */
	char    cPoNoHead1[9];              /* 原承兑汇票号码冠字头 */
	char    cPoNo11[17];                /* 原承兑汇票号码冠字头+有效位 */
	char    cAccPoNo2[17];              /* 新承兑汇票号码屏幕取值 */
	char    cPoNo2[9];                  /* 新承兑汇票号码有效位 */
	char    cPoNoHead2[9];              /* 新承兑汇票号码冠字头 */
	char    cPoNo22[17];                /* 新承兑汇票号码冠字头+有效位 */
	char    intst_flag[2];
	char    cPayeeName[51];             /* 收款人全称 */
	char    cPayeeAcNo[25];             /* 收款人账号 */
	char    cPayeeBrName[51];           /* 收款人开户行行名 */

	double    dPoAmt1 = 0.00;           /* 原票面金额 */
	double    dPoAmt2 = 0.00;           /* 新票面金额 */
	double    dZzAmt  = 0.00;           /* 退票时应该从保证金户扣划到结算户的金额 */
	double    dCeAmt  = 0.00;           /* 差额承兑汇票 */
	double    HQ_rate=0.00;
	double    HQdealval=0.00;

	struct    com_parm_c            sComParm;       /* 公共参数表 */
	struct    mo_bank_po_c          sMoBankPo;      /* 承兑汇票登记簿 */
	struct    mo_bail_rate_c        sMoBailRate;    /* 保证金利率登记薄 */
	struct    mo_bail_rate_c        sMoBailRate1;   /* 保证金利率登记薄 */
	struct    mo_bank_acc_po_c      sMoBankAccPo;   /* 承兑汇票基本信息登记簿 */
	struct    mo_bank_po_fee_c      sMobankpofee;   /*承兑汇票收费定义表*/

	memset( &sMoBankAccPo, 0x00, sizeof( struct mo_bank_acc_po_c ) );
	memset( &sMobankpofee, 0x00, sizeof( struct mo_bank_po_fee_c ) );
	memset( &sMoBankPo, 0x00, sizeof( struct mo_bank_po_c ) );
	memset( &sComParm, 0x00, sizeof( struct com_parm_c ) );
	memset( &sMoBailRate, 0x00, sizeof( struct mo_bail_rate_c ) );
	memset( &sMoBailRate1, 0x00, sizeof( struct mo_bail_rate_c ) );
	memset( &g_pub_intst,0x00,sizeof(struct S_pub_intst) );
	memset( cPayeeName, 0x00, sizeof( cPayeeName ) );
	memset( cPayeeAcNo, 0x00, sizeof( cPayeeAcNo ) );
	memset( cPayeeBrName, 0x00, sizeof( cPayeeBrName ) );
	memset( cAccPoNo1, 0x00, sizeof( cAccPoNo1 ) );
	memset( cAccPoNo2, 0x00, sizeof( cAccPoNo2 ) );
	memset( cPoNo1, 0x00, sizeof( cPoNo1 ) );
	memset( cPoNoHead1, 0x00, sizeof( cPoNoHead1 ) );
	memset( cPoNo11, 0x00, sizeof( cPoNo11 ) );
	memset( cPoNo2, 0x00, sizeof( cPoNo2 ) );
	memset( cPoNoHead2, 0x00, sizeof( cPoNoHead2 ) );
	memset( cPoNo22, 0x00, sizeof( cPoNo22 ) );
	memset( cCg_flag , 0x00 , sizeof(cCg_flag));
	memset( intst_flag, 0x00, sizeof( intst_flag ) );

	/* 数据初始化 */
	pub_base_sysinit();

	/* 从前台屏幕取值 */
	get_zd_data( "0330", sMoBankAccPo.pact_no );/* 承兑协议编号 */
	get_zd_int( "0500", &iPactNoNum1 );            /* 原编号 */
	get_zd_int( "0510", &iPactNoNum2 );            /* 新编号 */
	get_zd_data( "0580", cAccPoNo1 );            /* 原承兑汇票号码 */
	get_zd_double( "100E", &dPoAmt1 );            /* 原票面金额 */
	get_zd_data( "0590", cAccPoNo2 );            /* 新承兑汇票号码 */
	get_zd_double( "1004", &dPoAmt2 );            /* 新票面金额 */
	get_zd_data( "0270", cPayeeName );            /* 收款人全称 */
	get_zd_data( "0830", cPayeeAcNo );            /* 收款人账号 */
	vtcp_log( "[%s][%d]   num1[%d] num2[%d]",__FILE__,__LINE__,iPactNoNum1, iPactNoNum2 );
	get_zd_data( "0810", cPayeeBrName );        /* 收款人开户行行名 */
	vtcp_log( "[%s][%d]   num1[%d] num2[%d]",__FILE__,__LINE__, iPactNoNum1, iPactNoNum2 );

	/* 检查所输承兑汇票号码是否带冠字头 */
	ret = pub_com_ChkNoteHead1( "111", cAccPoNo1 );
	if ( ret )
	{
		sprintf( acErrMsg, "检查承兑汇票号码出错![%d]", ret );
		WRITEMSG
		goto ErrExit;
	}
	/* 根据承兑协议编号查找银行承兑汇票基本信息登记薄 */
	ret = Mo_bank_acc_po_Sel( g_pub_tx.reply, &sMoBankAccPo, "pact_no='%s'", sMoBankAccPo.pact_no );
	if ( ret == 100 )
	{
		sprintf( acErrMsg, "无此承兑协议编号!" );
		WRITEMSG
		sprintf( g_pub_tx.reply, "B187" );
		goto ErrExit;
	}
	else if ( ret )
	{
		sprintf( acErrMsg, "执行Mo_bank_acc_po_Sel错![%d]", ret );
		WRITEMSG
		goto ErrExit;
	}
	if ( sMoBankAccPo.acc_po_sts[0] == '0' )
	{
		sprintf( acErrMsg, "此承兑协议为录入状态!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B213" );
		goto ErrExit;
	}
	/*modified by liuxuan 2010-4-16 避免签发多笔如果有一笔签错必须全部签完才可退票的情况
	else if( sMoBankAccPo.acc_po_sts[0] == '1' )
	{
	sprintf( acErrMsg, "此承兑协议未做签发或未全部签发!" );
	WRITEMSG
	strcpy( g_pub_tx.reply, "B211" );
	goto ErrExit;
	}
	*/
	vtcp_log("[%s][%d] dPoAmt1=[%f] dPoAmt2=[%f]",__FILE__,__LINE__,dPoAmt1,dPoAmt2);

	dCeAmt = dPoAmt1 - dPoAmt2;

	vtcp_log("[%s][%d] sMoBankAccPo.bail_amt=[%f]",__FILE__,__LINE__,sMoBankAccPo.bail_amt);
	/*************modif by xyy 2010-6-24 12:07:39 *************/
	if (pub_base_CompDblVal(dCeAmt,0.00) < 0 )
	{
		vtcp_log("[%s][%d] 小票[%f]换大票[%f]情况=[%f]",__FILE__,__LINE__,dPoAmt1,dPoAmt2,dCeAmt);
		/***********这里需要检查是否还有足够的保证金可以去换大票*******
		1.计算实际可签发金额  
		2.计算已签发金额      
		***************************************************************/
		double dYqBzamt = 0.00;
		double dSign_amt= 0.00;

		if (pub_base_CompDblVal(sMoBankAccPo.bail_amt , 0.00) > 0)
			dSign_amt = sMoBankAccPo.bail_amt/sMoBankAccPo.bail_amt_ratio*100;
		else
			dSign_amt = sMoBankAccPo.sign_amt;

		/* 判断银行承兑汇票登记簿中对于同一个协议编号的汇票金额之和与录入时的票面总金额是否相等 */
		ret = sql_sum_double( "mo_bank_po", "po_amt", &dYqBzamt, "pact_no='%s' and po_sts='1'", sMoBankAccPo.pact_no );
		if ( ret )
		{
			sprintf( acErrMsg, "执行sql_sum_double错[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}
		vtcp_log("[%s][%d] 实际可签发金额[%f] 已签金额[%f]",__FILE__,__LINE__,dSign_amt,dYqBzamt);

		if (pub_base_CompDblVal(dYqBzamt , dSign_amt) > 0)
		{
			sprintf( acErrMsg, "已签发金额大于可签发金额，请联系科技科!!" );
			WRITEMSG
			goto ErrExit;
		}
		/*****本次想要增加的部分和还可以签发部分进行比较******/
		if (pub_base_CompDblVal(dPoAmt2 - dPoAmt1, dSign_amt - dYqBzamt) > 0)
		{
			sprintf( acErrMsg, "改签票面金额过大,保证金不足!!" );
			WRITEMSG
			goto ErrExit;
		}
		if (pub_base_CompDblVal(dPoAmt2 - dPoAmt1, dSign_amt - dYqBzamt) == 0)
		{
			vtcp_log("[%s][%d] 金额相等,那么，签发完毕!",__FILE__,__LINE__);
			if ( sMoBankAccPo.acc_po_sts[0] == '1' )
			{
				cCg_flag[0] = 'X';
			}
			else
			{
				sprintf( acErrMsg, "协议状态[%s]不正确!",sMoBankAccPo.acc_po_sts);
				WRITEMSG
				goto ErrExit;
			}
		}
	}
	else/***等额换票或者退票***/
	{
		vtcp_log("[%s][%d] 大票[%f]换小票[%f]情况=[%f]",__FILE__,__LINE__,dPoAmt1,dPoAmt2,dCeAmt);
		if (iPactNoNum2 == 0 )
		{
			if (pub_base_CompDblVal(dPoAmt2,0.00) != 0)
			{
				sprintf( acErrMsg, "新票面金额不为0=[%f]", dPoAmt2 );
				WRITEMSG
				goto ErrExit;
			}
			vtcp_log("[%s][%d] 退票的情况,新编号[%d]",__FILE__,__LINE__,iPactNoNum2);
			/*****检查协议是否已经为正常，非正常状态不允许退票*****/
			if ( sMoBankAccPo.acc_po_sts[0] != '2' )
			{
				/*add by gong 20110905 晋中商行信贷，校验陈对汇票收费定义表，如果没有信贷发起的，则认为已经全部撤销或完成
				ret = Mo_bank_po_fee_Sel(g_pub_tx.reply ," pact_no='%s'and sts='X' ",sMoBankAccPo.pact_no);
				if(ret)
				{
				ret = Mo_bank_acc_po_Dec_Upd( g_pub_tx.reply , "pact_no='%s'" , sMoBankAccPo.pact_no );
				if( ret )
				{
				sprintf( acErrMsg,"Prepare Mo_bank_acc_po_Dec_Upd 游标出错[%d]",ret);
				WRITEMSG
				goto ErrExit;
				}
				ret = Mo_bank_acc_po_Fet_Upd( &sMoBankAccPo , g_pub_tx.reply );
				if( ret==100 )
				{
				sprintf( acErrMsg,"无此审批记录![%d]",ret);
				WRITEMSG
				strcpy( g_pub_tx.reply ,"L175" );
				goto ErrExit ;
				}else if( ret ){
				sprintf( acErrMsg,"fetch Mo_bank_acc_po_Fet_Upd error code = [%d]",ret);
				WRITEMSG
				strcpy( g_pub_tx.reply ,"D103" );
				goto ErrExit ;
				}
				strcpy( sMoBankAccPo.acc_po_sts, "2" );
				ret = Mo_bank_acc_po_Upd_Upd( sMoBankAccPo, g_pub_tx.reply  );
				if ( ret )
				{
				sprintf( acErrMsg,"Mo_bank_acc_po_Upd_Upd 出错[%d]",ret);
				WRITEMSG
				return 1;
				}
				Mo_bank_acc_po_Clo_Upd( );
				sprintf(acErrMsg,"状态更新完毕！！ PASS!");
				WRITEMSG	
				}else{
				sprintf( acErrMsg, "协议状态不正常，不允许退票!" );
				WRITEMSG
				strcpy( g_pub_tx.reply, "B213" );
				goto ErrExit;
				}
				end by gong 20110905 晋中商行信贷*/
				sprintf( acErrMsg, "协议状态不正常，不允许退票!" );
				WRITEMSG
				strcpy( g_pub_tx.reply, "B213" );
				goto ErrExit;
			}
			dZzAmt =  dPoAmt1  * sMoBankAccPo.bail_amt_ratio /100 ;
			set_zd_double( "0410", dZzAmt );
			set_zd_double( "0420", dZzAmt );
		}
		else
		{
			vtcp_log("[%s][%d] 等额换票的情况,新编号[%d]",__FILE__,__LINE__,iPactNoNum2);
		}
	}
	/*******************end by xyy 2010-6-24 12:07:52*****************/
	vtcp_log("[%s][%d] dCeAmt=[%lf]",__FILE__,__LINE__,dCeAmt );

	vtcp_log( "[%s][%d]   num1[%d] num2[%d]",__FILE__,__LINE__,iPactNoNum1, iPactNoNum2 );
	/* 修改承兑汇票登记簿 */
	ret = Mo_bank_po_Dec_Upd( g_pub_tx.reply, "pact_no='%s' and pact_no_num=%d", \
		sMoBankAccPo.pact_no, iPactNoNum1 );
	if ( ret )
	{
		sprintf( acErrMsg, "执行Mo_bank_po_Dec_Upd错[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}
	ret = Mo_bank_po_Fet_Upd( &sMoBankPo, g_pub_tx.reply );
	if ( ret == 100 )
	{
		sprintf( acErrMsg, "此承兑协议编号不存在!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B199" );
		goto ErrExit;
	}
	else if ( ret )
	{
		sprintf( acErrMsg, "执行Mo_bank_po_Fet_Upd错![%d]", ret );
		WRITEMSG
		goto ErrExit;
	}
	sprintf( acErrMsg, "所输原汇票金额![%lf]po_amt[%lf]",dPoAmt1,sMoBankPo.po_amt);
	WRITEMSG

	pub_base_strpack( sMoBankPo.po_no );

	/* 取承兑汇票冠字头 */
	strncpy( cPoNoHead1, cAccPoNo1, 8 );
	cPoNoHead1[8]='\0';
	pub_base_strpack( cPoNoHead1 );

	/* 取承兑汇票有效位 */
	strncpy( cPoNo1, cAccPoNo1+8, 8 );
	cPoNo1[8] = '\0';
	sprintf( acErrMsg, "zxxxyu    cPoNo1[%s]", cPoNo1 );
	WRITEMSG

	sprintf( acErrMsg, "输原汇票金额![%lf]po_amt[%lf]",dPoAmt1,sMoBankPo.po_amt);
	WRITEMSG
	/* 承兑汇票号码冠字头+有效位 */
	sprintf( cPoNo11, " %s%s ", cPoNoHead1, cPoNo1 );
	sprintf( acErrMsg, "zxxxyu    cPoNo11[%s]", cPoNo11 );
	WRITEMSG
	pub_base_strpack_all( cPoNo11 );

	sprintf( acErrMsg, "所输原汇票金额![%lf]po_amt[%lf]",dPoAmt1,sMoBankPo.po_amt);
	WRITEMSG
	if ( strcmp( cPoNo11, sMoBankPo.po_no ) != 0 )
	{
		sprintf( acErrMsg, "原承兑汇票号码与原编号不符!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B204" );
		goto ErrExit;
	}
	ret = pub_base_CompDblVal( dPoAmt1, sMoBankPo.po_amt );
	if ( ret !=0 )
	{
		sprintf( acErrMsg, "所输原汇票金额有误![%lf]po_amt[%lf]",dPoAmt1,sMoBankPo.po_amt);
		WRITEMSG
		strcpy( g_pub_tx.reply, "B207" );
		goto ErrExit;
	}
	if ( sMoBankPo.po_sts[0] == '3' )
	{
		sprintf( acErrMsg, "此承兑汇票已做退票!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B212" );
		goto ErrExit;
	}
	else if ( sMoBankPo.po_sts[0] == '2' )
	{
		sprintf( acErrMsg, "此承兑汇票已核销，不允许退票!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B228" );
		goto ErrExit;
	}
	else if ( sMoBankPo.po_sts[0] == '4' )
	{
		sprintf( acErrMsg, "此承兑汇票已托收，不允许退票!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B229" );
		goto ErrExit;
	}
	strcpy( sMoBankPo.po_sts, "3" );    /* 承兑汇票状态: 3 退票 */
	sMoBankPo.cash_date = g_pub_tx.tx_date;/* 将核销日期作为退票日期 modified by liuxuan 2006-12-19*/

	ret = Mo_bank_po_Upd_Upd( sMoBankPo, g_pub_tx.reply );
	if ( ret )
	{
		sprintf( acErrMsg, "修改承兑汇票基本信息登记薄错[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}
	Mo_bank_po_Clo_Upd();
	vtcp_log( "[%s][%d]   num1[%d] num2[%d]",__FILE__,__LINE__,iPactNoNum1, iPactNoNum2 );

	if ( strcmp(cAccPoNo2, "") && iPactNoNum2 == 0 )
	{
		sprintf( acErrMsg, "新编号不允许与原编号重复!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B205" );
		goto ErrExit;
	}
	vtcp_log( "[%s][%d]   num1[%d] num2[%d]",__FILE__,__LINE__,iPactNoNum1, iPactNoNum2 );
	/* 如果新开汇票，则新增一条记录到承兑汇票登记簿中 */
	if ( iPactNoNum2 != 0 )
	{
		if ( iPactNoNum2 == iPactNoNum1 )
		{
			sprintf( acErrMsg, "新编号不允许与原编号重复!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B205" );
			goto ErrExit;
		}
		vtcp_log( "zyzyzy333   num1[%d] num2[%d]", iPactNoNum1, iPactNoNum2 );
		/* 根据承兑汇票编号查找银行承兑汇票登记薄 */
		ret = Mo_bank_po_Sel( g_pub_tx.reply, &sMoBankPo, "pact_no='%s' and pact_no_num=%d", \
			sMoBankAccPo.pact_no, iPactNoNum2 );
		if ( ret == 0 )
		{
			sprintf( acErrMsg, "编号已存在!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B208" );
			goto ErrExit;
		}
		else if ( ret != 100 )
		{
			TRACE
				sprintf( acErrMsg, "执行Mo_bank_po_Sel错![%d]", ret );
			WRITEMSG
			goto ErrExit;
		}
		if ( strcmp( cAccPoNo2, "" ) == 0 )
		{
			sprintf( acErrMsg, "如果新开汇票，请输入新承兑汇票号码!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B232" );
			goto ErrExit;
		}
		/* 检查所输承兑汇票号码是否带冠字头 */
		ret = pub_com_ChkNoteHead( "111", cAccPoNo2, g_pub_tx.tel);
		if ( ret )
		{
			sprintf( acErrMsg, "检查承兑汇票号码出错![%d]", ret );
			WRITEMSG
			goto ErrExit;
		}
		/* 查询承兑汇票是否已被领入 */
		ret = Com_parm_Sel( g_pub_tx.reply, &sComParm, "parm_code='CDHP'" );
		if ( ret )
		{
			sprintf( acErrMsg, "执行Com_parm_Sel错误![%d]", ret );
			WRITEMSG
			goto ErrExit;
		}
		pub_base_strpack( sComParm.val );

		vtcp_log("pono[%s]notetype[%s]brno[%s]tel[%s]",sMoBankPo.po_no, sComParm.val,g_pub_tx.tx_br_no, g_pub_tx.tel);

		/* 承兑汇票销号处理 */
		/**-- xxx --- @20050824@ ----**/
		g_pub_tx.svc_ind=7021;
		ret=pub_com_NoteUse( 0, 0, sComParm.val, cAccPoNo2, cAccPoNo2, g_pub_tx.tel );
		if ( ret) 
		{
			sprintf( acErrMsg, "银行承兑汇票销号出错!" );
			WRITEMSG
			goto ErrExit;
		}
		if ( dPoAmt2 < 0.005 )
		{
			sprintf( acErrMsg, "如果新开汇票，请输入新承兑汇票金额!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B233" );
			goto ErrExit;
		}
		/* 取承兑汇票冠字头 */
		strncpy( cPoNoHead2, cAccPoNo2, 8 );
		cPoNoHead2[8]='\0';
		pub_base_strpack( cPoNoHead2);

		/* 取承兑汇票有效位 */
		strncpy( cPoNo2, cAccPoNo2+8, 8 );
		cPoNo2[8] = '\0';
		sprintf( acErrMsg, "zxxxyu    cPoNo2[%s]", cPoNo2 );
		WRITEMSG

		/* 承兑汇票号码冠字头+有效位 */
		sprintf( cPoNo22, " %s%s ", cPoNoHead2, cPoNo2 );
		sprintf( acErrMsg, "zxxxyu    cPoNo22[%s]", cPoNo22 );
		WRITEMSG
		pub_base_strpack_all( cPoNo22 );

		/* 根据新承兑汇票号码查找银行承兑汇票登记薄 */
		ret = Mo_bank_po_Sel( g_pub_tx.reply, &sMoBankPo, "po_no='%s'", cPoNo22 );
		if ( ret == 0 )
		{
			sprintf( acErrMsg, "新承兑汇票号码已存在!" );
			WRITEMSG
			sprintf( g_pub_tx.reply, "B206" );
			goto ErrExit;
		}
		else if ( ret != 100 )
		{
			sprintf( acErrMsg, "执行Mo_bank_po_Sel错![%d]", ret );
			WRITEMSG
			goto ErrExit;
		}
		strcpy( sMoBankPo.sign_br_no, g_pub_tx.tx_br_no );    /* 签发行行号 */
		sMoBankPo.sign_date = g_pub_tx.tx_date;               /* 签发日期 */
		strcpy( sMoBankPo.pact_no, sMoBankAccPo.pact_no );    /* 承兑汇票协议编号 */
		sMoBankPo.pact_no_num = iPactNoNum2;                  /* 编号 */
		strcpy( sMoBankPo.po_no, cPoNo22 );                   /* 汇票号码 */
		sMoBankPo.po_amt = dPoAmt2;                           /* 票面金额 */
		strcpy( sMoBankPo.po_sts, "1" );                      /* 承兑汇票状态: 1 签发 */
		strcpy( sMoBankPo.po_ind, "1" );                      /* 承兑汇票标志: 1 正常 */

		ret = Mo_bank_po_Ins( sMoBankPo, g_pub_tx.reply );
		if ( ret )
		{
			sprintf( acErrMsg, "执行Mo_bank_po_Ins错[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}
	}
	/* 如果收款人信息有所改动，则根据承兑协议编号修改承兑汇票基本信息登记薄 */
	sprintf( acErrMsg, "cPayeeName=[%s],payee_name=[%s],cPayeeAcNo=[%s],payee_ac_no=[%s],\
		cPayeeBrName=[%s],payee_br_name=[%s]", cPayeeName, sMoBankAccPo.payee_name, \
		cPayeeAcNo, sMoBankAccPo.payee_ac_no, cPayeeBrName, sMoBankAccPo.payee_br_name );
	WRITEMSG

	pub_base_strpack( sMoBankAccPo.payee_name );
	pub_base_strpack( sMoBankAccPo.payee_ac_no );
	pub_base_strpack( sMoBankAccPo.payee_br_name );

	sprintf( acErrMsg, "cPayeeName=[%s],payee_name=[%s],cPayeeAcNo=[%s],payee_ac_no=[%s],\
		cPayeeBrName=[%s],payee_br_name=[%s]", cPayeeName, sMoBankAccPo.payee_name, \
		cPayeeAcNo, sMoBankAccPo.payee_ac_no, cPayeeBrName, sMoBankAccPo.payee_br_name );
	WRITEMSG

	if ( strcmp( cPayeeName, sMoBankAccPo.payee_name ) != 0 \
		|| strcmp( cPayeeAcNo, sMoBankAccPo.payee_ac_no ) != 0 \
		|| strcmp( cPayeeBrName, sMoBankAccPo.payee_br_name ) != 0 \
		|| pub_base_CompDblVal(dCeAmt,0.00) != 0 )
	{
		ret = Mo_bank_acc_po_Dec_Upd( g_pub_tx.reply, "pact_no='%s'",sMoBankAccPo.pact_no );
		if ( ret )
		{
			sprintf( acErrMsg, "执行Mo_bank_acc_po_Dec_Upd错[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}
		ret = Mo_bank_acc_po_Fet_Upd( &sMoBankAccPo, g_pub_tx.reply );
		if ( ret == 100 )
		{
			sprintf( acErrMsg, "此承兑协议编号不存在!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B199" );
			goto ErrExit;
		}
		else if ( ret )
		{
			sprintf( acErrMsg, "执行Mo_bank_acc_po_Fet_Upd错![%d]", ret );
			WRITEMSG
			goto ErrExit;
		}
		/* 修改承兑汇票基本信息登记簿 */
		if ( strcmp( cPayeeName, sMoBankAccPo.payee_name ) != 0 )
			strcpy( sMoBankAccPo.payee_name, cPayeeName );
		if ( strcmp( cPayeeAcNo, sMoBankAccPo.payee_ac_no ) != 0 )
			strcpy( sMoBankAccPo.payee_ac_no, cPayeeAcNo );
		if ( strcmp( cPayeeBrName, sMoBankAccPo.payee_br_name ) != 0 )
			strcpy( sMoBankAccPo.payee_br_name, cPayeeBrName );
		/**--- xxx ---- 20051105 ------ 修改保证金金额 ---**/

		vtcp_log("[%s][%d] sMoBankAccPo.bail_amt=[%f]",__FILE__,__LINE__,sMoBankAccPo.bail_amt);

		/*****************************modify by xyy 2010-6-24 15:29:12****************************/
		if ( pub_base_CompDblVal(dCeAmt,0.00) < 0 )/**小票换大票**/
		{
			vtcp_log("[%s][%d] dCeAmt=[%f] 小票换大票,原状态[%c]!!",__FILE__,__LINE__,dCeAmt,sMoBankAccPo.acc_po_sts[0]);
			if (cCg_flag[0] == 'X' )
			{
				strcpy( sMoBankAccPo.acc_po_sts, "2" );/***小票换大票,由审核变正常***/
				vtcp_log("[%s][%d]  小票换大票,由审核变正常[%s]",__FILE__,__LINE__,sMoBankAccPo.acc_po_sts);
			}
		}
		else
		{
			vtcp_log("[%s][%d] 需要判断是退票还是换票,新编号[%d]!",__FILE__,__LINE__,iPactNoNum2);
			if (iPactNoNum2 == 0 )/**退票**/
			{
				if (pub_base_CompDblVal(dPoAmt2,0.00) != 0)
				{
					sprintf( acErrMsg, "新票面金额不为0=[%f]", dPoAmt2 );
					WRITEMSG
					goto ErrExit;
				}
				double dPoTmp=0.00;
				vtcp_log("[%s][%d] 退票要更新原保证金!!!",__FILE__,__LINE__,iPactNoNum2);

				if (pub_base_CompDblVal(sMoBankAccPo.bail_amt , 0.00) > 0)
				{
					dPoTmp = pub_base_PubDround_1(dPoAmt1  * sMoBankAccPo.bail_amt_ratio /100) ;
					vtcp_log("[%s][%d] 原票面[%f] 比例[%f] 差额[%f]",__FILE__,__LINE__,dPoAmt1,sMoBankAccPo.bail_amt_ratio,dPoTmp);
					sMoBankAccPo.bail_amt=sMoBankAccPo.bail_amt - dPoTmp;
				}
			}
			else/***大票换小票***/
			{
				vtcp_log("[%s][%d] dCeAmt=[%f] 大票换小票或等额换票,状态[%c]!!",__FILE__,__LINE__,dCeAmt,sMoBankAccPo.acc_po_sts[0]);

				if (sMoBankAccPo.acc_po_sts[0] == '2' && pub_base_CompDblVal(dCeAmt,0.00) > 0)
				{
					vtcp_log("[%s][%d] 协议的状态=[%s]",__FILE__,__LINE__,sMoBankAccPo.acc_po_sts);
					strcpy( sMoBankAccPo.acc_po_sts, "1" );/***大票还小票，有正常变为审核***/
				}
			}
		}
		/****************************end by xyy 2010-6-24 15:29:26*******************************/
		ret = Mo_bank_acc_po_Upd_Upd( sMoBankAccPo, g_pub_tx.reply );
		if ( ret )
		{
			sprintf( acErrMsg, "修改承兑汇票基本信息登记薄错[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}
		Mo_bank_acc_po_Clo_Upd();
	}
	/* 当退票同时新开汇票时，需要再记一条流水 */
	if ( iPactNoNum2 != 0 )
	{
		/* 承兑汇票上需要打印的部分固定内容 */
		set_zd_data( "0740", "晋中银行" );        /* 付款行全称 */
		set_zd_data( "0310", "401175056393" );            /* 付款行行号 */
		set_zd_data( "0820", "山西省晋中市榆次区东顺城街129号" );    /* 付款行地址 */
	}
	g_pub_tx.tx_amt1 = dZzAmt;
	/** begin add by chenchao 2011/11/23 20:19:00 **/ 
	ret = sql_count("mo_bank_po_fee","num=%d and pact_no='%s' ",iPactNoNum1,sMoBankAccPo.pact_no);
	if(ret>0)
	{
		ret = sql_execute("update mo_bank_po_fee set new_num = %ld where num=%ld and pact_no='%s' ",iPactNoNum2,iPactNoNum1,sMoBankAccPo.pact_no);
		if(ret)
		{
			sprintf( g_pub_tx.reply, "H034" );
			sprintf( acErrMsg, "更新票据明细表失败[%d]", ret );
			WRITEMSG
			goto ErrExit; 
		}
	}else if(ret == 0)
	{
		ret = sql_execute("update mo_bank_po_fee set new_num = %ld where new_num=%ld and pact_no='%s' ",iPactNoNum2,iPactNoNum1,sMoBankAccPo.pact_no);
		if(ret)
		{
			sprintf( g_pub_tx.reply, "H034" );
			sprintf( acErrMsg, "更新票据明细表失败[%d]", ret ); 
			WRITEMSG
			goto ErrExit; 
		}
	}else{
		sprintf( g_pub_tx.reply, "H034" );
		sprintf( acErrMsg, "查裝ank_po_fee失败[%d]", ret ); 
		WRITEMSG
		goto ErrExit; 
	}
	/** end add by chenchao 2011/11/23 20:19:00 **/
	
	/* 检查是否有保证金，如果有走正常存取款分录，如果没有则只修改登记簿，没有账务处理 */
	get_zd_data( "0320", sMoBankAccPo.bail_ac_no );
	/***
	if ( strcmp( sMoBankAccPo.bail_ac_no, "" ) != 0 && sMoBankAccPo.bail_amt>0.005)
	***/
	if ( strcmp( sMoBankAccPo.bail_ac_no, "" ) != 0 && dZzAmt >0.005)/*** 判断取款金额 mod by chenchao 2013/3/20 19:58:23 **/
	{
		/**--- xxx ---- 20051104 ---- 计算保证金利息 ----**/
		if ( pub_base_CompDblVal(dZzAmt,0.00) < 0 )
		{
			/**sprintf( acErrMsg, "新开承兑汇票不能超过原承兑汇票金额 原=[%.2f]新=[%.2f]",dPoAmt1,dPoAmt2);
			WRITEMSG
			sprintf( g_pub_tx.reply, "B341" );
			goto ErrExit;***/
			goto OkExit;
		}
		if ( pub_base_CompDblVal(dZzAmt,0.00) == 0 )
			goto OkExit;

		ret = Mo_bail_rate_Dec_Upd(g_pub_tx.reply," bail_type='1' and pact_no='%s' and sts='1' ", sMoBankAccPo.pact_no );
		if ( ret )
		{
			sprintf( acErrMsg, "执行Mo_bail_rate_Dec_Upd错![%d][%s]",ret,sMoBankAccPo.pact_no);
			WRITEMSG
			sprintf( g_pub_tx.reply, "B334" );
			goto ErrExit;
		}
		ret = Mo_bail_rate_Fet_Upd(&sMoBailRate,g_pub_tx.reply);
		if ( ret==100 )    
		{
			strcpy(intst_flag,"0");
			g_pub_intst.dealval=0.00;
			g_pub_intst.tax_intst=0.00;                    /** 应税利息 **/
			g_pub_intst.factval=0.00;                    /** 实付利息 **/
			g_pub_intst.val=0.00;                        /** 利息税 **/
			g_pub_intst.keepval=0.00;                    /** 保值利息 **/
			set_zd_data("0153","52101");                /** 保证金利息 **/
			set_zd_double("102P",g_pub_intst.dealval);
			goto OkExit1;
		}
		else if ( ret )
		{
			sprintf( acErrMsg, "执行Mo_bail_rate_Fet_Upd错![%d][%s]",ret,sMoBankAccPo.pact_no);
			WRITEMSG
			sprintf( g_pub_tx.reply, "B334" );
			goto ErrExit;
		}
		memcpy(&sMoBailRate1,&sMoBailRate,sizeof(sMoBailRate));
		ret = pub_base_getllz(HQLLBH,"01",g_pub_tx.tx_date,&HQ_rate);
		if ( ret )
		{
			sprintf( acErrMsg, "取利率错误!" );
			WRITEMSG
			sprintf( g_pub_tx.reply, "O195" );
			goto ErrExit;	
		}
		/** 判断是否到期 **/
		if ( g_pub_tx.tx_date<sMoBailRate.end_date ) 	/** 提前支取 **/
		{
			/** 计算天数 **/
			if ( pub_base_CalDays(sMoBailRate.beg_date,g_pub_tx.tx_date,"0",&HQday) )
			{
				sprintf( acErrMsg, "计算天数错误!");
				WRITEMSG
				goto ErrExit;
			}
			day=0;
			/** 计算利息 **/
			if ( pub_base_CompDblVal(dZzAmt,sMoBailRate.amt) > 0 )
			{
				/*** HQdealval = HQ_rate * sMoBailRate.amt * HQday / L360 / L100; 晋中要求只计算输入的利率 2011-8-9 16:23:29 **/
				if (sMoBailRate.ic_rate > 0.005)
				{
					g_pub_intst.dealval = HQ_rate * sMoBailRate.amt * HQday / L360 / L100;
				}else
				{
					g_pub_intst.dealval = 0.00;
				}
				sMoBailRate1.amt=0.00;
			}
			else
			{
				/***HQdealval = HQ_rate * dZzAmt * HQday / L360 / L100;晋中要求只计算输入的利率 2011-8-9 16:23:29 **/
				if (sMoBailRate.ic_rate > 0.005)
				{
					g_pub_intst.dealval = HQ_rate * dZzAmt * HQday / L360 / L100;
				}else
				{
					g_pub_intst.dealval = 0.00;
				}
				sMoBailRate1.amt=sMoBailRate.amt-dZzAmt;
				sMoBailRate.amt=dZzAmt;
			}
		}
		else
		{
			/** 计算天数 **/
			if ( pub_base_CalDays(sMoBailRate.beg_date,sMoBailRate.end_date,"0",&day) )
			{
				sprintf( acErrMsg, "计算天数错误!");
				WRITEMSG
				goto ErrExit;
			}
			/** 计算利息 **/
			if ( pub_base_CompDblVal(dZzAmt,sMoBailRate.amt) > 0 )
			{
				/*** HQdealval = HQ_rate * sMoBailRate.amt * HQday / L360 / L100; 晋中要求只计算输入的利率 2011-8-9 16:23:29 **/
				if (sMoBailRate.ic_rate > 0.005)
				{
					g_pub_intst.dealval = sMoBailRate.ic_rate * sMoBailRate.amt * day / L360 / L100;
				}else
				{
					g_pub_intst.dealval = 0.00;
				}
				sMoBailRate1.amt=0.00;
			}
			else
			{
				/***HQdealval = HQ_rate * dZzAmt * HQday / L360 / L100;晋中要求只计算输入的利率 2011-8-9 16:23:29 **/
				if (sMoBailRate.ic_rate > 0.005)
				{
					g_pub_intst.dealval = sMoBailRate.ic_rate * dZzAmt * day / L360 / L100;
				}else
				{
					g_pub_intst.dealval = 0.00;
				}
				sMoBailRate1.amt=sMoBailRate.amt-dZzAmt;
				sMoBailRate.amt=dZzAmt;
			}
		}
		sprintf(acErrMsg,"HAO --- look --- HQday=[%d],day=[%d]",HQday,day);

		g_pub_intst.dealval = pub_base_PubDround_1(g_pub_intst.dealval);
		if ( pub_base_CompDblVal(g_pub_intst.dealval,0.00)<0 )
		{
			sprintf(acErrMsg,"计算利息错误错误!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"D158");
			goto ErrExit;
		}
		intst_flag[0]='1'; 
		/** 更新保证金计息金额 **/
		sMoBailRate.intst_date=g_pub_tx.tx_date;
		sMoBailRate.end_trace_no=g_pub_tx.trace_no;
		sMoBailRate.intst_amt=g_pub_intst.dealval;
		strcpy(sMoBailRate.sts,"*");
		ret = Mo_bail_rate_Upd_Upd(sMoBailRate,g_pub_tx.reply);
		if ( ret )
		{
			sprintf( acErrMsg, "执行Mo_bail_rate_Upd_Upd错![%d][%s]",ret,sMoBankAccPo.pact_no);
			WRITEMSG
			sprintf( g_pub_tx.reply, "B334" );
			goto ErrExit;
		}
		Mo_bail_rate_Clo_Upd();

		/** 插入新的保证金利息登记薄 **/
		vtcp_log("[%s][%d]注意: 新保证金=[%f]",__FILE__,__LINE__,sMoBailRate1.amt);
		if ( pub_base_CompDblVal(sMoBailRate1.amt,0.00) > 0 )
		{
			ret = Mo_bail_rate_Ins(sMoBailRate1,g_pub_tx.reply);
			if ( ret )
			{
				sprintf( acErrMsg, "执行Mo_bail_rate_Ins错![%d]",ret);
				WRITEMSG
				sprintf( g_pub_tx.reply, "B334" );
				goto ErrExit;
			}
		}
		g_pub_intst.tax_intst=g_pub_intst.dealval;    /** 应税利息 **/
		g_pub_intst.factval=g_pub_intst.dealval;    /** 实付利息 **/ 
		g_pub_intst.val=0.00;                        /** 利息税 **/
		g_pub_intst.keepval=0.00;                    /** 保值利息 **/

		dZzAmt=dZzAmt+g_pub_intst.dealval;
		set_zd_double("0420",dZzAmt);
		set_zd_data("0153","52101");                /** 保证金利息 **/
		set_zd_double("102P",g_pub_intst.dealval);
		goto OkExit1;
	}else if ( pub_base_CompDblVal(dZzAmt,0.00) > 0 )/***金额大于0 就要记账 mod by chenchao 2013/3/20 19:58:59**/
	{
		sprintf( acErrMsg, "金额大约0去记账[%.2lf]",dZzAmt);
		WRITEMSG
		goto OkExit1;
	}
	
OkExit:
	/**----- xxx ---- @20050819@ ---- 撤销必须要有协议编号 ----**/
	strcpy(g_pub_tx.ac_no,sMoBankAccPo.pact_no);
	strcpy( g_pub_tx.note_type, sComParm.val );
	strcpy( g_pub_tx.beg_note_no, cPoNo11 );
	strcpy( g_pub_tx.brf, cPoNo22 );
	g_pub_tx.svc_ind=0;
	strcpy(g_pub_tx.no_show,"1");
	ret = pub_ins_trace_log();
	if ( ret )
	{
		sprintf( acErrMsg, "记流水日志错" );
		WRITEMSG
		goto ErrExit;
	}
	strcpy(g_pub_tx.no_show,"0");
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
OkExit1:
	/**----- xxx ---- @20050819@ ---- 撤销必须要有协议编号 ----**/
		strcpy(g_pub_tx.ac_no,sMoBankAccPo.pact_no);
	strcpy( g_pub_tx.note_type, sComParm.val );
	strcpy( g_pub_tx.beg_note_no, cPoNo11 );
	strcpy( g_pub_tx.brf, cPoNo22 );
	g_pub_tx.svc_ind=0;
	strcpy(g_pub_tx.no_show,"1");
	if ( intst_flag[0]=='1' )        /*** 保证金计息了 ***/
	{
		strcpy(g_pub_tx.note_type,"Y");
	}
	ret = pub_ins_trace_log();
	if ( ret )
	{
		sprintf( acErrMsg, "记流水日志错" );
		WRITEMSG
		goto ErrExit;
	}
	strcpy(g_pub_tx.no_show,"0");
	strcpy( g_pub_tx.reply, "0001" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	if( strncmp(g_pub_tx.reply,"0000",4) == 0)
		memcpy(g_pub_tx.reply,"H034",4);
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
