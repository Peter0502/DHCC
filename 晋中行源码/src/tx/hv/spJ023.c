/*************************************************
* 文 件 名:  spJ023.c
* 功能描述： 自由格式报文录入,此交易为交易行向某行查询希望知道的信息
* 作    者:  xyy 
* 完成日期： 2006年8月18日
*
* 修改记录： 
* 日    期:
* 修 改 人:SSH
* 修改内容:录入的同时发送
*************************************************/
#define EXTERN
#include <string.h>
#include <stdio.h>
#include "hv_define.h"
#include "hvpack.h"
#define loc_t sys_loc_t
#include "find_debug.h"

int iGetHvAddid(char * addid);
int spJ023()
{
	int  	iOrderno=0;
	int     flag=0;
	char  	cTxbank[13];
	char  	cFlag[2];        
	char 	cBuff	[2000];
	char    Cashno   [13];  /** 接收行行号 **/
	char    cContent [512];
	HV_NOTPAY_AREA hv_notpay;
	THVPACKAGE pack303;
	int ret=0;	
	HV_FD123_AREA fd123;
	
	struct hv_fzf_c hv_fzf;
	struct hv_uniontab_c hv_uniontab;
	memset(cTxbank		,0,sizeof(cTxbank));
	memset(cFlag		,0,sizeof(cFlag));
	memset(cBuff		,0,sizeof(cBuff));
	memset(&hv_fzf		,0,sizeof(hv_fzf));
	memset(&hv_uniontab	,0,sizeof(hv_uniontab));
	memset(&hv_notpay, 0 , sizeof(hv_notpay));
   	memset(Cashno		,0, sizeof(Cashno ) );
	memset(cContent,	0x00,	sizeof(cContent));

	memset(&fd123,0x00,sizeof(fd123));

	memset(&pack303, 0 , sizeof(pack303));
	get_zd_data("0680",cFlag);	/*选择标识，0-接收行号、1-收报中心*/

	/*** 初始化全局变量 ***/
	pub_base_sysinit();
	vtcp_log("[%s][%d]F_orderno=[%s]T_orderno=[%s]",__FILE__,__LINE__,hv_notpay.F_orderno,hv_notpay.T_orderno);

	/***得到大额支付业务8583数据***/
	vtcp_log("[%s][%d]自由格式程序开始喽spJ023",__FILE__,__LINE__);

	g_reply_int = iHvTistoNotpay(&hv_notpay);
	vtcp_log("[%s][%d]T_orderno=[%s] ",__FILE__,__LINE__,hv_notpay.T_orderno);
	if( g_reply_int )
	{
		sprintf( acErrMsg,"调用函数iHvTistoNotpay出错,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "P015" );
		WRITEMSG
		goto ErrExit;
	}

	vtcp_log("[%s][%d]调用完iHvTistoNotpay函数",__FILE__,__LINE__);
	vtcp_log("[%s][%d]F_orderno=[%s]T_orderno=[%s]",__FILE__,__LINE__,hv_notpay.F_orderno,hv_notpay.T_orderno);

	/******检查营业的状态******/ 

	g_reply_int = iHv_checkstat(hv_notpay.F_cmtno);
	if( g_reply_int )
	{
		sprintf( acErrMsg,"工作状态不正常![%s][%d][%s]",__FILE__,__LINE__,g_reply_int);
		strcpy( g_pub_tx.reply, "HV01");
		set_zd_data(DC_GET_MSG,"大额支付系统当前状态无法做此交易");
		WRITEMSG
		goto ErrExit;
	}	
	vtcp_log("[%s][%d]调用完iHv_checkstat",__FILE__,__LINE__);
	vtcp_log("[%s][%d]F_orderno=[%s]T_orderno=[%s]",__FILE__,__LINE__,hv_notpay.F_orderno,hv_notpay.T_orderno);


	/** 校验发起、接收行号是否存在 **/
	memcpy(Cashno,hv_notpay.F_ac_br_no, sizeof(Cashno) -1 );
	
	if(cFlag[0]=='0')
	{
		if(strlen(Cashno)==0)
		{
			sprintf( acErrMsg,"接收行行号不能为空,[%s][%d]", __FILE__,__LINE__);
			strcpy( g_pub_tx.reply, "HV01");
			set_zd_data(DC_GET_MSG,"接收行行号不能为空");
			WRITEMSG
			goto ErrExit;
		}
	}
       
       
	GetOr_br_noByBr_no(hv_notpay.F_tx_br_no,cTxbank);/*根据机构号找行号*/
	vtcp_log("[%s][%d]T_orderno=[%s] ",__FILE__,__LINE__,hv_notpay.T_orderno);

	/** 接收行 **/
	if(cFlag[0]=='0') {
		g_reply_int = Hv_uniontab_Sel(g_pub_tx.reply,&hv_uniontab,"or_br_no ='%s' ",Cashno);       	
		if (g_reply_int)
		{
			sprintf( acErrMsg,"锁定接收行出错,[%s][%d]", __FILE__,__LINE__);
			strcpy( g_pub_tx.reply, "HV01");
			set_zd_data(DC_GET_MSG,"锁定接收行出错");
			WRITEMSG
			goto ErrExit;
		}
	        
		if(hv_uniontab.br_sts[0]!= '1') {
			sprintf( acErrMsg,"接收行状态错,[%s][%d]", __FILE__,__LINE__);
			WRITEMSG
			strcpy( g_pub_tx.reply, "HV01");
			set_zd_data(DC_GET_MSG,"接收行状态错");
			goto ErrExit;
		}
		if(memcmp(hv_uniontab.qs_no, HV_QSBRNO,sizeof(cTxbank) -1 ) == 0) {
			sprintf(acErrMsg,"接收行[%.12s]与发起行[%.12s]不能同属一个清算行[%s][%d]!",hv_uniontab.qs_no,HV_QSBRNO,__FILE__,__LINE__);
			WRITEMSG
			strcpy( g_pub_tx.reply, "HV01");
			set_zd_data(DC_GET_MSG,"接收行与发起行不能同属一个清算行");
			goto ErrExit;
		}
	}
	vtcp_log("[%s][%d]T_orderno=[%s] ",__FILE__,__LINE__,hv_notpay.T_orderno);
	vtcp_log(" [%s][%d]hv_notpay->F_content==[%s] ",__FILE__,__LINE__,hv_notpay.F_content);
	g_reply_int = iHvfFrontToTable(&hv_notpay,cBuff);
	vtcp_log("[%s][%d]T_orderno=[%s] ",__FILE__,__LINE__,hv_notpay.T_orderno);

	g_reply_int=iCharacterType(hv_notpay.T_req_content,sizeof(hv_notpay.T_req_content),'g');
    if(g_reply_int)
    {
        vtcp_log("%s,%d,输入的附言中[%.60s]第%d位存在非法字符",__FILE__,__LINE__,hv_notpay.T_req_content,g_reply_int);
        strncpy(acErrMsg,"输入的附言中存在非法字符",50);
		set_zd_data("0130","输入附言中有非法字符");
		goto ErrExit;
    }

	vtcp_log("[%s][%d]T_orderno=[%s] ",__FILE__,__LINE__,hv_notpay.T_orderno);
	vtcp_log(" [%s][%d]hv_notpay->T_req_content==[%s] ",__FILE__,__LINE__,hv_notpay.T_req_content);	
	if( g_reply_int )
	{
		sprintf( acErrMsg,"调用函数iHvTistoNotpay出错,[%s][%d]", __FILE__,__LINE__);
		WRITEMSG
		strcpy( g_pub_tx.reply, "P018");
		goto ErrExit;
	}
	vtcp_log(" [%s][%d]hv_notpay->T_req_content==[%s] ",__FILE__,__LINE__,hv_notpay.T_req_content);
	vtcp_log("[%s][%d]T_orderno=[%s] ",__FILE__,__LINE__,hv_notpay.T_orderno);
	find_qs_no(NULL,hv_notpay.T_pay_qs_no);
	if(cFlag[0]=='0')
	{

	     if((find_qs_no(hv_notpay.T_ac_br_no,hv_notpay.T_cash_qs_no))==NULL)
	     {
	        sprintf(acErrMsg,"没有找到接收清算行行号[%s][%d]!",__FILE__,__LINE__);
    	 	WRITEMSG
			strcpy( g_pub_tx.reply, "HV01");
			set_zd_data(DC_GET_MSG,"没有找到接收清算行行号");
    	 	goto ErrExit;
	     }

	}
	vtcp_log(" [%s][%d]hv_notpay->T_req_content==[%s] ",__FILE__,__LINE__,hv_notpay.T_req_content);
	vtcp_log("[%s][%d]T_orderno=[%s] ",__FILE__,__LINE__,hv_notpay.T_orderno);
	find_sendco(NULL,hv_notpay.T_receco);

	if(cFlag[0]=='0')
	{
		if((find_sendco(hv_notpay.F_ac_br_no,hv_notpay.T_receco))==NULL)
		{
			sprintf(acErrMsg,"未找到收报中心代码[%s][%d]!",__FILE__,__LINE__);
			WRITEMSG
			strcpy( g_pub_tx.reply, "HV01");
			set_zd_data(DC_GET_MSG,"未找到收报中心代码");
			goto ErrExit;
		}
	}
	else
	  memcpy(hv_notpay.T_receco,hv_notpay.F_ac_br_no,sizeof(hv_notpay.T_receco));
	vtcp_log(" [%s][%d]hv_notpay->T_req_content==[%s] ",__FILE__,__LINE__,hv_notpay.T_req_content);
	vtcp_log(" [%s][%d] ",__FILE__,__LINE__);
	hv_notpay.T_hv_fzf_sts[0]=SEND_ACCT_WAIT;/*更新状态*/
	vtcp_log("[%s][%d]T_orderno=[%s] ",__FILE__,__LINE__,hv_notpay.T_orderno);
	g_reply_int = iHvUpdFzfRecFromPayin(&hv_notpay,1);
	vtcp_log(" [%s][%d] ",__FILE__,__LINE__);
	vtcp_log(" [%s][%d]hv_notpay->T_req_content==[%s] ",__FILE__,__LINE__,hv_notpay.T_req_content);
	if( g_reply_int )
	{
		sprintf( acErrMsg,"调用函数iHvUpdFzfRecFromPayin出错,[%s][%d]",__FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01");
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log(" [%s][%d]hv_notpay->T_req_content==[%s] ",__FILE__,__LINE__,hv_notpay.T_req_content);
	/*** 开始写历史明细 ***/ 
	g_reply_int = dg_hv_fzfhtr(&hv_notpay);
	vtcp_log(" [%s][%d]hv_notpay->T_req_content==[%s] ",__FILE__,__LINE__,hv_notpay.T_req_content);
	if (g_reply_int)
	{
	   sprintf( acErrMsg,"插入自由格式明细表出错,[%s][%d]",__FILE__,__LINE__);
	   strcpy( g_pub_tx.reply, "HV01");
	   WRITEMSG
	   goto ErrExit;
	}
	/****Add by SSH,2006.8.28,直接发送****/
	vtcp_log(" [%s][%d]hv_notpay->T_req_content==[%s] ",__FILE__,__LINE__,hv_notpay.T_req_content);

	memcpy(cContent,hv_notpay.T_req_content,sizeof(hv_notpay.T_req_content));

	pub_com_CMT303_T2TAG(&hv_notpay,&pack303);
	vtcp_log(" [%s][%d]hv_notpay->T_req_content==[%s] ",__FILE__,__LINE__,hv_notpay.T_req_content);
	ret=hv_gen_pack(&pack303);
	if(ret<0){
		sprintf(acErrMsg,"组织大额支付报文错误!\n");
		WRITEMSG
		strcpy( g_pub_tx.reply, "P021" );
		goto ErrExit;
	}
	if(hv_insert_sendlist(&pack303,hv_notpay.F_tx_br_no,hv_notpay.F_tel\
			,hv_notpay.T_trace_no,hv_notpay.T_orderno)<0){
		sprintf(acErrMsg,"插入发送登记簿失败!\n");
		WRITEMSG
		strcpy( g_pub_tx.reply, "P010" );
		goto ErrExit;
	}
	if(hv_sendpack(&pack303)){
		sprintf(acErrMsg,"发送303报文到前置机失败!\n");
		strcpy( g_pub_tx.reply, "LS88" );
		WRITEMSG
		goto ErrExit;
	}
	/*************************************/

	/***处理一下附言,解决前台打印出现乱码的问题***/
	deal_half_charcter(cContent);
    vtcp_log("[%s]\n",cContent);
    memcpy(fd123.notpay_content,cContent,strlen(cContent));
    memcpy(fd123.orderno,hv_notpay.T_orderno,sizeof(fd123.orderno));
	
OkExit:
    pub_base_full_space((char *)&fd123,sizeof(fd123));
    set_zd_data("1230",(char *)&fd123);
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]\n",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
        if(memcmp(g_pub_tx.reply,"0000",4)==0)
                memcpy(g_pub_tx.reply,"T063",4);

	sprintf(acErrMsg,"Before return: reply is[%s]\n",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;	
	
}

/****处理在附言中存在换行符号和回车符号的情况****/
int   myfunxier(char *source,char *target)
{
    int i = 0;
    int j = 0;
    vtcp_log("[%s][%d]传入的字符串source==[%s]\n",__FILE__,__LINE__,source);
    while(source[i]!=0x00)
    {
    	
        if((int)source[i]==13)/**13是C中的换行符号**/
            i = i+2;
        else if((int)source[i]==10)/**10表示的是回车符号**/
            i = i+1;
        else
        {
            target[j] = source[i];
            i++;
            j++;
        }
    }
    target[j] = 0x00;/**必须的***/
    
    vtcp_log("[%s][%d]处理好的字符串target==[%s]\n",__FILE__,__LINE__,target);
    return 0;
}
int deal_half_charcter(char *_buf)
{
		int i = 0;
		int j = 0;
		int size = 0;
		int index =0;
		char buf[512];

		memset(buf,0x00,sizeof(buf));
		
		
	    myfunxier(_buf,buf);
		
		

		printf("[%s]====\n",buf);
		size = strlen(buf);
		
		char cBuf[1024];
		
		memset(cBuf,0x00,sizeof(cBuf));


		for(i=0;i<size;i++)
		{
				if((int)buf[i]<128 &&(int)buf[i+1]>128 && (i+1+index)%2)
				{
						cBuf[j] = buf[i];
						cBuf[j+1]= ' ';
						j = j+2;
						index++;
				}
				else
				{
						cBuf[j] = buf[i];
						cBuf[j] = buf[i];
						j++;
				}
		}

		memcpy(_buf,cBuf,sizeof(cBuf));
		return 0;
}
