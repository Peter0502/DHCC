/*****************************************************
* 文 件 名:  sp9566.c
* 功能描述： 查询小额支付交易基本信息 根据包信息查询!!!!!
* 作    者:  Liuyue 
* 完成日期： 2006年9月9日 
* 修改记录： 
* 日    期:
* 修 改 人:
* 修改内容:
* 注    意: 这里依旧采用了大额的fd123域,所以这个短服务,  
*	只是用于小额的非支付交易中, 只是用于小额的非支付交易中,
*	如果在支付交易中需要查询小额支付交易的   
*       明细需要采用sp9604这个短服务 
*****************************************************/

#define EXTERN
#include <string.h>
#include "public.h"
#include "hv_define.h"
#include "lv_pkgreg_c.h"
int sp9566()
{
	int ret=0,i;
	char cDate[9];
	struct lv_pkgreg_c  lv_pkgreg;
	char cKinbr[BRNO_LEN+1];
	char cBuff[2001];
	char cFilename[1000];
	memset(cFilename, 0 , sizeof(cFilename));
	HV_FD123_AREA fd123;
	memset(cBuff, 0 , sizeof(cBuff));
	memset(cDate, 0 , sizeof(cDate));
	memset(&fd123, ' ',sizeof(fd123));
	memset(&lv_pkgreg,0x00,sizeof(struct lv_pkgreg_c));
	memset(cKinbr , 0 , sizeof(cKinbr));
	vtcp_log("sp9566 开始！[%s][%d]",__FILE__,__LINE__);
 	
	pub_base_sysinit();

	get_zd_data("0030",cKinbr);
	get_fd_data("1230",(char*)&fd123);

	vtcp_log("[%s,%d]1230=[%s]",__FILE__,__LINE__,(char*)&fd123);

	/******获取三个索引的值*******/
	memcpy(lv_pkgreg.packid,fd123.orderno,sizeof(lv_pkgreg.packid)-1);
	memcpy(lv_pkgreg.or_br_no,fd123.or_br_no,sizeof(lv_pkgreg.or_br_no)-1);
	get_zd_long("0440",&lv_pkgreg.pack_date);	   /*委托日期*/	
	memcpy(lv_pkgreg.pkgno,fd123.o_cmtno,sizeof(lv_pkgreg.pkgno)-1);
	zip_space(lv_pkgreg.or_br_no);
	if(strlen(lv_pkgreg.or_br_no)==0)
	{
		GetOr_br_noByBr_no(cKinbr, lv_pkgreg.or_br_no );
	}

	
	vtcp_log("[%s][%d]根据这里锁定小额支付表:wt_date=[%ld],or_br_no=[%s],orderno=[%s]\n",__FILE__,__LINE__,lv_pkgreg.wt_date,lv_pkgreg.or_br_no,lv_pkgreg.orderno);
	
	vtcp_log("sp9566 开始！[%s][%d]",__FILE__,__LINE__);
	vtcp_log("sp9566 开始！[%s][%d]",__FILE__,__LINE__);
	vtcp_log("[%s][%d]fd123.o_txnum==[%s]\n",__FILE__,__LINE__,fd123.o_txnum);
	vtcp_log("[%s][%d]lv_pkgreg.txnum==[%s]\n",__FILE__,__LINE__,lv_pkgreg.txnum);
	zip_space(lv_pkgreg.txnum);	

	ret = Lv_pkgreg_Sel(g_pub_tx.reply,&lv_pkgreg, "packid='%s' and or_br_no='%s' and pack_date=%d and pkgno='%s'" , \ 
		lv_pkgreg.packid,lv_pkgreg.or_br_no,lv_pkgreg.pack_date,lv_pkgreg.pkgno);
	vtcp_log("[%s][%d]",__FILE__,__LINE__);
	if ( ret==100 )
	{
		sprintf(acErrMsg,"无此小额支付交易");
		WRITEMSG
		strcpy( g_pub_tx.reply, "9999" ); /***对于查询 即使不存在也要程序继续***/
		goto	ErrExit1;
	}
	else if ( ret )
	{
		sprintf(acErrMsg,"查询小额支付交易基本信息出错[%d]",ret);
		WRITEMSG
		goto ErrExit;
	}	
   	vtcp_log("查询成功！  [%s][%d]",__FILE__,__LINE__);
   	vtcp_log("查询成功！  [%s][%d]",__FILE__,__LINE__);

	memcpy(fd123.pay_ac_no       ,lv_pkgreg.pay_ac_no       ,sizeof(fd123.pay_ac_no       ));
	memcpy(fd123.pay_name        ,lv_pkgreg.pay_name        ,sizeof(fd123.pay_name        ));
	memcpy(fd123.pay_addr        ,lv_pkgreg.pay_addr        ,sizeof(fd123.pay_addr        ));

	memcpy(fd123.ac_br_no        ,lv_pkgreg.ac_br_no        ,sizeof(fd123.ac_br_no        ));

	memcpy(fd123.cash_ac_no      ,lv_pkgreg.cash_ac_no      ,sizeof(fd123.cash_ac_no      ));
	memcpy(fd123.cash_name       ,lv_pkgreg.cash_name       ,sizeof(fd123.cash_name       ));
	memcpy(fd123.cash_addr       ,lv_pkgreg.cash_addr       ,sizeof(fd123.cash_addr       ));

	memcpy(fd123.orderno         ,lv_pkgreg.orderno         ,sizeof(fd123.orderno         ));
	memcpy(fd123.o_orderno       ,lv_pkgreg.o_orderno       ,sizeof(fd123.o_orderno       ));

	pub_base_full_space((char*)&fd123,sizeof(fd123));
	set_zd_data	("1230",(char*)&fd123);
    	set_zd_double	("0390",lv_pkgreg.tx_amt);   /* 交易金额 */
        set_zd_long	("0450",lv_pkgreg.o_wt_date);   /* 原委托日期 */

	vtcp_log("PAY_AC_NO==[%s]\n",fd123.pay_ac_no);
	vtcp_log("cash_AC_NO==[%s]\n",fd123.cash_ac_no);

	 
		
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
ErrExit1:
	sprintf(acErrMsg,"fail !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}


