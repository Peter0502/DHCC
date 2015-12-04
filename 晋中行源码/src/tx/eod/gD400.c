/*************************************************
* �� �� ��: gD400.c
* ����������Ϊȼ����˾��������
* ��    ��: ���Ϊ
* �������: 2014��03��14��
* �޸�����:
* �� �� ��:
* �޸�����: 
* �����ļ���ʽ�� ú����|���|����
* insert into com_eod_exec (DO_SEQ, SUB_TX_CODE, PROG, PROG_NAME, BR_TYPE, EXEC_PD, PD_MONTH, EXEC_BEG, EXEC_END, DO_TYPE, USING, HOLIDAY, FINISH, EOY_NUM, EOY_FINISH)
values (123, 'Z110', 'gD400', 'ȼ�����۵���', '1111111111', '0', '0', '00', '00', '0', 'Y', 'Y', 'N', '3', 'Y');
*************************************************/
#define ERR_DEAL  {WRITEMSG goto ErrExit;}
#define EXTERN
#define cIdtp "1"		
#include "public.h"
#include <errno.h>
#include "pay_agent_c.h"
#include "ag_peritemmap_c.h"
#include "ag_unitinfo_c.h"
#include "cif_basic_inf_c.h"
#include "cif_id_code_rel_c.h"
#include "dd_mst_c.h"
#include "mdm_ac_rel_c.h"

gD400()
{
	char filename[248];
	char filename1[248];
	char filename2[248];
	char filename3[248];
	FILE *fp=NULL;
	FILE *fp1=NULL;
	FILE *fp2=NULL;
	FILE *fp3=NULL;
	
	char tmpstr[1024];
	char cUnit_no[5];
	char fldstr[5][100];
	char cReturn[32];
	char sItem_no[20];
  char tx_date[9];
  char tmp[3];
	
	int ret = 0, ins_flag=0, i=0;
	
	int iProc_flag = 0;		/*�����־*/
	int iProc_flag1 = 0;	/*�ɹ�ʧ�ܱ�־*/
	int iErr_flag = 0;		/*�Ƿ���ʧ�ܼ�¼��־*/
	
	double ys_amt = 0.00;
	double tx_amt = 0.00;
	double dd_bal = 0.00;
	char sTx_amt[18];
	
	long lSnum = 0;		/*�ɹ�����*/
	long lFnum = 0;		/*ʧ�ܱ���*/

	double dSamt = 0.00;	/*�ɹ����*/
	double dFamt = 0.00;	/*ʧ�ܽ��*/
	
	struct pay_agent_c sPay_agent;
	struct ag_peritemmap_c sAg_peritemmap;
	struct ag_unitinfo_c sAg_unitinfo;
	struct dd_mst_c sDd_mst;
	struct mdm_ac_rel_c sMdm_ac_rel;
  struct in_mst_c sIn_mst;
  
  sql_begin();
  
  memset(tx_date,0x00,sizeof(tx_date));
	sprintf(tx_date, "%ld", g_pub_tx.tx_date); 
	
	memset(tmp,0x00,sizeof(tmp));
	strcpy(tmp, tx_date+6);
	if(strncmp(tmp ,"22", 2)!=0)
	{
		vtcp_log("����22�� ������ú����[%s]",tx_date);
		goto OkExit;
  }

	memset(filename,0x0,sizeof(filename));
	sprintf(filename, "%s/k01%ld.txt", getenv("FILDIR"), g_pub_tx.tx_date); 

	pub_base_sysinit();
	/*---------------------------  �鵥λ��Ϣ ȼ��44 ----------------------------------*/	
	memset( &sAg_unitinfo, 0x0, sizeof(sAg_unitinfo) );
	ret = Ag_unitinfo_Sel ( g_pub_tx.reply, &sAg_unitinfo,
			"pay_type = '%s'" , "44" );
	if( ret )
	{
		sprintf(acErrMsg," ��ѯ��λ���ϳ���[%d]!!!", ret );
		WRITEMSG
		goto ErrExit;
	}
	
	memset(cUnit_no,0x00,sizeof(cUnit_no));
	strcpy(cUnit_no, sAg_unitinfo.unit_no);
	pub_base_strpack( sAg_unitinfo.unit_name );
	pub_base_strpack( sAg_unitinfo.pay_type );
	
	/*---------------------------------------�Ѽ�¼��ȷ�ʹ�����Ϣ���ļ���ʼ��------------------------------*/
	memset(filename1,0x0,sizeof(filename1));
	sprintf(filename1,"%s/0%s%ld%s.txt",getenv("FILDIR"),cUnit_no,g_pub_tx.tx_date,g_pub_tx.tel);
	fp1 = fopen( filename1,"w" );	/*���ļ�������¼��ȷ������*/
	if( fp1==NULL )
	{
		sprintf(acErrMsg," open file [%s] error ",filename1 );
		WRITEMSG
		strcpy( g_pub_tx.reply,"S047" );
		goto ErrExit;
	}

	fprintf( fp1, "                                     [%s-%s]��������ɹ���¼[%s-%ld]\r\n", sAg_unitinfo.unit_no, sAg_unitinfo.unit_name,g_pub_tx.tel,g_pub_tx.tx_date );
	fprintf( fp1, "�����������������������ө��������������������ө����������������ө��������������������ө��������������������ө���������\r\n" );
	fprintf( fp1, "��%20s��%20s��%16s��%20s��%20s��%8s��\r\n", "������", "�ͻ�����", "���׽��", "֤������", "�˺�", "������" );
	fprintf( fp1, "�����������������������㩥�������������������㩥���������������㩥�������������������㩥�������������������㩥��������\r\n" );

	memset(filename2,0x0,sizeof(filename2));
	sprintf(filename2,"%s/1%s%ld%s.txt",getenv("FILDIR"),cUnit_no,g_pub_tx.tx_date,g_pub_tx.tel);
	fp2 = fopen( filename2,"w" );	/*���ļ�������¼��������*/
	if( fp2==NULL )
	{
		sprintf(acErrMsg," open file [%s] error ",filename2 );
		WRITEMSG
		strcpy( g_pub_tx.reply,"S047" );
		goto ErrExit;
	}

	fprintf( fp2, "\r\n\r\n\r\n                         [%s-%s]��������ʧ�ܼ�¼[%s-%ld]\r\n", sAg_unitinfo.unit_no, sAg_unitinfo.unit_name,g_pub_tx.tel,g_pub_tx.tx_date);
	fprintf( fp2, "�����������ө����������ө��������������������ө����������������ө������ө�������������������������������\r\n" );
	fprintf( fp2, "��%8s��%10s��%20s��%16s��%6s��%30s��\r\n", "","�ͻ�����", "�˺�", "���", "�û���", "������" );
	fprintf( fp2, "�����������㩥���������㩥�������������������㩥���������������㩥�����ө�������������������������������\r\n" );

	/*---------------------------------  ��Ҫ�����ļ�    -----------------------------------*/
	fp = fopen( filename,"r" );
	if( fp==NULL )
	{
		sprintf( acErrMsg," open file [%s] error ", filename );
		strcpy( g_pub_tx.reply, "S047" );
		goto ErrExit;
	}
	
	while(1)
	{
		iProc_flag = 0;
		iProc_flag1 = 0;  
		
		fgets( tmpstr,1024,fp );
		if( feof(fp) )
		{
			break;
		}
		
		memset(fldstr,0x0,sizeof(fldstr));
		for( i=0; i<3; i++ )
		{
			ret=pub_base_cut_str( tmpstr,fldstr[i],"|",i+1 );
			vtcp_log(" [%d] [%s] ",i+1,fldstr[i]);
			pub_base_strpack( fldstr[i] );
		}
		if ( strlen(fldstr[0]) == 0 || strlen(fldstr[1]) == 0 )
		{
			continue;
		}
		memset(cReturn,0x00,sizeof(cReturn));
		vtcp_log(" ���� [%s] ",tx_date);
		if (strncmp(fldstr[2] ,tx_date,8) !=0 )
		{
				sprintf( cReturn, "�������ڲ���[%s][%s]", fldstr[0], fldstr[2]);
				sprintf( acErrMsg, "�������ڲ���[%s][%s]", fldstr[0], fldstr[2]);					
				WRITEMSG
				iProc_flag1 = 1;
				iErr_flag = 1;
				goto TrLog;
		}
		memset(sItem_no,0x00,sizeof(sItem_no));
		memset( &sAg_peritemmap, 0x0, sizeof(sAg_peritemmap) );
		ret = Ag_peritemmap_Sel( g_pub_tx.reply , &sAg_peritemmap,"unit_no = '%s' and remark = '%s' and stat='0'",cUnit_no, fldstr[0]);
		if(ret)
		{					
				sprintf( cReturn, "�û���Ų����ڣ�[%s]", fldstr[0]);
				sprintf( acErrMsg, "�û���Ų����ڣ�[%s][%s]",cUnit_no,fldstr[0]);					
				WRITEMSG
				iProc_flag1 = 1;
				iErr_flag = 1;
				goto TrLog;
		}
		strcpy( sItem_no,sAg_peritemmap.item_no);
		/**------------ ÿ��ѭ�������³�ʼ��ȫ�ֱ��� -------------**/
		memset( &g_pub_tx, 0x00, sizeof(g_pub_tx) );
		pub_base_sysinit();
		
		memset( &sMdm_ac_rel, 0x0, sizeof(sMdm_ac_rel));
		ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdm_ac_rel, "ac_no = '%s'", sAg_peritemmap.ac_no );
		if ( ret )
		{
			sprintf( cReturn, "�����˺�[%s]��Ϣ��", sAg_peritemmap.ac_no );
			sprintf( acErrMsg, "�����˺�[%s]��Ϣ��", sAg_peritemmap.ac_no );
			WRITEMSG
			iProc_flag1 = 1;
			iErr_flag = 1;
			goto TrLog;
		}
		pub_base_strpack( sMdm_ac_rel.note_sts );
		vtcp_log("%s,%d sMdm_ac_rel.note_sts =======[%s]",__FILE__,__LINE__,sMdm_ac_rel.note_sts);
		if(sMdm_ac_rel.note_sts[0] == '*')
		{
				sprintf( cReturn, "����״̬�쳣��" );
				sprintf( acErrMsg, "����״̬�쳣��" );
				WRITEMSG
				iProc_flag1 = 1;
				iErr_flag = 1;
				goto TrLog;
		}
		
				memset( &sDd_mst, 0x0, sizeof(sDd_mst) );
				ret = Dd_mst_Sel( g_pub_tx.reply, &sDd_mst, "ac_id = %ld", sMdm_ac_rel.ac_id );
				if ( ret )
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
					strcpy( cReturn, "������" );
					sprintf( acErrMsg, "������[%s]", sAg_peritemmap.ac_no);
					WRITEMSG
					iProc_flag1 = 1;
					iErr_flag = 1;
					goto TrLog;
				}
				if ( sDd_mst.hold_sts[0]=='1' )
				{
					strcpy( cReturn, "��ȫ����" );
					sprintf( acErrMsg, "��ȫ����[%s]", sAg_peritemmap.ac_no);
					WRITEMSG
					iProc_flag1 = 1;
					iErr_flag = 1;
					goto TrLog;
				}
				if ( sDd_mst.hold_sts[0]=='2' )
				{
					strcpy( cReturn, "ֻ������" );
					sprintf( acErrMsg, "ֻ������[%s]", sAg_peritemmap.ac_no);
					WRITEMSG
					iProc_flag1 = 1;
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
				ys_amt = atof(fldstr[1]);
				dd_bal = sDd_mst.bal - sDd_mst.ctl_amt ;
				if(sDd_mst.hold_sts[0]=='3')
				{
					dd_bal = dd_bal - sDd_mst.hold_amt;
				}
				if(ys_amt > dd_bal )
				{
					tx_amt = dd_bal;
				}else
				{
					tx_amt = ys_amt;
				}
				vtcp_log("[%s][%d] Ӧ��[%lf],���[%lf], ������[%lf], ���ƽ��[%lf], ������[%lf]",__FILE__,__LINE__,ys_amt,sDd_mst.bal,sDd_mst.hold_amt,sDd_mst.ctl_amt,tx_amt);
				if(tx_amt==0)
				{
					strcpy( cReturn, "���˻����Ϊ0" );
					sprintf( acErrMsg, "���˻����Ϊ0[%s]", sAg_peritemmap.ac_no);
					WRITEMSG
					iProc_flag1 = 1;
					iErr_flag = 1;
					/* д����Ϣ add by wudawei 20140610  ��β��#1#��#2#���Ϊ0���޷��Զ�����ȼ���ѣ��������쵽������������ɷ�**/
					pub_mob_sendmail_bat( sAg_unitinfo.pay_type, sAg_unitinfo.unit_no, "G008",  sAg_peritemmap.ac_no, sAg_unitinfo.opr_attri, 0.00,0.00, ys_amt); 
					/**end wudawei 20140610**/
					goto TrLog;
				}
				memset(sTx_amt,0x00,sizeof(sTx_amt));
				sprintf( sTx_amt, "%10.2lf", tx_amt);
				vtcp_log("[%s][%d] sAg_peritemmap.name=[%s] , sAg_peritemmap.ac_no=[%s]",__FILE__,__LINE__,sAg_peritemmap.name , sAg_peritemmap.ac_no);
				
		/*---------------�����������ݲ��ִ���-----------------*/
		memset(&sPay_agent,0x0,sizeof(sPay_agent));
		strcpy(sPay_agent.unit_no, sAg_unitinfo.unit_no );
		strcpy(sPay_agent.item_no, sItem_no );
		strcpy(sPay_agent.name, sAg_peritemmap.name );
		strcpy(sPay_agent.id_type, sAg_peritemmap.id_type );
		strcpy(sPay_agent.id_no, sAg_peritemmap.id_no );
		strcpy(sPay_agent.ac_no, sAg_peritemmap.ac_no );
		sPay_agent.trace_no = g_pub_tx.trace_no;
		sPay_agent.tx_date = g_pub_tx.tx_date;
		sPay_agent.end_date = g_pub_tx.tx_date;
		sPay_agent.tram = tx_amt;
		strcpy(sPay_agent.remark, sAg_peritemmap.remark );
		strcpy(sPay_agent.sata,"1");	/*-- ��Ҫ�������� --*/
		strcpy(sPay_agent.result,"1");	/*-- δ���� --*/
		strcpy(sPay_agent.cnt,"1");/*--���ո�����--*/
		strcpy(sPay_agent.result_name,fldstr[1]);/*--������Ӧ�ս��--*/
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
		
		
		/*-------------------    д��־  -----------------------------*/
	TrLog:
		if ( iProc_flag1 == 0 )
		{
			lSnum = lSnum + 1;
			dSamt = dSamt + tx_amt;
			/* fprintf( fp1, "��%20s��%20s��%16s��%20s��%20s��%8s��\r\n", "������", "�ͻ�����", "���׽��", "֤������", "�˺�", "������" ); */
			fprintf( fp1, "��%20s��%20s��%16s��%20s��%20s��%8s��\r\n",sItem_no, sAg_peritemmap.name,
					sTx_amt, sAg_peritemmap.id_no, sAg_peritemmap.ac_no, cReturn );
			vtcp_log("[%s][%d] д�ɹ���¼ sAg_peritemmap.ac_no=[%s] , sAg_peritemmap.item_no=[%s]",__FILE__,__LINE__,sAg_peritemmap.ac_no,sItem_no);
		}
		else
		{
			lFnum = lFnum + 1;
			dFamt = dFamt + atof(fldstr[1]);
			/* fprintf( fp2, "��%8s��%10s��%20s��%16s��%6s��%30s��\r\n", "������","�ͻ�����", "�˺�", "���", "�û���", "������" );*/
			fprintf( fp2, "��%8s��%10s��%20s��%16s��%6s��%30s��\r\n",sItem_no,sAg_peritemmap.name, sAg_peritemmap.ac_no,
					fldstr[1], fldstr[0], cReturn );
			vtcp_log("[%s][%d] дʧ�ܼ�¼[%s] sAg_peritemmap.ac_no=[%s] , sAg_peritemmap.item_no=[%s]",__FILE__,__LINE__,cReturn,sAg_peritemmap.ac_no,sItem_no);
		}
	}
	
	
	
	/* ѭ����� д�ļ��ײ� */
	
	fprintf( fp1, "�����������������������㩥�������������������㩥���������������㩥�������������������㩥�������������������㩥��������\r\n" );
	fprintf( fp1, "��%20s��%20d��%16.2f��%20s��%20s��%8s��\r\n","�ϼ�", lSnum, dSamt, "", "", "");
 	fprintf( fp1, "�����������������������۩��������������������۩����������������۩��������������������۩��������������������۩���������\r\n" );
 	
  fprintf( fp2, "�����������㩥���������㩥�������������������㩥���������������㩥�����ө�������������������������������\r\n" );
  fprintf( fp2, "��%8s��%10d��%20s��%16.2f��%6s��%30s��\r\n","�ϼ�",lFnum, "", dFamt, "", "" );
 	fprintf( fp2, "�����������۩����������۩��������������������۩����������������۩������۩�������������������������������\r\n" );

	fclose(fp);
	fclose(fp1);
	fclose(fp2);

	sprintf(acErrMsg,"�ɹ�����[%ld]���[%.2f];ʧ�ܱ���[%ld]���[%.2f]", lSnum, dSamt, lFnum, dFamt );		
	WRITEMSG
	ret = sql_execute( "update ag_unitinfo set beg_date = %ld, end_date = %ld \
		where unit_no = '%s'", g_pub_tx.tx_date, g_pub_tx.tx_date, cUnit_no );
	if ( ret )
	{
		sprintf( acErrMsg, "���ĵ�λ�����������ڴ�!!!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "H020" );
		ins_flag=1;
		goto ErrExit;
	}

	if ( iErr_flag != 0 ){
		if( ins_flag==0  ){
			vtcp_log("[%s][%d]���̷�ʽ���벿������",__FILE__,__LINE__);
			strcpy( cReturn, "�뿴 8608 ���ײ鿴������");
			goto OkExit;
		}
		/* ������ʽ����Ϊʧ��,����ȫ���ع� */
		vtcp_log("[%s][%d]��������ʧ��\r\n",__FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "H034" );
		goto ErrExit;
	}


OkExit:
	sql_commit();
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"��������ҵ������ϣ�[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;

ErrExit:
	fclose(fp);
	fclose(fp1);
	fclose(fp2);
	sql_rollback();
	sprintf(acErrMsg,"%s,%d,��������ʧ��ins[%d],iErr[%d]",__FILE__,__LINE__,ins_flag,iErr_flag);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
	
}
