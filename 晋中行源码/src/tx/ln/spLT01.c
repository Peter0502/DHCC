/*************************************************
* 文 件 名: spLT01.c
* 功能描述：   
*              贷款放款－委托协议贷款部分
*
* 作    者:    lance
* 完成日期：   2003年04月04日
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
		
spLT01()  
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
	memset(&g_mdm_ac_rel,0x00,sizeof(g_mdm_ac_rel));
	memset(&g_ln_mst,0x00,sizeof(g_ln_mst));
	memset(&g_ln_parm,0x00,sizeof(g_ln_parm));
	vtcp_log("[%s][%d]ttl_prvd_amt===[%s]",__FILE__,__LINE__,g_pub_tx.ac_no);
	vtcp_log("[%s][%d]ttl_prvd_amt===[%d]",__FILE__,__LINE__,g_pub_tx.ac_seqn);
	/* 取贷款信息 */
	/*ret = pub_ln_lndis_file(g_pub_tx.ac_no,g_pub_tx.ac_seqn,
		&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm );
	if( ret ) {return ret;}*/
/******************/		
		ret = Mdm_ac_rel_Sel( g_pub_tx.reply , &g_mdm_ac_rel, "ac_no='%s'", g_pub_tx.ac_no);
		if( ret==100 )
		{
				sprintf( acErrMsg,"凭证(介质)与账户关系表无此记录[%d][%s]",ret,g_pub_tx.ac_no);
				WRITEMSG
				strcpy( g_pub_tx.reply ,"O001" );
				goto ErrExit;
		} 
	else if( ret )
	{
    	sprintf( acErrMsg,"取凭证(介质)与账户关系表异常[%d][%s]",ret,g_pub_tx.ac_no);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"D103" );
    	goto ErrExit;
  }
   	/* 取贷款主文件信息 */
   		vtcp_log("[%s][%d]ttl_prvd_amt===[%lf]",__FILE__,__LINE__,g_ln_mst.ttl_prvd_amt);
   		vtcp_log("[%s][%d]amt_lmt===[%lf]",__FILE__,__LINE__,g_ln_mst.amt_lmt);
	ret = Ln_mst_Sel(g_pub_tx.reply, &g_ln_mst, "ac_id=%ld and ac_seqn=%d" ,
				        g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn );
	if( ret==100 )
	{
		sprintf( acErrMsg,"贷款主文件表中无此记录 erro code=[%d]",ret);
		WRITEMSG
		sprintf( acErrMsg,"mdm_ac_rel->ac_id=[%ld]ac_seqn=[%d]",g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L002" );
		goto ErrExit;

	}		
	else if( ret )
	{
   		sprintf( acErrMsg,"取贷款主文件异常 erro code=[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"D103" );
   		goto ErrExit;
   	}
 vtcp_log("[%s][%d]ttl_prvd_amt===[%lf]",__FILE__,__LINE__,g_ln_mst.ttl_prvd_amt);
   		vtcp_log("[%s][%d]amt_lmt===[%lf]",__FILE__,__LINE__,g_ln_mst.amt_lmt);

   	/* 取贷款参数信息 */
	ret = Ln_parm_Sel(g_pub_tx.reply, &g_ln_parm , " prdt_no='%s' " ,
				        g_ln_mst.prdt_no );
	if( ret==100 )
	{
		sprintf(acErrMsg,"贷款参数文件中无此记录 [%s][%d]",g_ln_mst.prdt_no,ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L001" );
		goto ErrExit;
	} 
	else if( ret )
	{
   		sprintf( acErrMsg,"取贷款参数文件异常 [%s][%d]",g_ln_mst.prdt_no,ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L001" );
   		goto ErrExit;
   	}
/********************/			
	vtcp_log("[%s][%d]ttl_prvd_amt===[%ld]",__FILE__,__LINE__,g_ln_mst.ac_id);
	vtcp_log("[%s][%d]ttl_prvd_amt===[%lf]",__FILE__,__LINE__,g_ln_mst.bal);
	vtcp_log("[%s][%d]ttl_prvd_amt===[%lf]",__FILE__,__LINE__,g_ln_mst.ttl_prvd_amt);
	if ( g_ln_parm.trust_prot_ind[0]=='N' )	
	{
		/* 非委托协议贷款 */
		goto OkExit;
	}else{
		/* 委托协议贷款 */
		/* 取委托协议信息 */
		sprintf(acErrMsg,"~%d@%s trust_no[%s]", __LINE__, __FILE__, g_ln_mst.trust_no);
		WRITEMSG		
		ret = pub_ln_trust_info(g_ln_mst.trust_no, &mo_trust_prot );
		if( ret ) goto ErrExit;
		/* 根据币种取序号 */
		ret = pub_base_CurToSeqn( mo_trust_prot.cur_no , &ac_seqn );
		if ( ret )
		{
			sprintf(acErrMsg,"调用函数根据币种生成账户序号错误!");
			WRITEMSG
			goto ErrExit;	
		}
		
		sprintf(acErrMsg,"[%d]",ac_seqn);
		WRITEMSG		
		/* 检查金额是否超限 */

		ret = pub_base_CompDblVal( mo_trust_prot.trust_amt - mo_trust_prot.out_amt , g_pub_tx.tx_amt1 );
		if( ret<0 )
		{
			sprintf(acErrMsg,"贷款金额超过此委托协议可放款最大金额![%lf][%lf]", 
					mo_trust_prot.trust_amt-mo_trust_prot.out_amt ,g_pub_tx.tx_amt1 );
			WRITEMSG
			strcpy(g_pub_tx.reply,"L108");
			goto ErrExit;
		}
		
		/* 修改已放款金额 */
		ret = sql_execute( "UPDATE mo_trust_prot SET out_amt=%lf WHERE trust_no='%s'" ,g_pub_tx.tx_amt1+mo_trust_prot.out_amt,g_ln_mst.trust_no );
		if( ret ) return ret;
		
		/* 记账 */
		/* 借委托账户 */			
		memset(g_pub_tx.ac_no, 0x0, sizeof(g_pub_tx.ac_no)); /*当ac_no为空时，从ac_id中查找记录*/
		g_pub_tx.ac_id = mo_trust_prot.afund_ac_id;
		g_pub_tx.ac_seqn = ac_seqn ;
		strcpy( g_pub_tx.ac_get_ind,"00");	/*已经读取了mdm_ac_rel*/ /*没读取呀？gujy20060828*/
		strcpy( g_pub_tx.ac_id_type,"1" );	/* 账号类型 1-存款 */
		strcpy( g_pub_tx.add_ind,"0" );		/* 增减:0减1加2销户 */	
		strcpy( g_pub_tx.ct_ind,"2" );		/* 现转:1现2转 */	
		strcpy( g_pub_tx.prt_ind,"0" );/*是否打印存折(有折无折标志):1是0否 */
		strcpy( g_pub_tx.hst_ind,"1" );/*入明细:1日间入2日终单笔入3日终汇总入*/        
		strcpy ( g_pub_tx.intst_type , "1" );	/* 计息类型 */	
		g_pub_tx.svc_ind=1001;	   
		strcpy ( g_pub_tx.brf,"委贷放款－借委托户" );	
		/*** 交易记帐处理 ***/
    /*
			sprintf(acErrMsg,"<<&&>> %d@%s ac_id[%ld] ac_id_type[%s]", __LINE__, __FILE__, g_pub_tx.ac_id,  g_pub_tx.ac_id_type);
			WRITEMSG
		if( pub_acct_trance() )
		{
			sprintf(acErrMsg,"交易记帐处理错误!");
			WRITEMSG
			goto ErrExit;
		}*/
			sprintf(acErrMsg,"<<&&>> %d@%s ac_id[%ld] ac_id_type[%s]", __LINE__, __FILE__, g_pub_tx.ac_id,  g_pub_tx.ac_id_type);
			WRITEMSG
		/* 进行会计记帐 */
		set_zd_data("102J",mo_trust_prot.cur_no);      
		set_zd_double("102F",g_pub_tx.tx_amt1);          
		strcpy(g_pub_tx.sub_tx_code,"D003");/**取款子交易**/
		strcpy(g_pub_tx.prdt_code,g_dd_mst.prdt_no);
		/*	sprintf(acErrMsg,"<<&&>> %d@%s ac_id[%ld] ac_id_type[%s]", __LINE__, __FILE__, g_pub_tx.ac_id,  g_pub_tx.ac_id_type);
			WRITEMSG
		ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
			sprintf(acErrMsg,"<<&&>> %d@%s ac_id[%ld] ac_id_type[%s]", __LINE__, __FILE__, g_pub_tx.ac_id,  g_pub_tx.ac_id_type);
			WRITEMSG
		if (ret != 0)
		{
			sprintf(acErrMsg,"会计记帐不成功!!");
			WRITEMSG
			goto ErrExit;
		}*/
		/* 贷委托贷款户 */
			sprintf(acErrMsg,"<<&&>> %d@%s ac_id[%ld] ac_id_type[%s]", __LINE__, __FILE__, g_pub_tx.ac_id,  g_pub_tx.ac_id_type);
			WRITEMSG
		memset(g_pub_tx.ac_no, 0x0, sizeof(g_pub_tx.ac_no)); /*当ac_no为空时，从ac_id中查找记录*/
		g_pub_tx.ac_id = mo_trust_prot.ln_ac_id;
 		g_pub_tx.ac_seqn = mo_trust_prot.ln_ac_seqn ;
			sprintf(acErrMsg,"<<&&>> %d@%s ac_id[%ld] ac_id_type[%s]", __LINE__, __FILE__, g_pub_tx.ac_id,  g_pub_tx.ac_id_type);
			WRITEMSG
		strcpy( g_pub_tx.ac_get_ind,"00");	/*已经读取了mdm_ac_rel*/ /*重取mdm_ac_rel gujy 20060828*/
		strcpy( g_pub_tx.ac_id_type,"3" );	/* 账号类型 3-贷款 */
		strcpy( g_pub_tx.add_ind,"1" );		/* 增减:0减1加2销户 */	
		strcpy( g_pub_tx.ct_ind,"2" );		/* 现转:1现2转 */	
		strcpy( g_pub_tx.prt_ind,"0" );/*是否打印存折(有折无折标志):1是0否 */
		strcpy( g_pub_tx.hst_ind,"1" );/*入明细:1日间入2日终单笔入3日终汇总入*/        
		strcpy ( g_pub_tx.intst_type , "1" );	/* 计息类型 */	
		g_pub_tx.svc_ind=3010;	
		strcpy ( g_pub_tx.brf,"委贷放款-贷贷户" );
	
			sprintf(acErrMsg,"<<&&>> %d@%s ac_id[%ld] ac_id_type[%s]", __LINE__, __FILE__, g_pub_tx.ac_id,  g_pub_tx.ac_id_type);
		/*** 交易记帐处理 ***/
	/*	if( pub_acct_trance() )
		{
			sprintf(acErrMsg,"交易记帐处理错误!");
			WRITEMSG
			goto ErrExit;
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
			goto ErrExit;
		}*/
		
		
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
	get_zd_data("0370",g_pub_tx.ac_no);		/* 卡/帐号 */
	get_zd_int("0340",&g_pub_tx.ac_seqn);		/* 序号 */	
	get_zd_double("0430",&g_pub_tx.tx_amt1);	/* 发放金额 */
	sprintf(acErrMsg,"发放金额[%lf]!!",g_pub_tx.tx_amt1);
	WRITEMSG	
	sprintf(acErrMsg,"卡/帐号[%s]!!",g_pub_tx.ac_no);
	WRITEMSG
	sprintf(acErrMsg,"序号[%d]!!",g_pub_tx.ac_seqn);
	WRITEMSG							
	return 0;
}
