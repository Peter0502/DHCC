/*************************************************
* �� �� ��:  sp5623.c
* ������������ػ�Ʊί�еǼ� 
*
* ��    ��: Jarod 
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
#include "whwtsk_reg_c.h"

int sp5623()
{
	struct whwtsk_reg_c sWhwtsk_reg;
	struct whwtsk_reg_c sWhwtsk_reg1;
	struct whwtsk_reg_c sWhwtsk_reg2;
		char filename[100];
		char wherelist[1024];  /**��ѯ����**/
		char tmpstr[512];
		char tablelist[128];   /**Ҫ��ѯ�ı���**/
		int ttlnum=0;			/**��ȡ��������**/
		int i=0;
		int ret=0;
		FILE *fp;

	memset( &sWhwtsk_reg,0x00,sizeof(struct whwtsk_reg_c));
	memset( &sWhwtsk_reg1,0x00,sizeof(struct whwtsk_reg_c));
	memset( &sWhwtsk_reg2,0x00,sizeof(struct whwtsk_reg_c));
	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset( wherelist,0,sizeof(wherelist) );
	memset( tablelist,0,sizeof(tablelist) );

	pub_base_sysinit();

	get_zd_data("0630",sWhwtsk_reg1.pact_no );
	get_zd_double("0400",&sWhwtsk_reg1.sign_amt );
	get_zd_double("0410",&sWhwtsk_reg2.sign_amt );
	get_zd_long("0290",&sWhwtsk_reg1.matr_date );
	get_zd_long("0440",&sWhwtsk_reg2.matr_date );
	/**��ɲ�ѯ����**/
	pub_base_strpack( sWhwtsk_reg1.pact_no );
	if( strlen(sWhwtsk_reg1.pact_no) )
	{
		sprintf( tmpstr," pact_no='%s' and",sWhwtsk_reg1.pact_no );
		strcat( wherelist,tmpstr );
	}
	if( sWhwtsk_reg1.sign_amt )
	{
		sprintf( tmpstr," sign_amt>=%lf and",sWhwtsk_reg1.sign_amt );
		strcat( wherelist,tmpstr );
	}
	if( sWhwtsk_reg2.sign_amt )
	{
		sprintf( tmpstr," sign_amt<=%lf and",sWhwtsk_reg2.sign_amt );
		strcat( wherelist,tmpstr );
	}
	if( sWhwtsk_reg1.matr_date )
	{
		sprintf( tmpstr," matr_date>=%ld and",sWhwtsk_reg1.matr_date );
		strcat( wherelist,tmpstr );
	}
	if( sWhwtsk_reg2.matr_date )
	{
		sprintf( tmpstr," matr_date<=%ld and",sWhwtsk_reg2.matr_date );
		strcat( wherelist,tmpstr );
	}
	strcpy( tablelist,"whwtsk_reg" );

	/**��ɲ�ѯ**/
	strcat(wherelist," 1=1") ;

	vtcp_log( "WHERE [%s]",wherelist );

	ret=Whwtsk_reg_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

		ttlnum=0;
TRACE
	while(1)
	{
		ret=Whwtsk_reg_Fet_Sel( &sWhwtsk_reg, g_pub_tx.reply );
		if( ret==100 ) break;
		ERR_DEAL
vtcp_log( "xxxxxx  pact_num[%s]", sWhwtsk_reg.pact_no );
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
					fprintf(fp,"~@�жһ�ƱЭ����|@���׻�����|@��������|@������ȫ��|@�������ʺ�|@�����˿������к�|@�����˿���������|@�տ���ȫ��|@�տ����ʺ�|@�տ��˿���������|$ǩ��Ʊ����|��������|\n");	
				}
	
				if(sWhwtsk_reg.sign_amt<-2147000000) sWhwtsk_reg.sign_amt=0.0;
				if(sWhwtsk_reg.matr_date<-2147000000) sWhwtsk_reg.matr_date=0;

				fprintf( fp,"%s|%s|%d|%s|%s|%s|%s|%s|%s|%s|%.2lf|%d|\n",sWhwtsk_reg.pact_no,sWhwtsk_reg.tx_br_no,sWhwtsk_reg.req_date,sWhwtsk_reg.payer_name,sWhwtsk_reg.payer_ac_no,sWhwtsk_reg.payer_br_no,sWhwtsk_reg.payer_br_name,sWhwtsk_reg.payee_name,sWhwtsk_reg.payee_ac_no,sWhwtsk_reg.payee_br_name,sWhwtsk_reg.sign_amt,sWhwtsk_reg.matr_date);	
				ttlnum++;
	}
	ret=Whwtsk_reg_Clo_Sel( );
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
