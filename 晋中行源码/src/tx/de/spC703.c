/*************************************************
* 文 件 名:    spC703.c
* 功能描述：   完成IC卡的电子钱包账号的存款交易（圈存）。
* 作    者:    wzw
* 完成日期：   2011年12月13日
insert into tx_def      values('3703','IC卡圈存','10100000000100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000100000000000000000010000100','1','1','2');
insert into tx_flow_def values('3703',0,'C703','#=0000@1@#$');
insert into tx_flow_def values('3703',1,'D003','#=0000@2@#$');
insert into tx_flow_def values('3703',2,'A017','#$');
insert into tx_sub_def  values('C703','IC卡圈存','spC703','0','0');
*************************************************/
#define  ERR_DEAL if(ret){sprintf(acErrMsg,"ERROR");WRITEMSG goto ErrExit;}
#include "public.h"
#define  EXTERN
#include "mdm_ac_rel_c.h"
#include "mdm_attr_c.h"
#include "prdt_ac_id_c.h"

spC703()
{
	int     ret=0;
	double  tx_amt=0.00;      /*交易金额*/
	char    ACNO_IN[20];      /*转入账号*/
	char    cPASSWORD[17];    /*密码*/
	char    cACC_NO[8];       /*电子钱包记账科目*/
	char    cIn_acno[20];     /*IC卡电子钱包内部账户*/
	
	struct  mdm_ac_rel_c      sMdm_ac_rel;
	struct  mdm_attr_c        sMdm_attr;
	struct  prdt_ac_id_c      sPrdt_ac_id;

	memset(ACNO_IN,           0x00, sizeof(ACNO_IN));
	memset(cPASSWORD,         0x00, sizeof(cPASSWORD));
	memset(cACC_NO,           0x00, sizeof(cACC_NO));
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
	get_zd_data("0790",cPASSWORD);
	get_zd_data("0640",cACC_NO);
	get_zd_data("0320",cIn_acno);
	pub_base_strpack(cIn_acno);
	pub_base_old_acno(cIn_acno);
		
	ret=Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdm_ac_rel, "ac_no='%s'",ACNO_IN);
		ERR_DEAL
	
	ret=Mdm_attr_Sel(g_pub_tx.reply, &sMdm_attr, "mdm_code='%s'", sMdm_ac_rel.mdm_code);
		ERR_DEAL
			
	ret=Prdt_ac_id_Sel(g_pub_tx.reply, &sPrdt_ac_id, "ac_id=%ld", sMdm_ac_rel.ac_id);
		ERR_DEAL
	
	/*借方(取款子交易)*/
	set_zd_data("1021",ACNO_IN);
	set_zd_long("1022",sPrdt_ac_id.ac_seqn);
	set_zd_data("1023",sMdm_attr.note_type);
	set_zd_data("1024",sMdm_ac_rel.note_no);
	set_zd_data("1026",sMdm_ac_rel.draw_pwd_yn);
	set_zd_data("1028",cPASSWORD);
	set_zd_double("102F", tx_amt);
	set_zd_data("102J", "01");
	set_zd_data("102K","2");
	set_zd_data("102Y","取款");
	set_zd_data("102O","0");
	
	/*贷方(内部帐贷方记帐)*/
	set_zd_data("1211",cIn_acno);
	set_zd_double("1218",tx_amt);
	set_zd_data("121A","账户圈存贷方记帐");
	set_zd_data("1215","2");
	set_zd_data("1214","01");
	
OKExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"IC卡圈存成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;	
ErrExit:
	sprintf(acErrMsg, "IC卡圈存失败![%s]", g_pub_tx.reply);
	WRITEMSG
		set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 1;	
}
