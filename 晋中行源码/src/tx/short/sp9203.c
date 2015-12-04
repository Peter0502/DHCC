/*************************************************
* 文 件 名:  sp9203.c
* 功能描述： 银行承兑汇票查询
*
* 作    者:  
* 完成日期： 
*
* 修改记录： 
* 日   期:   20081126
* 修 改 人:  
* 修改内容:
* 交易表数据：
------------------------------
tx_def
	insert into tx_def values('9203', '查询银行承兑汇票',
	'10000000000000000000000000000000000000010001110000000000000000000010110000000000000000000010000000000000000000000000010000000000',
	'0', '4', '0')	
-------------------------------	
tx_flow_def
	insert into tx_flow_def values('9203', '0', '9203', '#$')
-------------------------------
tx_sub_def	
	insert into tx_sub_def values('9203','银行承兑汇票查询', 'sp9203','0','0')
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "mo_bank_po_c.h"
#include "td_mst_c.h"

int sp9203()
{
       	int ret=0;
       	char  cTmp[20];	
       	char cType[2];
       	struct mo_bank_po_c mo_bank_po;
       	struct td_mst_c td_mst;
       	
       	memset(cType, 0x00, sizeof(cType));
	memset(cTmp, 0x00, sizeof(cTmp));
       /*memset(td_mst, 0x00, sizeof(struct td_mst_c));*/
       	memset(&mo_bank_po, 0x00, sizeof(struct mo_bank_po_c));
       
       	vtcp_log("进入9203交易\n");
       
       /*get_zd_data("0680", cType);*/
       	get_zd_data("0370",cTmp);
	pub_base_strpack(cTmp);
	

	vtcp_log("查询银行承兑汇票\n");
	ret = Mo_bank_po_Sel(g_pub_tx.reply, &mo_bank_po, " po_no = '%s'", cTmp);
	if(ret && ret!=100)
	{
		sprintf(acErrMsg,"查询银行承兑汇票登记簿出错!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"B060");
		goto ErrExit;
	}
	else if(ret==100)	
	{
		if(strlen(cTmp) <= 8)
		{
			sprintf(acErrMsg,"没有此承兑汇票, 可能为老系统汇票号!");
			WRITEMSG
			
			ret = Mo_bank_po_Sel(g_pub_tx.reply, &mo_bank_po, " po_no like '%%%s' ", cTmp);
			if(ret && ret!=100)
			{
				sprintf(acErrMsg,"查询银行承兑汇票登记簿出错!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"B060");
				goto ErrExit;
			}
			else if(ret==100)	
			{
				sprintf(acErrMsg,"没有此承兑汇票!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"B061");
				goto ErrExit;			
			}
		}
		else
		{
			sprintf(acErrMsg,"没有此承兑汇票号!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"B061");
			goto ErrExit;	
		}
	}       	
	set_zd_data("0910", mo_bank_po.sign_br_no);		/*签发行行号*/
	set_zd_long("0440", mo_bank_po.sign_date);		/*签发日期*/
	set_zd_long("0450", mo_bank_po.receive_date);		/*托收日期*/
	set_zd_long("0460", mo_bank_po.cash_date);		/*核销日期*/
	set_zd_double("0400", mo_bank_po.po_amt);		/*票面金额*/
	set_zd_data("0670", mo_bank_po.at_term_ind);		/*票据到期标志*/
	set_zd_data("0690", mo_bank_po.po_sts);		/*票据到期标志*/
	set_zd_data("0700", mo_bank_po.po_ind);	/*汇票挂失标志 0 挂失 1正常*/

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
