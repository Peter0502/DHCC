/*****************************************************
* 文 件 名:  sp9400.c
* 功能描述： 根据要发送的信息，找到此笔发送信息的明细  hv_zf
* 作    者:  xyy
* 完成日期： 2006年8月16日 
* 修改记录： 
* 日    期:
* 修 改 人:
* 修改内容:
*****************************************************/

#define EXTERN
#include <string.h>
#include "hv_define.h"
int sp9400()
{
	int ret=0;
	long lWtdate;
	char cKinbrBakcode[13];
	char cOrderno[9];
	char cCmtno[4];
	HV_FD123_AREA fd123;
	struct hv_zf_c hv_zf;

	lWtdate=0;
	memset(cOrderno,0,sizeof(cOrderno)-1);
	memset(cCmtno,0,sizeof(cCmtno)-1);
	memset(cKinbrBakcode,0,sizeof(cKinbrBakcode)-1);
	memset(&fd123, 0 , sizeof(fd123));
	memset(&hv_zf,0x00,sizeof(hv_zf));
	vtcp_log("sp9400开始！[%s][%d]",__FILE__,__LINE__);

	get_zd_data("1230",(char*)&fd123);
	vtcp_log("得到123结构体的内容=[%s]",(char*)&fd123);TRACE

	pub_base_sysinit();

	GetOr_br_noByBr_no(g_pub_tx.tx_br_no,cKinbrBakcode);/*根据机构号找行号*/

	memcpy(cOrderno	,fd123.orderno		,sizeof(cOrderno)-1);
	memcpy(cCmtno  	,fd123.cmtno  		,sizeof(cCmtno)-1);
	lWtdate=g_pub_tx.tx_date;	

	ret = Hv_zf_Sel(g_pub_tx.reply,&hv_zf, "orderno='%s' and cmtno='%s' and or_br_no='%s' and wt_date='%ld'",cOrderno,cKinbrBakcode,cKinbrBakcode,lWtdate);
	if ( ret==100 )
	{
		sprintf(acErrMsg,"无此记录");
		WRITEMSG
		goto	ErrExit;
	}
	else if ( ret )
	{
		sprintf(acErrMsg,"查询Hv_zf表出错![%d]",ret);
		WRITEMSG
		goto ErrExit;
	}	

	vtcp_log("得到123结构体的内容=[%s]",(char*)&fd123);TRACE

	set_zd_data("1230",(char *)&fd123);  /* 再把123返回给前台 */


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
