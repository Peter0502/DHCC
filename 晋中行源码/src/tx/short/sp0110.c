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
int sp0110()
{
	int	ret;
	struct    note_mst_c    sNoteMst;
   	struct    note_parm_c   sNoteParm;
   	struct    note_min_no_c sNoteMinNo;
   	char    cNote_type[4];  /*ƾ֤����*/
   	char    cTel[5];        /*����Ա*/
   	char    cBeg_note_no[17];/*ƾ֤��ʼ��*/
   	char    cMinNo[17];     /* ������С���� */
   	char    cInHead[17];    /*������ƾ֤�ŵĹ���ͷ*/
   	char    cMinHead[17];   /*ƾ֤��С����Ĺ���ͷ*/
   	long    lBegNo, lMinNo; /* ��ʼ�������С����(������ͷ) */
    	
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
	/*ǰ̨ȡֵ*/
	get_zd_data("0070",cTel);
	get_zd_data("0890",cNote_type);
	vtcp_log("spCHNO��ʼ��[%s][%s][%d]",cNote_type,__FILE__,__LINE__);
	get_zd_data("0590",cBeg_note_no);  

	pub_base_strpack(cBeg_note_no);
	vtcp_log("spCHNO��ʼ��[%ld][%s][%d]",atol(cBeg_note_no),__FILE__,__LINE__);
	if(!strlen(cBeg_note_no))
	{
		goto OkExit;
	}	    
	/* ����ƾ֤����ȡƾ֤�ṹ */
	ret = Note_parm_Sel(g_pub_tx.reply, &sNoteParm, "note_type = '%s'", cNote_type );
	if( ret== 100 )
	{
		sprintf(acErrMsg,"�޴�ƾ֤����!note_type=[%s]",cNote_type);
		WRITEMSG
		goto ErrExit;
	}
	else if (ret)
	{
		sprintf( acErrMsg, "ִ��Note_parm_Sel����!ret=[%d]",ret);
		WRITEMSG
		goto ErrExit;
	}     
	/* ȡƾ֤�������ͷ */
	strncpy( cInHead, cBeg_note_no, sNoteParm.head_len );
	cInHead[sNoteParm.head_len] = '\0';    
	/* ȡƾ֤��С�� */
	ret = Note_min_no_Sel(g_pub_tx.reply, &sNoteMinNo, "tel = '%s'and note_type = '%s'", cTel, cNote_type);
	if(ret ==100)/* ���ƾ֤��С�ű��в����ڼ�¼�����note_mst���в�����С�� */
	{
		sprintf( acErrMsg, "note_min_no�����޴�ƾ֤��С��!tel=[%s],note_type=[%s]",cTel,cNote_type );
		WRITEMSG
		ret = sql_min_string("note_mst","beg_note_no", cMinNo, sizeof(cMinNo)-1,"tel='%s' and note_type='%s' and beg_note_no[1,%ld]='%s' and sts[1]='0'",cTel, cNote_type, sNoteParm.head_len, cInHead);
		if(ret)
		{
			sprintf(acErrMsg,"ִ��sql_min_string����") ;
			WRITEMSG
			goto ErrExit;
		}
		if(!strlen(cMinNo))
		{
			strcpy( g_pub_tx.reply, "N075" ) ;
			WRITEMSG
			goto ErrExit;
		}
		/* ����Ƿ�����С�ţ���note_mst�ֲ��ң�,���������С�ţ��򷵻���С�� */
		if(strcmp(cBeg_note_no, cMinNo)>0)
		{
			sprintf( acErrMsg, "ƾ֤����С����,ƾ֤��[%s] ��С��[%s]",cBeg_note_no, cMinNo );
			WRITEMSG
			sprintf(cBeg_note_no, "%d", lMinNo) ; 
			sprintf(acErrMsg,"��С����%d",lMinNo);
			set_zd_data(DC_GET_MSG,acErrMsg);
			WRITEMSG
		}
	}    
	else if(ret)
	{
		sprintf( acErrMsg, "ִ��Note_min_no_Sel����!ret=[%d]",ret);
		WRITEMSG
		goto ErrExit;
	}
	/* ȡƾ֤��С����Ĺ���ͷ */
	strncpy( cMinHead, sNoteMinNo.note_min_no, sNoteParm.head_len );

	cMinHead[sNoteParm.head_len] = '\0';

	/* ȥ������ͷ��ƾ֤�������С���� */
	lBegNo = atol( cBeg_note_no + sNoteParm.head_len );
	lMinNo = atol( sNoteMinNo.note_min_no + sNoteParm.head_len );

	vtcp_log("[%s][%d]cMinHead=[%s],lBegNo=[%lf],lMinNO=[%lf]\n",__FILE__,__LINE__,cMinHead,lBegNo,lMinNo);
	/* ���note_mst���Ƿ������cInHead��ͷ��ƾ֤ */
	vtcp_log( "tel = '%s' and sts[1] = '0' and note_type = '%s' and beg_note_no[1,%d] = '%s'", cTel, cNote_type, sNoteParm.head_len, cInHead);
	ret = sql_count( "note_mst", "tel = '%s' and sts[1] = '0'and note_type = '%s' and beg_note_no[1,%d] = '%s'",cTel, cNote_type, sNoteParm.head_len, cInHead);
	if( ret < 0 )
	{
		sprintf(acErrMsg,"ִ��sql_count��ret=[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply, "D109" );
		goto ErrExit;
	}
	if( ret == 0 )
	{
		sprintf(acErrMsg,"��Ա�޴�ƾ֤tel=[%s],note_type[%s]", cTel,cNote_type);
		WRITEMSG
		strcpy( g_pub_tx.reply, "N007" );
		goto ErrExit;
	}      
		/* �ȽϹ���ͷ�Ƿ���ͬ */
	vtcp_log("[%s][%d]cInHead==[%s],cMinHead==[%s]\n",__FILE__,__LINE__,cInHead,cMinHead);
	if( !strcmp(cInHead, cMinHead) )  /* �������ͷ��ͬ */
	{
		if( lBegNo != lMinNo )  /* �Ƚ�ƾ֤���� */
		{
			sprintf(acErrMsg,"ƾ֤����С����,ƾ֤��[%d]��С��[%d][%s]",lBegNo,lMinNo,cNote_type) ;
			WRITEMSG
			strcpy( g_pub_tx.reply, "N014" );
			/* ����ǰ̨��С�� */
			sprintf(cBeg_note_no, "%d", lMinNo) ; 
			sprintf(acErrMsg,"��С����%d",lMinNo);
			set_zd_data(DC_GET_MSG,acErrMsg);
			WRITEMSG
		}
	}
   	else  /* �������ͷ��ͬ */
   	{
		ret = sql_min_string("note_mst","beg_note_no", cMinNo, sizeof(cMinNo)-1,"tel = '%s' and note_type = '%s' and beg_note_no[1,%ld] = '%s'and sts[1] = '0'", cTel, cNote_type, sNoteParm.head_len, cInHead);
		if( ret )
		{
			sprintf(acErrMsg,"ִ��sql_min_string����") ;
			WRITEMSG
			goto ErrExit;

		}
		/* ����Ƿ�����С�ţ���note_mst�ֲ��ң� */
		if( strcmp( cBeg_note_no, cMinNo ) != 0 )
		{
			strcpy( g_pub_tx.reply, "N014" );
			sprintf( acErrMsg, "ƾ֤����С����,ƾ֤��[%s] ��С��[%s]",cBeg_note_no, cMinNo );
			WRITEMSG            
			strcpy( cBeg_note_no, cMinNo );
			/* ����ǰ̨��С�� */
			sprintf( acErrMsg,"��С����%s", cMinNo );
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
