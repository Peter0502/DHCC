/*************************************************************
* 文 件 名: gD126.c
* 功能描述：日终其他昨日余额处理
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


gD126()
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

    ret = pub_eod_odmst_ysbal_upd();
    if (ret != 0)
    {
        sprintf(acErrMsg,"更新透支帐户主文件的昨日余额错误!!");
        WRITEMSG
        goto ErrExit;
    }

    ret = pub_eod_cashmst_ysbal_upd();
    if (ret != 0)
    {
        sprintf(acErrMsg,"更新现金台帐的昨日余额错误!!");
        WRITEMSG
        goto ErrExit;
    }

    ret = pub_eod_inmst_ysbal_upd();
    if (ret != 0)
    {
        sprintf(acErrMsg,"更新内部帐户主文件的昨日余额错误!!");
        WRITEMSG
        goto ErrExit;
    }

    ret = pub_eod_assmst_ysbal_upd();
    if (ret != 0)
    {
        sprintf(acErrMsg,"更新考核主文件的昨日余额错误!!");
        WRITEMSG
        goto ErrExit;
    }

    ret = pub_eod_notemst_ysbal_upd();
    if (ret != 0)
    {
        sprintf(acErrMsg,"更新凭证主文件的昨日余额错误!!");
        WRITEMSG
        goto ErrExit;
    }

    ret = pub_eod_submst_ysbal_upd();
    if (ret != 0)
    {
        sprintf(acErrMsg,"更新子帐号台帐的昨日余额错误!!");
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
 * 函 数 名：   pub_eod_odmst_ysbal_upd
 * 函数功能：   修改透支帐户主文件的昨日余额
 * 作者/时间：  rob/20030315
********************************************************************/
int pub_eod_odmst_ysbal_upd()
{
    int ret;
    struct od_mst_c od_mst_c;
	int i=0;

	vtcp_log("BEGIN OD_MST");

    memset(&od_mst_c, 0x0, sizeof(struct od_mst_c));

    ret = Od_mst_Dec_Upd(g_pub_tx.reply,"1=1");
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR!! [%d]",ret);
        WRITEMSG
        return ret;
    }

    while(1)
    {
        ret = Od_mst_Fet_Upd(&od_mst_c,g_pub_tx.reply);
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

		i++;
		vtcp_log("[%d-%d,%d]",i,od_mst_c.ac_id,od_mst_c.ac_seqn);

		ret = pub_eod_ye_bal ( od_mst_c.ys_bal , od_mst_c.bal , 
			od_mst_c.ac_id , od_mst_c.ac_seqn);
		if(ret)		return ret;

        od_mst_c.ys_bal = od_mst_c.bal;
        ret = Od_mst_Upd_Upd(od_mst_c,g_pub_tx.reply);
        if (ret != 0)
        {
            sprintf(acErrMsg,"UPDATE ERROR !! [%d]",ret);
            WRITEMSG
            return ret;
        }
    }
    Od_mst_Clo_Upd();

	vtcp_log("OVER  OD_MST");

    return 0;
}
/**********************************************************************
 * 函 数 名：   pub_eod_cashmst_ysbal_upd
 * 函数功能：   修改现金台帐的昨日余额
 * 作者/时间：  rob/20030315
********************************************************************/
int pub_eod_cashmst_ysbal_upd()
{
    int ret;
    struct cash_mst_c cash_mst_c;
	int i=0;

	vtcp_log( "BEGIN CASH_MST" );

    memset(&cash_mst_c, 0x0, sizeof(struct cash_mst_c));

    ret = Cash_mst_Dec_Upd(g_pub_tx.reply,"1=1");
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR!! [%d]",ret);
        WRITEMSG
        return ret;
    }

    while(1)
    {
        ret = Cash_mst_Fet_Upd(&cash_mst_c,g_pub_tx.reply);
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

		i++;
		vtcp_log("[%d-%s]",i,cash_mst_c.ac_no);
	
        cash_mst_c.ys_bal = cash_mst_c.bal;
        ret = Cash_mst_Upd_Upd(cash_mst_c,g_pub_tx.reply);
        if (ret != 0)
        {
            sprintf(acErrMsg,"UPDATE ERROR !! [%d]",ret);
            WRITEMSG
            return ret;
        }
    }
    Cash_mst_Clo_Upd();

	vtcp_log( "OVER  CASH_MST" );

    return 0;
}

/**********************************************************************
 * 函 数 名：   pub_eod_inmst_ysbal_upd
 * 函数功能：   修改内部帐户主文件的昨日余额
 * 作者/时间：  rob/20030315
********************************************************************/
int pub_eod_inmst_ysbal_upd()
{
    int ret;
    struct in_mst_c in_mst_c;
	int i=0;

	vtcp_log("BEIGN IN_MST");

    memset(&in_mst_c, 0x0, sizeof(struct in_mst_c));

    ret = In_mst_Dec_Upd(g_pub_tx.reply,"1=1");
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR!! [%d]",ret);
        WRITEMSG
        return ret;
    }

    while(1)
    {
        ret = In_mst_Fet_Upd(&in_mst_c,g_pub_tx.reply);
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

		i++;
		vtcp_log("[%d-%d,%d]",i,in_mst_c.ac_id,in_mst_c.ac_seqn);

		ret = pub_eod_ye_bal ( in_mst_c.ys_bal , in_mst_c.bal , 
			in_mst_c.ac_id , in_mst_c.ac_seqn);
		if(ret)		return ret;

        in_mst_c.ys_bal = in_mst_c.bal;
        ret = In_mst_Upd_Upd(in_mst_c,g_pub_tx.reply);
        if (ret != 0)
        {
            sprintf(acErrMsg,"UPDATE ERROR !! [%d]",ret);
            WRITEMSG
            return ret;
        }
    }
    In_mst_Clo_Upd();

	vtcp_log("BEIGN IN_MST");
    return 0;
}

/**********************************************************************
 * 函 数 名：   pub_eod_assmst_ysbal_upd
 * 函数功能：   修改考核台帐的昨日余额
 * 作者/时间：  rob/20030315
********************************************************************/
int pub_eod_assmst_ysbal_upd()
{
    int ret;
    struct ass_mst_c ass_mst_c;
	int i=0;

	vtcp_log("BEGIN ASS_MST");
    memset(&ass_mst_c, 0x0, sizeof(struct ass_mst_c));

    ret = Ass_mst_Dec_Upd(g_pub_tx.reply,"1=1");
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR!! [%d]",ret);
        WRITEMSG
        return ret;
    }

    while(1)
    {
        ret = Ass_mst_Fet_Upd(&ass_mst_c,g_pub_tx.reply);
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

		i++;
		vtcp_log("[%d-]",i);
	        
        ass_mst_c.ys_bal = ass_mst_c.bal;
        ret = Ass_mst_Upd_Upd(ass_mst_c,g_pub_tx.reply);
        if (ret != 0)
        {
            sprintf(acErrMsg,"UPDATE ERROR !! [%d]",ret);
            WRITEMSG
            return ret;
        }
    }
    Ass_mst_Clo_Upd();

	vtcp_log("OVER  ASS_MST");

    return 0;
}

/**********************************************************************
 * 函 数 名：   pub_eod_submst_ysbal_upd 
 * 函数功能：   修改子帐号台帐的昨日余额
 * 作者/时间：  rob/20030711
 *
 * 参数：
 *     输入: 
 *     输出: 
 *     返回: <>0失败
             =0成功
 *
 * 修改历史：
 *
********************************************************************/
int pub_eod_submst_ysbal_upd()
{
    int ret;
	
	vtcp_log("BEGIN SUB_DD_MST");

	ret=sql_execute("update sub_dd_mst set ys_bal=bal where 1=1");
	if( ret )
	{
            sprintf(acErrMsg,"UPDATE ERROR !! [%d]",ret);
            WRITEMSG
			strcpy(g_pub_tx.reply,"P434");
            return ret;
	}
	vtcp_log("OVER  SUB_DD_MST");

    return 0;
}
int pub_eod_notemst_ysbal_upd()
{
    int ret;

	vtcp_log("BEGIN NOTE_MST");
	ret=sql_execute("update note_mst set ys_cnt=cnt where ys_cnt!=cnt");
    if ( ret )
    {
        sprintf(acErrMsg,"更新凭证主文件的昨日余额错误!!");
        WRITEMSG
		strcpy(g_pub_tx.reply,"N019");
        return ret;
    }
	vtcp_log("OVER  NOTE_MST");
}
