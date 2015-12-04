/*************************************************
* 文 件 名: spC117.c
* 功能描述：理财产品录入、维护
* 作    者: wangjinlong
* 完成日期：2014/3/04 9:47:30
* 修改记录：
*     1. 日    期:
*        修 改 人:
*        修改内容:
*     2.

insert into tx_def values('4700','理财产品录入/维护','10000000000000010000001000000000000000000001100100000000000001100011100000000000000000000000000000000000000000000000000000000000','1','4','2');
insert into tx_flow_def values('4700','0','4700','#$');
insert into tx_sub_def values('4700','理财产品录入/维护','spC117','0','0');

*************************************************/
#include "public.h"
#define EXTERN
#include "fn_parm_c.h"
#include "fn_flt_radio_parm_c.h"
#include "fn_ac_rel_c.h"
static  struct  fn_parm_c  fn_parm;
int spC117()
{
	int iRet  = 0;
  double sRate = 0;
  char sTmp_rate[10];
  char Flt_radio2[11]; /*单份金额浮动比率——20140422wjl*/

  char flag[2];/*标志位判断是否更新维护*/
	struct fn_parm_c sFn_parm;
	struct fn_parm_c s_Fn_parm;
	struct fn_ac_rel_c sFn_ac_rel;
	struct fn_ac_rel_c s_Fn_ac_rel;
	struct fn_flt_radio_parm_c sFn_flt_radio_parm;
	struct fn_flt_radio_parm_c s_Fn_flt_radio_parm;


	pub_base_sysinit();
	memset(&sFn_parm,0x00,sizeof(struct fn_parm_c));
	memset(&s_Fn_parm,0x00,sizeof(struct fn_parm_c));
	memset(sTmp_rate,0x00,sizeof(sTmp_rate));
	memset(&sFn_ac_rel,0x00,sizeof(struct fn_ac_rel_c));
	memset(&s_Fn_ac_rel,0x00,sizeof(struct fn_ac_rel_c));
	memset(&sFn_flt_radio_parm,0x00,sizeof(struct fn_flt_radio_parm_c));
	memset(&s_Fn_flt_radio_parm,0x00,sizeof(struct fn_flt_radio_parm_c));
	memset( flag        ,'\0'  , sizeof(flag));
	memset(Flt_radio2,0x00,sizeof(Flt_radio2));
	/*取值*/
    get_zd_data("0630",sFn_parm.prdt_code);
    get_zd_data("0250",sFn_parm.title);
    get_zd_data("0670",sFn_parm.prdt_type);
    get_zd_data("0680",sFn_parm.intst_type);
    get_zd_long("0440",&sFn_parm.beg_date);
    get_zd_long("0450",&sFn_parm.end_date);
    get_zd_long("0460",&sFn_parm.ic_date);
    get_zd_data("0310",sFn_parm.com_ac_no);
    get_zd_data("0320",sFn_parm.per_ac_no);
    get_zd_long("0470",&sFn_parm.mtr_date);
    get_zd_data("0700",sFn_parm.cif_type);/*客户类型：0都可，1个人，2对公*/
    get_zd_data("0690",sFn_parm.risk_lvl);
    get_zd_data("0720",sFn_parm.invest_type);
    get_zd_data("0210",sFn_parm.cur_no);
    get_zd_data("0220",sFn_parm.redempt);/*是否赎回LJ*/
    /*get_zd_long("0540",&sFn_parm.fre_date);开放天数LJ*/
    /*get_zd_long("0550",&sFn_parm.fre_term);开放周期LJ*/
    /*get_zd_data("0980",sFn_parm.fre_type);开放周期类型LJ*/
    /*get_zd_long("0530",&sFn_parm.fre_num);开放周期期数LJ*/
    get_zd_data("0730",sTmp_rate);
    sRate=atoi(sTmp_rate);
    sFn_parm.rate=sRate/1000000;
   vtcp_log("%d%s,333333333333sTmp_rate[%s],sRate=[%f],sFn_parm.rate=[%f]", __LINE__, __FILE__,sTmp_rate,sRate,sFn_parm.rate);
    get_zd_double("0390",&sFn_parm.sale_amt);
    /*get_zd_double("0400",&sFn_parm.unit_amt);*/
    get_zd_double("0410",&sFn_parm.per_min_amt);
    get_zd_double("0420",&sFn_parm.com_min_amt);
    get_zd_data("0930",sFn_parm.cancel_flag);
    get_zd_double("1001",&sFn_parm.add_amt);
    get_zd_double("1002",&sFn_parm.min_amt);
    
    get_zd_data("0660",flag);
    sFn_parm.add_amt=sFn_parm.add_amt/100;
    sFn_parm.min_amt=sFn_parm.min_amt/100;
  vtcp_log("%d%s,sFn_parm.min_amt[%lf]sFn_parm.add_amt[%lf]", __LINE__, __FILE__, sFn_parm.min_amt, sFn_parm.add_amt);
    strcpy(sFn_parm.term_type,"D");
    sFn_parm.return_date=sFn_parm.mtr_date;
    sFn_parm.term=pub_base_CalTrueDays(sFn_parm.ic_date,sFn_parm.mtr_date);
    if(sFn_parm.term == 366)
    sFn_parm.term = 365;
    vtcp_log("%d%s,44444sFn_parm.term[%ld]", __LINE__, __FILE__,  sFn_parm.term);
    strcpy(sFn_parm.chnl_type,"****");
    sFn_parm.fct_rate=0.00;
    get_zd_data("0710",sFn_parm.float_rate_flag);
    sFn_parm.capital_percent=0;
    sFn_parm.chrg_rate=0.00;
    sFn_parm.trust_rate=0.00;
    sFn_parm.unit_amt=0.00;
    sFn_parm.manage_rate=0.00;
    sFn_parm.profit_rate=0.00;
    /*strcpy(sFn_parm.redempt,"N");LJ*/
    sFn_parm.redempt_chrg=0.00;
    strcpy(sFn_parm.quota_flag,"YY");
    strcpy(sFn_parm.stat,"0");
    pub_base_strpack(flag);
    get_zd_data("0640",Flt_radio2);
    
     sFn_flt_radio_parm.flt_radio2 = atof(Flt_radio2) / 1000000;
   
     vtcp_log("%d%s,3334444433Flt_radio2[%s,sFn_flt_radio_parm.flt_radio2=[%f]", __LINE__, __FILE__,Flt_radio2,sFn_flt_radio_parm.flt_radio2);
    get_zd_double("0400",&sFn_flt_radio_parm.buy_amt)	;
    get_zd_double("0430",&sFn_flt_radio_parm.buy_amt1);
    sFn_flt_radio_parm.buy_amt1=sFn_flt_radio_parm.buy_amt1/100;
    get_zd_double("0340",&sFn_flt_radio_parm.flt_radio21);
    sFn_flt_radio_parm.flt_radio21= sFn_flt_radio_parm.flt_radio21/100;
    get_zd_double("1003",&sFn_flt_radio_parm.buy_amt2);
    sFn_flt_radio_parm.buy_amt2=sFn_flt_radio_parm.buy_amt2/100;
    get_zd_double("0350",&sFn_flt_radio_parm.flt_radio22);
    sFn_flt_radio_parm.flt_radio22= sFn_flt_radio_parm.flt_radio22/100;
    get_zd_double("1004",&sFn_flt_radio_parm.buy_amt3);
    sFn_flt_radio_parm.buy_amt3=sFn_flt_radio_parm.buy_amt3/100;
    get_zd_double("0360",&sFn_flt_radio_parm.flt_radio23);
    sFn_flt_radio_parm.flt_radio23= sFn_flt_radio_parm.flt_radio23/100;
    get_zd_double("100E",&sFn_flt_radio_parm.buy_amt4);
    sFn_flt_radio_parm.buy_amt4=sFn_flt_radio_parm.buy_amt4/100;
    get_zd_double("0480",&sFn_flt_radio_parm.flt_radio24);
    sFn_flt_radio_parm.flt_radio24= sFn_flt_radio_parm.flt_radio24/100;
    
    /*add by lwb 20140820 增加对机构是否为清算中心的判断*/
    if(strcmp(g_pub_tx.tx_br_no,QS_BR_NO))  
    {
    	sprintf(acErrMsg,"非清算中心不允许做此交易！");
    	WRITEMSG
    	strcpy(g_pub_tx.reply,"P047");
    	goto ErrExit;
    }
    
    
    /*插入产品信息*/
    if(flag[0]=='1')
    {
		iRet = Fn_parm_Dec_Upd(g_pub_tx.reply,"prdt_code='%s'",sFn_parm.prdt_code);
		if(iRet)
		{sprintf(acErrMsg,"更新理财产品参数表错误![%s]",g_pub_tx.reply);
		WRITEMSG
			strcpy(g_pub_tx.reply,"D334");
			goto ErrExit;
		}
		iRet = Fn_parm_Fet_Upd(&s_Fn_parm,g_pub_tx.reply);
		 if(iRet)
		{sprintf(acErrMsg,"查询理财产品参数表错误![%s]",g_pub_tx.reply);
		WRITEMSG
			strcpy(g_pub_tx.reply,"D334");
			goto ErrExit;
		}
		strcpy(s_Fn_parm.prdt_code,sFn_parm.prdt_code);
		strcpy(s_Fn_parm.title,sFn_parm.title);
		strcpy(s_Fn_parm.prdt_type,sFn_parm.prdt_type);
		strcpy(s_Fn_parm.intst_type,sFn_parm.intst_type);
		s_Fn_parm.beg_date=sFn_parm.beg_date;
		s_Fn_parm.end_date=sFn_parm.end_date;
		s_Fn_parm.ic_date=sFn_parm.ic_date;
		s_Fn_parm.mtr_date=sFn_parm.mtr_date;
		strcpy(s_Fn_parm.com_ac_no,sFn_parm.com_ac_no);
		strcpy(s_Fn_parm.per_ac_no,sFn_parm.per_ac_no);
		strcpy(s_Fn_parm.cif_type,sFn_parm.cif_type);
		strcpy(s_Fn_parm.risk_lvl,sFn_parm.risk_lvl);
		strcpy(s_Fn_parm.invest_type,sFn_parm.invest_type);
		strcpy(s_Fn_parm.redempt,sFn_parm.redempt);/*LJ*/
		/*s_Fn_parm.fre_date=sFn_parm.fre_date;LJ*/
		/*s_Fn_parm.fre_term=sFn_parm.fre_term;LJ*/
		/*strcpy(s_Fn_parm.fre_type,sFn_parm.fre_type);LJ*/
		/*s_Fn_parm.fre_num=sFn_parm.fre_num;LJ*/
		s_Fn_parm.rate=sFn_parm.rate;
		s_Fn_parm.sale_amt=sFn_parm.sale_amt;
		s_Fn_parm.per_min_amt=sFn_parm.per_min_amt;
		s_Fn_parm.com_min_amt=sFn_parm.com_min_amt;
		strcpy(s_Fn_parm.cancel_flag,sFn_parm.cancel_flag);
		s_Fn_parm.add_amt=sFn_parm.add_amt;
		s_Fn_parm.min_amt=sFn_parm.min_amt;
		s_Fn_parm.term = sFn_parm.term;
		strcpy(s_Fn_parm.float_rate_flag,sFn_parm.float_rate_flag);  /*add by lwb 20150413*/
		
   iRet = Fn_parm_Upd_Upd(s_Fn_parm,g_pub_tx.reply);
			if(iRet)
			{
				sprintf(acErrMsg,"更新理财产品参数表簿错误![%s]",g_pub_tx.reply);
		WRITEMSG
				strcpy(g_pub_tx.reply,"D334");
				goto ErrExit;
			}
		Fn_parm_Clo_Upd();
		iRet = Fn_flt_radio_parm_Dec_Upd(g_pub_tx.reply,"prdt_code='%s'",sFn_parm.prdt_code);
		if(iRet)
		{sprintf(acErrMsg,"更新浮动参数表错误![%s]",g_pub_tx.reply);
		WRITEMSG
			strcpy(g_pub_tx.reply,"D334");
			goto ErrExit;
		}
		iRet = Fn_flt_radio_parm_Fet_Upd(&s_Fn_flt_radio_parm,g_pub_tx.reply);
		 if(iRet)
		{sprintf(acErrMsg,"查询理财产品参数表错误![%s]",g_pub_tx.reply);
		WRITEMSG
			strcpy(g_pub_tx.reply,"D334");
			goto ErrExit;
		}
		get_zd_long("0050",&sFn_flt_radio_parm.upd_date);
		strcpy(sFn_flt_radio_parm.upd_tel,g_pub_tx.tel);
		s_Fn_flt_radio_parm.flt_radio2=sFn_flt_radio_parm.flt_radio2;
		s_Fn_flt_radio_parm.buy_amt=sFn_flt_radio_parm.buy_amt;
		 /*新加4个梯度比率——20140630wjl*/
		s_Fn_flt_radio_parm.buy_amt1=sFn_flt_radio_parm.buy_amt1;
		s_Fn_flt_radio_parm.flt_radio21=sFn_flt_radio_parm.flt_radio21;
		s_Fn_flt_radio_parm.buy_amt2=sFn_flt_radio_parm.buy_amt2;
		s_Fn_flt_radio_parm.flt_radio22=sFn_flt_radio_parm.flt_radio22;
		s_Fn_flt_radio_parm.buy_amt3=sFn_flt_radio_parm.buy_amt3;
		s_Fn_flt_radio_parm.flt_radio23=sFn_flt_radio_parm.flt_radio23;
		s_Fn_flt_radio_parm.buy_amt4=sFn_flt_radio_parm.buy_amt4;
		s_Fn_flt_radio_parm.flt_radio24=sFn_flt_radio_parm.flt_radio24;
		
		iRet = Fn_flt_radio_parm_Upd_Upd(s_Fn_flt_radio_parm,g_pub_tx.reply);
			if(iRet)
			{
				sprintf(acErrMsg,"更新理财产品参数表簿错误![%s]",g_pub_tx.reply);
		WRITEMSG
				strcpy(g_pub_tx.reply,"D334");
				goto ErrExit;
			}
		Fn_flt_radio_parm_Clo_Upd();
		
		}
		else
    {
			iRet = Fn_parm_Ins(sFn_parm,g_pub_tx.reply);
			if(iRet)
			{
				Fn_parm_Debug(&sFn_parm);
		sprintf(acErrMsg,"插入理财产品错误![%s]",g_pub_tx.reply);
		WRITEMSG
				strcpy(g_pub_tx.reply,"S094");
				goto ErrExit;
			}
			/*插入理财配置表信息*/
		get_zd_data("0330",sFn_ac_rel.qs_ac_no);
		strcpy(sFn_ac_rel.prdt_code,sFn_parm.prdt_code);
		strcpy(sFn_ac_rel.wt_ac_no,"");
		strcpy(sFn_ac_rel.sy_ac_no,"");
		iRet = Fn_ac_rel_Ins(sFn_ac_rel,g_pub_tx.reply);
			if(iRet)
			{
				Fn_ac_rel_Debug(&sFn_ac_rel);
		sprintf(acErrMsg,"插入理财配置表信息![%s]",g_pub_tx.reply);
		WRITEMSG
				strcpy(g_pub_tx.reply,"S094");
				goto ErrExit;
			}
		strcpy(sFn_flt_radio_parm.prdt_code,sFn_parm.prdt_code);
		strcpy(sFn_flt_radio_parm.ctl_flag,"0");
		strcpy(sFn_flt_radio_parm.cif_lvl,"4");
		
		sFn_flt_radio_parm.flt_radio1=0.00;
		sFn_flt_radio_parm.total_amt=0.00;
		sFn_flt_radio_parm.flt_radio3=0.00;
		get_zd_long("0050",&sFn_flt_radio_parm.reg_date);
		strcpy(sFn_flt_radio_parm.reg_tel,g_pub_tx.tel);
		sFn_flt_radio_parm.upd_date='';
		strcpy(sFn_flt_radio_parm.upd_tel,"");
		iRet = Fn_flt_radio_parm_Ins(sFn_flt_radio_parm,g_pub_tx.reply);
					if(iRet)
			{
				Fn_flt_radio_parm_Debug(&sFn_ac_rel);
		sprintf(acErrMsg,"插入理财配置表信息![%s]",g_pub_tx.reply);
		WRITEMSG
				strcpy(g_pub_tx.reply,"S094");
				goto ErrExit;
			}
		}		
			
	
OkExit:
	strcpy(g_pub_tx.reply, "0000");
	sprintf(acErrMsg,"理财产品录入成功![%s]",g_pub_tx.reply);
 WRITEMSG
	set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 0;
ErrExit:
sprintf(acErrMsg,"理财产品录入失败![%s]",g_pub_tx.reply);
WRITEMSG
	set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 1;
}
