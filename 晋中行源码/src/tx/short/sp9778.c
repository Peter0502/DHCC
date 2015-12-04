/*************************************************
* 文 件 名:  sp9778.c
* 功能描述： 根据证券回购协议编号，记账金额，返回利息。
*
* 作    者:  
* 完成日期： 2003年3月30日 
*
* 修改记录： 
* 日   期:
* 修 改 人:
* 修改内容:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "mo_stok_rebuy_prot_c.h"

int sp9778()
{
	int ret=0;
	double lx=0.00;
	struct mo_stok_rebuy_prot_c sMo_stok_rebuy_prot,sMo_stok_rebuy_prot1;
	memset(&sMo_stok_rebuy_prot,0x0,sizeof(struct mo_stok_rebuy_prot_c));
	memset(&sMo_stok_rebuy_prot1,0x0,sizeof(struct mo_stok_rebuy_prot_c));

	get_zd_data("0860",sMo_stok_rebuy_prot.pact_no);
	get_zd_double("0400",&g_pub_tx.tx_amt1);
	get_zd_long(DC_DATE,&g_pub_tx.tx_date);        /* 交易日期 */

	ret = Mo_stok_rebuy_prot_Sel(g_pub_tx.reply,&sMo_stok_rebuy_prot,
	 "pact_no='%s' and sts='2'",sMo_stok_rebuy_prot.pact_no);
	if (ret != 0)
	{
		sprintf(acErrMsg,"查询证券回购协议登记簿错误!! [%d]",ret);
		WRITEMSG
		goto ErrExit;
	}

	/* 检查证券回购借出账户余额和交易金额的大小 */
	if ( pub_base_CompDblVal(sMo_stok_rebuy_prot.bal,g_pub_tx.tx_amt1)<0 )
	{
		strcpy( g_pub_tx.reply, "B325" );
		sprintf(acErrMsg,"交易金额大于证券回购借出帐户余额!!");
		WRITEMSG
		goto ErrExit;
	}

	ret=In_mst_Sel(g_pub_tx.reply,&g_in_mst,"ac_id='%d' ",sMo_stok_rebuy_prot.ac_id);
	if( ret )
	{
		sprintf(acErrMsg,"账号不存在!!");
		WRITEMSG
		goto ErrExit;
	}
	ret=In_parm_Sel(g_pub_tx.reply,&g_in_parm,"prdt_no='%s' ",g_in_mst.prdt_no);
	if( ret )
	{
		sprintf(acErrMsg,"产品参数不存在!!");
		WRITEMSG
		goto ErrExit;
	}

	/*** 调用计算利息积数函数 ***/
	if( pub_base_CalAcm(g_in_mst.lst_date,g_pub_tx.tx_date,
		g_in_parm.cal_day_type,g_in_parm.acm_type,
		g_in_mst.bal,-g_pub_tx.tx_amt1,&g_in_mst.intst_acm,
		g_in_parm.intst_knd,g_in_parm.intst_mon,g_in_parm.intst_date) )
	{
		sprintf(acErrMsg,"计算利息积数错误!");
		WRITEMSG
		goto ErrExit;
	}

	/*** 计算应还利息 ***/
	lx = g_in_mst.rate * g_in_mst.intst_acm / L360 / L100;
	lx = pub_base_PubDround_1(lx);
	if ( pub_base_CompDblVal(lx,0.00)<0 )
	{
		sprintf(acErrMsg,"计算利息错误!");
		WRITEMSG
		goto ErrExit;
	}

	set_zd_double("0410",lx);
	set_zd_double("0420",g_in_mst.bal);

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"success !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	set_zd_double("0410",0.00);
	sprintf(acErrMsg,"fail !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
