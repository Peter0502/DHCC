/*************************************************************
* 文 件 名: spD089.c
* 功能描述: 募集失败处理...前台交易直接调用.
*
* 作    者: zyl
* 完成日期: 20110825
*
* 修改记录: 改成焦作版本的失败处理 清算中心做
                全部处理

* 日    期:20130524
* 修 改 人:
* 备    注:
**************************************************************/
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"

#include "fn_parm_c.h"
#include "fn_reg_c.h"
#include "fn_reg_hst_c.h"
#include "mdm_ac_rel_c.h"
#include "dd_mst_c.h"


int spD089()
{
    int        iRet = 0;
    int        iFlag = 0;     /**标志 是否需要传文件.**/
    char       cPrdt_code[21];   /**产品编号**/
    char       cFilename[128];
    char       cTmpbuf[512];
    FILE      *fp = NULL;
    struct fn_parm_c      g_fn_parm;
    struct fn_reg_c       g_fn_reg;
    struct mdm_ac_rel_c   sMdm_ac_rel;
    struct dd_mst_c       sDd_mst;
    memset(cPrdt_code ,  0x00 , sizeof(cPrdt_code));
    memset(cFilename ,   0x00 , sizeof(cFilename));
    memset(cTmpbuf,      0x00 , sizeof(cTmpbuf));
    memset(&g_fn_parm,   0x00 , sizeof(struct fn_parm_c));
    memset(&g_fn_reg,    0x00 , sizeof(struct fn_reg_c));
    memset(&sMdm_ac_rel, 0x00 , sizeof(struct mdm_ac_rel_c));
    memset(&sDd_mst,     0x00 , sizeof(struct dd_mst_c));

    /** 初始化全局变量 **/
    pub_base_sysinit();

    /**得到交易机构,产品代码**/

    get_zd_data("0630", cPrdt_code);
    vtcp_log("prdt_code=[%s]", cPrdt_code);

    /*add by lwb 20140820 增加对机构的判断*/
    if(strcmp(g_pub_tx.tx_br_no,QS_BR_NO))
    {
	sprintf(acErrMsg,"非清算中心不允许做此交易！");
	WRITEMSG
	strcpy(g_pub_tx.reply,"P047");
	goto ErrExit;
    }
    /**生成下传全路径文件名**/
    pub_base_AllDwnFilName(cFilename);
    fp = fopen(cFilename, "w");
    if (fp == NULL)
    {
        vtcp_log("open file error [%s]", cFilename);
        strcpy(g_pub_tx.reply, "S066");
        goto ErrExit;
    }
    /**理财产品未销售成功明细单**/
    /** 姓名|账号|产品名称|金额|购买日期|返还日期 */
    fprintf( fp, "\n\n");
    fprintf( fp, "             理财募集失败处理\n");
    fprintf( fp, "             处理机构[%s]  柜员号[%s] 日期[%ld]\n",g_pub_tx.tx_br_no,g_pub_tx.tel,g_pub_tx.tx_date);
    fprintf( fp, "%-24s %-20s %-60s %-17s %-8s\n","账号","产品代码","产品名称","金额","认购日期");

    iRet = Fn_parm_Dec_Upd(g_pub_tx.reply, " prdt_code ='%s' ", cPrdt_code);
    if(iRet)
    {
        vtcp_log("定义fn_parm游标失败! iRet=[%d]", iRet);
        strcpy(g_pub_tx.reply, "D101");
        goto ErrExit;
    }
    iRet = Fn_parm_Fet_Upd(&g_fn_parm, g_pub_tx.reply);
    if(iRet == 100)
    {
        vtcp_log( "没有这个产品 iRet=[%d]", iRet);
        strcpy(g_pub_tx.reply, "D104");
        goto ErrExit;
    }
    else if(iRet)
    {
        vtcp_log( "打开fn_parm游标失败! iRet=[%d]", iRet);
        strcpy(g_pub_tx.reply, "D324");
        goto ErrExit;
    }
    /*只有募集状态和暂停发售的 能做募集失败*/
    if(g_fn_parm.stat[0] != '0' && g_fn_parm.stat[0] != '4')
    {
        vtcp_log( "理财产品状态 不能做募集失败! stat=[%s]", g_fn_parm.stat);
        strcpy(g_pub_tx.reply, "D3334");
        goto ErrExit;
    }
    /**检查产品各项状态..是否募集失败.**/
    vtcp_log("beg_date=[%ld]", g_fn_parm.beg_date);
    vtcp_log("end_date=[%ld]", g_fn_parm.end_date);
    vtcp_log("tx_date =[%ld]", g_pub_tx.tx_date);
    vtcp_log("sale_amt=[%17.2f]", g_fn_parm.sale_amt);
    /**募集失败,资金批量解控制.更新认购登记簿 fn_reg.**/
    iRet = Fn_reg_Dec_Upd(g_pub_tx.reply, "prdt_code='%s' and sts='0'", g_fn_parm.prdt_code);
    if(iRet)
    {
        vtcp_log( "定义fn_ret游标失败! iRet=[%d]", iRet);
        strcpy(g_pub_tx.reply, "D330");
        goto ErrExit;
    }
    while(1)
    {
        memset(&g_fn_reg,   0x00 , sizeof(struct fn_reg_c));
        iRet = Fn_reg_Fet_Upd(&g_fn_reg, g_pub_tx.reply);
        if(iRet == 100)
        {
            break;
        }
        else if(iRet)
        {
            vtcp_log("打开fn_reg游标失败! iRet=[%d]", iRet);
            strcpy(g_pub_tx.reply, "D330");
            goto ErrExit;
        }
        g_fn_reg.sts[0] = '3';
        iRet = Fn_reg_Upd_Upd(g_fn_reg, g_pub_tx.reply);
        if(iRet)
        {
            vtcp_log("更新fn_reg失败! iRet=[%d]", iRet);
            strcpy(g_pub_tx.reply, "D332");
            goto ErrExit;
        }
        vtcp_log("ac_id =[%ld] ac_seqn[%ld]", g_fn_reg.ac_id,g_fn_reg.ac_seqn);
        vtcp_log("tx_amt=[%17.2f]", g_fn_reg.tx_amt);
        memset(&sDd_mst,   0x00,  sizeof(struct dd_mst_c));
        iRet = Dd_mst_Dec_Upd(g_pub_tx.reply, "ac_id = %ld and ac_seqn=%d", g_fn_reg.ac_id,g_fn_reg.ac_seqn);
        if(iRet)
        {
            vtcp_log( "定义dd_mst游标失败! iRet=[%d]", iRet);
            strcpy(g_pub_tx.reply, "D226");
            goto ErrExit;
        }
        iRet = Dd_mst_Fet_Upd(&sDd_mst, g_pub_tx.reply);
        if(iRet)
        {
            vtcp_log("打开dd_mst游标失败! iRet=[%d]", iRet);
            strcpy(g_pub_tx.reply, "D230");
            goto ErrExit;
        }
        vtcp_log("ctl_amt=[%17.2f]", sDd_mst.ctl_amt);
        sDd_mst.ctl_amt -= g_fn_reg.tx_amt;
        if(pub_base_CompDblVal(sDd_mst.ctl_amt,0.00) <0)
        {
            vtcp_log("账号控制金额小于0了，必须找原因! ctl_amt=[%lf]",sDd_mst.ctl_amt);
            strcpy(g_pub_tx.reply,"P068");
            goto ErrExit;
        }
        iRet = Dd_mst_Upd_Upd(sDd_mst, g_pub_tx.reply);
        if(iRet)
        {
            vtcp_log("更新dd_mst失败! iRet=[%d]", iRet);
            strcpy(g_pub_tx.reply, "D226");
            goto ErrExit;
        }
        Dd_mst_Clo_Upd();
        /**查询ac_no**
        iRet = Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdm_ac_rel, "ac_no='%s'",g_fn_reg.ac_no);
        if (iRet == 100)
        {
            vtcp_log( "介质帐号关系不存在该帐号!");
            strcpy(g_pub_tx.reply, "P088");
            goto ErrExit;
        }
        if (iRet)
        {
            vtcp_log("取介质帐号关系表出错!");
            strcpy(g_pub_tx.reply, "P084");
            goto ErrExit;
        }
        */
        /**生成打印文件信息**/
        /** 姓名|账号|产品名称|金额|购买日期|返还日期 **/
        fprintf(fp, "%-24s %-20s %-60s %-17.2lf %-8d\n", \
                g_fn_reg.ac_no,g_fn_parm.prdt_code, g_fn_parm.title, g_fn_reg.tx_amt, g_fn_reg.tx_date);

        iFlag++;
    		 pub_mob_sendmail(  g_pub_tx.brf, "0089",g_fn_reg.ac_no,"1",g_fn_reg.tx_amt, 0.00 );
    
    }
    Fn_reg_Clo_Upd();
    g_fn_parm.stat[0] = '3';
    iRet = Fn_parm_Upd_Upd(g_fn_parm,g_pub_tx.reply);
    if (iRet)
    {
        vtcp_log("更新理财产品参数表失败!");
        strcpy(g_pub_tx.reply, "D325");
        goto ErrExit;
    }
    Fn_parm_Clo_Upd();
    fclose(fp);
    if(iFlag)
    {
        /*set_zd_data(DC_FILE_SND, "1"); 不返回文件了*/
    }

OkExit:
    strcpy(g_pub_tx.reply, "0000");
    vtcp_log( "处理成功![%s]", g_pub_tx.reply);
    set_zd_data("0120", g_pub_tx.reply);
    return 0;

ErrExit:
    vtcp_log("处理失败![%s]", g_pub_tx.reply);
    set_zd_data("0120", g_pub_tx.reply);
    return 1;
}


