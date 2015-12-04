/***************************************************************
* 文 件 名: spD094.c
* 功能描述：开放式理财产品提前分配收益以下选项：
            实际收益率  产品计息截止日期
            理财产品正常到期:
            实际收益率 
            其他情况只改下状态


            
* 作    者: shangyongchao
* 完成日期：2013/5/21
*
* 修改记录:
* 日    期:
* 修 改 人:
* 备    注:
*******************************************************************************/
#include "string.h"
#define EXTERN
#include "public.h"
#include "fn_parm_c.h"
#include "fn_reg_c.h"
#include "fn_ac_rel_c.h"
#include "mdm_ac_rel_c.h"
#include "in_mst_c.h"
#include "com_tel_c.h"
#define L365 365
int spD094()
{
    char cTx_code[9];
    int iFlag = 0 ;
    memset(cTx_code,0x00,sizeof(cTx_code));

    pub_base_sysinit();
    get_zd_data("0650",cTx_code);

    /*add by lwb 20140820 增加对机构的判断*/
    if(strcmp(g_pub_tx.tx_br_no,QS_BR_NO))
    {
	sprintf(acErrMsg,"非清算中心不允许做此交易!");
	WRITEMSG
	strcpy(g_pub_tx.reply,"P047");
	goto ErrExit;
    }

    if(strcmp(cTx_code,"0001") == 0)
    {
        if(func_0001(&iFlag))
        {
            vtcp_log("修改理财产品参数状态错误了");
            goto ErrExit;
        }
        vtcp_log("标志iFlag[%d]",iFlag);
        if(iFlag == 1)
        {
            /*去下一流程 分收益去喽*/
            vtcp_log("分收益去喽[%d]",iFlag);
            goto OkExit_Next;
        }
    }
    else
    {
        vtcp_log("未知的操作代码[%s]",cTx_code);
        strcpy(g_pub_tx.reply,"S083");
        goto ErrExit;
    }

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    vtcp_log( "Before OK return: reply is[%s]", g_pub_tx.reply);
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 0;
OkExit_Next:
    strcpy(g_pub_tx.reply,"0001");
    vtcp_log( "Before OK return: reply is[%s]", g_pub_tx.reply);
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 0;
ErrExit:    
    vtcp_log("Before return: reply is[%s]", g_pub_tx.reply);
    if(strlen(g_pub_tx.reply) == 0 || strcmp(g_pub_tx.reply,"0000") == 0)
    {
        strcpy(g_pub_tx.reply,"S035");
    }
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 1;
}
/*需要后续流程的 mode返回1 比如正常到期处理和提前中止
  其他返回0
  */
static int func_0001(int *mode)
{
    int iRet = 0;
    double dFct_rate1 = 0.000000;
    double dFct_rate = 0.000000;
    double dCapital_percent = 0.000000;
    long lRt_date = 0;
    char cPrdt_code[21];
    char cOpr_flag[2];
    char cCapital_percent[11];
    char sTmp_rate[10];
    struct fn_reg_c sFn_reg;
    struct fn_parm_c sFn_parm;
    struct com_tel_c sCom_tel;
    double syamt=0.00;/*实际收益金额*/
    long lDays=0;

    double fct_amt=0.00;
    double fct_profit_amt=0.00;
    int ret=0;
    double  fct_intst=0.00;
    double dProfit_rate=0.00;
    double dFct_intst = 0.00;
    double dTmp_amt = 0.00;
    double dProfit_amt=0.00;
    double fct_intst_tot=0.00;
    struct fn_ac_rel_c sFn_ac_rel;
    struct mdm_ac_rel_c  sMdm_ac_rel;
    struct in_mst_c  sIn_mst;
    char cCltype[2];     /*处理类型  0  分配理财收益信息录入  1  分配理财收益审核*/    
    
    memset(&sCom_tel,0x00,sizeof(sCom_tel));
    memset(cCltype,0x00,sizeof(cCltype));
    memset(sTmp_rate,0x00,sizeof(sTmp_rate));
    memset(cPrdt_code,0x00,sizeof(cPrdt_code));
    memset(cOpr_flag,0x00,sizeof(cOpr_flag));
    memset(&sFn_reg,0x00,sizeof(sFn_reg));
    memset(&sFn_ac_rel,0x00,sizeof(sFn_ac_rel));
    memset(&sMdm_ac_rel,0x00,sizeof(sMdm_ac_rel));
    memset(&sIn_mst,0x00,sizeof(sIn_mst));
    memset(&sFn_parm,0x00,sizeof(sFn_parm));
    memset(cCapital_percent,0x00,sizeof(cCapital_percent));

		get_zd_data("0340",cCltype);
    get_zd_data("0630",cPrdt_code);
    get_zd_data("0670",cOpr_flag);
    get_zd_long("0440",&lRt_date);
    get_zd_data("0640",cCapital_percent);
    get_zd_data("0730",sTmp_rate);
  
    
    dFct_rate1=atof(sTmp_rate)/1000000;
    dCapital_percent = atof(cCapital_percent) / 10000;
    vtcp_log("prdt_code[%s],rt_date[%ld],fct_rate[%lf] capital_percent[%lf][%s]333333333333sTmp_rate[%s]",\
            cPrdt_code,lRt_date,dFct_rate1,dCapital_percent,cCapital_percent,sTmp_rate);
    iRet = Fn_parm_Dec_Upd(g_pub_tx.reply,"prdt_code='%s'",cPrdt_code);
    if(iRet)
    {
        vtcp_log("定义更新理财产品参数游标失败! prdt_code[%s] iRet[%d]",cPrdt_code,iRet);
        strcpy(g_pub_tx.reply,"D332 ");
        goto ErrExit;
    }
    iRet = Fn_parm_Fet_Upd(&sFn_parm,g_pub_tx.reply);
    if(iRet == 100)
    {
        vtcp_log("没有这个理财产品! prdt_code[%s] iRet[%d]",sFn_reg.prdt_code,iRet);
        strcpy(g_pub_tx.reply,"D104");
        goto ErrExit;
    }
    if(iRet)
    {
        vtcp_log("查询理财产品参数表失败! prdt_code[%s] iRet[%d]",sFn_reg.prdt_code,iRet);
        strcpy(g_pub_tx.reply,"D324");
        goto ErrExit;
    }
    
    /*add by lwb 20140923  增加一个分配理财收益审核的交易 并把原来的改为信息录入*/
   
    if(cCltype[0] == '0')  /*处理类型为分配理财收益信息录入，只录入信息不分配收益*/
    {
    	if(cOpr_flag[0] == '3')	/*开放式理财提前分配收益 设置归还日期 实际收益率 */
    	{
    	sFn_parm.return_date =lRt_date;
        sFn_parm.fct_rate = dFct_rate1;
        strcpy(sFn_parm.tel1,g_pub_tx.tel);
        strcpy(g_pub_tx.brf,"开放理财分配收益");    		
    	}
    	 /*增加理财归集账户金额判断lpj*/
  /*按认购合同入账,而且要按认购机构出报表*/
    ret=Fn_reg_Dec_Upd(g_pub_tx.reply,"prdt_code='%s' and sts='1' and ic_date<%ld order by tx_br_no,ac_id",sFn_parm.prdt_code,lRt_date);
    if( ret )
    {
        vtcp_log("declare cursor error! ret[%d] ",ret);
        strcpy( g_pub_tx.reply,"D332" );
        goto ErrExit;
    }/*计算收益*/
    fct_intst=0.00;
     fct_profit_amt=0.00;
    while(1)
    {
        lDays = 0;
        dFct_rate = 0.000000;
        dProfit_rate = 0.000000; /*分润利率*/

        dFct_intst = 0.00;
        dTmp_amt = 0.00;
  
        memset(&g_dd_mst,0x00,sizeof(g_dd_mst));
        memset(&g_in_mst,0x00,sizeof(g_in_mst));
        memset(&sFn_reg,0x00,sizeof(struct fn_reg_c));
        ret=Fn_reg_Fet_Upd(&sFn_reg,g_pub_tx.reply);
        if( ret==100 )
        {
        	vtcp_log("已经取完数了，结束");
        	break;      		
        }
        else if( ret )
        {
            vtcp_log("fn_reg fetch error! ret[%d] ",ret);
            strcpy(g_pub_tx.reply,"D330");
            goto ErrExit;
        }
    {          
            dProfit_rate = sFn_parm.profit_rate;
            dFct_rate = dFct_rate1;
            lDays =  pub_base_CalTrueDays(sFn_reg.ic_date,lRt_date);
            
            /*add by lwb 晋中商行要求一年为365天*/ 
           if(lDays == 366)
	          {
		          lDays = 365;
	          }
            vtcp_log("利率[%lf],天数[%ld]  ic_date [%ld] end_date[%ld] ",dFct_rate,lDays,sFn_reg.ic_date,sFn_reg.end_date);
          		
 	 }
 	 /*计算收益 dFct_intst考虑后期加上开放式的调整利率的未分配收益*/
        dTmp_amt =(sFn_reg.tx_amt - sFn_reg.redempt_amt)* lDays * dFct_rate * (1 + sFn_reg.flt_radio)/L365/L100;
        dFct_intst += pub_base_PubDround_1(dTmp_amt);
        sFn_reg.fct_intst += dFct_intst;
        vtcp_log("本次计算出的收益[%lf]本次总实际收益[%lf] 预期[%lf]",dTmp_amt,dFct_intst,sFn_reg.intst);
        
        fct_intst+=sFn_reg.fct_intst;
        
 }
 			fct_intst_tot=fct_intst;
 		 ret=Fn_ac_rel_Sel(g_pub_tx.reply,&sFn_ac_rel,"prdt_code='%s'",sFn_parm.prdt_code);
    if(ret)
    {
        vtcp_log("查询理财产品清算账号配置表错误");
        strcpy(g_pub_tx.reply,"S035");
        goto ErrExit;
    }
    ret  = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_no = '%s'",sFn_ac_rel.qs_ac_no);
    if(ret != 0 && ret != 100)
    {
        vtcp_log( "查找账号失败![%d]", iRet);
        
        	set_zd_data(DC_GET_MSG,acErrMsg);
        	WRITEMSG
        	goto ErrExit;
    }
    else if(ret == 100)
    {
        vtcp_log( "无此账号![%s]", sFn_ac_rel.qs_ac_no);
        
        	set_zd_data(DC_GET_MSG,acErrMsg);
        	WRITEMSG
        	goto ErrExit;
    }
    ret = In_mst_Sel(g_pub_tx.reply,&sIn_mst,"ac_id=%ld and ac_seqn=%d",sMdm_ac_rel.ac_id,sMdm_ac_rel.ac_seqn);
    if(ret)
    {
        vtcp_log("查询内部户失败[%d]",ret);
        strcpy(g_pub_tx.reply,"W022");
      
        	set_zd_data(DC_GET_MSG,acErrMsg);
        	WRITEMSG
        	goto ErrExit;
       
    }
    vtcp_log("需要分配的收益金额为%16.2f,理财账户金额为%16.2f，%d ,%s",fct_intst_tot,sIn_mst.bal,__LINE__,__FILE__);
    if(pub_base_CompDblVal(sIn_mst.bal,fct_intst_tot)<0)
    	{
    		vtcp_log("理财清算账户余额不足，请充值");
        strcpy(g_pub_tx.reply,"A030");
      
        	set_zd_data(DC_GET_MSG,acErrMsg);
        	WRITEMSG
        	goto ErrExit;
    	}else if(pub_base_CompDblVal(fct_intst_tot,0.00)<=0)
    	{
    		vtcp_log("收益计算出错，请检查");
        strcpy(g_pub_tx.reply,"L071");
      
        	set_zd_data(DC_GET_MSG,acErrMsg);
        	WRITEMSG
        	goto ErrExit;
    	}
    /*增加理财归集账户金额判断lpj*/
   	 	
   		sFn_parm.stat[0] = '7';  /*fn_parm 的状态改为7 分配收益录入状态  add by lwb 20140926*/
    }
    else if(cCltype[0] == '1'){   /*处理类型为分配理财收益审核  ，通过即可分配收益*/
    	
    	vtcp_log(" sFn_parm.tel1 =%s  and g_pub_tx.tel =%s",sFn_parm.tel1,g_pub_tx.tel);
    	if(!strcmp(g_pub_tx.tel,sFn_parm.tel1))
    	{
    			
    		sprintf(acErrMsg,"审核和录入不能为同一个柜员！");       
        	strcpy(g_pub_tx.reply,"H034");
        	set_zd_data(DC_GET_MSG,acErrMsg);
        	WRITEMSG
        	goto ErrExit;	
    	}
    	*mode = 1;
    	/*iRet = Com_tel_Sel(g_pub_tx.reply,&sCom_tel,"tel = '%s' and lvl = '7' and br_no = '10001'",g_pub_tx.tel);
    	if(iRet == 100)
    	{
    		sprintf(acErrMsg,"柜员级别不够！");
    		strcpy(g_pub_tx.reply,"H034");
		set_zd_data(DC_GET_MSG,acErrMsg);
		WRITEMSG
		goto ErrExit;	
    	}*/
    	if(strcmp(g_pub_tx.tel,"0016"))
    		{
    		sprintf(acErrMsg,"柜员级别不够！");
    		strcpy(g_pub_tx.reply,"H034");
				set_zd_data(DC_GET_MSG,acErrMsg);
				WRITEMSG
				goto ErrExit;
		 }
    if(cOpr_flag[0] == '3')/*提前分配收益设置归还日期 同时更新计息截止日期 实际收益率 */
    {
        
        if(pub_base_CompDblVal(sFn_parm.fct_rate,dFct_rate1))
        {
        	sprintf(acErrMsg,"输入的实际利率与录入时不符！");
        	
        	strcpy(g_pub_tx.reply,"H034");
        	set_zd_data(DC_GET_MSG,acErrMsg);
        	WRITEMSG
        	goto ErrExit;
        }
        if(sFn_parm.return_date != lRt_date)
        {
        	sprintf(acErrMsg,"输入的计息截至日期与录入时不符！");
        	
        	strcpy(g_pub_tx.reply,"H034");
        	set_zd_data(DC_GET_MSG,acErrMsg);
        	WRITEMSG
        	goto ErrExit;	
        }
        
        strcpy(sFn_parm.tel2,g_pub_tx.tel);
        vtcp_log("sFn_parm.tel2 = %s",sFn_parm.tel2);
        strcpy(g_pub_tx.brf,"开放理财分配收益");
     		*mode = 1;
     
    	  /*提前中止的把fn_reg的计息结束日期改为当前交易日期 重算利息用*/
        iRet = Fn_reg_Dec_Upd(g_pub_tx.reply,"prdt_code='%s' and sts ='1'",sFn_parm.prdt_code);
        if(iRet)
        {
            vtcp_log("建立更新游标错误! iRet[%d] ",iRet);
            strcpy(g_pub_tx.reply,"D332");
            goto ErrExit;
        }
        while(1)
        {
						 vtcp_log("开始循环了 ", sFn_parm.return_date);        	
            memset(&sFn_reg,0x00,sizeof(sFn_reg));
            iRet = Fn_reg_Fet_Upd(&sFn_reg,g_pub_tx.reply);
            vtcp_log("开始循环了11 ", sFn_parm.return_date); 
            if(iRet != 0 && iRet != 100)
            {
                vtcp_log("取更新游标错误! iRet[%d] ",iRet);
                strcpy(g_pub_tx.reply,"D330");
                goto ErrExit;
            }
            else if(iRet == 100)
            {
                break;
            }
            if(lRt_date!=0)
            
            sFn_reg.return_rate = dFct_rate*(1+sFn_reg.flt_radio); /* add by lwb */
						vtcp_log("%s,%d return_rate = %f",__FILE__,__LINE__,sFn_reg.return_rate);
            strcpy(sFn_reg.filler,"提前分配收益");
            vtcp_log("lRt_date[%d] ", sFn_parm.return_date);
            iRet = Fn_reg_Upd_Upd(sFn_reg,g_pub_tx.reply);
            if(iRet)
            {
                vtcp_log("更新fn_reg错误! iRet[%d] ",iRet);
                strcpy(g_pub_tx.reply,"D333");
                goto ErrExit;
            }
        }
        Fn_reg_Clo_Upd();
    }
  }
  /*add end lwb*/
		
  			else   
        {
            vtcp_log("未知的状态[%s]",sFn_parm.stat);
            strcpy(g_pub_tx.reply,"S091");
            goto ErrExit;
        }
    
    
    iRet = Fn_parm_Upd_Upd(sFn_parm,g_pub_tx.reply);
    vtcp_log("跳出循环了33 ", sFn_parm.return_date); 
    if(iRet)
    {
        vtcp_log("更新理财产品参数表失败");
        strcpy(g_pub_tx.reply,"D334 ");
        goto ErrExit;
    }
    strcpy(g_pub_tx.ac_no,cPrdt_code);
    if( pub_ins_trace_log() )
    {
        vtcp_log("登记交易流水出错");
        strcpy(g_pub_tx.reply,"P126");
        goto ErrExit;
    }
    Fn_reg_Clo_Upd();
    Fn_parm_Clo_Upd();


OkExit:
    strcpy(g_pub_tx.reply,"0000");
    vtcp_log( "Before OK return: reply is[%s]", g_pub_tx.reply);
    return 0;
ErrExit:    
    vtcp_log("Before return: reply is[%s]", g_pub_tx.reply);
    return 1;
}
