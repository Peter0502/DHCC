/*************************************************
* 版 本 号 :  
* 程序作者 :  吴海帆
* 日    期 :  2015/01/19
* 所属模块 :  手机银行财富总览
* 程序名称 :  spMK06.c
* 程序作用 :
* 函数列表 :
* 使用注意 :
交易配置：
insert into tx_def values ('5061', '手机银行财富总览', '10000000000000000000001111110111111111110000110000010011111011111111111111101010010001000000100000011000000000000000000000000000', '0','4', '0');
insert into tx_flow_def values ('5061', 0, 'MK06', '#$');
insert into tx_sub_def values ('MK06', '手机银行财富总览', 'spMK06', '0', '0');
*************************************************/
#define EXTERN
#include <string.h>
#include "public.h"
#include "dd_mst_c.h"			/*活期存款主文件*/
#include "td_mst_c.h"			/*定期存款主文件*/
#include "mdm_ac_rel_c.h"		/*介质与账户关系表*/
#include "com_sys_parm_c.h"		/*公共系统参数*/
#include "cif_id_code_rel_c.h"		/*客户证件与客户号对照表*/
#include "dd_mst_hst_c.h"		/*活期存款明细*/
#include "td_mst_hst_c.h"		/*定期存款明细*/
#include "fn_reg_c.h"			/*理财产品登记簿*/
#include "cif_basic_inf_c.h"		/*客户基本信息表*/

#define CNT 200;

struct dd_mst_hst_c g_dd_mst_hst;
struct td_mst_hst_c g_td_mst_hst;
struct fn_reg_c g_fn_reg;

char l_date[6][10];	/*当前时间前5个月*/

int deal_date(long d_date);	/*时间处理*/
double deal_dd_mst(char *a_date,long ac_id);	/*活期总额处理*/
double deal_td_mst(char *a_date,long ac_id);	/*定期总额处理*/
double deal_fn_reg(char *a_date,long ac_id);	/*理财总额处理*/

int spMK06()
{
	struct com_sys_parm_c g_com_sys_parm;
	struct dd_mst_c g_dd_mst;
	struct cif_id_code_rel_c g_cif_id_code_rel;
	struct td_mst_c g_td_mst;
	struct cif_basic_inf_c g_cif_basic_inf;
	
	memset(&g_com_sys_parm,0x00,sizeof(g_com_sys_parm));
	memset(&g_cif_id_code_rel,0x00,sizeof(g_cif_id_code_rel));
	memset(&g_td_mst,0x00,sizeof(g_td_mst));
	
	char custName[60+1];	/*客户姓名*/
	char certType[1+1];	/*证件类型*/
	char certNo[20+1];	/*证件号码*/
	char type[1+1];		/*查询类型:1 资产*/
		
	memset(custName,0x00,sizeof(custName));
	memset(certType,0x00,sizeof(certType));
	memset(certNo,0x00,sizeof(certNo));
	memset(type,0x00,sizeof(type));
	
	double dd_mst_amt=0.00;		/*当前活期总额*/
	double qt_amt=0.00;		/*当前七天通知存款总额*/
	double td_mst_amt=0.00;		/*当前定期总额*/
	double fn_reg_amt=0.00;		/*当前理财总额*/
	double Amt=0.00;		/*当前月份总额*/
	
	double de_dd_mst_amt[6]={0.00};	/*月活期总额最后一笔*/
	double de_td_mst_amt[6]={0.00};	/*月定期总额最后一笔*/
	double dfn_reg_amt[6]={0.00};	/*月理财总额*/
	
	long hqac_id[CNT]={0};	/*活期账号*/
	long dqac_id[CNT]={0};	/*定期账号*/
	
	int ret;
	int k,i;
	char bufamt[20];
	long sysdate=0;
	long sys_month=0;
	
	double F_amt[5]={0.00};		/*前五个月月总额*/
	double amttmp=0.00;
	
	/**初始化公用结构**/
	if (pub_base_sysinit())
	{
	    vtcp_log("初始化公用结构错误!!");
	    strcpy(g_pub_tx.reply, "CU06");
	    goto ErrExit;
	}
	
	get_zd_data("0250",custName);
	get_zd_data("0680",certType);
	get_zd_data("0630",certNo);
	get_zd_data("0710",type);
	
	/*取当前系统时间*/
	ret = -1;
	ret=Com_sys_parm_Sel(g_pub_tx.reply,&g_com_sys_parm,"1=1");
	if( ret ) 
	{
		vtcp_log("%s %d::查询公共关系表错误!",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"O005");
		set_zd_data("0130","查询公共关系表错误");
		goto ErrExit;
	}
	sysdate=g_com_sys_parm.sys_date;
	vtcp_log("%s %d::t_date = [%ld]",__FILE__,__LINE__,sysdate);
	sys_month=sysdate/100;
	
	/*通过证件类型，证件号码取客户号*/
	ret = -1;
	zip_space(certNo);
	ret=Cif_id_code_rel_Sel(g_pub_tx.reply,&g_cif_id_code_rel,"id_type='%s' and id_no='%s'",certType,certNo);
	if( 100==ret || 1403==ret ) 
	{
		vtcp_log("%s %d::该证件对应的客户号不存在!",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"C020");
		set_zd_data("0130","该证件对应的客户号不存在");
		goto ErrExit;
	}
	else if( ret ) 
	{
		vtcp_log("%s %d::查询客户证件与客户号对照表错误!",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"AT06");
		set_zd_data("0130","查询客户证件与客户号对照表错误");
		goto ErrExit;
	}
	
	/*客户信息验证*/
	ret = -1;
	zip_space(custName);
	ret=Cif_basic_inf_Sel(g_pub_tx.reply,&g_cif_basic_inf,"cif_no='%ld' and name='%s'",g_cif_id_code_rel.cif_no,custName);
	if(ret)
	{
		vtcp_log("%s %d::客户名与证件号不匹配!",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"D249");
		set_zd_data("0130","客户名与证件号不匹配");
		goto ErrExit;	
	}
	
	/*查询活期账户id*/
    	memset(&g_dd_mst,0x00,sizeof(g_dd_mst));
    	k = 0;
	ret = -1;
	ret = Dd_mst_Dec_Sel(g_pub_tx.reply," cif_no='%ld'",g_cif_id_code_rel.cif_no);
	if( ret )
	{
		vtcp_log("[%s][%d]查询dd_mst错误![%d]\n",__FILE__,__LINE__,ret);
		strcpy(g_pub_tx.reply,"W015");
		set_zd_data("0130","查询活期存款主文件异常");
		return -1;
	}
	while(1)
	{
		ret=Dd_mst_Fet_Sel(&g_dd_mst,g_pub_tx.reply);
		if(ret==100 || ret == 1403)
		{
			break;
		}
		else if( 0 != ret  && 100 != ret)
		{
			vtcp_log("[%s],[%d] 查询dd_mst异常!",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"W015");
			set_zd_data("0130","查询活期存款主文件异常");
			Dd_mst_Clo_Sel();
			return -1;
		}
		else 
		{
			hqac_id[k]=g_dd_mst.ac_id;
			k++;
		}
	}
	Dd_mst_Clo_Sel();
    	
    	/*查询定期账户id*/
    	memset(&g_td_mst,0x00,sizeof(g_td_mst));
	k = 0;
	ret = -1;
	ret = Td_mst_Dec_Sel(g_pub_tx.reply," cif_no='%ld' ",g_cif_id_code_rel.cif_no);
	if( ret )
	{
		vtcp_log("[%s][%d]查询td_mst错误![%d]\n",__FILE__,__LINE__,ret);
		strcpy(g_pub_tx.reply,"W015");
		set_zd_data("0130","查询定期存款主文件异常");
		return -1;
	}
	while(1)
	{
		ret=Td_mst_Fet_Sel(&g_td_mst,g_pub_tx.reply);
		if(ret==100 || ret == 1403)
		{
			break;
		}
		else if( 0 != ret  && 100 != ret)
		{
			vtcp_log("[%s],[%d] 查询td_mst异常!",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"W017");
			set_zd_data("0130","查询定期存款主文件异常");
			Td_mst_Clo_Sel();
			return -1;
		}
		else 
		{
			dqac_id[k]=g_td_mst.ac_id;
			k++;
		}
	}
	Td_mst_Clo_Sel();
	
	/*时间处理*/
	memset(l_date,0x00,sizeof(l_date));
	ret = -1;
	ret=deal_date(sys_month);
	if(ret != 0)
	{
		vtcp_log("%s %d::日期处理失败! ret[%d] ",__FILE__,__LINE__,ret);
    		strcpy(g_pub_tx.reply,"9999");
    		set_zd_data("0130","日期处理失败");
    		goto ErrExit;	
	}
	
	/*当前活期总额*/
	ret = -1;
	ret=sql_sum_double("dd_mst","bal",&dd_mst_amt,"cif_no='%ld' and prdt_no <> '107'",g_cif_id_code_rel.cif_no);
	vtcp_log("%s %d::当前月活期总额dd_mst_amt=[%f] ",__FILE__,__LINE__,dd_mst_amt);				
    	if( ret )
    	{
    	 	vtcp_log("%s %d::sql_sum_double error! ret[%d] ",__FILE__,__LINE__,ret);
    	 	strcpy(g_pub_tx.reply,"9999");
    	 	set_zd_data("0130","当前月活期总额错误");
    		goto ErrExit;
    	}
	
	/*当前七天通知存款总额*/
	ret = -1;
	ret=sql_sum_double("dd_mst","bal",&qt_amt,"cif_no='%ld' and prdt_no='107'",g_cif_id_code_rel.cif_no);
	vtcp_log("%s %d::当前七天通知存款总额qt_amt=[%f] ",__FILE__,__LINE__,qt_amt); 				
    	if( ret )
    	{
    	 	vtcp_log("%s %d::sql_sum_double error! ret[%d] ",__FILE__,__LINE__,ret);
    	 	strcpy(g_pub_tx.reply,"9999");
    	 	set_zd_data("0130","当前七天通知存款总额错误");
    		goto ErrExit;
    	}
	
	/*当前定期总额*/
	ret = -1;
	ret=sql_sum_double("td_mst","bal",&td_mst_amt,"cif_no='%ld'",g_cif_id_code_rel.cif_no);
	vtcp_log("%s %d::当前定期总额td_mst_amt=[%f] ",__FILE__,__LINE__,td_mst_amt); 				
    	if( ret )
    	{
    	 	vtcp_log("%s %d::sql_sum_double error! ret[%d] ",__FILE__,__LINE__,ret);
    	 	strcpy(g_pub_tx.reply,"9999");
    	 	set_zd_data("0130","当前定期总额错误");
    		goto ErrExit;
    	}
    	
    	/*当前理财总额*/
    	double fn_reg_amt_tmp=0.00;
    	k=0;
	while(1)
	{
		if(hqac_id[k] == 0)
		{
			break;	
		}
		fn_reg_amt_tmp=0.00;
		ret=-1;
		ret=sql_sum_double("fn_reg","tx_amt",&fn_reg_amt_tmp,"ac_id='%ld' and pay_date<='%ld' and sts = '1'",hqac_id[k],g_com_sys_parm.sys_date);
		vtcp_log("%s %d::当前账号理财总额fn_reg_amt_tmp=[%f] ",__FILE__,__LINE__,fn_reg_amt_tmp); 				
    		if( ret )
    		{
    		 	vtcp_log("%s %d::sql_sum_double error! ret[%d] ",__FILE__,__LINE__,ret);
    		 	strcpy(g_pub_tx.reply,"9999");
    		 	set_zd_data("0130","当前账号理财总额错误");
    			goto ErrExit;
    		}
		dfn_reg_amt[0]+=fn_reg_amt_tmp;		/*月理财总额*/
		k++;
	}
    	
    	double dd_mst_amt_tmp=0.00;
	double td_mst_amt_tmp=0.00;
	for(i=1;i<6;i++)
	{
		/*处理前五个月活期总额*/
		k=0;
		while(1)
		{
			dd_mst_amt_tmp=0.00;
			if(hqac_id[k] == 0)
			{
				break;	
			}
			dd_mst_amt_tmp=deal_dd_mst(l_date[i],hqac_id[k]);
			de_dd_mst_amt[i]+=dd_mst_amt_tmp;	/*月活期总额*/
			k++;
				
		}
		
		/*处理前五个月定期总额*/
		k=0;
		while(1)
		{
			td_mst_amt_tmp=0.00;
			if(dqac_id[k] == 0)
			{
				break;	
			}
			td_mst_amt_tmp=deal_td_mst(l_date[i],dqac_id[k]);
			de_td_mst_amt[i]+=td_mst_amt_tmp;	/*月定期总额*/	
			k++;	
		}
		
		/*处理前五个月理财总额*/
		k=0;
		while(1)
		{
			fn_reg_amt_tmp=0.00;
			if(hqac_id[k] == 0)
			{
				break;	
			}
			fn_reg_amt_tmp=deal_fn_reg(l_date[i],hqac_id[k]);
			dfn_reg_amt[i]+=fn_reg_amt_tmp;		/*月理财总额*/
			k++;
		}			
	}

	set_zd_double("0560",dd_mst_amt);	/*当前活期总额*/
	set_zd_double("0570",qt_amt);		/*当前七天通知存款总额*/
	memset(bufamt,0x00,sizeof(bufamt));
	sprintf(bufamt,"%.2f",td_mst_amt*100);
	set_zd_data("0300",bufamt);		/*当前定期总额*/
	memset(bufamt,0x00,sizeof(bufamt));
	sprintf(bufamt,"%.2f",dfn_reg_amt[0]*100);
	set_zd_data("0310",bufamt);		/*当前理财总额*/
	Amt=dd_mst_amt+qt_amt+td_mst_amt+dfn_reg_amt[0];
	memset(bufamt,0x00,sizeof(bufamt));
	sprintf(bufamt,"%.2f",Amt*100);
	set_zd_data("0320",bufamt);		/*当前月份总额*/
	
	/*前五月总额处理：总额=活期+定期+理财*/
	int j=0;
	for(j=0;j<5;j++)
	{
		F_amt[j]=de_dd_mst_amt[j+1]+de_td_mst_amt[j+1]+dfn_reg_amt[j+1];
		vtcp_log("%s %d::F_amt[%d]=%.2f",__FILE__,__LINE__,j,F_amt[j]);	
	}
	
	memset(bufamt,0x00,sizeof(bufamt));
	sprintf(bufamt,"%.2f",F_amt[0]*100);
	set_zd_data("0330",bufamt);	/*前一月份总额*/
	memset(bufamt,0x00,sizeof(bufamt));
	sprintf(bufamt,"%.2f",F_amt[1]*100);
	set_zd_data("0370",bufamt);	/*前二月份总额*/
	memset(bufamt,0x00,sizeof(bufamt));
	sprintf(bufamt,"%.2f",F_amt[2]*100);
	set_zd_data("0380",bufamt);	/*前三月份总额*/
	memset(bufamt,0x00,sizeof(bufamt));
	sprintf(bufamt,"%.2f",F_amt[3]*100);
	set_zd_data("0620",bufamt);	/*前四月份总额*/
	memset(bufamt,0x00,sizeof(bufamt));
	sprintf(bufamt,"%.2f",F_amt[4]*100);
	set_zd_data("0750",bufamt);	/*前五月份总额*/
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	set_zd_data("0120","0000");
	set_zd_data("0130","手机银行财富总览成功！");
	vtcp_log("success !!: reply is[%s]", g_pub_tx.reply);
	return 0;
ErrExit:
	vtcp_log("fail !!: reply is[%s]", g_pub_tx.reply);
	return 1;
}

int deal_date(long d_date)
{
	long date_tmp=0;
	long date_year=0;
	
	date_tmp=d_date%100;
	date_year=d_date/100;
	sprintf(l_date[0],"%06ld",d_date);

	int i=0;
	for(i=0;i<5;i++)
	{
		if(date_tmp-1 > 0)
		{
			sprintf(l_date[i+1],"%04ld%02ld",date_year,date_tmp-1);
			date_tmp=date_tmp-1;		
		}
		if(date_tmp-1 == 0)
		{
			sprintf(l_date[i+1],"%04ld12",date_year-1);
			date_year=date_year-1;
			date_tmp=12;
		}	
	}
	return 0;
}


double deal_dd_mst(char *a_date,long ac_id)
{
	int ret = -1;
	double he_dd_mst_amt=0.00;
	
	/*月活期明细最后一笔*/
	memset(&g_dd_mst_hst,0x00,sizeof(g_dd_mst_hst));
	ret=Dd_mst_hst_Sel(g_pub_tx.reply, &g_dd_mst_hst,"ac_id ='%ld' and substr(tx_date,0,6)<='%s'\
	 order by hst_cnt desc", ac_id,a_date);
	if (ret != 100 && ret != 0)
	{
		vtcp_log("%s %d::查询活期账户明细失败!",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"W050");
		set_zd_data("0130","查询活期账户明细失败");
		Dd_mst_Clo_Sel();
		return -1;
	}  
	he_dd_mst_amt=g_dd_mst_hst.bal;	
	return he_dd_mst_amt;	
}

double deal_td_mst(char *a_date,long ac_id)
{
	int ret=-1;
	double he_td_mst_amt=0.00;
	
	memset(&g_td_mst_hst,0x00,sizeof(g_td_mst_hst));
	ret=Td_mst_hst_Sel(g_pub_tx.reply, &g_td_mst_hst,"ac_id ='%ld' and substr(tx_date,0,6)<='%s'\
	 order by hst_cnt desc", ac_id,a_date);
	if (ret != 100 && ret != 0)
	{
		vtcp_log("%s %d::查询定期账户明细失败!",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"W051");
		set_zd_data("0130","查询定期账户明细失败");
		Td_mst_Clo_Sel();
		return -1;
	}   
	he_td_mst_amt=g_td_mst_hst.bal;
	return he_td_mst_amt;
}

double deal_fn_reg(char *a_date,long ac_id)
{
	int ret=-1;
	double amt_tmp=0.00;

	/*月理财总额*/
	amt_tmp=0.00;
	ret=sql_sum_double("fn_reg","tx_amt",&amt_tmp,"ac_id=%ld and ((substr(pay_date,0,6)>='%s' and substr(return_date,0,6)<'%s'\
	 and sts='2') or (substr(pay_date,0,6)<='%s' and sts = '1'))",ac_id,a_date,a_date,a_date);
	vtcp_log("%s %d::当前账号理财总额amt_tmp=[%f] ",__FILE__,__LINE__,amt_tmp); 				
    	if( ret )
    	{
    	 	vtcp_log("%s %d::sql_sum_double error! ret[%d] ",__FILE__,__LINE__,ret);
    	 	strcpy(g_pub_tx.reply,"9999");
    	 	set_zd_data("0130","当前账号理财总额错误");
    	 	Dd_mst_Clo_Sel();
    		return -1;
    	}
	return amt_tmp;		
}
