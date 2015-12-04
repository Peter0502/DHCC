/*********************************************************************
* 版 本 号 :  
* 程序作者 :  吴海帆
* 日    期 :  2014/11/01
* 所属模块 :  付款账号与贷款账号关联查询
* 程序名称 :  spW220.c
* 程序作用 :
* 函数列表 :
* 使用注意 :
交易配置：
insert into tx_def values ('5060', '付款账号与贷款账号关联查询', '10000000000000000000000111011110000000000000001110000000000000000000000000000000000000000000000000000000000000000000000000000000', '0','4', '2');
insert into tx_flow_def values ('5060', 0, 'W220', '#$');
insert into tx_sub_def values ('W220', '付款账号与贷款账号关联查询', 'spW220', '0', '0');
***********************************************************************/
#define EXTERN
#include <string.h>
#include <stdio.h>
#include "public.h"
#include "mdm_ac_rel_c.h"
#include "ln_mst_c.h"
#include "ln_parm_c.h"

int spW220()
{
	struct mdm_ac_rel_c d_mdm_ac_rel;	/*介质与账户关系*/
	struct ln_mst_c	d_ln_mst;		/*贷款主文件*/
	
	char payAccount[32+1];		/*还款账号*/
	char clearType[1+1];		/*清算标志*/
	char tx_date[8+1];		/*平台日期*/
	char sw_traceno[10+1];		/*平台流水*/
	
	char turnPageBeginPos[10];		/*查询起始记录号*/
	char turnPageShowNum[10];		/*请求记录数*/
	
	char filename[60];
	char pcfilename[256];
	char cFtpfile[256];
	FILE *fp=NULL;
	
	double debitInterest=0.00;	/*已欠利息*/
	double OpenInterest=0.00;	/*未结利息*/
	double vbjlx=0.00,vbnlx=0.00,vbylx=0.00,vfllx=0.00;
		/*本金积数利息、表内欠息复利、表外欠息复利、复利欠息复利*/
	
	int ret;
	int recordNo=0;		/*记录数*/
	int count=0;		/*贷款主文件总记录数*/
	int cnt=1;		/*查询条数*/
	
	memset(&d_mdm_ac_rel,0x00,sizeof(d_mdm_ac_rel));
	memset(&d_ln_mst,0x00,sizeof(d_ln_mst));
	memset(payAccount,0x00,sizeof(payAccount));
	memset(clearType,0x00,sizeof(clearType));
	memset(tx_date, 0x00, sizeof(tx_date));
	memset(sw_traceno, 0x00, sizeof(sw_traceno));
	memset(filename,0x00,sizeof(filename));
	memset(pcfilename,0x00,sizeof(pcfilename));
	memset(cFtpfile,0x00,sizeof(cFtpfile));
	memset(turnPageBeginPos,0x00,sizeof(turnPageBeginPos));
	memset(turnPageShowNum,0x00,sizeof(turnPageShowNum));
	
	/**初始化公用结构**/
	if (pub_base_sysinit())
	{
	    vtcp_log("初始化公用结构错误!!");
	    strcpy(g_pub_tx.reply, "CU06");
	    goto ErrExit;
	}
	
	get_zd_data("0300",payAccount);
	if(!strlen(payAccount))
	{
		strcpy(g_pub_tx.reply, "9999");
		set_zd_data("0120","9999");
		set_zd_data("0130","关键域'还款账号'为空");
		return 1;
	}
	vtcp_log("%s,%d payAccount=[%s]\n",__FILE__,__LINE__,payAccount);
	get_zd_data("0660",clearType);
	
	get_zd_data("0280",turnPageBeginPos);
	if(!strlen(turnPageBeginPos))
	{
		strcpy(g_pub_tx.reply, "9999");
		set_zd_data("0120","9999");
		set_zd_data("0130","关键域'查询起始记录号'为空");
		return 1;
	}
	
	get_zd_data("0290",turnPageShowNum);
	if(!strlen(turnPageBeginPos))
	{
		strcpy(g_pub_tx.reply, "9999");
		set_zd_data("0120","9999");
		set_zd_data("0130","关键域'请求记录数'为空");
		return 1;
	}
	
	get_zd_data("0050",tx_date);
	get_zd_data("0520",sw_traceno);
	vtcp_log("%s %d ::平台日期tx_date=%s",__FILE__,__LINE__,tx_date);
	vtcp_log("%s %d ::平台流水sw_traceno=%s",__FILE__,__LINE__,sw_traceno);
	
	/*通过还款账号取还款账户ID---介质与账户关系*/
	ret = -1;
	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&d_mdm_ac_rel,"ac_no='%s'",payAccount);
	if(ret == 100 || ret == 1403)
	{
		strcpy(g_pub_tx.reply,"D137");
		set_zd_data("0120","D137");
		set_zd_data("0130","还款账号不存在");
		goto ErrExit;	
	}
	else if(ret)
	{
		vtcp_log("%s %d::Mdm_ac_rel_Sel [%d] error ",__FILE__,__LINE__,ret);
		strcpy(g_pub_tx.reply,"W011");
		set_zd_data("0120","W011");
		set_zd_data("0130","查询介质与账户关系表出错");
		goto ErrExit;	
	}
	
	/* 生成文件 */
	sprintf( filename,"5060netbank%s%s",tx_date,sw_traceno );
	sprintf( pcfilename,"%s/%s",getenv("FILDIR"),filename );
	vtcp_log("%s %d:: pcfilename[%s]  ",__FILE__,__LINE__,pcfilename);
	fp=fopen(pcfilename,"w");
	if( fp==NULL )
	{
	    vtcp_log("%s %d::open file [%s] error ",__FILE__,__LINE__,pcfilename);
	    strcpy(g_pub_tx.reply,"P157");
	    set_zd_data("0120","P157");
	    set_zd_data("0130","打开文件出错");
	    goto ErrExit;
	}
	/*文件头*/
	fprintf(fp, "贷款账号|贷款户名|贷款余额|已欠利息|未结利息|\n");
	
	/* 根据还款账户ID查询贷款主文件个数 */
	count = sql_count("ln_mst", "repay_ac_id='%ld' and ac_sts in('1','2','3','4','5')" , d_mdm_ac_rel.ac_id);
	if(count < 0)
	{
		vtcp_log("%s %d::sql_count [%d] error ",__FILE__,__LINE__,count);
	 	strcpy(g_pub_tx.reply,"D101");
	 	set_zd_data("0120","D101");
	 	fclose(fp);	
	 	goto ErrExit;
	}
	set_zd_int("0490",count);
	vtcp_log("%s %d::产品总数count=%d",__FILE__,__LINE__,count);
	
	/*通过还款账户ID取贷款账户信息---贷款主文件*/
	ret = -1;
	ret=Ln_mst_Dec_Sel(g_pub_tx.reply,"repay_ac_id='%ld' and ac_sts in('1','2','3','4','5')",d_mdm_ac_rel.ac_id);
	if(ret)
	{
		vtcp_log("%s %d::Ln_mst_Dec_Sel [%d] error ",__FILE__,__LINE__,ret);
	 	strcpy(g_pub_tx.reply,"D102");
	 	set_zd_data("0120","D102");
	 	set_zd_data("0130","打开游标出错");
	 	fclose(fp);
	 	goto ErrExit;		
	}
	
	while(1)
	{		
		memset(&d_ln_mst,0x00,sizeof(d_ln_mst));
		memset(&d_mdm_ac_rel,0x00,sizeof(d_mdm_ac_rel));
		
		memset(&g_ln_mst,0x00,sizeof(g_ln_mst));
		memset(&g_ln_parm,0x00,sizeof(g_ln_parm));
		memset(&g_mdm_ac_rel,0x00,sizeof(g_mdm_ac_rel));
		
		ret = -1;
		ret = Ln_mst_Fet_Sel(&d_ln_mst,g_pub_tx.reply);	
		if(ret == 1403 || ret == 100)
		{
			vtcp_log("%s %d::不存在要查询的账号[%d]",__FILE__,__LINE__,ret);
			break;	
		}
		 else if(0 != ret && 100 != ret)
		{
			vtcp_log("%s %d::Ln_mst_Fet_Sel error[%d]",__FILE__,__LINE__,ret);
			strcpy(g_pub_tx.reply,"D001");
			set_zd_data("0120","D001");
			set_zd_data("0130","查询错误");
			Ln_mst_Clo_Sel();
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
				/*通过贷款账号ID取贷款账号---介质与账户关系*/
				ret = -1;
				ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&d_mdm_ac_rel,"ac_id='%ld'",d_ln_mst.ac_id);
				if(ret)
				{
					vtcp_log("%s %d::Mdm_ac_rel_Sel [%d] error ",__FILE__,__LINE__,ret);
					strcpy(g_pub_tx.reply,"W011");
					set_zd_data("0120","W011");
					Ln_mst_Clo_Sel();
					fclose(fp);
					goto ErrExit;	
				}
				
				/* 取贷款参数信息 */
				ret = -1;
				ret = Ln_parm_Sel(g_pub_tx.reply,&g_ln_parm," prdt_no='%s' " ,d_ln_mst.prdt_no );
				if( ret==100 )
				{
					sprintf(acErrMsg,"贷款参数文件中无此记录 [%s][%d]",d_ln_mst.prdt_no,ret);
					WRITEMSG
					strcpy( g_pub_tx.reply ,"L001" );
					set_zd_data("0120","L001");
					Ln_mst_Clo_Sel();
					fclose(fp);
					return 1;
				} 
				else if( ret )
				{
					sprintf( acErrMsg,"取贷款参数文件异常 [%s][%d]",d_ln_mst.prdt_no,ret);
					WRITEMSG
					strcpy( g_pub_tx.reply ,"L001" );
					set_zd_data("0120","L001");
					Ln_mst_Clo_Sel();
					fclose(fp);
					return -1;
				}
				
				/* 检查贷款类型是否是按揭贷款 */
				if ( g_ln_parm.ln_pay_type[0]=='3' || g_ln_parm.ln_pay_type[0]=='4' )
				{
					vtcp_log("%s %d:: 此贷款为按揭贷款,不能在此交易![%s]",__FILE__,__LINE__,g_ln_parm.ln_pay_type);
					continue;
					
				}
				vtcp_log("%s %d:: 检查贷款类型是否是普通贷款",__FILE__,__LINE__);	
				
				vbjlx=vbnlx=vbylx=vfllx=0.00;
				
				vtcp_log("--------本金积数计算利息:bjlx----------");
				ret = -1;
				ret = pub_ln_CalIntst( d_ln_mst ,g_ln_parm ,"0",&vbjlx,1 );
				if( ret ) 
				{
					vtcp_log("%s %d::pub_ln_CalIntst error[%d]",__FILE__,__LINE__,ret);
					strcpy(g_pub_tx.reply,"A032");
					set_zd_data("0120","A032");
					set_zd_data("0130","本金积数计算利息错误");
					Ln_mst_Clo_Sel();
					fclose(fp);
					goto ErrExit; 
				 }
					
				vtcp_log("--------表内欠息积数计算复利:bnlx----------");
				ret = -1;
				ret = pub_ln_CalIntst( d_ln_mst ,g_ln_parm ,"1",&vbnlx,1 );
				if( ret ) 
				{ 
					vtcp_log("%s %d::pub_ln_CalIntst error[%d]",__FILE__,__LINE__,ret);
					strcpy(g_pub_tx.reply,"A032");
					set_zd_data("0120","A032");
					set_zd_data("0130","表内欠息积数计算复利错误");
					Ln_mst_Clo_Sel();
					fclose(fp);
					goto ErrExit; 
				 }	
				
				vtcp_log("--------表外欠息积数计算复利:bylx----------");
				ret = -1;
				ret = pub_ln_CalIntst( d_ln_mst ,g_ln_parm ,"2",&vbylx,1 );
				if( ret ) 
				{ 
					vtcp_log("%s %d::pub_ln_CalIntst error[%d]",__FILE__,__LINE__,ret);
					strcpy(g_pub_tx.reply,"A032");
					set_zd_data("0120","A032");
					set_zd_data("0130","表外欠息积数计算复利错误");
					Ln_mst_Clo_Sel();
					fclose(fp);
					goto ErrExit; 
				 }		
				
				vtcp_log("--------复利欠息积数计算复利:fllx----------");
				ret = -1;
				ret = pub_ln_CalIntst( d_ln_mst ,g_ln_parm ,"3",&vfllx,1 );
				if( ret ) 
				{ 
					vtcp_log("%s %d::pub_ln_CalIntst error[%d]",__FILE__,__LINE__,ret);
					strcpy(g_pub_tx.reply,"A032");
					set_zd_data("0120","A032");
					set_zd_data("0130","复利欠息积数计算错误");
					Ln_mst_Clo_Sel();
					fclose(fp);
					goto ErrExit; 
				 }
				 
				/*未结利息=未结本金利息+未结复利利息
				  未结复利利息=表内欠息复利+表外欠息复利+复利欠息复利
				*/	
				OpenInterest=vbjlx+vbnlx+vbylx+vfllx;
				 
				/*已欠利息=表内欠息+表外欠息+欠复利利息*/
				debitInterest=d_ln_mst.in_lo_intst+d_ln_mst.out_lo_intst+d_ln_mst.cmpd_lo_intst;
				
				/*写文件
					贷款账号|贷款户名|贷款余额|已欠利息|未结利息|
				*/
				
				if(0 == memcmp(clearType,"0",1))
				{
					if(d_ln_mst.bal == 0 && debitInterest == 0 && OpenInterest == 0)
					{
						fprintf(fp, "%20s|%60s|%15.2f|%15.2f|%15.2f|\n",\
							d_mdm_ac_rel.ac_no,d_ln_mst.name,d_ln_mst.bal,debitInterest,OpenInterest);
						vtcp_log("%s %d开始打印》%20s|%60s|%15.2f|%15.2f|%15.2f|\n",\
	 							__FILE__,__LINE__,d_mdm_ac_rel.ac_no,d_ln_mst.name,d_ln_mst.bal,debitInterest,OpenInterest);	
	 						
	 					++recordNo;
	 					vtcp_log("%s %d::产品记录数recordNo=%d",__FILE__,__LINE__,recordNo);
	 					if(recordNo >= atoi(turnPageShowNum))
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
				else if(0 == memcmp(clearType,"1",1))
				{
					if(d_ln_mst.bal != 0 || debitInterest != 0 || OpenInterest != 0)
					{
						fprintf(fp, "%20s|%60s|%15.2f|%15.2f|%15.2f|\n",\
							d_mdm_ac_rel.ac_no,d_ln_mst.name,d_ln_mst.bal,debitInterest,OpenInterest);
						vtcp_log("%s %d开始打印》%20s|%60s|%15.2f|%15.2f|%15.2f|\n",\
	 							__FILE__,__LINE__,d_mdm_ac_rel.ac_no,d_ln_mst.name,d_ln_mst.bal,debitInterest,OpenInterest);
						
						++recordNo;
						vtcp_log("%s %d::产品记录数recordNo=%d",__FILE__,__LINE__,recordNo);
	 					if(recordNo >= atoi(turnPageShowNum))
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
					fprintf(fp, "%20s|%60s|%15.2f|%15.2f|%15.2f|\n",\
						d_mdm_ac_rel.ac_no,d_ln_mst.name,d_ln_mst.bal,debitInterest,OpenInterest);
					vtcp_log("%s %d开始打印》%20s|%60s|%15.2f|%15.2f|%15.2f|\n",\
	 						__FILE__,__LINE__,d_mdm_ac_rel.ac_no,d_ln_mst.name,d_ln_mst.bal,debitInterest,OpenInterest);
					
					++recordNo;
					vtcp_log("%s %d::产品记录数recordNo=%d",__FILE__,__LINE__,recordNo);
	 				if(recordNo >= atoi(turnPageShowNum))
	 				{
	 					vtcp_log("%s %d::产品记录数最大",__FILE__,__LINE__);
	 					break;	
	 				}
				}
			}
		}
		
	}
	Ln_mst_Clo_Sel();
	
	fclose(fp);
	
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
		set_zd_data("0120","9999");
		set_zd_data("0130","文件传递出错");
		goto ErrExit;
	}
	
	vtcp_log("[%s][%d] ftp文件[%s]到平台成功", __FILE__, __LINE__,filename);
	
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	set_zd_data("0120","0000");
	set_zd_data("0130","付款账号与贷款账号关联查询！");
	vtcp_log("success !!: 5050 reply is[%s]", g_pub_tx.reply);
	return 0;
ErrExit:
	vtcp_log("fail !!: reply is[%s]", g_pub_tx.reply);
	return 1;
}

