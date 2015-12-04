/*************************************************************
* 文 件 名: gD124.c
* 功能描述：日终定期昨日余额处理
**************************************************************/
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"

gD124()
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

    ret = pub_eod_tdmst_ysbal_upd();
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
 * 函 数 名：   pub_eod_tdmst_ysbal_upd
 * 函数功能：   修改定期帐户主文件的昨日余额
********************************************************************/
int pub_eod_tdmst_ysbal_upd()
{
    int ret;
    struct td_mst_c td_mst_c;

	vtcp_log("BEGIN TD_MST");

    memset(&td_mst_c, 0x0, sizeof(struct td_mst_c));

    ret = Td_mst_Dec_Upd(g_pub_tx.reply,"1=1");
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR!! [%d]",ret);
        WRITEMSG
        return ret;
    }

    while(1)
    {
        ret = Td_mst_Fet_Upd(&td_mst_c,g_pub_tx.reply);
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
	ret = pub_eod_ye_bal ( td_mst_c.ys_bal , td_mst_c.bal , td_mst_c.ac_id , td_mst_c.ac_seqn);
	if(ret)		return ret;
	
        td_mst_c.ys_bal = td_mst_c.bal;
        ret = Td_mst_Upd_Upd(td_mst_c,g_pub_tx.reply);
        if (ret != 0)
        {
            sprintf(acErrMsg,"UPDATE ERROR !! [%d]",ret);
            WRITEMSG
            return ret;
        }
    }
    Td_mst_Clo_Upd();

	vtcp_log("OVER  TD_MST");

    return 0;
}
