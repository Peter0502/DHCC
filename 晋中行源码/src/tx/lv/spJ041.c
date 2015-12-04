/***************************************************************************/
/* 文件名称: dh651000.pc                                                   */
/* 功能描述: 普通贷记业务录入                                              */
/* 创建日期: 2005.08.01                                                    */
/* 作者:     SSH                                                           */
/* 说明:                                                                   */
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
*   DESCRIPTION:       1.初始化工作区                                         *
*                      2.执行检查                                             *
*                      3.插入(更新)登记簿                                     *
*                      4.准备输出数据   �                                     *
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
        sprintf(acErrMsg,"登记交易流水出错!");
        strncpy(g_pub_tx.reply,"S023",4);
        WRITEMSG
        goto ErrExit;
    }
	/* 增加对业务委托凭证的处理  added by liuxuan 2007-3-31*/
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
		vtcp_log("开始凭证处理 ac_no[%s]type[%s][%s][%s][%d]\n",g_pub_tx.ac_no,g_pub_tx.note_type,g_pub_tx.beg_note_no,__FILE__,__LINE__);
		iRc=pub_com_NoteUse(g_pub_tx.ac_id,g_pub_tx.ac_seqn,g_pub_tx.note_type,g_pub_tx.beg_note_no,g_pub_tx.end_note_no,g_pub_tx.tel);        
		if(iRc)
		{
			sprintf(acErrMsg,"处理凭证出错!");
			WRITEMSG
			goto ErrExit;
		}
	}
	***/
	/* 增加对业务委托凭证的检查不处理  added by martin 2007-11-13*/
	if((memcmp(payin.F_pkgno,"001",3)==0)&&(memcmp(g_pub_tx.note_type,"005",3)==0))
	{
		vtcp_log("开始凭证处理 ac_no[%s]type[%s][%s][%s][%d]\n",g_pub_tx.ac_no,g_pub_tx.note_type,g_pub_tx.beg_note_no,__FILE__,__LINE__);
		iRc=this_com_NoteUse(g_pub_tx.ac_id,g_pub_tx.ac_seqn,g_pub_tx.note_type,g_pub_tx.beg_note_no,g_pub_tx.end_note_no,g_pub_tx.tel);        
		if(iRc)
		{
			sprintf(acErrMsg,"处理凭证出错!");
			WRITEMSG
			goto ErrExit;
		}
	}

	/*20110803 晋中商行，因信贷系统修改 */
  char m_draw_no[25];
  memset(m_draw_no,'\0',sizeof(m_draw_no));
	get_zd_data("1188",m_draw_no);
	zip_space(m_draw_no);
	if(strlen(m_draw_no)>0){
		ret = sql_execute( "UPDATE draw_loan_hst SET brf1='LV%s%8d%.8s' WHERE draw_no ='%s' ",g_pub_tx.tx_br_no,g_pub_tx.tx_date,payin.T_orderno ,m_draw_no);/*关联交易序号*/
		if( ret < 0 )
		{
        	sprintf( acErrMsg, "更改draw_loan_hst表出错!ret=[%d]",ret );
        	WRITEMSG
        	goto ErrExit;
		}
	}
	
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"失败![%s]",g_pub_tx.reply);
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
	/****填充payin*********/
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
	/****基本检查****/
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
	/****要素检查****/
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
	/****行号检查****/
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
	/****付款帐号检查****/
			
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
	/****正交易****/
	iOrderno=iLvGetOrderno();
	if(iOrderno<=0){
		vtcp_log("%s,%d,得到支付交易序号错误!\n",__FL__);
		WRITEMSG
		strncpy(g_pub_tx.reply,"P046",4);
		set_zd_data("0130",acErrMsg);
		return(-1);
	}
	vtcp_log("[%s][%d]\n", __FL__);
	apitoa(iOrderno,sizeof(payin.F_orderno),payin.F_orderno);
	/***根据输入内容生成一条新的纪录****/
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
	/***插入新记录****/
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
*   DESCRIPTION:	   为返回包赋值											  *
******************************************************************************/
ilv10_PreTos()
{
	int iRc;
	/****构造输出内容payout2****/
	iLvSetPayout1(&payin);
	return(0);
}


/**********************************************************************
 * 函  数 名：  this_com_NoteUse
 * 函数 功能:   凭证销号处理（交易过程中使用凭证）
 *	      包括销客户凭证和柜员凭证两部分，根据凭证类型来判断，如果可以出售
 *		给客户，则销客户凭证，如果不可出售给客户，销柜员凭证。如果不管
 *		理号码，则直接返回0，不进行凭证处理
 * 作者/时间：  Martin /2007年11月8日
 * 
 * 参  数：
 *     输入：      
 *        char     note_type[4]    凭证类型
 *        char     beg_no[17]      起始号码
 *        char     end_no[17]      终止号码
 *        char     tel[5]          柜员号
 *
 *     输出：  char g_pub_tx.reply[]     响应码
 *
 *   返回值：  0: 成功    -1: 不成功
 *           
 * 修改历史：  包括修改人、时间、修改内容，以时间倒序方式
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
	
	/* 取凭证参数表:根据凭证类型取凭证结构 */
	ret = Note_parm_Sel(g_pub_tx.reply, &sNoteParm, "note_type = '%s'", note_type );
	if( ret == 100 )
	{
	    sprintf(acErrMsg,"取凭证参数note_parm表错!note_type=[%s]",note_type);
	    WRITEMSG
	    strcpy( g_pub_tx.reply, "N001" );
	    return (-1);
	}else if ( ret )
	{
	    sprintf( acErrMsg, "执行Note_parm_Sel错误![%d]", ret );
	    WRITEMSG
	    return(-1);
	}	
	
	/* 判断是否管理，如果不管理直接返回正确 */
	if ( sNoteParm.ctl_no_ind[0] == 'N' )
	{
		return(0);	/* 不管理号码，直接返回正确0 */
	}

	
	sprintf( acErrMsg, "111111111111111sNoteParm.sell_ind=[%s]",sNoteParm.sell_ind);
	WRITEMSG
	sprintf( acErrMsg, "111111111111111svc_ind=[%d]",g_pub_tx.svc_ind);
	WRITEMSG

	/* 判断是否允许卖给客户 */
	if ( sNoteParm.sell_ind[0] == 'N' )	/* 不允许出售给客户 */
	{
		sprintf( acErrMsg, "凭证不能出售给客户" );
	   		WRITEMSG
	   		return(-1);			
		
	}else if ( sNoteParm.sell_ind[0] == 'Y' )
   	{
   		ret = this_com_CheqClsNo( ac_id, ac_seqn, note_type, beg_no, end_no );
		if ( ret )
		{
	   		sprintf( acErrMsg, "销客户凭证错" );
	   		WRITEMSG
	   		return(-1);				
		}
   	}else
   	{
   		sprintf( acErrMsg, "凭证种类出售标志错，sell_ind=[%s]",sNoteParm.sell_ind );
   		WRITEMSG
   		return(-1);
   	}
   	
   	return(0);
}

/**********************************************************************
 * 函  数 名：  this_com_CheqClsNo
 * 函数 功能:   客户凭证销号处理
 * 作者/时间：  martin/2007年11月8日
 * 
 * 参  数：
 *     输入：  	
 *       	long ac_id		帐户ID
 *		int  ac_seqn		帐户序号
 *              char *note_type         凭证类型
 *		char *cheq_beg_no	起始号码
 *		char *cheq_end_no	终止号码
 *
 *     输出：  char g_pub_tx.reply[] 	响应码
 *
 *   返回值：  0: 成功 	非0: 失败
 *           
 * 修改历史：  包括修改人、时间、修改内容，以时间倒序方式
 *
********************************************************************/
int this_com_CheqClsNo( ac_id, ac_seqn, note_type, cheq_beg_no, cheq_end_no )
	long	ac_id;
	int	ac_seqn;
	char	*note_type;
	char	*cheq_beg_no;
	char	*cheq_end_no;
{
	struct	note_cheq_mst_c	sCheqMst;	/* 记录客户凭证台帐 */
	struct	note_mst_c	sNoteMst;
	char	sts[51];	/* 支票状态 */
	long	tx_date;	/* 交易日期 */
	char	tw_br_no[6];/* 对方机构号 */
   	char   	tw_tel[6];	/* 对方柜员号 */
	int	ret;
	
	memset( &sCheqMst, 0x00, sizeof( struct note_cheq_mst_c ) );
	memset( &sNoteMst, 0x00, sizeof( struct note_mst_c ) );
	
	/* 检查起始号码、终止号码的连续性,冠子头是否相同 */
	ret = pub_com_ChkBoeNote( note_type, cheq_beg_no, cheq_end_no);
	if ( ret == -1 )
	{
		sprintf( acErrMsg, "检查连续性,冠子头失败!ret=[%d]",ret );
		WRITEMSG
		return(-1);		
	}
	/*******************************/
	
	/* 根据帐户ID检查支票是否为该帐号所有 */
	ret=pub_com_ChkAcCheq( ac_id, ac_seqn, note_type, cheq_beg_no, cheq_end_no, sts );
	if( ret == -1 )
	{
		sprintf( acErrMsg, "客户无此凭证或凭证状态不一致!ac_id=[%d],\
		         note_type=[%s],cheq_beg_no=[%s],cheq_end_no=[%s]",\
		         ac_id,note_type,cheq_beg_no, cheq_end_no );
		WRITEMSG
		return(-1);
	}

	if ( sts[0] == '0' )
	{
		sprintf( acErrMsg, "客户无此凭证!sts=[%s]", sts);	
		WRITEMSG
		strcpy( g_pub_tx.reply, "N080" );
		return(-1);
	}else if ( sts[0] == '2' )
	{
		sprintf( acErrMsg, "此凭证已挂失!sts=[%s]", sts);	
		WRITEMSG
		strcpy( g_pub_tx.reply, "N081" );
		return(-1);
	}else if ( sts[0] == '3' )
	{
		sprintf( acErrMsg, "此凭证已回收!sts=[%s]", sts);	
		WRITEMSG
		strcpy( g_pub_tx.reply, "N082" );
		return(-1);
	}else if ( sts[0] == '4' )
	{
		sprintf( acErrMsg, "此凭证已作废!sts=[%s]", sts);	
		WRITEMSG
		strcpy( g_pub_tx.reply, "N083" );
		return(-1);
	}else if ( sts[0] == '5' )
	{
		sprintf( acErrMsg, "此凭证已使用!sts=[%s]", sts);	
		WRITEMSG
		strcpy( g_pub_tx.reply, "N084" );
		return(-1);
	}
	
	/*****************************/
	
	/* 计算凭证数量 */
	ret = pub_com_CalNoteCnt( note_type,  cheq_beg_no, cheq_end_no );
	if ( ret == -1 )
	{
		sprintf( acErrMsg, "计算凭证数量错，ret = [%d]", ret );	
		WRITEMSG
		strcpy( g_pub_tx.reply, "N010" );
		return(-1);
	}
	
	return 0;
}
