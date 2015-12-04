#include "find_debug.h" 
/*************************************************
* 文 件 名: pubf_base_InsTab.c
* 功能描述：   
*              pub_base_InsPrdt_ac_id   登记产品账号对照表
*              pub_base_InsMo_opn_cls   登记开销户登记簿
*
* 作    者:    lance
* 完成日期：   2003年01月12日
* 修改记录：   
*     1. 日    期:
*        修 改 人:
*        修改内容:
*     2. 
*************************************************/
#include <stdio.h>  
#define EXTERN
#include "public.h"
#include "prdt_ac_id_c.h"
#include "mo_opn_cls_c.h"

/**********************************************************************
* 函 数 名：  pub_base_InsPrdt_ac_id
* 函数功能：  登记产品账号对照表
* 作    者：  lance
* 完成时间：  2003年01月12日
*
* 参    数：
*     输  入：
*      		g_pub_tx结构
*     输  出: 
*
*     返回值： 0 成功
*              1 不存在
*             -1 错误
*
* 修改历史：
*                   
********************************************************************/
int pub_base_InsPrdt_ac_id( )
{
	struct prdt_ac_id_c	prdt_ac_id;
		
	int ret=0;
	
	MEMSET_DEBUG ( &prdt_ac_id , 0x00 , sizeof(struct prdt_ac_id_c) );

	/* 结构赋值 */	
	prdt_ac_id.ac_id = g_pub_tx.ac_id;	/* 账号ID */
	prdt_ac_id.ac_seqn = g_pub_tx.ac_seqn;/* 账户序号 */
	strcpy( prdt_ac_id.ac_id_type , g_pub_tx.ac_id_type );/* 账号类型1-活期；2-定期；3-贷款；4-透支；5-资金 */
	strcpy( prdt_ac_id.prdt_no , g_pub_tx.prdt_code );	/* 产品编号 */
			
	/* 登记产品账号对照表 */
	ret = Prdt_ac_id_Ins( prdt_ac_id , g_pub_tx.reply );
	if( ret )
	{
		sprintf(acErrMsg,"登记产品账号对照表失败![%d]",g_reply_int);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L025");
		return 1;
	}			
	sprintf(acErrMsg,"登记产品账号对照表 PASS!");
	WRITEMSG
	
	return 0;
		
}


/**********************************************************************
* 函 数 名：  pub_base_InsMo_opn_cls
* 函数功能：  登记开销户登记簿
* 作    者：  lance
* 完成时间：  2003年01月12日
*
* 参    数：
*     输  入：
*		name	char(50)	户名
*		acc_hrt	char(5)	科目控制字
*		wrk_sts char(1) 状态 0:申请 1:开户 2:销户
*
*     输  出: 
*		
*     返回值： 0 成功
*              1 不存在
*             -1 错误
*
* 修改历史：
*                   
********************************************************************/
int pub_base_InsMo_opn_cls( char *name , char *acc_hrt , char *wrk_sts )
{
	
	struct mo_opn_cls_c	mo_opn_cls;
	int ret=0;
	long cif_no_id;
	char cif_type_id[2];

	MEMSET_DEBUG ( &mo_opn_cls , 0x00 , sizeof(struct mo_opn_cls_c) );
	
	/* 调用根据客户号查询客户类型函数 */
	if ( g_pub_tx.ac_id_type[0]!='9')
	{
		/***如果为贴现的话客户号为0不用取客户类型**/
		if(g_pub_tx.cif_no==0)
		{
			
		}
		/*****/
		else
		{	
		ret = pub_cif_GetCifType( g_pub_tx.cif_no , cif_type_id  );
		if ( ret )
		{
			sprintf(acErrMsg,"调用根据客户号查询客户类型函数错误!");
			WRITEMSG
			return 1;	
		}
	}	
	}
	vtcp_log("[%s][%d] 客户类型==[%d]",__FILE__,__LINE__,cif_type_id);

	/* 结构赋值 */	
	strcpy ( mo_opn_cls.acc_hrt , acc_hrt );	/* 科目控制字 */

	strcpy ( mo_opn_cls.name , name );			/* 户名 */
	strcpy ( mo_opn_cls.wrk_sts , wrk_sts );	/* 状态 0:申请 1:开户 2:销户 */	
	strcpy ( mo_opn_cls.ac_no , g_pub_tx.ac_no );		/* 客户帐号 */
	strcpy ( mo_opn_cls.prdt_no , g_pub_tx.prdt_code );		/* 客户帐号 */
	mo_opn_cls.ac_id=g_pub_tx.ac_id;
	mo_opn_cls.ac_seqn = g_pub_tx.ac_seqn ;		/* 账户序号 */	
	strcpy ( mo_opn_cls.cur_no , g_pub_tx.cur_no );		/* 币种 */	
	strcpy ( mo_opn_cls.cif_type , cif_type_id );		/* 客户类型 */	
	mo_opn_cls.amt = g_pub_tx.tx_amt1 ; 			/* 金额 */
	strcpy ( mo_opn_cls.opn_br_no , g_pub_tx.opn_br_no );	/* 开户机构 */	
	strcpy ( mo_opn_cls.tx_brno , g_pub_tx.tx_br_no );	/* 发生机构 */	
	mo_opn_cls.tx_date = g_pub_tx.tx_date ;			/* 交易日期 */	
	mo_opn_cls.trace_no = g_pub_tx.trace_no ;		/* 流水号 */	
	strcpy ( mo_opn_cls.tel , g_pub_tx.tel );		/* 操作员 */	
	strcpy ( mo_opn_cls.chk , g_pub_tx.chk );		/* 复核员 */	
	strcpy ( mo_opn_cls.auth , g_pub_tx.auth );		/* 授权人 */		
	strcpy ( mo_opn_cls.ct_ind , g_pub_tx.ct_ind );	/* 现金标志1:现金 2:转帐 */							
	
	sprintf(acErrMsg,"登记开销户登记簿赋值结束!");
	WRITEMSG	
	vtcp_log("trace_no=[%d],tx_date=[%d]",g_pub_tx.trace_no,g_pub_tx.tx_date);	

	
	ret = Mo_opn_cls_Ins( mo_opn_cls , g_pub_tx.reply);
	if( ret )
	{
		sprintf(acErrMsg,"登记开销户登记簿错误![%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L027");
		return 1;
	}			
	sprintf(acErrMsg,"登记开销户登记簿 PASS!");
	WRITEMSG			
	return 0;
}

