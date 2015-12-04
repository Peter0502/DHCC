/*************************************************
* 文 件 名:  sp8553.c
* 功能描述： 关联企业信息维护快捷查询
* 作    者:  zj
* 完成日期： 2014年10月22日
insert into TX_DEF (TX_CODE, TX_NAME, BIT_MAP, LOG_IND, TX_TYPE, WRK_STS) values ('8553', '关联企业信息快查', '10000000000000000000000001000000000011000000000000000000000001000010100000000000000000000000000000000000000000000000000000000000', '0', '4', '2');
insert into TX_FLOW_DEF (TX_CODE, FLOW_CODE, SUB_TX_CODE, SUB_TX_RSLT) values ('8553', 0, '8553', '#$');
insert into TX_SUB_DEF (SUB_TX_CODE, SUB_TX_NAME, TX_FUNC, ACCT_IND, CHNL_IND) values ('8553', '关联企业信息快查', 'sp8553', '0', '0');

* 修改记录： 
* 日   期:
* 修 改 人:
* 修改内容:
*************************************************/

#include <string.h>

#define EXTERN
#include "public.h"
#include "cif_basic_inf_c.h"
#include "join_cop_inf_c.h"

int sp8553()
{
	int iRet = 0;
	long iCif_no = 0;
	int iCop_no = 0;

		
	struct join_cop_inf_c sJoin_cop_inf;
	struct cif_basic_inf_c sCif_basic_inf;
	
	memset(&sJoin_cop_inf,0x00,sizeof(sJoin_cop_inf));
	memset(&sCif_basic_inf,0x00,sizeof(sCif_basic_inf));
	
	pub_base_sysinit();
	
	get_zd_long("0280",&iCif_no);
	get_zd_int("0290",&iCop_no);
	
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
	
	/*开始查询关联企业信息表*/
	iRet = Join_cop_inf_Sel(g_pub_tx.reply,&sJoin_cop_inf,"cif_no = %d and cop_no = %d",iCif_no,iCop_no);	
	if(iRet == 100)
	{
		sprintf(acErrMsg,"关联企业信息表中无此记录！");
		WRITEMSG
		strcpy(g_pub_tx.reply,"");
		goto ErrExit;				
	}
	else if( iRet )
	{
		sprintf( acErrMsg, "查询关联企业信息表异常!!ret=[%d]", iRet );
		WRITEMSG
		goto ErrExit;
	}
	
	set_zd_data("0260",sJoin_cop_inf.name);
	set_zd_data("0670",sJoin_cop_inf.type);
	set_zd_data("0690",sJoin_cop_inf.id_type);
	set_zd_data("0620",sJoin_cop_inf.id_no);
	set_zd_data("0370",sJoin_cop_inf.br_no);
	set_zd_data("0380",sJoin_cop_inf.ag_cr_no);
	
	
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



