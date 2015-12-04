/*************************************************
* �� �� ��:  sp8602.c
* ���������� ծȨծ���ѯ
*
* ��    ��:  rob
* ������ڣ� 20031212
*
* �޸ļ�¼��
* ��   ��:
* �� �� ��:
* �޸�����:
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
	int ttlnum=0;			/**��ȡ��������**/
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

	 fprintf(fp, "~����|Ʊ�����ֵ���|�����|ͬҵ���|ծȯ��ع�����|������ת���ֻع�����|����ת������|ծȯ���ع�����|ǩ���жһ�Ʊ����|���н��|���|\n");

	while(beg_date <= end_date)
	{
	
		ret = sql_sum_double("mo_discnt", "par_amt", &z1, 
			"not_matr_date=%ld and pn_type='1'", beg_date);
		if ( ret )
		{
			sprintf(acErrMsg, "ͳ�����ֵ��ڽ����� [%d]", ret);
			WRITEMSG
			goto ErrExit;
		}

		ret = sql_sum_double("ln_mst", "ttl_prvd_amt", &z2,
			"mtr_date=%ld", beg_date);
		if ( ret )
		{
			sprintf(acErrMsg, "ͳ�ƴ���ڽ����� [%d]", ret);
			WRITEMSG
			goto ErrExit;
		}

		ret = sql_sum_double("mo_ibl", "amt", &z3, "matr_date=%ld", beg_date);
		if ( ret )
		{
			sprintf(acErrMsg, "ͳ��ͬҵ�������� [%d]", ret);
			WRITEMSG
			goto ErrExit;
		}

		ret = sql_sum_double("mo_stok_rebuy_prot", "bal", &z4, 
			"matr_date=%ld and buy_type='1'", beg_date);
		if ( ret )
		{
			sprintf(acErrMsg, "ͳ��ծȯ��ع����ڽ����� [%d]", ret);
			WRITEMSG
			goto ErrExit;
		}

		ret = sql_sum_double("mo_discnt", "par_amt", &w1, 
			"not_matr_date=%ld and pn_type in ('3', '5', '7')", beg_date);
		if ( ret )
		{
			sprintf(acErrMsg, "ͳ��������ת���ֻع����ڽ����� [%d]", ret);
			WRITEMSG
			goto ErrExit;
		}

		ret = sql_sum_double("mo_discnt", "par_amt", &w2,
			"not_matr_date=%ld and pn_type in ('2', '4', '6')", beg_date);
		if ( ret )
		{
			sprintf(acErrMsg, "ͳ��������ת�������Ͻ����� [%d]", ret);
			WRITEMSG
			goto ErrExit;
		}

		ret = sql_sum_double("mo_stok_rebuy_prot", "bal", &w3, 
			"matr_date=%ld and buy_type='2'", beg_date);
		if ( ret )
		{
			sprintf(acErrMsg, "ͳ��ծȯ���ع����ڽ����� [%d]", ret);
			WRITEMSG
			goto ErrExit;
		}

		ret = sql_sum_double("mo_bank_acc_po", "sign_amt", &w4,
			"matr_date=%ld and acc_po_sts='2'", beg_date);
		if ( ret )
		{
			sprintf(acErrMsg, "ͳ��ǩ���жһ�Ʊ���ڽ����� [%d]", ret);
			WRITEMSG
			goto ErrExit;
		}

		ret = sql_sum_double("mo_again_ln", "amt", &w5,
			"to_date=%ld and sts='1'", beg_date);
		if ( ret )
		{
			sprintf(acErrMsg, "ͳ�����н������� [%d]", ret);
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
