/*************************************************
* 文 件 名: spC111.c
* 功能描述：机构客户信息录入
* 作    者: andy
* 完成日期：2004年05月28日
* 修改记录：   
*     1. 日    期:
*        修 改 人:
*        修改内容:
*     2. 
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "stdio.h" 
#define EXTERN
#include "public.h"
#include "trace_log_c.h"
#include "cif_basic_inf_c.h"
#include "cif_magn_inf_c.h"
#include "cif_id_code_rel_c.h"

	static struct cif_basic_inf_c    cif_basic_inf;
	static struct cif_magn_inf_c      cif_magn_inf;
	static struct cif_id_code_rel_c	 g_cif_id_code_rel;    

	char credit[8];
	char corp_size[4];
	char corp_act_mode[4];	
	char corp_mana_sts[4];
	char cif_no[9];
		
spC111()  
{ 	
	int ret=0;
	
	/** 初始化全局变量 **/
	pub_base_sysinit();
	sprintf(acErrMsg,"初始化全局变量 PASS!");
	WRITEMSG	
	
	/** 数据初始化 **/
	if ( data_init_struct() )	goto ErrExit;
	sprintf(acErrMsg,"数据初始化 PASS!");
	WRITEMSG
		
	/** 取值、赋值 **/
	if ( data_set_struct() )        goto ErrExit;
	sprintf(acErrMsg,"取值、赋值 PASS!");
	WRITEMSG
		
	/** 处理流程 **/
	/* 检查客户号是否已经存在 */
	ret = Cif_basic_inf_Sel( g_pub_tx.reply , &cif_basic_inf , 
						" cif_no=%ld " , cif_magn_inf.cif_no );
	if( ret==100 )
	{
		sprintf(acErrMsg,"无此客户号，请检查![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C007");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"读取客户基本信息异常![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C006");
		goto ErrExit;
	}
	sprintf(acErrMsg,"检查客户号是否已经存在 PASS!");
	WRITEMSG
	
	/* 检查客户号是否是机构客户号 */
	sprintf( cif_no , "%ld" , cif_magn_inf.cif_no );
	if ( cif_basic_inf.type[0]!='3' ) 
	{
		sprintf(acErrMsg,"非机构客户号!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"C092");
		goto ErrExit;
	}		
	sprintf(acErrMsg,"检查客户号是否是机构客户号 PASS!");
	WRITEMSG

	/* 代码证号 */	
	/**			按财务要求不登记多余的证件信息  Cif_id_code_rel 这个表只保留一条对应证件信息 wudw 20130228	
	if(strlen(cif_magn_inf.units_license))
	{
		ret=Cif_id_code_rel_Sel(g_pub_tx.reply,&g_cif_id_code_rel, \
			"cif_no=%ld and id_type='8'",cif_magn_inf.cif_no);
		if(ret==100)
		{
			g_cif_id_code_rel.cif_no=cif_magn_inf.cif_no;
			strcpy(g_cif_id_code_rel.id_type,"8");
			strcpy(g_cif_id_code_rel.id_no,cif_magn_inf.units_license);

			ret=Cif_id_code_rel_Ins(g_cif_id_code_rel,&g_pub_tx.reply);
			if( ret && ret!=-239 )
			{
				sprintf(acErrMsg,"登记客户证件与客户号对照表失败![%d]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"C009");
				goto ErrExit;
			}else if( ret==-239 ){
				sprintf(acErrMsg,"此代码证已登记,请检查![%d]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"C093");
				goto ErrExit;
			}
		}
		else if(ret)
		{
			sprintf(acErrMsg,"查询客户证件与客户号对照表失败!,ret=[%d]",ret);
			WRITEMSG
			ERR_DEAL;
		}		
	}*/
	/* 事业法人证号 */
	/*if(strlen(cif_magn_inf.ap_qualife))
	{
		ret=Cif_id_code_rel_Sel(g_pub_tx.reply,&g_cif_id_code_rel, \
			"cif_no=%ld and id_type='B'",cif_magn_inf.cif_no);
		if(ret==100)
		{
			g_cif_id_code_rel.cif_no=cif_magn_inf.cif_no;
			strcpy(g_cif_id_code_rel.id_type,"B");
			strcpy(g_cif_id_code_rel.id_no,cif_magn_inf.ap_qualife);

			ret=Cif_id_code_rel_Ins(g_cif_id_code_rel,&g_pub_tx.reply);
			if( ret && ret!=-239 )
			{
				sprintf(acErrMsg,"登记客户证件与客户号对照表失败![%d]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"C009");
				goto ErrExit;
			}else if( ret==-239 ){
				sprintf(acErrMsg,"此事业法人证已登记,请检查![%d]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"C095");
				goto ErrExit;
			}
		}
		else if(ret)
		{
			sprintf(acErrMsg,"查询客户证件与客户号对照表失败!,ret=[%d]",ret);
			WRITEMSG
			ERR_DEAL;
		}		
	}*/

	ret = Cif_magn_inf_Sel(g_pub_tx.reply,&cif_magn_inf,"cif_no=%ld", \
			cif_magn_inf.cif_no);
	if(ret==100)
	{
		/* 登记机构客户信息表 */
		ret = Cif_magn_inf_Ins( cif_magn_inf ,g_pub_tx.reply); /*******增加返回码 20131028***/
		if( ret )
		{
			sprintf(acErrMsg,"登记机构客户信息表失败![%d]",ret);
			WRITEMSG
			goto ErrExit;
		}
		sprintf(acErrMsg,"登记机构客户信息表 PASS!");
		WRITEMSG
	}else if(!ret)
	{
		sprintf(acErrMsg,"该客户已登记机构客户信息![%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C087");
		goto ErrExit;
	}

	/* 登记交易流水 */
	if( pub_ins_trace_log() )
	{
		sprintf(acErrMsg,"登记交易流水错误!");
		WRITEMSG
		goto ErrExit;
	}
	sprintf(acErrMsg,"登记交易流水 PASS!");
	WRITEMSG
		
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"机构客户信息录入成功！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"机构客户信息录入失败！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
    
int data_init_struct()
{
	memset (&cif_basic_inf, 0x00, sizeof(struct cif_basic_inf_c));
	memset (&cif_magn_inf, 0x00, sizeof(struct cif_magn_inf_c));
	memset (&g_cif_id_code_rel, 0x00, sizeof(struct cif_id_code_rel_c));
	return 0;
}

int data_set_struct()
{    
	sprintf(acErrMsg,"进入赋值函数");
	WRITEMSG
	
	get_zd_long("0280",&cif_magn_inf.cif_no);		/* 客户号 */
	get_zd_data("0620",cif_magn_inf.units_license);		/* 代码证号 */		
	get_zd_data("0330",cif_magn_inf.ap_qualife);	/* 法人证号 */	
	get_zd_data("0810",cif_magn_inf.artificial_person);/* 事业法人名称 */				
	get_zd_data("0820",cif_magn_inf.ap_id);	   	/* 法人身份证号码 */
	get_zd_data("0580",cif_magn_inf.credit_no);		/* 贷款卡号 */
	get_zd_data("0710",cif_magn_inf.own_attri);	/* 所有制形式 */
	get_zd_data("0670",cif_magn_inf.ent_attri);		/* 企业性质 */	
	get_zd_data("0640",credit);				/* 信誉度 */
	sprintf(acErrMsg,"屏幕取的 [%s]",credit);
	WRITEMSG
	
	pub_base_strpack ( credit );
	sprintf(acErrMsg,"压缩后 [%s]",credit);
	WRITEMSG
	
	strncpy( cif_magn_inf.credit , credit , 2);
	sprintf(acErrMsg,"修改成三位后 [%s]",credit);
	WRITEMSG
		
	get_zd_data("0250",cif_magn_inf.supervior);	/* 上级单位 */	
	get_zd_data("0260",cif_magn_inf.director);	/* 主管单位 */	
	get_zd_data("0270",cif_magn_inf.financer);	/* 财政资金调拨单位 */	
	get_zd_data("0830",cif_magn_inf.opn_bank);	/* 基本账号开户行 */	
	get_zd_double("0410",&cif_magn_inf.opn_amt);	/* 开办资金 */	
	return 0;
}
