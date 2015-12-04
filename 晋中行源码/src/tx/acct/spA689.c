/*************************************************
* 文 件 名: spL689.c
* 功能描述：   
*              存款产品转出
*
* 作    者:    LiuHuafeng
* 完成日期：   2008-7-14 11:13:21
* 修改记录：   
*     1. 日    期:
*        修 改 人:
*        修改内容:
*     2. 
* 
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "stdio.h" 
#define EXTERN
#include "public.h"

static	char	 prod_no[4];
	double old_ln_amt=0.00;
			
spA689()
{ 	

	int ret=0;
	int tag=0;
	int amt_flag=0;
	
	char ln_ac_id[20];
	char acc_hrt[6];
	char wrk_sts[2];
	char type[2];
	long ac_id_t;
	struct	dd_mst_c	dd_mst_new;
	struct	dd_parm_c	dd_parm_new;	
	memset(ln_ac_id,0x00,sizeof(ln_ac_id));
	memset(acc_hrt,0x00,sizeof(acc_hrt));
	memset(wrk_sts,0x00,sizeof(wrk_sts));
	memset(type,0x00,sizeof(type));
	memset ( &dd_mst_new , 0x00 , sizeof( struct dd_mst_c ) );
	memset ( &dd_parm_new , 0x00 , sizeof( struct dd_parm_c ) );
	
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
	sprintf(acErrMsg,"ac_id[%ld] [%d] prdt_no=%s",g_dd_mst.ac_id,g_dd_mst.ac_seqn,g_dd_mst.prdt_no );
	WRITEMSG	
	
	sprintf(acErrMsg,"ac_id[%ld] [%d]",g_dd_mst.ac_id,g_dd_mst.ac_seqn );
	WRITEMSG	

	old_ln_amt = g_dd_mst.bal ;
	
	/* 取新产品信息 */
	ret = Dd_parm_Sel( g_pub_tx.reply , &dd_parm_new, "prdt_no='%s'", prod_no);		
	if( ret==100 )
	{
		sprintf( acErrMsg,"存款产品参数表无此记录 erro code=[%s]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L001" );
		goto ErrExit;
	}
	else if( ret !=0 )
	{
		sprintf( acErrMsg,"取存款产品参数表异常 erro code=[%s]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"D103" );
		goto ErrExit;
	}
	sprintf(acErrMsg,"取存款产品参数信息 PASS!");
	WRITEMSG		
	sprintf(acErrMsg,"313此两种存款产品[%s][%s]",g_dd_parm.prdt_no , dd_parm_new.prdt_no);
	WRITEMSG	

	if( g_dd_mst.ac_sts[0]!='1'  )
	{
		sprintf(acErrMsg,"存款帐户状态不正常,不可做此交易![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L137");
		goto ErrExit;
	}
	set_zd_data("0210","01");	/* 币种 */
	
	/* 产品号变化 */
	if( strcmp( g_dd_parm.prdt_no, dd_parm_new.prdt_no) )
	{
		set_zd_double("1074",-g_dd_mst.bal);
		tag++;
	}
	if( tag==0 )
	{
		sprintf(acErrMsg,"此两种存款产品不可以转换,请检查![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L140");
		goto ErrExit;
	}
		/**add by zhaoyue 20141119 当天有账务发生的账户不允许做产品转换**/
	  ret = sql_count("trace_log", "ac_id=%ld and ac_seqn=%d and tx_date=%ld and add_ind in('0','1') and amt<>0.00 ", g_dd_mst.ac_id, g_dd_mst.ac_seqn, g_pub_tx.tx_date);
	  if(ret>0){
		sprintf(acErrMsg,"该账户当日有账务发生，不允许做产品转换!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"A041");
		goto ErrExit;
	}
	/**add by zhaoyue 20141119 end**/
	
	
	/* Pub_tx结构赋值 */
	g_pub_tx.tx_amt1 = -g_dd_mst.bal;
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
	
	strcpy ( g_pub_tx.brf,"活期产品转出" );
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
    sprintf(acErrMsg,"存款产品转出成功！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"存款产品转出失败！[%s]",g_pub_tx.reply);
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
	return 0;
}
