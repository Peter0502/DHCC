/*************************************************
* 文 件 名: sp9717.c
* 功能描述：根据产品编号回显相关参数[透支协议登记]
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

sp9717()  
{ 		
	int ret=0;
	char tmp_ratio[21],tmp_prdt[4],tmp_title[41];

	struct prdt_ac_id_c	g_prdt_ac_id;

	memset(&g_od_parm,0x00,sizeof(struct od_parm_c));
	memset(&g_od_mst,0x00,sizeof(struct od_mst_c));
	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));

	/*系统初始化（初始化g_pub_tx)数据初始化*/
	pub_base_sysinit();

	/** 取值、赋值 **/	
	get_zd_data("0230",g_pub_tx.prdt_code);						/*产品便号*/
	get_zd_data("0240",tmp_prdt);						

	get_zd_data("0260",tmp_title);								/*产品名称*/
	get_zd_double("0390",&g_od_mst.max_bal);					/*透支限额*/
	get_zd_double("0400",&g_od_mst.cash_max_bal);				/*现金限额*/
	get_zd_double("0410",&g_od_mst.max_bal);					/*最低还款金额*/
	get_zd_double("0420",&g_od_mst.cash_max_bal);				/*最低还款比例*/
	get_zd_data("0310",g_mdm_ac_rel.ac_no);             		/*还款帐号*/

	if(!strlen(tmp_prdt))
	{
 		ret=Od_parm_Sel(g_pub_tx.reply,&g_od_parm,"prdt_no='%s'", \
						g_pub_tx.prdt_code);
		if( ret==100 )
		{
			sprintf(acErrMsg,"产品不存在");
			WRITEMSG
	 		strcpy(g_pub_tx.reply,"M003");
			goto ErrExit;
		}

		set_zd_data("0260",g_od_parm.title);				/*透支产品名称*/
		set_zd_double("0390",g_od_parm.max_amt);			/*最大透支限额*/
		set_zd_double("0400",g_od_parm.cash_amt);			/*现金透支限额*/
		set_zd_double("0410",g_od_parm.min_pay_amt);		/*最低还款金额*/
		g_od_parm.min_pay_ratio=g_od_parm.min_pay_ratio*100;
		set_zd_double("0420",g_od_parm.min_pay_ratio);		/*最低还款比例*/
	}
	else if(strlen(tmp_prdt)&&strcmp(g_pub_tx.prdt_code,tmp_prdt))
	{
		sprintf(acErrMsg,"产品类型不可更换,请选择下列参数");
	 	WRITEMSG
		strcpy(g_pub_tx.reply,"O158");
		goto ErrExit;
	}
	else if(!strcmp(g_pub_tx.prdt_code,tmp_prdt))
	{
		sprintf(acErrMsg,"tmp_title=[%s]",tmp_title);
	 	WRITEMSG
		set_zd_data("0260",g_od_parm.title);				/*产品名称*/
		set_zd_double("0390",g_od_mst.max_bal);				/*透支限额*/
		set_zd_double("0400",g_od_mst.cash_max_bal);		/*现金限额*/
		set_zd_double("0410",g_od_mst.max_bal);				/*最低还款金额*/
		set_zd_double("0420",g_od_mst.cash_max_bal);		/*最低还款比例*/
		set_zd_data("0310",g_mdm_ac_rel.ac_no);            	/*还款帐号*/
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
   
