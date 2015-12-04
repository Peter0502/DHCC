/*************************************************
* 文 件 名:  sp1705.c
* 功能描述： 客户联系方式信息维护
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
#include "cif_email_inf_c.h"

int sp1705()
{
	struct cif_email_inf_c vcif_email_inf;
	struct cif_email_inf_c f_cif_email_inf;
	char f_addr_type2[41];
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

	get_zd_long("0540",&f_cif_email_inf.cif_no );
	get_zd_long("0510",&f_cif_email_inf.addr_seqn );

	/**组成查询条件**/
	if( f_cif_email_inf.cif_no )
	{
		sprintf( tmpstr," cif_no=%ld and",f_cif_email_inf.cif_no );
		strcat( wherelist,tmpstr );
	}
	if( f_cif_email_inf.addr_seqn )
	{
		sprintf( tmpstr," addr_seqn=%d and",f_cif_email_inf.addr_seqn );
		strcat( wherelist,tmpstr );
	}

	strcpy( tablelist,"cif_email_inf" );

	/**组成查询**/
	strcat(wherelist," 1=1") ;

	vtcp_log( "WHERE [%s]",wherelist );
	strcpy( fieldlist,"cif_no,addr_seqn,addr_type,email,link_man");

	ret=Cif_email_inf_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

		ttlnum=0;

	while(1)
	{
		ret=Cif_email_inf_Fet_Sel( &vcif_email_inf, g_pub_tx.reply );
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
			fprintf( fp,"~客户代码|序号|@地址类型|@电子地址|@联系人|\n" );
		}

		/*pub_pubdb_prt_fld( fp,tablelist,fieldlist ); */

		if(vcif_email_inf.cif_no<-2147000000) vcif_email_inf.cif_no=0;
		pub_base_dic_show_str(f_addr_type2,"addr_type",vcif_email_inf.addr_type);

		fprintf( fp, "%ld|%d|%s|%s|%s|\n",vcif_email_inf.cif_no,vcif_email_inf.addr_seqn,f_addr_type2,vcif_email_inf.email,vcif_email_inf.link_man);
		ttlnum++;
	}

	ret=Cif_email_inf_Clo_Sel( );
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
