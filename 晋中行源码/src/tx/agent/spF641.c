/*************************************************
* �� �� ��: spF641.c
* ��������������ҵ����������(�˽�����641���ƶ���,�������޸ģ�Ϊ�����winqt��������������)
*
* ��    ��: jack
* �������: 2004��07��19��
*
* �޸�����:
* �� �� ��: 
* �޸�����: 
*insert into tx_Def (TX_CODE, TX_NAME, BIT_MAP, LOG_IND, TX_TYPE, WRK_STS)
values ('6641', 'winqt������������', '10000000000000000000000000000000000000000000000000000000000000000010000000000000000000000000000000000000000000000000000000000000', '1', '6', '2');
insert into tx_flow_def (TX_CODE, FLOW_CODE, SUB_TX_CODE, SUB_TX_RSLT)
values ('6641', 0, 'F641', '#$');
insert into tx_sub_def (SUB_TX_CODE, SUB_TX_NAME, TX_FUNC, ACCT_IND, CHNL_IND)
values ('F641', 'winqt������������', 'spF641', '0', '0');
---------------
insert into auth_item (TX_CODE, SEQN, CODE_AC, CODE_SEQN, CODE_AMT, CODE_DC, CODE_CT, CODE_XX, CODE_YY, CODE_ZZ, IND, MO)
values ('6641', 1, '', '', '', '', '', '', '', '', '0000', 'winqt������������');

insert into auth_cond (TYPE, NO, TX_CODE, SEQN, MO, COND)
values ('S', 0, '6641', 1, 'winqt������������', 'sq(1237)');
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#define EXTERN
#define cIdtp "1"
#include "public.h"
#include <errno.h>
#include "pay_agent_c.h"
#include "ag_peritemmap_c.h"
#include "ag_unitinfo_c.h"
#include "ag_paytlcs_c.h"
#include "cif_basic_inf_c.h"
#include "cif_id_code_rel_c.h"
#include "com_branch_c.h"
#include "rsp_code_c.h"
spF641()
{
	int ret = 0;
	int ret1 = 0;
	char cReturn[51];
	char execstr[248];
	char flag[2];
	int iProc_flag = 0;	/*	1�����Ӷ�Ӧ��ϵ;2���޸Ķ�Ӧ��ϵ		*/
	int iErr_flag = 0;	/*	0��������ȷ��1������ʧ��	*/
	int iErr_flag1 = 0;	/*	0��������ȷ��1������ʧ��	*/
	
	long cif_no = 0;
	char xs_acno[20];
	char jz_acno[20];
	int jz_ac_seq = 0;
	/*add by wudw 20130403 end*/
	char cTrace_no[7];
	/* end */
	long lSnum = 0;		/*�ɹ�����*/
	long lFnum = 0;		/*ʧ�ܱ���*/
	double dSamt = 0.00;	/*�ɹ����*/
	double dFamt = 0.00;	/*ʧ�ܽ��*/
	
	int iK_flag = 0;	/*�¿�����־*/
	long lK_num = 0;	/*�¿�����*/
	double dK_amt = 0.00;	/*�¿����*/
	/*****/
  char cOld_unit_no[5];
  char cOld_item_no[21];
  char cOld_ac_no[20];
  char cOld_cnt[3];
  /****/
	char filename[248];
	char filename1[248];
	char filename2[248];
	char filename3[248];
	char filename4[248];
	FILE *fp = NULL;
	FILE *fp1 = NULL;
	FILE *fp3 = NULL;
	int i=0;
	int lSsum1=0;
	char cUnit_no[5];

	struct pay_agent_c sPay_agent;
	struct ag_peritemmap_c sAg_peritemmap;
	struct ag_unitinfo_c sAg_unitinfo;
	struct ag_paytlcs_c sAg_paytlcs;
	struct dd_mst_c sDd_mst;
	struct in_mst_c sIn_mst;
	struct mdm_ac_rel_c sMdm_ac_rel;
	struct mdm_ac_rel_c mdm_ac_rel_t1;
	struct cif_basic_inf_c cif_basic_inf;
	struct cif_id_code_rel_c cif_id_code_rel;
	struct com_branch_c sCom_branch;
	struct rsp_code_c sRsp_code;
  
	/**------- ��ʼ��ȫ�ֱ��� --------**/
	pub_base_sysinit();
	/**********/
	memset( cUnit_no, 0x0, sizeof(cUnit_no) );
	/***********/
	memset(cOld_unit_no,0x0,sizeof(cOld_unit_no));
	memset(cOld_item_no,0x0,sizeof(cOld_item_no));
	memset(cOld_ac_no,0x0,sizeof(cOld_ac_no));
	memset(cOld_cnt,0x0,sizeof(cOld_cnt));

	/**********/
	get_zd_data( "0920", cUnit_no );
  memset(flag,0x00,sizeof(flag));
	memset( &sAg_unitinfo, 0x0, sizeof(sAg_unitinfo) );
	ret = Ag_unitinfo_Sel( g_pub_tx.reply , &sAg_unitinfo, 
		"unit_no = '%s'", cUnit_no );
	if ( ret )
	{
		sprintf(acErrMsg,"ִ��Ag_unitinfo_Dec_Sel����[%d]", ret );
		WRITEMSG
		set_zd_data(DC_REPLY,g_pub_tx.reply);
   		return 1;
	}
	pub_base_strpack( sAg_unitinfo.unit_no );
	pub_base_strpack( sAg_unitinfo.unit_name );
	pub_base_strpack( sAg_unitinfo.opn_br_no );
	pub_base_strpack( sAg_unitinfo.pay_type );
	
	if ( strcmp( sAg_unitinfo.pay_type, "35" ) == 0 )
	{
		sprintf( acErrMsg, "�ֽ����ҵ������6605����!!!!!" );
		set_zd_data( "0130", acErrMsg );
		strcpy( g_pub_tx.reply, "H034" );
		set_zd_data( DC_REPLY, g_pub_tx.reply );
   		return 1;
	}
	
	if ( strcmp( g_pub_tx.tx_br_no, sAg_unitinfo.opn_br_no ) )
	{
		sprintf( acErrMsg, "�Ǵ���λ��������,����[%s]������Ա���˽���", sAg_unitinfo.opn_br_no );
		WRITEMSG
		sprintf( acErrMsg, "����[%s]������Ա���˽���", sAg_unitinfo.opn_br_no );
		set_zd_data( "0130", acErrMsg );
		strcpy( g_pub_tx.reply, "H034" );
		set_zd_data( DC_REPLY, g_pub_tx.reply );
   		return 1;
	}
	
	/*--------------  ��鵥λ�˺�״̬  ---------------*/
	memset( &sMdm_ac_rel, 0x00, sizeof(sMdm_ac_rel) );
	ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdm_ac_rel, 
		"ac_no = '%s'", sAg_unitinfo.bank_acct_no );
	if ( ret )
	{
		sprintf(acErrMsg,"��ѯ�����˺Ŷ��ձ�ʧ��");
		WRITEMSG
		set_zd_data(DC_REPLY,g_pub_tx.reply);
		return 1;
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
			return 1;
	  }
	  if(sMdm_ac_rel.note_sts[0]!='0' ||sIn_mst.sts[0]!='1')
	  {
	  	sprintf(acErrMsg,"�˻�״̬������[%s]", sAg_unitinfo.bank_acct_no );
			WRITEMSG
			strcpy( g_pub_tx.reply, "H059" );
			set_zd_data(DC_REPLY,g_pub_tx.reply);
			return 1;
	  }
	}
	else
	{
		memset( &sDd_mst, 0x00, sizeof(sDd_mst) );
		ret = Dd_mst_Sel( g_pub_tx.reply , &sDd_mst, "ac_id = %ld and ac_seqn = 1", sMdm_ac_rel.ac_id );		
		if ( ret )
		{
			sprintf(acErrMsg,"��ѯ�������ļ�ʧ��!");
			WRITEMSG
			set_zd_data(DC_REPLY,g_pub_tx.reply);
			return 1;
		}
	
		if ( sMdm_ac_rel.note_sts[0] != '0' || sDd_mst.ac_sts[0] != '1' 
			|| sDd_mst.hold_sts[0] == '1' )	/*��������ȫ����*/
		{
			sprintf(acErrMsg,"�˻�״̬������[%s]", sAg_unitinfo.bank_acct_no );
			WRITEMSG
			strcpy( g_pub_tx.reply, "H059" );
			set_zd_data(DC_REPLY,g_pub_tx.reply);
			return 1;
		}	
	}
	ret=sql_count("pay_agent","unit_no = '%s' and sata = '1' and result = '1'", cUnit_no );
	if(!ret){
		sprintf(acErrMsg,"�޴˵�λ�������ʼ�¼");
		WRITEMSG
		strcpy( g_pub_tx.reply, "D104" );
		set_zd_data(DC_REPLY,g_pub_tx.reply);
		goto ErrExit;;
	}
	/*------------ �������ļ� -----------*/
	memset(filename,0x0,sizeof(filename));
	/***modify by kevinchan Ϊ��ֹ�ļ�������¼�������ļ�����***/
	/*sprintf(filename,"%s/0%s%ld%s.txt",getenv("FILDIR"), sAg_unitinfo.unit_no, g_pub_tx.tx_date,g_pub_tx.tel);*/
	sprintf(filename,"%s/DF0%s%ld%s.txt",getenv("FILDIR"), sAg_unitinfo.unit_no, g_pub_tx.tx_date,g_pub_tx.tel);
	
	/*modify by kevinchan 20120319 start*/
	/*�������� ��һ����λͬһ��ִ������ �ļ����ݲ���ն���׷��*/
	/*fp = fopen( filename,"w" );	*//*���ļ�������¼��ϸ������ȷ���*/
	fp = fopen( filename,"a" );
	/*modify by kevinchan 20120319 end*/
	if( fp==NULL )
	{
		sprintf(acErrMsg," open file [%s] error ",filename );
		WRITEMSG
		strcpy( g_pub_tx.reply, "S047" );
		set_zd_data(DC_REPLY,g_pub_tx.reply);
		return 1;
	}
	
	/**modify by kevinchan 20120320 start**/
	/**�����ļ���ͷ������ ͳһΪ��3��**/	
	/*fprintf( fp, "                                 [%s][%s]��������ҵ��ɹ���¼\r\n\r\n", sAg_unitinfo.unit_no, sAg_unitinfo.unit_name );*/
	fprintf( fp, "\r\n\r\n\r\n                                   [%s][%s]��������ҵ��ɹ���¼\r\n", sAg_unitinfo.unit_no, sAg_unitinfo.unit_name );
  /**modify by kevinchan 20120320 start**/
	fprintf( fp, "    �������������ө����������ө����������������ө��������������������ө��������������������ө���������\r\n" );
  fprintf( fp, "    ��%10s��%10s��%16s��%20s��%20s��%8s��\r\n", "������", "�ͻ�����", "���", "֤������", "�˺�", "������" );
  fprintf( fp, "    �������������㩥���������㩥���������������㩥�������������������㩥�������������������㩥��������\r\n" );

	memset( filename1, 0x0, sizeof(filename1) );	
  /***modify by kevinchan Ϊ��ֹ�ļ�������¼�������ļ�����***/
	/*sprintf( filename1, "%s/1%s%ld%s.txt", getenv("FILDIR"), sAg_unitinfo.unit_no, g_pub_tx.tx_date,g_pub_tx.tel);*/
	sprintf( filename1, "%s/DF1%s%ld%s.txt", getenv("FILDIR"), sAg_unitinfo.unit_no, g_pub_tx.tx_date,g_pub_tx.tel);
	
		/*modify by kevinchan 20120319 start*/
	/*�������� ��һ����λͬһ��ִ������ �ļ����ݲ���ն���׷��*/
	/*fp1 = fopen( filename1, "w" );	*//*���ļ�������¼��ϸ��������*/
	fp1 = fopen( filename1, "a" );
	/*modify by kevinchan 20120319 end*/
	if( fp1==NULL )
	{
		sprintf(acErrMsg," open file [%s] error ",filename1 );
		WRITEMSG
		strcpy( g_pub_tx.reply,"S047" );
		fclose( fp );
		set_zd_data(DC_REPLY,g_pub_tx.reply);
		return 1;
	}

	/**modify by kevinchan 20120320 start**/
	/**�����ļ���ͷ������ ͳһΪ��3��**/	
	/*fprintf( fp1, "                                [%s][%s]��������ҵ��ʧ�ܼ�¼\r\n\r\n", sAg_unitinfo.unit_no, sAg_unitinfo.unit_name );*/
	fprintf( fp1, "\r\n\r\n\r\n                                [%s][%s]��������ҵ��ʧ�ܼ�¼\r\n", sAg_unitinfo.unit_no, sAg_unitinfo.unit_name );
  /**modify by kevinchan 20120320 start**/
	fprintf( fp1, "          �������������ө��������������������ө����������������ө��������������������ө���������������������������������������������������\r\n" );
  fprintf( fp1, "          ��%10s��%10s��%16s��%20s��%50s��\r\n", "������", "�ͻ�����", "���", "֤������", "������" );
  fprintf( fp1, "          �������������㩥�������������������㩥���������������㩥�������������������㩥��������������������������������������������������\r\n" );

	ret = Pay_agent_Dec_Sel( g_pub_tx.reply , "unit_no = '%s' and sata = '1' and result = '1' ", cUnit_no );
		/*order by item_no", cUnit_no );*/
	if ( ret )
	{
		sprintf(acErrMsg,"ִ��Pay_agent_Dec_Sel����[%d]", ret );
		WRITEMSG
		fclose( fp );
		fclose( fp1 );
		set_zd_data(DC_REPLY,g_pub_tx.reply);
   		return 1;
	}
	
	while(1)
	{
		
		iErr_flag = 0;
		iProc_flag = 0;

		memset( cReturn, 0x0, sizeof(cReturn) );
		
		memset( xs_acno,0x0,sizeof(xs_acno) );

		memset( &mdm_ac_rel_t1 , 0x00 , sizeof( struct mdm_ac_rel_c ) );
	
		memset( &sPay_agent, 0x0, sizeof(sPay_agent) );
		ret = Pay_agent_Fet_Sel( &sPay_agent , g_pub_tx.reply );
		if ( ret == 100 )
		{
			if ( sAg_unitinfo.opr_attri[0] == '0' )	/*����ҵ����*/
			{
			}
			
			if ( sAg_unitinfo.opr_attri[0] == '1' && lFnum == 0 )	/*����ҵ����ȫ���ɹ�*/
			{
				
				ret = sql_execute( "insert into pay_agent_bk select * from \
					pay_agent where unit_no = '%s'", sAg_unitinfo.unit_no );
				 lSsum1= sql_count("(select distinct unit_no,item_no,ac_no from  pay_agent)"," unit_no='%s'",sAg_unitinfo.unit_no);		
							   vtcp_log("[%s][%d]====����====[%ld]\r\n",__FILE__,__LINE__,lSsum1);

				ret = sql_execute( "delete from pay_agent where unit_no = '%s'", sAg_unitinfo.unit_no );
			}
			if ( sAg_unitinfo.opr_attri[0] == '0' && sAg_unitinfo.end_date == g_pub_tx.tx_date )	/*����ҵ��*/
			{
				ret = sql_execute( "insert into pay_agent_bk select * from \
					pay_agent where unit_no = '%s'", sAg_unitinfo.unit_no );
					 lSsum1= sql_count("(select distinct unit_no,item_no,ac_no from  pay_agent)"," unit_no='%s'",sAg_unitinfo.unit_no);						
				   vtcp_log("[%s][%d]====����====[%ld]\r\n",__FILE__,__LINE__,lSsum1);
				ret = sql_execute( "delete from pay_agent where unit_no = '%s'", 
					sAg_unitinfo.unit_no );
			}
			
			break;
		}
		if ( ret )
		{
			sprintf(acErrMsg,"ִ��Pay_agent_Fet_Sel����[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}
		
		/**------------ ÿ��ѭ�������³�ʼ��ȫ�ֱ��� -------------**/
		memset( &g_pub_tx, 0x00, sizeof(g_pub_tx) );
		pub_base_sysinit();		
	
		ret = Ag_peritemmap_Sel( g_pub_tx.reply , &sAg_peritemmap, 
			"unit_no = '%s' and item_no = '%s'", 
			sPay_agent.unit_no, sPay_agent.item_no );
		if ( ret == 0 )
		{
			pub_base_strpack( sAg_peritemmap.id_no );
			pub_base_strpack( sPay_agent.id_no );
			pub_base_strpack_all( sAg_peritemmap.name );
			pub_base_strpack_all( sPay_agent.name );
 			pub_base_strpack_all( sAg_peritemmap.id_type);
			/*--- ֤������ͻ�������ͬ,����ʱ��Ϊ�ǲ�ͬ���� --------*/
			if ( strcmp(sAg_peritemmap.id_no, sPay_agent.id_no) && 
				strcmp(sAg_peritemmap.name, sPay_agent.name) && 
				sAg_unitinfo.opr_attri[0] == '1' &&				
				sAg_unitinfo.auto_flag[0] == '1' && strcmp(sAg_unitinfo.pay_type, "37" ) 
				&& strcmp(sAg_unitinfo.pay_type, "38" ) && strcmp(sAg_unitinfo.pay_type, "39" ) )
			{
				strcpy( cReturn, "�¿���" );
				sprintf(acErrMsg,"�¿���[%s]", sAg_peritemmap.item_no);
				WRITEMSG
				iK_flag = 1;
				iProc_flag = 2;
				goto KAIHU;
			}
			/*----------------------------------------------------*/
			
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
					iErr_flag = 1;
					goto TrLog;
				}
				
				pub_base_strpack( sDd_mst.ac_sts );
				if ( sDd_mst.ac_sts[0] != '1' || sDd_mst.hold_sts[0] == '1' )	/*��������ȫ����*/
				{
					sprintf( cReturn, "�˺�[%s]״̬��", sAg_peritemmap.ac_no );
					sprintf( acErrMsg, "�˺�[%s]״̬��", sAg_peritemmap.ac_no );
					WRITEMSG
					iErr_flag = 1;
					goto TrLog;
				}	
			}
		}
		else if ( ret == 100 )
		{
			if ( sAg_unitinfo.opr_attri[0] == '0' )
			{
				strcpy( cReturn, "�����Ų�����" );
				sprintf(acErrMsg, "��ѯ������¼����[%s][%s]", sPay_agent.unit_no, sPay_agent.item_no);
				WRITEMSG
				iErr_flag = 1;
				goto TrLog;
			}
			
			pub_base_strpack( sPay_agent.ac_no );
			pub_base_old_acno( sPay_agent.ac_no );
			
			pub_base_strpack( sAg_unitinfo.pay_type );
			if ( strncmp( sAg_unitinfo.pay_type, "31", 2 ) == 0 )	/*---������������---*/
			{
				memset( &sMdm_ac_rel, 0x0, sizeof(sMdm_ac_rel) );
				ret1 = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdm_ac_rel, 
					"ac_no = '%s'", sPay_agent.ac_no );
				if ( ret1 )
				{
					sprintf( cReturn, "�����˺�[%s]��Ϣ��", sAg_peritemmap.ac_no );
					sprintf( acErrMsg, "�����˺�[%s]��Ϣ��", sAg_peritemmap.ac_no );
					WRITEMSG
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
					iErr_flag = 1;
					goto TrLog;
				}
				
				pub_base_strpack( sDd_mst.ac_sts );
				if ( sDd_mst.ac_sts[0] == '*' )
				{
					strcpy( cReturn, "�¿���" );
					sprintf( acErrMsg, "�¿���[%s]", sAg_peritemmap.item_no );
					WRITEMSG
					iK_flag = 1;
					iProc_flag = 1;
					goto KAIHU;
				}
			}
			
			if ( sAg_unitinfo.opr_attri[0] == '1' && 
				sAg_unitinfo.auto_flag[0] == '1' && 
				strncmp( sAg_unitinfo.pay_type, "31", 2 ) != 0 && strcmp(sAg_unitinfo.pay_type, "37" ) 
				&& strcmp(sAg_unitinfo.pay_type, "38" ) && strcmp(sAg_unitinfo.pay_type, "39" ) )	/*����,�Զ���*/
			{
				strcpy( cReturn, "�¿���" );
				sprintf(acErrMsg,"�¿���[%s]", sPay_agent.item_no);
				WRITEMSG
				iK_flag = 1;
				iProc_flag = 1;
				goto KAIHU;
			}
			else if ( sAg_unitinfo.opr_attri[0] == '1' && 
				sAg_unitinfo.auto_flag[0] == '1' && 
				strncmp( sAg_unitinfo.pay_type, "31", 2 ) == 0 )	/*��������Ҫ����*/
			{
				sprintf(acErrMsg, "ֱ�����Ӷ�Ӧ��ϵ[%s]", sPay_agent.item_no);
				WRITEMSG
				strcpy( sAg_peritemmap.ac_no, sPay_agent.item_no );
				iProc_flag = 1;
			}
			else
			{
				strcpy( cReturn, "�����Ų�����" );
				sprintf(acErrMsg, "��ѯ������¼����[%s][%s]", sPay_agent.unit_no, sPay_agent.item_no);
				WRITEMSG
				iErr_flag = 1;
				goto TrLog;
			}
		}
		else if( ret  )
		{
			strcpy( cReturn, "������������" );
			sprintf( acErrMsg,"��ѯ������¼����[%s][%s]", sPay_agent.unit_no, sPay_agent.item_no );
			WRITEMSG
			goto ErrExit;
		}		
	
		sprintf(acErrMsg,"---------------------��ʼ���ڴ�ȡ����-------------------");
		WRITEMSG
	
		strcpy( g_pub_tx.ac_no, sAg_peritemmap.ac_no );
		g_pub_tx.ac_seqn = 1;
		
		if ( !strcmp(sAg_unitinfo.pay_type, "37" ) || 
			!strcmp(sAg_unitinfo.pay_type, "38" ) || !strcmp(sAg_unitinfo.pay_type, "39" ) )
		{
			g_pub_tx.ac_seqn = 0;
		}
	
		g_pub_tx.tx_amt1 = sPay_agent.tram;
	
		if ( sAg_unitinfo.opr_attri[0] == '1' )
		{
			strcpy(g_pub_tx.add_ind,"1");	/*-- ���� --*/
			
			memset( g_pub_tx.sub_tx_code, 0x0, sizeof(g_pub_tx.sub_tx_code) );
			strcpy( g_pub_tx.sub_tx_code, "D099" );
			set_zd_data( DC_SUB_TX_CODE, g_pub_tx.sub_tx_code );
			
			strcpy( g_pub_tx.prt_ind, "0" );
			set_zd_data( "1017", "0" );

			set_zd_data( "101A", "01" );
			set_zd_data( "101B", "2" );
			set_zd_double( "1013", g_pub_tx.tx_amt1 );
			strcpy(g_pub_tx.brf, "����");
		}
		else
		{
			strcpy(g_pub_tx.add_ind,"0");	/*-- ���� --*/
			
			memset( g_pub_tx.sub_tx_code, 0x0, sizeof(g_pub_tx.sub_tx_code) );
			strcpy( g_pub_tx.sub_tx_code, "D003" );
			set_zd_data( DC_SUB_TX_CODE, g_pub_tx.sub_tx_code );
			
			strcpy( g_pub_tx.prt_ind, "0" );
			set_zd_data( "102O", "0" );
			
			set_zd_data( "102J", "01" );
			set_zd_data( "102K", "2" );
			set_zd_double( "102F", g_pub_tx.tx_amt1 );
			set_zd_double( "102P", 0.00 );
			set_zd_double( "102Q", 0.00 );
			strcpy(g_pub_tx.brf, "����");
		}
	/**	strcpy(g_pub_tx.brf, sPay_agent.remark);**/
		strcpy(g_pub_tx.ct_ind,"2");
		strcpy(g_pub_tx.ac_wrk_ind,"001001001");
		strcpy(g_pub_tx.hst_ind,"1");
		g_pub_tx.svc_ind=1001;
	
		sprintf(acErrMsg,"BEG D099[%s][%s][%d]",g_pub_tx.ac_no,g_pub_tx.ac_wrk_ind,g_pub_tx.svc_ind);
		WRITEMSG
		
		if ( sAg_unitinfo.opr_attri[0] == '0' )	/*����ҵ����*/
		{
			/*sql_commit();
			sql_begin();*/
		}
	
		ret = pub_acct_trance();
		if( ret != 0 )
		{
			if ( sAg_unitinfo.opr_attri[0] == '0' )
			{
				/*sql_rollback();*/
			}
			
			sprintf( cReturn, "%s", g_pub_tx.reply );
			sprintf(acErrMsg,"���ô�ȡ������ʧ��!");
			WRITEMSG
			iErr_flag = 1;
			goto TrLog;
		}
		
		ret = pubf_acct_proc( g_pub_tx.sub_tx_code );
		if ( ret )
		{
			if ( sAg_unitinfo.opr_attri[0] == '0' )
			{
				/*sql_rollback();*/
			}
			
			sprintf( cReturn, "%s", g_pub_tx.reply );
			sprintf(acErrMsg,"���ô�ȡ������ʧ��!");
			WRITEMSG
			iErr_flag = 1;
			goto TrLog;
		}
		
		strcpy( xs_acno, sAg_peritemmap.ac_no );
		
		goto NOKAIHU;

KAIHU:
		memset(&cif_id_code_rel, 0x0, sizeof(cif_id_code_rel));
		memset(&cif_basic_inf, 0x0, sizeof(cif_basic_inf));

		strcpy(cif_id_code_rel.id_type, sAg_peritemmap.id_type);/* ֤������ */
		
		pub_base_strpack( sPay_agent.id_no );
		if ( strlen( sPay_agent.id_no ) == 0 )
		{
			sprintf(acErrMsg,"����ʱ֤�����벻��Ϊ��"  );
			WRITEMSG
			sprintf( cReturn, "%s", "����ʱ֤�����벻��Ϊ��" );
			iErr_flag = 1;
			goto TrLog;
		}
		strcpy(cif_id_code_rel.id_no, sPay_agent.id_no);/*֤������*/

		strcpy(cif_basic_inf.type,"1");		      /* �ͻ����� */
		strcpy(cif_basic_inf.name,sPay_agent.name);	  /* �ͻ����� */
		strcpy(cif_basic_inf.local,"1");	/* ���ر�־ */
		strcpy(cif_basic_inf.poten,"1");/* Ǳ�ڱ�־1-����2-Ǳ�� */
		strcpy(cif_basic_inf.shareholder_ind,"1");/*�Ƿ��йɶ�2-�� 1-�� */
		strcpy(cif_basic_inf.lvl,"4"); /*�ͻ�����*/
		/*--------��ʼ�����ͻ���Ϣ----------*/
		cif_no = 0;
	
		/* ���ͻ�֤�����ͣ�֤�������Ƿ��Ѿ����� */
		ret = Cif_id_code_rel_Sel( g_pub_tx.reply, &cif_id_code_rel,
				" id_type='%s' and id_no='%s' " , 
				cif_id_code_rel.id_type, cif_id_code_rel.id_no );
		if( ret==0 )
		{
			sprintf(acErrMsg,"�ÿͻ��Ѿ�ӵ�пͻ�ID![%s][%s]",sAg_peritemmap.id_type,sPay_agent.id_no);
			WRITEMSG
			cif_no = cif_id_code_rel.cif_no; /*���Ѵ��ڵĿͻ��Ÿ�ֵ*/
			goto OK_cif_no;	/*ֱ�ӿ���*/
		}
		else if( ret!=100 )
		{
			sprintf(acErrMsg,"ȡ�ͻ�֤����Ϣ�쳣![%s]",g_pub_tx.reply);
			WRITEMSG
			sprintf( cReturn, "%s", g_pub_tx.reply );
			iErr_flag = 1;
			goto TrLog;
		}

		ret = pub_cif_CrePer_cif_no( &cif_no );
		if ( ret )
		{
			sprintf(acErrMsg,"���ɿͻ�idʧ��!");
			WRITEMSG
			sprintf( cReturn, "%s", g_pub_tx.reply );
			iErr_flag = 1;
			goto TrLog;
		}
		cif_basic_inf.cif_no = cif_no ;
		cif_id_code_rel.cif_no = cif_no ;	
	
		/* �Ǽǿͻ�֤����ͻ��Ŷ��ձ� */
		ret = Cif_id_code_rel_Ins( cif_id_code_rel );
		if( ret )
		{
			sprintf(acErrMsg,"�Ǽǿͻ�֤����ͻ��Ŷ��ձ����![%d]",ret);
			WRITEMSG
			sprintf( cReturn, "%s", g_pub_tx.reply );
			iErr_flag = 1;
			goto TrLog;
		}
				/* �Ǽǿͻ�������Ϣ�� */
		cif_basic_inf.crt_date = g_pub_tx.tx_date ; /* �������� */
		cif_basic_inf.crt_time = g_pub_tx.tx_time ; /* ����ʱ�� */
		strcpy( cif_basic_inf.sts, "1" ) ;/* �ͻ�״̬��1����2ע�� */
		cif_basic_inf.cif_seqn = 1 ;                /* �ͻ���� */
	
		ret = Cif_basic_inf_Ins( cif_basic_inf );
		if( ret )
		{
			sprintf(acErrMsg,"�Ǽǿͻ�������Ϣ�����![%d]",ret);
			WRITEMSG
			sprintf( cReturn, "%s", g_pub_tx.reply );
			iErr_flag = 1;
			goto TrLog;
		}
		sprintf(acErrMsg,"�����ͻ���Ϣ�ɹ�,�ͻ���[%ld]",cif_no);/*���ɿͻ���*/
		WRITEMSG
OK_cif_no:
	
	/*--------------������------------------------*
		ret = pub_base_CrtAcChkb( cBeg_card_no, cCard_no, 3, 1);
		if( ret )
		{
			sprintf(acErrMsg,"���ɿ���[%s]У��λ����![%d]",cBeg_card_no,ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"F005");
			goto ErrExit;
		}
		*------------------------------------------*/
	
		strcpy(g_pub_tx.mdm_code,"0000");
		memset( g_pub_tx.beg_note_no, 0x0, sizeof(g_pub_tx.beg_note_no) );	
	
		strcpy(g_pub_tx.id_type, sAg_peritemmap.id_type);	
		strcpy(g_pub_tx.id_no, sPay_agent.id_no);
		strcpy(g_pub_tx.name, sPay_agent.name );
	
		strcpy(g_pub_tx.draw_uncon_yn,"N");
		strcpy(g_pub_tx.draw_pwd_yn,"Y");
		strcpy(g_pub_tx.qry_pwd,"888888");
		strcpy(g_pub_tx.draw_pwd,"888888");
		strcpy(g_pub_tx.draw_id_yn,"N");
		strcpy(g_pub_tx.draw_seal_yn,"N");
		
		strcpy(g_pub_tx.ac_wrk_ind,"001001001");	/*--����Ҫ��ƾ֤���Ŵ���--*/
	
		strcpy(g_pub_tx.brf,"������");
	
		ret = pub_acct_opn_cif_mdm();
		if( ret )
		{
			sprintf(acErrMsg,"�����ʴ�!!![%d][%s][%s][%s]", 
				ret, sAg_peritemmap.id_type, sPay_agent.id_no,sPay_agent.name );
			WRITEMSG
			sprintf( cReturn, "%s", g_pub_tx.reply );
			iErr_flag = 1;
			goto TrLog;
		}
		memset( xs_acno,0x0,sizeof(xs_acno) );
		strcpy(xs_acno,g_pub_tx.ac_no);	/*������ʾ�˺�*/
		vtcp_log("��ʾ�˺�Ϊ[%s]",g_pub_tx.ac_no);	
	
	/*---------------���˺�---------------------*/
	memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
		memset(&g_dd_parm,0x00,sizeof(struct dd_parm_c));
		memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	
		strcpy(g_pub_tx.ac_no,xs_acno);
		strcpy(g_pub_tx.prdt_code,"101");
		strcpy( g_pub_tx.mdm_code, "0000" );
		strcpy(g_pub_tx.cur_no,"01");
		strcpy(g_pub_tx.ac_type,"D");
		strcpy(g_pub_tx.intst_type,"");
		g_pub_tx.tx_amt1 = sPay_agent.tram;		/*������*/
		strcpy(g_pub_tx.name, sPay_agent.name );
		strcpy(g_pub_tx.ct_ind,"2");
		strcpy(g_pub_tx.add_ind,"1");
		strcpy(g_pub_tx.hst_ind,"1");	/*�ռ�����ϸ*/
		strcpy(g_pub_tx.brf,"������������");
		
		memset( g_pub_tx.sub_tx_code, 0x0, sizeof(g_pub_tx.sub_tx_code) );
		strcpy( g_pub_tx.sub_tx_code, "D002" );
		set_zd_data( DC_SUB_TX_CODE, g_pub_tx.sub_tx_code );
		set_zd_data( "1033", "01" );
		set_zd_data( "103E", "2" );
		set_zd_double( "1037", g_pub_tx.tx_amt1 );

		strcpy(g_pub_tx.ac_wrk_ind,"901");	/*����Ҫ��050907*/

		ret =  pub_acct_opn_ac();
		if (ret != 0)
		{
			sprintf(acErrMsg,"��������[%d]!",ret);
			WRITEMSG
			sprintf( cReturn, "%s", g_pub_tx.reply );
			iErr_flag = 1;
			goto TrLog;
		}
		
		ret = pubf_acct_proc( g_pub_tx.sub_tx_code );
		if ( ret )
		{
			sprintf(acErrMsg,"�ǻ���˳���[%d]!",ret);
			WRITEMSG
			sprintf( cReturn, "%s", g_pub_tx.reply );
			iErr_flag = 1;
			goto TrLog;
		}
	
		memset( jz_acno, 0x0, sizeof(jz_acno) );
		jz_ac_seq = 0;
		sprintf(jz_acno,"%d",g_pub_tx.ac_id);
		jz_ac_seq = g_pub_tx.ac_seqn;
	
		sprintf(acErrMsg,"���ɵļ����˺ź��˺���ŷֱ�Ϊ[%s] [%d]",jz_acno,g_pub_tx.ac_seqn);
		WRITEMSG
	
	
NOKAIHU:

	
		g_pub_tx.ac_id = 0;
		g_pub_tx.ac_seqn = 0;
		g_pub_tx.tx_amt1 = 0.00;        /*������*/
	
		strcpy( g_pub_tx.ac_id_type,"0" ); /*�˻�������Ϊ����*/
	
		/*--- ά�����ո������뵥λ���ձ�ag_peritemmap ---*/
		if ( iProc_flag == 1 )		/*--- ���Ӷ�Ӧ��ϵ ---*/
		{
			memset( &sAg_peritemmap, 0x00, sizeof(sAg_peritemmap) );
		
			strcpy( sAg_peritemmap.bat_flag, "1" );
			strcpy( sAg_peritemmap.remark, sAg_unitinfo.pay_type );
		
			strcpy( sAg_peritemmap.unit_no, sPay_agent.unit_no );
			strcpy( sAg_peritemmap.item_no, sPay_agent.item_no );
			strcpy( sAg_peritemmap.name, sPay_agent.name );
			strcpy( sAg_peritemmap.id_type, sAg_peritemmap.id_type );
			strcpy( sAg_peritemmap.id_no, sPay_agent.id_no );
			strcpy( sAg_peritemmap.ac_no, xs_acno );
			strcpy( sPay_agent.ac_no, xs_acno );
			sAg_peritemmap.bgn_use_date = g_pub_tx.tx_date;
			strcpy( sAg_peritemmap.stat, "0" );
			sAg_peritemmap.opn_date = g_pub_tx.tx_date;
			strcpy( sAg_peritemmap.opn_tel, g_pub_tx.tel );
			sAg_peritemmap.cls_date = 99999999;
			strcpy( sAg_peritemmap.cls_tel, "0000" );
	
			ret = Ag_peritemmap_Ins( sAg_peritemmap, g_pub_tx.reply );
			if ( ret )
			{
				sprintf(acErrMsg,"д��¼����պ����뵥λ��Ӧ��ʧ��");
				WRITEMSG
				strcpy( cReturn, "д���պ����뵥λ��Ӧ��ʧ��");
				iErr_flag = 1;
				goto TrLog;
			}
		}
		if ( iProc_flag == 2 )		/*--- �޸Ķ�Ӧ��ϵ ---*/
		{
			strcpy( sPay_agent.ac_no, xs_acno );

			ret = sql_execute( "update ag_peritemmap set name = '%s', \
				id_no = '%s' , ac_no = '%s', opn_date = %ld, opn_tel= '%s' \
				where unit_no = '%s' and item_no = '%s'", 
				sPay_agent.name, sPay_agent.id_no, xs_acno, 
				g_pub_tx.tx_date,g_pub_tx.tel, 
				sPay_agent.unit_no, sPay_agent.item_no );
			if ( ret )
			{
				sprintf(acErrMsg,"�޸�ag_peritemmapʧ��[%s][%d]!!!",sPay_agent.item_no,ret );
				WRITEMSG
				strcpy( cReturn, "ά�����պ����뵥λ��Ӧ��ʧ��");
				iErr_flag = 1;
				goto TrLog;
			}
		}

		/*---------------����������д��־---------------*/
		memset( &sAg_paytlcs, 0x00, sizeof(sAg_paytlcs) );
	
		strcpy( sAg_paytlcs.unit_no, sAg_unitinfo.unit_no );
		sprintf( sAg_paytlcs.bank_tlcs, "%ld", g_pub_tx.trace_no );
		strcpy( sAg_paytlcs.batch_no, "" );		/*��������*/
		strcpy( sAg_paytlcs.unit_tlcs, "" );	/*������ˮ*/
		strcpy( sAg_paytlcs.pre_no, "" );		/*�����˺�*/
		strcpy( sAg_paytlcs.bank_acct_no, sAg_peritemmap.ac_no ); /*�����˺� */
		sprintf( sAg_paytlcs.trn_date, "%ld", g_pub_tx.tx_date );
		sprintf( sAg_paytlcs.trn_time, "%ld", g_pub_tx.tx_time );
		strcpy( sAg_paytlcs.br_no, g_pub_tx.tx_br_no );
		strcpy( sAg_paytlcs.tel_no, g_pub_tx.tel );
		sAg_paytlcs.trn_amt = sPay_agent.tram;
		strcpy( sAg_paytlcs.item_no, sPay_agent.item_no );
		strncpy( sAg_paytlcs.pay_month, sAg_paytlcs.trn_date, 6 );
		strcpy( sAg_paytlcs.pay_way, "1" );		/*- �������� -*/
		strcpy( sAg_paytlcs.bill_flag, "0" );
		strcpy( sAg_paytlcs.stat, "0" );
	
		ret = Ag_paytlcs_Ins( sAg_paytlcs, g_pub_tx.reply );
		if (ret != 0)
		{
			if ( sAg_unitinfo.opr_attri[0] == '0' )
			{
				/*sql_rollback();*/
			}
			
			strcpy( cReturn, "д�ո�����ʷ�����");
			sprintf(acErrMsg,"����ag_paytlcs����[%d]",ret);
			WRITEMSG
			iErr_flag = 1;
			goto TrLog;
		}
	
		ret = sql_execute( "update pay_agent set sata = '0', \
			result = '0', result_name = '%s' \
			where unit_no = '%s' and item_no = '%s'" ,
			cReturn, sPay_agent.unit_no,sPay_agent.item_no );
		if (ret != 0)
		{
			if ( sAg_unitinfo.opr_attri[0] == '0' )
			{
				/*sql_rollback();*/
			}
			
			strcpy( cReturn, "����δ�����¼״̬��" );
			sprintf(acErrMsg,"����pay_agent����[%d][%s][%s]", 
				sPay_agent.unit_no, sPay_agent.item_no, ret);
			WRITEMSG
			iErr_flag = 1;
			goto TrLog;
		}
		
		/**add by kevinchan 20120326 start**/
		/**�����ļ��д���״̬**/
		if (iErr_flag == 0 )
		{
			strcpy( cReturn, "�ɹ�" );
		}
		/**add by kevinchan 20120326 end**/
		
TrLog:
		pub_base_strpack( sPay_agent.name );
		if ( iErr_flag == 0)
		{
			lSnum = lSnum + 1;
			dSamt = dSamt + sPay_agent.tram;
			 vtcp_log("[%s][%d]���ʺŴ���ɹ�[%s]\r\n",__FILE__,__LINE__,sAg_peritemmap.ac_no);
			 /** ��ѯ�ʺŵ�֤����Ϣ ���� add by wudawei 20130424 **/
			 ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&mdm_ac_rel_t1," ac_no='%s' ",sPay_agent.ac_no );
			fprintf( fp,"    ��%10s��%10s��%16.2lf��%20s��%20s��%8s��\r\n", sPay_agent.item_no, sPay_agent.name, 
				sPay_agent.tram, mdm_ac_rel_t1.id_no, sPay_agent.ac_no, cReturn );
				memset( &sDd_mst, 0x0, sizeof(sDd_mst) );
					ret = Dd_mst_Sel( g_pub_tx.reply, &sDd_mst, \
						"ac_id = %ld", mdm_ac_rel_t1.ac_id );					
			/* д����Ϣ add by wudawei 20140610**/
			if( sPay_agent.tram > 0)
			{
				pub_mob_sendmail_bat( sAg_unitinfo.pay_type, sAg_unitinfo.unit_no, "6641",  sPay_agent.ac_no, sAg_unitinfo.opr_attri, sPay_agent.tram,sDd_mst.bal, 0.00);
			}
			/**end wudawei 20140610**/
			if ( iK_flag == 1 )
			{
				lK_num = lK_num + 1;	/*�¿�����*/
				dK_amt = dK_amt + sPay_agent.tram;	/*�¿����*/
			}
			
			if ( sAg_unitinfo.opr_attri[0] == '0' )
			{
				/*sql_commit();*/
			}
			
		}
		else
		{
			vtcp_log("[%s][%d]���˺Ŵ���ʧ��[%s]\r\n",__FILE__,__LINE__,sAg_peritemmap.ac_no);
			memset( &sRsp_code, 0x0, sizeof(sRsp_code) );
			ret1 = Rsp_code_Sel( g_pub_tx.reply , &sRsp_code, "code = '%s'", cReturn );
			if ( ret1 == 0 )
			{
				memset( cReturn, 0x0, sizeof(cReturn) );
				pub_base_strpack( sRsp_code.memo );
				strcpy( cReturn, sRsp_code.memo );
			}
			
			iErr_flag1 = 1;
			lFnum = lFnum + 1;
			dFamt = dFamt + sPay_agent.tram;
			fprintf( fp1,"    ��%10s��%10s��%16.2lf��%20s��%50s��\r\n", sPay_agent.item_no, sPay_agent.name, 
				sPay_agent.tram, sPay_agent.id_no, cReturn );
		}
	}
	Pay_agent_Clo_Sel();	
	fprintf( fp, "    �������������㩥���������㩥���������������㩥�������������������㩥�������������������㩥��������\r\n" );
	fprintf( fp,"    ��%10s��%10d��%16.2lf��%20s��%20s��%8s��\r\n", "�ϼ�", lSnum, 
				dSamt, "", "", "" );
	fprintf( fp, "    �������������۩����������۩����������������۩��������������������۩��������������������۩���������\f" );
	
	fprintf( fp1, "          �������������㩥�������������������㩥���������������㩥�������������������㩥��������������������������������������������������\r\n" );
	fprintf( fp1,"    ��%10s��%10d��%16.2lf��%20s��%50s��\r\n", "�ϼ�", lFnum, 
				dFamt, "", "" );
	fprintf( fp1, "          �������������۩��������������������۩����������������۩��������������������۩���������������������������������������������������\f" );

	fclose(fp);
	fclose(fp1);	
	
	if ( iErr_flag1 != 0 && sAg_unitinfo.opr_attri[0] == '1' )
	{
		sprintf(acErrMsg,"ʧ��[%ld][%.2f],�¿�[%ld][%lf]", lFnum, dFamt, lK_num, dK_amt );
		WRITEMSG
		strcpy( g_pub_tx.reply, "H034" );
		goto ErrExit;
	}
	else if ( lSnum > 0 )
	{
		sprintf(acErrMsg,"-------------��ʼ��λ�˺Ŵ�ȡ����-----------");
		WRITEMSG
		
		pub_base_sysinit();
		
		g_pub_tx.ac_id = 0;	 /*test clean */
		g_pub_tx.ac_seqn = 0;
		strcpy(g_pub_tx.beg_note_no,"");
        	
		strcpy( g_pub_tx.ac_no, sAg_unitinfo.bank_acct_no );
		g_pub_tx.tx_amt1 = dSamt;
        	
		if ( sAg_unitinfo.bank_acct_no[0] == '9' )	/*-- �ڲ��� --*/
		{
			strcpy( g_pub_tx.ac_id_type,"9" ); /*�˻�����Ϊ�ڲ�*/
			strcpy( g_pub_tx.ac_get_ind,"00" ); /*������δ��ȡ�ֻ�*/
			g_pub_tx.svc_ind=9001;
			
			if ( sAg_unitinfo.opr_attri[0] == '1' )
			{
				strcpy(g_pub_tx.add_ind,"0");	/*-- ���� --*/
				
				memset( g_pub_tx.sub_tx_code, 0x0, sizeof(g_pub_tx.sub_tx_code) );
				strcpy( g_pub_tx.sub_tx_code, "A016" );
				set_zd_data( DC_SUB_TX_CODE, g_pub_tx.sub_tx_code );
			
				set_zd_data( "1204", "01" );
				set_zd_data( "1205", "2" );
				set_zd_double( "1208", dSamt );
				
				strcpy(g_pub_tx.brf, "����");
			}
			else
			{
				strcpy(g_pub_tx.add_ind,"1");	/*-- ���� --*/
				
				memset( g_pub_tx.sub_tx_code, 0x0, sizeof(g_pub_tx.sub_tx_code) );
				strcpy( g_pub_tx.sub_tx_code, "A017" );
				set_zd_data( DC_SUB_TX_CODE, g_pub_tx.sub_tx_code );
			
				set_zd_data( "1214", "01" );
				set_zd_data( "1215", "2" );
				set_zd_double( "1218", dSamt );
				
				strcpy(g_pub_tx.brf, "����");
			}
		}
		else
		{
			strcpy( g_pub_tx.ac_id_type,"1" ); /*�˻�����Ϊ����*/
			g_pub_tx.svc_ind=1001;

			if ( sAg_unitinfo.opr_attri[0] == '1' )
			{
				strcpy( g_pub_tx.add_ind, "0" );	/*-- ���� --*/				
				
				memset( g_pub_tx.sub_tx_code, 0x0, sizeof(g_pub_tx.sub_tx_code) );
				strcpy( g_pub_tx.sub_tx_code, "D003" );
				set_zd_data( DC_SUB_TX_CODE, g_pub_tx.sub_tx_code );
				
				strcpy( g_pub_tx.prt_ind, "0" );
				set_zd_data( "102O", "0" );
			
				set_zd_data( "102J", "01" );
				set_zd_data( "102K", "2" );
				set_zd_double( "102F", dSamt );
				set_zd_double( "102P", 0.00 );
				set_zd_double( "102Q", 0.00 );					
				
				strcpy(g_pub_tx.brf, "����");
				
				sprintf( acErrMsg, "----------------->��������2[%s]", sAg_unitinfo.opr_attri );
				WRITEMSG
			}
			else
			{
				strcpy(g_pub_tx.add_ind,"1");	/*-- ���� --*/
				
				memset( g_pub_tx.sub_tx_code, 0x0, sizeof(g_pub_tx.sub_tx_code) );
				strcpy( g_pub_tx.sub_tx_code, "D099" );
				set_zd_data( DC_SUB_TX_CODE, g_pub_tx.sub_tx_code );
				
				strcpy( g_pub_tx.prt_ind, "0" );
				set_zd_data( "1017", "0" );
        	
				set_zd_data( "101A", "01" );
				set_zd_data( "101B", "2" );
				set_zd_double( "1013", dSamt );
				
				strcpy(g_pub_tx.brf, "����");
				
				sprintf( acErrMsg, "----------------->��������3[%s]", sAg_unitinfo.opr_attri );
				WRITEMSG
			}
		}
		
		strcpy(g_pub_tx.ct_ind,"2");
		strcpy(g_pub_tx.ac_wrk_ind,"001001001");
		strcpy(g_pub_tx.hst_ind,"1");
		
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
			sprintf(acErrMsg,"�ǵ�λ����˳���[%d]!",ret);
			WRITEMSG
			goto ErrExit;
		}
		/*
		memset( filename3, 0x0, sizeof(filename3) );
		sprintf( filename3, "%s/DWHZ%s%ld%s.txt", getenv("FILDIR"), sAg_unitinfo.unit_no, g_pub_tx.tx_date,g_pub_tx.tel);*/
		
		fp3 = fopen( filename, "a" ); /*------��ȷ����������-------*/
		if( fp3==NULL )
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
		/**modify by kevinchan 20120320 start**/
		/**�����ļ���ͷ������ ͳһΪ��3��**/
		/*fprintf( fp3, "				[%s]��������λ��Ʊ	\r\n", sAg_unitinfo.unit_name );*/
		fprintf( fp3, "\r\n\r\n\r\n\r\n\r\n\r\n                 				 [%s]��������λ��Ʊ	\r\n", sAg_unitinfo.unit_name );
		/**modify by kevinchan 20120320 start**/
		fprintf( fp3, "                  ������: %s    			�������� : %s\r\n", g_pub_tx.tx_br_no, sCom_branch.br_name );
		fprintf( fp3, "                  ��λ����: %s   			��λ����: %s  \r\n", sAg_unitinfo.unit_no, sAg_unitinfo.unit_name );
		fprintf( fp3, "                  ���� : %ld 			��λ�˺ţ�%s \r\n", g_pub_tx.tx_date, sAg_unitinfo.bank_acct_no );
		fprintf( fp3, "                  ��ˮ�� : %s   			��Ա��: %s  \r\n", cTrace_no,g_pub_tx.tel );
		
		fprintf( fp3, "                  �����������Щ����������������������������������������Щ��������Щ�����������������\r\n" );
		fprintf( fp3, "                  ����λ��ũ�           ��   λ   ��   ��            ��  ����  ��    ��   ��     ��\r\n" );
		fprintf( fp3, "                  �����������੤���������������������������������������੤�������੤����������������\r\n" );
		fprintf( fp3, "                  ��%8s��%40s��%8ld��%16.2lf��\r\n", sAg_unitinfo.unit_no, sAg_unitinfo.unit_name, lSsum1, dSamt );
		fprintf( fp3, "                  �����������੤���������������������������������������੤�������੤����������������\r\n" );
		fprintf( fp3, "                  �� ��  �� ��                                        ��%8ld��%16.2lf�� \r\n", lSsum1, dSamt );
		/**modify by kevinchan 20120320 start**/
		/**�����ļ���β������ ͳһΪ��1��**/
		/*fprintf( fp3, "�����������ة����������������������������������������ة��������ة����������������� \r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n" );*/
	  fprintf( fp3, "                  �����������ة����������������������������������������ة��������ة����������������� \f" );
		/**modify by kevinchan 20120320 start**/
		
		/**modify by kevinchan 20120320 start**/
		/**�����ļ���ͷ������ ͳһΪ��3��**/	
		/*fprintf( fp3, "				[%s]����������˻��ܴ�Ʊ	\r\n", sAg_unitinfo.unit_name );*/
		fprintf( fp3, "\r\n\r\n\r\n\r\n\r\n\r\n      	            			[%s]����������˻��ܴ�Ʊ	\r\n", sAg_unitinfo.unit_name );
		/**modify by kevinchan 20120320 start**/
		fprintf( fp3, "                  ������: %s    			�������� : %s\r\n", g_pub_tx.tx_br_no, sCom_branch.br_name );
		fprintf( fp3, "                  ���� : %ld 			��ˮ�� ��%s \r\n", g_pub_tx.tx_date, cTrace_no);
		fprintf( fp3, "                  ��Ա��: %s           \r\n", g_pub_tx.tel);
		
		fprintf( fp3, "                  �����������Щ����������������������������������������Щ��������Щ�����������������\r\n" );
		fprintf( fp3, "                  ����λ��ũ�           ��   λ   ��   ��            ��  ����  ��    ��   ��     ��\r\n" );
		fprintf( fp3, "                  �����������੤���������������������������������������੤�������੤����������������\r\n" );
		fprintf( fp3, "                  ��%8s��%40s��%8ld��%16.2lf��\r\n", sAg_unitinfo.unit_no, sAg_unitinfo.unit_name, lSsum1, dSamt );
		fprintf( fp3, "                  �����������੤���������������������������������������੤�������੤����������������\r\n" );
		fprintf( fp3, "                  �� ��  �� ��                                        ��%8ld��%16.2lf�� \r\n", lSsum1, dSamt );
		fprintf( fp3, "                  �����������ة����������������������������������������ة��������ة����������������� \f" );
		
		fclose( fp3 );
		
		memset( filename4, 0x0, sizeof(filename4) );
		sprintf( filename4,"%s/DLKFMX%s%ld%s.txt",getenv("FILDIR"),cUnit_no,g_pub_tx.tx_date,g_pub_tx.tel);
			
		sprintf( execstr, "cp %s %s", filename, filename4 );
		ret = system( execstr );
		
		memset( filename2, 0x0, sizeof(filename2) );
		pub_base_AllDwnFilName( filename2 );      /*  �´��ļ���  */
		
		memset( execstr, 0x0, sizeof(execstr) );
		sprintf( execstr, "cp %s %s", filename, filename2 );
		
		ret = system( execstr );
		if ( ret )
		{
			sprintf(acErrMsg,"�����´��ļ���ʧ��[%s][%s][%d][%s]", filename2, strerror(errno), ret, execstr );
			WRITEMSG
			strcpy( g_pub_tx.reply, "S047" );
			goto ErrExit;		
		}
		
		sprintf( acErrMsg, "�¿�[%ld][%lf]", lK_num, dK_amt );
	}
	
	if ( iErr_flag1 != 0 )
	{
		sprintf(acErrMsg,"��ע��,��ʧ�ܼ�¼[%ld][%.2f],�¿�[%ld][%lf]", lFnum, dFamt, lK_num, dK_amt );
    WRITEMSG
	}
	if(lFnum==0&&lSnum==0)
	{
		flag[0]='0';
		set_zd_data("0670",flag);
	}
	set_zd_data( "0130", acErrMsg );	
        
	set_zd_data(DC_FILE_SND,"1");

OkExit:
    strcpy( g_pub_tx.reply, "0000" );
    sprintf( acErrMsg, "������������ҵ������ϣ�" );
    WRITEMSG
    set_zd_data( DC_REPLY, g_pub_tx.reply );
    return 0;
    
ErrExit:
	fclose(fp);
	fclose(fp1);
	Pay_agent_Clo_Sel();
    sprintf(acErrMsg,"������������ҵ����ʧ�ܣ�[%s]", g_pub_tx.reply );
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
