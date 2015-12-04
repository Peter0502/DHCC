/************************************************************************
* 文 件 名:  pubf_com_chnl_detl.c
* 功能描述： 渠道发来交登记相关交易详细信息
* 作  者:    chenggx
* 完成日期： 20130325
* 参    数： 渠道组好的数据串 单笔记账通过125域传，批量记账放到明细最后
* 程序修改： Modified by YangMeng 20130425 
*************************************************************************/
#include "find_debug.h" 
#include <stdio.h>
#include "public.h"
#include "chnl_tx_detl_c.h"

int strsplit(char *str,char *des,int k);

int pubf_com_checkin_txdetail(char *sDetail)
{
	int iRet = 0;
	char *ps=NULL;
	char sTmp[128] = {0};
	struct chnl_tx_detl_c stChnl_tx_detl;
	memset(sTmp,0x00,sizeof(sTmp));
	memset(&stChnl_tx_detl,0x00,sizeof(stChnl_tx_detl));/**/
	
	strcpy(stChnl_tx_detl.syscode,g_pub_tx.tty);
	strcpy(stChnl_tx_detl.tx_code,g_pub_tx.tx_code);
	stChnl_tx_detl.tx_date = g_pub_tx.tx_date;
	/*stChnl_tx_detl.tx_time = g_pub_tx.tx_time;*/
	sprintf(stChnl_tx_detl.tx_time,"%6ld", g_pub_tx.tx_time);    /** 系统时间  **/
	stChnl_tx_detl.trace_no = g_pub_tx.trace_no;
	stChnl_tx_detl.trace_cnt = g_pub_tx.trace_cnt;/**/
	
	/*循环取组好的串中的数据 给结构体赋值*/
	/*平台日期|平台流水|委托日期|支付序号|发起行行号|往来标示|交易账号行内账户|主账号增减标志 0减 1增|现转标志 1现 2转|交易金额|对方行行号|对方账号|对方名称|对方id|对方证件号码|业务类型编号|业务种类|渠道类型：1:银联卡 2:农信银 3:人行支付 4:二代代理平台 5:二代农信银*/
	ps = sDetail;
	memset(sTmp,0x00,sizeof(sTmp));
	strsplit(ps,sTmp,0);
	zip_space(sTmp);
	stChnl_tx_detl.pt_date = atoi(sTmp); /*平台日期*/
	memset(sTmp,0x00,sizeof(sTmp));
	strsplit(ps,sTmp,1);
	zip_space(sTmp);
	stChnl_tx_detl.pt_trace_no=atoi(sTmp); /*平台流水*/
	memset(sTmp,0x00,sizeof(sTmp));
	strsplit(ps,sTmp,2);
	zip_space(sTmp);
	stChnl_tx_detl.wt_date=atoi(sTmp); /*委托日期*/
	memset(sTmp,0x00,sizeof(sTmp));
	strsplit(ps,sTmp,3);
	zip_space(sTmp);
	strcpy(stChnl_tx_detl.orderno,sTmp); /*支付序号*/
	memset(sTmp,0x00,sizeof(sTmp));
	strsplit(ps,sTmp,4);
	zip_space(sTmp);
	strcpy(stChnl_tx_detl.or_br_no,sTmp); /*发起行行号*/
	memset(sTmp,0x00,sizeof(sTmp));
	strsplit(ps,sTmp,5);
	zip_space(sTmp);
	strcpy(stChnl_tx_detl.sndrcvflag,sTmp); /*往来标示*/
	memset(sTmp,0x00,sizeof(sTmp));
	strsplit(ps,sTmp,6);
	zip_space(sTmp);
	strcpy(stChnl_tx_detl.ac_no,sTmp); /*交易账号 行内账户*/
	memset(sTmp,0x00,sizeof(sTmp));
	strsplit(ps,sTmp,7);
	vtcp_log("[%s][%d],账户户名ac_name=[%s]",__FILE__,__LINE__,sTmp);
	zip_space(sTmp);
	vtcp_log("[%s][%d],账户户名ac_name=[%s]",__FILE__,__LINE__,sTmp);
	vtcp_log("[%s][%d],账户户名长度strlen=[%d]",__FILE__,__LINE__,strlen(sTmp));
	memcpy(stChnl_tx_detl.ac_name,sTmp,strlen(sTmp)+1); /*对方名称*/
	vtcp_log("[%s][%d],账户户名ac_name=[%s]",__FILE__,__LINE__,stChnl_tx_detl.ac_name);
	memset(sTmp,0x00,sizeof(sTmp));
	strsplit(ps,sTmp,8);
	zip_space(sTmp);
	strcpy(stChnl_tx_detl.add_ind,sTmp); /*主账号增减标志 0减 1增*/
	memset(sTmp,0x00,sizeof(sTmp));
	strsplit(ps,sTmp,9);
	zip_space(sTmp);
	strcpy(stChnl_tx_detl.ct_ind,sTmp); /*现转标志 1现 2转 */
	memset(sTmp,0x00,sizeof(sTmp));
	strsplit(ps,sTmp,10);
	zip_space(sTmp);
	stChnl_tx_detl.tx_amt = strtod(sTmp,NULL); /*交易金额*/
	memset(sTmp,0x00,sizeof(sTmp));
	strsplit(ps,sTmp,11);
	zip_space(sTmp);
	strcpy(stChnl_tx_detl.df_br_no,sTmp); /*对方行行号*/
	memset(sTmp,0x00,sizeof(sTmp));
	strsplit(ps,sTmp,12);
	zip_space(sTmp);
	strcpy(stChnl_tx_detl.df_ac_no,sTmp); /*对方账号*/
	memset(sTmp,0x00,sizeof(sTmp));
	strsplit(ps,sTmp,13);
	vtcp_log("[%s][%d],对方名称df_name=[%s]",__FILE__,__LINE__,sTmp);
	zip_space(sTmp);
	vtcp_log("[%s][%d],对方名称df_name=[%s]",__FILE__,__LINE__,sTmp);
	vtcp_log("[%s][%d],对方名称长度strlen=[%d]",__FILE__,__LINE__,strlen(sTmp));
	memcpy(stChnl_tx_detl.df_name,sTmp,strlen(sTmp)+1); /*对方名称*/
	vtcp_log("[%s][%d],对方名称df_name=[%s]",__FILE__,__LINE__,stChnl_tx_detl.df_name);
	memset(sTmp,0x00,sizeof(sTmp));
	strsplit(ps,sTmp,14);
	zip_space(sTmp);
	strcpy(stChnl_tx_detl.df_id_type,sTmp); /*对方id*/
	memset(sTmp,0x00,sizeof(sTmp));
	strsplit(ps,sTmp,15);
	zip_space(sTmp);
	strcpy(stChnl_tx_detl.df_id_no,sTmp); /*对方证件号码*/
	memset(sTmp,0x00,sizeof(sTmp));
	strsplit(ps,sTmp,16);
	zip_space(sTmp);
	memcpy(stChnl_tx_detl.txnum,sTmp,4); /*业务类型编号*/
	memset(sTmp,0x00,sizeof(sTmp));
	strsplit(ps,sTmp,17);
	zip_space(sTmp);
	memcpy(stChnl_tx_detl.yw_type,sTmp,5); /*业务种类*/
	memset(sTmp,0x00,sizeof(sTmp));
	strsplit(ps,sTmp,18);
	zip_space(sTmp);
	strcpy(stChnl_tx_detl.chnl_type,sTmp); /*渠道类型：1:银联卡 2:农信银 3:人行支付 4:二代代理平台 5:二代农信银*/
	strcpy(stChnl_tx_detl.dz_ind,"0");/*对账状态*/
	memset(sTmp,0x00,sizeof(sTmp));
	strsplit(ps,sTmp,19);
	zip_space(sTmp);
	strcpy(stChnl_tx_detl.filler1,sTmp);/*filler1用于登记附言*/
	/*增加行内账号户名 add by xk 2014/6/13 10:38:34 begin*/
	/*通存通兑现金汇款时 付款账号为0*/
	if(strcmp(stChnl_tx_detl.ac_no,"0")!=0 && stChnl_tx_detl.chnl_type[0]=='3'){
		iRet = pub_base_acnogetname(stChnl_tx_detl.ac_no,stChnl_tx_detl.ac_name);
		if(iRet)
		{
			sprintf(acErrMsg,"根据账号查找户名失败!!!");
			WRITEMSG
			/*strcpy(g_pub_tx.reply, "D104");
			goto ErrExit;此处不需要添加错误输出，因为来账的时候账号有可能是随便输入，去挂账了*/
		}
		vtcp_log("[%s][%d],stChnl_tx_detl.ac_name=[%s]",__FILE__,__LINE__,stChnl_tx_detl.ac_name);
	}
	/*增加行内账号户名 add by xk 2014/6/13 10:38:34 end*/
	
	Chnl_tx_detl_Debug(&stChnl_tx_detl);
	
	iRet = Chnl_tx_detl_Ins(stChnl_tx_detl,g_pub_tx.reply);
	if(iRet)
	{
		sprintf(acErrMsg,"登记渠道发起交易明细登记簿失败!!!");
		WRITEMSG
		Chnl_tx_detl_Debug(&stChnl_tx_detl);
		strcpy(g_pub_tx.reply, "D107");
		goto ErrExit;
	}
	
OKExit:
	return 0;
ErrExit:
	return 1;
}

/**************************************************************************
* FUNCTION  : 取特定位置字符串
* Input     : 原串、目标串、位置
* OUTPUT    :
* RETURN    : 0(成功)   、-1(失败)
**************************************************************************/
int strsplit(char *str,char *des,int k)
{	
	if(str == NULL || des == NULL)
	{
		return -1;	
	}
	int i=0;
	int t=0; /*记录上一次位置*/
	int j=0;
	while(*(str+i)!='\0')
	{
		if(*(str+i)=='|')
		{
			if(k==j++)
			{
				memcpy(des,str+t,i-t);	
			}	
			t=i+1;
		}	
		i++;
	}
	return 0;
}

