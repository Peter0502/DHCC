/*************************************************
* 文 件 名:  spN302.c
* 功能描述：积分兑换
*
* 作    者:  andy
* 完成日期： 2004年3月5日
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
#include "point_mst_hst_c.h"

	static struct point_point_mst_c		point_point_mst;
	static struct point_mst_hst_c		point_mst_hst;
	static struct mdm_ac_rel_c              mdm_ac_rel;

spN302()
{
	int ret=0;
	int spoint=0;
	char memo[40];
	
	/** 初始化全局变量 **/
	pub_base_sysinit();

	/** 数据初始化 **/
	memset(&point_point_mst,0x00,sizeof(struct point_point_mst_c));
	memset(&point_mst_hst,0x00,sizeof(struct point_mst_hst_c));
	memset(&mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	
	/** 取值、赋值 **/
	get_zd_data("1021",point_point_mst.ac_no);	/*卡/账号*/
	get_zd_int("0460",&spoint);			/*本次使用积分*/
	get_zd_data("0810",memo);			/*享受服务描述*/

	/** 处理流程 **/
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
	if(ret==100)
	{
		sprintf(acErrMsg,"查询积分主表错误![%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"N064");
		goto ErrExit;
	}
	 else if(ret)
        {
                sprintf(acErrMsg,"执行Point_point_mst_Fet_Upd 错[%d]",ret);
                WRITEMSG
                goto ErrExit;
        }

	point_point_mst.point=point_point_mst.point-spoint;
	point_point_mst.used_point=point_point_mst.used_point+spoint;

	ret=Point_point_mst_Upd_Upd(point_point_mst,g_pub_tx.reply);
	if(ret!=0)
	{
		sprintf(acErrMsg,"修改积分主表错误!");
		WRITEMSG
		goto ErrExit;
	}
	
	ret=Point_point_mst_Clo_Upd();

	/*登记积分使用明细表*/
	strcpy(point_mst_hst.ac_no,point_point_mst.ac_no);
	strcpy(point_mst_hst.cif_type,point_point_mst.cif_type);
	point_mst_hst.used_point=spoint;
	point_mst_hst.used_date=g_pub_tx.tx_date;
	strcpy(point_mst_hst.brf,memo);
	
	ret=Point_mst_hst_Ins(point_mst_hst,g_pub_tx.reply);
	if(ret)
	{
		sprintf(acErrMsg,"登记积分使用明细表错误![%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"N066");
		goto ErrExit;
	}
	/* 输出变量 */
	set_zd_long("0470",point_point_mst.point);

	

	/*登记交易流水*/
	if(pub_ins_trace_log())
	{
		sprintf(acErrMsg,"登记交易流水错误!");
		WRITEMSG
		goto ErrExit;
	}


OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"积分兑换成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;

ErrExit:
	sprintf(acErrMsg,"积分兑换失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}




