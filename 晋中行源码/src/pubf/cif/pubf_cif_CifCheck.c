/**********************************************************************
 * 函 数 名：pubf_cif_CifCheck();
 * 函数功能：
 *	     pub_cif_CifCheck()		根据证件类型和证件号码取得客户号及客户类型
 *	     pub_cif_GetCifType()	根据客户号取得客户类型
 *       pub_cif_AcToCifType()      根据显示账号取得客户类型  
 *		 pub_cif_AcToIdNoType() 根据账户id 和 ac_seqn取证件类型和号码
 *		 pub_cif_Ins_Chg_inf()	登记客户信息修改登记簿
 *
 * 作者/时间：xxxxx/20031011
 * 
 * 参数：
 *     输入：	无
 *          
 *     输出: 	
 *
 *   返回值：
 *           返回0成功，
 *           返回非0失败；
 * 修改历史：
 *          
********************************************************************/
#include <stdio.h>  
#define EXTERN
#include "public.h"
#include "cif_id_code_rel_c.h"
#include "cif_basic_inf_c.h"
#include "prdt_ac_id_c.h"
#include "cif_inf_chg_log_c.h"


/**********************************************************************
 * 函 数 名：pub_cif_CifCheck();
 * 函数功能：根据证件类型和证件号码取得客户号及客户类型
 *			 
 * 作者/时间：xxxxx/20030111
 * 
 * 参数：
 *     输入：	
 *		id_type		证件类型		
 *		id_no		证件号码
 *     输出: 	
 *		cif_no		客户号
 *		cif_type	客户类型
 *
 *          	响应码
 *   返回值：
 *           返回0成功，
 *           返回非0失败；
 * 修改历史：
 *          
********************************************************************/
int pub_cif_CifCheck ( char *id_type , char *id_num , long *cif_no , char *cif_type )
{
	struct cif_id_code_rel_c      cif_id_code_rel; 
	struct cif_basic_inf_c        cif_basic_inf; 
	int ret=0;

	memset (&cif_id_code_rel, 0x00, sizeof(struct cif_id_code_rel_c));
	memset (&cif_basic_inf, 0x00, sizeof(struct cif_basic_inf_c));
	
	/* 根据证件类型和证件号码取客户号 */

	ret = Cif_id_code_rel_Sel( g_pub_tx.reply , &cif_id_code_rel , 
					" id_type='%s' and id_no='%s' " , id_type , id_num );
	if( ret==100 )
	{
		sprintf(acErrMsg,"该证件信息没有登记,请到客户中心登记[%s][%s]",id_type,id_num);
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
	*cif_no = cif_id_code_rel.cif_no ;
					  
	/* 根据客户号查询客户类型 */			  
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
	strcpy ( cif_type , cif_basic_inf.type );				  

	return 0;
ErrExit:

    return 1;
}

/**********************************************************************
 * 函 数 名：pub_cif_GetCifType();
 * 函数功能：根据客户号取得客户类型
 *			 
 * 作者/时间：xxxxx/20030112
 * 
 * 参数：
 *     输入：	cif_mo		客户号		long
 *          
 *     输出: 	
 *		cif_type	客户类型	char
 *          	响应码
 *   返回值：
 *           返回0成功，
 *           返回非0失败；
 * 修改历史：
 *          
********************************************************************/
int pub_cif_GetCifType ( long cif_no , char *cif_type )
{

	struct cif_basic_inf_c        cif_basic_inf; 
	int ret=0;

	memset (&cif_basic_inf, 0x00, sizeof(struct cif_basic_inf_c));
	sprintf(acErrMsg,"客户号[%d]",cif_no);
	WRITEMSG

						  
	/* 根据客户号查询客户类型 */			  
	ret = Cif_basic_inf_Sel( g_pub_tx.reply , &cif_basic_inf , 
					" cif_no=%ld " , cif_no );
	if( ret==100 )
	{
		sprintf(acErrMsg,"该客户号不存在[%s][%ld]",g_pub_tx.reply,cif_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C007");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"取客户基本信息异常![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C006");
		goto ErrExit;
	}					  
	strcpy ( cif_type , cif_basic_inf.type );				  

	return 0;
ErrExit:

    return 1;
}


/**********************************************************************
 * 函 数 名：pub_cif_AcToCifType();
 * 函数功能：根据显示账号取得客户类型
 *			 
 * 作者/时间：xxxxx/20030117
 * 
 * 参数：
 *     输入：	ac_no		显示账号	char
 *          
 *     输出: 	
 *		cif_type	客户类型	char
 *          	响应码
 *   返回值：
 *           返回0成功，
 *           返回非0失败；
 * 修改历史：
 *          
********************************************************************/
int pub_cif_AcToCifType ( char *ac_no , char *cif_type )
{			
	int ret=0;
	long cif_no;
						  
	memset (&g_mdm_ac_rel, 0x00, sizeof(struct mdm_ac_rel_c));
	
	/* 根据显示账号查询介质与账户关系信息 */ 
	ret = Mdm_ac_rel_Sel( g_pub_tx.reply , &g_mdm_ac_rel , 
					" ac_no='%s' " , ac_no );
	if( ret==100 )
	{
		sprintf(acErrMsg,"介质与账户关系信息中无此卡号或账号，请检查![%s]",
						g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L014");
		goto ErrExit;
	}else if( ret )
		{
			sprintf(acErrMsg,"读取介质与账户关系信息异常![%s]",g_pub_tx.reply);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L015");
			goto ErrExit;
		}
	sprintf(acErrMsg,"根据显示账号查询其介质状态 PASS!");
	WRITEMSG  

	/* 调用函数 */
	
	ret = pub_cif_CifCheck ( g_mdm_ac_rel.id_type ,g_mdm_ac_rel.id_no ,&cif_no , cif_type );
	if( ret )
		{
			sprintf(acErrMsg,"调用函数pub_cif_CifCheck错误!");
			WRITEMSG
			goto ErrExit;	
		}
	return 0;
ErrExit:

    return 1;
}

/**********************************************************************
 * 函 数 名：pub_cif_AcToIdNoType();
 * 函数功能：根据证件类型和证件号码取得客户号及客户类型
 *			 
 * 作者/时间：rob/20030728
 * 
 * 参数：
 *     输入：	
 *		ac_id		账户id
 *		ac_seqn	    账户序号
 *     输出: 	
 *		id_type		证件类型		
 *		id_no		证件号码
 *
 *          	响应码
 *   返回值：
 *           返回0成功，
 *           返回非0失败；
 * 修改历史：
 *          
********************************************************************/
int pub_cif_AcToIdNoType(long ac_id, long ac_seqn, char *id_type, char *id_no)
{
	int ret = 0;
	struct prdt_ac_id_c sPrdt_ac_id;
	struct cif_id_code_rel_c sCif_id_code_rel;

	memset(&sPrdt_ac_id, 0x0, sizeof(struct prdt_ac_id_c));
	memset(&sCif_id_code_rel, 0x0, sizeof(struct cif_id_code_rel_c));

	sprintf(acErrMsg, " CGH -------> ac_id=[%ld], ac_seqn=[%ld]", 
		ac_id, ac_seqn);
	WRITEMSG
	ret = Prdt_ac_id_Sel(g_pub_tx.reply, &sPrdt_ac_id, "ac_id=%ld and \
			ac_seqn=%ld", ac_id, ac_seqn);
	if (ret != 0 && ret != 100)
	{
		sprintf(acErrMsg, "查询产品账号对照表错误!! [%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "W013");
		return ret;
	}
	else if (ret == 100)
	{
		sprintf(acErrMsg, "产品账号对照表中不存在该记录!!");
		WRITEMSG
		strcpy(g_pub_tx.reply, "W012");
		return ret;
	}

	ret = Cif_id_code_rel_Sel(g_pub_tx.reply, &sCif_id_code_rel, 
		"cif_no=%ld", sPrdt_ac_id.cif_no);
	if (ret != 0 && ret != 100)
	{
		sprintf(acErrMsg, "客户证件与客户号对照表错误!! [%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "C002");
		return ret;
	}
	else if (ret == 100)
	{
		sprintf(acErrMsg, "客户证件与客户号对照表没有此记录!!");
		WRITEMSG
		strcpy(g_pub_tx.reply, "B115");
		return ret;
	}

	pub_base_strpack(sCif_id_code_rel.id_type);
	pub_base_strpack(sCif_id_code_rel.id_no);
	strcpy(id_type, sCif_id_code_rel.id_type);
	strcpy(id_no, sCif_id_code_rel.id_no);

	return 0;
}

/**********************************************************************
 * 函 数 名：pub_cif_Ins_Chg_inf();
 * 函数功能：登记客户信息修改登记簿
 *			 
 * 作者/时间：xxxxx/20031130
 * 
 * 参数：
 *     输入：	
 *		cif_no	integer	客户代码
 *		chg_tablename	char(20)	被修改客户信息表名
 *		chg_columnname			被修改客户信息列名
 *		bef_content	char(40)	修改前内容
 *		aft_content			修改后内容
 *		memo		char(40)	说明

 *     输出: 	
 *
 *          	响应码
 *   返回值：
 *           返回0成功，
 *           返回非0失败；
 * 修改历史：
 *          
********************************************************************/
int pub_cif_Ins_Chg_inf ( long cif_no ,char *chg_tablename ,char *chg_columnname ,char *bef_content, char *aft_content, char *memo )
{
	struct cif_inf_chg_log_c      S_cif_inf_chg_log; 
	int ret=0;

	memset (&S_cif_inf_chg_log, 0x00, sizeof(struct cif_inf_chg_log_c));
	
	S_cif_inf_chg_log.cif_no=cif_no;
	/****因做交易时出现赋值错误，下面统一由strcpy修改为使用memcpy函数  modify at 20130110****/
	memcpy( S_cif_inf_chg_log.chg_tablename,chg_tablename,sizeof(S_cif_inf_chg_log.chg_tablename));
	memcpy( S_cif_inf_chg_log.chg_columnname,chg_columnname,sizeof(S_cif_inf_chg_log.chg_columnname));
	S_cif_inf_chg_log.chg_date=g_pub_tx.tx_date;
	S_cif_inf_chg_log.chg_time=g_pub_tx.tx_time;
	memcpy( S_cif_inf_chg_log.bef_content,bef_content,sizeof(S_cif_inf_chg_log.bef_content));
	memcpy( S_cif_inf_chg_log.aft_content,aft_content,sizeof(S_cif_inf_chg_log.aft_content));	
	memcpy( S_cif_inf_chg_log.clerk_no,g_pub_tx.tel,sizeof(S_cif_inf_chg_log.clerk_no));
	memcpy( S_cif_inf_chg_log.memo,memo,sizeof(S_cif_inf_chg_log.memo));
	/****end at 20130110****/
	vtcp_log("[%s][%d] cif_no==[%ld]  S_cif_inf_chg_log.cif_no==[%ld]",__FILE__,__LINE__,cif_no,S_cif_inf_chg_log.cif_no);
	vtcp_log("[%s][%d] CHG_TABLENAME==[%s]  S_cif_inf_chg_log.chg_tablename==[%s]",__FILE__,__LINE__,chg_tablename,S_cif_inf_chg_log.chg_tablename);
	vtcp_log("[%s][%d] CHG_COLUNMNAME==[%s] S_cif_inf_chg_log.chg_columnname==[%s]",__FILE__,__LINE__,chg_columnname,S_cif_inf_chg_log.chg_columnname);
	vtcp_log("[%s][%d] BEF_CONTENT==[%s]  S_cif_inf_chg_log.bef_content==[%s]",__FILE__,__LINE__,bef_content,S_cif_inf_chg_log.bef_content);
	vtcp_log("[%s][%d] ATF_CONTENT==[%s]  S_cif_inf_chg_log.aft_content==[%s]",__FILE__,__LINE__,aft_content,S_cif_inf_chg_log.aft_content);
	vtcp_log("[%s][%d] clerk_no==[%s]",__FILE__,__LINE__,g_pub_tx.tel);
	vtcp_log("[%s][%d] S_cif_inf_chg_log.clerk_no==[%s]",__FILE__,__LINE__,S_cif_inf_chg_log.clerk_no);

	ret = Cif_inf_chg_log_Ins( S_cif_inf_chg_log , g_pub_tx.reply );
	if( ret )
	{
		Cif_inf_chg_log_Debug(&S_cif_inf_chg_log);
		sprintf(acErrMsg,"登记Cif_inf_chg_log_Ins信息异常![%s]",g_pub_tx.reply);
		WRITEMSG
		goto ErrExit;
	}			  

	return 0;
ErrExit:

    return 1;
}
