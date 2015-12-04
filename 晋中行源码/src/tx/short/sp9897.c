/*************************************************
* 文 件 名:  sp9897.c
* 功能描述： 个人客户信息维护快捷查询
*
* 作    者:  rob
* 完成日期： 2003年2月3日
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
#include "cif_per_inf_c.h"

int sp9897()
{
	int ret = 0;
	long cif_no;
	char sts[2];
	char cif_typ[2];

	struct cif_basic_inf_c g_cif_basic_inf;			
	struct cif_per_inf_c g_cif_per_inf;			

	/*清空*/
	memset(&g_cif_basic_inf,0x00,sizeof(struct cif_basic_inf_c));
	memset(&g_cif_per_inf,0x00,sizeof(struct cif_per_inf_c));

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
	if ( cif_typ[0]!='1' && cif_typ[0]!='5' )
	{
		sprintf(acErrMsg,"非对私客户号!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"C011");
		goto ErrExit;
	}	

	/*查询个人客户信息表*/
	ret=Cif_per_inf_Sel(g_pub_tx.reply,&g_cif_per_inf,"cif_no=%ld",cif_no);
	if( ret==100 )
	{
		sprintf(acErrMsg,"此客户尚未登记个人客户信息!cif_no=[%ld]",cif_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C110");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf( acErrMsg, "查询个人客户信息表异常!!ret=[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	set_zd_data("0270",g_cif_per_inf.name);            	/*客户名称*/
		sprintf( acErrMsg, "AAAAAAAAAAAAA=[%s]", g_cif_per_inf.sex );
		WRITEMSG
	set_zd_data("0670",g_cif_per_inf.sex);             	/*客户性别*/
	set_zd_long("0450",g_cif_per_inf.n_bir);			/*农历生日*/	
	set_zd_long("0440",g_cif_per_inf.g_bir);			/*公历生日*/	
	set_zd_data("0810",g_cif_per_inf.birth_adr);       	/*出生地*/
	set_zd_data("0230",g_cif_per_inf.nationality);     	/*国籍代码*/
	set_zd_data("0210",g_cif_per_inf.folk);     		/*民族代码*/
	set_zd_data("0220",g_cif_per_inf.language);   		/*语言代码*/
	set_zd_data("0710",g_cif_per_inf.edu);   			/*最高学历*/
	set_zd_data("0890",g_cif_per_inf.headship);        	/*职务*/
	set_zd_data("0900",g_cif_per_inf.technical_post);  	/*职称*/
	set_zd_data("0650",g_cif_per_inf.clerk_no);  		/*职员编号*/
	set_zd_double("0400",g_cif_per_inf.mon_income);  	/*月收入*/
	set_zd_double("0410",g_cif_per_inf.h_mon_income);	/*家庭月收入*/
	set_zd_long("0500",g_cif_per_inf.home_cnt);  		/*负担家庭成员数目*/
	set_zd_long("0290",g_cif_per_inf.mar_date);			/*结婚日期*/	
	set_zd_data("0680",g_cif_per_inf.marriage);        	/*婚姻状况*/
	set_zd_data("0690",g_cif_per_inf.health);        	/*健康状况*/
	set_zd_data("0700",g_cif_per_inf.religion);      	/*宗教信仰*/
	set_zd_data("0240",g_cif_per_inf.country);      	/*居住国家*/

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
