/*************************************************
* 文 件 名:  sp1702.c
* 功能描述： 个人客户信息查询
*
* 作    者:  peter
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
#include "cif_per_inf_c.h"

int sp1702()
{
	struct cif_per_inf_c vcif_per_inf;
	struct cif_per_inf_c f_cif_per_inf;
	struct cif_per_inf_c f_cif_per_infa;
	char f_sex4[41];
	char f_marriage5[41];
	char f_edu7[41];
	char f_health14[41];
	char f_technical_post15[41];
	char f_headship16[41];
	char f_religion_ind[41];
	char f_nationality[41];
	char f_country[41];
	char f_folk[41];
	char f_language[41];
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

	get_zd_long("0540",&f_cif_per_inf.cif_no );
	get_zd_data("0630",f_cif_per_inf.name );
	get_zd_long("0290",&f_cif_per_inf.g_bir );
	get_zd_long("0440",&f_cif_per_infa.g_bir );
	get_zd_long("0450",&f_cif_per_inf.n_bir );
	get_zd_long("0460",&f_cif_per_infa.n_bir );
	get_zd_data("0700",f_cif_per_inf.sex );
	get_zd_data("0240",f_cif_per_inf.country );
	get_zd_data("0210",f_cif_per_inf.folk );
	get_zd_data("0260",f_cif_per_inf.birth_adr );
	get_zd_double("1001",&f_cif_per_inf.h_mon_income );
	get_zd_double("1002",&f_cif_per_infa.h_mon_income );
	get_zd_double("1004",&f_cif_per_inf.mon_income );
	get_zd_double("1003",&f_cif_per_infa.mon_income );

	/**组成查询条件**/
	if( f_cif_per_inf.cif_no )
	{
		sprintf( tmpstr," cif_no=%ld and",f_cif_per_inf.cif_no );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_cif_per_inf.name );
	if( strlen(f_cif_per_inf.name) )
	{
		sprintf( tmpstr," name like '%%%%%s%%%%' and",f_cif_per_inf.name );
		strcat( wherelist,tmpstr );
	}
	if( f_cif_per_inf.g_bir )
	{
		sprintf( tmpstr," g_bir>=%ld and",f_cif_per_inf.g_bir );
		strcat( wherelist,tmpstr );
	}
	if( f_cif_per_infa.g_bir )
	{
		sprintf( tmpstr," g_bir<=%ld and",f_cif_per_infa.g_bir );
		strcat( wherelist,tmpstr );
	}
	if( f_cif_per_inf.n_bir )
	{
		sprintf( tmpstr," n_bir>=%ld and",f_cif_per_inf.n_bir );
		strcat( wherelist,tmpstr );
	}
	if( f_cif_per_infa.n_bir )
	{
		sprintf( tmpstr," n_bir<=%ld and",f_cif_per_infa.n_bir );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_cif_per_inf.sex );
	if( strlen(f_cif_per_inf.sex) )
	{
		sprintf( tmpstr," sex='%s' and",f_cif_per_inf.sex );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_cif_per_inf.country );
	if( strlen(f_cif_per_inf.country) )
	{
		sprintf( tmpstr," country='%s' and",f_cif_per_inf.country );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_cif_per_inf.folk );
	if( strlen(f_cif_per_inf.folk) )
	{
		sprintf( tmpstr," folk='%s' and",f_cif_per_inf.folk );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_cif_per_inf.birth_adr );
	if( strlen(f_cif_per_inf.birth_adr) )
	{
		sprintf( tmpstr," birth_adr='%s' and",f_cif_per_inf.birth_adr );
		strcat( wherelist,tmpstr );
	}
	if( f_cif_per_inf.h_mon_income )
	{
		sprintf( tmpstr," h_mon_income>=%lf and",f_cif_per_inf.h_mon_income );
		strcat( wherelist,tmpstr );
	}
	if( f_cif_per_infa.h_mon_income )
	{
		sprintf( tmpstr," h_mon_income<=%lf and",f_cif_per_infa.h_mon_income );
		strcat( wherelist,tmpstr );
	}
	if( f_cif_per_inf.mon_income )
	{
		sprintf( tmpstr," mon_income>=%lf and",f_cif_per_inf.mon_income );
		strcat( wherelist,tmpstr );
	}
	if( f_cif_per_infa.mon_income )
	{
		sprintf( tmpstr," mon_income<=%lf and",f_cif_per_infa.mon_income );
		strcat( wherelist,tmpstr );
	}

	strcpy( tablelist,"cif_per_inf" );

	/**组成查询**/
	strcat(wherelist," 1=1") ;

	vtcp_log( "WHERE [%s]",wherelist );
	strcpy( fieldlist,"cif_no,name,g_bir,n_bir,sex,marriage,mar_date,edu,religion,nationality,country,folk,language,clerk_no,health,technical_post,headship,birth_adr,mon_income,h_mon_income,home_cnt");

	ret=Cif_per_inf_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

		ttlnum=0;

	while(1)
	{
		ret=Cif_per_inf_Fet_Sel( &vcif_per_inf, g_pub_tx.reply );
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
			fprintf( fp,"~客户代码|@客户尊称|公历出生日期|农历出生日期|@客户性别|@婚姻状态|结婚日期|@教育|@宗教信仰|@国籍代码|@居住国代码|@民族代码|@语言代码|@职员编号|@客户健康状况|@职称|@职务|@出生地|$月收入|$家庭月收入|负担家庭成员数目|\n" );
		}

		/*pub_pubdb_prt_fld( fp,tablelist,fieldlist ); */

		if(vcif_per_inf.cif_no<-2147000000) vcif_per_inf.cif_no=0;
		if(vcif_per_inf.g_bir<-2147000000) vcif_per_inf.g_bir=0;
		if(vcif_per_inf.n_bir<-2147000000) vcif_per_inf.n_bir=0;
		pub_base_dic_show_str(f_sex4,"cif_sex",vcif_per_inf.sex);
		pub_base_dic_show_str(f_marriage5,"cif_marriage",vcif_per_inf.marriage);
		if(vcif_per_inf.mar_date<-2147000000) vcif_per_inf.mar_date=0;
		pub_base_dic_show_str(f_edu7,"cif_edu",vcif_per_inf.edu);
		pub_base_dic_show_str(f_health14,"cif_health",vcif_per_inf.health);
		pub_base_dic_show_str(f_technical_post15,"technical_post",vcif_per_inf.technical_post);
		pub_base_dic_show_str(f_headship16,"cif_headship",vcif_per_inf.headship);
		pub_base_dic_show_str(f_religion_ind,"religion_ind",vcif_per_inf.religion);
		pub_base_dic_show_str(f_nationality,"nationality",vcif_per_inf.nationality);
		pub_base_dic_show_str(f_country,"nationality",vcif_per_inf.country);
		pub_base_dic_show_str(f_folk,"folk",vcif_per_inf.folk);
		pub_base_dic_show_str(f_language,"language",vcif_per_inf.language);
		if(vcif_per_inf.mon_income<-2147000000) vcif_per_inf.mon_income=0.0;
		if(vcif_per_inf.h_mon_income<-2147000000) vcif_per_inf.h_mon_income=0.0;

		fprintf( fp, "%ld|%s|%ld|%ld|%s|%s|%ld|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%.2lf|%.2lf|%d|\n",vcif_per_inf.cif_no,vcif_per_inf.name,vcif_per_inf.g_bir,vcif_per_inf.n_bir,f_sex4,f_marriage5,vcif_per_inf.mar_date,f_edu7,f_religion_ind,f_nationality,f_country,f_folk,f_language,vcif_per_inf.clerk_no,f_health14,f_technical_post15,f_headship16,vcif_per_inf.birth_adr,vcif_per_inf.mon_income,vcif_per_inf.h_mon_income,vcif_per_inf.home_cnt);
		ttlnum++;
	}

	ret=Cif_per_inf_Clo_Sel( );
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
