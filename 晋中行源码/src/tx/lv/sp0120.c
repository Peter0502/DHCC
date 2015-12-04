/*************************************************
* �� �� ��:  spCHNO.c
* ���������� �����С���ƾ֤��
*
* ��    ��:  ligl
* ������ڣ� 2006��8��31��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
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
    	char    cNote_type[4];  /*ƾ֤����*/
    	char    cTel[5];        /*����Ա*/
    	char    cBeg_note_no[17];/*ƾ֤��ʼ��*/
    	
    	memset(&sNoteParm, 0x00, sizeof(struct note_parm_c));
    	memset(&sNoteMst, 0x00, sizeof(struct note_mst_c));
    	memset(&sNoteMinNo, 0x00, sizeof(struct note_min_no_c));
        memset(cNote_type  ,0,sizeof(cNote_type));
        memset(cTel,0,sizeof(cTel));
        memset(cBeg_note_no,0,sizeof(cBeg_note_no));
        
		vtcp_log("spCHNO��ʼ��************\n"__FILE__,__LINE__);
		pub_base_sysinit();
		/*ǰ̨ȡֵ*/
		get_zd_data("0070",cTel);
		get_zd_data("0890",cNote_type);
		vtcp_log("spCHNO��ʼ��[%s][%s][%d]",cNote_type,__FILE__,__LINE__);
		get_zd_data("0590",cBeg_note_no);  

		pub_base_strpack(cBeg_note_no);
		vtcp_log("spCHNO��ʼ��[%ld][%s][%d]",atol(cBeg_note_no),__FILE__,__LINE__);
		if(!strlen(cBeg_note_no))
		{
				sprintf(acErrMsg,"����ƾ֤����ʧ��!");
				set_zd_data(DC_GET_MSG,acErrMsg);
				goto ErrExit;
		}	    
		/* ����ƾ֤����ȡƾ֤�ṹ */
		ret=pub_com_ChkNoteUse(cNote_type);
		if( ret)
		{
				sprintf(acErrMsg,"ƾ֤����ͣ��״̬!note_type=[%s]",cNote_type);
				set_zd_data(DC_GET_MSG,acErrMsg);
				WRITEMSG
				goto ErrExit;
		}
		ret=pub_com_NoteUse(0,0,cNote_type,cBeg_note_no,cBeg_note_no,cTel);
		if( ret)
		{
				sprintf(acErrMsg,"����ƾ֤�������!note_num=[%s]",cBeg_note_no);
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

