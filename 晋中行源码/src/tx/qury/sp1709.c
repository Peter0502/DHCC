/*************************************************
* 文 件 名:  sp1709.c
* 功能描述： 其他金融机构产品信息
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
#include "cif_oth_prdt_inf_c.h"

int sp1709()
{
	struct cif_oth_prdt_inf_c vcif_oth_prdt_inf;
	struct cif_oth_prdt_inf_c f_cif_oth_prdt_inf;
	char f_prdt_code2[41];
	char f_unit_type3[41];
	char tmp1[21];
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

	get_zd_long("0540",&f_cif_oth_prdt_inf.cif_no );
	get_zd_long("0510",&f_cif_oth_prdt_inf.inf_no );
	get_zd_data("0210",f_cif_oth_prdt_inf.prdt_code );
	get_zd_data("0220",f_cif_oth_prdt_inf.unit_type );
	get_zd_data("0640",f_cif_oth_prdt_inf.unit_no );

	/**组成查询条件**/
	if( f_cif_oth_prdt_inf.cif_no )
	{
		sprintf( tmpstr," cif_no=%ld and",f_cif_oth_prdt_inf.cif_no );
		strcat( wherelist,tmpstr );
	}
	if( f_cif_oth_prdt_inf.inf_no )
	{
		sprintf( tmpstr," inf_no=%d and",f_cif_oth_prdt_inf.inf_no );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_cif_oth_prdt_inf.prdt_code );
	if( strlen(f_cif_oth_prdt_inf.prdt_code) )
	{
		sprintf( tmpstr," prdt_code='%s' and",f_cif_oth_prdt_inf.prdt_code );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_cif_oth_prdt_inf.unit_type );
	if( strlen(f_cif_oth_prdt_inf.unit_type) )
	{
		sprintf( tmpstr," unit_type='%s' and",f_cif_oth_prdt_inf.unit_type );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_cif_oth_prdt_inf.unit_no );
	if( strlen(f_cif_oth_prdt_inf.unit_no) )
	{
		sprintf( tmpstr," unit_no='%s' and",f_cif_oth_prdt_inf.unit_no );
		strcat( wherelist,tmpstr );
	}

	strcpy( tablelist,"cif_oth_prdt_inf" );

	/**组成查询**/
	strcat(wherelist," 1=1") ;

	vtcp_log( "WHERE [%s]",wherelist );
	strcpy( fieldlist,"cif_no,inf_no,prdt_code,unit_type,unit_no,ac_no,pro_cur_no,balance,mon_pay");

	ret=Cif_oth_prdt_inf_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

		ttlnum=0;

	while(1)
	{
		ret=Cif_oth_prdt_inf_Fet_Sel( &vcif_oth_prdt_inf, g_pub_tx.reply );
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
			fprintf( fp,"~客户代码|序号|@产品类型|@机构类型|@机构编号|@账号/卡号|@产品币种|$存贷余额|$月还款额|\n" );
		}

		/*pub_pubdb_prt_fld( fp,tablelist,fieldlist ); */
		pub_base_dic_show_str(tmp1,"cur_name",vcif_oth_prdt_inf.pro_cur_no);

		if(vcif_oth_prdt_inf.cif_no<-2147000000) vcif_oth_prdt_inf.cif_no=0;
		pub_base_dic_show_str(f_prdt_code2,"prdt_type",vcif_oth_prdt_inf.prdt_code);
		pub_base_dic_show_str(f_unit_type3,"unit_type",vcif_oth_prdt_inf.unit_type);
		if(vcif_oth_prdt_inf.balance<-2147000000) vcif_oth_prdt_inf.balance=0.0;
		if(vcif_oth_prdt_inf.mon_pay<-2147000000) vcif_oth_prdt_inf.mon_pay=0.0;

		fprintf( fp, "%ld|%d|%s|%s|%s|%s|%s|%.2lf|%.2lf|\n",vcif_oth_prdt_inf.cif_no,vcif_oth_prdt_inf.inf_no,f_prdt_code2,f_unit_type3,vcif_oth_prdt_inf.unit_no,vcif_oth_prdt_inf.ac_no,tmp1,vcif_oth_prdt_inf.balance,vcif_oth_prdt_inf.mon_pay);
		ttlnum++;
	}

	ret=Cif_oth_prdt_inf_Clo_Sel( );
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
