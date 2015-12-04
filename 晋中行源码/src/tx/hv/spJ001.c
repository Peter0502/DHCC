/***************************************************************
* 文 件 名:     spJ001.c
* 功能描述：	大额支付业务录入
*
* 作    者:   LiuHuafeng
* 完成日期：  2006-8-9 9:28
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "hvpack.h"
#include "find_debug.h"
#include "note_parm_c.h"
#include "note_mst_c.h"
#include "note_cheq_mst_c.h"

spJ001()
{
	int ret = 0;
	HV_PAYIN_AREA hv_payin;
	char cAc_no[25];
	char cKinbr[BRNO_LEN+1];
	char node0[10]; 
	
	/*** 初始化全局变量 ***/
	pub_base_sysinit();
	memset(&hv_payin, 0 , sizeof(hv_payin));
	memset(cAc_no, 0 , sizeof(cAc_no));
	memset(cKinbr, 0 ,sizeof(cKinbr));
	memset(node0, 0x0, sizeof(node0));
	
	get_zd_data(DC_TX_BR_NO,cKinbr);
	get_zd_data("0270",node0); 
	/***得到大额支付业务8583数据***/
	ret=iHvTistoPayin(&hv_payin);
	if( ret )
	{
		strcpy( g_pub_tx.reply, "HV01");
		WRITEMSG
		goto ErrExit;
	}
	/* 准备写入登记簿的数据 */
	ret=iHvFrontToTable(&hv_payin);
	if( ret )
	{
		strcpy( g_pub_tx.reply, "HV01");
		WRITEMSG
		goto ErrExit;
	}
	/**网银交易在这里直接将状态致为复核状态 hzh 20130610**/
	if(memcmp( g_pub_tx.tx_code,"6389",sizeof( g_pub_tx.tx_code))==0 &&
		memcmp(g_pub_tx.sub_tx_code,"J001",sizeof(g_pub_tx.sub_tx_code))==0)
		{
		memcpy(hv_payin.F_opcd, "30", 2);
		hv_payin.T_hv_sts[0] = SEND_ACCT_CHECK;	
		vtcp_log("%s %d 网银交易在录入时就把状态致成了复核状态g_pub_tx.tx_code=[%s],g_pub_tx.sub_tx_code=[%s],hv_payin.F_opcd=[%s],hv_payin.T_hv_sts=[%s]",__FILE__,__LINE__,g_pub_tx.tx_code,g_pub_tx.sub_tx_code,hv_payin.F_opcd,hv_payin.T_hv_sts);
		}
	/**网银交易在这里直接将状态致为复核状态 hzh 20130610**/
	/* 检查非法字符 */
	ret=iHvPayin_characterCheck(&hv_payin,acErrMsg);
	if(ret)
	{
		vtcp_log("%s,%d errmsg %s",__FILE__,__LINE__,acErrMsg);
		set_zd_data("0130",acErrMsg);
		MEMCPY_DEBUG(g_pub_tx.reply,"T042",4);
		WRITEMSG
		goto ErrExit;
	}
	ret=iHv_checkstat(hv_payin.F_cmtno,hv_payin.F_yw_type);
	if(ret)
	{
		set_zd_data("0130",acErrMsg);
		MEMCPY_DEBUG(g_pub_tx.reply,"T042",4);
		WRITEMSG
		goto ErrExit;
	}
	/* 写入登记簿 */
	ret=iHvUpdZfRecFromPayin(&hv_payin,1);
	if( ret )
	{
		strcpy( g_pub_tx.reply, "HV01");
		WRITEMSG
		goto ErrExit;
	}
	iHvResetPayin(&hv_payin);
	iHvReset8583(&hv_payin);
	/* 写入业务流水 */
	vtcp_log("%s %d ac_no[%s]",__FILE__,__LINE__,g_pub_tx.ac_no);
	vtcp_log("%s %d !!!svc_ind[%d]",__FILE__,__LINE__,g_pub_tx.svc_ind);
/*
	g_pub_tx.svc_ind=0;
*/	
	ret=pub_ins_trace_log();
	if(ret)
	{
		strcpy( g_pub_tx.reply, "HV01");
		sprintf(acErrMsg,"登记交易流水出错!");
		WRITEMSG
		goto ErrExit;
	}
        /* 增加对业务委托凭证的处理  added by liuxuan 2007-3-31*/
        get_zd_data("1023",g_pub_tx.note_type);
        get_zd_data("1024",g_pub_tx.beg_note_no);
        get_zd_data("1024",g_pub_tx.end_note_no);
        get_zd_int("1022",&g_pub_tx.ac_seqn);
  	vtcp_log("大额业务处理机构g_pub_tx.tx_br_no==[%s]HV_CZ_QSBRNO==[%s][%s][%d]\n",g_pub_tx.tx_br_no,HV_CZ_QSBRNO,__FILE__,__LINE__);
  /* del by martin 20071108 
  if( memcmp( g_pub_tx.tx_br_no,HV_CZ_QSBRNO,sizeof(g_pub_tx.tx_br_no)-1 ) )
  {
	  if (!((strcmp(hv_payin.F_cmtno,"100"))&&(strcmp(g_pub_tx.note_type,"005"))))
	  {
          	vtcp_log("开始凭证处理 type[%s][%s][%s][%d]\n",g_pub_tx.note_type,g_pub_tx.beg_note_no,__FILE__,__LINE__);
	  	ret=pub_com_NoteUse(g_pub_tx.ac_id,g_pub_tx.ac_seqn,g_pub_tx.note_type,g_pub_tx.beg_note_no,g_pub_tx.end_note_no,g_pub_tx.tel);
	  	if(ret)
	  	{
	  		sprintf(acErrMsg,"处理凭证出错!");
	  		WRITEMSG
	  		goto ErrExit;
	  	}
	  }
  }*/
  /* 增加对业务委托凭证的检查不处理  added by martin 2007-11-8*/
  if( memcmp( g_pub_tx.tx_br_no,HV_CZ_QSBRNO,sizeof(g_pub_tx.tx_br_no)-1 ) )
  {
	  if ( (memcmp(hv_payin.F_cmtno,"100",3)==0) && (memcmp(g_pub_tx.note_type,"005",3)==0) )
	  {
          	vtcp_log("开始凭证处理 type[%s][%s][%s][%d]\n",g_pub_tx.note_type,g_pub_tx.beg_note_no,__FILE__,__LINE__);
	  	ret=this_com_NoteUse(g_pub_tx.ac_id,g_pub_tx.ac_seqn,g_pub_tx.note_type,g_pub_tx.beg_note_no,g_pub_tx.end_note_no,g_pub_tx.tel);
	  	if(ret)
	  	{
	  		sprintf(acErrMsg,"处理凭证出错!");
	  		WRITEMSG
	  		goto ErrExit;
	  	}
	  }
  }
  /*20110803 晋中商行，因信贷系统修改 */
  char m_draw_no[25];
  memset(m_draw_no,'\0',sizeof(m_draw_no));
  sprintf(acErrMsg,"我是5801交易吧。");
	WRITEMSG
	get_zd_data("1188",m_draw_no);
	zip_space(m_draw_no);
	vtcp_log("[%s][%d],draw_no=[%s]",__FILE__,__LINE__,m_draw_no);
	if(strlen(m_draw_no)>0){
		ret = sql_execute( "UPDATE draw_loan_hst SET brf1='HV%s%8d%.8s' WHERE draw_no ='%s' ",g_pub_tx.tx_br_no,g_pub_tx.tx_date,hv_payin.F_orderno ,m_draw_no);/*关联交易序号*/
		if( ret < 0 )
		{
        	sprintf( acErrMsg, "更改draw_loan_hst表出错!ret=[%d]",ret );
        	WRITEMSG
        	goto ErrExit;
		}
	}
	/**网银大额交易在这里得到本机构大额清算帐号 20130605 hzh**/
	 if(memcmp(node0, "nbanklvhv", 9) == 0) 
		{
				ret=pub_base_GetParm_Str(cKinbr,PARM_HVQS,cAc_no);
			if(ret)
			{
				sprintf(acErrMsg,"得到本机构大额清算帐号错误!\n");
				WRITEMSG
				vtcp_log("%s,%d 得到本机构大额清算帐号错误\n",__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"P073");
				goto ErrExit;
			}
				set_zd_data("0310",cAc_no);
				vtcp_log("[%s][%d],取得本机构大额清算帐号=[%s]",__FILE__,__LINE__,cAc_no);
		}
	/**网银大额交易在这里得到本机构大额清算帐号 20130605 hzh**/
	/*** 输出变量 ***/
	sprintf(acErrMsg,"ac_no[%s]",g_pub_tx.ac_no);
	WRITEMSG
	set_zd_data("112A",g_pub_tx.ac_no);

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	if(memcmp(g_pub_tx.reply,"0000",4)==0)
	{
		MEMCPY_DEBUG(g_pub_tx.reply,"T063",4);
	}
	sprintf(acErrMsg,"失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
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
	
	MEMSET_DEBUG( &sNoteParm, 0x00, sizeof( struct note_parm_c ) );
	
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
	
	MEMSET_DEBUG( &sCheqMst, 0x00, sizeof( struct note_cheq_mst_c ) );
	MEMSET_DEBUG( &sNoteMst, 0x00, sizeof( struct note_mst_c ) );
	
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
