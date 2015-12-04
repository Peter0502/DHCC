/*************************************************
* 文 件 名:  sp8105.c
* 功能描述： 冻结登记簿查询
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
#include "mo_hold_c.h"
#include "prdt_ac_id_c.h"
#include "dd_mst_c.h"
#include "td_mst_c.h"

int sp8105()
{
	struct mo_hold_c vmo_hold;
	struct mo_hold_c f_mo_hold;
	struct mo_hold_c v_mo_hold;
	struct prdt_ac_id_c	sPrdt_ac_id;
	struct dd_mst_c  g_dd_mst;
	struct td_mst_c  g_td_mst;
	char cBrno[6];
	char ac_no0[25];
	char f_hold_typ3[41];
	char f_auto_unhold_ind5[41];
	char f_hold_sts6[41];
	char f_acno0[25];
	char name[50];
		char filename[100];
		char wherelist[1024];  /**查询条件**/
		char tmpstr[512];
		char tmpmo[512];
		char fieldlist[1024];  /**要查询的字段列表**/
		char tablelist[128];   /**要查询的表名**/
		char titstr[1024];
		char tel[5];			/**核心识别电话银行标志**/
		int ttlnum=0;			/**读取的总条数**/
		int i=0;
		int ret=0;
		FILE *fp;

	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset( wherelist,0,sizeof(wherelist) );
	memset( fieldlist,0,sizeof(fieldlist) );
	memset( tablelist,0,sizeof(tablelist) );
	memset( tel,0x0,sizeof(tel));
	memset( cBrno,0x0,sizeof(cBrno));	
	memset( &vmo_hold,0,sizeof(vmo_hold));
	memset( &f_mo_hold,0,sizeof(f_mo_hold));
	memset( &v_mo_hold,0,sizeof(v_mo_hold));

	pub_base_sysinit();

	get_zd_data("0380",f_acno0);
    pub_base_old_acno( f_acno0 );
	get_zd_long("0510",&f_mo_hold.ac_seqn );
	get_zd_data("0700",f_mo_hold.hold_typ );
	get_zd_data("0660",f_mo_hold.hold_sts );
	get_zd_long("0290",&f_mo_hold.beg_hold_date );
	get_zd_long("0450",&f_mo_hold.wrk_date );
	get_zd_long("0460",&v_mo_hold.wrk_date );
	get_zd_long("0440",&f_mo_hold.fct_unhold_date );
	get_zd_data("0910",f_mo_hold.hold_br_no );
	get_zd_data("0810",tel);
	get_zd_data("0030",cBrno);

	/**组成查询条件**/
	pub_base_strpack( f_acno0 );
	if( strlen(f_acno0) )
	{
		if ( pub_base_acno_acid(&f_mo_hold.ac_id,f_acno0) )
		{
		goto ErrExit;
		}
		sprintf( tmpstr," ac_id=%ld and",f_mo_hold.ac_id );
		strcat( wherelist,tmpstr );
	}
	if( f_mo_hold.ac_seqn )
	{
		sprintf( tmpstr," ac_seqn=%d and",f_mo_hold.ac_seqn );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_mo_hold.hold_typ );
	if( strlen(f_mo_hold.hold_typ) )
	{
		if(f_mo_hold.hold_typ[0] == '0')
				sprintf( tmpstr," hold_typ like '%%%%' and");
		else
				sprintf( tmpstr," hold_typ='%s' and",f_mo_hold.hold_typ );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_mo_hold.hold_sts );
	if( strlen(f_mo_hold.hold_sts) )
	{
		sprintf( tmpstr," hold_sts='%s' and",f_mo_hold.hold_sts );
		strcat( wherelist,tmpstr );
	}
	if( f_mo_hold.beg_hold_date )
	{
		sprintf( tmpstr," beg_hold_date=%ld and",f_mo_hold.beg_hold_date );
		strcat( wherelist,tmpstr );
	}
	if( f_mo_hold.wrk_date )
	{
		sprintf( tmpstr," wrk_date>=%ld and",f_mo_hold.wrk_date );
		strcat( wherelist,tmpstr );
	}
	if( v_mo_hold.wrk_date )
	{
		sprintf( tmpstr," wrk_date<=%ld and",v_mo_hold.wrk_date );
		strcat( wherelist,tmpstr );
	}
	if( f_mo_hold.fct_unhold_date )
	{
		sprintf( tmpstr," fct_unhold_date=%ld and",f_mo_hold.fct_unhold_date );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_mo_hold.hold_br_no );
	if( strlen(f_mo_hold.hold_br_no) )
	{
		sprintf( tmpstr," hold_br_no='%s' and",f_mo_hold.hold_br_no );
		strcat( wherelist,tmpstr );
	}

	strcpy( tablelist,"mo_hold" );

	/**组成查询**/
	strcat(wherelist," 1=1 ORDER BY wrk_date,ac_id,ac_seqn,hold_seqn ") ;

	vtcp_log( "WHERE [%s]",wherelist );
	strcpy( fieldlist,"ac_id,ac_seqn,hold_seqn,hold_typ,plan_hold_amt,auto_unhold_ind,hold_sts,beg_hold_date,plan_unhold_dat,fct_unhold_date,hold_br_no,hold_tel,hold_brf,unhold_br_no,unhold_tel,unhold_brf,wrk_date,trace_no");

	ret=Mo_hold_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

		ttlnum=0;

	while(1)
	{
		/*vtcp_log( "[%s][%d] ttlnum= [%d]",__FILE__,__LINE__,ttlnum );*/
		ret=Mo_hold_Fet_Sel( &vmo_hold, g_pub_tx.reply );
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
			fprintf( fp,"~冻结序号|账号|序号|@户名|@冻结类型|$计划冻结金额|@自动解冻标志|@冻结状态解除|冻结起始日期|计划解冻日期|实际解冻日期|@冻结机构号|@冻结操作员|@冻结备注|@解冻机构号|@解冻操作员|@解冻备注|交易日期|\n" );
		}

		/*pub_pubdb_prt_fld( fp,tablelist,fieldlist ); */

		pub_base_acid_acno( vmo_hold.ac_id,vmo_hold.ac_seqn,ac_no0 );
		if(vmo_hold.hold_seqn<-2147000000) vmo_hold.hold_seqn=0;
		pub_base_dic_show_str(f_hold_typ3,"hold_type",vmo_hold.hold_typ);
		if(vmo_hold.plan_hold_amt<-2147000000) vmo_hold.plan_hold_amt=0.0;
		pub_base_dic_show_str(f_auto_unhold_ind5,"auto_unhold_ind",vmo_hold.auto_unhold_ind);
		pub_base_dic_show_str(f_hold_sts6,"hold_ind",vmo_hold.hold_sts);
		if(vmo_hold.beg_hold_date<-2147000000) vmo_hold.beg_hold_date=0;
		if(vmo_hold.plan_unhold_date<-2147000000) vmo_hold.plan_unhold_date=0;
		if(vmo_hold.fct_unhold_date<-2147000000) vmo_hold.fct_unhold_date=0;
		if(vmo_hold.wrk_date<-2147000000) vmo_hold.wrk_date=0;
		if(vmo_hold.trace_no<-2147000000) vmo_hold.trace_no=0;
		/**------- 取户名 --------**/
		memset(&g_td_mst,0x0,sizeof(struct td_mst_c));
		memset(&g_dd_mst,0x0,sizeof(struct dd_mst_c));
		ret=Prdt_ac_id_Sel(g_pub_tx.reply,&sPrdt_ac_id,"ac_id=%ld and ac_seqn=%d",vmo_hold.ac_id,vmo_hold.ac_seqn);
		ERR_DEAL
		if( sPrdt_ac_id.prdt_no[0]=='1' )
		{
			ret=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,"ac_id=%ld and ac_seqn=%d",vmo_hold.ac_id,vmo_hold.ac_seqn);
			ERR_DEAL
			/**------- 只打印本行 --------**/
			if( strcmp( cBrno,g_dd_mst.opn_br_no) )
			{
				/*vtcp_log( "[%s][%d] cBrno= [%s],g_dd_mst.opn_br_no=[%s]",__FILE__,__LINE__,cBrno ,g_dd_mst.opn_br_no);*/
					vtcp_log( "[%s][%d] tel= [%s]",__FILE__,__LINE__,tel );
				if(memcmp(tel,"tel",3))
				{	
				continue;
				}
			}
			strcpy(name,g_dd_mst.name);
			/*vtcp_log( "[%s][%d] name= [%s]",__FILE__,__LINE__,name );*/
		}
		else if( sPrdt_ac_id.prdt_no[0]=='2' )
		{
			ret=Td_mst_Sel(g_pub_tx.reply,&g_td_mst,"ac_id=%ld and ac_seqn=%d",vmo_hold.ac_id,vmo_hold.ac_seqn);
			ERR_DEAL
			/**------- 只打印本行 --------**/
			if( strcmp( cBrno,g_td_mst.opn_br_no) )
			{
				if(memcmp(tel,"tel",3))
				{	
				continue;
				}
			}
			strcpy(name,g_td_mst.name);
		}
		

		fprintf( fp, "%ld|%s|%d|%s|%s|%.2lf|%s|%s|%ld|%ld|%ld|%s|%s|%s|%s|%s|%s|%ld|\n",vmo_hold.hold_seqn,ac_no0,vmo_hold.ac_seqn,name,f_hold_typ3,vmo_hold.plan_hold_amt,f_auto_unhold_ind5,f_hold_sts6,vmo_hold.beg_hold_date,vmo_hold.plan_unhold_date,vmo_hold.fct_unhold_date,vmo_hold.hold_br_no,vmo_hold.hold_tel,vmo_hold.hold_brf,vmo_hold.unhold_br_no,vmo_hold.unhold_tel,vmo_hold.unhold_brf,vmo_hold.wrk_date);
		ttlnum++;
		/*vtcp_log( "[%s][%d] ttlnum= [%d]",__FILE__,__LINE__,ttlnum );*/
	}

	ret=Mo_hold_Clo_Sel( );
	ERR_DEAL
	
	vtcp_log( "[%s][%d] ttlnum= [%d]",__FILE__,__LINE__,ttlnum );

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
