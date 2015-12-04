/*************************************************
* �� �� ��: spF624.c
* ������������˰��������ҵ��
*
* ��    ��: jack
* �������: 2004��06��25��
* �޸�����:
* �� �� ��:
* �޸�����:
*
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;}
#define EXTERN
#define cIdtp "1"		/*--- ֤������Ĭ��Ϊ1 ---*/
#include "public.h"
#include <errno.h>
#include "trace_log_c.h"
#include "cif_basic_inf_c.h"
#include "cif_id_code_rel_c.h"
#include "ag_unitinfo_c.h"
#include "ag_peritemmap_c.h"
#include "note_min_no_c.h"

spF624()
{ 	
	
	int ret=0;
	char execstr[256];

	char cPrdt_code[4];
	char cBeg_card_no[20];
	char cEnd_card_no[20];
	char cCard_no[20];
	char cNote_no[20];
	char cz_acno[20];
	char cUnit_no[5];
	char cNote_type[4];
	double dTx_amt = 0.00;
	double dT_amt = 0.00;
	double dTmp_amt = 0.00;
	double dTemp_amt = 0.00;

	char filename[100];
	char filename1[100];
	char fldstr[6][100];
	char cSign[2];
	char tmpstr[1024];
	FILE *fp;
	FILE *fp1;
	int i=0;
	
	long cif_no = 0;
	char xs_acno[20];
	char jz_acno[20];
	int jz_ac_seq = 0;
	long lBeg_note_no = 0;
	char cNote_tmp[16];
	char cNote_tmp1[9];
	int iLen = 0;

	long lCount = 0;
	long lRnum = 0;

	char cMessage[41];
	char cRctp[9];

	struct cif_basic_inf_c cif_basic_inf;
	struct cif_id_code_rel_c cif_id_code_rel;
	struct ag_unitinfo_c sAg_unitinfo;
	struct ag_peritemmap_c sAg_peritemmap;
	struct note_min_no_c sNote_min_no;

	memset(cPrdt_code,0x0,sizeof(cPrdt_code));
	memset(cBeg_card_no,0x0,sizeof(cBeg_card_no));
	memset(cEnd_card_no,0x0,sizeof(cEnd_card_no));
	memset(cCard_no,0x0,sizeof(cCard_no));
	memset(cNote_no,0x0,sizeof(cNote_no));
	memset(cz_acno,0x0,sizeof(cz_acno));
	memset(cUnit_no,0x0,sizeof(cUnit_no));
	memset(cNote_type,0x0,sizeof(cNote_type));

	memset(cMessage, 0x0, sizeof(cMessage) );

	get_zd_data("0230",cPrdt_code);
	get_zd_data("0920",cUnit_no);
	get_zd_data("0900",cNote_type);

	/** ��ʼ��ȫ�ֱ��� **/
	pub_base_sysinit();

	/*------------------	�ָ���	----------------*/
	memset( cSign, 0x0, sizeof(cSign) );
	cSign[0] = '\t';
	cSign[1] = '\0';
	
	memset( cRctp, 0x0, sizeof(cRctp) );
	if ( !strcmp(cNote_type, "010") ) /* ���� */
	{
		strcpy( cRctp, "����" );
	}
	if ( !strcmp(cNote_type, "020") ) /* �� */
	{
		strcpy( cRctp, "��" );
	}

	memset( &sAg_unitinfo, 0x0, sizeof(sAg_unitinfo) );
	ret=Ag_unitinfo_Sel( g_pub_tx.reply , &sAg_unitinfo ,\
			 "unit_no = '%s' and unit_stat = '0'",cUnit_no);
	if( ret==100 )
	{
		sprintf( acErrMsg, "û�в�ѯ���õ�λ��Ŷ�Ӧ��¼!\
			unit_no=[%s]", cUnit_no );
		WRITEMSG
		strcpy( g_pub_tx.reply, "H009" );
		goto	ErrExit;
	}
	else if( ret )
	{
		sprintf( acErrMsg, "ִ��Ag_unitinfo����!ret=[%d]", ret );
		WRITEMSG
		goto	ErrExit;
	}
	
	memset(filename1,0x0,sizeof(filename1));
    sprintf( filename1, "%s/Banksyyh%ld0001.txt", getenv("FILDIR"), \
		g_pub_tx.tx_date );
    fp1 = fopen( filename1,"w" );   /*���ļ�������¼������ȷ������*/
    if( fp1==NULL )
    {
       	sprintf(acErrMsg," open file [%s] error ",filename1 );
       	WRITEMSG
       	strcpy( g_pub_tx.reply,"S047" );
       	goto ErrExit;
    }
    	
    pub_base_strpack( sAg_unitinfo.unit_name );

	memset(filename,0x0,sizeof(filename));
	Gl_file_first=0;
	pub_base_AllDwnFilName( filename );

	sprintf(acErrMsg,"ȡ�õ��ļ���Ϊ[%s]",filename);
	WRITEMSG

	fp = fopen( filename,"r" );
	if( fp==NULL )
	{
		sprintf(acErrMsg,"----->�򿪿��������ļ�[%s] error ",filename );
		strcpy( g_pub_tx.reply,"S047" );
		goto ErrExit;
	}

	/*------ �жϹ�Ա�Ƿ�ӵ������������ƾ֤ -----*/
	while (1)
	{
		memset( tmpstr, 0x0, sizeof(tmpstr) );
		fgets( tmpstr,1024,fp );
		if( feof(fp) ) 
		{
			break;
		}
		
		memset(fldstr,0x0,sizeof(fldstr));
		for( i=0;i<5;i++ )
		{
			ret=pub_base_cut_str( tmpstr,fldstr[i],cSign,i+1 );
				vtcp_log(" [%d] [%s] ",i+1,fldstr[i]);
			pub_base_strpack( fldstr[i] );
		}
		/*-	fldstr[0] ˰�� ---------------------------------
			fldstr[1] ��˰����
			fldstr[2] ����
			fldstr[3] ����������
			fldstr[4] ֤������
		--------------------------------------------------*/

		lCount = lCount + 1;
	}
	fclose ( fp );
	
	ret = sql_sum_long ( "note_mst", "cnt", &lRnum, "tel = '%s' \
				and note_type = '%s' and sts = '0'", \
				g_pub_tx.tel, cNote_type );
	if ( lRnum < lCount )
	{
		sprintf(acErrMsg, "��������[%d]ƾ֤��[%d]", lCount, lRnum );
		WRITEMSG

		sprintf(acErrMsg, "��Ա����ƾ֤���������������!!!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "H018" );
		goto ErrExit;
	}

	sprintf(acErrMsg, "��������[%d]ƾ֤��[%d]", lCount, lRnum );
	WRITEMSG

	if ( lCount == 0 )
	{
		sprintf(acErrMsg,"û����Ҫ���������ļ�¼!!!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "H017" );
		goto ErrExit;
	}

	fp = fopen( filename,"r" );
	if( fp==NULL )
	{
		sprintf(acErrMsg," open file [%s] error ",filename );
		WRITEMSG
		strcpy( g_pub_tx.reply,"S047" );
		goto ErrExit;
	}

	while(1)
	{
		memset( tmpstr, 0x0, sizeof(tmpstr) );
		fgets( tmpstr,1024,fp );
		if( feof(fp) ) 
		{
			break;
		}

		memset(fldstr,0x0,sizeof(fldstr));
		for( i=0;i<5;i++ )
		{
			ret=pub_base_cut_str( tmpstr, fldstr[i], cSign, i+1 );
				vtcp_log(" [%d] [%s] ",i+1,fldstr[i]);
			pub_base_strpack( fldstr[i] );
		}
		/*-	fldstr[0] ˰�� ---------------------------------
			fldstr[1] ��˰����
			fldstr[2] ����
			fldstr[3] ����������
			fldstr[4] ֤������
		--------------------------------------------------*/
		
		/**------------ ÿ��ѭ�������³�ʼ��ȫ�ֱ��� -------------**/
		memset( &g_pub_tx, 0x00, sizeof(g_pub_tx) );
		pub_base_sysinit();
		
		dTx_amt = 0.00;

		memset( &sAg_peritemmap, 0x00, sizeof(sAg_peritemmap) );
		ret = Ag_peritemmap_Sel(g_pub_tx.reply,&sAg_peritemmap,\
			"unit_no = '%s' and item_no = '%s'", \
			cUnit_no, fldstr[0] );
		if ( ret == 0 )
		{
			/*--------��ȷ���Ƿ�������-------------------
			pub_base_strpack( sAg_peritemmap.id_no );
			if ( strcmp( sAg_peritemmap.id_no, fldstr[4] ) == 0 )
			{
				sprintf(acErrMsg,"�ÿͻ�[%s]��¼�Ѵ��ڲ������¿���", fldstr[0] );
				WRITEMSG
				goto ErrExit;
			}
			*---------------------------------------------*/
		}
		else if ( ret != 100 )
		{
			sprintf( acErrMsg,"��ѯag_peritemmap����[%s]",fldstr[0] );
			WRITEMSG
			goto ErrExit;
		}

		/*-------------��ʱ��-----------------------------
		if ( sAg_unitinfo.opr_attri[0] == '0' )
		{
			ret = pub_base_CompDblVal( dTx_amt, 0.00 );
			if ( ret != 0 )
			{
				sprintf( acErrMsg,"�����˻����������!!!" );
				WRITEMSG
				strcpy(g_pub_tx.reply,"H016");
				goto ErrExit;
			}
		}
		-----------------------------------------------*/

		memset(&cif_id_code_rel, 0x0, sizeof(cif_id_code_rel));
		memset(&cif_basic_inf, 0x0, sizeof(cif_basic_inf));

		strcpy(cif_id_code_rel.id_type, cIdtp);/* ֤������ */
		strcpy(cif_id_code_rel.id_no,fldstr[4]);  /* ֤������ */

		strcpy(cif_basic_inf.type,"1");		      /* �ͻ����� */
		strcpy(cif_basic_inf.name, fldstr[3]);	  /* �ͻ����� */
		strcpy(cif_basic_inf.local,"1");	/* ���ر�־ */
		strcpy(cif_basic_inf.poten,"1");	/* Ǳ�ڱ�־1-����2-Ǳ�� */
		strcpy(cif_basic_inf.shareholder_ind,"1");/*�Ƿ��йɶ�2-�� 1-�� */
		strcpy(cif_basic_inf.lvl,"4"); /*�ͻ�����*/

		/*--------��ʼ�����ͻ���Ϣ----------*/
		cif_no = 0;

		/* ���ͻ�֤�����ͣ�֤�������Ƿ��Ѿ����� */
		ret = Cif_id_code_rel_Sel( g_pub_tx.reply , &cif_id_code_rel , 
			" id_type='%s' and id_no='%s' " , cif_id_code_rel.id_type ,
			cif_id_code_rel.id_no );
		if( ret==0 )
		{
			sprintf(acErrMsg,"�ÿͻ��Ѿ�ӵ�пͻ�ID![%s][%s]",cIdtp,fldstr[4]);
			WRITEMSG
			cif_no = cif_id_code_rel.cif_no; /*���Ѵ��ڵĿͻ��Ÿ�ֵ*/
			goto OK_cif_no;	/*ֱ�ӿ���*/
		}
		else if( ret!=100 )
		{
			sprintf(acErrMsg,"ȡ�ͻ�֤����Ϣ�쳣![%s]",g_pub_tx.reply);
			WRITEMSG
			strcpy(g_pub_tx.reply,"C002");
			goto ErrExit;
		}
	
		ret = pub_cif_CrePer_cif_no( &cif_no );
		if ( ret )
		{
			sprintf(acErrMsg,"���ɿͻ�idʧ��!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"C003");
			goto ErrExit;
		}
	
		cif_basic_inf.cif_no = cif_no ;
		cif_id_code_rel.cif_no = cif_no ;	

		/* �Ǽǿͻ�֤����ͻ��Ŷ��ձ� */
		ret = Cif_id_code_rel_Ins( cif_id_code_rel );
		if( ret )
		{
			sprintf(acErrMsg,"�Ǽǿͻ�֤����ͻ��Ŷ��ձ����![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"C004");
			goto ErrExit;
		}
	
		/* �Ǽǿͻ�������Ϣ�� */
		cif_basic_inf.crt_date = g_pub_tx.tx_date ; /* �������� */
		cif_basic_inf.crt_time = g_pub_tx.tx_time ; /* ����ʱ�� */
		strcpy( cif_basic_inf.sts , "1" ) ;  /* �ͻ�״̬��1���� 2ע�� */
		cif_basic_inf.cif_seqn = 1 ;                /* �ͻ���� */

		ret = Cif_basic_inf_Ins( cif_basic_inf );
		if( ret )
		{
			sprintf(acErrMsg,"�Ǽǿͻ�������Ϣ�����![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"C005");
			goto ErrExit;
		}
	
		sprintf(acErrMsg,"�����ͻ���Ϣ�ɹ�,�ͻ���[%ld]",cif_no);/*���ɿͻ���*/
		WRITEMSG

OK_cif_no:

/*--------------------������------------------------*
		ret = pub_base_CrtAcChkb( cBeg_card_no, cCard_no, 3, 1);
		if( ret )
		{
			sprintf(acErrMsg,"���ɿ���[%s]У��λ����![%d]",cBeg_card_no,ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"F005");
			goto ErrExit;
		}
		*------------------------------------------*/

		memset( &sNote_min_no, 0x00, sizeof(sNote_min_no) );
		ret = Note_min_no_Sel( g_pub_tx.reply, &sNote_min_no, \
			"br_no = '%s' and tel = '%s' and note_type = '%s'", \
			g_pub_tx.tx_br_no, g_pub_tx.tel, cNote_type );
		if( ret )
		{
			sprintf( acErrMsg,"��ѯ��Ա��С����ƾ֤����[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}

		if ( !strcmp(cNote_type, "010") ) /* ���� */
		{
			strcpy(g_pub_tx.mdm_code,"0010");	
			strcpy(g_pub_tx.beg_note_no, sNote_min_no.note_min_no );
		}
		if ( !strcmp(cNote_type, "020") ) /* �� */
		{
			strcpy(g_pub_tx.mdm_code,"0020");	
			strcpy(g_pub_tx.beg_note_no, sNote_min_no.note_min_no );

			memset( cNote_no, 0x0, sizeof(cNote_no) );
			memcpy( cNote_no, sNote_min_no.note_min_no+6, 10 );

			strcpy(g_pub_tx.crd_no, "940071733" );
			strcat(g_pub_tx.crd_no, cNote_no );
		}

		strcpy(g_pub_tx.id_type, cIdtp);	
		strcpy(g_pub_tx.id_no, fldstr[4]);
		strcpy(g_pub_tx.name, fldstr[3]);

		strcpy(g_pub_tx.draw_uncon_yn,"N");
		strcpy(g_pub_tx.draw_pwd_yn,"Y");
		strcpy(g_pub_tx.qry_pwd,"000000");
		strcpy(g_pub_tx.draw_pwd,"000000");
		strcpy(g_pub_tx.draw_id_yn,"N");
		strcpy(g_pub_tx.draw_seal_yn,"N");

		strcpy(g_pub_tx.brf,"������");

		pub_base_strpack(g_pub_tx.crd_no);
		pub_base_strpack(g_pub_tx.beg_note_no);

		ret = pub_acct_opn_cif_mdm();
		if( ret )
		{
			sprintf(acErrMsg,"�����ʴ�!!![%d][%s][%s][%s]",ret, cIdtp, fldstr[4],fldstr[3]);
			WRITEMSG
			goto ErrExit;
		}
		memset( xs_acno,0x0,sizeof(xs_acno) );
		strcpy(xs_acno,g_pub_tx.ac_no);	/*������ʾ�˺�*/
		vtcp_log("��ʾ�˺�Ϊ[%s]",g_pub_tx.ac_no);

/*---------------------���˺�---------------------*/
    	memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
		memset(&g_dd_parm,0x00,sizeof(struct dd_parm_c));
		memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));

		strcpy(g_pub_tx.ac_no,xs_acno);
		strcpy(g_pub_tx.prdt_code,cPrdt_code);
		strcpy(g_pub_tx.cur_no,"01");
		strcpy(g_pub_tx.ac_type,"D");
		strcpy(g_pub_tx.intst_type,"");
		g_pub_tx.tx_amt1 = 0.00;		/*	�������Ϊ0	*/
		strcpy(g_pub_tx.name,fldstr[3]);
		strcpy(g_pub_tx.ct_ind,"2");
		strcpy(g_pub_tx.add_ind,"1");
		strcpy(g_pub_tx.brf,"������������");
		
		memset( g_pub_tx.sub_tx_code, 0x0, sizeof(g_pub_tx.sub_tx_code) );
		strcpy( g_pub_tx.sub_tx_code, "D002" );
		set_zd_data( DC_SUB_TX_CODE, g_pub_tx.sub_tx_code );

		ret =  pub_acct_opn_ac();
		if (ret != 0)
		{
			sprintf(acErrMsg,"��������[%d]!",ret);
			WRITEMSG
			goto ErrExit;
		}

		set_zd_data( "1033", "01" );
		set_zd_data( "103E", "2" );
		set_zd_double( "1037", 0.00 );
		ret = pubf_acct_proc( g_pub_tx.sub_tx_code );
		if ( ret )
		{
			sprintf(acErrMsg,"�ǻ��ڿ�������˳���[%d]!",ret);
			WRITEMSG
			goto ErrExit;
		}
	
		memset( jz_acno, 0x0, sizeof(jz_acno) );
		jz_ac_seq = 0;
		sprintf(jz_acno,"%d",g_pub_tx.ac_id);
		jz_ac_seq = g_pub_tx.ac_seqn;

		sprintf(acErrMsg,"���ɵļ����˺ź��˺���ŷֱ�Ϊ[%s] [%d]",jz_acno,g_pub_tx.ac_seqn);
		WRITEMSG

		/*--------------- д�����Ӧ��¼ ---------------*/
		memset( &sAg_peritemmap, 0x00, sizeof(sAg_peritemmap) );

		strcpy( sAg_peritemmap.bat_flag, "0" );
		strcpy( sAg_peritemmap.remark, "����˰��" );

		strcpy( sAg_peritemmap.unit_no, cUnit_no );
		strcpy( sAg_peritemmap.item_no, fldstr[0] );
		strcpy( sAg_peritemmap.name, fldstr[3] );
		strcpy( sAg_peritemmap.id_type, cIdtp );
		strcpy( sAg_peritemmap.id_no, fldstr[4] );
		strcpy( sAg_peritemmap.ac_no, xs_acno );
		sAg_peritemmap.bgn_use_date = g_pub_tx.tx_date;
		strcpy( sAg_peritemmap.stat, "0" );
		sAg_peritemmap.opn_date = g_pub_tx.tx_date;
		strcpy( sAg_peritemmap.opn_tel, g_pub_tx.tel );
		sAg_peritemmap.cls_date = 99999999;
		strcpy( sAg_peritemmap.cls_tel, "0000" );

		ret = Ag_peritemmap_Ins( sAg_peritemmap, g_pub_tx.reply );
		if (ret == -268 )	/*--- ���ظ���¼ ---*/
		{
			ret = sql_execute( "update ag_peritemmap set name = '%s', \
				id_no = '%s', ac_no = '%s',opn_date = %ld, opn_tel= '%s'\
				where unit_no = '%s' and item_no = '%s'", \
				fldstr[3], fldstr[4], xs_acno, g_pub_tx.tx_date, \
				g_pub_tx.tel, cUnit_no, fldstr[0] );
			if ( ret )
			{
				sprintf(acErrMsg,"�޸�ag_peritemmapʧ��[%s][%d]!!!",fldstr[0],ret );
				WRITEMSG
				goto ErrExit;
			}
		}
		else if ( ret )
		{
			sprintf(acErrMsg,"д��¼��Ag_peritemmapʧ��!!!!");
			WRITEMSG
			goto ErrExit;
		}

		g_pub_tx.ac_id = 0;
		g_pub_tx.ac_seqn = 0;
		g_pub_tx.tx_amt1 = 0.00;		/*������*/
		
		fprintf( fp1, "%s\t%s\t%s\t%s\t%s\t%s\t%ld\n", fldstr[0], \
			fldstr[1], fldstr[2], xs_acno, fldstr[3], fldstr[4], \
			g_pub_tx.tx_date );
	}
	
	fclose(fp);
	fclose(fp1);
	
	if ( access( filename1, 0) )
	{
		sprintf(acErrMsg,"Ҫ���ʵ��ļ�������[%s]", filename1 );
		WRITEMSG
		strcpy( g_pub_tx.reply, "S047" );
		goto ErrExit;
	}

	memset( execstr, 0x0, sizeof(execstr) );
	sprintf( execstr, "cp %s %s", filename1, filename );

	ret = system( execstr );
	if ( ret )
	{
		sprintf(acErrMsg,"�����´��ļ���ʧ��[%s][%s][%d][%s]", filename, strerror(errno), ret, execstr );
		WRITEMSG
		strcpy( g_pub_tx.reply, "S047" );
		goto ErrExit;
		
	}

	set_zd_data(DC_FILE_SND,"1");

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"���������ɹ���[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    fclose(fp);
    fclose(fp1);
    sprintf(acErrMsg,"��������ʧ�ܣ�[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
