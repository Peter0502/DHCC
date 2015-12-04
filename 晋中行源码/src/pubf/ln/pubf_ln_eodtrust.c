/**********************************************************************
* 函 数 名：  pub_eod_ln_trust
* 函数功能：  日终－委托贷款函数
* 作    者：  rob
* 完成时间：  2004年04月06日
*
* 参    数： 
*     输  入：type   char(1)   类型(1-放款 2-还本 3-还息)    
*      
*     输  出: 
*
*     返回值： 0 成功
*            非0 失败
*
* 修改历史：
*                   
********************************************************************/
#include <stdio.h>  
#define EXTERN
#include "public.h"
#include "mo_trust_prot_c.h"

int pub_eod_ln_trust( struct ln_mst_c p_ln_mst , struct ln_parm_c p_ln_parm , char *type )
{
	int ret = 0;
	int ac_seqn;
	double amt1,amt2,amt3,amt4;
	
	struct mo_trust_prot_c	mo_trust_prot;
	memset (&mo_trust_prot, 0x00, sizeof(struct mo_trust_prot_c));
	vtcp_log("LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL\n");
	if ( p_ln_parm.trust_prot_ind[0]=='N' )	
	{
		/* 非委托协议贷款 */
		return 0;
	}
	
	/* 取委托协议信息 */
	ret = pub_ln_trust_info(p_ln_mst.trust_no, &mo_trust_prot );
	if( ret ) return -1;;
	/* 根据币种取序号 */
	/**
	ret = pub_base_CurToSeqn( mo_trust_prot.cur_no , &ac_seqn );
	if ( ret )
	{
		sprintf(acErrMsg,"调用函数根据币种生成账户序号错误!");
		WRITEMSG
		return -1;;	
	}
	**/
	/********modify by ligl 2006-10-22 19:57*/
	ac_seqn=0;
	/********/
	vtcp_log("LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL[%lf]\n",mo_trust_prot.trust_amt);
	sprintf(acErrMsg,"[%d]",ac_seqn);
	WRITEMSG		
	
	if( type[0]=='1')	/* 放款(只涉及到本金) */
	{
		vtcp_log("byebyebyebyebye[%s][%d]\n",__FILE__,__LINE__);
		return(0);
		/* 检查金额是否超限 */
		ret = pub_base_CompDblVal( mo_trust_prot.trust_amt - mo_trust_prot.out_amt , g_pub_tx.tx_amt1 );
		if( ret<0 )
		{
			sprintf(acErrMsg,"贷款金额超过此委托协议可放款最大金额![%lf][%lf]", 
					mo_trust_prot.trust_amt-mo_trust_prot.out_amt ,g_pub_tx.tx_amt1 );
			WRITEMSG
			strcpy(g_pub_tx.reply,"L108");
			return -1;
		}		
		/* 记账 */
		/* 借委托账户 */			
		g_pub_tx.ac_id = mo_trust_prot.afund_ac_id;
		g_pub_tx.ac_seqn = ac_seqn ;
		/*	 modified by liuxuan 以前是10 造成记帐不对 2006-10-20
		strcpy( g_pub_tx.ac_get_ind,"10");	*已经读取了mdm_ac_rel*
		*/
		strcpy( g_pub_tx.ac_id_type,"1" );	/* 账号类型 1-存款 */
		strcpy( g_pub_tx.add_ind,"0" );		/* 增减:0减1加2销户 */	
		strcpy( g_pub_tx.ct_ind,"2" );		/* 现转:1现2转 */	
		strcpy( g_pub_tx.prt_ind,"0" );/*是否打印存折(有折无折标志):1是0否 */
		strcpy( g_pub_tx.hst_ind,"1" );/*入明细:1日间入2日终单笔入3日终汇总入*/        
		strcpy ( g_pub_tx.intst_type , "1" );	/* 计息类型 */	
		g_pub_tx.svc_ind=1001;	   
		strcpy ( g_pub_tx.brf,"借委托账户" );	
		/*** 交易记帐处理 ***/
		/*if( pub_acct_trance() )
		{
			sprintf(acErrMsg,"交易记帐处理错误!");
			WRITEMSG
			return -1;;
		}*/
		/* 进行会计记帐 */
		/*set_zd_data("102J",mo_trust_prot.cur_no);      
		set_zd_double("102F",g_pub_tx.tx_amt1);          
		strcpy(g_pub_tx.sub_tx_code,"D003");*//**取款子交易**/
		/*strcpy(g_pub_tx.prdt_code,g_dd_mst.prdt_no);
		ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
		if (ret != 0)
		{
			sprintf(acErrMsg,"会计记帐不成功!!");
			WRITEMSG
			return -1;;
		}*/
		/* 贷委托贷款户 */
		g_pub_tx.ac_id = mo_trust_prot.ln_ac_id;
 		g_pub_tx.ac_seqn = mo_trust_prot.ln_ac_seqn ;
		strcpy( g_pub_tx.ac_get_ind,"10");	/*已经读取了mdm_ac_rel*/
		strcpy( g_pub_tx.ac_id_type,"3" );	/* 账号类型 3-贷款 */
		strcpy( g_pub_tx.add_ind,"1" );		/* 增减:0减1加2销户 */	
		strcpy( g_pub_tx.ct_ind,"2" );		/* 现转:1现2转 */	
		strcpy( g_pub_tx.prt_ind,"0" );/*是否打印存折(有折无折标志):1是0否 */
		strcpy( g_pub_tx.hst_ind,"1" );/*入明细:1日间入2日终单笔入3日终汇总入*/        
		strcpy ( g_pub_tx.intst_type , "1" );	/* 计息类型 */	
		g_pub_tx.svc_ind=3010;	
		strcpy ( g_pub_tx.brf,"委托贷款账户" );
	
		/*** 交易记帐处理 ***/
		/*if( pub_acct_trance() )
		{
			sprintf(acErrMsg,"交易记帐处理错误!");
			WRITEMSG
			return -1;;
		}*/
		/* 进行会计记帐 */
		/*set_zd_data("0210",g_ln_parm.cur_no); 
		set_zd_double("0430",g_pub_tx.tx_amt1);                                       
		strcpy(g_pub_tx.sub_tx_code,"L104");
		strcpy(g_pub_tx.prdt_code,g_ln_mst.prdt_no);
		ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
		if (ret != 0)
		{
			sprintf(acErrMsg,"会计记帐不成功!!");
			WRITEMSG
			return -1;;
		}	*/		
	}
	if( type[0]=='2')	/* 还本 */
	{
		vtcp_log("byebyebyebyebye[%s][%d]\n",__FILE__,__LINE__);
		return(0);
		/* 贷委托协议还本账户(是指本金还入的帐户) */			
		g_pub_tx.ac_id = mo_trust_prot.amt_ac_id;
		g_pub_tx.ac_seqn = ac_seqn ;
		/*strcpy( g_pub_tx.ac_get_ind,"10");	*已经读取了mdm_ac_rel*/
		strcpy( g_pub_tx.ac_id_type,"1" );	/* 账号类型 1-存款 */
		strcpy( g_pub_tx.add_ind,"1" );		/* 增减:0减1加2销户 */	
		strcpy( g_pub_tx.ct_ind,"2" );		/* 现转:1现2转 */	
		strcpy( g_pub_tx.prt_ind,"0" );/*是否打印存折(有折无折标志):1是0否 */
		strcpy( g_pub_tx.hst_ind,"1" );/*入明细:1日间入2日终单笔入3日终汇总入*/        
		strcpy ( g_pub_tx.intst_type , "1" );	/* 计息类型 */	
		g_pub_tx.svc_ind=1001;	   
		strcpy ( g_pub_tx.brf,"贷委托还本账户" );	
		/*** 交易记帐处理 ***/
		/*if( pub_acct_trance() )
		{
			sprintf(acErrMsg,"交易记帐处理错误!");
			WRITEMSG
			return -1;;
		}*/
		/* 进行会计记帐 */
		set_zd_data("101A",mo_trust_prot.cur_no);         
		set_zd_double("1013",g_pub_tx.tx_amt1);                     
		strcpy(g_pub_tx.sub_tx_code,"D099");
		strcpy(g_pub_tx.prdt_code,g_dd_mst.prdt_no);
		/*ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
		if (ret != 0)
		{
			sprintf(acErrMsg,"会计记帐不成功!!");
			WRITEMSG
			return -1;;
		}*/
		/* 贷委托贷款户 */
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
		strcpy ( g_pub_tx.brf,"借委托贷款账户" );
	
		/*** 交易记帐处理 ***/
		/*if( pub_acct_trance() )
		{
			sprintf(acErrMsg,"交易记帐处理错误!");
			WRITEMSG
			return -1;;
		}*/
		/* 进行会计记帐 */
		set_zd_data("0210",g_ln_parm.cur_no); 
		set_zd_double("107F",g_pub_tx.tx_amt1);                                       
		strcpy(g_pub_tx.sub_tx_code,"L201");
		strcpy(g_pub_tx.prdt_code,g_ln_mst.prdt_no);
		/*ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
		if (ret != 0)
		{
			sprintf(acErrMsg,"会计记帐不成功!!");
			WRITEMSG
			return -1;;
		}*/
	}		
	if( type[0]=='3')	/* 还息 */
	{
		vtcp_log("LLLLLL进还息了LLLLLLLLLLLLLLLLLLLLLL[%ld]\n",mo_trust_prot.intst_ac_id);
		amt1 = g_pub_tx.tx_amt1;
		amt2 = g_pub_tx.tx_amt2;
		amt3 = g_pub_tx.tx_amt3;
		amt4 = g_pub_tx.tx_amt4;
		g_pub_tx.tx_amt1 += g_pub_tx.tx_amt2+g_pub_tx.tx_amt3+g_pub_tx.tx_amt4;					
		/* 记账 */
		/* 贷委托协议贷款还息账户 */
			vtcp_log("LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL[%ld]\n",mo_trust_prot.intst_ac_id);

		memset(&g_mdm_ac_rel,0x0,sizeof(struct mdm_ac_rel_c));
		ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_id=%ld",mo_trust_prot.intst_ac_id);
		if(ret) return -1;
		memcpy(g_pub_tx.ac_no,g_mdm_ac_rel.ac_no,sizeof(g_pub_tx.ac_no));
			vtcp_log("LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL[%s]\n",g_pub_tx.ac_no);
		g_pub_tx.ac_id = mo_trust_prot.intst_ac_id;
		g_pub_tx.ac_seqn = ac_seqn ;
		strcpy( g_pub_tx.ac_get_ind,"10");	/*已经读取了mdm_ac_rel*/
		strcpy( g_pub_tx.ac_id_type,"1" );	/* 账号类型 1-存款 */
		strcpy( g_pub_tx.add_ind,"1" );		/* 增减:0减1加2销户 */	
		strcpy( g_pub_tx.ct_ind,"2" );		/* 现转:1现2转 */	
		strcpy( g_pub_tx.prt_ind,"0" );/*是否打印存折(有折无折标志):1是0否 */
		strcpy( g_pub_tx.hst_ind,"1" );/*入明细:1日间入2日终单笔入3日终汇总入*/        
		strcpy ( g_pub_tx.intst_type , "1" );	/* 计息类型 */	
		g_pub_tx.svc_ind=1001;	   
		strcpy ( g_pub_tx.brf,"委托还息" );	
		/*** 交易记帐处理 ***/
		if( pub_acct_trance() )
		{
			sprintf(acErrMsg,"交易记帐处理错误!");
			WRITEMSG
			return -1;;
		}
		/* 进行会计记帐 */
		set_zd_data("101A",mo_trust_prot.cur_no);         
		set_zd_double("1013",g_pub_tx.tx_amt1);                     
		strcpy(g_pub_tx.sub_tx_code,"D099");
		strcpy(g_pub_tx.prdt_code,g_dd_mst.prdt_no);
		ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
		if (ret != 0)
		{
			sprintf(acErrMsg,"会计记帐不成功!!");
			WRITEMSG
			return -1;;
		}
		/* 贷－501 利息收益 */
		memset(&g_mdm_ac_rel,0x0,sizeof(struct mdm_ac_rel_c));
		ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_id=%ld",mo_trust_prot.ln_ac_id);
		if(ret) return -1;
		memcpy(g_pub_tx.ac_no,g_mdm_ac_rel.ac_no,sizeof(g_pub_tx.ac_no));
		vtcp_log("LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL[%lf]\n",g_pub_tx.ac_no);
		g_pub_tx.ac_id = mo_trust_prot.ln_ac_id;
 		g_pub_tx.ac_seqn = mo_trust_prot.ln_ac_seqn ;
 		g_pub_tx.tx_amt1 = -g_pub_tx.tx_amt1;
		strcpy( g_pub_tx.ac_get_ind,"10");	/*已经读取了mdm_ac_rel*/
		strcpy( g_pub_tx.ac_id_type,"3" );	/* 账号类型 3-贷款 */
		strcpy( g_pub_tx.add_ind,"1" );		/* 增减:0减1加2销户 */	
		strcpy( g_pub_tx.ct_ind,"2" );		/* 现转:1现2转 */	
		strcpy( g_pub_tx.prt_ind,"0" );/*是否打印存折(有折无折标志):1是0否 */
		strcpy( g_pub_tx.hst_ind,"1" );/*入明细:1日间入2日终单笔入3日终汇总入*/        
		strcpy ( g_pub_tx.intst_type , "1" );	/* 计息类型 */	
		g_pub_tx.svc_ind=3010;	
		strcpy ( g_pub_tx.brf,"利息收益" );
	
		/*** 交易记帐处理 ***/
		/*if( pub_acct_trance() )
		{
			sprintf(acErrMsg,"交易记帐处理错误!");
			WRITEMSG
			return -1;;
		}*/
		/* 进行会计记帐 */
		set_zd_data("0210",g_ln_parm.cur_no); 
		set_zd_double("108H",g_pub_tx.tx_amt1);                                       
		strcpy(g_pub_tx.sub_tx_code,"LN03");
		strcpy(g_pub_tx.prdt_code,g_ln_mst.prdt_no);
		ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
		if (ret != 0)
		{
			sprintf(acErrMsg,"会计记帐不成功!!");
			WRITEMSG
			return -1;;
		}
		g_pub_tx.tx_amt1 = amt1;
		g_pub_tx.tx_amt2 = amt2;
		g_pub_tx.tx_amt3 = amt3;
		g_pub_tx.tx_amt4 = amt4;	
	}
	return 0;
}

