/*************************************************
* 文 件 名:  spF329.c
* 功能描述： 担保保证金存款
*
* 作    者:  mike
* 完成日期： 2004年11月09日
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
#include "protect_note_c.h"

int spF329()
{
	struct	protect_note_c 		sProtectNote;	/* 保函登记簿 */
	struct	mdm_ac_rel_c 		sMdmAcRel;		/* 介质账号对照表 */
	struct	dd_mst_c 			sDdMst;			/* 活期主文件 */
	double	dAmt;								/* 补缴金额 */
	char	cOperCode[2];						/* 操作码 */
	char	cAmtSource[2];						/* 金额来源 */
	char	cAcNo[20];							/* 扣款账号 */
	char	cBailAcNo[25];						/* 保证金帐号 */
	int		ret;

	/* 数据初始化 */
	memset( &sProtectNote, 0x00, sizeof( struct protect_note_c ) );
	memset( &sMdmAcRel, 0x00, sizeof( struct mdm_ac_rel_c ) );
	memset( &sDdMst, 0x00, sizeof( struct dd_mst_c ) );
	memset( cOperCode, 0x00, sizeof( cOperCode ) );
	memset( cAmtSource, 0x00, sizeof( cAmtSource ) );
	memset( cAcNo, 0x00, sizeof( cAcNo ) );
	memset( cBailAcNo, 0x00, sizeof( cBailAcNo ) );
	pub_base_sysinit();

	/* 从前台屏幕取值 */
	get_zd_data( "0690", cOperCode );			/* 操作码 */
	get_zd_data( "0330", sProtectNote.pact_no );/* 承兑汇票协议编号 */
	get_zd_double( "0420", &dAmt );				/* 补缴金额 */
	get_zd_data( "0680", cAmtSource );			/* 金额来源 */
	get_zd_data( "0380", cAcNo );				/* 扣款账号 */
	get_zd_data( "0320", cBailAcNo );			/* 保证金帐号 */
	if( cOperCode[0] == '1' )			/* 缴存保证金 */
	{
		/* 根据承兑协议编号查找银行承兑汇票基本信息登记薄 */
		ret = Protect_note_Sel( g_pub_tx.reply, &sProtectNote, "pact_no='%s'", sProtectNote.pact_no );
		if( ret == 0 )
		{
			sprintf( acErrMsg, "此合同号已存在!" );
			WRITEMSG
			sprintf( g_pub_tx.reply, "B071" );
			goto ErrExit;
		}
		else if( ret != 100 && ret )
	   	{
	   		sprintf( acErrMsg, "执行Protect_note_Sel错![%d]", ret );
			WRITEMSG
			goto ErrExit;
		}

		/* 检查保证金帐号的有效性:活期账号、账户状态正常 */
		ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdmAcRel, "ac_no='%s'", cBailAcNo );
		if( ret == 100 )
		{
			sprintf( acErrMsg, "保证金账号不存在![%s]", cBailAcNo );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B195" );
			goto ErrExit;
		}
		else if( ret )
		{
			sprintf( acErrMsg, "执行Mdm_ac_rel_Sel出错![%d]", ret );
			WRITEMSG
			goto ErrExit;
		}

		ret=Dd_mst_Sel(g_pub_tx.reply,&sDdMst,"ac_id=%ld and ac_seqn=%d",sMdmAcRel.ac_id, sMdmAcRel.ac_seqn );
		if( ret == 100 )
		{
			sprintf( acErrMsg, "输入的保证金账号非活期账号!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B196" );
			goto ErrExit;
		}
		else if( ret )
		{
			sprintf( acErrMsg, "执行Dd_mst_Sel错误![%d]", ret );
			WRITEMSG
			goto ErrExit;
		}

		if( strcmp( sDdMst.prdt_no, "142" ) !=0 )
		{
			sprintf( acErrMsg, "输入的保证金账号非担保保证金账号!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B346" );
			goto ErrExit;
		}

		if( sDdMst.ac_sts[0] != '1' )
		{
			sprintf( acErrMsg, "此保证金账号状态不正常!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B197" );
			goto ErrExit;
		}

		/* 登记汇票登记薄 */
		strcpy( sProtectNote.bail_ac_no, cBailAcNo );	/* 保证金帐号 */
		sProtectNote.bail_amt = dAmt;					/* 保证金金额 */
		strcpy( sProtectNote.protect_sts, "A" );       	/* 担保协议状态：A 存保证金 */
		sProtectNote.opn_date = g_pub_tx.tx_date;		/* 记录存保证金日期 */

		ret=Protect_note_Ins( sProtectNote, g_pub_tx.reply );
		if( ret == -239 )
    	{
        	sprintf( acErrMsg, "向保函登记薄插入记录错误!有重复记录!" );
        	WRITEMSG
        	strcpy( g_pub_tx.reply, "B347" );
        	return (-1);
    	}
		else if ( ret )
    	{
        	sprintf( acErrMsg, "执行Protect_note_Ins错误!ret=[%d]", ret );
        	WRITEMSG
        	return(-1);
    	}

		/* 为记流水作准备 */
		strcpy( g_pub_tx.brf, "承兑汇票缴存保证金" );	/* 备注 */
	}
	else						/* 补缴保证金 */
	{
		/* 金额来源如果为扣划，则需要检查扣款账号的有效性：本行活期账号，账户状态正常 */
		if( cAmtSource[0] == '2' )
		{
			ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdmAcRel, "ac_no='%s'", cAcNo );
			if( ret == 100 )
			{
				sprintf( acErrMsg, "扣款账号不存在![%s]", cAcNo );
				WRITEMSG
				strcpy( g_pub_tx.reply, "B214" );
				goto ErrExit;
			}
			else if( ret )
			{
				sprintf( acErrMsg, "执行Mdm_ac_rel_Sel出错![%d]", ret );
				WRITEMSG
				goto ErrExit;
			}
	
			ret = Dd_mst_Sel( g_pub_tx.reply, &sDdMst, "ac_id=%ld and ac_seqn=%d",\
						sMdmAcRel.ac_id, sMdmAcRel.ac_seqn );
			if( ret == 100 )
			{
				sprintf( acErrMsg, "此扣款账号非活期账号!" );
				WRITEMSG
				strcpy( g_pub_tx.reply, "B215" );
				goto ErrExit;
			}
			else if( ret )
			{
				sprintf( acErrMsg, "执行Dd_mst_Sel错误![%d]", ret );
				WRITEMSG
				goto ErrExit;
			}
	
			if( sDdMst.ac_sts[0] != '1' )
			{
				sprintf( acErrMsg, "此扣款账号状态不正常!" );
				WRITEMSG
				strcpy( g_pub_tx.reply, "B216" );
				goto ErrExit;
			}
		}

		/* 将补缴金额加到承兑汇票基本信息登记簿中保证金额一项中 */
		ret = Protect_note_Dec_Upd( g_pub_tx.reply, "pact_no='%s'", sProtectNote.pact_no );
		if ( ret )
		{
			sprintf( acErrMsg, "执行Protect_note_Dec_Upd错[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}
	
		ret = Protect_note_Fet_Upd( &sProtectNote, g_pub_tx.reply );
		if( ret == 100 )
		{
			sprintf( acErrMsg, "此合同号不存在!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B117" );
			goto ErrExit;
		}
		else if( ret )
	   	{
	   		sprintf( acErrMsg, "执行Protect_note_Fet_Upd错![%d]", ret );
	       	WRITEMSG
			goto ErrExit;
		}

		if( sProtectNote.protect_sts[0] == '1' )
		{
			sprintf( acErrMsg, "此协议已到期!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B348" );
			goto ErrExit;
		}

		/*---- 检查保证金帐号是否正确 ----*/
		pub_base_strpack(sProtectNote.bail_ac_no);
		sprintf( acErrMsg, "保证金帐号[%s][%s]",sProtectNote.bail_ac_no,cBailAcNo);
		WRITEMSG
		if( strlen(sProtectNote.bail_ac_no)>0 && strcmp(sProtectNote.bail_ac_no,cBailAcNo) )
		{
	   		sprintf( acErrMsg, "保证金帐号错误![%s][%s]",sProtectNote.bail_ac_no,cBailAcNo);
			WRITEMSG
			strcpy(g_pub_tx.reply,"B258");
			goto ErrExit;
		}
		if( strlen(sProtectNote.bail_ac_no)==0 )
		{
			strcpy(sProtectNote.bail_ac_no,cBailAcNo);
		}

		/* 修改承兑汇票基本信息登记簿 */
		sProtectNote.bail_amt += dAmt;		/* 保证金额 */
		ret = Protect_note_Upd_Upd( sProtectNote, g_pub_tx.reply );
		if ( ret )
		{
			sprintf( acErrMsg, "修改承兑汇票基本信息登记薄错[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}
		Protect_note_Clo_Upd();
		strcpy( g_pub_tx.brf, "补缴担保保证金" );	/* 备注 */
	}

	/* 记流水日志 */
	strcpy(g_pub_tx.ac_no,sProtectNote.pact_no);
	g_pub_tx.tx_amt1=dAmt;
	strcpy(g_pub_tx.no_show,"1");
	strcpy(g_pub_tx.add_ind,cOperCode);		/** 缴存补缴 **/
	ret = pub_ins_trace_log();
	if ( ret )
	{
		sprintf( acErrMsg, "记流水日志错" );
		WRITEMSG
		goto ErrExit;
	}
	set_zd_data( "0820", sDdMst.name );    /* 为打印传户名 */

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
