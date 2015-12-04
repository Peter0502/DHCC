/*************************************************
* �� �� ��:  sp8902.c
* ���������� ��Ա���ӵǼǲ���ѯ
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
#include "com_tel_connect_c.h"

int sp8902()
{
	struct com_tel_connect_c vcom_tel_connect;
	struct com_tel_connect_c f_com_tel_connect;
	struct com_tel_connect_c f_com_tel_connecta;
		char filename[100];
		char f_ind14[41]; 
		char wherelist[1024];  /**��ѯ����**/
		char tmpstr[512];
		char tmpmo[512];
		char fieldlist[1024];  /**Ҫ��ѯ���ֶ��б�**/
		char tablelist[128];   /**Ҫ��ѯ�ı���**/
		char titstr[1024];
		char f_cur_name[21];
		int ttlnum=0;			/**��ȡ��������**/
		int i=0;
		int ret=0;
		FILE *fp;

	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset( wherelist,0,sizeof(wherelist) );
	memset( fieldlist,0,sizeof(fieldlist) );
	memset( tablelist,0,sizeof(tablelist) );

	pub_base_sysinit();

	get_zd_data("0910",f_com_tel_connect.tel );
	get_zd_data("0640",f_com_tel_connect.tw_tel );
	get_zd_data("0650",f_com_tel_connect.br_no );
	get_zd_long("0290",&f_com_tel_connect.tx_date );
	get_zd_long("0440",&f_com_tel_connecta.tx_date );
	get_zd_data("0210",f_com_tel_connect.cur_no );

	/**��ɲ�ѯ����**/
	pub_base_strpack( f_com_tel_connect.tel );
	if( strlen(f_com_tel_connect.tel) )
	{
		sprintf( tmpstr," tel='%s' and",f_com_tel_connect.tel );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_com_tel_connect.tw_tel );
	if( strlen(f_com_tel_connect.tw_tel) )
	{
		sprintf( tmpstr," tw_tel='%s' and",f_com_tel_connect.tw_tel );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_com_tel_connect.br_no );
	if( strlen(f_com_tel_connect.br_no) )
	{
		sprintf( tmpstr," br_no='%s' and",f_com_tel_connect.br_no );
		strcat( wherelist,tmpstr );
	}
	if( f_com_tel_connect.tx_date )
	{
		sprintf( tmpstr," tx_date>=%ld and",f_com_tel_connect.tx_date );
		strcat( wherelist,tmpstr );
	}
	if( f_com_tel_connecta.tx_date )
	{
		sprintf( tmpstr," tx_date<=%ld and",f_com_tel_connecta.tx_date );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_com_tel_connect.cur_no );
	if( strlen(f_com_tel_connect.cur_no) )
	{
		sprintf( tmpstr," cur_no='%s' and",f_com_tel_connect.cur_no );
		strcat( wherelist,tmpstr );
	}

	strcpy( tablelist,"com_tel_connect" );

	/**��ɲ�ѯ**/
	strcat(wherelist," 1=1 ORDER BY tx_date,br_no,tel,tw_tel,cur_no ") ;

	vtcp_log( "WHERE [%s]",wherelist );
	strcpy( fieldlist,"tel,tw_tel,amt,br_no,tx_date,cur_no,ind");

	ret=Com_tel_connect_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

		ttlnum=0;

	while(1)
	{
		ret=Com_tel_connect_Fet_Sel( &vcom_tel_connect, g_pub_tx.reply );
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
			fprintf( fp,"~@��־|@��Ա����|@�Է���Ա|$���ӽ��|@����|��������|@�����|\n" );
		}

		/*pub_pubdb_prt_fld( fp,tablelist,fieldlist ); */

		if(vcom_tel_connect.amt<-2147000000) vcom_tel_connect.amt=0.0;
		if(vcom_tel_connect.tx_date<-2147000000) vcom_tel_connect.tx_date=0;
		pub_base_dic_show_str(f_ind14,"get_ind",vcom_tel_connect.ind);
		pub_base_dic_show_str(f_cur_name,"cur_name",vcom_tel_connect.cur_no);
		
		fprintf( fp, "%s|%s|%s|%.2lf|%s|%ld|%s|\n",f_ind14,vcom_tel_connect.tel,vcom_tel_connect.tw_tel,vcom_tel_connect.amt,f_cur_name,vcom_tel_connect.tx_date,vcom_tel_connect.br_no);
		ttlnum++;
	}

	ret=Com_tel_connect_Clo_Sel( );
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
