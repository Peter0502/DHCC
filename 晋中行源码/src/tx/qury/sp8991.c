/*************************************************
* 文 件 名:  sp8991.c
* 功能描述： 活期账户明细查询
*
* 作    者:   LinGuoTao
* 完成日期：  20110802
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
#include "dd_mst_hst_c.h"
#include "mdm_ac_rel_c.h"

int sp8991()
{
	struct dd_mst_hst_c vdd_mst_hst;
	struct dd_mst_hst_c f_dd_mst_hst;
	struct dd_mst_hst_c f_dd_mst_hsta;
	struct mdm_ac_rel_c v_mdm_ac_rel;
	struct mdm_ac_rel_c s_mdm_ac_rel;
	struct dd_mst_c vdd_mst;
	
	char add_ind[3];
	char ct_ind[5];
	char ac_no3[25];
	char f_note_type9[41];
	char ac_no[25];
	char filename[100];
	char wherelist[1024];  /**查询条件**/
	char wherelist1[1024];  /**查询条件**/
	char tmpstr[512];
	char tmpmo[512];
	char fieldlist[1024];  /**要查询的字段列表**/
	char tablelist[128];   /**要查询的表名**/
	char titstr[1024];
	int ttlnum=0;         /**读取的总条数**/
	int i=0;
	int ret=0;
	double j_tx_amt=0.00;
	double d_tx_amt=0.00;
	double sumBal=0.00;   /**集团账户总余额**/
	FILE *fp;
	char tmp_date[9];
	char tmp_time[7];
	int fuk=0;

	memset(tmp_date,0x00,sizeof(tmp_date));
	memset(tmp_time,0x00,sizeof(tmp_time));
	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset( wherelist,0,sizeof(wherelist) );
	memset( wherelist1,0,sizeof(wherelist1) );
	memset( fieldlist,0,sizeof(fieldlist) );
	memset( tablelist,0,sizeof(tablelist) );
	memset( &vdd_mst_hst,0,sizeof(vdd_mst_hst));
	memset( &vdd_mst,0,sizeof(vdd_mst));
	memset( &f_dd_mst_hst,0,sizeof(f_dd_mst_hst));
	memset( &f_dd_mst_hsta,0,sizeof(f_dd_mst_hsta));
	memset( &v_mdm_ac_rel , 0x00 , sizeof( struct mdm_ac_rel_c ) );
	memset( &s_mdm_ac_rel , 0x00 , sizeof( struct mdm_ac_rel_c ) );

	vtcp_log( "0700===f_dd_mst_hst.add_ind [%s]",f_dd_mst_hst.add_ind );
	pub_base_sysinit();
	get_zd_data("0700",f_dd_mst_hst.add_ind );
	vtcp_log( "0700===f_dd_mst_hst.add_ind [%s]",f_dd_mst_hst.add_ind );
	get_zd_data("0380",ac_no);
	pub_base_old_acno(ac_no);
	vtcp_log( "0700===f_dd_mst_hst.add_ind [%s]",f_dd_mst_hst.add_ind );
	get_zd_long("0510",&f_dd_mst_hst.ac_seqn );
	get_zd_data("0910",f_dd_mst_hst.opn_br_no );
	get_zd_data("0700",f_dd_mst_hst.add_ind );
	get_zd_data("0660",f_dd_mst_hst.ct_ind );
	get_zd_long("0290",&f_dd_mst_hst.tx_date );     /**起始日期**/
	get_zd_long("0440",&f_dd_mst_hsta.tx_date );    /**终止日期**/
	get_zd_long("0050",&g_pub_tx.tx_date );         /**当前交易日期**/

	vtcp_log( "0700===f_dd_mst_hst.add_ind [%s]",f_dd_mst_hst.add_ind );

	pub_base_strpack( ac_no );

	/**交易日当前账户总余额合计**/
	ret=Mdm_ac_rel_Dec_Sel( g_pub_tx.reply," ac_no like '%s%%' ",ac_no );
	ERR_DEAL
	while(1)
	{
		ret=Mdm_ac_rel_Fet_Sel( &s_mdm_ac_rel,g_pub_tx.reply );

		if( ret==100 ) break;
		ERR_DEAL
		
		ret=Dd_mst_Dec_Sel( g_pub_tx.reply," ac_id=%d ",s_mdm_ac_rel.ac_id  );
		ERR_DEAL
		while(1)
		{
			ret=Dd_mst_Fet_Sel( &vdd_mst, g_pub_tx.reply );
			if( ret==100 ) break;
			sumBal+=vdd_mst.bal;
		}
		ret=Dd_mst_Clo_Sel( );
		ERR_DEAL
	}
	ret=Mdm_ac_rel_Clo_Sel( );	

	/**组成查询条件**/
	ttlnum=0;

	memset( &v_mdm_ac_rel , 0x00 , sizeof( struct mdm_ac_rel_c ) );
	memset( wherelist,0,sizeof(wherelist) );
	memset( wherelist1,0,sizeof(wherelist1) );

	fuk++;
	
	vtcp_log( "%s,%d,次数为 [%d]",__FILE__,__LINE__,fuk );
	vtcp_log( "%s,%d,次数为2 [%d]",__FILE__,__LINE__,fuk );
	
	sprintf( tmpstr," ac_id in ( select ac_id from mdm_ac_rel where ac_no like '%s%%%%' ) and ",ac_no );
	strcat( wherelist,tmpstr );
	strcat( wherelist1,tmpstr );
	
	if( f_dd_mst_hst.ac_seqn )
	{
		sprintf( tmpstr," ac_seqn=%d and",f_dd_mst_hst.ac_seqn );
		strcat( wherelist,tmpstr );
		strcat( wherelist1,tmpstr );
	}
	pub_base_strpack( f_dd_mst_hst.opn_br_no );

	pub_base_strpack( f_dd_mst_hst.add_ind );
	if( strlen(f_dd_mst_hst.add_ind) )
	{
		if( f_dd_mst_hst.add_ind[0]=='0' || f_dd_mst_hst.add_ind[0]=='1' ){
			vtcp_log( "f_dd_mst_hst.add_ind [%s]",f_dd_mst_hst.add_ind );
			sprintf( tmpstr," add_ind='%s' and",f_dd_mst_hst.add_ind );
			strcat( wherelist,tmpstr );
			strcat( wherelist1,tmpstr );
		}
	}
	pub_base_strpack( f_dd_mst_hst.ct_ind );
	if( strlen(f_dd_mst_hst.ct_ind) )
	{
		sprintf( tmpstr," ct_ind='%s' and",f_dd_mst_hst.ct_ind );
		strcat( wherelist,tmpstr );
		strcat( wherelist1,tmpstr );
	}

	if( f_dd_mst_hst.tx_date )
	{
		sprintf( tmpstr," tx_date>=%ld and",f_dd_mst_hst.tx_date );
		strcat( wherelist,tmpstr );
	}
	if( f_dd_mst_hsta.tx_date )
	{
		sprintf( tmpstr," tx_date<=%ld and",f_dd_mst_hsta.tx_date );
		strcat( wherelist,tmpstr );
	}

	/*终止日不是当前日*/
	if( (f_dd_mst_hsta.tx_date < g_pub_tx.tx_date) && f_dd_mst_hsta.tx_date != 0)
	{
	vtcp_log( "!!!!!终止日不是当前日" );
		if( f_dd_mst_hst.tx_date )
		{
			sprintf( tmpstr," tx_date>%ld and",f_dd_mst_hsta.tx_date );
			strcat( wherelist1,tmpstr );
		}
		if( f_dd_mst_hsta.tx_date )
		{
			sprintf( tmpstr," tx_date<=%ld and",g_pub_tx.tx_date );
			strcat( wherelist1,tmpstr );
		}
	}

	strcpy( tablelist,"dd_mst_hst" );

	/**组成查询**/
	strcat(wherelist," 1=1 ORDER BY tx_date,trace_no,trace_cnt");
	strcat(wherelist1," 1=1 ORDER BY tx_date,trace_no,trace_cnt");
	
	vtcp_log( "WHERE [%s]",wherelist );
	vtcp_log( "WHERE1 [%s]",wherelist1 );
	strcpy( fieldlist,"trace_no,trace_cnt,opn_br_no,ac_id,ac_seqn,add_ind,ct_ind,tx_amt,bal,note_type,note_no,tx_date,tx_time,brf,hst_cnt,tel,chk,auth");


	/*终止日不是交易日情况下，终止日账户总金额合计*/
	if( (f_dd_mst_hsta.tx_date < g_pub_tx.tx_date) && f_dd_mst_hsta.tx_date != 0)
	{
		ret=Dd_mst_hst_Dec_Sel( g_pub_tx.reply,wherelist1 );
		ERR_DEAL
	
		while(1)
		{
			memset( &vdd_mst_hst,0,sizeof(vdd_mst_hst));
			ret=Dd_mst_hst_Fet_Sel( &vdd_mst_hst, g_pub_tx.reply );
			if( ret==100 ) break;
			ERR_DEAL

		if(vdd_mst_hst.trace_no<-2147000000) vdd_mst_hst.trace_no=0;
		if(vdd_mst_hst.trace_cnt<-2147000000) vdd_mst_hst.trace_cnt=0;
		pub_base_acid_acno( vdd_mst_hst.ac_id,vdd_mst_hst.ac_seqn,ac_no3 );
		if(vdd_mst_hst.tx_amt<-2147000000) vdd_mst_hst.tx_amt=0.0;
			if(vdd_mst_hst.bal<-2147000000) vdd_mst_hst.bal=0.0;
			pub_base_dic_show_str(f_note_type9,"note_type",vdd_mst_hst.note_type);
			if(vdd_mst_hst.tx_date<-2147000000) vdd_mst_hst.tx_date=0;
			if(vdd_mst_hst.tx_time<-2147000000) vdd_mst_hst.tx_time=0;
			if(vdd_mst_hst.hst_cnt<-2147000000) vdd_mst_hst.hst_cnt=0;
	
			if(vdd_mst_hst.add_ind[0]=='0')		 
			{
				strcpy(add_ind,"借");
				sumBal=sumBal+vdd_mst_hst.tx_amt;
			}
			else if(vdd_mst_hst.add_ind[0]=='1') 
			{
				strcpy(add_ind,"贷");
				sumBal=sumBal-vdd_mst_hst.tx_amt;
			}
		}
		ret=Dd_mst_hst_Clo_Sel( );
		ERR_DEAL
	}

	/*查询条件下，账户总金额合计*/
	ret=Dd_mst_hst_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

	while(1)
	{
		memset( &vdd_mst_hst,0,sizeof(vdd_mst_hst));
		ret=Dd_mst_hst_Fet_Sel( &vdd_mst_hst, g_pub_tx.reply );
		if( ret==100 ) break;
		ERR_DEAL

		if(vdd_mst_hst.trace_no<-2147000000) vdd_mst_hst.trace_no=0;
		if(vdd_mst_hst.trace_cnt<-2147000000) vdd_mst_hst.trace_cnt=0;
		pub_base_acid_acno( vdd_mst_hst.ac_id,vdd_mst_hst.ac_seqn,ac_no3 );
		if(vdd_mst_hst.tx_amt<-2147000000) vdd_mst_hst.tx_amt=0.0;
		if(vdd_mst_hst.bal<-2147000000) vdd_mst_hst.bal=0.0;
		pub_base_dic_show_str(f_note_type9,"note_type",vdd_mst_hst.note_type);
		if(vdd_mst_hst.tx_date<-2147000000) vdd_mst_hst.tx_date=0;
		if(vdd_mst_hst.tx_time<-2147000000) vdd_mst_hst.tx_time=0;
		if(vdd_mst_hst.hst_cnt<-2147000000) vdd_mst_hst.hst_cnt=0;

		if(vdd_mst_hst.add_ind[0]=='0')		 
		{
			strcpy(add_ind,"借");
			sumBal=sumBal+vdd_mst_hst.tx_amt;
		}
		else if(vdd_mst_hst.add_ind[0]=='1') 
		{
			strcpy(add_ind,"贷");
			sumBal=sumBal-vdd_mst_hst.tx_amt;
		}
	}
	ret=Dd_mst_hst_Clo_Sel( );
	ERR_DEAL

	/*查询条件下集团账户明细合计*/
	ret=Dd_mst_hst_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

	while(1)
	{
		memset( &vdd_mst_hst,0,sizeof(vdd_mst_hst));
		ret=Dd_mst_hst_Fet_Sel( &vdd_mst_hst, g_pub_tx.reply );
		if( ret==100 ) break;
		ERR_DEAL

		if( !ttlnum )
		{
			pub_base_AllDwnFilName( filename );
			vtcp_log( "%s,%d,FILENAME [%s]",__FILE__,__LINE__,filename );
			fp = fopen( filename,"w" );
			if( fp==NULL )
			{
				sprintf(acErrMsg," open file [%s] error ",filename );
				WRITEMSG
				strcpy( g_pub_tx.reply,"S047" );
				goto ErrExit;
			}

			/**显示列标题**/
			fprintf( fp,"~@交易日期|@借贷|$交易金额|$清算余额|@现转|@摘要|@凭证种类|@凭证号码|@操作员|交易时间|序号|\n" );
		}

		if(vdd_mst_hst.trace_no<-2147000000) vdd_mst_hst.trace_no=0;
		if(vdd_mst_hst.trace_cnt<-2147000000) vdd_mst_hst.trace_cnt=0;
		pub_base_acid_acno( vdd_mst_hst.ac_id,vdd_mst_hst.ac_seqn,ac_no3 );
		if(vdd_mst_hst.tx_amt<-2147000000) vdd_mst_hst.tx_amt=0.0;
		if(vdd_mst_hst.bal<-2147000000) vdd_mst_hst.bal=0.0;
		pub_base_dic_show_str(f_note_type9,"note_type",vdd_mst_hst.note_type);
		if(vdd_mst_hst.tx_date<-2147000000) vdd_mst_hst.tx_date=0;
		if(vdd_mst_hst.tx_time<-2147000000) vdd_mst_hst.tx_time=0;
		if(vdd_mst_hst.hst_cnt<-2147000000) vdd_mst_hst.hst_cnt=0;

		if(vdd_mst_hst.add_ind[0]=='0')		 
		{
			strcpy(add_ind,"借");
			sumBal=sumBal-vdd_mst_hst.tx_amt;
			j_tx_amt=j_tx_amt+vdd_mst_hst.tx_amt;
		}
		else if(vdd_mst_hst.add_ind[0]=='1') 
		{
			strcpy(add_ind,"贷");
			sumBal=sumBal+vdd_mst_hst.tx_amt;
			d_tx_amt=d_tx_amt+vdd_mst_hst.tx_amt;
		}

		pub_base_dic_show_str(ct_ind,"ct_ind",vdd_mst_hst.ct_ind);
		sprintf(tmp_date,"%ld",vdd_mst_hst.tx_date);
		sprintf(tmp_time,"%d",vdd_mst_hst.tx_time);

		fprintf(fp, "%s|%s|%.2lf|%.2lf|%s|%s|%s|%s|%s|%d|%d|\n",tmp_date,add_ind,vdd_mst_hst.tx_amt,sumBal,ct_ind,vdd_mst_hst.brf,f_note_type9,vdd_mst_hst.note_no,vdd_mst_hst.tel,vdd_mst_hst.tx_time,vdd_mst_hst.ac_seqn);
		ttlnum++;
	}
	ret=Dd_mst_hst_Clo_Sel( );
	ERR_DEAL
	if( !ttlnum )
	{
		strcpy( g_pub_tx.reply,"S049" );
		goto ErrExit;
	}
	else
	{
		/* fprintf( fp,"借方发生额合计:");*/
		fprintf( fp,"借方合计:|");
		fprintf( fp,"%.2lf||",j_tx_amt);
		/*  fprintf( fp,"贷方发生额合计:");*/
		fprintf( fp,"贷方合计:|");
		fprintf( fp,"%.2lf|\n",d_tx_amt);
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
