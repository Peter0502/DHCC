/*************************************************
* 文 件 名: sp1710.c
* 功能描述：客户授信额度信息查询
* 作    者:  peter
* 完成日期： 
* 修改记录：
* 日    期:
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
#include "cif_crdt_inf_c.h"
#include "cif_crdt_code_c.h"

int sp1710()
{
	struct cif_crdt_inf_c vcif_crdt_inf;
	struct cif_crdt_inf_c f_cif_crdt_inf;
	struct cif_crdt_code_c f_cif_crdt_code;
		char f_cur_name[41];
		char f_yes_no[5];
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

	get_zd_long("0280",&f_cif_crdt_code.cif_no);
	get_zd_data("0580",f_cif_crdt_inf.credit_no);
	get_zd_data("0210",f_cif_crdt_inf.cur_no);
	get_zd_double("0400",&f_cif_crdt_inf.bal);
	get_zd_data("0670",f_cif_crdt_inf.cyc_ind);

	/**组成查询条件**/
	if( f_cif_crdt_code.cif_no )
	{
		ret=Cif_crdt_code_Sel(g_pub_tx.reply,&f_cif_crdt_code,"cif_no=%ld", \
							  f_cif_crdt_code.cif_no);
		if(ret==100)
		{
			sprintf(acErrMsg,"该客户不存在授信合同");
			WRITEMSG
			strcpy( g_pub_tx.reply,"C084" );
			goto ErrExit;
		}
		else if(ret)
		{
			sprintf(acErrMsg,"查询客户授信额度信息错");
			WRITEMSG
			goto ErrExit;
		}

		sprintf(tmpstr,"credit_no='%s' and ",f_cif_crdt_code.credit_no);
		strcat( wherelist,tmpstr );
	}

	if( strlen(f_cif_crdt_inf.credit_no) )
	{
		sprintf(tmpstr,"credit_no like '%%%%%s%%%%' and ",f_cif_crdt_inf.credit_no);
		strcat( wherelist,tmpstr );
	}

	if( strlen(f_cif_crdt_inf.cur_no) )
	{
		sprintf(tmpstr,"cur_no='%s' and ",f_cif_crdt_inf.cur_no );
		strcat( wherelist,tmpstr );
	}

	if( pub_base_CompDblVal(f_cif_crdt_inf.bal,0.00) )
	{
		sprintf(tmpstr,"bal=%lf and ",f_cif_crdt_inf.bal );
		strcat( wherelist,tmpstr );
	}

	if( strlen(f_cif_crdt_inf.cyc_ind) )
	{
		sprintf(tmpstr,"cyc_ind='%s' and ",f_cif_crdt_inf.cyc_ind );
		strcat( wherelist,tmpstr );
	}

	if( strlen(f_cif_crdt_inf.credit_no) )
	{
		sprintf(tmpstr,"credit_no like '%%%%%s%%%%' and ",f_cif_crdt_inf.credit_no);
		strcat( wherelist,tmpstr );
	}

	strcpy( tablelist,"cif_crdt_inf" );

	/**组成查询**/
	strcat(wherelist," 1=1") ;

	ret=Cif_crdt_inf_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

	ttlnum=0;

	while(1)
	{
		ret=Cif_crdt_inf_Fet_Sel( &vcif_crdt_inf, g_pub_tx.reply );
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
			fprintf( fp,"~授信合同号|@币种|@授信金额|@已使用金额|@循环使用|\n");
		}

		/*pub_pubdb_prt_fld( fp,tablelist,fieldlist ); */

		pub_base_dic_show_str(f_cur_name,"cur_name",vcif_crdt_inf.cur_no);
		pub_base_dic_show_str(f_yes_no,"yes_no",vcif_crdt_inf.cyc_ind);

		fprintf( fp, "%s|%s|%lf|%lf|%s|\n",vcif_crdt_inf.credit_no,f_cur_name,vcif_crdt_inf.bal,vcif_crdt_inf.amt,f_yes_no);
		ttlnum++;
	}

	ret=Cif_crdt_inf_Clo_Sel( );
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
