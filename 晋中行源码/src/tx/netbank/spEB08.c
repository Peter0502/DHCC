/*************************************************
* 文 件 名: spEB08.c
* 功能描述：根据账号判断证件类型和证件号码是否正确，
*	   并查询企业客户信息
* 作    者: 李洋，孙朋君
* 完成日期：2010年10月23日
* 
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;}
#include "string.h"
#define EXTERN
#include "public.h"
#include "mdm_ac_rel_c.h"
#include "cif_id_code_rel_c.h"
#include "cif_cop_inf_c.h"
#include "cif_addr_inf_c.h"
#include "cif_email_inf_c.h"
#include "cif_magn_inf_c.h"
#include "cif_basic_inf_c.h"

int spEB08()
{
        /** 数据初始化 **/
        struct mdm_ac_rel_c mdm_ac_rel;
        struct cif_id_code_rel_c cif_id_code_rel;
        struct cif_cop_inf_c cif_cop_inf;
        struct cif_addr_inf_c cif_addr_inf;
        struct cif_email_inf_c cif_email_inf;
        struct cif_magn_inf_c cif_magn_inf;
        struct dd_mst_c g_dd_mst;
        struct cif_basic_inf_c cif_basic_inf;
        int    ret = 0;
        char Ac_no[33];
        char Id_type[2];
        char Id_no[21];

        memset(Ac_no, 0x00, sizeof(Ac_no));
        memset(Id_type, 0x00, sizeof(Id_type));
        memset(Id_no, 0x00, sizeof(Id_no));
        memset(&mdm_ac_rel, 0x00, sizeof(struct mdm_ac_rel_c));
        memset(&cif_id_code_rel, 0x00, sizeof(struct cif_id_code_rel_c));
        memset(&cif_cop_inf, 0x00, sizeof(struct cif_cop_inf_c));
        memset(&cif_addr_inf, 0x00, sizeof(struct cif_addr_inf_c));
        memset(&cif_email_inf, 0x00, sizeof(struct cif_email_inf_c));
        memset(&cif_magn_inf, 0x00, sizeof(struct cif_magn_inf_c));
        memset(&g_dd_mst, 0x00, sizeof(struct dd_mst_c));
        memset(&cif_basic_inf, 0x00, sizeof(struct cif_basic_inf_c));

        /** 取值、赋值 **/
        get_zd_data("0300", Ac_no);   /* 帐户号 */
        get_zd_data("0680", Id_type);   /* 企业证件种类 */
        get_zd_data("0630", Id_no);   /* 企业证件号码 */
        vtcp_log("[%s],[%d],Ac_no=[%s],Id_type=[%s],Id_no=[%s]", __FILE__,__LINE__,Ac_no,Id_type,Id_no);

	/* 根据帐户号取到账户开户网点 */
	/*ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&mdm_ac_rel,"ac_no = '%s' ", Ac_no);
        if (ret == 100) 
	{
		sprintf(acErrMsg, "该账号不存在！[%s]", g_pub_tx.reply);
		WRITEMSG
			strcpy(g_pub_tx.reply, "D104");
		goto ErrExit;
	} 
	else if (ret) 
	{
		sprintf(acErrMsg, "取账号信息异常![%s]", g_pub_tx.reply);
		WRITEMSG
			strcpy(g_pub_tx.reply, "D103");
		goto ErrExit;
	}
	set_zd_data("0910", mdm_ac_rel.opn_br_no);*/
	
	/* 判断证件类型是否正确 */
	/*if(memcmp(Id_type,mdm_ac_rel.id_type,1)!=0)
	{
		sprintf(acErrMsg, "证件类型不一致![%s]", g_pub_tx.reply);
		WRITEMSG
			strcpy(g_pub_tx.reply, "P210");
		goto ErrExit;
	}*/
	/* 证件号码是否正确 */
	/*if(memcmp(Id_no,mdm_ac_rel.id_no,strlen(Id_no))!=0)
	{
		sprintf(acErrMsg, "证件号码不一致![%s]", g_pub_tx.reply);
		WRITEMSG
			strcpy(g_pub_tx.reply, "O086");
		goto ErrExit;
	}
*/
	/* 		 */
	/*ret = Dd_mst_Sel(&g_pub_tx.reply, &g_dd_mst,"ac_id=%ld ",  mdm_ac_rel.ac_id);
	if ( ret==100 ) {
		sprintf(acErrMsg,"活期主文件中不存在该记录[%d]",ret);
		WRITEMSG
			strcpy (g_pub_tx.reply,"W014");
		return 1;
	}
	else if(ret) {
		sprintf(acErrMsg,"查询活期主文件异常[%d]",ret);
			WRITEMSG
		strcpy (g_pub_tx.reply,"W015");
		return 1;
	}
	set_zd_data("0250", g_dd_mst.name);*/

	/* 根据企业证件类型和企业证件号码查询企业客户代码 */

	ret = Cif_id_code_rel_Sel(g_pub_tx.reply, &cif_id_code_rel," id_type = '%s' and id_no = '%s'", Id_type, Id_no);
	if (ret == 100)
	{
		sprintf(acErrMsg, "客户证件不存在![%s]", g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D104");
		goto ErrExit;
	}
	if ( ret != 0)
	{
		sprintf(acErrMsg, "取客户证件信息异常![%s]", g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D103");
		goto ErrExit;
	}
	set_zd_long("0370", cif_id_code_rel.cif_no);

 /* 根据客户号查询客户名称 */
	ret = Cif_basic_inf_Sel(g_pub_tx.reply, &cif_basic_inf," cif_no=%ld ", cif_id_code_rel.cif_no);
	if (ret == 100) 
	{
		sprintf(acErrMsg, "该证件对应的客户号不存在[%s]", g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D104");
		goto ErrExit;
	} 
	else if (ret) 
	{
		sprintf(acErrMsg, "取客户基本信息异常![%s]", g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply, "D103");
		goto ErrExit;
	}
	/* 输出变量 */
	
	pub_base_strpack(cif_basic_inf.name);	
	set_zd_data("0250", cif_basic_inf.name);
	/*set_zd_long("0370",g_dd_mst.cif_no); *//**从dd_mst表中取 20101110**/
	vtcp_log("[%s],[%d],cif_basic_inf.type=[%s]", __FILE__,__LINE__,cif_basic_inf.type);
	/* 根据企业客户代码查询企业中文名称、组织机构代码和法人代表 */
	if(memcmp(cif_basic_inf.type,"2",1)==0)
	{
		ret = Cif_cop_inf_Sel(g_pub_tx.reply, &cif_cop_inf, "cif_no = %ld", cif_id_code_rel.cif_no);
		if (ret != 100 && ret != 0)
		{
			sprintf(acErrMsg, "取客户证件信息异常![%s]", g_pub_tx.reply);
			WRITEMSG
				strcpy(g_pub_tx.reply, "D103");
			goto ErrExit;
		}
			vtcp_log("[%s],[%d],cif_cop_inf.artificial_person=[%s]", __FILE__,__LINE__,cif_cop_inf.artificial_person);
			set_zd_data("0380", cif_cop_inf.crd_no);
			set_zd_data("0260", cif_cop_inf.artificial_person);
	}
	else if ((memcmp(cif_basic_inf.type,"3",1)==0 || (memcmp(cif_basic_inf.type,"4",1)==0)))
	{
		ret = Cif_magn_inf_Sel(g_pub_tx.reply, &cif_magn_inf, "cif_no = %ld", cif_id_code_rel.cif_no);
			if (ret != 100 && ret != 0)
			{
				sprintf(acErrMsg, "取客户证件信息异常![%s]", g_pub_tx.reply);
				WRITEMSG
					strcpy(g_pub_tx.reply, "D103");
				goto ErrExit;
			}
			vtcp_log("[%s],[%d],cif_magn_inf.artificial_person=[%s]", __FILE__,__LINE__,cif_magn_inf.artificial_person);
			set_zd_data("0260", cif_magn_inf.artificial_person);
	}
	/* 根据企业客户代码查询企业地址和邮政编码*/
	ret = Cif_addr_inf_Sel(g_pub_tx.reply, &cif_addr_inf, "cif_no = %ld ", cif_id_code_rel.cif_no);
	if (ret != 100 && ret != 0)
	{
		sprintf(acErrMsg, "取客户证件信息异常![%s]", g_pub_tx.reply);
		WRITEMSG
			strcpy(g_pub_tx.reply, "D103");
		goto ErrExit;
	}
	set_zd_data("0760", cif_addr_inf.addr);
	set_zd_data("0640", cif_addr_inf.post_code);
        
	/* 根据企业客户代码查询联系人和联系方式 */
	ret = Cif_email_inf_Sel(g_pub_tx.reply, &cif_email_inf, "cif_no = %ld and addr_type='2'", cif_id_code_rel.cif_no);
	if (ret != 100 && ret != 0)
	{
		sprintf(acErrMsg, "取客户证件信息异常![%s]", g_pub_tx.reply);
		WRITEMSG
			strcpy(g_pub_tx.reply, "D103");
		goto ErrExit;
	}
	set_zd_data("0590", cif_email_inf.email);
	set_zd_data("0820", cif_email_inf.link_man);


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
