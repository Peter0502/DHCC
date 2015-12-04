/*************************************************
* 文 件 名:  sp9896.c
* 功能描述： 公司客户信息维护快捷查询
*
* 作    者:  rob
* 完成日期： 2005年2月3日
*
* 修改记录： 
* 日   期:
* 修 改 人:
* 修改内容:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "cif_basic_inf_c.h"
#include "cif_cop_inf_c.h"

int sp9896()
{
	int ret = 0;
	long cif_no;
	char sts[2];
	char cif_typ[2];
	
	struct cif_basic_inf_c g_cif_basic_inf;			
	struct cif_cop_inf_c g_cif_cop_inf;			

	/*清空*/
	memset(&g_cif_basic_inf,0x00,sizeof(struct cif_basic_inf_c));
	memset(&g_cif_cop_inf,0x00,sizeof(struct cif_cop_inf_c));

	/*初始化*/
	pub_base_sysinit();

	/*取值*/
	get_zd_data("0670",sts);				/*操作类型*/
	get_zd_long("0280",&cif_no);			/*客户号*/

	/*流程*/
	/* 客户号检查 */
	ret = pub_cif_GetCifType( cif_no , &cif_typ );
	if (ret) goto ErrExit;
	
	/* 检查是否个人客户 */
	if ( cif_typ[0]!='2' )
	{
		sprintf(acErrMsg,"非公司客户号!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"C124");
		goto ErrExit;
	}	

	/*查询公司客户信息表*/
	ret=Cif_cop_inf_Sel(g_pub_tx.reply,&g_cif_cop_inf,"cif_no=%ld",cif_no);
	if( ret==100 )
	{
		sprintf(acErrMsg,"此客户尚未登记公司客户信息!!cif_no=[%ld]",cif_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C109");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf( acErrMsg, "查询公司客户信息表异常!!ret=[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	set_zd_data( "0620",g_cif_cop_inf.crd_no);          /*代码证号*/
	set_zd_data( "0630",g_cif_cop_inf.license);         /*营业执照*/
	set_zd_data( "0580",g_cif_cop_inf.credit_no);       /*贷款卡号*/
	set_zd_data( "0300",g_cif_cop_inf.units_license);   /*登记证号*/
	set_zd_data( "0310",g_cif_cop_inf.g_tax_no);   		/*国税号*/
	set_zd_data( "0320",g_cif_cop_inf.d_tax_no);   		/*地税号*/
	set_zd_data( "0660",g_cif_cop_inf.eco_attri);  		/*经济性质*/
	set_zd_data( "0670",g_cif_cop_inf.ent_attri);  		/*企业性质*/
	set_zd_data( "0220",g_cif_cop_inf.cif_scopes); 		/*所属领域*/
	set_zd_data( "0680",g_cif_cop_inf.belong_bank); 	/*归属行业(人行)*/
	set_zd_data( "0690",g_cif_cop_inf.belong_stat); 	/*归属行业(人行)*/
	set_zd_data( "0700",g_cif_cop_inf.region); 			/*地域级别*/
	set_zd_data( "0710",g_cif_cop_inf.mana_orga_moda); 	/*经营形式*/
	set_zd_data( "0720",g_cif_cop_inf.bd_corp_orga_moda);	/*企业形式*/
	set_zd_data( "0230",g_cif_cop_inf.corp_act_mode);	/*运作方式*/
	set_zd_long( "0440",g_cif_cop_inf.reg_date);		/*注册日期*/
	set_zd_data( "0890",g_cif_cop_inf.reg_country);		/*注册国家*/
	set_zd_data( "0210",g_cif_cop_inf.cur_no);			/*注册币种*/
	set_zd_double( "0400",g_cif_cop_inf.reg_fund);		/*注册资金*/
	set_zd_double( "0410",g_cif_cop_inf.paicl_up_capital);/*实收资本*/
	set_zd_data( "0330",g_cif_cop_inf.ap_qualife);		/*法人资格*/
	set_zd_data( "0810",g_cif_cop_inf.artificial_person);	/*法人名称*/
	set_zd_data( "0820",g_cif_cop_inf.ap_id);			/*法人身份证号*/
	set_zd_data( "0900",g_cif_cop_inf.corp_size);		/*企业规模*/
	set_zd_data( "0240",g_cif_cop_inf.corp_mana_sts);	/*经营状态*/
	set_zd_data( "0640",g_cif_cop_inf.credit);			/*信誉度*/
	set_zd_data( "0250",g_cif_cop_inf.supervior);		/*上级单位*/
	set_zd_data( "0260",g_cif_cop_inf.director);		/*主管单位*/

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
