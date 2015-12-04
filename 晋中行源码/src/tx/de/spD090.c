/*************************************************************
* 文 件 名: spD090.c
* 功能描述: 分配理财的收益 归还日当天晚上日终批量执行（翻盘前）
如果需要切日后就入账 可以改成翻盘后执行

* 作    者: shangyongchao
* 完成日期: 2013/5/23

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
#include "fn_ac_rel_c.h"
#include "in_mst_c.h"
#include "com_branch_c.h"
#include "com_sys_parm_c.h"
#include "mdm_ac_rel_c.h"
#include "redempt_rate_c.h"
#include "fn_br_profit_c.h"
#include "com_parm_c.h"
#define LINE_PAGE 40
#define HD_LINE 3
#define L365 365
int spD090()
{
    int ret,iPage,iFlag,iHdCnt,iIntst_flag,iPage_qs;
    long lLine,lLine_qs,lCnt_qs,lDays;
    char cFilename[256],cFilename1[256],cCmd[512],cAc_no[25],cAc_no_gz[25],cName[61];;
    char cPrdt_code[21];
    char cOld_br_no[6];
    char cBrf[21];/*摘要*/
    int iHold_flag = 0;/*0账户未被全部冻结，1 账户被全部冻结*/
    double dProfit_rate = 0.000000; /*分润利率*/
    double dRbj_amt = 0.00;/*归还本金*/
    double dFct_rate=0.00000000;/*实际收益率*/
    double dProfit_amt = 0.00;/*分润金额*/
    double dFct_intst = 0.00;/*本次实际收益*/
    double dTmp_amt = 0.00;
    FILE * fp=NULL;/*写各个认购机构的明细*/
    FILE * fp1=NULL;/*写各个认购机构的客户回单*/
    FILE *fp_qs =NULL;/*写清算中心的各个机构的汇总*/
    /*定义一个 统计数据用的结构体*/
    struct total_info
    {
        long cnt;/*笔数*/
        double tx_amt;/*认购金额*/
        double redempt_amt;/*赎回金额*/
        double return_capital1;/*本次返回的本金*/
        double fct_intst1;/*本次实际收益*/
        double profit_amt1;/*本次分润金额*/
        double trust_amt;/*托管费金额*/
        double return_capital2;/*累计返回的本金*/
        double fct_intst2;/*累计实际收益*/
        double profit_amt2;/*累计分润金额*/
    }sTotal_qs,sTotal_jg;
    struct fn_parm_c sFn_parm;
    struct fn_reg_c sFn_reg,sFn_reg_tmp;
    struct fn_ac_rel_c sFn_ac_rel;
    struct fn_reg_hst_c sFn_reg_hst;
    struct in_mst_c sIn_mst;
    struct com_branch_c sCom_branch;
    struct com_sys_parm_c sCom_sys_parm;
    struct fn_br_profit_c sFn_br_profit;
    
    ret=iPage=iFlag=iHdCnt=iIntst_flag=iPage_qs=0;
    lCnt_qs=lLine_qs=lLine = lDays = 0;
    
    memset(cFilename,0x00,sizeof(cFilename));
    memset(cFilename1,0x00,sizeof(cFilename1));
    memset(cCmd,0x00,sizeof(cCmd));
    memset(cAc_no,0x00,sizeof(cAc_no));
    memset(cAc_no_gz,0x00,sizeof(cAc_no_gz));
    memset(cOld_br_no,0x00,sizeof(cOld_br_no));
    memset(&sTotal_jg,0x00,sizeof(sTotal_jg));
    memset(&sTotal_qs,0x00,sizeof(sTotal_qs));
    memset(&sFn_parm,0x00,sizeof(sFn_parm));
    memset(&sFn_reg,0x00,sizeof(sFn_reg));
    memset(&sFn_reg_tmp,0x00,sizeof(sFn_reg_tmp));
    memset(&sFn_ac_rel,0x00,sizeof(sFn_ac_rel));
    memset(&sFn_reg_hst,0x00,sizeof(sFn_reg_hst));
    memset(&sIn_mst,0x00,sizeof(sIn_mst));
    memset(&sCom_branch,0x00,sizeof(sCom_branch));
    memset(&sCom_sys_parm,0x00,sizeof(sCom_sys_parm));
    memset(&sFn_br_profit,0x00,sizeof(sFn_br_profit));
    memset(cPrdt_code,0x00,sizeof(cPrdt_code));
    
    pub_base_sysinit();
    get_zd_data("0630",cPrdt_code);
   /* 
    strcpy(g_pub_tx.tx_code,"G189");
    strcpy(g_pub_tx.sub_tx_code,"G189");
    */
    set_zd_data("096C","0100");
    /*set_zd_data("096M",YW_TYPE_ZC);by wy*/
    
    /* 删除文件 */
    strcpy(cFilename,"lcsy");    /* 理财收益 */
    sprintf(cCmd,"rm -f %s/report/%s/%s.txt",getenv("HOME"),"*",cFilename);
    vtcp_log("rm -f %s/report/%s/%s.txt",getenv("HOME"),"*",cFilename);
    ret=system(cCmd);
    if( ret )
    {
        vtcp_log("rm file error [%s] [%d]",cCmd,ret);
        strcpy(g_pub_tx.reply,"S035");
        goto ErrExit;
    }
    strcpy(cFilename1,"lcsyhd");    /* 理财收益回单 */
    sprintf(cCmd,"rm -f %s/report/%s/%s.txt",getenv("HOME"),"*",cFilename1);
    vtcp_log("rm -f %s/report/%s/%s.txt",getenv("HOME"),"*",cFilename);
    ret=system(cCmd);
    if( ret )
    {
        vtcp_log("rm file error [%s] [%d]",cCmd,ret);
        strcpy(g_pub_tx.reply,"S035");
        goto ErrExit;
    }
    ret=pub_rpt_append_openfile(&fp_qs,QS_BR_NO,cFilename);
    if(ret)
    {
        vtcp_log("%s,%d,打开文件失败!");
        strcpy(g_pub_tx.reply,"S066");
        goto ErrExit;
    }

    /*
    ret = sql_begin();
    if(ret)
    {
        vtcp_log("打开数据库错误 [%d]",ret);
        goto ErrExit;
    }
    */
    /*按归还收益日期处理*/
    ret=Fn_parm_Dec_Upd(g_pub_tx.reply,"prdt_code='%s' and stat ='7'",cPrdt_code);
    if(ret)
    {
        vtcp_log("定义fn_parm游标失败! ret=[%d]",ret);
        strcpy(g_pub_tx.reply,"D325");
        goto ErrExit;
    }
    iHdCnt = 0;
    lLine_qs = 0;
    lLine = 0;
    memset(&sFn_parm,0x00,sizeof(sFn_parm));
    memset(&sTotal_jg,0x00,sizeof(sTotal_jg));
    memset(&sTotal_qs,0x00,sizeof(sTotal_qs));
    ret=Fn_parm_Fet_Upd(&sFn_parm,g_pub_tx.reply);
    if( ret==100 )
    {
        vtcp_log(" 没有这个理财产品! ");
        goto ErrExit;
    }
    else if( ret )
    {
        vtcp_log("  fn_parm fetch error! ret[%d] ",ret);
        strcpy(g_pub_tx.reply,"D324");
        goto ErrExit;
    }
    fprintf(fp_qs,"\n\n\n");
    fprintf(fp_qs, "                                           理财产品[%s]归还收益统计\n",sFn_parm.prdt_code);
    fprintf(fp_qs, "                                                                                                   日期:%d年%d月%d日\n",g_pub_tx.tx_date/10000,g_pub_tx.tx_date/100%100,g_pub_tx.tx_date%100);
    fprintf(fp_qs, "==============================================================================================================================\n" );
    fprintf(fp_qs, "%-6s %-20s%-8s%-17s%-17s%-17s%-17s%-17s\n","机构号","理财产品代码","总笔数","理财金额","赎回金额","返还本金","实际收益","分润金额");
    fprintf(fp_qs, "==============================================================================================================================\n" );

    vtcp_log("return_date[%ld] ",sFn_parm.return_date);
    /* 取主机流水号 */
    /*
    if( pub_base_PubQlsh(&g_pub_tx.trace_no,g_pub_tx.reply) )
    {
        vtcp_log("取主机流水号错 [%d]", g_pub_tx.trace_no);
        goto ErrExit;
    }
    
    set_zd_long(DC_TRACE_NO,g_pub_tx.trace_no);
    g_pub_tx.trace_cnt = 0;
    set_zd_long(DC_TRACE_CNT,g_pub_tx.trace_cnt);
    */
    strcpy(cOld_br_no,"-----");

    ret=Fn_ac_rel_Sel(g_pub_tx.reply,&sFn_ac_rel,"prdt_code='%s'",sFn_parm.prdt_code);
    if(ret)
    {
        vtcp_log("查询理财产品清算账号配置表错误");
        strcpy(g_pub_tx.reply,"S035");
        goto ErrExit;
    }
    /*按认购合同入账,而且要按认购机构出报表*/
    ret=Fn_reg_Dec_Upd(g_pub_tx.reply,"prdt_code='%s' and sts='1' order by tx_br_no,ac_id",sFn_parm.prdt_code);
    if( ret )
    {
        vtcp_log("declare cursor error! ret[%d] ",ret);
        strcpy( g_pub_tx.reply,"D332" );
        goto ErrExit;
    }
    while(1)
    {
        lDays = 0;
        dFct_rate = 0.000000;
        dProfit_rate = 0.000000; /*分润利率*/
        iIntst_flag = 0;
        dFct_intst = 0.00;
        dTmp_amt = 0.00;
        memset(cBrf,0x00,sizeof(cBrf));
        memset(cAc_no,0x00,sizeof(cAc_no));
        memset(cName,0x00,sizeof(cName));
        memset(&g_dd_mst,0x00,sizeof(g_dd_mst));
        memset(&g_in_mst,0x00,sizeof(g_in_mst));
        memset(&sFn_reg_hst,0x00,sizeof(sFn_reg_hst));
        memset(&sFn_reg,0x00,sizeof(struct fn_reg_c));
        memset(&sFn_br_profit,0x00,sizeof(sFn_br_profit));
        ret=Fn_reg_Fet_Upd(&sFn_reg,g_pub_tx.reply);
        if( ret==100 )
        {
            vtcp_log("fn_reg fetch over! ");
            if( lLine>0 )/*打上一个机构这个产品的尾*/
            {
                iPage++;
                fprintf( fp, "=======================================================================================================================================\n" );
                fprintf( fp, "合计:%8ld笔     %-17.2lf%-17.2lf%-17.2lf%-17.2lf%-17.2lf               第%ld页\n",\
                    sTotal_jg.cnt,sTotal_jg.tx_amt,sTotal_jg.redempt_amt,sTotal_jg.return_capital1,\
                    sTotal_jg.fct_intst1,sTotal_jg.profit_amt1,iPage);
                fprintf(fp,"%c",0x0C);
                /*如果上个机构有纪录，统计到清算中心*/
                sTotal_qs.cnt += sTotal_jg.cnt;
                sTotal_qs.tx_amt+=sTotal_jg.tx_amt;
                sTotal_qs.redempt_amt+= sTotal_jg.redempt_amt;
                sTotal_qs.return_capital1+= sTotal_jg.return_capital1;
                sTotal_qs.fct_intst1 += sTotal_jg.fct_intst1;
                sTotal_qs.profit_amt1+= sTotal_jg.profit_amt1;
                /*写一条这个机构的统计信息*/
                fprintf(fp_qs,"%-6s %-20s%-8ld%-17.2lf%-17.2lf%-17.2lf%-17.2lf%-17.2lf\n",\
                    cOld_br_no,sFn_parm.prdt_code,sTotal_jg.cnt,sTotal_jg.tx_amt,sTotal_jg.redempt_amt,\
                    sTotal_jg.return_capital1,sTotal_jg.fct_intst1,sTotal_jg.profit_amt1);
                lLine_qs ++;
                if(lLine_qs % LINE_PAGE == 0)
                {
                    iPage_qs++;
                    fprintf( fp_qs, "==============================================================================================================================\n" );
                    fprintf( fp_qs, "                                                                                                                     第%ld页\n",iPage_qs);
                    fprintf(fp_qs,"%c",0x0C);
                }
            }
            if(lLine_qs > 0)/*清算中心文件写 合计尾*/
            {
                iPage_qs++;
                fprintf( fp_qs, "==============================================================================================================================\n" );
                fprintf(fp_qs,  "          合计:            %-8ld%-17.2f%-17.2lf%-17.2lf%-17.2lf%-17.2lf第%ld页\n",\
                        sTotal_qs.cnt,sTotal_qs.tx_amt,sTotal_qs.redempt_amt,sTotal_qs.return_capital1,\
                        sTotal_qs.fct_intst1,sTotal_qs.profit_amt1,iPage_qs);
                fprintf(fp_qs,"%c",0x0C);
                
            }
            if(iHdCnt > 0)/*这个产品查完 回单文件换页*/
            {
                fprintf(fp1,"\f");
            }
            break;
        }
        else if( ret )
        {
            vtcp_log("fn_reg fetch error! ret[%d] ",ret);
            strcpy(g_pub_tx.reply,"D330");
            goto ErrExit;
        }
        /*计算收益 分封闭式和开放式的取不同利率 确定计息天数*/
        if(sFn_parm.prdt_type[0] == '1')
        {
            /*封闭式的起止日期相同，采用相同的利率 相同的分润利率*/
            /*由于存在提前中止的情况，所以所有收益均重新计算*/
            dProfit_rate = sFn_parm.profit_rate;
            dFct_rate = sFn_parm.fct_rate;
            lDays =  pub_base_CalTrueDays(sFn_reg.ic_date,sFn_reg.end_date);
            /*add by lwb 晋中商行要求一年为365天*/ 
           if(lDays == 366)
	          {
		          lDays = 365;
	          }
            vtcp_log("利率[%lf],天数[%ld]  ic_date [%ld] end_date[%ld] ",dFct_rate,lDays,sFn_reg.ic_date,sFn_reg.end_date);
        }
        else if(sFn_parm.prdt_type[0] == '2')
        {
            /*开放式的起始日期不同，采用分段利率 同时存在未分配的收益(因调整收益率计算出的未入账的)*/
            lDays =  pub_base_CalTrueDays(sFn_reg.ic_date,sFn_reg.end_date);
	    /*add by lwb 晋中商行要求一年为365天*/ 
    	    if(lDays== 366)
	        {
	         	lDays = 365;
	         }
            dProfit_rate = sFn_parm.profit_rate;
            dFct_rate = sFn_parm.fct_rate;
            vtcp_log("利率[%lf],天数[%ld]",dFct_rate,lDays);
            /*存在的因调整收益率生成的未分配收益*/
            dFct_intst += sFn_reg.amt1;
            dProfit_amt += sFn_reg.amt2;
            /*
            vtcp_log("参数错误了 prdt_type[%s]",sFn_parm.prdt_type);
            strcpy(g_pub_tx.reply,"S035");
            goto ErrExit;
            */
        }
        else
        {
            vtcp_log("参数错误了 prdt_type[%s]",sFn_parm.prdt_type);
            strcpy(g_pub_tx.reply,"S035");
            goto ErrExit;
        }
        /*确定归还本金*/
        /*保本的 认购 -赎回*/
        if(sFn_parm.intst_type[0] == '1' || sFn_parm.intst_type[0] == '2')
        {
            dRbj_amt=sFn_reg.tx_amt - sFn_reg.redempt_amt;
        }
        else
        {
            /*不保本的  如果本金收不回来 就要参数表里的实际收益率是0 为保证不算收益 天数重置 0*/
            if(sFn_parm.capital_percent  - 100.000000 > 0.0000005)
            {
                vtcp_log("返还本金比例大于百分之100 参数错误! sFn_parm.capital_percent=[%lf]",sFn_parm.capital_percent);
                strcpy(g_pub_tx.reply,"P370");
                goto ErrExit;
            } 
            else if(sFn_parm.capital_percent - 100.000000 < 0.0000005&&(pub_base_CompDblVal(sFn_parm.capital_percent,100.00) != 0 )) /*add by lwb*/
            {
                lDays = 0;/*损失本金 不算收益了 天数置0*/
                dRbj_amt=pub_base_PubDround_1((sFn_reg.tx_amt - sFn_reg.redempt_amt) * sFn_parm.capital_percent/100);
            }
            else
            {
                dRbj_amt=sFn_reg.tx_amt - sFn_reg.redempt_amt;
            }
        }
        vtcp_log("确定的 利率[%lf],天数[%ld],本金[%lf]",dFct_rate,lDays,dRbj_amt);
        sFn_reg.return_capital += dRbj_amt;/*赎回的如果是部分赎回 已经有一部分值了 累加上*/
        /*有金额才记账*/
        if(pub_base_CompDblVal(dRbj_amt,0.00) > 0)
        {
        		 /* 贷方：客户账户(本金) */
            ret = my_get_ac_no(sFn_reg.ac_no,cAc_no,cName);
            if(ret)
            {
                vtcp_log("没有合适的入账的账号![%s]",sFn_reg.ac_no);
                goto ErrExit;
            }
            ret = my_get_ac_hold(sFn_reg.tx_br_no,sFn_reg.ac_id,sFn_reg.ac_seqn,&iHold_flag,cAc_no_gz);
            if(ret)
            {
                vtcp_log("查询账号是否被冻结失败![%s]",sFn_reg.ac_no);
                goto ErrExit;
            }
                if(iHold_flag == 1)
            {
            	  sFn_reg.sts[0]='6';
            	  sFn_reg.return_date = g_pub_tx.tx_date;
            	  vtcp_log("return_date = %d",sFn_reg.return_date);
            	 
            	  ret=Fn_reg_Upd_Upd(sFn_reg,g_pub_tx.reply);
            	  if(ret)
            	  {
      
			            vtcp_log("update fn_reg_hst error! ret[%d]  ",ret);
			            strcpy(g_pub_tx.reply,"D332");
			            goto ErrExit;
        	
            	  }
            	  continue;
            	  
                /*set_zd_data("096H",cAc_no_gz);
                /
                strcpy(g_pub_tx.brf,sFn_reg.prdt_code);
                /
                snprintf(g_pub_tx.brf,20,"%s本金",sFn_parm.prdt_code);
                if(func_in_acct(cAc_no_gz,"1",dRbj_amt))
                {
                    vtcp_log("记账失败");
                    goto ErrExit;
                }
                sprintf(cBrf,"挂账:%s",cAc_no_gz);
                sprintf(sFn_reg.filler,"挂账%s|%.2lf",cAc_no_gz,dRbj_amt);*/
            }
            else
          {
          	
            strcpy(g_pub_tx.ac_no,cAc_no);
            strcpy(g_pub_tx.name,"");
            strcpy(cBrf,"正常入账");
            g_pub_tx.cif_no = 0;
            g_pub_tx.ac_id=sFn_reg.ac_id;
            g_pub_tx.ac_seqn=sFn_reg.ac_seqn;
            g_pub_tx.tx_amt1=dRbj_amt;
            g_pub_tx.add_ind[0]='1';
            strcpy(g_pub_tx.hst_ind,"1");               /* 日间入明细 */
            strcpy(g_pub_tx.ac_wrk_ind,"0000010011");
            g_pub_tx.svc_ind=1001;
            strcpy(g_pub_tx.ct_ind,"2");                /* 转账 */
            strcpy(g_pub_tx.ac_id_type,"1");
            strcpy(g_pub_tx.note_type,"");
            strcpy(g_pub_tx.beg_note_no,"");
            strcpy(g_pub_tx.brf,"理财返还本金");
            vtcp_log("ac_id[%ld] tx_amt[%lf] 贷：客户账户(本金) ",g_pub_tx.ac_id,g_pub_tx.tx_amt1);
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
           }
        }
        /*计算收益 dFct_intst考虑后期加上开放式的调整利率的未分配收益*/
        dTmp_amt = dRbj_amt * lDays * dFct_rate * (1 + sFn_reg.flt_radio)/L365/L100;
        dFct_intst += pub_base_PubDround_1(dTmp_amt);
        sFn_reg.fct_intst += dFct_intst;
        vtcp_log("本次计算出的收益[%lf]本次总实际收益[%lf] 预期[%lf]",dTmp_amt,dFct_intst,sFn_reg.intst);
        /*分润金额也重算下*/
        dTmp_amt = 0.00;
        dTmp_amt = dRbj_amt * lDays * sFn_parm.profit_rate /L365/L100;
        dProfit_amt += pub_base_PubDround_1(dTmp_amt);
        sFn_reg.fct_profit_amt += dProfit_amt;/*考虑到可能存在部分赎回的 这里累加上*/
        vtcp_log("算出的分润[%lf] 总的分润[%lf]",dProfit_amt,sFn_reg.fct_profit_amt);
        if(pub_base_CompDblVal(dFct_intst,0.00) > 0)
        {
            /* 借方：224199其他应付款-理财资金清算户(收益) */
            g_pub_tx.tx_amt1=dFct_intst;
            strcpy(g_pub_tx.ac_no,sFn_ac_rel.qs_ac_no);
            strcpy(g_pub_tx.name,"");
            g_pub_tx.cif_no=0;
            g_pub_tx.ac_id=0;
            g_pub_tx.ac_seqn=0;
            g_pub_tx.add_ind[0]='0';
            strcpy(g_pub_tx.hst_ind,"1");               /* 日间入明细 */
            strcpy(g_pub_tx.ac_wrk_ind,"0000010011");
            g_pub_tx.svc_ind=9001;
            strcpy(g_pub_tx.ct_ind,"2");                /* 转账 */
            strcpy(g_pub_tx.ac_id_type,"9");
            strcpy(g_pub_tx.note_type,"");
            strcpy(g_pub_tx.beg_note_no,"");
            strcpy(g_pub_tx.brf,"理财返还收益");
            vtcp_log("ac_id[%ld] tx_amt[%lf] 借：其他应付款-理财资金清算户(本金) ",g_pub_tx.ac_id,g_pub_tx.tx_amt1);
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
                        /*全部冻结的存到挂账账号*/
            if(iHold_flag == 1)
            {
            		
            		
                set_zd_data("096H",cAc_no_gz);
                snprintf(g_pub_tx.brf,20,"%s收益",sFn_reg.prdt_code);
                if(func_in_acct(cAc_no_gz,"1",dFct_intst))
                {
                    vtcp_log("记账失败");
                    goto ErrExit;
                }
                /*在这里吧fn_reg的filler改了 记录下是挂账了*/
                sprintf(sFn_reg.filler+strlen(sFn_reg.filler),"|%.2lf",dFct_intst);
            }
            /* 贷方：客户账户(收益) */
            else
            {
            g_pub_tx.tx_amt1=dFct_intst;
            strcpy(g_pub_tx.ac_no,cAc_no);/*直接取上面的账号就行了*/
            strcpy(g_pub_tx.name,"");
            g_pub_tx.cif_no = 0;
            g_pub_tx.ac_id=sFn_reg.ac_id;
            g_pub_tx.ac_seqn=sFn_reg.ac_seqn;
            g_pub_tx.add_ind[0]='1';
            strcpy(g_pub_tx.hst_ind,"1");               /* 日间入明细 */
            strcpy(g_pub_tx.ac_wrk_ind,"0000010011");
            g_pub_tx.svc_ind=1001;
            strcpy(g_pub_tx.ct_ind,"2");                /* 转账 */
            strcpy(g_pub_tx.ac_id_type,"1");
            strcpy(g_pub_tx.note_type,"");
            strcpy(g_pub_tx.beg_note_no,"");
            strcpy(g_pub_tx.brf,"理财分配收益");
            vtcp_log("ac_id[%ld] tx_amt[%lf] 贷：客户账户(收益) ",g_pub_tx.ac_id,g_pub_tx.tx_amt1);
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
          }
        }
        	
        	
        	
             /*借方：20110199其他存款（本金） */
            /*strcpy(g_pub_tx.tx_br_no,QS_BR_NO);清算中心归还给客户*/
            strcpy(g_pub_tx.name,"");
            /*mod by lwb 201341202 收益和本金都从清算账号出*/
            strcpy(g_pub_tx.ac_no,sFn_ac_rel.qs_ac_no);
            /*if( sFn_reg.cif_type[0]=='1' )
            {
                strcpy(g_pub_tx.ac_no,sFn_parm.per_ac_no);
            }
            else if( sFn_reg.cif_type[0]=='2'||sFn_reg.cif_type[0]=='3'||sFn_reg.cif_type[0]=='4'||sFn_reg.cif_type[0]=='6' ) /add by lwb 增加客户类型为3,4,6的判断/
            {
                strcpy(g_pub_tx.ac_no,sFn_parm.com_ac_no);
            }*/
            /*是科目的话 开户机构为清算中心*/
            if(strlen(g_pub_tx.ac_no) < 10)
            {
                strcpy(g_pub_tx.opn_br_no,QS_BR_NO);
            }
            g_pub_tx.cif_no = 0;
            g_pub_tx.ac_id = 0;
            g_pub_tx.ac_seqn=0;
            g_pub_tx.tx_amt1=dRbj_amt;
            g_pub_tx.add_ind[0]='0';
            strcpy(g_pub_tx.hst_ind,"1");               /* 日间入明细 */
            strcpy(g_pub_tx.ac_wrk_ind,"0000010011");
            /*g_pub_tx.svc_ind=1001;*/
            g_pub_tx.svc_ind=9001;
            strcpy(g_pub_tx.ct_ind,"2");                /* 转账 */
            strcpy(g_pub_tx.ac_id_type,"9");
            strcpy(g_pub_tx.note_type,"");
            strcpy(g_pub_tx.beg_note_no,"");
            strcpy(g_pub_tx.brf,"理财返还本金");
            vtcp_log(" ac_id[%ld] tx_amt[%lf] 借：其他存款(本金) ",g_pub_tx.ac_id,g_pub_tx.tx_amt1);
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
           
/*如果是开放式理财到期，则需要在这里进行登记机构分润金额，封闭式的则是在日终进行登记lpj20151109*/
        /*登记机构分润*/
        if(sFn_parm.prdt_type[0]=='2')
        {
        ret = Fn_br_profit_Dec_Upd(g_pub_tx.reply,"prdt_code='%s' and br_no='%s' and sts='0'",sFn_reg.prdt_code,sFn_reg.tx_br_no);/*如果是已入账的则不进行更新lpj20151109*/
        if(ret)
        {
            vtcp_log("定义更新机构分润表失败");
            strcpy(g_pub_tx.reply,"D346");
            goto ErrExit;
        }
        ret = Fn_br_profit_Fet_Upd(&sFn_br_profit,g_pub_tx.reply);
        if(ret  == 100)
        {
            /*没有记录 说明没有赎回过 第一次处理*/
            vtcp_log("没有机构的分润信息，登记一条");
            strcpy(sFn_br_profit.prdt_code,sFn_reg.prdt_code);
            strcpy(sFn_br_profit.br_no,sFn_reg.tx_br_no);
            sFn_br_profit.tx_amt = sFn_reg.tx_amt;
            sFn_br_profit.redempt_amt = sFn_reg.redempt_amt;
            sFn_br_profit.profit_amt = sFn_reg.fct_profit_amt;
            sFn_br_profit.reg_date = g_pub_tx.tx_date;
            sFn_br_profit.reg_trace_no = g_pub_tx.trace_no;
            strcpy(sFn_br_profit.sts,"0");
            strcpy(sFn_br_profit.brf,"到期分配");
            ret = Fn_br_profit_Ins(sFn_br_profit,g_pub_tx.reply);
            if(ret)
            {
                Fn_br_profit_Debug(&sFn_br_profit);
                vtcp_log("登记机构分润表失败[%d]",ret);
                strcpy(g_pub_tx.reply,"D348");
                goto ErrExit;
            }
        }
        else if(ret == 0)
        {
            vtcp_log("有分润信息 更新");
            /*为了支持部分赎回，这里加本次的本金(不是实际归还的)*/
            sFn_br_profit.tx_amt += sFn_reg.tx_amt - sFn_reg.redempt_amt;
            /*赎回金额不用动，因为赎回交易会登记 到期分配收益不会处理赎回金额*/
            sFn_br_profit.profit_amt += dProfit_amt;
            ret = Fn_br_profit_Upd_Upd(sFn_br_profit,g_pub_tx.reply);
            if(ret)
            {
                vtcp_log("更新机构分润表失败");
                strcpy(g_pub_tx.reply,"D346");
                goto ErrExit;
            }
        }
        Fn_br_profit_Clo_Upd();
      }

        /*登记操作纪录*/
        sFn_reg_hst.tx_date = g_pub_tx.tx_date;
        sFn_reg_hst.trace_no = g_pub_tx.trace_no;
        strcpy(sFn_reg_hst.tx_br_no,g_pub_tx.tx_br_no);
        strcpy(sFn_reg_hst.tel,g_pub_tx.tel);
        strcpy(sFn_reg_hst.type,"2");
        sFn_reg_hst.tx_amt = dRbj_amt + dFct_intst;/*记录下实际归还的本金加收益*/
        sFn_reg_hst.buy_date=sFn_reg.tx_date;
        sFn_reg_hst.buy_trace_no=sFn_reg.trace_no;
        strcpy(sFn_reg_hst.brf,"分配收益");
        ret = Fn_reg_hst_Ins(sFn_reg_hst,g_pub_tx.reply);
        if(ret)
        {
            Fn_reg_hst_Debug(&sFn_reg_hst);
            vtcp_log("登记理财产品登记簿错误iRet[%d]",ret);
            strcpy(g_pub_tx.reply,"D335");
            goto ErrExit;
        }
    
        sFn_reg.profit_flag[0] ='Y';
        sFn_reg.return_date=g_pub_tx.tx_date;
        sFn_reg.return_trace_no=g_pub_tx.trace_no;
        sFn_reg.sts[0]='2';
        /*sFn_reg.return_rate = dFct_rate;*/
        ret=Fn_reg_Upd_Upd(sFn_reg,g_pub_tx.reply);
        if(ret)
        {
            vtcp_log("update fn_reg_hst error! ret[%d]  ",ret);
            strcpy(g_pub_tx.reply,"D332");
            goto ErrExit;
        }
        /* 根据合同生成报表清单 */
        if( strcmp(sFn_reg.tx_br_no,cOld_br_no) )
        {
            if( lLine>0 )/*打上一个机构这个产品的尾*/
            {
                iPage++;
                fprintf( fp, "=======================================================================================================================================\n" );
                fprintf( fp, "合计:%8ld笔     %-17.2lf%-17.2lf%-17.2lf%-17.2lf%-17.2lf              第%ld页\n",\
                    sTotal_jg.cnt,sTotal_jg.tx_amt,sTotal_jg.redempt_amt,sTotal_jg.return_capital1,\
                    sTotal_jg.fct_intst1,sTotal_jg.profit_amt1,iPage);
                fprintf(fp,"%c",0x0C);
                /*如果上个机构有纪录，统计到清算中心*/
                sTotal_qs.cnt += sTotal_jg.cnt;
                sTotal_qs.tx_amt+=sTotal_jg.tx_amt;
                sTotal_qs.redempt_amt+= sTotal_jg.redempt_amt;
                sTotal_qs.return_capital1+= sTotal_jg.return_capital1;
                sTotal_qs.fct_intst1 += sTotal_jg.fct_intst1;
                sTotal_qs.profit_amt1+= sTotal_jg.profit_amt1;
                /*写一条这个机构的统计信息*/
                fprintf(fp_qs,"%-6s %-20s%-8ld%-17.2lf%-17.2lf%-17.2lf%-17.2lf%-17.2lf\n",\
                    cOld_br_no,sFn_parm.prdt_code,sTotal_jg.cnt,sTotal_jg.tx_amt,sTotal_jg.redempt_amt,\
                    sTotal_jg.return_capital1,sTotal_jg.fct_intst1,sTotal_jg.profit_amt1);
                lLine_qs ++;
                if(lLine_qs % LINE_PAGE == 0)
                {
                    iPage_qs++;
                    fprintf( fp_qs, "======================================================================================================================\n" );
                    fprintf( fp_qs, "                                                                                                             第%ld页\n",iPage_qs);
                    fprintf(fp_qs,"%c",0x0C);
                }
            }
            memset(&sTotal_jg,0x00,sizeof(sTotal_jg));
            memset(&sCom_branch,0x00,sizeof(sCom_branch));
	            ret=Com_branch_Sel(g_pub_tx.reply,&sCom_branch,"br_no='%s'",sFn_reg.tx_br_no);
	            if(ret)
	            {
	                vtcp_log("select com_branch error! ret[%d]  ",ret);
	                strcpy(g_pub_tx.reply,"T087");
	                goto ErrExit;
	            }
            strcpy(cOld_br_no,sFn_reg.tx_br_no);
            /* 打开新机构文件 */
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
            
            fprintf(fp, "                                            机构[%s]理财产品[%s]分配收益清单\n",cOld_br_no,sFn_parm.prdt_code);
            fprintf(fp, "                                                                                                     日期:%d年%d月%d日\n",g_pub_tx.tx_date/10000,g_pub_tx.tx_date/100%100,g_pub_tx.tx_date%100);
            fprintf(fp, "=======================================================================================================================================\n" );
                fprintf(fp,"%-20s%-17s%-17s%-17s%-17s%-17s%-8s\n","账号","认购金额","赎回金额","返还本金","收益金额","分润金额","摘  要");
            fprintf(fp, "=======================================================================================================================================\n" );
            iPage=0;
            lLine=0;
            /*上个机构的回单换页 下个产品还打到那个文件 防止换页错乱*/
            if(iHdCnt > 0)
            {
                fprintf(fp1,"%c",0x0C);
            }
            /* 打开回单文件 */
            if(fp1 != NULL)
            {
                fclose(fp1);
                fp1 = NULL;
            }
            ret=pub_rpt_append_openfile(&fp1,sFn_reg.tx_br_no,cFilename1);
            if(ret)
            {
                vtcp_log("打开文件失败!");
                strcpy(g_pub_tx.reply,"S066");
                goto ErrExit;
            }
            iHdCnt = 0;/*新机构 回单数量清0*/
        }
        /*统计单个机构这个产品的信息*/
        sTotal_jg.cnt ++;
        sTotal_jg.tx_amt+=sFn_reg.tx_amt;
        sTotal_jg.redempt_amt+= sFn_reg.redempt_amt;
        sTotal_jg.return_capital1+= dRbj_amt;
        sTotal_jg.fct_intst1 += dFct_intst;
        sTotal_jg.profit_amt1 += dProfit_amt;
        
        fprintf(fp,"%-20s%-17.2lf%-17.2f%-17.2lf%-17.2lf%-17.2lf%-8s\n",\
            sFn_reg.ac_no,sFn_reg.tx_amt,sFn_reg.redempt_amt,\
            sFn_reg.return_capital,dFct_intst,dProfit_amt,cBrf);
        lLine++;
        if( lLine%LINE_PAGE==0 )
        {
            iPage++;
            fprintf( fp, "=======================================================================================================================================\n" );
            fprintf( fp, "                                                                                                                         第%ld页\n",iPage);
            fprintf(fp,"%c",0x0C);
        }
        /*回单3份每页*/
        if(iHdCnt % HD_LINE ==0 && iHdCnt > 0)
        {
            fprintf(fp1,"%c",0x0C);
        }
        fprintf(fp1,"\n\n\n\n                                      理财产品收益                         客户回单\n");
        fprintf(fp1,"               认购日期:%d-%d-%d    经办机构:%s\n",\
        sFn_reg.tx_date/10000,sFn_reg.tx_date/100%100,sFn_reg.tx_date%100,sCom_branch.br_name);
        
        fprintf(fp1,"               产品代码:%s\n",sFn_parm.prdt_code);
        fprintf(fp1,"               产品名称:%s\n",sFn_parm.title);
        fprintf(fp1,"               认购客户:%s\n",cName);
        fprintf(fp1,"               认购金额:%-17.2lf  赎回金额:%-17.2lf \n",\
                        sFn_reg.tx_amt,sFn_reg.redempt_amt);
        fprintf(fp1,"               返还本金:%-17.2lf  收益率:%-9.6lf  收益:%-17.2lf\n",\
                        sFn_reg.return_capital,dFct_rate,dFct_intst);
        fprintf(fp1,"               入账账号:%s\n",cAc_no);
        fprintf(fp1,"               入账户名:%s\n",cName);
        fprintf(fp1,"               金额（大写）:%s\n",pub_rpt_pcask(dFct_intst));
        fprintf(fp1,"               金额（小写）:%.2lf\n",dFct_intst);
        fprintf(fp1,"               现转标志:%s\n","转账");
        fprintf(fp1,"               返还本金和收益已转入入账账号\n");
        fprintf(fp1,"               交易日期:%8ld         操作员:%6s         流水号:%-8ld\n",g_pub_tx.tx_date,g_pub_tx.tel,g_pub_tx.trace_no);
        iHdCnt ++;
        
    }
    Fn_reg_Clo_Upd();

    /* fn_parm产品状态修改 */
    sFn_parm.stat[0]='2';
    strcpy(sFn_parm.tel2,g_pub_tx.tel);
    ret=Fn_parm_Upd_Upd(sFn_parm,g_pub_tx.reply);
    if(ret)
    {
        vtcp_log("更新fn_parm失败! ret=[%d]",ret);
        Fn_parm_Clo_Upd();
        strcpy(g_pub_tx.reply,"D325");
        goto ErrExit;
    }
    Fn_parm_Clo_Upd();

OkExit:
    if(fp != NULL)
    {
        fclose(fp);
        fp = NULL;
    }
    if(fp1 != NULL)
    {
        fclose(fp1);
        fp1 = NULL;
    }
    if(fp_qs != NULL)
    {
        fclose(fp_qs);
        fp_qs = NULL;
    }
    /*sql_commit();    --提交事务--*/
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
    if(fp1 != NULL)
    {
        fclose(fp1);
        fp1 = NULL;
    }
    if(fp_qs != NULL)
    {
        fclose(fp_qs);
        fp_qs = NULL;
    }
    /*sql_rollback();   --事务回滚--*/
    vtcp_log("处理失败![%s]", g_pub_tx.reply);
    if(strlen(g_pub_tx.reply) == 0 || strcmp(g_pub_tx.reply,"0000") == 0)
    {
        strcpy(g_pub_tx.reply,"S035");
    }
    set_zd_data("0120", g_pub_tx.reply);
    return 1;
}

/*根据客户账号找到能入账的账号 找不到 就报错shangyongchao 2013/3/9 20:46:46
ac_no 传入旧账号
new_ac_no 根据ac_id找到的状态正常的账号*/
static int my_get_ac_no(char *ac_no,char *new_ac_no ,char *name)
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
        sprintf(acErrMsg, "查找账号失败![%d]", iRet);
        WRITEMSG
        return 1;
    }
    else if(iRet == 100)
    {
        sprintf(acErrMsg, "无此合适入账账号![%s]", ac_no);
        WRITEMSG
        return 1;
    }
    strcpy(new_ac_no,sMdm_ac_rel.ac_no);
    /*strcpy(name,sMdm_ac_rel.name);*/
    return 0;
}

/*内部帐记账*/
static int func_in_acct(char *ac_no,char *dc_ind,double amt)
{
    memset(&g_pub_tx.name,0x00,sizeof(g_pub_tx.name));
    memset(&g_pub_tx.ac_no,0x00,sizeof(g_pub_tx.ac_no));
    g_pub_tx.cif_no = 0;
    strcpy(g_pub_tx.ac_no,ac_no);
    /*流水控制*/
    strcpy(g_pub_tx.ac_wrk_ind,"0000010011");
    vtcp_log("看看g_pub_tx.ac_wrk_ind[%s]",g_pub_tx.ac_wrk_ind);
    g_pub_tx.ac_id = 0;
    g_pub_tx.ac_seqn=0;
    g_pub_tx.tx_amt1=amt;
    g_pub_tx.add_ind[0]=dc_ind[0];
    strcpy(g_pub_tx.hst_ind,"1");               /* 日间入明细 */
    g_pub_tx.svc_ind=9001;
    strcpy(g_pub_tx.ct_ind,"2");                /* 转账 */
    strcpy(g_pub_tx.ac_id_type,"9");
    strcpy(g_pub_tx.note_type,"");
    strcpy(g_pub_tx.beg_note_no,"");
    if ( pub_acct_trance() )
    {
        vtcp_log("调用存取款主控失败!");
        return 1;
    }
    if(dc_ind[0] == '0')
    {
        set_zd_data("102K",g_pub_tx.ct_ind);
        set_zd_double("102I",g_pub_tx.tx_amt1);
        if ( pubf_acct_proc("LC01") )
        {
            vtcp_log("登记会计流水错误!");
            return 1;
        }
    }
    else
    {
        set_zd_data("102K",g_pub_tx.ct_ind);
        set_zd_double("1002",g_pub_tx.tx_amt1);
        if ( pubf_acct_proc("LC02") )
        {
            vtcp_log("登记会计流水错误!");
            return 1;
        }
    }
    return 0;
}
/**挂账处理**/
static int my_get_ac_hold(char *br_no,long ac_id ,int ac_seqn,int *hold_flag,char *ac_no)
{
    int ret = 0;
    struct dd_mst_c sDd_mst;
    struct com_parm_c sCom_parm;
    memset(&sDd_mst,0x00,sizeof(sDd_mst));
    memset(&sCom_parm,0x00,sizeof(sCom_parm));
    
    ret = Dd_mst_Sel(g_pub_tx.reply,&sDd_mst,"ac_id=%ld and ac_seqn=%d",ac_id,ac_seqn);
    if(ret)
    {
        vtcp_log("查询活期主文件失败[%d]",ret);
        strcpy(g_pub_tx.reply,"D230");
        return 1;
    }
    /*全部冻结的导致不能正常入账 返回标志*/
    if(sDd_mst.hold_sts[0] == '1')
    {
        *hold_flag = 1;
        /*取认购机构的挂账账号*/
       /* ret = Com_parm_Sel(g_pub_tx.reply,&sCom_parm,"parm_code='LC_GZ' and parm_seqn=%d",br_no);
        if(ret)
        {
            vtcp_log("查询公共参数表错误[%d]",ret);
            strcpy(g_pub_tx.reply,"P465");
            return 1;
        }
        strcpy(ac_no,sCom_parm.val);*/
    }
    else
    {
        *hold_flag = 0;
    }
    return 0;
}
