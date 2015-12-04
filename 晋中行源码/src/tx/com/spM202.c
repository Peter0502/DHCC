/*************************************************
* �� �� ��:  spM202.c
* ���������� �ṩ�����ڲ������ֽ���ɵĹ��ܡ�
*
* ��    ��:  andy
* ������ڣ� 2004��1��6��
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
#include "com_tx_br_rel_c.h"
#include "cash_book_c.h"
#include "com_branch_c.h"
#include "note_mst_c.h"
#include "note_parm_c.h"

int spM202()
{
	struct	com_tx_br_rel_c	sComTxBrRel;/* ���׻�����ϵ�� */
	struct	cash_book_c		sCashBook;	/* �ֽ��ո��Ǽǲ� */
	struct	com_branch_c	sComBranch;	/* ����������� */
	struct  note_mst_c    sNoteMst;   /* ��Աƾ̨֤�� */
	struct  note_parm_c   sNoteParm;  /* ƾ֤���� */
	char	cOperCode[2];				/* ������ */
	char	cTwBrNo[6];					/* �Է��������� */
	char	tBrNo[6];				  /* ����Է����� */
	char	oBrNo[6];				  /* ���㿪������ */
	char	tTel[5];					/* ����Է���Ա */
	
	long	lNoteNo;					/* ƾ֤���� */
	long  tNoteNo;          /* ��������ˮ��(����)*/
	double	dAmt;						/* ��� */
	int	ret, num = 0;


	memset( &sComTxBrRel, 0x00, sizeof( struct com_tx_br_rel_c ) );
	memset( &sCashBook, 0x00, sizeof( struct cash_book_c ) );
	memset( &sComBranch, 0x00, sizeof( struct com_branch_c ) );
	memset( &sNoteMst, 0x00, sizeof( struct note_mst_c ) );
	memset( &sNoteParm, 0x00, sizeof( struct note_parm_c ) );
	memset( cOperCode, 0x00, sizeof( cOperCode ) );
	memset( cTwBrNo, 0x00, sizeof( cTwBrNo ) );
	memset( tBrNo, 0x00, sizeof( tBrNo ) );
	memset( oBrNo, 0x00, sizeof( oBrNo ) );
	memset( tTel, 0x00, sizeof( tTel ) );


	/* ���ݳ�ʼ�� */
	pub_base_sysinit();

	/* ��ǰ̨��Ļȡֵ */
	get_zd_data( "0670", cOperCode );		/* ������ */
	get_zd_data( "0910", cTwBrNo );			/* �Է��������� */
	get_zd_data( "1181", sCashBook.cur_no );	/* ���� */
	get_zd_long( "0440", &lNoteNo );		/* ƾ֤���� */
	get_zd_double( "1184", &dAmt );			/* ��� */
	
	get_zd_data("0450",g_pub_tx.new_note_no);   /* ƾ֤���� */
  get_zd_long("0460", &tNoteNo );             /* �������� */
  strcpy( g_pub_tx.note_type , "152" );   /* ƾ֤���� */
vtcp_log("g_pub_tx.new_note_no:%s",lNoteNo);
	if( cOperCode[0] == '0' )			/* �ֽ𸶳� */
	{
		/*���Է������Ƿ���ǩ��*/
		ret=Com_branch_Sel( g_pub_tx.reply, &sComBranch, "br_no='%s'",
							cTwBrNo );
		if( ret )
		{
			sprintf( acErrMsg, "��ѯ�������������!!" );
			WRITEMSG
			goto ErrExit;
		}

		if( sComBranch.wrk_sts[0] == '0' )
		{
			sprintf( acErrMsg, "�Է������Ѿ�ǩ��!!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "O253" );
			goto ErrExit;
		}

		if( sComBranch.wrk_sts[0] == '*' )
		{
			sprintf( acErrMsg, "�Է������ѱ��ر�!!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "O254" );
			goto ErrExit;
		}

		/* ��齻�׻�����ϵ�����Ƿ�������¼���ϵ */
		ret = Com_tx_br_rel_Sel( g_pub_tx.reply, &sComTxBrRel, "br_no='%s' and up_br_no='%s'\
							and rel_type=1", cTwBrNo,g_pub_tx.tx_br_no );
		if( ret != 100 && ret )
		{
			sprintf( acErrMsg, "ִ��Com_tx_br_rel_Sel��!ret=[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}else if( ret == 0 )
		{
			set_zd_data( "0580", "��ͷ��" );
		}else
		{
			/* ��齻�׻�����ϵ�����Ƿ�������¼���ϵ */
			ret = Com_tx_br_rel_Sel( g_pub_tx.reply, &sComTxBrRel, "br_no='%s' and up_br_no='%s'\
									and rel_type=1", g_pub_tx.tx_br_no, cTwBrNo );
			if( ret != 100 && ret )
			{
				sprintf( acErrMsg, "ִ��Com_tx_br_rel_Sel��!ret=[%d]", ret );
				WRITEMSG
				goto ErrExit;
			}else if( ret == 100 )
			{
				sprintf( acErrMsg, "��齻�׻�����ϵ���в����ڴ����¼���ϵ![%s][%s]",cTwBrNo,g_pub_tx.tx_br_no );
				WRITEMSG
				strcpy( g_pub_tx.reply, "N068" );
				goto ErrExit;
			}

			set_zd_data( "0580", "�Ͻ�" );
		}
		
	   ret = Note_parm_Sel(g_pub_tx.reply, &sNoteParm, "note_type = '%s'", g_pub_tx.note_type );
	   if( ret == 100 )
	   {
	       sprintf(acErrMsg,"ȡƾ֤����note_parm���!note_type=[%s]",g_pub_tx.note_type);
	       WRITEMSG
	       strcpy( g_pub_tx.reply, "N001" );
	       goto ErrExit;
	   }else if ( ret )
	   {
	       sprintf( acErrMsg, "ִ��Note_parm_Sel����![%d]", ret );
	       WRITEMSG
	       goto ErrExit;
	   }	
		/*** �ֽ������ƾ֤�Ĵ��� add by martin 20071031 ***/
		if( !strcmp(g_pub_tx.tx_br_no,"10001") )
		{
			 vtcp_log("���㿪ʼƾ֤���� !strcmp(g_pub_tx.tx_br_no[%s],10001)[%s][%d]\n",g_pub_tx.tx_br_no,__FILE__,__LINE__);
			  	/* 20140427 BY WYW */ 
			   ret=Note_mst_Sel( g_pub_tx.reply, &sNoteMst, " br_no='%s' and  sts='0' and note_type='152' and substr(beg_note_no,9,8)<='%s' and substr(end_note_no,9,8)>='%s'" ,cTwBrNo,g_pub_tx.new_note_no,g_pub_tx.new_note_no);
			   if(ret)
			   	  {
		          	 sprintf(acErrMsg,"֧��ƾ֤��ѯ����!");
		          	 WRITEMSG
		          	 goto ErrExit;
		        }
		     ret=pub_com_NoteClsNo(  g_pub_tx.note_type ,g_pub_tx.new_note_no , g_pub_tx.new_note_no , sNoteMst.tel );
		        if(ret)
		          {
		          	sprintf(acErrMsg,"����ƾ֤����!");
		          	WRITEMSG
		          	goto ErrExit;
		          }
		          
		     strcpy( tBrNo , g_pub_tx.tx_br_no );
	       strcpy( oBrNo , g_pub_tx.opn_br_no );
	       /**strcpy( tTel , g_pub_tx.tel );**/
	       strcpy( g_pub_tx.tx_br_no , cTwBrNo );
	       strcpy( g_pub_tx.opn_br_no , cTwBrNo );
	       /**strcpy( g_pub_tx.tel , sNoteMst.tel );**/
	       num++;
	       
		}
			else 
			{
           vtcp_log("�����㿪ʼƾ֤���� type[%s][%s][%s][%d]\n",g_pub_tx.note_type,g_pub_tx.new_note_no,__FILE__,__LINE__);
		        
		  
		        ret=pub_com_NoteClsNo(  g_pub_tx.note_type ,g_pub_tx.new_note_no , g_pub_tx.new_note_no , g_pub_tx.tel );
		        if(ret)
		          {
		          	sprintf(acErrMsg,"����ƾ֤����!");
		          	WRITEMSG
		          	goto ErrExit;
		          }
		  }		          
		sprintf( acErrMsg, "��Աƾ֤ʹ�ü���" );
	  WRITEMSG
		sprintf( acErrMsg, "111111111111111svc_ind=[%d]",g_pub_tx.svc_ind);
		WRITEMSG
		ret = pub_acct_NoteUse_log( 0 , 0 , g_pub_tx.note_type, g_pub_tx.new_note_no, g_pub_tx.new_note_no ,atol(g_pub_tx.new_note_no+sNoteParm.head_len)-atol(g_pub_tx.new_note_no+sNoteParm.head_len)+1);
		if( ret )
		{
	   		sprintf( acErrMsg, "��Աƾ֤ʹ�ü��ʴ�" );
	   		WRITEMSG
	   		goto ErrExit;		
		}
		if( num!=0 )
			{
				 vtcp_log("����ƾ֤�������num=[%d][%s][%d] \n",num,__FILE__,__LINE__);
		     
	       strcpy( g_pub_tx.tx_br_no , tBrNo );
	       strcpy( g_pub_tx.opn_br_no , oBrNo );
	       /**strcpy( g_pub_tx.tel , tTel );**/
			}
		
		/** end martin **/

		strcpy( sCashBook.tw_br_no, cTwBrNo );			/* �Է��������� */
		strcpy( sCashBook.br_no, g_pub_tx.tx_br_no );	/* ���׻��� */
		strcpy( sCashBook.tel, g_pub_tx.tel );			/* ��Ա���� */
		strcpy( sCashBook.ind, "0" );					/* 	��ȡ��־: 0 δȡ 1 ��ȡ */
		/*sCashBook.wrk_no=0;
		sCashBook.out_no=g_pub_tx.trace_no;*/
		sCashBook.wrk_no=g_pub_tx.trace_no;           /*** ������Ϊ������ ***/
		sCashBook.out_no = atol(g_pub_tx.new_note_no);/*** ƾ֤�� ***/
		sCashBook.trace_no = g_pub_tx.trace_no;		/* ��ˮ�ţ��ֽ���������� */
		sCashBook.amt = dAmt;
		sCashBook.tx_date = g_pub_tx.tx_date;

		/* �Ǽ��ֽ��ո��Ǽǲ� */
		ret = Cash_book_Ins( sCashBook, g_pub_tx.reply );
		if( ret )
		{
			sprintf( acErrMsg, "ִ��Cash_book_Ins��!ret=[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}

		/* Ϊ����ˮ��ֵ */
		strcpy( g_pub_tx.add_ind, "0" );			/* ����:0��1�� */
			
		set_zd_data( "0680", "1" );		/* ������ */
	}
	else			/* �ֽ����� */
	{
		/* ��ѯ�ֽ��ո��Ǽǲ����Ƿ����������ĶԷ��������룬
		���һ�²��һ�ȡ��־Ϊ"δȡ"�ļ�¼ */
		/***sprintf(acErrMsg, "br_no='%s' \
					and tw_br_no='%s' and trace_no=%ld and tx_date=%d",\
					cTwBrNo, g_pub_tx.tx_br_no,\
					lNoteNo, g_pub_tx.tx_date );
		WRITEMSG
		ret = Cash_book_Dec_Upd( g_pub_tx.reply, "br_no='%s' \
					and tw_br_no='%s' and trace_no=%ld and tx_date=%d",\
					cTwBrNo, g_pub_tx.tx_br_no,\
					lNoteNo, g_pub_tx.tx_date );  20071101 by martin***/
		sprintf(acErrMsg, "br_no='%s' \
					and tw_br_no='%s' and wrk_no=%ld and out_no=%s and tx_date=%d",\
					cTwBrNo, g_pub_tx.tx_br_no,\
					tNoteNo,g_pub_tx.new_note_no, g_pub_tx.tx_date );
		WRITEMSG
		ret = Cash_book_Dec_Upd( g_pub_tx.reply, "br_no='%s' \
					and tw_br_no='%s' and wrk_no=%ld and out_no=%s and tx_date=%d",\
					cTwBrNo, g_pub_tx.tx_br_no,\
					tNoteNo,g_pub_tx.new_note_no, g_pub_tx.tx_date );
		if( ret )
		{
			sprintf( acErrMsg, "ִ��Cash_book_Dec_Upd��!ret=[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}

		ret = Cash_book_Fet_Upd( &sCashBook, g_pub_tx.reply );
		if ( ret == 100 )
		{
			sprintf( acErrMsg, "�ֽ��ո��Ǽǲ��в����ڴ˸�����¼!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "M006" );
			goto ErrExit;
		}else if ( ret )
		{
			sprintf( acErrMsg, "ִ��Cash_book_Fet_Upd��!ret=[%d]",ret );
			WRITEMSG
			goto ErrExit;
		}
sprintf( acErrMsg, "sCashBook.trace_no[%ld]lNoteNo[%ld]",sCashBook.trace_no,lNoteNo );
WRITEMSG
		if( sCashBook.ind[0] == '1' )
		{
			sprintf( acErrMsg, "�ñʸ�����¼��������!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "M031" );
			goto ErrExit;
		}else if( sCashBook.ind[0] == '2' )
		{
			sprintf( acErrMsg, "�ñʼ�¼ΪԤ��Ʊ��¼������Ӫҵ����δ����������!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "M033" );
			goto ErrExit;
		}

		if( pub_base_CompDblVal( sCashBook.amt, dAmt ) != 0 )
		{
			sprintf( acErrMsg, "�������븶������!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "M030" );
			goto ErrExit;
		}

		strcpy( sCashBook.ind, "1" );	/* ��ȡ��־: 0 δȡ 1 ��ȡ */
		sCashBook.wrk_no=g_pub_tx.trace_no;

		ret = Cash_book_Upd_Upd( sCashBook, g_pub_tx.reply );
		if( ret )
		{
			sprintf( acErrMsg, "ִ��Cash_book_Upd_Upd��!ret=[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}

    		Cash_book_Clo_Upd( );

		/* Ϊ����ˮ��ֵ */
		strcpy( g_pub_tx.add_ind, "1" );			/* ����:0��1�� */

		set_zd_data( "0680", "2" );		/* ������ */
	}

	/* ����ˮ��־ */
/*	strcpy(g_pub_tx.no_show,"1");*/

	strcpy( g_pub_tx.cur_no, sCashBook.cur_no );	/* ���� */
	strcpy( g_pub_tx.ac_no, "10104" );	/* ��;������ֽ� */
	strcpy( g_pub_tx.ct_ind, "1" );	/* ��ת */
	strcpy( g_pub_tx.hst_ind, "1" );	/* ��ת */
	strcpy( g_pub_tx.brf, "�����ֽ��ո�" );
	g_pub_tx.tx_amt1 = dAmt;
	if( cOperCode[0] == '0' )			/* �ֽ𸶳� */
		strcpy( g_pub_tx.add_ind, "1" );
	else						/* �ֽ����� */
		strcpy( g_pub_tx.add_ind, "0" );
	strcpy(g_pub_tx.beg_note_no,cTwBrNo); /*���Է������ŵ�����Ϊ�˲���ˮʱ֪���Ǹ����ĸ����� */
	
	ret = pub_ins_trace_log();
	if ( ret )
	{
		sprintf( acErrMsg, "����ˮ��־��" );
		WRITEMSG
		goto ErrExit;
	}

	if( cOperCode[0] == '0' )			/* �ֽ𸶳� */
		goto OkExit1;
	else						/* �ֽ����� */
		goto OkExit2;

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
OkExit1:
	strcpy( g_pub_tx.reply, "0001" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
OkExit2:
	strcpy( g_pub_tx.reply, "0002" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
}
