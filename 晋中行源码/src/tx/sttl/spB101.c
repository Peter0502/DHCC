/*************************************************
* 文 件 名:  spB101.c
* 功能描述： 完成银行汇票信息录入的功能。
*	     申请人和收款人为单位的，不得签发现金银行汇票
*	    （系统自动判断），即如果为非个人客户，必须是转
*	     帐汇票；现金汇票必须输入代理付款行行号和代理
*	     付款行行名，转账不输入；现金汇票可以不输入收款
*	     人帐号。
*
* 作    者:  jane
* 完成日期： 2003年1月17日
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

int spB101()
{
	struct	mo_bank_po_c	sMoBankPo;		/* 银行汇票登记簿 */
	char	cRequAcno[25];				/* 申请人帐号 */
	char	cif_type[2];				/* 客户类型 */
	char	cOpenBrno[9];				/* 申请人帐号开户机构 */
	char	cOperCode[6];				/* 业务代码 */
	double	dAcBal;					/* 帐户余额 */
	int	ret;

	memset( &sMoBankPo, 0x00, sizeof( struct mo_bank_po_c ) );
	memset( cRequAcno, 0x00, sizeof( cRequAcno ) );
	memset( cif_type, 0x00, sizeof( cif_type ) );
	memset( cOpenBrno, 0x00, sizeof( cOpenBrno ) );
	memset( cOperCode, 0x00, sizeof( cOperCode ) );
	
	/* 数据初始化 */
	pub_base_sysinit();
	
	/* 从前台屏幕取值 */
	get_zd_data( "0580", sMoBankPo.po_requ_no );		/* 申请书编号 */
	get_zd_data( "0590", sMoBankPo.po_no );			/* 汇票编号 */
	get_zd_data( "0670", sMoBankPo.ct_ind );		/* 现转标志 */
	get_zd_data( "0250", sMoBankPo.requ_name );		/* 申请人姓名 */
	get_zd_data( "0300", cRequAcno );	
	get_zd_long( "0340", &sMoBankPo.requ_ac_seqn );		/* 申请人帐号序号 */
	get_zd_data( "0810", sMoBankPo.payee_name );		/* 收款人名称 */
	get_zd_data( "0310", sMoBankPo.payee_ac_no );		/* 收款人账号 */
	get_zd_data( "0870", sMoBankPo.payer_br_no );		/* 代理付款行行号 */
	get_zd_data( "0270", sMoBankPo.payer_br_name );		/* 代理付款行行名 */
	get_zd_double( "0390", &sMoBankPo.sign_amt );		/* 签发金额 */
	get_zd_data( "0820", sMoBankPo.purpose );		/* 用途 */
	get_zd_data( "0830", sMoBankPo.rmk );			/* 备注 */
	get_zd_data( "0680", sMoBankPo.attorn_ind );		/* 转让标志 */

	strcpy( cOperCode, "10001" );

	/* 检查交易机构是否具有权限 */
	ret = pub_sttl_ChkInputBrno( cOperCode, g_pub_tx.tx_br_no );
	if ( ret )
	{
		sprintf( acErrMsg, "检查交易机构权限错" );
		WRITEMSG
		goto ErrExit;
	}

	/* 根据申请人帐号和帐号序号取申请人账号ID */
	ret = pub_base_ChkAc( cRequAcno, sMoBankPo.requ_ac_seqn, cOpenBrno, &sMoBankPo.requ_ac_id );
	if( ret )
	{
		sprintf( acErrMsg, "根据申请人帐号和帐号序号取申请人账号ID错" );
		WRITEMSG
		goto ErrExit;
	}
	
	/* 根据申请人帐号取客户类型 */
	ret = pub_cif_AcToCifType ( cRequAcno, cif_type );
	if( ret )
	{
		sprintf( acErrMsg, "根据申请人帐号取客户类型错" );
		WRITEMSG
		goto ErrExit;
	}

	/* 如果是非个人帐户，必须转帐 */
	if( cif_type[0]!='3' && cif_type[0]!='4' )
	{
		if( sMoBankPo.ct_ind[0] == '1' )
		{
			sprintf( acErrMsg, "非个人申请人，不能是现金汇票" );
			WRITEMSG
			sprintf( g_pub_tx.reply, "B013" );
			goto ErrExit;
		}
	}
	
	/* 检查出票金额是否小于帐户余额 */
	ret = pub_base_ChkAcBal( cRequAcno, sMoBankPo.requ_ac_seqn, &dAcBal );
	if( ret )	
	{
		sprintf( acErrMsg, "检查帐户余额错" );
		WRITEMSG
		goto ErrExit;
	}
	
	if( pub_base_CompDblVal( sMoBankPo.sign_amt , dAcBal ) == 1 )
	{
		sprintf( acErrMsg, "出票金额大于帐户余额,不能进行汇票申请!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B001" );
		goto ErrExit;
	}

	/* 为银行汇票结构赋值 */
	strcpy( sMoBankPo.po_sts, "0" );			/* 状态为录入 */
	strcpy( sMoBankPo.po_ind, "1" );			/* 标志为正常 */
	sMoBankPo.requ_date = g_pub_tx.tx_date;			/* 汇票申请日期 */
	strcpy( sMoBankPo.tx_br_no, g_pub_tx.tx_br_no );	/* 出票行行号 */
	
	/* 登记汇票登记薄 */
	ret = pub_sttl_RegMoBankPo( sMoBankPo );
	if ( ret )
    	{
        	sprintf( acErrMsg, "执行pub_sttl_RegMoBankPo错误!ret=[%d]", ret );
        	WRITEMSG
        	goto ErrExit;
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
