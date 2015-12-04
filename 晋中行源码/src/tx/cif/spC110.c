/*************************************************
* 文 件 名: spC110.c
* 功能描述：客户授信额度录入
*
* 作    者: andy
* 完成日期：2004年01月24日
* 修改记录：
*     1. 日    期:
*修 改 人:
*修改内容:
*     2.
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;}
#include "stdio.h"
#define EXTERN
#include "public.h"
#include "trace_log_c.h"
#include "cif_basic_inf_c.h"
#include "cif_crdt_inf_c.h"
#include "cif_crdt_code_c.h"

	static struct	cif_basic_inf_c		cif_basic_inf;
	static struct	cif_crdt_inf_c		cif_crdt_inf;
	static struct	cif_crdt_code_c		cif_crdt_code;

spC110()
{
	int ret=0;

	/** 初始化全局变量 **/
	pub_base_sysinit();
	sprintf(acErrMsg,"初始化全局变量PASS!");
	WRITEMSG

	/** 数据初始化 **/
	memset(&cif_basic_inf,0x00,sizeof(struct cif_basic_inf_c));
	memset(&cif_crdt_code,0x00,sizeof(struct cif_crdt_code_c));
	memset(&cif_crdt_inf,0x00,sizeof(struct cif_crdt_inf_c));

	/** 取值，赋值**/
	get_zd_long("0280",&cif_crdt_code.cif_no);
	get_zd_data("0580",cif_crdt_code.credit_no);
	get_zd_data("0580",cif_crdt_inf.credit_no);
	get_zd_data("0210",cif_crdt_inf.cur_no);
	get_zd_double("0400",&cif_crdt_inf.bal);
	get_zd_data("0670",cif_crdt_inf.cyc_ind);

	/** 流程处理 **/
	/* 检查客户号是否存在 */
	ret=Cif_basic_inf_Sel(g_pub_tx.reply,&cif_basic_inf,"cif_no=%ld",
					cif_crdt_code.cif_no);
	if( ret==100)
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

	/* 登记客户授信代号对照表 */
	ret=Cif_crdt_code_Ins(cif_crdt_code);
	if( ret )
	{
		sprintf(acErrMsg,"登记客户授信代号对照表失败![%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C028");
		ERR_DEAL;
	}

	/* 登记客户授信信息表 */
	ret=Cif_crdt_inf_Ins(cif_crdt_inf);
	if( ret )
	{
		sprintf(acErrMsg,"登记客户授信信息表失败![%d]");
		WRITEMSG
		strcpy(g_pub_tx.reply,"C029");
		ERR_DEAL;
	}

	/* 登记交易流水 */
	if(pub_ins_trace_log())
	{
		sprintf(acErrMsg,"登记交易流水错误!");
		WRITEMSG
		goto ErrExit;
	}

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"登记客户授信信息成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
  	sprintf(acErrMsg,"登记客户授信信息失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

