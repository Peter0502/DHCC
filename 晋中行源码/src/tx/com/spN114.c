/*************************************************
* �� �� ��:  spN114.c
* ���������� �ƿ�����
*
* ��    ��:  andy
* ������ڣ� 2004��1��22��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/
#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "note_mst_c.h"
#include "note_mst_hst_c.h"
#include "com_branch_c.h"
#include "com_tel_c.h"
#include "card_book_c.h"
#include "mdm_attr_c.h"
extern int pubf_card_loop_create_cardno(const char * start_no,const char * end_no,long int mk_date,char * filename,char **errmsg);
int spN114()
{
	char	cHstFlag[2];		/* �Ƿ�ɾ����ϸ��־Yɾ��N��ɾ��*/
								/* �����ɾ����������һ����ϸ */
	long	trace_no;			/* ��ˮ�� */
	int	ret;
	int len1,len;
	long cnt,cnt1;
	struct card_book_c card_book;
	struct card_book_c o_card_book;
	struct mdm_attr_c mdm_attr;
	double dMaxno=0;
	char cMaxno[20];
	memset(cMaxno, 0 , sizeof(cMaxno));
	/* ��ʼ��g_pub_tx�ṹ */
	pub_base_sysinit();
	/**����Ȩ�޼��**/
	if( pub_com_NoteAuthChk( g_pub_tx.tx_br_no,g_pub_tx.tel,101,0 ) )
	{
		vtcp_log("%s,%d Ȩ�޼�����",__FILE__,__LINE__);
		goto ErrExit;
	}
	get_zd_data( "1161" , g_pub_tx.mdm_code );
	get_zd_data( "0370" , g_pub_tx.ac_no );
	get_zd_data( "0380" , g_pub_tx.ac_no1 );
	get_zd_long( "0520" , &cnt );

	len=strlen( g_pub_tx.ac_no );
	len1=strlen( g_pub_tx.ac_no1 );
	vtcp_log("%s,%d ac_no=[%s]",__FILE__,__LINE__,g_pub_tx.ac_no);
	if( len!=16 && len!=19 )
	{
		sprintf( acErrMsg,"���ų��ȴ���" );
		WRITEMSG
		strcpy( g_pub_tx.reply,"O023" );
		goto ErrExit;
	}
	if( len1!=16 && len1!=19 )
	{
		sprintf( acErrMsg,"���ų��ȴ���" );
		WRITEMSG
		strcpy( g_pub_tx.reply,"O023" );
		goto ErrExit;
	}
	sql_max_double("card_book","end_no",&dMaxno,"1=1");
	vtcp_log("%s,%d max no ==[%f]",__FILE__,__LINE__,dMaxno);
	dMaxno++;
	sprintf(cMaxno,"%08.0f",dMaxno);
	if(memcmp(cMaxno,g_pub_tx.ac_no+8,8))
	{
		sprintf( acErrMsg,"�������С����ԭ�����Ų���������" );
		WRITEMSG
		vtcp_log("%s,%d max_no=[%s],start_no=[%s]",__FILE__,__LINE__,cMaxno,g_pub_tx.ac_no);
		strcpy( g_pub_tx.reply,"D184" );
		goto ErrExit;		
	}
	/* rem by LiuHuafeng �о�ԭ�����������꿨�������������ƿ�����
		��Ͳ����������ƿ�������,���԰�У�鿨��У��λ��Ϊ��������У��λ
	if( pub_card_par_1212( 0,g_pub_tx.ac_no,g_pub_tx.crd_no ) )
	{
		sprintf( acErrMsg,"����[%s]����",g_pub_tx.ac_no );
		WRITEMSG
		strcpy( g_pub_tx.reply,"D182" );
		goto ErrExit;
	}
	if( pub_card_par_1212( 0,g_pub_tx.ac_no1,g_pub_tx.crd_no ) )
	{
		sprintf( acErrMsg,"����[%s]����",g_pub_tx.ac_no1 );
		WRITEMSG
		strcpy( g_pub_tx.reply,"D183" );
		goto ErrExit;
	}
	*******�����ǲ�������У��λ����*******/
	if( pub_card_par_1212( 1,g_pub_tx.ac_no,g_pub_tx.crd_no ) )
	{
		sprintf( acErrMsg,"����[%s]����",g_pub_tx.ac_no );
		WRITEMSG
		strcpy( g_pub_tx.reply,"D182" );
		goto ErrExit;
	}
	if( pub_card_par_1212( 1,g_pub_tx.ac_no1,g_pub_tx.crd_no ) )
	{
		sprintf( acErrMsg,"����[%s]����",g_pub_tx.ac_no1 );
		WRITEMSG
		strcpy( g_pub_tx.reply,"D183" );
		goto ErrExit;
	}
	/* end by LiuHuafeng 20070125 ****/
	
	pub_card_CardToNote( g_pub_tx.ac_no,g_pub_tx.beg_note_no ) ;
vtcp_log("%s,%d end_not_no==%s,beg_note_no=[%s]",__FILE__,__LINE__,g_pub_tx.end_note_no,g_pub_tx.beg_note_no);
	pub_card_CardToNote( g_pub_tx.ac_no1,g_pub_tx.end_note_no ) ;
vtcp_log("%s,%d end_not_no==%s,beg_note_no=[%s]",__FILE__,__LINE__,g_pub_tx.end_note_no,g_pub_tx.beg_note_no);
	cnt1=atol(g_pub_tx.end_note_no+8)-atol(g_pub_tx.beg_note_no+8)+1;
	if( cnt!=cnt1 )
	{
		vtcp_log("%s,%d ac_no==%s,ac_no1=[%s]",__FILE__,__LINE__,g_pub_tx.ac_no,g_pub_tx.ac_no1);
		vtcp_log("%s,%d end_not_no==%s,beg_note_no=[%s]",__FILE__,__LINE__,g_pub_tx.end_note_no,g_pub_tx.beg_note_no);
		vtcp_log("%s,%d cnt=%d,cnt1=[%d]",__FILE__,__LINE__,cnt,cnt1);
		vtcp_log("%s,%d end_not=%d,beg_not=[%d]",__FILE__,__LINE__,atol(g_pub_tx.end_note_no+8),atol(g_pub_tx.beg_note_no+8));
		sprintf( acErrMsg,"���ų��ȴ���" );
		WRITEMSG
		strcpy( g_pub_tx.reply,"N079" );
		goto ErrExit;
	}

	memset( &card_book,0,sizeof(card_book) );

	strcpy( card_book.mdm_code,g_pub_tx.mdm_code );
	strcpy( card_book.beg_no,g_pub_tx.ac_no );
	strcpy( card_book.end_no,g_pub_tx.ac_no1 );
	card_book.cnt=cnt ;
	card_book.tx_date=g_pub_tx.tx_date ;
	strcpy( card_book.br_no,g_pub_tx.tx_br_no );
	strcpy( card_book.tel,g_pub_tx.tel );

	ret = Card_book_Sel( g_pub_tx.reply,&o_card_book,
		"mdm_code='%s' and beg_no>='%s' and end_no<='%s'",
		g_pub_tx.mdm_code,card_book.beg_no,card_book.end_no );
	if( ret==100 )
	{
		;
	}
	else if( ret )
	{
		TRACE
		goto ErrExit;
	}
	else
	{
		sprintf(acErrMsg,"���ִ���[%s][%s][%s]",
			g_pub_tx.mdm_code,card_book.beg_no,card_book.end_no );
		WRITEMSG
		strcpy( g_pub_tx.reply,"D184" );
		goto ErrExit;
	}

	if( Card_book_Ins( card_book , g_pub_tx.reply ) )
		goto ErrExit;
	
	ret=Mdm_attr_Sel(g_pub_tx.reply,&mdm_attr,"mdm_code='%s'",g_pub_tx.mdm_code);
	if( ret )
	{
		TRACE
		goto ErrExit;
	}

	strcpy( g_pub_tx.note_type,mdm_attr.note_type );
	g_pub_tx.tx_amt1=(double)cnt;

	/* rem by LiuHuafeng 20070125
	 * ƾ֤����ù�Ա�Լ���������Ƚ����
	if( pzrk() )
		goto ErrExit;
	*****************************/
	/* add by LiuHuafeng 2007-1-26 16:45 ����д�������ļ����� */
	{
		char * pErrmsg;
		char cFilename[512];
		memset(cFilename , 0 , sizeof(cFilename));
		vtcp_log("%s,%d beg_no=[%s], end_no=[%s]",__FILE__,__LINE__,card_book.beg_no,card_book.end_no);
		g_reply_int=pubf_card_loop_create_cardno(card_book.beg_no,card_book.end_no,g_pub_tx.tx_date,cFilename,&pErrmsg);
		if(g_reply_int)
		{
			TRACE
			strncpy(g_pub_tx.reply,"P157",4);
			memcpy(acErrMsg,pErrmsg,40);
			goto ErrExit;
		}
	}
	/* end by LiuHuafeng 2007-1-26 16:46 */
OkExit:
        strcpy( g_pub_tx.reply, "0000" );
        sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
        WRITEMSG
        set_zd_data("0120",g_pub_tx.reply);
        return 0;
ErrExit:
        sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
        WRITEMSG
        set_zd_data("0120",g_pub_tx.reply);
        return 1;
}
int pzrk()
{
	struct	note_mst_c	sNoteMst;	/* ��Աƾ̨֤�� */
	char	cNoteAct[2];			/* ƾ֤��Ϊ */
	char	tw_br_no[6];			/* �Է������� */
	char	tw_tel[6];			/* �Է�����Ա */
	char	cInOut[2];       		/* ת�Ʊ�־ */
	int	ret;
	
	memset( &sNoteMst, 0x00, sizeof( struct note_mst_c ) );
	memset( cNoteAct, 0x00, sizeof( cNoteAct ) );
	memset( tw_br_no, 0x00, sizeof( tw_br_no ) );
	memset( tw_tel, 0x00, sizeof( tw_tel ) );
	memset( cInOut, 0x00, sizeof( cInOut ) );
	
	strcpy( cNoteAct, "1" );		/* "1"��ʾΪƾ֤������ */
	strcpy( cInOut, "I" );			/* I��ת�� O��ת�� */
	
	/* ϵͳ��ʼ������ʼ��g_pub_tx)���ݳ�ʼ�� */

	/* ƾ֤��ʼ���롢��ֹ�����飨������ͷ�� */
	ret=pub_com_ChkNoteHead( g_pub_tx.note_type, g_pub_tx.beg_note_no, g_pub_tx.tel );
	if( ret == -1 )
	{
		sprintf( acErrMsg, "���ƾ֤��ʼ�������ͷ��" );
		WRITEMSG
		goto ErrExit;	
	}
	ret=pub_com_ChkNoteHead( g_pub_tx.note_type, g_pub_tx.end_note_no, g_pub_tx.tel );
	if( ret == -1 )	
	{
		sprintf( acErrMsg, "���ƾ֤��ֹ�������ͷ��" );
		WRITEMSG
		goto ErrExit;	
	}
	/* ���ƾ֤�Ƿ���ڣ��Ƿ����� */
	ret = pub_com_ChkNoteUse( g_pub_tx.note_type );
	if ( ret )
	{
		sprintf( acErrMsg, "���ƾ֤�Ƿ���ڣ��Ƿ����ô�" );
		WRITEMSG
		goto ErrExit;		
	}

	/* ��Ա�Ƿ�ӵ��ƾ֤ */
	strcpy( sNoteMst.tel, g_pub_tx.tel );
	strcpy( sNoteMst.br_no,  g_pub_tx.tx_br_no );
	strcpy( sNoteMst.note_type,  g_pub_tx.note_type );
	strcpy( sNoteMst.beg_note_no,  g_pub_tx.beg_note_no );
	strcpy( sNoteMst.end_note_no,  g_pub_tx.end_note_no );
	sNoteMst.cnt = (long)g_pub_tx.tx_amt1;
	
	/* ����Աƾ֤ */
	ret=pub_com_ChkNote( cNoteAct, sNoteMst );
	if( ret == -1 )
	{
		sprintf( acErrMsg, "����Աƾ֤��" );
		WRITEMSG
		goto ErrExit;
	}
	
	/* �Ǽǹ�Աƾ̨֤�� */
	ret=pub_com_RegTelNote( g_pub_tx.tel, g_pub_tx.tx_br_no, cNoteAct, \
				g_pub_tx.note_type, g_pub_tx.beg_note_no, \
				g_pub_tx.end_note_no );
	if( ret == -1 )
	{
		sprintf( acErrMsg, "�Ǽǹ�Աƾ̨֤�ʴ�" );
		WRITEMSG
		goto ErrExit;
	}	

	/* �Ǽǹ�Աƾ֤��ϸ */
	ret=pub_com_RegNoteHst( cNoteAct, sNoteMst, tw_br_no, tw_tel );
	if ( ret == -1 )
	{
		sprintf( acErrMsg, "�Ǽǹ�Աƾ֤��ϸ��" );
		WRITEMSG
		goto ErrExit;
	}

	/* ���ƾ֤��С�� */
	ret = pub_com_ChkNoteMin( g_pub_tx.note_type, g_pub_tx.beg_note_no, \
				  g_pub_tx.tel );
	if ( ret == -1 )
	{
		sprintf( acErrMsg, "���ƾ֤��С�Ŵ�" );
		WRITEMSG
		goto ErrExit;	
	}
	if ( !ret  )
	{
		/* �Ǽ���Сƾ֤���� */
		ret = pub_com_RegNoteMin( g_pub_tx.tx_br_no, g_pub_tx.note_type, \
				g_pub_tx.beg_note_no, g_pub_tx.tel, cInOut);
		if ( ret )
		{
			sprintf( acErrMsg, "�Ǽ�ƾ֤��С�Ŵ�" );
			WRITEMSG
			goto ErrExit;	
		}
	}

	/* ����ˮ��־ */
	ret = pub_ins_trace_log();
	if ( ret )
	{
		sprintf( acErrMsg, "����ˮ��־��" );
		WRITEMSG
		goto ErrExit;
	}

OkExit:
	return 0;
ErrExit:
	return 1;
}

