/*************************************************
* ÎÄ ¼þ Ãû:  spE607.c
* ¹¦ÄÜÃèÊö£º ±¾ÈÕÔþÕË(¿ÆÄ¿)
*
* ×÷    Õß:  andy
* Íê³ÉÈÕÆÚ£º 2004-07-04
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
#include "com_item_c.h"
#include "com_tel_c.h"
#include "mo_dc_inf_c.h"
#include "dc_log_c.h"

int spE607()
{

		char filename[100];
		char wherelist[1024];  /**²éÑ¯Ìõ¼þ**/
		char tmpstr[512];
		char tmpmo[512];

		char titstr[1024];
		char tel[5];
		char old_acc_hrt[6];
		
		int ttlnum=0;			/**¶ÁÈ¡µÄ×ÜÌõÊý**/
		int num=0;	
		int i=0;
		int ret=0;
		
		double	dr_cash_amt=0.00;
		long	dr_cash_cnt=0;
		double	dr_ch_amt=0.00;
		long	dr_ch_cnt=0;
		double	dc_cash_amt=0.00;
		long	dc_cash_cnt=0;
		double	dc_ch_amt=0.00;
		long	dc_ch_cnt=0;
		
		char old_acc_no_lvl1[8],old_acc_no_lvl2[8],old_acc_no_lvl3[8];
									
		FILE *fp;
	
	struct dc_log_c		v_dc_log;
	struct com_item_c	v_com_item;
	struct com_tel_c	v_com_tel;
	struct com_item_c	v_com_item_lvl1;
	struct com_item_c	v_com_item_lvl2;
	struct com_item_c	v_com_item_lvl3;
	struct mo_dc_inf_c	v_mo_dc_inf;
				
	memset( &v_dc_log , 0x00 , sizeof(struct trace_log_c));
	memset( &v_com_item , 0x00 , sizeof(struct com_item_c));
	memset( &v_com_tel , 0x00 , sizeof(struct com_tel_c));
	memset( &v_com_item_lvl1 , 0x00 , sizeof(struct com_item_c));
	memset( &v_com_item_lvl2 , 0x00 , sizeof(struct com_item_c));
	memset( &v_com_item_lvl3 , 0x00 , sizeof(struct com_item_c));
	memset( &v_mo_dc_inf , 0x00 , sizeof(struct mo_dc_inf_c));
						
	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset( wherelist,0,sizeof(wherelist) );
	memset( old_acc_no_lvl1,0,sizeof(old_acc_no_lvl1) );
	memset( old_acc_no_lvl2,0,sizeof(old_acc_no_lvl2) );
	memset( old_acc_no_lvl3,0,sizeof(old_acc_no_lvl3) );
		
	strcpy( old_acc_hrt , "*****" );
		
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
	sprintf( tmpstr," tx_tx_br_no='%s' and",g_pub_tx.tx_br_no );
	strcat( wherelist,tmpstr );

	/* ½»Ò×ÈÕÆÚ */
	sprintf( tmpstr," tx_date=%ld and",g_pub_tx.tx_date );
	strcat( wherelist,tmpstr );
		
	/**×é³É²éÑ¯**/
	strcat(wherelist," sts='0' ORDER BY acc_hrt ") ;

	vtcp_log( "WHERE [%s]",wherelist );
	

	ret = sql_execute( "delete from mo_dc_inf where br_no='%s' and tel='%s' " , g_pub_tx.tx_br_no , g_pub_tx.tel );
	vtcp_log("ret=[%d]",ret);
        if(ret==-1) ret=0;
	ERR_DEAL	
	
	/* Í³¼ÆÊý¾Ý¹ý³Ì */
	ret=Dc_log_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

		ttlnum=0;
	
	while(1)
	{
		sprintf(acErrMsg,">>>>>>>>>>>>>>>>>>>>>>>>>>>>");
		WRITEMSG
		ret=Dc_log_Fet_Sel( &v_dc_log, g_pub_tx.reply);
		if( ret==100 ) 
		{
			sprintf(acErrMsg,"+++++++++++++++++++++ttlnum====[%d]",ttlnum);
		        WRITEMSG
			if( ttlnum )
			{
				/* ²é³ö¿ÆÄ¿»ù±¾ÐÅÏ¢ */
				ret =pub_base_item_inf( old_acc_hrt, &v_com_item );
				ERR_DEAL
				
				v_mo_dc_inf.dr_cash_amt=dr_cash_amt;				/* ½è·½ÏÖ½ð½ð¶î */				
				v_mo_dc_inf.dr_cash_cnt=dr_cash_cnt;				/* ½è·½ÏÖ½ð±ÊÊý */
				v_mo_dc_inf.dr_ch_amt=dr_ch_amt;				/* ½è·½×ªÕË½ð¶î */
				v_mo_dc_inf.dr_ch_cnt=dr_ch_cnt;				/* ½è·½×ªÕË±ÊÊý */
				v_mo_dc_inf.dc_cash_amt=dc_cash_amt;				/* ´û·½ÏÖ½ð½ð¶î */
				v_mo_dc_inf.dc_cash_cnt=dc_cash_cnt;				/* ´û·½ÏÖ½ð±ÊÊý */
				v_mo_dc_inf.dc_ch_amt=dc_ch_amt;				/* ´û·½×ªÕË½ð¶î */
				v_mo_dc_inf.dc_ch_cnt=dc_ch_cnt;				/* ´û·½×ªÕË±ÊÊý */
				if( v_com_item.acc_no[0]>'5' )					/* ±íÄÚ/±íÍâ±êÖ¾ */
				{
					strcpy( v_mo_dc_inf.sts , "1" );
				}else{
					strcpy( v_mo_dc_inf.sts , "0" );
				}
				strcpy( v_mo_dc_inf.tel , g_pub_tx.tel );			/* ¹ñÔ±±àºÅ */
				strcpy( v_mo_dc_inf.br_no , g_pub_tx.tx_br_no );		/* »ú¹¹±àºÅ */	
				sprintf(acErrMsg,"¿ÆÄ¿ºÅ[%s]¼¶±ð[%s]!",v_com_item.acc_no,v_com_item.acc_lvl);
				WRITEMSG				
				/* Èý¼¶¿ÆÄ¿ */
				if( v_com_item.acc_lvl[0]=='3' )
				{
					strcpy( v_mo_dc_inf.acc_no_lvl3 , v_com_item.acc_no );		/* Èý¼¶¿ÆÄ¿ºÅ */
					strcpy( v_mo_dc_inf.acc_name_lvl3 , v_com_item.acc_name );	/* Èý¼¶¿ÆÄ¿Ãû³Æ */
					
					/* ²é¸Ã¿ÆÄ¿¶þ¼¶¿ÆÄ¿ÐÅÏ¢ */                 
					ret =pub_base_item_inf( v_com_item.up_acc_hrt, &v_com_item_lvl2 );
					ERR_DEAL	
					strcpy( v_mo_dc_inf.acc_no_lvl2 , v_com_item_lvl2.acc_no );	/* ¶þ¼¶¿ÆÄ¿ºÅ */
					strcpy( v_mo_dc_inf.acc_name_lvl2 , v_com_item_lvl2.acc_name );	/* ¶þ¼¶¿ÆÄ¿Ãû³Æ */
					
					/* ²é¸Ã¿ÆÄ¿Ò»¼¶¿ÆÄ¿ÐÅÏ¢ */
					ret =pub_base_item_inf( v_com_item_lvl2.up_acc_hrt, &v_com_item_lvl1 );
					ERR_DEAL
					sprintf(acErrMsg,"Ò»¼¶¿ÆÄ¿[%s]¶þ¼¶ÉÏ¼¶[%s]!",v_com_item_lvl1.acc_no,v_com_item_lvl2.up_acc_hrt);
					WRITEMSG
											
					strcpy( v_mo_dc_inf.acc_no_lvl1 , v_com_item_lvl1.acc_no );	/* Ò»¼¶¿ÆÄ¿ºÅ */
					strcpy( v_mo_dc_inf.acc_name_lvl1 , v_com_item_lvl1.acc_name );	/* Ò»¼¶¿ÆÄ¿Ãû³Æ */												
				
				}else if( v_com_item.acc_lvl[0]=='2' ){		/* ¶þ¼¶¿ÆÄ¿ */
					strcpy( v_mo_dc_inf.acc_no_lvl2 , v_com_item.acc_no );		/* ¶þ¼¶¿ÆÄ¿ºÅ */
					strcpy( v_mo_dc_inf.acc_name_lvl2 , v_com_item.acc_name );	/* ¶þ¼¶¿ÆÄ¿Ãû³Æ */					

					/* ²é¸Ã¿ÆÄ¿Ò»¼¶¿ÆÄ¿ÐÅÏ¢ */
					ret =pub_base_item_inf( v_com_item.up_acc_hrt, &v_com_item_lvl1 );
					ERR_DEAL	
					strcpy( v_mo_dc_inf.acc_no_lvl1 , v_com_item_lvl1.acc_no );	/* Ò»¼¶¿ÆÄ¿ºÅ */
					strcpy( v_mo_dc_inf.acc_name_lvl1 , v_com_item_lvl1.acc_name );	/* Ò»¼¶¿ÆÄ¿Ãû³Æ */												
				}else if( v_com_item.acc_lvl[0]=='1' ){		/* Ò»¼¶¿ÆÄ¿ */
					strcpy( v_mo_dc_inf.acc_no_lvl1 , v_com_item.acc_no );		/* Ò»¼¶¿ÆÄ¿ºÅ */
					strcpy( v_mo_dc_inf.acc_name_lvl1 , v_com_item.acc_name );	/* Ò»¼¶¿ÆÄ¿Ãû³Æ */
				}else{
					sprintf(acErrMsg,"ÎÞ´Ë¼¶±ð¿ÆÄ¿ºÅ!");
					WRITEMSG
					strcpy( g_pub_tx.reply ,"S056" );			
					goto ErrExit;		
				}								
								
				/* µÇ¼ÇÍ³¼Æ±í */

				
				ret = Mo_dc_inf_Ins( v_mo_dc_inf , g_pub_tx.reply );
				if( ret )
				{
					sprintf(acErrMsg,"µÇ¼ÇÔþÕË¿ÆÄ¿Í³¼Æ±íÒì³£³ö´í,ÇëÓëÖÐÐÄ»ú·¿ÁªÏµ!");
					WRITEMSG
					strcpy( g_pub_tx.reply ,"G106" );			
					goto ErrExit;					
				}
				
				strcpy( old_acc_hrt , v_dc_log.acc_hrt);	/* ¿ÆÄ¿¿ØÖÆ×Ö */ 	
				
				/* Êý¾ÝÇåÁã */
				dr_cash_amt=0.00;
				dr_cash_cnt=0;
				dr_ch_amt=0.00;
				dr_ch_cnt=0;
				dc_cash_amt=0.00;
				dc_cash_cnt=0;
				dc_ch_amt=0.00;
				dc_ch_cnt=0;
				memset( &v_com_tel , 0x00 , sizeof(struct com_tel_c));
				memset( &v_com_item_lvl1 , 0x00 , sizeof(struct com_item_c));
				memset( &v_com_item_lvl2 , 0x00 , sizeof(struct com_item_c));
				memset( &v_com_item_lvl3 , 0x00 , sizeof(struct com_item_c));
				memset( &v_mo_dc_inf , 0x00 , sizeof(struct mo_dc_inf_c));				
			}
			sprintf(acErrMsg,"-----------------------------");
			WRITEMSG
			break;
			
		}else if(ret){
			goto ErrExit;
		}
		
		sprintf(acErrMsg,">>>>>>>>>>>>>>>>>>>>>>>>>>>>");
		WRITEMSG
		/* Èô¿ÆÄ¿¿ØÖÆ×ÖÎª¿Õ */		
		if( strlen(v_dc_log.acc_hrt)==0 )
		{
			strcpy( v_dc_log.acc_hrt , "ÁÙ" );
		}
		
		/* µÚÒ»Ìõ¼ÇÂ¼ */
		if( strcmp( old_acc_hrt , "*****")==0 )
		{
			TRACE
			strcpy( old_acc_hrt, v_dc_log.acc_hrt );
		}		
		
		
		if( strcmp( v_dc_log.acc_hrt , old_acc_hrt ) )	/* ¿ÆÄ¿¿ØÖÆ×Ö±ä»¯Ê± */
		{
		
	sprintf(acErrMsg,"¿ÆÄ¿¿ØÖÆ×Ö±ä»¯Ê±");
	WRITEMSG
			/* ²é³ö¿ÆÄ¿»ù±¾ÐÅÏ¢ */
			ret =pub_base_item_inf( old_acc_hrt, &v_com_item );
			ERR_DEAL
						
			v_mo_dc_inf.dr_cash_amt=dr_cash_amt;				/* ½è·½ÏÖ½ð½ð¶î */				
			v_mo_dc_inf.dr_cash_cnt=dr_cash_cnt;				/* ½è·½ÏÖ½ð±ÊÊý */
			v_mo_dc_inf.dr_ch_amt=dr_ch_amt;				/* ½è·½×ªÕË½ð¶î */
			v_mo_dc_inf.dr_ch_cnt=dr_ch_cnt;				/* ½è·½×ªÕË±ÊÊý */
			v_mo_dc_inf.dc_cash_amt=dc_cash_amt;				/* ´û·½ÏÖ½ð½ð¶î */
			v_mo_dc_inf.dc_cash_cnt=dc_cash_cnt;				/* ´û·½ÏÖ½ð±ÊÊý */
			v_mo_dc_inf.dc_ch_amt=dc_ch_amt;				/* ´û·½×ªÕË½ð¶î */
			v_mo_dc_inf.dc_ch_cnt=dc_ch_cnt;				/* ´û·½×ªÕË±ÊÊý */
			if( v_com_item.acc_no[0]>'5' )					/* ±íÄÚ/±íÍâ±êÖ¾ */
			{
				strcpy( v_mo_dc_inf.sts , "1" );
			}else{
				strcpy( v_mo_dc_inf.sts , "0" );
			}
			strcpy( v_mo_dc_inf.tel , g_pub_tx.tel );			/* ¹ñÔ±±àºÅ */
			strcpy( v_mo_dc_inf.br_no , g_pub_tx.tx_br_no );		/* »ú¹¹±àºÅ */	
			
			/* Èý¼¶¿ÆÄ¿ */
			if( v_com_item.acc_lvl[0]=='3' )
			{
				strcpy( v_mo_dc_inf.acc_no_lvl3 , v_com_item.acc_no );		/* Èý¼¶¿ÆÄ¿ºÅ */
				strcpy( v_mo_dc_inf.acc_name_lvl3 , v_com_item.acc_name );	/* Èý¼¶¿ÆÄ¿Ãû³Æ */
				
				/* ²é¸Ã¿ÆÄ¿¶þ¼¶¿ÆÄ¿ÐÅÏ¢ */
				ret =pub_base_item_inf( v_com_item.up_acc_hrt, &v_com_item_lvl2 );
				ERR_DEAL	
				strcpy( v_mo_dc_inf.acc_no_lvl2 , v_com_item_lvl2.acc_no );	/* ¶þ¼¶¿ÆÄ¿ºÅ */
				strcpy( v_mo_dc_inf.acc_name_lvl2 , v_com_item_lvl2.acc_name );	/* ¶þ¼¶¿ÆÄ¿Ãû³Æ */
				
				/* ²é¸Ã¿ÆÄ¿Ò»¼¶¿ÆÄ¿ÐÅÏ¢ */
				ret =pub_base_item_inf( v_com_item_lvl2.up_acc_hrt, &v_com_item_lvl1 );
				ERR_DEAL	
				strcpy( v_mo_dc_inf.acc_no_lvl1 , v_com_item_lvl1.acc_no );	/* Ò»¼¶¿ÆÄ¿ºÅ */
				strcpy( v_mo_dc_inf.acc_name_lvl1 , v_com_item_lvl1.acc_name );	/* Ò»¼¶¿ÆÄ¿Ãû³Æ */												
			
			}else if( v_com_item.acc_lvl[0]=='2' ){		/* ¶þ¼¶¿ÆÄ¿ */
				strcpy( v_mo_dc_inf.acc_no_lvl2 , v_com_item.acc_no );		/* ¶þ¼¶¿ÆÄ¿ºÅ */
				strcpy( v_mo_dc_inf.acc_name_lvl2 , v_com_item.acc_name );	/* ¶þ¼¶¿ÆÄ¿Ãû³Æ */					

				/* ²é¸Ã¿ÆÄ¿Ò»¼¶¿ÆÄ¿ÐÅÏ¢ */
				ret =pub_base_item_inf( v_com_item.up_acc_hrt, &v_com_item_lvl1 );
				ERR_DEAL	
				strcpy( v_mo_dc_inf.acc_no_lvl1 , v_com_item_lvl1.acc_no );	/* Ò»¼¶¿ÆÄ¿ºÅ */
				strcpy( v_mo_dc_inf.acc_name_lvl1 , v_com_item_lvl1.acc_name );	/* Ò»¼¶¿ÆÄ¿Ãû³Æ */												
			}else if( v_com_item.acc_lvl[0]=='1' ){		/* Ò»¼¶¿ÆÄ¿ */
				strcpy( v_mo_dc_inf.acc_no_lvl1 , v_com_item.acc_no );		/* Ò»¼¶¿ÆÄ¿ºÅ */
				strcpy( v_mo_dc_inf.acc_name_lvl1 , v_com_item.acc_name );	/* Ò»¼¶¿ÆÄ¿Ãû³Æ */
			}else{
				sprintf(acErrMsg,"ÎÞ´Ë¼¶±ð¿ÆÄ¿ºÅ!lvl[%s]acc_hrt[%s]",v_com_item.acc_lvl,old_acc_hrt);
				WRITEMSG
				strcpy( g_pub_tx.reply ,"S056" );			
				goto ErrExit;		
			}								
							
			/* µÇ¼ÇÍ³¼Æ±í */

			ret = Mo_dc_inf_Ins( v_mo_dc_inf , g_pub_tx.reply );
			if( ret )
			{
				sprintf(acErrMsg,"µÇ¼ÇÔþÕË¿ÆÄ¿Í³¼Æ±íÒì³£³ö´í,ÇëÓëÖÐÐÄ»ú·¿ÁªÏµ!");
				WRITEMSG
				strcpy( g_pub_tx.reply ,"G106" );			
				goto ErrExit;					
			}
			
			strcpy( old_acc_hrt , v_dc_log.acc_hrt);	/* ¿ÆÄ¿¿ØÖÆ×Ö */ 	
			
			/* Êý¾ÝÇåÁã */
			dr_cash_amt=0.00;
			dr_cash_cnt=0;
			dr_ch_amt=0.00;
			dr_ch_cnt=0;
			dc_cash_amt=0.00;
			dc_cash_cnt=0;
			dc_ch_amt=0.00;
			dc_ch_cnt=0;
			memset( &v_com_tel , 0x00 , sizeof(struct com_tel_c));
			memset( &v_com_item_lvl1 , 0x00 , sizeof(struct com_item_c));
			memset( &v_com_item_lvl2 , 0x00 , sizeof(struct com_item_c));
			memset( &v_com_item_lvl3 , 0x00 , sizeof(struct com_item_c));
			memset( &v_mo_dc_inf , 0x00 , sizeof(struct mo_dc_inf_c));						
		}
		
		
		/* Í³¼ÆÐÅÏ¢ */
		if( pub_base_CompDblVal( v_dc_log.amt , 0.00 ) )
		{
			if( v_dc_log.ct_ind[0]=='1' )		/* ÏÖ½ð */
			{
				if( v_dc_log.dc_ind[0]=='1')	/* ½è·½ */
				{
					dr_cash_amt+=v_dc_log.amt;
					dr_cash_cnt++;
				}
				if( v_dc_log.dc_ind[0]=='2')	/* ´û·½ */
				{
					dc_cash_amt+=v_dc_log.amt;
					dc_cash_cnt++;
				}
			}
			if( v_dc_log.ct_ind[0]=='2' )		/* ×ªÕË */
			{
				if( v_dc_log.dc_ind[0]=='1')	/* ½è·½ */
				{
					dr_ch_amt+=v_dc_log.amt;
					dr_ch_cnt++;
				}
				if( v_dc_log.dc_ind[0]=='2')	/* ´û·½ */
				{
					dc_ch_amt+=v_dc_log.amt;
					dc_ch_cnt++;
				}
			}			
		}	
		
		ttlnum++;
	}

	sprintf(acErrMsg,"===-=--=-=--=-=-=----=-=-=-");
	WRITEMSG
		sprintf(acErrMsg,"hello");
WRITEMSG
	ret=Dc_log_Clo_Sel();
	sprintf(acErrMsg,"hello");
WRITEMSG
	ERR_DEAL
	sprintf(acErrMsg,"hello");
WRITEMSG
	if( !ttlnum )
	{
		strcpy( g_pub_tx.reply,"S049" );
		goto ErrExit;
	}else{
/***JYW		fclose(fp); **/
		set_zd_data( DC_FILE_SND,"1" );
	}
		
	
	/** Ð´ÏÔÊ¾ÎÄ¼þ **/
	
	ret=Mo_dc_inf_Dec_Sel( g_pub_tx.reply,"br_no='%s' and tel='%s' order by acc_no_lvl1,acc_no_lvl2,acc_no_lvl3",g_pub_tx.tx_br_no,g_pub_tx.tel );
	ERR_DEAL

		num=0;
	sprintf(acErrMsg,"===-=--=-=--=-=-=----=-=-=-");
	WRITEMSG	
	while(1)
	{
		ret=Mo_dc_inf_Fet_Sel( &v_mo_dc_inf, g_pub_tx.reply );
		if( ret==100 ) 
		{
			sprintf(acErrMsg,"<><><><><><><><><><><>");
			WRITEMSG
			if( num )
			{
				/* ·Ö±ðÐ´±íÄÚ/±íÍâ¿ÆÄ¿Í³¼ÆÐÅÏ¢ */
				/* ±íÄÚÐÅÏ¢ */
				ret = sql_sum_double( "mo_dc_inf" , "dr_cash_amt" , &dr_cash_amt , "br_no='%s' and tel='%s' and sts='0'" , g_pub_tx.tx_br_no,g_pub_tx.tel);
				ERR_DEAL				
				ret = sql_sum_double( "mo_dc_inf" , "dc_cash_amt" , &dc_cash_amt , "br_no='%s' and tel='%s' and sts='0'" , g_pub_tx.tx_br_no,g_pub_tx.tel);			
				ERR_DEAL
				ret = sql_sum_double( "mo_dc_inf" , "dr_ch_amt" , &dr_ch_amt , "br_no='%s' and tel='%s' and sts='0'" , g_pub_tx.tx_br_no,g_pub_tx.tel);
				ERR_DEAL				
				ret = sql_sum_double( "mo_dc_inf" , "dc_ch_amt" , &dc_ch_amt , "br_no='%s' and tel='%s' and sts='0'" , g_pub_tx.tx_br_no,g_pub_tx.tel);		
				ERR_DEAL
				
				ret = sql_sum_long( "mo_dc_inf" , "dr_cash_cnt" , &dr_cash_cnt , "br_no='%s' and tel='%s' and sts='0'" , g_pub_tx.tx_br_no,g_pub_tx.tel);
				ERR_DEAL				
				ret = sql_sum_long( "mo_dc_inf" , "dc_cash_cnt" , &dc_cash_cnt , "br_no='%s' and tel='%s' and sts='0'" , g_pub_tx.tx_br_no,g_pub_tx.tel);			
				ERR_DEAL
				ret = sql_sum_long( "mo_dc_inf" , "dr_ch_cnt" , &dr_ch_cnt , "br_no='%s' and tel='%s' and sts='0'" , g_pub_tx.tx_br_no,g_pub_tx.tel);
				ERR_DEAL				
				ret = sql_sum_long( "mo_dc_inf" , "dc_ch_cnt" , &dc_ch_cnt , "br_no='%s' and tel='%s' and sts='0'" , g_pub_tx.tx_br_no,g_pub_tx.tel);		
				ERR_DEAL
																
				/**ÏÔÊ¾ÁÐ±êÌâ**/
				fprintf( fp,"@3v1\n");
				fprintf( fp,"`\n" );
				fprintf( fp,"`     ¿Æ Ä¿ ºÅ: ¡¾±í    ÄÚ¡¿\n");
				fprintf( fp,"`     ¿ÆÄ¿Ãû³Æ: ¡¾±¾ÈÕ±íÄÚ¿ÆÄ¿»ã×ÜÐÅÏ¢¡¿\n" );
				fprintf( fp,"`     ¿ÆÄ¿¼¶±ð: ¡¾±íÄÚ¿ÆÄ¿¡¿\n" );	
				fprintf( fp,"`\n" );
				fprintf( fp,"`    ©±©¥©¥©Ó©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©Ó©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©µ\n" );
				fprintf( fp,"`    ©¦    ©¦          ½è       ·½       ©¦           ´û      ·½       ©¦\n" );
				fprintf( fp,"`    ©¦    ©Á©¥©¥©¥©¥©¥©¥©¥©¥©Ó©¥©¥©¥©¥©¥©ã©¥©¥©¥©¥©¥©¥©¥©¥©Ó©¥©¥©¥©¥©¥©É\n" );										
				fprintf( fp,"`    ©¦    ©¦   ½ð      ¶î   ©¦  ±Ê  Êý  ©¦   ½ð      ¶î   ©¦  ±Ê  Êý  ©¦\n" );
				fprintf( fp,"`    ©Á©¥©¥©ã©¥©¥©¥©¥©¥©¥©¥©¥©ã©¥©¥©¥©¥©¥©ã©¥©¥©¥©¥©¥©¥©¥©¥©ã©¥©¥©¥©¥©¥©É\n" );
				fprintf( fp,"`    ©¦ÏÖ½ð©¦%16.2lf©¦%10d©¦%16.2lf©¦%10d©¦\n" ,dr_cash_amt,dr_cash_cnt,dc_cash_amt,dc_cash_cnt );
				fprintf( fp,"`    ©Á©¥©¥©ã©¥©¥©¥©¥©¥©¥©¥©¥©ã©¥©¥©¥©¥©¥©ã©¥©¥©¥©¥©¥©¥©¥©¥©ã©¥©¥©¥©¥©¥©É\n" );
				fprintf( fp,"`    ©¦×ªÕË©¦%16.2lf©¦%10d©¦%16.2lf©¦%10d©¦\n" ,dr_ch_amt,dr_ch_cnt,dc_ch_amt,dc_ch_cnt);
				fprintf( fp,"`    ©Á©¥©¥©ã©¥©¥©¥©¥©¥©¥©¥©¥©ã©¥©¥©¥©¥©¥©ã©¥©¥©¥©¥©¥©¥©¥©¥©ã©¥©¥©¥©¥©¥©É\n" );
				fprintf( fp,"`    ©¦ºÏ¼Æ©¦%16.2lf©¦%10d©¦%16.2lf©¦%10d©¦\n" ,dr_cash_amt+dr_ch_amt,dr_cash_cnt+dr_ch_cnt,dc_cash_amt+dc_ch_amt,dc_cash_cnt+dc_ch_cnt);
				fprintf( fp,"`    ©¹©¥©¥©Û©¥©¥©¥©¥©¥©¥©¥©¥©Û©¥©¥©¥©¥©¥©Û©¥©¥©¥©¥©¥©¥©¥©¥©Û©¥©¥©¥©¥©¥©½\n" );		
				fprintf( fp,"`\n" );			
				fprintf( fp,"`\n" );
				
				/* Êý¾ÝÇåÁã */				
				
				dr_cash_amt=0.00;
				dr_cash_cnt=0;
				dr_ch_amt=0.00;
				dr_ch_cnt=0;
				dc_cash_amt=0.00;
				dc_cash_cnt=0;
				dc_ch_amt=0.00;
				dc_ch_cnt=0;
				
				/* ±íÍâÐÅÏ¢ */
				ret = sql_sum_double( "mo_dc_inf" , "dr_cash_amt" , &dr_cash_amt , "br_no='%s' and tel='%s' and sts='1'" , g_pub_tx.tx_br_no,g_pub_tx.tel);
				ERR_DEAL				
				ret = sql_sum_double( "mo_dc_inf" , "dc_cash_amt" , &dc_cash_amt , "br_no='%s' and tel='%s' and sts='1'" , g_pub_tx.tx_br_no,g_pub_tx.tel);			
				ERR_DEAL
				ret = sql_sum_double( "mo_dc_inf" , "dr_ch_amt" , &dr_ch_amt , "br_no='%s' and tel='%s' and sts='1'" , g_pub_tx.tx_br_no,g_pub_tx.tel);
				ERR_DEAL				
				ret = sql_sum_double( "mo_dc_inf" , "dc_ch_amt" , &dc_ch_amt , "br_no='%s' and tel='%s' and sts='1'" , g_pub_tx.tx_br_no,g_pub_tx.tel);		
				ERR_DEAL
				
				ret = sql_sum_long( "mo_dc_inf" , "dr_cash_cnt" , &dr_cash_cnt , "br_no='%s' and tel='%s' and sts='1'" , g_pub_tx.tx_br_no,g_pub_tx.tel);
				ERR_DEAL				
				ret = sql_sum_long( "mo_dc_inf" , "dc_cash_cnt" , &dc_cash_cnt , "br_no='%s' and tel='%s' and sts='1'" , g_pub_tx.tx_br_no,g_pub_tx.tel);			
				ERR_DEAL
				ret = sql_sum_long( "mo_dc_inf" , "dr_ch_cnt" , &dr_ch_cnt , "br_no='%s' and tel='%s' and sts='1'" , g_pub_tx.tx_br_no,g_pub_tx.tel);
				ERR_DEAL				
				ret = sql_sum_long( "mo_dc_inf" , "dc_ch_cnt" , &dc_ch_cnt , "br_no='%s' and tel='%s' and sts='1'" , g_pub_tx.tx_br_no,g_pub_tx.tel);		
				ERR_DEAL
																
				/**ÏÔÊ¾ÁÐ±êÌâ**/
				fprintf( fp,"@3v1\n");
				fprintf( fp,"`\n" );
				fprintf( fp,"`     ¿Æ Ä¿ ºÅ: ¡¾±í    Íâ¡¿\n");
				fprintf( fp,"`     ¿ÆÄ¿Ãû³Æ: ¡¾±¾ÈÕ±íÍâ¿ÆÄ¿»ã×ÜÐÅÏ¢¡¿\n" );
				fprintf( fp,"`     ¿ÆÄ¿¼¶±ð: ¡¾±íÍâ¿ÆÄ¿¡¿\n" );	          
				fprintf( fp,"`\n" );
				fprintf( fp,"`    ©±©¥©¥©Ó©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©Ó©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©µ\n" );
				fprintf( fp,"`    ©¦    ©¦          ½è       ·½       ©¦           ´û      ·½       ©¦\n" );
				fprintf( fp,"`    ©¦    ©Á©¥©¥©¥©¥©¥©¥©¥©¥©Ó©¥©¥©¥©¥©¥©ã©¥©¥©¥©¥©¥©¥©¥©¥©Ó©¥©¥©¥©¥©¥©É\n" );										
				fprintf( fp,"`    ©¦    ©¦   ½ð      ¶î   ©¦  ±Ê  Êý  ©¦   ½ð      ¶î   ©¦  ±Ê  Êý  ©¦\n" );
				fprintf( fp,"`    ©Á©¥©¥©ã©¥©¥©¥©¥©¥©¥©¥©¥©ã©¥©¥©¥©¥©¥©ã©¥©¥©¥©¥©¥©¥©¥©¥©ã©¥©¥©¥©¥©¥©É\n" );
				fprintf( fp,"`    ©¦ÏÖ½ð©¦%16.2lf©¦%10d©¦%16.2lf©¦%10d©¦\n" ,dr_cash_amt,dr_cash_cnt,dc_cash_amt,dc_cash_cnt );
				fprintf( fp,"`    ©Á©¥©¥©ã©¥©¥©¥©¥©¥©¥©¥©¥©ã©¥©¥©¥©¥©¥©ã©¥©¥©¥©¥©¥©¥©¥©¥©ã©¥©¥©¥©¥©¥©É\n" );
				fprintf( fp,"`    ©¦×ªÕË©¦%16.2lf©¦%10d©¦%16.2lf©¦%10d©¦\n" ,dr_ch_amt,dr_ch_cnt,dc_ch_amt,dc_ch_cnt);
				fprintf( fp,"`    ©Á©¥©¥©ã©¥©¥©¥©¥©¥©¥©¥©¥©ã©¥©¥©¥©¥©¥©ã©¥©¥©¥©¥©¥©¥©¥©¥©ã©¥©¥©¥©¥©¥©É\n" );
				fprintf( fp,"`    ©¦ºÏ¼Æ©¦%16.2lf©¦%10d©¦%16.2lf©¦%10d©¦\n" ,dr_cash_amt+dr_ch_amt,dr_cash_cnt+dr_ch_cnt,dc_cash_amt+dc_ch_amt,dc_cash_cnt+dc_ch_cnt);
				fprintf( fp,"`    ©¹©¥©¥©Û©¥©¥©¥©¥©¥©¥©¥©¥©Û©¥©¥©¥©¥©¥©Û©¥©¥©¥©¥©¥©¥©¥©¥©Û©¥©¥©¥©¥©¥©½\n" );		
				fprintf( fp,"`\n" );			
				fprintf( fp,"`\n" );
				
				/* Êý¾ÝÇåÁã */				
				
				dr_cash_amt=0.00;
				dr_cash_cnt=0;
				dr_ch_amt=0.00;
				dr_ch_cnt=0;
				dc_cash_amt=0.00;
				dc_cash_cnt=0;
				dc_ch_amt=0.00;
				dc_ch_cnt=0;							
			
			}
			
			break;
			
		}else if(ret){
			goto ErrExit;
		}						
		if( !num )
		{
			pub_base_AllDwnFilName( filename );
			vtcp_log("filename=[%s]",filename);
			fp = fopen( filename,"w" );
			if( fp==NULL )
			{
				sprintf(acErrMsg," open file [%s] error ",filename );
				WRITEMSG
				strcpy( g_pub_tx.reply,"S047" );
				goto ErrExit;
			}
		}
		/* Í³¼ÆÒ»¼¶¿ÆÄ¿ÐÅÏ¢ */
		if( strcmp(  v_mo_dc_inf.acc_no_lvl1 , old_acc_no_lvl1 ) && strlen(v_mo_dc_inf.acc_no_lvl1)!=0 )
		{
			ret = sql_sum_double( "mo_dc_inf" , "dr_cash_amt" , &dr_cash_amt , "br_no='%s' and tel='%s' and acc_no_lvl1='%s'" , g_pub_tx.tx_br_no,g_pub_tx.tel,v_mo_dc_inf.acc_no_lvl1);
			ERR_DEAL				
			ret = sql_sum_double( "mo_dc_inf" , "dc_cash_amt" , &dc_cash_amt , "br_no='%s' and tel='%s' and acc_no_lvl1='%s'" , g_pub_tx.tx_br_no,g_pub_tx.tel,v_mo_dc_inf.acc_no_lvl1);			
			ERR_DEAL
			ret = sql_sum_double( "mo_dc_inf" , "dr_ch_amt" , &dr_ch_amt , "br_no='%s' and tel='%s' and acc_no_lvl1='%s'" , g_pub_tx.tx_br_no,g_pub_tx.tel,v_mo_dc_inf.acc_no_lvl1);
			ERR_DEAL				
			ret = sql_sum_double( "mo_dc_inf" , "dc_ch_amt" , &dc_ch_amt , "br_no='%s' and tel='%s' and acc_no_lvl1='%s'" , g_pub_tx.tx_br_no,g_pub_tx.tel,v_mo_dc_inf.acc_no_lvl1);		
			ERR_DEAL
			
			ret = sql_sum_long( "mo_dc_inf" , "dr_cash_cnt" , &dr_cash_cnt , "br_no='%s' and tel='%s' and acc_no_lvl1='%s'" , g_pub_tx.tx_br_no,g_pub_tx.tel,v_mo_dc_inf.acc_no_lvl1);
			ERR_DEAL				
			ret = sql_sum_long( "mo_dc_inf" , "dc_cash_cnt" , &dc_cash_cnt , "br_no='%s' and tel='%s' and acc_no_lvl1='%s'" , g_pub_tx.tx_br_no,g_pub_tx.tel,v_mo_dc_inf.acc_no_lvl1);			
			ERR_DEAL
			ret = sql_sum_long( "mo_dc_inf" , "dr_ch_cnt" , &dr_ch_cnt , "br_no='%s' and tel='%s' and acc_no_lvl1='%s'" , g_pub_tx.tx_br_no,g_pub_tx.tel,v_mo_dc_inf.acc_no_lvl1);
			ERR_DEAL				
			ret = sql_sum_long( "mo_dc_inf" , "dc_ch_cnt" , &dc_ch_cnt , "br_no='%s' and tel='%s' and acc_no_lvl1='%s'" , g_pub_tx.tx_br_no,g_pub_tx.tel,v_mo_dc_inf.acc_no_lvl1);		
			ERR_DEAL
															
			/**ÏÔÊ¾ÁÐ±êÌâ**/
			fprintf( fp,"@3v1\n");
			fprintf( fp,"`\n" );		
			fprintf( fp,"`     ¿Æ Ä¿ ºÅ: ¡¾%7s ¡¿\n" , v_mo_dc_inf.acc_no_lvl1 );
			fprintf( fp,"`     ¿ÆÄ¿Ãû³Æ: ¡¾%30s¡¿\n" , v_mo_dc_inf.acc_name_lvl1);
			fprintf( fp,"`     ¿ÆÄ¿¼¶±ð: ¡¾Ò»¼¶¿ÆÄ¿¡¿\n" );				
			fprintf( fp,"`\n" );
			fprintf( fp,"`    ©±©¥©¥©Ó©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©Ó©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©µ\n" );
			fprintf( fp,"`    ©¦    ©¦          ½è       ·½       ©¦           ´û      ·½       ©¦\n" );
			fprintf( fp,"`    ©¦    ©Á©¥©¥©¥©¥©¥©¥©¥©¥©Ó©¥©¥©¥©¥©¥©ã©¥©¥©¥©¥©¥©¥©¥©¥©Ó©¥©¥©¥©¥©¥©É\n" );										
			fprintf( fp,"`    ©¦    ©¦   ½ð      ¶î   ©¦  ±Ê  Êý  ©¦   ½ð      ¶î   ©¦  ±Ê  Êý  ©¦\n" );
			fprintf( fp,"`    ©Á©¥©¥©ã©¥©¥©¥©¥©¥©¥©¥©¥©ã©¥©¥©¥©¥©¥©ã©¥©¥©¥©¥©¥©¥©¥©¥©ã©¥©¥©¥©¥©¥©É\n" );
			fprintf( fp,"`    ©¦ÏÖ½ð©¦%16.2lf©¦%10d©¦%16.2lf©¦%10d©¦\n" ,dr_cash_amt,dr_cash_cnt,dc_cash_amt,dc_cash_cnt );
			fprintf( fp,"`    ©Á©¥©¥©ã©¥©¥©¥©¥©¥©¥©¥©¥©ã©¥©¥©¥©¥©¥©ã©¥©¥©¥©¥©¥©¥©¥©¥©ã©¥©¥©¥©¥©¥©É\n" );
			fprintf( fp,"`    ©¦×ªÕË©¦%16.2lf©¦%10d©¦%16.2lf©¦%10d©¦\n" ,dr_ch_amt,dr_ch_cnt,dc_ch_amt,dc_ch_cnt);
			fprintf( fp,"`    ©Á©¥©¥©ã©¥©¥©¥©¥©¥©¥©¥©¥©ã©¥©¥©¥©¥©¥©ã©¥©¥©¥©¥©¥©¥©¥©¥©ã©¥©¥©¥©¥©¥©É\n" );
			fprintf( fp,"`    ©¦ºÏ¼Æ©¦%16.2lf©¦%10d©¦%16.2lf©¦%10d©¦\n" ,dr_cash_amt+dr_ch_amt,dr_cash_cnt+dr_ch_cnt,dc_cash_amt+dc_ch_amt,dc_cash_cnt+dc_ch_cnt);
			fprintf( fp,"`    ©¹©¥©¥©Û©¥©¥©¥©¥©¥©¥©¥©¥©Û©¥©¥©¥©¥©¥©Û©¥©¥©¥©¥©¥©¥©¥©¥©Û©¥©¥©¥©¥©¥©½\n" );		
			fprintf( fp,"`\n" );			
			fprintf( fp,"`\n" );
			
			/* Êý¾ÝÇåÁã */		
			dr_cash_amt=0.00;
			dr_cash_cnt=0;
			dr_ch_amt=0.00;
			dr_ch_cnt=0;
			dc_cash_amt=0.00;
			dc_cash_cnt=0;
			dc_ch_amt=0.00;
			dc_ch_cnt=0;
			
			strcpy( old_acc_no_lvl1 , v_mo_dc_inf.acc_no_lvl1 );
			memset( &v_mo_dc_inf , 0x00 , sizeof(struct mo_dc_inf_c));
		}else{
			continue;
		}
		
		/* ²éÑ¯¶þ¼¶¿ÆÄ¿ÐÅÏ¢ */
		ret=Mo_dc_inf_Dec_Sel1( g_pub_tx.reply,"br_no='%s' and tel='%s' and acc_no_lvl1='%s' order by acc_no_lvl1,acc_no_lvl2,acc_no_lvl3",g_pub_tx.tx_br_no,g_pub_tx.tel,old_acc_no_lvl1 );
		ERR_DEAL				
		while(1)
		{
			ret=Mo_dc_inf_Fet_Sel1( &v_mo_dc_inf, g_pub_tx.reply );
			if( ret==100 ) 
			{
				break;			
			}else if(ret){
				goto ErrExit;
			}
			/* ´òÓ¡¶þ¼¶¿ÆÄ¿ÐÅÏ¢ */
			if( strcmp(  v_mo_dc_inf.acc_no_lvl2 , old_acc_no_lvl2 ) && strlen(v_mo_dc_inf.acc_no_lvl2)!=0 )
			{
				ret = sql_sum_double( "mo_dc_inf" , "dr_cash_amt" , &dr_cash_amt , "br_no='%s' and tel='%s' and acc_no_lvl2='%s'" , g_pub_tx.tx_br_no,g_pub_tx.tel,v_mo_dc_inf.acc_no_lvl2);
				ERR_DEAL				
				ret = sql_sum_double( "mo_dc_inf" , "dc_cash_amt" , &dc_cash_amt , "br_no='%s' and tel='%s' and acc_no_lvl2='%s'" , g_pub_tx.tx_br_no,g_pub_tx.tel,v_mo_dc_inf.acc_no_lvl2);			
				ERR_DEAL
				ret = sql_sum_double( "mo_dc_inf" , "dr_ch_amt" , &dr_ch_amt , "br_no='%s' and tel='%s' and acc_no_lvl2='%s'" , g_pub_tx.tx_br_no,g_pub_tx.tel,v_mo_dc_inf.acc_no_lvl2);
				ERR_DEAL				
				ret = sql_sum_double( "mo_dc_inf" , "dc_ch_amt" , &dc_ch_amt , "br_no='%s' and tel='%s' and acc_no_lvl2='%s'" , g_pub_tx.tx_br_no,g_pub_tx.tel,v_mo_dc_inf.acc_no_lvl2);		
				ERR_DEAL
				
				ret = sql_sum_long( "mo_dc_inf" , "dr_cash_cnt" , &dr_cash_cnt , "br_no='%s' and tel='%s' and acc_no_lvl2='%s'" , g_pub_tx.tx_br_no,g_pub_tx.tel,v_mo_dc_inf.acc_no_lvl2);
				ERR_DEAL				
				ret = sql_sum_long( "mo_dc_inf" , "dc_cash_cnt" , &dc_cash_cnt , "br_no='%s' and tel='%s' and acc_no_lvl2='%s'" , g_pub_tx.tx_br_no,g_pub_tx.tel,v_mo_dc_inf.acc_no_lvl2);			
				ERR_DEAL
				ret = sql_sum_long( "mo_dc_inf" , "dr_ch_cnt" , &dr_ch_cnt , "br_no='%s' and tel='%s' and acc_no_lvl2='%s'" , g_pub_tx.tx_br_no,g_pub_tx.tel,v_mo_dc_inf.acc_no_lvl2);
				ERR_DEAL				
				ret = sql_sum_long( "mo_dc_inf" , "dc_ch_cnt" , &dc_ch_cnt , "br_no='%s' and tel='%s' and acc_no_lvl2='%s'" , g_pub_tx.tx_br_no,g_pub_tx.tel,v_mo_dc_inf.acc_no_lvl2);		
				ERR_DEAL
																
				/**ÏÔÊ¾ÁÐ±êÌâ**/
				fprintf( fp,"@3v1\n");
				fprintf( fp,"`\n" );
				fprintf( fp,"`     ¿Æ Ä¿ ºÅ: ¡¾%7s ¡¿\n" , v_mo_dc_inf.acc_no_lvl2 );
				fprintf( fp,"`     ¿ÆÄ¿Ãû³Æ: ¡¾%30s¡¿\n" , v_mo_dc_inf.acc_name_lvl2);
				fprintf( fp,"`     ¿ÆÄ¿¼¶±ð: ¡¾¶þ¼¶¿ÆÄ¿¡¿\n" );	
				fprintf( fp,"`\n" );
				fprintf( fp,"`    ©±©¥©¥©Ó©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©Ó©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©µ\n" );
				fprintf( fp,"`    ©¦    ©¦          ½è       ·½       ©¦           ´û      ·½       ©¦\n" );
				fprintf( fp,"`    ©¦    ©Á©¥©¥©¥©¥©¥©¥©¥©¥©Ó©¥©¥©¥©¥©¥©ã©¥©¥©¥©¥©¥©¥©¥©¥©Ó©¥©¥©¥©¥©¥©É\n" );										
				fprintf( fp,"`    ©¦    ©¦   ½ð      ¶î   ©¦  ±Ê  Êý  ©¦   ½ð      ¶î   ©¦  ±Ê  Êý  ©¦\n" );
				fprintf( fp,"`    ©Á©¥©¥©ã©¥©¥©¥©¥©¥©¥©¥©¥©ã©¥©¥©¥©¥©¥©ã©¥©¥©¥©¥©¥©¥©¥©¥©ã©¥©¥©¥©¥©¥©É\n" );
				fprintf( fp,"`    ©¦ÏÖ½ð©¦%16.2lf©¦%10d©¦%16.2lf©¦%10d©¦\n" ,dr_cash_amt,dr_cash_cnt,dc_cash_amt,dc_cash_cnt );
				fprintf( fp,"`    ©Á©¥©¥©ã©¥©¥©¥©¥©¥©¥©¥©¥©ã©¥©¥©¥©¥©¥©ã©¥©¥©¥©¥©¥©¥©¥©¥©ã©¥©¥©¥©¥©¥©É\n" );
				fprintf( fp,"`    ©¦×ªÕË©¦%16.2lf©¦%10d©¦%16.2lf©¦%10d©¦\n" ,dr_ch_amt,dr_ch_cnt,dc_ch_amt,dc_ch_cnt);
				fprintf( fp,"`    ©Á©¥©¥©ã©¥©¥©¥©¥©¥©¥©¥©¥©ã©¥©¥©¥©¥©¥©ã©¥©¥©¥©¥©¥©¥©¥©¥©ã©¥©¥©¥©¥©¥©É\n" );
				fprintf( fp,"`    ©¦ºÏ¼Æ©¦%16.2lf©¦%10d©¦%16.2lf©¦%10d©¦\n" ,dr_cash_amt+dr_ch_amt,dr_cash_cnt+dr_ch_cnt,dc_cash_amt+dc_ch_amt,dc_cash_cnt+dc_ch_cnt);
				fprintf( fp,"`    ©¹©¥©¥©Û©¥©¥©¥©¥©¥©¥©¥©¥©Û©¥©¥©¥©¥©¥©Û©¥©¥©¥©¥©¥©¥©¥©¥©Û©¥©¥©¥©¥©¥©½\n" );		
				fprintf( fp,"`\n" );			
				fprintf( fp,"`\n" );
				
				/* Êý¾ÝÇåÁã */		
				dr_cash_amt=0.00;
				dr_cash_cnt=0;
				dr_ch_amt=0.00;
				dr_ch_cnt=0;
				dc_cash_amt=0.00;
				dc_cash_cnt=0;
				dc_ch_amt=0.00;
				dc_ch_cnt=0;
				
				strcpy( old_acc_no_lvl2 , v_mo_dc_inf.acc_no_lvl2 );
				memset( &v_mo_dc_inf , 0x00 , sizeof(struct mo_dc_inf_c));
			}else{
				continue;
			}
			
			/* ²éÑ¯Èý¼¶¿ÆÄ¿ÐÅÏ¢ */
			ret=Mo_dc_inf_Dec_Sel2( g_pub_tx.reply,"br_no='%s' and tel='%s' and acc_no_lvl1='%s' and acc_no_lvl2='%s' order by acc_no_lvl1,acc_no_lvl2,acc_no_lvl3",g_pub_tx.tx_br_no,g_pub_tx.tel,old_acc_no_lvl1,old_acc_no_lvl2 );
			ERR_DEAL				
			while(1)
			{
				ret=Mo_dc_inf_Fet_Sel2( &v_mo_dc_inf, g_pub_tx.reply );
				if( ret==100 ) 
				{
					break;			
				}else if(ret){
					goto ErrExit;
				}
				/* ´òÓ¡Èý¼¶¿ÆÄ¿ÐÅÏ¢ */
				if( strcmp(  v_mo_dc_inf.acc_no_lvl3 , old_acc_no_lvl3 ) && strlen(v_mo_dc_inf.acc_no_lvl3)!=0)
				{
					ret = sql_sum_double( "mo_dc_inf" , "dr_cash_amt" , &dr_cash_amt , "br_no='%s' and tel='%s' and acc_no_lvl3='%s'" , g_pub_tx.tx_br_no,g_pub_tx.tel,v_mo_dc_inf.acc_no_lvl3);
					ERR_DEAL				
					ret = sql_sum_double( "mo_dc_inf" , "dc_cash_amt" , &dc_cash_amt , "br_no='%s' and tel='%s' and acc_no_lvl3='%s'" , g_pub_tx.tx_br_no,g_pub_tx.tel,v_mo_dc_inf.acc_no_lvl3);			
					ERR_DEAL
					ret = sql_sum_double( "mo_dc_inf" , "dr_ch_amt" , &dr_ch_amt , "br_no='%s' and tel='%s' and acc_no_lvl3='%s'" , g_pub_tx.tx_br_no,g_pub_tx.tel,v_mo_dc_inf.acc_no_lvl3);
					ERR_DEAL				
					ret = sql_sum_double( "mo_dc_inf" , "dc_ch_amt" , &dc_ch_amt , "br_no='%s' and tel='%s' and acc_no_lvl3='%s'" , g_pub_tx.tx_br_no,g_pub_tx.tel,v_mo_dc_inf.acc_no_lvl3);		
					ERR_DEAL
					
					ret = sql_sum_long( "mo_dc_inf" , "dr_cash_cnt" , &dr_cash_cnt , "br_no='%s' and tel='%s' and acc_no_lvl3='%s'" , g_pub_tx.tx_br_no,g_pub_tx.tel,v_mo_dc_inf.acc_no_lvl3);
					ERR_DEAL				
					ret = sql_sum_long( "mo_dc_inf" , "dc_cash_cnt" , &dc_cash_cnt , "br_no='%s' and tel='%s' and acc_no_lvl3='%s'" , g_pub_tx.tx_br_no,g_pub_tx.tel,v_mo_dc_inf.acc_no_lvl3);			
					ERR_DEAL
					ret = sql_sum_long( "mo_dc_inf" , "dr_ch_cnt" , &dr_ch_cnt , "br_no='%s' and tel='%s' and acc_no_lvl3='%s'" , g_pub_tx.tx_br_no,g_pub_tx.tel,v_mo_dc_inf.acc_no_lvl3);
					ERR_DEAL				
					ret = sql_sum_long( "mo_dc_inf" , "dc_ch_cnt" , &dc_ch_cnt , "br_no='%s' and tel='%s' and acc_no_lvl3='%s'" , g_pub_tx.tx_br_no,g_pub_tx.tel,v_mo_dc_inf.acc_no_lvl3);		
					ERR_DEAL
																	
					/**ÏÔÊ¾ÁÐ±êÌâ**/
					fprintf( fp,"@3v1\n");
					fprintf( fp,"`\n" );
					fprintf( fp,"`     ¿Æ Ä¿ ºÅ: ¡¾%7s ¡¿\n" , v_mo_dc_inf.acc_no_lvl3 );
					fprintf( fp,"`     ¿ÆÄ¿Ãû³Æ: ¡¾%30s¡¿\n" , v_mo_dc_inf.acc_name_lvl3);
					fprintf( fp,"`     ¿ÆÄ¿¼¶±ð: ¡¾Èý¼¶¿ÆÄ¿¡¿\n" );	
					fprintf( fp,"`\n" );
					fprintf( fp,"`    ©±©¥©¥©Ó©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©Ó©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©µ\n" );
					fprintf( fp,"`    ©¦    ©¦          ½è       ·½       ©¦           ´û      ·½       ©¦\n" );
					fprintf( fp,"`    ©¦    ©Á©¥©¥©¥©¥©¥©¥©¥©¥©Ó©¥©¥©¥©¥©¥©ã©¥©¥©¥©¥©¥©¥©¥©¥©Ó©¥©¥©¥©¥©¥©É\n" );										
					fprintf( fp,"`    ©¦    ©¦   ½ð      ¶î   ©¦  ±Ê  Êý  ©¦   ½ð      ¶î   ©¦  ±Ê  Êý  ©¦\n" );
					fprintf( fp,"`    ©Á©¥©¥©ã©¥©¥©¥©¥©¥©¥©¥©¥©ã©¥©¥©¥©¥©¥©ã©¥©¥©¥©¥©¥©¥©¥©¥©ã©¥©¥©¥©¥©¥©É\n" );
					fprintf( fp,"`    ©¦ÏÖ½ð©¦%16.2lf©¦%10d©¦%16.2lf©¦%10d©¦\n" ,dr_cash_amt,dr_cash_cnt,dc_cash_amt,dc_cash_cnt );
					fprintf( fp,"`    ©Á©¥©¥©ã©¥©¥©¥©¥©¥©¥©¥©¥©ã©¥©¥©¥©¥©¥©ã©¥©¥©¥©¥©¥©¥©¥©¥©ã©¥©¥©¥©¥©¥©É\n" );
					fprintf( fp,"`    ©¦×ªÕË©¦%16.2lf©¦%10d©¦%16.2lf©¦%10d©¦\n" ,dr_ch_amt,dr_ch_cnt,dc_ch_amt,dc_ch_cnt);
					fprintf( fp,"`    ©Á©¥©¥©ã©¥©¥©¥©¥©¥©¥©¥©¥©ã©¥©¥©¥©¥©¥©ã©¥©¥©¥©¥©¥©¥©¥©¥©ã©¥©¥©¥©¥©¥©É\n" );
					fprintf( fp,"`    ©¦ºÏ¼Æ©¦%16.2lf©¦%10d©¦%16.2lf©¦%10d©¦\n" ,dr_cash_amt+dr_ch_amt,dr_cash_cnt+dr_ch_cnt,dc_cash_amt+dc_ch_amt,dc_cash_cnt+dc_ch_cnt);
					fprintf( fp,"`    ©¹©¥©¥©Û©¥©¥©¥©¥©¥©¥©¥©¥©Û©¥©¥©¥©¥©¥©Û©¥©¥©¥©¥©¥©¥©¥©¥©Û©¥©¥©¥©¥©¥©½\n" );		
					fprintf( fp,"`\n" );			
					fprintf( fp,"`\n" );
					
					/* Êý¾ÝÇåÁã */		
					dr_cash_amt=0.00;
					dr_cash_cnt=0;
					dr_ch_amt=0.00;
					dr_ch_cnt=0;
					dc_cash_amt=0.00;
					dc_cash_cnt=0;
					dc_ch_amt=0.00;
					dc_ch_cnt=0;
					
					strcpy( old_acc_no_lvl3 , v_mo_dc_inf.acc_no_lvl3 );
					memset( &v_mo_dc_inf , 0x00 , sizeof(struct mo_dc_inf_c));
				}else{
					continue;
				}			
			num++;
			}
			ret=Mo_dc_inf_Clo_Sel2( );
			ERR_DEAL
		num++;
		}					
		ret=Mo_dc_inf_Clo_Sel1( );
		ERR_DEAL
	num++;
	}
	ret=Mo_dc_inf_Clo_Sel( );
	ERR_DEAL
		
	fclose(fp);
	set_zd_data( DC_FILE_SND,"1" );
	

GoodExit:
	
	ret = sql_execute( "delete from  mo_dc_inf where br_no='%s' and tel='%s' " , g_pub_tx.tx_br_no , g_pub_tx.tel );
	ERR_DEAL
	
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
