/*************************************************
* �� �� ��:  sp8921.c
* ���������� �����ֽ�������ѯ
*
* ��    ��:  jane
* ������ڣ� 2003��7��26��
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
#include "cash_book_c.h"
#include "trace_log_c.h"
#include "trace_log_rz_c.h"

int sp8921()
{
	struct cash_book_c vcash_book;
	struct cash_book_c f_cash_book;
	struct cash_book_c f_cash_booka;
	struct trace_log_c f_trace_log;
	struct trace_log_rz_c f_trace_log_rz;
	char f_ind7[41];
		char filename[100];
		char wherelist[1024];  /**��ѯ����**/
		char tmpstr[512];
		char tmpmo[512];
		char fieldlist[1024];  /**Ҫ��ѯ���ֶ��б�**/
		char tablelist[128];   /**Ҫ��ѯ�ı���**/
		char titstr[1024];
		int ttlnum=0;			/**��ȡ��������**/
		int i=0;
		int ret=0,ret1=0;
		FILE *fp;

	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset( wherelist,0,sizeof(wherelist) );
	memset( fieldlist,0,sizeof(fieldlist) );
	memset( tablelist,0,sizeof(tablelist) );
	memset( &f_trace_log,0x00,sizeof( struct trace_log_c));
	memset( &f_trace_log_rz,0x00,sizeof( struct trace_log_rz_c));

	pub_base_sysinit();

	get_zd_long("0290",&f_cash_book.trace_no );
	get_zd_long("0440",&f_cash_book.tx_date );
	get_zd_long("0450",&f_cash_booka.tx_date );
	get_zd_data("0910",f_cash_book.br_no );
	get_zd_data("0640",f_cash_book.tel );
	get_zd_data("0650",f_cash_book.tw_br_no );
	get_zd_data("0210",f_cash_book.cur_no );
	get_zd_data("0670",f_cash_book.ind );
	get_zd_double("0400",&f_cash_book.amt );
	get_zd_double("0410",&f_cash_booka.amt );
	memset(tmpstr, 0 , sizeof(tmpstr));
	/**��ɲ�ѯ����**/
	if( f_cash_book.trace_no )
	{
		sprintf( tmpstr," trace_no=%ld and",f_cash_book.trace_no );
		strcat( wherelist,tmpstr );
	}
	if( f_cash_book.tx_date )
	{
		sprintf( tmpstr," tx_date>=%ld and",f_cash_book.tx_date );
		strcat( wherelist,tmpstr );
	}
	if( f_cash_booka.tx_date )
	{
		sprintf( tmpstr," tx_date<=%ld and",f_cash_booka.tx_date );
		strcat( wherelist,tmpstr );
	}
	/***�޸�Ϊ(��������)���Բ����еģ�ÿ��֧��ֻ�ܲ��Լ��� modified by wanglei 20061031 ***/
	if(strcmp(g_pub_tx.tx_br_no,QS_BR_NO)==0)
	{
		pub_base_strpack( f_cash_book.br_no );
		if( strlen(f_cash_book.br_no) )
		{
			sprintf( tmpstr," br_no='%s' and",f_cash_book.br_no );
			strcat( wherelist,tmpstr );
		}
		pub_base_strpack( f_cash_book.tw_br_no );
		if( strlen(f_cash_book.tw_br_no) )
		{
			sprintf( tmpstr," tw_br_no='%s' and",f_cash_book.tw_br_no );
			strcat( wherelist,tmpstr );
		}
	}
	else
	{
		pub_base_strpack( f_cash_book.br_no );
		if( strlen(f_cash_book.br_no) )
		{
			sprintf( tmpstr," br_no='%s' and",f_cash_book.br_no );
			strcat( wherelist,tmpstr );
		}
		else
		{
			sprintf( tmpstr," br_no='%s' and",g_pub_tx.tx_br_no );
			strcat( wherelist,tmpstr );
		}
	}
	pub_base_strpack( f_cash_book.tel );
	if( strlen(f_cash_book.tel) )
	{
		sprintf( tmpstr," tel='%s' and",f_cash_book.tel );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_cash_book.tw_br_no );
	if( strlen(f_cash_book.tw_br_no) )
	{
		sprintf( tmpstr," tw_br_no='%s' and",f_cash_book.tw_br_no );
		strcat( wherelist,tmpstr );
	}
	else
	{
		/*
		if(strcmp(g_pub_tx.tx_br_no,QS_BR_NO)==0)
		{
		}
		else
			sprintf( tmpstr," tw_br_no='%s' and",g_pub_tx.tx_br_no );
		strcat( wherelist,tmpstr );
		*/
	}

	pub_base_strpack( f_cash_book.cur_no );
	if( strlen(f_cash_book.cur_no) )
	{
		sprintf( tmpstr," cur_no='%s' and",f_cash_book.cur_no );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_cash_book.cur_no );
	if( strlen(f_cash_book.ind) )
	{
		sprintf( tmpstr," ind='%s' and",f_cash_book.ind );
		strcat( wherelist,tmpstr );
	}
	if( f_cash_book.amt )
	{
		sprintf( tmpstr," amt>=%lf and",f_cash_book.amt );
		strcat( wherelist,tmpstr );
	}
	if( f_cash_booka.amt )
	{
		sprintf( tmpstr," amt<=%lf and",f_cash_booka.amt );
		strcat( wherelist,tmpstr );
	}
	if( f_cash_book.ind[0]=='0' )
	{
		sprintf( tmpstr," tx_date=%ld and",g_pub_tx.tx_date );
		strcat( wherelist,tmpstr );
	}

	strcpy( tablelist,"cash_book" );

	/**��ɲ�ѯ**/
	strcat(wherelist," 1=1") ;

	vtcp_log( "WHERE [%s]",wherelist );

	ret=Cash_book_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

		ttlnum=0;

	while(1)
	{
		ret=Cash_book_Fet_Sel( &vcash_book, g_pub_tx.reply );
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
			fprintf( fp,"~��������|��������|@��������|@������Ա|@�������|$���|@��ȡ���|������ˮ��|������ˮ��|\n" );
		}

		if(vcash_book.trace_no<-2147000000) vcash_book.trace_no=0;
		if(vcash_book.tx_date<-2147000000) vcash_book.tx_date=0;
		if(vcash_book.amt<-2147000000) vcash_book.amt=0.0;
		pub_base_dic_show_str(f_ind7,"get_ind",vcash_book.ind);

		fprintf( fp, "%ld|%ld|%s|%s|%s|%.2lf|%s|%d|%d|\n",
			vcash_book.trace_no, vcash_book.tx_date, vcash_book.br_no,
			vcash_book.tel, vcash_book.tw_br_no, vcash_book.amt,
			f_ind7, vcash_book.out_no, vcash_book.wrk_no);
		ttlnum++;
	}

	ret=Cash_book_Clo_Sel( );
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
