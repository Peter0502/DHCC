/*************************************************
* �� �� ��:  sp8303.c
* ���������� ƾ֤��ʷ��ѯ
*
* ��    ��:  jane
* ������ڣ� 
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
#include "note_mst_hst_c.h"
#include "com_tel_c.h"
#include "com_tx_br_rel_c.h"

int sp8303()
{
	struct note_mst_hst_c vnote_mst_hst;
	struct note_mst_hst_c f_note_mst_hst;
	struct com_tel_c  sComTel;		/* ��Ա��Ϣ�� */
	struct com_tx_br_rel_c	sComTxBrRel;	/* ���׻�����ϵ�� */
	char f_note_type4[41];
	char f_sts_ind13[41];
		char filename[100];
		/*char wherelist[1024];  **��ѯ����**/		
		char wherelist[3072];
		char tmpstr[521];
		char tmpmo[512];
		char fieldlist[1024];  /**Ҫ��ѯ���ֶ��б�**/
		char titstr[1024];
		int ttlnum=0;			/**��ȡ��������**/
		int i=0;
		int ret=0;
		FILE *fp;
	int tmpflag = 0;

	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset( wherelist,0,sizeof(wherelist) );
	memset( fieldlist,0,sizeof(fieldlist) );
	
	memset( tmpstr, 0x0, sizeof(tmpstr) );
	memset( &f_note_mst_hst, 0x0, sizeof(f_note_mst_hst) );

	pub_base_sysinit();

	get_zd_data("0910",f_note_mst_hst.tel );
	get_zd_data("0890",f_note_mst_hst.note_type );
	get_zd_data("0640",f_note_mst_hst.br_no );
	get_zd_data("0590",f_note_mst_hst.beg_note_no );
	get_zd_data("0600",f_note_mst_hst.end_note_no );
	get_zd_long("0440",&f_note_mst_hst.tx_date );

	/**��ɲ�ѯ����**/
	pub_base_strpack( f_note_mst_hst.tel );
	if( strlen(f_note_mst_hst.tel) )
	{
		/* ��������Ա�뽻�׹�Ա֮��Ĺ�ϵ */
		ret = pub_base_GetTelInfo( f_note_mst_hst.tel, &sComTel );
		if ( ret )
		{
			sprintf( acErrMsg, "���ҹ�Ա��Ϣ����,ret=[%d]", ret );
			WRITEMSG
			goto ErrExit;	
		}

                pub_base_strpack(sComTel.br_no);
                pub_base_strpack(g_pub_tx.tx_br_no);
		if ( strcmp( g_pub_tx.tx_br_no, sComTel.br_no ) != 0 )
		{
			/* �����������Ƿ�Ϊ���׻������¼� */
			ret = pub_com_ChkBrRel( 2, sComTel.br_no, g_pub_tx.tx_br_no );
			if ( ret )
			{
				sprintf( acErrMsg, "������֮��Ĺ�ϵ��" );
				WRITEMSG
				goto ErrExit;
			}	
		}
		sprintf( tmpstr," tel='%s' and",f_note_mst_hst.tel );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_note_mst_hst.note_type );
	if( strlen(f_note_mst_hst.note_type) )
	{
		sprintf( tmpstr," note_type='%s' and",f_note_mst_hst.note_type );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_note_mst_hst.br_no );
	if( strlen(f_note_mst_hst.br_no) )
	{
		/* �����������뽻�׻����Ĺ�ϵ */
		if ( strcmp( g_pub_tx.tx_br_no, f_note_mst_hst.br_no ) != 0 )
		{
			/* �����������Ƿ�Ϊ���׻������¼� */
			ret = pub_com_ChkBrRel( 2, f_note_mst_hst.br_no, g_pub_tx.tx_br_no );
			if ( ret )
			{
				sprintf( acErrMsg, "������֮��Ĺ�ϵ��" );
				WRITEMSG
				goto ErrExit;
			}	
		}
		sprintf( tmpstr," br_no='%s' and",f_note_mst_hst.br_no );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_note_mst_hst.beg_note_no );
	if( strlen(f_note_mst_hst.beg_note_no) )
	{
		sprintf( tmpstr," end_note_no>='%s' and",f_note_mst_hst.beg_note_no );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_note_mst_hst.end_note_no );
	if( strlen(f_note_mst_hst.end_note_no) )
	{
		sprintf( tmpstr," beg_note_no<='%s' and",f_note_mst_hst.end_note_no );
		strcat( wherelist,tmpstr );
	}
	if( f_note_mst_hst.tx_date )
	{
		sprintf( tmpstr," tx_date=%ld and",f_note_mst_hst.tx_date );
		strcat( wherelist,tmpstr );
	}
	
	/* �����Ա�źͻ����������Ϊ�� */
	if( !strlen(f_note_mst_hst.tel) && !strlen(f_note_mst_hst.br_no) )
	{
		sprintf( tmpstr," br_no in(select br_no from com_tx_br_rel where rel_type='2' and up_br_no='%s') and ",g_pub_tx.tx_br_no);
		strcat( wherelist,tmpstr );
	}

	/**��ɲ�ѯ**/
	strcat(wherelist," 1=1 order by beg_note_no") ;

	vtcp_log( "----->WHERE [%s]",wherelist );
	strcpy( fieldlist,"trace_no,trace_cnt,tel,br_no,note_type,beg_note_no,end_note_no,note_cnt,tx_date,tx_time,auth,tw_br_no,tw_tel,sts_ind");

	ret=Note_mst_hst_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

		ttlnum=0;

	while(1)
	{
		memset( &vnote_mst_hst, 0x0, sizeof(vnote_mst_hst) );
		memset(f_note_type4,0x00,sizeof(f_note_type4));
		ret=Note_mst_hst_Fet_Sel( &vnote_mst_hst, g_pub_tx.reply );
		if( ret==100 ) break;
		ERR_DEAL

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

			/**��ʾ�б���**/
			fprintf( fp,"~��ˮ��|@����Ա|@�����|@ƾ֤����|@ƾ֤����|@��ʼ����|@��ֹ����|ƾ֤����|�������|����ʱ��|@��Ȩ��|@�Է�����|@�Է�����Ա|@������|\n" );
		}

		if(vnote_mst_hst.trace_no<-2147000000) vnote_mst_hst.trace_no=0;
		if(vnote_mst_hst.trace_cnt<-2147000000) vnote_mst_hst.trace_cnt=0;
		pub_base_dic_show_str(f_note_type4,"note_type",vnote_mst_hst.note_type);
		if(vnote_mst_hst.note_cnt<-2147000000) vnote_mst_hst.note_cnt=0;
		if(vnote_mst_hst.tx_date<-2147000000) vnote_mst_hst.tx_date=0;
		if(vnote_mst_hst.tx_time<-2147000000) vnote_mst_hst.tx_time=0;
		pub_base_dic_show_str(f_sts_ind13,"note_act",vnote_mst_hst.sts_ind);

		fprintf( fp, "%ld|%s|%s|%s|%s|%s|%s|%ld|%ld|%ld|%s|%s|%s|%s|\n",
			vnote_mst_hst.trace_no,vnote_mst_hst.tel,vnote_mst_hst.br_no,vnote_mst_hst.note_type,f_note_type4,vnote_mst_hst.beg_note_no,vnote_mst_hst.end_note_no,vnote_mst_hst.note_cnt,vnote_mst_hst.tx_date,vnote_mst_hst.tx_time,vnote_mst_hst.auth,vnote_mst_hst.tw_br_no,vnote_mst_hst.tw_tel,f_sts_ind13);
		ttlnum++;
	}

	ret=Note_mst_hst_Clo_Sel( );
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
