/*************************************************
* 文 件 名: spC306.c
* 功能描述：
		客户别名信息维护
*
* 作    者:    
* 完成日期：   2004年01月26日
* 修改记录：
*     1. 日    期:2003-05-31
*        修 改 人:andy
*        修改内容:最后一条记录不能删除
*     2.
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;}
#include "stdio.h"
#define EXTERN
#include "public.h"
#include "trace_log_c.h"
#include "cif_basic_inf_c.h"
#include "cif_alias_inf_c.h"
#include "cif_inf_chg_log_c.h"

	static struct cif_basic_inf_c		cif_basic_inf;
	static struct cif_alias_inf_c		cif_alias_inf;
	static struct cif_alias_inf_c		g_cif_alias_inf;
	static struct cif_inf_chg_log_c		cif_inf_chg_log;
	static struct cif_inf_chg_log_c		cif_inf_chg;
	
	char old_alias[30],tbwhere[50],memo[41];
	char sts[2];

spC306()
{
	int ret=0;
	int cmp_code=0;

	/** 初始化全局变量 **/
	pub_base_sysinit();

	/** 数据初始化 **/
	memset(&cif_basic_inf,0x00,sizeof(struct cif_basic_inf_c));
	memset(&cif_alias_inf,0x00,sizeof(struct cif_alias_inf_c));
	memset(&g_cif_alias_inf,0x00,sizeof(struct cif_alias_inf_c));
	memset(&cif_inf_chg_log,0x00,sizeof(struct cif_inf_chg_log_c));

	/** 取值、赋值 **/
	get_zd_long("0280",&g_cif_alias_inf.cif_no);
	get_zd_data("0670",sts);
	get_zd_long("0360",&g_cif_alias_inf.alias_seqn);
	get_zd_data("0270",old_alias);						/*原别名*/
	get_zd_data("0250",g_cif_alias_inf.alias);			/*新别名*/
	get_zd_data("0810",memo);

	/** 处理流程 **/
	switch(sts[0])
	{
		case '1':										/*1-修改*/
				/* 检查客户状态是否改变 */
				if(!strcmp(old_alias,g_cif_alias_inf.alias))
				{
					sprintf(acErrMsg,"未进行任何修改，请检查后提交![%s]",
							g_pub_tx.reply);
					WRITEMSG
					strcpy(g_pub_tx.reply,"C012");
					ERR_DEAL;
				}

				ret=Cif_alias_inf_Dec_Upd(g_pub_tx.reply,
				 	"cif_no=%ld and alias_seqn=%d",g_cif_alias_inf.cif_no,
					g_cif_alias_inf.alias_seqn);
			 	if (ret)
			 	{
					sprintf(acErrMsg,"DECLARE Cif_alias_inf_rel游标异常![%s]",
						   g_pub_tx.reply);
				  	WRITEMSG
					strcpy(g_pub_tx.reply,"C006");
					ERR_DEAL;
				}
				ret=Cif_alias_inf_Fet_Upd(&cif_alias_inf,g_pub_tx.reply);
				if (ret)
				{
				   	sprintf(acErrMsg,"FETCH Cif_alias_inf游标异常![%s]",
							g_pub_tx.reply);
					WRITEMSG
					strcpy(g_pub_tx.reply,"C006");
					ERR_DEAL;
				}
				vtcp_log("rowid==[%20s]\n",cif_alias_inf.rowid);
		memcpy(g_cif_alias_inf.rowid, cif_alias_inf.rowid,sizeof(g_cif_alias_inf.rowid));
				ret=Cif_alias_inf_Upd_Upd(g_cif_alias_inf,g_pub_tx.reply);
				if (ret)
				{
    				sprintf(acErrMsg,"UPDATE Cif_alias_inf游标异常![%s]",
							g_pub_tx.reply);
					WRITEMSG
					strcpy(g_pub_tx.reply,"C006");
					ERR_DEAL;
				}
				Cif_alias_inf_Clo_Upd( );
				break;
		case '2':										/*2-删除*/
				/*删除客户别名信息表内容*/
				sprintf(tbwhere,"cif_no=%ld and alias_seqn=%d",
						g_cif_alias_inf.cif_no,g_cif_alias_inf.alias_seqn);
				ret =deldb("cif_alias_inf",tbwhere);
				if( ret )
				{
					 sprintf(acErrMsg,"删除客户别名信息表记录异常!![%s]",
							ret);
					 WRITEMSG
					 strcpy(g_pub_tx.reply,"C053");
					 ERR_DEAL;
				}
				break;
		default:
				break;
	}

	/* 登记客户信息修改日志 */
	cif_inf_chg_log.cif_no=g_cif_alias_inf.cif_no;				/*客户ID*/
	memcpy(cif_inf_chg_log.chg_tablename,"cif_alias_inf",sizeof(cif_inf_chg_log.chg_tablename));		/*被修改的客户信息表名*/
	memcpy(cif_inf_chg_log.chg_columnname,"alias",sizeof(cif_inf_chg_log.chg_columnname));		/*被修改客户信息列名*/
	cif_inf_chg_log.chg_date=g_pub_tx.tx_date;					/*修改日期*/
	cif_inf_chg_log.chg_time=g_pub_tx.tx_time;					/*修改时间*/
	memcpy(cif_inf_chg_log.bef_content,old_alias,sizeof(cif_inf_chg_log.bef_content));				/*修改前内容*/
	memcpy(cif_inf_chg_log.aft_content,g_cif_alias_inf.alias,sizeof(cif_inf_chg_log.aft_content));	/*修改后内容*/
	memcpy(cif_inf_chg_log.clerk_no,g_pub_tx.tel,sizeof(cif_inf_chg_log.clerk_no));        		/*职员编号*/
  memcpy(cif_inf_chg_log.memo,memo,sizeof(cif_inf_chg_log.memo));   						/*更改说明*/

    ret = Cif_inf_chg_log_Ins( cif_inf_chg_log );
	if( cmp_code )
    {
         sprintf(acErrMsg,"登记客户信息修改日志失败![%d]",ret);
         WRITEMSG
         strcpy(g_pub_tx.reply,"C013");
         goto ErrExit;
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
    sprintf(acErrMsg,"客户别名信息维护成功![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;

ErrExit:
    sprintf(acErrMsg,"客户别名信息维护失败![%s]",g_pub_tx.reply);

    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}

