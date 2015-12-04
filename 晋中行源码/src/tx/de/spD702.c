/***************************************************************
* 文 件 名: spD702.c
* 功能描述: 透支协议取消
* 作    者: mike
* 完成日期：2003年3月31日
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
#include "od_dd_rel_c.h"
#include "cif_basic_inf_c.h"

spD702()
{
	char tmp_ac[21];
	int ret=0;

	struct	od_dd_rel_c		g_od_dd_rel;
	struct	od_mst_c		f_od_mst;
	struct	cif_basic_inf_c	g_cif_basic_inf;

	memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
	memset(&g_od_mst,0x00,sizeof(struct od_mst_c));
	memset(&f_od_mst,0x00,sizeof(struct od_mst_c));
	memset(&g_od_parm,0x00,sizeof(struct od_parm_c));
	memset(&g_od_dd_rel,0x00,sizeof(struct od_dd_rel_c));
	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&g_cif_basic_inf,0x00,sizeof(struct cif_basic_inf_c));

	/*** 初始化全局变量 ***/
	pub_base_sysinit();

	/*** 取值 ***/
	get_zd_data("0300",g_pub_tx.ac_no);

	/*帐号检查*/
	ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'", \
						 g_pub_tx.ac_no);
 	if( ret==100 )
 	{
 		sprintf(acErrMsg,"该帐户不存在");
  		WRITEMSG
		strcpy(g_pub_tx.reply,"P102");
    	goto ErrExit;
 	}
	else if( ret )
	{
		sprintf(acErrMsg,"查找记录出错[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	/*查询活期文件*/
	ret = Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,"ac_id=%ld",g_mdm_ac_rel.ac_id);
 	if( ret==100 )
 	{
 		sprintf(acErrMsg,"帐号不存在于活期文件");
  		WRITEMSG
		strcpy(g_pub_tx.reply,"P102");
    	goto ErrExit;
 	}
	else if( ret )
	{
		sprintf(acErrMsg,"查找记录出错[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	/*查询透支帐单文件*/
	ret = sql_count("od_bill","ac_id=%ld and ac_seqn=%d and pay_sts='N'", \
					g_dd_mst.ac_id,g_dd_mst.ac_seqn);
 	if( ret>0 )
 	{
 		sprintf(acErrMsg,"该账户还有透支金额未还清");
  		WRITEMSG
		strcpy(g_pub_tx.reply,"D153");
    	goto ErrExit;
 	}
	else if(ret<0)
 	{
 		sprintf(acErrMsg,"查询透支帐单异常");
  		WRITEMSG
    	goto ErrExit;
 	}

	/*查询账户透支文件*/
	ret = Od_mst_Sel(g_pub_tx.reply,&g_od_mst,"ac_id=%ld and ac_seqn=%d", \
					 g_dd_mst.ac_id,g_dd_mst.ac_seqn);
 	if( ret==100 )
 	{
 		sprintf(acErrMsg,"该账户没有透支产品");
  		WRITEMSG
		strcpy(g_pub_tx.reply,"D148");
    	goto ErrExit;
 	}
	else if( ret )
	{
		sprintf(acErrMsg,"查找记录出错[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	if( g_od_mst.ac_sts[0]=='*'&&g_dd_mst.od_ind[0]=='N' )
 	{
 		sprintf(acErrMsg,"该账户已取消透支登记");
  		WRITEMSG
		strcpy(g_pub_tx.reply,"D150");
    	goto ErrExit;
 	}

	ret = pub_base_CompDblVal(g_od_mst.intst_acm,0.00);
	if( ret )
 	{
 		sprintf(acErrMsg,"该账户还有利息积数");
  		WRITEMSG
		strcpy(g_pub_tx.reply,"D147");
    	goto ErrExit;
 	}

	ret = pub_base_CompDblVal(g_od_mst.bal,0.00);
	if( ret )
 	{
 		sprintf(acErrMsg,"该账户还有透支余额");
  		WRITEMSG
		strcpy(g_pub_tx.reply,"D146");
    	goto ErrExit;
 	}

	ret = pub_base_CompDblVal(g_od_mst.intst_bal,0.00);
	if( ret )
 	{
 		sprintf(acErrMsg,"该账户还有透支利息余额");
  		WRITEMSG
		strcpy(g_pub_tx.reply,"D145");
    	goto ErrExit;
 	}

	/*修改透支文件为销户*/
	g_reply_int = Od_mst_Dec_Upd(g_pub_tx.reply,"ac_id=%ld and ac_seqn=%d", \
								 g_dd_mst.ac_id,g_dd_mst.ac_seqn);
	if( g_reply_int )
	{
	   	sprintf(acErrMsg,"定义游标错误!");
		strcpy(g_pub_tx.reply, "P162");
		return -1;
	}

	g_reply_int = Od_mst_Fet_Upd(&f_od_mst,g_pub_tx.reply);
	if( g_reply_int==100 )
	{
		sprintf(acErrMsg,"账号不存在[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}
	else if( g_reply_int )
	{
	  	sprintf(acErrMsg,"取记录错误!");
		strcpy(g_pub_tx.reply, "P162");
		return -1;
	}

	strcpy(f_od_mst.ac_sts,"*");

	g_reply_int = Od_mst_Upd_Upd(f_od_mst,g_pub_tx.reply);
  	if( g_reply_int )
	{
	   	sprintf(acErrMsg,"定义游标错误!");
		strcpy(g_pub_tx.reply, "P162");
		return -1;
	}

	Od_mst_Clo_Upd( );

	/*修改活期文件为无透支产品*/
	g_reply_int = Dd_mst_Dec_Upd(g_pub_tx.reply,"ac_id=%ld",g_mdm_ac_rel.ac_id);
	if( g_reply_int )
	{
	   	sprintf(acErrMsg,"定义游标错误!");
		strcpy(g_pub_tx.reply, "P162");
		return -1;
	}

	g_reply_int = Dd_mst_Fet_Upd(&g_dd_mst,g_pub_tx.reply);
	if( g_reply_int==100 )
	{
		sprintf(acErrMsg,"账号不存在[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}
	else if(g_reply_int )
	{
	  	sprintf(acErrMsg,"取记录错误!");
		strcpy(g_pub_tx.reply, "P162");
		goto ErrExit;
	}

	strcpy(g_dd_mst.od_ind,"N");

	/*--- g_reply_int=Dd_mst_Upd_Upd(g_od_mst,g_pub_tx.reply); ---*/
	g_reply_int = Dd_mst_Upd_Upd(g_dd_mst,g_pub_tx.reply);
  	if( g_reply_int )
	{
	   	sprintf(acErrMsg,"定义游标错误!");
		strcpy(g_pub_tx.reply, "P162");
		goto ErrExit;
	}

	Dd_mst_Clo_Upd( );

	/*登记透支历史文件
	ret = pubf_od_hst(&g_od_mst,0.00,"2","透支协议取消");
	if( ret )
	{
		sprintf(acErrMsg,"登记透支明细文件失败!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"T028");
		goto ErrExit;
	}
	*/

	if( pub_ins_trace_log() )
	{
	  	sprintf(acErrMsg,"登记交易流水错误!");
		WRITEMSG
	 	goto ErrExit;
	}

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"透支协议取消成功![%d]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"透支协议取消失败![%d]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
