/***********************************************************
*文件名: sp8925.c
*功能描述: 查询本月利息税
*作者: Jarod
*日期: 200611051100
***********************************************************/

#define EXTERN
#include "public.h"
#include "sum_tax_c.h"
#include "com_sys_parm_c.h"
#include "gl_mst_c.h"
#include "dd_parm_c.h"
#include "td_parm_c.h"

int sp8925()
{
	struct dd_parm_c sDd_parm;
	struct td_parm_c sTd_parm;
	struct gl_mst_c sGl_mst;
	struct sum_tax_c sSum_tax;
	struct com_sys_parm_c sCom_sys_parm;
	long date1=0,date2=0;
	char brno[6];
	char brno1[6];
	char brno2[6];
	char prdt_no[4];
	int ret=0;
	char wherelist[1024];
	char tmpstr[512];
	FILE *fp;
	int  ttlnum=0;
	char fieldlist[1024];
	char tablename[128];
	char title[40];
	char acc_hrt[6];
	char filename[100];
	char date[9];

	memset(date,0x00,sizeof(date));
	memset(acc_hrt,0x00,sizeof(acc_hrt));
	memset(title,0x00,sizeof(title));
	memset(fieldlist,0x00,sizeof(fieldlist));
	memset(tablename,0x00,sizeof(tablename));
	memset(filename,0x00,sizeof(filename));
	memset(tmpstr,0x00,sizeof(tmpstr));
	memset(wherelist,0x00,sizeof(wherelist));
	memset(brno,0x00,sizeof(brno));
	memset(brno1,0x00,sizeof(brno1));
	memset(brno2,0x00,sizeof(brno1));
	memset(prdt_no,0x00,sizeof(prdt_no));
	memset(&sSum_tax,0x00,sizeof(struct sum_tax_c));
	memset(&sCom_sys_parm,0x00,sizeof(struct com_sys_parm_c));
	memset(&sGl_mst,0x00,sizeof(struct gl_mst_c));
	memset(&sDd_parm,0x00,sizeof(struct dd_parm_c));
	memset(&sTd_parm,0x00,sizeof(struct td_parm_c));

	pub_base_sysinit();
	/*从前台取值*/
	get_zd_data("0230",prdt_no);
	get_zd_data("0910",brno1);
	get_zd_data("0030",brno2);
	get_zd_long("0440",&date1);
	get_zd_long("0450",&date2);

	if(date1==0 || date2==0)
	{ 	
		/*取得系统日期*/
		ret=Com_sys_parm_Sel(g_pub_tx.reply,&sCom_sys_parm,"1=1");
		if(ret)
		{
			sprintf(acErrMsg,"查询公共系统参数表出错!![%d]",ret);
			WRITEMSG
			goto ErrExit;
		}
		g_pub_tx.tx_date=sCom_sys_parm.lst_date;
		date1=g_pub_tx.tx_date/100*100+1;
		date2=g_pub_tx.tx_date/100*100+31;
	}
vtcp_log("日期为:[%ld]--->[%ld] [%s]--[%d]",date1,date2,__FILE__,__LINE__);
 	/*如果没有输入机构号就查询本机构的信息*/
	if(strlen(brno1))
		strcpy(brno,brno1);
	else
		strcpy(brno,brno2);
	
	sprintf(tmpstr,"\"date\">=%ld and \"date\"<=%ld and br_no='%s' ",date1,date2,brno);
	strcat(wherelist,tmpstr);
	if(strlen(prdt_no))
	{
		sprintf(tmpstr," and prdt_no='%s' ",prdt_no);
		strcat(wherelist,tmpstr);
	}
	strcpy(tablename,"sum_tax");	
vtcp_log("wherelist is:[%s] [%s]--[%d]",wherelist,__FILE__,__LINE__);
	ret=Sum_tax_Dec_Sel(g_pub_tx.reply,wherelist);
	if(ret)
	{
		sprintf(acErrMsg,"查询利息税表错误!![%d]",ret);
		WRITEMSG
		goto ErrExit;
	}	 
	while(1)
	{
		ret=Sum_tax_Fet_Sel(&sSum_tax,g_pub_tx.reply);
		if(ret == 100) break;
		else if(ret){
			sprintf(acErrMsg,"取利息税表出错!![%d]",ret);
			WRITEMSG
			goto ErrExit;
		}
		if(!ttlnum)
		{  pub_base_AllDwnFilName(filename);
			fp=fopen(filename,"w");
			if( !fp )
			{
				sprintf(acErrMsg," open file [%s] error ",filename );
				WRITEMSG
				strcpy( g_pub_tx.reply,"S047" );
				goto ErrExit;
			}
			fprintf(fp,"~@日期|@机构号|@产品号|@产品名称|$本期结付利息额|$扣缴所得税额|$扣缴人次|\n");
		}
		/*取产品名称*/
		if(sSum_tax.prdt_no[0] == '1')
		{
			ret=Dd_parm_Sel(g_pub_tx.reply,&sDd_parm,"prdt_no='%s'",sSum_tax.prdt_no);
			if(ret)
			{
				sprintf(acErrMsg,"查询活期产品参数表出错!![%d]",ret);
				WRITEMSG
				goto ErrExit;
			}
			strcpy(title,sDd_parm.title);
		}
		else if(sSum_tax.prdt_no[0] == '2')
		{
			ret=Td_parm_Sel(g_pub_tx.reply,&sTd_parm,"prdt_no='%s'",sSum_tax.prdt_no);
			if(ret)
			{
				sprintf(acErrMsg,"查询定期产品参数表出错!![%d]",ret);
				WRITEMSG
				goto ErrExit;
			}
			strcpy(title,sTd_parm.title);
		}	
	/*本期期末储蓄存款余额
	 	if(!strncmp(sSum_tax.prdt_no,"101",3 ) || !strncmp(sSum_tax.prdt_no,"102",3) || !strncmp(sSum_tax.prdt_no,"241",3))
			strcpy(acc_hrt,"21100");
		else
			strcpy(acc_hrt,"21500");
vtcp_log("取科目[%s]的值.[%s]--[%d]",acc_hrt,__FILE__,__LINE__);
		ret=Gl_mst_Sel(g_pub_tx.reply,&sGl_mst,"br_no='%s' and acc_hrt='%s'",brno,acc_hrt);
		if(ret)
		{
			sprintf(acErrMsg,"查询总帐主文件出错!![%d]",ret);
			WRITEMSG
			goto ErrExit;
		}
vtcp_log("本期期末储蓄存款余额:[%s] [%s]--[%d]",sGl_mst.cr_bal,__FILE__,__LINE__); 
	*/
		if(sSum_tax.date < -2147000000) sSum_tax.date= 0;
    	if(sSum_tax.intst < -2147000000)sSum_tax.intst=0.0;
		if(sSum_tax.tax <-2147000000)sSum_tax.tax=0.0;
		if(sSum_tax.cnt < -2147000000)sSum_tax.cnt=0;
	/*	if(sGl_mst.cr_bal < -2147000000)sGl_mst.cr_bal=0.0;*/
		sprintf(date,"%ld",sSum_tax.date);
		fprintf(fp,"%s|%s|%s|%s|%.2lf|%.2lf|%ld|%ld|\n",date,sSum_tax.br_no,sSum_tax.prdt_no,title,sSum_tax.intst,sSum_tax.tax,sSum_tax.cnt);
		ttlnum++;
	} /*end of while*/
	ret=Sum_tax_Clo_Sel();
	if(ret)
	{
		sprintf(acErrMsg,"关闭Sum_tax游标错误!![%d]",ret);
		WRITEMSG
		goto ErrExit;
	}
	if(!ttlnum)
	{
		strcpy( g_pub_tx.reply,"S049" );
		goto ErrExit;
	}
	else{
		fclose(fp);
		set_zd_data( DC_FILE_SND,"1" );
	}
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"Before OK return: reply is [%d]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before ERROR return: reply is[%d]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
