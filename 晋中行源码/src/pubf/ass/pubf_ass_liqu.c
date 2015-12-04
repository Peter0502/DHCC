/***************************************************************
* �� �� ��:     pubf_acct_trance.c
* ����������
*              1����ͨ��ͨ�ҡ���衢ͬ���޸Ŀ���407̨�ʣ�
*              1���Ǽǿ�����ϸ�ʣ�
*
* ��    ��:     xxx
* ������ڣ�    2004��12��30��
*
* �޸ļ�¼��
*    ��    ��:
*    �� �� ��:
*    �޸�����:
*
**************************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "ass_br_def_c.h"
#include "ass_mst_c.h"
#include "ass_mst_hst_c.h"

pubf_ass_liqu()
{
	int i=0;
	int	add_ind=0;
	char   liqu_br_no[6];
	struct ass_br_def_c opn_ass_br_def;
	struct ass_br_def_c tx_ass_br_def;
	struct ass_br_def_c tmp_ass_br_def;

	memset(&opn_ass_br_def,0x00,sizeof(struct ass_br_def_c));
	memset(&tx_ass_br_def,0x00,sizeof(struct ass_br_def_c));
	memset(&tmp_ass_br_def,0x00,sizeof(struct ass_br_def_c));

	if( !strcmp(g_pub_tx.opn_br_no,g_pub_tx.tx_br_no) )
		return 0;

	if( pub_ass_get_br_def(g_pub_tx.opn_br_no,&opn_ass_br_def) )
	{
		sprintf(acErrMsg,"ȡ�����������˲�����![%s]",g_pub_tx.opn_br_no);
		WRITEMSG
		goto ErrExit;
	}

	if( g_pub_tx.add_ind[0]=='1' )	add_ind=1;
	else	add_ind=0;
	g_reply_int=chg_ass_mst(g_pub_tx.opn_br_no,"",add_ind);
	if( g_reply_int )
	{
		sprintf(acErrMsg,"�޸Ŀ���̨�ʳ���!");
		WRITEMSG
		goto ErrExit;
	}

	if( pub_ass_get_br_def(g_pub_tx.tx_br_no,&tx_ass_br_def) )
	{
		sprintf(acErrMsg,"ȡ���׻������˲�����![%s]",g_pub_tx.opn_br_no);
		WRITEMSG
		goto ErrExit;
	}

	if( g_pub_tx.add_ind[0]=='1' )	add_ind=0;
	else	add_ind=1;
	g_reply_int=chg_ass_mst(g_pub_tx.tx_br_no,"",add_ind);
	if( g_reply_int )
	{
		sprintf(acErrMsg,"�޸Ŀ���̨�ʳ���!");
		WRITEMSG
		goto ErrExit;
	}

	liqu_br_no[0]=='\0';
	for( i=1;i<10;i++ )
	{
		if( !strcmp(opn_ass_br_def.br_knd,tx_ass_br_def.br_knd) )
		{
			if( !strcmp(opn_ass_br_def.sub_br_no,tx_ass_br_def.sub_br_no) )
			{
				strcpy(liqu_br_no,opn_ass_br_def.sub_br_no);
			}
			else 
			{
				if(pub_ass_get_br_def(opn_ass_br_def.sub_br_no,&tmp_ass_br_def))
				{
					sprintf(acErrMsg,"ȡ���������ϼ����˻���������![%s]",opn_ass_br_def.sub_br_no);
					WRITEMSG
					goto ErrExit;
				}

				if( g_pub_tx.add_ind[0]=='1' )	add_ind=0;
				else	add_ind=1;
				g_reply_int=chg_ass_mst(tmp_ass_br_def.br_no,g_pub_tx.opn_br_no,add_ind);
				if( g_reply_int )
				{
					sprintf(acErrMsg,"�޸Ŀ���̨�ʳ���!");
					WRITEMSG
					goto ErrExit;
				}

				if( g_pub_tx.add_ind[0]=='1' )	add_ind=1;
				else	add_ind=0;
				g_reply_int=chg_ass_mst(tmp_ass_br_def.br_no,"",add_ind);
				if( g_reply_int )
				{
					sprintf(acErrMsg,"�޸Ŀ���̨�ʳ���!");
					WRITEMSG
					goto ErrExit;
				}

				memcpy(&opn_ass_br_def,&tmp_ass_br_def,sizeof(struct ass_br_def_c));

				if(pub_ass_get_br_def(tx_ass_br_def.sub_br_no,&tmp_ass_br_def))
				{
					sprintf(acErrMsg,"ȡ���������ϼ�����������![%s]",tx_ass_br_def.sub_br_no);
					WRITEMSG
					goto ErrExit;
				}

				if( g_pub_tx.add_ind[0]=='1' )	add_ind=1;
				else	add_ind=0;
				g_reply_int=chg_ass_mst(tmp_ass_br_def.br_no,g_pub_tx.tx_br_no,add_ind);
				if( g_reply_int )
				{
					sprintf(acErrMsg,"�޸Ŀ���̨�ʳ���!");
					WRITEMSG
					goto ErrExit;
				}

				if( g_pub_tx.add_ind[0]=='1' )	add_ind=0;
				else	add_ind=1;
				g_reply_int=chg_ass_mst(tmp_ass_br_def.br_no,"",add_ind);
				if( g_reply_int )
				{
					sprintf(acErrMsg,"�޸Ŀ���̨�ʳ���!");
					WRITEMSG
					goto ErrExit;
				}
				memcpy(&tx_ass_br_def,&tmp_ass_br_def,sizeof(struct ass_br_def_c));
			}
		}
		else if( opn_ass_br_def.br_knd>tx_ass_br_def.br_knd )
		{
			if( pub_ass_get_br_def(tx_ass_br_def.sub_br_no,&tmp_ass_br_def) )
			{
				sprintf(acErrMsg,"ȡ���������ϼ�����������![%s]",tx_ass_br_def.sub_br_no);
				WRITEMSG
				goto ErrExit;
			}

			if( g_pub_tx.add_ind[0]=='1' )	add_ind=1;
			else	add_ind=0;
			g_reply_int=chg_ass_mst(tmp_ass_br_def.br_no,g_pub_tx.tx_br_no,add_ind);
			if( g_reply_int )
			{
				sprintf(acErrMsg,"�޸Ŀ���̨�ʳ���!");
				WRITEMSG
				goto ErrExit;
			}

			if( g_pub_tx.add_ind[0]=='1' )	add_ind=0;
			else	add_ind=1;
			g_reply_int=chg_ass_mst(tmp_ass_br_def.br_no,"",add_ind);
			if( g_reply_int )
			{
				sprintf(acErrMsg,"�޸Ŀ���̨�ʳ���!");
				WRITEMSG
				goto ErrExit;
			}
			memcpy(&tx_ass_br_def,&tmp_ass_br_def,sizeof(struct ass_br_def_c));
		}
		else
		{
			if( pub_ass_get_br_def(opn_ass_br_def.sub_br_no,&tmp_ass_br_def) )
			{
				sprintf(acErrMsg,"ȡ���������ϼ�����������![%s]",opn_ass_br_def.sub_br_no);
				WRITEMSG
				goto ErrExit;
			}

			if( g_pub_tx.add_ind[0]=='1' )	add_ind=0;
			else	add_ind=1;
			g_reply_int=chg_ass_mst(tmp_ass_br_def.br_no,g_pub_tx.opn_br_no,add_ind);
			if( g_reply_int )
			{
				sprintf(acErrMsg,"�޸Ŀ���̨�ʳ���!");
				WRITEMSG
				goto ErrExit;
			}

			if( g_pub_tx.add_ind[0]=='1' )	add_ind=1;
			else	add_ind=0;
			g_reply_int=chg_ass_mst(tmp_ass_br_def.br_no,"",add_ind);
			if( g_reply_int )
			{
				sprintf(acErrMsg,"�޸Ŀ���̨�ʳ���!");
				WRITEMSG
				goto ErrExit;
			}
			memcpy(&opn_ass_br_def,&tmp_ass_br_def,sizeof(struct ass_br_def_c));
		}
	}

	if( liqu_br_no[0]!=NULL )
	{
		if( g_pub_tx.add_ind[0]=='1' )	add_ind=0;
		else	add_ind=1;

		g_reply_int=chg_ass_mst(liqu_br_no,opn_ass_br_def.br_no,add_ind);
		if( g_reply_int )
		{
			sprintf(acErrMsg,"�޸Ŀ���̨�ʳ���!");
			WRITEMSG
			goto ErrExit;
		}

		if( g_pub_tx.add_ind[0]=='1' )	add_ind=1;
		else	add_ind=0;

		g_reply_int=chg_ass_mst(liqu_br_no,tx_ass_br_def.br_no,add_ind);
		if( g_reply_int )
		{
			sprintf(acErrMsg,"�޸Ŀ���̨�ʳ���!");
			WRITEMSG
			goto ErrExit;
		}
	}
	else
	{
		sprintf(acErrMsg,"�������������!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"");
		goto ErrExit;
	}

	return 0;
ErrExit:
	return 1;
}

chg_ass_mst(char br_no[6],char down_br_no[6],int add_ind)
{
	struct ass_mst_c	ass_mst_c;
	struct ass_mst_hst_c	ass_mst_hst_c;

	pub_base_strpack(br_no);
	pub_base_strpack(down_br_no);
	memset(&ass_mst_c,0x00,sizeof(struct ass_mst_c));
	memset(&ass_mst_hst_c,0x00,sizeof(struct ass_mst_hst_c));

	if( down_br_no[0]==NULL )
	{
		g_reply_int=Ass_mst_Dec_Upd(g_pub_tx.reply,"br_no='%s'",br_no);
		if( g_reply_int )
		{
			sprintf(acErrMsg,"ȡ�������ļ�����![%s][%d]",br_no,g_reply_int);
			WRITEMSG
			strcpy(g_pub_tx.reply,"");
			goto ErrExit;
		}

	}
	else	{
		g_reply_int=Ass_mst_Dec_Upd(g_pub_tx.reply,"br_no='%s' and cash_br_no='%s' ",br_no,down_br_no);
		if( g_reply_int )
		{
			sprintf(acErrMsg,"ȡ�������ļ�����![%s][%d]",br_no,g_reply_int);
			WRITEMSG
			strcpy(g_pub_tx.reply,"");
			goto ErrExit;
		}
	}

	g_reply_int=Ass_mst_Fet_Upd(&ass_mst_c,g_pub_tx.reply);
	if( g_reply_int )
	{
		sprintf(acErrMsg,"ȡ�������ļ�����![%s][%d]",br_no,g_reply_int);
		WRITEMSG
		strcpy(g_pub_tx.reply,"");
		goto ErrExit;
	}
	
	if( ass_mst_c.sts[0]!='1' )
	{
		sprintf(acErrMsg,"ȡ�������ļ�״̬������![%s]",ass_mst_c.sts);
		WRITEMSG
		strcpy(g_pub_tx.reply,"");
		goto ErrExit;
	}

	/*** ���ü�������ĺ��� ***/

	ass_mst_c.lst_date=g_pub_tx.tx_date;

	if( add_ind==0 )
	{
		ass_mst_c.bal=ass_mst_c.bal-g_pub_tx.tx_amt1;
	}
	else if( add_ind==1 )
	{
		ass_mst_c.bal=ass_mst_c.bal+g_pub_tx.tx_amt1;
	}
	else
	{
		sprintf(acErrMsg,"������־��![%d]",add_ind);
		WRITEMSG
		strcpy(g_pub_tx.reply,"");
		goto ErrExit;
	}

	g_reply_int=Ass_mst_Upd_Upd(&ass_mst_c,g_pub_tx.reply);
	if( g_reply_int )
	{
		sprintf(acErrMsg,"ȡ�������ļ�����![%s][%d]",br_no,g_reply_int);
		WRITEMSG
		strcpy(g_pub_tx.reply,"");
		goto ErrExit;
	}

	Ass_mst_Clo_Upd();

	strcpy(ass_mst_hst_c.opn_br_no,br_no);
	ass_mst_hst_c.tx_date=g_pub_tx.tx_date;
	ass_mst_hst_c.trace_no=g_pub_tx.trace_no;
	strcpy(ass_mst_hst_c.tx_br_no,g_pub_tx.tx_br_no);
	ass_mst_hst_c.ac_id=ass_mst_c.ac_id;
	ass_mst_hst_c.ac_seqn=ass_mst_c.ac_seqn;
	sprintf(ass_mst_hst_c.add_ind,"%1d",add_ind);
	ass_mst_hst_c.amt=g_pub_tx.tx_amt1;
	ass_mst_hst_c.bal=ass_mst_c.bal;
	ass_mst_hst_c.intst_acm=ass_mst_c.intst_acm;
	ass_mst_hst_c.tx_time=g_pub_tx.tx_time;
	strcpy(ass_mst_hst_c.brf,"ͨ������");
	ass_mst_hst_c.hst_cnt++;
	strcpy(ass_mst_hst_c.tel,g_pub_tx.tel);
	strcpy(ass_mst_hst_c.chk,g_pub_tx.chk);
	strcpy(ass_mst_hst_c.auth,g_pub_tx.auth);
	g_reply_int=Ass_mst_hst_Ins(ass_mst_hst_c,g_pub_tx.reply);
	if( g_reply_int )
	{
		sprintf(acErrMsg,"���뿼����ϸ�ʳ���![%d]",g_reply_int);
		WRITEMSG
		strcpy(g_pub_tx.reply,"");
		goto ErrExit;
	}
	
	return 0;
ErrExit:
	return 1;
}
