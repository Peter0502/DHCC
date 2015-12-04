/*************************************************
* 文 件 名:  sp1703.c
* 功能描述： 公司客户信息查询
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
#include "cif_cop_inf_c.h"
#include "cif_basic_inf_c.h"

int sp1703()
{
	struct cif_cop_inf_c vcif_cop_inf;
	struct cif_cop_inf_c f_cif_cop_inf;
	struct cif_cop_inf_c f_cif_cop_infa;
	struct cif_basic_inf_c cif_basic_inf;	
	
	char f_eco_attri11[41];
	char f_ent_attri12[41];
	char f_credit13[41];
	char f_region14[41];
	char f_belong_bank17[41];
	char f_belong_stat18[41];
	char f_mana_orga_moda19[41];
	char f_bd_corp_orga_moda20[41];
	char f_corp_size21[41];
	char f_corp_act_mode22[41];
	char f_corp_mana_sts24[41];
	char f_cif_scopes28[41];
	char f_cur_name[41];
	char f_country[41];
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
		int ret_tmp=0;
		FILE *fp;

	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset( wherelist,0,sizeof(wherelist) );
	memset( fieldlist,0,sizeof(fieldlist) );
	memset( tablelist,0,sizeof(tablelist) );

	pub_base_sysinit();
	memset(f_eco_attri11,0x00,sizeof(f_eco_attri11));
	memset(f_ent_attri12,0x00,sizeof(f_ent_attri12));
	memset(f_credit13,0x00,sizeof(f_credit13));
	memset(f_region14,0x00,sizeof(f_region14));
	memset(f_belong_bank17,0x00,sizeof(f_belong_bank17));
	memset(f_belong_stat18,0x00,sizeof(f_belong_stat18));
	memset(f_mana_orga_moda19,0x00,sizeof(f_mana_orga_moda19));
	memset(f_bd_corp_orga_moda20,0x00,sizeof(f_bd_corp_orga_moda20));
	memset(f_corp_size21,0x00,sizeof(f_corp_size21));
	memset(f_corp_act_mode22,0x00,sizeof(f_corp_act_mode22));
	memset(f_corp_mana_sts24,0x00,sizeof(f_corp_mana_sts24));
	memset(f_cif_scopes28,0x00,sizeof(f_cif_scopes28));
	memset(f_cur_name,0x00,sizeof(f_cur_name));
	memset(f_country,0x00,sizeof(f_country));
	memset(&vcif_cop_inf,0x00,sizeof(struct cif_cop_inf_c));
	memset(&f_cif_cop_inf,0x00,sizeof(struct cif_cop_inf_c));
	memset(&f_cif_cop_infa,0x00,sizeof(struct cif_cop_inf_c));
	memset(&cif_basic_inf,0x00,sizeof(struct cif_basic_inf_c));
	memset(filename,0x00,sizeof(filename));
        memset(tmpstr,0x00,sizeof(tmpstr));
        memset(tmpmo,0x00,sizeof(tmpmo));

        memset(titstr,0x00,sizeof(titstr));

	get_zd_long("0540",&f_cif_cop_inf.cif_no );
	get_zd_long("0290",&f_cif_cop_inf.reg_date );
	get_zd_long("0440",&f_cif_cop_infa.reg_date );
	get_zd_data("0240",f_cif_cop_inf.reg_country );
	get_zd_double("0410",&f_cif_cop_inf.reg_fund );
	get_zd_double("0420",&f_cif_cop_infa.reg_fund );
	get_zd_data("0210",f_cif_cop_inf.cur_no );
	get_zd_data("0630",f_cif_cop_inf.crd_no );
	get_zd_data("0620",f_cif_cop_inf.units_license );
	get_zd_data("0320",f_cif_cop_inf.license );
	get_zd_data("0700",f_cif_cop_inf.belong_bank );
	get_zd_data("0660",f_cif_cop_inf.belong_stat );
	get_zd_data("0330",f_cif_cop_inf.artificial_person );
	get_zd_data("0300",f_cif_cop_inf.ap_id );
	get_zd_data("102A",f_cif_cop_inf.ap_qualife );
	

	
	/**组成查询条件**/
	if( f_cif_cop_inf.cif_no )
	{
		sprintf( tmpstr," cif_no=%ld and",f_cif_cop_inf.cif_no );
		strcat( wherelist,tmpstr );
	}
	if( f_cif_cop_inf.reg_date )
	{
		sprintf( tmpstr," reg_date>=%ld and",f_cif_cop_inf.reg_date );
		strcat( wherelist,tmpstr );
	}
	if( f_cif_cop_infa.reg_date )
	{
		sprintf( tmpstr," reg_date<=%ld and",f_cif_cop_infa.reg_date );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_cif_cop_inf.reg_country );
	if( strlen(f_cif_cop_inf.reg_country) )
	{
		sprintf( tmpstr," reg_country='%s' and",f_cif_cop_inf.reg_country );
		strcat( wherelist,tmpstr );
	}
	if( f_cif_cop_inf.reg_fund )
	{
		sprintf( tmpstr," reg_fund>=%lf and",f_cif_cop_inf.reg_fund );
		strcat( wherelist,tmpstr );
	}
	if( f_cif_cop_infa.reg_fund )
	{
		sprintf( tmpstr," reg_fund<=%lf and",f_cif_cop_infa.reg_fund );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_cif_cop_inf.cur_no );
	if( strlen(f_cif_cop_inf.cur_no) )
	{
		sprintf( tmpstr," cur_no='%s' and",f_cif_cop_inf.cur_no );
		strcat( wherelist,tmpstr );
	}	
	pub_base_strpack( f_cif_cop_inf.ap_qualife );
	if( strlen(f_cif_cop_inf.ap_qualife) )
	{
		sprintf( tmpstr," ap_qualife='%s' and",f_cif_cop_inf.ap_qualife );
		strcat( wherelist,tmpstr );
	}		
	pub_base_strpack( f_cif_cop_inf.crd_no );
	if( strlen(f_cif_cop_inf.crd_no) )
	{
		sprintf( tmpstr," crd_no='%s' and",f_cif_cop_inf.crd_no );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_cif_cop_inf.units_license );
	if( strlen(f_cif_cop_inf.units_license) )
	{
		sprintf( tmpstr," units_license='%s' and",f_cif_cop_inf.units_license );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_cif_cop_inf.license );
	if( strlen(f_cif_cop_inf.license) )
	{
		sprintf( tmpstr," license='%s' and",f_cif_cop_inf.license );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_cif_cop_inf.belong_bank );
	if( strlen(f_cif_cop_inf.belong_bank) )
	{
		sprintf( tmpstr," belong_bank='%s' and",f_cif_cop_inf.belong_bank );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_cif_cop_inf.belong_stat );
	if( strlen(f_cif_cop_inf.belong_stat) )
	{
		sprintf( tmpstr," belong_stat='%s' and",f_cif_cop_inf.belong_stat );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_cif_cop_inf.artificial_person );
	if( strlen(f_cif_cop_inf.artificial_person) )
	{
		sprintf( tmpstr," artificial_person='%s' and",f_cif_cop_inf.artificial_person );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_cif_cop_inf.ap_id );
	if( strlen(f_cif_cop_inf.ap_id) )
	{
		sprintf( tmpstr," ap_id='%s' and",f_cif_cop_inf.ap_id );
		strcat( wherelist,tmpstr );
	}

	strcpy( tablelist,"cif_cop_inf" );

	/**组成查询**/
	strcat(wherelist," 1=1") ;

	vtcp_log( "WHERE [%s]",wherelist );
	strcpy( fieldlist,"cif_no,reg_date,reg_country,reg_fund,cur_no,crd_no,units_license,g_tax_no,d_tax_no,credit_no,license,eco_attri,ent_attri,credit,region,supervior,director,belong_bank,belong_stat,mana_orga_moda,bd_corp_orga_moda,corp_size,corp_act_mode,paicl_up_capital,corp_mana_sts,ap_qualife,artificial_person,ap_id,cif_scopes");

	ret=Cif_cop_inf_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

		ttlnum=0;

	while(1)
	{
		ret=Cif_cop_inf_Fet_Sel( &vcif_cop_inf, g_pub_tx.reply );
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
			fprintf( fp,"~客户号|@公司名称|@代码证号|@营业执照|@经营许可证|@企业法人证书|@法定代表名称|@法定身份证号|@企业性质|@经营形式|@产权形式|@企业规模|@信誉度|@贷款卡号|@国税登记证|@地税登记证|@归属行业(人行)|@所属行业(统计局)|年检日期|@注册国|$注册资金|@币种|$实收资本|@上级单位|@主管单位|\n" );
		}

		/*pub_pubdb_prt_fld( fp,tablelist,fieldlist ); */

		if(vcif_cop_inf.cif_no<-2147000000) vcif_cop_inf.cif_no=0;
		if(vcif_cop_inf.reg_date<-2147000000) vcif_cop_inf.reg_date=0;
		if(vcif_cop_inf.reg_fund<-2147000000) vcif_cop_inf.reg_fund=0.0;
		pub_base_dic_show_str(f_eco_attri11,"eco_attri",vcif_cop_inf.eco_attri);
		pub_base_dic_show_str(f_ent_attri12,"ent_attri",vcif_cop_inf.ent_attri);
		pub_base_dic_show_str(f_credit13,"cif_credit",vcif_cop_inf.credit);
		pub_base_dic_show_str(f_region14,"cif_region",vcif_cop_inf.region);
		pub_base_dic_show_str(f_belong_bank17,"cif_belong_bank",vcif_cop_inf.belong_bank);
		pub_base_dic_show_str(f_belong_stat18,"cif_belong_stat",vcif_cop_inf.belong_stat);
		pub_base_dic_show_str(f_mana_orga_moda19,"mana_orga_moda",vcif_cop_inf.mana_orga_moda);
		pub_base_dic_show_str(f_bd_corp_orga_moda20,"corp_orga_moda",vcif_cop_inf.bd_corp_orga_moda);
		pub_base_dic_show_str(f_corp_size21,"corp_size",vcif_cop_inf.corp_size);
		pub_base_dic_show_str(f_corp_act_mode22,"corp_act_mode",vcif_cop_inf.corp_act_mode);
		if(vcif_cop_inf.paicl_up_capital<-2147000000) vcif_cop_inf.paicl_up_capital=0.0;
		pub_base_dic_show_str(f_corp_mana_sts24,"corp_mana_sts",vcif_cop_inf.corp_mana_sts);
		pub_base_dic_show_str(f_cif_scopes28,"cif_scopes",vcif_cop_inf.cif_scopes);
		pub_base_dic_show_str(f_cur_name,"cur_name",vcif_cop_inf.cur_no);
		pub_base_dic_show_str(f_country,"nationality",vcif_cop_inf.reg_country);
		
		/* 查询客户名称 */
		ret_tmp = Cif_basic_inf_Sel(g_pub_tx.reply,&cif_basic_inf,"cif_no=%ld",
							vcif_cop_inf.cif_no );
		if( ret_tmp )
		{
			strcpy( cif_basic_inf.name , "无此公司名称");
		}	

		sprintf(acErrMsg,"!!!!!! [%s]", cif_basic_inf.name);
		WRITEMSG

		fprintf( fp, "%ld|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%ld|%s|%.2lf|%s|%.2lf|%s|%s|\n",vcif_cop_inf.cif_no,cif_basic_inf.name,vcif_cop_inf.crd_no,vcif_cop_inf.license,vcif_cop_inf.units_license,vcif_cop_inf.ap_qualife,vcif_cop_inf.artificial_person,vcif_cop_inf.ap_id,f_ent_attri12,f_mana_orga_moda19,f_bd_corp_orga_moda20,f_corp_size21,f_credit13,vcif_cop_inf.credit_no,vcif_cop_inf.g_tax_no,vcif_cop_inf.d_tax_no,f_belong_bank17,f_belong_stat18,vcif_cop_inf.reg_date,f_country,vcif_cop_inf.reg_fund,f_cur_name,vcif_cop_inf.paicl_up_capital,vcif_cop_inf.supervior,vcif_cop_inf.director);
		ttlnum++;
	}

	ret=Cif_cop_inf_Clo_Sel( );
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
