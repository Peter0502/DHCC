/*************************************************
* 文 件 名: spC311.c
* 功能描述：
		客户其他金融产品信息维护
*
* 作    者: andy
* 完成日期：2004年01月26日
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

	static struct cif_basic_inf_c		cif_basic_inf;
	static struct cif_oth_prdt_inf_c		cif_oth_prdt_inf;
	static struct cif_oth_prdt_inf_c		g_cif_oth_prdt_inf;
	
	char old_alias[30],tbwhere[50],memo[41];
	char sts[2];

spC311()
{
	int ret=0;
	int cmp_code=0;
	double lbalance=0.0;
        double lmon_pay=0.0; 
	/** 初始化全局变量 **/
	pub_base_sysinit();

	/** 数据初始化 **/
	memset(&cif_basic_inf,0x00,sizeof(struct cif_basic_inf_c));
	memset(&cif_oth_prdt_inf,0x00,sizeof(struct cif_oth_prdt_inf_c));
	memset(&g_cif_oth_prdt_inf,0x00,sizeof(struct cif_oth_prdt_inf_c));

	/** 取值、赋值 **/
	get_zd_data("0680",sts);								/*操作类型*/
	get_zd_long("0280",&g_cif_oth_prdt_inf.cif_no);			/*客户号*/
	get_zd_long("0490",&g_cif_oth_prdt_inf.inf_no);			/*工作序号*/

	get_zd_data("0670",g_cif_oth_prdt_inf.unit_type);   	/*机构类型*/
	get_zd_data("0910",g_cif_oth_prdt_inf.unit_no);     	/*机构编码*/
	get_zd_data("0660",g_cif_oth_prdt_inf.prdt_code);   	/*产品类型*/
	get_zd_data("0210",g_cif_oth_prdt_inf.pro_cur_no);  	/*产品币种*/
	get_zd_data("0300",g_cif_oth_prdt_inf.ac_no);       	/*产品币种*/
	get_zd_double("0390",&lbalance);       /*存贷余额*/
	get_zd_double("0400",&lmon_pay);       /*月还款额*/
        vtcp_log("[%s][%d][%.2f]",__FILE__,__LINE__,lbalance);
        vtcp_log("[%s][%d][%.2f]",__FILE__,__LINE__,lmon_pay);
	/** 处理流程 **/
	switch(sts[0])
	{
		case '1':										/*1-修改*/
                             vtcp_log("hehehe1");
				ret=Cif_oth_prdt_inf_Dec_Upd(g_pub_tx.reply,
				 	"cif_no=%ld and inf_no=%d",g_cif_oth_prdt_inf.cif_no,
					g_cif_oth_prdt_inf.inf_no);

                             vtcp_log("hehehe2");
			 	if (ret)
			 	{
                             vtcp_log("hehehe3");
					sprintf(acErrMsg,"DECLARE Cif_oth_prdt_inf_rel游标异常![%s]",
						   g_pub_tx.reply);
				  	WRITEMSG
					strcpy(g_pub_tx.reply,"C006");
					ERR_DEAL;
				}
                             vtcp_log("hehehe4");
				ret=Cif_oth_prdt_inf_Fet_Upd(&g_cif_oth_prdt_inf,g_pub_tx.reply);
                             vtcp_log("hehehe5");
				if (ret)
				{
                             vtcp_log("hehehe[6]");
				   	sprintf(acErrMsg,"FETCH Cif_oth_prdt_inf游标异常![%s]",
							g_pub_tx.reply);
					WRITEMSG
					strcpy(g_pub_tx.reply,"C006");
					ERR_DEAL;
				}
				g_cif_oth_prdt_inf.balance=lbalance;
				g_cif_oth_prdt_inf.mon_pay=lmon_pay;
                             vtcp_log("hehehe[61]");
				ret=Cif_oth_prdt_inf_Upd_Upd(g_cif_oth_prdt_inf,g_pub_tx.reply);
                             vtcp_log("hehehe[62]");
				if (ret)
				{
                             vtcp_log("hehehe[63]");
    				sprintf(acErrMsg,"UPDATE Cif_oth_prdt_inf游标异常![%s]",
							g_pub_tx.reply);
					WRITEMSG
					strcpy(g_pub_tx.reply,"C006");
					ERR_DEAL;
				}
                             vtcp_log("hehehe[64]");
				Cif_oth_prdt_inf_Clo_Upd( );
                             vtcp_log("hehehe[65]");
				break;
		case '2':										/*2-删除*/
				/*删除客户其他金融产品信息表内容*/
				sprintf(tbwhere,"cif_no=%ld and inf_no=%d",
						g_cif_oth_prdt_inf.cif_no,g_cif_oth_prdt_inf.inf_no);
				ret =deldb("cif_oth_prdt_inf",tbwhere);
				if( ret )
				{
					 sprintf(acErrMsg,"删除客户其他金融产品信息表异常!![%s]",
							ret);
					 WRITEMSG
					 strcpy(g_pub_tx.reply,"C053");
					 ERR_DEAL;
				}
				break;
		default:
				break;
	}

    /* 登记交易流水 */
    if( pub_ins_trace_log() )
    {
         sprintf(acErrMsg,"登记交易流水错误!");
         WRITEMSG
         goto ErrExit;
    }

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"客户其他金融产品信息维护成功![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;

ErrExit:
    sprintf(acErrMsg,"客户其他金融产品信息维护失败![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}

