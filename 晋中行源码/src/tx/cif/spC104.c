/***********************************************
* 文 件 名: spC104.c
* 功能描述：客户别名信息录入
*
* 作    者: andy
* 完成日期：2004年01月15号
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
#include "cif_alias_inf_c.h"

	static struct cif_basic_inf_c	  cif_basic_inf;
	static struct cif_alias_inf_c     cif_alias_inf;
	
spC104()
{
	int ret=0;
	
	memset(&cif_basic_inf,0x00,sizeof(struct cif_basic_inf_c));
	memset(&cif_alias_inf,0x00,sizeof(struct cif_alias_inf_c));

	get_zd_long("0280",&cif_alias_inf.cif_no);	/*客户号*/
	get_zd_data("0250",cif_alias_inf.alias);	/*客户别名*/

	/** 初始化全局变量 **/
	pub_base_sysinit();

	/** 处理流程 **/
	/* 检查客户ID是否存在 */
	ret=Cif_basic_inf_Sel(g_pub_tx.reply,&cif_basic_inf,
					"cif_no=%ld",cif_alias_inf.cif_no);
	if(ret==100)
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

	/*查询别名信息表是否存在记录*/
	ret=sql_count("cif_alias_inf","cif_no=%ld",cif_alias_inf.cif_no);
	if(ret<0)
	{
		sprintf(acErrMsg,"FUNCTION SQL_COUNT ERROR![%d]",ret);
		WRITEMSG
		goto ErrExit;
	}
	else if(ret==0)
	{
		cif_alias_inf.alias_seqn=1;

		/* 登记客户别名信息表 */
		ret=Cif_alias_inf_Ins(cif_alias_inf);
		if(ret)
		{
			sprintf(acErrMsg,"登记客户别名信息表错误![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"C021");
			goto ErrExit;
		}
	}
	else 
	{
		ret=sql_max_int("cif_alias_inf","alias_seqn", \
						"cif_no=%ld",cif_alias_inf.cif_no);
		if(ret<0)
		{
			sprintf(acErrMsg,"FUNCTION SQL_MAX_INT ERROR![%d]",ret);
			WRITEMSG
			goto ErrExit;
		}
		else if(ret)
		{
			cif_alias_inf.alias_seqn=ret+1;

			/* 登记客户别名信息表 */
			ret=Cif_alias_inf_Ins(cif_alias_inf);
			if(ret)
			{
				sprintf(acErrMsg,"登记客户别名信息表错误![%d]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"C021");
				goto ErrExit;
			}
		}
	}

	/* 登记交易流水 */
	if(pub_ins_trace_log())
	{
		sprintf(acErrMsg,"登记交易流水错误!");
		WRITEMSG
		goto ErrExit;
	}

	/*输出变量*/
	set_zd_long("0350",cif_alias_inf.alias_seqn);

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"客户别名信息录入成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"客户别名信息录入失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
