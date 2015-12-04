/***************************************************************
* 文 件 名:     spJ016.c
* 功能描述：    汇票加/核押 
*
* 作    者:   LiuYue
* 完成日期：  2006-8-26  18:56
* 描述:       根据几个要素生成一个密押并进行判断
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "hvpack.h"
#include "hv_define.h"
#include "hvnet.h"
#include "hv_pub.h"
#include "find_debug.h"

/**********************************
*挂失日期放到原委托日期字段       *
*汇票号码 用文件传递			  *
*在这里调用一个短通讯             *
***********************************/
struct HP_PASSWD{
	char  type	[1+1];	/*处理类型 0:加押 1:核押*/
	char po_type	[1+1];	/*1.现金汇票 2.可转让汇票 3.不可转让汇票 4.查复书业务*/
	char sign_date	[8+1];	/*汇票签发日期*/
	char po_no	[8+1];	/*汇票号码*/
	char tx_amt	[15+1];	/*金额,以分为单位,不足15位前补0*/
	char pay_brno	[12+1];	/*签发行行号*/
	char cash_brno	[12+1];	/*兑付行行号,若无填全0*/
	char cr_txnum	[8+1];	/*前8位为查复书编号,不足前补0,其余全填0,非查复书业务填全0*/
	char pwd	[10+1];	/*编押时返回得到的密押,核押时为要验证的汇票密押*/
}hp_passwd;
spJ016()
{
	int ret	= 0;
	HV_FD123_AREA	   fd123;
	char cOtype	[2];
	double dTxamt = 0.0;
	char cHptype;
	char cOtherInfo[61];
	char cQry_num[9];
	vtcp_log("[%s][%d]开始汇票核押/编押程序!\n",__FILE__,__LINE__);
	
	memset(&hp_passwd,0x00,sizeof(hp_passwd));
	memset(&fd123,0x00,sizeof(fd123));
	memset(cOtype,0x00,sizeof(cOtype));
	memset(cOtherInfo, 0,sizeof(cOtherInfo));
	memset(cQry_num,0x00,sizeof(cQry_num));
	memcpy(cQry_num,"00000000",8);
 	
	/*** 初始化全局变量 ***/
	pub_base_sysinit();
   	 /*****************************
     	 *PART 1   得到前台数据      *
     	 *****************************/
	
	get_fd_data("1230",(char *)&fd123);/**获取123域里面内容**/
	vtcp_log("[%s][%d]fd123===[%s]\n",__FILE__,__LINE__,(char *)&fd123);
	get_zd_data("0720",hp_passwd.type);   /*操作类型**/
	get_zd_data("0720",cOtype);   /*操作类型**/
	vtcp_log("%s,%d, optype===[%c] ",__FILE__,__LINE__,cOtype[0]);
	
	hp_passwd.po_type[0] = fd123.po_type[0];
	get_zd_data("0460",hp_passwd.sign_date);
	MEMCPY_DEBUG(hp_passwd.po_no,fd123.po_no,sizeof(fd123.po_no));
	get_fd_data("0390",hp_passwd.tx_amt);
	vtcp_log("[%s][%d]hp_passwd.tx_amt===[%s]\n",__FILE__,__LINE__,hp_passwd.tx_amt);
	str2dbl(hp_passwd.tx_amt,strlen(hp_passwd.tx_amt),0,&dTxamt);
	vtcp_log("[%s][%d]dTxmat===[%lf]\n",__FILE__,__LINE__,dTxamt);
	sprintf(hp_passwd.tx_amt,"%015.0f",dTxamt*100);
	MEMCPY_DEBUG(hp_passwd.pay_brno,fd123.pay_opn_br_no,sizeof(fd123.pay_opn_br_no));
	
	/**编押/核押字段***/
	if(hp_passwd.type[0]=='1')/**核押**/
	{
		get_zd_data("0880",hp_passwd.pwd);
	}
	
	/**汇票类型***/
	if(hp_passwd.po_type[0]=='1' || hp_passwd.po_type[0]=='5')/**现金汇票 划款凭证时.也用此变量**/
	{
		MEMCPY_DEBUG(hp_passwd.cash_brno,fd123.cash_opn_br_no,sizeof(fd123.cash_opn_br_no));
	}
	else 
	{
		memset(hp_passwd.cash_brno,'0',12);
	}
	/**是否查复书***/
	if(hp_passwd.po_type[0]=='4')/**查复书**/
	{
		MEMCPY_DEBUG(hp_passwd.cr_txnum,fd123.cr_tx_num,sizeof(fd123.cr_tx_num));
	}
	else 
		memset(hp_passwd.cr_txnum,'0',sizeof(hp_passwd.cr_txnum)-1);
	 
	
	vtcp_log("[%s][%d]获取到了前台数据\n",__FILE__,__LINE__);
	vtcp_log("[%s][%d]hp_passwd===[%s]\n",__FILE__,__LINE__,(char *)&hp_passwd);
	

	/**********************
         *PART 2   基本检测   *
	 **********************/
	  
	ret = SetParamAll(HP_SERVERNAME1,HP_SERVERNAME2,TIME_OUT);
	if(ret)
	{
	 	sprintf(acErrMsg,"设置网络参数错误!\n");
		set_zd_data("0130",acErrMsg);
	 	WRITEMSG 
	 	strcpy(g_pub_tx.reply,"P013");
	 	goto ErrExit;
	}
	vtcp_log("%s,%d 操作类型=[%d]",__FILE__,__LINE__,cOtype[0]-'0');
	vtcp_log("%s,%d signdate=[%s]",__FILE__,__LINE__,hp_passwd.sign_date);
	vtcp_log("%s,%d pono=[%s]",__FILE__,__LINE__,hp_passwd.po_no);
	vtcp_log("%s,%d amt=[%s]",__FILE__,__LINE__,hp_passwd.tx_amt);
	vtcp_log("%s,%d paybrno=[%s]",__FILE__,__LINE__,hp_passwd.pay_brno);
	vtcp_log("%s,%d cashbrno=[%s]",__FILE__,__LINE__,hp_passwd.cash_brno);
	vtcp_log("%s,%d cr_txnum=[%s]",__FILE__,__LINE__,hp_passwd.cr_txnum);
	/*vtcp_log("%s,%d pwd=[%s]",__FILE__,__LINE__,hp_passwd.pwd);*/
	vtcp_log("%s,%d po_type=[%c]",__FILE__,__LINE__,hp_passwd.po_type[0]);
	if(hp_passwd.po_type[0]=='4') cHptype=4;
	if(hp_passwd.po_type[0]=='1') cHptype=1;
	if(hp_passwd.po_type[0]=='2') cHptype=3;
	if(hp_passwd.po_type[0]=='3') cHptype=2;
	if(hp_passwd.po_type[0]=='5') cHptype=5;/**add by zyl 20110705 划款凭证**/
	vtcp_log("%s,%d hptype=[%d]",__FILE__,__LINE__,cHptype);


	memset(cOtherInfo, 0 , sizeof(cOtherInfo));
	memcpy(cQry_num,hp_passwd.cr_txnum,8);
    cOtherInfo[0]=((cQry_num[0]&0x0F)<<4)+(cQry_num[1]&0x0F);
    cOtherInfo[1]=((cQry_num[2]&0x0F)<<4)+(cQry_num[3]&0x0F);
    cOtherInfo[2]=((cQry_num[4]&0x0F)<<4)+(cQry_num[5]&0x0F);
    cOtherInfo[3]=((cQry_num[6]&0x0F)<<4)+(cQry_num[7]&0x0F);

    vtcp_log("[%s][%d]cOtherInfo=[%x]",__FILE__,__LINE__,cOtherInfo[0]);
    vtcp_log("[%x]",cOtherInfo[1]);
    vtcp_log("[%x]",cOtherInfo[2]);
    vtcp_log("[%x]\n",cOtherInfo[3]);

	vtcp_log("[%s][%d] 别人的cOtherInfo===[%s]\n",__FILE__,__LINE__,&cOtherInfo[0]);
	/****add by liuyue 20070209****/
	memset(cOtherInfo, 0 , sizeof(cOtherInfo));
	cOtherInfo[0]=(char) (  (cQry_num[0]-'0')*16+(cQry_num[1]-'0') );
	cOtherInfo[1]=(char) (  (cQry_num[2]-'0')*16+(cQry_num[3]-'0') );
	cOtherInfo[2]=(char) (  (cQry_num[4]-'0')*16+(cQry_num[5]-'0') );
	cOtherInfo[3]=(char) (  (cQry_num[6]-'0')*16+(cQry_num[7]-'0') );
	/**************add end************/
    vtcp_log("[%s][%d]cOtherInfo=[%x]",__FILE__,__LINE__,cOtherInfo[0]);
    vtcp_log("[%x]",cOtherInfo[1]);
    vtcp_log("[%x]",cOtherInfo[2]);
    vtcp_log("[%x]\n",cOtherInfo[3]);
	vtcp_log("[%s][%d]cOtherInfo===[%s]\n",__FILE__,__LINE__,&cOtherInfo[0]);
		
	ret = DraftEncrypt(cOtype[0]-'0',cHptype,hp_passwd.sign_date,
		hp_passwd.po_no,hp_passwd.tx_amt,hp_passwd.pay_brno,hp_passwd.cash_brno,
		cOtherInfo,hp_passwd.pwd);
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
	/*vtcp_log("_______cPwd____[%s]_____\n",hp_passwd.pwd);*/
	
	

	/*********************
	 *PART 3  操作数据库 *
         *********************/
	
	 

	/*******************
         *PART 4  结尾工作 *
         *******************/	
	ret=pub_ins_trace_log();
	if(ret)
	{
		sprintf(acErrMsg,"登记交易流水出错!");
		WRITEMSG
		goto ErrExit;
	}
	/*** 输出变量 ***/

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	if(hp_passwd.type[0] =='0')
		set_zd_data("0880",hp_passwd.pwd);/***生成的密押**/
	else 
		set_zd_data("0880","成功!");/***返回成功***/
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
