/********************************************************
�������ƣ�������ѯ�˻�״̬ 
���ߣ�Cole
ʱ�䣺20130521
insert into tx_def values('7030','������ѯ�˻�״̬','10000000000000000000000001000000000000000000000100000000000000000001000000000000011000000000000000000000000000000000000000000000','1','1','2');
insert into tx_flow_def values('7030','0','7030','#$');
insert into tx_sub_def values('7030','������ѯ�˻�״̬','sp7030','0','0');
********************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#include "public.h"
#include "mdm_ac_rel_c.h"
sp7030()
{
	
	int i=0;
	int j=0;
	int k=0;
	int l=0;
	int ret=0;
	long tot_num=0;
	char cSeq[17] = {0};
	
	struct mdm_ac_rel_c sMdm_ac_rel;
	struct dd_mst_c sDd_mst;
	char filename[248];
	char filename_ret[400];
	char filename_rt[400];
	char filename_full[248];
	char fldstr[11][100];
	char tmpstr[1024];
	char ac_no[21];
	char cmd[200]={""};
	char *p;
	FILE *fp=NULL;
	FILE *fp1=NULL;
	
	memset(filename,0x0,sizeof(filename));
	memset(filename_full,0x0,sizeof(filename_full));
	memset(filename_ret,0x0,sizeof(filename_ret));
	memset(filename_rt,0x0,sizeof(filename_rt));
	memset(ac_no,0x0,sizeof(ac_no));
	memset(fldstr,0,sizeof(fldstr));
	memset(tmpstr,0,sizeof(tmpstr));

	memset(&sMdm_ac_rel	,0x00,sizeof(sMdm_ac_rel));
	
	pub_base_sysinit();
        
	/********��ȡƽ̨����***************/
	get_zd_data("0260",filename);/*****���˺�/��Ŀ********/
	get_zd_long("0480",&tot_num);/***�ܱ���***/
	pub_base_strpack_all(filename);
	
	/*******��������begin**************
	sprintf(filename,"%s/txt/hx_batch_20110809_1.txt",getenv("HOME"));
	*******��������end*********/
	
	/******ȡ�����ļ������********/
	for ( i=0;i<strlen(filename);i++ )
	{
		if(filename[i]=='_') 
		{
			j++;
			if(j==2) k=i+1;
		}
		if(filename[i]=='.')
		{
			l=i+1;
		}
	}
	vtcp_log("I��ֵ��[%d][%s][%d]",i,__FILE__,__LINE__);
	vtcp_log("J��ֵ��[%d][%s][%d]",j,__FILE__,__LINE__);
	vtcp_log("K��ֵ��[%d][%s][%d]",k,__FILE__,__LINE__);
	vtcp_log("L��ֵ��[%d][%s][%d]",l,__FILE__,__LINE__);
	memcpy(cSeq,filename+k,l-1-k);
	vtcp_log("cSeq��ֵ��[%s][%d]ȡ������ֵ[%s]",__FILE__,__LINE__,cSeq);
	
	sprintf(filename_ret,"%s/txt/CNAPS2_batch_%s.ret",getenv("HOME"),cSeq);
	sprintf(filename_rt,"CNAPS2_batch_%s.ret",cSeq);
	vtcp_log("Filename[%s][%d]�ش�ƽ̨�ļ�[%s]",__FILE__,__LINE__,filename_rt);
	
	sprintf(filename_full,"%s/txt/%s",getenv("HOME"),filename);
	vtcp_log("[%s],[%d]Filename[%s]",__FILE__,__LINE__,filename_full);
	fp = fopen( filename_full,"r" );
	if ( fp==NULL )
	{
		sprintf( acErrMsg," ��du�ļ����� [%s]  ", filename_full );
		strcpy( g_pub_tx.reply, "S047" );
		goto ErrExit;
	}
	fp1 = fopen( filename_ret,"w" );
	if ( fp1==NULL )
	{
		sprintf( acErrMsg," ��xie�ļ����� [%s]  ", filename_full );
		strcpy( g_pub_tx.reply, "S047" );
		goto ErrExit;
	}
	while(1){
		memset( tmpstr, 0x0, sizeof(tmpstr) );
		p=fgets(tmpstr,1024,fp);/*ȡ��һ����¼*/
		if(feof(fp))
		{
			vtcp_log("Filename[%s][%d]ȡ�ļ���¼��ɣ�",__FILE__,__LINE__);
			break;
		}
		pub_base_strpack_all(tmpstr);
		
		memset(fldstr,0x0,sizeof(fldstr));
		for ( i=0;i<2;i++ )
		{
			ret=pub_base_cut_str( tmpstr,fldstr[i],"|",i+1 );
			vtcp_log(" [%d] [%s] ",i+1,fldstr[i]);
			pub_base_strpack_all( fldstr[i] );
		}
		/************
		fldstr[0] ac_no 
		fldstr[1] ac_name
		fldstr[2] opn_br_no
		*******************/
		i++;
		vtcp_log("%s,%d,i=[%d]!",__FILE__,__LINE__,i);
		ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_no='%s'",fldstr[0]);
		if(ret==100)
		{
			fprintf(fp1,"%-32s|%-60s|%-4s\n",fldstr[0],fldstr[1],"AS00");/*������*/
		}
		/*ERR_DEAL*/
		else if(ret==0)
		{
			if(sMdm_ac_rel.note_sts[0]=='0')
			{
				/*��ƽ�����  ����˻�����*/
				ret = Dd_mst_Sel(g_pub_tx.reply,&sDd_mst,"ac_id=%d and ac_seqn=1",sMdm_ac_rel.ac_id);
				if (ret==100)
				{
					fprintf(fp1,"%-32s|%-60s|%-4s\n",fldstr[0],fldstr[1],"AS00");/*������*/
				}
				
				if (memcmp(fldstr[1],sDd_mst.name,strlen(sDd_mst.name)))      /*���������*/
				{
					vtcp_log("[%s],[%d],name=[%s],sDd_mst.name=[%s]���Ҵ���!",__FILE__,__LINE__,fldstr[1],sDd_mst.name);
					fprintf(fp1,"%-32s|%-60s|%-4s\n",fldstr[0],fldstr[1],"AS03");/*AS03  ��������*/
				}
				else   /*�������*/
				{
					fprintf(fp1,"%-32s|%-60s|%-4s\n",fldstr[0],fldstr[1],"AS01");/*�ѿ���*/	
				}		
			}
			else
			{	
				fprintf(fp1,"%-32s|%-60s|%-4s\n",fldstr[0],fldstr[1],"AS02");/*������*/
			}
		}
	}
	fclose(fp);
	fclose(fp1);
	if(fp1!=NULL){
		fclose(fp1);
		memset(cmd,0x0,sizeof(cmd));
		sprintf(cmd,"%s/bin/send_back.sh %s",getenv("HOME"),filename_rt);
		ret=system(cmd);
		if(ret){
			sprintf(acErrMsg, "�ش��ļ�����[%d]",ret);
			WRITEMSG
			strcpy( g_pub_tx.reply, "S047" );
			goto ErrExit;
		}
		set_zd_data("0820",filename_rt);
		vtcp_log("%s,%d,filename_rt=[%s]!",__FILE__,__LINE__,filename_rt);
	}
OkExit:
	vtcp_log("%s,%d,����ת�˼��˳ɹ�!",__FILE__,__LINE__);
	strcpy(g_pub_tx.reply,"0000");
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	set_zd_data(DC_GET_MSG,g_pub_tx.reply);
	return(0);
ErrExit:
	if(fp!=NULL)
	fclose(fp);
	if(fp1!=NULL)
	fclose(fp1);
	WRITEMSG
	return(1);
}

