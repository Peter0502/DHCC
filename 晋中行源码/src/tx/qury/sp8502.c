/*************************************************
* 文 件 名:  sp8502.c
* 功能描述： 内部账明细查询
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
#include "in_mst_hst_c.h"
#include "dc_log_c.h"
#include "com_item_c.h"
#include "com_acc_br_c.h"
	char kmtj[100];

	struct com_item_c vitem;
	struct com_acc_br_c vacc_br;
	struct in_mst_hst_c f_in_mst_hst;
	struct in_mst_hst_c f_in_mst_hsta;
	char ac_no2[25];
	char f_note_type8[41];
	char f_acno3[25];
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
	int kmbz=0;

int sp8502()
{

	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset( wherelist,0,sizeof(wherelist) );
	memset( fieldlist,0,sizeof(fieldlist) );
	memset( tablelist,0,sizeof(tablelist) );

	pub_base_sysinit();

	get_zd_data("0910",f_in_mst_hst.opn_br_no );
	get_zd_long("0290",&f_in_mst_hst.tx_date );
	get_zd_long("0440",&f_in_mst_hsta.tx_date );
	get_zd_data("0380",f_acno3);
    pub_base_old_acno(f_acno3);
	get_zd_data("0700",f_in_mst_hst.add_ind );
	get_zd_data("0660",f_in_mst_hst.ct_ind );
	get_zd_double("1001",&f_in_mst_hst.tx_amt );
	get_zd_double("1002",&f_in_mst_hsta.tx_amt );
	get_zd_data("0240",f_in_mst_hst.note_type );
	get_zd_data("0590",f_in_mst_hst.note_no );
	get_zd_data("0630",f_in_mst_hst.brf );

	pub_base_strpack( f_acno3 );
	if( strlen(f_acno3)==3 || strlen(f_acno3)==5 || strlen(f_acno3)==7 )
	{
		kmbz=1;
		ret=Com_item_Sel( g_pub_tx.reply,&vitem,"acc_no='%s'",f_acno3 );
		if( ret )
		{
			vtcp_log( "acc_no='%s'",f_acno3 );
			strcpy( g_pub_tx.reply,"P100" );
			goto ErrExit;
		}
		if( vitem.acc_knd[0]=='0' && strlen(f_acno3)==3 )
		{
			sprintf( kmtj,"acc_hrt[1,3]='%s'",f_acno3 );
		}
		else if( vitem.acc_knd[0]=='0' )
		{
			sprintf( kmtj,"acc_hrt in (select acc_hrt from com_item where up_acc_hrt='%s') ",vitem.acc_hrt );
		}
		else
		{
			sprintf( kmtj,"acc_hrt='%s'",vitem.acc_hrt );
		}
	}
	else if( strlen(f_acno3) )
	{
		kmbz=0;
	}
	else
	{
		sprintf(acErrMsg,"需要输入帐号或科目号");
		WRITEMSG
		strcpy( g_pub_tx.reply,"D100" );
		goto ErrExit;
	}


	/******取得帐户*********************************/
	ret=get_ac_hst();
	if( ret ) goto ErrExit;

	/******取得科目*********************************/
	ret=get_dc_hst();
	if( ret ) goto ErrExit;

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
get_ac_hst()
{
	struct in_mst_hst_c vin_mst_hst;
	int first=0;
	char addstr[3];
	char ctstr[3];

	/**组成查询条件**/
	if( kmbz==1 ) /**输入了科目**/
	{
		ret=In_mst_Dec_Sel( g_pub_tx.reply,
			"prdt_no in(select prdt_no from in_parm where %s ) order by ac_id",
			kmtj );
		ERR_DEAL
	}
	else
	{
		if ( pub_base_acno_acid(&f_in_mst_hst.ac_id,f_acno3) )
		{
			vtcp_log("change acno[%s] to acid error",f_acno3 );
			goto ErrExit;
		}
		ret=In_mst_Dec_Sel( g_pub_tx.reply,
			" ac_id=%ld",f_in_mst_hst.ac_id );
		ERR_DEAL
	}

	strcpy( wherelist,"" );
/*
	pub_base_strpack( f_in_mst_hst.opn_br_no );
	if( strlen(f_in_mst_hst.opn_br_no) )
	{
		sprintf( tmpstr," opn_br_no='%s' and",f_in_mst_hst.opn_br_no );
		strcat( wherelist,tmpstr );
	}
*/
	if( f_in_mst_hst.tx_date )
	{
		sprintf( tmpstr," tx_date>=%ld and",f_in_mst_hst.tx_date );
		strcat( wherelist,tmpstr );
	}
	if( f_in_mst_hsta.tx_date )
	{
		sprintf( tmpstr," tx_date<=%ld and",f_in_mst_hsta.tx_date );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_in_mst_hst.ct_ind );
	if( strlen(f_in_mst_hst.ct_ind) )
	{
		sprintf( tmpstr," ct_ind='%s' and",f_in_mst_hst.ct_ind );
		strcat( wherelist,tmpstr );
	}
	if( f_in_mst_hst.tx_amt )
	{
		sprintf( tmpstr," tx_amt>=%lf and",f_in_mst_hst.tx_amt );
		strcat( wherelist,tmpstr );
	}
	if( f_in_mst_hsta.tx_amt )
	{
		sprintf( tmpstr," tx_amt<=%lf and",f_in_mst_hsta.tx_amt );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_in_mst_hst.note_type );
	if( strlen(f_in_mst_hst.note_type) )
	{
		sprintf( tmpstr," note_type='%s' and",f_in_mst_hst.note_type );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_in_mst_hst.note_no );
	if( strlen(f_in_mst_hst.note_no) )
	{
		sprintf( tmpstr," note_no='%s' and",f_in_mst_hst.note_no );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_in_mst_hst.brf );
	if( strlen(f_in_mst_hst.brf) )
	{
		sprintf( tmpstr," brf like  '%%%%%s%%%%' and",f_in_mst_hst.brf );
		strcat( wherelist,tmpstr );
	}

	/**组成查询**/
	strcat(wherelist," 1=1") ;
	ttlnum=0;

	while( 1 )
	{
		ret=In_mst_Fet_Sel( &g_in_mst, g_pub_tx.reply );
		if( ret==100 ) break;
		ERR_DEAL

		if( first==0 && kmbz==0 )
		{
			ret=In_parm_Sel(g_pub_tx.reply,&g_in_parm,"prdt_no='%s'",
				g_in_mst.prdt_no);
			ERR_DEAL
			pub_base_strpack(g_in_parm.acc_hrt);
			ret=Com_item_Sel(g_pub_tx.reply,&vitem,"acc_hrt='%s'",
				g_in_parm.acc_hrt);
			ERR_DEAL
			first=1;
		}

		vtcp_log( "WHERE [%s ac_id=%d]",wherelist,g_in_mst.ac_id );

		ret=In_mst_hst_Dec_Sel( g_pub_tx.reply,"%s and ac_id=%d order by tx_date,trace_no,trace_cnt",
			wherelist,g_in_mst.ac_id );
		ERR_DEAL


		while(1)
		{
			ret=In_mst_hst_Fet_Sel( &vin_mst_hst, g_pub_tx.reply );
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
				fprintf( fp,
			"~@账号|交易日期|流水号|@现转|@借贷|$发生额|$余额|@凭证种类|@凭证号码|@摘要|@开户行|@操作|@复核|@授权|\n"
					);
			}

			if( strlen(f_in_mst_hst.add_ind) )
			{
				if( f_in_mst_hst.add_ind[0]=='1' ) /**借**/
				{
					if( vitem.dc_ind[0]=='2' )
					{
						if( vin_mst_hst.add_ind[0]=='1' )
							continue;
					}
					else
					{
						if( vin_mst_hst.add_ind[0]=='0' )
							continue;
					}
				}
				else
				{
					if( vitem.dc_ind[0]=='2' )
					{
						if( vin_mst_hst.add_ind[0]=='0' )
							continue;
					}
					else
					{
						if( vin_mst_hst.add_ind[0]=='1' )
							continue;
					}
				}
			}
				if( vitem.dc_ind[0]=='2' )
				{
					if( vin_mst_hst.add_ind[0]=='1' )
						strcpy( addstr,"贷" );
					else
						strcpy( addstr,"借" );
				}
				else
				{
					if( vin_mst_hst.add_ind[0]=='0' )
						strcpy( addstr,"贷" );
					else
						strcpy( addstr,"借" );
				}

		/*pub_pubdb_prt_fld( fp,tablelist,fieldlist ); */

			if(vin_mst_hst.trace_no<-2147000000) vin_mst_hst.trace_no=0;
			if(vin_mst_hst.trace_cnt<-2147000000) vin_mst_hst.trace_cnt=0;
			pub_base_acid_acno( vin_mst_hst.ac_id,vin_mst_hst.ac_seqn,ac_no2 );
			if(vin_mst_hst.tx_amt<-2147000000) vin_mst_hst.tx_amt=0.0;
			if(vin_mst_hst.bal<-2147000000) vin_mst_hst.bal=0.0;
			pub_base_dic_show_str(f_note_type8,"note_type",
							vin_mst_hst.note_type);
			if(vin_mst_hst.tx_date<-2147000000) vin_mst_hst.tx_date=0;
			if( vin_mst_hst.ct_ind[0]=='1' ) strcpy(ctstr,"现");
			else strcpy( ctstr,"转" );

			fprintf( fp, "%s|%ld|%ld|%s|%s|%.2lf|%.2lf|%s|%s|%s|%s|%s|%s|%s|\n",
				ac_no2,vin_mst_hst.tx_date,vin_mst_hst.trace_no,ctstr,
				addstr,vin_mst_hst.tx_amt,
				vin_mst_hst.bal,f_note_type8,vin_mst_hst.note_no,"",
				vin_mst_hst.opn_br_no,
				vin_mst_hst.tel, vin_mst_hst.chk,vin_mst_hst.auth);
			ttlnum++;
		}

		ret=In_mst_hst_Clo_Sel( );
		ERR_DEAL

	}
	In_mst_Clo_Sel();

	return 0;
ErrExit:
	return -1;
}
get_dc_hst()
{
	struct com_item_c com_item_c;
	struct dc_log_c vdclog;
	char addstr[3];
	char ctstr[3];

TRACE
	if( kmbz==1 ) /**输入了科目**/
		;
	else
		return 0;

	strcpy( wherelist,"" );
	pub_base_strpack( f_in_mst_hst.opn_br_no );
	if( strlen(f_in_mst_hst.opn_br_no) )
	{
		sprintf( tmpstr," tx_opn_br_no='%s' and",f_in_mst_hst.opn_br_no );
		strcat( wherelist,tmpstr );
	}
	if( f_in_mst_hst.tx_date )
	{
		sprintf( tmpstr," tx_date>=%ld and",f_in_mst_hst.tx_date );
		strcat( wherelist,tmpstr );
	}
	if( f_in_mst_hsta.tx_date )
	{
		sprintf( tmpstr," tx_date<=%ld and",f_in_mst_hsta.tx_date );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_in_mst_hst.ct_ind );
	if( strlen(f_in_mst_hst.ct_ind) )
	{
		sprintf( tmpstr," ct_ind='%s' and",f_in_mst_hst.ct_ind );
		strcat( wherelist,tmpstr );
	}
	if( f_in_mst_hst.tx_amt )
	{
		sprintf( tmpstr," amt>=%lf and",f_in_mst_hst.tx_amt );
		strcat( wherelist,tmpstr );
	}
	if( f_in_mst_hsta.tx_amt )
	{
		sprintf( tmpstr," amt<=%lf and",f_in_mst_hsta.tx_amt );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_in_mst_hst.note_type );
	if( strlen(f_in_mst_hst.note_type) )
	{
		sprintf( tmpstr," note_type='%s' and",f_in_mst_hst.note_type );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_in_mst_hst.note_no );
	if( strlen(f_in_mst_hst.note_no) )
	{
		sprintf( tmpstr," note_no='%s' and",f_in_mst_hst.note_no );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_in_mst_hst.add_ind );
	if( strlen(f_in_mst_hst.add_ind) )
	{
		sprintf( tmpstr," dc_ind='%s' and",f_in_mst_hst.add_ind );
		strcat( wherelist,tmpstr );
	}

	/**组成查询**/

	/** rob -- 20031110 ---- 去掉撤销的流水 **
	strcat(wherelist," 1=1") ;
	**--------------------------------------**/
	strcat(wherelist," sts='0' ") ;


TRACE
	ret=Com_item_Dec_Sel( g_pub_tx.reply," %s and acc_knd='3' order by acc_no"
		,kmtj );
	ERR_DEAL

	while( 1 )
	{
		ret=Com_item_Fet_Sel( &com_item_c,g_pub_tx.reply );
		if( ret==100 ) break;
		ERR_DEAL

TRACE
		if( f_in_mst_hst.tx_date>=g_pub_tx.tx_date ) goto TODAY;

		pub_base_strpack( com_item_c.acc_hrt );
		ret=Dc_log_rz_Dec_Sel( g_pub_tx.reply," %s and acc_hrt='%s' order by tx_date,trace_no,trace_cnt",
			wherelist,com_item_c.acc_hrt );
		ERR_DEAL
TRACE
		while(1)
		{
			ret=Dc_log_rz_Fet_Sel( &vdclog,g_pub_tx.reply );
			if( ret==100 ) break;
			ERR_DEAL

TRACE
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
				fprintf( fp,
			"~@账号|交易日期|流水号|@现转||$发生额|$余额|@凭证种类|@凭证号码|@摘要|@开户行|@操作|@复核|@授权|\n"
					);
			}

			if(vdclog.trace_no<-2147000000) vdclog.trace_no=0;
			if(vdclog.trace_cnt<-2147000000) vdclog.trace_cnt=0;
			strcpy( ac_no2,com_item_c.acc_no );
			if(vdclog.amt<-2147000000) vdclog.amt=0.0;
			/* if(vin_mst_hst.bal<-2147000000) vin_mst_hst.bal=0.0;*/
			pub_base_dic_show_str(f_note_type8,"note_type",
							vdclog.note_type);
			if(vdclog.tx_date<-2147000000) vdclog.tx_date=0;
			if( vdclog.dc_ind[0]=='1' ) strcpy(addstr,"借" );
			else strcpy(addstr,"贷" );
			if( vdclog.ct_ind[0]=='1' ) strcpy(ctstr,"现");
			else strcpy( ctstr,"转" );
TRACE

			fprintf( fp, "%s|%ld|%ld|%s|%s|%.2lf|%.2lf|%s|%s|%s|%s|%s|%s|%s|\n",
				ac_no2,vdclog.tx_date,vdclog.trace_no,ctstr,
				addstr,vdclog.amt,
				0.00,f_note_type8,vdclog.note_no,"",
				vdclog.tx_opn_br_no,
				vdclog.tel, "","");
			ttlnum++;
TRACE

		}
		Dc_log_rz_Clo_Sel();

		TODAY:

		if( f_in_mst_hsta.tx_date &&  f_in_mst_hsta.tx_date<g_pub_tx.tx_date )
			goto NOTODAY;

TRACE
vtcp_log( " %s and acc_hrt='%s' ", wherelist,com_item_c.acc_hrt );
		pub_base_strpack( com_item_c.acc_hrt );
		ret=Dc_log_Dec_Sel( g_pub_tx.reply," %s and acc_hrt='%s' order by tx_date,trace_no,trace_cnt",
			wherelist,com_item_c.acc_hrt );
		ERR_DEAL
		while(1)
		{
			ret=Dc_log_Fet_Sel( &vdclog,g_pub_tx.reply );
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
				fprintf( fp,
			"~@账号|交易日期|流水号|@现转||$发生额|$余额|@凭证种类|@凭证号码|@摘要|@开户行|@操作|@复核|@授权|\n"
					);
			}

			if(vdclog.trace_no<-2147000000) vdclog.trace_no=0;
			if(vdclog.trace_cnt<-2147000000) vdclog.trace_cnt=0;
			strcpy( ac_no2,com_item_c.acc_no );
			if(vdclog.amt<-2147000000) vdclog.amt=0.0;
			/* if(vin_mst_hst.bal<-2147000000) vin_mst_hst.bal=0.0;*/
			pub_base_dic_show_str(f_note_type8,"note_type",
							vdclog.note_type);
			if(vdclog.tx_date<-2147000000) vdclog.tx_date=0;
			if( vdclog.dc_ind[0]=='1' ) strcpy(addstr,"借" );
			else strcpy(addstr,"贷" );
			if( vdclog.ct_ind[0]=='1' ) strcpy(ctstr,"现");
			else strcpy( ctstr,"转" );

			fprintf( fp, "%s|%ld|%ld|%s|%s|%.2lf|%.2lf|%s|%s|%s|%s|%s|%s|%s|\n",
				ac_no2,vdclog.tx_date,vdclog.trace_no,ctstr,
				addstr,vdclog.amt,
				0.00,f_note_type8,vdclog.note_no,"",
				vdclog.tx_opn_br_no,
				vdclog.tel, "","");
			ttlnum++;

		}
		Dc_log_Clo_Sel();

		NOTODAY:
			ret=ret;
	}
	Com_item_Clo_Sel();

	return 0;
ErrExit:
	return -1;
}
