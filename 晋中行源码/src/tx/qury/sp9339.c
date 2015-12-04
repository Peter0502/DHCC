/*************************************************
* 文 件 名:  sp9339.c
* 功能描述： 财付通汇总明细查询
*
* 作    者:  Huzhaohua
* 完成日期： 201311128
*
* 修改记录：
* 日   期:
* 修 改 人:
* 修改内容:
*************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define EXTERN
#include "public.h"
#include "dd_mst_hst_c.h"

int sp9339()
{
	char ac_no[25];
	char filename[100];
	int ttlnum=0;			/**读取的总条数**/
	int ret=0;
	long beg_date=0, end_date=0,ac_id=0;
	double tx_amt_sum=0.00;
	double bal_begin=0.00;
	FILE *fp;
	struct dd_mst_hst_c sDd_mst_hst;

	memset(ac_no,0x00,sizeof(ac_no));
	memset(filename,0x00,sizeof(filename));
	memset(&sDd_mst_hst,0x00,sizeof(sDd_mst_hst));
	
	get_zd_data("0300",ac_no);
  pub_base_old_acno(ac_no);
	get_zd_long("0290",&beg_date );
	get_zd_long("0450",&end_date );

	pub_base_strpack( ac_no );
	if( strlen(ac_no) )/**账号**/
	{
		if ( pub_base_acno_acid(&ac_id,ac_no) )
		{
		goto ErrExit;
		}
	}
	vtcp_log( "%s,%d,ac_no[%s],ac_id[%ld],startdate[%ld],enddate[%ld]",__FILE__,__LINE__,ac_no,ac_id,beg_date,end_date); 

	ttlnum = 0;

	pub_base_AllDwnFilName( filename );
	/**sprintf( filename,"%s/tenpay_hzmx.txt",getenv("FILDIR")); **/
	vtcp_log( "%s,%d,filename[%s]",__FILE__,__LINE__,filename);
	fp = fopen( filename,"w" ); 
	if( fp==NULL ) 
	{ 
		sprintf(acErrMsg," open file [%s] error ",filename ); 
		WRITEMSG 
		strcpy( g_pub_tx.reply,"S047" ); 
		goto ErrExit; 
	}

	 fprintf(fp, "账务日期|账务时间|借方金额|贷方金额|余额|摘要|\n");
	 
	/**求财付通账户对应这一查询区间的起始账户余额**/
	memset(&sDd_mst_hst,0x00,sizeof(struct dd_mst_hst_c));
	ret=Dd_mst_hst_Sel(g_pub_tx.reply,&sDd_mst_hst," ac_id=%ld and tx_date<=%ld order by tx_date desc,trace_no desc,trace_cnt desc ",ac_id,beg_date);
	if( ret )
	{
		sprintf(acErrMsg, "查询财付通起始金额失败 [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}
	else
	{
		bal_begin=sDd_mst_hst.bal;
		vtcp_log("%s,%d,在这一查询[%ld]-[%ld]区间之前财付通账户起始余额是[%.2lf]\n",__FILE__,__LINE__,beg_date,end_date,sDd_mst_hst.bal);
	}
	while(beg_date <= end_date)
	{
		tx_amt_sum=0.00;
		ret = sql_sum_double("dd_mst_hst", "tx_amt", &tx_amt_sum, 
			"ac_id='%ld' and tx_date=%ld and brf='财付通网上支付'", ac_id,beg_date);
		if ( ret )
		{
			sprintf(acErrMsg, "统计财付通网上支付金额错误 [%d]", ret);
			WRITEMSG
			goto ErrExit;
		}
		bal_begin=bal_begin+tx_amt_sum;
		fprintf(fp, "%ld|00:00:00|0.00|%.2lf|%.2lf|财付通一点通网上支付|\n",beg_date,tx_amt_sum,bal_begin);
		vtcp_log("%ld|00:00:00|0.00|%.2lf|%.2lf|财付通一点通网上支付|\n",beg_date,tx_amt_sum,bal_begin);
		
		tx_amt_sum=0.00;
		ret = sql_sum_double("dd_mst_hst", "tx_amt", &tx_amt_sum, 
			"ac_id='%ld' and tx_date=%ld and brf='财付通单笔提现'", ac_id,beg_date);
		if ( ret )
		{
			sprintf(acErrMsg, "统计财付通单笔提现金额错误 [%d]", ret);
			WRITEMSG
			goto ErrExit;
		}
		bal_begin=bal_begin-tx_amt_sum;
		fprintf(fp, "%ld|00:00:00|%.2lf|0.00|%.2lf|财付通一点通提现|\n",beg_date,tx_amt_sum,bal_begin);
		vtcp_log("%ld|00:00:00|%.2lf|0.00|%.2lf|财付通一点通提现|\n",beg_date,tx_amt_sum,bal_begin);
		
		tx_amt_sum=0.00;
		ret = sql_sum_double("dd_mst_hst", "tx_amt", &tx_amt_sum, 
			"ac_id='%ld' and tx_date=%ld and brf='财付通快捷支付'", ac_id,beg_date);
		if ( ret )
		{
			sprintf(acErrMsg, "统计财付通快捷支付金额错误 [%d]", ret);
			WRITEMSG
			goto ErrExit;
		}
		bal_begin=bal_begin+tx_amt_sum;
		fprintf(fp, "%ld|00:00:00|0.00|%.2lf|%.2lf|财付通快捷支付|\n",beg_date,tx_amt_sum,bal_begin);
		vtcp_log("%ld|00:00:00|0.00|%.2lf|%.2lf|财付通快捷支付|\n",beg_date,tx_amt_sum,bal_begin);
		
		tx_amt_sum=0.00;
		ret = sql_sum_double("dd_mst_hst", "tx_amt", &tx_amt_sum, 
			"ac_id='%ld' and tx_date=%ld and brf='财付通快捷退货'", ac_id,beg_date);
		if ( ret )
		{
			sprintf(acErrMsg, "统计财付通快捷退货金额错误 [%d]", ret);
			WRITEMSG
			goto ErrExit;
		}
		bal_begin=bal_begin-tx_amt_sum;
		fprintf(fp, "%ld|00:00:00|%.2lf|0.00|%.2lf|财付通快捷退货|\n",beg_date,tx_amt_sum,bal_begin);
		vtcp_log("%ld|00:00:00|%.2lf|0.00|%.2lf|财付通快捷退货|\n",beg_date,tx_amt_sum,bal_begin);
		
		pub_base_deadlineD(beg_date, 1, &beg_date);
		ttlnum++;
	}
	
		fclose(fp);
		set_zd_data( DC_FILE_SND,"1" );

GoodExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before bad return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
