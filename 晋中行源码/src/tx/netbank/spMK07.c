/*********************************************************************
* 版 本 号 :  
* 程序作者 :  吴海帆
* 日    期 :  2015/11/19
* 所属模块 :  手机理财产品列表查询
* 程序名称 :  spMK07.c
* 程序作用 :
* 函数列表 :
* 使用注意 :
交易配置：
insert into tx_def (TX_CODE, TX_NAME, BIT_MAP, LOG_IND, TX_TYPE, WRK_STS)
values ('5052','手机理财产品列表查询','10000000000000000000110011111111100000111111111111001000000001000111111100000000000110000000100000011000000000000000000000000000', '0', '4', '0');
insert into tx_flow_def (TX_CODE, FLOW_CODE, SUB_TX_CODE, SUB_TX_RSLT)
values ('5052', 0, 'MK07', '#$');
insert into tx_sub_def (SUB_TX_CODE, SUB_TX_NAME, TX_FUNC, ACCT_IND, CHNL_IND)
values ('MK07', '手机理财列表查询', 'spMK07', '0', '0');
***********************************************************************/
#define EXTERN
#include <string.h>
#include <stdio.h>
#include "public.h"
#include "fn_parm_c.h"			/*理财产品参数表*/
#include "fn_reg_c.h"			/*理财产品登记簿*/


int spMK07()
{
    
	struct fn_parm_c sFn_parm;   /*理财产品参数表*/
	
	char proName[62];	/*产品名称字段*/
	char purchaMark[6];	/*能否购买标志*/
	char turnPageBeginPos[10];	/*查询起始记录号*/
	char turnPageShowNum[10];	/*请求记录数*/
	char proCode[26];		/*产品代码*/
	char sortAttr[4];		/*排序字段0:期限1:收益率2:风险等级*/
	char flag[6];		/*排序标志0:正序1:倒序*/
	char cstType[6];	/*客户类型*/

	char tx_date[8+1];		/*平台交易日期*/
	char sw_traceno[10+1];		/*平台流水*/
	char filename[60];
	char pcfilename[256];
	char cFtpfile[256];
	char wherelist[1024];
	char wherebuf[256];
	FILE *fp = NULL;
	int iRt;
	int cnt=1,proSum=0;
	double dTx_amt=0.00;	/*总行剩余额*/
	double sale_amt=0.00;	/*发售金额*/
	char d_date[10];
	long l_date=0;
	int count=0;			/*产品总数*/
	double proProgress=0.00; /*产品进度*/
	
	memset(proName,0x00,sizeof(proName));
	memset(purchaMark,0x00,sizeof(purchaMark));
	memset(proCode,0x00,sizeof(proCode));
	memset(turnPageBeginPos,0x00,sizeof(turnPageBeginPos));
	memset(turnPageShowNum,0x00,sizeof(turnPageShowNum));
	memset(sortAttr,0x00,sizeof(sortAttr));
	memset(flag,0x00,sizeof(flag));
	
	memset(filename,0x00,sizeof(filename));
	memset(pcfilename,0x00,sizeof(pcfilename));
	memset(cFtpfile,0x00,sizeof(cFtpfile));
	memset(tx_date, 0x00, sizeof(tx_date));
	memset(sw_traceno, 0x00, sizeof(sw_traceno));
	memset(d_date, 0x00, sizeof(d_date));
	memset(wherelist, 0x00, sizeof(wherelist));
	memset(wherebuf, 0x00, sizeof(wherebuf));
	
	
	/**初始化公用结构**/
	if (pub_base_sysinit())
	{
	    vtcp_log("初始化公用结构错误!!");
	    strcpy(g_pub_tx.reply, "CU06");
	    goto ErrExit;
	}
	
	
	get_zd_data("0700",sortAttr);
	zip_space(sortAttr);
	get_zd_data("0230",purchaMark);
	zip_space(purchaMark);
	get_zd_data("0240",flag);
	zip_space(flag);
	get_zd_data("0260",proName);
	zip_space(proName);
	get_zd_data("0280",turnPageBeginPos);
	zip_space(turnPageBeginPos);
	get_zd_data("0290",turnPageShowNum);
	zip_space(turnPageShowNum);
	get_zd_data("0320",proCode);
	zip_space(proCode);
	get_zd_data("0660",cstType);
	zip_space(cstType);
	
	vtcp_log("%s %d ::cstType[%s]sortAttr[%s]proName[%s]purchaMark[%s]flag[%s]proCode[%s]turnPageBeginPos[%s]turnPageShowNum[%s]",\
		__FILE__,__LINE__,cstType,sortAttr,proName,purchaMark,flag,proCode,turnPageBeginPos,turnPageShowNum);
	
	if(!strlen(cstType))
	{
		strcpy(g_pub_tx.reply,"9999");
		set_zd_data("0130","客户类型不能为空");
		vtcp_log("%s %d ::客户类型不能为空[%s]",__FILE__,__LINE__,g_pub_tx.reply);
		goto ErrExit;
	}
	
	if(0 == strcmp(cstType,"0"))
	{
		strcpy(cstType,"1");	
	}
	else
	{
		strcpy(cstType,"2");
	}
	
	/*查询起始记录号*/
	if(!strlen(turnPageBeginPos))
	{
		strcpy(g_pub_tx.reply,"9999");
		set_zd_data("0130","查询起始值有误");
		vtcp_log("%s %d ::[%s]查询起始值有误turnPageBeginPos=%ld",__FILE__,__LINE__,g_pub_tx.reply,turnPageBeginPos);
		goto ErrExit;
	}
	vtcp_log("%s %d ::查询起始记录号turnPageBeginPos=%s",__FILE__,__LINE__,turnPageBeginPos);

	/*请求记录数*/	
	if(!strlen(turnPageShowNum))
	{
		strcpy(g_pub_tx.reply,"9999");
		set_zd_data("0130","请求记录数有误");
		vtcp_log("%s %d ::[%s]请求记录数有误turnPageShowNum=%s",__FILE__,__LINE__,g_pub_tx.reply,turnPageShowNum);
		goto ErrExit;
	}
	
	vtcp_log("%s %d ::请求记录数turnPageShowNum=%s",__FILE__,__LINE__,turnPageShowNum);
	
	get_zd_data("0050",tx_date);
	get_zd_data("0520",sw_traceno);
	
	vtcp_log("%s %d ::平台日期tx_date=%s",__FILE__,__LINE__,tx_date);
	vtcp_log("%s %d ::平台流水sw_traceno=%s",__FILE__,__LINE__,sw_traceno);
	
	/* 生成文件 */
	sprintf( filename,"5052mobbank%s%s",tx_date,sw_traceno );
	sprintf( pcfilename,"%s/%s",getenv("FILDIR"),filename );
	vtcp_log("%s %d:: pcfilename[%s]  ",__FILE__,__LINE__,pcfilename);
	fp=fopen(pcfilename,"w");
	if( fp==NULL )
	{
	    vtcp_log("%s %d::open file [%s] error ",__FILE__,__LINE__,pcfilename);
	    strcpy(g_pub_tx.reply,"P157");
	    set_zd_data("0130","打开文件错");
	    goto ErrExit;
	}
	/*文件头*/
	fprintf(fp, "|@产品名称|@产品代码|@期限|@风险等级|@预期收益率|@产品起息日|@产品到期日|@起购金额|@产品进度|@状态|@募集起始日|@募集结束日|\n");
	
	/*查询条件*/
	if(strlen(proCode) >0)
	{
		/*产品代码*/
		sprintf(wherelist,"prdt_code='%s'",proCode);
		vtcp_log("%s %d ::prdt_code=%s",__FILE__,__LINE__,proCode);
	}
	else
	{
		if(strlen(proName) > 0)
		{
			/*产品字段名*/
			sprintf(wherelist,"title like '%%%%%s%%%%'",proName);	
		}
		else
		{
			if(strlen(cstType) > 0)
			{
				/*客户类型*/
				sprintf(wherelist,"(cif_type='0' or cif_type='%s')",cstType);	
			}
		}
		vtcp_log("%s %d ::proName=[%s]cstType=[%s]",__FILE__,__LINE__,proName,cstType);
		
		if(strlen(purchaMark) > 0)
		{
			/*能否购买标志*/
			memset(wherebuf, 0x00, sizeof(wherebuf));
			vtcp_log("%s %d ::purchaMark=[%s]",__FILE__,__LINE__,purchaMark);
			
			/*购买标志: 0 可购买  1 不可购买*/
			if(!memcmp(purchaMark,"0",1))
			{
				sprintf(wherebuf," and stat='0' and beg_date<='%s' and end_date>='%s'",tx_date,tx_date);
					
			}	
			else
			{
				sprintf(wherebuf," and stat <> '0'");	
			}
			strcat(wherelist,wherebuf);
		}
		
		/*取当前日期前90天日期*/
		l_date=pub_base_daynumSUB(g_pub_tx.tx_date,90);
		memset(wherebuf, 0x00, sizeof(wherebuf));
		sprintf(wherebuf," and beg_date>='%d'",l_date);
		strcat(wherelist,wherebuf);
		
		if(strlen(flag) > 0)
		{
			vtcp_log("%s %d ::flag=[%s]",__FILE__,__LINE__,flag);
			vtcp_log("%s %d ::sortAttr=[%s]",__FILE__,__LINE__,sortAttr);
			
			/*排序标志  0:正序1:倒序*/
			if(memcmp(flag,"0",1) == 0)
			{
				if(strlen(sortAttr) > 0)
				{
					vtcp_log("%s %d ::sortAttr=[%s]",__FILE__,__LINE__,sortAttr);
					
					/*排序字段*/
					if(memcmp(sortAttr,"0",1) == 0)
					{
						/*期限*/
						memset(wherebuf, 0x00, sizeof(wherebuf));
						sprintf(wherebuf," order by term asc");
						strcat(wherelist,wherebuf);	
					}	
					else if(memcmp(sortAttr,"1",1) == 0)
					{
						/*收益率*/
						memset(wherebuf, 0x00, sizeof(wherebuf));
						/*sprintf(wherebuf," order by fct_rate asc"); by guanjianfeng 20151202*/
						sprintf(wherebuf," order by rate desc");
						strcat(wherelist,wherebuf);	
					}
					else
					{
						/*风险等级*/
						memset(wherebuf, 0x00, sizeof(wherebuf));
						sprintf(wherebuf," order by risk_lvl asc");
						strcat(wherelist,wherebuf);	
					}
				}
			}
			else
			{
				if(strlen(sortAttr) > 0)
				{
					vtcp_log("%s %d ::sortAttr=[%s]",__FILE__,__LINE__,sortAttr);
					
					/*排序字段*/
					if(memcmp(sortAttr,"0",1) == 0)
					{
						/*期限*/
						memset(wherebuf, 0x00, sizeof(wherebuf));
						sprintf(wherebuf," order by term desc");
						strcat(wherelist,wherebuf);	
					}	
					else if(memcmp(sortAttr,"1",1) == 0)
					{
						/*收益率*/
						memset(wherebuf, 0x00, sizeof(wherebuf));
						/*sprintf(wherebuf," order by fct_rate desc"); by guanjianfeng 20151202*/
						sprintf(wherebuf," order by rate asc");
						strcat(wherelist,wherebuf);	
					}
					else
					{
						/*风险等级*/
						memset(wherebuf, 0x00, sizeof(wherebuf));
						sprintf(wherebuf," order by risk_lvl desc");
						strcat(wherelist,wherebuf);	
					}
				}
			}
		}
	}
	
	vtcp_log("%s %d::查询条件wherelist[%s]",__FILE__,__LINE__,wherelist);
	
	/*产品总数*/
	count=sql_count("fn_parm",wherelist);
	if(count < 0)
	{
		vtcp_log("%s %d::sql_count [%d] error ",__FILE__,__LINE__,count);
	 	strcpy(g_pub_tx.reply,"D101");	
	 	fclose(fp);
	 	goto ErrExit;
	}
	set_zd_int("0490",count);
	vtcp_log("%s %d::产品总数count=%d",__FILE__,__LINE__,count);
	
	iRt = -1;
	/*查询理财产品参数表fn_parm*/
	iRt=Fn_parm_Dec_Sel(g_pub_tx.reply,wherelist);
	
	if(0 != iRt)
	{
		vtcp_log("%s %d::Fn_parm_Dec_Sel [%s] error ",__FILE__,__LINE__,iRt);
	 	strcpy(g_pub_tx.reply,"D102");
	 	set_zd_data("0130","打开游标出错");
	 	fclose(fp);
	 	goto ErrExit;
	}
	else
	{
		while(1)
		{
			memset(&sFn_parm,0x00,sizeof(sFn_parm));
			iRt=Fn_parm_Fet_Sel(&sFn_parm,g_pub_tx.reply);
			if(iRt == 1403 || iRt == 100)
			{
				vtcp_log("%s %d::无要查询的理财产品记录[%s]",__FILE__,__LINE__,iRt);
	 			break;	
			}
			else if(0 != iRt && 100 != iRt)
			{
				vtcp_log("%s %d::Fn_parm_Fet_Sel error[%s]",__FILE__,__LINE__,iRt);
				strcpy(g_pub_tx.reply,"D001");
	 			set_zd_data("#sys13","查询错误");
	 			Fn_parm_Clo_Sel();
	 			fclose(fp);
	 			goto ErrExit;
	 		}
	 		else
	 		{
	 			if(cnt < atoi(turnPageBeginPos))
	 			{
	 				cnt++;
	 				continue;	
	 			}
	 			else
	 			{
	 				double perLowest=0.00;
	 				
	 				/*最低限额*/
	 				if(0 == strcmp(cstType,"1"))
	 				{
	 					perLowest=sFn_parm.per_min_amt;	/*个人*/
	 				}
	 				else
	 				{
	 					perLowest=sFn_parm.com_min_amt;	/*对公*/
	 				}
	 				
	 				/*总行剩余额*/
	 				/*认购金额：该产品所有认购金额总和,理财产品登记簿下某一状态不为*(撤销)的产品*/
	 				iRt = -1;
	 				iRt=sql_sum_double("fn_reg","tx_amt",&dTx_amt,"prdt_code='%s' and sts <> '*'",\
	 					sFn_parm.prdt_code);
	 				vtcp_log("%s %d::认购金额dTx_amt=[%f] ",__FILE__,__LINE__,dTx_amt);
	 				
    			if( iRt )
    			{
    			 	vtcp_log("%s %d::sql_sum_double error! ret[%d] ",__FILE__,__LINE__,iRt);
    			 	strcpy(g_pub_tx.reply,"9999");
    			 	set_zd_data("0130","认购金额错误");
    				break;
    			}
    			/*总行剩余额=发售金额-认购金额总和*/
	 				dTx_amt=sFn_parm.sale_amt-dTx_amt;
	 				
	 				sale_amt=sFn_parm.sale_amt;
	 				vtcp_log("%s %d::dTx_amt[%.2f]sFn_parm.sale_amt[%.2f]sale_amt[%.2f] ",__FILE__,__LINE__,dTx_amt,sFn_parm.sale_amt,sale_amt);
	 				
	 				if(memcmp(sFn_parm.stat,"0",1) == 0)
	 				{
	 					/*产品进度=1-总行剩余额/发售金额 按购买量算*/
	 					proProgress = 1-dTx_amt / sale_amt;
	 				}
	 				else
	 				{
	 					vtcp_log("%s %d::dTx_amt[%.2f]sFn_parm.sale_amt[%.2f] ",__FILE__,__LINE__,dTx_amt,sFn_parm.sale_amt);
	 					
	 					proProgress=1;
	 				}
	 				
	 				vtcp_log("%s %d::proProgress[%.2f] ",__FILE__,__LINE__,proProgress);
	 				
	 				if(!memcmp(purchaMark,"0",1))
	 				{
	 					if(dTx_amt > 0)
	 					{
	 						/*"|@产品名称|@产品代码|@期限|@风险等级|@预期收益率|@产品起息日|@产品到期日|@起购金额|@产品进度|@状态|@募集起始日|@募集结束日|*/
	 						fprintf(fp,"|%s|%s|%d|%s|%9.2f|%d|%d|%16.2f|%9.2f|%s|%d|%d|\n",
	 							sFn_parm.title,sFn_parm.prdt_code,sFn_parm.term,sFn_parm.risk_lvl,sFn_parm.rate,\
	 							sFn_parm.ic_date,sFn_parm.mtr_date,perLowest,proProgress,sFn_parm.stat,sFn_parm.beg_date,sFn_parm.end_date);
	 						/*vtcp_log("%s %d开始打印》|%s|%s|%s|%s|%9.2f|%s|%s|%16.2|%9.2|%s|\n",
	 							sFn_parm.title,sFn_parm.prdt_code,sFn_parm.term,sFn_parm.risk_lvl,sFn_parm.rate,\
	 							sFn_parm.ic_date,sFn_parm.mtr_date,perLowest,proProgress,sFn_parm.stat);*/
	 					
	 						/*产品记录数*/
	 						proSum++;
	 						vtcp_log("%s %d::产品记录数proSum=%d",__FILE__,__LINE__,proSum);
	 						if(proSum >= atoi(turnPageShowNum))
	 						{
	 							vtcp_log("%s %d::产品记录数最大",__FILE__,__LINE__);
	 							break;	
	 						}
	 					}
	 					else
	 					{
	 						continue;	
	 					}
	 				}
	 				else
	 				{
	 					/*"|@产品名称|@产品代码|@期限|@风险等级|@预期收益率|@产品起息日|@产品到期日|@起购金额|@产品进度|@状态|@募集起始日|@募集结束日|*/
	 						fprintf(fp,"|%s|%s|%d|%s|%9.2f|%d|%d|%16.2f|%9.2f|%s|%d|%d|\n",
	 							sFn_parm.title,sFn_parm.prdt_code,sFn_parm.term,sFn_parm.risk_lvl,sFn_parm.rate,\
	 							sFn_parm.ic_date,sFn_parm.mtr_date,perLowest,proProgress,sFn_parm.stat,sFn_parm.beg_date,sFn_parm.end_date);
	 						/*vtcp_log("%s %d开始打印》|%s|%s|%s|%s|%9.2f|%s|%s|%16.2|%9.2|%s|\n",
	 							sFn_parm.title,sFn_parm.prdt_code,sFn_parm.term,sFn_parm.risk_lvl,sFn_parm.rate,\
	 							sFn_parm.ic_date,sFn_parm.mtr_date,perLowest,proProgress,sFn_parm.stat);
	 					*/
	 					/*产品记录数*/
	 					proSum++;
	 					vtcp_log("%s %d::产品记录数proSum=%d",__FILE__,__LINE__,proSum);
	 					if(proSum >= atoi(turnPageShowNum))
	 					{
	 						vtcp_log("%s %d::产品记录数最大",__FILE__,__LINE__);
	 						break;	
	 					}	
	 				}	
	 			}
	 		}
		}	
	}
	Fn_parm_Clo_Sel();
	fclose(fp);
	
	/*ftp传输文件*/
	iRt = -1;
	vtcp_log("%s,%d filename=[%s]\n",__FILE__,__LINE__,filename);
	set_zd_data("0260",filename);
	
	sprintf(cFtpfile,"%s/bin/ftp_to_nsw.sh %s %s %s",getenv("WORKDIR"),get_env_info("NSW_FTP_USER"),\
		get_env_info("NSW_FTP_PWD"),filename);
	vtcp_log("%s,%d cFtpfile=[%s]\n",__FILE__,__LINE__,cFtpfile);
	iRt=system(cFtpfile);
	if (iRt)
	{
		vtcp_log("[%s][%d] ftp文件[%s]到平台出错[%d]", __FILE__, __LINE__,filename,iRt);
		strcpy(g_pub_tx.reply,"9999");
		set_zd_data("0130","文件传输出错");
		goto ErrExit;
	}
	
	vtcp_log("[%s][%d] ftp文件[%s]到平台成功", __FILE__, __LINE__,filename);
	
   
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	set_zd_data("0120","0000");
	set_zd_data("0130","理财产品列表查询成功！");
	vtcp_log("success !!: 5050 reply is[%s]", g_pub_tx.reply);
	return 0;
ErrExit:
	set_zd_data("0120",g_pub_tx.reply);
	vtcp_log("fail !!: reply is[%s]", g_pub_tx.reply);
	return 1;
}

