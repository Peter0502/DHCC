/*************************************************
* 文 件 名:  sp8118.c
* 功能描述： 子帐号明细帐查询
*
* 作    者:  mike
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
#include "sub_dd_mst_hst_c.h"

int sp8118()
{
	struct sub_dd_mst_hst_c vsub_dd_mst_hst;
	struct sub_dd_mst_hst_c f_sub_dd_mst_hst;
	char f_add_ind6[41];
	char f_ct_ind7[41];
		char filename[100];
		char wherelist[1024];  /**查询条件**/
		char tmpstr[512];
		char tmpmo[512];
		char fieldlist[1024];  /**要查询的字段列表**/
		char tablelist[128];   /**要查询的表名**/
		char titstr[1024];
		char tm_ac_no[21];
		int ttlnum=0;			/**读取的总条数**/
		int i=0;
		int ret=0;
		FILE *fp;

	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset( wherelist,0,sizeof(wherelist) );
	memset( fieldlist,0,sizeof(fieldlist) );
	memset( tablelist,0,sizeof(tablelist) );

	pub_base_sysinit();

	get_zd_long("0290",&f_sub_dd_mst_hst.trace_no );
	get_zd_data("0910",f_sub_dd_mst_hst.opn_br_no );
	get_zd_data("0640",f_sub_dd_mst_hst.tx_br_no );
	get_zd_data("0380",tm_ac_no );
	get_zd_data("0370",f_sub_dd_mst_hst.sub_ac_no );
	get_zd_data("0670",f_sub_dd_mst_hst.add_ind );
	get_zd_data("0700",f_sub_dd_mst_hst.ct_ind );
	get_zd_double("0400",&f_sub_dd_mst_hst.tx_amt );
	get_zd_long("0440",&f_sub_dd_mst_hst.tx_date );
	get_zd_data("0630",f_sub_dd_mst_hst.brf );
	get_zd_data("0650",f_sub_dd_mst_hst.tel );

	/**组成查询条件**/
	if( f_sub_dd_mst_hst.trace_no )
	{
		sprintf( tmpstr," trace_no=%ld and",f_sub_dd_mst_hst.trace_no );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_sub_dd_mst_hst.opn_br_no );
	if( strlen(f_sub_dd_mst_hst.opn_br_no) )
	{
		sprintf( tmpstr," opn_br_no='%s' and",f_sub_dd_mst_hst.opn_br_no );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_sub_dd_mst_hst.tx_br_no );
	if( strlen(f_sub_dd_mst_hst.tx_br_no) )
	{
		sprintf( tmpstr," tx_br_no='%s' and",f_sub_dd_mst_hst.tx_br_no );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack(tm_ac_no );
	if( strlen(tm_ac_no) )
	{
		pub_base_old_acno( tm_ac_no ); 
		sprintf( tmpstr," ac_no='%s' and",tm_ac_no );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_sub_dd_mst_hst.sub_ac_no );
	if( strlen(f_sub_dd_mst_hst.sub_ac_no) )
	{
		sprintf( tmpstr," sub_ac_no='%s' and",f_sub_dd_mst_hst.sub_ac_no );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_sub_dd_mst_hst.add_ind );
	if( strlen(f_sub_dd_mst_hst.add_ind) )
	{
		sprintf( tmpstr," add_ind='%s' and",f_sub_dd_mst_hst.add_ind );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_sub_dd_mst_hst.ct_ind );
	if( strlen(f_sub_dd_mst_hst.ct_ind) )
	{
		sprintf( tmpstr," ct_ind='%s' and",f_sub_dd_mst_hst.ct_ind );
		strcat( wherelist,tmpstr );
	}
	if( f_sub_dd_mst_hst.tx_amt )
	{
		sprintf( tmpstr," tx_amt=%lf and",f_sub_dd_mst_hst.tx_amt );
		strcat( wherelist,tmpstr );
	}
	if( f_sub_dd_mst_hst.tx_date )
	{
		sprintf( tmpstr," tx_date=%ld and",f_sub_dd_mst_hst.tx_date );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_sub_dd_mst_hst.brf );
	if( strlen(f_sub_dd_mst_hst.brf) )
	{
		sprintf( tmpstr," brf='%s' and",f_sub_dd_mst_hst.brf );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_sub_dd_mst_hst.tel );
	if( strlen(f_sub_dd_mst_hst.tel) )
	{
		sprintf( tmpstr," tel='%s' and",f_sub_dd_mst_hst.tel );
		strcat( wherelist,tmpstr );
	}

	strcpy( tablelist,"sub_dd_mst_hst" );

	/**组成查询**/
	strcat(wherelist," 1=1") ;

	vtcp_log( "WHERE [%s]",wherelist );
	strcpy( fieldlist,"trace_no,trace_cnt,opn_br_no,tx_br_no,ac_no,sub_ac_no,add_ind,ct_ind,tx_amt,bal,intst_acm,tx_date,tx_time,hst_cnt,brf,tel,chk,auth");

	ret=Sub_dd_mst_hst_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

		ttlnum=0;

	while(1)
	{
		ret=Sub_dd_mst_hst_Fet_Sel( &vsub_dd_mst_hst, g_pub_tx.reply );
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
			fprintf( fp,"~流水号|流水笔次|@开户机构|@交易机构|@母帐号|@子帐号|@增减标志|@现金转账标志|$交易金额|$子帐号余额|$利息积数|交易日期|交易时间|明细笔数|@摘要|@操作员|@复核员|@授权人|\n" );
		}

		/*pub_pubdb_prt_fld( fp,tablelist,fieldlist ); */

		if(vsub_dd_mst_hst.trace_no<-2147000000) vsub_dd_mst_hst.trace_no=0;
		if(vsub_dd_mst_hst.trace_cnt<-2147000000) vsub_dd_mst_hst.trace_cnt=0;
		pub_base_dic_show_str(f_add_ind6,"add_ind",vsub_dd_mst_hst.add_ind);
		pub_base_dic_show_str(f_ct_ind7,"ct_ind",vsub_dd_mst_hst.ct_ind);
		if(vsub_dd_mst_hst.tx_amt<-2147000000) vsub_dd_mst_hst.tx_amt=0.0;
		if(vsub_dd_mst_hst.bal<-2147000000) vsub_dd_mst_hst.bal=0.0;
		if(vsub_dd_mst_hst.intst_acm<-2147000000) vsub_dd_mst_hst.intst_acm=0.0;
		if(vsub_dd_mst_hst.tx_date<-2147000000) vsub_dd_mst_hst.tx_date=0;
		if(vsub_dd_mst_hst.tx_time<-2147000000) vsub_dd_mst_hst.tx_time=0;
		if(vsub_dd_mst_hst.hst_cnt<-2147000000) vsub_dd_mst_hst.hst_cnt=0;

		fprintf( fp, "%ld|%ld|%s|%s|%s|%s|%s|%s|%.2lf|%.2lf|%.2lf|%ld|%ld|%ld|%s|%s|%s|%s|\n",vsub_dd_mst_hst.trace_no,vsub_dd_mst_hst.trace_cnt,vsub_dd_mst_hst.opn_br_no,vsub_dd_mst_hst.tx_br_no,vsub_dd_mst_hst.ac_no,vsub_dd_mst_hst.sub_ac_no,f_add_ind6,f_ct_ind7,vsub_dd_mst_hst.tx_amt,vsub_dd_mst_hst.bal,vsub_dd_mst_hst.intst_acm,vsub_dd_mst_hst.tx_date,vsub_dd_mst_hst.tx_time,vsub_dd_mst_hst.hst_cnt,vsub_dd_mst_hst.brf,vsub_dd_mst_hst.tel,vsub_dd_mst_hst.chk,vsub_dd_mst_hst.auth);
		ttlnum++;
	}

	ret=Sub_dd_mst_hst_Clo_Sel( );
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
