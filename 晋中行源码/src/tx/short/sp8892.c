/*************************************************
* 文 件 名:  sp8892.c
* 功能描述： 判断该客户是否个人;对于网银、晋中宝、手机银行查询账号和手机号是否匹配
* 作    者:  lwb
* 完成日期： 2015年4月15日
*
* 修改记录： 
*   日   期: 
*   修 改 人:
*   修改内容:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "dd_mst_c.h"
#include "cif_basic_inf_c.h"
#include "mob_acct_type_c.h"
#include "mdm_ac_rel_c.h"

int sp8892()
{
	int ret = 0;
	char cAcno[21];	
	char mobile[15];
	
	struct mob_acct_type_c sMob_acct_type;	/*短信银行客户账号与服务品种对应表*/
	struct cif_basic_inf_c sCif_basic_inf;	/*客户基本信息表*/
	struct dd_mst_c sDd_mst;								/*活期账户信息表*/
	struct mdm_ac_rel_c sMdm_ac_rel;				/*介质与账户关系表*/
	
	/*清空*/
	memset(&sCif_basic_inf,0x00,sizeof(sCif_basic_inf));
	memset(&sMob_acct_type,0x00,sizeof(sMob_acct_type));
	memset(&sDd_mst,0x00,sizeof(sDd_mst));
	memset(&sMdm_ac_rel,0x00,sizeof(sMdm_ac_rel));

	
	/*前台取值*/
	get_zd_data("0370",cAcno);
	sprintf(acErrMsg, "账号为：[%s]", cAcno);
	WRITEMSG
	
	ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_no = '%s'",cAcno);
	if(ret == 100)
	{
   		vtcp_log("[%s],[%d] mdm_ac_rel 表中无此账号!",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"C115");
		goto ErrExit;
	}
	else if(ret)
	{
		vtcp_log("[%s][%d]查询表mdm_ac_rel错误!",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"L182");
		goto ErrExit;
	}
	
	if(sMdm_ac_rel.note_sts[0]=='*')
	{
		vtcp_log("[%s][%d]该介质已销户!",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"P172");
		goto ErrExit;
	}
	ret = Dd_mst_Sel(g_pub_tx.reply,&sDd_mst,"ac_id = %ld ",sMdm_ac_rel.ac_id);
	if(ret == 100)
	{
		vtcp_log("[%s][%d]此账号非活期账号!",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"P172");
		goto ErrExit;
  	
	}	
	else if(ret)
	{
		vtcp_log("[%s][%d]查询dd_mst错误![%d]\n",__FILE__,__LINE__,ret);
		strcpy(g_pub_tx.reply,"W015");
		goto ErrExit;	
	}

	if(sDd_mst.ac_sts[0] == '*')
  	{
		vtcp_log("[%s][%d]此账号已销户![%d]\n",__FILE__,__LINE__,ret);
		strcpy(g_pub_tx.reply,"P245");
		goto ErrExit;		
  	}
	
	/*查询短信银行客户账号与服务品种对应表*/
	ret = Mob_acct_type_Sel(g_pub_tx.reply,&sMob_acct_type,"ac_no='%s'  ",cAcno);
	
	if(ret == 100)
	{
		vtcp_log("[%s],[%d] 此客户未做过短信签约或已解约!",__FILE__,__LINE__);
		goto ErrExit;
	}
	else if( ret )
	{
		vtcp_log("[%s][%d]查询表Mob_acct_type_Sel错误!",__FILE__,__LINE__);
    strcpy(g_pub_tx.reply,"D104");
    goto ErrExit; 
	}

	sprintf(acErrMsg,"检查短信银行客户账号与服务品种对应表 PASS!");
	strcpy(mobile,sMob_acct_type.mobile);   /*记录手机号*/

	 /*将手机号传给前台*/
	 set_zd_data("0810",mobile);

	
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

