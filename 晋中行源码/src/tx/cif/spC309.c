/*************************************************
* 文 件 名: spC309.c
* 功能描述：
		客户工作历史信息维护
*
* 作    者: 
* 完成日期：2004年01月26日
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

	static struct cif_basic_inf_c		cif_basic_inf;
	static struct cif_wrk_hst_inf_c		cif_wrk_hst_inf;
	static struct cif_wrk_hst_inf_c		g_cif_wrk_hst_inf;
	
	char old_alias[30],tbwhere[50],memo[41];
	char sts[2];

spC309()
{
	int ret=0;
	int cmp_code=0;

	/** 初始化全局变量 **/
	pub_base_sysinit();

	/** 数据初始化 **/
	memset(&cif_basic_inf,0x00,sizeof(struct cif_basic_inf_c));
	memset(&cif_wrk_hst_inf,0x00,sizeof(struct cif_wrk_hst_inf_c));
	memset(&g_cif_wrk_hst_inf,0x00,sizeof(struct cif_wrk_hst_inf_c));

	/** 取值、赋值 **/
	get_zd_data("0700",sts);								/*操作类型*/
	get_zd_long("0280",&g_cif_wrk_hst_inf.cif_no);			/*客户号*/
	get_zd_long("0490",&g_cif_wrk_hst_inf.wrk_no);			/*工作序号*/

	get_zd_data("0670",g_cif_wrk_hst_inf.wrk_ind);			/*工作状态*/
	get_zd_data("0220",g_cif_wrk_hst_inf.industry_no);		/*行业代码*/
	get_zd_data("0680",g_cif_wrk_hst_inf.vocation_no);		/*职业代码*/
	get_zd_data("0690",g_cif_wrk_hst_inf.position_no);		/*职位代码*/

	get_zd_data("0260",g_cif_wrk_hst_inf.unit_name);		/*单位名称*/
	get_zd_long("0440",&g_cif_wrk_hst_inf.beg_date);		/*入职日期*/
	get_zd_long("0450",&g_cif_wrk_hst_inf.end_date);		/*离职日期*/
	get_zd_double("0410",&g_cif_wrk_hst_inf.income);		/*年薪收入*/

	sprintf(acErrMsg,"%s",g_cif_wrk_hst_inf.unit_name);
	WRITEMSG	
	/** 处理流程 **/
	switch(sts[0])
	{
		case '1':										/*1-修改*/
				ret=Cif_wrk_hst_inf_Dec_Upd(g_pub_tx.reply,
				 	"cif_no=%ld and wrk_no=%d",g_cif_wrk_hst_inf.cif_no,
					g_cif_wrk_hst_inf.wrk_no);
			 	if (ret)
			 	{
					sprintf(acErrMsg,"DECLARE Cif_wrk_hst_inf_rel游标异常![%s]",
						   g_pub_tx.reply);
				  	WRITEMSG
					strcpy(g_pub_tx.reply,"C006");
					ERR_DEAL;
				}
				ret=Cif_wrk_hst_inf_Fet_Upd(&cif_wrk_hst_inf,g_pub_tx.reply);
				if (ret)
				{
				   	sprintf(acErrMsg,"FETCH Cif_wrk_hst_inf游标异常![%s]",
							g_pub_tx.reply);
					WRITEMSG
					strcpy(g_pub_tx.reply,"C006");
					ERR_DEAL;
				}
				
				
			memcpy(g_cif_wrk_hst_inf.rowid, cif_wrk_hst_inf.rowid,sizeof(g_cif_wrk_hst_inf.rowid));
				ret=Cif_wrk_hst_inf_Upd_Upd(g_cif_wrk_hst_inf,g_pub_tx.reply);
				if (ret)
				{
    				sprintf(acErrMsg,"UPDATE Cif_wrk_hst_inf游标异常![%s]",
							g_pub_tx.reply);
					WRITEMSG
					strcpy(g_pub_tx.reply,"C006");
					ERR_DEAL;
				}
				Cif_wrk_hst_inf_Clo_Upd( );
				break;
		case '2':										/*2-删除*/
				/*删除客户工作历史信息表内容*/
				sprintf(tbwhere,"cif_no=%ld and wrk_no=%d",
						g_cif_wrk_hst_inf.cif_no,g_cif_wrk_hst_inf.wrk_no);
				ret =deldb("cif_wrk_hst_inf",tbwhere);
				if( ret )
				{
					 sprintf(acErrMsg,"删除客户工作历史信息表记录异常!![%s]",
							ret);
					 WRITEMSG
					 strcpy(g_pub_tx.reply,"C053");
					 ERR_DEAL;
				}
				break;
		default:
				break;
	}

    /* 登记交易流水 */
    if( pub_ins_trace_log() )
    {
         sprintf(acErrMsg,"登记交易流水错误!");
         WRITEMSG
         goto ErrExit;
    }

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"客户工作信息维护成功![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;

ErrExit:
    sprintf(acErrMsg,"客户工作信息维护失败![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}

