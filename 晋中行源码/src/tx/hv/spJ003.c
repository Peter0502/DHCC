/***************************************************************
* 文 件 名:     spJ003.c
* 功能描述：	大额支付退汇业务录入
*
* 作    者:   LiuHuafeng
* 完成日期：  2006-8-9 9:28
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "hvpack.h"
#include "hv_zf_c.h"
#include "find_debug.h"

spJ003()
{
	int ret = 0;
	HV_PAYIN_AREA hv_payin,hv_th_payin;
	struct hv_zf_c o_hv_zf;
	memset(&o_hv_zf,0x00,sizeof(o_hv_zf));
	
	/*** 初始化全局变量 ***/
	pub_base_sysinit();
	memset(&hv_th_payin, 0 , sizeof(hv_th_payin));
	memset(&hv_payin, 0 , sizeof(hv_payin));
	/***得到大额支付业务8583数据***/
	ret=iHvTistoPayin(&hv_payin);
	if( ret )
	{
		sprintf(acErrMsg,"调用函数iHvTistoPayin失败!\n");
		WRITEMSG
		strcpy( g_pub_tx.reply, "HV01");
		set_zd_data(DC_GET_MSG,"调用函数iHvTistoPayin失败");
		goto ErrExit;
	}
	vtcp_log("%s,%d 输入的交易金额[%.15s]",__FILE__,__LINE__,hv_payin.F_tx_amt);
	/* 准备写入登记簿的数据 */
	ret=iHvFrontToTable(&hv_payin);
	if( ret )
	{
		sprintf(acErrMsg,"调用函数iHvFrontToTable失败!\n");
		WRITEMSG
		strcpy( g_pub_tx.reply, "HV01");
		set_zd_data(DC_GET_MSG,"调用函数iHvFrontToTable失败");
		goto ErrExit;
	}
	vtcp_log("%s,%d 输入的交易金额[%.15s]",__FILE__,__LINE__,hv_payin.F_tx_amt);
	/* 检查非法字符 */
	/*****20070119 liuyue  退汇不检查输入半角检测
	ret=iHvPayin_characterCheck(&hv_payin,acErrMsg);
	if(ret)
	{
		vtcp_log("[%s][%d]调用函数iHvPayin_characterCheck失败!\n",__FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "T042");
		set_zd_data(DC_GET_MSG,acErrMsg);
		goto ErrExit;
	}
	************************************/

	ret = iCharacterType(hv_payin.F_hv_brf,sizeof(hv_payin.F_hv_brf),'g');
	if(ret)
	{
		vtcp_log("%s,%d,输入的附言中[%.60s]第%d位存在非法字符",__FILE__,__LINE__,hv_payin.F_hv_brf,ret);
		strncpy(acErrMsg,"输入的附言中存在非法字符",50);
		strcpy( g_pub_tx.reply, "T042");
		set_zd_data(DC_GET_MSG,acErrMsg);
		goto ErrExit;
	}
	vtcp_log("[%s][%d]oper==[%c],cmtno==[%s]\n",__FILE__,__LINE__,hv_payin.F_operlevel[0],hv_payin.F_cmtno);
	if(hv_payin.F_operlevel[0]=='0' &&
		 (!memcmp(hv_payin.F_cmtno,"100",sizeof(hv_payin.F_cmtno))
		 ||!memcmp(hv_payin.F_cmtno,"101",sizeof(hv_payin.F_cmtno))
		 ||!memcmp(hv_payin.F_cmtno,"102",sizeof(hv_payin.F_cmtno))
	     ||!memcmp(hv_payin.F_cmtno,"103",sizeof(hv_payin.F_cmtno))
	     ||!memcmp(hv_payin.F_cmtno,"105",sizeof(hv_payin.F_cmtno))
	     ||!memcmp(hv_payin.F_cmtno,"108",sizeof(hv_payin.F_cmtno))
        )
    )
    {
        char cInputTxamt[17];
        double dInputTxamt=0.0;
        double dMinTxamt=0.0;
        dMinTxamt=dGetHvMinTxamt();
        memset(cInputTxamt , 0 ,sizeof(cInputTxamt));
        memcpy(cInputTxamt, hv_payin.F_tx_amt,sizeof(cInputTxamt)-1);
        zip_space(cInputTxamt);
        str2dbl(cInputTxamt,strlen(cInputTxamt),0,&dInputTxamt);
            vtcp_log("%s,%d 输入的金额=[%f], 最小金额=[%f] ",__FILE__,__LINE__,dInputTxamt , dMinTxamt);
        if(dInputTxamt < 20000.00)
        {
            vtcp_log("%s,%d 输入的金额=[%f], 最小金额=[%f] ",__FILE__,__LINE__,dInputTxamt , dMinTxamt);
            vtcp_log("%s,%d 请选择加急或特级优先级别",__FILE__,__LINE__);
			strcpy( g_pub_tx.reply, "HV01");
			set_zd_data(DC_GET_MSG,"请选择加急或特级优先级别");
			goto ErrExit;
            WRITEMSG
        }
    }

	vtcp_log("%s,%d 输入的交易金额[%.15s]",__FILE__,__LINE__,hv_payin.F_tx_amt);
	ret=iHv_checkstat(hv_payin.F_cmtno,hv_payin.F_yw_type);
	if(ret)
	{
		sprintf(acErrMsg,"调用函数iHv_checkstat失败!\n");
		WRITEMSG
		strcpy( g_pub_tx.reply, "T042");
		set_zd_data(DC_GET_MSG,"大额系统日期非法/状态不正常");
		goto ErrExit;
	}
	vtcp_log("%s,%d 输入的交易金额[%.15s]",__FILE__,__LINE__,hv_payin.F_tx_amt);
	/* 得到原来来帐信息 */
	
	/*****add by liuyue 20061219 改变原来赖账的o_sts字段**/
	ret = Hv_zf_Dec_Upd(g_pub_tx.reply,"orderno='%.8s' and wt_date='%.8s' and or_br_no='%.12s' ",
			hv_payin.F_o_orderno,hv_payin.F_o_wt_date,hv_payin.F_ac_br_no);
	if(ret)
	{
		vtcp_log("[%s][%d]数据库操作错误!\n",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"D103");
		set_zd_data("0130","数据库操作错误!");
		goto ErrExit;
	}
	ret = Hv_zf_Fet_Upd(&o_hv_zf,g_pub_tx.reply);
	if(ret &&ret!=100)
	{
		vtcp_log("[%s][%d]数据库操作错误!\n",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"D103");
		set_zd_data("0130","数据库操作错误!");
		goto ErrExit;
	}
	else if(ret==100)
	{
		vtcp_log("[%s][%d]锁定原来帐交易错误!ret=[%d]\n",__FILE__,__LINE__,ret);
		strcpy(g_pub_tx.reply,"HV01");
		set_zd_data("0130","锁定原支付来帐错误");
		goto ErrExit;
	}

	/***更新 记录 ***/
	o_hv_zf.hv_osts[0] = o_hv_zf.hv_sts[0];

	ret = Hv_zf_Upd_Upd(o_hv_zf,g_pub_tx.reply);
	if(ret)
	{
		vtcp_log("[%s][%d]更新o_sts 错误!\n",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"D103");
		set_zd_data("0130","数据库操作错误!");
		goto ErrExit;
	}
	
	Hv_zf_Clo_Upd();

	/****add end by liuyue 20061219****/
	ret=iHvGetThMsg(&hv_payin,&hv_th_payin,acErrMsg);
	if(ret)
	{
		vtcp_log("调用函数iHvGetThMsg失败!\n",__FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "T042");
		set_zd_data(DC_GET_MSG,acErrMsg);
		goto ErrExit;
	}
	vtcp_log("%s,%d 输入的交易金额[%.15s]",__FILE__,__LINE__,hv_payin.F_tx_amt);
	/* 比较输入信息与原来帐信息是否一致 */
	ret=iHvCheckTh_input(&hv_payin,&hv_th_payin,acErrMsg);
	if(ret)
	{
		vtcp_log("[%s][%d]调用函数iHvCheckTh_input失败!\n",__FILE__,__LINE__);
		WRITEMSG
		strcpy( g_pub_tx.reply, "T042");
		set_zd_data(DC_GET_MSG,acErrMsg);
		goto ErrExit;
	}
	/* 写入登记簿 */
	ret=iHvUpdZfRecFromPayin(&hv_payin,1);
	if( ret )
	{
		sprintf(acErrMsg,"调用函数iHvCheckTh_input失败!\n");
		WRITEMSG
		strcpy( g_pub_tx.reply, "HV01");
		set_zd_data(DC_GET_MSG,"调用函数iHvCheckTh_input失败");
		goto ErrExit;
	}

	iHvReset8583(&hv_payin);
	vtcp_log("%s,%d new orderno===[%s][%s]",__FILE__,__LINE__,hv_payin.F_orderno,hv_payin.T_orderno);
	/* 写入业务流水 */

	/*
	vtcp_log("[%s][%d]trace_no=[%d]\n",__FILE__,__LINE__,g_pub_tx.trace_no);
	vtcp_log("[%s][%d]tx_date=[%d]\n",__FILE__,__LINE__,g_pub_tx.tx_date);
	vtcp_log("[%s][%d]tx_time=[%d]\n",__FILE__,__LINE__,g_pub_tx.tx_time);
	vtcp_log("[%s][%d]tx_br_no=[%s]\n",__FILE__,__LINE__,g_pub_tx.tx_br_no);
	vtcp_log("[%s][%d]opn_br_no=[%s]\n",__FILE__,__LINE__,g_pub_tx.opn_br_no);
	vtcp_log("[%s][%d]tx_code=[%s]\n",__FILE__,__LINE__,g_pub_tx.tx_code);
	vtcp_log("[%s][%d]sub_tx_code=[%s]\n",__FILE__,__LINE__,g_pub_tx.sub_tx_code);
	vtcp_log("[%s][%d]trace_no=[%s]\n",__FILE__,__LINE__,g_pub_tx.trace_no);
	vtcp_log("[%s][%d]trace_no=[%s]\n",__FILE__,__LINE__,g_pub_tx.trace_no);
	vtcp_log("[%s][%d]trace_no=[%s]\n",__FILE__,__LINE__,g_pub_tx.trace_no);
	vtcp_log("[%s][%d]trace_no=[%s]\n",__FILE__,__LINE__,g_pub_tx.trace_no);
	vtcp_log("[%s][%d]trace_no=[%s]\n",__FILE__,__LINE__,g_pub_tx.trace_no);*/
	ret=pub_ins_trace_log();
	if(ret)
	{
		sprintf(acErrMsg,"登记交易流水出错!");
		strcpy( g_pub_tx.reply, "HV01");
		set_zd_data(DC_GET_MSG,"登记交易流水出错");
		WRITEMSG
		goto ErrExit;
	}
	/*** 输出变量 ***/
	sprintf(acErrMsg,"ac_no[%s]",g_pub_tx.ac_no);
	WRITEMSG
	set_zd_data("112A",g_pub_tx.ac_no);

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
