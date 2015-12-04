/*************************************************
* 文 件 名:  sp6111.c
* 功能描述： 
*
* 作    者:  Gujy JinZ
* 完成日期： 
*
* 修改记录：20070428
* 日   期:
* 修 改 人:
* 修改内容: 从pay_agent_bk代收付表中记录
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
#include "pay_agent_bk_c.h"

int sp6111()
{
	struct pay_agent_bk_c pay_agent_bk;
	char ac_no0[25];
	char actno[25];
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
		char sdate[9];
		FILE *fp;

	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset( wherelist,0,sizeof(wherelist) );
	memset( fieldlist,0,sizeof(fieldlist) );
	memset( tablelist,0,sizeof(tablelist) );

	pub_base_sysinit();
	memset(&pay_agent_bk, 0x0, sizeof(pay_agent_bk));

  	get_zd_data("0300", sdate);
	get_zd_data("0920",pay_agent_bk.unit_no);


	/**组成查询条件**/
	pub_base_strpack( pay_agent_bk.unit_no );
	if( strlen(pay_agent_bk.unit_no) )
	{
		sprintf( tmpstr," unit_no='%s' and ", pay_agent_bk.unit_no );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( sdate );
	if( strlen(sdate) )
	{
		sprintf( tmpstr," tx_date='%s' and ", sdate );
		strcat( wherelist,tmpstr );
	}


	strcpy( tablelist,"pay_agent_bk" );

	/**组成查询**/
	sprintf(wherelist+strlen(wherelist),"   1=1  ORDER BY tx_date desc,ac_no "  );

	vtcp_log( "[%s][%d] WHERE [%s]",__FILE__,__LINE__,wherelist );
	strcpy( fieldlist,"actno,actnoseq,txdate,atiday,adjday,rate,dinum,intr,avbal, stat");
	memset(actno, 0x0, sizeof(actno));
	ret=Pay_agent_bk_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL
		ttlnum=0;

	while(1)
	{
		ret=Pay_agent_bk_Fet_Sel( &pay_agent_bk, g_pub_tx.reply );
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
			fprintf( fp,"~单发编号|账号|户名|交易日期|$交易金额|\n" );
		}
		 
				
		/*pub_pubdb_prt_fld( fp,tablelist,fieldlist ); */
		 /**88888**/

		fprintf( fp, "%s|%s|%.60s|%ld|%.2lf|\n",
				pay_agent_bk.item_no,pay_agent_bk.ac_no,pay_agent_bk.name,
			pay_agent_bk.tx_date, pay_agent_bk.tram);
		ttlnum++;
	}

	ret=Pay_agent_bk_Clo_Sel( );
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
