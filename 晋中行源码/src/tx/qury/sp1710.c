/*************************************************
* �� �� ��: sp1710.c
* �����������ͻ����Ŷ����Ϣ��ѯ
* ��    ��:  peter
* ������ڣ� 
* �޸ļ�¼��
* ��    ��:
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
#include "cif_crdt_inf_c.h"
#include "cif_crdt_code_c.h"

int sp1710()
{
	struct cif_crdt_inf_c vcif_crdt_inf;
	struct cif_crdt_inf_c f_cif_crdt_inf;
	struct cif_crdt_code_c f_cif_crdt_code;
		char f_cur_name[41];
		char f_yes_no[5];
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

	get_zd_long("0280",&f_cif_crdt_code.cif_no);
	get_zd_data("0580",f_cif_crdt_inf.credit_no);
	get_zd_data("0210",f_cif_crdt_inf.cur_no);
	get_zd_double("0400",&f_cif_crdt_inf.bal);
	get_zd_data("0670",f_cif_crdt_inf.cyc_ind);

	/**��ɲ�ѯ����**/
	if( f_cif_crdt_code.cif_no )
	{
		ret=Cif_crdt_code_Sel(g_pub_tx.reply,&f_cif_crdt_code,"cif_no=%ld", \
							  f_cif_crdt_code.cif_no);
		if(ret==100)
		{
			sprintf(acErrMsg,"�ÿͻ����������ź�ͬ");
			WRITEMSG
			strcpy( g_pub_tx.reply,"C084" );
			goto ErrExit;
		}
		else if(ret)
		{
			sprintf(acErrMsg,"��ѯ�ͻ����Ŷ����Ϣ��");
			WRITEMSG
			goto ErrExit;
		}

		sprintf(tmpstr,"credit_no='%s' and ",f_cif_crdt_code.credit_no);
		strcat( wherelist,tmpstr );
	}

	if( strlen(f_cif_crdt_inf.credit_no) )
	{
		sprintf(tmpstr,"credit_no like '%%%%%s%%%%' and ",f_cif_crdt_inf.credit_no);
		strcat( wherelist,tmpstr );
	}

	if( strlen(f_cif_crdt_inf.cur_no) )
	{
		sprintf(tmpstr,"cur_no='%s' and ",f_cif_crdt_inf.cur_no );
		strcat( wherelist,tmpstr );
	}

	if( pub_base_CompDblVal(f_cif_crdt_inf.bal,0.00) )
	{
		sprintf(tmpstr,"bal=%lf and ",f_cif_crdt_inf.bal );
		strcat( wherelist,tmpstr );
	}

	if( strlen(f_cif_crdt_inf.cyc_ind) )
	{
		sprintf(tmpstr,"cyc_ind='%s' and ",f_cif_crdt_inf.cyc_ind );
		strcat( wherelist,tmpstr );
	}

	if( strlen(f_cif_crdt_inf.credit_no) )
	{
		sprintf(tmpstr,"credit_no like '%%%%%s%%%%' and ",f_cif_crdt_inf.credit_no);
		strcat( wherelist,tmpstr );
	}

	strcpy( tablelist,"cif_crdt_inf" );

	/**��ɲ�ѯ**/
	strcat(wherelist," 1=1") ;

	ret=Cif_crdt_inf_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

	ttlnum=0;

	while(1)
	{
		ret=Cif_crdt_inf_Fet_Sel( &vcif_crdt_inf, g_pub_tx.reply );
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
			fprintf( fp,"~���ź�ͬ��|@����|@���Ž��|@��ʹ�ý��|@ѭ��ʹ��|\n");
		}

		/*pub_pubdb_prt_fld( fp,tablelist,fieldlist ); */

		pub_base_dic_show_str(f_cur_name,"cur_name",vcif_crdt_inf.cur_no);
		pub_base_dic_show_str(f_yes_no,"yes_no",vcif_crdt_inf.cyc_ind);

		fprintf( fp, "%s|%s|%lf|%lf|%s|\n",vcif_crdt_inf.credit_no,f_cur_name,vcif_crdt_inf.bal,vcif_crdt_inf.amt,f_yes_no);
		ttlnum++;
	}

	ret=Cif_crdt_inf_Clo_Sel( );
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
