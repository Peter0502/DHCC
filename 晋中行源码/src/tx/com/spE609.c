/*************************************************
* �� �� ��:  spE609.c
* ���������� ��������(ƾ֤)
*
* ��    ��:  andy
* ������ڣ� 2004-07-06
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
#include "com_tel_c.h"
#include "note_mst_c.h"
#include "note_mst_hst_c.h"


int spE609()
{
		char filename[100];
		char wherelist[1024];  /**��ѯ����**/
		char tmpstr[512];
		
		char tel[5];
		char old_note_type[4];
		char note_name[21];
		int ttlnum=0;			/**��ȡ��������**/
		int i=0;
		int ret=0;

		long in_cnt=0,out_cnt=0;
		long ye_cnt=0,now_cnt=0;
							
		FILE *fp;

	struct com_tel_c	v_com_tel;
	struct note_mst_hst_c	v_note_mst_hst;
		
	memset( &v_com_tel , 0x00 , sizeof(struct com_tel_c));
	memset( &v_note_mst_hst , 0x00 , sizeof(struct note_mst_hst_c));
		
	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset( wherelist,0,sizeof(wherelist) );

	strcpy( old_note_type , "***" );
	
	pub_base_sysinit();

	get_zd_data("0910",tel );

	/**��ɲ�ѯ����**/
	pub_base_strpack( tel );
	if( strlen(tel) )
	{
		/* ����Ա���Ƿ���� */
		ret = Com_tel_Sel(g_pub_tx.reply,&v_com_tel,"tel='%s'",tel);
		if(ret==100)
		{
			sprintf(acErrMsg,"�˹�Ա�Ų�����![%s]",tel);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"O120" );			
			goto ErrExit;
		}else if(ret){
			sprintf(acErrMsg,"��ȡ��Ա��Ϣ���쳣����!");
			WRITEMSG
			strcpy( g_pub_tx.reply ,"O221" );			
			goto ErrExit;
		}									
		sprintf( tmpstr," tel='%s' and",tel );
		strcat( wherelist,tmpstr );
	}
	
	/* �������� */
	sprintf( tmpstr," br_no='%s' and",g_pub_tx.tx_br_no );
	strcat( wherelist,tmpstr );

	/* �������� */
	sprintf( tmpstr," tx_date=%ld and",g_pub_tx.tx_date );
	strcat( wherelist,tmpstr );
	strcat( wherelist," (((sts_ind='2' or sts_ind='3') and br_no!=tw_br_no) or (sts_ind !='2' and sts_ind!='3')) and " );/*jinyw addΪ�˲�ͳ��ƾ֤��Ա����*/
		
	/**��ɲ�ѯ**/
	strcat(wherelist," 1=1  ORDER BY note_type ") ;

	vtcp_log( "WHERE [%s]",wherelist );

	ret=Note_mst_hst_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

		ttlnum=0;
		
	while(1)
	{
		ret=Note_mst_hst_Fet_Sel( &v_note_mst_hst, g_pub_tx.reply );
		if( ret==100 ) 
		{
			if( ttlnum )
			{
				/* ���ƾ֤���� */					
				ret = pub_base_get_note_name( old_note_type, note_name );	
				ERR_DEAL
				sprintf(acErrMsg,"note_name=%s",note_name);
				WRITEMSG
				
				pub_base_strpack( note_name );	
		
				/* ͳ�Ƹ���ƾ֤���������������� */
				pub_base_strpack( tel );
				if( strlen(tel) )	/* ��Ա�ŷǿ� */
				{
					ret = sql_sum_long( "note_mst" , "ys_cnt" , &ye_cnt , 
                          "br_no='%s' and note_type='%s' and sts in( 0 , 1 ) and tel='%s'" , 
                          g_pub_tx.tx_br_no,old_note_type,tel);
					ERR_DEAL					
					ret = sql_sum_long( "note_mst" , "cnt" , &now_cnt , 
                          "br_no='%s' and note_type='%s' and sts in( 0 , 1 ) and tel='%s'" , 
                          g_pub_tx.tx_br_no,old_note_type,tel);
					ERR_DEAL							
				}else{			/* ��Ա��Ϊ�� */
					ret = sql_sum_long( "note_mst" , "ys_cnt" , &ye_cnt , 
					      "br_no='%s' and note_type='%s' and sts in( 0 , 1 ) " , 
                          g_pub_tx.tx_br_no,old_note_type);
					ERR_DEAL					
					ret = sql_sum_long( "note_mst" , "cnt" , &now_cnt , 
                          "br_no='%s' and note_type='%s' and sts in( 0 , 1 ) " , 
                          g_pub_tx.tx_br_no,old_note_type);
					ERR_DEAL
                    vtcp_log(">>>>>>>>>>>>>>>>>>>>>>�������:[%d]",now_cnt);
				}
			
				/**��ʾ�б���**/	
				fprintf( fp,"@3v1\n");
				fprintf( fp,"`           ƾ֤����: ��%3s��\n",old_note_type);
				fprintf( fp,"`           ƾ֤����: ��%s��\n",note_name);
				fprintf( fp,"`           �������������ө����������ө����������ө�����������\n" );
				fprintf( fp,"`           �� ������Ŀ �� �������� �� ���ո��� �� ����ʣ�� ��\n" );			
				fprintf( fp,"`           �������������㩥���������㩥���������㩥����������\n" );			
				fprintf( fp,"`           ��%10ld��%10ld��%10ld��%10ld��\n" ,ye_cnt,in_cnt,out_cnt,now_cnt);			
				fprintf( fp,"`           �������������۩����������۩����������۩�����������\n" );	
				fprintf( fp,"`\n" );					
				fprintf( fp,"`\n" );
								
				/* �������� */
				in_cnt=0;
				out_cnt=0;
				ye_cnt=0;
				now_cnt=0;		
			}
			
			break;		
		}else if(ret){
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
		}
		TRACE
		
		/* ��һ����¼ */
		if( strcmp( old_note_type , "***")==0 )
		{
			TRACE
			strcpy( old_note_type, v_note_mst_hst.note_type );
		}		
		
		
		if( strcmp( v_note_mst_hst.note_type , old_note_type ) )	/* ƾ֤���ͱ仯ʱ */
		{
			/* ���ƾ֤���� */					
			ret = pub_base_get_note_name( old_note_type, note_name );	
			ERR_DEAL
			sprintf(acErrMsg,"note_name=%s",note_name);
			WRITEMSG
			
			pub_base_strpack( note_name );	
	
			/* ͳ�Ƹ���ƾ֤���������������� */
			pub_base_strpack( tel );
			if( strlen(tel) )	/* ��Ա�ŷǿ� */
			{
				ret = sql_sum_long( "note_mst" , "ys_cnt" , &ye_cnt , "br_no='%s' and note_type='%s' and sts in( 0 , 1 ) and tel='%s'" , g_pub_tx.tx_br_no,old_note_type,tel);
				ERR_DEAL					
				ret = sql_sum_long( "note_mst" , "cnt" , &now_cnt , "br_no='%s' and note_type='%s' and sts in( 0 , 1 ) and tel='%s'" , g_pub_tx.tx_br_no,old_note_type,tel);
				ERR_DEAL			
			}else{			/* ��Ա��Ϊ�� */
				ret = sql_sum_long( "note_mst" , "ys_cnt" , &ye_cnt , "br_no='%s' and note_type='%s' and sts in( 0 , 1 ) " , g_pub_tx.tx_br_no,old_note_type);
				ERR_DEAL					
				ret = sql_sum_long( "note_mst" , "cnt" , &now_cnt , "br_no='%s' and note_type='%s' and sts in( 0 , 1 ) " , g_pub_tx.tx_br_no,old_note_type);
				ERR_DEAL		
			}
		
			/**��ʾ�б���**/	
			fprintf( fp,"@3v1\n");
			fprintf( fp,"`           ƾ֤����: ��%3s��\n",old_note_type);
			fprintf( fp,"`           ƾ֤����: ��%s��\n",note_name);
			fprintf( fp,"`           �������������ө����������ө����������ө�����������\n" );
			fprintf( fp,"`           �� ������Ŀ �� �������� �� ���ո��� �� ����ʣ�� ��\n" );			
			fprintf( fp,"`           �������������㩥���������㩥���������㩥����������\n" );			
			fprintf( fp,"`           ��%10ld��%10ld��%10ld��%10ld��\n" ,ye_cnt,in_cnt,out_cnt,now_cnt);			
			fprintf( fp,"`           �������������۩����������۩����������۩�����������\n" );	
			fprintf( fp,"`\n" );					
			fprintf( fp,"`\n" );
						
			/* �������� */
			in_cnt=0;
			out_cnt=0;
			ye_cnt=0;
			now_cnt=0;
			strcpy( old_note_type , v_note_mst_hst.note_type );	/* ƾ֤���� */ 			
		} 
		
		
		/* ͳ����Ϣ */
		if( v_note_mst_hst.note_cnt )
		{
			/* 1-��� 3-���� 5-ƾ֤ת��ת�� 9-�����ս� F-�����ջ� */
			if( v_note_mst_hst.sts_ind[0]=='1' || v_note_mst_hst.sts_ind[0]=='3' || 
				v_note_mst_hst.sts_ind[0]=='5' || v_note_mst_hst.sts_ind[0]=='9' ||
					v_note_mst_hst.sts_ind[0]=='F')		
			{
				in_cnt+=v_note_mst_hst.note_cnt;	
			}
			
			/* 2-�·� 4-ƾ֤ת��ת�� 6-���� 7-�����Ͻ� D-ʹ�� E-���� */
			if( v_note_mst_hst.sts_ind[0]=='2' || v_note_mst_hst.sts_ind[0]=='4' || 
				v_note_mst_hst.sts_ind[0]=='6' || v_note_mst_hst.sts_ind[0]=='7' ||
					v_note_mst_hst.sts_ind[0]=='D' || v_note_mst_hst.sts_ind[0]=='E' )			
			{
				out_cnt+=v_note_mst_hst.note_cnt;
			}
		}
		
		ttlnum++;
	}

	ret=Note_mst_hst_Clo_Sel( );
	ERR_DEAL
TRACE
	if( !ttlnum )
	{
		strcpy( g_pub_tx.reply,"S049" );
		goto ErrExit;
	}else{
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
