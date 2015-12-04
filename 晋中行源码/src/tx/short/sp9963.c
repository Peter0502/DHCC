/*************************************************
*2101的证件快捷查询
* 文 件 名: sp9963.c
* 功能描述：根据证件类型和证件号码 返回客户姓名
*			返回
* 作    者: mike
* 完成日期：2003年07月13日
* 修改记录：   
*     1. 日    期:
*        修 改 人:
*        修改内容:
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#define EXTERN
#include "public.h"
#include "cif_basic_inf_c.h"
#include "cif_id_code_rel_c.h"

sp9963()  
{ 		
	/** 数据初始化 **/
	struct cif_id_code_rel_c      cif_id_code_rel; 
	struct cif_basic_inf_c        cif_basic_inf; 
	int ret=0;

	memset (&cif_id_code_rel, 0x00, sizeof(struct cif_id_code_rel_c));
	memset (&cif_basic_inf, 0x00, sizeof(struct cif_basic_inf_c));
	
	/** 取值、赋值 **/	
	get_zd_data( "0670" , cif_id_code_rel.id_type );	/* 证件类型 */
	get_zd_data( "0620" , cif_id_code_rel.id_no );		/* 证件号码 */
	vtcp_log("[%s].........[%s]",cif_id_code_rel.id_type,cif_id_code_rel.id_no);
	
	/* 根据证件类型和证件号码取客户号 */
	ret = Cif_id_code_rel_Sel( g_pub_tx.reply , &cif_id_code_rel , 
		  "id_type='%s' and id_no='%s'",cif_id_code_rel.id_type, \
		  cif_id_code_rel.id_no );
	if( ret==100 )
	{
		sprintf(acErrMsg,"该证件信息没有登记,请到客户中心登记[%s]", \
				g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C019");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"取客户证件信息异常![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C002");
		goto ErrExit;
	}
					  
	/* 根据客户号查询客户名称 */			  
	ret = Cif_basic_inf_Sel( g_pub_tx.reply , &cif_basic_inf , 
					" cif_no=%ld " , cif_id_code_rel.cif_no );
	if( ret==100 )
	{
		sprintf(acErrMsg,"该证件对应的客户号不存在[%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C020");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"取客户基本信息异常![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C006");
		goto ErrExit;
	}

	/* 输出变量 */
	pub_base_strpack(cif_basic_inf.name);
	set_zd_data( "0250", cif_basic_inf.name );
	sprintf(g_pub_tx.ac_no,"%ld",cif_id_code_rel.cif_no);
	set_zd_data( "0300", g_pub_tx.ac_no );
	sprintf(g_pub_tx.ac_no,"%ld",cif_id_code_rel.cif_no);

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
