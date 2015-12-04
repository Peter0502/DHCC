/***********************************************************
*文件名: sp6383.c
*功能描述: 判断输入账户是否为该客户最后一个未销户活期账户，
若是则判断该客户是否有签约网银，用于控制当该客户在销最后
一个账户时先去销网银账户,可以销户67域返回“0”,不能销户返回“1”
*作者: 胡朝华
*日期: 20130811
***********************************************************/

#define EXTERN
#include "public.h"
#include "com_sys_parm_c.h"
#include "mdm_ac_rel_c.h"
#include "prdt_ac_id_c.h"
#include "dd_mst_c.h"
#include "netbank_cif_ukey_rel_c.h"

int sp6383()
{
	int ret;
	char sAcno[30];
	struct mdm_ac_rel_c mdm_ac_rel;
	struct prdt_ac_id_c prdt_ac_id;
	struct dd_mst_c dd_mst;
	struct netbank_cif_ukey_rel_c netbank_cif_ukey_rel;
	
	ret=0;
	memset(sAcno,0x00,sizeof(sAcno));
	memset(&mdm_ac_rel, 0x00, sizeof(struct mdm_ac_rel_c));
	memset(&prdt_ac_id, 0x00, sizeof(struct prdt_ac_id_c));
	memset(&dd_mst, 0x00, sizeof(struct dd_mst_c));
	memset(&netbank_cif_ukey_rel, 0x00, sizeof(struct netbank_cif_ukey_rel_c));

	pub_base_sysinit();
	/*从前台取值*/
	get_zd_data("0300",sAcno);
	vtcp_log("前台传来账号为:[%s] [%s]--[%d]",sAcno,__FILE__,__LINE__);
	pub_base_old_acno( sAcno );                 /**对旧帐号的处理**/

	ret = Mdm_ac_rel_Sel( g_pub_tx.reply,&mdm_ac_rel,"ac_no='%s'",sAcno );
	if( ret==100 )
	{
		sprintf(acErrMsg,"介质帐号对照表不存在该记录!![%s]",sAcno);
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"查找出错[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}
	ret = Prdt_ac_id_Sel(g_pub_tx.reply,&prdt_ac_id,"ac_id=%ld", mdm_ac_rel.ac_id);
	if( ret==100 )
	{
		sprintf(acErrMsg,"产品帐号对照表不存在该记录!! [%ld]",mdm_ac_rel.ac_id);
 		WRITEMSG
  		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"查找帐号出错!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}
	if(memcmp(prdt_ac_id.ac_id_type,"1",1)==0)
	{
		vtcp_log("该账号客户号为:[%s] [%d]--[%d]",__FILE__,__LINE__,prdt_ac_id.cif_no);
		ret = sql_count("dd_mst","cif_no=%ld and ac_sts='1'",prdt_ac_id.cif_no);
		if( ret<0 )
		{
			sprintf(acErrMsg,"sql_count error!! [%d]",ret);
			WRITEMSG
			goto ErrExit;
		}
		else if( ret==1 ) 
		{
		vtcp_log("该账号客户号为:[%s] [%d]--[%d]",__FILE__,__LINE__,prdt_ac_id.cif_no);
			ret = Netbank_cif_ukey_rel_Sel( g_pub_tx.reply,&netbank_cif_ukey_rel,"cif_no=%ld and sts='0'",prdt_ac_id.cif_no );
			if( ret==100 )
			{
				vtcp_log("该客户未签约网银或者已经解约!",prdt_ac_id.cif_no,__FILE__,__LINE__);
				set_zd_data("0670","0");
				goto OkExit;
			}
			else if( ret )
			{
				sprintf(acErrMsg,"查找出错[%d]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"D103");
				goto ErrExit;
			}
			vtcp_log("该客户存在签约网银，提示前台先销网银，再做销户!",__FILE__,__LINE__);
			set_zd_data("0670","1");
			goto OkExit;
		}
		else if( ret>1 )
		{
		vtcp_log("该客户还有多个正常状态活期账户，不需要销网银!",prdt_ac_id.cif_no,__FILE__,__LINE__);
		set_zd_data("0670","0");
		}
	}
	else
	{
	vtcp_log("该账户不为活期账户，直接正常退出!",prdt_ac_id.ac_id_type,__FILE__,__LINE__);
	set_zd_data("0670","0");
	}
	
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"Before OK return: reply is [%d]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before ERROR return: reply is[%d]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
