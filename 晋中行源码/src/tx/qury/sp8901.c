/*************************************************
* 文 件 名:  sp8901.c
* 功能描述： 开销户登记簿查询
*
* 作    者:  lance
* 完成日期： 200306
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
#include "mo_opn_cls_c.h"
#include "com_item_c.h"

int sp8901()
{
	struct mo_opn_cls_c vmo_opn_cls;
	struct mo_opn_cls_c f_mo_opn_cls;
	struct mo_opn_cls_c f_mo_opn_clsa;
	struct com_item_c com_item;
	char f_cif_type5[41];
	char opn_ind[2];
	char f_ct_ind14[41];
	char f_wrk_sts15[41];
	int ret=0;
		char filename[100];
		char wherelist[1024];  /**查询条件**/
		char tmpstr[512];
		char tmpmo[512];
		char fieldlist[1024];  /**要查询的字段列表**/
		char tablelist[128];   /**要查询的表名**/
		char titstr[1024];
		int ttlnum=0;			/**读取的总条数**/
		int i=0;
		long data_tmp;
		FILE *fp;

	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset( wherelist,0,sizeof(wherelist) );
	memset( fieldlist,0,sizeof(fieldlist) );
	memset( tablelist,0,sizeof(tablelist) );

	pub_base_sysinit();

	get_zd_data("0380",f_mo_opn_cls.ac_no );
	get_zd_long("0510",&f_mo_opn_cls.ac_seqn );
	get_zd_data("0210",f_mo_opn_cls.cur_no );
	get_zd_data("0260",f_mo_opn_cls.name );
	get_zd_data("0700",f_mo_opn_cls.cif_type );
	get_zd_double("1001",&f_mo_opn_cls.amt );
	get_zd_double("1002",&f_mo_opn_clsa.amt );
	get_zd_data("0910",f_mo_opn_cls.opn_br_no );
	get_zd_long("0290",&f_mo_opn_cls.tx_date );
	get_zd_data("0640",f_mo_opn_cls.tel );
	get_zd_long("0440",&data_tmp );
	get_zd_data("0670",opn_ind );
	
	/**组成查询条件**/
	pub_base_strpack( f_mo_opn_cls.ac_no );
	if( strlen(f_mo_opn_cls.ac_no) )
	{
		sprintf( tmpstr," ac_no='%s' and",f_mo_opn_cls.ac_no );
		strcat( wherelist,tmpstr );
	}
	if( f_mo_opn_cls.ac_seqn )
	{
		sprintf( tmpstr," ac_seqn=%d and",f_mo_opn_cls.ac_seqn );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_mo_opn_cls.cur_no );
	if( strlen(f_mo_opn_cls.cur_no) )
	{
		sprintf( tmpstr," cur_no='%s' and",f_mo_opn_cls.cur_no );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_mo_opn_cls.name );
	if( strlen(f_mo_opn_cls.name) )
	{
		sprintf( tmpstr," name like '%%%%%s%%%%' and",f_mo_opn_cls.name );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_mo_opn_cls.cif_type );
	if( strlen(f_mo_opn_cls.cif_type) )
	{
		sprintf( tmpstr," cif_type='%s' and",f_mo_opn_cls.cif_type );
		strcat( wherelist,tmpstr );
	}
	if( f_mo_opn_cls.amt )
	{
		sprintf( tmpstr," amt>=%lf and",f_mo_opn_cls.amt );
		strcat( wherelist,tmpstr );
	}
	if( f_mo_opn_clsa.amt )
	{
		sprintf( tmpstr," amt<=%lf and",f_mo_opn_clsa.amt );
		strcat( wherelist,tmpstr );
	}
	/*
	pub_base_strpack( f_mo_opn_cls.opn_br_no );
	if( strlen(f_mo_opn_cls.opn_br_no) )
	*/
	/**只差本网点的**/
	{
		sprintf( tmpstr," opn_br_no='%s' and",g_pub_tx.tx_br_no );
		strcat( wherelist,tmpstr );
	}
		
	if( f_mo_opn_cls.tx_date )
	{
		sprintf( tmpstr," tx_date>=%ld and",f_mo_opn_cls.tx_date );
		strcat( wherelist,tmpstr );
	}
	if( data_tmp )
	{
		sprintf( tmpstr," tx_date<=%ld and",data_tmp );
		strcat( wherelist,tmpstr );
	}	
	if( strlen(opn_ind) )
	{
		sprintf( tmpstr," wrk_sts='%s' and",opn_ind );
		strcat( wherelist,tmpstr );
	}	
	
	
	pub_base_strpack( f_mo_opn_cls.tel );
	if( strlen(f_mo_opn_cls.tel) )
	{
		sprintf( tmpstr," tel='%s' and",f_mo_opn_cls.tel );
		strcat( wherelist,tmpstr );
	}

	strcpy( tablelist,"mo_opn_cls" );

	/**组成查询**/
	strcat(wherelist," 1=1 ORDER BY wrk_sts,tx_date,trace_no,amt,tx_brno ") ;

	vtcp_log( "WHERE [%s]",wherelist );
	strcpy( fieldlist,"ac_no,ac_seqn,cur_no,name,acc_hrt,cif_type,amt,opn_br_no,tx_date,tx_brno,trace_no,tel,chk,auth,ct_ind,wrk_sts");

	ret=Mo_opn_cls_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

		ttlnum=0;

	while(1)
	{
		ret=Mo_opn_cls_Fet_Sel( &vmo_opn_cls, g_pub_tx.reply );
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
	fprintf( fp,"~@  客  户  帐  号|序号|@    户    名|$   金      额|@科目号|@状态|交易日期|@交易机构|@现转|@操作员|@复核员|@授权人|流水号|@客户类型|@开户行|\n" );
		}
		sprintf(acErrMsg, "CGH***** [%s]", vmo_opn_cls.cif_type);
		WRITEMSG
		pub_base_dic_show_str(f_cif_type5,"cif_type",vmo_opn_cls.cif_type);

		/*pub_pubdb_prt_fld( fp,tablelist,fieldlist ); */
		pub_base_strpack(vmo_opn_cls.acc_hrt);
				sprintf(acErrMsg,"[%s]=[%s]AAAAA",
					vmo_opn_cls.acc_hrt,vmo_opn_cls.prdt_no);
				WRITEMSG
		if(!strlen(vmo_opn_cls.acc_hrt) && strlen(vmo_opn_cls.prdt_no) )
		{
			if( vmo_opn_cls.prdt_no[0]=='1' )
			{
				ret= Com_item_Sel(g_pub_tx.reply,&com_item,"acc_hrt=(select acc_hrt from dc_acc_rel where data_code='0152' and dc_code in(select dc_code from dd_parm where prdt_no='%s'))",vmo_opn_cls.prdt_no ) ;
				sprintf(acErrMsg,"RET=%d",ret);
				WRITEMSG
			}
			else if( vmo_opn_cls.prdt_no[0]=='2' )
			{
				ret= Com_item_Sel(g_pub_tx.reply,&com_item,"acc_hrt=(select acc_hrt from dc_acc_rel where data_code='0152' and dc_code in(select dc_code from td_parm where prdt_no='%s'))",vmo_opn_cls.prdt_no ) ;
				sprintf(acErrMsg,"RET=%d",ret);
				WRITEMSG
			}
			else if( vmo_opn_cls.prdt_no[0]=='3' )
			{
				ret= Com_item_Sel(g_pub_tx.reply,&com_item,"acc_hrt=(select acc_hrt from dc_acc_rel where data_code='0152' and dc_code in(select dc_code from ln_parm where prdt_no='%s'))",vmo_opn_cls.prdt_no ) ;
				sprintf(acErrMsg,"RET=%d",ret);
				WRITEMSG
			}
			else if( vmo_opn_cls.prdt_no[0]=='9' )
			{
				ret= Com_item_Sel(g_pub_tx.reply,&com_item,"acc_hrt=(select acc_hrt from dc_acc_rel where data_code='0152' and dc_code in(select dc_code from in_parm where prdt_no='%s'))",vmo_opn_cls.prdt_no ) ;
				sprintf(acErrMsg,"RET=%d",ret);
				WRITEMSG
			}
			else
			{
				ret=1;
			}
			if( ret )
				strcpy( vmo_opn_cls.acc_hrt,"" );
			else
				strcpy( vmo_opn_cls.acc_hrt,com_item.acc_no );

		}

		sprintf(acErrMsg, "CGH***** [%s]", vmo_opn_cls.cif_type);
		WRITEMSG
		if(vmo_opn_cls.amt<-2147000000) vmo_opn_cls.amt=0.0;
		if(vmo_opn_cls.tx_date<-2147000000) vmo_opn_cls.tx_date=0;
		if(vmo_opn_cls.trace_no<-2147000000) vmo_opn_cls.trace_no=0;
		pub_base_dic_show_str(f_ct_ind14,"ct_ind",vmo_opn_cls.ct_ind);
		pub_base_dic_show_str(f_wrk_sts15,"opn_cls_sts",vmo_opn_cls.wrk_sts);

		fprintf( fp, "%s|%d|%s|%.2lf|%s|%s|%ld|%s|%s|%s|%s|%s|%ld|%s|%s|\n",vmo_opn_cls.ac_no,vmo_opn_cls.ac_seqn,vmo_opn_cls.name,vmo_opn_cls.amt,vmo_opn_cls.acc_hrt,f_wrk_sts15,vmo_opn_cls.tx_date,vmo_opn_cls.tx_brno,f_ct_ind14,vmo_opn_cls.tel,vmo_opn_cls.chk,vmo_opn_cls.auth,vmo_opn_cls.trace_no,f_cif_type5,vmo_opn_cls.opn_br_no);
		ttlnum++;
/*****************
fprintf( fp,"~@客户帐号|序号|@户名|@科目号
|$金额|@状态|交易日期|@交易机构|@现转
|@操作员|@复核员|@授权人|流水号
|@客户类型|@开户行|\n" );



fprintf( fp, "%s|%d|%s|%s
|%.2lf|%s|%ld|%s|%s
|%s|%s|%s|%ld
|%s|%s|\n",
vmo_opn_cls.ac_no,vmo_opn_cls.ac_seqn,vmo_opn_cls.name,vmo_opn_cls.acc_hrt,
vmo_opn_cls.amt,f_wrk_sts15,vmo_opn_cls.tx_date,vmo_opn_cls.tx_brno,f_ct_ind14,
vmo_opn_cls.tel,vmo_opn_cls.chk,vmo_opn_cls.auth,vmo_opn_cls.trace_no,
f_cif_type5,vmo_opn_cls.opn_br_no);

********************/		
		
	}

	ret=Mo_opn_cls_Clo_Sel( );
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
