/***************************************************************
* 文 件 名:  sp8135.c
* 功能描述:  遂心存签约/解约登记簿查询	
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
#include "mo_sxc_hst_c.h"
#include "com_parm_c.h"
#include "mob_acct_type_c.h"

#define NUM 4000
long lDaynumLAT(long date, int num);

int sp8315()
{	
    int iRet=0;
    int i = 0;
    int j = 0;
    int iCount=0;
    int iDays = 0;
    int iDay = 0;
    long lBeg_date = 0;
    long lEnd_date = 0;
    long lTx_date = 0;
    long lTmp = 0;
    long lDd_mst_ic_date   = 0;
    long lF_Dd_mst_ic_date = 0;
    
    double dTx_amt = 0.00;
    double dSy_tx_amt = 0.00;
    double dIntst = 0.00;
    double dIntst_hst = 0.00;
    double dRate_val = 0.00;
    double dBeg_amt = 0.00;
    double dEnd_amt = 0.00;
    
    double dLc_amt = 0.00;
    double dSx_amt = 0.00;   /**com_parm表里配置的生效触发阀值**/
    double dBs_amt = 0.00;   /**com_parm表里配置的登记/释放单位倍数**/
    double dGc_amt       = 0.00;
    double dGc_up_amt    = 0.00;
    double dGc_down_amt  = 0.00;  /**每次存入金额 向下取整**/
    double dDd_mst_bal   = 0.00;
    double dF_Dd_mst_bal = 0.00;
    double dIntst_hq     = 0.00;
    double dF_Intst_hq   = 0.00;
    
    /* add  20151128  begin */
		int iIntst_days = 0;
    long lQx_Date = 0;
    long   lRate_date = 0;
    double dIntst_hst_all=0.00;
    /* add  20151128  end  */
    
    char cAc_no[25];
    char cBr_no[6];
    char cSts[2];
    char cFlag[2];
    char sFilename[1024];
    char sWherelist[1024];
    char sTmp[1024];
    char cRate_code[4];
    char cLllx[15];

    FILE * fp = NULL;
    
    struct mo_sxc_c     sMo_sxc;
    struct mo_sxc_hst_c sMo_sxc_hst;
    struct mdm_ac_rel_c sMdm_ac_rel;
    struct dd_mst_c     sDd_mst;
    struct com_parm_c   sCom_parm;
    struct mob_acct_type_c sMob_acct_type;
    struct sxc_hst
    {
         double tx_amt;
         double intst;
         long   rate_date;
         long   ic_date;
    } sSxc_hst[NUM];
    
    memset(cSts,   0x00, sizeof(cSts));
    memset(cFlag,  0x00, sizeof(cFlag));
    memset(cAc_no, 0x00, sizeof(cAc_no));
    memset(sTmp,   0x00, sizeof(sTmp));
    memset(sFilename,    0x00, sizeof(sFilename));
    memset(sWherelist,   0x00, sizeof(sWherelist));
    memset(cRate_code,   0x00, sizeof(cRate_code));
    memset(cLllx,        0x00, sizeof(cLllx));
    
    memset(&sMo_sxc,     0x00, sizeof(sMo_sxc));
    memset(&sMo_sxc_hst, 0x00, sizeof(sMo_sxc_hst));
    memset(&sMdm_ac_rel, 0x00, sizeof(sMdm_ac_rel));
    memset(&sDd_mst,     0x00, sizeof(sDd_mst));
    memset(&sCom_parm,   0x00, sizeof(sCom_parm));
    memset(&sMob_acct_type, 0x00, sizeof(sMob_acct_type));
    memset(&sSxc_hst,    0x00, sizeof(sSxc_hst));
    pub_base_sysinit();
    
    get_zd_data("0300", cAc_no);
    get_zd_data("0680", cFlag);
    pub_base_strpack(cAc_no);
    pub_base_old_acno(cAc_no);
    
    /***查询账户信息***/
    vtcp_log("账号长度：[%d]", strlen(cAc_no));
    if(0 != strlen(cAc_no))
    {
        iRet = Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdm_ac_rel, "ac_no='%s'", cAc_no);
        if(iRet)
        {
            strcpy(g_pub_tx.reply, "M003");
            sprintf( acErrMsg, "Mdm_ac_rel_Sel出错!!!" );
            WRITEMSG
            goto ErrExit;
        }
    }    
    if(cFlag[0] == '1') /****签约查询****/
    {
        get_zd_long("0440", &lBeg_date);
        get_zd_long("0450", &lEnd_date);
        get_zd_data("0670", cSts);
        get_zd_double("0410", &dBeg_amt);
        get_zd_double("0420", &dEnd_amt);
        get_zd_data("0910", cBr_no);
        pub_base_strpack(cBr_no);
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
          
        fprintf(fp, "~@状态|@账号|@户名|@计息中金额|@已结利息总额|@签约日期|@签约机构|@解约日期|@解约机构|@开户机构|\n");
        vtcp_log("[%s][%d]开始逐条查询遂心存登记薄信息",__FILE__,__LINE__);
        if( lBeg_date != 0 )
        {
            memset(sTmp, 0x00, sizeof(sTmp));
            if('*' == cSts[0])
            {
                sprintf(sTmp, "untx_date>=%ld and ", lBeg_date);	
            }
            else
        	{
                sprintf(sTmp, "tx_date>=%ld and ", lBeg_date);
            }
            strcat(sWherelist, sTmp);
        }
        if( lEnd_date != 0 )
        {
        	memset(sTmp, 0x00, sizeof(sTmp));
        	if('*' == cSts[0])
        	{
        	    sprintf(sTmp, "untx_date<=%ld and ", lEnd_date);
        	}
        	else
        	{
        	    sprintf(sTmp, "tx_date<=%ld and ", lEnd_date);
        	}
        	strcat(sWherelist, sTmp);	
        }
        
        if(pub_base_CompDblVal(dBeg_amt, 0.00))
        {
        	sprintf(sTmp, "bal>=%.2f and ", dBeg_amt);
        	strcat(sWherelist, sTmp);	
        }
        if(pub_base_CompDblVal(dEnd_amt, 0.00))
        {
        	sprintf(sTmp, "bal<=%.2f and ", dEnd_amt);
        	strcat(sWherelist, sTmp);	
        }
        
        if( strlen(cBr_no) > 0 )
        {
            memset(sTmp, 0x00, sizeof(sTmp));
            sprintf(sTmp, "opn_br_no='%s' and ", cBr_no);
            strcat(sWherelist, sTmp);
        }
        pub_base_strpack(sWherelist);
         /*查询当前已经签约的所有账户*/
        iRet =Mo_sxc_Dec_Sel(g_pub_tx.reply, "%s sts='%s' order by tx_date, trace_no", sWherelist, cSts);
        if(iRet)
        {
            vtcp_log("[%s][%d]查询游标定义出错", __FILE__, __LINE__);
            goto ErrExit;	
        }
    
        iCount=0;
        while(1)
        {
            memset(&sMo_sxc, 0x00, sizeof(sMo_sxc));
            memset(&sMdm_ac_rel, 0x00, sizeof(sMdm_ac_rel));
            
            iRet =Mo_sxc_Fet_Sel(&sMo_sxc, g_pub_tx.reply);
            if(100 == iRet)
            {
                vtcp_log("获取遂心存已签约账户完成");
                break;	
            }	
            else if(iRet)
            {
                vtcp_log("[%s][%d]获取签约账户出错！！", __FILE__, __LINE__);
                Mo_sxc_Clo_Sel();
                goto ErrExit;
            }
            
            vtcp_log("[%s][%d] 看看结构体中的值是否正确---ac_id=[%ld]\n", __FILE__, __LINE__,sMo_sxc.ac_id);
            
            if('1' == cSts[0])  /**目前仅有卡能签约，显示卡号，都找不到显示ac_id以便查找原因**/
            {
                iRet= Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdm_ac_rel, "ac_id=%ld and note_sts in('0','1') and mdm_code<>'1010'", sMo_sxc.ac_id);
                if(iRet)
                {
                   	iRet= Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdm_ac_rel, "ac_id=%ld and note_sts='*' and mdm_code<>'1010'", sMo_sxc.ac_id);
                    if(iRet)
                    {     
                   	    sprintf(sMdm_ac_rel.ac_no, "%ld", sMo_sxc.ac_id);
                   	}
                }
                memset(sTmp, 0x00, sizeof(sTmp));
           	    strcpy(sTmp, "签约");	
            }
            else
            {
                iRet= Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdm_ac_rel, "ac_id=%ld and note_sts in('0','1') and mdm_code<>'1010'", sMo_sxc.ac_id);
                if(iRet)
                {
                   	iRet= Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdm_ac_rel, "ac_id=%ld and note_sts='*' and mdm_code<>'1010'", sMo_sxc.ac_id);
                    if(iRet)
                    {
                   	    sprintf(sMdm_ac_rel.ac_no, "%ld", sMo_sxc.ac_id);
                   	}
                }
                memset(sTmp, 0x00, sizeof(sTmp));
           	    strcpy(sTmp, "解约");	
            }
            iRet = Dd_mst_Sel(g_pub_tx.reply, &sDd_mst, "ac_id=%ld", sMo_sxc.ac_id);
            if(iRet)
            {
                vtcp_log("[%s][%d]Dd_mst_Sel获取签约账户出错！！", __FILE__, __LINE__);
                Mo_sxc_Clo_Sel();
                goto ErrExit;
            }
            /*写文件列表*/
            fprintf(fp, "%s|%s|%s|%.2f|%.2f|%ld|%s|%ld|%s|%s|\n", sTmp, sMdm_ac_rel.ac_no, sDd_mst.name, sMo_sxc.bal,
                	sMo_sxc.intst, sMo_sxc.tx_date, sMo_sxc.tx_br_no, sMo_sxc.untx_date, sMo_sxc.untx_br_no, sMo_sxc.opn_br_no);
            
             iCount++;
        }
        fprintf(fp, "户数统计:%d||\n", iCount);
        Mo_sxc_Clo_Sel();
        
        fclose(fp);
        set_zd_data(DC_FILE_SND, "1");
    }
    else if(cFlag[0] == '2')  /****短交易查询返回前台用***/
    {
    	/***查找遂心存登记簿**/
        iRet = Mo_sxc_Sel(g_pub_tx.reply, &sMo_sxc, "ac_id=%ld and ac_seqn=1 order by tx_date desc,sts desc", sMdm_ac_rel.ac_id);
        vtcp_log("%d,sMo_sxc.sts=[%s]", iRet, sMo_sxc.sts);
        if(iRet != 100 && iRet != 0)
        {
            strcpy(g_pub_tx.reply, "D101");
            sprintf( acErrMsg, "Mo_sxc_Dec_Upd出错!!!" );
            WRITEMSG
            goto ErrExit;
        }
        else if(iRet == 100)
        {
            set_zd_data("0690", "0"); /**未签约**/
        }
        else
        {
            if(sMo_sxc.sts[0] == '1')
            {
                set_zd_double("0400", sMo_sxc.bal);
                set_zd_data("0690", "1");
            }
            else               /**签约过但是解约了，目前未签约**/
            {
                set_zd_data("0690", "0");
                set_zd_data("0700", sMo_sxc.sts); /**明细查询用**/
            }	
        }
        /*查找com_parm表*/
        iRet = Com_parm_Sel(g_pub_tx.reply, &sCom_parm, "parm_code='SXC' and parm_seqn=6");
        if(iRet)
        {
            strcpy(g_pub_tx.reply, "M003");
            sprintf( acErrMsg, "Com_pram_Sel出错!!!" );
            WRITEMSG
            goto ErrExit;	
        }
        set_zd_data("0810",sCom_parm.val);
        /**查找短信银行签约**/
        iRet = Mob_acct_type_Sel(g_pub_tx.reply, &sMob_acct_type, "ac_id=%ld and mob_sts='1'", sMdm_ac_rel.ac_id);
        vtcp_log("短信银行签约状态:mob_sts=[%s]", sMob_acct_type.mob_sts);
        if(iRet != 100 && iRet != 0)
        {
            strcpy(g_pub_tx.reply, "D101");
            sprintf( acErrMsg, "Mob_acct_type_Dec_Upd出错!!!" );
            WRITEMSG
            goto ErrExit;	
        }
        else if(iRet == 100)
        {
            set_zd_data("0710", "0");	
        }
        else
        {
            set_zd_data("0710", "1");	
        }
    }
    else if(cFlag[0] == '3') /**解约试算利息分支**/
    {
        get_zd_long("0460", &lTx_date);
        
        /**取参数配置**/
        iRet = pub_base_GetParm_double("SXC", 1, &dLc_amt);
        if(iRet)
        {
            sprintf( acErrMsg, "pub_base_GetParm_double1取参数配置表错误!!!" );
            WRITEMSG
            goto ErrExit;
        }
        iRet = pub_base_GetParm_double("SXC", 2, &dSx_amt);
        if(iRet)
        {
            sprintf( acErrMsg, "pub_base_GetParm_double2取参数配置表错误!!!" );
            WRITEMSG
            goto ErrExit;
        }
        iRet = pub_base_GetParm_double("SXC", 3, &dBs_amt);
        if(iRet)
        {
            sprintf( acErrMsg, "pub_base_GetParm_double3取参数配置表错误!!!" );
            WRITEMSG
            goto ErrExit;
        }
        iRet = pub_base_GetParm_int("SXC", 5, &iDay);
        if(iRet)
        {
            sprintf( acErrMsg, "pub_base_GetParm_double5取周期结息相隔天数参数配置表错误!!!" );
            WRITEMSG
            goto ErrExit;
        }
        
        /***查询活期账户**/
        iRet = Dd_mst_Sel(g_pub_tx.reply, &sDd_mst, "ac_id=%ld", sMdm_ac_rel.ac_id);
        if(iRet)
        {
            vtcp_log("[%s][%d]Dd_mst_Sel获取签约账户出错！！", __FILE__, __LINE__);
            goto ErrExit;
        }
        /**查询随心存登记簿**/
        iRet = Mo_sxc_Sel(g_pub_tx.reply, &sMo_sxc, "ac_id=%ld and sts='1'", sMdm_ac_rel.ac_id);
        if(iRet)
        {
            vtcp_log("[%s][%d]Dd_mst_Sel获取签约账户出错！！", __FILE__, __LINE__);
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
        fprintf(fp, "~登记日|起息日|结息日|计息天数|计息金额|利率类型|利率|试结利息|\n");
        
        
        iRet = Mo_sxc_hst_Dec_Sel(g_pub_tx.reply, "ac_id=%ld and ac_seqn=1 and sts='1'order by rate_date", sMdm_ac_rel.ac_id);
        if(iRet)
        {
            strcpy(g_pub_tx.reply, "D101");
            sprintf( acErrMsg, "Mo_sxc_Dec_Upd出错!!!" );
            WRITEMSG
            goto ErrExit;
        }
        while(1)
        {
            memset(&sMo_sxc_hst, 0x00, sizeof(sMo_sxc_hst));
            iRet = Mo_sxc_hst_Fet_Sel(&sMo_sxc_hst, g_pub_tx.reply);
            if(iRet == 100)
            {
                break;
            }
            else if(iRet)
            {
                strcpy(g_pub_tx.reply, "D102");
                sprintf( acErrMsg, "Mo_sxc_Dec_Upd出错!!!" );
                WRITEMSG
                goto ErrExit;
            }
            sSxc_hst[i].tx_amt = sMo_sxc_hst.hst_bal;
            sSxc_hst[i].intst  = 0;
            sSxc_hst[i].rate_date = sMo_sxc_hst.tx_date;
            sSxc_hst[i].ic_date = sMo_sxc_hst.ic_date;
             
            i++;
            
            if(i > NUM)
            {
                break;
            }
        }
        Mo_sxc_hst_Clo_Sel();
        
        lEnd_date = g_pub_tx.tx_date;
        vtcp_log("原有登记了[%d]条明细 lEnd_date[%ld", i, lEnd_date);
        
        while (lEnd_date < lTx_date)
        {
           
            if(iCount == 0)  /***模拟第一天日终跑批***/
            {
            	vtcp_log("-----首先模拟日终先登记第一条明细-----");        
            	
            	dGc_amt = sDd_mst.bal - sMo_sxc.bal - dLc_amt; /**计算扎差金额=活期余额-遂心存余额-留存金额**/
                iRet = pub_base_CompDblVal(dGc_amt, 0.00);
                if(iRet > 0) /***轧差金额为正、登记明细***/
                {
            	    dGc_down_amt = dBs_amt * (int)((sDd_mst.bal-sMo_sxc.bal-dLc_amt)/dBs_amt); /**计算登记金额=dBs_amt的整数倍（向下取整)**/
                    if(pub_base_CompDblVal(dGc_down_amt,0.00) > 0 )
                    {
            	        
                        sSxc_hst[i].tx_amt = dGc_down_amt;
                        sSxc_hst[i].intst  = 0;
                        sSxc_hst[i].rate_date = g_pub_tx.tx_date;
                        sSxc_hst[i].ic_date = g_pub_tx.tx_date;
                        
                        i++;
                        
                        if(i > NUM)
                        {
                            break;
                        }
                    }
                    else
                    {
                        dGc_down_amt = 0.00;
                    }
                    
                    dDd_mst_bal = sDd_mst.bal - (sMo_sxc.bal+dGc_down_amt) - dLc_amt;  /**记录除去遂心存余额之后的活期余额**/
                    lDd_mst_ic_date = lEnd_date;     
                }
                else if(iRet < 0) /***轧差金额为负、释放***/
                {
                    dGc_up_amt = dBs_amt * ( -1*(int)(dGc_amt/dBs_amt) + 1); /***计算释放金额=dBs_amt的整数倍（向上取整）***/
                    if(pub_base_CompDblVal(dGc_amt-(int)(dGc_amt/dBs_amt)*dBs_amt, 0.00) == 0)
                    {
                        dGc_up_amt -= dBs_amt;   /**如果为整数倍，上面计算时多加了1，在此减去**/
                    }
            
                    vtcp_log("dGc_up_amt需要释放金额为[%lf]", dGc_up_amt);
                    if(pub_base_CompDblVal(dGc_up_amt,0.00) > 0 )
                    {
                        /***-----模拟释放结息------****/
                        dSy_tx_amt = dGc_up_amt;
                        
                        for( j = i-1; j >= 0; j--)
                        {
                            vtcp_log("j=[%d]", j);
                            /***确定每条明细中需释放的金额***/
                            if(pub_base_CompDblVal(dSy_tx_amt, sSxc_hst[j].tx_amt) <0 )
                            {
                                dTx_amt = dSy_tx_amt;
                            }
                            else
                            {
                                dTx_amt = sSxc_hst[j].tx_amt;
                            }
                            
                            /***每次循环初始化***/
                            dIntst_hst = 0.00;
                            
                            /***计算取利率的天数  取靠档利率编号***/
                            memset(cLllx, 0x00, sizeof(cLllx));
                            iDays = pub_base_CalTrueDays(sSxc_hst[j].rate_date, lEnd_date);
                            if(iDays > 0 && iDays < 7)
                            {
                                strcpy(cRate_code, HQLLBH);
                                sMo_sxc_hst.rate_date = g_pub_tx.tx_date;
                                strcpy(cLllx, "活期");
                            }
                            else if(iDays >= 7 && iDays < 90)
                            {
                                strcpy(cRate_code, QTTZCK);
                                sMo_sxc_hst.rate_date = g_pub_tx.tx_date;
                                strcpy(cLllx, "七天通知");
                            }
                            else if(iDays >= 90 && iDays < 180)
                            {
                                 strcpy(cRate_code, ZCZQ03);
                                 strcpy(cLllx, "整存整取三个月");
                            }
                            else if(iDays >= 180 && iDays < 360)
                            {
                                strcpy(cRate_code, ZCZQ06);
                                strcpy(cLllx, "整存整取六个月");
                            }
                            else if(iDays >=360 && iDays < 720)
                            {
                                strcpy(cRate_code, ZCZQ12);
                                strcpy(cLllx, "整存整取一年");
                            }
                            else if(iDays >=720 && iDays < 1080)
                            {
                                strcpy(cRate_code, ZCZQ24);
                                strcpy(cLllx, "整存整取二年");
                            }
                            else if(iDays >=1080 && iDays < 1800)
                            {
                                strcpy(cRate_code, ZCZQ36);
                                strcpy(cLllx, "整存整取三年");
                            }
                            else if(iDays >= 1800)
                            {
                                strcpy(cRate_code, "205");
                                strcpy(cLllx, "整存整取五年");
                            }
                            else if(iDays <= 0)
                            {
                                iDays = 0;
                                strcpy(cRate_code, HQLLBH);
                                strcpy(cLllx, "活期");
                            }
                            iRet = pub_base_getllz(cRate_code, "01", sSxc_hst[j].ic_date, &dRate_val);
                            if (iRet != 0)
                            {
                                sprintf(acErrMsg,"从com_rate取利率值错误 [%d]",iRet);
                                WRITEMSG
                                strcpy(g_pub_tx.reply, "W110");
                            }
                            
                            iDays = pub_base_CalTrueDays(sSxc_hst[j].ic_date, lEnd_date);    /**计算计息天数**/
                            
                            dIntst_hst = pub_base_PubDround_1(dTx_amt * iDays * dRate_val / L360 / L100); /**计算利息**/
                            
                            fprintf(fp, "%ld|%ld|%ld|%d|%.2lf|%s|%.3lf%%|%.2lf|\n", \
                                sSxc_hst[j].rate_date, sSxc_hst[j].ic_date, lEnd_date, iDays, dTx_amt, cLllx, dRate_val, dIntst_hst);
                            
                            
                            sSxc_hst[j].intst  += dIntst_hst;
                            sSxc_hst[j].tx_amt -= dTx_amt;
                            
                            /***累加每条明细的结出利息**/
                            dIntst += dIntst_hst;
                            
                            /**检查是否可以退出**/
                            dSy_tx_amt -= dTx_amt;
                            if(dSy_tx_amt == 0.00)
                            {
                                break;
                            }
                        }  	
                    }
                    else
                    {
                    
                        dGc_up_amt = 0.00;	
                    }
                    dDd_mst_bal = sDd_mst.bal - (sMo_sxc.bal-dGc_up_amt) - dLc_amt + dIntst;  /**记录除去遂心存余额之后的活期余额**/
                    lDd_mst_ic_date = lEnd_date;
                    
                }
                else
                {
                   dDd_mst_bal = sDd_mst.bal - sMo_sxc.bal - dLc_amt;  /**记录除去遂心存余额之后的活期余额**/
                   lDd_mst_ic_date = lEnd_date; 	
                }
            }
            /**换日  循环模拟下一天的所有日终跑批**/
            lEnd_date = lDaynumLAT(lEnd_date, 1);
            
            /***--------开始模拟活期主户结息-----------**/
            if( lEnd_date > g_pub_tx.tx_date && (lEnd_date%100)== 21 
            	 && ( (lEnd_date/100%100)==3 || (lEnd_date/100%100)==6 || (lEnd_date/100%100)==9 || (lEnd_date/100%100)==12 )
              )
            {
                /***签约的活期利息试算**/
                dIntst_hst = 0.00;
                dTx_amt = 0.00;
                
                /***计算取利率的天数  取靠档利率编号***/
                memset(cLllx, 0x00, sizeof(cLllx));
                iDays = pub_base_CalTrueDays(lDd_mst_ic_date, lEnd_date);
                
                strcpy(cRate_code, HQLLBH);
                
                iRet = pub_base_getllz(cRate_code, "01", lEnd_date, &dRate_val);
                if (iRet != 0)
                {
                    sprintf(acErrMsg,"从com_rate取利率值错误 [%d]",iRet);
                    WRITEMSG
                    strcpy(g_pub_tx.reply, "W110");
                }
                
                iDays = pub_base_CalTrueDays(lDd_mst_ic_date, lEnd_date);    /**计算计息天数**/
                
                dIntst_hst = pub_base_PubDround_1(dDd_mst_bal * iDays * dRate_val / L360 / L100); /**计算利息**/
                
                
                /***累加结出利息**/
                dIntst_hq += dIntst_hst;
                dDd_mst_bal += dIntst_hst;
                
                lDd_mst_ic_date = lEnd_date;
                
            }
            
            /***---------------模拟周期结息----------------**/
            for(j = 0; j < i; j++)
            {
               if( pub_base_CompDblVal(sSxc_hst[j].tx_amt,0.00) > 0 )
               {
                   iDays = pub_base_CalTrueDays(sSxc_hst[j].ic_date, lEnd_date);
                   vtcp_log("=========ic_date[%ld], lEnd_date[%ld], iDays[%d]===========", sSxc_hst[j].ic_date, lEnd_date, iDays);
                   if(iDays == iDay)
                   {
                       vtcp_log("=========sSxc_hst[%d].ic_date[%ld]=======", j, sSxc_hst[j].ic_date);
                       /***每次循环初始化***/
                       dIntst_hst = 0.00;
                       dTx_amt = 0.00;
                       
                       /***计算取利率的天数  取靠档利率编号***/
                       memset(cLllx, 0x00, sizeof(cLllx));
                       iDays = pub_base_CalTrueDays(sSxc_hst[j].rate_date, lEnd_date);
                       if(iDays > 0 && iDays < 7)
                       {
                           strcpy(cRate_code, HQLLBH);
                           sMo_sxc_hst.rate_date = g_pub_tx.tx_date;
                           strcpy(cLllx, "活期");
                       }
                       else if(iDays >= 7 && iDays < 90)
                       {
                           strcpy(cRate_code, QTTZCK);
                           sMo_sxc_hst.rate_date = g_pub_tx.tx_date;
                           strcpy(cLllx, "七天通知");
                       }
                       else if(iDays >= 90 && iDays < 180)
                       {
                            strcpy(cRate_code, ZCZQ03);
                            strcpy(cLllx, "整存整取三个月");
                       }
                       else if(iDays >= 180 && iDays < 360)
                       {
                           strcpy(cRate_code, ZCZQ06);
                           strcpy(cLllx, "整存整取六个月");
                       }
                       else if(iDays >=360 && iDays < 720)
                       {
                           strcpy(cRate_code, ZCZQ12);
                           strcpy(cLllx, "整存整取一年");
                       }
                       else if(iDays >=720 && iDays < 1080)
                       {
                           strcpy(cRate_code, ZCZQ24);
                           strcpy(cLllx, "整存整取二年");
                       }
                       else if(iDays >=1080 && iDays < 1800)
                       {
                           strcpy(cRate_code, ZCZQ36);
                           strcpy(cLllx, "整存整取三年");
                       }
                       else if(iDays >= 1800)
                       {
                           strcpy(cRate_code, "205");
                           strcpy(cLllx, "整存整取五年");
                       }
                       else if(iDays <= 0)
                       {
                           iDays = 0;
                           strcpy(cRate_code, HQLLBH);
                           strcpy(cLllx, "活期");
                       }
                       iRet = pub_base_getllz(cRate_code, "01", sSxc_hst[j].ic_date, &dRate_val);
                       if (iRet != 0)
                       {
                           sprintf(acErrMsg,"从com_rate取利率值错误 [%d]",iRet);
                           WRITEMSG
                           strcpy(g_pub_tx.reply, "W110");
                       }
                       
                       iDays = pub_base_CalTrueDays(sSxc_hst[j].ic_date, lEnd_date);    /**计算计息天数**/
                       
                       dIntst_hst = pub_base_PubDround_1(sSxc_hst[j].tx_amt * iDays * dRate_val / L360 / L100); /**计算利息**/
                       
                       fprintf(fp, "%ld|%ld|%ld|%d|%.2lf|%s|%.3lf%%|%.2lf|\n", \
                           sSxc_hst[j].rate_date, sSxc_hst[j].ic_date, lEnd_date, iDays, sSxc_hst[j].tx_amt, cLllx, dRate_val, dIntst_hst);
                       
                       /***累加每条明细的结出利息**/
                       dIntst += dIntst_hst;
                       
                       /**修改ic_date, intst**/
                       sSxc_hst[j].intst  += dIntst_hst;
                       sSxc_hst[j].ic_date = lEnd_date;
                       
                       dDd_mst_bal += dIntst_hst;
                       vtcp_log("-------dDd_mst_bal[%lf]----------", dDd_mst_bal);
                   }
               }
                   
            }
            
            /**-------模拟周期结息之后模拟登记明细----------------**/
            if(pub_base_CompDblVal(dDd_mst_bal, dSx_amt) >= 0) /****账户余额达到生效阀值，才登记***/
            {
                dGc_down_amt = dBs_amt * (int)(dDd_mst_bal/dBs_amt); /**计算登记金额=dBs_amt的整数倍（向下取整)**/
                
                if(pub_base_CompDblVal(dGc_down_amt,0.00) > 0 )
                {
                    /**模拟登记明细**/
                    sSxc_hst[i].tx_amt = dGc_down_amt;
                    sSxc_hst[i].intst  = 0;
                    sSxc_hst[i].rate_date = lEnd_date;
                    sSxc_hst[i].ic_date = lEnd_date;
                
                    i++;
                    
                    if(i > NUM)
                    {
                        break;
                    }
                    
                    dDd_mst_bal -= dGc_down_amt;
                }
                vtcp_log("-------dDd_mst_bal[%lf]----------", dDd_mst_bal);
            }
            
            iCount++;
            if(iCount > 15000)
            {
                break;	
            }
            
        }/**end while**/
        
        lEnd_date = lTx_date;
        /***模拟最后解约计息***/
        for(j = 0; j < i; j++)
        {
            if(sSxc_hst[j].ic_date < lEnd_date && pub_base_CompDblVal(sSxc_hst[j].tx_amt, 0.00) > 0)
            {
                /***每次循环初始化***/
                dIntst_hst = 0.00;
                
                /***计算取利率的天数  取靠档利率编号***/
                memset(cLllx, 0x00, sizeof(cLllx));
                iDays = pub_base_CalTrueDays(sSxc_hst[j].rate_date, lEnd_date);
                if(iDays > 0 && iDays < 7)
                {
                    strcpy(cRate_code, HQLLBH);
                    sMo_sxc_hst.rate_date = g_pub_tx.tx_date;
                    strcpy(cLllx, "活期");
                }
                else if(iDays >= 7 && iDays < 90)
                {
                    strcpy(cRate_code, QTTZCK);
                    sMo_sxc_hst.rate_date = g_pub_tx.tx_date;
                    strcpy(cLllx, "七天通知");
                }
                else if(iDays >= 90 && iDays < 180)
                {
                     strcpy(cRate_code, ZCZQ03);
                     strcpy(cLllx, "整存整取三个月");
                }
                else if(iDays >= 180 && iDays < 360)
                {
                    strcpy(cRate_code, ZCZQ06);
                    strcpy(cLllx, "整存整取六个月");
                }
                else if(iDays >=360 && iDays < 720)
                {
                    strcpy(cRate_code, ZCZQ12);
                    strcpy(cLllx, "整存整取一年");
                }
                else if(iDays >=720 && iDays < 1080)
                {
                    strcpy(cRate_code, ZCZQ24);
                    strcpy(cLllx, "整存整取二年");
                }
                else if(iDays >=1080 && iDays < 1800)
                {
                    strcpy(cRate_code, ZCZQ36);
                    strcpy(cLllx, "整存整取三年");
                }
                else if(iDays >= 1800)
                {
                    strcpy(cRate_code, "205");
                    strcpy(cLllx, "整存整取五年");
                }
                else if(iDays <= 0)
                {
                    iDays = 0;
                    strcpy(cRate_code, HQLLBH);
                    strcpy(cLllx, "活期");
                }
                iRet = pub_base_getllz(cRate_code, "01", sSxc_hst[j].ic_date, &dRate_val);
                if (iRet != 0)
                {
                    sprintf(acErrMsg,"从com_rate取利率值错误 [%d]",iRet);
                    WRITEMSG
                    strcpy(g_pub_tx.reply, "W110");
                }
                
                iDays = pub_base_CalTrueDays(sSxc_hst[j].ic_date, lEnd_date);    /**计算计息天数**/
                
                dIntst_hst = pub_base_PubDround_1(sSxc_hst[j].tx_amt * iDays * dRate_val / L360 / L100); /**计算利息**/
                 
                fprintf(fp, "%ld|%ld|%ld|%d|%.2lf|%s|%.3lf%%|%.2lf|\n", \
                    sSxc_hst[j].rate_date, sSxc_hst[j].ic_date, lEnd_date, iDays, sSxc_hst[j].tx_amt, cLllx, dRate_val, dIntst_hst);
                  
                /***累加每条明细的结出利息**/
                dIntst += dIntst_hst;
            }
        }
          
        /**模拟活期账户利息**/
        if(lEnd_date > lDd_mst_ic_date) /***签约的活期利息试算**/
        {
            dIntst_hst = 0.00;
            dTx_amt = 0.00;
            
            /***计算取利率的天数  取靠档利率编号***/
            memset(cLllx, 0x00, sizeof(cLllx));
            iDays = pub_base_CalTrueDays(lDd_mst_ic_date, lEnd_date);
            
            strcpy(cRate_code, HQLLBH);
            
            iRet = pub_base_getllz(cRate_code, "01", lEnd_date, &dRate_val);
            if (iRet != 0)
            {
                sprintf(acErrMsg,"从com_rate取利率值错误 [%d]",iRet);
                WRITEMSG
                strcpy(g_pub_tx.reply, "W110");
            }
            
            iDays = pub_base_CalTrueDays(lDd_mst_ic_date, lEnd_date);    /**计算计息天数**/
            
            dIntst_hst = pub_base_PubDround_1(dDd_mst_bal * iDays * dRate_val / L360 / L100); /**计算利息**/
            
           
            /***累加结出利息**/
            dIntst_hq += dIntst_hst;
            dDd_mst_bal += dIntst_hst;
            
            lDd_mst_ic_date = lEnd_date;
        }
        if(pub_base_CompDblVal(sMo_sxc.un_intst, 0.00) > 0)
        {
            fprintf(fp,"|||||||解约试算利息总额:%.2lf（其中未入账利息:%.2lf）|\n", dIntst+sMo_sxc.un_intst, sMo_sxc.un_intst);
        }
        else
        {
            fprintf(fp,"|||||||解约试算利息总额:%.2lf|\n", dIntst);	
        }	
        
        fclose(fp);
        set_zd_data(DC_FILE_SND, "1");
        
        /***日期重新赋回**/
        set_zd_double("0400", dIntst);
        vtcp_log("%s,%d,dIntst==[%.2f]", __FILE__,__LINE__,dIntst);
    }
    else if(cFlag[0] == '4') /****签约利息计算器****/
    {
        /****完成功能：假如客户签约时存入一笔钱，计算到某日支取时可得到多少利息****
       
        get_zd_double("0410", &dTx_amt);
        get_zd_long("0440", &lTx_date);
        
        iRet = pub_base_GetParm_double("SXC", 1, &dLc_amt);
        if(iRet)
        {
            sprintf( acErrMsg, "pub_base_GetParm_double1取参数配置表错误!!!" );
            WRITEMSG
            goto ErrExit;
        }
        iRet = pub_base_GetParm_double("SXC", 2, &dSx_amt);
        if(iRet)
        {
            sprintf( acErrMsg, "pub_base_GetParm_double2取参数配置表错误!!!" );
            WRITEMSG
            goto ErrExit;
        }
        iRet = pub_base_GetParm_double("SXC", 3, &dBs_amt);
        if(iRet)
        {
            sprintf( acErrMsg, "pub_base_GetParm_double3取参数配置表错误!!!" );
            WRITEMSG
            goto ErrExit;
        }
        iRet = pub_base_GetParm_int("SXC", 5, &iDay);
        if(iRet)
        {
            sprintf( acErrMsg, "pub_base_GetParm_double5取周期结息相隔天数参数配置表错误!!!" );
            WRITEMSG
            goto ErrExit;
        }
        * 生成下传全路经文件名(批量) *
        pub_base_AllDwnFilName(sFilename);
        fp = fopen(sFilename, "w");
        if (fp == NULL) 
        {
            sprintf(acErrMsg, "open file error [%s]", sFilename);
            WRITEMSG
            strcpy(g_pub_tx.reply, "S047");
            goto ErrExit;
        }
        **写表头**
        fprintf(fp, "~登记日|起息日|结息日|计息天数|计息金额|利率类型|利率|试结利息|\n");
        lEnd_date = g_pub_tx.tx_date;
        
        while (lEnd_date < lTx_date)
        {
            ***模拟日终先登记一条明细***
            if(i == 0)
            {
            	vtcp_log("-----首先模拟日终先登记第一条明细-----");        
            	dGc_down_amt = dBs_amt * (int)((dTx_amt-dLc_amt)/dBs_amt); **计算登记金额=dBs_amt的整数倍（向下取整)**
                if(pub_base_CompDblVal(dGc_down_amt,0.00) > 0 )
                {
            	    
                    sSxc_hst[i].tx_amt = dGc_down_amt;
                    sSxc_hst[i].intst  = 0;
                    sSxc_hst[i].rate_date = g_pub_tx.tx_date;
                    sSxc_hst[i].ic_date = g_pub_tx.tx_date;
                    
                    i++;
                   
                    dDd_mst_bal += (dTx_amt - dLc_amt - dGc_down_amt);
                    lDd_mst_ic_date = lEnd_date;
                    
                    dF_Dd_mst_bal += dTx_amt;
                    lF_Dd_mst_ic_date = lEnd_date;
                }
            }
            
            **换日**
            lEnd_date = lDaynumLAT(lEnd_date, 1);
            
            ***开始模拟活期主户结息**
            if( (lEnd_date%100)== 21 
            	 && ( (lEnd_date/100%100)==3 || (lEnd_date/100%100)==6 || (lEnd_date/100%100)==9 || (lEnd_date/100%100)==12 )
              )
            {
                 ***签约的活期利息试算**
                {
                    dIntst_hst = 0.00;
                    dTx_amt = 0.00;
                    
                    ***计算取利率的天数  取靠档利率编号***
                    memset(cLllx, 0x00, sizeof(cLllx));
                    iDays = pub_base_CalTrueDays(lDd_mst_ic_date, lEnd_date);
                    
                    strcpy(cRate_code, HQLLBH);
                    
                    iRet = pub_base_getllz(cRate_code, "01", lEnd_date, &dRate_val);
                    if (iRet != 0)
                    {
                        sprintf(acErrMsg,"从com_rate取利率值错误 [%d]",iRet);
                        WRITEMSG
                        
                        strcpy(g_pub_tx.reply, "W110");
                    }
                    
                    iDays = pub_base_CalTrueDays(lDd_mst_ic_date, lEnd_date);    **计算计息天数**
                    
                    dIntst_hst = pub_base_PubDround_1(dDd_mst_bal * iDays * dRate_val / L360 / L100); **计算利息**
                    
                   
                    ***累加结出利息**
                    dIntst_hq += dIntst_hst;
                    dDd_mst_bal += dIntst_hst;
                    
                    lDd_mst_ic_date = lEnd_date;
                }
                
                ***非签约的活期利息试算**
                
                {
                    dIntst_hst = 0.00;
                    dTx_amt = 0.00;
                    
                    ***计算取利率的天数  取靠档利率编号***
                    memset(cLllx, 0x00, sizeof(cLllx));
                    iDays = pub_base_CalTrueDays(lF_Dd_mst_ic_date, lEnd_date);
                    
                    strcpy(cRate_code, HQLLBH);
                    
                    iRet = pub_base_getllz(cRate_code, "01", lEnd_date, &dRate_val);
                    if (iRet != 0)
                    {
                        sprintf(acErrMsg,"从com_rate取利率值错误 [%d]",iRet);
                        WRITEMSG
                        strcpy(g_pub_tx.reply, "W110");
                    }
                    
                    iDays = pub_base_CalTrueDays(lF_Dd_mst_ic_date, lEnd_date);    **计算计息天数**
                    
                    dIntst_hst = pub_base_PubDround_1(dF_Dd_mst_bal * iDays * dRate_val / L360 / L100); **计算利息**
                    
                   
                    ***累加结出利息**
                    dF_Intst_hq += dIntst_hst;
                    dF_Dd_mst_bal += dIntst_hst;
                    
                    
                    lF_Dd_mst_ic_date = lEnd_date;
                }
            }
            
            vtcp_log("=========开始模拟周期计息，lEnd_date[%ld]============", lEnd_date);
            ***模拟周期结息**
            for(j = 0; j < i; j++)
            {
               iDays = pub_base_CalTrueDays(sSxc_hst[j].ic_date, lEnd_date);
               
               if(iDays == iDay)
               {
                   vtcp_log("=========sSxc_hst[%d].ic_date[%ld]=======", j, sSxc_hst[j].ic_date);
                   ***每次循环初始化***
                   dIntst_hst = 0.00;
                   dTx_amt = 0.00;
                   
                   ***计算取利率的天数  取靠档利率编号***
                   memset(cLllx, 0x00, sizeof(cLllx));
                   iDays = pub_base_CalTrueDays(sSxc_hst[j].rate_date, lEnd_date);
                   if(iDays > 0 && iDays < 7)
                   {
                       strcpy(cRate_code, HQLLBH);
                       sMo_sxc_hst.rate_date = g_pub_tx.tx_date;
                       strcpy(cLllx, "活期");
                   }
                   else if(iDays >= 7 && iDays < 90)
                   {
                       strcpy(cRate_code, QTTZCK);
                       sMo_sxc_hst.rate_date = g_pub_tx.tx_date;
                       strcpy(cLllx, "七天通知");
                   }
                   else if(iDays >= 90 && iDays < 180)
                   {
                        strcpy(cRate_code, ZCZQ03);
                        strcpy(cLllx, "整存整取三个月");
                   }
                   else if(iDays >= 180 && iDays < 360)
                   {
                       strcpy(cRate_code, ZCZQ06);
                       strcpy(cLllx, "整存整取六个月");
                   }
                   else if(iDays >=360 && iDays < 720)
                   {
                       strcpy(cRate_code, ZCZQ12);
                       strcpy(cLllx, "整存整取一年");
                   }
                   else if(iDays >=720 && iDays < 1080)
                   {
                       strcpy(cRate_code, ZCZQ24);
                       strcpy(cLllx, "整存整取二年");
                   }
                   else if(iDays >=1080 && iDays < 1800)
                   {
                       strcpy(cRate_code, ZCZQ36);
                       strcpy(cLllx, "整存整取三年");
                   }
                   else if(iDays >= 1800)
                   {
                       strcpy(cRate_code, "205");
                       strcpy(cLllx, "整存整取五年");
                   }
                   else if(iDays <= 0)
                   {
                       iDays = 0;
                       strcpy(cRate_code, HQLLBH);
                       strcpy(cLllx, "活期");
                   }
                   iRet = pub_base_getllz(cRate_code, "01", sSxc_hst[j].ic_date, &dRate_val);
                   if (iRet != 0)
                   {
                       sprintf(acErrMsg,"从com_rate取利率值错误 [%d]",iRet);
                       WRITEMSG
                       strcpy(g_pub_tx.reply, "W110");
                   }
                   
                   iDays = pub_base_CalTrueDays(sSxc_hst[j].ic_date, lEnd_date);    **计算计息天数**
                   
                   dIntst_hst = pub_base_PubDround_1(sSxc_hst[j].tx_amt * iDays * dRate_val / L360 / L100); **计算利息**
                   
                   fprintf(fp, "%ld|%ld|%ld|%d|%.2lf|%s|%.3lf%%|%.2lf|\n", \
                       sSxc_hst[j].rate_date, sSxc_hst[j].ic_date, lEnd_date, iDays, sSxc_hst[j].tx_amt, cLllx, dRate_val, dIntst_hst);
                   vtcp_log("%ld|%ld|%ld|%d|%.2lf|%s|%.3lf%%|%.2lf|\n", sSxc_hst[j].rate_date, sSxc_hst[j].ic_date, lEnd_date, iDays, sSxc_hst[j].tx_amt, cLllx, dRate_val, dIntst_hst);
                   
                   ***累加每条明细的结出利息**
                   dIntst += dIntst_hst;
                   
                   **修改ic_date, intst**
                   sSxc_hst[j].intst  += dIntst_hst;
                   sSxc_hst[j].ic_date = lEnd_date;
                   
                   dDd_mst_bal += dIntst_hst;
                   vtcp_log("-------dDd_mst_bal[%lf]----------", dDd_mst_bal);
               }
                   
            }
            
            vtcp_log("=========模拟周期结息之后模拟登记明细：lEnd_date[%ld]============", lEnd_date);
            **模拟周期结息之后模拟登记明细**
            vtcp_log("-------dDd_mst_bal[%lf]----------", dDd_mst_bal);
            if(pub_base_CompDblVal(dDd_mst_bal, dSx_amt) >= 0) ****账户余额达到生效阀值，才登记***
            {
                dGc_down_amt = dBs_amt * (int)(dDd_mst_bal/dBs_amt); **计算登记金额=dBs_amt的整数倍（向下取整)**
                
                if(pub_base_CompDblVal(dGc_down_amt,0.00) > 0 )
                {
                    **模拟登记明细**
                    sSxc_hst[i].tx_amt = dGc_down_amt;
                    sSxc_hst[i].intst  = 0;
                    sSxc_hst[i].rate_date = lEnd_date;
                    sSxc_hst[i].ic_date = lEnd_date;
                
                    i++;
                    
                    if(i > NUM)
                    {
                        break;
                    }
                    
                    dDd_mst_bal -= dGc_down_amt;
                }
                vtcp_log("-------dDd_mst_bal[%lf]----------", dDd_mst_bal);
            }
            
            iCount++;
            if(iCount > 15000)
            {
                break;	
            }
            
        }**end while**
        
        lEnd_date = lTx_date;
        ***模拟最后解约计息***
        for(j = 0; j < i; j++)
        {
            if(sSxc_hst[j].ic_date < lEnd_date)
            {
                ***每次循环初始化***
                dIntst_hst = 0.00;
                
                ***计算取利率的天数  取靠档利率编号***
                memset(cLllx, 0x00, sizeof(cLllx));
                iDays = pub_base_CalTrueDays(sSxc_hst[j].rate_date, lEnd_date);
                if(iDays > 0 && iDays < 7)
                {
                    strcpy(cRate_code, HQLLBH);
                    sMo_sxc_hst.rate_date = g_pub_tx.tx_date;
                    strcpy(cLllx, "活期");
                }
                else if(iDays >= 7 && iDays < 90)
                {
                    strcpy(cRate_code, QTTZCK);
                    sMo_sxc_hst.rate_date = g_pub_tx.tx_date;
                    strcpy(cLllx, "七天通知");
                }
                else if(iDays >= 90 && iDays < 180)
                {
                     strcpy(cRate_code, ZCZQ03);
                     strcpy(cLllx, "整存整取三个月");
                }
                else if(iDays >= 180 && iDays < 360)
                {
                    strcpy(cRate_code, ZCZQ06);
                    strcpy(cLllx, "整存整取六个月");
                }
                else if(iDays >=360 && iDays < 720)
                {
                    strcpy(cRate_code, ZCZQ12);
                    strcpy(cLllx, "整存整取一年");
                }
                else if(iDays >=720 && iDays < 1080)
                {
                    strcpy(cRate_code, ZCZQ24);
                    strcpy(cLllx, "整存整取二年");
                }
                else if(iDays >=1080 && iDays < 1800)
                {
                    strcpy(cRate_code, ZCZQ36);
                    strcpy(cLllx, "整存整取三年");
                }
                else if(iDays >= 1800)
                {
                    strcpy(cRate_code, "205");
                    strcpy(cLllx, "整存整取五年");
                }
                else if(iDays <= 0)
                {
                    iDays = 0;
                    strcpy(cRate_code, HQLLBH);
                    strcpy(cLllx, "活期");
                }
                iRet = pub_base_getllz(cRate_code, "01", sSxc_hst[j].ic_date, &dRate_val);
                if (iRet != 0)
                {
                    sprintf(acErrMsg,"从com_rate取利率值错误 [%d]",iRet);
                    WRITEMSG
                    strcpy(g_pub_tx.reply, "W110");
                }
                
                iDays = pub_base_CalTrueDays(sSxc_hst[j].ic_date, lEnd_date);    **计算计息天数**
                
                dIntst_hst = pub_base_PubDround_1(sSxc_hst[j].tx_amt * iDays * dRate_val / L360 / L100); **计算利息**
                 
                fprintf(fp, "%ld|%ld|%ld|%d|%.2lf|%s|%.3lf%%|%.2lf|\n", \
                    sSxc_hst[j].rate_date, sSxc_hst[j].ic_date, lEnd_date, iDays, sSxc_hst[j].tx_amt, cLllx, dRate_val, dIntst_hst);
                  
                ***累加每条明细的结出利息**
                dIntst += dIntst_hst;
            }
        }
          
        **模拟活期账户利息**
        if(lEnd_date > lDd_mst_ic_date) ***签约的活期利息试算**
        {
            dIntst_hst = 0.00;
            dTx_amt = 0.00;
            
            ***计算取利率的天数  取靠档利率编号***
            memset(cLllx, 0x00, sizeof(cLllx));
            iDays = pub_base_CalTrueDays(lDd_mst_ic_date, lEnd_date);
            
            strcpy(cRate_code, HQLLBH);
            
            iRet = pub_base_getllz(cRate_code, "01", lEnd_date, &dRate_val);
            if (iRet != 0)
            {
                sprintf(acErrMsg,"从com_rate取利率值错误 [%d]",iRet);
                WRITEMSG
                strcpy(g_pub_tx.reply, "W110");
            }
            
            iDays = pub_base_CalTrueDays(lDd_mst_ic_date, lEnd_date);    **计算计息天数**
            
            dIntst_hst = pub_base_PubDround_1(dDd_mst_bal * iDays * dRate_val / L360 / L100); **计算利息**
            
           
            ***累加结出利息**
            dIntst_hq += dIntst_hst;
            dDd_mst_bal += dIntst_hst;
            
            lDd_mst_ic_date = lEnd_date;
        }
         
        ***非签约的活期利息试算**
        if(lEnd_date > lF_Dd_mst_ic_date)  
        {
            dIntst_hst = 0.00;
            dTx_amt = 0.00;
            
            ***计算取利率的天数  取靠档利率编号***
            memset(cLllx, 0x00, sizeof(cLllx));
            iDays = pub_base_CalTrueDays(lF_Dd_mst_ic_date, lEnd_date);
            
            strcpy(cRate_code, HQLLBH);
            
            iRet = pub_base_getllz(cRate_code, "01", lEnd_date, &dRate_val);
            if (iRet != 0)
            {
                sprintf(acErrMsg,"从com_rate取利率值错误 [%d]",iRet);
                WRITEMSG
                strcpy(g_pub_tx.reply, "W110");
            }
            
            iDays = pub_base_CalTrueDays(lF_Dd_mst_ic_date, lEnd_date);    **计算计息天数**
            
            dIntst_hst = pub_base_PubDround_1(dF_Dd_mst_bal * iDays * dRate_val / L360 / L100); **计算利息**
            
           
            ***累加结出利息**
            dF_Intst_hq += dIntst_hst;
            dF_Dd_mst_bal += dIntst_hst;
            
            
            lF_Dd_mst_ic_date = lEnd_date;
        }
        
        fprintf(fp,"|||活期利息:%.2lf|+|遂心存利息:%.2lf|=|签约利息总额:%.2lf|\n", dIntst_hq, dIntst, dIntst+dIntst_hq);
        fprintf(fp,"|||||||不签约利息总额:%.2lf|\n", dF_Intst_hq);
        fprintf(fp,"|||||||签约可多得利息:%.2lf|\n", dIntst+dIntst_hq-dF_Intst_hq);
        fclose(fp);
        set_zd_data(DC_FILE_SND, "1");
        
        */
        
        get_zd_double("0410", &dTx_amt);
        get_zd_long("0440", &lTx_date);
        
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
        /**写表头**/
        fprintf(fp, "~登记日|起息日|结息日|计息天数|计息金额|利率类型|利率|试结利息|\n");
        
        int iMouth=0;
        lQx_Date = g_pub_tx.tx_date;
        while (pub_base_CalTrueDays(lQx_Date,lTx_date)>=0 )
        {
   					  /***----计算取利率的天数----***/
   					 vtcp_log("aaaaaaaaaaaaaaaaaa系统日期==[%ld]，截至日期==[%ld]",lQx_Date, lTx_date);
   					 iDays = pub_base_CalTrueDays(lQx_Date, lTx_date);
   					 
   					 lRate_date = lQx_Date; /**定期为起息日的利率**/
   					 memset(cLllx, 0x00, sizeof(cLllx));
   					 
   					 /***----取靠档利率编号----***/
   					 if(iDays > 0 && iDays < 7)
   					 {
   					     strcpy(cRate_code, HQLLBH);
   					     
   					     lRate_date = lQx_Date;  /**活期为支取日的利率**/
   					     strcpy(cLllx, "活期");
   					 }
   					 else if(iDays >= 7 && iDays < 90)
   					 {
   					     strcpy(cRate_code, QTTZCK);
   					     
   					     lRate_date = lQx_Date; /**通知存款为支取日的利率**/
   					     strcpy(cLllx, "七天通知");
   					 }
   					 else if(iDays >= 90 && iDays < 180)
   					 {
   					      strcpy(cRate_code, ZCZQ03);
   					      strcpy(cLllx, "整存整取三个月");
   					      
   					 }
   					 else if(iDays >= 180 && iDays < 360)
   					 {
   					     strcpy(cRate_code, ZCZQ06);
   					     strcpy(cLllx, "整存整取六个月");
   					     
   					 }
   					 else if(iDays >=360 && iDays < 720)
   					 {
   					     strcpy(cRate_code, ZCZQ12);
   					     strcpy(cLllx, "整存整取一年");
   					     
   					 }
   					 else if(iDays >=720 && iDays < 1080)
   					 {
   					     strcpy(cRate_code, ZCZQ24);
   					     strcpy(cLllx, "整存整取二年");
   					     
   					 }
   					 else if(iDays >=1080 && iDays < 1800)
   					 {
   					     strcpy(cRate_code, ZCZQ36);
   					     strcpy(cLllx, "整存整取三年");
   					     
   					 }
   					 else if(iDays >= 1800)
   					 {
   					     strcpy(cRate_code, "205");
   					     strcpy(cLllx, "整存整取五年");
   					     
   					 }
   					 else if(iDays <= 0)
   					 {
   					     iDays = 0;
   					     strcpy(cRate_code, HQLLBH);
   					     strcpy(cLllx, "活期");
   					 }
   					 else
   					 {
   					     sprintf(acErrMsg, "计算天数出错![%d]", iDays);
   					     WRITEMSG
   					     goto ErrExit;
   					 }
   					 
   					 iRet = pub_base_getllz(cRate_code, "01", lRate_date, &dRate_val);
   					 if (iRet != 0)
   					 {
   					     sprintf(acErrMsg,"从com_rate取利率值错误 [%d]",iRet);
   					     WRITEMSG
   					     strcpy(g_pub_tx.reply, "W110");
   					 }
   					 /**----计算计息天数, 目前是按每天30天算-----**/
   					 iIntst_days = pub_base_CalTrueDays(lQx_Date, lTx_date);
   					 /** -----计算利息---------**/
   					 dIntst_hst = pub_base_PubDround_1(dTx_amt * iIntst_days * dRate_val / L360 / L100);
   					 fprintf(fp, "%ld|%ld|%ld|%d|%.2lf|%s|%.3lf%%|%.2lf|\n", \
                       lQx_Date, lQx_Date, lTx_date, iIntst_days, dTx_amt, cLllx, dRate_val, dIntst_hst);

   					 
   					 dIntst_hst_all+=dIntst_hst;
   					 /*lQx_Date=lDaynumLAT(lQx_Date,30);  每月按30天算*/
   					 iMouth++;
   					 pub_base_deadlineM(g_pub_tx.tx_date,iMouth,&lQx_Date);
   					 vtcp_log("bbbbbbbbbbbbbbbbbb系统日期==[%ld]，截至日期==[%ld],iDays=[%d] pub_base_CompDblVal(lTx_date,lQx_Date)=[%d]",lQx_Date, lTx_date,iDays,pub_base_CompDblVal(lTx_date,lQx_Date));
   			}
   			/*总利息和*/
   			vtcp_log("ccccccccccccccccc系统日期==[%ld]，截至日期==[%ld],iDays=[%d]",lQx_Date, lTx_date,iDays);
        fprintf(fp,"||||||签约利息总额:|%.2lf|\n", dIntst_hst_all);
        fclose(fp);
        set_zd_data(DC_FILE_SND, "1");
    }
    else
    {
    	strcpy(g_pub_tx.reply, "S036");
        sprintf( acErrMsg, "传入标志[%s]错误!!!", cFlag);
        WRITEMSG
        goto ErrExit;
    }
        
OkExit:
    strcpy(g_pub_tx.reply, "0000");
    sprintf(acErrMsg, "success!!");
    WRITEMSG
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 0;
    
ErrExit:
    if(NULL == fp)
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

long lDaynumLAT(long date, int num) /*等于date+num(天)*/
{
	short   y1,m1,d1,num1;
	long   date1;
	long   date2;
	int i=0;
	int ret=0;
	
	date1 = date;
	
	num1=num;
	y1=date1/10000;
	m1=(date1/100)%100;
	d1=date1%100;
	
	num1=d1+num1;
	if (num<0)
	{
		vtcp_log("[%s][%d]num1==[%d]\n",__FILE__,__LINE__,num1);
		while (num1 <0)
		{
			if (((m1==5)||(m1==7)||(m1==10)||(m1==12)) && num1!=0)
			{
				vtcp_log("[%s][%d]m1==[%ld]\n",__FILE__,__LINE__,m1);
				vtcp_log("[%s][%d]num1==[%ld]\n",__FILE__,__LINE__,num1);
				if (num1<=-30)
				{
					i++;
					m1--;
					num1=num1+30;
				}
				else if ( num1 <0)
				{
					if (-num > d1)
					{
						m1--;
					}
					num1=num1+30;
				}
				vtcp_log("[%s][%d]num1==[%ld]\n",__FILE__,__LINE__,num1);
				vtcp_log("[%s][%d]m1==[%ld]\n",__FILE__,__LINE__,m1);
			}
			if (((m1==1)||(m1==2)||(m1==4)||(m1==6)||(m1==8)||(m1==9)||(m1==11)) && num1!=0 )
			{
				vtcp_log("[%s][%d]m1==[%ld]\n",__FILE__,__LINE__,m1);
				vtcp_log("[%s][%d]num1==[%ld]\n",__FILE__,__LINE__,num1);
				if (num1<=-31)
				{
					m1--;
					num1=num1+31;
					if (m1==0)
					{ 
						m1=12;
						y1=y1-1;
					}
				}
				else if ( num1 <0)
				{
					if (-num > d1)
					{
						m1--;
						if (m1==0)
						{ 
							m1=12;
							y1=y1-1;
						}
					}
					num1=num1+31;
				}
				vtcp_log("[%s][%d]num1==[%ld]\n",__FILE__,__LINE__,num1);
				vtcp_log("[%s][%d]m1==[%ld]\n",__FILE__,__LINE__,m1);
			}
			if (m1==3 && num1!=0)
			{
				vtcp_log("[%s][%d]m1==[%ld]\n",__FILE__,__LINE__,m1);
				vtcp_log("[%s][%d]num1==[%ld]\n",__FILE__,__LINE__,num1);
				if ( (( y1%4==0 && y1%100!=0 )||(y1%400==0)) && (m1==3))
				{
					vtcp_log("[%s][%d]今年闰年[%d]\n",__FILE__,__LINE__,y1);
					if (num1<=-29)
					{
						m1--; 
						num1=num1+29;
					}
					else if ( num1 <0)
					{
						if (-num > d1)
						{
							m1--;
						}
						num1=num1+29;
					}   
				}         
				else
				{
					if (num1<=-28)
					{
						m1--; 
						num1=num1+28;
					}
					else if ( num1 <0)
					{
						if (-num > d1)
						{
							m1--;
						}  
						num1=num1+28;
					}   
				}        
				vtcp_log("[%s][%d]num1==[%ld]\n",__FILE__,__LINE__,num1);
				vtcp_log("[%s][%d]m1==[%ld]\n",__FILE__,__LINE__,m1);
			}
		}
		vtcp_log("[%s][%d]num1=[%d]m1=[%d]\n",__FILE__,__LINE__,num1,m1);
		if (num1 == 0)
		{
			if ((m1==5)||(m1==7)||(m1==10)||(m1==12))
			{
				num1 = 30;
				m1--;
			}
			else if ((m1==1)||(m1==2)||(m1==4)||(m1==6)||(m1==8)||(m1==9)||(m1==11))
			{
				num1 = 31;
				m1--;
				if (m1==0)
				{ 
					m1=12;
					y1=y1-1;
				}
			}
			else
			{
				if (( y1%4==0 && y1%100!=0 )||(y1%400==0))
				{
					num1 = 29;
					m1--;
				}
				else
				{
					num1 = 28;
					m1--;
				}    
			}
		}
	}
	else
	{
		while (num1 >28)
		{
			if ((m1==1)||(m1==3)||(m1==5)||(m1==7)||(m1==8)||(m1==10)||(m1==12))
			{
				vtcp_log("[%s][%d]m1==[%ld]\n",__FILE__,__LINE__,m1);
				if (num1>31)
				{
					m1++;
					num1=num1-31;
					if (m1==13)
					{ 
						m1=1;
						y1=y1+1;
					}
				}
				else
				{
					break;
				}
				vtcp_log("[%s][%d]num1==[%ld]\n",__FILE__,__LINE__,num1);
			}  
			if ((m1==4)||(m1==6)||(m1==9)||(m1==11))
			{
				vtcp_log("[%s][%d]m1==[%ld]\n",__FILE__,__LINE__,m1);
				if (num1>30)
				{
					m1++;
					num1=num1-30;
				}
				else
				{
					break;
				}
				vtcp_log("[%s][%d]num1==[%ld]\n",__FILE__,__LINE__,num1);
			}  
			if (m1==2)
			{
				vtcp_log("[%s][%d]m1==[%ld]\n",__FILE__,__LINE__,m1);
				if ( (( y1%4==0 && y1%100!=0 )||(y1%400==0)) && (m1==2))
				{
					vtcp_log("[%s][%d]今年闰年[%d]\n",__FILE__,__LINE__,y1);
					if (num1>29)
					{
						m1++; 
						num1=num1-29;
					}
					else
					{
						break;
					}
				}
				else
				{
					if (num1>28)
					{
						m1++; 
						num1=num1-28;
					}
					else
					{
						break;
					}
				}
				vtcp_log("[%s][%d]num1==[%ld]\n",__FILE__,__LINE__,num1);
			}
		}
	}
	date2=y1*10000+m1*100+num1;
	vtcp_log("[%s][%d]date2=[%ld]\n",__FILE__,__LINE__, date2);
	return date2;
}
