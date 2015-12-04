/***************************************************************
* 文 件 名: spD701.c
* 功能描述: 透支协议登记
* 作    者: jack
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

spD701()
{
	char tmp_ac[21];
	int ret=0,flag=0;

	struct	mdm_ac_rel_c	f_mdm_ac_rel;
	struct	od_dd_rel_c		g_od_dd_rel;
	struct	od_mst_c		f_od_mst;
	struct	od_mst_c		h_od_mst;
	struct	dd_mst_c		f_dd_mst;
	struct	cif_basic_inf_c	g_cif_basic_inf;

	memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
	memset(&f_dd_mst,0x00,sizeof(struct dd_mst_c));
	memset(&g_od_mst,0x00,sizeof(struct od_mst_c));
	memset(&f_od_mst,0x00,sizeof(struct od_mst_c));
	memset(&h_od_mst,0x00,sizeof(struct od_mst_c));
	memset(&g_od_parm,0x00,sizeof(struct od_parm_c));
	memset(&g_od_dd_rel,0x00,sizeof(struct od_dd_rel_c));
	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&f_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&g_cif_basic_inf,0x00,sizeof(struct cif_basic_inf_c));

	/*** 初始化全局变量 ***/
	pub_base_sysinit();

	/*取值*/
	get_zd_data("0300",g_pub_tx.ac_no);
	get_zd_data("0230",g_od_mst.prdt_no);
	get_zd_double("0390",&g_od_mst.max_bal);
	get_zd_double("0400",&g_od_mst.cash_max_bal);
	get_zd_double("0410",&g_od_mst.min_pay_amt);
	get_zd_double("0420",&g_od_mst.min_pay_ratio);
	g_od_mst.min_pay_ratio=g_od_mst.min_pay_ratio/100;
	get_zd_data("0310",tmp_ac);

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

	if(g_mdm_ac_rel.note_sts[0]!='0')
 	{
 		sprintf(acErrMsg,"该账户介质状态不正常");
  		WRITEMSG
		strcpy(g_pub_tx.reply,"D140");
    	goto ErrExit;
 	}

	if(g_mdm_ac_rel.coll_sts[0]!='0')
 	{
 		sprintf(acErrMsg,"该账户介质处于捡拾状态");
  		WRITEMSG
		strcpy(g_pub_tx.reply,"D141");
    	goto ErrExit;
 	}

	ret = Dd_mst_Dec_Upd(g_pub_tx.reply,"ac_id=%ld",g_mdm_ac_rel.ac_id);
 	if( ret )
 	{
 		sprintf(acErrMsg,"游标定义错误");
  		WRITEMSG
		strcpy(g_pub_tx.reply,"D101");
    	goto ErrExit;
 	}

	ret = Dd_mst_Fet_Upd(&g_dd_mst,g_pub_tx.reply);
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

	if(g_dd_mst.ac_sts[0]!='1')
 	{
 		sprintf(acErrMsg,"帐户状态不正常");
  		WRITEMSG
		strcpy(g_pub_tx.reply,"O085");
    	goto ErrExit;
 	}

	if(g_dd_mst.hold_sts[0]!='0')
 	{
 		sprintf(acErrMsg,"帐户被冻结");
  		WRITEMSG
		strcpy(g_pub_tx.reply,"O084");
    	goto ErrExit;
 	}

	strcpy(g_dd_mst.od_ind,"Y");			/*修改为有透支产品*/

	ret = Dd_mst_Upd_Upd(g_dd_mst,g_pub_tx.reply);
 	if( ret )
 	{
 		sprintf(acErrMsg,"修改活期文件错误");
  		WRITEMSG
		strcpy(g_pub_tx.reply,"D105");
    	goto ErrExit;
 	}

	Dd_mst_Clo_Upd();

	/*还款帐号检查*/
	ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&f_mdm_ac_rel,"ac_no='%s'",tmp_ac);
 	if( ret==100 )
 	{
 		sprintf(acErrMsg,"还款帐户不存在");
  		WRITEMSG
		strcpy(g_pub_tx.reply,"D137");
    	goto ErrExit;
 	}
	else if( ret )
	{
		sprintf(acErrMsg,"查找记录出错[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	if(f_mdm_ac_rel.note_sts[0]!='0')
 	{
 		sprintf(acErrMsg,"还款帐号介质状态不正常");
  		WRITEMSG
		strcpy(g_pub_tx.reply,"D139");
    	goto ErrExit;
 	}

	if(f_mdm_ac_rel.coll_sts[0]!='0')
 	{
 		sprintf(acErrMsg,"还款帐号介质处于捡拾状态");
  		WRITEMSG
		strcpy(g_pub_tx.reply,"D138");
    	goto ErrExit;
 	}

	ret = Dd_mst_Sel(g_pub_tx.reply,&f_dd_mst,"ac_id=%ld",f_mdm_ac_rel.ac_id);
 	if( ret==100 )
 	{
 		sprintf(acErrMsg,"还款帐号不存在");
  		WRITEMSG
		strcpy(g_pub_tx.reply,"D137");
    	goto ErrExit;
 	}
	else if( ret )
	{
		sprintf(acErrMsg,"查找记录出错[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	if(f_dd_mst.ac_sts[0]!='1')
 	{
 		sprintf(acErrMsg,"还款帐户状态不正常");
  		WRITEMSG
		strcpy(g_pub_tx.reply,"D136");
    	goto ErrExit;
 	}

	if(f_dd_mst.hold_sts[0]!='0')
 	{
 		sprintf(acErrMsg,"还款帐户已被冻结");
  		WRITEMSG
		strcpy(g_pub_tx.reply,"D135");
    	goto ErrExit;
 	}

	/*活期产品与透支产品关系是否存在*/
	ret = Od_dd_rel_Sel(g_pub_tx.reply,&g_od_dd_rel, \
						"dd_prdt_code='%s' and od_prdt_code='%s'", \
						g_dd_mst.prdt_no,g_od_mst.prdt_no);
 	if( ret==100 )
 	{
 		sprintf(acErrMsg,"活期产品和透支产品关系不存在");
  		WRITEMSG
		strcpy(g_pub_tx.reply,"D134");
    	goto ErrExit;
 	}
	else if( ret )
	{
		sprintf(acErrMsg,"查找记录出错[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	/*检查该账户是否已存在透支产品记录*/
	ret = Od_mst_Sel(g_pub_tx.reply,&f_od_mst,"ac_id=%ld and ac_seqn=%d", \
					 g_dd_mst.ac_id,g_dd_mst.ac_seqn);
 	if( !ret && f_od_mst.ac_sts[0]!='*' )flag=1;
	else if( !ret && f_od_mst.ac_sts[0]=='*' )flag=2;
	else if( ret&&ret!=100 )
 	{
 		sprintf(acErrMsg,"查询透支文件异常ret=[%d]",ret);
  		WRITEMSG
		strcpy(g_pub_tx.reply,"D133");
    	goto ErrExit;
 	}

	/*检查客户类型和限额*/
	ret =Od_parm_Sel(g_pub_tx.reply,&g_od_parm,"prdt_no='%s'",g_od_mst.prdt_no);
	if( ret==100 )
 	{
 		sprintf(acErrMsg,"该透支产品不存在");
  		WRITEMSG
		strcpy(g_pub_tx.reply,"D129");
    	goto ErrExit;
 	}
	else if( ret )
	{
		sprintf(acErrMsg,"查找记录出错[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	ret = Cif_basic_inf_Sel(g_pub_tx.reply,&g_cif_basic_inf,"cif_no=%ld", \
							g_dd_mst.cif_no);
	if(ret==100)
 	{
 		sprintf(acErrMsg,"不存在该客户");
  		WRITEMSG
		strcpy(g_pub_tx.reply,"C107");
    	goto ErrExit;
 	}
	else if( ret )
	{
		sprintf(acErrMsg,"查找记录出错[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	if(strcmp(g_cif_basic_inf.type,g_od_parm.cif_type))
 	{
 		sprintf(acErrMsg,"该客户类型不可登记此透支产品");
  		WRITEMSG
		strcpy(g_pub_tx.reply,"D143");
    	goto ErrExit;
 	}

	if(g_od_mst.max_bal>g_od_parm.max_amt)
 	{
 		sprintf(acErrMsg,"不可超过透支最高限额");
  		WRITEMSG
		strcpy(g_pub_tx.reply,"D132");
    	goto ErrExit;
 	}

	if(g_od_mst.cash_max_bal>g_od_parm.cash_amt)
 	{
 		sprintf(acErrMsg,"不可超过现金最高透支限额");
  		WRITEMSG
		strcpy(g_pub_tx.reply,"D131");
    	goto ErrExit;
 	}

	g_od_mst.ac_id=g_dd_mst.ac_id;
	g_od_mst.ac_seqn=g_dd_mst.ac_seqn;
	strcpy(g_od_mst.opn_br_no,g_pub_tx.tx_br_no);
	g_od_mst.bal=0.00;
	g_od_mst.drt_bal=0.00;
	g_od_mst.intst_amt=0.00;
	g_od_mst.intst_bal=0.00;
	/*g_od_mst.drt_intst_amt=0.00;*/
	g_od_mst.cash_bal=0.00;
	g_od_mst.ys_bal=0.00;
	g_od_mst.hst_bal=0.00;
	g_od_mst.intst_acm=0.00;
	g_od_mst.drt_intst_acm=0.00;
	g_od_mst.hst_cnt=0;
	g_od_mst.hst_pg=0;
	strcpy(g_od_mst.ac_sts,"1");
	g_od_mst.pay_ac_id=f_dd_mst.ac_id;
	g_od_mst.pay_ac_seqn=f_dd_mst.ac_seqn;
	strcpy(g_od_mst.mac,"");

	switch(flag)
	{
		case 0:
			/*登记透支文件*/
			ret=Od_mst_Ins(g_od_mst,g_pub_tx.reply);
			if(ret)
			{
				sprintf(acErrMsg,"登记透支文件失败!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"D142");
				goto ErrExit;
			}
			break;
		case 1:
			if( strncmp(f_od_mst.prdt_no,g_od_mst.prdt_no,3) )
			{
				sprintf(acErrMsg,"透支产品不得更改!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"D142");
				goto ErrExit;
			}

			g_reply_int = Od_mst_Dec_Upd(g_pub_tx.reply, \
										 "ac_id=%ld and ac_seqn=%d", \
			  							 g_dd_mst.ac_id,g_dd_mst.ac_seqn);
		  	if( g_reply_int )
			{
			   	sprintf(acErrMsg,"定义游标错误!");
				strcpy(g_pub_tx.reply, "P162");
				return -1;
			}

			g_reply_int = Od_mst_Fet_Upd(&h_od_mst,g_pub_tx.reply);
			if( g_reply_int==100 )
			{
				sprintf(acErrMsg,"记录不存在[%d]",ret);
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

			h_od_mst.max_bal=g_od_mst.max_bal;
			h_od_mst.cash_max_bal=g_od_mst.cash_max_bal;
			h_od_mst.pay_ac_id=g_od_mst.pay_ac_id;
			h_od_mst.pay_ac_seqn=g_od_mst.pay_ac_seqn;

			g_reply_int=Od_mst_Upd_Upd(h_od_mst,g_pub_tx.reply);
		  	if( g_reply_int )
			{
			   	sprintf(acErrMsg,"定义游标错误!");
				strcpy(g_pub_tx.reply, "P162");
				return -1;
			}

			Od_mst_Clo_Upd( );
			break;
		case 2:
			/*修改透支文件*/
			memset( &f_od_mst,0x00,sizeof(struct od_mst_c) );
			ret = Od_mst_Dec_Upd(g_pub_tx.reply,"ac_id=%ld and ac_seqn=%d", \
								 g_dd_mst.ac_id,g_dd_mst.ac_seqn);
 			if( ret )
 			{
 				sprintf(acErrMsg,"游标定义错误");
  				WRITEMSG
				strcpy(g_pub_tx.reply,"D101");
    			goto ErrExit;
 			}

			ret = Od_mst_Fet_Upd(&f_od_mst,g_pub_tx.reply);
 			if( ret==100 )
 			{
 				sprintf(acErrMsg,"帐号不存在于透支文件");
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

			ret=Od_mst_Upd_Upd(g_od_mst,g_pub_tx.reply);
 			if( ret )
 			{
 				sprintf(acErrMsg,"修改透支文件错误");
  				WRITEMSG
				strcpy(g_pub_tx.reply,"D105");
    			goto ErrExit;
 			}

			Od_mst_Clo_Upd();
			break;
		default:
 			sprintf(acErrMsg,"FLAG=[%d]值异常",flag);
  			WRITEMSG
			break;
	}

	/*登记透支历史文件
	ret = pubf_od_hst(g_od_mst,0.00,"1","透支协议登记");
	if( ret )
	{
		sprintf(acErrMsg,"登记透支历史文件失败!");
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
	sprintf(acErrMsg,"透支协议登记成功![%d]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"透支协议登记失败![%d]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
