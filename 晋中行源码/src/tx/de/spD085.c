/*************************************************************
* 文 件 名: spD085.c
* 功能描述: 分配支行理财收益

* 作    者: 李文博
* 完成日期: 2014/9/28
insert into TX_DEF (TX_CODE, TX_NAME, BIT_MAP, LOG_IND, TX_TYPE, WRK_STS) values ('2225', '理财产品分配支行收益', '10000000000010000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000', '1', '1', '0');
insert into TX_FLOW_DEF (TX_CODE, FLOW_CODE, SUB_TX_CODE, SUB_TX_RSLT) values ('2225', 0, 'D085', '#=0001@1@#$');
insert into TX_FLOW_DEF (TX_CODE, FLOW_CODE, SUB_TX_CODE, SUB_TX_RSLT) values ('2225', 1, 'D091', '#$');
insert into TX_SUB_DEF (SUB_TX_CODE, SUB_TX_NAME, TX_FUNC, ACCT_IND, CHNL_IND) values ('D085, '理财参数维护', 'spD086', '0', '0');
insert into TX_SUB_DEF (SUB_TX_CODE, SUB_TX_NAME, TX_FUNC, ACCT_IND, CHNL_IND) values ('D091', '理财分配支行收益', 'spD091', '0', '0');

insert into DIC_DATA (PR_CODE, PR_SEQ, CONTENT) values ('fn_parm_sts', '8', '已分配支行收益');

* 修改记录:
* 日    期:
* 修 改 人:
* 备    注:
**************************************************************/
#include "string.h"
#define EXTERN
#include "public.h"
#include "fn_parm_c.h"
#include "com_tel_c.h"
#include "fn_br_profit_c.h"

int spD085()
{
    char cTx_code[9];
    int iFlag = 0 ;
    memset(cTx_code,0x00,sizeof(cTx_code));

    pub_base_sysinit();
    get_zd_data("0650",cTx_code);

   
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
            vtcp_log("分支行收益去喽[%d]",iFlag);
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
        strcpy(g_pub_tx.reply,"H034");
    }
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 1;
}

int func_0001(int * mode)
{
	int iRet = 0;
	int ttlnum = 0;
	double dRate = 0.00;
	char cPrdt_code[21];
	char cTmp_rate[20];
	struct fn_br_profit_c sFn_br_profit;
	struct fn_parm_c sFn_parm;
	
	
	memset(&sFn_parm,0x00,sizeof(sFn_parm));
	memset(&sFn_br_profit,0x00,sizeof(sFn_br_profit));
	memset(cTmp_rate,0x00,sizeof(cTmp_rate));
	memset(cPrdt_code,0x00,sizeof(cPrdt_code));
	
	get_zd_data("0630",cPrdt_code);
	get_zd_data("0730",cTmp_rate);
	dRate = atof(cTmp_rate)/1000000;
	vtcp_log("dRate = '%lf'",dRate);
	
	iRet = Fn_parm_Sel(g_pub_tx.reply,&sFn_parm,"prdt_code = '%s' ",cPrdt_code);
	if(iRet == 100)
	{
		sprintf(acErrMsg,"无此理财产品！");
		WRITEMSG
		strcpy(g_pub_tx.reply,"D324");
		goto ErrExit;	
	}
	else if(iRet)
	{
		sprintf(acErrMsg,"查询理财产品参数表错误！");
		WRITEMSG
		strcpy(g_pub_tx.reply,"D324");
		goto ErrExit;	
	}
	
	if(sFn_parm.stat[0]!='2')
	{
		sprintf(acErrMsg,"该理财产品状态不为已分配客户收益！");
		strcpy(g_pub_tx.reply,"H034");
		set_zd_data(DC_GET_MSG,acErrMsg);
		WRITEMSG
		goto ErrExit;		
	}
	if(sFn_parm.stat[0] == '8')
	{
		sprintf(acErrMsg,"该理财产品已分配支行收益！");
		strcpy(g_pub_tx.reply,"H034");
		set_zd_data(DC_GET_MSG,acErrMsg);
		WRITEMSG
		goto ErrExit;		
	}
	
	iRet = Fn_br_profit_Dec_Upd(g_pub_tx.reply,"prdt_code = '%s' and br_no!='20400' and sts='0'",cPrdt_code);   /*网银不分配支行收益*//*只有登记的支行利润才进行分配，已入账的则不进行分配lpj20151109*/
	if(iRet)
	{
		sprintf(acErrMsg,"查询理财产品机构分润表错误！");
		WRITEMSG
		strcpy(g_pub_tx.reply,"D344");
		goto ErrExit;		
	}
	while(1)
	{
		iRet = Fn_br_profit_Fet_Upd(&sFn_br_profit,g_pub_tx.reply);	
		if(iRet == 100)
		{
			if(!ttlnum)
			{
				sprintf(acErrMsg,"理财产品机构分润表中无此产品记录！");
				WRITEMSG
				strcpy(g_pub_tx.reply,"D344");
				goto ErrExit;		
			}
			break;
		}
		else if(iRet)
		{
			sprintf(acErrMsg,"查询理财产品机构分润表错误！");
			WRITEMSG
			strcpy(g_pub_tx.reply,"D344");
			goto ErrExit;		
		}
		sFn_br_profit.profit_amt = sFn_br_profit.tx_amt * dRate/100;
		sFn_br_profit.reg_date = g_pub_tx.tx_date;
		sFn_br_profit.reg_trace_no = g_pub_tx.trace_no;
		
		  
	  iRet = Fn_br_profit_Upd_Upd(sFn_br_profit,g_pub_tx.reply);
		if(iRet)
		{
			sprintf(acErrMsg,"更新理财产品机构分润表错误！");
			WRITEMSG
			strcpy(g_pub_tx.reply,"D344");
			goto ErrExit;			
		}
		ttlnum++;
	}
	Fn_br_profit_Clo_Upd();
	*mode = 1;	
	
	strcpy(g_pub_tx.ac_no,cPrdt_code);
	strcpy(g_pub_tx.brf,"分配支行收益");
  if( pub_ins_trace_log() )
  {
    vtcp_log("登记交易流水出错");
    strcpy(g_pub_tx.reply,"P126");
    goto ErrExit;
  }
	
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    vtcp_log( "Before OK return: reply is[%s]", g_pub_tx.reply);
    return 0;
ErrExit:    
    vtcp_log("Before return: reply is[%s]", g_pub_tx.reply);
    return 1;
	
	
}