/*************************************************
* 文 件 名:  sp1701.c
* 功能描述： 客户识别ID查询
*
* 作    者:  peter
* 完成日期： 2003-06-01
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
#include "cif_basic_inf_c.h"
#include "cif_id_code_rel_c.h"
#include "cif_email_inf_c.h"
#include "cif_addr_inf_c.h"

int sp1701()
{
	struct cif_basic_inf_c vcif_basic_inf;
	struct cif_basic_inf_c f_cif_basic_inf;
	struct cif_id_code_rel_c f_cif_id_code_rel;
	struct cif_email_inf_c cif_email_inf;
	struct cif_addr_inf_c cif_addr_inf;		
	
	char f_type1[41];
	char f_local3[41];
	char f_poten4[41];
	char f_lvl7[41];
	char f_shareholder_ind8[41];
	char f_id_type[41];
	char f_sts10[41];
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
		long beg_date=0,end_date=0;
		FILE *fp;
		int f_khh=0;

	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset( wherelist,0,sizeof(wherelist) );
	memset( fieldlist,0,sizeof(fieldlist) );
	memset( tablelist,0,sizeof(tablelist) );
	memset(&cif_email_inf,0x00,sizeof(struct cif_email_inf_c));
	memset(&cif_addr_inf,0x00,sizeof(struct cif_addr_inf_c));
		
	pub_base_sysinit();

	get_zd_long("0540",&f_cif_basic_inf.cif_no );
	get_zd_data("0700",f_cif_basic_inf.type );
	get_zd_data("0260",f_cif_basic_inf.name );
	get_zd_data("0660",f_cif_basic_inf.local );
	get_zd_data("0670",f_cif_basic_inf.poten );
	get_zd_data("0680",f_cif_id_code_rel.id_type );				/*证件类型*/
	get_zd_data("0620",f_cif_id_code_rel.id_no );				/*证件号码*/
	get_zd_data("0690",f_cif_basic_inf.shareholder_ind );
	
	get_zd_data("0710",f_cif_basic_inf.lvl );				/*客户级别*/
	get_zd_long("0440",&beg_date );				/*起始创建日期*/
	get_zd_long("0450",&end_date );				/*终止创建日期*/	
	
	/**组成查询条件**/
	if( f_cif_basic_inf.cif_no )
	{
		ret=Cif_id_code_rel_Sel(g_pub_tx.reply,&f_cif_id_code_rel, \
								"cif_no='%ld'", f_cif_basic_inf.cif_no);
		if(ret==100)
		{
			sprintf(acErrMsg,"无匹配的纪录!! ");
			WRITEMSG
			strcpy(g_pub_tx.reply,"O043");
			goto ErrExit;
		}

		sprintf( tmpstr," cif_no=%ld and",f_cif_basic_inf.cif_no );
		strcat( wherelist,tmpstr );
		f_khh=1;
	}
	else if(strlen(f_cif_id_code_rel.id_type)&&strlen(f_cif_id_code_rel.id_no))
	{
		ret=Cif_id_code_rel_Sel(g_pub_tx.reply,&f_cif_id_code_rel, \
								"id_type='%s' and id_no='%s'", \
								f_cif_id_code_rel.id_type, \
								f_cif_id_code_rel.id_no);
		if(ret==100)
		{
			sprintf(acErrMsg,"无匹配的纪录!! ");
			WRITEMSG
			strcpy(g_pub_tx.reply,"O043");
			goto ErrExit;
		}

		f_cif_basic_inf.cif_no=f_cif_id_code_rel.cif_no;
		sprintf( tmpstr," cif_no=%ld and",f_cif_basic_inf.cif_no );
		strcat( wherelist,tmpstr );
	}
	else if(strlen(f_cif_id_code_rel.id_no))
	{
		sprintf( tmpstr," cif_no in(select cif_no from Cif_id_code_rel where id_no='%s' )  and ",f_cif_id_code_rel.id_no );
		strcat( wherelist,tmpstr );
	}
	else if( strlen(f_cif_basic_inf.name) )
		;
	else 
	{
		sprintf(acErrMsg,"请输入更多的查询条件");
		set_zd_data(DC_GET_MSG,acErrMsg);
		WRITEMSG
		strcpy( g_pub_tx.reply,"BBCL" );
		goto ErrExit;
	}

	pub_base_strpack( f_cif_basic_inf.type );
	if( strlen(f_cif_basic_inf.type) )
	{
		sprintf( tmpstr," type='%s' and",f_cif_basic_inf.type );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_cif_basic_inf.name );
	if( strlen(f_cif_basic_inf.name) )
	{
		sprintf( tmpstr," name like '%%%%%s%%%%' and",f_cif_basic_inf.name );
vtcp_log("name is:[%s] [%s] [%d]",tmpstr,__FILE__,__LINE__);
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_cif_basic_inf.local );
	if( strlen(f_cif_basic_inf.local) )
	{
		sprintf( tmpstr," local='%s' and",f_cif_basic_inf.local );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_cif_basic_inf.poten );
	if( strlen(f_cif_basic_inf.poten) )
	{
		sprintf( tmpstr," poten='%s' and",f_cif_basic_inf.poten );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_cif_basic_inf.shareholder_ind );
	if( strlen(f_cif_basic_inf.shareholder_ind) )
	{
		sprintf( tmpstr," shareholder_ind='%s' and",f_cif_basic_inf.shareholder_ind );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_cif_basic_inf.lvl );
	if( strlen(f_cif_basic_inf.lvl) )
	{
		sprintf( tmpstr," lvl='%s' and",f_cif_basic_inf.lvl );
		strcat( wherelist,tmpstr );
	}
	
	if( beg_date )
	{
		sprintf( tmpstr," crt_date>=%ld and crt_date<=%ld and",beg_date, end_date);
		strcat( wherelist,tmpstr );
	}
		
	strcpy( tablelist,"cif_basic_inf" );

	/**组成查询**/
	strcat(wherelist," 1=1 order by cif_no,type") ;

	vtcp_log( "WHERE [%s] [%s] [%d]",wherelist,__FILE__,__LINE__ );
	sprintf(acErrMsg,"[wherelist]=[%s]",wherelist); 
	strcpy( fieldlist,"cif_no,type,name,local,poten,crt_date,crt_time,lvl,shareholder_ind,cif_seqn,sts");

	ret=Cif_basic_inf_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

		ttlnum=0;

	while(1)
	{
		ret=Cif_basic_inf_Fet_Sel( &vcif_basic_inf, g_pub_tx.reply );
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
			fprintf( fp,"~客户代码|@客户类型|@客户名称|@证件类型|@证件号码|建立日期|建立时间|@客户级别|@是否本行股东|客户账号序号|@客户当前状态|@联系电话|@联系地址|\n" );
		}

		ret=Cif_id_code_rel_Sel(g_pub_tx.reply,&f_cif_id_code_rel,"cif_no=%ld", vcif_basic_inf.cif_no);
		if(ret==100)continue;

		/*pub_pubdb_prt_fld( fp,tablelist,fieldlist ); */

		if(vcif_basic_inf.cif_no<-2147000000) vcif_basic_inf.cif_no=0;
		pub_base_dic_show_str(f_type1,"cif_type",vcif_basic_inf.type);
		pub_base_dic_show_str(f_local3,"cif_local",vcif_basic_inf.local);
		pub_base_dic_show_str(f_poten4,"cif_poten",vcif_basic_inf.poten);
		if(vcif_basic_inf.crt_date<-2147000000) vcif_basic_inf.crt_date=0;
		if(vcif_basic_inf.crt_time<-2147000000) vcif_basic_inf.crt_time=0;
		pub_base_dic_show_str(f_lvl7,"cif_lvl",vcif_basic_inf.lvl);
		pub_base_dic_show_str(f_shareholder_ind8,"shareholder",vcif_basic_inf.shareholder_ind);
		if(vcif_basic_inf.cif_seqn<-2147000000) vcif_basic_inf.cif_seqn=0;
		pub_base_dic_show_str(f_sts10,"cif_sts",vcif_basic_inf.sts);
		pub_base_dic_show_str(f_id_type,"id_type",f_cif_id_code_rel.id_type);
		
		/* 联系电话 */
		ret=Cif_email_inf_Sel(g_pub_tx.reply,&cif_email_inf, "cif_no=%ld and addr_seqn=1 ",vcif_basic_inf.cif_no);
		if( ret==100 )
		{
			strcpy( cif_email_inf.email , "无" );
		}
		/* 联系地址 */
		ret=Cif_addr_inf_Sel(g_pub_tx.reply,&cif_addr_inf, "cif_no=%ld and addr_seqn=1 ",vcif_basic_inf.cif_no);
		if( ret==100 )
		{
			strcpy( cif_addr_inf.addr , "无" );
		}
				
		fprintf( fp, "%ld|%s|%s|%s|%s|%ld|%ld|%s|%s|%ld|%s|%s|%s|\n",vcif_basic_inf.cif_no,f_type1,vcif_basic_inf.name,f_id_type,f_cif_id_code_rel.id_no,vcif_basic_inf.crt_date,vcif_basic_inf.crt_time,f_lvl7,f_shareholder_ind8,vcif_basic_inf.cif_seqn,f_sts10,cif_email_inf.email,cif_addr_inf.addr);
		ttlnum++;
	}

	ret=Cif_basic_inf_Clo_Sel( );
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
