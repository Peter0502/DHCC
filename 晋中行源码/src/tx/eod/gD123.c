/*************************************************************
* 文 件 名: gD123.c
* 功能描述：日终活期昨日余额处理
*
* 作    者: rob
* 完成日期: 2003年3月15日
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"

gD123()
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

    ret = pub_eod_ddmst_ysbal_upd();
    if (ret != 0)
    {
        sprintf(acErrMsg,"更新活期帐户主文件的昨日余额错误!!");
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
 * 函 数 名：   pub_eod_ddmst_ysbal_upd
 * 函数功能：   修改活期帐户主文件的昨日余额
 * 作者/时间：  rob/20030315
********************************************************************/
int pub_eod_ddmst_ysbal_upd()
{
    int ret;
    struct dd_mst_c dd_mst_c;
	int i=0;

	vtcp_log("BEGIN DD_MST");

    memset(&dd_mst_c, 0x0, sizeof(struct dd_mst_c));

    ret = Dd_mst_Dec_Upd(g_pub_tx.reply,"1=1");
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR!! [%d]",ret);
        WRITEMSG
        return ret;
    }

	i=0;
    while(1)
    {
        ret = Dd_mst_Fet_Upd(&dd_mst_c,g_pub_tx.reply);
        if (ret == 100) break;
        if ( ret )
        {
            sprintf(acErrMsg,"FETCH ERROR !! [%d]",ret);
            WRITEMSG
            return ret;
        }

		i++;
		vtcp_log("[%d-%d,%d]",i,dd_mst_c.ac_id,dd_mst_c.ac_seqn);
        
		ret = pub_eod_ye_bal ( dd_mst_c.ys_bal , dd_mst_c.bal , 
			dd_mst_c.ac_id , dd_mst_c.ac_seqn);
		if(ret)		return ret;
	        
		if( pub_base_CompDblVal(dd_mst_c.ys_bal,dd_mst_c.bal) )
		{
        	dd_mst_c.ys_bal = dd_mst_c.bal;
                
        	ret = Dd_mst_Upd_Upd(dd_mst_c,g_pub_tx.reply);
        	if ( ret )
        	{
            	sprintf(acErrMsg,"UPDATE ERROR !! [%d]",ret);
            	WRITEMSG
            	return ret;
        	}
		}
    }
    Dd_mst_Clo_Upd();

	vtcp_log("OVER  DD_MST");

    return 0;
}
