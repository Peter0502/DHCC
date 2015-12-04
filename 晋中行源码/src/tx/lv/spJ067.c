/***************************************************************
* 文 件 名:   spJ067.c
* 功能描述：  小额通用信息业务录入
*
* 作    者:   dongxy
* 完成日期：  2006-8-29 
*
* 修改记录：
* 日    期: 2006-9-18
* 修 改 人: liuyue 
* 修改内容:
**************************************************************/
#define EXTERN
#include <string.h>
#include <stdio.h>
#include "hv_define.h"
#include "lv_define.h"
#include "hvpack.h"
#include "lvpack.h"
#include "lv_fd123.h"
#define loc_t sys_loc_t

spJ067()
{
	int  			iOrderno = 0;
   	char 			cBrno[6];		/** 交易机构 **/
	PAY_IN_AREA 		lv_payin;
	struct lv_pkgreg_c 	lv_pkgreg;
	LV_FD123_AREA		fd123;	
	int			ret = 0;


	memset(cBrno ,	  	0x00,		sizeof(cBrno));
	memset(&lv_payin,	0x00,		sizeof(lv_payin));
	memset(&lv_pkgreg,	0x00,		sizeof(lv_pkgreg));
	memset(&fd123,		0x00,		sizeof(fd123));

	vtcp_log("[%s][%d]spJ067 Begin!通用信息业务录入",__FILE__,__LINE__);


	/**********************
	 * 1.初始化全局变量   *
	 **********************/

	 pub_base_sysinit();
	if(iLvchkstat())
	{
		strncpy(g_pub_tx.reply,"P046",4);
		WRITEMSG
		goto ErrExit;
	}



	/**********************
	 * 2.获取8583域内容   *
	 **********************/

	 ret = iLvTisToPayin(&lv_payin);
	 if(ret)
	 {
		vtcp_log("[%s][%d]从前台获取数据失败!\n",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"P053");
		goto ErrExit;
	 }

    ret =  iLvPayinCheck1(&lv_payin);
    if(ret)
    {
        vtcp_log("[%s][%d]行号检查错误!\n",__FILE__,__LINE__);
        strcpy(g_pub_tx.reply,"P053");
        goto ErrExit;
    }

	 /***********************
	  * 3. 执行数据基本检查 *
	  ***********************/

	  ret = iLvFzFCommonCheck2(&lv_payin);
	  if(ret)
	  {
		vtcp_log("[%s][%d]获取8583域内容失败!\n",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"P054");
		goto ErrExit;
	  }


	  /********************
	   * 4.执行数据库操作 *
	   ********************/



	/*****生成交易序号*********/
	iOrderno=iLvGetOrderno();

	if(iOrderno<=0)
	{
		vtcp_log("%s,%d,得到支付交易序号错误!\n",__FL__);
		WRITEMSG
		strncpy(g_pub_tx.reply,"P046",4);
		goto ErrExit;
	}
	apitoa(iOrderno,sizeof(lv_payin.F_orderno),lv_payin.F_orderno);
 
	ret = iLvNewRecFromPayin(&lv_payin);
	if(ret)
	{
		vtcp_log("[%s][%d]执行函数iLvNewRecFromPayin错误!\n",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"P046");
		goto ErrExit;
	}


	ret = iLvUpdRecFromPayin(&lv_payin,1);
	if(ret)
	{
		vtcp_log("[%s][%d]执行函数iLvUpdRecFromPayin错误!\n",__FILE__,__LINE__);	
		strcpy(g_pub_tx.reply,"P046");
		goto ErrExit;
	}

	vtcp_log("[%s][%d]数据库操作成功!\n",__FILE__,__LINE__);
	  

	/***********************
	 * 5.准备流水日志      *
	 ***********************/

        ret  = pub_ins_trace_log();
        if (ret)
        {
                sprintf( acErrMsg, "记流水日志错" );
                WRITEMSG
                goto ErrExit;
        }

	/*****************
	 * 6.准备输出    *
	 *****************/
	memcpy(fd123.orderno,lv_payin.F_orderno,sizeof(fd123.orderno));
	vtcp_log("[%s][%d]F_ordenro===[%.8s]\n",__FILE__,__LINE__,fd123.orderno);
	pub_base_full_space((char *)&fd123,sizeof(fd123));
	set_zd_data("1230",(char *)&fd123);
	vtcp_log("[%s]\n",(char *)&fd123);

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]\n",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]\n",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
	
