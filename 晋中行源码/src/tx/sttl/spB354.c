/*************************************************
* 文 件 名:  spB354.c
* 功能描述： 完成担保帐号和借据号关系的建立和解除
*
* 作    者:  Dongxy
* 完成日期： 2006年9月18日
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
#include "mo_bank_acc_po_c.h"

int spB354()
{
	struct	mo_bank_acc_po_c 	sMoBankAccPo;	/* 担保关系登记簿 */
	char	cOperCode[2];			/* 操作码 */
	int		ret;

	memset( &sMoBankAccPo, 0x00, sizeof( struct mo_bank_acc_po_c ) );
	memset( cOperCode, 0x00, sizeof( cOperCode) );

	/* 数据初始化 */
	pub_base_sysinit();

	/* 从前台屏幕取值 */
	get_zd_data( "0670", cOperCode );		/** 操作类型 **/
	get_zd_data( "0330", sMoBankAccPo.pact_no );	/** 承兑协议号 **/

	vtcp_log("[%s][%d]========操作类型=[%s]",__FILE__,__LINE__,cOperCode);


	if(cOperCode[0] == '0')	/** 前台选择建立 **/
	{
		vtcp_log("[%s][%d]========开始建立",__FILE__,__LINE__);
		/*	根据郭玉峰指示不检查票的状态了  2006－12－14
        	ret = Mo_bank_acc_po_Dec_Upd( g_pub_tx.reply, "pact_no='%s' and acc_po_sts in ('2','3')", \
		*/
        	ret = Mo_bank_acc_po_Dec_Upd( g_pub_tx.reply, "pact_no='%s' ", \
                                sMoBankAccPo.pact_no);
	        if ( ret )
	        {
       	         	sprintf( acErrMsg, "执行Mo_bank_acc_po_Dec_Upd错[%d]", ret );
                	WRITEMSG
                	goto ErrExit;
        	}

        	ret = Mo_bank_acc_po_Fet_Upd( &sMoBankAccPo, g_pub_tx.reply );
        	if( ret == 100 )
        	{
                	sprintf( acErrMsg, "此承兑协议编号不存在!" );
                	WRITEMSG
                	strcpy( g_pub_tx.reply, "B199" );
                	goto ErrExit;
        	}else if( ret )
        	{
                	sprintf( acErrMsg, "执行Mo_bank_acc_po_Fet_Upd错![%d]", ret );
                	WRITEMSG
                	goto ErrExit;
        	}
     /***
		if(sMoBankAccPo.acc_po_sts[0] != '0')
		{
                        sprintf( acErrMsg, "此协议已作其他交易，不可以删除" );
                        sprintf( g_pub_tx.reply, "B164" );
                        WRITEMSG
                        goto ErrExit;

		}***/
		/** 修改保证金帐号和比率 **/
		get_zd_data( "0320", sMoBankAccPo.bail_ac_no );	/** 保证金帐号 **/
		get_zd_double("1004", &sMoBankAccPo.bail_amt );	/** 保证金额 **/

		pub_base_old_acno( sMoBankAccPo.bail_ac_no );
		vtcp_log("[%s][%d]========bail_amt=[%f]",__FILE__,__LINE__,sMoBankAccPo.bail_amt);
		sMoBankAccPo.bail_amt_ratio=100;
		vtcp_log("[%s][%d]========bail_amt=[%f]",__FILE__,__LINE__,sMoBankAccPo.bail_amt);

	}
	else if(cOperCode[0] == '1')	/** 前台选择删除 **/
	{
		vtcp_log("[%s][%d]========开始删除",__FILE__,__LINE__);
		get_zd_data( "0320", sMoBankAccPo.bail_ac_no );	/** 保证金帐号 **/
                ret = Mo_bank_acc_po_Dec_Upd( g_pub_tx.reply, "pact_no='%s' and bail_ac_no='%s'", \
                                sMoBankAccPo.pact_no, sMoBankAccPo.bail_ac_no );
                if ( ret )
                {
                        sprintf( acErrMsg, "执行Mo_bank_acc_po_Dec_Upd错[%d]", ret );
                        WRITEMSG
                        goto ErrExit;
                }

                ret = Mo_bank_acc_po_Fet_Upd( &sMoBankAccPo, g_pub_tx.reply );
                if( ret == 100 )
                {
                        sprintf( acErrMsg, "此承兑协议编号不存在!" );
                        WRITEMSG
                        strcpy( g_pub_tx.reply, "B199" );
                        goto ErrExit;
                }else if( ret )
                {
                        sprintf( acErrMsg, "执行Mo_bank_acc_po_Fet_Upd错![%d]", ret );
                        WRITEMSG
                        goto ErrExit;
                }

		
		memset( sMoBankAccPo.bail_ac_no, 0x00, sizeof(sMoBankAccPo.bail_ac_no)-1 );	/** 保证金帐号 **/
		sMoBankAccPo.bail_amt=0;	/** 保证金额 **/
		sMoBankAccPo.bail_amt_ratio=0;
                set_zd_data( "0320", sMoBankAccPo.bail_ac_no ); /** 保证金帐号 **/
                set_zd_double("1004", sMoBankAccPo.bail_amt );  /** 保证金额 **/

	}
	else
	{
		sprintf( acErrMsg, "操作类型错误![%s]",cOperCode );
		WRITEMSG
		strcpy( g_pub_tx.reply, "O020" );
		goto ErrExit;
	}

        ret = Mo_bank_acc_po_Upd_Upd( sMoBankAccPo, g_pub_tx.reply );
        if ( ret )
        {
               	sprintf( acErrMsg, "修改承兑汇票基本信息登记薄错[%d]", ret );
               	WRITEMSG
               	goto ErrExit;
        }

        Mo_bank_acc_po_Clo_Upd();

	set_zd_data( "0300", sMoBankAccPo.payer_ac_no ); /* 出票人帐号*/
	set_zd_data( "0250", sMoBankAccPo.payer_name ); /* 出票人名称*/

	vtcp_log("[%s][%d]========开始写流水",__FILE__,__LINE__);
	/* 为记流水赋值 */
	strcpy( g_pub_tx.brf, "保证金帐号关联" );	/* 备注 */

	/* 记流水日志 */
	memcpy(g_pub_tx.ac_no,sMoBankAccPo.pact_no,19);	/* 流水里的帐号是19位的 */
	memcpy(g_pub_tx.beg_note_no,sMoBankAccPo.pact_no,16);
	g_pub_tx.tx_amt1=0.00;
	strcpy(g_pub_tx.add_ind,"");	/**/
	ret = pub_ins_trace_log();
	if ( ret )
	{
		sprintf( acErrMsg, "记流水日志错" );
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("[%s][%d]========程序结束",__FILE__,__LINE__);

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
