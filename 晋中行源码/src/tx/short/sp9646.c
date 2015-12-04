/*************************************************
* 文 件 名:    sp9646.c
* 功能描述：   
*              根据帐号回显原来关联的证件类型和证件号码
		修改后的证件类型和证件号码
*
* 作    者:    ZHGF
* 完成日期：   2008年05月16日
* insert into tx_def values ('9646','帐户关联证件号码修改','10000000000000000000000000000000000000000000000000000000000001100000001100000000000000000000000000000000000000000000000000000000','0','4','2');
* insert into tx_flow_def values ('9646',0,'9646','#$');
* insert into tx_sub_def values ('9646','帐户关联证件号码修改','sp9646','0','0');
* 修改记录：   
*     1. 日    期:
*        修 改 人:
*        修改内容:
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;}
#include "string.h"
#define EXTERN
#include "public.h"
#include "cif_id_code_rel_c.h"
#include "dd_mst_c.h"
#include "td_mst_c.h"
#include "ln_mst_c.h"
#include "mdm_ac_rel_c.h"
#include "prdt_ac_id_c.h"

int sp9646()
{
	int             ret = 0;
	long		lCif_no=0;
	char            ac_no[20];
	
	struct mdm_ac_rel_c z_mdm_ac_rel;
	struct dd_mst_c z_dd_mst;
	struct td_mst_c z_td_mst;
	struct ln_mst_c z_ln_mst;
	struct cif_id_code_rel_c z_cif_id_code_rel;
	struct prdt_ac_id_c z_prdt_ac_id;
	
	memset(ac_no	    , 0x00, sizeof(ac_no));
	memset(&z_prdt_ac_id, 0x00, sizeof(struct prdt_ac_id_c));
	memset(&z_mdm_ac_rel, 0x00, sizeof(struct mdm_ac_rel_c));
	memset(&z_dd_mst, 0x00, sizeof(struct dd_mst_c));
	memset(&z_td_mst, 0x00, sizeof(struct td_mst_c));
	memset(&z_ln_mst, 0x00, sizeof(struct ln_mst_c));
	memset(&z_cif_id_code_rel, 0x00, sizeof(struct cif_id_code_rel_c));
	
	/** 取值、赋值 **/
	get_zd_data("0300", ac_no);	/* 帐号 */
	pub_base_old_acno(ac_no);	/**对旧帐号的处理**/
	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&z_mdm_ac_rel,"ac_no='%s'",ac_no);
	if(ret)
	{
		sprintf(acErrMsg,"查找介质与账号对照表错误!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}
	ret=Prdt_ac_id_Sel(g_pub_tx.reply,&z_prdt_ac_id,"ac_id=%ld",z_mdm_ac_rel.ac_id);
	if(ret)
	{
		sprintf(acErrMsg,"查询产品账号对照表出错!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"W013");
		goto ErrExit;
	}
	vtcp_log("%s,%d z_prdt_ac_id.ac_id_type=[%s]",__FILE__,__LINE__,z_prdt_ac_id.ac_id_type);
	switch (z_prdt_ac_id.ac_id_type[0]){
	case '1':
		ret=Dd_mst_Sel(g_pub_tx.reply,&z_dd_mst,"ac_id=%ld",z_mdm_ac_rel.ac_id);
		if(ret)
		{
			sprintf(acErrMsg,"查询活期产品主文件出错!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"W015");
			goto ErrExit;
		}
		lCif_no=z_dd_mst.cif_no;
		vtcp_log("%s,%d lCif_no=[%d]",__FILE__,__LINE__,lCif_no);
		break;
	case '2':
		ret=Td_mst_Sel(g_pub_tx.reply,&z_td_mst,"ac_id=%ld",z_mdm_ac_rel.ac_id);
		if(ret)
		{
			sprintf(acErrMsg,"查询定期产品主文件出错!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"W017");
			goto ErrExit;
		}
		lCif_no=z_td_mst.cif_no;
		vtcp_log("%s,%d lCif_no=[%d]",__FILE__,__LINE__,lCif_no);
		break;
	case '3':
		ret=Ln_mst_Sel(g_pub_tx.reply,&z_ln_mst,"ac_id=%ld",z_mdm_ac_rel.ac_id);
		if(ret)
		{
			sprintf(acErrMsg,"查询贷款产品主文件出错!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"W019");
			goto ErrExit;
		}
		lCif_no=z_ln_mst.cif_no;
		vtcp_log("%s,%d lCif_no=[%d]",__FILE__,__LINE__,lCif_no);
		break;
	}
	ret=Cif_id_code_rel_Sel(g_pub_tx.reply,&z_cif_id_code_rel,"cif_no=%ld",lCif_no);
	if(ret)
	{
		sprintf(acErrMsg,"查询产品账号对照表出错!");
		WRITEMSG
		goto ErrExit;
	}
	set_zd_data("0710",z_mdm_ac_rel.id_type);
	set_zd_data("0630",z_mdm_ac_rel.id_no);
	set_zd_data("0720",z_cif_id_code_rel.id_type);
	set_zd_data("0620",z_cif_id_code_rel.id_no);
		
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
