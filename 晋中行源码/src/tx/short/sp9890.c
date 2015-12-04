/*************************************************
* 文 件 名:  sp9890.c
* 功能描述： 客户其他金融产品信息维护快捷查询
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
#include "cif_oth_prdt_inf_c.h"

int sp9890()
{
	int ret = 0;
	int inf_no;
	long cif_no;
	char sts[2];

	struct cif_basic_inf_c g_cif_basic_inf;			
	struct cif_oth_prdt_inf_c g_cif_oth_prdt_inf;			

	/*清空*/
	memset(&g_cif_basic_inf,0x00,sizeof(struct cif_basic_inf_c));
	memset(&g_cif_oth_prdt_inf,0x00,sizeof(struct cif_oth_prdt_inf_c));

	/*初始化*/
	pub_base_sysinit();

	/*取值*/
	get_zd_data("0680",sts);				/*操作类型*/
	get_zd_long("0280",&cif_no);			/*客户号*/
	get_zd_int("0490",&inf_no);				/*信息序号*/

	/*流程*/
	/*查询客户基本信息表*/
	ret=Cif_basic_inf_Sel( g_pub_tx.reply , &g_cif_basic_inf ,
			 				"cif_no=%ld",cif_no);
	if( ret==100 )
	{
		sprintf(acErrMsg,"客户基本信息表中不存在该客户号!!cif_no=[%ld]",cif_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C051");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf( acErrMsg, "查询客户信息表异常!!ret=[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	/*查询客户其他金融产品信息表*/
	ret=Cif_oth_prdt_inf_Sel(g_pub_tx.reply,&g_cif_oth_prdt_inf,
						"cif_no=%ld and inf_no=%d",cif_no,inf_no);
	if( ret==100 )
	{
		sprintf(acErrMsg,"客户其他金融产品信息表不存在该记录!!cif_no=[%ld]",
				cif_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C072");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf( acErrMsg, "查询客户其他金融产品信息表异常!!ret=[%d]", ret );
		WRITEMSG
		strcpy(g_pub_tx.reply,"C073");
		goto ErrExit;
	}

	set_zd_data("0670",g_cif_oth_prdt_inf.unit_type);	/*机构类型*/
	set_zd_data("0910",g_cif_oth_prdt_inf.unit_no);		/*机构编码*/
	set_zd_data("0660",g_cif_oth_prdt_inf.prdt_code);	/*产品类型*/
	set_zd_data("0210",g_cif_oth_prdt_inf.pro_cur_no);	/*产品币种*/
	set_zd_data("0300",g_cif_oth_prdt_inf.ac_no);		/*产品币种*/
	set_zd_double("0390",g_cif_oth_prdt_inf.balance);		/*存贷余额*/
	set_zd_double("0400",g_cif_oth_prdt_inf.mon_pay);		/*月还款额*/

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
