/*************************************************
* 文 件 名:  sp8210.c
* 功能描述： 贷款罚息启用止用信息查询
*
* 作    者:  jane
* 完成日期： 2003-08-19
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
#include "mo_pun_acc_c.h"

int sp8210()
{
	struct mo_pun_acc_c vmo_pun_acc;
	struct mo_pun_acc_c f_mo_pun_acc;
	struct mo_pun_acc_c f_mo_pun_acca;
	char ac_no[25];
	char sts_c[5];
	char cBr_no[6];
		char filename[100];
		char wherelist[1024];  /**查询条件**/
		char tmpstr[512];
		char tmpmo[512];
		char titstr[1024];
		int ttlnum=0;			/**读取的总条数**/
		int i=0;
		int ret=0;
		FILE *fp;
		long min_beg_date=0,min_end_date=0;
		long max_beg_date=0,max_end_date=0;

	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset( wherelist,0,sizeof(wherelist) );
  memset( cBr_no,0x0,sizeof(cBr_no));
	pub_base_sysinit();

	get_zd_data("0310",ac_no );
    pub_base_old_acno(ac_no);
	get_zd_data("0670",sts_c );
	get_zd_long("0440",&min_beg_date );
	get_zd_long("0450",&max_beg_date );

	get_zd_long("0460",&min_end_date );
	get_zd_long("0470",&max_end_date );	
	get_zd_data("0030",cBr_no);
	/**组成查询条件**/
	pub_base_strpack( ac_no );
	if( strlen(ac_no) )
	{
		ret = Mdm_ac_rel_Sel( g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",ac_no );
		if( ret==100 )
		{
			sprintf(acErrMsg,"账号不存在" );
			WRITEMSG
			strcpy( g_pub_tx.reply,"M003" );
			goto ErrExit;
		}
		else if(ret)
		{
			sprintf(acErrMsg,"查询错误ac_no=[%s]ret=[%d]",ac_no,ret);
			WRITEMSG
			strcpy( g_pub_tx.reply,"D103" );
			goto ErrExit;
		}
		ret = Ln_mst_Sel( g_pub_tx.reply,&g_ln_mst,"ac_id=%ld and ac_seqn=%d", \
						  g_mdm_ac_rel.ac_id , g_mdm_ac_rel.ac_seqn );
		if( ret==100 )
		{
			sprintf(acErrMsg,"账号不存在" );
			WRITEMSG
			strcpy( g_pub_tx.reply,"L114" );
			goto ErrExit;
		}
		else if(ret)
		{
			sprintf(acErrMsg,"查询错误");
			WRITEMSG
			strcpy( g_pub_tx.reply,"D103" );
			goto ErrExit;
		}

		sprintf( tmpstr," ac_id=%ld and ac_seqn=%d and", \
				 g_ln_mst.ac_id, g_ln_mst.ac_seqn );
		strcat( wherelist,tmpstr );
	}


	pub_base_strpack( sts_c );
	if( strlen( sts_c ) )
	{
		if( sts_c[0]=='0' )    /* 启用 */
		{			
			strcpy( tmpstr," sts=0 and" );
			strcat( wherelist,tmpstr );
		}
		else if( sts_c[0]=='1' )    /* 止用 */
		{			
			strcpy( tmpstr," sts != 0 and" );
			strcat( wherelist,tmpstr );
		}
	}

	if( min_beg_date )
	{
		sprintf( tmpstr," beg_date>=%ld and",min_beg_date );
		strcat( wherelist,tmpstr );
	}

	if( max_beg_date )
	{
		sprintf( tmpstr," beg_date<=%ld and",max_beg_date );
		strcat( wherelist,tmpstr );
	}

	if( min_end_date )
	{
		sprintf( tmpstr," end_date>=%ld and",min_end_date );
		strcat( wherelist,tmpstr );
	}

	if( max_end_date )
	{
		sprintf( tmpstr," end_date<=%ld and",max_end_date );
		strcat( wherelist,tmpstr );
	}
	

	/**组成查询**/
	strcat(wherelist," 1=1 ORDER BY ac_id,ac_seqn,sts") ;

	vtcp_log( "WHERE [%s]",wherelist );

	ret=Mo_pun_acc_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

		ttlnum=0;

	while(1)
	{
		ret=Mo_pun_acc_Fet_Sel( &vmo_pun_acc, g_pub_tx.reply );
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
			fprintf( fp,"~序号|@贷款账号|@贷款户名|@状态|启用日期|@启用柜员|@启用机构|@违约原因|止用日期|@止用柜员|@止用机构|\n" );
		}

		pub_base_acid_acno( vmo_pun_acc.ac_id,vmo_pun_acc.ac_seqn,ac_no );
		if(vmo_pun_acc.beg_date<-2147000000) vmo_pun_acc.beg_date=0;
		if(vmo_pun_acc.end_date<-2147000000) vmo_pun_acc.end_date=0;

		if( vmo_pun_acc.sts == 0 )
			strcpy( sts_c, "启用" );
		else if( vmo_pun_acc.sts > 0 )
			strcpy( sts_c, "止用" );

		memset( &g_ln_mst, 0x00, sizeof( struct ln_mst_c ) );
		ret = Ln_mst_Sel( g_pub_tx.reply,&g_ln_mst,"ac_id=%d and ac_seqn=%d", \
						  vmo_pun_acc.ac_id , vmo_pun_acc.ac_seqn );
		if(ret)	ERR_DEAL
		if( strcmp( cBr_no,g_ln_mst.opn_br_no) )
			{
				continue;
			}
		pub_base_strpack( g_ln_mst.name );
		sprintf(acErrMsg,"name [%s]",g_ln_mst.name);
		WRITEMSG		
		fprintf( fp, "%d|%s|%s|%s|%d|%s|%s|%s|%ld|%s|%s|\n",vmo_pun_acc.sts,ac_no,g_ln_mst.name,sts_c,vmo_pun_acc.beg_date,vmo_pun_acc.beg_tel,vmo_pun_acc.beg_br_no,vmo_pun_acc.brf,vmo_pun_acc.end_date,vmo_pun_acc.end_tel,vmo_pun_acc.end_br_no);
		ttlnum++;
	}

	ret=Mo_pun_acc_Clo_Sel( );
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
