#include "find_debug.h" 
/*************************************************
* �ļ���:    pubf_com_note.c
* ���������� ƾ֤��������
*           pub_com_ChkNote      ƾ֤��鴦��
*
* ��   ��:  jack
* ������ڣ� 2004��12��16��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/
#define MYSQLERR if( ret ) { \
			sprintf(acErrMsg,"�����ݿ��[%d]",ret);\
			WRITEMSG \
			goto ErrExit; \
			}
                 
#include <stdio.h>  
#define EXTERN
#include "public.h"
#include "note_tel_auth_c.h"
/**��������pbrno��Աptel
   ƾ֤����ptype
   ��Ϊact 10X �����ࣨ101���102�·�103�սɣ� 20X�ڲ�������
		   30X ʹ����
	mode ��鱾��0 �Է�1
****************/
pub_com_NoteAuthChk( pbrno,ptel,act,mode )
 char *pbrno;
 char *ptel;
 int act;
 int mode;
{
	struct note_tel_auth_c note_tel_auth_c;
	int ret=0;
	int chksut;
	chksut=1;

	if( act==101 )  /**���**/
	{
		/**����ƾ֤����Ա**/
		vtcp_log("HERE!!! brno=[%s],tel=[%s]",pbrno,ptel);
		ret=Note_tel_auth_Sel( g_pub_tx.reply , &note_tel_auth_c ,
				"br_no='%s' and tel='%s' and sts='0' and substr(lvl,1,1) in('1') ",
				pbrno,ptel );
		if( ret==0 ) chksut=0;
		else if( ret==100 ) {strcpy( g_pub_tx.reply,"N088" );
				vtcp_log("HERE1!!!");
				}
		else { MYSQLERR }
	}
	else if( act==102 )  /**�·�**/ /**�ս�**/
	{
		if( mode==0 ) /*����*/
		{
			ret=Note_tel_auth_Sel( g_pub_tx.reply , &note_tel_auth_c ,
				"br_no='%s' and tel='%s' and sts='0' and substr(lvl,1,1) in('1','2','3') ",
				pbrno,ptel );
			if( ret==0 ) chksut=0;
			else if( ret==100 ) strcpy( g_pub_tx.reply,"N088" );
			else { MYSQLERR }
TRACE
		}
		else if( mode==10 ) /*�������жԷ�*/
		{
			ret=Note_tel_auth_Sel( g_pub_tx.reply , &note_tel_auth_c ,
			"br_no='%s' and tel='%s' and sts='0' and substr(lvl,1,1) in('3')",
				pbrno,ptel );
			if( ret==0 ) chksut=0;
			else if( ret==100 ) strcpy( g_pub_tx.reply,"N091" );
			else { MYSQLERR }
TRACE
		}
		else /*�Է�*/
		{
			ret=Note_tel_auth_Sel( g_pub_tx.reply , &note_tel_auth_c ,
			"br_no='%s' and tel='%s' and sts='0' and substr(lvl,1,1) in('2','3','4')",
				pbrno,ptel );
			if( ret==0 ) chksut=0;
			else if( ret==100 ) strcpy( g_pub_tx.reply,"N091" );
			else { MYSQLERR }
TRACE
		}
	}
	else if( act==103 )   /**����**/
	{
			ret=Note_tel_auth_Sel( g_pub_tx.reply , &note_tel_auth_c ,
			"br_no='%s' and tel='%s' and sts='0' and substr(lvl,1,1) in('2','3','4')",
				pbrno,ptel );
			if( ret==0 ) chksut=0;
			else if( ret==100 ) strcpy( g_pub_tx.reply,"N088" );
			else { MYSQLERR }
	}
	else if( act==104 )   /**�Ͻ�**/
	{
		if( mode==10 ) /*��֧���Ͻ�ƾ֤*/
		{
			ret=Note_tel_auth_Sel( g_pub_tx.reply , &note_tel_auth_c ,
			"br_no='%s' and tel='%s' and sts='0' and substr(lvl,1,1) in('3')",
				pbrno,ptel );
			if( ret==0 ) chksut=0;
			else if( ret==100 ) strcpy( g_pub_tx.reply,"N088" );
			else { MYSQLERR }
		}
		else if( mode==0 ) /*����*/
		{
			ret=Note_tel_auth_Sel( g_pub_tx.reply , &note_tel_auth_c ,
			"br_no='%s' and tel='%s' and sts='0' and substr(lvl,1,1) in('2','4')",
				pbrno,ptel );
			if( ret==0 ) chksut=0;
			else if( ret==100 ) strcpy( g_pub_tx.reply,"N088" );
			else { MYSQLERR }
		}
		else /*�Է�*/
		{
			ret=Note_tel_auth_Sel( g_pub_tx.reply , &note_tel_auth_c ,
			"br_no='%s' and tel='%s' and sts='0' and substr(lvl,1,1) in('1','2')",
				pbrno,ptel );
			if( ret==0 ) chksut=0;
			else if( ret==100 ) strcpy( g_pub_tx.reply,"N091" );
			else { MYSQLERR }
		}
	}
	else if( act==301 ) /**����**/
	{
		if( mode==0 )
		{
			/*����ԱȨ��**/
			ret=Note_tel_auth_Sel( g_pub_tx.reply , &note_tel_auth_c ,
				"br_no='%s' and tel='%s' and sts='0' and substr(lvl,1,1) in('1','2') ",
				pbrno,ptel );
			vtcp_log("����ԱȨ�����[%s][%d]����ֵ[%d]",__FILE__,__LINE__,ret);
			if( ret==0 ) strcpy( g_pub_tx.reply,"N090" );
			else if( ret==100 ) chksut=0;
			else { MYSQLERR }
		}
		else
		{
			/*����ԱȨ��**/
			ret=Note_tel_auth_Sel( g_pub_tx.reply , &note_tel_auth_c ,
				"br_no='%s' and tel='%s' and sts='0' and substr(lvl,1,1) in('1','2') ",
				pbrno,ptel );
			vtcp_log("����ԱȨ�����[%s][%d]����ֵ[%d]",__FILE__,__LINE__,ret);
			if( ret==0 ) strcpy( g_pub_tx.reply,"N093" );
			else if( ret==100 ) chksut=0;
			else { MYSQLERR }
		}
	}
	else if( act==401 ) /**��ͨ��Աʹ�õĽ���**/
	{
		/*����ԱȨ��**/
		ret=Note_tel_auth_Sel( g_pub_tx.reply , &note_tel_auth_c ,
		"br_no='%s' and tel='%s' and sts='0' and substr(lvl,1,1) in('1','2','3','4') ",
				pbrno,ptel );
		vtcp_log("����ԱȨ�����[%s][%d]����ֵ[%d]",__FILE__,__LINE__,ret);
		if( ret==0 ) strcpy( g_pub_tx.reply,"N090" );
		else if( ret==100 ) chksut=0;
		else { MYSQLERR }
	}
	else
	{
		chksut=0;
	}

OkExit:
	return chksut;
ErrExit:
	return -1;
}
