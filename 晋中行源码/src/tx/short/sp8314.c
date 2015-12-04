/***************************************************************
* 文 件 名:  sp8314.c
* 功能描述:  遂心存明细查询	
* 作    者:  nx

* 完成日期：    
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "mo_sxc_c.h"
#include "com_rate_c.h"
#include "mo_sxc_hst_c.h"
#include "mo_sxc_hst_intst_c.h"

int
sp8314()
{   	
    int    iRet=0;
    int    iAc_seqn=1;
    long   lBegin_date=0;
    long   lEnd_date=0;
    double  total =0.00;
    double  intst=0.00;

    char    cFlag[2];	/*状态标志*/
    char    cFlag_1[2];/*是否查询释放明细标志*/
    char    cSts[5];
    char    cType[9];
    char    cAc_no[25];
    char    sTmp[1024];
    char    sFilename[1024];
    char    sWherelist[1024];
    char    sWherelist1[1024];
    FILE *  fp =NULL;
    
    struct    mo_sxc_c              sMo_sxc;
    struct    com_rate_c              sCom_rate;
    struct    mo_sxc_hst_c          sMo_sxc_hst;
    struct    mo_sxc_hst_intst_c    sMo_sxc_hst_intst;
    struct    mdm_ac_rel_c          sMdm_ac_rel;
    
    memset(cFlag,   0x00, sizeof(cFlag));
    memset(cFlag_1, 0x00, sizeof(cFlag_1));
    memset(cSts,    0x00, sizeof(cSts));
    memset(cAc_no,  0x00, sizeof(cAc_no));
    memset(cType,   0x00, sizeof(cType));
    memset(sWherelist,   0x00, sizeof(sWherelist));
    memset(sWherelist1,   0x00, sizeof(sWherelist1));
    memset(sFilename,    0x00, sizeof(sFilename));
    memset(&sMo_sxc,     0x00, sizeof(sMo_sxc));
    memset(&sCom_rate,     0x00, sizeof(sCom_rate));
    memset(&sMo_sxc_hst, 0x00, sizeof(sMo_sxc_hst));
    memset(&sMdm_ac_rel, 0x00, sizeof(sMdm_ac_rel));
    memset(&sMo_sxc_hst_intst, 0x00, sizeof(sMo_sxc_hst_intst));
    
    pub_base_sysinit();
    get_zd_data("0310", cAc_no);
    get_zd_long("0440", &lBegin_date);
    get_zd_long("0450", &lEnd_date);
    get_zd_data("0670", cFlag);
    get_zd_data("0680", cFlag_1);
    pub_base_strpack(cAc_no);
    pub_base_old_acno(cAc_no);	/**对旧帐号的处理**/

    /*根据ac_no从表mdm_ac_rel中获取ac_id*/
    iRet =Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdm_ac_rel, "ac_no='%s'", cAc_no);
    if(iRet == 100)
    {
        vtcp_log("[%s][%d]无此卡号，请检查！！ac_no=[%s]", __FILE__, __LINE__, cAc_no);
    	strcpy(g_pub_tx.reply, "M003");
    	goto ErrExit;
    }
    else if(iRet)
    {
    	vtcp_log("[%s][%d]，iRet=[%d]", __FILE__, __LINE__, iRet);
    	strcpy(g_pub_tx.reply, "W011");
    	goto ErrExit;	
    }
    
    /* 生成下传全路经文件名(批量) */
    pub_base_AllDwnFilName(sFilename);
    fp = fopen(sFilename, "w");
    if (fp == NULL) 
    {
                sprintf(acErrMsg, "open file error [%s]", sFilename);
		WRITEMSG
		strcpy(g_pub_tx.reply, "S047");
		goto ErrExit;
    }
    
    
    
    
    
    fprintf(fp, "~@类型|@起息日期|@交易日期|@交易金额|@计息金额|@已结息金额|@已结利息|@计息天数|@计息利率|@利率类型|\n");
    
    vtcp_log("[%s][%d]开始逐条查询帐号信息",__FILE__,__LINE__);
    
   
    sprintf(sWherelist, "ac_id=%ld and ac_seqn=%d and ", sMdm_ac_rel.ac_id, iAc_seqn);

    if(lBegin_date > 0)
    {
    	memset(sTmp, 0x00, sizeof(sTmp));
    	sprintf(sTmp, "rate_date>=%ld and ", lBegin_date);
    	strcat(sWherelist, sTmp);
    }
    if(lEnd_date > 0)
    {
    	memset(sTmp, 0x00, sizeof(sTmp));
        sprintf(sTmp, "rate_date<=%ld and ", lEnd_date);
    	strcat(sWherelist, sTmp);
    }
    strcat(sWherelist, " 1=1 order by tx_date");
    
    /*写明细*/
    fprintf(fp,"存入明细|||||||\n");
    iRet =Mo_sxc_hst_Dec_Sel(g_pub_tx.reply, sWherelist);
    if(iRet)
    {
    	vtcp_log("[%s][%d]获取遂心存明细时定义游标出错!!", __FILE__, __LINE__);
    	goto ErrExit;	
    }
    else if(iRet == 100)
    {
    	vtcp_log("[%s][%d]该卡号未签约遂心存，请检查！ac_no=[%s]", __FILE__, __LINE__, cAc_no);	
    	goto ErrExit;
    }
    while(1)
    {
    	memset(&sMo_sxc_hst, 0x00, sizeof(sMo_sxc_hst));
    	iRet =Mo_sxc_hst_Fet_Sel(&sMo_sxc_hst, g_pub_tx.reply);
    	if(iRet == 100)
    	{
	    vtcp_log("遂心存明细登记薄读取完成！！");
    	    break;	
    	}
    	else if(iRet)
    	{
            vtcp_log("[%s][%d]查询遂心存明细登记薄出错,iRet=[%d]", __FILE__, __LINE__, iRet);
            goto ErrExit;	
    	}

    	vtcp_log("[%s][%d]开始写文件---->,cSts=[%s],cType=[%s],ic_date=[%ld],rate_date=[%ld],tx_amt=[%lf]\n", __FILE__, __LINE__,cSts,cType,sMo_sxc_hst.ic_date,sMo_sxc_hst.rate_date,sMo_sxc_hst.tx_amt);
    	/*写文件列表*/
			
    	memset(cSts, 0x00, sizeof(cSts));
    	if('1' == sMo_sxc_hst.sts[0])
    	{
    	    strcpy(cSts, "计 息 中");	
    	}
    	else if('*' == sMo_sxc_hst.sts[0])
    	{
    	    strcpy(cSts, "结    清");	
    	}
    	
    	fprintf(fp, "%s|%ld|%ld|%-.2f|%-.2f|%-.2f|%-.2f||||\n", cSts, sMo_sxc_hst.ic_date, sMo_sxc_hst.rate_date, sMo_sxc_hst.tx_amt, sMo_sxc_hst.hst_bal,
    	            sMo_sxc_hst.tx_amt-sMo_sxc_hst.hst_bal,sMo_sxc_hst.intst);


    }
    Mo_sxc_hst_Clo_Sel();
    

    
    
    /*写利息明细*/
  	if(0 == atoi(cFlag_1))
  	{  
  		fprintf(fp,"结息明细||||||||\n");
  	  /*获取遂心存利息明细信息*/
  	  iRet =Mo_sxc_hst_intst_Dec_Sel(g_pub_tx.reply, sWherelist);
  	  if(iRet)
  	  {
  	  	vtcp_log("[%s][%d]获取遂心存利息明细时定义游标出错!!", __FILE__, __LINE__);
  	  	goto ErrExit;	
  	  }
  	  else if(iRet == 100)
  	  {
  	  	vtcp_log("[%s][%d]该卡号尚未有结息，请检查！ac_no=[%s]", __FILE__, __LINE__, cAc_no);	
  	  	goto ErrExit;
  	  }
  	  while(1)
  	  {
  	  	memset(&sMo_sxc_hst_intst, 0x00, sizeof(sMo_sxc_hst_intst));
  	  	iRet =Mo_sxc_hst_intst_Fet_Sel(&sMo_sxc_hst_intst, g_pub_tx.reply);
  	  	if(iRet == 100)
  	  	{
		    vtcp_log("遂心存利息明细登记薄读取完成！！");
  	  	    break;	
  	  	}
  	  	else if(iRet)
  	  	{
  	          vtcp_log("[%s][%d]查询遂心存利息明细登记薄出错,iRet=[%d]", __FILE__, __LINE__, iRet);
  	          goto ErrExit;	
  	  	}
  	  	/*
  	  	memset(cType, 0x00, sizeof(cType));
  	  	if('1' == sMo_sxc_hst.type[0])
  	  	{
  	  	    strcpy(cType, "登    记");	
  	  	}
  	  	
  	  	memset(cSts, 0x00, sizeof(cSts));
  	  	if('1' == sMo_sxc_hst.sts[0])
  	  	{
  	  	    strcpy(cSts, "计息");	
  	  	}
  	  	else if('*' == sMo_sxc_hst.sts[0])
  	  	{
  	  	    strcpy(cSts, "结清");	
  	  	}
  	  	
  	  	
  	  	intst += sMo_sxc_hst.intst;
  	  	
  	  	vtcp_log("[%s][%d]开始写文件---->,cSts=[%s],cType=[%s],ic_date=[%ld],rate_date=[%ld],tx_amt=[%lf]\n", __FILE__, __LINE__,cSts,cType,sMo_sxc_hst.ic_date,sMo_sxc_hst.rate_date,sMo_sxc_hst.tx_amt);
  	  	*/
  	  	
  	  	
  	  	/*写文件列表
  	  	if('1' == cFlag_1[0])
  	  	{
  	  	    fprintf(fp, "%s|%ld|%ld|%-.2f|%-.2f|||\n",  cType, sMo_sxc_hst.ic_date, sMo_sxc_hst.rate_date, sMo_sxc_hst.tx_amt, 
  	  	            sMo_sxc_hst.hst_bal, sMo_sxc_hst.intst);
  	  	}
  	  	else
  	    {
  	        fprintf(fp, "%s||%ld|%-.2f|%-.2f|||\n",  cType, sMo_sxc_hst.rate_date, sMo_sxc_hst.tx_amt, 
  	  	            sMo_sxc_hst.hst_bal, sMo_sxc_hst.intst);	
  	    }*/

  	  	 vtcp_log("cFlag=[%s]",cFlag[0]);
  	  	 memset(sWherelist1, 0x00, sizeof(sWherelist1));
  	  	 if(cFlag[0] != '0')
  	  	 {
  	       memset(sTmp, 0x00, sizeof(sTmp));
  	       sprintf(sTmp, " sts='%s' and ",cFlag);
  	  		 strcat(sWherelist1, sTmp);	
  	  	 }
  	      memset(sTmp, 0x00, sizeof(sTmp));
  	      sprintf(sTmp, "ac_id=%ld and ac_seqn=%d and rate_date=%ld order by tx_date ",sMo_sxc_hst_intst.ac_id, sMo_sxc_hst_intst.ac_seqn, sMo_sxc_hst_intst.rate_date);
  	  		strcat(sWherelist1, sTmp);	
  	 
  	  		/*按登记状态获取遂心存明细*/
  	  		iRet = sql_count("mo_sxc_hst", sWherelist1);
  	  		if(iRet > 0)
  	  		{
  	  		
  	  			/*if('2' == sMo_sxc_hst_intst.sts[0])
  	  			{
  	  			    strcpy(cSts, "未入账");
  	  			}*/
  	
  	  			memset(cType, 0x00, sizeof(cType));
  	  		  if('2' == sMo_sxc_hst_intst.type[0])
  	  	    {
  	  	            strcpy(cType, "取    款");	
  	  	    }
  	  	    /*
  	  	    if('3' == sMo_sxc_hst_intst.type[0])
  	  	    {
  	  	            strcpy(cType, "周期结息");	
  	  	    }*/
  	  	    if('4' == sMo_sxc_hst_intst.type[0])
  	  	    {
  	  	            strcpy(cType, "解约取款");	
  	  	    }
  	  			/*查询利率类型*/
  	  			memset(&sCom_rate, 0x00, sizeof(sCom_rate));
  	  			iRet =Com_rate_Sel(g_pub_tx.reply, &sCom_rate, "rate_code='%s'", sMo_sxc_hst_intst.rate_code);
  	  			if(iRet)
  	  			{
  	   	  		strcpy(g_pub_tx.reply, "M003");
  	   	  		sprintf( acErrMsg, "Com_rate_Sel出错!!!" );
  	   	  		WRITEMSG
  	   	  		goto ErrExit;
  	  			}
  	  			
  	  			fprintf(fp, "%s|%ld|%ld|%-.2f||%-.2f|%-.2f|%ld|%-.3f%%|%s|\n", \
  	  			         cType, sMo_sxc_hst_intst.ic_date, sMo_sxc_hst_intst.tx_date, sMo_sxc_hst_intst.tx_amt,sMo_sxc_hst_intst.tx_amt, \
  	  			         sMo_sxc_hst_intst.intst, sMo_sxc_hst_intst.days, sMo_sxc_hst_intst.rate,sCom_rate.name);	
  	  					
  	  		}else if (iRet < 0)
          {
                    sprintf(acErrMsg, "查询sMo_sxc_hst错误");
                    WRITEMSG
                    strcpy(g_pub_tx.reply, "KH04");
                    goto ErrExit;
          }
  	  }
  	  Mo_sxc_hst_intst_Clo_Sel();
  	}  
    
    
    /**查找遂心存登记簿*
    fprintf(fp,"|\n");
     fprintf(fp, "|||签约账号:|%s|||||\n", cAc_no);
     
    iRet = Mo_sxc_Dec_Sel(g_pub_tx.reply, "ac_id=%ld and ac_seqn=%d order by tx_date", sMdm_ac_rel.ac_id, iAc_seqn);
    if(iRet)
    {
        strcpy(g_pub_tx.reply, "D101");
        sprintf( acErrMsg, "Mo_sxc_Sel出错!!!" );
        WRITEMSG
        goto ErrExit;
    }
    while(1)
    {
    	memset(&sMo_sxc, 0x00, sizeof(sMo_sxc));
    	
        iRet = Mo_sxc_Fet_Sel(&sMo_sxc, g_pub_tx.reply);
        if(iRet == 100)
        {
           break;	
        }
        else if(iRet)
        {
            strcpy(g_pub_tx.reply, "D102");
            sprintf( acErrMsg, "Mo_sxc_Sel出错!!!" );
            WRITEMSG
            goto ErrExit;
        }
        
        if(pub_base_CompDblVal(sMo_sxc.un_intst, 0.00) > 0 )
        {
            fprintf(fp, "||;|签约日期:%ld|解约日期:%ld|遂心存余额:%.2f|已结利息:%.2f|未入账利息:%.2f||\n", \
                                sMo_sxc.tx_date, sMo_sxc.untx_date, sMo_sxc.bal, sMo_sxc.intst, sMo_sxc.un_intst);
        }
        else
        {
            fprintf(fp, "|||签约日期:%ld|解约日期:%ld|遂心存余额:%.2f|已结利息:%.2f|||\n", \
                                  sMo_sxc.tx_date,sMo_sxc.untx_date, sMo_sxc.bal, sMo_sxc.intst);
        }
    }
    Mo_sxc_Clo_Sel();
   */
    
    fclose(fp);

OkExit:
    strcpy(g_pub_tx.reply, "0000");
    sprintf(acErrMsg, "success!!");
    WRITEMSG
    set_zd_data(DC_FILE_SND, "1");
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 0;
    
ErrExit:
	if(NULL != fp)
	{
		fclose(fp);	
	}	
    sprintf(acErrMsg, "failed !! reply is[%s]", g_pub_tx.reply);
    WRITEMSG
    if(strlen(g_pub_tx.reply) == 0 || strcmp(g_pub_tx.reply, "0000") == 0)
    {
         strcpy(g_pub_tx.reply, "S035");
    }
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 1;

}
