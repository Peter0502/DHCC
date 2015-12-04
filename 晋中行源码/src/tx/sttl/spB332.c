/*************************************************
* 文 件 名:  spB332.c
* 功能描述： 完成银行承兑汇票信息审核功能
*
* 作    者:  jane
* 完成日期： 2003年5月25日
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

int spB332()
{
	struct	mo_bank_acc_po_c 	sMoBankAcc;	/* 承兑汇票基本信息登记簿 */
	struct	mdm_ac_rel_c 		sMdmAcRel;	/* 介质账号对照表 */
	struct	dd_mst_c 		sDdMst;		/* 活期主文件 */
	char	cPayeeName[51];				/* 收款人全称 */
	/****Modified by SSH,2006.12.6,放到33位,暂时通过****/
	char	cPayeeAcNo[33];				/* 收款人账号 */
	char	cPayeeBrName[41];			/* 收款人开户行行名 */
	int	ret;

	memset( &sMoBankAcc, 0x00, sizeof( struct mo_bank_acc_po_c ) );
	memset( &sMdmAcRel, 0x00, sizeof( struct mdm_ac_rel_c ) );
	memset( &sDdMst, 0x00, sizeof( struct dd_mst_c ) );
	memset( cPayeeName, 0x00, sizeof( cPayeeName ) );
	memset( cPayeeAcNo, 0x00, sizeof( cPayeeAcNo ) );
	memset( cPayeeBrName, 0x00, sizeof( cPayeeBrName ) );

	/* 数据初始化 */
	pub_base_sysinit();

	/* 从前台屏幕取值 */
	get_zd_data( "0330", sMoBankAcc.pact_no );	/* 承兑汇票协议编号 */
	get_zd_data( "0270", cPayeeName );		/* 收款人全称 */
	get_zd_data( "0830", cPayeeAcNo );		/* 收款人账号 */
	get_zd_data( "0810", cPayeeBrName );		/* 收款人开户行行名 */

	/* 根据承兑协议编号查找银行承兑汇票基本信息登记薄 */
	ret = Mo_bank_acc_po_Sel( g_pub_tx.reply, &sMoBankAcc, "pact_no='%s'", sMoBankAcc.pact_no );
	if( ret == 100 )
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

	if( sMoBankAcc.acc_po_sts[0] == '1' )
	{
		sprintf( acErrMsg, "此承兑协议已审核!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B200" );
		goto ErrExit;
	}else if( sMoBankAcc.acc_po_sts[0] == '2' )
	{
		sprintf( acErrMsg, "此笔承兑已签发!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B201" );
		goto ErrExit;
	}else if( sMoBankAcc.acc_po_sts[0] == '3' )
	{
		sprintf( acErrMsg, "此笔承兑已做退票修改!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B250" );
		goto ErrExit;
	}

	/* 如果收款人信息有所改动，则根据承兑协议编号修改承兑汇票基本信息登记薄 */
	ret = Mo_bank_acc_po_Dec_Upd( g_pub_tx.reply, "pact_no='%s' and acc_po_sts='%s'", \
				sMoBankAcc.pact_no, "0" );
	if ( ret )
	{
		sprintf( acErrMsg, "执行Mo_bank_acc_po_Dec_Upd错[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	ret = Mo_bank_acc_po_Fet_Upd( &sMoBankAcc, g_pub_tx.reply );
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

	/* 2008/12/9 17:01 add by martin 审核不能与录入为同一个人*/
	if(!strncmp(sMoBankAcc.tx_br_no,g_pub_tx.tel,sizeof(g_pub_tx.tel)-1))
		{
			sprintf(acErrMsg,"录入和复核不能同一个操作员!");
      WRITEMSG
      strcpy( g_pub_tx.reply,"P080");
      goto ErrExit;
		}
	strcpy( sMoBankAcc.tx_br_no, g_pub_tx.tx_br_no );	/* 交易机构 */
	
	/* 修改承兑汇票基本信息登记簿，状态为审核*/
	if( strcmp( cPayeeName, sMoBankAcc.payee_name ) != 0 )
		strcpy( sMoBankAcc.payee_name, cPayeeName );
	if( strcmp( cPayeeAcNo, sMoBankAcc.payee_ac_no ) != 0 )
		strcpy( sMoBankAcc.payee_ac_no, cPayeeAcNo );
	if( strcmp( cPayeeBrName, sMoBankAcc.payee_br_name ) != 0 )
		strcpy( sMoBankAcc.payee_br_name, cPayeeBrName );

	strcpy( sMoBankAcc.acc_po_sts, "1" );			/* 标志：1 审核 */

	ret = Mo_bank_acc_po_Upd_Upd( sMoBankAcc, g_pub_tx.reply );
	if ( ret )
	{
		sprintf( acErrMsg, "修改承兑汇票基本信息登记薄错[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	Mo_bank_acc_po_Clo_Upd();

	/* 记流水日志 */
	/**----- rob ---- 20030819 ---- 撤销必须要有协议编号 ----**/
	strcpy(g_pub_tx.ac_no,sMoBankAcc.pact_no);
	g_pub_tx.tx_amt1=0.00;
	strcpy( g_pub_tx.brf, "银行承兑汇票审批" );	/* 备注 */
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
