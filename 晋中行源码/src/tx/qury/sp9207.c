/*****************************************************
* 文 件 名:  sp9207.c
* 功能描述： 得到柜面通的总笔数和总金额
* 作    者:  
* 完成日期： 2015年5月25日 
* 修改记录： 
* 日    期:
* 备    注:  3位名称+4位笔数+16金额（数据格式左对齐）
* 				   sh1笔数金额fx1笔数金额tg1笔数金额zq1笔数金额rx1笔数金额gj1笔数金额rx1笔数金额
*****************************************************/

#define EXTERN

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "public.h"
#include "trace_log_c.h"
#include "trace_log_rz_c.h"

int sp9207()
{
	int			ret = 0;
	int			i = 0;
	int			j = 0;
	int			len = 0;
	int			num = 0;			/*总笔数*/
	double	amt = 0;			/*总金额*/
	double	amt_0 = 0;
	double	amt_1 = 0;
	char		cDate[10];
	char		str[162];			/*总串*/
	char		brBz[3];			/*行名标志*/
	char		cQueryc[100];	/*查询条件*/
	long		ltxdate = 0;
	char		brTmp[15];
	char		szTable[20];
	char		str_br[4];			/*行名标志串*/
	char		str_no[5];			/*笔数串*/
	char		str_amt[17];			/*金额串*/
	
	memset(cDate,0x00,sizeof(cDate));
	memset(szTable, 0, sizeof(szTable));
	memset(cQueryc,0x00,sizeof(cQueryc));
	memset(&str,0x00,162);
	memset(&brTmp,0x00,22);
	
	/*初始化结构*/
	pub_base_sysinit();
	get_zd_data("0280",cDate);
	memcpy(cDate+8,"\0",1);
	ltxdate = atoi(cDate);
	strcpy(brTmp,"01020304050700");

	sprintf(acErrMsg,"查询日期:[%ld]",ltxdate);
	WRITEMSG
	
	for(i=0; i<7; i++)
	{
		memset(&brBz,0x00,3);
		strncpy(brBz,brTmp+i*2,2);
		num = 0;
		amt = 0;
		amt_0 = 0;
		amt_1 = 0;
		memset(&str_br,0x00,4);
		memset(&str_no,0x00,5);
		memset(&str_amt,0x00,17);
		/*****设置查询条件 - 开始*****/
		memset(cQueryc,0x00,sizeof(cQueryc));
		sprintf(cQueryc," brf like '%%s_%柜面通%' and tx_date= %ld order by trace_no", brBz, ltxdate);
	
		sprintf(acErrMsg,"[%s][%d] 查询条件:[%s]!",__FILE__,__LINE__,cQueryc);
		WRITEMSG
		
		if(g_pub_tx.tx_date != ltxdate)
		{
			strncpy(szTable, "trace_log_rz", sizeof(szTable) - 1);
		}
		else
		{
			strncpy(szTable, "trace_log", sizeof(szTable) - 1);
		}
		
		num = count_distinct(szTable,"trace_no","%s",cQueryc);
		sql_sum_double(szTable, "amt", &amt_0,"%s and dc_ind='0'",cQueryc);
		sql_sum_double(szTable, "amt", &amt_1,"%s and dc_ind='1'",cQueryc);
		amt = amt_1-amt_0;
		
		if(strcmp(brBz,"00")==0)
		{
			strcpy(str_br,"sh1");
		}
		else if(strcmp(brBz,"01")==0)
		{
			strcpy(str_br,"rx1");
		}
		else if(strcmp(brBz,"02")==0)
		{
			strcpy(str_br,"fx1");
		}
		else if(strcmp(brBz,"03")==0)
		{
			strcpy(str_br,"xy1");
		}
		else if(strcmp(brBz,"04")==0)
		{
			strcpy(str_br,"zq1");
		}
		else if(strcmp(brBz,"05")==0)
		{
			strcpy(str_br,"tg1");
		}
		else if(strcmp(brBz,"06")==0)
		{
			strcpy(str_br,"gj1");
		}
		
		strcpy(str_br,brBz);
		sprintf(str_no,"%-4d",num);
		sprintf(str_amt,"%-.2lf",amt);
		len=strlen(str_amt);
		for(j=0; j<(16-len); j++)
		{
			strcat(str_amt, " ");
		}
		str_amt[16]='\0';
		vtcp_log("[%s][%d] 总串str[%s]\n", __FILE__, __LINE__, str);
		vtcp_log("[%s][%d] 行标str_br[%s]\n", __FILE__, __LINE__, str_br);
		vtcp_log("[%s][%d] 笔数str_no[%s]\n", __FILE__, __LINE__, str_no);
		vtcp_log("[%s][%d] 金额str_amt[%s]\n", __FILE__, __LINE__, str_amt);
		strcat(str,str_br);
		strcat(str,str_no);
		strcat(str,str_amt);
	}
	
	set_zd_data("0950", str);
	
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"success !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;

ErrExit:
	sprintf(acErrMsg,"fail !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
