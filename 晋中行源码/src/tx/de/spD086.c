/***************************************************************
* 文 件 名: spD086.c
* 功能描述：提前终止时修改理财产品参数的以下选项：
            实际收益率 返回本金比例 产品计息截止日期
            理财产品正常到期:
            实际收益率 返回本金比例
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
#include "com_tel_c.h"
int spD086()
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
    long iQXRQ = 0;/*20151103起息日*/
    double dFct_rate = 0.000000;
    double dCapital_percent = 0.000000;
    long lRt_date = 0;
    long lRe_date = 0;/*20151103阶段募集结束日*/
    char cPrdt_code[21];
    char cOpr_flag[2];
    char cCapital_percent[11];
    char sTmp_rate[10];
    struct fn_reg_c sFn_reg;
    struct fn_parm_c sFn_parm;
    struct com_tel_c sCom_tel;
    char cCltype[2];     /*处理类型  0  分配理财收益信息录入  1  分配理财收益审核*/    
    
    memset(&sCom_tel,0x00,sizeof(sCom_tel));
    memset(cCltype,0x00,sizeof(cCltype));
    memset(sTmp_rate,0x00,sizeof(sTmp_rate));
    memset(cPrdt_code,0x00,sizeof(cPrdt_code));
    memset(cOpr_flag,0x00,sizeof(cOpr_flag));
    memset(&sFn_reg,0x00,sizeof(sFn_reg));
    memset(&sFn_parm,0x00,sizeof(sFn_parm));
    memset(cCapital_percent,0x00,sizeof(cCapital_percent));

		get_zd_data("0340",cCltype);
		get_zd_long("0460",&iQXRQ);
		vtcp_log("aaaaaaa收到的阶段性起息日期是： %d ", iQXRQ);
    get_zd_data("0630",cPrdt_code);
    get_zd_data("0670",cOpr_flag);
    get_zd_long("0440",&lRt_date);
    get_zd_long("0450",&lRe_date);/*20151103阶段募集结束日*/
    get_zd_data("0640",cCapital_percent);
    get_zd_data("0730",sTmp_rate);
    dFct_rate=atof(sTmp_rate)/1000000;
    dCapital_percent = atof(cCapital_percent) / 10000;
    vtcp_log("prdt_code[%s],rt_date[%ld],fct_rate[%lf] capital_percent[%lf][%s]333333333333sTmp_rate[%s]",\
            cPrdt_code,lRt_date,dFct_rate,dCapital_percent,cCapital_percent,sTmp_rate);
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
    if(cOpr_flag[0] == '1' || cOpr_flag[0] == '2')
    {	
    
    if(cCltype[0] == '0')  /*处理类型为分配理财收益信息录入，只录入信息不分配收益*/
    {
    	if(cOpr_flag[0] == '1')	/*正常到期 设置归还日期 实际收益率 返还本金比例*/
    	{
    	sFn_parm.return_date =g_pub_tx.tx_date;
        sFn_parm.fct_rate = dFct_rate;
        sFn_parm.capital_percent = dCapital_percent;
        strcpy(sFn_parm.tel1,g_pub_tx.tel);
        strcpy(g_pub_tx.brf,"理财产品设置归还日期");    		
    	}
   	 	if(cOpr_flag[0] == '2')  /*提前中止 设置归还日期 同时更新计息截止日期 实际收益率 返还本金比例*/
   		{
   			strcpy(g_pub_tx.brf,"理财产品提前中止");
   			strcpy(sFn_parm.tel1,g_pub_tx.tel);
   			vtcp_log(" sFn_parm.tel1 = %s",sFn_parm.tel1);
			sFn_parm.return_date = lRt_date;
			sFn_parm.fct_rate = dFct_rate;
			sFn_parm.capital_percent = dCapital_percent;	
   			
   		}
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
    		
	if(cOpr_flag[0] == '1')/*正常到期 设置归还日期 实际收益率 返还本金比例*/
	{
    		
    		if(pub_base_CompDblVal(sFn_parm.fct_rate,dFct_rate))
       		{
        		sprintf(acErrMsg,"输入的实际利率与录入时不符！");
        		strcpy(g_pub_tx.reply,"H034");
        		set_zd_data(DC_GET_MSG,acErrMsg);
        		WRITEMSG
        		goto ErrExit;
       		}
        
     	strcpy(sFn_parm.tel2,g_pub_tx.tel);
        strcpy(g_pub_tx.brf,"理财产品设置归还日期");
        *mode = 1;
        /*add by lwb */
        iRet = Fn_reg_Dec_Upd(g_pub_tx.reply,"prdt_code='%s' and sts ='1'",sFn_parm.prdt_code);
        if(iRet)
        {
            vtcp_log("建立更新游标错误! iRet[%d] ",iRet);
            strcpy(g_pub_tx.reply,"D332");
            goto ErrExit;
        }
        while(1)
        {
            iRet = Fn_reg_Fet_Upd(&sFn_reg,g_pub_tx.reply);
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
            sFn_reg.end_date = lRt_date;
            vtcp_log("%s,%d return_rate = %f",__FILE__,__LINE__,sFn_reg.return_rate);
            strcpy(sFn_reg.filler,"正常处理");
            sFn_reg.return_rate = dFct_rate*(1+sFn_reg.flt_radio); /* add by lwb */
            vtcp_log("lRt_date[%d] ", sFn_parm.return_date);
            iRet = Fn_reg_Upd_Upd(sFn_reg,g_pub_tx.reply);
            if(iRet)
            {
                vtcp_log("更新fn_reg错误! iRet[%d] ",iRet);
                strcpy(g_pub_tx.reply,"D333");
                goto ErrExit;
            }
            
        }
        /*add end lwb*/
    }
    else if(cOpr_flag[0] == '2')/*提前中止 设置归还日期 同时更新计息截止日期 实际收益率 返还本金比例*/
    {
        
        if(pub_base_CompDblVal(sFn_parm.fct_rate,dFct_rate))
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
        if(pub_base_CompDblVal(sFn_parm.capital_percent,dCapital_percent))
        {
        	sprintf(acErrMsg,"输入的归还本金比例与录入时不符！");
        	
        	strcpy(g_pub_tx.reply,"H034");
        	set_zd_data(DC_GET_MSG,acErrMsg);
        	WRITEMSG
        	goto ErrExit;	
        }
        strcpy(sFn_parm.tel2,g_pub_tx.tel);
        vtcp_log("sFn_parm.tel2 = %s",sFn_parm.tel2);
        strcpy(g_pub_tx.brf,"理财产品提前中止");
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
            sFn_reg.end_date = lRt_date;
            sFn_reg.return_rate = dFct_rate*(1+sFn_reg.flt_radio); /* add by lwb */
						vtcp_log("%s,%d return_rate = %f",__FILE__,__LINE__,sFn_reg.return_rate);
            strcpy(sFn_reg.filler,"提前中止");
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
  /*add end l
  wb*/
		}
    
    else if(cOpr_flag[0] == '3')
    {
        strcpy(sFn_parm.stat,"4");
        strcpy(g_pub_tx.brf,"理财产品暂停发售");
    }
    else if(cOpr_flag[0] == '4')
    {
        strcpy(g_pub_tx.brf,"理财产品重启发售");
        if(strcmp(sFn_parm.stat,"4") == 0)
        {
            strcpy(sFn_parm.stat,"0");/*重新认购*/
        }
        else if(strcmp(sFn_parm.stat,"6") ==0)
        {
            strcpy(sFn_parm.stat,"1");/*可以申购*/
        }
        else
        {
            vtcp_log("未知的状态[%s]",sFn_parm.stat);
            strcpy(g_pub_tx.reply,"S091");
            goto ErrExit;
        }
    }
    else if(cOpr_flag[0] == '5')
    {
        strcpy(g_pub_tx.brf,"理财产品宣布成立");
        strcpy(sFn_parm.stat,"5");
    }
    else if(cOpr_flag[0] == '6')
    {
        strcpy(g_pub_tx.brf,"理财产品停止申购");
        strcpy(sFn_parm.stat,"6");
    }
    /*LJ 20151102start*/
    else if(cOpr_flag[0] == '7')
    {
        strcpy(sFn_parm.stat,"0");
        strcpy(g_pub_tx.brf,"开放式产品开放");
        sFn_parm.ic_date=iQXRQ;/*20151103*/
        sFn_parm.beg_date=lRt_date;/*20151103*/
        sFn_parm.end_date=lRe_date;/*20151103*/
    }
    /*LJ end*/
    /*LJ 20151104start*/
    else if(cOpr_flag[0] == '8')
    {
        strcpy(sFn_parm.stat,"1");      
     		iRet = sql_execute("update fn_br_profit set sts='0',tx_amt=0.00,redempt_amt=0.00,profit_amt=0.00 where prdt_code='%s' and sts='1'",cPrdt_code);/*LJ20151117*/
     		if( iRet ) 
				{
      		sprintf( acErrMsg, "执行sql_execute错![%d]", iRet );
					WRITEMSG
					goto ErrExit;
      	}
        strcpy(g_pub_tx.brf,"开放结束");
    }
    /*LJ end*/
    else
    {
        vtcp_log("未知的操作代码! cOpr_flag[%s] ",cOpr_flag);
        strcpy(g_pub_tx.reply,"S083");
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
