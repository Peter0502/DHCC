/***************************************************************************************
*交易名称: 实现对支票的历史查询
*文件名称: spJ415.c 
*
*
*功能描述: 
*改造者  : lyz
*改造日期: 20070130
*
***************************************************************************************/
#define ERR_DEAL if(ret){\
		 	sprintf(acErrMsg,"sql error.[%d] [%s]--[%s]",ret,__FILE__,__LINE__);\
			WRITEMSG \
			goto ErrExit;\
			}
#define EXTERN
#include "public.h"
#include "lv_pkgreg_c.h"

int spJ415()
{
	int ret=0;
	FILE *fp;
	char filename[100];
	int cnt=0;
	char brno[6];
	char stat[2];
	struct lv_pkgreg_c lv_pkgreg;
	char stat1[20];
	char txnum[20];
	
	memset(stat1,0x00,sizeof(stat1));
	memset(txnum,0x00,sizeof(txnum));
	memset(&lv_pkgreg,0x00,sizeof(struct lv_pkgreg_c));
	memset(brno,'\0',sizeof(brno));
	memset(stat,0x00,sizeof(stat));
	memset(filename,0x00,sizeof(filename));

	pub_base_sysinit();

	get_zd_data("0280",brno);	
	get_zd_data("0670",stat);
vtcp_log("入口参数brno=[%s],stat=[%s] [%s]--[%d]",brno,stat,__FILE__,__LINE__);
	if(brno[0] == '\0') goto ErrExit;
 
	ret=Lv_pkgreg_Dec_Sel(g_pub_tx.reply," (txnum='30102' or txnum='30103') and lv_sts='%c' and br_no = '%s' ",stat[0],brno);
	ERR_DEAL	

	while(1)
	{
		memset(stat1,0x00,sizeof(stat1));
		memset(txnum,0x00,sizeof(txnum));
		memset(&lv_pkgreg,0x00,sizeof(struct lv_pkgreg_c));
		
vtcp_log(" [%s]--[%d]",__FILE__,__LINE__);
		ret=Lv_pkgreg_Fet_Sel(&lv_pkgreg,g_pub_tx.reply);
		if(ret == 100) break;
		else if(ret)
		{
		 	sprintf(acErrMsg,"sql error.[%d] [%s]--[%s]",ret,__FILE__,__LINE__);\
			WRITEMSG 
			goto ErrExit;
		}	
vtcp_log(" [%s]--[%d]",__FILE__,__LINE__);
		if(!cnt)
		{
			pub_base_AllDwnFilName( filename );
			fp = fopen( filename,"w" );
			if(!fp)
			{
				sprintf(acErrMsg," open file [%s] error ",filename );
				WRITEMSG
				strcpy( g_pub_tx.reply,"S047" );
				goto ErrExit;
			}
		}
vtcp_log(" [%s]--[%d]",__FILE__,__LINE__);
		if(stat[0] == '1')
			sprintf(stat1,"往帐录入状态");
		if(!memcmp(lv_pkgreg.txnum,"30102",5))
		{
			sprintf(txnum,"支票截留业务");
		}
		else if(!memcmp(lv_pkgreg.txnum,"30103",5))
		{
			sprintf(txnum,"通用截留业务");
		}
vtcp_log(" [%s]--[%d]",__FILE__,__LINE__);
		fprintf(fp,"%ld|%ld|%s|%.2lf|%s|%s|%s|%s|%s|%s|%s\n",lv_pkgreg.in_date,lv_pkgreg.wt_date,lv_pkgreg.orderno,lv_pkgreg.tx_amt,stat1,lv_pkgreg.or_br_no,lv_pkgreg.ac_br_no,lv_pkgreg.pay_ac_no,lv_pkgreg.cash_ac_no,txnum,lv_pkgreg.txnum);
	cnt++;
	}
	Lv_pkgreg_Clo_Sel();

	if(!cnt)
	{
		strcpy( g_pub_tx.reply,"S049" );
		goto ErrExit;
	}
	else
	{
		fclose(fp);
		set_zd_data( DC_FILE_SND,"1" );
	}
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"Before OK return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;	
ErrExit:
	sprintf(acErrMsg,"Before ERROR return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;	
	
}
