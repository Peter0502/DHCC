/*************************************************
* 文 件 名: spL102.c
* 功能描述：   
*              放款计划
*
* 作    者:    bob
* 完成日期：   2003年01月10日
* 修改记录：   
*     1. 日    期:2003-03-25
*        修 改 人:lance
*        修改内容:
*     2. 
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#define EXTERN
#include "public.h"
#include "ln_pay_pln_c.h"

static char dd_ac_no[20];
static int  dd_ac_seqn;

spL102()  
{ 	
	
	int ret=0,flag=0;
	char filename[100];
	char fldstr[6][100];
	double amt=0.00,atm_tmp=0.00;
	long date=0;
	char tmpstr[1024];
	FILE *fp;
	int i=0;
	
    struct ln_pay_pln_c	ln_pay_pln;
    struct ln_pay_pln_c	ln_pay_pln_v;	

	memset (&ln_pay_pln,0x00,sizeof(struct ln_pay_pln_c) );	
	memset (&ln_pay_pln_v,0x00,sizeof(struct ln_pay_pln_c) );	
		
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
	
	sprintf(acErrMsg,"读取贷款户信息[%s][%d]",g_pub_tx.ac_no,g_pub_tx.ac_seqn);
	WRITEMSG		
	/* 调用根据贷款显示账号和账号序号取贷款主文件函数*/
	ret = pub_ln_lndis_file(g_pub_tx.ac_no,g_pub_tx.ac_seqn,
						&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm ); /*读取贷款户信息*/
	if( ret )
	{
		sprintf(acErrMsg,"读取贷款户信息error");
		WRITEMSG
		goto ErrExit;
	}
	sprintf(acErrMsg,"ac_id[%d] [%d]",g_ln_mst.ac_id,g_ln_mst.ac_seqn );
	WRITEMSG
	
	/* 判断贷款是否处于正常状态 */
	if ( g_ln_mst.ac_sts[0]!='1' )
	{
		sprintf(acErrMsg,"贷款处于非正常状态,不可做此交易!",
										g_ln_parm.pay_pln_type);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L079");
		goto ErrExit;
	}
		
	/* 判断放款计划类型 N无 M手动 */
	if ( g_ln_parm.pay_pln_type[0]!='M' )
	{
		sprintf(acErrMsg,"放款计划类型非手动[%s],不可做此交易!",
										g_ln_parm.pay_pln_type);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L075");
		goto ErrExit;
	}	
	/* 清除记录 */
	ret = Ln_pay_pln_Dec_Sel( g_pub_tx.reply,"ac_id=%ld and ac_seqn=%d and \
								pay_ind='2'",g_ln_mst.ac_id,g_ln_mst.ac_seqn );
	if( ret==0 )
	{
		ret=sql_execute("delete from ln_pay_pln where ac_id=%ld and \
				ac_seqn=%d and pay_ind='2'",g_ln_mst.ac_id,g_ln_mst.ac_seqn);
		if(ret)		goto ErrExit;
	}else if( ret&&ret!=100 )
	{
		goto ErrExit;
	}
	/* 取出存款户的账户ID */
	ret = pub_base_currentdis_file(dd_ac_no,dd_ac_seqn,&g_dd_mst);
	if( ret )
	{	
		sprintf(acErrMsg,"取活期主文件函数错误");
		WRITEMSG
		goto ErrExit;
	}	
	Gl_file_first=0;	
	pub_base_AllDwnFilName( filename );
	fp = fopen( filename,"r" );
	if( fp==NULL )
	{
		sprintf(acErrMsg," open file [%s] error ",filename );
		strcpy( g_pub_tx.reply,"S047" );
	}
	while(1)
	{
		fgets( tmpstr,1024,fp );
		if( feof(fp) ) break;

		for( i=0;i<6;i++ )
		{
			ret=pub_base_cut_str( tmpstr,fldstr[i],"|",i+1 );
				vtcp_log(" [%d] [%s] ",i+1,fldstr[i]);
		}
		atm_tmp+=atof(fldstr[4]);
		amt=atof(fldstr[4]);
		date=atol(fldstr[3]);
		vtcp_log("%.2lf,%.2lf,%s",amt,atof(fldstr[4]),fldstr[4]);
		
		/* 检查金额是否超限 */
		ret=pub_base_CompDblVal(g_ln_mst.amt_lmt,g_ln_mst.ttl_prvd_amt+atm_tmp);
		if(ret<0)
		{
			sprintf(acErrMsg,"计划放款金额过大,超出贷款放款限额![%lf][%lf]",
								g_ln_mst.amt_lmt,g_ln_mst.ttl_prvd_amt+atm_tmp);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L076");
			goto ErrExit;
		}
		ln_pay_pln.ac_id=g_ln_mst.ac_id;
		ln_pay_pln.ac_seqn=g_ln_mst.ac_seqn;
		ln_pay_pln.de_ac_id=g_dd_mst.ac_id;
		ln_pay_pln.de_ac_seqn=dd_ac_seqn;
		ln_pay_pln.ttl_amt=amt;
		ln_pay_pln.wrk_date=g_pub_tx.tx_date;
		ln_pay_pln.beg_date=date;
		strcpy(ln_pay_pln.pay_ind,"2");
																	
		/* 添加还款计划表 */	
		ret = Ln_pay_pln_Ins( ln_pay_pln , g_pub_tx.reply );
		if(ret)
		{
			sprintf(acErrMsg,"登记放款计划表失败");
			WRITEMSG
			strcpy(g_pub_tx.reply,"L077");
			goto ErrExit;
		}
		/* Pub_tx结构赋值 */
		strcpy ( g_pub_tx.ac_wrk_ind,"0" );
		g_pub_tx.tx_amt1 = 0.00 ;
		strcpy ( g_pub_tx.ac_id_type , "3" );	/* 账号类型 3-贷款 */
		strcpy ( g_pub_tx.add_ind , "1" );		/* 增减1加 */	
		strcpy ( g_pub_tx.ct_ind , "2" );		/* 现转:1现2转 */	
		strcpy ( g_pub_tx.prt_ind , "0" );	/* 是否打印存折(有折无折标志)0否 */
		strcpy ( g_pub_tx.hst_ind , "1" );  /* 入明细帐标志:1日间入 2日终单笔入 															3日终汇总入 */
		g_pub_tx.svc_ind = 3060 ;
		strcpy( g_pub_tx.brf,"贷款放款计划" );
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

	}
	vtcp_log("amt [%.2lf]",amt);	
	fclose(fp);
	
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"放款计划成功！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"放款计划失败！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
    
int data_init_struct()
{
	memset (&g_ln_mst, 0x00, sizeof(struct ln_mst_c));
	memset (&g_dd_mst, 0x00, sizeof(struct dd_mst_c));
	memset (&g_mdm_ac_rel, 0x00, sizeof(struct mdm_ac_rel_c));
	memset (&g_ln_mst, 0x00, sizeof(struct ln_mst_c));
	
	return 0;
	
}

int data_set_struct()
{    
	sprintf(acErrMsg,"进入赋值函数");
	WRITEMSG
	
	g_ln_mst.opn_date = g_pub_tx.tx_date;	/* 开户日期 */
	get_zd_data("0370",g_pub_tx.ac_no);		/* 贷款账号 */
	get_zd_int("0340",&g_pub_tx.ac_seqn);	/* 贷款账号序号 */
	get_zd_data("0310",dd_ac_no);			/* 存款账号 */
	get_zd_int("0350",&dd_ac_seqn);			/* 存款账号序号 */					
	return 0;
}

