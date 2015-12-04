/*************************************************
* 文 件 名:  sp8551.c
* 功能描述： 公司股东信息维护快捷查询
* 作    者:  zj
* 完成日期： 2014年10月20日
insert into TX_DEF (TX_CODE, TX_NAME, BIT_MAP, LOG_IND, TX_TYPE, WRK_STS) values ('8551', '公司股东信息快查', '10000000000000000000000001000000000011010000000000000000000001000011100000000000000000000000000000000000000000000000000000000000', '0', '4', '2');
insert into TX_FLOW_DEF (TX_CODE, FLOW_CODE, SUB_TX_CODE, SUB_TX_RSLT) values ('8551', 0, '8551', '#$');
insert into TX_SUB_DEF (SUB_TX_CODE, SUB_TX_NAME, TX_FUNC, ACCT_IND, CHNL_IND) values ('8551', '公司股东信息快查', 'sp8551', '0', '0');

* 修改记录： 
* 日   期:
* 修 改 人:
* 修改内容:
*************************************************/

#include <string.h>

#define EXTERN
#include "public.h"
#include "cif_basic_inf_c.h"
#include "cif_shareholder_inf_c.h"

int sp8551()
{
	int iRet = 0;
	long iCif_no = 0;
	int iShareholder_no = 0;

		
	struct cif_shareholder_inf_c sCif_shareholder_inf;
	struct cif_basic_inf_c sCif_basic_inf;
	
	memset(&sCif_shareholder_inf,0x00,sizeof(sCif_shareholder_inf));
	memset(&sCif_basic_inf,0x00,sizeof(sCif_basic_inf));
	
	pub_base_sysinit();
	
	get_zd_long("0280",&iCif_no);
	get_zd_int("0290",&iShareholder_no);
	
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
	
	/*开始查询公司股东信息表*/
	iRet = Cif_shareholder_inf_Sel(g_pub_tx.reply,&sCif_shareholder_inf,"cif_no = %d and shareholder_no = %d",iCif_no,iShareholder_no);	
	if(iRet == 100)
	{
		sprintf(acErrMsg,"公司股东信息表中无此记录！");
		WRITEMSG
		strcpy(g_pub_tx.reply,"");
		goto ErrExit;				
	}
	else if( iRet )
	{
		sprintf( acErrMsg, "查询公司股东信息表异常!!ret=[%d]", iRet );
		WRITEMSG
		goto ErrExit;
	}
	
	set_zd_data("0260",sCif_shareholder_inf.name);
	set_zd_data("0670",sCif_shareholder_inf.type);
	set_zd_data("0680",sCif_shareholder_inf.sex);
	set_zd_data("0690",sCif_shareholder_inf.id_type);
	set_zd_data("0620",sCif_shareholder_inf.id_no);
	set_zd_data("0400",sCif_shareholder_inf.rate);
	set_zd_data("0370",sCif_shareholder_inf.br_no);
	set_zd_data("0380",sCif_shareholder_inf.ag_cr_no);
	
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

