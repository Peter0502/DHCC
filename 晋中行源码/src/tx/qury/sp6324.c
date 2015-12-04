/*************************************************
* 文 件 名:  sp6324.c
* 功能描述： 保函信息查询
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
#include "protect_note_c.h"

int sp6324()
{
	struct protect_note_c vprotect_note;
	struct protect_note_c f_protect_note;
	struct protect_note_c f_protect_notea;
	char f_protect_typ21[41];
	char f_protect_sts22[41];
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

	pub_base_sysinit();

	get_zd_data("0380",f_protect_note.pro_ac_no );
	get_zd_data("0630",f_protect_note.ben_ac_no );
	get_zd_data("0370",f_protect_note.bail_ac_no );
	get_zd_data("0620",f_protect_note.pact_no );
	get_zd_long("0290",&f_protect_note.tx_date );
	get_zd_long("0440",&f_protect_notea.tx_date );

	/**组成查询条件**/
	pub_base_strpack( f_protect_note.pro_ac_no );
	if( strlen(f_protect_note.pro_ac_no) )
	{
		sprintf( tmpstr," pro_ac_no='%s' and",f_protect_note.pro_ac_no );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_protect_note.ben_ac_no );
	if( strlen(f_protect_note.ben_ac_no) )
	{
		sprintf( tmpstr," ben_ac_no='%s' and",f_protect_note.ben_ac_no );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_protect_note.bail_ac_no );
	if( strlen(f_protect_note.bail_ac_no) )
	{
		sprintf( tmpstr," bail_ac_no='%s' and",f_protect_note.bail_ac_no );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_protect_note.pact_no );
	if( strlen(f_protect_note.pact_no) )
	{
		sprintf( tmpstr," pact_no='%s' and",f_protect_note.pact_no );
		strcat( wherelist,tmpstr );
	}
	if( f_protect_note.tx_date )
	{
		sprintf( tmpstr," tx_date>=%ld and",f_protect_note.tx_date );
		strcat( wherelist,tmpstr );
	}
	if( f_protect_notea.tx_date )
	{
		sprintf( tmpstr," tx_date<=%ld and",f_protect_notea.tx_date );
		strcat( wherelist,tmpstr );
	}

	strcpy( tablelist,"protect_note" );

	/**组成查询**/
	strcat(wherelist," 1=1") ;

	vtcp_log( "WHERE [%s]",wherelist );
	strcpy( fieldlist,"tx_br_no,pro_ac_no,pro_cif_no,ben_ac_no,ben_cif_no,bail_ac_no,pact_no,opn_date,time_limt,end_date,sum_amt,amt,bail_amt,ratio,mort_typ,mort_ac_no,pro_name,pro_addr,ben_name,ben_addr,amends_amt,protect_typ,protect_sts");

	ret=Protect_note_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

		ttlnum=0;

	while(1)
	{
		ret=Protect_note_Fet_Sel( &vprotect_note, g_pub_tx.reply );
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
			fprintf( fp,"~@发生机构|@申请人帐号|申请人客户号|@受益人帐号|受益人客户号|@保证金帐号|@合同号或标号|开具保函日期|保函有效期限|保函到期日期|$合同总金额|$保函金额|$保证金金额|担保费比例(%)|@质押物类别|@质押物号码|@申请人户名|@申请人地址|@受益人户名|@受益人地址|$赔偿金额|@保函类型|@状态|\n" );
		}

		/*pub_pubdb_prt_fld( fp,tablelist,fieldlist ); */

		if(vprotect_note.pro_cif_no<-2147000000) vprotect_note.pro_cif_no=0;
		if(vprotect_note.ben_cif_no<-2147000000) vprotect_note.ben_cif_no=0;
		if(vprotect_note.opn_date<-2147000000) vprotect_note.opn_date=0;
		if(vprotect_note.end_date<-2147000000) vprotect_note.end_date=0;
		if(vprotect_note.sum_amt<-2147000000) vprotect_note.sum_amt=0.0;
		if(vprotect_note.amt<-2147000000) vprotect_note.amt=0.0;
		if(vprotect_note.bail_amt<-2147000000) vprotect_note.bail_amt=0.0;
		if(vprotect_note.ratio<-2147000000) vprotect_note.ratio=0.0;
		if(vprotect_note.amends_amt<-2147000000) vprotect_note.amends_amt=0.0;
		pub_base_dic_show_str(f_protect_typ21,"protect_typ",vprotect_note.protect_typ);
		pub_base_dic_show_str(f_protect_sts22,"protect_sts",vprotect_note.protect_sts);

		fprintf( fp, "%s|%s|%ld|%s|%ld|%s|%s|%ld|%d|%ld|%.2lf|%.2lf|%.2lf|%.5lf|%s|%s|%s|%s|%s|%s|%.2lf|%s|%s|\n",vprotect_note.tx_br_no,vprotect_note.pro_ac_no,vprotect_note.pro_cif_no,vprotect_note.ben_ac_no,vprotect_note.ben_cif_no,vprotect_note.bail_ac_no,vprotect_note.pact_no,vprotect_note.opn_date,vprotect_note.time_limt,vprotect_note.end_date,vprotect_note.sum_amt,vprotect_note.amt,vprotect_note.bail_amt,vprotect_note.ratio,vprotect_note.mort_typ,vprotect_note.mort_ac_no,vprotect_note.pro_name,vprotect_note.pro_addr,vprotect_note.ben_name,vprotect_note.ben_addr,vprotect_note.amends_amt,f_protect_typ21,f_protect_sts22);
		ttlnum++;
	}

	ret=Protect_note_Clo_Sel( );
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
