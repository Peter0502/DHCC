/*************************************************
* 文 件 名:  gD339.c
* 功能描述： 财付通汇总明细生成
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
#include"com_sys_parm_c.h"

int gD339()
{
	char filename[100];
	char ftpname[100];
	char lst_date[9];
	int ttlnum=0;			/**读取的总条数**/
	int ttlnum_jj=0;
	int ttlnum_dj=0;
	int ret=0,iRc=0;
	long ac_id=0;
	double tx_amt_sum=0.00;
	double bal_begin=0.00;
	double tx_amt_sum_jj=0.00;
	double tx_amt_sum_dj=0.00;
	
	FILE *fp;
	struct dd_mst_hst_c sDd_mst_hst;
	struct com_sys_parm_c com_sys_parm;
	
	memset(filename,0x00,sizeof(filename));
	memset(ftpname,0x00,sizeof(ftpname));
	memset(lst_date,0x00,sizeof(lst_date));
	memset(&sDd_mst_hst,0x00,sizeof(sDd_mst_hst));
	memset(&com_sys_parm, 0x00, sizeof(struct com_sys_parm_c) );
	
	/**取核心系统时间**/
	iRc = pub_base_GetSysparm(&com_sys_parm);
	if(iRc)
		{
		  vtcp_log("[%s][%d],取核心系统时间出错[%d]!",__FILE__,__LINE__,iRc);
		  strcpy ( g_pub_tx.reply , "G001" );
		  goto ErrExit;
	}
	
	vtcp_log("[%s][%d],取核心上一日期[%ld],核心当前时间[%ld]!",__FILE__,__LINE__,com_sys_parm.lst_date,com_sys_parm.sys_date);

	
	if ( pub_base_acno_acid(&ac_id,"5010439700018") )
	{
	goto ErrExit;
	}
	
	/**sprintf( filename,"%s/tenpay_hzmx%ld.txt",getenv("FILDIR"),com_sys_parm.lst_date);**/
	sprintf( filename,"%s/TENPAYSHORTCUT_%ld.txt",getenv("FILDIR"),com_sys_parm.lst_date);
	sprintf( lst_date,"%ld",com_sys_parm.lst_date);
	vtcp_log( "%s,%d,filename[%s]",__FILE__,__LINE__,filename);
	fp = fopen( filename,"w" ); 
	if( fp==NULL ) 
	{ 
		sprintf(acErrMsg," open file [%s] error ",filename ); 
		WRITEMSG 
		strcpy( g_pub_tx.reply,"S047" ); 
		goto ErrExit; 
	}


	tx_amt_sum_jj=0.00;
	ret = sql_sum_double("dd_mst_hst", "tx_amt", &tx_amt_sum_jj, 
		"ac_id='%ld' and tx_date=%ld and add_ind=0 ", ac_id,com_sys_parm.lst_date);
	if ( ret )
	{
		sprintf(acErrMsg, "统计财付通借记金额错误 [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}
	tx_amt_sum_jj=tx_amt_sum_jj*100;  /**按照财付通的要求，汇总明细金额以分为单位 lzy20140218**/
		
	tx_amt_sum_dj=0.00;
	ret = sql_sum_double("dd_mst_hst", "tx_amt", &tx_amt_sum_dj, 
		"ac_id='%ld' and tx_date=%ld and add_ind=1 ", ac_id,com_sys_parm.lst_date);
	if ( ret )
	{
		sprintf(acErrMsg, "统计财付通贷记金额错误 [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}
	tx_amt_sum_dj=tx_amt_sum_dj*100;  /**按照财付通的要求，汇总明细金额以分为单位 lzy20140218**/

	ttlnum_jj=0;
	ttlnum_jj = sql_count("dd_mst_hst", "ac_id='%ld' and tx_date=%ld and add_ind=0 ", ac_id,com_sys_parm.lst_date);

	ttlnum_dj=0;
	ttlnum_dj = sql_count("dd_mst_hst", "ac_id='%ld' and tx_date=%ld and add_ind=1 ", ac_id,com_sys_parm.lst_date);

	/*fprintf(fp, "总行数|借方总笔数|借方金额合计|贷方总笔数|贷方金额合计|\n");*/
	fprintf(fp, "6|%ld|%.0lf|%ld|%.0lf|\n",ttlnum_jj,tx_amt_sum_jj,ttlnum_dj,tx_amt_sum_dj);
	vtcp_log("6|%ld|%.0lf|%ld|%.0lf|\n",ttlnum_jj,tx_amt_sum_jj,ttlnum_dj,tx_amt_sum_dj);
	
	
	/**求财付通账户昨日账户余额**/
	memset(&sDd_mst_hst,0x00,sizeof(struct dd_mst_hst_c));
	ret=Dd_mst_hst_Sel(g_pub_tx.reply,&sDd_mst_hst," ac_id=%ld and tx_date<%ld order by tx_date desc,trace_no desc,trace_cnt desc ",ac_id,com_sys_parm.lst_date);
	if( ret )
	{
		sprintf(acErrMsg, "查询财付通起始金额失败 [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}
	else
	{
		bal_begin=sDd_mst_hst.bal;
		vtcp_log("%s,%d,查询财付通账户昨日余额是[%.2lf]\n",__FILE__,__LINE__,sDd_mst_hst.bal);
	}
	
	ttlnum = 0;
	ttlnum = sql_count("dd_mst_hst", "ac_id='%ld' and tx_date=%ld and brf='财付通网上支付'", ac_id,com_sys_parm.lst_date);
	
	tx_amt_sum=0.00;
	ret = sql_sum_double("dd_mst_hst", "tx_amt", &tx_amt_sum, 
		"ac_id='%ld' and tx_date=%ld and brf='财付通网上支付'", ac_id,com_sys_parm.lst_date);
	if ( ret )
	{
		sprintf(acErrMsg, "统计财付通网上支付金额错误 [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}

	/*fprintf(fp, "帐务日期|帐务时间|借方笔数|借方金额|贷方笔数|贷方金额|余额|摘要|\n");*/	
	bal_begin=bal_begin+tx_amt_sum;
	fprintf(fp, "%ld|00:00:00|0|0|%ld|%.0lf|%.0lf|财付通一点通网上支付|\n",com_sys_parm.lst_date,ttlnum,tx_amt_sum*100,bal_begin*100);
	vtcp_log("%ld|00:00:00|0|0|%ld|%.0lf|%.0lf|财付通一点通网上支付|\n",com_sys_parm.lst_date,ttlnum,tx_amt_sum*100,bal_begin*100);
	/**按照财付通的要求，汇总明细金额以分为单位,以下皆同 lzy20140218**/
	
	ttlnum = 0;
	ttlnum = sql_count("dd_mst_hst", "ac_id='%ld' and tx_date=%ld and brf='财付通单笔提现'", ac_id,com_sys_parm.lst_date);
	
	tx_amt_sum=0.00;
	ret = sql_sum_double("dd_mst_hst", "tx_amt", &tx_amt_sum, 
		"ac_id='%ld' and tx_date=%ld and brf='财付通单笔提现'", ac_id,com_sys_parm.lst_date);
	if ( ret )
	{
		sprintf(acErrMsg, "统计财付通单笔提现金额错误 [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}
	bal_begin=bal_begin-tx_amt_sum;
	fprintf(fp, "%ld|00:00:00|%ld|%.0lf|0|0|%.0lf|财付通一点通提现|\n",com_sys_parm.lst_date,ttlnum,tx_amt_sum*100,bal_begin*100);
	vtcp_log("%ld|00:00:00|%ld|%.0lf|0|0|%.0lf|财付通一点通提现|\n",com_sys_parm.lst_date,ttlnum,tx_amt_sum*100,bal_begin*100);
	
	
	ttlnum = 0;
	ttlnum = sql_count("dd_mst_hst", "ac_id='%ld' and tx_date=%ld and brf='财付通快捷支付'", ac_id,com_sys_parm.lst_date);
	
	tx_amt_sum=0.00;
	ret = sql_sum_double("dd_mst_hst", "tx_amt", &tx_amt_sum, 
		"ac_id='%ld' and tx_date=%ld and brf='财付通快捷支付'", ac_id,com_sys_parm.lst_date);
	if ( ret )
	{
		sprintf(acErrMsg, "统计财付通快捷支付金额错误 [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}
	
	bal_begin=bal_begin+tx_amt_sum;
	fprintf(fp, "%ld|00:00:00|0|0|%ld|%.0lf|%.0lf|财付通快捷支付|\n",com_sys_parm.lst_date,ttlnum,tx_amt_sum*100,bal_begin*100);
	vtcp_log("%ld|00:00:00|0|0|%ld|%.0lf|%.0lf|财付通快捷支付|\n",com_sys_parm.lst_date,ttlnum,tx_amt_sum*100,bal_begin*100);
	
	
	ttlnum = 0;
	ttlnum = sql_count("dd_mst_hst", "ac_id='%ld' and tx_date=%ld and brf='财付通快捷退货'", ac_id,com_sys_parm.lst_date);
	
	tx_amt_sum=0.00;
	ret = sql_sum_double("dd_mst_hst", "tx_amt", &tx_amt_sum, 
		"ac_id='%ld' and tx_date=%ld and brf='财付通快捷退货'", ac_id,com_sys_parm.lst_date);
	if ( ret )
	{
		sprintf(acErrMsg, "统计财付通快捷退货金额错误 [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}
	
	bal_begin=bal_begin-tx_amt_sum;
	fprintf(fp, "%ld|00:00:00|%ld|%.0lf|0|0|%.0lf|财付通快捷退货|\n",com_sys_parm.lst_date,ttlnum,tx_amt_sum*100,bal_begin*100);
	vtcp_log("%ld|00:00:00|%ld|%.0lf|0|0|%.0lf|财付通快捷退货|\n",com_sys_parm.lst_date,ttlnum,tx_amt_sum*100,bal_begin*100);
	

	ttlnum = 0;
	ttlnum = sql_count("dd_mst_hst", "ac_id='%ld' and tx_date=%ld and  add_ind=1 and tx_code!='6203' ", ac_id,com_sys_parm.lst_date);
	
	tx_amt_sum=0.00;
	ret = sql_sum_double("dd_mst_hst", "tx_amt", &tx_amt_sum, 
		"ac_id='%ld' and tx_date=%ld and  add_ind=1 and  tx_code!='6203' ", ac_id,com_sys_parm.lst_date);
	if ( ret )
	{
		sprintf(acErrMsg, "统计资金结转转入金额错误 [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}

	bal_begin=bal_begin+tx_amt_sum;
	fprintf(fp, "%ld|00:00:00|0|0|%ld|%.0lf|%.0lf|资金结转转入|\n",com_sys_parm.lst_date,ttlnum,tx_amt_sum*100,bal_begin*100);
	vtcp_log("%ld|00:00:00|0|0|%ld|%.0lf|%.0lf|资金结转转入|\n",com_sys_parm.lst_date,ttlnum,tx_amt_sum*100,bal_begin*100);

	
	ttlnum = 0;
	ttlnum = sql_count("dd_mst_hst", "ac_id='%ld' and tx_date=%ld and  add_ind=0 and tx_code!='6203' ", ac_id,com_sys_parm.lst_date);
	
	tx_amt_sum=0.00;
	ret = sql_sum_double("dd_mst_hst", "tx_amt", &tx_amt_sum, 
		"ac_id='%ld' and tx_date=%ld and add_ind=0 and tx_code!='6203' ", ac_id,com_sys_parm.lst_date);
	if ( ret )
	{
		sprintf(acErrMsg, "统计资金结转转出金额错误 [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}
	bal_begin=bal_begin-tx_amt_sum;
	fprintf(fp, "%ld|00:00:00|%ld|%.0lf|0|0|%.0lf|资金结转转出|\n",com_sys_parm.lst_date,ttlnum,tx_amt_sum*100,bal_begin*100);
	vtcp_log("%ld|00:00:00|%ld|%.0lf|0|0|%.0lf|资金结转转出|\n",com_sys_parm.lst_date,ttlnum,tx_amt_sum*100,bal_begin*100);

	
	fclose(fp);

	sprintf( ftpname,"%s/gD339ftp.sh",getenv("BINDIR"));
	ret=system( ftpname );
		if (ret != 0)
			{
				sprintf(acErrMsg,"gD339ftp error %s %d",ftpname,ret);
				WRITEMSG
				return ret;
			}


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