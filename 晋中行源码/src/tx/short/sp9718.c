/*************************************************
* 文 件 名: sp9718.c
* 功能描述：根据柜员号回显柜员名称
*
* 作    者: rob
* 完成日期：2003年03月31日
* 修改记录： 
*     1. 日    期:
*        修 改 人:
*        修改内容:
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#define EXTERN
#include "public.h"
#include "com_tel_c.h"

sp9718()  
{ 		
	int ret=0;
	char tmp_title[41];
	char cTx_code[5];

	struct com_tel_c	g_com_tel;

	memset(&g_com_tel,0x00,sizeof(struct com_tel_c));
	memset(cTx_code,0x00,sizeof(cTx_code));

	/*系统初始化（初始化g_pub_tx)数据初始化*/
	pub_base_sysinit();

	/** 取值、赋值 **/	
	get_zd_data("0920",g_pub_tx.tel);	/*柜员号码*/
	get_zd_data("0650",cTx_code);	/*交易代码   目前只用于现金调剂交易*/
	
	if( strlen(g_pub_tx.tel)==0 )
	{
		goto OkExit;
	}
	
	ret=Com_tel_Sel(g_pub_tx.reply,&g_com_tel,"tel='%s'",g_pub_tx.tel);
	if(ret==100)
	{
		sprintf(acErrMsg,"该柜员不存在");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O120");
		goto ErrExit;
	}
	else if(ret)
	{
		sprintf(acErrMsg,"查询柜员信息表错误[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"O065");
		goto ErrExit;
	}

	/****当做现金调剂交易时，对方没签到，不允许调剂  add by wanglei 20070429****/
	/****当现金调剂时，对方不是签到状态不允许调剂 modify at 20100722
	if(g_com_tel.csts[0] != '2')
	****/
	vtcp_log("[%s][%d] TX_CODE==[%s]",__FILE__,__LINE__,cTx_code);
	if(memcmp(cTx_code,"5203",4)==0)
	{
		if(g_com_tel.csts[0] == '1')
		{
			sprintf(acErrMsg,"该柜员状态已签退! tel=[%s]",g_pub_tx.tel);
			WRITEMSG
			strcpy(g_pub_tx.reply,"O046");
			goto ErrExit;
		}else if(g_com_tel.csts[0] == '2')
		{
			sprintf(acErrMsg,"该柜员状态已签退! tel=[%s]",g_pub_tx.tel);
			WRITEMSG
			strcpy(g_pub_tx.reply,"O046");
			goto ErrExit;
		}else if(g_com_tel.csts[0] == '3')
		{
			sprintf(acErrMsg,"该柜员状态已锁定! tel=[%s]",g_pub_tx.tel);
			WRITEMSG
			strcpy(g_pub_tx.reply,"O063");
			goto ErrExit;
		}else if(g_com_tel.csts[0] == '4')
		{
			sprintf(acErrMsg,"该柜员状态已删除! tel=[%s]",g_pub_tx.tel);
			WRITEMSG
			strcpy(g_pub_tx.reply,"O062");
			goto ErrExit;
		}
	}
	/**** end by wanglei ****/

	set_zd_data("0860",g_com_tel.name);
	vtcp_log("[%s][%d] br_no = [%s]\n",__FILE__,__LINE__,g_com_tel.br_no);
	set_zd_data("0910",g_com_tel.br_no); /****返回机构号   20070425****/


OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
   
