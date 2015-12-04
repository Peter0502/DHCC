/*******************************************************************************
*** 版 本 号:  V1.0
*** 程序作者: shangyongchao
*** 日    期: 2013/5/16 11:30:53
*** 所属模块: 理财产品
*** 程序名称: spD088
*** 程序作用: 完成理财产品认购、撤销和赎回
*** 使用注意:
*** 修改记录:
***     修改作者:
***     修改时间:
***     修改内容:
*******************************************************************************/
#include "string.h"
#define EXTERN
#include "public.h"
#include "fn_parm_c.h"
#include "fn_reg_c.h"
#include "fn_reg_hst_c.h"
#include "cif_ass_reg_c.h"
#include "mdm_ac_rel_c.h"
#include "dd_mst_c.h"
#include "dd_parm_c.h"
#include "fn_br_quota_parm_c.h"
#include "fn_flt_radio_parm_c.h"
#include "cif_basic_inf_c.h"
#include "cif_id_code_rel_c.h"
#include "fn_br_profit_c.h"
#include "redempt_rate_c.h"
#include "fn_ac_rel_c.h"
#include "com_branch_c.h"
#include "com_sys_parm_c.h"
#include "com_parm_c.h"
#define L365 365

int spD088()
{
    int iRet = 0;
    char cOper_flag[2];/*1认购 2撤销 3赎回（LJ20151105）*/
    memset(cOper_flag,0x00,sizeof(cOper_flag));
    /** 初始化全局变量 **/
    pub_base_sysinit();
    get_zd_data("0670",cOper_flag);
    /*根据cOper_flag进行不同处理*/
    if(cOper_flag[0] == '1')
    {
        /*认购*/
        iRet = buy_func();
        if(iRet)
        {
            vtcp_log("认购失败了");
            goto ErrExit;
        }
        
    }
    else if(cOper_flag[0] == '2')
    {
        /*撤销*/
        iRet = cancel_func();
        if(iRet)
        {
            vtcp_log("撤销失败了");
            goto ErrExit;
        }
    }
    /*LJ20151105*/
    else if(cOper_flag[0] == '3')
    {
        /*赎回*/
        iRet = redempt_func();
        if(iRet)
        {
            vtcp_log("赎回失败了");
            goto ErrExit;
        }
    }
    else
    {
        vtcp_log("操作类型错误 [%s]",cOper_flag);
        strcpy(g_pub_tx.reply, "S083");
        goto ErrExit;
    }

OkExit:
    strcpy(g_pub_tx.reply, "0000");
    vtcp_log("Before OK return: reply is[%s]", g_pub_tx.reply);
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 0;
ErrExit:
    vtcp_log( "Before return: reply is[%s]", g_pub_tx.reply);
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 1;
}

/*认购主函数*/
int buy_func()
{
    int iRet = 0;
    int iAc_seqn = 0;/*账户序号*/
    long lDays = 0;
    char cPrdt_code[21];/*产品代码*/
    char cCif_type[2];/*客户类型1个人2对公*/
    char cAgent_id_type[2];/*对公户的代理人证件类型*/
    char cAgent_id[21];/*对公户的代理人证件*/
    char cAgent_name[61];/*对公户的代理人姓名*/
    char cAc_no[25];/*账号*/
    char cPact_no[21];/*合同号*/
    char cPact_no_tmp[21];
    char cMang[7];/*客户经理号*/
    char cCif_lvl[2];
    char cCpmc[60]; /*产品名称*/  
    double dAvbal = 0.00;/*账户可用余额*/
    double dQuota_amt = 0.00;/*该机构 剩余配额*/
    double dFlt_radio = 0.00;/*收益上浮比例*/
    double dTx_amt = 0.00;/*认购金额*/
    
    struct fn_parm_c sFn_parm;
    struct fn_reg_c sFn_reg,sFn_reg_tmp;
    struct fn_reg_hst_c sFn_reg_hst;
    struct mdm_ac_rel_c sMdm_ac_rel;
    struct dd_mst_c sDd_mst;
    struct dd_parm_c sDd_parm;
    struct fn_br_quota_parm_c sFn_br_qp;
    
    memset(cPrdt_code   ,0x00,sizeof(cPrdt_code));
    memset(cCpmc		,0x00,sizeof(cCpmc)); 
    memset(cCif_type    ,0x00,sizeof(cCif_type));
    memset(cAgent_id    ,0x00,sizeof(cAgent_id));
    memset(cAgent_id_type    ,0x00,sizeof(cAgent_id_type));
    memset(cAgent_name  ,0x00,sizeof(cAgent_name));
    memset(cAc_no       ,0x00,sizeof(cAc_no));
    memset(cPact_no     ,0x00,sizeof(cPact_no));
    memset(cPact_no_tmp     ,0x00,sizeof(cPact_no_tmp));
    memset(cMang        ,0x00,sizeof(cMang));
    memset(cCif_lvl,0x00,sizeof(cCif_lvl));
    memset(&sFn_parm        ,0x00,sizeof(sFn_parm));
    memset(&sFn_reg        ,0x00,sizeof(sFn_reg));
    memset(&sFn_reg_tmp        ,0x00,sizeof(sFn_reg_tmp));
    memset(&sFn_reg_hst        ,0x00,sizeof(sFn_reg_hst));
    memset(&sMdm_ac_rel        ,0x00,sizeof(sMdm_ac_rel));
    memset(&sDd_parm        ,0x00,sizeof(sDd_parm));
    memset(&sDd_mst        ,0x00,sizeof(sDd_mst));
    memset(&sFn_br_qp,0x00,sizeof(sFn_br_qp));
    
    get_zd_int("0340",&iAc_seqn);
    get_zd_double("0390",&dTx_amt);
    get_zd_data("0300",cAc_no);
    get_zd_data("0690",cAgent_id_type);
    get_zd_data("0620",cAgent_id);
    get_zd_data("0250",cAgent_name);
    get_zd_data("0630",cPrdt_code);
    get_zd_data("0260",cCpmc);      /*前台获取产品名称*/
    vtcp_log(" 前台传送产品名称  【 %s 】",cCpmc);
    get_zd_data("0800",cPact_no_tmp);  /**由86域暂改为80域**/
    get_zd_data("0920",cMang);
    /*先检查账号信息*/
    sprintf(cPact_no,"%012s",cPact_no_tmp);  /*add by lwb 20141030 把协议号转化成12位*/
    pub_base_old_acno(cAc_no);
    iRet = Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdm_ac_rel, "ac_no='%s'", cAc_no);
    if (iRet == 100)
    {
        vtcp_log( "不存在该账户!!ac_id=[%ld]", sMdm_ac_rel.ac_id);
        strcpy(g_pub_tx.reply, "B117");
        set_zd_data("0130","不存在该账户");
        goto ErrExit;
    }
    else if (iRet)
    {
        vtcp_log( "读取介质账号对照表失败[%d]", iRet);
        strcpy(g_pub_tx.reply, "C114 ");
        goto ErrExit;
    }
    /*检查介质状态*/
    iRet = check_note_sts(sMdm_ac_rel.note_sts);
    if(iRet)
    {
        vtcp_log("介质状态不正常");
        goto ErrExit;
    }
    /*验证客户类型*/
    iRet = get_cif_type(sMdm_ac_rel,cCif_type,cCif_lvl);
    if(iRet)
    {
        vtcp_log("验证客户类型失败");
        goto ErrExit;
    }
    vtcp_log("客户级别[%s]",cCif_lvl);
    /*检查理财产品*/
    vtcp_log("[%s],[%d]理财产品编号cPrdt_code[%s]",__FILE__,__LINE__,cPrdt_code);
    iRet = Fn_parm_Sel(g_pub_tx.reply,&sFn_parm,"prdt_code='%s'",cPrdt_code);
    if(iRet != 0 && iRet != 100)
    {
        vtcp_log("查询理财产品参数表失败iRet[%d]",iRet);
        strcpy(g_pub_tx.reply,"D324");
        goto ErrExit;
    }
    else if(iRet == 100)
    {
        vtcp_log("没有该理财产品");
        strcpy(g_pub_tx.reply,"D104");
        set_zd_data("0130","没有该理财产品");
        goto ErrExit;    
    }
    vtcp_log("fn_parm.stat = [ %s ] ",sFn_parm.stat);
    /*理财产品状态*/
    if( sFn_parm.stat[0] != '0' )
    {
        vtcp_log("该产品非认购状态[%s] ",sFn_parm.stat);
        strcpy(g_pub_tx.reply,"C334");
        set_zd_data("0130","该产品非认购状态");
        goto ErrExit;
    }
    /*检查是否在募集期*/
    if(sFn_parm.beg_date > g_pub_tx.tx_date || sFn_parm.end_date < g_pub_tx.tx_date)
    {
        vtcp_log("该产品非认购状态[%s] ",sFn_parm.stat);
        strcpy(g_pub_tx.reply,"C334");
        set_zd_data("0130","该产品非认购状态");
        goto ErrExit;
    }
    /*如果是柜面 判断前台传的产品名称和后台的产品代码查询到的产品是否一致（网银和手银暂时不判断）*/
    vtcp_log(" 后台查询产品名称  【 %s 】",sFn_parm.title);
    if(!strcmp(g_pub_tx.tty,"netb"))   /*如果是网银*/
    {
    		   	
    }
    else if(!strcmp(g_pub_tx.tty,"mobb"))/*如果是手机银行 */
    {
    	   	
    }
    else{
    	if(strcmp(sFn_parm.title,cCpmc) != 0)
	    {
	    	vtcp_log(" 产品代码传送错误  【 %s 】",sFn_parm.prdt_code);
	    	strcpy(g_pub_tx.reply,"P146");
	    	set_zd_data("0130","产品代码传输出错 请重做！ ");
	    	goto ErrExit;
	    }	
    }
    
    /*认购渠道*/
    /*if(strcmp(sFn_parm.chnl_type,g_pub_rule.chnl_type) != 0 && strcmp(sFn_parm.chnl_type,"****") != 0)
    {
        vtcp_log("该渠道不能认购[%s] ",sFn_parm.chnl_type);
        strcpy(g_pub_tx.reply,"P470");
        goto ErrExit;
    }*/
    if(strcmp(sFn_parm.cif_type,"0") != 0 && strcmp(sFn_parm.cif_type,cCif_type) !=0)
    {
        vtcp_log("客户类型不符[%s] ",sFn_parm.cif_type);
        strcpy(g_pub_tx.reply,"D112");
        set_zd_data("0130","客户类型不符");
        goto ErrExit;
    }
	iRet=Fn_reg_Sel(g_pub_tx.reply,&sFn_reg_tmp,"pact_no = '%s'",cPact_no);
	if(iRet == 0)
	{
		sprintf(acErrMsg,"此协议号已存在 [%s]",cPact_no);
		strcpy(g_pub_tx.reply,"H034");
		set_zd_data("0130",acErrMsg);		
		WRITEMSG	
		goto ErrExit;
	}
	else if(iRet != 100)
	{
		vtcp_log("查询fn_reg失败");
		strcpy(g_pub_tx.reply,"D331");
		goto ErrExit;
	}


    /*获取该机构配额*/
    iRet = get_br_quota(sFn_parm,&dQuota_amt);
    if(iRet)
    {
        vtcp_log("得到配额错误iRet[%d]",iRet);
        goto ErrExit;
    }
    /*认购金额是否满足配额*/
    /*sFn_reg.count = dTx_amt / sFn_parm.unit_amt;*/
    
    if(pub_base_CompDblVal(sFn_reg.tx_amt,dQuota_amt) > 0)
    {
        vtcp_log("没有限额了 tx_amt[%lf] quot_amt[%lf]",sFn_reg.tx_amt,dQuota_amt);
        strcpy(g_pub_tx.reply,"D349");
        set_zd_data("0130","没有限额了");
        goto ErrExit;
    }
     sFn_reg.tx_amt  = dTx_amt;
    if(sFn_parm.prdt_type[0] == '1') /*封闭式产品不动 按之前的判断*/
    {
	    /*是否满足最低认购额*//*前台已经限制购买额度 20140411_wjl*/
	    if(cCif_type[0] == '1' && pub_base_CompDblVal(sFn_reg.tx_amt,sFn_parm.per_min_amt) < 0)
	    {
	        vtcp_log("低于最低认购金额[%lf][%lf]",sFn_reg.tx_amt,sFn_parm.per_min_amt);
	        strcpy(g_pub_tx.reply,"D338");
	        goto ErrExit;
	    }
	    else if(cCif_type[0] == '2' && pub_base_CompDblVal(sFn_reg.tx_amt,sFn_parm.com_min_amt) <0 )
	    {
	        vtcp_log("低于最低认购金额[%lf][%lf]",sFn_reg.tx_amt,sFn_parm.com_min_amt);
	        strcpy(g_pub_tx.reply,"D338");
	        goto ErrExit;
	    }
	}
	if(sFn_parm.prdt_type[0] == '2') /*开放式产品对认购金额判断如果已经认购上期扣款则不判断最低认购金额*/
	{
		double sTmt = 0.00;
		
		iRet = Fn_reg_Dec_Sel(g_pub_tx.reply,"ac_no = '%s' and prdt_code = '%s' and sts = '1'",cAc_no,cPrdt_code);
	    if(iRet)
	    {
	        vtcp_log("定义理财登记簿游标错误");
	        strcpy(g_pub_tx.reply,"D101");
	        goto ErrExit;
	    }
	    
	    while(1)
	    {      
	        iRet = Fn_reg_Fet_Sel(&sFn_reg,g_pub_tx.reply);
	        if(iRet != 100 && iRet != 0)
	        {
	            vtcp_log("取理财登记簿游标错误");
	            strcpy(g_pub_tx.reply,"D330");
	            goto ErrExit;
	        }
	        if(iRet == 0)
	        {       
	        	sTmt = sFn_reg.tx_amt - sFn_reg.redempt_amt;
	        }
	        else
	        	break;
	    }
	    Fn_reg_Clo_Sel();
	    sFn_reg.tx_amt  = dTx_amt;
   		vtcp_log("认购金额是 [%.2lf] ",sFn_reg.tx_amt);
   		sFn_reg.count = sql_max_int("fn_reg","count","ac_no = '%s' and prdt_code = '%s'",cAc_no,cPrdt_code); 
   		vtcp_log("sFn_reg.count [%d] ",sFn_reg.count);
	    if(sTmt == 0)
	    {
	    	if(sFn_reg.count == 0)
	    	{
	    		sFn_reg.count = 1;		
	    	}
	    	else 
	    		++sFn_reg.count;
	    	/*是否满足最低认购额*//*前台已经限制购买额度 20140411_wjl*/
		    if(cCif_type[0] == '1' && pub_base_CompDblVal(sFn_reg.tx_amt,sFn_parm.per_min_amt) < 0)
		    {
		        vtcp_log("低于最低认购金额[%lf][%lf]",sFn_reg.tx_amt,sFn_parm.per_min_amt);
		        strcpy(g_pub_tx.reply,"D338");
		        goto ErrExit;
		    }
		    else if(cCif_type[0] == '2' && pub_base_CompDblVal(sFn_reg.tx_amt,sFn_parm.com_min_amt) <0 )
		    {
		        vtcp_log("低于最低认购金额[%lf][%lf]",sFn_reg.tx_amt,sFn_parm.com_min_amt);
		        strcpy(g_pub_tx.reply,"D338");
		        goto ErrExit;
		    }
	    }
	    else
	    	++sFn_reg.count;
	    sFn_reg.tx_amt  = dTx_amt;
	    
	    	
	}
	
    vtcp_log("认购金额第二次是 [%.2lf] ",sFn_reg.tx_amt);
    /*获取收益浮动比例*/
    if(sFn_parm.float_rate_flag[0] == 'Y')
    {
        iRet = get_flt_radio(sFn_parm.prdt_code,sFn_reg.tx_amt,cCif_lvl,sMdm_ac_rel.ac_id,&dFlt_radio);
        if(iRet)
        {
            vtcp_log("获取收益上浮比例错误");
            goto ErrExit;
        }
        vtcp_log("收益上浮比例为[%f]",dFlt_radio);
    }
    else
    {
        dFlt_radio = 0.000000;
    }
    sFn_reg.flt_radio = dFlt_radio;
    /*计算预期收益均按天数算*/
    lDays = pub_base_CalTrueDays(sFn_parm.ic_date,sFn_parm.mtr_date);/*mod by lwb  去掉+1*/
    /*add by lwb 晋中商行要求一年为365天*/ 
    if(lDays== 366)
    {
     	lDays = 365;
     }
    vtcp_log("mtr_date[%ld] - ic_date[%ld] = lDays[%ld]",sFn_parm.ic_date,sFn_parm.mtr_date,lDays);
    /*sFn_reg.intst=sFn_reg.tx_amt * lDays * sFn_parm.rate * (1+ sFn_reg.flt_radio)/L360/L100;*/
		sFn_reg.intst=sFn_reg.tx_amt * lDays * sFn_parm.rate * (1+ sFn_reg.flt_radio)/L365/L100;
     vtcp_log("收益为[%lf]",sFn_reg.intst);
    sFn_reg.profit_amt= sFn_reg.tx_amt * lDays * sFn_parm.profit_rate/L365/L100;
    /*四舍五入*/
    sFn_reg.intst = pub_base_PubDround_1(sFn_reg.intst);
    set_zd_double("1001",sFn_reg.intst);
    set_zd_double("0840",sFn_reg.flt_radio);
    sFn_reg.profit_amt = pub_base_PubDround_1(sFn_reg.profit_amt);
    /*登记fn_reg,fn_reg_hst*/
    vtcp_log("LJg_pub_tx.tty=[%s]",g_pub_tx.tty);
    if(!strcmp(g_pub_tx.tty,"netb"))   /*如果是网银的话 交易行记为开户行   add by lwb 20141114*/
    {
    		strcpy(g_pub_tx.tx_br_no,sMdm_ac_rel.opn_br_no);
    		strcpy(sFn_reg.chnl_type,"0400");   	
    }
    else if(!strcmp(g_pub_tx.tty,"mobb"))/*如果是手机银行的话 交易行记为开户行   LJ 20151116*/
    {
    	strcpy(g_pub_tx.tx_br_no,sMdm_ac_rel.opn_br_no);
    	strcpy(sFn_reg.chnl_type,"0300");   	
    }
    else{
    	 strcpy(sFn_reg.chnl_type,"0100");	
    }
    sFn_reg.tx_date = g_pub_tx.tx_date;
    sFn_reg.trace_no=g_pub_tx.trace_no;
    strcpy(sFn_reg.tx_br_no,g_pub_tx.tx_br_no);
    strcpy(sFn_reg.pact_no,cPact_no);
    strcpy(sFn_reg.prdt_code,cPrdt_code);
    strcpy(sFn_reg.ac_no,cAc_no);
    sFn_reg.ac_id = sMdm_ac_rel.ac_id;
    sFn_reg.ac_seqn = iAc_seqn;
    strcpy(sFn_reg.cif_type,cCif_type);
    strcpy(sFn_reg.ct_ind,"2");
    /*strcpy(sFn_reg.chnl_type,g_pub_rule.chnl_type);*/

    sFn_reg.ic_date = sFn_parm.ic_date;
    sFn_reg.end_date = sFn_parm.mtr_date;
    strcpy(sFn_reg.profit_flag,"N");
    strcpy(sFn_reg.brf,"认购理财产品");
    strcpy(sFn_reg.sts,"0");
    strcpy(sFn_reg.tel,g_pub_tx.tel);
    strcpy(sFn_reg.mang,cMang);
    strcpy(sFn_reg.agent_id_type,cAgent_id_type);
    strcpy(sFn_reg.agent_id,cAgent_id);
    strcpy(sFn_reg.agent_name,cAgent_name);
    iRet = Fn_reg_Ins(sFn_reg,g_pub_tx.reply);
    if(iRet)
    {
        Fn_reg_Debug(&sFn_reg);
        vtcp_log("登记理财产品登记簿错误iRet[%d]",iRet);
        strcpy(g_pub_tx.reply,"D345");
        goto ErrExit;
    }
    /*登记明细*/
    sFn_reg_hst.tx_date = g_pub_tx.tx_date;
    sFn_reg_hst.trace_no = g_pub_tx.trace_no;
    strcpy(sFn_reg_hst.tx_br_no,g_pub_tx.tx_br_no);
    strcpy(sFn_reg_hst.tel,g_pub_tx.tel);
    strcpy(sFn_reg_hst.type,"0");
    sFn_reg_hst.tx_amt = sFn_reg.tx_amt;
    strcpy(sFn_reg_hst.agent_id_type,cAgent_id_type);
    strcpy(sFn_reg_hst.agent_id,cAgent_id);
    strcpy(sFn_reg_hst.agent_name,cAgent_name);
    sFn_reg_hst.buy_date=sFn_reg_hst.tx_date;
    sFn_reg_hst.buy_trace_no=sFn_reg_hst.trace_no;
    strcpy(sFn_reg_hst.brf,"认购理财产品");
    iRet = Fn_reg_hst_Ins(sFn_reg_hst,g_pub_tx.reply);
    if(iRet)
    {
        Fn_reg_hst_Debug(&sFn_reg_hst);
        vtcp_log("登记理财产品登记簿错误iRet[%d]",iRet);
        strcpy(g_pub_tx.reply,"D345");
        goto ErrExit;
    }
    
    /*检查账户余额，同时更改控制金额*/
    iRet = Dd_mst_Dec_Upd(g_pub_tx.reply,"ac_id= %ld and ac_seqn=%d",sMdm_ac_rel.ac_id,iAc_seqn);
    if(iRet)
    {
        vtcp_log("定义活期主文件更新游标失败iRet[%d]",iRet);
        strcpy(g_pub_tx.reply,"D332 ");
        goto ErrExit;
    }
    iRet = Dd_mst_Fet_Upd(&sDd_mst,g_pub_tx.reply);
    if(iRet != 0 && iRet != 100)
    {
        vtcp_log("定义活期主文件更新游标失败iRet[%d]",iRet);
        strcpy(g_pub_tx.reply,"D332");
        goto ErrExit;
    }
    else if(iRet == 100)
    {
        vtcp_log("没有这个账户");
        strcpy(g_pub_tx.reply,"D104");
        set_zd_data("0130","该账户不存在");
        goto ErrExit;
    }
    if(sDd_mst.ac_sts[0] != '1')
    {
        vtcp_log("活期账户账户状态不正常");
        strcpy(g_pub_tx.reply,"B124");
        set_zd_data("0130","该活期账户账户状态不正常");
        goto ErrExit;
    }
    if(sDd_mst.hold_sts[0] == '1' || sDd_mst.hold_sts[0] == '2')
    {
        vtcp_log("账户被全部冻或只进不出");
        strcpy(g_pub_tx.reply,"L098");
        goto ErrExit;
    }
    /*检查下可用余额*/
    iRet = Dd_parm_Sel(g_pub_tx.reply,&sDd_parm,"prdt_no='%s'",sDd_mst.prdt_no);
    if(iRet)
    {
        vtcp_log("查询活期产品参数错误iRet[%d]",iRet);
        strcpy(g_pub_tx.reply,"W025 ");
        goto ErrExit;
    }
    /*检查下账户的币种*/
    if(strcmp(sFn_parm.cur_no,sDd_parm.cur_no) != 0)
    {
        vtcp_log("币种不支持fn_parm[%s] dd_parm[%s]",sFn_parm.cur_no,sDd_parm.cur_no);
        strcpy(g_pub_tx.reply,"D350");
        set_zd_data("0130","币种不支持");
        goto ErrExit;
    }
    /*dAvbal = sDd_mst.bal -  sDd_mst.hold_amt - sDd_mst.zf_bal - sDd_mst.ctl_amt - sDd_parm.min_bal;*/
    dAvbal = sDd_mst.bal -  sDd_mst.hold_amt - sDd_mst.ctl_amt - sDd_parm.min_bal;
    if(pub_base_CompDblVal(sFn_reg.tx_amt,dAvbal) > 0)
    {
        vtcp_log("账户可用余额不足");
        strcpy(g_pub_tx.reply,"P103");
        set_zd_data("0130","账户可用余额不足");
        goto ErrExit;
    }
    sDd_mst.ctl_amt += sFn_reg.tx_amt;
    iRet =  Dd_mst_Upd_Upd(sDd_mst,g_pub_tx.reply);
    if(iRet)
    {
        vtcp_log("更新活期主文件失败iRet[%d]",iRet);
        strcpy(g_pub_tx.reply,"O012");
        goto ErrExit;
    }
    Dd_mst_Clo_Upd();
    /*更改本机构配额*/
    iRet = upd_br_quota(sFn_parm,sFn_reg.tx_amt,0);
    if(iRet)
    {
        vtcp_log("更新机构配额错误[%d]",iRet);
        goto ErrExit;
    }
    strcpy(g_pub_tx.ac_no,cAc_no);
    strcpy(g_pub_tx.brf,"认购理财产品");
    g_pub_tx.tx_amt1 = sFn_reg.tx_amt;
    pub_mob_sendmail(  g_pub_tx.brf, "2219",g_pub_tx.ac_no,"0",g_pub_tx.tx_amt1, 0.00 );
    if( pub_ins_trace_log() )
    {
        vtcp_log("登记交易流水出错");
        strcpy(g_pub_tx.reply,"P126 ");
        goto ErrExit;
    }
   set_zd_data("0600",cPact_no);
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    vtcp_log("认购成功");
    return 0;
ErrExit:
    vtcp_log("认购失败");
    return 1;
}
/*撤销主函数*/
int cancel_func()
{
    int iRet = 0;
    char cAc_no[25];
    char cPrdt_code[21];
    char cFd123[1000];
    char cTmp[21];
    char cBuy_date[9];
    char cBuy_trace_no[10];
    char cAgent_name[61];
    char cAgent_id[21];
    char cAgent_id_type[2];
    double dAmt_tmp = 0.00;
    
    struct fn_parm_c sFn_parm;
    struct fn_reg_c sFn_reg;
    struct fn_reg_hst_c sFn_reg_hst;
    struct dd_mst_c sDd_mst;
    struct mdm_ac_rel_c sMdm_ac_rel;
    
    memset(cPrdt_code,0x00,sizeof(cPrdt_code));
    memset(cFd123,0x00,sizeof(cFd123));
    memset(cAc_no,0x00,sizeof(cAc_no));
    memset(cAgent_id,0x00,sizeof(cAgent_id));
    memset(cAgent_name,0x00,sizeof(cAgent_name));
    memset(cAgent_id_type,0x00,sizeof(cAgent_id_type));
    memset(&sMdm_ac_rel,0x00,sizeof(sMdm_ac_rel));
    
    get_zd_data("1230",cFd123);
    get_zd_data("0300",cAc_no);
    get_zd_data("0250",cAgent_name);
    get_zd_data("0620",cAgent_id);
    get_zd_data("0690",cAgent_id_type);

    
    iRet = Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdm_ac_rel, "ac_no='%s'", cAc_no);
    if (iRet == 100)
    {
        vtcp_log( "不存在该账户!!ac_id=[%ld]", sMdm_ac_rel.ac_id);
        strcpy(g_pub_tx.reply, "B117");
        set_zd_data("0130","不存在该账户");
        goto ErrExit;
    }
    else if (iRet)
    {
        vtcp_log( "读取介质账号对照表失败[%d]", iRet);
        strcpy(g_pub_tx.reply, "C114 ");
        goto ErrExit;
    }
        
    if(!strcmp(g_pub_tx.tty,"netb"))   /*如果是网银则流水记到开户行  add by lwb 20141202*/
    {
    	strcpy(g_pub_tx.tx_br_no,sMdm_ac_rel.opn_br_no);	
    }
    if(!strcmp(g_pub_tx.tty,"mobb"))   /*如果是手机银行则流水记到开户行  LJ 20151116*/
    {
    	strcpy(g_pub_tx.tx_br_no,sMdm_ac_rel.opn_br_no);	
    }
    /*截取123域信息 开始撤销记录*/
    for(int i = 1; ;i++)
    {
        memset(&sFn_parm,0x00,sizeof(sFn_parm));
        memset(&sFn_reg_hst,0x00,sizeof(sFn_reg_hst));
        memset(&sFn_reg,0x00,sizeof(sFn_reg));
        memset(&sDd_mst,0x00,sizeof(sDd_mst));
        memset(cTmp,0x00,sizeof(cTmp));
        memset(cBuy_trace_no,0x00,sizeof(cBuy_trace_no));
        memset(cBuy_date,0x00,sizeof(cBuy_date));
        pub_base_cut_str( cFd123, cTmp, "|", i );
        vtcp_log("cTmp[%s] i[%d]",cTmp,i);
        if( strlen(cTmp) == 0 )
        {
            vtcp_log("数据为空!");
            break;
        }
        strncpy(cBuy_date,cTmp,8);
        strncpy(cBuy_trace_no,cTmp+8,9);
        vtcp_log("cBuy_date[%s],cBuy_trace_no[%s]",cBuy_date,cBuy_trace_no);
        
        /*更新fn_reg dd_mst 登记fn_reg_hst*/
        iRet = Fn_reg_Dec_Upd(g_pub_tx.reply,"tx_date=%ld and trace_no=%ld",atol(cBuy_date),atol(cBuy_trace_no));
        if(iRet)
        {
            vtcp_log("定义理财产品登记簿游标出错[%d] ",iRet);
            strcpy(g_pub_tx.reply,"D332");
            goto ErrExit;
        }
        iRet = Fn_reg_Fet_Upd(&sFn_reg,g_pub_tx.reply);
        if(iRet)
        {
            vtcp_log("取理财产品登记簿游标出错[%d] ",iRet);
            strcpy(g_pub_tx.reply,"D330");
            goto ErrExit;
        }
        /*先检查产品是不是能撤销*/
        iRet = Fn_parm_Sel(g_pub_tx.reply,&sFn_parm,"prdt_code='%s'",sFn_reg.prdt_code);
        if(iRet)
        {
            vtcp_log("查询理财产品参数表失败");
            strcpy(g_pub_tx.reply, "D324");
            goto ErrExit;
        }
        if(sFn_parm.cancel_flag[0] != 'Y')
        {
            vtcp_log("该理财产品不能撤销");
            strcpy(g_pub_tx.reply, "D351");
            set_zd_data("0130","该理财产品不能撤销");
            goto ErrExit;
        }
        if(sFn_parm.stat[0] != '0')
        {
            vtcp_log("该产品非认购状态[%s] ",sFn_parm.stat);
            strcpy(g_pub_tx.reply,"C334");
            set_zd_data("0130","该产品非认购状态");
            goto ErrExit;
        }
        if(sFn_reg.sts[0] != '0')
        {
            vtcp_log("该记录非认购状态[%s] ",sFn_reg.sts);
            strcpy(g_pub_tx.reply,"D352 ");
            set_zd_data("0130","该记录非认购状态");
            goto ErrExit;
        }
        /*不是本机构购买的不能撤销*/
        
        if(strcmp(sFn_reg.tx_br_no,g_pub_tx.tx_br_no) != 0)
        {

            vtcp_log("该记录非本机构交易[%s] ",sFn_reg.tx_br_no);
            strcpy(g_pub_tx.reply,"D353 ");
            set_zd_data("0130","该记录非本机构交易");
            goto ErrExit;
        }
        sFn_reg.sts[0] = '*';
        sFn_reg.date1 = g_pub_tx.tx_date;
        iRet = Fn_reg_Upd_Upd(sFn_reg,g_pub_tx.reply);
        if(iRet)
        {
            vtcp_log("更新理财产品登记簿出错[%d] ",iRet);
            strcpy(g_pub_tx.reply,"D333 ");
            goto ErrExit;
        }
        Fn_reg_Clo_Upd();
        /*把dd_mst的控制金额更新更新*/
        iRet = Dd_mst_Dec_Upd(g_pub_tx.reply,"ac_id= %ld and ac_seqn=%d",sFn_reg.ac_id,sFn_reg.ac_seqn);
        if(iRet)
        {
            vtcp_log("定义活期主文件更新游标失败iRet[%d]",iRet);
            strcpy(g_pub_tx.reply,"D332");
            goto ErrExit;
        }
        iRet = Dd_mst_Fet_Upd(&sDd_mst,g_pub_tx.reply);
        if(iRet != 0 && iRet != 100)
        {
            vtcp_log("定义活期主文件更新游标失败iRet[%d]",iRet);
            strcpy(g_pub_tx.reply,"D332");
            goto ErrExit;
        }
        else if(iRet == 100)
        {
            vtcp_log("没有这个账户");
            strcpy(g_pub_tx.reply,"D104");
            set_zd_data("0130","没有该账户");
            goto ErrExit;
        }
        sDd_mst.ctl_amt -= sFn_reg.tx_amt;
        if(pub_base_CompDblVal(sDd_mst.ctl_amt,0.00) < 0)
        {
            vtcp_log("控制金额为负了[%lf]",sDd_mst.ctl_amt);
            strcpy(g_pub_tx.reply,"D354");
            set_zd_data("0130","控制金额为负");
            goto ErrExit;
        }
        iRet =  Dd_mst_Upd_Upd(sDd_mst,g_pub_tx.reply);
        if(iRet)
        {
            vtcp_log("更新活期主文件失败iRet[%d]",iRet);
            strcpy(g_pub_tx.reply,"O012");
            goto ErrExit;
        }
        Dd_mst_Clo_Upd();
        /*登记fn_reg_hst*/
        sFn_reg_hst.tx_date = g_pub_tx.tx_date;
        sFn_reg_hst.trace_no = g_pub_tx.trace_no;
        strcpy(sFn_reg_hst.tx_br_no,g_pub_tx.tx_br_no);
        strcpy(sFn_reg_hst.tel,g_pub_tx.tel);
        strcpy(sFn_reg_hst.type,"*");
        sFn_reg_hst.tx_amt = sFn_reg.tx_amt;
        strcpy(sFn_reg_hst.agent_id_type,cAgent_id_type);
        strcpy(sFn_reg_hst.agent_id,cAgent_id);
        strcpy(sFn_reg_hst.agent_name,cAgent_name);
        sFn_reg_hst.buy_date=sFn_reg.tx_date;
        sFn_reg_hst.buy_trace_no=sFn_reg.trace_no;
        strcpy(sFn_reg_hst.brf,"理财产品撤销");
        iRet = Fn_reg_hst_Ins(sFn_reg_hst,g_pub_tx.reply);
        if(iRet)
        {
            Fn_reg_hst_Debug(&sFn_reg_hst);
            vtcp_log("登记理财产品登记簿错误iRet[%d]",iRet);
            strcpy(g_pub_tx.reply,"D345");
            goto ErrExit;
        }
        /*更改本机构配额*/
    	vtcp_log("sFn_reg.tx_amt = %.2lf",sFn_reg.tx_amt);
	dAmt_tmp = sFn_reg.tx_amt;
        iRet = upd_br_quota(sFn_parm,sFn_reg.tx_amt,1);
        if(iRet)
        {
            vtcp_log("更新机构配额错误[%d]",iRet);
            goto ErrExit;
        }
    }
    strcpy(g_pub_tx.brf,"撤销理财产品");
    strcpy(g_pub_tx.ac_no,cAc_no);
    vtcp_log("sFn_reg.tx_amt = %.2lf",sFn_reg.tx_amt);
    pub_mob_sendmail(  g_pub_tx.brf, "2219",g_pub_tx.ac_no,"1",dAmt_tmp, 0.00 );
    if( pub_ins_trace_log() )
    {
        vtcp_log("登记交易流水出错");
        strcpy(g_pub_tx.reply,"P126");
        goto ErrExit;
    }
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    vtcp_log("撤销成功");
    set_zd_data("0130","撤销成功");
    return 0;
ErrExit:
    vtcp_log("撤销失败");
    return 1;
}
/*LJ20151105赎回主函数*/
int redempt_func()
{
    int iRet = 0;
    int  ret = 0;
    int iCount = 0;/*LJ*/
    int iHold_flag = 0;/*0账户未被全部冻结，1 账户被全部冻结*/
    int  iIntst_flag = 0;
    long lDays = 0;
    
    double dAmt_tmp = 0.00;
    double dRedempt_amt = 0.00;
    double dTem_amt = 0.00;/*用来存放当期购买金额与赎回金额的差值*/
    double dTmp_amt = 0.00;
    double cTmt = 0.00;
    double sTsy = 0.00;
    double dProfit_rate = 0.000000; /*分润利率*/
   	double dFct_rate=0.00000000;/*实际收益率*/
    double dProfit_amt = 0.00;/*分润金额*/
    double dFct_intst = 0.00;/*本次实际收益*/
    double dFredem_amt =0.00; /*实际一共赎回的钱*/
    
    char cAc_no[25];
    char cPrdt_code[21];
    char cFd123[1000];
    char cTmp[21];
    char cBuy_date[9];
    char cBuy_trace_no[10];
    char cAgent_name[61];
    char cAgent_id[21];
    char cAgent_id_type[2];
    char cBrf[21];/*摘要*/
    char cAc_no_gz[25],cName[61];
    
    struct fn_parm_c sFn_parm;
    struct fn_reg_c sFn_reg;
    struct fn_reg_hst_c sFn_reg_hst;
    struct dd_mst_c sDd_mst;
    struct mdm_ac_rel_c sMdm_ac_rel;
    struct fn_br_profit_c sFn_br_profit;
    struct fn_ac_rel_c sFn_ac_rel;
    
    memset(cPrdt_code,0x00,sizeof(cPrdt_code));
    memset(cAc_no,0x00,sizeof(cAc_no));
    memset(cAgent_id,0x00,sizeof(cAgent_id));
    memset(cAgent_name,0x00,sizeof(cAgent_name));
    memset(cAgent_id_type,0x00,sizeof(cAgent_id_type));
    memset(&sMdm_ac_rel,0x00,sizeof(sMdm_ac_rel));
    memset(&sFn_ac_rel,0x00,sizeof(sFn_ac_rel));
    memset(&sFn_br_profit,0x00,sizeof(sFn_br_profit));
    memset(&sFn_parm,0x00,sizeof(sFn_parm));
    memset(&sFn_reg_hst,0x00,sizeof(sFn_reg_hst));
    memset(&sFn_reg,0x00,sizeof(sFn_reg));
    memset(&sDd_mst,0x00,sizeof(sDd_mst));
    memset(cTmp,0x00,sizeof(cTmp));
    memset(cBuy_trace_no,0x00,sizeof(cBuy_trace_no));
    memset(cBuy_date,0x00,sizeof(cBuy_date));
    
    get_zd_data("0630",cPrdt_code);
    get_zd_data("0300",cAc_no);
    get_zd_data("0250",cAgent_name);
    get_zd_data("0620",cAgent_id);
    get_zd_data("0690",cAgent_id_type);
    get_zd_double("0420",&dRedempt_amt);
    /*前台获取赎回金额后保存赎回金额*/
    dFredem_amt = dRedempt_amt;

    
    iRet = Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdm_ac_rel, "ac_no='%s'", cAc_no);
    if (iRet == 100)
    {
        vtcp_log( "不存在该账户!!ac_id=[%ld]", sMdm_ac_rel.ac_id);
        strcpy(g_pub_tx.reply, "B117");
        set_zd_data("0130","不存在该账户");
        goto ErrExit;
    }
    else if (iRet)
    {
        vtcp_log( "读取介质账号对照表失败[%d]", iRet);
        strcpy(g_pub_tx.reply, "C114 ");
        goto ErrExit;
    }
        
    if(!strcmp(g_pub_tx.tty,"netb"))   /*如果是网银则流水记到开户行  add by lwb 20141202*/
    {
    	strcpy(g_pub_tx.tx_br_no,sMdm_ac_rel.opn_br_no);	
    }
    /*先检查产品是不是能赎回*/
    iRet = Fn_parm_Sel(g_pub_tx.reply,&sFn_parm,"prdt_code='%s'",cPrdt_code);
    if(iRet)
    {
        vtcp_log("查询理财产品参数表失败");
        strcpy(g_pub_tx.reply, "D324");
        goto ErrExit;
    }
    if(sFn_parm.prdt_type[0] != '2')
    {
        vtcp_log("该产品不是开放式");
        strcpy(g_pub_tx.reply, "D351");
        goto ErrExit;
    }
    if(sFn_parm.redempt[0] != 'Y')
    {
        vtcp_log("该理财产品不能赎回");
        strcpy(g_pub_tx.reply, "D351");
        set_zd_data("0130","该理财产品不能赎回");
        goto ErrExit;
    }
    if(sFn_parm.stat[0] != '0')
    {
        vtcp_log("该产品非认购状态[%s] ",sFn_parm.stat);
        strcpy(g_pub_tx.reply,"C334");
        set_zd_data("0130","该产品非认购状态");
        goto ErrExit;
    }
   	vtcp_log("sFn_parm.rate = [ %.2lf ] ",sFn_parm.rate);
    dFct_rate = sFn_parm.rate; /*计算收益时的利率*/
    /* 根据产品代码和帐号进行赎回,更新fn_reg，进行赎回交易*/
   iRet = Fn_reg_Dec_Upd(g_pub_tx.reply,"ac_no = '%s' and prdt_code = '%s'  and sts = '1' order by count desc",cAc_no,cPrdt_code);
    vtcp_log("游标是 【 %d 】",iRet);
    if(iRet)
    {
        vtcp_log("定义理财登记簿游标错误");
        strcpy(g_pub_tx.reply,"D101");
        goto ErrExit;
    }
    /*iRet = sql_max_int("fn_reg","count","ac_no = '%s' and prdt_code = '%s'",cAc_no,cPrdt_code);*/
    iCount = sql_min_long("fn_reg","count","ac_no = '%s' and prdt_code = '%s' and sts = '1'",cAc_no,cPrdt_code);/*LJ*/
    while(1)
    {      
        iRet = Fn_reg_Fet_Upd(&sFn_reg,g_pub_tx.reply);
	        if(iRet != 100 && iRet != 0)
	        {
	            vtcp_log("取理财登记簿游标错误");
	            strcpy(g_pub_tx.reply,"D330");
	            goto ErrExit;
	        }
	        
	        if(iRet == 0)
	        {      
	        	lDays =  pub_base_CalTrueDays(sFn_reg.ic_date,g_pub_tx.tx_date); /*计算收益时的天数*/
	        	if(sFn_reg.count == iCount) /*对最低赎回金额进行判断*/
	        	{
	        		dTem_amt = sFn_reg.tx_amt - dRedempt_amt - sFn_reg.redempt_amt;
			        vtcp_log("dRedempt_amt = [ %.2lf ] ",dRedempt_amt);
			        vtcp_log("第一次进入循环count = %d ",sFn_reg.count);
			        /*区分个人和对公*/
			        if(sFn_reg.cif_type[0] == '1')
			        {
			        	if(dTem_amt >= sFn_parm.per_min_amt)
			        	{
			        		sFn_reg.redempt_amt += dRedempt_amt;
			        	}
			        	else
			        	{
			        		sFn_reg.redempt_amt = sFn_reg.tx_amt;
			        		strcpy(sFn_reg.sts,"#");
			        		dRedempt_amt = sFn_reg.tx_amt - sFn_reg.redempt_amt;
			        		
			        	}
			        }
			        else
			        {
			        	if(dTem_amt >= sFn_parm.com_min_amt)
			        	{
			        		sFn_reg.redempt_amt += dRedempt_amt;
			        	}
			        	else
			        	{
			        		sFn_reg.redempt_amt = sFn_reg.tx_amt;
			        		strcpy(sFn_reg.sts,"#");
			        		dRedempt_amt = sFn_reg.tx_amt - sFn_reg.redempt_amt;
			        	}
			        }
			        dFct_intst = 0.00;
			        sTsy = 0.00;
			        sTsy = dRedempt_amt * lDays * dFct_rate * (1 + sFn_reg.flt_radio)/L365/L100;/*计算收益*/
			        dFct_intst += pub_base_PubDround_1(sTsy);
   					sFn_reg.fct_intst += dFct_intst;
   					sFn_reg.date2 = g_pub_tx.tx_date;
			    	sFn_reg.return_date=g_pub_tx.tx_date;
					sFn_reg.return_trace_no=g_pub_tx.trace_no;
				    sFn_reg.return_capital += dRedempt_amt;
			        iRet = Fn_reg_Upd_Upd(sFn_reg,g_pub_tx.reply);
			        if(iRet)
			        {
			            vtcp_log("更新理财产品登记簿出错[%d] ",iRet);
			            strcpy(g_pub_tx.reply,"D333 ");
			            goto ErrExit;
			        }
	        	}
	        	if(sFn_reg.tx_amt - sFn_reg.redempt_amt > dRedempt_amt) /*直接进行赎回不进行最低金额的判断*/
	        	{
	        		dFct_intst = 0.00;
			        sTsy = 0.00;
	        		vtcp_log("第二次进入循环count = %d ",sFn_reg.count);
	        		sFn_reg.redempt_amt += dRedempt_amt;
	        		sTsy = dRedempt_amt * lDays * dFct_rate * (1 + sFn_reg.flt_radio)/L365/L100;/*计算收益*/
			        dFct_intst += pub_base_PubDround_1(sTsy);
   					sFn_reg.fct_intst += dFct_intst;
				    sFn_reg.date2 = g_pub_tx.tx_date;
			    	sFn_reg.return_date=g_pub_tx.tx_date;
					sFn_reg.return_trace_no=g_pub_tx.trace_no;
				    sFn_reg.return_capital += dRedempt_amt;
			        iRet = Fn_reg_Upd_Upd(sFn_reg,g_pub_tx.reply);
			        if(iRet)
			        {
			            vtcp_log("更新理财产品登记簿出错[%d] ",iRet);
			            strcpy(g_pub_tx.reply,"D333 ");
			            goto ErrExit;
			        }
   					break;
	        		
	        	}
	        	else
        		{
        			dFct_intst = 0.00;
			        sTsy = 0.00;
        			vtcp_log("第三次进入循环count = %d ",sFn_reg.count);
        			cTmt = sFn_reg.tx_amt - sFn_reg.redempt_amt;
        			sFn_reg.redempt_amt = sFn_reg.tx_amt;
		        	strcpy(sFn_reg.sts,"#");
		        	sTsy = cTmt * lDays * dFct_rate * (1 + sFn_reg.flt_radio)/L365/L100;/*计算收益*/
		        	dFct_intst += pub_base_PubDround_1(sTsy);
					sFn_reg.fct_intst += dFct_intst;
		        	dRedempt_amt -= cTmt;
        		    sFn_reg.date2 = g_pub_tx.tx_date;
			    	sFn_reg.return_date=g_pub_tx.tx_date;
					sFn_reg.return_trace_no=g_pub_tx.trace_no;
				    sFn_reg.return_capital = sFn_reg.tx_amt;
			        iRet = Fn_reg_Upd_Upd(sFn_reg,g_pub_tx.reply);
			        if(iRet)
			        {
			            vtcp_log("更新理财产品登记簿出错[%d] ",iRet);
			            strcpy(g_pub_tx.reply,"D333 ");
			            goto ErrExit;
			        }
        		}
        		if(dRedempt_amt == 0)
        			break;

	        }
	        else
	        	break;
    }
    Fn_reg_Clo_Upd();
    ret=Fn_ac_rel_Sel(g_pub_tx.reply,&sFn_ac_rel,"prdt_code='%s'",cPrdt_code);
    if(ret)
	{
   		vtcp_log("查询理财产品清算账号配置表错误");
    	strcpy(g_pub_tx.reply,"S035");
   		goto ErrExit;
	}
    /*不是本机构购买的不能赎回
    if(strcmp(sFn_reg.tx_br_no,g_pub_tx.tx_br_no) != 0)
    {

        vtcp_log("该记录非本机构交易[%s] ",sFn_reg.tx_br_no);
        strcpy(g_pub_tx.reply,"D353 ");
        set_zd_data("0130","该记录非本机构交易");
        goto ErrExit;
    }*/
    /*有金额才记账*/
    if(pub_base_CompDblVal(dFredem_amt,0.00) > 0)
    {
    		 /* 贷方：客户账户(本金) */
        ret = my_get_ac_no(sFn_reg.ac_no,cAc_no,cName);
        vtcp_log("帐号是 【 %s 】",sFn_reg.ac_no);
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
        	  
        }
        else
        {
	        strcpy(g_pub_tx.ac_no,cAc_no);
	        strcpy(g_pub_tx.name,"");
	        strcpy(cBrf,"正常入账");
	        g_pub_tx.cif_no = 0;
	        g_pub_tx.ac_id=sFn_reg.ac_id;
	        g_pub_tx.ac_seqn=sFn_reg.ac_seqn;
	        g_pub_tx.tx_amt1=dFredem_amt;
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
    dTmp_amt = 0.00;
    dFct_intst =0.00;
    /*计算收益 dFct_intst考虑后期加上开放式的调整利率的未分配收益*/
    vtcp_log("lDays= %d dFct_rate= %d ",lDays,dFct_rate);
    dTmp_amt = dFredem_amt * lDays * dFct_rate * (1 + sFn_reg.flt_radio)/L365/L100;
    set_zd_double("0400",dTmp_amt);
    dFct_intst += pub_base_PubDround_1(dTmp_amt);
    vtcp_log("本次计算出的收益dTmp_amt[%lf]本次总实际收益dFct_intst[%lf] 预期sFn_reg.intst[%lf]",dTmp_amt,dFct_intst,sFn_reg.intst);
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
	    g_pub_tx.tx_amt1=dFredem_amt;
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
	 /*登记机构分润*/
    {
	    ret = Fn_br_profit_Dec_Upd(g_pub_tx.reply,"prdt_code='%s' and br_no='%s' and sts='0'",sFn_reg.prdt_code,sFn_reg.tx_br_no);
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
	        sFn_br_profit.tx_amt =0;
	        sFn_br_profit.redempt_amt = dFredem_amt ;
	        sFn_br_profit.profit_amt = 0;
	        sFn_br_profit.reg_date = g_pub_tx.tx_date;
	        sFn_br_profit.reg_trace_no = g_pub_tx.trace_no;
	        strcpy(sFn_br_profit.sts,"0");
	        strcpy(sFn_br_profit.brf,"理财赎回");
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
			sFn_br_profit.redempt_amt += dFredem_amt ;
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
    
	    /*登记fn_reg_hst*/
	    sFn_reg_hst.tx_date = g_pub_tx.tx_date;
	    sFn_reg_hst.trace_no = g_pub_tx.trace_no;
	    strcpy(sFn_reg_hst.tx_br_no,g_pub_tx.tx_br_no);
	    strcpy(sFn_reg_hst.tel,g_pub_tx.tel);
	    strcpy(sFn_reg_hst.type,"6");
	    sFn_reg_hst.tx_amt = dFredem_amt;
	    strcpy(sFn_reg_hst.agent_id_type,cAgent_id_type);
	    strcpy(sFn_reg_hst.agent_id,cAgent_id);
	    strcpy(sFn_reg_hst.agent_name,cAgent_name);
	    sFn_reg_hst.buy_date=sFn_reg.tx_date;
	    sFn_reg_hst.buy_trace_no=sFn_reg.trace_no;
	    strcpy(sFn_reg_hst.brf,"开放式产品赎回");
	    iRet = Fn_reg_hst_Ins(sFn_reg_hst,g_pub_tx.reply);
	    if(iRet)
	    {
	        Fn_reg_hst_Debug(&sFn_reg_hst);
	        vtcp_log("登记理财产品登记簿错误iRet[%d]",iRet);
	        strcpy(g_pub_tx.reply,"D345");
	        goto ErrExit;
	    }
 
    	/*更改本机构配额*/
		vtcp_log("sFn_reg_hst.tx_amt = %.2lf",sFn_reg_hst.tx_amt); /*将更改金额由认购金额改为赎回金额*/
		dAmt_tmp = sFn_reg_hst.tx_amt;
	    iRet = upd_br_quota(sFn_parm,sFn_reg_hst.tx_amt,1);
	    if(iRet)
	    {
	        vtcp_log("更新机构配额错误[%d]",iRet);
	        goto ErrExit;
	    }
	      
	    strcpy(g_pub_tx.brf,"赎回理财产品");
	    strcpy(g_pub_tx.ac_no,cAc_no);
	    pub_mob_sendmail(  g_pub_tx.brf, "2219",g_pub_tx.ac_no,"1",dAmt_tmp, 0.00 );
	    if( pub_ins_trace_log() )
	    {
	        vtcp_log("登记交易流水出错");
	        strcpy(g_pub_tx.reply,"P126");
	        goto ErrExit;
	    }
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    vtcp_log("赎回成功");
    set_zd_data("0130","赎回成功");
    return 0;
ErrExit:
    vtcp_log("赎回失败");
    return 1;
}

/*得到本机构的配额  只查认购时的配额，申购的算法不一样*/
int get_br_quota(struct fn_parm_c sfn_parm,double *quota_amt)
{
    int iRet  = 0;
    double dSum_amt= 0.00;/*已认购总额*/
    struct fn_br_quota_parm_c sFn_br_qp;
    memset(&sFn_br_qp        ,0x00,sizeof(sFn_br_qp));
    if(sfn_parm.quota_flag[0] == 'Y')
    {
        /*检查本机构理财产品是否能认购*/
        iRet = Fn_br_quota_parm_Sel(g_pub_tx.reply,&sFn_br_qp,"prdt_code='%s' and br_no='%s'",\
            sfn_parm.prdt_code,g_pub_tx.tx_br_no);
        if(iRet != 0 && iRet != 100)
        {
            vtcp_log("查询理财产品配额参数表失败iRet[%d]",iRet);
            strcpy(g_pub_tx.reply,"D327");
            goto ErrExit;
        }
        else if(iRet == 100)
        {
            /*机构没配置配额，则判断是否能认购*/
            if(sfn_parm.quota_flag[1] == 'Y')
            {
                /*查询总行的认购金额*/
                iRet = sql_sum_double("fn_reg","tx_amt",&dSum_amt,"prdt_code='%s' and sts='0'",\
                    sfn_parm.prdt_code);
                if(iRet)
                {
                    vtcp_log("统计fn_reg错误!");
                    strcpy(g_pub_tx.reply, "D331");
                    goto ErrExit;
                }
                vtcp_log("dSum_amt[%lf]",dSum_amt);
                *quota_amt = sfn_parm.sale_amt - dSum_amt;
            }
            else
            {
                *quota_amt = 0.00;
            }    
        }
        else
        {
            /*直接返回剩余配额*/
            vtcp_log("sFn_br_qp.available_amt[%lf]",sFn_br_qp.available_amt);
            *quota_amt = sFn_br_qp.available_amt;
        }
    }
    else/*不设置配额的取全行*/
    {
        /*查询总行的认购金额*/
        iRet = sql_sum_double("fn_reg","tx_amt",&dSum_amt,"prdt_code='%s' and sts='0'",\
            sfn_parm.prdt_code);
        if(iRet)
        {
            vtcp_log("统计fn_reg错误!");
            strcpy(g_pub_tx.reply, "D331");
            goto ErrExit;
        }
        vtcp_log("dSum_amt[%lf]",dSum_amt);
        *quota_amt = sfn_parm.sale_amt - dSum_amt;
    }
    vtcp_log("dSum_amt[%lf]",*quota_amt);
    if(pub_base_CompDblVal(*quota_amt,0.00) < 0)
    {
        *quota_amt  = 0.00;
    }
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    vtcp_log("获取配额成功 quota_amt[%lf]",*quota_amt);
    return 0;
ErrExit:
    vtcp_log("获取配额失败");
    return 1;
}
/*得到特殊客户的收益上浮比例,判断客户类型和账号是否一致*/
int get_flt_radio(char *prdt_code,double tx_amt,char * cif_lvl,long ac_id,double *flt_radio)
{
    int iRet = 0;
    double dBuy_amt = 0.00;
    double dFlt_radio1 = 0.000000;
    double dFlt_radio2 = 0.000000;
    double dFlt_radio3 = 0.000000;
    struct fn_flt_radio_parm_c sFn_flt_rp;
    memset(&sFn_flt_rp        ,0x00,sizeof(sFn_flt_rp));
    /*统计这个账号这个产品一共买了多少*/
    iRet = sql_sum_double("fn_reg","tx_amt",&dBuy_amt,"prdt_code='%s' and ac_id=%ld and sts='0'",\
            prdt_code,ac_id);
    if(iRet)
    {
        vtcp_log("统计fn_reg错误!");
        strcpy(g_pub_tx.reply, "D331");
        goto ErrExit;
    }
    vtcp_log("已经认购金额[%lf]",dBuy_amt);
    /*取这个产品的上浮规则*/
    iRet = Fn_flt_radio_parm_Sel(g_pub_tx.reply,&sFn_flt_rp,"prdt_code='%s'",prdt_code);
    if(iRet)
    {
        vtcp_log("查询理财产品浮动利率表错误!iRet[%d]",iRet);
        strcpy(g_pub_tx.reply, "D355");
        goto ErrExit;
    }
    if(sFn_flt_rp.ctl_flag[0]  == '0')
    {
        /*取最大值*/
        if(cif_lvl[0] >= sFn_flt_rp.cif_lvl[0])
        {
        	if(pub_base_CompDblVal(tx_amt,sFn_flt_rp.buy_amt) >= 0)
            dFlt_radio1 += sFn_flt_rp.flt_radio2;
        }
       /* if(pub_base_CompDblVal(tx_amt,sFn_flt_rp.buy_amt) >= 0)
        {
            dFlt_radio2= sFn_flt_rp.flt_radio21;
        }*/
        /** 判定新加4个浮动利率判定标准  取最大单笔交易金额的浮动利率 通过比较金额覆盖前面浮动利率获取 */
                if(pub_base_CompDblVal(tx_amt,sFn_flt_rp.buy_amt1) >= 0  && (pub_base_CompDblVal(sFn_flt_rp.buy_amt1 , 0.00)>0))  
        {
            dFlt_radio2= sFn_flt_rp.flt_radio21;
        }
                if(pub_base_CompDblVal(tx_amt,sFn_flt_rp.buy_amt2) >= 0  && (pub_base_CompDblVal(sFn_flt_rp.buy_amt2 , 0.00)>0))
        {
            dFlt_radio2= sFn_flt_rp.flt_radio22;
        }
                if(pub_base_CompDblVal(tx_amt,sFn_flt_rp.buy_amt3) >= 0  && (pub_base_CompDblVal(sFn_flt_rp.buy_amt3 , 0.00)>0))
        {
            dFlt_radio2= sFn_flt_rp.flt_radio23;
        }
                if(pub_base_CompDblVal(tx_amt,sFn_flt_rp.buy_amt4) >= 0  && (pub_base_CompDblVal(sFn_flt_rp.buy_amt4 , 0.00)>0))
        {
            dFlt_radio2= sFn_flt_rp.flt_radio24;
        }
        /**end 20140630*/
        if(pub_base_CompDblVal(dBuy_amt,sFn_flt_rp.total_amt) >=0)
        {
            dFlt_radio3 += sFn_flt_rp.flt_radio3;
        }
        /*取一个最大的*/
        if(dFlt_radio1 - dFlt_radio2 > 0.000001)
        {
            if(dFlt_radio1 - dFlt_radio3 > 0.000001)
            {
                *flt_radio = dFlt_radio1;
            }
            else
            {    
                *flt_radio = dFlt_radio3;
            }
        }
        else
        {
            if(dFlt_radio2 - dFlt_radio3 > 0.000001)
            {
                *flt_radio = dFlt_radio2;
            }
            else
            {    
                *flt_radio = dFlt_radio3;
            }
        }
        vtcp_log("上浮比例[%lf]",*flt_radio);
    }
    else
    {
        /*累加*/
        if(cif_lvl[0] >= sFn_flt_rp.cif_lvl[0])
        {
            *flt_radio += sFn_flt_rp.flt_radio1;
        }
        if(pub_base_CompDblVal(tx_amt,sFn_flt_rp.buy_amt) >= 0)
        {
            *flt_radio += sFn_flt_rp.flt_radio2;
        }
        if(pub_base_CompDblVal(dBuy_amt,sFn_flt_rp.total_amt) >=0)
        {
            *flt_radio += sFn_flt_rp.flt_radio3;
        }
        vtcp_log("上浮比例[%lf]",*flt_radio);
    }
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    vtcp_log("获取上浮比例成功");
    return 0;
ErrExit:
    vtcp_log("获取上浮比例失败");
    return 1;
}
/*检查账户类型*/
int get_cif_type(struct mdm_ac_rel_c smdm_ac_rel,char * cif_type,char *cif_lvl)
{
    int iRet = 0;
    struct cif_id_code_rel_c sCif_id_code_rel;
    struct cif_basic_inf_c sCif_basic_inf;
    memset(&sCif_id_code_rel,0x00,sizeof(sCif_id_code_rel));
    memset(&sCif_basic_inf,0x00,sizeof(sCif_basic_inf));

    /*取客户id*/
    iRet = Cif_id_code_rel_Sel(g_pub_tx.reply,&sCif_id_code_rel,"id_type='%s' and id_no='%s'",\
        smdm_ac_rel.id_type,smdm_ac_rel.id_no);
    if(iRet == 100)
    {
        vtcp_log("未在本行建立客户信息");
        strcpy(g_pub_tx.reply,"B116");
        goto ErrExit;
    }
    else if(iRet)
    {
        vtcp_log("查询客户证件信息失败【%d】",iRet);
        strcpy(g_pub_tx.reply,"C002");
        goto ErrExit;
    }
    /*取客户基本信息*/
    iRet = Cif_basic_inf_Sel(g_pub_tx.reply,&sCif_basic_inf,"cif_no=%ld",sCif_id_code_rel.cif_no);
    if(iRet)
    {
        vtcp_log("查询客户基本信息失败【%d】",iRet);
        strcpy(g_pub_tx.reply,"C113");
        goto ErrExit;
    }
    strcpy(cif_type,sCif_basic_inf.type); 
    cif_type[1] ='\0';
    /*strcpy(cif_lvl,sCif_basic_inf.cif_lvl);*/
    strcpy(cif_lvl,"5");
    cif_lvl[1] = '\0';
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    vtcp_log("客户类型和账号对应的相符 cif_lvl[%s]",cif_lvl);
    return 0;
ErrExit:
    vtcp_log("验证客户类型失败");
    return 1;
}
/*检查账号的介质状态*/
int check_note_sts(char *note_sts)
{
    /*检查介质*/
    if (note_sts[0] == '1')
    {
        vtcp_log( "该介质已挂失!");
        strcpy(g_pub_tx.reply, "P170");
        goto ErrExit;
    }
    else if (note_sts[0] == '2')
    {
        vtcp_log( "该介质已挂失换证!");
        strcpy(g_pub_tx.reply, "P171");
        goto ErrExit;
    }
    else if (note_sts[0] == '3')
    {
        vtcp_log( "该介质已正常换证!");
        strcpy(g_pub_tx.reply, "P104");
        goto ErrExit;
    }
    else if (note_sts[0] == '*')
    {
        vtcp_log("该介质已销户!");
        strcpy(g_pub_tx.reply, "M012");
        goto ErrExit;
    }
    else if (note_sts[0] == '4')
    {
        vtcp_log( "该介质已做部提，请输入新帐号!");
        strcpy(g_pub_tx.reply, "O217");
        goto ErrExit;
    }
    else if(note_sts[0] != '0')
    {
        vtcp_log( "介质状态不正常!");
        strcpy(g_pub_tx.reply, "M013");
        goto ErrExit;
    }
OkExit:
    return 0;
ErrExit:
    return 1;    
}

/*更新机构配额*/
int upd_br_quota(struct fn_parm_c sfn_parm,double tx_amt,int mode)
{
    int iRet  = 0;
    struct fn_br_quota_parm_c sFn_br_qp;
    memset(&sFn_br_qp        ,0x00,sizeof(sFn_br_qp));
    if(sfn_parm.quota_flag[0] == 'Y')
    {
        /*检查本机构理财产品是否能认购*/
        iRet = Fn_br_quota_parm_Dec_Upd(g_pub_tx.reply,"prdt_code='%s' and br_no='%s'",\
            sfn_parm.prdt_code,g_pub_tx.tx_br_no);
        if(iRet)
        {
            vtcp_log("查询理财产品配额参数表失败iRet[%d]",iRet);
            strcpy(g_pub_tx.reply,"D327");
            goto ErrExit;
        }
        iRet = Fn_br_quota_parm_Fet_Upd(&sFn_br_qp,g_pub_tx.reply);
        if(iRet != 0 && iRet != 100)
        {
            vtcp_log("查询理财产品配额参数表失败iRet[%d]",iRet);
            strcpy(g_pub_tx.reply,"D327");
            goto ErrExit;
        }
        else if(iRet == 100)
        {
            vtcp_log("没有限额不更新");
        }
        else
        {
            if(mode == 0)
            {
                sFn_br_qp.available_amt -= tx_amt;
            }
            else if(mode == 1)
            {
                sFn_br_qp.available_amt += tx_amt;
            }
            else
            {
                strcpy(g_pub_tx.reply,"S035");
                goto ErrExit;
            }
            iRet = Fn_br_quota_parm_Upd_Upd(sFn_br_qp,g_pub_tx.reply);
            if(iRet)
            {
                vtcp_log("更新理财产品配额参数表失败iRet[%d]",iRet);
                strcpy(g_pub_tx.reply,"D348");
                goto ErrExit;
            }
        }
        Fn_br_quota_parm_Clo_Upd();
    }
    else/*没有配额不做*/
    {
        
    }
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    vtcp_log("更新配额成功 ");
    return 0;
ErrExit:
    vtcp_log("更新配额失败");
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