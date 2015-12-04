/*************************************************
* �� �� ��:  sp8104.c
* ���������� �����˻���ϸ��ѯ
*
* ��    ��:  
* ������ڣ� 
*
* �޸ļ�¼��
* ��   ��:2004.12.12
* �� �� ��:jane
* �޸�����:�Ǳ�������������������ʻ���Ϣ���ܲ�ѯ
*************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define EXTERN
#include "public.h"
#include "td_mst_hst_c.h"
#include "com_branch_c.h"

int sp8104()
{
	struct td_mst_hst_c vtd_mst_hst;
	struct td_mst_hst_c f_td_mst_hst;
	struct td_mst_hst_c f_td_mst_hsta;
	struct mdm_ac_rel_c mdm_ac_rel_c;
	struct mdm_ac_rel_c f_mdm_ac_rel_c;
	struct com_branch_c g_com_branch;
	char ac_no3[25];
	char ac_no4[25];
	char f_note_type11[41];
	char f_acno1[25];
	char add_ind[3];
	char ct_ind[5];
		char filename[100];
		char wherelist[1024];  /**��ѯ����**/
		char tmpstr[512];
		char tmpmo[512];
		char fieldlist[1024];  /**Ҫ��ѯ���ֶ��б�**/
		char tablelist[128];   /**Ҫ��ѯ�ı���**/
		char titstr[1024];
		int ttlnum=0;			/**��ȡ��������**/
		int i=0;
		int ret=0;
		int cnt,tmp_cnt;
		FILE *fp;

	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset( wherelist,0,sizeof(wherelist) );
	memset( fieldlist,0,sizeof(fieldlist) );
	memset( tablelist,0,sizeof(tablelist) );
	memset( ac_no4,0,sizeof(ac_no4) );

	pub_base_sysinit();

	get_zd_data("0910",f_td_mst_hst.opn_br_no );
	get_zd_data("0380",f_acno1);
    pub_base_old_acno(f_acno1);
	get_zd_long("0510",&f_td_mst_hst.ac_seqn );
	get_zd_data("0700",f_td_mst_hst.add_ind );
	get_zd_data("0660",f_td_mst_hst.ct_ind );
	get_zd_long("0290",&f_td_mst_hst.tx_date );
	get_zd_long("0440",&f_td_mst_hsta.tx_date );

	/**��ɲ�ѯ����**/
	pub_base_strpack( f_td_mst_hst.opn_br_no );
	if( strlen(f_td_mst_hst.opn_br_no) )
	{
		sprintf( tmpstr," opn_br_no='%s' and",f_td_mst_hst.opn_br_no );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_acno1 );
	if( strlen(f_acno1) )
	{
			ret=Mdm_ac_rel_Sel( g_pub_tx.reply,&f_mdm_ac_rel_c,
				"ac_no='%s' ",f_acno1 );
			if( ret==100 )
			{
				if ( pub_base_acno_acid(&f_td_mst_hst.ac_id,f_acno1) )
					goto ErrExit;
			}
			else if( ret ) goto ErrExit;
			else
			{
				strcpy( ac_no4,f_acno1 );
				f_td_mst_hst.ac_id=f_mdm_ac_rel_c.ac_id;
			}
		sprintf( tmpstr," ac_id=%ld and",f_td_mst_hst.ac_id );
		strcat( wherelist,tmpstr );
	}
	if( f_td_mst_hst.ac_seqn )
	{
		sprintf( tmpstr," ac_seqn=%d and",f_td_mst_hst.ac_seqn );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_td_mst_hst.add_ind );
	if( strlen(f_td_mst_hst.add_ind) )
	{
		sprintf( tmpstr," add_ind='%s' and",f_td_mst_hst.add_ind );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_td_mst_hst.ct_ind );
	if( strlen(f_td_mst_hst.ct_ind) )
	{
		sprintf( tmpstr," ct_ind='%s' and",f_td_mst_hst.ct_ind );
		strcat( wherelist,tmpstr );
	}
	if( f_td_mst_hst.tx_date )
	{
		sprintf( tmpstr," tx_date>=%ld and",f_td_mst_hst.tx_date );
		strcat( wherelist,tmpstr );
	}
	if( f_td_mst_hsta.tx_date )
	{
		sprintf( tmpstr," tx_date<=%ld and",f_td_mst_hsta.tx_date );
		strcat( wherelist,tmpstr );
	}

	strcpy( tablelist,"td_mst_hst" );

	/**��ɲ�ѯ**/
	strcat(wherelist," 1=1 ORDER BY tx_date,trace_no,hst_cnt,tx_time,ac_id,ac_seqn ") ;

	vtcp_log( "WHERE [%s]",wherelist );
	strcpy( fieldlist,"trace_no,trace_cnt,opn_br_no,ac_id,ac_seqn,ct_ind,add_ind,tx_amt,bal,tx_date,tx_time,note_type,note_no,hst_cnt,brf,tel,chk,auth");

	ret=Td_mst_hst_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

		ttlnum=0;

	TRACE
	while(1)
	{
		ret=Td_mst_hst_Fet_Sel( &vtd_mst_hst, g_pub_tx.reply );
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
			fprintf( fp,"~��      ��|���|��������|����ʱ��|@ժ    Ҫ|$���׽��|$��      ��|@��ת|@����|@ƾ֤����|@ƾ֤��|@������|��ϸ����|@����Ա|@����Ա|@��Ȩ��|��ˮ��|�ʴ�|\n" );
		}

		/*pub_pubdb_prt_fld( fp,tablelist,fieldlist ); */

		if(vtd_mst_hst.trace_no<-2147000000) vtd_mst_hst.trace_no=0;
		if(vtd_mst_hst.trace_cnt<-2147000000) vtd_mst_hst.trace_cnt=0;

		if( strlen(ac_no4) ) strcpy( ac_no3,ac_no4 );
		else
			pub_base_acid_acno( vtd_mst_hst.ac_id,vtd_mst_hst.ac_seqn,ac_no3 );

		if(vtd_mst_hst.tx_amt<-2147000000) vtd_mst_hst.tx_amt=0.0;
		if(vtd_mst_hst.bal<-2147000000) vtd_mst_hst.bal=0.0;
		if(vtd_mst_hst.tx_date<-2147000000) vtd_mst_hst.tx_date=0;
		if(vtd_mst_hst.tx_time<-2147000000) vtd_mst_hst.tx_time=0;
		pub_base_dic_show_str(f_note_type11,"note_type",vtd_mst_hst.note_type);
		if(vtd_mst_hst.hst_cnt<-2147000000) vtd_mst_hst.hst_cnt=0;

		pub_base_dic_show_str(add_ind,"add_ind",vtd_mst_hst.add_ind);
		if(vtd_mst_hst.add_ind[0]=='0')		 /*0-��*/
		{
			strcpy(add_ind,"��");
		}
		else if(vtd_mst_hst.add_ind[0]=='1') /*1-��*/
		{
			strcpy(add_ind,"��");
		}

		pub_base_dic_show_str(ct_ind,"ct_ind",vtd_mst_hst.ct_ind);
		
		/*-----------------------------------update by jane begin-------------------------------*/
		cnt=tmp_cnt=0;
    if ((!strcmp(g_pub_tx.tx_br_no,"61000"))||(!strcmp(g_pub_tx.tx_br_no,"99999"))||(!strcmp(g_pub_tx.tx_br_no,"61099"))||(!strcmp(g_pub_tx.tx_br_no,"61056"))||(!strcmp(g_pub_tx.tx_br_no,"61058")))
		 fprintf( fp, "%s|%d|%ld|%06ld|%s|%.2lf|%.2lf|%s|%s|%s|%s|%s|%ld|%s|%s|%s|%ld|%ld|\n",
		         ac_no3,vtd_mst_hst.ac_seqn,vtd_mst_hst.tx_date,vtd_mst_hst.tx_time,
		         vtd_mst_hst.brf,vtd_mst_hst.tx_amt,vtd_mst_hst.bal,ct_ind,add_ind,
		         f_note_type11,vtd_mst_hst.note_no,vtd_mst_hst.opn_br_no,
		         vtd_mst_hst.hst_cnt,vtd_mst_hst.tel,vtd_mst_hst.chk,vtd_mst_hst.auth,
		         vtd_mst_hst.trace_no,vtd_mst_hst.trace_cnt);
		else 
			{
			 ret=Com_branch_Dec_Sel(g_pub_tx.reply ,"up_br_no='%s'",g_pub_tx.tx_br_no);
       if(ret)
       {
        sprintf(acErrMsg,"[DECLARE CURSOR]�쳣!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"D101");
        goto ErrExit;
       }
       
       while(1)
       {
        memset( &g_com_branch,0x00,sizeof(struct com_branch_c) );
        ret=Com_branch_Fet_Sel(&g_com_branch,g_pub_tx.reply);
        if (ret==100)
           break;
        else ERR_DEAL
        cnt++;
       
        if(strcmp(g_com_branch.br_no,vtd_mst_hst.opn_br_no))
        tmp_cnt++;
			 }
			 
			 if((cnt!=tmp_cnt)||(!strcmp(g_pub_tx.tx_br_no,vtd_mst_hst.opn_br_no)))
			 	fprintf( fp, "%s|%d|%ld|%06ld|%s|%.2lf|%.2lf|%s|%s|%s|%s|%s|%ld|%s|%s|%s|%ld|%ld|\n",
		         ac_no3,vtd_mst_hst.ac_seqn,vtd_mst_hst.tx_date,vtd_mst_hst.tx_time,
		         vtd_mst_hst.brf,vtd_mst_hst.tx_amt,vtd_mst_hst.bal,ct_ind,add_ind,
		         f_note_type11,vtd_mst_hst.note_no,vtd_mst_hst.opn_br_no,
		         vtd_mst_hst.hst_cnt,vtd_mst_hst.tel,vtd_mst_hst.chk,vtd_mst_hst.auth,
		         vtd_mst_hst.trace_no,vtd_mst_hst.trace_cnt);
		}
	  /*-----------------------------------update by jane end-------------------------------*/		
		ttlnum++;

		if( !strncmp(vtd_mst_hst.brf,"����",4) )
		{
			if( f_mdm_ac_rel_c.note_sts[0]=='4' )
				break;
		}
TRACE
	}

TRACE

	ret=Td_mst_hst_Clo_Sel( );
	ERR_DEAL

TRACE
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
