/*************************************************
* 文 件 名:  sp9780.c
* 功能描述： 由客户号显示机构信息
*
* 作    者:  lance
* 完成日期： 2003年06月03日 
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "cif_id_code_rel_c.h"
#include "cif_basic_inf_c.h"
#include "cif_magn_inf_c.h"

	
int sp9780()
{

	int	ret;
	long cif_no;
	char cif_no_str[9];
	char cif_typ[2];
	
	struct 	cif_id_code_rel_c   g_cif_id_code_rel;	/*客户证件号码表*/
	struct  cif_basic_inf_c cif_basic_inf;
	struct  cif_magn_inf_c cif_magn_inf;
	
	/*清空*/
	memset(&g_cif_id_code_rel,0x00,sizeof(struct cif_id_code_rel_c));
	memset(&cif_basic_inf,0x00,sizeof(struct cif_basic_inf_c));
	memset(&cif_magn_inf,0x00,sizeof(struct cif_magn_inf_c));
		
	/*前台取值*/
	get_zd_long("0280",&cif_no);       				/*客户号*/

	/* 检查客户号是否已经存在 */
	ret = Cif_basic_inf_Sel( g_pub_tx.reply , &cif_basic_inf , 
						" cif_no=%ld " , cif_no );
	if( ret==100 )
	{
		sprintf(acErrMsg,"无此客户号，请检查![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C007");
		goto ErrExit;;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"读取客户基本信息异常![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C006");
		goto ErrExit;;
	}
	sprintf(acErrMsg,"检查客户号是否已经存在 PASS!");
	WRITEMSG
		
	/* 客户号检查 */
	ret = pub_cif_GetCifType( cif_no , &cif_typ );
	if (ret) goto ErrExit;
	
	/* 检查是否对公客户 */
	if ( cif_typ[0]!='3' )
	{
		sprintf(acErrMsg,"非机构客户号!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"C101");
		goto ErrExit;
	}
	
	/* 查询机构客户信息表检查是否登记过 */
   	ret=Cif_magn_inf_Sel(g_pub_tx.reply,&cif_magn_inf, "cif_no=%ld",cif_no);
   	if( ret==100 )
   	{
       		sprintf( acErrMsg,"此机构客户信息尚未登记!" );
   	  	WRITEMSG
   	  	strcpy(g_pub_tx.reply,"C103");
   		goto ErrExit;
   	}else if( ret ){
         	sprintf( acErrMsg,"读取机构客户信息表失败!" );
   	  	WRITEMSG
   		goto ErrExit; 		   		
	}
		
	sprintf(acErrMsg,"进入赋值部分");
	WRITEMSG
	
	set_zd_long("0280",cif_magn_inf.cif_no);		/* 客户号 */
	set_zd_data("0620",cif_magn_inf.units_license);		/* 代码证号 */		
	set_zd_data("0330",cif_magn_inf.ap_qualife);	/* 法人证号 */	
	set_zd_data("0810",cif_magn_inf.artificial_person);/* 事业法人名称 */				
	set_zd_data("0820",cif_magn_inf.ap_id);	   	/* 法人身份证号码 */
	set_zd_data("0580",cif_magn_inf.credit_no);		/* 贷款卡号 */
	set_zd_data("0710",cif_magn_inf.own_attri);	/* 所有制形式 */
	set_zd_data("0670",cif_magn_inf.ent_attri);		/* 企业性质 */	
	set_zd_data("0640",cif_magn_inf.credit);				/* 信誉度 */		
	set_zd_data("0250",cif_magn_inf.supervior);	/* 上级单位 */	
	set_zd_data("0260",cif_magn_inf.director);	/* 主管单位 */	
	set_zd_data("0270",cif_magn_inf.financer);	/* 财政资金调拨单位 */	
	set_zd_data("0830",cif_magn_inf.opn_bank);	/* 基本账号开户行 */	
	set_zd_double("0410",cif_magn_inf.opn_amt);	/* 开办资金 */	





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

