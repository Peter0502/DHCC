/***************************************************************
* 文 件 名: spJ725.c
* 功能描述：汇票全额兑付通知
*			更新签发登记簿中的信息
*
* 作    者: jane
* 完成日期：2004年07月01日
*
* 修改记录：
*    日    期:
*    修 改 人:
*    修改内容:
*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "hv_pub.h"
#include "hv_define.h"
#include "com_tel_c.h"
#include "find_debug.h"

spJ725()
{
	struct mbfebillopnreg_c billopnreg_v;
	struct mbfebrbkno_c brbkno_v;
	struct com_tel_c sComTel;

	memset( &billopnreg_v, 0x00, sizeof( struct mbfebillopnreg_c ) );

	/*签发登记簿处理*/
	billopnreg_v.amount = atof( mbfetran.remittanceamnt ) / 100;    /*出票金额*/
	pub_base_strpack( mbfetran.payeracc );
	pub_base_old_acno( mbfetran.payeracc );
	g_reply_int = Mbfebillopnreg_Dec_Upd( g_pub_tx.reply, "billsts='2' and payeropenaccbkcode='%s' and signdate='%s' and billcode='%s' and billofxchngseal='%s' and payeracc='%s' and amount='%lf'", mbfetran.payeropenaccbkcode, mbfetran.warrantdate, mbfetran.warrantno, mbfetran.billofxchngseal, mbfetran.payeracc, billopnreg_v.amount );
	if( g_reply_int )
	{
		sprintf(acErrMsg,"汇票全额兑付通知检查签发登记簿声明游标数据库操作失败[%d]", g_reply_int);
		WRITEMSG
		goto ErrExit;
	}
	g_reply_int = Mbfebillopnreg_Fet_Upd( &billopnreg_v, g_pub_tx.reply );
	if( g_reply_int == 100 )
	{
		strcpy( mbfetran.procstate,"5E" );
		sprintf(acErrMsg,"汇票全额兑付通知非本行签发并且未核销汇票");
		WRITEMSG
		Mbfebillopnreg_Clo_Upd();
		strcpy( mbfetran.prilevel, "1" );    /*传递清算状态0:成功清算1:错误报文*/
		goto ErrExit;
	}
	else if( g_reply_int )
	{
		sprintf(acErrMsg,"汇票全额兑付通知检查签发登记簿取游标数据库操作失败[%d]", g_reply_int);
		WRITEMSG
		goto ErrExit;
	}

	strcpy( mbfetran.prilevel, "0" );    /*传递清算状态0:成功清算1:错误报文*/

	strcpy( billopnreg_v.billsts, "*" );    /*汇票状态*:核销*/
	strcpy( billopnreg_v.useind, "2" );    /*核销状态3:全额兑付*/
	billopnreg_v.canceldate = g_pub_tx.tx_date;    /*核销日期*/
	billopnreg_v.cancelflowno = g_pub_tx.trace_no;    /*核销流水号*/
	strcpy( billopnreg_v.cancel_tel, g_pub_tx.tel );    /*核销操作员*/
	billopnreg_v.prmptpaydate = atol( mbfetran.notepaydate );    /*提示付款日期*/
	billopnreg_v.repudiationamnt = atof( mbfetran.factclramnt ) / 100;    /*实际结算金额*/
	billopnreg_v.redundantamnt = atof( mbfetran.amount ) / 100;    /*多余金额*/
	strcpy( billopnreg_v.recipientopenaccbk, mbfetran.recipientopenaccbk );    /*最后持票人开户行号*/
	strcpy( billopnreg_v.lastbillholderacc, mbfetran.lastbillholderacc );    /*最后持票人账号*/
	strcpy( billopnreg_v.lastbillholdername, mbfetran.lastbillholdername );    /*最后持票人名称*/
	/*自动解挂处理*/
	if( billopnreg_v.los_rpt_ind[0] == '1' )
	{
		strcpy( billopnreg_v.los_rpt_ind, "2" );    /*挂失标志2:解挂*/
		billopnreg_v.clos_rpt_date = g_pub_tx.tx_date;    /*解挂日期*/
		strcpy( billopnreg_v.clos_rpt_tel, g_pub_tx.tel );    /*解挂操作员*/
	}

	g_reply_int = Mbfebillopnreg_Upd_Upd( billopnreg_v, g_pub_tx.reply );
	if( g_reply_int )
	{
		sprintf(acErrMsg,"汇票全额兑付通知检查签发登记簿执行游标数据库操作失败[%d]", g_reply_int);
		WRITEMSG
		goto ErrExit;
	}

	Mbfebillopnreg_Clo_Upd();

	if( pub_mbfe_bmreg() ) goto ErrExit;

	if( mbfetran.prilevel[0] == '0' )
	{
		/*置来帐交易机构号和操作员号*/
		memset( &brbkno_v, 0x00, sizeof( struct mbfebrbkno_c ) );
		g_reply_int = Mbfebrbkno_Sel( g_pub_tx.reply, &brbkno_v, "bkno='%s'", mbfetran.payeropenaccbkcode );
		if( g_reply_int )
		{
			sprintf(acErrMsg,"查询来帐接收行对应机构号数据库操作失败[%d]",g_reply_int);
			WRITEMSG
			return -1;
		}
		strcpy( g_pub_tx.tx_br_no, brbkno_v.brno );
		set_zd_data( "0030", g_pub_tx.tx_br_no );
		memset( &sComTel, 0x00, sizeof( struct com_tel_c ) );
		g_reply_int = Com_tel_Sel( g_pub_tx.reply, &sComTel, "br_no='%s' and lvl='4'", brbkno_v.brno );
		if( g_reply_int )
		{
			sprintf(acErrMsg,"查询来帐接收行对应机构号数据库操作失败[%d]",g_reply_int);
			WRITEMSG
			return -1;
		}
		strcpy( g_pub_tx.tel, sComTel.tel );
		set_zd_data( "0070", g_pub_tx.tel );

		/*清算开出汇票签发户和移存户*/
		g_reply_int = set_bill_opnrmv( billopnreg_v.amount );
		if( g_reply_int )
		{
			sprintf(acErrMsg,"清算开出汇票签发户和移存户出错[%d]", g_reply_int);
			WRITEMSG
			goto ErrExit;
		}
	}

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"更新汇票签发登记簿成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
        if(memcmp(g_pub_tx.reply,"0000",4)==0)
                MEMCPY_DEBUG(g_pub_tx.reply,"T063",4);
	sprintf(acErrMsg,"更新汇票签发登记簿失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

/************************************************
*     登记汇票全额兑付通知表                    *
************************************************/
int pub_mbfe_bmreg()
{
	struct mbfebillmsgreg_c mbfebillmsgreg_v;

	memset( &mbfebillmsgreg_v, 0x00, sizeof( struct mbfebillmsgreg_c ) );

	if( get_serialno_val( "13", mbfebillmsgreg_v.msssno ) );
	strcpy( mbfebillmsgreg_v.sts, mbfetran.prilevel );
	mbfebillmsgreg_v.consigndate=atol(mbfetran.consigndate);
	strcpy( mbfebillmsgreg_v.rdficode, mbfetran.rdficode );
	strcpy( mbfebillmsgreg_v.payeropenaccbkcode, mbfetran.payeropenaccbkcode );
	strcpy( mbfebillmsgreg_v.payeracc, mbfetran.payeracc );
	strcpy( mbfebillmsgreg_v.payername, mbfetran.payername );
	strcpy( mbfebillmsgreg_v.payeraddr, mbfetran.payeraddr );
	mbfebillmsgreg_v.billdate = atol( mbfetran.warrantdate );
	strcpy( mbfebillmsgreg_v.billcode, mbfetran.warrantno );
	strcpy( mbfebillmsgreg_v.billofxchngseal, mbfetran.billofxchngseal );
	mbfebillmsgreg_v.remittanceamnt = atof( mbfetran.remittanceamnt )/100;
	mbfebillmsgreg_v.repudiationamnt = atof( mbfetran.factclramnt )/100;
	strcpy( mbfebillmsgreg_v.rdficenter, mbfetran.rdficenter );
	strcpy( mbfebillmsgreg_v.recipientopenaccbk, mbfetran.recipientopenaccbk );
	strcpy( mbfebillmsgreg_v.lastbillholderacc, mbfetran.lastbillholderacc );
	strcpy( mbfebillmsgreg_v.lastbillholdername, mbfetran.lastbillholdername );
	mbfebillmsgreg_v.prmptpaydate = atol( mbfetran.notepaydate );
	strcpy( mbfebillmsgreg_v.strinfo, mbfetran.strinfo );

	sprintf( acErrMsg, "mbfebillmsgreg: 1n[%s] 2n[%ld] 3n[%s] 4n[%s] 5n[%s] 6n[%s] 7n[%s] 8n[%ld] 9[%s] 10[%s] 11n[%f] 12[%f] 13n[%s] 14[%s] 15n[%s] 16[%s] 17[%ld] 18[%s]", mbfebillmsgreg_v.msssno, mbfebillmsgreg_v.consigndate, mbfebillmsgreg_v.rdficode, mbfebillmsgreg_v.payeropenaccbkcode, mbfebillmsgreg_v.payeracc, mbfebillmsgreg_v.payername, mbfebillmsgreg_v.payeraddr, mbfebillmsgreg_v.billdate, mbfebillmsgreg_v.billcode, mbfebillmsgreg_v.billofxchngseal, mbfebillmsgreg_v.remittanceamnt, mbfebillmsgreg_v.repudiationamnt, mbfebillmsgreg_v.rdficenter, mbfebillmsgreg_v.recipientopenaccbk, mbfebillmsgreg_v.lastbillholderacc, mbfebillmsgreg_v.lastbillholdername, mbfebillmsgreg_v.prmptpaydate, mbfebillmsgreg_v.strinfo );
	WRITEMSG

	g_reply_int=Mbfebillmsgreg_Ins(mbfebillmsgreg_v,g_pub_tx.reply);
	if ( g_reply_int )
	{
		sprintf(acErrMsg,"登记汇票全额兑付通知表数据库操作失败[%d]",g_reply_int);
		WRITEMSG
		return -1;
	}

	return 0;
}
