/*************************************************
* 文 件 名:  spB108.c
* 功能描述： 完成银行汇票挂失/解挂的功能
*
* 作    者:  Terry
* 完成日期： 2003年1月19日
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
#include "mo_loss_c.h"

int spB108()
{
	struct	mo_bank_po_c	sMoBankPo;	/* 银行汇票登记簿 */
	struct  mo_loss_c	sMoLoss;	/* 挂失解挂登记薄 */
	char	cIdType[3];			/* 证件类型 */
	char	cIdNo[21];			/* 证件号码 */
	char	cName[51];			/* 申请人名称 */
	char	cFlag[2];			/* 挂失解挂标志0挂失1解挂 */
	char	cSts[2];			/* 状态 */
	char	cPerSts[2];			/* 前期状态 */
	char	cOperCode[6];				/* 业务代码 */
	int	ret;
	
	/* 数据初始化 */
	memset( &sMoBankPo, 0x00, sizeof( struct mo_bank_po_c ) );
	memset( &sMoLoss, 0x00, sizeof( struct mo_loss_c ) );
	
	memset( cIdType, 0x00, sizeof ( cIdType ) );
	memset( cIdNo, 0x00, sizeof ( cIdNo ) );
	memset( cName, 0x00, sizeof ( cName ) );
	memset( cFlag, 0x00, sizeof ( cFlag ) );
	memset( cSts, 0x00, sizeof ( cSts ) );
	memset( cOperCode, 0x00, sizeof( cOperCode ) );
	
	pub_base_sysinit();
	
	/* 从前台屏幕取值 */
	get_zd_data( "0670", cFlag );			/* 挂失解挂标志0挂失1解挂 */
	get_zd_data( "0590", sMoBankPo.po_no );		/* 汇票号码 */
	get_zd_data( "0680", cIdType );			/* 证件类型 */
	get_zd_data( "0620", cIdNo );			/* 证件号码 */
	get_zd_data( "0250", cName );			/* 申请人全称 */
	strcpy( cOperCode, "10001" );
	
	pub_base_strpack( sMoBankPo.po_no );
	
	if ( cFlag[0] == '0' )		/* 挂失 */
	{
		strcpy( cPerSts, "1" );
		strcpy( cSts, "0" );
	}else				/* 解挂 */
	{
		strcpy( cPerSts, "0" );
		strcpy( cSts, "1" );
	}
	/* 查找汇票是否存在 */
	ret = sql_count( "mo_bank_po", "po_no = '%s'", sMoBankPo.po_no );
	if ( ret < 0 )
	{
		sprintf( acErrMsg, "查找汇票登记薄错[%d]", ret );
		WRITEMSG
		sprintf( g_pub_tx.reply, "B018" );
		goto ErrExit;
	}else if ( !ret )
	{
		sprintf( acErrMsg, "无此银行汇票" );
		WRITEMSG
		sprintf( g_pub_tx.reply, "B012" );
		goto ErrExit;
	}
	
	/* 检查汇票状态或者标志是否正常 */
	ret = sql_count( "mo_bank_po", "po_no = '%s' and po_ind = '%s'", \
					sMoBankPo.po_no , cPerSts );
	if ( ret < 0 )
	{
		sprintf( acErrMsg, "查找汇票登记薄错[%d]", ret );
		WRITEMSG
		sprintf( g_pub_tx.reply, "B018" );
		goto ErrExit;
	}else if ( !ret )
	{
		sprintf( acErrMsg, "银行汇票标志不正常ret=[%d]",ret );
		WRITEMSG
		sprintf( g_pub_tx.reply, "B032" );
		goto ErrExit;
	}else if ( ret > 1 )
	{
		sprintf( acErrMsg, "银行汇票登记薄存在多条记录" );
		WRITEMSG
		sprintf( g_pub_tx.reply, "B019" );
		goto ErrExit;	
	}

	/* 声明打开游标 */
	ret = Mo_bank_po_Dec_Upd( g_pub_tx.reply , "po_no = '%s' \
				and po_ind = '%s'", sMoBankPo.po_no, cPerSts );
	if ( ret )
	{
		sprintf( acErrMsg, "执行Mo_bank_po_Dec_Upd错[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}
	
	ret = Mo_bank_po_Fet_Upd( &sMoBankPo, g_pub_tx.reply );
	if ( ret == 100 )
	{
		sprintf( acErrMsg, "查找汇票登记薄错，无此银行汇票" );
		WRITEMSG
		sprintf( g_pub_tx.reply, "B012" );
		goto ErrExit;
	}else if ( ret )
	{
		sprintf( acErrMsg, "执行Mo_bank_po_Fet_Upd错[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}
			
	/* 检查交易机构是否具有权限 */
	ret = pub_sttl_ChkSignBrno( cOperCode, sMoBankPo.tx_br_no, g_pub_tx.tx_br_no );
	if ( ret )
	{
		sprintf( acErrMsg, "检查交易机构是否是签发行错" );
		WRITEMSG
		goto ErrExit;	
	}
	
	/* 修改汇票登记薄 */
	strcpy( sMoBankPo.po_ind, cSts );		/* 汇票标志 */	
	
	/* 修改汇票登记薄 */
	ret = Mo_bank_po_Upd_Upd( sMoBankPo , g_pub_tx.reply  );
	if ( ret )
	{
		sprintf( acErrMsg, "修改银行汇票登记薄错[%d]", ret );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B017" );
		goto ErrExit;	
	}
	
	Mo_bank_po_Clo_Upd( );
	
	/* 登记挂失登记薄 ,谁挂失谁解挂 */
	strcpy( sMoLoss.ac_no, sMoBankPo.po_no );/* 汇票号码 */
	strcpy( sMoLoss.id_type, cIdType);	/* 证件类型 */
	strcpy( sMoLoss.id_no, cIdNo );		/* 证件号码 */
	strcpy( sMoLoss.name, cName );		/* 申请人姓名 */

	if ( cFlag[0] == '0' )
	{
		strcpy( sMoLoss.loss_ind , "1" );	/* 挂失标志 */
		sMoLoss.loss_date = g_pub_tx.tx_date;	/* 挂失日期 */
		strcpy( sMoLoss.loss_br_no, g_pub_tx.tx_br_no );	/* 挂失机构 */
		strcpy( sMoLoss.loss_tel, g_pub_tx.tel );	/* 挂失柜员 */
	
		ret = Mo_loss_Ins( sMoLoss, g_pub_tx.reply );
		if ( ret )
		{
			sprintf( acErrMsg, "登记挂失登记薄错[%d]", ret );
			WRITEMSG
			goto ErrExit;	
		}
	}
	else 
	{
		pub_base_strpack(sMoLoss.ac_no);
		/* 查询挂失登记薄 */
		ret = Mo_loss_Dec_Upd( g_pub_tx.reply , "ac_no = '%s' \
				and id_type = '%s' and id_no = '%s' \
				and loss_ind = '1' and unloss_ind IS NULL" ,\
				sMoLoss.ac_no, sMoLoss.id_type, sMoLoss.id_no );
		if ( ret )
		{
			sprintf( acErrMsg, "执行Mo_loss_Dec_Upd错误" );
			WRITEMSG
			goto ErrExit;
		}
		
		ret = Mo_loss_Fet_Upd( &sMoLoss, g_pub_tx.reply );
		if ( ret == 100 )
		{
			sprintf( acErrMsg, "没找到挂失记录" );
			WRITEMSG
			sprintf( g_pub_tx.reply, "B033" );
			goto ErrExit;	
		}else if ( ret )
		{
			sprintf( acErrMsg, "取挂失记录错误[%d]", ret );
			WRITEMSG
			goto ErrExit;	
		}
				
		/* 登记解挂登记薄 */
		strcpy( sMoLoss.unloss_ind , "1" );	/* 解挂标志 */
		sMoLoss.unloss_date = g_pub_tx.tx_date;	/* 解挂日期 */
		strcpy( sMoLoss.unloss_tel, g_pub_tx.tel );	/* 解挂柜员 */		
		
		ret = Mo_loss_Upd_Upd( sMoLoss, g_pub_tx.reply );
		if ( ret )
		{
			sprintf( acErrMsg, "修改挂失登记薄错" );
			WRITEMSG
			sprintf( g_pub_tx.reply, "B034" );
			goto ErrExit;	
		}
		
		Mo_loss_Clo_Upd( );	
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

