/*************************************************
* �� �� ��:  sp8704.c
* ���������� ���ʱ��ѯ
*
* ��    ��:  
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
#include "com_rate_c.h"

int sp8704()
{
	struct com_rate_c vcom_rate;
	struct com_rate_c f_com_rate;
		char filename[100];
		char wherelist[1024];  /**��ѯ����**/
		char tmpstr[512];
		char tmpmo[512];
		char fieldlist[1024];  /**Ҫ��ѯ���ֶ��б�**/
		char tablelist[128];   /**Ҫ��ѯ�ı���**/
		char titstr[1024];
		int ttlnum=0;			/**��ȡ��������**/
		int i=0;
		int ret=0;
		FILE *fp;

	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset( wherelist,0,sizeof(wherelist) );
	memset( fieldlist,0,sizeof(fieldlist) );
	memset( tablelist,0,sizeof(tablelist) );

	pub_base_sysinit();

	get_zd_data("0210",f_com_rate.cur_no );
	get_zd_data("0240",f_com_rate.rate_code );
	get_zd_long("0290",&f_com_rate.beg_date );
	get_zd_long("0440",&f_com_rate.end_date );

	/**��ɲ�ѯ����**/
	pub_base_strpack( f_com_rate.cur_no );
	if( strlen(f_com_rate.cur_no) )
	{
		sprintf( tmpstr," cur_no='%s' and",f_com_rate.cur_no );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_com_rate.rate_code );
	if( strlen(f_com_rate.rate_code) )
	{
		sprintf( tmpstr," rate_code='%s' and",f_com_rate.rate_code );
		strcat( wherelist,tmpstr );
	}
	if( f_com_rate.beg_date )
	{
		sprintf( tmpstr," beg_date>=%ld and",f_com_rate.beg_date );
		strcat( wherelist,tmpstr );
	}
	if( f_com_rate.end_date )
	{
		sprintf( tmpstr," end_date<=%ld and",f_com_rate.end_date );
		strcat( wherelist,tmpstr );
	}

	strcpy( tablelist,"com_rate" );

	/**��ɲ�ѯ**/
	strcat(wherelist," 1=1 order by rate_code, beg_date, end_date") ;

	vtcp_log( "WHERE [%s]",wherelist );
	strcpy( fieldlist,"cur_no,rate_code,name,beg_date,end_date,rate_val,rate_type,rate_unit,fl_rate_code,ref_rate_code,ref_ratio");

	ret=Com_rate_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

		ttlnum=0;

	while(1)
	{
		ret=Com_rate_Fet_Sel( &vcom_rate, g_pub_tx.reply );
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
			fprintf( fp,"~@����|@���ʱ��|@����|��������|ֹ������|����ֵ|��������|���ʵ�λ|@�µ����ʱ��|@�ο����ʱ��|�ο�����|\n" );
		}

		/*pub_pubdb_prt_fld( fp,tablelist,fieldlist ); */

		if(vcom_rate.beg_date<-2147000000) vcom_rate.beg_date=0;
		if(vcom_rate.end_date<-2147000000) vcom_rate.end_date=0;
		if(vcom_rate.rate_val<-2147000000) vcom_rate.rate_val=0.0;
		if(vcom_rate.rate_unit<-2147000000) vcom_rate.rate_unit=0;
		if(vcom_rate.ref_ratio<-2147000000) vcom_rate.ref_ratio=0.0;

		fprintf( fp, "%s|%s|%s|%ld|%ld|%.5lf|%d|%ld|%s|%s|%.5lf|\n",vcom_rate.cur_no,vcom_rate.rate_code,vcom_rate.name,vcom_rate.beg_date,vcom_rate.end_date,vcom_rate.rate_val,vcom_rate.rate_type,vcom_rate.rate_unit,vcom_rate.fl_rate_code,vcom_rate.ref_rate_code,vcom_rate.ref_ratio);
		ttlnum++;
	}

	ret=Com_rate_Clo_Sel( );
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
