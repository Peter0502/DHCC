/*************************************************
* 文 件 名:  spJ089.c
* 功能描述： 系统状态查询
*
* 作    者:  liq 
* 完成日期： 2006-8-29
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
#include "lv_sysctl_c.h"                                       	
		char filename[100];
		char wherelist[1024];  /**查询条件**/
		char fieldlist[1024];  /**要查询的字段列表**/
		char titstr[1024];
		char tmpstr[512];
		int  ttlnum=0;			/**读取的总条数**/
		int  i=0;
		int  ret=0;
		FILE *fp;
struct lv_sysctl_c  l_lv_sysctl;

int spJ089()
{
	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset( wherelist,0,sizeof(wherelist) );
	memset( fieldlist,0,sizeof(fieldlist) );
	memset(&l_lv_sysctl,'\0',sizeof(l_lv_sysctl));	

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

	/**组成查询条件**/

	/**  memset( wherelist,0,sizeof(wherelist)); 
	pub_base_strpack(accountreg.change_no);
	if( strlen(accountreg.change_no) )
	{
		sprintf( tmpstr," change_no='%s' and",accountreg.change_no );
		strcat( wherelist,tmpstr );                                                 
	}
	if( accountreg.tx_date )
	{
		sprintf( tmpstr," tx_date=%ld and",accountreg.tx_date );
		strcat( wherelist,tmpstr );
	}  **/
	
	/**组成查询**/
	vtcp_log("查询条件 wherelist =[%s]",wherelist);
	
	printf("[%s][%d]\n",__FILE__,__LINE__);
	ret=Lv_sysctl_Dec_Sel( g_pub_tx.reply, " 1=1 ");
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
		
	  memset(&l_lv_sysctl,'\0',sizeof(l_lv_sysctl));		  	
		ret = Lv_sysctl_Fet_Sel( &l_lv_sysctl, g_pub_tx.reply );
	
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
			
	  if(memcmp(l_lv_sysctl.stat,"1",1)==0){     /**恢复运行******/
		printf("[%s][%d]\n",__FILE__,__LINE__);
		memcpy(l_lv_sysctl.curstat,"正常营业",8);
	  }
	  else /****日切***/
	  { printf("[%s][%d]\n",__FILE__,__LINE__);
		  memcpy(l_lv_sysctl.curstat,"暂停营业",8);
	  }
	  vtcp_log("l_lv_sysctl.curstat=[%d]",l_lv_sysctl.curstat);
		

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
			"~@当前工作日期|下一工作日期|变更节点|营业状态|状态变更描述|\n");
		}
		 
		fprintf( fp, "%d|%d|%s|%s|%s|\n",
			l_lv_sysctl.tx_date,l_lv_sysctl.next_date,l_lv_sysctl.chgnode,l_lv_sysctl.curstat,l_lv_sysctl.message);
   
		ttlnum++;    
	}
	
	Lv_sysctl_Clo_Sel();
       
	return 0;
}

