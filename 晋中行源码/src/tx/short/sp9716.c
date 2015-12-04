/*************************************************
* 文 件 名: sp9716.c
* 功能描述：根据帐号回显相关信息[透支协议登记]
*
* 作    者: rob
* 完成日期：2003年03月31日
* 修改记录： 
*     1. 日    期:
*        修 改 人:
*        修改内容:
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#define EXTERN
#include "public.h"
#include "prdt_ac_id_c.h"
#include "cif_basic_inf_c.h"

sp9716()  
{ 		
	int ret=0;
	char tmp_ratio[21];

	struct cif_basic_inf_c	g_cif_basic_inf;
	struct prdt_ac_id_c	g_prdt_ac_id;

	memset(&g_od_parm,0x00,sizeof(struct od_parm_c));
	memset(&g_cif_basic_inf,0x00,sizeof(struct cif_basic_inf_c));

	/*系统初始化（初始化g_pub_tx)数据初始化*/
	pub_base_sysinit();

	/** 取值、赋值 **/	
	get_zd_data("0300",g_pub_tx.ac_no);						/*帐号*/
	pub_base_old_acno( g_pub_tx.ac_no );

	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel, \
	   					"ac_no='%s'",g_pub_tx.ac_no);
	if( ret==100 )
	{
		sprintf(acErrMsg,"该帐户不存在");
	 	WRITEMSG
		strcpy(g_pub_tx.reply,"P102");
		goto ErrExit;
	}

	ret=Prdt_ac_id_Sel(g_pub_tx.reply,&g_prdt_ac_id,
					 "ac_id=%ld",g_mdm_ac_rel.ac_id);
	if( ret==100 )
	{
	 	sprintf(acErrMsg,"不存在该账户!!ac_id=[%ld],ac_seqn=[%d]",
	 			g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_seqn);
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}

	ret=Cif_basic_inf_Sel(g_pub_tx.reply,&g_cif_basic_inf,
						 "cif_no=%ld",g_prdt_ac_id.cif_no);
	if( ret==100 )
	{
		sprintf(acErrMsg,"不存在该客户号!!cif_no=[%ld]",g_prdt_ac_id.cif_no);
		WRITEMSG
	 	strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}

	set_zd_data("0250",g_cif_basic_inf.name);

 	ret=Od_mst_Sel(g_pub_tx.reply,&g_od_mst,"ac_id=%ld and ac_seqn=%d \
			and ac_sts<>'*'", g_prdt_ac_id,g_prdt_ac_id.ac_seqn);
		sprintf(acErrMsg,"ret=[%d][%ld]",ret, g_prdt_ac_id );
		WRITEMSG
	if(!ret)
	{
 		ret=Od_parm_Sel(g_pub_tx.reply,&g_od_parm,"prdt_no='%s'", \
						g_od_mst.prdt_no);
		if( ret==100 )
		{
			sprintf(acErrMsg,"产品不存在g_od_mst.prdt_no=[%s]",g_od_mst.prdt_no);
			WRITEMSG
	 		strcpy(g_pub_tx.reply,"M003");
			goto ErrExit;
		}

		ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_id=%ld", \
							g_od_mst.pay_ac_id);
		if( ret==100 )
		{
			sprintf(acErrMsg,"帐号不存在g_od_mst.pay_ac_id=[%ld]", \
					g_od_mst.prdt_no);
			WRITEMSG
	 		strcpy(g_pub_tx.reply,"M003");
			goto ErrExit;
		}

		sprintf(acErrMsg,"ret=[%d]",ret);
		WRITEMSG
		set_zd_data("0230",g_od_mst.prdt_no);				/*透支产品编号*/
		set_zd_data("0240",g_od_mst.prdt_no);				
		set_zd_data("0260",g_od_parm.title);				/*透支产品名称*/
		set_zd_double("0390",g_od_mst.max_bal);				/*最大透支限额*/
		set_zd_double("0400",g_od_mst.cash_max_bal);		/*现金透支限额*/
		set_zd_double("0410",g_od_mst.min_pay_amt);			/*最低还款金额*/
		g_od_mst.min_pay_ratio=g_od_mst.min_pay_ratio*100;
		set_zd_double("0420",g_od_mst.min_pay_ratio);		/*最低还款比例*/
		set_zd_data("0310",g_mdm_ac_rel.ac_no);				/*还款帐号*/
	}

	set_zd_data("0300",g_pub_tx.ac_no);
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
   
