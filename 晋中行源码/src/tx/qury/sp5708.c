/*************************************************
* 文 件 名:  sp5708.c
* 功能描述： 授权号信息查询
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
#include "auth_inf_c.h"

int sp5708()
{
	struct auth_inf_c vauth_inf;
	struct auth_inf_c f_auth_inf;
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
	char csts[11];

	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset( wherelist,0,sizeof(wherelist) );
	memset( fieldlist,0,sizeof(fieldlist) );
	memset( tablelist,0,sizeof(tablelist) );

	pub_base_sysinit();

	get_zd_long("0290",&f_auth_inf.auth_no );
	get_zd_data("0910",f_auth_inf.br_no );
	get_zd_data("0650",f_auth_inf.tel );
	get_zd_long("0440",&f_auth_inf.mtr_date );
	get_zd_data("0640",f_auth_inf.tx_code );
	get_zd_double("0400",&f_auth_inf.amt );
	get_zd_data("0380",f_auth_inf.ac_no );
	get_zd_data("0870",f_auth_inf.auth_tel );
	get_zd_data("0880",f_auth_inf.auth_br_no );
	get_zd_long("0450",&f_auth_inf.auth_date );
	get_zd_data("0670",f_auth_inf.sts );
	get_zd_long("0460",&f_auth_inf.use_date );
	get_zd_long("0280",&f_auth_inf.use_trace_no );
	get_zd_double("0410",&f_auth_inf.use_amt );
	get_zd_data("0730",f_auth_inf.use_tel );

	/**组成查询条件**/
	if( f_auth_inf.auth_no )
	{
		sprintf( tmpstr," auth_no=%ld and",f_auth_inf.auth_no );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_auth_inf.br_no );
	if( strlen(f_auth_inf.br_no) )
	{
		sprintf( tmpstr," br_no='%s' and",f_auth_inf.br_no );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_auth_inf.tel );
	if( strlen(f_auth_inf.tel) )
	{
		sprintf( tmpstr," tel='%s' and",f_auth_inf.tel );
		strcat( wherelist,tmpstr );
	}
	if( f_auth_inf.mtr_date )
	{
		sprintf( tmpstr," mtr_date=%ld and",f_auth_inf.mtr_date );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_auth_inf.tx_code );
	if( strlen(f_auth_inf.tx_code) )
	{
		sprintf( tmpstr," tx_code='%s' and",f_auth_inf.tx_code );
		strcat( wherelist,tmpstr );
	}
	if( f_auth_inf.amt )
	{
		sprintf( tmpstr," amt=%lf and",f_auth_inf.amt );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_auth_inf.ac_no );
	if( strlen(f_auth_inf.ac_no) )
	{
		sprintf( tmpstr," ac_no='%s' and",f_auth_inf.ac_no );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_auth_inf.auth_tel );
	if( strlen(f_auth_inf.auth_tel) )
	{
		sprintf( tmpstr," auth_tel='%s' and",f_auth_inf.auth_tel );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_auth_inf.auth_br_no );
	if( strlen(f_auth_inf.auth_br_no) )
	{
		sprintf( tmpstr," auth_br_no='%s' and",f_auth_inf.auth_br_no );
		strcat( wherelist,tmpstr );
	}
	if( f_auth_inf.auth_date )
	{
		sprintf( tmpstr," auth_date=%ld and",f_auth_inf.auth_date );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_auth_inf.sts );
	if( strlen(f_auth_inf.sts) )
	{
		sprintf( tmpstr," sts='%s' and",f_auth_inf.sts );
		strcat( wherelist,tmpstr );
	}
	if( f_auth_inf.use_date )
	{
		sprintf( tmpstr," use_date=%ld and",f_auth_inf.use_date );
		strcat( wherelist,tmpstr );
	}
	if( f_auth_inf.use_trace_no )
	{
		sprintf( tmpstr," use_trace_no=%ld and",f_auth_inf.use_trace_no );
		strcat( wherelist,tmpstr );
	}
	if( f_auth_inf.use_amt )
	{
		sprintf( tmpstr," use_amt=%lf and",f_auth_inf.use_amt );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_auth_inf.use_tel );
	if( strlen(f_auth_inf.use_tel) )
	{
		sprintf( tmpstr," use_tel='%s' and",f_auth_inf.use_tel );
		strcat( wherelist,tmpstr );
	}

	strcpy( tablelist,"auth_inf" );

	/**组成查询**/
	strcat(wherelist," 1=1") ;

	vtcp_log( "WHERE [%s]",wherelist );
	strcpy( fieldlist,"auth_no,br_no,tel,mtr_date,tx_code,amt,ac_no,auth_tel,auth_br_no,auth_date,sts,use_date,use_trace_no,use_amt,use_tel");

	ret=Auth_inf_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

		ttlnum=0;

	while(1)
	{
		ret=Auth_inf_Fet_Sel( &vauth_inf, g_pub_tx.reply );
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
			fprintf( fp,"~授权码|@被授权机构|@被授权人|有效日期|@交易码|$金额|@帐号|@授权人|@授权所在机构|授权日期|@状态|完成日期|完成流水号|$完成金额|@完成柜员|\n" );
		}

		/*pub_pubdb_prt_fld( fp,tablelist,fieldlist ); */

		if(vauth_inf.auth_no<-2147000000) vauth_inf.auth_no=0;
		if(vauth_inf.mtr_date<-2147000000) vauth_inf.mtr_date=0;
		if(vauth_inf.amt<-2147000000) vauth_inf.amt=0.0;
		if(vauth_inf.auth_date<-2147000000) vauth_inf.auth_date=0;
		if(vauth_inf.use_date<-2147000000) vauth_inf.use_date=0;
		if(vauth_inf.use_trace_no<-2147000000) vauth_inf.use_trace_no=0;
		if(vauth_inf.use_amt<-2147000000) vauth_inf.use_amt=0.0;
		if( vauth_inf.sts[0]=='0' ) strcpy(csts,"未用");
		else if( vauth_inf.sts[0]=='1' )strcpy(csts,"已用");
		else strcpy(csts,"废弃");


		fprintf( fp, "%ld|%s|%s|%ld|%s|%.2lf|%s|%s|%s|%ld|%s|%ld|%ld|%.2lf|%s|\n",vauth_inf.auth_no,vauth_inf.br_no,vauth_inf.tel,vauth_inf.mtr_date,vauth_inf.tx_code,vauth_inf.amt,vauth_inf.ac_no,vauth_inf.auth_tel,vauth_inf.auth_br_no,vauth_inf.auth_date,csts,vauth_inf.use_date,vauth_inf.use_trace_no,vauth_inf.use_amt,vauth_inf.use_tel);
		ttlnum++;
	}

	ret=Auth_inf_Clo_Sel( );
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
