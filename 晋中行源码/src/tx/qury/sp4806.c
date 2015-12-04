/*************************************************
* �� �� ��:  sp4806.c
* ���������� �ٴ������Ϣ��ѯ
*
* ��    ��:  lance
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
#include "mo_pay_again_ln_c.h"
#include "mo_again_ln_c.h"

int sp4806()
{
	struct mo_pay_again_ln_c vmo_pay_again_ln;
	struct mo_pay_again_ln_c f_mo_pay_again_ln;
	struct mo_pay_again_ln_c f_mo_pay_again_lna;
	struct	mo_again_ln_c	sMoAgainLn;	/* �ٴ���ǼǱ� */
		char f_sts14[41];
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
		int rett=0;
		FILE *fp;

	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset( wherelist,0,sizeof(wherelist) );
	memset( fieldlist,0,sizeof(fieldlist) );
	memset( tablelist,0,sizeof(tablelist) );
	memset( &sMoAgainLn, 0x00, sizeof( struct mo_again_ln_c ) );

	pub_base_sysinit();

	get_zd_data("0600",f_mo_pay_again_ln.pact_no );
	get_zd_double("0400",&f_mo_pay_again_ln.amt );
	get_zd_double("0410",&f_mo_pay_again_lna.amt );
	get_zd_long("0290",&f_mo_pay_again_ln.pay_date );
	get_zd_long("0440",&f_mo_pay_again_lna.pay_date );
	get_zd_data("0670",f_mo_pay_again_ln.sts );
	
	/**��ɲ�ѯ����**/
	pub_base_strpack( f_mo_pay_again_ln.pact_no );
	if( strlen(f_mo_pay_again_ln.pact_no) )
	{
		sprintf( tmpstr," pact_no='%s' and",f_mo_pay_again_ln.pact_no );
		strcat( wherelist,tmpstr );
	}
	if( f_mo_pay_again_ln.amt )
	{
		sprintf( tmpstr," amt>=%lf and",f_mo_pay_again_ln.amt );
		strcat( wherelist,tmpstr );
	}
	if( f_mo_pay_again_lna.amt )
	{
		sprintf( tmpstr," amt<=%lf and",f_mo_pay_again_lna.amt );
		strcat( wherelist,tmpstr );
	}
	if( f_mo_pay_again_ln.pay_date )
	{
		sprintf( tmpstr," pay_date>=%ld and",f_mo_pay_again_ln.pay_date );
		strcat( wherelist,tmpstr );
	}
	if( f_mo_pay_again_lna.pay_date )
	{
		sprintf( tmpstr," pay_date<=%ld and",f_mo_pay_again_lna.pay_date );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_mo_pay_again_ln.sts );
	if( strlen(f_mo_pay_again_ln.sts) )
	{
		sprintf( tmpstr," sts='%s' and",f_mo_pay_again_ln.sts );
		strcat( wherelist,tmpstr );
	}

	strcpy( tablelist,"mo_pay_again_ln" );

	/**��ɲ�ѯ**/
	strcat(wherelist," 1=1") ;

	vtcp_log( "WHERE [%s]",wherelist );
	strcpy( fieldlist,"pact_no,amt,pay_date,sts");

	ret=Mo_pay_again_ln_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

		ttlnum=0;

	while(1)
	{
		ret=Mo_pay_again_ln_Fet_Sel( &vmo_pay_again_ln, g_pub_tx.reply );
		if( ret==100 ) break;
		ERR_DEAL
		memcpy(sMoAgainLn.pact_no,vmo_pay_again_ln.pact_no,sizeof(vmo_pay_again_ln.pact_no));	     /* Э���� */
  /* ����ٴ���ǼǱ����Ƿ���ڴ�Э���� 20090928*/
	  ret = Mo_again_ln_Sel( g_pub_tx.reply, &sMoAgainLn, "pact_no='%s'", sMoAgainLn.pact_no );
	  if( ret == 100 )
   	{
		sprintf( acErrMsg, "�ٴ���ǼǱ��в����ڴ�Э����!" );
                WRITEMSG
                strcpy( g_pub_tx.reply, "B107" );
                goto ErrExit;
    }
    else if( ret )
    {
       		sprintf( acErrMsg, "ִ��Mo_again_ln_Sel��![%d]", ret );
                WRITEMSG
                goto ErrExit;
    }
       	sMoAgainLn.amt-=vmo_pay_again_ln.amt; /* ��Ƿ��� 20090928*/
       	
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
			fprintf( fp,"~@Э����|$������|��������|@��־|��Ƿ���|\n" );
		}

		/*pub_pubdb_prt_fld( fp,tablelist,fieldlist ); */

		if(vmo_pay_again_ln.amt<-2147000000) vmo_pay_again_ln.amt=0.0;
		if(vmo_pay_again_ln.pay_date<-2147000000) vmo_pay_again_ln.pay_date=0;
		pub_base_dic_show_str(f_sts14,"payagain_ln_sts",vmo_pay_again_ln.sts);

		fprintf( fp, "%s|%.2lf|%ld|%s|%.2lf|\n",vmo_pay_again_ln.pact_no,vmo_pay_again_ln.amt,vmo_pay_again_ln.pay_date,f_sts14,sMoAgainLn.amt);
		ttlnum++;
	}

	ret=Mo_pay_again_ln_Clo_Sel( );
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
