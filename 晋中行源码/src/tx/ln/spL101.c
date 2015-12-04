/*************************************************
* 文 件 名: spL101.c
* 功能描述：   
*              贷款开户
*
* 作    者:    lance
* 完成日期：   2003年01月10日
* 修改记录：   
*     1. 日    期:20110827
*        修 改 人:gongliangliang
*        修改内容:添加对放款账号和还款账号的校验，只能是本行本人的活期账户
*     2. 
*************************************************/
#define EXTERN
#include "stdio.h"
#include "string.h" 
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "public.h"
#include "ln_auth_c.h"
#include "draw_loan_limit_c.h"
#include "mo_po_co_c.h"
#include "ln_gage_reg_c.h"
#include "dd_mst_c.h"
#include "mdm_ac_rel_c.h"

static	double gage_amt_tmp;	

spL101()  
{ 	
	int ret=0 , flag=0;
	long min_day=0,max_day=0;
	double basic_rate=0.00;
	int	iUpd_flag=0;
	int	iIns_flag=0;
	
	struct ln_auth_c	ln_auth;
	struct draw_loan_limit_c sDraw_limit;
	struct mo_po_co_c	sMo_po_co;
	struct ln_gage_reg_c	sLn_gage_reg,sLnGageReg;
	struct dd_mst_c	g_dd_mst;
	struct mdm_ac_rel_c sMdm_ac_rel;
			
	/** 初始化全局变量 **/
	memset (&g_ln_mst, 0x00, sizeof(struct ln_mst_c));
	memset (&g_dd_mst, 0x00, sizeof(struct dd_mst_c));
	memset (&sMdm_ac_rel, 0x00, sizeof(struct mdm_ac_rel_c));
	memset (&ln_auth, 0x00, sizeof(struct ln_auth_c));
	memset (&sMo_po_co, 0x00, sizeof(sMo_po_co));
	memset (&sLn_gage_reg, 0x00, sizeof(sLn_gage_reg));
	memset (&sLnGageReg, 0x00, sizeof(sLnGageReg));
			
	pub_base_sysinit();
	
	/** 取值、赋值 **/
	if ( data_set_struct() )        ERR_DEAL;
	sprintf(acErrMsg,"取值、赋值 PASS!");
	WRITEMSG

	/* 检查是否与贷款审批登记簿中数据相同 */
	ret = Ln_auth_Dec_Upd( g_pub_tx.reply , "pact_no='%s'" , g_ln_mst.pact_no );
	if( ret )
	{
		sprintf( acErrMsg,"Prepare Ln_auth_Dec_Upd 游标出错[%d]",ret);
		WRITEMSG
		ERR_DEAL;
	}
	ret = Ln_auth_Fet_Upd( &ln_auth , g_pub_tx.reply );
	if( ret==100 )
	{
		sprintf( acErrMsg,"无此审批记录![%d]",ret);
		WRITEMSG
	 	strcpy( g_pub_tx.reply ,"L175" );
		ERR_DEAL ;
	}else if( ret ){
	 	sprintf( acErrMsg,"fetch Ln_auth_Fet_Upd error code = [%d]",ret);
	 	WRITEMSG
	 	strcpy( g_pub_tx.reply ,"D103" );
	 	ERR_DEAL ;
	}
	if( ln_auth.sts[0]!='X' )/*20110728 晋中商行启用信贷系统，审核状态由信贷提供，改为X */
	{
	 	sprintf( acErrMsg,"此借据已存在,请检查!");
	 	WRITEMSG
	 	strcpy( g_pub_tx.reply ,"L020" );
	 	ERR_DEAL ;		
	}
	/* 开户信息与审批信息比较 */
	/* 贷款户名 */
	pub_base_strpack(ln_auth.name);
	
	/*if( strcmp(g_pub_tx.name , ln_auth.name) )	
	{
	 	TRACE
	 	TRACE
	 	sprintf( acErrMsg,"贷款户名与审批信息不符![%s][%s]",g_pub_tx.name , ln_auth.name);
	 	WRITEMSG
	 	strcpy( g_pub_tx.reply ,"L166" );
	 	ERR_DEAL;
	}*/
	
	/******* edit by gong 20110827 晋中信贷不给核心传存款账号了！！！
	存款账户ID
	if( g_ln_mst.save_ac_id != ln_auth.save_ac_id )
	{
	 	sprintf( acErrMsg,"放款账号与审批信息不符!");
	 	WRITEMSG
	 	strcpy( g_pub_tx.reply ,"L167" );
	 	ERR_DEAL;
	}	
	 *********end by gong 20110827************************/
	
	/**add by gong 20110827 晋中商行还款账号/存款账号校验
	
	char br_no_tmp[6];
	memset(br_no_tmp,0x00,sizeof(br_no_tmp));
	memcpy(br_no_tmp,ln_auth.pact_no+1,5);
	
	ret = Dd_mst_Sel(g_pub_tx.reply ,&g_dd_mst,"ac_id in (select ac_id from mdm_ac_rel where ac_no ='%s')",g_pub_tx.ac_no1);
	if(ret)
	{
		sprintf(acErrMsg, "  还款账号/活期账号非活期账号![%d]",ret);
		set_zd_data(DC_GET_MSG, acErrMsg);
		WRITEMSG
		strcpy(g_pub_tx.reply, "B192");
		goto ErrExit;
	}

	if(strcmp(g_dd_mst.opn_br_no,br_no_tmp)!=0)
	{
		vtcp_log("[%s][%d]活期开户机构号[%s],信贷发来机构号[%s]",__FILE__,__LINE__,g_dd_mst.opn_br_no,br_no_tmp);
		sprintf( acErrMsg,"  还款账号/存款账号非本机构开户号![%d]",ret);
		set_zd_data(DC_GET_MSG, acErrMsg);
	 	WRITEMSG
	 	strcpy( g_pub_tx.reply ,"N098" );
	 	ERR_DEAL;
	}

	if(ln_auth.cif_no!=g_dd_mst.cif_no)
	{
		vtcp_log("[%s][%d]信贷传来的客户号[%ld],活期账户客户号[%ld]",__FILE__,__LINE__,ln_auth.cif_no,g_dd_mst.cif_no);
		sprintf( acErrMsg,"  还款账号/存款账号非本人账号![%d]",ret);
		set_zd_data(DC_GET_MSG, acErrMsg);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"N099" );
		ERR_DEAL ;
	}

   end by gong 20110827 晋中商行还款账号/存款账号校验**/
	
	/* 贷款金额 */
	if( pub_base_CompDblVal( ln_auth.amt , g_ln_mst.amt_lmt ) )
	{
	 	sprintf( acErrMsg,"计划放款金额与审批信息不符!");
	 	WRITEMSG
	 	strcpy( g_pub_tx.reply ,"L168" );
	 	ERR_DEAL;
	}
	/* 合同利率 */		
	if( pub_base_CompDblVal( ln_auth.rate , g_ln_mst.rate ) )
	{
	 	sprintf( acErrMsg,"合同利率与审批信息不符!");
	 	WRITEMSG
	 	strcpy( g_pub_tx.reply ,"L169" );
	 	ERR_DEAL;
	}	
	/****** edit by gong 20110827 晋中信贷不给核心传存款账号了！！！
	还款账号ID 
	if( g_ln_mst.repay_ac_id!=ln_auth.pay_ac_id )
	{
	 	sprintf( acErrMsg,"还款账号与审批信息不符!");
	 	WRITEMSG
	 	strcpy( g_pub_tx.reply ,"L170" );
	 	ERR_DEAL;
	}
	****************end by gong 20110827****************/
	
	/* 贷款期限 */	
	pub_base_deadlineM( g_ln_mst.opn_date,ln_auth.time_lmt-3,&min_day);/**  martin  原为1个月。现在3个**/
	pub_base_deadlineM( g_ln_mst.opn_date,ln_auth.time_lmt+1,&max_day);
	/** 
	if( g_ln_mst.mtr_date < min_day )
	{
	 	sprintf( acErrMsg,"到期日期过小,与审批信息不符![%ld][%ld][%ld]",min_day,max_day,g_ln_mst.mtr_date);
	 	WRITEMSG
	 	strcpy( g_pub_tx.reply ,"L171" );
	 	ERR_DEAL;
	}
	
	if( g_ln_mst.mtr_date > max_day )
	{
	 	sprintf( acErrMsg,"到期日期过大,与审批信息不符![%ld][%ld][%ld]",min_day,max_day,g_ln_mst.mtr_date);
	 	WRITEMSG
	 	strcpy( g_pub_tx.reply ,"L172" );
	 	ERR_DEAL;
	}
	**/
	
	/* 取基础利率 */
	ret = Ln_parm_Sel( g_pub_tx.reply , &g_ln_parm , "prdt_no='%s' " , g_pub_tx.prdt_code );
	if(ret)	
	{
		sprintf(acErrMsg,"取贷款参数信息失败!");
		WRITEMSG
		ERR_DEAL;	
	}
	ret = pub_base_getllz( g_ln_parm.rate_no, g_ln_parm.cur_no, g_pub_tx.tx_date , &basic_rate );
	if( ret )
	{
		sprintf(acErrMsg,"调用根据利率编号取得利率函数错误!");
		WRITEMSG
		return -1;	
	}
	basic_rate = basic_rate/1.2;/* 转换成月利率 */	
	/* 利率浮动比例计算 */		
	
	g_ln_mst.rate_flt = pub_base_PubDround_1( g_ln_mst.rate*100/basic_rate );		
	g_ln_mst.over_rate_flt = pub_base_PubDround_1( g_ln_mst.over_rate*100/basic_rate);
	g_ln_mst.fine_rate_flt = pub_base_PubDround_1( g_ln_mst.fine_rate*100/basic_rate);
			
			
	/* 修改审批登记簿中状态 */
	strcpy( ln_auth.sts , "1" );	
	ret = Ln_auth_Upd_Upd( ln_auth, g_pub_tx.reply  );
	if ( ret )
	{
		sprintf( acErrMsg,"Ln_auth_Upd_Upd 出错[%d]",ret);
		WRITEMSG
		return 1;
	}
	
	/*add by gong 20110829修改mdm_code*/	
	
	ret = Mdm_ac_rel_Sel(g_pub_tx.reply ,&sMdm_ac_rel,"ac_id ='%ld'",g_ln_mst.save_ac_id);
	if(ret)
	{
		sprintf(acErrMsg, "  数据库查询失败![%d]",ret);
		set_zd_data(DC_GET_MSG, acErrMsg);
		WRITEMSG
		strcpy(g_pub_tx.reply, "B192");
		goto ErrExit;
	}
	strcpy( ln_auth.save_mdm_code , sMdm_ac_rel.mdm_code );	
	ret = Ln_auth_Upd_Upd( ln_auth, g_pub_tx.reply  );
	if ( ret )
	{
		sprintf( acErrMsg,"Ln_auth_Upd_Upd 出错[%d]",ret);
		WRITEMSG
		return 1;
	}
	/*end by gong 20110829*/
	Ln_auth_Clo_Upd( );
	sprintf(acErrMsg,"与审批信息校验 PASS!");
	WRITEMSG		
	
	/* 若是在输入的卡/账号上开设贷款账户 */
	if ( strlen( g_pub_tx.crd_no )!= 0 )
	{
	/* 取该卡/账号对应的介质代码 *
		ret = Mdm_ac_rel_Sel( g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",
					g_pub_tx.crd_no );
		if( ret==100 )
		{
			sprintf(acErrMsg,"介质与账户关系信息中无此卡号或账号，请检查![%s]", 
					g_pub_tx.reply);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L014");
			goto ErrExit;
		}else if( ret )
		  {
			sprintf(acErrMsg,"读取介质与账户关系信息异常![%s]",g_pub_tx.reply);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L015");
			goto ErrExit;
		  }
		sprintf(acErrMsg,"取出的介质代码[%s]", g_mdm_ac_rel.mdm_code);
		WRITEMSG		  
		strcpy( g_pub_tx.mdm_code , g_mdm_ac_rel.mdm_code ); 
		g_pub_tx.ac_seqn = ALL_SEQN ;
		sprintf(acErrMsg,"取该卡/账号对应的介质代码 PASS!");
		WRITEMSG
		****/
	strcpy ( g_pub_tx.ac_no , g_pub_tx.crd_no );  
	}
	else
	{
		strcpy ( g_pub_tx.ac_wrk_ind,"0110" );
		strcpy ( g_pub_tx.ac_id_type , "3" );	/* 账号类型 3-贷款 */
		strcpy ( g_pub_tx.add_ind , "1" );		/* 增减1加 */	
		strcpy ( g_pub_tx.ct_ind , "2" );		/* 现转:1现2转 */	
		strcpy ( g_pub_tx.prt_ind , "0" );	/* 是否打印存折(有折无折标志)0否 */
		strcpy ( g_pub_tx.hst_ind , "1" );	/* 入明细帐标志:1日间入2日终单笔入3日终汇总入 */
		strcpy( g_pub_tx.brf,"贷款开户开介质" );		
		
		TRACE
		/***生成账号***/
		if( pub_acct_opn_cif_mdm() )
		{
			sprintf(acErrMsg,"生成账号错误!");
			WRITEMSG
			ERR_DEAL
		}
	}

	TRACE
	/* 确定贷款客户类型 */
	strcpy( g_ln_mst.cmpd_intst_ind , g_ln_parm.cmpd_intst_ind );	/*是否记复利标志*/
	
	/* Pub_tx结构赋值 */
	strcpy ( g_pub_tx.ac_wrk_ind,"0110" );
	strcpy ( g_pub_tx.ac_id_type , "3" );	/* 账号类型 3-贷款 */
	strcpy ( g_pub_tx.add_ind , "1" );		/* 增减1加 */	
	strcpy ( g_pub_tx.ct_ind , "2" );		/* 现转:1现2转 */	
	strcpy ( g_pub_tx.prt_ind , "0" );	/* 是否打印存折(有折无折标志)0否 */
	strcpy ( g_pub_tx.hst_ind , "1" );	/* 入明细帐标志:1日间入2日终单笔入3日终汇总入 */
	g_pub_tx.svc_ind = 3001;
	strcpy( g_pub_tx.brf,"贷款开户" );
	TRACE
	/***生成账户ID***/
	if( pub_acct_opn_ac() )
	{
		sprintf(acErrMsg,"生成账号ID错误!");
		WRITEMSG
		ERR_DEAL
	}

	sprintf(acErrMsg,"Pub_tx结构赋值 PASS!");
	WRITEMSG
	
	/*  登记流水*/
	if( pub_ins_ln_trace_log(ln_auth.amt))
    {
        sprintf(acErrMsg,"登记交易流水错误!");
        WRITEMSG
        goto ErrExit;
    }
    sprintf(acErrMsg,"登记交易流水 PASS!");
    WRITEMSG
	/*
	g_pub_tx.tx_amt1 = ln_auth.amt;
	g_pub_tx.svc_ind = 3001;
	strcpy( g_pub_tx.brf,"贷款开户" );
	if( pub_acct_trance() )
	{
		sprintf(acErrMsg,"交易记帐处理错误!");
		WRITEMSG
		goto ErrExit;
	}	
	********/	
	
	set_zd_data("0310",g_pub_tx.ac_no); 		/* 贷款账号 */
	set_zd_long("0350",g_ln_mst.ac_seqn);		/* 贷款序号 */
	set_zd_data("0720",g_ln_parm.pay_pln_type);	/* 放款计划类型 */
	set_zd_data("0660",g_ln_parm.pay_pln_crt); 	/* 还款计划生成类型 */
	set_zd_data("0210",g_pub_tx.cur_no);		/* 币种 */	
	/** begin add by chenchao 20110801 修改支付限制表 **/
	/****修改贷款自主委托支付限额表*****/
	if( (g_ln_parm.ln_type[0]!='A') )
	{
		memset(&sDraw_limit,0x00,sizeof(sDraw_limit));
		g_reply_int=Draw_loan_limit_Dec_Upd(g_pub_tx.reply,"pact_no='%s'",g_ln_mst.pact_no);
		if(g_reply_int)
		{
			sprintf(acErrMsg,"Draw_loan_limit声明游标错误");
			WRITEMSG
			strcpy( g_pub_tx.reply ,"D101" );
			goto ErrExit;
		}
		g_reply_int=Draw_loan_limit_Fet_Upd(&sDraw_limit,g_pub_tx.reply);
		if(g_reply_int)
		{
			sprintf(acErrMsg,"Draw_loan_limit取游标错误");
			WRITEMSG
			strcpy( g_pub_tx.reply ,"B384" );
			goto ErrExit;
		}
		sDraw_limit.sts[0]='1';
		sDraw_limit.ac_id=g_ln_mst.save_ac_id;
		g_reply_int=Draw_loan_limit_Upd_Upd(sDraw_limit,g_pub_tx.reply);
		if(g_reply_int)
		{
			sprintf(acErrMsg,"Draw_loan_limit更新记录错误");
			WRITEMSG
			strcpy( g_pub_tx.reply ,"D384" );
			goto ErrExit;
		}
		Draw_loan_limit_Clo_Upd();
	}
	vtcp_log("[%s][%d]pact_no=[%s]",__FILE__,__LINE__,g_ln_mst.pact_no);
	/** 增加贷款抵质押物的登记 ***/
	ret = Mo_po_co_Sel(g_pub_tx.reply,&sMo_po_co,"pact_no='%s'",g_ln_mst.pact_no);
	if(ret)
	{
		sprintf(acErrMsg,"查询合同借据关系表出错!!");
		WRITEMSG
		strcpy( g_pub_tx.reply ,"D107" );
		goto ErrExit;
	}
	ret = Ln_gage_reg_Dec_Upd(g_pub_tx.reply,"pact_no='%s' ",sMo_po_co.contract_no);
	if(ret)
	{
		sprintf(acErrMsg,"声明游标出错");
		WRITEMSG
		strcpy( g_pub_tx.reply ,"D101" );
		goto ErrExit;
	}
	while(1)
	{
		memset(&sLn_gage_reg,0x00,sizeof(sLn_gage_reg));
		vtcp_log("[%s][%d]pact_no=[%s]",__FILE__,__LINE__,g_ln_mst.pact_no);
		ret = Ln_gage_reg_Fet_Upd(&sLn_gage_reg,g_pub_tx.reply);
		if(ret && ret != 100)
		{
			sprintf(acErrMsg,"查找贷款抵质押登记薄出错!!");
			WRITEMSG
			strcpy( g_pub_tx.reply ,"D107" );
			goto ErrExit;
		}else if(ret == 100){
			if(!iIns_flag && strlen(sLn_gage_reg.pact_no)>0)
			{
				sLnGageReg.ac_id = g_ln_mst.ac_id;
				sLnGageReg.ac_seqn = g_ln_mst.ac_seqn;
				memset(sLnGageReg.name,0x00,sizeof(sLnGageReg.name));
				memcpy(sLnGageReg.name,g_ln_mst.name,sizeof(sLnGageReg.name)-1);
				ret = Ln_gage_reg_Ins(sLnGageReg,g_pub_tx.reply);
				if(ret)
				{
					Ln_gage_reg_Clo_Upd();
					sprintf(acErrMsg,"插入贷款抵质押登记薄出错!!");
					WRITEMSG
					strcpy( g_pub_tx.reply ,"D107" );
					goto ErrExit;
				}
				vtcp_log("[%s][%d]本次为插入操作",__FILE__,__LINE__);
			}
			break;
		}
		if(sLn_gage_reg.ac_id == 0)
		{
			sLn_gage_reg.ac_id = g_ln_mst.ac_id;
			sLn_gage_reg.ac_seqn = g_ln_mst.ac_seqn;
			memset(sLn_gage_reg.name,0x00,sizeof(sLn_gage_reg.name));
			memcpy(sLn_gage_reg.name,g_ln_mst.name,sizeof(sLn_gage_reg.name)-1);
			ret = Ln_gage_reg_Upd_Upd(sLn_gage_reg,g_pub_tx.reply);
			if(ret)
			{
				Ln_gage_reg_Clo_Upd();
				sprintf(acErrMsg,"更新贷款抵质押登记薄出错!!");
				WRITEMSG
				strcpy( g_pub_tx.reply ,"D107" );
				goto ErrExit;
			}
			vtcp_log("[%s][%d]本次为跟新操作",__FILE__,__LINE__);
			iIns_flag ++;
		}
		memcpy(&sLnGageReg,&sLn_gage_reg,sizeof(sLnGageReg));
	}
	Ln_gage_reg_Clo_Upd();
	/** END add by chenchao 20110801 修改支付限制表 **/
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"贷款开户成功！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"贷款开户失败！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}

int data_set_struct()
{    
	char save_ac_no[20];
	long cif_no_tmp;
	int  ac_seqn_tmp;
	int  ret=0;
	
	g_ln_mst.opn_date = g_pub_tx.tx_date;		/* 开户日期 */
	get_zd_data("0370",g_pub_tx.crd_no);		/* 卡/帐号 */
	get_zd_data("0670",g_pub_tx.id_type);		/* 证件类型 */
	get_zd_data("0620",g_pub_tx.id_no);		/* 证件号码 */			
	get_zd_data("0630",g_ln_mst.pact_no);		/* 合同号 */	
	get_zd_double("0430",&g_ln_mst.amt_lmt);	/* 计划发放额，贷款限额 */
	g_pub_tx.tx_amt1=0.00;				/* 开户同时的发放额 */	
	get_zd_double("0840",&g_ln_mst.rate);		/* 正常利率 */	
	get_zd_double("0850",&g_ln_mst.over_rate);	/* 逾期利率 */
	get_zd_double("0940",&g_ln_mst.fine_rate);	/* 罚息利率 */		
	get_zd_data("0230",g_pub_tx.prdt_code);		/* 产品代码 */
	get_zd_data("0210",g_pub_tx.cur_no);		/* 币种 */
	get_zd_data("0690",g_ln_mst.repay_type);	/* 贷款支付方式 */
	get_zd_data("0610",g_ln_mst.trust_no);		/* 委托协议编号 */		
	get_zd_long("0450",&g_ln_mst.mtr_date);		/* 到期日期 */
	get_zd_data("0710",g_pub_tx.intst_type);	/* 计息类型 */	
	get_zd_data("0250",g_pub_tx.name);		/* 贷款户名 */		
	get_zd_double("0420",&gage_amt_tmp);/* 抵押品金额 */
	get_zd_double("0410",&g_ln_mst.advis_amt_pln);/* 财务顾问费 */
	get_zd_int("0340",&ac_seqn_tmp );		/* 账号序号 */
	get_zd_data("0380",g_pub_tx.ac_no1);		/* 还款账号 */
	pub_base_old_acno(g_pub_tx.ac_no1);
	
	sprintf(acErrMsg,"还款帐号!!!![%s]",g_pub_tx.ac_no1);
	WRITEMSG
	get_zd_data("0320",save_ac_no);	/* 存款账号 */
	pub_base_old_acno(save_ac_no);
	sprintf(acErrMsg,"存款账号!!!![%s]",save_ac_no);
	WRITEMSG   
	/* 取存款账号ID */	
	pub_base_strpack(save_ac_no);
	if( strlen(save_ac_no) )
	{
		ret = pub_ln_AcCheck( save_ac_no,ac_seqn_tmp,
			&cif_no_tmp,&g_ln_mst.save_ac_id );
		if( ret )
		{
			sprintf(acErrMsg,"调用函数pub_ln_AcCheck错误!");
			WRITEMSG
			return 1;
		}	
	}  		
   	/* 取还款账号ID */	
 	pub_base_strpack(g_pub_tx.ac_no1);
	if( strlen(g_pub_tx.ac_no1) )
	{
		ret = pub_ln_AcCheck( g_pub_tx.ac_no1,ac_seqn_tmp,
			&cif_no_tmp,&g_ln_mst.repay_ac_id );
		if( ret )
		{
			sprintf(acErrMsg,"调用函数pub_ln_AcCheck错误!");
			WRITEMSG
			return 1;
		}	
	}  		
	return 0;
}

