/*************************************************
* 文 件 名:  sp9121.c
* 功能描述： 判断柜员级别
*
* 作    者:  shiweina
* 完成日期： 2009年7月8日 
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "com_tel_c.h"

int 
sp9121()
{
	struct com_tel_c sComTel;	/* 柜员信息表结构 */
	int             ret;
	char cTel[TEL_LENGTH+1];
	char cTel2[TEL_LENGTH+1];
	char lvl1[2]; 
	char lvl2[2];
	/* begin add by LiuHuafeng 20090718 */
	memset(&sComTel , 0 , sizeof(sComTel));
	/* end by LiuHuafeng 20090718 */
	memset(cTel,0x00,sizeof(cTel));
	memset(cTel2,0x00,sizeof(cTel2));
	memset(lvl1,0x00,sizeof(lvl1));
	memset(lvl2,0x00,sizeof(lvl2));
	/****
	get_zd_data("0910",cTel); 
	要查询的操作员 
	**/
	/* rem by LiuHuafeng 20090719 用第92域表明操作员
	*get_zd_data("0280",cTel); 
	***要查询的操作员 ****/
	get_zd_data("0920",cTel); /****要查询的操作员 ****/
	get_zd_data("0070",g_pub_tx.tel);
	sprintf(acErrMsg, "tel[%s]", g_pub_tx.tel);
	WRITEMSG
	/****去掉柜员号里的空格****/
	pub_base_strpack_all(cTel);
	vtcp_log("%s,%d,进来了吗",__FILE__,__LINE__);
		
	ret = Com_tel_Sel(g_pub_tx.reply, &sComTel, \
				  "tel='%s'", g_pub_tx.tel);
	if (ret == 100) {
			sprintf(acErrMsg, "取柜员信息表错!\
				tel=[%s]", g_pub_tx.tel);
			WRITEMSG
				strcpy(g_pub_tx.reply, "S009");
			goto ErrExit;
	} else if (ret) {
			sprintf(acErrMsg, "执行Com_tel_Sel错误!ret=[%d]", ret);
			WRITEMSG
				goto ErrExit;
	}
	vtcp_log("%s,%d,g_pub_tx.tel='%s'",__FILE__,__LINE__,g_pub_tx.tel);
	memcpy(lvl1,sComTel.lvl,sizeof(lvl1)-1); /**操作员的级别**/
		
	ret = Com_tel_Sel(g_pub_tx.reply, &sComTel, \
				  "tel='%s'", cTel);
	if (ret == 100) {
		sprintf(acErrMsg, "取柜员信息表错!\
			tel=[%s]", cTel);
		WRITEMSG
			strcpy(g_pub_tx.reply, "S009");
		goto ErrExit;
	} else if (ret) {
		sprintf(acErrMsg, "执行Com_tel_Sel错误!ret=[%d]", ret);
		WRITEMSG
			goto ErrExit;
	}
	memcpy(lvl2,sComTel.lvl,sizeof(lvl2)-1);  /**要查询的柜员的级别**/
	vtcp_log("%s,%d,进来了吗",__FILE__,__LINE__);
	/**	
	 *if(strcmp(lvl1,"1")==0 || strcmp(lvl1,"2")==0)
	 *{
	 *	if(strcmp(g_pub_tx.tel,cTel)!=0)
	 *	{
	 *		vtcp_log("%s,%d,两个最低级别的柜员%s,%s,%s,%s",__FILE__,__LINE__,g_pub_tx.tel,cTel,lvl1,lvl2);
	 *		sprintf(acErrMsg,"最低两个级别的柜员不能互查");
	 *		WRITEMSG
	 *		goto ErrExit;
	 *	}
	 *}
	 *else if(strcmp(lvl1,"5")==0)
	 *{
	 *	sprintf(acErrMsg,"凭证管理员可以查所有人的凭证");
	 *}
	 *else if(strcmp(lvl1,lvl2)<0)
	 *{
	 *	vtcp_log("%s,%d,不能查询高级的柜员%s,%s",__FILE__,__LINE__,lvl1,lvl2);
	 *	sprintf(acErrMsg,"除了凭证管理员，不能查询比自己级别高的柜员的凭证");
	 *	WRITEMSG
	 *	goto ErrExit;
	 *}
	 **/
	memcpy(cTel2,g_pub_tx.tel,sizeof(cTel2)-1);
	vtcp_log("%s,%d,进来了吗",__FILE__,__LINE__);
	set_zd_data("0680",lvl1);
	set_zd_data("0690",lvl2);
	set_zd_data("0790",cTel2);
	vtcp_log("%s,%d,看看交易柜员='%s' 柜员级别%s",__FILE__,__LINE__,g_pub_tx.tel,lvl1);
	vtcp_log("%s,%d,看看查询的柜员='%s' ",__FILE__,__LINE__,cTel);
		
	
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
