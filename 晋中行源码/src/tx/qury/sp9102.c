/*************************************************
* �� �� ��:  sp9102.c
* ���������� �����˻���ϸ��ѯ(���ڽ��е绰���в鿨������ϸ)
*
* ��    ��:  
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
#include "dd_mst_hst_c.h"

int sp9102()
{
	struct dd_mst_hst_c vdd_mst_hst;
	struct dd_mst_hst_c f_dd_mst_hst;
	struct dd_mst_hst_c f_dd_mst_hsta;
	
	char add_ind[3];
	char ct_ind[5];
	char ac_no3[25];
	char f_note_type9[41];
	char f_acno0[25];
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
		double j_tx_amt=0.00;
		double d_tx_amt=0.00;
		FILE *fp;
		char tmp_date[9];
		char tmp_time[7];

	memset(tmp_date,0x00,sizeof(tmp_date));
	memset(tmp_time,0x00,sizeof(tmp_time));
	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset( wherelist,0,sizeof(wherelist) );
	memset( fieldlist,0,sizeof(fieldlist) );
	memset( tablelist,0,sizeof(tablelist) );
	memset( &vdd_mst_hst,0,sizeof(vdd_mst_hst));
	memset( &f_dd_mst_hst,0,sizeof(f_dd_mst_hst));
	memset( &f_dd_mst_hsta,0,sizeof(f_dd_mst_hsta));
    
    vtcp_log( "0700===f_dd_mst_hst.add_ind [%s]",f_dd_mst_hst.add_ind );
	pub_base_sysinit();
    get_zd_data("0700",f_dd_mst_hst.add_ind );
    vtcp_log( "0700===f_dd_mst_hst.add_ind [%s]",f_dd_mst_hst.add_ind );
	get_zd_data("0380",f_acno0);
    pub_base_old_acno(f_acno0);
    vtcp_log( "0700===f_dd_mst_hst.add_ind [%s]",f_dd_mst_hst.add_ind );
	get_zd_long("0510",&f_dd_mst_hst.ac_seqn );
	get_zd_data("0910",f_dd_mst_hst.opn_br_no );
	get_zd_data("0700",f_dd_mst_hst.add_ind );
	get_zd_data("0660",f_dd_mst_hst.ct_ind );
	get_zd_long("0290",&f_dd_mst_hst.tx_date );
	get_zd_long("0440",&f_dd_mst_hsta.tx_date );

   vtcp_log( "0700===f_dd_mst_hst.add_ind [%s]",f_dd_mst_hst.add_ind );
   /**������뽻��-wangwk-20101229**/
   ret=iCheckPasswd();
   if(ret)
   	{
   		goto ErrExit;
   	}
   /**��ӽ���**/
	/**��ɲ�ѯ����**/
	pub_base_strpack( f_acno0 );
	if( strlen(f_acno0) )
	{
		if ( pub_base_acno_acid(&f_dd_mst_hst.ac_id,f_acno0) )
		{
		goto ErrExit;
		}
		sprintf( tmpstr," ac_id=%ld and",f_dd_mst_hst.ac_id );
		strcat( wherelist,tmpstr );
	}
	if( f_dd_mst_hst.ac_seqn )
	{
		sprintf( tmpstr," ac_seqn=%d and",f_dd_mst_hst.ac_seqn );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_dd_mst_hst.opn_br_no );
/*
	if( strlen(f_dd_mst_hst.opn_br_no) )
	{
		if(memcmp(f_dd_mst_hst.opn_br_no, QS_BR_NO, 5) == 0||memcmp(f_dd_mst_hst.opn_br_no, KJ_BR_NO, 5) == 0)
		{
		}
		else
		{
		sprintf( tmpstr," opn_br_no='%s' and",f_dd_mst_hst.opn_br_no );
		strcat( wherelist,tmpstr );
		}
	}
*/
	pub_base_strpack( f_dd_mst_hst.add_ind );
	if( strlen(f_dd_mst_hst.add_ind) )
	{
		/*if( f_dd_mst_hst.add_ind[0]=='0' ) f_dd_mst_hst.add_ind[0]='1';
		else f_dd_mst_hst.add_ind[0]='0';)*/
		if( f_dd_mst_hst.add_ind[0]=='0' || f_dd_mst_hst.add_ind[0]=='1' ){
    		vtcp_log( "f_dd_mst_hst.add_ind [%s]",f_dd_mst_hst.add_ind );
    		sprintf( tmpstr," add_ind='%s' and",f_dd_mst_hst.add_ind );
    		strcat( wherelist,tmpstr );
	    }
	}
	pub_base_strpack( f_dd_mst_hst.ct_ind );
	if( strlen(f_dd_mst_hst.ct_ind) )
	{
		sprintf( tmpstr," ct_ind='%s' and",f_dd_mst_hst.ct_ind );
		strcat( wherelist,tmpstr );
	}
	if( f_dd_mst_hst.tx_date )
	{
		sprintf( tmpstr," tx_date>=%ld and",f_dd_mst_hst.tx_date );
		strcat( wherelist,tmpstr );
	}
	if( f_dd_mst_hsta.tx_date )
	{
		sprintf( tmpstr," tx_date<=%ld and",f_dd_mst_hsta.tx_date );
		strcat( wherelist,tmpstr );
	}

	strcpy( tablelist,"dd_mst_hst" );

	/**��ɲ�ѯ
	strcat(wherelist," 1=1 ORDER BY tx_date,hst_cnt") ;
	strcat(wherelist," 1=1 ORDER BY tx_date,trace_no,trace_cnt");**/
	strcat(wherelist," 1=1 ORDER BY tx_date desc,trace_no desc,trace_cnt,tx_time desc");/*modfiy by ChengGX 20100111*/

	vtcp_log( "WHERE [%s]",wherelist );
	strcpy( fieldlist,"trace_no,trace_cnt,opn_br_no,ac_id,ac_seqn,add_ind,ct_ind,tx_amt,bal,note_type,note_no,tx_date,tx_time,brf,hst_cnt,tel,chk,auth");

	ret=Dd_mst_hst_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL
  
  ttlnum=0;
  
	while(1)
	{
		ret=Dd_mst_hst_Fet_Sel( &vdd_mst_hst, g_pub_tx.reply );
		if( ret==100 ) break;
		ERR_DEAL

		if( !ttlnum )
		{
			pub_base_AllDwnFilName( filename );
			vtcp_log( "%s,%d,FILENAME [%s]",__FILE__,__LINE__,filename );
			fp = fopen( filename,"w" );
			if( fp==NULL )
			{
				sprintf(acErrMsg," open file [%s] error ",filename );
				WRITEMSG
				strcpy( g_pub_tx.reply,"S047" );
				goto ErrExit;
			}

			/**��ʾ�б���**/
		/*	fprintf( fp,"~@��������|@����ʱ��|@ժ    Ҫ|$���׽��|@���|@��ת|$�������|@ƾ֤����|@ƾ֤����|@����������|��ϸ����|@����Ա|@����Ա|@��Ȩ��|��ˮ��|�ʴ�|@�ʺ�|���|\n" );*/
  		fprintf( fp,"~@��������|@���|$���׽��|@��ת|$�������|@ժҪ|@ƾ֤����|@ƾ֤����|@����Ա|����ʱ��|���|\n" );
		}

		/*pub_pubdb_prt_fld( fp,tablelist,fieldlist );*/

		if(vdd_mst_hst.trace_no<-2147000000) vdd_mst_hst.trace_no=0;
		if(vdd_mst_hst.trace_cnt<-2147000000) vdd_mst_hst.trace_cnt=0;
		pub_base_acid_acno( vdd_mst_hst.ac_id,vdd_mst_hst.ac_seqn,ac_no3 );
		if(vdd_mst_hst.tx_amt<-2147000000) vdd_mst_hst.tx_amt=0.0;
		if(vdd_mst_hst.bal<-2147000000) vdd_mst_hst.bal=0.0;
		pub_base_dic_show_str(f_note_type9,"note_type",vdd_mst_hst.note_type);
		if(vdd_mst_hst.tx_date<-2147000000) vdd_mst_hst.tx_date=0;
		if(vdd_mst_hst.tx_time<-2147000000) vdd_mst_hst.tx_time=0;
		if(vdd_mst_hst.hst_cnt<-2147000000) vdd_mst_hst.hst_cnt=0;

		if(vdd_mst_hst.add_ind[0]=='0')		 
		{
			strcpy(add_ind,"��");
		 j_tx_amt=j_tx_amt+vdd_mst_hst.tx_amt;
	}
		
		else if(vdd_mst_hst.add_ind[0]=='1') 
		{
			strcpy(add_ind,"��");
     d_tx_amt=d_tx_amt+vdd_mst_hst.tx_amt;
  }
		pub_base_dic_show_str(ct_ind,"ct_ind",vdd_mst_hst.ct_ind);
	sprintf(tmp_date,"%ld",vdd_mst_hst.tx_date);
	sprintf(tmp_time,"%d",vdd_mst_hst.tx_time);

   	/*fprintf(fp, "%s|%s|%s|%.2lf|%s|%s|%.2lf|%s|%s|%s|%ld|%s|%s|%s|%ld|%ld|%s|%d|\n",tmp_date,tmp_time,vdd_mst_hst.brf,vdd_mst_hst.tx_amt,add_ind,ct_ind,vdd_mst_hst.bal,f_note_type9,vdd_mst_hst.note_no,vdd_mst_hst.opn_br_no,vdd_mst_hst.hst_cnt,vdd_mst_hst.tel,vdd_mst_hst.chk,vdd_mst_hst.auth,vdd_mst_hst.trace_no,vdd_mst_hst.trace_cnt,ac_no3,vdd_mst_hst.ac_seqn);*/
			fprintf(fp, "%s|%s|%.2lf|%s|%.2lf|%s|%s|%s|%s|%d|%d|\n",tmp_date,add_ind,vdd_mst_hst.tx_amt,ct_ind,vdd_mst_hst.bal,vdd_mst_hst.brf,f_note_type9,vdd_mst_hst.note_no,vdd_mst_hst.tel,vdd_mst_hst.tx_time,vdd_mst_hst.ac_seqn);
			
		/*pub_base_dic_show_str(add_ind,"add_ind",vdd_mst_hst.add_ind);*/
    
		
		ttlnum++;
   
	}
  
	
	ret=Dd_mst_hst_Clo_Sel( );
	ERR_DEAL

	if( !ttlnum )
	{
		strcpy( g_pub_tx.reply,"S049" );
		goto ErrExit;
	}
	else
	{
	 /* fprintf( fp,"�跽������ϼ�:");*/
	  fprintf( fp,"�跽�ϼ�:|");
	  fprintf( fp,"%.2lf||",j_tx_amt);
	/*  fprintf( fp,"����������ϼ�:");*/
	  fprintf( fp,"�����ϼ�:|");
    fprintf( fp,"%.2lf|\n",d_tx_amt);
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
int iCheckPasswd()
{
	int ret = 0;
	struct mdm_ac_rel_c st_mdm_ac_rel;
	memset(&st_mdm_ac_rel, 0, sizeof(st_mdm_ac_rel));
	get_zd_data("0380",st_mdm_ac_rel.ac_no );		/*���Ӻ���*/
	vtcp_log( "%s,%d===���Ӻ���[%s]",__FILE__,__LINE__,st_mdm_ac_rel.ac_no);
	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&st_mdm_ac_rel,"ac_no='%s'", st_mdm_ac_rel.ac_no);
	if(ret==100)
	{
		sprintf(acErrMsg,"��ƥ��ļ�¼!! ");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O043");
		return -1;
	}
	get_zd_data("0790",g_pub_tx.draw_pwd);
	/*vtcp_log( "%s,%d===g_pub_tx.draw_pwd[%s]",__FILE__,__LINE__,g_pub_tx.draw_pwd);*/
	get_zd_data("0670",g_pub_tx.draw_pwd_yn);
	vtcp_log( "%s,%d===g_pub_tx.draw_pwd_yn[%s]",__FILE__,__LINE__,g_pub_tx.draw_pwd_yn);
	pub_base_EnDes(g_pub_tx.tx_date,"",g_pub_tx.draw_pwd);
	if(g_pub_tx.draw_pwd_yn[0] == 'Y' && 
		 strcmp(st_mdm_ac_rel.draw_pwd, g_pub_tx.draw_pwd) != 0)
		{
          sprintf(acErrMsg, "�������!!");
          WRITEMSG
          strcpy(g_pub_tx.reply, "O123");
          return -1;
    }
   return 0;
}
