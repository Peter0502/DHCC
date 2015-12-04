/*************************************************
* 文 件 名: spC312.c
* 功能描述：客户授信额度维护
*
* 作    者: 
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
#include "cif_crdt_inf_c.h"
#include "cif_crdt_code_c.h"

static struct cif_crdt_inf_c		g_cif_crdt_inf;
static struct cif_crdt_inf_c		cif_crdt_inf;
static struct cif_crdt_code_c		g_cif_crdt_code;
	
char old_alias[30],tbwhere[50],memo[41];
char sts[2];

spC312()
{
	int ret=0;
	int cmp_code=0;

	/** 初始化全局变量 **/
	pub_base_sysinit();

	/** 数据初始化 **/
	memset(&cif_crdt_inf,0x00,sizeof(struct cif_crdt_inf_c));
	memset(&g_cif_crdt_inf,0x00,sizeof(struct cif_crdt_inf_c));
	memset(&g_cif_crdt_code,0x00,sizeof(struct cif_crdt_code_c));

	/** 取值、赋值 **/
	get_zd_data("0670",sts);								/*操作类型*/
	get_zd_long("0280",&g_cif_crdt_code.cif_no);			/*客户号*/
	get_zd_data("0580",g_cif_crdt_inf.credit_no);			/*合同编号*/
	get_zd_data("0680",g_cif_crdt_inf.cyc_ind);   			/*循环标志*/
	get_zd_data("0210",g_cif_crdt_inf.cur_no);  			/*币种*/
	get_zd_double("0400",&g_cif_crdt_inf.bal); 			    /*授信金额*/

	/** 处理流程 **/
	switch(sts[0])
	{
		case '1':										/*1-修改*/
				ret=Cif_crdt_inf_Dec_Upd(g_pub_tx.reply,
				 	"credit_no='%s'",g_cif_crdt_inf.credit_no);
			 	if (ret)
			 	{
					sprintf(acErrMsg,"DECLARE 游标异常![%s]",g_pub_tx.reply);
				  	WRITEMSG
					strcpy(g_pub_tx.reply,"C006");
					ERR_DEAL;
				}
				ret=Cif_crdt_inf_Fet_Upd(&cif_crdt_inf,g_pub_tx.reply);
				if (ret)
				{
				   	sprintf(acErrMsg,"FETCH Cif_crdt_inf游标异常![%s]",
							g_pub_tx.reply);
					WRITEMSG
					strcpy(g_pub_tx.reply,"C006");
					ERR_DEAL;
				}
				/* 20061010 增加rowid */
				memcpy(g_cif_crdt_inf.rowid, cif_crdt_inf.rowid,sizeof(g_cif_crdt_inf.rowid));
				ret=Cif_crdt_inf_Upd_Upd(g_cif_crdt_inf,g_pub_tx.reply);
				if (ret)
				{
    				sprintf(acErrMsg,"UPDATE Cif_crdt_inf游标异常![%s]",
							g_pub_tx.reply);
					WRITEMSG
					strcpy(g_pub_tx.reply,"C006");
					ERR_DEAL;
				}
				Cif_crdt_inf_Clo_Upd( );
				break;
		case '2':										/*2-删除*/
				/*删除删除客户授信代号对照表内容*/
				sprintf(tbwhere,"cif_no=%ld and credit_no='%s'",
						g_cif_crdt_code.cif_no,g_cif_crdt_inf.credit_no);
				ret =deldb("cif_crdt_code",tbwhere);
				if( ret )
				{
					 sprintf(acErrMsg,"删除客户授信代号对照表异常!![%s]",ret);
					 WRITEMSG
					 strcpy(g_pub_tx.reply,"C053");
					 ERR_DEAL;
				}

				memset(tbwhere,0x00,sizeof(tbwhere));

				/*删除客户授信信息表内容*/
				sprintf(tbwhere,"credit_no='%s'",g_cif_crdt_inf.credit_no);
				ret =deldb("cif_crdt_inf",tbwhere);
				if( ret )
				{
					 sprintf(acErrMsg,"删除客户授信信息表异常!![%s]",ret);
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
    sprintf(acErrMsg,"客户授信额度维护成功![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;

ErrExit:
    sprintf(acErrMsg,"客户授信额度维护失败![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}

