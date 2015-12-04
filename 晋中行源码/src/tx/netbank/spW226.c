/*********************************************************************
* 版 本 号 :  
* 程序作者 :  刘增盈
* 日    期 :  2015/11/26
* 所属模块 : 	IBPS记账登记DETL表
* 程序名称 :  spW226.c
* 程序作用 :
* 函数列表 :
* 使用注意 :
交易配置：
insert into TX_DEF (TX_CODE, TX_NAME, BIT_MAP, LOG_IND, TX_TYPE, WRK_STS) values ('5064', 'IBPS记账登记detl表', '10000000000000000000110011010011100000111111111110000000000001000111111100000000000110000000100000011000000000000000000000000000', '0', '6', '0');
insert into TX_FLOW_DEF (TX_CODE, FLOW_CODE, SUB_TX_CODE, SUB_TX_RSLT) values ('5064', 0, 'W226', '#$');
insert into TX_SUB_DEF (SUB_TX_CODE, SUB_TX_NAME, TX_FUNC, ACCT_IND, CHNL_IND) values ('W226', 'IBPS记账登记DETL表', 'spW226', '0', '0');
***********************************************************************/
#define EXTERN
#include <string.h>
#include <stdio.h>
#include "public.h"
#include "chnl_tx_detl_c.h"			      /*二代支付交易日志表*/
char *msstrtok(char *instr, char *outstr, char *delimiter);

int spW226()
{	
	char detl[1024]={0};
	char *str_tmp=detl;
	char temp[100]={0};
	char txtype[6]={0};

	int iRet=0;
		
	struct chnl_tx_detl_c chnl_tx_detl;
	memset(&chnl_tx_detl,0x00,sizeof(chnl_tx_detl));
		
	/**初始化公用结构**/
	if (pub_base_sysinit())
	{
		vtcp_log("初始化公用结构错误!!");
		strcpy(g_pub_tx.reply, "CU06");
		goto ErrExit;
	}	
	get_zd_data("0180",txtype);	 /**交易类型**/
	if (strlen(txtype) == 0 )
	{
		strcpy(g_pub_tx.reply,"9999");
		set_zd_data("0130","交易类型未传来!");		
		goto ErrExit;
	}
	get_zd_data("1080",detl);	 /**detl明细字段**/
  
	if (strlen(detl) == 0 )
	{
		strcpy(g_pub_tx.reply,"9999");
		set_zd_data("0130","明细内容未传来!");		
		goto ErrExit;
	}
	vtcp_log("[%s][%d],明细内容为[%s]",__FILE__,__LINE__,detl);
	
	memset(temp, 0x00,sizeof(temp));
	str_tmp=msstrtok(str_tmp,temp,"|");
	chnl_tx_detl.pt_date=atol(temp);
	vtcp_log("[%s][%d]--平台日期=[%ld]",__FILE__,__LINE__,chnl_tx_detl.pt_date);
	
	memset(temp, 0x00,sizeof(temp));
	str_tmp=msstrtok(str_tmp,temp,"|");
	chnl_tx_detl.pt_trace_no=atol(temp);
	vtcp_log("[%s][%d]--平台流水=[%ld]",__FILE__,__LINE__,chnl_tx_detl.pt_trace_no);
	
	memset(temp, 0x00,sizeof(temp));
	str_tmp=msstrtok(str_tmp,temp,"|");
	chnl_tx_detl.tx_amt=atof(temp);
	vtcp_log("[%s][%d]--金额=[%.2f]",__FILE__,__LINE__,chnl_tx_detl.tx_amt);
	
	memset(temp, 0x00,sizeof(temp));
	str_tmp=msstrtok(str_tmp,temp,"|");
	chnl_tx_detl.trace_no=atol(temp);
	vtcp_log("[%s][%d]--核心流水=[%ld]",__FILE__,__LINE__,chnl_tx_detl.trace_no);
	
	memset(temp, 0x00,sizeof(temp));
	str_tmp=msstrtok(str_tmp,temp,"|");
	strcpy(chnl_tx_detl.or_br_no,temp);
	strcpy(chnl_tx_detl.df_br_no,temp);
	vtcp_log("[%s][%d]--交易行号=[%s]",__FILE__,__LINE__,chnl_tx_detl.or_br_no);
	
  memset(temp, 0x00,sizeof(temp));
	str_tmp=msstrtok(str_tmp,temp,"|");
	strcpy(chnl_tx_detl.df_name,temp);
	vtcp_log("[%s][%d]--对方户名=[%s]",__FILE__,__LINE__,chnl_tx_detl.df_name);
	
	memset(temp, 0x00,sizeof(temp));
	str_tmp=msstrtok(str_tmp,temp,"|");
	strcpy(chnl_tx_detl.df_ac_no,temp);
	vtcp_log("[%s][%d]--对方账号=[%s]",__FILE__,__LINE__,chnl_tx_detl.df_ac_no);
	
	memset(temp, 0x00,sizeof(temp));
	str_tmp=msstrtok(str_tmp,temp,"|");
	strcpy(chnl_tx_detl.ac_name	,temp);
	vtcp_log("[%s][%d]--户名=[%s]",__FILE__,__LINE__,chnl_tx_detl.ac_name);
	
	memset(temp, 0x00,sizeof(temp));
	str_tmp=msstrtok(str_tmp,temp,"|");
	strcpy(chnl_tx_detl.ac_no,temp);
	vtcp_log("[%s][%d]--账号=[%s]",__FILE__,__LINE__,chnl_tx_detl.ac_no);
	
	memset(temp, 0x00,sizeof(temp));
	str_tmp=msstrtok(str_tmp,temp,"|");
	strcpy(chnl_tx_detl.filler1,temp);
	vtcp_log("[%s][%d]--附言=[%s]",__FILE__,__LINE__,chnl_tx_detl.filler1);
	
	strcpy(chnl_tx_detl.tx_code,"6234");
	strcpy(chnl_tx_detl.syscode,"IBPS");
	strcpy(chnl_tx_detl.chnl_type,"3");
	chnl_tx_detl.tx_date=g_pub_tx.tx_date;
	sprintf(chnl_tx_detl.tx_time,"%6d",g_pub_tx.tx_time);
	chnl_tx_detl.trace_cnt=0;
	chnl_tx_detl.wt_date=g_pub_tx.tx_date;
	strcpy(chnl_tx_detl.orderno,"000000");
	if ( memcmp(txtype,"1012",4)==0 )   /**IBPS贷记来账**/
	{
		strcpy(chnl_tx_detl.sndrcvflag,"SR01");
		strcpy(chnl_tx_detl.filler2,"IBPS贷记来账");
	}
	else if ( memcmp(txtype,"1031",4)==0 ) /**IBPS借记往账**/
	{
		strcpy(chnl_tx_detl.sndrcvflag,"SR00");
		strcpy(chnl_tx_detl.filler2,"IBPS借记往账");
	}
	strcpy(chnl_tx_detl.add_ind,"1");
	strcpy(chnl_tx_detl.ct_ind,"2");
	strcpy(chnl_tx_detl.df_br_no,"2");
	strcpy(chnl_tx_detl.dz_ind,"0");
	
	iRet = Chnl_tx_detl_Ins(chnl_tx_detl,g_pub_tx.reply);
	if(iRet)
	{
		vtcp_log("[%s][%d],登记渠道发起交易明细登记簿失败",__FILE__,__LINE__);
		Chnl_tx_detl_Debug(&chnl_tx_detl);
		strcpy(g_pub_tx.reply, "D107");
		set_zd_data("0130","登记渠道发起交易明细登记簿失败!");	
		goto ErrExit;
	}


	OkExit:
		strcpy(g_pub_tx.reply,"0000");
		set_zd_data("0120","0000");
		set_zd_data("0130","IBPS记账登记DETL表成功!");
		vtcp_log("success !!: 5064 reply is[%s]", g_pub_tx.reply);
		return 0;
		
	ErrExit:
		vtcp_log("fail !!: 5064 reply is[%s]", g_pub_tx.reply);
		return -1;
}

/**功能：取得instr中第一个分隔符delimiter前的内容
参数： outstr指向instr中第一个delimiter之前的内容
返回值：返回下一个delimiter的位置***************/
char * strstr(char *,char *);
char *msstrtok(char *instr, char *outstr, char *delimiter)
{
  char *tmpstr;
  if(memcmp(instr, delimiter, strlen(instr))==0){
    return(NULL);
  }
  if(instr==NULL || strlen(instr)==0){return(NULL);}
  tmpstr=strstr(instr,delimiter);
  if(tmpstr!=NULL){
    memcpy(outstr, instr, strlen(instr)-strlen(tmpstr));
    return(strstr(instr,delimiter)+strlen(delimiter));
  }else{
    memcpy(outstr, instr, strlen(instr));
    return(NULL);
  }
}