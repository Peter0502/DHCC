/*************************************************
* 文 件 名:  sp8107.c
* 功能描述： 挂失登记簿查询
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
#include "mo_loss_c.h"

int sp8107()
{
	struct mo_loss_c vmo_loss;
	struct mo_loss_c f_mo_loss;
	struct mo_loss_c f_mo_lossa;
	char cBrno[7];
	char f_id_type1[41];
	char f_loss_ind4[41];
	char f_unloss_ind6[41];
	char f_lossbook[21];
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
		int needjg=0;
		char gsjg[10];
		char jgjg[10];

	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset( wherelist,0,sizeof(wherelist) );
	memset( fieldlist,0,sizeof(fieldlist) );
	memset( tablelist,0,sizeof(tablelist) );

	pub_base_sysinit();

	get_zd_data("0380",f_mo_loss.ac_no );
    pub_base_old_acno(f_mo_loss.ac_no );
	get_zd_data("0260",f_mo_loss.name );
	get_zd_data("0700",f_mo_loss.loss_ind );
	get_zd_data("0660",f_mo_loss.unloss_ind );
	get_zd_long("0290",&f_mo_loss.loss_date );
	get_zd_long("0440",&f_mo_lossa.loss_date );
	get_zd_data("0370",f_mo_loss.new_ac_no );
	get_zd_data("0910",gsjg );
	get_zd_data("0880",jgjg );
	get_zd_data("0030",cBrno);

	/**组成查询条件**/
	memset( tmpstr, 0x0, sizeof(tmpstr) );

	pub_base_strpack( cBrno );
	if( strlen(cBrno) )
	{
		sprintf( tmpstr," opn_br_no='%s' and",cBrno );
		strcat( wherelist,tmpstr );
		needjg=1;
	}
	pub_base_strpack( f_mo_loss.ac_no );
	if( strlen(f_mo_loss.ac_no) )
	{
		sprintf( tmpstr," ac_no='%s' and",f_mo_loss.ac_no );
		strcat( wherelist,tmpstr );
		needjg=1;
	}
	pub_base_strpack( f_mo_loss.name );
	if( strlen(f_mo_loss.name) )
	{
		sprintf( tmpstr," name='%s' and",f_mo_loss.name );
		strcat( wherelist,tmpstr );
		needjg=1;
	}
	pub_base_strpack( f_mo_loss.loss_ind );
	if( strlen(f_mo_loss.loss_ind) )
	{
		sprintf( tmpstr," loss_ind='%s' and",f_mo_loss.loss_ind );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_mo_loss.unloss_ind );
	if( strlen(f_mo_loss.unloss_ind) )
	{
		sprintf( tmpstr," unloss_ind='%s' and",f_mo_loss.unloss_ind );
		strcat( wherelist,tmpstr );
	}
	if( f_mo_loss.loss_date )
	{
		sprintf( tmpstr," loss_date>=%ld and",f_mo_loss.loss_date );
		strcat( wherelist,tmpstr );
	}
	if( f_mo_lossa.loss_date )
	{
		sprintf( tmpstr," loss_date<=%ld and",f_mo_lossa.loss_date );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_mo_loss.new_ac_no );
	if( strlen(f_mo_loss.new_ac_no) )
	{
		sprintf( tmpstr," new_ac_no='%s' and",f_mo_loss.new_ac_no );
		strcat( wherelist,tmpstr );
		needjg=1;
	}
		if( strlen(gsjg) )
		{
			sprintf( tmpstr," loss_br_no='%s' and ", gsjg );
			strcat( wherelist,tmpstr );
		}
		if( strlen(jgjg) )
		{
			sprintf( tmpstr," unloss_br_no='%s' and ", jgjg );
			strcat( wherelist,tmpstr );
		}

	strcat( wherelist," 1=1 " );

	strcpy( tablelist,"mo_loss" );

	/**组成查询**/
	strcat(wherelist," ORDER BY wrk_date,loss_ind,unloss_ind,ac_no ") ;

	vtcp_log( "WHERE [%s]",wherelist );
	strcpy( fieldlist,"ac_no,id_type,id_no,name,loss_ind,prop_book_loss,unloss_ind,opn_br_no,loss_date,loss_br_no,loss_tel,loss_chk,loss_auth,unloss_date,unloss_tel,unloss_chk,unloss_auth,new_ac_no,wrk_date,trace_no");

	ret=Mo_loss_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

		ttlnum=0;

	while(1)
	{
		ret=Mo_loss_Fet_Sel( &vmo_loss, g_pub_tx.reply );
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
			TRACE
			fprintf( fp,"~@挂失标志|@解挂标志|@原账/卡号|@申请人证件|@证件号码|@申请人姓名|@开户机构|挂失日期|@挂失机构|@挂失操作员|@挂失授权人|解挂日期|@解挂操作员|@解挂授权人|@新账卡号|\n" );
			TRACE
		}

		/*pub_pubdb_prt_fld( fp,tablelist,fieldlist ); */
		TRACE
		pub_base_dic_show_str(f_id_type1,"id_type",vmo_loss.id_type);
		pub_base_dic_show_str(f_loss_ind4,"loss_type",vmo_loss.loss_ind);
		/*
		pub_base_dic_show_str(f_unloss_ind6,"loss_type1",vmo_loss.unloss_ind);
		*/
		if( vmo_loss.unloss_ind[0]=='0' )
			strcpy(f_unloss_ind6,"未解挂");
		else if( vmo_loss.unloss_ind[0]=='7' )
			strcpy(f_unloss_ind6,"挂失销户");
		else
			strcpy(f_unloss_ind6,"已解挂");
		pub_base_dic_show_str(f_lossbook,"lossbook_ind",vmo_loss.prop_book_loss);		
		TRACE
		if(vmo_loss.loss_date<-2147000000) vmo_loss.loss_date=0;
		if(vmo_loss.unloss_date<-2147000000) vmo_loss.unloss_date=0;
		if(vmo_loss.wrk_date<-2147000000) vmo_loss.wrk_date=0;
		if(vmo_loss.trace_no<-2147000000) vmo_loss.trace_no=0;

		TRACE
		fprintf( fp, "%s|%s|%s|%s|%s|%s|%s|%ld|%s|%s|%s|%ld|%s|%s|%s|\n",f_loss_ind4,f_unloss_ind6,vmo_loss.ac_no,f_id_type1,vmo_loss.id_no,vmo_loss.name,vmo_loss.opn_br_no,vmo_loss.loss_date,vmo_loss.loss_br_no,vmo_loss.loss_tel,vmo_loss.loss_auth,vmo_loss.unloss_date,vmo_loss.unloss_tel,vmo_loss.unloss_auth,vmo_loss.new_ac_no);
		TRACE
		ttlnum++;
	}

	ret=Mo_loss_Clo_Sel( );
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
