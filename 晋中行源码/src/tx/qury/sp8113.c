/*************************************************
* 文 件 名:  sp8113.c
* 功能描述： 换证登记簿查询
*
* 作    者:  
* 完成日期： 
*
* 修改记录：
* 日   期:
* 修 改 人:
* 修改内容:
*************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define EXTERN
#include "public.h"
#include "mo_rpl_note_c.h"
#include "mdm_attr_c.h"

int sp8113()
{
	struct mo_rpl_note_c vmo_rpl_note;
	struct mo_rpl_note_c f_mo_rpl_note;
	struct mo_rpl_note_c f_mo_rpl_notea;
	struct mdm_attr_c	mdm_attr_tmp;
	
	char f_rmk9[41];
		char filename[100];
		char wherelist[1024];  /**查询条件**/
		char tmpstr[512];
		char tmpmo[512];
		char fieldlist[1024];  /**要查询的字段列表**/
		char tablelist[128];   /**要查询的表名**/
		char titstr[1024];
		int ttlnum=0;			/**读取的总条数**/
		int i=0;
		int ret=0;
		FILE *fp;

	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset( wherelist,0,sizeof(wherelist) );
	memset( fieldlist,0,sizeof(fieldlist) );
	memset( tablelist,0,sizeof(tablelist) );
	memset( &mdm_attr_tmp,0,sizeof(struct mdm_attr_c));
	
	pub_base_sysinit();

	get_zd_data("0380",f_mo_rpl_note.old_ac_no );
	get_zd_data("0910",f_mo_rpl_note.br_no );
	get_zd_data("0640",f_mo_rpl_note.mdm_code );
	get_zd_data("0590",f_mo_rpl_note.old_note_no );
	get_zd_data("0370",f_mo_rpl_note.new_ac_no );
	get_zd_data("0600",f_mo_rpl_note.new_note_no );
	get_zd_long("0290",&f_mo_rpl_note.rpl_date );
	get_zd_long("0440",&f_mo_rpl_notea.rpl_date );
	get_zd_data("0700",f_mo_rpl_note.rmk );

	/**组成查询条件**/
	pub_base_strpack( f_mo_rpl_note.old_ac_no );
	if( strlen(f_mo_rpl_note.old_ac_no) )
	{
		sprintf( tmpstr," old_ac_no='%s' and",f_mo_rpl_note.old_ac_no );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_mo_rpl_note.br_no );
	if( strlen(f_mo_rpl_note.br_no) )
	{
		sprintf( tmpstr," br_no='%s' and",f_mo_rpl_note.br_no );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_mo_rpl_note.mdm_code );
	if( strlen(f_mo_rpl_note.mdm_code) )
	{
		sprintf( tmpstr," mdm_code='%s' and",f_mo_rpl_note.mdm_code );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_mo_rpl_note.old_note_no );
	if( strlen(f_mo_rpl_note.old_note_no) )
	{
		sprintf( tmpstr," old_note_no='%s' and",f_mo_rpl_note.old_note_no );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_mo_rpl_note.new_ac_no );
	if( strlen(f_mo_rpl_note.new_ac_no) )
	{
		sprintf( tmpstr," new_ac_no='%s' and",f_mo_rpl_note.new_ac_no );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_mo_rpl_note.new_note_no );
	if( strlen(f_mo_rpl_note.new_note_no) )
	{
		sprintf( tmpstr," new_note_no='%s' and",f_mo_rpl_note.new_note_no );
		strcat( wherelist,tmpstr );
	}
	if( f_mo_rpl_note.rpl_date )
	{
		sprintf( tmpstr," rpl_date>=%ld and",f_mo_rpl_note.rpl_date );
		strcat( wherelist,tmpstr );
	}
	if( f_mo_rpl_notea.rpl_date )
	{
		sprintf( tmpstr," rpl_date<=%ld and",f_mo_rpl_notea.rpl_date );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_mo_rpl_note.rmk );
	if( strlen(f_mo_rpl_note.rmk) )
	{
		sprintf( tmpstr," rmk='%s' and",f_mo_rpl_note.rmk );
		strcat( wherelist,tmpstr );
	}

	strcpy( tablelist,"mo_rpl_note" );

	/**组成查询**/
	strcat(wherelist," 1=1 ORDER BY rpl_date,mdm_code,rmk,old_ac_no ") ;

	vtcp_log( "WHERE [%s]",wherelist );
	strcpy( fieldlist,"old_ac_no,br_no,mdm_code,old_note_no,new_ac_no,new_note_no,rpl_date,tel,trace_no,rmk");

	ret=Mo_rpl_note_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

		ttlnum=0;

	while(1)
	{
		ret=Mo_rpl_note_Fet_Sel( &vmo_rpl_note, g_pub_tx.reply );
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

			/**显示列标题**/
			fprintf( fp,"~@介质名称|@卡/账号|@凭证号|@新帐号/卡号|@新凭证号码|@机构|换证日期|@柜员|流水号|@换证原因|\n" );
		}

		/*pub_pubdb_prt_fld( fp,tablelist,fieldlist ); */

		if(vmo_rpl_note.rpl_date<-2147000000) vmo_rpl_note.rpl_date=0;
		if(vmo_rpl_note.trace_no<-2147000000) vmo_rpl_note.trace_no=0;
		pub_base_dic_show_str(f_rmk9,"rmk",vmo_rpl_note.rmk);
		/* 在mdm_attr表中查询介质名称 */
		g_reply_int = Mdm_attr_Sel(g_pub_tx.reply,&mdm_attr_tmp,"mdm_code='%s'",vmo_rpl_note.mdm_code);
		if (g_reply_int)	goto ErrExit;
		fprintf( fp, "%s|%s|%s|%s|%s|%s|%ld|%s|%ld|%s|\n",mdm_attr_tmp.mdm_name,vmo_rpl_note.old_ac_no,vmo_rpl_note.old_note_no,vmo_rpl_note.new_ac_no,vmo_rpl_note.new_note_no,vmo_rpl_note.br_no,vmo_rpl_note.rpl_date,vmo_rpl_note.tel,vmo_rpl_note.trace_no,f_rmk9);
		ttlnum++;
	}

	ret=Mo_rpl_note_Clo_Sel( );
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
