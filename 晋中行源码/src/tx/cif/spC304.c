/*************************************************
* 文 件 名: spC304.c
* 功能描述：   
*           个人客户信息维护
*
* 作    者: andy
* 完成日期: 2004年02月05日
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

static struct cif_per_inf_c    	 cif_per_inf;
static struct cif_per_inf_c    	 g_cif_per_inf;
static struct cif_basic_inf_c    cif_basic_inf;

char memo[41],tbwhere[50];
char sts[2];
		
spC304()  
{ 	
	int ret=0;
	int cmp_code=0;
	
	/** 初始化全局变量 **/
	pub_base_sysinit();
	
	/** 数据初始化 **/
	memset (&cif_per_inf, 0x00, sizeof(struct cif_per_inf_c));
	memset (&g_cif_per_inf, 0x00, sizeof(struct cif_per_inf_c));
	memset (&cif_basic_inf, 0x00, sizeof(struct cif_basic_inf_c));

	/** 取值、赋值 **/
	get_zd_long("0280",&g_cif_per_inf.cif_no);			/* 客户号 */
	get_zd_data("0720",sts);							/* 操作类型 */
	get_zd_data( "0270",g_cif_per_inf.name);            /*客户名称*/
	get_zd_data( "0670",g_cif_per_inf.sex);             /*客户性别*/
	get_zd_long( "0450",&g_cif_per_inf.n_bir);          /*农历生日*/
	get_zd_long( "0440",&g_cif_per_inf.g_bir);          /*公历生日*/
	get_zd_data( "0810",g_cif_per_inf.birth_adr);       /*出生地*/
	get_zd_data( "0230",g_cif_per_inf.nationality);     /*国籍代码*/
	get_zd_data( "0210",g_cif_per_inf.folk);            /*民族代码*/
	get_zd_data( "0220",g_cif_per_inf.language);        /*语言代码*/
	get_zd_data( "0710",g_cif_per_inf.edu);             /*最高学历*/
	get_zd_data( "0890",g_cif_per_inf.headship);        /*职务*/
	get_zd_data( "0900",g_cif_per_inf.technical_post);  /*职称*/
	get_zd_data( "0650",g_cif_per_inf.clerk_no);        /*职员编号*/
	get_zd_double( "0400",&g_cif_per_inf.mon_income);   /*月收入*/
	get_zd_double( "0410",&g_cif_per_inf.h_mon_income); /*家庭月收入*/
	get_zd_long( "0500",&g_cif_per_inf.home_cnt);        /*负担家庭成员数目*/
	get_zd_long( "0290",&g_cif_per_inf.mar_date);       /*结婚日期*/
	get_zd_data( "0680",g_cif_per_inf.marriage);        /*婚姻状况*/
	get_zd_data( "0690",g_cif_per_inf.health);          /*健康状况*/
	get_zd_data( "0700",g_cif_per_inf.religion);        /*宗教信仰*/
	get_zd_data( "0240",g_cif_per_inf.country);         /*居住国家*/

	/** 处理流程 **/
	vtcp_log("sts is [%s]",sts);
	switch(sts[0])
	{
		case '1':									/*修改*/
				ret=Cif_per_inf_Dec_Upd(g_pub_tx.reply,
				   			"cif_no=%ld ",g_cif_per_inf.cif_no);
			    if (ret)
			    {
				   sprintf(acErrMsg,"DECLARE Cif_per_inf_rel游标异常![%s]",
				   g_pub_tx.reply);
				   WRITEMSG
				   strcpy(g_pub_tx.reply,"C006");
				   ERR_DEAL;
			    }
			    ret=Cif_per_inf_Fet_Upd(&cif_per_inf,g_pub_tx.reply);
			    if (ret)
			    {
				   sprintf(acErrMsg,"FETCH Cif_per_inf游标异常![%s]",
				   g_pub_tx.reply);
				   WRITEMSG
				   strcpy(g_pub_tx.reply,"C006");
				   ERR_DEAL;
			    }
				strcpy(g_cif_per_inf.rowid,cif_per_inf.rowid);
				ret=Cif_per_inf_Upd_Upd(g_cif_per_inf,g_pub_tx.reply);
				if (ret)
				{
					sprintf(acErrMsg,"UPDATE Cif_per_inf游标异常![%s]",
							g_pub_tx.reply);
					WRITEMSG
					strcpy(g_pub_tx.reply,"C006");
					ERR_DEAL;
				}
				Cif_per_inf_Clo_Upd( );
				break;
		case '2':									/*删除*/

				/*删除客户基本信息表内容*/
				sprintf(tbwhere,"cif_no=%ld",g_cif_per_inf.cif_no);
				ret =deldb("cif_per_inf",tbwhere);
				if( ret )
				{
				   sprintf(acErrMsg,"删除个人客户基本信息表记录异常!![%s]",ret);
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
	sprintf(acErrMsg,"登记交易流水 PASS!");
	WRITEMSG
		
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"个人客户基本信息维护成功！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"个人客户基本信息维护失败！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
