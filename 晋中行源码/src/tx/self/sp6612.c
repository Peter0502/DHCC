/********************************************************
�������ƣ��������˽���sp6242	 
����������ֻ����ת���࣬�������ֽ���
�����ļ���hx_batch_yyyymmdd_���.txt
�ļ���ʽ����������|���|ת���˺�|ת�뿨��|���׽��|������|�����ѳ���|ժҪ
ע�������ѳ��� 0��ת���˺ų�  1��ת���˺ų�
��Ҫ�޸ĺ�SXFKM----�����ѿ�Ŀ
insert into tx_def values('6242','��������','10000000000000000000000000000000000000000000000000000000000000000001000000000000000000000000000000000000000000000000000000000100','1','1','2','3','');
insert into tx_flow_def values('6242','0','6242','#$');
insert into tx_sub_def values('6242','��������','sp6242','0','0');
wk 20110712
********************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define SXFKM "602101"
#define EXTERN
#include "public.h"
#include <errno.h>
#include "dd_mst_c.h"
#include "mdm_ac_rel_c.h"
#include "com_sys_parm_c.h"
#include "in_mst_c.h"
#include "com_item_c.h"
#include "chnl_batch_acct_c.h"
#include "counter_tlog_c.h"
sp6242()
{
	struct dd_mst_c	out_dd_mst;
	struct dd_mst_c	in_dd_mst;
	struct com_sys_parm_c	com_sys_parm;
	struct mdm_ac_rel_c out_mdm_ac_rel;
	struct mdm_ac_rel_c in_mdm_ac_rel;
	struct com_item_c com_item;
	struct in_mst_c in_mst;
	struct chnl_batch_acct_c chnl_batch_acct;
	struct counter_tlog_c g_counter_tlog;
	
	char filename[248];
	char filename_ret[400];
	char fldstr[10][100];
	char tmpstr[1024];
	char cmd[300];
	char ac_no[21];
	char brf[21];
	char bbmk[5];
	char cSeq[5];
	double tot_amt=0.00;
	double fact_amt=0.00;
	long fact_num=0;
	long tot_num=0;
	long pt_date=0;
	long lTrace_no=0;
		
	int ret=0;
	int in_flag=0;/****�ڲ��˱�־ 1 �ڲ��� 2 ���ڻ�*******/
	int trace_cnt=0;
	int i=0;
	int j=0;
	int k=0;
	int l=0;
	char *p;
	FILE *fp=NULL;
	FILE *fp1=NULL;
	
	memset(filename,0x0,sizeof(filename));
	memset(filename_ret,0x0,sizeof(filename_ret));
	memset(cmd,0x0,sizeof(cmd));
	memset(cSeq,0x0,sizeof(cSeq));
	memset(ac_no,0x0,sizeof(ac_no));
	memset(bbmk,0x0,sizeof(bbmk));
	memset(brf,0x0,sizeof(brf));
	memset(fldstr,0,sizeof(fldstr));
	memset(tmpstr,0,sizeof(tmpstr));
	memset(&out_dd_mst	,0x00,sizeof(out_dd_mst));
	memset(&in_dd_mst	,0x00,sizeof(in_dd_mst));
	memset(&com_sys_parm	,0x00,sizeof(com_sys_parm));
	memset(&out_mdm_ac_rel	,0x00,sizeof(out_mdm_ac_rel));
	memset(&in_mdm_ac_rel	,0x00,sizeof(in_mdm_ac_rel));
	memset(&com_item	,0x00,sizeof(com_item));
	memset(&in_mst	,0x00,sizeof(in_mst));
	memset(&chnl_batch_acct,0x00,sizeof(chnl_batch_acct));
	memset(&g_counter_tlog,0x00,sizeof(struct counter_tlog_c));
	
	pub_base_sysinit();
	vtcp_log("[%s][%d],tx_date=[%d]",__FILE__,__LINE__,g_pub_tx.tx_date);
	set_zd_long("0460",g_pub_tx.tx_date);
        
	/********��ȡƽ̨����***************/
	get_zd_data("0300",ac_no);/*****���˺�/��Ŀ********/
	pub_base_strpack(ac_no);
	get_zd_double("0400",&tot_amt);/**�ܽ��**/
	get_zd_long("0480",&tot_num);/***�ܱ���***/
	get_zd_data("0810",brf);
	pub_base_strpack(brf);
	get_zd_long("0440",&pt_date);/****ƽ̨����*****/
	get_zd_data("0830",filename);/**�����ļ���***/
	pub_base_strpack_all(filename);
	
	
	
	ret = Com_sys_parm_Sel(g_pub_tx.reply,&com_sys_parm,"1=1");
	if(ret){
		sprintf( acErrMsg," ��ѯϵͳ���ڴ��� " );
		WRITEMSG
		goto ErrExit;
		}
	if(pt_date!=com_sys_parm.sys_date)
	{
		sprintf( acErrMsg," ƽ̨������������ڲ����!!ƽ̨��%ld�� ���ġ�%ld�� ",pt_date,com_sys_parm.sys_date);
		WRITEMSG
		goto ErrExit;
	}

	/******ȡ�����ļ������********/
	for ( i=0;i<strlen(filename);i++ )
		{
			if(filename[i]=='_') 
			{
				j++;
				if(j==3) k=i+1;
			}
			if(filename[i]=='.')
			{
				l=i+1;
			}
		}
		
	memcpy(cSeq,filename+k,l-1-k);
	
	sprintf(filename_ret,"%s/txt/CNAPS2_batch_%ld_%s.ret",getenv("HOME"),com_sys_parm.sys_date,cSeq);
	/******���ļ�������ļ��Ƿ�����*******/
	vtcp_log("Filename[%s][%s][%d]",filename,__FILE__,__LINE__);
	pub_base_strpack(filename);
	fp = fopen( filename,"r" );
	if ( fp==NULL )
	{
		sprintf( acErrMsg," ���ļ����� [%s]  ", filename );
		strcpy( g_pub_tx.reply, "S047" );
		goto ErrExit;
	}
	while(1){
		memset( tmpstr, 0x0, sizeof(tmpstr) );
		
    p=fgets(tmpstr,1024,fp);/*ȡ��һ����¼*/
		if(feof(fp))
		{
			break;
		}
		pub_base_strpack_all(tmpstr);
		
		memset(fldstr,0x0,sizeof(fldstr));
		for ( i=0;i<8;i++ )
		{
			ret=pub_base_cut_str( tmpstr,fldstr[i],"|",i+1 );
			vtcp_log(" [%d] [%s] ",i+1,fldstr[i]);
			pub_base_strpack_all( fldstr[i] );
		}
		/************
		fldstr[0] �������� 
		fldstr[1] ���
		fldstr[2] ת���˺�
		fldstr[3] ת�뿨��
		fldstr[4] ���׽��
		fldstr[5] ������
		fldstr[6] �����ѳ���
		fldstr[7] ժҪ
		*******************/
		fact_num++;
		fact_amt+=atof(fldstr[4]);
	}
	fclose(fp);
	
	if(fact_num!=tot_num){
		sprintf( acErrMsg," �����ܱ������ļ��ܱ�����һ�¡�%ld����%ld��!!��",tot_num,fact_num);
		WRITEMSG
		strcpy(g_pub_tx.reply,"H019");
		goto ErrExit;
		}
		
	if(pub_base_CompDblVal(fact_amt,tot_amt)){
		sprintf( acErrMsg," �����ܽ�����ļ��ܽ�һ�¡�%lf����%lf��!!��",tot_amt,fact_amt);
		WRITEMSG
		strcpy(g_pub_tx.reply,"H019");
		goto ErrExit;
		}
		
		
		if (pub_base_PubQlsh(&lTrace_no, g_pub_tx.reply)){
       sprintf(acErrMsg, "ȡ������ˮ�Ŵ� [%d]", lTrace_no);
       WRITEMSG
       goto ErrExit;
   	}
	/********���ļ���ת��************/
	fp = fopen( filename,"r" );
	if ( fp==NULL )
	{
		sprintf( acErrMsg," ���ļ����� [%s]  ", filename );
		strcpy( g_pub_tx.reply, "S047" );
		goto ErrExit;
	}
	j=0;
	while(1){
		memset( tmpstr, 0x0, sizeof(tmpstr) );
		
    p=fgets(tmpstr,1024,fp);/*ȡ��һ����¼*/
		if(feof(fp))
		{
			break;
		}
		pub_base_strpack_all(tmpstr);
		
		memset(fldstr,0x0,sizeof(fldstr));
		for ( i=0;i<8;i++ )
		{
			ret=pub_base_cut_str( tmpstr,fldstr[i],"|",i+1 );
			vtcp_log(" [%d] [%s] ",i+1,fldstr[i]);
			pub_base_strpack_all( fldstr[i] );
		}
		/************
		fldstr[0] �������� 
		fldstr[1] ���
		fldstr[2] ת���˺�
		fldstr[3] ת�뿨��
		fldstr[4] ���׽��
		fldstr[5] ������
		fldstr[6] �����ѳ���
		fldstr[7] ժҪ
		*******************/
		if(j==0){
			fp1 = fopen( filename_ret,"w" );
			if ( fp1==NULL )
			{
				sprintf( acErrMsg," ���ļ����� [%s]  ", filename_ret );
				strcpy( g_pub_tx.reply, "S047" );
				goto ErrExit;
			}
		}
		j++;
		
		
		ret = Counter_tlog_Sel(g_pub_tx.reply, &g_counter_tlog, "", fldstr[0], fldstr[1]);
		vtcp_log("[%s][%d] ��ѯcounter_tlog ƽ̨����[%ld],ƽ̨��ˮ[%s]\n", __FILE__,__LINE__, fldstr[0], fldstr[1]);
		if(ret)
		{
			sprintf(acErrMsg, "��ѯ����ͨҵ������%d��",ret);
	    WRITEMSG
	    goto ErrExit;
		}
		if(ret == 100)
		{
			/*******ת���˺ż��**********/
			if(strlen(fldstr[2])==0){
				sprintf(acErrMsg, "ת���˺�Ϊ��,���[%d]", atoi(fldstr[1]));
	       WRITEMSG
	       goto FAILE;
	     }
	    else if(strlen(fldstr[2])<=8)/******��Ŀ��*******/
	    {
	    	ret=Com_item_Sel(g_pub_tx.reply,&com_item,"acc_no='%s'",fldstr[2]);
	    	if(ret==100){
	    		sprintf(acErrMsg, "��Ŀ�Ų�����[%s]", atoi(fldstr[2]));
	        WRITEMSG
	        goto FAILE;
	    	}
	    	else if(ret){
	    		sprintf(acErrMsg, "��ѯ��Ŀ�����%d��",ret);
	        WRITEMSG
	        goto ErrExit;
	    	}
	    	if(com_item.sub_acc_yn[0]=='Y'){
	    		sprintf(acErrMsg, "�ÿ�Ŀ���¼���Ŀ[%s]", atoi(fldstr[2]));
	        WRITEMSG
	        goto FAILE;
	    		}
	    	if(com_item.acc_knd[0]!='3'){
	    		sprintf(acErrMsg, "�ÿ�Ŀ�����ڲ��˻�[%s]", atoi(fldstr[2]));
	        WRITEMSG
	        goto FAILE;
	    		}
	    	/*****�ڲ��˱�־ 1 �ڲ���  0 ������**********/
	    	in_flag=1;
	    }
	    else 
	    {
	    	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&out_mdm_ac_rel,"ac_no='%s'",fldstr[2]);
	    	if(ret==100){
		    		sprintf(acErrMsg, "ת���˺Ų�����,[%s]", atoi(fldstr[2]));
		        WRITEMSG
		        goto FAILE;
		    	}
		    	else if(ret){
		    		sprintf(acErrMsg, "��ѯMdm_ac_rel�����%d��",ret);
		        WRITEMSG
		        goto ErrExit;
		    	}
		    if(out_mdm_ac_rel.note_sts[0]!='0'){
		    	sprintf(acErrMsg, "ת���˺Ž���״̬������,[%s]", atoi(fldstr[2]));
		        WRITEMSG
		        goto FAILE;
		    	}
		    	
		    if(fldstr[2][0]=='9')/***�ڲ��˺�******/
		    {
		    	ret=In_mst_Sel(g_pub_tx.reply,&in_mst,"ac_id=%ld and ac_seqn=0",out_mdm_ac_rel.ac_id);
		    	if(ret==100){
		    		sprintf(acErrMsg, "�ڲ��˺Ų�����,[%s]", atoi(fldstr[2]));
		        WRITEMSG
		        goto FAILE;
		    	}
		    	else if(ret){
		    		sprintf(acErrMsg, "��ѯ�ڲ����ļ�����%d��",ret);
		        WRITEMSG
		        goto ErrExit;
		    	}
		    	if(in_mst.sts[0]=='*'){
		    		sprintf(acErrMsg, "�ڲ��˺�������,[%s]", atoi(fldstr[2]));
		        WRITEMSG
		        goto FAILE;
		    		}
		    	in_flag=1;
		    }
		    else 
		    {
		    	ret=Dd_mst_Sel(g_pub_tx.reply,&out_dd_mst,"ac_id=%d and ac_seqn=1",out_mdm_ac_rel.ac_id);
		    	if(ret==100){
		    		sprintf(acErrMsg, "�����˺Ų�����,[%s]", atoi(fldstr[2]));
		        WRITEMSG
		        goto FAILE;
		    	}
		    	else if(ret){
		    		sprintf(acErrMsg, "��ѯ�������ļ�����%d��",ret);
		        WRITEMSG
		        goto ErrExit;
		    	}
		    	if(out_dd_mst.ac_sts[0]=='0'){
		    		sprintf(acErrMsg, "ת���˺ſ���δȷ��,[%s]", atoi(fldstr[2]));
		        WRITEMSG
		        goto FAILE;
		    		}
		    	if(out_dd_mst.ac_sts[0]=='*'){
		    		sprintf(acErrMsg, "ת���˺�������,[%s]", atoi(fldstr[2]));
		        WRITEMSG
		        goto FAILE;
		    		}
		    		if(out_dd_mst.hold_sts[0]=='1'){
		    		sprintf(acErrMsg, "ת���˺�Ϊ��ȫ����״̬,[%s]", atoi(fldstr[2]));
		        WRITEMSG
		        goto FAILE;
		    		}
		    		if(out_dd_mst.hold_sts[0]=='2'){
		    		sprintf(acErrMsg, "ת���˺�Ϊֻ������״̬,[%s]", atoi(fldstr[2]));
		        WRITEMSG
		        goto FAILE;
		    		}
		    		in_flag=0;
		    }
	  	}
	  	/****ת���˺ż��*****/
	  	if(strlen(fldstr[3])==0){
				sprintf(acErrMsg, "ת���˺�Ϊ��,���[%d]", atoi(fldstr[1]));
	       WRITEMSG
	       goto FAILE;
	     }
	     if(strlen(fldstr[3])!=13 ||strlen(fldstr[3])!=19){
				sprintf(acErrMsg, "ת���˺�λ������,���[%d]", atoi(fldstr[1]));
	       WRITEMSG
	       goto FAILE;
	     }
	  	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&in_mdm_ac_rel,"ac_no='%s'",fldstr[3]);
	    	if(ret==100){
		    		sprintf(acErrMsg, "ת�뿨�Ų�����,[%s]", atoi(fldstr[3]));
		        WRITEMSG
		        goto FAILE;
		    	}
		    	else if(ret){
		    		sprintf(acErrMsg, "��ѯMdm_ac_rel�����%d��",ret);
		        WRITEMSG
		        goto ErrExit;
		    	}
		   if(in_mdm_ac_rel.note_sts[0]!='0')
		   {
	   			sprintf(acErrMsg, "ת�뿨�Ž���״̬������,[%s]", atoi(fldstr[3]));
	        WRITEMSG
	        goto FAILE;
		   }
		   ret=Dd_mst_Sel(g_pub_tx.reply,&in_dd_mst,"ac_id=%ld and ac_seqn=1",in_mdm_ac_rel.ac_id);
		   if(ret==100){
		    		sprintf(acErrMsg, "ת���˺Ų�����,[%s]", atoi(fldstr[3]));
		        WRITEMSG
		        goto FAILE;
		    	}
		    	else if(ret){
		    		sprintf(acErrMsg, "��ѯ�������ļ�����%d��",ret);
		        WRITEMSG
		        goto ErrExit;
		    	}
		    
		    if(in_dd_mst.ac_sts[0]=='0'){
		    		sprintf(acErrMsg, "ת���˺ſ���δȷ��,[%s]", atoi(fldstr[3]));
		        WRITEMSG
		        goto FAILE;
		    		}
	  		if(in_dd_mst.ac_sts[0]=='*'){
		  		sprintf(acErrMsg, "ת���˺�������,[%s]", atoi(fldstr[3]));
		      WRITEMSG
		      goto FAILE;
	  		}
	  		if(out_dd_mst.hold_sts[0]=='1'){
		  		sprintf(acErrMsg, "ת���˺�Ϊ��ȫ����״̬,[%s]", atoi(fldstr[3]));
		      WRITEMSG
		      goto FAILE;
	  		}
	  		
			/******ÿ��ѭ����ʼ��ȫ�ֱ���************/
			memset(&g_pub_tx,0x00,sizeof(g_pub_tx));
			pub_base_sysinit();
				/****g_pub_tx�ṹ�帳ֵ----ת���˺�*********/
			strcpy(g_pub_tx.cur_no, RMB);
			strcpy(g_pub_tx.ct_ind, "2");
			g_pub_tx.trace_no=lTrace_no;
			trace_cnt++;
			g_pub_tx.trace_cnt =trace_cnt;
			
			if(strlen(brf)==0)
				strcpy(g_pub_tx.brf,"����ת��ת��");
			else 
				strcpy(g_pub_tx.brf,brf);
			if(in_flag==0){
				g_pub_tx.svc_ind=1001;
				strcpy(g_pub_tx.sub_tx_code, "D003");
				g_pub_tx.ac_seqn=1;
				}
			else{
				g_pub_tx.svc_ind=9001;
				strcpy(g_pub_tx.sub_tx_code, "A016");
				strcpy(g_pub_tx.ac_id_type,"9");
				g_pub_tx.ac_seqn=0;
				}
			strcpy(g_pub_tx.ac_no,fldstr[2]);
			g_pub_tx.tx_amt1=atof(fldstr[4]);
			strcpy(g_pub_tx.add_ind,"0"); 
			strcpy(g_pub_tx.hst_ind,"1"); /**�ռ�����ϸ**/
			strcpy(g_pub_tx.ac_wrk_ind,"0000010011");
			strcpy(g_pub_tx.ac_get_ind, "00");
			strcpy(g_pub_tx.note_type,"");
			strcpy(g_pub_tx.beg_note_no,"");
		
			if ( pub_acct_trance() )
			{
				sprintf(acErrMsg,"���ô�ȡ������ʧ��!");
				WRITEMSG
				goto FAILE;;
			}
			if(in_flag==0){
				set_zd_data("102J","01");
				set_zd_data("102K",g_pub_tx.ct_ind);
				set_zd_double("102F",g_pub_tx.tx_amt1);
				}
			else{
				set_zd_data("1204","01");
				set_zd_data("1205",g_pub_tx.ct_ind);
				set_zd_double("1208",g_pub_tx.tx_amt1);
				}
			if ( pubf_acct_proc(g_pub_tx.sub_tx_code) )
			{
				sprintf(acErrMsg,"�Ǽǻ����ˮ����!");
				WRITEMSG
				goto FAILE;;
			}
			/*******ת���˺�***********/
			memset(g_pub_tx.sub_tx_code, 0x0, sizeof(g_pub_tx.sub_tx_code));
			memset(g_pub_tx.brf, 0x0, sizeof(g_pub_tx.brf));
			memset(g_pub_tx.ac_no, 0x0, sizeof(g_pub_tx.ac_no));
			memset(g_pub_tx.add_ind, 0x0, sizeof(g_pub_tx.add_ind));
			g_pub_tx.ac_id=0;
			
			strcpy(g_pub_tx.ac_no,fldstr[3]);
			g_pub_tx.ac_seqn=1;
			
			if(strlen(brf)==0)
				strcpy(g_pub_tx.brf,"����ת��ת��");
			else 
				strcpy(g_pub_tx.brf,brf);
			
			g_pub_tx.svc_ind=1001;
			strcpy(g_pub_tx.sub_tx_code, "D099");
			
			g_pub_tx.tx_amt1=atof(fldstr[4]);
				
			strcpy(g_pub_tx.add_ind,"1"); 
			strcpy(g_pub_tx.ac_id_type,"");
			
			if ( pub_acct_trance() )
			{
				sprintf(acErrMsg,"���ô�ȡ������ʧ��!");
				WRITEMSG
				goto FAILE;;
			}
			set_zd_data("101A","01");
			set_zd_data("101B",g_pub_tx.ct_ind);
			set_zd_double("1013",g_pub_tx.tx_amt1);
			
			
			if ( pubf_acct_proc(g_pub_tx.sub_tx_code) )
			{
				sprintf(acErrMsg,"�Ǽǻ����ˮ����!");
				WRITEMSG
				goto FAILE;;
			}
			
			/******����������********/
			if(atof(fldstr[5])>0.0005)
			{
				/******��������*******/
				memset(g_pub_tx.sub_tx_code, 0x0, sizeof(g_pub_tx.sub_tx_code));
				memset(g_pub_tx.brf, 0x0, sizeof(g_pub_tx.brf));
				memset(g_pub_tx.ac_no, 0x0, sizeof(g_pub_tx.ac_no));
				memset(g_pub_tx.add_ind, 0x0, sizeof(g_pub_tx.add_ind));
				g_pub_tx.ac_id=0;
				g_pub_tx.ac_seqn=0;
				
				strcpy(g_pub_tx.ac_no, SXFKM); /* �����ѿ�Ŀ */
				g_pub_tx.tx_amt1 =atof(fldstr[5]) ;
				strcpy(g_pub_tx.ac_id_type, "9");	/* �˻�����Ϊ�ڲ� */
				strcpy(g_pub_tx.ac_get_ind, "00");
				g_pub_tx.svc_ind = 9001;
				strcpy(g_pub_tx.add_ind, "1");
	
				strcpy(g_pub_tx.sub_tx_code, "A017");
	
				set_zd_data("1214","01");
				set_zd_data("1215","2");
				set_zd_double("1218",g_pub_tx.tx_amt1);
				strcpy(g_pub_tx.brf,"����������");/***ժҪ***/
				strcpy(g_pub_tx.ct_ind,"2");/**��ת��־**/
				strcpy(g_pub_tx.ac_wrk_ind,"001001001");
				strcpy(g_pub_tx.hst_ind,"1");/***�ռ�����ϸ***/
				
				ret = pub_acct_trance();
				if (ret) {
					sprintf(acErrMsg, "���ô�ȡ������ʧ��[%s]", g_pub_tx.reply);
					WRITEMSG
					goto FAILE;
				}
				ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
				if (ret) {
					sprintf(acErrMsg, "���ô�ȡ������ʧ��[%s]", g_pub_tx.reply);
					WRITEMSG
					goto FAILE;
				}
				/********��������*************/
				memset(g_pub_tx.sub_tx_code, 0x0, sizeof(g_pub_tx.sub_tx_code));
				memset(g_pub_tx.brf, 0x0, sizeof(g_pub_tx.brf));
				memset(g_pub_tx.ac_no, 0x0, sizeof(g_pub_tx.ac_no));
				memset(g_pub_tx.add_ind, 0x0, sizeof(g_pub_tx.add_ind));
				g_pub_tx.ac_id=0;
				
				g_pub_tx.tx_amt1 =atof(fldstr[5]) ;
				strcpy(g_pub_tx.ac_get_ind, "00");
				strcpy(g_pub_tx.add_ind, "0");
				strcpy(g_pub_tx.brf,"������");/***ժҪ***/
				strcpy(g_pub_tx.ac_wrk_ind,"001001001");
				strcpy(g_pub_tx.hst_ind,"1");/***�ռ�����ϸ***/
				if(fldstr[6][0]=='0'){/******ת���˺ų�***********/
					strcpy(g_pub_tx.ac_no, fldstr[2]);
					if(in_flag==0){/***���ڻ�******/
						g_pub_tx.ac_seqn=1;
						strcpy(g_pub_tx.sub_tx_code,"D003");
						g_pub_tx.svc_ind=1001;
						strcpy(g_pub_tx.ac_id_type, "");
						
						set_zd_data("102J","01");
						set_zd_data("102K","2");
						set_zd_double("102F",g_pub_tx.tx_amt1);
					}
					else{/****�ڲ���******/
						g_pub_tx.ac_seqn=0;
						strcpy(g_pub_tx.sub_tx_code,"A016");
						g_pub_tx.svc_ind=9001;
						strcpy(g_pub_tx.ac_id_type, "9");	/* �˻�����Ϊ�ڲ� */
						set_zd_data("1204","01");
						set_zd_data("1205","2");
						set_zd_double("1208",g_pub_tx.tx_amt1);
					}
				}
				else{/***ת���˺ų�*******/
					strcpy(g_pub_tx.ac_no, fldstr[3]); 
					g_pub_tx.ac_seqn=1;
					strcpy(g_pub_tx.sub_tx_code,"D099");
					g_pub_tx.svc_ind=1001;
					strcpy(g_pub_tx.ac_id_type, "");
					
					set_zd_data("102J","01");
					set_zd_data("102K","2");
					set_zd_double("102F",g_pub_tx.tx_amt1);
				}
				
				
				ret = pub_acct_trance();
				if (ret) {
					sprintf(acErrMsg, "���ô�ȡ������ʧ��[%s]", g_pub_tx.reply);
					WRITEMSG
					goto FAILE;
				}
				ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
				if (ret) {
					sprintf(acErrMsg, "���ô�ȡ������ʧ��[%s]", g_pub_tx.reply);
					WRITEMSG
					goto FAILE;
				}
			}
			/********�����ˣ��Ǽ��������˵Ǽǲ�********/
			memset(&chnl_batch_acct,0x00,sizeof(chnl_batch_acct));
			chnl_batch_acct.tx_date=com_sys_parm.sys_date;
			sprintf(chnl_batch_acct.tx_time,"%ld",g_pub_tx.tx_time);
			strcpy(chnl_batch_acct.batch_ref,filename);
			sprintf(chnl_batch_acct.dz_ref,"%s%s",fldstr[0],fldstr[1]);
			chnl_batch_acct.trace_no=g_pub_tx.trace_no;
			strcpy(chnl_batch_acct.cur_no,"01");
			strcpy(chnl_batch_acct.ac_no,ac_no);
			strcpy(chnl_batch_acct.ac_no1,fldstr[2]);
			strcpy(chnl_batch_acct.ac_no2,fldstr[3]);
			strcpy(chnl_batch_acct.tx_code,g_pub_tx.tx_code);
			strcpy(chnl_batch_acct.tx_br_no,g_pub_tx.tx_br_no);
			strcpy(chnl_batch_acct.opn_br_no,g_pub_tx.opn_br_no);
			strcpy(chnl_batch_acct.tel,g_pub_tx.tel);
			get_zd_data("0100",chnl_batch_acct.bbmk);
			strcpy(chnl_batch_acct.ttyn,g_pub_tx.tty);
			chnl_batch_acct.pt_date=pt_date;
			get_zd_data("0170",chnl_batch_acct.pt_time);
			get_zd_data("0520",chnl_batch_acct.pt_trace_no);
			chnl_batch_acct.tx_amt=atof(fldstr[4]);
			chnl_batch_acct.ct_ind[0]='2';
			strcpy(chnl_batch_acct.brf,brf);
			chnl_batch_acct.fee_amt=atof(fldstr[5]);
			chnl_batch_acct.sts[0]='1';
			chnl_batch_acct.dz_ind[0]='0';
			ret=Chnl_batch_acct_Ins(chnl_batch_acct,g_pub_tx.reply);
			if(ret){
				sprintf(acErrMsg, "�����������˵Ǽǲ�����[%d]",ret);
				WRITEMSG
				goto FAILE;
				}
			fprintf(fp1,"%ld|%s|1|\n",com_sys_parm.sys_date,fldstr[1]);	
			sql_commit();
			sql_begin();
			continue;
	FAILE:
			vtcp_log("wkwkwk111");
		  fprintf(fp1,"%ld|%s|0|\n",com_sys_parm.sys_date,fldstr[1]);
		  vtcp_log("wkwkwk222");
		  sql_rollback();
		  vtcp_log("wkwkwk333");
		  sql_begin();
		  vtcp_log("wkwkwk444");
		  continue;
		}
		fclose(fp);
		
		if(fp1!=NULL){
			fclose(fp1);
			memset(cmd,0x0,sizeof(cmd));
			sprintf(cmd,"send_back.sh %s",filename_ret);
			ret=system(cmd);
			if(ret){
				sprintf(acErrMsg, "�ش��ļ�����[%d]",ret);
				WRITEMSG
				goto ErrExit;
				}
			set_zd_data("0830","");
			set_zd_data("0830",filename_ret);
		}
	}
OkExit:
	sql_commit();
	vtcp_log("%s,%d,����ת�˼��˳ɹ�!",__FILE__,__LINE__);
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
	vtcp_log("%s,%d,����ת�˼���ʧ��!",__FILE__,__LINE__);
	if(fp!=NULL)
	fclose(fp);
	if(fp1!=NULL)
	fclose(fp1);
	WRITEMSG
	return(1);
}
	
