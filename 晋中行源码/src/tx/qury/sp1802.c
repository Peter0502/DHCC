/*************************************************
* 文 件 名:  sp1802.c
* 功能描述： 客户与产品关系查询
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
#include "cif_prdt_rel_c.h"

int sp1802()
{
	struct cif_prdt_rel_c vcif_prdt_rel;
	struct cif_prdt_rel_c f_cif_prdt_rel;
	struct cif_prdt_rel_c f_cif_prdt_rela;
	char ac_no2[25];
	char f_relation_no3[41];
	char f_relation_flag4[41];
	char tmp_title[41];
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

	get_zd_long("0540",&f_cif_prdt_rel.cif_no );
	get_zd_data("0240",f_cif_prdt_rel.prdt_code );
	get_zd_data("0210",f_cif_prdt_rel.relation_no );
	get_zd_long("0290",&f_cif_prdt_rel.crt_date );
	get_zd_long("0440",&f_cif_prdt_rela.crt_date );

	/**组成查询条件**/
	if( f_cif_prdt_rel.cif_no )
	{
		sprintf( tmpstr," cif_no=%ld and",f_cif_prdt_rel.cif_no );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_cif_prdt_rel.prdt_code );
	if( strlen(f_cif_prdt_rel.prdt_code) )
	{
		sprintf( tmpstr," prdt_code='%s' and",f_cif_prdt_rel.prdt_code );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_cif_prdt_rel.relation_no );
	if( strlen(f_cif_prdt_rel.relation_no) )
	{
		sprintf( tmpstr," relation_no='%s' and",f_cif_prdt_rel.relation_no );
		strcat( wherelist,tmpstr );
	}
	if( f_cif_prdt_rel.crt_date )
	{
		sprintf( tmpstr," crt_date>=%ld and",f_cif_prdt_rel.crt_date );
		strcat( wherelist,tmpstr );
	}
	if( f_cif_prdt_rela.crt_date )
	{
		sprintf( tmpstr," crt_date<=%ld and",f_cif_prdt_rela.crt_date );
		strcat( wherelist,tmpstr );
	}

	strcpy( tablelist,"cif_prdt_rel" );

	/**组成查询**/
	strcat(wherelist," 1=1") ;

	vtcp_log( "WHERE [%s]",wherelist );
	strcpy( fieldlist,"cif_no,prdt_code,ac_id,relation_no,relation_flag,addr_seri,crt_date,cls_date");

	ret=Cif_prdt_rel_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

	ttlnum=0;

	while(1)
	{
		ret=Cif_prdt_rel_Fet_Sel( &vcif_prdt_rel, g_pub_tx.reply );
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
			fprintf( fp,"~客户代码|@产品名称|账户号码|@关系代码|@关系状态|账单地址|建立日期|解除日期|\n" );
		}

		/*pub_pubdb_prt_fld( fp,tablelist,fieldlist ); */

		switch(vcif_prdt_rel.prdt_code[0])
		{
			case '1':
					 ret=Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm, \
						 "prdt_no='%s'",vcif_prdt_rel.prdt_code);
					 strcpy(tmp_title,g_dd_parm.title);
					 break;
			case '2':
					 ret=Td_parm_Sel(g_pub_tx.reply,&g_td_parm, \
						 "prdt_no='%s'",vcif_prdt_rel.prdt_code);
					 strcpy(tmp_title,g_td_parm.title);
					 break;
			case '3':
					 ret=Ln_parm_Sel(g_pub_tx.reply,&g_ln_parm, \
						 "prdt_no='%s'",vcif_prdt_rel.prdt_code);
					 strcpy(tmp_title,g_ln_parm.title);
					 break;
			 default:
					 break;
		}

		if(vcif_prdt_rel.cif_no<-2147000000) vcif_prdt_rel.cif_no=0;
		pub_base_acid_acno( vcif_prdt_rel.ac_id,0,ac_no2 );
		pub_base_dic_show_str(f_relation_no3,"relation_no",vcif_prdt_rel.relation_no);
		pub_base_dic_show_str(f_relation_flag4,"rel_sts",vcif_prdt_rel.relation_flag);
		if(vcif_prdt_rel.crt_date<-2147000000) vcif_prdt_rel.crt_date=0;
		if(vcif_prdt_rel.cls_date<-2147000000) vcif_prdt_rel.cls_date=0;

		fprintf( fp, "%ld|%s|%s|%s|%s|%d|%ld|%ld|\n",vcif_prdt_rel.cif_no,tmp_title,ac_no2,f_relation_no3,f_relation_flag4,vcif_prdt_rel.addr_seri,vcif_prdt_rel.crt_date,vcif_prdt_rel.cls_date);
		ttlnum++;
	}

	ret=Cif_prdt_rel_Clo_Sel( );
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
