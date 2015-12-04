/**************************************************************
* 文 件 名:  spH630.c
* 功能描述： 个人代理编号信息查询
*
* 作    者:  jack
* 完成日期： 2004年5月26日
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
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "ag_peritemmap_c.h"
int spH630()
{
		struct ag_peritemmap_c sAg_peritemmap;	/*单位信息表*/
		int ret = 0;
		char cGr_Ac_no[20];
  	char cUnit_no[5];
  	char cGr_name[51];
  	char cItem_no[21];
  	char cId_type[2];
  	char cId_no[21];
  	char wherelist[1024];  /**查询条件**/
		char tmpstr[512];
		int ttlnum=0;			/**读取的总条数**/
		int i=0;
		FILE *fp;
		char filename[100];
		char f_Id_type[21];
	memset( cGr_Ac_no, 0x0, sizeof(cGr_Ac_no) );
	memset( cUnit_no,0x0,sizeof(cUnit_no));
	memset( cGr_name,0x0,sizeof(cGr_name));
	memset( cItem_no,0x0,sizeof(cItem_no));
	memset( cId_no,0x0,sizeof(cId_no));
	memset( cId_type,0x0,sizeof(cId_type));
	memset( &sAg_peritemmap,0x0,sizeof(struct ag_peritemmap_c));
	memset( wherelist,0,sizeof(wherelist) );
	pub_base_sysinit();
	get_zd_data( "0310", cGr_Ac_no);
	get_zd_data( "0920", cUnit_no);
	get_zd_data( "0270", cGr_name);
	get_zd_data( "0600", cId_no);
	get_zd_data( "0700", cId_type);
	get_zd_data( "0630", cItem_no);
	pub_base_old_acno( cGr_Ac_no );
	/*set_zd_data( "0300", cGr_Ac_no);*/
  /**组成查询条件**/
	if( strlen( cGr_Ac_no ) )
	{
		sprintf( tmpstr," ac_no='%s' and",cGr_Ac_no );
		strcat( wherelist,tmpstr );
	}
	if( strlen( cUnit_no ) )
	{
		sprintf( tmpstr," unit_no='%s' and",cUnit_no );
		strcat( wherelist,tmpstr );
	}
  if( strlen( cGr_name ) )
  {
  	sprintf( tmpstr," name like '%%%%%s%%%%' and",cGr_name );
    strcat( wherelist,tmpstr );
  }
  if( strlen( cId_type ) )
  {
  	sprintf( tmpstr," id_type='%s' and",cId_type );
  	strcat( wherelist,tmpstr );
  }
  if( strlen(cId_no) )
  {
  	sprintf( tmpstr," id_no='%s' and",cId_no );
  	strcat( wherelist,tmpstr );
  }
  if(strlen(cItem_no))
  {
  	sprintf( tmpstr," item_no='%s' and",cItem_no);
  	strcat( wherelist,tmpstr );
  }
	/**组成查询**/
	strcat(wherelist," 1=1 order by unit_no,to_number(item_no)");
	vtcp_log( "WHERE [%s]",wherelist );
	ret=Ag_peritemmap_Dec_Sel(g_pub_tx.reply,wherelist);
	ERR_DEAL

  ttlnum=0;
  while(1)
	{
		ret=Ag_peritemmap_Fet_Sel(&sAg_peritemmap, g_pub_tx.reply );
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
			fprintf( fp,"~个人代理编号|个人账号|个人名称|证件类型|证件号码|单位编号|建立柜员|\n" );
		}
    pub_base_dic_show_str(f_Id_type,"id_type",sAg_peritemmap.id_type);
		fprintf( fp, "%s|%s|%s|%s|%s|%s|%s|\n",sAg_peritemmap.item_no,sAg_peritemmap.ac_no,sAg_peritemmap.name,f_Id_type,sAg_peritemmap.id_no,sAg_peritemmap.unit_no,sAg_peritemmap.opn_tel);
		ttlnum++;
	}
	ret=Ag_peritemmap_Clo_Sel();
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
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
