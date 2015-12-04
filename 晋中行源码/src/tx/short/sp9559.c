/*****************************************************
* 文 件 名:  sp9559.c
* 功能描述： 同城交易的前台校验输入的提入或提出行号是否正确,并取到交易机构的交换行号
* 数 据 库:  changetab
* 作    者:  Liuxuan
* 完成日期： 2006年8月17日 
* 修改记录： 
* 日    期:
* 修 改 人:
* 修改内容:
*****************************************************/

#define EXTERN
#include <string.h>
#include "public.h"
#include "changetab_c.h"

int sp9559()
{
	int ret=0;
	char or_br_no[11],kinbr[6];
	struct changetab_c changetab;    
	
	memset(or_br_no,'\0',sizeof(or_br_no));
	memset(&changetab,0x00,sizeof(changetab));
 	vtcp_log("sp9559开始！[%s][%d]",__FILE__,__LINE__);

	pub_base_sysinit();

	memset(kinbr,'\0',sizeof(kinbr));
	get_zd_data("0020",kinbr);
	get_zd_data("0860",or_br_no);
	or_br_no[10]='\0';
   	pub_base_strpack(or_br_no);	
	vtcp_log("[%s][%d]  or_br_no=[%s],kinbr=[%s]",__FILE__,__LINE__,or_br_no,kinbr);
	ret = Changetab_Sel(g_pub_tx.reply,&changetab, "change_no='%s'",or_br_no);
	if ( ret )
	{
		sprintf(acErrMsg,"查询行名出错[%d]",ret);
		WRITEMSG
		goto ErrExit;
	}	
	
 vtcp_log("[%s][%d]  br_name_f=[%s]",__FILE__,__LINE__,changetab.br_name);

 set_zd_data("0961",changetab.br_name);  /* 机构全称: 支付系统行名 */
/*
	memset(&changetab,'\0',sizeof(changetab));
	ret = Changetab_Sel(g_pub_tx.reply,&changetab, "br_no='%s'",kinbr);
	if ( ret )
	{
		sprintf(acErrMsg,"查询交易机构的交换行号出错[%d][%s]",ret,kinbr);
		WRITEMSG
		goto ErrExit;
	}	
*/
 vtcp_log("[%s][%d]  br_name_f=[%s]",__FILE__,__LINE__,changetab.br_name);
 set_zd_data("0870",changetab.change_no);  /* 交易行对应的交换行号 */
 set_zd_data("0810",changetab.br_name);  /* 交易行对应的交换行号 */

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"success !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;

ErrExit:
	sprintf(acErrMsg,"fail !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

