#include "find_debug.h" 
/*************************************************
* ÎÄ ¼ş Ãû:  pubf_base_product.c
* ¹¦ÄÜÃèÊö£º
*           °üÀ¨ »îÆÚ²úÆ·ÊôĞÔ¼ì²éº¯Êı pub_base_check_current();	
* 				 ¶¨ÆÚ²úÆ·ÊôĞÔ¼ì²éº¯Êı pub_base_check_terminate();			
*                »îÆÚ²úÆ·±ä¸üº¯Êı pub_base_change_current();	
* 				 ¼ì²é¿Í»§ÀàĞÍÓë¹ºÂò²úÆ·ÖÖÀàÊÇ·ñÏà·ûº¯Êı pub_base_check_cltprdt;
* 				 Éú³ÉÕÊ»§ID²¢Ìí¼ÓÕË»§ĞòºÅID±íº¯Êı pub_base_get_cltid();			
* 				 Éú³ÉÄÚ²¿ÕÊÕÊºÅIDº¯Êı pub_base_get_inid();			
*		 		 È¡¿Í»§ÕË»§ĞòºÅ pub_base_get_cltidseq();
*				 ¸ù¾İ²úÆ·´úÂëÈ¡²úÆ·²ÎÊıº¯Êı	pub_base_get_prdtparm();©
*				 ÏÔÊ¾ÕËºÅ×ª»»³ÉÕËºÅIDµÄº¯Êı pub_base_disid_cltid();
*				 ¾ÉÕËºÅ×ª»»³ÉĞÂÕËºÅº¯Êı£ºpub_base_oldac_newac();
*                ÓÉÕË»§IDºÍÕË»§ĞòºÅ²éÑ¯²úÆ·Ãû³Æ    pub_base_get_prdt_name
* 				 ÓÉ²úÆ·ºÅ²éÑ¯²úÆ·Ãû³Æ	pub_base_prdt_name    
* 				 ¸ù¾İ¿ÆÄ¿¿ØÖÆ×ÖÈ¡¿ÆÄ¿ĞÅÏ¢	pub_base_item_inf    
* 				 ¸ù¾İÆ¾Ö¤ÀàĞÍÈ¡Æ¾Ö¤Ãû³Æ    	pub_base_get_note_name   
* ×÷    Õß: rob
* Íê³ÉÈÕÆÚ£º2003/12/27
* ĞŞ¸Ä¼ÇÂ¼£º  
* 1. ÈÕ   ÆÚ:
*    ĞŞ ¸Ä ÈË:
*    ĞŞ¸ÄÄÚÈİ:
* 
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "td_mst_hst_c.h"
#include "clt_id_seq_c.h"
#include "in_id_seq_c.h"
#include "cif_basic_inf_c.h"
#include "cif_id_code_rel_c.h"
#include "prdt_ac_id_c.h"
#include "mo_merge_ac_c.h"
#include "mdm_unprt_hst_c.h"
#include "ac_seqn_id_c.h"
#include "com_item_c.h"
#include "note_parm_c.h"

struct  pub_tx f_pub_tx;
struct  dd_parm_c f_dd_parm;
struct  dd_mst_c f_dd_mst;
struct  td_mst_c f_td_mst;
struct  td_mst_hst_c f_td_mst_hst;
struct  clt_id_seq_c g_clt_id_seq;					/*¿Í»§ÕË»§id½á¹¹*/
struct  in_id_seq_c g_in_id_seq;					/*ÄÚ²¿ÕË»§id½á¹¹*/
struct  cif_basic_inf_c g_cif_basic_inf;			/*¿Í»§»ù±¾ĞÅÏ¢½á¹¹*/
struct  cif_id_code_rel_c g_cif_id_code_rel;		/*¿Í»§Ö¤¼şÓë¿Í»§ºÅ¶ÔÕÕ½á¹¹*/
struct  prdt_ac_id_c g_prdt_ac_id;					/*²úÆ·ÕËºÅ¶ÔÕÕ½á¹¹*/
struct  mo_merge_ac_c g_mo_merge_ac;				/*ĞÂ¾ÉÕËºÅ¶ÔÕÕ½á¹¹*/
struct  mdm_unprt_hst_c g_mdm_unprt_hst;			/*Î´´òÓ¡Ã÷Ï¸ÕË½á¹¹*/
struct  ac_seqn_id_c ac_seqn_id;					/*ÕË»§ĞòºÅID±í½á¹¹*/
struct  com_item_c com_item;						/* ¹«¹²¿ÆÄ¿±í */
struct	note_parm_c	note_parm;						/* Æ¾Ö¤²ÎÊı±í */

static int ret=0;

/**********************************************************************
 * º¯ Êı Ãû£ºpub_base_check_current();
 * º¯Êı¹¦ÄÜ£º»îÆÚ²úÆ·ÊôĞÔ¼ì²éº¯Êı
 *           
 * ×÷Õß/Ê±¼ä£º
 * 
 * ²ÎÊı£º
 *    ÊäÈë£º
 *       	½»Ò×ÀàĞÍ		char(1);	
 *		
 *     Êä³ö: 
 *           
 *   ·µ»ØÖµ£º
 *           ·µ»Ø0³É¹¦£¬
 *           ·µ»Ø·Ç0Ê§°Ü£»
 * ĞŞ¸ÄÀúÊ·£º
 *          
********************************************************************/
int pub_base_check_current(tran_type)
	char *tran_type;
{
	/*¸ù¾İ»îÆÚ²úÆ·±àºÅÈ¡»îÆÚ²ÎÊı±í*/
	MEMSET_DEBUG( &g_dd_parm,0x00,sizeof(struct dd_parm_c) );

	ret = Dd_parm_Sel(g_pub_tx.reply, &g_dd_parm, 
						"prdt_no='%s'", g_pub_tx.prdt_code);
	if ( ret==100 )
	{
		sprintf(acErrMsg,"»îÆÚ²úÆ·²ÎÊı±íÖĞ²»´æÔÚ¸Ã¼ÇÂ¼[%d]",ret);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "W024" );
		return 1;
	}
	else if ( ret && ret!=100 )
	{
		sprintf(acErrMsg,"²éÑ¯»îÆÚ²úÆ·²ÎÊı±íÒì³£[%d]",ret);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "W025" );
		return 1;
	}

	/*¸ù¾İ½»Ò×ÀàĞÍ,¼ì²éÏà¹Ø²ÎÊı*/
	if (tran_type[0]=='1')							/*1-¿ª»§*/
	{
		/*¼ì²é¿ª»§ÈÕÆÚÊÇ·ñÔÚÆğÊ¼ÈÕÆÚºÍÖÕÖ¹ÈÕÆÚÖ®¼ä*/	
		if ( (g_dd_parm.beg_date > g_dd_mst.opn_date) || \
			(g_dd_mst.opn_date > g_dd_parm.end_date))
		{	
			strcpy(acErrMsg,"open date is not between begin and end date!");
			WRITEMSG
			strcpy ( g_pub_tx.reply , "A001" );
			return 1;
		}

		/*¼ì²é¿ª»§Í¨¶Ò±êÖ¾*/
		if (g_dd_parm.thr_opn_ind[0]=='N')
		{	
			strcpy(acErrMsg,"thr_opn_ind is N,open failed!");
			WRITEMSG
			strcpy ( g_pub_tx.reply , "A002" );
			return 1;
		}

		/*¼ì²é¸Ã²úÆ·×îµÍ¿ª»§½ğ¶îºÍ×î¸ß¿ª»§½ğ¶î*/
		if ( g_dd_mst.bal<g_dd_parm.min_opn_amt )
		{
			strcpy(acErrMsg,"open bal is too low!! ");
			WRITEMSG
			strcpy ( g_pub_tx.reply , "A003" );
			return 1;
		}
		else if ( g_dd_mst.bal>g_dd_parm.max_opn_amt)
		{
			strcpy(acErrMsg,"open bal overflow!! ");
			WRITEMSG
			strcpy ( g_pub_tx.reply , "A004" );
			return 1;
		}

		/*¼ì²éÀûÂÊÊÇ·ñ³¬¹ı¹æ¶¨ÀûÂÊÉÏ¸¡ºÍÏÂ¸¡ºóµÄ½çÏŞ*/
		if ( g_dd_mst.rate < g_dd_mst.rate - g_dd_parm.min_flt_ratio )
		{
			strcpy(acErrMsg,"current rate is too low!! ");
			WRITEMSG
			strcpy ( g_pub_tx.reply , "A005" );
			return 1;
		}
		else if ( g_dd_mst.rate > g_dd_mst.rate + g_dd_parm.max_flt_ratio )
		{
			strcpy(acErrMsg,"current rate is too high!! ");
			WRITEMSG
			strcpy ( g_pub_tx.reply , "A006" );
			return 1;
		}

		/* ¼ì²é¿ª»§ÏŞÖÆÌõ¼ş±àÂë (µ÷ÓÃº¯Êı) */
		if ( g_dd_parm.opn_lmt == 000 )
		{
			return 1;
		}
 	}
	else if(tran_type[0]=='2')						/*2-Ïú»§*/
	{
		/*¼ì²éÏú»§Í¨¶Ò±êÖ¾*/
		if ( !strcmp( g_dd_parm.thr_cls_ind,"N") )
		{	
			strcpy(acErrMsg,"thr_cls_ind is N,cls failed!");
			WRITEMSG
			strcpy ( g_pub_tx.reply , "A007" );
			return 1;
		}
		/* ¼ì²éÏú»§ÏŞÖÆÌõ¼ş±àÂë (µ÷ÓÃº¯Êı) */
		if ( g_dd_parm.cls_lmt == 000 )
		{
			
		}
	}
	else if (tran_type[0]=='3')				/*3-´æ¿î*/
	{
		MEMSET_DEBUG( &f_dd_mst, 0x00, sizeof(struct dd_mst_c) );

		ret = Dd_mst_Sel(g_pub_tx.reply, &f_dd_mst,
						"prdt_no='%s' and ac_id=%d and ac_seqn=%d",
						g_pub_tx.prdt_code,g_pub_tx.ac_id,g_pub_tx.ac_seqn);
	
		if ( ret==100 )
		{
			sprintf(acErrMsg,"»îÆÚÖ÷ÎÄ¼şÖĞ²»´æÔÚ¸Ã¼ÇÂ¼[%d]",ret);
			WRITEMSG
			strcpy ( g_pub_tx.reply , "W014" );
			return 1;
		}
		else if ( ret!=0 )
		{
			sprintf(acErrMsg,"²éÑ¯»îÆÚÖ÷ÎÄ¼şÒì³£[%d]",ret);
			WRITEMSG
			strcpy ( g_pub_tx.reply , "W015" );
			return 1;
		}

		/*¼ì²é´æ¿îºó½ğ¶îÊÇ·ñ³¬¹ı×î´ó½ğ¶î*/
		if( ( g_dd_mst.bal+g_pub_tx.tx_amt1) > g_dd_parm.max_bal )	
		{
			strcpy(acErrMsg,"amt is too high!");
			WRITEMSG
			strcpy ( g_pub_tx.reply , "A008" );
			return 1;
		}

		/*¼ì²éĞø´æ±êÖ¾*/
		if (g_dd_parm.dpst_ind[0]=='N')
		{
			strcpy(acErrMsg,"dpst_ind is N,dpst failed!");
			WRITEMSG
			strcpy ( g_pub_tx.reply , "A009" );
			return 1;
		}

		/*¼ì²éÍ¨´æ±êÖ¾*/
		if ( g_dd_parm.thr_dpst_ind[0]=='N')
		{	
			strcpy(acErrMsg,"thr_dpst_ind is N, thr_dpst_ind failed!");
			WRITEMSG
			strcpy ( g_pub_tx.reply , "A010" );
			return 1;
		}

		/* ¼ì²é´æ¿îÏŞÖÆÌõ¼ş±àÂë (µ÷ÓÃº¯Êı)
		if ( g_dd_parm.cls_lmt == 000 )
		{

		} */
	}
	else if(tran_type[0]=='4')							/*4-È¡¿î*/
	{
		/*¼ì²éĞøÈ¡±êÖ¾*/
		if (g_dd_parm.dpst_ind[0]=='N')
		{
			strcpy(acErrMsg,"dpst_ind is N,dpst failed!");
			WRITEMSG
			strcpy ( g_pub_tx.reply , "A009" );
			return 1;
		}

		/*¼ì²éÍ¨¶Ò±êÖ¾*/
		if (g_dd_parm.thr_draw_ind[0]=='N')
		{
			strcpy(acErrMsg,"thr_draw_ind is N,thr_draw failed!");
			WRITEMSG
			strcpy ( g_pub_tx.reply , "A011" );
			return 1;
		}

		/*¼ì²éÌáÏÖ±êÖ¾*/
		if (g_pub_tx.ct_ind[0]=='1')
		{
			if(g_dd_parm.cash_ind[0]=='N')
			{
				strcpy(acErrMsg,"cash_ind is N,take cash is refused!");
				WRITEMSG
				strcpy ( g_pub_tx.reply , "A012" );
				return 1;
			}
		}
	}

   return 0;
}

/**********************************************************************
 * º¯ Êı Ãû£ºpub_base_check_terminate();
 * º¯Êı¹¦ÄÜ£º¶¨ÆÚ²úÆ·ÊôĞÔ¼ì²éº¯Êı
 *           
 * ×÷Õß/Ê±¼ä£º
 * 
 * ²ÎÊı£º
 *     ÊäÈë£º
 *           ½»Ò×ÀàĞÍ 				tran_type(2);
 *     Êä³ö: 
 *         	 ÏìÓ¦Âë
 *   ·µ»ØÖµ£º
 *           ·µ»Ø0³É¹¦£¬
 *           ·µ»Ø·Ç0Ê§°Ü£»
 * ĞŞ¸ÄÀúÊ·£º
 *          
********************************************************************/
int pub_base_check_terminate(tran_type)
	char *tran_type;
{
	int days;

	/*¸ù¾İ¶¨ÆÚ²úÆ·±àºÅÈ¡¶¨ÆÚ²úÆ·²ÎÊı*/
	MEMSET_DEBUG( &g_td_parm,0x00,sizeof(struct td_parm_c) );

	ret = Td_parm_Sel(g_pub_tx.reply, &g_td_parm, 
					"prdt_no='%s'", g_pub_tx.prdt_code);
	if ( ret==100 )
	{
		sprintf(acErrMsg,"¶¨ÆÚ²úÆ·²ÎÊı±íÖĞ²»´æÔÚ¸Ã¼ÇÂ¼[%d]",ret);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "W026" );
		return 1;
	}
	else if (ret && ret!=0 )
	{
		sprintf(acErrMsg,"²éÑ¯¶¨ÆÚ²úÆ·²ÎÊı±íÒì³£[%d]",ret);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "W027" );
		return 1;
	}

	/*¸ù¾İÕË»§id,ÕË»§ĞòºÅÈ¡¶¨ÆÚ²úÆ·Ö÷ÎÄ¼ş*/
	MEMSET_DEBUG( &f_td_mst, 0x00, sizeof(struct td_mst_c) );

	ret = Td_mst_Sel(g_pub_tx.reply, &f_td_mst,
 					"prdt_no='%s' and ac_id=%ld and ac_seqn=%d",
					g_pub_tx.prdt_code,g_pub_tx.ac_id,g_pub_tx.ac_seqn);
	
	if ( ret==100 )
	{
		sprintf(acErrMsg,"¶¨ÆÚ²úÆ·Ö÷ÎÄ¼şÖĞ²»´æÔÚ¸Ã¼ÇÂ¼[%d]",ret);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "W016" );
		return 1;
	}
	else if (ret && ret!=0 )
	{
		sprintf(acErrMsg,"²éÑ¯¶¨ÆÚ²úÆ·Ö÷ÎÄ¼şÒì³£[%d]",ret);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "W017" );
		return 1;
	}

	/*¸ù¾İÕË»§id,ÕË»§ĞòºÅÈ¡¶¨ÆÚ´æ¿îÃ÷Ï¸±í*/
	MEMSET_DEBUG( &f_td_mst_hst, 0x00, sizeof(struct td_mst_hst_c) );

	ret = Td_mst_hst_Sel(g_pub_tx.reply, &f_td_mst_hst,
						"ac_id=%ld and ac_seqn=%d and max(trace_no)",
						g_pub_tx.ac_id , g_pub_tx.ac_seqn);
	if ( ret==100 )
	{
		sprintf(acErrMsg,"¶¨ÆÚ²úÆ·Ã÷Ï¸±íÖĞ²»´æÔÚ¸Ã¼ÇÂ¼[%d]",ret);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "W028" );
		return 1;
	}
	else if ( ret && ret!=100 )
	{
		sprintf(acErrMsg,"²éÑ¯¶¨ÆÚ²úÆ·Ã÷Ï¸±íÒì³£[%d]",ret);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "W029" );
		return 1;
	}

	switch( atoi(tran_type) )
	{
	  case 1:										/*1-¿ª»§*/
			/*¼ì²é¿ª»§ÈÕÆÚÊÇ·ñÔÚÆğÊ¼ÈÕÆÚºÍÖÕÖ¹ÈÕÆÚÖ®¼ä*/	
			if ( (g_td_parm.beg_date > g_td_mst.opn_date) || \
				(g_td_mst.opn_date > g_td_parm.end_date))
			{	
				strcpy(acErrMsg,"open date is not between begin and end date!");
				WRITEMSG
				strcpy ( g_pub_tx.reply , "A001" );
				break;
			}

			/*¼ì²éÍ¨¶Ò¿ª»§±êÖ¾*/
			if (g_td_parm.thr_opn_ind[0]=='N')
			{	
				strcpy(acErrMsg,"thr_opn_ind is N,open failed!");
				WRITEMSG
				strcpy ( g_pub_tx.reply , "A002" );
				break;
			}

			/*¼ì²é¸Ã²úÆ·×îµÍ¿ª»§½ğ¶îºÍ×î¸ß¿ª»§½ğ¶î*/
			if ( g_td_mst.bal<g_td_parm.min_opn_amt )
			{
				strcpy(acErrMsg,"open bal is too low!! ");
				WRITEMSG
				strcpy ( g_pub_tx.reply , "A003" );
				break;
			}
			else if ( g_td_mst.bal>g_td_parm.max_opn_amt)
			{
				strcpy(acErrMsg,"open bal overflow!! ");
				WRITEMSG
				strcpy ( g_pub_tx.reply , "A004" );
				break;
			}

			/*¼ì²éÀûÂÊÊÇ·ñ³¬¹ı¹æ¶¨ÀûÂÊÉÏ¸¡ºÍÏÂ¸¡ºóµÄ½çÏŞ */
			if ( g_td_mst.rate < g_td_mst.rate - g_td_parm.min_flt_ratio )
			{
				strcpy(acErrMsg,"current rate is too low!! ");
				WRITEMSG
				strcpy ( g_pub_tx.reply , "A005" );
				break;
			}
			else if ( g_td_mst.rate > g_td_mst.rate + g_td_parm.max_flt_ratio )
			{
				strcpy(acErrMsg,"current rate is too high!! ");
				WRITEMSG
				strcpy ( g_pub_tx.reply , "A006" );
				break;
			}

			/* ¼ì²é¿ª»§ÏŞÖÆÌõ¼ş±àÂë */
			if ( g_td_parm.opn_lmt == 000 )
			{
				break;
			}
			break;
	case 2:										/*2-Ïú»§*/
			/*¼ì²éÔ¤Ô¼×ª´æ±êÖ¾*/
			if (g_td_parm.tfr_lmt_ind[0]=='N')
			{	
				if (g_td_mst.tfr_ind[0]=='Y')
				{
					strcpy(acErrMsg,"tfr_ind is N,transfer failed!");
					WRITEMSG
					strcpy ( g_pub_tx.reply , "A013" );
					break;
				}
			}
			
			/*¼ì²éÊÇ·ñÌáÇ°Ö§È¡*/
			if (g_td_parm.fwd_draw_ind[0]=='N')
			{
				if ( g_pub_tx.tx_date < f_td_mst.mtr_date )
				{
					strcpy(acErrMsg,"advance take cash is refused!");
					WRITEMSG
					strcpy ( g_pub_tx.reply , "A014" );
					break;
				}
			}	

			/*¼ì²éÏú»§Í¨¶Ò±êÖ¾*/
			if (g_td_parm.thr_cls_ind[0]=='N')
			{	
				strcpy(acErrMsg,"thr_cls_ind is N,cls failed!");
				WRITEMSG
				strcpy ( g_pub_tx.reply , "A007" );
				break;
			}

			/*¼ì²éÏú»§ÏŞÖÆÌõ¼ş±àÂë*/
			if ( g_td_parm.cls_lmt == 000 )
			{

				break;
			}
			break;

	 case 3:									/*3-´æ¿î*/
			/*¼ì²é´æ¿îºó½ğ¶îÊÇ·ñ³¬¹ı×î´ó½ğ¶î*/
			if( ( g_td_mst.bal + g_pub_tx.tx_amt1) > g_td_parm.max_bal )	
			{
				strcpy(acErrMsg,"amt is too high!");
				WRITEMSG
				strcpy ( g_pub_tx.reply , "A008" );
				break;
			}

			/*¼ì²éĞø´æ±êÖ¾*/
			if (g_td_parm.dpst_ind[0]=='N')
			{
				strcpy(acErrMsg,"dpst_ind is N,dpst failed!");
				WRITEMSG
				strcpy ( g_pub_tx.reply , "A009" );
				break;
			}
			else if ( (g_td_parm.dpst_ind[0]=='Y') || \
					 (g_td_parm.dedr_intvl != 0) )
			{
				if( g_td_parm.intvl_type[0]=='Y')	
				{
					days = 365 * g_td_parm.dedr_intvl;
				}
				else if( g_td_parm.intvl_type[0]=='Q')
				{
					days = 90 * g_td_parm.dedr_intvl;
				}
				else if( g_td_parm.intvl_type[0]=='M')
				{
					days = 30 * g_td_parm.dedr_intvl;
				}
				else if( g_td_parm.intvl_type[0]=='D')
				{
					days = g_td_parm.dedr_intvl;
				}

				if ( (g_pub_tx.tx_date - f_td_mst_hst.tx_date) < days )	
				{
					strcpy(acErrMsg,"save days are not enough!");
					WRITEMSG
					strcpy ( g_pub_tx.reply , "A016" );
					break;
				}
			}

			/*¼ì²é×îµÍĞø´æ½ğ¶î*/
			if ( g_pub_tx.tx_amt1 < g_td_parm.dpst_min_amt )
			{
				strcpy(acErrMsg,"save money is too low,save failed!");
				WRITEMSG
				strcpy ( g_pub_tx.reply , "A015" );
				break;
			}

			/*¼ì²éÍ¨´æ±êÖ¾*/
			if (g_dd_parm.thr_dpst_ind[0]=='N')
			{	
				strcpy(acErrMsg,"thr_dpst_ind is N, thr_dpst_ind failed!");
				WRITEMSG
				strcpy ( g_pub_tx.reply , "A010" );
				break;
			}

			/* ¼ì²é´æ¿îÏŞÖÆÌõ¼ş±àÂë*/
			if ( g_dd_parm.cls_lmt == 000 )
			{

				break;
			}
			break;

	case 4:									/*4-È¡¿î*/
			/*¼ì²éÕËºÅÖ§È¡ºóÓà¶îÊÇ·ñµÍÓÚ²úÆ·×îµÍÓà¶î*/
			if ( (g_td_mst.bal - g_pub_tx.tx_amt1) < g_td_parm.max_bal )
			{
				strcpy(acErrMsg,"after take,remain is not enough!");
				WRITEMSG
				strcpy ( g_pub_tx.reply , "A017" );
				break;
			}

			/*¼ì²é×îµÍĞøÈ¡½ğ¶î*/
			if ( g_pub_tx.tx_amt1 < g_td_parm.draw_min_amt )
			{
				strcpy(acErrMsg,"draw cash is too low,draw failed!");
				WRITEMSG
				strcpy ( g_pub_tx.reply , "A015" );
				break;
			}

			/*¼ì²éĞøÈ¡ÆÚÏŞ*/
			if ( g_td_parm.dedr_intvl != 0 )
			 {
				if(g_td_parm.intvl_type[0]=='Y')
				{
				 	days = 365 * g_td_parm.dedr_intvl;
				}
			    else if(g_td_parm.intvl_type[0]=='Q')
			  	{
				    days = 90 * g_td_parm.dedr_intvl;
				}
		 		else if(g_td_parm.intvl_type[0]=='M')
			    {
				   	days = 30 * g_td_parm.dedr_intvl;
				}
				else if(g_td_parm.intvl_type[0]=='D')
				{
					days = g_td_parm.dedr_intvl;
				}

				if ( ( g_pub_tx.tx_date - f_td_mst_hst.tx_date )<days )
				{
					strcpy(acErrMsg,"save days are not enough!");
					WRITEMSG
					strcpy ( g_pub_tx.reply , "A016" );
					break;
				}
			}

			/*¼ì²éÍ¨¶Ò±êÖ¾*/
			if ( g_td_parm.thr_draw_ind[0]=='N')
			{
				strcpy(acErrMsg,"thr_draw_ind is N,thr_draw failed!");
				WRITEMSG
				strcpy ( g_pub_tx.reply , "A011" );
				break;
			}

			/*¼ì²éÌáÏÖ±êÖ¾*/
			if (g_pub_tx.ct_ind[0]=='1')
			{
				if(g_td_parm.cash_ind[0]=='N')
				{
					strcpy(acErrMsg,"cash_ind is N,take cash is refused!");
					WRITEMSG
					strcpy ( g_pub_tx.reply , "A012" );
					break;
				}
			}	
			break;
	case 5:									/*5-²¿Ìá*/
			/*¼ì²é²¿Ìá±êÖ¾*/
			if(g_td_parm.wdr_ind[0]=='N')
			{
				strcpy(acErrMsg,"take part cash is refused!");
				WRITEMSG
				strcpy ( g_pub_tx.reply , "A018" );
				break;
			}

			/*¼ì²é²¿Ìá´ÎÊı*/
			if (g_td_parm.wdr_lmt_cnt != 0 && \
				g_td_mst.tx_cnt == g_td_parm.wdr_lmt_cnt)
			{
				strcpy(acErrMsg,"take part cash times is full!");
				WRITEMSG
				strcpy ( g_pub_tx.reply , "A019" );
				break;
			}

	default:break;		

	}
 return 0;
}

/**********************************************************************
 * º¯ Êı Ãû£ºpub_base_change_current();
 * º¯Êı¹¦ÄÜ£º»îÆÚ²úÆ·±ä¸üº¯Êı
 *           ½«ÕË»§¶ÔÓ¦µÄ»îÆÚ²úÆ·¸Ä±ä
 * ×÷Õß/Ê±¼ä£º
 * 
 * ²ÎÊı£º
 *     ÊäÈë£º ÕË»§id		long;
 *     		  ÕË»§ĞòºÅ		int;
 *     Êä³ö: 
 *          	ÏìÓ¦Âë
 *   ·µ»ØÖµ£º
 *           ·µ»Ø0³É¹¦£¬
 *           ·µ»Ø·Ç0Ê§°Ü£»
 * ĞŞ¸ÄÀúÊ·£º
 *          
********************************************************************/
int pub_base_change_current( ac_id, ac_seq )
	long ac_id;
	int ac_seq; 
{
	char tran_tp[2];

	/*¸ù¾İÕÊ»§IDºÍÕË»§ĞòºÅÕÒµ½Ô­»îÆÚ²úÆ·±àºÅ*/
	MEMSET_DEBUG( &g_dd_mst, 0x00, sizeof(struct dd_mst_c) );

	ret = Dd_mst_Sel(g_pub_tx.reply, &g_dd_mst, "ac_id=%ld and ac_seq=%d",
						g_pub_tx.ac_id,g_pub_tx.ac_seqn);
	if ( ret==100 )
	{
		sprintf(acErrMsg,"»îÆÚ²úÆ·Ö÷ÎÄ¼şÖĞ²»´æÔÚ¸Ã¼ÇÂ¼[%d]",ret);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "W014" );
		return 1;
	}
	else if (ret && ret!=100 )
	{
		sprintf(acErrMsg,"²éÑ¯»îÆÚ²úÆ·Ö÷ÎÄ¼şÒì³£[%d]",ret);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "W015" );
		return 1;
	}

	/*¸ù¾İÔ­»îÆÚ²úÆ·±àºÅÈ¡³öÔ­»îÆÚ²úÆ·²ÎÊı½á¹¹*/
	MEMSET_DEBUG( &g_dd_parm, 0x00, sizeof(struct dd_parm_c) );
	ret = Dd_parm_Sel(g_pub_tx.reply, &g_dd_parm, "prdt_no='%s'",
						g_dd_mst.prdt_no);
	if ( ret==100 )
	{
		sprintf(acErrMsg,"»îÆÚ²úÆ·²ÎÊı±íÖĞ²»´æÔÚ¸Ã¼ÇÂ¼[%d]",ret);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "W024" );
		return 1;
	}
	else if ( ret && ret!=100 )
	{
		sprintf(acErrMsg,"²éÑ¯»îÆÚ²úÆ·²ÎÊı±íÒì³£[%d]",ret);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "W025" );
		return 1;
	}

	/*¼ì²é¾É»îÆÚ²úÆ·ÊÇ·ñ·ûºÏĞÂ»îÆÚ²úÆ·ÊôĞÔ*/
	tran_tp[0]='1';
	ret = pub_base_check_current(tran_tp);

	/*¸ù¾İ²úÆ·±àºÅÈ¡ĞÂ²úÆ·²ÎÊı*/
	MEMSET_DEBUG( &f_dd_parm, 0x00, sizeof(struct dd_parm_c) );

	ret = Dd_parm_Sel(g_pub_tx.reply, &f_dd_parm, 
						"prdt_no='%s'", g_pub_tx.prdt_code);
	if ( ret==100 )
	{
		sprintf(acErrMsg,"»îÆÚ²úÆ·²ÎÊı±íÖĞ²»´æÔÚ¸Ã¼ÇÂ¼[%d]",ret);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "W024" );
		return 1;
	}
	else if ( ret && ret!=100 )
	{
		sprintf(acErrMsg,"²éÑ¯»îÆÚ²úÆ·²ÎÊı±íÒì³£[%d]",ret);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "W025" );
		return 1;
	}
	
	/*ĞŞ¸Ä»îÆÚ²úÆ·Ö÷ÎÄ¼ş*/
	ret = Dd_mst_Dec_Upd(g_pub_tx.reply, "ac_id=%ld and ac_seqn=%d", 
							g_pub_tx.ac_id,g_pub_tx.ac_seqn);
	if ( ret )
	{
		sprintf(acErrMsg,"declare Dd_mst_Dec_Upd error code=[%d]",ret);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "D101" );
		return 1;
	}

	ret = Dd_mst_Fet_Upd( &g_dd_mst,g_pub_tx.reply );
	if ( ret&&ret!=100 )
	{
		sprintf(acErrMsg,"fetch Dd_mst_Fet_Upd error code=[%d]",ret);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "D102" );
		return 1;
	}
	else if ( ret==100 )
	{
		sprintf(acErrMsg,"fetch Dd_mst_Fet_Upd error code=[%d]",ret);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "D104" );
		return 1;
	}

	strcpy( g_dd_mst.prdt_no,g_pub_tx.prdt_code );

	ret = Dd_mst_Upd_Upd( g_dd_mst, g_pub_tx.reply );
	if ( ret )
	{
		sprintf(acErrMsg,"update Dd_mst_Upd_Upd error code=[%d]",ret);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "D105" );
		return 1;
	}

	Dd_mst_Clo_Upd( );

	return 0;
}

/**********************************************************************
 * º¯ Êı Ãû£ºpub_base_check_cltprdt();
 * º¯Êı¹¦ÄÜ£º¼ì²é¿Í»§ÀàĞÍÓë¹ºÂò²úÆ·ÖÖÀàÊÇ·ñÏà·ûº¯Êı
 *			 Èç¸öÈË¿Í»§²»ÄÜ¹ºÂò¶Ô¹«²úÆ·£¬¶Ô¹«¿Í»§²»ÄÜ¹ºÂò¸öÈË²úÆ·¡£
 *
 * ×÷Õß/Ê±¼ä£ºrob/2003/12/31
 * 
 * ²ÎÊı£º
 *     ÊäÈë£º	¿Í»§ÀàĞÍ		clt_type	char(2);
 *          	²úÆ·±àºÅ		prdt_no		char(4);
 *     Êä³ö: 
 *          	ÏìÓ¦Âë
 *   ·µ»ØÖµ£º
 *           ·µ»Ø0³É¹¦£¬
 *           ·µ»Ø·Ç0Ê§°Ü£»
 * ĞŞ¸ÄÀúÊ·£º
 *          
********************************************************************/
int pub_base_check_cltprdt( clt_type, prdt_no )
	char *clt_type;
	char *prdt_no;
{
	char prdt_seqn[2];
	char str_tt[41];

	struct ln_parm_c	ln_parm_cif;
	prdt_seqn[0]=prdt_no[0];

	MEMSET_DEBUG( &ln_parm_cif, 0x00, sizeof(struct ln_parm_c) );
	MEMSET_DEBUG( str_tt, 0x0 , sizeof( str_tt ) );
	
	switch(atoi(prdt_seqn))
	{
		case 1:
				/*²éÑ¯»îÆÚ²ÎÊı±í*/
				MEMSET_DEBUG( &g_dd_parm, 0x00, sizeof(struct dd_parm_c) );
				ret = Dd_parm_Sel(g_pub_tx.reply, &g_dd_parm, 
								"prdt_no='%s' and cif_type='%s'",
								prdt_no , clt_type);
				if ( ret==100 )
				{
					sprintf(acErrMsg,"»îÆÚ²úÆ·²ÎÊı±íÖĞ²»´æÔÚ¸Ã¼ÇÂ¼[%d]",ret);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "W024" );
					return 1;
				}
				else if ( ret && ret!=0 )
				{
					sprintf(acErrMsg,"²éÑ¯»îÆÚ²úÆ·²ÎÊı±íÒì³£[%d]",ret);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "W025" );
					return 1;
				}
				break;
		case 2:
				/*²éÑ¯¶¨ÆÚ²ÎÊı±í*/
				MEMSET_DEBUG( &g_td_parm, 0x00, sizeof(struct td_parm_c) );
				ret = Td_parm_Sel(g_pub_tx.reply, &g_td_parm, 
									"prdt_no='%s' and cif_type='%s'", 
									prdt_no , clt_type);
				if ( ret==100 )
				{
					sprintf(acErrMsg,"¶¨ÆÚ²úÆ·²ÎÊıÖĞ²»´æÔÚ¸Ã¼ÇÂ¼[%d]",ret);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "W026" );
					return 1;
				}
				else if (ret && ret!=0 )
				{
					sprintf(acErrMsg,"²éÑ¯¶¨ÆÚ²úÆ·²ÎÊı±íÒì³£[%d]",ret);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "W027" );
					return 1;
				}
				break;
		case 3:
				/*²éÑ¯´û¿î²ÎÊı±í*/
				if( clt_type[0]=='1' )
				strcpy( str_tt," cif_type in('1','0') " );
				if( clt_type[0]=='2' )
				strcpy( str_tt," cif_type in('0','2') " );
				if( clt_type[0]=='3' )
				strcpy( str_tt," cif_type in('0','2') " );
				if( clt_type[0]=='4' )
				strcpy( str_tt," cif_type in('0','2') " );
				if( clt_type[0]=='5' )
				strcpy( str_tt," cif_type in('1','0') " );

				sprintf(acErrMsg,"str_tt![%s]",str_tt);
				WRITEMSG
				
				ret =Ln_parm_Sel(g_pub_tx.reply, &ln_parm_cif, 
								"prdt_no='%s' and %s ", prdt_no , str_tt );
				if ( ret==100 )
				{
					sprintf(acErrMsg,"¸Ã¿Í»§¿Í»§ÀàĞÍÓë´û¿î²úÆ·ÊÊºÏµÄ¿Í»§ÀàĞÍ²»·û![%d]",ret);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "L146" );
					return 1;
				}
				else if (ret && ret!=0 )
				{
					sprintf(acErrMsg,"²éÑ¯´û¿î²úÆ·²ÎÊı±íÒì³£[%d]",ret);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "W027" );
					return 1;
				}
				break;
		case 4:
				/*²éÑ¯Í¸Ö§²ÎÊı±í*/
				MEMSET_DEBUG( &g_od_parm, 0x00, sizeof(struct od_parm_c) );
				ret =Od_parm_Sel(g_pub_tx.reply, &g_od_parm, 
								"prdt_no='%s' and cif_type='%s'", 
								prdt_no , clt_type);
				if ( ret==100 )
				{
					sprintf(acErrMsg,"Í¸Ö§²úÆ·²ÎÊı±íÖĞ²»´æÔÚ¸Ã¼ÇÂ¼[%d]",ret);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "W026" );
					return 1;
				}
				else if (ret && ret!=0 )
				{
					sprintf(acErrMsg,"²éÑ¯Í¸Ö§²úÆ·²ÎÊı±íÒì³£[%d]",ret);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "W027" );
					return 1;
				}
				break;
		default:
				break;
		}
return 0;
}

/**********************************************************************
 * º¯ Êı Ãû£ºpub_base_get_cltid();
 * º¯Êı¹¦ÄÜ£ºÉú³É¿Í»§ÕÊ»§IDº¯Êı£¬²¢Ìí¼ÓÕË»§ĞòºÅID±í
 *			 
 *
 * ×÷Õß/Ê±¼ä£º
 * 
 * ²ÎÊı£º
 *     ÊäÈë£º	ÎŞ
 *          
 *     Êä³ö: 	
 *				¿Í»§ÕË»§ID		long;
 *          	ÏìÓ¦Âë
 *   ·µ»ØÖµ£º
 *           ·µ»Ø0³É¹¦£¬
 *           ·µ»Ø·Ç0Ê§°Ü£»
 * ĞŞ¸ÄÀúÊ·£ºlance/20050112
 * ĞŞ¸ÄÔ­Òò£ºÔö¼Ó¶Ô"ÕË»§ĞòºÅID±í"µÄÌí¼Ó²Ù×÷
 *          
********************************************************************/
int pub_base_get_cltid( cltid )
	long	*cltid;
{
	/*È¡×î´ó¿Í»§ÕËºÅid,²¢ĞŞ¸Ä£¬·µ»Ø¿ÉÓÃ×î´ó¿Í»§ÕËºÅid*/
	ret = Clt_id_seq_Dec_Upd (g_pub_tx.reply, " 1=1 ");
	if (ret)
	{
		sprintf(acErrMsg,"SELECT clt_id_seq error code=[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D101");
		return 1;
	}

	ret = Clt_id_seq_Fet_Upd (&g_clt_id_seq,g_pub_tx.reply);
	if (ret&&ret!=100)
	{
		sprintf(acErrMsg,"FETCH clt_id_seq error code=[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D102");
		return 1;
	}
	if (ret==100)
	{
		sprintf(acErrMsg,"FETCH clt_id_seq error code=[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D104");
		return 1;
	}

	g_clt_id_seq.ac_seqn = g_clt_id_seq.ac_seqn + 1;

	ret = Clt_id_seq_Upd_Upd (g_clt_id_seq,g_pub_tx.reply);
	if (ret)
	{
		sprintf(acErrMsg,"update clt_id_seq error code=[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D105");
		return 1;
	}

	Clt_id_seq_Clo_Upd( );

	*cltid = g_clt_id_seq.ac_seqn;
	
	/* Ìí¼ÓÕË»§ĞòºÅID±í */
	ac_seqn_id.ac_id = *cltid;	/* ÕËºÅID */
	ac_seqn_id.ac_seqn_id = 200;	/* ³õÊ¼»¯ÕË»§ĞòºÅ */

	ret = Ac_seqn_id_Ins( ac_seqn_id );
	if( ret )
	{
		sprintf(acErrMsg,"Ìí¼ÓÕË»§ĞòºÅID±íÊ§°Ü![%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"W042");
		return 1;
	}
	sprintf(acErrMsg,"Ìí¼ÓÕË»§ĞòºÅID±í PASS!");
	WRITEMSG

	return 0;

}

int pub_base_crt_cltid( cltid )
	long	*cltid;
{
	/*È¡×î´ó¿Í»§ÕËºÅid,²¢ĞŞ¸Ä£¬·µ»Ø¿ÉÓÃ×î´ó¿Í»§ÕËºÅid*/
	ret = Clt_id_seq_Dec_Upd (g_pub_tx.reply, " 1=1 ");
	if (ret)
	{
		sprintf(acErrMsg,"SELECT clt_id_seq error code=[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D101");
		return 1;
	}

	ret = Clt_id_seq_Fet_Upd (&g_clt_id_seq,g_pub_tx.reply);
	if (ret&&ret!=100)
	{
		sprintf(acErrMsg,"FETCH clt_id_seq error code=[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D102");
		return 1;
	}
	if (ret==100)
	{
		sprintf(acErrMsg,"FETCH clt_id_seq error code=[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D104");
		return 1;
	}

	g_clt_id_seq.ac_seqn = g_clt_id_seq.ac_seqn + 1;

	ret = Clt_id_seq_Upd_Upd (g_clt_id_seq,g_pub_tx.reply);
	if (ret)
	{
		sprintf(acErrMsg,"update clt_id_seq error code=[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D105");
		return 1;
	}

	Clt_id_seq_Clo_Upd( );

	*cltid = g_clt_id_seq.ac_seqn;
	
	return 0;
}

/**********************************************************************
 * º¯ Êı Ãû£ºpub_base_get_inid();
 * º¯Êı¹¦ÄÜ£ºÉú³ÉÄÚ²¿ÕÊ»§IDº¯Êı
 *			 
 *
 * ×÷Õß/Ê±¼ä£ºrob/2003/12/31
 * 
 * ²ÎÊı£º
 *     ÊäÈë£º	ÎŞ
 *          
 *     Êä³ö: 
 *			ÄÚ²¿ÕË»§ID		long;
 *         	ÏìÓ¦Âë
 *   ·µ»ØÖµ£º
 *           ·µ»Ø0³É¹¦£¬
 *           ·µ»Ø·Ç0Ê§°Ü£»
 * ĞŞ¸ÄÀúÊ·£º
 *          
********************************************************************/
int pub_base_get_inid( inid )
	long *inid;
{
	/*È¡×îĞ¡ÄÚ²¿ÕËºÅid,²¢ĞŞ¸Ä£¬·µ»Ø¿ÉÓÃ×îĞ¡ÄÚ²¿ÕËºÅid*/
	ret = In_id_seq_Dec_Upd (g_pub_tx.reply, " 1=1 ");
	if (ret)
	{
		sprintf(acErrMsg,"SELECT clt_id_seq error code=[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D101");
		return 1;
	}

	ret = In_id_seq_Fet_Upd (&g_in_id_seq,g_pub_tx.reply);
	if (ret&&ret!=100)
	{
		sprintf(acErrMsg,"fetch clt_id_seq error code=[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D102");
		return 1;
	}
	else if (ret==100) 
	{
		sprintf(acErrMsg,"fetch clt_id_seq error code=[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D104");
		return 1;
	}

	g_in_id_seq.ac_seqn = g_in_id_seq.ac_seqn + 1;

	ret = In_id_seq_Upd_Upd (g_in_id_seq,g_pub_tx.reply);
	if (ret)
	{
		sprintf(acErrMsg,"update clt_id_seq error code=[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D105");
		return 1;
	}

	In_id_seq_Clo_Upd( );

	*inid = g_in_id_seq.ac_seqn;

	return 0;
}

/**********************************************************************
 * º¯ Êı Ãû£ºpub_base_get_cltidseq();
 * º¯Êı¹¦ÄÜ£ºÈ¡¿Í»§ÕË»§ĞòºÅº¯Êı
 *			 
 *
 * ×÷Õß/Ê±¼ä£ºrob/2003/12/31
 * 
 * ²ÎÊı£º
 *     ÊäÈë£ºÖ¤¼şÀàĞÍ		
 *           Ö¤¼şºÅÂë
 *     Êä³ö: 
 *			¿Í»§ÕË»§ĞòºÅ
 *         	ÏìÓ¦Âë
 *   ·µ»ØÖµ£º
 *           ·µ»Ø0³É¹¦£¬
 *           ·µ»Ø·Ç0Ê§°Ü£»
 * ĞŞ¸ÄÀúÊ·£º
 *          
********************************************************************/
int pub_base_get_cltidseq(id_type,id_num,cif_seq)
	char *id_type;	
	char *id_num;
	long *cif_seq;
{
	/*È¡¿Í»§´úÂë*/
	MEMSET_DEBUG( &g_cif_id_code_rel, 0x00, sizeof(struct cif_id_code_rel_c) );

	ret = Cif_id_code_rel_Sel (g_pub_tx.reply, &g_cif_id_code_rel, 
					 		"id_type='%s' and id_no='%s'", id_type , id_num);
	if ( ret && ret!=100)
	{
		sprintf(acErrMsg,"SELECT cif_id_code_rel error code=[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		return 1;
	}
	else if( ret==100 )
	{
		sprintf(acErrMsg,"SELECT cif_id_code_rel error code=[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D104");
		return 1;
	}

	/*È¡¿Í»§ÕËºÅĞòºÅĞŞ¸Ä,²¢·µ»Ø*/
	ret = Cif_basic_inf_Dec_Upd (g_pub_tx.reply, " cif_no=%d ",
									g_cif_id_code_rel.cif_no);
	if (ret)
	{
		sprintf(acErrMsg,"SELECT cif_basic_inf error code=[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D101");
		return 1;
	}

	ret = Cif_basic_inf_Fet_Upd (&g_cif_id_code_rel,g_pub_tx.reply);
	if (ret && ret!=100)
	{
		sprintf(acErrMsg,"fetch cif_basic_inf error code=[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D102");
		return 1;
	}
	else if (ret==100) 
	{
		sprintf(acErrMsg,"fetch clt_id_seq error code=[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D104");
		return 1;
	}

	g_cif_basic_inf.cif_seqn++;

	ret = Cif_basic_inf_Upd_Upd (g_cif_basic_inf,g_pub_tx.reply);
	if (ret)
	{
		sprintf(acErrMsg,"update cif_basic_inf error code=[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D105");
		return 1;
	}

	Cif_basic_inf_Clo_Upd( );

	*cif_seq = g_cif_basic_inf.cif_seqn;

	return 0;
}

/**********************************************************************
 * º¯ Êı Ãû£ºpub_base_get_prdtparm();
 * º¯Êı¹¦ÄÜ£º¸ù¾İ²úÆ·´úÂëÈ¡²úÆ·²ÎÊıº¯Êı
 *			 
 *
 * ×÷Õß/Ê±¼ä£ºrob/2003/12/31
 * 
 * ²ÎÊı£º
 *     ÊäÈë£º²úÆ·´úÂë		
 *          
 *     Êä³ö: 
 *			²úÆ·²ÎÊıÊı¾İ½á¹¹
 *         	ÏìÓ¦Âë
 *   ·µ»ØÖµ£º
 *           ·µ»Ø0³É¹¦£¬
 *           ·µ»Ø·Ç0Ê§°Ü£»
 * ĞŞ¸ÄÀúÊ·£º
 *          
********************************************************************/
int pub_base_get_prdtparm( char *prdt_no )
{
	char prdt_type[2];

	/*È¡²úÆ·ÀàĞÍ*/

	strncpy(prdt_type,prdt_no,1);

    sprintf(acErrMsg,"prdt_type[0]=[%c],[%s]",prdt_type[0],prdt_no);
    WRITEMSG

	switch ( prdt_type[0] )
	{
		case '1':										/*1-»îÆÚ*/
				/*È¡»îÆÚ²ÎÊı±í*/
				MEMSET_DEBUG( &g_dd_parm,0x00,sizeof(struct dd_parm_c) );

				ret = Dd_parm_Sel(g_pub_tx.reply, &g_dd_parm, 
									"prdt_no='%s' order by dc_code", prdt_no);
				if ( ret==100 )
				{
					sprintf(acErrMsg,
					"No match record in table dd_parm, error code=[%d]",ret);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "W024" );
					break;
				}
				else if ( ret )
				{
					sprintf(acErrMsg,
					"SELECT dd_parm error, error code=[%d]",ret);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "W025" );
					break;
				}
				break;
		case '2':										/*2-¶¨ÆÚ*/
				/*È¡¶¨ÆÚ²ÎÊı±í*/
			    sprintf(acErrMsg, "È¡¶¨ÆÚ²úÆ·²ÎÊı±í");
				WRITEMSG
				MEMSET_DEBUG( &g_td_parm,0x00,sizeof(struct td_parm_c) );

				ret = Td_parm_Sel(g_pub_tx.reply, &g_td_parm,"prdt_no='%s' order by dc_code",prdt_no);

				if( ret==100 )
				{
					sprintf(acErrMsg,
					"No match record in table td_parm, error code=[%d]",ret);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "W026" );
					break;
				}
				else if( ret )
				{
					sprintf(acErrMsg,
					"SELECT td_parm error, error code=[%d]",ret);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "W027" );
					break;
				}
				break;
		case '3':										/*3-´û¿î*/
				/*È¡´û¿î²ÎÊı±í*/
				MEMSET_DEBUG( &g_ln_parm,0x00,sizeof(struct ln_parm_c) );

				ret = Ln_parm_Sel(g_pub_tx.reply, &g_ln_parm,"prdt_no='%s'", \
								  prdt_no);
				if( ret==100 )
				{
					sprintf(acErrMsg,
					"No match record in table ln_parm, error code=[%d]",ret);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "W030" );
					break;
				}
				else if( ret )
				{
					sprintf(acErrMsg,"SELECT ln_parm error code=[%d]",ret);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "W031" );
					break;
				}
				break;
		case '4':										/*4-Í¸Ö§*/
				/*È¡Í¸Ö§²ÎÊı±í*/
				MEMSET_DEBUG( &g_od_parm,0x00,sizeof(struct od_parm_c) );

				ret=Od_parm_Sel(g_pub_tx.reply,&g_od_parm,"prdt_no='%s'", \
								prdt_no);
				if( ret==100 )
				{
					sprintf(acErrMsg,
					"No match record in table od_parm, error code=[%d]",ret);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "W032" );
					break;
				}
				else if(ret)
				{
					sprintf(acErrMsg,"SELECT od_parm error code=[%d]",ret);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "W033" );
					break;
				}
				break;
		case '9':										/*5-ÄÚ²¿²úÆ·*/
				/*È¡ÄÚ²¿²úÆ·²ÎÊı±í*/
				MEMSET_DEBUG( &g_in_parm,0x00,sizeof(struct in_parm_c) );
				/****ÎªÊ²Ã´ÓÃDd_parm_Sel  20121128
				ret = Dd_parm_Sel(g_pub_tx.reply, &g_in_parm,"prdt_no='%s' order by acc_hrt", \
				****/
				ret = In_parm_Sel(g_pub_tx.reply, &g_in_parm,"prdt_no='%s' order by acc_hrt", \
								  prdt_no);
				if( ret==100 )
				{
					sprintf(acErrMsg,
					"No match record in table in_parm, error code=[%d]",ret);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "W034" );
					break;
				}
				else if(ret)
				{
					sprintf(acErrMsg,
					"SELECT in_parm error, error code=[%d]",ret);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "W035" );
					break;
				}
				break;
		default:
			    sprintf(acErrMsg, "È¡²úÆ·²ÎÊı±í´íÎó");
				WRITEMSG
				break;
	}
	return 0;
}
/**********************************************************************
 * º¯ Êı Ãû£ºpub_base_disid_cltid();
 * º¯Êı¹¦ÄÜ£ºÏÔÊ¾ÕËºÅ×ª»»³ÉÕËºÅIDµÄº¯Êı
 *			 
 *
 * ×÷Õß/Ê±¼ä£ºrob/2003/12/31
 * 
 * ²ÎÊı£º
 *     ÊäÈë£ºÏÔÊ¾ÕËºÅ		
 *          
 *     Êä³ö: 
 *			ÕËºÅID
 *         	ÏìÓ¦Âë
 *   ·µ»ØÖµ£º
 *           ·µ»Ø0³É¹¦£¬
 *           ·µ»Ø·Ç0Ê§°Ü£»
 * ĞŞ¸ÄÀúÊ·£º
 *          
********************************************************************/
int pub_base_disid_cltid( ac_no , ac_id )
	char *ac_no;
	long *ac_id;
{
	/*¸ù¾İÏÔÊ¾ÕËºÅ×ª»»³ÉÕËºÅID*/
	MEMSET_DEBUG( &g_mdm_ac_rel, 0x00, sizeof(struct mdm_ac_rel_c) );

	ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &g_mdm_ac_rel, 
						"ac_no='%s'", ac_no);
	if ( ret==100 )
	{
		sprintf(acErrMsg,
		"No match record in table mdm_ac_rel, error code=[%d]",ret);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "W010" );
		return 1;
	}
	else if ( ret && ret!=100 )
	{
		sprintf(acErrMsg,"SELECT mdm_ac_rel error, error code=[%d]",ret);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "W011" );
		return 1;
	}

	*ac_id = g_mdm_ac_rel.ac_id;

	return 0;
}

/**********************************************************************
 * º¯ Êı Ãû£ºpub_base_oldac_newac();
 * º¯Êı¹¦ÄÜ£º¾ÉÕËºÅ×ª»»³ÉĞÂÕËºÅº¯Êıı
 *			 
 *
 * ×÷Õß/Ê±¼ä£ºrob/2003/12/31
 * 
 * ²ÎÊı£º
 *     ÊäÈë£º¾ÉÕËºÅ		
 *          
 *     Êä³ö: 
 *			ĞÂÕËºÅ
 *         	ÏìÓ¦Âë
 *   ·µ»ØÖµ£º
 *           ·µ»Ø0³É¹¦£¬
 *           ·µ»Ø·Ç0Ê§°Ü£»
 * ĞŞ¸ÄÀúÊ·£º
 *          
********************************************************************/
int pub_base_oldac_newac( old_ac,new_ac )
	char *old_ac;
	char *new_ac;
{
	/*¸ù¾İÏÔÊ¾ÕËºÅ×ª»»³ÉÕËºÅID*/
	MEMSET_DEBUG( &g_mo_merge_ac, 0x00, sizeof(struct mo_merge_ac_c) );

	ret = Mo_merge_ac_Sel(g_pub_tx.reply, &g_mo_merge_ac, 
							"old_ac_no='%s'", old_ac);
	if ( ret==100 )
	{
		sprintf(acErrMsg,
		"No match record in table mo_merge_ac, error code=[%d]",ret);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "W036" );
		return 1;
	}
	else if ( ret && ret!=100 )
	{
		sprintf(acErrMsg,
		"SELECT mo_merge_ac error, error code=[%d]",ret);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "W037" );
		return 1;
	}
	strcpy (new_ac , g_mo_merge_ac.ac_no);

	return 0;
}

/**********************************************************************
 * º¯ Êı Ãû£ºpub_base_wrt_unprt();
 * º¯Êı¹¦ÄÜ£ºµÇ¼ÇÎ´´òÓ¡Ã÷Ï¸ÕÊº¯Êı
 *			 
 *
 * ×÷Õß/Ê±¼ä£º
 * 
 * ²ÎÊı£º
 *     ÊäÈë£ºpub_tx 		¹«¹²¼ÇÕË½á¹¹		
 *          
 *     Êä³ö: 
 *         	ÏìÓ¦Âë
 *   ·µ»ØÖµ£º
 *           ·µ»Ø0³É¹¦£¬
 *           ·µ»Ø·Ç0Ê§°Ü£»
 * ĞŞ¸ÄÀúÊ·£º
 *          
********************************************************************/
int pub_base_wrt_unprt( f_pub_tx )
	struct pub_tx f_pub_tx;
{
	char prdt_type[2];

	g_mdm_unprt_hst.trace_no = f_pub_tx.trace_no;			/*Á÷Ë®ºÅ*/
	g_mdm_unprt_hst.trace_cnt = f_pub_tx.trace_cnt;			/*Á÷Ë®±Ê´Î*/
	strcpy (g_mdm_unprt_hst.tx_br_no,f_pub_tx.tx_br_no);	/*½»Ò×»ú¹¹ºÅ*/	
	g_mdm_unprt_hst.ac_id = f_pub_tx.ac_id;					/*ÕËºÅID*/
	g_mdm_unprt_hst.ac_seqn = f_pub_tx.ac_seqn;				/*ÕË»§ĞòºÅ*/
	strcpy (g_mdm_unprt_hst.add_ind , f_pub_tx.add_ind);	/*Ôö¼õ±êÖ¾*/	
	g_mdm_unprt_hst.tx_amt = f_pub_tx.tx_amt1;				/*½»Ò×½ğ¶î*/
	g_mdm_unprt_hst.tx_date = f_pub_tx.tx_date;				/*½»Ò×ÈÕÆÚ*/
	strcpy (g_mdm_unprt_hst.tel , f_pub_tx.tel);			/*²Ù×÷Ô±*/
	strcpy (g_mdm_unprt_hst.brf , f_pub_tx.tel);			/*ÕªÒª*/

	/*¸ù¾İÕËºÅidºÍÕË»§ĞòºÅID²éÕÒ²úÆ·ÀàĞÍ*/
	MEMSET_DEBUG( &g_prdt_ac_id, 0x00, sizeof(struct prdt_ac_id_c) );

	ret = Prdt_ac_id_Sel(g_pub_tx.reply, &g_prdt_ac_id, 
							"ac_id=%ld and ac_seqn=%d", 
							f_pub_tx.ac_id,f_pub_tx.ac_seqn);
	if ( ret==100 )
	{
		sprintf(acErrMsg,
		"No match record in table prdt_ac_id, error code=[%d]",ret);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "W012" );
		return 1;
	}
	else if ( ret!=0 )
	{
		sprintf(acErrMsg,
		"SELECT prdt_ac_id error, error code=[%d]",ret);
		WRITEMSG
		strcpy ( g_pub_tx.reply , "W013" );
		return 1;
	}

	/*È¡²úÆ·ÀàĞÍ*/
	strncpy ( prdt_type,g_prdt_ac_id.ac_id_type,1 );

	switch ( atoi(prdt_type) )
	{
		case 1:										/*1-»îÆÚ*/
				MEMSET_DEBUG( &g_dd_parm, 0x00, sizeof(struct dd_parm_c) );

				ret = Dd_parm_Sel(g_pub_tx.reply, &g_dd_parm, 
								"ac_id='%d' and ac_seqn='%d'", 
								f_pub_tx.ac_id,f_pub_tx.ac_seqn);
				if ( ret==100 )
				{
					sprintf(acErrMsg,
					"No match record in table dd_parm, error code=[%d]",ret);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "W024" );
					break;
				}
				else if ( ret&&ret!=100 )
				{
					sprintf(acErrMsg,
					"SELECT dd_parm error, error code=[%d]",ret);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "W025" );
					break;
				}
				g_mdm_unprt_hst.bal = g_dd_mst.bal;				/*Óà¶î*/
				g_mdm_unprt_hst.hst_cnt = g_dd_mst.hst_cnt;		/*Ã÷Ï¸±ÊÊı*/

				break;
		case 2:										/*2-¶¨ÆÚ*/
				MEMSET_DEBUG( &g_td_parm, 0x00, sizeof(struct td_parm_c) );

				ret = Td_parm_Sel(g_pub_tx.reply, &g_td_parm, 
								"ac_id=%ld and ac_seqn=%d", 
								f_pub_tx.ac_id,f_pub_tx.ac_seqn);
				if ( ret==100 )
				{
					sprintf(acErrMsg,
					"No match record in table td_parm, error code=[%d]",ret);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "W026" );
					break;
				}
				else if ( ret && ret!=100 )
				{
					sprintf(acErrMsg,
					"SELECT td_parm error, error code=[%d]",ret);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "W027" );
					break;
				}
				g_mdm_unprt_hst.bal = g_td_mst.bal;				/*Óà¶î*/
				g_mdm_unprt_hst.hst_cnt = g_td_mst.hst_cnt;		/*Ã÷Ï¸±ÊÊı*/

				break;
		case 3:										/*3-´û¿î*/
				MEMSET_DEBUG( &g_ln_parm, 0x00, sizeof(struct ln_parm_c) );

				ret = Ln_parm_Sel(g_pub_tx.reply, &g_ln_parm, 
								"ac_id=%ld and ac_seqn=%d", 
								f_pub_tx.ac_id,f_pub_tx.ac_seqn);
				if ( ret==100 )
				{
					sprintf(acErrMsg,
					"No match record in table ln_parm, error code=[%d]",ret);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "W030" );
					break;
				}
				else if ( ret&&ret!=100 )
				{
					sprintf(acErrMsg,
					"SELECT ln_parm error, error code=[%d]",ret);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "W031" );
					break;
				}
				g_mdm_unprt_hst.bal = g_ln_mst.bal;				/*Óà¶î*/
				g_mdm_unprt_hst.hst_cnt = g_ln_mst.hst_cnt;		/*Ã÷Ï¸±ÊÊı*/

				break;
		case 4:										/*4-Í¸Ö§*/
				MEMSET_DEBUG( &g_od_parm, 0x00, sizeof(struct od_parm_c) );

				ret = Od_parm_Sel(g_pub_tx.reply, &g_od_parm, 
				"ac_id=%ld and ac_seqn=%d", f_pub_tx.ac_id,f_pub_tx.ac_seqn);
				if ( ret==100 )
				{
					sprintf(acErrMsg,
					"No match record in table od_parm, error code=[%d]",ret);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "W032" );
					break;
				}
				else if ( ret && ret!=100 )
				{
					sprintf(acErrMsg,
					"SELECT od_parm error, error code=[%d]",ret);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "W033" );
					break;
				}
				g_mdm_unprt_hst.bal = g_od_mst.bal;				/*Óà¶î*/
				g_mdm_unprt_hst.hst_cnt = g_od_mst.hst_cnt;		/*Ã÷Ï¸±ÊÊı*/

				break;
		case 9:										/*9-ÄÚ²¿²úÆ·*/
				MEMSET_DEBUG( &g_in_parm, 0x00, sizeof(struct in_parm_c) );

				ret = In_parm_Sel(g_pub_tx.reply, &g_in_parm, 
								"ac_id=%ld and ac_seqn=%d", 
								f_pub_tx.ac_id,f_pub_tx.ac_seqn);
				if ( ret==100 )
				{
					sprintf(acErrMsg,
					"No match record in table in_parm, error code=[%d]",ret);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "W034" );
					break;
				}
				else if ( ret&&ret!=100 )
				{
					sprintf(acErrMsg,
					"SELECT in_parm error, error code=[%d]",ret);
					WRITEMSG
					strcpy ( g_pub_tx.reply , "W035" );
					break;
				}
				g_mdm_unprt_hst.bal = g_in_mst.bal;				/*Óà¶î*/
				g_mdm_unprt_hst.hst_cnt = g_in_mst.hst_cnt;		/*Ã÷Ï¸±ÊÊı*/

				break;
		default:
				break;
	}

	return 0;
}

/**********************************************************************
* º¯ÊıÃû£º     	pub_base_get_prdt_name
* º¯Êı¹¦ÄÜ£º    ÓÉÕË»§IDºÍÕË»§ĞòºÅ²éÑ¯²úÆ·Ãû³Æ
* ×÷Õß/Ê±¼ä£º   lance/2005Äê07ÔÂ04ÈÕ
* 
* ²ÎÊı£º
*     ÊäÈë£º ac_id       £ºÕË»§ID
*            ac_seqn     : ÕË»§ĞòºÅ
*           
*     Êä³ö£º prdt_name   : ²úÆ·Ãû³Æ
*
* ·µ »Ø Öµ: 0 ³É¹¦,1 Ê§°Ü
*           
* ĞŞ¸ÄÀúÊ·£º
*
********************************************************************/
int pub_base_get_prdt_name( long ac_id, int ac_seqn, char *prdt_name )
{    
	
	MEMSET_DEBUG( &g_prdt_ac_id , 0x00 ,sizeof( struct prdt_ac_id_c ));
	
	g_reply_int=Prdt_ac_id_Fet_Sel(&g_prdt_ac_id,g_pub_tx.reply);
	if (g_reply_int)
	{
		sprintf(acErrMsg,"FETCT ²úÆ·ÕÊºÅ¶ÔÕÕ±íÒì³£!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"D102");
		return 1;
	}
	switch(g_prdt_ac_id.ac_id_type[0])
	{
		case '1':
			g_reply_int=Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm, \
						"prdt_no='%s'", g_prdt_ac_id.prdt_no);
			if(g_reply_int==100)
			{
				sprintf(acErrMsg,"»îÆÚÃ÷Ï¸ÎÄ¼ş²»´æÔÚ¸Ã¼ÇÂ¼!!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"N062");
				break;
			}
			strcpy(prdt_name,g_dd_parm.title);			/*²úÆ·Ãû³Æ*/						
			return 0;
		case '2':
			g_reply_int=Td_parm_Sel(g_pub_tx.reply,&g_td_parm, \
						"prdt_no='%s'",g_prdt_ac_id.prdt_no);
			if(g_reply_int==100)
			{
				sprintf(acErrMsg,"¶¨ÆÚÃ÷Ï¸ÎÄ¼ş²»´æÔÚ¸Ã¼ÇÂ¼!!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"N062");
				break;
			}
			strcpy(prdt_name,g_td_parm.title);			/*²úÆ·Ãû³Æ*/							
			return 0;
		case '3':
			g_reply_int=Ln_parm_Sel(g_pub_tx.reply,&g_ln_parm, \
							"prdt_no='%s'",g_prdt_ac_id.prdt_no);
			if(g_reply_int==100)
			{
				sprintf(acErrMsg,"´û¿îÃ÷Ï¸ÎÄ¼ş²»´æÔÚ¸Ã¼ÇÂ¼!!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"N062");
				break;
			}
			strcpy(prdt_name,g_ln_parm.title);			/*²úÆ·Ãû³Æ*/							
			return 0;
		default:
			break;
	}/*switch½áÊø*/

	return 1;
}
/**********************************************************************
* º¯ÊıÃû£º     	pub_base_prdt_name
* º¯Êı¹¦ÄÜ£º    ÓÉ²úÆ·ºÅ²éÑ¯²úÆ·Ãû³Æ
* ×÷Õß/Ê±¼ä£º   lance/2005Äê07ÔÂ04ÈÕ
* 
* ²ÎÊı£º
*     ÊäÈë£º prdt_no     £º²úÆ·´úÂë
*           
*     Êä³ö£º prdt_name   : ²úÆ·Ãû³Æ
*
* ·µ »Ø Öµ: 0 ³É¹¦,1 Ê§°Ü
*           
* ĞŞ¸ÄÀúÊ·£º
*
********************************************************************/
int pub_base_prdt_name( char *prdt_no, char *prdt_name )
{    
	
	sprintf( acErrMsg,"prdt_no[%s],prdt_name[%s]",prdt_no,prdt_name);
	WRITEMSG
	if( strcmp(prdt_no,"ÁÙ")==0 )
	{
		strcpy( prdt_name , "ÁÙÊ±Ãû³Æ" );
		return 0;
	}
	switch(prdt_no[0])
	{
		case '1':
			g_reply_int=Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm, \
						"prdt_no='%s'", prdt_no);
			if(g_reply_int==100)
			{
				sprintf(acErrMsg,"»îÆÚ²ÎÊıÎÄ¼ş²»´æÔÚ¸Ã¼ÇÂ¼!!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"N062");
				break;
			}
			strcpy(prdt_name,g_dd_parm.title);			/*²úÆ·Ãû³Æ*/						
			return 0;
		case '2':
			g_reply_int=Td_parm_Sel(g_pub_tx.reply,&g_td_parm,"prdt_no='%s'", \
									prdt_no);
			if( g_reply_int==100 )
			{
				sprintf(acErrMsg,"¶¨ÆÚ²ÎÊıÎÄ¼ş²»´æÔÚ¸Ã¼ÇÂ¼![%s]",prdt_no);
				WRITEMSG
				strcpy(g_pub_tx.reply,"N062");
				break;
			}
			strcpy(prdt_name,g_td_parm.title);			/*²úÆ·Ãû³Æ*/							
			return 0;
		case '3':
			g_reply_int=Ln_parm_Sel(g_pub_tx.reply,&g_ln_parm, \
							"prdt_no='%s'",prdt_no);
			if(g_reply_int==100)
			{
				sprintf(acErrMsg,"´û¿î²ÎÊıÎÄ¼ş²»´æÔÚ¸Ã¼ÇÂ¼!!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"N062");
				break;
			}
			strcpy(prdt_name,g_ln_parm.title);			/*²úÆ·Ãû³Æ*/							
			return 0;
		case '4':
			g_reply_int=Od_parm_Sel(g_pub_tx.reply,&g_od_parm, \
							"prdt_no='%s'",prdt_no);
			if(g_reply_int==100)
			{
				sprintf(acErrMsg,"Í¸Ö§²ÎÊıÎÄ¼ş²»´æÔÚ¸Ã¼ÇÂ¼!!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"N062");
				break;
			}
			strcpy(prdt_name,g_od_parm.title);			/*²úÆ·Ãû³Æ*/							
			return 0;
		case '9':
			g_reply_int=In_parm_Sel(g_pub_tx.reply,&g_in_parm, \
							"prdt_no='%s'",prdt_no);
			if(g_reply_int==100)
			{
				sprintf(acErrMsg,"ÄÚ²¿ÕË²ÎÊıÎÄ¼ş²»´æÔÚ¸Ã¼ÇÂ¼!!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"N062");
				break;
			}
			strcpy(prdt_name,g_in_parm.prdt_mo);			/*²úÆ·Ãû³Æ*/							
			return 0;
		default:
			strcpy( g_pub_tx.reply,"D103" );
			break;
	}/*switch½áÊø*/

	return 1;
}
/**********************************************************************
* º¯ÊıÃû£º     	pub_base_item_inf
* º¯Êı¹¦ÄÜ£º    ¸ù¾İ¿ÆÄ¿¿ØÖÆ×ÖÈ¡¿ÆÄ¿ĞÅÏ¢
* ×÷Õß/Ê±¼ä£º   lance/2005Äê07ÔÂ05ÈÕ
* 
* ²ÎÊı£º
*     ÊäÈë£º 	acc_hrt   	   £º¿ÆÄ¿¿ØÖÆ×Ö
*           
*     Êä³ö£º 	struct	com_item   : ¿ÆÄ¿ĞÅÏ¢½á¹¹
*
* ·µ »Ø Öµ: 0 ³É¹¦,1 Ê§°Ü
*           
* ĞŞ¸ÄÀúÊ·£º
*
********************************************************************/
int pub_base_item_inf( char *acc_hrt, struct com_item_c	*com_item )
{    
	int ret=0;
	
	MEMSET_DEBUG( com_item , 0x00 , sizeof(struct com_item_c) );
	
	sprintf( acErrMsg,"acc_hrt[%s]",acc_hrt);
	WRITEMSG


	ret = Com_item_Sel( g_pub_tx.reply , com_item , " acc_hrt='%s' " , acc_hrt );
	if( ret )
	{
		sprintf(acErrMsg,"¶ÁÈ¡¿ÆÄ¿±íĞÅÏ¢Òì³£³ö´í!![%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply,"D103" );			
		return 1;
	}
	
	return 0;
}

/**********************************************************************
* º¯ÊıÃû£º     	pub_base_get_note_name
* º¯Êı¹¦ÄÜ£º    ¸ù¾İÆ¾Ö¤ÀàĞÍÈ¡Æ¾Ö¤Ãû³Æ
* ×÷Õß/Ê±¼ä£º   lance/2005Äê07ÔÂ06ÈÕ
* 
* ²ÎÊı£º
*     ÊäÈë£º 	note_type   	   £ºÆ¾Ö¤ÀàĞÍ
*           
*     Êä³ö£º 	note_name	   : Æ¾Ö¤Ãû³Æ
*
* ·µ »Ø Öµ: 0 ³É¹¦,1 Ê§°Ü
*           
* ĞŞ¸ÄÀúÊ·£º
*
********************************************************************/
int pub_base_get_note_name( char *note_type, char *note_name )
{    
	int ret=0;
	
	MEMSET_DEBUG( &note_parm , 0x00 , sizeof( struct note_parm_c ) );
	
	sprintf( acErrMsg,"note_type[%s]",note_type);
	WRITEMSG

	ret = Note_parm_Sel( g_pub_tx.reply , &note_parm , "note_type='%s'" , note_type );
	if( ret==100 )
	{
		sprintf(acErrMsg,"Æ¾Ö¤²ÎÊı±íÖĞÎŞ´ËÆ¾Ö¤!![%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply,"N085" );			
		return 1;
	}else if( ret ){
		sprintf(acErrMsg,"²éÑ¯Æ¾Ö¤²ÎÊı±íÒì³£³ö´í,ÇëÓëÖĞĞÄ»ú·¿ÁªÏµ!![%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply,"N086" );			
		return 1;
	}else if( ret==0 ){
		
		/* ¼ì²éÆ¾Ö¤ÊÇ·ñ´¦ÓÚÊ¹ÓÃ×´Ì¬ */
		if( note_parm.use_ind[0] == 'N' )
		{
			sprintf( acErrMsg,"Æ¾Ö¤´¦ÓÚÍ£ÓÃ×´Ì¬!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "N025" );
			return 1;
		}		
	}
	strcpy( note_name , note_parm.name );
	
	return 0;
}
