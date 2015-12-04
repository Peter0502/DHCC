/*************************************************
* 文 件 名:  sp2718.c
* 功能描述： 对公/个人活期账户查询
*
* 作    者:  赵家鑫
* 完成日期： 
*
* 修改记录：
* 日   期:
* 修 改 人:
* 修改内容:
*insert into tx_def  values ('2718', '个人活期账户查询', '10000000000000010000011111111111111110001100000000010100000000000000000000000000000000000000000000000000000000000000000000000000', '1', '3', '1', '0', null);
*insert into tx_flow_def  values ('2718', 0, '2718', '#$');
*insert into tx_sub_def  values ('2718', '对公/个人活期账户查询', 'sp2718', '0', '0');
*************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define EXTERN
#include "public.h"
#include "dd_mst_c.h"

int sp2718()
{
	struct dd_mst_c vcif_alias_inf;
	/*struct dd_mst_c f_cif_alias_inf;*/
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
		int ac_type=0;
		long ac_id=0;
		double min=0.00;
		double max=0.00;
		FILE *fp;

	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset( wherelist,0,sizeof(wherelist) );
	memset( fieldlist,0,sizeof(fieldlist) );
	memset( tablelist,0,sizeof(tablelist) );

	pub_base_sysinit();

	get_zd_int("0520",&ac_type);
	get_zd_long("0540",&ac_id );
	get_zd_double("0410",&min );
	get_zd_double("0420",&max );
	
	/**组成查询条件**/
	if(ac_type==1 || ac_type==5)
		{
			sprintf( tmpstr," ac_type=%d and",ac_type );
			strcat( wherelist,tmpstr );
		}		
	if( ac_id )
		{
			sprintf( tmpstr," ac_id=%ld and",ac_id );
			strcat( wherelist,tmpstr );
		}
	if( min>0.00 && min<max )
		{
			sprintf( tmpstr," bal>%lf and",min );
			strcat( wherelist,tmpstr );
		}
	if(max>min && max!=0)
		{
			sprintf( tmpstr," bal<%lf and",max );
			strcat( wherelist,tmpstr );
		}
		
	/*strcpy( tablelist,"cif_alias_inf" );*/

	/**组成查询**/
	strcat(wherelist," 1=1") ;

	vtcp_log( "WHERE [%s]",wherelist );
	/*strcpy( fieldlist,"cif_no,alias_seqn,alias");*/

	ret=Dd_mst_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

		ttlnum=0;

	while(1)
	{
		ret=Dd_mst_Fet_Sel( & vcif_alias_inf, g_pub_tx.reply );
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
			fprintf( fp,"~账号|户名|开户日期|账户类型|余额\n" );
		}

		/*pub_pubdb_prt_fld( fp,tablelist,fieldlist ); */

		if( vcif_alias_inf.ac_id<-2147000000)  vcif_alias_inf.ac_id=0;

		fprintf( fp, "%ld|%s|%ld|%d|%lf|\n", vcif_alias_inf.ac_id, vcif_alias_inf.name, vcif_alias_inf.opn_date,  vcif_alias_inf.ac_type, vcif_alias_inf.bal);
		ttlnum++;
	}

	ret=Dd_mst_Clo_Sel( );
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
