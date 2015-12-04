/***************************************************************************
* 文 件 名：spMK06.c
* 功能描述：客户信息、账户信息查询
* 作    者：wuhaifan
* 完成日期：2015-09-24
insert into tx_def (TX_CODE, TX_NAME, BIT_MAP, LOG_IND, TX_TYPE, WRK_STS)
values ('8131', '客户信息账户信息查询', '10100000000000000001000110010000000001000011100001000000000000000110100000001000010000000000000000000000000000000000000000000000', '1', '5', '0');
insert into tx_flow_def (TX_CODE, FLOW_CODE, SUB_TX_CODE, SUB_TX_RSLT)
values ('8131', 0, 'MK06', '#$');
insert into tx_sub_def (SUB_TX_CODE, SUB_TX_NAME, TX_FUNC, ACCT_IND, CHNL_IND)
values ('MK06', '客户信息账户信息查询', 'spMK06', '0', '0');
***************************************************************************/
#include <stdio.h>
#include <string.h>
#include "public.h"
#include "mdm_ac_rel_c.h"
#include "prdt_ac_id_c.h"
#include "dd_mst_c.h"
#include "td_mst_c.h"
#include "com_branch_c.h"
#include "mob_acct_type_c.h"

int spMK06()
{
	const	char * trs_desc="网银账户验证";
	int	  ret=0;
	char	cAc_no[25]={0};
	char	cDraw_pwd[7]={0};	
	char	cType[2]={0};
	char  Id_type[2];
	char  Id_no[21];
	char  nochkpin[10];
	char  Name[60+1];
	struct mdm_ac_rel_c	g_mdm_ac_rel;
	struct prdt_ac_id_c sPrdt_ac_id;
	struct dd_mst_c	g_dd_mst;
	struct td_mst_c	g_td_mst;
	struct com_branch_c	g_com_branch;
	struct mob_acct_type_c sMob_acct_type;	/*短信银行客户账号与服务品种对应表*/
	
	memset(Name,0x00, sizeof(Name));
	memset(cAc_no,0x00, sizeof(cAc_no));
	memset(cDraw_pwd,0x00, sizeof(cDraw_pwd));
	memset(cType,0x00, sizeof(cType));
	memset(Id_type,0x00, sizeof(Id_type));
	memset(Id_no,0x00, sizeof(Id_no));
	memset(nochkpin,0x00, sizeof(nochkpin));
	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&sPrdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));
	memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
	memset(&g_td_mst,0x00,sizeof(struct td_mst_c));
	memset(&g_com_branch,0x00,sizeof(struct com_branch_c));
	memset(&sMob_acct_type,0x00,sizeof(sMob_acct_type));
	
	vtcp_log("[%s],[%d],[%s]处理开始...",__FILE__,__LINE__,trs_desc);
	pub_base_sysinit();              /*初始化公共变量*/	
	get_zd_data("0300",cAc_no);      /*账号*/
	
	pub_base_strpack(cAc_no);
	vtcp_log("[%s][%d],账号为[%s]",__FILE__,__LINE__,cAc_no);
	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",cAc_no);
	if( 100==ret ) 
	{
		sprintf(acErrMsg,"该账户不存在!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"D104");
		set_zd_data("0130","账户不存在");
		goto ErrExit;
	}
	else if( ret ) {
		sprintf(acErrMsg,"查询介质和帐户关系表错误!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"AT06");
		goto ErrExit;
	}
	
	set_zd_data("0670", g_mdm_ac_rel.note_sts);	/*介质状态*/
	set_zd_data("0770", g_mdm_ac_rel.opn_br_no);	/*机构号*/
	set_zd_data("0200", g_mdm_ac_rel.id_type);	/*证件类型*/
	set_zd_data("0380", g_mdm_ac_rel.id_no);	/*证件号*/
	
	/**活期定期区分**/
	ret = Prdt_ac_id_Sel(g_pub_tx.reply, &sPrdt_ac_id,"ac_id=%ld", g_mdm_ac_rel.ac_id);
	if( 100==ret ) 
	{
	   sprintf(acErrMsg,"该账户号不存在!");
	   WRITEMSG
	   strcpy(g_pub_tx.reply,"D104");
	   set_zd_data("0130","账户不存在");
	   goto ErrExit;
	}
	else if( ret ) 
	{
	   sprintf(acErrMsg,"查询产品账号对照表错误!");
	   WRITEMSG
	   strcpy(g_pub_tx.reply,"AT06");
	   goto ErrExit;
	 }
	if (sPrdt_ac_id.ac_id_type[0] == '1')                   /**查询活期产品!***/
	{
	     /**获取账户客户名、账户状态、账户类型**/
	     ret=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,"ac_id=%ld ",g_mdm_ac_rel.ac_id);
	     if( 100==ret ) 
	     {
	     	sprintf(acErrMsg,"该账户号不存在!");
	     	WRITEMSG
	     	strcpy(g_pub_tx.reply,"D104");
	     	set_zd_data("0130","账户不存在");
	     	goto ErrExit;
	     }
	     else if( ret ) 
	     {
	     	sprintf(acErrMsg,"查询活期存款主文件表错误!");
	     	WRITEMSG
	     	strcpy(g_pub_tx.reply,"AT06");
	     	goto ErrExit;
	     }
	     
		sprintf(acErrMsg,"[%s],[%d]Name=[%s],账户客户名=[%s],账户类型=[%s],账户凭证状态=[%s],账户冻结状态=[%s]",__FILE__,__LINE__,Name,g_dd_mst.name,g_dd_mst.prdt_no,g_dd_mst.ac_sts,g_dd_mst.hold_sts);
		WRITEMSG
		set_zd_data("0250", g_dd_mst.name);		/*客户姓名*/
		set_zd_data("0660", g_dd_mst.ac_sts);	/*账户状态*/
		set_zd_data("0690", g_dd_mst.hold_sts);	/*冻结状态*/
		set_zd_long("0530", g_dd_mst.opn_date);	/*开户日期*/
    }
	else if (sPrdt_ac_id.ac_id_type[0] == '2')               /**查询定期产品!***/
	{
	     /**获取账户客户名、账户状态、账户类型**/
	     ret=Td_mst_Sel(g_pub_tx.reply,&g_td_mst,"ac_id=%ld ",g_mdm_ac_rel.ac_id);
	     if( 100==ret ) 
	     {
	     	sprintf(acErrMsg,"该账户号不存在!");
	     	WRITEMSG
	     	strcpy(g_pub_tx.reply,"D104");
	     	set_zd_data("0130","账户不存在");
	     	goto ErrExit;
	     }
	     else if( ret ) 
	     {
	     	sprintf(acErrMsg,"查询活期存款主文件表错误!");
	     	WRITEMSG
	     	strcpy(g_pub_tx.reply,"AT06");
	     	goto ErrExit;
	     }
	    
		sprintf(acErrMsg,"[%s],[%d]Name=[%s],账户客户名=[%s],账户类型=[%s],账户凭证状态=[%s],账户冻结状态=[%s]",__FILE__,__LINE__,Name,g_td_mst.name,g_td_mst.prdt_no,g_td_mst.ac_sts,g_td_mst.hold_sts);
		WRITEMSG
		set_zd_data("0250", g_td_mst.name);		/*客户姓名*/
		set_zd_data("0660", g_td_mst.ac_sts);	/*账户状态*/
		set_zd_data("0690", g_td_mst.hold_sts);	/*冻结状态*/
		set_zd_long("0530", g_td_mst.opn_date);	/*开户日期*/
    }
    set_zd_double("0280",sPrdt_ac_id.cif_no);/**客户号**/
    set_zd_data("0240", sPrdt_ac_id.ac_id_type);	/*账户类型*/
    
	/**获取开户行行名行号**/
	ret=Com_branch_Sel(g_pub_tx.reply,&g_com_branch,"br_no='%s'",g_mdm_ac_rel.opn_br_no);
	if( 100==ret ) 
	{
		sprintf(acErrMsg,"该机构号不存在!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"D104");
		goto ErrExit;
	}
	else if( ret ) 
	{
		sprintf(acErrMsg,"查询公共机构码表错误!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"AT06");
		goto ErrExit;
	}
	sprintf(acErrMsg,"[%s],[%d]开户行号=[%s],开户行名=[%s]，账号类型=[%s]",__FILE__,__LINE__,g_com_branch.br_no,g_com_branch.br_name,sPrdt_ac_id.ac_id_type);
	WRITEMSG
	set_zd_data("0820", g_com_branch.br_name);	/*开户行名*/
	
	/*查询短信签约表*/
	ret = Mob_acct_type_Sel(g_pub_tx.reply,&sMob_acct_type,"ac_no='%s' and mob_sts='1' ",cAc_no);	
	if(ret == 100)
	{
		vtcp_log("[%s],[%d] 此客户未做过短信签约或已解约!",__FILE__,__LINE__);
		set_zd_data("0580","");
		/*goto ErrExit;*/
	}
	else if( ret )
	{
		vtcp_log("[%s][%d]查询表Mob_acct_type_Sel错误!",__FILE__,__LINE__);
	  strcpy(g_pub_tx.reply,"D104");
	  set_zd_data("0130","查询短信签约表出错");
	  goto ErrExit; 
	}
	else
	{
		vtcp_log("[%s],[%d] 检查短信银行客户账号与服务品种对应表!",__FILE__,__LINE__);  
		vtcp_log("[%s],[%d] sMob_acct_type.mobile=[%s]",__FILE__,__LINE__,sMob_acct_type.mobile);     
  	set_zd_data("0580",sMob_acct_type.mobile);
	}	
	
OkExit:
	vtcp_log("[%s],[%d],[%s]成功!",__FILE__,__LINE__,trs_desc);
	strcpy(g_pub_tx.reply, "0000");
	set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 0;

ErrExit:
	vtcp_log("[%s],[%d],[%s]失败!",__FILE__,__LINE__,trs_desc);
	set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 1;
}
