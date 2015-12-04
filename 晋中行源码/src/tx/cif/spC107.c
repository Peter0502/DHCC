/*************************************************
* 文 件 名: spC107.c
* 功能描述：客户工作信息录入
*
* 作    者: andy
* 完成日期：2004年01月17日
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
#include "cif_wrk_hst_inf_c.h"

static struct cif_basic_inf_c	cif_basic_inf;
static struct cif_wrk_hst_inf_c	cif_wrk_hst_inf;

spC107()
{
	int ret=0;

	/** 初始化全局变量 **/
    pub_base_sysinit();
    sprintf(acErrMsg,"初始化全局变量 PASS!");
    WRITEMSG

    /** 数据初始化 **/
    memset (&cif_basic_inf, 0x00, sizeof(struct cif_basic_inf_c));
	memset (&cif_wrk_hst_inf,0x00,sizeof(struct cif_wrk_hst_inf_c));

    /** 取值、赋值 **/
	get_zd_long("0280",&cif_wrk_hst_inf.cif_no);		/*客户ID*/
	get_zd_data("0670",cif_wrk_hst_inf.wrk_ind);		/*工作状态*/
	get_zd_data("0220",cif_wrk_hst_inf.industry_no); 	/*行业类别代码*/
	get_zd_data("0680",cif_wrk_hst_inf.vocation_no); 	/*职业代码*/
	get_zd_data("0690",cif_wrk_hst_inf.position_no); 	/*职位代码*/

	get_zd_data("0260",cif_wrk_hst_inf.unit_name);		/*单位*/	
	get_zd_long("0440",&cif_wrk_hst_inf.beg_date);		/*入职日期*/
	get_zd_long("0450",&cif_wrk_hst_inf.end_date); 		/*离职日期*/
	get_zd_double("0410",&cif_wrk_hst_inf.income);		/*年薪收入*/
	
	/** 处理流程 **/
	/* 检查客户号是否已经存在 */
	ret=Cif_basic_inf_Sel( g_pub_tx.reply,&cif_basic_inf,
							"cif_basic_inf.cif_no=%ld",cif_wrk_hst_inf.cif_no);
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
	
	/*检查客户类型是个人还是公司*/
	if(cif_basic_inf.type[0]!='1')
	{
 		sprintf(acErrMsg,"该客户为对公客户，不得登记!![%s]",g_pub_tx.reply);
        WRITEMSG
        strcpy(g_pub_tx.reply,"C081");
        ERR_DEAL;
    }

	/* 检查客户工作历史信息表是否存在记录*/
	ret=sql_count("cif_wrk_hst_inf","cif_no=%ld",cif_wrk_hst_inf.cif_no);
	if(ret<0)
	{
		sprintf(acErrMsg,"FUNCTION SQL_COUNT ERROR![%d]",ret);
		WRITEMSG
		goto ErrExit;
	}
	else if(ret==0)
	{
		cif_wrk_hst_inf.wrk_no=1;

		/* 登记客户工作历史信息表 */
		ret=Cif_wrk_hst_inf_Ins(cif_wrk_hst_inf);
		if( ret )
		{
			sprintf(acErrMsg,"登记客户工作历史信息表失败![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"C022");
			goto ErrExit;
		}
	}
	else
	{
		ret=sql_max_int("cif_wrk_hst_inf","wrk_no",\
						"cif_no=%ld",cif_wrk_hst_inf.cif_no);
		if(ret<0)
		{
			sprintf(acErrMsg,"FUNCTION SQL_MAX_INT ERROR![%d]",ret);
			WRITEMSG
			goto ErrExit;
		}
		else if(ret)
		{
			cif_wrk_hst_inf.wrk_no=ret+1;

			/* 登记客户工作历史信息表 */
			ret=Cif_wrk_hst_inf_Ins(cif_wrk_hst_inf);
			if( ret )
			{
				sprintf(acErrMsg,"登记客户工作历史信息表失败![%d]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"C022");
				goto ErrExit;
			}

		}
	}
				
	set_zd_long("0350",cif_wrk_hst_inf.wrk_no);

	/*登记交易流水*/
	if( pub_ins_trace_log() )
    {
        sprintf(acErrMsg,"登记交易流水错误!");
        WRITEMSG
        goto ErrExit;
    }


OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"客户工作信息录入成功!!%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
    sprintf(acErrMsg,"客户工作信息录入失败!!%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}

