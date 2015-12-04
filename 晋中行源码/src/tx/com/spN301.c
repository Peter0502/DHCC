/*************************************************
* 文 件 名:  spN301.c
* 功能描述：奖励积分管理

*
* 作    者:  andy
* 完成日期： 2004年3月3日
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
	static struct mdm_ac_rel_c		mdm_ac_rel;

spN301()
{
	int ret=0;
	int addpoint=0;
	
	/** 初始化全局变量 **/
	pub_base_sysinit();

	/** 数据初始化 **/
	memset(&point_point_mst,0x00,sizeof(struct point_point_mst_c));
	memset(&mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));

	/** 取值、赋值 **/
	get_zd_data("1021",point_point_mst.ac_no);	/*卡/账号*/	
	get_zd_int("0470",&addpoint);			/*奖励积分*/

	/**处理流程 **/
	/* 检查账号是否存在 */
	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&mdm_ac_rel,
					"ac_no='%s'",point_point_mst.ac_no);
	if(ret==100)
	{
		sprintf(acErrMsg,"无此账号，请检查!![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"N062");
		goto ErrExit;
	}
	else if(ret)
	{
		sprintf(acErrMsg,"查询账号出错![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply," N063");
		goto ErrExit;
	}

	
	/* 修改积分主表 */
	ret=Point_point_mst_Dec_Upd(g_pub_tx.reply,"ac_no='%s'",point_point_mst.ac_no);
	if(ret)
	{
		sprintf(acErrMsg,"执行Point_point_mst_Dec_Upd错[%d]",ret);
		WRITEMSG
		goto ErrExit;
	}

	ret=Point_point_mst_Fet_Upd(&point_point_mst,g_pub_tx.reply);
	if(ret == 100)	/*---记录不存在---*/
	{
		/*---如果记录不存在则先进行添加---*/
		strcpy(point_point_mst.ac_no,point_point_mst.ac_no);
		strcpy(point_point_mst.cif_type,"1");
		point_point_mst.beg_date=g_pub_tx.tx_date;
		point_point_mst.point=addpoint;
		point_point_mst.yt_point=0;
		point_point_mst.used_point=0;

		ret=Point_point_mst_Ins(point_point_mst);
		if(ret)
		{
			sprintf(acErrMsg,"添加积分主表错误![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"N065");
			goto ErrExit;
		}
		goto AAA;
	}
	else if(ret)
	{
		sprintf(acErrMsg,"执行Point_point_mst_Fet_Upd 错[%d]",ret);
		WRITEMSG
		goto ErrExit;
	}

	point_point_mst.yt_point = point_point_mst.point;
	addpoint=point_point_mst.point+addpoint;
	point_point_mst.point=addpoint;
	ret=Point_point_mst_Upd_Upd(point_point_mst,g_pub_tx.reply);
	if(ret !=0)
	{
		sprintf(acErrMsg,"修改积分主表错误!");
		WRITEMSG
		goto ErrExit;
	}
	
	Point_point_mst_Clo_Upd();
	
AAA:
	 /* 登记交易流水 */
     if( pub_ins_trace_log() )
     {
         sprintf(acErrMsg,"登记交易流水错误!");
         WRITEMSG
         goto ErrExit;
     }


OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"奖励积分管理成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"奖励积分管理失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}




