/*************************************************
* �� �� ��: spEB01.c
* ����������������������ҵ����
*
* ��    ��: jack
* �������: 2004��02��28��
* �޸�����: 20130527
* �� �� ��: ������
* �޸�����: ��������ԭ�����������������������ֲ��޸�
*************************************************/
#define ERR_DEAL  {WRITEMSG goto ErrExit;}
#define EXTERN
#define cIdtp "1"		/*Ĭ��֤������Ϊ���֤*�ָ�Ϊ�����κ�֤������fldstr[5]*/
#include "public.h"
#include <errno.h>
#include "pay_agent_c.h"
#include "ag_peritemmap_c.h"
#include "ag_unitinfo_c.h"
#include "ag_paytlcs_c.h"
#include "cif_basic_inf_c.h"
#include "cif_id_code_rel_c.h"
#include "ag_qs_gd_rel_c.h"

spEB01()
{
	int ret=0, ret1=0, ins_flag=0; /* 0�ɹ�  1ʧ�� */
	char cReturn[32];

	char cAc_no[20];
	char cUnit_no[5];
	char cUnit_no_tmp[5];
	char cRemark[17];
	char cHand_flag[2];		/*---��Ϣ¼�뷽ʽ---*/
	char cFtpfile[200];
	double dT_amt = 0.00;	/*---ǰ̨�����ܽ��---*/
	long lCount_i = 0;		/*---ǰ̨�����ܱ���---*/

	long lBeg_date = 0;
	long lEnd_date = 0;

	double dTx_amt = 0.00;
	double dTmp_amt = 0.00;
	double dTemp_amt = 0.00;

	/*------���ؽ����Ϣ------*/
	long lSnum = 0;		/*�ɹ�����*/
	long lFnum = 0;		/*ʧ�ܱ���*/
	double dSamt = 0.00;	/*�ɹ����*/
	double dFamt = 0.00;	/*ʧ�ܽ��*/

	int iK_flag = 0;	/*�¿�����־*/
	long lK_num = 0;	/*�¿�����*/
	double dK_amt = 0.00;	/*�¿����*/

	char cDate[9];		/*���÷�������*/

	long cif_no = 0;
	int iLen = 0;
	long lCount = 0;

	char filename[248];
	char pcfilename[248];
	char filename1[248];
	char filename2[248];
	char sw_trace[20];
	FILE *fp=NULL;
	FILE *fp1=NULL;

	char execstr[256];

	char fldstr[10][100];
	char tmpstr[1024];
	

	int iProc_flag = 0;		/*�����־*/
	int iProc_flag1 = 0;		/*�ɹ�ʧ�ܱ�־*/
	int iErr_flag = 0;		/*�Ƿ���ʧ�ܼ�¼��־*/

	int i=0;

	struct pay_agent_c sPay_agent;
	struct ag_peritemmap_c sAg_peritemmap;
	struct ag_unitinfo_c sAg_unitinfo;
	struct ag_paytlcs_c sAg_paytlcs;
	struct dd_mst_c sDd_mst;
	struct mdm_ac_rel_c sMdm_ac_rel;
	struct in_mst_c sIn_mst;
	struct ag_qs_gd_rel_c sAg_qs_gd_rel;
	memset(cAc_no,0x0,sizeof(cAc_no));
	memset(cUnit_no,0x0,sizeof(cUnit_no));
	memset(cRemark,0x0,sizeof(cRemark));
	memset(cReturn,0x0,sizeof(cReturn));
	memset( cHand_flag, 0x0, sizeof(cHand_flag) );
	memset( cFtpfile, 0x0, sizeof(cFtpfile) );
	/****/
	memset(cUnit_no_tmp,0x0,sizeof(cUnit_no_tmp));
	memset(sw_trace,0x0,sizeof(sw_trace));
	memset(pcfilename,0x0,sizeof(pcfilename));
	memset(filename2,0x0,sizeof(filename2));
	/*add by kevinchan 20120311 start*/
	struct ag_peritemmap_c sAg_peritemmap1;
	int daliCnt=0;
	char newAcno[25];
	/*add by kevinchan 20120311 end*/

	get_zd_data("0520",sw_trace);
	get_zd_data("0920",cUnit_no);
	sprintf( acErrMsg, "��λ���[%s]",cUnit_no);
	WRITEMSG
	memcpy(cUnit_no_tmp,cUnit_no,sizeof(cUnit_no_tmp)-1);
		sprintf( acErrMsg, "��λ���[%s]",cUnit_no_tmp);
	WRITEMSG
	memcpy(cUnit_no,cUnit_no_tmp,sizeof(cUnit_no)-1);
	sprintf( acErrMsg, "��λ���[%s]",cUnit_no);
	WRITEMSG
	get_zd_data("0300",cAc_no);
	pub_base_old_acno( cAc_no );

	get_zd_data("0610",cRemark);
	get_zd_data( "0700", cHand_flag );

	get_zd_long( "0460", &lBeg_date );
	get_zd_long( "0470", &lEnd_date );

	memset( cDate, 0x0, sizeof(cDate) );
	sprintf( cDate, "%ld", lBeg_date );
	sprintf( acErrMsg, "��λ�˺�[%s]",cAc_no);
	WRITEMSG

	sprintf( acErrMsg, "ȡ�õ�����������Ϊ[%ld][%ld]", lBeg_date, lEnd_date );
	WRITEMSG

	/**------- ��ʼ��ȫ�ֱ��� --------**/
	pub_base_sysinit();
	memcpy(cUnit_no,cUnit_no_tmp,sizeof(cUnit_no)-1);
	sprintf( acErrMsg, "��λ���[%s]",cUnit_no);
	WRITEMSG

	memset(filename,0x0,sizeof(filename));
	Gl_file_first = 0;
	pub_base_AllDwnFilName( filename );

	memset( &sAg_unitinfo, 0x0, sizeof(sAg_unitinfo) );
	ret = Ag_unitinfo_Sel ( g_pub_tx.reply, &sAg_unitinfo,
			"unit_no = '%s'" , cUnit_no );
	if( ret )
	{
		sprintf(acErrMsg," ��ѯ��λ���ϳ���[%d]!!!", ret );
		WRITEMSG
		goto ErrExit;
	}
	pub_base_strpack( sAg_unitinfo.unit_name );
	pub_base_strpack( sAg_unitinfo.pay_type );
	/**У����ҵ�����˻��͹����˻�������ϵ**/
	memset( &sAg_qs_gd_rel, 0x00, sizeof(sAg_qs_gd_rel) );
	ret = Ag_qs_gd_rel_Sel( g_pub_tx.reply, &sAg_qs_gd_rel, \
		"qs_ac_no = '%s' and unit_no = '%s'", cAc_no,cUnit_no );
	if ( ret )
	{
		sprintf(acErrMsg,"��ѯ��������ҵ������˻��͹����˻�������ʧ��");
		WRITEMSG
		strcpy( g_pub_tx.reply,"EB05" );
		goto ErrExit;
	}
	vtcp_log("%s,%d �����������ҵ������˻��͹����˻�������ͨ��!",__FILE__,__LINE__);
	memset(filename1,0x0,sizeof(filename1));
	memset(filename2,0x0,sizeof(filename2));
	sprintf(filename2,"nb_bat_res%ld%s.txt",g_pub_tx.tx_date,sw_trace);
	sprintf(filename1,"%s/nb_bat_res%ld%s.txt",getenv("FILDIR"),g_pub_tx.tx_date,sw_trace);
	fp1 = fopen( filename1,"w" );	/*���ļ�������¼������*/
	if( fp1==NULL )
	{
		sprintf(acErrMsg," open file [%s] error ",filename1 );
		WRITEMSG
		strcpy( g_pub_tx.reply,"S047" );
		goto ErrExit;
	}
	
	fprintf( fp1, "%20s��%20s��%16s��%20s��%20s��%18s��%28s��\n", "Ա�����", "Ա���˺�", "Ա������", "���׽��", "�ɹ�״̬", "��ע" ,"ָ����ˮ��");

	memset(filename,0x0,sizeof(filename));
	Gl_file_first=0;
	get_zd_data("0260",filename);
	sprintf(pcfilename,"%s/tmp/%s",getenv("WORKDIR"),filename);
	sprintf(acErrMsg,"ȡ�õ��ļ���Ϊ[%s]",pcfilename);
	WRITEMSG
	
	sprintf(cFtpfile,"%s/bin/ftp_getfile_nsw.sh %s %s %s",getenv("WORKDIR"),get_env_info("NSW_FTP_USER"),get_env_info("NSW_FTP_PWD"),filename);
	vtcp_log("%s,%d cFtpfile=[%s]\n",__FILE__,__LINE__,cFtpfile);
	ret=system(cFtpfile);
	if (ret)
	{
		sprintf(acErrMsg,"ftp�ļ�[%s]��ƽ̨ȡ�ļ�����[%d]",filename,ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P157");
		goto ErrExit;
	}
	sprintf(acErrMsg,"ftp�ļ�[%s]��ƽ̨ȡ�ļ��ɹ�",filename);
	WRITEMSG

	fp = fopen( pcfilename,"r" );
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
	else /**���ֹ���ʽ�ļ���һ��Ϊ������Ϣ**/
	{
		memset( tmpstr, 0x0, sizeof(tmpstr) );
		fgets( tmpstr,1024,fp );

		memset(fldstr,0x0,sizeof(fldstr));
		for( i=0;i<9;i++ )
		{
			ret=pub_base_cut_str( tmpstr,fldstr[i],"|",i+1 );
				vtcp_log(" [%d] [%s] ",i+1,fldstr[i]);
			pub_base_strpack( fldstr[i] );
		}

		if ( strncmp( cUnit_no, fldstr[3], 4 ) != 0 )
		{
			sprintf( acErrMsg, "����λ[%s]�����Ϣ����", fldstr[3] );
			WRITEMSG
			strcpy( g_pub_tx.reply, "H035" );
			goto ErrExit;
		}

		memset( cDate, 0x0, sizeof(cDate) );
		strcpy( cDate, fldstr[2] );
		/**����ļ��е������뵱�����ڲ�һ��,������ͨ�� add at 20070517**/
		if(atol(fldstr[2])!=g_pub_tx.tx_date)
		{
			sprintf( acErrMsg, "��������[%s]��Ϣ����", fldstr[2] );
			WRITEMSG
			strcpy( g_pub_tx.reply, "P096" );
			goto ErrExit;
		}
		/**end 20070517**/
		dT_amt = atof( fldstr[7] );
		lCount_i = atoi(fldstr[6] );

	}

	/*------ �ж���ϸ�����ܽ���Ƿ���� -----*/
	while (1)
	{
		memset( tmpstr, 0x0, sizeof(tmpstr) );
		fgets( tmpstr,1024,fp );
		if( feof(fp) )
		{
			break;
		}

		memset(fldstr,0x0,sizeof(fldstr));
		for( i=0;i<6;i++ )
		{
			ret=pub_base_cut_str( tmpstr,fldstr[i],"|",i+1 );
				vtcp_log(" [%d] [%s] ",i+1,fldstr[i]);
			pub_base_strpack( fldstr[i] );
		}

		/**	fldstr[0] �˻�����
				fldstr[1] ���
				fldstr[2] �˻�
				fldstr[3] ��λ���
				fldstr[4] �������
				fldstr[5] ָ����ˮ�� **/
		if ( strlen(fldstr[0]) == 0 && strlen(fldstr[1]) == 0 )
		{
			continue;
		}

		dTemp_amt = 0.00;
		dTemp_amt = atof( fldstr[1] );
		dTmp_amt = dTmp_amt + dTemp_amt;
		sprintf(acErrMsg,"�ñ���ϸ�����ֻ����׽��ϼƷֱ�Ϊ[%.2lf][%.2lf]", dTemp_amt, dTmp_amt );
		WRITEMSG 

		lCount = lCount + 1;
	}
	fclose ( fp );

	sprintf(acErrMsg,"�ܽ����ֻ����׽��ϼƷֱ�Ϊ[%.2lf][%.2lf]", dT_amt, dTmp_amt );
	WRITEMSG

	ret = pub_base_CompDblVal( dT_amt, dTmp_amt );
	if ( ret )
	{
		sprintf(acErrMsg,"�ܽ����ֻ����׽��ϼƲ����[��%lf][��ϸ%lf]", dT_amt, dTmp_amt );
		WRITEMSG
		sprintf(acErrMsg,"��ϸ�ܽ��Ϊ[%.2lf]", dTmp_amt );
		set_zd_data( "0130", acErrMsg );
		strcpy( g_pub_tx.reply, "H019" );
		goto ErrExit;
	}

	if ( lCount != lCount_i )
	{
		sprintf(acErrMsg,"�ܱ�����ֻ���ϸ�ϼƱ��������[%d][%d]!!!",lCount,lCount_i);
		WRITEMSG
		strcpy( g_pub_tx.reply, "H019" );
		goto ErrExit;
	}
	vtcp_log("aaaaaaaaa[%s]\n",sAg_unitinfo.bank_acct_no);
		/**��⹫˾�ʻ���״̬**/
		memset( &sMdm_ac_rel, 0x00, sizeof(sMdm_ac_rel) );
		ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdm_ac_rel, \
			"ac_no = '%s'", sAg_unitinfo.bank_acct_no );
		if ( ret )
		{
			sprintf(acErrMsg,"��ѯ�����˺Ŷ��ձ�ʧ��");
			WRITEMSG
			set_zd_data(DC_REPLY,g_pub_tx.reply);
			goto ErrExit;
		}
		
	if(sAg_unitinfo.bank_acct_no[0]=='9')
  {
		 memset(&sIn_mst,0x00,sizeof(sIn_mst));
	   ret=In_mst_Sel(g_pub_tx.reply,&sIn_mst,"ac_id=%ld and ac_seqn= 0",sMdm_ac_rel.ac_id);
	   if(ret)
	   {
				sprintf(acErrMsg,"��ѯ�ڲ����ļ�ʧ��!");
				WRITEMSG
				set_zd_data(DC_REPLY,g_pub_tx.reply);
				goto ErrExit;
	   }
	   if(sMdm_ac_rel.note_sts[0]!='0' || sIn_mst.sts[0]!='1')
		 {
				sprintf(acErrMsg,"�˻�״̬������[%s]", sAg_unitinfo.bank_acct_no );
				WRITEMSG
				strcpy( g_pub_tx.reply, "H059" );
				set_zd_data(DC_REPLY,g_pub_tx.reply);
				goto ErrExit;
		 }
	}
	else
	{
				memset( &sDd_mst, 0x00, sizeof(sDd_mst) );
				ret = Dd_mst_Sel( g_pub_tx.reply , &sDd_mst, "ac_id = %ld \
					and ac_seqn = 1", sMdm_ac_rel.ac_id );
				if ( ret )
				{
					sprintf( acErrMsg,"��ѯ�������ļ�ʧ��[%d][%ld][%s]", ret, sMdm_ac_rel.ac_id, sAg_unitinfo.bank_acct_no );
					WRITEMSG
					set_zd_data(DC_REPLY,g_pub_tx.reply);
					goto ErrExit;
				}
				if ( sMdm_ac_rel.note_sts[0] != '0' || sDd_mst.ac_sts[0] != '1' \
					|| sDd_mst.hold_sts[0] == '1' )	
				{
					sprintf(acErrMsg,"�˻�״̬������[%s]", sAg_unitinfo.bank_acct_no );
					WRITEMSG
					strcpy( g_pub_tx.reply, "H059" );
					set_zd_data(DC_REPLY,g_pub_tx.reply);
					goto ErrExit;	
				}
	}
	/**********��ӶԻ�����ҵ�˻��˻�״̬�ļ����������ļ�� 20130727 hzh start************/
		memset( &sMdm_ac_rel, 0x00, sizeof(sMdm_ac_rel) );
		ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdm_ac_rel, \
		"ac_no = '%s' and note_sts= '0'", cAc_no );
		if ( ret )
		{
			sprintf(acErrMsg,"��ѯ�����˺Ŷ��ձ�ʧ��");
			WRITEMSG
			set_zd_data(DC_REPLY,g_pub_tx.reply);
			goto ErrExit;
		}
		memset( &sDd_mst, 0x00, sizeof(sDd_mst) );
		ret = Dd_mst_Sel( g_pub_tx.reply , &sDd_mst, "ac_id = %ld \
		and ac_seqn = 1", sMdm_ac_rel.ac_id );
		if ( ret )
		{
			sprintf( acErrMsg,"��ѯ�������ļ�ʧ��[%d][%ld][%s]", ret, sMdm_ac_rel.ac_id, sAg_unitinfo.bank_acct_no );
			WRITEMSG
			set_zd_data(DC_REPLY,g_pub_tx.reply);
			goto ErrExit;
		}
		if ( sMdm_ac_rel.note_sts[0] != '0' || sDd_mst.ac_sts[0] != '1' \
			|| sDd_mst.hold_sts[0] == '1' )	
		{
			sprintf(acErrMsg,"�˻�״̬������[%s]", sMdm_ac_rel.ac_no );
			WRITEMSG
			strcpy( g_pub_tx.reply, "H059" );
			set_zd_data(DC_REPLY,g_pub_tx.reply);
			goto ErrExit;	
		}
		if(sDd_mst.bal<dT_amt)
		{
			sprintf(acErrMsg,"�˻�������[%s]", sMdm_ac_rel.ac_no);
			WRITEMSG
			strcpy( g_pub_tx.reply, "T066" );
			set_zd_data(DC_REPLY,g_pub_tx.reply);
			goto ErrExit;
		}
	/**********��ӶԻ�����ҵ�˻��˻�״̬�ļ����������ļ�� 20130727 hzh end************/

	fp = fopen( pcfilename,"r" );
	if( fp==NULL )
	{
		sprintf(acErrMsg," open ���� file [%s] error ",pcfilename );
		WRITEMSG
		strcpy( g_pub_tx.reply,"S047" );
		goto ErrExit;
	}
	sprintf(acErrMsg,"���ļ�����[%s]", pcfilename );
	WRITEMSG
	if ( cHand_flag[0] != '0' )	/*---���ֹ�ȥ����һ��---*/
	{
		memset( tmpstr, 0x0, sizeof(tmpstr) );
		fgets( tmpstr, 1024, fp );
	}
		ins_flag=0;

	while(1)
	{
		iProc_flag = 0;
		iProc_flag1 = 0;
		iK_flag = 0;

		fgets( tmpstr,1024,fp );
		if( feof(fp) )
		{
			break;
		}

		memset(fldstr,0x0,sizeof(fldstr));
		for( i=0;i<6;i++ )
		{
			ret=pub_base_cut_str( tmpstr,fldstr[i],"|",i+1 );
				vtcp_log(" [%d] [%s] ",i+1,fldstr[i]);
			pub_base_strpack( fldstr[i] );
		}

		if ( strlen(fldstr[0]) == 0 && strlen(fldstr[1]) == 0 )
		{
			sprintf(acErrMsg,"*******[%s],[%s],[%s]", pcfilename,fldstr[0],fldstr[1] );
			WRITEMSG
			continue;
		}
		
		/**�����¾��˺�ת��**/
		memset(newAcno,0x0,sizeof(newAcno));
		strncpy(newAcno,fldstr[2],strlen(fldstr[2]));
		pub_base_old_acno(newAcno);
		
		/**�������**/
		sprintf(acErrMsg,"3333333333333333[%s]", pcfilename );
		WRITEMSG
			memset( &sAg_peritemmap1, 0x0, sizeof(sAg_peritemmap1) );
			ret = Ag_peritemmap_Sel( g_pub_tx.reply , &sAg_peritemmap1,"unit_no = '%s' and ac_no = '%s' and stat='0'",cUnit_no, newAcno);
			if(ret)
			{
						if (memcmp(newAcno,"621",3) == 0)
						{
							sprintf( cReturn, "��λ��Ŵ�����߿��Ų�����");
							sprintf( acErrMsg, "��λ��Ŵ�����߿��Ų�����[%s][%s]",cUnit_no,fldstr[2]);
						}
						else
						{
							sprintf( cReturn, "��λ��Ż��˺���Ϣ����");
							sprintf( acErrMsg, "��λ��Ż��˺���Ϣ����[%s][%s]",cUnit_no,fldstr[2]);
						}
						WRITEMSG
						iProc_flag1 = 1;
						iErr_flag = 1;
						goto TrLog;
			}
		/**------------ ÿ��ѭ�������³�ʼ��ȫ�ֱ��� -------------**/
		memset( &g_pub_tx, 0x00, sizeof(g_pub_tx) );
		pub_base_sysinit();
		/*�Դ��մ��� ��ˮ��ѵ�add by ligl 2006-11-24 21:06*/
		if(sAg_unitinfo.opr_attri[0]=='0')
	{
			memset( &sAg_peritemmap, 0x0, sizeof(sAg_peritemmap));
			pub_base_old_acno(fldstr[2]);
			  ret = Ag_peritemmap_Sel( g_pub_tx.reply , &sAg_peritemmap,"unit_no = '%s' and ac_no = '%s'",sAg_unitinfo.unit_no, fldstr[2]);
		if(ret==0)
		{
		  pub_base_strpack_all( sAg_peritemmap.name );
					pub_base_strpack_all( fldstr[0] );
					if ( strcmp( sAg_peritemmap.name, fldstr[0] ) )
					{
							sprintf( cReturn, "��������,ԭ����Ϊ[%s]", sAg_peritemmap.name );
							sprintf( acErrMsg, "��������,ԭ����Ϊ[%s]", sAg_peritemmap.name );
							WRITEMSG
							iProc_flag1 = 1;
							iErr_flag = 1;
							goto TrLog;
					}
				memset( &sMdm_ac_rel, 0x0, sizeof(sMdm_ac_rel));
				ret1 = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdm_ac_rel,
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
				ret1 = Dd_mst_Sel( g_pub_tx.reply, &sDd_mst,
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
				if ( sDd_mst.ac_sts[0] == '*'||sDd_mst.hold_sts[0]=='1' )
				{
					strcpy( cReturn, "������,��ȫ����" );
					sprintf( acErrMsg, "������,�¿���[%s]", sAg_peritemmap.ac_no);
					WRITEMSG
					iK_flag = 1;
					iProc_flag = 1;
					iErr_flag = 1;
					goto TrLog;
				}
				if(sDd_mst.sttl_type[0]=='S')
				{
					strcpy( cReturn, "�û�Ϊ�ǽ��㻧,����ת��" );
					sprintf( acErrMsg, "�û�Ϊ�ǽ��㻧,����ת��[%s]", sAg_peritemmap.ac_no);
					WRITEMSG
					iProc_flag1 = 1;
					iErr_flag = 1;
					goto TrLog;
				}

		}
		else if(ret==100)
		{
			strcpy( cReturn, "����������" );
			sprintf(acErrMsg, "��ѯ���ռ�¼����[%s][%s]", cUnit_no, fldstr[2]);
			WRITEMSG
			iProc_flag1 = 1;
			iErr_flag = 1;
			goto TrLog;
		}
		else
		{
			sprintf(acErrMsg, "��ѯ������¼����[%s][%s][%d]", cUnit_no, fldstr[2], ret );
			WRITEMSG
			goto ErrExit;
		}
	/*---------------�����������ݲ��ִ���-----------------*/
		memset(&sPay_agent,0x0,sizeof(sPay_agent));
		strcpy(sPay_agent.unit_no, sAg_unitinfo.unit_no );
		strcpy(sPay_agent.item_no, sAg_peritemmap1.item_no );
		strcpy(sPay_agent.name, fldstr[0] );
		strcpy(sPay_agent.ac_no, fldstr[2]);
		sPay_agent.trace_no = g_pub_tx.trace_no;
		sPay_agent.tx_date = g_pub_tx.tx_date;
		sPay_agent.end_date = lEnd_date;
		sPay_agent.tram = atof(fldstr[1]);
		strcpy(sPay_agent.remark, cRemark );
		strcpy(sPay_agent.sata,"1");	   /*-- ��Ҫ�������� --*/
		strcpy(sPay_agent.result,"1");	 /*-- δ���� --*/
		vtcp_log("[%s][%d],======fldstr[4]========[%s]\n",__FILE__,__LINE__,fldstr[4]);
		strcpy(sPay_agent.cnt,fldstr[4]);/*--���ո�����--*/
		vtcp_log("[%s][%d],======fldstr[4]========[%s]\n",__FILE__,__LINE__,fldstr[4]);
		ret = 0;
		ret = Pay_agent_Ins( sPay_agent, g_pub_tx.reply  );
		if (ret != 0)
		{
			if ( ret ==-268)
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
		if ( iProc_flag1 == 0 )
		{
			strcpy( cReturn, "�ɹ�" );
		}
	}
	/**end by ligl 2006-11-24 21:06**/
	else
	{
		sprintf(acErrMsg,"4444444444444444[%s]", pcfilename );
		WRITEMSG
		memset( &sAg_peritemmap, 0x0, sizeof(sAg_peritemmap) );
		ret = Ag_peritemmap_Sel( g_pub_tx.reply , &sAg_peritemmap,
			"unit_no = '%s' and item_no = '%s' and stat='0'",
			cUnit_no, sAg_peritemmap1.item_no);
		if ( ret == 0 )
		{
			pub_base_strpack( sAg_peritemmap.id_no );
			pub_base_strpack_all( sAg_peritemmap.name );

			pub_base_strpack_all( fldstr[0] );

			if ( strcmp( sAg_peritemmap.name, fldstr[0] ) )
			{
				sprintf( cReturn, "��������,ԭ����Ϊ[%s]", sAg_peritemmap.name );
				sprintf( acErrMsg, "��������,ԭ����Ϊ[%s]", sAg_peritemmap.name );
				WRITEMSG
				iProc_flag1 = 1;
				iErr_flag = 1;
				goto TrLog;
			}

			if ( strcmp(sAg_peritemmap.id_no, fldstr[6]) &&
				strcmp( sAg_peritemmap.name, fldstr[0]) &&
				sAg_unitinfo.opr_attri[0] == '1' &&
				sAg_unitinfo.auto_flag[0] == '1') /*-֤������ͻ�����ͬ,����-*/
			{
				iLen = 0; ins_flag=1;
				iLen = strlen( fldstr[6] );
				if ( iLen == 0 )
				{
					sprintf( cReturn, "�¿���,֤�����벻��Ϊ��" );
					sprintf( acErrMsg, "�¿���,֤�����벻��Ϊ��" );
					WRITEMSG
					iProc_flag1 = 1;
					iErr_flag = 1;
					goto TrLog;
				}

				strcpy( cReturn, "�¿���" );
				sprintf(acErrMsg,"�¿���[%s]", sAg_peritemmap.item_no);
				WRITEMSG
				iK_flag = 1;
				iProc_flag = 1;
			}
			/*-----------------------------------------------------------*/
			pub_base_strpack( sAg_peritemmap.ac_no );
			pub_base_old_acno( sAg_peritemmap.ac_no );

			if ( sAg_unitinfo.opr_attri[0] == '1' && strcmp(sAg_unitinfo.pay_type, "37" )
				&& strcmp(sAg_unitinfo.pay_type, "38" ) && strcmp(sAg_unitinfo.pay_type, "39" ) )
			{
				memset( &sMdm_ac_rel, 0x0, sizeof(sMdm_ac_rel) );
				ret1 = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdm_ac_rel,
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
				
				if(strlen(fldstr[2]) != strlen(sAg_peritemmap.ac_no) || memcmp(fldstr[2],sAg_peritemmap.ac_no,strlen(fldstr[2])) != 0)
				{
						if (memcmp(fldstr[2],"621",3) == 0)
						{
							sprintf( cReturn, "��λ��Ŵ�����߿��Ų�����");
							sprintf( acErrMsg, "��λ��Ŵ�����߿��Ų�����[%s][%s]",cUnit_no,fldstr[2]);
						}
						else
						{
							sprintf( cReturn, "��λ��Ż��˺���Ϣ����");
							sprintf( acErrMsg, "��λ��Ż��˺���Ϣ����[%s][%s]",cUnit_no,fldstr[2]);
						}
						WRITEMSG
						iProc_flag1 = 1;
						iErr_flag = 1;
						goto TrLog;
				}

				memset( &sDd_mst, 0x0, sizeof(sDd_mst) );
				ret1 = Dd_mst_Sel( g_pub_tx.reply, &sDd_mst,
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
				
				if( sDd_mst.hold_sts[0]=='1' )
				{
					strcpy( cReturn, "���˻���ȫ����,����ת��" );
					sprintf( acErrMsg, "���˻���ȫ����,����ת��[%s]", sAg_peritemmap.ac_no);
					WRITEMSG
					iProc_flag1 = 1;
					iErr_flag = 1;
					goto TrLog;
				}
				
				if( sMdm_ac_rel.note_sts[0]!='0' )
				{
					strcpy( cReturn, "���˻�����״̬������,����ת��" );
					sprintf( acErrMsg, "���˻�����״̬������,����ת��[%s]", sAg_peritemmap.ac_no);
					WRITEMSG
					iProc_flag1 = 1;
					iErr_flag = 1;
					goto TrLog;
				}
				
				if ( sDd_mst.ac_sts[0] == '*')
				{
					strcpy( cReturn, "���˻�������" );
					sprintf( acErrMsg, "���˻�������,���¿���[%s]", sAg_peritemmap.item_no );
					WRITEMSG

					iLen = 0;
					iLen = strlen( fldstr[6] );
					if ( iLen == 0 && sDd_mst.ac_sts[0] == '*' )
					{
						sprintf( cReturn, "���˻�������,���¿���" );
						sprintf( acErrMsg, "�����¿�,֤�����벻��Ϊ��" );
						WRITEMSG
						iProc_flag1 = 1;
						iErr_flag = 1;
						goto TrLog;
					}

					iK_flag = 1;
					iProc_flag = 1;
				}
				
				if(sDd_mst.sttl_type[0]=='S')
				{
					strcpy( cReturn, "�û�Ϊ�ǽ��㻧,����ת��" );
					sprintf( acErrMsg, "�û�Ϊ�ǽ��㻧,����ת��[%s]", sAg_peritemmap.ac_no);
					WRITEMSG
					iProc_flag1 = 1;
					iErr_flag = 1;
					goto TrLog;
				}
				
				/**���ӶԿͻ�����������**/
				daliCnt = sql_count("pay_agent","ac_no='%s' and unit_no='%s'",newAcno,cUnit_no);
				if( daliCnt<0 )
				{
					sprintf(acErrMsg,"sql_count error!! [%d]",daliCnt);
					WRITEMSG
					goto ErrExit;
				}
				if((daliCnt+1)!=atoi(fldstr[4]))
				{
						sprintf( cReturn, "�����������Ӧ��Ϊ[%d]",daliCnt+1);
						sprintf( acErrMsg, "�����������[%s][%s]",cUnit_no,fldstr[2]);
						WRITEMSG
						iProc_flag1 = 1;
						iErr_flag = 1;
						goto TrLog;
				}
			}
		}
		else if ( ret == 100 )
		{
				strcpy( cReturn, "�����Ų�����" );
				sprintf(acErrMsg, "��ѯ������¼����[%s][%s]", cUnit_no, sAg_peritemmap1.item_no);
				WRITEMSG
				iProc_flag1 = 1;
				iErr_flag = 1;
				goto TrLog;
		}
		else
		{
			sprintf(acErrMsg, "��ѯ������¼����[%s][%s][%d]", cUnit_no, sAg_peritemmap1.item_no, ret );
			WRITEMSG
			goto ErrExit;
		}

		/*---------------�����������ݲ���-----------------*/
		memset(&sPay_agent,0x0,sizeof(sPay_agent));

		strcpy(sPay_agent.unit_no, sAg_unitinfo.unit_no );
		strcpy(sPay_agent.item_no, sAg_peritemmap1.item_no );
		strcpy(sPay_agent.name, fldstr[0] );
		strcpy(sPay_agent.ac_no, sAg_peritemmap.ac_no );
		sPay_agent.trace_no = g_pub_tx.trace_no;
		sPay_agent.tx_date = g_pub_tx.tx_date;
		sPay_agent.end_date = lEnd_date;
		sPay_agent.tram = atof(fldstr[1]);
		strcpy(sPay_agent.remark, cRemark );
		strcpy(sPay_agent.sata,"1");	/*-- ��Ҫ�������� --*/
		strcpy(sPay_agent.result,"1");	/*-- δ���� --*/
		vtcp_log("[%s][%d],======fldstr[4]========[%s]\n",__FILE__,__LINE__,fldstr[4]);
		strcpy(sPay_agent.cnt,fldstr[4]);/*--���ո�����--*/
		vtcp_log("[%s][%d],======fldstr[4]========[%s]\n",__FILE__,__LINE__,fldstr[4]);
		if ( sAg_unitinfo.opr_attri[0] == '0' )	/*���۴���20050902*/
		{
			ret = sql_count( "pay_agent", "unit_no = '%s' and item_no = '%s'and cnt='%s' \
				and end_date >= %ld", cUnit_no, sAg_peritemmap1.item_no,fldstr[4], g_pub_tx.tx_date );
			if ( ret > 0 )
			{
				strcpy( cReturn, "���ۼ�¼�Ѵ���,���������");
				sprintf(acErrMsg,"���ۼ�¼�Ѵ���,���������!");
				WRITEMSG
				iProc_flag1 = 1;
				iErr_flag = 1;
				goto TrLog;
			}
		}

		ret = 0;
		ret = Pay_agent_Ins( sPay_agent, g_pub_tx.reply  );
		if (ret != 0)
		{
			if ( ret ==-268)
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
			Pay_agent_Debug(&sPay_agent);
			iProc_flag1 = 1;
			iErr_flag = 1;
			goto TrLog;
		}
		if ( iProc_flag1 == 0 )
		{
			strcpy( cReturn, "�ɹ�" );
		}
		}
TrLog:
		if ( iProc_flag1 == 0 )
		{
			lSnum = lSnum + 1;
			dSamt = dSamt + atof(fldstr[1]);
			fprintf( fp1, "%10s��%20s��%30s��%20s��%10s��%20s��%20s��\n",sAg_peritemmap1.item_no, sAg_peritemmap.ac_no ,fldstr[0],
			fldstr[1], "Y", cReturn ,fldstr[5]);

			if ( iK_flag == 1 )
			{
			lK_num = lK_num + 1;	/*�¿�����*/
			dK_amt = dK_amt + atof(fldstr[1]);	/*�¿����*/
			}
		}
		else
		{
			lFnum = lFnum + 1;
			dFamt = dFamt + atof(fldstr[1]);
			fprintf( fp1, "%20s��%20s��%16s��%20s��%20s��%8s��%20s��\n",sAg_peritemmap1.item_no, fldstr[2] ,fldstr[0],
			fldstr[1], "N", cReturn ,fldstr[5]);
		}
	}

 fprintf( fp1, "\n" );

	fclose(fp);
	fclose(fp1);
	
	sprintf(acErrMsg,"�ɹ�����[%ld]���[%.2f];ʧ�ܱ���[%ld]���[%.2f]", lSnum, dSamt, lFnum, dFamt );
	
	ret = sql_execute( "update ag_unitinfo set beg_date = %ld, end_date = %ld \
		where unit_no = '%s'", lBeg_date, lEnd_date, cUnit_no );
	if ( ret )
	{
		sprintf( acErrMsg, "���ĵ�λ�����������ڴ�!!!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "H020" );
		ins_flag=1;
		goto ErrExit;
	}

	if ( iErr_flag != 0 ){
		/*              �Ǵ���ҵ��           �����ݿ�����        �ǵ��̷�ʽ   */
		if(sAg_unitinfo.opr_attri[0]=='1' && ins_flag==0  && cHand_flag[0] != '0'){
			vtcp_log("[%s][%d]���̷�ʽ���벿������",__FILE__,__LINE__);
			strcpy( cReturn, "�뿴 8608 ���ײ鿴������");
			memset( cFtpfile, 0x0, sizeof(cFtpfile) );
			sprintf(cFtpfile,"%s/bin/ftp_to_nsw.sh %s %s %s",getenv("WORKDIR"),get_env_info("NSW_FTP_USER"),get_env_info("NSW_FTP_PWD"),filename2);
			vtcp_log("%s,%d cFtpfile=[%s]\n",__FILE__,__LINE__,cFtpfile);
			ret=system(cFtpfile);
			if (ret)
			{
			sprintf(acErrMsg,"ftp�ļ�[%s]��ƽ̨����[%d]",filename2,ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P157");
			goto ErrExit;
			}
			sprintf(acErrMsg,"ftp�ļ�[%s]��ƽ̨�ɹ�",filename2);
			WRITEMSG
			goto OkExit;
		}
		/* ������ʽ����Ϊʧ��,����ȫ���ع� */
		vtcp_log("[%s][%d]��������ʧ��\n",__FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "H034" );
		goto ErrExit;
	}
OkExit:
	/**����������Ҫ��������ӶԳɹ����ļ��� 20130726 hzh start**/
	/****************�跽���˸�ֵ********************/
	set_zd_data("0660","1");/**�����־**/
	set_zd_data("0670","2");/**��ת��־**/
	set_zd_data("0680","1");/**��������**/
	set_zd_data("0880","����ת��");/**ժҪ**/
	set_zd_data("0210","01");/**����**/
	/**89,58�򿴿�������ʲô��;**/
	/****/
	strcpy( g_pub_tx.ac_get_ind,"00" ); /*������δ��ȡ�ֻ�*/
	strcpy( g_pub_tx.prt_ind,"0" ); /*������*/
	strcpy( g_pub_tx.ac_wrk_ind,"000" ); /*���˶�ƾ֤�ţ��������ˮ����ϸ*/
	strcpy(g_pub_tx.hst_ind,"1"); /*�ռ�����ϸ*/
	/**ֻ�Գɹ��Ǽǵ�pay_agent�����ո�δ�����¼�������ݼ���**/
	set_zd_double("0400",dSamt);
	/****************�������˸�ֵ********************/
	set_zd_data("1011",sAg_unitinfo.bank_acct_no);/**ת���ڲ��˻�**/
	set_zd_data("1012","0");/**�˻����**/
	set_zd_data("1016","����ת��");/**ժҪ**/
	set_zd_data("101A","01");/**����**/
	set_zd_data("101B","01");/**��ת��־**/
	set_zd_data("101C","031");/**����ժҪ��־**/
	set_zd_double("1013",dSamt);
	/**1016,101C,1014,1015**/
	vtcp_log("%s,%d �ɹ��Ǽ�����ո�δ�����¼��pay_agent���ܽ�Ϊ[%lf]\n",__FILE__,__LINE__,dSamt);
	vtcp_log("%s,%d ���˸�ֵ����������\n",__FILE__,__LINE__);
	/**����������Ҫ��������ӶԳɹ����ļ��� 20130726 hzh end**/
	/**��ȷʱʱҲ��ƽ̨����ϸ�ļ�**/
	memset( cFtpfile, 0x0, sizeof(cFtpfile) );
	sprintf(cFtpfile,"%s/bin/ftp_to_nsw.sh %s %s %s",getenv("WORKDIR"),get_env_info("NSW_FTP_USER"),get_env_info("NSW_FTP_PWD"),filename2);
	vtcp_log("%s,%d cFtpfile=[%s]\n",__FILE__,__LINE__,cFtpfile);
	ret=system(cFtpfile);
	if (ret)
	{
	sprintf(acErrMsg,"ftp�ļ�[%s]��ƽ̨����[%d]",filename2,ret);
	WRITEMSG
	strcpy(g_pub_tx.reply,"P157");
	goto ErrExit;
	}
	sprintf(acErrMsg,"ftp�ļ�[%s]��ƽ̨�ɹ�",filename2);
	WRITEMSG
	/**��ȷʱҲ��ƽ̨����ϸ�ļ�**/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"��������ҵ������ϣ�[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;

ErrExit:
	fclose(fp);
	fclose(fp1);
	sprintf(acErrMsg,"%s,%d,��������ʧ��ins[%d],iErr[%d]",__FILE__,__LINE__,ins_flag,iErr_flag);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}