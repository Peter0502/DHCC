/*************************************************
* �� �� ��:  gD339.c
* ���������� �Ƹ�ͨ������ϸ����
*
* ��    ��:  Huzhaohua
* ������ڣ� 201311128
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
#include "dd_mst_hst_c.h"
#include"com_sys_parm_c.h"

int gD339()
{
	char filename[100];
	char ftpname[100];
	char lst_date[9];
	int ttlnum=0;			/**��ȡ��������**/
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
	
	/**ȡ����ϵͳʱ��**/
	iRc = pub_base_GetSysparm(&com_sys_parm);
	if(iRc)
		{
		  vtcp_log("[%s][%d],ȡ����ϵͳʱ�����[%d]!",__FILE__,__LINE__,iRc);
		  strcpy ( g_pub_tx.reply , "G001" );
		  goto ErrExit;
	}
	
	vtcp_log("[%s][%d],ȡ������һ����[%ld],���ĵ�ǰʱ��[%ld]!",__FILE__,__LINE__,com_sys_parm.lst_date,com_sys_parm.sys_date);

	
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
		sprintf(acErrMsg, "ͳ�ƲƸ�ͨ��ǽ����� [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}
	tx_amt_sum_jj=tx_amt_sum_jj*100;  /**���ղƸ�ͨ��Ҫ�󣬻�����ϸ����Է�Ϊ��λ lzy20140218**/
		
	tx_amt_sum_dj=0.00;
	ret = sql_sum_double("dd_mst_hst", "tx_amt", &tx_amt_sum_dj, 
		"ac_id='%ld' and tx_date=%ld and add_ind=1 ", ac_id,com_sys_parm.lst_date);
	if ( ret )
	{
		sprintf(acErrMsg, "ͳ�ƲƸ�ͨ���ǽ����� [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}
	tx_amt_sum_dj=tx_amt_sum_dj*100;  /**���ղƸ�ͨ��Ҫ�󣬻�����ϸ����Է�Ϊ��λ lzy20140218**/

	ttlnum_jj=0;
	ttlnum_jj = sql_count("dd_mst_hst", "ac_id='%ld' and tx_date=%ld and add_ind=0 ", ac_id,com_sys_parm.lst_date);

	ttlnum_dj=0;
	ttlnum_dj = sql_count("dd_mst_hst", "ac_id='%ld' and tx_date=%ld and add_ind=1 ", ac_id,com_sys_parm.lst_date);

	/*fprintf(fp, "������|�跽�ܱ���|�跽���ϼ�|�����ܱ���|�������ϼ�|\n");*/
	fprintf(fp, "6|%ld|%.0lf|%ld|%.0lf|\n",ttlnum_jj,tx_amt_sum_jj,ttlnum_dj,tx_amt_sum_dj);
	vtcp_log("6|%ld|%.0lf|%ld|%.0lf|\n",ttlnum_jj,tx_amt_sum_jj,ttlnum_dj,tx_amt_sum_dj);
	
	
	/**��Ƹ�ͨ�˻������˻����**/
	memset(&sDd_mst_hst,0x00,sizeof(struct dd_mst_hst_c));
	ret=Dd_mst_hst_Sel(g_pub_tx.reply,&sDd_mst_hst," ac_id=%ld and tx_date<%ld order by tx_date desc,trace_no desc,trace_cnt desc ",ac_id,com_sys_parm.lst_date);
	if( ret )
	{
		sprintf(acErrMsg, "��ѯ�Ƹ�ͨ��ʼ���ʧ�� [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}
	else
	{
		bal_begin=sDd_mst_hst.bal;
		vtcp_log("%s,%d,��ѯ�Ƹ�ͨ�˻����������[%.2lf]\n",__FILE__,__LINE__,sDd_mst_hst.bal);
	}
	
	ttlnum = 0;
	ttlnum = sql_count("dd_mst_hst", "ac_id='%ld' and tx_date=%ld and brf='�Ƹ�ͨ����֧��'", ac_id,com_sys_parm.lst_date);
	
	tx_amt_sum=0.00;
	ret = sql_sum_double("dd_mst_hst", "tx_amt", &tx_amt_sum, 
		"ac_id='%ld' and tx_date=%ld and brf='�Ƹ�ͨ����֧��'", ac_id,com_sys_parm.lst_date);
	if ( ret )
	{
		sprintf(acErrMsg, "ͳ�ƲƸ�ͨ����֧�������� [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}

	/*fprintf(fp, "��������|����ʱ��|�跽����|�跽���|��������|�������|���|ժҪ|\n");*/	
	bal_begin=bal_begin+tx_amt_sum;
	fprintf(fp, "%ld|00:00:00|0|0|%ld|%.0lf|%.0lf|�Ƹ�ͨһ��ͨ����֧��|\n",com_sys_parm.lst_date,ttlnum,tx_amt_sum*100,bal_begin*100);
	vtcp_log("%ld|00:00:00|0|0|%ld|%.0lf|%.0lf|�Ƹ�ͨһ��ͨ����֧��|\n",com_sys_parm.lst_date,ttlnum,tx_amt_sum*100,bal_begin*100);
	/**���ղƸ�ͨ��Ҫ�󣬻�����ϸ����Է�Ϊ��λ,���½�ͬ lzy20140218**/
	
	ttlnum = 0;
	ttlnum = sql_count("dd_mst_hst", "ac_id='%ld' and tx_date=%ld and brf='�Ƹ�ͨ��������'", ac_id,com_sys_parm.lst_date);
	
	tx_amt_sum=0.00;
	ret = sql_sum_double("dd_mst_hst", "tx_amt", &tx_amt_sum, 
		"ac_id='%ld' and tx_date=%ld and brf='�Ƹ�ͨ��������'", ac_id,com_sys_parm.lst_date);
	if ( ret )
	{
		sprintf(acErrMsg, "ͳ�ƲƸ�ͨ�������ֽ����� [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}
	bal_begin=bal_begin-tx_amt_sum;
	fprintf(fp, "%ld|00:00:00|%ld|%.0lf|0|0|%.0lf|�Ƹ�ͨһ��ͨ����|\n",com_sys_parm.lst_date,ttlnum,tx_amt_sum*100,bal_begin*100);
	vtcp_log("%ld|00:00:00|%ld|%.0lf|0|0|%.0lf|�Ƹ�ͨһ��ͨ����|\n",com_sys_parm.lst_date,ttlnum,tx_amt_sum*100,bal_begin*100);
	
	
	ttlnum = 0;
	ttlnum = sql_count("dd_mst_hst", "ac_id='%ld' and tx_date=%ld and brf='�Ƹ�ͨ���֧��'", ac_id,com_sys_parm.lst_date);
	
	tx_amt_sum=0.00;
	ret = sql_sum_double("dd_mst_hst", "tx_amt", &tx_amt_sum, 
		"ac_id='%ld' and tx_date=%ld and brf='�Ƹ�ͨ���֧��'", ac_id,com_sys_parm.lst_date);
	if ( ret )
	{
		sprintf(acErrMsg, "ͳ�ƲƸ�ͨ���֧�������� [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}
	
	bal_begin=bal_begin+tx_amt_sum;
	fprintf(fp, "%ld|00:00:00|0|0|%ld|%.0lf|%.0lf|�Ƹ�ͨ���֧��|\n",com_sys_parm.lst_date,ttlnum,tx_amt_sum*100,bal_begin*100);
	vtcp_log("%ld|00:00:00|0|0|%ld|%.0lf|%.0lf|�Ƹ�ͨ���֧��|\n",com_sys_parm.lst_date,ttlnum,tx_amt_sum*100,bal_begin*100);
	
	
	ttlnum = 0;
	ttlnum = sql_count("dd_mst_hst", "ac_id='%ld' and tx_date=%ld and brf='�Ƹ�ͨ����˻�'", ac_id,com_sys_parm.lst_date);
	
	tx_amt_sum=0.00;
	ret = sql_sum_double("dd_mst_hst", "tx_amt", &tx_amt_sum, 
		"ac_id='%ld' and tx_date=%ld and brf='�Ƹ�ͨ����˻�'", ac_id,com_sys_parm.lst_date);
	if ( ret )
	{
		sprintf(acErrMsg, "ͳ�ƲƸ�ͨ����˻������� [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}
	
	bal_begin=bal_begin-tx_amt_sum;
	fprintf(fp, "%ld|00:00:00|%ld|%.0lf|0|0|%.0lf|�Ƹ�ͨ����˻�|\n",com_sys_parm.lst_date,ttlnum,tx_amt_sum*100,bal_begin*100);
	vtcp_log("%ld|00:00:00|%ld|%.0lf|0|0|%.0lf|�Ƹ�ͨ����˻�|\n",com_sys_parm.lst_date,ttlnum,tx_amt_sum*100,bal_begin*100);
	

	ttlnum = 0;
	ttlnum = sql_count("dd_mst_hst", "ac_id='%ld' and tx_date=%ld and  add_ind=1 and tx_code!='6203' ", ac_id,com_sys_parm.lst_date);
	
	tx_amt_sum=0.00;
	ret = sql_sum_double("dd_mst_hst", "tx_amt", &tx_amt_sum, 
		"ac_id='%ld' and tx_date=%ld and  add_ind=1 and  tx_code!='6203' ", ac_id,com_sys_parm.lst_date);
	if ( ret )
	{
		sprintf(acErrMsg, "ͳ���ʽ��תת������� [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}

	bal_begin=bal_begin+tx_amt_sum;
	fprintf(fp, "%ld|00:00:00|0|0|%ld|%.0lf|%.0lf|�ʽ��תת��|\n",com_sys_parm.lst_date,ttlnum,tx_amt_sum*100,bal_begin*100);
	vtcp_log("%ld|00:00:00|0|0|%ld|%.0lf|%.0lf|�ʽ��תת��|\n",com_sys_parm.lst_date,ttlnum,tx_amt_sum*100,bal_begin*100);

	
	ttlnum = 0;
	ttlnum = sql_count("dd_mst_hst", "ac_id='%ld' and tx_date=%ld and  add_ind=0 and tx_code!='6203' ", ac_id,com_sys_parm.lst_date);
	
	tx_amt_sum=0.00;
	ret = sql_sum_double("dd_mst_hst", "tx_amt", &tx_amt_sum, 
		"ac_id='%ld' and tx_date=%ld and add_ind=0 and tx_code!='6203' ", ac_id,com_sys_parm.lst_date);
	if ( ret )
	{
		sprintf(acErrMsg, "ͳ���ʽ��תת�������� [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}
	bal_begin=bal_begin-tx_amt_sum;
	fprintf(fp, "%ld|00:00:00|%ld|%.0lf|0|0|%.0lf|�ʽ��תת��|\n",com_sys_parm.lst_date,ttlnum,tx_amt_sum*100,bal_begin*100);
	vtcp_log("%ld|00:00:00|%ld|%.0lf|0|0|%.0lf|�ʽ��תת��|\n",com_sys_parm.lst_date,ttlnum,tx_amt_sum*100,bal_begin*100);

	
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