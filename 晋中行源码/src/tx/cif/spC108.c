/*************************************************
* 文 件 名: spC108.c
* 功能描述：客户生活信息录入
*
* 作    者: andy
* 完成日期：2004年01月17日
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
#include "cif_lifsty_inf_c.h"

static struct cif_basic_inf_c	cif_basic_inf;
static struct cif_lifsty_inf_c	cif_lifsty_inf;
static struct cif_lifsty_inf_c	g_cif_lifsty_inf;

spC108()
{
	int ret=0;

	/** 初始化全局变量 **/
    pub_base_sysinit();

    /** 数据初始化 **/
    memset (&cif_basic_inf, 0x00, sizeof(struct cif_basic_inf_c));
	memset (&cif_lifsty_inf,0x00,sizeof(struct cif_lifsty_inf_c));
	memset (&g_cif_lifsty_inf,0x00,sizeof(struct cif_lifsty_inf_c));

    /** 取值、赋值 **/
	get_zd_long("0280",&cif_lifsty_inf.cif_no);		/*客户ID*/
	get_zd_data("0670",cif_lifsty_inf.life_type);	/*客户方式类别*/
	get_zd_data("0220",cif_lifsty_inf.life_detail); /*客户生活方式明细*/

	/** 处理流程 **/
	/* 检查客户号是否已经存在 */
	ret=Cif_basic_inf_Sel( g_pub_tx.reply,&cif_basic_inf,
						"cif_no=%ld",cif_lifsty_inf.cif_no);
	if( ret==100)
	{
		sprintf(acErrMsg,"无此客户号，请检查!![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C007");
		ERR_DEAL;
	}
	else if( ret )
	{
	 	sprintf(acErrMsg,"读取客户基本信息异常!![%s]",g_pub_tx.reply);
        WRITEMSG
        strcpy(g_pub_tx.reply,"C006");
        ERR_DEAL;
    }

	/*检查输入的生活方式和已有的生活方式明细是否重复*/
	ret=Cif_lifsty_inf_Dec_Sel(g_pub_tx.reply,"cif_no=%ld order by lifsty_no", \
							   cif_lifsty_inf.cif_no );
	if( ret )
	{
		sprintf(acErrMsg,"DECLARE Cif_lifsty_inf_Dec_Sel异常![%s]",
				g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C039");
		ERR_DEAL;
	}

	while(1)
	{
		ret =Cif_lifsty_inf_Fet_Sel(&g_cif_lifsty_inf,g_pub_tx.reply);
		if( ret==100 ) 
		{
			cif_lifsty_inf.lifsty_no=g_cif_lifsty_inf.lifsty_no+1;
			break;
		}	
		else if( ret )
		{
			sprintf(acErrMsg,"FETCH Cif_email_inf_Dec_Sel游标异常![%s]",
					g_pub_tx.reply);
			WRITEMSG
			strcpy(g_pub_tx.reply,"C038");
			ERR_DEAL;
		}

		pub_base_strpack(g_cif_lifsty_inf.life_type);
		pub_base_strpack(cif_lifsty_inf.life_type);
		pub_base_strpack(g_cif_lifsty_inf.life_detail);
		pub_base_strpack(cif_lifsty_inf.life_detail);

		if (!strcmp(g_cif_lifsty_inf.life_type,cif_lifsty_inf.life_type)&&!strcmp(g_cif_lifsty_inf.life_detail,cif_lifsty_inf.life_detail))
		{
			sprintf(acErrMsg,"该客户已存在这种生活方式!![%s]");
			WRITEMSG
			strcpy(g_pub_tx.reply,"C040");
			ERR_DEAL;
		}
	}

	Cif_lifsty_inf_Clo_Sel ();

	/* 登记客户生活方式信息表 */
	ret=Cif_lifsty_inf_Ins(cif_lifsty_inf);
	if( ret )
	{
		sprintf(acErrMsg,"登记客户生活方式表失败![%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C022");
		goto ErrExit;
	}

	/*登记交易流水*/
 	if( pub_ins_trace_log() )
    {
        sprintf(acErrMsg,"登记交易流水错误!");
        WRITEMSG
        goto ErrExit;
    }

	set_zd_long("0350",cif_lifsty_inf.lifsty_no);

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"客户生活方式信息录入成功！[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
    sprintf(acErrMsg,"客户生活方式信息录入失败！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}


