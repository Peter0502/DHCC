/*************************************************
* �� �� ��:  spF074.c
* ���������� ����ҵ����ƾ֤����(ȡů����Ʊ)
*
* ��    ��:  wyw 
* ������ڣ� 2008��4��14��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "note_mst_c.h"
#include "note_mst_hst_c.h"
#include "note_parm_c.h"
 #define FP_VERSION "113080713003"
  #define FP_VERSION01 "113080813007"
int spF074()
{

    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"��ʼ�����ýṹ����!!");
        WRITEMSG
        goto ErrExit;
    }
    int ret=0;
    long note_num=0;
    char note_version[20+1];
    memset(note_version,0,sizeof(note_version));
    get_zd_long("0620",&note_num);   /*ƾ֤����*/
    sprintf(g_pub_tx.beg_note_no,"%016d",note_num);
    get_zd_data("0630",note_version);  /*��Ʊ�汾��*/
    get_zd_data("0890",g_pub_tx.note_type); /*ƾ֤����*/
    sprintf(acErrMsg, "ƾ֤����: [%s],ƾ֤����:[%s],��Ʊ�汾��[%s]", g_pub_tx.beg_note_no,g_pub_tx.note_type,note_version);
	WRITEMSG
 /*����汾������  liuyj 20081020*/
   /* if(memcmp(note_version,FP_VERSION,strlen(note_version)))
    {
	vtcp_log("%s,%d,��Ʊ�汾�Ŵ���!",__FILE__,__LINE__);
	strcpy(g_pub_tx.reply,"N061");
	goto ErrExit;
    }*/
    
  if(memcmp(note_version,FP_VERSION,strlen(note_version)) && memcmp(note_version,FP_VERSION01,strlen(note_version)) )
    {
	vtcp_log("%s,%d,��Ʊ�汾�Ŵ���!",__FILE__,__LINE__);
	strcpy(g_pub_tx.reply,"N061");
	goto ErrExit;
    }
    
 /*����汾������  liuyj end*/
    ret = pub_com_NoteUse(0,0,g_pub_tx.note_type,g_pub_tx.beg_note_no,g_pub_tx.beg_note_no,g_pub_tx.tel);
    if(ret)
    {
	vtcp_log("%s,%d,��ƾ֤����ʧ��!",__FILE__,__LINE__);
	if(memcmp(g_pub_tx.reply,"0000",4)==0)
	{
		strcpy(g_pub_tx.reply,"N061");
	}
	goto ErrExit;
    }
	vtcp_log("%s,%d,������Աƾ֤ʧ��!",__FILE__,__LINE__);
OkExit:
    strcpy( g_pub_tx.reply, "0000" );
    sprintf(acErrMsg,"success!!");
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;

ErrExit:
	sprintf(acErrMsg,"failed !! reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

