/***************************************************************************/
/* ÎÄ¼þÃû³Æ: dh651000.pc                                                   */
/* ¹¦ÄÜÃèÊö: ÆÕÍ¨´û¼ÇÒµÎñÂ¼Èë                                              */
/* ´´½¨ÈÕÆÚ: 2005.08.01                                                    */
/* ×÷Õß:     SSH                                                           */
/* ËµÃ÷:                                                                   */
/***************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include "lv_define.h"
#include "hv_define.h"
#include "note_parm_c.h"
#include "note_mst_c.h"
#include "note_cheq_mst_c.h"

static PAY_IN_AREA payin;
/******************************************************************************
*                                                                             *
*   FUNCTIONS:         dh65100_Work                                           *
*                                                                             *
*   DESCRIPTION:       1.³õÊ¼»¯¹¤×÷Çø                                         *
*                      2.Ö´ÐÐ¼ì²é                                             *
*                      3.²åÈë(¸üÐÂ)µÇ¼Ç²¾                                     *
*                      4.×¼±¸Êä³öÊý¾Ý   ¾                                     *
******************************************************************************/
int spJ041()
{
	int ret=0;
	int iRc=0;
	long ori_ac_id;
vtcp_log("AP MSG: Begin of dh65100_Work\n");
	/*********************/
	/* Initial variables */
	/*********************/
	ori_ac_id=g_pub_tx.ac_id;
	iRc=ilv10_Initial();
vtcp_log("[%s][%d]\n", __FL__);
	if(iRc)
	{
		sprintf(acErrMsg,"%s,%d init error!",__FL__);
		WRITEMSG
		goto ErrExit;
	}
	/**********************************/
	/* verify data for basic checking */
	/**********************************/
	iRc=ilv10_Verify();
	vtcp_log("[%s][%d]\n", __FL__);
	/********************************/
	if(iRc)
	{
		sprintf(acErrMsg,"%s,%d verify error!",__FL__);
		WRITEMSG
		goto ErrExit;
	}
	/********************************************************/
	/* Process -- 1.Initial PBMR record to opening stauts   */
	/*            2.Insert PBMR record/Delete(reversal)     */
	/********************************************************/
	iRc=ilv10_Process();
	if(iRc)
	{
		sprintf(acErrMsg,"%s,%d process error!",__FL__);
		WRITEMSG
		goto ErrExit;
	}
	/*******************************/
	/* prepare XDTL data for batch */
	/*******************************/
	/******************************/
	/*       Prepare TOS          */
	/******************************/
	iRc=ilv10_PreTos();
	if(iRc)
	{
		sprintf(acErrMsg,"%s,%d set fd123 error!",__FL__);
		strncpy(g_pub_tx.reply,"P079",4);
		WRITEMSG
		goto ErrExit;
	}
	
    iRc=pub_ins_trace_log();
    if(iRc)
    {
        sprintf(acErrMsg,"µÇ¼Ç½»Ò×Á÷Ë®³ö´í!");
        strncpy(g_pub_tx.reply,"S023",4);
        WRITEMSG
        goto ErrExit;
    }
	/* Ôö¼Ó¶ÔÒµÎñÎ¯ÍÐÆ¾Ö¤µÄ´¦Àí  added by liuxuan 2007-3-31*/
	get_zd_data("1021",g_pub_tx.ac_no);
	get_zd_data("1023",g_pub_tx.note_type);
	get_zd_data("1024",g_pub_tx.beg_note_no);
	get_zd_data("1024",g_pub_tx.end_note_no);
	get_zd_int("1022",&g_pub_tx.ac_seqn);
	g_pub_tx.ac_id=ori_ac_id;
	vtcp_log("sssssssspayin.F_pkgno[%s],g_pub_tx.note_type=[%s]\n",payin.F_pkgno,g_pub_tx.note_type);
	/* del by martin 20071108 
	if((memcmp(payin.F_pkgno,"001",3)==0)&&(memcmp(g_pub_tx.note_type,"005",3)==0))
	{
		vtcp_log("¿ªÊ¼Æ¾Ö¤´¦Àí ac_no[%s]type[%s][%s][%s][%d]\n",g_pub_tx.ac_no,g_pub_tx.note_type,g_pub_tx.beg_note_no,__FILE__,__LINE__);
		iRc=pub_com_NoteUse(g_pub_tx.ac_id,g_pub_tx.ac_seqn,g_pub_tx.note_type,g_pub_tx.beg_note_no,g_pub_tx.end_note_no,g_pub_tx.tel);        
		if(iRc)
		{
			sprintf(acErrMsg,"´¦ÀíÆ¾Ö¤³ö´í!");
			WRITEMSG
			goto ErrExit;
		}
	}
	***/
	/* Ôö¼Ó¶ÔÒµÎñÎ¯ÍÐÆ¾Ö¤µÄ¼ì²é²»´¦Àí  added by martin 2007-11-13*/
	if((memcmp(payin.F_pkgno,"001",3)==0)&&(memcmp(g_pub_tx.note_type,"005",3)==0))
	{
		vtcp_log("¿ªÊ¼Æ¾Ö¤´¦Àí ac_no[%s]type[%s][%s][%s][%d]\n",g_pub_tx.ac_no,g_pub_tx.note_type,g_pub_tx.beg_note_no,__FILE__,__LINE__);
		iRc=this_com_NoteUse(g_pub_tx.ac_id,g_pub_tx.ac_seqn,g_pub_tx.note_type,g_pub_tx.beg_note_no,g_pub_tx.end_note_no,g_pub_tx.tel);        
		if(iRc)
		{
			sprintf(acErrMsg,"´¦ÀíÆ¾Ö¤³ö´í!");
			WRITEMSG
			goto ErrExit;
		}
	}

	/*20110803 ½úÖÐÉÌÐÐ£¬ÒòÐÅ´ûÏµÍ³ÐÞ¸Ä */
  char m_draw_no[25];
  memset(m_draw_no,'\0',sizeof(m_draw_no));
	get_zd_data("1188",m_draw_no);
	zip_space(m_draw_no);
	if(strlen(m_draw_no)>0){
		ret = sql_execute( "UPDATE draw_loan_hst SET brf1='LV%s%8d%.8s' WHERE draw_no ='%s' ",g_pub_tx.tx_br_no,g_pub_tx.tx_date,payin.T_orderno ,m_draw_no);/*¹ØÁª½»Ò×ÐòºÅ*/
		if( ret < 0 )
		{
        	sprintf( acErrMsg, "¸ü¸Ädraw_loan_hst±í³ö´í!ret=[%d]",ret );
        	WRITEMSG
        	goto ErrExit;
		}
	}
	
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"³É¹¦![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Ê§°Ü![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
/******************************************************************************
*                                                                             *
*   FUNCTION:          a10_Initial()                                         *
*                                                                             *
*   DESCRIPTION:       1.Initial all variables.                               *
******************************************************************************/
int ilv10_Initial()
{
	memset(&payin,'\0',sizeof(payin));
	pub_base_sysinit();
	/**************/
	/* Initial WD */
	/**************/
	/****Ìî³äpayin*********/
	if(iLvchkstat())
	{
		if(strlen(acErrMsg)!=0)
			set_zd_data("0130",acErrMsg);
		strncpy(g_pub_tx.reply,"P046",4);
		WRITEMSG
		return(-1);
	}
	if(iLvTisToPayin(&payin)){
		if(strlen(acErrMsg)!=0)
			set_zd_data("0130",acErrMsg);
		strncpy(g_pub_tx.reply,"P046",4);
		WRITEMSG
		return(-1);
	}
vtcp_log("[%s][%d]\n", __FL__);
	return(0);	

}
/******************************************************************************
*                                                                             *
*   FUNCTION:                            lv10_Verify()                        *
*                                                                             *
*   DESCRIPTION:                         1.Initial all variables.             *
******************************************************************************/
int ilv10_Verify()
{
	int iRc;
	/****»ù±¾¼ì²é****/
vtcp_log("%s,%d,txnum==[%.5s]\n",__FL__,payin.T_txnum);
vtcp_log("[%s][%d]T_ptype==[%s]\n", __FL__,payin.T_ptype);
	iRc=iLvPayinCheck0(&payin);
	if(iRc){
		vtcp_log("[%s][%d] err in check0 err=[%d]\n", __FL__,iRc);
		if(strlen(acErrMsg)!=0)
			set_zd_data("0130",acErrMsg);
		strncpy(g_pub_tx.reply,"P046",4);
		WRITEMSG
		return(-1);
	}
vtcp_log("[%s][%d]T_ptype==[%s]\n", __FL__,payin.T_ptype);
	/****ÒªËØ¼ì²é****/
vtcp_log("%s,%d,txnum==[%.5s]\n",__FL__,payin.T_txnum);
	iRc=iLvPayinCheck4(&payin);
	if(iRc){
		vtcp_log("[%s][%d] err in check0 err=[%d]\n", __FL__,iRc);
		if(strlen(acErrMsg)!=0)
			set_zd_data("0130",acErrMsg);
		strncpy(g_pub_tx.reply,"P046",4);
		WRITEMSG
		return(-1);
	}
vtcp_log("[%s][%d]T_ptype==[%s]\n", __FL__,payin.T_ptype);
vtcp_log("[%s][%d]\n", __FL__);
	/****ÐÐºÅ¼ì²é****/
vtcp_log("%s,%d,txnum==[%.5s]\n",__FL__,payin.T_txnum);
	iRc=iLvPayinCheck1(&payin);
	if(iRc){
		if(strlen(acErrMsg)!=0)
			set_zd_data("0130",acErrMsg);
		strncpy(g_pub_tx.reply,"P046",4);
		WRITEMSG
		vtcp_log("[%s][%d] err in check0 err=[%d]\n", __FL__,iRc);
		return(-1);
	}
vtcp_log("[%s][%d]\n", __FL__);
vtcp_log("[%s][%d]T_ptype==[%s]\n", __FL__,payin.T_ptype);
vtcp_log("%s,%d,txnum==[%.5s]\n",__FL__,payin.T_txnum);
	/****¸¶¿îÕÊºÅ¼ì²é****/
			
	return(0);	
}
/******************************************************************************
*   FUNCTION:          lv10_Process()                                         *
*   DESCRIPTION:  1.Initial PBMR record to opening stauts                     *
*                 2.Insert PBMR record/Delete(reversal)                       *
******************************************************************************/
int ilv10_Process()
{
	int iOrderno;
	int iRc;
	vtcp_log("%s,%d,txnum==[%.5s]\n",__FL__,payin.T_txnum);
	/****Õý½»Ò×****/
	iOrderno=iLvGetOrderno();
	if(iOrderno<=0){
		vtcp_log("%s,%d,µÃµ½Ö§¸¶½»Ò×ÐòºÅ´íÎó!\n",__FL__);
		WRITEMSG
		strncpy(g_pub_tx.reply,"P046",4);
		set_zd_data("0130",acErrMsg);
		return(-1);
	}
	vtcp_log("[%s][%d]\n", __FL__);
	apitoa(iOrderno,sizeof(payin.F_orderno),payin.F_orderno);
	/***¸ù¾ÝÊäÈëÄÚÈÝÉú³ÉÒ»ÌõÐÂµÄ¼ÍÂ¼****/
	vtcp_log("%s,%d,txnum==[%.5s]\n",__FL__,payin.T_txnum);
	iRc=iLvNewRecFromPayin(&payin);
	vtcp_log("%s,%d,txnum==[%.5s]\n",__FL__,payin.T_txnum);
	if(iRc){
		WRITEMSG
		strncpy(g_pub_tx.reply,"P046",4);
		set_zd_data("0130",acErrMsg);
		return(-1);
	}
	vtcp_log("[%s][%d]\n", __FL__);
	/***²åÈëÐÂ¼ÇÂ¼****/
	vtcp_log("%s,%d,txnum==[%.5s]\n",__FL__,payin.T_txnum);
	iRc=iLvUpdRecFromPayin(&payin,1);
	if(iRc){
		WRITEMSG
		strncpy(g_pub_tx.reply,"P046",4);
		set_zd_data("0130",acErrMsg);
		return(-1);
	}
	vtcp_log("%s,%d,txnum==[%.5s]\n",__FL__,payin.T_txnum);
	return(0);

}

/******************************************************************************
*  																			  *
*	FUNCTION:		   lv10_PreTos()										  *
*   DESCRIPTION:	   Îª·µ»Ø°ü¸³Öµ											  *
******************************************************************************/
ilv10_PreTos()
{
	int iRc;
	/****¹¹ÔìÊä³öÄÚÈÝpayout2****/
	iLvSetPayout1(&payin);
	return(0);
}


/**********************************************************************
 * º¯  Êý Ãû£º  this_com_NoteUse
 * º¯Êý ¹¦ÄÜ:   Æ¾Ö¤ÏúºÅ´¦Àí£¨½»Ò×¹ý³ÌÖÐÊ¹ÓÃÆ¾Ö¤£©
 *	      °üÀ¨Ïú¿Í»§Æ¾Ö¤ºÍ¹ñÔ±Æ¾Ö¤Á½²¿·Ö£¬¸ù¾ÝÆ¾Ö¤ÀàÐÍÀ´ÅÐ¶Ï£¬Èç¹û¿ÉÒÔ³öÊÛ
 *		¸ø¿Í»§£¬ÔòÏú¿Í»§Æ¾Ö¤£¬Èç¹û²»¿É³öÊÛ¸ø¿Í»§£¬Ïú¹ñÔ±Æ¾Ö¤¡£Èç¹û²»¹Ü
 *		ÀíºÅÂë£¬ÔòÖ±½Ó·µ»Ø0£¬²»½øÐÐÆ¾Ö¤´¦Àí
 * ×÷Õß/Ê±¼ä£º  Martin /2007Äê11ÔÂ8ÈÕ
 * 
 * ²Î  Êý£º
 *     ÊäÈë£º      
 *        char     note_type[4]    Æ¾Ö¤ÀàÐÍ
 *        char     beg_no[17]      ÆðÊ¼ºÅÂë
 *        char     end_no[17]      ÖÕÖ¹ºÅÂë
 *        char     tel[5]          ¹ñÔ±ºÅ
 *
 *     Êä³ö£º  char g_pub_tx.reply[]     ÏìÓ¦Âë
 *
 *   ·µ»ØÖµ£º  0: ³É¹¦    -1: ²»³É¹¦
 *           
 * ÐÞ¸ÄÀúÊ·£º  °üÀ¨ÐÞ¸ÄÈË¡¢Ê±¼ä¡¢ÐÞ¸ÄÄÚÈÝ£¬ÒÔÊ±¼äµ¹Ðò·½Ê½
 *
********************************************************************/
int this_com_NoteUse(
	long	ac_id,
	int	ac_seqn,
    	char    *note_type,
    	char    *beg_no,
    	char    *end_no,
    	char    *tel)
{
	struct note_parm_c sNoteParm;
	
	int	ret;
	
	memset( &sNoteParm, 0x00, sizeof( struct note_parm_c ) );
	
	/* È¡Æ¾Ö¤²ÎÊý±í:¸ù¾ÝÆ¾Ö¤ÀàÐÍÈ¡Æ¾Ö¤½á¹¹ */
	ret = Note_parm_Sel(g_pub_tx.reply, &sNoteParm, "note_type = '%s'", note_type );
	if( ret == 100 )
	{
	    sprintf(acErrMsg,"È¡Æ¾Ö¤²ÎÊýnote_parm±í´í!note_type=[%s]",note_type);
	    WRITEMSG
	    strcpy( g_pub_tx.reply, "N001" );
	    return (-1);
	}else if ( ret )
	{
	    sprintf( acErrMsg, "Ö´ÐÐNote_parm_Sel´íÎó![%d]", ret );
	    WRITEMSG
	    return(-1);
	}	
	
	/* ÅÐ¶ÏÊÇ·ñ¹ÜÀí£¬Èç¹û²»¹ÜÀíÖ±½Ó·µ»ØÕýÈ· */
	if ( sNoteParm.ctl_no_ind[0] == 'N' )
	{
		return(0);	/* ²»¹ÜÀíºÅÂë£¬Ö±½Ó·µ»ØÕýÈ·0 */
	}

	
	sprintf( acErrMsg, "111111111111111sNoteParm.sell_ind=[%s]",sNoteParm.sell_ind);
	WRITEMSG
	sprintf( acErrMsg, "111111111111111svc_ind=[%d]",g_pub_tx.svc_ind);
	WRITEMSG

	/* ÅÐ¶ÏÊÇ·ñÔÊÐíÂô¸ø¿Í»§ */
	if ( sNoteParm.sell_ind[0] == 'N' )	/* ²»ÔÊÐí³öÊÛ¸ø¿Í»§ */
	{
		sprintf( acErrMsg, "Æ¾Ö¤²»ÄÜ³öÊÛ¸ø¿Í»§" );
	   		WRITEMSG
	   		return(-1);			
		
	}else if ( sNoteParm.sell_ind[0] == 'Y' )
   	{
   		ret = this_com_CheqClsNo( ac_id, ac_seqn, note_type, beg_no, end_no );
		if ( ret )
		{
	   		sprintf( acErrMsg, "Ïú¿Í»§Æ¾Ö¤´í" );
	   		WRITEMSG
	   		return(-1);				
		}
   	}else
   	{
   		sprintf( acErrMsg, "Æ¾Ö¤ÖÖÀà³öÊÛ±êÖ¾´í£¬sell_ind=[%s]",sNoteParm.sell_ind );
   		WRITEMSG
   		return(-1);
   	}
   	
   	return(0);
}

/**********************************************************************
 * º¯  Êý Ãû£º  this_com_CheqClsNo
 * º¯Êý ¹¦ÄÜ:   ¿Í»§Æ¾Ö¤ÏúºÅ´¦Àí
 * ×÷Õß/Ê±¼ä£º  martin/2007Äê11ÔÂ8ÈÕ
 * 
 * ²Î  Êý£º
 *     ÊäÈë£º  	
 *       	long ac_id		ÕÊ»§ID
 *		int  ac_seqn		ÕÊ»§ÐòºÅ
 *              char *note_type         Æ¾Ö¤ÀàÐÍ
 *		char *cheq_beg_no	ÆðÊ¼ºÅÂë
 *		char *cheq_end_no	ÖÕÖ¹ºÅÂë
 *
 *     Êä³ö£º  char g_pub_tx.reply[] 	ÏìÓ¦Âë
 *
 *   ·µ»ØÖµ£º  0: ³É¹¦ 	·Ç0: Ê§°Ü
 *           
 * ÐÞ¸ÄÀúÊ·£º  °üÀ¨ÐÞ¸ÄÈË¡¢Ê±¼ä¡¢ÐÞ¸ÄÄÚÈÝ£¬ÒÔÊ±¼äµ¹Ðò·½Ê½
 *
********************************************************************/
int this_com_CheqClsNo( ac_id, ac_seqn, note_type, cheq_beg_no, cheq_end_no )
	long	ac_id;
	int	ac_seqn;
	char	*note_type;
	char	*cheq_beg_no;
	char	*cheq_end_no;
{
	struct	note_cheq_mst_c	sCheqMst;	/* ¼ÇÂ¼¿Í»§Æ¾Ö¤Ì¨ÕÊ */
	struct	note_mst_c	sNoteMst;
	char	sts[51];	/* Ö§Æ±×´Ì¬ */
	long	tx_date;	/* ½»Ò×ÈÕÆÚ */
	char	tw_br_no[6];/* ¶Ô·½»ú¹¹ºÅ */
   	char   	tw_tel[6];	/* ¶Ô·½¹ñÔ±ºÅ */
	int	ret;
	
	memset( &sCheqMst, 0x00, sizeof( struct note_cheq_mst_c ) );
	memset( &sNoteMst, 0x00, sizeof( struct note_mst_c ) );
	
	/* ¼ì²éÆðÊ¼ºÅÂë¡¢ÖÕÖ¹ºÅÂëµÄÁ¬ÐøÐÔ,¹Ú×ÓÍ·ÊÇ·ñÏàÍ¬ */
	ret = pub_com_ChkBoeNote( note_type, cheq_beg_no, cheq_end_no);
	if ( ret == -1 )
	{
		sprintf( acErrMsg, "¼ì²éÁ¬ÐøÐÔ,¹Ú×ÓÍ·Ê§°Ü!ret=[%d]",ret );
		WRITEMSG
		return(-1);		
	}
	/*******************************/
	
	/* ¸ù¾ÝÕÊ»§ID¼ì²éÖ§Æ±ÊÇ·ñÎª¸ÃÕÊºÅËùÓÐ */
	ret=pub_com_ChkAcCheq( ac_id, ac_seqn, note_type, cheq_beg_no, cheq_end_no, sts );
	if( ret == -1 )
	{
		sprintf( acErrMsg, "¿Í»§ÎÞ´ËÆ¾Ö¤»òÆ¾Ö¤×´Ì¬²»Ò»ÖÂ!ac_id=[%d],\
		         note_type=[%s],cheq_beg_no=[%s],cheq_end_no=[%s]",\
		         ac_id,note_type,cheq_beg_no, cheq_end_no );
		WRITEMSG
		return(-1);
	}

	if ( sts[0] == '0' )
	{
		sprintf( acErrMsg, "¿Í»§ÎÞ´ËÆ¾Ö¤!sts=[%s]", sts);	
		WRITEMSG
		strcpy( g_pub_tx.reply, "N080" );
		return(-1);
	}else if ( sts[0] == '2' )
	{
		sprintf( acErrMsg, "´ËÆ¾Ö¤ÒÑ¹ÒÊ§!sts=[%s]", sts);	
		WRITEMSG
		strcpy( g_pub_tx.reply, "N081" );
		return(-1);
	}else if ( sts[0] == '3' )
	{
		sprintf( acErrMsg, "´ËÆ¾Ö¤ÒÑ»ØÊÕ!sts=[%s]", sts);	
		WRITEMSG
		strcpy( g_pub_tx.reply, "N082" );
		return(-1);
	}else if ( sts[0] == '4' )
	{
		sprintf( acErrMsg, "´ËÆ¾Ö¤ÒÑ×÷·Ï!sts=[%s]", sts);	
		WRITEMSG
		strcpy( g_pub_tx.reply, "N083" );
		return(-1);
	}else if ( sts[0] == '5' )
	{
		sprintf( acErrMsg, "´ËÆ¾Ö¤ÒÑÊ¹ÓÃ!sts=[%s]", sts);	
		WRITEMSG
		strcpy( g_pub_tx.reply, "N084" );
		return(-1);
	}
	
	/*****************************/
	
	/* ¼ÆËãÆ¾Ö¤ÊýÁ¿ */
	ret = pub_com_CalNoteCnt( note_type,  cheq_beg_no, cheq_end_no );
	if ( ret == -1 )
	{
		sprintf( acErrMsg, "¼ÆËãÆ¾Ö¤ÊýÁ¿´í£¬ret = [%d]", ret );	
		WRITEMSG
		strcpy( g_pub_tx.reply, "N010" );
		return(-1);
	}
	
	return 0;
}
