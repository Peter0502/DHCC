/*************************************************
* �� �� ��:  sp8804.c
* ���������� ������Ȩ���ײ�ѯ
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
#include "auth_code_c.h"
#include "menu_c.h"

int sp8804()
{
	struct auth_code_c vauth_code;
	struct auth_code_c f_auth_code;
	struct menu_c menu_v;
	char f_auth_ind2[41];
	char f_sts3[41];
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
	char dspcode_v[5];    /*ǰ̨��ʾ����*/

	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset( wherelist,0,sizeof(wherelist) );
	memset( fieldlist,0,sizeof(fieldlist) );
	memset( tablelist,0,sizeof(tablelist) );
	memset( &menu_v,0,sizeof(struct menu_c) );

	pub_base_sysinit();

	get_zd_data("0910",f_auth_code.tel );
	get_zd_data("0640",f_auth_code.chk );
	get_zd_data("0650",f_auth_code.auth );
	get_zd_data("0870",f_auth_code.aud );
	get_zd_data("0920",f_auth_code.tx_code );
	get_zd_data("0670",f_auth_code.auth_ind );
	get_zd_data("0700",f_auth_code.sts );
	get_zd_long("0290",&f_auth_code.auth_no );

	/**��ɲ�ѯ����**/
	pub_base_strpack( f_auth_code.tel );
	if( strlen(f_auth_code.tel) )
	{
		sprintf( tmpstr," tel='%s' and",f_auth_code.tel );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_auth_code.chk );
	if( strlen(f_auth_code.chk) )
	{
		sprintf( tmpstr," chk='%s' and",f_auth_code.chk );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_auth_code.auth );
	if( strlen(f_auth_code.auth) )
	{
		sprintf( tmpstr," auth='%s' and",f_auth_code.auth );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_auth_code.aud );
	if( strlen(f_auth_code.aud) )
	{
		sprintf( tmpstr," aud='%s' and",f_auth_code.aud );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_auth_code.tx_code );
	if( strlen(f_auth_code.tx_code) )
	{
		sprintf( tmpstr," tx_code='%s' and",f_auth_code.tx_code );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_auth_code.auth_ind );
	if( strlen(f_auth_code.auth_ind) )
	{
		sprintf( tmpstr," auth_ind='%s' and",f_auth_code.auth_ind );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_auth_code.sts );
	if( strlen(f_auth_code.sts) )
	{
		sprintf( tmpstr," sts='%s' and",f_auth_code.sts );
		strcat( wherelist,tmpstr );
	}
	sprintf( tmpstr," br_no='%s' and",g_pub_tx.tx_br_no );
	strcat( wherelist,tmpstr );
	if( f_auth_code.auth_no )
	{
		sprintf( tmpstr," auth_no=%ld and",f_auth_code.auth_no );
		strcat( wherelist,tmpstr );
	}

	strcpy( tablelist,"auth_code" );

	/**��ɲ�ѯ**/
	strcat(wherelist," 1=1") ;

	vtcp_log( "WHERE [%s]",wherelist );
	strcpy( fieldlist,"auth_no,tx_code,auth_ind,sts,tel,chk,auth,aud,trace_no,done_trace_no");

	ret=Auth_code_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

		ttlnum=0;

	while(1)
	{
		ret=Auth_code_Fet_Sel( &vauth_code, g_pub_tx.reply );
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
			fprintf( fp,"~��Ȩ��|@���״���(����)|@��Ȩ��־|@ʹ��״̬|@����Ա|@����Ա|@��Ȩ��|@������|��ˮ��|��ɽ��׵���ˮ��|\n" );
		}

		/*pub_pubdb_prt_fld( fp,tablelist,fieldlist ); */

		if(vauth_code.auth_no<-2147000000) vauth_code.auth_no=0;
		pub_base_dic_show_str(f_auth_ind2,"auth_ind",vauth_code.auth_ind);
		pub_base_dic_show_str(f_sts3,"auth_sts",vauth_code.sts);
		if(vauth_code.trace_no<-2147000000) vauth_code.trace_no=0;
		if(vauth_code.done_trace_no<-2147000000) vauth_code.done_trace_no=0;

		ret = Menu_Sel( g_pub_tx.reply, &menu_v, "code='%s'", vauth_code.tx_code );
		if( ret ) memset( &menu_v,0,sizeof(menu_v) );

		pub_base_strpack(menu_v.title);

		fprintf( fp, "%ld|%s(%s)|%s|%s|%s|%s|%s|%s|%ld|%ld|\n",vauth_code.auth_no,menu_v.code,menu_v.title,f_auth_ind2,f_sts3,vauth_code.tel,vauth_code.chk,vauth_code.auth,vauth_code.aud,vauth_code.trace_no,vauth_code.done_trace_no);
		ttlnum++;
	}

	ret=Auth_code_Clo_Sel( );
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
