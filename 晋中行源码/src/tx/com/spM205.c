/*************************************************
* �� �� ��:  spM205.c
* ���������� �ṩ�����ڲ������ֽ����Ԥ�����Ĺ��ܡ�
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
#include "com_tel_c.h"
#include "com_tx_br_rel_c.h"
#include "cash_book_c.h"
#include "com_branch_c.h"
#include "com_sys_parm_c.h"

int spM205()
{
	struct	com_tel_c		sComTel;		/* ��Ա��Ϣ */
	struct	com_tx_br_rel_c	sComTxBrRel;	/* ���׻�����ϵ�� */
	struct	cash_book_c		sCashBook;		/* �ֽ��ո��Ǽǲ� */
	struct	com_branch_c	sComBranch;		/* ����������� */
	char	cOperCode[2];					/* ������ */
	char	cTwBrNo[6];						/* �Է��������� */
	long	lNoteNo;						/* ƾ֤���� */
	long	lYtDate;						/* Ԥ��Ʊ��ӡ���� */
	int		ret;
	struct  com_sys_parm_c sys_parm;

	memset( &sComTel, 0x00, sizeof( struct com_tel_c ) );
	memset( &sComTxBrRel, 0x00, sizeof( struct com_tx_br_rel_c ) );
	memset( &sCashBook, 0x00, sizeof( struct cash_book_c ) );
	memset( &sComBranch, 0x00, sizeof( struct com_branch_c ) );
	memset( cOperCode, 0x00, sizeof( cOperCode ) );
	memset( cTwBrNo, 0x00, sizeof( cTwBrNo ) );
	memset( &g_pub_tx, 0x00, sizeof( g_pub_tx ) );

	/* ���ݳ�ʼ�� */
	pub_base_sysinit();
	ret = Com_sys_parm_Sel(g_pub_tx.reply,&sys_parm,"1=1");
	if( ret ) goto ErrExit;

	/* ��ǰ̨��Ļȡֵ */
	get_zd_data( "0670", cOperCode );		/* ������ */
	get_zd_data( "0910", cTwBrNo );			/* �Է��������� */
	get_zd_data( "1181", sCashBook.cur_no );/* ���� */
	get_zd_long( "0440", &lNoteNo );		/* ƾ֤���� */
	get_zd_double( "1184", &sCashBook.amt );/* ��� */

	if( cOperCode[0] == '0' )	
	{
		strcpy( g_pub_tx.brf,"Ԥ��Ʊ" );
		g_pub_tx.tx_amt1=sCashBook.amt ;
		/* ��齻�׻�����ϵ�����Ƿ�������¼���ϵ */
		ret = Com_tx_br_rel_Sel( g_pub_tx.reply, &sComTxBrRel, 
			"br_no='%s' and up_br_no='%s' and rel_type=1", 
			g_pub_tx.tx_br_no, cTwBrNo );
		if( ret == 0 )
		{
			sprintf( acErrMsg,"�������¼�����Ԥ��Ʊ!������[%s]�ϼ�����[%s]",
				g_pub_tx.tx_br_no, cTwBrNo );
			WRITEMSG
			strcpy( g_pub_tx.reply, "N094" );
			goto ErrExit;
		}else if( ret != 100 && ret )
		{
			sprintf( acErrMsg, "ִ��Com_tx_br_rel_Sel��!ret=[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}

		/* ��齻�׻�����ϵ�����Ƿ�������¼���ϵ */
		ret = Com_tx_br_rel_Sel( g_pub_tx.reply, &sComTxBrRel, 
			"br_no='%s' and up_br_no='%s' and rel_type=1", 
				cTwBrNo,g_pub_tx.tx_br_no );
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

		strcpy( sCashBook.tw_br_no, cTwBrNo );		/* �Է��������� */
		strcpy( sCashBook.br_no, g_pub_tx.tx_br_no);/* ���׻��� */
		strcpy( sCashBook.tel, g_pub_tx.tel );		/* ��Ա���� */
		strcpy( sCashBook.ind, "2" );				/* ��ȡ��־: 2 Ԥ��*/
		sCashBook.trace_no = g_pub_tx.trace_no;		/* ��ˮ�ţ��ֽ���������� */
		sCashBook.wrk_no = 0;		/* ��ˮ�� */
		sCashBook.out_no = 0;		/* ��ˮ�� */
		sCashBook.tx_date = g_pub_tx.tx_date;
		set_zd_long("0440",sCashBook.trace_no);
		/* Ϊ��ӡ�ֽ��������ֵ */
		set_zd_data( "0580", "��ͷ��" );			/* ��; */
		ret = pub_base_deadlineD( g_pub_tx.tx_date, 1, &lYtDate);
		if( ret )
		{
			sprintf( acErrMsg, "���㽻�����ڵ���һ�մ���!" );
			WRITEMSG
			goto ErrExit;
		}
		set_zd_long( "0450", lYtDate );				/* Ԥ��Ʊ��ӡ���� */

		/* �Ǽ��ֽ��ո��Ǽǲ� */
		ret = Cash_book_Ins( sCashBook, g_pub_tx.reply );
		if( ret )
		{
			sprintf( acErrMsg, "ִ��Cash_book_Ins��!ret=[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}
	}
	else		
	{
		strcpy( g_pub_tx.brf,"����Ԥ��Ʊ" );
		vtcp_log( "YKPHX����Ԥ��Ʊ" );
		g_pub_tx.tx_amt1=sCashBook.amt ;
		/* ��ѯ�ֽ��ո��Ǽǲ����Ƿ����������ĶԷ��������룬
		���һ�²��һ�ȡ��־Ϊ"δȡ"�ļ�¼ */
		vtcp_log( "tx_date=%d and br_no='%s' and tw_br_no='%s' and amt=%.2lf and ind='2'", sys_parm.lst_date,g_pub_tx.tx_br_no,cTwBrNo,sCashBook.amt );
		ret = Cash_book_Dec_Upd( g_pub_tx.reply, "tx_date=%d and br_no='%s' and tw_br_no='%s' and amt='%.2lf'and ind='2'",sys_parm.lst_date,g_pub_tx.tx_br_no,cTwBrNo,sCashBook.amt);
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
       		strcpy( g_pub_tx.reply, "M026" );
       		goto ErrExit;
    		}else if ( ret )
    		{
       		sprintf( acErrMsg, "ִ��Cash_book_Fet_Upd��!ret=[%d]",ret );
       		WRITEMSG
       		goto ErrExit;
    		}else
    		{
    		if( sCashBook.trace_no != lNoteNo )
			{
				sprintf( acErrMsg, "����ƾ֤��������![%ld]",lNoteNo );
				WRITEMSG
				strcpy( g_pub_tx.reply, "M020" );
				goto ErrExit;
			}
			
			strcpy( sCashBook.ind, "3" );	/* ��ȡ��־: ����*/
			sCashBook.out_no=g_pub_tx.trace_no;

    		ret = Cash_book_Upd_Upd( sCashBook, g_pub_tx.reply );
    		if( ret )
			{
				sprintf( acErrMsg, "ִ��Cash_book_Upd_Upd��!ret=[%d]", ret );
				WRITEMSG
				goto ErrExit;
			}
			sCashBook.tx_date=g_pub_tx.tx_date;
			sCashBook.out_no=g_pub_tx.trace_no;
			sCashBook.wrk_no=0;
			strcpy( sCashBook.ind, "0" );	/* ��ȡ��־: 0 δȡ 1 ��ȡ */
			/* �Ǽ��ֽ��ո��Ǽǲ� */
			ret = Cash_book_Ins( sCashBook, g_pub_tx.reply );
			if( ret )
			{
				sprintf( acErrMsg, "ִ��Cash_book_Ins��!ret=[%d]", ret );
				WRITEMSG
				goto ErrExit;
			}
    	}

    	Cash_book_Clo_Upd( );

	}

	/* ����ˮ��־ */
	strcpy(g_pub_tx.no_show,"0");
	ret = pub_ins_trace_log();
	if ( ret )
	{
		sprintf( acErrMsg, "����ˮ��־��" );
		WRITEMSG
		goto ErrExit;
	}

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
