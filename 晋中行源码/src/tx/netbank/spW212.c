/*********************************************************************
* 版 本 号 :  
* 程序作者 :  lixiang
* 日    期 :  2014/11/01
* 所属模块 :  贷款还款记录明细查询
* 程序名称 :  spW212.c
* 程序作用 :
* 函数列表 :
* 使用注意 :
交易配置：
insert into tx_def values ('5063', '贷款还款记录明细查询', '10000000000000000000110010010011100000111111111110000000000001000111111100000000000110000000100000011000000000000000000000000000', '0','5', '2');
insert into tx_flow_def values ('5063', 0, 'W212', '#$');
insert into tx_sub_def values ('W212', '贷款还款记录明细查询', 'spW212', '0', '0');
***********************************************************************/
#define EXTERN
#include <string.h>
#include <stdio.h>
#include "public.h"
#include "mdm_ac_rel_c.h"
#include "ln_mst_c.h"
#include "ln_parm_c.h"
#include "ln_mst_hst_c.h"

int spW212()
{
	struct mdm_ac_rel_c mdm_ac_rel_1;	/*介质与账户关系*/
	struct mdm_ac_rel_c mdm_ac_rel_2;	/*介质与账户关系*/
	struct ln_mst_c	ln_mst_1;		/*贷款主文件*/
	struct ln_mst_hst_c ln_mst_hst_1;	/*贷款明细*/
	
	char loanAccount[22];		/*贷款账号*/
	char quryType[2];		/*查询类型*/
	char beginDate[10];		/*开始日期*/
	char endDate[10];		/*截止日期*/
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
	int Product=0;
	int recordNo_1=0;
	int recordNo=0;		/*记录数*/
	
	memset(&mdm_ac_rel_1,0x00,sizeof(mdm_ac_rel_1));
	memset(&mdm_ac_rel_2,0x00,sizeof(mdm_ac_rel_2));
	memset(&ln_mst_1,0x00,sizeof(ln_mst_1));
	memset(&ln_mst_hst_1,0x00,sizeof(ln_mst_hst_1));
	memset(loanAccount,0x00,sizeof(loanAccount));
	memset(quryType,0x00,sizeof(quryType));
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
	
	/**初始化公用结构**/
	if (pub_base_sysinit())
	{
	    vtcp_log("初始化公用结构错误!!");
	    strcpy(g_pub_tx.reply, "CU06");
	    goto ErrExit;
	}
	
	get_zd_data("0380",loanAccount);
	get_zd_data("0050",tx_date);
	get_zd_data("0520",sw_traceno);
	get_zd_data("0440",beginDate);
	get_zd_data("0450",endDate);
	vtcp_log("%s,%d 贷款账号=[%s]\n",__FILE__,__LINE__,loanAccount);
	vtcp_log("%s %d ::平台日期tx_date=%s",__FILE__,__LINE__,tx_date);
	vtcp_log("%s %d ::平台流水sw_traceno=%s",__FILE__,__LINE__,sw_traceno);
	vtcp_log("%s %d ::开始日期beginDate=%s",__FILE__,__LINE__,beginDate);
	vtcp_log("%s %d ::截止日期endDate=%s",__FILE__,__LINE__,endDate);
	
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
	sprintf( filename,"5063netbank%s%s",tx_date,sw_traceno );
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
	fprintf(fp, "贷款账号|贷款户名|交易日期|交易时间|还款金额|余额|现转标志|还款账号|摘要|\n");
	
	/*根据贷款账号获取贷款账号ID;mdm_ac_rel_1.ac_id*/
	ret = -1;
	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&mdm_ac_rel_1,"ac_no='%s'",loanAccount);
	if(ret)
	{
		vtcp_log("%s %d::Mdm_ac_rel_Sel [%d] error ",__FILE__,__LINE__,ret);
		strcpy(g_pub_tx.reply,"W011");
		goto ErrExit;	
	}
	
	/*通过贷款账户ID获取还款账号ID和贷款户名:ln_mst_1.repay_ac_id，ln_mst_1.name*/
	ret=-1;
	ret=Ln_mst_Sel(g_pub_tx.reply,&ln_mst_1,"ac_id='%d'",mdm_ac_rel_1.ac_id);
	if(ret==100)
	{
		set_zd_data("0120","D004");
		set_zd_data("0130","该账号为非贷款账号");
		goto ErrExit;
	}
	else if(ret)
	{
		vtcp_log("%s %d::Mdm_ac_rel_Sel [%d] error ",__FILE__,__LINE__,ret);
		strcpy(g_pub_tx.reply,"W011");
		goto ErrExit;	
	}
	
	/*通过还款账号ID获取还款账号mdm_ac_rel_2.ac_no*/
	ret = -1;
	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&mdm_ac_rel_2,"ac_id='%d'",ln_mst_1.repay_ac_id);
	if(ret)
	{
		vtcp_log("%s %d::Mdm_ac_rel_Sel [%d] error ",__FILE__,__LINE__,ret);
		strcpy(g_pub_tx.reply,"W011");
		goto ErrExit;	
	}
	
	/*==================生成查询条件whereList*=================*/
	
	/*还款时增减标志位0*/
	sprintf(buf,"ac_id='%ld' and add_ind='0' ",mdm_ac_rel_1.ac_id);
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
	sprintf( buf,"  and tx_date<='%s' ",endDate );
	strcat( whereList,buf);
	}
	
	/*获取查询类型*/
	get_zd_data("0310",quryType);
	if(strlen(quryType))
	{
		if(strcmp(quryType,"1")==0)
		{
			memset(buf, 0x00, sizeof(buf));
			sprintf( buf,"  and ln_tx_type='0' order by tx_date desc , tx_time desc");
			strcat( whereList,buf);
		}
		else if(strcmp(quryType,"2")==0)
		{
			memset(buf, 0x00, sizeof(buf));
			sprintf( buf,"  and ln_tx_type in ('1','2') order by tx_date desc , tx_time desc");
			strcat( whereList,buf);	
		}
		else
		{
			strcpy(g_pub_tx.reply,"9999");
			set_zd_data("0130","查询类型有误");
			vtcp_log("%s %d ::[%s]查询类型有误turnPageBeginPos=%ld",__FILE__,__LINE__,g_pub_tx.reply,quryType);
			goto ErrExit;
			
		}
	}	
	
	/*通过贷款账户ID获取还款记录明细*/
	recordNo_1=sql_count("ln_mst_hst",whereList);
		set_zd_int("0450",recordNo_1);
		vtcp_log("[%s][%d]::贷款还款记录明细记录总数为[%d]",__FILE__,__LINE__,recordNo_1);
		if ( recordNo_1 == 0 )
		{
			vtcp_log("[%s][%d]::贷款还款记录明细记录总数为0,直接返回.",__FILE__,__LINE__);
			fclose(fp);
			goto OkExit;	
		}
	ret = -1;
	ret=Ln_mst_hst_Dec_Sel(g_pub_tx.reply,whereList);
	if(ret)
	{
		vtcp_log("%s %d::Ln_mst_Dec_Sel [%d] error ",__FILE__,__LINE__,ret);
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
			ret=Ln_mst_hst_Fet_Sel(&ln_mst_hst_1,g_pub_tx.reply);
			if(ret == 100)
			{
				vtcp_log("%s %d::取活期存款明细游标错误[%d]",__FILE__,__LINE__,ret);
				break;	
			}
			 else if(0 != ret && 100 != ret)
			{
				vtcp_log("%s %d::Ln_mst_Fet_Sel error[%d]",__FILE__,__LINE__,ret);
				strcpy(g_pub_tx.reply,"D001");
				set_zd_data("0130","查询错误");
				Ln_mst_hst_Clo_Sel();
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
	 			if(!strcmp(ln_mst_hst_1.ct_ind,"1"))
	 			{
	 				/*写文件:贷款账号|贷款户名|交易日期|交易时间|还款金额|余额|现转标志|还款账号|摘要|*/
					fprintf(fp, "%20s|%60s|%ld|%ld|%16.2f|%16.2f|%2s|%20s|%25s|\n",\
					loanAccount,ln_mst_1.name,ln_mst_hst_1.tx_date,ln_mst_hst_1.tx_time,ln_mst_hst_1.tx_amt,\
					ln_mst_hst_1.bal,ln_mst_hst_1.ct_ind,"现金还款",ln_mst_hst_1.brf);
	 					
	 			}
	 			else
	 			{
					/*写文件:贷款账号|贷款户名|交易日期|交易时间|还款金额|余额|现转标志|还款账号|摘要|*/
					fprintf(fp, "%20s|%60s|%ld|%ld|%16.2f|%16.2f|%2s|%20s|%25s|\n",\
					loanAccount,ln_mst_1.name,ln_mst_hst_1.tx_date,ln_mst_hst_1.tx_time,ln_mst_hst_1.tx_amt,\
					ln_mst_hst_1.bal,ln_mst_hst_1.ct_ind,mdm_ac_rel_2.ac_no,ln_mst_hst_1.brf);
				}
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
	Ln_mst_hst_Clo_Sel();
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
	set_zd_data("0130"," 贷款还款记录明细查询成功");
	vtcp_log("success !!: 5063 reply is[%s]", g_pub_tx.reply);
	return 0;
ErrExit:
	vtcp_log("fail !!: reply is[%s]", g_pub_tx.reply);
	return 1;
}
