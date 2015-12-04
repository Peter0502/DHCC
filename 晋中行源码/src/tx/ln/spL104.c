/*************************************************
* 文 件 名: spL104.c
* 功能描述：   
*              贷款放款
*
* 作    者:    lance
* 完成日期：   2003年01月14日
* 修改记录：   
*     1. 日    期:20110826
*        修 改 人:gongliangliang
*        修改内容:添加该借据号是否有抵质押物品的查询
*     2. 
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "stdio.h" 
#define EXTERN
#include "public.h"
#include "trace_log_c.h"
#include "ln_auth_c.h"
#include "ln_mst_c.h"
#include "mo_po_co_c.h"
#include "ln_gage_reg_c.h"
		
		char ct_flag[2];
		char de_ac_no[21];
		char de_ac_name[61];
spL104()  
{ 		
	int ret=0;
	char save_ac_no[20];
	
	struct mdm_ac_rel_c	mdm_ac_rel_tmp;
	struct ln_auth_c	ln_auth_tmp;
	struct ln_mst_c ln_mst_tmp;
	struct mo_po_co_c	sMo_po_co;
	struct ln_gage_reg_c	sLn_gage_reg;
	
	/** 数据初始化 **/
	memset( &mdm_ac_rel_tmp,0x00,sizeof(struct mdm_ac_rel_c) );
	memset( &ln_auth_tmp,0x00,sizeof(struct ln_auth_c) );	
	memset(ct_flag,0x00,sizeof(ct_flag));	
	memset(&ln_mst_tmp,0x00,sizeof(struct ln_mst_c));
	memset (&sMo_po_co, 0x00, sizeof(sMo_po_co));
	memset (&sLn_gage_reg, 0x00, sizeof(sLn_gage_reg));
	/** 初始化全局变量 **/
	pub_base_sysinit();
	
	sprintf(acErrMsg,"初始化全局变量 PASS!");
	WRITEMSG	
		
	/** 取值、赋值 **/
	if ( data_set_struct() )        ERR_DEAL;
	
	sprintf(acErrMsg,"取值、赋值 PASS!");
	WRITEMSG
	
	/*对该借据号是否有抵质押物品入库进行校验*/
	/*add by gong 20110826 晋中商业银行*/
	if(memcmp(g_ln_parm.prdt_no,"3Z",2) != 0)
	{
		ret = Ln_mst_Sel(g_pub_tx.reply,&ln_mst_tmp,"ac_id in (select ac_id from mdm_ac_rel where ac_no='%s' )",g_pub_tx.ac_no);
		if(ret)
		{
			sprintf( acErrMsg, "查询贷款主文件表出错!ac_no[%s]ret=[%d]" ,g_pub_tx.ac_no ,ret);
			WRITEMSG
			strcpy( g_pub_tx.reply, "B110" );
			goto ErrExit;
		}
		
		ret = Mo_po_co_Sel(g_pub_tx.reply,&sMo_po_co,"pact_no='%s'",ln_mst_tmp.pact_no);
		if(ret)
		{
			sprintf(acErrMsg,"查询合同借据关系表出错!!");
			WRITEMSG
			strcpy( g_pub_tx.reply ,"D107" );
			goto ErrExit;
		}
		
		ret = Ln_gage_reg_Sel(g_pub_tx.reply,&sLn_gage_reg,"pact_no='%s' ",sMo_po_co.contract_no);
		if(ret==0)
		{
			ret=sql_count("ln_gage_reg","sts in('X','x') and pact_no ='%s'",sLn_gage_reg.pact_no);
			if(ret>0)
			{
				memset(acErrMsg, 0 , sizeof(acErrMsg));
				sprintf( acErrMsg,"您存在%d笔抵质押物品需要入库",ret);
				set_zd_data(DC_GET_MSG,acErrMsg);
				WRITEMSG
				strcpy(g_pub_tx.reply,"L350");
				goto ErrExit;
			}
		}else if(ret&&ret!=100){
			vtcp_log("[%s][%d]数据库查询出错！！！",__FILE__,__LINE__);
			goto ErrExit;
		}
	}
	/*end by gong 20110826 晋中商业银行*/
	
	/** 处理流程 **/
	/* 按揭贷款不能重复放款 */
	/* 按揭贷款发放金额必须和计划发放金额相同 */
	if ( g_ln_parm.ln_pay_type[0]=='3'||g_ln_parm.ln_pay_type[0]=='4' )
	{
		ret = pub_base_CompDblVal(g_ln_mst.bal,0.00);
		if( ret!=0 )
		{
			sprintf( acErrMsg,"此按揭贷款已发放,不可重复发放",ret);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"L110" );
			ERR_DEAL
		}
		ret = pub_base_CompDblVal(g_ln_mst.amt_lmt,g_pub_tx.tx_amt1);
		if( ret!=0 )
		{
			sprintf( acErrMsg,"金额错误,按揭贷款必须一次性发放",ret);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"L111" );
			ERR_DEAL
		}
	}						
	/* Pub_tx结构赋值 */

	strcpy ( g_pub_tx.add_ind , "1" );		/* 增减:0减1加2销户 */	

	/* 是否打印存折(有折无折标志):1是0否 */
	strcpy ( g_pub_tx.prt_ind , "0" );	
	/* 入明细帐标志:1日间入2日终单笔入3日终汇总入 */
	strcpy ( g_pub_tx.hst_ind , "1" );
	g_pub_tx.svc_ind = 3010 ;
	strcpy( g_pub_tx.brf,"贷款放款" );
	
	sprintf(acErrMsg,"Pub_tx结构赋值 PASS!");
	WRITEMSG
	
	sprintf(acErrMsg,"贷款账户序号[%d]",g_pub_tx.ac_seqn);
	WRITEMSG

vtcp_log("JJJJ调试ttl=[%f]",g_ln_mst.ttl_prvd_amt);
	/*** 交易记帐处理 ***/
	if( pub_acct_trance() )
	{
		sprintf(acErrMsg,"交易记帐处理错误!");
		WRITEMSG
		goto ErrExit;
	}
	sprintf(acErrMsg,"交易记帐处理 PASS!");
	WRITEMSG

	/* 登记贷款登记薄 */
	ret = pub_ln_ln_reg( g_ln_mst,g_ln_mst.opn_date,g_ln_mst.mtr_date,"1",g_pub_tx.tx_amt1,0.00,"0",0 );	
	if( ret )
	{
            sprintf(acErrMsg,"登记贷款登记薄失败");
            WRITEMSG
            strcpy(g_pub_tx.reply,"L082");
            goto ErrExit;
        }						
	sprintf(acErrMsg,"登记贷款登记薄 PASS!");
	WRITEMSG
	
vtcp_log("Jarod1: ct_flag=[%s] [%s][%d]",ct_flag,__FILE__,__LINE__);	
	set_zd_data("0210",g_ln_parm.cur_no);/* 对币种赋值 */
vtcp_log("Jarod2: ct_flag=[%s] [%s] [%d]",ct_flag,__FILE__,__LINE__);	
	if( g_ln_mst.save_ac_id!=0)
	{
		if( ct_flag[0]!='2')
		{
            		sprintf(acErrMsg,"贷款开户时已指定放款账号,只能做转账放款!");
            		WRITEMSG
            		strcpy(g_pub_tx.reply,"L174");
            		goto ErrExit;				
		}
	}

	if( g_ln_parm.ln_type[0]!='9' )
	{
		/* 根据存款账户ID取存款账户 */
		ret = Ln_auth_Sel( g_pub_tx.reply , &ln_auth_tmp , "pact_no='%s'" , g_ln_mst.pact_no );
		if(ret)
		{
			sprintf(acErrMsg,"查询贷款审批登记簿异常出错![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L181");
			goto ErrExit;
		}			
		ret = Mdm_ac_rel_Sel( g_pub_tx.reply , &mdm_ac_rel_tmp , "ac_id=%ld and mdm_code='%s' and note_sts in( '0','1' )" , g_ln_mst.save_ac_id , ln_auth_tmp.save_mdm_code );
		if(ret==100)
		{
			sprintf(acErrMsg,"存款账户状态不正常,请与中心机房联系!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"L183");
			goto ErrExit;
		}else if(ret){						
			sprintf(acErrMsg,"查询介质账号对照表异常出错![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L182");
			goto ErrExit;	
		}		
		strcpy( save_ac_no , mdm_ac_rel_tmp.ac_no );	
		set_zd_data( "1061" , save_ac_no );
	}
	
	/* 打印程序 */
	TRACE
	/* 打印--贷款放款借方传票 */
	ret = print_LNFFJF( );
	if(ret)		
	{
		sprintf(acErrMsg,"print_LNFFJF,失败请检查!" );
		WRITEMSG		
		strcpy(g_pub_tx.reply,"L207");
		goto ErrExit;
	}
	TRACE
	
	/* 打印--存款方凭证(类似进账单格式) */
	ret = print_LNFFCK( );
	if(ret)		
	{
		sprintf(acErrMsg,"print_LNFF,失败请检查!" );
		WRITEMSG		
		strcpy(g_pub_tx.reply,"L207");
		goto ErrExit;
	}
	TRACE		
        if( g_ln_parm.ln_type[0]!='A' && memcmp(g_ln_parm.prdt_no,"3Z",2)!=0 )
        {
		ret = sql_execute2("update draw_loan_limit set sts='2' where pact_no='%s' and sts='1'" ,g_ln_mst.pact_no );
		if(ret)
		{
			sprintf(acErrMsg,"更新数据库失败!" );
			WRITEMSG		
			strcpy(g_pub_tx.reply,"D105");
			goto ErrExit;
		}
        }
	
		pub_mob_sendmail("贷款放款",g_pub_tx.tx_code,g_mdm_ac_rel.ac_no,g_pub_tx.add_ind,g_pub_tx.tx_amt1,0.0);

	
	if( ct_flag[0]=='1' )	   /*现金*/
		goto ErrExit1;
	else if( ct_flag[0]=='2' ) /*转账*/
		goto ErrExit2;
	else if (ct_flag[0]=='3')  /*提出*/
		goto ErrExit3;

ErrExit:
    sprintf(acErrMsg,"贷款放款失败！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
ErrExit1:
	strcpy(g_pub_tx.reply,"0001");
	sprintf(acErrMsg,"判断现转标志成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit2:
	strcpy(g_pub_tx.reply,"0002");
	sprintf(acErrMsg,"判断现转标志成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit3:
    strcpy(g_pub_tx.reply,"0003");
    sprintf(acErrMsg,"判断现转标志成功![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;	
    
}
    
int data_init_struct()
{	
	return 0;	
}

int data_set_struct()
{    
	int ret=0;
	sprintf(acErrMsg,"进入赋值函数");
	WRITEMSG
			
	get_zd_data("0370",g_pub_tx.ac_no);		/* 贷款帐号 */
	vtcp_log("[%s]aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa[%s][%d]",g_pub_tx.ac_no,__FILE__,__LINE__);

	sprintf(acErrMsg,"贷款帐号[%s]",g_pub_tx.ac_no);
	WRITEMSG
	
	get_zd_int( "0340" , &g_pub_tx.ac_seqn);	/* 贷款账户序号 */
	sprintf(acErrMsg,"贷款账户序号[%d]",g_pub_tx.ac_seqn);
	WRITEMSG	
	
	get_zd_data("0890",g_pub_tx.note_type);		/* 凭证种类 */	
			vtcp_log("[%s]aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa[%s][%d]",g_pub_tx.note_type,__FILE__,__LINE__);

	get_zd_data("0580",g_pub_tx.beg_note_no);	/* 凭证号 */
	vtcp_log("[%s]aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa[%s][%d]",g_pub_tx.beg_note_no,__FILE__,__LINE__);
	
	get_zd_data("0580",g_pub_tx.end_note_no);	/* 凭证号 */
	vtcp_log("[%s]aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa[%s][%d]",g_pub_tx.end_note_no,__FILE__,__LINE__);
	
	get_zd_double("0430",&g_pub_tx.tx_amt1);	/* 发生额 */
	vtcp_log("[%lf]aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa[%s][%d]",g_pub_tx.tx_amt1,__FILE__,__LINE__);

	vtcp_log("[%lf]aaaaaaavbtaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa[%s][%d]",g_pub_tx.tx_amt1,__FILE__,__LINE__);


	get_zd_data("0660",g_pub_tx.ct_ind);							/* 现转标志 */
	get_zd_data("0660",ct_flag);							/* 现转标志 */	
vtcp_log("Jarod: ct_flag=[%s] [%s] [%d]",ct_flag,__FILE__,__LINE__);	
	sprintf(acErrMsg,"现转标志![%s]",g_pub_tx.ct_ind);
	WRITEMSG
vtcp_log("Jarod: ct_flag=[%s] [%s] [%d]",ct_flag,__FILE__,__LINE__);	
	get_zd_data("1011" , de_ac_no );
vtcp_log("Jarod: ct_flag=[%s] [%s] [%d]",ct_flag,__FILE__,__LINE__);	
	get_zd_data("1018" , de_ac_name );	
vtcp_log("Jarod: ct_flag=[%s] [%s] [%d]",ct_flag,__FILE__,__LINE__);	
	sprintf(acErrMsg,"存款账号![%s]户名",de_ac_no,de_ac_name);
	WRITEMSG
		
vtcp_log("Jarod: ct_flag=[%s] [%s] [%d]",ct_flag,__FILE__,__LINE__);	
	ret = pub_ln_lndis_file(g_pub_tx.ac_no,g_pub_tx.ac_seqn,
		&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm ); /*读取贷款户信息*/
	if( ret ) {return ret;}
	vtcp_log("[%lf]aaaaaaavbtaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa[%s][%d]",g_ln_mst.bal,__FILE__,__LINE__);
vtcp_log("Jarod: ct_flag=[%s] [%s] [%d]",ct_flag,__FILE__,__LINE__);	
					
	return 0;
}

/* 打印--贷款放款借方传票(打印在借据第四联背面 )  */
int print_LNFFJF( )
{
	char tx_type[9];
    	char filename[51],tmpname[101];
    	FILE *fp;
	
	char rate[11];
	char over_rate[11];
	char fine_rate[11];
	char save_ac_no[21];
	char pay_ac_no[21];	
	
	long end_date=0;
	char exp_ind[5];
	char ln_acc_hrt[8];
	char data_code[5];
	int ret_tmp=0;
	
	
	memset( &exp_ind ,0 , strlen(exp_ind) );
	
	
    	
	pub_base_AllDwnFilName( tmpname );
	fp=fopen(tmpname,"a");
        if( fp == NULL )
        {
		sprintf(acErrMsg,"文件名:[%s]!",tmpname);
		WRITEMSG		
		sprintf(acErrMsg,"LNFFJF打开文件失败!");
		WRITEMSG
		strcpy( g_pub_tx.reply,"L206" );
		return 1;
        }
        	
	/* 赋值部分 */
	strcpy( tx_type , "贷款放款" );		
	/* 取科目号 */
	strcpy( data_code , "0152" );	/* 本金科目 */
	ret_tmp = pub_ln_get_acc_hrt( g_ln_mst , g_ln_parm , data_code , ln_acc_hrt );
	if( ret_tmp )
	{
		sprintf(acErrMsg,"取贷款科目号失败!");
		WRITEMSG		
		return 1;
	}
	
	if( g_ln_mst.exp_mtr_date && g_ln_mst.exp_ind[0]!='0' )
	{
		end_date = g_ln_mst.exp_mtr_date;
		strcpy( exp_ind , "(展)" );
		sprintf( rate , "%8.5lf‰" , g_ln_mst.exp_rate );		
	}else{
		end_date = g_ln_mst.mtr_date;
		strcpy( exp_ind , "" );
		sprintf( rate , "%8.5lf‰" , g_ln_mst.rate );
	}

	sprintf( over_rate , "%8.5lf‰" , g_ln_mst.over_rate);
	sprintf( fine_rate , "%8.5lf‰" , g_ln_mst.fine_rate);
	
	pub_base_acid_acno( g_ln_mst.save_ac_id,g_pub_tx.ac_seqn,save_ac_no );
	pub_base_acid_acno( g_ln_mst.repay_ac_id,g_pub_tx.ac_seqn,pay_ac_no );
				
	pub_base_strpack( g_pub_tx.ac_no );
	pub_base_strpack( g_ln_mst.name );
	pub_base_strpack( g_ln_mst.pact_no );
	pub_base_strpack( g_ln_parm.title );
	pub_base_strpack( tx_type );
	pub_base_strpack( rate );
	pub_base_strpack( over_rate );
	pub_base_strpack( fine_rate );
	pub_base_strpack( de_ac_no );
	pub_base_strpack( pay_ac_no );
	pub_base_strpack( ln_acc_hrt );	
					
	/*  1.交易类型、  2.贷款账号、  3.借据号、   4.贷款户名、  5.产品名称 	*/
	/*  6.交易金额、  7.开户日期、  8.到期日期、 9.贷款余额、 10.累计放款     */
	/* 11.展期标志、 12.正常利率、 13.逾期利率、14.罚息利率   15.放款账号   */
	/* 16.还款账号   17.科目号、   18.流水号*/
	fprintf(fp,"LNFFJF%s|%s|%s|%s|%s|%.2lf|%ld|%ld|%.2lf|%.2lf|%.s|%s|%s|%s|%s|%s|%s|%ld|\n",
	tx_type , g_pub_tx.ac_no , g_ln_mst.pact_no , g_ln_mst.name ,  g_ln_parm.title ,
	g_pub_tx.tx_amt1 , g_ln_mst.opn_date , end_date , g_ln_mst.bal , g_ln_mst.ttl_prvd_amt ,
	exp_ind , rate , over_rate , fine_rate , de_ac_no , pay_ac_no ,ln_acc_hrt ,g_pub_tx.trace_no );	
	
	fclose(fp);		
	set_zd_data(DC_FILE_SND,"1");	
	return 0;
}	

/* 打印--存款方凭证(类似转账进账单) */
int  print_LNFFCK()
{ 
    	char tx_type[9];
	
    	char filename[51],tmpname[101];
    	FILE *fp;
    
    	pub_base_AllDwnFilName( tmpname );
	fp=fopen(tmpname,"a");
        if( fp == NULL )
        {
		sprintf(acErrMsg,"文件名:[%s]!",tmpname);
		WRITEMSG		
		sprintf(acErrMsg,"LNFFCK打开文件失败!");
		WRITEMSG
		strcpy( g_pub_tx.reply,"L206" );
		return 1;
        }
	
	strcpy( tx_type , "贷款放款" );	
	
	pub_base_strpack( de_ac_no );
	pub_base_strpack( de_ac_name );
	pub_base_strpack( tx_type );
			
	/* 1.贷款账号、	     2.户名、	      3.类型、    4.放款金额 	*/
	fprintf(fp,"LNFFCK%s|%s|%s|%.2lf|\n",de_ac_no , de_ac_name , tx_type , g_pub_tx.tx_amt1 );	
	
	fclose(fp);
	set_zd_data(DC_FILE_SND,"1");
		
	return 0;
}
