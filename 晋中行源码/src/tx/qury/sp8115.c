/*************************************************
* �� �� ��:  sp8115.c
* ���������� �˺Ž�����Ϣ��ѯ
*
* ��    ��:  lance
* ������ڣ� 2003-06-15
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
#include "mdm_attr_c.h"

int sp8115()
{
	struct mdm_ac_rel_c vmdm_ac_rel;
	struct mdm_ac_rel_c f_mdm_ac_rel;
	char f_main_ind2[41];
	char f_note_sts4[41];
	char f_coll_sts5[41];
	char ac_no7[25];
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
		FILE *fp;
	struct mdm_attr_c	mdm_attr_tmp;
	
	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset( wherelist,0,sizeof(wherelist) );
	memset( fieldlist,0,sizeof(fieldlist) );
	memset( tablelist,0,sizeof(tablelist) );
	memset( &mdm_attr_tmp,0,sizeof(struct mdm_attr_c));
	
	pub_base_sysinit();

	get_zd_data("0370",f_mdm_ac_rel.ac_no );
    pub_base_old_acno(f_mdm_ac_rel.ac_no);
	
	/* ȡac_id */
	ret = Mdm_ac_rel_Sel( g_pub_tx.reply,&g_mdm_ac_rel,
			" ac_no='%s' ",f_mdm_ac_rel.ac_no );
	if(ret && ret!=100)
	{
 		sprintf(acErrMsg,"��ѯ�����˺Ŷ��ձ�ʱ�쳣����!!");
 		WRITEMSG
 		goto ErrExit;
 	}else if( ret==100 ){
  		sprintf(acErrMsg,"�˺Ų�����,����!!");
 		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
 		goto ErrExit;
 	}
 		
	/**��ɲ�ѯ����**/
	
	sprintf( tmpstr," ac_id=%ld and",g_mdm_ac_rel.ac_id );
	strcat( wherelist,tmpstr );

	strcpy( tablelist,"mdm_ac_rel" );
	
				  
	/**��ɲ�ѯ**/
	strcat(wherelist," 1=1") ;

	vtcp_log( "WHERE [%s]",wherelist );
	strcpy( fieldlist,"ac_no,mdm_code,main_ind,note_no,note_sts,coll_sts,opn_br_no,ac_id,ac_seqn,beg_date,end_date");

	ret=Mdm_ac_rel_Dec_Sel1( g_pub_tx.reply,wherelist );
	if(ret)
	{
		TRACE
		
		sprintf(acErrMsg,"Mdm_ac_rel_Dec_Sel1ʧ��!!");
		WRITEMSG
		goto ErrExit;
	}

	ttlnum=0;

	while(1)
	{
		
		TRACE
		ret=Mdm_ac_rel_Fet_Sel1( &vmdm_ac_rel, g_pub_tx.reply );
		if( ret && ret!=100 )
		{
 			sprintf(acErrMsg,"��ѯ�����˺Ŷ��ձ�ʱ�쳣����!!");
 			WRITEMSG
 			goto ErrExit;
 		}else if( ret==100 ) break;
		TRACE
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
			fprintf( fp,"~@    ��/����|@��������|@��־|@ƾ֤����|@����״̬|@��ʰ״̬|@������|��������|ֹ������|\n" );
		}

		/*pub_pubdb_prt_fld( fp,tablelist,fieldlist ); */
		/* ��mdm_attr���в�ѯ�������� */
		g_reply_int = Mdm_attr_Sel(g_pub_tx.reply,&mdm_attr_tmp,"mdm_code='%s'",vmdm_ac_rel.mdm_code);
		if (g_reply_int)	goto ErrExit;
		
		pub_base_dic_show_str(f_main_ind2,"main_ind",vmdm_ac_rel.main_ind);
		pub_base_dic_show_str(f_note_sts4,"mdm_sts",vmdm_ac_rel.note_sts);
		pub_base_dic_show_str(f_coll_sts5,"coll_sts",vmdm_ac_rel.coll_sts);
		if(vmdm_ac_rel.beg_date<-2147000000) vmdm_ac_rel.beg_date=0;
		if(vmdm_ac_rel.end_date<-2147000000) vmdm_ac_rel.end_date=0;
		if(mdm_attr_tmp.mdm_type[0]=='1')
		{
			strcpy( f_main_ind2 , "����" );
		}else if(mdm_attr_tmp.mdm_type[0]=='2'){
			strcpy( f_main_ind2 , "�浥" );
		}else if(mdm_attr_tmp.mdm_type[0]=='6'){
			strcpy( f_main_ind2 , "�޵���" );
		}
		fprintf( fp, "%s|%s|%s|%s|%s|%s|%s|%ld|%ld|\n",vmdm_ac_rel.ac_no,mdm_attr_tmp.mdm_name,f_main_ind2,vmdm_ac_rel.note_no,f_note_sts4,f_coll_sts5,vmdm_ac_rel.opn_br_no,vmdm_ac_rel.beg_date,vmdm_ac_rel.end_date);	
		ttlnum++;
	}

	ret=Mdm_ac_rel_Clo_Sel1( );
	ERR_DEAL

	if( !ttlnum )
	{
		strcpy( g_pub_tx.reply,"S049" );
		goto ErrExit;
	}else{
		fclose(fp);

		set_zd_data( DC_FILE_SND,"1" );
		set_zd_long("0460",vmdm_ac_rel.end_date); /*���ظ�3202.XML���ײ��̫�����Ĺ������� 20151119*/
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
