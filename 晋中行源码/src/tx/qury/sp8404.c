/*************************************************
* �� �� ��:  sp8404.c
* ���������� ��Ա������Ϣ��ѯ
*
* ��    ��:  jack
* ������ڣ� 2005-07-03
*
* �޸ļ�¼��
* ��   ��:
* �� �� ��:

*************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define EXTERN
#include "public.h"
#include "com_tel_c.h"
#include "auth_auth_auth_c.h"
#include "auth_auth_lmt_c.h"

int sp8404()
{
	struct com_tel_c vcom_tel;
	struct com_tel_c f_com_tel;
	struct auth_auth_auth_c auth_tel;
	struct auth_auth_lmt_c auth_lmt;

	char f_lvl2[20];
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
	
	if( strlen(br_no) )
	{
		sprintf( tmpstr," br_no='%s' and ",br_no );
		strcat( wherelist,tmpstr );
	}
	
	if( strlen(f_com_tel.tel) )
	{
		sprintf( tmpstr," tel='%s' and ",f_com_tel.tel );
		strcat( wherelist,tmpstr );
	}
	
	/* �������� */
	strcpy( tablelist,"com_tel" );
	
	/**��ɲ�ѯ**/
	strcat(wherelist," 1=1 and substr(tel,3,2)!='99' ORDER BY br_no , csts , tel ") ;
		 
	vtcp_log( "WHERE [%s]",wherelist );	
	        
	strcpy( fieldlist,"tel,name,lvl,csts,br_no,tx_type");

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
	fprintf( fp,"~@��Ա��|@��Ա����|@��Ա����|@״̬|@��������|@����绰|@��������к�|\n" );
        
    ret=Com_tel_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL
        
	ttlnum=0;

	while(1)
	{
		ret=Com_tel_Fet_Sel( &vcom_tel, g_pub_tx.reply );
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

		memset( &auth_tel,0x00,sizeof(struct auth_auth_auth_c));
		memset( &auth_lmt,0x00,sizeof(struct auth_auth_lmt_c));

		ret = pub_base_getbrname( vcom_tel.br_no , br_name );
		if( ret ){ strcpy( br_name,"δ�ҵ�" ); }

		ret = pub_base_getbrtele( vcom_tel.br_no , br_tele );
		if( ret ){ strcpy( br_tele,"δ�ҵ�" );}

		char tmp_type[20];
		memset(tmp_type,0x00,sizeof(tmp_type));
		memcpy(tmp_type,vcom_tel.tx_type,8);
		pub_base_dic_show_str(f_lvl2,"tel_type",tmp_type);
		pub_base_dic_show_str(f_csts3,"csts",vcom_tel.csts);

		ret = Auth_auth_auth_Sel(g_pub_tx.reply,&auth_tel,"tel='%s'", \
								 vcom_tel.tel);
		char debkno[30];
		memset(debkno,0x00,sizeof(debkno));
                ret = GetOr_br_noByBr_no(vcom_tel.br_no,debkno);
		if( ret ){ strcpy( debkno,"δ�ҵ�" ); }
/*		�µ�Ȩ��������ò���
		ret = Auth_auth_lmt_Sel(g_pub_tx.reply,&auth_lmt,"auth_code='%s'", \
								auth_tel.auth_code);
		if( ret==100 )  strcpy(auth_lmt.mo,"��ͨ��Ա"); 
*/

		fprintf( fp, "%s|%s|%s|%s|%s|%s|%s|\n", vcom_tel.tel,vcom_tel.name,
				 f_lvl2,f_csts3,br_name,br_tele,debkno);
		ttlnum++;
	}

	Com_tel_Clo_Sel( );

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
