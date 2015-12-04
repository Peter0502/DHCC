/*************************************************
* 文 件 名: spA688.c
* 功能描述：   
*              对公活期产品转入 
*
* 作    者:    LiuHuafeng
* 完成日期：   2008-7-13 8:42:40
* 修改记录：   
*     1. 日    期:
*        修 改 人:
*        修改内容:
*     2. 
TX_CODE	TX_NAME	BIT_MAP	LOG_IND	TX_TYPE	WRK_STS	YW_CNT_IND	YW_PERCENT
3319	存款产品转换	10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000	1	4	2	3	
TX_CODE	FLOW_CODE	SUB_TX_CODE	SUB_TX_RSLT
3319	0	A689	#=0000@1@#$
3319	1	A688	#$
SUB_TX_CODE	SUB_TX_NAME	TX_FUNC	ACCT_IND	CHNL_IND
A668	存款产品转入	spA668	1	0
A669	存款差品转出	spA669	1	0
SUB_TX_CODE	DC_CODE	ENTRY_CODE
A688	0000	A68800
A689	0000	A68900

ENTRY_CODE	SEQN	CUR_NO	ACC_HRT	DC_IND	USE_BR_IND	CT_TYPE	CT_IND	AMT_NO	BRF
A68800	1	0210	@0152	2	1	1	2	1074	
A68900	1	0210	@0152	2	1	1	2	1074	
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "stdio.h" 
#define EXTERN
#include "public.h"
#include "cif_basic_inf_c.h"
#include "cif_prdt_rel_c.h"

static	long	 end_date_new;
static	char	 prod_no[4];
		
spA688()  
{ 	
	
	int ret=0;
	int tag=0;
	int amt_flag=0;
	
	char ln_ac_id[20];
	char acc_hrt[6];
	char wrk_sts[2];
	char type[2];
	long ac_id_t;
	memset(ln_ac_id,0x00,sizeof(ln_ac_id));
	memset(acc_hrt,0x00,sizeof(acc_hrt));
	memset(wrk_sts,0x00,sizeof(wrk_sts));
	memset(type,0x00,sizeof(type));

	struct	dd_parm_c	dd_parm_new;	
	struct	cif_basic_inf_c	cif_basic_inf;	
	struct	cif_prdt_rel_c	cif_prdt_rel;	
			
	memset ( &dd_parm_new , 0x00 , sizeof( struct dd_parm_c ) );
	memset ( &cif_basic_inf , 0x00 , sizeof( struct cif_basic_inf_c ) );
	memset ( &cif_prdt_rel , 0x00 , sizeof( struct cif_prdt_rel_c ) );
				
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
	
	/* 取老产品信息 */
	ret = pub_dd_dddis_file(g_pub_tx.ac_no,g_pub_tx.ac_seqn,
			&g_mdm_ac_rel,&g_dd_mst,&g_dd_parm ); /*读取存款户信息*/
	if( ret )
	{
		sprintf(acErrMsg,"读取存款户信息error");
		WRITEMSG
		goto ErrExit;
	}

	sprintf(acErrMsg,"ac_id[%ld] [%d]",g_dd_mst.ac_id,g_dd_mst.ac_seqn );
	WRITEMSG

	/* 取新产品信息 */
	ret = Dd_parm_Sel( g_pub_tx.reply , &dd_parm_new, "prdt_no='%s'", prod_no);		
	if( ret==100 )
	{
		sprintf( acErrMsg,"存款产品参数表无此记录 erro code=[%s]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L001" );
		return 1;
	}else if( ret ){
    		sprintf( acErrMsg,"取存款产品参数表异常 erro code=[%s]",ret);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"D103" );
	    		return -1;
    	}
	sprintf(acErrMsg,"取存款产品参数信息 PASS!");
	WRITEMSG		
	sprintf(acErrMsg,"此两种存款产品[%s][%s]",g_dd_parm.prdt_no , dd_parm_new.prdt_no);
	WRITEMSG
			
	if( g_dd_mst.ac_sts[0]!='1'  )
	{
		sprintf(acErrMsg,"帐户状态不正常,不可做此交易![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L137");
		goto ErrExit;
	}
	TRACE
	/* 客户类型必须相同 */
	/* 根据客户号查询客户名称 */			  
	ret = Cif_basic_inf_Sel( g_pub_tx.reply , &cif_basic_inf , 
					" cif_no=%ld " , g_dd_mst.cif_no );
	if( ret==100 )
	{
		sprintf(acErrMsg,"该证件对应的客户号不存在[%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C020");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"取客户基本信息异常![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C006");
		goto ErrExit;
	}				
	sprintf(acErrMsg,"此两种存款产品[%s][%s]",g_dd_parm.prdt_no , dd_parm_new.prdt_no);
	WRITEMSG
	TRACE
	sprintf(acErrMsg,"此两种存款产品[%s][%s]",g_dd_parm.prdt_no , dd_parm_new.prdt_no);
	WRITEMSG
				
	set_zd_data("0210","01");	/* 币种 */
	sprintf(acErrMsg,"此两种存款产品[%s][%s]",g_dd_parm.prdt_no , dd_parm_new.prdt_no);
	WRITEMSG

	WRITEMSG
	/* 期限变化 */
	sprintf(acErrMsg,"此两种存款产品[%s][%s]",g_dd_parm.prdt_no , dd_parm_new.prdt_no);
	WRITEMSG	
	get_zd_double("1074",&g_dd_mst.bal);
	g_dd_mst.bal=g_dd_mst.bal * -1;
	set_zd_double("1074",g_dd_mst.bal);		
	
	/* 产品号变化 */
	if( strcmp( g_dd_parm.prdt_no, dd_parm_new.prdt_no) )
	{
		set_zd_double("1074",g_dd_mst.bal);
		tag++;
	}
	if( tag==0 )
	{
		sprintf(acErrMsg,"此两种存款产品不可以转换,请检查![%s][%s]",g_dd_parm.prdt_no , dd_parm_new.prdt_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L140");
		goto ErrExit;
	}
	/* 客户类型是否合乎产品定义 */
	/* 20120613 wudw 按财务部要求对机构不做限制 */
	/*ret = pub_base_check_cltprdt( cif_basic_inf.type, prod_no );
	if( ret )
	{
		sprintf(acErrMsg,"新旧产品客户类型不相同,不可做此交易![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L139");
		ERR_DEAL;	
	}		*/
	TRACE

	strcpy( type , "1" );
	strcpy(g_dd_mst.prdt_no,prod_no);	/* 产品编号 */
	
			
	/* 更新存款主文件 */
	ret = Dd_mst_Dec_Upd( g_pub_tx.reply , "ac_id=%ld and ac_seqn=%d" ,g_dd_mst.ac_id , g_dd_mst.ac_seqn );
	if ( ret )
	{
		sprintf( acErrMsg, "执行Dd_mst_Dec_Upd错!ret=[%d]",ret );
		WRITEMSG
		ERR_DEAL
	}
    
	ret = Dd_mst_Fet_Upd( &g_dd_mst,g_pub_tx.reply );   
	if ( ret == 100 )
	{
		sprintf( acErrMsg, "存款主文件中无此记录" );
		WRITEMSG
		ERR_DEAL
	}else if ( ret ){
		sprintf( acErrMsg, "执行Dd_mst_Fet_Upd出错!ret=[%d]",ret );
		WRITEMSG
		ERR_DEAL
	}

	strcpy( g_dd_mst.prdt_no , prod_no );   		/* 产品代码 */
	
	ret = Dd_mst_Upd_Upd( g_dd_mst, g_pub_tx.reply );
	if ( ret == -239 )
	{
		sprintf( acErrMsg, "修改存款主文件错,存在重复记录!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"L031");
		ERR_DEAL
	}else if( ret ){
		sprintf( acErrMsg,"执行Dd_mst_Upd_Upd错!ret=[%d]",ret );
		WRITEMSG
		ERR_DEAL
	}
	Dd_mst_Clo_Upd();
	sprintf(acErrMsg,"更新存款主文件 PASS!");
	WRITEMSG
	
	/*  修改客户-产品关系表 */
	ret = Cif_prdt_rel_Dec_Upd( g_pub_tx.reply , "ac_id=%ld and cif_no=%ld" ,g_dd_mst.ac_id , g_dd_mst.cif_no );
	if ( ret )
	{
		sprintf( acErrMsg, "执行Cif_prdt_rel_Dec_Upd错!ret=[%d]",ret );
		WRITEMSG
		ERR_DEAL
	}
    
	ret = Cif_prdt_rel_Fet_Upd( &cif_prdt_rel,g_pub_tx.reply );   
	if ( ret == 100 )
	{
		sprintf( acErrMsg, "客户-产品关系表中无此记录" );
		WRITEMSG
		Cif_prdt_rel_Clo_Upd();
	}else if ( ret ){
		sprintf( acErrMsg, "执行Cif_prdt_rel_Fet_Upd出错!ret=[%d]",ret );
		WRITEMSG
		ERR_DEAL
	}else if( !ret ){

		strcpy( cif_prdt_rel.prdt_code , prod_no );   		/* 产品代码 */
			
		ret = Cif_prdt_rel_Upd_Upd( cif_prdt_rel, g_pub_tx.reply );
		if ( ret == -239 )
		{
			sprintf( acErrMsg, "修改客户-产品关系表错,存在重复记录!" );
			WRITEMSG
			strcpy(g_pub_tx.reply,"L149");
			ERR_DEAL
		}else if( ret ){
			sprintf( acErrMsg,"执行Cif_prdt_rel_Upd_Upd错!ret=[%d]",ret );
			WRITEMSG
			ERR_DEAL
		}
		Cif_prdt_rel_Clo_Upd();
		sprintf(acErrMsg,"更新客户-产品关系表 PASS!");
		WRITEMSG		
	}
	
	/* Pub_tx结构赋值 */
	get_zd_double("1074",&g_dd_mst.bal);
	g_pub_tx.tx_amt1 = g_dd_mst.bal;
	g_pub_tx.ac_id = g_dd_mst.ac_id;		/* 账户ID */
	g_pub_tx.ac_seqn = g_dd_mst.ac_seqn;	/* 账户序号 */
	strcpy ( g_pub_tx.ac_id_type , "1" );	/* 账号类型 1-对公活期 3-存款 */
	strcpy ( g_pub_tx.intst_type , g_dd_mst.intst_type );	/* 计息类型 */	
	strcpy ( g_pub_tx.add_ind , "1" );		/* 增减:0减1加2销户 */	
	strcpy ( g_pub_tx.ct_ind , "2" );		/* 现转:1现2转 */	
	/* 是否打印存折(有折无折标志):1是0否 */
	strcpy ( g_pub_tx.prt_ind , "1" );	
	/* 入明细帐标志:1日间入2日终单笔入3日终汇总入 
	strcpy ( g_pub_tx.hst_ind , "1" );*/
	strcpy ( g_pub_tx.hst_ind , "0" );/*不入 根据晋中需求 modify by martin 2009/12/21 10:00:18*/
	g_pub_tx.svc_ind = 1001;			/* 存款结转 */
	strcpy( g_pub_tx.ac_wrk_ind , "0100000" ); 
	strcpy ( g_pub_tx.brf,"活期产品转入(本金)" );
	sprintf(acErrMsg,"Pub_tx结构赋值 PASS!");
	WRITEMSG
	
	/*** 登记交易流水 ***/
	if( pub_acct_trance() )
	{
		sprintf(acErrMsg,"登记交易流水错误!");
		WRITEMSG
		goto ErrExit;
	}
	sprintf(acErrMsg,"登记交易流水 PASS!");
	WRITEMSG
			

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"存款产品转入成功！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"存款产品转入失败！[%s]",g_pub_tx.reply);
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
	return 0;	
}

int data_set_struct()
{    
	int ret=0;
	sprintf(acErrMsg,"进入赋值函数");
	WRITEMSG
	
	get_zd_data("0310" , g_pub_tx.ac_no); 		/* 存款账号 */
	g_pub_tx.ac_seqn = 1;				/* 账号序号 */

	get_zd_data("0240" , prod_no); 			/* 新产品代码 */
	sprintf(acErrMsg,"到期日期=[%ld]",end_date_new);	
	WRITEMSG						
	return 0;
}
