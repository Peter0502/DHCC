/*************************************************
* 文 件 名:  spN303.c
* 功能描述：积分查询
*
* 作    者:  andy
* 完成日期： 2004年3月7日
*
* 修改记录：
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/
#include "pubf_data_code.h"
#define EXTERN
#include "public.h"
#include "point_point_mst_c.h"

	static struct point_point_mst_c		point_point_mst;

spN303()
{
	int ret=0;
	
	/** 初始化全局变量 **/
	pub_base_sysinit();

	/** 数据初始化 **/
	 memset(&point_point_mst,0x00,sizeof(struct point_point_mst_c));

	/** 取值、赋值 **/
	get_zd_data("1021",point_point_mst.ac_no);

	/** 处理流程 **/
	/* 查询积分主表 */
	ret=Point_point_mst_Sel(g_pub_tx.reply,&point_point_mst,
					"ac_no='%s'",point_point_mst.ac_no);
	if(ret==100)
	{
		sprintf(acErrMsg,"无此账号，请检查![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"N062");
		goto ErrExit;
	}
	else if(ret)
	{
		sprintf(acErrMsg,"查询积分主表错误![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"N064");
		goto ErrExit;
	}
	
	/*  输出变量 */
	set_zd_long("0460",point_point_mst.used_point);
	set_zd_long("0470",point_point_mst.point);

	/** 登记交易流水 **/
	if(pub_ins_trace_log())
	{
		sprintf(acErrMsg,"登记交易流水错误!");
		WRITEMSG
		goto ErrExit;
	}

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"积分查询成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;

ErrExit:
	sprintf(acErrMsg,"积分查询失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
	
