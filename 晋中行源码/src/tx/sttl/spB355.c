/*************************************************
* 文 件 名:  spB355.c
* 功能描述： 完成银行承兑汇票入库功能
*
* 作    者:  Dongxy
* 完成日期： 2006年9月14日
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

int spB355()
{
	struct	mo_dep_reg_c 		sMoDepReg;	/* 质押存单登记簿 */
	struct	mo_bank_po_c 		sMoBankPo;	/* 银行承兑汇票登记簿 */

	char    cLoadName[50];       		   		/* 上传文件名 */
	int		ret, flag, num;
	double	dTotamt;		/** 质押总金额 **/

	memset( &sMoDepReg, 0x00, sizeof( struct mo_dep_reg_c ) );
	memset( &sMoBankPo, 0x00, sizeof( struct mo_bank_po_c ) );
	memset( cLoadName, 0x00, sizeof( cLoadName ) );

	/* 数据初始化 */
	pub_base_sysinit();

	/* 从前台屏幕取值 */
	get_zd_data( "0630", sMoDepReg.pact_no );			/* 质押汇票编号 */

	/* 检查银行承兑汇票入库登记簿中是否已存在此承兑协议编号 */
	ret = Mo_dep_reg_Sel( g_pub_tx.reply, &sMoDepReg, " pact_no='%s' ", sMoDepReg.pact_no );
	if( ret == 0 )
	{
		sprintf( acErrMsg, "此承兑协议编号已存在!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B307" );
		goto ErrExit;
	}
	else if( ret != 100 )
	{
		sprintf( acErrMsg, "执行Mo_dep_reg_Sel出错![%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	/* 从文件导入临时表mo_dep_reg中 */
	if( strcmp( sMoDepReg.pact_no, "" ) )
	{
		pub_base_GenDwnFilName( cLoadName );
		ret = tab_load( cLoadName, "Mo_dep_reg", "pact_no, po_no, po_amt" );
		if( ret )
		{
			sprintf(acErrMsg,"RET [%d]",ret);
			WRITEMSG
	       		goto ErrExit;
		}
		vtcp_log("[%s][%d]-----已经写入数据库!",__FILE__,__LINE__);
		/** 计算总金额 **/
		ret = sql_sum_double("mo_dep_reg","po_amt",&dTotamt," pact_no='%s' ",sMoDepReg.pact_no);
		if ( ret )
		{
		        sprintf( acErrMsg, "计算总金额错[%d]", ret );
		        WRITEMSG
		        goto ErrExit;
		}

		/* 目前控制一张汇票只能用于一个质押汇票编号的录入 */
		ret = Mo_dep_reg_Dec_Sel( g_pub_tx.reply, " pact_no='%s' ", sMoDepReg.pact_no );
		if( ret!= 0 )
		{
			sprintf(acErrMsg,"执行Mo_dep_reg_Dec_Sel错误![%d]",ret);
			WRITEMSG
       			goto ErrExit;
		}
		flag=0;
		while(1)
		{
			ret = Mo_dep_reg_Fet_Sel( &sMoDepReg, g_pub_tx.reply );
			if( ret == 100 && flag == 0 )
			{
				sprintf( acErrMsg, "此质押汇票编号不存在![%s]", sMoDepReg.pact_no );
				WRITEMSG
				strcpy( g_pub_tx.reply, "B308" );
				goto ErrExit;
			}
			else if( ret == 100 && flag > 0 )
			{
				break;
			}
			else if( ret )
			{
				sprintf( acErrMsg, "执行Mo_dep_reg_Sel出错![%d]", g_pub_tx.reply );
				WRITEMSG
				goto ErrExit;
			}
			num = 0;
			/* 计算汇票存单出现次数 */
			num = sql_count( "mo_dep_reg", " po_no='%s' ", sMoDepReg.po_no );
			if( num <= 0 )
			{
				sprintf( acErrMsg, "银行质押汇票入库登记簿mo_dep_reg错误!" );
				WRITEMSG
				goto ErrExit;
			}
			else if( num > 1 )
			{
				sprintf( acErrMsg, "一张汇票只允许对应一个质押汇票编号!" );
				WRITEMSG
				strcpy( g_pub_tx.reply, "B309" );
				goto ErrExit;
			}
			flag++;

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
			/** 判断汇票状态是否正确 **/
			if( (sMoBankPo.po_sts[0] != '1' && sMoBankPo.po_sts[0] != '9') || sMoBankPo.po_ind[0] != '1')
		        {
		                sprintf( acErrMsg, "承兑汇票状态不正常!po_sts=[%c],po_ind=[%c]",sMoBankPo.po_sts[0],sMoBankPo.po_ind[0] );
		                WRITEMSG
			            	sprintf( g_pub_tx.reply, "B062" );
		                goto ErrExit;
		    	}
			/** 判断前台输入的汇票金额是否小于汇票实际的票面金额 **/
			ret = pub_base_CompDblVal(sMoBankPo.po_amt,sMoDepReg.po_amt);
			if ( ret )
			{
			        sprintf( acErrMsg, "前台输入的汇票金额与汇票实际的票面金额不符[%d]", ret );
			        WRITEMSG
				      sprintf( g_pub_tx.reply, "B353" );
			        goto ErrExit;
			}
			sMoBankPo.po_sts[0] = '5';	/** 汇票状态改为担保 **/
		        ret = Mo_bank_po_Upd_Upd( sMoBankPo, g_pub_tx.reply );
		        if ( ret )
		        {
		                sprintf( acErrMsg, "修改承兑汇票登记薄错[%d]", ret );
		                WRITEMSG
			       	      sprintf( g_pub_tx.reply, "B068" );
		                goto ErrExit;
		        }
		        Mo_bank_po_Clo_Upd();
		}

		Mo_dep_reg_Clo_Sel ();
	}

	vtcp_log("[%s][%d]==========dTotamt=[%f]",__FILE__,__LINE__,dTotamt);

	set_zd_double( "0400", dTotamt);	/**  质押总金额 **/

	/* 为记流水赋值 */
	strcpy( g_pub_tx.brf, "银行质押汇票入库" );	/* 备注 */
	/* 记流水日志 */
	/**----- hao ---- @20050819@ ---- 撤销必须要有协议编号 ----**/
	strcpy(g_pub_tx.ac_no,sMoDepReg.pact_no);
	strcpy(g_pub_tx.beg_note_no,sMoDepReg.pact_no);
	g_pub_tx.tx_amt1=0.00;
	strcpy(g_pub_tx.add_ind,"");	/**/
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
