/*************************************************
* �� �� ��:     pubf_ass_br_def
* ����������    1������ģ��pub_ass_get_br_def���ݻ�������ȡ����������
* ��    ��:
* ������ڣ�    2003��12��30��
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "ass_br_def_c.h"

/**********************************************************************
 * �� �� ����   pub_ass_get_br_def
 * �������ܣ�   ���ݻ�������ȡ���˻���������
 * ����/ʱ�䣺
 *
 * ������
 *     ����: ��������
 *     ���: ��������
 *     ����: <>0ʧ��
             =0�ɹ�
 *
 * �޸���ʷ��
 *
********************************************************************/
int pub_ass_get_br_def(char br_no[6],struct ass_br_def_c *ass_br_def_c)
{

 memset(ass_br_def_c,0x00,sizeof(struct ass_br_def_c));
 pub_base_strpack(br_no);

 g_reply_int=Ass_br_def_Sel( g_pub_tx.reply,&ass_br_def_c," br_no='%s' ",br_no);
 if ( g_reply_int==100 )
 {
 	sprintf(acErrMsg,"�û����ڿ���ϵͳ�в�����!br_no=[%s]",br_no);
 	WRITEMSG
 	strcpy ( g_pub_tx.reply , "K101" );
 	return -1;
 }
 else if ( g_reply_int )
 {
 	sprintf(acErrMsg,"����������쳣!br_no=[%s][%d]",br_no,g_reply_int);
 	WRITEMSG
 	strcpy ( g_pub_tx.reply , "K101" );
 	return -1;
 }

 return (0);
}
