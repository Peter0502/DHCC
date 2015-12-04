/*************************************************
* 文 件 名:  sp9760.c
* 功能描述： 大额支付快捷查询
*			 当选择转账汇款时根据帐号和金额在汇兑登记簿(mbfeghvsendjrn)
*			 中取一条记录，反显户名和地址
*
* 作    者:  jane
* 完成日期： 2003年6月3日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#define EXTERN
#include "public.h"
#include "mbfe_pub.h"
#include "prdt_ac_id_c.h"
#include "cif_addr_inf_c.h"

int sp9760()
{
	char remit_flag[2];
	char oprttype[3];
	struct mbfeghvsendjrn_c mbfeghvsendjrn_v;
	struct mdm_ac_rel_c mdm_ac_rel_v;
	struct prdt_ac_id_c prdt_ac_id_v;
	struct cif_addr_inf_c cif_addr_inf_v;

	memset(&mbfeghvsendjrn_v,0x00,sizeof(struct mbfeghvsendjrn_c));
	memset(&mdm_ac_rel_v,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&prdt_ac_id_v,0x00,sizeof(struct prdt_ac_id_c));
	memset(&cif_addr_inf_v,0x00,sizeof(struct cif_addr_inf_c));

	get_zd_char("0670",remit_flag);
	get_zd_data("0300",mbfeghvsendjrn_v.payeracc);
	get_zd_double("0390",&mbfeghvsendjrn_v.amount);

	if( remit_flag[0]=='2' )
		goto OkExit;

	pub_base_sysinit();

	sprintf(acErrMsg, 
		"payeracc='%s' and amount='%f' and procstate='0' and tx_br_no='%s'",
		mbfeghvsendjrn_v.payeracc, mbfeghvsendjrn_v.amount, g_pub_tx.tx_br_no);
		WRITEMSG
	g_reply_int=Mbfeghvsendjrn_Dec_Sel( g_pub_tx.reply, 
		"payeracc='%s' and amount='%f' and procstate='0' and tx_br_no='%s'",
		mbfeghvsendjrn_v.payeracc, mbfeghvsendjrn_v.amount, g_pub_tx.tx_br_no);
	if(g_reply_int)
	{
		sprintf( acErrMsg, "查询汇兑登记簿数据库操作声明游标出错[%d]",g_reply_int );
		MBFEWRITEMSG
		goto ErrExit;
	}

	g_reply_int=Mbfeghvsendjrn_Fet_Sel(&mbfeghvsendjrn_v, g_pub_tx.reply);
	if ( g_reply_int==100 )
	{
		sprintf( acErrMsg, "没有登记该笔汇款记录!");
		MBFEWRITEMSG
		goto ErrExit;
	}
	else if(g_reply_int)
	{
		sprintf( acErrMsg, "查询汇款登记簿数据库操作取游标出错[%d]",g_reply_int );
		MBFEWRITEMSG
		goto ErrExit;
	}

	Mbfeghvsendjrn_Clo_Sel();

	set_zd_data("0250",mbfeghvsendjrn_v.payername);

	g_reply_int=Mdm_ac_rel_Sel( g_pub_tx.reply, &mdm_ac_rel_v, "ac_no='%s'", mbfeghvsendjrn_v.payeracc);
	if(g_reply_int==100)
		goto OkExit;
	else if(g_reply_int)
	{
		sprintf( acErrMsg, "查询帐号介质对照表数据库操作出错[%d]",g_reply_int );
		MBFEWRITEMSG
		goto ErrExit;
	}

	g_reply_int=Prdt_ac_id_Sel( g_pub_tx.reply , &prdt_ac_id_v , "ac_id=%ld" , 
							  mdm_ac_rel_v.ac_id);
	if(g_reply_int==100)
		goto OkExit;
	else if(g_reply_int)
	{
		sprintf( acErrMsg, "查询产品账号关系视图表数据库操作出错[%d]",g_reply_int );
		MBFEWRITEMSG
	    goto ErrExit;
	}
	
	g_reply_int=Cif_addr_inf_Sel( g_pub_tx.reply, &cif_addr_inf_v,
		"cif_no='%ld' and addr_seqn=1", prdt_ac_id_v.cif_no);
	if(g_reply_int==100)
		goto OkExit;
	else if(g_reply_int)
	{
		sprintf( acErrMsg, "查询客户地址信息表数据库操作出错[%d]",g_reply_int );
		MBFEWRITEMSG
		goto ErrExit;
	}

	set_zd_data("0260",cif_addr_inf_v.addr);

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
