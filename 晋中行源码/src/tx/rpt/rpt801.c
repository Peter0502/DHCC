/*************************************************************
* 文 件 名：rpt801.c
* 功能描述：邓惠盈要求新的存贷款情况打印.
*           读取文件 pfm/cdktj801.txt 其中,包含存贷款类型,select语句.---暂不使用此方法.写死存贷款类型.
*           打印内容至 report/RPT801.txt
*           20110610 只打印存贷款类型的统计值,不打印明细.
* 作    者：
* 完成日期：20110610
* 
*
* 修改记录：
* 日    期：
* 修 改 人：
* 修改内容：
**************************************************************/
#define EXTERN   
#define ERR_DEAL if( iRet ) \
	  {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include "public.h"
#include "tx_def_c.h"
#include "com_sys_parm_c.h"
#include "dd_mst_c.h"
#include "td_mst_c.h"
#include "ln_mst_c.h"

rpt801()
{
    int    iRet = 0;
    int    iCount = 0;
    long   lTmpDate = 0;
    double dSum = 0.0;
    char   cFilename[128];
    FILE   *fp = NULL;
    char   sTmpbuf[1024];
    char   sComm[128];
    struct com_sys_parm_c sCom_sys_parm;
    struct ln_mst_c sLn_mst;
    memset(&sCom_sys_parm, 0x00, sizeof(struct com_sys_parm_c));
    memset(&sLn_mst,0x00,sizeof(struct ln_mst_c));
    memset(cFilename,0x00,sizeof(cFilename));
    memset(sTmpbuf,0x00,sizeof(sTmpbuf));
    memset(sComm,0x00,sizeof(sComm));
    
    iRet=sql_begin(); /*打开事务*/
    if( iRet )
    {
        sprintf( acErrMsg, "打开事务失败!!!" );
        WRITEMSG
        goto ErrExit;
    }
    
    /* 初始化全局变量 */
    pub_base_sysinit();
      
    /* 取报表打印日期 */
    iRet = Com_sys_parm_Sel( g_pub_tx.reply, &sCom_sys_parm, "1=1" );
    ERR_DEAL
    
    g_pub_tx.tx_date = sCom_sys_parm.lst_date;
    /**删除文件.jk_cdktj.txt **/
    strcpy(sComm,"rm -f /home/jzht/bin/jk_cdktj.txt");
    system(sComm);
    /**创建文件.jk_cdktj.txt **/
    strcpy(cFilename, "/home/jzht/bin/jk_cdktj.txt" );
    if( (fp = fopen(cFilename,"ab+")) == NULL)
    {
        vtcp_log("[%s][%d]文件读取失败..",__FILE__,__LINE__ );
        strcpy(g_pub_tx.reply,"L206");
        goto ErrExit;
    }
    memset(sTmpbuf,0x00,sizeof(sTmpbuf));
    strcpy(sTmpbuf,"       晋  中  市  商  业  银  行  存  贷  款  分  类  统  计  表       \n");
    fputs(sTmpbuf,fp);
    /**报表日期**/
    vtcp_log("[%s][%d] g_pub_tx.tx_date=[%ld]",__FILE__,__LINE__,g_pub_tx.tx_date);
    memset(sTmpbuf,0x00,sizeof(sTmpbuf));
    sprintf(sTmpbuf,"日期:    %8d      \n",g_pub_tx.tx_date);
    fputs(sTmpbuf,fp);
    memset(sTmpbuf,0x00,sizeof(sTmpbuf));
    strcpy(sTmpbuf,"========================================================================\n");
    fputs(sTmpbuf,fp);
    memset(sTmpbuf,0x00,sizeof(sTmpbuf));
    strcpy(sTmpbuf,"存贷款分类                                   户数               金额合计\n");
    fputs(sTmpbuf,fp);
    /**剩余到期日>=1年的单位定期存款 12个月.**/
    lTmpDate = 0;
    iCount = 0;
    dSum = 0.0;
    pub_base_deadlineM(g_pub_tx.tx_date,12,&lTmpDate);
    vtcp_log("[%s][%d] 12月后=[%ld]",__FILE__,__LINE__,lTmpDate);
		iCount = sql_count("td_mst"," mtr_date >= %ld and ac_sts='1' and cif_no > 50000000 and cif_no < 60000000",lTmpDate);
    iRet = sql_sum_double("td_mst","bal",&dSum," mtr_date >= %ld and ac_sts='1' and cif_no > 50000000 and cif_no < 60000000",lTmpDate);
    if( iRet )
    {
    	sprintf(acErrMsg,"计算存款总金额失败! iRet=[%d]",iRet);
    	WRITEMSG
    	strcpy(g_pub_tx.reply,"D103");
    	goto ErrExit;
    }
    vtcp_log("[%s][%d] iCount=[%ld]",__FILE__,__LINE__,iCount);
    vtcp_log("[%s][%d] dSum=[%17.2f]",__FILE__,__LINE__,dSum);
    memset(sTmpbuf,0x00,sizeof(sTmpbuf));
    sprintf(sTmpbuf,"剩余到期日>=1年的单位定期存款            %8d      %17.2f\n",iCount,dSum);
    fputs(sTmpbuf,fp);
    
    /**jk**/
    
        /**剩余到期日<1年的单位定期存款 12个月.**/
    lTmpDate = 0;
    iCount = 0;
    dSum = 0.0;
    pub_base_deadlineM(g_pub_tx.tx_date,12,&lTmpDate);
    vtcp_log("[%s][%d] 12月后=[%ld]",__FILE__,__LINE__,lTmpDate);
		iCount = sql_count("td_mst"," mtr_date < %ld and ac_sts='1' and cif_no > 50000000 and cif_no < 60000000",lTmpDate);
    iRet = sql_sum_double("td_mst","bal",&dSum," mtr_date < %ld and ac_sts='1' and cif_no > 50000000 and cif_no < 60000000",lTmpDate);
    if( iRet )
    {
    	sprintf(acErrMsg,"计算存款总金额失败! iRet=[%d]",iRet);
    	WRITEMSG
    	strcpy(g_pub_tx.reply,"D103");
    	goto ErrExit;
    }
    vtcp_log("[%s][%d] iCount=[%ld]",__FILE__,__LINE__,iCount);
    vtcp_log("[%s][%d] dSum=[%17.2f]",__FILE__,__LINE__,dSum);
    memset(sTmpbuf,0x00,sizeof(sTmpbuf));
    sprintf(sTmpbuf,"剩余到期日<1年的单位定期存款            %8d      %17.2f\n",iCount,dSum);
    fputs(sTmpbuf,fp);
    /**jk**/
    /**剩余到期日>=1年的定期储蓄存款 12个月.**/
    lTmpDate = 0;
    iCount = 0;
    dSum = 0.0;
    pub_base_deadlineM(g_pub_tx.tx_date,12,&lTmpDate);
    vtcp_log("[%s][%d] 12月后=[%ld]",__FILE__,__LINE__,lTmpDate);
		iCount = sql_count("td_mst"," mtr_date >= %ld and ac_sts='1' and cif_no < 20000000",lTmpDate);
    iRet = sql_sum_double("td_mst","bal",&dSum," mtr_date >= %ld and ac_sts='1' and cif_no < 20000000",lTmpDate);
    if( iRet )
    {
    	sprintf(acErrMsg,"计算存款总金额失败! iRet=[%d]",iRet);
    	WRITEMSG
    	strcpy(g_pub_tx.reply,"D103");
    	goto ErrExit;
    }
    vtcp_log("[%s][%d] iCount=[%ld]",__FILE__,__LINE__,iCount);
    vtcp_log("[%s][%d] dSum=[%17.2f]",__FILE__,__LINE__,dSum);
    memset(sTmpbuf,0x00,sizeof(sTmpbuf));
    sprintf(sTmpbuf,"剩余到期日>=1年的定期储蓄存款            %8d      %17.2f\n",iCount,dSum);
    fputs(sTmpbuf,fp);

    /**剩余到期日<1年的定期储蓄存款 12个月.**/
    lTmpDate = 0;
    iCount = 0;
    dSum = 0.0;
    pub_base_deadlineM(g_pub_tx.tx_date,12,&lTmpDate);
    vtcp_log("[%s][%d] 12月后=[%ld]",__FILE__,__LINE__,lTmpDate);
		iCount = sql_count("td_mst"," mtr_date < %ld and ac_sts='1' and cif_no < 20000000",lTmpDate);
    iRet = sql_sum_double("td_mst","bal",&dSum," mtr_date < %ld and ac_sts='1' and cif_no < 20000000",lTmpDate);
    if( iRet )
    {
    	sprintf(acErrMsg,"计算存款总金额失败! iRet=[%d]",iRet);
    	WRITEMSG
    	strcpy(g_pub_tx.reply,"D103");
    	goto ErrExit;
    }
    vtcp_log("[%s][%d] iCount=[%ld]",__FILE__,__LINE__,iCount);
    vtcp_log("[%s][%d] dSum=[%17.2f]",__FILE__,__LINE__,dSum);
    memset(sTmpbuf,0x00,sizeof(sTmpbuf));
    sprintf(sTmpbuf,"剩余到期日<1年的定期储蓄存款            %8d      %17.2f\n",iCount,dSum);
    fputs(sTmpbuf,fp);
       
    /**剩余到期日<30天的单位定期存款 30天.**/
    lTmpDate = 0;
    iCount = 0;
    dSum = 0.0;
    pub_base_deadlineD(g_pub_tx.tx_date,30,&lTmpDate);
    vtcp_log("[%s][%d] 30天后=[%ld]",__FILE__,__LINE__,lTmpDate);
		iCount = sql_count("td_mst"," mtr_date < %ld and ac_sts='1' and cif_no > 50000000 and cif_no < 60000000",lTmpDate);
    iRet = sql_sum_double("td_mst","bal",&dSum," mtr_date < %ld and ac_sts='1' and cif_no > 50000000 and cif_no < 60000000",lTmpDate);
    if( iRet )
    {
    	sprintf(acErrMsg,"计算存款总金额失败! iRet=[%d]",iRet);
    	WRITEMSG
    	strcpy(g_pub_tx.reply,"D103");
    	goto ErrExit;
    }
    vtcp_log("[%s][%d] iCount=[%ld]",__FILE__,__LINE__,iCount);
    vtcp_log("[%s][%d] dSum=[%17.2f]",__FILE__,__LINE__,dSum);
    memset(sTmpbuf,0x00,sizeof(sTmpbuf));
    sprintf(sTmpbuf,"剩余到期日<30天的单位定期存款            %8d      %17.2f\n",iCount,dSum);
    fputs(sTmpbuf,fp);
    
    /**剩余到期日<30天的定期储蓄存款 30天.**/
    lTmpDate = 0;
    iCount = 0;
    dSum = 0.0;
    pub_base_deadlineD(g_pub_tx.tx_date,30,&lTmpDate);
    vtcp_log("[%s][%d] 30天后=[%ld]",__FILE__,__LINE__,lTmpDate);
		iCount = sql_count("td_mst"," mtr_date < %ld and ac_sts='1' and cif_no < 20000000",lTmpDate);
    iRet = sql_sum_double("td_mst","bal",&dSum," mtr_date < %ld and ac_sts='1' and cif_no < 20000000",lTmpDate);
    if( iRet )
    {
    	sprintf(acErrMsg,"计算存款总金额失败! iRet=[%d]",iRet);
    	WRITEMSG
    	strcpy(g_pub_tx.reply,"D103");
    	goto ErrExit;
    }
    vtcp_log("[%s][%d] iCount=[%ld]",__FILE__,__LINE__,iCount);
    vtcp_log("[%s][%d] dSum=[%17.2f]",__FILE__,__LINE__,dSum);
    memset(sTmpbuf,0x00,sizeof(sTmpbuf));
    sprintf(sTmpbuf,"剩余到期日<30天的定期储蓄存款            %8d      %17.2f\n",iCount,dSum);
    fputs(sTmpbuf,fp);
    
    /**活期储蓄存款（有贷款的）**/
    lTmpDate = 0;
    iCount = 0;
    dSum = 0.0;
    iCount = sql_count("dd_mst"," cif_no in(select cif_no from ln_mst where ac_sts!='*' and cif_no<20000000) and ac_type='5' and ac_sts='1' ");
    iRet = sql_sum_double("dd_mst","bal",&dSum," cif_no in(select cif_no from ln_mst where ac_sts!='*' and cif_no<20000000) and ac_type='5' and ac_sts='1' ");
    if( iRet )
    {
    	sprintf(acErrMsg,"计算存款总金额失败! iRet=[%d]",iRet);
    	WRITEMSG
    	strcpy(g_pub_tx.reply,"D103");
    	goto ErrExit;
    }
    vtcp_log("[%s][%d] iCount=[%ld]",__FILE__,__LINE__,iCount);
    vtcp_log("[%s][%d] dSum=[%17.2f]",__FILE__,__LINE__,dSum);
    memset(sTmpbuf,0x00,sizeof(sTmpbuf));
    sprintf(sTmpbuf,"活期储蓄存款（有贷款的）                 %8d      %17.2f\n",iCount,dSum);
    fputs(sTmpbuf,fp);
    
    /**对公活期存款（有贷款的）**/
    lTmpDate = 0;
    iCount = 0;
    dSum = 0.0;
    iCount = sql_count("dd_mst"," cif_no in(select cif_no from ln_mst where ac_sts!='*' and cif_no>50000000 and cif_no<60000000) and ac_type in('1','2','3','4') and ac_sts='1' ");
    iRet = sql_sum_double("dd_mst","bal",&dSum," cif_no in(select cif_no from ln_mst where ac_sts!='*' and cif_no>50000000 and cif_no<60000000) and ac_type in('1','2','3','4') and ac_sts='1' ");
    if( iRet )
    {
    	sprintf(acErrMsg,"计算存款总金额失败! iRet=[%d]",iRet);
    	WRITEMSG
    	strcpy(g_pub_tx.reply,"D103");
    	goto ErrExit;
    }
    vtcp_log("[%s][%d] iCount=[%ld]",__FILE__,__LINE__,iCount);
    vtcp_log("[%s][%d] dSum=[%17.2f]",__FILE__,__LINE__,dSum);
    memset(sTmpbuf,0x00,sizeof(sTmpbuf));
    sprintf(sTmpbuf,"对公活期存款（有贷款的）                 %8d      %17.2f\n",iCount,dSum);
    fputs(sTmpbuf,fp);
    
    /**山西和田物资集团有限公司  活期存款**/
    lTmpDate = 0;
    iCount = 0;
    dSum = 0.0;
    iCount = sql_count("dd_mst"," cif_no='50082098' and ac_type in('1','2','3','4') and ac_sts='1' ");
    iRet = sql_sum_double("dd_mst","bal",&dSum," cif_no in(select cif_no from dd_mst where name='山西和田物资集团有限公司') and ac_type in('1','2','3','4') and ac_sts='1' ");
    if( iRet )
    {
    	sprintf(acErrMsg,"计算存款总金额失败! iRet=[%d]",iRet);
    	WRITEMSG
    	strcpy(g_pub_tx.reply,"D103");
    	goto ErrExit;
    }
    vtcp_log("[%s][%d] iCount=[%ld]",__FILE__,__LINE__,iCount);
    vtcp_log("[%s][%d] dSum=[%17.2f]",__FILE__,__LINE__,dSum);
    memset(sTmpbuf,0x00,sizeof(sTmpbuf));
    sprintf(sTmpbuf,"山西和田物资集团有限公司 活期存款        %8d      %17.2f\n",iCount,dSum);
    fputs(sTmpbuf,fp);    
    
    /**山西和田物资集团有限公司  定期存款 **/
    lTmpDate = 0;
    iCount = 0;
    dSum = 0.0;
    iCount = sql_count("td_mst"," cif_no in(select cif_no from td_mst where name='山西和田物资集团有限公司') and ac_sts='1' ");
    iRet = sql_sum_double("td_mst","bal",&dSum," cif_no in(select cif_no from td_mst where name='山西和田物资集团有限公司')  and ac_sts='1' ");
    if( iRet )
    {
    	sprintf(acErrMsg,"计算存款总金额失败! iRet=[%d]",iRet);
    	WRITEMSG
    	strcpy(g_pub_tx.reply,"D103");
    	goto ErrExit;
    }
    vtcp_log("[%s][%d] iCount=[%ld]",__FILE__,__LINE__,iCount);
    vtcp_log("[%s][%d] dSum=[%17.2f]",__FILE__,__LINE__,dSum);
    memset(sTmpbuf,0x00,sizeof(sTmpbuf));
    sprintf(sTmpbuf,"山西和田物资集团有限公司 定期存款        %8d      %17.2f\n",iCount,dSum);
    fputs(sTmpbuf,fp);    
      
    /**山西田森超市集团有限公司  活期存款**/
     lTmpDate = 0;
    iCount = 0;
    dSum = 0.0;
    iCount = sql_count("dd_mst"," cif_no='50082098' and ac_type in('1','2','3','4') and ac_sts='1' ");
    iRet = sql_sum_double("dd_mst","bal",&dSum," cif_no in(select cif_no from dd_mst where name='山西田森超市集团有限公司') and ac_type in('1','2','3','4') and ac_sts='1' ");
    if( iRet )
    {
    	sprintf(acErrMsg,"计算存款总金额失败! iRet=[%d]",iRet);
    	WRITEMSG
    	strcpy(g_pub_tx.reply,"D103");
    	goto ErrExit;
    }
    vtcp_log("[%s][%d] iCount=[%ld]",__FILE__,__LINE__,iCount);
    vtcp_log("[%s][%d] dSum=[%17.2f]",__FILE__,__LINE__,dSum);
    memset(sTmpbuf,0x00,sizeof(sTmpbuf));
    sprintf(sTmpbuf,"山西田森超市集团有限公司 活期存款        %8d      %17.2f\n",iCount,dSum);
    fputs(sTmpbuf,fp);   
    
    /**山西田森超市集团有限公司  定期存款**/
    
    lTmpDate = 0;
    iCount = 0;
    dSum = 0.0;
    iCount = sql_count("td_mst"," cif_no in(select cif_no from td_mst where name='山西田森超市集团有限公司') and ac_sts='1' ");
    iRet = sql_sum_double("td_mst","bal",&dSum," cif_no in(select cif_no from td_mst where name='山西田森超市集团有限公司')  and ac_sts='1' ");
    if( iRet )
    {
    	sprintf(acErrMsg,"计算存款总金额失败! iRet=[%d]",iRet);
    	WRITEMSG
    	strcpy(g_pub_tx.reply,"D103");
    	goto ErrExit;
    }
    vtcp_log("[%s][%d] iCount=[%ld]",__FILE__,__LINE__,iCount);
    vtcp_log("[%s][%d] dSum=[%17.2f]",__FILE__,__LINE__,dSum);
    memset(sTmpbuf,0x00,sizeof(sTmpbuf));
    sprintf(sTmpbuf,"山西田森超市集团有限公司 定期存款        %8d      %17.2f\n",iCount,dSum);
    fputs(sTmpbuf,fp);
        
    /**一年内到期的单位贷款**/
    lTmpDate = 0;
    iCount = 0;
    dSum = 0.0;
    pub_base_deadlineM(g_pub_tx.tx_date,12,&lTmpDate);
    vtcp_log("[%s][%d] 12月后=[%ld]",__FILE__,__LINE__,lTmpDate);
		iCount = sql_count("ln_mst"," mtr_date < %ld and ac_sts='1' and cif_no > 50000000 and cif_no < 60000000",lTmpDate);
    iRet = sql_sum_double("ln_mst","bal",&dSum," mtr_date < %ld and ac_sts='1' and cif_no > 50000000 and cif_no < 60000000",lTmpDate);
    if( iRet )
    {
    	sprintf(acErrMsg,"计算存款总金额失败! iRet=[%d]",iRet);
    	WRITEMSG
    	strcpy(g_pub_tx.reply,"D103");
    	goto ErrExit;
    }
    vtcp_log("[%s][%d] iCount=[%ld]",__FILE__,__LINE__,iCount);
    vtcp_log("[%s][%d] dSum=[%17.2f]",__FILE__,__LINE__,dSum);
    memset(sTmpbuf,0x00,sizeof(sTmpbuf));
    sprintf(sTmpbuf,"一年内到期的单位贷款                     %8d      %17.2f\n",iCount,dSum);
    fputs(sTmpbuf,fp);
    
    /**一年内到期的单位贷款利息**/
    lTmpDate = 0;
    iCount = 0;
    dSum = 0.0;
    pub_base_deadlineM(g_pub_tx.tx_date,12,&lTmpDate);
    vtcp_log("[%s][%d] 12月后=[%ld]",__FILE__,__LINE__,lTmpDate);
		iCount = sql_count("ln_mst"," mtr_date < %ld and ac_sts='1' and cif_no > 50000000 and cif_no < 60000000",lTmpDate);
    /*iSum 需要逐条计算.*/
    iRet = Ln_mst_Dec_Sel(g_pub_tx.reply, " mtr_date < %ld and ac_sts='1' and cif_no > 50000000 and cif_no < 60000000 and intst_type!='0' ",lTmpDate);
    if( iRet ) 
    {
        sprintf(acErrMsg,"DECLARE FOR SELECT ERROR !!iRet = [%d]",iRet);
        WRITEMSG
        goto ErrExit;
    }

    while(1)
    {
        iRet = Ln_mst_Fet_Sel( &sLn_mst, g_pub_tx.reply );
        if (iRet != 0 && iRet != 100)
        {
            sprintf(acErrMsg,"FETCH ERROR !! iRet = [%d]",iRet);
            WRITEMSG
            goto ErrExit;
        }
        else if (iRet == 100)
        {
            break;
        }
        
        dSum = dSum + ((sLn_mst.bal*sLn_mst.rate)/L100)*pub_base_CalTrueDays(g_pub_tx.tx_date,sLn_mst.mtr_date)/L360;
        /**
        vtcp_log("[%s][%d] bal  =[%17.2f]",__FILE__,__LINE__,sLn_mst.bal);
        vtcp_log("[%s][%d] intst=[%17.2f]",__FILE__,__LINE__,((sLn_mst.bal*sLn_mst.rate)/L100)*pub_base_CalTrueDays(g_pub_tx.tx_date,sLn_mst.mtr_date)/L360);
        vtcp_log("L100=[%d],L360=[%d]",L100,L360);
        vtcp_log("bal=[%17.2f] rate=[%9.6f],days=[%d]",sLn_mst.bal,sLn_mst.rate,pub_base_CalTrueDays(g_pub_tx.tx_date,sLn_mst.mtr_date));
        **/
    }
    Ln_mst_Clo_Sel();
    vtcp_log("[%s][%d] iCount=[%ld]",__FILE__,__LINE__,iCount);
    vtcp_log("[%s][%d] dSum=[%17.2f]",__FILE__,__LINE__,dSum);
    memset(sTmpbuf,0x00,sizeof(sTmpbuf));
    sprintf(sTmpbuf,"一年内到期的单位贷款利息                 %8d      %17.2f\n",iCount,dSum);
    fputs(sTmpbuf,fp);
    
    /**其中：剩余到期日<30天的单位贷款**/
    lTmpDate = 0;
    iCount = 0;
    dSum = 0.0;
    pub_base_deadlineD(g_pub_tx.tx_date,30,&lTmpDate);
    vtcp_log("[%s][%d] 30天后=[%ld]",__FILE__,__LINE__,lTmpDate);
		iCount = sql_count("ln_mst"," mtr_date < %ld and ac_sts='1' and cif_no > 50000000 and cif_no < 60000000",lTmpDate);
    iRet = sql_sum_double("ln_mst","bal",&dSum," mtr_date < %ld and ac_sts='1' and cif_no > 50000000 and cif_no < 60000000",lTmpDate);
    if( iRet )
    {
    	sprintf(acErrMsg,"计算存款总金额失败! iRet=[%d]",iRet);
    	WRITEMSG
    	strcpy(g_pub_tx.reply,"D103");
    	goto ErrExit;
    }
    vtcp_log("[%s][%d] iCount=[%ld]",__FILE__,__LINE__,iCount);
    vtcp_log("[%s][%d] dSum=[%17.2f]",__FILE__,__LINE__,dSum);
    memset(sTmpbuf,0x00,sizeof(sTmpbuf));
    sprintf(sTmpbuf,"其中：剩余到期日<30天的单位贷款          %8d      %17.2f\n",iCount,dSum);
    fputs(sTmpbuf,fp);
    
    /**其中：剩余到期日<30天的单位贷款利息**/
    lTmpDate = 0;
    iCount = 0;
    dSum = 0.0;
    pub_base_deadlineD(g_pub_tx.tx_date,30,&lTmpDate);
    vtcp_log("[%s][%d] 30天后=[%ld]",__FILE__,__LINE__,lTmpDate);
		iCount = sql_count("ln_mst"," mtr_date < %ld and ac_sts='1' and cif_no > 50000000 and cif_no < 60000000",lTmpDate);
    /*iSum 需要逐条计算.*/
    iRet = Ln_mst_Dec_Sel(g_pub_tx.reply, " mtr_date < %ld and ac_sts='1' and cif_no > 50000000 and cif_no < 60000000 and intst_type!='0' ",lTmpDate);
    if( iRet ) 
    {
        sprintf(acErrMsg,"DECLARE FOR SELECT ERROR !!iRet = [%d]",iRet);
        WRITEMSG
        goto ErrExit;
    }

    while(1)
    {
        iRet = Ln_mst_Fet_Sel( &sLn_mst, g_pub_tx.reply );
        if (iRet != 0 && iRet != 100)
        {
            sprintf(acErrMsg,"FETCH ERROR !! iRet = [%d]",iRet);
            WRITEMSG
            goto ErrExit;
        }
        else if (iRet == 100)
        {
            break;
        }
        
        dSum = dSum + ((sLn_mst.bal*sLn_mst.rate)/L100)*pub_base_CalTrueDays(g_pub_tx.tx_date,sLn_mst.mtr_date)/L360;
        /**
        vtcp_log("[%s][%d] bal  =[%17.2f]",__FILE__,__LINE__,sLn_mst.bal);
        vtcp_log("[%s][%d] intst=[%17.2f]",__FILE__,__LINE__,((sLn_mst.bal*sLn_mst.rate)/L100)*pub_base_CalTrueDays(g_pub_tx.tx_date,sLn_mst.mtr_date)/L360);
        vtcp_log("L100=[%d],L360=[%d]",L100,L360);
        vtcp_log("bal=[%17.2f] rate=[%9.6f],days=[%d]",sLn_mst.bal,sLn_mst.rate,pub_base_CalTrueDays(g_pub_tx.tx_date,sLn_mst.mtr_date));
        **/
    }
    Ln_mst_Clo_Sel();
    vtcp_log("[%s][%d] iCount=[%ld]",__FILE__,__LINE__,iCount);
    vtcp_log("[%s][%d] dSum=[%17.2f]",__FILE__,__LINE__,dSum);
    memset(sTmpbuf,0x00,sizeof(sTmpbuf));
    sprintf(sTmpbuf,"其中：剩余到期日<30天的单位贷款利息      %8d      %17.2f\n",iCount,dSum);
    fputs(sTmpbuf,fp);
    
    /**一年内到期的个人贷款**/
    lTmpDate = 0;
    iCount = 0;
    dSum = 0.0;
    pub_base_deadlineM(g_pub_tx.tx_date,12,&lTmpDate);
    vtcp_log("[%s][%d] 12月后=[%ld]",__FILE__,__LINE__,lTmpDate);
		iCount = sql_count("ln_mst"," mtr_date < %ld and ac_sts='1' and cif_no < 20000000",lTmpDate);
    iRet = sql_sum_double("ln_mst","bal",&dSum," mtr_date < %ld and ac_sts='1' and cif_no < 20000000",lTmpDate);
    if( iRet )
    {
    	sprintf(acErrMsg,"计算存款总金额失败! iRet=[%d]",iRet);
    	WRITEMSG
    	strcpy(g_pub_tx.reply,"D103");
    	goto ErrExit;
    }
    vtcp_log("[%s][%d] iCount=[%ld]",__FILE__,__LINE__,iCount);
    vtcp_log("[%s][%d] dSum=[%17.2f]",__FILE__,__LINE__,dSum);
    memset(sTmpbuf,0x00,sizeof(sTmpbuf));
    sprintf(sTmpbuf,"一年内到期的个人贷款                     %8d      %17.2f\n",iCount,dSum);
    fputs(sTmpbuf,fp);
    
    /**一年内到期的个人贷款利息**/
    lTmpDate = 0;
    iCount = 0;
    dSum = 0.0;
    pub_base_deadlineM(g_pub_tx.tx_date,12,&lTmpDate);
    vtcp_log("[%s][%d] 12月后=[%ld]",__FILE__,__LINE__,lTmpDate);
		iCount = sql_count("ln_mst"," mtr_date < %ld and ac_sts='1' and cif_no < 20000000",lTmpDate);
    /*iSum 需要逐条计算.*/
    iRet = Ln_mst_Dec_Sel(g_pub_tx.reply, " mtr_date < %ld and ac_sts='1' and cif_no < 20000000 and intst_type!='0' ",lTmpDate);
    if( iRet ) 
    {
        sprintf(acErrMsg,"DECLARE FOR SELECT ERROR !!iRet = [%d]",iRet);
        WRITEMSG
        goto ErrExit;
    }

    while(1)
    {
        iRet = Ln_mst_Fet_Sel( &sLn_mst, g_pub_tx.reply );
        if (iRet != 0 && iRet != 100)
        {
            sprintf(acErrMsg,"FETCH ERROR !! iRet = [%d]",iRet);
            WRITEMSG
            goto ErrExit;
        }
        else if (iRet == 100)
        {
            break;
        }
        
        dSum = dSum + ((sLn_mst.bal*sLn_mst.rate)/L100)*pub_base_CalTrueDays(g_pub_tx.tx_date,sLn_mst.mtr_date)/L360;
        /**
        vtcp_log("[%s][%d] bal  =[%17.2f]",__FILE__,__LINE__,sLn_mst.bal);
        vtcp_log("[%s][%d] intst=[%17.2f]",__FILE__,__LINE__,((sLn_mst.bal*sLn_mst.rate)/L100)*pub_base_CalTrueDays(g_pub_tx.tx_date,sLn_mst.mtr_date)/L360);
        vtcp_log("L100=[%d],L360=[%d]",L100,L360);
        vtcp_log("bal=[%17.2f] rate=[%9.6f],days=[%d]",sLn_mst.bal,sLn_mst.rate,pub_base_CalTrueDays(g_pub_tx.tx_date,sLn_mst.mtr_date));
        **/
    }
    Ln_mst_Clo_Sel();
    vtcp_log("[%s][%d] iCount=[%ld]",__FILE__,__LINE__,iCount);
    vtcp_log("[%s][%d] dSum=[%17.2f]",__FILE__,__LINE__,dSum);
    memset(sTmpbuf,0x00,sizeof(sTmpbuf));
    sprintf(sTmpbuf,"一年内到期的个人贷款利息                 %8d      %17.2f\n",iCount,dSum);
    fputs(sTmpbuf,fp);
    
    /**其中：剩余到期日<30天的个人贷款**/
    lTmpDate = 0;
    iCount = 0;
    dSum = 0.0;
    pub_base_deadlineD(g_pub_tx.tx_date,30,&lTmpDate);
    vtcp_log("[%s][%d] 30天后=[%ld]",__FILE__,__LINE__,lTmpDate);
		iCount = sql_count("ln_mst"," mtr_date < %ld and ac_sts='1' and cif_no < 20000000",lTmpDate);
    iRet = sql_sum_double("ln_mst","bal",&dSum," mtr_date < %ld and ac_sts='1' and cif_no < 20000000",lTmpDate);
    if( iRet )
    {
    	sprintf(acErrMsg,"计算存款总金额失败! iRet=[%d]",iRet);
    	WRITEMSG
    	strcpy(g_pub_tx.reply,"D103");
    	goto ErrExit;
    }
    vtcp_log("[%s][%d] iCount=[%ld]",__FILE__,__LINE__,iCount);
    vtcp_log("[%s][%d] dSum=[%17.2f]",__FILE__,__LINE__,dSum);
    memset(sTmpbuf,0x00,sizeof(sTmpbuf));
    sprintf(sTmpbuf,"其中：剩余到期日<30天的个人贷款          %8d      %17.2f\n",iCount,dSum);
    fputs(sTmpbuf,fp);
    
    /**其中：剩余到期日<30天的个人贷款利息**/
    lTmpDate = 0;
    iCount = 0;
    dSum = 0.0;
    pub_base_deadlineD(g_pub_tx.tx_date,30,&lTmpDate);
    vtcp_log("[%s][%d] 30天后=[%ld]",__FILE__,__LINE__,lTmpDate);
		iCount = sql_count("ln_mst"," mtr_date < %ld and ac_sts='1' and cif_no < 20000000",lTmpDate);
    /*iSum 需要逐条计算.*/
    iRet = Ln_mst_Dec_Sel(g_pub_tx.reply, " mtr_date < %ld and ac_sts='1' and cif_no < 20000000 and intst_type!='0' ",lTmpDate);
    if( iRet ) 
    {
        sprintf(acErrMsg,"DECLARE FOR SELECT ERROR !!iRet = [%d]",iRet);
        WRITEMSG
        goto ErrExit;
    }

    while(1)
    {
        iRet = Ln_mst_Fet_Sel( &sLn_mst, g_pub_tx.reply );
        if (iRet != 0 && iRet != 100)
        {
            sprintf(acErrMsg,"FETCH ERROR !! iRet = [%d]",iRet);
            WRITEMSG
            goto ErrExit;
        }
        else if (iRet == 100)
        {
            break;
        }
        
        dSum = dSum + ((sLn_mst.bal*sLn_mst.rate)/L100)*pub_base_CalTrueDays(g_pub_tx.tx_date,sLn_mst.mtr_date)/L360;
        /**
        vtcp_log("[%s][%d] bal  =[%17.2f]",__FILE__,__LINE__,sLn_mst.bal);
        vtcp_log("[%s][%d] intst=[%17.2f]",__FILE__,__LINE__,((sLn_mst.bal*sLn_mst.rate)/L100)*pub_base_CalTrueDays(g_pub_tx.tx_date,sLn_mst.mtr_date)/L360);
        vtcp_log("L100=[%d],L360=[%d]",L100,L360);
        vtcp_log("bal=[%17.2f] rate=[%9.6f],days=[%d]",sLn_mst.bal,sLn_mst.rate,pub_base_CalTrueDays(g_pub_tx.tx_date,sLn_mst.mtr_date));
        **/
    }
    Ln_mst_Clo_Sel();
    vtcp_log("[%s][%d] iCount=[%ld]",__FILE__,__LINE__,iCount);
    vtcp_log("[%s][%d] dSum=[%17.2f]",__FILE__,__LINE__,dSum);
    memset(sTmpbuf,0x00,sizeof(sTmpbuf));
    sprintf(sTmpbuf,"其中：剩余到期日<30天的个人贷款利息      %8d      %17.2f\n",iCount,dSum);
    fputs(sTmpbuf,fp);
    
    /**一年以上（含一年）到期的单位贷款**/
    lTmpDate = 0;
    iCount = 0;
    dSum = 0.0;
    pub_base_deadlineM(g_pub_tx.tx_date,12,&lTmpDate);
    vtcp_log("[%s][%d] 12月后=[%ld]",__FILE__,__LINE__,lTmpDate);
		iCount = sql_count("ln_mst"," mtr_date >= %ld and ac_sts='1' and cif_no > 50000000 and cif_no < 60000000",lTmpDate);
    iRet = sql_sum_double("ln_mst","bal",&dSum," mtr_date >= %ld and ac_sts='1' and cif_no > 50000000 and cif_no < 60000000",lTmpDate);
    if( iRet )
    {
    	sprintf(acErrMsg,"计算存款总金额失败! iRet=[%d]",iRet);
    	WRITEMSG
    	strcpy(g_pub_tx.reply,"D103");
    	goto ErrExit;
    }
    vtcp_log("[%s][%d] iCount=[%ld]",__FILE__,__LINE__,iCount);
    vtcp_log("[%s][%d] dSum=[%17.2f]",__FILE__,__LINE__,dSum);
    memset(sTmpbuf,0x00,sizeof(sTmpbuf));
    sprintf(sTmpbuf,"一年以上（含一年）到期的单位贷款         %8d      %17.2f\n",iCount,dSum);
    fputs(sTmpbuf,fp);
    
    /**一年以上（含一年）到期的单位贷款利息**/
    lTmpDate = 0;
    iCount = 0;
    dSum = 0.0;
    pub_base_deadlineM(g_pub_tx.tx_date,12,&lTmpDate);
    vtcp_log("[%s][%d] 12月后=[%ld]",__FILE__,__LINE__,lTmpDate);
		iCount = sql_count("ln_mst"," mtr_date >= %ld and ac_sts='1' and cif_no > 50000000 and cif_no < 60000000",lTmpDate);
    /*iSum 需要逐条计算.*/
    iRet = Ln_mst_Dec_Sel(g_pub_tx.reply, " mtr_date >= %ld and ac_sts='1' and cif_no > 50000000 and cif_no < 60000000 and intst_type!='0' ",lTmpDate);
    if( iRet ) 
    {
        sprintf(acErrMsg,"DECLARE FOR SELECT ERROR !!iRet = [%d]",iRet);
        WRITEMSG
        goto ErrExit;
    }

    while(1)
    {
        iRet = Ln_mst_Fet_Sel( &sLn_mst, g_pub_tx.reply );
        if (iRet != 0 && iRet != 100)
        {
            sprintf(acErrMsg,"FETCH ERROR !! iRet = [%d]",iRet);
            WRITEMSG
            goto ErrExit;
        }
        else if (iRet == 100)
        {
            break;
        }
        
        dSum = dSum + ((sLn_mst.bal*sLn_mst.rate)/L100)*pub_base_CalTrueDays(g_pub_tx.tx_date,sLn_mst.mtr_date)/L360;
        /**
        vtcp_log("[%s][%d] bal  =[%17.2f]",__FILE__,__LINE__,sLn_mst.bal);
        vtcp_log("[%s][%d] intst=[%17.2f]",__FILE__,__LINE__,((sLn_mst.bal*sLn_mst.rate)/L100)*pub_base_CalTrueDays(g_pub_tx.tx_date,sLn_mst.mtr_date)/L360);
        vtcp_log("L100=[%d],L360=[%d]",L100,L360);
        vtcp_log("bal=[%17.2f] rate=[%9.6f],days=[%d]",sLn_mst.bal,sLn_mst.rate,pub_base_CalTrueDays(g_pub_tx.tx_date,sLn_mst.mtr_date));
        **/
    }
    Ln_mst_Clo_Sel();
    vtcp_log("[%s][%d] iCount=[%ld]",__FILE__,__LINE__,iCount);
    vtcp_log("[%s][%d] dSum=[%17.2f]",__FILE__,__LINE__,dSum);
    memset(sTmpbuf,0x00,sizeof(sTmpbuf));
    sprintf(sTmpbuf,"一年以上（含一年）到期的单位贷款利息     %8d      %17.2f\n",iCount,dSum);
    fputs(sTmpbuf,fp);
    
    /**一年以上（含一年）到期的个人贷款**/
    lTmpDate = 0;
    iCount = 0;
    dSum = 0.0;
    pub_base_deadlineM(g_pub_tx.tx_date,12,&lTmpDate);
    vtcp_log("[%s][%d] 12月后=[%ld]",__FILE__,__LINE__,lTmpDate);
		iCount = sql_count("ln_mst"," mtr_date >= %ld and ac_sts='1' and cif_no < 20000000",lTmpDate);
    iRet = sql_sum_double("ln_mst","bal",&dSum," mtr_date >= %ld and ac_sts='1' and cif_no < 20000000",lTmpDate);
    if( iRet )
    {
    	sprintf(acErrMsg,"计算存款总金额失败! iRet=[%d]",iRet);
    	WRITEMSG
    	strcpy(g_pub_tx.reply,"D103");
    	goto ErrExit;
    }
    vtcp_log("[%s][%d] iCount=[%ld]",__FILE__,__LINE__,iCount);
    vtcp_log("[%s][%d] dSum=[%17.2f]",__FILE__,__LINE__,dSum);
    memset(sTmpbuf,0x00,sizeof(sTmpbuf));
    sprintf(sTmpbuf,"一年以上（含一年）到期的个人贷款         %8d      %17.2f\n",iCount,dSum);
    fputs(sTmpbuf,fp);
    
    /**一年以上（含一年）到期的个人贷款利息**/
    lTmpDate = 0;
    iCount = 0;
    dSum = 0.0;
    pub_base_deadlineM(g_pub_tx.tx_date,12,&lTmpDate);
    vtcp_log("[%s][%d] 12月后=[%ld]",__FILE__,__LINE__,lTmpDate);
		iCount = sql_count("ln_mst"," mtr_date >= %ld and ac_sts='1' and cif_no < 20000000",lTmpDate);
    /*iSum 需要逐条计算.*/
    iRet = Ln_mst_Dec_Sel(g_pub_tx.reply, " mtr_date >= %ld and ac_sts='1' and cif_no < 20000000 and intst_type!='0' ",lTmpDate);
    if( iRet ) 
    {
        sprintf(acErrMsg,"DECLARE FOR SELECT ERROR !!iRet = [%d]",iRet);
        WRITEMSG
        goto ErrExit;
    }

    while(1)
    {
        iRet = Ln_mst_Fet_Sel( &sLn_mst, g_pub_tx.reply );
        if (iRet != 0 && iRet != 100)
        {
            sprintf(acErrMsg,"FETCH ERROR !! iRet = [%d]",iRet);
            WRITEMSG
            goto ErrExit;
        }
        else if (iRet == 100)
        {
            break;
        }
        
        dSum = dSum + ((sLn_mst.bal*sLn_mst.rate)/L100)*pub_base_CalTrueDays(g_pub_tx.tx_date,sLn_mst.mtr_date)/L360;
        /**
        vtcp_log("[%s][%d] bal  =[%17.2f]",__FILE__,__LINE__,sLn_mst.bal);
        vtcp_log("[%s][%d] intst=[%17.2f]",__FILE__,__LINE__,((sLn_mst.bal*sLn_mst.rate)/L100)*pub_base_CalTrueDays(g_pub_tx.tx_date,sLn_mst.mtr_date)/L360);
        vtcp_log("L100=[%d],L360=[%d]",L100,L360);
        vtcp_log("bal=[%17.2f] rate=[%9.6f],days=[%d]",sLn_mst.bal,sLn_mst.rate,pub_base_CalTrueDays(g_pub_tx.tx_date,sLn_mst.mtr_date));
        **/
    }
    Ln_mst_Clo_Sel();
    vtcp_log("[%s][%d] iCount=[%ld]",__FILE__,__LINE__,iCount);
    vtcp_log("[%s][%d] dSum=[%17.2f]",__FILE__,__LINE__,dSum);
    memset(sTmpbuf,0x00,sizeof(sTmpbuf));
    sprintf(sTmpbuf,"一年以上（含一年）到期的个人贷款利息     %8d      %17.2f\n",iCount,dSum);
    fputs(sTmpbuf,fp);
    

    
    fclose(fp);
    
    
OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"银行存贷款情况打印成功!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--事务回滚--*/
	sprintf(acErrMsg,"银行存贷款情况打印失败!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

