/*************************************************
* 文 件 名:  sp8305.c
* 功能描述： 凭证汇总查询
*
* 作    者:  lance
* 完成日期： 2003-10-27
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
#include "note_parm_c.h"
#include "note_mst_c.h"

int sp8305()
{
	struct note_parm_c vnote_parm;
	struct note_mst_c vnote_mst;
	struct note_mst_c f_note_mst;
	char f_sts3[41];
	char filename[100];
	char wherelist[1024];  	/**查询条件**/
	char Twherelist[1024];  /**查询条件**/
	char wherelist1[1024];  /**查询条件**/
	char wherelist2[1024];  /**查询条件**/	
	char tmpstr[512];
	char tmpmo[512];
	char fieldlist[1024];  	/**要查询的字段列表**/
	char tablelist[128];   	/**要查询的表名**/
	char titstr[1024];
	int flag=0;				/**读取的总条数**/
	long ttlnum=0;			/**读取的总条数**/
	int i=0;
	int ret=0;
	FILE *fp;
	
	long ys_cnt=0;			/* 昨日库存 */
	long in_cnt=0;			/* 今日领入 */
	long out_cnt=0;			/* 今日交出 */
	long use_cnt=0;			/* 今日使用 */
	long buy_cnt=0;			/* 客户购买 */
	long dsy_cnt=0;			/* 今日作废 */
	long cls_cnt=0;			/* 今日销号 */
	long lose_cnt=0;		/* 今日挂失 */
	long unlose_cnt=0;		/* 今日解挂 */
	long dd_cnt=0;			/* 今日销号 */
	long now_cnt=0;			/* 今日结存 */
	long tmp_cnt=0;

	long ys_cnt_tol=0;		/* 昨日库存总计 */
	long in_cnt_tol=0;		/* 今日领入总计 */
	long out_cnt_tol=0;		/* 今日交出总计 */
	long use_cnt_tol=0;		/* 今日使用总计 */
	long buy_cnt_tol=0;		/* 客户购买总计 */
	long dsy_cnt_tol=0;		/* 今日作废总计 */
	long cls_cnt_tol=0;		/* 今日销号总计 */
	long lose_cnt_tol=0;	/* 今日挂失总计 */
	long unlose_cnt_tol=0;	/* 今日解挂总计 */
	long dd_cnt_tol=0;		/* 今日销号总计 */	
	long now_cnt_tol=0;		/* 今日结存总计 */
	
	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset( wherelist,0,sizeof(wherelist) );
	memset( wherelist1,0,sizeof(wherelist1) );
	memset( wherelist2,0,sizeof(wherelist2) );	
	memset( fieldlist,0,sizeof(fieldlist) );
	memset( tablelist,0,sizeof(tablelist) );

	pub_base_sysinit();

	get_zd_data("0650",f_note_mst.br_no );
	get_zd_data("0910",f_note_mst.tel );
	get_zd_data("0890",f_note_mst.note_type );
	get_zd_data("0670",f_note_mst.sts );

	/**组成查询条件**
	pub_base_strpack( f_note_mst.br_no );
	if( strlen(f_note_mst.br_no) )
	{
		sprintf( tmpstr," br_no='%s' and ",f_note_mst.br_no );
		strcat( wherelist,tmpstr );
	}

	pub_base_strpack( f_note_mst.tel );
	if( strlen(f_note_mst.tel) )
	{
		sprintf( tmpstr," tel='%s' and ",f_note_mst.tel );
		strcat( wherelist,tmpstr );
	}
	***/

	pub_base_strpack( f_note_mst.note_type );
	if( strlen(f_note_mst.note_type) )
	{
		sprintf( tmpstr," note_type='%s' and ",f_note_mst.note_type );
		strcat( wherelist,tmpstr );
	}

	/***
	pub_base_strpack( f_note_mst.sts );
	if( strlen(f_note_mst.sts) )
	{
		sprintf( tmpstr," sts='%s' and ",f_note_mst.sts );
		strcat( wherelist,tmpstr );
	}
	***/

	strcat(wherelist," 1=1 ") ;
	sprintf(wherelist2,"%s ORDER BY note_type ",wherelist);
	
	ret=Note_parm_Dec_Sel( g_pub_tx.reply,wherelist2 );
	ERR_DEAL

	ttlnum=0;

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
	fprintf( fp,"~@种类|@凭证名称|领入|缴出|使用|出售|作废|销号|结  存|\n" );

	while(1)
	{
		memset(&vnote_parm, 0x0, sizeof(struct note_parm_c));
		ret=Note_parm_Fet_Sel( &vnote_parm, g_pub_tx.reply );
		if( ret==100 ) 
		{
			if( !flag )
			{
				strcpy( g_pub_tx.reply,"S049" );
				goto ErrExit;
			}else{
				fprintf( fp, "%s|%s|%ld|%ld|%ld|%ld|%ld|%ld|%ld|\n",
					"总计","",in_cnt_tol,out_cnt_tol,use_cnt_tol,
					buy_cnt_tol,dsy_cnt_tol,cls_cnt_tol,now_cnt_tol);
				break ;
			}
		}
		ERR_DEAL

		memset(wherelist1, 0x0, sizeof(wherelist1));
		pub_base_strpack( f_note_mst.tel );
		if( strlen(f_note_mst.tel) )
		{
			sprintf( tmpstr," and tel='%s' ",f_note_mst.tel );
			strcat( wherelist1,tmpstr );
		}

		pub_base_strpack( f_note_mst.br_no );
		if( strlen(f_note_mst.br_no) )
		{
			sprintf( tmpstr," and br_no='%s' ",f_note_mst.br_no );
			strcat( wherelist1,tmpstr );
		}

		pub_base_strpack( f_note_mst.sts );
		if( strlen(f_note_mst.sts) )
		{
			sprintf( tmpstr," and sts='%s' ",f_note_mst.sts );
			strcat( wherelist1,tmpstr );
		}
		
		/**组成查询**/
		sprintf(tmpstr," %s and note_type='%s' ", wherelist1,vnote_parm.note_type) ;
		memset(Twherelist, 0x0, sizeof(Twherelist));
		strcpy( Twherelist, wherelist);
		strcat( Twherelist, tmpstr );

		vtcp_log( "WHERE [%s]",Twherelist );
		strcpy( fieldlist,"tel,note_type,br_no,sts,cnt");

		ttlnum=0;
		ret= sql_sum_long ( "note_mst" , "cnt" , &ttlnum , Twherelist );
		ERR_DEAL

     		/* 统计今日结存 */
		ret = sql_sum_long( "note_mst" , "cnt" , &tmp_cnt , \
				" %s and sts in( 0 , 1 ) " , \
				Twherelist );
		ERR_DEAL

     		now_cnt = tmp_cnt;
		now_cnt_tol+=now_cnt;
		
		/* 今日领入 */
		ret = sql_sum_long( "note_mst_hst" , "note_cnt" , &tmp_cnt , 
			" tx_date=%ld and %s and sts_ind in('1','3','5','9','A')" , 
			g_pub_tx.tx_date,Twherelist );
		ERR_DEAL			

		in_cnt=tmp_cnt;
		in_cnt_tol+=in_cnt;

		/* 今日交出 */	
		ret = sql_sum_long( "note_mst_hst" , "note_cnt" , &tmp_cnt ,
			"tx_date=%ld and %s and sts_ind in('2','4','7','8')" , 
			g_pub_tx.tx_date,Twherelist );
		ERR_DEAL		

		out_cnt=tmp_cnt;
		out_cnt_tol+=out_cnt;

		/* 今日使用 */	
		ret = sql_sum_long( "note_mst_hst" , "note_cnt" , &tmp_cnt , 
			" tx_date=%ld and %s and sts_ind ='D'" , 
			g_pub_tx.tx_date,Twherelist );
		ERR_DEAL		

		use_cnt=tmp_cnt;
		use_cnt_tol+=use_cnt;

		/* 客户购买 */	
		ret = sql_sum_long( "note_mst_hst" , "note_cnt" , &tmp_cnt , 
			" tx_date=%ld and %s and sts_ind ='6'" , 
			g_pub_tx.tx_date,Twherelist );
		ERR_DEAL		

		buy_cnt=tmp_cnt;
		buy_cnt_tol+=buy_cnt;

		/* 今日作废 */	
		ret = sql_sum_long( "note_mst_hst" , "note_cnt" , &tmp_cnt , 
			" tx_date=%ld and %s and sts_ind ='E'" , 
			g_pub_tx.tx_date,Twherelist );
		ERR_DEAL		

		dsy_cnt=tmp_cnt;
		dsy_cnt_tol+=dsy_cnt;

		/* 今日销号 */
		ret = sql_sum_long( "note_mst_hst" , "note_cnt" , &tmp_cnt , 
			" tx_date=%ld and %s and sts_ind ='H'" , 
			g_pub_tx.tx_date,Twherelist );
		ERR_DEAL		

		cls_cnt=tmp_cnt;
		cls_cnt_tol+=cls_cnt;

		/* 今日挂失 *
		ret = sql_sum_long( "note_mst_hst" , "note_cnt" , &tmp_cnt , 
			" tx_date=%ld and %s and sts_ind ='B'" , 
			g_pub_tx.tx_date,Twherelist );
		ERR_DEAL		

		lose_cnt=tmp_cnt;
		***/

		/* 今日解挂 *
		ret = sql_sum_long( "note_mst_hst" , "note_cnt" , &tmp_cnt , 
			" tx_date=%ld and %s and sts_ind ='C'" ,
			g_pub_tx.tx_date,Twherelist );
		ERR_DEAL		

		unlose_cnt=tmp_cnt;
		**/	

     		/* 统计昨日库存 *
		ys_cnt = now_cnt + out_cnt + use_cnt + buy_cnt + 
					 dsy_cnt + cls_cnt + lose_cnt + unlose_cnt - in_cnt;
		*/

		fprintf( fp, "%s|%s|%ld|%ld|%ld|%ld|%ld|%ld|%ld|\n",
		vnote_parm.note_type,vnote_parm.name,in_cnt,out_cnt,use_cnt,
		buy_cnt,dsy_cnt,cls_cnt,now_cnt);
		
		flag++;
	}
	Note_parm_Clo_Sel();

	if( !flag )
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
