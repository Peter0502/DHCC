/*************************************************
* 文 件 名: spL3A1.c
* 功能描述：   
*              贷款剥离--转出
*
* 作    者:    lance
* 完成日期：   2003年11月24日
* 修改记录：   
*     1. 日    期:
*        修 改 人:
*        修改内容:
*     2. 
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#define EXTERN
#include "public.h"

	char brno[6];
	char old_brno[6];
spL3A1()  
{ 		
	int ret=0;
	char wherelist[200];
	char comm[100];
	double lo_amt1=0.00;
	double lo_amt2=0.00;
						
	/** 初始化全局变量 **/
	pub_base_sysinit();
	sprintf(acErrMsg,"初始化全局变量 PASS!");
	WRITEMSG	
	
	/** 数据初始化 **/
	if ( data_init_struct() )	ERR_DEAL;
	sprintf(acErrMsg,"数据初始化 PASS!");
	WRITEMSG
	
	/** 取值、赋值 **/
	if ( data_set_struct() )    ERR_DEAL;
	sprintf(acErrMsg,"取值、赋值 PASS!");
	WRITEMSG			

	/* 检查转入转出方是否是同一网点 */
	if( strcmp( old_brno , brno )==0 )
	{
		sprintf(acErrMsg,"转入机构与贷款现所属机构为同一机构,不可剥离!");
		WRITEMSG
		strcpy ( g_pub_tx.reply , "L221" );		
		goto ErrExit;
	}
	set_zd_double("1084",0.00 );
	set_zd_double("1085",0.00 );
	set_zd_double("1086",0.00 );
	set_zd_double("1087",0.00 );
		
	/* 条件定义 */
	sprintf( wherelist," opn_br_no='%s' ",old_brno );
	if( strlen(g_pub_tx.ac_no) )
	{
		ret=Mdm_ac_rel_Sel( g_pub_tx.reply, &g_mdm_ac_rel , "ac_no='%s' and ac_seqn=%d ", g_pub_tx.ac_no,g_pub_tx.ac_seqn);
		if(ret)	goto ErrExit;
		
		sprintf( comm ," and ac_id=%ld and ac_seqn=%d " , g_mdm_ac_rel.ac_id , g_mdm_ac_rel.ac_seqn );
		strcat( wherelist , comm );
	}
	
	ret= Eod_ln_mst_Dec_Upd( g_pub_tx.reply , wherelist );	
	if (ret)
	{
		sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR [%d]",ret);
		WRITEMSG
		goto ErrExit;
	}
	while(1)
	{
		ret = Eod_ln_mst_Fet_Upd( &g_ln_mst, g_pub_tx.reply );
		if( ret==100 )
		{
			sprintf( acErrMsg,"无记录[%d]",ret);
			WRITEMSG
			break;
		}else if(ret){
			sprintf(acErrMsg,"FETCH FOR UPDATE ERROR [%d]",ret);
			WRITEMSG
			goto ErrExit;	
		}
		ret=Mdm_ac_rel_Sel( g_pub_tx.reply, &g_mdm_ac_rel , "ac_id=%ld and ac_seqn=%d ", g_ln_mst.ac_id,g_ln_mst.ac_seqn);
		if(ret)	goto ErrExit;
									
		/* Pub_tx结构赋值 */
		g_pub_tx.tx_amt1 = 0.00;		
		g_pub_tx.tx_amt2 = 0.00;
		g_pub_tx.tx_amt3 = 0.00;		
		g_pub_tx.tx_amt4 = 0.00;
		g_pub_tx.ac_id = g_ln_mst.ac_id;
		g_pub_tx.ac_seqn = g_ln_mst.ac_seqn;
		strcpy( g_pub_tx.ac_no, g_mdm_ac_rel.ac_no);
		g_pub_tx.cif_no = g_ln_mst.cif_no;			/* 客户号 */
		strcpy ( g_pub_tx.ac_id_type , "3" );	/* 账号类型 3-贷款 */
		strcpy ( g_pub_tx.intst_type , g_ln_mst.intst_type );	/* 计息类型 */	
		strcpy ( g_pub_tx.add_ind , "1" );		/* 增减:0减1加2销户 */	
		strcpy ( g_pub_tx.ct_ind , "2" );		/* 现转:1现2转 */	
		/* 是否打印存折(有折无折标志):1是0否 */
		strcpy ( g_pub_tx.prt_ind , "1" );	
		/* 入明细帐标志:1日间入2日终单笔入3日终汇总入 */
		strcpy ( g_pub_tx.hst_ind , "1" );
		g_pub_tx.svc_ind = 3059;			/* 贷款结转 */
		strcpy( g_pub_tx.ac_wrk_ind , "0110000" );
		strcpy ( g_pub_tx.brf,"贷款剥离(移出)" );
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


	   	/* 进行会计记帐 */     
		set_zd_double("1084",-g_ln_mst.bal);
		set_zd_double("1085",-g_ln_mst.in_lo_intst);
		set_zd_double("1086",-g_ln_mst.out_lo_intst);
		set_zd_double("1087",-g_ln_mst.cmpd_lo_intst);
		/****del by lance 2003-11-29
		if( g_ln_parm.ln_pay_type[0]=='3'||g_ln_parm.ln_pay_type[0]=='4' )
		{
			* 统计按揭贷款欠款表数据 *
			ret= sql_sum_double ( "ln_lo" , "lo_intst" , &lo_amt1 , "ac_id=%ld and ac_seqn=%d and pay_type='0' and old_ac_ind='1' ", g_ln_mst.ac_id,g_ln_mst.ac_seqn );
			if(ret)	goto ErrExit;
			ret= sql_sum_double ( "ln_lo" , "pay_lo_intst" , &lo_amt2 , "ac_id=%ld and ac_seqn=%d and pay_type='0' and old_ac_ind='1' ", g_ln_mst.ac_id,g_ln_mst.ac_seqn );
			if(ret)	goto ErrExit;	
								
			set_zd_double("1085",lo_amt1-lo_amt2 );
			set_zd_double("1086",0.00 );
			set_zd_double("1087",0.00 );
				
		}
		if( g_ln_mst.prdt_no[0]=='3' && g_ln_mst.prdt_no[1]=='Z' )
		{
			set_zd_double("1085",0.00 );
			set_zd_double("1086",0.00 );
			set_zd_double("1087",0.00 );	
		}	
		***********/				
		set_zd_data("0210","01");
		set_zd_data("0660","1" );	/*借方红字*/
		strcpy(g_pub_tx.sub_tx_code,"L3A1");
		strcpy(g_pub_tx.prdt_code,g_ln_mst.prdt_no);
		ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
		if (ret != 0)
		{
			sprintf(acErrMsg,"会计记帐不成功!!");
			WRITEMSG
			goto ErrExit;
		}
		set_zd_double("1084",0.00 );
		set_zd_double("1085",0.00 );
		set_zd_double("1086",0.00 );
		set_zd_double("1087",0.00 );   	

		ret = Eod_ln_mst_Upd_Upd( g_ln_mst, g_pub_tx.reply  );
		if ( ret )
		{
			sprintf( acErrMsg,"Ln_mst_Upd_Upd 出错[%d]",ret);
			WRITEMSG
			goto ErrExit;
		}		
	}
	Eod_ln_mst_Clo_Upd( );
		
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"贷款剥离转出成功！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"贷款剥离转出失败！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
    
int data_init_struct()
{
	memset (&g_ln_mst, 0x00, sizeof(struct ln_mst_c));
	memset (&g_mdm_ac_rel, 0x00, sizeof(struct mdm_ac_rel_c));
	
	return 0;
	
}

int data_set_struct()
{    
	sprintf(acErrMsg,"进入赋值函数");
	WRITEMSG
	
	
	get_zd_data("0370",g_pub_tx.ac_no);		/* 贷款账号 */	
	get_zd_data("0910",brno);			/* 接收网点 */
		
	g_pub_tx.ac_seqn=0;				/* 贷款账号序号 */
	strcpy( old_brno, g_pub_tx.tx_br_no );		/* 转出网点 */
	sprintf(acErrMsg,"进入赋值函数贷款账号[%s]转出网点[%s] 转入网点[%s]",g_pub_tx.ac_no,old_brno,brno);
	WRITEMSG						
	return 0;
}

