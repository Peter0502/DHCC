/*************************************************
* 文 件 名: spC102.c
* 功能描述：个人客户信息录入
* 作    者: andy
* 完成日期：2004年01月08日
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
#include "cif_per_inf_c.h"

	static struct cif_basic_inf_c    cif_basic_inf;
	static struct cif_per_inf_c      cif_per_inf;
	
	char technical_post[4];
	char headship[4];
	char cif_no[9];
	
spC102()  
{ 	
	int ret=0;
	
	/** 初始化全局变量 **/
	pub_base_sysinit();
	sprintf(acErrMsg,"初始化全局变量 PASS!");
	WRITEMSG	
	
	/** 数据初始化 **/
	if ( data_init_struct() )	ERR_DEAL;
	sprintf(acErrMsg,"数据初始化 PASS!");
	WRITEMSG
		
	/** 取值、赋值 **/
	if ( data_set_struct() )        ERR_DEAL;
	sprintf(acErrMsg,"取值、赋值 PASS!");
	WRITEMSG
		
	/** 处理流程 **/
	/* 检查客户号是否已经存在 */
	ret = Cif_basic_inf_Sel( g_pub_tx.reply , &cif_basic_inf , 
					" cif_no=%ld " , cif_per_inf.cif_no );
	if( ret==100 )
	{
		sprintf(acErrMsg,"无此客户号，请检查![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C007");
		ERR_DEAL;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"读取客户基本信息异常![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C006");
		ERR_DEAL;
	}
	sprintf(acErrMsg,"检查客户号是否已经存在 PASS!");
	WRITEMSG

	/* 检查客户号是否是对私客户号 */
	sprintf( cif_no , "%ld" , cif_per_inf.cif_no );
	if ( cif_no[0] != '1' )
		{
			sprintf(acErrMsg,"非对私客户号!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"C011");
			ERR_DEAL;
		}		
	sprintf(acErrMsg,"检查客户号是否是对私客户号 PASS!");
	WRITEMSG
				
	/* 登记个人客户信息表 */
	ret = Cif_per_inf_Ins( cif_per_inf , &g_pub_tx.reply );
	if( ret && ret!=-239 )
	{
		sprintf(acErrMsg,"登记个人客户信息表失败![%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C008");
		ERR_DEAL;
	}else if( ret==-239 ){
		sprintf(acErrMsg,"此客户个人信息已登记![%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C088");
		ERR_DEAL;
	}

	sprintf(acErrMsg,"登记个人客户信息表 PASS!");
	WRITEMSG
			
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
    sprintf(acErrMsg,"个人客户信息录入成功！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"个人客户信息录入失败！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
    
int data_init_struct()
{
	memset (&cif_basic_inf, 0x00, sizeof(struct cif_basic_inf_c));
	memset (&cif_per_inf, 0x00, sizeof(struct cif_per_inf_c));
	return 0;
	
}

int data_set_struct()
{    
	sprintf(acErrMsg,"进入赋值函数");
	WRITEMSG
	get_zd_long("0280",&cif_per_inf.cif_no);	/* 客户号 */
	get_zd_data("0270",cif_per_inf.name);		/* 客户尊称 */
	get_zd_long("0440",&cif_per_inf.g_bir);		/* 公历出生日期 */
	get_zd_long("0450",&cif_per_inf.n_bir);		/* 农历出生日期 */
	get_zd_data("0670",cif_per_inf.sex);		/* 客户性别 */
	get_zd_data("0680",cif_per_inf.marriage);	/* 婚姻状态 */
	get_zd_long("0290",&cif_per_inf.mar_date);	/* 结婚日期 */
	get_zd_data("0710",cif_per_inf.edu);		/* 最高学历 */		
	get_zd_data("0700",cif_per_inf.religion);	/* 宗教信仰 */		
	get_zd_data("0230",cif_per_inf.nationality);/* 国籍代码 */	
	get_zd_data("0240",cif_per_inf.country);	/* 居住国代码 */
	get_zd_data("0210",cif_per_inf.folk);		/* 民族代码 */		
	get_zd_data("0220",cif_per_inf.language);	/* 语言代码 */	
	get_zd_data("0650",cif_per_inf.clerk_no);	/* 职员编号 */				
	get_zd_data("0690",cif_per_inf.health);	   	/* 健康状况 */
		
	get_zd_data("0900",technical_post);		/* 职称 */
	sprintf(acErrMsg,"屏幕取的 [%s]",technical_post);
	WRITEMSG
	
	pub_base_strpack ( technical_post );
	sprintf(acErrMsg,"压缩后 [%s]",technical_post);
	WRITEMSG
	
	strncpy( cif_per_inf.technical_post , technical_post , 3);
	sprintf(acErrMsg,"修改成三位后 [%s]",technical_post);
	WRITEMSG
	
	get_zd_data("0890",headship);			/* 职务 */	
	pub_base_strpack ( headship );
	strncpy( cif_per_inf.headship , headship , 3);	
	
	get_zd_data("0810",cif_per_inf.birth_adr);	/* 出生地 */
	get_zd_double("0400",&cif_per_inf.mon_income);	/* 月收入 */
	get_zd_double("0410",&cif_per_inf.h_mon_income);	/* 家庭月收入 */
	
	get_zd_long("0500",&cif_per_inf.home_cnt);	/* 负担家庭成员数目 */	
	sprintf(acErrMsg,"负担家庭成员数目 [%d]",cif_per_inf.home_cnt);
	WRITEMSG
		
	return 0;
}
