/*****************************************************
* 文 件 名:  sp9401.c
* 功能描述： 查询大额支付交易基本信息
* 作    者:  xyy
* 完成日期： 2006-9-1 22:06 
* 修改记录： 
* 日    期:
* 修 改 人:
* 修改内容:
*****************************************************/
#define EXTERN
#include <string.h>
#include "public.h"
#include "lv_define.h"
#include "lv_fd123.h"
#include "lv_pkgreg_c.h"
int sp9401()
{
	int ret=0,i;
	struct lv_pkgreg_c  lv_pkgreg;
	char cKinbr[BRNO_LEN+1];
	LV_FD123_AREA fd123;
	memset(&fd123, ' ',sizeof(fd123));
	memset(&lv_pkgreg,0x00,sizeof(struct lv_pkgreg_c));
	memset(cKinbr , 0 , sizeof(cKinbr));
	vtcp_log("sp9401 开始！[%s][%d]",__FILE__,__LINE__);
 	
	pub_base_sysinit();
	get_zd_data("0030",cKinbr);
	get_fd_data("1230",(char*)&fd123);
	vtcp_log("[%s,%d]1230=[%s]",__FILE__,__LINE__,(char*)&fd123);
	lv_pkgreg.wt_date=apatoi(fd123.wt_date,8);
	memcpy(lv_pkgreg.orderno,fd123.orderno,sizeof(lv_pkgreg.orderno)-1);
	memcpy(lv_pkgreg.or_br_no,fd123.or_br_no,sizeof(lv_pkgreg.or_br_no)-1);
	vtcp_log("+++++lv_pkgreg.or_br_no==[%s]!+++++",lv_pkgreg.or_br_no);
	zip_space(lv_pkgreg.or_br_no);
	if(strlen(lv_pkgreg.or_br_no)==0)
	{
		GetOr_br_noByBr_no(cKinbr, lv_pkgreg.or_br_no );
	}
	vtcp_log("sp9401 开始！[%s][%d]",__FILE__,__LINE__);	
	vtcp_log("[%s][%d]orderno=[%s],or_br_no=[%s]wt_date=[%d]",__FILE__,__LINE__,
		lv_pkgreg.orderno,lv_pkgreg.or_br_no,lv_pkgreg.wt_date);
	vtcp_log("sp9401 开始！[%s][%d]",__FILE__,__LINE__);
	ret = lv_pkgreg_Sel(g_pub_tx.reply,&lv_pkgreg, "orderno='%s' and or_br_no ='%s' and\
	 	wt_date=%s",lv_pkgreg.orderno,lv_pkgreg.or_br_no,lv_pkgreg.wt_date);
	vtcp_log("sp9401 开始！[%s][%d]",__FILE__,__LINE__);
	vtcp_log("[%s][%d]",__FILE__,__LINE__);
	if ( ret==100 )
	{
		sprintf(acErrMsg,"无此大额支付交易");
		WRITEMSG
		strcpy( g_pub_tx.reply, "HV01" );
		goto	ErrExit;
	}
	else if ( ret )
	{
		sprintf(acErrMsg,"查询大额支付交易基本信息出错[%d]",ret);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}	
	vtcp_log("sp9401 开始！[%s][%d]",__FILE__,__LINE__);
	vtcp_log("查询成功！  [%s][%d]",__FILE__,__LINE__);

	/*************暂时加这些东东，如果需要自己再往里加返回值，谢谢!*************/

	/**PKGNO***/
	memcpy(fd123.pkgno		,lv_pkgreg.pkgno,sizeof(fd123.pkgno));
	/**业务类型编号**/
	memcpy(fd123.txnum		,lv_pkgreg.txnum,sizeof(fd123.txnum));
	/**包编号**/
	memcpy(fd123.pack_id		,lv_pkgreg.packid,sizeof(fd123.pack_id));
	/**包日期**/
	apitoa(lv_pkgreg.pack_date	,sizeof(fd123.pack_date),fd123.pack_date);
	/**发起清算行行号**/
	memcpy(fd123.pay_qs_no		,lv_pkgreg.pay_qs_no,sizeof(fd123.pay_qs_no));
	/**发起行号 原发起行号 **/
	memcpy(fd123.or_br_no		,lv_pkgreg.or_br_no,sizeof(fd123.or_br_no));
	/**付款人开户行**/
	memcpy(fd123.pay_opn_br_no	,lv_pkgreg.pay_opn_br_no,sizeof(fd123.pay_opn_br_no));
	/**付款人帐号(原收款人帐号)**/
	memcpy(fd123.pay_ac_no		,lv_pkgreg.pay_ac_no,sizeof(fd123.pay_ac_no));
	/**付款人名称(原收款人名称)**/
	memcpy(fd123.pay_name		,lv_pkgreg.pay_name,sizeof(fd123.pay_name));
	/**付款人地址(原收款人地址)**/
	memcpy(fd123.pay_addr		,lv_pkgreg.pay_addr,sizeof(fd123.pay_addr));
	/**接受清算行号**/
	memcpy(fd123.cash_qs_no		,lv_pkgreg.cash_qs_no,sizeof(fd123.cash_qs_no));
	/**接受行号(原发起行号)**/
	memcpy(fd123.ac_br_no		,lv_pkgreg.ac_br_no,sizeof(fd123.ac_br_no));
	/**收款人开户行号**/
	memcpy(fd123.cash_opn_br_no	,lv_pkgreg.cash_opn_br_no,sizeof(fd123.cash_opn_br_no));
	/**收款人帐号,原付款人帐号**/
	memcpy(fd123.cash_ac_no		,lv_pkgreg.cash_ac_no,sizeof(fd123.cash_ac_no));
	/**收款人姓名,原付款人姓名**/
	memcpy(fd123.cash_name		,lv_pkgreg.cash_name,sizeof(fd123.cash_name));
	/**收款人地址**/
	memcpy(fd123.cash_addr		,lv_pkgreg.cash_addr,sizeof(fd123.cash_addr));
	/**业务种类**/	
	memcpy(fd123.yw_type		,lv_pkgreg.lv_yw_ind,sizeof(fd123.yw_type));
	/*交易序号 **/
	memcpy(fd123.orderno		,lv_pkgreg.orderno,sizeof(fd123.orderno));
	/**附言**/
	memcpy(fd123.brf		,lv_pkgreg.filler,sizeof(fd123.brf));
	/**原支付交易序号**/
	memcpy(fd123.o_orderno		,lv_pkgreg.o_orderno,sizeof(fd123.o_orderno));
	/**优先级别**/
	fd123.operlevel[0]=lv_pkgreg.operlevel[0];
	/**支付交易状态**/
	fd123.lv_sts[0]=lv_pkgreg.lv_sts[0];
	/**手续费标志**/
	fd123.chrg_ind[0]=lv_pkgreg.tx_chrg_ind[0];
	/**交易金额**/
	dot_str2dbl(fd123.tx_amt,sizeof(fd123.tx_amt),&lv_pkgreg.tx_amt);
	/**手续费金额**/
	dot_str2dbl(fd123.chrg_amt,sizeof(fd123.chrg_amt),&lv_pkgreg.chrg_amt);
	/**填充空格**/
	pub_base_full_space((char*)&fd123,sizeof(fd123));
	set_zd_data("1230",(char*)&fd123);
	vtcp_log("fd123=[%s]",(char*)&fd123);
	/*根据需要再增加*/
	if(!memcmp(lv_pkgreg.cur_no,"RMB",3))
	set_zd_data("0210","01");   /* 货币符号 */
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
