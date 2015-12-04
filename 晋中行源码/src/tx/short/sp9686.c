/*************************************************
* 文 件 名:  sp9686.c
* 功能描述： lv_chgbank查询
*
* 作    者:  
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
#include "lv_chgbank_c.h"

int sp9686()
{
	  struct lv_chgbank_c vlv_chgbank;
	  long  lEDDATE=0,lSTADATE=0;
	  char  cFlag[2];
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
  memset( cFlag,0,sizeof(cFlag));
	pub_base_sysinit();

	get_zd_long("0440",&lSTADATE);
	get_zd_long("0450",&lEDDATE );
	get_zd_data("0700",cFlag );

	/**组成查询条件**/
	pub_base_strpack(cFlag);
	if( strlen(cFlag) )
	{
		sprintf( tmpstr," stat='%s' and ",cFlag );
		strcat( wherelist,tmpstr );
	}
	
	if( lSTADATE )
	{
		sprintf( tmpstr," effday>=%ld and ",lSTADATE );
		strcat( wherelist,tmpstr );
	}
	if( lEDDATE )
	{
		sprintf( tmpstr," effday<=%ld and ",lEDDATE );
		strcat( wherelist,tmpstr );
	}

	/**组成查询**/
	strcat(wherelist," 1=1 order by EFFDAY") ;
	vtcp_log( "WHERE [%s]",wherelist );
	ret=Lv_chgbank_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL
		ttlnum=0;
	while(1)
	{
		memset(&vlv_chgbank,0x00,sizeof(struct lv_chgbank_c));
		ret=Lv_chgbank_Fet_Sel(&vlv_chgbank, g_pub_tx.reply );
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
			fprintf( fp,"~BAKCODE|AGECODE|OAGECODE|状态|有效日期|备注|\n" );
		}
		char statname[5];
		memset(statname,0x00,sizeof(statname));
    if(vlv_chgbank.stat[0]=='1')
    {
    	 memcpy(statname,"增加",4);
    }
    else if(vlv_chgbank.stat[0]=='2')
    {
    	 memcpy(statname,"修改",4);
    }
    else
    {
    	 memcpy(statname,"刪除",4);
    }
		
		fprintf( fp, "%s|%s|%s|%s|%d|%s|\n",vlv_chgbank.bakcode,vlv_chgbank.agecode,vlv_chgbank.oagecode,statname,vlv_chgbank.effday,vlv_chgbank.filler);
		ttlnum++;
	}

	ret=Lv_chgbank_Clo_Sel( );
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
