/*************************************************
* ÎÄ ¼ş Ãû:  spE608.c
* ¹¦ÄÜÃèÊö£º ±¾ÈÕÔşÕË(ÏÖ½ğ)
*
* ×÷    Õß:  andy
* Íê³ÉÈÕÆÚ£º 2004-07-06
*
* ĞŞ¸Ä¼ÇÂ¼£º
* ÈÕ   ÆÚ:
* ĞŞ ¸Ä ÈË:
* ĞŞ¸ÄÄÚÈİ:
*************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define EXTERN
#include "public.h"
#include "com_tel_c.h"
#include "cash_mst_c.h"

int spE608()
{

	char filename[100];
	char wherelist[1024];  /**²éÑ¯Ìõ¼ş**/
	char tmpstr[512];
	
	char tel[5];
	int ret=0;
	
	double in_amt=0.00,out_amt=0.00,tot_amt=0.00,ys_tot_amt=0.00;
						
	FILE *fp;
	
	struct com_tel_c	v_com_tel;
	struct cash_mst_c	v_cash_mst;
	
	memset( &v_com_tel , 0x00 , sizeof(struct com_tel_c));			
	memset( &v_cash_mst , 0x00 , sizeof(struct cash_mst_c));	
	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset( wherelist,0,sizeof(wherelist) );

	
	pub_base_sysinit();

	get_zd_data("0910",tel );

	/**×é³É²éÑ¯Ìõ¼ş**/
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
			sprintf(acErrMsg,"¶ÁÈ¡¹ñÔ±ĞÅÏ¢±íÒì³£³ö´í!");
			WRITEMSG
			strcpy( g_pub_tx.reply ,"O221" );			
			goto ErrExit;
		}									
		sprintf( tmpstr," tel='%s' and",tel );
		strcat( wherelist,tmpstr );
		
	/* »ú¹¹±àÂë */
	sprintf( tmpstr," br_no='%s' and cur_no='01' and ",g_pub_tx.tx_br_no );
	strcat( wherelist,tmpstr );

	
	/**×é³É²éÑ¯**/
	strcat(wherelist," sts='0' ") ;

	vtcp_log( "WHERE [%s]",wherelist );
	
	ret = Cash_mst_Sel( g_pub_tx.reply , &v_cash_mst , wherelist );
	if( ret ==100 )
	{
		sprintf(acErrMsg,"´Ë¹ñÔ±ÎŞ¿ÉÓÃÇ®Ïä!");
		WRITEMSG
		strcpy( g_pub_tx.reply ,"D189" );			
		goto ErrExit;		
	}else if( ret ){
		sprintf(acErrMsg,"²éÑ¯ÏÖ½ğÖ÷ÎÄ¼şÒì³£³ö´í,ÇëÓëÖĞĞÄ»ú·¿ÁªÏµ!");
		WRITEMSG
		strcpy( g_pub_tx.reply ,"D190" );			
		goto ErrExit;	
	}
	tot_amt    = v_cash_mst.bal;	
	ys_tot_amt = v_cash_mst.ys_bal;	
}
else{/*Èç¹û¹ñÔ±ºÅÎª¿Õ,ÔòÈ¡ËùÓĞ¹ñÔ±Ç®ÏäÓà¶îÎª»ú¹¹µÄÏÖ½ğÓà¶îJYW*/
	sprintf( tmpstr," br_no='%s' and cur_no='01' and  ",g_pub_tx.tx_br_no );
	strcat( wherelist,tmpstr );
  strcat( wherelist,"  sts='0' ");
	
	/**×é³É²éÑ¯**/
	
	vtcp_log( "WHERE [%s]",wherelist );	
	ret=sql_sum_double( "cash_mst","bal",&tot_amt,wherelist);
	ret=sql_sum_double( "cash_mst","ys_bal",&ys_tot_amt,wherelist);
	vtcp_log("[%s][%d], wherelist = [%s] ,ys_bal=[%.2f]\n",__FILE__,__LINE__,wherelist,ys_tot_amt);
  ERR_DEAL
}	
	
	/* Í³¼Æ±¾ÈÕÏÖ½ğÊÕÈë¸¶³ö½ğ¶î */
	pub_base_strpack( tel );
	if( strlen(tel) )	/* ¹ñÔ±ºÅ·Ç¿Õ */
	{	
		ret = sql_sum_double( "cash_mst_hst" , "amt" , &in_amt , "tx_date=%ld and br_no='%s' and cur_no='01' and add_ind='1' and tel='%s'" , g_pub_tx.tx_date,g_pub_tx.tx_br_no,tel);
		ERR_DEAL				
		ret = sql_sum_double( "cash_mst_hst" , "amt" , &out_amt , "tx_date=%ld and br_no='%s' and cur_no='01' and add_ind='0' and tel='%s'" , g_pub_tx.tx_date,g_pub_tx.tx_br_no,tel);			
		ERR_DEAL			
	}else{			/* ¹ñÔ±ºÅÎª¿Õ */
	
		ret = sql_sum_double( "cash_mst_hst" , "amt" , &in_amt , "tx_date=%ld and br_no='%s' and cur_no='01' and add_ind='1' " , g_pub_tx.tx_date,g_pub_tx.tx_br_no);
		ERR_DEAL				
		ret = sql_sum_double( "cash_mst_hst" , "amt" , &out_amt , "tx_date=%ld and br_no='%s' and cur_no='01' and add_ind='0' " , g_pub_tx.tx_date,g_pub_tx.tx_br_no);			
		ERR_DEAL
	}	
	
	/* Ğ´ÏÔÊ¾ÎÄ¼ş */
	pub_base_AllDwnFilName( filename );
	fp = fopen( filename,"w" );
	if( fp==NULL )
	{
		sprintf(acErrMsg," open file [%s] error ",filename );
		WRITEMSG
		strcpy( g_pub_tx.reply,"S047" );
		goto ErrExit;
	}
	fprintf( fp,"@3v1");
	fprintf( fp,"`\n" );	
	fprintf( fp,"`\n" );
	fprintf( fp,"`                            ¡¾±¾ÈÕÏÖ½ğÔş²îĞÅÏ¢¡¿\n" );
	fprintf( fp,"`\n" );	
	fprintf( fp,"` ©±©¥©¥©¥©¥©¥©¥©¥©¥©Ó©¥©¥©¥©¥©¥©¥©¥©¥©Ó©¥©¥©¥©¥©¥©¥©¥©¥©Ó©¥©¥©¥©¥©¥©¥©¥©¥©µ\n" );
	fprintf( fp,"` ©¦   ×ò ÈÕ Óà ¶î  ©¦   ±¾ ÈÕ ÊÕ Èë  ©¦   ±¾ ÈÕ ¸¶ ³ö  ©¦   ±¾ ÈÕ Óà ¶î  ©¦\n" );			
	fprintf( fp,"` ©Á©¥©¥©¥©¥©¥©¥©¥©¥©ã©¥©¥©¥©¥©¥©¥©¥©¥©ã©¥©¥©¥©¥©¥©¥©¥©¥©ã©¥©¥©¥©¥©¥©¥©¥©¥©É\n" );			
	fprintf( fp,"` ©¦%16.2lf©¦%16.2lf©¦%16.2lf©¦%16.2lf©¦\n" ,ys_tot_amt,in_amt,out_amt,tot_amt);			
	fprintf( fp,"` ©¹©¥©¥©¥©¥©¥©¥©¥©¥©Û©¥©¥©¥©¥©¥©¥©¥©¥©Û©¥©¥©¥©¥©¥©¥©¥©¥©Û©¥©¥©¥©¥©¥©¥©¥©¥©½\n" );	
	fprintf( fp,"`\n" );		
	fprintf( fp,"`\n" );
	
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
