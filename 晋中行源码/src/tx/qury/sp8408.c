/*************************************************
* �� �� ��:  sp8408.c
* ���������� ��Ա��Ȩ��Ϣ��ѯ
*
*************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define EXTERN
#include "public.h"
#include "com_tel_c.h"
#include "auth_tel_auth_c.h"
#include "auth_auth_lmt_c.h"

int sp8408()
{
	struct com_tel_c vcom_tel;
	struct com_tel_c f_com_tel;
	struct auth_tel_auth_c auth_tel_auth;
	struct auth_auth_lmt_c auth_lmt;

	char f_lvl2[41];
	char f_csts3[41];
	char br_no[6];
	char br_name[51];
	char br_tele[31];
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
        
	get_zd_data("0910",br_no );
	get_zd_data("0920",f_com_tel.tel );

	/**��ɲ�ѯ����**/
	pub_base_strpack( br_no );
	pub_base_strpack( f_com_tel.tel );
	
	
	if( strlen(f_com_tel.tel) )
	{
		sprintf( tmpstr," tel='%s' and ",f_com_tel.tel );
		strcat( wherelist,tmpstr );
	}
	
	/* �������� */
	strcat(wherelist," 1=1 ORDER BY tel ") ;
		 
	vtcp_log( "WHERE [%s]",wherelist );	
	        
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
	fprintf(fp,"~@����|@��Ա|@����|@����|@״̬|$Ȩ��|$�ֽ�Ȩ��|$�ڲ�ת��|$ͬ��Ȩ��|$���Ȩ��|\n" );
        
    ret=Auth_tel_auth_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL
        
	ttlnum=0;

	while(1)
	{
		ret=Auth_tel_auth_Fet_Sel( &auth_tel_auth, g_pub_tx.reply );
		if( ret==100 ) 
		{
			if( !ttlnum )
			{
			 strcpy( g_pub_tx.reply,"S055" );
		         goto ErrExit;
		        }
			break;
		}
		 ERR_DEAL

		ret=Com_tel_Sel(g_pub_tx.reply,&vcom_tel,"tel='%s'",auth_tel_auth.tel);
		if( ret==100 ) continue;
		ERR_DEAL

		if( strlen(br_no) && strcmp(vcom_tel.br_no,br_no) )
			continue;
		pub_base_dic_show_str(f_lvl2,"tel_lvl",vcom_tel.lvl);
		pub_base_dic_show_str(f_csts3,"csts",vcom_tel.csts);

	fprintf(fp,"~@����|@��Ա|@����|@����|@״̬|$Ȩ��|$�ֽ�Ȩ��|$�ڲ�ת��|$ͬ��Ȩ��|$���Ȩ��|\n" );
		fprintf( fp, "%s|%s|%s|%s|%s|", 
			vcom_tel.br_no,auth_tel_auth.tel,vcom_tel.name,
			f_lvl2,f_csts3 );

		ret = Auth_auth_lmt_Sel(g_pub_tx.reply,&auth_lmt,"auth_code='%s'", \
								auth_tel_auth.au_al);
		if( ret==100 )  auth_lmt.max_amt=0.00;
		else if( ret ) { ERR_DEAL }
		fprintf( fp,"%.2lf|",auth_lmt.max_amt );

		ret = Auth_auth_lmt_Sel(g_pub_tx.reply,&auth_lmt,"auth_code='%s'", \
								auth_tel_auth.au_xj);
		if( ret==100 )  auth_lmt.max_amt=0.00;
		else if( ret ) { ERR_DEAL }
		fprintf( fp,"%.2lf|",auth_lmt.max_amt );

		ret = Auth_auth_lmt_Sel(g_pub_tx.reply,&auth_lmt,"auth_code='%s'", \
								auth_tel_auth.au_zz);
		if( ret==100 )  auth_lmt.max_amt=0.00;
		else if( ret ) { ERR_DEAL }
		fprintf( fp,"%.2lf|",auth_lmt.max_amt );

		ret = Auth_auth_lmt_Sel(g_pub_tx.reply,&auth_lmt,"auth_code='%s'", \
								auth_tel_auth.au_tc);
		if( ret==100 )  auth_lmt.max_amt=0.00;
		else if( ret ) { ERR_DEAL }
		fprintf( fp,"%.2lf|",auth_lmt.max_amt );

		ret = Auth_auth_lmt_Sel(g_pub_tx.reply,&auth_lmt,"auth_code='%s'", \
								auth_tel_auth.au_de);
		if( ret==100 )  auth_lmt.max_amt=0.00;
		else if( ret ) { ERR_DEAL }
		fprintf( fp,"%.2lf|",auth_lmt.max_amt );

		fprintf( fp,"\n");
		ttlnum++;
	}

	Auth_tel_auth_Clo_Sel( );

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
