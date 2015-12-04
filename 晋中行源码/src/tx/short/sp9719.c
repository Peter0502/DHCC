/*************************************************
* 文 件 名: sp9719.c
* 功能描述：根据客户号和合同编号回显信息
*
* 作    者: rob
* 完成日期：2003年03月31日
* 修改记录： 
*     1. 日    期:
*        修 改 人:
*        修改内容:
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#define EXTERN
#include "public.h"
#include "cif_crdt_code_c.h"
#include "cif_crdt_inf_c.h"
#include "cif_basic_inf_c.h"

sp9719()  
{ 		
	int ret=0;
	char tmp_title[41];

	struct cif_basic_inf_c	g_cif_basic_inf;
	struct cif_crdt_code_c	g_cif_crdt_code;
	struct cif_crdt_inf_c	g_cif_crdt_inf;

	memset(&g_cif_basic_inf,0x00,sizeof(struct cif_basic_inf_c));
	memset(&g_cif_crdt_code,0x00,sizeof(struct cif_crdt_code_c));
	memset(&g_cif_crdt_inf,0x00,sizeof(struct cif_crdt_inf_c));

	/*系统初始化（初始化g_pub_tx)数据初始化*/
	pub_base_sysinit();

	/** 取值、赋值 **/	
	get_zd_long("0280",&g_cif_crdt_code.cif_no);			/*客户号*/
	get_zd_data("0580",g_cif_crdt_code.credit_no);			/*合同编号*/


		sprintf(acErrMsg,"客户号不存在[%ld]",g_cif_crdt_code.cif_no);
		WRITEMSG
	ret=Cif_basic_inf_Sel(g_pub_tx.reply,&g_cif_basic_inf,"cif_no=%ld", \
							g_cif_crdt_code.cif_no);
	if(ret==100)
	{
		sprintf(acErrMsg,"客户号不存在");
		WRITEMSG
		strcpy(g_pub_tx.reply,"C007");
		goto ErrExit;
	}
	else if(ret)
	{
		sprintf(acErrMsg,"查询客户基本信息表错误[%ld]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"O065");
		goto ErrExit;
	}

	ret=Cif_crdt_code_Sel(g_pub_tx.reply,&g_cif_crdt_code, \
							"cif_no=%ld and credit_no='%s'", \
							g_cif_crdt_code.cif_no, \
							g_cif_crdt_code.credit_no);
	if(ret==100)
	{
		sprintf(acErrMsg,"该客户不存在此授信合同");
		WRITEMSG
		strcpy(g_pub_tx.reply,"C084");
		goto ErrExit;
	}
	else if(ret)
	{
		sprintf(acErrMsg,"查询客户授信代号对照表错误[%ld]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"O065");
		goto ErrExit;
	}

	ret=Cif_crdt_inf_Sel(g_pub_tx.reply,&g_cif_crdt_inf, \
						"credit_no='%s'", g_cif_crdt_code.credit_no);
	if(ret==100)
	{
		sprintf(acErrMsg,"不存在该授信合同信息");
		WRITEMSG
		strcpy(g_pub_tx.reply,"C085");
		goto ErrExit;
	}
	else if(ret)
	{
		sprintf(acErrMsg,"查询客户授信信息表错误[%ld]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"O065");
		goto ErrExit;
	}

	set_zd_data("0210",g_cif_crdt_inf.cur_no);
	set_zd_double("0400",g_cif_crdt_inf.bal);
	set_zd_data("0680",g_cif_crdt_inf.cyc_ind);

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
   
