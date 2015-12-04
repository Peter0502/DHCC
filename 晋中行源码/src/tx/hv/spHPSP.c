/***************************************************************
* 文 件 名:     spHPSP.c
* 功能描述：    逾期退回银行汇票解付  
*
* 作    者:   LiuYue
* 完成日期：  2006-8-27 21:08
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


spHPSP()
{
	/** 准备帐务处理分录,从参数配置表中得到相应的帐号 */
	/* add by LiuHuafeng 2006-10-3 16:05 */
		int iRet=0;
		char cKinbr[BRNO_LEN+1];
		char cAc_no[25];
		char cStep[2];
		memset(cStep, 0 , sizeof(cStep));
		memset(cAc_no, 0 , sizeof(cAc_no));
		memset(cKinbr, 0 , sizeof(cKinbr));
		/* 得到当前执行的步骤 */
		get_zd_data("0990",cStep);
		get_zd_data(DC_TX_BR_NO,cKinbr);
		switch(cStep[0]){
		case '0':
		/* 得到第一对分录 dr 261   cr 客户  */
			memset(cAc_no,0,sizeof(cAc_no));
			/* 2 客户帐*/
			get_zd_data("1011",cAc_no);
			set_zd_data("0660","2");
			set_zd_data("0990","1");
		vtcp_log("%s,%d step===[%c]",__FILE__,__LINE__,cStep[0]);
			break;
		case '1':
		/* 1,261帐号 */
			iRet=pub_base_GetParm_Str(cKinbr,PARM_HPGZ,cAc_no);
			if(iRet)
			{
				sprintf(acErrMsg,"得到本机构汇票挂帐帐号错误!\n");
				WRITEMSG
				vtcp_log("%s,%d 得到本机构汇票挂帐帐号错误\n",__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"P005");
				goto ErrExit;
			}
			set_zd_data("0660","1");
			set_zd_data("0990","2");
			set_zd_data("0680","0");
		vtcp_log("%s,%d step===[%c]",__FILE__,__LINE__,cStep[0]);
			break;
		case '2':
			/* 得到第二对分录 dr 2440403   cr 2440401  */
			/* 1,2440403支行退回专用帐号 */
			iRet=pub_base_GetParm_Str(cKinbr,PARM_HPTH,cAc_no);
			if(iRet)
			{
				sprintf(acErrMsg,"得到本机构汇票挂帐帐号错误!\n");
				WRITEMSG
				vtcp_log("%s,%d 得到本机构汇票挂帐帐号错误\n",__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"P005");
				goto ErrExit;
			}
			set_zd_data("0660","1");
			set_zd_data("0990","3");
			set_zd_data("0680","0");
		vtcp_log("%s,%d step===[%c]",__FILE__,__LINE__,cStep[0]);
			break;
		case '3':
			memset(cAc_no,0,sizeof(cAc_no));
			/* 2 2440401支行签发专用帐户 */
			iRet=pub_base_GetParm_Str(cKinbr,PARM_HPYC,cAc_no);
			if(iRet)
			{
				sprintf(acErrMsg,"得到本机构汇票挂帐帐号错误!\n");
				WRITEMSG
				vtcp_log("%s,%d 得到本机构汇票挂帐帐号错误\n",__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"P005");
				goto ErrExit;
			}
			set_zd_data("0660","2");
			set_zd_data("0990","4");
			set_zd_data("0680","0");
		vtcp_log("%s,%d step===[%c]",__FILE__,__LINE__,cStep[0]);
			break;
		default:
			vtcp_log("%s,%d step===[%c]",__FILE__,__LINE__,cStep[0]);
			goto FinishExit1;
			break;
		}
		set_zd_data("0300",cAc_no);
		vtcp_log("%s,%d ac_no====[%s]",__FILE__,__LINE__,cAc_no);
OkExit:
	strcpy(g_pub_tx.reply,"0001");
	sprintf(acErrMsg,"成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
FinishExit1:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"全部成功结束![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
