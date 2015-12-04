/*************************************************
* 文 件 名: spL205.c
* 功能描述：   
*              贷款销户
*
* 作    者:    lance
* 完成日期：   2003年01月21日
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
#include "ln_gage_reg_c.h"
#include "ln_pay_pln_c.h"
#include "mo_po_co_c.h"
static struct ln_mst_c    ln_mst_upd ;
static struct ln_pay_pln_c   ln_pay_pln_upd ;
#include "mob_acct_type_c.h" /*add by zgf 2013-07-25*/
spL205()  
{
	int ret=0;
	double intst=0.00;
	struct ln_gage_reg_c ln_gage_reg;
	struct mo_po_co_c  sMo_po_co;
	struct mob_acct_type_c st_mob_acct_type;/*add by zgf 2013-07-25*/

	memset (&ln_gage_reg,0x00,sizeof(struct ln_gage_reg_c));
	memset (&sMo_po_co,0x00,sizeof(sMo_po_co));
	/** 初始化全局变量 **/
	pub_base_sysinit();
	sprintf(acErrMsg,"初始化全局变量 PASS!");
	WRITEMSG	
	
	/** 数据初始化 **/
	if ( data_init_struct() )	ERR_DEAL;
	sprintf(acErrMsg,"数据初始化 PASS!");
	WRITEMSG

	/** 取值、赋值 **/
	if ( data_get_struct() )        ERR_DEAL;
	sprintf(acErrMsg,"取值、赋值 PASS!");
	WRITEMSG
	vtcp_log("AAAAAAAAAAAAAAAAA\n");

	/** 处理流程 **/
	/* 查询贷款主文件 贷款本金 */
	if (g_ln_mst.bal)
	{
		sprintf( acErrMsg, "贷款本金未还清,不能销户[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L049");
		ERR_DEAL
	}	
	
	 /*增加控制短信存在签约信息不能进行账户销户,必须先进行解约----add by zgf 2013-07-25*/
	  memset(&st_mob_acct_type,0x00,sizeof(st_mob_acct_type));
	  ret=Mob_acct_type_Sel(g_pub_tx.reply,&st_mob_acct_type,"ac_no='%s' and mob_sts='1' ",g_ln_mst.pact_no);
	  if(ret==100){
	          vtcp_log("[%s][%d]账号[%s]未查询到短信签约记录,可以销户!!!",__FILE__,__LINE__,g_ln_mst.pact_no);
	  }else if(ret==0){
	          vtcp_log("[%s][%d]账号[%s]存在短信签约记录!!!",__FILE__,__LINE__,g_ln_mst.pact_no);
	          sprintf(acErrMsg, "该账号存在短信银行签约尚未解约!!");
	          strcpy(g_pub_tx.reply,"F037");
	          set_zd_data(DC_GET_MSG,acErrMsg);
	          goto ErrExit;
	
	  }else{
	          vtcp_log("[%s][%d]查询短信银行签约表错误!!!",__FILE__,__LINE__,g_ln_mst.pact_no);
	          sprintf(acErrMsg, "查询短信银行签约表错误!!");
	          strcpy(g_pub_tx.reply,"XXXX");
	          set_zd_data(DC_GET_MSG,acErrMsg);
	          goto ErrExit;
	  }
	  /*end add by zgf*/
	
	if(memcmp(g_ln_mst.prdt_no,"3Y1",3))
	{		
	/* 查询贷款主文件 欠息 */
      vtcp_log("[%lf]%%%%%%%%%%%%wzzwzz",g_ln_mst.cmpd_lo_intst);
       vtcp_log("[%lf]%%%%%%%%%%%%wzzwzz",g_ln_mst.in_lo_intst); 
      vtcp_log("[%lf]%%%%%%%%%%%%wzzwzz",g_ln_mst.out_lo_intst);
	double tmp_amt=g_ln_mst.in_lo_intst+g_ln_mst.out_lo_intst+g_ln_mst.cmpd_lo_intst;
	vtcp_log("1111111tmp_amt[%.2f]",tmp_amt);
	if (pub_base_CompDblVal(tmp_amt , 0.00) )
	/*if (pub_base_CompDblVal(g_ln_mst.in_lo_intst+g_ln_mst.out_lo_intst+g_ln_mst.cmpd_lo_intst , 0.00) )*/
	{
		sprintf( acErrMsg, "贷款欠息未还清,不能销户[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L050");
		ERR_DEAL
	}				
	/* 查询贷款主文件 积数 */
	if (pub_base_CompDblVal(g_ln_mst.intst_acm+g_ln_mst.in_lo_acm+g_ln_mst.out_lo_acm+g_ln_mst.cmpd_lo_acm , 0.00) && g_ln_mst.intst_type[0]!='0')
	{
		sprintf( acErrMsg, "贷款有积数未还清,不能销户[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L051");
		ERR_DEAL
	}	

	/* 分段积数利息 */
	ret = pub_base_Get_SectIntst( g_ln_mst.ac_id , g_ln_mst.ac_seqn , "1" , &intst , 1 );
	if( ret ) goto ErrExit;
	if ( pub_base_CompDblVal(intst , 0.00) )
	{
		sprintf( acErrMsg, "贷款利息未还清,不能销户[%lf]",intst);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L052");
		ERR_DEAL
	}
		vtcp_log("AAAAAAAAAAAAAAAAAzzzzz\n");
	ret = pub_base_Get_SectIntst( g_ln_mst.ac_id , g_ln_mst.ac_seqn , "2" , &intst , 1 );	
	if( ret ) goto ErrExit;
	if (pub_base_CompDblVal(intst , 0.00) )
	{
		sprintf( acErrMsg, "贷款利息未还清,不能销户[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L052");
		ERR_DEAL
	}
	
	ret = pub_base_Get_SectIntst( g_ln_mst.ac_id , g_ln_mst.ac_seqn , "3" , &intst , 1 );
	if( ret ) goto ErrExit;
	if (pub_base_CompDblVal(intst , 0.00))
	{
		sprintf( acErrMsg, "贷款利息未还清,不能销户[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L052");
		ERR_DEAL
	}
	
	ret = pub_base_Get_SectIntst( g_ln_mst.ac_id , g_ln_mst.ac_seqn , "4" , &intst , 1 );
	if( ret ) goto ErrExit;
	if (pub_base_CompDblVal(intst , 0.00))
	{
		sprintf( acErrMsg, "贷款利息未还清,不能销户[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L052");
		ERR_DEAL
	}
}
	/* 若为按揭贷款 */
	if( g_ln_parm.ln_pay_type[0]=='3' || g_ln_parm.ln_pay_type[0]=='4' )
	{
		sprintf(acErrMsg,"按揭贷款");
		WRITEMSG
	
		/* 查询按揭欠款表 */
		/*ret = sql_count( "ln_lo" , "ac_id=%ld and ac_seqn=%d  and pay_type='0'" , g_pub_tx.ac_id,g_pub_tx.ac_seqn ); mod time 20120103*/
		ret = sql_count( "ln_lo" , "ac_id=%ld and ac_seqn=%d  and pay_type='0'" , g_ln_mst.ac_id,g_ln_mst.ac_seqn );
		if (ret)
		{
			sprintf( acErrMsg, "该按揭贷款存在欠款,不能销户[%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L053");
			ERR_DEAL
		}		
	}
	/** add by chenchao 增加控制,在销户之前必须做出库交易   2011/9/16 16:14:00 **/
	ret = Mo_po_co_Sel(g_pub_tx.reply,&sMo_po_co,"pact_no='%s'",g_ln_mst.pact_no);
	if(ret && ret!=100 )
	{
		sprintf( acErrMsg, "查询合同借据关系表出错[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L053");
		ERR_DEAL
	}else if(ret == 0)
	{
		ret = sql_count("ln_mst","ac_id in (select ac_id from ln_gage_reg where pact_no='%s' and sts='0') and ac_sts<>'*'",sMo_po_co.contract_no);
		if(ret<0)
		{
			sprintf( acErrMsg, "根据抵质押物查询贷款失败[%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L053");
			ERR_DEAL
		}else if(ret==1)
		{
			sprintf( acErrMsg, "还有抵质押物未出库,请先做出库交易pact_no=[%s]",g_ln_mst.pact_no);
			WRITEMSG
			strcpy(g_pub_tx.reply,"T056");
			ERR_DEAL
		}
	}
	/**** add by chenchao end 2011/9/16 16:29:18 ***/
	
	/* Pub_tx结构赋值 */
	g_pub_tx.ac_id = g_ln_mst.ac_id;		/* 账户序号 */
	g_pub_tx.ac_seqn = g_ln_mst.ac_seqn;	/* 账户序号 */
	strcpy ( g_pub_tx.ac_id_type , "3" );	/* 账号类型 3-贷款 */
	strcpy ( g_pub_tx.intst_type , "1" );	/* 计息类型 */	
	strcpy ( g_pub_tx.add_ind , "2" );		/* 增减:0减1加2销户 */	
	strcpy ( g_pub_tx.ct_ind , "2" );		/* 现转:1现2转 */	
	strcpy ( g_pub_tx.prt_ind , "0" );	/*是否打印存折(有折无折标志):1是0否 */
	strcpy ( g_pub_tx.hst_ind , "1" );	/*入明细:1日间入2日终单笔入3日终汇总入*/
	strcpy( g_pub_tx.ac_wrk_ind , "0110000" );
	strcpy ( g_pub_tx.brf,"贷款销户" );
	g_pub_tx.svc_ind = 2;				/* 销户 */

	sprintf(acErrMsg,"Pub_tx结构赋值 PASS!");
	WRITEMSG

	/*** 交易记帐处理 ***/
	if( pub_acct_trance() )
	{
		sprintf(acErrMsg,"交易记帐处理错误!");
		WRITEMSG
		goto ErrExit;
	}
	sprintf(acErrMsg,"交易记帐处理 PASS!");
	WRITEMSG

	/* 更新贷款主文件 */	
/****	
g_reply_int=sql_execute("update ln_mst set ac_sts='*' \
			where ac_id=%ld and ac_seqn=%d ",g_ln_mst.ac_id,g_ln_mst.ac_seqn);
*****/
	
	ret = Ln_mst_Dec_Upd( g_pub_tx.reply , " ac_id=%ld and ac_seqn=%d " , g_ln_mst.ac_id,g_ln_mst.ac_seqn );
	if (ret != 0)
	{
		sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR [%d]",ret);
		WRITEMSG
		goto ErrExit;
	}
	ret = Ln_mst_Fet_Upd( &ln_mst_upd , g_pub_tx.reply );
	if (ret != 0 && ret != 100)
	{
		sprintf(acErrMsg,"FETCH ERROR [%d]",ret);
		WRITEMSG
		goto ErrExit;
	}else if (ret == 100){
		goto ErrExit;
	}
	strcpy( ln_mst_upd.ac_sts,"*" );
	ln_mst_upd.lst_date=g_pub_tx.tx_date;
	ret = Ln_mst_Upd_Upd( ln_mst_upd , g_pub_tx.reply  );
        if (ret != 0)
        {
               sprintf(acErrMsg,"更新贷款主文件错误!!");
               WRITEMSG
               goto ErrExit;
        }  				
	Ln_mst_Clo_Upd();
	/***对于按揭贷款销户与还款计划表的修改，将pay_ind置为'1'***/
	if( g_ln_parm.ln_pay_type[0]=='3' || g_ln_parm.ln_pay_type[0]=='4' )
	{
		sprintf(acErrMsg,"按揭贷款");
		WRITEMSG
		
			ret = Ln_pay_pln_Dec_Upd( g_pub_tx.reply , " ac_id=%ld and ac_seqn=%d " , g_ln_mst.ac_id,g_ln_mst.ac_seqn );
		if (ret != 0)
		{
			sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR [%d]",ret);
			WRITEMSG
			goto ErrExit;
		}
		ret = Ln_pay_pln_Fet_Upd( &ln_pay_pln_upd , g_pub_tx.reply );
		if (ret != 0 && ret != 100)
		{
			sprintf(acErrMsg,"FETCH ERROR [%d]",ret);
			WRITEMSG
			goto ErrExit;
		}else if (ret == 100){
			goto ErrExit;
		}
		strcpy( ln_pay_pln_upd.pay_ind , "1" );
		ret = Ln_pay_pln_Upd_Upd( ln_pay_pln_upd , g_pub_tx.reply  );
        if (ret != 0)
        {
               sprintf(acErrMsg,"更新贷款主文件错误!!");
               WRITEMSG
               goto ErrExit;
        }  				
				Ln_pay_pln_Clo_Upd();
	}

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"贷款销户成功！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"贷款销户失败！[%s]",g_pub_tx.reply);
    WRITEMSG
	if ((strcmp(g_pub_tx.reply,"0000")==0) || (strcmp(g_pub_tx.reply,"")==0) || \
		 g_pub_tx.reply[0] == '\0')
	{
		strcpy(g_pub_tx.reply,"D114");
	}
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
    
int data_init_struct()
{
	memset (&ln_mst_upd, 0x00, sizeof(struct ln_mst_c));
	memset (&ln_pay_pln_upd, 0x00, sizeof(struct ln_pay_pln_c));
	memset (&g_ln_mst, 0x00, sizeof(struct ln_mst_c));
	memset (&g_ln_parm, 0x00, sizeof(struct ln_parm_c));
	memset (&g_dd_mst, 0x00, sizeof(struct dd_mst_c));
	memset (&g_mdm_ac_rel, 0x00, sizeof(struct mdm_ac_rel_c));
		
	return 0;
	
}
int data_get_struct()
{    
	int ret=0;

	sprintf(acErrMsg,"进入赋值函数");
	WRITEMSG

	get_zd_data("0370",g_pub_tx.ac_no);		/* 帐号 */
	get_zd_int("0340",&g_pub_tx.ac_seqn);		/* 账户序号 */
	pub_base_old_acno(g_pub_tx.ac_no);
	/*读取贷款户信息*/
	ret = pub_ln_lndis_file(g_pub_tx.ac_no,g_pub_tx.ac_seqn,&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm );
	if( ret ) {return ret;}
/*
	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",g_pub_tx.ac_no);
	if( ret ) {return ret;}
	ret = Ln_mst_Sel( g_pub_tx.reply ,&g_ln_mst, " ac_id=%ld and ac_seqn=%d",g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_seqn);
	if( ret ) {return ret;}
	double tmp_amt=g_ln_mst.in_lo_intst+g_ln_mst.out_lo_intst+g_ln_mst.cmpd_lo_intst;
	if (pub_base_CompDblVal(tmp_amt , 0.00) )
	{
		sprintf( acErrMsg, "贷款欠息未还清,不能销户[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L050");
		return(-1);
	}
*/
	/*ret = pub_base_check_mdm_sts( g_mdm_ac_rel ) *检查介质状态*
	if( ret ) return ret; */


	ret = pub_ln_check_sts( g_ln_mst,0 ); /*检查贷款户状态*/
	if( ret ) {return ret;}
					

	return 0;
}
