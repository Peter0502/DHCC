/*************************************************
* 文 件 名:  sp9892.c
* 功能描述： 客户工作历史信息维护快捷查询
*
* 作    者:  rob
* 完成日期： 2003年2月3日
*
* 修改记录： 
* 日   期:
* 修 改 人:
* 修改内容:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "cif_basic_inf_c.h"
#include "cif_wrk_hst_inf_c.h"

int sp9892()
{
	int ret = 0;
	int wrk_no;
	long cif_no;
	char sts[2];

	struct cif_basic_inf_c g_cif_basic_inf;			
	struct cif_wrk_hst_inf_c g_cif_wrk_hst_inf;			

	/*清空*/
	memset(&g_cif_basic_inf,0x00,sizeof(struct cif_basic_inf_c));
	memset(&g_cif_wrk_hst_inf,0x00,sizeof(struct cif_wrk_hst_inf_c));

	/*初始化*/
	pub_base_sysinit();

	/*取值*/
	get_zd_data("0700",sts);				/*操作类型*/
	get_zd_long("0280",&cif_no);			/*客户号*/
	get_zd_int("0490",&wrk_no);				/*工作序号*/

	/*流程*/
	/*查询客户基本信息表*/
	ret=Cif_basic_inf_Sel( g_pub_tx.reply , &g_cif_basic_inf ,
			 				"cif_no=%ld",cif_no);
	if( ret==100 )
	{
		sprintf(acErrMsg,"客户基本信息表中不存在该客户号!!cif_no=[%ld]",cif_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C051");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf( acErrMsg, "查询客户信息表异常!!ret=[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	/*查询客户地址信息表*/
	ret=Cif_wrk_hst_inf_Sel(g_pub_tx.reply,&g_cif_wrk_hst_inf,
						"cif_no=%ld and wrk_no=%d",cif_no,wrk_no);
	if( ret==100 )
	{
		sprintf(acErrMsg,"客户工作历史信息表不存在该记录!!cif_no=[%ld]",cif_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C068");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf( acErrMsg, "查询客户工作历史信息表异常!!ret=[%d]", ret );
		WRITEMSG
		strcpy(g_pub_tx.reply,"C069");
		goto ErrExit;
	}

	pub_base_strpack(g_cif_wrk_hst_inf.industry_no);	

	set_zd_data("0670",g_cif_wrk_hst_inf.wrk_ind);		/*工作状态*/
	set_zd_data("0220",g_cif_wrk_hst_inf.industry_no);	/*行业代码*/
	set_zd_data("0680",g_cif_wrk_hst_inf.vocation_no);	/*职业代码*/
	set_zd_data("0690",g_cif_wrk_hst_inf.position_no);	/*职位代码*/

	set_zd_data("0260",g_cif_wrk_hst_inf.unit_name);	/*单位名称*/
	set_zd_long("0440",g_cif_wrk_hst_inf.beg_date);		/*入职日期*/
	set_zd_long("0450",g_cif_wrk_hst_inf.end_date);		/*离职日期*/
	set_zd_double("0410",g_cif_wrk_hst_inf.income);		/*年薪收入*/

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
