/*************************************************
* 文 件 名:  sp8805.c
* 功能描述： 他日交易查询
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
#include "trace_log_rz_c.h"

int sp8805()
{
	struct trace_log_rz_c vtrace_log_rz;
	struct trace_log_rz_c f_trace_log_rz;
	struct trace_log_rz_c f_trace_log_rza;
	char f_note_type9[41];
	char f_add_ind18[41];
	char f_ct_ind18[41];
	char f_sts18[41];
	char cur_no[21];
	char log_sts[2];
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
		
	get_zd_long("0470",&f_trace_log_rz.tx_date );	
	get_zd_data("0710",log_sts );	
	get_zd_long("0290",&f_trace_log_rz.trace_no );
	get_zd_data("0910",f_trace_log_rz.tx_br_no );
	get_zd_data("0380",f_trace_log_rz.ac_no );
	get_zd_long("0510",&f_trace_log_rz.ac_seqn );
	get_zd_data("0240",f_trace_log_rz.note_type );
	get_zd_data("0590",f_trace_log_rz.note_no );
	get_zd_data("0700",f_trace_log_rz.add_ind );
	get_zd_double("1001",&f_trace_log_rz.amt );
	get_zd_double("1002",&f_trace_log_rza.amt );
	get_zd_data("0660",f_trace_log_rz.ct_ind );
	get_zd_data("0630",f_trace_log_rz.brf );
	get_zd_data("0640",f_trace_log_rz.tel );
	get_zd_data("0650",f_trace_log_rz.chk );
	get_zd_data("0870",f_trace_log_rz.aut );

	/**组成查询条件**/
	if( log_sts[0]=='0' )
	{
		sprintf( tmpstr," sts='%s' and",log_sts );
		strcat( wherelist,tmpstr );
	}else if( log_sts[0]=='1' ){
		sprintf( tmpstr," sts='%s' and",log_sts );
		strcat( wherelist,tmpstr );
	}else if( log_sts[0]=='2' ){
		strcpy( tmpstr," sts in ('0' ,'1')  and" );
		strcat( wherelist,tmpstr );
	}
	/*
	else{	
		sprintf(acErrMsg,"流水状态标志错误,不存在输入的标志! [%s]",log_sts);
		WRITEMSG
		strcpy( g_pub_tx.reply, "L187" );
		goto ErrExit;		
	}	
	*/
	if( f_trace_log_rz.tx_date )
	{
		sprintf( tmpstr," tx_date=%ld and",f_trace_log_rz.tx_date );
		strcat( wherelist,tmpstr );
	}		
	if( f_trace_log_rz.trace_no )
	{
		sprintf( tmpstr," trace_no=%ld and",f_trace_log_rz.trace_no );
		strcat( wherelist,tmpstr );
	}
	
	/*－－－－－－－－只允许查本网点的流水－－－－－dgf－－－－－－*/
	memset( tmpstr, 0x0, sizeof(tmpstr) );
	sprintf( tmpstr," tx_br_no='%s' and", g_pub_tx.tx_br_no );
	strcat( wherelist,tmpstr );
	
	/*－－－－－－只允许查本网点的话，则输入交易机构就无意义－dgf－*
	pub_base_strpack( f_trace_log_rz.tx_br_no );
	if( strlen(f_trace_log_rz.tx_br_no) )
	{
		sprintf( tmpstr," tx_br_no='%s' and",f_trace_log_rz.tx_br_no );
		strcat( wherelist,tmpstr );
	}
	*－－－－－－－－只允许查本网点的流水－－－－－－－－－－－*/	
	
	pub_base_strpack( f_trace_log_rz.ac_no );
	if( strlen(f_trace_log_rz.ac_no) )
	{
		sprintf( tmpstr," ac_no='%s' and",f_trace_log_rz.ac_no );
		strcat( wherelist,tmpstr );
	}
	if( f_trace_log_rz.ac_seqn )
	{
		sprintf( tmpstr," ac_seqn=%d and",f_trace_log_rz.ac_seqn );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_trace_log_rz.note_type );
	if( strlen(f_trace_log_rz.note_type) )
	{
		sprintf( tmpstr," note_type='%s' and",f_trace_log_rz.note_type );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_trace_log_rz.note_no );
	if( strlen(f_trace_log_rz.note_no) )
	{
		sprintf( tmpstr," note_no='%s' and",f_trace_log_rz.note_no );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_trace_log_rz.add_ind );
	if( strlen(f_trace_log_rz.add_ind) )
	{
		sprintf( tmpstr," add_ind='%s' and",f_trace_log_rz.add_ind );
		strcat( wherelist,tmpstr );
	}
	if( pub_base_CompDblVal(f_trace_log_rz.amt,0.00) )
	{
		sprintf( tmpstr," amt>=%lf and",f_trace_log_rz.amt );
		strcat( wherelist,tmpstr );
	}
	if( pub_base_CompDblVal(f_trace_log_rza.amt,0.00) )
	{
		sprintf( tmpstr," amt<=%lf and",f_trace_log_rza.amt );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_trace_log_rz.ct_ind );
	if( strlen(f_trace_log_rz.ct_ind) )
	{
		sprintf( tmpstr," ct_ind='%s' and",f_trace_log_rz.ct_ind );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_trace_log_rz.brf );
	if( strlen(f_trace_log_rz.brf) )
	{
		sprintf( tmpstr," brf like '%%%%%s%%%%' and",f_trace_log_rz.brf );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_trace_log_rz.tel );
	if( strlen(f_trace_log_rz.tel) )
	{
		sprintf( tmpstr," tel='%s' and",f_trace_log_rz.tel );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_trace_log_rz.chk );
	if( strlen(f_trace_log_rz.chk) )
	{
		sprintf( tmpstr," chk='%s' and",f_trace_log_rz.chk );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_trace_log_rz.aut );
	if( strlen(f_trace_log_rz.aut) )
	{
		sprintf( tmpstr," aut='%s' and",f_trace_log_rz.aut );
		strcat( wherelist,tmpstr );
	}

	strcpy( tablelist,"trace_log_rz" );

	/**组成查询**/
	strcat(wherelist," no_show!='1' order by tx_date,trace_no,trace_cnt ") ;

	vtcp_log( "WHERE [%s]",wherelist );
	strcpy( fieldlist,"trace_no,trace_cnt,tx_time,tx_br_no,opn_br_no,svc_ind,cur_no,ac_no,ac_seqn,note_type,note_no,add_ind,amt,ct_ind,brf,tel,chk,aut,sts");

	ret=Trace_log_rz_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

		ttlnum=0;

	while(1)
	{
		ret=Trace_log_rz_Fet_Sel( &vtrace_log_rz, g_pub_tx.reply );
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
			fprintf( fp,"~交易日期|@状态|流水号|笔次|@账号|序号|@增减|$发生额|@现转|@币种|@摘要|@凭证种类|@凭证号|@开户行|交易时间|@交易行|@操作员|@复核员|@授权人|\n" );
		}

		/*pub_pubdb_prt_fld( fp,tablelist,fieldlist ); */

		if(vtrace_log_rz.trace_no<-2147000000) vtrace_log_rz.trace_no=0;
		if(vtrace_log_rz.trace_cnt<-2147000000) vtrace_log_rz.trace_cnt=0;
		if(vtrace_log_rz.tx_time<-2147000000) vtrace_log_rz.tx_time=0;
		pub_base_dic_show_str(f_note_type9,"note_type",vtrace_log_rz.note_type);
		if(vtrace_log_rz.amt<-2147000000) vtrace_log_rz.amt=0.0;
		pub_base_dic_show_str(f_add_ind18,"add_ind",vtrace_log_rz.add_ind);
		/************
		if(vtrace_log_rz.add_ind[0]=='0')
		{
			strcpy(f_add_ind18,"借");
		}
		else if(vtrace_log_rz.add_ind[0]=='1')
		{
			strcpy(f_add_ind18,"贷");
		}
		*****************/
		pub_base_dic_show_str(f_ct_ind18,"ct_ind",vtrace_log_rz.ct_ind);
		pub_base_dic_show_str(f_sts18,"log_sts",vtrace_log_rz.sts);
		pub_base_dic_show_str(cur_no,"cur_name",vtrace_log_rz.cur_no);

		fprintf( fp, "%ld|%s|%ld|%ld|%s|%d|%s|%.2lf|%s|%s|%s|%s|%s|%s|%ld|%s|%s|%s|%s|\n",vtrace_log_rz.tx_date,f_sts18,vtrace_log_rz.trace_no,vtrace_log_rz.trace_cnt,vtrace_log_rz.ac_no,vtrace_log_rz.ac_seqn,f_add_ind18,vtrace_log_rz.amt,f_ct_ind18,cur_no,vtrace_log_rz.brf,f_note_type9,vtrace_log_rz.note_no,vtrace_log_rz.opn_br_no,vtrace_log_rz.tx_time,vtrace_log_rz.tx_br_no,vtrace_log_rz.tel,vtrace_log_rz.chk,vtrace_log_rz.aut);
		ttlnum++;
	}

	ret=Trace_log_rz_Clo_Sel( );
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
