/*************************************************
* �� �� ��:  sp4342.c
* ���������� �������Ϣ��ѯ
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
#include "mo_sg_cash_po_bill_c.h"

int sp4342()
{
	struct mo_sg_cash_po_bill_c vmo_sg_cash_po_bill;
	struct mo_sg_cash_po_bill_c f_mo_sg_cash_po_bill;
	struct mo_sg_cash_po_bill_c f_mo_sg_cash_po_billa;
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

	get_zd_data("0380",f_mo_sg_cash_po_bill.lading_bill );
	get_zd_double("0400",&f_mo_sg_cash_po_bill.pickup_amt );
	get_zd_double("0410",&f_mo_sg_cash_po_billa.pickup_amt );

	/**��ɲ�ѯ����**/
	pub_base_strpack( f_mo_sg_cash_po_bill.lading_bill );
	if( strlen(f_mo_sg_cash_po_bill.lading_bill) )
	{
		sprintf( tmpstr," lading_bill='%s' and",f_mo_sg_cash_po_bill.lading_bill );
		strcat( wherelist,tmpstr );
	}
	if( f_mo_sg_cash_po_bill.pickup_amt )
	{
		sprintf( tmpstr," pickup_amt>=%lf and",f_mo_sg_cash_po_bill.pickup_amt );
		strcat( wherelist,tmpstr );
	}
	if( f_mo_sg_cash_po_billa.pickup_amt )
	{
		sprintf( tmpstr," pickup_amt<=%lf and",f_mo_sg_cash_po_billa.pickup_amt );
		strcat( wherelist,tmpstr );
	}

	strcpy( tablelist,"mo_sg_cash_po_bill" );

	/**��ɲ�ѯ**/
	strcat(wherelist," 1=1") ;

	vtcp_log( "WHERE [%s]",wherelist );
	strcpy( fieldlist,"pact_no,bill_num,lading_bill,pickup_amt");

	ret=Mo_sg_cash_po_bill_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

		ttlnum=0;

	while(1)
	{
		ret=Mo_sg_cash_po_bill_Fet_Sel( &vmo_sg_cash_po_bill, g_pub_tx.reply );
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
			fprintf( fp,"~@�ж�Э����|��������|@���������|$��������|\n" );
		}

		/*pub_pubdb_prt_fld( fp,tablelist,fieldlist ); */

		if(vmo_sg_cash_po_bill.pickup_amt<-2147000000) vmo_sg_cash_po_bill.pickup_amt=0.0;

		fprintf( fp, "%s|%d|%s|%.2lf|\n",vmo_sg_cash_po_bill.pact_no,vmo_sg_cash_po_bill.bill_num,vmo_sg_cash_po_bill.lading_bill,vmo_sg_cash_po_bill.pickup_amt);
		ttlnum++;
	}

	ret=Mo_sg_cash_po_bill_Clo_Sel( );
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
