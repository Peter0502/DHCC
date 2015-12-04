/*************************************************
* 文 件 名:  spB356.c
* 功能描述： 完成银行承兑汇票质押出库功能
*
* 作    者:  dongxy
* 完成日期： 2006年09月15日
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
#include "mo_dep_reg_c.h"


int spB356()
{
        struct  mo_dep_reg_c            sMoDepReg;      /* 质押存单登记簿 */
        struct  mo_bank_po_c            sMoBankPo;      /* 银行承兑汇票登记簿 */

	char    cLoadName[50];       		   		/* 上传文件名 */
	double	sum_je=0.00;
	int		ret=0;

	/* 数据初始化 */
        memset( &sMoDepReg, 0x00, sizeof( struct mo_dep_reg_c ) );
        memset( &sMoBankPo, 0x00, sizeof( struct mo_bank_po_c ) );
	memset( cLoadName, 0x00, sizeof( cLoadName ) );
	pub_base_sysinit();

	/* 从前台屏幕取值 */
	get_zd_data( "0630", sMoDepReg.pact_no );			/* 质押编号 */

	/* 检查质押存单登记薄中是否已存在此质押协议编号 */
	ret = Mo_dep_reg_Sel( g_pub_tx.reply, &sMoDepReg, " pact_no='%s' ", sMoDepReg.pact_no );
	if( ret )
	{
		sprintf( acErrMsg, "执行Mo_dep_reg_Sel出错![%d][%s]", ret,sMoDepReg.pact_no );
		WRITEMSG
		goto ErrExit;
	}

	get_zd_double( "1002", &sMoDepReg.po_amt );			/* 质押总金额 */
	/* 检查质押总金额是否相符 */
	ret = sql_sum_double("mo_dep_reg","po_amt",&sum_je," pact_no='%s' ",sMoDepReg.pact_no );
	if( ret )
	{
		sprintf( acErrMsg, "计算总金额错误![%d][%s]", ret,sMoDepReg.pact_no );
		WRITEMSG
		goto ErrExit;
	}
	if( pub_base_CompDblVal(sum_je,sMoDepReg.po_amt)!=0 )
	{
		sprintf( acErrMsg, "总金额错误![%.2f][%.2f][%s]",sum_je,sMoDepReg.po_amt,sMoDepReg.pact_no );
		WRITEMSG
		strcpy(g_pub_tx.reply,"B186");
		goto ErrExit;
	}

	ret = Mo_dep_reg_Dec_Upd( g_pub_tx.reply, " pact_no='%s' ", sMoDepReg.pact_no );
	if( ret )
	{
		sprintf(acErrMsg,"执行Mo_dep_reg_Dec_Upd错误![%d]",ret);
		WRITEMSG
       		goto ErrExit;
	}

	while(1)
	{
		ret = Mo_dep_reg_Fet_Upd( &sMoDepReg, g_pub_tx.reply );
		if( ret == 100 ) break;
		else if( ret )
		{
			sprintf( acErrMsg, "执行Mo_dep_reg_Fet_Upd出错![%d]", ret );
			WRITEMSG
			goto ErrExit;
		}

                /** 判断此汇票的状态和金额 **/
                ret = Mo_bank_po_Dec_Upd( g_pub_tx.reply, "po_no='%s'",sMoDepReg.po_no);
                if( ret )
                {
                        sprintf( acErrMsg, "执行Mo_bank_po_Dec_Upd出错![%d]", ret );
                        WRITEMSG
                        goto ErrExit;
                }

                ret = Mo_bank_po_Fet_Upd( &sMoBankPo, g_pub_tx.reply );
                if( ret == 100 )
                {
                        sprintf( acErrMsg, "无此承兑汇票!" );
                        WRITEMSG
                        sprintf( g_pub_tx.reply, "B217" );
                        goto ErrExit;
                }
                else if( ret )
                {
                        sprintf( acErrMsg, "执行Mo_bank_po_Fet_Upd错![%d]", ret );
                        WRITEMSG
                        goto ErrExit;
                }
				/****add by liuyue 20070104 ***
                if( sMoBankPo.po_sts[0] != '5' || sMoBankPo.po_ind[0] != '1' )*/
                if( (sMoBankPo.po_sts[0] != '5' &&  sMoBankPo.po_sts[0] !='9') || sMoBankPo.po_ind[0] != '1' )
                {
                        sprintf( acErrMsg, "承兑汇票做过其它交易，不能出库!po_sts=[%c],po_ind=[%c]",sMoBankPo.po_sts[0],sMoBankPo.po_ind[0] );
                        WRITEMSG
                        sprintf( g_pub_tx.reply, "B062" );
                        goto ErrExit;
                }

                sMoBankPo.po_sts[0] = '1';      /** 汇票状态改为签发 **/
                ret = Mo_bank_po_Upd_Upd( sMoBankPo, g_pub_tx.reply );
                if ( ret )
                {
                        sprintf( acErrMsg, "修改承兑汇票登记薄错[%d]", ret );
                        WRITEMSG
                        sprintf( g_pub_tx.reply, "B068" );
                        goto ErrExit;
                }
                Mo_bank_po_Clo_Upd();

		ret = Mo_dep_reg_Del_Upd(sMoDepReg,g_pub_tx.reply);
		if( ret )
		{
			sprintf( acErrMsg, "执行Mo_dep_reg_Del_Upd出错![%d]", ret );
			WRITEMSG
			goto ErrExit;
		}

	}
	Mo_dep_reg_Clo_Sel ();

	/* 为记流水赋值 */
	strcpy( g_pub_tx.brf, "承兑汇票质押存单出库" );	/* 备注 */
	/* 记流水日志 */
	strcpy(g_pub_tx.ac_no,sMoDepReg.pact_no);
	strcpy(g_pub_tx.beg_note_no,sMoDepReg.pact_no);
	g_pub_tx.tx_amt1=0.00;
	strcpy(g_pub_tx.add_ind,"");
	ret = pub_ins_trace_log();
	if ( ret )
	{
		sprintf( acErrMsg, "记流水日志错" );
		WRITEMSG
		goto ErrExit;
	}

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
