/*********************************************************************************
***文 件 名:  sp1717.c                                                         ***
***功能描述：  根据卡号获得客户信息和账户余额及校验状态                        ***
***                                                                            ***
***作    者:  zhaoxingwen                                                      ***
***完成日期： 2011-11-15                                                       ***
***                                                                            ***
***记    录： 代收费系统调用了此程序，如果有人改动，请联系武大为	       ***
**********************************************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define EXTERN
#include "public.h"
#include "dd_mst_c.h"
#include "mdm_ac_rel_c.h"
#include "cif_basic_inf_c.h"
#include "cif_email_inf_c.h"
#include "cif_addr_inf_c.h"
#include "cif_per_inf_c.h"
#include "cif_wrk_hst_inf_c.h"
#include "cif_id_code_rel_c.h"
#include "mo_hold_c.h"
#include "card.h"

int sp1717()
{
	struct mdm_ac_rel_c st_mdm_ac_rel;
	struct dd_mst_c st_dd_mst;
	struct cif_basic_inf_c st_cif_basic_inf;
	struct cif_email_inf_c st_cif_email_inf;
	struct cif_addr_inf_c st_cif_addr_inf;
	struct cif_per_inf_c st_cif_per_inf;
	struct cif_wrk_hst_inf_c st_cif_wrk_hst_inf;
	struct cif_id_code_rel_c st_cif_id_code_rel;
	struct mo_hold_c st_mo_hold;
	char buf[64]={'\0'};
	char qury_lx[64]={'\0'};
	int iCif_no=0;
	int ret=0;
	
	memset(&g_pub_tx,0x00,sizeof(g_pub_tx));
	memset(&st_dd_mst,0x00,sizeof(st_dd_mst));
	memset(&st_mdm_ac_rel,0x00,sizeof(st_mdm_ac_rel));
	memset(&st_cif_basic_inf,0x00,sizeof(st_cif_basic_inf));
	memset(&st_cif_email_inf,0x00,sizeof(st_cif_email_inf));
	memset(&st_cif_addr_inf,0x00,sizeof(st_cif_addr_inf));
	memset(&st_cif_per_inf,0x00,sizeof(st_cif_per_inf));
	memset(&st_cif_wrk_hst_inf,0x00,sizeof(st_cif_wrk_hst_inf));
	memset(&st_cif_id_code_rel,0x00,sizeof(st_cif_id_code_rel));
	memset(&st_mo_hold, 0x00, sizeof(st_mo_hold));
		
	pub_base_sysinit();

	get_zd_data("0620",st_mdm_ac_rel.ac_no );		/*卡号码*/
	get_zd_data("0270",qury_lx );		/*查询类型*/
	vtcp_log( "[%s][%d] ac_no[%s],qury_lx[%s]",__FILE__,__LINE__,st_mdm_ac_rel.ac_no,qury_lx);	
		/**根据折子号找客户id**/
	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&st_mdm_ac_rel,"ac_no='%s'", st_mdm_ac_rel.ac_no);
	if(ret==100)
	{
		sprintf(acErrMsg,"无匹配的纪录!! ");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O043");
		goto ErrExit;
	}
	/*IC卡交易不在核心验密
	get_zd_data("0790",g_pub_tx.draw_pwd);
	vtcp_log( "%s,%d===g_pub_tx.draw_pwd[%s]",__FILE__,__LINE__,g_pub_tx.draw_pwd);
	get_zd_data("0660",g_pub_tx.draw_pwd_yn);
	vtcp_log( "%s,%d===g_pub_tx.draw_pwd_yn[%s]",__FILE__,__LINE__,g_pub_tx.draw_pwd_yn);
	pub_base_EnDes(g_pub_tx.tx_date,"",g_pub_tx.draw_pwd);
	if(g_pub_tx.draw_pwd_yn[0] != 'N' && 
	 strcmp(st_mdm_ac_rel.draw_pwd, g_pub_tx.draw_pwd) != 0)
	{
          sprintf(acErrMsg, "密码错误[%s]->[%s]!!",st_mdm_ac_rel.draw_pwd,g_pub_tx.draw_pwd);
          WRITEMSG
          strcpy(g_pub_tx.reply, "O123");
          goto ErrExit;
	}
        **/
	st_dd_mst.ac_id=st_mdm_ac_rel.ac_id;
	st_dd_mst.ac_seqn=st_mdm_ac_rel.ac_seqn;
	set_zd_data("0200",st_mdm_ac_rel.id_type);/*证件种类*/
	vtcp_log( "[%s][%d] 证件种类：[%s]",__FILE__,__LINE__,st_mdm_ac_rel.id_type);
	set_zd_data("0320",st_mdm_ac_rel.id_no);/*证件号码*/
	vtcp_log( "[%s][%d] 证件号码：[%s]",__FILE__,__LINE__,st_mdm_ac_rel.id_no);
	set_zd_data("0440",st_mdm_ac_rel.opn_br_no);/*开户机构号*/
	vtcp_log( "[%s][%d] 开户机构号[%s]",__FILE__,__LINE__,st_mdm_ac_rel.opn_br_no);
	/**判断若是电子现金调账交易调用此程序则不管账户状态是否正常也返回相关客户信息 20131225 hzh**/
	if(memcmp(qury_lx,"dzxjtzcx",8) != 0)
	{
		if ('0' != st_mdm_ac_rel.note_sts[0])
		{
			sprintf(acErrMsg,"介质状态不正常!! ");
			WRITEMSG
			strcpy(g_pub_tx.reply,"D140");
			goto ErrExit;
		}
  	
		if ('0' != st_mdm_ac_rel.coll_sts[0])
		{
			sprintf(acErrMsg,"介质处于捡拾状态!! ");
			WRITEMSG
			strcpy(g_pub_tx.reply,"D141");
			goto ErrExit;
		}
	}
	
	/**根据客户id找客户号**/
	ret=Dd_mst_Sel(g_pub_tx.reply,&st_dd_mst,"ac_id='%ld'  ", st_dd_mst.ac_id );
	if(ret==100)
	{
		sprintf(acErrMsg,"无匹配的纪录!! ");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O043");
		goto ErrExit;
	}
	else if(ret==0)
	{
		/**判断若是电子现金调账交易调用此程序则不管账户状态是否正常也返回相关客户信息 20131225 hzh**/
		if(memcmp(qury_lx,"dzxjtzcx",8) != 0)
		{
			if ('1' != st_dd_mst.ac_sts[0])
			{
				sprintf(acErrMsg,"账户状态不正常!! ");
				WRITEMSG
				strcpy(g_pub_tx.reply,"T999");
				goto ErrExit;
			}
			if (('1' == st_dd_mst.hold_sts[0] || '2' == st_dd_mst.hold_sts[0]) && memcmp(st_mdm_ac_rel.ac_no,ICCARDHEADCODE,6))/**对于IC卡（621780）在此处查询不管冻结状态**/
			{
				sprintf(acErrMsg,"账户处于完全冻结或只进不出状态!! ");
				WRITEMSG
				strcpy(g_pub_tx.reply,"L098");
				goto ErrExit;
			}

			if (('1' == st_dd_mst.hold_sts[0] || '2' == st_dd_mst.hold_sts[0] || '3' == st_dd_mst.hold_sts[0]) && memcmp(qury_lx,"agent_pt",8) == 0)/**对于IC卡（621780）在此处查询不管冻结状态**/
			{
				sprintf(acErrMsg,"账户处于完全冻结或只进不出状态!! ");
				WRITEMSG
				strcpy(g_pub_tx.reply,"L098");
				goto ErrExit;
			}
		}
		iCif_no=st_dd_mst.cif_no;
		vtcp_log( "[%s][%d] 客户号：[%ld]",__FILE__,__LINE__,iCif_no);
		set_zd_long("0540",iCif_no);   /**客户号**/
		set_zd_data("0260",st_dd_mst.name);/*户名*/
		/**set_zd_double("1213",st_dd_mst.bal);**/
		st_dd_mst.bal*=100 ;
		sprintf(buf,"%.0f",st_dd_mst.bal);
		set_zd_data("0330",buf);/*余额*100*/
		vtcp_log( "[%s][%d] 余额*100：[%s]",__FILE__,__LINE__,buf);
	}

	/**根据客户号到cif_basic_inf表中查询客户相关信息**/
	ret=Cif_basic_inf_Sel(g_pub_tx.reply,&st_cif_basic_inf," cif_no=%ld ",iCif_no);
	if(ret==100)
	{
		sprintf(acErrMsg,"无匹配的纪录!! ");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O043");
		goto ErrExit;
	}
	else if(ret==0)
	{
		vtcp_log( "[%s][%d] Cif_basic_inf_Sel run well[%s]",__FILE__,__LINE__);
	}
	
GoodExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before bad return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
