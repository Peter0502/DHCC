/*********************************************************************
* 版 本 号 :  
* 程序作者 :  lixiang
* 日    期 :  2015/01/06
* 所属模块 :  交易明细查询
* 程序名称 :  spMK03.c
* 程序作用 :
* 函数列表 :
* 使用注意 :
交易配置：
insert into tx_def values ('5023', '交易明细查询', '10000000000000000000110011010011100000111111111110000000000001000111111100000000000110000000100000011000000000000000000000000000', '0','5', '0');
insert into tx_flow_def values ('5023', 0, 'MK03', '#$');
insert into tx_sub_def values ('MK03', '贷款还款记录明细查询', 'spMK03', '0', '0');
***********************************************************************/
#define EXTERN
#include <string.h>
#include <stdio.h>
#include "public.h"
#include "mdm_ac_rel_c.h"
#include "com_branch_c.h"
#include "dd_mst_c.h"
#include "dd_mst_hst_c.h"

int spMK03()
{
	struct mdm_ac_rel_c mdm_ac_rel_1;	/*介质与账户关系*/
	struct dd_mst_hst_c dd_mst_hst_1;	/*活期存款明细*/
	struct com_branch_c com_branch_1;	/*公共机构码表*/
	struct dd_mst_c dd_mst_1;		/*活期存款主文件*/
	
	char accountNo[22];		/*账号*/
	char beginDate[10];		/*交易开始日期*/
	char endDate[10];		/*交易截止日期*/
	char turnPageBeginPos[10];	/*查询起始记录号*/
	char turnPageShowNum[10];	/*请求记录数*/
	char sw_traceno[12];		/*平台流水*/
	char tx_date[12];		/*平台日期*/
	char whereList[1024];		/*查询条件*/
	char buf[256];
	char filename[60];
	char pcfilename[256];
	char cFtpfile[256];
	FILE *fp=NULL;
	int ret;
	int cnt=1;
	double totalOutput;		/*汇出总额*/
	double totalInput;		/*汇入总额*/
	int recordNo_1=0;		/*总笔数*/
	int recordNo=0;			/*记录数*/
	char add_ind[1+1];		/*借贷标志*/
	char bz[4];			/*币种*/
	
	memset(&mdm_ac_rel_1,0x00,sizeof(mdm_ac_rel_1));
	memset(&dd_mst_hst_1,0x00,sizeof(dd_mst_hst_1));
	memset(&dd_mst_1,0x00,sizeof(dd_mst_1));
	memset(&com_branch_1,0x00,sizeof(com_branch_1));
	memset(accountNo,0x00,sizeof(accountNo));
	memset(beginDate,0x00,sizeof(beginDate));
	memset(endDate,0x00,sizeof(endDate));
	memset(turnPageBeginPos,0x00,sizeof(turnPageBeginPos));
	memset(turnPageShowNum,0x00,sizeof(turnPageShowNum));
	memset(sw_traceno, 0x00, sizeof(sw_traceno));
	memset(tx_date, 0x00, sizeof(tx_date));
	memset(whereList, 0x00, sizeof(whereList));
	memset(buf, 0x00, sizeof(buf));
	memset(filename,0x00,sizeof(filename));
	memset(pcfilename,0x00,sizeof(pcfilename));
	memset(cFtpfile,0x00,sizeof(cFtpfile));
	memset(add_ind,0x00,sizeof(add_ind));
	memset(bz,0x00,sizeof(bz));
	strcpy(bz, "CNY");
	
	/**初始化公用结构**/
	if (pub_base_sysinit())
	{
	    vtcp_log("初始化公用结构错误!!");
	    strcpy(g_pub_tx.reply, "CU06");
	    goto ErrExit;
	}
	
	get_zd_data("0380",accountNo);
	get_zd_data("0050",tx_date);
	get_zd_data("0520",sw_traceno);
	get_zd_data("0440",beginDate);
	get_zd_data("0450",endDate);
	vtcp_log("%s,%d 账号=[%s]\n",__FILE__,__LINE__,accountNo);
	vtcp_log("%s %d ::平台日期tx_date=%s",__FILE__,__LINE__,tx_date);
	vtcp_log("%s %d ::平台流水sw_traceno=%s",__FILE__,__LINE__,sw_traceno);
	vtcp_log("%s %d ::交易开始日期beginDate=%s",__FILE__,__LINE__,beginDate);
	vtcp_log("%s %d ::交易截止日期endDate=%s",__FILE__,__LINE__,endDate);
	
	/*查询起始记录号*/
	get_zd_data("0280",turnPageBeginPos);
	if(!strlen(turnPageBeginPos))
	{
		strcpy(g_pub_tx.reply,"9999");
		set_zd_data("0130","查询起始值有误");
		vtcp_log("%s %d ::[%s]查询起始值有误turnPageBeginPos=%ld",__FILE__,__LINE__,g_pub_tx.reply,turnPageBeginPos);
		goto ErrExit;
	}
	vtcp_log("%s %d ::查询起始记录号turnPageBeginPos=%s",__FILE__,__LINE__,turnPageBeginPos);

	/*请求记录数*/
	get_zd_data("0290",turnPageShowNum);	
	if(!strlen(turnPageShowNum))
	{
		strcpy(g_pub_tx.reply,"9999");
		set_zd_data("0130","请求记录数有误");
		vtcp_log("%s %d ::[%s]请求记录数有误turnPageShowNum=%s",__FILE__,__LINE__,g_pub_tx.reply,turnPageShowNum);
		goto ErrExit;
	}
	/* 生成文件 */
	sprintf( filename,"1040mobbank%s%s",tx_date,sw_traceno );
	sprintf( pcfilename,"%s/%s",getenv("FILDIR"),filename );
	vtcp_log("%s %d:: pcfilename[%s]  ",__FILE__,__LINE__,pcfilename);
	fp=fopen(pcfilename,"w");
	if( fp==NULL )
	{
	    vtcp_log("%s %d::open file [%s] error ",__FILE__,__LINE__,pcfilename);
	    strcpy(g_pub_tx.reply,"P157");
	    goto ErrExit;
	}
	/*文件头*/
	fprintf(fp, "账号|账号名称|开户机构|账号开户行名称|币种|借贷方向|发生额|余额|交易日期|交易时间|交易流水号|摘要|\n");
	
	/*根据账号获取账号ID;mdm_ac_rel_1.ac_id*/
	ret = -1;
	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&mdm_ac_rel_1,"ac_no='%s'",accountNo);
	if(ret)
	{
		vtcp_log("%s %d::Mdm_ac_rel_Sel [%d] error ",__FILE__,__LINE__,ret);
		strcpy(g_pub_tx.reply,"W011");
		goto ErrExit;	
	}
	
	/*通过账户ID获取账号名称和开户机构号:dd_mst_1.name，dd_mst_1.opn_br_no*/
	ret=-1;
	ret=Dd_mst_Sel(g_pub_tx.reply,&dd_mst_1,"ac_id='%d'",mdm_ac_rel_1.ac_id);
	if(ret==100)
	{
		set_zd_data("0120","D004");
		set_zd_data("0130","该账号不存在");
		goto ErrExit;
	}
	else if(ret)
	{
		vtcp_log("%s %d::Dd_mst_Sel [%d] error ",__FILE__,__LINE__,ret);
		strcpy(g_pub_tx.reply,"W011");
		set_zd_data("0130","查询介质与账户关系表出错");
		goto ErrExit;	
	}
	
	/*通过开户机构号获取开户机构名称com_branch_1.br_name*/
	ret = -1;
	ret=Com_branch_Sel(g_pub_tx.reply,&com_branch_1,"br_no='%s'",dd_mst_1.opn_br_no);
	if(ret)
	{
		vtcp_log("%s %d::Com_branch_Sel [%d] error ",__FILE__,__LINE__,ret);
		strcpy(g_pub_tx.reply,"W011");
		goto ErrExit;	
	}
	
	
	/*==================生成查询条件whereList*=================*/
	
	sprintf(buf,"ac_id='%ld' ",mdm_ac_rel_1.ac_id);
	strcat( whereList,buf);
	
	/*查询开始日期*/
	if(strlen(beginDate))
	{
	memset(buf, 0x00, sizeof(buf));
	sprintf( buf,"  and tx_date>='%s' ",beginDate );
	strcat( whereList,buf);
	}
	
	/*查询截止日期*/
	if(strlen(endDate))
	{
	memset(buf, 0x00, sizeof(buf));
	sprintf( buf,"  and tx_date<='%s' order by tx_date desc,trace_no desc,trace_cnt desc,tx_time desc ",endDate );
	strcat( whereList,buf);
	}
	vtcp_log("[%s][%d]::查询条件[%s].",__FILE__,__LINE__,whereList);
	
	/*计算汇出款总额*/
	ret=-1;
	ret = sql_sum_double("dd_mst_hst","tx_amt",&totalOutput,"add_ind='0' and %s ",whereList);
 	if(ret)
	{
		vtcp_log("统计dd_mst_hst错误!");
		strcpy(g_pub_tx.reply, "D331");
		goto ErrExit;
	}
	set_zd_double("0390",totalOutput);
	
	/*计算汇入款总额*/
	ret=-1;
	ret = sql_sum_double("dd_mst_hst","tx_amt",&totalInput,"add_ind='1' and %s ",whereList);
 	if(ret)
	{
		vtcp_log("统计dd_mst_hst错误!");
		strcpy(g_pub_tx.reply, "D331");
		goto ErrExit;
	}
	set_zd_double("0400",totalInput);
	
	/*通过活期村存款账户账户ID获取交易记录明细*/
	recordNo_1=sql_count("dd_mst_hst",whereList);
	set_zd_int("0450",recordNo_1);
	vtcp_log("[%s][%d]::贷款还款记录明细记录总数为[%d]",__FILE__,__LINE__,recordNo_1);
	if ( recordNo_1 == 0 )
	{
		vtcp_log("[%s][%d]::贷款还款记录明细记录总数为0,直接返回.",__FILE__,__LINE__);
		fclose(fp);
		goto OkExit;	
	}
	ret = -1;
	ret=Dd_mst_hst_Dec_Sel(g_pub_tx.reply,whereList);
	if(ret)
	{
		vtcp_log("%s %d::Dd_mst_Dec_Sel [%d] error ",__FILE__,__LINE__,ret);
	 	strcpy(g_pub_tx.reply,"D102");
	 	set_zd_data("0130","定义游标出错");
	 	fclose(fp);
	 	goto ErrExit;		
	}
	else
	{
		while(1)
		{	
			ret=-1;
			ret=Dd_mst_hst_Fet_Sel(&dd_mst_hst_1,g_pub_tx.reply);
			if(ret != 100 && ret != 0)
			{
				vtcp_log("%s %d::取活期存款明细游标错误[%d]",__FILE__,__LINE__,ret);
				strcpy(g_pub_tx.reply,"D330");
				set_zd_data("0130","查找活期交易明细表报错");	
				Dd_mst_hst_Clo_Sel();
				goto ErrExit;	
			}
			
			else if(ret == 100 || ret == 1403)
			{
				break;
			}
			else
	 		{
	 			if(cnt < atoi(turnPageBeginPos))
	 			{
	 				cnt++;
	 				continue;	
	 			}
	 			if (dd_mst_hst_1.add_ind[0] == '0')
	 			{
					
					sprintf(add_ind,"%s","0"); /*借 */ 
				}
				else
				{
					sprintf(add_ind,"%s","1"); /*贷*/
				}		
				/*写文件:账号|账号名称|开户机构|账号开户行名称|币种|借贷方向|发生额|余额|交易日期|交易时间|交易流水号|摘要|*/
				fprintf(fp, "%33s|%60s|%6s|%60s|%4s|%s|%15.2f|%15.2f|%ld|%ld|%ld|%33s|\n",\
				accountNo,dd_mst_1.name,dd_mst_1.opn_br_no,com_branch_1.br_name,bz,add_ind,dd_mst_hst_1.tx_amt,\
				dd_mst_hst_1.bal,dd_mst_hst_1.tx_date,dd_mst_hst_1.tx_time,dd_mst_hst_1.trace_no,dd_mst_hst_1.brf);
				/*记录数*/
	 			++recordNo; 
	 			vtcp_log("%s %d::记录数recordNo=%d",__FILE__,__LINE__,recordNo);
	 			if(recordNo >= atoi(turnPageShowNum))
	 			{
	 				break;	
	 			}
			}
		}
	}
	Dd_mst_hst_Clo_Sel();
	fclose(fp);
	fp=NULL;
	
	/*ftp传输文件*/
	vtcp_log("%s,%d filename=[%s]\n",__FILE__,__LINE__,filename);
	set_zd_data("0260",filename);
	
	sprintf(cFtpfile,"%s/bin/ftp_to_nsw.sh %s %s %s",getenv("WORKDIR"),get_env_info("NSW_FTP_USER"),\
		get_env_info("NSW_FTP_PWD"),filename);
	vtcp_log("%s,%d cFtpfile=[%s]\n",__FILE__,__LINE__,cFtpfile);
	ret = -1;
	ret=system(cFtpfile);
	if (ret)
	{
		vtcp_log("[%s][%d] ftp文件[%s]到平台出错[%d]", __FILE__, __LINE__,filename,ret);
		strcpy(g_pub_tx.reply,"9999");
		set_zd_data("0130","文件传递出错");
		goto ErrExit;
	}
	
	vtcp_log("[%s][%d] ftp文件[%s]到平台成功", __FILE__, __LINE__,filename);
	
   
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	set_zd_data("0120","0000");
	set_zd_data("0130"," 明细查询交易成功");
	vtcp_log("success !!: 1040 reply is[%s]", g_pub_tx.reply);
	return 0;
ErrExit:
	vtcp_log("fail !!: reply is[%s]", g_pub_tx.reply);
	if(fp != NULL)
	{
		fclose(fp);
		fp=NULL;	
	}
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

