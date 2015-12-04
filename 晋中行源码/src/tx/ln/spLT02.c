/*************************************************
* 文 件 名: spLT02.c
* 功能描述：   
*              贷款还本－委托协议贷款部分
*
* 作    者:    lance
* 完成日期：   2003年04月05日
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

static	struct mo_trust_prot_c	mo_trust_prot;
		
spLT02()  
{ 	
	int ret=0;
	int ac_seqn;


	memset (&mo_trust_prot, 0x00, sizeof(struct mo_trust_prot_c));

	/** 初始化全局变量 **/
	pub_base_sysinit();
	
	/** 取值、赋值 **/
	if ( data_set_struct() )        ERR_DEAL;
	sprintf(acErrMsg,"取值、赋值 PASS!");
	WRITEMSG
	
	/* 取贷款信息 */
	ret = pub_ln_lndis_file(g_pub_tx.ac_no,g_pub_tx.ac_seqn,
		&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm );
	if( ret ) {return ret;}	
	
	if ( g_ln_parm.trust_prot_ind[0]=='N' )	
	{
		/* 非委托协议贷款 */
		goto OkExit;
	}else{
		/* 委托协议贷款 */
		/* 取委托协议信息 */
		ret = pub_ln_trust_info(g_ln_mst.trust_no, &mo_trust_prot );
		vtcp_log("[%s][%d] 开户日期: [%ld] \n",__FILE__,__LINE__,g_ln_mst.opn_date);
		/****对老协议贷款的处理   add 20070824****/
		if(g_ln_mst.opn_date < 20070501)
		{
			if(ret == 100)
			{
				goto OkExit;
			}
		}
		/****对老协议贷款的处理  end  20070824****/
		else if( ret ) 
		{
			goto ErrExit;
		}
		/* 根据币种取序号 */
		ret = pub_base_CurToSeqn( mo_trust_prot.cur_no , &ac_seqn );
		if ( ret )
		{
			sprintf(acErrMsg,"调用函数根据币种生成账户序号错误!");
			WRITEMSG
			goto ErrExit;	
		}
		ac_seqn=0;
		sprintf(acErrMsg,"[%d]",ac_seqn);
		WRITEMSG		
		
		/* 记账 */
		/* 贷委托协议还本账户 */			
		memset(&g_mdm_ac_rel,0x0,sizeof(struct mdm_ac_rel_c));
		ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_id=%ld",mo_trust_prot.amt_ac_id);
		if(ret) goto ErrExit;
		
		memcpy(g_pub_tx.ac_no,g_mdm_ac_rel.ac_no,sizeof(g_pub_tx.ac_no));
		g_pub_tx.ac_id = mo_trust_prot.amt_ac_id;
		g_pub_tx.ac_seqn = ac_seqn ;
		strcpy( g_pub_tx.ac_get_ind,"10");	/*已经读取了mdm_ac_rel*/
		strcpy( g_pub_tx.ac_id_type,"1" );	/* 账号类型 1-存款 */
		strcpy( g_pub_tx.add_ind,"1" );		/* 增减:0减1加2销户 */	
		strcpy( g_pub_tx.ct_ind,"2" );		/* 现转:1现2转 */	
		strcpy( g_pub_tx.prt_ind,"0" );/*是否打印存折(有折无折标志):1是0否 */
		strcpy( g_pub_tx.hst_ind,"1" );/*入明细:1日间入2日终单笔入3日终汇总入*/        
		strcpy ( g_pub_tx.intst_type , "1" );	/* 计息类型 */	
		g_pub_tx.svc_ind=1001;	   
		strcpy ( g_pub_tx.brf,"委托还本" );	
		/*** 交易记帐处理 ***/
		/*if( pub_acct_trance() )
		{
			sprintf(acErrMsg,"交易记帐处理错误!");
			WRITEMSG
			goto ErrExit;
		}*/
		/* 进行会计记帐 */
	/*	set_zd_data("101A",mo_trust_prot.cur_no);         
		set_zd_double("1013",g_pub_tx.tx_amt1);                     
		strcpy(g_pub_tx.sub_tx_code,"D099");
		strcpy(g_pub_tx.prdt_code,g_dd_mst.prdt_no);
		ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
		if (ret != 0)
		{
			sprintf(acErrMsg,"会计记帐不成功!!");
			WRITEMSG
			goto ErrExit;
		}*/
		/* 贷委托贷款户 */
		memset(&g_mdm_ac_rel,0x0,sizeof(struct mdm_ac_rel_c));
		ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_id=%ld",mo_trust_prot.ln_ac_id);
		if(ret) goto ErrExit;
		memcpy(g_pub_tx.ac_no,g_mdm_ac_rel.ac_no,sizeof(g_pub_tx.ac_no));
		g_pub_tx.ac_id = mo_trust_prot.ln_ac_id;
 		g_pub_tx.ac_seqn = mo_trust_prot.ln_ac_seqn ;
		strcpy( g_pub_tx.ac_get_ind,"10");	/*已经读取了mdm_ac_rel*/
		strcpy( g_pub_tx.ac_id_type,"3" );	/* 账号类型 3-贷款 */
		strcpy( g_pub_tx.add_ind,"0" );		/* 增减:0减1加2销户 */	
		strcpy( g_pub_tx.ct_ind,"2" );		/* 现转:1现2转 */	
		strcpy( g_pub_tx.prt_ind,"0" );/*是否打印存折(有折无折标志):1是0否 */
		strcpy( g_pub_tx.hst_ind,"1" );/*入明细:1日间入2日终单笔入3日终汇总入*/        
		strcpy ( g_pub_tx.intst_type , "1" );	/* 计息类型 */	
		g_pub_tx.svc_ind=3010;	
		strcpy ( g_pub_tx.brf,"委托贷款" );
	
		/*** 交易记帐处理 ***/
		/*if( pub_acct_trance() )
		{
			sprintf(acErrMsg,"交易记帐处理错误!");
			WRITEMSG
			goto ErrExit;
		}*/
		/* 进行会计记帐 */
		/*set_zd_data("0210",g_ln_parm.cur_no); 
		set_zd_double("107F",g_pub_tx.tx_amt1);                                       
		strcpy(g_pub_tx.sub_tx_code,"L201");
		strcpy(g_pub_tx.prdt_code,g_ln_mst.prdt_no);
		ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
		if (ret != 0)
		{
			sprintf(acErrMsg,"会计记帐不成功!!");
			WRITEMSG
			goto ErrExit;
		}
		*/
		
	}				  
		  		  				  	
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
	g_ln_mst.opn_date = g_pub_tx.tx_date;		/* 开户日期 */
	get_zd_data("1071",g_pub_tx.ac_no);		/* 卡/帐号 */
	get_zd_int("1072",&g_pub_tx.ac_seqn);		/* 序号 */	
	get_zd_double("107E",&g_pub_tx.tx_amt1);	/* 发放金额 */
	sprintf(acErrMsg,"发放金额[%lf]!!",g_pub_tx.tx_amt1);
	WRITEMSG	
	sprintf(acErrMsg,"卡/帐号[%s]!!",g_pub_tx.ac_no);
	WRITEMSG
	sprintf(acErrMsg,"序号[%d]!!",g_pub_tx.ac_seqn);
	WRITEMSG							
	return 0;
}
