/*************************************************
* �� �� ��:  sp8402.c
* ���������� �ֽ�Ǽǲ���ѯ
*
* ��    ��:  peter
* ������ڣ� 
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
#include "cash_mst_hst_c.h"
#include "trace_log_c.h"

int sp8402()
{
	struct cash_mst_hst_c vcash_mst_hst;
	struct cash_mst_hst_c f_cash_mst_hst;
	struct cash_mst_hst_c f_cash_mst_hsta;
	struct trace_log_c trace_log;

	char f_add_ind6[41];
		char filename[100];
		char wherelist[1024];  /**��ѯ����**/
		char tmpstr[512];
		char tmpmo[512];
		char fieldlist[1024];  /**Ҫ��ѯ���ֶ��б�**/
		char tablelist[128];   /**Ҫ��ѯ�ı���**/
		char titstr[1024];
		char f_cur_name[11];
		int ttlnum=0;			/**��ȡ��������**/
		int i=0;
		int ret=0;
		FILE *fp;

	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset( wherelist,0,sizeof(wherelist) );
	memset( fieldlist,0,sizeof(fieldlist) );
	memset( tablelist,0,sizeof(tablelist) );

	pub_base_sysinit();

	get_zd_data("0910",f_cash_mst_hst.br_no );
	get_zd_data("0380",f_cash_mst_hst.ac_no );
	get_zd_data("0640",f_cash_mst_hst.tel );
	get_zd_data("0210",f_cash_mst_hst.cur_no );
	get_zd_data("0700",f_cash_mst_hst.add_ind );    /***��������ȯ��ǼǱ�־***/
	get_zd_double("1001",&f_cash_mst_hst.amt );
	get_zd_long("0290",&f_cash_mst_hst.tx_date );
	get_zd_long("0440",&f_cash_mst_hsta.tx_date );
	get_zd_data("0630",f_cash_mst_hst.brf );


	/**��ɲ�ѯ����**/
	pub_base_strpack( f_cash_mst_hst.br_no );
	if( strlen(f_cash_mst_hst.br_no) )
	{
		sprintf( tmpstr," br_no='%s' and",f_cash_mst_hst.br_no );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_cash_mst_hst.ac_no );
	if( strlen(f_cash_mst_hst.ac_no) )
	{
		sprintf( tmpstr," ac_no='%s' and",f_cash_mst_hst.ac_no );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_cash_mst_hst.tel );
	if( strlen(f_cash_mst_hst.tel) )
	{
		sprintf( tmpstr," tel='%s' and",f_cash_mst_hst.tel );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_cash_mst_hst.cur_no );
	if( strlen(f_cash_mst_hst.cur_no) )
	{
		sprintf( tmpstr," cur_no='%s' and",f_cash_mst_hst.cur_no );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_cash_mst_hst.add_ind );
	if( strlen(f_cash_mst_hst.add_ind) )
	{
		sprintf( tmpstr," add_ind='%s' and",f_cash_mst_hst.add_ind );
		strcat( wherelist,tmpstr );
	}else{
		sprintf( tmpstr," add_ind!='9' and");
		strcat( wherelist,tmpstr );
	}
	if( f_cash_mst_hst.amt )
	{
		sprintf( tmpstr," amt=%lf and",f_cash_mst_hst.amt );
		strcat( wherelist,tmpstr );
	}
	if( f_cash_mst_hst.tx_date )
	{
		sprintf( tmpstr," tx_date>=%ld and",f_cash_mst_hst.tx_date );
		strcat( wherelist,tmpstr );
	}
	if( f_cash_mst_hsta.tx_date )
	{
		sprintf( tmpstr," tx_date<=%ld and",f_cash_mst_hsta.tx_date );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_cash_mst_hst.brf );
	if( strlen(f_cash_mst_hst.brf) )
	{
		sprintf( tmpstr," brf like '%%%%%s%%%%' and",f_cash_mst_hst.brf);
		strcat( wherelist,tmpstr );
	}

	strcpy( tablelist,"cash_mst_hst" );


	/**��ɲ�ѯ**/
	strcat(wherelist," 1=1 ORDER BY tx_date,tel,trace_no,trace_cnt ") ;

	vtcp_log( "WHERE [%s]",wherelist );
	strcpy( fieldlist,"trace_no,trace_cnt,br_no,tel,ac_no,cur_no,add_ind,amt,tx_date,tx_time,brf");

	ret=Cash_mst_hst_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

		ttlnum=0;

	while(1)
	{
		ret=Cash_mst_hst_Fet_Sel( &vcash_mst_hst, g_pub_tx.reply );
		if( ret==100 ) break;
		ERR_DEAL


		if( !ttlnum )
		{
			pub_base_AllDwnFilName( filename );
			fp = fopen( filename,"w" );
			if( fp==NULL )
			{
				sprintf(acErrMsg," open file [%s] error ",filename );
				WRITEMSG
				strcpy( g_pub_tx.reply,"S047" );
				goto ErrExit;
			}

			/**��ʾ�б���**/
			fprintf( fp,"~@�ո�|@  ��    ��|@�Է��˺�|$  ��  ��  ��|@��  ��|@ժ    Ҫ|��ˮ����|����ʱ��|@����|@��Ա|��ˮ��|�ʴ�|\n" );
		}

		/*pub_pubdb_prt_fld( fp,tablelist,fieldlist ); */


		if(vcash_mst_hst.trace_no<-2147000000) vcash_mst_hst.trace_no=0;
		if(vcash_mst_hst.trace_cnt<-2147000000) vcash_mst_hst.trace_cnt=0;
		pub_base_dic_show_str(f_add_ind6,"income_pay",vcash_mst_hst.add_ind);
		if(vcash_mst_hst.amt<-2147000000) vcash_mst_hst.amt=0.0;
		if(vcash_mst_hst.tx_date<-2147000000) vcash_mst_hst.tx_date=0;
		if(vcash_mst_hst.tx_time<-2147000000) vcash_mst_hst.tx_time=0;
		pub_base_dic_show_str(f_cur_name,"cur_name",vcash_mst_hst.cur_no);

		/***modify by wanglei 20070416 ����Ա�Ǽǹ�ȯ������ǰ̨��ʾȯ������***/
		vtcp_log("[%s][%d] add_ind = [%s]\n",__FILE__,__LINE__,vcash_mst_hst.add_ind);
		if(vcash_mst_hst.add_ind[0] != '9')
		{
			fprintf( fp, "%s|%s|%s|%.2lf|%s|%s|%ld|%ld|%s|%s|%ld|%ld|\n",f_add_ind6,vcash_mst_hst.ac_no,vcash_mst_hst.tw_ac_no,vcash_mst_hst.amt,f_cur_name,vcash_mst_hst.brf,vcash_mst_hst.tx_date,vcash_mst_hst.tx_time,vcash_mst_hst.br_no,vcash_mst_hst.tel,vcash_mst_hst.trace_no,vcash_mst_hst.trace_cnt);
		}
		else
		{
			fprintf( fp, "%s|%s|%s|%.2lf|%s|%s|%ld|%ld|%s|%s|%ld|%ld|%ld|%ld|%ld|%ld|%ld|%ld|%ld|%ld|%ld|%ld|%ld|%ld|%ld|\n",f_add_ind6,vcash_mst_hst.ac_no,vcash_mst_hst.tw_ac_no,vcash_mst_hst.amt,f_cur_name,vcash_mst_hst.brf,vcash_mst_hst.tx_date,vcash_mst_hst.tx_time,vcash_mst_hst.br_no,vcash_mst_hst.tel,vcash_mst_hst.trace_no,vcash_mst_hst.trace_cnt,vcash_mst_hst.t10000,vcash_mst_hst.t5000,vcash_mst_hst.t2000,vcash_mst_hst.t1000,vcash_mst_hst.t500,vcash_mst_hst.t200,vcash_mst_hst.t100,vcash_mst_hst.t50,vcash_mst_hst.t20,vcash_mst_hst.t10,vcash_mst_hst.t5,vcash_mst_hst.t2,vcash_mst_hst.t1);
		}
		/***end by wanglei***/


		ttlnum++;
	}

	ret=Cash_mst_hst_Clo_Sel( );
	ERR_DEAL

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
