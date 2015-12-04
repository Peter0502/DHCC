/*************************************************
* �� �� ��:  sp1901.c
* ���������� �ͻ���������ѯ
*
* ��    ��:  peter
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
#include "cif_tranq_stat_c.h"
#include "cif_basic_inf_c.h"

int sp1901()
{
	struct cif_tranq_stat_c vcif_tranq_stat;
	struct cif_tranq_stat_c f_cif_tranq_stat;
	struct cif_basic_inf_c g_cif_basic_inf;
		char f_cur_name[41];
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

	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset( wherelist,0,sizeof(wherelist) );
	memset( fieldlist,0,sizeof(fieldlist) );
	memset( tablelist,0,sizeof(tablelist) );

	pub_base_sysinit();

	get_zd_long("0540",&f_cif_tranq_stat.cif_no );

	/**��ɲ�ѯ����**/
	if( f_cif_tranq_stat.cif_no )
	{
		ret=Cif_basic_inf_Sel(g_pub_tx.reply,&g_cif_basic_inf, \
							"cif_no=%ld",f_cif_tranq_stat.cif_no);
		if(ret==100)
		{
			sprintf(acErrMsg,"��ƥ��ļ�¼!! ");
			WRITEMSG
			strcpy(g_pub_tx.reply,"O043");
			goto ErrExit;
		}

		sprintf( tmpstr," cif_no=%ld and",f_cif_tranq_stat.cif_no );
		strcat( wherelist,tmpstr );
	}

	strcpy( tablelist,"cif_tranq_stat" );

	/**��ɲ�ѯ**/
	strcat(wherelist," 1=1") ;

	vtcp_log( "WHERE [%s]",wherelist );
	strcpy( fieldlist,"cif_no,cur_no,t_cash_cnt,t_cash_amt,t_turn_cnt,t_turn_amt,x_cash_cnt,x_cash_amt,x_turn_cnt,x_turn_amt,m_cash_cnt,m_cash_amt,m_turn_cnt,m_turn_amt,q_cash_cnt,q_cash_amt,q_turn_cnt,q_turn_amt,hy_cash_cnt,hy_cash_amt,hy_turn_cnt,hy_turn_amt,y_cash_cnt,y_cash_amt,y_turn_cnt,y_turn_amt");

	ret=Cif_tranq_stat_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

		ttlnum=0;

	while(1)
	{
		ret=Cif_tranq_stat_Fet_Sel( &vcif_tranq_stat, g_pub_tx.reply );
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
			fprintf( fp,"~�ͻ�����|@����|@����|�����ֽ��ױ���|$�����ֽ��׽��|����ת�˱���|$����ת�ʽ��|��Ѯ�ֽ��ױ���|$��Ѯ�ֽ��׽��|��Ѯת�˱���|$��Ѯת�ʽ��|�����ֽ��ױ���|$�����ֽ��׽��|����ת�˱���|$����ת�ʽ��|�����ֽ��ױ���|$�����ֽ��׽��|����ת�˱���|$����ת�ʽ��|�������ֽ��ױ���|$�����ֽ��׽��|������ת�˱���|$������ת�˽��|�����ֽ��ױ���|$�����ֽ��׽��|����ת�˱���|$����ת�˽��|\n" );
		}

		ret=Cif_basic_inf_Sel(g_pub_tx.reply,&g_cif_basic_inf, \
							  "cif_no=%ld",vcif_tranq_stat.cif_no);
		if(ret==100)
		{
			sprintf(acErrMsg,"��ƥ���¼!!");
			WRITEMSG
			strcpy( g_pub_tx.reply,"O043" );
			break;
		}

		/*pub_pubdb_prt_fld( fp,tablelist,fieldlist ); */

		if(vcif_tranq_stat.cif_no<-2147000000) vcif_tranq_stat.cif_no=0;
		if(vcif_tranq_stat.t_cash_cnt<-2147000000) vcif_tranq_stat.t_cash_cnt=0;
		if(vcif_tranq_stat.t_cash_amt<-2147000000) vcif_tranq_stat.t_cash_amt=0.0;
		if(vcif_tranq_stat.t_turn_cnt<-2147000000) vcif_tranq_stat.t_turn_cnt=0;
		if(vcif_tranq_stat.t_turn_amt<-2147000000) vcif_tranq_stat.t_turn_amt=0.0;
		if(vcif_tranq_stat.x_cash_cnt<-2147000000) vcif_tranq_stat.x_cash_cnt=0;
		if(vcif_tranq_stat.x_cash_amt<-2147000000) vcif_tranq_stat.x_cash_amt=0.0;
		if(vcif_tranq_stat.x_turn_cnt<-2147000000) vcif_tranq_stat.x_turn_cnt=0;
		if(vcif_tranq_stat.x_turn_amt<-2147000000) vcif_tranq_stat.x_turn_amt=0.0;
		if(vcif_tranq_stat.m_cash_cnt<-2147000000) vcif_tranq_stat.m_cash_cnt=0;
		if(vcif_tranq_stat.m_cash_amt<-2147000000) vcif_tranq_stat.m_cash_amt=0.0;
		if(vcif_tranq_stat.m_turn_cnt<-2147000000) vcif_tranq_stat.m_turn_cnt=0;
		if(vcif_tranq_stat.m_turn_amt<-2147000000) vcif_tranq_stat.m_turn_amt=0.0;
		if(vcif_tranq_stat.q_cash_cnt<-2147000000) vcif_tranq_stat.q_cash_cnt=0;
		if(vcif_tranq_stat.q_cash_amt<-2147000000) vcif_tranq_stat.q_cash_amt=0.0;
		if(vcif_tranq_stat.q_turn_cnt<-2147000000) vcif_tranq_stat.q_turn_cnt=0;
		if(vcif_tranq_stat.q_turn_amt<-2147000000) vcif_tranq_stat.q_turn_amt=0.0;
		if(vcif_tranq_stat.hy_cash_cnt<-2147000000) vcif_tranq_stat.hy_cash_cnt=0;
		if(vcif_tranq_stat.hy_cash_amt<-2147000000) vcif_tranq_stat.hy_cash_amt=0.0;
		if(vcif_tranq_stat.hy_turn_cnt<-2147000000) vcif_tranq_stat.hy_turn_cnt=0;
		if(vcif_tranq_stat.hy_turn_amt<-2147000000) vcif_tranq_stat.hy_turn_amt=0.0;
		if(vcif_tranq_stat.y_cash_cnt<-2147000000) vcif_tranq_stat.y_cash_cnt=0;
		if(vcif_tranq_stat.y_cash_amt<-2147000000) vcif_tranq_stat.y_cash_amt=0.0;
		if(vcif_tranq_stat.y_turn_cnt<-2147000000) vcif_tranq_stat.y_turn_cnt=0;
		if(vcif_tranq_stat.y_turn_amt<-2147000000) vcif_tranq_stat.y_turn_amt=0.0;
		pub_base_dic_show_str(f_cur_name,"cur_name",vcif_tranq_stat.cur_no);

		fprintf( fp, "%ld|%s|%s|%ld|%.2lf|%ld|%.2lf|%ld|%.2lf|%ld|%.2lf|%ld|%.2lf|%ld|%.2lf|%ld|%.2lf|%ld|%.2lf|%ld|%.2lf|%ld|%.2lf|%ld|%.2lf|%ld|%.2lf|\n",
		vcif_tranq_stat.cif_no,g_cif_basic_inf.name,
		f_cur_name,vcif_tranq_stat.t_cash_cnt,
		vcif_tranq_stat.t_cash_amt,vcif_tranq_stat.t_turn_cnt,
		vcif_tranq_stat.t_turn_amt,vcif_tranq_stat.x_cash_cnt,
		vcif_tranq_stat.x_cash_amt,vcif_tranq_stat.x_turn_cnt,vcif_tranq_stat.x_turn_amt,vcif_tranq_stat.m_cash_cnt,vcif_tranq_stat.m_cash_amt,vcif_tranq_stat.m_turn_cnt,vcif_tranq_stat.m_turn_amt,vcif_tranq_stat.q_cash_cnt,vcif_tranq_stat.q_cash_amt,vcif_tranq_stat.q_turn_cnt,vcif_tranq_stat.q_turn_amt,vcif_tranq_stat.hy_cash_cnt,vcif_tranq_stat.hy_cash_amt,vcif_tranq_stat.hy_turn_cnt,vcif_tranq_stat.hy_turn_amt,vcif_tranq_stat.y_cash_cnt,vcif_tranq_stat.y_cash_amt,vcif_tranq_stat.y_turn_cnt,vcif_tranq_stat.y_turn_amt);
		ttlnum++;
	}

	ret=Cif_tranq_stat_Clo_Sel( );
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
