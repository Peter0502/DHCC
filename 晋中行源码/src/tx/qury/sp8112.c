/*************************************************
* �� �� ��:  sp8112.c
* ���������� ��ʰ�Ǽǲ���ѯ
*
* ��    ��:  mike
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
#include "mo_coll_c.h"

int sp8112()
{
	struct mo_coll_c vmo_coll;
	struct mo_coll_c f_mo_coll;
	char f_sts7[41];
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

	get_zd_data("0380",f_mo_coll.ac_no );
    pub_base_old_acno(f_mo_coll.ac_no);
	get_zd_data("0260",f_mo_coll.name );
	get_zd_long("0290",&f_mo_coll.pick_date );
	get_zd_data("0910",f_mo_coll.pick_br_no );
	get_zd_data("0640",f_mo_coll.pick_tel );

	/**��ɲ�ѯ����**/
	pub_base_strpack( f_mo_coll.ac_no );
	if( strlen(f_mo_coll.ac_no) )
	{
		sprintf( tmpstr," ac_no='%s' and",f_mo_coll.ac_no );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_mo_coll.name );
	if( strlen(f_mo_coll.name) )
	{
		sprintf( tmpstr," name='%s' and",f_mo_coll.name );
		strcat( wherelist,tmpstr );
	}
	if( f_mo_coll.pick_date )
	{
		sprintf( tmpstr," pick_date=%ld and",f_mo_coll.pick_date );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_mo_coll.pick_br_no );
	if( strlen(f_mo_coll.pick_br_no) )
	{
		sprintf( tmpstr," pick_br_no='%s' and",f_mo_coll.pick_br_no );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_mo_coll.pick_tel );
	if( strlen(f_mo_coll.pick_tel) )
	{
		sprintf( tmpstr," pick_tel='%s' and",f_mo_coll.pick_tel );
		strcat( wherelist,tmpstr );
	}

	strcpy( tablelist,"mo_coll" );

	/**��ɲ�ѯ**/
	strcat(wherelist," 1=1 ORDER BY sts,pick_date,ret_date ") ;

	vtcp_log( "WHERE [%s]",wherelist );
	strcpy( fieldlist,"ac_no,name,pick_date,pick_br_no,pick_tel,ret_date,ret_br_no,sts,wrk_date,trance_no,ret_tel");

	ret=Mo_coll_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

		ttlnum=0;

	while(1)
	{
		ret=Mo_coll_Fet_Sel( &vmo_coll, g_pub_tx.reply );
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
			fprintf( fp,"~@״̬|@��/�˺�|@����|��ʰ����|@��ʰ����|@��ʰ��Ա|�黹����|@�黹����|@�黹��Ա|\n" );
		}

		/*pub_pubdb_prt_fld( fp,tablelist,fieldlist ); */

		if(vmo_coll.pick_date<-2147000000) vmo_coll.pick_date=0;
		if(vmo_coll.ret_date<-2147000000) vmo_coll.ret_date=0;
		pub_base_dic_show_str(f_sts7,"coll_ind",vmo_coll.sts);
		if(vmo_coll.wrk_date<-2147000000) vmo_coll.wrk_date=0;
		if(vmo_coll.trance_no<-2147000000) vmo_coll.trance_no=0;

		fprintf( fp, "%s|%s|%s|%ld|%s|%s|%ld|%s|%s|\n",f_sts7,vmo_coll.ac_no,vmo_coll.name,vmo_coll.pick_date,vmo_coll.pick_br_no,vmo_coll.pick_tel,vmo_coll.ret_date,vmo_coll.ret_br_no,vmo_coll.ret_tel);
		ttlnum++;
	}

	ret=Mo_coll_Clo_Sel( );
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
