/*************************************************
* 文 件 名:  sp8205.c
* 功能描述： 贷款展期登记簿查询
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
#include "ln_expand_c.h"

int sp8205()
{
	struct ln_expand_c vln_expand;
	struct ln_expand_c f_ln_expand;
	char ac_no0[25];
	char f_exp_ind7[41];
	char f_acno0[25];
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

	get_zd_data("0380",f_acno0);
    pub_base_old_acno(f_acno0);
	get_zd_long("0510",&f_ln_expand.ac_seqn );
	get_zd_data("0910",f_ln_expand.exp_br_no );
	get_zd_double("0400",&f_ln_expand.exp_amt );
	get_zd_long("0290",&f_ln_expand.exp_beg_date );
	get_zd_long("0440",&f_ln_expand.exp_mtr_date );
	get_zd_data("0700",f_ln_expand.exp_ind );

	/**组成查询条件**/
	pub_base_strpack( f_acno0 );
	if( strlen(f_acno0) )
	{
		if ( pub_base_acno_acid(&f_ln_expand.ac_id,f_acno0) )
		{
		goto ErrExit;
		}
		sprintf( tmpstr," ac_id=%ld and",f_ln_expand.ac_id );
		strcat( wherelist,tmpstr );
	}
	if( f_ln_expand.ac_seqn )
	{
		sprintf( tmpstr," ac_seqn=%d and",f_ln_expand.ac_seqn );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_ln_expand.exp_br_no );
	if( strlen(f_ln_expand.exp_br_no) )
	{
		sprintf( tmpstr," exp_br_no='%s' and",f_ln_expand.exp_br_no );
		strcat( wherelist,tmpstr );
	}
	if( f_ln_expand.exp_amt )
	{
		sprintf( tmpstr," exp_amt=%lf and",f_ln_expand.exp_amt );
		strcat( wherelist,tmpstr );
	}
	if( f_ln_expand.exp_beg_date )
	{
		sprintf( tmpstr," exp_beg_date=%ld and",f_ln_expand.exp_beg_date );
		strcat( wherelist,tmpstr );
	}
	if( f_ln_expand.exp_mtr_date )
	{
		sprintf( tmpstr," exp_mtr_date=%ld and",f_ln_expand.exp_mtr_date );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_ln_expand.exp_ind );
	if( strlen(f_ln_expand.exp_ind) )
	{
		sprintf( tmpstr," exp_ind='%s' and",f_ln_expand.exp_ind );
		strcat( wherelist,tmpstr );
	}

	strcpy( tablelist,"ln_expand" );

	/**组成查询**/
	strcat(wherelist," 1=1 ORDER BY ac_id,ac_seqn,exp_ind ") ;

	vtcp_log( "WHERE [%s]",wherelist );
	strcpy( fieldlist,"ac_id,ac_seqn,exp_br_no,exp_amt,exp_rate,exp_beg_date,exp_mtr_date,exp_ind,rgst_date,tel,trace_no");

	ret=Ln_expand_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

		ttlnum=0;

	while(1)
	{
		ret=Ln_expand_Fet_Sel( &vln_expand, g_pub_tx.reply );
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
			fprintf( fp,"~账    号|序号|@户名|$展期金额|展期利率|展期起始日|展期到期日|@展期次数|登记日期|@展期机构|@操作员|@流水号|\n" );
		}

		/* 取贷款户名称 */
		ret = Ln_mst_Sel( g_pub_tx.reply , &g_ln_mst , " ac_id='%ld' and ac_seqn='%d' " , vln_expand.ac_id,vln_expand.ac_seqn );		
		if( ret )
		{
			sprintf(acErrMsg,"读取贷款主文件信息error");
			WRITEMSG
			strcpy( g_pub_tx.reply,"S049" );
			goto ErrExit;
		}
		
		/*pub_pubdb_prt_fld( fp,tablelist,fieldlist ); */

		pub_base_acid_acno( vln_expand.ac_id,vln_expand.ac_seqn,ac_no0 );
		if(vln_expand.exp_amt<-2147000000) vln_expand.exp_amt=0.0;
		if(vln_expand.exp_rate<-2147000000) vln_expand.exp_rate=0.0;
		if(vln_expand.exp_beg_date<-2147000000) vln_expand.exp_beg_date=0;
		if(vln_expand.exp_mtr_date<-2147000000) vln_expand.exp_mtr_date=0;
		pub_base_dic_show_str(f_exp_ind7,"exp_ind",vln_expand.exp_ind);
		if(vln_expand.rgst_date<-2147000000) vln_expand.rgst_date=0;

		fprintf( fp, "%s|%d|%s|%.2lf|%.5lf|%ld|%ld|%s|%ld|%s|%s|%s|\n",ac_no0,vln_expand.ac_seqn,g_ln_mst.name,vln_expand.exp_amt,vln_expand.exp_rate,vln_expand.exp_beg_date,vln_expand.exp_mtr_date,f_exp_ind7,vln_expand.rgst_date,vln_expand.exp_br_no,vln_expand.tel,vln_expand.trace_no);
		ttlnum++;
	}

	ret=Ln_expand_Clo_Sel( );
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
