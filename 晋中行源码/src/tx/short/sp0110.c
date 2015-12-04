/*************************************************
* 文 件 名:  spCHNO.c
* 功能描述： 检查最小大额凭证号
*
* 作    者:  ligl
* 完成日期： 2006年8月31日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "mdm_attr_c.h"
#include "note_mst_c.h"
#include "note_mst_hst_c.h"
#include "note_min_no_c.h"
#include "note_parm_c.h"
int sp0110()
{
	int	ret;
	struct    note_mst_c    sNoteMst;
   	struct    note_parm_c   sNoteParm;
   	struct    note_min_no_c sNoteMinNo;
   	char    cNote_type[4];  /*凭证类型*/
   	char    cTel[5];        /*操作员*/
   	char    cBeg_note_no[17];/*凭证起始号*/
   	char    cMinNo[17];     /* 保存最小号码 */
   	char    cInHead[17];    /*待检查的凭证号的冠子头*/
   	char    cMinHead[17];   /*凭证最小号码的冠子头*/
   	long    lBegNo, lMinNo; /* 开始号码和最小号码(带冠子头) */
    	
   	memset(&sNoteParm, 0x00, sizeof(struct note_parm_c));
   	memset(&sNoteMst, 0x00, sizeof(struct note_mst_c));
   	memset(&sNoteMinNo, 0x00, sizeof(struct note_min_no_c));
	memset(cNote_type  ,0,sizeof(cNote_type));
	memset(cTel,0,sizeof(cTel));
	memset(cBeg_note_no,0,sizeof(cBeg_note_no));
	memset(cMinNo,0,sizeof(cMinNo));
	memset(cInHead,0,sizeof(cInHead));
	memset(cMinHead,0,sizeof(cMinHead));

	pub_base_sysinit();
	/*前台取值*/
	get_zd_data("0070",cTel);
	get_zd_data("0890",cNote_type);
	vtcp_log("spCHNO开始！[%s][%s][%d]",cNote_type,__FILE__,__LINE__);
	get_zd_data("0590",cBeg_note_no);  

	pub_base_strpack(cBeg_note_no);
	vtcp_log("spCHNO开始！[%ld][%s][%d]",atol(cBeg_note_no),__FILE__,__LINE__);
	if(!strlen(cBeg_note_no))
	{
		goto OkExit;
	}	    
	/* 根据凭证类型取凭证结构 */
	ret = Note_parm_Sel(g_pub_tx.reply, &sNoteParm, "note_type = '%s'", cNote_type );
	if( ret== 100 )
	{
		sprintf(acErrMsg,"无此凭证类型!note_type=[%s]",cNote_type);
		WRITEMSG
		goto ErrExit;
	}
	else if (ret)
	{
		sprintf( acErrMsg, "执行Note_parm_Sel错误!ret=[%d]",ret);
		WRITEMSG
		goto ErrExit;
	}     
	/* 取凭证号码冠子头 */
	strncpy( cInHead, cBeg_note_no, sNoteParm.head_len );
	cInHead[sNoteParm.head_len] = '\0';    
	/* 取凭证最小号 */
	ret = Note_min_no_Sel(g_pub_tx.reply, &sNoteMinNo, "tel = '%s'and note_type = '%s'", cTel, cNote_type);
	if(ret ==100)/* 如果凭证最小号表中不存在记录，则从note_mst表中查找最小号 */
	{
		sprintf( acErrMsg, "note_min_no表中无此凭证最小号!tel=[%s],note_type=[%s]",cTel,cNote_type );
		WRITEMSG
		ret = sql_min_string("note_mst","beg_note_no", cMinNo, sizeof(cMinNo)-1,"tel='%s' and note_type='%s' and beg_note_no[1,%ld]='%s' and sts[1]='0'",cTel, cNote_type, sNoteParm.head_len, cInHead);
		if(ret)
		{
			sprintf(acErrMsg,"执行sql_min_string错误！") ;
			WRITEMSG
			goto ErrExit;
		}
		if(!strlen(cMinNo))
		{
			strcpy( g_pub_tx.reply, "N075" ) ;
			WRITEMSG
			goto ErrExit;
		}
		/* 检查是否是最小号（从note_mst种查找）,如果不是最小号，则返回最小号 */
		if(strcmp(cBeg_note_no, cMinNo)>0)
		{
			sprintf( acErrMsg, "凭证非最小号码,凭证号[%s] 最小号[%s]",cBeg_note_no, cMinNo );
			WRITEMSG
			sprintf(cBeg_note_no, "%d", lMinNo) ; 
			sprintf(acErrMsg,"最小号是%d",lMinNo);
			set_zd_data(DC_GET_MSG,acErrMsg);
			WRITEMSG
		}
	}    
	else if(ret)
	{
		sprintf( acErrMsg, "执行Note_min_no_Sel错误!ret=[%d]",ret);
		WRITEMSG
		goto ErrExit;
	}
	/* 取凭证最小号码的冠子头 */
	strncpy( cMinHead, sNoteMinNo.note_min_no, sNoteParm.head_len );

	cMinHead[sNoteParm.head_len] = '\0';

	/* 去掉冠子头后凭证号码和最小号码 */
	lBegNo = atol( cBeg_note_no + sNoteParm.head_len );
	lMinNo = atol( sNoteMinNo.note_min_no + sNoteParm.head_len );

	vtcp_log("[%s][%d]cMinHead=[%s],lBegNo=[%lf],lMinNO=[%lf]\n",__FILE__,__LINE__,cMinHead,lBegNo,lMinNo);
	/* 检查note_mst中是否存在以cInHead开头的凭证 */
	vtcp_log( "tel = '%s' and sts[1] = '0' and note_type = '%s' and beg_note_no[1,%d] = '%s'", cTel, cNote_type, sNoteParm.head_len, cInHead);
	ret = sql_count( "note_mst", "tel = '%s' and sts[1] = '0'and note_type = '%s' and beg_note_no[1,%d] = '%s'",cTel, cNote_type, sNoteParm.head_len, cInHead);
	if( ret < 0 )
	{
		sprintf(acErrMsg,"执行sql_count错，ret=[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply, "D109" );
		goto ErrExit;
	}
	if( ret == 0 )
	{
		sprintf(acErrMsg,"柜员无此凭证tel=[%s],note_type[%s]", cTel,cNote_type);
		WRITEMSG
		strcpy( g_pub_tx.reply, "N007" );
		goto ErrExit;
	}      
		/* 比较冠子头是否相同 */
	vtcp_log("[%s][%d]cInHead==[%s],cMinHead==[%s]\n",__FILE__,__LINE__,cInHead,cMinHead);
	if( !strcmp(cInHead, cMinHead) )  /* 如果冠子头相同 */
	{
		if( lBegNo != lMinNo )  /* 比较凭证号码 */
		{
			sprintf(acErrMsg,"凭证非最小号码,凭证号[%d]最小号[%d][%s]",lBegNo,lMinNo,cNote_type) ;
			WRITEMSG
			strcpy( g_pub_tx.reply, "N014" );
			/* 返回前台最小号 */
			sprintf(cBeg_note_no, "%d", lMinNo) ; 
			sprintf(acErrMsg,"最小号是%d",lMinNo);
			set_zd_data(DC_GET_MSG,acErrMsg);
			WRITEMSG
		}
	}
   	else  /* 如果冠子头不同 */
   	{
		ret = sql_min_string("note_mst","beg_note_no", cMinNo, sizeof(cMinNo)-1,"tel = '%s' and note_type = '%s' and beg_note_no[1,%ld] = '%s'and sts[1] = '0'", cTel, cNote_type, sNoteParm.head_len, cInHead);
		if( ret )
		{
			sprintf(acErrMsg,"执行sql_min_string错误！") ;
			WRITEMSG
			goto ErrExit;

		}
		/* 检查是否是最小号（从note_mst种查找） */
		if( strcmp( cBeg_note_no, cMinNo ) != 0 )
		{
			strcpy( g_pub_tx.reply, "N014" );
			sprintf( acErrMsg, "凭证非最小号码,凭证号[%s] 最小号[%s]",cBeg_note_no, cMinNo );
			WRITEMSG            
			strcpy( cBeg_note_no, cMinNo );
			/* 返回前台最小号 */
			sprintf( acErrMsg,"最小号是%s", cMinNo );
			set_zd_data( DC_GET_MSG,acErrMsg );
			WRITEMSG
		}
	}
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	if(memcmp((char*)getenv("PZTEST"),"PZTEST",6)==0)
	{
		strcpy( g_pub_tx.reply, "0000" );
		set_zd_data(DC_REPLY,g_pub_tx.reply);
		return 0;		
	}
	else
	{
		set_zd_data(DC_REPLY,g_pub_tx.reply);
		return 1;		
	}
}
