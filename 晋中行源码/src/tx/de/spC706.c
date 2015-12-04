/*************************************************
* 文 件 名:    spC706.c
* 功能描述：   完成IC卡的电子钱包账号的取款交易（圈提）。
* 作    者:    wzw
* 完成日期：   2011年12月15日
insert into tx_def      values('3706','IC卡圈提','10100000000100000000000000000000000000000000000000000000000000000000000000000000000000000000000000001000000000000000101100000100','1','1','2');
insert into tx_flow_def values('3706',0,'C706','#=0000@1@#$');
insert into tx_flow_def values('3706',1,'A016','#=0000@2@#$');
insert into tx_flow_def values('3706',2,'D005','#=0001@3@#=0002@4@#$');
insert into tx_flow_def values('3706',3,'M002','#$');
insert into tx_flow_def values('3706',4,'D099','#$');
insert into tx_sub_def  values('C706','IC卡圈提','spC706','0','0'); 
*************************************************/
#define  ERR_DEAL if(ret){sprintf(acErrMsg,"ERROR");WRITEMSG goto ErrExit;}
#include "public.h"
#define  EXTERN
#include "mdm_ac_rel_c.h"
#include "mdm_attr_c.h"
#include "prdt_ac_id_c.h"

spC706()
{
	int     ret=0;
	double  tx_amt=0.00;      /*交易金额*/
	char    ACNO_IN[20];      /*转入账号*/
	char    XZ_Flag[2];       /*现转标志*/
	char    cAcc_no[8];       /*电子钱包账户记账科目*/
	char    cIn_acno[20];     /*IC卡电子钱包内部账户*/
	
	struct  mdm_ac_rel_c      sMdm_ac_rel;
	struct  mdm_attr_c        sMdm_attr;
	struct  prdt_ac_id_c      sPrdt_ac_id;

	memset(ACNO_IN,           0x00, sizeof(ACNO_IN));
	memset(XZ_Flag,           0x00, sizeof(XZ_Flag));
	memset(cAcc_no,           0x00, sizeof(cAcc_no));
	memset(cIn_acno,          0x00, sizeof(cIn_acno));
	
	memset(&sMdm_ac_rel,      0x00, sizeof(struct mdm_ac_rel_c));
	memset(&sMdm_attr,        0x00, sizeof(struct mdm_attr_c));
	memset(&sPrdt_ac_id,      0x00, sizeof(struct prdt_ac_id_c));
 
	/** 初始化全局变量 **/
	pub_base_sysinit();
	
	/*取值，赋值*/
	get_zd_data("0310",ACNO_IN);
	pub_base_strpack(ACNO_IN);
	pub_base_old_acno(ACNO_IN);
	get_zd_double("0400", &tx_amt);
	get_zd_data("0670",XZ_Flag);
	get_zd_data("0640",cAcc_no);
	get_zd_data("0320",cIn_acno);
	pub_base_strpack(cIn_acno);
	pub_base_old_acno(cIn_acno);

	/*贷方*/
	if(XZ_Flag[0]=='0') /*现金*/
	{
		set_zd_data( "1191" , "01" );	 
		set_zd_double( "1194", tx_amt );	
		set_zd_data( "1197", "现金付出" );	
		set_zd_data( "1198",  cIn_acno);
		vtcp_log("%s%d[%s]",__FILE__,__LINE__,cIn_acno);
		
		set_zd_data("1171", "1");
	}
	else if(XZ_Flag[0]=='1') /*转账*/
	{
		ret=Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdm_ac_rel, "ac_no='%s'",ACNO_IN);
			ERR_DEAL

		ret=Mdm_attr_Sel(g_pub_tx.reply, &sMdm_attr, "mdm_code='%s'", sMdm_ac_rel.mdm_code);
			ERR_DEAL
	
		ret=Prdt_ac_id_Sel(g_pub_tx.reply, &sPrdt_ac_id, "ac_id=%ld", sMdm_ac_rel.ac_id);
			ERR_DEAL
		
		/*存款子交易*/
		set_zd_data("1011",ACNO_IN);
		set_zd_long("1012",sPrdt_ac_id.ac_seqn);
		set_zd_data("1014",sMdm_attr.note_type);
		set_zd_data("1015",sMdm_ac_rel.note_no);
		set_zd_double("1013",tx_amt);
		set_zd_data("101A","01");
		set_zd_data("101B","2");
		set_zd_data("1016","存款");
		set_zd_data("1017","0");
		
		set_zd_data("1171", "2");
	}
	
	/*借方(内部帐借方记帐)*/
	set_zd_data("1201",cIn_acno);
	set_zd_double("1208",tx_amt);
	set_zd_data("120A","内部帐借方记帐");
	if(XZ_Flag[0]=='0')
	{
		set_zd_data("1205","1");
	}
	else if(XZ_Flag[0]=='1')
	{
		set_zd_data("1205","2");
	}
	set_zd_data("1204","01");

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"IC卡圈提成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg, "IC卡圈提失败![%s]", g_pub_tx.reply);
	WRITEMSG
		set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 1;	
} 
