/*************************************************
* ÎÄ ¼þ Ãû:  sp9796.c
* ¹¦ÄÜÃèÊö£º 1103½»Ò×µÄ¿ì²é¡
* ×÷    Õß:  rob 
* Íê³ÉÈÕÆÚ£º 2003Äê2ÔÂ21ÈÕ
*
* ÐÞ¸Ä¼ÇÂ¼£º 
*   ÈÕ   ÆÚ:
*   ÐÞ ¸Ä ÈË:
*   ÐÞ¸ÄÄÚÈÝ:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "cif_id_code_rel_c.h"
#include "cif_basic_inf_c.h"
#include "cif_cop_inf_c.h"

int sp9796()
{
	int	ret;
	long cif_no;
	char cif_no_str[9];
	char cif_typ[2];

	struct 	cif_id_code_rel_c   g_cif_id_code_rel;	/*¿Í»§Ö¤¼þºÅÂë±í*/
	struct  cif_basic_inf_c cif_basic_inf;
	struct  cif_cop_inf_c cif_cop_inf;
	
	/*Çå¿Õ*/
	memset(&g_cif_id_code_rel,0x00,sizeof(struct cif_id_code_rel_c));
	memset(&cif_basic_inf,0x00,sizeof(struct cif_basic_inf_c));
	memset(&cif_cop_inf,0x00,sizeof(struct cif_cop_inf_c));
		
	/*Ç°Ì¨È¡Öµ*/
	get_zd_long("0280",&cif_no);       				/*¿Í»§ºÅ*/

	sprintf(acErrMsg, "¿Í»§ºÅÎª£º[%ld]", cif_no);
	WRITEMSG

	/* ¼ì²é¿Í»§ºÅÊÇ·ñÒÑ¾­´æÔÚ */
	ret = Cif_basic_inf_Sel( g_pub_tx.reply , &cif_basic_inf , 
						" cif_no=%ld " , cif_no );
	if( ret==100 )
	{
		sprintf(acErrMsg,"ÎÞ´Ë¿Í»§ºÅ£¬Çë¼ì²é![%s], [%ld]",
		g_pub_tx.reply, cif_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C007");
		goto ErrExit;;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"¶ÁÈ¡¿Í»§»ù±¾ÐÅÏ¢Òì³£![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C006");
		goto ErrExit;;
	}
	sprintf(acErrMsg,"¼ì²é¿Í»§ºÅÊÇ·ñÒÑ¾­´æÔÚ PASS!");
	WRITEMSG
		
	/* ¿Í»§ºÅ¼ì²é */
	ret = pub_cif_GetCifType( cif_no , &cif_typ );
	if (ret) goto ErrExit;
	
	/* ¼ì²éÊÇ·ñ¶Ô¹«¿Í»§ */
	if ( cif_typ[0]!='2' && cif_typ[0]!='4' )
	{
		sprintf(acErrMsg,"·Ç¶Ô¹«¿Í»§ºÅ!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"C010");
		goto ErrExit;
	}
	
	/* ²éÑ¯¹«Ë¾¿Í»§ÐÅÏ¢±í¼ì²éÊÇ·ñµÇ¼Ç¹ý */
   	ret=Cif_cop_inf_Sel(g_pub_tx.reply,&cif_cop_inf, "cif_no=%ld",cif_no);
   	if( ret==0 )
   	{
       		sprintf( acErrMsg,"´Ë¿Í»§ÐÅÏ¢ÒÑµÇ¼Ç!" );
   	  	WRITEMSG
   	  	strcpy(g_pub_tx.reply,"C100");
   		goto ErrExit;
   	}else if( ret&&ret!=100 ){
         	sprintf( acErrMsg,"¶ÁÈ¡¹«Ë¾¿Í»§ÐÅÏ¢±íÊ§°Ü!" );
   	  	WRITEMSG
   		goto ErrExit; 		   		
	}


   	ret=Cif_id_code_rel_Dec_Sel(g_pub_tx.reply, "cif_no=%ld",cif_no);
   	if( ret )
   	{
        sprintf( acErrMsg,"²éÑ¯¿Í»§Ö¤¼þÓë¿Í»§ºÅ¶ÔÕÕ±íÒì³£!" );
        WRITEMSG
        goto ErrExit;
	}

	while(1)
	{
		ret=Cif_id_code_rel_Fet_Sel(&g_cif_id_code_rel,g_pub_tx.reply);
		if(ret==100)break;
   		else if( !ret )
   		{
			if(g_cif_id_code_rel.id_type[0]=='8')
				set_zd_data( "0620", g_cif_id_code_rel.id_no );
			if(g_cif_id_code_rel.id_type[0]=='A')
				set_zd_data( "0630", g_cif_id_code_rel.id_no );
			if(g_cif_id_code_rel.id_type[0]=='9')
				set_zd_data( "0300", g_cif_id_code_rel.id_no );	
			if(g_cif_id_code_rel.id_type[0]=='B')
				set_zd_data( "0330", g_cif_id_code_rel.id_no );								
   		}
   		else if( ret!=100 )
   		{
        	sprintf( acErrMsg,"²éÑ¯¿Í»§Ö¤¼þÓë¿Í»§ºÅ¶ÔÕÕ±íÒì³£!" );
     	  	WRITEMSG
       		goto ErrExit;
   		}
	}

	Cif_id_code_rel_Clo_Sel();

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

