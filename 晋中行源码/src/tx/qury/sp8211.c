/*************************************************
* 文 件 名:  sp8211.c
* 功能描述： 按揭欠款查询
*
* 作    者:  lance
* 完成日期： 2003年9月9日
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
#include "ln_mst_c.h"
#include "ln_lo_c.h"

int sp8211()
{
	struct ln_lo_c vln_lo;
	struct ln_lo_c f_ln_lo;
	struct ln_lo_c f_ln_loa;
	struct ln_mst_c  g_ln_mst;
	char ac_no0[25];
	char f_pay_type1[41];
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
	get_zd_data("0670",f_ln_lo.pay_type );
	get_zd_long("0290",&f_ln_lo.over_date );
	get_zd_long("0440",&f_ln_loa.over_date );
	

	/**组成查询条件**/
  pub_base_strpack( f_acno0 );
	if( strlen(f_acno0) )
	{
		if ( pub_base_acno_acid(&f_ln_lo.ac_id,f_acno0) )
		{
		goto ErrExit;
		}
		sprintf( tmpstr," ac_id=%ld and",f_ln_lo.ac_id );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_ln_lo.pay_type );
	if( strlen(f_ln_lo.pay_type) )
	{
		sprintf( tmpstr," pay_type='%s' and",f_ln_lo.pay_type );
		strcat( wherelist,tmpstr );
	}
	if( f_ln_lo.over_date )
	{
		sprintf( tmpstr," over_date>=%ld and",f_ln_lo.over_date );
		strcat( wherelist,tmpstr );
	}
	if( f_ln_loa.over_date )
	{
		sprintf( tmpstr," over_date<=%ld and",f_ln_loa.over_date );
		strcat( wherelist,tmpstr );
	}
	
	strcpy( tablelist,"ln_lo" );

	/**组成查询**/
	strcat(wherelist," 1=1") ;
 
	vtcp_log( "WHERE [%s]",wherelist );
strcpy( fieldlist,"ac_id,pay_type,lo_term,lo_amt,lo_intst,pun_intst,over_date,shld_pay_date,pay_lo_amt,pay_lo_intst,pay_pun_intst,old_ac_ind");
 
	ret=Ln_lo_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL
  printf("[%s][%d]\n",__FILE__,__LINE__);
	ttlnum=0;

	while(1)
	{
		ret=Ln_lo_Fet_Sel( &vln_lo, g_pub_tx.reply );
		if( ret==100 ) break;
		ERR_DEAL
    		printf("[%s][%d]\n",__FILE__,__LINE__);
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
     			vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
			/**显示列标题**/
			fprintf( fp,"~贷款账号|归还标志|欠款期次|欠本金|欠利息|欠罚息|转逾日期|应还日期|已还本金|已还利息|已归还罚息金额|旧账标志|\n" );
		}
    
	  /** pub_pubdb_prt_fld( fp,tablelist,fieldlist );

		ret= Ln_mst_Sel(g_pub_tx.reply,&g_ln_mst,"ac_id=%ld and ac_seqn=%d ",vln_lo.ac_id,vln_lo.ac_seqn);
		if(ret)
		{
			sprintf(acErrMsg," find ln_mst error " );
			WRITEMSG
			goto ErrExit;
		}
	 
		加入机构限制条件 
		sprintf(acErrMsg," opn_br_no=[%s] tx_br_no=[%s] ",g_ln_mst.opn_br_no, g_pub_tx.tx_br_no );
		WRITEMSG		
		if( strcmp( g_ln_mst.opn_br_no, g_pub_tx.tx_br_no) )
		{
			continue;
		}**/

		pub_base_acid_acno( vln_lo.ac_id,vln_lo.ac_seqn,ac_no0 );    /** 根据 ac_no 取得 ac_id 或 ac_seqn**/

		pub_base_dic_show_str(f_pay_type1,"pay_type",vln_lo.pay_type);
		if(vln_lo.lo_amt<-2147000000) vln_lo.lo_amt=0.0;
		if(vln_lo.lo_intst<-2147000000) vln_lo.lo_intst=0.0;
		if(vln_lo.pun_intst<-2147000000) vln_lo.pun_intst=0.0;
		if(vln_lo.over_date<-2147000000) vln_lo.over_date=0;
		if(vln_lo.shld_pay_date<-2147000000) vln_lo.shld_pay_date=0;
		if(vln_lo.pay_lo_amt<-2147000000) vln_lo.pay_lo_amt=0.0;
		if(vln_lo.pay_lo_intst<-2147000000) vln_lo.pay_lo_intst=0.0;
		if(vln_lo.pay_pun_intst<-2147000000) vln_lo.pay_pun_intst=0.0;	
 
    		printf("[%s][%d]\n",__FILE__,__LINE__);
		fprintf( fp, "%s|%s|%d|%.2f|%.2f|%.2f|%d|%d|%.2f|%.2f|%.2f|%s|\n",ac_no0,f_pay_type1,vln_lo.lo_term,vln_lo.lo_amt,vln_lo.lo_intst,vln_lo.pun_intst,vln_lo.over_date,vln_lo.shld_pay_date,vln_lo.pay_lo_amt,vln_lo.pay_lo_intst,vln_lo.pay_pun_intst,vln_lo.old_ac_ind);
		ttlnum++;
	}

	ret=Ln_lo_Clo_Sel( );
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
