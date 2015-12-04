/*************************************************
* 文 件 名: sp9317.c
* 功能描述：电话银行取款密码核对
* 作    者: ZHGF
* 完成日期：2008年05月19日
* 修改记录：
*     1. 日    期:
*        修 改 人:
*        修改内容:
*     2.
	  交易配置：
	insert into tx_def values('9317','电话银行取款密码核对','10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000','0','4','0');
	insert into tx_flow_def values('9317','0','9317','#$');
	insert into tx_sub_def values('9317','电话银行取款密码核对','sp9317','0','0');
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;}
#include "stdio.h"
#define EXTERN
#include "public.h"
#include "mdm_ac_rel_c.h"

int sp9317()
{
	int ret=0;
	char cAc_no[20];
	char cDraw_pwd[7];
	char filename[100];
	FILE *fp;
	struct mdm_ac_rel_c		z_mdm_ac_rel;
	/** 初始化全局变量 **/
	pub_base_sysinit();
	sprintf(acErrMsg,"初始化全局变量 PASS!");
	WRITEMSG
	memset(filename,0x00,sizeof(filename));
	memset(cAc_no,0x00,sizeof(cAc_no));
	memset(cDraw_pwd,0x00,sizeof(cDraw_pwd));
	memset(&z_mdm_ac_rel,0x00,sizeof(z_mdm_ac_rel));
	sprintf(acErrMsg,"数据初始化 PASS!");
	WRITEMSG
	/*****{
		set_zd_data("0300","6229366601000341208");
		set_zd_data("0790","111112");
	}*****/
	get_zd_data("0300",g_pub_tx.ac_no);
	get_zd_data("0790",g_pub_tx.draw_pwd);
	pub_base_strpack(g_pub_tx.draw_pwd);
	pub_base_strpack(g_pub_tx.ac_no);
	pub_base_old_acno(g_pub_tx.ac_no);
	vtcp_log("%s,%d,账号为[%s]",__FILE__,__LINE__,g_pub_tx.ac_no);
	{/**开始检查取款密码**/
		/*vtcp_log("%s,%d 电话银行取款密码[%s]",__FILE__,__LINE__,g_pub_tx.draw_pwd);*/
		ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&z_mdm_ac_rel,"ac_no='%s'",g_pub_tx.ac_no);
		if(ret){
			sprintf(acErrMsg,"查询介质和帐户关系表错误!");
			set_zd_data(DC_GET_MSG,acErrMsg);
			WRITEMSG
			    strcpy(g_pub_tx.reply,"W011");
			ERR_DEAL;
		}
		if(z_mdm_ac_rel.draw_pwd_yn[0]=='N'){
			sprintf(acErrMsg,"无密户请到柜台办理业务!");
			set_zd_data(DC_GET_MSG,acErrMsg);
			WRITEMSG
			    strcpy(g_pub_tx.reply,"D240");
			ERR_DEAL;
		}
		strcpy(g_pub_tx.id_no,z_mdm_ac_rel.id_no);
		strcpy(g_pub_tx.id_type,z_mdm_ac_rel.id_type);
		/**开始检查电话银行取款密码**/
		ret = pub_base_check_draw("101");
		if(ret){
			sprintf(acErrMsg,"支取密码检验错误!");
			sprintf(g_pub_tx.reply,"M005");
			goto ErrExit;
		}
	}
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"电话银行取款密码核对成功！[%s]",g_pub_tx.reply);
	WRITEMSG
	    set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;

ErrExit:
	sprintf(acErrMsg,"电话银行取款密码核对失败！[%s]",g_pub_tx.reply);
	WRITEMSG
	    set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
