/*************************************************
* 文 件 名:  sp8404.c
* 功能描述： 柜员基本信息查询
*
* 作    者:  jack
* 完成日期： 2005-07-03
*
* 修改记录：
* 日   期:
* 修 改 人:

*************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define EXTERN
#include "public.h"
#include "com_tel_c.h"
#include "auth_auth_auth_c.h"
#include "auth_auth_lmt_c.h"

int sp8404()
{
	struct com_tel_c vcom_tel;
	struct com_tel_c f_com_tel;
	struct auth_auth_auth_c auth_tel;
	struct auth_auth_lmt_c auth_lmt;

	char f_lvl2[20];
	char f_csts3[41];
	char br_no[6];
	char br_name[51];
	char br_tele[31];
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
        
	get_zd_data("0910",br_no );
	get_zd_data("0920",f_com_tel.tel );

	/**组成查询条件**/
	pub_base_strpack( br_no );
	pub_base_strpack( f_com_tel.tel );
	
	if( strlen(br_no) )
	{
		sprintf( tmpstr," br_no='%s' and ",br_no );
		strcat( wherelist,tmpstr );
	}
	
	if( strlen(f_com_tel.tel) )
	{
		sprintf( tmpstr," tel='%s' and ",f_com_tel.tel );
		strcat( wherelist,tmpstr );
	}
	
	/* 机构编码 */
	strcpy( tablelist,"com_tel" );
	
	/**组成查询**/
	strcat(wherelist," 1=1 and substr(tel,3,2)!='99' ORDER BY br_no , csts , tel ") ;
		 
	vtcp_log( "WHERE [%s]",wherelist );	
	        
	strcpy( fieldlist,"tel,name,lvl,csts,br_no,tx_type");

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
	fprintf( fp,"~@柜员号|@柜员姓名|@柜员级别|@状态|@网点名称|@网点电话|@大额行名行号|\n" );
        
    ret=Com_tel_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL
        
	ttlnum=0;

	while(1)
	{
		ret=Com_tel_Fet_Sel( &vcom_tel, g_pub_tx.reply );
		if( ret==100 ) 
		{
			if( !ttlnum )
			{
			 strcpy( g_pub_tx.reply,"S055" );
		         goto ErrExit;
		        }
			break;
		}
		 ERR_DEAL

		memset( &auth_tel,0x00,sizeof(struct auth_auth_auth_c));
		memset( &auth_lmt,0x00,sizeof(struct auth_auth_lmt_c));

		ret = pub_base_getbrname( vcom_tel.br_no , br_name );
		if( ret ){ strcpy( br_name,"未找到" ); }

		ret = pub_base_getbrtele( vcom_tel.br_no , br_tele );
		if( ret ){ strcpy( br_tele,"未找到" );}

		char tmp_type[20];
		memset(tmp_type,0x00,sizeof(tmp_type));
		memcpy(tmp_type,vcom_tel.tx_type,8);
		pub_base_dic_show_str(f_lvl2,"tel_type",tmp_type);
		pub_base_dic_show_str(f_csts3,"csts",vcom_tel.csts);

		ret = Auth_auth_auth_Sel(g_pub_tx.reply,&auth_tel,"tel='%s'", \
								 vcom_tel.tel);
		char debkno[30];
		memset(debkno,0x00,sizeof(debkno));
                ret = GetOr_br_noByBr_no(vcom_tel.br_no,debkno);
		if( ret ){ strcpy( debkno,"未找到" ); }
/*		新的权限下面的用不了
		ret = Auth_auth_lmt_Sel(g_pub_tx.reply,&auth_lmt,"auth_code='%s'", \
								auth_tel.auth_code);
		if( ret==100 )  strcpy(auth_lmt.mo,"普通柜员"); 
*/

		fprintf( fp, "%s|%s|%s|%s|%s|%s|%s|\n", vcom_tel.tel,vcom_tel.name,
				 f_lvl2,f_csts3,br_name,br_tele,debkno);
		ttlnum++;
	}

	Com_tel_Clo_Sel( );

	fclose(fp);
	set_zd_data( DC_FILE_SND,"1" );

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
