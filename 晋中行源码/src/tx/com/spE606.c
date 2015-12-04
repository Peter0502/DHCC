/*************************************************
* ÎÄ ¼þ Ãû:  spE606.c
* ¹¦ÄÜÃèÊö£º ±¾ÈÕÔþÕË(²úÆ·)
*
* ×÷    Õß:  andy
* Íê³ÉÈÕÆÚ£º 2004-07-03
*
* ÐÞ¸Ä¼ÇÂ¼£º
* ÈÕ   ÆÚ:
* ÐÞ ¸Ä ÈË:
* ÐÞ¸ÄÄÚÈÝ:
*************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define EXTERN
#include "public.h"
#include "trace_log_c.h"
#include "mo_opn_cls_c.h"
#include "com_tel_c.h"

int spE606()
{

		char filename[100];
		char wherelist[1024];  /**²éÑ¯Ìõ¼þ**/
		char tmpstr[512];
		char tmpmo[512];
		char fieldlist[1024];  /**Òª²éÑ¯µÄ×Ö¶ÎÁÐ±í**/
		char tablelist[128];   /**Òª²éÑ¯µÄ±íÃû**/
		char titstr[1024];
		char tel[5];
		char old_prdt_no[4];
		char prdt_name[51];
		int ttlnum=0;			/**¶ÁÈ¡µÄ×ÜÌõÊý**/
		int i=0;
		int ret=0;
		
		double cash_amt_add=0.00,ch_amt_add=0.00;
		double cash_amt_sub=0.00,ch_amt_sub=0.00;
		long cash_cnt_add=0,ch_cnt_add=0;
		long cash_cnt_sub=0,ch_cnt_sub=0;

		double opn_amt_add=0.00,cls_amt_sub=0.00;
		long opn_cnt_add=0,cls_cnt_sub=0;
							
		FILE *fp;
	
	struct trace_log_c	v_trace_log;
	struct mo_opn_cls_c	v_mo_opn_cls;
	struct com_tel_c	v_com_tel;
		
	memset( &v_trace_log , 0x00 , sizeof(struct trace_log_c));
	memset( &v_mo_opn_cls , 0x00 , sizeof(struct mo_opn_cls_c));
	memset( &v_com_tel , 0x00 , sizeof(struct com_tel_c));
			
	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset( wherelist,0,sizeof(wherelist) );
	memset( fieldlist,0,sizeof(fieldlist) );
	memset( tablelist,0,sizeof(tablelist) );

	strcpy( old_prdt_no , "***" );
	
	pub_base_sysinit();

	get_zd_data("0910",tel );

	/**×é³É²éÑ¯Ìõ¼þ**/
	pub_base_strpack( tel );
	if( strlen(tel) )
	{
		/* ¼ì²é¹ñÔ±ºÅÊÇ·ñ´æÔÚ */
		ret = Com_tel_Sel(g_pub_tx.reply,&v_com_tel,"tel='%s'",tel);
		if(ret==100)
		{
			sprintf(acErrMsg,"´Ë¹ñÔ±ºÅ²»´æÔÚ![%s]",tel);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"O120" );			
			goto ErrExit;
		}else if(ret){
			sprintf(acErrMsg,"¶ÁÈ¡¹ñÔ±ÐÅÏ¢±íÒì³£³ö´í!");
			WRITEMSG
			strcpy( g_pub_tx.reply ,"O221" );			
			goto ErrExit;
		}									
		sprintf( tmpstr," tel='%s' and",tel );
		strcat( wherelist,tmpstr );
	}
	
	/* »ú¹¹±àÂë */
	sprintf( tmpstr," tx_br_no='%s' and",g_pub_tx.tx_br_no );
	strcat( wherelist,tmpstr );

	strcpy( tablelist,"trace_log" );
	
	/**×é³É²éÑ¯**/
	strcat(wherelist," sts='0' and no_show!='1' ORDER BY prdt_no ") ;

	vtcp_log( "WHERE [%s]",wherelist );

	ret=Trace_log_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

		ttlnum=0;
		
	while(1)
	{
		ret=Trace_log_Fet_Sel( &v_trace_log, g_pub_tx.reply );
		if( ret==100 ) 
		{
			if( ttlnum )
			{
				/* ²é³ö²úÆ·Ãû³Æ */
	TRACE			
				
				ret = pub_base_prdt_name( old_prdt_no, prdt_name );	
				ERR_DEAL
				pub_base_strpack( prdt_name );	
	TRACE			
				/* Í³¼Æ¿ªÏú»§±ÊÊýÒÔ¼°½ð¶î */
				pub_base_strpack( tel );
				if( strlen(tel) )	/* ¹ñÔ±ºÅ·Ç¿Õ */
				{
					opn_cnt_add = sql_count( "mo_opn_cls" , 
	                              "prdt_no='%s' and wrk_sts='1' and tel='%s' and tx_date=%d and tx_brno='%s'" , 
                                   old_prdt_no,tel,g_pub_tx.tx_date,g_pub_tx.tx_br_no);
					cls_cnt_sub = sql_count( "mo_opn_cls" , 
                                  "prdt_no='%s' and wrk_sts='2' and tel='%s' and tx_date=%d and tx_brno='%s'" , 
                                  old_prdt_no,tel,g_pub_tx.tx_date,g_pub_tx.tx_br_no);
					
					ret = sql_sum_double( "mo_opn_cls" , "amt" , &opn_amt_add , 
                          "prdt_no='%s' and wrk_sts='1' and tel='%s' and tx_date=%d and tx_brno='%s'" , 
                          old_prdt_no,tel,g_pub_tx.tx_date,g_pub_tx.tx_br_no);
					ERR_DEAL				
					ret = sql_sum_double( "mo_opn_cls" , "amt" , &cls_amt_sub , 
                         "prdt_no='%s' and wrk_sts='2' and tel='%s' and tx_date=%d and tx_brno='%s' " , 
                         old_prdt_no,tel,g_pub_tx.tx_date,g_pub_tx.tx_br_no);			
					ERR_DEAL			
				}else{			/* ¹ñÔ±ºÅÎª¿Õ */
					opn_cnt_add = sql_count( "mo_opn_cls" , 
                                  "prdt_no='%s' and wrk_sts='1' and tx_date=%d and tx_brno='%s'" , 
                                  old_prdt_no,g_pub_tx.tx_date,g_pub_tx.tx_br_no);
					cls_cnt_sub = sql_count( "mo_opn_cls" , 
                                  "prdt_no='%s' and wrk_sts='2' and tx_date=%d and tx_brno='%s'" , 
                                  old_prdt_no,g_pub_tx.tx_date,g_pub_tx.tx_br_no);
					
					ret = sql_sum_double( "mo_opn_cls" , "amt" , &opn_amt_add , 
                          "prdt_no='%s' and wrk_sts='1' and tx_date=%d and tx_brno='%s'" , 
                          old_prdt_no,g_pub_tx.tx_date,g_pub_tx.tx_br_no);
					ERR_DEAL				
					ret = sql_sum_double( "mo_opn_cls" , "amt" , &cls_amt_sub , 
                          "prdt_no='%s' and wrk_sts='2' and tx_date=%d and tx_brno='%s'" , 
                          old_prdt_no,g_pub_tx.tx_date,g_pub_tx.tx_br_no);			
					ERR_DEAL
				}
				if( pub_base_CompDblVal( cash_amt_add+ch_amt_add+cash_amt_sub+ch_amt_sub+cash_cnt_add+ch_cnt_add+cash_cnt_sub+ch_cnt_sub+opn_amt_add+cls_amt_sub+opn_cnt_add+cls_cnt_sub , 0.00 )!=0 && strcmp( old_prdt_no , "ÁÙ")!=0)		
				{					
					/**ÏÔÊ¾ÁÐ±êÌâ**/
					fprintf( fp,"@3v1\n");
					fprintf( fp,"`     ²úÆ·±àºÅ: ¡¾%3s¡¿\n", old_prdt_no );
					fprintf( fp,"`     ²úÆ·Ãû³Æ: ¡¾%s¡¿\n", prdt_name );
					/***	fprintf( fp,"`\n" );	**/
					fprintf( fp,"`    ©±©¥©¥©Ó©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©Ó©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©µ\n" );
					fprintf( fp,"`    ©¦    ©¦          Ôö       ¼Ó       ©¦           ¼õ      ÉÙ       ©¦\n" );
					fprintf( fp,"`    ©¦    ©Á©¥©¥©¥©¥©¥©¥©¥©¥©Ó©¥©¥©¥©¥©¥©ã©¥©¥©¥©¥©¥©¥©¥©¥©Ó©¥©¥©¥©¥©¥©É\n" );										
					fprintf( fp,"`    ©¦    ©¦   ½ð      ¶î   ©¦  ±Ê  Êý  ©¦   ½ð      ¶î   ©¦  ±Ê  Êý  ©¦\n" );
					fprintf( fp,"`    ©Á©¥©¥©ã©¥©¥©¥©¥©¥©¥©¥©¥©ã©¥©¥©¥©¥©¥©ã©¥©¥©¥©¥©¥©¥©¥©¥©ã©¥©¥©¥©¥©¥©É\n" );
					fprintf( fp,"`    ©¦ÏÖ½ð©¦%16.2lf©¦%10d©¦%16.2lf©¦%10d©¦\n" ,cash_amt_add,cash_cnt_add,cash_amt_sub,cash_cnt_sub );
					fprintf( fp,"`    ©Á©¥©¥©ã©¥©¥©¥©¥©¥©¥©¥©¥©ã©¥©¥©¥©¥©¥©ã©¥©¥©¥©¥©¥©¥©¥©¥©ã©¥©¥©¥©¥©¥©É\n" );
					fprintf( fp,"`    ©¦×ªÕË©¦%16.2lf©¦%10d©¦%16.2lf©¦%10d©¦\n" ,ch_amt_add,ch_cnt_add,ch_amt_sub,ch_cnt_sub);
					fprintf( fp,"`    ©Á©¥©¥©ã©¥©¥©¥©¥©¥©¥©¥©¥©ã©¥©¥©¥©¥©¥©ã©¥©¥©¥©¥©¥©¥©¥©¥©ã©¥©¥©¥©¥©¥©É\n" );
					fprintf( fp,"`    ©¦ºÏ¼Æ©¦%16.2lf©¦%10d©¦%16.2lf©¦%10d©¦\n" ,cash_amt_add+ch_amt_add,cash_cnt_add+ch_cnt_add,cash_amt_sub+ch_amt_sub,cash_cnt_sub+ch_cnt_sub);
					fprintf( fp,"`    ©¹©¥©¥©Û©¥©¥©¥©¥©¥©¥©¥©¥©Û©¥©¥©¥©¥©¥©Û©¥©¥©¥©¥©¥©¥©¥©¥©Û©¥©¥©¥©¥©¥©½\n" );		
					fprintf( fp,"`    ©±©¥©¥©Ó©¥©¥©¥©¥©¥©¥©¥©¥©Ó©¥©¥©¥©¥©¥©µ\n" );
					fprintf( fp,"`    ©¦¿ª»§©¦%16.2lf©¦%10d©¦\n" ,opn_amt_add,opn_cnt_add);			
					fprintf( fp,"`    ©Á©¥©¥©ã©¥©¥©¥©¥©¥©¥©¥©¥©ã©¥©¥©¥©¥©¥©É\n" );			
					fprintf( fp,"`    ©¦Ïú»§©¦%16.2lf©¦%10d©¦\n" ,cls_amt_sub,cls_cnt_sub);			
					fprintf( fp,"`    ©¹©¥©¥©Û©¥©¥©¥©¥©¥©¥©¥©¥©Û©¥©¥©¥©¥©¥©½\n" );	
					/***fprintf( fp,"`    ©¹©¥©¥©Û©¥©¥©¥©¥©¥©¥©¥©¥©Û©¥©¥©¥©¥©¥©Û©¥©¥©Û©¥©¥©¥©¥©¥©¥©¥©¥©Û©¥©¥©¥©¥©¥©½\n" );*******/
					/***fprintf( fp,"`    ©¦¿ª»§©¦ %15.2lf©¦%10d©¦Ïú»§©¦ %15.2lf©¦%10d©¦\n" ,opn_amt_add,opn_cnt_add,cls_amt_sub,cls_cnt_sub);****/			
				}
				strcpy( old_prdt_no , v_trace_log.prdt_no);	/* ²úÆ·±àºÅ */ 	
				
				/* Êý¾ÝÇåÁã */
				cash_amt_add=0.00;
				ch_amt_add=0.00;
				cash_amt_sub=0.00;
				ch_amt_sub=0.00;
				cash_cnt_add=0;
				ch_cnt_add=0;
				cash_cnt_sub=0;
				ch_cnt_sub=0;
	
				opn_amt_add=0.00;
				cls_amt_sub=0.00;
				opn_cnt_add=0;
				cls_cnt_sub=0;			
			}
			
			break;
			
		}else if(ret){
			goto ErrExit;
		}
		

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
		}
		TRACE
		
		/* Èô²úÆ·ºÅÎª¿Õ */		
		if( strlen(v_trace_log.prdt_no)==0 )
		{
			strcpy( v_trace_log.prdt_no , "ÁÙ" );
		}
		
		/* µÚÒ»Ìõ¼ÇÂ¼ */
		if( strcmp( old_prdt_no , "***")==0 )
		{
			TRACE
			strcpy( old_prdt_no, v_trace_log.prdt_no );
		}		
		
		
		if( strcmp( v_trace_log.prdt_no , old_prdt_no ) )	/* ²úÆ·±ä»¯Ê± */
		{
			/* ²é³ö²úÆ·Ãû³Æ */
TRACE			
			
			ret = pub_base_prdt_name( old_prdt_no, prdt_name );	
			ERR_DEAL
			pub_base_strpack( prdt_name );	
TRACE			
			/* Í³¼Æ¿ªÏú»§±ÊÊýÒÔ¼°½ð¶î */
			pub_base_strpack( tel );
			if( strlen(tel) )	/* ¹ñÔ±ºÅ·Ç¿Õ */
			{
				opn_cnt_add = sql_count( "mo_opn_cls" , 
	                          "prdt_no='%s' and wrk_sts='1' and tel='%s' and tx_date=%d and tx_brno='%s'" , 
                               old_prdt_no,tel,g_pub_tx.tx_date,g_pub_tx.tx_br_no);
				cls_cnt_sub = sql_count( "mo_opn_cls" , 
                              "prdt_no='%s' and wrk_sts='2' and tel='%s' and tx_date=%d and tx_brno='%s'" , 
                              old_prdt_no,tel,g_pub_tx.tx_date,g_pub_tx.tx_br_no);
				
					ret = sql_sum_double( "mo_opn_cls" , "amt" , &opn_amt_add , 
                          "prdt_no='%s' and wrk_sts='1' and tel='%s' and tx_date=%d and tx_brno='%s'" , 
                          old_prdt_no,tel,g_pub_tx.tx_date,g_pub_tx.tx_br_no);
					ERR_DEAL				
					ret = sql_sum_double( "mo_opn_cls" , "amt" , &cls_amt_sub , 
                         "prdt_no='%s' and wrk_sts='2' and tel='%s' and tx_date=%d and tx_brno='%s' " , 
                         old_prdt_no,tel,g_pub_tx.tx_date,g_pub_tx.tx_br_no);			
					ERR_DEAL			
			}else{			/* ¹ñÔ±ºÅÎª¿Õ */
				opn_cnt_add = sql_count( "mo_opn_cls" , 
                                 "prdt_no='%s' and wrk_sts='1' and tx_date=%d and tx_brno='%s'" , 
                                 old_prdt_no,g_pub_tx.tx_date,g_pub_tx.tx_br_no);
				cls_cnt_sub = sql_count( "mo_opn_cls" , 
                                  "prdt_no='%s' and wrk_sts='2' and tx_date=%d and tx_brno='%s'" , 
                                  old_prdt_no,g_pub_tx.tx_date,g_pub_tx.tx_br_no);
				
					ret = sql_sum_double( "mo_opn_cls" , "amt" , &opn_amt_add , 
                          "prdt_no='%s' and wrk_sts='1' and tx_date=%d and tx_brno='%s'" , 
                          old_prdt_no,g_pub_tx.tx_date,g_pub_tx.tx_br_no);
					ERR_DEAL				
					ret = sql_sum_double( "mo_opn_cls" , "amt" , &cls_amt_sub , 
                          "prdt_no='%s' and wrk_sts='2' and tx_date=%d and tx_brno='%s'" , 
                          old_prdt_no,g_pub_tx.tx_date,g_pub_tx.tx_br_no);			
					ERR_DEAL
			}
			
			if( pub_base_CompDblVal( cash_amt_add+ch_amt_add+cash_amt_sub+ch_amt_sub+cash_cnt_add+ch_cnt_add+cash_cnt_sub+ch_cnt_sub+opn_amt_add+cls_amt_sub+opn_cnt_add+cls_cnt_sub , 0.00 )!=0 && strcmp( old_prdt_no , "ÁÙ")!=0)		
			{
				/**ÏÔÊ¾ÁÐ±êÌâ**/
				fprintf( fp,"@3v1\n");
				fprintf( fp,"`     ²úÆ·±àºÅ: ¡¾%3s¡¿\n", old_prdt_no );
				fprintf( fp,"`     ²úÆ·Ãû³Æ: ¡¾%s¡¿\n", prdt_name );
				/***	fprintf( fp,"`\n" );	**/
				fprintf( fp,"`    ©±©¥©¥©Ó©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©Ó©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©µ\n" );
				fprintf( fp,"`    ©¦    ©¦          Ôö       ¼Ó       ©¦           ¼õ      ÉÙ       ©¦\n" );
				fprintf( fp,"`    ©¦    ©Á©¥©¥©¥©¥©¥©¥©¥©¥©Ó©¥©¥©¥©¥©¥©ã©¥©¥©¥©¥©¥©¥©¥©¥©Ó©¥©¥©¥©¥©¥©É\n" );										
				fprintf( fp,"`    ©¦    ©¦   ½ð      ¶î   ©¦  ±Ê  Êý  ©¦   ½ð      ¶î   ©¦  ±Ê  Êý  ©¦\n" );
				fprintf( fp,"`    ©Á©¥©¥©ã©¥©¥©¥©¥©¥©¥©¥©¥©ã©¥©¥©¥©¥©¥©ã©¥©¥©¥©¥©¥©¥©¥©¥©ã©¥©¥©¥©¥©¥©É\n" );
				fprintf( fp,"`    ©¦ÏÖ½ð©¦%16.2lf©¦%10d©¦%16.2lf©¦%10d©¦\n" ,cash_amt_add,cash_cnt_add,cash_amt_sub,cash_cnt_sub );
				fprintf( fp,"`    ©Á©¥©¥©ã©¥©¥©¥©¥©¥©¥©¥©¥©ã©¥©¥©¥©¥©¥©ã©¥©¥©¥©¥©¥©¥©¥©¥©ã©¥©¥©¥©¥©¥©É\n" );
				fprintf( fp,"`    ©¦×ªÕË©¦%16.2lf©¦%10d©¦%16.2lf©¦%10d©¦\n" ,ch_amt_add,ch_cnt_add,ch_amt_sub,ch_cnt_sub);
				fprintf( fp,"`    ©Á©¥©¥©ã©¥©¥©¥©¥©¥©¥©¥©¥©ã©¥©¥©¥©¥©¥©ã©¥©¥©¥©¥©¥©¥©¥©¥©ã©¥©¥©¥©¥©¥©É\n" );
				fprintf( fp,"`    ©¦ºÏ¼Æ©¦%16.2lf©¦%10d©¦%16.2lf©¦%10d©¦\n" ,cash_amt_add+ch_amt_add,cash_cnt_add+ch_cnt_add,cash_amt_sub+ch_amt_sub,cash_cnt_sub+ch_cnt_sub);
				fprintf( fp,"`    ©¹©¥©¥©Û©¥©¥©¥©¥©¥©¥©¥©¥©Û©¥©¥©¥©¥©¥©Û©¥©¥©¥©¥©¥©¥©¥©¥©Û©¥©¥©¥©¥©¥©½\n" );		
				fprintf( fp,"`    ©±©¥©¥©Ó©¥©¥©¥©¥©¥©¥©¥©¥©Ó©¥©¥©¥©¥©¥©µ\n" );
				fprintf( fp,"`    ©¦¿ª»§©¦%16.2lf©¦%10d©¦\n" ,opn_amt_add,opn_cnt_add);			
				fprintf( fp,"`    ©Á©¥©¥©ã©¥©¥©¥©¥©¥©¥©¥©¥©ã©¥©¥©¥©¥©¥©É\n" );			
				fprintf( fp,"`    ©¦Ïú»§©¦%16.2lf©¦%10d©¦\n" ,cls_amt_sub,cls_cnt_sub);			
				fprintf( fp,"`    ©¹©¥©¥©Û©¥©¥©¥©¥©¥©¥©¥©¥©Û©¥©¥©¥©¥©¥©½\n" );	
				/***fprintf( fp,"`    ©¹©¥©¥©Û©¥©¥©¥©¥©¥©¥©¥©¥©Û©¥©¥©¥©¥©¥©Û©¥©¥©Û©¥©¥©¥©¥©¥©¥©¥©¥©Û©¥©¥©¥©¥©¥©½\n" );*******/
				/***fprintf( fp,"`    ©¦¿ª»§©¦ %15.2lf©¦%10d©¦Ïú»§©¦ %15.2lf©¦%10d©¦\n" ,opn_amt_add,opn_cnt_add,cls_amt_sub,cls_cnt_sub);****/			
			}
/*************	
©Ó©§©ã©¹ ©Û ©É©Á	
***********/		
TRACE

			
			strcpy( old_prdt_no , v_trace_log.prdt_no);	/* ²úÆ·±àºÅ */ 	
			
			/* Êý¾ÝÇåÁã */
			cash_amt_add=0.00;
			ch_amt_add=0.00;
			cash_amt_sub=0.00;
			ch_amt_sub=0.00;
			cash_cnt_add=0;
			ch_cnt_add=0;
			cash_cnt_sub=0;
			ch_cnt_sub=0;

			opn_amt_add=0.00;
			cls_amt_sub=0.00;
			opn_cnt_add=0;
			cls_cnt_sub=0;			
		}
		
		
		/* Í³¼ÆÐÅÏ¢ */
		if( pub_base_CompDblVal( v_trace_log.amt , 0.00 ) )
		{
			if( v_trace_log.ct_ind[0]=='1' )		/* ÏÖ½ð */
			{
				if( v_trace_log.add_ind[0]=='1')	/* Ôö¼Ó */
				{
					cash_amt_add+=v_trace_log.amt;
					cash_cnt_add++;
				}
			}
			if( v_trace_log.ct_ind[0]=='1' )		/* ÏÖ½ð */
			{
				if( v_trace_log.add_ind[0]=='0')	/* ¼õÉÙ */
				{
					cash_amt_sub+=v_trace_log.amt;
					cash_cnt_sub++;
				}
			}
			if( v_trace_log.ct_ind[0]=='2' )		/* ×ªÕË */
			{
				if( v_trace_log.add_ind[0]=='1')	/* Ôö¼Ó */
				{
					ch_amt_add+=v_trace_log.amt;
					ch_cnt_add++;
				}
			}
			if( v_trace_log.ct_ind[0]=='2' )		/* ×ªÕË */
			{
				if( v_trace_log.add_ind[0]=='0')	/* ¼õÉÙ */
				{
					ch_amt_sub+=v_trace_log.amt;
					ch_cnt_sub++;
				}
			}
		}
		/* Í³¼Æ¿ªÏú»§±ÊÊýÒÔ¼°½ð¶î *			
		ret = Mo_opn_cls_Sel( g_pub_tx.reply , &v_mo_opn_cls , "tx_date=%ld and trace_no=%ld" , g_pub_tx.tx_date,v_trace_log.trace_no);
		if( ret && ret!=100 )
		{
			sprintf(acErrMsg,"È¡¿ªÏú»§µÇ¼Ç±¡ÐÅÏ¢Òì³£³ö´í![%ld][%ld]",g_pub_tx.tx_date,v_trace_log.trace_no);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"L178" );			
			goto ErrExit;
		}else if( ret==0 ){
			if( v_mo_opn_cls.wrk_sts[0]=='1')	* ¿ª»§ *
			{
				opn_amt_add+=v_mo_opn_cls.amt;
				opn_cnt_add++;
			}
			if( v_mo_opn_cls.wrk_sts[0]=='2')	* Ïú»§ *
			{
				cls_amt_sub+=v_mo_opn_cls.amt;
				cls_cnt_sub++;
			}		
		}
		********/
		

		ttlnum++;
	}

	ret=Trace_log_Clo_Sel( );
	ERR_DEAL
TRACE
	if( !ttlnum )
	{
		strcpy( g_pub_tx.reply,"S049" );
		goto ErrExit;
	}else{
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
