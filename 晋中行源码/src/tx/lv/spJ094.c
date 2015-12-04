/*************************************************
* 文 件 名:  spJ094.c
* 功能描述： 系统参数查询
*
* 作    者:  liq 
* 完成日期： 2006-9-13
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
#include "lv_param_c.h"                                       	
		char filename[100];
		char wherelist[1024];  /**查询条件**/
		char fieldlist[1024];  /**要查询的字段列表**/
		char titstr[1024];
		char tmpstr[512];
		int  ttlnum=0;			/**读取的总条数**/
		int  i=0;
		int  ret=0;
		FILE *fp;
struct lv_param_c  lv_param;


int spJ094()
{
	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset( wherelist,0,sizeof(wherelist) );
	memset( fieldlist,0,sizeof(fieldlist) );
	memset(&lv_param,'\0',sizeof(lv_param));	

	pub_base_sysinit();

	ret = get_ac_hst();
	if( ret ) goto ErrExit;

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



int get_ac_hst()
{
	char tmp_no[64];

	
	/**组成查询**/
	vtcp_log("查询条件 wherelist =[%s]",wherelist);
	
	printf("[%s][%d]\n",__FILE__,__LINE__);
	ret=Lv_param_Dec_Sel( g_pub_tx.reply, " 1=1 ");
	vtcp_log("ret =[%d]",ret);	
	
	if(ret)
			{
	     	sprintf( acErrMsg, "sql error" );
		    WRITEMSG 
		    return 1;
			}


	ttlnum=0;

	while( 1 )
	{
		
	  memset(&lv_param,'\0',sizeof(lv_param));		  	
		ret = Lv_param_Fet_Sel( &lv_param, g_pub_tx.reply );
	
	  vtcp_log("ret =[%d]",ret);

		if(ret == 100) /*获取完了*/
		{
			  return 0;
		}	  	   		
		else if(ret)
			{

     	sprintf( acErrMsg, "sql error" );
		    WRITEMSG 
		    return 1;
			}
					
		if( !ttlnum )
		{
			pub_base_AllDwnFilName( filename );
			fp = fopen( filename,"w" );
			if( fp==NULL )
			{
				sprintf(acErrMsg," open file [%s] error ",filename);
				WRITEMSG
				strcpy( g_pub_tx.reply,"S047" );
				return 1;
			}
     
			/**显示列标题**/
			fprintf( fp,
			"~@参数代码|参数名称|参数值|\n");
		}
		 
		fprintf( fp, "%s|%s|%s|\n",
			lv_param.par_code,lv_param.par_name,lv_param.par_value);
   
		ttlnum++;    
	}
	
	Lv_param_Clo_Sel();
       
	return 0;
}
