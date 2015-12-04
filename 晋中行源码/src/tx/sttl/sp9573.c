/*************************************************
* 文 件 名: sp9573.c
* 功能描述：仿真电票转贴现审批
*
* 作    者: rob
* 完成日期：2003年03月31日
* 修改记录： 
*     1. 日    期:
*        修 改 人:
*        修改内容:
insert into tx_def values ('9573','模拟电票转贴现审批','10000000000000000000000000000000000000000000000000000000010000000000000000000000000000000000000000000000000000000000000000000000','1','3','2')
insert into tx_flow_def  values ('9573',0,'9573','#$')
insert into tx_sub_def values ('9573','模拟电票转贴现审批','sp9573','0','0')
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
sp9573()  
{ 		
	int ret=0;
	char pnote_no[128];
	char not_matr_date[128];
	char par_amt[128];
	char rem_ac_no[128];
	char br_no[128];
	char pn_ac_no[128];
	char pact_no[128];
	char ac_type[128];
	char dis_type[128];
	memset(&fd123,'\0',sizeof(fd123));	
	get_fd_data("1230",&fd123);
	memset(pnote_no,'\0',128);
	memset(not_matr_date,'\0',128);
	memset(par_amt,'\0',128);
	memset(rem_ac_no,'\0',128);
	memset(br_no,'\0',128);
	memset(pn_ac_no,'\0',128);
	memset(pact_no,'\0',128);
	memset(ac_type,'\0',128);
	memset(dis_type,'\0',128);
	memcpy(pnote_no,fd123.idno,sizeof(fd123.idno));	
	pub_base_strpack(pnote_no);
	memcpy(not_matr_date,fd123.mtr_date,sizeof(fd123.mtr_date));	
	pub_base_strpack(not_matr_date);
	memcpy(par_amt,fd123.isseamt,sizeof(fd123.isseamt));	
	pub_base_strpack(par_amt);
	memcpy(rem_ac_no,fd123.drwr_acno,sizeof(fd123.drwr_acno));	
	pub_base_strpack(rem_ac_no);
	memcpy(br_no,fd123.tx_br_no,sizeof(fd123.tx_br_no));	
	pub_base_strpack(br_no);
	get_zd_data("0300",pn_ac_no);
	memcpy(pact_no,fd123.pact_no,sizeof(fd123.pact_no));	
	pub_base_strpack(pact_no);
	get_zd_data("0670",ac_type);
	get_zd_data("0680",dis_type);
	ret=sql_execute("update mo_discnt set sts='*' where pnote_no='%s'and sts='X'",pnote_no);
	ret=sql_execute("insert into mo_discnt (pnote_no,note_type,not_matr_date,sts,par_amt,rem_ac_no,br_no,po_knd,pn_ac_no,pact_no,pn_type,ac_type,pn_rate) values('%s','111','%s','X',%.2f,'%s','%s','E','%s','%s','%s','%s',6)",pnote_no,not_matr_date,atof(par_amt),rem_ac_no,br_no,pn_ac_no,pact_no,dis_type,ac_type);
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
   
