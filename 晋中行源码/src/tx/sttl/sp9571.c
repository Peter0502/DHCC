/*************************************************
* 文 件 名: sp9571.c
* 功能描述：仿真电票承兑审批
*
* 作    者: rob
* 完成日期：2003年03月31日
* 修改记录： 
*     1. 日    期:
*        修 改 人:
*        修改内容:
insert into tx_def values ('9571','模拟电票承兑审批','10000000000000000000000000000000000000000000000000000000010000000000000000000000000000000000000000000000000000000000000000000000','1','3','2')
insert into tx_flow_def  values ('9571',0,'9571','#$')
insert into tx_sub_def values ('9571','模拟电票承兑审批','sp9571','0','0')
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#define EXTERN
#include "public.h"
static struct {
	char tx_code[4];
	char tx_br_no[5];
	char tel[4];
	char pact_no[30];
	char pact_seqn[4];
	char voc_num[30];
	char idno[30];
	char op_type[1];
	char isseamt[20];
	char amt[20];
	char cif_no[9];
	char cif_name[60];
	char drwr_acno[32];
	char pyee_acno[32];
	char pyee_name[60];
	char id_type[1];
	char rate[20];
	char over_rate[20];
	char opn_date[8];
	char mtr_date[8];
	char pad[900];
}fd123;
sp9571()  
{ 		
	int ret=0;
	char pact_no[128];
	char sign_amt[128];
	char payer_ac_no[128];
	char payer_br_no[128];
	char payee_ac_no[128];
	char bail_ac_no[128];
	char bail_amt[128];
	char acc_po_sts[128];
	char acc_po_ind[128];
	char acc_type[128];
	memset(&fd123,'\0',sizeof(fd123));	
	get_fd_data("1230",&fd123);
	memset(pact_no,'\0',128);
	memset(sign_amt,'\0',128);
	memset(payer_ac_no,'\0',128);
	memset(payer_br_no,'\0',128);
	memset(payee_ac_no,'\0',128);
	memset(bail_ac_no,'\0',128);
	memset(bail_amt,'\0',128);
	memset(acc_po_sts,'\0',128);
	memset(acc_po_ind,'\0',128);
	memset(acc_type,'\0',128);
	memcpy(pact_no,fd123.pact_no,sizeof(fd123.pact_no));	
	pub_base_strpack(pact_no);
	memcpy(sign_amt,fd123.amt,sizeof(fd123.amt));	
	pub_base_strpack(sign_amt);
	memcpy(payer_ac_no,fd123.drwr_acno,sizeof(fd123.drwr_acno));	
	pub_base_strpack(payer_ac_no);
	memcpy(payer_br_no,fd123.tx_br_no,sizeof(fd123.tx_br_no));	
	pub_base_strpack(payer_br_no);

	ret=sql_execute("insert into mo_bank_acc_po (pact_no,sign_amt,payer_ac_no,payer_br_no,payee_ac_no ,bail_ac_no,bail_amt,acc_po_sts,acc_po_ind,acc_type) values ('%s',%.2f,'%s','%s','%s','5004617600025',%.2f,'1','0','3')",pact_no,atof(sign_amt),payer_ac_no,payer_br_no,payee_ac_no,atof(sign_amt));
	if(ret){
		vtcp_log("执行插入语句错误:%d",ret);
		strcpy(g_pub_tx.reply,"E001");
		goto ErrExit;
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
   
