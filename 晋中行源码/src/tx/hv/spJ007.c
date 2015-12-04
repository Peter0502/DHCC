/***************************************************************
* 文 件 名:     spJ007.c
* 功能描述：    银行汇票签发			
*
* 作    者:   LiuYue
* 完成日期：  2006-8-17 21:08
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
int SetParamAll(char *My_IPAddress1,char *My_IPAdress2,int TimeOut);

spJ007()
{
	int ret = 0;
	char cPwd[11];
	char cAc_no[20];
	HV_PAYIN_AREA hv_payin;
	struct hv_poinfo_c hv_poinfo;
	HV_FD123_AREA fd123;
	memset(cAc_no, 0 , sizeof(cAc_no));
	char cVocNum[17] ;
	char cTel	[5];
	char cCash_Brno[13];
	
	memset(&hv_payin,  0x00 ,  sizeof(hv_payin));
	memset(&hv_poinfo, 0x00 ,  sizeof(hv_poinfo));
	memset(&fd123    , 0x00 ,  sizeof(fd123));
	memset(cPwd,0x00,sizeof(cPwd));
	memset(cVocNum,	0x00,	sizeof(cVocNum));
	memset(cTel,	0x00,	sizeof(cTel));
	memset(cCash_Brno,0x00,sizeof(cCash_Brno));


	/*** 初始化全局变量 ***/
	pub_base_sysinit();


	/************************ 
     *PART 1 获取到前台数据 *
     ************************/

	/***得到大额支付业务8583数据***/
	vtcp_log("+++++++++[%s]++++++[%d]\n",__FILE__,__LINE__);
	ret=iHvTistoPayin(&hv_payin);
	if( ret )
	{
		WRITEMSG
		strcpy( g_pub_tx.reply, "HV01");
		vtcp_log("+++++++++[%s]++++++[%d]\n",__FILE__,__LINE__);
		goto ErrExit;
	}
	get_zd_data("0880",cPwd);/***获取密押***/
	/*vtcp_log("[%s][%d]cpwd==[%s]\n",__FILE__,__LINE__,cPwd);*/
	get_fd_data("1230",(char *)&fd123);
	get_zd_data("0070",cTel);
	get_zd_data("0600",cVocNum);
	vtcp_log("[%s][%d]cVocNum===[%s]\n",__FILE__,__LINE__,cVocNum);
	

    /**********************
     *PART 2 基本判断 	  *
     **********************/	
	MEMCPY_DEBUG(hv_poinfo.po_no,fd123.po_no,sizeof(fd123.po_no));
	MEMCPY_DEBUG(hv_poinfo.pay_br_no,fd123.pay_opn_br_no,sizeof(hv_poinfo.pay_br_no)-1);
	ret = Hv_poinfo_Dec_Upd(g_pub_tx.reply,"po_no='%.8s'  and  pay_br_no='%.12s'",fd123.po_no,hv_poinfo.pay_br_no);
	if(ret)
	{
		sprintf(acErrMsg,"申明游标错误!ret=[%d]\n",ret);
		strcpy( g_pub_tx.reply, "HV01");
		WRITEMSG
		goto ErrExit;
	}
	ret = Hv_poinfo_Fet_Upd(&hv_poinfo,g_pub_tx.reply);
	if(ret&&ret!=100)
	{
		sprintf(acErrMsg,"取游标错误!ret=[%d]\n",ret);
		strcpy( g_pub_tx.reply, "HV01");
		WRITEMSG
		goto ErrExit;
	}
	else if(ret==100)
	{
		sprintf(acErrMsg,"查询的记录不存在!ret=[%d]\n",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P001");
		goto ErrExit;
	}
	/****判断数据合法性*******/
	if(hv_poinfo.po_sts[0]!=HV_HP_REG)
	{
		sprintf(acErrMsg,"汇票类型错误!\n");
		WRITEMSG 
		strcpy(g_pub_tx.reply,"P002");
		goto ErrExit;
	}
	if(hv_poinfo.bank_type[0]!='0')
	{
		sprintf(acErrMsg,"非本行的汇票不可以签发!\n");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P000");
		goto ErrExit;
	}

	/*****设置汇票密押**************/
	if(fd123.hp_add_pwd[0]=='2')/**手工编押**/
	{
		MEMCPY_DEBUG(hv_poinfo.pwd,cPwd,sizeof(hv_poinfo.pwd)-1);	
	}
	else if(fd123.hp_add_pwd[0]=='1')/**机器编押**/
	{
		char cPoind='0';
		char cPono[HV_POLEN+1];
		char cSigndate[9];
		char cTxamt[16];
		double dTxamt=0;
		char cInfo[61];
		memset(cInfo, 0 , sizeof(cInfo));
		memset(cPwd,0x00,sizeof(cPwd));
		/******处理一下兑付行，便于密押函数的调用*****/
		if(hv_poinfo.po_ind[0]=='2')
			MEMCPY_DEBUG(cCash_Brno,hv_poinfo.cash_br_no,sizeof(hv_poinfo.cash_br_no)-1);
		else
			MEMCPY_DEBUG(cCash_Brno,"000000000000",12);

		if(hv_poinfo.po_ind[0] == '0')
		{
			cPoind=2;
		}
		else if(hv_poinfo.po_ind[0] == '1')
		{
			cPoind=3;
		}
		else if(hv_poinfo.po_ind[0] == '2')
		{
			cPoind=1;
		}

		memset(cPono, 0 , sizeof(cPono));
		memset(cSigndate,0,sizeof(cSigndate));
		memset(cTxamt, 0 , sizeof(cTxamt));
		MEMCPY_DEBUG(cPono,fd123.po_no,sizeof(cPono)-1);
		MEMCPY_DEBUG(cSigndate,hv_payin.F_wt_date,8);
		dTxamt=hv_poinfo.po_bal*100;
		dbl2str(&dTxamt,0,sizeof(cTxamt)-1,0,cTxamt);
		/****调用密押函数***********/
		vtcp_log("%s,%d po_ind=[%d],wtdate=[%s]",__FILE__,__LINE__,cPoind,cSigndate);
		vtcp_log("%s,%d pono=[%s],txamt=[%s]",__FILE__,__LINE__,cPono,cTxamt);
		vtcp_log("%s,%d paybrno=[%s],cashbrno=[%s]",__FILE__,__LINE__,hv_poinfo.pay_br_no,cCash_Brno);
		vtcp_log("+++++开始机器编押++++++++++[%d]\n",__LINE__);
		ret = SetParamAll(HP_SERVERNAME1,HP_SERVERNAME2,TIME_OUT);
		if(ret)
		{
			vtcp_log("[%s][%d]设置网络参数失败!ret=[%d]\n",__FILE__,__LINE__,ret);
			strcpy(acErrMsg,"设置网络参数失败!");
			set_zd_data("0130",acErrMsg);
			WRITEMSG
			strcpy(g_pub_tx.reply,"AT08");
			goto ErrExit;
		}
		vtcp_log("+网络参数设置成功,向密押服务器进行加押处理+[%d]\n",__LINE__);
		ret = DraftEncrypt(0,cPoind,cSigndate,cPono,cTxamt,hv_poinfo.pay_br_no,cCash_Brno,cInfo,cPwd);
		if(ret)
		{
			if(ret==9001)
				sprintf(acErrMsg,"连接密押服务器进程异常[%d]",ret);
			if(ret==9002)
				sprintf(acErrMsg,"发动到密押服务器异常[%d]",ret);
			if(ret==9003)
				sprintf(acErrMsg,"接收密押服务器信息异常[%d]",ret);
			if(ret==9004)
				sprintf(acErrMsg,"加押错误[%d]",ret);
			if(ret==9005)
				sprintf(acErrMsg,"核押错误[%d]",ret);
			if(ret==9006)
				sprintf(acErrMsg,"核对签名错误[%d]",ret);
			if(ret==9007)
				sprintf(acErrMsg,"发报行标识错误[%d]",ret);
			if(ret==9008)
				sprintf(acErrMsg,"收报行标识错误[%d]",ret);
			if(ret==9009)
				sprintf(acErrMsg,"加密卡处理异常[%d]",ret);
			if(ret==9010)
				sprintf(acErrMsg,"数据报文错误[%d]",ret);
			if(ret==9011)
				sprintf(acErrMsg,"网络通信超时[%d]",ret);
			if(ret==9012)
				sprintf(acErrMsg,"发报行无编押权限[%d]",ret);
			set_zd_data("0130",acErrMsg);
			WRITEMSG
			strcpy(g_pub_tx.reply,"AT08");
			goto ErrExit;
		}
		
		vtcp_log("+++++++++[%s]++++++[%d]\n",__FILE__,__LINE__);
		/*vtcp_log("[%s,%d] +++++++pwd==[%s]++++++\n",__FILE__,__LINE__,cPwd);*/
		MEMCPY_DEBUG(hv_poinfo.pwd,cPwd,sizeof(hv_poinfo.pwd)-1);
		
	}

	/**************检测凭证********************/
		ret=pub_com_ChkNoteUse("165");/*检查凭证种类是否可用 汇票签发采用了165凭证*/
		if( ret )
		{
				sprintf(acErrMsg,"凭证处于停用状态!");
				strcpy( g_pub_tx.reply, "HV01");
				WRITEMSG
				goto ErrExit;
		}
		vtcp_log("[%s][%d] xier cVocunm==[%s],cTel==[%s]\n",__FILE__,__LINE__,cVocNum,cTel);
		vtcp_log("[%s][%d] xier cVocunm==[%s],cTel==[%s]\n",__FILE__,__LINE__,cVocNum,cTel);
		ret=pub_com_NoteUse(0,0,"165",cVocNum,cVocNum,cTel);/*凭证销号处理（交易过程中使用凭证）*/
		if( ret )
		{
				sprintf(acErrMsg,"柜员使用凭证错误!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"N007");
				goto ErrExit;
		}

	/*vtcp_log("hv_poinfo.pwd===[%s]\n",hv_poinfo.pwd);
	vtcp_log("hv_poinfo.pwd===[%s]\n",hv_poinfo.pwd);
	vtcp_log("hv_poinfo.pwd===[%s]\n",hv_poinfo.pwd);
	vtcp_log("hv_poinfo.pwd===[%s]\n",hv_poinfo.pwd);
	vtcp_log("hv_poinfo.pwd===[%s]\n",hv_poinfo.pwd);*/

	/*******设置当日交易次数***/
	if(hv_poinfo.l_tx_date==apatoi(hv_payin.F_wt_date,sizeof(hv_payin.F_wt_date)))
	{
		hv_poinfo.td_cnt = hv_poinfo.td_cnt+1; /**是否当天**/
	}
	else 
		hv_poinfo.td_cnt = 1;
	/********设置最近一次交易的日期****/
	hv_poinfo.l_tx_date = apatoi(hv_payin.F_wt_date,sizeof(hv_payin.F_wt_date));
	/********设置汇票上一次状态*****/
	hv_poinfo.l_po_sts[0] = hv_poinfo.po_sts[0];
	/*********设置汇票的当前状态****/
	hv_poinfo.po_sts[0]   = HV_HP_SIGN;
	/*********设置加押方式**********/
	hv_poinfo.add_pwd_ind[0] = fd123.hp_add_pwd[0];
	/*********设置签发日期**********/
	hv_poinfo.sign_date = apatoi(hv_payin.F_wt_date,sizeof(hv_payin.F_wt_date));
	/*******写入附言*****************/
	MEMCPY_DEBUG(hv_poinfo.brf,fd123.brf,sizeof(hv_poinfo.brf)-1);

     	/***********************
     	*PART 3   操作数据表  *
     	***********************/	
	/******现在开始更新那些字段吧*************************/		
	ret = Hv_poinfo_Upd_Upd(hv_poinfo,g_pub_tx.reply);
	if(ret)
	{
		sprintf(acErrMsg,"执行函数hv_poinfo_upd_upd错误!ret==[%d]\n",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D105");
		return (-1);
	}
	 
	/*******************
     * PART 4 结尾工作 *
     ******************/

	/* 写入业务流水 */
	MEMCPY_DEBUG(g_pub_tx.beg_note_no,hv_poinfo.po_no,sizeof(g_pub_tx.beg_note_no));
	ret=pub_ins_trace_log();
	if(ret)
	{
		sprintf(acErrMsg,"登记交易流水出错!");
		WRITEMSG
		goto ErrExit;
	}
	/* 设置2440401 帐号 */
	ret=pub_base_GetParm_Str(g_pub_tx.tx_br_no,PARM_HPQF,cAc_no);
	if(ret)
	{
		sprintf(acErrMsg,"得到本机构大额签发帐号错误!\n");
		WRITEMSG
		vtcp_log("%s,%d 得到本机构大额签发帐号错误\n",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"P005");
		goto ErrExit;
	}
	set_zd_data( "1211", cAc_no );
	set_zd_data( "1214", "01" );
	set_zd_data( "1215", "2" );
	set_zd_double( "1218", hv_poinfo.po_bal);
	set_zd_data( "121A", "大额汇票签发" );

	/*** 输出变量 ***/

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	/*vtcp_log("!!!!!!![%s][%d]cPWd===[%s]\n",__FILE__,__LINE__,cPwd);*/
	set_zd_data("0880",cPwd);
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

