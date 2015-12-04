/*************************************************
* �� �� ��: gD008.c
* ��������������ҵ��������������
*
* ��    ��: jack
* �������: 2005��07��19��
*
* �޸�����: 20140318
* �� �� ��: jk
* �޸�����: ��������ȼ����Ŀ
*
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
#include "dd_mst_c.h"
#define PAGE_CNT 50

gD008()
{
	int ret = 0,tmp_trace_cnt=0;
	int ret1 = 0;
	char cReturn[51];
	char execstr[248];
	char str_temp[2048];
	int iProc_flag = 0;	/*	1�����Ӷ�Ӧ��ϵ;2���޸Ķ�Ӧ��ϵ		*/
	int iErr_flag = 0;	/*	0��������ȷ��1������ʧ��	*/
	
	long cif_no = 0;
	char xs_acno[20];
	char jz_acno[20];
	int jz_ac_seq = 0;
	
	long lSnum = 0;		/*�ɹ�����*/
	long lFnum = 0;		/*ʧ�ܱ���*/
	double dSamt = 0.00;	/*�ɹ����*/
	double dFamt = 0.00;	/*ʧ�ܽ��*/
	
	int iK_flag = 0;	/*�¿�����־*/
	long lK_num = 0;	/*�¿�����*/
	double dK_amt = 0.00;	/*�¿����*/
	
	double ys_amt = 0.00;/* ȼ������ Ӧ�ս��*/

	char filename0[248];	/*����ȼ����˾��Ŀ*/
	char filename[248];
	char filename1[248];
	char filename3[248];
	FILE *fp0 = NULL;
	FILE *fp = NULL;
	FILE *fp1 = NULL;
	FILE *fp3 = NULL;
	
	char filename00[248];	/*����ȼ����˾����*/
	char filename01[248];
	char filename11[248];
	char filename31[248];
	FILE *fp00 = NULL;
	FILE *fp01 = NULL;
	FILE *fp11 = NULL;
	FILE *fp31 = NULL;
	double sumtram=0.0;
	int i=0,tot_cnt=0,err_tot_cnt=0;
		int lSsum1=0;
	long lTrace_no = 0;
	
	char tmpstr[512];

	struct pay_agent_c sPay_agent;
	struct ag_peritemmap_c sAg_peritemmap;
	struct ag_unitinfo_c sAg_unitinfo;
	struct ag_paytlcs_c sAg_paytlcs;
	struct dd_mst_c sDd_mst;
	struct in_mst_c sIn_mst;
	struct mdm_ac_rel_c sMdm_ac_rel;
	struct cif_basic_inf_c cif_basic_inf;
	struct cif_id_code_rel_c cif_id_code_rel;
	struct com_branch_c sCom_branch;
	struct rsp_code_c sRsp_code;

	/**------- ��ʼ��ȫ�ֱ��� --------**/
	pub_base_sysinit();
	set_zd_data( DC_SUB_TX_CODE, g_pub_tx.sub_tx_code );

	sql_begin();
	
	/*---����Ҫ���죬����Ҫ�ڴ���������---*/
	/*
	ret = Ag_unitinfo_Dec_Sel_hold( g_pub_tx.reply , "unit_stat = '0' \
	*/
	ret = Ag_unitinfo_Dec_Sel( g_pub_tx.reply , "unit_stat = '0' \
		and (( opr_attri = '1' and beg_date = %ld) or \
		( opr_attri = '0' and beg_date <= %ld and end_date >= %ld ))", \
		g_pub_tx.tx_date, g_pub_tx.tx_date, g_pub_tx.tx_date );
	if ( ret )
	{
		sprintf(acErrMsg,"ִ��Ag_unitinfo_Dec_Sel����[%d]", ret );
		WRITEMSG
		set_zd_data(DC_REPLY,g_pub_tx.reply);
    		return 1;
	}
		
	while(1)
	{
		memset( &sAg_unitinfo, 0x0, sizeof(sAg_unitinfo) );
		lSnum = 0;	/*�ɹ�����*/
		lFnum = 0;	/*ʧ�ܱ���*/
		dSamt = 0.00;	/*�ɹ����*/
		dFamt = 0.00;	/*ʧ�ܽ��*/
    sumtram=0.00;/**/
		ret = Ag_unitinfo_Fet_Sel( &sAg_unitinfo, g_pub_tx.reply );
		if ( ret == 100 )
		{
			/*
			sql_begin();
			*/
			break;
		}
		vtcp_log("aaaaaaaaaaaaa[%s]\n",sAg_unitinfo.bank_acct_no);
		if ( ret )
		{
			sprintf(acErrMsg,"ִ��Ag_unitinfo_Fet_Sel����[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}
		
		pub_base_strpack( sAg_unitinfo.pay_type );
	
		if ( strcmp( sAg_unitinfo.pay_type, "35" ) == 0 )
		{
			continue;	/*------�������ֽ������------*/
		}
		
		/*--- һ����λ��һ������ ---*/
		/*
		sql_begin();
		*/
		
		pub_base_strpack( sAg_unitinfo.unit_no );
		pub_base_strpack( sAg_unitinfo.unit_name );
		pub_base_strpack( sAg_unitinfo.opn_br_no );
		pub_base_strpack( sAg_unitinfo.bank_acct_no );	
		
		/*--------------  ��鵥λ�˺�״̬  ---------------*/
		memset( &sMdm_ac_rel, 0x00, sizeof(sMdm_ac_rel) );
		ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdm_ac_rel, \
			"ac_no = '%s'", sAg_unitinfo.bank_acct_no );
		if ( ret )
		{
			sprintf(acErrMsg,"��ѯ�����˺Ŷ��ձ�ʧ��");
			WRITEMSG
			set_zd_data(DC_REPLY,g_pub_tx.reply);
			/*sql_rollback();*/
			continue;
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
					continue;
	  		}
	  		if(sMdm_ac_rel.note_sts[0]!='0' || sIn_mst.sts[0]!='1')
	  		{
	  			sprintf(acErrMsg,"�˻�״̬������[%s]", sAg_unitinfo.bank_acct_no );
					WRITEMSG
					strcpy( g_pub_tx.reply, "H059" );
					set_zd_data(DC_REPLY,g_pub_tx.reply);
					continue;
	  		}
	  		if(sAg_unitinfo.opr_attri[0]=='1')
	  		{
	  		ret=sql_sum_double("pay_agent","tram",&sumtram,"tx_date=%ld and unit_no='%s' and sata='1' and result='1'",g_pub_tx.tx_date,sAg_unitinfo.unit_no);
	 			if(sIn_mst.bal<sumtram)
	 			{
	 				sprintf(acErrMsg,"�˻�������[%s]", sAg_unitinfo.bank_acct_no );
					WRITEMSG
					strcpy( g_pub_tx.reply, "H059" );
					set_zd_data(DC_REPLY,g_pub_tx.reply);
					continue;
	 			}
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
					/*sql_rollback();*/
					continue;
				}
				
				if ( sMdm_ac_rel.note_sts[0] != '0' || sDd_mst.ac_sts[0] != '1' \
					|| sDd_mst.hold_sts[0] == '1' || sDd_mst.hold_sts[0]=='2' )	/*������ȫ��*/
				{
					sprintf(acErrMsg,"�˻�״̬������[%s]", sAg_unitinfo.bank_acct_no );
					WRITEMSG
					/*sql_rollback();*/
					continue;	/*������ʼ��һ����λ*/
				}
				if(sAg_unitinfo.opr_attri[0]=='1')
	  		{	
				ret=sql_sum_double("pay_agent","tram",&sumtram,"tx_date=%ld and unit_no='%s' and sata='1' and result='1'",g_pub_tx.tx_date,sAg_unitinfo.unit_no);
	 			if(sDd_mst.bal-sDd_mst.hold_amt<sumtram)
	 			{
	 				sprintf(acErrMsg,"�˻�������[%s]", sAg_unitinfo.bank_acct_no);
					WRITEMSG
					strcpy( g_pub_tx.reply, "H059" );
					set_zd_data(DC_REPLY,g_pub_tx.reply);
					continue;
	 			}
	 		 }			
		 }
		/*------------ �������ļ� -----------*/
		/*����ȼ�����ɴ��۳ɹ��ļ�*/
		if(sAg_unitinfo.opr_attri[0]=='0' && memcmp(sAg_unitinfo.pay_type,"44",2)==0)
	    {	
				memset(filename0,0x0,sizeof(filename0));
				sprintf(filename0,"%s/txt/y01%ld.txt", getenv("HOME"), g_pub_tx.tx_date+1 );
				
				fp0 = fopen( filename0,"w" );	
				if( fp0==NULL )
				{
					sprintf(acErrMsg," open file [%s] error ",filename0 );
					WRITEMSG
					strcpy( g_pub_tx.reply, "S047" );
					set_zd_data(DC_REPLY,g_pub_tx.reply);
		   			return 1;
				}
		
		
	  		/*����ˮ���ɴ��۳ɹ��ļ�*/	
		}else if (sAg_unitinfo.opr_attri[0]=='0' && memcmp(sAg_unitinfo.pay_type,"43",2)==0)
	    {	
	    	/*
				memset(filename0,0x0,sizeof(filename0));
				sprintf(filename0,"%s/txt/CXZ%ld", getenv("HOME"), g_pub_tx.tx_date );
				
				fp0 = fopen( filename0,"w" );	
				if( fp0==NULL )
				{
					sprintf(acErrMsg," open file [%s] error ",filename0 );
					WRITEMSG
					strcpy( g_pub_tx.reply, "S047" );
					set_zd_data(DC_REPLY,g_pub_tx.reply);
		   			return 1;
				}
		
				 int sum_count=sql_count("pay_agent","tx_date=%ld and unit_no='%s' and sata='1' and result='1'",g_pub_tx.tx_date,sAg_unitinfo.unit_no);
				 double sum_amt = 0.00;
				 sql_sum_double("pay_agent","tram",&sum_amt,"tx_date=%ld and unit_no='%s' and sata='1' and result='1'",g_pub_tx.tx_date,sAg_unitinfo.unit_no);

				 sprintf(acErrMsg,"sum_amt=[%.2lf]  sum_count=[%d]  ",sum_amt, sum_count);
				 WRITEMSG

				 fprintf( fp0,"%d\r\n%010.0lf\r\n",sum_count,sum_amt*100  );	  
				 */
	  			
		}
		/*���ļ����*/
		
		memset(filename,0x0,sizeof(filename));
		sprintf(filename,"%s/report/%s/BR0%s.txt", getenv("HOME"), sAg_unitinfo.opn_br_no, sAg_unitinfo.unit_no );
		
		fp = fopen( filename,"w" );	/*���ļ�������¼��ϸ������ȷ���*/
		if( fp==NULL )
		{
			sprintf(acErrMsg," open file [%s] error ",filename );
			WRITEMSG
			strcpy( g_pub_tx.reply, "S047" );
			set_zd_data(DC_REPLY,g_pub_tx.reply);
   			return 1;
		}
		/*******add by ligl 2006-11-23 17:11********/
		if(sAg_unitinfo.opr_attri[0]=='0')
	  {	
	  			fprintf( fp, "                                 [%s][%s]��������ҵ��ɹ���¼\n\n", sAg_unitinfo.unit_no, sAg_unitinfo.unit_name );
					fprintf( fp, "�����������������������ө��������������������ө��������������������ө����������������ө����������������ө���������\n" );
    			fprintf( fp, "��%20s��%20s��%20s��%16s��%16s��%8s��\n", "������","�ͻ��ʺ�", "�ͻ�����", "���", "���", "������" );
    			fprintf( fp, "�����������������������㩥�������������������㩥�������������������㩥���������������㩥���������������㩥��������\n" );
	  	
		}
	  else
	  {	
			fprintf( fp, "                                 [%s][%s]��������ҵ��ɹ���¼\n\n", sAg_unitinfo.unit_no, sAg_unitinfo.unit_name );
			fprintf( fp, "�����������������������ө��������������������ө����������������ө��������������������ө��������������������ө���������\n" );
    	fprintf( fp, "��%20s��%20s��%16s��%20s��%20s��%8s��\n", "������", "�ͻ�����", "���", "֤������", "�ͻ��˺�", "������" );
    	fprintf( fp, "�����������������������㩥�������������������㩥���������������㩥�������������������㩥�������������������㩥��������\n" );
    }    	
		memset( filename1, 0x0, sizeof(filename1) );
		sprintf( filename1, "%s/report/%s/BR1%s.txt", getenv("HOME"), sAg_unitinfo.opn_br_no, sAg_unitinfo.unit_no );
		
		fp1 = fopen( filename1, "w" );/*���ļ�������¼��ϸ��������*/
		if( fp1==NULL )
		{
			sprintf(acErrMsg," open file [%s] error ",filename1 );
			WRITEMSG
			strcpy( g_pub_tx.reply,"S047" );
			fclose( fp );
			set_zd_data(DC_REPLY,g_pub_tx.reply);
   			return 1;
		}
		/*******add by ligl 2006-11-23 17:11********/
 		if(sAg_unitinfo.opr_attri[0]=='0')
	  {	
			fprintf( fp1, "                                [%s][%s]��������ҵ��ʧ�ܼ�¼\n\n", sAg_unitinfo.unit_no, sAg_unitinfo.unit_name );
			fprintf( fp1, "�����������������������ө��������������������ө��������������������ө����������������ө����������������ө���������\n" );
    	fprintf( fp1, "��%20s��%20s��%20s��%16s��%16s��%8s��\n", "������","�ͻ��ʺ�", "�ͻ�����", "��    ��", "��    ��", "������" );
    	fprintf( fp1, "�����������������������㩥�������������������㩥�������������������㩥���������������㩥���������������㩥��������\n" );
		}
	  else
	  {       	
			fprintf( fp1, "                                [%s][%s]��������ҵ��ʧ�ܼ�¼\n\n", sAg_unitinfo.unit_no, sAg_unitinfo.unit_name );
			fprintf( fp1, "�����������������������ө��������������������ө����������������ө��������������������ө��������������������ө�������������������������������\n" );
    	fprintf( fp1, "��%20s��%20s��%16s��%20s��%20s��%30s��\n", "������", "�ͻ�����",   "��  ��", "֤������", "�ͻ��ʺ�", "������" );
    	fprintf( fp1, "�����������������������㩥�������������������㩥���������������㩥�������������������㩥�������������������㩥������������������������������\n" );
    }    	
        	lTrace_no = 0;
        	pub_base_PubQlsh_Trans( &lTrace_no );
    if(sAg_unitinfo.opr_attri[0]=='1')
    {
		ret = Pay_agent_Dec_Sel( g_pub_tx.reply , "unit_no = '%s' \
			and sata = '1' and result = '1' and tx_date=%ld order by to_number(item_no)", sAg_unitinfo.unit_no,g_pub_tx.tx_date );
		}
		else
		{
		ret = Pay_agent_Dec_Sel( g_pub_tx.reply , "unit_no = '%s' \
			and sata = '1' and result = '1' and tx_date=%ld", sAg_unitinfo.unit_no,g_pub_tx.tx_date );	
		}
		if ( ret )
		{
			sprintf(acErrMsg,"ִ��Pay_agent_Dec_Sel����[%d]", ret );
			WRITEMSG
			fclose( fp );
			fclose( fp1 );
			set_zd_data(DC_REPLY,g_pub_tx.reply);
   			return 1;
		}		

		tot_cnt=0; err_tot_cnt=0;
		while(1)
		{
			iErr_flag = 0;
			iProc_flag = 0;			
			
			memset( &sPay_agent, 0x0, sizeof(sPay_agent) );
			ret = Pay_agent_Fet_Sel( &sPay_agent , g_pub_tx.reply );
			if ( ret == 100 )
			{
				if ( sAg_unitinfo.opr_attri[0] == '0' )	/*����ҵ����*/
				{
					/*
					sql_begin();
					*/
				}
				/*if ( sAg_unitinfo.opr_attri[0] == '1' && lFnum == 0 )	����ҵ����ȫ���ɹ�*/
				
				if ( sAg_unitinfo.opr_attri[0] == '1')	/*����ҵ����ȫ���ɹ�*/
				{
					ret = sql_execute( "insert into pay_agent_bk select * from \
						pay_agent where unit_no = '%s' and tx_date = %ld", \
						sAg_unitinfo.unit_no, g_pub_tx.tx_date );
					lSsum1= sql_count("(select distinct unit_no,item_no,ac_no from  pay_agent)"," unit_no='%s'",sAg_unitinfo.unit_no);		
					vtcp_log("[%s][%d]====����====[%ld]\n",__FILE__,__LINE__,lSsum1);
					ret = sql_execute( "delete from pay_agent where unit_no = '%s' \
						and tx_date = %ld and result='0'", sAg_unitinfo.unit_no, g_pub_tx.tx_date );
				}
				if ( sAg_unitinfo.opr_attri[0] == '0' && sAg_unitinfo.end_date == g_pub_tx.tx_date)	/*����ҵ��*/
				{
					ret = sql_execute( "insert into pay_agent_bk select * from \
						pay_agent where unit_no = '%s'", \
						sAg_unitinfo.unit_no );
					lSsum1= sql_count("(select distinct unit_no,item_no,ac_no from  pay_agent)"," unit_no='%s'",sAg_unitinfo.unit_no);		
					vtcp_log("[%s][%d]====����====[%ld]\n",__FILE__,__LINE__,lSsum1);
	
					ret = sql_execute( "delete from pay_agent where unit_no = '%s' and result='0'", \
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

			/**----------- ÿ��ѭ�������³�ʼ��ȫ�ֱ��� -------------**/
			memset( &g_pub_tx, 0x00, sizeof(g_pub_tx) );
			pub_base_sysinit();	
			
			g_pub_tx.trace_no = lTrace_no;	/*���¸�ֵΪ���ɵ���ˮ��*/
			set_zd_long( "0040", lTrace_no );
      /*g_pub_tx.trace_cnt=atoi(sPay_agent.cnt); */
			tmp_trace_cnt++;
      g_pub_tx.trace_cnt=tmp_trace_cnt;
			memset( cReturn, 0x0, sizeof(cReturn) );
			memset( xs_acno,0x0,sizeof(xs_acno) );
			
			/*---���׻���Ϊ����λ��������---*/
			strcpy( g_pub_tx.tx_br_no, sAg_unitinfo.opn_br_no );
			set_zd_data( DC_TX_BR_NO, sAg_unitinfo.opn_br_no );
			
			memset( tmpstr, 0x0, sizeof(tmpstr) );
			get_zd_data( "0030", tmpstr );
			sprintf(acErrMsg,"���������׻���Ϊ[%s][%s]", g_pub_tx.tx_br_no, tmpstr );
			WRITEMSG
  if(sAg_unitinfo.opr_attri[0] == '0')
  {
			ret = Ag_peritemmap_Sel( g_pub_tx.reply , &sAg_peritemmap,"unit_no = '%s' and ac_no = '%s'",sPay_agent.unit_no, sPay_agent.ac_no);
			if ( ret == 0 )
			{
				pub_base_strpack_all( sAg_peritemmap.name );
				pub_base_strpack_all( sPay_agent.name );
				if ( strcmp( sAg_peritemmap.name, sPay_agent.name) )
					{
							sprintf( cReturn, "��������,ԭ����Ϊ[%s]", sAg_peritemmap.name );
							sprintf( acErrMsg, "��������,ԭ����Ϊ[%s]", sAg_peritemmap.name );
							WRITEMSG
							iErr_flag = 1;
							goto TrLog;
					}
				/*----------------------------------------------------*/
					vtcp_log("aaaaaaaaaaaaa[%s]\n",sAg_unitinfo.opr_attri);
					memset( &sMdm_ac_rel, 0x0, sizeof(sMdm_ac_rel) );
					ret1 = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdm_ac_rel, \
						"ac_no = '%s' and note_sts <> '2'", sPay_agent.ac_no );
                                        /*
                                          2009/2/13, added by lianghaoran, "and note_sts <> '2'"
                                        */
					if ( ret1 )
					{
						sprintf( cReturn, "�����˺�[%s]��Ϣ��", sPay_agent.ac_no );
						sprintf( acErrMsg, "�����˺�[%s]��Ϣ��", sPay_agent.ac_no );
						WRITEMSG
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
						iErr_flag = 1;
						goto TrLog;
					}
					
					pub_base_strpack( sDd_mst.ac_sts );
					if ( sDd_mst.ac_sts[0] != '1' || sDd_mst.hold_sts[0] == '1'||sMdm_ac_rel.note_sts[0]!='0' )	/*��������ȫ����*/
					{
						strcpy( cReturn, "����,��ȫ����͹�ʧ" );
						sprintf(acErrMsg,"��������ȫ����[%s][%c][%c][%c]", sAg_peritemmap.item_no,sDd_mst.ac_sts[0] ,sDd_mst.hold_sts[0] ,sMdm_ac_rel.note_sts[0]);
						WRITEMSG
						iK_flag = 1;
						iErr_flag = 1;
						goto TrLog;
					}					
					sumtram=0.00;
					ret=sql_sum_double("pay_agent","tram",&sumtram,"tx_date=%ld and unit_no='%s' and ac_no='%s' and sata='1' and result='1'",g_pub_tx.tx_date,sAg_unitinfo.unit_no,sAg_peritemmap.ac_no);
	 				if(sDd_mst.bal<sumtram)
	 				{
							strcpy( cReturn, "������" );
	 						sprintf(acErrMsg,"�˻�������[%s]", sAg_peritemmap.ac_no);
							WRITEMSG
							strcpy( g_pub_tx.reply, "H059" );
							set_zd_data(DC_REPLY,g_pub_tx.reply);
							iErr_flag = 1;
							goto TrLog;
	 				}	
			}
			else if ( ret == 100 )
			{
					strcpy( cReturn, "����������" );
					sprintf(acErrMsg, "��ѯ������¼����[%s][%s]", sPay_agent.unit_no, sPay_agent.ac_no);
					WRITEMSG
					iErr_flag = 1;
					goto TrLog;
			}
			else if(ret)
			{
				strcpy( cReturn, "����������" );
				sprintf( acErrMsg,"��ѯ���ۼ�¼����[%s][%s]", sPay_agent.unit_no, sPay_agent.ac_no );
				WRITEMSG
				goto ErrExit;
			}
	}
 	else
 	{	
			ret = Ag_peritemmap_Sel( g_pub_tx.reply , &sAg_peritemmap, \
				"unit_no = '%s' and item_no = '%s'", \
				sPay_agent.unit_no, sPay_agent.item_no );
				/*ret= Ag_peritemmap_Dec_Sel(g_pub_tx.reply,);*/
			if ( ret == 0 )
			{
				pub_base_strpack( sAg_peritemmap.id_no );
				pub_base_strpack( sPay_agent.id_no );
				pub_base_strpack_all( sAg_peritemmap.name );
				pub_base_strpack_all( sPay_agent.name );
        	
				/*--- ֤������ͻ�������ͬ,����ʱ��Ϊ�ǲ�ͬ���� --------*/
				if ( strcmp(sAg_peritemmap.id_no, sPay_agent.id_no) && 
					strcmp(sAg_peritemmap.name, sPay_agent.name) && 
					sAg_unitinfo.opr_attri[0] == '1' &&				
					sAg_unitinfo.auto_flag[0] == '1' && strcmp(sAg_unitinfo.pay_type, "37" ) && 
					strcmp(sAg_unitinfo.pay_type, "38" ) && strcmp(sAg_unitinfo.pay_type, "39" ))
				{
					strcpy( cReturn, "�¿���" );
					sprintf(acErrMsg,"�¿���[%s]", sAg_peritemmap.item_no);
					WRITEMSG
					iK_flag = 1;
					iErr_flag = 1;
					goto TrLog;
					/*goto KAIHU;*/
				}
				/*----------------------------------------------------*/
					memset( &sMdm_ac_rel, 0x0, sizeof(sMdm_ac_rel) );
					ret1 = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdm_ac_rel, \
						"ac_no = '%s' and note_sts <> '2'", sAg_peritemmap.ac_no );
					if ( ret1 )
					{
						sprintf( cReturn, "�����˺�[%s]��Ϣ��", sAg_peritemmap.ac_no );
						sprintf( acErrMsg, "�����˺�[%s]��Ϣ��", sAg_peritemmap.ac_no );
						WRITEMSG
						iErr_flag = 1;
						goto TrLog;
					}
				/*----------------------------------------------------*/
				/* added by lianghaoran, 2009/2/13                    */
				
				if ( sAg_unitinfo.opr_attri[0] == '1' && strcmp(sAg_unitinfo.pay_type, "37" ) \
				&& strcmp(sAg_unitinfo.pay_type, "38" ) && strcmp(sAg_unitinfo.pay_type, "39" ) )
				{
					memset( &sMdm_ac_rel, 0x0, sizeof(sMdm_ac_rel) );
					ret1 = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdm_ac_rel, \
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
					ret1 = Dd_mst_Sel( g_pub_tx.reply, &sDd_mst, \
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
					if ( sDd_mst.ac_sts[0] != '1' || sDd_mst.hold_sts[0] == '1')	/*��������ȫ����*/
					{
						strcpy( cReturn, "��������ȫ����,��ʧ" );
						sprintf(acErrMsg,"�¿���[%s]", sAg_peritemmap.item_no);
						WRITEMSG
						iK_flag = 1;
						iErr_flag = 1;
						goto TrLog;
					}					
				}
				if(sAg_unitinfo.opr_attri[0] == '0')
				{
					vtcp_log("aaaaaaaaaaaaa[%s]\n",sAg_unitinfo.opr_attri);
					memset( &sMdm_ac_rel, 0x0, sizeof(sMdm_ac_rel) );
					ret1 = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdm_ac_rel, \
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
					ret1 = Dd_mst_Sel( g_pub_tx.reply, &sDd_mst, \
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
					if ( sDd_mst.ac_sts[0] != '1' || sDd_mst.hold_sts[0] == '1'||sMdm_ac_rel.note_sts[0]!='0' )	/*��������ȫ����*/
					{
						strcpy( cReturn, "����,��ȫ����͹�ʧ" );
						sprintf(acErrMsg,"��������ȫ����[%s]", sAg_peritemmap.item_no);
						WRITEMSG
						iK_flag = 1;
						iErr_flag = 1;
						goto TrLog;
					}					
					sumtram=0.00;
					ret=sql_sum_double("pay_agent","tram",&sumtram,"tx_date=%ld and unit_no='%s' and item_no='%s' and sata='1' and result='1'",g_pub_tx.tx_date,sAg_unitinfo.unit_no,sAg_peritemmap.item_no);
	 				if(sDd_mst.bal<sumtram)
	 				{
							strcpy( cReturn, "������" );
	 						sprintf(acErrMsg,"�˻�������[%s]", sAg_peritemmap.ac_no);
							WRITEMSG
							strcpy( g_pub_tx.reply, "H059" );
							set_zd_data(DC_REPLY,g_pub_tx.reply);
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
					ret1 = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdm_ac_rel, \
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
					ret1 = Dd_mst_Sel( g_pub_tx.reply, &sDd_mst, \
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
						iErr_flag = 1;
						goto TrLog;
					}
				}
				vtcp_log("%s,%d opr_attri=[%s],auto_flag=[%s]",__FILE__,__LINE__,sAg_unitinfo.opr_attri,sAg_unitinfo.auto_flag);
				vtcp_log("%s,%d pay_type=[%s]",__FILE__,__LINE__,sAg_unitinfo.pay_type);
				vtcp_log("%s,%d unit_no=[%s]",__FILE__,__LINE__,sAg_unitinfo.unit_no);
				if ( sAg_unitinfo.opr_attri[0] == '1' && \
					sAg_unitinfo.auto_flag[0] == '1' && \
					strncmp( sAg_unitinfo.pay_type, "31", 2 ) != 0 && strcmp(sAg_unitinfo.pay_type, "37" ) && \
					strcmp(sAg_unitinfo.pay_type, "38" ) && strcmp(sAg_unitinfo.pay_type, "39" ) )	/*����,�Զ���*/
				{
					strcpy( cReturn, "�¿���" );
					sprintf(acErrMsg,"�¿���[%s]", sPay_agent.item_no);
					WRITEMSG
					iK_flag = 1;
					iProc_flag = 1;
					goto KAIHU;
				}
				else if ( sAg_unitinfo.opr_attri[0] == '1' && \
					sAg_unitinfo.auto_flag[0] == '1' && \
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
	}		
			sprintf(acErrMsg,"---------------------��ʼ���ڴ�ȡ����-------------------");
			WRITEMSG

			strcpy( g_pub_tx.ac_no, sAg_peritemmap.ac_no );
			g_pub_tx.ac_seqn = 1;
		
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
				
				strcpy( g_pub_tx.prt_ind, "0" );
				set_zd_data( "102O", "0" );
				
				set_zd_data( "102J", "01" );
				set_zd_data( "102K", "2" );
				set_zd_double( "102F", g_pub_tx.tx_amt1 );
				set_zd_double( "102P", 0.00 );
				set_zd_double( "102Q", 0.00 );
				strcpy(g_pub_tx.brf, "����");
			}
			/** strcpy(g_pub_tx.brf, sPay_agent.remark); **/
			strcpy(g_pub_tx.ct_ind,"2");
			strcpy(g_pub_tx.ac_wrk_ind,"001001001");
			strcpy(g_pub_tx.hst_ind,"1");
			g_pub_tx.svc_ind=1001;
			strcpy( g_pub_tx.tx_code, "G008" );
			sprintf(acErrMsg,"BEG D099[%s][%s][%d]",g_pub_tx.ac_no,g_pub_tx.ac_wrk_ind,g_pub_tx.svc_ind);
			WRITEMSG
			
			if ( sAg_unitinfo.opr_attri[0] == '0' )	/*����ҵ����*/
			{
				/*
				sql_commit();
				sql_begin();
				*/
			}

			ret = pub_acct_trance();
			if( ret != 0 )
			{
				sprintf( cReturn, "%s", g_pub_tx.reply );
				sprintf(acErrMsg,"���ô�ȡ������ʧ��[%s]", g_pub_tx.reply );
				WRITEMSG
				goto ErrExit;
			}
			
			ret = pubf_acct_proc( g_pub_tx.sub_tx_code );
			if ( ret )
			{
				sprintf( cReturn, "%s", g_pub_tx.reply );
				sprintf(acErrMsg,"���ô�ȡ������ʧ��[%s]", g_pub_tx.reply );
				WRITEMSG
				goto ErrExit;
			}
			
			strcpy( xs_acno, sAg_peritemmap.ac_no );
			
			goto NOKAIHU;
        	
KAIHU:  	
			memset(&cif_id_code_rel, 0x0, sizeof(cif_id_code_rel));
			memset(&cif_basic_inf, 0x0, sizeof(cif_basic_inf));
        	
			strcpy(cif_id_code_rel.id_type, cIdtp);/* ֤������ */
			
			pub_base_strpack( sPay_agent.id_no );
			if ( strlen( sPay_agent.id_no ) == 0 )
			{
				sprintf(acErrMsg,"����ʱ֤�����벻��Ϊ��"  );
				WRITEMSG
				strcpy( cReturn, "����֤�����ܿ�");
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
				sprintf(acErrMsg,"�ÿͻ��Ѿ�ӵ�пͻ�ID![%s][%s]",cIdtp,sPay_agent.id_no);
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
		
			g_pub_tx.ac_id = 0;	 /*test clean */
			g_pub_tx.ac_seqn = 0;
			strcpy(g_pub_tx.beg_note_no,"");
		
			strcpy(g_pub_tx.mdm_code,"0000");
			memset( g_pub_tx.beg_note_no, 0x0, sizeof(g_pub_tx.beg_note_no) );
		
			strcpy(g_pub_tx.id_type, cIdtp);	
			strcpy(g_pub_tx.id_no, sPay_agent.id_no);
			strcpy(g_pub_tx.name, sPay_agent.name );
		
			strcpy(g_pub_tx.draw_uncon_yn,"N");
			strcpy(g_pub_tx.draw_pwd_yn,"Y");
			strcpy(g_pub_tx.qry_pwd,"888888");
			strcpy(g_pub_tx.draw_pwd,"888888");
			strcpy(g_pub_tx.draw_id_yn,"N");
			strcpy(g_pub_tx.draw_seal_yn,"N");
			
			strcpy(g_pub_tx.ac_wrk_ind,"001001001");
		
			strcpy(g_pub_tx.brf,"������");
		
			ret = pub_acct_opn_cif_mdm();
			if( ret )
			{
				sprintf(acErrMsg,"�����ʴ�!!![%d][%s][%s][%s]", \
					ret, cIdtp, sPay_agent.id_no,sPay_agent.name );
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
			strcpy(g_pub_tx.brf,"��������");
			
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
				sprintf(acErrMsg,"��������[%d]!",ret);
				WRITEMSG
				sprintf( cReturn, "%s", g_pub_tx.reply );
				goto ErrExit;
			}	
		
			memset( jz_acno, 0x0, sizeof(jz_acno) );
			jz_ac_seq = 0;
			sprintf(jz_acno,"%d",g_pub_tx.ac_id);
			jz_ac_seq = g_pub_tx.ac_seqn;
		
			sprintf(acErrMsg,"���ɵļ����˺ź��˺���ŷֱ�Ϊ[%s] [%d]",jz_acno,g_pub_tx.ac_seqn);
			WRITEMSG
		
NOKAIHU:		
			/*--- ά�����ո������뵥λ���ձ�ag_peritemmap ---*/
			if ( iProc_flag == 1 )		/*--- ���Ӷ�Ӧ��ϵ ---*/
			{
				memset( &sAg_peritemmap, 0x00, sizeof(sAg_peritemmap) );
			
				strcpy( sAg_peritemmap.bat_flag, "1" );
				strcpy( sAg_peritemmap.remark, sAg_unitinfo.pay_type );
			
				strcpy( sAg_peritemmap.unit_no, sPay_agent.unit_no );
				strcpy( sAg_peritemmap.item_no, sPay_agent.item_no );
				strcpy( sAg_peritemmap.name, sPay_agent.name );
				strcpy( sAg_peritemmap.id_type, cIdtp );
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
					id_no = '%s' , ac_no = '%s', opn_date = %ld, opn_tel= '%s'\
					where unit_no = '%s' and item_no = '%s'", \
					sPay_agent.name, sPay_agent.id_no, xs_acno, \
					g_pub_tx.tx_date,g_pub_tx.tel, \
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
			if (ret )
			{
				strcpy( cReturn, "д�ո�����ʷ�����");
				sprintf(acErrMsg,"����ag_paytlcs����[%d]",ret);
				WRITEMSG
				goto ErrExit;
			}
		
			ret = sql_execute( "update pay_agent set sata = '0', \
				result = '0', result_name = '%s' \
				where unit_no = '%s' and item_no = '%s'" ,\
				cReturn, sPay_agent.unit_no,sPay_agent.item_no );
			if ( ret )
			{
				strcpy( cReturn, "����δ�����¼״̬��" );
				sprintf(acErrMsg,"����pay_agent����[%d][%s][%s]", \
					sPay_agent.unit_no, sPay_agent.item_no, ret);
				WRITEMSG
				goto ErrExit;
			}

TrLog:
			pub_base_strpack( sPay_agent.name );
			if ( iErr_flag == 0)
			{
				lSnum = lSnum + 1;
				dSamt = dSamt + sPay_agent.tram;
				/*******add by ligl 2006-11-23 17:11********/
				/*start ����ȼ��*/
				if(sAg_unitinfo.opr_attri[0]=='0' && memcmp(sAg_unitinfo.pay_type,"44",2)==0)
	  			{						
					fprintf( fp0,"%s|%.2lf|%4s|%ld|%2s|%08ld|%5s\n", sPay_agent.remark,sPay_agent.tram,"9999",g_pub_tx.tx_date+1,"00",g_pub_tx.trace_no ,g_pub_tx.tx_br_no);	  										
				
				}
				/*start ��������ˮ*/
				else if(sAg_unitinfo.opr_attri[0]=='0' && memcmp(sAg_unitinfo.pay_type,"43",2)==0)
	  			{						
				/*	fprintf( fp0,"%-10s%-30s%08.0lf%8ld\r\n", sPay_agent.remark,sPay_agent.ac_no ,sPay_agent.tram*100,g_pub_tx.tx_date  );	*/  										
				
				}
				/*end ����*/
				if(sAg_unitinfo.opr_attri[0]=='0')
	  			{
					memset( &sMdm_ac_rel, 0x0, sizeof(sMdm_ac_rel) );
					ret1 = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdm_ac_rel, \
						"ac_no = '%s'", sAg_peritemmap.ac_no );
					if ( ret1 )
					{
						sprintf( cReturn, "�����˺�[%s]��Ϣ��", sAg_peritemmap.ac_no );
						sprintf( acErrMsg, "�����˺�[%s]��Ϣ��", sAg_peritemmap.ac_no );
						WRITEMSG
					}
					memset( &sDd_mst, 0x0, sizeof(sDd_mst) );
					ret1 = Dd_mst_Sel( g_pub_tx.reply, &sDd_mst, \
						"ac_id = %ld", sMdm_ac_rel.ac_id );
					if ( ret1 )
					{
						sprintf( cReturn, "�����˺�[%s]������Ϣ��", sAg_peritemmap.ac_no );
						sprintf( acErrMsg, "�����˺�[%s]������Ϣ��", sAg_peritemmap.ac_no );
						WRITEMSG
					}				
					fprintf( fp,"��%20s��%20s��%20s��%16.2lf��%16.2lf��%8s��\n", sPay_agent.item_no,sPay_agent.ac_no,sPay_agent.name,sPay_agent.tram,sDd_mst.bal,"��    ��");	  					
					/* д����Ϣ add by wudawei 20140610**/
					if(memcmp(sAg_unitinfo.pay_type,"44",2)==0)
					{
							ys_amt = atof(sPay_agent.result_name);/*����ֶ���ʱû�� ������ֶδ���ȼ����Ӧ�ս��*/
				    }else if(memcmp(sAg_unitinfo.pay_type,"43",2)==0)
					{
							ys_amt = sPay_agent.tram;/*����ֶ���ʱû�� ������ֶδ���ȼ����Ӧ�ս��*/
				    }else{
				  		    ys_amt = 0.00;
				    }
					pub_mob_sendmail_bat( sAg_unitinfo.pay_type, sAg_unitinfo.unit_no, "G008",  sPay_agent.ac_no, sAg_unitinfo.opr_attri, sPay_agent.tram,sDd_mst.bal, ys_amt);
					/**end wudawei 20140610**/
					if(tot_cnt==PAGE_CNT){
						fprintf( fp,"\f");
						tot_cnt=0;
					}
		  	}	/* ���շ���� */
			/*******add by ligl 2006-11-23 17:11********/
			else
			{
					memset( &sMdm_ac_rel, 0x0, sizeof(sMdm_ac_rel) );
					ret1 = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdm_ac_rel, \
						"ac_no = '%s'", sAg_peritemmap.ac_no );
					if ( ret1 )
					{
						sprintf( cReturn, "�����˺�[%s]��Ϣ��", sAg_peritemmap.ac_no );
						sprintf( acErrMsg, "�����˺�[%s]��Ϣ��", sAg_peritemmap.ac_no );
						WRITEMSG
					}
					memset( &sDd_mst, 0x0, sizeof(sDd_mst) );
					ret1 = Dd_mst_Sel( g_pub_tx.reply, &sDd_mst, \
						"ac_id = %ld", sMdm_ac_rel.ac_id );
					if ( ret1 )
					{
						sprintf( cReturn, "�����˺�[%s]������Ϣ��", sAg_peritemmap.ac_no );
						sprintf( acErrMsg, "�����˺�[%s]������Ϣ��", sAg_peritemmap.ac_no );
						WRITEMSG
					}				
					fprintf( fp,"��%20s��%20s��%16.2lf��%20s��%20s��%8s��\n", sPay_agent.item_no, sPay_agent.name, \
					sPay_agent.tram, sPay_agent.id_no, sPay_agent.ac_no, cReturn );
				/* д����Ϣ add by wudawei 20140610**/
					if(sPay_agent.tram >0)
					{
						pub_mob_sendmail_bat( sAg_unitinfo.pay_type, sAg_unitinfo.unit_no, "G008",  sPay_agent.ac_no, sAg_unitinfo.opr_attri, sPay_agent.tram,sDd_mst.bal, ys_amt); 
					}
					/**end wudawei 20140610**/
					if(tot_cnt==PAGE_CNT){
						fprintf( fp,"\f");
						tot_cnt=0;
					}
			}
				if ( iK_flag == 1 )
				{
					lK_num = lK_num + 1;	/*�¿�����*/
					dK_amt = dK_amt + sPay_agent.tram;	/*�¿����*/
				}
				
				if ( sAg_unitinfo.opr_attri[0] == '0' )
				{
					/*
					sql_commit();
					*/
				}
				tot_cnt++;
			}
			else
			{
				memset( &sRsp_code, 0x0, sizeof(sRsp_code) );
				ret1 = Rsp_code_Sel( g_pub_tx.reply , &sRsp_code, "code = '%s'", cReturn );
				if ( ret1 == 0 )
				{
					memset( cReturn, 0x0, sizeof(cReturn) );
					pub_base_strpack( sRsp_code.memo );
					strcpy( cReturn, sRsp_code.memo );
				}
				
				lFnum = lFnum + 1;
				dFamt = dFamt + sPay_agent.tram;
				/*******add by ligl 2006-11-23 17:11********/
				if(sAg_unitinfo.opr_attri[0]=='0')
	  		{
					memset( &sMdm_ac_rel, 0x0, sizeof(sMdm_ac_rel) );
					ret1 = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdm_ac_rel, \
						"ac_no = '%s'", sAg_peritemmap.ac_no );
					if ( ret1 )
					{
						sprintf( cReturn, "�����˺�[%s]��Ϣ��", sAg_peritemmap.ac_no );
						sprintf( acErrMsg, "�����˺�[%s]��Ϣ��", sAg_peritemmap.ac_no );
						WRITEMSG
					}
					memset( &sDd_mst, 0x0, sizeof(sDd_mst) );
					ret1 = Dd_mst_Sel( g_pub_tx.reply, &sDd_mst, \
						"ac_id = %ld", sMdm_ac_rel.ac_id );
					if ( ret1 )
					{
						sprintf( cReturn, "�����˺�[%s]������Ϣ��", sAg_peritemmap.ac_no );
						sprintf( acErrMsg, "�����˺�[%s]������Ϣ��", sAg_peritemmap.ac_no );
						WRITEMSG
					}				
					fprintf( fp1,"��%20s��%20s��%20s��%16.2lf��%16.2lf��%8s��\n", sPay_agent.item_no,sPay_agent.ac_no,sPay_agent.name,sPay_agent.tram,sDd_mst.bal,"ʧ    ��");	  			
					if(err_tot_cnt==PAGE_CNT){
						fprintf( fp1,"\f");
						err_tot_cnt=0;
					}
	  		}	/* ������� */
	  		/********/
	  		else
	  		{				
				fprintf( fp1,"��%20s��%20s��%16.2lf��%20s��%20s��%30s��\n", sPay_agent.item_no,sPay_agent.name, \
					sPay_agent.tram, sPay_agent.id_no,sPay_agent.ac_no, cReturn );
					if(err_tot_cnt==PAGE_CNT){
						fprintf( fp1,"\f");
						err_tot_cnt=0;
					}
			}
			err_tot_cnt++;
		}
	}
	Pay_agent_Clo_Sel();
	/*******add by ligl 2006-11-23 17:11********/		
		if(sAg_unitinfo.opr_attri[0]=='0' && memcmp(sAg_unitinfo.pay_type,"44",2)==0)
	  {	
	  		fclose(fp0);
	  }else if(sAg_unitinfo.opr_attri[0]=='0' && memcmp(sAg_unitinfo.pay_type,"43",2)==0)
	  {	
	  	/*	fclose(fp0); */
	  }
    if(sAg_unitinfo.opr_attri[0]=='0')
    {
   	fprintf( fp, "�����������������������۩��������������������۩��������������������۩����������������۩����������������۩���������\n" );
   	fprintf( fp, "                                                                                                ��������[%ld]\n" ,g_pub_tx.tx_date); /**���ӱ��������� zgf 20131125**/
		fprintf( fp1,"�����������������������۩��������������������۩��������������������۩����������������۩����������������۩���������\n" );
    }
    else
    { 
   	/*******end by ligl 2006-11-23 17:11********/
		fprintf( fp, "�����������������������۩��������������������۩����������������۩��������������������۩��������������������۩���������\n" );
		fprintf( fp, "                                                                                                ��������[%ld]\n" ,g_pub_tx.tx_date); /**���ӱ��������� zgf 20131125**/
		fprintf( fp1,"�����������������������۩��������������������۩����������������۩��������������������۩��������������������۩�������������������������������\n" );
    }
		fclose(fp);
		fclose(fp1);
		
		if ( lFnum > 0 )	/*��ʧ�ܼ�¼*/
		{
			memset( filename11, 0x0, sizeof(filename11) );
			sprintf( filename11, "%s/report/%s/BR1%s.name", getenv("HOME"), sAg_unitinfo.opn_br_no, sAg_unitinfo.unit_no );
		
			fp11 = fopen( filename11, "w" );/*���ļ�������¼��ϸ��������*/
			if( fp11==NULL )
			{
				sprintf(acErrMsg," open file [%s] error ", filename11 );
				WRITEMSG
				strcpy( g_pub_tx.reply,"S047" );
				fclose( fp );
				set_zd_data(DC_REPLY,g_pub_tx.reply);
			}
			
			fprintf( fp11, "BR1%s.txt ����ҵ��ʧ����ϸ�嵥\n", sAg_unitinfo.unit_no );
			
			fclose( fp11 );
			
			if ( sAg_unitinfo.opr_attri[0] == '1' )
			{
				/*continue;	*��������Ͳ���Ҫ��������*/
			}
		}

		if ( lSnum > 0 )	/*�гɹ���¼*/
		{
			sprintf(acErrMsg,"-------------��ʼ��λ�˺Ŵ�ȡ����-----------");
			WRITEMSG

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
					set_zd_double( "1208", g_pub_tx.tx_amt1 );
					
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
					set_zd_double( "1218", g_pub_tx.tx_amt1 );
					
					strcpy(g_pub_tx.brf, "����");
				}				
			}
			else
			{
				strcpy( g_pub_tx.ac_id_type,"1" ); /*�˻�����Ϊ����*/
				strcpy( g_pub_tx.ac_get_ind,"00" ); /*������δ��ȡ�ֻ�*/
				g_pub_tx.svc_ind=1001;
								
				if ( sAg_unitinfo.opr_attri[0] == '1' )
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
					set_zd_double( "1013", g_pub_tx.tx_amt1 );					
					
					strcpy(g_pub_tx.brf, "����");
				}
			}		
			
			strcpy(g_pub_tx.ct_ind,"2");
			strcpy(g_pub_tx.ac_wrk_ind,"001001001");
			strcpy(g_pub_tx.hst_ind,"1");
			
			ret=pub_acct_trance();
			if( ret )
			{
				sprintf( cReturn, "�Ǵ���λ�˳���[%s]", g_pub_tx.reply );
				sprintf(acErrMsg,"���ô�ȡ������ʧ��[%s]", g_pub_tx.reply );
				WRITEMSG
				goto ErrExit;
			}
			
			ret = pubf_acct_proc( g_pub_tx.sub_tx_code );
			if ( ret )
			{
				sprintf( cReturn, "�Ǵ���λ�˳���[%s]", g_pub_tx.reply );
				sprintf( acErrMsg, "���ô�ȡ������ʧ��[%s]", g_pub_tx.reply );
				WRITEMSG
				goto ErrExit;
			}	
                	
			g_pub_tx.ac_id = 0;
			g_pub_tx.ac_seqn = 0;
			g_pub_tx.tx_amt1 = 0.00;        /*������*/
			
			strcpy( g_pub_tx.ac_id_type,"0" ); /*�˻�������Ϊ����*/			
			
			memset( filename01, 0x0, sizeof(filename01) );
			sprintf( filename01, "%s/report/%s/BR0%s.name", getenv("HOME"), sAg_unitinfo.opn_br_no, sAg_unitinfo.unit_no );
		
			fp01 = fopen( filename01, "w" );
			if( fp01==NULL )
			{
				sprintf(acErrMsg," open file [%s] error ", filename01 );
				WRITEMSG
				strcpy( g_pub_tx.reply,"S047" );
				fclose( fp );
				set_zd_data(DC_REPLY,g_pub_tx.reply);
			}
			else
			{
				fprintf( fp01, "BR0%s.txt ����ҵ��ɹ���ϸ�嵥\n", sAg_unitinfo.unit_no );
			}			

			fclose( fp01 );
			
			memset( filename3, 0x0, sizeof(filename3) );
			sprintf( filename3, "%s/report/%s/BRDWHZ%s.txt", getenv("HOME"), sAg_unitinfo.opn_br_no, sAg_unitinfo.unit_no );
			
			fp3 = fopen( filename3, "w" ); /*------��ȷ����������-------*/
			if( fp1==NULL )
			{
				sprintf(acErrMsg," open file [%s] error ",filename3 );
				WRITEMSG
				strcpy( g_pub_tx.reply,"S047" );
			}
			else
			{
				memset( &sCom_branch, 0x0, sizeof(sCom_branch) );
				ret = Com_branch_Sel( g_pub_tx.reply, &sCom_branch, "br_no = '%s'", g_pub_tx.tx_br_no );
				
				pub_base_strpack( sCom_branch.br_name );
				pub_base_strpack( sAg_unitinfo.bank_acct_no );
				fprintf( fp3, "				[%s]��������λ��Ʊ	\n", sAg_unitinfo.unit_name );
				fprintf( fp3, "������: %s    			�������� : %s\n", g_pub_tx.tx_br_no, sCom_branch.br_name );
				fprintf( fp3, "��λ����: %s   			��λ����: %s  \n", sAg_unitinfo.unit_no, sAg_unitinfo.unit_name );
				fprintf( fp3, "���� : %ld 			��λ�˺ţ�%s \n", g_pub_tx.tx_date, sAg_unitinfo.bank_acct_no );
				
				fprintf( fp3, "�����������Щ����������������������������������������Щ��������Щ�����������������\n" );
				fprintf( fp3, "����λ��ũ�           ��   λ   ��   ��            ��  ����  ��    ��   ��     ��\n" );
				fprintf( fp3, "�����������੤���������������������������������������੤�������੤����������������\n" );
				fprintf( fp3, "��%8s��%40s��%8ld��%16.2lf��\n", sAg_unitinfo.unit_no, sAg_unitinfo.unit_name, lSnum, dSamt );
				fprintf( fp3, "�����������੤���������������������������������������੤�������੤����������������\n" );
				fprintf( fp3, "�� ��  �� ��                                        ��%8ld��%16.2lf�� \n", lSnum, dSamt );
				fprintf( fp3, "�����������ة����������������������������������������ة��������ة����������������� \n\n\n\n\n\n\n" );
				
				fprintf( fp3, "				[%s]����������˻��ܴ�Ʊ	\n", sAg_unitinfo.unit_name );
				fprintf( fp3, "������: %s    			�������� : %s\n", g_pub_tx.tx_br_no, sCom_branch.br_name );
				fprintf( fp3, "���� : %ld 			���ܿ�Ŀ �� \n", g_pub_tx.tx_date );
				
				fprintf( fp3, "�����������Щ����������������������������������������Щ��������Щ�����������������\n" );
				fprintf( fp3, "����λ��ũ�           ��   λ   ��   ��            ��  ����  ��    ��   ��     ��\n" );
				fprintf( fp3, "�����������੤���������������������������������������੤�������੤����������������\n" );
				fprintf( fp3, "��%8s��%40s��%8ld��%16.2lf��\n", sAg_unitinfo.unit_no, sAg_unitinfo.unit_name, lSnum, dSamt );
				fprintf( fp3, "�����������੤���������������������������������������੤�������੤����������������\n" );
				fprintf( fp3, "�� ��  �� ��                                        ��%8ld��%16.2lf�� \n", lSnum, dSamt );
				fprintf( fp3, "�����������ة����������������������������������������ة��������ة����������������� \n" );
			}			
			
			fclose( fp3 );	
			
			memset( filename31, 0x0, sizeof(filename31) );
			sprintf( filename31, "%s/report/%s/BRDWHZ%s.name", getenv("HOME"), sAg_unitinfo.opn_br_no, sAg_unitinfo.unit_no );
			
			fp31 = fopen( filename31, "w" );
			if( fp31==NULL )
			{
				sprintf(acErrMsg," open file [%s] error ", filename31 );
				WRITEMSG
				strcpy( g_pub_tx.reply,"S047" );
				fclose( fp );
				set_zd_data(DC_REPLY,g_pub_tx.reply);
			}
			else
			{
				fprintf( fp31, "BRDWHZ%s.txt ����ҵ��ɹ������嵥\n", sAg_unitinfo.unit_no );
			}
			
			fclose( fp31 );
		}

		lSnum = 0;	/*�ɹ�����*/
		lFnum = 0;	/*ʧ�ܱ���*/
		dSamt = 0.00;	/*�ɹ����*/
		dFamt = 0.00;	/*ʧ�ܽ��*/
	}
	Ag_unitinfo_Clo_Sel();	
	ret = sql_execute( "insert into pay_agent_bk select * from 	pay_agent where tx_date = %ld",g_pub_tx.tx_date );
	ret = sql_execute( "delete from pay_agent where tx_date = %ld",g_pub_tx.tx_date );
OkExit:
	set_zd_data( "DC_TX_BR_NO", "99999" );
	sql_commit();
    strcpy( g_pub_tx.reply, "0000" );
    sprintf( acErrMsg, "������������ҵ����ɹ���ϣ�" );
    WRITEMSG
    set_zd_data( DC_REPLY, g_pub_tx.reply );
    return 0;
ErrExit:
	set_zd_data( "DC_TX_BR_NO", "99999" );
	fclose(fp);
	fclose(fp1);
	Pay_agent_Clo_Sel();
	sql_rollback();
    sprintf(acErrMsg,"������������ҵ����ʧ�ܣ�[%s]", g_pub_tx.reply );
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
