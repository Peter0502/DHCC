/*************************************************
* 文 件 名: spL307.c
* 功能描述：   
*              委托协议登记
*
* 作    者:    lance
* 完成日期：   2003年01月24日
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
#include "mo_trust_prot_c.h"
	
	static	struct	mo_trust_prot_c		mo_trust_prot;
	static	struct	mo_trust_prot_c		p_mo_trust_prot;
	char 	intst_ac_no[20],ln_acno[20];
	char 	amt_ac_no[20];
	char 	afund_ac_no[20];
	int     ac_seqn;
				
spL307()  
{ 		
	int ret=0;
	long cif_no_back,ac_id_tmp;

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
	/* 检查 委托协议编号 */
	ret = Mo_trust_prot_Sel( g_pub_tx.reply , &p_mo_trust_prot , 
					" trust_no='%s' " , p_mo_trust_prot.trust_no );
	if( ret!=0 && ret!=100)
	{
		sprintf(acErrMsg,"读取委托协议信息异常![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L065");
		ERR_DEAL;
	}else if ( ret == 0 )
	{
		sprintf(acErrMsg,"委托协议编号已存在![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L066");
		ERR_DEAL;
	}
	sprintf(acErrMsg,"检查 委托协议编号 PASS!");
	WRITEMSG
	
	/* 检查账号的合法性 */
	/* 检查 委托户账号 */	
	ret = pub_ln_AcCheck( afund_ac_no , ac_seqn , &cif_no_back , &p_mo_trust_prot.afund_ac_id );
	if( ret )
	{
		sprintf(acErrMsg,"调用函数pub_ln_AcCheck错误!");
		WRITEMSG
		ERR_DEAL;	
	}
	sprintf(acErrMsg,"检查 委托户账号 PASS!");
	WRITEMSG	
	/* 检查 本金归还账号 */	
	ret = pub_ln_AcCheck( amt_ac_no , ac_seqn , &cif_no_back , &p_mo_trust_prot.amt_ac_id );
	if( ret )
	{
		sprintf(acErrMsg,"调用函数pub_ln_AcCheck错误!");
		WRITEMSG
		ERR_DEAL;	
	}
	sprintf(acErrMsg,"检查 本金归还账号 PASS!");
	WRITEMSG
	/* 检查 利息收入帐号 */	
	ret = pub_ln_AcCheck( intst_ac_no , ac_seqn , &cif_no_back , &p_mo_trust_prot.intst_ac_id );
	if( ret )
	{
		sprintf(acErrMsg,"调用函数pub_ln_AcCheck错误!");
		WRITEMSG
		ERR_DEAL;	
	}
	sprintf(acErrMsg,"检查 利息收入帐号 PASS!");
	WRITEMSG
	
	
		
	/* 登记委托协议登记簿 */
	strcpy( p_mo_trust_prot.tx_br_no , g_pub_tx.tx_br_no );/* 交易机构号 */
	sprintf(acErrMsg,"交易机构号[%s]",p_mo_trust_prot.tx_br_no);
	WRITEMSG
	
	ret = Mo_trust_prot_Ins( p_mo_trust_prot , g_pub_tx.reply );
	if (ret)
	{
		sprintf(acErrMsg,"登记委托协议登记簿失败![%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L064");
		goto ErrExit;
	}			
	sprintf(acErrMsg,"登记委托协议登记簿 PASS!");
	WRITEMSG		
			
	/* Pub_tx结构赋值 */

	strcpy ( g_pub_tx.add_ind , "1" );		/* 增减:0减1加2销户 */	
	strcpy ( g_pub_tx.ct_ind , "2" );		/* 现转:1现2转 */	
	/* 是否打印存折(有折无折标志):1是0否 */
	strcpy ( g_pub_tx.prt_ind , "1" );	
	/* 入明细帐标志:1日间入2日终单笔入3日终汇总入 */
	strcpy ( g_pub_tx.hst_ind , "0" );
	g_pub_tx.svc_ind = 3060;		
	strcpy( g_pub_tx.ac_wrk_ind , "0100000" );
	strcpy ( g_pub_tx.brf,"委托协议登记" );	
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
			

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"委托协议登记成功！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"委托协议登记失败！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
    
int data_init_struct()
{	
	memset( &mo_trust_prot , 0x00 , sizeof(struct mo_trust_prot_c) );
	memset( &p_mo_trust_prot , 0x00 , sizeof(struct mo_trust_prot_c) );
	
	return 0;	
}

int data_set_struct()
{    
	int ret=0;
	sprintf(acErrMsg,"进入赋值函数");
	WRITEMSG
			
	get_zd_data("0620",p_mo_trust_prot.trust_no);		/* 委托编号 */
	sprintf(acErrMsg,"委托编号[%s]",p_mo_trust_prot.trust_no);
	WRITEMSG
	
	get_zd_data( "0210" , p_mo_trust_prot.cur_no);	/* 币种 */
	sprintf(acErrMsg,"币种[%s]",p_mo_trust_prot.cur_no);
	WRITEMSG	
	
	get_zd_data("0830",p_mo_trust_prot.name);			/* 委托人 */
	sprintf(acErrMsg,"委托人[%s]",p_mo_trust_prot.name);
	WRITEMSG
	
	get_zd_data("0300",afund_ac_no);		/* 委托账户 */
	sprintf(acErrMsg,"委托账户[%s]",afund_ac_no);
	WRITEMSG
	pub_base_old_acno(afund_ac_no);
	get_zd_data("0310",amt_ac_no);			/* 委托贷款本金归还账号 */
	sprintf(acErrMsg,"委托贷款本金归还账号[%s]",amt_ac_no);
	WRITEMSG
	pub_base_old_acno(amt_ac_no);

	get_zd_data("0320",intst_ac_no);		/* 利息收入帐号 */
	sprintf(acErrMsg,"利息收入帐号[%s]",intst_ac_no);
	WRITEMSG
	pub_base_old_acno(intst_ac_no);
	
	get_zd_data("0700",p_mo_trust_prot.trust_ind);			/* 是否公积金贷款{1-公积金贷款 0-非公积金} */
	sprintf(acErrMsg,"是否公积金贷款[%s]",p_mo_trust_prot.trust_ind);
	WRITEMSG

	get_zd_double("0400",&p_mo_trust_prot.trust_amt);			/* 委托金额 */
	sprintf(acErrMsg,"委托金额[%lf]",p_mo_trust_prot.trust_amt);
	WRITEMSG

	get_zd_data("0370",ln_acno);			/* 委托人贷款账号 */
	sprintf(acErrMsg,"委托人贷款账号[%s]",ln_acno);
	WRITEMSG
	pub_base_old_acno(ln_acno);
	get_zd_long("0340",&p_mo_trust_prot.ln_ac_seqn);		/* 贷款账户序号 */
	sprintf(acErrMsg,"贷款账户序号[%d]",p_mo_trust_prot.ln_ac_seqn);
	WRITEMSG
		
	/* 根据币种生成账户序号 */
	ret = pub_base_CurToSeqn( p_mo_trust_prot.cur_no , &ac_seqn );
	if ( ret )
		{
			sprintf(acErrMsg,"调用函数根据币种生成账户序号错误!");
			WRITEMSG
			return 1;	
		}
	sprintf(acErrMsg,"根据币种生成账户序号 PASS![%s]",p_mo_trust_prot.cur_no);
	WRITEMSG
	/* 调用根据贷款显示账号和账号序号取贷款主文件函数*/
	ret = pub_ln_lndis_file(ln_acno,p_mo_trust_prot.ln_ac_seqn,
			&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm ); /*读取贷款户信息*/
	if( ret )
	{
		sprintf(acErrMsg,"读取贷款户信息error");
		WRITEMSG
		return 1;	
	}
	p_mo_trust_prot.ln_ac_id = g_ln_mst.ac_id ;
	
	sprintf(acErrMsg,"委托人贷款账号[%ld]",p_mo_trust_prot.ln_ac_id);
	WRITEMSG
	TRACE
							
	return 0;
}

