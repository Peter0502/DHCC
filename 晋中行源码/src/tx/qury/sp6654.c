/*************************************************
* �� �� ��:  sp6654.c
* ���������� �����кŲ�ѯ
*
* ��    ��:  
* ������ڣ� 20060831
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
#include <string.h>
#include "public.h"
#include "hv_uniontab_c.h"

int sp6654()
{
  	struct hv_uniontab_c s_hv_uniontab;
	struct hv_uniontab_c d_hv_uniontab;
	char br_sts[41];
	char br_categ[41];
	char obr_attr[41];
	char filename[100];
  	char wherelist[1024];
  	char tmpstr[512];
	char fieldlist[1024];  /**Ҫ��ѯ���ֶ��б�**/
	char tablelist[128];   /**Ҫ��ѯ�ı���**/
	int totcnt=0;			     /**��ȡ��������**/
	int ret=0;
	FILE *fp;
	
	/**��ʼ��ȫ�ֱ���**/
	memset(&s_hv_uniontab,0x00,sizeof(struct hv_uniontab_c));
	memset(&d_hv_uniontab,0x00,sizeof(struct hv_uniontab_c));
	memset(br_sts, 0 ,sizeof(br_sts));
	memset(obr_attr, 0 ,sizeof(obr_attr));
	memset(br_categ, 0 ,sizeof(br_categ));
	memset(filename, 0 ,sizeof(filename));
	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset( wherelist,0,sizeof(wherelist) );
	memset( fieldlist,0,sizeof(fieldlist) );
	memset( tablelist,0,sizeof(tablelist) );
	pub_base_sysinit();
	
  	/**ȡǰ̨�Ĵ���ֵ***/
  	get_zd_data("0670",s_hv_uniontab.obr_no_attr);
	get_zd_data("0260",s_hv_uniontab.br_name_f);
	get_zd_data("0580",s_hv_uniontab.or_br_no );
	
	/**��ɲ�ѯ����**/
	pub_base_strpack(s_hv_uniontab.obr_no_attr );
  vtcp_log( "s_hv_uniontab.OBR_NO_ATTR=[%s][%s][%d]",s_hv_uniontab.obr_no_attr,__FILE__,__LINE__);
  if(strlen(s_hv_uniontab.obr_no_attr) && strlen(s_hv_uniontab.br_name_f)==0 && strlen(s_hv_uniontab.or_br_no)==0 )
	{
		sprintf( tmpstr," OBR_NO_ATTR=%s and",s_hv_uniontab.obr_no_attr );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack(s_hv_uniontab.br_name_f );
	vtcp_log( "s_hv_uniontab.BR_NAME_F=[%s][%s][%d]",s_hv_uniontab.br_name_f,__FILE__,__LINE__);

	if(strlen(s_hv_uniontab.br_name_f))
	{
		vtcp_log("br_name_f[%s]",s_hv_uniontab.br_name_f);
		sprintf( tmpstr," br_name_f like'%%%%%s%%%%' and",s_hv_uniontab.br_name_f );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack(s_hv_uniontab.or_br_no );
  	vtcp_log( "s_hv_uniontab.or_br_no=[%s][%s][%d]",s_hv_uniontab.or_br_no,__FILE__,__LINE__);
  	if(strlen(s_hv_uniontab.or_br_no))
	{
		sprintf( tmpstr," OR_BR_NO=%s and",s_hv_uniontab.or_br_no );
		strcat( wherelist,tmpstr );
	}
	strcat(wherelist," 1=1 ORDER BY BR_STS,OR_BR_NO ") ;
	vtcp_log( "WHERE [%s]",wherelist );
  strcpy( tablelist,"hv_uniontab" );
  ret=Hv_uniontab_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL
	/****��ѯȡֵ****/
	while(1)
	{
	  ret=Hv_uniontab_Fet_Sel(&d_hv_uniontab, g_pub_tx.reply );
	  if( ret==100 ) 
	    break;
	  ERR_DEAL
	  if( !totcnt )
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
			fprintf( fp,"~��        ��|@�� �� �� �� |@�� �� ȫ ��|\n" );
		}	
		fprintf( fp, "%s|%s|%s|\n",d_hv_uniontab.or_br_no,d_hv_uniontab.qs_no,d_hv_uniontab.br_name_f);
		totcnt++;
	}
	if( !totcnt )
	{
		strcpy( g_pub_tx.reply,"S049" );
		goto ErrExit;
	}
	else
	{
		fclose(fp);

		set_zd_data( DC_FILE_SND,"1" );
	}
	 /****����ѯ��¼��һ��ʱ�����±��������ļ�****/
	 
	if(totcnt==1)
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

	  pub_base_dic_show_str(br_sts,"bankstat",d_hv_uniontab.br_sts);
	  pub_base_dic_show_str(obr_attr,"obr_attr",d_hv_uniontab.obr_no_attr);
	  pub_base_dic_show_str(br_categ,"br_categ",d_hv_uniontab.br_categ);
	  
	  fprintf(fp,"CKPT%s|%s|%s|%s|%s|%s|%s|",d_hv_uniontab.or_br_no, d_hv_uniontab.qs_no, d_hv_uniontab.br_name_f,
	          d_hv_uniontab.city_code,br_categ,obr_attr,br_sts);
	}
	
	ret=Hv_uniontab_Clo_Sel( );
	ERR_DEAL
	
	if( !totcnt )
	{
		strcpy( g_pub_tx.reply,"S049" );
		goto ErrExit;
	}
	else
	{
		fclose(fp);

		set_zd_data( DC_FILE_SND,"1" );
	}
  /***�������***/
GoodExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before bad return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY ,g_pub_tx.reply);
  return 1;
}
