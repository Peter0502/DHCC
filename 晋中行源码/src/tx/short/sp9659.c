/*************************************************
* 文 件 名:  sp9659.c
* 功能描述： 验证身份证号码校验位
*
* 作    者:  ZHGF 
* 完成日期： 2008年2月22日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/
#include <string.h>
#define EXTERN
#include "public.h"
#define ERR_DEAL if(ret){\
              sprintf(acErrMsg,"[%s],[%d],sqlcode=[%d]",__FILE__,__LINE__,ret);\
              WRITEMSG \
              goto ErrExit;\
             }
int sp9659()
{
	int             ret;
	char            id_no[20];
	char						cOutBuf[20];
	/* 初始化 */
	memset(id_no, 0x00, sizeof(id_no));
	memset(cOutBuf,0x00,sizeof(cOutBuf));
	/* 前台取值 */
	get_zd_data("0620", id_no);	/* 证件号码 */
	vtcp_log("[%s][%d]传入的身份证为[%s]\n",__FILE__,__LINE__,id_no);
	ret = iPub_base_SFZYZ(id_no, cOutBuf);
        if(ret == -1)
        {
	        sprintf(acErrMsg,"身份证校验失败",g_pub_tx.reply);
                WRITEMSG
                strcpy(g_pub_tx.reply,"CD02");
                vtcp_log("%s,%d 身份证校验失败,校验位为[%d]",__FILE__,__LINE__,ret);
                goto ErrExit;
          }
	vtcp_log("%s,%d,cOutBuf[%s]",__FILE__,__LINE__,cOutBuf);
	set_zd_data("0620",cOutBuf);
OkExit:
	strcpy(g_pub_tx.reply, "0000");
	sprintf(acErrMsg, "Before OK return: reply is[%s]", g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg, "Before return: reply is[%s]", g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 1;
}
