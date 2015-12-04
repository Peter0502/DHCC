/*************************************************
* 文 件 名:  spF241.c
* 功能描述：联机对账交易 (单笔，批量都写一块)
*
* 作    者:  wk
* 完成日期： 2011年10月25日
*insert into tx_def values ('6241','联机对账','10000000000000010000000000100000000000000001100000000000000000000000000000000000100000000000000000000000000000000000000000100100,'1','3','0','3',null);
*insert into tx_flow_def  values ('6241',0,'B241','#$');
*insert into tx_sub_def values ('B241','联机对账','spB241','0','0');
* 生成对账文件 子系统号(bbmk)_compare_yyyymmdd.txt，格式为：
*  核心日期|核心流水号|平台日期|平台流水号|帐卡号|交易金额|记账状态|
*  83域返回文件名
	           
* 日   期:
* 修 改 人:
* 修改内容:
*************************************************/
#include <string.h>
#include <stdio.h>
#include "public.h"
#include "chnl_single_acct_c.h"
#include "chnl_single_hold_c.h"
#include "chnl_batch_acct_c.h"

int spF241()
{ 
  vtcp_log("spF241 开始  [%s][%d] 日期 [%ld]",__FILE__,__LINE__,g_pub_tx.tx_date);
  int ret=0;
  long beg_date=0;
  long end_date=0;
  FILE *fp=NULL; /*写文件*/
  char *p;
  char bbmk[5];
  char cFilename[300];
  char cmd[400];
    
  struct chnl_single_acct_c chnl_single_acct;
  struct chnl_single_hold_c chnl_single_hold;
  struct chnl_batch_acct_c  chnl_batch_acct;
  
  memset(bbmk,0x00,sizeof(bbmk));
  memset(cFilename,0x00,sizeof(cFilename));
  memset(&chnl_single_acct,0x00,sizeof(struct chnl_single_acct_c));
  memset(&chnl_single_hold,0x00,sizeof(struct chnl_single_hold_c));
  memset(&chnl_batch_acct,0x00,sizeof(struct chnl_batch_acct_c));
  /*初始化*/
  pub_base_sysinit();
  vtcp_log("[%s][%d],tx_date=[%d]",__FILE__,__LINE__,g_pub_tx.tx_date);
  get_zd_data("0010",bbmk);
  get_zd_long("0460",&beg_date);
  get_zd_long("0470",&end_date);
  

  /*文件名*/
  sprintf(cFilename,"%s/%s_compare_%ld.txt",(char*)getenv("FILDIR"),bbmk,g_pub_tx.tx_date);
  vtcp_log("[%s][%d]   file name is %s ",__FILE__,__LINE__,cFilename); 
  fp=fopen(cFilename,"w");
  if(fp==NULL)
  {
    vtcp_log("[%s][%d]打开文件错误！文件名是：%s ",__FILE__,__LINE__,cFilename);
    fclose(fp);
    strcpy(g_pub_tx.reply,"L206");
    goto ErrExit;
  }
  /****单笔记账*******/
  ret=Chnl_single_acct_Dec_Upd(g_pub_tx.reply,"tx_date>=%ld and tx_date<=%ld and dz_ind='0'",beg_date,end_date);
  if(ret){
  		vtcp_log("打开游标出错,%d",ret);
  		strcpy(g_pub_tx.reply,"D103");
  		goto ErrExit;
  }
  while(1){
  	memset(&chnl_single_acct,0x00,sizeof(struct chnl_single_acct_c));
  	ret=Chnl_single_acct_Fet_Upd(&chnl_single_acct,g_pub_tx.reply);
  	if(ret=100) break;
  	else if(ret){
  		vtcp_log("fet出错,%d",ret);
  		strcpy(g_pub_tx.reply,"D103");
  		goto ErrExit;
  	}
  	/******0 交易无效  1冲正成功******/
  	if(chnl_single_acct.sts[0]=='0' || chnl_single_acct.sts[0]=='4'){
  		continue;
  	}
  	chnl_single_acct.dz_ind[0]='1';/***已对********/
  	fprintf(fp,"%ld|%ld|%ld|%ld|%s|%.2f|%s|\n",chnl_single_acct.tx_date,chnl_single_acct.trace_no,chnl_single_acct.pt_date,chnl_single_acct.pt_trace_no,chnl_single_acct.ac_no,chnl_single_acct.tx_amt,chnl_single_acct.sts);
  	
  	ret=Chnl_single_acct_Upd_Upd(chnl_single_acct,g_pub_tx.reply);
  	if(ret){
  		vtcp_log("upd出错,%d",ret);
  		strcpy(g_pub_tx.reply,"D103");
  		goto ErrExit;
  	}
  }
  Chnl_single_acct_Clo_Upd();
  /*****单笔预授权********/
  ret=Chnl_single_hold_Dec_Upd(g_pub_tx.reply,"tx_date>=%ld and tx_date<=%ld and dz_ind='0' ",beg_date,end_date);
  if(ret){
  		vtcp_log("打开游标出错,%d",ret);
  		strcpy(g_pub_tx.reply,"D103");
  		goto ErrExit;
  }
  while(1){
  	memset(&chnl_single_hold,0x00,sizeof(chnl_single_hold));
  	ret=Chnl_single_hold_Fet_Upd(&chnl_single_hold,g_pub_tx.reply);
  	if(ret=100) break;
  	else if(ret){
  		vtcp_log("fet出错,%d",ret);
  		strcpy(g_pub_tx.reply,"D103");
  		goto ErrExit;
  	}
  	/******0 交易无效  1冲正成功******/
  	if(chnl_single_hold.sts[0]=='0' || chnl_single_hold.sts[0]=='4'){
  		continue;
  	}
  	chnl_single_hold.dz_ind[0]='1';/***已对********/
  	fprintf(fp,"%ld|%ld|%ld|%ld|%s|%.2f|%s|\n",chnl_single_hold.tx_date,chnl_single_hold.trace_no,chnl_single_hold.pt_date,chnl_single_hold.pt_trace_no,chnl_single_hold.ac_no,chnl_single_hold.tx_amt,chnl_single_hold.sts);
  	
  	ret=Chnl_single_hold_Upd_Upd(chnl_single_hold,g_pub_tx.reply);
  	if(ret){
  		vtcp_log("upd出错,%d",ret);
  		strcpy(g_pub_tx.reply,"D103");
  		goto ErrExit;
  	}
  }
  Chnl_single_hold_Clo_Upd();
  /******批量******/
  ret=Chnl_batch_acct_Dec_Upd(g_pub_tx.reply,"tx_date>=%ld and tx_date<=%ld and dz_ind='0'",beg_date,end_date);
  if(ret){
  		vtcp_log("打开游标出错,%d",ret);
  		strcpy(g_pub_tx.reply,"D103");
  		goto ErrExit;
  }
  while(1){
  	memset(&chnl_batch_acct,0x00,sizeof(chnl_batch_acct));
  	ret=Chnl_batch_acct_Fet_Upd(&chnl_batch_acct,g_pub_tx.reply);
  	if(ret=100) break;
  	else if(ret){
  		vtcp_log("fet出错,%d",ret);
  		strcpy(g_pub_tx.reply,"D103");
  		goto ErrExit;
  	}
  	/******0 交易无效  1冲正成功******/
  	if(chnl_batch_acct.sts[0]=='0' || chnl_batch_acct.sts[0]=='4'){
  		continue;
  	}
  	chnl_batch_acct.dz_ind[0]='1';
  	fprintf(fp,"%ld|%ld|%ld|%ld|%s|%.2f|%s|\n",chnl_batch_acct.tx_date,chnl_batch_acct.trace_no,chnl_batch_acct.pt_date,chnl_batch_acct.pt_trace_no,chnl_batch_acct.ac_no,chnl_batch_acct.tx_amt,chnl_batch_acct.sts);
  	
  	ret=Chnl_batch_acct_Upd_Upd(chnl_batch_acct,g_pub_tx.reply);
  	if(ret){
  		vtcp_log("upd出错,%d",ret);
  		strcpy(g_pub_tx.reply,"D103");
  		goto ErrExit;
  	}
  }
  Chnl_batch_acct_Clo_Upd();
 
  if(fp==NULL){
  		vtcp_log("今天已对账或未发生业务");
  		strcpy(g_pub_tx.reply,"D104");
  		goto ErrExit;
  }
  fclose(fp);
  memset(cFilename,0x00,sizeof(cFilename));
  sprintf(cFilename,"%s_compare_%ld.txt",bbmk,g_pub_tx.tx_date);
  memset(cmd,0x0,sizeof(cmd));
	sprintf(cmd,"send_back.sh %s",cFilename);
	ret=system(cmd);
	if(ret){
		sprintf(acErrMsg, "回传文件出错[%d]",ret);
		WRITEMSG
		goto ErrExit;
		}
  set_zd_data("0830",cFilename);
  
OkExit:
  strcpy( g_pub_tx.reply, "0000" );
  sprintf(acErrMsg,"Before OK return: reply [%s]",g_pub_tx.reply);
  WRITEMSG
  set_zd_data(DC_REPLY,g_pub_tx.reply);
  return 0;
ErrExit:
  sprintf(acErrMsg,"Before bad return: reply [%s]",g_pub_tx.reply);
  WRITEMSG
  if(fp!=NULL){
  	fclose(fp);
  }
  set_zd_data(DC_REPLY,g_pub_tx.reply);
  return 1;
 }
 
 
