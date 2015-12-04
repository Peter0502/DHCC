/*************************************************
* �� �� ��:  sp8405.c
* ���������� ��Ա������ѯ
*
* ��    ��:  rob
* ������ڣ� 2003��09��05��
*
* �޸ļ�¼��
* ��   ��:
* �� �� ��:
* �޸�����:
*************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define EXTERN
#include "public.h"
#include "note_cheq_mst_c.h"
#include "note_parm_c.h"

int sp8405()
{
	struct 	mdm_ac_rel_c 	sMdm_ac_rel_c;		/* �������˻���ϵ�� */
	char f_acno0[25];
	FILE *fp;

	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset( wherelist,0,sizeof(wherelist) );
	memset( tablelist,0,sizeof(tablelist) );
	memset(&sMdm_ac_rel_c,0x00,sizeof(struct mdm_ac_rel_c));

	pub_base_sysinit();

	get_zd_data("0380",f_acno0);
	get_zd_data("0910",f_note_cheq_mst.br_no );
	get_zd_long("0440",&f_note_cheq_msta.sell_date );

	/**��ɲ�ѯ����**/
	pub_base_strpack( f_acno0 );
	if( strlen(f_acno0) )
	{
		if ( pub_base_acno_acid(&f_note_cheq_mst.ac_id,f_acno0) )
		{
		goto ErrExit;
		}
		sprintf( tmpstr," ac_id=%ld and",f_note_cheq_mst.ac_id );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_note_cheq_mst.br_no );
	if( strlen(f_note_cheq_mst.br_no) )
	{
		sprintf( tmpstr," br_no='%s' and",f_note_cheq_mst.br_no );
		strcat( wherelist,tmpstr );
	}
	if( f_note_cheq_msta.sell_date )
	{
		sprintf( tmpstr," sell_date<=%ld and",f_note_cheq_msta.sell_date );
		strcat( wherelist,tmpstr );
	}

	strcpy( tablelist,"note_cheq_mst" );

	/**��ɲ�ѯ**/
	strcat(wherelist," 1=1") ;

	vtcp_log( "WHERE [%s]",wherelist );
	
	ret=Note_cheq_mst_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

	ttlnum=0;

	while(1)
	{
		ret=Note_cheq_mst_Fet_Sel( &vnote_cheq_mst, g_pub_tx.reply );
		if( ret==100 ) break;
		ERR_DEAL

		ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdm_ac_rel_c, "ac_id=%ld and ac_seqn=%d", vnote_cheq_mst.ac_id, vnote_cheq_mst.ac_seq );
		if( ret == 100 )
		{
			sprintf( acErrMsg, "�������˻���ϵ���в����ڴ��ʺ�!" );
	       	WRITEMSG
	       	strcpy( g_pub_tx.reply, "B117" );
	       	goto ErrExit;
		}
		else if( ret )
		{
			sprintf( acErrMsg, "ִ��Mdm_ac_rel_Sel��![%d]", ret );
	       	WRITEMSG
	       	goto ErrExit;
		}

		if( !ttlnum )
		{
			pub_base_AllDwnFilName( filename );
			fp = fopen( filename,"w" );
			if( fp==NULL )
			{
				sprintf(acErrMsg," open file [%s] error ",filename );
				WRITEMSG
				strcpy( g_pub_tx.reply,"S047" );
				goto ErrExit;
			}

			/* ��ʾ�б��� */
			fprintf( fp,"~@��    ��|@ƾ֤����|@ƾ֤����|@֧Ʊ״̬|��������|\n" );
		}

		/* ��������ȡ����ͷ���� */
		ret=Note_parm_Sel( g_pub_tx.reply, &sNoteParm, "note_type='%s'", vnote_cheq_mst.note_type );
		vtcp_log("note_type[%s]",vnote_cheq_mst.note_type);
		ERR_DEAL

		/* ��ȡ����ͷ */
    		strncpy( cBegHead1, vnote_cheq_mst.beg_note_no, sNoteParm.head_len );
    		cBegHead1[sNoteParm.head_len] = '\0';

		/* ������ƾ֤���벻Ϊ��ʱ�ٽ�ȡ����ͷ */
		if( strlen(f_note_cheq_mst.beg_note_no) )
		{
			strncpy( cBegHead, f_note_cheq_mst.beg_note_no, sNoteParm.head_len );
    			cBegHead[sNoteParm.head_len] = '\0';
    			/* �ȽϹ���ͷ */
    			if ( strcmp( cBegHead, cBegHead1 ))
        			continue;
		}
		if( strlen(f_note_cheq_mst.end_note_no) )
		{
			strncpy( cEndHead, f_note_cheq_mst.end_note_no, sNoteParm.head_len );
    			cEndHead[sNoteParm.head_len] = '\0';
    			/* �ȽϹ���ͷ */
    			if ( strcmp( cEndHead, cBegHead1 ))
        			continue;
		}

		if( strlen(f_note_cheq_mst.end_note_no) && 
		strcmp(f_note_no, f_note_cheq_mst.end_note_no ) >0 )
		continue;
		
		fprintf( fp, "%s|%s|%s|%s|%ld|\n",sMdm_ac_rel_c.ac_no,f_note_type2,\
				f_note_no,f_book_sts1,vnote_cheq_mst.sell_date);
		ttlnum++;
		}
	}

	ret=Note_cheq_mst_Clo_Sel( );
	ERR_DEAL

	if( !ttlnum )
	{
		strcpy( g_pub_tx.reply,"S049" );
		goto ErrExit;
	}
	else
	{
		fclose(fp);

		set_zd_data( DC_FILE_SND,"1" );
	}

GoodExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before bad return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
