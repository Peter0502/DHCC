/*************************************************
* �� �� ��: spF611.c
* ����������������������(�˽�����610���ƶ�����Ϊ�����winqt��������������)
*
* ��    ��: jack
* �������: 2005��02��24��
* �޸�����:
* �� �� ��:
* �޸�����:
*insert into tx_Def (TX_CODE, TX_NAME, BIT_MAP, LOG_IND, TX_TYPE, WRK_STS)
values ('6611', 'winqt������������', '10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000', '1', '6', '2');
insert into tx_flow_def (TX_CODE, FLOW_CODE, SUB_TX_CODE, SUB_TX_RSLT)
values ('6611', 0, 'F611', '#$');
insert into tx_sub_def (SUB_TX_CODE, SUB_TX_NAME, TX_FUNC, ACCT_IND, CHNL_IND)
values ('F611', 'winqt������������', 'spF611', '0', '0');
----------------------------------------------------------
insert into auth_item (TX_CODE, SEQN, CODE_AC, CODE_SEQN, CODE_AMT, CODE_DC, CODE_CT, CODE_XX, CODE_YY, CODE_ZZ, IND, MO)
values ('6611', 1, '', '', '', '', '', '', '', '', '0000', 'winqt��������');
insert into auth_cond (TYPE, NO, TX_CODE, SEQN, MO, COND)
values ('S', 0, '6611', 1, '��������������Ҫ��Ȩ', 'sq(1237)');
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;}
#define EXTERN
#define cIdtp "1"		/*--- ֤������Ĭ��Ϊ1 -�ָ�Ϊ�����κ�֤������fldstr[5]-*/
#include "public.h"
#include <errno.h>
#include "trace_log_c.h"
#include "cif_basic_inf_c.h"
#include "cif_id_code_rel_c.h"
#include "cif_addr_inf_c.h"    /****����ͻ���ַ��****/
#include "ag_unitinfo_c.h"
#include "ag_peritemmap_c.h"
#include "pay_agent_c.h"
#include "note_min_no_c.h"
#include "com_branch_c.h"

spF611()
{ 	
	
	int ret=0;
	int dRet=0;
	char execstr[256];
  /*add by kevinchan 20121027 start*/
  char tmpChar;
  /*add by kevinchan 20121027 end*/
	char cPrdt_code[4];
	char cBeg_card_no[20];
	char cEnd_card_no[20];
	char cCard_no[20];
	char cNote_no[20];
	char cz_acno[20];
	char cUnit_no[5];
	char cNote_type[4];
	/*add by wudw 20130403 start*/
	char cTrace_no[7];
	/* end */
	double dTx_amt = 0.00;
	double dTmp_amt = 0.00;
	double dTemp_amt = 0.00;	
	
	double dTram = 0.00;

	char cPass[7];
	long lLen = 0;

	char cErrname[51];  /****����黧��ʱ��������������ȷ�������ڴ˱����У����ظ�ǰ̨ 20070627****/

	char filename[100];
	char filename1[100];
	char fldstr[10][100];
	char tmpstr[1024];
	FILE *fp = NULL;
	FILE *fp1 = NULL;
	int i=0;

	char filename2[100];
	FILE *fp2 = NULL;
	char filename3[100];
	FILE *fp3 = NULL;	
	char filename4[100];	/*����ȷ�����������*/
	
	long lSnum = 0;
	double dSamt = 0.00;
	long lSnum1 = 0;
	double dSamt1 = 0.00;
	
	int iErr_flag = 0;	/*---��־������¼���Ƿ��д������---*/
	char cReturn[64];

	long cif_no = 0;
	char xs_acno[20];
	char jz_acno[20];
	int jz_ac_seq = 0;
	long lBeg_note_no = 0;
	char cNote_tmp[16];
	char cNote_tmp1[9];
	int iLen = 0;

	long lCount = 0;
	double dBd_amt=0.00;
	
	char cHand_flag[2];	/*---��Ϣ¼�뷽ʽ---*/
	double dT_amt = 0.00;	/*---ǰ̨�����ܽ��---*/
	long lCount_i = 0;	/*---ǰ̨�����ܱ���---*/
	long lRnum = 0;

	char cRctp[9];

	struct cif_basic_inf_c cif_basic_inf;
	struct cif_id_code_rel_c cif_id_code_rel;
	struct cif_addr_inf_c cif_addr_inf;
	struct ag_unitinfo_c sAg_unitinfo;
	struct ag_peritemmap_c sAg_peritemmap;
	struct pay_agent_c sPay_agent;
	struct note_min_no_c sNote_min_no;
	struct com_branch_c sCom_branch;
	struct cif_basic_inf_c t_cif_basic_inf;
	memset(&cif_addr_inf,0x0,sizeof(cif_addr_inf));

	memset(cPrdt_code,0x0,sizeof(cPrdt_code));
	memset(cBeg_card_no,0x0,sizeof(cBeg_card_no));
	memset(cEnd_card_no,0x0,sizeof(cEnd_card_no));
	memset(cCard_no,0x0,sizeof(cCard_no));
	memset(cNote_no,0x0,sizeof(cNote_no));
	memset(cz_acno,0x0,sizeof(cz_acno));
	memset(cUnit_no,0x0,sizeof(cUnit_no));
	memset(cNote_type,0x0,sizeof(cNote_type));
	memset( cReturn, 0x0, sizeof(cReturn) );
	memset( cHand_flag, 0x0, sizeof(cHand_flag) );

	memset( cPass, 0x0, sizeof(cPass) );

	get_zd_data("0230",cPrdt_code);
	get_zd_data("0300",cz_acno);
	pub_base_old_acno( cz_acno );

	get_zd_data("0920",cUnit_no);
	get_zd_data("0900",cNote_type);
	
	get_zd_data( "0700", cHand_flag );

	get_zd_data( "0790", cPass );
	lLen = strlen( cPass );
	if ( lLen == 0 )
	{
		sprintf( acErrMsg, "���벻��Ϊ��!!!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "H034" );
		set_zd_data( "0130", "���벻��Ϊ��!!!" );
		goto ErrExit;
	}

	/** ��ʼ��ȫ�ֱ��� **/
	pub_base_sysinit();
	
	memset( cRctp, 0x0, sizeof(cRctp) );
	if ( !strcmp(cNote_type, "010") ) /* ���� */
	{
		strcpy( cRctp, "����" );
	}
	if ( !strcmp(cNote_type, "011") ) /* ������������ */
	{
		strcpy( cRctp, "��������" );
	}
	if ( !strcmp(cNote_type, "012") ) /* �ؿ����� */
	{
		strcpy( cRctp, "�ؿ��浥" );
	}
	if ( !strcmp(cNote_type, "013") ) /* ���ڴ浥 */
	{
		strcpy( cRctp, "���ڴ浥" );
	}
	if ( !strcmp(cNote_type, "015") ) /* ����浥 */
	{
		strcpy( cRctp, "����浥" );
	}
	if ( !strcmp(cNote_type, "016") ) /* ����һ��ͨ */
	{
		strcpy( cRctp, "һ��ͨ" );
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
	pub_base_strpack( sAg_unitinfo.unit_name );
	
	memset( filename1, 0x0, sizeof(filename1) );
	/**sprintf( filename1,"%s/0%s%ld%s.txt",getenv("FILDIR"),cUnit_no,g_pub_tx.tx_date,g_pub_tx.tel); **/ /**Del by hxc 100529 �ļ������6λ��ˮ��**/
    	sprintf( filename1,"%s/0%s%ld%s%06d.txt",getenv("FILDIR"),cUnit_no,g_pub_tx.tx_date,g_pub_tx.tel,g_pub_tx.trace_no);
    	fp1 = fopen( filename1,"w" );   /*���ļ�������¼������ȷ������*/
    	if( fp1==NULL )
    	{
        	sprintf(acErrMsg," open file [%s] error ",filename1 );
        	WRITEMSG
        	strcpy( g_pub_tx.reply,"S047" );
        	goto ErrExit;
    	}    	
    	
    	fprintf( fp1, "\r\n\r\n\r\n                            ���������ɹ������Ϣ��[%s-%s]-[%s-%ld]\r\n", \
    		sAg_unitinfo.unit_no, sAg_unitinfo.unit_name,g_pub_tx.tel,g_pub_tx.tx_date );
    	fprintf( fp1, "   ���������������ө����������������ө����������������ө��������������������ө��������������������ө��������ө�����������������\r\n" );
    	fprintf( fp1, "   ��%12s��%16s��%16s��%20s��%20s��%8s��%16s��\r\n", "������", "�ͻ�����", "�������", "֤������", "�˺�", "ƾ֤����", "ƾ֤��" );
    	fprintf( fp1, "   ���������������㩥���������������㩥���������������㩥�������������������㩥�������������������㩥�������㩥����������������\r\n" );
    	
    	memset( filename2, 0x0, sizeof(filename2) );
    	/**sprintf( filename2,"%s/1%s%ld%s.txt",getenv("FILDIR"),cUnit_no,g_pub_tx.tx_date,g_pub_tx.tel); **/ /**Del by hxc 100529 �ļ������6λ��ˮ��**/
    	sprintf( filename2,"%s/1%s%ld%s%06d.txt",getenv("FILDIR"),cUnit_no,g_pub_tx.tx_date,g_pub_tx.tel,g_pub_tx.trace_no);
    	fp2 = fopen( filename2,"w" );   /*���ļ�������¼������������*/
    	if( fp2==NULL )
    	{
        	sprintf( acErrMsg, " open file [%s] error ", filename2 );
        	WRITEMSG
        	strcpy( g_pub_tx.reply,"S047" );
        	goto ErrExit;
    	}
    	
	fprintf( fp2, "\r\n\r\n\r\n                            ����������������Ϣ��[%s-%s]-[%s-%ld]\r\n", \
    		sAg_unitinfo.unit_no, sAg_unitinfo.unit_name,g_pub_tx.tel,g_pub_tx.tx_date );
    	fprintf( fp2, "   ���������������ө����������������ө����������������ө��������������������ө���������������������������������������������\r\n" );
    	fprintf( fp2, "   ��%12s��%16s��%16s��%20s��%44s��\r\n", "������", "�ͻ�����", "�������", "֤������", "������Ϣ" );
    	fprintf( fp2, "   ���������������㩥���������������㩥���������������㩥�������������������㩥��������������������������������������������\r\n" );


	memset(filename,0x0,sizeof(filename));
	Gl_file_first=0;
	pub_base_AllDwnFilName( filename );
	/*strcat(filename,"_610");*/

	sprintf(acErrMsg,"ȡ�õ��ļ���Ϊ[%s]",filename);
	WRITEMSG

	fp = fopen( filename,"r" );
	if( fp==NULL )
	{
		sprintf( acErrMsg," open file [%s] error ", filename );
		strcpy( g_pub_tx.reply, "S047" );
		goto ErrExit;
	}

	if ( cHand_flag[0] == '0' )	/*---�ֹ�---*/
	{
		get_zd_double("0400",&dT_amt);
		get_zd_long( "0550", &lCount_i );
	}
	else				/*---���ֹ���ʽ�ļ���һ��Ϊ������Ϣ---*/
	{
		memset( tmpstr, 0x0, sizeof(tmpstr) );
		fgets( tmpstr,1024,fp );
		
		memset(fldstr,0x0,sizeof(fldstr));
		for( i=0;i<8;i++ )
		{
			ret=pub_base_cut_str( tmpstr,fldstr[i],"|",i+1 );
				vtcp_log(" [%d] [%s] ",i+1,fldstr[i]);
			pub_base_strpack( fldstr[i] );
		}
		if ( strncmp( cUnit_no, fldstr[3], 4 ) != 0 )
		{
			sprintf( acErrMsg, "����λ����[%s]��Ϣ����", fldstr[3] );
        		WRITEMSG
        		strcpy( g_pub_tx.reply, "H035" );
        		goto ErrExit;
		}
		
		dT_amt = atof( fldstr[7] );
		lCount_i = atoi(fldstr[6] );
	
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
		/****������7��˴�Ӧ�ø�Ϊ7 ****/
		for( i=0;i<7;i++ )
		{
			ret=pub_base_cut_str( tmpstr,fldstr[i],"|",i+1 );
				vtcp_log(" [%d] [%s] ",i+1,fldstr[i]);
			pub_base_strpack( fldstr[i] );
		}
		
		/*-	fldstr[0] ������ ------------------------
			fldstr[1] �ͻ�����
			fldstr[2] �������
			fldstr[3] ���֤����
		--------------------------------------------------*/
		vtcp_log("[%s][%d]֤������[%s]\r\n",__FILE__,__LINE__,fldstr[5]);
		if ( strlen(fldstr[1]) == 0 || strlen(fldstr[3]) == 0 )
		{
			continue;	/*�����κ�һ��Ϊ����Ϊ�ü�¼��Ч*/
		}
		
		dTemp_amt = 0.00;
		dTemp_amt = atof( fldstr[2] );
		dTmp_amt = dTmp_amt + dTemp_amt;

		lCount = lCount + 1;
	}
	fclose ( fp );
	
	ret = pub_base_CompDblVal( dT_amt, dTmp_amt);
	if ( ret )
	{
		sprintf(acErrMsg,"�ܽ����ֻ����׽��ϼƲ����!!!" );
		WRITEMSG
		sprintf(acErrMsg,"��ϸ���Ϊ[%.2lf]", dTmp_amt);
		set_zd_data( "0130", acErrMsg );
		strcpy( g_pub_tx.reply, "H019" );
		goto ErrExit;
	}
	
	if ( lCount != lCount_i )
	{
		sprintf(acErrMsg,"�ܱ�����ֻ���ϸ�ϼƱ��������!!!" );
		WRITEMSG
		sprintf(acErrMsg,"��ϸ����Ϊ[%d]", lCount );
		set_zd_data( "0130", acErrMsg );
		strcpy( g_pub_tx.reply, "H019" );
		goto ErrExit;
	}

	if ( strcmp( cPrdt_code, "102" )!= 0 )		/*102��ƷΪ�޽��ʻ�*/
	{
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
	}

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
	
	if ( cHand_flag[0] != '0' )	/*---���ֹ�ȥ����һ��---*/
	{
		memset( tmpstr, 0x0, sizeof(tmpstr) );
		fgets( tmpstr, 1024, fp );
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
		for( i=0;i<7;i++ )
		{
			ret=pub_base_cut_str( tmpstr,fldstr[i],"|",i+1 );
				vtcp_log(" [%d] [%s] ",i+1,fldstr[i]);
			pub_base_strpack( fldstr[i] );

				vtcp_log("[%s][%d]ѭ��i[%s]\r\n",__FILE__,__LINE__,fldstr[i]);
		}
		/*-	fldstr[0] ������ ----------------------------
			fldstr[1] �ͻ�����
			fldstr[2] �������
			fldstr[3] ���֤����
			fldstr[4] ��λ���
			fldstr[5] ֤������
		--------------------------------------------------*/
				vtcp_log("[%s][%d]֤������[%s]\r\n",__FILE__,__LINE__,fldstr[5]);
		pub_base_ltrim_zero( fldstr[0] );	/*---ȥ�����˱����ߵ���---*/
		if ( strlen(fldstr[0]) == 0 )
		{
			strcpy( fldstr[0], "0" );
		}
		
		/*--------�������š��ͻ����������֤����Ϊ��ʱ����ѭ��--------*/
		if ( strlen(fldstr[1]) == 0 || strlen(fldstr[3]) == 0 )
		{
			continue;	/*�����κ�һ��Ϊ����Ϊ�ü�¼��Ч*/
		}
				
		/**------------ ÿ��ѭ�������³�ʼ��ȫ�ֱ��� -------------**/
		memset( &g_pub_tx, 0x00, sizeof(g_pub_tx) );
		pub_base_sysinit();
		
		dTx_amt = 0.00;
		dTx_amt = atof(fldstr[2]);

		pub_base_strpack_all( fldstr[1] );
    		vtcp_log("[%s][%d]֤������[%s]\r\n",__FILE__,__LINE__,fldstr[5]);
		/*----��֤�������ĵ�λ���� �� excel��ÿ����λ�����Ƿ�һ��----*/
		if ( strncmp( cUnit_no, fldstr[4], 4 ) != 0 )
		{
				iErr_flag = 1;
				ret = 1;
				dT_amt = dT_amt - dTx_amt;
				pub_base_strpack( sAg_peritemmap.item_no );
				sprintf( cReturn, "excel��λ����[%s]��¼��ĵ�λ����[%s]",fldstr[4],cUnit_no );
				sprintf( acErrMsg, "excel��λ����[%s]��¼��ĵ�λ����[%s]",fldstr[4],cUnit_no);				
				WRITEMSG
				
				goto TrLog;
		}
		memset( &sAg_peritemmap, 0x00, sizeof(sAg_peritemmap) );
		ret = Ag_peritemmap_Sel(g_pub_tx.reply,&sAg_peritemmap,\
			"unit_no = '%s' and item_no = '%s'", \
			cUnit_no, fldstr[0] );
		if ( ret == 0 )
		{
			/*----ͬһ��λ��� ��������˴������ظ�----*/

				iErr_flag = 1;
				ret = 1;
				dT_amt = dT_amt - dTx_amt;
				pub_base_strpack( sAg_peritemmap.item_no );
				sprintf( cReturn, " ���[%s]�ظ�", sAg_peritemmap.item_no );
				/** 
				strcpy( g_pub_tx.reply, "H033" ); 
				**/
				sprintf( acErrMsg, "���[%s]�ظ�", sAg_peritemmap.item_no );				
				WRITEMSG
				
				goto TrLog;
			
		}	
			
			/*----��������֤��1��֤�����Ȳ���18 ��ʧ�� ������������Ͳ��ܳ���20λ ��Ϊcif_id_code_rel�ĳ���Ϊchar[20]----*/
			pub_base_strpack(fldstr[3]);
			pub_base_strpack(fldstr[5]);
			if(strcmp(fldstr[5],"1") == 0)
			{
				if(strlen(fldstr[3]) != 18 )
				{
					iErr_flag = 1;
					ret = 1;
					dT_amt = dT_amt - dTx_amt;
					sprintf( cReturn, "���֤[%s]��18λ", fldstr[3]);
					sprintf( acErrMsg, "���֤[%s]����18λ", fldstr[3] );				
					WRITEMSG
					
					goto TrLog;
				}
			}
			else
			{
				if(strlen(fldstr[3]) > 20 )
				{
					iErr_flag = 1;
					ret = 1;
					dT_amt = dT_amt - dTx_amt;
					sprintf( cReturn, "֤��[%s]������20λ", fldstr[3]);
					sprintf( acErrMsg, "֤��[%s]������20λ", fldstr[3] );				
					WRITEMSG
					
					goto TrLog;
				}
			}
			/*----ͬһ��Ŷ�Ӧ�µ����֤�����Ѿ�������������----*/
			/** ������Ҫ��ȥ�����֤�����ظ�����֤ modify by wudawei 20130603
			memset( &sAg_peritemmap, 0x00, sizeof(sAg_peritemmap) );
		  ret = Ag_peritemmap_Sel(g_pub_tx.reply,&sAg_peritemmap,\
			"unit_no = '%s' and id_no = '%s'", \
			cUnit_no, fldstr[3] );
		if ( ret == 0 )
		{
				
				pub_base_strpack( sAg_peritemmap.id_no );

				iErr_flag = 1;
				ret = 1;
				dT_amt = dT_amt - dTx_amt;
				pub_base_strpack( sAg_peritemmap.ac_no );
				sprintf( cReturn, "[%s]�Ѿ��ڴ˵�λ������", sAg_peritemmap.id_no );
				sprintf( acErrMsg, "���֤[%s]�Ѿ��ڴ˵�λ�¿��������������¿���", sAg_peritemmap.id_no );				
				WRITEMSG
				
				goto TrLog;
			
		
		}
		else if ( ret != 100 )
		{
			sprintf( acErrMsg,"��ѯag_peritemmap����[%s]",fldstr[0] );
			WRITEMSG
			goto ErrExit;
		}
		**/
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
			memset(&t_cif_basic_inf, 0x0, sizeof(t_cif_basic_inf));
     /**modified by ligl 2006-10-13 10:42**/
		strcpy(cif_id_code_rel.id_type, fldstr[5]);/* ֤������ */
		strcpy(cif_id_code_rel.id_no,fldstr[3]);  /* ֤������ */
    		vtcp_log("[%s][%d]֤������[%s]\r\n",__FILE__,__LINE__,fldstr[5]);

		strcpy(cif_basic_inf.type,"1");		      /* �ͻ����� */
		strcpy(cif_basic_inf.name,fldstr[1]);	  /* �ͻ����� */
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
			sprintf(acErrMsg,"�ÿͻ��Ѿ�ӵ�пͻ�ID![%s][%s]",fldstr[5],fldstr[3]);
			WRITEMSG
			/****���ӶԻ������ж�****/
			dRet = Cif_basic_inf_Sel(g_pub_tx.reply, &t_cif_basic_inf,"cif_no = %ld ",cif_id_code_rel.cif_no);
			if(dRet == 0)
			{
				pub_base_strpack(fldstr[1]);
				pub_base_strpack(t_cif_basic_inf.name);				
				if(strcmp(t_cif_basic_inf.name,fldstr[1]) != 0)
				{
					iErr_flag = 1;
					ret = 1;
					dT_amt = dT_amt - dTx_amt;
					/****�˴���cErrname������������Ĵ�����Ϣ����13�򷵻ظ�ǰ̨ 20070627****/
					sprintf(cErrname,"���ݿ���Ϊ [%s] �ļ���Ϊ [%s]\r\n", t_cif_basic_inf.name,fldstr[1]);
					WRITEMSG
					set_zd_data(DC_GET_MSG,cErrname);
					/**
					strcpy(g_pub_tx.reply,"C086");
					goto ErrExit;
					**/
					sprintf( cReturn, "���ݿ���Ϊ[%s],�ļ���Ϊ[%s]", t_cif_basic_inf.name,fldstr[1]);
					goto TrLog;
				}
				else
				{
					goto OK_cif_no;	/*ֱ�ӿ���*/
				}
			}
			else if( dRet )
			{
				sprintf(acErrMsg,"ȡ�ͻ���Ϣ�쳣![%s]",g_pub_tx.reply);
				WRITEMSG
				strcpy(g_pub_tx.reply,"C006");
				goto ErrExit;
			}
			cif_no = cif_id_code_rel.cif_no; /*���Ѵ��ڵĿͻ��Ÿ�ֵ*/
			/****/
			/*g_pub_tx.cif_no=cif_no;*/
			/*******/
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
		ret = Cif_id_code_rel_Ins( cif_id_code_rel,g_pub_tx.reply );
		if( ret )
		{
			vtcp_log("[%s][%d]�ͻ���[%d]\r\n",__FILE__,__LINE__,cif_id_code_rel.cif_no);
			vtcp_log("[%s][%d]�ͻ�֤������[%s]\r\n",__FILE__,__LINE__,cif_id_code_rel.id_type);
			vtcp_log("[%s][%d]�ͻ�֤����[%s]\r\n",__FILE__,__LINE__,cif_id_code_rel.id_no);
			sprintf(acErrMsg,"�Ǽǿͻ�֤����ͻ��Ŷ��ձ����![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"C004");
			goto ErrExit;
		}
	
		/* �Ǽǿͻ�������Ϣ�� */
		cif_basic_inf.crt_date = g_pub_tx.tx_date ; /* �������� */
		cif_basic_inf.crt_time = g_pub_tx.tx_time ; /* ����ʱ�� */
		strcpy( cif_basic_inf.sts , "1" ) ;         /* �ͻ�״̬��1���� 2ע�� */
		cif_basic_inf.cif_seqn = 1 ;                /* �ͻ���� */

		ret = Cif_basic_inf_Ins( cif_basic_inf,g_pub_tx.reply );
		if( ret )
		{
			sprintf(acErrMsg,"�Ǽǿͻ�������Ϣ�����![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"C005");
			goto ErrExit;
		}
		/****/
		/*g_pub_tx.cif_no=cif_no;*/
		/*******/
				vtcp_log("[%s][%d]֤������[%s]\r\n",__FILE__,__LINE__,fldstr[5]);

		sprintf(acErrMsg,"�����ͻ���Ϣ�ɹ�,�ͻ���[%ld]",cif_no);/*���ɿͻ���*/
		WRITEMSG

		/****���̿���ʱ,���Ӳ���ͻ������Ϣ,��ַ�ֶβ����ַ���ͱ���  add by wanglei 20070618****/

		vtcp_log("[%s][%d]�ͻ���ַ���ͱ��� [%s]\r\n",__FILE__,__LINE__,fldstr[6]);
		if ( cHand_flag[0] != '0' )	/*---����---*/
		{
			vtcp_log("[%s][%d]�ͻ���ַ���ͱ��� [%s]\r\n",__FILE__,__LINE__,fldstr[6]);
		
			cif_addr_inf.cif_no       = cif_no; /* �ͻ��� */
			cif_addr_inf.addr_seqn    = 1;      /* ��ַ��� */
			cif_addr_inf.addr_type[0] = '2';    /* ��ַ���� 1-��λ 2-���� 3-���� */
			strcpy(cif_addr_inf.country, "01");   /* ���� �й� */
			strcpy(cif_addr_inf.addr,fldstr[6] );    /* ��ַ���ͱ��� */
			vtcp_log("[%s][%d]�ͻ���ַ���ͱ��� [%s] addr = [%s]\r\n",__FILE__,__LINE__,fldstr[6],cif_addr_inf.addr);

			ret = Cif_addr_inf_Ins( cif_addr_inf,g_pub_tx.reply );
			if( ret )
			{
				sprintf(acErrMsg,"�Ǽǿͻ���ַ��Ϣ�����![%d]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"C008");
				goto ErrExit;
			}
			vtcp_log("[%s][%d]�ͻ���ַ���ͱ��� [%s] addr = [%s]\r\n",__FILE__,__LINE__,fldstr[6],cif_addr_inf.addr);
		}
		/****ended  20070618****/

OK_cif_no:
		memset( &sNote_min_no, 0x00, sizeof(sNote_min_no) );
		ret = Note_min_no_Sel( g_pub_tx.reply, &sNote_min_no, \
			"br_no = '%s' and tel = '%s' and note_type = '%s'", \
			g_pub_tx.tx_br_no, g_pub_tx.tel, cNote_type );
		if( ret==100 )
		{
			ret = sql_min_string("note_mst","beg_note_no" ,
				sNote_min_no.note_min_no , 16, 
				"br_no='%s' and tel='%s' and note_type='%s' and sts='0'",
				g_pub_tx.tx_br_no, g_pub_tx.tel, cNote_type ) ;
			if( ret )
			{
				sprintf( acErrMsg,"��ѯ��Ա��С����ƾ֤����[%d]", ret );
				WRITEMSG
				strcpy( g_pub_tx.reply,"N012" );
				goto ErrExit;
			}
			sprintf( acErrMsg,"ƾ֤[%s]",sNote_min_no.note_min_no );
			WRITEMSG
			pub_base_strpack( sNote_min_no.note_min_no );
			if( strlen(sNote_min_no.note_min_no)==0 )
			{
				sprintf( acErrMsg,"��ѯ��Ա��С����ƾ֤����[%d]", ret );
				WRITEMSG
				strcpy( g_pub_tx.reply,"N012" );
				goto ErrExit;
			}
			strcpy( sNote_min_no.br_no,g_pub_tx.tx_br_no );
			strcpy( sNote_min_no.tel,g_pub_tx.tel );
			strcpy( sNote_min_no.note_type,cNote_type );
			
			ret=Note_min_no_Ins( sNote_min_no,g_pub_tx.reply );
			if( ret )
			{
				sprintf( acErrMsg,"��ѯ��Ա��С����ƾ֤����[%d]", ret );
				WRITEMSG
				strcpy( g_pub_tx.reply,"N012" );
				goto ErrExit;
			}
		}
		else if( ret==0 )
		{
			pub_base_strpack( sNote_min_no.note_min_no );
			if( strlen(sNote_min_no.note_min_no)==0 )
			{
				pub_base_strpack( sNote_min_no.note_min_no );
				ret = sql_min_string("note_mst","beg_note_no" ,
					sNote_min_no.note_min_no , 16, 
					"br_no='%s' and tel='%s' and note_type='%s' and sts='0'",
					g_pub_tx.tx_br_no, g_pub_tx.tel, cNote_type ) ;
				if( ret )
				{
					sprintf( acErrMsg,"��ѯ��Ա��С����ƾ֤����[%d]", ret );
					WRITEMSG
					strcpy( g_pub_tx.reply,"N012" );
					goto ErrExit;
				}
				sprintf( acErrMsg,"ƾ֤[%s]",sNote_min_no.note_min_no );
				WRITEMSG
				pub_base_strpack( sNote_min_no.note_min_no );
				if( strlen(sNote_min_no.note_min_no)==0 )
				{
					sprintf( acErrMsg,"��ѯ��Ա��С����ƾ֤����[%d]", ret );
					WRITEMSG
					strcpy( g_pub_tx.reply,"N012" );
					goto ErrExit;
				}
				strcpy( sNote_min_no.br_no,g_pub_tx.tx_br_no );
				strcpy( sNote_min_no.tel,g_pub_tx.tel );
				strcpy( sNote_min_no.note_type,cNote_type );

				ret=sql_execute( "update note_min_no set note_min_no='%s' where br_no='%s' and tel='%s' and note_type='%s'",
					sNote_min_no.note_min_no, g_pub_tx.tx_br_no, 
					g_pub_tx.tel, cNote_type );
				if( ret )
				{
					sprintf( acErrMsg,"��ѯ��Ա��С����ƾ֤����[%d]", ret );
					WRITEMSG
					strcpy( g_pub_tx.reply,"N012" );
					goto ErrExit;
				}
			}
		}
		else if( ret )
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
		if ( !strcmp(cNote_type, "011") ) /* ������������ */
		{
			strcpy(g_pub_tx.mdm_code,"0011");
			strcpy(g_pub_tx.beg_note_no, sNote_min_no.note_min_no );
		}
		if ( !strcmp(cNote_type, "012") ) /* �ؿ��浥 */
		{
			strcpy(g_pub_tx.mdm_code,"0012");
			strcpy(g_pub_tx.beg_note_no, sNote_min_no.note_min_no );
		}
		if ( !strcmp(cNote_type, "013") ) /* �����������ڴ浥 */
		{
			strcpy(g_pub_tx.mdm_code,"0013");
			strcpy(g_pub_tx.beg_note_no, sNote_min_no.note_min_no );
		}
		if ( !strcmp(cNote_type, "015") ) /* ������˶���浥 */
		{
			strcpy(g_pub_tx.mdm_code,"0015");
			strcpy(g_pub_tx.beg_note_no, sNote_min_no.note_min_no );
		}
		if ( !strcmp(cNote_type, "016") ) /* ����һ��ͨ */
		{
			strcpy(g_pub_tx.mdm_code,"0016");
			strcpy(g_pub_tx.beg_note_no, sNote_min_no.note_min_no );
		}
		if ( !strcmp(cNote_type, "020") ) /* �� */
		{
			strcpy(g_pub_tx.mdm_code,"0020");	
			strcpy(g_pub_tx.beg_note_no, sNote_min_no.note_min_no );

			memset( cCard_no, 0x0, sizeof(cCard_no) );
			memset( cNote_no, 0x0, sizeof(cNote_no) );
			memcpy( cNote_no, sNote_min_no.note_min_no+9, 7 );
			
			sprintf( acErrMsg,"-------->���ɵĿ��ŵ�ƾ֤��Ϊ[%s]", cNote_no );
        		WRITEMSG

			strcpy( cCard_no, "94007173308" );
			strcat( cCard_no, cNote_no );
			strcat( cCard_no, "0" );
			
			if( pub_card_par_1212( 1, cCard_no, g_pub_tx.crd_no ) )
    			{
        			sprintf( acErrMsg,"���ɿ���[%s]У��λ����", cCard_no );
        			WRITEMSG
        			strcpy( g_pub_tx.reply,"F005" );
        			goto ErrExit;
    			}
    			
    			sprintf( acErrMsg, "-------->���ɵĿ���Ϊ[%s]", g_pub_tx.crd_no );
        		WRITEMSG
		}
		
		if ( strcmp( cPrdt_code, "102" ) == 0 )		/*102��ƷΪ�޽��ʻ�*/
		{
			strcpy( g_pub_tx.mdm_code, "0000" );
			memset( g_pub_tx.beg_note_no, 0x0, sizeof(g_pub_tx.beg_note_no) );
			strcpy( g_pub_tx.ac_wrk_ind, "0" );	/*--����Ҫ��ƾ֤���Ŵ���--*/
		}

		strcpy(g_pub_tx.id_type, fldstr[5]);	
		strcpy(g_pub_tx.id_no,fldstr[3]);
		strcpy(g_pub_tx.name,fldstr[1]);

		strcpy(g_pub_tx.draw_uncon_yn,"N");
		strcpy(g_pub_tx.draw_pwd_yn,"Y");
		strcpy(g_pub_tx.qry_pwd, cPass );
		strcpy(g_pub_tx.draw_pwd, cPass );
		strcpy(g_pub_tx.draw_id_yn,"N");
		strcpy(g_pub_tx.draw_seal_yn,"N");

		strcpy(g_pub_tx.brf,"������");

		pub_base_strpack(g_pub_tx.crd_no);
		pub_base_strpack(g_pub_tx.beg_note_no);
		
		memset( g_pub_tx.sub_tx_code, 0x0, sizeof(g_pub_tx.sub_tx_code) );
		strcpy( g_pub_tx.sub_tx_code, "D002" );
		set_zd_data( DC_SUB_TX_CODE, g_pub_tx.sub_tx_code );

		ret = pub_acct_opn_cif_mdm();
		if( ret )
		{
			sprintf(acErrMsg,"�����ʴ�!!![%d][%s][%s][%s]",ret, fldstr[5], fldstr[3],fldstr[1]);
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
		g_pub_tx.tx_amt1 = dTx_amt;		/*������*/
		strcpy(g_pub_tx.name,fldstr[1]);
		strcpy(g_pub_tx.ct_ind,"2");
		strcpy(g_pub_tx.add_ind,"1");
		strcpy(g_pub_tx.hst_ind,"1");	/*�ռ�����ϸ*/
		strcpy(g_pub_tx.brf,"��������");
		strcpy( g_pub_tx.ac_wrk_ind, "001" );		
		
		if ( strcmp( cPrdt_code, "102" ) == 0 )		/*102��ƷΪ�޽��ʻ�*/
		{
			strcpy( g_pub_tx.mdm_code, "0000" );
		}
		
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
		set_zd_double( "1037", dTx_amt );
		dBd_amt+=dTx_amt;
		strcpy( g_pub_tx.brf, "��������" );
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

		pub_base_strpack( sAg_unitinfo.pay_type );
		if ( strncmp( sAg_unitinfo.pay_type, "31", 2 ) == 0  \
			&& strcmp( fldstr[0], "���������¿���" ) == 0 )
		{
			strcpy( fldstr[0], xs_acno );
		}
		pub_base_strpack( fldstr[0] );

		/*--------------- д�����Ӧ��¼ ---------------*/
		memset( &sAg_peritemmap, 0x00, sizeof(sAg_peritemmap) );

		if ( sAg_unitinfo.opr_attri[0] == '0' )	/* ���� */
		{
			strcpy( sAg_peritemmap.bat_flag, "0" );		/*-- �Ƿ�Ĭ������������Ҫȷ��??? --*/
			strcpy( sAg_peritemmap.remark, "����ɷ�" );
		}
		if ( sAg_unitinfo.opr_attri[0] == '1' )	/* ���� */
		{
			strcpy( sAg_peritemmap.bat_flag, "1" );
			strcpy( sAg_peritemmap.remark, "������" );
		}

		strcpy( sAg_peritemmap.unit_no, cUnit_no );
		strcpy( sAg_peritemmap.item_no, fldstr[0] );
		strcpy( sAg_peritemmap.name, fldstr[1] );
		strcpy( sAg_peritemmap.id_type, fldstr[5] );
		strcpy( sAg_peritemmap.id_no, fldstr[3] );
		strcpy( sAg_peritemmap.ac_no, xs_acno );
		sAg_peritemmap.bgn_use_date = g_pub_tx.tx_date;
		strcpy( sAg_peritemmap.stat, "0" );
		sAg_peritemmap.opn_date = g_pub_tx.tx_date;
		strcpy( sAg_peritemmap.opn_tel, g_pub_tx.tel );
		sAg_peritemmap.cls_date = 99999999;
		strcpy( sAg_peritemmap.cls_tel, "0000" );
		/*add by kevinchan 20121026 start*/
    Ag_peritemmap_Debug(&sAg_peritemmap);
    /*add by kevinchan 20121026 end*/
    ret = Ag_peritemmap_Ins( sAg_peritemmap, g_pub_tx.reply );
		if (ret == -268 )	/*--- ���ظ���¼ ---*/
		{
			ret = sql_execute( "update ag_peritemmap set name = '%s', \
				id_no = '%s', ac_no = '%s',opn_date = %ld, opn_tel= '%s'\
				where unit_no = '%s' and item_no = '%s'", \
				fldstr[1], fldstr[3], xs_acno,g_pub_tx.tx_date, \
				g_pub_tx.tel, cUnit_no, fldstr[0] );
			if ( ret )
			{
				sprintf(acErrMsg,"�޸�ag_peritemmapʧ��[%s][%d]!!!",fldstr[0],ret );
				WRITEMSG
				strcpy( g_pub_tx.reply, "P090" );
				goto ErrExit;
			}
		}
		else if ( ret )
		{
			/** 
			sprintf(acErrMsg,"д��¼��Ag_peritemmapʧ��!!!!");
			WRITEMSG
			goto ErrExit;
			**/
			/** ������Ҫ������Ա����������ʾ  **/
			sprintf(acErrMsg,"д��¼��Ag_peritemmapʧ��!!!!Ψһ��ֵ�ظ���");
			WRITEMSG
			sprintf( acErrMsg, "   ���˴������ظ������飡" );
			set_zd_data( "0130", acErrMsg );
			fclose( fp );
  	  fclose( fp1 );
    	fclose( fp2 );
			strcpy( g_pub_tx.reply, "D108" );
			set_zd_data( DC_REPLY, g_pub_tx.reply );
   		return 1;
   		/** ADD END **/
		}

		g_pub_tx.ac_id = 0;
		g_pub_tx.ac_seqn = 0;
		g_pub_tx.tx_amt1 = 0.00;		/*������*/

TrLog:		
		dTram = atof( fldstr[2] );

		if ( ret == 0 )
		{
			if ( strcmp( cPrdt_code, "102" ) == 0 )		/*102��ƷΪ�޽��ʻ�*/
			{				
				memset( sNote_min_no.note_min_no, 0x0, sizeof(sNote_min_no.note_min_no) );
				memset( cRctp, 0x0, sizeof(cRctp) );
				strcpy( cRctp, "�޽���" );
			}
			fprintf( fp1, "   ��%12s��%16s��%16.2f��%20s��%20s��%8s��%16s��\r\n", fldstr[0], fldstr[1], \
				dTram, fldstr[3], xs_acno, cRctp, sNote_min_no.note_min_no );
				
			lSnum = lSnum + 1;
			dSamt = dSamt + dTram;
		}
		else
		{
			fprintf( fp2, "   ��%12s��%16s��%16.2f��%20s��%44s��\r\n", fldstr[0], fldstr[1], \
				dTram, fldstr[3], cReturn );
			lSnum1 = lSnum1 + 1;
			dSamt1 = dSamt1 + dTram;
		}
	}

	fprintf( fp1, "   ���������������㩥���������������㩥���������������㩥�������������������㩥�������������������㩥�������㩥����������������\r\n" );
	fprintf( fp1, "   ��%12s��%16d��%16.2f��%20s��%20s��%8s��%16s��\r\n", "�ϼ�", lSnum, \
				dSamt, "", "", "", "" );
	fprintf( fp1, "   ���������������۩����������������۩����������������۩��������������������۩��������������������۩��������۩�����������������\f" );
	fprintf( fp2, "   ���������������㩥���������������㩥���������������㩥�������������������㩥��������������������������������������������\r\n" );
	fprintf( fp2, "   ��%12s��%16d��%16.2f��%20s��%44s��\r\n", "�ϼ�",  lSnum1, \
				dSamt1, "", "" );
	fprintf( fp2, "   ���������������۩����������������۩����������������۩��������������������۩���������������������������������������������\f" );

	fclose( fp );
	fclose( fp1 );
	fclose( fp2 );
	ret = pub_base_CompDblVal( dT_amt, dBd_amt);
	if ( ret )
	{
		sprintf(acErrMsg,"�ܽ����ֻ����׽��ϼƲ����!!!" );
		WRITEMSG
		sprintf(acErrMsg,"��ϸ���Ϊ[%.2lf]", dBd_amt);
		set_zd_data( "0130", acErrMsg );
		strcpy( g_pub_tx.reply, "H019" );
		goto ErrExit;
	}
	
	if ( dT_amt > 0 )	/*����ܽ�����0 ��ǵ�λ��*/
	{ 
		sprintf(acErrMsg,"-----------------��ʼ��λ�˻�ȡ����-------------------");
		WRITEMSG
		memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
		memset(&g_in_mst,0x00,sizeof(struct in_mst_c));
		memset(&g_in_parm,0x00,sizeof(struct in_parm_c));
        
		g_pub_tx.ac_id = 0;	 /*test clean */
		g_pub_tx.ac_seqn = 0;
		strcpy(g_pub_tx.beg_note_no,"");
        
		strcpy(g_pub_tx.ac_no,cz_acno);
		g_pub_tx.tx_amt1 = dT_amt;		/*��λ�˻����˽�� */
		strcpy(g_pub_tx.ct_ind,"2");
        
		if ( cz_acno[0] == '9' )	/*-- �ڲ��� --*/
		{
			strcpy( g_pub_tx.ac_id_type,"9" ); /*�˻�����Ϊ�ڲ�*/
			strcpy( g_pub_tx.ac_get_ind,"00" ); /*������δ��ȡ�ֻ�*/
			g_pub_tx.svc_ind=9001;
        
			memset( g_pub_tx.sub_tx_code, 0x0, sizeof(g_pub_tx.sub_tx_code) );
			strcpy( g_pub_tx.sub_tx_code, "A016" );
			set_zd_data( DC_SUB_TX_CODE, g_pub_tx.sub_tx_code );
			
			set_zd_data( "1204", "01" );
			set_zd_data( "1205", "2" );
			set_zd_data( "120A", "��������" );
			set_zd_double( "1208", dT_amt );    /*��λ�˻����˽�� */
		}
		else
		{
			strcpy( g_pub_tx.ac_id_type,"1" ); /*�˻�����Ϊ���ڽ��㻧*/
			g_pub_tx.svc_ind=1001;
        
			memset( g_pub_tx.sub_tx_code, 0x0, sizeof(g_pub_tx.sub_tx_code) );
			strcpy( g_pub_tx.sub_tx_code, "D003" );
			set_zd_data( DC_SUB_TX_CODE, g_pub_tx.sub_tx_code );
			
			set_zd_data( "102J", "01" );
			set_zd_data( "102K", "2" );
			set_zd_double( "102F", dT_amt );  /*��λ�˻����˽�� */
			set_zd_double( "102P", 0.00 );
			set_zd_double( "102Q", 0.00 );
		}
        
		if ( sAg_unitinfo.opr_attri[0] == '1' )
		{
			strcpy(g_pub_tx.brf, "����ҵ�񿪻�");
		}
		else
		{
			strcpy(g_pub_tx.brf, "����ҵ�񿪻�");
		}
        
		strcpy( g_pub_tx.ac_wrk_ind,"000" ); /*���˶�ƾ֤��,�������ˮ,��ϸ*/
		strcpy(g_pub_tx.add_ind,"0");/*�跽��־*/
		strcpy( g_pub_tx.prt_ind,"0" ); /*������*/
		strcpy(g_pub_tx.prt_ind,"0");
		strcpy(g_pub_tx.hst_ind,"1"); /*�ռ�����ϸ*/
        
		ret=pub_acct_trance();
		if( ret )
		{
			sprintf(acErrMsg,"���ô�ȡ������ʧ��!");
			WRITEMSG
			goto ErrExit;
		}		
		
		ret = pubf_acct_proc( g_pub_tx.sub_tx_code );
		if ( ret )
		{
			sprintf(acErrMsg,"�ǻ��ڿ�������˳���[%d]!",ret);
			WRITEMSG
			goto ErrExit;
		}
	}
	
	memset( execstr, 0x0, sizeof(execstr) );
	if ( dT_amt > 0 )	/*����ܽ�����0*/
	{
		memset( filename3, 0x0, sizeof(filename3) );
		/**sprintf( filename3, "%s/DWKHHZ%s%ld%s.txt", getenv("FILDIR"), cUnit_no, g_pub_tx.tx_date,g_pub_tx.tel); **/
		sprintf( filename3, "%s/DWKHHZ%s%ld%s%06d.txt", getenv("FILDIR"), cUnit_no, g_pub_tx.tx_date,g_pub_tx.tel,g_pub_tx.trace_no); /**Add by hxc 100529 �ļ������6λ��ˮ��**/
		/*add by kevinchan 20121026* start*/
		sprintf( execstr, "cp %s %s", filename1, filename3);
		ret = system( execstr );
		fp3 = fopen(filename3,"a");
		fp2 = fopen(filename2,"r");			
		while(1)
		{
			memset( tmpstr, 0x0, sizeof(tmpstr) );
			fgets( tmpstr,1024,fp2);
			fputs(tmpstr,fp3);
			if(feof(fp2)) 
			{
				fclose(fp2);
				fclose(fp3);
				break;
			}
	  }
		/*add by kevinchan 20121026 end*/
		/*modify by kevinchan 20121026 start*/
		/*fp3 = fopen( filename3, "w" );*/
		fp3 = fopen( filename3, "a" ); /*------��ȷ����������-------*/
		/*modify by kevinchan 20121026 end*/
		if( fp1==NULL )
		{
			sprintf(acErrMsg," open file [%s] error ",filename3 );
			WRITEMSG
			strcpy( g_pub_tx.reply,"S047" );
			goto ErrExit;
		}
		
		memset( &sCom_branch, 0x0, sizeof(sCom_branch) );
		ret = Com_branch_Sel( g_pub_tx.reply, &sCom_branch, "br_no = '%s'", g_pub_tx.tx_br_no );
		
		pub_base_strpack( sCom_branch.br_name );
		pub_base_strpack( sAg_unitinfo.bank_acct_no );
		/*add by wudw 20130403 end*/
		get_zd_data("0040",cTrace_no);
	  /* end */
		
		sprintf(acErrMsg,"trace_no = [%s]  ",cTrace_no );
		fprintf( fp3, "\r\n\r\n\r\n\r\n\r\n\r\n");
    fprintf( fp3, "				           [%s]��������λ��Ʊ	\r\n", sAg_unitinfo.unit_name );
		fprintf( fp3, "                  ������: %s    			�������� : %s\r\n", g_pub_tx.tx_br_no, sCom_branch.br_name );
		fprintf( fp3, "                  ��λ����: %s   			��λ����: %s  \r\n", cUnit_no, sAg_unitinfo.unit_name );
		fprintf( fp3, "                  ���� : %ld 			��λ�˺ţ�%s \r\n", g_pub_tx.tx_date, sAg_unitinfo.bank_acct_no );
		fprintf( fp3, "                  ��ˮ�� : %s			\r\n", cTrace_no );
		
		fprintf( fp3, "                  �����������Щ����������������������������������������Щ��������Щ�����������������\r\n" );
		fprintf( fp3, "                  ����λ��ũ�           ��   λ   ��   ��            ��  ����  ��    ��   ��     ��\r\n" );
		fprintf( fp3, "                  �����������੤���������������������������������������੤�������੤����������������\r\n" );
		fprintf( fp3, "                  ��%8s��%40s��%8ld��%16.2lf��\r\n", sAg_unitinfo.unit_no, sAg_unitinfo.unit_name, lSnum, dSamt );
		fprintf( fp3, "                  �����������੤���������������������������������������੤�������੤����������������\r\n" );
		fprintf( fp3, "                  �� ��  �� ��                                        ��%8ld��%16.2lf�� \r\n", lSnum, dSamt );
		fprintf( fp3, "                  �����������ة����������������������������������������ة��������ة����������������� \f");

		fprintf( fp3, "\r\n\r\n\r\n\r\n\r\n\r\n");
		fprintf( fp3, "                  	   [%s]����������˻��ܴ�Ʊ	\r\n", sAg_unitinfo.unit_name );
		fprintf( fp3, "                  ������: %s    			�������� : %s\r\n", g_pub_tx.tx_br_no, sCom_branch.br_name );
		fprintf( fp3, "                  ���� : %ld 			��ˮ�� ��%s \r\n", g_pub_tx.tx_date, cTrace_no);
		
		fprintf( fp3, "                  �����������Щ����������������������������������������Щ��������Щ�����������������\r\n" );
		fprintf( fp3, "                  ����λ��ũ�           ��   λ   ��   ��            ��  ����  ��    ��   ��     ��\r\n" );
		fprintf( fp3, "                  �����������੤���������������������������������������੤�������੤����������������\r\n" );
		fprintf( fp3, "                  ��%8s��%40s��%8ld��%16.2lf��\r\n", sAg_unitinfo.unit_no, sAg_unitinfo.unit_name, lSnum, dSamt );
		fprintf( fp3, "                  �����������੤���������������������������������������੤�������੤����������������\r\n" );
		fprintf( fp3, "                  �� ��  �� ��                                        ��%8ld��%16.2lf�� \r\n", lSnum, dSamt );
		fprintf( fp3, "                  �����������ة����������������������������������������ة��������ة����������������� \f" );	
		
		fclose( fp3 );		
		
		memset( filename4, 0x0, sizeof(filename4) );
		/**sprintf( filename4,"%s/DLKHMX%s%ld%s.txt",getenv("FILDIR"),cUnit_no,g_pub_tx.tx_date,g_pub_tx.tel); **/
    sprintf( filename4,"%s/DLKHMX%s%ld%s%06d.txt",getenv("FILDIR"),cUnit_no,g_pub_tx.tx_date,g_pub_tx.tel,g_pub_tx.trace_no); /**Add by hxc 100529 �ļ������6λ��ˮ��**/
		sprintf( execstr, "cp %s %s", filename1, filename4 );
		ret = system( execstr );
		sprintf( execstr, "cp %s %s", filename3, filename );
	}
	else
	{
		sprintf( execstr, "cp %s %s", filename2, filename );
	}

	ret = system( execstr );
	if ( ret )
	{
		sprintf(acErrMsg,"�����´��ļ���ʧ��[%s][%s][%d][%s]", filename, strerror(errno), ret, execstr );
		WRITEMSG
		strcpy( g_pub_tx.reply, "S047" );
		goto ErrExit;		
	}

	set_zd_data( DC_FILE_SND, "1" );
	/**
	if ( iErr_flag == 1 )
	{
		strcpy( g_pub_tx.reply, "H034" );
		goto ErrExit;
	}
	**/
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"���������ɹ���[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    fclose( fp );
    fclose( fp1 );
    fclose( fp2 );
    sprintf(acErrMsg,"��������ʧ�ܣ�[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
