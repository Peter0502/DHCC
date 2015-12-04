/*************************************************
* 版 本 号 :  
* 程序作者 :  吴海帆
* 日    期 :  2015/01/08
* 所属模块 :  收款账户信息校验
* 程序名称 :  spMK04.c
* 程序作用 :
* 函数列表 :
* 使用注意 :
交易配置：
insert into tx_def values ('5024', '收款账户信息校验', '10000000000000000000001111110100000111100000110000010000001000001111111100001010010001000000100000011000000000000000000000000000', '0','4', '0');
insert into tx_flow_def values ('5024', 0, 'MK04', '#$');
insert into tx_sub_def values ('MK04', '收款账户信息校验', 'spMK04', '0', '0');
*************************************************/
#define EXTERN
#include <string.h>
#include "public.h"
#include "dd_mst_c.h"			/*活期存款主文件*/
#include "td_mst_c.h"			/*定期存款主文件*/
#include "mdm_ac_rel_c.h"		/*介质与账户关系表*/
#include "prdt_ac_id_c.h"		/*产品账号对照表*/

int spMK04()
{
	struct mdm_ac_rel_c g_mdm_ac_rel;
	struct prdt_ac_id_c sPrdt_ac_id;
	struct dd_mst_c g_dd_mst;
	struct td_mst_c g_td_mst;	
	
	memset(&g_mdm_ac_rel,0x00,sizeof(g_mdm_ac_rel));
	memset(&sPrdt_ac_id,0x00,sizeof(sPrdt_ac_id));
	memset(&g_dd_mst,0x00,sizeof(g_dd_mst));
	memset(&g_td_mst,0x00,sizeof(g_td_mst));
	
	int ret=-1;
	char ac_id_str[20]={0};
	char Ac_id_str[200]={0};
	char sTemp[10]={0};
	
	char accountNo[32+1]; /*账号*/
	char custName[60+1];  /*客户姓名*/
	
	memset(accountNo,0x00,sizeof(accountNo));
	memset(custName,0x00,sizeof(custName));
	
	/**初始化公用结构**/
	if (pub_base_sysinit())
	{
	    vtcp_log("初始化公用结构错误!!");
	    strcpy(g_pub_tx.reply, "CU06");
	    goto ErrExit;
	}
	
	get_zd_data("0300",accountNo);
	get_zd_data("0370",custName);
	
	/*检查账户*/
	pub_base_strpack(accountNo);
	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",accountNo);
	if( 100==ret || 1403==ret ) 
	{
		vtcp_log("%s %d::该账户不存在!",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"D104");
		set_zd_data("0130","该账户不存在");
		goto ErrExit;
	}
	else if( ret ) 
	{
		vtcp_log("%s %d::查询介质和帐户关系表错误!",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"AT06");
		set_zd_data("0130","查询介质和帐户关系表错误");
		goto ErrExit;
	}
	set_zd_data("0670", g_mdm_ac_rel.note_sts); /*介质状态*/	
	 
	/**活期定期区分**/
	ret = -1;
	ret = Prdt_ac_id_Sel(g_pub_tx.reply, &sPrdt_ac_id,"ac_id=%ld", g_mdm_ac_rel.ac_id);
	if( 100==ret || 1403 == ret ) 
	{
		vtcp_log("%s %d::该账户号不存在!",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"D104");
		set_zd_data("0130","该账户号不存在");
		goto ErrExit;
	}
	else if( ret ) 
	{
		vtcp_log("%s %d::查询产品账号对照表错误!",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"AT06");
		set_zd_data("0130","查询产品账号对照表错误");
		goto ErrExit;
	}
	
	/**查询活期产品!***/
	if (sPrdt_ac_id.ac_id_type[0] == '1')
	{/**获取账户状态**/
		ret = -1;
		ret=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,"ac_id=%ld ",g_mdm_ac_rel.ac_id);
		if( 100==ret || 1403==ret) 
		{
			vtcp_log("%s %d::该账户号不存在!",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"D104");
			set_zd_data("0130","该账户号不存在");
			goto ErrExit;
		}
		else if( ret ) 
		{
			vtcp_log("%s %d::查询活期存款主文件表错误!",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"AT06");
			set_zd_data("0130","查询活期存款主文件表错误");
			goto ErrExit;
		}
		
		set_zd_data("0660", g_dd_mst.ac_sts);/*账户状态*/
		set_zd_data("0690", g_dd_mst.hold_sts);/*冻结状态*/
	}
	/**查询定期产品!***/
	else if (sPrdt_ac_id.ac_id_type[0] == '2')               
	{/**获取账户状态**/
		ret = -1;
		ret=Td_mst_Sel(g_pub_tx.reply,&g_td_mst,"ac_id=%ld ",g_mdm_ac_rel.ac_id);
		if( 100==ret || 1403==ret) 
		{
			vtcp_log("%s %d::该账户号不存在!",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"D104");
			set_zd_data("0130","该账户号不存在");
			goto ErrExit;
		}
		else if( ret ) 
		{
			vtcp_log("%s %d::查询定期存款主文件表错误!",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"AT06");
			set_zd_data("0130","查询定期存款主文件表错误");
			goto ErrExit;
		}
		
		set_zd_data("0660", g_td_mst.ac_sts);/*账户状态*/
		set_zd_data("0690", g_td_mst.hold_sts);/*冻结状态*/
	}
	
	memset(sTemp,0x00,sizeof(sTemp));
	sprintf(sTemp,"%ld",sPrdt_ac_id.cif_no);
	set_zd_data("0860",sTemp);  /**客户号**/
	set_zd_data("0240", sPrdt_ac_id.ac_id_type);  /*账户类型*/
	
	
OkExit:
        strcpy(g_pub_tx.reply, "0000");
        vtcp_log("[%s],[%d]::Before OK return: reply is[%s]",__FILE__,__LINE__,g_pub_tx.reply);
        set_zd_data(DC_REPLY, g_pub_tx.reply);
        return 0;
ErrExit:
        vtcp_log("[%s],[%d]::Before return: reply is[%s]",__FILE__,__LINE__,g_pub_tx.reply);
        set_zd_data(DC_REPLY, g_pub_tx.reply);
        return 1;
}











