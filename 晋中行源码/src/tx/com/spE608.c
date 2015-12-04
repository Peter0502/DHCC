/*************************************************
* �� �� ��:  spE608.c
* ���������� ��������(�ֽ�)
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
#include "cash_mst_c.h"

int spE608()
{

	char filename[100];
	char wherelist[1024];  /**��ѯ����**/
	char tmpstr[512];
	
	char tel[5];
	int ret=0;
	
	double in_amt=0.00,out_amt=0.00,tot_amt=0.00,ys_tot_amt=0.00;
						
	FILE *fp;
	
	struct com_tel_c	v_com_tel;
	struct cash_mst_c	v_cash_mst;
	
	memset( &v_com_tel , 0x00 , sizeof(struct com_tel_c));			
	memset( &v_cash_mst , 0x00 , sizeof(struct cash_mst_c));	
	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset( wherelist,0,sizeof(wherelist) );

	
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
		
	/* �������� */
	sprintf( tmpstr," br_no='%s' and cur_no='01' and ",g_pub_tx.tx_br_no );
	strcat( wherelist,tmpstr );

	
	/**��ɲ�ѯ**/
	strcat(wherelist," sts='0' ") ;

	vtcp_log( "WHERE [%s]",wherelist );
	
	ret = Cash_mst_Sel( g_pub_tx.reply , &v_cash_mst , wherelist );
	if( ret ==100 )
	{
		sprintf(acErrMsg,"�˹�Ա�޿���Ǯ��!");
		WRITEMSG
		strcpy( g_pub_tx.reply ,"D189" );			
		goto ErrExit;		
	}else if( ret ){
		sprintf(acErrMsg,"��ѯ�ֽ����ļ��쳣����,�������Ļ�����ϵ!");
		WRITEMSG
		strcpy( g_pub_tx.reply ,"D190" );			
		goto ErrExit;	
	}
	tot_amt    = v_cash_mst.bal;	
	ys_tot_amt = v_cash_mst.ys_bal;	
}
else{/*�����Ա��Ϊ��,��ȡ���й�ԱǮ�����Ϊ�������ֽ����JYW*/
	sprintf( tmpstr," br_no='%s' and cur_no='01' and  ",g_pub_tx.tx_br_no );
	strcat( wherelist,tmpstr );
  strcat( wherelist,"  sts='0' ");
	
	/**��ɲ�ѯ**/
	
	vtcp_log( "WHERE [%s]",wherelist );	
	ret=sql_sum_double( "cash_mst","bal",&tot_amt,wherelist);
	ret=sql_sum_double( "cash_mst","ys_bal",&ys_tot_amt,wherelist);
	vtcp_log("[%s][%d], wherelist = [%s] ,ys_bal=[%.2f]\n",__FILE__,__LINE__,wherelist,ys_tot_amt);
  ERR_DEAL
}	
	
	/* ͳ�Ʊ����ֽ����븶����� */
	pub_base_strpack( tel );
	if( strlen(tel) )	/* ��Ա�ŷǿ� */
	{	
		ret = sql_sum_double( "cash_mst_hst" , "amt" , &in_amt , "tx_date=%ld and br_no='%s' and cur_no='01' and add_ind='1' and tel='%s'" , g_pub_tx.tx_date,g_pub_tx.tx_br_no,tel);
		ERR_DEAL				
		ret = sql_sum_double( "cash_mst_hst" , "amt" , &out_amt , "tx_date=%ld and br_no='%s' and cur_no='01' and add_ind='0' and tel='%s'" , g_pub_tx.tx_date,g_pub_tx.tx_br_no,tel);			
		ERR_DEAL			
	}else{			/* ��Ա��Ϊ�� */
	
		ret = sql_sum_double( "cash_mst_hst" , "amt" , &in_amt , "tx_date=%ld and br_no='%s' and cur_no='01' and add_ind='1' " , g_pub_tx.tx_date,g_pub_tx.tx_br_no);
		ERR_DEAL				
		ret = sql_sum_double( "cash_mst_hst" , "amt" , &out_amt , "tx_date=%ld and br_no='%s' and cur_no='01' and add_ind='0' " , g_pub_tx.tx_date,g_pub_tx.tx_br_no);			
		ERR_DEAL
	}	
	
	/* д��ʾ�ļ� */
	pub_base_AllDwnFilName( filename );
	fp = fopen( filename,"w" );
	if( fp==NULL )
	{
		sprintf(acErrMsg," open file [%s] error ",filename );
		WRITEMSG
		strcpy( g_pub_tx.reply,"S047" );
		goto ErrExit;
	}
	fprintf( fp,"@3v1");
	fprintf( fp,"`\n" );	
	fprintf( fp,"`\n" );
	fprintf( fp,"`                            �������ֽ�������Ϣ��\n" );
	fprintf( fp,"`\n" );	
	fprintf( fp,"` �������������������ө����������������ө����������������ө�����������������\n" );
	fprintf( fp,"` ��   �� �� �� ��  ��   �� �� �� ��  ��   �� �� �� ��  ��   �� �� �� ��  ��\n" );			
	fprintf( fp,"` �������������������㩥���������������㩥���������������㩥����������������\n" );			
	fprintf( fp,"` ��%16.2lf��%16.2lf��%16.2lf��%16.2lf��\n" ,ys_tot_amt,in_amt,out_amt,tot_amt);			
	fprintf( fp,"` �������������������۩����������������۩����������������۩�����������������\n" );	
	fprintf( fp,"`\n" );		
	fprintf( fp,"`\n" );
	
	fclose(fp);
	set_zd_data( DC_FILE_SND,"1" );

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
