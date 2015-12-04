/*************************************************
* 文 件 名:  sp8554.c
* 功能描述： 公司机构信息维护快捷查询
* 作    者:  lwb
* 完成日期： 2014年10月28日
insert into TX_DEF (TX_CODE, TX_NAME, BIT_MAP, LOG_IND, TX_TYPE, WRK_STS) values ('8554', '公司机构信息快查', '10000000000000000000000011100110000000000001100101010000000101100010001000000000111000000000000000000000000000000000000000000000', '0', '4', '2');
insert into TX_FLOW_DEF (TX_CODE, FLOW_CODE, SUB_TX_CODE, SUB_TX_RSLT) values ('8554', 0, '8554', '#$');
insert into TX_SUB_DEF (SUB_TX_CODE, SUB_TX_NAME, TX_FUNC, ACCT_IND, CHNL_IND) values ('8554', '公司机构信息快查', 'sp8554', '0', '0');

* 修改记录： 
* 日   期:
* 修 改 人:
* 修改内容:
*************************************************/

#include <string.h>

#define EXTERN
#include "public.h"
#include "cif_basic_inf_c.h"
#include "cif_copa_inf_c.h"

int sp8554()
{
	int iRet = 0;
	long iCif_no = 0;
	int iCop_no = 0;

		
	struct cif_copa_inf_c sCif_copa_inf;
	struct cif_basic_inf_c sCif_basic_inf;
	
	memset(&sCif_copa_inf,0x00,sizeof(sCif_copa_inf));
	memset(&sCif_basic_inf,0x00,sizeof(sCif_basic_inf));
	
	pub_base_sysinit();
	
	get_zd_long("0280",&iCif_no);
	
	iRet = Cif_basic_inf_Sel(g_pub_tx.reply,&sCif_basic_inf,"cif_no=%d ",iCif_no);
	if(iRet == 100 )
	{
		sprintf(acErrMsg,"客户基本信息表中无此客户号！");
		WRITEMSG
		strcpy(g_pub_tx.reply,"C051");
		goto ErrExit;			
	}
	else if( iRet )
	{
		sprintf( acErrMsg, "查询客户信息表异常!!ret=[%d]", iRet );
		WRITEMSG
		goto ErrExit;
	}
	
	/*开始查询公司机构信息表*/
	iRet = Cif_copa_inf_Sel(g_pub_tx.reply,&sCif_copa_inf,"cif_no = %d ",iCif_no);	
	if(iRet == 100)
	{
		sprintf(acErrMsg,"公司机构信息表中无此记录！");
		strcpy(g_pub_tx.reply,"H034");
		set_zd_data(DC_GET_MSG,acErrMsg);
		WRITEMSG
		goto ErrExit;				
	}
	else if( iRet )
	{
		sprintf( acErrMsg, "查询公司机构信息表异常!!ret=[%d]", iRet );
		strcpy(g_pub_tx.reply,"H034");
		set_zd_data(DC_GET_MSG,acErrMsg);
		WRITEMSG
		goto ErrExit;
	}
	
	set_zd_data("0250",sCif_copa_inf.sc_bs);
	set_zd_data("0260",sCif_copa_inf.con_en_name);	
	set_zd_data("0270",sCif_copa_inf.up_br_name);

	set_zd_data("0300",sCif_copa_inf.ag_cr_no);
	set_zd_data("0820",sCif_copa_inf.or_ag_type);
	set_zd_data("0480",sCif_copa_inf.or_ag_mst_type);
	set_zd_data("0580",sCif_copa_inf.eco_ind_type);
	set_zd_data("0500",sCif_copa_inf.eco_type);
	set_zd_data("0810",sCif_copa_inf.ent_tel);
	set_zd_data("0670",sCif_copa_inf.up_reg_type);
	set_zd_data("0310",sCif_copa_inf.up_reg_no);
	set_zd_data("0600",sCif_copa_inf.up_ag_zz_no);
	set_zd_data("0830",sCif_copa_inf.up_ag_cr_no);
	set_zd_data("0620",sCif_copa_inf.check_no);
	set_zd_data("0630",sCif_copa_inf.reg_ad_area_no);
	set_zd_long("0440",sCif_copa_inf.beg_date);
	set_zd_long("0450",sCif_copa_inf.end_date);
	set_zd_data("0710",sCif_copa_inf.cop_sts);
	vtcp_log("经济行业分类：%s\n",sCif_copa_inf.eco_ind_type);
	vtcp_log("行政区划：%s\n",sCif_copa_inf.reg_ad_area_no);

	
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




