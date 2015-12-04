/*************************************************
* 文 件 名: spL308.c
* 功能描述：   
*              贷款基本信息维护
*
* 作    者:    lance
* 完成日期：   2003年01月25日
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
#include "mdm_prdt_rel_c.h"

static struct ln_mst_c			ln_mst;
static struct ln_mst_c			ln_mst_tmp;
static struct ln_mst_c			p_ln_mst;
static struct mdm_prdt_rel_c	mdm_prdt_rel;
	
static	char ac_no[25];
static	char ac_no_back[21];
	
static	long cif_no;
static	long cif_no_id;
static	long cif_no_back;
	
static	char cif_type_id[2];
static	int ac_seqn;
static	char type[2];
static  char rate_knd[2];
		
spL308()  
{ 	
	
	int ret=0;
	char ln_ac_id[20];
	char acc_hrt[6];
	char wrk_sts[2];
	long ac_id_t;
	
	/** 初始化全局变量 **/
	pub_base_sysinit();
	vtcp_log("%s,%d prdt_no=[%s] ",__FILE__,__LINE__,g_pub_tx.prdt_code);
	sprintf(acErrMsg,"初始化全局变量 PASS!");
	WRITEMSG	
	
	vtcp_log("%s,%d prdt_no=[%s] ",__FILE__,__LINE__,g_pub_tx.prdt_code);
	/** 数据初始化 **/
	if ( data_init_struct() )	ERR_DEAL;
	sprintf(acErrMsg,"数据初始化 PASS!");
	WRITEMSG
		
	vtcp_log("%s,%d prdt_no=[%s] ",__FILE__,__LINE__,g_pub_tx.prdt_code);
	/** 取值、赋值 **/
	if ( data_set_struct() )        ERR_DEAL;
	sprintf(acErrMsg,"取值、赋值 PASS!");
	WRITEMSG
		
	vtcp_log("%s,%d prdt_no=[%s] ",__FILE__,__LINE__,g_pub_tx.prdt_code);
	/** 处理流程 **/
	/* 根据币种生成账户序号 */
	ret = pub_base_CurToSeqn(g_pub_tx.cur_no , &ac_seqn );
	if ( ret )
		{
			sprintf(acErrMsg,"调用函数根据币种生成账户序号错误!");
			WRITEMSG
			ERR_DEAL;	
		}
	sprintf(acErrMsg,"根据币种生成账户序号 PASS!");
	WRITEMSG
	vtcp_log("%s,%d prdt_no=[%s] ",__FILE__,__LINE__,g_pub_tx.prdt_code);
		
	/* 根据证件类型和证件号码取得客户号及客户类型 */
	ret = pub_cif_CifCheck( g_pub_tx.id_type , g_pub_tx.id_no , &cif_no_id , 
							cif_type_id  );
	if ( ret )
		{
			sprintf(acErrMsg,"调用函数pub_cif_CifCheck错误!");
			WRITEMSG
			ERR_DEAL;	
		}
	sprintf(acErrMsg, "取出的客户号为[%ld] ", cif_no_id );
	WRITEMSG				
	sprintf(acErrMsg,"根据证件类型和证件号码取得客户号及客户类型 PASS!");
	WRITEMSG
	
	/* 检查 还款帐号 *	
	if ( strcmp( ac_no , ac_no_back ))	
  	{
		ret = pub_ln_AcCheck( ac_no_back , ac_seqn , &cif_no_back , &ac_id_t);
		if( ret )
		{
			sprintf(acErrMsg,"调用函数pub_ln_AcCheck错误!");
			WRITEMSG
			ERR_DEAL;	
		}
		if ( cif_no_back != cif_no_id  )
		{
			sprintf(acErrMsg,"根据证件查出的客户号与活期账号中客户号不符");
			WRITEMSG
			strcpy(g_pub_tx.reply,"L019");
			ERR_DEAL;
		}
	}
	sprintf(acErrMsg,"检查 还款帐号 PASS!");
	WRITEMSG
	*******/
	vtcp_log("%s,%d prdt_no=[%s] ",__FILE__,__LINE__,g_pub_tx.prdt_code);
	
	/* 还款账户ID */
	ret = pub_base_disid_cltid( ac_no_back , &p_ln_mst.repay_ac_id );
	if( ret )
		{
			sprintf(acErrMsg,"调用显示账号转换账户ID函数错误!");
			WRITEMSG
			ERR_DEAL;	
		}	
	sprintf(acErrMsg,"调用函数得出的还款账户ID为[%ld]",p_ln_mst.repay_ac_id);
	WRITEMSG	
	sprintf(acErrMsg,"检查 还款账户ID PASS!");
	WRITEMSG
				
	ret = Ln_mst_Sel( g_pub_tx.reply , &ln_mst_tmp , " ac_id=%ld and ac_seqn=%d " , g_ln_mst.ac_id , g_ln_mst.ac_seqn );		
	if (ret) {return ret;}
		
	/* 检查还款账户户名是否与贷款户名一致 */
	/**
	ret = Dd_mst_Sel( g_pub_tx.reply , &g_dd_mst , " ac_id=%ld and ac_seqn=1 " , p_ln_mst.repay_ac_id );
	if( ret!=0 && ret!=100)
	{
		sprintf(acErrMsg,"读取贷款主文件信息异常![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L021");
		ERR_DEAL;
	}	
	pub_base_strpack(g_dd_mst.name);
	pub_base_strpack(p_ln_mst.name);	
	if( strcmp( g_dd_mst.name, p_ln_mst.name) )
	{
		sprintf(acErrMsg,"根据证件查出的客户号与活期账号中客户号不符");
		WRITEMSG
		strcpy(g_pub_tx.reply,"L019");
		ERR_DEAL;
	}	
	*/
	vtcp_log("%s,%d prdt_no=[%s] ",__FILE__,__LINE__,g_pub_tx.prdt_code);
	/* 检查 合同号 */
	ret = Ln_mst_Sel( g_pub_tx.reply , &ln_mst , 
					" pact_no='%s' " , g_ln_mst.pact_no );
	if( ret!=0 && ret!=100)
	{
		sprintf(acErrMsg,"读取贷款主文件信息异常![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L021");
		ERR_DEAL;
	}else if ( ret == 0 )
		{			
			if( strcmp(ln_mst.pact_no , ln_mst_tmp.pact_no) && strcmp( ln_mst.ac_sts , "9" ) )
			{
			sprintf(acErrMsg,"合同号已存在![%s]",g_pub_tx.reply);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L020");
			ERR_DEAL;
			}			
		}
	sprintf(acErrMsg,"检查 合同号 PASS!");
	WRITEMSG
	vtcp_log("%s,%d prdt_no=[%s] ",__FILE__,__LINE__,g_pub_tx.prdt_code);
	
	/* 贷款产品属性检查(开户) */
	strcpy( type , "1" );
	strcpy(g_ln_mst.prdt_no,g_pub_tx.prdt_code);	/* 贷款产品编号 */
	vtcp_log("%s,%d prdt_no=[%s] ",__FILE__,__LINE__,g_pub_tx.prdt_code);
	
	WRITEMSG
	/* 长沙商行－只针对上线以后数据检查 */
	if( g_ln_mst.opn_date>=20070501 )
	{
		WRITEMSG
		ret = pub_ln_check( type );
		if( ret )
			{
				sprintf(acErrMsg,"调用贷款产品属性检查函数错误!");
				WRITEMSG
				ERR_DEAL;	
			}
		sprintf(acErrMsg,"贷款产品属性检查(开户) PASS!");
		WRITEMSG
	}
	WRITEMSG
	/* 客户类型是否合乎产品定义 */
	if( g_ln_mst.opn_date>=20070501 )
	{
		ret = pub_base_check_cltprdt( cif_type_id, g_pub_tx.prdt_code );
		if( ret )
		{
			sprintf(acErrMsg,"调用客户类型是否合乎产品定义函数错误!");
			WRITEMSG
			ERR_DEAL;	
		}
	}	
	WRITEMSG
	/***********
	g_ln_mst.bal = 0;
	************/
	WRITEMSG
	sprintf(acErrMsg,"客户类型是否合乎产品定义检查 PASS!");
	WRITEMSG
				 	
	/* 取贷款产品参数信息 */

	WRITEMSG
	ret = Ln_parm_Sel( g_pub_tx.reply , &g_ln_parm, "prdt_no='%s'", 
						g_ln_mst.prdt_no);
	WRITEMSG
	if( ret==100 )
	{
		sprintf( acErrMsg,"贷款产品参数表无此记录 erro code=[%s]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L001" );
		return 1;
	}else if( ret !=0 )
		{
    		sprintf( acErrMsg,"取贷款产品参数表异常 erro code=[%s]",ret);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"D103" );
    		return -1;
    		}
	sprintf(acErrMsg,"取贷款产品参数信息 PASS!");
	WRITEMSG	

	/* 检查到期日期是否可修改 */
	
	if( ln_mst_tmp.mtr_date!= p_ln_mst.mtr_date )
	{		
		if( g_ln_parm.ln_pay_type[0]=='3' || g_ln_parm.ln_pay_type[0]=='4' )/* 按揭贷款 */
		{
			sprintf(acErrMsg,"此贷款是按揭贷款,此处不可修改其到期日期![%s]",g_pub_tx.reply);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L142");
			ERR_DEAL;		
		}else{	/* 普通贷款 */
			if( ln_mst_tmp.ac_sts[0]!='1')
			{
				sprintf(acErrMsg,"贷款账户状态不正常,不可修改其到期日期![%s]",g_pub_tx.reply);
				WRITEMSG
				strcpy(g_pub_tx.reply,"L143");
				ERR_DEAL;
			}else{
				if( ln_mst_tmp.exp_ind[0]!='0' )
				{
					sprintf(acErrMsg,"贷款账户已展期,不可修改其到期日期![%s]",g_pub_tx.reply);
					WRITEMSG
					strcpy(g_pub_tx.reply,"L144");
					ERR_DEAL;					
				}
			}				
		}
	}
	/* 检查开户日期是否可修改 */
	if( ln_mst_tmp.opn_date!= p_ln_mst.opn_date )
	{		
		vtcp_log("[%d],[%d]",ln_mst_tmp.opn_date,p_ln_mst.opn_date);
		if( pub_base_CompDblVal(ln_mst_tmp.bal , 0.00) )/* 贷款已放款 */
		{
			sprintf(acErrMsg,"此贷款已经发放,不可修改其开户日期![%s]",g_pub_tx.reply);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L150");
			ERR_DEAL;					
		}
	}	
	
	/* 检查开户日期是否可修改 */
	
	if( ln_mst_tmp.opn_date!= p_ln_mst.opn_date )
	{		
		if( pub_base_CompDblVal(ln_mst_tmp.bal , 0.00) )/* 贷款已放款 */
		{
			sprintf(acErrMsg,"此贷款已经发放,不可修改其开户日期![%s]",g_pub_tx.reply);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L150");
			ERR_DEAL;					
		}
	}
	
	/* 检查计划发放额是否可修改 */
	
	if( pub_base_CompDblVal(ln_mst_tmp.amt_lmt , p_ln_mst.amt_lmt) )
	{		
		if( pub_base_CompDblVal(ln_mst_tmp.bal , 0.00) )/* 贷款已放款 */
		{							
			if( pub_base_CompDblVal(p_ln_mst.amt_lmt , ln_mst_tmp.bal)<0 )/* 贷款计划发放额小于贷款余额 */
			{
				sprintf(acErrMsg,"计划发放金额不能低于贷款现余额![%s]",g_pub_tx.reply);
				WRITEMSG
				strcpy(g_pub_tx.reply,"L151");
				ERR_DEAL;					
			}
		}
	}

	/* 检查顾问费是否可修改 */
	
	if( pub_base_CompDblVal(ln_mst_tmp.advis_amt_pln , p_ln_mst.advis_amt_pln) )
	{								
		if( pub_base_CompDblVal(p_ln_mst.advis_amt_pln , ln_mst_tmp.advis_amt_fac) )/* 顾问费低于已付给的顾问费 */
		{
			sprintf(acErrMsg,"财务顾问费低于已付的财务顾问费![%s]",g_pub_tx.reply);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L152");
			ERR_DEAL;					
		}
	}
					
	/* 更新贷款主文件 */
    ret = Ln_mst_Dec_Upd( g_pub_tx.reply , "ac_id=%ld and ac_seqn=%d" ,g_ln_mst.ac_id , g_ln_mst.ac_seqn );
    if ( ret )
    {
        sprintf( acErrMsg, "执行Ln_mst_Dec_Upd错!ret=[%d]",ret );
        WRITEMSG
        ERR_DEAL
    }
    
    ret = Ln_mst_Fet_Upd( &g_ln_mst,g_pub_tx.reply );   
    if ( ret == 100 )
    {
        sprintf( acErrMsg, "贷款主文件中无此记录" );
        WRITEMSG
        ERR_DEAL
    }else if ( ret )
    {
        sprintf( acErrMsg, "执行Ln_mst_Fet_Upd出错!ret=[%d]",ret );
        WRITEMSG
        ERR_DEAL
    }

	g_ln_mst.cif_no = cif_no_id;   		/* 客户号 */
	g_ln_mst.opn_date = p_ln_mst.opn_date;	/* 开户日期 */
	strcpy( g_ln_mst.pact_no , p_ln_mst.pact_no );	/* 合同号 */
	g_ln_mst.amt_lmt = p_ln_mst.amt_lmt;	/* 计划发放额 */
	
	/*	g_ln_mst.bal = p_ln_mst.bal;	*/	
	/* 计划发放额 */	
	g_ln_mst.repay_ac_id= p_ln_mst.repay_ac_id;	/* 还款账号 */
	g_ln_mst.rate = p_ln_mst.rate;		/* 正常利率 */	
	g_ln_mst.over_rate = p_ln_mst.over_rate;	/* 逾期利率 */
	strcpy(g_ln_mst.repay_type,p_ln_mst.repay_type);	/* 贷款支付方式 */
	strcpy(g_ln_mst.cal_code , p_ln_mst.cal_code);		/* 指标体系代码 */		
	g_ln_mst.mtr_date = p_ln_mst.mtr_date;		/* 到期日期 */
	strcpy(g_ln_mst.intst_type,p_ln_mst.intst_type);	/* 计息类型 */	
	strcpy(g_ln_mst.cmpd_intst_ind,p_ln_mst.cmpd_intst_ind);	/* 复利标志 */
	strcpy(g_ln_mst.name, p_ln_mst.name);		/* 贷款户名 */	
	sprintf(acErrMsg,"!!!!!!!!!!!!!!贷款户名p_ln_mst.name[%s]",g_ln_mst.name);
	WRITEMSG		
	
	ret = Ln_mst_Upd_Upd( g_ln_mst, g_pub_tx.reply );
	if ( ret == -239 )
	{
		sprintf( acErrMsg, "修改贷款主文件错,存在重复记录!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"L031");
		ERR_DEAL
	}else if( ret )
		{
			sprintf( acErrMsg,"执行Ln_mst_Upd_Upd错!ret=[%d]",ret );
			WRITEMSG
			ERR_DEAL
		}
	Ln_mst_Clo_Upd();
	sprintf(acErrMsg,"更新贷款主文件 PASS!");
	WRITEMSG	
	
	/* Pub_tx结构赋值 */
	
	g_pub_tx.cif_no = cif_no_id;			/* 客户号 */
	strcpy ( g_pub_tx.ac_no , ln_ac_id );	/* 账号 */
	g_pub_tx.ac_id = g_ln_mst.ac_id;		/* 账户ID */
	g_pub_tx.ac_seqn = g_ln_mst.ac_seqn;	/* 账户序号 */
	strcpy ( g_pub_tx.ac_id_type , "3" );	/* 账号类型 3-贷款 */
	strcpy ( g_pub_tx.intst_type , g_ln_mst.intst_type );	/* 计息类型 */	
	strcpy ( g_pub_tx.add_ind , "1" );		/* 增减:0减1加2销户 */	
	strcpy ( g_pub_tx.ct_ind , "2" );		/* 现转:1现2转 */	
	/* 是否打印存折(有折无折标志):1是0否 */
	strcpy ( g_pub_tx.prt_ind , "1" );	
	/* 入明细帐标志:1日间入2日终单笔入3日终汇总入 */
	strcpy ( g_pub_tx.hst_ind , "1" );
	g_pub_tx.svc_ind = 3060;			/* 贷款结转 */
	strcpy( g_pub_tx.ac_wrk_ind , "0100000" );
	strcpy ( g_pub_tx.brf,"贷款基本信息维护" );
	sprintf(acErrMsg,"Pub_tx结构赋值 PASS!");
	WRITEMSG
	
	/*** 登记交易流水 ***/
	if( pub_ins_trace_log() )
	{
		sprintf(acErrMsg,"登记交易流水错误!");
		WRITEMSG
		goto ErrExit;
	}
	sprintf(acErrMsg,"登记交易流水 PASS!");
	WRITEMSG		
	
	/* 输出 */	
	sprintf(acErrMsg,"输出 PASS!");
	WRITEMSG	

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"贷款基本信息维护成功！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"贷款基本信息维护失败！[%s]",g_pub_tx.reply);
    WRITEMSG
    if ( strcmp( g_pub_tx.reply , "L018" )==0 )
    {
    	strcpy( g_pub_tx.reply , "L115" );
    }
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
    
int data_init_struct()
{
	memset (&g_ln_mst, 0x00, sizeof(struct ln_mst_c));
	memset (&g_dd_mst, 0x00, sizeof(struct dd_mst_c));
	memset (&g_mdm_ac_rel, 0x00, sizeof(struct mdm_ac_rel_c));
	
	memset (&ln_mst, 0x00, sizeof(struct ln_mst_c));
	memset (&p_ln_mst, 0x00, sizeof(struct ln_mst_c));
	memset (&ln_mst_tmp, 0x00, sizeof(struct ln_mst_c));
	memset (&mdm_prdt_rel, 0x00, sizeof(struct mdm_prdt_rel_c));
	
	return 0;
	
}

int data_set_struct()
{    
	int ret=0;
	g_ln_mst.opn_date = 0;
	sprintf(acErrMsg,"进入赋值函数");
	WRITEMSG
	get_zd_data("0670",g_pub_tx.id_type);		/* 证件类型 */
	get_zd_data("0620",g_pub_tx.id_no);			/* 证件号码 */
	get_zd_data("0210",g_pub_tx.cur_no);		/* 币种 */	
	get_zd_data("0380",ac_no_back);				/* 还款帐号 */
	get_zd_data("0630",g_ln_mst.pact_no);		/* 合同号 */	
	get_zd_double("0430",&g_ln_mst.amt_lmt);	/* 计划发放额 */
	get_zd_double("0400",&g_ln_mst.advis_amt_pln);	/* 财务顾问费 */	
	/*	get_zd_double("0430",&g_ln_mst.bal);	*/
	/* 计划发放额 */	
	get_zd_double("0840",&g_ln_mst.rate);		/* 正常利率 */	
	get_zd_double("0850",&g_ln_mst.over_rate);	/* 逾期利率 */
	get_zd_double("0850",&g_ln_mst.over_rate);	/* 逾期利率 */
	get_zd_double("0940",&g_ln_mst.fine_rate);	/* 罚息利率 */
	vtcp_log("aaaaaaaaaaa[%lf][%lf][%lf]\n",g_ln_mst.rate,g_ln_mst.over_rate,g_ln_mst.fine_rate);	
	get_zd_data("0230",g_pub_tx.prdt_code);		/* 产品代码 */
	get_zd_data("0680",rate_knd);			/* 利率取得方式 */
	get_zd_data("0690",g_ln_mst.repay_type);	/* 贷款支付方式 */
	get_zd_data("0610",g_ln_mst.cal_code);		/* 指标体系代码 */		
	get_zd_long("0450",&g_ln_mst.mtr_date);		/* 到期日期 */
	get_zd_long("0440",&g_ln_mst.opn_date); 	/* 开户日期 */
	get_zd_data("0710",g_ln_mst.intst_type);	/* 计息类型 */	
	get_zd_data("0930",g_ln_mst.cmpd_intst_ind);	/* 复利标志 */
	get_zd_data("0270",g_ln_mst.name);		/* 贷款户名 */	
	sprintf(acErrMsg,"!!!!!!!!!!!!!!贷款户名g_ln_mst.name[%s]",g_ln_mst.name);
	WRITEMSG			
	
	sprintf(acErrMsg,"~~开户日 [%ld], .amt_lmt[%f]",g_ln_mst.opn_date, g_ln_mst.amt_lmt);
	WRITEMSG
	get_zd_data("0310",ac_no); 		/* 贷款账号 */
	
	/* 根据贷款帐号查询账号介质关系表取出账户ID */
	ret = Mdm_ac_rel_Sel( g_pub_tx.reply , &g_mdm_ac_rel , " ac_no='%s' " , ac_no );			
	if( ret==100 )
	{
		sprintf( acErrMsg,"凭证(介质)与账户关系表无此记录 erro code=[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"O001" );
		return 1;
	} else if( ret !=0 )
		{
    			sprintf( acErrMsg,"取凭证(介质)与账户关系表异常erro code=[%s]",ret);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"D103" );
    			return 1;
    	}
	sprintf(acErrMsg,"取出的账户ID为[%ld]",g_mdm_ac_rel.ac_id);
	WRITEMSG
	
	g_ln_mst.ac_id = g_mdm_ac_rel.ac_id;		/* 贷款账户ID */	
	get_zd_long("0350",&g_ln_mst.ac_seqn);		/* 贷款序号 */	
	get_zd_data("0630",p_ln_mst.pact_no);		/* 合同号 */	
	get_zd_double("0430",&p_ln_mst.amt_lmt);	/* 计划发放额 */
	get_zd_double("0400",&p_ln_mst.advis_amt_pln);	/* 财务顾问费 */	
	/*	get_zd_double("0430",&p_ln_mst.bal);	*/	
	/* 计划发放额 */	
	
	get_zd_double("0840",&p_ln_mst.rate);		/* 正常利率 */	
	get_zd_double("0850",&p_ln_mst.over_rate);	/* 逾期利率 */
	get_zd_double("0940",&p_ln_mst.fine_rate);	/* 罚息利率 */		
	get_zd_data("0690",p_ln_mst.repay_type);	/* 贷款支付方式 */
	get_zd_data("0610",p_ln_mst.cal_code);		/* 指标体系代码 */		
	get_zd_long("0450",&p_ln_mst.mtr_date);		/* 到期日期 */
	get_zd_long("0440",&p_ln_mst.opn_date); 		/* 开户日期 */
	get_zd_data("0710",p_ln_mst.intst_type);	/* 计息类型 */
	get_zd_data("0930",p_ln_mst.cmpd_intst_ind);	/* 复利标志 */	
	get_zd_data("0270",p_ln_mst.name);		/* 贷款户名 */	
	sprintf(acErrMsg,"!!!!!!!!!!!!!!贷款户名p_ln_mst.name[%s]",p_ln_mst.name);
	WRITEMSG			
	p_ln_mst.ac_id = g_mdm_ac_rel.ac_id;		/* 贷款账户ID */	
	get_zd_long("0350",&p_ln_mst.ac_seqn);		/* 贷款序号 */				
	return 0;
}

