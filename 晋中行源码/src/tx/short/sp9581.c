/*****************************************************
* 文 件 名:  sp9581.c
* 功能描述： 查找zjgl_mst--资金管理状况表，返回基本信息
*  借方余额，贷方余额，开户日期，起息日期，科目号 科目控制字
*  FD1001     fd1002    FD44      FD45     FD64    FD65
*  计算利息类型，正常利率，借方基数，贷方基数，户名，交易限额
*   fd72          fd1005    fd56      fd67     fd25  fd1003
*  累计交易金额，限额周期，限额开始日期，限额结束日期,币种
*  fd1004          fd1007   FD46           FD47       fd1006
* 作    者:  LiuHuafeng 2006-10-25 17:58
* 完成日期： 2006-10-25 17:58
* 修改记录： 
* 日    期:
* 修 改 人:
* 修改内容:
*****************************************************/

#define EXTERN
#include <string.h>
#include "public.h"
#include "zjgl_mst_c.h"
int sp9581()
{
	int ret=0,i;
	struct zjgl_mst_c  sZjgl_mst;
	char cKinbr[BRNO_LEN+1];
	char cAcc_no[8];
	char cCur_no[3];
	memset(cCur_no, 0 , sizeof(cCur_no));
	memset(&sZjgl_mst, 0 , sizeof(sZjgl_mst));
	memset(cKinbr , 0 , sizeof(cKinbr));
	memset(cAcc_no, 0 , sizeof(cAcc_no));

	
	vtcp_log("sp9581 开始！[%s][%d]",__FILE__,__LINE__);
 	
	pub_base_sysinit();
	get_zd_data("0910",cKinbr);
	get_zd_data("0640",cAcc_no);
	get_zd_data("0210",cCur_no);
	vtcp_log("%s,%d curno====[%s] ",__FILE__,__LINE__,cCur_no);
	
	/******获取三个索引的值*******/
	ret = Zjgl_mst_Sel(g_pub_tx.reply,&sZjgl_mst, "opn_br_no='%s' and cur_no='%s' and acc_no ='%s' ", cKinbr,cCur_no,cAcc_no);
	if ( ret==100 )
	{
		sprintf(acErrMsg,"输入的科目不需要进行资金管理!!");
		WRITEMSG
		goto	ErrExit;
	}
	else if ( ret )
	{
		sprintf(acErrMsg,"数据库操作失败[%d]",ret);
		WRITEMSG
		goto ErrExit;
	}
/*  借方余额，贷方余额，开户日期，起息日期，科目号 科目控制字
*  FD1001     fd1002    FD44      FD45     FD64    FD65
*  计算利息类型，正常利率，借方基数，贷方基数，户名，交易限额
*   FD73          fd1005    fd56      fd57     fd25  fd1003
*  累计交易金额，限额周期，限额开始日期，限额结束日期,币种
*  fd1004          fd1007   FD46           FD47       fd1006
*/

	/***若交易限额为负数,则改为正数返回前台 add by wanglei 20061123***/
	vtcp_log("[%s][%d] lim_amt = %.2f\n",__FILE__,__LINE__,sZjgl_mst.lim_amt);
	if(sZjgl_mst.lim_amt < 0)
	{
		sZjgl_mst.lim_amt = -sZjgl_mst.lim_amt;
	}
	vtcp_log("[%s][%d] lim_amt = %.2f\n",__FILE__,__LINE__,sZjgl_mst.lim_amt);

	set_zd_double("1001",sZjgl_mst.dr_bal);
	set_zd_double("1002",sZjgl_mst.cr_bal);
	set_zd_double("1003",sZjgl_mst.lim_amt); /* 交易限额 */
	set_zd_double("1004",sZjgl_mst.lim_bal); /* 累计交易金额 */
	set_zd_double("1005",sZjgl_mst.rate); /* 利率 */
	set_zd_data("100A",cCur_no);/* 币种 */
	set_zd_data("100B",sZjgl_mst.lim_term); /* 周期 */
	set_zd_data("0250",sZjgl_mst.name); /* 名称 */
	set_zd_long("0440",sZjgl_mst.opn_date); /* 开户日期 */
	set_zd_long("0450",sZjgl_mst.ic_date); /* 起息日期 */
	set_zd_long("0460",sZjgl_mst.lim_beg_date); /* 限额开始日期 */
	set_zd_long("0470",sZjgl_mst.lim_end_date); /* 限额结束日期 */
	set_zd_double("0560",sZjgl_mst.dr_intst_acm); /* 借方基数 */
	set_zd_double("0570",sZjgl_mst.cr_intst_acm); /* 贷方基数 */
	set_zd_data("0650",sZjgl_mst.acc_hrt); /* 科目控制字 */
	set_zd_data("0730",sZjgl_mst.intst_knd); /* 计息类型 */
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"success !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;

ErrExit:
	sprintf(acErrMsg,"fail !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
