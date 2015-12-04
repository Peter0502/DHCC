/*************************************************
* 文 件 名:  sp9942.c
* 功能描述： 通过客户号查询客户类型
* 作    者:  lance
* 完成日期： 2003年07月01日
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
#include "cif_per_inf_c.h"

int sp9942()
{
	int	ret=0,i=0;
	long cif_no;
	long bir_date;
	char sex[3];
	char cif_typ[2];
	char sts[2];
	
	struct 	cif_id_code_rel_c   g_cif_id_code_rel;	/*客户证件号码表*/
	struct 	cif_per_inf_c   cif_per_inf;	/*个人客户信息表*/
	
	/*清空*/
	memset(&g_cif_id_code_rel,0x00,sizeof(struct cif_id_code_rel_c));
	memset(&cif_per_inf,0x00,sizeof(struct cif_per_inf_c));	
	
	/*前台取值*/
	get_zd_long("0280",&cif_no);       				/*客户号*/

	/* 客户号检查 */
	ret = pub_cif_GetCifType( cif_no , &cif_typ );
	if (ret) goto ErrExit;
	
	/* 检查是否个人客户 */
	if ( cif_typ[0]=='1' || cif_typ[0]=='5' )
	{
		sprintf(acErrMsg,"对私客户!");
		WRITEMSG
		strcpy(sts,"1");
		set_zd_data("0660",sts);
	}/***增加返回机构客户类型 add by wanglei 20070405***/
	else if(cif_typ[0] == '3')
	{
		sprintf(acErrMsg,"机构客户!");
		WRITEMSG
		strcpy(sts,"3");
		set_zd_data("0660",sts);
	}else{
		sprintf(acErrMsg,"对公客户!");
		WRITEMSG
		strcpy(sts,"2");		
		set_zd_data("0660",sts);
	}		

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
