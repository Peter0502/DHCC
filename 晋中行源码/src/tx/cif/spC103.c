/*************************************************
* 文 件 名: spC103.c
* 功能描述：公司客户信息录入
* 作    者: andy
* 完成日期：2004年01月10日
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
#include "cif_cop_inf_c.h"
#include "cif_id_code_rel_c.h"

	static struct cif_basic_inf_c    cif_basic_inf;
	static struct cif_cop_inf_c      cif_cop_inf;
	static struct cif_id_code_rel_c	 g_cif_id_code_rel;    

	char credit[8];
	char corp_size[4];
	char corp_act_mode[4];	
	char corp_mana_sts[4];
	char cif_no[9];
		
spC103()  
{ 	
	int ret=0;
	
	/** 初始化全局变量 **/
	pub_base_sysinit();
	sprintf(acErrMsg,"初始化全局变量 PASS!");
	WRITEMSG	
	
	/** 数据初始化 **/
	if ( data_init_struct() )	ERR_DEAL;
	sprintf(acErrMsg,"数据初始化 PASS!");
	WRITEMSG
		
	/** 取值、赋值 **/
	if ( data_set_struct() )        ERR_DEAL;
	sprintf(acErrMsg,"取值、赋值 PASS!");
	WRITEMSG
		
	/** 处理流程 **/
	/* 检查客户号是否已经存在 */
	ret = Cif_basic_inf_Sel( g_pub_tx.reply , &cif_basic_inf , 
						" cif_no=%ld " , cif_cop_inf.cif_no );
	if( ret==100 )
	{
		sprintf(acErrMsg,"无此客户号，请检查![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C007");
		ERR_DEAL;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"读取客户基本信息异常![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C006");
		ERR_DEAL;
	}
	sprintf(acErrMsg,"检查客户号是否已经存在 PASS!");
	WRITEMSG
	
	/* 检查客户号是否是对公客户号 */
	sprintf( cif_no , "%ld" , cif_cop_inf.cif_no );
	if ( cif_no[0] != '5' )
	{
		sprintf(acErrMsg,"非对公客户号!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"C010");
		ERR_DEAL;
	}		
	sprintf(acErrMsg,"检查客户号是否是对公客户号 PASS!");
	WRITEMSG
	/* 代码证号 */
	/**			按财务要求不登记多余的证件信息  Cif_id_code_rel 这个表只保留一条对应证件信息 wudw 20130228
	if(strlen(cif_cop_inf.crd_no))
	{
			memset (&g_cif_id_code_rel, 0x00, sizeof(struct cif_id_code_rel_c));
		ret=Cif_id_code_rel_Sel(g_pub_tx.reply,&g_cif_id_code_rel, \
			"cif_no=%ld and id_type='8'",cif_cop_inf.cif_no);
		if(ret==100)
		{
			g_cif_id_code_rel.cif_no=cif_cop_inf.cif_no;
			strcpy(g_cif_id_code_rel.id_type,"8");
			strcpy(g_cif_id_code_rel.id_no,cif_cop_inf.crd_no);

			ret=Cif_id_code_rel_Ins(g_cif_id_code_rel,&g_pub_tx.reply);
			if( ret && ret!=-239 )
			{
				sprintf(acErrMsg,"登记客户证件与客户号对照表失败![%d]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"C009");
				goto ErrExit;
			}else if( ret==-239 ){
				sprintf(acErrMsg,"此代码证已登记,请检查![%d]",ret);
				WRITEMSG
				sprintf(acErrMsg,"此代码证[%s][%s]",cif_cop_inf.crd_no,g_cif_id_code_rel.id_no);
				WRITEMSG
				strcpy(g_pub_tx.reply,"C093");
				goto ErrExit;
			}
		}
		else if(ret)
		{
			sprintf(acErrMsg,"查询客户证件与客户号对照表失败!,ret=[%d]",ret);
			WRITEMSG
			ERR_DEAL;
		}		
	}  */
	/* 营业执照 */
	/* if(strlen(cif_cop_inf.license))
	{
			memset (&g_cif_id_code_rel, 0x00, sizeof(struct cif_id_code_rel_c));
		vtcp_log("PASSPASSPASSPASS");
		ret=Cif_id_code_rel_Sel(g_pub_tx.reply,&g_cif_id_code_rel,
			"cif_no=%ld and id_type='A'",cif_cop_inf.cif_no);
		if(ret==100)
		{
			g_cif_id_code_rel.cif_no=cif_cop_inf.cif_no;
			strcpy(g_cif_id_code_rel.id_type,"A");
			strcpy(g_cif_id_code_rel.id_no,cif_cop_inf.license);

			ret=Cif_id_code_rel_Ins(g_cif_id_code_rel,&g_pub_tx.reply);
			if(ret&&ret!=-239)
			{
				sprintf(acErrMsg,"登记客户证件与客户号对照表失败![%d]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"C009");
				goto ErrExit;
			}else if(ret==-239){
				sprintf(acErrMsg,"此营业执照已登记,请检查!");
				WRITEMSG
				sprintf(acErrMsg,"营业执照[%s][%s]",cif_cop_inf.license,g_cif_id_code_rel.id_no);
				WRITEMSG
				strcpy(g_pub_tx.reply,"C091");
				goto ErrExit;
			}

		}
		else if(ret)
		{
			sprintf(acErrMsg,"查询客户证件与客户号对照表失败!,ret=[%d]",ret);
			WRITEMSG
			ERR_DEAL;
		}		
	} */
	/* 事业单位登记许可证 */
	/*if(strlen(cif_cop_inf.units_license))
	{
			memset (&g_cif_id_code_rel, 0x00, sizeof(struct cif_id_code_rel_c));
		vtcp_log("PASSPASSPASSPASS");
		ret=Cif_id_code_rel_Sel(g_pub_tx.reply,&g_cif_id_code_rel, \
			"cif_no=%ld and id_type='9'",cif_cop_inf.cif_no);
		if(ret==100)
		{
			g_cif_id_code_rel.cif_no=cif_cop_inf.cif_no;
			strcpy(g_cif_id_code_rel.id_type,"9");
			strcpy(g_cif_id_code_rel.id_no,cif_cop_inf.units_license);

			ret=Cif_id_code_rel_Ins(g_cif_id_code_rel,&g_pub_tx.reply);
			if(ret&&ret!=-239)
			{
				sprintf(acErrMsg,"登记客户证件与客户号对照表失败![%d]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"C009");
				goto ErrExit;
			}else if(ret==-239){
				sprintf(acErrMsg,"此经营许可证已登记,请检查!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"C094");
				goto ErrExit;
			}

		}
		else if(ret)
		{
			sprintf(acErrMsg,"查询客户证件与客户号对照表失败!,ret=[%d]",ret);
			WRITEMSG
			ERR_DEAL;
		}		
	}
	**/
	ret = Cif_cop_inf_Sel(g_pub_tx.reply,&cif_cop_inf,"cif_no=%ld", \
			cif_cop_inf.cif_no);
	if(ret==100)
	{
		/* 登记公司客户信息表 */
		ret = Cif_cop_inf_Ins( cif_cop_inf );
		if( ret )
		{
			sprintf(acErrMsg,"登记公司客户信息表失败![%d]",ret);
			WRITEMSG
			goto ErrExit;
		}
		sprintf(acErrMsg,"登记公司客户信息表 PASS!");
		WRITEMSG
	}
	else if(!ret)
	{
		sprintf(acErrMsg,"该客户已登记公司客户信息![%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C087");
		goto ErrExit;
	}

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
    sprintf(acErrMsg,"公司客户信息录入成功！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"公司客户信息录入失败！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
    
int data_init_struct()
{
	memset (&cif_basic_inf, 0x00, sizeof(struct cif_basic_inf_c));
	memset (&cif_cop_inf, 0x00, sizeof(struct cif_cop_inf_c));
	memset (&g_cif_id_code_rel, 0x00, sizeof(struct cif_id_code_rel_c));
	return 0;
}

int data_set_struct()
{    
	sprintf(acErrMsg,"进入赋值函数");
	WRITEMSG
	
	get_zd_long("0280",&cif_cop_inf.cif_no);		/* 客户号 */
	get_zd_long("0440",&cif_cop_inf.reg_date);		/* 注册日期 */
	get_zd_data("0890",cif_cop_inf.reg_country);	/* 注册国家 */
	get_zd_double("0400",&cif_cop_inf.reg_fund);	/* 注册资金 */	
	get_zd_data("0210",cif_cop_inf.cur_no);			/* 注册币种 */
	get_zd_data("0620",cif_cop_inf.crd_no);			/* 代码证号 */	
	get_zd_data("0300",cif_cop_inf.units_license);	/* 事业单位登记许可证 */		
	get_zd_data("0310",cif_cop_inf.g_tax_no);		/* 国税登记证 */	
	get_zd_data("0320",cif_cop_inf.d_tax_no);		/* 地税登记证 */	
	get_zd_data("0580",cif_cop_inf.credit_no);		/* 贷款卡号 */
	get_zd_data("0630",cif_cop_inf.license);		/* 营业执照 */	
	/*
	get_zd_data("0660",cif_cop_inf.eco_attri);	* 经济性质 *	
	*/
	strcpy(cif_cop_inf.eco_attri,"");
	get_zd_data("0670",cif_cop_inf.ent_attri);		/* 企业性质 */	
	
	get_zd_data("0640",credit);						/* 信誉度 */
	sprintf(acErrMsg,"屏幕取的 [%s]",credit);
	WRITEMSG
	
	pub_base_strpack ( credit );
	sprintf(acErrMsg,"压缩后 [%s]",credit);
	WRITEMSG
	
	strncpy( cif_cop_inf.credit , credit , 2);
	sprintf(acErrMsg,"修改成三位后 [%s]",credit);
	WRITEMSG	
	
	get_zd_data("0700",cif_cop_inf.region);		/* 地域级别 */	
	get_zd_data("0250",cif_cop_inf.supervior);	/* 上级单位 */	
	get_zd_data("0260",cif_cop_inf.director);	/* 主管单位 */	
	get_zd_data("0680",cif_cop_inf.belong_bank);/* 归属行业(人行划分) */	
	get_zd_data("0690",cif_cop_inf.belong_stat);/* 所属行业(统计局划分) */
	get_zd_data("0710",cif_cop_inf.mana_orga_moda);	/* 经营形式 */
	get_zd_data("0720",cif_cop_inf.bd_corp_orga_moda);/* 企业形式 */
		
	get_zd_data("0900",corp_size);			/* 企业规模 */	
	pub_base_strpack ( corp_size );
	strncpy( cif_cop_inf.corp_size , corp_size , 2);
	
	get_zd_data("0230",corp_act_mode);		/* 运作方式 */	
	pub_base_strpack ( corp_act_mode );
	strncpy( cif_cop_inf.corp_act_mode , corp_act_mode , 2);
	
	get_zd_double("0410",&cif_cop_inf.paicl_up_capital);/* 实收资本 */				
	get_zd_data("0240",corp_mana_sts);		/* 经营状态 */	
	pub_base_strpack ( corp_mana_sts );
	strncpy( cif_cop_inf.corp_mana_sts , corp_mana_sts , 2);	
					
	get_zd_data("0330",cif_cop_inf.ap_qualife);	/* 法人资格 */	
	get_zd_data("0810",cif_cop_inf.artificial_person);/* 法人姓名 */				
	get_zd_data("0820",cif_cop_inf.ap_id);	   	/* 法人身份证号码 */
	/*
	get_zd_data("0220",cif_cop_inf.cif_scopes);	   	* 所属领域 *
	*/
	strcpy(cif_cop_inf.cif_scopes,"");
	return 0;
}
