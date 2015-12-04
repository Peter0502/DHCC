/*************************************************
* 文 件 名:  sp8106.c
* 功能描述： 通知登记簿查询
*
* 作    者:  
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
#include "mo_infrm_c.h"
#include "td_mst_c.h"

int sp8106()
{
	struct mo_infrm_c vmo_infrm;
	struct mo_infrm_c f_mo_infrm;
	struct mo_infrm_c f_mo_infrma;
	struct td_mst_c  g_td_mst;
	char cBrno[6];
	char ac_no0[25];
	char f_sts7[41];
	char f_sts9[11];
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
	memset( cBrno,0x0,sizeof(cBrno));

	pub_base_sysinit();

	get_zd_data("0380",f_acno0);
	get_zd_long("0510",&f_mo_infrm.ac_seqn );
	get_zd_long("0290",&f_mo_infrm.infrm_date );
	get_zd_long("0440",&f_mo_infrm.prm_draw_date );
	get_zd_long("0450",&f_mo_infrma.prm_draw_date );
	get_zd_data("0260",f_mo_infrm.name );
	get_zd_data("0030",cBrno);

	/**组成查询条件**/
	pub_base_strpack( f_acno0 );
	if( strlen(f_acno0) )
	{
		if ( pub_base_acno_acid(&f_mo_infrm.ac_id,f_acno0) )
		{
		goto ErrExit;
		}
		sprintf( tmpstr," ac_id=%ld and",f_mo_infrm.ac_id );
		strcat( wherelist,tmpstr );
	}
	if( f_mo_infrm.ac_seqn )
	{
		sprintf( tmpstr," ac_seqn=%d and",f_mo_infrm.ac_seqn );
		strcat( wherelist,tmpstr );
	}
	if( f_mo_infrm.infrm_date )
	{
		sprintf( tmpstr," infrm_date=%ld and",f_mo_infrm.infrm_date );
		strcat( wherelist,tmpstr );
	}
	if( f_mo_infrm.prm_draw_date )
	{
		sprintf( tmpstr," prm_draw_date>=%ld and",f_mo_infrm.prm_draw_date );
		strcat( wherelist,tmpstr );
	}
	if( f_mo_infrma.prm_draw_date )
	{
		sprintf( tmpstr," prm_draw_date<=%ld and",f_mo_infrma.prm_draw_date );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_mo_infrm.name );
	if( strlen(f_mo_infrm.name) )
	{
		sprintf( tmpstr," name like '%%%%%s%%%%' and",f_mo_infrm.name );
		strcat( wherelist,tmpstr );
	}

	strcpy( tablelist,"mo_infrm" );

	/**组成查询**/
	strcat(wherelist," 1=1 ORDER BY sts,ac_id,ac_seqn ") ;

	vtcp_log( "WHERE [%s]",wherelist );
	strcpy( fieldlist,"ac_id,ac_seqn,infrm_date,prm_draw_date,prm_draw_amt,fct_wd_amt,occ_date,sts,name,phone,wrk_date,trace_no,id_knd,id_no");

	ret=Mo_infrm_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

		ttlnum=0;

	while(1)
	{
		ret=Mo_infrm_Fet_Sel( &vmo_infrm, g_pub_tx.reply );
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
			fprintf( fp,"~账       号|序号|约定日期|$约定金额|$实际金额|实际日期|@状态|通知日期|@通知人姓名|@电话|@证件类型|@证件号码|\n" );
		}

		/*pub_pubdb_prt_fld( fp,tablelist,fieldlist ); */

		pub_base_acid_acno( vmo_infrm.ac_id,vmo_infrm.ac_seqn,ac_no0 );
		if(vmo_infrm.infrm_date<-2147000000) vmo_infrm.infrm_date=0;
		if(vmo_infrm.prm_draw_date<-2147000000) vmo_infrm.prm_draw_date=0;
		if(vmo_infrm.prm_draw_amt<-2147000000) vmo_infrm.prm_draw_amt=0.0;
		if(vmo_infrm.fct_wd_amt<-2147000000) vmo_infrm.fct_wd_amt=0.0;
		if(vmo_infrm.occ_date<-2147000000) vmo_infrm.occ_date=0;
		pub_base_dic_show_str(f_sts7,"infrm_sts",vmo_infrm.sts);
		if(vmo_infrm.wrk_date<-2147000000) vmo_infrm.wrk_date=0;
		if(vmo_infrm.trace_no<-2147000000) vmo_infrm.trace_no=0;
		pub_base_dic_show_str(f_sts9,"id_type",vmo_infrm.id_knd);
		
		/**------- 只打印本行 --------**/
		memset(&g_td_mst,0x0,sizeof(struct td_mst_c));
		ret=Td_mst_Sel(g_pub_tx.reply,&g_td_mst,"ac_id=%ld and ac_seqn=%d",vmo_infrm.ac_id,vmo_infrm.ac_seqn);
			ERR_DEAL
			if( strcmp( cBrno,g_td_mst.opn_br_no) )
			{
				continue;
			}
		
		fprintf( fp, "%s|%d|%ld|%.2lf|%.2lf|%ld|%s|%ld|%s|%s|%s|%s|\n",ac_no0,vmo_infrm.ac_seqn,vmo_infrm.prm_draw_date,vmo_infrm.prm_draw_amt,vmo_infrm.fct_wd_amt,vmo_infrm.occ_date,f_sts7,vmo_infrm.infrm_date,vmo_infrm.name,vmo_infrm.phone,f_sts9,vmo_infrm.id_no);
		ttlnum++;
	}

	ret=Mo_infrm_Clo_Sel( );
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
