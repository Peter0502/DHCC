/*********************************************************************
* 版 本 号 :  
* 程序作者 :  lixiang
* 日    期 :  2015/01/19
* 所属模块 : 手机银行财富总览
* 程序名称 :  spMK07.c
* 程序作用 :
* 函数列表 :
* 使用注意 :
交易配置：
insert into tx_def values ('5028', '手机银行财富总览', '10000000000000000000110011010011100000111111111110000001111111000111111100000000000110000000100000011000000000000000000000000000', '0','5', '0');
insert into tx_flow_def values ('5028', 0, 'MK07', '#$');
insert into tx_sub_def values ('MK07', '手机银行财富总览', 'spMK07', '0', '0');
***********************************************************************/
#define EXTERN
#include <string.h>
#include <stdio.h>
#include "public.h"
#include "dd_mst_c.h"
#include "fn_reg_c.h"
#include "td_mst_hst_c.h"
#include "dd_mst_hst_c.h"
#include "cif_id_code_rel_c.h"
#include "cif_basic_inf_c.h"
#include "com_sys_parm_c.h"

int spMK07()
{
	struct dd_mst_hst_c dd_mst_hst_1;		/*活期存款明细*/
	struct dd_mst_c dd_mst_1;			/*活期存款主文件*/
	struct td_mst_c td_mst_1;			/*定期存款主文件*/
	struct td_mst_hst_c td_mst_hst_1;		/*定期存款明细*/
	struct fn_reg_c	fn_reg_1;			/*理财产品登记簿*/
	struct cif_id_code_rel_c cif_id_code_rel_1;	/*客户证件与客户号对照表*/
	struct cif_basic_inf_c cif_basic_inf_1;  	/*客户基本信息表*/
	struct com_sys_parm_c com_sys_parm_1;		/*公共系统参数*/
	
	int ret=0;
	int i=0;
	long time_1=0;
	long time_2=0;
	char custName[61+1];				/*户名*/
	char certType[1+1];				/*证件类型*/
	char certNo[22];				/*证件号码*/
	char WhereList[1024];				/*查询条件*/
	double DdTotalMoney[6]={0.00};			/*前i个月活期总额*/
	double SevTotalMoney=0.00;			/*当前七天通存总额*/
	double TdTotalMoney[6]={0.00};			/*前i个月定期总额*/
	double ProTotalMoney[6]={0.00};			/*前i个月理财总额*/
	double SigProTotalMoney_1=0.00;			
	double SigProTotalMoney_2=0.00;			
	
	memset(custName,0x00,sizeof(custName));
	memset(WhereList,0x00,sizeof(WhereList));
	memset(certType,0x00,sizeof(certType));
	memset(certNo,0x00,sizeof(certNo));
	memset(&td_mst_hst_1,0x00,sizeof(td_mst_hst_1));
	memset(&dd_mst_hst_1,0x00,sizeof(dd_mst_hst_1));
	memset(&dd_mst_1,0x00,sizeof(dd_mst_1));
	memset(&td_mst_1,0x00,sizeof(td_mst_1));
	memset(&fn_reg_1,0x00,sizeof(fn_reg_1));
	memset(&cif_id_code_rel_1,0x00,sizeof(cif_id_code_rel_1));
	memset(&cif_basic_inf_1,0x00,sizeof(cif_basic_inf_1));
	memset(&com_sys_parm_1,0x00,sizeof(com_sys_parm_1));
	
	/**初始化公用结构**/
	if (pub_base_sysinit())
	{
	    vtcp_log("初始化公用结构错误!!");
	    strcpy(g_pub_tx.reply, "CU06");
	    goto ErrExit;
	}
	/*取系统日期com_sys_parm_1.sys_date*/
	ret=Com_sys_parm_Sel( g_pub_tx.reply,&com_sys_parm_1, "1=1" );
	if(ret)
	{
		vtcp_log("%s %d::Com_sys_parm_Sel [%d] error ",__FILE__,__LINE__,ret);
		set_zd_data("0130","查询公共系统参数状态错误!!");
	 	strcpy(g_pub_tx.reply,"O072");
	 	goto ErrExit;	
	}
	time_1=com_sys_parm_1.sys_date-com_sys_parm_1.sys_date%100+31;
	
	get_zd_data("0380",custName);
	vtcp_log("%s,%d 户名=[%s]\n",__FILE__,__LINE__,custName);
	get_zd_data("0670",certType);
	vtcp_log("%s,%d 证件类型=[%s]\n",__FILE__,__LINE__,certType);
	get_zd_data("0250",certNo);
	vtcp_log("%s,%d 证件号码=[%s]\n",__FILE__,__LINE__,certNo);
	
	/*客户证件与客户号对照表,获取客户号cif_id_code_rel_1.cif_no*/
  	ret = Cif_id_code_rel_Sel(g_pub_tx.reply,&cif_id_code_rel_1,"id_type='%s' and id_no='%s'",certType,certNo);
	if(ret)
	{
		vtcp_log("%s %d::Cif_id_code_rel [%d] error ",__FILE__,__LINE__,ret);
		strcpy(g_pub_tx.reply,"B115");
		set_zd_data("0130","客户证件与客户号不符");
		goto ErrExit;	
	}
  	/*客户基本信息表*/ 
 	ret = Cif_basic_inf_Sel(g_pub_tx.reply,&cif_basic_inf_1,"cif_no =%ld and name='%s' and sts='1' ",cif_id_code_rel_1.cif_no,zip_space(custName));
	if(ret)
	{
		vtcp_log("%s %d::Cif_basic_inf_sel [%d] error ",__FILE__,__LINE__,ret);
		strcpy(g_pub_tx.reply,"C113");
		set_zd_data("0130","客户姓名与客户号不符");
		goto ErrExit;	
	}
	
	/*当前七天通知存款总额*/
	ret = -1;
	ret=sql_sum_double("dd_mst","bal",&SevTotalMoney,"cif_no='%ld' and prdt_no='107'",cif_id_code_rel_1.cif_no);
	vtcp_log("%s %d::当前七天通知存款总额SevTotalMoney=[%f] ",__FILE__,__LINE__,SevTotalMoney); 				
    	if( ret )
    	{
    	 	vtcp_log("%s %d::sql_sum_double error! ret[%d] ",__FILE__,__LINE__,ret);
    	 	strcpy(g_pub_tx.reply,"9999");
    	 	set_zd_data("0130","当前七天通知存款总额错误");
    		goto ErrExit;
    	}
	
	/*-------统计前i个月各个部分总额-------*/
	for(i=0;i<6;i++)
	{
		/*前i个月理财总额ProTotalMoney[i]*/
		ret = -1;
		memset(WhereList,0x00,sizeof(WhereList));
		sprintf(WhereList,"cif_no='%ld'",cif_id_code_rel_1.cif_no);
		ret = Dd_mst_Dec_Sel(g_pub_tx.reply,"%s",WhereList);
		while(1)
		{	
			ret=-1;
			ret=Dd_mst_Fet_Sel(&dd_mst_1,g_pub_tx.reply);
			if(ret != 100 && ret != 0)
			{
				vtcp_log("%s %d::取活期存款主文件游标错误[%d]",__FILE__,__LINE__,ret);
				strcpy(g_pub_tx.reply,"D330");
				set_zd_data("0130","查找活期存款主文件表报错");
				Dd_mst_Clo_Sel ();	
				goto ErrExit;	
			}
			else if(ret == 100 || ret == 1403)
			{
				break;
			}
			else
			{
				time_2=time_1;
				memset(WhereList,0x00,sizeof(WhereList));
				if(((time_2/100)%100)<=i)
				{
					time_2=time_1-i*100-8800;
				}
				else
				{
					time_2=time_1-i*100;
				}
				/*未分配收益部分*/
				ret=-1;
				ret=sql_sum_double("fn_reg","tx_amt",&SigProTotalMoney_1,"ac_id='%ld' and pay_date<='%ld' and sts='1'",dd_mst_1.ac_id,time_2);				
    				if( ret )
    				{
    			 		vtcp_log("%s %d::sql_sum_double error! ret[%d] ",__FILE__,__LINE__,ret);
    			 		strcpy(g_pub_tx.reply,"9999");
    			 		set_zd_data("0130","统计账号未分配收益部分理财总额错误");
    			 		Dd_mst_Clo_Sel();
    					return -1;
    				}
    				ProTotalMoney[i]+=SigProTotalMoney_1;
    				
    				/*已分配收益部分*/
    				ret=-1;
    				ret=sql_sum_double("fn_reg","tx_amt",&SigProTotalMoney_2,"ac_id='%ld' and pay_date<='%ld' and return_date >'%ld' and sts='2'",dd_mst_1.ac_id,time_2,time_2);				
    				if( ret )
    				{
    			 		vtcp_log("%s %d::sql_sum_double error! ret[%d] ",__FILE__,__LINE__,ret);
    			 		strcpy(g_pub_tx.reply,"9999");
    			 		set_zd_data("0130","统计账号已分配收益部分理财总额错误");
    			 		Dd_mst_Clo_Sel();
    					return -1;
    				}
    				ProTotalMoney[i]+=SigProTotalMoney_2;				
			}			
		}
		vtcp_log("%s %d::前[%d]个月理财总额ProTotalMoney[%d]=[%f] ",__FILE__,__LINE__,i,i,ProTotalMoney[i]); 
		Dd_mst_Clo_Sel ();
		
		/*前i个月活期总额DdTotalMoney[i] */
		ret = -1;
		memset(WhereList,0x00,sizeof(WhereList));
		sprintf(WhereList,"cif_no='%ld' ",cif_id_code_rel_1.cif_no);
		ret = Dd_mst_Dec_Sel(g_pub_tx.reply,"%s",WhereList);
		while(1)
		{	
			ret=-1;
			ret=Dd_mst_Fet_Sel(&dd_mst_1,g_pub_tx.reply);
			if(ret != 100 && ret != 0)
			{
				vtcp_log("%s %d::取活期存款主文件游标错误[%d]",__FILE__,__LINE__,ret);
				strcpy(g_pub_tx.reply,"D330");
				set_zd_data("0130","查找活期存款主文件表报错");
				Dd_mst_Clo_Sel ();	
				goto ErrExit;	
			}
			else if(ret == 100 || ret == 1403)
			{
				break;
			}
			else
			{
				time_2=time_1;
				memset(WhereList,0x00,sizeof(WhereList));
				if(((time_2/100)%100)<=i)
				{
					time_2=time_1-i*100-8800;
				}
				else
				{
					time_2=time_1-i*100;
				}
				memset(&dd_mst_hst_1,0x00,sizeof(dd_mst_hst_1));
				ret=-1;			
				ret = Dd_mst_hst_Sel(g_pub_tx.reply,&dd_mst_hst_1,"ac_id=%ld and tx_date<='%ld' order by hst_cnt desc ",dd_mst_1.ac_id,time_2);
				if(ret != 100 && ret != 0)
				{
				  vtcp_log("查询活期存款明细失败");
				  strcpy(g_pub_tx.reply,"W050");
				  set_zd_data("0130","查询活期明细表异常");
				  Dd_mst_Clo_Sel();
				  goto ErrExit;
				}
				/*活期总额hq_sum*/
				DdTotalMoney[i] += dd_mst_hst_1.bal;
			}
		}
		vtcp_log("%s %d::前%d个月活期总额DdTotalMoney[%d] = [%16.2f]",__FILE__,__LINE__,i,i,DdTotalMoney[i]);
		Dd_mst_Clo_Sel ();
		
		/*前i个月定期总额DdTotalMoney[i] */
		ret = -1;
		memset(WhereList,0x00,sizeof(WhereList));
		sprintf(WhereList,"cif_no='%ld' ",cif_id_code_rel_1.cif_no);
		ret = Td_mst_Dec_Sel(g_pub_tx.reply,"%s",WhereList);
		while(1)
		{	
			ret=-1;
			ret=Td_mst_Fet_Sel(&td_mst_1,g_pub_tx.reply);
			if(ret != 100 && ret != 0)
			{
				vtcp_log("%s %d::取定期存款主文件游标错误[%d]",__FILE__,__LINE__,ret);
				strcpy(g_pub_tx.reply,"D330");
				set_zd_data("0130","查找定期存款主文件表报错");
				Td_mst_Clo_Sel ();	
				goto ErrExit;	
			}
			else if(ret == 100 || ret == 1403)
			{
				break;
			}
			else
			{
				time_2=time_1;
				memset(WhereList,0x00,sizeof(WhereList));
				if(((time_2/100)%100)<=i)
				{
					time_2=time_1-i*100-8800;
				}
				else
				{
					time_2=time_1-i*100;
				}
				memset(&td_mst_hst_1,0x00,sizeof(td_mst_hst_1));
				ret=-1;			
				ret = Td_mst_hst_Sel(g_pub_tx.reply,&td_mst_hst_1,"ac_id=%ld and tx_date<='%ld' order by hst_cnt desc ",td_mst_1.ac_id,time_2);
				if(ret != 100 && ret != 0)
				{
				  vtcp_log("查询定期存款明细失败");
				  strcpy(g_pub_tx.reply,"W050");
				  set_zd_data("0130","查询定期明细表异常");
				  Td_mst_Clo_Sel();
				  goto ErrExit;
				}
				/*定期总额TdTotalMoney*/
				TdTotalMoney[i] += td_mst_hst_1.bal;
			}
		}
		vtcp_log("%s %d::前%d个月定期总额DdTotalMoney[%d] = [%16.2f]",__FILE__,__LINE__,i,i,TdTotalMoney[i]);
		Td_mst_Clo_Sel();
	}	
	
	/*当前活期总额*/
	set_zd_double("0390",DdTotalMoney[0]-SevTotalMoney);
	vtcp_log("当前活期总额 = [%16.2f]",DdTotalMoney[0]);
	/*当前定期总额*/
	set_zd_double("0400",TdTotalMoney[0]);
	vtcp_log("当前定期总额= [%16.2f]",TdTotalMoney[0]);
	/*当前理财总额*/
	set_zd_double("0410",ProTotalMoney[0]);
	vtcp_log("当前理财总额= [%16.2f]",ProTotalMoney[0]);
	/*当前七天通知存款总额*/
	set_zd_double("0420",SevTotalMoney);
	vtcp_log("当前七天通知存款总额= [%16.2f]",SevTotalMoney);
	/*当前月份总额*/
	set_zd_double("0560",DdTotalMoney[0]+TdTotalMoney[0]+ProTotalMoney[0]);
	vtcp_log("当前月份总额 = [%16.2f]",DdTotalMoney[0]+TdTotalMoney[0]+ProTotalMoney[0]);
	/*前一月份总额*/
	set_zd_double("0570",DdTotalMoney[1]+TdTotalMoney[1]+ProTotalMoney[1]);
	vtcp_log("前一月份总额 = [%16.2f]",DdTotalMoney[1]+TdTotalMoney[1]+ProTotalMoney[1]);
	/*前二月份总额*/
	set_zd_double("0580",DdTotalMoney[2]+TdTotalMoney[2]+ProTotalMoney[2]);
	vtcp_log("前二月份总额 = [%16.2f]",DdTotalMoney[2]+TdTotalMoney[2]+ProTotalMoney[2]);
	/*前三月份总额*/
	set_zd_double("0590",DdTotalMoney[3]+TdTotalMoney[3]+ProTotalMoney[3]);
	vtcp_log("前三月份总额 = [%16.2f]",DdTotalMoney[3]+TdTotalMoney[3]+ProTotalMoney[3]);
	/*前四月份总额*/
	set_zd_double("0600",DdTotalMoney[4]+TdTotalMoney[4]+ProTotalMoney[4]);
	vtcp_log("前四月份总额 = [%16.2f]",DdTotalMoney[4]+TdTotalMoney[4]+ProTotalMoney[4]);
	/*前五月份总额*/
	set_zd_double("0610",DdTotalMoney[5]+TdTotalMoney[5]+ProTotalMoney[5]);
	vtcp_log("前五月份总额 = [%16.2f]",DdTotalMoney[5]+TdTotalMoney[5]+ProTotalMoney[5]);

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	set_zd_data("0120","0000");
	set_zd_data("0130","手机银行财富总览交易成功");
	vtcp_log("success !!: reply is[%s]", g_pub_tx.reply);
	return 0;
ErrExit:
	vtcp_log("fail !!: reply is[%s]", g_pub_tx.reply);
	return 1;
}