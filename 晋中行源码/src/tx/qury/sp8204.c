/*************************************************
* 文 件 名:  sp8204.c
* 功能描述： 贷款明细查询
*
* 作    者:  lance
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
#include "ln_mst_hst_c.h"

int sp8204()
{
	struct ln_mst_hst_c vln_mst_hst;
	struct ln_mst_hst_c f_ln_mst_hst;
	struct ln_mst_hst_c f_ln_mst_hsta;
	char ac_no3[25];
	char f_ln_tx_type5[41];
	char f_note_type11[41];
	char f_acno1[25];
	char f_add_ind[3];
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
	double	tol_amt_add=0.00;	
	double	tol_amt_rec=0.00;	
	double	tol_amt=0.00;
	double	tol_bal=0.00;
	char	add_inf[5];

	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset( wherelist,0,sizeof(wherelist) );
	memset( fieldlist,0,sizeof(fieldlist) );
	memset( tablelist,0,sizeof(tablelist) );

	pub_base_sysinit();

	get_zd_data("0910",f_ln_mst_hst.opn_br_no );
	get_zd_data("0380",f_acno1);
	get_zd_long("0510",&f_ln_mst_hst.ac_seqn );
	get_zd_data("0700",f_ln_mst_hst.ln_tx_type );
	get_zd_data("0660",f_ln_mst_hst.add_ind );
	get_zd_data("0670",f_ln_mst_hst.ct_ind );
	get_zd_double("1001",&f_ln_mst_hst.tx_amt );
	get_zd_double("1002",&f_ln_mst_hst.bal );
	get_zd_long("0290",&f_ln_mst_hst.tx_date );
	get_zd_long("0440",&f_ln_mst_hsta.tx_date );
	get_zd_data("0630",f_ln_mst_hst.brf );

	/**组成查询条件**/
	pub_base_strpack( f_ln_mst_hst.opn_br_no );
	if( strlen(f_ln_mst_hst.opn_br_no) )
	{
		sprintf( tmpstr," opn_br_no='%s' and",f_ln_mst_hst.opn_br_no );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_acno1 );
	pub_base_old_acno( f_acno1 );
	if( strlen(f_acno1) )
	{
		if ( pub_base_acno_acid(&f_ln_mst_hst.ac_id,f_acno1) )
		{
		goto ErrExit;
		}
		sprintf( tmpstr," ac_id=%ld and",f_ln_mst_hst.ac_id );
		strcat( wherelist,tmpstr );
	}
	if( f_ln_mst_hst.ac_seqn )
	{
		sprintf( tmpstr," ac_seqn=%d and",f_ln_mst_hst.ac_seqn );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_ln_mst_hst.ln_tx_type );
	if( strlen(f_ln_mst_hst.ln_tx_type) )
	{
		sprintf( tmpstr," ln_tx_type='%s' and",f_ln_mst_hst.ln_tx_type );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_ln_mst_hst.add_ind );
	if( strlen(f_ln_mst_hst.add_ind) )
	{
		sprintf( tmpstr," add_ind='%s' and",f_ln_mst_hst.add_ind );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_ln_mst_hst.ct_ind );
	if( strlen(f_ln_mst_hst.ct_ind) )
	{
		sprintf( tmpstr," ct_ind='%s' and",f_ln_mst_hst.ct_ind );
		strcat( wherelist,tmpstr );
	}
	if( f_ln_mst_hst.tx_amt )
	{
		sprintf( tmpstr," tx_amt=%lf and",f_ln_mst_hst.tx_amt );
		strcat( wherelist,tmpstr );
	}
	if( f_ln_mst_hst.bal )
	{
		sprintf( tmpstr," bal=%lf and",f_ln_mst_hst.bal );
		strcat( wherelist,tmpstr );
	}
	if( f_ln_mst_hst.tx_date )
	{
		sprintf( tmpstr," tx_date>=%ld and",f_ln_mst_hst.tx_date );
		strcat( wherelist,tmpstr );
	}
	if( f_ln_mst_hsta.tx_date )
	{
		sprintf( tmpstr," tx_date<=%ld and",f_ln_mst_hsta.tx_date );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_ln_mst_hst.brf );
	if( strlen(f_ln_mst_hst.brf) )
	{
		sprintf( tmpstr," brf like '%%%%%s%%%%' and",f_ln_mst_hst.brf);
		strcat( wherelist,tmpstr );
	}

	strcpy( tablelist,"ln_mst_hst" );

	/**组成查询**/
	strcat(wherelist," 1=1 order by ac_id , ac_seqn , tx_date , hst_cnt") ;

	vtcp_log( "查询条件是 WHERE [%s]",wherelist );
	strcpy( fieldlist,"trace_no,trace_cnt,opn_br_no,ac_id,ac_seqn,ln_tx_type,add_ind,tx_amt,bal,tx_date,tx_time,note_type,note_no,hst_cnt,brf,tel,chk,auth");

	ret=Ln_mst_hst_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

		ttlnum=0;

	while(1)
	{
		ret=Ln_mst_hst_Fet_Sel( &vln_mst_hst, g_pub_tx.reply );
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
			fprintf( fp,"~账    号|明细|交易日期|@开户行|户名|@交易类型|@借贷|$交易金额|$余额|@摘要|交易时间|@交易机构|@操作员|@复核员|@授权人|流水号|笔次|\n" );
		}
		
		/* 取贷款户名称 */
		ret = Ln_mst_Sel( g_pub_tx.reply , &g_ln_mst , " ac_id='%ld' and ac_seqn='%d' " , vln_mst_hst.ac_id,vln_mst_hst.ac_seqn );		
		if( ret )
		{
			sprintf(acErrMsg,"读取贷款主文件信息error");
			WRITEMSG
			strcpy( g_pub_tx.reply,"S049" );
			goto ErrExit;
		}

		/*pub_pubdb_prt_fld( fp,tablelist,fieldlist ); */

		if(vln_mst_hst.trace_no<-2147000000) vln_mst_hst.trace_no=0;
		if(vln_mst_hst.trace_cnt<-2147000000) vln_mst_hst.trace_cnt=0;
		pub_base_acid_acno( vln_mst_hst.ac_id,vln_mst_hst.ac_seqn,ac_no3 );
		pub_base_dic_show_str(f_ln_tx_type5,"ln_tx_type_hst",vln_mst_hst.ln_tx_type);
		if(vln_mst_hst.tx_amt<-2147000000) vln_mst_hst.tx_amt=0.0;
		if(vln_mst_hst.bal<-2147000000) vln_mst_hst.bal=0.0;
		if(vln_mst_hst.tx_date<-2147000000) vln_mst_hst.tx_date=0;
		if(vln_mst_hst.tx_time<-2147000000) vln_mst_hst.tx_time=0;
		pub_base_dic_show_str(f_note_type11,"note_type",vln_mst_hst.note_type);
		pub_base_dic_show_str(f_add_ind,"ln_add_ind",vln_mst_hst.add_ind);
		if(vln_mst_hst.ln_tx_type[0]=='6' || vln_mst_hst.ln_tx_type[0]=='7'|| vln_mst_hst.ln_tx_type[0]=='8'){
			pub_base_dic_show_str(f_add_ind,"bw_add_ind",vln_mst_hst.add_ind);
		}
		if(vln_mst_hst.hst_cnt<-2147000000) vln_mst_hst.hst_cnt=0;

		fprintf( fp, "%s|%ld|%ld|%s|%s|%s|%s|%.2lf|%.2lf|%s|%ld|%s|%s|%s|%s|%ld|%ld|\n",ac_no3,vln_mst_hst.hst_cnt,vln_mst_hst.tx_date,vln_mst_hst.opn_br_no,g_ln_mst.name,f_ln_tx_type5,f_add_ind,vln_mst_hst.tx_amt,vln_mst_hst.bal,vln_mst_hst.brf,vln_mst_hst.tx_time,vln_mst_hst.tx_br_no,vln_mst_hst.tel,vln_mst_hst.chk,vln_mst_hst.auth,vln_mst_hst.trace_no,vln_mst_hst.trace_cnt);
		
		if(vln_mst_hst.add_ind[0]=='0')	/*减少*/
		{
			tol_amt_rec+=vln_mst_hst.tx_amt;
		}else{				/*增加*/
		
			tol_amt_add+=vln_mst_hst.tx_amt;
		}
		tol_bal+=vln_mst_hst.bal;
		
		ttlnum++;
	}

	ret=Ln_mst_hst_Clo_Sel( );
	ERR_DEAL

	if( !ttlnum )
	{
		strcpy( g_pub_tx.reply,"S049" );
		goto ErrExit;
	}
	else
	{
		/*****
		fprintf( fp, "%s|%ld|%ld|%s|%s|%s|%s|%.2lf|%.2lf|%s|%ld|%s|%s|%s|%s|%ld|%ld|\n",ac_no3,g_ln_mst.name,vln_mst_hst.hst_cnt,f_ln_tx_type5,f_add_ind,vln_mst_hst.tx_amt,vln_mst_hst.bal,vln_mst_hst.brf,vln_mst_hst.tx_date,vln_mst_hst.tx_time,vln_mst_hst.tx_br_no,vln_mst_hst.opn_br_no,vln_mst_hst.tel,vln_mst_hst.chk,vln_mst_hst.auth,vln_mst_hst.trace_no,vln_mst_hst.trace_cnt);
		******/
		if( pub_base_CompDblVal( tol_amt_add,tol_amt_rec)>=0)
		{
			strcpy( add_inf,"增加");
			tol_amt=tol_amt_add-tol_amt_rec;
		}else{
			strcpy( add_inf,"减少");
			tol_amt=tol_amt_rec-tol_amt_add;
		}
		
		/*fprintf( fp, "%s|%ld|%ld|%s|%s|%s|%s|%.2lf|%.2lf|%s|%ld|%s|%s|%s|%s|%ld|%ld|\n","总    计:",0,g_pub_tx.tx_date,"","","",add_inf,tol_amt,tol_bal,"",g_pub_tx.tx_time,"","","","",0,0);	*/	
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
