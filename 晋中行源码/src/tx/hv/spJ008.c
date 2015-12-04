/***************************************************************
* 文 件 名:     spJ008.c
* 功能描述：    汇票资金移存入帐			
*
* 作    者:   LiuHuafeng 
* 完成日期：  2006-9-5 15:58
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "hvpack.h"
#include "hv_define.h"
#include "hv_poinfo_c.h"
#include "find_debug.h"


int DraftEncrypt(int TransactType,char YwType,char* Date,char* SerialNo,char* Amount,char* SendNode,char* RecvNode,char* OtherInfo,char* CipherValue);
int SetParmAll(char *My_IPAddress1,char *My_IPAdress2,int TimeOut);

spJ008()
{
	int ret = 0;
	char cPwd[11];
	HV_PAYIN_AREA hv_payin;
	struct hv_poinfo_c hv_poinfo;
	HV_FD123_AREA fd123;
	char cAc_no[20];
	struct hv_poinfo_c s_hv_poinfo;
	HV_121_ADD hv_121_add;
	char cBuff[500];

	/*** 初始化全局变量 ***/
	pub_base_sysinit();
	memset(&hv_payin,  0x00 ,  sizeof(hv_payin));
	memset(&hv_poinfo, 0x00 ,  sizeof(hv_poinfo));
	memset(&fd123    , 0x00 ,  sizeof(fd123));
	memset(cPwd,0x00,sizeof(cPwd));
	memset(cAc_no,	0x00,	sizeof(cAc_no));
	memset(&hv_121_add,0x00,sizeof(hv_121_add));
	memset(&s_hv_poinfo,0x00,sizeof(s_hv_poinfo));
	memset(cBuff,0x00,sizeof(cBuff));

	get_fd_data("1230",(char *)&fd123);
	/************************ 
	*PART 1 获取到前台数据  *
	************************/

	/***得到大额支付业务8583数据***/
	vtcp_log("+++++++++[%s]++++++[%d]\n",__FILE__,__LINE__);
	ret=iHvTistoPayin(&hv_payin);
	if( ret )
	{
		WRITEMSG
		vtcp_log("+++++++++[%s]++++++[%d]\n",__FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01");
		goto ErrExit;
	}
	/*vtcp_log("[%s][%d]cpwd==[%s]\n",__FILE__,__LINE__,cPwd);*/
	
	ret=iHv_checkstat(hv_payin.F_cmtno,hv_payin.F_yw_type);
	if(ret)
	{
		set_zd_data("0130",acErrMsg);
		MEMCPY_DEBUG(g_pub_tx.reply,"T042",4);
		WRITEMSG
		goto ErrExit;
	}

	/*********************************
	 *121 移存交易 附加域处理放到这  *
     *********************************/
	 vtcp_log("[%s][%d]输入的汇票号码为[%s],签发行=[%s]\n",__FILE__,__LINE__,fd123.po_no,hv_payin.F_or_br_no);
	 ret = Hv_poinfo_Sel(g_pub_tx.reply,&s_hv_poinfo,"po_no='%.8s' and pay_br_no='%.12s'",fd123.po_no,hv_payin.F_or_br_no);
	 if(ret)
	 {
		MEMCPY_DEBUG(g_pub_tx.reply,"HV01",4);
		set_zd_data("0130","该汇票不存在或者不是你行签发!");
		vtcp_log("[%s][%d]汇票号不存在!【%.8s],[%.12s]\n",__FILE__,__LINE__,fd123.po_no,hv_payin.F_or_br_no);
		goto ErrExit;
	 }
	 if(s_hv_poinfo.po_ind[0]=='2' && strlen(s_hv_poinfo.cash_br_no)!=0) /**如果是现金汇票  需要填写兑付行***/
		MEMCPY_DEBUG(hv_121_add.spec_br_no,s_hv_poinfo.cash_br_no,12);	
	 else
		MEMCPY_DEBUG(hv_121_add.spec_br_no,"            ",12);	
	 sprintf(hv_121_add.pj_date,"%8d",s_hv_poinfo.sign_date);	/*签发日期**/
	 MEMCPY_DEBUG(hv_121_add.pj_num,s_hv_poinfo.po_no,8);		/*票据号码**/
	 hv_121_add.hp_type[0] = s_hv_poinfo.po_ind[0];				/*汇票类型**/
	 MEMCPY_DEBUG(hv_121_add.hp_miya,s_hv_poinfo.pwd,10);		/*汇票密押**/
		
     MEMCPY_DEBUG(cBuff,(char *)&hv_121_add,sizeof(hv_121_add));
     ret=iHvSetAddit(hv_payin.F_tx_date,hv_payin.F_wt_date,hv_payin.T_addid,"000",cBuff,strlen(cBuff));
	 if (ret)
	 {
			sprintf( acErrMsg,"写入附加域错误![%s][%d][%d]",__FILE__,__LINE__,ret);
			set_zd_data(DC_GET_MSG,"写入附加域错误");
			WRITEMSG
			return -1;
	 }

	 vtcp_log("[%s][%d]写附加域成功buf===[%s]\n",__FILE__,__LINE__,cBuff);

	 
	ret=iHvFrontToTable(&hv_payin);
	if(ret)
	{
		set_zd_data("0130",acErrMsg);
		MEMCPY_DEBUG(g_pub_tx.reply,"T042",4);
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("%s,%d ",__FILE__,__LINE__);
	ret=iHv121Proc(&hv_payin);
	if(ret)
	{
		set_zd_data("0130",acErrMsg);
		strcpy( g_pub_tx.reply, "HV01");
		WRITEMSG
		goto ErrExit;
	}
	
	ret=iHvUpdZfRecFromPayin(&hv_payin,1);
	if( ret )
	{
		strcpy( g_pub_tx.reply, "HV01");
		WRITEMSG
		goto ErrExit;
	}
	iHvReset8583(&hv_payin);

	ret=pub_base_GetParm_Str(g_pub_tx.tx_br_no,PARM_HPYC,cAc_no);
	if(ret)
	{
		vtcp_log("[%s][%d]获取汇票移存帐号错误!\n",__FILE__,__LINE__);
		set_zd_data("0130","获取汇票移存帐号错误");
		strcpy( g_pub_tx.reply, "HV01");
		goto ErrExit;
	}
	set_zd_data("0300",cAc_no);
	set_zd_data("0680","6");
	ret=pub_base_GetParm_Str(g_pub_tx.tx_br_no,PARM_HVQS,cAc_no);
	if(ret)
	{
		vtcp_log("[%s][%d]获取汇票移存帐号错误!\n",__FILE__,__LINE__);
		set_zd_data("0130","获取汇票移存帐号错误");
		goto ErrExit;
	}
	set_zd_data("0310",cAc_no);
	set_zd_data("0680","6");


	/* 写入业务流水 */
	ret=pub_ins_trace_log();
	if(ret)
	{
		sprintf(acErrMsg,"登记交易流水出错!");
		strcpy( g_pub_tx.reply, "S023");
		WRITEMSG
		goto ErrExit;
	}
	/*** 输出变量 ***/

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
        if(memcmp(g_pub_tx.reply,"0000",4)==0)
                MEMCPY_DEBUG(g_pub_tx.reply,"T063",4);
	sprintf(acErrMsg,"失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
