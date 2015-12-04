/*************************************************
* 文 件 名: spC313.c
* 功能描述：机构客户信息维护
* 作    者: andy
* 完成日期：2004年06月03日
* 修改记录：   
*     1. 日    期:
*        修 改 人:
*        修改内容:
*     2. 
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "stdio.h" 
#define EXTERN
#include "public.h"
#include "trace_log_c.h"
#include "cif_basic_inf_c.h"
#include "cif_magn_inf_c.h"
#include "cif_id_code_rel_c.h"

	static struct cif_magn_inf_c      cif_magn_inf; 

spC313()  
{ 	
	int ret=0;
	long    lCif_no=0;
	char    cLicense[21];
	char    cQualife[21];
	char    cName[21];
	char    cId_no[21];
	char    cCredit_no[17];
	char    cOwn_type[2];
	char    cEnt_type[2];
	char    cCredit[2];
	char    cSupervior[51];
	char    cDirector[51];
	char    cFinancer[51];
	char    cBank[51];
	double  dOpn_amt=0.0;
	
	/** 初始化全局变量 **/
	pub_base_sysinit();
	sprintf(acErrMsg,"初始化全局变量 PASS!");
	WRITEMSG	
	
	/** 数据初始化 **/
	memset(cLicense,0x00,sizeof(cLicense));
	memset(cQualife,0x00,sizeof(cQualife));
	memset(cName,0x00,sizeof(cName));
	memset(cId_no,0x00,sizeof(cId_no));
	memset(cCredit_no,0x00,sizeof(cCredit_no));
	memset(cOwn_type,0x00,sizeof(cOwn_type));
	memset(cEnt_type,0x00,sizeof(cEnt_type));
	memset(cCredit,0x00,sizeof(cCredit));
	memset(cSupervior,0x00,sizeof(cSupervior));
	memset(cDirector,0x00,sizeof(cDirector));
	memset(cFinancer,0x00,sizeof(cFinancer));
	memset(cBank,0x00,sizeof(cBank));
	memset(&cif_magn_inf, 0x00, sizeof(struct cif_magn_inf_c));

	/** 取值、赋值 **/
	get_zd_long("0280",&lCif_no);		/* 客户号 */
	get_zd_data("0620",cLicense);		/* 代码证号 */		
	get_zd_data("0330",cQualife);	/* 法人证号 */	
	get_zd_data("0810",cName);/* 事业法人名称 */				
	get_zd_data("0820",cId_no);	   	/* 法人身份证号码 */
	get_zd_data("0580",cCredit_no);		/* 贷款卡号 */
	get_zd_data("0710",cOwn_type);	/* 所有制形式 */
	get_zd_data("0670",cEnt_type);		/* 企业性质 */
	get_zd_data("0640",cCredit);				/* 信誉度 */
	get_zd_data("0250",cSupervior);	/* 上级单位 */	
	get_zd_data("0260",cDirector);	/* 主管单位 */	
	get_zd_data("0270",cFinancer);	/* 财政资金调拨单位 */	
	get_zd_data("0830",cBank);	/* 基本账号开户行 */	
	get_zd_double("0410",&dOpn_amt);	/* 开办资金 */	
	
	
	sprintf(acErrMsg,"代码证号[%s]",cif_magn_inf.units_license);
	WRITEMSG
	sprintf(acErrMsg,"屏幕取的 [%s]",cCredit);
	WRITEMSG
	
	pub_base_strpack ( cCredit );
	sprintf(acErrMsg,"压缩后 [%s]",cCredit);
	WRITEMSG

		
	/** 处理流程 **/
	
	vtcp_log("[%s][%d]代码证号1[%s]",__FILE__,__LINE__,cif_magn_inf.units_license);
	ret=Cif_magn_inf_Dec_Upd(g_pub_tx.reply,"cif_no=%ld ",lCif_no);
	if (ret)
	{
		sprintf(acErrMsg,"DECLARE Cif_magn_inf_rel游标异常![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C006");
		ERR_DEAL;
	}
	
	vtcp_log("[%s][%d]代码证号2[%s]",__FILE__,__LINE__,cif_magn_inf.units_license);
	ret=Cif_magn_inf_Fet_Upd(&cif_magn_inf,g_pub_tx.reply);
	if (ret)
	{
		sprintf(acErrMsg,"FETCH Cif_magn_inf游标异常![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C006");
		ERR_DEAL;
	}

	memcpy(cif_magn_inf.units_license,cLicense,sizeof(cif_magn_inf.units_license));		/* 代码证号 */		
	memcpy(cif_magn_inf.ap_qualife,cQualife,sizeof(cif_magn_inf.ap_qualife));	/* 法人证号 */	
	memcpy(cif_magn_inf.artificial_person,cName,sizeof(cif_magn_inf.artificial_person));/* 事业法人名称 */				
	memcpy(cif_magn_inf.ap_id,cId_no,sizeof(cif_magn_inf.ap_id));	   	/* 法人身份证号码 */
	memcpy(cif_magn_inf.credit_no,cCredit_no,sizeof(cif_magn_inf.credit_no));		/* 贷款卡号 */
	memcpy(cif_magn_inf.own_attri,cOwn_type,sizeof(cif_magn_inf.own_attri));	/* 所有制形式 */
	memcpy(cif_magn_inf.ent_attri,cEnt_type,sizeof(cif_magn_inf.ent_attri));		/* 企业性质 */
	memcpy(cif_magn_inf.credit,cCredit,sizeof(cif_magn_inf.credit));				/* 信誉度 */
	memcpy(cif_magn_inf.supervior,cSupervior,sizeof(cif_magn_inf.supervior));	/* 上级单位 */	
	memcpy(cif_magn_inf.director,cDirector,sizeof(cif_magn_inf.director));	/* 主管单位 */	
	memcpy(cif_magn_inf.financer,cFinancer,sizeof(cif_magn_inf.financer));	/* 财政资金调拨单位 */	
	memcpy(cif_magn_inf.opn_bank,cBank,sizeof(cif_magn_inf.opn_bank));	/* 基本账号开户行 */	
	cif_magn_inf.opn_amt=dOpn_amt;	/* 开办资金 */	
	vtcp_log("[%s][%d]代码证号3[%s]",__FILE__,__LINE__,cif_magn_inf.units_license);
	ret=Cif_magn_inf_Upd_Upd(cif_magn_inf,g_pub_tx.reply);
	if (ret)
	{
		sprintf(acErrMsg,"UPDATE Cif_magn_inf游标异常![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C006");
		ERR_DEAL;
	}
	Cif_magn_inf_Clo_Upd( );	
	
	vtcp_log("[%s][%d]代码证号4[%s]",__FILE__,__LINE__,cif_magn_inf.units_license);
	/* 登记交易流水 */
	if( pub_ins_trace_log() )
	{
		sprintf(acErrMsg,"登记交易流水错误!");
		WRITEMSG
		goto ErrExit;
	}
	sprintf(acErrMsg,"登记交易流水 PASS!");
	WRITEMSG
		
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"机构客户信息维护成功！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"机构客户信息维护失败！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
