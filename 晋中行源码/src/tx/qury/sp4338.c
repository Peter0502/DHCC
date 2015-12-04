/*************************************************
* �� �� ��:  sp4338.c
* ���������� ���гжһ�Ʊ��ѯ
*
* ��    ��:  peter
* ������ڣ� 2003��6��1��
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
#include "mo_bank_po_c.h"
#include "mo_bank_acc_po_c.h"

int sp4338()
{
	struct 	mo_bank_po_c 		vmo_bank_po;
	struct 	mo_bank_po_c 		f_mo_bank_po;
	struct 	mo_bank_po_c 		f_mo_bank_poa;
	struct	mo_bank_acc_po_c 	vmo_bank_acc_po;	/* �жһ�Ʊ������Ϣ�Ǽǲ� */
	struct	mo_bank_acc_po_c 	f_mo_bank_acc_po;
	struct	mo_bank_acc_po_c 	f_mo_bank_acc_poa;
	char 	f_po_sts7[41];
	char 	f_po_ind8[41];
	char	f_acc_po_sts5[41];
	char	f_acc_po_ind5[41];
	char	f_at_term_ind5[41];
	char	cPoNo2[9];							/* �³жһ�Ʊ������Чλ */
	char	cPoNoHead2[5];						/* �³жһ�Ʊ�������ͷ */
	char	cPoNo22[17];						/* �³жһ�Ʊ�������ͷ+��Чλ */
	char    dbact[25];
	char    bzact[25];
	char filename[100];
	char wherelist1[1024];  /**��ѯ����**/
	char tmpstr1[512];
	char cBr_no[6];
	int ttlnum=0;			/**��ȡ��������**/
	int ret=0;
	FILE *fp;

	/*--------add by jack 20041009---------*/
	int iCount = 0;			/*���ܱ���*/
	double dTram = 0.00;	/*���ܽ��*/
	/*------------------------------------*/

	memset( &g_pub_tx,0x00,sizeof(g_pub_tx) );
	memset( wherelist1,0x00,sizeof(wherelist1) );
	memset( tmpstr1,0x00,sizeof(tmpstr1) );
	memset( cBr_no,0x00,sizeof(cBr_no) );
	memset( &vmo_bank_po,0x00,sizeof(vmo_bank_po) );
	memset( &f_mo_bank_po,0x00,sizeof(f_mo_bank_po) );
	memset( &f_mo_bank_poa,0x00,sizeof(f_mo_bank_poa) );
	memset( &vmo_bank_acc_po,0x00,sizeof(vmo_bank_acc_po) );
	memset( &f_mo_bank_acc_po,0x00,sizeof(f_mo_bank_acc_po) );
	memset( &f_mo_bank_acc_poa,0x00,sizeof(f_mo_bank_acc_poa) );


	pub_base_sysinit();

	get_zd_data("0590",f_mo_bank_po.po_no );
	get_zd_long("0290",&f_mo_bank_po.sign_date );
	get_zd_long("0440",&f_mo_bank_poa.sign_date );
	get_zd_double("0400",&f_mo_bank_po.po_amt );
	get_zd_double("0410",&f_mo_bank_poa.po_amt );
	get_zd_data("0670",f_mo_bank_po.po_sts );
	get_zd_data("0030",cBr_no);
	get_zd_long("0450",&f_mo_bank_acc_po.matr_date );
	get_zd_long("0460",&f_mo_bank_acc_poa.matr_date );

	/**��ɲ�ѯ����**/
	pub_base_strpack( f_mo_bank_po.po_no );
	if( strlen(f_mo_bank_po.po_no) )
	{
		/* �������жһ�Ʊ�����Ƿ������ͷ *
		ret = pub_com_ChkNoteHead1( "111", f_mo_bank_po.po_no );
		if( ret )
		{
			sprintf( acErrMsg, "���жһ�Ʊ�������![%d]", ret );
			WRITEMSG
			goto ErrExit;
		}
		**********************************/

		/* ȡ�жһ�Ʊ����ͷ */
		strncpy( cPoNoHead2, f_mo_bank_po.po_no, 4 );
		cPoNoHead2[4]='\0';
		pub_base_strpack( cPoNoHead2);

		/* ȡ�жһ�Ʊ��Чλ */
		strncpy( cPoNo2, f_mo_bank_po.po_no+4, 8 );
		cPoNo2[8] = '\0';
sprintf( acErrMsg, "xxxxx    cPoNo2[%s]", cPoNo2 );
WRITEMSG

		/* �жһ�Ʊ�������ͷ+��Чλ */
		sprintf( cPoNo22, " %s%s ", cPoNoHead2, cPoNo2 );
sprintf( acErrMsg, "xxxxx    cPoNo22[%s]", cPoNo22 );
WRITEMSG
		pub_base_strpack_all( cPoNo22 );

		sprintf( tmpstr1," po_no like '%%%%%s%%%%' and",cPoNo22 );
		strcat( wherelist1,tmpstr1 );
	}
	if( f_mo_bank_po.sign_date )
	{
		sprintf( tmpstr1," sign_date<=%ld and",f_mo_bank_po.sign_date );
		strcat( wherelist1,tmpstr1 );
	}
	if( f_mo_bank_poa.sign_date )
	{
		sprintf( tmpstr1," sign_date>=%ld and",f_mo_bank_poa.sign_date );
		strcat( wherelist1,tmpstr1 );
	}
	if( f_mo_bank_po.po_amt )
	{
		sprintf( tmpstr1," po_amt>=%lf and",f_mo_bank_po.po_amt );
		strcat( wherelist1,tmpstr1 );
	}
	if( f_mo_bank_poa.po_amt )
	{
		sprintf( tmpstr1," po_amt<=%lf and",f_mo_bank_poa.po_amt );
		strcat( wherelist1,tmpstr1 );
	}
	pub_base_strpack( f_mo_bank_po.po_sts );
	if( strlen(f_mo_bank_po.po_sts) )
	{
		sprintf( tmpstr1," po_sts='%s' and",f_mo_bank_po.po_sts );
		strcat( wherelist1,tmpstr1 );
	}

	/**��ɲ�ѯ**/
	strcat(wherelist1," 1=1 and sign_br_no='%s' order by sign_date,pact_no,pact_no_num") ;

	vtcp_log( "��ȡ���Ĳ�ѯ����Ϊ WHERE [%s]",wherelist1 );
	vtcp_log("[%s][%d]  cBr_no=[%s]\n",__FILE__,__LINE__, cBr_no);

	ret=Mo_bank_po_Dec_Sel( g_pub_tx.reply,wherelist1 ,cBr_no);
	ERR_DEAL

	ttlnum=0;

	while(1)
	{
		ret=Mo_bank_po_Fet_Sel( &vmo_bank_po, g_pub_tx.reply );
		if( ret==100 ) break;
		ERR_DEAL

		pub_base_strpack( vmo_bank_po.pact_no );
		vtcp_log("[%s][%d]  vmo_bank_po.pact_no=[%s]\n",__FILE__,__LINE__, vmo_bank_po.pact_no);
		/* ���ݳж�Э���Ų������гжһ�Ʊ������Ϣ�ǼǱ� */
		ret = Mo_bank_acc_po_Sel( g_pub_tx.reply, &vmo_bank_acc_po, \
					"pact_no='%s'", vmo_bank_po.pact_no );
		if( ret == 100 )
		{
			sprintf( acErrMsg, "�޴˳ж���Ϣ!" );
               	 	WRITEMSG
                	strcpy( g_pub_tx.reply, "B230" );
                	goto ErrExit;
       		}else if( ret )
       		{
       			sprintf( acErrMsg, "ִ��Mo_bank_acc_po_Sel��![%d]", ret );
                	WRITEMSG
                	goto ErrExit;
       		}

		if( f_mo_bank_acc_po.matr_date )
		{
			if( vmo_bank_acc_po.matr_date < f_mo_bank_acc_po.matr_date )
			{
				break;
			}
		}
		if( f_mo_bank_acc_poa.matr_date )
		{
			if( vmo_bank_acc_po.matr_date > f_mo_bank_acc_poa.matr_date )
			{
				break;
			}
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

			/**��ʾ�б���**
			fprintf( fp,"~@�ж�Э����|���|@��Ʊ����|$��Ʊ���|@ǩ�����к�|ǩ������|��������|��������|��������|@��Ʊ���˺�|@��Ʊ��ȫ��|@�������к�|@������ȫ��|@�տ����˺�|@�տ���ȫ��|@�տ��˿���������|$ǩ��Ʊ���ܽ��|@��֤���˺�|$��֤����|$��֤�����|$��Ѻ�ܽ��|$���е���|@�ж�Э��״̬|@�ж�Э�鵽�ڴ����־|@��Ʊ���ڱ�־|@�жһ�Ʊ״̬|@�жһ�Ʊ��־|\n" );
			***/
			fprintf( fp,"~@ǩ�����к�|@��Ʊ��ȫ��|$��Ʊ���|ǩ������|��������|��Ʊ���˺�|$��֤�����|@�ж�Э����|���|@��Ʊ����|@�������к�|@������ȫ��|�տ����˺�|�տ���ȫ��|@�տ��˿���������|$ǩ��Ʊ���ܽ��|@�����ʺ�|@��֤���˺�|$��֤����|$��Ѻ�ܽ��|$���е���|@�ж�Э��״̬|@�ж�Э�鵽�ڴ����־|@��Ʊ���ڱ�־|@�жһ�Ʊ״̬|��������|@�жһ�Ʊ��־|\n" );
		}

		memset(dbact,0,sizeof(dbact));
		memset(bzact,0,sizeof(bzact));
		if(vmo_bank_acc_po.matr_date<-2147000000) vmo_bank_acc_po.matr_date=0;
		if(vmo_bank_po.sign_date<-2147000000) vmo_bank_po.sign_date=0;
		if(vmo_bank_po.receive_date<-2147000000) vmo_bank_po.receive_date=0;
		if(vmo_bank_po.po_amt<-2147000000) vmo_bank_po.po_amt=0.0;
		if(vmo_bank_acc_po.advance_amt<-2147000000) vmo_bank_acc_po.advance_amt=0.0;
		pub_base_dic_show_str(f_po_sts7,"acc_po_sts",vmo_bank_po.po_sts);
		pub_base_dic_show_str(f_po_ind8,"ind",vmo_bank_po.po_ind);
		pub_base_dic_show_str(f_at_term_ind5,"at_term_ind",vmo_bank_po.at_term_ind);
		pub_base_dic_show_str(f_acc_po_sts5,"accpo_sts",vmo_bank_acc_po.acc_po_sts);
		pub_base_dic_show_str(f_acc_po_ind5,"accpo_ind",vmo_bank_acc_po.acc_po_ind);

		if(vmo_bank_acc_po.acc_type[0] == '4')	/** �����ж� **/
		{
				memcpy(dbact,vmo_bank_acc_po.bail_ac_no,sizeof(dbact)-1);
		}else{
				memcpy(bzact,vmo_bank_acc_po.bail_ac_no,sizeof(bzact)-1);
		}

		/***
		fprintf( fp, "%s|%d|%s|%.2lf|%s|%ld|%ld|%ld|%ld|%s|%s|%s|%s|%s|%s|%s|%.2lf|%s|%.2lf|%.2lf|%.2lf|%.2lf|%s|%s|%s|%s|%s|\n",\
			vmo_bank_po.pact_no,vmo_bank_acc_po.payer_name, \
			vmo_bank_po.po_amt, vmo_bank_po.sign_date, \
			vmo_bank_acc_po.matr_date, vmo_bank_acc_po.payer_ac_no, \
			vmo_bank_po.sign_br_no, \
			vmo_bank_po.pact_no_num, vmo_bank_po.po_no, \
			vmo_bank_po.receive_date, vmo_bank_acc_po.req_date, \
			vmo_bank_acc_po.payer_ac_no,\
			vmo_bank_acc_po.payer_br_no,vmo_bank_acc_po.payer_br_name, \
			vmo_bank_acc_po.payee_ac_no,vmo_bank_acc_po.payee_name, \
			vmo_bank_acc_po.payee_br_name,vmo_bank_acc_po.sign_amt,\
			vmo_bank_acc_po.bail_ac_no,vmo_bank_acc_po.bail_amt, \
			vmo_bank_acc_po.bail_amt_ratio, vmo_bank_acc_po.tot_mort_amt, \
			vmo_bank_acc_po.advance_amt,f_acc_po_sts5,f_acc_po_ind5, \
			f_at_term_ind5,f_po_sts7,f_po_ind8);
			***/
			if( vmo_bank_po.cash_date<0 )
				vmo_bank_po.cash_date=0;
		fprintf( fp, "%s|%s|%.2lf|%ld|%ld|%s|%.2lf|%s|%ld|%s|%s|%s|%s|%s|%s|%.2lf|%s|%s|%.2lf|%.2lf|%.2lf|%s|%s|%s|%s|%ld|%s|\n",\
			vmo_bank_po.sign_br_no,vmo_bank_acc_po.payer_name, \
			vmo_bank_po.po_amt,vmo_bank_po.sign_date,vmo_bank_acc_po.matr_date,\
			vmo_bank_acc_po.payer_ac_no, vmo_bank_acc_po.bail_amt_ratio, \
			vmo_bank_po.pact_no, vmo_bank_po.pact_no_num,\
			vmo_bank_po.po_no, vmo_bank_acc_po.payer_br_no, \
			vmo_bank_acc_po.payer_br_name, vmo_bank_acc_po.payee_ac_no, \
			vmo_bank_acc_po.payee_name, vmo_bank_acc_po.payee_br_name, \
			vmo_bank_acc_po.sign_amt, dbact,bzact, \
			vmo_bank_acc_po.bail_amt, vmo_bank_acc_po.tot_mort_amt, \
			vmo_bank_acc_po.advance_amt, f_acc_po_sts5,f_acc_po_ind5, \
			f_at_term_ind5,f_po_sts7,vmo_bank_po.cash_date,
			f_po_ind8);

		ttlnum++;
		
		iCount = iCount + 1;
		dTram = dTram + vmo_bank_po.po_amt;
	}
	ret=Mo_bank_po_Clo_Sel( );
	ERR_DEAL

	if( !ttlnum )
	{
		strcpy( g_pub_tx.reply,"S049" );
		goto ErrExit;
	}
	else
	{
		fprintf( fp, "%s|%s|%.2lf|%s|%ld|%s|%s|%s|%s|%s|%s|%s| \
			%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|\n", \
			"����", "�ܽ��:", dTram, "�ܱ���:", iCount, "", \
			"", "", "", "", "", "", "", "", "", "", \
			"", "", "", "", "", "", "", "", "", "" );

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
