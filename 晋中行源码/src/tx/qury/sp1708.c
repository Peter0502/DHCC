/*************************************************
* �� �� ��:  sp1708.c
* ���������� �ͻ�������Ϣ��ѯ
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
#include "cif_lifsty_inf_c.h"

int sp1708()
{
	struct cif_lifsty_inf_c vcif_lifsty_inf;
	struct cif_lifsty_inf_c f_cif_lifsty_inf;
	char f_life_type2[41];
	char f_life_detail3[41];
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

	get_zd_long("0540",&f_cif_lifsty_inf.cif_no );
	get_zd_long("0510",&f_cif_lifsty_inf.lifsty_no );
	get_zd_data("0210",f_cif_lifsty_inf.life_type );

	/**��ɲ�ѯ����**/
	if( f_cif_lifsty_inf.cif_no )
	{
		sprintf( tmpstr," cif_no=%ld and",f_cif_lifsty_inf.cif_no );
		strcat( wherelist,tmpstr );
	}
	if( f_cif_lifsty_inf.lifsty_no )
	{
		sprintf( tmpstr," lifsty_no=%d and",f_cif_lifsty_inf.lifsty_no );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_cif_lifsty_inf.life_type );
	if( strlen(f_cif_lifsty_inf.life_type) )
	{
		sprintf( tmpstr," life_type='%s' and",f_cif_lifsty_inf.life_type );
		strcat( wherelist,tmpstr );
	}

	strcpy( tablelist,"cif_lifsty_inf" );

	/**��ɲ�ѯ**/
	strcat(wherelist," 1=1") ;

	vtcp_log( "WHERE [%s]",wherelist );
	strcpy( fieldlist,"cif_no,lifsty_no,life_type,life_detail");

	ret=Cif_lifsty_inf_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

		ttlnum=0;

	while(1)
	{
		ret=Cif_lifsty_inf_Fet_Sel( &vcif_lifsty_inf, g_pub_tx.reply );
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
			fprintf( fp,"~�ͻ�����|���|@���ʽ����|@���ʽ��ϸ|\n" );
		}

		/*pub_pubdb_prt_fld( fp,tablelist,fieldlist ); */

		if(vcif_lifsty_inf.cif_no<-2147000000) vcif_lifsty_inf.cif_no=0;
		pub_base_dic_show_str(f_life_type2,"life_type",vcif_lifsty_inf.life_type);
		pub_base_dic_show_str(f_life_detail3,"life_detail",vcif_lifsty_inf.life_detail);

		fprintf( fp, "%ld|%d|%s|%s|\n",vcif_lifsty_inf.cif_no,vcif_lifsty_inf.lifsty_no,f_life_type2,f_life_detail3);
		ttlnum++;
	}

	ret=Cif_lifsty_inf_Clo_Sel( );
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
