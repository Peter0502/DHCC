/*************************************************
* �� �� ��: spF630.c
* �����������������������������ݴ���
*
* ��    ��: jack
* �������: 2004��05��04��
* �޸�����:
* �� �� ��:
* �޸�����:
*
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#define EXTERN
#include "public.h"
#include <errno.h>
#include "pay_agent_c.h"
#include "ag_peritemmap_c.h"
#include "ag_unitinfo_c.h"
#include "ag_itemlist_c.h"
#include "cif_basic_inf_c.h"
#include "cif_id_code_rel_c.h"

spF630()
{
	
	int ret = 0;
	int ret1 = 0;
	char cReturn[32];

	char cAc_no[20];
	char cUnit_no[5];
	char cBat_flag[2];
	char cRemark[17];
	
	long lBeg_date = 0;
	long lEnd_date = 0;
	
	char cHand_flag[2];	/*---��Ϣ¼�뷽ʽ---*/
	double dT_amt = 0.00;	/*---ǰ̨�����ܽ��---*/
	long lCount_i = 0;	/*---ǰ̨�����ܱ���---*/
	
	double dTx_amt = 0.00;
	double dTmp_amt = 0.00;
	double dTemp_amt = 0.00;
	
	/*--- ͳ�Ƴɹ���ʧ�ܱ������� ---*/
	long lSnum = 0;
	long lFnum = 0;
	double dSamt = 0.00;
	double dFamt = 0.00;
	
	int iK_flag = 0;	/*�¿�����־*/
	long lK_num = 0;	/*�¿�����*/
	double dK_amt = 0.00;	/*�¿����*/
	
	char cDate[11];		/*���÷�������*/
	
	char filename[248];
	char filename1[248];
	char filename2[248];
	char filename3[248];
	FILE *fp;
	FILE *fp1;
	FILE *fp2;
	
	char execstr[256];
	
	char fldstr[32][100];
	char tmpstr[1025];

	int iProc_flag = 0;
	int iProc_flag1 = 0;
	int iErr_flag = 0;
	int i=0;

	struct pay_agent_c sPay_agent;
	struct ag_peritemmap_c sAg_peritemmap;
	struct ag_unitinfo_c sAg_unitinfo;
	struct dd_mst_c sDd_mst;
	struct mdm_ac_rel_c sMdm_ac_rel;
	struct ag_itemlist_c sAg_itemlist;
	struct cif_basic_inf_c cif_basic_inf;
	struct cif_id_code_rel_c cif_id_code_rel;

	memset(cAc_no,0x0,sizeof(cAc_no));
	memset(cUnit_no,0x0,sizeof(cUnit_no));
	memset(cRemark,0x0,sizeof(cRemark));
	memset(cReturn,0x0,sizeof(cReturn));
	memset( cHand_flag, 0x0, sizeof(cHand_flag) );
	memset( cDate, 0x0, sizeof(cDate) );

	get_zd_data("0920",cUnit_no);
	get_zd_data("0300",cAc_no);
	pub_base_old_acno( cAc_no );
	
	get_zd_long( "0460", &lBeg_date );
	get_zd_long( "0470", &lEnd_date );
	
	memset( cDate, 0x0, sizeof(cDate) );
	sprintf( cDate, "%ld", lBeg_date );


	get_zd_data("0610",cRemark);
	get_zd_data( "0700", cHand_flag );

	/**------- ��ʼ��ȫ�ֱ��� --------**/
	pub_base_sysinit();

	memset(filename,0x0,sizeof(filename));
	Gl_file_first=0;
	pub_base_AllDwnFilName( filename );

	sprintf(acErrMsg,"ȡ�õ��ļ���Ϊ[%s]����[%s]�ն�[%s]",filename,g_pub_tx.tx_br_no,g_pub_tx.tty);
	WRITEMSG

	memset( &sAg_unitinfo, 0x0, sizeof(sAg_unitinfo) );
	ret = Ag_unitinfo_Sel ( g_pub_tx.reply, &sAg_unitinfo, \
			"unit_no = '%s'" , cUnit_no );
	if( ret )
	{
		sprintf(acErrMsg," ��ѯ��λ���ϳ���[%d]!!!", ret );
		WRITEMSG
		goto ErrExit;
	}

	memset(filename1,0x0,sizeof(filename1));
	sprintf(filename1,"%s/0%s%ld.txt",getenv("FILDIR"),cUnit_no,g_pub_tx.tx_date);
	fp1 = fopen( filename1,"w" );	/*���ļ�������¼��ȷ������*/
	if( fp1==NULL )
	{
		sprintf(acErrMsg," open file [%s] error ",filename1 );
		WRITEMSG
		strcpy( g_pub_tx.reply,"S047" );
		goto ErrExit;
	}
	
	pub_base_strpack( sAg_unitinfo.unit_name );
	
	fprintf( fp1, "              [%s-%s]������������ҵ����ɹ���¼\n", sAg_unitinfo.unit_no, sAg_unitinfo.unit_name );
	fprintf( fp1, "�����������������������ө��������������������ө����������������ө��������������������ө��������������������ө���������������������\n" );
    	fprintf( fp1, "��%20s��%20s��%16s��%20s��%20s��%20s��\n", "������", "�ͻ�����", "ʵ�����", "���֤����", "�˺�", "������" );
    	fprintf( fp1, "�����������������������㩥�������������������㩥���������������㩥�������������������㩥�������������������㩥��������������������\n" );


	memset(filename2,0x0,sizeof(filename2));
	sprintf(filename2,"%s/1%s%ld.txt",getenv("FILDIR"),cUnit_no,g_pub_tx.tx_date);
	fp2 = fopen( filename2,"w" );	/*���ļ�������¼��ȷ������*/
	if( fp2==NULL )
	{
		sprintf(acErrMsg," open file [%s] error ",filename2 );
		WRITEMSG
		strcpy( g_pub_tx.reply,"S047" );
		goto ErrExit;
	}
	
	fprintf( fp2, "          [%s-%s]������������ҵ����ʧ�ܼ�¼\n", sAg_unitinfo.unit_name );
	fprintf( fp2, "�����������������������ө����������������ө���������������������������������������������������\n" );
    	fprintf( fp2, "��%20s��%16s��%50s��\n", "������", "ʵ�����", "������" );
    	fprintf( fp2, "�����������������������㩥���������������㩥��������������������������������������������������\n" );

/*-------------------------------ͳ�ƺ˶���Ϣ----------------------
	fp = fopen( filename,"r" );
	if( fp==NULL )
	{
		sprintf(acErrMsg," open ���� file [%s] error ",filename );
		WRITEMSG
		strcpy( g_pub_tx.reply,"S047" );
		goto ErrExit;
	}
	
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
	
	while (1)
	{
		memset( tmpstr, 0x0, sizeof(tmpstr) );
		fgets( tmpstr,1024,fp );
		if( feof(fp) ) 
		{
			break;
		}
		
		i = 0;
		memset( fldstr, 0x0, sizeof(fldstr) );
		for( ; ; )
		{
			ret = pub_base_cut_str( tmpstr,fldstr[i],",",i+1 );
			if ( ret == 100 )
			{
				break;
			}

			vtcp_log(" [%d] [%s] ",i+1,fldstr[i]);
			pub_base_strpack( fldstr[i] );

			i = i + 1;
		}
		
		dTemp_amt = 0.00;
		dTemp_amt = atof( fldstr[2] );
		dTmp_amt = dTmp_amt + dTemp_amt;

		lCount = lCount + 1;
	}
	fclose ( fp );
*------------------------------------------------------------------------------*/
	
	fp = fopen( filename,"r" );
	if( fp==NULL )
	{
		sprintf(acErrMsg," open ���� file [%s] error ",filename );
		WRITEMSG
		strcpy( g_pub_tx.reply,"S047" );
		goto ErrExit;
	}
	
	memset( tmpstr, 0x0, sizeof(tmpstr) );
	fgets( tmpstr,1024,fp );

	while(1)
	{
		iProc_flag = 0;
		iProc_flag1 = 0;
		iK_flag = 0;
		
		memset( tmpstr, 0x0, sizeof(tmpstr) );
		fgets( tmpstr,1024,fp );
		if( feof(fp) ) 
		{
			break;
		}
		
		memset(fldstr,0x0,sizeof(fldstr));
		i = 0;
		for( ; ; )
		{
			ret=pub_base_cut_str( tmpstr,fldstr[i],",",i+1 );
			if ( ret == 100 )
			{
				break;
			}

			vtcp_log(" [%d] [%s] ",i+1,fldstr[i]);
			pub_base_strpack( fldstr[i] );

			i = i + 1;
		}
		
		memset( &sAg_peritemmap, 0x00, sizeof(sAg_peritemmap) );
		ret = Ag_peritemmap_Sel( g_pub_tx.reply , &sAg_peritemmap, \
			"unit_no = '%s' and item_no = '%s'", \
			cUnit_no, fldstr[1] );
		if ( ret == 0 )
		{
			pub_base_strpack( sAg_peritemmap.ac_no );
			pub_base_old_acno( sAg_peritemmap.ac_no );
			
			memset( &sMdm_ac_rel, 0x0, sizeof(sMdm_ac_rel) );
			ret1 = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdm_ac_rel, \
				"ac_no = '%s'", sAg_peritemmap.ac_no );
			if ( ret1 )
			{
				sprintf( cReturn, "�����˺�[%s]��Ϣ��", sAg_peritemmap.ac_no );
				sprintf( acErrMsg, "�����˺�[%s]��Ϣ��", sAg_peritemmap.ac_no );
				WRITEMSG
				iProc_flag1 = 1;
				iErr_flag = 1;
				goto TrLog;
			}
			
			memset( &sDd_mst, 0x0, sizeof(sDd_mst) );
			ret1 = Dd_mst_Sel( g_pub_tx.reply, &sDd_mst, \
				"ac_id = %ld", sMdm_ac_rel.ac_id );
			if ( ret1 )
			{
				sprintf( cReturn, "�����˺�[%s]������Ϣ��", sAg_peritemmap.ac_no );
				sprintf( acErrMsg, "�����˺�[%s]������Ϣ��", sAg_peritemmap.ac_no );
				WRITEMSG
				iProc_flag1 = 1;
				iErr_flag = 1;
				goto TrLog;
			}
			
			pub_base_strpack( sDd_mst.ac_sts );
			if ( sDd_mst.ac_sts[0] == '*' )
			{
				strcpy( cReturn, "������,�¿���" );
				sprintf( acErrMsg, "������,�¿���[%s]", sAg_peritemmap.item_no );
				WRITEMSG
				iK_flag = 1;
				iProc_flag = 1;
			}
			
			memset(&cif_id_code_rel, 0x0, sizeof(cif_id_code_rel));
			memset(&cif_basic_inf, 0x0, sizeof(cif_basic_inf));
                        
			/* ���ͻ�֤�����ͣ�֤�������Ƿ��Ѿ����� */
			ret = Cif_id_code_rel_Sel( g_pub_tx.reply , &cif_id_code_rel ,
					"cif_no= %ld" , sDd_mst.cif_no );
			if( ret )
			{
				sprintf( acErrMsg, "�ÿͻ���֤����Ϣ������![%ld]", sDd_mst.cif_no );
				WRITEMSG
				strcpy( cReturn, "�ͻ�֤����Ϣ������");
				goto TrLog;				
			}
			
			ret = Cif_basic_inf_Sel( g_pub_tx.reply , &cif_basic_inf ,
					"cif_no= %ld" , sDd_mst.cif_no );
			if( ret )
			{
				sprintf( acErrMsg, "�ÿͻ��Ļ�����Ϣ������![%ld]", sDd_mst.cif_no );
				WRITEMSG
				strcpy( cReturn, "�ͻ�������Ϣ������");
				goto TrLog;
			}
			
			pub_base_strpack( cif_id_code_rel.id_no );
			pub_base_strpack( cif_basic_inf.name );
		}
		else if ( ret == 100 )
		{
			pub_base_old_acno( fldstr[1] );
			
			memset( &sMdm_ac_rel, 0x0, sizeof(sMdm_ac_rel) );
			ret1 = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdm_ac_rel, \
				"ac_no = '%s'", fldstr[1] );
			if ( ret1 )
			{
				sprintf( cReturn, "�����˺�[%s]��Ϣ��", fldstr[1] );
				sprintf( acErrMsg, "�����˺�[%s]��Ϣ��", fldstr[1] );
				WRITEMSG
				iProc_flag1 = 1;
				iErr_flag = 1;
				goto TrLog;
			}
			
			memset( &sDd_mst, 0x0, sizeof(sDd_mst) );
			ret1 = Dd_mst_Sel( g_pub_tx.reply, &sDd_mst, \
				"ac_id = %ld", sMdm_ac_rel.ac_id );
			if ( ret1 )
			{
				sprintf( cReturn, "�����˺�[%s]������Ϣ��", fldstr[1] );
				sprintf( acErrMsg, "�����˺�[%s]������Ϣ��", fldstr[1] );
				WRITEMSG
				iProc_flag1 = 1;
				iErr_flag = 1;
				goto TrLog;
			}
			
			pub_base_strpack( sDd_mst.ac_sts );
			if ( sDd_mst.ac_sts[0] == '*' )
			{
				strcpy( cReturn, "������,�¿���" );
				sprintf( acErrMsg, "������,�¿���[%s]", sAg_peritemmap.item_no );
				WRITEMSG
				iK_flag = 1;
				iProc_flag = 1;
			}
			
			memset(&cif_id_code_rel, 0x0, sizeof(cif_id_code_rel));
			memset(&cif_basic_inf, 0x0, sizeof(cif_basic_inf));
                        
			/* ���ͻ�֤�����ͣ�֤�������Ƿ��Ѿ����� */
			ret = Cif_id_code_rel_Sel( g_pub_tx.reply , &cif_id_code_rel ,
					"cif_no= %ld" , sDd_mst.cif_no );
			if( ret )
			{
				sprintf( acErrMsg, "�ÿͻ���֤����Ϣ������![%ld]", sDd_mst.cif_no );
				WRITEMSG
				strcpy( cReturn, "�ͻ�֤����Ϣ������");
				goto TrLog;				
			}
			
			ret = Cif_basic_inf_Sel( g_pub_tx.reply , &cif_basic_inf ,
					"cif_no= %ld" , sDd_mst.cif_no );
			if( ret )
			{
				sprintf( acErrMsg, "�ÿͻ��Ļ�����Ϣ������![%ld]", sDd_mst.cif_no );
				WRITEMSG
				strcpy( cReturn, "�ͻ�������Ϣ������");
				goto TrLog;
			}
			
			pub_base_strpack( cif_id_code_rel.id_no );
			pub_base_strpack( cif_basic_inf.name );
			
			strcpy( sAg_peritemmap.ac_no, fldstr[1] );
			strcpy( sAg_peritemmap.name, cif_basic_inf.name );
		}
		else if ( ret )
		{
			sprintf(acErrMsg,"ϵͳ����!!![%d", ret );
			WRITEMSG
			goto ErrExit;
		}
			
		/*---------------���벿��-----------------*/
		memset(&sPay_agent,0x0,sizeof(sPay_agent));
			
		strcpy(sPay_agent.unit_no, sAg_unitinfo.unit_no ); 
		strcpy(sPay_agent.item_no, fldstr[1] );
		strcpy(sPay_agent.name, sAg_peritemmap.name );
		strcpy(sPay_agent.id_type, cif_id_code_rel.id_type );
		strcpy(sPay_agent.id_no, cif_id_code_rel.id_no );
		strcpy(sPay_agent.ac_no, sAg_peritemmap.ac_no );
		sPay_agent.trace_no = g_pub_tx.trace_no;
		sPay_agent.tx_date = g_pub_tx.tx_date;
		sPay_agent.tram = atof(fldstr[28]);
		strcpy(sPay_agent.remark, cRemark );
		strcpy(sPay_agent.sata,"1");	/*-- ��Ҫ�������� --*/
		strcpy(sPay_agent.result,"1");	/*-- δ���� --*/

		ret = Pay_agent_Ins( sPay_agent, g_pub_tx.reply  );
		if (ret != 0)
		{
			if ( ret == -268 )
			{
				strcpy( cReturn, "δ�����¼�ظ�");
				sprintf(acErrMsg,"δ�����¼�ظ�");
			}
			else
			{
				strcpy( cReturn, "дδ�����¼�����");
				sprintf(acErrMsg,"����δ�����¼��ʧ��!");
			}
			
			WRITEMSG
			iProc_flag1 = 1;
			iErr_flag = 1;
			goto TrLog;
			
		}
				
		memset( &sAg_itemlist, 0x0, sizeof(sAg_itemlist) );		
		strcpy( sAg_itemlist.stcd, "1" );
		
		strcpy( sAg_itemlist.item_no, fldstr[1] );
		strcpy( sAg_itemlist.unit_no, cUnit_no );
		sprintf( sAg_itemlist.bank_tlcs, "%ld", g_pub_tx.trace_no );
		strncpy( sAg_itemlist.owe_month, fldstr[3], 4 );	/*   ���÷�������   */
		strncat( sAg_itemlist.owe_month, fldstr[3]+5, 2 );
		
		sprintf( sAg_itemlist.trn_date, "%ld", g_pub_tx.tx_date );
		sAg_itemlist.am00 = atof( fldstr[4] );
		sAg_itemlist.am01 = atof( fldstr[5] );
		sAg_itemlist.am02 = atof( fldstr[6] );
		sAg_itemlist.am03 = atof( fldstr[7] );
		sAg_itemlist.am04 = atof( fldstr[8] );
		sAg_itemlist.am05 = atof( fldstr[9] );
		sAg_itemlist.am06 = atof( fldstr[10] );
		sAg_itemlist.am07 = atof( fldstr[11] );
		sAg_itemlist.am08 = atof( fldstr[12] );
		sAg_itemlist.am09 = atof( fldstr[13] );
		sAg_itemlist.am10 = atof( fldstr[14] );
		sAg_itemlist.am11 = atof( fldstr[15] );
		sAg_itemlist.am12 = atof( fldstr[16] );
		sAg_itemlist.am13 = atof( fldstr[17] );
		sAg_itemlist.am14 = atof( fldstr[18] );
		sAg_itemlist.am15 = atof( fldstr[19] );
		sAg_itemlist.am16 = atof( fldstr[20] );
		sAg_itemlist.am17 = atof( fldstr[21] );
		sAg_itemlist.am18 = atof( fldstr[22] );
		sAg_itemlist.am19 = atof( fldstr[23] );
		sAg_itemlist.am20 = atof( fldstr[24] );
		sAg_itemlist.am21 = atof( fldstr[25] );
		sAg_itemlist.am22 = atof( fldstr[26] );
		sAg_itemlist.am23 = atof( fldstr[27] );
		sAg_itemlist.am24 = atof( fldstr[28] );		/*	��������Ӧ�����	*/
		sAg_itemlist.am25 = 0.00;
		strcpy( sAg_itemlist.sc00, " " );
		strcpy( sAg_itemlist.sc01, " " );
		strcpy( sAg_itemlist.sc02, " " );
		strcpy( sAg_itemlist.sc03, " " );
		strcpy( sAg_itemlist.sc04, " " );
		strcpy( sAg_itemlist.sc05, " " );
		strcpy( sAg_itemlist.sc06, " " );
		
		ret = Ag_itemlist_Ins( sAg_itemlist, g_pub_tx.reply );
		if ( ret )
		{
			strcpy( cReturn, "д��������������ϸ����" );
			sprintf( acErrMsg, "д��������������ϸ����[%d]!!!!!!!!!!!", ret );
			WRITEMSG
		}
		if ( iProc_flag == 0 )
		{
			strcpy( cReturn, "�ɹ�" );
		}
		
TrLog:
		if ( iProc_flag1 == 0 )
		{
			lSnum = lSnum + 1;
			dSamt = dSamt + atof(fldstr[28]);
			fprintf( fp1, "��%20s��%20s��%16s��%20s��%20s��%20s��\n",fldstr[1], cif_basic_inf.name, \
					fldstr[28], cif_id_code_rel.id_no, sAg_peritemmap.ac_no, cReturn );
					
			if ( iK_flag == 1 )
			{
				lK_num = lK_num + 1;	/*�¿�����*/
				dK_amt = dK_amt + atof(fldstr[28]);	/*�¿����*/
			}
		}
		else
		{
			lFnum = lFnum + 1;
			dFamt = dFamt + atof(fldstr[28]);
			fprintf(fp2,"��%20s��%16s��%50s��\n", fldstr[1], fldstr[28], cReturn );
		}
	}
	
	fprintf( fp1, "�����������������������۩��������������������۩����������������۩��������������������۩��������������������۩���������������������\n" );
	fprintf( fp2, "�����������������������۩����������������۩���������������������������������������������������\n" );
	
	fclose(fp);
	fclose(fp1);
	fclose(fp2);
	
	/*
	sprintf(acErrMsg, "�ɹ�����[%ld]���[%.2f];ʧ�ܱ���[%ld]���[%.2f]", lSnum, dSamt, lFnum, dFamt );
	*/
	
	memset( filename3, 0x0, sizeof(filename3) );
	pub_base_AllDwnFilName( filename3 );			/*  �´��ļ���  */
	
	memset( execstr, 0x0, sizeof(execstr) );
	if ( iErr_flag == 0 )
	{
		sprintf( acErrMsg, "�¿�[%ld],����[%s]", lK_num, cDate );
		sprintf( execstr, "cp %s %s", filename1, filename3 );
	}
	else
	{
		sprintf(acErrMsg,"ʧ��[%ld][%.2f],�¿�[%ld],����[%s],", lFnum, dFamt, lK_num, cDate );
		sprintf( execstr, "cp %s %s", filename2, filename3 );
	}
	set_zd_data( "0130", acErrMsg );

	ret = system( execstr );
	if ( ret )
	{
		sprintf(acErrMsg,"�����´��ļ���ʧ��[%s][%s][%d][%s]", filename3, strerror(errno), ret, execstr );
		WRITEMSG
		strcpy( g_pub_tx.reply, "S047" );
		goto ErrExit;
	}

	set_zd_data(DC_FILE_SND,"1");
	
	if ( iErr_flag != 0 )
	{
		strcpy( g_pub_tx.reply, "H034" );
		goto ErrExit;
	}	

	ret = sql_execute( "update ag_unitinfo set beg_date = %ld, end_date = %ld \
		where unit_no = '%s'", lBeg_date, lEnd_date, cUnit_no );
	if ( ret )
	{
		sprintf( acErrMsg, "���ĵ�λ�����������ڴ�!!!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "H020" );
		goto ErrExit;
	}

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"���������������������ݴ���ɹ���[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
	fclose(fp);
	fclose(fp1);
	fclose(fp2);
    sprintf(acErrMsg,"���������������������ݴ���ʧ�ܣ�[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
