/*********************************************************************
* 版 本 号 :  
* 程序作者 :  lixiang
* 日    期 :  2014/10/21
* 所属模块 :  理财产品信息查询
* 程序名称 :  spW201.c
* 程序作用 :
* 函数列表 :
* 使用注意 :
交易配置：
insert into tx_def values ('5051', '理财产品信息查询', '10000000000000000000110010010011100000111111111110000000000001000111111100000000000110000000100000011000000000000000000000000000', '1','6', '2');
insert into tx_flow_def values ('5051', 0, 'W201', '#$');
insert into tx_sub_def values ('W201', '理财产品信息查询', 'spW201', '0', '0');
***********************************************************************/
#define EXTERN
#include "public.h"
#include "fn_parm_c.h"			/*理财产品参数表*/
#include "fn_reg_c.h"			/*理财产品登记簿*/
#include "fn_br_quota_parm_c.h"		/*理财产品机构配额参数表*/

int spW201()
{
	struct fn_parm_c sFn_parm;  	 	/*理财产品参数表*/
	struct fn_br_quota_parm_c sFn_br_qp;	/*理财产品机构配额参数表*/
	
	char cPrdt_code[20];
	int iRt;
	int ret=0;
	double restAmt=0.00;
	double dTx_amt=0.00;
	double dRedempt_amt = 0.00;
	double dBuying_amt = 0.00;
	double trust_rate=0.00;

	memset(cPrdt_code,0x00,sizeof(cPrdt_code));
	memset(&sFn_br_qp,0x00,sizeof(sFn_br_qp));
	
	/**初始化公用结构**/
	if (pub_base_sysinit())
	{
	    vtcp_log("初始化公用结构错误!!");
	    strcpy(g_pub_tx.reply, "CU06");
	    goto ErrExit;
	}
	
	/*从平台获取查询条件*/
	get_zd_data("0620",cPrdt_code);
	if(!strlen(cPrdt_code))
	{
		strcpy(g_pub_tx.reply,"9999");
		set_zd_data("0130","查询条件不能为空");
		sprintf(acErrMsg,"查询条件不能为空!");
		WRITEMSG
		vtcp_log("fail !!: reply is[%s]", g_pub_tx.reply);
		goto ErrExit;
	}
	
	
	/*根据产品代码查询理财产品参数表fn_parm*/
	iRt=-1;
	iRt=Fn_parm_Sel(g_pub_tx.reply,&sFn_parm,"prdt_code='%s'",cPrdt_code);
	if( iRt==100 )
		{
			vtcp_log("fatal error! table no record ");
			strcpy(g_pub_tx.reply,"T062");
			goto ErrExit;
		}
	else if( iRt )
		{
			vtcp_log("database error! ret[%d] ",iRt);
			strcpy(g_pub_tx.reply,"D324");
			goto ErrExit;
		}
		
	set_zd_data("0250",sFn_parm.title);		/*产品名称*/
	set_zd_data("0670",sFn_parm.prdt_type);		/*产品类型*/
	set_zd_data("0680",sFn_parm.intst_type);	/*收益类型*/
	set_zd_long("0440",sFn_parm.beg_date);		/*募集起始日期*/
	set_zd_long("0450",sFn_parm.end_date);		/*募集结束日期*/
	set_zd_long("0460",sFn_parm.ic_date);		/*产品成立日期*/
	set_zd_long("0470",sFn_parm.mtr_date);		/*产品到期日期*/
	set_zd_long("0280",sFn_parm.term);		/*期限*/
	set_zd_data("0700",sFn_parm.cif_type);		/*客户类型：0都可，1个人，2对公*/
	set_zd_data("0690",sFn_parm.risk_lvl);		/*风险等级*/
	set_zd_data("0720",sFn_parm.invest_type);	/*投资方向*/
	set_zd_data("0210",sFn_parm.cur_no);		/*币种*/
	set_zd_double("0840",sFn_parm.rate);		/*预期最大年化收益率*/
	vtcp_log("预期最大年化收益率=[%9.6f]",sFn_parm.rate);
	set_zd_double("0850",sFn_parm.chrg_rate);	/*手续费利率*/
	set_zd_double("0310",sFn_parm.trust_rate);	/*托管费率*/
	get_zd_double("0310",&trust_rate);
	vtcp_log("托管费率--=[%9.6f]",trust_rate);
	
	set_zd_double("0390",sFn_parm.sale_amt);	/*发售金额*/
	/*已经认购的总额*/
	iRt=-1;
	iRt=sql_sum_double("fn_reg","tx_amt",&dTx_amt,"prdt_code='%s' and sts <> '*'",sFn_parm.prdt_code);
   	if( iRt)
    	{
        	vtcp_log("sql_sum_double error! ret[%d] ",iRt);
       		strcpy(g_pub_tx.reply,"D331");
       		goto ErrExit;
    	}
   	set_zd_double("0430",dTx_amt);			/*该产品认购总额*/
   	restAmt=sFn_parm.sale_amt-dTx_amt;
   	set_zd_double("0400",restAmt);			/*总行剩余额=发售总额-认购总额*/
   	
   	/*检查该产品是否配额*/
   	if(sFn_parm.quota_flag[0] == 'Y')
		{
        /*检查本机构理财产品是否能认购*/
        	ret = Fn_br_quota_parm_Sel(g_pub_tx.reply,&sFn_br_qp,"prdt_code='%s' and br_no='%s'",\
           	sFn_parm.prdt_code,g_pub_tx.tx_br_no);
       		if(ret != 0 && ret != 100)
        	{
           		vtcp_log("查询理财产品配额参数表失败iRet[%d]",ret);
            		strcpy(g_pub_tx.reply,"D327");
           		goto ErrExit;
        	}
        	else if(ret == 100)/*这个机构没配额，取全行*/
        	{
           		if(sFn_parm.quota_flag[1] == 'Y')
            		{
               			 if(sFn_parm.stat[0] == '1')/*统计总行已经赎回的总额 只有状态是1的时候才有赎回和申购*/
               			 {
                    			/*赎回的金额汇总*/
                    			ret = sql_sum_double("fn_reg","redempt_amt",&dRedempt_amt,"prdt_code='%s' and sts='%s'",\
                       			sFn_parm.prdt_code,sFn_parm.stat);
                    			if(ret)
                    			{
                        			vtcp_log("统计fn_reg错误!");
                        			strcpy(g_pub_tx.reply, "D331");
                        			goto ErrExit;
                    			}
            				/*申购中的金额汇总*/        		
                    			ret = sql_sum_double("fn_reg","tx_amt",&dBuying_amt,"prdt_code='%s' and sts='4'",\
                  			sFn_parm.prdt_code);
                    			if(ret)
                    			{
                        			vtcp_log("统计fn_reg错误!");
                        			strcpy(g_pub_tx.reply, "D331");
                        			goto ErrExit;
                    			}
                		}
				vtcp_log("dRedempt_amt[%lf]",dRedempt_amt);	/*赎回的金额汇总*/
				vtcp_log("dBuying_amt[%lf]",dBuying_amt);	/*申购中的金额汇总*/ 
				/* 机构剩余额=发售金额-已经认购的总额-申购中的金额汇总+赎回的金额汇总*/
				set_zd_double("0320",sFn_parm.sale_amt - (dTx_amt - dRedempt_amt) - dBuying_amt);
          		}
           	 	else
           	 	{
                		set_zd_double("0320",0.00);
           		 }
       		 }
       		 else
        	{
           	 set_zd_double("0320",sFn_br_qp.available_amt);
        	}
		}
		else/*产品不设置配额，取全行的*/
		{
			/*统计总行已经赎回的总额 只有状态是1的时候才有赎回和申购*/
			if(sFn_parm.stat[0] == '1')
				{
					ret = sql_sum_double("fn_reg","redempt_amt",&dRedempt_amt,"prdt_code='%s' and sts='%s'",\
					sFn_parm.prdt_code,sFn_parm.stat);
					if(ret)
					{
							vtcp_log("统计fn_reg错误!");
							strcpy(g_pub_tx.reply, "D331");
							goto ErrExit;
					}
					ret = sql_sum_double("fn_reg","tx_amt",&dBuying_amt,"prdt_code='%s' and sts='4'",sFn_parm.prdt_code);
 					if(ret)
					{
							vtcp_log("统计fn_reg错误!");
							strcpy(g_pub_tx.reply, "D331");
							goto ErrExit;
					}
				}
				vtcp_log("dRedempt_amt[%lf]",dRedempt_amt);	/*赎回的金额汇总*/
				vtcp_log("dBuying_amt[%lf]",dBuying_amt);	/*申购中的金额汇总*/
				/* 机构剩余额=发售金额-已经认购的总额-申购中的金额汇总+赎回的金额汇总*/
				set_zd_double("0430",sFn_parm.sale_amt - (dTx_amt - dRedempt_amt) - dBuying_amt);
		}
	set_zd_double("0300",sFn_parm.add_amt);		/*递增金额*/
	set_zd_double("0410",sFn_parm.per_min_amt);	/*个人起存金额*/
	set_zd_double("0420",sFn_parm.com_min_amt);	/*单位起存金额*/
	set_zd_data("0930",sFn_parm.cancel_flag);	/*是否撤销*/
	set_zd_data("0710",sFn_parm.float_rate_flag);		/*是否有浮动利率*/
	set_zd_double("0970",sFn_parm.manage_rate);	/*管理费率*/
	/*add end*/
	
   
OkExit:
	strcpy(g_pub_tx.reply,"0000");			/*通用记账结构.响应吗*/
	vtcp_log("success !!: reply is[%s]", g_pub_tx.reply);
	set_zd_data("0120","0000");	
	return 0;
ErrExit:
	vtcp_log("fail !!: reply is[%s]", g_pub_tx.reply);
	set_zd_data("0120",g_pub_tx.reply);	
	return 1;
}