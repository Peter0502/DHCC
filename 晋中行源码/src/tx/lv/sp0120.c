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
int sp0120()
{
		int	ret;
		struct    note_mst_c    sNoteMst;
    	struct    note_parm_c   sNoteParm;
    	struct    note_min_no_c sNoteMinNo;
    	char    cNote_type[4];  /*凭证类型*/
    	char    cTel[5];        /*操作员*/
    	char    cBeg_note_no[17];/*凭证起始号*/
    	
    	memset(&sNoteParm, 0x00, sizeof(struct note_parm_c));
    	memset(&sNoteMst, 0x00, sizeof(struct note_mst_c));
    	memset(&sNoteMinNo, 0x00, sizeof(struct note_min_no_c));
        memset(cNote_type  ,0,sizeof(cNote_type));
        memset(cTel,0,sizeof(cTel));
        memset(cBeg_note_no,0,sizeof(cBeg_note_no));
        
		vtcp_log("spCHNO开始！************\n"__FILE__,__LINE__);
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
				sprintf(acErrMsg,"消除凭证号码失败!");
				set_zd_data(DC_GET_MSG,acErrMsg);
				goto ErrExit;
		}	    
		/* 根据凭证类型取凭证结构 */
		ret=pub_com_ChkNoteUse(cNote_type);
		if( ret)
		{
				sprintf(acErrMsg,"凭证处于停用状态!note_type=[%s]",cNote_type);
				set_zd_data(DC_GET_MSG,acErrMsg);
				WRITEMSG
				goto ErrExit;
		}
		ret=pub_com_NoteUse(0,0,cNote_type,cBeg_note_no,cBeg_note_no,cTel);
		if( ret)
		{
				sprintf(acErrMsg,"消除凭证号码错误!note_num=[%s]",cBeg_note_no);
				set_zd_data(DC_GET_MSG,acErrMsg);
				WRITEMSG
				goto ErrExit;
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
		strcpy( g_pub_tx.reply, "ZZ01" );
		set_zd_data(DC_REPLY,g_pub_tx.reply);
		return 0;		
}

