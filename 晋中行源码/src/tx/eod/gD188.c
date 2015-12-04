/*************************************************************
* 文 件 名: gD188.c
* 功能描述: 理财募集结束扣款
*
* 作    者: shangyongchao
* 完成日期: 2013/5/23 16:07:35
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
#define LINE_PAGE 40
int gD188()
{
    int ret,iPage,iFail_Cnt,iPage_qs;
    long lLine = 0;
    long lLine_qs = 0;
    long lCnt_qs = 0;
    char cFilename[256],cCmd[512],cFilename_fail[256];;
    char cOld_br_no[6];
    char cAc_no[25];/*扣款账号*/
    char cName[61];
    double dSum_amt=0.00; /* 产品认购金额统计 */
    double dTx_amt_jg;/*统计单个机构的某个产品的认购金额*/
    double dTx_amt;/*统计总行的认购金额*/
    FILE * fp=NULL;
    FILE *fp_fail = NULL;
    FILE *fp_qs=NULL;
    struct fn_parm_c sFn_parm;
    struct fn_reg_c sFn_reg;
    struct fn_reg_hst_c sFn_reg_hst;
    struct dd_mst_c sDd_mst;
    struct mdm_ac_rel_c sMdm_ac_rel;

    ret=iPage=lLine=iFail_Cnt=iPage_qs=0;
    memset(cFilename_fail,0x00,sizeof(cFilename_fail));
    memset(cFilename,0x00,sizeof(cFilename));
    memset(cCmd,0x00,sizeof(cCmd));
    memset(cOld_br_no,0x00,sizeof(cOld_br_no));
    memset(&sFn_parm,0x00,sizeof(sFn_parm));
    memset(&sFn_reg,0x00,sizeof(sFn_reg));
    memset(&sFn_reg_hst,0x00,sizeof(sFn_reg_hst));
    memset(&sDd_mst,0x00,sizeof(sDd_mst));
    memset(&sMdm_ac_rel,0x00,sizeof(sMdm_ac_rel));

    pub_base_sysinit();

    strcpy(g_pub_tx.tx_code,"G188");
    strcpy(g_pub_tx.sub_tx_code,"G188");
    set_zd_data("096C","0100");
    /*set_zd_data("096M",YW_TYPE_ZC);by wy*/
    strcpy(g_pub_tx.brf,"理财划款");
    strcpy(cFilename,"lckk");    /* 理财扣款 */
    /* 删除文件 */
    sprintf(cCmd,"rm -f %s/report/%s/%s.txt",getenv("HOME"),"*",cFilename);
    vtcp_log(" rm -f %s/report/%s/%s.txt",getenv("HOME"),"*",cFilename);
    ret=system(cCmd);
    if( ret )
    {
        vtcp_log("rm file error [%s] [%d]",cCmd,ret);
        strcpy(g_pub_tx.reply,"S035");
        goto ErrExit;
    }
    strcpy(cFilename_fail,"lcfail");
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
    ret=pub_rpt_append_openfile(&fp_qs,QS_BR_NO,cFilename);
    if(ret)
    {
        vtcp_log("打开文件失败!");
        strcpy(g_pub_tx.reply,"S035");
        goto ErrExit;
    }
    sql_begin();
    /*查找当天募集到期的*/
    /*
    ret=Fn_parm_Dec_Upd(g_pub_tx.reply,"stat='0' and end_date  = %ld",g_pub_tx.tx_date);
    */
    /*找宣布成立的扣款*/
    ret=Fn_parm_Dec_Upd(g_pub_tx.reply,"stat='5' order by prdt_code");
    if(ret)
    {
        vtcp_log("定义fn_parm游标失败! ret=[%d]",ret);
        strcpy(g_pub_tx.reply,"D324");
        goto ErrExit;
    }
    while(1)
    {
        dTx_amt_jg = 0.00;
        lLine = 0;
        iPage = 0;
        lLine_qs = 0;
        dTx_amt = 0.00;
        lCnt_qs = 0;
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

        vtcp_log("end_date[%ld] ",sFn_parm.end_date);
        /* 取主机流水号 */
        if( pub_base_PubQlsh(&g_pub_tx.trace_no,g_pub_tx.reply) )
        {
            vtcp_log("取主机流水号错 [%d]", g_pub_tx.trace_no);
            goto ErrExit;
        }
        set_zd_long(DC_TRACE_NO,g_pub_tx.trace_no);
        g_pub_tx.trace_cnt = 0;
        set_zd_long(DC_TRACE_CNT,g_pub_tx.trace_cnt);
        memset(cOld_br_no,0x00,sizeof(cOld_br_no));

        /*不需要统计了，行里宣布成立 就扣款
        ret=sql_sum_double("fn_reg","tx_amt",&dSum_amt,"prdt_code='%s' and sts='0'",sFn_parm.prdt_code);
        if( ret )
        {
            vtcp_log("统计fn_reg错误!");
            strcpy(g_pub_tx.reply, "D330");
            goto ErrExit;
        }
        */
        vtcp_log("min_amt[%.2lf] sum[%.2lf] ",sFn_parm.min_amt,dSum_amt);
        /*
        if(pub_base_CompDblVal(dSum_amt,sFn_parm.min_amt) >= 0)
        */
        if(1)
        {
                    /*每个产品打一个表头*/
            fprintf(fp_qs,"\n\n\n");
            fprintf(fp_qs, "                                           理财产品[%s]扣款统计\n",sFn_parm.prdt_code);
            fprintf(fp_qs, "                                                                                                   日期:%d年%d月%d日\n",g_pub_tx.tx_date/10000,g_pub_tx.tx_date/100%100,g_pub_tx.tx_date%100);
            fprintf(fp_qs, "======================================================================================================================\n" );
            fprintf(fp_qs, "%-6s %-20s%-8s%-17s%-8s\n","机构号","理财产品代码","笔数","理财金额","处理结果");
            fprintf(fp_qs, "======================================================================================================================\n" );

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
                memset(cAc_no,0x00,sizeof(cAc_no));
                memset(cName,0x00,sizeof(cName));
                memset(&sFn_reg,0x00,sizeof(sFn_reg));
                ret=Fn_reg_Fet_Upd(&sFn_reg,g_pub_tx.reply);
                if( ret==100 )
                {
                    vtcp_log("fn_reg_hst fetch over! ac_id[%ld] ",sFn_reg.ac_id);
                    if(lLine > 0)
                    {
                        vtcp_log("! dTx_amt_jg[%lf]  ",dTx_amt_jg);
                        iPage++;
                        fprintf( fp, "======================================================================================================================\n" );
                        fprintf(fp,  "                  合计笔数:%-8ld       合计金额:%-17.2f                                         第%ld页\n",lLine,dTx_amt_jg,iPage);
                        fprintf(fp,"%c",0x0C);
                        fprintf(fp_qs, "%-6s %-20s%-8d%-17.2lf%-8s\n",cOld_br_no,sFn_parm.prdt_code,lLine,dTx_amt_jg,"扣款成功");
                        lLine_qs ++;
                        if( lLine_qs%LINE_PAGE==0 )
                        {
                            iPage_qs++;
                            fprintf( fp_qs, "======================================================================================================================\n" );
                            fprintf( fp_qs, "                                                                                                             第%ld页\n",iPage_qs);
                            fprintf(fp_qs,"%c",0x0C);
                        }
                    }
                    if(lLine_qs>0)
                    {
                        iPage_qs++;
                        fprintf( fp_qs, "======================================================================================================================\n" );
                        fprintf(fp_qs,  "               合计笔数:%-8ld          合计金额:%-17.2f                                         第%ld页\n",lCnt_qs,dTx_amt,iPage_qs);
                        fprintf(fp_qs,"%c",0x0C);
                    }
                    break;
                }
                else if( ret )
                {
                    vtcp_log("fn_reg fetch error! ret[%d] ",ret);
                    strcpy(g_pub_tx.reply,"D330");
                    goto ErrExit;
                }
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
                		/*如果账号被全部冻结只进不出了应该自动解除*/
		/*账号冻结的自动解除 不扣款 syc 201301101*/
                if(sDd_mst.hold_sts[0] == '1' || sDd_mst.hold_sts[0] == '2'
			|| (sDd_mst.bal - sDd_mst.hold_amt - sDd_mst.ctl_amt < sFn_reg.tx_amt)) 
                {
                   sFn_reg.sts[0]='*';
                   strcpy(sFn_reg.filler,"因冻结或止付未扣款");
                   ret=Fn_reg_Upd_Upd(sFn_reg,g_pub_tx.reply);
                   if(ret)
                   {
                       vtcp_log("update fn_reg error! ret[%d]  ",ret);
                       strcpy(g_pub_tx.reply,"D332");
                       goto ErrExit;
                   }
                   continue;
                }
                
                /* 借方：客户账户 */
                strcpy(g_pub_tx.tx_br_no,sFn_reg.tx_br_no);
                /*查找同ac_id下介质状态是正常和挂失的账号，可能存在销介质的情况*/
                ret = my_get_ac_no(sFn_reg.ac_no,cAc_no,cName);
                if(ret)
                {
                    vtcp_log("没有合适的扣款账号![%s]",sFn_reg.ac_no);
                    goto ErrExit;
                }
                strcpy(g_pub_tx.ac_no,cAc_no);
                strcpy(g_pub_tx.name,"");
                g_pub_tx.cif_no = 0;
                g_pub_tx.ac_id=sFn_reg.ac_id;
                g_pub_tx.ac_seqn=sFn_reg.ac_seqn;
                g_pub_tx.tx_amt1=sFn_reg.tx_amt;
                g_pub_tx.add_ind[0]='0';
                strcpy(g_pub_tx.hst_ind,"1");               /* 日间入明细 */
                strcpy(g_pub_tx.ac_wrk_ind,"0000010011");
                g_pub_tx.svc_ind=1001;
                strcpy(g_pub_tx.ct_ind,"2");                /* 转账 */
                strcpy(g_pub_tx.ac_id_type,"1");
                strcpy(g_pub_tx.note_type,"");
                strcpy(g_pub_tx.beg_note_no,"");
                if ( pub_acct_trance() )
                {
                    vtcp_log("调用存取款主控失败!");
                    goto ErrExit;
                }
                set_zd_data("102K",g_pub_tx.ct_ind);
                set_zd_double("102I",g_pub_tx.tx_amt1);
                if ( pubf_acct_proc("LC01") )
                {
                    vtcp_log("登记会计流水错误!");
                    goto ErrExit;
                }
                /* 贷方：理财资金账户 */
                strcpy(g_pub_tx.name,"");
                g_pub_tx.cif_no = 0;
                if( sFn_reg.cif_type[0]=='1' )
                {
                    strcpy(g_pub_tx.ac_no,sFn_parm.per_ac_no);
                }
                else if( sFn_reg.cif_type[0]=='2'||sFn_reg.cif_type[0]=='3'||sFn_reg.cif_type[0]=='4'||sFn_reg.cif_type[0]=='6' )   /*add by lwb 增加客户类型为3 机构 4 金融同业 6金融机构*/
                {
                    strcpy(g_pub_tx.ac_no,sFn_parm.com_ac_no);
                }
                /*如果是科目 开户机构是清算中心*/
                if(strlen(g_pub_tx.ac_no) < 10)
                {
                    strcpy(g_pub_tx.opn_br_no,QS_BR_NO);
                }
                 vtcp_log("%s,%d g_pub_tx.ac_no====[%s]\n",__FILE__,__LINE__,g_pub_tx.ac_no);
                 vtcp_log("%s,%d 444444444sFn_parm.prdt_code====[%s]\n",__FILE__,__LINE__,sFn_parm.prdt_code);
                g_pub_tx.ac_id = 0;
                g_pub_tx.ac_seqn=0;
                g_pub_tx.tx_amt1=sFn_reg.tx_amt;
                g_pub_tx.add_ind[0]='1';
                strcpy(g_pub_tx.hst_ind,"1");               /* 日间入明细 */
                strcpy(g_pub_tx.ac_wrk_ind,"0000010011");
                /*g_pub_tx.svc_ind=1001;*/
                g_pub_tx.svc_ind=9001;
                strcpy(g_pub_tx.ct_ind,"2");                /* 转账 */
                strcpy(g_pub_tx.ac_id_type,"9");
                strcpy(g_pub_tx.note_type,"");
                strcpy(g_pub_tx.beg_note_no,"");
                if ( pub_acct_trance() )
                {
                    vtcp_log("调用存取款主控失败!");
                    goto ErrExit;
                }
                set_zd_data("102K",g_pub_tx.ct_ind);
                set_zd_double("1002",g_pub_tx.tx_amt1);
                if ( pubf_acct_proc("LC02") )
                {
                    vtcp_log("登记会计流水错误!");
                    goto ErrExit;
                }
                sFn_reg.pay_date=g_pub_tx.tx_date;
                sFn_reg.pay_trace_no=g_pub_tx.trace_no;
                sFn_reg.sts[0]='1';
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
                strcpy(sFn_reg_hst.type,"1");
                sFn_reg_hst.tx_amt = sFn_reg.tx_amt;
                sFn_reg_hst.buy_date=sFn_reg.tx_date;
                sFn_reg_hst.buy_trace_no=sFn_reg.trace_no;
                strcpy(sFn_reg_hst.brf,"批量扣款");
                ret = Fn_reg_hst_Ins(sFn_reg_hst,g_pub_tx.reply);
                if(ret)
                {
                    Fn_reg_hst_Debug(&sFn_reg_hst);
                    vtcp_log("登记理财产品登记簿错误iRet[%d]",ret);
                    strcpy(g_pub_tx.reply,"D335");
                    goto ErrExit;
                }
                vtcp_log(" dTx_amt_jg[%lf]  ",dTx_amt_jg);
                /* 生成报表清单 */
                if( strcmp(sFn_reg.tx_br_no,cOld_br_no) )
                {
                    if(lLine > 0)
                    {
                        vtcp_log("! dTx_amt_jg[%lf]  ",dTx_amt_jg);
                        iPage++;
                        fprintf( fp, "======================================================================================================================\n" );
                        fprintf(fp,  "                  合计笔数:%-8ld笔       合计金额:%-17.2f                                         第%ld页\n",lLine,dTx_amt_jg,iPage);
                        fprintf(fp,"%c",0x0C);
                        /*清算中心的统计文件*/
                        fprintf(fp_qs, "%-6s %-20s%-8d%-17.2lf%-8s\n",cOld_br_no,sFn_parm.prdt_code,lLine,dTx_amt_jg,"扣款成功");
                        lLine_qs ++;
                        if( lLine_qs%LINE_PAGE==0 )
                        {
                            iPage_qs++;
                            fprintf( fp_qs, "======================================================================================================================\n" );
                            fprintf( fp_qs, "                                                                                                             第%ld页\n",iPage_qs);
                            fprintf(fp_qs,"%c",0x0C);
                        }
                    }
                    dTx_amt_jg = 0.00;
                    vtcp_log(" dTx_amt_jg[%lf]  ",dTx_amt_jg);
                    /* 打开文件 */
                    if(fp != NULL)
                    {
                        fclose(fp);
                        fp = NULL;
                    }
                    ret=pub_rpt_append_openfile(&fp,sFn_reg.tx_br_no,cFilename);
                    if(ret)
                    {
                        vtcp_log("打开文件失败!");
                        strcpy(g_pub_tx.reply,"S066");
                        goto ErrExit;
                    }
                    memcpy(cOld_br_no,sFn_reg.tx_br_no,sizeof(sFn_reg.tx_br_no)-1);
                    fprintf(fp, "                                            机构[%s]理财扣款清单\n",cOld_br_no);
                    fprintf(fp, "                                                                                                   日期:%d年%d月%d日\n",g_pub_tx.tx_date/10000,g_pub_tx.tx_date/100%100,g_pub_tx.tx_date%100);
                    fprintf(fp, "======================================================================================================================\n" );
                    fprintf(fp, "%-20s %-20s%-60s%-17s\n","理财产品代码","账号","户名","理财金额");
                    fprintf(fp, "======================================================================================================================\n" );
                    iPage=0;
                    lLine=0;
                }
                fprintf(fp, "%-20s %-20s%-60s%-17.2lf\n",sFn_parm.prdt_code,cAc_no,cName,sFn_reg.tx_amt);
                lLine++;
                dTx_amt_jg += sFn_reg.tx_amt;
                if( lLine%LINE_PAGE==0 )
                {
                    iPage++;
                    fprintf( fp, "======================================================================================================================\n" );
                    fprintf( fp, "                                                                                                             第%ld页\n",iPage);
                    fprintf(fp,"%c",0x0C);
                }
                dTx_amt += sFn_reg.tx_amt;
                lCnt_qs ++;
            }
            Fn_reg_Clo_Upd();

            /* fn_parm产品状态修改 */
            sFn_parm.stat[0]='1';
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
            if(sFn_parm.end_date ==  g_pub_tx.tx_date)
            {
                if(iFail_Cnt == 0)
                {
                    /*结束日是当天的  募集失败的 写到文件里 文件只在清算中心打开一次*/
                    ret=pub_rpt_append_openfile(&fp_fail,QS_BR_NO,cFilename_fail);
                    if(ret)
                    {
                        vtcp_log("打开文件失败!");
                        strcpy(g_pub_tx.reply,"S035");
                        goto ErrExit;
                    }
                    fprintf(fp_fail,"\n");
                    fprintf(fp_fail,"\n");
                    fprintf(fp_fail,"\n");
                    fprintf(fp_fail,"\n");
                    fprintf(fp_fail,"                            募集失败理财产品列表\n");
                }
                fprintf( fp_fail, "   理财产品%s募集失败：最小募集金额：%.2lf  本次募集金额：%.2lf 暂不扣款，请处理！\n",\
                    sFn_parm.prdt_code,sFn_parm.min_amt,dSum_amt );
                if(iFail_Cnt >0 && iFail_Cnt % 40 == 0)
                {
                    fprintf(fp_fail,"\f");
                }
                iFail_Cnt ++;
            }
            vtcp_log("募集总金额[%.2lf]小于此产品[%s]最小募集金额[%.2lf]，募集总金额[%.2lf]暂不扣款 ",dSum_amt,sFn_parm.prdt_code,sFn_parm.min_amt,sFn_parm.sale_amt);
        }
    }
    Fn_parm_Clo_Upd();

OkExit:
    if(fp != NULL)
    {
        fclose(fp);
        fp = NULL;
    }
    if(fp_fail != NULL)
    {
        fclose(fp_fail);
        fp_fail = NULL;
    }
    if(fp_qs != NULL)
    {
        fclose(fp_qs);
        fp_qs = NULL;
    }
    sql_commit();     /*--提交事务--*/
    strcpy(g_pub_tx.reply, "0000");
    vtcp_log("处理成功![%s]", g_pub_tx.reply);
    set_zd_data("0120", g_pub_tx.reply);
    return 0;

ErrExit:
    if(fp != NULL)
    {
        fclose(fp);
        fp = NULL;
    }
    if(fp_fail != NULL)
    {
        fclose(fp_fail);
        fp_fail = NULL;
    }
    if(fp_qs != NULL)
    {
        fclose(fp_qs);
        fp_qs = NULL;
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
/*根据客户账号找到能扣账的账号 找不到 就报错shangyongchao 2013/3/9 20:46:46
ac_no 传入旧账号
new_ac_no 根据ac_id找到的状态正常的账号*/
static int my_get_ac_no(char *ac_no,char *new_ac_no,char *name)
{
    int iRet = 0;
    long lAc_id = 0;
    struct mdm_ac_rel_c sMdm_ac_rel;
    
    memset(&sMdm_ac_rel,0x00,sizeof(sMdm_ac_rel));
    
    iRet  = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_no = '%s'",ac_no);
    if(iRet != 0 && iRet != 100)
    {
        vtcp_log( "查找账号失败![%d]", iRet);
        return 1;
    }
    else if(iRet == 100)
    {
        vtcp_log( "无此账号![%s]", ac_no);
        return 1;
    }
    /*正常和挂失的 直接返回就行了 能正常入账*/
    if(sMdm_ac_rel.note_sts[0] == '1' || sMdm_ac_rel.note_sts[0] =='0')
    {
        strcpy(new_ac_no,ac_no);
        /*strcpy(name,sMdm_ac_rel.name);*/
        return 0;
    }
    lAc_id = sMdm_ac_rel.ac_id;
    /*其他状态的 找 ac_id相符的 正常或者挂失的账号 实在找不到就报错*/
    memset(&sMdm_ac_rel,0x00,sizeof(sMdm_ac_rel));
    
    iRet = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_id=%ld and note_sts in('0','1')",lAc_id);
    if(iRet != 0 && iRet != 100)
    {
        vtcp_log( "查找账号失败![%d]", iRet);
        return 1;
    }
    else if(iRet == 100)
    {
        vtcp_log("无此账号![%s]", ac_no);
        return 1;
    }
    strcpy(new_ac_no,sMdm_ac_rel.ac_no);
    /*strcpy(name,sMdm_ac_rel.name);*/
    return 0;
}
