/*************************************************
* 文 件 名: spC109.c
* 功能描述：其他金融机构产品信息录入
*
* 作    者: andy
* 完成日期：2004年01月20日
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
#include "cif_oth_prdt_inf_c.h"

	static struct cif_basic_inf_c	cif_basic_inf;
	static struct cif_oth_prdt_inf_c	cif_oth_prdt_inf;
	static struct cif_oth_prdt_inf_c	g_cif_oth_prdt_inf;
	
spC109()
{
	int ret=0;

	/** 初始化全局变量 **/
	pub_base_sysinit();

	/** 数据初始化 **/
	memset(&cif_basic_inf,0x00,sizeof(struct cif_basic_inf_c));
	memset(&cif_oth_prdt_inf,0x00,sizeof(struct cif_oth_prdt_inf_c));
	memset(&g_cif_oth_prdt_inf,0x00,sizeof(struct cif_oth_prdt_inf_c));

	/** 取值、赋值 **/
	get_zd_long("0280",&cif_oth_prdt_inf.cif_no);		/*客户号*/
	get_zd_data("0670",cif_oth_prdt_inf.unit_type);		/*机构类型*/
	get_zd_data("0910",cif_oth_prdt_inf.unit_no);		/*机构编号*/
	get_zd_data("0660",cif_oth_prdt_inf.prdt_code);		/*产品类型*/
	get_zd_data("0210",cif_oth_prdt_inf.pro_cur_no);	/*产品币种*/
	get_zd_data("0300",cif_oth_prdt_inf.ac_no);			/*帐/卡号*/
	get_zd_double("0390",&cif_oth_prdt_inf.balance);	/*存贷余额*/
	get_zd_double("0400",&cif_oth_prdt_inf.mon_pay);	/*月还余额*/
	
	/** 处理流程 **/
	/* 检查客户号是否存在 */
	ret=Cif_basic_inf_Sel(g_pub_tx.reply,&cif_basic_inf,
						 "cif_no=%ld",cif_oth_prdt_inf.cif_no);	
	if(ret==100)
	{
		sprintf(acErrMsg,"无此客户号，请检查![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C007");
		ERR_DEAL;
	}
	else if(ret)
	{
		sprintf(acErrMsg,"读取客户基本信息异常![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C006");
		ERR_DEAL;
	}

	ret=Cif_oth_prdt_inf_Dec_Sel(g_pub_tx.reply,"cif_no=%ld order by inf_no", \
							    cif_oth_prdt_inf.cif_no);
	if( ret )
	{
		sprintf(acErrMsg,"DECLARE cif_oth_prdt_inf异常![%s]",
				g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C039");
		ERR_DEAL;
	}

	while(1)
	{
		ret =Cif_oth_prdt_inf_Fet_Sel(&g_cif_oth_prdt_inf,g_pub_tx.reply);
		if( ret==100 ) 
		{
			cif_oth_prdt_inf.inf_no=g_cif_oth_prdt_inf.inf_no+1;
			break;
		}	
		else if( ret )
		{
			sprintf(acErrMsg,"FETCH cif_oth_prdt_inf游标异常![%s]",
					g_pub_tx.reply);
			WRITEMSG
			strcpy(g_pub_tx.reply,"C038");
			ERR_DEAL;
		}
	}

	Cif_lifsty_inf_Clo_Sel ();

	/* 登记客户其他（金融机构）产品信息表 */
	ret=Cif_oth_prdt_inf_Ins(cif_oth_prdt_inf,g_pub_tx.reply);
	if(ret)
	{
		sprintf(acErrMsg,"登记客户其他产品信息表失败![%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C026");
		goto ErrExit;
	}
	
	/* 登记交易流水 */
	if(pub_ins_trace_log())
	{
		sprintf(acErrMsg,"登记交易流水错误!");
		WRITEMSG
		goto ErrExit;
	}

	set_zd_long("0350",cif_oth_prdt_inf.inf_no);

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"客户其他产品信息录入成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:	
	sprintf(acErrMsg,"客户其他产品信息录入失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

