/*************************************************
* 文 件 名:  sp1711.c
* 功能描述： 机构客户信息查询
*
* 作    者:  peter
* 完成日期： 2004-06-01
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
#include "cif_magn_inf_c.h"
#include "cif_basic_inf_c.h"

int sp1711()
{
	struct cif_magn_inf_c vcif_magn_inf;
	struct cif_magn_inf_c f_cif_magn_inf;
	struct cif_basic_inf_c cif_basic_inf;
		
	char f_credit8[41];
		char filename[100];
		char wherelist[1024];  /**查询条件**/
		char tmpstr[512];
		char tmpmo[512];
		char fieldlist[1024];  /**要查询的字段列表**/
		char tablelist[128];   /**要查询的表名**/
		char f_ent_attri8[11];
		char f_own_attri8[11];
		char titstr[1024];
		int ttlnum=0;			/**读取的总条数**/
		int i=0;
		int ret=0;
		int ret_tmp=0;
		double amt_min=0.00;
		double amt_max=0.00;
		FILE *fp;

	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset( wherelist,0,sizeof(wherelist) );
	memset( fieldlist,0,sizeof(fieldlist) );
	memset( tablelist,0,sizeof(tablelist) );

	pub_base_sysinit();

	get_zd_long("0540",&f_cif_magn_inf.cif_no );
	get_zd_data("0630",f_cif_magn_inf.units_license );
	get_zd_data("0620",f_cif_magn_inf.ap_qualife );
	get_zd_data("0320",f_cif_magn_inf.artificial_person );
	get_zd_data("0330",f_cif_magn_inf.ap_id );
	get_zd_data("0590",f_cif_magn_inf.credit_no );
	get_zd_data("0670",f_cif_magn_inf.own_attri );
	get_zd_data("0700",f_cif_magn_inf.ent_attri );
	get_zd_data("0680",f_cif_magn_inf.credit );
	get_zd_data("0260",f_cif_magn_inf.supervior );
	get_zd_data("0250",f_cif_magn_inf.director );
	get_zd_data("0270",f_cif_magn_inf.financer );
	get_zd_data("0810",f_cif_magn_inf.opn_bank );
	get_zd_double("0410",&amt_min );
	get_zd_double("0420",&amt_max );
	
	/**组成查询条件**/
	if( f_cif_magn_inf.cif_no )
	{
		sprintf( tmpstr," cif_no=%ld and",f_cif_magn_inf.cif_no );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_cif_magn_inf.units_license );
	if( strlen(f_cif_magn_inf.units_license) )
	{
		sprintf( tmpstr," units_license='%s' and",f_cif_magn_inf.units_license );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_cif_magn_inf.ap_qualife );
	if( strlen(f_cif_magn_inf.ap_qualife) )
	{
		sprintf( tmpstr," ap_qualife='%s' and",f_cif_magn_inf.ap_qualife );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_cif_magn_inf.artificial_person );
	if( strlen(f_cif_magn_inf.artificial_person) )
	{
		sprintf( tmpstr," artificial_person like '%%%%%s%%%%' and",f_cif_magn_inf.artificial_person );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_cif_magn_inf.ap_id );
	if( strlen(f_cif_magn_inf.ap_id) )
	{
		sprintf( tmpstr," ap_id='%s' and",f_cif_magn_inf.ap_id );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_cif_magn_inf.credit_no );
	if( strlen(f_cif_magn_inf.credit_no) )
	{
		sprintf( tmpstr," credit_no='%s' and",f_cif_magn_inf.credit_no );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_cif_magn_inf.own_attri );
	if( strlen(f_cif_magn_inf.own_attri) )
	{
		sprintf( tmpstr," own_attri='%s' and",f_cif_magn_inf.own_attri );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_cif_magn_inf.ent_attri );
	if( strlen(f_cif_magn_inf.ent_attri) )
	{
		sprintf( tmpstr," ent_attri='%s' and",f_cif_magn_inf.ent_attri );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_cif_magn_inf.credit );
	if( strlen(f_cif_magn_inf.credit) )
	{
		sprintf( tmpstr," credit='%s' and",f_cif_magn_inf.credit );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_cif_magn_inf.supervior );
	if( strlen(f_cif_magn_inf.supervior) )
	{
		sprintf( tmpstr," supervior like '%%%%%s%%%%' and",f_cif_magn_inf.supervior );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_cif_magn_inf.director );
	if( strlen(f_cif_magn_inf.director) )
	{
		sprintf( tmpstr," director like '%%%%%s%%%%' and",f_cif_magn_inf.director );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_cif_magn_inf.financer );
	if( strlen(f_cif_magn_inf.financer) )
	{
		sprintf( tmpstr," financer like '%%%%%s%%%%' and",f_cif_magn_inf.financer );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_cif_magn_inf.opn_bank );
	if( strlen(f_cif_magn_inf.opn_bank) )
	{
		sprintf( tmpstr," opn_bank like  '%%%%%s%%%%' and",f_cif_magn_inf.opn_bank );
		strcat( wherelist,tmpstr );
	}
	if( amt_min )
	{
		sprintf( tmpstr," opn_amt<%lf and opn_amt>%lf and ",amt_max , amt_min );
		strcat( wherelist,tmpstr );
	}

	strcpy( tablelist,"cif_magn_inf" );

	/**组成查询**/
	strcat(wherelist," 1=1") ;

	vtcp_log( "WHERE [%s]",wherelist );
	strcpy( fieldlist,"cif_no,units_license,ap_qualife,artificial_person,ap_id,credit_no,own_attri,ent_attri,credit,supervior,director,financer,opn_bank,opn_amt");

	ret=Cif_magn_inf_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

		ttlnum=0;

	while(1)
	{
		ret=Cif_magn_inf_Fet_Sel( &vcif_magn_inf, g_pub_tx.reply );
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
			fprintf( fp,"~客户号|@机构名称|@机构代码证号|@事业法人证号|@法人代表名称|@法人身份证号|@贷款卡号|@所有制性质|@单位性质|@信誉度|@上级单位|@主管单位|@财政资金调拨单位|@基本账号开户行|$开办资金|\n" );
		}

		/*pub_pubdb_prt_fld( fp,tablelist,fieldlist ); */

		if(vcif_magn_inf.cif_no<-2147000000) vcif_magn_inf.cif_no=0;
		pub_base_dic_show_str(f_credit8,"cif_credit",vcif_magn_inf.credit);
		if(vcif_magn_inf.opn_amt<-2147000000) vcif_magn_inf.opn_amt=0.0;
		pub_base_dic_show_str(f_ent_attri8,"ent_attri8",vcif_magn_inf.ent_attri);
		pub_base_dic_show_str(f_own_attri8,"own_attri8",vcif_magn_inf.own_attri);		
		
		/* 查询客户名称 */
		ret_tmp = Cif_basic_inf_Sel(g_pub_tx.reply,&cif_basic_inf,"cif_no=%ld",vcif_magn_inf.cif_no );
		if( ret_tmp )
		{
			strcpy( cif_basic_inf.name , "无此公司名称");
		}
		
		fprintf( fp, "%ld|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%.2lf|\n",vcif_magn_inf.cif_no,cif_basic_inf.name,vcif_magn_inf.units_license,vcif_magn_inf.ap_qualife,vcif_magn_inf.artificial_person,vcif_magn_inf.ap_id,vcif_magn_inf.credit_no,f_own_attri8,f_own_attri8,f_credit8,vcif_magn_inf.supervior,vcif_magn_inf.director,vcif_magn_inf.financer,vcif_magn_inf.opn_bank,vcif_magn_inf.opn_amt);
		ttlnum++;
	}

	ret=Cif_magn_inf_Clo_Sel( );
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
