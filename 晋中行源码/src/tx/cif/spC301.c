/*************************************************
* 文 件 名: spC301.c
* 功能描述：客户识别ID状态维护
* 作    者: andy
* 完成日期：2004年01月10日
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
#include "trace_log_c.h"
#include "cif_basic_inf_c.h"
#include "cif_inf_chg_log_c.h"

static struct cif_basic_inf_c    cif_basic_inf_t;
static struct cif_basic_inf_c    cif_basic_inf;
static struct cif_inf_chg_log_c  cif_inf_chg_log;
	
char beg_sts[2];
		
spC301()  
{ 	
	int ret=0;
	
	/** 初始化全局变量 **/
	pub_base_sysinit();
	
	/** 数据初始化 **/
	memset (&cif_basic_inf, 0x00, sizeof(struct cif_basic_inf_c));
	memset (&cif_basic_inf_t, 0x00, sizeof(struct cif_basic_inf_c));
	memset (&cif_inf_chg_log, 0x00, sizeof(struct cif_inf_chg_log_c));	
		
	/** 取值、赋值 **/
	get_zd_long("0280",&cif_basic_inf_t.cif_no);	/* 客户号 */
	get_zd_data("0700",cif_basic_inf_t.sts);		/* 客户状态 */
	get_zd_data("0810",cif_inf_chg_log.memo);		/* 更改说明 */
		
	/** 处理流程 **/
	sprintf(acErrMsg,"cif_basic_inf_t.cif_no=[%ld]",cif_basic_inf_t.cif_no);
	WRITEMSG

	/* 检查客户号是否已经存在，并取其信息 */
	ret = Cif_basic_inf_Sel(g_pub_tx.reply,&cif_basic_inf,
							 "cif_no=%ld",cif_basic_inf_t.cif_no);
	if( ret==100 )
	{
		sprintf(acErrMsg,"无此客户号，请检查![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C007");
		ERR_DEAL;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"读取客户基本信息异常![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C006");
		ERR_DEAL;
	}
	
	/* 检查客户状态是否改变 */
/**	if (!strcmp (cif_basic_inf.sts , cif_basic_inf_t.sts ))
	{
		sprintf(acErrMsg,"未进行任何修改，请检查后提交![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C012"); 
		ERR_DEAL;
	}

	strcpy(beg_sts,cif_basic_inf.sts);	**/		/* 保存修改前状态 */
				
	/* 修改客户基本信息表 */
	memset (&cif_basic_inf, 0x00, sizeof(struct cif_basic_inf_c));
		
	ret = Cif_basic_inf_Dec_Upd(g_pub_tx.reply,"cif_no=%ld",
	                            cif_basic_inf_t.cif_no);
	if( ret )
	{
		sprintf( acErrMsg,"取客户基本信息表数据异常,游标DEC错误[%s]",ret);
		WRITEMSG
		goto ErrExit;
	}
		
	ret = Cif_basic_inf_Fet_Upd ( &cif_basic_inf, g_pub_tx.reply);
	if(ret) 
	{
		sprintf( acErrMsg,"取客户基本信息表数据异常,游标FET错误 [%s]",ret);
		WRITEMSG
		goto ErrExit;
	}			
	
	strcpy(cif_basic_inf.sts,cif_basic_inf_t.sts);

	ret = Cif_basic_inf_Upd_Upd( cif_basic_inf, g_pub_tx.reply);
	if(ret)
	{
		sprintf( acErrMsg,"执行Cif_basic_inf_Upd_Upd错 [%s]",ret);
		WRITEMSG
		goto ErrExit;
	} 
			
	Cif_basic_inf_Clo_Upd( );

	/* 登记客户信息修改日志 */
	cif_inf_chg_log.cif_no=cif_basic_inf_t.cif_no;	 /* 客户代码 */	
	memcpy(cif_inf_chg_log.chg_tablename,"cif_basic_inf",sizeof(cif_inf_chg_log.chg_tablename));  	 /* 被修改客户信息表名 */
	memcpy (cif_inf_chg_log.chg_columnname,"sts",sizeof(cif_inf_chg_log.chg_columnname));	 /* 修改客户信息列名 */	
	cif_inf_chg_log.chg_date=g_pub_tx.tx_date; 	 	 /* 修改日期 */	
	cif_inf_chg_log.chg_time=g_pub_tx.tx_time; 	 	 /* 修改时间 */	
	memcpy(cif_inf_chg_log.bef_content,beg_sts,sizeof(cif_inf_chg_log.bef_content));	 /* 修改前内容 */	
	memcpy(cif_inf_chg_log.aft_content,cif_basic_inf_t.sts,sizeof(cif_inf_chg_log.aft_content));		 /* 修改后内容 */ 
	memcpy(cif_inf_chg_log.clerk_no,g_pub_tx.tel,sizeof(cif_inf_chg_log.clerk_no)); 	 	 /* 职员编号 */	
	ret = Cif_inf_chg_log_Ins(cif_inf_chg_log);
	if( ret )
	{
		sprintf(acErrMsg,"登记客户信息修改日志失败![%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C013");
		goto ErrExit;
	}
			
	/* 登记交易流水 */
       strcpy(g_pub_tx.brf,"客户识别ID状态维护");
	if( pub_ins_trace_log() )
	{
		sprintf(acErrMsg,"登记交易流水错误!");
		WRITEMSG
		goto ErrExit;
	}
		
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"客户识别ID状态维护成功！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
ErrExit:
    sprintf(acErrMsg,"客户识别ID状态维护失败！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
