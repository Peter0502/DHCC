/********************************************************
�������ƣ��������˽���sp6244	 
����������ֻ����ת���࣬�������ֽ���
�����ļ���hx_batch_compare_yyyymmdd.txt
�ļ���ʽ��ƽ̨����|ƽ̨��ˮ��|�����ܽ��|�ܱ���|
�����Ķ����ļ��в����������ͽ�����Ч״̬�ļ�¼
�����Ķ��ʽ���ļ�����txtĿ¼�� ������+�ն˺�  
��ʽ����������|ƽ̨��ˮ|ƽ̨�ܱ���|ƽ̨�ܽ��|�����ܱ���|�����ܽ��|���˽��|");
										���ܣ��ɹ���������ʧ�ܱ�����
���˳ɹ��ļ�¼��chnl_batch_acct��dz_ind��Ϊ1��ʧ�ܲ��䣬Ϊ0��
insert into tx_def values('6244','��������','10000000000000000000000000000000000000000000000000000000000000000001000000000000000000000000000000000000000000000000000000000100','1','1','2','3','');
insert into tx_flow_def values('6244','0','6244','#$');
insert into tx_sub_def values('6244','��������','sp6244','0','0');
wk 20110712
********************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define EXTERN
#include "public.h"
#include <errno.h>
#include "com_sys_parm_c.h"
#include "chnl_batch_acct_c.h"
sp6244()
{
	struct com_sys_parm_c	com_sys_parm;
	struct chnl_batch_acct_c chnl_batch_acct;
	
	char filename[60];
	char cFilename[80];
	char all_filename[248];/***ȫ·��**/
	char fldstr[10][100];
	char tmpstr[1024];
	char brf[21];
	char lst_trace_no[12];
	double tot_amt=0.00;
	double fact_amt=0.00;
	double fail_amt=0.00;
	long fact_num=0;
	long tot_num=0;
	long fail_num=0;
	long ttl_fail_num=0;/***ʧ���ܱ���**/
	long ttl_suc_num=0;/*****�ɹ��ܱ���*****/
	long pt_date=0;
		
	int ret=0;
	int i=0;
	char *p;
	FILE *fp=NULL;
	FILE *new_fp=NULL;
	
	memset(filename,0x0,sizeof(filename));
	memset(cFilename,0x0,sizeof(cFilename));
	memset(lst_trace_no,0x0,sizeof(lst_trace_no));
	memset(brf,0x0,sizeof(brf));
	memset(fldstr,0,sizeof(fldstr));
	memset(tmpstr,0,sizeof(tmpstr));
	memset(all_filename,0,sizeof(all_filename));
	memset(&com_sys_parm	,0x00,sizeof(com_sys_parm));
	memset(&chnl_batch_acct,0x00,sizeof(chnl_batch_acct));
	
	pub_base_sysinit();
	vtcp_log("[%s][%d],tx_date=[%d]",__FILE__,__LINE__,g_pub_tx.tx_date);
        set_zd_long("0460",g_pub_tx.tx_date);
	
	/********��ȡƽ̨����***************/
	
	get_zd_data("0810",brf);
	pub_base_strpack(brf);
	get_zd_long("0450",&pt_date);/****���Ķ�������*****/
	get_zd_data("0830",filename);/**�����ļ���***/
	
	
	ret = Com_sys_parm_Sel(g_pub_tx.reply,&com_sys_parm,"1=1");
	if(ret){
		sprintf( acErrMsg," ��ѯϵͳ���ڴ��� " );
		WRITEMSG
		goto ErrExit;
		}
	if(pt_date!=com_sys_parm.sys_date)
	{
		sprintf( acErrMsg," ����������������ڲ����!!ƽ̨��%ld�� ���ġ�%ld�� ",pt_date,com_sys_parm.sys_date);
		WRITEMSG
		goto ErrExit;
	}
	
	/*****�򿪶��˽���ļ�**********/
	pub_base_AllDwnFilName( cFilename );
	vtcp_log("[%s][%d]cFilename=[%s]",__FILE__,__LINE__,cFilename);
	new_fp=fopen(cFilename,"w");
	if( new_fp==NULL )
	{
		vtcp_log("[%s][%d]���ļ�[%s]��",__FILE__,__LINE__,filename);
		strcpy( g_pub_tx.reply,"P157" );
		goto ErrExit;
	}
	fprintf(new_fp,"~@��������|@ƽ̨��ˮ|@ƽ̨�ܱ���|@ƽ̨�ܽ��|@�����ܱ���|@�����ܽ��|@���˽��|\n");

	/******���ļ�������ļ��Ƿ�����*******/
	vtcp_log("filename[%s][%s][%d]",filename,__FILE__,__LINE__);
	pub_base_strpack(filename);
	sprintf(all_filename,"%s/txt/%s",getenv("HOME"),filename);

	pub_base_strpack(all_filename);
	fp = fopen( all_filename,"r" );
	if ( fp==NULL )
	{
		sprintf( acErrMsg," ���ļ����� [%s]  ", filename );
		strcpy( g_pub_tx.reply, "S047" );
		goto ErrExit;
	}
	while(1){
		memset( tmpstr, 0x0, sizeof(tmpstr) );
		pt_date=0;
		tot_amt=0.00;
		fact_amt=0.00;
		tot_num=0;
		fact_num=0;
		
		
    p=fgets(tmpstr,1024,fp);/*ȡ��һ����¼*/
		if(feof(fp))
		{
			break;
		}
		pub_base_strpack_all(tmpstr);
		
		memset(fldstr,0x0,sizeof(fldstr));
		for ( i=0;i<4;i++ )
		{
			ret=pub_base_cut_str( tmpstr,fldstr[i],"|",i+1 );
			vtcp_log(" [%d] [%s] ",i+1,fldstr[i]);
			pub_base_strpack_all( fldstr[i] );
		}
		/************
		fldstr[0] ƽ̨���� 
		fldstr[1] ƽ̨��ˮ��
		fldstr[2] �����ܽ��
		fldstr[3] �����ܱ���
		*******************/
		pt_date=atol(fldstr[0]);
		
		tot_amt=atof(fldstr[2]);
		tot_num=atol(fldstr[3]);
		vtcp_log("wkwk==========");
		/*******���������Ǽǲ�*******/
		ret=sql_sum_double("chnl_batch_acct","tx_amt",&fact_amt,"tx_date=%ld and pt_trace_no='%s' and sts not in('0','4') ",pt_date,fldstr[1]);
		if(ret){
			sprintf( acErrMsg," �����ܽ�����ƽ̨��ˮ��%s��!!��",fldstr[1]);
			WRITEMSG
			goto ErrExit;
			}
		vtcp_log("fact_amt==[%lf]",fact_amt);
		fact_num=sql_count("chnl_batch_acct","tx_date=%ld and pt_trace_no='%s' and sts not in('0','4')",pt_date,fldstr[1]);
		vtcp_log("fact_num==[%ld]",fact_num);
		
		/****����*****/
		if(tot_num!=fact_num || pub_base_CompDblVal(tot_amt,fact_amt)!=0)/*******����********/
		{
			ttl_fail_num++;
			fprintf(new_fp,"%ld|%s|%ld|%.2f|%ld|%.2f|ʧ��|\n",pt_date,fldstr[1],tot_num,tot_amt,fact_num,fact_amt);
			
		}
		else{/*******���*******/
			ret=sql_execute("update chnl_batch_acct set dz_ind='1' where tx_date=%ld and pt_trace_no='%s' and sts not in('0','4') ",pt_date,fldstr[1]);
			if(ret){
				sprintf( acErrMsg," ���¶���״̬����!![%d]",ret);
				WRITEMSG
				goto ErrExit;
				}
			ttl_suc_num++;
			fprintf(new_fp,"%ld|%s|%ld|%.2f|%ld|%.2f|�ɹ�|\n",pt_date,fldstr[1],tot_num,tot_amt,fact_num,fact_amt);
			
		}
		
	}
	fclose(fp);
	/*****���һ�£�������û�ж��**********/
	fact_num=0;/******�ܵı���*******/
	fail_num=0;/*****ͬһ��ˮ���ܱ���*******/
	fail_amt=0.00;/*******ͬһ��ˮ���ܽ��********/
	ret=Chnl_batch_acct_Dec_Upd(g_pub_tx.reply,"tx_date=%ld and dz_ind='0' and sts not in('0','4') order by pt_trace_no",com_sys_parm.sys_date);
	if(ret){
		sprintf( acErrMsg," ���α����!!ret��%d��",ret);
		WRITEMSG
		goto ErrExit;
		}
	while(1){
		memset(&chnl_batch_acct,0x00,sizeof(chnl_batch_acct));
		ret=Chnl_batch_acct_Fet_Upd(&chnl_batch_acct,g_pub_tx.reply);
		if(ret==100) {/***���һ��*****/
			if(fact_num!=0){
				fprintf(new_fp,"%ld|%s|%ld|%.2f|%ld|%.2f|ʧ��|\n",com_sys_parm.sys_date,lst_trace_no,0,0.00,fail_num,fail_amt);
				ttl_fail_num++;
				}
			break;
			}
		else if(ret){
		sprintf( acErrMsg," FET����!!ret��%d��",ret);
		WRITEMSG
		goto ErrExit;
		}
		vtcp_log("lst_trace_no====[%s]",lst_trace_no);
		if(strlen(lst_trace_no)!=0 && memcmp(lst_trace_no,chnl_batch_acct.pt_trace_no,sizeof(chnl_batch_acct.pt_trace_no)-1)){/***����ˮ��***/
			fprintf(new_fp,"%ld|%s|%ld|%.2f|%ld|%.2f|ʧ��|\n",com_sys_parm.sys_date,lst_trace_no,0,0.00,fail_num,fail_amt);
			ttl_fail_num++;
			fail_num=0;/********��0*********/
			fail_amt=0.00;
			}
		fact_num++;
		fail_num++;
		fail_amt+=chnl_batch_acct.tx_amt;
		memcpy(lst_trace_no,chnl_batch_acct.pt_trace_no,sizeof(chnl_batch_acct.pt_trace_no)-1);
		ret=Chnl_batch_acct_Upd_Upd(chnl_batch_acct,g_pub_tx.reply);
		if(ret){
			sprintf( acErrMsg," ���µǼǲ�״̬����%d��!!",ret);
			WRITEMSG
			goto ErrExit;
			}
	}
	Chnl_batch_acct_Clo_Upd();
	
	/*****����**********/
	fprintf(new_fp,"���ܣ��ɹ�������%ld��ʧ�ܱ�����%ld\n",ttl_suc_num,ttl_fail_num);
	fclose(new_fp);
	set_zd_data( DC_FILE_SND,"1" );

	
	
OkExit:
	sql_commit();
	vtcp_log("%s,%d,�����������!",__FILE__,__LINE__);
	strcpy(g_pub_tx.reply,"0000");
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	set_zd_data(DC_GET_MSG,g_pub_tx.reply);
	return(0);
ErrExit:
	sql_rollback();
	if(strlen(g_pub_tx.reply)==0||!strcmp(g_pub_tx.reply,"0000"))
	{
		strcpy(g_pub_tx.reply,"D103");
	}
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	set_zd_data(DC_GET_MSG,acErrMsg);
	vtcp_log("%s,%d,��������ʧ��!",__FILE__,__LINE__);
	if(fp!=NULL)
	fclose(fp);
	WRITEMSG
	return(1);
	}
	