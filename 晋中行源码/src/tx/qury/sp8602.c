/*************************************************
* 文 件 名:  sp8602.c
* 功能描述： 债权债务查询
*
* 作    者:  rob
* 完成日期： 20031212
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

int sp8602()
{
	char filename[100];
	int ttlnum=0;			/**读取的总条数**/
	int i=0;
	int ret=0;
	FILE *fp;
	long beg_date=0, end_date=0;
	double z1=0.00, z2=0.00, z3=0.00, z4=0.00, w1=0.00, w2=0.00, w3=0.00;
	double w4=0.00, w5=0.00, c1=0.00;

	memset( &g_pub_tx,0,sizeof(g_pub_tx) );

	pub_base_sysinit();

	get_zd_long("0440",&beg_date );
	get_zd_long("0450",&end_date );

	ttlnum = 0;

	pub_base_AllDwnFilName( filename ); 
	fp = fopen( filename,"w" ); 
	if( fp==NULL ) 
	{ 
		sprintf(acErrMsg," open file [%s] error ",filename ); 
		WRITEMSG 
		strcpy( g_pub_tx.reply,"S047" ); 
		goto ErrExit; 
	}

	 fprintf(fp, "~日期|票据贴现到期|贷款到期|同业拆借|债券逆回购到期|再贴现转贴现回购到期|再贴转贴卖断|债券正回购到期|签发承兑汇票到期|人行借款|差额|\n");

	while(beg_date <= end_date)
	{
	
		ret = sql_sum_double("mo_discnt", "par_amt", &z1, 
			"not_matr_date=%ld and pn_type='1'", beg_date);
		if ( ret )
		{
			sprintf(acErrMsg, "统计贴现到期金额错误 [%d]", ret);
			WRITEMSG
			goto ErrExit;
		}

		ret = sql_sum_double("ln_mst", "ttl_prvd_amt", &z2,
			"mtr_date=%ld", beg_date);
		if ( ret )
		{
			sprintf(acErrMsg, "统计贷款到期金额错误 [%d]", ret);
			WRITEMSG
			goto ErrExit;
		}

		ret = sql_sum_double("mo_ibl", "amt", &z3, "matr_date=%ld", beg_date);
		if ( ret )
		{
			sprintf(acErrMsg, "统计同业拆借金额错误 [%d]", ret);
			WRITEMSG
			goto ErrExit;
		}

		ret = sql_sum_double("mo_stok_rebuy_prot", "bal", &z4, 
			"matr_date=%ld and buy_type='1'", beg_date);
		if ( ret )
		{
			sprintf(acErrMsg, "统计债券逆回购到期金额错误 [%d]", ret);
			WRITEMSG
			goto ErrExit;
		}

		ret = sql_sum_double("mo_discnt", "par_amt", &w1, 
			"not_matr_date=%ld and pn_type in ('3', '5', '7')", beg_date);
		if ( ret )
		{
			sprintf(acErrMsg, "统计再贴现转贴现回购到期金额错误 [%d]", ret);
			WRITEMSG
			goto ErrExit;
		}

		ret = sql_sum_double("mo_discnt", "par_amt", &w2,
			"not_matr_date=%ld and pn_type in ('2', '4', '6')", beg_date);
		if ( ret )
		{
			sprintf(acErrMsg, "统计再贴现转贴现卖断金额错误 [%d]", ret);
			WRITEMSG
			goto ErrExit;
		}

		ret = sql_sum_double("mo_stok_rebuy_prot", "bal", &w3, 
			"matr_date=%ld and buy_type='2'", beg_date);
		if ( ret )
		{
			sprintf(acErrMsg, "统计债券正回购到期金额错误 [%d]", ret);
			WRITEMSG
			goto ErrExit;
		}

		ret = sql_sum_double("mo_bank_acc_po", "sign_amt", &w4,
			"matr_date=%ld and acc_po_sts='2'", beg_date);
		if ( ret )
		{
			sprintf(acErrMsg, "统计签发承兑汇票到期金额错误 [%d]", ret);
			WRITEMSG
			goto ErrExit;
		}

		ret = sql_sum_double("mo_again_ln", "amt", &w5,
			"to_date=%ld and sts='1'", beg_date);
		if ( ret )
		{
			sprintf(acErrMsg, "统计人行借款金额错误 [%d]", ret);
			WRITEMSG
			goto ErrExit;
		}
		c1=z1+z2+z3+z4-w1-w2-w3-w4-w5;

		fprintf(fp, "%ld|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|\n",beg_date,z1,z2,z3,z4,-w1,-w2,-w3,-w4,-w5,c1);


		pub_base_deadlineD(beg_date, 1, &beg_date);
		ttlnum++;
	}

	if( !ttlnum )
	{
		strcpy( g_pub_tx.reply,"S049" );
		goto ErrExit;
	}
	else
	{
		fclose(fp);
		set_zd_data( DC_FILE_SND,"1" );
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
