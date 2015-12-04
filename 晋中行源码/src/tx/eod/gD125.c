/*************************************************************
* 文 件 名: gD125.c
* 功能描述：日终贷款昨日余额处理
**************************************************************/
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"
#include "cash_mst_c.h"
#include "ass_mst_c.h"
#include "mo_jl_bal_c.h"


gD125()
{
	int ret = 0;

    ret=sql_begin(); /*打开事务*/
    if(ret != 0)
    {
        sprintf( acErrMsg, "打开事务失败!!!" );
        WRITEMSG
        goto ErrExit;
    }

    /**------- 初始化全局变量 --------**/
    pub_base_sysinit();

    ret = pub_eod_lnmst_ysbal_upd();
    if (ret != 0)
    {
        sprintf(acErrMsg,"更新定期帐户主文件的昨日余额错误!!");
        WRITEMSG
        goto ErrExit;
    }

OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"日终昨日余额处理成功!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
    if (strcmp(g_pub_tx.reply,"0000") == 0 || g_pub_tx.reply[0]=='\0')
        strcpy(g_pub_tx.reply,"G009");
	sql_rollback();	/*--事务回滚--*/
	sprintf(acErrMsg,"日终昨日余额处理失败!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
/**********************************************************************
 * 函 数 名：   pub_eod_lnmst_ysbal_upd
 * 函数功能：   修改贷款帐户主文件的昨日余额
 * 作者/时间：  rob/20030315
********************************************************************/
int pub_eod_lnmst_ysbal_upd()
{
    int ret;
    struct ln_mst_c ln_mst_c;
	int i=0;

	vtcp_log("BEGIN LN_MST");

    memset(&ln_mst_c, 0x0, sizeof(struct ln_mst_c));

    ret = Ln_mst_Dec_Upd(g_pub_tx.reply,"1=1");
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR!! [%d]",ret);
        WRITEMSG
        return ret;
    }

    while(1)
    {
        ret = Ln_mst_Fet_Upd(&ln_mst_c,g_pub_tx.reply);
        if (ret != 0 && ret != 100)
        {
            sprintf(acErrMsg,"FETCH ERROR !! [%d]",ret);
            WRITEMSG
            return ret;
        }
        else if (ret == 100)
        {
            break;
        }

		vtcp_log("[%d-%d,%d]",++i,ln_mst_c.ac_id,ln_mst_c.ac_seqn );

		ret = pub_eod_ye_bal ( ln_mst_c.ys_bal , ln_mst_c.bal , 
			ln_mst_c.ac_id , ln_mst_c.ac_seqn);
		if(ret)		return ret;
	
        ln_mst_c.ys_bal = ln_mst_c.bal;
        ret = Ln_mst_Upd_Upd(ln_mst_c,g_pub_tx.reply);
        if (ret != 0)
        {
            sprintf(acErrMsg,"UPDATE ERROR !! [%d]",ret);
            WRITEMSG
            return ret;
        }
    }
    Ln_mst_Clo_Upd();

	vtcp_log("OVER  LN_MST");

    return 0;
}
