/*************************************************
* 文 件 名:  sp8202.c
* 功能描述： 贷款登记簿查询
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
#include "ln_reg_c.h"
#include "ln_mst_c.h"

int sp8202()
{
	struct ln_reg_c vln_reg;
	struct ln_reg_c f_ln_reg;
	struct ln_reg_c f_ln_rega;
	char ac_no0[25];
	char f_type4[41];
	char f_sts7[41];
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
		char bbr_no[6];
		FILE *fp;

	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset( wherelist,0,sizeof(wherelist) );
	memset( fieldlist,0,sizeof(fieldlist) );
	memset( tablelist,0,sizeof(tablelist) );

	pub_base_sysinit();

  get_zd_data("0030",bbr_no);
	get_zd_data("0380",f_acno0);
	get_zd_long("0510",&f_ln_reg.ac_seqn );
	get_zd_long("0290",&f_ln_reg.reg_date_beg );
	get_zd_long("0440",&f_ln_rega.reg_date_beg );
	get_zd_data("0700",f_ln_reg.type );
	get_zd_double("1001",&f_ln_reg.lo_bal );
	get_zd_data("0660",f_ln_reg.sts );


	/**组成查询条件**/
	pub_base_strpack( f_acno0 );
	if( strlen(f_acno0) )
	{
		if ( pub_base_acno_acid(&f_ln_reg.ac_id,f_acno0) )
		{
		goto ErrExit;
		}
		sprintf( tmpstr," ac_id=%ld and",f_ln_reg.ac_id );
		strcat( wherelist,tmpstr );
	}
	if( f_ln_reg.ac_seqn )
	{
		sprintf( tmpstr," ac_seqn=%d and",f_ln_reg.ac_seqn );
		strcat( wherelist,tmpstr );
	}
	if( f_ln_reg.reg_date_beg )
	{
		sprintf( tmpstr," reg_date_beg>=%ld and",f_ln_reg.reg_date_beg );
		strcat( wherelist,tmpstr );
	}
	if( f_ln_rega.reg_date_beg )
	{
		sprintf( tmpstr," reg_date_beg<=%ld and",f_ln_rega.reg_date_beg );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_ln_reg.type );
	if( strlen(f_ln_reg.type) )
	{
		sprintf( tmpstr," type='%s' and",f_ln_reg.type );
		strcat( wherelist,tmpstr );
	}
	if( f_ln_reg.lo_bal )
	{
		sprintf( tmpstr," lo_bal=%lf and",f_ln_reg.lo_bal );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_ln_reg.sts );
	if( strlen(f_ln_reg.sts) )
	{
		sprintf( tmpstr," sts='%s' and",f_ln_reg.sts );
		strcat( wherelist,tmpstr );
	}

	strcpy( tablelist,"ln_reg" );

	/**组成查询**/
	sprintf(wherelist+strlen(wherelist)," 1=1 ORDER BY ac_id,ac_seqn,type,sts ");

	vtcp_log( "[%s][%d] WHERE [%s]",__FILE__,__LINE__,wherelist );
	strcpy( fieldlist,"ac_id,ac_seqn,reg_date_beg,pay_date,type,lo_bal,pay_bal,sts");

	ret=Ln_reg_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

		ttlnum=0;

	while(1)
	{
		ret=Ln_reg_Fet_Sel( &vln_reg, g_pub_tx.reply );
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
			fprintf( fp,"~账    号|序号|@户名|产生起始日期|产生终止日期|结清日期|@类型|$欠款金额|$已还金额|@欠款状态|\n" );
		}
				
		/*pub_pubdb_prt_fld( fp,tablelist,fieldlist ); */

		pub_base_acid_acno( vln_reg.ac_id,vln_reg.ac_seqn,ac_no0 );
		
		/* 取贷款户名称 */
		ret = Ln_mst_Sel( g_pub_tx.reply , &g_ln_mst , " ac_id='%ld' and ac_seqn='%d' " , vln_reg.ac_id,vln_reg.ac_seqn );		
		if( ret )
		{
			sprintf(acErrMsg,"读取贷款主文件信息error");
			WRITEMSG
			strcpy( g_pub_tx.reply,"S049" );
			goto ErrExit;
		}		
		 /** 查询本机构 **/
		if( memcmp( bbr_no,g_ln_mst.opn_br_no,sizeof(g_ln_mst.opn_br_no)-1))
		{
		   if( strlen(f_acno0) != 0 )
		   {
		   	sprintf(acErrMsg," br_no==:[%s]isn't opn_br_no[%s]  error ",bbr_no,g_ln_mst.opn_br_no );
				WRITEMSG
				strcpy( g_pub_tx.reply,"T053" );
				goto ErrExit;
		   }
		   continue;
		}   
		 /**88888**/
		if(vln_reg.reg_date_beg<-2147000000) vln_reg.reg_date_beg=0;
		if(vln_reg.pay_date<-2147000000) vln_reg.pay_date=0;
		pub_base_dic_show_str(f_type4,"ln_reg_type",vln_reg.type);
		if(vln_reg.lo_bal<-2147000000) vln_reg.lo_bal=0.0;
		if(vln_reg.pay_bal<-2147000000) vln_reg.pay_bal=0.0;
		pub_base_dic_show_str(f_sts7,"pay_type",vln_reg.sts);

		fprintf( fp, "%s|%d|%s|%ld|%ld|%ld|%s|%.2lf|%.2lf|%s|\n",ac_no0,vln_reg.ac_seqn,g_ln_mst.name,vln_reg.reg_date_beg,vln_reg.reg_date_end,vln_reg.pay_date,f_type4,vln_reg.lo_bal,vln_reg.pay_bal,f_sts7);
		ttlnum++;
	}

	ret=Ln_reg_Clo_Sel( );
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
