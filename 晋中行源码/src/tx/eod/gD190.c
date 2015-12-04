/************************************************************
* 文 件 名: gD190.c
* 功能描述: 检查募集到期的理财产品未达到募集金额自动解除
*
* 作    者: shangyongchao
* 完成日期: 2013/5/24 15:44:16
*
* 修改记录:
* 日    期:
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
#include "dd_mst_c.h"
#include "mdm_ac_rel_c.h"
#define LINE_PAGE 40
int gD190()
{
    int ret = 0;
    long lLine = 0;
    double dSum_amt = 0.00;
    double dSum_tx_amt = 0.00;
    char cCmd[512];
    char cFilename_fail[256];;
    FILE *fp_fail=NULL;
    struct fn_parm_c sFn_parm;
    struct fn_reg_c sFn_reg;
    struct fn_reg_hst_c sFn_reg_hst;
    struct dd_mst_c sDd_mst;

    memset(cFilename_fail,0x00,sizeof(cFilename_fail));
    memset(cCmd,0x00,sizeof(cCmd));
    memset(&sFn_parm,0x00,sizeof(sFn_parm));
    memset(&sFn_reg,0x00,sizeof(sFn_reg));
    memset(&sFn_reg_hst,0x00,sizeof(sFn_reg_hst));
    memset(&sDd_mst,0x00,sizeof(sDd_mst));

    pub_base_sysinit();

    strcpy(g_pub_tx.tx_code,"G190");
    strcpy(g_pub_tx.sub_tx_code,"G190");
    set_zd_data("096C","0100");
    strcpy(g_pub_tx.brf,"理财失败");
    strcpy(cFilename_fail,"lcfail_mx");
    sprintf(cCmd,"rm -f %s/report/%s/%s.txt",getenv("HOME"),QS_BR_NO,cFilename_fail);
    vtcp_log("rm -f %s/report/%s/%s.txt",getenv("HOME"),QS_BR_NO,cFilename_fail);
    ret=system(cCmd);
    if( ret )
    {
        vtcp_log("rm file error [%s] [%d]",cCmd,ret);
        strcpy(g_pub_tx.reply,"S035");
        goto ErrExit;
    }
    /*打开清算中心的 统计文件*/
    ret=pub_rpt_append_openfile(&fp_fail,QS_BR_NO,cFilename_fail);
    if(ret)
    {
        vtcp_log("打开文件失败!");
        strcpy(g_pub_tx.reply,"S035");
        goto ErrExit;
    }
    sql_begin();
    /*查找当天募集到期的 募集期的和暂停发售的*/
    ret=Fn_parm_Dec_Upd(g_pub_tx.reply,"(stat='0' or stat ='4') and end_date  = %ld",g_pub_tx.tx_date);
    if(ret)
    {
        vtcp_log("定义fn_parm游标失败! ret=[%d]",ret);
        strcpy(g_pub_tx.reply,"D324");
        goto ErrExit;
    }
    while(1)
    {
        lLine = 0;
        dSum_amt = 0.00;
        dSum_tx_amt = 0.00;
        memset(&sFn_parm,0x00,sizeof(sFn_parm));
        ret=Fn_parm_Fet_Upd(&sFn_parm,g_pub_tx.reply);
        if( ret==100 )
        {
            vtcp_log("fn_parm fetch over!");
            break;
        }
        else if( ret )
        {
            vtcp_log("fn_parm fetch error! ret[%d] ",ret);
            strcpy(g_pub_tx.reply,"D324");
            goto ErrExit;
        }
        ret=sql_sum_double("fn_reg","tx_amt",&dSum_amt,"prdt_code='%s' and sts='0'",sFn_parm.prdt_code);
        if( ret )
        {
            vtcp_log("统计fn_reg错误!");
            strcpy(g_pub_tx.reply, "D330");
            goto ErrExit;
        }
        vtcp_log("min_amt[%.2lf] sum[%.2lf] ",sFn_parm.min_amt,dSum_amt);
        if(pub_base_CompDblVal(dSum_amt,sFn_parm.min_amt) > 0)
        {
                    /*每个产品打一个表头*/
            fprintf(fp_fail,"\n\n\n");
            fprintf(fp_fail, "                                           理财产品[%s]募集失败处理\n",sFn_parm.prdt_code);
            fprintf(fp_fail, "                                                                                                   日期:%d年%d月%d日\n",g_pub_tx.tx_date/10000,g_pub_tx.tx_date/100%100,g_pub_tx.tx_date%100);
            fprintf(fp_fail, "======================================================================================================================\n" );
            fprintf(fp_fail, "%-6s %-20s%-20s%-17s\n","机构号","理财产品代码","账号","理财金额");
            fprintf(fp_fail, "======================================================================================================================\n" );

            ret=Fn_reg_Dec_Upd(g_pub_tx.reply,"prdt_code ='%s'  and sts='0' order by tx_br_no,ac_id",\
                    sFn_parm.prdt_code);
            if( ret )
            {
                vtcp_log(" declare cursor error! ret[%d] ",ret);
                strcpy( g_pub_tx.reply,"D332" );
                goto ErrExit;
            }
            while(1)
            {
                memset(&sFn_reg,0x00,sizeof(sFn_reg));
                ret=Fn_reg_Fet_Upd(&sFn_reg,g_pub_tx.reply);
                if( ret==100 )
                {
                    /*处理完这个产品就换页*/
                    /*处理完一个产品的 写统计信息*/
                    fprintf(fp_fail,"合计: %ld 条  合计总额: %.2lf",lLine,dSum_tx_amt);
                    fprintf(fp_fail,"\f");
                    vtcp_log("fn_reg_hst fetch over! ac_id[%ld] ",sFn_reg.ac_id);
                    break;
                }
                else if( ret )
                {
                    vtcp_log("fn_reg fetch error! ret[%d] ",ret);
                    strcpy(g_pub_tx.reply,"D330");
                    goto ErrExit;
                }
                
                if(lLine % LINE_PAGE == 0 && lLine > 0)
                {
                    fprintf(fp_fail,"\f");/*换页*/
                }
                fprintf(fp_fail, "%-6s %-20s%-20s%-17.2lf\n",sFn_reg.tx_br_no,sFn_reg.prdt_code,sFn_reg.ac_no,sFn_reg.tx_amt);
                dSum_tx_amt  += sFn_reg.tx_amt;
                lLine ++;
                memset(&sDd_mst,0x00,sizeof(sDd_mst));
                ret=Dd_mst_Dec_Upd(g_pub_tx.reply,"ac_id=%ld",sFn_reg.ac_id);
                if( ret )
                {
                    vtcp_log("定义dd_mst游标失败! ret=[%d]",ret);
                    strcpy(g_pub_tx.reply,"D226");
                    goto ErrExit;
                }
                ret=Dd_mst_Fet_Upd(&sDd_mst,g_pub_tx.reply);
                if( ret )
                {
                    vtcp_log("打开dd_mst游标失败! ret=[%d]",ret);
                    strcpy(g_pub_tx.reply,"D230");
                    goto ErrExit;
                }
                vtcp_log("clt_amt[%.2lf] tx_amt[%.2lf] ",sDd_mst.ctl_amt,sFn_reg.tx_amt);
                sDd_mst.ctl_amt-=sFn_reg.tx_amt;
                if(pub_base_CompDblVal(sDd_mst.ctl_amt,0.00) <0)
                {
                    vtcp_log( "控制金额是负了 必须检查！[%lf] ",sDd_mst.ctl_amt);
                    strcpy(g_pub_tx.reply,"S035");
                    goto ErrExit;
                }
                ret=Dd_mst_Upd_Upd(sDd_mst,g_pub_tx.reply);
                if( ret )
                {
                    vtcp_log("更新dd_mst失败! ret=[%d]",ret);
                    strcpy(g_pub_tx.reply,"D226");
                    goto ErrExit;
                }
                Dd_mst_Clo_Upd();
                
                sFn_reg.sts[0]='3';
                ret=Fn_reg_Upd_Upd(sFn_reg,g_pub_tx.reply);
                if(ret)
                {
                    vtcp_log("update fn_reg error! ret[%d]  ",ret);
                    strcpy(g_pub_tx.reply,"D332");
                    goto ErrExit;
                }
                /*登记明细*/
                sFn_reg_hst.tx_date = g_pub_tx.tx_date;
                sFn_reg_hst.trace_no = g_pub_tx.trace_no;
                strcpy(sFn_reg_hst.tx_br_no,g_pub_tx.tx_br_no);
                strcpy(sFn_reg_hst.tel,g_pub_tx.tel);
                strcpy(sFn_reg_hst.type,"3");
                sFn_reg_hst.tx_amt = sFn_reg.tx_amt;
                sFn_reg_hst.buy_date=sFn_reg.tx_date;
                sFn_reg_hst.buy_trace_no=sFn_reg.trace_no;
                strcpy(sFn_reg_hst.brf,"批量募集失败");
                ret = Fn_reg_hst_Ins(sFn_reg_hst,g_pub_tx.reply);
                if(ret)
                {
                    Fn_reg_hst_Debug(&sFn_reg_hst);
                    vtcp_log("登记理财产品登记簿错误iRet[%d]",ret);
                    strcpy(g_pub_tx.reply,"D335");
                    goto ErrExit;
                }
            }
            Fn_reg_Clo_Upd();
            sFn_parm.stat[0]='3';
            ret=Fn_parm_Upd_Upd(sFn_parm,g_pub_tx.reply);
            if(ret)
            {
                vtcp_log("更新fn_parm失败! ret=[%d]",ret);
                Fn_parm_Clo_Upd();
                strcpy(g_pub_tx.reply,"D325");
                goto ErrExit;
            }
        }
        else
        {
            continue;
        }
    }
    Fn_parm_Clo_Upd();

OkExit:
    if(fp_fail != NULL)
    {
        fclose(fp_fail);
        fp_fail = NULL;
    }
    sql_commit();     /*--提交事务--*/
    strcpy(g_pub_tx.reply, "0000");
    vtcp_log("处理成功![%s]", g_pub_tx.reply);
    set_zd_data("0120", g_pub_tx.reply);
    return 0;

ErrExit:
    if(fp_fail != NULL)
    {
        fclose(fp_fail);
        fp_fail = NULL;
    }
    sql_rollback();   /*--事务回滚--*/
    vtcp_log( "处理失败![%s]", g_pub_tx.reply);
    if(strlen(g_pub_tx.reply) == 0 || strcmp(g_pub_tx.reply,"0000") == 0)
    {
        strcpy(g_pub_tx.reply,"S035");
    }
    set_zd_data("0120", g_pub_tx.reply);
    return 1;
}
